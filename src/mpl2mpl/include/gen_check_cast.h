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
#ifndef MPL2MPL_INCLUDE_GEN_CHECK_CAST_H
#define MPL2MPL_INCLUDE_GEN_CHECK_CAST_H
#include "class_hierarchy.h"
#include "module_phase.h"
#include "phase_impl.h"

namespace maple {

class CheckCastGenerator : public FuncOptimizeImpl {
 public:
  CheckCastGenerator(MIRModule *mod, KlassHierarchy *kh, bool dump);
  ~CheckCastGenerator() {}

  FuncOptimizeImpl *Clone() override {
    return new CheckCastGenerator(*this);
  }

  void ProcessFunc(MIRFunction *func) override;

 private:
  void InitTypes();
  void InitFuncs();
  void GenAllCheckCast();
  void GenCheckCast(StmtNode &stmt);
  BaseNode *GetObjectShadow(BaseNode *opnd);
  MIRSymbol *GetOrCreateClassInfoSymbol(const std::string &className);
  MIRType *pointerObjType = nullptr;
  MIRType *pointerClassMetaType = nullptr;
  MIRType *classinfoType = nullptr;
  MIRFunction *throwCastException = nullptr;
  MIRFunction *checkCastingNoArray = nullptr;
  MIRFunction *checkCastingArray = nullptr;
};

class DoCheckCastGeneration : public ModulePhase {
 public:
  explicit DoCheckCastGeneration(ModulePhaseID id) : ModulePhase(id) {}

  ~DoCheckCastGeneration() = default;

  std::string PhaseName() const override {
    return "gencheckcast";
  }

  AnalysisResult *Run(MIRModule *mod, ModuleResultMgr *mrm) override {
    OPT_TEMPLATE(CheckCastGenerator);
    return nullptr;
  }
};

}  // namespace maple
#endif  // MPL2MPL_INCLUDE_GEN_CHECK_CAST_H
