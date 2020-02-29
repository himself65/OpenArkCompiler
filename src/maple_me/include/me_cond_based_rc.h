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
#ifndef MAPLE_ME_INCLUDE_MECONDBASEDRC_H
#define MAPLE_ME_INCLUDE_MECONDBASEDRC_H
#include "me_cond_based.h"

namespace maple {
class CondBasedRC : public MeCondBased {
 public:
  CondBasedRC(MeFunction *f, Dominance *dom) : MeCondBased(f, dom) {}

  virtual ~CondBasedRC() = default;
};

class MeDoCondBasedRC : public MeFuncPhase {
 public:
  explicit MeDoCondBasedRC(MePhaseID id) : MeFuncPhase(id) {}

  virtual ~MeDoCondBasedRC() = default;
  AnalysisResult *Run(MeFunction *func, MeFuncResultMgr *m, ModuleResultMgr *mrm) override;
  std::string PhaseName() const override {
    return "condbasedrc";
  }
};
}  // namespace maple
#endif  // MAPLE_ME_INCLUDE_MECONDBASEDRC_H
