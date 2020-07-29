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
#include "me_abco.h"

// This phase removes redundant array bounds checks.
// ABCD: Eliminating Array Bounds Checks on Demand.
// Rastislav Bodik, Rajiv Gupta, Vivek Sarkar.
namespace maple {
bool MeABC::isDebug = false;
constexpr int kNumOpnds = 2;
constexpr int kPiListSize = 2;

void MeABC::AddEdgePair(ESSABaseNode &from, ESSABaseNode &to, int64 value, EdgeType type) {
  InequalEdge *pairEdge1 = inequalityGraph->AddEdge(from, to, value, type);
  InequalEdge *pairEdge2 = inequalityGraph->AddEdge(to, from, -value, type);
  pairEdge1->SetPairEdge(*pairEdge2);
  pairEdge2->SetPairEdge(*pairEdge1);
}

void MeABC::ABCCollectArrayExpr(MeStmt &meStmt, MeExpr &meExpr, bool isUpdate) {
  if (meExpr.GetOp() == OP_array) {
    auto *nMeExpr = static_cast<NaryMeExpr*>(&meExpr);
    if (nMeExpr->GetBoundCheck()) {
      if (!isUpdate) {
        CHECK_FATAL(arrayChecks.find(&meStmt) == arrayChecks.end(), "Must be");
        arrayChecks[&meStmt] = nMeExpr;
        CHECK_FATAL(meStmt.GetOp() == OP_dassign || meStmt.GetOp() == OP_iassign ||
                    meStmt.GetOp() == OP_maydassign, "must be");
        if (containsBB.find(meStmt.GetBB()) == containsBB.end()) {
          containsBB[meStmt.GetBB()] = { &meStmt };
        } else {
          containsBB[meStmt.GetBB()].push_back(&meStmt);
        }
      } else {
        CHECK_FATAL(arrayNewChecks.find(&meStmt) == arrayNewChecks.end(), "Must be");
        arrayNewChecks[&meStmt] = nMeExpr;
      }
    }
  }
  for (int32 i = 0; i < meExpr.GetNumOpnds(); ++i) {
    ABCCollectArrayExpr(meStmt, *(meExpr.GetOpnd(i)), isUpdate);
  }
}

bool MeABC::CollectABC() {
  auto eIt = meFunc->valid_end();
  for (auto bIt = meFunc->valid_begin(); bIt != eIt; ++bIt) {
    for (auto &meStmt : (*bIt)->GetMeStmts()) {
      for (size_t i = 0; i < meStmt.NumMeStmtOpnds(); ++i) {
        ABCCollectArrayExpr(meStmt, *(meStmt.GetOpnd(i)));
      }
    }
  }
  return !arrayChecks.empty();
}

bool MeABC::IsVirtualVar(const VarMeExpr &var, const SSATab &ssaTab) const {
  const OriginalSt *ost = ssaTab.GetOriginalStFromID(var.GetOStIdx());
  return ost->GetIndirectLev() > 0;
}

ESSABaseNode *MeABC::GetOrCreateRHSNode(MeExpr &expr) {
  auto &rhs = static_cast<VarMeExpr&>(expr);
  ESSABaseNode *result = nullptr;
  if (rhs.GetDefBy() != kDefByPhi) {
    result = inequalityGraph->GetOrCreateVarNode(rhs);
  } else {
    MePhiNode *defPhi = &(rhs.GetDefPhi());
    result = inequalityGraph->GetOrCreatePhiNode(*defPhi);
  }
  return result;
}

void MeABC::BuildPhiInGraph(MePhiNode &phi) {
  if (!IsPrimitivePureScalar(phi.GetLHS()->GetPrimType())) {
    MeExpr *varExpr = phi.GetLHS();
    varExpr = TryToResolveVar(*varExpr, false);
    if (varExpr != nullptr) {
      ESSAArrayNode *arrayNode = inequalityGraph->GetOrCreateArrayNode(*(phi.GetLHS()));
      ESSAVarNode *varNode = inequalityGraph->GetOrCreateVarNode(*varExpr);
      AddEdgePair(*arrayNode, *varNode, 0, EdgeType::kUpper);
    }
    return;
  }
  VarMeExpr *lhsExpr = static_cast<VarMeExpr*>(phi.GetLHS());
  if (lhsExpr != nullptr && IsVirtualVar(*lhsExpr, irMap->GetSSATab())) {
    return;
  }
  for (auto *phiRHS : phi.GetOpnds()) {
    AddUseDef(*phiRHS);
  }
}

void MeABC::BuildSoloPiInGraph(const PiassignMeStmt &piMeStmt) {
  VarMeExpr *piLHS = piMeStmt.GetLHS();
  VarMeExpr *piRHS = piMeStmt.GetRHS();
  AddUseDef(*piRHS);
  ESSAVarNode *piLHSNode = inequalityGraph->GetOrCreateVarNode(*piLHS);
  ESSABaseNode *piRHSNode = GetOrCreateRHSNode(*piRHS);
  (void)inequalityGraph->AddEdge(*piRHSNode, *piLHSNode, 0, EdgeType::kUpper);
  (void)inequalityGraph->AddEdge(*piLHSNode, *piRHSNode, 0, EdgeType::kLower);
}

bool MeABC::PiExecuteBeforeCurrentCheck(const PiassignMeStmt &piMeStmt) {
  BB *currentCheckBB = currentCheck->GetBB();
  BB *piBB = piMeStmt.GetBB();
  if (currentCheckBB != piBB) {
    return dom->Dominate(*piBB, *currentCheckBB);
  }
  MeStmt *lastMeStmt = piBB->GetLastMe();
  CHECK_FATAL(lastMeStmt->GetNextMeStmt() == nullptr, "must be");
  MeStmt *tmpMeStmt = piMeStmt.GetNextMeStmt();
  while (tmpMeStmt != nullptr) {
    if (tmpMeStmt == currentCheck) {
      return true;
    }
    tmpMeStmt = tmpMeStmt->GetNextMeStmt();
  }
  return false;
}

bool MeABC::BuildArrayCheckInGraph(MeStmt &meStmt) {
  CHECK_FATAL(meStmt.GetOp() == OP_piassign, "must be");
  auto *piMeStmt = static_cast<PiassignMeStmt*>(&meStmt);
  BuildSoloPiInGraph(*piMeStmt);
  if (!PiExecuteBeforeCurrentCheck(*piMeStmt)) {
    return true;
  }
  MeStmt *generatedByMeStmt = piMeStmt->GetGeneratedBy();
  CHECK_FATAL(arrayChecks.find(generatedByMeStmt) != arrayChecks.end(), "must be");
  NaryMeExpr *arrCheck = arrayChecks[generatedByMeStmt];
  MeExpr *opnd1 = arrCheck->GetOpnd(0);
  MeExpr *opnd2 = arrCheck->GetOpnd(1);
  CHECK_FATAL(opnd1->GetMeOp() == kMeOpVar, "must be");
  CHECK_FATAL(opnd1->GetPrimType() == PTY_ref, "must be");
  CHECK_FATAL(opnd2->GetMeOp() == kMeOpVar, "must be");
  AddUseDef(*opnd1);
  VarMeExpr *piLHS = piMeStmt->GetLHS();
  ESSAArrayNode *arrayNode = inequalityGraph->GetOrCreateArrayNode(*opnd1);
  ESSAVarNode *piLHSNode = inequalityGraph->GetOrCreateVarNode(*piLHS);
  (void)inequalityGraph->AddEdge(*arrayNode, *piLHSNode, -1, EdgeType::kNone);
  (void)inequalityGraph->AddEdge(*piLHSNode, *(inequalityGraph->GetOrCreateConstNode(0)), 0, EdgeType::kNone);
  return true;
}

Opcode TransCmpOp(Opcode cmpOp, Opcode brOp) {
  if (brOp == OP_brtrue) {
    return cmpOp;
  }
  switch (cmpOp) {
    case OP_ge:
      return OP_lt;
    case OP_le:
      return OP_gt;
    case OP_lt:
      return OP_ge;
    case OP_gt:
      return OP_le;
    case OP_ne:
      return OP_eq;
    case OP_eq:
      return OP_ne;
    default:
      CHECK_FATAL(false, "must be");
  }
}

bool MeABC::BuildBrMeStmtInGraph(MeStmt &meStmt) {
  auto *brMeStmt = static_cast<CondGotoMeStmt*>(&meStmt);
  BB *brBB = brMeStmt->GetBB();
  BB *brTarget = brBB->GetSucc(1);
  BB *brFallThru = brBB->GetSucc(0);
  CHECK_FATAL(brMeStmt->GetOffset() == brTarget->GetBBLabel(), "must be");
  MapleMap<BB*, std::vector<PiassignMeStmt*>> &brTargetPiBBList = brTarget->GetPiList();
  MapleMap<BB*, std::vector<PiassignMeStmt*>> &brFallThruPiBBList = brFallThru->GetPiList();
  if (brTargetPiBBList.find(brBB) == brTargetPiBBList.end()) {
    return false;
  }
  for (PiassignMeStmt* piMeStmt : brTargetPiBBList[brBB]) {
    BuildSoloPiInGraph(*piMeStmt);
  }
  for (PiassignMeStmt* piMeStmt : brFallThruPiBBList[brBB]) {
    BuildSoloPiInGraph(*piMeStmt);
  }
  std::vector<PiassignMeStmt*> &brTargetPiList = brTargetPiBBList[brBB];
  std::vector<PiassignMeStmt*> &brFallThruPiList = brFallThruPiBBList[brBB];
  ESSABaseNode *brTargetOpnd1 = nullptr;
  ESSABaseNode *brTargetOpnd2 = nullptr;
  ESSABaseNode *brFallThruOpnd1 = nullptr;
  ESSABaseNode *brFallThruOpnd2 = nullptr;
  auto *opMeExpr = static_cast<OpMeExpr*>(brMeStmt->GetOpnd());
  MeExpr *opOpnd1 = opMeExpr->GetOpnd(0);
  MeExpr *opOpnd2 = opMeExpr->GetOpnd(1);
  if (brTargetPiList.size() == kPiListSize) {
    brTargetOpnd1 = inequalityGraph->GetOrCreateVarNode(*brTargetPiList[0]->GetLHS());
    brTargetOpnd2 = inequalityGraph->GetOrCreateVarNode(*brTargetPiList[1]->GetLHS());
    brFallThruOpnd1 = inequalityGraph->GetOrCreateVarNode(*brFallThruPiList.at(0)->GetLHS());
    brFallThruOpnd2 = inequalityGraph->GetOrCreateVarNode(*brFallThruPiList.at(1)->GetLHS());
    AddUseDef(*opOpnd1);
    AddUseDef(*opOpnd2);
  } else if (opOpnd1->GetMeOp() == kMeOpConst) {
    brTargetOpnd1 = inequalityGraph->GetOrCreateConstNode(static_cast<ConstMeExpr*>(opOpnd1)->GetIntValue());
    brTargetOpnd2 = inequalityGraph->GetOrCreateVarNode(*brTargetPiList[0]->GetLHS());
    brFallThruOpnd1 = inequalityGraph->GetOrCreateConstNode(static_cast<ConstMeExpr*>(opOpnd1)->GetIntValue());
    brFallThruOpnd2 = inequalityGraph->GetOrCreateVarNode(*brFallThruPiList.at(0)->GetLHS());
    AddUseDef(*opOpnd2);
  } else if (opOpnd2->GetMeOp() == kMeOpConst) {
    brTargetOpnd1 = inequalityGraph->GetOrCreateVarNode(*brTargetPiList[0]->GetLHS());
    brTargetOpnd2 = inequalityGraph->GetOrCreateConstNode(static_cast<ConstMeExpr*>(opOpnd2)->GetIntValue());
    brFallThruOpnd1 = inequalityGraph->GetOrCreateVarNode(*brFallThruPiList.at(0)->GetLHS());
    brFallThruOpnd2 = inequalityGraph->GetOrCreateConstNode(static_cast<ConstMeExpr*>(opOpnd2)->GetIntValue());
    AddUseDef(*opOpnd1);
  } else {
    CHECK_FATAL(false, "impossible");
  }
  Opcode cmpOp = TransCmpOp(opMeExpr->GetOp(), brMeStmt->GetOp());
  switch (cmpOp) {
    case OP_ge: {
      (void)inequalityGraph->AddEdge(*brTargetOpnd1, *brTargetOpnd2, 0, EdgeType::kNone);
      (void)inequalityGraph->AddEdge(*brFallThruOpnd2, *brFallThruOpnd1, -1, EdgeType::kNone);
      break;
    }
    case OP_le: {
      (void)inequalityGraph->AddEdge(*brTargetOpnd2, *brTargetOpnd1, 0, EdgeType::kNone);
      (void)inequalityGraph->AddEdge(*brFallThruOpnd1, *brFallThruOpnd2, -1, EdgeType::kNone);
      break;
    }
    case OP_lt: {
      (void)inequalityGraph->AddEdge(*brTargetOpnd2, *brTargetOpnd1, -1, EdgeType::kNone);
      (void)inequalityGraph->AddEdge(*brFallThruOpnd1, *brFallThruOpnd2, 0, EdgeType::kNone);
      break;
    }
    case OP_gt: {
      (void)inequalityGraph->AddEdge(*brTargetOpnd1, *brTargetOpnd2, -1, EdgeType::kNone);
      (void)inequalityGraph->AddEdge(*brFallThruOpnd2, *brFallThruOpnd1, 0, EdgeType::kNone);
      break;
    }
    case OP_ne: {
      AddEdgePair(*brFallThruOpnd1, *brFallThruOpnd2, 0, EdgeType::kUpper);
      AddEdgePair(*brFallThruOpnd2, *brFallThruOpnd1, 0, EdgeType::kLower);
      break;
    }
    case OP_eq: {
      AddEdgePair(*brTargetOpnd1, *brTargetOpnd2, 0, EdgeType::kUpper);
      AddEdgePair(*brTargetOpnd2, *brTargetOpnd1, 0, EdgeType::kLower);
      break;
    }
    default:
      CHECK_FATAL(false, "impossible");
  }
  return true;
}

MeExpr *MeABC::TryToResolveVar(MeExpr &expr, bool isConst) {
  std::set<MePhiNode*> visitedPhi;
  ConstMeExpr dummyExpr(kInvalidExprID, nullptr);
  MeExpr *tmp = TryToResolveVar(expr, visitedPhi, dummyExpr, isConst);
  if (tmp != nullptr && tmp != &dummyExpr) {
    return tmp;
  }
  return nullptr;
}

MeExpr *MeABC::TryToResolveVar(MeExpr &expr, std::set<MePhiNode*> &visitedPhi, MeExpr &dummyExpr, bool isConst) {
  CHECK_FATAL(expr.GetMeOp() == kMeOpVar, "must be");
  auto *var = static_cast<VarMeExpr*>(&expr);

  if (var->GetDefBy() == kDefByStmt) {
    ASSERT_NOT_NULL(var->GetDefStmt());
    auto *rhs = var->GetDefStmt()->GetRHS();
    CHECK_NULL_FATAL(rhs);
    if (isConst && !rhs->IsLeaf()) {
      return nullptr;
    }
    if (isConst && rhs->GetMeOp() == kMeOpConst) {
      return rhs;
    }
    if (!isConst && rhs->GetMeOp() == kMeOpIvar) {
      return rhs;
    }
    if (isConst) {
      CHECK_FATAL(rhs->GetMeOp() == kMeOpVar, "must be");
      return TryToResolveVar(*rhs, visitedPhi, dummyExpr, isConst);
    }
  }

  if (var->GetDefBy() == kDefByPhi) {
    MePhiNode *phi = &(var->GetDefPhi());
    if (visitedPhi.find(phi) != visitedPhi.end()) {
      return &dummyExpr;
    }
    visitedPhi.insert(phi);
    std::set<MeExpr*> res;
    for (auto *phiOpnd : phi->GetOpnds()) {
      MeExpr *tmp = TryToResolveVar(*phiOpnd, visitedPhi, dummyExpr, isConst);
      if (tmp == nullptr) {
        return nullptr;
      }
      if (tmp != &dummyExpr) {
        res.insert(tmp);
      }
    }
    if (res.size() == 1) {
      return *(res.begin());
    } else if (res.empty()) {
      return &dummyExpr;
    }
  }

  return nullptr;
}

bool MeABC::BuildAssignInGraph(MeStmt &meStmt) {
  MeExpr *lhs = meStmt.GetLHS();
  MeExpr *rhs = meStmt.GetRHS();
  CHECK_NULL_FATAL(lhs);
  CHECK_NULL_FATAL(rhs);
  if (!lhs->IsLeaf()) {
    return false;
  }
  if (!IsPrimitivePureScalar(lhs->GetPrimType()) && rhs->GetOp() != OP_gcmallocjarray &&
      rhs->GetPrimType() != PTY_ref) {
    return false;
  }
  if (!rhs->IsLeaf() && rhs->GetMeOp() != kMeOpOp && rhs->GetMeOp() != kMeOpNary && rhs->GetPrimType() != PTY_ref) {
    return false;
  }
  if (rhs->GetMeOp() == kMeOpOp) {
    auto *opMeExpr = static_cast<OpMeExpr*>(rhs);
    switch (opMeExpr->GetOp()) {
      case OP_gcmallocjarray: {
        ESSAArrayNode *arrLength = inequalityGraph->GetOrCreateArrayNode(*lhs);
        CHECK_FATAL(opMeExpr->GetNumOpnds() == 1, "must be");
        ESSABaseNode *rhsNode = nullptr;
        if (opMeExpr->GetOpnd(0)->GetMeOp() == kMeOpVar) {
          rhsNode = GetOrCreateRHSNode(*opMeExpr->GetOpnd(0));
          AddUseDef(*(opMeExpr->GetOpnd(0)));
          // Try to resolve Var is assigned from Const
          MeExpr *varExpr = opMeExpr->GetOpnd(0);
          varExpr = TryToResolveVar(*varExpr, true);
          if (varExpr != nullptr) {
            CHECK_FATAL(varExpr->GetMeOp() == kMeOpConst, "must be");
            ESSAConstNode *constNode = inequalityGraph->GetOrCreateConstNode(
                static_cast<ConstMeExpr*>(varExpr)->GetIntValue());
            AddEdgePair(*arrLength, *constNode, 0, EdgeType::kNone);
          } else {
            AddEdgePair(*arrLength, *rhsNode, 0, EdgeType::kNone);
          }
        } else {
          CHECK_FATAL(opMeExpr->GetOpnd(0)->GetMeOp() == kMeOpConst, "must be");
          rhsNode = inequalityGraph->GetOrCreateConstNode(
              static_cast<ConstMeExpr*>(opMeExpr->GetOpnd(0))->GetIntValue());
          AddEdgePair(*arrLength, *rhsNode, 0, EdgeType::kNone);
        }
        return true;
      }
      case OP_sub: {
        if (MeOption::conservativeABCO) {
          return false;
        }
        CHECK_FATAL(opMeExpr->GetNumOpnds() == kNumOpnds, "must be");
        MeExpr *opnd1 = opMeExpr->GetOpnd(0);
        MeExpr *opnd2 = opMeExpr->GetOpnd(1);
        if (!opnd1->IsLeaf() || !opnd2->IsLeaf()) {
          return false;
        }
        ESSAVarNode *lhsNode = inequalityGraph->GetOrCreateVarNode(*lhs);
        if (opnd1->GetMeOp() == kMeOpConst || opnd2->GetMeOp() == kMeOpConst) {
          if (opnd1->GetMeOp() == kMeOpConst && opnd2->GetMeOp() == kMeOpConst) {
            CHECK_FATAL(false, "consider this pattern");
          } else if (opnd2->GetMeOp() == kMeOpConst) {
            visited.clear();
            if (!HasRelativeWithLength(*opnd1)) {
              return false;
            }
            ESSABaseNode *rhsNode = GetOrCreateRHSNode(*opnd1);
            AddUseDef(*opnd1);
            AddEdgePair(*rhsNode, *lhsNode,
                -static_cast<ConstMeExpr*>(opnd2)->GetIntValue(), EdgeType::kUpper);
            AddEdgePair(*lhsNode, *rhsNode,
                static_cast<ConstMeExpr*>(opnd2)->GetIntValue(), EdgeType::kLower);
            return true;
          }else {
            // support this pattern later
            return false;
          }
        } else if (opnd1->GetMeOp() == kMeOpVar && opnd2->GetMeOp() == kMeOpVar) {
          // Try to resolve Var is assigned from Const
          opnd2 = TryToResolveVar(*opnd2, true);
          if (opnd2 == nullptr) {
            return false;
          }
          CHECK_FATAL(opnd2->GetMeOp() == kMeOpConst, "must be");
          visited.clear();
          if (!HasRelativeWithLength(*opnd1)) {
            return false;
          }
          ESSABaseNode *rhsNode = GetOrCreateRHSNode(*opnd1);
          AddUseDef(*opnd1);
          AddEdgePair(*rhsNode, *lhsNode,
              -static_cast<ConstMeExpr*>(opnd2)->GetIntValue(), EdgeType::kUpper);
          AddEdgePair(*lhsNode, *rhsNode,
              static_cast<ConstMeExpr*>(opnd2)->GetIntValue(), EdgeType::kLower);
          return true;
        } else {
          // support this pattern later
          return false;
        }
        CHECK_FATAL(false, "impossible");
      }
      case OP_add: {
        if (MeOption::conservativeABCO) {
          return false;
        }
        CHECK_FATAL(opMeExpr->GetNumOpnds() == kNumOpnds, "must be");
        MeExpr *opnd1 = opMeExpr->GetOpnd(0);
        MeExpr *opnd2 = opMeExpr->GetOpnd(1);
        if (!opnd1->IsLeaf() || !opnd2->IsLeaf()) {
          return false;
        }
        ESSAVarNode *lhsNode = inequalityGraph->GetOrCreateVarNode(*lhs);
        if (opnd1->GetMeOp() != kMeOpConst && opnd2->GetMeOp() != kMeOpConst) {
          CHECK_FATAL(opnd1->GetMeOp() == kMeOpVar, "must be");
          CHECK_FATAL(opnd2->GetMeOp() == kMeOpVar, "must be");
          MeExpr *tmpVar = opnd2;
          tmpVar = TryToResolveVar(*tmpVar, true);
          if (tmpVar != nullptr) {
            CHECK_FATAL(tmpVar->GetMeOp() == kMeOpConst, "must be");
            visited.clear();
            if (!HasRelativeWithLength(*opnd1)) {
              return false;
            }
            AddUseDef(*opnd1);
            ESSABaseNode *rhsNode = GetOrCreateRHSNode(*opnd1);
            AddEdgePair(*rhsNode, *lhsNode,
                static_cast<ConstMeExpr*>(tmpVar)->GetIntValue(), EdgeType::kUpper);
            AddEdgePair(*lhsNode, *rhsNode,
                -static_cast<ConstMeExpr*>(tmpVar)->GetIntValue(), EdgeType::kLower);
            return true;
          }
          visited.clear();
          if (HasRelativeWithLength(*opnd1)) {
            unresolveEdge[std::make_pair(lhs, opnd1)] = opnd2;
            AddUseDef(*opnd1);
            AddUseDef(*opnd2);
          }
          visited.clear();
          if (HasRelativeWithLength(*opnd2)) {
            unresolveEdge[std::make_pair(lhs, opnd2)] = opnd1;
            AddUseDef(*opnd1);
            AddUseDef(*opnd2);
          }
          return false;
        }
        if (opnd1->GetMeOp() == kMeOpConst && opnd2->GetMeOp() == kMeOpConst) {
          CHECK_FATAL(false, "consider this pattern");
        } else if (opnd2->GetMeOp() == kMeOpConst) {
          visited.clear();
          if (!HasRelativeWithLength(*opnd1)) {
            return false;
          }
          ESSABaseNode *rhsNode = GetOrCreateRHSNode(*opnd1);
          AddUseDef(*opnd1);
          AddEdgePair(*rhsNode, *lhsNode,
              static_cast<ConstMeExpr*>(opnd2)->GetIntValue(), EdgeType::kUpper);
          AddEdgePair(*lhsNode, *rhsNode,
              -static_cast<ConstMeExpr*>(opnd2)->GetIntValue(), EdgeType::kLower);
          return true;
        } else {
          visited.clear();
          if (!HasRelativeWithLength(*opnd2)) {
            return false;
          }
          ESSABaseNode *rhsNode = GetOrCreateRHSNode(*opnd2);
          AddUseDef(*opnd2);
          AddEdgePair(*rhsNode, *lhsNode,
              static_cast<ConstMeExpr*>(opnd1)->GetIntValue(), EdgeType::kUpper);
          AddEdgePair(*lhsNode, *rhsNode,
              -static_cast<ConstMeExpr*>(opnd1)->GetIntValue(), EdgeType::kLower);
          return true;
        }
        CHECK_FATAL(false, "impossible");
      }
      case OP_band: {
        CHECK_FATAL(opMeExpr->GetNumOpnds() == kNumOpnds, "must be");
        MeExpr *opnd1 = opMeExpr->GetOpnd(0);
        MeExpr *opnd2 = opMeExpr->GetOpnd(1);
        if (!opnd1->IsLeaf() || !opnd2->IsLeaf()) {
          return false;
        }
        ESSAVarNode *lhsNode = inequalityGraph->GetOrCreateVarNode(*lhs);
        ESSABaseNode *rhsNode1 = nullptr;
        ESSABaseNode *rhsNode2 = nullptr;
        if (IsUnsignedInteger(opnd1->GetPrimType())) {
          AddUseDef(*opnd1);
          rhsNode1 = GetOrCreateRHSNode(*opnd1);
        } else if (opnd1->GetMeOp() == kMeOpConst && static_cast<ConstMeExpr*>(opnd1)->GetIntValue() >= 0) {
          rhsNode1 = inequalityGraph->GetOrCreateConstNode(static_cast<ConstMeExpr*>(opnd1)->GetIntValue());
        }
        if (IsUnsignedInteger(opnd2->GetPrimType())) {
          AddUseDef(*opnd2);
          rhsNode2 = GetOrCreateRHSNode(*opnd2);
        } else if (opnd2->GetMeOp() == kMeOpConst && static_cast<ConstMeExpr*>(opnd2)->GetIntValue() >= 0) {
          rhsNode2 = inequalityGraph->GetOrCreateConstNode(static_cast<ConstMeExpr*>(opnd2)->GetIntValue());
        }
        if (rhsNode1 == nullptr && rhsNode2 == nullptr) {
          return false;
        }
        if (rhsNode1 != nullptr) {
          (void)inequalityGraph->AddEdge(*rhsNode1, *lhsNode, 0, EdgeType::kUpper);
        }
        if (rhsNode2 != nullptr) {
          (void)inequalityGraph->AddEdge(*rhsNode2, *lhsNode, 0, EdgeType::kUpper);
        }
        (void)inequalityGraph->AddEdge(*lhsNode, *(inequalityGraph->GetOrCreateConstNode(0)), 0, EdgeType::kNone);
        return true;
      }
      case OP_div: {
        if (MeOption::conservativeABCO) {
          return false;
        }
        CHECK_FATAL(opMeExpr->GetNumOpnds() == kNumOpnds, "must be");
        MeExpr *opnd1 = opMeExpr->GetOpnd(0);
        MeExpr *opnd2 = opMeExpr->GetOpnd(1);
        if (!opnd1->IsLeaf() || !opnd2->IsLeaf()) {
          return false;
        }
        if (opnd2->GetMeOp() == kMeOpConst) {
          if (static_cast<ConstMeExpr*>(opnd2)->GetIntValue() > 0) {
            visited.clear();
            if (HasRelativeWithLength(*opnd1)) {
              unresolveEdge[std::make_pair(lhs, nullptr)] = opnd2;
              AddUseDef(*opnd1);
            }
          }
          return false;
        }
        MeExpr *varExpr = opnd2;
        varExpr = TryToResolveVar(*varExpr, true);
        if (varExpr != nullptr) {
          CHECK_FATAL(varExpr->GetMeOp() == kMeOpConst, "must be");
          if (static_cast<ConstMeExpr*>(varExpr)->GetIntValue() > 0) {
            visited.clear();
            if (HasRelativeWithLength(*opnd1)) {
              unresolveEdge[std::make_pair(lhs, nullptr)] = opnd2;
              AddUseDef(*opnd1);
            }
          }
        }
        return false;
      }
      default:
        return false;
    }
  } else if (rhs->GetMeOp() == kMeOpNary) {
    auto *nary = static_cast<NaryMeExpr*>(rhs);
    if (nary->GetIntrinsic() != INTRN_JAVA_ARRAY_LENGTH) {
      return false;
    }
    ESSAVarNode *lhsNode = inequalityGraph->GetOrCreateVarNode(*lhs);
    CHECK_FATAL(nary->GetOpnds().size() == 1, "must be");
    MeExpr *array = nary->GetOpnd(0);
    if (IsPrimitivePureScalar(array->GetPrimType())) {
      return false;
    }
    CHECK_FATAL(array->GetPrimType() == PTY_ref, "must be");
    AddUseDef(*array);
    ESSAArrayNode *arrLength = inequalityGraph->GetOrCreateArrayNode(*array);
    (void)inequalityGraph->AddEdge(*arrLength, *lhsNode, 0, EdgeType::kUpper);
    (void)inequalityGraph->AddEdge(*lhsNode, *arrLength, 0, EdgeType::kLower);
    return true;
  } else if (rhs->GetPrimType() == PTY_ref) {
    if ((rhs->GetMeOp() != kMeOpVar && rhs->GetMeOp() != kMeOpIvar) || (lhs->GetPrimType() != PTY_ref)) {
      return false;
    }
    ESSAVarNode *ivarNode = inequalityGraph->GetOrCreateVarNode(*rhs);
    ESSAArrayNode *arrayNode = inequalityGraph->GetOrCreateArrayNode(*lhs);
    AddEdgePair(*ivarNode, *arrayNode, 0, EdgeType::kUpper);
    if (rhs->GetMeOp() == kMeOpVar) {
      AddUseDef(*rhs);
    }
    return true;
  } else {
    CHECK_FATAL(rhs->GetMeOp() == kMeOpVar || rhs->GetMeOp() == kMeOpConst, "must be");
    ESSAVarNode *lhsNode = inequalityGraph->GetOrCreateVarNode(*lhs);
    if (rhs->GetMeOp() == kMeOpVar) {
      AddUseDef(*rhs);
      ESSABaseNode *rhsNode = GetOrCreateRHSNode(*rhs);
      AddEdgePair(*rhsNode, *lhsNode, 0, EdgeType::kUpper);
      AddEdgePair(*lhsNode, *rhsNode, 0, EdgeType::kLower);
    } else {
      ESSAConstNode *rhsNode = inequalityGraph->GetOrCreateConstNode(static_cast<ConstMeExpr*>(rhs)->GetIntValue());
      AddEdgePair(*rhsNode, *lhsNode, 0, EdgeType::kUpper);
      AddEdgePair(*lhsNode, *rhsNode, 0, EdgeType::kLower);
    }
    return true;
  }
}

bool MeABC::HasRelativeWithLength(MeExpr &meExpr) {
  if (MeOption::aggressiveABCO) {
    return true;
  }
  if (meExpr.GetMeOp() != kMeOpVar) {
    return false;
  }
  auto *varOpnd1 = static_cast<VarMeExpr*>(&meExpr);
  switch (varOpnd1->GetDefBy()) {
    case kDefByStmt: {
      MeStmt *meStmt = varOpnd1->GetDefStmt();
      if (meStmt->GetOp() == OP_dassign) {
        DassignMeStmt *dassignMeStmt = static_cast<DassignMeStmt*>(meStmt);
        MeExpr *rhs = dassignMeStmt->GetRHS();
        if (rhs->GetMeOp() == kMeOpVar) {
          return HasRelativeWithLength(*rhs);
        } else if (rhs->GetMeOp() == kMeOpOp && (rhs->GetOp() == OP_add || rhs->GetOp() == OP_sub)) {
          auto *opMeExpr = static_cast<OpMeExpr*>(rhs);
          CHECK_FATAL(opMeExpr->GetNumOpnds() == kNumOpnds, "must be");
          MeExpr *opnd1 = opMeExpr->GetOpnd(0);
          MeExpr *opnd2 = opMeExpr->GetOpnd(1);
          return HasRelativeWithLength(*opnd1) || HasRelativeWithLength(*opnd2);
        }
      } else if (meStmt->GetOp() == OP_piassign) {
        PiassignMeStmt *piMeStmt = static_cast<PiassignMeStmt*>(meStmt);
        if (piMeStmt->GetGeneratedBy()->IsCondBr()) {
          auto *opMeExpr = static_cast<OpMeExpr*>(piMeStmt->GetGeneratedBy()->GetOpnd(0));
          MeExpr *opOpnd1 = opMeExpr->GetOpnd(0);
          MeExpr *opOpnd2 = opMeExpr->GetOpnd(1);
          return HasRelativeWithLength(*opOpnd1) || HasRelativeWithLength(*opOpnd2);
        } else {
          return HasRelativeWithLength(*piMeStmt->GetRHS());
        }
      }
      break;
    }
    case kDefByChi: {
      MeStmt *meStmt = varOpnd1->GetDefChi().GetBase();
      MeExpr *rhs = meStmt->GetRHS();
      if (rhs == nullptr || rhs->GetMeOp() != kMeOpNary) {
        return false;
      }
      auto *nary = static_cast<NaryMeExpr*>(rhs);
      if (nary->GetIntrinsic() != INTRN_JAVA_ARRAY_LENGTH) {
        return false;
      }
      return true;
    }
    case kDefByPhi: {
      MePhiNode &defPhi = varOpnd1->GetDefPhi();
      if (visited.find(&defPhi) != visited.end()) {
        return false;
      }
      (void)visited.insert(&defPhi);
      for (MeExpr *expr : defPhi.GetOpnds()) {
        if (HasRelativeWithLength(*expr)) {
          return true;
        }
      }
      break;
    }
    case kDefByNo:
    case kDefByMustDef:
      break;
  }
  return false;
}

bool MeABC::BuildStmtInGraph(MeStmt &meStmt) {
  bool validInsn = false;
  switch (meStmt.GetOp()) {
    case OP_dassign:
    case OP_maydassign: {
      validInsn = BuildAssignInGraph(meStmt);
      break;
    }
    case OP_piassign: {
      validInsn = BuildArrayCheckInGraph(meStmt);
      break;
    }
    case OP_brfalse:
    case OP_brtrue: {
      validInsn = BuildBrMeStmtInGraph(meStmt);
      break;
    }
    default:
      break;
  }
  return validInsn;
}
void MeABC::AddCareInsn(MeStmt &defS) {
  MeStmt *defStmt = &defS;
  if (careMeStmts.find(defStmt) == careMeStmts.end()) {
    CarePoint *carePoint = GetMemPool()->New<CarePoint>(CarePoint::CareKind::kMeStmt);
    carePoint->SetMeStmt(*defStmt);
    carePoints.push_back(carePoint);
    careMeStmts.insert(defStmt);
  }
}
void MeABC::AddCarePhi(MePhiNode &defP) {
  MePhiNode *defPhi = &defP;
  if (careMePhis.find(defPhi) == careMePhis.end()) {
    CarePoint *carePoint = GetMemPool()->New<CarePoint>(CarePoint::CareKind::kMePhi);
    carePoint->SetMePhi(*defPhi);
    carePoints.push_back(carePoint);
    careMePhis.insert(defPhi);
  }
}
void MeABC::AddUseDef(MeExpr &meExpr) {
  CHECK_FATAL(meExpr.GetMeOp() == kMeOpVar, "must be");
  auto *varOpnd1 = static_cast<VarMeExpr*>(&meExpr);
  switch (varOpnd1->GetDefBy()) {
    case kDefByStmt: {
      MeStmt *meStmt = varOpnd1->GetDefStmt();
      MeStmt *defStmt = meStmt;
      if (meStmt->GetOp() == OP_piassign) {
        MeStmt *tmp = static_cast<PiassignMeStmt*>(meStmt)->GetGeneratedBy();
        if (tmp->IsCondBr()) {
          defStmt = tmp;
        }
      }
      AddCareInsn(*defStmt);
      break;
    }
    case kDefByChi: {
      MeStmt *defStmt = varOpnd1->GetDefChi().GetBase();
      AddCareInsn(*defStmt);
      break;
    }
    case kDefByPhi: {
      MePhiNode *defPhi = &varOpnd1->GetDefPhi();
      AddCarePhi(*defPhi);
      break;
    }
    case kDefByNo: {
      CHECK_FATAL(varOpnd1->IsZeroVersion(irMap->GetSSATab()), "must be");
      break;
    }
    case kDefByMustDef: {
      break;
    }
  }
}

void MeABC::CollectCareInsns() {
  for (auto pair : arrayChecks) {
    MeStmt *meStmt = pair.first;
    for (size_t i = 0; i < meStmt->NumMeStmtOpnds(); ++i) {
      ABCCollectArrayExpr(*meStmt, *(meStmt->GetOpnd(i)), true);
    }
  }
  CHECK_FATAL(arrayNewChecks.size() == arrayChecks.size(), "must be");
}

bool MeABC::ProveGreaterZ(const MeExpr &weight) {
  ESSABaseNode &zNode = inequalityGraph->GetNode(0);
  ESSABaseNode *idxNode = &(inequalityGraph->GetNode(weight));
  bool lowerResult = prove->DemandProve(zNode, *idxNode, kLower);
  return lowerResult;
}

void MeABC::ReSolveEdge() {
  for (auto pair : unresolveEdge) {
    MeExpr *weight = pair.second;
    if (!inequalityGraph->HasNode(*weight)) {
      continue;
    }
    if (ProveGreaterZ(*weight)) {
      MeExpr *lhs = pair.first.first;
      MeExpr *rhs = pair.first.second;
      ESSAVarNode *lhsNode = inequalityGraph->GetOrCreateVarNode(*lhs);
      ESSABaseNode *rhsNode = nullptr;
      if (rhs != nullptr) {
        rhsNode = GetOrCreateRHSNode(*rhs);
      } else {
        rhsNode = &(inequalityGraph->GetNode(0));
      }
      (void)inequalityGraph->AddEdge(*lhsNode, *rhsNode, 0, EdgeType::kNone);
    }
  }
}

void MeABC::BuildInequalityGraph() {
  for (size_t i = 0; i < carePoints.size(); ++i) {
    CarePoint *cp = carePoints[i];
    if (cp->IsCareMeStmt()) {
      (void)BuildStmtInGraph(*(cp->GetMeStmt()));
    } else {
      BuildPhiInGraph(*(cp->GetMePhi()));
    }
  }
  inequalityGraph->ConnectTrivalEdge();
  ReSolveEdge();
}

// opnds <= opnd1.length
bool MeABC::IsLessOrEuqal(const MeExpr &opnd1, const MeExpr &opnd2) {
  CHECK_FATAL(opnd1.GetMeOp() == kMeOpVar, "must be");
  CHECK_FATAL(opnd1.GetPrimType() == PTY_ref, "must be");
  if (!inequalityGraph->HasNode(opnd1)) {
    return false;
  }
  if (opnd2.GetMeOp() == kMeOpVar && !inequalityGraph->HasNode(opnd2)) {
    return false;
  }
  if (prove->IsLessOrEqual(opnd1, opnd2)) {
    return true;
  } else {
    return false;
  }
}

void MeABC::FindRedundantABC(MeStmt &meStmt, NaryMeExpr &naryMeExpr) {
  MeExpr *opnd1 = naryMeExpr.GetOpnd(0);
  MeExpr *opnd2 = naryMeExpr.GetOpnd(1);
  CHECK_FATAL(opnd1->GetMeOp() == kMeOpVar, "must be");
  CHECK_FATAL(opnd1->GetPrimType() == PTY_ref, "must be");
  if (!inequalityGraph->HasNode(*opnd1)) {
    if (MeABC::isDebug) {
      LogInfo::MapleLogger() << "Array Node Not Found" << '\n';
      meStmt.Dump(irMap);
    }
    return;
  }
  if (opnd2->GetMeOp() == kMeOpVar && !inequalityGraph->HasNode(*opnd2)) {
    if (MeABC::isDebug) {
      LogInfo::MapleLogger() << "Array Index Not Found" << '\n';
      meStmt.Dump(irMap);
    }
    return;
  }
  if (prove->DemandProve(*opnd1, *opnd2)) {
    if (MeABC::isDebug) {
      LogInfo::MapleLogger() << "Find One OPT" << '\n';
      meStmt.Dump(irMap);
    }
    targetMeStmt.insert(&meStmt);
  } else {
    if (MeABC::isDebug) {
      LogInfo::MapleLogger() << "Can not OPT" << '\n';
      meStmt.Dump(irMap);
    }
  }
}

MeExpr *MeABC::ReplaceArrayExpr(MeExpr &rhs, MeExpr &naryMeExpr, MeStmt *ivarStmt) {
  if (rhs.IsLeaf()) {
    return nullptr;
  }
  if (&rhs == &naryMeExpr) {
    auto *oldNaryMeExpr = static_cast<NaryMeExpr*>(&naryMeExpr);
    NaryMeExpr tmpNaryMeExpr(&(irMap->GetIRMapAlloc()), kInvalidExprID, *oldNaryMeExpr);
    CHECK_FATAL(tmpNaryMeExpr.GetBoundCheck(), "must be");
    tmpNaryMeExpr.SetBoundCheck(false);
    MeExpr *newNaryMeExpr = irMap->HashMeExpr(tmpNaryMeExpr);
    return newNaryMeExpr;
  }
  if (rhs.GetMeOp() == kMeOpOp) {
    auto &oldOpMeExpr = static_cast<OpMeExpr&>(rhs);
    OpMeExpr newMeExpr(oldOpMeExpr, kInvalidExprID);
    for (size_t i = 0; i < kOperandNumTernary; i++) {
      if (oldOpMeExpr.GetOpnd(i) == nullptr) {
        continue;
      }
      MeExpr *newOpnd = ReplaceArrayExpr(*(oldOpMeExpr.GetOpnd(i)), naryMeExpr, ivarStmt);
      newMeExpr.SetOpnd(i, newOpnd);
    }
    MeExpr *newOpMeExpr = irMap->HashMeExpr(newMeExpr);
    return newOpMeExpr;
  }
  CHECK_FATAL(rhs.GetMeOp() == kMeOpIvar, "must be");
  MeExpr *newBase = ReplaceArrayExpr(*static_cast<IvarMeExpr&>(rhs).GetBase(), naryMeExpr, ivarStmt);
  CHECK_NULL_FATAL(newBase);
  MeExpr *newIvarExpr = nullptr;
  if (ivarStmt == nullptr) {
    auto *oldIvarExpr = static_cast<IvarMeExpr*>(&rhs);
    IvarMeExpr tmpIvarMeExpr(kInvalidExprID, *oldIvarExpr);
    tmpIvarMeExpr.SetBase(newBase);
    newIvarExpr = irMap->HashMeExpr(tmpIvarMeExpr);
  } else {
    auto &iassingMeStmt = *static_cast<IassignMeStmt*>(ivarStmt);
    newIvarExpr = irMap->BuildLHSIvar(*newBase, iassingMeStmt, iassingMeStmt.GetLHSVal()->GetFieldID());
  }
  return newIvarExpr;
}

bool MeABC::CleanABCInStmt(MeStmt &meStmt, NaryMeExpr &naryMeExpr) {
  bool replaced = false;
  switch (meStmt.GetOp()) {
    case OP_dassign:
    case OP_maydassign: {
      MeExpr *rhs = meStmt.GetRHS();
      ASSERT_NOT_NULL(rhs);
      CHECK_FATAL(!rhs->IsLeaf(), "must be");
      MeExpr *newRHS = ReplaceArrayExpr(*rhs, naryMeExpr, nullptr);
      replaced = rhs != newRHS;
      meStmt.SetOpnd(0, newRHS);
      break;
    }
    case OP_iassign: {
      MeExpr *lhs = static_cast<IassignMeStmt*>(&meStmt)->GetLHSVal();
      MeExpr *newLHS = ReplaceArrayExpr(*lhs, naryMeExpr, &meStmt);
      replaced = lhs != newLHS;
      static_cast<IassignMeStmt*>(&meStmt)->SetLHSVal(static_cast<IvarMeExpr*>(newLHS));
      break;
    }
    default:
      CHECK_FATAL(false, "must be");
  }
  return replaced;
}

void MeABC::DeleteABC() {
  for (MeStmt *meStmt : targetMeStmt) {
    NaryMeExpr *naryMeExpr = arrayChecks[meStmt];
    bool replaced = CleanABCInStmt(*meStmt, *naryMeExpr);
    CHECK_FATAL(replaced, "must be");
  }
  inequalityGraph = nullptr;
  prove = nullptr;
  ssi = nullptr;
}

void MeABC::InitNewStartPoint(MeStmt &meStmt, MeExpr &opnd1, MeExpr &opnd2, bool clearGraph) {
  careMeStmts.clear();
  careMePhis.clear();
  carePoints.clear();
  unresolveEdge.clear();
  if (clearGraph) {
    inequalityGraph = std::make_unique<InequalityGraph>(*meFunc);
  }
  CHECK_FATAL(inequalityGraph != nullptr, "inequalityGraph is nullptr");
  prove = std::make_unique<ABCD>(*inequalityGraph);
  CHECK_FATAL(prove != nullptr, "prove is nullptr");
  CHECK_FATAL(opnd1.GetMeOp() == kMeOpVar, "must be");
  AddUseDef(opnd1);
  if (opnd2.GetMeOp() == kMeOpVar) {
    AddUseDef(opnd2);
  } else {
    CHECK_FATAL(opnd2.GetMeOp() == kMeOpConst, "must be");
    (void)inequalityGraph->GetOrCreateConstNode(static_cast<ConstMeExpr*>(&opnd2)->GetIntValue());
  }
  BB *curBB = meStmt.GetBB();
  if (curBB->GetPiList().size()) {
    for (auto pair : curBB->GetPiList()) {
      CHECK_FATAL(pair.second.size() >= 1, "must be");
      PiassignMeStmt *pi = pair.second[0];
      AddUseDef(*pi->GetLHS());
    }
  }
  currentCheck = &meStmt;
}

void MeABC::ProcessCallParameters(CallMeStmt &callNode) {
  MIRFunction *callFunc = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(callNode.GetPUIdx());
  if (callFunc->GetBaseClassName().compare("Ljava_2Flang_2FSystem_3B") == 0 &&
      callFunc->GetBaseFuncName().compare("arraycopy") == 0) {
    MeExpr *opnd1 = callNode.GetOpnd(1); // The 1th parameter.
    MeExpr *opnd3 = callNode.GetOpnd(3); // The 3th parameter.
    CHECK_FATAL(opnd1->GetOp() == OP_dread, "must be");
    CHECK_FATAL(opnd3->GetOp() == OP_dread, "must be");
    ConstMeExpr *constOpnd1 = nullptr;
    ConstMeExpr *constOpnd3 = nullptr;
    VarMeExpr *varOpnd1 = static_cast<VarMeExpr*>(opnd1);
    if (varOpnd1->GetDefBy() == kDefByStmt) {
      MeStmt *defOpnd1Stmt = varOpnd1->GetDefStmt();
      if (defOpnd1Stmt->GetOp() == OP_dassign && defOpnd1Stmt->GetOpnd(0)->GetOp() == OP_constval) {
        constOpnd1 = static_cast<ConstMeExpr*>(defOpnd1Stmt->GetOpnd(0));
      }
    }
    VarMeExpr *varOpnd3 = static_cast<VarMeExpr*>(opnd3);
    if (varOpnd3->GetDefBy() == kDefByStmt) {
      MeStmt *defOpnd3Stmt = varOpnd3->GetDefStmt();
      if (defOpnd3Stmt->GetOp() == OP_dassign && defOpnd3Stmt->GetOpnd(0)->GetOp() == OP_constval) {
        constOpnd3 = static_cast<ConstMeExpr*>(defOpnd3Stmt->GetOpnd(0));
      }
    }
    if (constOpnd1 != nullptr && constOpnd3 != nullptr && constOpnd1->IsZero() && constOpnd3->IsZero()) {
      MeExpr *opnd0 = callNode.GetOpnd(0); // The 0th parameter.
      MeExpr *opnd2 = callNode.GetOpnd(2); // The 2th parameter.
      MeExpr *opnd4 = callNode.GetOpnd(4); // The 4th parameter.
      InitNewStartPoint(callNode, *opnd0, *opnd4);
      BuildInequalityGraph();
      bool opnd0Proved = IsLessOrEuqal(*opnd0, *opnd4);
      InitNewStartPoint(callNode, *opnd2, *opnd4, false);
      BuildInequalityGraph();
      bool opnd2Proved = IsLessOrEuqal(*opnd2, *opnd4);
      bool boundaryCheck = false;
      if (opnd0Proved && opnd2Proved) {
        boundaryCheck = true;
      }
      CHECK_FATAL(opnd0->GetOp() == OP_dread, "must be");
      CHECK_FATAL(opnd2->GetOp() == OP_dread, "must be");
      VarMeExpr *array0 = static_cast<VarMeExpr*>(opnd0);
      VarMeExpr *array2 = static_cast<VarMeExpr*>(opnd2);
      bool nullCheck = false;
      if (!array0->GetMaybeNull() && !array2->GetMaybeNull()) {
        nullCheck = true;
      }
      bool typeCheck = false;
      bool isScalar = false;
      if (array0->GetInferredTyIdx() != 0 && array2->GetInferredTyIdx() != 0) {
        MIRType *type0 = GlobalTables::GetTypeTable().GetTypeFromTyIdx(array0->GetInferredTyIdx());
        MIRType *type2 = GlobalTables::GetTypeTable().GetTypeFromTyIdx(array2->GetInferredTyIdx());
        if (type0 && type2 && type0->IsMIRJarrayType() && type2->IsMIRJarrayType()) {
          MIRArrayType *arrayType0 = static_cast<MIRArrayType*>(type0);
          MIRArrayType *arrayType2 = static_cast<MIRArrayType*>(type2);
          if (arrayType0->GetElemTyIdx() == arrayType2->GetElemTyIdx()) {
            typeCheck = true;
            if (arrayType0->GetElemType()->IsScalarType()) {
              isScalar = true;
            }
          }
        }
      }
      if (boundaryCheck && nullCheck && typeCheck) {
        if (isScalar) {
          MIRFunction *arrayCopyFunc = meFunc->GetMIRModule().GetMIRBuilder()->GetOrCreateFunction(
              "Native_java_lang_System_arraycopyCharUnchecked___3CI_3CII", (TyIdx) (PTY_void));
          callNode.SetPUIdx(arrayCopyFunc->GetPuidx());
          return;
        } // ref can be handled, but need to extend ABI.
      }
    }
    if (callFunc->GetBaseFuncNameWithType().compare(
        "arraycopy_7C_28Ljava_2Flang_2FObject_3BILjava_2Flang_2FObject_3BII_29V") == 0) {
      MIRFunction *arrayCopyFunc = meFunc->GetMIRModule().GetMIRBuilder()->GetOrCreateFunction(
          "Native_java_lang_System_arraycopy__Ljava_lang_Object_2ILjava_lang_Object_2II", (TyIdx)(PTY_void));
      callNode.SetPUIdx(arrayCopyFunc->GetPuidx());
    }
  }
}

void MeABC::ExecuteABCO() {
  MeABC::isDebug = false;
  if (CollectABC()) {
    ssi->ConvertToSSI();
    CollectCareInsns();
    for (auto pair : arrayNewChecks) {
      InitNewStartPoint(*(pair.first), *((static_cast<NaryMeExpr *>(pair.second))->GetOpnd(0)),
                        *((static_cast<NaryMeExpr *>(pair.second))->GetOpnd(1)));
      BuildInequalityGraph();
      if (MeABC::isDebug) {
        meFunc->GetTheCfg()->DumpToFile(meFunc->GetName());
        inequalityGraph->DumpDotFile(*irMap, DumpType::kDumpUpperAndNone);
        inequalityGraph->DumpDotFile(*irMap, DumpType::kDumpLowerAndNone);
      }
      FindRedundantABC(*(pair.first), *(static_cast<NaryMeExpr*>(pair.second)));
    }
    ssi->ConvertToSSA();
    DeleteABC();
  }
}

AnalysisResult *MeDoABCOpt::Run(MeFunction *func, MeFuncResultMgr *frm, ModuleResultMgr*) {
  if (func->GetSecondPass()) {
    return nullptr;
  }
  CHECK_FATAL(frm != nullptr, "frm is nullptr");
  auto *dom = static_cast<Dominance*>(frm->GetAnalysisResult(MeFuncPhase_DOMINANCE, func));
  CHECK_FATAL(dom != nullptr, "dominance phase has problem");
  auto *irMap = static_cast<MeIRMap*>(frm->GetAnalysisResult(MeFuncPhase_IRMAP, func));
  CHECK_FATAL(irMap != nullptr, "irMap phase has problem");
  MemPool *abcoMemPool = NewMemPool();
  MeABC meABC(*func, *dom, *irMap, *abcoMemPool);
  meABC.ExecuteABCO();
  if (DEBUGFUNC(func)) {
    LogInfo::MapleLogger() << "\n============== After boundary check optimization  =============" << std::endl;
    irMap->Dump();
  }
  return nullptr;
}
}
