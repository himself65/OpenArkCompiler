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
#include "cg_option.h"
#include <fstream>
#include <string>
#include <unordered_map>
#include "mpl_logging.h"
#include "parser_opt.h"
#include "mir_parser.h"
#include "option_parser.h"
#include "string_utils.h"

namespace maplebe {
using namespace maple;
using namespace mapleOption;

const std::string kMplcgVersion = "";

bool CGOptions::dumpBefore = false;
bool CGOptions::dumpAfter = false;
bool CGOptions::timePhases = false;
std::unordered_set<std::string> CGOptions::dumpPhases = {};
std::unordered_set<std::string> CGOptions::skipPhases = {};
std::unordered_map<std::string, std::vector<std::string>> CGOptions::cyclePatternMap = {};
std::string CGOptions::skipFrom = "";
std::string CGOptions::skipAfter = "";
std::string CGOptions::dumpFunc = "*";
std::string CGOptions::duplicateAsmFile = "maple/mrt/codetricks/arch/arm64/duplicateFunc.s";
std::string CGOptions::globalVarProfile = "";
#if TARGAARCH64
bool CGOptions::useBarriersForVolatile = false;
#else
bool CGOptions::useBarriersForVolatile = true;
#endif
bool CGOptions::quiet = true;
bool CGOptions::exclusiveEH = false;
bool CGOptions::checkArrayStore = false;
bool CGOptions::doPIC = false;
bool CGOptions::noCalleeCFI = true;
bool CGOptions::emitCyclePattern = false;
bool CGOptions::insertYieldPoint = false;
bool CGOptions::mapleLinker = false;
bool CGOptions::printFunction = false;
bool CGOptions::nativeOpt = false;
bool CGOptions::withDwarf = false;
bool CGOptions::lazyBinding = false;
bool CGOptions::hotFix = false;
bool CGOptions::genLongCalls = false;
bool CGOptions::gcOnly = false;

enum OptionIndex : uint64 {
  kCGQuiet = kCommonOptionEnd + 1,
  kPie,
  kPic,
  kCGVerbose,
  kCGMapleLinker,
  kCgen,
  kCGNativeOpt,
  kInsertCall,
  kTrace,
  kCGClassList,
  kGenDef,
  kGenGctib,
  kCGBarrier,
  kGenPrimorList,
  kRaColor,
  kConstFoldOpt,
  kSuppressFinfo,
  kEhList,
  kObjMap,
  kCGDumpcfg,
  kCGDumpBefore,
  kCGDumpAfter,
  kCGTimePhases,
  kCGDumpFunc,
  kDebuggingInfo,
  kStackGuard,
  kDebugGenDwarf,
  kDebugUseSrc,
  kDebugUseMix,
  kDebugAsmMix,
  kProfilingInfo,
  kProfileEnable,
  kCGO0,
  kCGO1,
  kCGO2,
  kProepilogue,
  kYieldPoing,
  kLocalRc,
  kCalleeCFI,
  kCyclePatternList,
  kDuplicateToDelPlt,
  kInsertSoe,
  kCheckArrayStore,
  kPrintFunction,
  kCGDumpPhases,
  kCGSkipPhases,
  kCGSkipFrom,
  kCGSkipAfter,
  kCGLazyBinding,
  kCGHotFix,
  kLongCalls,
};

const Descriptor kUsage[] = {
  { kPie,
    kEnable,
    nullptr,
    "pie",
    kBuildTypeProduct,
    kArgCheckPolicyBool,
    "  --pie                       \tGenerate position-independent executable\n"
    "  --no-pie\n",
    "mplcg",
    {} },
  { kPic,
    kEnable,
    nullptr,
    "fpic",
    kBuildTypeProduct,
    kArgCheckPolicyBool,
    "  --fpic                      \tGenerate position-independent shared library\n"
    "  --no-fpic\n",
    "mplcg",
    {} },
  { kCGVerbose,
    kEnable,
    nullptr,
    "verbose-asm",
    kBuildTypeProduct,
    kArgCheckPolicyBool,
    "  --verbose-asm               \tAdd comments to asm output\n"
    "  --no-verbose-asm\n",
    "mplcg",
    {} },
  { kCGMapleLinker,
    kEnable,
    nullptr,
    "maplelinker",
    kBuildTypeProduct,
    kArgCheckPolicyBool,
    "  --maplelinker               \tGenerate the MapleLinker .s format\n"
    "  --no-maplelinker\n",
    "mplcg",
    {} },
  { kCGQuiet,
    kEnable,
    nullptr,
    "quiet",
    kBuildTypeExperimental,
    kArgCheckPolicyBool,
    "  --quiet                     \tBe quiet (don't output debug messages)\n"
    "  --no-quiet\n",
    "mplcg",
    {} },
  { kCgen,
    kEnable,
    nullptr,
    "cg",
    kBuildTypeExperimental,
    kArgCheckPolicyBool,
    "  --cg                        \tGenerate the output .s file\n"
    "  --no-cg\n",
    "mplcg",
    {} },
  { kCGLazyBinding,
    kEnable,
    nullptr,
    "lazy-binding",
    kBuildTypeProduct,
    kArgCheckPolicyBool,
    "  --lazy-binding              \tBind class symbols lazily[default off]\n",
    "mplcg",
    {} },
  { kCGHotFix,
    kEnable,
    nullptr,
    "hot-fix",
    kBuildTypeExperimental,
    kArgCheckPolicyBool,
    "  --hot-fix                   \tOpen for App hot fix[default off]\n"
    "  --no-hot-fix\n",
    "mplcg",
    {} },
  { kCGNativeOpt,
    kEnable,
    nullptr,
    "nativeopt",
    kBuildTypeProduct,
    kArgCheckPolicyBool,
    "  --nativeopt                 \tEnable native opt\n"
    "  --no-nativeopt\n",
    "mplcg",
    {} },
  { kObjMap,
    kEnable,
    nullptr,
    "objmap",
    kBuildTypeExperimental,
    kArgCheckPolicyBool,
    "  --objmap                    \tCreate object maps (GCTIBs) inside the main output (.s) file\n"
    "  --no-objmap\n",
    "mplcg",
    {} },
  { kYieldPoing,
    kEnable,
    nullptr,
    "yieldpoint",
    kBuildTypeExperimental,
    kArgCheckPolicyBool,
    "  --yieldpoint                \tGenerate yieldpoints [default]\n"
    "  --no-yieldpoint\n",
    "mplcg",
    {} },
  { kProepilogue,
    kEnable,
    nullptr,
    "proepilogue",
    kBuildTypeExperimental,
    kArgCheckPolicyBool,
    "  --proepilogue               \tDo tail call optimization and eliminate unnecessary prologue and epilogue.\n"
    "  --no-proepilogue\n",
    "mplcg",
    {} },
  { kLocalRc,
    kEnable,
    nullptr,
    "local-rc",
    kBuildTypeExperimental,
    kArgCheckPolicyBool,
    "  --local-rc                  \tHandle Local Stack RC [default]\n"
    "  --no-local-rc\n",
    "mplcg",
    {} },
  { kInsertCall,
    0,
    nullptr,
    "insert-call",
    kBuildTypeExperimental,
    kArgCheckPolicyRequired,
    "  --insert-call=name          \tInsert a call to the named function\n",
    "mplcg",
    {} },
  { kTrace,
    0,
    nullptr,
    "add-debug-trace",
    kBuildTypeProduct,
    kArgCheckPolicyNone,
    "  --add-debug-trace           \tInstrument the output .s file to print call traces at runtime\n",
    "mplcg",
    {} },
  { kCGClassList,
    0,
    nullptr,
    "class-list-file",
    kBuildTypeExperimental,
    kArgCheckPolicyRequired,
    "  --class-list-file           \tSet the class list file for the following generation options,\n"
    "                              \tif not given, generate for all visible classes\n"
    "                              \t--class-list-file=class_list_file\n",
    "mplcg",
    {} },
  { kGenDef,
    kEnable,
    nullptr,
    "gen-c-macro-def",
    kBuildTypeProduct,
    kArgCheckPolicyBool,
    "  --gen-c-macro-def           \tGenerate a .def file that contains extra type metadata, including the\n"
    "                              \tclass instance sizes and field offsets (default)\n"
    "  --no-gen-c-macro-def\n",
    "mplcg",
    {} },
  { kGenGctib,
    kEnable,
    nullptr,
    "gen-gctib-file",
    kBuildTypeExperimental,
    kArgCheckPolicyBool,
    "  --gen-gctib-file            \tGenerate a separate .s file for GCTIBs. Usually used together with\n"
    "                              \t--no-objmap (not implemented yet)\n"
    "  --no-gen-gctib-file\n",
    "mplcg",
    {} },
  { kStackGuard,
    kEnable,
    nullptr,
    "stackguard",
    kBuildTypeExperimental,
    kArgCheckPolicyBool,
    "  -stackguard                 \tadd stack guard\n"
    "  -no-stackguard\n",
    "mplcg",
    {} },
  { kDebuggingInfo,
    0,
    "g",
    nullptr,
    kBuildTypeExperimental,
    kArgCheckPolicyNone,
    "  -g                          \tGenerate debug information\n",
    "mplcg",
    {} },
  { kDebugGenDwarf,
    0,
    nullptr,
    "gdwarf",
    kBuildTypeProduct,
    kArgCheckPolicyNone,
    "  --gdwarf                    \tGenerate dwarf infomation\n",
    "mplcg",
    {} },
  { kDebugUseSrc,
    0,
    nullptr,
    "gsrc",
    kBuildTypeProduct,
    kArgCheckPolicyNone,
    "  --gsrc                      \tUse original source file instead of mpl file for debugging\n",
    "mplcg",
    {} },
  { kDebugUseMix,
    0,
    nullptr,
    "gmixedsrc",
    kBuildTypeProduct,
    kArgCheckPolicyNone,
    "  --gmixedsrc                 \tUse both original source file and mpl file for debugging\n",
    "mplcg",
    {} },
  { kDebugAsmMix,
    0,
    nullptr,
    "gmixedasm",
    kBuildTypeExperimental,
    kArgCheckPolicyNone,
    "  --gmixedasm                 \tComment out both original source file and mpl file for debugging\n",
    "mplcg",
    {} },
  { kRaColor,
    0,
    nullptr,
    "with-ra-graph-color",
    kBuildTypeExperimental,
    kArgCheckPolicyNone,
    "  --with-ra-graph-color       \tDo coloring-based register allocation\n",
    "mplcg",
    {} },
  { kConstFoldOpt,
    0,
    nullptr,
    "const-fold",
    kBuildTypeExperimental,
    kArgCheckPolicyNone,
    "  --const-fold                \tEnable constant folding\n",
    "mplcg",
    {} },
  { kEhList,
    0,
    nullptr,
    "eh-exclusive-list",
    kBuildTypeExperimental,
    kArgCheckPolicyRequired,
    "  --eh-exclusive-list         \tFor generating gold files in unit testing\n"
    "                              \t--eh-exclusive-list=list_file\n",
    "mplcg",
    {} },
  { kCGO0,
    0,
    nullptr,
    "O0",
    kBuildTypeExperimental,
    kArgCheckPolicyNone,
    "  -O0                         \tNo optimization.\n",
    "mplcg",
    {} },
  { kCGO1,
    0,
    nullptr,
    "O1",
    kBuildTypeExperimental,
    kArgCheckPolicyOptional,
    "  -O1                         \tDo some optimization.\n",
    "mplcg",
    {} },
  { kCGO2,
    0,
    nullptr,
    "O2",
    kBuildTypeProduct,
    kArgCheckPolicyOptional,
    "  -O2                          \tDo some optimization.\n",
    "mplcg",
    {} },
  { kSuppressFinfo,
    0,
    nullptr,
    "suppress-fileinfo",
    kBuildTypeExperimental,
    kArgCheckPolicyNone,
    "  --suppress-fileinfo         \tFor generating gold files in unit testing\n",
    "mplcg",
    {} },
  { kCGDumpcfg,
    0,
    nullptr,
    "dump-cfg",
    kBuildTypeExperimental,
    kArgCheckPolicyNone,
    "  --dump-cfg\n",
    "mplcg",
    {} },
  { kCGDumpPhases,
    0,
    nullptr,
    "dump-phases",
    kBuildTypeExperimental,
    kArgCheckPolicyRequired,
    "  --dump-phases=PHASENAME,... \tEnable debug trace for specified phases in the comma separated list\n",
    "mplcg",
    {} },
  { kCGSkipPhases,
    0,
    nullptr,
    "skip-phases",
    kBuildTypeExperimental,
    kArgCheckPolicyRequired,
    "  --skip-phases=PHASENAME,... \tSkip the phases specified in the comma separated list\n",
    "mplcg",
    {} },
  { kCGSkipFrom,
    0,
    nullptr,
    "skip-from",
    kBuildTypeExperimental,
    kArgCheckPolicyRequired,
    "  --skip-from=PHASENAME       \tSkip the rest phases from PHASENAME(included)\n",
    "mplcg",
    {} },
  { kCGSkipAfter,
    0,
    nullptr,
    "skip-after",
    kBuildTypeExperimental,
    kArgCheckPolicyRequired,
    "  --skip-after=PHASENAME      \tSkip the rest phases after PHASENAME(excluded)\n",
    "mplcg",
    {} },
  { kCGDumpFunc,
    0,
    nullptr,
    "dump-func",
    kBuildTypeExperimental,
    kArgCheckPolicyRequired,
    "  --dump-func=FUNCNAME        \tDump/trace only for functions whose names contain FUNCNAME as substring\n"
    "                              \t(can only specify once)\n",
    "mplcg",
    {} },
  { kCGDumpBefore,
    kEnable,
    nullptr,
    "dump-before",
    kBuildTypeExperimental,
    kArgCheckPolicyBool,
    "  --dump-before               \tDo extra IR dump before the specified phase\n"
    "  --no-dump-before            \tDon't extra IR dump before the specified phase\n",
    "mplcg",
    {} },
  { kCGDumpAfter,
    kEnable,
    nullptr,
    "dump-after",
    kBuildTypeExperimental,
    kArgCheckPolicyBool,
    "  --dump-after                \tDo extra IR dump after the specified phase\n"
    "  --no-dump-after             \tDon't extra IR dump after the specified phase\n",
    "mplcg",
    {} },
  { kCGTimePhases,
    kEnable,
    nullptr,
    "time-phases",
    kBuildTypeExperimental,
    kArgCheckPolicyBool,
    "  --time-phases               \tCollect compilation time stats for each phase\n"
    "  --no-time-phases            \tDon't Collect compilation time stats for each phase\n",
    "mplcg",
    {} },
  { kCGBarrier,
    kEnable,
    nullptr,
    "use-barriers-for-volatile",
    kBuildTypeExperimental,
    kArgCheckPolicyBool,
    "  --use-barriers-for-volatile \tOptimize volatile load/str\n"
    "  --no-use-barriers-for-volatile\n",
    "mplcg",
    {} },
  { kCalleeCFI,
    kEnable,
    nullptr,
    "callee-cfi",
    kBuildTypeExperimental,
    kArgCheckPolicyBool,
    "  --callee-cfi                \tcallee cfi message will be generated\n"
    "  --no-callee-cfi             \tcallee cfi message will not be generated\n",
    "mplcg",
    {} },
  { kPrintFunction,
    kEnable,
    nullptr,
    "print-func",
    kBuildTypeExperimental,
    kArgCheckPolicyBool,
    "  --print-func\n"
    "  --no-print-func\n",
    "mplcg",
    {} },
  { kCyclePatternList,
    0,
    nullptr,
    "cycle-pattern-list",
    kBuildTypeExperimental,
    kArgCheckPolicyRequired,
    "  --cycle-pattern-list        \tFor generating cycle pattern meta\n"
    "                              \t--cycle-pattern-list=list_file\n",
    "mplcg",
    {} },
  { kDuplicateToDelPlt,
    0,
    nullptr,
    "duplicate_asm_list",
    kBuildTypeProduct,
    kArgCheckPolicyRequired,
    "  --duplicate_asm_list        \tDuplicate asm functions to delete plt call\n"
    "                              \t--duplicate_asm_list=list_file\n",
    "mplcg",
    {} },
  { kInsertSoe,
    0,
    nullptr,
    "soe-check",
    kBuildTypeExperimental,
    kArgCheckPolicyNone,
    "  --soe-check                 \tInsert a soe check instruction[default off]\n",
    "mplcg",
    {} },
  { kCheckArrayStore,
    kEnable,
    nullptr,
    "check-arraystore",
    kBuildTypeExperimental,
    kArgCheckPolicyBool,
    "  --check-arraystore          \tcheck arraystore exception[default off]\n"
    "  --no-check-arraystore\n",
    "mplcg",
    {} },
  { kLongCalls,
    kEnable,
    nullptr,
    "long-calls",
    kBuildTypeExperimental,
    kArgCheckPolicyBool,
    "  --long-calls                \tgenerate long call\n"
    "  --no-long-calls\n",
    "mplcg",
    {} },
// End
  { kUnknown,
    0,
    nullptr,
    nullptr,
    kBuildTypeAll,
    kArgCheckPolicyNone,
    nullptr,
    "mplcg",
    {} }
};

CGOptions &CGOptions::GetInstance() {
  static CGOptions instance;
  return instance;
}

CGOptions::CGOptions() {
  CreateUsages(kUsage);
}

void CGOptions::DecideMplcgRealLevel(const std::vector<mapleOption::Option> &inputOptions, bool isDebug) {
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
  if (isDebug) {
    LogInfo::MapleLogger() << "Real Mplcg level:" << std::to_string(realLevel) << "\n";
  }
  if (realLevel ==  CGOptions::kLevel0) {
    EnableO0();
  } else if (realLevel ==  CGOptions::kLevel1) {
    EnableO1();
  } else if (realLevel ==  CGOptions::kLevel2) {
    EnableO2();
  }
}

bool CGOptions::SolveOptions(const std::vector<Option> &opts, bool isDebug) {
  DecideMplcgRealLevel(opts, isDebug);
  for (const mapleOption::Option &opt : opts) {
    if (isDebug) {
      LogInfo::MapleLogger() << "mplcg options: "  << opt.Index() << " " << opt.OptionKey() << " " <<
                                opt.Args() << '\n';
    }
    switch (opt.Index()) {
      case kCGQuiet:
        SetQuiet((opt.Type() == kEnable));
        break;
      case kVerbose:
        SetQuiet((opt.Type() == kEnable) ? false : true);
        break;
      case kPie:
        (opt.Type() == kEnable) ? SetOption(CGOptions::kGenPie)
                                : ClearOption(CGOptions::kGenPie);
        break;
      case kPic: {
        if (opt.Type() == kEnable) {
          EnablePIC();
          SetOption(CGOptions::kGenPic);
        } else {
          DisablePIC();
          ClearOption(CGOptions::kGenPic);
        }
        break;
      }
      case kCGVerbose:
        (opt.Type() == kEnable) ? SetOption(CGOptions::kVerboseAsm)
                                : ClearOption(CGOptions::kVerboseAsm);
        break;
      case kCGMapleLinker:
        (opt.Type() == kEnable) ? EnableMapleLinker() : DisableMapleLinker();
        break;
      case kCGBarrier:
        (opt.Type() == kEnable) ? EnableBarriersForVolatile() : DisableBarriersForVolatile();
        break;
      case kCGDumpBefore:
        (opt.Type() == kEnable) ? EnableDumpBefore() : DisableDumpBefore();
        break;
      case kCGDumpAfter:
        (opt.Type() == kEnable) ? EnableDumpAfter() : DisableDumpAfter();
        break;
      case kCGTimePhases:
        (opt.Type() == kEnable) ? EnableTimePhases() : DisableTimePhases();
        break;
      case kCGDumpFunc:
        SetDumpFunc(opt.Args());
        break;
      case kDuplicateToDelPlt:
        SetDuplicateAsmFile(opt.Args());
        break;
      case kInsertCall:
        SetInstrumentationFunction(opt.Args());
        SetInsertCall(true);
        break;
      case kStackGuard:
        SetOption(kUseStackGuard);
        break;
      case kDebuggingInfo:
        SetOption(kDebugFriendly);
        SetOption(kWithLoc);
        ClearOption(kSuppressFileInfo);
        break;
      case kDebugGenDwarf:
        SetOption(kDebugFriendly);
        SetOption(kWithLoc);
        SetOption(kWithDwarf);
        SetParserOption(kWithDbgInfo);
        ClearOption(kSuppressFileInfo);
        EnableWithDwarf();
        break;
      case kDebugUseSrc:
        SetOption(kDebugFriendly);
        SetOption(kWithLoc);
        SetOption(kWithSrc);
        ClearOption(kWithMpl);
        break;
      case kDebugUseMix:
        SetOption(kDebugFriendly);
        SetOption(kWithLoc);
        SetOption(kWithSrc);
        SetOption(kWithMpl);
        break;
      case kDebugAsmMix:
        SetOption(kDebugFriendly);
        SetOption(kWithLoc);
        SetOption(kWithSrc);
        SetOption(kWithMpl);
        SetOption(kWithAsm);
        break;
      case kProfilingInfo:
        SetOption(kWithProfileCode);
        SetParserOption(kWithProfileInfo);
        break;
      case kRaColor:
        SetOption(kDoColorRegAlloc);
        ClearOption(kDoLinearScanRegAlloc);
        break;
      case kPrintFunction:
        (opt.Type() == kEnable) ? EnablePrintFunction() : DisablePrintFunction();
        break;
      case kTrace:
        SetOption(kAddDebugTrace);
        break;
      case kProfileEnable:
        SetOption(kAddFuncProfile);
        break;
      case kSuppressFinfo:
        SetOption(kSuppressFileInfo);
        break;
      case kConstFoldOpt:
        SetOption(kConstFold);
        break;
      case kCGDumpcfg:
        SetOption(kDumpCFG);
        break;
      case kCGClassList:
        SetClassListFile(opt.Args());
        break;
      case kGenDef:
        SetOrClear(GetGenerateFlags(), CGOptions::kCMacroDef, opt.Type());
        break;
      case kGenGctib:
        SetOrClear(GetGenerateFlags(), CGOptions::kGctib, opt.Type());
        break;
      case kGenPrimorList:
        SetOrClear(GetGenerateFlags(), CGOptions::kPrimorList, opt.Type());
        break;
      case kYieldPoing:
        SetOrClear(GetGenerateFlags(), CGOptions::kGenYieldPoint, opt.Type());
        break;
      case kLocalRc:
        SetOrClear(GetGenerateFlags(), CGOptions::kGenLocalRc, opt.Type());
        break;
      case kEhList: {
        const std::string &ehList = opt.Args();
        SetEHExclusiveFile(ehList);
        EnableExclusiveEH();
        ParseExclusiveFunc(ehList);
        break;
      }
      case kCyclePatternList: {
        const std::string &patternList = opt.Args();
        SetCyclePatternFile(patternList);
        EnableEmitCyclePattern();
        ParseCyclePattern(patternList);
        break;
      }
      case kCgen: {
        bool cgFlag = (opt.Type() == kEnable);
        SetRunCGFlag(cgFlag);
        cgFlag ? SetOption(CGOptions::kDoCg) : ClearOption(CGOptions::kDoCg);
        break;
      }
      case kObjMap:
        SetGenerateObjectMap(opt.Type() == kEnable);
        break;
      case kCGLazyBinding:
        (opt.Type() == kEnable) ? EnableLazyBinding() : DisableLazyBinding();
        break;
      case kCGHotFix:
        (opt.Type() == kEnable) ? EnableHotFix() : DisableHotFix();
        break;
      case kInsertSoe:
        SetOption(CGOptions::kSoeCheckInsert);
        break;
      case kCheckArrayStore:
        (opt.Type() == kEnable) ? EnableCheckArrayStore() : DisableCheckArrayStore();
        break;
      case kCGNativeOpt:
        DisableNativeOpt();
        break;
      case kCalleeCFI:
        (opt.Type() == kEnable) ? DisableNoCalleeCFI() : EnableNoCalleeCFI();
        break;
      case kProepilogue:
        (opt.Type() == kEnable) ? SetOption(CGOptions::kProEpilogueOpt)
                                : ClearOption(CGOptions::kProEpilogueOpt);
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
        SplitPhases(opt.Args(), GetDumpPhases());
        break;
      case kCGSkipPhases:
        SplitPhases(opt.Args(), GetSkipPhases());
        break;
      case kCGSkipFrom:
        SetSkipFrom(opt.Args());
        break;
      case kCGSkipAfter:
        SetSkipAfter(opt.Args());
        break;
      case kLongCalls:
        (opt.Type() == kEnable) ? EnableLongCalls() : DisableLongCalls();
        break;
      case kGCOnly:
        (opt.Type() == kEnable) ? EnableGCOnly() : DisableGCOnly();
        break;
      default:
        WARN(kLncWarn, "input invalid key for mplcg " + opt.OptionKey());
        break;
    }
  }
  return true;
}

void CGOptions::ParseExclusiveFunc(const std::string &fileName) {
  std::ifstream file(fileName);
  if (!file.is_open()) {
    ERR(kLncErr, "%s open failed!", fileName.c_str());
    return;
  }
  std::string content;
  while (file >> content) {
    ehExclusiveFunctionName.push_back(content);
  }
}

void CGOptions::ParseCyclePattern(const std::string &fileName) {
  std::ifstream file(fileName);
  if (!file.is_open()) {
    ERR(kLncErr, "%s open failed!", fileName.c_str());
    return;
  }
  std::string content;
  std::string classStr("class: ");
  while (getline(file, content)) {
    if (content.compare(0, classStr.length(), classStr) == 0) {
      std::vector<std::string> classPatternContent;
      std::string patternContent;
      while (getline(file, patternContent)) {
        if (patternContent.length() == 0) {
          break;
        }
        classPatternContent.push_back(patternContent);
      }
      std::string className = content.substr(classStr.length());
      CGOptions::cyclePatternMap[className] = move(classPatternContent);
    }
  }
}


/* Set default options according to different languages. */
void CGOptions::SetDefaultOptions(const maple::MIRModule &mod) {
  if (mod.IsJavaModule()) {
    generateFlag = generateFlag | kGenYieldPoint | kGenLocalRc;
  }
  insertYieldPoint = GenYieldPoint();
}

void CGOptions::EnableO0() {
  optimizeLevel = kLevel0;
  SetOption(kUseStackGuard);
}

void CGOptions::EnableO1() {
  optimizeLevel = kLevel1;
  ClearOption(kProEpilogueOpt);
  ClearOption(kUseStackGuard);
}

void CGOptions::EnableO2() {
  optimizeLevel = kLevel2;
  ClearOption(kProEpilogueOpt);
  ClearOption(kUseStackGuard);
}

void CGOptions::SplitPhases(const std::string &str, std::unordered_set<std::string> &set) {
  const std::string& tmpStr{ str };
  if ((tmpStr.compare("*") == 0) || (tmpStr.compare("cgir") == 0)) {
    set.insert(tmpStr);
    return;
  }
  StringUtils::Split(tmpStr, set, ',');
}

bool CGOptions::DumpPhase(const std::string &phase) {
  return (IS_STR_IN_SET(dumpPhases, "*") || IS_STR_IN_SET(dumpPhases, "cgir") || IS_STR_IN_SET(dumpPhases, phase));
}

/* match sub std::string of function name */
bool CGOptions::FuncFilter(const std::string &name) {
  return ((dumpFunc.compare("*") == 0) || (name.find(dumpFunc.c_str()) != std::string::npos));
}
}  /* namespace maplebe */
