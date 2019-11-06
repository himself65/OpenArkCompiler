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
#include <iterator>
#include <algorithm>
#include "compiler.h"
#include "usages.h"
#include "string_utils.h"
#include "mpl_logging.h"
#include "driver_runner.h"

namespace maple {
using namespace mapleOption;

const std::string MapleCombCompiler::GetInputFileName(const MplOptions &options) const {
  if (options.GetInputFileType() == InputFileType::kVtableImplMpl) {
    return options.GetOutputFolder() + options.GetOutputName() + ".VtableImpl.mpl";
  } else {
    return options.GetOutputFolder() + options.GetOutputName() + ".mpl";
  }
}


const std::unordered_set<std::string> MapleCombCompiler::GetFinalOutputs(const MplOptions &mplOptions) const {
  auto finalOutputs = std::unordered_set<std::string>();
  finalOutputs.insert(mplOptions.GetOutputFolder() + mplOptions.GetOutputName() + ".VtableImpl.mpl");
  return finalOutputs;
}

void MapleCombCompiler::PrintCommand(const MplOptions &options) const {
  std::string runStr = "--run=";
  std::string optionStr = "--option=\"";
  std::string connectSym = "";
  bool firstComb = false;
  if (options.GetExeOptions().find(kBinNameMe) != options.GetExeOptions().end()) {
    runStr += "me";
    auto inputMeOptions = options.GetExeOptions().find(kBinNameMe);
    for (auto &opt : inputMeOptions->second) {
      connectSym = opt.Args() != "" ? "=" : "";
      optionStr += " --" + opt.OptionKey() + connectSym + opt.Args();
    }
    firstComb = true;
  }
  if (options.GetExeOptions().find(kBinNameMpl2mpl) != options.GetExeOptions().end()) {
    if (firstComb) {
      runStr += ":mpl2mpl";
      optionStr += ":";
    } else {
      runStr += "mpl2mpl";
    }
    auto inputMpl2mplOptions = options.GetExeOptions().find(kBinNameMpl2mpl);
    for (auto &opt : inputMpl2mplOptions->second) {
      connectSym = opt.Args() != "" ? "=" : "";
      optionStr += " --" + opt.OptionKey() + connectSym + opt.Args();
    }
  }
  optionStr += "\"";
  LogInfo::MapleLogger() << "Starting:" << options.GetExeFolder() << "maple " << runStr << " " << optionStr << " "
                         << GetInputFileName(options) << options.GetPrintCommandStr() << '\n';
}

MeOption *MapleCombCompiler::MakeMeOptions(const MplOptions &options, maple::MemPool &optMp) {
  MeOption *meOption = new MeOption(optMp);
  auto inputMeOptions = options.GetExeOptions().find(kBinNameMe);
  if (inputMeOptions == options.GetExeOptions().end()) {
    LogInfo::MapleLogger() << "no me input options" << '\n';
    return meOption;
  }
  for (auto &opt : inputMeOptions->second) {
    if (options.HasSetDebugFlag()) {
      LogInfo::MapleLogger() << "Me options: " << opt.Index() << " " << opt.OptionKey() << " " << opt.Args()
                             << '\n';
    }
    switch (opt.Index()) {
      case kMeSkipPhases:
        meOption->SplitSkipPhases(opt.Args());
        break;
      case kMeRange:
        meOption->useRange = true;
        meOption->GetRange(opt.Args());
        break;
      case kMeDumpAfter:
        meOption->dumpAfter = true;
        break;
      case kMeDumpFunc:
        meOption->dumpFunc = opt.Args();
        break;
      case kMeDumpPhases:
        meOption->SplitPhases(opt.Args(), meOption->dumpPhases);
        break;
      case kMeQuiet:
        meOption->quiet = true;
        break;
      case kSetCalleeHasSideEffect:
        meOption->setCalleeHasSideEffect = true;
        break;
      case kNoSteensgaard:
        meOption->noSteensgaard = true;
        break;
      case kNoTBAA:
        meOption->noTBAA = true;
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
        break;
      case kMeNoDot:
        meOption->noDot = true;
        break;
      case kStmtNum:
        meOption->stmtNum = true;
        break;
      case kLessThrowAlias:
        meOption->lessThrowAlias = true;
        break;
      case kFinalFieldAlias:
        meOption->finalFieldAlias = true;
        break;
      case kRegReadAtReturn:
        meOption->regreadAtReturn = true;
        break;
      default:
        WARN(kLncWarn, "input invalid key for me " + opt.OptionKey());
        break;
    }
  }
  return meOption;
}

Options *MapleCombCompiler::MakeMpl2MplOptions(const MplOptions &options, maple::MemPool &optMp) {
  Options *mpl2mplOption = new Options(optMp);
  auto inputOptions = options.GetExeOptions().find(kBinNameMpl2mpl);
  if (inputOptions == options.GetExeOptions().end()) {
    LogInfo::MapleLogger() << "no mpl2mpl input options" << '\n';
    return mpl2mplOption;
  }
  for (auto &opt : inputOptions->second) {
    if (options.HasSetDebugFlag()) {
      LogInfo::MapleLogger() << "mpl2mpl options: " << opt.Index() << " " << opt.OptionKey() << " " << opt.Args()
                             << '\n';
    }
    switch (opt.Index()) {
      case kMpl2MplDumpBefore:
        mpl2mplOption->dumpBefore = true;
        break;
      case kMpl2MplDumpAfter:
        mpl2mplOption->dumpAfter = true;
        break;
      case kMpl2MplDumpFunc:
        mpl2mplOption->dumpFunc = opt.Args();
        break;
      case kMpl2MplQuiet:
        mpl2mplOption->quiet = true;
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
        mpl2mplOption->regNativeDynamicOnly = true;
        break;
      case kRegNativeStaticBindingList:
        mpl2mplOption->staticBindingList = opt.Args();
        break;
      case kMpl2MplStubJniFunc:
        mpl2mplOption->regNativeFunc = true;
        break;
      case kNativeWrapper:
        mpl2mplOption->nativeWrapper = opt.Type();
        break;
      case kMpl2MplMapleLinker:
        mpl2mplOption->mapleLinker = true;
        mpl2mplOption->dumpMuidFile = true;
        break;
      case kMplnkDumpMuid:
        mpl2mplOption->dumpMuidFile = true;
        break;
      case kEmitVtableImpl:
        mpl2mplOption->emitVtableImpl = true;
        break;
#if MIR_JAVA
      case kMpl2MplSkipVirtual:
        mpl2mplOption->skipVirtualMethod = true;
        break;
#endif
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
  auto iterMe = std::find(options.GetRunningExes().begin(), options.GetRunningExes().end(), kBinNameMe);
  if (iterMe != options.GetRunningExes().end()) {
    meOptions.reset(MakeMeOptions(options, *optMp));
  }
  auto iterMpl2Mpl = std::find(options.GetRunningExes().begin(), options.GetRunningExes().end(), kBinNameMpl2mpl);
  if (iterMpl2Mpl != options.GetRunningExes().end()) {
    mpl2mplOptions.reset(MakeMpl2MplOptions(options, *optMp));
  }

  LogInfo::MapleLogger() << "Starting mpl2mpl&mplme" << '\n';
  PrintCommand(options);
  DriverRunner runner(theModule, options.GetRunningExes(), mpl2mplOptions.get(), fileName, meOptions.get(),
                      fileName, fileName, optMp,
                      options.HasSetTimePhases(), options.HasSetGenMeMpl());
  ErrorCode nErr = runner.Run();

  memPoolCtrler.DeleteMemPool(optMp);
  return nErr;
}
}  // namespace maple
