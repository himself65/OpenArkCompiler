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
#include "jbc_bb.h"
#include "jbc_stmt.h"
#include "jbc_function.h"

namespace maple {
JBCBB::JBCBB(const jbc::JBCConstPool &argConstPool)
    : JBCBB(GeneralBBKind::kBBKindDefault, argConstPool) {}

JBCBB::JBCBB(uint8 argBBKind, const jbc::JBCConstPool &argConstPool)
    : GeneralBB(argBBKind),
      constPool(argConstPool),
      stackError(false),
      stackInUpdated(false),
      stackOutUpdated(false),
      updatePredEnd(false) {}

bool JBCBB::InitForFuncHeader() {
  updatePredEnd = true;
  stackInUpdated = true;
  return UpdateStack();
}

bool JBCBB::InitForCatch() {
  updatePredEnd = true;
  stackInUpdated = true;
  minStackIn.PushItem(jbc::JBCPrimType::kTypeRef);
  return UpdateStack();
}

bool JBCBB::UpdateStack() {
  minStackOut.CopyFrom(minStackIn);
  const JBCStmt *stmt = static_cast<const JBCStmt*>(stmtHead);
  while (stmt != nullptr) {
    JBCStmtKind kind = stmt->GetKind();
    if (kind == JBCStmtKind::kJBCStmtInst) {
      const JBCStmtInst *stmtInst = static_cast<const JBCStmtInst*>(stmt);
      const jbc::JBCOp &op = stmtInst->GetOp();
      stackError = stackError || (!minStackOut.StackChange(op, constPool));
    } else if (kind == JBCStmtKind::kJBCStmtInstBranch) {
      const JBCStmtInstBranch *stmtInstBranch = static_cast<const JBCStmtInstBranch*>(stmt);
      const jbc::JBCOp &op = stmtInstBranch->GetOp();
      stackError = stackError || (!minStackOut.StackChange(op, constPool));
    }
    if (stackError) {
      return false;
    }
    if (stmt == stmtTail) {
      break;
    }
    const FELinkListNode *node = stmt->GetNext();
    stmt = static_cast<const JBCStmt*>(node);
  }
  stackOutUpdated = true;
  return true;
}

bool JBCBB::UpdateStackByPredBB(const JBCBB &bb) {
  if (bb.stackError) {
    return false;
  }
  if (!bb.stackOutUpdated) {
    return true;
  }
  if (!stackInUpdated) {
    minStackIn.CopyFrom(bb.minStackOut);
    stackInUpdated = true;
    return UpdateStack();
  }
  if (!minStackIn.EqualTo(bb.minStackOut)) {
    stackError = true;
    return false;
  }
  return true;
}

bool JBCBB::UpdateStackByPredBBEnd() const {
  return updatePredEnd;
}

bool JBCBB::CheckStack() {
  if (!stackInUpdated || !stackOutUpdated) {
    return false;
  }
  if (stackError) {
    return false;
  }
  for (GeneralBB *bb : GetPredBBs()) {
    if (bb->GetBBKind() == GeneralBBKind::kBBKindPesudoHead) {
      continue;
    }
    if (bb->GetBBKind() == JBCBBPesudoCatchPred::kBBKindPesudoCatchPred) {
      continue;
    }
    JBCBB *jbcBB = static_cast<JBCBB*>(bb);
    if (!minStackIn.EqualTo(jbcBB->minStackOut)) {
      return false;
    }
  }
  return true;
}

void JBCBB::DumpImpl() const {
  std::cout << "GeneralBB (id=" << id << ", kind=" << GetBBKindName() <<
               ", preds={";
  for (GeneralBB *bb : predBBs) {
    if (bb->GetBBKind() == GeneralBBKind::kBBKindPesudoHead) {
      std::cout << "FuncHead ";
    } else if (bb->GetBBKind() == JBCBBPesudoCatchPred::kBBKindPesudoCatchPred) {
      std::cout << "CatchPred ";
    } else {
      std::cout << bb->GetID() << " ";
    }
  }
  std::cout << "}, succs={";
  for (GeneralBB *bb : succBBs) {
    std::cout << bb->GetID() << " ";
  }
  std::cout << "})" << std::endl;
  std::cout << "  StackIn (" << (stackInUpdated ? "updated" : "") << "): ";
  minStackIn.Dump();
  std::cout << std::endl;
  const FELinkListNode *nodeStmt = stmtHead;
  while (nodeStmt != nullptr) {
    const GeneralStmt *stmt = static_cast<const GeneralStmt*>(nodeStmt);
    stmt->Dump("  ");
    if (nodeStmt == stmtTail) {
      break;
    }
    nodeStmt = nodeStmt->GetNext();
  }
  std::cout << "  StackOut (" << (stackOutUpdated ? "updated" : "") << "): ";
  minStackOut.Dump();
  std::cout << std::endl;
}

uint32 JBCBB::GetSwapSize() const {
  uint32 sizeIn = minStackIn.GetStackSize();
  uint32 sizeOut = minStackOut.GetStackSize();
  return sizeIn > sizeOut ? sizeIn : sizeOut;
}
}  // namespace maple
