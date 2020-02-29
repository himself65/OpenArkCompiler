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
#ifndef MAPLE_ME_INCLUDE_MERENAME2PREG_H
#define MAPLE_ME_INCLUDE_MERENAME2PREG_H
#include "me_phase.h"
#include "me_function.h"

namespace maple {
class MeDoSSARename2Preg : public MeFuncPhase {
 public:
  explicit MeDoSSARename2Preg(MePhaseID id) : MeFuncPhase(id) {}

  virtual ~MeDoSSARename2Preg() = default;
  AnalysisResult *Run(MeFunction *func, MeFuncResultMgr *funcRst, ModuleResultMgr*) override;
  std::string PhaseName() const override;
};
}  // namespace maple
#endif  // MAPLE_ME_INCLUDE_MERENAME2PREG_H
