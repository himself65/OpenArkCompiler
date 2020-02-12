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
#ifndef MAPLE_ME_INCLUDE_MESSAEPRE_H
#define MAPLE_ME_INCLUDE_MESSAEPRE_H

#include "me_function.h"
#include "me_irmap.h"
#include "me_cfg.h"
#include "ssa_epre.h"

namespace maple {
class MeSSAEPre : public SSAEPre {
 public:
  // a symbol is a candidate for ssaupdate if its ostidx key exists in the map;
  // the mapped set gives bbs where dassign's are inserted by ssa_epre for the symbol
  explicit MeSSAEPre(MeFunction *func, IRMap &map, Dominance &dom, MemPool &memPool, MemPool &mp2, uint32 limit,
                     bool includeRef, bool epreLocalRefVar, bool lhsIvar)
      : SSAEPre(map, dom, memPool, mp2, kExprPre, limit, includeRef, lhsIvar),
        candsForSSAUpdate(std::less<OStIdx>(), ssaPreAllocator.Adapter()),
        func(func),
        epreLocalRefVar(epreLocalRefVar) {}

  virtual ~MeSSAEPre() = default;
  void GetIterDomFrontier(BB &bb, MapleSet<uint32> &dfSet, std::vector<bool> &visitedMap) override;
  bool ScreenPhiBB(BBId) const override {
    return true;
  }

  MapleMap<OStIdx, MapleSet<BBId>*> &GetCandsForSSAUpdate() {
    return candsForSSAUpdate;
  }

 protected:
  MapleMap<OStIdx, MapleSet<BBId>*> candsForSSAUpdate;
  MeFunction *func;
  bool epreLocalRefVar;

 private:
  void BuildWorkList() override;
  BB *GetBB(BBId id) override {
    return func->GetBBFromID(id);
  }

  PUIdx GetPUIdx() const override {
    return func->GetMirFunc()->GetPuidx();
  }

  bool CfgHasDoWhile() override {
    return func->GetTheCfg()->GetHasDoWhile();
  }

  bool EpreLocalRefVar() override {
    return epreLocalRefVar;
  }

  void EnterCandsForSSAUpdate(OStIdx ostIdx, BB *bb) override {
    if (candsForSSAUpdate.find(ostIdx) == candsForSSAUpdate.end()) {
      MapleSet<BBId> *bbSet = ssaPreMemPool->New<MapleSet<BBId>>(std::less<BBId>(), ssaPreAllocator.Adapter());
      bbSet->insert(bb->GetBBId());
      candsForSSAUpdate[ostIdx] = bbSet;
    } else {
      candsForSSAUpdate[ostIdx]->insert(bb->GetBBId());
    }
  }
};

class MeDoSSAEPre : public MeFuncPhase {
 public:
  explicit MeDoSSAEPre(MePhaseID id) : MeFuncPhase(id) {}

  virtual ~MeDoSSAEPre() = default;
  AnalysisResult *Run(MeFunction *ir, MeFuncResultMgr *m, ModuleResultMgr *mrm) override;
  std::string PhaseName() const override {
    return "epre";
  }
};
}  // namespace maple
#endif  // MAPLE_ME_INCLUDE_MESSAEPRE_H
