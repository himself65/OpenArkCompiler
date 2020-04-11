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
#include "me_store_pre.h"

namespace maple {
// ================ Step 6: Code Motion ================
void MeStorePre::CheckCreateCurTemp() {
  if (curTemp != nullptr) {
    return;
  }
  ASSERT_NOT_NULL(irMap);
  // try to use the same preg that LPRE used for the variable
  auto mapIt = irMap->FindLpreTmpsItem(workCand->GetOst()->GetIndex());
  if (mapIt == irMap->GetLpreTmpsEnd()) {
    curTemp = irMap->CreateRegMeExpr(workCand->GetTheVar()->GetPrimType());
  } else {
    curTemp = mapIt->second;
  }
}

// Starting at bb's bottom, search for the definition of workCand->theOst and
// save its RHS in curTemp.  Since the def must dominate, searching by ascending
// the dominator tree is sufficient.  If the def is by a phi, call recursively
// for each phi operand and also insert phi for curTemp if needed.
// bbCurTempMap maps bb to cur_temp_version and is used to avoid re-processing
// each bb.  The return value is the curTemp version that contains the RHS value
// at the entry to bb;
RegMeExpr *MeStorePre::EnsureRHSInCurTemp(BB &bb) {
  CHECK_FATAL(&bb != func->GetCommonEntryBB(), "EnsureRHSInCurTemp: cannot find earlier definition");
  // see if processed before
  auto mapIt = bbCurTempMap.find(&bb);
  if (mapIt != bbCurTempMap.end()) {
    return mapIt->second;
  }
  // traverse statements
  auto &meStmts = bb.GetMeStmts();
  for (auto itStmt = meStmts.rbegin(); itStmt != meStmts.rend(); ++itStmt) {
    if (itStmt->GetOp() == OP_dassign) {
      auto *dass = static_cast<DassignMeStmt*>(to_ptr(itStmt));
      if (dass->GetVarLHS()->GetOStIdx() != workCand->GetOst()->GetIndex()) {
        continue;
      }
      if (enabledDebug) {
        LogInfo::MapleLogger() << "EnsureRHSInCurTemp: found dassign at BB" << bb.GetBBId() << '\n';
      }
      ASSERT_NOT_NULL(curTemp);
      if (dass->GetRHS()->GetMeOp() == kMeOpReg &&
          static_cast<RegMeExpr*>(dass->GetRHS())->GetOstIdx() == curTemp->GetOstIdx()) {
        return static_cast<RegMeExpr*>(dass->GetRHS());
      }
      // create and insert regassign before dass
      RegMeExpr *lhsReg = irMap->CreateRegMeExprVersion(*curTemp);
      RegassignMeStmt *rass = irMap->CreateRegassignMeStmt(*lhsReg, *dass->GetRHS(), bb);
      rass->SetSrcPos(itStmt->GetSrcPosition());
      lhsReg->SetDefByStmt(*rass);
      bb.InsertMeStmtBefore(dass, rass);
      // change dass's rhs to lhsReg
      dass->SetRHS(lhsReg);
      bbCurTempMap[&bb] = lhsReg;
      return lhsReg;
    } else if (kOpcodeInfo.IsCallAssigned(itStmt->GetOp())) {
      MapleVector<MustDefMeNode> *mustDefList = itStmt->GetMustDefList();
      CHECK_NULL_FATAL(mustDefList);
      if (!mustDefList->empty()) {
        MeExpr *mdLHS = mustDefList->front().GetLHS();
        if (mdLHS->GetMeOp() != kMeOpVar) {
          continue;
        }
        auto *lhsVar = static_cast<VarMeExpr*>(mdLHS);
        if (lhsVar->GetOStIdx() != workCand->GetOst()->GetIndex()) {
          continue;
        }
        if (enabledDebug) {
          LogInfo::MapleLogger() << "EnsureRHSInCurTemp: found callassigned at BB" << bb.GetBBId() << '\n';
        }
        // change mustDefList
        RegMeExpr *lhsReg = irMap->CreateRegMeExprVersion(*curTemp);
        mustDefList->front().UpdateLHS(*lhsReg);
        // create dassign
        DassignMeStmt *dass = irMap->CreateDassignMeStmt(*lhsVar, *lhsReg, bb);
        dass->SetSrcPos(itStmt->GetSrcPosition());
        lhsVar->SetDefByStmt(*dass);
        bb.InsertMeStmtAfter(to_ptr(itStmt), dass);
        bbCurTempMap[&bb] = lhsReg;
        return lhsReg;
      }
    }
  }
  // check if there is def by phi
  auto phiIt = bb.GetMevarPhiList().find(workCand->GetOst()->GetIndex());
  if (phiIt != bb.GetMevarPhiList().end()) {
    if (enabledDebug) {
      LogInfo::MapleLogger() << "EnsureRHSInCurTemp: found def-by-phi at BB" << bb.GetBBId() << '\n';
    }
    RegMeExpr *lhsReg = irMap->CreateRegMeExprVersion(*curTemp);
    bbCurTempMap[&bb] = lhsReg;
    // form a new phi for the temp
    MeRegPhiNode *regPhi = irMap->NewInPool<MeRegPhiNode>();
    regPhi->SetLHS(lhsReg);
    regPhi->SetDefBB(&bb);
    // call recursively for each varPhi operands
    for (BB *pred : bb.GetPred()) {
      RegMeExpr *regPhiOpnd = EnsureRHSInCurTemp(*pred);
      CHECK_NULL_FATAL(regPhiOpnd);
      regPhi->GetOpnds().push_back(regPhiOpnd);
      regPhiOpnd->GetPhiUseSet().insert(regPhi);
    }
    // insert the regPhi
    bb.GetMeRegPhiList().insert(std::make_pair(lhsReg->GetOstIdx(), regPhi));
    return lhsReg;
  }
  // continue at immediate dominator
  if (enabledDebug) {
    LogInfo::MapleLogger() << "EnsureRHSInCurTemp: cannot find def at BB" << bb.GetBBId() << '\n';
  }
  RegMeExpr *savedCurTemp = EnsureRHSInCurTemp(*dom->GetDom(bb.GetBBId()));
  CHECK_NULL_FATAL(savedCurTemp);
  bbCurTempMap[&bb] = savedCurTemp;
  return savedCurTemp;
}

void MeStorePre::CodeMotion() {
  // pass 1 only donig insertion
  for (SOcc *occ : allOccs) {
    if (occ->GetOccTy() != kSOccLambdaRes) {
      continue;
    }
    auto *lambdaResOcc = static_cast<SLambdaResOcc*>(occ);
    if (lambdaResOcc->GetInsertHere()) {
      // form the lhs VarMeExpr node
      VarMeExpr *lhsVar = irMap->CreateVarMeExprVersion(*workCand->GetTheVar());
      // create a new dassign
      BB *insertBB = &lambdaResOcc->GetBB();
      CheckCreateCurTemp();
      CHECK_FATAL(insertBB->GetPred().size() == 1, "CodeMotion: encountered critical edge");
      RegMeExpr *rhsReg = EnsureRHSInCurTemp(*insertBB->GetPred(0));
      DassignMeStmt *newDass = irMap->CreateDassignMeStmt(*lhsVar, *rhsReg, *insertBB);
      lhsVar->SetDefByStmt(*newDass);
      // insert at earliest point in BB, but after statements required to be
      // first statements in BBG
      MeStmt *curStmt = to_ptr(insertBB->GetMeStmts().begin());
      while (curStmt != nullptr && (curStmt->GetOp() == OP_catch || curStmt->GetOp() == OP_try ||
             curStmt->GetOp() == OP_comment)) {
        curStmt = curStmt->GetNext();
      }
      if (curStmt == nullptr) {
        insertBB->AddMeStmtLast(newDass);
      } else {
        insertBB->InsertMeStmtBefore(curStmt, newDass);
      }
    }
  }
  // pass 2 only doing deletion
  ASSERT_NOT_NULL(workCand);
  for (SOcc *occ : workCand->GetRealOccs()) {
    if (occ->GetOccTy() != kSOccReal) {
      continue;
    }
    auto *realOcc = static_cast<SRealOcc*>(occ);
    if (realOcc->GetRedundant()) {
      if (realOcc->GetStmt()->GetOp() == OP_dassign) {
        auto *dass = static_cast<DassignMeStmt*>(realOcc->GetStmt());
        if (dass->GetRHS()->CouldThrowException()) {
          // insert a new eval statement
          UnaryMeStmt *evalStmt = irMap->New<UnaryMeStmt>(OP_eval);
          evalStmt->SetBB(dass->GetBB());
          evalStmt->SetSrcPos(dass->GetSrcPosition());
          evalStmt->SetMeStmtOpndValue(dass->GetRHS());
          realOcc->GetBB().InsertMeStmtBefore(dass, evalStmt);
        }
        realOcc->GetBB().RemoveMeStmt(dass);
      } else {
        CHECK_FATAL(kOpcodeInfo.IsCallAssigned(realOcc->GetStmt()->GetOp()), "CodeMotion: callassign expected");
        MapleVector<MustDefMeNode> *mustDefList = realOcc->GetStmt()->GetMustDefList();
        CHECK_NULL_FATAL(mustDefList);
        mustDefList->clear();
      }
    }
  }
}

// ================ Step 0: collect occurrences ================
// create a new real occurrence for the store of meStmt of symbol oidx
void MeStorePre::CreateRealOcc(OStIdx ostIdx, MeStmt &meStmt) {
  SpreWorkCand *wkCand = nullptr;
  auto mapIt = workCandMap.find(ostIdx);
  if (mapIt != workCandMap.end()) {
    wkCand = mapIt->second;
  } else {
    const OriginalSt *ost = ssaTab->GetSymbolOriginalStFromID(ostIdx);
    wkCand = spreMp->New<SpreWorkCand>(spreAllocator, *ost);
    workCandMap[ostIdx] = wkCand;
    // if it is local symbol, insert artificial real occ at common_exit_bb
    if (ost->IsLocal()) {
      SRealOcc *artOcc = spreMp->New<SRealOcc>(*func->GetCommonExitBB());
      wkCand->GetRealOccs().push_back(artOcc);
    }
  }
  if (wkCand->GetTheVar() == nullptr) {
    if (meStmt.GetOp() == OP_dassign) {
      wkCand->SetTheVar(*static_cast<DassignMeStmt*>(&meStmt)->GetVarLHS());
    } else {
      ASSERT(kOpcodeInfo.IsCallAssigned(meStmt.GetOp()), "CreateRealOcc: callassign expected");
      MapleVector<MustDefMeNode> *mustDefList = meStmt.GetMustDefList();
      CHECK_FATAL(mustDefList != nullptr, "CreateRealOcc: mustDefList cannot be empty");
      CHECK_FATAL(!mustDefList->empty(), "CreateRealOcc: mustDefList cannot be empty");
      wkCand->SetTheVar(*static_cast<VarMeExpr*>(mustDefList->front().GetLHS()));
    }
  }
  SRealOcc *newOcc = spreMp->New<SRealOcc>(meStmt);
  wkCand->GetRealOccs().push_back(newOcc);
  wkCand->SetHasStoreOcc(true);
}

// create a new use occurrence for symbol oidx in given bb
void MeStorePre::CreateUseOcc(OStIdx ostIdx, BB &bb) const {
  SpreWorkCand *wkCand = nullptr;
  auto mapIt = workCandMap.find(ostIdx);
  if (mapIt == workCandMap.end()) {
    return;
  }
  wkCand = mapIt->second;
  CHECK_FATAL(!wkCand->GetRealOccs().empty(), "empty container check");
  SOcc *lastOcc = wkCand->GetRealOccs().back();
  if (lastOcc->GetOccTy() == kSOccUse && &lastOcc->GetBB() == &bb) {
    return;  // no need to push consecutive use occurrences at same BB
  }
  SUseOcc *newOcc = spreMp->New<SUseOcc>(bb);
  wkCand->GetRealOccs().push_back(newOcc);
}

// create use occurs for all the symbols that alias with muost
void MeStorePre::CreateSpreUseOccsThruAliasing(const OriginalSt &muOst, BB &bb) const {
  if (muOst.GetIndex() >= aliasClass->GetAliasElemCount()) {
    return;
  }
  AliasElem *ae = aliasClass->FindAliasElem(muOst);
  if (ae->GetClassSet() == nullptr) {
    return;
  }
  for (auto setIt = ae->GetClassSet()->begin(); setIt != ae->GetClassSet()->end(); ++setIt) {
    unsigned int elemId = *setIt;
    AliasElem *ae0 = aliasClass->FindID2Elem(elemId);
    if (ae0->GetOriginalSt().GetIndirectLev() == 0) {
      CreateUseOcc(ae0->GetOriginalSt().GetIndex(), bb);
    }
  }
}

void MeStorePre::FindAndCreateSpreUseOccs(const MeExpr &meExpr, BB &bb) const {
  if (meExpr.GetMeOp() == kMeOpVar) {
    auto *var = static_cast<const VarMeExpr*>(&meExpr);
    const OriginalSt *ost = ssaTab->GetOriginalStFromID(var->GetOStIdx());
    if (!ost->IsVolatile()) {
      CreateUseOcc(var->GetOStIdx(), bb);
    }
    return;
  }
  for (uint8 i = 0; i < meExpr.GetNumOpnds(); i++) {
    FindAndCreateSpreUseOccs(*meExpr.GetOpnd(i), bb);
  }
  if (IsJavaLang()) {
    return;
  }
  if (meExpr.GetMeOp() == kMeOpIvar) {
    auto *ivarMeExpr = static_cast<const IvarMeExpr*>(&meExpr);
    if (ivarMeExpr->GetMu() != nullptr) {
      CreateSpreUseOccsThruAliasing(*ssaTab->GetOriginalStFromID(ivarMeExpr->GetMu()->GetOStIdx()), bb);
    }
  }
}

void MeStorePre::CreateSpreUseOccsForAll(BB &bb) const {
  // go thru all workcands and insert a use occurrence for each of them
  for (std::pair<OStIdx, SpreWorkCand*> wkCandPair : workCandMap) {
    SpreWorkCand *wkCand = wkCandPair.second;
    CHECK_NULL_FATAL(wkCand);
    CHECK_FATAL(!wkCand->GetRealOccs().empty(), "container empty check");
    SOcc *lastOcc = wkCand->GetRealOccs().back();
    if (lastOcc->GetOccTy() == kSOccUse && &lastOcc->GetBB() == &bb) {
      continue;  // no need to push consecutive use occurrences at same BB
    }
    SUseOcc *newOcc = spreMp->New<SUseOcc>(bb);
    wkCand->GetRealOccs().push_back(newOcc);
  }
}

void MeStorePre::BuildWorkListBB(BB *bb) {
  if (bb == nullptr) {
    return;
  }
  // traverse statements backwards
  auto &meStmts = bb->GetMeStmts();
  for (auto stmt = meStmts.rbegin(); stmt != meStmts.rend(); ++stmt) {
    // look for real occurrence of stores
    OStIdx lhsOstIdx(0);
    if (stmt->GetOp() == OP_dassign) {
      auto *dass = static_cast<DassignMeStmt*>(to_ptr(stmt));
      if (dass->GetLHS()->GetPrimType() != PTY_ref) {
        lhsOstIdx = dass->GetVarLHS()->GetOStIdx();
      }
    } else if (kOpcodeInfo.IsCallAssigned(stmt->GetOp())) {
      MapleVector<MustDefMeNode> *mustDefList = stmt->GetMustDefList();
      CHECK_NULL_FATAL(mustDefList);
      if (!mustDefList->empty()) {
        MeExpr *mdLHS = mustDefList->front().GetLHS();
        if (mdLHS->GetMeOp() == kMeOpVar && mdLHS->GetPrimType() != PTY_ref) {
          lhsOstIdx = static_cast<VarMeExpr*>(mdLHS)->GetOStIdx();
        }
      }
    }
    if (lhsOstIdx != 0u) {
      const OriginalSt *ost = ssaTab->GetOriginalStFromID(lhsOstIdx);
      if (!ost->IsVolatile()) {
        CreateRealOcc(lhsOstIdx, *to_ptr(stmt));
      }
    }
    // look for use occurrence of stores
    for (size_t i = 0; i < stmt->NumMeStmtOpnds(); i++) {
      FindAndCreateSpreUseOccs(*stmt->GetOpnd(i), *stmt->GetBB());
    }
    if (!IsJavaLang()) {
      // go thru mu list
      NaryMeStmt *naryMeStmt = safe_cast<NaryMeStmt>(to_ptr(stmt));
      if (naryMeStmt != nullptr) {
        CHECK_NULL_FATAL(naryMeStmt->GetMuList());
        for (std::pair<OStIdx, VarMeExpr*> muPair : *(naryMeStmt->GetMuList())) {
          CreateSpreUseOccsThruAliasing(*ssaTab->GetOriginalStFromID(muPair.second->GetOStIdx()), *bb);
        }
      }
    }
  }
  if (bb->GetAttributes(kBBAttrIsCatch)) {
    CreateSpreUseOccsForAll(*bb);
  }
  if (bb->GetAttributes(kBBAttrIsEntry)) {
    CreateEntryOcc(*bb);
  }
  // recurse on child BBs in post-dominator tree
  for (BBId bbId : dom->GetPdomChildrenItem(bb->GetBBId())) {
    BuildWorkListBB(func->GetAllBBs().at(bbId));
  }
}

AnalysisResult *MeDoStorePre::Run(MeFunction *func, MeFuncResultMgr *m, ModuleResultMgr*) {
  auto *dom = static_cast<Dominance*>(m->GetAnalysisResult(MeFuncPhase_DOMINANCE, func));
  ASSERT(dom != nullptr, "dominance phase has problem");
  auto *aliasClass = static_cast<AliasClass*>(m->GetAnalysisResult(MeFuncPhase_ALIASCLASS, func));
  ASSERT(aliasClass != nullptr, "aliasClass phase has problem");
  auto *meIrMap = static_cast<MeIRMap*>(m->GetAnalysisResult(MeFuncPhase_IRMAP, func));
  CHECK_FATAL(meIrMap != nullptr, "irmap phase has problem");
  MeStorePre storePre(*func, *dom, *aliasClass, *NewMemPool(), DEBUGFUNC(func));
  storePre.ApplySSUPre();
  if (DEBUGFUNC(func)) {
    func->Dump(false);
  }
  return nullptr;
}
}  // namespace maple
