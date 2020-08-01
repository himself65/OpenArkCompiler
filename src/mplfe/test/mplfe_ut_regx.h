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
#ifndef MPLFE_UT_INCLUDE_MPLFE_UT_REGX_H
#define MPLFE_UT_INCLUDE_MPLFE_UT_REGX_H
#include <string>
#include <regex>
#include "types_def.h"

namespace maple {
class MPLFEUTRegx {
 public:
  static const uint32 kAnyNumber = 0xFFFFFFFF;
  MPLFEUTRegx() = default;
  ~MPLFEUTRegx() = default;
  static bool Match(const std::string &str, const std::string &pattern);
  static std::string RegName(uint32 regNum);
  static std::string RefIndex(uint32 typeIdx);
  static std::string Any() {
    return "(.|\r|\n)*";
  }
};
}  // namespace maple
#endif  // MPLFE_UT_INCLUDE_MPLFE_UT_REGX_H