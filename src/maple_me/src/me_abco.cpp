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
namespace maple {
bool MeABC::kDebug = false;
constexpr int kNumOpnds = 2;
constexpr int kPiStmtUpperBound = 2;
constexpr int kPiListSize = 2;

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

bool MeABC::IsLegal(MeStmt &meStmt) {
  CHECK_FATAL(meStmt.IsCondBr(), "must be");
  auto *brMeStmt = static_cast<CondGotoMeStmt*>(&meStmt);
  MeExpr *meCmp = brMeStmt->GetOpnd();
  // may be we need consider ne, eq, and only one opnd in branch insn
  if (meCmp->GetMeOp() != kMeOpOp) {
    return false;
  }
  auto *opMeExpr = static_cast<OpMeExpr*>(meCmp);
  CHECK_FATAL(opMeExpr->GetNumOpnds() == kNumOpnds, "must be");
  if (opMeExpr->GetOp() != OP_ge && opMeExpr->GetOp() != OP_le &&
      opMeExpr->GetOp() != OP_lt && opMeExpr->GetOp() != OP_gt &&
      opMeExpr->GetOp() != OP_ne && opMeExpr->GetOp() != OP_eq) {
    return false;
  }
  MeExpr *opnd1 = opMeExpr->GetOpnd(0);
  MeExpr *opnd2 = opMeExpr->GetOpnd(1);
  if ((opnd1->GetMeOp() != kMeOpVar && opnd1->GetMeOp() != kMeOpConst) ||
      (opnd2->GetMeOp() != kMeOpVar && opnd2->GetMeOp() != kMeOpConst)) {
    return false;
  }
  if (!IsPrimitivePureScalar(opnd1->GetPrimType()) || !IsPrimitivePureScalar(opnd2->GetPrimType())) {
    return false;
  }
  return true;
}

VarMeExpr *MeABC::CreateNewPiExpr(MeExpr &opnd) {
  if (opnd.GetMeOp() == kMeOpConst) {
    return nullptr;
  }
  CHECK_FATAL(opnd.GetMeOp() == kMeOpVar, "must be");
  SSATab &ssaTab = irMap->GetSSATab();
  OriginalSt *ost = ssaTab.GetOriginalStFromID(static_cast<VarMeExpr*>(&opnd)->GetOStIdx());
  CHECK_FATAL(ost != nullptr && !ost->IsVolatile(), "must be");
  VarMeExpr *var = irMap->NewInPool<VarMeExpr>(irMap->GetExprID(), ost->GetIndex(),
      irMap->GetVerst2MeExprTable().size());
  irMap->SetExprID(irMap->GetExprID() + 1);
  irMap->PushBackVerst2MeExprTable(var);
  ost->PushbackVersionIndex(var->GetVstIdx());
  var->InitBase(opnd.GetOp(), opnd.GetPrimType(), 0);
  return var;
}

void MeABC::CreateNewPiStmt(VarMeExpr *lhs, MeExpr &rhs, BB &bb, MeStmt &generatedBy, bool isToken) {
  if (lhs == nullptr) {
    return;
  }
  PiassignMeStmt *meStmt = GetMemPool()->New<PiassignMeStmt>(&GetAllocator());
  meStmt->SetGeneratedBy(generatedBy);
  meStmt->SetRHS(*(static_cast<VarMeExpr*>(&rhs)));
  meStmt->SetLHS(*lhs);
  meStmt->SetBB(&bb);
  meStmt->SetIsToken(isToken);
  lhs->SetDefBy(kDefByStmt);
  lhs->SetDefStmt(meStmt);
  bb.InsertPi(*(generatedBy.GetBB()), *meStmt);
  DefPoint *newDef = GetMemPool()->New<DefPoint>(DefPoint::DefineKind::kDefByPi);
  newDef->SetDefPi(*meStmt);
  newDefPoints.push_back(newDef);
  newDef2Old[newDef] = static_cast<VarMeExpr*>(&rhs);
}

void MeABC::CreateNewPiStmt(VarMeExpr *lhs, MeExpr &rhs, MeStmt &generatedBy) {
  if (lhs == nullptr) {
    return;
  }
  PiassignMeStmt *meStmt = GetMemPool()->New<PiassignMeStmt>(&GetAllocator());
  meStmt->SetGeneratedBy(generatedBy);
  meStmt->SetRHS(*(static_cast<VarMeExpr*>(&rhs)));
  meStmt->SetLHS(*lhs);
  meStmt->SetBB(generatedBy.GetBB());
  lhs->SetDefBy(kDefByStmt);
  lhs->SetDefStmt(meStmt);
  generatedBy.GetBB()->InsertMeStmtAfter(&generatedBy, meStmt);
  DefPoint *newDef = GetMemPool()->New<DefPoint>(DefPoint::DefineKind::kDefByPi);
  newDef->SetDefPi(*meStmt);
  newDefPoints.push_back(newDef);
  newDef2Old[newDef] = static_cast<VarMeExpr*>(&rhs);
}

void MeABC::InsertPiNodes() {
  for (auto bIt = meFunc->valid_begin(), eIt = meFunc->valid_end(); bIt != eIt; ++bIt) {
    BB *bb = *bIt;
    if (bb->GetKind() == kBBCondGoto && IsLegal(*(bb->GetLastMe()))) {
      auto *brMeStmt = static_cast<CondGotoMeStmt*>(bb->GetLastMe());
      BB *brTarget = bb->GetSucc(1);
      BB *brFallThru = bb->GetSucc(0);
      CHECK_FATAL(brMeStmt->GetOffset() == brTarget->GetBBLabel(), "must be");
      auto *opMeExpr = static_cast<OpMeExpr*>(brMeStmt->GetOpnd());
      MeExpr *opnd1 = opMeExpr->GetOpnd(0);
      MeExpr *opnd2 = opMeExpr->GetOpnd(1);
      VarMeExpr *brTargetNewOpnd1 = CreateNewPiExpr(*opnd1);
      VarMeExpr *brTargetNewOpnd2 = CreateNewPiExpr(*opnd2);
      CreateNewPiStmt(brTargetNewOpnd1, *opnd1, *brTarget, *brMeStmt, true);
      CreateNewPiStmt(brTargetNewOpnd2, *opnd2, *brTarget, *brMeStmt, true);
      VarMeExpr *brFallThruNewOpnd1 = CreateNewPiExpr(*opnd1);
      VarMeExpr *brFallThruNewOpnd2 = CreateNewPiExpr(*opnd2);
      CreateNewPiStmt(brFallThruNewOpnd1, *opnd1, *brFallThru, *brMeStmt, false);
      CreateNewPiStmt(brFallThruNewOpnd2, *opnd2, *brFallThru, *brMeStmt, false);
    }
    auto it = containsBB.find(bb);
    if (it == containsBB.end()) {
      continue;
    }
    std::vector<MeStmt*> &arryChk = it->second;
    for (MeStmt *meStmt : arryChk) {
      NaryMeExpr *nMeExpr = arrayChecks[meStmt];
      CHECK_FATAL(nMeExpr->GetOpnds().size() == kNumOpnds, "must be");
      MeExpr *opnd1 = nMeExpr->GetOpnd(0);
      MeExpr *opnd2 = nMeExpr->GetOpnd(1);
      // consider whether we should create pi if opnd2 is const
      CHECK_FATAL(opnd1->GetMeOp() == kMeOpVar, "must be");
      CHECK_FATAL(opnd1->GetPrimType() == PTY_ref, "must be");
      CHECK_FATAL(opnd2->GetMeOp() == kMeOpVar || opnd2->GetMeOp() == kMeOpConst, "must be");
      VarMeExpr *arracyCheckOpnd2 = CreateNewPiExpr(*opnd2);
      CreateNewPiStmt(arracyCheckOpnd2, *opnd2, *meStmt);
    }
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

bool MeABC::ExistedPhiNode(BB &bb, VarMeExpr &rhs) {
  return bb.GetMevarPhiList().find(rhs.GetOStIdx()) != bb.GetMevarPhiList().end();
}

bool MeABC::ExistedPiNode(BB &bb, BB &parentBB, VarMeExpr &rhs) {
  MapleMap<BB*, std::vector<PiassignMeStmt*>> &piList = bb.GetPiList();
  auto it = piList.find(&parentBB);
  if (it == piList.end()) {
    return false;
  }
  std::vector<PiassignMeStmt*> &piStmts = it->second;
  CHECK_FATAL(piStmts.size() >= 1 && piStmts.size() <= kPiStmtUpperBound, "must be");
  PiassignMeStmt *pi1 = piStmts.at(0);
  if (pi1->GetLHS()->GetOStIdx() == rhs.GetOStIdx()) {
    return true;
  }
  if (piStmts.size() == kPiStmtUpperBound) {
    PiassignMeStmt *pi2 = piStmts.at(1);
    if (pi2->GetLHS()->GetOStIdx() == rhs.GetOStIdx()) {
      return true;
    }
  }
  return false;
}

void MeABC::CreatePhi(VarMeExpr &rhs, BB &dfBB) {
  VarMeExpr *phiNewLHS = CreateNewPiExpr(rhs);
  MeVarPhiNode *newPhi = GetMemPool()->New<MeVarPhiNode>(phiNewLHS, &GetAllocator());
  newPhi->SetDefBB(&dfBB);
  newPhi->GetOpnds().resize(dfBB.GetPred().size(), &rhs);
  newPhi->SetPiAdded();
  dfBB.GetMevarPhiList().insert(std::make_pair(phiNewLHS->GetOStIdx(), newPhi));
  DefPoint *newDef = GetMemPool()->New<DefPoint>(DefPoint::DefineKind::kDefByPhi);
  newDef->SetDefPhi(*newPhi);
  newDefPoints.push_back(newDef);
  newDef2Old[newDef] = &rhs;
}

void MeABC::InsertPhiNodes() {
  for (size_t i = 0; i < newDefPoints.size(); ++i) {
    DefPoint *newDefStmt = newDefPoints[i];
    BB *newDefBB = newDefStmt->GetBB();
    VarMeExpr *rhs = newDefStmt->GetRHS();
    if (newDefStmt->IsPiStmt()) {
      BB *genByBB = newDefStmt->GetGeneratedByBB();
      if (!dom->Dominate(*genByBB, *newDefBB) && !ExistedPhiNode(*newDefBB, *rhs)) {
        CreatePhi(*rhs, *newDefBB);
        continue;
      }
    }
    BB *oldDefBB = rhs->DefByBB();
    if (oldDefBB == nullptr) {
      oldDefBB = meFunc->GetCommonEntryBB();
      CHECK_FATAL(rhs->IsZeroVersion(irMap->GetSSATab()), "must be");
    }
    CHECK_FATAL(newDefBB != nullptr && oldDefBB != nullptr, "must be");
    MapleSet<BBId> &dfs = dom->GetDomFrontier(newDefBB->GetBBId());
    for (auto bbID : dfs) {
      BB *dfBB = meFunc->GetBBFromID(bbID);
      if (!dom->Dominate(*oldDefBB, *dfBB)) {
        MapleSet<BBId> &dfsTmp = dom->GetDomFrontier(oldDefBB->GetBBId());
        CHECK_FATAL(dfsTmp.find(bbID) != dfsTmp.end(), "must be");
        continue;
      }
      if (ExistedPhiNode(*dfBB, *rhs)) {
        continue;
      }
      CreatePhi(*rhs, *dfBB);
    }
  }
}

void MeABC::RenameStartPiBr(DefPoint &newDefPoint) {
  const OStIdx &ostIdx = newDefPoint.GetOStIdx();
  BB *newDefBB = newDefPoint.GetBB();
  if (ExistedPhiNode(*(newDefPoint.GetBB()), *(newDefPoint.GetRHS()))) {
    MeVarPhiNode* phi = newDefBB->GetMevarPhiList()[ostIdx];
    BB *genByBB = newDefPoint.GetGeneratedByBB();
    size_t index = 0;
    while (index < newDefBB->GetPred().size()) {
      if (newDefBB->GetPred(index) == genByBB) {
        break;
      }
      ++index;
    }
    CHECK_FATAL(index < newDefBB->GetPred().size(), "must be");
    VarMeExpr *oldVar = phi->GetOpnd(index);
    phi->SetOpnd(index, newDefPoint.GetLHS());
    if (!phi->IsPiAdded()) {
      if (modifiedPhi.find(phi) == modifiedPhi.end()) {
        modifiedPhi[phi] = std::vector<VarMeExpr*>(phi->GetOpnds().size(), nullptr);
      }
      if (modifiedPhi[phi][index] == nullptr) {
        modifiedPhi[phi][index] = oldVar;
      }
    }
    return;
  }
  RenameStartPhi(newDefPoint);
}

void MeABC::RenameStartPiArray(DefPoint &newDefPoint) {
  BB *newDefBB = newDefPoint.GetBB();
  MeStmt *piStmt = newDefPoint.GetPiStmt();
  if (piStmt != newDefBB->GetLastMe()) {
    for (MeStmt *meStmt = piStmt->GetNext(); meStmt != nullptr; meStmt = meStmt->GetNext()) {
      if (ReplaceStmt(*meStmt, *(newDefPoint.GetLHS()), *(newDef2Old[&newDefPoint]))) {
        return;
      }
    }
  }
  ReplacePiPhiInSuccs(*newDefBB, *(newDefPoint.GetLHS()));
  const MapleSet<BBId> &children = dom->GetDomChildren(newDefBB->GetBBId());
  for (const BBId &child : children) {
    ReplaceBB(*(meFunc->GetBBFromID(child)), *newDefBB, newDefPoint);
  }
}

void MeABC::RenameStartPhi(DefPoint &newDefPoint) {
  BB *newDefBB = newDefPoint.GetBB();
  for (MeStmt &meStmt : newDefBB->GetMeStmts()) {
    if (ReplaceStmt(meStmt, *newDefPoint.GetLHS(), *newDef2Old[&newDefPoint])) {
      return;
    }
  }
  ReplacePiPhiInSuccs(*newDefBB, *(newDefPoint.GetLHS()));
  const MapleSet<BBId> &children = dom->GetDomChildren(newDefBB->GetBBId());
  for (const BBId &child : children) {
    ReplaceBB(*(meFunc->GetBBFromID(child)), *newDefBB, newDefPoint);
  }
}

void MeABC::ReplacePiPhiInSuccs(BB &bb, VarMeExpr &newVar) {
  for (BB *succBB : bb.GetSucc()) {
    MapleMap<BB*, std::vector<PiassignMeStmt*>> &piList = succBB->GetPiList();
    auto it1 = piList.find(&bb);
    if (it1 != piList.end()) {
      std::vector<PiassignMeStmt*> &piStmts = it1->second;
      // the size of pi statements must be 1 or 2
      CHECK_FATAL(piStmts.size() >= 1 && piStmts.size() <= 2, "must be");
      PiassignMeStmt *pi1 = piStmts.at(0);
      if (pi1->GetLHS()->GetOStIdx() == newVar.GetOStIdx()) {
        pi1->SetRHS(newVar);
        continue;
      }
      if (piStmts.size() == kPiStmtUpperBound) {
        PiassignMeStmt *pi2 = piStmts.at(1);
        if (pi2->GetLHS()->GetOStIdx() == newVar.GetOStIdx()) {
          pi2->SetRHS(newVar);
          continue;
        }
      }
    }
    size_t index = 0;
    while (index < succBB->GetPred().size()) {
      if (succBB->GetPred(index) == &bb) {
        break;
      }
      ++index;
    }
    CHECK_FATAL(index < succBB->GetPred().size(), "must be");
    MapleMap<OStIdx, MeVarPhiNode*> &phiList = succBB->GetMevarPhiList();
    auto it2 = phiList.find(newVar.GetOStIdx());
    if (it2 != phiList.end()) {
      MeVarPhiNode *phi = it2->second;
      VarMeExpr *oldVar = phi->GetOpnd(index);
      phi->SetOpnd(index, &newVar);
      if (!phi->IsPiAdded()) {
        if (modifiedPhi.find(phi) == modifiedPhi.end()) {
          modifiedPhi[phi] = std::vector<VarMeExpr*>(phi->GetOpnds().size(), nullptr);
        }
        if (modifiedPhi[phi][index] == nullptr) {
          modifiedPhi[phi][index] = oldVar;
        }
      }
    }
  }
}

MeExpr *MeABC::NewMeExpr(MeExpr &meExpr) {
  switch (meExpr.GetMeOp()) {
    case kMeOpIvar: {
      auto &ivarMeExpr = static_cast<IvarMeExpr&>(meExpr);
      IvarMeExpr *newIvarExpr = GetMemPool()->New<IvarMeExpr>(irMap->GetExprID(), ivarMeExpr);
      irMap->SetExprID(irMap->GetExprID() + 1);
      return newIvarExpr;
    }
    case kMeOpOp: {
      auto &opMeExpr = static_cast<OpMeExpr&>(meExpr);
      OpMeExpr *newOpMeExpr = GetMemPool()->New<OpMeExpr>(opMeExpr, irMap->GetExprID());
      irMap->SetExprID(irMap->GetExprID() + 1);
      return newOpMeExpr;
    }
    case kMeOpNary: {
      auto &naryMeExpr = static_cast<NaryMeExpr&>(meExpr);
      NaryMeExpr *newNaryMeExpr = GetMemPool()->New<NaryMeExpr>(&GetAllocator(), irMap->GetExprID(), naryMeExpr);
      irMap->SetExprID(irMap->GetExprID() + 1);
      newNaryMeExpr->InitBase(meExpr.GetOp(), meExpr.GetPrimType(), meExpr.GetNumOpnds());
      return newNaryMeExpr;
    }
    default:
      CHECK_FATAL(false, "impossible");
  }
}

MeExpr *MeABC::ReplaceMeExprExpr(MeExpr &origExpr, MeExpr &meExpr, MeExpr &repExpr) {
  if (origExpr.IsLeaf()) {
    return &origExpr;
  }
  switch (origExpr.GetMeOp()) {
    case kMeOpOp: {
      auto &opMeExpr = static_cast<OpMeExpr&>(origExpr);
      OpMeExpr newMeExpr(opMeExpr, kInvalidExprID);
      bool needRehash = false;
      for (uint32 i = 0; i < kOperandNumTernary; ++i) {
        if (opMeExpr.GetOpnd(i) == nullptr) {
          continue;
        }
        if (opMeExpr.GetOpnd(i) == &meExpr) {
          needRehash = true;
          newMeExpr.SetOpnd(i, &repExpr);
        } else if (!opMeExpr.GetOpnd(i)->IsLeaf()) {
          newMeExpr.SetOpnd(i, ReplaceMeExprExpr(*newMeExpr.GetOpnd(i), meExpr, repExpr));
          if (newMeExpr.GetOpnd(i) != opMeExpr.GetOpnd(i)) {
            needRehash = true;
          }
        }
      }
      return needRehash ? NewMeExpr(newMeExpr) : &origExpr;
    }
    case kMeOpNary: {
      auto &naryMeExpr = static_cast<NaryMeExpr&>(origExpr);
      NaryMeExpr newMeExpr(&GetAllocator(), kInvalidExprID, naryMeExpr);
      const MapleVector<MeExpr*> &opnds = naryMeExpr.GetOpnds();
      bool needRehash = false;
      for (size_t i = 0; i < opnds.size(); ++i) {
        MeExpr *opnd = opnds[i];
        if (opnd == &meExpr) {
          newMeExpr.SetOpnd(i, &repExpr);
          needRehash = true;
        } else if (!opnd->IsLeaf()) {
          newMeExpr.SetOpnd(i, ReplaceMeExprExpr(*newMeExpr.GetOpnd(i), meExpr, repExpr));
          if (newMeExpr.GetOpnd(i) != opnd) {
            needRehash = true;
          }
        }
      }
      return needRehash ? NewMeExpr(newMeExpr) : &origExpr;
    }
    case kMeOpIvar: {
      auto &ivarExpr = static_cast<IvarMeExpr&>(origExpr);
      IvarMeExpr newMeExpr(kInvalidExprID, ivarExpr);
      bool needRehash = false;
      if (ivarExpr.GetBase() == &meExpr) {
        newMeExpr.SetBase(&repExpr);
        needRehash = true;
      } else if (!ivarExpr.GetBase()->IsLeaf()) {
        newMeExpr.SetBase(ReplaceMeExprExpr(*newMeExpr.GetBase(), meExpr, repExpr));
        if (newMeExpr.GetBase() != ivarExpr.GetBase()) {
          needRehash = true;
        }
      }
      return needRehash ? NewMeExpr(newMeExpr) : &origExpr;
    }
    default:
      CHECK_FATAL(false, "NYI");
  }
}

bool MeABC::ReplaceMeExprStmtOpnd(uint32 opndID, MeStmt &meStmt, MeExpr &oldVar, MeExpr &newVar, bool update) {
  MeExpr *opnd = meStmt.GetOpnd(opndID);
  bool isFromIassign = (meStmt.GetOp() == OP_iassign) && (opndID == 0);
  if (isFromIassign) {
    opnd = static_cast<IassignMeStmt*>(&meStmt)->GetLHSVal();
  }
  bool replaced = false;
  MeExpr *newExpr = nullptr;
  if (opnd == &oldVar) {
    if (isFromIassign) {
      static_cast<IassignMeStmt*>(&meStmt)->SetLHSVal(static_cast<IvarMeExpr*>(&newVar));
    } else {
      meStmt.SetOpnd(opndID, &newVar);
    }
    replaced = true;
  } else if (!opnd->IsLeaf()) {
    newExpr = ReplaceMeExprExpr(*opnd, oldVar, newVar);
    replaced = (newExpr != opnd);
    if (isFromIassign) {
      static_cast<IassignMeStmt*>(&meStmt)->SetLHSVal(static_cast<IvarMeExpr *>(newExpr));
    } else {
      meStmt.SetOpnd(opndID, newExpr);
    }
  }
  if (replaced && update) {
    if (modifiedStmt.find(std::make_pair(&meStmt, opndID)) == modifiedStmt.end()) {
      modifiedStmt[std::make_pair(&meStmt, opndID)] = opnd;
    }
  }
  return replaced;
}

bool MeABC::ReplaceStmtWithNewVar(MeStmt &meStmt, MeExpr &oldVar, MeExpr &newVar, bool update) {
  bool isReplaced = false;
  switch (meStmt.GetOp()) {
    case OP_dassign:
    case OP_maydassign:
    case OP_brtrue:
    case OP_brfalse: {
      isReplaced = ReplaceMeExprStmtOpnd(0, meStmt, oldVar, newVar, update);
      break;
    }
    case OP_iassign: {
      bool baseIsReplaced = ReplaceMeExprStmtOpnd(0, meStmt, oldVar, newVar, update);
      bool rhsReplaced = ReplaceMeExprStmtOpnd(1, meStmt, oldVar, newVar, update);
      isReplaced = baseIsReplaced || rhsReplaced;
      break;
    }
    case OP_regassign: {
      CHECK_FATAL(false, "should not happen");
    }
    default: {
      break;
    }
  }
  return isReplaced;
}

bool MeABC::ReplaceStmt(MeStmt &meStmt, VarMeExpr &newVar, VarMeExpr &oldVar) {
  if (meStmt.GetOp() == OP_piassign) {
    auto *pi = static_cast<PiassignMeStmt*>(&meStmt);
    if (pi->GetRHS() == &oldVar) {
      pi->SetRHS(newVar);
    }
  } else {
    (void)ReplaceStmtWithNewVar(meStmt, oldVar, newVar, true);
  }
  const OStIdx &ostIdx = newVar.GetOStIdx();
  MapleMap<OStIdx, ChiMeNode*> *chiList = meStmt.GetChiList();
  if (chiList != nullptr && chiList->find(ostIdx) != chiList->end()) {
    return true;
  }
  MeExpr *lhs = meStmt.GetAssignedLHS();
  if (lhs != nullptr && lhs->GetMeOp() == kMeOpVar && static_cast<VarMeExpr*>(lhs)->GetOStIdx() == ostIdx) {
    return true;
  }
  lhs = meStmt.GetLHS();
  if (lhs != nullptr && lhs->GetMeOp() == kMeOpVar && static_cast<VarMeExpr*>(lhs)->GetOStIdx() == ostIdx) {
    return true;
  }
  return false;
}

void MeABC::Rename() {
  for (size_t i = 0; i < newDefPoints.size(); ++i) {
    DefPoint *newDefStmt = newDefPoints[i];
    visitedBBs.clear();
    if (newDefStmt->IsPiStmt()) {
      if (newDefStmt->IsGeneratedByBr()) {
        RenameStartPiBr(*newDefStmt);
      } else {
        RenameStartPiArray(*newDefStmt);
      }
    } else {
      RenameStartPhi(*newDefStmt);
    }
  }
}

void MeABC::ReplaceBB(BB &bb, BB &parentBB, DefPoint &newDefPoint) {
  if (visitedBBs.find(&bb) != visitedBBs.end()) {
    return;
  }
  visitedBBs.insert(&bb);
  if (ExistedPhiNode(bb, *(newDefPoint.GetLHS())) || ExistedPiNode(bb, parentBB, *(newDefPoint.GetLHS()))) {
    return;
  }
  for (MeStmt &meStmt : bb.GetMeStmts()) {
    if (ReplaceStmt(meStmt, *(newDefPoint.GetLHS()), *(newDef2Old[&newDefPoint]))) {
      return;
    }
  }
  ReplacePiPhiInSuccs(bb, *(newDefPoint.GetLHS()));
  const MapleSet<BBId> &children = dom->GetDomChildren(bb.GetBBId());
  for (const BBId &child : children) {
    ReplaceBB(*(meFunc->GetBBFromID(child)), bb, newDefPoint);
  }
}

void MeABC::RemoveExtraNodes() {
  for (DefPoint *defP : newDefPoints) {
      defP->RemoveFromBB();
  }
  for (auto pair : modifiedStmt) {
    MeStmt *meStmt = pair.first.first;
    MeExpr *newVar = nullptr;
    if ((meStmt->GetOp() == OP_iassign) && (pair.first.second == 0)) {
      newVar = static_cast<IassignMeStmt*>(meStmt)->GetLHSVal();
    } else {
      newVar = meStmt->GetOpnd(pair.first.second);
    }
    MeExpr *oldVar = pair.second;
    bool replaced = ReplaceStmtWithNewVar(*meStmt, *newVar, *oldVar, false);
    CHECK_FATAL(replaced, "must be");
  }
  for (auto pair : modifiedPhi) {
    MeVarPhiNode *phi = pair.first;
    for (size_t i = 0; i < pair.second.size(); ++i) {
      size_t index = i;
      VarMeExpr *oldVar = pair.second[i];
      if (oldVar != nullptr) {
        phi->SetOpnd(index, oldVar);
      }
    }
  }
  memPoolCtrler.DeleteMemPool(memPool);
}

bool MeABC::IsVirtualVar(VarMeExpr &var, SSATab &ssaTab) const {
  const OriginalSt *ost = ssaTab.GetOriginalStFromID(var.GetOStIdx());
  return ost->GetIndirectLev() > 0;
}

ESSABaseNode *MeABC::GetOrCreateRHSNode(MeExpr &expr) {
  auto &rhs = static_cast<VarMeExpr&>(expr);
  ESSABaseNode *result = nullptr;
  if (rhs.GetDefBy() != kDefByPhi) {
    result = inequalityGraph->GetOrCreateVarNode(rhs);
  } else {
    MeVarPhiNode *defPhi = &(rhs.GetDefPhi());
    result = inequalityGraph->GetOrCreatePhiNode(*defPhi);
  }
  return result;
}

void MeABC::BuildPhiInGraph(MeVarPhiNode &phi) {
  if (!IsPrimitivePureScalar(phi.GetLHS()->GetPrimType())) {
    MeExpr *varExpr = phi.GetLHS();
    std::set<MeVarPhiNode*> visitedPhi;
    ConstMeExpr dummyExpr(kInvalidExprID, nullptr);
    varExpr = TryToResolveVar(*varExpr, visitedPhi, dummyExpr, false);
    if (varExpr != nullptr && varExpr != &dummyExpr) {
      ESSAArrayNode *arrayNode = inequalityGraph->GetOrCreateArrayNode(*(phi.GetLHS()));
      ESSAVarNode *varNode = inequalityGraph->GetOrCreateVarNode(*varExpr);
      InequalEdge *pairEdge1 = inequalityGraph->AddEdge(*arrayNode, *varNode, 0, EdgeType::kUpper);
      InequalEdge *pairEdge2 = inequalityGraph->AddEdge(*varNode, *arrayNode, 0, EdgeType::kUpper);
      pairEdge1->SetPairEdge(*pairEdge2);
      pairEdge2->SetPairEdge(*pairEdge1);
    }
    return;
  }
  if (IsVirtualVar(*(phi.GetLHS()), irMap->GetSSATab())) {
    return;
  }
  for (VarMeExpr *phiRHS : phi.GetOpnds()) {
    AddUseDef(*phiRHS);
  }
}

void MeABC::BuildSoloPiInGraph(PiassignMeStmt &piMeStmt) {
  VarMeExpr *piLHS = piMeStmt.GetLHS();
  VarMeExpr *piRHS = piMeStmt.GetRHS();
  AddUseDef(*piRHS);
  ESSAVarNode *piLHSNode = inequalityGraph->GetOrCreateVarNode(*piLHS);
  ESSABaseNode *piRHSNode = GetOrCreateRHSNode(*piRHS);
  (void)inequalityGraph->AddEdge(*piRHSNode, *piLHSNode, 0, EdgeType::kUpper);
  (void)inequalityGraph->AddEdge(*piLHSNode, *piRHSNode, 0, EdgeType::kLower);
}

bool MeABC::BuildArrayCheckInGraph(MeStmt &meStmt) {
  CHECK_FATAL(meStmt.GetOp() == OP_piassign, "must be");
  auto *piMeStmt = static_cast<PiassignMeStmt*>(&meStmt);
  BuildSoloPiInGraph(*piMeStmt);
  if (piMeStmt == forbidenPi) {
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
      InequalEdge *pairEdge1 = inequalityGraph->AddEdge(*brFallThruOpnd1, *brFallThruOpnd2, 0, EdgeType::kUpper);
      InequalEdge *pairEdge2 = inequalityGraph->AddEdge(*brFallThruOpnd2, *brFallThruOpnd1, 0, EdgeType::kUpper);
      pairEdge1->SetPairEdge(*pairEdge2);
      pairEdge2->SetPairEdge(*pairEdge1);
      pairEdge1 = inequalityGraph->AddEdge(*brFallThruOpnd2, *brFallThruOpnd1, 0, EdgeType::kLower);
      pairEdge2 = inequalityGraph->AddEdge(*brFallThruOpnd1, *brFallThruOpnd2, 0, EdgeType::kLower);
      pairEdge1->SetPairEdge(*pairEdge2);
      pairEdge2->SetPairEdge(*pairEdge1);
      break;
    }
    case OP_eq: {
      InequalEdge *pairEdge1 = inequalityGraph->AddEdge(*brTargetOpnd1, *brTargetOpnd2, 0, EdgeType::kUpper);
      InequalEdge *pairEdge2 = inequalityGraph->AddEdge(*brTargetOpnd2, *brTargetOpnd1, 0, EdgeType::kUpper);
      pairEdge1->SetPairEdge(*pairEdge2);
      pairEdge2->SetPairEdge(*pairEdge1);
      pairEdge1 = inequalityGraph->AddEdge(*brTargetOpnd2, *brTargetOpnd1, 0, EdgeType::kLower);
      pairEdge2 = inequalityGraph->AddEdge(*brTargetOpnd1, *brTargetOpnd2, 0, EdgeType::kLower);
      pairEdge1->SetPairEdge(*pairEdge2);
      pairEdge2->SetPairEdge(*pairEdge1);
      break;
    }
    default:
      CHECK_FATAL(false, "impossible");
  }
  return true;
}

MeExpr *MeABC::TryToResolveVar(MeExpr &expr, std::set<MeVarPhiNode*> &visitedPhi, MeExpr &dummyExpr, bool isConst) {
  CHECK_FATAL(expr.GetMeOp() == kMeOpVar, "must be");
  auto *var = static_cast<VarMeExpr*>(&expr);
  if (var->GetDefBy() == kDefByStmt && isConst && !var->GetDefStmt()->GetRHS()->IsLeaf()) {
    return nullptr;
  }
  if (var->GetDefBy() == kDefByStmt && isConst && var->GetDefStmt()->GetRHS()->GetMeOp() == kMeOpConst) {
    return var->GetDefStmt()->GetRHS();
  }
  if (var->GetDefBy() == kDefByStmt && !isConst && var->GetDefStmt()->GetRHS()->GetMeOp() == kMeOpIvar) {
    return var->GetDefStmt()->GetRHS();
  }
  if (var->GetDefBy() == kDefByStmt && isConst) {
    CHECK_FATAL(var->GetDefStmt()->GetRHS()->GetMeOp() == kMeOpVar, "must be");
    return TryToResolveVar(*(var->GetDefStmt()->GetRHS()), visitedPhi, dummyExpr, isConst);
  }

  if (var->GetDefBy() == kDefByPhi) {
    MeVarPhiNode *phi = &(var->GetDefPhi());
    if (visitedPhi.find(phi) != visitedPhi.end()) {
      return &dummyExpr;
    }
    visitedPhi.insert(phi);
    std::set<MeExpr*> res;
    for (VarMeExpr *phiOpnd : phi->GetOpnds()) {
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
    } else if (res.size() == 0) {
      return &dummyExpr;
    }
  }

  return nullptr;
}

bool MeABC::BuildAssignInGraph(MeStmt &meStmt) {
  MeExpr *lhs = meStmt.GetLHS();
  MeExpr *rhs = meStmt.GetRHS();
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
          std::set<MeVarPhiNode*> visitedPhi;
          ConstMeExpr dummyExpr(kInvalidExprID, nullptr);
          varExpr = TryToResolveVar(*varExpr, visitedPhi, dummyExpr, true);
          if (varExpr != nullptr && varExpr != &dummyExpr) {
            CHECK_FATAL(varExpr->GetMeOp() == kMeOpConst, "must be");
            ESSAConstNode *constNode = inequalityGraph->GetOrCreateConstNode(
                static_cast<ConstMeExpr*>(varExpr)->GetIntValue());
            (void)inequalityGraph->AddEdge(*arrLength, *constNode, 0, EdgeType::kNone);
          }
        } else {
          CHECK_FATAL(opMeExpr->GetOpnd(0)->GetMeOp() == kMeOpConst, "must be");
          rhsNode = inequalityGraph->GetOrCreateConstNode(
              static_cast<ConstMeExpr*>(opMeExpr->GetOpnd(0))->GetIntValue());
        }
        (void)inequalityGraph->AddEdge(*arrLength, *rhsNode, 0, EdgeType::kNone);
        return true;
      }
      case OP_sub: {
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
            ESSABaseNode *rhsNode = GetOrCreateRHSNode(*opnd1);
            AddUseDef(*opnd1);
            InequalEdge *pairEdge1 = inequalityGraph->AddEdge(*rhsNode, *lhsNode,
                -static_cast<ConstMeExpr*>(opnd2)->GetIntValue(), EdgeType::kUpper);
            InequalEdge *pairEdge2 = inequalityGraph->AddEdge(*lhsNode, *rhsNode,
                static_cast<ConstMeExpr*>(opnd2)->GetIntValue(), EdgeType::kUpper);
            pairEdge1->SetPairEdge(*pairEdge2);
            pairEdge2->SetPairEdge(*pairEdge1);
            pairEdge1 = inequalityGraph->AddEdge(*lhsNode, *rhsNode,
                static_cast<ConstMeExpr*>(opnd2)->GetIntValue(), EdgeType::kLower);
            pairEdge2 = inequalityGraph->AddEdge(*rhsNode, *lhsNode,
                -static_cast<ConstMeExpr*>(opnd2)->GetIntValue(), EdgeType::kLower);
            pairEdge1->SetPairEdge(*pairEdge2);
            pairEdge2->SetPairEdge(*pairEdge1);
            return true;
          }else {
            // support this pattern later
            // CHECK_FATAL(false, "consider this pattern");
            return false;
          }
        } else if (opnd1->GetMeOp() == kMeOpVar && opnd2->GetMeOp() == kMeOpVar) {
          // Try to resolve Var is assigned from Const
          std::set<MeVarPhiNode*> visitedPhi;
          ConstMeExpr dummyExpr(kInvalidExprID, nullptr);
          opnd2 = TryToResolveVar(*opnd2, visitedPhi, dummyExpr, true);
          if (opnd2 != nullptr && opnd2 != &dummyExpr) {
            CHECK_FATAL(opnd2->GetMeOp() == kMeOpConst, "must be");
            ESSABaseNode *rhsNode = GetOrCreateRHSNode(*opnd1);
            AddUseDef(*opnd1);
            InequalEdge *pairEdge1 = inequalityGraph->AddEdge(*rhsNode, *lhsNode,
                -static_cast<ConstMeExpr*>(opnd2)->GetIntValue(), EdgeType::kUpper);
            InequalEdge *pairEdge2 = inequalityGraph->AddEdge(*lhsNode, *rhsNode,
                static_cast<ConstMeExpr*>(opnd2)->GetIntValue(), EdgeType::kUpper);
            pairEdge1->SetPairEdge(*pairEdge2);
            pairEdge2->SetPairEdge(*pairEdge1);
            pairEdge1 = inequalityGraph->AddEdge(*lhsNode, *rhsNode,
                static_cast<ConstMeExpr*>(opnd2)->GetIntValue(), EdgeType::kLower);
            pairEdge2 = inequalityGraph->AddEdge(*rhsNode, *lhsNode,
                -static_cast<ConstMeExpr*>(opnd2)->GetIntValue(), EdgeType::kLower);
            pairEdge1->SetPairEdge(*pairEdge2);
            pairEdge2->SetPairEdge(*pairEdge1);
            return true;
          } else {
            return false;
          }
        } else {
          // support this pattern later
          return false;
        }
        CHECK_FATAL(false, "impossible");
      }
      case OP_add: {
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
            ESSABaseNode *rhsNode = GetOrCreateRHSNode(*opnd1);
            AddUseDef(*opnd1);
            InequalEdge *pairEdge1 = inequalityGraph->AddEdge(*rhsNode, *lhsNode,
                static_cast<ConstMeExpr*>(opnd2)->GetIntValue(), EdgeType::kUpper);
            InequalEdge *pairEdge2 = inequalityGraph->AddEdge(*lhsNode, *rhsNode,
                -static_cast<ConstMeExpr*>(opnd2)->GetIntValue(), EdgeType::kUpper);
            pairEdge1->SetPairEdge(*pairEdge2);
            pairEdge2->SetPairEdge(*pairEdge1);
            pairEdge1 = inequalityGraph->AddEdge(*lhsNode, *rhsNode,
                -static_cast<ConstMeExpr*>(opnd2)->GetIntValue(), EdgeType::kLower);
            pairEdge2 = inequalityGraph->AddEdge(*rhsNode, *lhsNode,
                static_cast<ConstMeExpr*>(opnd2)->GetIntValue(), EdgeType::kLower);
            pairEdge1->SetPairEdge(*pairEdge2);
            pairEdge2->SetPairEdge(*pairEdge1);
            return true;
          } else {
            ESSABaseNode *rhsNode = GetOrCreateRHSNode(*opnd2);
            AddUseDef(*opnd2);
            InequalEdge *pairEdge1 = inequalityGraph->AddEdge(*rhsNode, *lhsNode,
                static_cast<ConstMeExpr*>(opnd1)->GetIntValue(), EdgeType::kUpper);
            InequalEdge *pairEdge2 = inequalityGraph->AddEdge(*lhsNode, *rhsNode,
                -static_cast<ConstMeExpr*>(opnd1)->GetIntValue(), EdgeType::kUpper);
            pairEdge1->SetPairEdge(*pairEdge2);
            pairEdge2->SetPairEdge(*pairEdge1);
            pairEdge1 = inequalityGraph->AddEdge(*lhsNode, *rhsNode,
                -static_cast<ConstMeExpr*>(opnd1)->GetIntValue(), EdgeType::kLower);
            pairEdge2 = inequalityGraph->AddEdge(*rhsNode, *lhsNode,
                static_cast<ConstMeExpr*>(opnd1)->GetIntValue(), EdgeType::kLower);
            pairEdge1->SetPairEdge(*pairEdge2);
            pairEdge2->SetPairEdge(*pairEdge1);
            return true;
          }
        } else {
          // support this pattern later
          return false;
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
    if ((rhs->GetMeOp() == kMeOpVar || rhs->GetMeOp() == kMeOpIvar) && (lhs->GetPrimType() == PTY_ref)) {
      ESSAVarNode *ivarNode = inequalityGraph->GetOrCreateVarNode(*rhs);
      ESSAArrayNode *arrayNode = inequalityGraph->GetOrCreateArrayNode(*lhs);
      InequalEdge *pairEdge1 = inequalityGraph->AddEdge(*ivarNode, *arrayNode, 0, EdgeType::kUpper);
      InequalEdge *pairEdge2 = inequalityGraph->AddEdge(*arrayNode, *ivarNode, 0, EdgeType::kUpper);
      pairEdge1->SetPairEdge(*pairEdge2);
      pairEdge2->SetPairEdge(*pairEdge1);
      return true;
    } else {
      return false;
    }
  } else {
    CHECK_FATAL(rhs->GetMeOp() == kMeOpVar || rhs->GetMeOp() == kMeOpConst, "must be");
    ESSAVarNode *lhsNode = inequalityGraph->GetOrCreateVarNode(*lhs);
    if (rhs->GetMeOp() == kMeOpVar) {
      AddUseDef(*rhs);
      ESSABaseNode *rhsNode = GetOrCreateRHSNode(*rhs);
      InequalEdge *pairEdge1 = inequalityGraph->AddEdge(*rhsNode, *lhsNode, 0, EdgeType::kUpper);
      InequalEdge *pairEdge2 = inequalityGraph->AddEdge(*lhsNode, *rhsNode, 0, EdgeType::kUpper);
      pairEdge1->SetPairEdge(*pairEdge2);
      pairEdge2->SetPairEdge(*pairEdge1);
      pairEdge1 = inequalityGraph->AddEdge(*lhsNode, *rhsNode, 0, EdgeType::kLower);
      pairEdge2 = inequalityGraph->AddEdge(*rhsNode, *lhsNode, 0, EdgeType::kLower);
      pairEdge1->SetPairEdge(*pairEdge2);
      pairEdge2->SetPairEdge(*pairEdge1);
    } else {
      ESSAConstNode *rhsNode = inequalityGraph->GetOrCreateConstNode(static_cast<ConstMeExpr*>(rhs)->GetIntValue());
      InequalEdge *pairEdge1 = inequalityGraph->AddEdge(*rhsNode, *lhsNode, 0, EdgeType::kUpper);
      InequalEdge *pairEdge2 = inequalityGraph->AddEdge(*lhsNode, *rhsNode, 0, EdgeType::kUpper);
      pairEdge1->SetPairEdge(*pairEdge2);
      pairEdge2->SetPairEdge(*pairEdge1);
      pairEdge1 = inequalityGraph->AddEdge(*lhsNode, *rhsNode, 0, EdgeType::kLower);
      pairEdge2 = inequalityGraph->AddEdge(*rhsNode, *lhsNode, 0, EdgeType::kLower);
      pairEdge1->SetPairEdge(*pairEdge2);
      pairEdge2->SetPairEdge(*pairEdge1);
    }
    return true;
  }
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
void MeABC::AddCarePhi(MeVarPhiNode &defP) {
  MeVarPhiNode *defPhi = &defP;
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
      MeVarPhiNode *defPhi = &varOpnd1->GetDefPhi();
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
    for (size_t iii = 0; iii < meStmt->NumMeStmtOpnds(); ++iii) {
      ABCCollectArrayExpr(*meStmt, *(meStmt->GetOpnd(iii)), true);
    }
  }
  CHECK_FATAL(arrayNewChecks.size() == arrayChecks.size(), "must be");
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
}

void MeABC::FindRedundantABC(MeStmt &meStmt, NaryMeExpr &naryMeExpr) {
  MeExpr *opnd1 = naryMeExpr.GetOpnd(0);
  MeExpr *opnd2 = naryMeExpr.GetOpnd(1);
  CHECK_FATAL(opnd1->GetMeOp() == kMeOpVar && opnd1->GetPrimType() == PTY_ref, "must be");
  if (!inequalityGraph->HasNode(*opnd1)) {
    if (MeABC::kDebug) {
      LogInfo::MapleLogger() << "Array Node Not Found" << '\n';
      meStmt.Dump(irMap);
    }
    return;
  }
  if (opnd2->GetMeOp() == kMeOpVar && !inequalityGraph->HasNode(*opnd2)) {
    if (MeABC::kDebug) {
      LogInfo::MapleLogger() << "Array Index Not Found" << '\n';
      meStmt.Dump(irMap);
    }
    return;
  }
  if (prove->DemandProve(*opnd1, *opnd2)) {
    if (MeABC::kDebug) {
      LogInfo::MapleLogger() << "Find One OPT" << '\n';
      meStmt.Dump(irMap);
    }
    targetMeStmt.insert(&meStmt);
  } else {
    if (MeABC::kDebug) {
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
  CHECK_FATAL(rhs.GetMeOp() == kMeOpIvar, "must be");
  MeExpr *newBase = ReplaceArrayExpr(*static_cast<IvarMeExpr&>(rhs).GetBase(), naryMeExpr, ivarStmt);
  CHECK_FATAL(newBase, "must be");
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
      CHECK_FATAL(!rhs->IsLeaf(), "must be");
      MeExpr *newRHS = ReplaceArrayExpr(*rhs, naryMeExpr, nullptr);
      replaced = rhs != newRHS;
      meStmt.SetOpnd(0, newRHS);
      break;
    }
    case OP_iassign: {
      CHECK_FATAL(meStmt.GetRHS()->IsLeaf(), "must be");
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
}

void MeABC::InitNewStartPoint(MeStmt &meStmt, NaryMeExpr &nMeExpr) {
  careMeStmts.clear();
  careMePhis.clear();
  carePoints.clear();
  inequalityGraph = std::make_unique<InequalityGraph>(*meFunc);
  CHECK_FATAL(inequalityGraph != nullptr, "inequalityGraph is nullptr");
  prove = std::make_unique<ABCD>(*inequalityGraph);
  CHECK_FATAL(prove != nullptr, "prove is nullptr");
  CHECK_FATAL(nMeExpr.GetNumOpnds() == kNumOpnds, "msut be");
  MeExpr *opnd1 = nMeExpr.GetOpnd(0);
  MeExpr *opnd2 = nMeExpr.GetOpnd(1);
  CHECK_FATAL(opnd1->GetMeOp() == kMeOpVar, "must be");
  AddUseDef(*opnd1);
  if (opnd2->GetMeOp() == kMeOpVar) {
    AddUseDef(*opnd2);
  } else {
    CHECK_FATAL(opnd2->GetMeOp() == kMeOpConst, "must be");
    (void)inequalityGraph->GetOrCreateConstNode(static_cast<ConstMeExpr*>(opnd2)->GetIntValue());
  }
  BB *curBB = meStmt.GetBB();
  if (curBB->GetPiList().size()) {
    for (auto pair : curBB->GetPiList()) {
      CHECK_FATAL(pair.second.size() >= 1, "must be");
      PiassignMeStmt *pi = pair.second[0];
      AddUseDef(*pi->GetLHS());
    }
  }
  forbidenPi = meStmt.GetNextMeStmt();
  CHECK_FATAL(forbidenPi != nullptr, "forbidenPi is nullptr");
  CHECK_FATAL(forbidenPi->GetOp() == OP_piassign, "must be");
}

void MeABC::executeABCO() {
  MeABC::kDebug = false;
  if (CollectABC()) {
    if (MeABC::kDebug) {
      LogInfo::MapleLogger() << meFunc->GetName() << "\n";
      irMap->Dump();
    }
    InsertPiNodes();
    InsertPhiNodes();
    Rename();
    CollectCareInsns();
    for (auto pair : arrayNewChecks) {
      InitNewStartPoint(*(pair.first), *(static_cast<NaryMeExpr*>(pair.second)));
      BuildInequalityGraph();
      if (MeABC::kDebug) {
        meFunc->GetTheCfg()->DumpToFile(meFunc->GetName());
        inequalityGraph->DumpDotFile(*irMap, DumpType::kDumpUpperAndNone);
        inequalityGraph->DumpDotFile(*irMap, DumpType::kDumpLowerAndNone);
      }
      FindRedundantABC(*(pair.first), *(static_cast<NaryMeExpr*>(pair.second)));
    }
    RemoveExtraNodes();
    DeleteABC();
    inequalityGraph = nullptr;
    prove = nullptr;
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
  MemPool *abcoMemPool = memPoolCtrler.NewMemPool(PhaseName());
  MeABC meABC(*func, *dom, *irMap, *abcoMemPool);
  meABC.executeABCO();
  if (DEBUGFUNC(func)) {
    LogInfo::MapleLogger() << "\n============== After boundary check optimization  =============" << std::endl;
    irMap->Dump();
  }
  return nullptr;
}
}
