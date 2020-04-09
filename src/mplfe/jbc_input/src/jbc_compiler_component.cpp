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
#include "jbc_compiler_component.h"
#include "jbc_class2fe_helper.h"
#include "fe_timer.h"
#include "fe_manager.h"
#include "jbc_function.h"

namespace maple {
JBCCompilerComponent::JBCCompilerComponent(MIRModule &module)
    : MPLFECompilerComponent(module, kSrcLangJava),
      mp(memPoolCtrler.NewMemPool("MemPool for JBCCompilerComponent")),
      allocator(mp),
      jbcInput(module) {}

JBCCompilerComponent::~JBCCompilerComponent() {
  mp = nullptr;
}

void JBCCompilerComponent::ReleaseMemPool() {
  memPoolCtrler.DeleteMemPool(mp);
  mp = nullptr;
}

bool JBCCompilerComponent::InitFromOptionsImpl() {
  return true;
}

bool JBCCompilerComponent::ParseInputImpl() {
  FETimer timer;
  bool success = true;
  timer.StartAndDump("JBCCompilerComponent::ParseInput()");
  FE_INFO_LEVEL(FEOptions::kDumpLevelInfo, "===== Process JBCCompilerComponent::ParseInput() =====");
  const std::list<std::string> &inputClassNames = FEOptions::GetInstance().GetInputClassFiles();
  const std::list<std::string> &inputJarNames = FEOptions::GetInstance().GetInputJarFiles();
  success = success && jbcInput.ReadClassFiles(inputClassNames);
  success = success && jbcInput.ReadJarFiles(inputJarNames);
  CHECK_FATAL(success, "JBCCompilerComponent::ParseInput failed. Exit.");
  const jbc::JBCClass *klass = jbcInput.GetFirstClass();
  while (klass != nullptr) {
    std::unique_ptr<FEInputStructHelper> structHelper = std::make_unique<JBCClass2FEHelper>(allocator, *klass);
    structHelpers.push_back(std::move(structHelper));
    klass = jbcInput.GetNextClass();
  }
  timer.StopAndDumpTimeMS("JBCCompilerComponent::ParseInput()");
  return success;
}

bool JBCCompilerComponent::PreProcessDeclImpl() {
  FETimer timer;
  timer.StartAndDump("JBCCompilerComponent::PreProcessDecl()");
  FE_INFO_LEVEL(FEOptions::kDumpLevelInfo, "===== Process JBCCompilerComponent::PreProcessDecl() =====");
  bool success = true;
  for (const std::unique_ptr<FEInputStructHelper> &helper : structHelpers) {
    ASSERT(helper != nullptr, "nullptr check");
    success = helper->PreProcessDecl() ? success : false;
  }
  timer.StopAndDumpTimeMS("JBCCompilerComponent::PreProcessDecl()");
  return success;
}

bool JBCCompilerComponent::ProcessDeclImpl() {
  FETimer timer;
  timer.StartAndDump("JBCCompilerComponent::ProcessDecl()");
  FE_INFO_LEVEL(FEOptions::kDumpLevelInfo, "===== Process JBCCompilerComponent::ProcessDecl() =====");
  bool success = true;
  for (const std::unique_ptr<FEInputStructHelper> &helper : structHelpers) {
    ASSERT(helper != nullptr, "nullptr check");
    success = helper->ProcessDecl() ? success : false;
  }
  timer.StopAndDumpTimeMS("JBCCompilerComponent::ProcessDecl()");
  return success;
}

bool JBCCompilerComponent::PreProcessWithoutFunctionImpl() {
  return false;
}

bool JBCCompilerComponent::PreProcessWithFunctionImpl() {
  FETimer timer;
  timer.StartAndDump("JBCCompilerComponent::PreProcessWithFunction()");
  FE_INFO_LEVEL(FEOptions::kDumpLevelInfo, "===== Process JBCCompilerComponent::PreProcessWithFunction() =====");
  for (const std::unique_ptr<FEInputStructHelper> &structHelper : structHelpers) {
    ASSERT(structHelper != nullptr, "nullptr check");
    for (FEInputMethodHelper *methodHelper : structHelper->GetMethodHelpers()) {
      ASSERT(methodHelper != nullptr, "nullptr check");
      JBCClassMethod2FEHelper *jbcMethodHelper = static_cast<JBCClassMethod2FEHelper*>(methodHelper);
      GStrIdx methodNameIdx = methodHelper->GetMethodNameIdx();
      bool isStatic = methodHelper->IsStatic();
      MIRFunction *mirFunc = FEManager::GetTypeManager().GetMIRFunction(methodNameIdx, isStatic);
      CHECK_NULL_FATAL(mirFunc);
      std::unique_ptr<FEFunction> feFunction = std::make_unique<JBCFunction>(*jbcMethodHelper, *mirFunc,
                                                                             phaseResultTotal);
      module.AddFunction(mirFunc);
      feFunction->Init();
      feFunction->SetSrcFileName(structHelper->GetSrcFileName());
      functions.push_back(std::move(feFunction));
    }
  }
  timer.StopAndDumpTimeMS("JBCCompilerComponent::PreProcessWithFunction()");
  return true;
}

std::string JBCCompilerComponent::GetComponentNameImpl() const {
  return "JBCCompilerComponent";
}

bool JBCCompilerComponent::ParallelableImpl() const {
  return true;
}

void JBCCompilerComponent::DumpPhaseTimeTotalImpl() const {
  INFO(kLncInfo, "[PhaseTime] JBCCompilerComponent");
  MPLFECompilerComponent::DumpPhaseTimeTotalImpl();
}
}  // namespace maple