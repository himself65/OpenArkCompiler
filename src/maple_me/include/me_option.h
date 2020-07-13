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
#ifndef MAPLE_ME_INCLUDE_ME_OPTION_H
#define MAPLE_ME_INCLUDE_ME_OPTION_H
#include <vector>
#include <string>
#include "mempool.h"
#include "mempool_allocator.h"
#include "types_def.h"
#include "driver_option_common.h"
#include "option_parser.h"

namespace maple {
class MeOption : public MapleDriverOptionBase {
 public:
  static MeOption &GetInstance();

  MeOption();

  ~MeOption() = default;

  bool SolveOptions(const std::vector<mapleOption::Option> &opts, bool isDebug);

  void ParseOptions(int argc, char **argv, std::string &fileName);

  void SplitPhases(const std::string &str, std::unordered_set<std::string> &set) const;
  void SplitSkipPhases(const std::string &str) {
    SplitPhases(str, skipPhases);
  }
  bool GetRange(const std::string &str) const;

  const std::unordered_set<std::string> &GetSkipPhases() const {
    return skipPhases;
  }

  static bool DumpPhase(const std::string &phase);
  static std::unordered_set<std::string> dumpPhases;
  static bool dumpBefore;
  static bool dumpAfter;
  static constexpr int kRangeArrayLen = 2;
  static unsigned long range[kRangeArrayLen];
  static bool useRange;
  static std::string dumpFunc;
  static std::string skipFrom;
  static std::string skipAfter;
  static bool quiet;
  static bool setCalleeHasSideEffect;
  static bool noSteensgaard;
  static bool noTBAA;
  static uint8 aliasAnalysisLevel;
  static bool noDot;
  static bool stmtNum;
  static bool rcLowering;
  static uint8 optLevel;
  static uint32 stmtprePULimit;
  static uint32 epreLimit;
  static uint32 eprePULimit;
  static uint32 lpreLimit;
  static uint32 lprePULimit;
  static uint32 pregRenameLimit;
  static uint32 delRcPULimit;
  static uint32 profileBBHotRate;
  static uint32 profileBBColdRate;
  static bool ignoreIPA;
  static bool aggressiveABCO;
  static bool commonABCO;
  static bool conservativeABCO;
  static bool epreIncludeRef;
  static bool epreLocalRefVar;
  static bool epreLHSIvar;
  static bool dseKeepRef;
  static bool lessThrowAlias;
  static bool noDelegateRC;
  static bool noCondBasedRC;
  static bool nullCheckPre;
  static bool assign2FinalPre;
  static bool clinitPre;
  static bool dassignPre;
  static bool regreadAtReturn;
  static bool propBase;
  static bool propIloadRef;
  static bool propGlobalRef;
  static bool propFinaliLoadRef;
  static bool propIloadRefNonParm;
  static bool propAtPhi;
  static bool lpreSpeculate;
  static bool spillAtCatch;
  static bool optDirectCall;
 private:
  void DecideMeRealLevel(const std::vector<mapleOption::Option> &inputOptions) const;
  std::unordered_set<std::string> skipPhases;
};

#ifndef DEBUGFUNC
#define DEBUGFUNC(f)                                                       \
  (MeOption::dumpPhases.find(PhaseName()) != MeOption::dumpPhases.end() && \
   (MeOption::dumpFunc.compare("*") == 0 || (f)->GetName().find(MeOption::dumpFunc) != std::string::npos))
#endif
}  // namespace maple
#endif  // MAPLE_ME_INCLUDE_ME_OPTION_H
