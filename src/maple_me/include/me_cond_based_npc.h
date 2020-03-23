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
#ifndef MAPLE_ME_INCLUDE_MECONDBASEDNPC_H
#define MAPLE_ME_INCLUDE_MECONDBASEDNPC_H
#include "me_cond_based.h"

namespace maple {
class CondBasedNPC : public MeCondBased {
 public:
  CondBasedNPC(MeFunction *func, Dominance *dom) : MeCondBased(func, dom) {}

  ~CondBasedNPC() = default;
  void DoCondBasedNPC() const;
};

class MeDoCondBasedNPC : public MeFuncPhase {
 public:
  explicit MeDoCondBasedNPC(MePhaseID id) : MeFuncPhase(id) {}

  virtual ~MeDoCondBasedNPC() = default;
  AnalysisResult *Run(MeFunction *func, MeFuncResultMgr *m, ModuleResultMgr *mrm) override;
  std::string PhaseName() const override {
    return "condbasednpc";
  }
};
}  // namespace maple
#endif  // MAPLE_ME_INCLUDE_MECONDBASEDNPC_H
