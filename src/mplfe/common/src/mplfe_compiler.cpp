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
#include "mplfe_compiler.h"
#include <sstream>
#include "fe_manager.h"
#include "fe_file_type.h"
#include "fe_timer.h"

namespace maple {
MPLFECompiler::MPLFECompiler(MIRModule &argModule)
    : module(argModule),
      mp(memPoolCtrler.NewMemPool("MemPool for MPLFECompiler")),
      allocator(mp) {}

MPLFECompiler::~MPLFECompiler() {
  mp = nullptr;
}

void MPLFECompiler::Init() {
  FEManager::Init(module);
  FEStructMethodInfo::InitJavaPolymorphicWhiteList();
}

void MPLFECompiler::Release() {
  FEManager::Release();
  memPoolCtrler.DeleteMemPool(mp);
  mp = nullptr;
}

void MPLFECompiler::CheckInput() {
  FE_INFO_LEVEL(FEOptions::kDumpLevelInfo, "===== Process MPLFECompiler::CheckInput() =====");
  size_t nInput = 0;

  // check input class files
  const std::list<std::string> &inputClassNames = FEOptions::GetInstance().GetInputClassFiles();
  if (!inputClassNames.empty()) {
    nInput += inputClassNames.size();
    if (firstInputName.empty()) {
      firstInputName = inputClassNames.front();
    }
  }

  // check input jar files
  const std::list<std::string> &inputJarNames = FEOptions::GetInstance().GetInputJarFiles();
  if (!inputJarNames.empty()) {
    nInput += inputJarNames.size();
    if (firstInputName.empty()) {
      firstInputName = inputJarNames.front();
    }
  }

  CHECK_FATAL(nInput > 0, "Error occurs: no inputs. exit.");
}

void MPLFECompiler::SetupOutputPathAndName() {
  FE_INFO_LEVEL(FEOptions::kDumpLevelInfo, "===== Process MPLFECompiler::SetupOutputPathAndName() =====");
  // get outputName from option
  const std::string &outputName0 = FEOptions::GetInstance().GetOutputName();
  if (!outputName0.empty()) {
    outputName = outputName0;
  } else {
    // use default
    outputName = FEFileType::GetName(firstInputName, false);
    outputPath = FEFileType::GetPath(firstInputName);
  }
  const std::string &outputPath0 = FEOptions::GetInstance().GetOutputPath();
  if (!outputPath0.empty()) {
    outputPath = outputPath0;
  }
  FE_INFO_LEVEL(FEOptions::kDumpLevelInfo, "OutputPath: %s", outputPath.c_str());
  FE_INFO_LEVEL(FEOptions::kDumpLevelInfo, "OutputName: %s", outputName.c_str());
  std::string outName = "";
  if (outputPath.empty()) {
    outName = outputName;
  } else {
    outName = outputPath + "/" + outputName;
  }
  FE_INFO_LEVEL(FEOptions::kDumpLevelInfo, "OutputFullName: %s", outName.c_str());
  module.SetFileName(outName);
}

bool MPLFECompiler::LoadMplt() {
  bool success = true;
  FE_INFO_LEVEL(FEOptions::kDumpLevelInfo, "===== Process MPLFECompiler::LoadMplt() =====");
  // load mplt from sys
  const std::list<std::string> &mpltsFromSys = FEOptions::GetInstance().GetInputMpltFilesFromSys();
  success = success && FEManager::GetTypeManager().LoadMplts(mpltsFromSys, FETypeFlag::kSrcMpltSys,
                                                             "Load mplt from sys");

  // load mplt from apk
  const std::list<std::string> &mpltsFromApk = FEOptions::GetInstance().GetInputMpltFilesFromApk();
  success = success && FEManager::GetTypeManager().LoadMplts(mpltsFromApk, FETypeFlag::kSrcMpltApk,
                                                             "Load mplt from apk");

  // load mplt
  const std::list<std::string> &mplts = FEOptions::GetInstance().GetInputMpltFiles();
  success = success && FEManager::GetTypeManager().LoadMplts(mplts, FETypeFlag::kSrcMplt, "Load mplt");
  return success;
}

void MPLFECompiler::ExportMpltFile() {
  FETimer timer;
  timer.StartAndDump("Output mplt");
  module.DumpToHeaderFile(!FEOptions::GetInstance().IsGenAsciiMplt());
  timer.StopAndDumpTimeMS("Output mplt");
}

void MPLFECompiler::ExportMplFile() {
  FETimer timer;
  timer.StartAndDump("Output mpl");
  module.OutputAsciiMpl("", false);
  timer.StopAndDumpTimeMS("Output mpl");
}

void MPLFECompiler::RegisterCompilerComponent(std::unique_ptr<MPLFECompilerComponent> comp) {
  CHECK_FATAL(comp != nullptr, "input compiler component is nullptr");
  components.push_back(std::move(comp));
}

void MPLFECompiler::InitFromOptions() {
  for (const std::unique_ptr<MPLFECompilerComponent> &comp : components) {
    ASSERT(comp != nullptr, "nullptr check");
    bool success = comp->InitFromOptions();
    CHECK_FATAL(success, "Error occurs in MPLFECompiler::InitFromOptions(). exit.");
  }
}

void MPLFECompiler::ParseInputs() {
  FETimer timer;
  timer.StartAndDump("MPLFECompiler::ParseInputs()");
  for (const std::unique_ptr<MPLFECompilerComponent> &comp : components) {
    CHECK_NULL_FATAL(comp);
    bool success = comp->ParseInput();
    CHECK_FATAL(success, "Error occurs in MPLFECompiler::ParseInputs(). exit.");
  }
  timer.StopAndDumpTimeMS("MPLFECompiler::ParseInputs()");
}

void MPLFECompiler::PreProcessDecls() {
  FETimer timer;
  timer.StartAndDump("MPLFECompiler::PreProcessDecls()");
  for (const std::unique_ptr<MPLFECompilerComponent> &comp : components) {
    ASSERT(comp != nullptr, "nullptr check");
    bool success = comp->PreProcessDecl();
    CHECK_FATAL(success, "Error occurs in MPLFECompiler::PreProcessDecls(). exit.");
  }
  timer.StopAndDumpTimeMS("MPLFECompiler::PreProcessDecl()");
}

void MPLFECompiler::ProcessDecls() {
  FETimer timer;
  timer.StartAndDump("MPLFECompiler::ProcessDecl()");
  for (const std::unique_ptr<MPLFECompilerComponent> &comp : components) {
    ASSERT(comp != nullptr, "nullptr check");
    bool success = comp->ProcessDecl();
    CHECK_FATAL(success, "Error occurs in MPLFECompiler::ProcessDecls(). exit.");
  }
  timer.StopAndDumpTimeMS("MPLFECompiler::ProcessDecl()");
}

void MPLFECompiler::PreProcessWithFunctions() {
  FETimer timer;
  timer.StartAndDump("MPLFECompiler::PreProcessWithFunctions()");
  for (const std::unique_ptr<MPLFECompilerComponent> &comp : components) {
    ASSERT(comp != nullptr, "nullptr check");
    bool success = comp->PreProcessWithFunction();
    CHECK_FATAL(success, "Error occurs in MPLFECompiler::PreProcessWithFunctions(). exit.");
  }
  timer.StopAndDumpTimeMS("MPLFECompiler::PreProcessWithFunctions()");
}

void MPLFECompiler::ProcessFunctions() {
  FETimer timer;
  bool success = true;
  timer.StartAndDump("MPLFECompiler::ProcessFunctions()");
  for (const std::unique_ptr<MPLFECompilerComponent> &comp : components) {
    ASSERT(comp != nullptr, "nullptr check");
    uint32 nthreads = FEOptions::GetInstance().GetNThreads();
    if (comp->Parallelable() && nthreads > 0) {
      FEConfigParallel::GetInstance().EnableParallel();
      success = comp->ProcessFunctionParallel(nthreads) && success;
    } else {
      success = comp->ProcessFunctionSerial() && success;
    }
    if (FEOptions::GetInstance().IsDumpPhaseTime()) {
      comp->DumpPhaseTimeTotal();
    }
  }
  timer.StopAndDumpTimeMS("MPLFECompiler::ProcessFunctions()");
  CHECK_FATAL(success, "ProcessFunction error");
}
}  // namespace maple