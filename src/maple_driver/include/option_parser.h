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
#ifndef MAPLE_UTIL_INCLUDE_OPTION_PARSER_H
#define MAPLE_UTIL_INCLUDE_OPTION_PARSER_H
#include <map>
#include <string>
#include <vector>
#include "error_code.h"
#include "option_descriptor.h"
#include "driver_option_common.h"

namespace mapleOption {
class OptionParser {
 public:
  OptionParser() = default;
  ~OptionParser() = default;

  void RegisteUsages(const maple::MapleDriverOptionBase &base);
  void RegisteUsages(const Descriptor usage[]);

  maple::ErrorCode Parse(int argc, char **argv, const std::string exeName = "all");

  maple::ErrorCode HandleInputArgs(const std::vector<std::string> &inputArgs, const std::string &exeName,
                                   std::vector<mapleOption::Option> &inputOption, bool isAllOption = false);

  const std::vector<Option> &GetOptions() const {
    return options;
  }

  const std::vector<std::string> &GetNonOptions() const {
    return nonOptionsArgs;
  }

  int GetNonOptionsCount() const {
    return nonOptionsArgs.size();
  }

  void InsertExtraUsage(const Descriptor &usage);

  void CreateNoOption(const Descriptor &usage);

  bool SetOption(const std::string &key, const std::string &value, const std::string &exeName,
                 std::vector<mapleOption::Option> &exeOption);
  void PrintUsage(const std::string &helpType, const uint32_t helpLevel = kBuildTypeDefault) const;

 private:
  bool HandleKeyValue(const std::string &key, const std::string &value,
                      std::vector<mapleOption::Option> &inputOption, const std::string &exeName,
                      bool isAllOption = true);
  bool CheckOpt(const std::string option, std::string &lastKey, bool &isLastMatch,
                std::vector<mapleOption::Option> &inputOption, const std::string &exeName);
  void InsertOption(const std::string &opt, const Descriptor &usage) {
    if (usage.IsEnabledForCurrentBuild()) {
      usages.insert(make_pair(opt, usage));
    }
  }
  bool CheckSpecialOption(const std::string &option, std::string &key, std::string &value);
  std::vector<Descriptor> rawUsages;
  std::multimap<std::string, Descriptor> usages;
  std::vector<Option> options;
  std::vector<std::string> nonOptionsArgs;
  bool isValueEmpty = false;
};
enum MatchedIndex {
  kMatchNone,
  kMatchShortOpt,
  kMatchLongOpt
};
enum Level {
  kLevelZero = 0,
  kLevelOne = 1,
  kLevelTwo = 2,
  kLevelThree = 3
};
}  // namespace mapleOption

#endif  // MAPLE_UTIL_INCLUDE_OPTION_PARSER_H
