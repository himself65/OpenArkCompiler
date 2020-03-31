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
#ifndef MAPLEBE_INCLUDE_BE_AARCH64_AARCH64_RT_H
#define MAPLEBE_INCLUDE_BE_AARCH64_AARCH64_RT_H

#include <cstdint>
#include "rt.h"

namespace maplebe {
/*
 * This class contains constants that describes the object (memory) layout at
 * run time.
 *
 * WARNING: DO NOT USE `sizeof` OR `alignof`!  This class runs on the host, but
 * describes the target which is usually different, therefore `sizeof` and
 * `alignof` does not match the sizes and alignments on the target.  In the
 * MapleJava project, we run `mplcg` on x86_64, but run the application on
 * AArch64.
 */
class AArch64RTSupport {
 public:
  static const uint64_t kObjectAlignment = 8;    /* Word size. Suitable for all Java types. */
  static const uint64_t kObjectHeaderSize = 8;   /* java object header used by MM. */

#ifdef USE_32BIT_REF
  static const uint32_t kRefFieldSize = 4;       /* reference field in java object */
  static const uint32_t kRefFieldAlign = 4;
#else
  static const uint32_t kRefFieldSize = 8;       /* reference field in java object */
  static const uint32_t kRefFieldAlign = 8;
#endif /* USE_32BIT_REF */
  /* The array length offset is fixed since CONTENT_OFFSET is fixed to simplify code */
  static const int64_t kArrayLengthOffset = 12;  /* shadow + monitor + [padding] */
  /* The array content offset is aligned to 8B to alow hosting of size-8B elements */
  static const int64_t kArrayContentOffset = 16; /* fixed */
  static const int64_t kGcTibOffset = -8;
  static const int64_t kGcTibOffsetAbs = -kGcTibOffset;
};
} /* namespace maplebe */

#endif /* MAPLEBE_INCLUDE_BE_AARCH64_AARCH64_RT_H */