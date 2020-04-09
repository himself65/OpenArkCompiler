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
#ifndef MPLFE_INCLUDE_JBC_UTIL_H
#define MPLFE_INCLUDE_JBC_UTIL_H
#include <string>
#include <vector>
#include "jbc_opcode.h"

namespace maple {
namespace jbc {
class JBCUtil {
 public:
  static std::string ClassInternalNameToFullName(const std::string &name);
  static std::vector<std::string> SolveMethodSignature(const std::string &signature);
  static std::string SolveParamName(const std::string &signature);
  static JBCPrimType GetPrimTypeForName(const std::string &name);

 private:
  JBCUtil() = default;
  ~JBCUtil() = default;
};
}  // namespace jbc
}  // namespace maple
#endif  // MPLFE_INCLUDE_JBC_UTIL_H