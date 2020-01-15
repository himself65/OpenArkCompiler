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

namespace maple {
// convert loop to do-while format
class MeDoLoopCanon : public MeFuncPhase {
 public:
  MeDoLoopCanon(MePhaseID id) : MeFuncPhase(id) {}

  ~MeDoLoopCanon() = default;

  AnalysisResult *Run(MeFunction *func, MeFuncResultMgr *m, ModuleResultMgr *mrm) override;
  std::string PhaseName() const override {
    return "loopcanon";
  }

 private:
  using Key = std::pair<BB*, BB*>;
  void Convert(MeFunction *func, BB *bb, BB *pred, MapleMap<Key, bool> &swapSuccs);
  bool NeedConvert(BB *bb, BB *pred, MapleAllocator &alloc, MapleMap<Key, bool> &swapSuccs);
};
}  // namespace maple
#endif  // MAPLE_ME_INCLUDE_MELOOPCANON_H
