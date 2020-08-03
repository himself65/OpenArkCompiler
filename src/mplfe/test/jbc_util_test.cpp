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
#include "jbc_util.h"

namespace maple {
namespace jbc {
TEST(JBCUtil, ClassInternalNameToFullName) {
  EXPECT_EQ(JBCUtil::ClassInternalNameToFullName("java/lang/Object"), "Ljava/lang/Object;");
  EXPECT_EQ(JBCUtil::ClassInternalNameToFullName("[Ljava/lang/Object;"), "[Ljava/lang/Object;");
}

TEST(JBCUtil, SolveMethodSignature) {
  std::vector<std::string> ans;
  ans = JBCUtil::SolveMethodSignature("func(Ljava/lang/Object;[II)J");
  ASSERT_EQ(ans.size(), 4);
  EXPECT_EQ(ans[0], "J");
  EXPECT_EQ(ans[1], "Ljava/lang/Object;");
  EXPECT_EQ(ans[2], "[I");
  EXPECT_EQ(ans[3], "I");
}
}  // namespace jbc
}  // namespace maple