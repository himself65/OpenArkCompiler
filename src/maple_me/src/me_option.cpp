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

void MeOption::SplitPhases(const std::string &str, std::unordered_set<std::string> &set) const {
  std::string s{ str };

  if (s.compare("*") == 0) {
    set.insert(s);
    return;
  }
  StringUtils::Split(s, set, ',');
}

void MeOption::GetRange(const std::string &str) const {
  std::string s{ str };
  size_t comma = s.find_first_of(",", 0);
  if (comma != std::string::npos) {
    range[0] = std::stoul(s.substr(0, comma), nullptr);
    range[1] = std::stoul(s.substr(comma + 1, std::string::npos - (comma + 1)), nullptr);
  }
  if (range[0] > range[1]) {
    LogInfo::MapleLogger(kLlErr) << "invalid values for --range=" << range[0] << "," << range[1] << '\n';
    ASSERT(false, "GetRange exit");
  }
}

bool MeOption::DumpPhase(const std::string &phase) {
  if (phase == "") {
    return false;
  }
  return ((dumpPhases.find(phase) != dumpPhases.end()) || (dumpPhases.find("*") != dumpPhases.end()));
}
} // namespace maple
