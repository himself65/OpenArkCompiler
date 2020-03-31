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
#ifndef MAPLEBE_INCLUDE_BE_COMMON_UTILS_H
#define MAPLEBE_INCLUDE_BE_COMMON_UTILS_H
#include <cstdint>
#include "types_def.h"
#include "mpl_logging.h"

namespace maplebe {
using namespace maple;
constexpr uint32 kOffsetAlignmentOf8Bit = 0;
constexpr uint32 kOffsetAlignmentOf16Bit = 1;
constexpr uint32 kOffsetAlignmentOf32Bit = 2;
constexpr uint32 kOffsetAlignmentOf64Bit = 3;
constexpr uint32 kBaseOffsetAlignment = 3;
/*
 * The constexpr implementations, without assertions.  Suitable for using in
 * constants.
 */
constexpr uint32 k1FConst = 31;
constexpr uint32 k4BitSize = 4;
constexpr uint32 k6BitSize = 6;
constexpr uint32 k8BitSize = 8;
constexpr uint32 k16BitSize = 16;
constexpr uint32 k24BitSize = 24;
constexpr uint32 k32BitSize = 32;
constexpr uint32 k48BitSize = 48;
constexpr uint32 k56BitSize = 56;
constexpr uint32 k64BitSize = 64;
constexpr uint32 k128BitSize = 128;

constexpr uint32 k1ByteSize = 1;
constexpr uint32 k2ByteSize = 2;
constexpr uint32 k4ByteSize = 4;
constexpr uint32 k8ByteSize = 8;
constexpr uint32 k14ByteSize = 14;
constexpr uint32 k15ByteSize = 15;
constexpr uint32 k16ByteSize = 16;

constexpr uint32 k8BitShift = 3;  /* 8 is 1 << 3; */
constexpr uint32 k16BitShift = 4; /* 16 is 1 << 4 */

/* Storage location of operands in one insn */
constexpr int32 kInsnFirstOpnd = 0;
constexpr int32 kInsnSecondOpnd = 1;
constexpr int32 kInsnThirdOpnd = 2;
constexpr int32 kInsnFourthOpnd = 3;
constexpr int32 kInsnFifthOpnd = 4;
constexpr int32 kInsnSixthOpnd = 5;
constexpr int32 kInsnSeventhOpnd = 6;
constexpr int32 kInsnEighthOpnd = 7;


/* Check whether the value is an even number. */
constexpr int32 kDivide2 = 2;
constexpr int32 kRegNum2 = 2;
constexpr int32 kModNum2 = 2;
constexpr int32 kStepNum2 = 2;

/*
 * if the number of local refvar is less than 12, use stp or str to init local refvar
 * else call function MCC_InitializeLocalStackRef to init.
 */
constexpr int32 kRefNum12 = 12;

/* mod function max argument size */
constexpr uint32  kMaxModFuncArgSize = 8;

/* string length of spacial name "__EARetTemp__" */
constexpr int32 kEARetTempNameSize = 10;

/*
 * Aarch64 data processing instructions have 12 bits of space for values in their instuction word
 * This is arranged as a four-bit rotate value and an eight-bit immediate value:
 */
constexpr uint32 kMaxAarch64ImmVal12Bits = 12;

constexpr uint32 kMaxAarch64ImmVal13Bits = 13;

/* aarch64 assembly takes up to 24-bits */
constexpr uint32 kMaxAarch64ImmVal24Bits = 24;

constexpr double kMicroSecPerMilliSec = 1000.0;

constexpr double kPercent = 100.0;

inline bool IsPowerOf2Const(uint64 i) {
  return (i & (i - 1)) == 0;
}

inline uint64 RoundUpConst(uint64 offset, uint64 align) {
  return (-align) & (offset + align - 1);
}

inline bool IsPowerOf2(uint64 i) {
  return IsPowerOf2Const(i);
}

/* align must be a power of 2 */
inline uint64 RoundUp(uint64 offset, uint64 align) {
  if (align == 0) {
    return offset;
  }
  ASSERT(IsPowerOf2(align), "align must be power of 2!");
  return RoundUpConst(offset, align);
}

inline bool IsAlignedTo(uint64 offset, uint64 align) {
  ASSERT(IsPowerOf2(align), "align must be power of 2!");
  return (offset & (align - 1)) == 0;
}
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_BE_COMMON_UTILS_H */
