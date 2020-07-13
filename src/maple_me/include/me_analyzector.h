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
#ifndef MAPLEME_INCLUDE_ANALYZECTOR_H
#define MAPLEME_INCLUDE_ANALYZECTOR_H

#include "me_function.h"
#include "me_phase.h"
#include "me_irmap.h"
#include "annotation_analysis.h"

namespace maple {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"

class AnalyzeCtor {
 public:
  AnalyzeCtor(MeFunction &func, Dominance &dom, KlassHierarchy &kh) : func(&func), dominance(&dom), klassh(&kh) {}
  virtual ~AnalyzeCtor() = default;

  virtual void ProcessFunc();
  void ProcessStmt(MeStmt &stmt);

 private:
  bool hasSideEffect = false;
  std::unordered_set<FieldID> fieldSet;
  MeFunction *func;
  Dominance *dominance;
  KlassHierarchy *klassh;

};
#pragma clang diagnostic pop

class MeDoAnalyzeCtor : public MeFuncPhase {
 public:
  explicit MeDoAnalyzeCtor(MePhaseID id) : MeFuncPhase(id) {}

  virtual ~MeDoAnalyzeCtor() = default;

  AnalysisResult *Run(MeFunction*, MeFuncResultMgr*, ModuleResultMgr*) override;

  std::string PhaseName() const override {
    return "analyzector";
  }
};
}  // namespace maple
#endif  // MAPLEME_INCLUDE_ANALYZECTOR_H
