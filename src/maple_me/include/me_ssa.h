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
  MeSSA(MeFunction &func, Dominance &dom, MemPool &memPool, bool enabledDebug);
  ~MeSSA() = default;

  void BuildSSA();
  bool VerifySSA() const;

 private:
  bool VerifySSAOpnd(const BaseNode &node) const;
  void CollectDefBBs(std::map<OStIdx, std::set<BBId>> &ostDefBBs);
  void InsertPhiNode();
  void RenameBB(BB&);
  MeFunction *func;
  Dominance *dom;
  bool enabledDebug;
};

class MeDoSSA : public MeFuncPhase {
 public:
  explicit MeDoSSA(MePhaseID id) : MeFuncPhase(id) {}

  ~MeDoSSA() = default;

 private:
  AnalysisResult *Run(MeFunction *func, MeFuncResultMgr *funcResMgr, ModuleResultMgr *moduleResMgr) override;
  std::string PhaseName() const override {
    return "ssa";
  }
};
}  // namespace maple
#endif  // MAPLE_ME_INCLUDE_ME_SSA_H
