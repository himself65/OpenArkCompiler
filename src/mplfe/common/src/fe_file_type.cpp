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
#include "fe_file_type.h"
#include <fstream>
#include "mpl_logging.h"

namespace maple {
FEFileType FEFileType::fileType;

FEFileType::FEFileType() {
  LoadDefault();
}

FEFileType::FileType FEFileType::GetFileTypeByExtName(const std::string &extName) const {
  if (extName.empty()) {
    WARN(kLncWarn, "Empty input for GetFileTypeByExtName()...skipped");
    return kUnknown;
  }
  auto itExtNameType = mapExtNameType.find(extName);
  if (itExtNameType == mapExtNameType.end()) {
    WARN(kLncWarn, "Unknown file extension name %s...skipped", extName.c_str());
    return kUnknown;
  }
  return itExtNameType->second;
}

FEFileType::FileType FEFileType::GetFileTypeByPathName(const std::string &pathName) const {
  std::string extName = GetExtName(pathName);
  return GetFileTypeByExtName(extName);
}

FEFileType::FileType FEFileType::GetFileTypeByMagicNumber(const std::string &pathName) const {
  std::ifstream file(pathName);
  if (!file.is_open()) {
    ERR(kLncErr, "unable to open file %s", pathName.c_str());
    return kUnknown;
  }
  uint32 magic = 0;
  (void)file.read(reinterpret_cast<char*>(&magic), sizeof(uint32));
  file.close();
  return GetFileTypeByMagicNumber(magic);
}

FEFileType::FileType FEFileType::GetFileTypeByMagicNumber(BasicIOMapFile &file) const {
  BasicIORead fileReader(file);
  bool success = false;
  uint32 magic = fileReader.ReadUInt32(success);
  if (!success) {
    ERR(kLncErr, "unable to open file %s", file.GetFileName().c_str());
    return kUnknown;
  }
  return GetFileTypeByMagicNumber(magic);
}

FEFileType::FileType FEFileType::GetFileTypeByMagicNumber(uint32 magic) const {
  std::map<uint32, FileType>::const_iterator it = mapMagicType.find(magic);
  if (it != mapMagicType.end()) {
    return it->second;
  } else {
    return kUnknown;
  }
}

void FEFileType::Reset() {
  mapExtNameType.clear();
  mapTypeMagic.clear();
  mapMagicType.clear();
}

void FEFileType::LoadDefault() {
  Reset();
  RegisterExtName(kClass, "class");
  RegisterMagicNumber(kClass, kMagicClass);
  RegisterExtName(kJar, "jar");
  RegisterMagicNumber(kJar, kMagicZip);
}

void FEFileType::RegisterExtName(FileType argFileType, const std::string &extName) {
  if (extName.empty() || argFileType == kUnknown) {
    WARN(kLncWarn, "Invalid input for RegisterMagicNumber()...skipped");
    return;
  }
  mapExtNameType[extName] = argFileType;
  mapTypeMagic[argFileType] = 0;
}

void FEFileType::RegisterMagicNumber(FileType argFileType, uint32 magicNumber) {
  if (magicNumber == 0 || argFileType == kUnknown) {
    WARN(kLncWarn, "Invalid input for RegisterMagicNumber()...skipped");
    return;
  }
  mapTypeMagic[argFileType] = magicNumber;
  mapMagicType[magicNumber] = argFileType;
}

std::string FEFileType::GetPath(const std::string &pathName) {
  size_t pos = pathName.find_last_of('/');
  if (pos != std::string::npos) {
    return pathName.substr(0, pos + 1);
  } else {
    return "";
  }
}

std::string FEFileType::GetName(const std::string &pathName, bool withExt) {
  size_t pos = pathName.find_last_of('/');
  std::string name = "";
  if (pos != std::string::npos) {
    name = pathName.substr(pos + 1);
  } else {
    name = pathName;
  }
  if (withExt) {
    return name;
  }
  size_t posDot = name.find_last_of('.');
  if (posDot != std::string::npos) {
    return name.substr(0, pos);
  } else {
    return name;
  }
}

std::string FEFileType::GetExtName(const std::string &pathName) {
  std::string name = GetName(pathName, true);
  size_t pos = name.find_last_of('.');
  if (pos != std::string::npos) {
    return name.substr(pos + 1);
  } else {
    return "";
  }
}
}  // namespace maple