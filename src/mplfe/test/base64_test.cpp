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
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "base64.h"

namespace maple {
TEST(Base64, Encode) {
  uint8 buf[] = { 'A', 'B', 'C', 'D' };
  std::string base64Str = Base64::Encode(buf, 4);
  EXPECT_EQ(base64Str, "QUJDRA==");
}

TEST(Base64, Decode) {
  std::string base64Str = "QUJDRA==";
  size_t length = 0;
  uint8 *buf = Base64::Decode(base64Str, length);
  ASSERT_EQ(length, 4);
  EXPECT_EQ(buf[0], 'A');
  EXPECT_EQ(buf[1], 'B');
  EXPECT_EQ(buf[2], 'C');
  EXPECT_EQ(buf[3], 'D');
}

TEST(Base64, Decode_Boundary1) {
  std::string base64Str = "";
  size_t length = 0;
  (void)Base64::Decode(base64Str, length);
  ASSERT_EQ(length, 0);
}

TEST(Base64, Decode_Boundary2) {
  std::string base64Str = "QUI=";
  size_t length = 0;
  uint8 *buf = Base64::Decode(base64Str, length);
  ASSERT_EQ(length, 2);
  EXPECT_EQ(buf[0], 'A');
  EXPECT_EQ(buf[1], 'B');
}
}  // namespace maple