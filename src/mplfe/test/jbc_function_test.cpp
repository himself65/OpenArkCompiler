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
#include "jbc_function.h"
#include "redirect_buffer.h"
#include "mplfe_ut_environment.h"

namespace maple {
class JBCFunctionTest : public testing::Test, public RedirectBuffer {
 public:
  static MemPool *mp;
  MapleAllocator allocator;
  jbc::JBCClass jbcClass;
  jbc::JBCClassMethod jbcMethod;
  JBCClassMethod2FEHelper jbcMethodHelper;
  MIRFunction mirFunction;
  JBCFunction jbcFunction;
  JBCFunctionTest()
      : allocator(mp),
        jbcClass(allocator),
        jbcMethod(allocator, jbcClass),
        jbcMethodHelper(allocator, jbcMethod),
        mirFunction(&MPLFEUTEnvironment::GetMIRModule(), StIdx(0, 0)),
        jbcFunction(jbcMethodHelper, mirFunction, std::make_unique<FEFunctionPhaseResult>(true)) {}
  ~JBCFunctionTest() = default;

  static void SetUpTestCase() {
    mp = memPoolCtrler.NewMemPool("MemPool for JBCFunctionTest");
  }

  static void TearDownTestCase() {
    memPoolCtrler.DeleteMemPool(mp);
    mp = nullptr;
  }
};
MemPool *JBCFunctionTest::mp = nullptr;

TEST_F(JBCFunctionTest, BuildTryInfo_Case1) {
  std::map<std::pair<uint32, uint32>, std::vector<uint32>> rawInfo;
  /* Case1
   * S(0)--E(10)--C(20)
   */
  rawInfo[std::make_pair(0, 10)] = std::vector<uint32>({ 20 });
  std::map<uint32, uint32> outMapStartEnd;
  std::map<uint32, std::vector<uint32>> outMapStartCatch;
  jbcFunction.BuildTryInfo(rawInfo, outMapStartEnd, outMapStartCatch);
  EXPECT_EQ(outMapStartEnd[0], 10);
  ASSERT_EQ(outMapStartCatch[0].size(), 1);
  EXPECT_EQ(outMapStartCatch[0][0], 20);
}

TEST_F(JBCFunctionTest, BuildTryInfo_Case2) {
  std::map<std::pair<uint32, uint32>, std::vector<uint32>> rawInfo;
  /* Case2
   * S(0)--E(10)--C(10)
   */
  rawInfo[std::make_pair(0, 10)] = std::vector<uint32>({ 10 });
  std::map<uint32, uint32> outMapStartEnd;
  std::map<uint32, std::vector<uint32>> outMapStartCatch;
  jbcFunction.BuildTryInfo(rawInfo, outMapStartEnd, outMapStartCatch);
  EXPECT_EQ(outMapStartEnd[0], 10);
  ASSERT_EQ(outMapStartCatch[0].size(), 1);
  EXPECT_EQ(outMapStartCatch[0][0], 10);
}

TEST_F(JBCFunctionTest, BuildTryInfo_Case3) {
  std::map<std::pair<uint32, uint32>, std::vector<uint32>> rawInfo;
  /* Case3
   * S(0)--E(10)--C(5)
   */
  rawInfo[std::make_pair(0, 10)] = std::vector<uint32>({ 5 });
  std::map<uint32, uint32> outMapStartEnd;
  std::map<uint32, std::vector<uint32>> outMapStartCatch;
  jbcFunction.BuildTryInfo(rawInfo, outMapStartEnd, outMapStartCatch);
  EXPECT_EQ(outMapStartEnd[0], 5);
  EXPECT_EQ(outMapStartEnd[5], 10);
  ASSERT_EQ(outMapStartCatch[0].size(), 1);
  ASSERT_EQ(outMapStartCatch[5].size(), 1);
  EXPECT_EQ(outMapStartCatch[0][0], 5);
  EXPECT_EQ(outMapStartCatch[5][0], 5);
}

TEST_F(JBCFunctionTest, BuildTryInfo_Case4) {
  std::map<std::pair<uint32, uint32>, std::vector<uint32>> rawInfo;
  /* Case4
   * S1/S2(0)--E1(10)--E2(20)--C1(25)--C2(30)
   */
  rawInfo[std::make_pair(0, 10)] = std::vector<uint32>({ 25 });
  rawInfo[std::make_pair(0, 20)] = std::vector<uint32>({ 30 });
  std::map<uint32, uint32> outMapStartEnd;
  std::map<uint32, std::vector<uint32>> outMapStartCatch;
  jbcFunction.BuildTryInfo(rawInfo, outMapStartEnd, outMapStartCatch);
  EXPECT_EQ(outMapStartEnd[0], 10);
  EXPECT_EQ(outMapStartEnd[10], 20);
  ASSERT_EQ(outMapStartCatch[0].size(), 2);
  ASSERT_EQ(outMapStartCatch[10].size(), 1);
  EXPECT_EQ(outMapStartCatch[0][0], 25);
  EXPECT_EQ(outMapStartCatch[0][1], 30);
  EXPECT_EQ(outMapStartCatch[10][0], 30);
}

TEST_F(JBCFunctionTest, BuildTryInfo_Case5) {
  std::map<std::pair<uint32, uint32>, std::vector<uint32>> rawInfo;
  /* Case5
   * S(0)--E(10)--C_1(20)--C_2(30)
   */
  rawInfo[std::make_pair(0, 10)] = std::vector<uint32>({ 20, 30 });
  std::map<uint32, uint32> outMapStartEnd;
  std::map<uint32, std::vector<uint32>> outMapStartCatch;
  jbcFunction.BuildTryInfo(rawInfo, outMapStartEnd, outMapStartCatch);
  EXPECT_EQ(outMapStartEnd[0], 10);
  ASSERT_EQ(outMapStartCatch[0].size(), 2);
  EXPECT_EQ(outMapStartCatch[0][0], 20);
  EXPECT_EQ(outMapStartCatch[0][1], 30);
}

TEST_F(JBCFunctionTest, BuildTryInfo_Case6) {
  std::map<std::pair<uint32, uint32>, std::vector<uint32>> rawInfo;
  /* Case6
   * S(0)--C_1(5)--E(10)--C_2(20)
   */
  rawInfo[std::make_pair(0, 10)] = std::vector<uint32>({ 5, 20 });
  std::map<uint32, uint32> outMapStartEnd;
  std::map<uint32, std::vector<uint32>> outMapStartCatch;
  jbcFunction.BuildTryInfo(rawInfo, outMapStartEnd, outMapStartCatch);
  EXPECT_EQ(outMapStartEnd[0], 5);
  EXPECT_EQ(outMapStartEnd[5], 10);
  ASSERT_EQ(outMapStartCatch[0].size(), 2);
  EXPECT_EQ(outMapStartCatch[0][0], 5);
  EXPECT_EQ(outMapStartCatch[0][1], 20);
  ASSERT_EQ(outMapStartCatch[5].size(), 2);
  EXPECT_EQ(outMapStartCatch[5][0], 5);
  EXPECT_EQ(outMapStartCatch[5][1], 20);
}

TEST_F(JBCFunctionTest, BuildTryInfo_Case7) {
  std::map<std::pair<uint32, uint32>, std::vector<uint32>> rawInfo;
  /* Case7
   * S1(0)-E1(10)-C1(20)-S2(30)-E2(40)-C2(50)
   */
  rawInfo[std::make_pair(0, 10)] = std::vector<uint32>({ 20 });
  rawInfo[std::make_pair(30, 40)] = std::vector<uint32>({ 50 });
  std::map<uint32, uint32> outMapStartEnd;
  std::map<uint32, std::vector<uint32>> outMapStartCatch;
  jbcFunction.BuildTryInfo(rawInfo, outMapStartEnd, outMapStartCatch);
  EXPECT_EQ(outMapStartEnd[0], 10);
  EXPECT_EQ(outMapStartEnd[30], 40);
  ASSERT_EQ(outMapStartCatch[0].size(), 1);
  EXPECT_EQ(outMapStartCatch[0][0], 20);
  ASSERT_EQ(outMapStartCatch[30].size(), 1);
  EXPECT_EQ(outMapStartCatch[30][0], 50);
}
}  // namespace maple