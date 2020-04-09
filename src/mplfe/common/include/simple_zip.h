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
// ref: https://pkware.cachefly.net/webdocs/casestudies/APPNOTE.TXT
#ifndef MPLFE_INCLUDE_SIMPLE_ZIP_H
#define MPLFE_INCLUDE_SIMPLE_ZIP_H
#include <list>
#include <memory>
#include <string>
#include "types_def.h"
#include "basic_io.h"

namespace maple {
static const uint32 kZipSigLocalFile = 0x04034B50;
static const uint32 kZipSigDataDescriptor = 0x08074B50;
static const uint32 kZipSigCentralDir = 0x02014B50;

class ZipLocalFileHeader {
 public:
  ZipLocalFileHeader() = default;
  ~ZipLocalFileHeader();
  static std::unique_ptr<ZipLocalFileHeader> Parse(BasicIORead &io);
  uint16 GetGPFlag() const {
    return gpFlag;
  }

  std::string GetFileName() const {
    return fileName;
  }

 private:
  uint32 signature = 0;
  uint16 minVersion = 0;
  uint16 gpFlag = 0;
  uint16 compMethod = 0;
  uint16 rawTime = 0;
  uint16 rawDate = 0;
  uint32 crc32 = 0;
  uint32 compSize = 0;
  uint32 unCompSize = 0;
  uint16 lengthFileName = 0;
  uint16 lengthExtraField = 0;
  std::string fileName = "";
  uint8 *extraField = nullptr;
};

class ZipDataDescriptor {
 public:
  ZipDataDescriptor() = default;
  ~ZipDataDescriptor() = default;
  static std::unique_ptr<ZipDataDescriptor> Parse(BasicIORead &io);
  uint32 GetSignature() const {
    return signature;
  }

  uint32 GetCRC32() const {
    return crc32;
  }

  uint32 GetCompSize() const {
    return compSize;
  }

  uint32 GetUnCompSize() const {
    return unCompSize;
  }

 private:
  uint32 signature = 0;
  uint32 crc32 = 0;
  uint32 compSize = 0;
  uint32 unCompSize = 0;
};

class ZipLocalFile {
 public:
  ZipLocalFile() = default;
  ~ZipLocalFile();
  static std::unique_ptr<ZipLocalFile> Parse(BasicIORead &io);
  std::string GetFileName() const {
    return header->GetFileName();
  }

  const uint8 *GetUnCompData() const {
    return unCompData;
  }

  uint32 GetUnCompDataSize() const {
    return unCompDataSize;
  }

 private:
  uint32 GetDataEndPos(const BasicIORead &io);
  void ProcessUncompressedFile(BasicIORead &io, uint32 start, uint32 end);
  void ProcessCompressedFile(BasicIORead &io, uint32 start, uint32 end);

  std::unique_ptr<ZipLocalFileHeader> header;
  uint8 *compData = nullptr;
  uint8 *unCompData = nullptr;
  uint32 unCompDataSize = 0;
  std::unique_ptr<ZipDataDescriptor> dataDesc;
  bool isCompressed = false;
};

class SimpleZip : public BasicIORead {
 public:
  SimpleZip(BasicIOMapFile &file);
  ~SimpleZip();
  bool ParseFile();

  const std::list<std::unique_ptr<ZipLocalFile>> &GetFiles() const {
    return files;
  }

 private:
  std::list<std::unique_ptr<ZipLocalFile>> files;
};
}  // namespace maple
#endif
