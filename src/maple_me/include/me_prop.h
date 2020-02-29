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
#ifndef MAPLE_ME_INCLUDE_MEPROP_H
#define MAPLE_ME_INCLUDE_MEPROP_H
#include "me_irmap.h"
#include "bb.h"
#include "me_phase.h"
#include "prop.h"

namespace maple {
class MeProp : public Prop {
 public:
  MeProp(MeIRMap &irMap, Dominance &dom, MemPool &memPool, const PropConfig &config)
      : Prop(irMap, dom, memPool, std::vector<BB*>(irMap.GetFunc().GetAllBBs().begin(),
                                                  irMap.GetFunc().GetAllBBs().end()),
             *irMap.GetFunc().GetCommonEntryBB(), config) {}

  virtual ~MeProp() = default;
};

class MeDoMeProp : public MeFuncPhase {
 public:
  explicit MeDoMeProp(MePhaseID id) : MeFuncPhase(id) {}

  virtual ~MeDoMeProp() = default;
  AnalysisResult *Run(MeFunction *func, MeFuncResultMgr *m, ModuleResultMgr *mrm) override;
  std::string PhaseName() const override {
    return "hprop";
  }
};
}  // namespace maple
#endif  // MAPLE_ME_INCLUDE_MEPROP_H
