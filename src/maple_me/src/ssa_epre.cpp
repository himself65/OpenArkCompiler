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
#include "ssa_epre.h"
namespace maple {
void SSAEPre::GenerateSaveLHSRealocc(MeRealOcc *realOcc, MeExpr *regOrVar) {
  CHECK_FATAL(realOcc->GetOpcodeOfMeStmt() == OP_iassign, "GenerateSaveLHSReal: only iassign expected");
  auto *iass = static_cast<IassignMeStmt*>(realOcc->GetMeStmt());
  IvarMeExpr *theLHS = iass->GetLHSVal();
  MeExpr *savedRHS = iass->GetRHS();
  TyIdx savedTyIdx = iass->GetTyIdx();
  MapleMap<OStIdx, ChiMeNode*> savedChiList = *iass->GetChiList();
  iass->GetChiList()->clear();
  SrcPosition savedSrcPos = iass->GetSrcPosition();
  BB *savedBB = iass->GetBB();
  MeStmt *savedPrev = iass->GetPrev();
  MeStmt *savedNext = iass->GetNext();
  RegassignMeStmt *rass = nullptr;
  if (!workCand->NeedLocalRefVar() || GetPlacementRCOn()) {
    CHECK_FATAL(regOrVar->GetMeOp() == kMeOpReg, "GenerateSaveLHSRealocc: EPRE temp must b e preg here");
    // change original iassign to regassign;
    // use placement new to modify in place, because other occ nodes are pointing
    // to this statement in order to get to the rhs expression;
    // this assumes RegassignMeStmt has smaller size then IassignMeStmt
    rass = new (iass) RegassignMeStmt();
    rass->SetLHS(static_cast<RegMeExpr*>(regOrVar));
    rass->SetRHS(savedRHS);
    rass->SetSrcPos(savedSrcPos);
    rass->SetBB(savedBB);
    rass->SetPrev(savedPrev);
    rass->SetNext(savedNext);
    regOrVar->SetDefByStmt(*rass);
  } else {
    // regOrVar is kMeOpReg and localRefVar is kMeOpVar
    VarMeExpr *localRefVar = CreateNewCurLocalRefVar();
    temp2LocalRefVarMap[static_cast<RegMeExpr*>(regOrVar)] = localRefVar;
    // generate localRefVar = saved_rhs by changing original iassign to dassign;
    // use placement new to modify in place, because other occ nodes are pointing
    // to this statement in order to get to the rhs expression;
    // this assumes DassignMeStmt has smaller size then IassignMeStmt
    DassignMeStmt *dass = new (iass) DassignMeStmt(&irMap->GetIRMapAlloc());
    dass->SetLHS(localRefVar);
    dass->SetRHS(savedRHS);
    dass->SetSrcPos(savedSrcPos);
    dass->SetBB(savedBB);
    dass->SetPrev(savedPrev);
    dass->SetNext(savedNext);
    localRefVar->SetDefByStmt(*dass);
    rass = irMap->CreateRegassignMeStmt(*regOrVar, *localRefVar, *savedBB);
    regOrVar->SetDefByStmt(*rass);
    savedBB->InsertMeStmtAfter(dass, rass);
    EnterCandsForSSAUpdate(localRefVar->GetOStIdx(), savedBB);
  }
  // create new iassign for original lhs
  IassignMeStmt *newIass = irMap->NewInPool<IassignMeStmt>(savedTyIdx, theLHS, regOrVar, &savedChiList);
  theLHS->SetDefStmt(newIass);
  newIass->SetBB(savedBB);
  savedBB->InsertMeStmtAfter(rass, newIass);
  // go throu saved_Chi_List to update each chi base to point to newIass
  for (auto it = newIass->GetChiList()->begin(); it != newIass->GetChiList()->end(); it++) {
    ChiMeNode *chi = it->second;
    chi->SetBase(newIass);
  }
  realOcc->SetSavedExpr(*regOrVar);
}

void SSAEPre::GenerateSaveRealOcc(MeRealOcc *realOcc) {
  ASSERT(GetPUIdx() == workCand->GetPUIdx() || workCand->GetPUIdx() == 0,
         "GenerateSaveRealOcc: inconsistent puIdx");
  MeExpr *regOrVar = CreateNewCurTemp(realOcc->GetMeExpr());
  if (realOcc->IsLHS()) {
    GenerateSaveLHSRealocc(realOcc, regOrVar);
    return;
  }
  // create a new meStmt before realOcc->GetMeStmt()
  MeStmt *newMeStmt = nullptr;
  bool isRHSOfDassign = false;
  if (workCand->NeedLocalRefVar() && (realOcc->GetOpcodeOfMeStmt() == OP_dassign) &&
      (realOcc->GetMeStmt()->GetOpnd(0) == realOcc->GetMeExpr())) {
    isRHSOfDassign = true;
    // setting flag so delegaterc will skip
    static_cast<DassignMeStmt*>(realOcc->GetMeStmt())->GetVarLHS()->SetNoDelegateRC(1);
  }
  if (!workCand->NeedLocalRefVar() || isRHSOfDassign || GetPlacementRCOn()) {
    if (regOrVar->GetMeOp() == kMeOpReg) {
      newMeStmt = irMap->CreateRegassignMeStmt(*regOrVar, *realOcc->GetMeExpr(), *realOcc->GetMeStmt()->GetBB());
    } else {
      newMeStmt = irMap->CreateDassignMeStmt(*regOrVar, *realOcc->GetMeExpr(), *realOcc->GetMeStmt()->GetBB());
    }
    regOrVar->SetDefByStmt(*newMeStmt);
    realOcc->GetMeStmt()->GetBB()->InsertMeStmtBefore(realOcc->GetMeStmt(), newMeStmt);
  } else {
    // regOrVar is MeOp_reg and localRefVar is kMeOpVar
    VarMeExpr *localRefVar = CreateNewCurLocalRefVar();
    temp2LocalRefVarMap[static_cast<RegMeExpr*>(regOrVar)] = localRefVar;
    newMeStmt = irMap->CreateDassignMeStmt(*localRefVar, *realOcc->GetMeExpr(), *realOcc->GetMeStmt()->GetBB());
    localRefVar->SetDefByStmt(*newMeStmt);
    realOcc->GetMeStmt()->GetBB()->InsertMeStmtBefore(realOcc->GetMeStmt(), newMeStmt);
    newMeStmt = irMap->CreateRegassignMeStmt(*regOrVar, *localRefVar, *realOcc->GetMeStmt()->GetBB());
    regOrVar->SetDefByStmt(*newMeStmt);
    realOcc->GetMeStmt()->GetBB()->InsertMeStmtBefore(realOcc->GetMeStmt(), newMeStmt);
    EnterCandsForSSAUpdate(localRefVar->GetOStIdx(), realOcc->GetMeStmt()->GetBB());
  }
  // replace realOcc->GetMeStmt()'s occ with regOrVar
  bool isReplaced = irMap->ReplaceMeExprStmt(*realOcc->GetMeStmt(), *realOcc->GetMeExpr(), *regOrVar);
  // rebuild worklist
  if (isReplaced) {
    BuildWorkListStmt(realOcc->GetMeStmt(), realOcc->GetSequence(), true, regOrVar);
  }
  realOcc->SetSavedExpr(*regOrVar);
}

void SSAEPre::GenerateReloadRealOcc(MeRealOcc *realOcc) {
  CHECK_FATAL(!realOcc->IsLHS(), "GenerateReloadRealOcc: cannot be LHS occurrence");
  MeExpr *regOrVar = nullptr;
  MeOccur *defOcc = realOcc->GetDef();
  if (defOcc->GetOccType() == kOccReal) {
    auto *defRealOcc = static_cast<MeRealOcc*>(defOcc);
    regOrVar = defRealOcc->GetSavedExpr();
  } else if (defOcc->GetOccType() == kOccPhiocc) {
    auto *defPhiOcc = static_cast<MePhiOcc*>(defOcc);
    MeRegPhiNode *regPhi = defPhiOcc->GetRegPhi();
    if (regPhi != nullptr) {
      regOrVar = regPhi->GetLHS();
    } else {
      regOrVar = defPhiOcc->GetVarPhi()->GetLHS();
    }
  } else if (defOcc->GetOccType() == kOccInserted) {
    auto *defInsertedOcc = static_cast<MeInsertedOcc*>(defOcc);
    regOrVar = defInsertedOcc->GetSavedExpr();
  } else {
    CHECK_FATAL(false, "NYI");
  }
  ASSERT(regOrVar, "temp not yet generated");
  // replace realOcc->GetMeStmt()'s occ with regOrVar
  bool isReplaced = irMap->ReplaceMeExprStmt(*realOcc->GetMeStmt(), *realOcc->GetMeExpr(), *regOrVar);
  // update worklist
  if (isReplaced) {
    BuildWorkListStmt(realOcc->GetMeStmt(), realOcc->GetSequence(), true, regOrVar);
  }
}

// for each variable in realz that is defined by a phi, replace it by the jth
// phi opnd
MeExpr *SSAEPre::PhiOpndFromRes(MeRealOcc *realZ, size_t j) {
  MeOccur *defZ = realZ->GetDef();
  CHECK_FATAL(defZ, "must be def by phiocc");
  CHECK_FATAL(defZ->GetOccType() == kOccPhiocc, "must be def by phiocc");
  MeExpr *exprQ = CopyMeExpr(utils::ToRef(realZ->GetMeExpr()));
  BB *ePhiBB = defZ->GetBB();
  CHECK_FATAL(exprQ != nullptr, "nullptr check");
  switch (exprQ->GetMeOp()) {
    case kMeOpOp: {
      auto *opMeExpr = static_cast<OpMeExpr*>(exprQ);
      for (size_t i = 0; i < 3; i++) {
        MeExpr *opnd = opMeExpr->GetOpnd(i);
        if (opnd == nullptr) {
          break;
        };
        MeExpr *retOpnd = GetReplaceMeExpr(opnd, ePhiBB, j);
        if (retOpnd != nullptr) {
          opMeExpr->SetOpnd(i, retOpnd);
        }
      }
      break;
    }
    case kMeOpNary: {
      auto *naryMeExpr = static_cast<NaryMeExpr*>(exprQ);
      MapleVector<MeExpr*> &opnds = naryMeExpr->GetOpnds();
      for (size_t i = 0; i < opnds.size(); i++) {
        MeExpr *retOpnd = GetReplaceMeExpr(opnds[i], ePhiBB, j);
        if (retOpnd) {
          opnds[i] = retOpnd;
        }
      }
      break;
    }
    case kMeOpIvar: {
      auto *ivarMeExpr = static_cast<IvarMeExpr*>(exprQ);
      MeExpr *retOpnd = GetReplaceMeExpr(ivarMeExpr->GetBase(), ePhiBB, j);
      if (retOpnd) {
        ivarMeExpr->SetBase(retOpnd);
      }
      MeExpr *muOpnd = GetReplaceMeExpr(ivarMeExpr->GetMu(), ePhiBB, j);
      if (muOpnd != nullptr) {
        ivarMeExpr->SetMuVal(static_cast<VarMeExpr*>(muOpnd));
      }
      break;
    }
    default:
      ASSERT(false, "NYI");
  }
  return irMap->HashMeExpr(*exprQ);
}

// Df phis are computed into the df_phis set; Var Phis in the var_phis set
void SSAEPre::ComputeVarAndDfPhis() {
  varPhiDfns.clear();
  dfPhiDfns.clear();
  const MapleVector<MeRealOcc*> &realOccList = workCand->GetRealOccs();
  CHECK_FATAL(!dom->IsBBVecEmpty(), "size to be allocated is 0");
  for (auto it = realOccList.begin(); it != realOccList.end(); it++) {
    MeRealOcc *realOcc = *it;
    BB *defBB = realOcc->GetBB();
    std::vector<bool> visitedMap(dom->GetBBVecSize(), false);
    GetIterDomFrontier(*defBB, dfPhiDfns, visitedMap);
    MeExpr *meExpr = realOcc->GetMeExpr();
    switch (meExpr->GetMeOp()) {
      case kMeOpOp: {
        auto *meExprOp = static_cast<OpMeExpr*>(meExpr);
        for (uint32 i = 0; i < 3; i++) {
          MeExpr *kidExpr = meExprOp->GetOpnd(i);
          if (kidExpr != nullptr) {
            SetVarPhis(kidExpr);
          }
        }
        break;
      }
      case kMeOpNary: {
        auto *naryMeExpr = static_cast<NaryMeExpr*>(meExpr);
        MapleVector<MeExpr*> &opnds = naryMeExpr->GetOpnds();
        for (size_t i = 0; i < opnds.size(); i++) {
          MeExpr *kidExpr = opnds[i];
          if (kidExpr) {
            SetVarPhis(kidExpr);
          }
        }
        break;
      }
      case kMeOpIvar: {
        auto *ivarMeExpr = static_cast<IvarMeExpr*>(meExpr);
        SetVarPhis(ivarMeExpr->GetBase());
        break;
      }
      default:
        CHECK_FATAL(false, "NYI");
    }
  }
}

// build worklist for each expression;
// isRebuild means the expression is built from second time, in which case,
// tempVar is not nullptr, and it matches only expressions with tempVar as one of
// its operands; isRebuild is true only when called from the code motion phase
void SSAEPre::BuildWorkListExpr(MeStmt *meStmt, int32 seqStmt, MeExpr *meExpr, bool isRebuild, MeExpr *tempVar,
                                bool isRootExpr) {
  if (meExpr->GetTreeID() == (curTreeId + 1)) {
    return;  // already visited twice in the same tree
  }
  MeExprOp meOp = meExpr->GetMeOp();
  switch (meOp) {
    case kMeOpOp: {
      auto *meOpExpr = static_cast<OpMeExpr*>(meExpr);
      bool isHypo = true;
      bool hasTempVarAs1Opnd = false;
      for (uint32 i = 0; i < 3; i++) {
        MeExpr *opnd = meOpExpr->GetOpnd(i);
        if (opnd != nullptr) {
          if (!opnd->IsLeaf()) {
            BuildWorkListExpr(meStmt, seqStmt, opnd, isRebuild, tempVar, false);
            isHypo = false;
          } else if (LeafIsVolatile(opnd)) {
            isHypo = false;
          } else if (tempVar != nullptr && opnd->IsUseSameSymbol(*tempVar)) {
            hasTempVarAs1Opnd = true;
          }
        }
      }
      if (meExpr->GetPrimType() == PTY_agg) {
        isHypo = false;
      }
      if (isHypo && (!isRebuild || hasTempVarAs1Opnd) && !(isRootExpr && kOpcodeInfo.IsCompare(meOpExpr->GetOp())) &&
          meOpExpr->GetOp() != OP_gcmallocjarray && meOpExpr->GetOp() != OP_gcmalloc &&
          (epreIncludeRef || meOpExpr->GetPrimType() != PTY_ref)) {
        // create a HypotheTemp for this expr
        // Exclude cmp operator
        CreateRealOcc(*meStmt, seqStmt, *meExpr, isRebuild);
      }
      break;
    }
    case kMeOpNary: {
      auto *naryMeExpr = static_cast<NaryMeExpr*>(meExpr);
      bool isHypo = true;
      bool hasTempVarAs1Opnd = false;
      MapleVector<MeExpr*> &opnds = naryMeExpr->GetOpnds();
      for (auto it = opnds.begin(); it != opnds.end(); it++) {
        MeExpr *opnd = *it;
        if (!opnd->IsLeaf()) {
          BuildWorkListExpr(meStmt, seqStmt, opnd, isRebuild, tempVar, false);
          isHypo = false;
        } else if (LeafIsVolatile(opnd)) {
          isHypo = false;
        } else if (tempVar != nullptr && opnd->IsUseSameSymbol(*tempVar)) {
          hasTempVarAs1Opnd = true;
        }
      }
      if (meExpr->GetPrimType() == PTY_agg) {
        isHypo = false;
      }
      if (isHypo && (!isRebuild || hasTempVarAs1Opnd) && naryMeExpr->GetPrimType() != PTY_u1 &&
          (GetPrimTypeSize(naryMeExpr->GetPrimType()) >= 4 || IsPrimitivePoint(naryMeExpr->GetPrimType()) ||
           (naryMeExpr->GetOp() == OP_intrinsicop && IntrinDesc::intrinTable[naryMeExpr->GetIntrinsic()].IsPure())) &&
          (epreIncludeRef || naryMeExpr->GetPrimType() != PTY_ref)) {
        if (meExpr->GetOp() == OP_array) {
          MIRType *mirType = GlobalTables::GetTypeTable().GetTypeTable().at(naryMeExpr->GetTyIdx());
          CHECK_FATAL(mirType->GetKind() == kTypePointer, "array must have pointer type");
          auto *ptrMIRType = static_cast<MIRPtrType*>(mirType);
          MIRJarrayType *arryType = safe_cast<MIRJarrayType>(ptrMIRType->GetPointedType());
          if (arryType == nullptr) {
            CreateRealOcc(*meStmt, seqStmt, *meExpr, isRebuild);
          } else {
            int dim = arryType->GetDim();  // to compute the dim field
            if (dim < 2) {
              CreateRealOcc(*meStmt, seqStmt, *meExpr, isRebuild);
            } else {
              if (GetSSAPreDebug()) {
                mirModule->GetOut() << "----- real occ suppressed for jarray with dim " << dim << '\n';
              }
            }
          }
        } else {
          IntrinDesc *intrinDesc = &IntrinDesc::intrinTable[naryMeExpr->GetIntrinsic()];
          if (CfgHasDoWhile() && naryMeExpr->GetIntrinsic() == INTRN_JAVA_ARRAY_LENGTH) {
            auto *varOpnd = safe_cast<VarMeExpr>(naryMeExpr->GetOpnd(0));
            if (varOpnd != nullptr) {
              const OriginalSt *ost = ssaTab->GetOriginalStFromID(varOpnd->GetOStIdx());
              if (ost->IsFormal()) {
                break;
              }
            }
          }
          if (!intrinDesc->IsLoadMem()) {
            CreateRealOcc(*meStmt, seqStmt, *meExpr, isRebuild);
          }
        }
      }
      break;
    }
    case kMeOpIvar: {
      auto *ivarMeExpr = static_cast<IvarMeExpr*>(meExpr);
      MeExpr *base = ivarMeExpr->GetBase();
      if (meExpr->GetPrimType() == PTY_agg) {
        break;
      }
      if (!base->IsLeaf()) {
        BuildWorkListExpr(meStmt, seqStmt, ivarMeExpr->GetBase(), isRebuild, tempVar, false);
      } else if (ivarMeExpr->IsVolatile()) {
        break;
      } else if (!epreIncludeRef && ivarMeExpr->GetPrimType() == PTY_ref) {
        break;
      } else if (!isRebuild || base->IsUseSameSymbol(*tempVar)) {
        CreateRealOcc(*meStmt, seqStmt, *meExpr, isRebuild);
      }
      break;
    }
    case kMeOpVar:
    case kMeOpReg:
    case kMeOpAddrof:
    case kMeOpAddroffunc:
    case kMeOpGcmalloc:
    case kMeOpConst:
    case kMeOpConststr:
    case kMeOpConststr16:
    case kMeOpSizeoftype:
    case kMeOpFieldsDist:
      break;
    default:
      CHECK_FATAL(false, "MeOP NIY");
  }
  if (meExpr->GetTreeID() == curTreeId) {
    meExpr->SetTreeID(curTreeId + 1);  // just processed 2nd time; not
  }
  // to be processed again in this tree
  else {
    meExpr->SetTreeID(curTreeId);  // just processed 1st time; willing to
  }
  // process one more time
  return;
}

void SSAEPre::BuildWorkListIvarLHSOcc(MeStmt *meStmt, int32 seqStmt, bool isRebuild, MeExpr *tempVar) {
  if (!enableLHSIvar || GetPlacementRCOn()) {
    return;
  }
  if (meStmt->GetOp() != OP_iassign) {
    return;
  }
  auto *iass = static_cast<IassignMeStmt*>(meStmt);
  IvarMeExpr *ivarMeExpr = iass->GetLHSVal();
  if (ivarMeExpr->GetPrimType() == PTY_agg) {
    return;
  }
  if (ivarMeExpr->IsVolatile()) {
    return;
  }
  if (!epreIncludeRef && ivarMeExpr->GetPrimType() == PTY_ref) {
    return;
  }
  MeExpr *base = ivarMeExpr->GetBase();
  if (!base->IsLeaf()) {
    return;
  }
  if (!isRebuild || base->IsUseSameSymbol(*tempVar)) {
    CreateRealOcc(*meStmt, seqStmt, *ivarMeExpr, isRebuild, true);
  }
}

// collect meExpr's variables and put them into varVec
// varVec can only store RegMeExpr and VarMeExpr
void SSAEPre::CollectVarForMeExpr(MeExpr *meExpr, std::vector<MeExpr*> &varVec) {
  switch (meExpr->GetMeOp()) {
    case kMeOpOp: {
      for (uint32 i = 0; i < 3; i++) {
        auto *opMeExpr = static_cast<OpMeExpr*>(meExpr);
        MeExpr *opnd = opMeExpr->GetOpnd(i);
        if (opnd && (opnd->GetMeOp() == kMeOpVar || opnd->GetMeOp() == kMeOpReg)) {
          varVec.push_back(opnd);
        }
      }
      break;
    }
    case kMeOpNary: {
      auto *naryMeExpr = static_cast<NaryMeExpr*>(meExpr);
      MapleVector<MeExpr*> &opnds = naryMeExpr->GetOpnds();
      for (MeExpr *kidExpr : opnds) {
        if (kidExpr->GetMeOp() == kMeOpVar || kidExpr->GetMeOp() == kMeOpReg) {
          varVec.push_back(kidExpr);
        }
      }
      break;
    }
    case kMeOpIvar: {
      auto *ivarMeExpr = static_cast<IvarMeExpr*>(meExpr);
      CHECK_FATAL(ivarMeExpr->GetBase()->GetMeOp() == kMeOpVar || ivarMeExpr->GetBase()->GetMeOp() == kMeOpConst ||
                  ivarMeExpr->GetBase()->GetMeOp() == kMeOpAddrof || ivarMeExpr->GetBase()->GetMeOp() == kMeOpReg,
                  "ivarMeExpr not first order expr");
      if (ivarMeExpr->GetBase()->GetMeOp() == kMeOpVar || ivarMeExpr->GetBase()->GetMeOp() == kMeOpReg) {
        varVec.push_back(ivarMeExpr->GetBase());
      }
      // in case of lhs occurrence, mu can be nullptr, and can use nullptr as value
      varVec.push_back(ivarMeExpr->GetMu());
      break;
    }
    default:
      ASSERT(false, "should not be here");
  }
}

void SSAEPre::CollectVarForCand(MeRealOcc *realOcc, std::vector<MeExpr*> &varVec) {
  CollectVarForMeExpr(realOcc->GetMeExpr(), varVec);
}
}  // namespace maple
