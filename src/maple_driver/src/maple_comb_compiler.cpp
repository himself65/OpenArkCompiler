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
#include "usages.h"
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

void MapleCombCompiler::DecideMeRealLevel(MeOption &meOption, const std::vector<mapleOption::Option> &inputOptions) {
  for (const mapleOption::Option &opt : inputOptions) {
    switch (opt.Index()) {
      case kMeOptL1:
        meOption.optLevel = MeOption::kLevelOne;
        break;
      case kMeOptL2:
        meOption.optLevel = MeOption::kLevelTwo;
        // Turn the followings ON only at O2
        meOption.optDirectCall = true;
        meOption.epreIncludeRef = true;
        break;
      default:
        break;
    }
  }
}

MeOption *MapleCombCompiler::MakeMeOptions(const MplOptions &options, MemPool &optMp) {
  MeOption *meOption = new MeOption(optMp);
  auto it = options.GetExeOptions().find(kBinNameMe);
  if (it == options.GetExeOptions().end()) {
    LogInfo::MapleLogger() << "no me input options\n";
    return meOption;
  }
  DecideMeRealLevel(*meOption, it->second);
  if (options.HasSetDebugFlag()) {
    LogInfo::MapleLogger() << "Real Me level:" << std::to_string(meOption->optLevel) << "\n";
  }
  for (const mapleOption::Option &opt : it->second) {
    if (options.HasSetDebugFlag()) {
      LogInfo::MapleLogger() << "Me options: " << opt.Index() << " " << opt.OptionKey() << " " << opt.Args() << '\n';
    }
    switch (opt.Index()) {
      case kMeSkipPhases:
        meOption->SplitSkipPhases(opt.Args());
        break;
      case kMeOptL1:
        // Already handled above in DecideMeRealLevel
        break;
      case kMeOptL2:
        // Already handled above in DecideMeRealLevel
        break;
      case kMeRange:
        meOption->useRange = true;
        meOption->GetRange(opt.Args());
        break;
      case kMeDumpAfter:
        meOption->dumpAfter = (opt.Type() == kEnable);
        break;
      case kMeDumpFunc:
        meOption->dumpFunc = opt.Args();
        break;
      case kMeDumpPhases:
        meOption->SplitPhases(opt.Args(), meOption->dumpPhases);
        break;
      case kMeQuiet:
        meOption->quiet = (opt.Type() == kEnable);
        break;
      case kSetCalleeHasSideEffect:
        meOption->setCalleeHasSideEffect = (opt.Type() == kEnable);
        break;
      case kNoSteensgaard:
        meOption->noSteensgaard = (opt.Type() == kEnable);
        break;
      case kNoTBAA:
        meOption->noTBAA = (opt.Type() == kEnable);
        break;
      case kAliasAnalysisLevel:
        meOption->aliasAnalysisLevel = std::stoul(opt.Args(), nullptr);
        if (meOption->aliasAnalysisLevel > MeOption::kLevelThree) {
          meOption->aliasAnalysisLevel = MeOption::kLevelThree;
        }
        switch (meOption->aliasAnalysisLevel) {
          case MeOption::kLevelThree:
            meOption->setCalleeHasSideEffect = false;
            meOption->noSteensgaard = false;
            meOption->noTBAA = false;
            break;
          case MeOption::kLevelZero:
            meOption->setCalleeHasSideEffect = true;
            meOption->noSteensgaard = true;
            meOption->noTBAA = true;
            break;
          case MeOption::kLevelOne:
            meOption->setCalleeHasSideEffect = false;
            meOption->noSteensgaard = false;
            meOption->noTBAA = true;
            break;
          case MeOption::kLevelTwo:
            meOption->setCalleeHasSideEffect = false;
            meOption->noSteensgaard = true;
            meOption->noTBAA = false;
            break;
          default:
            break;
        }
        if (options.HasSetDebugFlag()) {
          LogInfo::MapleLogger() << "--sub options: setCalleeHasSideEffect "
                                 << meOption->setCalleeHasSideEffect << '\n';
          LogInfo::MapleLogger() << "--sub options: noSteensgaard " << meOption->noSteensgaard << '\n';
          LogInfo::MapleLogger() << "--sub options: noTBAA " << meOption->noTBAA << '\n';
        }
        break;
      case kRcLower:
        meOption->rcLowering = (opt.Type() == kEnable);
        break;
      case kMeNoDot:
        meOption->noDot = (opt.Type() == kEnable);
        break;
      case kStmtNum:
        meOption->stmtNum = (opt.Type() == kEnable);
        break;
      case kEpreLimit:
        meOption->epreLimit = std::stoul(opt.Args(), nullptr);
        break;
      case kEprepuLimit:
        meOption->eprePULimit = std::stoul(opt.Args(), nullptr);
        break;
      case kStmtPrepuLimit:
        meOption->stmtprePULimit = std::stoul(opt.Args(), nullptr);
        break;
      case kLpreLimit:
        meOption->lpreLimit = std::stoul(opt.Args(), nullptr);
        break;
      case kLprepulLimit:
        meOption->lprePULimit = std::stoul(opt.Args(), nullptr);
        break;
      case kPregreNameLimit:
        meOption->pregRenameLimit = std::stoul(opt.Args(), nullptr);
        break;
      case kDelrcpuLimit:
        meOption->delRcPULimit = std::stoul(opt.Args(), nullptr);
        break;
      case kEpreIncludeRef:
        meOption->epreIncludeRef = (opt.Type() == kEnable);
        break;
      case kEpreLocalRefVar:
        meOption->epreLocalRefVar = (opt.Type() == kEnable);
        break;
      case kEprelhSivar:
        meOption->epreLHSIvar = (opt.Type() == kEnable);
        break;
      case kLessThrowAlias:
        meOption->lessThrowAlias = (opt.Type() == kEnable);
        break;
      case kNodeLegateRc:
        meOption->noDelegateRC = (opt.Type() == kEnable);
        break;
      case kNocondBasedRc:
        meOption->noCondBasedRC = (opt.Type() == kEnable);
        break;
      case kNullcheckPre:
        meOption->nullCheckPre = (opt.Type() == kEnable);
        break;
      case kClinitPre:
        meOption->clinitPre = (opt.Type() == kEnable);
        break;
      case kDassignPre:
        meOption->dassignPre = (opt.Type() == kEnable);
        break;
      case kAssign2finalPre:
        meOption->assign2FinalPre = (opt.Type() == kEnable);
        break;
      case kRegReadAtReturn:
        meOption->regreadAtReturn = (opt.Type() == kEnable);
        break;
      case kLpreSpeculate:
        meOption->lpreSpeculate = (opt.Type() == kEnable);
        break;
      case kSpillatCatch:
        meOption->spillAtCatch = (opt.Type() == kEnable);
        break;
      default:
        WARN(kLncWarn, "input invalid key for me " + opt.OptionKey());
        break;
    }
  }
  return meOption;
}

void MapleCombCompiler::DecideMpl2MplRealLevel(Options &mpl2mplOption,
                                               const std::vector<mapleOption::Option> &inputOptions) const {
  for (const mapleOption::Option &opt : inputOptions) {
    if (opt.Index() == kMpl2MplOptL2) {
      mpl2mplOption.O2 = true;
      mpl2mplOption.usePreg = true;
      break;
    }
  }
}

Options *MapleCombCompiler::MakeMpl2MplOptions(const MplOptions &options, MemPool &optMp) {
  auto *mpl2mplOption = new Options(optMp);
  auto it = options.GetExeOptions().find(kBinNameMpl2mpl);
  if (it == options.GetExeOptions().end()) {
    LogInfo::MapleLogger() << "no mpl2mpl input options\n";
    return mpl2mplOption;
  }
  DecideMpl2MplRealLevel(*mpl2mplOption, it->second);
  for (const mapleOption::Option &opt : it->second) {
    if (options.HasSetDebugFlag()) {
      LogInfo::MapleLogger() << "mpl2mpl options: " << opt.Index() << " " << opt.OptionKey() << " " << opt.Args()
                             << '\n';
    }
    switch (opt.Index()) {
      case kMpl2MplDumpBefore:
        mpl2mplOption->dumpBefore = (opt.Type() == kEnable);
        break;
      case kMpl2MplDumpAfter:
        mpl2mplOption->dumpAfter = (opt.Type() == kEnable);
        break;
      case kMpl2MplDumpFunc:
        mpl2mplOption->dumpFunc = opt.Args();
        break;
      case kMpl2MplQuiet:
        mpl2mplOption->quiet = (opt.Type() == kEnable);
        break;
      case kMpl2MplDumpPhase:
        mpl2mplOption->dumpPhase = opt.Args();
        break;
      case kMpl2MplSkipPhase:
        mpl2mplOption->skipPhase = opt.Args();
        break;
      case kMpl2MplSkipFrom:
        mpl2mplOption->skipFrom = opt.Args();
        break;
      case kMpl2MplSkipAfter:
        mpl2mplOption->skipAfter = opt.Args();
        break;
      case kRegNativeDynamicOnly:
        mpl2mplOption->regNativeDynamicOnly = (opt.Type() == kEnable);
        break;
      case kRegNativeStaticBindingList:
        mpl2mplOption->staticBindingList = opt.Args();
        break;
      case kMpl2MplStubJniFunc:
        mpl2mplOption->regNativeFunc = (opt.Type() == kEnable);
        break;
      case kNativeWrapper:
        mpl2mplOption->nativeWrapper = (opt.Type() == kEnable);
        break;
      case kInlineWithProfile:
        mpl2mplOption->inlineWithProfile = (opt.Type() == kEnable);
        break;
      case kMpl2MplUseInline:
        mpl2mplOption->useInline = (opt.Type() == kEnable);
        break;
      case kMpl2MplNoInlineFuncList:
        mpl2mplOption->noInlineFuncList = opt.Args();
        break;
      case kMpl2MplUseCrossModuleInline:
        mpl2mplOption->useCrossModuleInline = (opt.Type() == kEnable);
        break;
      case kInlineSmallFunctionThreshold:
        if (opt.Args().empty()) {
          LogInfo::MapleLogger(kLlErr) << "expecting not empty for --inline-small-function-threshold\n";
          exit(1);
        } else {
          mpl2mplOption->inlineSmallFunctionThreshold = std::stoul(opt.Args());
        }
        break;
      case kInlineHotFunctionThreshold:
        if (opt.Args().empty()) {
          LogInfo::MapleLogger(kLlErr) << "expecting not empty for --inline-hot-function-threshold\n";
          exit(1);
        } else {
          mpl2mplOption->inlineHotFunctionThreshold = std::stoul(opt.Args());
        }
        break;
      case kInlineModuleGrowth:
        if (opt.Args().empty()) {
          LogInfo::MapleLogger(kLlErr) << "expecting not empty for --inline-module-growth\n";
          exit(1);
        } else {
          mpl2mplOption->inlineModuleGrowth = std::stoul(opt.Args());
        }
        break;
      case kInlineColdFunctionThreshold:
        if (opt.Args().empty()) {
          LogInfo::MapleLogger(kLlErr) << "expecting not empty for --inline-cold-function-threshold\n";
          exit(1);
        } else {
          mpl2mplOption->inlineColdFunctionThreshold = std::stoul(opt.Args());
        }
        break;
      case kProfileHotCount:
        if (opt.Args().empty()) {
          LogInfo::MapleLogger(kLlErr) << "expecting not empty for --profile-hot-count\n";
          exit(1);
        } else {
          mpl2mplOption->profileHotCount = std::stoul(opt.Args());
        }
        break;
      case kProfileColdCount:
        if (opt.Args().empty()) {
          LogInfo::MapleLogger(kLlErr) << "expecting not empty for --profile-cold-count\n";
          exit(1);
        } else {
          mpl2mplOption->profileColdCount = std::stoul(opt.Args());
        }
        break;
      case kProfileHotRate:
        if (opt.Args().empty()) {
          LogInfo::MapleLogger(kLlErr) << "expecting not empty for --profile-hot-rate\n";
          exit(1);
        } else {
          mpl2mplOption->profileHotRate = std::stoul(opt.Args());
        }
        break;
      case kProfileColdRate:
        if (opt.Args().empty()) {
          LogInfo::MapleLogger(kLlErr) << "expecting not empty for --profile-cold-rate\n";
          exit(1);
        } else {
          mpl2mplOption->profileColdRate = std::stoul(opt.Args());
        }
        break;
      case kMpl2MplMapleLinker:
        mpl2mplOption->mapleLinker = (opt.Type() == kEnable);
        mpl2mplOption->dumpMuidFile = (opt.Type() == kEnable);
        if (options.HasSetDebugFlag()) {
          LogInfo::MapleLogger() << "--sub options: dumpMuidFile " << mpl2mplOption->dumpMuidFile << '\n';
        }
        break;
      case kMplnkDumpMuid:
        mpl2mplOption->dumpMuidFile = (opt.Type() == kEnable);
        break;
      case kEmitVtableImpl:
        mpl2mplOption->emitVtableImpl = (opt.Type() == kEnable);
        break;
#if MIR_JAVA
      case kMpl2MplSkipVirtual:
        mpl2mplOption->skipVirtualMethod = (opt.Type() == kEnable);
        break;
#endif
      case kMpl2MplNativeOpt:
        mpl2mplOption->nativeOpt = (opt.Type() == kEnable);
        break;
      case kMpl2MplOptL2:
        // Already handled above in DecideMpl2MplRealLevel
        break;
      case kMpl2MplNoDot:
        mpl2mplOption->noDot = (opt.Type() == kEnable);
        break;
      case kGenIRProfile:
        mpl2mplOption->genIRProfile = (opt.Type() == kEnable);
        break;
      case kTestCase:
        mpl2mplOption->testCase = (opt.Type() == kEnable);
        break;
      default:
        WARN(kLncWarn, "input invalid key for mpl2mpl " + opt.OptionKey());
        break;
    }
  }
  return mpl2mplOption;
}

ErrorCode MapleCombCompiler::Compile(const MplOptions &options, MIRModulePtr &theModule) {
  MemPool *optMp = memPoolCtrler.NewMemPool("maplecomb mempool");
  std::string fileName = GetInputFileName(options);
  theModule = new MIRModule(fileName);
  std::unique_ptr<MeOption> meOptions;
  std::unique_ptr<Options> mpl2mplOptions;
  auto it = std::find(options.GetRunningExes().begin(), options.GetRunningExes().end(), kBinNameMe);
  if (it != options.GetRunningExes().end()) {
    meOptions.reset(MakeMeOptions(options, *optMp));
  }
  auto iterMpl2Mpl = std::find(options.GetRunningExes().begin(), options.GetRunningExes().end(), kBinNameMpl2mpl);
  if (iterMpl2Mpl != options.GetRunningExes().end()) {
    mpl2mplOptions.reset(MakeMpl2MplOptions(options, *optMp));
  }

  LogInfo::MapleLogger() << "Starting mpl2mpl&mplme\n";
  PrintCommand(options);
  DriverRunner runner(theModule, options.GetRunningExes(), mpl2mplOptions.get(), fileName, meOptions.get(),
                      fileName, fileName, optMp,
                      options.HasSetTimePhases(), options.HasSetGenVtableImpl(), options.HasSetGenMeMpl());
  ErrorCode nErr = runner.Run();

  memPoolCtrler.DeleteMemPool(optMp);
  return nErr;
}
}  // namespace maple
