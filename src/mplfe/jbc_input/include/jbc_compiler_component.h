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
#ifndef MPLFE_INCLUDE_JBC_COMPILER_COMPONENT_H
#define MPLFE_INCLUDE_JBC_COMPILER_COMPONENT_H
#include "fe_macros.h"
#include "mplfe_compiler_component.h"
#include "jbc_input.h"
#include "fe_function_phase_result.h"

namespace maple {
class JBCCompilerComponent : public MPLFECompilerComponent {
 public:
  JBCCompilerComponent(MIRModule &module);
  ~JBCCompilerComponent();
  void ReleaseMemPool();

 protected:
  bool InitFromOptionsImpl() override;
  bool ParseInputImpl() override;
  bool PreProcessDeclImpl() override;
  bool ProcessDeclImpl() override;
  bool PreProcessWithoutFunctionImpl() override;
  bool PreProcessWithFunctionImpl() override;
  std::string GetComponentNameImpl() const override;
  bool ParallelableImpl() const override;
  void DumpPhaseTimeTotalImpl() const override;

 private:
  MemPool *mp;
  MapleAllocator allocator;
  jbc::JBCInput jbcInput;
};  // class JBCCompilerComponent
}  // namespace maple
#endif  // MPLFE_INCLUDE_JBC_COMPILER_COMPONENT_H