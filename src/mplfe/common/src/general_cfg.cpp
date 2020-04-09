/*
 * Copyright (c) [2020] Huawei Technologies Co.,Ltd.All rights reserved.
 *
 * OpenArkCompiler is licensed under the Mulan PSL v1.
 * You can use this software according to the terms and conditions of the Mulan PSL v1.
 * You may obtain a copy of Mulan PSL v1 at:
 *
 *     http://license.coscl.org.cn/MulanPSL
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR
 * FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v1 for more details.
 */
#include "general_cfg.h"
#include "mpl_logging.h"

namespace maple {
GeneralCFG::GeneralCFG(const GeneralStmt &argStmtHead, const GeneralStmt &argStmtTail)
    : stmtHead(argStmtHead), stmtTail(argStmtTail) {}

void GeneralCFG::Init() {
  bbHead = std::make_unique<GeneralBB>(kBBKindPesudoHead);
  bbTail = std::make_unique<GeneralBB>(kBBKindPesudoTail);
  bbHead->SetNext(bbTail.get());
  bbTail->SetPrev(bbHead.get());
}

void GeneralCFG::BuildBB() {
  BuildBasicBB();
  AppendAuxStmt();
}

void GeneralCFG::BuildBasicBB() {
  const FELinkListNode *nodeStmt = stmtHead.GetNext();
  GeneralBB *currBB = nullptr;
  while (nodeStmt != nullptr) {
    const GeneralStmt *stmt = static_cast<const GeneralStmt*>(nodeStmt);
    if (stmt->GetGeneralStmtKind() == kStmtDummyEnd) {
      break;
    }
    if (stmt->IsAux() == false) {
      // check start of BB
      if (currBB == nullptr || stmt->GetGeneralStmtKind() == GeneralStmtKind::kStmtMultiIn) {
        currBB = NewBBAppend();
        bbTail->InsertBefore(currBB);
      }
      CHECK_FATAL(currBB != nullptr, "nullptr check of currBB");
      currBB->AppendStmt(stmt);
      // check end of BB
      if (stmt->IsFallThru() == false || stmt->GetGeneralStmtKind() == GeneralStmtKind::kStmtMultiOut) {
        currBB = nullptr;
      }
    }
    nodeStmt = nodeStmt->GetNext();
  }
}

void GeneralCFG::AppendAuxStmt() {
  FELinkListNode *nodeBB = bbHead->GetNext();
  while (nodeBB != nullptr && nodeBB != bbTail.get()) {
    GeneralBB *bb = static_cast<GeneralBB*>(nodeBB);
    // add pre
    FELinkListNode *nodeStmt = bb->GetStmtHead()->GetPrev();
    while (nodeStmt != nullptr) {
      GeneralStmt *stmt = static_cast<GeneralStmt*>(nodeStmt);
      if (stmt->GetGeneralStmtKind() == kStmtDummyBegin) {
        break;
      }
      if (stmt->IsAuxPre()) {
        bb->AddStmtAuxPre(stmt);
      } else {
        break;
      }
      nodeStmt = nodeStmt->GetPrev();
    }
    // add post
    nodeStmt = bb->GetStmtTail()->GetNext();
    while (nodeStmt != nullptr) {
      GeneralStmt *stmt = static_cast<GeneralStmt*>(nodeStmt);
      if (stmt->GetGeneralStmtKind() == kStmtDummyEnd) {
        break;
      }
      if (stmt->IsAuxPost()) {
        bb->AddStmtAuxPost(stmt);
      } else {
        break;
      }
      nodeStmt = nodeStmt->GetNext();
    }
    nodeBB = nodeBB->GetNext();
  }
}

GeneralBB *GeneralCFG::NewBBAppend() {
  std::unique_ptr<GeneralBB> bbNew = NewGeneralBB();
  ASSERT(bbNew != nullptr, "nullptr check for bbNew");
  listBB.push_back(std::move(bbNew));
  return listBB.back().get();
}

bool GeneralCFG::BuildCFG() {
  // build target map
  std::map<const GeneralStmt*, GeneralBB*> mapTargetStmtBB;
  FELinkListNode *nodeBB = bbHead->GetNext();
  while (nodeBB != nullptr && nodeBB != bbTail.get()) {
    GeneralBB *bb = static_cast<GeneralBB*>(nodeBB);
    const GeneralStmt *locStmtHead = bb->GetStmtNoAuxHead();
    if (locStmtHead != nullptr && locStmtHead->GetGeneralStmtKind() == GeneralStmtKind::kStmtMultiIn) {
      mapTargetStmtBB[locStmtHead] = bb;
    }
    nodeBB = nodeBB->GetNext();
  }
  // link
  nodeBB = bbHead->GetNext();
  bool firstBB = true;
  while (nodeBB != nullptr && nodeBB != bbTail.get()) {
    GeneralBB *bb = static_cast<GeneralBB*>(nodeBB);
    if (firstBB) {
      bb->AddPredBB(bbHead.get());
      bbHead->AddSuccBB(bb);
      firstBB = false;
    }
    const GeneralStmt *locStmtTail = bb->GetStmtNoAuxTail();
    CHECK_FATAL(locStmtTail != nullptr, "stmt tail is nullptr");
    if (locStmtTail->IsFallThru()) {
      FELinkListNode *nodeBBNext = nodeBB->GetNext();
      if (nodeBBNext == nullptr || nodeBBNext == bbTail.get()) {
        ERR(kLncErr, "Method without return");
        return false;
      }
      GeneralBB *bbNext = static_cast<GeneralBB*>(nodeBBNext);
      bb->AddSuccBB(bbNext);
      bbNext->AddPredBB(bb);
    }
    if (locStmtTail->GetGeneralStmtKind() == GeneralStmtKind::kStmtMultiOut) {
      for (GeneralStmt *stmt : locStmtTail->GetSuccs()) {
        auto itBB = mapTargetStmtBB.find(stmt);
        CHECK_FATAL(itBB != mapTargetStmtBB.end(), "Target BB is not found");
        GeneralBB *bbNext = itBB->second;
        bb->AddSuccBB(bbNext);
        bbNext->AddPredBB(bb);
      }
    }
    nodeBB = nodeBB->GetNext();
  }
  return true;
}

const GeneralBB *GeneralCFG::GetHeadBB() {
  currBBNode = bbHead->GetNext();
  if (currBBNode == bbTail.get()) {
    return nullptr;
  }
  return static_cast<GeneralBB*>(currBBNode);
}

const GeneralBB *GeneralCFG::GetNextBB() {
  currBBNode = currBBNode->GetNext();
  if (currBBNode == bbTail.get()) {
    return nullptr;
  }
  return static_cast<GeneralBB*>(currBBNode);
}
}  // namespace maple
