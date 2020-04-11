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
#include "mpl_logging.h"
#include "mplfe_options.h"
#include "mplfe_compiler.h"
#include "jbc_compiler_component.h"
#include "fe_errno.h"
#include "mpl_timer.h"
#include "mplfe_env.h"
#include "fe_manager.h"
using namespace maple;

int main(int argc, char **argv) {
  MPLTimer timer;
  timer.Start();
  MPLFEOptions &options = MPLFEOptions::GetInstance();
  if (options.SolveArgs(argc, argv) == false) {
    return static_cast<int>(FEErrno::kCmdParseError);
  }
  MPLFEEnv::GetInstance().Init();
  MIRModule module;
  MPLFECompiler compiler(module);
  bool success = true;
  compiler.Init();
  compiler.CheckInput();
  compiler.SetupOutputPathAndName();
  success = success && compiler.LoadMplt();
  std::unique_ptr<MPLFECompilerComponent> jbcCompilerComp = std::make_unique<JBCCompilerComponent>(module);
  compiler.RegisterCompilerComponent(std::move(jbcCompilerComp));
  compiler.InitFromOptions();
  compiler.ParseInputs();
  compiler.PreProcessDecls();
  compiler.ProcessDecls();
  FEManager::GetTypeManager().InitMCCFunctions();
  compiler.PreProcessWithFunctions();
  compiler.ProcessFunctions();
  CHECK_FATAL(success, "Compile Error");
  compiler.ExportMpltFile();
  compiler.ExportMplFile();
  MPLFEEnv::GetInstance().Finish();
  timer.Stop();
  if (FEOptions::GetInstance().IsDumpTime()) {
    INFO(kLncInfo, "mplfe time: %.2lfs", timer.ElapsedMilliseconds() / 1000.0);
  }
  compiler.Release();
  return static_cast<int>(FEErrno::kNoError);
}