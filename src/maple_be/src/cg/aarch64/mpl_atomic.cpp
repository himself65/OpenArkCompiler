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
#include "mpl_atomic.h"
#include <cstdint>
#include "mpl_logging.h"

namespace maple {
namespace {
constexpr int32 kMaxSizeOfTab = 6;
};
MemOrd MemOrdFromU32(uint32 val) {
  /* 6 is the size of tab below. 2 is memory_order_consume, it is Disabled. */
  CHECK_FATAL(val <= kMaxSizeOfTab, "Illegal number for MemOrd: %u", val);
  CHECK_FATAL(val != 2, "Illegal number for MemOrd: %u", val);
  static std::array<MemOrd, kMaxSizeOfTab + 1> tab = {
    MemOrd::kNotAtomic,
    MemOrd::memory_order_relaxed,
    /*
     * memory_order_consume Disabled. Its semantics is debatable.
     * We don't support it now, but reserve the number. Use memory_order_acquire instead.
     */
    MemOrd::memory_order_acquire,  /* padding entry */
    MemOrd::memory_order_acquire,
    MemOrd::memory_order_release,
    MemOrd::memory_order_acq_rel,
    MemOrd::memory_order_seq_cst,
  };
  return tab[val];
}

bool MemOrdIsAcquire(MemOrd ord) {
  static std::array<bool, kMaxSizeOfTab + 1> tab = {
    false,  /* kNotAtomic */
    false,  /* memory_order_relaxed */
    true,   /* memory_order_consume */
    true,   /* memory_order_acquire */
    false,  /* memory_order_release */
    true,   /* memory_order_acq_rel */
    true,   /* memory_order_seq_cst */
  };
  uint32 tabIndex = static_cast<uint32>(ord);
  CHECK_FATAL(tabIndex <= kMaxSizeOfTab, "Illegal number for MemOrd: %u", tabIndex);
  return tab[tabIndex];
}

bool MemOrdIsRelease(MemOrd ord) {
  static std::array<bool, kMaxSizeOfTab + 1> tab = {
    false,  /* kNotAtomic */
    false,  /* memory_order_relaxed */
    false,  /* memory_order_consume */
    false,  /* memory_order_acquire */
    true,   /* memory_order_release */
    true,   /* memory_order_acq_rel */
    true,   /* memory_order_seq_cst */
  };
  uint32 tabIndex = static_cast<uint32>(ord);
  CHECK_FATAL(tabIndex <= kMaxSizeOfTab, "Illegal number for MemOrd: %u", tabIndex);
  return tab[tabIndex];
}
}  /* namespace maple */
