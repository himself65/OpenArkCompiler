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
#include "base64.h"
#include <malloc.h>
#include <map>
#include "types_def.h"
#include "mpl_logging.h"

namespace maple {
namespace {
const std::string kBase64IdxStr = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const uint32 kBase64IdxStrLength = 64;
const uint8 kBase64MaskEncode = 0x3F;
const uint8 kBase64MaskDecode = 0xFF;
const uint32 kBase64Shift0 = 0;
const uint32 kBase64Shift8 = 8;
const uint32 kBase64Shift16 = 16;
const uint32 kBase64Shift6 = 6;
const uint32 kBase64Shift12 = 12;
const uint32 kBase64Shift18 = 18;
const size_t kBase64EncodeBaseLen = 3;
const size_t kBase64DecodeBaseLen = 4;
}

std::map<char, char> Base64::encodeMap = Base64::InitEncodeMap();
std::map<char, uint8> Base64::decodeMap = Base64::InitDecodeMap();

std::map<char, char> Base64::InitEncodeMap() {
  // init encode map
  std::map<char, char> ans;
  CHECK_FATAL(kBase64IdxStr.length() == kBase64IdxStrLength, "length of base64_idx_str_ must be 64");
  for (size_t i = 0; i < kBase64IdxStr.length(); i++) {
    CHECK_FATAL(ans.insert(std::make_pair(i, kBase64IdxStr[i])).second, "encodeMap insert failed");
  }
  return ans;
}

std::map<char, uint8> Base64::InitDecodeMap() {
  // init decode map
  std::map<char, uint8> ans;
  CHECK_FATAL(kBase64IdxStr.length() == kBase64IdxStrLength, "length of base64_idx_str_ must be 64");
  for (uint32 i = 0; i < kBase64IdxStr.length(); i++) {
    CHECK_FATAL(ans.insert(std::make_pair(kBase64IdxStr[i], i)).second, "decodeMap insert failed");
  }
  CHECK_FATAL(ans.insert(std::make_pair('=', 0)).second, "decodeMap insert failed");
  return ans;
}

std::string Base64::Encode(const uint8 *input, size_t length) {
  // process input
  std::string strEncoded;
  uint32 temp;
  size_t offset;
  while (length > 0) {
    temp = 0;
    if (length >= 1) {
      temp |= (input[0] << kBase64Shift16);
    }
    if (length >= 2) {
      temp |= (input[1] << kBase64Shift8);
    }
    if (length >= 3) {
      temp |= (input[2] << kBase64Shift0);
    }

    // encoded code 0
    strEncoded.push_back(encodeMap[static_cast<char>((temp >> kBase64Shift18) & kBase64MaskEncode)]);
    // encoded code 1
    strEncoded.push_back(encodeMap[static_cast<char>((temp >> kBase64Shift12) & kBase64MaskEncode)]);

    if (length < 2) {
      strEncoded.push_back('=');
    } else {
      // encoded code 2
      strEncoded.push_back(encodeMap[static_cast<char>((temp >> kBase64Shift6) & kBase64MaskEncode)]);
    }

    if (length < kBase64EncodeBaseLen) {
      strEncoded.push_back('=');
    } else {
      // encoded code 3
      strEncoded.push_back(encodeMap[static_cast<char>((temp >> kBase64Shift0) & kBase64MaskEncode)]);
    }

    offset = (length >= kBase64EncodeBaseLen) ? kBase64EncodeBaseLen : length;
    length -= offset;
    input += offset;
  }
  return strEncoded;
}

size_t Base64::DecodeLength(const std::string &input) {
  // length calculation
  size_t length;
  const char *inputBuf = input.c_str();
  size_t inputLength = input.length();
  if (inputLength == 0) {
    return 0;
  }
  CHECK_FATAL(inputLength % kBase64DecodeBaseLen == 0, "input.length must be factor of 4");
  length = inputLength * kBase64EncodeBaseLen / kBase64DecodeBaseLen;
  if (inputBuf[inputLength - 1] == '=') {
    length--;
  }
  if (inputBuf[inputLength - 2] == '=') {
    length--;
  }
  return length;
}

uint8 *Base64::Decode(const std::string &input, size_t &lengthRet) {
  size_t inputLength = input.length();
  lengthRet = DecodeLength(input);
  if (lengthRet == 0) {
    return nullptr;
  }
  size_t idx = 0;
  const char *inputBuf = input.c_str();
  uint8 *buf = static_cast<uint8*>(malloc(sizeof(uint8) * lengthRet));
  CHECK_FATAL(buf != nullptr, "malloc failed");
  uint32 temp;
  while (inputLength > 0) {
    auto it0 = decodeMap.find(inputBuf[0]);
    auto it1 = decodeMap.find(inputBuf[1]);
    auto it2 = decodeMap.find(inputBuf[2]);
    auto it3 = decodeMap.find(inputBuf[3]);
    if (it0 == decodeMap.end() || it1 == decodeMap.end() || it2 == decodeMap.end() || it3 == decodeMap.end()) {
      CHECK_FATAL(false, "invalid input");
    }
    temp = (it0->second << kBase64Shift18) | (it1->second << kBase64Shift12) | (it2->second << kBase64Shift6) |
           (it3->second << kBase64Shift0);
    uint8 c0 = (temp >> kBase64Shift16) & kBase64MaskDecode;
    uint8 c1 = (temp >> kBase64Shift8) & kBase64MaskDecode;
    uint8 c2 = (temp >> kBase64Shift0) & kBase64MaskDecode;
    if (inputBuf[2] == '=') {
      CHECK_FATAL(inputLength == kBase64DecodeBaseLen, "'=' must be in last package");
      CHECK_FATAL(inputBuf[3] == '=', "'=' must be in last package");
      buf[idx++] = c0;
    } else if (inputBuf[3] == '=') {
      CHECK_FATAL(inputLength == kBase64DecodeBaseLen, "'=' must be in last package");
      buf[idx++] = c0;
      buf[idx++] = c1;
    } else {
      buf[idx++] = c0;
      buf[idx++] = c1;
      buf[idx++] = c2;
    }
    inputLength -= kBase64DecodeBaseLen;
    inputBuf += kBase64DecodeBaseLen;
  }
  return buf;
}
}  // namespace maple
