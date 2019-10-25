/*
 * Copyright (c) [2019] Huawei Technologies Co.,Ltd.All rights reserved.
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
#ifndef MAPLE_ME_INCLUDE_ME_SSA_H
#define MAPLE_ME_INCLUDE_ME_SSA_H
#include <iostream>
#include "mir_module.h"
#include "mir_nodes.h"
#include "me_phase.h"
#include "ssa.h"
#include "bb.h"
#include "dominance.h"

namespace maple {
class MeSSA : public SSA, public AnalysisResult {
 public:
  MeSSA(MeFunction *func, Dominance *dom, MemPool *memPool);
  ~MeSSA() = default;

  void CollectDefBBs(std::map<OStIdx, std::set<BBId>> &ostDefBBs);
  void BuildSSA();
  void InsertPhiNode();
  void RenameBB(BB&);
  bool VerifySSA() const;
  std::string PhaseName() const {
    return "ssa";
  }

 private:
  bool VerifySSAOpnd(const BaseNode &node) const;
  MeFunction *func;
  Dominance *dom;
};

class MeDoSSA : public MeFuncPhase {
 public:
  explicit MeDoSSA(MePhaseID id) : MeFuncPhase(id) {}

  ~MeDoSSA() = default;

  AnalysisResult *Run(MeFunction *ir, MeFuncResultMgr *m, ModuleResultMgr *mrm) override;
  std::string PhaseName() const override {
    return "ssa";
  }
};
}  // namespace maple
#endif  // MAPLE_ME_INCLUDE_ME_SSA_H
