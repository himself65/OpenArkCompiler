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
#ifndef MPL2MPL_INCLUDE_VTABLE_IMPL_H
#define MPL2MPL_INCLUDE_VTABLE_IMPL_H
#include "module_phase.h"
#include "phase_impl.h"

namespace maple {
enum CallKind {
  kStaticCall = 0,
  kVirtualCall = 1,
  kSuperCall = 2
};


class VtableImpl : public FuncOptimizeImpl {
 public:
  VtableImpl(MIRModule *mod, KlassHierarchy *kh, bool dump);
  ~VtableImpl() = default;

  void ProcessFunc(MIRFunction *func) override;
  FuncOptimizeImpl *Clone() override {
    return new VtableImpl(*this);
  }

 private:
  void ReplaceResolveInterface(StmtNode &stmt, const ResolveFuncNode &resolveNode);
  MIRModule *mirModule;
  MIRFunction *mccItabFunc;
};

class DoVtableImpl : public ModulePhase {
 public:
  explicit DoVtableImpl(ModulePhaseID id) : ModulePhase(id) {}

  std::string PhaseName() const override {
    return "VtableImpl";
  }

  ~DoVtableImpl() = default;

  AnalysisResult *Run(MIRModule *mod, ModuleResultMgr *mrm) override {
    OPT_TEMPLATE(VtableImpl);
    return nullptr;
  }
};
}  // namespace maple
#endif  // MPL2MPL_INCLUDE_VTABLE_IMPL_H
