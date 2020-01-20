/*
 * Copyright (c) [2019] Huawei Technologies Co.,Ltd.All rights reserved.
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
#include "bb.h"
#include "mempool_allocator.h"
#include "ver_symbol.h"
#include "me_ssa.h"
#include "me_ir.h"

namespace maple {
std::string BB::StrAttribute() const {
  switch (kind) {
    case kBBUnknown:
      return "unknown";
    case kBBCondGoto:
      return "condgoto";
    case kBBGoto:
      return "goto";
    case kBBFallthru:
      return "fallthru";
    case kBBReturn:
      return "return";
    case kBBAfterGosub:
      return "aftergosub";
    case kBBSwitch:
      return "switch";
    case kBBInvalid:
      return "invalid";
    default:
      CHECK_FATAL(false, "not yet implemented");
  }
  return "NYI";
}

void BB::DumpBBAttribute(MIRModule *mod) const {
  if (GetAttributes(kBBAttrIsEntry)) {
    mod->GetOut() << " Entry ";
  }
  if (GetAttributes(kBBAttrIsExit)) {
    mod->GetOut() << " Exit ";
  }
  if (GetAttributes(kBBAttrIsTry)) {
    mod->GetOut() << " Try ";
  }
  if (GetAttributes(kBBAttrIsTryEnd)) {
    mod->GetOut() << " Tryend ";
  }
  if (GetAttributes(kBBAttrIsJSCatch)) {
    mod->GetOut() << " JSCatch ";
  }
  if (GetAttributes(kBBAttrIsJSFinally)) {
    mod->GetOut() << " JSFinally ";
  }
  if (GetAttributes(kBBAttrIsJavaFinally)) {
    mod->GetOut() << " Catch::Finally ";
  } else if (GetAttributes(kBBAttrIsCatch)) {
    mod->GetOut() << " Catch ";
  }
  if (GetAttributes(kBBAttrIsInLoop)) {
    mod->GetOut() << " InLoop ";
  }
}

void BB::DumpHeader(MIRModule *mod) const {
  mod->GetOut() << "============BB id:" << GetBBId() << " " << StrAttribute() << " [";
  DumpBBAttribute(mod);
  mod->GetOut() << "]===============\n";
  mod->GetOut() << "preds: ";
  for (const auto &predElement : pred) {
    mod->GetOut() << predElement->GetBBId() << " ";
  }
  mod->GetOut() << "\nsuccs: ";
  for (const auto &succElement : succ) {
    mod->GetOut() << succElement->GetBBId() << " ";
  }
  mod->GetOut() << '\n';
  if (bbLabel != 0) {
    static LabelNode lblNode;
    lblNode.SetLabelIdx(bbLabel);
    lblNode.Dump(*mod, 0);
    mod->GetOut() << '\n';
  }
}

void BB::Dump(MIRModule *mod) {
  DumpHeader(mod);
  DumpPhi();
  for (auto &stmt : stmtNodeList) {
    stmt.Dump(*mod, 1);
  }
}

void BB::DumpPhi() {
  for (auto &phi : phiList) {
    phi.second.Dump();
  }
}

const PhiNode *BB::PhiofVerStInserted(const VersionSt &versionSt) const {
  auto phiIt = phiList.find(versionSt.GetOrigSt());
  return (phiIt != phiList.end()) ? &(phiIt->second) : nullptr;
}

void BB::InsertPhi(MapleAllocator *alloc, VersionSt *versionSt) {
  PhiNode phiNode(*alloc, *versionSt);
  for (auto prevIt = pred.begin(); prevIt != pred.end(); ++prevIt) {
    phiNode.GetPhiOpnds().push_back(versionSt);
  }
  phiList.insert(std::make_pair(versionSt->GetOrigSt(), phiNode));
}

bool BB::IsInList(const MapleVector<BB*> &bbList) const {
  for (const auto &bb : bbList) {
    if (bb == this) {
      return true;
    }
  }
  return false;
}

// remove the given bb from the BB vector bbVec; nothing done if not found
int BB::RemoveBBFromVector(MapleVector<BB*> &bbVec) const {
  size_t i = 0;
  while (i < bbVec.size()) {
    if (bbVec[i] == this) {
      break;
    }
    ++i;
  }
  if (i == bbVec.size()) {
    // bb not in the vector
    return -1;
  }
  bbVec.erase(bbVec.cbegin() + i);
  return i;
}

void BB::RemoveBBFromPred(BB *bb) {
  int index = bb->RemoveBBFromVector(pred);
  ASSERT(index != -1, "-1 is a very large number in BB::RemoveBBFromPred");
  for (auto &phi : phiList) {
    ASSERT(phi.second.GetPhiOpnds().size() > index, "index out of range in BB::RemoveBBFromPred");
    phi.second.GetPhiOpnds().erase(phi.second.GetPhiOpnds().cbegin() + index);
  }
  for (auto &phi : meVarPhiList) {
    ASSERT(phi.second->GetOpnds().size() > index, "index out of range in BB::RemoveBBFromPred");
    phi.second->GetOpnds().erase(phi.second->GetOpnds().cbegin() + index);
  }
  for (auto &phi : meRegPhiList) {
    ASSERT(phi.second->GetOpnds().size() > index, "index out of range in BB::RemoveBBFromPred");
    phi.second->GetOpnds().erase(phi.second->GetOpnds().cbegin() + index);
  }
}

void BB::RemoveBBFromSucc(BB *bb) {
  bb->RemoveBBFromVector(succ);
}

// add stmtnode to bb and update first_stmt_ and last_stmt_
void BB::AddStmtNode(StmtNode *stmt) {
  CHECK_FATAL(stmt != nullptr, "null ptr check");
  stmtNodeList.push_back(stmt);
}

// add stmtnode at beginning of bb and update first_stmt_ and last_stmt_
void BB::PrependStmtNode(StmtNode *stmt) {
  stmtNodeList.push_front(stmt);
}

void BB::PrependMeStmt(MeStmt *meStmt) {
  meStmtList.push_front(meStmt);
  meStmt->SetBB(this);
}

// if the bb contains only one stmt other than comment, return that stmt
// otherwise return nullptr
StmtNode *BB::GetTheOnlyStmtNode() {
  StmtNode *onlyStmtNode = nullptr;
  for (auto &stmtNode : stmtNodeList) {
    if (stmtNode.GetOpCode() == OP_comment) {
      continue;
    }
    if (onlyStmtNode != nullptr) {
      return nullptr;
    }
    onlyStmtNode = &stmtNode;
  }
  return onlyStmtNode;
}

// warning: if stmt is not in the bb, this will cause undefined behavior
void BB::RemoveStmtNode(StmtNode *stmt) {
  CHECK_FATAL(stmt != nullptr, "null ptr check");
  stmtNodeList.erase(StmtNodes::iterator(stmt));
}

void BB::InsertStmtBefore(StmtNode *stmt, StmtNode *newStmt) {
  CHECK_FATAL(newStmt != nullptr, "null ptr check");
  CHECK_FATAL(stmt != nullptr, "null ptr check");
  stmtNodeList.insert(stmt, newStmt);
}

void BB::ReplaceStmt(StmtNode *stmt, StmtNode *newStmt) {
  InsertStmtBefore(stmt, newStmt);
  RemoveStmtNode(stmt);
}

// delete last_stmt_ and update
void BB::RemoveLastStmt() {
  stmtNodeList.pop_back();
}

// replace pred with newbb to keep position and add this as succ of newpred
void BB::ReplacePred(const BB *old, BB *newPred) {
  for (auto &predElement : pred) {
    if (predElement == old) {
      predElement = newPred;
      newPred->succ.push_back(this);
      break;
    }
  }
}

// replace succ in current position with newsucc and add this as pred of newsucc
void BB::ReplaceSucc(const BB *old, BB *newSucc) {
  for (auto &succElement : succ) {
    if (succElement == old) {
      succElement = newSucc;
      newSucc->pred.push_back(this);
      break;
    }
  }
}

// this is common_entry_bb, so newsucc's pred is left as is
void BB::ReplaceSuccOfCommonEntryBB(const BB *old, BB *newSucc) {
  for (auto &succElement : succ) {
    if (succElement == old) {
      succElement = newSucc;
      break;
    }
  }
}

void BB::FindReachableBBs(std::vector<bool> &visitedBBs) const {
  CHECK_FATAL(GetBBId() < visitedBBs.size(), "out of range in BB::FindReachableBBs");
  if (visitedBBs[GetBBId()]) {
    return;
  }
  visitedBBs[GetBBId()] = true;
  for (const BB *bb : succ) {
    bb->FindReachableBBs(visitedBBs);
  }
}

void BB::FindWillExitBBs(std::vector<bool> &visitedBBs) const {
  CHECK_FATAL(GetBBId() < visitedBBs.size(), "out of range in BB::FindReachableBBs");
  if (visitedBBs[GetBBId()]) {
    return;
  }
  visitedBBs[GetBBId()] = true;
  for (const BB *bb : pred) {
    bb->FindWillExitBBs(visitedBBs);
  }
}

void BB::SetFirstMe(MeStmt *stmt) {
  meStmtList.update_front(stmt);
}

void BB::SetLastMe(MeStmt *stmt) {
  meStmtList.update_back(stmt);
}

MeStmt *BB::GetLastMe() {
  return &meStmtList.back();
}

void BB::RemoveMeStmt(MeStmt *meStmt) {
  CHECK_FATAL(meStmt != nullptr, "null ptr check");
  meStmtList.erase(meStmt);
}

void BB::AddMeStmtFirst(MeStmt *meStmt) {
  CHECK_FATAL(meStmt != nullptr, "null ptr check");
  meStmtList.push_front(meStmt);
  meStmt->SetBB(this);
}

void BB::AddMeStmtLast(MeStmt *meStmt) {
  CHECK_FATAL(meStmt != nullptr, "null ptr check");
  meStmtList.push_back(meStmt);
  meStmt->SetBB(this);
}

void BB::InsertMeStmtBefore(const MeStmt *meStmt, MeStmt *inStmt) {
  CHECK_FATAL(meStmt != nullptr, "null ptr check");
  CHECK_FATAL(inStmt != nullptr, "null ptr check");
  meStmtList.insert(meStmt, inStmt);
  inStmt->SetBB(this);
}

void BB::InsertMeStmtAfter(const MeStmt *meStmt, MeStmt *inStmt) {
  meStmtList.insertAfter(meStmt, inStmt);
  inStmt->SetBB(this);
}

// insert instmt before br to the last of bb
void BB::InsertMeStmtLastBr(MeStmt *inStmt) {
  if (IsMeStmtEmpty()) {
    AddMeStmtLast(inStmt);
    return;
  }
  MeStmt *brStmt = meStmtList.rbegin().base().d();
  Opcode op = brStmt->GetOp();
  if (brStmt->IsCondBr() || op == OP_goto || op == OP_switch || op == OP_throw || op == OP_return || op == OP_gosub ||
      op == OP_retsub) {
    InsertMeStmtBefore(brStmt, inStmt);
  } else {
    AddMeStmtLast(inStmt);
  }
}

void BB::ReplaceMeStmt(MeStmt *stmt, MeStmt *newStmt) {
  InsertMeStmtBefore(stmt, newStmt);
  RemoveMeStmt(stmt);
}

void BB::DumpMeBB(IRMap &irMap) {
  for (MeStmt &meStmt : GetMeStmts()) {
    meStmt.Dump(&irMap);
  }
}

void BB::DumpMeVarPiList(IRMap *irMap) {
  if (meVarPiList.empty()) {
    return;
  }
  std::cout << "<<<<<<<<<<<<<<  PI Node Start >>>>>>>>>>>>>>>>>>\n";
  for (const auto &pair : meVarPiList) {
    BB *bb = pair.first;
    std::cout << "Frome BB : " << bb->GetBBId() << '\n';
    for (const auto *stmt : pair.second) {
      stmt->Dump(irMap);
    }
  }
  std::cout << "<<<<<<<<<<<<<<  PI Node End >>>>>>>>>>>>>>>>>>>>\n";
}

void BB::DumpMeVarPhiList(IRMap *irMap) {
  int count = 0;
  for (const auto &phi : meVarPhiList) {
    phi.second->Dump(irMap);
    int dumpVsyNum = DumpOptions::GetDumpVsyNum();
    if (dumpVsyNum > 0 && ++count >= dumpVsyNum) {
      break;
    }
    ASSERT(count >= 0, "meVarPhiList too large");
  }
}

void BB::DumpMeRegPhiList(IRMap *irMap) {
  for (const auto &phi : meRegPhiList) {
    phi.second->Dump(irMap);
  }
}

void SCCOfBBs::Dump() {
  std::cout << "SCC " << id << " contains\n";
  for (BB *bb : bbs) {
    std::cout << "bb(" << bb->UintID() << ")  ";
  }
  std::cout << '\n';
}

bool SCCOfBBs::HasCycle() const {
  CHECK_FATAL(!bbs.empty(), "should have bbs in the scc");
  if (bbs.size() > 1) {
    return true;
  }
  BB *bb = bbs[0];
  for (const BB *succ : bb->GetSucc()) {
    if (succ == bb) {
      return true;
    }
  }
  return false;
}

void SCCOfBBs::Verify(MapleVector<SCCOfBBs*> &sccOfBB) {
  CHECK_FATAL(!bbs.empty(), "should have bbs in the scc");
  for (const BB *bb : bbs) {
    SCCOfBBs *scc = sccOfBB.at(bb->UintID());
    CHECK_FATAL(scc == this, "");
  }
}

void SCCOfBBs::SetUp(MapleVector<SCCOfBBs*> &sccOfBB) {
  for (BB *bb : bbs) {
    for (BB *succ : bb->GetSucc()) {
      if (succ == nullptr || sccOfBB.at(succ->UintID()) == this) {
        continue;
      }
      succSCC.insert(sccOfBB[succ->UintID()]);
    }

    for (BB *pred : bb->GetPred()) {
      if (pred == nullptr || sccOfBB.at(pred->UintID()) == this) {
        continue;
      }
      predSCC.insert(sccOfBB[pred->UintID()]);
    }
  }
}

bool ControlFlowInInfiniteLoop(const BB &bb, Opcode opcode) {
  switch (opcode) {
    // goto always return true
    case OP_goto:
      return true;
    case OP_brtrue:
    case OP_brfalse:
    case OP_switch:
      return bb.GetAttributes(kBBAttrWontExit);
    default:
      return false;
  }
}
}  // namespace maple
