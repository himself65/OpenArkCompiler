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
#include "hdse.h"
#include <iostream>
#include "ssa_mir_nodes.h"
#include "ver_symbol.h"
#include "irmap.h"
#include "opcode_info.h"
#include "mir_preg.h"
#include "utils.h"

// This phase do dead store elimination. This optimization is done on a per-SSA
// version basis. As a result, an overall criterion is that the SSA version must
// not have appeared as phi operand;
// This optimization consider all stmt are not needed at first.
// The work steps as follow:
// 1. mark all stmt are not needed. init an empty worklist to put live node.
// 2. mark some special stmts which has side effect as needed, such as
//    return/eh/call and some assigned stmts who have volatile fileds and so on.
//    Put all operands and mayUse nodes of the needed stmt into worklist.
// 3. For the nodes in worklist mark the def stmt as needed just as step 2 and
//    pop the node from the worklist.
// 4. Repeat step 3 until the worklist is empty.
namespace maple {
using namespace utils;

void HDSE::RemoveNotRequiredCallAssignPart(MeStmt &stmt) {
  if (!kOpcodeInfo.IsCallAssigned(stmt.GetOp())) {
    return;
  }
  auto *assignPart = stmt.GetMustDefList();
  bool assignPartRequired = false;
  for (auto it = assignPart->begin(); it != assignPart->end(); ++it) {
    if (IsExprNeeded(*it->GetLHS())) {
      assignPartRequired = true;
    }
  }
  if (!assignPartRequired) {
    assignPart->clear();
  }
}

void HDSE::RemoveNotRequiredStmtsInBB(BB &bb) {
  for (auto &meStmt : bb.GetMeStmts()) {
    if (!meStmt.GetIsLive()) {
      if (hdseDebug) {
        mirModule.GetOut() << "========== HSSA DSE is deleting this stmt: ";
        meStmt.Dump(&irMap);
      }
      if (meStmt.IsCondBr() || meStmt.GetOp() == OP_switch) {
        // update CFG
        while (bb.GetSucc().size() != 1) {
          BB *succ = bb.GetSucc().back();
          succ->RemovePred(bb);
        }
        bb.SetKind(kBBFallthru);
      }
      // A ivar contained in stmt
      if (stmt2NotNullExpr.find(&meStmt) != stmt2NotNullExpr.end()) {
        for (MeExpr *meExpr : stmt2NotNullExpr.at(&meStmt)) {
          if (NeedNotNullCheck(*meExpr, bb)) {
            UnaryMeStmt *nullCheck = irMap.New<UnaryMeStmt>(OP_assertnonnull);
            nullCheck->SetBB(&bb);
            nullCheck->SetSrcPos(meStmt.GetSrcPosition());
            nullCheck->SetMeStmtOpndValue(meExpr);
            bb.InsertMeStmtBefore(&meStmt, nullCheck);
            nullCheck->SetIsLive(true);
            notNullExpr2Stmt[meExpr].push_back(nullCheck);
          }
        }
      }
      bb.RemoveMeStmt(&meStmt);
    }
    RemoveNotRequiredCallAssignPart(meStmt);
  }
}

// If a ivar's base not used as not null, should insert a not null stmt
// Only make sure throw NPE in same BB
// If must make sure throw at first stmt, much more not null stmt will be inserted
bool HDSE::NeedNotNullCheck(MeExpr &meExpr, const BB &bb) {
  for (MeStmt *stmt : notNullExpr2Stmt[&meExpr]) {
    if (!stmt->GetIsLive()) {
      continue;
    }
    if (postDom.Dominate(*(stmt->GetBB()), bb)) {
      return false;
    }
  }
  return true;
}

void HDSE::MarkMuListRequired(MapleMap<OStIdx, VarMeExpr*> &muList) {
  for (auto &pair : muList) {
    AddNewUse(*pair.second);
  }
}

void HDSE::MarkChiNodeRequired(ChiMeNode &chiNode) {
  if (chiNode.GetIsLive()) {
    return;
  }
  chiNode.SetIsLive(true);
  AddNewUse(*chiNode.GetRHS());
  MeStmt *meStmt = chiNode.GetBase();
  MarkStmtRequired(*meStmt);
}

template <class VarOrRegPhiNode>
void HDSE::MarkPhiRequired(VarOrRegPhiNode &mePhiNode) {
  if (mePhiNode.GetIsLive()) {
    return;
  }
  mePhiNode.SetIsLive(true);
  for (auto *meExpr : mePhiNode.GetOpnds()) {
    if (meExpr != nullptr) {
      MarkSingleUseLive(*meExpr);
    }
  }
  MarkControlDependenceLive(*mePhiNode.GetDefBB());
}

void HDSE::MarkDefStmt(ScalarMeExpr &scalarExpr) {
  switch (scalarExpr.GetDefBy()) {
    case kDefByNo:
      break;
    case kDefByStmt: {
      auto *defStmt = scalarExpr.GetDefStmt();
      MarkStmtRequired(*defStmt);
      break;
    }
    case kDefByPhi: {
      MarkPhiRequired(scalarExpr.GetDefPhi());
      break;
    }
    case kDefByChi: {
      MarkChiNodeRequired(scalarExpr.GetDefChi());
      break;
    }
    case kDefByMustDef: {
      auto *mustDef = &scalarExpr.GetDefMustDef();
      if (!mustDef->GetIsLive()) {
        mustDef->SetIsLive(true);
        MarkStmtRequired(*mustDef->GetBase());
      }
      break;
    }
    default:
      ASSERT(false, "var defined wrong");
      break;
  }
}

// Find all stmt contains ivar and save to stmt2NotNullExpr
// Find all not null expr used as ivar's base、OP_array's or OP_assertnonnull's opnd
// And save to notNullExpr2Stmt
void HDSE::CollectNotNullExpr(MeStmt &stmt) {
  size_t opndNum = stmt.NumMeStmtOpnds();
  size_t i = 0;
  if (opndNum > 0 && instance_of<CallMeStmt>(stmt)) {
    CallMeStmt &callStmt = static_cast<CallMeStmt&>(stmt);
    if (callStmt.GetTargetFunction().IsStatic()) {
      CollectNotNullExpr(stmt, ToRef(stmt.GetOpnd(0)), kExprTypeNormal);
    }
    CollectNotNullExpr(stmt, ToRef(stmt.GetOpnd(0)), kExprTypeNotNull);
    ++i;
  }
  for (; i < opndNum; ++i) {
    MeExpr *opnd = stmt.GetOpnd(i);
    // A normal opnd not sure
    MeExprOp meOp = opnd->GetMeOp();
    if (meOp == kMeOpVar || meOp == kMeOpReg) {
      continue;
    }
    CollectNotNullExpr(stmt, ToRef(opnd), kExprTypeNormal);
  }
}

void HDSE::CollectNotNullExpr(MeStmt &stmt, MeExpr &meExpr, uint8 exprType) {
  MeExprOp meOp = meExpr.GetMeOp();
  switch (meOp) {
    case kMeOpVar:
    case kMeOpReg:
    case kMeOpConst: {
      PrimType type = meExpr.GetPrimType();
      // Ref expr used in ivar、array or assertnotnull
      if (exprType != kExprTypeNormal && (type == PTY_ref || type == PTY_ptr)) {
        notNullExpr2Stmt[&meExpr].push_back(&stmt);
      }
      break;
    }
    case kMeOpIvar: {
      MeExpr *base = static_cast<IvarMeExpr&>(meExpr).GetBase();
      if (exprType != kExprTypeIvar) {
        stmt2NotNullExpr[&stmt].push_back(base);
        MarkSingleUseLive(meExpr);
      }
      notNullExpr2Stmt[base].push_back(&stmt);
      CollectNotNullExpr(stmt, ToRef(base), kExprTypeIvar);
      break;
    }
    default: {
      if (exprType != kExprTypeNormal) {
        // Ref expr used in ivar array or assertnotnull
        PrimType type = meExpr.GetPrimType();
        if (type == PTY_ref || type == PTY_ptr) {
          notNullExpr2Stmt[&meExpr].push_back(&stmt);
        }
      } else {
        // Ref expr used array or assertnotnull
        Opcode op = meExpr.GetOp();
        bool notNull = op == OP_array || op == OP_assertnonnull;
        exprType = notNull ? kExprTypeNotNull : kExprTypeNormal;
      }
      for (size_t i = 0; i < meExpr.GetNumOpnds(); ++i) {
        CollectNotNullExpr(stmt, ToRef(meExpr.GetOpnd(i)), exprType);
      }
      break;
    }
  }
}

void HDSE::PropagateUseLive(MeExpr &meExpr) {
  switch (meExpr.GetMeOp()) {
    case kMeOpVar:
    case kMeOpReg: {
      MarkDefStmt(static_cast<ScalarMeExpr&>(meExpr));
      return;
    }
    default: {
      ASSERT(false, "MeOp ERROR");
      return;
    }
  }
}

bool HDSE::ExprHasSideEffect(const MeExpr &meExpr) const {
  Opcode op = meExpr.GetOp();
  if (kOpcodeInfo.HasSideEffect(op)) {
    return true;
  }
  // may throw exception
  if (op == OP_gcmallocjarray || op == OP_gcpermallocjarray) {
    return true;
  }
  // create a instance of interface
  if (op == OP_gcmalloc || op == OP_gcpermalloc) {
    auto &gcMallocMeExpr = static_cast<const GcmallocMeExpr&>(meExpr);
    MIRType *type = GlobalTables::GetTypeTable().GetTypeFromTyIdx(gcMallocMeExpr.GetTyIdx());
    return type->GetKind() == kTypeInterface;
  }
  return false;
}

bool HDSE::ExprNonDeletable(const MeExpr &meExpr) const {
  if (ExprHasSideEffect(meExpr)) {
    return true;
  }
  switch (meExpr.GetMeOp()) {
    case kMeOpReg: {
      auto &regMeExpr = static_cast<const RegMeExpr&>(meExpr);
      return (regMeExpr.GetRegIdx() == -kSregThrownval);
    }
    case kMeOpVar: {
      auto &varMeExpr = static_cast<const VarMeExpr&>(meExpr);
      return varMeExpr.IsVolatile(ssaTab) ||
             (decoupleStatic && ssaTab.GetSymbolOriginalStFromID(varMeExpr.GetOStIdx())->GetMIRSymbol()->IsGlobal());
    }
    case kMeOpIvar: {
      auto &opIvar = static_cast<const IvarMeExpr&>(meExpr);
      return opIvar.IsVolatile() || ExprNonDeletable(*opIvar.GetBase());
    }
    case kMeOpNary: {
      auto &opNary = static_cast<const NaryMeExpr&>(meExpr);
      if (meExpr.GetOp() == OP_intrinsicop) {
        IntrinDesc *intrinDesc = &IntrinDesc::intrinTable[opNary.GetIntrinsic()];
        return (!intrinDesc->HasNoSideEffect());
      }
      break;
    }
    default:
      break;
  }
  for (size_t i = 0; i != meExpr.GetNumOpnds(); ++i) {
    if (ExprNonDeletable(*meExpr.GetOpnd(i))) {
      return true;
    }
  }
  return false;
}

bool HDSE::HasNonDeletableExpr(const MeStmt &meStmt) const {
  Opcode op = meStmt.GetOp();
  switch (op) {
    case OP_dassign: {
      auto &dasgn = static_cast<const DassignMeStmt&>(meStmt);
      VarMeExpr *varMeExpr = dasgn.GetVarLHS();
      return (varMeExpr != nullptr && varMeExpr->IsVolatile(ssaTab)) || ExprNonDeletable(*dasgn.GetRHS()) ||
             (hdseKeepRef && dasgn.Propagated()) || dasgn.GetWasMayDassign() ||
             (decoupleStatic && ssaTab.GetSymbolOriginalStFromID(varMeExpr->GetOStIdx())->GetMIRSymbol()->IsGlobal());
    }
    case OP_regassign: {
      auto &rasgn = static_cast<const RegassignMeStmt&>(meStmt);
      return ExprNonDeletable(*rasgn.GetRHS());
    }
    case OP_maydassign:
      return true;
    case OP_iassign: {
      auto &iasgn = static_cast<const IassignMeStmt&>(meStmt);
      auto &ivarMeExpr = static_cast<IvarMeExpr&>(*iasgn.GetLHSVal());
      return ivarMeExpr.IsVolatile() || ivarMeExpr.IsFinal() ||
          ExprNonDeletable(*iasgn.GetLHSVal()->GetBase()) || ExprNonDeletable(*iasgn.GetRHS());
    }
    default:
      return false;
  }
}

void HDSE::MarkLastUnconditionalGotoInPredBBRequired(const BB &bb) {
  for (auto predIt = bb.GetPred().begin(); predIt != bb.GetPred().end(); ++predIt) {
    BB *predBB = *predIt;
    if (predBB == &bb || predBB->GetMeStmts().empty()) {
      continue;
    }
    auto &lastStmt = predBB->GetMeStmts().back();
    if (!lastStmt.GetIsLive() && lastStmt.GetOp() == OP_goto) {
      MarkStmtRequired(lastStmt);
    }
  }
}

void HDSE::MarkLastStmtInPDomBBRequired(const BB &bb) {
  CHECK(bb.GetBBId() < postDom.GetPdomFrontierSize(), "index out of range in HDSE::MarkLastStmtInPDomBBRequired");
  for (BBId cdBBId : postDom.GetPdomFrontierItem(bb.GetBBId())) {
    BB *cdBB = bbVec[cdBBId];
    CHECK_FATAL(cdBB != nullptr, "cdBB is null in HDSE::MarkLastStmtInPDomBBRequired");
    if (cdBB == &bb || cdBB->IsMeStmtEmpty()) {
      continue;
    }
    auto &lastStmt = cdBB->GetMeStmts().back();
    Opcode op = lastStmt.GetOp();
    CHECK_FATAL((lastStmt.IsCondBr() || op == OP_switch || op == OP_retsub || op == OP_throw ||
                 cdBB->GetAttributes(kBBAttrIsTry) || cdBB->GetAttributes(kBBAttrWontExit)),
                "HDSE::MarkLastStmtInPDomBBRequired: control dependent on unexpected statement");
    if ((IsBranch(op) || op == OP_retsub || op == OP_throw)) {
      MarkStmtRequired(lastStmt);
    }
  }
}

void HDSE::MarkLastBranchStmtInBBRequired(BB &bb) {
  auto &meStmts = bb.GetMeStmts();
  if (!meStmts.empty()) {
    auto &lastStmt = meStmts.back();
    Opcode op = lastStmt.GetOp();
    if (IsBranch(op)) {
      MarkStmtRequired(lastStmt);
    }
  }
}

void HDSE::MarkControlDependenceLive(BB &bb) {
  if (bbRequired[bb.GetBBId()]) {
    return;
  }
  bbRequired[bb.GetBBId()] = true;

  MarkLastBranchStmtInBBRequired(bb);
  MarkLastStmtInPDomBBRequired(bb);
  MarkLastUnconditionalGotoInPredBBRequired(bb);
}

void HDSE::MarkSingleUseLive(MeExpr &meExpr) {
  if (IsExprNeeded(meExpr)) {
    return;
  }
  SetExprNeeded(meExpr);
  MeExprOp meOp = meExpr.GetMeOp();
  switch (meOp) {
    case kMeOpVar:
    case kMeOpReg: {
      workList.push_front(&meExpr);
      return;
    }
    case kMeOpIvar: {
      auto *base = static_cast<IvarMeExpr&>(meExpr).GetBase();
      if (base != nullptr) {
        MarkSingleUseLive(*base);
      }

      MarkSingleUseLive(*static_cast<IvarMeExpr&>(meExpr).GetMu());
      return;
    }
    default:
      break;
  }

  for (size_t i = 0; i != meExpr.GetNumOpnds(); ++i) {
    MeExpr *operand = meExpr.GetOpnd(i);
    if (operand != nullptr) {
      MarkSingleUseLive(*operand);
    }
  }
}

void HDSE::MarkStmtUseLive(MeStmt &meStmt) {
  // mark single use
  for (size_t i = 0; i < meStmt.NumMeStmtOpnds(); ++i) {
    auto *operand = meStmt.GetOpnd(i);
    if (operand != nullptr) {
      MarkSingleUseLive(*operand);
    }
  }

  // mark MuList
  auto *muList = meStmt.GetMuList();
  if (muList != nullptr) {
    MarkMuListRequired(*muList);
  }
}

void HDSE::MarkStmtRequired(MeStmt &meStmt) {
  if (meStmt.GetIsLive()) {
    return;
  }
  meStmt.SetIsLive(true);

  if (meStmt.GetOp() == OP_comment) {
    return;
  }
  auto *prev = meStmt.GetPrev();
  if (prev != nullptr && prev->GetOp() == OP_comment) {
    prev->SetIsLive(true);
  }

  // mark use
  MarkStmtUseLive(meStmt);

  // markBB
  MarkControlDependenceLive(*meStmt.GetBB());
}

bool HDSE::StmtMustRequired(const MeStmt &meStmt, const BB &bb) const {
  Opcode op = meStmt.GetOp();
  // special opcode cannot be eliminated
  if (IsStmtMustRequire(op)) {
    return true;
  }
  // control flow in an infinite loop cannot be eliminated
  if (ControlFlowInInfiniteLoop(bb, op)) {
    return true;
  }
  // if stmt has not deletable expr
  if (HasNonDeletableExpr(meStmt)) {
    return true;
  }
  return false;
}

void HDSE::MarkSpecialStmtRequired() {
  for (auto *bb : bbVec) {
    if (bb == nullptr) {
      continue;
    }
    auto &meStmtNodes = bb->GetMeStmts();
    for (auto itStmt = meStmtNodes.rbegin(); itStmt != meStmtNodes.rend(); ++itStmt) {
      MeStmt *pStmt = to_ptr(itStmt);
      if (pStmt->GetIsLive()) {
        continue;
      }
      CollectNotNullExpr(*pStmt);
      if (StmtMustRequired(*pStmt, *bb)) {
        MarkStmtRequired(*pStmt);
      }
    }
  }
}

void HDSE::DseInit() {
  // Init bb's required flag
  bbRequired[commonEntryBB.GetBBId()] = true;
  bbRequired[commonExitBB.GetBBId()] = true;

  // Init all MeExpr to be dead;
  exprLive.resize(irMap.GetExprID(), false);

  for (auto *bb : bbVec) {
    if (bb == nullptr) {
      continue;
    }
    // mark phi nodes dead
    for (const auto &phiPair : bb->GetMePhiList()) {
      phiPair.second->SetIsLive(false);
    }

    for (auto &stmt : bb->GetMeStmts()) {
      // mark stmt dead
      stmt.SetIsLive(false);
      // mark chi nodes dead
      MapleMap<OStIdx, ChiMeNode*> *chiList = stmt.GetChiList();
      if (chiList != nullptr) {
        for (std::pair<OStIdx, ChiMeNode*> pair : *chiList) {
          pair.second->SetIsLive(false);
        }
      }
      // mark mustDef nodes dead
      if (kOpcodeInfo.IsCallAssigned(stmt.GetOp())) {
        MapleVector<MustDefMeNode> *mustDefList = stmt.GetMustDefList();
        for (MustDefMeNode &mustDef : *mustDefList) {
          mustDef.SetIsLive(false);
        }
      }
    }
  }
}

void HDSE::InvokeHDSEUpdateLive() {
  DseInit();
  MarkSpecialStmtRequired();
  PropagateLive();
}

void HDSE::DoHDSE() {
  DseInit();
  MarkSpecialStmtRequired();
  PropagateLive();
  RemoveNotRequiredStmts();
}
} // namespace maple
