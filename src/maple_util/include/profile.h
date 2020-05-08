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
#ifndef MAPLE_UTIL_INCLUDE_PROFILE_H
#define MAPLE_UTIL_INCLUDE_PROFILE_H
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include "profile_type.h"
#include "mpl_logging.h"
#include "types_def.h"
#include "option.h"

namespace maple {

struct IRProfileDesc {
  uint32 counterStart = 0;
  uint32 counterEnd = 0;
  uint64 funcHash = 0;
  IRProfileDesc() = default;
  IRProfileDesc(uint64 hash, uint32 start, uint32 end) : counterStart(start), counterEnd(end), funcHash(hash) {}
};


class Profile {
 public:
  struct FuncItem {
    uint32 callTimes;
    uint8 type;
  };
  // function ir profile info
  struct BBInfo {
    uint64 funcHash = 0;
    uint32 totalCounter = 0;
    std::vector<uint32> counter;
    BBInfo() = default;
    BBInfo(uint64 hash, uint32 num, std::vector<uint32> &&counter)
        : funcHash(hash), totalCounter(num), counter(counter) {}
    BBInfo(uint64 hash, uint32 num, std::initializer_list<uint32> iList) : funcHash(hash), totalCounter(num),
        counter(iList) {}
  };

  static const uint8 kStringEnd;
  void InitTestData();
  bool CheckFuncHot(const std::string &className) const;
  bool CheckMethodHot(const std::string &className) const;
  bool CheckFieldHot(const std::string &className) const;
  bool CheckClassHot(const std::string &className) const;
  bool CheckLiteralHot(const std::string &literal) const;
  bool CheckReflectionStrHot(const std::string &str, uint8 &layoutType) const;
  void InitPreHot();
  // default get all kind profile
  bool DeCompress(const std::string &fileName, const std::string &dexName, ProfileType type = kAll);
  const std::unordered_map<std::string, FuncItem> &GetFunctionProf() const;
  bool GetFunctionBBProf(const std::string &funcName, BBInfo &result);
  size_t GetLiteralProfileSize() const;
  bool CheckProfValid() const;
  bool CheckDexValid(uint32 idx);
  void SetProfileMode();
  void Dump() const;
  void DumpFuncIRProfUseInfo() const;
  void SetFuncStatus(const std::string &funcName, bool succ);
  Profile();
  ~Profile() = default;

  bool IsValid() const {
    return valid;
  }

 private:
  bool valid = false;
  bool initialized = false;
  bool profileMode = false;
  bool isCoreSo = false;
  bool isAppProfile = false;
  static bool debug;
  static uint32 hotFuncCountThreshold;
  std::vector<std::string> strMap;
  std::string dexName;
  std::string appPackageName;
  std::unordered_set<std::string> classMeta;
  std::unordered_set<std::string> methodMeta;
  std::unordered_set<std::string> fieldMeta;
  std::unordered_set<std::string> literal;
  std::unordered_map<std::string, uint8> reflectionStrData;
  std::unordered_map<std::string, Profile::FuncItem> funcProfData;
  std::unordered_set<std::string> &GetMeta(uint8 type);
  std::unordered_map<std::string, BBInfo> funcBBProfData;
  std::unordered_map<std::string, bool> funcBBProfUseInfo;
  std::unordered_map<std::string, IRProfileDesc> funcDesc;
  std::vector<uint32> counterTab;
  bool CheckProfileHeader(const Header *header) const;
  std::string GetProfileNameByType(uint8 type) const;
  std::string GetFunctionName(uint32 classIdx, uint32 methodIdx, uint32 sigIdx);
  void ParseMeta(const char *data, int fileNum, std::unordered_set<std::string> &metaData);
  void ParseReflectionStr(const char *data, int fileNum);
  void ParseFunc(const char *data, int fileNum);
  void ParseLiteral(const char *data, const char *end);
  void ParseIRFuncDesc(const char *data, int fileNum);
  void ParseCounterTab(const char *data, int fileNum);
};
}  // namespace maple
#endif  // MAPLE_UTIL_INCLUDE_PROFILE_H
