/*
 * Copyright (c) [2019-2020] Huawei Technologies Co.,Ltd.All rights reserved.
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
#include <cstdlib>
#include "compiler.h"
#include "default_options.def"
#include "mpl_logging.h"
#include "mpl_timer.h"
#include "driver_runner.h"

namespace maple {
using namespace maplebe;
using namespace mapleOption;

DefaultOption MplcgCompiler::GetDefaultOptions(const MplOptions &options) const {
  DefaultOption defaultOptions = { nullptr, 0 };
  if (options.GetOptimizationLevel() == kO0 && options.HasSetDefaultLevel()) {
    defaultOptions.mplOptions = kMplcgDefaultOptionsO0;
    defaultOptions.length = sizeof(kMplcgDefaultOptionsO0) / sizeof(MplOption);
  } else if (options.GetOptimizationLevel() == kO2 && options.HasSetDefaultLevel()) {
    defaultOptions.mplOptions = kMplcgDefaultOptionsO2;
    defaultOptions.length = sizeof(kMplcgDefaultOptionsO2) / sizeof(MplOption);
  }
  for (uint32_t i = 0; i < defaultOptions.length; ++i) {
    defaultOptions.mplOptions[i].SetValue(
        FileUtils::AppendMapleRootIfNeeded(defaultOptions.mplOptions[i].GetNeedRootPath(),
                                           defaultOptions.mplOptions[i].GetValue(),
                                           options.GetExeFolder()));
  }
  return defaultOptions;
}

const std::string &MplcgCompiler::GetBinName() const {
  return kBinNameMplcg;
}

std::string MplcgCompiler::GetInputFileName(const MplOptions &options) const {
  if (!options.GetRunningExes().empty()) {
    if (options.GetRunningExes()[0] == kBinNameMplcg) {
      return options.GetInputFiles();
    }
  }
  // Get base file name
  auto idx = options.GetOutputName().find(".VtableImpl");
  std::string outputName = options.GetOutputName();
  if (idx != std::string::npos) {
    outputName = options.GetOutputName().substr(0, idx);
  }
  return options.GetOutputFolder() + outputName + ".VtableImpl.mpl";
}
void MplcgCompiler::PrintCommand(const MplOptions &options) const {
  std::string runStr = "--run=";
  std::string optionStr = "--option=\"";
  std::string connectSym = "";
  if (options.GetExeOptions().find(kBinNameMplcg) != options.GetExeOptions().end()) {
    runStr += "mplcg";
    auto it = options.GetExeOptions().find(kBinNameMplcg);
    if (it == options.GetExeOptions().end()) {
      return;
    }
    for (const mapleOption::Option &opt : it->second) {
      connectSym = !opt.Args().empty() ? "=" : "";
      optionStr += (" --" + opt.OptionKey() + connectSym + opt.Args());
    }
  }
  optionStr += "\"";
  LogInfo::MapleLogger() << "Starting:" << options.GetExeFolder() << "maple " << runStr << " " << optionStr
                         << " --infile " << GetInputFileName(options) << '\n';
}

bool MplcgCompiler::MakeCGOptions(const MplOptions &options) {
  CGOptions &cgOption = CGOptions::GetInstance();
  cgOption.SetOption(CGOptions::kDefaultOptions);
  cgOption.SetOption(CGOptions::kWithMpl);
  cgOption.SetGenerateFlags(CGOptions::kDefaultGflags);
  auto it = options.GetExeOptions().find(kBinNameMplcg);
  if (it == options.GetExeOptions().end()) {
    LogInfo::MapleLogger() << "no me input options\n";
    return false;
  }
  bool result = cgOption.SolveOptions(it->second, options.HasSetDebugFlag());
  if (result == false) {
    LogInfo::MapleLogger() << "Meet error mplcg options\n";
    return false;
  }
  return true;
}

ErrorCode MplcgCompiler::Compile(const MplOptions &options, MIRModulePtr &theModule) {
  MemPool *optMp = memPoolCtrler.NewMemPool("maplecg mempool");
  CGOptions &cgOption = CGOptions::GetInstance();
  bool result = MakeCGOptions(options);
  if (!result) {
    return kErrorCompileFail;
  }
  std::string fileName = GetInputFileName(options);
  std::string baseName = options.GetOutputFolder() + FileUtils::GetFileName(fileName, false);
  std::string output = baseName + ".s";
  bool parsed = false;
  std::unique_ptr<MIRParser> theParser;
  if (theModule == nullptr) {
    MPLTimer timer;
    timer.Start();
    theModule = new MIRModule(fileName);
    theModule->SetWithMe(
        std::find(options.GetRunningExes().begin(), options.GetRunningExes().end(),
                  kBinNameMe) != options.GetRunningExes().end());
    theParser.reset(new MIRParser(*theModule));
    parsed = theParser->ParseMIR(0, cgOption.GetParserOption());
    if (parsed) {
      if (!cgOption.IsQuiet() && theParser->GetWarning().size()) {
        theParser->EmitWarning(fileName);
      }
    } else {
      if (theParser != nullptr) {
        theParser->EmitError(fileName);
      }
      memPoolCtrler.DeleteMemPool(optMp);
      return kErrorCompileFail;
    }
    timer.Stop();
    LogInfo::MapleLogger() << "Mplcg Parser consumed " << timer.ElapsedMilliseconds() << "ms\n";
  }

  LogInfo::MapleLogger() << "Starting mplcg\n";
  DriverRunner runner(theModule, options.GetRunningExes(), fileName, optMp, options.HasSetTimePhases());
  PrintCommand(options);
  runner.SetCGInfo(&cgOption, fileName);
  runner.ProcessCGPhase(output, baseName);

  memPoolCtrler.DeleteMemPool(optMp);
  return kErrorNoError;
}
}  // namespace maple
