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
// 4. Repeat step 3 untile the worklist is empty.

namespace maple {
using namespace utils;

void HDSE::RemoveNotRequiredStmtsInBB(BB &bb) {
  for (auto &meStmt : bb.GetMeStmts()) {
    if (!meStmt.GetIsLive()) {
      if (hdseDebug) {
        mirModule.GetOut() << "========== HSSA DSE is deleting this stmt: ";
        meStmt.Dump(&irMap);
      }
      if (meStmt.GetOp() != OP_dassign && (meStmt.IsCondBr() || meStmt.GetOp() == OP_switch)) {
        // update CFG
        while (bb.GetSucc().size() != 1) {
          BB *succ = bb.GetSucc().back();
          succ->RemoveBBFromPred(&bb);
          bb.GetSucc().pop_back();
        }
        bb.SetKind(kBBFallthru);
      }
      bb.RemoveMeStmt(&meStmt);
    }
  }
}

void HDSE::MarkMuListRequired(MapleMap<OStIdx, VarMeExpr*> &muList) {
  for (auto &pair : muList) {
    workList.push_front(pair.second);
  }
}

void HDSE::MarkChiNodeRequired(ChiMeNode &chiNode) {
  if (chiNode.GetIsLive()) {
    return;
  }
  chiNode.SetIsLive(true);
  workList.push_front(chiNode.GetRHS());
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

void HDSE::MarkVarDefByStmt(VarMeExpr &varExpr) {
  switch (varExpr.GetDefBy()) {
    case kDefByNo:
      break;
    case kDefByStmt: {
      auto *defStmt = varExpr.GetDefStmt();
      if (defStmt != nullptr) {
        MarkStmtRequired(*defStmt);
      }
      break;
    }
    case kDefByPhi: {
      MarkPhiRequired(varExpr.GetDefPhi());
      break;
    }
    case kDefByChi: {
      auto *defChi = &varExpr.GetDefChi();
      if (defChi != nullptr) {
        MarkChiNodeRequired(*defChi);
      }
      break;
    }
    case kDefByMustDef: {
      auto *mustDef = &varExpr.GetDefMustDef();
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

void HDSE::MarkRegDefByStmt(RegMeExpr &regMeExpr) {
  PregIdx regIdx = regMeExpr.GetRegIdx();
  if (regIdx == -kSregRetval0) {
    if (regMeExpr.GetDefStmt()) {
      MarkStmtRequired(*regMeExpr.GetDefStmt());
    }
    return;
  }
  switch (regMeExpr.GetDefBy()) {
    case kDefByNo:
      break;
    case kDefByStmt: {
      auto *defStmt = regMeExpr.GetDefStmt();
      if (defStmt != nullptr) {
        MarkStmtRequired(*defStmt);
      }
      break;
    }
    case kDefByPhi:
      MarkPhiRequired(regMeExpr.GetDefPhi());
      break;
    case kDefByMustDef: {
      MustDefMeNode *mustDef = &regMeExpr.GetDefMustDef();
      if (!mustDef->GetIsLive()) {
        mustDef->SetIsLive(true);
        MarkStmtRequired(*mustDef->GetBase());
      }
      break;
    }
    default:
      ASSERT(false, "MarkRegDefByStmt unexpected defBy value");
      break;
  }
}

void HDSE::PropagateUseLive(MeExpr &meExpr) {
  switch (meExpr.GetMeOp()) {
    case kMeOpVar: {
      auto &varMeExpr = static_cast<VarMeExpr&>(meExpr);
      MarkVarDefByStmt(varMeExpr);
      return;
    }
    case kMeOpReg: {
      auto &regMeExpr = static_cast<RegMeExpr&>(meExpr);
      MarkRegDefByStmt(regMeExpr);
      return;
    }
    default: {
      ASSERT(false, "MeOp ERROR");
      return;
    }
  }
}

bool HDSE::ExprNonDeletable(MeExpr &meExpr) {
  if (kOpcodeInfo.HasSideEffect(meExpr.GetOp())) {
    return true;
  }
  switch (meExpr.GetMeOp()) {
    case kMeOpReg: {
      auto &regMeExpr = static_cast<RegMeExpr&>(meExpr);
      return (regMeExpr.GetRegIdx() == -kSregThrownval);
    }
    case kMeOpVar: {
      auto &varMeExpr = static_cast<VarMeExpr&>(meExpr);
      return varMeExpr.IsVolatile(ssaTab) ||
             (decoupleStatic && ssaTab.GetSymbolOriginalStFromID(varMeExpr.GetOStIdx())->GetMIRSymbol()->IsGlobal());
    }
    case kMeOpIvar: {
      auto &opIvar = static_cast<IvarMeExpr&>(meExpr);
      return opIvar.IsVolatile() || ExprNonDeletable(*opIvar.GetBase());
    }
    case kMeOpOp: {
      if (meExpr.GetOp() == OP_gcmallocjarray) {
        return true;
      }
      break;
    }
    case kMeOpNary: {
      auto &opNary = static_cast<NaryMeExpr&>(meExpr);
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

bool HDSE::HasNonDeletableExpr(const MeStmt &meStmt) {
  Opcode op = meStmt.GetOp();
  switch (op) {
    case OP_dassign: {
      auto &dasgn = static_cast<const DassignMeStmt&>(meStmt);
      VarMeExpr *varMeExpr = dasgn.GetVarLHS();
      return (varMeExpr && varMeExpr->IsVolatile(ssaTab)) || ExprNonDeletable(*dasgn.GetRHS()) ||
          (hdseKeepRef && dasgn.Propagated()) || dasgn.GetWasMayDassign();
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

void HDSE::MarkLastUnconditionalGotoInPredBBRequired(BB &bb) {
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
    if ((IsBranch(op) || op == OP_retsub || op == OP_throw))   {
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
  SetExprNeeded(&meExpr);
  MeExprOp meOp  = meExpr.GetMeOp();
  switch (meOp) {
    case kMeOpVar:
    case kMeOpReg: {
      workList.push_front(&meExpr);
      break;
    }
    case kMeOpIvar: {
      auto *base = static_cast<IvarMeExpr&>(meExpr).GetBase();
      if (base != nullptr) {
        MarkSingleUseLive(*base);
      }
      break;
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

  // mark use
  MarkStmtUseLive(meStmt);

  // markBB
  MarkControlDependenceLive(*meStmt.GetBB());
}

bool HDSE::StmtMustRequired(const MeStmt &meStmt, const BB &bb) {
  Opcode op = meStmt.GetOp();
  // special opcode cannot be eliminated
  if (IsStmtMustRequire(op) || op == OP_comment) {
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
      if (StmtMustRequired(*pStmt, *bb)) {
        MarkStmtRequired(*pStmt);
      }
    }
  }
}

void HDSE::DseInit() {
  // Init bb's required flag
  if (&commonEntryBB != *bbVec.begin()) {
    bbRequired[commonEntryBB.GetBBId()] = true;
  }
  if (&commonExitBB != *bbVec.end()) {
    bbRequired[commonExitBB.GetBBId()] = true;
  }
  // Init all MeExpr to be dead;
  exprLive.resize(irMap.GetExprID(), false);

  for (auto *bb : bbVec) {
    if (bb == nullptr) {
      continue;
    }
    // mark phi nodes dead
    for (std::pair<OStIdx, MeVarPhiNode*> varPhiPair : bb->GetMevarPhiList()) {
      varPhiPair.second->SetIsLive(false);
    }
    for (std::pair<OStIdx, MeRegPhiNode*> regPhiPair : bb->GetMeRegPhiList()) {
      regPhiPair.second->SetIsLive(false);
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
