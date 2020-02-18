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
#include "me_ssa_lpre.h"
#include "mir_builder.h"
#include "me_lower_globals.h"

namespace maple {
void MeSSALPre::GenerateSaveRealOcc(MeRealOcc *realOcc) {
  ASSERT(GetPUIdx() == workCand->GetPUIdx() || workCand->GetPUIdx() == 0,
         "GenerateSaveRealOcc: inconsistent puIdx");
  MeExpr *regOrVar = CreateNewCurTemp(realOcc->GetMeExpr());
  if (!realOcc->IsLHS()) {
    // create a new meStmt before realOcc->GetMeStmt()
    MeStmt *newMeStmt = irMap->CreateRegassignMeStmt(*regOrVar, *realOcc->GetMeExpr(), *realOcc->GetMeStmt()->GetBB());
    regOrVar->SetDefByStmt(*newMeStmt);
    realOcc->GetMeStmt()->GetBB()->InsertMeStmtBefore(realOcc->GetMeStmt(), newMeStmt);
    // replace realOcc->GetMeStmt()'s occ with regOrVar
    (void)irMap->ReplaceMeExprStmt(*realOcc->GetMeStmt(), *realOcc->GetMeExpr(), *regOrVar);
  } else if (realOcc->IsFormalAtEntry()) {
    // no need generate any code, but change formal declaration to preg
    CHECK_FATAL(regOrVar->GetMeOp() == kMeOpReg, "formals not promoted to register");
    auto *varMeExpr = static_cast<VarMeExpr*>(realOcc->GetMeExpr());
    const MIRSymbol *oldFormalSt = ssaTab->GetMIRSymbolFromID(varMeExpr->GetOStIdx());
    auto *regFormal = static_cast<RegMeExpr*>(regOrVar);
    MIRSymbol *newFormalSt = mirModule->GetMIRBuilder()->CreatePregFormalSymbol(oldFormalSt->GetTyIdx(),
                                                                                regFormal->GetRegIdx(),
                                                                                *func->GetMirFunc());
    size_t i = 0;
    for (; i < func->GetMirFunc()->GetFormalCount(); ++i) {
      if (func->GetMirFunc()->GetFormal(i) == oldFormalSt) {
        func->GetMirFunc()->SetFormal(i, newFormalSt);
        break;
      }
    }
    CHECK_FATAL(i < func->GetMirFunc()->GetFormalCount(), "Cannot replace promoted formal");
  } else if (realOcc->GetOpcodeOfMeStmt() == OP_dassign || realOcc->GetOpcodeOfMeStmt() == OP_maydassign) {
    VarMeExpr *theLHS = realOcc->GetMeStmt()->GetVarLHS();
    MeExpr *savedRHS = realOcc->GetMeStmt()->GetRHS();
    CHECK_NULL_FATAL(theLHS);
    CHECK_NULL_FATAL(savedRHS);
    CHECK_NULL_FATAL(realOcc->GetMeStmt()->GetChiList());
    realOcc->GetMeStmt()->GetChiList()->clear();
    SrcPosition savedSrcPos = realOcc->GetMeStmt()->GetSrcPosition();
    BB *savedBB = realOcc->GetMeStmt()->GetBB();
    MeStmt *savedPrev = realOcc->GetMeStmt()->GetPrev();
    MeStmt *savedNext = realOcc->GetMeStmt()->GetNext();
    // change original dassign/maydassign to regassign;
    // use placement new to modify in place, because other occ nodes are pointing
    // to this statement in order to get to the rhs expression;
    // this assume RegassignMeStmt has smaller size then DassignMeStmt and
    // MaydassignMeStmt
    RegassignMeStmt *rass = new (realOcc->GetMeStmt()) RegassignMeStmt();
    rass->SetLHS(static_cast<RegMeExpr*>(regOrVar));
    rass->SetRHS(savedRHS);
    rass->SetSrcPos(savedSrcPos);
    rass->SetBB(savedBB);
    rass->SetPrev(savedPrev);
    rass->SetNext(savedNext);
    regOrVar->SetDefByStmt(*rass);
    // create new dassign for original lhs
    MeStmt *newDassign = irMap->CreateDassignMeStmt(*theLHS, *regOrVar, *savedBB);
    theLHS->SetDefByStmt(*newDassign);
    savedBB->InsertMeStmtAfter(realOcc->GetMeStmt(), newDassign);
  } else {
    CHECK_FATAL(kOpcodeInfo.IsCallAssigned(realOcc->GetOpcodeOfMeStmt()),
                "LHS real occurrence has unrecognized stmt type");
    MapleVector<MustDefMeNode> *mustDefList = realOcc->GetMeStmt()->GetMustDefList();
    CHECK_NULL_FATAL(mustDefList);
    ASSERT(!mustDefList->empty(), "empty mustdef in callassigned stmt");
    MustDefMeNode *mustDefMeNode = &mustDefList->front();
    if (regOrVar->GetMeOp() == kMeOpReg) {
      auto *theLHS = static_cast<VarMeExpr*>(mustDefMeNode->GetLHS());
      // change mustDef lhs to regOrVar
      mustDefMeNode->UpdateLHS(*regOrVar);
      // create new dassign for original lhs
      MeStmt *newDassign = irMap->CreateDassignMeStmt(*theLHS, *regOrVar, *realOcc->GetMeStmt()->GetBB());
      theLHS->SetDefByStmt(*newDassign);
      realOcc->GetMeStmt()->GetBB()->InsertMeStmtAfter(realOcc->GetMeStmt(), newDassign);
    } else {
      CHECK_FATAL(false, "GenerateSaveRealOcc: non-reg temp for callassigned LHS occurrence NYI");
    }
  }
  realOcc->SetSavedExpr(*regOrVar);
}

void MeSSALPre::GenerateReloadRealOcc(MeRealOcc *realOcc) {
  CHECK_FATAL(!realOcc->IsLHS(), "GenerateReloadRealOcc: cannot be LHS occurrence");
  MeExpr *regOrVar = nullptr;
  MeOccur *defOcc = realOcc->GetDef();
  if (defOcc->GetOccType() == kOccReal) {
    auto *defRealOcc = static_cast<MeRealOcc*>(defOcc);
    regOrVar = defRealOcc->GetSavedExpr();
  } else if (defOcc->GetOccType() == kOccPhiocc) {
    auto *defPhiOcc = static_cast<MePhiOcc*>(defOcc);
    MeRegPhiNode *regPhi = defPhiOcc->GetRegPhi();
    regOrVar = regPhi->GetLHS();
  } else if (defOcc->GetOccType() == kOccInserted) {
    auto *defInsertedOcc = static_cast<MeInsertedOcc*>(defOcc);
    regOrVar = defInsertedOcc->GetSavedExpr();
  } else {
    CHECK_FATAL(false, "NYI");
  }
  CHECK_NULL_FATAL(regOrVar);
  // replace realOcc->GetMeStmt()'s occ with regOrVar
  (void)irMap->ReplaceMeExprStmt(*realOcc->GetMeStmt(), *realOcc->GetMeExpr(), *regOrVar);
}

// the variable in realZ is defined by a phi; replace it by the jth phi opnd
MeExpr *MeSSALPre::PhiOpndFromRes(MeRealOcc *realZ, size_t j) {
  MeOccur *defZ = realZ->GetDef();
  CHECK_NULL_FATAL(defZ);
  CHECK_FATAL(defZ->GetOccType() == kOccPhiocc, "must be def by phiocc");
  MeExpr *meExprZ = realZ->GetMeExpr();
  BB *ePhiBB = static_cast<MePhiOcc*>(defZ)->GetBB();
  MeExpr *retVar = GetReplaceMeExpr(meExprZ, ePhiBB, j);
  return (retVar != nullptr) ? retVar : meExprZ;
}

// accumulate the BBs that are in the iterated dominance frontiers of bb in
// the set dfSet, visiting each BB only once
void MeSSALPre::GetIterDomFrontier(BB &bb, MapleSet<uint32> &dfSet, std::vector<bool> &visitedMap) {
  CHECK_FATAL(bb.GetBBId() < visitedMap.size(), "index out of range in MeSSALPre::GetIterDomFrontier");
  if (visitedMap[bb.GetBBId()]) {
    return;
  }
  visitedMap[bb.GetBBId()] = true;
  CHECK_FATAL(bb.GetBBId() < dom->GetDomFrontierSize(), "index out of range in MeSSALPre::GetIterDomFrontier");
  for (BBId frontierBBId : dom->GetDomFrontier(bb.GetBBId())) {
    dfSet.insert(dom->GetDtDfnItem(frontierBBId));
    BB *frontierBB = GetBB(frontierBBId);
    GetIterDomFrontier(*frontierBB, dfSet, visitedMap);
  }
}

void MeSSALPre::ComputeVarAndDfPhis() {
  varPhiDfns.clear();
  dfPhiDfns.clear();
  PreWorkCand *workCand = GetWorkCand();
  const MapleVector<MeRealOcc*> &realOccList = workCand->GetRealOccs();
  CHECK_FATAL(!dom->IsBBVecEmpty(), "size to be allocated is 0");
  for (auto it = realOccList.begin(); it != realOccList.end(); ++it) {
    std::vector<bool> visitedMap(dom->GetBBVecSize(), false);
    MeRealOcc *realOcc = *it;
    BB *defBB = realOcc->GetBB();
    GetIterDomFrontier(*defBB, dfPhiDfns, visitedMap);
    MeExpr *meExpr = realOcc->GetMeExpr();
    if (meExpr->GetMeOp() == kMeOpVar) {
      SetVarPhis(meExpr);
    }
  }
}

void MeSSALPre::BuildEntryLHSOcc4Formals() {
  if (preKind == kAddrPre) {
    return;
  }
  PreWorkCand *workCand = GetWorkCand();
  auto *varMeExpr = static_cast<VarMeExpr*>(workCand->GetTheMeExpr());
  const OriginalSt *ost = ssaTab->GetSymbolOriginalStFromID(varMeExpr->GetOStIdx());
  if (!ost->IsFormal()) {
    return;
  }
  if (assignedFormals.find(ost->GetIndex()) != assignedFormals.end()) {
    return;  // the formal's memory location has to be preserved
  }
  // Avoid promoting formals if it has been marked localrefvar
  if (ost->HasAttr(ATTR_localrefvar)) {
    return;
  }
  // get the zero version VarMeExpr node
  VarMeExpr *zeroVersion = irMap->GetOrCreateZeroVersionVarMeExpr(*ost);
  MeRealOcc *occ = ssaPreMemPool->New<MeRealOcc>(nullptr, 0, zeroVersion);
  auto occIt = workCand->GetRealOccs().begin();
  workCand->GetRealOccs().insert(occIt, occ);  // insert at beginning
  occ->SetIsLHS(true);
  occ->SetIsFormalAtEntry(true);
  occ->SetBB(*func->GetFirstBB());
}

void MeSSALPre::BuildWorkListLHSOcc(MeStmt *meStmt, int32 seqStmt) {
  if (preKind == kAddrPre) {
    return;
  }
  if (meStmt->GetOp() == OP_dassign || meStmt->GetOp() == OP_maydassign) {
    VarMeExpr *lhs = meStmt->GetVarLHS();
    CHECK_NULL_FATAL(lhs);
    const OriginalSt *ost = ssaTab->GetSymbolOriginalStFromID(lhs->GetOStIdx());
    if (ost->IsFormal()) {
      assignedFormals.insert(ost->GetIndex());
    }
    CHECK_NULL_FATAL(meStmt->GetRHS());
    if (ost->IsVolatile()) {
      return;
    }
    if (lhs->GetPrimType() == PTY_agg) {
      return;
    }
    (void)CreateRealOcc(*meStmt, seqStmt, *lhs, false, true);
  } else if (kOpcodeInfo.IsCallAssigned(meStmt->GetOp())) {
    MapleVector<MustDefMeNode> *mustDefList = meStmt->GetMustDefList();
    if (mustDefList->empty()) {
      return;
    }
    if (mustDefList->front().GetLHS()->GetMeOp() != kMeOpVar) {
      return;
    }
    auto *theLHS = static_cast<VarMeExpr*>(mustDefList->front().GetLHS());
    const OriginalSt *ost = ssaTab->GetOriginalStFromID(theLHS->GetOStIdx());
    if (ost->IsFormal()) {
      assignedFormals.insert(ost->GetIndex());
    }
    if (theLHS->GetPrimType() == PTY_ref && !MeOption::rcLowering) {
      return;
    }
    if (ost->IsVolatile()) {
      return;
    }
    if (theLHS->GetPrimType() == PTY_agg) {
      return;
    }
    (void)CreateRealOcc(*meStmt, seqStmt, *theLHS, false, true);
  }
}

void MeSSALPre::CreateMembarOccAtCatch(BB &bb) {
  // go thru all workcands and insert a membar occurrence for each of them
  for (size_t i = 0; i < workList.size() && i <= preLimit; ++i) {
    PreWorkCand *workCand = workList[i];
    MeRealOcc *newOcc = ssaPreMemPool->New<MeRealOcc>(nullptr, 0, workCand->GetTheMeExpr());
    newOcc->SetOccType(kOccMembar);
    newOcc->SetBB(bb);
    workCand->AddRealOccAsLast(*newOcc, GetPUIdx());
    if (preKind == kAddrPre) {
      continue;
    }
    auto *varMeExpr = static_cast<VarMeExpr*>(workCand->GetTheMeExpr());
    const OriginalSt *ost = ssaTab->GetOriginalStFromID(varMeExpr->GetOStIdx());
    if (ost->IsFormal()) {
      assignedFormals.insert(ost->GetIndex());
    }
  }
}

// only handle the leaf of load, because all other expressions has been done by
// previous SSAPre
void MeSSALPre::BuildWorkListExpr(MeStmt *meStmt, int32 seqStmt, MeExpr *meExpr, bool, MeExpr*, bool) {
  MeExprOp meOp = meExpr->GetMeOp();
  switch (meOp) {
    case kMeOpVar: {
      if (preKind != kLoadPre) {
        break;
      }
      auto *varMeExpr = static_cast<VarMeExpr*>(meExpr);
      const OriginalSt *ost = ssaTab->GetOriginalStFromID(varMeExpr->GetOStIdx());
      if (ost->IsVolatile()) {
        break;
      }
      const MIRSymbol *sym = ost->GetMIRSymbol();
      if (sym->IsInstrumented()) {
        // not doing because its SSA form is not complete
        break;
      }
      if (meExpr->GetPrimType() == PTY_agg) {
        break;
      }
      (void)CreateRealOcc(*meStmt, seqStmt, *meExpr, false);
      break;
    }
    case kMeOpAddrof: {
      if (preKind != kAddrPre) {
        break;
      }
      auto *addrOfMeExpr = static_cast<AddrofMeExpr*>(meExpr);
      const OriginalSt *ost = ssaTab->GetOriginalStFromID(addrOfMeExpr->GetOstIdx());
      if (ost->IsLocal()) {  // skip lpre for stack addresses as they are cheap
        break;
      }
      (void)CreateRealOcc(*meStmt, seqStmt, *meExpr, false);
      break;
    }
    case kMeOpAddroffunc: {
      if (preKind != kAddrPre) {
        break;
      }
      (void)CreateRealOcc(*meStmt, seqStmt, *meExpr, false);
      break;
    }
    case kMeOpOp: {
      auto *meOpExpr = static_cast<OpMeExpr*>(meExpr);
      for (size_t i = 0; i < kOperandNumTernary; ++i) {
        MeExpr *opnd = meOpExpr->GetOpnd(i);
        if (opnd != nullptr) {
          BuildWorkListExpr(meStmt, seqStmt, opnd, false, nullptr, false);
        }
      }
      break;
    }
    case kMeOpNary: {
      auto *naryMeExpr = static_cast<NaryMeExpr*>(meExpr);
      MapleVector<MeExpr*> &opnds = naryMeExpr->GetOpnds();
      for (auto it = opnds.begin(); it != opnds.end(); ++it) {
        MeExpr *opnd = *it;
        BuildWorkListExpr(meStmt, seqStmt, opnd, false, nullptr, false);
      }
      break;
    }
    case kMeOpIvar: {
      auto *ivarMeExpr = static_cast<IvarMeExpr*>(meExpr);
      BuildWorkListExpr(meStmt, seqStmt, ivarMeExpr->GetBase(), false, nullptr, false);
      break;
    }
    default:
      break;
  }
}

void MeSSALPre::BuildWorkList() {
  MeFunction &tmpFunc = irMap->GetFunc();
  size_t numBBs = dom->GetDtPreOrderSize();
  if (numBBs > kDoLpreBBsLimit) {
    return;
  }
  const MapleVector<BBId> &preOrderDt = dom->GetDtPreOrder();
  for (size_t i = 0; i < numBBs; ++i) {
    BB *bb = tmpFunc.GetBBFromID(preOrderDt[i]);
    BuildWorkListBB(bb);
  }
}

void MeSSALPre::FindLoopHeadBBs(IdentifyLoops *identLoops) {
  for (LoopDesc *mapleLoop : identLoops->GetMeLoops()) {
    if (mapleLoop->head != nullptr) {
      loopHeadBBs.insert(mapleLoop->head->GetBBId());
    }
  }
}

AnalysisResult *MeDoSSALPre::Run(MeFunction *irFunc, MeFuncResultMgr *funcMgr, ModuleResultMgr*) {
  static uint32 puCount = 0;  // count PU to support the lprePULimit option
  if (puCount > MeOption::lprePULimit) {
    ++puCount;
    return nullptr;
  }
  auto *dom = static_cast<Dominance*>(funcMgr->GetAnalysisResult(MeFuncPhase_DOMINANCE, irFunc));
  CHECK_NULL_FATAL(dom);
  auto *irMap = static_cast<MeIRMap*>(funcMgr->GetAnalysisResult(MeFuncPhase_IRMAP, irFunc));
  CHECK_NULL_FATAL(irMap);
  auto *identLoops = static_cast<IdentifyLoops*>(funcMgr->GetAnalysisResult(MeFuncPhase_MELOOP, irFunc));
  CHECK_NULL_FATAL(identLoops);
  bool lprePULimitSpecified = MeOption::lprePULimit != UINT32_MAX;
  uint32 lpreLimitUsed =
      (lprePULimitSpecified && puCount != MeOption::lprePULimit) ? UINT32_MAX : MeOption::lpreLimit;
  {
    MeSSALPre ssaLpre(irFunc, *irMap, *dom, *NewMemPool(), *NewMemPool(), kLoadPre, lpreLimitUsed);
    ssaLpre.SetRcLoweringOn(MeOption::rcLowering);
    ssaLpre.SetRegReadAtReturn(MeOption::regreadAtReturn);
    ssaLpre.SetSpillAtCatch(MeOption::spillAtCatch);
    if (lprePULimitSpecified && puCount == MeOption::lprePULimit && lpreLimitUsed != UINT32_MAX) {
      LogInfo::MapleLogger() << "applying LPRE limit " << lpreLimitUsed << " in function " <<
          irFunc->GetMirFunc()->GetName() << '\n';
    }
    if (DEBUGFUNC(irFunc)) {
      ssaLpre.SetSSAPreDebug(true);
    }
    if (MeOption::lpreSpeculate && !irFunc->HasException()) {
      ssaLpre.FindLoopHeadBBs(identLoops);
    }
    ssaLpre.ApplySSAPRE();
    if (DEBUGFUNC(irFunc)) {
      LogInfo::MapleLogger() << "\n==============after LoadPre =============" << '\n';
      irFunc->Dump(false);
    }
  }
  MeLowerGlobals lowerGlobals(irFunc, irFunc->GetMeSSATab());
  lowerGlobals.Run();
  {
    MeSSALPre ssaLpre(irFunc, *irMap, *dom, *NewMemPool(), *NewMemPool(), kAddrPre, lpreLimitUsed);
    ssaLpre.SetSpillAtCatch(MeOption::spillAtCatch);
    if (DEBUGFUNC(irFunc)) {
      ssaLpre.SetSSAPreDebug(true);
    }
    if (MeOption::lpreSpeculate && !irFunc->HasException()) {
      ssaLpre.FindLoopHeadBBs(identLoops);
    }
    ssaLpre.ApplySSAPRE();
    if (DEBUGFUNC(irFunc)) {
      LogInfo::MapleLogger() << "\n==============after AddrPre =============" << '\n';
      irFunc->Dump(false);
    }
  }
  ++puCount;
  return nullptr;
}
}  // namespace maple
