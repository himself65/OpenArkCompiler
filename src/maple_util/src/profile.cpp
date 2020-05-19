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
#include "profile.h"
#include <cassert>
#include <cstring>
#include <fstream>
#include <istream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sys/types.h>
#include <cerrno>
#include <algorithm>
#include "namemangler.h"
#include "file_layout.h"
#include "types_def.h"

namespace maple {
const uint8 Profile::kStringEnd = 0x00;
uint32 Profile::hotFuncCountThreshold = 0;
bool Profile::debug = false;
const uint32 kPrecision = 1000000;
// preHot data
static const std::string preClassHot[] = {
    "Ljava/lang/Class;",
    "Ljava/lang/Object;",
    "Ljava/lang/reflect/Field;",
    "Ljava/lang/reflect/Constructor;",
    "Ljava/lang/reflect/Method;",
    "Ljava/lang/reflect/Executable;",
    "Ljava/lang/String;"
};


Profile::Profile() {}

bool Profile::CheckProfileHeader(const Header *header) const {
  return (memcmp(header->magic, kProfileMagic, sizeof(kProfileMagic)) == 0);
}

std::string Profile::GetProfileNameByType(uint8 type) const {
  switch (type) {
    case kFunction:
      return "FUNCTION";
    case kClassMeta:
      return "CLASSMETA";
    case kFieldMeta:
      return "FIELDMETA";
    case kMethodMeta:
      return "METHODMETA";
    case kReflectionStr:
      return "ReflectionStr";
    case kLiteral:
      return "Literal";
    case kBBInfo:
      return "IRProfDescption";
    case kIRCounter:
      return "IRProfCounter";
    case kFileDesc:
      return "FileDescription";
    default:
      CHECK_FATAL(false, "type not found");
  }
  return "";
}

bool Profile::CheckDexValid(uint32 idx) {
  if (isAppProfile) {
    return true; // for app dont't check dexName
  }
  return (dexName.empty() || dexName.find(strMap.at(idx)) != std::string::npos);
}

void Profile::ParseLiteral(const char *data, const char *end) {
  if(data > end) {
    LogInfo::MapleLogger() << "parse Literal error" << '\n';;
  }
  std::string str(data,end-data);
  std::stringstream ss;
  ss.str(str);
  std::string item;
  while (std::getline(ss, item)) {
    literal.insert(item);
  }
  if (debug) {
    LogInfo::MapleLogger() << "parse Literal succ literal size " << literal.size() << "\n";
  }
}

std::string Profile::GetFunctionName(uint32 classIdx, uint32 methodIdx, uint32 sigIdx) {
  std::string className = NameMangler::EncodeName(strMap.at(classIdx));
  std::string methodName = NameMangler::EncodeName(strMap.at(methodIdx));
  std::string sigName = NameMangler::EncodeName(strMap.at(sigIdx));
  std::string funcName = className + "_7C" + methodName + "_7C" + sigName;
  return funcName;
}

void Profile::ParseFunc(const char *data, int fileNum) {
  const MapleFileProf *funcProf = nullptr;
  const FunctionItem *funcItem = nullptr;
  uint32 offset = 0;
  for (int32 mapleFileIdx = 0; mapleFileIdx < fileNum; mapleFileIdx++) {
    funcProf = reinterpret_cast<const MapleFileProf*>(data + offset);
    if (CheckDexValid(funcProf->idx))  {
      if (debug) {
        LogInfo::MapleLogger() << "FuncProfile" << ":" << strMap.at(funcProf->idx) << ":" << funcProf->num << "\n";
      }
      funcItem = reinterpret_cast<const FunctionItem*>(data + offset + sizeof(MapleFileProf));
      for (uint32 item = 0; item < funcProf->num; item++, ++funcItem) {
        if (funcItem->type >= kLayoutTypeCount) {
          if (debug) {
            LogInfo::MapleLogger() << "ParseFunc Error usupport type " << funcItem->type << "\n";
          }
          continue;
        }
        std::string funcName = GetFunctionName(funcItem->classIdx, funcItem->methodIdx, funcItem->sigIdx);
        funcProfData.insert(
            std::make_pair(funcName, (FuncItem){ .callTimes = funcItem->callTimes, .type = funcItem->type }));
      }
    }
    // new maple file's profile
    offset += sizeof(MapleFileProf) + funcProf->size;
  }
}

void Profile::ParseIRFuncDesc(const char *data, int fileNum) {
  const MapleFileProf *funcProf = nullptr;
  const FunctionIRProfItem *funcItem = nullptr;
  uint32 offset = 0;
  for (int32 mapleFileIdx = 0; mapleFileIdx < fileNum; mapleFileIdx++) {
    funcProf = reinterpret_cast<const MapleFileProf*>(data + offset);
    if (CheckDexValid(funcProf->idx))  {
      if (debug) {
        LogInfo::MapleLogger() << "IRFuncProfile" << ":" << strMap.at(funcProf->idx) << ":" << funcProf->num << "\n";
      }
      funcItem = reinterpret_cast<const FunctionIRProfItem*>(data + offset + sizeof(MapleFileProf));
      for (uint32 item = 0; item < funcProf->num; ++item, ++funcItem) {
        if ((funcItem->counterStart <= counterTab.size()) && (funcItem->counterEnd <= counterTab.size())) {
          auto begin = counterTab.begin() + funcItem->counterStart;
          auto end = counterTab.begin() + funcItem->counterEnd +  1;
          std::vector<uint32> tempCounter(begin, end);
          BBInfo bbInfo(funcItem->hash, tempCounter.size(), std::move(tempCounter));
          std::string funcName = GetFunctionName(funcItem->classIdx, funcItem->methodIdx, funcItem->sigIdx);
          funcBBProfData.emplace(std::make_pair(funcName, bbInfo));
        }
      }
    }
    // new maple file's profile
    offset += sizeof(MapleFileProf) + funcProf->size;
  }
}

void Profile::ParseCounterTab(const char *data, int fileNum) {
  const MapleFileProf *counterProf = nullptr;
  uint32 offset = 0;
  for (int32 mapleFileIdx = 0; mapleFileIdx < fileNum; mapleFileIdx++) {
    counterProf = reinterpret_cast<const MapleFileProf*>(data  + offset);
    if (CheckDexValid(counterProf->idx)) {
      if (debug) {
        LogInfo::MapleLogger() << "module name " << strMap.at(counterProf->idx) << std::endl;
      }
      const FuncCounterItem *item = reinterpret_cast<const FuncCounterItem*>(data + offset + sizeof(MapleFileProf));
      for (uint32 i = 0; i < counterProf->num; ++i, ++item) {
        counterTab.emplace_back(item->callTimes);
      }
    }
    offset += sizeof(MapleFileProf) + counterProf->size;
  }
}

void Profile::ParseMeta(const char *data, int fileNum, std::unordered_set<std::string> &metaData) {
  const MapleFileProf *metaProf = nullptr;
  uint32 offset = 0;
  for (int32 mapleFileIdx = 0; mapleFileIdx < fileNum; mapleFileIdx++) {
    metaProf = reinterpret_cast<const MapleFileProf*>(data  + offset);
    if (CheckDexValid(metaProf->idx)) {
      if (debug) {
        LogInfo::MapleLogger() << "module name " << strMap.at(metaProf->idx) << '\n';
      }
      const MetaItem *metaItem = reinterpret_cast<const MetaItem*>(data + offset + sizeof(MapleFileProf));
      for (uint32 item = 0; item < metaProf->num; ++item, ++metaItem) {
        metaData.insert(strMap.at(metaItem->idx));
      }
    }
    offset += sizeof(MapleFileProf) + metaProf->size;
  }
}

void Profile::ParseReflectionStr(const char *data, int fileNum) {
  const MapleFileProf *metaProf = nullptr;
  uint32 offset = 0;
  for (int32 mapleFileIdx = 0; mapleFileIdx < fileNum; mapleFileIdx++) {
    metaProf = reinterpret_cast<const MapleFileProf*>(data  + offset);
    if (CheckDexValid(metaProf->idx)) {
      if (debug) {
        LogInfo::MapleLogger() << "module name " << strMap.at(metaProf->idx) << '\n';
      }

      const ReflectionStrItem *strItem =
          reinterpret_cast<const ReflectionStrItem*>(data + offset + sizeof(MapleFileProf));
      for (uint32 item = 0; item < metaProf->num; ++item, ++strItem) {
        reflectionStrData.insert(std::make_pair(strMap.at(strItem->idx), strItem->type));
      }
    }
    offset += sizeof(MapleFileProf) + metaProf->size;
  }
}

void Profile::InitPreHot() {
  const char *kcoreDexName = "core-all";
  if (dexName.find(kcoreDexName) != std::string::npos) {
    for (auto &item : preClassHot) {
      classMeta.insert(item);
    }
    isCoreSo = true;
  }
}

bool Profile::DeCompress(const std::string &path, const std::string &dexNameInner, ProfileType type) {
  if (initialized) {
    return valid;
  }
  initialized = true;

  this->dexName = dexNameInner;
  InitPreHot();
  bool res = true;
  std::ifstream in(path, std::ios::binary);
  if (!in) {
    if (errno != ENOENT && errno != EACCES) {
      LogInfo::MapleLogger() << "WARN: DeCompress("
                << "), failed to open " << path << '\n';;
    }
    res = false;
    return res;
  }
  in.seekg(0, std::ios::end);
  size_t byteCount = static_cast<size_t>(in.tellg());
  in.seekg(0, std::ios::beg);
  std::vector<char> bufVector;
  bufVector.resize(byteCount);
  char *buf = reinterpret_cast<char*>(bufVector.data());
  if (!in.read(buf, byteCount)) {
    LogInfo::MapleLogger() << "WARN: DeCompress("
              << "), failed to read all data for " << path << '\n';;
    res = false;
    in.close();
    return res;
  }
  if (byteCount < sizeof(Header)) {
    LogInfo::MapleLogger() << "WARN: DeCompress("
              << "), failed, read no data for " << path << '\n';;
    res = false;
    in.close();
    return res;
  }
  Header *header = reinterpret_cast<Header*>(buf);
  if (!CheckProfileHeader(header)) {
    if (debug) {
      LogInfo::MapleLogger() << "invalid maigc number " << header->magic << '\n';;
    }
    res = false;
    in.close();
    return res;
  }
  this->isAppProfile = (header->profileFileType == kApp) ? true : false;
  size_t stringTabSize = byteCount - header->stringTabOff;
  if (debug) {
    LogInfo::MapleLogger() << "Header summary "
              << "profile num " << static_cast<uint32>(header->profileNum) << "string table size" << stringTabSize
              << '\n';;
  }
  const char *strBuf = buf + header->stringTabOff;
  const char *cursor = strBuf;
  const char *sentinel = strBuf + stringTabSize;
  while (cursor < sentinel) {
    size_t len = std::strlen(cursor);
    const char *next = cursor + len + 1;
    strMap.emplace_back(cursor);
    cursor = next;
  }
  ASSERT(strMap.size() == header->stringCount, "string count doesn't match");
  if (debug) {
    LogInfo::MapleLogger() << "str size " << strMap.size() << '\n';;
    for (const auto &item : strMap) {
      LogInfo::MapleLogger() << item << '\n';;
    }
    LogInfo::MapleLogger() << "str size print end  " << '\n';;
  }
  size_t idx = 0;
  for (idx = 0; idx < header->profileNum; idx++) {
    ProfileDataInfo *profileDataInfo = &(header->data[idx]);
    if (debug) {
      LogInfo::MapleLogger() << "profile file num for type  " << GetProfileNameByType(profileDataInfo->profileType) << " "
                << static_cast<uint32>(profileDataInfo->mapleFileNum) << '\n';;
      }
    if (debug) {
      LogInfo::MapleLogger() << GetProfileNameByType(profileDataInfo->profileType) << " Start" << '\n';;
    }
    char *proFileData = buf + profileDataInfo->profileDataOff;
    if (type != kAll && type != profileDataInfo->profileType) {
      continue; // only parse the indicated type
    }
    switch(profileDataInfo->profileType) {
      case kFunction:
        ParseFunc(proFileData, profileDataInfo->mapleFileNum);
        break;
      case kClassMeta:
        ParseMeta(proFileData, profileDataInfo->mapleFileNum, classMeta);
        break;
      case kFieldMeta:
        ParseMeta(proFileData, profileDataInfo->mapleFileNum, fieldMeta);
        break;
      case kMethodMeta:
        ParseMeta(proFileData, profileDataInfo->mapleFileNum, methodMeta);
        break;
      case kReflectionStr:
        ParseReflectionStr(proFileData, profileDataInfo->mapleFileNum);
        break;
      case kLiteral:
        ParseLiteral(proFileData,strBuf);
        break;
      case kBBInfo:
        ParseIRFuncDesc(proFileData, profileDataInfo->mapleFileNum);
        break;
      case kIRCounter:
        ParseCounterTab(proFileData, profileDataInfo->mapleFileNum);
        break;
      case kFileDesc: {
        uint32_t appPackageNameIdx = *reinterpret_cast<uint32_t*>(proFileData);
        this->appPackageName = strMap.at(appPackageNameIdx);
        if (!appPackageName.empty() && this->appPackageName != appPackageName) {
          LogInfo::MapleLogger() << "app profile doesnt match expect " << this->appPackageName
                                 << " but got " << appPackageName << '\n';;
          return false;
        }
        break;
      }
      default:
        LogInfo::MapleLogger() << "unsupported tag " << profileDataInfo->profileType << '\n';;
        break;
    }
  }
  LogInfo::MapleLogger() << "SUCC parse " << path << '\n';;
  valid = true;
  in.close();
  return res;
}

void Profile::SetProfileMode() {
  profileMode = true;
}

bool Profile::CheckFuncHot(const std::string &funcName) const {
  if (funcProfData.empty()) {
    return false;
  }
  if (valid) {
    auto iter = funcProfData.find(funcName);
    if (iter == funcProfData.end()) {
      return false;
    }
    if (hotFuncCountThreshold == 0) {
      if (Options::profileHotCountSeted) {
        hotFuncCountThreshold = Options::profileHotCount;
      } else {
        std::vector<uint32> times;
        for (auto &item : funcProfData) {
          times.push_back((item.second).callTimes);
        }
        std::sort(times.begin(), times.end(), std::greater<uint32>());
        size_t index = static_cast<size_t>(static_cast<double>(times.size()) / kPrecision * (Options::profileHotRate));
        hotFuncCountThreshold = times.at(index);
      }
    }
    return (iter->second).callTimes >= hotFuncCountThreshold;
  }
  return false;
}

bool Profile::CheckMethodHot(const std::string &className) const {
  if (methodMeta.empty()) {
    return true;
  }
  if (valid) {
    if (methodMeta.find(className) == methodMeta.end()) {
      return false;
    }
    return true;
  }
  return false;
}

bool Profile::CheckFieldHot(const std::string &className) const {
  if (fieldMeta.empty()) {
    return true;
  }
  if (valid) {
    if (fieldMeta.find(className) == fieldMeta.end()) {
      return false;
    }
    return true;
  }
  return false;
}

bool Profile::CheckClassHot(const std::string &className) const {
  // If in mode sample all class set to cold, except for core-all so,core-all have
  // some hot class pre defined
  if (profileMode && !isCoreSo) {
    return false;
  }
  if (classMeta.empty()) {
    return true;
  }
  if (valid || isCoreSo) {
    return classMeta.find(className) != classMeta.end();
  }
  return false;
}

bool Profile::CheckLiteralHot(const std::string &literalInner) const {
  if (valid) {
    if ((this->literal).find(literalInner) == (this->literal).end()) {
      return false;
    }
    return true;
  }
  return false;
}

size_t Profile::GetLiteralProfileSize() const {
  if (valid) {
    return literal.size();
  }
  return 0;
}

bool Profile::CheckReflectionStrHot(const std::string &str, uint8 &layoutType) const {
  if (valid) {
    auto item = reflectionStrData.find(str);
    if (item == reflectionStrData.end()) {
      return false;
    }
    layoutType = item->second;
    return true;
  }
  return false;
}

const std::unordered_map<std::string, Profile::FuncItem>& Profile::GetFunctionProf() const {
  return funcProfData;
}

bool Profile::GetFunctionBBProf(const std::string &funcName, Profile::BBInfo &result) {
  auto item = funcBBProfData.find(funcName);
  if (item == funcBBProfData.end()) {
    return false;
  }
  result = item->second;
  return true;
}

std::unordered_set<std::string> &Profile::GetMeta(uint8 type) {
  switch (type) {
    case kClassMeta:
      return classMeta;
    case kFieldMeta:
      return fieldMeta;
    case kMethodMeta:
      return methodMeta;
    default:
      CHECK_FATAL(0, "type not found");
      return classMeta;
  }
}

void Profile::SetFuncStatus(const std::string &funcName, bool succ) {
  if (succ) {
    funcBBProfUseInfo[funcName] = true;
  }
}

void Profile::DumpFuncIRProfUseInfo() const {
  if (funcBBProfData.empty()) {
    return;
  }
  LogInfo::MapleLogger() << "ir profile succ  " << funcBBProfUseInfo.size() <<  " total func "
                         << funcBBProfData.size() << '\n';
}

void Profile::Dump() const {
  std::ofstream outFile;
  outFile.open("prof.dump");
  outFile << "classMeta profile start " <<'\n';;
  for (const auto &item : classMeta) {
    outFile << item << '\n';;
  }

  outFile << "fieldMeta profile start " <<'\n';;
  for (const auto &item : fieldMeta) {
    outFile << item << '\n';;
  }

  outFile << "methodMeta profile start " <<'\n';;
  for (const auto &item : methodMeta) {
    outFile << item << '\n';;
  }

  outFile << "literal profile start " <<'\n';;
  for (const auto &item : literal) {
    outFile << item << '\n';;
  }

  outFile << "func profile start " <<'\n';;
  for (const auto &item : funcProfData) {
    outFile << item.first << " "
            << static_cast<uint32>((item.second).type) << " " << (item.second).callTimes << '\n';;
  }

  outFile << "reflectStr profile start " <<'\n';;
  for (const auto &item : reflectionStrData) {
    outFile << item.first << " " << static_cast<uint32>(item.second) << '\n';;
  }
  outFile.close();
}

}  // namespace maple
