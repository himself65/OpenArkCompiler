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
#include "me_option.h"
#include <iostream>
#include <cstring>
#include "mpl_logging.h"
#include "option_parser.h"
#include "string_utils.h"

namespace maple {
using namespace mapleOption;

std::unordered_set<std::string> MeOption::dumpPhases = {};
bool MeOption::dumpAfter = false;
std::string MeOption::dumpFunc = "*";
unsigned long MeOption::range[kRangeArrayLen] = { 0, 0 };
bool MeOption::useRange = false;
bool MeOption::quiet = false;
bool MeOption::setCalleeHasSideEffect = false;
bool MeOption::noSteensgaard = false;
bool MeOption::noTBAA = false;
uint8 MeOption::aliasAnalysisLevel = 3;
bool MeOption::noDot = false;
bool MeOption::stmtNum = false;
uint8 MeOption::optLevel = 0;
bool MeOption::ignoreIPA = true;
bool MeOption::lessThrowAlias = true;
bool MeOption::regreadAtReturn = true;
bool MeOption::propBase = true;
bool MeOption::propIloadRef = false;
bool MeOption::propGlobalRef = false;
bool MeOption::propFinaliLoadRef = true;
bool MeOption::propIloadRefNonParm = false;
uint32 MeOption::delRcPULimit = UINT32_MAX;
uint32 MeOption::stmtprePULimit = UINT32_MAX;
uint32 MeOption::epreLimit = UINT32_MAX;
uint32 MeOption::eprePULimit = UINT32_MAX;
uint32 MeOption::lpreLimit = UINT32_MAX;
uint32 MeOption::lprePULimit = UINT32_MAX;
uint32 MeOption::pregRenameLimit = UINT32_MAX;
uint32 MeOption::profileBBHotRate = 10;
uint32 MeOption::profileBBColdRate = 99;
bool MeOption::noDelegateRC = false;
bool MeOption::noCondBasedRC = false;
bool MeOption::clinitPre = true;
bool MeOption::dassignPre = true;
bool MeOption::nullCheckPre = false;
bool MeOption::assign2FinalPre = false;
bool MeOption::epreIncludeRef = false;
bool MeOption::epreLocalRefVar = true;
bool MeOption::epreLHSIvar = true;
bool MeOption::lpreSpeculate = false;
bool MeOption::spillAtCatch = false;
bool MeOption::rcLowering = true;
bool MeOption::optDirectCall = false;
bool MeOption::propAtPhi = true;
bool MeOption::dseKeepRef = false;

enum OptionIndex {
  kMeHelp = kCommonOptionEnd + 1,
  kMeDumpPhases,
  kMeSkipPhases,
  kMeDumpFunc,
  kMeQuiet,
  kMeNoDot,
  kMeSkipFrom,
  kMeSkipAfter,
  kSetCalleeHasSideEffect,
  kNoSteensgaard,
  kNoTBAA,
  kAliasAnalysisLevel,
  kStmtNum,
  kRcLower,
  kNoRcLower,
  kMeDumpBefore,
  kMeDumpAfter,
  kMeOptL1,
  kMeOptL2,
  kMeRange,
  kEpreLimit,
  kEprepuLimit,
  kStmtPrepuLimit,
  kLpreLimit,
  kLprepulLimit,
  kPregreNameLimit,
  kDelrcpuLimit,
  kProfileBBHotRate,
  kProfileBBColdRate,
  kEpreIncludeRef,
  kNoEpreIncludeRef,
  kEpreLocalRefVar,
  kNoEpreLocalRefVar,
  kEprelhSivar,
  kLessThrowAlias,
  kNodeLegateRc,
  kNocondBasedRc,
  kNullcheckPre,
  kClinitPre,
  kDassignPre,
  kAssign2finalPre,
  kRegReadAtReturn,
  kLpreSpeculate,
  kNoLpreSpeculate,
  kSpillatCatch,
};

const Descriptor kUsage[] = {
  { kMeHelp,
    0,
    "h-me",
    "help-me",
    kBuildTypeAll,
    kArgCheckPolicyOptional,
    "  -h-me --help-me             \tPrint usage and exit.Available command names:\n"
    "                              \tme\n",
    "me",
    {} },
  { kMeOptL1,
    0,
    nullptr,
    "O1",
    kBuildTypeProduct,
    kArgCheckPolicyOptional,
    "  -O1                         \tDo some optimization.\n",
    "me",
    {} },
  { kMeOptL2,
    0,
    nullptr,
    "O2",
    kBuildTypeProduct,
    kArgCheckPolicyOptional,
    "  -O2                         \tDo some optimization.\n",
    "me",
    {} },
  { kMeRange,
    0,
    nullptr,
    "range",
    kBuildTypeDebug,
    kArgCheckPolicyRequired,
    "  --range                     \tOptimize only functions in the range [NUM0, NUM1]\n"
    "                              \t--range=NUM0,NUM1\n",
    "me",
    {} },
  { kMeDumpPhases,
    0,
    nullptr,
    "dump-phases",
    kBuildTypeDebug,
    kArgCheckPolicyRequired,
    "  --dump-phases               \tEnable debug trace for specified phases in the comma separated list\n"
    "                              \t--dump-phases=PHASENAME,...\n",
    "me",
    {} },
  { kMeSkipPhases,
    0,
    nullptr,
    "skip-phases",
    kBuildTypeDebug,
    kArgCheckPolicyRequired,
    "  --skip-phases               \tSkip the phases specified in the comma separated list\n"
    "                              \t--skip-phases=PHASENAME,...\n",
    "me",
    {} },
  { kMeDumpFunc,
    0,
    nullptr,
    "dump-func",
    kBuildTypeDebug,
    kArgCheckPolicyRequired,
    "  --dump-func                 \tDump/trace only for functions whose names contain FUNCNAME as substring\n"
    "                              \t(can only specify once)\n"
    "                              \t--dump-func=FUNCNAME\n",
    "me",
    {} },
  { kMeQuiet,
    kEnable,
    nullptr,
    "quiet",
    kBuildTypeProduct,
    kArgCheckPolicyBool,
    "  --quiet                     \tDisable brief trace messages with phase/function names\n"
    "  --no-quiet                  \tEnable brief trace messages with phase/function names\n",
    "me",
    {} },
  { kMeNoDot,
    kEnable,
    nullptr,
    "nodot",
    kBuildTypeAll,
    kArgCheckPolicyBool,
    "  --nodot                     \tDisable dot file generation from cfg\n"
    "  --no-nodot                  \tEnable dot file generation from cfg\n",
    "me",
    {} },
  { kSetCalleeHasSideEffect,
    kEnable,
    nullptr,
    "setCalleeHasSideEffect",
    kBuildTypeAll,
    kArgCheckPolicyBool,
    "  --setCalleeHasSideEffect    \tSet all the callees have side effect\n"
    "  --no-setCalleeHasSideEffect \tNot set all the callees have side effect\n",
    "me",
    {} },
  { kNoSteensgaard,
    kEnable,
    nullptr,
    "noSteensgaard",
    kBuildTypeAll,
    kArgCheckPolicyBool,
    "  --noSteensgaard             \tDisable Steensgaard-style alias analysis\n"
    "  --no-noSteensgaard          \tEnable Steensgaard-style alias analysis\n",
    "me",
    {} },
  { kNoTBAA,
    kEnable,
    nullptr,
    "noTBAA",
    kBuildTypeAll,
    kArgCheckPolicyBool,
    "  --noTBAA                    \tDisable type-based alias analysis\n"
    "  --no-noTBAA                 \tEnable type-based alias analysis\n",
    "me",
    {} },
  { kAliasAnalysisLevel,
    0,
    nullptr,
    "aliasAnalysisLevel",
    kBuildTypeAll,
    kArgCheckPolicyRequired,
    "  --aliasAnalysisLevel        \tSet level of alias analysis. \n"
    "                              \t0: most conservative;\n"
    "                              \t1: Steensgaard-style alias analysis; 2: type-based alias analysis;\n"
    "                              \t3: Steensgaard-style and type-based alias analysis\n"
    "                              \t--aliasAnalysisLevel=NUM\n",
    "me",
    {} },
  { kStmtNum,
    kEnable,
    nullptr,
    "stmtnum",
    kBuildTypeAll,
    kArgCheckPolicyBool,
    "  --stmtnum                   \tPrint MeStmt index number in IR dump\n"
    "  --no-stmtnum                \tDon't print MeStmt index number in IR dump\n",
    "me",
    {} },
  { kRcLower,
    kEnable,
    nullptr,
    "rclower",
    kBuildTypeAll,
    kArgCheckPolicyBool,
    "  --rclower                   \tEnable rc lowering\n"
    "  --no-rclower                \tDisable rc lowering\n",
    "me",
    {} },
  { kMeDumpBefore,
    kEnable,
    nullptr,
    "dump-before",
    kBuildTypeDebug,
    kArgCheckPolicyBool,
    "  --dump-before               \tDo extra IR dump before the specified phase in me\n"
    "  --no-dump-before            \tDon't extra IR dump before the specified phase in me\n",
    "me",
    {} },
  { kMeDumpAfter,
    kEnable,
    nullptr,
    "dump-after",
    kBuildTypeDebug,
    kArgCheckPolicyBool,
    "  --dump-after                \tDo extra IR dump after the specified phase in me\n"
    "  --no-dump-after             \tDo not extra IR dump after the specified phase in me\n",
    "me",
    {} },
  { kEpreLimit,
    0,
    nullptr,
    "eprelimit",
    kBuildTypeDebug,
    kArgCheckPolicyRequired,
    "  --eprelimit                 \tApply EPRE optimization only for the first NUM expressions\n"
    "                              \t--eprelimit=NUM\n",
    "me",
    {} },
  { kEprepuLimit,
    0,
    nullptr,
    "eprepulimit",
    kBuildTypeDebug,
    kArgCheckPolicyRequired,
    "  --eprepulimit               \tApply EPRE optimization only for the first NUM PUs\n"
    "                              \t--eprepulimit=NUM\n",
    "me",
    {} },
  { kStmtPrepuLimit,
    0,
    nullptr,
    "stmtprepulimit",
    kBuildTypeDebug,
    kArgCheckPolicyRequired,
    "  --stmtprepulimit            \tApply STMTPRE optimization only for the first NUM PUs\n"
    "                              \t--stmtprepulimit=NUM\n",
    "me",
    {} },
  { kLpreLimit,
    0,
    nullptr,
    "lprelimit",
    kBuildTypeDebug,
    kArgCheckPolicyRequired,
    "  --lprelimit                 \tApply LPRE optimization only for the first NUM variables\n"
    "                              \t--lprelimit=NUM\n",
    "me",
    {} },
  { kLprepulLimit,
    0,
    nullptr,
    "lprepulimit",
    kBuildTypeDebug,
    kArgCheckPolicyRequired,
    "  --lprepulimit               \tApply LPRE optimization only for the first NUM PUs\n"
    "                              \t--lprepulimit=NUM\n",
    "me",
    {} },
  { kPregreNameLimit,
    0,
    nullptr,
    "pregrenamelimit",
    kBuildTypeAll,
    kArgCheckPolicyRequired,
    "  --pregrenamelimit           \tApply Preg Renaming optimization only up to NUM times\n"
    "                              \t--pregrenamelimit=NUM\n",
    "me",
    {} },
  { kDelrcpuLimit,
    0,
    nullptr,
    "delrcpulimit",
    kBuildTypeAll,
    kArgCheckPolicyRequired,
    "  --delrcpulimit              \tApply DELEGATERC optimization only for the first NUM PUs\n"
    "                              \t--delrcpulimit=NUM\n",
    "me",
    {} },
  { kProfileBBHotRate,
    0,
    nullptr,
    "profile-bb-hot-rate",
    kBuildTypeAll,
    kArgCheckPolicyRequired,
    "  --profile-bb-hot-rate=10   \tA count is regarded as hot if it is in the largest 10%\n",
    "me",
    {} },
  { kProfileBBColdRate,
    0,
    nullptr,
    "profile-bb-cold-rate",
    kBuildTypeAll,
    kArgCheckPolicyRequired,
    "  --profile-bb-cold-rate=99  \tA count is regarded as cold if it is in the smallest 1%\n",
    "me",
    {} },
  { kEpreIncludeRef,
    kEnable,
    nullptr,
    "epreincluderef",
    kBuildTypeAll,
    kArgCheckPolicyBool,
    "  --epreincluderef            \tInclude ref-type expressions when performing epre optimization\n"
    "  --no-epreincluderef         \tDon't include ref-type expressions when performing epre optimization\n",
    "me",
    {} },
  { kEpreLocalRefVar,
    kEnable,
    nullptr,
    "eprelocalrefvar",
    kBuildTypeAll,
    kArgCheckPolicyBool,
    "  --eprelocalrefvar           \tThe EPRE phase will create new localrefvars when appropriate\n"
    "  --no-eprelocalrefvar        \tDisable the EPRE phase create new localrefvars when appropriate\n",
    "me",
    {} },
  { kEprelhSivar,
    kEnable,
    nullptr,
    "eprelhsivar",
    kBuildTypeAll,
    kArgCheckPolicyBool,
    "  --eprelhsivar               \tThe EPRE phase will consider iassigns when optimizing ireads\n"
    "  --no-eprelhsivar            \tDisable the EPRE phase consider iassigns when optimizing ireads\n",
    "me",
    {} },
  { kLessThrowAlias,
    kEnable,
    nullptr,
    "lessthrowalias",
    kBuildTypeAll,
    kArgCheckPolicyBool,
    "  --lessthrowalias            \tHandle aliases at java throw statements more accurately\n"
    "  --no-lessthrowalias         \tDisable lessthrowalias\n",
    "me",
    {} },
  { kNodeLegateRc,
    kEnable,
    nullptr,
    "nodelegaterc",
    kBuildTypeAll,
    kArgCheckPolicyBool,
    "  --nodelegateerc             \tDo not apply RC delegation to local object reference pointers\n"
    "  --no-nodelegateerc          \tDisable nodelegateerc\n",
    "me",
    {} },
  { kNocondBasedRc,
    kEnable,
    nullptr,
    "nocondbasedrc",
    kBuildTypeAll,
    kArgCheckPolicyBool,
    "  --nocondbasedrc             \tDo not apply condition-based RC optimization to\n"
    "                              \tlocal object reference pointers\n"
    "  --no-nocondbasedrc          \tDisable nocondbasedrc\n",
    "me",
    {} },
  { kRegReadAtReturn,
    kEnable,
    nullptr,
    "regreadatreturn",
    kBuildTypeAll,
    kArgCheckPolicyBool,
    "  --regreadatreturn           \tAllow register promotion to promote the operand of return statements\n"
    "  --no-regreadatreturn        \tDisable regreadatreturn\n",
    "me",
    {} },
  { kUnknown,
    0,
    nullptr,
    nullptr,
    kBuildTypeAll,
    kArgCheckPolicyNone,
    nullptr,
    "me",
    {} }
};

void MeOption::DecideMeRealLevel(const std::vector<mapleOption::Option> &inputOptions) const {
  int realLevel = -1;
  for (const mapleOption::Option &opt : inputOptions) {
    switch (opt.Index()) {
      case kMeOptL1:
        realLevel = kLevelOne;
        break;
      case kMeOptL2:
        realLevel = kLevelTwo;
        break;
      default:
        break;
    }
  }
  if (realLevel == kLevelOne) {
    optLevel = kLevelOne;
  } else if (realLevel == kLevelTwo) {
    optLevel = kLevelTwo;
    // Turn the followings ON only at O2
    optDirectCall = true;
    epreIncludeRef = true;
  }
}

bool MeOption::SolveOptions(const std::vector<mapleOption::Option> &opts, bool isDebug) {
  DecideMeRealLevel(opts);
  if (isDebug) {
    LogInfo::MapleLogger() << "Real Me level:" << std::to_string(optLevel) << "\n";
  }
  bool result = true;
  for (const mapleOption::Option &opt : opts) {
    if (isDebug) {
      LogInfo::MapleLogger() << "Me options: " << opt.Index() << " " << opt.OptionKey() << " " << opt.Args() << '\n';
    }
    switch (opt.Index()) {
      case kMeSkipPhases:
        SplitSkipPhases(opt.Args());
        break;
      case kMeOptL1:
        // Already handled above in DecideMeRealLevel
        break;
      case kMeOptL2:
        // Already handled above in DecideMeRealLevel
        break;
      case kMeRange:
        useRange = true;
        result = GetRange(opt.Args());
        break;
      case kMeDumpAfter:
        dumpAfter = (opt.Type() == kEnable);
        break;
      case kMeDumpFunc:
        dumpFunc = opt.Args();
        break;
      case kMeDumpPhases:
        SplitPhases(opt.Args(), dumpPhases);
        break;
      case kMeQuiet:
        quiet = (opt.Type() == kEnable);
        break;
      case kSetCalleeHasSideEffect:
        setCalleeHasSideEffect = (opt.Type() == kEnable);
        break;
      case kNoSteensgaard:
        noSteensgaard = (opt.Type() == kEnable);
        break;
      case kNoTBAA:
        noTBAA = (opt.Type() == kEnable);
        break;
      case kAliasAnalysisLevel:
        aliasAnalysisLevel = std::stoul(opt.Args(), nullptr);
        if (aliasAnalysisLevel > kLevelThree) {
          aliasAnalysisLevel = kLevelThree;
        }
        switch (aliasAnalysisLevel) {
          case kLevelThree:
            setCalleeHasSideEffect = false;
            noSteensgaard = false;
            noTBAA = false;
            break;
          case kLevelZero:
            setCalleeHasSideEffect = true;
            noSteensgaard = true;
            noTBAA = true;
            break;
          case kLevelOne:
            setCalleeHasSideEffect = false;
            noSteensgaard = false;
            noTBAA = true;
            break;
          case kLevelTwo:
            setCalleeHasSideEffect = false;
            noSteensgaard = true;
            noTBAA = false;
            break;
          default:
            break;
        }
        if (isDebug) {
          LogInfo::MapleLogger() << "--sub options: setCalleeHasSideEffect "
                                 << setCalleeHasSideEffect << '\n';
          LogInfo::MapleLogger() << "--sub options: noSteensgaard " << noSteensgaard << '\n';
          LogInfo::MapleLogger() << "--sub options: noTBAA " << noTBAA << '\n';
        }
        break;
      case kRcLower:
        rcLowering = (opt.Type() == kEnable);
        break;
      case kMeNoDot:
        noDot = (opt.Type() == kEnable);
        break;
      case kStmtNum:
        stmtNum = (opt.Type() == kEnable);
        break;
      case kEpreLimit:
        epreLimit = std::stoul(opt.Args(), nullptr);
        break;
      case kEprepuLimit:
        eprePULimit = std::stoul(opt.Args(), nullptr);
        break;
      case kStmtPrepuLimit:
        stmtprePULimit = std::stoul(opt.Args(), nullptr);
        break;
      case kLpreLimit:
        lpreLimit = std::stoul(opt.Args(), nullptr);
        break;
      case kLprepulLimit:
        lprePULimit = std::stoul(opt.Args(), nullptr);
        break;
      case kPregreNameLimit:
        pregRenameLimit = std::stoul(opt.Args(), nullptr);
        break;
      case kDelrcpuLimit:
        delRcPULimit = std::stoul(opt.Args(), nullptr);
        break;
      case kProfileBBHotRate:
        profileBBHotRate = std::stoul(opt.Args(), nullptr);
        break;
      case kProfileBBColdRate:
        profileBBColdRate = std::stoul(opt.Args(), nullptr);
        break;
      case kEpreIncludeRef:
        epreIncludeRef = (opt.Type() == kEnable);
        break;
      case kEpreLocalRefVar:
        epreLocalRefVar = (opt.Type() == kEnable);
        break;
      case kEprelhSivar:
        epreLHSIvar = (opt.Type() == kEnable);
        break;
      case kLessThrowAlias:
        lessThrowAlias = (opt.Type() == kEnable);
        break;
      case kNodeLegateRc:
        noDelegateRC = (opt.Type() == kEnable);
        break;
      case kNocondBasedRc:
        noCondBasedRC = (opt.Type() == kEnable);
        break;
      case kNullcheckPre:
        nullCheckPre = (opt.Type() == kEnable);
        break;
      case kClinitPre:
        clinitPre = (opt.Type() == kEnable);
        break;
      case kDassignPre:
        dassignPre = (opt.Type() == kEnable);
        break;
      case kAssign2finalPre:
        assign2FinalPre = (opt.Type() == kEnable);
        break;
      case kRegReadAtReturn:
        regreadAtReturn = (opt.Type() == kEnable);
        break;
      case kLpreSpeculate:
        lpreSpeculate = (opt.Type() == kEnable);
        break;
      case kSpillatCatch:
        spillAtCatch = (opt.Type() == kEnable);
        break;
      default:
        WARN(kLncWarn, "input invalid key for me " + opt.OptionKey());
        break;
    }
  }
  return result;
}

MeOption &MeOption::GetInstance() {
  static MeOption instance;
  return instance;
}

MeOption::MeOption() {
  CreateUsages(kUsage);
}


void MeOption::SplitPhases(const std::string &str, std::unordered_set<std::string> &set) const {
  std::string s{ str };

  if (s.compare("*") == 0) {
    set.insert(s);
    return;
  }
  StringUtils::Split(s, set, ',');
}

bool MeOption::GetRange(const std::string &str) const {
  std::string s{ str };
  size_t comma = s.find_first_of(",", 0);
  if (comma != std::string::npos) {
    range[0] = std::stoul(s.substr(0, comma), nullptr);
    range[1] = std::stoul(s.substr(comma + 1, std::string::npos - (comma + 1)), nullptr);
  }
  if (range[0] > range[1]) {
    LogInfo::MapleLogger(kLlErr) << "invalid values for --range=" << range[0] << "," << range[1] << '\n';
    return false;
  }
  return true;
}

bool MeOption::DumpPhase(const std::string &phase) {
  if (phase == "") {
    return false;
  }
  return ((dumpPhases.find(phase) != dumpPhases.end()) || (dumpPhases.find("*") != dumpPhases.end()));
}
} // namespace maple
