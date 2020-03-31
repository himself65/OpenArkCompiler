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
#ifndef MPLFE_INCLUDE_COMMON_FEIR_BB_H
#define MPLFE_INCLUDE_COMMON_FEIR_BB_H
#include <vector>
#include "types_def.h"
#include "mempool_allocator.h"
#include "fe_configs.h"
#include "fe_utils.h"
#include "feir_stmt.h"
#include "general_bb.h"

namespace maple {
class FEIRBB : public GeneralBB {
 public:
  FEIRBB(MapleAllocator &allocator, uint8 argKind);
  virtual ~FEIRBB() = default;
};  // class FEIRBB
}  // namespace maple
#endif  // MPLFE_INCLUDE_COMMON_FEIR_BB_H