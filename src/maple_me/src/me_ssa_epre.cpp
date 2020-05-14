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
#include "me_ssa_epre.h"
#include "me_dominance.h"
#include "me_ssa_update.h"

// accumulate the BBs that are in the iterated dominance frontiers of bb in
// the set dfSet, visiting each BB only once
namespace maple {
void MeSSAEPre::GetIterDomFrontier(const BB &bb, MapleSet<uint32> &dfSet, std::vector<bool> &visitedMap) const {
  CHECK_FATAL(bb.GetBBId() < visitedMap.size(), "index out of range in MeSSAEPre::GetIterDomFrontier");
  if (visitedMap[bb.GetBBId()]) {
    return;
  }
  visitedMap[bb.GetBBId()] = true;
  for (BBId frontierBBId : dom->GetDomFrontier(bb.GetBBId())) {
    (void)dfSet.insert(dom->GetDtDfnItem(frontierBBId));
    BB *frontierBB = GetBB(frontierBBId);
    GetIterDomFrontier(*frontierBB, dfSet, visitedMap);
  }
}

void MeSSAEPre::BuildWorkList() {
  const MapleVector<BBId> &preOrderDt = dom->GetDtPreOrder();
  for (auto &bbID : preOrderDt) {
    BB *bb = func->GetAllBBs().at(bbID);
    BuildWorkListBB(bb);
  }
}

bool MeSSAEPre::IsThreadObjField(const IvarMeExpr &expr) const {
  if (expr.GetFieldID() == 0) {
    return false;
  }
  auto *type = static_cast<MIRPtrType*>(GlobalTables::GetTypeTable().GetTypeFromTyIdx(expr.GetTyIdx()));
  TyIdx runnableInterface = klassHierarchy.GetKlassFromLiteral("Ljava_2Flang_2FRunnable_3B")->GetTypeIdx();
  Klass *klass = klassHierarchy.GetKlassFromTyIdx(type->GetPointedTyIdx());
  if (klass == nullptr) {
    return false;
  }
  for (Klass *inter : klass->GetImplInterfaces()) {
    if (inter->GetTypeIdx() == runnableInterface) {
      return true;
    }
  }
  return false;
}

AnalysisResult *MeDoSSAEPre::Run(MeFunction *func, MeFuncResultMgr *m, ModuleResultMgr *mrm) {
  static uint32 puCount = 0;  // count PU to support the eprePULimit option
  if (puCount > MeOption::eprePULimit) {
    ++puCount;
    return nullptr;
  }
  auto *dom = static_cast<Dominance*>(m->GetAnalysisResult(MeFuncPhase_DOMINANCE, func));
  ASSERT(dom != nullptr, "dominance phase has problem");
  auto *irMap = static_cast<MeIRMap*>(m->GetAnalysisResult(MeFuncPhase_IRMAP, func));
  ASSERT(irMap != nullptr, "irMap phase has problem");
  KlassHierarchy *kh = static_cast<KlassHierarchy*>(mrm->GetAnalysisResult(MoPhase_CHA, &func->GetMIRModule()));
  CHECK_FATAL(kh != nullptr, "KlassHierarchy phase has problem");
  bool eprePULimitSpecified = MeOption::eprePULimit != UINT32_MAX;
  uint32 epreLimitUsed =
      (eprePULimitSpecified && puCount != MeOption::eprePULimit) ? UINT32_MAX : MeOption::epreLimit;
  MemPool *ssaPreMemPool = NewMemPool();
  bool epreIncludeRef = MeOption::epreIncludeRef;
  MeSSAEPre ssaPre(*func, *irMap, *dom, *kh, *ssaPreMemPool, *NewMemPool(), epreLimitUsed, epreIncludeRef,
                   MeOption::epreLocalRefVar, MeOption::epreLHSIvar);
  ssaPre.SetSpillAtCatch(MeOption::spillAtCatch);
  if (eprePULimitSpecified && puCount == MeOption::eprePULimit && epreLimitUsed != UINT32_MAX) {
    LogInfo::MapleLogger() << "applying EPRE limit " << epreLimitUsed << " in function " <<
        func->GetMirFunc()->GetName() << "\n";
  }
  if (DEBUGFUNC(func)) {
    ssaPre.SetSSAPreDebug(true);
  }
  ssaPre.ApplySSAPRE();
  if (!ssaPre.GetCandsForSSAUpdate().empty()) {
    MemPool *tmp = memPoolCtrler.NewMemPool("MeSSAUpdate");
    MeSSAUpdate ssaUpdate(*func, *func->GetMeSSATab(), *dom, ssaPre.GetCandsForSSAUpdate(), *tmp);
    ssaUpdate.Run();
  }
  if (DEBUGFUNC(func)) {
    LogInfo::MapleLogger() << "\n============== EPRE =============" << "\n";
    func->Dump(false);
  }
  ++puCount;
  return nullptr;
}
}  // namespace maple
