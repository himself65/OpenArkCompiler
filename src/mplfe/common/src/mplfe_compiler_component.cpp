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
#include "mplfe_compiler_component.h"
#include <sstream>
#include "fe_macros.h"
#include "fe_timer.h"
#include "fe_config_parallel.h"

namespace maple {
// ---------- FEFunctionProcessTask ----------
FEFunctionProcessTask::FEFunctionProcessTask(FEFunction &argFunction)
    : function(argFunction) {}

int FEFunctionProcessTask::RunImpl(MplTaskParam *param) {
  function.Process();
  return 0;
}

int FEFunctionProcessTask::FinishImpl(MplTaskParam *param) {
  function.Finish();
  return 0;
}

// ---------- FEFunctionProcessSchedular ----------
void FEFunctionProcessSchedular::AddFunctionProcessTask(const std::unique_ptr<FEFunction> &function) {
  FEFunction *ptrFunc = function.get();
  CHECK_NULL_FATAL(ptrFunc);
  std::unique_ptr<FEFunctionProcessTask> task = std::make_unique<FEFunctionProcessTask>(*ptrFunc);
  AddTask(task.get());
  tasks.push_back(std::move(task));
}

void FEFunctionProcessSchedular::CallbackThreadMainStart() {
  std::thread::id tid = std::this_thread::get_id();
  if (FEOptions::GetInstance().GetDumpLevel() >= FEOptions::kDumpLevelInfoDebug) {
    INFO(kLncInfo, "Start Run Thread (tid=%lx)", tid);
  }
  FEConfigParallel::GetInstance().RegisterRunThreadID(tid);
}

// ---------- MPLFECompilerComponent ----------
MPLFECompilerComponent::MPLFECompilerComponent(MIRModule &argModule, MIRSrcLang argSrcLang)
    : module(argModule),
      srcLang(argSrcLang),
      phaseResultTotal(std::make_unique<FEFunctionPhaseResult>(true)) {}

bool MPLFECompilerComponent::ProcessFunctionSerialImpl() {
  std::stringstream ss;
  ss << GetComponentName() << "::ProcessFunctionSerial()";
  FETimer timer;
  timer.StartAndDump(ss.str());
  FE_INFO_LEVEL(FEOptions::kDumpLevelInfo, "===== Process %s =====", ss.str().c_str());
  for (const std::unique_ptr<FEFunction> &function : functions) {
    ASSERT(function != nullptr, "nullptr check");
    function->Process();
    function->Finish();
  }
  timer.StopAndDumpTimeMS(ss.str());
  return true;
}

bool MPLFECompilerComponent::ProcessFunctionParallelImpl(uint32 nthreads) {
  std::stringstream ss;
  ss << GetComponentName() << "::ProcessFunctionParallel()";
  FETimer timer;
  timer.StartAndDump(ss.str());
  FE_INFO_LEVEL(FEOptions::kDumpLevelInfo, "===== Process %s =====", ss.str().c_str());
  FEFunctionProcessSchedular schedular(ss.str());
  for (const std::unique_ptr<FEFunction> &function : functions) {
    schedular.AddFunctionProcessTask(function);
  }
  schedular.SetDumpTime(FEOptions::GetInstance().IsDumpThreadTime());
  (void)schedular.RunTask(nthreads, true);
  timer.StopAndDumpTimeMS(ss.str());
  return true;
}

std::string MPLFECompilerComponent::GetComponentNameImpl() const {
  return "MPLFECompilerComponent";
}

bool MPLFECompilerComponent::ParallelableImpl() const {
  return false;
}

void MPLFECompilerComponent::DumpPhaseTimeTotalImpl() const {
  CHECK_NULL_FATAL(phaseResultTotal);
  phaseResultTotal->DumpMS();
}
}  // namespace maple