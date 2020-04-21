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
#include "mpl_options.h"
#include <string>
#include <vector>
#include <algorithm>
#include "compiler_factory.h"
#include "file_utils.h"
#include "mpl_logging.h"
#include "option_parser.h"
#include "string_utils.h"
#include "version.h"
#include "default_options.def"
#include "driver_option_common.h"
#ifndef EMUI_BUILD
#include "jbc2mpl_option.h"
#endif
#include "me_option.h"
#include "option.h"
#include "cg_option.h"

namespace maple {
using namespace mapleOption;
using namespace maplebe;

const std::string kMapleDriverVersion = "MapleDriver " + std::to_string(Version::kMajorMplVersion) + "." +
                                        std::to_string(Version::kMinorCompilerVersion) + " 20190929";

const std::vector<std::string> kMapleCompilers = { "jbc2mpl",
    "me", "mpl2mpl", "mplcg" };

int MplOptions::Parse(int argc, char **argv) {
  optionParser.reset(new OptionParser());
  optionParser->RegisteUsages(DriverOptionCommon::GetInstance());
#ifndef EMUI_BUILD
  optionParser->RegisteUsages(jbcUsage);
#endif
  optionParser->RegisteUsages(Options::GetInstance());
  optionParser->RegisteUsages(MeOption::GetInstance());
  optionParser->RegisteUsages(CGOptions::GetInstance());
  exeFolder = FileUtils::GetFileFolder(*argv);
  int ret = optionParser->Parse(argc, argv);
  if (ret != kErrorNoError) {
    return ret;
  }
  // We should recognize O0, O2 and run options firstly to decide the real options
  ret = DecideRunType();
  if (ret != kErrorNoError) {
    return ret;
  }

  // Set default as O0
  if (runMode == RunMode::kUnkownRun) {
    optimizationLevel = kO0;
  }
  // Make sure in Auto mode
  if (runMode != RunMode::kCustomRun) {
    setDefaultLevel = true;
  }

  // Check whether the input files were valid
  ret = CheckInputFileValidity();
  if (ret != kErrorNoError) {
    return ret;
  }

  // Decide runningExes for default options(O0, O2) by input files
  if (runMode != RunMode::kCustomRun) {
    ret = DecideRunningPhases();
    if (ret != kErrorNoError) {
      return ret;
    }
  }

  // Handle other options
  ret = HandleGeneralOptions();
  if (ret != kErrorNoError) {
    return ret;
  }
  // Check whether the file was readable
  ret = CheckFileExits();
  return ret;
}

ErrorCode MplOptions::HandleGeneralOptions() {
  ErrorCode ret = kErrorNoError;
  for (auto opt : optionParser->GetOptions()) {
    switch (opt.Index()) {
      case kHelp: {
        if (!opt.Args().empty()) {
          if (std::find(kMapleCompilers.begin(), kMapleCompilers.end(), opt.Args()) != kMapleCompilers.end()) {
            optionParser->PrintUsage(opt.Args(), helpLevel);
            return kErrorExitHelp;
          }
        }
        optionParser->PrintUsage("all", helpLevel);
        return kErrorExitHelp;
      }
      case kVersion: {
        LogInfo::MapleLogger() << kMapleDriverVersion << "\n";
        return kErrorExitHelp;
      }
      case kMeOpt:
        ret = UpdatePhaseOption(opt.Args(), kBinNameMe);
        if (ret != kErrorNoError) {
          return ret;
        }
        break;
      case kMpl2MplOpt:
        ret = UpdatePhaseOption(opt.Args(), kBinNameMpl2mpl);
        if (ret != kErrorNoError) {
          return ret;
        }
        break;
      case kMplcgOpt:
        ret = UpdatePhaseOption(opt.Args(), kBinNameMplcg);
        if (ret != kErrorNoError) {
          return ret;
        }
        break;
      case kTimePhases:
        timePhases = true;
        printCommandStr += " -time-phases";
        break;
      case kGenMeMpl:
        genMeMpl = true;
        printCommandStr += " --genmempl";
        break;
      case kGenVtableImpl:
        genVtableImpl = true;
        printCommandStr += " --genVtableImpl";
        break;
      case kSaveTemps:
        isSaveTmps = true;
        genMeMpl = true;
        genVtableImpl = true;
        StringUtils::Split(opt.Args(), saveFiles, ',');
        printCommandStr += " --save-temps";
        break;
      case kOption:
        if (UpdateExtraOptionOpt(opt.Args()) != kErrorNoError) {
          return kErrorInvalidParameter;
        }
        break;
      case kInMplt:
        break;
      case kAllDebug:
        debugFlag = true;
        break;
      default:
        // I do not care
        break;
    }
    ret = AddOption(opt);
  }
  return ret;
}

ErrorCode MplOptions::DecideRunType() {
  ErrorCode ret = kErrorNoError;
  bool runModeConflict = false;
  for (auto opt : optionParser->GetOptions()) {
    switch (opt.Index()) {
      case kOptimization0:
        if (runMode == RunMode::kCustomRun) {  // O0 and run should not appear at the same time
          runModeConflict = true;
        } else {
          runMode = RunMode::kAutoRun;
          optimizationLevel = kO0;
        }
        break;
      case kOptimization2:
        if (runMode == RunMode::kCustomRun) {  // O0 and run should not appear at the same time
          runModeConflict = true;
        } else {
          runMode = RunMode::kAutoRun;
          optimizationLevel = kO2;
        }
        break;
      case kRun:
        if (runMode == RunMode::kAutoRun) {    // O0 and run should not appear at the same time
          runModeConflict = true;
        } else {
          runMode = RunMode::kCustomRun;
          UpdateRunningExe(opt.Args());
        }
        break;
      case kInFile: {
        if (!Init(opt.Args())) {
          return kErrorInitFail;
        }
        break;
      }
      case kHelpLevel:
        helpLevel = std::stoul(opt.Args());
        break;
      default:
        break;
    }
  }
  if (runModeConflict) {
    LogInfo::MapleLogger(kLlErr) << "Cannot set auto mode and run mode at the same time!\n";
    ret = kErrorInvalidParameter;
  }
  return ret;
}

ErrorCode MplOptions::DecideRunningPhases() {
  ErrorCode ret = kErrorNoError;
  bool isNeedMapleComb = true;
  bool isNeedMplcg = true;
  switch (inputFileType) {
    case InputFileType::kJar:
      // fall-through
    case InputFileType::kClass:
      UpdateRunningExe(kBinNameJbc2mpl);
      break;
    case InputFileType::kMpl:
      break;
    case InputFileType::kVtableImplMpl:
      isNeedMapleComb = false;
      break;
    case InputFileType::kS:
      isNeedMplcg = false;
      break;
    case InputFileType::kNone:
      break;
    default:
      break;
  }
  if (isNeedMapleComb) {
    ret = AppendDefaultCombOptions();
    if (ret != kErrorNoError) {
      return ret;
    }
  }
  if (isNeedMplcg) {
    ret = AppendDefaultCgOptions();
    if (ret != kErrorNoError) {
      return ret;
    }
  }
  return ret;
}

ErrorCode MplOptions::CheckInputFileValidity() {
  // Get input fileName
  if (optionParser->GetNonOptionsCount() <= 0) {
    return kErrorNoError;
  }
  std::ostringstream optionString;
  const std::vector<std::string> &inputs = optionParser->GetNonOptions();
  for (size_t i = 0; i < inputs.size(); ++i) {
    if (i == 0) {
      optionString << inputs[i];
    } else {
      optionString <<  "," << inputs[i];
    }
  }
  if (!Init(optionString.str())) {
    return kErrorInitFail;
  }
  return kErrorNoError;
}

ErrorCode MplOptions::CheckFileExits() {
  ErrorCode ret = kErrorNoError;
  if (inputFiles == "") {
    LogInfo::MapleLogger(kLlErr) << "Forgot to input files?\n";
    return ErrorCode::kErrorInitFail;
  }
  for (auto fileName : splitsInputFiles) {
    std::ifstream infile;
    infile.open(fileName);
    if (infile.fail()) {
      LogInfo::MapleLogger(kLlErr) << "Cannot open input file " << fileName << '\n';
      ret = kErrorFileNotFound;
      return ret;
    }
  }
  return ret;
}

ErrorCode MplOptions::AddOption(const mapleOption::Option &option) {
  if (!option.HasExtra()) {
    return kErrorNoError;
  }
  for (const auto &exeName : option.GetExtras()) {
    auto iter = std::find(runningExes.begin(), runningExes.end(), exeName);
    if (iter == runningExes.end()) {
      continue;
    }
    // For compilers, such as me, mpl2mpl
    exeOptions[exeName].push_back(option);
    // Fill extraOption
    // For compiler bins called by system()
    auto &extraOption = extras[exeName];
    if (option.Args() != "") {
      MplOption mplOption("-" + option.OptionKey(), option.Args());
      extraOption.push_back(mplOption);
    } else {
      MplOption mplOption("-" + option.OptionKey(), "");
      extraOption.push_back(mplOption);
    }
  }
  return kErrorNoError;
}

bool MplOptions::Init(const std::string &inputFile) {
  if (StringUtils::Trim(inputFile).empty()) {
    return false;
  }
  inputFiles = inputFile;
  StringUtils::Split(inputFile, splitsInputFiles, ',');
  std::string firstInputFile = splitsInputFiles[0];
  inputFolder = FileUtils::GetFileFolder(firstInputFile);
  outputFolder = inputFolder;
  outputName = FileUtils::GetFileName(firstInputFile, false);
  std::string extensionName = FileUtils::GetFileExtension(firstInputFile);
  if (extensionName == "class") {
    inputFileType = InputFileType::kClass;
  }
  else if (extensionName == "jar") {
    inputFileType = InputFileType::kJar;
  } else if (extensionName == "mpl") {
    if (firstInputFile.find("VtableImpl") == std::string::npos) {
      inputFileType = InputFileType::kMpl;
    } else {
      inputFileType = InputFileType::kVtableImplMpl;
    }
  } else if (extensionName == "s") {
    inputFileType = InputFileType::kS;
  } else {
    return false;
  }
  return true;
}

ErrorCode MplOptions::AppendDefaultCombOptions() {
  ErrorCode ret = kErrorNoError;
  if (optimizationLevel == kO0) {
    ret = AppendDefaultOptions(kBinNameMe, kMeDefaultOptionsO0, sizeof(kMeDefaultOptionsO0) / sizeof(MplOption));
    if (ret != kErrorNoError) {
      return ret;
    }
    ret = AppendDefaultOptions(kBinNameMpl2mpl, kMpl2MplDefaultOptionsO0,
                               sizeof(kMpl2MplDefaultOptionsO0) / sizeof(MplOption));
    if (ret != kErrorNoError) {
      return ret;
    }
  } else if (optimizationLevel == kO2) {
    ret = AppendDefaultOptions(kBinNameMe, kMeDefaultOptionsO2, sizeof(kMeDefaultOptionsO2) / sizeof(MplOption));
    if (ret != kErrorNoError) {
      return ret;
    }
    ret = AppendDefaultOptions(kBinNameMpl2mpl, kMpl2MplDefaultOptionsO2,
                               sizeof(kMpl2MplDefaultOptionsO2) / sizeof(MplOption));
    if (ret != kErrorNoError) {
      return ret;
    }
  }
  return ret;
}

ErrorCode MplOptions::AppendDefaultCgOptions() {
  ErrorCode ret = kErrorNoError;
  if (optimizationLevel == kO0) {
    ret = AppendDefaultOptions(kBinNameMplcg, kMplcgDefaultOptionsO0,
                               sizeof(kMplcgDefaultOptionsO0) / sizeof(MplOption));
    if (ret != kErrorNoError) {
      return ret;
    }
  } else if (optimizationLevel == kO2) {
    ret = AppendDefaultOptions(kBinNameMplcg, kMplcgDefaultOptionsO2,
                               sizeof(kMplcgDefaultOptionsO2) / sizeof(MplOption));
    if (ret != kErrorNoError) {
      return ret;
    }
  }
  return ret;
}

ErrorCode MplOptions::AppendDefaultOptions(const std::string &exeName, MplOption mplOptions[], unsigned int length) {
  auto &exeOption = exeOptions[exeName];
  for (size_t i = 0; i < length; ++i) {
    bool ret = optionParser->SetOption(mplOptions[i].GetKey(), mplOptions[i].GetValue(), exeName, exeOption);
    if (!ret) {
      return kErrorInvalidParameter;
    }
  }
  auto iter = std::find(runningExes.begin(), runningExes.end(), exeName);
  if (iter == runningExes.end()) {
    runningExes.push_back(exeName);
  }
  return kErrorNoError;
}

ErrorCode MplOptions::UpdatePhaseOption(const std::string &args, const std::string &exeName) {
  auto iter = std::find(runningExes.begin(), runningExes.end(), exeName);
  if (iter == runningExes.end()) {
    LogInfo::MapleLogger(kLlErr) << "Cannot find phase " << exeName << '\n';
    return kErrorExit;
  }
  std::vector<std::string> tmpArgs;
  // Split options with ' '
  StringUtils::Split(args, tmpArgs, ' ');
  auto &exeOption = exeOptions[exeName];
  ErrorCode ret = optionParser->HandleInputArgs(tmpArgs, exeName, exeOption);
  if (ret != kErrorNoError) {
    return ret;
  }
  // Fill extraOption
  // For compiler bins called by system()
  auto &extraOption = extras[exeName];
  for (size_t i = 0; i < exeOption.size(); ++i) {
    if (exeOption[i].Args() != "") {
      MplOption mplOption("-" + exeOption[i].OptionKey(), exeOption[i].Args());
      extraOption.push_back(mplOption);
    } else {
      MplOption mplOption("-" + exeOption[i].OptionKey(), "");
      extraOption.push_back(mplOption);
    }
  }
  return ret;
}

ErrorCode MplOptions::UpdateExtraOptionOpt(const std::string &args) {
  std::vector<std::string> temp;
#ifdef _WIN32
  // Paths on windows may contain such string like "C:/", then it would be confused with the split symbol ":"
  StringUtils::Split(args, temp, ';');
#else
  StringUtils::Split(args, temp, ':');
#endif
  if (temp.size() != runningExes.size()) {
    // parameter not match ignore
    LogInfo::MapleLogger(kLlErr) << "The --run and --option are not matched, please check them."
                                 << "(Too many or too few ':'?)"
                                 << '\n';
    return kErrorInvalidParameter;
  }
  auto settingExe = runningExes.begin();
  for (const auto &tempIt : temp) {
    ErrorCode ret = UpdatePhaseOption(tempIt, *settingExe);
    if (ret != kErrorNoError) {
      return ret;
    }
    ++settingExe;
  }
  return kErrorNoError;
}

void MplOptions::UpdateRunningExe(const std::string &args) {
  std::vector<std::string> results;
  StringUtils::Split(args, results, ':');
  for (size_t i = 0; i < results.size(); ++i) {
    auto iter = std::find(runningExes.begin(), runningExes.end(), results[i]);
    if (iter == runningExes.end()) {
      runningExes.push_back(results[i]);
    }
  }
}
}  // namespace maple
