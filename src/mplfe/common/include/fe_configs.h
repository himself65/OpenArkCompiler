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
#ifndef MPLFE_INCLUDE_COMMON_FE_CONFIGS_H
#define MPLFE_INCLUDE_COMMON_FE_CONFIGS_H
#include "types_def.h"

#if ENABLE_COV_CHECK == 1
#define LLT_MOCK_TARGET virtual
#define LLT_MOCK_TARGET_VIRTUAL virtual
#define LLT_PUBLIC public
#define LLT_PROTECTED public
#define LLT_PRIVATE public
#else
#define LLT_MOCK_TARGET
#define LLT_MOCK_TARGET_VIRTUAL virtual
#define LLT_PUBLIC public
#define LLT_PROTECTED protected
#define LLT_PRIVATE private
#endif

namespace maple {
using TypeDim = uint8;

class FEConstants {
 public:
  const static uint8 kDimMax = UINT8_MAX;
}; // class FEContants
}  // namespace maple

#include "fe_config_parallel.h"
#endif  // MPLFE_INCLUDE_COMMON_FE_CONFIGS_H