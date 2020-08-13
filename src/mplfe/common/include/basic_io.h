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
#ifndef MPLFE_INCLUDE_COMMON_BASIC_IO_H
#define MPLFE_INCLUDE_COMMON_BASIC_IO_H
#include <climits>
#include <string>
#include <memory>
#include "mempool_allocator.h"
#include "types_def.h"
#include "mpl_logging.h"
#include "securec.h"

namespace maple {
class BasicIOEndian {
 public:
  static const uint32 kLengthByte = 1;
  static const uint32 kLengthWord = 2;
  static const uint32 kLengthDWord = 4;
  static const uint32 kLengthQWord = 8;

  static uint16 GetUInt16BigEndian(const uint8 *p) {
    // for performance, validation of p must be done by caller
    return static_cast<uint16>((p[kBufIndex0] << kOffset8) | p[kBufIndex1]);
  }

  static uint16 GetUInt16LittleEndian(const uint8 *p) {
    // for performance, validation of p must be done by caller
    return static_cast<uint16>((p[kBufIndex1] << kOffset8) | p[kBufIndex0]);
  }

  static uint32 GetUInt32BigEndian(const uint8 *p) {
    // for performance, validation of p must be done by caller
    uint32 value = 0;
    for (uint32 i = 0; i < kLengthDWord; i++) {
      value = (value << kOffset8) | p[i];
    }
    return value;
  }

  static uint32 GetUInt32LittleEndian(const uint8 *p) {
    // for performance, validation of p must be done by caller
    uint32 value = 0;
    for (int i = static_cast<int>(kLengthDWord) - 1; i >= 0; i--) {
      value = (value << kOffset8) | p[i];
    }
    return value;
  }

  static uint64 GetUInt64BigEndian(const uint8 *p) {
    // for performance, validation of p must be done by caller
    uint64 value = 0;
    for (uint32 i = 0; i < kLengthQWord; i++) {
      value = (value << kOffset8) | p[i];
    }
    return value;
  }

  static uint64 GetUInt64LittleEndian(const uint8 *p) {
    // for performance, validation of p must be done by caller
    uint64 value = 0;
    for (int i = static_cast<int>(kLengthQWord) - 1; i >= 0; i--) {
      value = (value << kOffset8) | p[i];
    }
    return value;
  }

 private:
  static const uint32 kBufIndex0 = 0;
  static const uint32 kBufIndex1 = 1;
  static const uint32 kBufIndex2 = 2;
  static const uint32 kBufIndex3 = 3;
  static const uint32 kOffset8 = 8;
  static const uint32 kOffset16 = 16;
  static const uint32 kOffset24 = 24;

  BasicIOEndian() = default;
  ~BasicIOEndian() = default;
};

class BasicIOMapFile {
 public:
  explicit BasicIOMapFile(const std::string &name);
  BasicIOMapFile(const std::string &name, const uint8 *ptrIn, long lengthIn);
  virtual ~BasicIOMapFile();
  bool OpenAndMap();
  void Close();
  static std::unique_ptr<BasicIOMapFile> GenFileInMemory(const std::string &name, const uint8 *buf, size_t len);

  size_t GetLength() const {
    return length;
  }

  const uint8 *GetPtr() const {
    return ptr;
  }

  const uint8 *GetPtrOffset(size_t offset) const {
    if (offset >= length) {
      CHECK_FATAL(false, "offset is out of range");
      return nullptr;
    }
    return ptr + offset;
  }

  const std::string &GetFileName() const {
    return fileName;
  }

 protected:
  int fd;
  const uint8 *ptr;
  uint8 *ptrMemMap;
  size_t length;
  std::string fileName;
};

class BasicIORead {
 public:
  BasicIORead(BasicIOMapFile &f, bool bigEndian = false);
  virtual ~BasicIORead() = default;
  uint8 ReadUInt8();
  uint8 ReadUInt8(bool &success);
  int8 ReadInt8();
  int8 ReadInt8(bool &success);
  char ReadChar();
  char ReadChar(bool &success);
  uint16 ReadUInt16();
  uint16 ReadUInt16(bool &success);
  int16 ReadInt16();
  int16 ReadInt16(bool &success);
  uint32 ReadUInt32();
  uint32 ReadUInt32(bool &success);
  int32 ReadInt32();
  int32 ReadInt32(bool &success);
  uint64 ReadUInt64();
  uint64 ReadUInt64(bool &success);
  int64 ReadInt64();
  int64 ReadInt64(bool &success);
  float ReadFloat();
  float ReadFloat(bool &success);
  double ReadDouble();
  double ReadDouble(bool &success);
  void ReadBufferUInt8(uint8 *dst, uint32 length);
  void ReadBufferUInt8(uint8 *dst, uint32 length, bool &success);
  void ReadBufferInt8(int8 *dst, uint32 length);
  void ReadBufferInt8(int8 *dst, uint32 length, bool &success);
  void ReadBufferChar(char *dst, uint32 length);
  void ReadBufferChar(char *dst, uint32 length, bool &success);
  std::string ReadString(uint32 length);
  std::string ReadString(uint32 length, bool &success);

  const uint8 *GetBuffer(uint32 size) const {
    if (pos + size > file.GetLength()) {
      ERR(kLncErr, "BasicIORead: not enough data");
      return nullptr;
    } else {
      return file.GetPtrOffset(pos);
    }
  }

  const uint8 *GetSafeBuffer(uint32 size) const {
    CHECK_FATAL(pos + size <= file.GetLength(), "not enough data");
    return file.GetPtrOffset(pos);
  }

  const uint8 *GetSafeBufferAt(uint32 pos0, uint32 size) const {
    CHECK_FATAL(pos0 + size <= file.GetLength(), "not enough data");
    return file.GetPtrOffset(pos0);
  }

  const uint8 *GetSafeDataAt(uint32 offset) const {
    CHECK_FATAL(0 < offset && offset < file.GetLength(), "Invalid offset: 0x%x, file total size: 0x%x",
                offset, file.GetLength());
    return file.GetPtrOffset(offset);
  }

  uint32 GetPos() const {
    return pos;
  }

  void SetPos(uint32 p) {
    CHECK_FATAL(p < file.GetLength(), "invalid pos %d exceeds the length of file %ld", p, file.GetLength());
    pos = p;
  }

  size_t GetFileLength() const {
    return file.GetLength();
  }

 protected:
  BasicIOMapFile &file;
  bool isBigEndian;
  uint32 pos;
};
}  // namespace maple
#endif
