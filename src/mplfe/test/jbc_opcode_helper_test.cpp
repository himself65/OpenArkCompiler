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
#include "fe_configs.h"
#include "base64.h"
#include "mempool.h"
#include "mempool_allocator.h"
#include "jbc_class.h"
#include "jbc_opcode.h"
#include "jbc_opcode_helper.h"
#include "redirect_buffer.h"

namespace maple {
class JBCOpcodeHelperTest : public testing::Test, public RedirectBuffer {
 public:
  JBCOpcodeHelperTest()
      : allocator(mp),
        klass(allocator),
        method(allocator, klass),
        helper(method) {}
  ~JBCOpcodeHelperTest() = default;

  static void SetUpTestCase() {
    mp = memPoolCtrler.NewMemPool("MemPool for JBCOpcodeHelperTest");
  }

  static void TearDownTestCase() {
    memPoolCtrler.DeleteMemPool(mp);
    mp = nullptr;
  }

 LLT_PRIVATE:
  static MemPool *mp;
  MapleAllocator allocator;
  jbc::JBCClass klass;
  jbc::JBCClassMethod method;
  JBCOpcodeHelper helper;
};  // class JBCOpcodeHelperTest
MemPool *JBCOpcodeHelperTest::mp = nullptr;

TEST_F(JBCOpcodeHelperTest, GetBaseTypeNamesForOPDefault) {
  jbc::JBCOpGoto *op = mp->New<jbc::JBCOpGoto>(allocator, jbc::kOpGoto, jbc::kOpKindGoto, false);
  bool success = false;
  std::vector<std::string> ans = helper.GetBaseTypeNamesForOP(*op, success);
  EXPECT_EQ(success, true);
  EXPECT_EQ(ans.size(), 0);
}

TEST_F(JBCOpcodeHelperTest, GetBaseTypeNamesForOPConst_Class) {
  uint16 idxConstClass;
  (void)klass.constPool.NewConstClass(idxConstClass, "Ljava/lang/Object;");
  jbc::JBCOpConst *op = mp->New<jbc::JBCOpConst>(allocator, jbc::kOpLdc, jbc::kOpKindConst, false);
  op->SetIndex(idxConstClass);
  bool success = false;
  std::vector<std::string> ans = helper.GetBaseTypeNamesForOP(*op, success);
  EXPECT_EQ(success, true);
  ASSERT_EQ(ans.size(), 1);
  EXPECT_EQ(ans[0], "Ljava/lang/Object;");
  op->SetIndex(0xFFFF);
  ans = helper.GetBaseTypeNamesForOP(*op, success);
  EXPECT_EQ(success, false);
  EXPECT_EQ(ans.size(), 0);
}

TEST_F(JBCOpcodeHelperTest, GetBaseTypeNamesForOPFieldOpr) {
  uint16 idxConstRef;
  (void)klass.constPool.NewConstRef(idxConstRef, jbc::kConstFieldRef, "LTest;", "field", "Ljava/lang/String;");
  jbc::JBCOpFieldOpr *op = mp->New<jbc::JBCOpFieldOpr>(allocator, jbc::kOpGetField, jbc::kOpKindFieldOpr, false);
  op->SetFieldIdx(idxConstRef);
  bool success = false;
  std::vector<std::string> ans = helper.GetBaseTypeNamesForOP(*op, success);
  EXPECT_EQ(success, true);
  ASSERT_EQ(ans.size(), 1);
  EXPECT_EQ(ans[0], "Ljava/lang/String;");
  op->SetFieldIdx(0xFFFF);
  ans = helper.GetBaseTypeNamesForOP(*op, success);
  EXPECT_EQ(success, false);
  EXPECT_EQ(ans.size(), 0);
}

TEST_F(JBCOpcodeHelperTest, GetBaseTypeNamesForOPInvoke) {
  uint16 idxConstRef;
  (void)klass.constPool.NewConstRef(idxConstRef, jbc::kConstMethodRef, "LTest;", "method",
                                         "(Ljava/lang/String;IJ)Ljava/lang/Object;");
  jbc::JBCOpInvoke *op = mp->New<jbc::JBCOpInvoke>(allocator, jbc::kOpInvokeVirtual, jbc::kOpKindInvoke, false);
  op->SetMethodIdx(idxConstRef);
  bool success = false;
  std::vector<std::string> ans = helper.GetBaseTypeNamesForOP(*op, success);
  EXPECT_EQ(success, true);
  ASSERT_EQ(ans.size(), 4);
  EXPECT_EQ(ans[0], "Ljava/lang/Object;");
  EXPECT_EQ(ans[1], "Ljava/lang/String;");
  EXPECT_EQ(ans[2], "I");
  EXPECT_EQ(ans[3], "J");
  op->SetMethodIdx(0xFFFF);
  ans = helper.GetBaseTypeNamesForOP(*op, success);
  EXPECT_EQ(success, false);
  EXPECT_EQ(ans.size(), 0);
}

TEST_F(JBCOpcodeHelperTest, GetBaseTypeNamesForOPNew) {
  uint16 idxConstClass;
  (void)klass.constPool.NewConstClass(idxConstClass, "Ljava/lang/Integer;");
  jbc::JBCOpNew *op = mp->New<jbc::JBCOpNew>(allocator, jbc::kOpNew, jbc::kOpKindNew, false);
  op->SetRefTypeIdx(idxConstClass);
  bool success = false;
  std::vector<std::string> ans = helper.GetBaseTypeNamesForOP(*op, success);
  EXPECT_EQ(success, true);
  ASSERT_EQ(ans.size(), 1);
  EXPECT_EQ(ans[0], "Ljava/lang/Integer;");
  op->SetRefTypeIdx(0xFFFF);
  ans = helper.GetBaseTypeNamesForOP(*op, success);
  EXPECT_EQ(success, false);
  EXPECT_EQ(ans.size(), 0);
}

TEST_F(JBCOpcodeHelperTest, GetBaseTypeNamesForOPNew_Array) {
  uint16 idxConstClass;
  (void)klass.constPool.NewConstClass(idxConstClass, "[Ljava/lang/Float;");
  jbc::JBCOpNew *op = mp->New<jbc::JBCOpNew>(allocator, jbc::kOpANewArray, jbc::kOpKindNew, false);
  op->SetRefTypeIdx(idxConstClass);
  bool success = false;
  std::vector<std::string> ans = helper.GetBaseTypeNamesForOP(*op, success);
  EXPECT_EQ(success, true);
  ASSERT_EQ(ans.size(), 1);
  EXPECT_EQ(ans[0], "Ljava/lang/Float;");
  op->SetRefTypeIdx(0xFFFF);
  ans = helper.GetBaseTypeNamesForOP(*op, success);
  EXPECT_EQ(success, false);
  EXPECT_EQ(ans.size(), 0);
}

TEST_F(JBCOpcodeHelperTest, GetBaseTypeNamesForOPMultiANewArray) {
  uint16 idxConstClass;
  (void)klass.constPool.NewConstClass(idxConstClass, "[Ljava/lang/Double;");
  jbc::JBCOpMultiANewArray *op = mp->New<jbc::JBCOpMultiANewArray>(allocator, jbc::kOpMultiANewArray,
                                                                   jbc::kOpKindMultiANewArray, false);
  op->SetRefTypeIdx(idxConstClass);
  bool success = false;
  std::vector<std::string> ans = helper.GetBaseTypeNamesForOP(*op, success);
  EXPECT_EQ(success, true);
  ASSERT_EQ(ans.size(), 1);
  EXPECT_EQ(ans[0], "Ljava/lang/Double;");
  op->SetRefTypeIdx(0xFFFF);
  ans = helper.GetBaseTypeNamesForOP(*op, success);
  EXPECT_EQ(success, false);
  EXPECT_EQ(ans.size(), 0);
}

TEST_F(JBCOpcodeHelperTest, GetBaseTypeNamesForOPTypeCheck) {
  uint16 idxConstClass;
  (void)klass.constPool.NewConstClass(idxConstClass, "Ljava/lang/Object;");
  jbc::JBCOpTypeCheck *op = mp->New<jbc::JBCOpTypeCheck>(allocator, jbc::kOpCheckCast,
                                                         jbc::kOpKindTypeCheck, false);
  op->SetTypeIdx(idxConstClass);
  bool success = false;
  std::vector<std::string> ans = helper.GetBaseTypeNamesForOP(*op, success);
  EXPECT_EQ(success, true);
  ASSERT_EQ(ans.size(), 1);
  EXPECT_EQ(ans[0], "Ljava/lang/Object;");
  op->SetTypeIdx(0xFFFF);
  ans = helper.GetBaseTypeNamesForOP(*op, success);
  EXPECT_EQ(success, false);
  EXPECT_EQ(ans.size(), 0);
}
}  // namespace maple