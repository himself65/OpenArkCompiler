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
#include "jbc_stack_helper.h"

namespace maple {
TEST(JBCStackHelper, PushItem_PopItem) {
  JBCStackHelper stackHelper;
  stackHelper.PushItem(jbc::JBCPrimType::kTypeInt);
  EXPECT_EQ(stackHelper.PopItem(jbc::JBCPrimType::kTypeInt), true);
  stackHelper.Reset();
  stackHelper.PushItem(jbc::JBCPrimType::kTypeInt);
  EXPECT_EQ(stackHelper.PopItem(jbc::JBCPrimType::kTypeFloat), false);
  stackHelper.Reset();
  stackHelper.PushItem(jbc::JBCPrimType::kTypeLong);
  EXPECT_EQ(stackHelper.PopItem(jbc::JBCPrimType::kTypeInt), false);
  stackHelper.Reset();
  stackHelper.PushItem(jbc::JBCPrimType::kTypeLong);
  EXPECT_EQ(stackHelper.PopItem(jbc::JBCPrimType::kTypeLong), true);
  stackHelper.Reset();
  stackHelper.PushItem(jbc::JBCPrimType::kTypeDouble);
  EXPECT_EQ(stackHelper.PopItem(jbc::JBCPrimType::kTypeDouble), true);
  stackHelper.Reset();
  EXPECT_EQ(stackHelper.PopItem(jbc::JBCPrimType::kTypeDouble), false);
}

TEST(JBCStackHelper, Dup) {
  JBCStackHelper stackHelper;
  stackHelper.PushItem(jbc::JBCPrimType::kTypeInt);
  EXPECT_EQ(stackHelper.Dup(jbc::kOpDup), true);
  std::vector<jbc::JBCPrimType> types({ jbc::JBCPrimType::kTypeInt, jbc::JBCPrimType::kTypeInt });
  EXPECT_EQ(stackHelper.PopItems(types), true);

  stackHelper.Reset();
  EXPECT_EQ(stackHelper.Dup(jbc::kOpDup), false);

  stackHelper.Reset();
  stackHelper.PushItem(jbc::JBCPrimType::kTypeLong);
  EXPECT_EQ(stackHelper.Dup(jbc::kOpDup), false);
}

TEST(JBCStackHelper, DupX1) {
  JBCStackHelper stackHelper;
  stackHelper.PushItem(jbc::JBCPrimType::kTypeInt);
  stackHelper.PushItem(jbc::JBCPrimType::kTypeFloat);
  EXPECT_EQ(stackHelper.Dup(jbc::kOpDupX1), true);
  std::vector<jbc::JBCPrimType> types({ jbc::JBCPrimType::kTypeFloat, jbc::JBCPrimType::kTypeInt,
                                        jbc::JBCPrimType::kTypeFloat });
  EXPECT_EQ(stackHelper.PopItems(types), true);

  stackHelper.Reset();
  EXPECT_EQ(stackHelper.Dup(jbc::kOpDupX1), false);

  stackHelper.Reset();
  stackHelper.PushItem(jbc::JBCPrimType::kTypeInt);
  EXPECT_EQ(stackHelper.Dup(jbc::kOpDupX1), false);

  stackHelper.Reset();
  stackHelper.PushItem(jbc::JBCPrimType::kTypeLong);
  EXPECT_EQ(stackHelper.Dup(jbc::kOpDupX1), false);

  stackHelper.Reset();
  stackHelper.PushItem(jbc::JBCPrimType::kTypeInt);
  stackHelper.PushItem(jbc::JBCPrimType::kTypeLong);
  EXPECT_EQ(stackHelper.Dup(jbc::kOpDupX1), false);

  stackHelper.Reset();
  stackHelper.PushItem(jbc::JBCPrimType::kTypeLong);
  stackHelper.PushItem(jbc::JBCPrimType::kTypeInt);
  EXPECT_EQ(stackHelper.Dup(jbc::kOpDupX1), false);

  stackHelper.Reset();
  stackHelper.PushItem(jbc::JBCPrimType::kTypeLong);
  stackHelper.PushItem(jbc::JBCPrimType::kTypeLong);
  EXPECT_EQ(stackHelper.Dup(jbc::kOpDupX1), false);
}

TEST(JBCStackHelper, DupX2) {
  JBCStackHelper stackHelper;
  // mode 1: ..., typeI, typeI, typeI ->
  // input: ..., I, S, B ->
  // output: ..., B, I, S, B ->
  stackHelper.PushItem(jbc::JBCPrimType::kTypeInt);
  stackHelper.PushItem(jbc::JBCPrimType::kTypeShort);
  stackHelper.PushItem(jbc::JBCPrimType::kTypeByteOrBoolean);
  EXPECT_EQ(stackHelper.Dup(jbc::kOpDupX2), true);
  std::vector<jbc::JBCPrimType> types1({ jbc::JBCPrimType::kTypeByteOrBoolean, jbc::JBCPrimType::kTypeInt,
                                         jbc::JBCPrimType::kTypeShort, jbc::JBCPrimType::kTypeByteOrBoolean });
  EXPECT_EQ(stackHelper.PopItems(types1), true);

  // mode2: ..., typeII, typeI ->
  // input: ..., J, I ->
  // output: ..., I, J, I ->
  stackHelper.Reset();
  stackHelper.PushItem(jbc::JBCPrimType::kTypeLong);
  stackHelper.PushItem(jbc::JBCPrimType::kTypeInt);
  EXPECT_EQ(stackHelper.Dup(jbc::kOpDupX2), true);
  std::vector<jbc::JBCPrimType> types2({ jbc::JBCPrimType::kTypeInt, jbc::JBCPrimType::kTypeLong,
                                         jbc::JBCPrimType::kTypeInt });
  EXPECT_EQ(stackHelper.PopItems(types2), true);

  // TODO(UT): add false cases
}

TEST(JBCStackHelper, Dup2) {
  JBCStackHelper stackHelper;
  // mode 1: ..., typeI, typeI ->
  // input: ..., I, S ->
  // output: ..., I, S, I, S ->
  stackHelper.PushItem(jbc::JBCPrimType::kTypeInt);
  stackHelper.PushItem(jbc::JBCPrimType::kTypeShort);
  EXPECT_EQ(stackHelper.Dup(jbc::kOpDup2), true);
  std::vector<jbc::JBCPrimType> types1({ jbc::JBCPrimType::kTypeInt, jbc::JBCPrimType::kTypeShort,
                                         jbc::JBCPrimType::kTypeInt, jbc::JBCPrimType::kTypeShort });
  EXPECT_EQ(stackHelper.PopItems(types1), true);

  // mode 2: ..., typeII ->
  // input: ..., J ->
  // output: ..., J, J ->
  stackHelper.Reset();
  stackHelper.PushItem(jbc::JBCPrimType::kTypeLong);
  EXPECT_EQ(stackHelper.Dup(jbc::kOpDup2), true);
  std::vector<jbc::JBCPrimType> types2({ jbc::JBCPrimType::kTypeLong, jbc::JBCPrimType::kTypeLong });
  EXPECT_EQ(stackHelper.PopItems(types2), true);

  // TODO(UT): add false cases
}

TEST(JBCStackHelper, Dup2X1) {
  JBCStackHelper stackHelper;
  // mode 1: ..., typeI, typeI, typeI ->
  // input: ..., I, S, B ->
  // output: ..., S, B, I, S, B ->
  stackHelper.PushItem(jbc::JBCPrimType::kTypeInt);
  stackHelper.PushItem(jbc::JBCPrimType::kTypeShort);
  stackHelper.PushItem(jbc::JBCPrimType::kTypeByteOrBoolean);
  EXPECT_EQ(stackHelper.Dup(jbc::kOpDup2X1), true);
  std::vector<jbc::JBCPrimType> types1({ jbc::JBCPrimType::kTypeShort, jbc::JBCPrimType::kTypeByteOrBoolean,
                                         jbc::JBCPrimType::kTypeInt, jbc::JBCPrimType::kTypeShort,
                                         jbc::JBCPrimType::kTypeByteOrBoolean });
  EXPECT_EQ(stackHelper.PopItems(types1), true);

  // mode 2: ..., typeI, typeII ->
  // input: ..., I, J ->
  // output: ..., J, I, J ->
  stackHelper.Reset();
  stackHelper.PushItem(jbc::JBCPrimType::kTypeInt);
  stackHelper.PushItem(jbc::JBCPrimType::kTypeLong);
  EXPECT_EQ(stackHelper.Dup(jbc::kOpDup2X1), true);
  std::vector<jbc::JBCPrimType> types2({ jbc::JBCPrimType::kTypeLong, jbc::JBCPrimType::kTypeInt,
                                         jbc::JBCPrimType::kTypeLong });
  EXPECT_EQ(stackHelper.PopItems(types2), true);

  // TODO(UT): add false cases
}

TEST(JBCStackHelper, Dup2X2) {
  JBCStackHelper stackHelper;
  // mode 1: ..., typeI, typeI, typeI, typeI ->
  // input: ..., I, S, B, C ->
  // output: ..., B, C, I, S, B, C ->
  stackHelper.PushItem(jbc::JBCPrimType::kTypeInt);
  stackHelper.PushItem(jbc::JBCPrimType::kTypeShort);
  stackHelper.PushItem(jbc::JBCPrimType::kTypeByteOrBoolean);
  stackHelper.PushItem(jbc::JBCPrimType::kTypeChar);
  EXPECT_EQ(stackHelper.Dup(jbc::kOpDup2X2), true);
  std::vector<jbc::JBCPrimType> types1({ jbc::JBCPrimType::kTypeByteOrBoolean, jbc::JBCPrimType::kTypeChar,
                                         jbc::JBCPrimType::kTypeInt, jbc::JBCPrimType::kTypeShort,
                                         jbc::JBCPrimType::kTypeByteOrBoolean, jbc::JBCPrimType::kTypeChar });
  EXPECT_EQ(stackHelper.PopItems(types1), true);

  // mode 2: ..., typeI, typeI, typeII ->
  // input: ..., I, S, J ->
  // output: ..., J, I, S, J ->
  stackHelper.Reset();
  stackHelper.PushItem(jbc::JBCPrimType::kTypeInt);
  stackHelper.PushItem(jbc::JBCPrimType::kTypeShort);
  stackHelper.PushItem(jbc::JBCPrimType::kTypeLong);
  EXPECT_EQ(stackHelper.Dup(jbc::kOpDup2X2), true);
  std::vector<jbc::JBCPrimType> types2({ jbc::JBCPrimType::kTypeLong, jbc::JBCPrimType::kTypeInt,
                                         jbc::JBCPrimType::kTypeShort, jbc::JBCPrimType::kTypeLong });
  EXPECT_EQ(stackHelper.PopItems(types2), true);

  // mode 3: ..., typeII, typeI, typeI ->
  // input: ..., J, I, S ->
  // output: ..., I, S, J, I, S ->
  stackHelper.Reset();
  stackHelper.PushItem(jbc::JBCPrimType::kTypeLong);
  stackHelper.PushItem(jbc::JBCPrimType::kTypeInt);
  stackHelper.PushItem(jbc::JBCPrimType::kTypeShort);
  EXPECT_EQ(stackHelper.Dup(jbc::kOpDup2X2), true);
  std::vector<jbc::JBCPrimType> types3({ jbc::JBCPrimType::kTypeInt, jbc::JBCPrimType::kTypeShort,
                                         jbc::JBCPrimType::kTypeLong, jbc::JBCPrimType::kTypeInt,
                                         jbc::JBCPrimType::kTypeShort });
  EXPECT_EQ(stackHelper.PopItems(types3), true);

  // mode 4: ..., typeII, typeII ->
  // input: ..., J, D ->
  // output: ..., D, J, D ->
  stackHelper.Reset();
  stackHelper.PushItem(jbc::JBCPrimType::kTypeLong);
  stackHelper.PushItem(jbc::JBCPrimType::kTypeDouble);
  EXPECT_EQ(stackHelper.Dup(jbc::kOpDup2X2), true);
  std::vector<jbc::JBCPrimType> types4({ jbc::JBCPrimType::kTypeDouble, jbc::JBCPrimType::kTypeLong,
                                         jbc::JBCPrimType::kTypeDouble });
  EXPECT_EQ(stackHelper.PopItems(types4), true);

  // TODO(UT): add false cases
}

TEST(JBCStackHelper, Swap) {
  JBCStackHelper stackHelper;
  stackHelper.PushItem(jbc::JBCPrimType::kTypeInt);
  stackHelper.PushItem(jbc::JBCPrimType::kTypeShort);
  EXPECT_EQ(stackHelper.Swap(), true);
  std::vector<jbc::JBCPrimType> types({ jbc::JBCPrimType::kTypeShort, jbc::JBCPrimType::kTypeInt });
  EXPECT_EQ(stackHelper.PopItems(types), true);

  // TODO(UT): add false cases
}
}  // namespace maple