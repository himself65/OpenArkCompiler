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
#include "fe_function.h"
#include "general_bb.h"
#include "mpl_logging.h"
#include "fe_options.h"
#include "fe_manager.h"
#include "feir_var_name.h"
#include "feir_var_reg.h"
#include "mplfe_env.h"

namespace maple {
FEFunction::FEFunction(MIRFunction &argMIRFunction, const std::unique_ptr<FEFunctionPhaseResult> &argPhaseResultTotal)
    : genStmtHead(nullptr),
      genStmtTail(nullptr),
      genBBHead(nullptr),
      genBBTail(nullptr),
      feirStmtHead(nullptr),
      feirStmtTail(nullptr),
      feirBBHead(nullptr),
      feirBBTail(nullptr),
      phaseResult(FEOptions::GetInstance().IsDumpPhaseTimeDetail() || FEOptions::GetInstance().IsDumpPhaseTime()),
      phaseResultTotal(argPhaseResultTotal),
      mirFunction(argMIRFunction) {
}

FEFunction::~FEFunction() {
  genStmtHead = nullptr;
  genStmtTail = nullptr;
  genBBHead = nullptr;
  genBBTail = nullptr;
  feirStmtHead = nullptr;
  feirStmtTail = nullptr;
  feirBBHead = nullptr;
  feirBBTail = nullptr;
}

void FEFunction::InitImpl() {
  // general stmt/bb
  genStmtHead = RegisterGeneralStmt(std::make_unique<GeneralStmt>(GeneralStmtKind::kStmtDummyBegin));
  genStmtTail = RegisterGeneralStmt(std::make_unique<GeneralStmt>(GeneralStmtKind::kStmtDummyEnd));
  genStmtHead->SetNext(genStmtTail);
  genStmtTail->SetPrev(genStmtHead);
  genBBHead = RegisterGeneralBB(std::make_unique<GeneralBB>(GeneralBBKind::kBBKindPesudoHead));
  genBBTail = RegisterGeneralBB(std::make_unique<GeneralBB>(GeneralBBKind::kBBKindPesudoTail));
  genBBHead->SetNext(genBBTail);
  genBBTail->SetPrev(genBBHead);
  // feir stmt/bb
  feirStmtHead = RegisterFEIRStmt(std::make_unique<FEIRStmt>(GeneralStmtKind::kStmtDummyBegin,
                                                             FEIRNodeKind::kStmtPesudoFuncStart));
  feirStmtTail = RegisterFEIRStmt(std::make_unique<FEIRStmt>(GeneralStmtKind::kStmtDummyEnd,
                                                             FEIRNodeKind::kStmtPesudoFuncEnd));
  feirStmtHead->SetNext(feirStmtTail);
  feirStmtTail->SetPrev(feirStmtHead);
  feirBBHead = RegisterFEIRBB(std::make_unique<GeneralBB>(GeneralBBKind::kBBKindPesudoHead));
  feirBBTail = RegisterFEIRBB(std::make_unique<GeneralBB>(GeneralBBKind::kBBKindPesudoTail));
  feirBBHead->SetNext(feirBBTail);
  feirBBTail->SetPrev(feirBBHead);
}

GeneralStmt *FEFunction::RegisterGeneralStmt(std::unique_ptr<GeneralStmt> stmt) {
  genStmtList.push_back(std::move(stmt));
  return genStmtList.back().get();
}

const std::unique_ptr<GeneralStmt> &FEFunction::RegisterGeneralStmtUniqueReturn(std::unique_ptr<GeneralStmt> stmt) {
  genStmtList.push_back(std::move(stmt));
  return genStmtList.back();
}

GeneralBB *FEFunction::RegisterGeneralBB(std::unique_ptr<GeneralBB> bb) {
  genBBList.push_back(std::move(bb));
  return genBBList.back().get();
}

FEIRStmt *FEFunction::RegisterFEIRStmt(UniqueFEIRStmt stmt) {
  feirStmtList.push_back(std::move(stmt));
  return feirStmtList.back().get();
}

GeneralBB *FEFunction::RegisterFEIRBB(std::unique_ptr<GeneralBB> bb) {
  feirBBList.push_back(std::move(bb));
  return feirBBList.back().get();
}

GeneralBB *FEFunction::NewGeneralBB() {
  return new GeneralBB();
}

GeneralBB *FEFunction::NewGeneralBB(uint8 argBBKind) {
  return new GeneralBB(argBBKind);
}

bool FEFunction::BuildGeneralBB(const std::string &phaseName) {
  phaseResult.RegisterPhaseNameAndStart(phaseName);
  bool success = phaseResult.IsSuccess();
  if (!success) {
    return phaseResult.Finish(success);
  }
  // Build BB
  FELinkListNode *nodeStmt = genStmtHead->GetNext();
  GeneralBB *currBB = nullptr;
  while (nodeStmt != nullptr && nodeStmt != genStmtTail) {
    GeneralStmt *stmt = static_cast<GeneralStmt*>(nodeStmt);
    if (stmt->IsAux() == false) {
      // check start of BB
      if (currBB == nullptr || stmt->GetGeneralStmtKind() == GeneralStmtKind::kStmtMultiIn) {
        currBB = NewGeneralBB();
        genBBTail->InsertBefore(currBB);
      }
      CHECK_FATAL(currBB != nullptr, "nullptr check for currBB");
      currBB->AppendStmt(stmt);
      // check end of BB
      if (stmt->IsFallThru() == false || stmt->GetGeneralStmtKind() == GeneralStmtKind::kStmtMultiOut) {
        currBB = nullptr;
      }
    }
    nodeStmt = nodeStmt->GetNext();
  }
  // Add Aux Stmt
  FELinkListNode *nodeBB = genBBHead->GetNext();
  while (nodeBB != nullptr && nodeBB != genBBTail) {
    GeneralBB *bb = static_cast<GeneralBB*>(nodeBB);
    // add pre
    nodeStmt = bb->GetStmtHead()->GetPrev();
    while (nodeStmt != nullptr && nodeStmt != genStmtHead) {
      const GeneralStmt *stmt = static_cast<const GeneralStmt*>(nodeStmt);
      if (stmt->IsAuxPre()) {
        bb->AddStmtAuxPre(stmt);
      } else {
        break;
      }
      nodeStmt = nodeStmt->GetPrev();
    }
    // add post
    nodeStmt = bb->GetStmtTail()->GetNext();
    while (nodeStmt != nullptr && nodeStmt != genStmtTail) {
      GeneralStmt *stmt = static_cast<GeneralStmt*>(nodeStmt);
      if (stmt->IsAuxPost()) {
        bb->AddStmtAuxPost(stmt);
      } else {
        break;
      }
      nodeStmt = nodeStmt->GetNext();
    }
    nodeBB = nodeBB->GetNext();
  }
  return phaseResult.Finish(success);
}

bool FEFunction::BuildGeneralCFG(const std::string &phaseName) {
  phaseResult.RegisterPhaseNameAndStart(phaseName);
  bool success = phaseResult.IsSuccess();
  if (!success) {
    return phaseResult.Finish(success);
  }
  // build target map
  std::map<const GeneralStmt*, GeneralBB*> mapTargetStmtBB;
  FELinkListNode *nodeBB = genBBHead->GetNext();
  while (nodeBB != nullptr && nodeBB != genBBTail) {
    GeneralBB *bb = static_cast<GeneralBB*>(nodeBB);
    const GeneralStmt *stmtHead = bb->GetStmtNoAuxHead();
    if (stmtHead != nullptr && stmtHead->GetGeneralStmtKind() == GeneralStmtKind::kStmtMultiIn) {
      mapTargetStmtBB[stmtHead] = bb;
    }
    nodeBB = nodeBB->GetNext();
  }
  // link
  nodeBB = genBBHead->GetNext();
  bool firstBB = true;
  while (nodeBB != nullptr && nodeBB != genBBTail) {
    GeneralBB *bb = static_cast<GeneralBB*>(nodeBB);
    if (firstBB) {
      bb->AddPredBB(genBBHead);
      genBBHead->AddSuccBB(bb);
      firstBB = false;
    }
    const GeneralStmt *stmtTail = bb->GetStmtNoAuxTail();
    CHECK_FATAL(stmtTail != nullptr, "stmt tail is nullptr");
    if (stmtTail->IsFallThru()) {
      FELinkListNode *nodeBBNext = nodeBB->GetNext();
      if (nodeBBNext == nullptr || nodeBBNext == genBBTail) {
        ERR(kLncErr, "Method without return");
        return phaseResult.Finish(false);
      }
      GeneralBB *bbNext = static_cast<GeneralBB*>(nodeBBNext);
      bb->AddSuccBB(bbNext);
      bbNext->AddPredBB(bb);
    }
    if (stmtTail->GetGeneralStmtKind() == GeneralStmtKind::kStmtMultiOut) {
      for (GeneralStmt *stmt : stmtTail->GetSuccs()) {
        auto itBB = mapTargetStmtBB.find(stmt);
        CHECK_FATAL(itBB != mapTargetStmtBB.end(), "Target BB is not found");
        GeneralBB *bbNext = itBB->second;
        bb->AddSuccBB(bbNext);
        bbNext->AddPredBB(bb);
      }
    }
    nodeBB = nodeBB->GetNext();
  }
  return phaseResult.Finish(success);
}

bool FEFunction::CheckDeadBB(const std::string &phaseName) {
  phaseResult.RegisterPhaseNameAndStart(phaseName);
  bool success = phaseResult.IsSuccess();
  if (!success) {
    return phaseResult.Finish(success);
  }
  uint32 nDeadBB = 0;
  FELinkListNode *nodeBB = genBBHead->GetNext();
  while (nodeBB != nullptr && nodeBB != genBBTail) {
    GeneralBB *bb = static_cast<GeneralBB*>(nodeBB);
    ASSERT(bb != nullptr, "nullptr check");
    if (bb->IsDead()) {
      nDeadBB++;
    }
    nodeBB = nodeBB->GetNext();
  }
  if (nDeadBB > 0) {
    ERR(kLncErr, "Dead BB existed");
    success = false;
  }
  return phaseResult.Finish(success);
}

void FEFunction::LabelGenStmt() {
  FELinkListNode *nodeStmt = genStmtHead;
  uint32 idx = 0;
  while (nodeStmt != nullptr) {
    GeneralStmt *stmt = static_cast<GeneralStmt*>(nodeStmt);
    stmt->SetID(idx);
    idx++;
    nodeStmt = nodeStmt->GetNext();
  }
}

void FEFunction::LabelGenBB() {
  FELinkListNode *nodeBB = genBBHead;
  uint32 idx = 0;
  while (nodeBB != nullptr) {
    GeneralBB *bb = static_cast<GeneralBB*>(nodeBB);
    bb->SetID(idx);
    idx++;
    nodeBB = nodeBB->GetNext();
  }
}

bool FEFunction::HasDeadBB() {
  FELinkListNode *nodeBB = genBBHead->GetNext();
  while (nodeBB != nullptr && nodeBB != genBBTail) {
    GeneralBB *bb = static_cast<GeneralBB*>(nodeBB);
    if (bb->IsDead()) {
      return true;
    }
    nodeBB = nodeBB->GetNext();
  }
  return false;
}

void FEFunction::DumpGeneralStmts() {
  FELinkListNode *nodeStmt = genStmtHead;
  while (nodeStmt != nullptr) {
    GeneralStmt *stmt = static_cast<GeneralStmt*>(nodeStmt);
    stmt->Dump();
    nodeStmt = nodeStmt->GetNext();
  }
}

void FEFunction::DumpGeneralBBs() {
  FELinkListNode *nodeBB = genBBHead->GetNext();
  while (nodeBB != nullptr && nodeBB != genBBTail) {
    GeneralBB *bb = static_cast<GeneralBB*>(nodeBB);
    bb->Dump();
    nodeBB = nodeBB->GetNext();
  }
}

void FEFunction::DumpGeneralCFGGraph() {
  MPLFE_PARALLEL_FORBIDDEN();
  if (!FEOptions::GetInstance().IsDumpGeneralCFGGraph()) {
    return;
  }
  std::string fileName = FEOptions::GetInstance().GetJBCCFGGraphFileName();
  CHECK_FATAL(!fileName.empty(), "General CFG Graph FileName is empty");
  std::ofstream file(fileName);
  CHECK_FATAL(file.is_open(), "Failed to open General CFG Graph FileName: %s", fileName.c_str());
  file << "digraph {" << std::endl;
  file << "  # /* " << GetGeneralFuncName() << " */" << std::endl;
  FELinkListNode *nodeBB = genBBHead->GetNext();
  while (nodeBB != nullptr && nodeBB != genBBTail) {
    GeneralBB *bb = static_cast<GeneralBB*>(nodeBB);
    DumpGeneralCFGGraphForBB(file, *bb);
    nodeBB = nodeBB->GetNext();
  }
  DumpGeneralCFGGraphForCFGEdge(file);
  DumpGeneralCFGGraphForDFGEdge(file);
  file << "}" << std::endl;
  file.close();
}

void FEFunction::DumpGeneralCFGGraphForBB(std::ofstream &file, const GeneralBB &bb) {
  file << "  BB" << bb.GetID() << " [shape=record,label=\"{" << std::endl;
  const FELinkListNode *nodeStmt = bb.GetStmtHead();
  while (nodeStmt != nullptr) {
    const GeneralStmt *stmt = static_cast<const GeneralStmt*>(nodeStmt);
    file << "      " << stmt->DumpDotString();
    if (nodeStmt == bb.GetStmtTail()) {
      file << std::endl;
      break;
    } else {
      file << " |" << std::endl;
    }
    nodeStmt = nodeStmt->GetNext();
  }
  file << "    }\"];" << std::endl;
}

void FEFunction::DumpGeneralCFGGraphForCFGEdge(std::ofstream &file) {
  file << "  subgraph cfg_edges {" << std::endl;
  file << "    edge [color=\"#000000\",weight=0.3,len=3];" << std::endl;
  const FELinkListNode *nodeBB = genBBHead->GetNext();
  while (nodeBB != nullptr && nodeBB != genBBTail) {
    const GeneralBB *bb = static_cast<const GeneralBB*>(nodeBB);
    const GeneralStmt *stmtS = bb->GetStmtTail();
    for (GeneralBB *bbNext : bb->GetSuccBBs()) {
      const GeneralStmt *stmtE = bbNext->GetStmtHead();
      file << "    BB" << bb->GetID() << ":stmt" << stmtS->GetID() << " -> ";
      file << "BB" << bbNext->GetID() << ":stmt" << stmtE->GetID() << std::endl;
    }
    nodeBB = nodeBB->GetNext();
  }
  file << "  }" << std::endl;
}

void FEFunction::DumpGeneralCFGGraphForDFGEdge(std::ofstream &file) {
  file << "  subgraph cfg_edges {" << std::endl;
  file << "    edge [color=\"#00FF00\",weight=0.3,len=3];" << std::endl;
  file << "  }" << std::endl;
}

bool FEFunction::BuildGeneralStmtBBMap(const std::string &phaseName) {
  phaseResult.RegisterPhaseNameAndStart(phaseName);
  FELinkListNode *nodeBB = genBBHead->GetNext();
  while (nodeBB != nullptr && nodeBB != genBBTail) {
    GeneralBB *bb = static_cast<GeneralBB*>(nodeBB);
    const FELinkListNode *nodeStmt = bb->GetStmtHead();
    while (nodeStmt != nullptr) {
      const GeneralStmt *stmt = static_cast<const GeneralStmt*>(nodeStmt);
      genStmtBBMap[stmt] = bb;
      if (nodeStmt == bb->GetStmtTail()) {
        break;
      }
      nodeStmt = nodeStmt->GetNext();
    }
    nodeBB = nodeBB->GetNext();
  }
  return phaseResult.Finish();
}

bool FEFunction::LabelGeneralStmts(const std::string &phaseName) {
  phaseResult.RegisterPhaseNameAndStart(phaseName);
  uint32 idx = 0;
  FELinkListNode *nodeStmt = genStmtHead;
  while (nodeStmt != nullptr) {
    GeneralStmt *stmt = static_cast<GeneralStmt*>(nodeStmt);
    stmt->SetID(idx);
    idx++;
    nodeStmt = nodeStmt->GetNext();
  }
  return phaseResult.Finish();
}

bool FEFunction::LabelGeneralBBs(const std::string &phaseName) {
  phaseResult.RegisterPhaseNameAndStart(phaseName);
  uint32 idx = 0;
  FELinkListNode *nodeBB = genBBHead->GetNext();
  while (nodeBB != nullptr && nodeBB != genBBTail) {
    GeneralBB *bb = static_cast<GeneralBB*>(nodeBB);
    bb->SetID(idx);
    idx++;
    nodeBB = nodeBB->GetNext();
  }
  return phaseResult.Finish();
}

std::string FEFunction::GetGeneralFuncName() const {
  return mirFunction.GetName();
}

void FEFunction::PhaseTimerStart(FETimerNS &timer) {
  if (!FEOptions::GetInstance().IsDumpPhaseTime()) {
    return;
  }
  timer.Start();
}

void FEFunction::PhaseTimerStopAndDump(FETimerNS &timer, const std::string &label) {
  if (!FEOptions::GetInstance().IsDumpPhaseTime()) {
    return;
  }
  timer.Stop();
  INFO(kLncInfo, "[PhaseTime]   %s: %lld ns", label.c_str(), timer.GetTimeNS());
}

bool FEFunction::UpdateFormal(const std::string &phaseName) {
  phaseResult.RegisterPhaseNameAndStart(phaseName);
  MPLFE_PARALLEL_FORBIDDEN();
  uint32 idx = 0;
  mirFunction.ClearFormals();
  FEManager::GetMIRBuilder().SetCurrentFunction(mirFunction);
  for (const std::unique_ptr<FEIRVar> &argVar : argVarList) {
    MIRSymbol *sym = nullptr;
    if (idx == 0 && HasThis()) {
      GStrIdx thisNameIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName("_this");
      std::unique_ptr<FEIRVar> varThis = std::make_unique<FEIRVarName>(thisNameIdx, argVar->GetType()->Clone());
      sym = varThis->GenerateMIRSymbol(FEManager::GetMIRBuilder());
    } else {
      sym = argVar->GenerateMIRSymbol(FEManager::GetMIRBuilder());
    }
    mirFunction.AddFormal(sym);
    idx++;
  }
  return phaseResult.Finish();
}

bool FEFunction::EmitToMIR(const std::string &phaseName) {
  phaseResult.RegisterPhaseNameAndStart(phaseName);
  mirFunction.NewBody();
  FEManager::GetMIRBuilder().SetCurrentFunction(mirFunction);
  BuildMapLabelIdx();
  EmitToMIRStmt();
  return phaseResult.Finish();
}

bool FEFunction::ReleaseGenStmts(const std::string &phaseName) {
  phaseResult.RegisterPhaseNameAndStart(phaseName);
  if (FEOptions::GetInstance().IsReleaseAfterEmit()) {
    genStmtList.clear();
  }
  return phaseResult.Finish();
}

void FEFunction::BuildMapLabelIdx() {
  FELinkListNode *nodeStmt = feirStmtHead->GetNext();
  while (nodeStmt != nullptr && nodeStmt != feirStmtTail) {
    FEIRStmt *stmt = static_cast<FEIRStmt*>(nodeStmt);
    if (stmt->GetKind() == FEIRNodeKind::kStmtPesudoLabel) {
      FEIRStmtPesudoLabel *stmtLabel = static_cast<FEIRStmtPesudoLabel*>(stmt);
      stmtLabel->GenerateLabelIdx(FEManager::GetMIRBuilder());
      mapLabelIdx[stmtLabel->GetLabelIdx()] = stmtLabel->GetMIRLabelIdx();
    }
    nodeStmt = nodeStmt->GetNext();
  }
}

bool FEFunction::ProcessFEIRFunction() {
  bool success = true;
  success = success && BuildMapLabelStmt("fe/build map label stmt");
  success = success && SetupFEIRStmtJavaTry("fe/setup stmt javatry");
  success = success && SetupFEIRStmtBranch("fe/setup stmt branch");
  return success;
}

bool FEFunction::BuildMapLabelStmt(const std::string &phaseName) {
  phaseResult.RegisterPhaseNameAndStart(phaseName);
  FELinkListNode *nodeStmt = feirStmtHead->GetNext();
  while (nodeStmt != nullptr && nodeStmt != feirStmtTail) {
    FEIRStmt *stmt = static_cast<FEIRStmt*>(nodeStmt);
    FEIRNodeKind kind = stmt->GetKind();
    switch (kind) {
      case FEIRNodeKind::kStmtPesudoLabel:
      case FEIRNodeKind::kStmtPesudoJavaCatch: {
        FEIRStmtPesudoLabel *stmtLabel = static_cast<FEIRStmtPesudoLabel*>(stmt);
        mapLabelStmt[stmtLabel->GetLabelIdx()] = stmtLabel;
        break;
      }
      default:
        break;
    }
    nodeStmt = nodeStmt->GetNext();
  }
  return phaseResult.Finish();
}

bool FEFunction::SetupFEIRStmtJavaTry(const std::string &phaseName) {
  phaseResult.RegisterPhaseNameAndStart(phaseName);
  FELinkListNode *nodeStmt = feirStmtHead->GetNext();
  while (nodeStmt != nullptr && nodeStmt != feirStmtTail) {
    FEIRStmt *stmt = static_cast<FEIRStmt*>(nodeStmt);
    if (stmt->GetKind() == FEIRNodeKind::kStmtPesudoJavaTry) {
      FEIRStmtPesudoJavaTry *stmtJavaTry = static_cast<FEIRStmtPesudoJavaTry*>(stmt);
      for (uint32 labelIdx : stmtJavaTry->GetCatchLabelIdxVec()) {
        auto it = mapLabelStmt.find(labelIdx);
        CHECK_FATAL(it != mapLabelStmt.end(), "label is not found");
        stmtJavaTry->AddCatchTarget(it->second);
      }
    }
    nodeStmt = nodeStmt->GetNext();
  }
  return phaseResult.Finish();
}

bool FEFunction::SetupFEIRStmtBranch(const std::string &phaseName) {
  phaseResult.RegisterPhaseNameAndStart(phaseName);
  bool success = true;
  FELinkListNode *nodeStmt = feirStmtHead->GetNext();
  while (nodeStmt != nullptr && nodeStmt != feirStmtTail) {
    FEIRStmt *stmt = static_cast<FEIRStmt*>(nodeStmt);
    FEIRNodeKind kind = stmt->GetKind();
    switch (kind) {
      case FEIRNodeKind::kStmtGoto:
      case FEIRNodeKind::kStmtCondGoto:
        success = success && SetupFEIRStmtGoto(*(static_cast<FEIRStmtGoto*>(stmt)));
        break;
      case FEIRNodeKind::kStmtSwitch:
        success = success && SetupFEIRStmtSwitch(*(static_cast<FEIRStmtSwitch*>(stmt)));
        break;
      default:
        break;
    }
    nodeStmt = nodeStmt->GetNext();
  }
  return phaseResult.Finish(success);
}

bool FEFunction::SetupFEIRStmtGoto(FEIRStmtGoto &stmt) {
  auto it = mapLabelStmt.find(stmt.GetLabelIdx());
  if (it == mapLabelStmt.end()) {
    ERR(kLncErr, "target not found for stmt goto");
    return false;
  }
  stmt.SetStmtTarget(it->second);
  return true;
}

bool FEFunction::SetupFEIRStmtSwitch(FEIRStmtSwitch &stmt) {
  // default target
  auto itDefault = mapLabelStmt.find(stmt.GetDefaultLabelIdx());
  if (itDefault == mapLabelStmt.end()) {
    ERR(kLncErr, "target not found for stmt goto");
    return false;
  }
  stmt.SetDefaultTarget(itDefault->second);

  // value targets
  for (const auto &itItem : stmt.GetMapValueLabelIdx()) {
    auto itTarget = mapLabelStmt.find(itItem.second);
    if (itTarget == mapLabelStmt.end()) {
      ERR(kLncErr, "target not found for stmt goto");
      return false;
    }
    stmt.AddTarget(itItem.first, itTarget->second);
  }
  return true;
}

void FEFunction::EmitToMIRStmt() {
  FELinkListNode *nodeStmt = feirStmtHead->GetNext();
  while (nodeStmt != nullptr && nodeStmt != feirStmtTail) {
    FEIRStmt *stmt = static_cast<FEIRStmt*>(nodeStmt);
    std::list<StmtNode*> mirStmts = stmt->GenMIRStmts(FEManager::GetMIRBuilder());
    for (StmtNode *mirStmt : mirStmts) {
      FEManager::GetMIRBuilder().AddStmtInCurrentFunctionBody(*mirStmt);
    }
    nodeStmt = nodeStmt->GetNext();
  }
}
}  // namespace maple
