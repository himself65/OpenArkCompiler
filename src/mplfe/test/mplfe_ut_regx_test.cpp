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
#include "mplfe_ut_regx.h"

namespace maple {
TEST(MPLFEUTRegx, Any) {
  std::string pattern = MPLFEUTRegx::Any();
  std::string str = "\n";
  EXPECT_EQ(MPLFEUTRegx::Match(str, pattern), true);
}

TEST(MPLFEUTRegx, RegName) {
  std::string patternAny = MPLFEUTRegx::RegName(MPLFEUTRegx::kAnyNumber);
  std::string pattern100 = MPLFEUTRegx::RegName(100);
  EXPECT_EQ(MPLFEUTRegx::Match("Reg100", patternAny), true);
  EXPECT_EQ(MPLFEUTRegx::Match("Reg100", pattern100), true);
  EXPECT_EQ(MPLFEUTRegx::Match("Reg1000", patternAny), true);
  EXPECT_EQ(MPLFEUTRegx::Match("Reg1000", pattern100), false);
}

TEST(MPLFEUTRegx, RefIndex) {
  std::string patternAny = MPLFEUTRegx::RefIndex(MPLFEUTRegx::kAnyNumber);
  std::string pattern100 = MPLFEUTRegx::RefIndex(100);
  EXPECT_EQ(MPLFEUTRegx::Match("R100", patternAny), true);
  EXPECT_EQ(MPLFEUTRegx::Match("R100", pattern100), true);
  EXPECT_EQ(MPLFEUTRegx::Match("R1000", patternAny), true);
  EXPECT_EQ(MPLFEUTRegx::Match("R1000", pattern100), false);
}
}  // namespace maple