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
#include "option.h"
#include <iostream>
#include <cstring>
#include <cctype>
#include "mpl_logging.h"
#include "option_parser.h"

namespace maple {
using namespace mapleOption;

bool Options::dumpBefore = false;
bool Options::dumpAfter = false;
std::string Options::dumpPhase = "*";
std::string Options::dumpFunc = "*";
std::string Options::skipPhase;
std::string Options::skipFrom;
std::string Options::skipAfter;
bool Options::quiet = false;
bool Options::regNativeFunc = false;
bool Options::nativeWrapper = true;         // Enabled by default
bool Options::inlineWithProfile = false;
bool Options::useInline = true;             // Enabled by default
bool Options::useCrossModuleInline = true;  // Enabled by default
std::string Options::noInlineFuncList = "";
uint32 Options::inlineSmallFunctionThreshold = 15;
uint32 Options::inlineHotFunctionThreshold = 30;
uint32 Options::inlineModuleGrowth = 10;
uint32 Options::inlineColdFunctionThreshold = 3;
uint32 Options::profileHotCount = 1000;
uint32 Options::profileColdCount = 10;
bool Options::profileHotCountSeted = false;
bool Options::profileColdCountSeted = false;
uint32 Options::profileHotRate = 500000;
uint32 Options::profileColdRate = 900000;
bool Options::regNativeDynamicOnly = false;
std::string Options::staticBindingList;
bool Options::usePreg = false;
bool Options::mapleLinker = false;
bool Options::dumpMuidFile = false;
bool Options::emitVtableImpl = false;
#if MIR_JAVA
bool Options::skipVirtualMethod = false;
#endif
bool Options::checkArrayStore = false;
enum OptionIndex {
  kUnknown,
  kHelp,
  kDumpPhase,
  kSkipPhase,
  kSkipFrom,
  kSkipAfter,
  kDumpFunc,
  kQuiet,
  kStubJniFunc,
  kInlineWithProfile,
  kInlineWithoutProfile,
  kUseInline,
  kNoInline,
  kNoInlineFuncList,
  kUseCrossModuleInline,
  kNoCrossModuleInline,
  kInlineSmallFunctionThreshold,
  kInlineHotFunctionThreshold,
  kInlineModuleGrowth,
  kInlineColdFunctionThreshold,
  kProfileHotCount,
  kProfileColdCount,
  kProfileHotRate,
  kProfileColdRate,
  kRegNativeDynamicOnly,
  kRegNativeStaticBindingList,
  kNativeWrapper,
  kDumpBefore,
  kDumpAfter,
  kSkipVirtual,
  kMapleLinker,
  kMplnkDumpMuid,
  kEmitVtableImpl,
  kCheckArrayStore,
};

const Descriptor kUsage[] = {
  { kUnknown, 0, "", "", kBuildTypeAll, kArgCheckPolicyUnknown,
    "USAGE:  mplxxx [options] *.mpl\n\n"
    "OPTIONS:" },
  { kHelp, 0, "h", "help", kBuildTypeAll, kArgCheckPolicyNone,
    "  -h, --help                        Print usage and exit" },
  { kDumpPhase, 0, "", "dump-phase", kBuildTypeAll, kArgCheckPolicyRequired,
    "  --dump-phase=PHASENAME            Enable debug trace for specified phase (can only specify once)" },
  { kSkipPhase, 0, "", "skip-phase", kBuildTypeAll, kArgCheckPolicyRequired,
    "  --skip-phase=PHASENAME            Skip the phase when adding it to phase manager" },
  { kSkipFrom, 0, "", "skip-from", kBuildTypeAll, kArgCheckPolicyRequired,
    "  --skip-from=PHASENAME             Skip all remaining phases including PHASENAME" },
  { kSkipAfter, 0, "", "skip-after", kBuildTypeAll, kArgCheckPolicyRequired,
    "  --skip-after=PHASENAME            Skip all remaining phases after PHASENAME" },
  { kDumpFunc, 0, "", "dump-func", kBuildTypeAll, kArgCheckPolicyRequired,
    "  --dump-func=FUNCNAME              Dump/trace only for functions whose names contain FUNCNAME as substring\n"
    "                                    (can only specify once)" },
  { kQuiet, 0, "", "quiet", kBuildTypeAll, kArgCheckPolicyNone,
    "  --quiet                           Disable brief trace messages with phase/function names" },
  { kStubJniFunc, 0, "", "regnativefunc", kBuildTypeAll, kArgCheckPolicyNone,
    "  --regnativefunc                   Generate native stub function to support JNI registration and calling" },
  { kInlineWithProfile, 0, "", "inline-with-profile", kBuildTypeAll, kArgCheckPolicyNone,
    "  --inline-with-profile             Enable profile-based inlining" },
  { kInlineWithoutProfile, 0, "", "inline-without-profile", kBuildTypeAll, kArgCheckPolicyNone,
    "  --inline-without-profile          Disable profile-based inlining" },
  { kUseInline, 0, "", "inline", kBuildTypeAll, kArgCheckPolicyNone,
    "  --inline                          Enable function inlining" },
  { kNoInline, 0, "", "no-inline", kBuildTypeAll, kArgCheckPolicyNone,
    "  --no-inline                       Disable function inlining" },
  { kNoInlineFuncList, 0, "", "no-inlinefunclist", kBuildTypeAll, kArgCheckPolicyRequired,
    "  --no-inlinefunclist=list          Do not inline function in this list" },
  { kUseCrossModuleInline, 0, "", "cross-module-inline", kBuildTypeAll, kArgCheckPolicyNone,
    "  --cross-module-inline             Enable cross-module inlining" },
  { kNoCrossModuleInline, 0, "", "no-cross-module-inline", kBuildTypeAll, kArgCheckPolicyNone,
    "  --no-cross-module-inline          Disable cross-module inlining" },
  { kInlineSmallFunctionThreshold, 0, "", "inline-small-function-threshold", kBuildTypeAll, kArgCheckPolicyRequired,
    "  --inline-small-function-threshold=15        Threshold for inlining small function" },
  { kInlineHotFunctionThreshold, 0, "", "inline-hot-function-threshold", kBuildTypeAll, kArgCheckPolicyRequired,
    "  --inline-hot-function-threshold=30          Threshold for inlining hot function" },
  { kInlineModuleGrowth, 0, "", "inline-module-growth", kBuildTypeAll, kArgCheckPolicyRequired,
    "  --inline-module-growth=100000               Threshold for maxmium code size growth rate. (10%)" },
  { kInlineColdFunctionThreshold, 0, "", "inline-cold-function-threshold", kBuildTypeAll, kArgCheckPolicyRequired,
    "  --inline-cold-function-threshold=3          Threshold for inlining cold function" },
  { kProfileHotCount, 0, "", "profile-hot-count", kBuildTypeAll, kArgCheckPolicyRequired,
    "  --profile-hot-count=1000          A count is regarded as hot if it exceeds this number" },
  { kProfileColdCount, 0, "", "profile-cold-count", kBuildTypeAll, kArgCheckPolicyRequired,
    "  --profile-cold-count=10           A count is regarded as cold if it is below this number" },
  { kProfileHotRate, 0, "", "profile-hot-rate", kBuildTypeAll, kArgCheckPolicyRequired,
    "  --profile-hot-rate=500000         A count is regarded as hot if it is in the largest 50%" },
  { kProfileColdRate, 0, "", "profile-cold-rate", kBuildTypeAll, kArgCheckPolicyRequired,
    "  --profile-cold-rate=900000        A count is regarded as cold if it is in the smallest 10%" },
  { kNativeWrapper, 1, "", "nativewrapper", kBuildTypeAll, kArgCheckPolicyNone,
    "  --nativewrapper                   Generate native wrappers [default]" },
  { kNativeWrapper, 0, "", "no-nativewrapper", kBuildTypeAll, kArgCheckPolicyNone,
    "  --no-nativewrapper                Do not generate native wrappers" },
  { kRegNativeDynamicOnly, 0, "", "regnative-dynamic-only", kBuildTypeAll, kArgCheckPolicyNone,
    "  --regnative-dynamic-only          Only Generate dynamic register code, Report Fatal Msg if no implemented" },
  { kRegNativeStaticBindingList, 0, "", "static-binding-list", kBuildTypeAll, kArgCheckPolicyRequired,
    "  --static-bindig-list=file         Only Generate static binding function in file configure list" },
  { kDumpBefore, 0, "", "dump-before", kBuildTypeAll, kArgCheckPolicyNone,
    "  --dump-before                     Do extra IR dump before the specified phase" },
  { kDumpAfter, 0, "", "dump-after", kBuildTypeAll, kArgCheckPolicyNone,
    "  --dump-after                      Do extra IR dump after the specified phase" },
  { kMapleLinker, 0, "", "maplelinker", kBuildTypeAll, kArgCheckPolicyNone,
    "  --maplelinker                     Generate MUID symbol tables and references" },
  { kMplnkDumpMuid, 0, "", "dump-muid", kBuildTypeAll, kArgCheckPolicyNone,
    "  --dump-muid                       Dump MUID def information into a .muid file" },
  { kEmitVtableImpl, 0, "", "emitVtableImpl", kBuildTypeAll, kArgCheckPolicyNone,
    "  --emitVtableImpl                  Generate VtableImpl file" },
#if MIR_JAVA
  { kSkipVirtual, 0, "", "skipvirtual", kBuildTypeAll, kArgCheckPolicyNone, "  --skipvirtual" },
#endif
  { kCheckArrayStore, 0, "", "check-array-store", kBuildTypeAll, kArgCheckPolicyNone,
    "  --check-array-store               Check array store[default off]" },
  { 0, 0, nullptr, nullptr, kBuildTypeAll, kArgCheckPolicyNone, nullptr }
};

bool Options::ParseOptions(int argc, char **argv, std::string &fileName) const {
  bool result = true;
  OptionParser optionParser(kUsage);
  int ret = optionParser.Parse(argc, argv);
  CHECK_FATAL(ret == kErrorNoError, "option parser error");
  for (auto opt : optionParser.GetOptions()) {
    switch (opt.Index()) {
      case kHelp: {
        if (opt.Args().empty()) {
          optionParser.PrintUsage();
          result = false;
        }
        break;
      }
      case kDumpBefore:
        Options::dumpBefore = true;
        break;
      case kDumpAfter:
        Options::dumpAfter = true;
        break;
      case kDumpFunc:
        Options::dumpFunc = opt.Args();
        break;
      case kDumpPhase:
        Options::dumpPhase = opt.Args();
        break;
      case kSkipPhase:
        Options::skipPhase = opt.Args();
        break;
      case kSkipFrom:
        Options::skipFrom = opt.Args();
        break;
      case kSkipAfter:
        Options::skipAfter = opt.Args();
        break;
      case kQuiet:
        Options::quiet = true;
        break;
      case kRegNativeDynamicOnly:
        Options::regNativeDynamicOnly = true;
        break;
      case kRegNativeStaticBindingList:
        Options::staticBindingList = opt.Args();
        break;
      case kStubJniFunc:
        Options::regNativeFunc = true;
        break;
      case kInlineWithProfile:
        Options::inlineWithProfile = true;
        break;
      case kInlineWithoutProfile:
        Options::inlineWithProfile = false;
        break;
      case kUseInline:
        Options::useInline = true;
        break;
      case kNoInline:
        Options::useInline = false;
        break;
      case kNoInlineFuncList:
        Options::noInlineFuncList = opt.Args();
        break;
      case kUseCrossModuleInline:
        Options::useCrossModuleInline = true;
        break;
      case kNoCrossModuleInline:
        Options::useCrossModuleInline = false;
        break;
      case kInlineSmallFunctionThreshold:
        Options::inlineSmallFunctionThreshold = std::stoul(opt.Args());
        break;
      case kInlineHotFunctionThreshold:
        Options::inlineHotFunctionThreshold = std::stoul(opt.Args());
        break;
      case kInlineModuleGrowth:
        Options::inlineModuleGrowth = std::stoul(opt.Args());
        break;
      case kInlineColdFunctionThreshold:
        Options::inlineColdFunctionThreshold = std::stoul(opt.Args());
        break;
      case kProfileHotCount:
        Options::profileHotCount = std::stoul(opt.Args());
        Options::profileHotCountSeted = true;
        break;
      case kProfileColdCount:
        Options::profileColdCount = std::stoul(opt.Args());
        Options::profileColdCountSeted = true;
        break;
      case kProfileHotRate:
        Options::profileHotRate = std::stoul(opt.Args());
        break;
      case kProfileColdRate:
        Options::profileColdRate = std::stoul(opt.Args());
        break;
      case kNativeWrapper:
        Options::nativeWrapper = opt.Type();
        break;
      case kMapleLinker:
        Options::mapleLinker = true;
        Options::dumpMuidFile = false;
        break;
      case kMplnkDumpMuid:
        Options::dumpMuidFile = true;
        break;
      case kEmitVtableImpl:
        Options::emitVtableImpl = true;
        break;
#if MIR_JAVA
      case kSkipVirtual:
        Options::skipVirtualMethod = true;
        break;
#endif
      case kCheckArrayStore:
        Options::checkArrayStore = true;
        break;
      default:
        result = false;
        ASSERT(false, "unhandled case in Options");
    }
  }

  if (result) {
    if (optionParser.GetNonOptionsCount() != 1) {
      LogInfo::MapleLogger(kLlErr) << "expecting one .mpl file as last argument, found: ";
      for (const auto &optionArg : optionParser.GetNonOptions()) {
        LogInfo::MapleLogger(kLlErr) << optionArg << " ";
      }
      LogInfo::MapleLogger(kLlErr) << "\n";
      result = false;
    }

    if (result) {
      fileName = optionParser.GetNonOptions().front();
    }
  }
  return result;
}

void Options::DumpOptions() const {
  LogInfo::MapleLogger() << "phase sequence : \t";
  if (phaseSeq.empty()) {
    LogInfo::MapleLogger() << "default phase sequence\n";
  } else {
    for (size_t i = 0; i < phaseSeq.size(); ++i) {
      LogInfo::MapleLogger() << " " << phaseSeq[i];
    }
  }
  LogInfo::MapleLogger() << "\n";
}
};  // namespace maple
