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
#include "me_ssi.h"

namespace maple {
bool MeSSI::isDebug = false;
constexpr int kNumOpnds = 2;
constexpr int kPiStmtUpperBound = 2;

VarMeExpr *MeSSI::CreateNewPiExpr(const MeExpr &opnd) {
  if (opnd.GetMeOp() == kMeOpConst) {
    return nullptr;
  }
  CHECK_FATAL(opnd.GetMeOp() == kMeOpVar, "must be");
  SSATab &ssaTab = irMap->GetSSATab();
  OriginalSt *ost = ssaTab.GetOriginalStFromID(static_cast<const VarMeExpr*>(&opnd)->GetOStIdx());
  CHECK_NULL_FATAL(ost);
  CHECK_FATAL(!ost->IsVolatile(), "must be");
  VarMeExpr *var = irMap->NewInPool<VarMeExpr>(irMap->GetExprID(), ost->GetIndex(),
                                               irMap->GetVerst2MeExprTable().size());
  irMap->SetExprID(irMap->GetExprID() + 1);
  irMap->PushBackVerst2MeExprTable(var);
  ost->PushbackVersionIndex(var->GetVstIdx());
  var->InitBase(opnd.GetOp(), opnd.GetPrimType(), 0);
  return var;
}

void MeSSI::CreateNewPiStmt(VarMeExpr *lhs, MeExpr &rhs, BB &bb, MeStmt &generatedBy, bool isToken) {
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

void MeSSI::CreateNewPiStmt(VarMeExpr *lhs, MeExpr &rhs, MeStmt &generatedBy) {
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

void MeSSI::AddPiForABCOpt(BB &bb) {
  CHECK_FATAL(containsBB != nullptr && arrayChecks != nullptr, "must be");
  auto it = containsBB->find(&bb);
  if (it == containsBB->end()) {
    return;
  }
  std::vector<MeStmt*> &arryChk = it->second;
  for (MeStmt *meStmt : arryChk) {
    NaryMeExpr *naryMeExpr = (*arrayChecks)[meStmt];
    CHECK_FATAL(naryMeExpr->GetOpnds().size() == kNumOpnds, "must be");
    MeExpr *opnd1 = naryMeExpr->GetOpnd(0);
    MeExpr *opnd2 = naryMeExpr->GetOpnd(1);
    CHECK_FATAL(opnd1->GetMeOp() == kMeOpVar, "must be");
    CHECK_FATAL(opnd1->GetPrimType() == PTY_ref, "must be");
    CHECK_FATAL(opnd2->GetMeOp() == kMeOpVar || opnd2->GetMeOp() == kMeOpConst, "must be");
    VarMeExpr *arrayCheckOpnd2 = CreateNewPiExpr(*opnd2);
    CreateNewPiStmt(arrayCheckOpnd2, *opnd2, *meStmt);
  }
}

NaryMeExpr *MeSSI::GetInstanceOfType(MeExpr &e) {
  CHECK_FATAL(e.GetMeOp() == kMeOpVar, "must b");
  VarMeExpr *var = static_cast<VarMeExpr*>(&e);
  if (var->GetPrimType() != PTY_u1 || var->GetDefBy() != kDefByStmt) {
    return nullptr;
  }
  MeStmt *defStmt = var->GetDefStmt();
  if (defStmt == nullptr || defStmt->GetOp() != OP_dassign) {
    return nullptr;
  }
  MeExpr *expr = defStmt->GetRHS();
  if (expr == nullptr || expr->GetOp() != OP_intrinsicopwithtype) {
    return nullptr;
  }
  auto *callNode = safe_cast<NaryMeExpr>(expr);
  if (callNode == nullptr || callNode->GetIntrinsic() != INTRN_JAVA_INSTANCE_OF) {
    return nullptr;
  }
  if (callNode->GetOpnd(0)->GetMeOp() != kMeOpVar && callNode->GetOpnd(0)->GetMeOp() != kMeOpIvar) {
    return nullptr;
  }
  return callNode;
}

uint8_t MeSSI::AnalysisBranch(MeStmt &meStmt) {
  CHECK_FATAL(meStmt.IsCondBr(), "must be");
  auto *brMeStmt = static_cast<CondGotoMeStmt*>(&meStmt);
  MeExpr *meCmp = brMeStmt->GetOpnd();
  uint8_t result = 0;
  if (meCmp->GetMeOp() == kMeOpVar) {
    NaryMeExpr *instanceofType = GetInstanceOfType(*meCmp);
    if (instanceofType != nullptr) {
      result = result | kCheckCastOpt;
    }
  } else if (meCmp->GetMeOp() == kMeOpOp) {
    auto *opMeExpr = static_cast<OpMeExpr*>(meCmp);
    if (opMeExpr->GetNumOpnds() != kNumOpnds) {
      return result;
    }
    if (opMeExpr->GetOp() != OP_ge && opMeExpr->GetOp() != OP_le &&
        opMeExpr->GetOp() != OP_lt && opMeExpr->GetOp() != OP_gt &&
        opMeExpr->GetOp() != OP_ne && opMeExpr->GetOp() != OP_eq) {
      return result;
    }
    MeExpr *opnd1 = opMeExpr->GetOpnd(0);
    MeExpr *opnd2 = opMeExpr->GetOpnd(1);
    if ((opnd1->GetMeOp() != kMeOpVar && opnd1->GetMeOp() != kMeOpConst) ||
        (opnd2->GetMeOp() != kMeOpVar && opnd2->GetMeOp() != kMeOpConst)) {
      return result;
    }
    if (IsPrimitivePureScalar(opnd1->GetPrimType()) && IsPrimitivePureScalar(opnd2->GetPrimType())) {
      return result | kArrayBoundsCheckOpt;
    }
    if (opMeExpr->GetOp() == OP_ne || opMeExpr->GetOp() == OP_eq) {
      CHECK_FATAL(IsAddress(opnd1->GetPrimType()) == IsPrimitivePoint(opnd1->GetPrimType()), "MUST BE");
      CHECK_FATAL(IsAddress(opnd2->GetPrimType()) == 1, "MUST BE");
      return result | kNullPointerCheckOpt | kCondBaseRCOpt;
    }
  }
  return result;
}

void MeSSI::AddNullPointerInfoForVar() {
  CHECK_FATAL(false, "NYI");
}

void MeSSI::InsertPiNodes() {
  for (auto bIt = meFunc->valid_begin(), eIt = meFunc->valid_end(); bIt != eIt; ++bIt) {
    BB *bb = *bIt;
    if (bb->GetKind() == kBBCondGoto) {
      uint8_t careOpt = AnalysisBranch(*(bb->GetLastMe()));
      SSIType careBranch{};
      careBranch.SetOptType(careOpt & ssiType.GetOptType());
      auto *brMeStmt = static_cast<CondGotoMeStmt*>(bb->GetLastMe());
      BB *brTarget = bb->GetSucc(1);
      BB *brFallThru = bb->GetSucc(0);
      CHECK_FATAL(brMeStmt->GetOffset() == brTarget->GetBBLabel(), "must be");
      if (careBranch.GetOptKindType(kCheckCastOpt)) {
        NaryMeExpr *instanceofType = GetInstanceOfType(*brMeStmt->GetOpnd());
        MIRType *mirType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(instanceofType->GetTyIdx());
        MeExpr *opnd = instanceofType->GetOpnd(0);
        if (opnd->GetMeOp() == kMeOpIvar) {
          opnd = static_cast<IvarMeExpr*>(opnd)->GetMu();
        }
        VarMeExpr *brTargetNewOpnd = CreateNewPiExpr(*opnd);
        bool isToken = (brMeStmt->GetOp() == OP_brtrue);
        CreateNewPiStmt(brTargetNewOpnd, *opnd, *brTarget, *brMeStmt, true);
        VarMeExpr *brFallThruNewOpnd = CreateNewPiExpr(*opnd);
        CreateNewPiStmt(brFallThruNewOpnd, *opnd, *brFallThru, *brMeStmt, false);
        // Add Type Info For Var
        if (isToken) {
          inferredType[brTargetNewOpnd] = mirType;
        } else {
          inferredType[brFallThruNewOpnd] = mirType;
        }
      } else if (careBranch.GetOptType() != 0) {
        CHECK_FATAL(brMeStmt->GetOpnd()->GetMeOp() == kMeOpOp, "must be");
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
        if (careBranch.GetOptKindType(kNullPointerCheckOpt) || careBranch.GetOptKindType(kCondBaseRCOpt)) {
          AddNullPointerInfoForVar();
        }
      }
    }
    if (ssiType.GetOptKindType(kArrayBoundsCheckOpt)) {
      AddPiForABCOpt(*bb);
    }
  }
}

bool MeSSI::ExistedPhiNode(BB &bb, VarMeExpr &rhs) {
  return bb.GetMePhiList().find(rhs.GetOStIdx()) != bb.GetMePhiList().end();
}

bool MeSSI::ExistedPiNode(BB &bb, BB &parentBB, const VarMeExpr &rhs) {
  MapleMap<BB*, std::vector<PiassignMeStmt*>> &piList = bb.GetPiList();
  auto it = piList.find(&parentBB);
  if (it == piList.end()) {
    return false;
  }
  std::vector<PiassignMeStmt*> &piStmts = it->second;
  CHECK_FATAL(!piStmts.empty(), "should not be empty");
  CHECK_FATAL(piStmts.size() <= kPiStmtUpperBound, "must be");
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

void MeSSI::CreatePhi(VarMeExpr &rhs, BB &dfBB) {
  VarMeExpr *phiNewLHS = CreateNewPiExpr(rhs);
  ASSERT_NOT_NULL(phiNewLHS);
  MePhiNode *newPhi = GetMemPool()->New<MePhiNode>(phiNewLHS, &GetAllocator());
  newPhi->SetDefBB(&dfBB);
  newPhi->GetOpnds().resize(dfBB.GetPred().size(), &rhs);
  newPhi->SetPiAdded();
  dfBB.GetMePhiList().insert(std::make_pair(phiNewLHS->GetOStIdx(), newPhi));
  DefPoint *newDef = GetMemPool()->New<DefPoint>(DefPoint::DefineKind::kDefByPhi);
  newDef->SetDefPhi(*newPhi);
  newDefPoints.push_back(newDef);
  newDef2Old[newDef] = &rhs;
}

void MeSSI::InsertPhiNodes() {
  for (size_t i = 0; i < newDefPoints.size(); ++i) {
    DefPoint *newDefStmt = newDefPoints[i];
    BB *newDefBB = newDefStmt->GetBB();
    CHECK_NULL_FATAL(newDefBB);
    VarMeExpr *rhs = newDefStmt->GetRHS();
    if (newDefStmt->IsPiStmt()) {
      BB *genByBB = newDefStmt->GetGeneratedByBB();
      if (!dom->Dominate(*genByBB, *newDefBB)) {
        if (!ExistedPhiNode(*newDefBB, *rhs)) {
          CreatePhi(*rhs, *newDefBB);
        }
        continue;
      }
    }
    BB *oldDefBB = rhs->DefByBB();
    if (oldDefBB == nullptr) {
      oldDefBB = meFunc->GetCommonEntryBB();
      CHECK_FATAL(rhs->IsZeroVersion(irMap->GetSSATab()), "must be");
    }
    CHECK_NULL_FATAL(oldDefBB);
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

void MeSSI::RenameStartPiBr(DefPoint &newDefPoint) {
  const OStIdx &ostIdx = newDefPoint.GetOStIdx();
  BB *newDefBB = newDefPoint.GetBB();
  if (!ExistedPhiNode(*(newDefPoint.GetBB()), *(newDefPoint.GetRHS()))) {
    RenameStartPhi(newDefPoint);
    return;
  }
  MePhiNode* phi = newDefBB->GetMePhiList()[ostIdx];
  BB *genByBB = newDefPoint.GetGeneratedByBB();
  size_t index = 0;
  while (index < newDefBB->GetPred().size()) {
    if (newDefBB->GetPred(index) == genByBB) {
      break;
    }
    ++index;
  }
  CHECK_FATAL(index < newDefBB->GetPred().size(), "must be");
  ScalarMeExpr*oldVar = phi->GetOpnd(index);
  phi->SetOpnd(index, newDefPoint.GetLHS());
  if (!phi->IsPiAdded()) {
    if (modifiedPhi.find(phi) == modifiedPhi.end()) {
      modifiedPhi[phi] = std::vector<ScalarMeExpr*>(phi->GetOpnds().size(), nullptr);
    }
    if (modifiedPhi[phi][index] == nullptr) {
      modifiedPhi[phi][index] = oldVar;
    }
  }
}

void MeSSI::RenameStartPiArray(DefPoint &newDefPoint) {
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

void MeSSI::RenameStartPhi(DefPoint &newDefPoint) {
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

void MeSSI::ReplacePiPhiInSuccs(BB &bb, VarMeExpr &newVar) {
  for (BB *succBB : bb.GetSucc()) {
    MapleMap<BB*, std::vector<PiassignMeStmt*>> &piList = succBB->GetPiList();
    auto it1 = piList.find(&bb);
    if (it1 != piList.end()) {
      std::vector<PiassignMeStmt*> &piStmts = it1->second;
      // the size of pi statements must be 1 or 2
      CHECK_FATAL(!piStmts.empty(), "should not be empty");
      CHECK_FATAL(piStmts.size() <= kPiStmtUpperBound, "must be");
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
    MapleMap<OStIdx, MePhiNode*> &phiList = succBB->GetMePhiList();
    auto it2 = phiList.find(newVar.GetOStIdx());
    if (it2 != phiList.end()) {
      MePhiNode *phi = it2->second;
      ScalarMeExpr *oldVar = phi->GetOpnd(index);
      phi->SetOpnd(index, &newVar);
      if (!phi->IsPiAdded()) {
        if (modifiedPhi.find(phi) == modifiedPhi.end()) {
          modifiedPhi[phi] = std::vector<ScalarMeExpr*>(phi->GetOpnds().size(), nullptr);
        }
        if (modifiedPhi[phi][index] == nullptr) {
          modifiedPhi[phi][index] = oldVar;
        }
      }
    }
  }
}

MeExpr *MeSSI::NewMeExpr(MeExpr &meExpr) {
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

MeExpr *MeSSI::ReplaceMeExprExpr(MeExpr &origExpr, MeExpr &meExpr, MeExpr &repExpr) {
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
      if (ivarExpr.GetMu() == &meExpr) {
        CHECK_FATAL(repExpr.GetMeOp() == kMeOpVar, "must be");
        newMeExpr.SetMuVal(static_cast<VarMeExpr*>(&repExpr));
        needRehash = true;
      }
      return needRehash ? NewMeExpr(newMeExpr) : &origExpr;
    }
    default:
      CHECK_FATAL(false, "NYI");
  }
}

bool MeSSI::ReplaceMeExprStmtOpnd(uint32 opndID, MeStmt &meStmt, MeExpr &oldVar, MeExpr &newVar, bool update) {
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
      static_cast<IassignMeStmt*>(&meStmt)->SetLHSVal(static_cast<IvarMeExpr*>(newExpr));
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

bool MeSSI::ReplaceStmtWithNewVar(MeStmt &meStmt, MeExpr &oldVar, MeExpr &newVar, bool update) {
  switch (meStmt.GetOp()) {
    case OP_dassign:
    case OP_maydassign:
    case OP_brtrue:
    case OP_brfalse: {
      return ReplaceMeExprStmtOpnd(0, meStmt, oldVar, newVar, update);
    }
    case OP_iassign: {
      bool baseIsReplaced = ReplaceMeExprStmtOpnd(0, meStmt, oldVar, newVar, update);
      bool rhsReplaced = ReplaceMeExprStmtOpnd(1, meStmt, oldVar, newVar, update);
      return baseIsReplaced || rhsReplaced;
    }
    case OP_intrinsiccallwithtype: {
      auto &callNode = static_cast<IntrinsiccallMeStmt&>(meStmt);
      if (callNode.GetIntrinsic() != INTRN_JAVA_CHECK_CAST) {
        break;
      }
      return ReplaceMeExprStmtOpnd(0, meStmt, oldVar, newVar, update);
    }
    case OP_regassign: {
      CHECK_FATAL(false, "should not happen");
    }
    default: {
      break;
    }
  }
  return false;
}

bool MeSSI::ReplaceStmt(MeStmt &meStmt, VarMeExpr &newVar, VarMeExpr &oldVar) {
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
  return (lhs != nullptr && lhs->GetMeOp() == kMeOpVar && static_cast<VarMeExpr*>(lhs)->GetOStIdx() == ostIdx);
}

void MeSSI::Rename() {
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

void MeSSI::ReplaceBB(BB &bb, BB &parentBB, DefPoint &newDefPoint) {
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

void MeSSI::RemoveExtraNodes() {
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
    MePhiNode *phi = pair.first;
    for (size_t i = 0; i < pair.second.size(); ++i) {
      size_t index = i;
      ScalarMeExpr*oldVar = pair.second[i];
      if (oldVar != nullptr) {
        phi->SetOpnd(index, oldVar);
      }
    }
  }
}

MIRType *MeSSI::GetInferredType(MeExpr *expr) {
  if (inferredType.find(expr) == inferredType.end()) {
    return nullptr;
  }
  return inferredType[expr];
}

void MeSSI::ConvertToSSI() {
  InsertPiNodes();
  InsertPhiNodes();
  Rename();
}
void MeSSI::ConvertToSSA() {
  RemoveExtraNodes();
}
}
