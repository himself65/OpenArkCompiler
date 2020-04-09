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
#include "aarch64_immediate.h"
#include "common_utils.h"
#include "mpl_logging.h"

#if DEBUG
#include <stdio.h>
#include <set>
#include <iomanip>
#endif

namespace maplebe {

namespace {
constexpr uint32 kMaxBitTableSize = 5;
#if DEBUG
constexpr uint32 kN16ChunksCheck = 2;
#endif
constexpr std::array<uint64, kMaxBitTableSize> bitmaskImmMultTable = {
    0x0000000100000001UL, 0x0001000100010001UL, 0x0101010101010101UL, 0x1111111111111111UL, 0x5555555555555555UL,
};
};

bool IsBitmaskImmediate(uint64 val, uint32 bitLen) {
  ASSERT(val != 0, "IsBitmaskImmediate() don's accept 0 or -1");
  ASSERT(static_cast<int64>(val) != -1, "IsBitmaskImmediate() don's accept 0 or -1");
  if ((val & 0x1) != 0) {
    /*
     * we want to work with
     * 0000000000000000000000000000000000000000000001100000000000000000
     * instead of
     * 1111111111111111111111111111111111111111111110011111111111111111
     */
    val = ~val;
  }

  if (bitLen == k32BitSize) {
    val = (val << k32BitSize) | (val & ((1UL << k32BitSize) - 1));
  }

  /* get the least significant bit set and add it to 'val' */
  uint64 tmpVal = val + (val & static_cast<uint64>(-val));

  /* now check if tmp is a power of 2 or tmpVal==0. */
  tmpVal = tmpVal & (tmpVal - 1);
  if (tmpVal == 0) {
    /* power of two or zero ; return true */
    return true;
  }

  int32 p0 = __builtin_ctzll(val);
  int32 p1 = __builtin_ctzll(tmpVal);
  int64 diff = p1 - p0;

  /* check if diff is a power of two; return false if not. */
  if ((static_cast<uint64>(diff) & (static_cast<uint64>(diff) - 1)) != 0) {
    return false;
  }

  int32 logDiff = __builtin_ctzll(diff);
  int64 pattern = val & ((1UL << static_cast<uint64>(diff)) - 1);
  return val == pattern * bitmaskImmMultTable[kMaxBitTableSize - logDiff];
}

bool IsMoveWidableImmediate(uint64 val, uint32 bitLen) {
  if (bitLen == k64BitSize) {
    /* 0xHHHH000000000000 or 0x0000HHHH00000000, return true */
    if (((val & ((static_cast<uint64>(0xffff)) << k48BitSize)) == val) ||
        ((val & ((static_cast<uint64>(0xffff)) << k32BitSize)) == val)) {
      return true;
    }
  } else {
    /* get lower 32 bits */
    val &= static_cast<uint64>(0xffffffff);
  }
  /* 0x00000000HHHH0000 or 0x000000000000HHHH, return true */
  return ((val & ((static_cast<uint64>(0xffff)) << k16BitSize)) == val ||
          (val & ((static_cast<uint64>(0xffff)) << 0)) == val);
}

bool BetterUseMOVZ(uint64 val) {
  int32 n16zerosChunks = 0;
  int32 n16onesChunks = 0;
  uint64 sa = 0;
  /* a 64 bits number is split 4 chunks, each chunk has 16 bits. check each chunk whether is all 1 or is all 0 */
  for (uint64 i = 0; i < k4BitSize; ++i, sa += k16BitSize) {
    uint64 chunkVal = (val >> (static_cast<uint64>(sa))) & 0x0000FFFFUL;
    if (chunkVal == 0) {
      ++n16zerosChunks;
    } else if (chunkVal == 0xFFFFUL) {
      ++n16onesChunks;
    }
  }
  /*
   * note that since we already check if the value
   * can be movable with as a single mov instruction,
   * we should not exepct either n_16zeros_chunks>=3 or n_16ones_chunks>=3
   */
  ASSERT(n16zerosChunks <= kN16ChunksCheck, "n16zerosChunks ERR");
  ASSERT(n16onesChunks <= kN16ChunksCheck, "n16onesChunks ERR");
  return (n16zerosChunks >= n16onesChunks);
}
}  /* namespace maplebe */
