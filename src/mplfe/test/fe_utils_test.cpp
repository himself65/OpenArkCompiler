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
#include "fe_utils.h"

namespace maple {
TEST(FEUtils, Split) {
  std::string str = "A,B,C";
  std::vector<std::string> results = FEUtils::Split(str, ',');
  ASSERT_EQ(results.size(), 3);
  EXPECT_EQ(results[0], "A");
  EXPECT_EQ(results[1], "B");
  EXPECT_EQ(results[2], "C");
}
}  // namespace maple