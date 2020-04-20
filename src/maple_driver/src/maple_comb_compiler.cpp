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
#include <iterator>
#include <algorithm>
#include "compiler.h"
#include "string_utils.h"
#include "mpl_logging.h"
#include "driver_runner.h"

namespace maple {
using namespace mapleOption;

std::string MapleCombCompiler::GetInputFileName(const MplOptions &options) const {
  if (!options.GetRunningExes().empty()) {
    if (options.GetRunningExes()[0] == kBinNameMe || options.GetRunningExes()[0] == kBinNameMpl2mpl) {
      return options.GetInputFiles();
    }
  }
  if (options.GetInputFileType() == InputFileType::kVtableImplMpl) {
    return options.GetOutputFolder() + options.GetOutputName() + ".VtableImpl.mpl";
  }
  return options.GetOutputFolder() + options.GetOutputName() + ".mpl";
}

void MapleCombCompiler::GetTmpFilesToDelete(const MplOptions &mplOptions, std::vector<std::string> &tempFiles) const {
  std::string filePath;
  if ((realRunningExe == kBinNameMe) && !mplOptions.HasSetGenMeMpl()) {
    filePath = mplOptions.GetOutputFolder() + mplOptions.GetOutputName() + ".me.mpl";
  } else if (mplOptions.HasSetGenVtableImpl() == false) {
    filePath = mplOptions.GetOutputFolder() + mplOptions.GetOutputName() + ".VtableImpl.mpl";
  }
  tempFiles.push_back(filePath);
  filePath = mplOptions.GetOutputFolder() + mplOptions.GetOutputName() + ".data.muid";
  tempFiles.push_back(filePath);
  filePath = mplOptions.GetOutputFolder() + mplOptions.GetOutputName() + ".func.muid";
  tempFiles.push_back(filePath);
  for (auto iter = tempFiles.begin(); iter != tempFiles.end();) {
    std::ifstream infile;
    infile.open(*iter);
    if (infile.fail()) {
      iter = tempFiles.erase(iter);
    } else {
      ++iter;
    }
  }
}

std::unordered_set<std::string> MapleCombCompiler::GetFinalOutputs(const MplOptions &mplOptions) const {
  std::unordered_set<std::string> finalOutputs;
  finalOutputs.insert(mplOptions.GetOutputFolder() + mplOptions.GetOutputName() + ".VtableImpl.mpl");
  return finalOutputs;
}

void MapleCombCompiler::PrintCommand(const MplOptions &options) const {
  std::string runStr = "--run=";
  std::ostringstream optionStr;
  optionStr << "--option=\"";
  std::string connectSym = "";
  bool firstComb = false;
  if (options.GetExeOptions().find(kBinNameMe) != options.GetExeOptions().end()) {
    runStr += "me";
    auto it = options.GetExeOptions().find(kBinNameMe);
    for (const mapleOption::Option &opt : it->second) {
      connectSym = !opt.Args().empty() ? "=" : "";
      optionStr << " --" << opt.OptionKey() << connectSym << opt.Args();
    }
    firstComb = true;
  }
  if (options.GetExeOptions().find(kBinNameMpl2mpl) != options.GetExeOptions().end()) {
    if (firstComb) {
      runStr += ":mpl2mpl";
      optionStr << ":";
    } else {
      runStr += "mpl2mpl";
    }
    auto it = options.GetExeOptions().find(kBinNameMpl2mpl);
    for (const mapleOption::Option &opt : it->second) {
      connectSym = !opt.Args().empty() ? "=" : "";
      optionStr << " --" << opt.OptionKey() << connectSym << opt.Args();
    }
  }
  optionStr << "\"";
  LogInfo::MapleLogger() << "Starting:" << options.GetExeFolder() << "maple " << runStr << " " << optionStr.str() << " "
                         << GetInputFileName(options) << options.GetPrintCommandStr() << '\n';
}

bool MapleCombCompiler::MakeMeOptions(const MplOptions &options) {
  MeOption &meOption = MeOption::GetInstance();
  auto it = options.GetExeOptions().find(kBinNameMe);
  if (it == options.GetExeOptions().end()) {
    LogInfo::MapleLogger() << "no me input options\n";
    return false;
  }
  bool result = meOption.SolveOptions(it->second, options.HasSetDebugFlag());
  if (result == false) {
    LogInfo::MapleLogger() << "Meet error mpl2mpl options\n";
    return false;
  }
  return true;
}

bool MapleCombCompiler::MakeMpl2MplOptions(const MplOptions &options) {
  auto &mpl2mplOption = Options::GetInstance();
  auto it = options.GetExeOptions().find(kBinNameMpl2mpl);
  if (it == options.GetExeOptions().end()) {
    LogInfo::MapleLogger() << "no mpl2mpl input options\n";
    return false;
  }
  bool result = mpl2mplOption.SolveOptions(it->second,
                                           options.HasSetDebugFlag());
  if (result == false) {
    LogInfo::MapleLogger() << "Meet error mpl2mpl options\n";
    return false;
  }
  return true;
}

ErrorCode MapleCombCompiler::Compile(const MplOptions &options, MIRModulePtr &theModule) {
  MemPool *optMp = memPoolCtrler.NewMemPool("maplecomb mempool");
  std::string fileName = GetInputFileName(options);
  theModule = new MIRModule(fileName);
  MeOption &meOptions = MeOption::GetInstance();
  Options &mpl2mplOptions = Options::GetInstance();
  auto it = std::find(options.GetRunningExes().begin(), options.GetRunningExes().end(), kBinNameMe);
  if (it != options.GetRunningExes().end()) {
    bool result = MakeMeOptions(options);
    if (!result) {
      return kErrorCompileFail;
    }
  }
  auto iterMpl2Mpl = std::find(options.GetRunningExes().begin(), options.GetRunningExes().end(), kBinNameMpl2mpl);
  if (iterMpl2Mpl != options.GetRunningExes().end()) {
    bool result = MakeMpl2MplOptions(options);
    if (!result) {
      return kErrorCompileFail;
    }
  }

  LogInfo::MapleLogger() << "Starting mpl2mpl&mplme\n";
  PrintCommand(options);
  DriverRunner runner(theModule, options.GetRunningExes(), &mpl2mplOptions, fileName, &meOptions,
                      fileName, fileName, optMp,
                      options.HasSetTimePhases(), options.HasSetGenVtableImpl(), options.HasSetGenMeMpl());
  ErrorCode nErr = runner.Run();

  memPoolCtrler.DeleteMemPool(optMp);
  return nErr;
}
}  // namespace maple
