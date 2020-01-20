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
#include "option_parser.h"
#include <regex>
#include <iostream>
#include <cstring>
#include "mpl_logging.h"

using namespace maple;
using namespace mapleOption;

OptionParser::OptionParser(const Descriptor usage[]) : rawUsages(usage) {
  for (size_t i = 0; usage[i].help != nullptr; ++i) {
    if (usage[i].shortOption != nullptr) {
      InsertOption(std::string(usage[i].shortOption), usage[i]);
    }
    if (usage[i].longOption != nullptr) {
      InsertOption(std::string(usage[i].longOption), usage[i]);
    }
  }
}

void OptionParser::PrintUsage() const {
  for (size_t i = 0; rawUsages[i].help != nullptr; ++i) {
    if (rawUsages[i].IsEnabledForCurrentBuild()) {
      LogInfo::MapleLogger() << rawUsages[i].help << '\n';
    }
  }
}

#ifdef OPTION_PARSER_EXTRAOPT
void OptionParser::PrintUsage(const std::string &helpType) const {
  for (size_t i = 0; rawUsages[i].help != nullptr; ++i) {
    if (rawUsages[i].IsEnabledForCurrentBuild() && rawUsages[i].exeName == helpType) {
      LogInfo::MapleLogger() << rawUsages[i].help << '\n';
    }
  }
}
#endif


bool OptionParser::HandleKeyValue(const std::string &key, const std::string &value, bool isValueEmpty,
                                  std::vector<mapleOption::Option> &inputOption, const std::string &exeName,
                                  bool isAllOption) {
  if (key.empty()) {
    if (!isAllOption) {
      LogInfo::MapleLogger(kLlErr) << "Cannot recognize " << value << '\n';
      return false;
    }
    nonOptionsArgs.push_back(value);
    return true;
  }
  size_t count = usages.count(key);
  auto item = usages.find(key);
#ifdef OPTION_PARSER_EXTRAOPT
  while (count > 0 && item->second.exeName != exeName) {
    ++item;
    --count;
  }
#endif
  if (count == 0) {
    LogInfo::MapleLogger(kLlErr) << ("Unknown Option: " + key) << '\n';
    return false;
  }
  switch (item->second.checkPolicy) {
    case kArgCheckPolicyUnknown:
      LogInfo::MapleLogger(kLlErr) << ("Unknown option " + key) << '\n';
      return false;
    case kArgCheckPolicyNone:
    case kArgCheckPolicyOptional:
      break;
    case kArgCheckPolicyRequired:
      if (value.empty() && !isValueEmpty) {
        LogInfo::MapleLogger(kLlErr) << ("Option " + key + " requires an argument.") << '\n';
        return false;
      }
      break;
    case kArgCheckPolicyNumeric:
      if (value.empty()) {
        LogInfo::MapleLogger(kLlErr) << ("Option " + key + " requires an argument.") << '\n';
        return false;
      } else {
        std::regex rx("^(-|)[0-9]+\\b");
        bool isNumeric = std::regex_match(value.begin(), value.end(), rx);
        if (!isNumeric) {
          LogInfo::MapleLogger(kLlErr) << ("Option " + key + " requires a numeric argument.") << '\n';
          return false;
        }
      }
      break;
    default:
      break;
  }
  inputOption.push_back(Option(item->second, key, value));
  return true;
}

#ifdef OPTION_PARSER_EXTRAOPT
bool OptionParser::SetOption(const std::string &rawKey, const std::string &value, const std::string &exeName,
                             std::vector<mapleOption::Option> &exeOption) {
  constexpr char kOptionMark = '-';
  if (rawKey.empty()) {
    LogInfo::MapleLogger(kLlErr) << "Invalid key" << '\n';
    PrintUsage("all");
    return false;
  }
  int index = 0;
  if (rawKey[0] == kOptionMark) {
    ++index;
    if (rawKey[1] == kOptionMark) {
      ++index;
    }
  }
  std::string key = rawKey.substr(index);
  int count = usages.count(key);
  auto item = usages.find(key);
  while (count > 0) {
    if (item->second.exeName != exeName) {
      ++item;
      --count;
      continue;
    }
    switch (item->second.checkPolicy) {
      case kArgCheckPolicyNone:
      case kArgCheckPolicyOptional:
        break;
      case kArgCheckPolicyRequired:
        if (value.empty()) {
          LogInfo::MapleLogger(kLlErr) << ("Option " + key + " requires an argument.") << '\n';
          return false;
        }
        break;
      default:
        break;
    }
    break;
  }
  exeOption.push_back(Option(item->second, key, value));
  return true;
}
#endif

bool OptionParser::CheckOpt(const std::string option, std::string &lastKey,
                            bool &isLastMatch, std::vector<mapleOption::Option> &inputOption,
                            const std::string &exeName) {
  std::vector<std::string> temps;
  size_t pos = option.find('=');
  if (pos != std::string::npos) {
    ASSERT(pos > 0, "option should not begin with symbol '='");
    isLastMatch = false;
    std::string key = option.substr(0, pos);
    std::string value = option.substr(pos + 1);
    return HandleKeyValue(key, value, value.empty(), inputOption, exeName);
  } else {
    auto item = usages.find(option);
    if (item != usages.end()) {
      if (item->second.checkPolicy == kArgCheckPolicyRequired || item->second.checkPolicy == kArgCheckPolicyNumeric) {
        lastKey = option;
        isLastMatch = true;
      } else {
        return HandleKeyValue(option, "", false, inputOption, exeName);
      }
    } else {
      LogInfo::MapleLogger(kLlErr) << ("Unknown Option: " + option) << '\n';
      return false;
    }
  }
  return true;
}

ErrorCode OptionParser::HandleInputArgs(const std::vector<std::string> &inputArgs, const std::string &exeName,
                                        std::vector<mapleOption::Option> &inputOption) {
  bool isLastMatchOpt = false;
  std::string lastKey = "";
  bool ret = true;
  for (size_t i = 0; i < inputArgs.size(); ++i) {
    if (inputArgs[i] == "") {
      continue;
    }
    bool isMatchLongOpt = false;
    bool isMatchShortOpt = false;
    MatchedIndex index = kMatchNone;
    if (inputArgs[i][0] == '-') {
      index = kMatchShortOpt;
      if (inputArgs[i][1] == '-') {
        index = kMatchLongOpt;
      }
    }
    if (index == kMatchShortOpt) {
      isMatchShortOpt = true;
    } else if (index == kMatchLongOpt) {
      isMatchLongOpt = true;
    }
    std::string arg = inputArgs[i].substr(index);
    bool isAllOption = (exeName == "all") ? true : false;
    bool isOptMatched = isMatchLongOpt || isMatchShortOpt;
    if (!isLastMatchOpt && isOptMatched) {
      ret = CheckOpt(arg, lastKey, isLastMatchOpt, inputOption, exeName);
    } else if (isLastMatchOpt && !isOptMatched) {
      isLastMatchOpt = false;
      ret = HandleKeyValue(lastKey, arg, false, inputOption, exeName, isAllOption);
    } else if (isLastMatchOpt && isOptMatched) {
      LogInfo::MapleLogger(kLlErr) << ("Unknown Option: " + arg) << '\n';
      return kErrorInvalidParameter;
    } else {
      ret = HandleKeyValue("", arg, false, inputOption, exeName, isAllOption);
    }
    if (i == inputArgs.size() - 1 && isLastMatchOpt) {
      LogInfo::MapleLogger(kLlErr) << ("Option " + lastKey + " requires an argument.") << '\n';
      return kErrorInvalidParameter;
    }
    if (!ret) {
      return kErrorInvalidParameter;
    }
  }
  return kErrorNoError;
}

ErrorCode OptionParser::Parse(int argc, char **argv) {
  if (argc > 0) {
    --argc;
    ++argv;  // skip program name argv[0] if present
  }
  if (argc == 0 || *argv == nullptr) {
#ifdef OPTION_PARSER_EXTRAOPT
    PrintUsage("all");
#else
    PrintUsage();
#endif
    LogInfo::MapleLogger(kLlErr) << "No input files!" << '\n';
    return kErrorInitFail;
  }
  // transform char* to string
  std::vector<std::string> inputArgs;
  while (argc != 0 && *argv != nullptr) {
    inputArgs.push_back(*argv);
    ++argv;
    --argc;
  }
  ErrorCode ret = HandleInputArgs(inputArgs, "all", options);
  return ret;
}
