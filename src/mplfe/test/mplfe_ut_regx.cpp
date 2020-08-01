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
#include "mplfe_ut_regx.h"
#include <sstream>
#include <iostream>

namespace maple {
bool MPLFEUTRegx::Match(const std::string &str, const std::string &pattern) {
  if (std::regex_match(str, std::regex(pattern))) {
    return true;
  } else {
    std::cerr << "Pattern: " << pattern << std::endl;
    std::cerr << "String:  " << str << std::endl;
    return false;
  }
}

std::string MPLFEUTRegx::RegName(uint32 regNum) {
  std::stringstream ss;
  if (regNum == kAnyNumber) {
    ss << "Reg[0-9]+";
  } else {
    ss << "Reg" << regNum;
  }
  return ss.str();
}

std::string MPLFEUTRegx::RefIndex(uint32 typeIdx) {
  std::stringstream ss;
  if (typeIdx == kAnyNumber) {
    ss << "R[0-9]+";
  } else {
    ss << "R" << typeIdx;
  }
  return ss.str();
}
}  // namespace maple