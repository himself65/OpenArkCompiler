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
#ifndef MPLFE_INCLUDE_COMMON_BASE64_H
#define MPLFE_INCLUDE_COMMON_BASE64_H
#include <map>
#include <string>
#include "types_def.h"

namespace maple {
class Base64 {
 public:
  static std::string Encode(const uint8 *input, size_t length);
  static uint8 *Decode(const std::string &input, size_t &lengthRet);
  static std::map<char, char> InitEncodeMap();
  static std::map<char, uint8> InitDecodeMap();

 private:
  Base64() = default;
  ~Base64() = default;

  static std::map<char, char> encodeMap;
  static std::map<char, uint8> decodeMap;
  static size_t DecodeLength(const std::string &input);
};
}  // namespace maple
#endif