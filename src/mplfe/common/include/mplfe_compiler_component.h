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
#ifndef MPLFE_INCLUDE_COMMON_MPLFE_COMPILER_COMPONENT_H
#define MPLFE_INCLUDE_COMMON_MPLFE_COMPILER_COMPONENT_H
#include <list>
#include <memory>
#include "mir_module.h"
#include "mplfe_options.h"
#include "fe_function.h"
#include "fe_input.h"
#include "fe_input_helper.h"
#include "mpl_scheduler.h"

namespace maple {
class FEFunctionProcessTask : public MplTask {
 public:
  FEFunctionProcessTask(FEFunction &argFunction);
  virtual ~FEFunctionProcessTask() = default;

 protected:
  int RunImpl(MplTaskParam *param) override;
  int FinishImpl(MplTaskParam *param) override;

 private:
  FEFunction &function;
};

class FEFunctionProcessSchedular : public MplScheduler {
 public:
  FEFunctionProcessSchedular(const std::string &name)
      : MplScheduler(name) {}
  virtual ~FEFunctionProcessSchedular() = default;
  void AddFunctionProcessTask(const std::unique_ptr<FEFunction> &function);
  void SetDumpTime(bool arg) {
    dumpTime = arg;
  }

 private:
  std::list<std::unique_ptr<FEFunctionProcessTask>> tasks;
};

class MPLFECompilerComponent {
 public:
  MPLFECompilerComponent(MIRModule &argModule, MIRSrcLang argSrcLang);
  virtual ~MPLFECompilerComponent() = default;
  bool InitFromOptions() {
    return InitFromOptionsImpl();
  }
  bool ParseInput() {
    return ParseInputImpl();
  }

  bool PreProcessDecl() {
    return PreProcessDeclImpl();
  }

  bool ProcessDecl() {
    return ProcessDeclImpl();
  }

  bool PreProcessWithoutFunction() {
    return PreProcessWithoutFunctionImpl();
  }

  bool PreProcessWithFunction() {
    return PreProcessWithFunctionImpl();
  }

  bool ProcessFunctionSerial() {
    return ProcessFunctionSerialImpl();
  }

  bool ProcessFunctionParallel(uint32 nthreads) {
    return ProcessFunctionParallelImpl(nthreads);
  }

  std::string GetComponentName() const {
    return GetComponentNameImpl();
  }

  bool Parallelable() const {
    return ParallelableImpl();
  }

  void DumpPhaseTimeTotal() const {
    DumpPhaseTimeTotalImpl();
  }

 protected:
  virtual bool InitFromOptionsImpl() = 0;
  virtual bool ParseInputImpl() = 0;
  virtual bool PreProcessDeclImpl() = 0;
  virtual bool ProcessDeclImpl() = 0;
  virtual bool PreProcessWithoutFunctionImpl() = 0;
  virtual bool PreProcessWithFunctionImpl() = 0;
  virtual bool ProcessFunctionSerialImpl();
  virtual bool ProcessFunctionParallelImpl(uint32 nthreads);
  virtual std::string GetComponentNameImpl() const;
  virtual bool ParallelableImpl() const;
  virtual void DumpPhaseTimeTotalImpl() const;

  MIRModule &module;
  MIRSrcLang srcLang;
  std::list<std::unique_ptr<FEInputFieldHelper>> fieldHelpers;
  std::list<std::unique_ptr<FEInputMethodHelper>> methodHelpers;
  std::list<std::unique_ptr<FEInputStructHelper>> structHelpers;
  std::list<std::unique_ptr<FEFunction>> functions;
  std::unique_ptr<FEFunctionPhaseResult> phaseResultTotal;
};
}  // namespace maple
#endif