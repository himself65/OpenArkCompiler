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
#ifndef MPLFE_INCLUDE_COMMON_FE_FILE_TYPE_H
#define MPLFE_INCLUDE_COMMON_FE_FILE_TYPE_H
#include <string>
#include <map>
#include "types_def.h"
#include "basic_io.h"

namespace maple {
class FEFileType {
 public:
  enum FileType {
    kUnknown,
    kClass,
    kJar,
  };

  inline static FEFileType &GetInstance() {
    return fileType;
  }

  FileType GetFileTypeByExtName(const std::string &extName) const;
  FileType GetFileTypeByPathName(const std::string &pathName) const;
  FileType GetFileTypeByMagicNumber(const std::string &pathName) const;
  FileType GetFileTypeByMagicNumber(BasicIOMapFile &file) const;
  FileType GetFileTypeByMagicNumber(uint32 magic) const;
  void Reset();
  void LoadDefault();
  void RegisterExtName(FileType fileType, const std::string &extName);
  void RegisterMagicNumber(FileType fileType, uint32 magicNumber);
  static std::string GetPath(const std::string &pathName);
  static std::string GetName(const std::string &pathName, bool withExt = true);
  static std::string GetExtName(const std::string &pathName);

 private:
  static FEFileType fileType;
  static const uint32 kMagicClass = 0xBEBAFECA;
  static const uint32 kMagicZip = 0x04034B50;
  std::map<std::string, FileType> mapExtNameType;
  std::map<FileType, uint32> mapTypeMagic;
  std::map<uint32, FileType> mapMagicType;

  FEFileType();
  ~FEFileType() = default;
};
}
#endif