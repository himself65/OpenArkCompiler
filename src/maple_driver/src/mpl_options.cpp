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
#include "usages.h"
#include "version.h"
#include "default_options.def"

namespace {
using namespace maple;
const mapleOption::Descriptor USAGES[] = {
  // index, type , shortOption , longOption, connector, isCanAppend, delimiter, enableBuildType, checkPolicy, help,
  // extra
  { kUnknown,
    0,
    nullptr,
    nullptr,
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "USAGE: maple [options]\n\n Options:",
    "all",
    { { nullptr } } },
  { kHelp,
    0,
    "h",
    "help",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyOptional,
    "  -h --help [command]         \tPrint usage and exit.\n",
    "all",
    { { nullptr } } },
  { kVersion,
    0,
    nullptr,
    "version",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyOptional,
    "  --version [command]         \tPrint version and exit.\n",
    "all",
    { { nullptr } } },
  { kInFile,
    0,
    nullptr,
    "infile",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyRequired,
    "  --infile file1,file2,file3  \tInput files.\n",
    "all",
    { { nullptr } } },
  { kInMplt,
    0,
    nullptr,
    "mplt",
    "=",
    true,
    ",",
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyRequired,
    "  --mplt=file1,file2,file3    \tImport mplt files.\n",
    "all",
    { { "jbc2mpl", "-mplt", " ", nullptr },
      // End
      { nullptr, nullptr, nullptr, nullptr } } },
  { kOptimization0,
    0,
    "O0",
    nullptr,
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  -O0                         \tNo optimization.\n",
    "all",
    { { nullptr } } },
  { kMeOpt,
    0,
    nullptr,
    "me-opt",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --me-opt                    \tSet options for me\n",
    "all",
    { { nullptr } } },
  { kMpl2MplOpt,
    0,
    nullptr,
    "mpl2mpl-opt",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --mpl2mpl-opt               \tSet options for mpl2mpl\n",
    "all",
    { { nullptr } } },
  { kSaveTemps,
    0,
    nullptr,
    "save-temps",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --save-temps                \tDo not delete intermediate files.\n"
    "                              \t--save-temps Save all intermediate files.\n"
    "                              \t--save-temps=file1,file2,file3 Save the\n"
    "                              \ttarget files.\n",
    "all",
    { { nullptr } } },
  { kRun,
    0,
    nullptr,
    "run",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeDebug,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyRequired,
    "  --run=cmd1:cmd2             \tThe name of executables that are going\n"
    "                              \tto execute. IN SEQUENCE.\n"
    "                              \tSeparated by \":\".Available exe names:\n"
    "                              \tjbc2mpl, me, mpl2mpl, mplcg\n"
    "                              \tInput file must match the tool can\n"
    "                              \thandle\n",
    "all",
    { { nullptr } } },
  { kOption,
    0,
    nullptr,
    "option",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeDebug,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyRequired,
    "  --option=\"opt1:opt2\"      \tOptions for each executable,\n"
    "                              \tseparated by \":\".\n"
    "                              \tThe sequence must match the sequence in\n"
    "                              \t--run.\n",
    "all",
    { { nullptr } } },
  { kTimePhases,
    0,
    "time-phases",
    nullptr,
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  -time-phases                \tTiming phases and print percentages\n",
    "all",
    { { nullptr } } },
  { kGenMeMpl,
    0,
    nullptr,
    "genmempl",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --genmempl                  \tGenerate me.mpl file\n",
    "all",
    { { nullptr } } },
  { kGenVtableImpl,
    0,
    nullptr,
    "genVtableImpl",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --genVtableImpl             \tGenerate VtableImpl.mpl file\n",
    "all",
    { { nullptr } } },
  { kVerify,
    0,
    nullptr,
    "verify",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --verify                    \tVerify mpl file\n",
    "all",
    { { nullptr } } },
  { kAllDebug,
    0,
    nullptr,
    "debug",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --debug                     \tPrint debug info.\n",
    "all",
    { { nullptr } } },
  // jbc2mpl
  { kUseStringFactory,
    0,
    nullptr,
    "use-string-factory",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  -use-string-factory         \tReplace String.<init> by StringFactory call\n",
    "jbc2mpl",
    { { nullptr } } },
  { kJbc2mplOutMpl,
    0,
    "o",
    "out",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeDebug,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyRequired,
    "  -o, -out=output.mpl         \toutput mpl name\n",
    "jbc2mpl",
    { { nullptr } } },
  // me
  { kMeHelp,
    0,
    "h-me",
    "help-me",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyOptional,
    "  -h-me --help-me             \tPrint usage and exit.Available command names:\n"
    "                              \tme\n",
    "all",
    { { nullptr } } },
  { kMeRange,
    0,
    nullptr,
    "range",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyRequired,
    "  --range                     \tOptimize only functions in the range [NUM0, NUM1]\n"
    "                              \t--range=NUM0,NUM1\n",
    "me",
    { { nullptr } } },
  { kMeDumpPhases,
    0,
    nullptr,
    "dump-phases",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyRequired,
    "  --dump-phases               \tEnable debug trace for specified phases in the comma separated list\n"
    "                              \t--dump-phases=PHASENAME,...\n",
    "me",
    { { nullptr } } },
  { kMeSkipPhases,
    0,
    nullptr,
    "skip-phases",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyRequired,
    "  --skip-phases               \tSkip the phases specified in the comma separated list\n"
    "                              \t--skip-phases=PHASENAME,...\n",
    "me",
    { { nullptr } } },
  { kMeDumpFunc,
    0,
    nullptr,
    "dump-func",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyRequired,
    "  --dump-func                 \tDump/trace only for functions whose names contain FUNCNAME as substring\n"
    "                              \t(can only specify once)\n"
    "                              \t--dump-func=FUNCNAME\n",
    "me",
    { { nullptr } } },
  { kMeQuiet,
    0,
    nullptr,
    "quiet",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --quiet                     \tDisable brief trace messages with phase/function names\n",
    "me",
    { { nullptr } } },
  { kMeNoDot,
    0,
    nullptr,
    "nodot",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --nodot                     \tDisable dot file generation from cfg\n",
    "me",
    { { nullptr } } },
  { kSetCalleeHasSideEffect,
    0,
    nullptr,
    "setCalleeHasSideEffect",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --setCalleeHasSideEffect    \tSet all the callees have side effect\n",
    "me",
    { { nullptr } } },
  { kNoSteensgaard,
    0,
    nullptr,
    "noSteensgaard",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --noSteensgaard             \tDisable Steensgaard-style alias analysis\n",
    "me",
    { { nullptr } } },
  { kNoTBAA,
    0,
    nullptr,
    "noTBAA",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --noTBAA                    \tDisable type-based alias analysis\n",
    "me",
    { { nullptr } } },
  { kAliasAnalysisLevel,
    0,
    nullptr,
    "aliasAnalysisLevel",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyRequired,
    "  --aliasAnalysisLevel        \tSet level of alias analysis. \n"
    "                              \t0: most conservative;\n"
    "                              \t1: Steensgaard-style alias analysis; 2: type-based alias analysis;\n"
    "                              \t3: Steensgaard-style and type-based alias analysis\n"
    "                              \t--aliasAnalysisLevel=NUM\n",
    "me",
    { { nullptr } } },
  { kStmtNum,
    0,
    nullptr,
    "stmtnum",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --stmtnum                   \tPrint MeStmt index number in IR dump\n",
    "me",
    { { nullptr } } },
  { kMeDumpAfter,
    0,
    nullptr,
    "dump-after",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --dump-after                \tDo extra IR dump after the specified phase in me\n",
    "me",
    { { nullptr } } },
  { kLessThrowAlias,
    0,
    nullptr,
    "lessthrowalias",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --lessthrowalias            \tHandle aliases at java throw statements more accurately\n",
    "me",
    { { nullptr } } },
  { kRegReadAtReturn,
    0,
    nullptr,
    "regreadatreturn",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --regreadatreturn           \tAllow register promotion to promote the operand of return statements\n",
    "me",
    { { nullptr } } },
  // mpl2mpl
  { kMpl2MplHelp,
    0,
    "h-mpl2mpl",
    "help-mpl2mpl",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyOptional,
    "  -h-mpl2mpl --help-mpl2mpl   \tPrint usage and exit.Available command names:\n"
    "                              \tmpl2mpl\n",
    "all",
    { { nullptr } } },
  { kMpl2MplDumpPhase,
    0,
    nullptr,
    "dump-phase",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyRequired,
    "  --dump-phase                \tEnable debug trace for specified phase (can only specify once)\n"
    "                              \t--dump-phase=PHASENAME\n",
    "mpl2mpl",
    { { nullptr } } },
  { kMpl2MplSkipPhase,
    0,
    nullptr,
    "skip-phase",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyRequired,
    "  --skip-phase                \tSkip the phase when adding it to phase manager\n"
    "                              \t--skip-phase=PHASENAME\n",
    "mpl2mpl",
    { { nullptr } } },
  { kMpl2MplSkipFrom,
    0,
    nullptr,
    "skip-from",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyRequired,
    "  --skip-from                 \tSkip all remaining phases including PHASENAME\n"
    "                              \t--skip-from=PHASENAME\n",
    "mpl2mpl",
    { { nullptr } } },
  { kMpl2MplSkipAfter,
    0,
    nullptr,
    "skip-after",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyRequired,
    "  --skip-after                \tSkip all remaining phases after PHASENAME\n"
    "                              \t--skip-after=PHASENAME\n",
    "mpl2mpl",
    { { nullptr } } },
  { kMpl2MplDumpFunc,
    0,
    nullptr,
    "dump-func",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyRequired,
    "  --dump-func                 \tDump/trace only for functions whose names contain FUNCNAME as substring\n"
    "                              \t(can only specify once)\n"
    "                              \t--dump-func=FUNCNAME\n",
    "mpl2mpl",
    { { nullptr } } },
  { kMpl2MplQuiet,
    0,
    nullptr,
    "quiet",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --quiet                     \tDisable brief trace messages with phase/function names\n",
    "mpl2mpl",
    { { nullptr } } },
  { kMpl2MplMapleLinker,
    0,
    nullptr,
    "maplelinker",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --maplelinker               \tGenerate MUID symbol tables and references\n",
    "mpl2mpl",
    { { nullptr } } },
  { kMpl2MplStubJniFunc,
    0,
    nullptr,
    "regnativefunc",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --regnativefunc             \tGenerate native stub function to support JNI registration and calling\n",
    "mpl2mpl",
    { { nullptr } } },
  { kInlineWithProfile,
    0,
    nullptr,
    "inline-with-profile",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --inline-with-profile       \tEnable profile-based inlining\n",
    "mpl2mpl",
    { { nullptr } } },
  { kInlineWithoutProfile,
    0,
    nullptr,
    "inline-without-profile",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --inline-without-profile    \tDisable profile-based inlining\n",
    "mpl2mpl",
    { { nullptr } } },
  { kMpl2MplUseInline,
    0,
    nullptr,
    "inline",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --inline                    \tEnable function inlining\n",
    "mpl2mpl",
    { { nullptr } } },
  { kMpl2MplNoInline,
    0,
    nullptr,
    "no-inline",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --no-inline                 \tDisable function inlining\n",
    "mpl2mpl",
    { { nullptr } } },
  { kMpl2MplNoInlineFuncList,
    0,
    nullptr,
    "no-inlinefunclist",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyRequired,
    "  --no-inlinefunclist=list    \tDo not inline function in this list\n",
    "mpl2mpl",
    { { nullptr } } },
  { kMpl2MplUseCrossModuleInline,
    0,
    nullptr,
    "cross-module-inline",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --cross-module-inline       \tEnable cross-module inlining\n",
    "mpl2mpl",
    { { nullptr } } },
  { kMpl2MplNoCrossModuleInline,
    0,
    nullptr,
    "no-cross-module-inline",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --no-cross-module-inline    \tDisable cross-module inlining\n",
    "mpl2mpl",
    { { nullptr } } },
  { kInlineSmallFunctionThreshold,
    0,
    nullptr,
    "inline-small-function-threshold",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyRequired,
    "  --inline-small-function-threshold=15            \tThreshold for inlining small function\n",
    "mpl2mpl",
    { { nullptr } } },
  { kInlineHotFunctionThreshold,
    0,
    nullptr,
    "inline-hot-function-threshold",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyRequired,
    "  --inline-hot-function-threshold=30              \tThreshold for inlining hot function\n",
    "mpl2mpl",
    { { nullptr } } },
  { kInlineModuleGrowth,
    0,
    nullptr,
    "inline-module-growth",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyRequired,
    "  --inline-module-growth=100000                   \tThreshold for maxmium code size growth rate (10%)\n",
    "mpl2mpl",
    { { nullptr } } },
  { kInlineColdFunctionThreshold,
    0,
    nullptr,
    "inline-cold-function-threshold",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyRequired,
    "  --inline-cold-function-threshold=3              \tThreshold for inlining hot function\n",
    "mpl2mpl",
    { { nullptr } } },
  { kProfileHotCount,
    0,
    nullptr,
    "profile-hot-count",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyRequired,
    "  --profile-hot-count=1000    \tA count is regarded as hot if it exceeds this number\n",
    "mpl2mpl",
    { { nullptr } } },
  { kProfileColdCount,
    0,
    nullptr,
    "profile-cold-count",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyRequired,
    "  --profile-cold-count=10     \tA count is regarded as cold if it is below this number\n",
    "mpl2mpl",
    { { nullptr } } },
  { kProfileHotRate,
    0,
    nullptr,
    "profile-hot-rate",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyRequired,
    "  --profile-hot-rate=500000   \tA count is regarded as hot if it is in the largest 50%\n",
    "mpl2mpl",
    { { nullptr } } },
  { kProfileColdRate,
    0,
    nullptr,
    "profile-cold-rate",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyRequired,
    "  --profile-cold-rate=900000  \tA count is regarded as cold if it is in the smallest 10%\n",
    "mpl2mpl",
    { { nullptr } } },
  { kNativeWrapper,
    kEnable,
    nullptr,
    "nativewrapper",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --nativewrapper             \tGenerate native wrappers [default]\n",
    "mpl2mpl",
    { { nullptr } } },
  { kNativeWrapper,
    kDisable,
    nullptr,
    "no-nativewrapper",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --no-nativewrapper\n",
    "mpl2mpl",
    { { nullptr } } },
  { kRegNativeDynamicOnly,
    0,
    nullptr,
    "regnative-dynamic-only",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --regnative-dynamic-only    \tOnly Generate dynamic register code, Report Fatal Msg if no implemented\n",
    "mpl2mpl",
    { { nullptr } } },
  { kRegNativeStaticBindingList,
    0,
    nullptr,
    "static-binding-list",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyRequired,
    "  --static-bindig-list        \tOnly Generate static binding function in file configure list\n"
    "                              \t--static-bindig-list=file\n",
    "mpl2mpl",
    { { nullptr } } },
  { kMpl2MplDumpBefore,
    0,
    nullptr,
    "dump-before",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --dump-before               \tDo extra IR dump before the specified phase\n",
    "mpl2mpl",
    { { nullptr } } },
  { kMpl2MplDumpAfter,
    0,
    nullptr,
    "dump-after",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --dump-after                \tDo extra IR dump after the specified phase\n",
    "mpl2mpl",
    { { nullptr } } },
  { kMplnkDumpMuid,
    0,
    nullptr,
    "dump-muid",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --dump-muid                 \tDump MUID def information into a .muid file\n",
    "mpl2mpl",
    { { nullptr } } },
  { kEmitVtableImpl,
    0,
    nullptr,
    "emitVtableImpl",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --emitVtableImpl            \tgenerate VtableImpl file\n",
    "mpl2mpl",
    { { nullptr } } },
#if MIR_JAVA
  { kMpl2MplSkipVirtual,
    0,
    nullptr,
    "skipvirtual",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --skipvirtual\n",
    "mpl2mpl",
    { { nullptr } } },
#endif
  // mplcg
  { kPie,
    kEnable,
    nullptr,
    "pie",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --pie                       \tGenerate position-independent executable\n",
    "mplcg",
    { { nullptr } } },
  { kPie,
    kDisable,
    nullptr,
    "no-pie",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --no-pie\n",
    "mplcg",
    { { nullptr } } },
  { kPic,
    kEnable,
    nullptr,
    "fpic",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --fpic                      \tGenerate position-independent shared library\n",
    "mplcg",
    { { nullptr } } },
  { kPic,
    kDisable,
    nullptr,
    "no-fpic",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --no-fpic\n",
    "mplcg",
    { { nullptr } } },
  { kVerbose,
    kEnable,
    nullptr,
    "verbose-asm",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --verbose-asm               \tAdd comments to asm output\n",
    "mplcg",
    { { nullptr } } },
  { kVerbose,
    kDisable,
    nullptr,
    "no-verbose-asm",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --no-verbose-asm\n",
    "mplcg",
    { { nullptr } } },
  { kCGMapleLinker,
    kEnable,
    nullptr,
    "maplelinker",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --maplelinker               \tGenerate the MapleLinker .s format\n",
    "mplcg",
    { { nullptr } } },
  { kCGQuiet,
    kEnable,
    nullptr,
    "quiet",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --quiet                     \tBe quiet (don't output debug messages)\n",
    "mplcg",
    { { nullptr } } },
  { kCGQuiet,
    kDisable,
    nullptr,
    "no-quiet",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    "  --no-quiet\n",
    "mplcg",
    { { nullptr } } },

  { kDuplicateToDelPlt,
    0,
    nullptr,
    "duplicate_asm_list",
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyRequired,
    "  --duplicate_asm_list        \tDuplicate asm functions to delete plt call\n"
    "                              \t--duplicate_asm_list=list_file\n",
    "mplcg",
    { { nullptr } } },

  { kUnknown,
    0,
    nullptr,
    nullptr,
    nullptr,
    false,
    nullptr,
    mapleOption::BuildType::kBuildTypeAll,
    mapleOption::ArgCheckPolicy::kArgCheckPolicyNone,
    nullptr,
    "",
    { { nullptr } } }
};
} // namepsace

namespace maple {
using namespace mapleOption;

const std::string kMapleDriverVersion = "MapleDriver " + std::to_string(Version::kMajorMplVersion) + "." +
                                        std::to_string(Version::kMinorCompilerVersion) + " 20190929";
int MplOptions::Parse(int argc, char **argv) {
  optionParser.reset(new OptionParser(USAGES));
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
        optionParser->PrintUsage("all");
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
      case kMeHelp:
        optionParser->PrintUsage(kBinNameMe);
        return kErrorExitHelp;
      case kMpl2MplHelp:
        optionParser->PrintUsage(kBinNameMpl2mpl);
        return kErrorExitHelp;
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
      case kVerify:
        verify = true;
        printCommandStr += " --verify";
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
  for (const auto &extra : option.GetExtras()) {
    auto iter = std::find(runningExes.begin(), runningExes.end(), extra.exeName);
    if (iter == runningExes.end()) {
      continue;
    }
    // For outside compilers, such as jbc2mpl
    options[extra.exeName].push_back(option);
    // For internel compilers, such as me, mpl2mpl
    auto &exeOption = exeOptions[extra.exeName];
    bool ret = optionParser->SetOption(option.OptionKey(), option.Args(), extra.exeName, exeOption);
    if (!ret) {
      return kErrorInvalidParameter;
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

std::string MplOptions::OptimizationLevelStr() const {
  switch (optimizationLevel) {
    case OptimizationLevel::kO0: {
      return "-O0";
    }
    case OptimizationLevel::kO1: {
      return "-O1";
    }
    case OptimizationLevel::kO2: {
      return "-O2";
    }
  }
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
  }
  return ret;
}

ErrorCode MplOptions::AppendDefaultCgOptions() {
  ErrorCode ret = kErrorNoError;
  if (optimizationLevel == kO0) {
    UpdateRunningExe(kBinNameMplcg);
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
      MplOption mplOption("-" + exeOption[i].OptionKey(), exeOption[i].Args(), "=", false, "");
      extraOption.push_back(mplOption);
    } else {
      MplOption mplOption("-" + exeOption[i].OptionKey(), "", " ", false, "");
      extraOption.push_back(mplOption);
    }
  }
  return ret;
}

ErrorCode MplOptions::UpdateExtraOptionOpt(const std::string &args) {
  std::vector<std::string> temp;
  StringUtils::Split(args, temp, ':');
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
