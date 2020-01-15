/*
 * Copyright (c) [2019-2020] Huawei Technologies Co.,Ltd.All rights reserved.
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
#ifndef MAPLE_ME_INCLUDE_ME_BB_LAYOUT_H
#define MAPLE_ME_INCLUDE_ME_BB_LAYOUT_H
#include "me_function.h"
#include "me_phase.h"

namespace maple {
class BBLayout : public AnalysisResult {
 public:
  BBLayout(MemPool &memPool, MeFunction &f, bool enabledDebug)
      : AnalysisResult(&memPool),
        func(f),
        layoutAlloc(&memPool),
        layoutBBs(layoutAlloc.Adapter()),
        laidOut(func.GetAllBBs().size(), false, layoutAlloc.Adapter()),
        enabledDebug(enabledDebug) {
    laidOut[func.GetCommonEntryBB()->GetBBId()] = true;
    laidOut[func.GetCommonExitBB()->GetBBId()] = true;
  }

  virtual ~BBLayout() = default;
  BB *NextBB() {
    // return the next BB following strictly program input order
    ++curBBId;
    while (curBBId < func.GetAllBBs().size()) {
      BB *nextBB = func.GetBBFromID(curBBId);
      if (nextBB != nullptr && !laidOut[nextBB->GetBBId()]) {
        return nextBB;
      }
      ++curBBId;
    }
    return nullptr;
  }

  void OptimizeBranchTarget(BB &bb);
  bool BBEmptyAndFallthru(const BB &bb);
  bool BBContainsOnlyGoto(const BB &bb) const;
  bool BBContainsOnlyCondGoto(const BB &bb) const;
  bool HasSameBranchCond(BB &bb1, BB &bb2) const;
  bool BBCanBeMoved(const BB &fromBB, const BB &toAfterBB) const;
  void AddBB(BB &bb);
  BB *GetFallThruBBSkippingEmpty(BB &bb);
  void ResolveUnconditionalFallThru(BB &bb, BB &nextBB);
  void ChangeToFallthruFromGoto(BB &bb);
  const MapleVector<BB*> &GetBBs() const {
    return layoutBBs;
  }

  bool IsNewBBInLayout() const {
    return bbCreated;
  }

  void SetNewBBInLayout() {
    bbCreated = true;
  }

  bool GetTryOutstanding() const {
    return tryOutstanding;
  }

  void SetTryOutstanding(bool val) {
    tryOutstanding = val;
  }

  bool IsBBLaidOut(BBId bbid) const {
    return laidOut.at(bbid);
  }

  void AddLaidOut(bool val) {
    return laidOut.push_back(val);
  }

 private:
  MeFunction &func;
  MapleAllocator layoutAlloc;
  MapleVector<BB*> layoutBBs;  // gives the determined layout order
  BBId curBBId { 0 };          // to index into func.bb_vec_ to return the next BB
  bool bbCreated = false;      // new create bb will change mefunction::bb_vec_ and
  // related analysis result
  MapleVector<bool> laidOut;   // indexed by bbid to tell if has been laid out
  bool tryOutstanding = false; // true if a try laid out but not its endtry
  bool enabledDebug;
};

class MeDoBBLayout : public MeFuncPhase {
 public:
  explicit MeDoBBLayout(MePhaseID id) : MeFuncPhase(id) {}

  virtual ~MeDoBBLayout() = default;
  AnalysisResult *Run(MeFunction *func, MeFuncResultMgr *funcResMgr, ModuleResultMgr *moduleResMgr) override;
  std::string PhaseName() const override {
    return "bblayout";
  }
};
}  // namespace maple
#endif  // MAPLE_ME_INCLUDE_ME_BB_LAYOUT_H
