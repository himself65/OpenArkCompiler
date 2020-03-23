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
#ifndef MAPLE_ME_INCLUDE_MECRITICALEDGE_H
#define MAPLE_ME_INCLUDE_MECRITICALEDGE_H

#include "me_phase.h"
#include "bb.h"

namespace maple {
// Split critical edge
class MeDoSplitCEdge : public MeFuncPhase {
 public:
  explicit MeDoSplitCEdge(MePhaseID id) : MeFuncPhase(id) {}

  ~MeDoSplitCEdge() = default;

  AnalysisResult *Run(MeFunction *func, MeFuncResultMgr *m, ModuleResultMgr *mrm) override;
  std::string PhaseName() const override {
    return "splitcriticaledge";
  }

 private:
  void BreakCriticalEdge(MeFunction &func, BB &pred, BB &succ) const;
};
}  // namespace maple
#endif  // MAPLE_ME_INCLUDE_MECRITICALEDGE_H
