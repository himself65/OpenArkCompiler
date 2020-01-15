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
#ifndef MAPLE_ME_INCLUDE_MEDSE_H
#define MAPLE_ME_INCLUDE_MEDSE_H
#include <iostream>
#include "bb.h"
#include "me_phase.h"
#include "me_option.h"
#include "dominance.h"
#include "me_function.h"
#include "dse.h"
#include "me_cfg.h"

namespace maple {
class MeDSE : public DSE {
 public:
  MeDSE(MeFunction *f, Dominance *dom, bool enabledDebug)
      : DSE(f->GetMIRModule(), std::vector<BB*>(f->GetAllBBs().begin(), f->GetAllBBs().end()),
            *f->GetCommonEntryBB(), *f->GetCommonExitBB(), *f->GetMeSSATab(),
            *dom, enabledDebug),
        func(*f) {}
  virtual ~MeDSE() = default;

  void RunDSE();

 private:
  MeFunction &func;
  void VerifyPhi() const;
};

class MeDoDSE : public MeFuncPhase {
 public:
  MeDoDSE(MePhaseID id) : MeFuncPhase(id) {}

  virtual ~MeDoDSE() = default;
  AnalysisResult *Run(MeFunction *ir, MeFuncResultMgr *m, ModuleResultMgr *mrm) override;
  std::string PhaseName() const override {
    return "dse";
  }
};
}  // namespace maple
#endif  // MAPLE_ME_INCLUDE_MEDSE_H
