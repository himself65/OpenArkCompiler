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
#include "compiler.h"
#include <cstdlib>
#include "file_utils.h"
#include "safe_exe.h"
#include "mpl_timer.h"

namespace maple {
using namespace mapleOption;

const std::string kBinNameJbc2mpl = "jbc2mpl";
const std::string kBinNameMe = "me";
const std::string kBinNameMpl2mpl = "mpl2mpl";
const std::string kBinNameMplcg = "mplcg";
const std::string kBinNameMapleComb = "maplecomb";

int Compiler::Exe(const MplOptions &mplOptions, const std::string &options) const {
  std::ostringstream ostrStream;
  ostrStream << GetBinPath(mplOptions) << GetBinName();
  std::string binPath = FileUtils::ConvertPathIfNeeded(ostrStream.str());
  return SafeExe::Exe(binPath, options);
}

std::string Compiler::GetBinPath(const MplOptions &mplOptions) const {
#ifdef MAPLE_PRODUCT_EXECUTABLE  // build flag -DMAPLE_PRODUCT_EXECUTABLE
  std::string binPath = std::string(MAPLE_PRODUCT_EXECUTABLE);
  if (binPath.empty()) {
    binPath = mplOptions.exeFolder;
  } else {
    binPath = binPath + kFileSeperatorChar;
  }
#else
  std::string binPath = mplOptions.GetExeFolder();
#endif
  return binPath;
}

ErrorCode Compiler::Compile(const MplOptions &options, MIRModulePtr &theModule) {
  MPLTimer timer = MPLTimer();
  LogInfo::MapleLogger() << "Starting " << GetName() << '\n';
  timer.Start();
  std::string strOption = MakeOption(options);
  if (strOption.empty()) {
    return ErrorCode::kErrorInvalidParameter;
  }
  if (Exe(options, strOption) != 0) {
    return ErrorCode::kErrorCompileFail;
  }
  timer.Stop();
  LogInfo::MapleLogger() << (GetName() + " consumed ") << timer.Elapsed() << "s" << '\n';
  return ErrorCode::kErrorNoError;
}

std::string Compiler::MakeOption(const MplOptions &options) {
  std::map<std::string, MplOption> finalOptions;
  auto defaultOptions = MakeDefaultOptions(options);
  AppendDefaultOptions(finalOptions, defaultOptions);
  for (auto binName : GetBinNames()) {
    auto userOption = options.GetOptions().find(binName);
    if (userOption != options.GetOptions().end()) {
      AppendUserOptions(finalOptions, userOption->second);
    }
  }
  AppendExtraOptions(finalOptions, options.GetExtras());
  std::ostringstream strOption;
  for (auto finalOption : finalOptions) {
    strOption << " " << finalOption.first << finalOption.second.GetconnectSymbol() << finalOption.second.GetValue();
    if (options.HasSetDebugFlag()) {
      LogInfo::MapleLogger() << Compiler::GetName() << " options: " << finalOption.first << " "
                             << finalOption.second.GetValue() << '\n';
    }
  }
  strOption << " " << this->GetInputFileName(options);
  if (options.HasSetDebugFlag()) {
    LogInfo::MapleLogger() << Compiler::GetName() << " input files: " << GetInputFileName(options) << '\n';
  }
  return FileUtils::ConvertPathIfNeeded(strOption.str());
}

void Compiler::AppendDefaultOptions(std::map<std::string, MplOption> &finalOptions,
                                    const std::map<std::string, MplOption> &defaultOptions) const {
  for (auto defaultIt : defaultOptions) {
    finalOptions.insert(make_pair(defaultIt.first, defaultIt.second));
  }
}

void Compiler::AppendUserOptions(std::map<std::string, MplOption> &finalOptions,
                                 const std::vector<Option> &userOptions) const {
  for (auto &binName : GetBinNames()) {
    for (auto userOption : userOptions) {
      auto extra = userOption.FindExtra(binName);
      if (extra != nullptr) {
        AppendOptions(finalOptions, extra->optionKey, userOption.Args(), userOption.ConnectSymbol(binName));
      }
    }
  }
}

void Compiler::AppendExtraOptions(std::map<std::string, MplOption> &finalOptions,
                                  const std::map<std::string, std::vector<MplOption>> &extraOptions) const {
  auto binNames = GetBinNames();
  for (auto &binNamesIt : binNames) {
    auto extras = extraOptions.find(binNamesIt);
    if (extras == extraOptions.end()) {
      continue;
    }
    for (auto &secondExtras : extras->second) {
      AppendOptions(finalOptions, secondExtras.GetKey(), secondExtras.GetValue(), secondExtras.GetconnectSymbol());
    }
  }
}

std::map<std::string, MplOption> Compiler::MakeDefaultOptions(const MplOptions &options) {
  auto rawDefaultOptions = GetDefaultOptions(options);
  std::map<std::string, MplOption> defaultOptions;
  if (rawDefaultOptions.mplOptions != nullptr) {
    for (unsigned int i = 0; i < rawDefaultOptions.length; i++) {
      defaultOptions.insert(std::make_pair(rawDefaultOptions.mplOptions[i].GetKey(), rawDefaultOptions.mplOptions[i]));
    }
  }
  return defaultOptions;
}

void Compiler::AppendOptions(std::map<std::string, MplOption> &finalOptions, const std::string &key,
                             const std::string &value, const std::string &connectSymbol) const {
  auto finalOpt = finalOptions.find(key);
  if (finalOpt != finalOptions.end()) {
    if (finalOpt->second.GetIsAppend()) {
      std::string temp = finalOpt->second.GetValue() + finalOpt->second.GetAppendSplit() + value;
      finalOpt->second.SetValue(temp);
    } else {
      finalOpt->second.SetValue(value);
    }
  } else {
    MplOption option(key, value, connectSymbol, false, "");
    finalOptions.insert(make_pair(key, option));
  }
}

const bool Compiler::CanAppendOptimization(const std::string &optionStr) const {
  // there're some issues for passing -Ox to each component, let users determine self.
  return false;
}

std::string Compiler::AppendOptimization(const MplOptions &options, const std::string &optionStr) const {
  if (!CanAppendOptimization(optionStr)) {
    return optionStr;
  }
  return optionStr + " " + options.OptimizationLevelStr();
}
}  // namespace maple
