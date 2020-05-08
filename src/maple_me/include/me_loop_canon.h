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
#ifndef MAPLE_ME_INCLUDE_MELOOPCANON_H
#define MAPLE_ME_INCLUDE_MELOOPCANON_H
#include "me_function.h"
#include "me_phase.h"
#include "me_loop_analysis.h"

namespace maple {
// convert loop to do-while format
class MeDoLoopCanon : public MeFuncPhase {
 public:
  explicit MeDoLoopCanon(MePhaseID id) : MeFuncPhase(id) {}

  ~MeDoLoopCanon() = default;

  AnalysisResult *Run(MeFunction *func, MeFuncResultMgr *m, ModuleResultMgr*) override;
  std::string PhaseName() const override {
    return "loopcanon";
  }

 private:
  using Key = std::pair<BB*, BB*>;
  std::map<BB*, std::vector<BB*>> heads;
  void Convert(MeFunction &func, BB &bb, BB &pred, MapleMap<Key, bool> &swapSuccs);
  bool NeedConvert(BB &bb, BB &pred, MapleAllocator &alloc, MapleMap<Key, bool> &swapSuccs) const;
  void FindHeadBBs(MeFunction &func, Dominance &dom, BB *bb);
  bool IsDoWhileLoop(const LoopDesc &loop) const;
  void Merge(MeFunction &func);
  void AddPreheader(MeFunction &func);
  void InsertNewExitBB(MeFunction &func, LoopDesc &loop);
  void InsertExitBB(MeFunction &func, LoopDesc &loop);
  void SplitCondGotBB(MeFunction &func, LoopDesc &loop);
  void ExecuteLoopCanon(MeFunction &func, MeFuncResultMgr &m, Dominance &dom);
  void ExecuteLoopNormalization(MeFunction &func,  MeFuncResultMgr *m, Dominance &dom);
};
}  // namespace maple
#endif  // MAPLE_ME_INCLUDE_MELOOPCANON_H
