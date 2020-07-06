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
#ifndef MEMPOOL_INCLUDE_MAPLE_STRING_H
#define MEMPOOL_INCLUDE_MAPLE_STRING_H
#include <cstring>
#include <climits>
#include "mempool.h"
#include "securec.h"
#include "mpl_logging.h"

namespace maple {
class MapleString {
 public:
  MapleString() = default;
  explicit MapleString(MemPool *currMp) : memPool(currMp) {}
  MapleString(const MapleString &str);
  MapleString(const char *str, MemPool *memPool);
  MapleString(const char *str, size_t size, MemPool *memPool);  // copyin
  MapleString(size_t size, MemPool *memPool);
  MapleString(const MapleString &str, MemPool *memPool);
  MapleString(const std::string &str, MemPool *memPool);
  ~MapleString() = default;

  size_t length() const {
    return dataLength;
  }

  char *c_str() {
    return data;
  }

  const char *c_str() const {
    if (dataLength <= 0) {
      return nullptr;
    }
    return data;
  }

  char &operator[](const int x) {
    return data[x];
  }

  const char &operator[](const int x) const {
    return data[x];
  }

  MapleString &operator=(const char c) {
    data = static_cast<char*>(memPool->Malloc(2 * sizeof(char)));
    CHECK_FATAL(data != nullptr, "nullptr check");
    data[0] = c;
    data[1] = '\0';
    dataLength = 1;
    return *this;
  }

  MapleString &operator=(const char *str) {
    if (str == nullptr) {
      return *this;
    }
    size_t size = strlen(str);
    CHECK_FATAL(size <= UINT_MAX - 1, "str too large");

    // if data is null, old_size = 0, else +1
    size_t oldSize = ((data == nullptr) ? 0 : (dataLength + 1));
    if (oldSize < (1 + size)) {
      data = static_cast<char*>(memPool->Realloc(data, oldSize * sizeof(char), (1 + size) * sizeof(char)));
    }
    CHECK_FATAL(data != nullptr, "null ptr check ");
    if (size == 0) {
      data[0] = '\0';
      return *this;
    }
    errno_t eNum = memcpy_s(data, size + 1, str, size);
    CHECK_FATAL(eNum == EOK, "memcpy_s failed");
    dataLength = size;
    CHECK_FATAL(data != nullptr, "null ptr check ");
    data[dataLength] = '\0';
    return *this;
  }

  MapleString &operator=(const std::string &str) {
    size_t size = str.length();
    CHECK_FATAL(size <= UINT_MAX - 1, "str too large");

    size_t oldSize = (data == nullptr) ? 0 : (dataLength + 1);
    if (oldSize < (1 + size)) {
      data = static_cast<char*>(memPool->Realloc(data, oldSize * sizeof(char), (1 + size) * sizeof(char)));
    }
    CHECK_FATAL(data != nullptr, "null ptr check ");
    if (size == 0) {
      data[0] = '\0';
      return *this;
    }
    errno_t eNum = memcpy_s(data, size, str.data(), size);
    CHECK_FATAL(eNum == EOK, "memcpy_s failed");
    dataLength = size;
    data[dataLength] = '\0';
    return *this;
  }

  MapleString &operator=(const MapleString &str) {
    if (&str == this) {
      return *this;
    }
    size_t size = str.dataLength;
    CHECK_FATAL(size <= UINT_MAX - 1, "str too large");

    size_t oldSize = (data == nullptr) ? 0 : (dataLength + 1);
    data = static_cast<char*>(memPool->Realloc(data, oldSize * sizeof(char), (1 + size) * sizeof(char)));
    CHECK_FATAL(data != nullptr, "null ptr check");
    if (size == 0) {
      data[0] = '\0';
      return *this;
    }
    errno_t eNum = memcpy_s(data, size, str.data, size);
    CHECK_FATAL(eNum == EOK, "memcpy_s failed");
    dataLength = size;
    data[dataLength] = '\0';
    return *this;
  }

  MapleString &operator+=(const char c) {
    size_t oldSize = ((data == nullptr) ? 0 : (dataLength + 1));
    CHECK_FATAL(oldSize <= UINT_MAX - 1, "str too large");

    data = static_cast<char*>(memPool->Realloc(data, oldSize * sizeof(char), (dataLength + 1 + 1) * sizeof(char)));
    ++dataLength;
    data[dataLength - 1] = c;
    data[dataLength] = '\0';
    return *this;
  }

  MapleString &operator+=(const char *str) {
    if (str == nullptr) {
      return *this;
    }
    size_t size = strlen(str);
    size_t oldSize = ((data == nullptr) ? 0 : (dataLength + 1));
    CHECK_FATAL(size <= UINT_MAX - oldSize, "str too large");

    data = static_cast<char*>(memPool->Realloc(data, oldSize * sizeof(char), (dataLength + size + 1) * sizeof(char)));
    CHECK_FATAL(data != nullptr, "null ptr check");
    errno_t eNum = memcpy_s(data + dataLength, size, str, size);
    CHECK_FATAL(eNum == EOK, "memcpy_s failed");
    dataLength += size;
    data[dataLength] = '\0';
    return *this;
  }

  MapleString &operator+=(const MapleString &str) {
    size_t oldSize = ((data == nullptr) ? 0 : (dataLength + 1));
    CHECK_FATAL(str.dataLength <= UINT_MAX - oldSize, "str too large");

    data = static_cast<char*>(
        memPool->Realloc(data, oldSize * sizeof(char), (dataLength + str.dataLength + 1) * sizeof(char)));
    errno_t eNum = memcpy_s(data + dataLength, str.dataLength, str.data, str.dataLength);
    CHECK_FATAL(eNum == EOK, "memcpy_s failed");
    dataLength += str.dataLength;
    data[dataLength] = '\0';
    return *this;
  }

  MapleString &operator+=(const std::string &str) {
    size_t size = str.length();
    size_t oldSize = ((data == nullptr) ? 0 : (dataLength + 1));
    CHECK_FATAL(size <= UINT_MAX - oldSize, "str too large");

    data = static_cast<char*>(memPool->Realloc(data, oldSize * sizeof(char), (dataLength + size + 1) * sizeof(char)));
    CHECK_FATAL(data != nullptr, " null ptr check");
    errno_t eNum = memcpy_s(data + dataLength, size, str.data(), size);
    CHECK_FATAL(eNum == EOK, "memcpy_s failed");
    dataLength += size;
    data[dataLength] = '\0';
    return *this;
  }

  void clear();
  bool empty() const {
    if (dataLength <= 0 || data == nullptr) {
      return true;
    } else {
      return false;
    }
  }

  MapleString &push_back(const char c);
  MapleString &append(const MapleString &str);
  MapleString &append(const MapleString &str, size_t subPos, size_t subLen);
  MapleString &append(const char *s);
  MapleString &append(const char *s, size_t n);
  MapleString &append(size_t n, char c);
  MapleString &append(const std::string &str);
  size_t find(const MapleString &str, size_t pos = 0) const;
  size_t find(const char *s, size_t pos = 0) const;
  size_t find(const char *s, size_t pos, size_t n) const;
  size_t find(char c, size_t pos = 0) const;
  size_t find_last_of(const char*, size_t pos = 0) const;
  MapleString substr(size_t pos, size_t len) const;
  MapleString &insert(size_t pos, const MapleString &str);
  MapleString &insert(size_t pos, const MapleString &str, size_t subPos, size_t subLen);
  MapleString &insert(size_t pos, const char *s);
  MapleString &insert(size_t pos, const char *s, size_t n);
  MapleString &insert(size_t pos, size_t n, char c);
  MapleString &assign(const MapleString &str);
  MapleString &assign(const MapleString &str, size_t subPos, size_t subLen);
  MapleString &assign(const char *s);
  MapleString &assign(const char *s, size_t n);
  MapleString &assign(size_t n, char c);

 private:
  inline static size_t StrLen(const char *s) {
    if (s == nullptr) {
      return 0;
    }
    return ::strlen(s);
  }

  inline static char *NewData(MemPool *memPool, const char *source, size_t len);

  friend bool operator==(const MapleString&, const MapleString&);
  friend bool operator==(const MapleString&, const char*);
  friend bool operator==(const char*, const MapleString&);
  friend bool operator<(const MapleString &str1, const MapleString &str2);

  char *data = nullptr;
  MemPool *memPool = nullptr;
  size_t dataLength = 0;
};

template <typename OS>
inline OS &operator<<(OS &os, const MapleString &data) {
  os << data.c_str();
  return os;
}

// global operators
bool operator==(const MapleString &str1, const MapleString &str2);
bool operator==(const MapleString &str1, const char *str2);
bool operator==(const char *str1, const MapleString &str2);
bool operator!=(const MapleString &str1, const MapleString &str2);
bool operator!=(const MapleString &str1, const char *str2);
bool operator!=(const char *str1, const MapleString &str2);
bool operator<(const MapleString &str1, const MapleString &str2);
}  // namespace maple
#endif  // MEMPOOL_INCLUDE_MAPLE_STRING_H
