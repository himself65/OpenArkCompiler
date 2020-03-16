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
#include "me_cond_based_rc.h"
#include "me_cond_based_npc.h"
#include "me_const.h"

// We do two types of condition based optimization here:
// 1. condition based null pointer check(NPC) elimination
// 2. condition based RC elimination
// both use post dominance data to see if the symbol they are dealing with is
// zero or not. To be specific, retrieve every stmt of all the bbs', if the stmt
// is an assertnonnull/MCC_DecRef_NaiveRCFast, then retrieve the enclosing bb's
// every post dominance frontiers in a backward order, if a frontier is found of
// conditional branch, and the `expr` it compared is the same as that of
// the assertnonnull/MCC_DecRef_NaiveRCFast, then the value of the symbol can be
// known. If it's not zero, condbasednpc could remove the assertnonnull
// statement; if it is a zero, then MCC_DecRef_NaiveRCFast could be removed.
//
// There are other conditions that the assertnonnull can be removed, e.g. if
// the statement next to assertnonnull is an iread and the base is the
// symbol assertnonnull is dealing with, then the assertnonnull can be removed,
// because the exception will be thrown at this same place by iread, as the
// symbol's value is zero.
namespace maple {
// check if varmeexpr is under the condition varmeexpr == 0 when expectedEq0 is true,
// or varmeexpr != 0 if expectedEq0 is false
bool MeCondBased::NullValueFromOneTestCond(const VarMeExpr &varMeExpr, const BB &cdBB, const BB &bb, bool expectedEq0) {
  auto &meStmts = cdBB.GetMeStmts();
  if (meStmts.empty()) {
    return false;
  }
  if (!meStmts.back().IsCondBr()) {
    return false;
  }
  auto &condBrMeStmt = static_cast<const CondGotoMeStmt&>(meStmts.back());
  bool isTrueBr = condBrMeStmt.GetOp() == OP_brtrue;
  MeExpr *testMeExpr = condBrMeStmt.GetOpnd();
  if (testMeExpr->GetOp() != OP_eq && testMeExpr->GetOp() != OP_ne) {
    return false;
  }
  bool isEq = testMeExpr->GetOp() == OP_eq;
  auto *cmpMeExpr = static_cast<OpMeExpr*>(testMeExpr);
  if (cmpMeExpr->GetOpnd(0)->GetMeOp() != kMeOpVar || cmpMeExpr->GetOpnd(1)->GetOp() != OP_constval) {
    return false;
  }
  auto *constMeExpr = static_cast<ConstMeExpr*>(cmpMeExpr->GetOpnd(1));
  if (!constMeExpr->IsZero()) {
    return false;
  }
  if (!cmpMeExpr->GetOpnd(0)->IsSameVariableValue(varMeExpr)) {
    return false;
  }
  const BB *sucDomBB = nullptr;
  for (size_t i = 0; i < cdBB.GetSucc().size(); ++i) {
    const BB *sucBB = cdBB.GetSucc(i);
    if (dominance->Dominate(*sucBB, bb)) {
      sucDomBB = sucBB;
      break;
    }
  }
  if (sucDomBB == nullptr) {
    return false;
  }
  bool isJumptoBB = sucDomBB->GetBBLabel() == condBrMeStmt.GetOffset();
  if (expectedEq0) {
    isEq = !isEq;
  }
  return isJumptoBB ? ((isTrueBr && !isEq) || (!isTrueBr && isEq)) : ((isTrueBr && isEq) || (!isTrueBr && !isEq));
}

bool MeCondBased::NullValueFromTestCond(const VarMeExpr &varMeExpr, const BB &bb, bool expectedEq0) {
  MapleSet<BBId> *pdomFrt = &dominance->GetPdomFrontierItem(bb.GetBBId());
  size_t bbSize = dominance->GetBBVecSize();
  std::vector<bool> visitedMap(bbSize, false);
  bool provenNull = false;
  while (pdomFrt->size() == 1) {
    BB &cdBB = *func->GetAllBBs().at(*(pdomFrt->begin()));
    if (visitedMap[cdBB.GetBBId()]) {
      break;
    }
    visitedMap[cdBB.GetBBId()] = true;
    if (NullValueFromOneTestCond(varMeExpr, cdBB, bb, expectedEq0)) {
      provenNull = true;
      break;
    }
    pdomFrt = &dominance->GetPdomFrontierItem(cdBB.GetBBId());
  }
  return provenNull;
}

bool MeCondBased::IsIreadWithTheBase(const VarMeExpr &var, const MeExpr &meExpr) {
  if (meExpr.GetOp() == OP_iread) {
    auto &ivarMeExpr = static_cast<const IvarMeExpr&>(meExpr);
    if (ivarMeExpr.GetBase()->GetExprID() == var.GetExprID()) {
      return true;
    }
  }
  for (uint8 i = 0; i < meExpr.GetNumOpnds(); ++i) {
    if (IsIreadWithTheBase(var, *meExpr.GetOpnd(i))) {
      return true;
    }
  }
  return false;
}

bool MeCondBased::StmtHasDereferencedBase(const MeStmt &stmt, const VarMeExpr &var) {
  if (stmt.GetOp() == OP_iassign) {
    auto &iassStmt = static_cast<const IassignMeStmt&>(stmt);
    if (iassStmt.GetLHSVal()->GetBase()->GetExprID() == var.GetExprID()) {
      return true;
    }
  }
  if (stmt.GetOp() == OP_syncenter || stmt.GetOp() == OP_syncexit) {
    auto &syncMeStmt = static_cast<const SyncMeStmt&>(stmt);
    const MapleVector<MeExpr*> &opnds = syncMeStmt.GetOpnds();
    for (auto it = opnds.begin(); it != opnds.end(); ++it) {
      if ((*it)->GetExprID() == var.GetExprID()) {
        return true;
      }
    }
  }
  for (size_t i = 0; i < stmt.NumMeStmtOpnds(); ++i) {
    MeExpr *meExpr = stmt.GetOpnd(i);
    if (IsIreadWithTheBase(var, *meExpr)) {
      return true;
    }
  }
  return false;
}

bool MeCondBased::PointerWasDereferencedBefore(const VarMeExpr &var, const UnaryMeStmt &assertMeStmt, const BB *bb) {
  // If var is defined in the function, let BBx be the BB that defines var.
  // If var is not defined, then let BBx be the function entry BB.
  // Let BBy be the current BB that contains the assertnonnull.
  // Search backward along the path in the dominator tree from BBy to BBx.
  // If it sees an iread or iassign whose base is var, then the assertnonnull can be deleted.
  MeStmt *defMeStmt = nullptr;
  BB *bbx = var.GetDefByBBMeStmt(*dominance, defMeStmt);
  if (bbx == nullptr) {
    return false;
  }
  CHECK_FATAL(dominance->Dominate(*bbx, *bb), "bbx should dominate bb at this point");
  for (MeStmt *stmt = assertMeStmt.GetPrev(); stmt != nullptr; stmt = stmt->GetPrev()) {
    if (StmtHasDereferencedBase(*stmt, var)) {
      return true;
    }
  }
  if (bbx == bb) {
    return false;
  }
  BB *itBB = dominance->GetDom(bb->GetBBId());
  while (itBB != bbx) {
    // check if there is an iread or iassign in itbb whose base is var
    auto &meStmts = itBB->GetMeStmts();
    for (auto itStmt = meStmts.rbegin(); itStmt != meStmts.rend(); ++itStmt) {
      if (StmtHasDereferencedBase(*to_ptr(itStmt), var)) {
        return true;
      }
    }
    itBB = dominance->GetDom(itBB->GetBBId());
  }
  auto &meStmts = bbx->GetMeStmts();
  for (auto itStmt = meStmts.rbegin(); to_ptr(itStmt) != defMeStmt; ++itStmt) {
    if (StmtHasDereferencedBase(*to_ptr(itStmt), var)) {
      return true;
    }
  }
  return false;
}

bool MeCondBased::PointerWasDereferencedRightAfter(const VarMeExpr &var, const UnaryMeStmt &assertMeStmt) {
  // assertnonnull(var)
  // t = iread(var, 0)
  // we can safely delete assertnonnull(var)
  // if we got
  // assertnonnull(var)
  // t2 <-
  // t = iread (var)
  // we can't remove assertnonnull(var) safely, because if var is null, then the position of throwing exception is
  // not the same.
  MeStmt *nextMeStmt = assertMeStmt.GetNextMeStmt();
  return (nextMeStmt != nullptr) && StmtHasDereferencedBase(*nextMeStmt, var);
}

bool MeCondBased::IsNotNullValue(const VarMeExpr &varMeExpr, const UnaryMeStmt &assertMeStmt, const BB *bb) {
  const OriginalSt *varOst = func->GetMeSSATab()->GetSymbolOriginalStFromID(varMeExpr.GetOStIdx());
  if (varOst->IsFormal() && varOst->GetMIRSymbol()->GetName() == kStrThisPointer) {
    return true;
  }
  if (varMeExpr.GetDefBy() == kDefByStmt && varMeExpr.GetDefStmt()->GetOp() == OP_dassign) {
    MeExpr *rhs = static_cast<DassignMeStmt*>(varMeExpr.GetDefStmt())->GetRHS();
    if (rhs->GetOp() == OP_gcmalloc || rhs->GetOp() == OP_gcmallocjarray) {
      return true;
    }
    if (rhs->GetMeOp() == kMeOpVar) {
      auto *rhsVar = static_cast<VarMeExpr*>(rhs);
      const OriginalSt *ost = func->GetMeSSATab()->GetSymbolOriginalStFromID(rhsVar->GetOStIdx());
      if (ost->IsFormal() && ost->GetMIRSymbol()->GetName() == kStrThisPointer) {
        return true;
      }
    }
  }
  if (PointerWasDereferencedBefore(varMeExpr, assertMeStmt, bb)) {
    return true;
  }
  return PointerWasDereferencedRightAfter(varMeExpr, assertMeStmt);
}

void CondBasedNPC::DoCondBasedNPC() {
  auto eIt = GetFunc()->valid_end();
  for (auto bIt = GetFunc()->valid_begin(); bIt != eIt; ++bIt) {
    auto *bb = *bIt;
    for (auto &stmt : bb->GetMeStmts()) {
      if (stmt.GetOp() != OP_assertnonnull) {
        continue;
      }
      auto &assertMeStmt = static_cast<UnaryMeStmt&>(stmt);
      if (assertMeStmt.GetOpnd()->GetMeOp() != kMeOpVar) {
        continue;
      }
      auto *varMeExpr = static_cast<VarMeExpr*>(assertMeStmt.GetOpnd());
      if (NullValueFromTestCond(*varMeExpr, *bb, false) || IsNotNullValue(*varMeExpr, assertMeStmt, bb)) {
        bb->RemoveMeStmt(&stmt);
      }
    }
  }
}

AnalysisResult *MeDoCondBasedRC::Run(MeFunction *func, MeFuncResultMgr *m, ModuleResultMgr*) {
  auto *dom = static_cast<Dominance*>(m->GetAnalysisResult(MeFuncPhase_DOMINANCE, func));
  ASSERT(dom != nullptr, "dominance phase has problem");
  CondBasedRC condBasedRC(func, dom);
  auto eIt = func->valid_end();
  for (auto bIt = func->valid_begin(); bIt != eIt; ++bIt) {
    auto *bb = *bIt;
    for (auto &stmt : bb->GetMeStmts()) {
      if (stmt.GetOp() != OP_decref) {
        continue;
      }
      auto &decref = static_cast<UnaryMeStmt&>(stmt);
      if (decref.GetOpnd()->GetMeOp() != kMeOpVar) {
        continue;
      }
      auto *varMeExpr = static_cast<VarMeExpr*>(decref.GetOpnd());
      const OriginalSt *ost = func->GetMeSSATab()->GetSymbolOriginalStFromID(varMeExpr->GetOStIdx());
      if (!ost->IsLocal() && ost->IsVolatile()) {
        // global volatile cannot be optimized
        continue;
      }
      MeStmt *refAssign = stmt.GetNext();
      if (!condBasedRC.NullValueFromTestCond(*varMeExpr, *bb, true)) {
        continue;
      }
      bb->RemoveMeStmt(&stmt);  // delete the decref
      if (refAssign == nullptr) {
        break;
      }
      refAssign->DisableNeedDecref();
      stmt = *refAssign;  // next iteration will process the stmt after refassign
    }
  }
  return nullptr;
}

AnalysisResult *MeDoCondBasedNPC::Run(MeFunction *func, MeFuncResultMgr *m, ModuleResultMgr*) {
  auto *dom = static_cast<Dominance*>(m->GetAnalysisResult(MeFuncPhase_DOMINANCE, func));
  ASSERT(dom != nullptr, "dominance phase has problem");
  CondBasedNPC condBasedNPC(func, dom);
  condBasedNPC.DoCondBasedNPC();
  return nullptr;
}
}  // namespace maple
