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
#ifndef MPLFE_INCLUDE_FE_UTILS_JAVA_H
#define MPLFE_INCLUDE_FE_UTILS_JAVA_H
#include <string>
#include <vector>
#include "feir_type.h"

namespace maple {
class FEUtilJava {
 public:
  static std::vector<std::string> SolveMethodSignature(std::string signature, bool inMpl = true);
  static std::string SolveParamNameInJavaFormat(const std::string &signature);

 private:
  FEUtilJava() = default;
  ~FEUtilJava() = default;
};
}  // namespace maple
#endif  // MPLFE_INCLUDE_FE_UTILS_JAVA_H