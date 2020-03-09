/*
 * Copyright (c) [2019] Huawei Technologies Co.,Ltd.All rights reserved.
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
class Profile {
 public:
  struct FuncItem {
    uint32 callTimes;
    uint8 type;
  };

  static const uint8 kStringEnd;
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
  size_t GetLiteralProfileSize() const;
  bool CheckProfValid() const;
  bool CheckDexValid(uint32 idx);
  void SetProfileMode();
  void Dump() const;
  Profile();
  ~Profile() = default;

 private:
  bool valid;
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
  bool CheckProfileHeader(const Header *header) const;
  std::string GetProfileNameByType(uint8 type) const;
  void ParseMeta(const char *data, int fileNum, std::unordered_set<std::string> &metaData);
  void ParseReflectionStr(const char *data, int fileNum);
  void ParseFunc(const char *data, int fileNum);
  void ParseLiteral(const char *data, const char *end);
};

}  // namespace maple
#endif  // MAPLE_UTIL_INCLUDE_PROFILE_H
