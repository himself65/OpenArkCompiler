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
#include "basic_io.h"
#include <cerrno>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <cstdlib>
#include <unistd.h>
#include <sys/mman.h>
#include "mpl_logging.h"

namespace maple {
BasicIOMapFile::BasicIOMapFile(const std::string &name)
    : fd(-1), ptr(nullptr), ptrMemMap(nullptr), length(0), fileName(name) {}

BasicIOMapFile::BasicIOMapFile(const std::string &name, const uint8 *ptrIn, long lengthIn)
    : fd(-1), ptr(ptrIn), ptrMemMap(nullptr), length(lengthIn), fileName(name) {}

BasicIOMapFile::~BasicIOMapFile() {
  ptr = nullptr;
  ptrMemMap = nullptr;
}

bool BasicIOMapFile::OpenAndMap() {
  fd = -1;
  fd = open(fileName.c_str(), O_RDONLY);
  if (fd < 0) {
    ERR(kLncErr, "Unable to open %s.\nError %d in open()", fileName.c_str(), errno);
    return false;
  }
  long start = lseek(fd, 0L, SEEK_SET);
  long end = lseek(fd, 0L, SEEK_END);
  if (end > start) {
    length = static_cast<size_t>(end - start);
    ptrMemMap = static_cast<uint8*>(mmap(NULL, length, PROT_READ, MAP_FILE | MAP_PRIVATE, fd, start));
    ptr = ptrMemMap;
    return true;
  } else {
    length = 0;
    ptrMemMap = nullptr;
    ptr = nullptr;
    if (close(fd) != 0) {
      FATAL(kLncFatal, "close error");
    }
    return false;
  }
}

void BasicIOMapFile::Close() {
  if (fd > 0) {
    if (munmap(ptrMemMap, length) != 0) {
      FATAL(kLncFatal, "munmap error");
    }
    ptrMemMap = nullptr;
    ptr = nullptr;
    if (close(fd) != 0) {
      FATAL(kLncFatal, "close error");
    }
  }
  fd = -1;
}

std::unique_ptr<BasicIOMapFile> BasicIOMapFile::GenFileInMemory(const std::string &name, const uint8 *buf, size_t len) {
  std::unique_ptr<BasicIOMapFile> file = std::make_unique<BasicIOMapFile>(name);
  file->ptr = buf;
  file->length = len;
  return file;
}

BasicIORead::BasicIORead(BasicIOMapFile &f, bool bigEndian) : file(f), isBigEndian(bigEndian), pos(0) {}

uint8 BasicIORead::ReadUInt8() {
  const uint8 *p = GetSafeBuffer(BasicIOEndian::kLengthByte);
  pos += BasicIOEndian::kLengthByte;
  return p[0];
}

uint8 BasicIORead::ReadUInt8(bool &success) {
  const uint8 *p = GetBuffer(BasicIOEndian::kLengthByte);
  if (p == nullptr) {
    success = false;
    return 0;
  }
  pos += BasicIOEndian::kLengthByte;
  success = true;
  return p[0];
}

int8 BasicIORead::ReadInt8() {
  return static_cast<int8>(ReadUInt8());
}

int8 BasicIORead::ReadInt8(bool &success) {
  return static_cast<int8>(ReadUInt8(success));
}

char BasicIORead::ReadChar() {
  return static_cast<char>(ReadUInt8());
}

char BasicIORead::ReadChar(bool &success) {
  return static_cast<char>(ReadUInt8(success));
}

uint16 BasicIORead::ReadUInt16() {
  const uint8 *p = GetSafeBuffer(BasicIOEndian::kLengthWord);
  pos += BasicIOEndian::kLengthWord;
  if (isBigEndian) {
    return BasicIOEndian::GetUInt16BigEndian(p);
  } else {
    return BasicIOEndian::GetUInt16LittleEndian(p);
  }
}

uint16 BasicIORead::ReadUInt16(bool &success) {
  const uint8 *p = GetBuffer(BasicIOEndian::kLengthWord);
  if (p == nullptr) {
    success = false;
    return 0;
  }
  pos += BasicIOEndian::kLengthWord;
  success = true;
  if (isBigEndian) {
    return BasicIOEndian::GetUInt16BigEndian(p);
  } else {
    return BasicIOEndian::GetUInt16LittleEndian(p);
  }
}

int16 BasicIORead::ReadInt16() {
  return static_cast<int16>(ReadUInt16());
}

int16 BasicIORead::ReadInt16(bool &success) {
  return static_cast<int16>(ReadUInt16(success));
}

uint32 BasicIORead::ReadUInt32() {
  const uint8 *p = GetSafeBuffer(BasicIOEndian::kLengthDWord);
  pos += BasicIOEndian::kLengthDWord;
  if (isBigEndian) {
    return BasicIOEndian::GetUInt32BigEndian(p);
  } else {
    return BasicIOEndian::GetUInt32LittleEndian(p);
  }
}

uint32 BasicIORead::ReadUInt32(bool &success) {
  const uint8 *p = GetBuffer(BasicIOEndian::kLengthDWord);
  if (p == nullptr) {
    success = false;
    return 0;
  }
  pos += BasicIOEndian::kLengthDWord;
  success = true;
  if (isBigEndian) {
    return BasicIOEndian::GetUInt32BigEndian(p);
  } else {
    return BasicIOEndian::GetUInt32LittleEndian(p);
  }
}

int32 BasicIORead::ReadInt32() {
  return static_cast<int32>(ReadUInt32());
}

int32 BasicIORead::ReadInt32(bool &success) {
  return static_cast<int32>(ReadUInt32(success));
}

uint64 BasicIORead::ReadUInt64() {
  const uint8 *p = GetSafeBuffer(BasicIOEndian::kLengthQWord);
  pos += BasicIOEndian::kLengthQWord;
  if (isBigEndian) {
    return BasicIOEndian::GetUInt64BigEndian(p);
  } else {
    return BasicIOEndian::GetUInt64LittleEndian(p);
  }
}

uint64 BasicIORead::ReadUInt64(bool &success) {
  const uint8 *p = GetBuffer(BasicIOEndian::kLengthQWord);
  if (p == nullptr) {
    success = false;
    return 0;
  }
  pos += BasicIOEndian::kLengthQWord;
  success = true;
  if (isBigEndian) {
    return BasicIOEndian::GetUInt64BigEndian(p);
  } else {
    return BasicIOEndian::GetUInt64LittleEndian(p);
  }
}

int64 BasicIORead::ReadInt64() {
  return static_cast<int64>(ReadUInt64());
}

int64 BasicIORead::ReadInt64(bool &success) {
  return static_cast<int64>(ReadUInt64(success));
}

float BasicIORead::ReadFloat() {
  union {
    uint32 iv;
    float fv;
  } v;
  v.iv = ReadUInt32();
  return v.fv;
}

float BasicIORead::ReadFloat(bool &success) {
  union {
    uint32 iv;
    float fv;
  } v;
  v.iv = ReadUInt32(success);
  return v.fv;
}

double BasicIORead::ReadDouble() {
  union {
    uint64 lv;
    double dv;
  } v;
  v.lv = ReadUInt64();
  return v.dv;
}

double BasicIORead::ReadDouble(bool &success) {
  union {
    uint64 lv;
    double dv;
  } v;
  v.lv = ReadUInt64(success);
  return v.dv;
}

void BasicIORead::ReadBufferUInt8(uint8 *dst, uint32 length) {
  const uint8 *p = GetSafeBuffer(length);
  pos += length;
  errno_t err = memcpy_s(dst, length, p, length);
  CHECK_FATAL(err == EOK, "memcpy_s failed");
}

void BasicIORead::ReadBufferUInt8(uint8 *dst, uint32 length, bool &success) {
  const uint8 *p = GetBuffer(length);
  if (p == nullptr) {
    success = false;
    return;
  }
  pos += length;
  success = true;
  errno_t err = memcpy_s(dst, length, p, length);
  CHECK_FATAL(err == EOK, "memcpy_s failed");
}

void BasicIORead::ReadBufferInt8(int8 *dst, uint32 length) {
  CHECK_NULL_FATAL(dst);
  const uint8 *p = GetSafeBuffer(length);
  pos += length;
  errno_t err = memcpy_s(dst, length, p, length);
  CHECK_FATAL(err == EOK, "memcpy_s failed");
}

void BasicIORead::ReadBufferInt8(int8 *dst, uint32 length, bool &success) {
  CHECK_NULL_FATAL(dst);
  const uint8 *p = GetBuffer(length);
  if (p == nullptr) {
    success = false;
    return;
  }
  pos += length;
  success = true;
  errno_t err = memcpy_s(dst, length, p, length);
  CHECK_FATAL(err == EOK, "memcpy_s failed");
}

void BasicIORead::ReadBufferChar(char *dst, uint32 length) {
  const uint8 *p = GetSafeBuffer(length);
  pos += length;
  errno_t err = memcpy_s(dst, length, p, length);
  CHECK_FATAL(err == EOK, "memcpy_s failed");
}

void BasicIORead::ReadBufferChar(char *dst, uint32 length, bool &success) {
  const uint8 *p = GetBuffer(length);
  if (p == nullptr) {
    success = false;
    return;
  }
  pos += length;
  success = true;
  errno_t err = memcpy_s(dst, length, p, length);
  CHECK_FATAL(err == EOK, "memcpy_s failed");
}

std::string BasicIORead::ReadString(uint32 length) {
  const void *p = GetSafeBuffer(length);
  const char *pchar = static_cast<const char*>(p);
  pos += length;
  return std::string(pchar, length);
}

std::string BasicIORead::ReadString(uint32 length, bool &success) {
  const void *p = GetBuffer(length);
  const char *pchar = static_cast<const char*>(p);
  if (p == nullptr) {
    success = false;
    return "";
  }
  pos += length;
  success = true;
  return std::string(pchar, length);
}
}  // namespace maple
