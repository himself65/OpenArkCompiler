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
#ifndef MRT_INCLUDE_LITERALSTRNAME_H
#define MRT_INCLUDE_LITERALSTRNAME_H
#include <map>
#include <cstring>
#include "muid.h"

// literal string naming is shared between maple compiler and runtime, thus not in namespace maplert
const std::string kConstString = "_C_STR_";
const std::string kConstStringPtr = "_PTR_C_STR_";
const std::string kLocalStringPrefix = "L_STR_";
constexpr int kConstStringLen = 7;
constexpr unsigned int kDigestHashLength = 16;

union DigestHash {
  uint8_t bytes[kDigestHashLength];
  struct {
    uint64_t first;
    uint64_t second;
  } d;
};

class LiteralStrName {
 public:
  static int32_t CalculateHashSwapByte(const char16_t *data, uint32_t len);
  static uint32_t CalculateHash(const char16_t *data, uint32_t len, bool dataIsCompress) {
    uint32_t hash = 0;
    if (dataIsCompress) {
      const char *dataStart = reinterpret_cast<const char*>(data);
      const char *end = dataStart + len;
      while (dataStart < end) {
        hash = (hash << 5) - hash + *dataStart++; // calculate the hash code of data
      }
    } else {
      const char16_t *end = data + len;
      while (data < end) {
        hash = (static_cast<unsigned int>(hash) << 5) - hash + *data++; // calculate the hash code of data
      }
    }
    return hash;
  }

  static std::string GetHexStr(const uint8_t *bytes, uint32_t len);
  static std::string GetLiteralStrName(const uint8_t *bytes, uint32_t len);
  static std::string ComputeMuid(const uint8_t *bytes, uint32_t len);
};

#endif
