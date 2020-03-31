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
#include "usages.h"
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

void MplcgCompiler::DecideMplcgRealLevel(CGOptions &cgOption,
                                         const std::vector<mapleOption::Option> &inputOptions,
                                         const MplOptions &options) {
  int realLevel = -1;
  for (const mapleOption::Option &opt : inputOptions) {
    switch (opt.Index()) {
      case kCGO0:
        realLevel = CGOptions::kLevel0;
        break;
      case kCGO1:
        realLevel = CGOptions::kLevel1;
        break;
      case kCGO2:
        realLevel = CGOptions::kLevel2;
        break;
      default:
        break;
    }
  }
  if (options.HasSetDebugFlag()) {
    LogInfo::MapleLogger() << "Real Mplcg level:" << std::to_string(realLevel) << "\n";
  }
  if (realLevel ==  CGOptions::kLevel0) {
    cgOption.EnableO0();
  } else if (realLevel ==  CGOptions::kLevel1) {
    cgOption.EnableO1();
  } else if (realLevel ==  CGOptions::kLevel2) {
    cgOption.EnableO2();
  }
}

CGOptions *MplcgCompiler::MakeCGOptions(const MplOptions &options, MemPool &optMp) {
  auto *cgOption = new CGOptions(optMp);
  cgOption->SetOption(CGOptions::kDefaultOptions);
  cgOption->SetOption(CGOptions::kWithMpl);
  cgOption->SetGenerateFlags(CGOptions::kDefaultGflags);
  auto it = options.GetExeOptions().find(kBinNameMplcg);
  if (it == options.GetExeOptions().end()) {
    LogInfo::MapleLogger() << "no me input options\n";
    return cgOption;
  }
  DecideMplcgRealLevel(*cgOption, it->second, options);
  for (const mapleOption::Option &opt : it->second) {
    if (options.HasSetDebugFlag()) {
      LogInfo::MapleLogger() << "mplcg options: " << opt.Index() << " " << opt.OptionKey() << " " << opt.Args()
                             << '\n';
    }
    switch (opt.Index()) {
      case kCGQuiet:
        cgOption->SetQuiet((opt.Type() == kEnable));
        break;
      case kPie:
        (opt.Type() == kEnable) ? cgOption->SetOption(CGOptions::kGenPie)
                                : cgOption->ClearOption(CGOptions::kGenPie);
        break;
      case kPic: {
        if (opt.Type() == kEnable) {
          cgOption->EnablePIC();
          cgOption->SetOption(CGOptions::kGenPic);
        } else {
          cgOption->DisablePIC();
          cgOption->ClearOption(CGOptions::kGenPic);
        }
        break;
      }
      case kVerbose:
        (opt.Type() == kEnable) ? cgOption->SetOption(CGOptions::kVerboseAsm)
                                : cgOption->ClearOption(CGOptions::kVerboseAsm);
        break;
      case kCGMapleLinker:
        (opt.Type() == kEnable) ? cgOption->EnableMapleLinker() : cgOption->DisableMapleLinker();
        break;
      case kCGBarrier:
        (opt.Type() == kEnable) ? cgOption->EnableBarriersForVolatile() : cgOption->DisableBarriersForVolatile();
        break;
      case kCGDumpBefore:
        (opt.Type() == kEnable) ? cgOption->EnableDumpBefore() : cgOption->DisableDumpBefore();
        break;
      case kCGDumpAfter:
        (opt.Type() == kEnable) ? cgOption->EnableDumpAfter() : cgOption->DisableDumpAfter();
        break;
      case kCGTimePhases:
        (opt.Type() == kEnable) ? cgOption->EnableTimePhases() : cgOption->DisableTimePhases();
        break;
      case kCGDumpFunc:
        cgOption->SetDumpFunc(opt.Args());
        break;
      case kDuplicateToDelPlt:
        cgOption->SetDuplicateAsmFile(opt.Args());
        break;
      case kInsertCall:
        cgOption->SetInstrumentationFunction(opt.Args());
        cgOption->SetInsertCall(true);
        break;
      case kStackGuard:
        cgOption->SetOption(cgOption->kUseStackGuard);
        break;
      case kDebuggingInfo:
        cgOption->SetOption(cgOption->kDebugFriendly);
        cgOption->SetOption(cgOption->kWithLoc);
        cgOption->ClearOption(cgOption->kSuppressFileInfo);
        break;
      case kDebugGenDwarf:
        cgOption->SetOption(cgOption->kDebugFriendly);
        cgOption->SetOption(cgOption->kWithLoc);
        cgOption->SetOption(cgOption->kWithDwarf);
        cgOption->SetParserOption(kWithDbgInfo);
        cgOption->ClearOption(cgOption->kSuppressFileInfo);
        cgOption->EnableWithDwarf();
        break;
      case kDebugUseSrc:
        cgOption->SetOption(cgOption->kDebugFriendly);
        cgOption->SetOption(cgOption->kWithLoc);
        cgOption->SetOption(cgOption->kWithSrc);
        cgOption->ClearOption(cgOption->kWithMpl);
        break;
      case kDebugUseMix:
        cgOption->SetOption(cgOption->kDebugFriendly);
        cgOption->SetOption(cgOption->kWithLoc);
        cgOption->SetOption(cgOption->kWithSrc);
        cgOption->SetOption(cgOption->kWithMpl);
        break;
      case kDebugAsmMix:
        cgOption->SetOption(cgOption->kDebugFriendly);
        cgOption->SetOption(cgOption->kWithLoc);
        cgOption->SetOption(cgOption->kWithSrc);
        cgOption->SetOption(cgOption->kWithMpl);
        cgOption->SetOption(cgOption->kWithAsm);
        break;
      case kProfilingInfo:
        cgOption->SetOption(cgOption->kWithProfileCode);
        cgOption->SetParserOption(kWithProfileInfo);
        break;
      case kRaColor:
        cgOption->SetOption(cgOption->kDoColorRegAlloc);
        cgOption->ClearOption(cgOption->kDoLinearScanRegAlloc);
        break;
      case kPrintFunction:
        (opt.Type() == kEnable) ? cgOption->EnablePrintFunction() : cgOption->DisablePrintFunction();
        break;
      case kTrace:
        cgOption->SetOption(cgOption->kAddDebugTrace);
        break;
      case kProfileEnable:
        cgOption->SetOption(cgOption->kAddFuncProfile);
        break;
      case kSuppressFinfo:
        cgOption->SetOption(cgOption->kSuppressFileInfo);
        break;
      case kConstFoldOpt:
        cgOption->SetOption(cgOption->kConstFold);
        break;
      case kCGDumpcfg:
        cgOption->SetOption(cgOption->kDumpCFG);
        break;
      case kCGClassList:
        cgOption->SetClassListFile(opt.Args());
        break;
      case kGenDef:
        cgOption->SetOrClear(cgOption->GetGenerateFlags(), CGOptions::kCMacroDef, opt.Type());
        break;
      case kGenGctib:
        cgOption->SetOrClear(cgOption->GetGenerateFlags(), CGOptions::kGctib, opt.Type());
        break;
      case kGenPrimorList:
        cgOption->SetOrClear(cgOption->GetGenerateFlags(), CGOptions::kPrimorList, opt.Type());
        break;
      case kYieldPoing:
        cgOption->SetOrClear(cgOption->GetGenerateFlags(), CGOptions::kGenYieldPoint, opt.Type());
        break;
      case kLocalRc:
        cgOption->SetOrClear(cgOption->GetGenerateFlags(), CGOptions::kGenLocalRc, opt.Type());
        break;
      case kEhList: {
        const std::string &ehList = opt.Args();
        cgOption->SetEHExclusiveFile(ehList);
        cgOption->EnableExclusiveEH();
        cgOption->ParseExclusiveFunc(ehList);
        break;
      }
      case kCyclePatternList: {
        const std::string &patternList = opt.Args();
        cgOption->SetCyclePatternFile(patternList);
        cgOption->EnableEmitCyclePattern();
        cgOption->ParseCyclePattern(patternList);
        break;
      }
      case kCgen: {
        bool cgFlag = (opt.Type() == kEnable);
        cgOption->SetRunCGFlag(cgFlag);
        cgFlag ? cgOption->SetOption(CGOptions::kDoCg) : cgOption->ClearOption(CGOptions::kDoCg);
        break;
      }
      case kObjMap:
        cgOption->SetGenerateObjectMap(opt.Type() == kEnable);
        break;
      case kCGLazyBinding:
        (opt.Type() == kEnable) ? cgOption->EnableLazyBinding() : cgOption->DisableLazyBinding();
        break;
      case kCGHotFix:
        (opt.Type() == kEnable) ? cgOption->EnableHotFix() : cgOption->DisableHotFix();
        break;
      case kInsertSoe:
        cgOption->SetOption(CGOptions::kSoeCheckInsert);
        break;
      case kCheckArrayStore:
        (opt.Type() == kEnable) ? cgOption->EnableCheckArrayStore() : cgOption->DisableCheckArrayStore();
        break;
      case kCGNativeOpt:
        cgOption->DisableNativeOpt();
        break;
      case kCalleeCFI:
        (opt.Type() == kEnable) ? cgOption->DisableNoCalleeCFI() : cgOption->EnableNoCalleeCFI();
        break;
      case kProepilogue:
        (opt.Type() == kEnable) ? cgOption->SetOption(CGOptions::kProEpilogueOpt)
                                : cgOption->ClearOption(CGOptions::kProEpilogueOpt);
        break;
      case kCGO0:
        // Already handled above in DecideMplcgRealLevel
        break;
      case kCGO1:
        // Already handled above in DecideMplcgRealLevel
        break;
      case kCGO2:
        // Already handled above in DecideMplcgRealLevel
        break;
      case kCGDumpPhases:
        cgOption->SplitPhases(opt.Args(), cgOption->GetDumpPhases());
        break;
      case kCGSkipPhases:
        cgOption->SplitPhases(opt.Args(), cgOption->GetSkipPhases());
        break;
      case kCGSkipFrom:
        cgOption->SetSkipFrom(opt.Args());
        break;
      case kCGSkipAfter:
        cgOption->SetSkipAfter(opt.Args());
        break;
      case kLongCalls:
        (opt.Type() == kEnable) ? cgOption->EnableLongCalls() : cgOption->DisableLongCalls();
        break;
      case kGcOnly:
        (opt.Type() == kEnable) ? cgOption->EnableGCOnly() : cgOption->DisableGCOnly();
        break;
      default:
        WARN(kLncWarn, "input invalid key for mplcg " + opt.OptionKey());
        break;
    }
  }
  return cgOption;
}

ErrorCode MplcgCompiler::Compile(const MplOptions &options, MIRModulePtr &theModule) {
  MemPool *optMp = memPoolCtrler.NewMemPool("maplecg mempool");
  std::unique_ptr<CGOptions> cgOption(MakeCGOptions(options, *optMp));
  if (cgOption == nullptr) {
    return kErrorCompileFail;
  }
  std::string fileName = GetInputFileName(options);
  std::string baseName = options.GetOutputFolder() + FileUtils::GetFileName(fileName, false);
  std::string output = baseName + ".s";
  bool parsed = false;
  std::unique_ptr<MIRParser> theParser;
  if (theModule == nullptr) {
    theModule = new MIRModule(fileName);
    theModule->SetWithMe(
        std::find(options.GetRunningExes().begin(), options.GetRunningExes().end(),
                  kBinNameMe) != options.GetRunningExes().end());
    theParser.reset(new MIRParser(*theModule));
    parsed = theParser->ParseMIR(0, cgOption->GetParserOption());
    if (parsed) {
      if (!cgOption->IsQuiet() && theParser->GetWarning().size()) {
        theParser->EmitWarning(fileName);
      }
    } else {
      if (theParser != nullptr) {
        theParser->EmitError(fileName);
      }
      memPoolCtrler.DeleteMemPool(optMp);
      return kErrorCompileFail;
    }
  }

  LogInfo::MapleLogger() << "Starting mplcg\n";
  DriverRunner runner(theModule, options.GetRunningExes(), fileName, optMp, options.HasSetTimePhases());
  PrintCommand(options);
  runner.SetCGInfo(cgOption.get(), fileName);
  runner.ProcessCGPhase(output, baseName);

  memPoolCtrler.DeleteMemPool(optMp);
  return kErrorNoError;
}
}  // namespace maple
