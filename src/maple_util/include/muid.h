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
#ifndef MAPLE_UTIL_INCLUDE_MUID_H
#define MAPLE_UTIL_INCLUDE_MUID_H

// This is shared between maple compiler and runtime.
#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>

#ifdef USE_32BIT_REF
#define USE_64BIT_MUID
#endif // USE_32BIT_REF

constexpr unsigned int kSystemNamespace = 0xc0;
constexpr unsigned int kApkNamespace = 0x80;
constexpr unsigned int kBitMask = 0x3f;
constexpr unsigned int kGroupSize = 64;
constexpr unsigned int kShiftAmount = 32;
constexpr unsigned int kBlockLength = 16;
constexpr unsigned int kByteLength = 8;
constexpr unsigned int kNumLowAndHigh = 2;

#ifdef USE_64BIT_MUID
constexpr unsigned int kMuidLength = 8;
#else
constexpr unsigned int kMuidLength = 16;
#endif // USE_64BIT_MUID

constexpr unsigned int kDigestShortHashLength = 8;
constexpr unsigned int kDigestHashLength = 16;
union DigestHash {
  uint8_t bytes[kDigestHashLength];
  struct {
    uint64_t first;
    uint64_t second;
  } d;
};

// muid-related files are shared between maple compiler and runtime, thus not in
// namespace maplert
struct MuidContext {
  unsigned int a, b, c, d;
  unsigned int count[kNumLowAndHigh];
  unsigned int block[kBlockLength];
  unsigned char buffer[kGroupSize];
};

class MUID {
 public:
  union {
#ifdef USE_64BIT_MUID
    uint32_t words[kNumLowAndHigh];
    uint8_t bytes[kMuidLength];
    uint64_t raw;
#else
    uint64_t words[kNumLowAndHigh];
    uint8_t bytes[kMuidLength];
#endif // USE_64BIT_MUID
  } data;

  inline bool IsSystemNameSpace() const {
    return (data.bytes[kMuidLength - 1] & ~kBitMask) == kSystemNamespace;
  }
  inline bool IsApkNameSpace() const {
    return (data.bytes[kMuidLength - 1] & ~kBitMask) == kApkNamespace;
  }
  inline void SetSystemNameSpace() {
    data.bytes[kMuidLength - 1] &= kBitMask;
    data.bytes[kMuidLength - 1] |= kSystemNamespace;
  }
  inline void SetApkNameSpace() {
    data.bytes[kMuidLength - 1] &= kBitMask;
    data.bytes[kMuidLength - 1] |= kApkNamespace;
  }
  bool operator<(const MUID &muid) const {
    return (data.words[1] < muid.data.words[1] ||
            (data.words[1] == muid.data.words[1] && data.words[0] < muid.data.words[0]));
  }
  bool operator>(const MUID &muid) const {
    return (data.words[1] > muid.data.words[1] ||
            (data.words[1] == muid.data.words[1] && data.words[0] > muid.data.words[0]));
  }
  bool operator==(const MUID &muid) const {
    return data.words[1] == muid.data.words[1] && data.words[0] == muid.data.words[0];
  }
  bool operator!=(const MUID &muid) const {
    return data.words[1] != muid.data.words[1] || data.words[0] != muid.data.words[0];
  }
  std::string ToStr() const {
    std::stringstream sbuf;
#ifdef USE_64BIT_MUID
    // 8 spaces to 64 bit
    sbuf << std::setfill('0') << std::setw(8) << std::hex << data.words[1] << std::setfill('0') << std::setw(8) <<
        std::hex << data.words[0];
#else
    // 16 spaces to 32 bit
    sbuf << std::setfill('0') << std::setw(16) << std::hex << data.words[1] << std::setfill('0') << std::setw(16) <<
        std::hex << data.words[0];
#endif // USE_64BIT_MUID
    return sbuf.str();
  }

  // Return 64-bit size hash for AArch64.
  uint64_t hash() const {
#ifdef USE_64BIT_MUID
    return data.raw;
#else
    return data.words[0]; // Just return one word.
#endif // USE_64BIT_MUID
  }
};

void MuidInit(MuidContext &status);
void MuidDecode(MuidContext &status, const unsigned char &data, uint64_t size);

template <typename T>
void FullEncode(T &result, MuidContext &status);
void MuidEncode(unsigned char (&result)[kDigestShortHashLength], MuidContext &status);
void MuidEncode(unsigned char (&result)[kDigestHashLength], MuidContext &status, bool use64Bit = false);

void GetMUIDHash(const unsigned char &data, size_t size, MUID &muid);
DigestHash GetDigestHash(const unsigned char &bytes, uint32_t len);
MUID GetMUID(const std::string &symbolName, bool forSystem = true);
#endif
