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
#include <memory>
#include "jbc_stack_helper.h"
#include "jbc_stack2fe_helper.h"
#include "feir_test_base.h"
#include "mplfe_ut_regx.h"

namespace maple {
class JBCStack2FEHelperTest : public FEIRTestBase {
 public:
  JBCStackHelper stackHelper;
  JBCStack2FEHelper helper;
  JBCStack2FEHelperTest() = default;
  ~JBCStack2FEHelperTest() = default;
};  // class JBCStack2FEHelperTest

TEST_F(JBCStack2FEHelperTest, GetRegNumForSlot) {
  helper.SetNStacks(8);
  helper.SetNSwaps(2);
  helper.SetNLocals(4);
  helper.SetNArgs(4);
  // 0~7: stack
  // 8~9: swap
  // 10~13: local
  // 14~17: arg
  EXPECT_EQ(helper.GetRegNumForSlot(0), 14);
  EXPECT_EQ(helper.GetRegNumForSlot(2), 16);
  EXPECT_EQ(helper.GetRegNumForSlot(4), 10);
  EXPECT_EQ(helper.GetRegNumForSlot(6), 12);
}

TEST_F(JBCStack2FEHelperTest, PushItem_PopItem) {
  helper.SetNStacks(3);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(0), PTY_i32), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(1), PTY_i64), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "IJJ");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "NWD");
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(3), PTY_f32), false);
  UniqueFEIRVar var = helper.PopItem(PTY_i32);
  EXPECT_EQ(var, nullptr);
  var = helper.PopItem(PTY_i64);
  EXPECT_EQ(var->GetNameRaw(), "Reg1");
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "I");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "N");
  var = helper.PopItem(PTY_i64);
  EXPECT_EQ(var, nullptr);
  var = helper.PopItem(PTY_i32);
  EXPECT_EQ(var->GetNameRaw(), "Reg0");
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "");
  var = helper.PopItem(PTY_i32);
  EXPECT_EQ(var, nullptr);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(0), PTY_i32), true);
  var = helper.PopItem(PTY_f32);
  EXPECT_EQ(var, nullptr);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(1), PTY_i64), true);
  var = helper.PopItem(PTY_f64);
  EXPECT_EQ(var, nullptr);
}

TEST_F(JBCStack2FEHelperTest, GetRegNumForStack) {
  helper.SetNStacks(6);
  helper.ClearStack();
  EXPECT_EQ(helper.GetRegNumForStack(), 0);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(0), PTY_i64), true);
  EXPECT_EQ(helper.GetRegNumForStack(), 2);
  helper.ClearStack();
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(1), PTY_i64), true);
  EXPECT_EQ(helper.GetRegNumForStack(), 0);
}

TEST_F(JBCStack2FEHelperTest, Swap) {
  helper.SetNStacks(6);
  helper.ClearStack();
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(0), PTY_i32), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(1), PTY_f32), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "IF");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "NN");
  EXPECT_EQ(helper.Swap(), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "FI");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "NN");
  UniqueFEIRVar var = helper.PopItem(PTY_i32);
  EXPECT_EQ(var->GetNameRaw(), "Reg0");
  var = helper.PopItem(PTY_f32);
  EXPECT_EQ(var->GetNameRaw(), "Reg1");
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "");
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(0), PTY_i64), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(2), PTY_f64), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "JJDD");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "WDWD");
  EXPECT_EQ(helper.Swap(), false);
  helper.ClearStack();
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(0), PTY_i32), true);
  EXPECT_EQ(helper.Swap(), false);
}

TEST_F(JBCStack2FEHelperTest, Pop) {
  helper.SetNStacks(6);
  helper.ClearStack();
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(0), PTY_i32), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(1), PTY_f32), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(2), PTY_ref), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "IFR");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "NNN");
  EXPECT_EQ(helper.Pop(jbc::kOpPop), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "IF");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "NN");
  EXPECT_EQ(helper.Pop(jbc::kOpPop2), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "");
  EXPECT_EQ(helper.Pop(jbc::kOpPop), false);
  EXPECT_EQ(helper.Pop(jbc::kOpPop2), false);
  EXPECT_EQ(helper.Pop(jbc::kOpDup), false);
}

TEST_F(JBCStack2FEHelperTest, Pop2) {
  helper.SetNStacks(6);
  helper.ClearStack();
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(0), PTY_i64), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(2), PTY_f64), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "JJDD");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "WDWD");
  EXPECT_EQ(helper.Pop(jbc::kOpPop), false);
  EXPECT_EQ(helper.Pop(jbc::kOpPop2), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "JJ");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "WD");
  EXPECT_EQ(helper.Pop(jbc::kOpPop2), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "");
  EXPECT_EQ(helper.Pop(jbc::kOpPop2), false);
  EXPECT_EQ(helper.Pop(jbc::kOpDup), false);
}

TEST_F(JBCStack2FEHelperTest, Dup_Correct) {
  helper.SetNStacks(6);
  helper.ClearStack();
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(0), PTY_i32), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "I");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "N");
  EXPECT_EQ(helper.Dup(jbc::kOpDup), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "II");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "NN");
  PrimType pty;
  UniqueFEIRVar var1 = helper.PopItem(false, pty);
  EXPECT_EQ(pty, PTY_i32);
  EXPECT_EQ(var1->GetNameRaw(), "Reg0");
  UniqueFEIRVar var2 = helper.PopItem(false, pty);
  EXPECT_EQ(pty, PTY_i32);
  EXPECT_EQ(var2->GetNameRaw(), "Reg0");
}

TEST_F(JBCStack2FEHelperTest, Dup_Error_Empty) {
  helper.SetNStacks(6);
  helper.ClearStack();
  EXPECT_EQ(helper.Dup(jbc::kOpDup), false);
}

TEST_F(JBCStack2FEHelperTest, Dup_Error_WrongType) {
  helper.SetNStacks(6);
  helper.ClearStack();
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(0), PTY_i64), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "JJ");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "WD");
  EXPECT_EQ(helper.Dup(jbc::kOpDup), false);
}

TEST_F(JBCStack2FEHelperTest, DupX1_Correct) {
  helper.SetNStacks(6);
  helper.ClearStack();
  // before: Reg0_ref, Reg1_i32, Reg2_f32, ->
  // after:  Reg0_ref, Reg2_f32, Reg1_i32, Reg2_f32, ->
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(0), PTY_ref), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(1), PTY_i32), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(2), PTY_f32), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "RIF");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "NNN");
  EXPECT_EQ(helper.Dup(jbc::kOpDupX1), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "RFIF");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "NNNN");
  PrimType pty;
  UniqueFEIRVar var = helper.PopItem(false, pty);
  EXPECT_EQ(pty, PTY_f32);
  EXPECT_EQ(var->GetNameRaw(), "Reg2");
  UniqueFEIRVar var2 = helper.PopItem(false, pty);
  EXPECT_EQ(pty, PTY_i32);
  EXPECT_EQ(var2->GetNameRaw(), "Reg1");
  UniqueFEIRVar var3 = helper.PopItem(false, pty);
  EXPECT_EQ(pty, PTY_f32);
  EXPECT_EQ(var3->GetNameRaw(), "Reg2");
}

TEST_F(JBCStack2FEHelperTest, DupX1_Error_NotEnough) {
  helper.SetNStacks(6);
  helper.ClearStack();
  // before: ->
  // after:  Error
  EXPECT_EQ(helper.Dup(jbc::kOpDupX1), false);
  // before: ->
  // after:  Error
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(0), PTY_ref), true);
  EXPECT_EQ(helper.Dup(jbc::kOpDupX1), false);
}

TEST_F(JBCStack2FEHelperTest, DupX1_Error_WrongType) {
  helper.SetNStacks(6);
  helper.ClearStack();
  // before: Reg0_ref, Reg1_i64, ->
  // after:  Error
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(0), PTY_ref), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(1), PTY_i64), true);
  EXPECT_EQ(helper.Dup(jbc::kOpDupX1), false);
}

TEST_F(JBCStack2FEHelperTest, DupX2_Correct_S1) {
  helper.SetNStacks(6);
  helper.ClearStack();
  // before: Reg0_ref, Reg1_ref, Reg2_i32, Reg3_f32, ->
  // after:  Reg0_ref, Reg3_f32, Reg1_ref, Reg2_i32, Reg3_f32, ->
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(0), PTY_ref), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(1), PTY_ref), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(2), PTY_i32), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(3), PTY_f32), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "RRIF");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "NNNN");
  EXPECT_EQ(helper.Dup(jbc::kOpDupX2), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "RFRIF");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "NNNNN");
  PrimType pty;
  UniqueFEIRVar var = helper.PopItem(false, pty);
  EXPECT_EQ(pty, PTY_f32);
  EXPECT_EQ(var->GetNameRaw(), "Reg3");
  UniqueFEIRVar var2 = helper.PopItem(false, pty);
  EXPECT_EQ(pty, PTY_i32);
  EXPECT_EQ(var2->GetNameRaw(), "Reg2");
  UniqueFEIRVar var3 = helper.PopItem(false, pty);
  EXPECT_EQ(pty, PTY_ref);
  EXPECT_EQ(var3->GetNameRaw(), "Reg1");
  UniqueFEIRVar var4 = helper.PopItem(false, pty);
  EXPECT_EQ(pty, PTY_f32);
  EXPECT_EQ(var4->GetNameRaw(), "Reg3");
}

TEST_F(JBCStack2FEHelperTest, DupX2_Correct_S2) {
  helper.SetNStacks(6);
  helper.ClearStack();
  // before: Reg0_ref, Reg1_i64, Reg3_i32, ->
  // after:  Reg0_ref, Reg3_i32, Reg1_i64, Reg3_i32, ->
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(0), PTY_ref), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(1), PTY_i64), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(3), PTY_i32), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "RJJI");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "NWDN");
  EXPECT_EQ(helper.Dup(jbc::kOpDupX2), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "RIJJI");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "NNWDN");
  PrimType pty;
  UniqueFEIRVar var1 = helper.PopItem(false, pty);
  EXPECT_EQ(pty, PTY_i32);
  EXPECT_EQ(var1->GetNameRaw(), "Reg3");
  UniqueFEIRVar var2 = helper.PopItem(true, pty);
  EXPECT_EQ(pty, PTY_i64);
  EXPECT_EQ(var2->GetNameRaw(), "Reg1");
  UniqueFEIRVar var3 = helper.PopItem(false, pty);
  EXPECT_EQ(pty, PTY_i32);
  EXPECT_EQ(var3->GetNameRaw(), "Reg3");
}

TEST_F(JBCStack2FEHelperTest, Dup2_Correct_S1) {
  helper.SetNStacks(6);
  helper.ClearStack();
  // before: Reg0_ref, Reg1_i32, Reg2_f32, ->
  // after:  Reg0_ref, Reg1_i32, Reg2_f32, Reg1_i32, Reg2_f32, ->
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(0), PTY_ref), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(1), PTY_i32), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(2), PTY_f32), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "RIF");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "NNN");
  EXPECT_EQ(helper.Dup(jbc::kOpDup2), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "RIFIF");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "NNNNN");
  PrimType pty;
  UniqueFEIRVar var1 = helper.PopItem(false, pty);
  EXPECT_EQ(pty, PTY_f32);
  EXPECT_EQ(var1->GetNameRaw(), "Reg2");
  UniqueFEIRVar var2 = helper.PopItem(false, pty);
  EXPECT_EQ(pty, PTY_i32);
  EXPECT_EQ(var2->GetNameRaw(), "Reg1");
  UniqueFEIRVar var3 = helper.PopItem(false, pty);
  EXPECT_EQ(pty, PTY_f32);
  EXPECT_EQ(var3->GetNameRaw(), "Reg2");
  UniqueFEIRVar var4 = helper.PopItem(false, pty);
  EXPECT_EQ(pty, PTY_i32);
  EXPECT_EQ(var4->GetNameRaw(), "Reg1");
}

TEST_F(JBCStack2FEHelperTest, Dup2_Correct_S2) {
  helper.SetNStacks(6);
  helper.ClearStack();
  // before: Reg0_ref, Reg1_i64 ->
  // after:  Reg0_ref, Reg1_i64, Reg1_i64 ->
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(0), PTY_ref), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(1), PTY_i64), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "RJJ");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "NWD");
  EXPECT_EQ(helper.Dup(jbc::kOpDup2), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "RJJJJ");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "NWDWD");
  PrimType pty;
  UniqueFEIRVar var1 = helper.PopItem(true, pty);
  EXPECT_EQ(pty, PTY_i64);
  EXPECT_EQ(var1->GetNameRaw(), "Reg1");
  UniqueFEIRVar var2 = helper.PopItem(true, pty);
  EXPECT_EQ(pty, PTY_i64);
  EXPECT_EQ(var2->GetNameRaw(), "Reg1");
}

TEST_F(JBCStack2FEHelperTest, Dup2X1_Correct_S1) {
  helper.SetNStacks(6);
  helper.ClearStack();
  // before: Reg0_ref, Reg1_i32, Reg2_f32, Reg3_ref ->
  // after:  Reg0_ref, Reg2_f32, Reg3_ref, Reg1_i32, Reg2_f32, Reg3_ref ->
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(0), PTY_ref), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(1), PTY_i32), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(2), PTY_f32), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(3), PTY_ref), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "RIFR");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "NNNN");
  EXPECT_EQ(helper.Dup(jbc::kOpDup2X1), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "RFRIFR");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "NNNNNN");
  PrimType pty;
  UniqueFEIRVar var1 = helper.PopItemAuto(pty);
  EXPECT_EQ(pty, PTY_ref);
  EXPECT_EQ(var1->GetNameRaw(), "Reg3");
  UniqueFEIRVar var2 = helper.PopItemAuto(pty);
  EXPECT_EQ(pty, PTY_f32);
  EXPECT_EQ(var2->GetNameRaw(), "Reg2");
  UniqueFEIRVar var3 = helper.PopItemAuto(pty);
  EXPECT_EQ(pty, PTY_i32);
  EXPECT_EQ(var3->GetNameRaw(), "Reg1");
  UniqueFEIRVar var4 = helper.PopItemAuto(pty);
  EXPECT_EQ(pty, PTY_ref);
  EXPECT_EQ(var4->GetNameRaw(), "Reg3");
  UniqueFEIRVar var5 = helper.PopItemAuto(pty);
  EXPECT_EQ(pty, PTY_f32);
  EXPECT_EQ(var5->GetNameRaw(), "Reg2");
}

TEST_F(JBCStack2FEHelperTest, Dup2X1_Correct_S2) {
  helper.SetNStacks(6);
  helper.ClearStack();
  // before: Reg0_ref, Reg1_i32, Reg2_i64 ->
  // after:  Reg0_ref, Reg2_i64, Reg1_i32, Reg2_i64 ->
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(0), PTY_ref), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(1), PTY_i32), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(2), PTY_i64), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "RIJJ");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "NNWD");
  EXPECT_EQ(helper.Dup(jbc::kOpDup2X1), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "RJJIJJ");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "NWDNWD");
  PrimType pty;
  UniqueFEIRVar var1 = helper.PopItemAuto(pty);
  EXPECT_EQ(pty, PTY_i64);
  EXPECT_EQ(var1->GetNameRaw(), "Reg2");
  UniqueFEIRVar var2 = helper.PopItemAuto(pty);
  EXPECT_EQ(pty, PTY_i32);
  EXPECT_EQ(var2->GetNameRaw(), "Reg1");
  UniqueFEIRVar var3 = helper.PopItemAuto(pty);
  EXPECT_EQ(pty, PTY_i64);
  EXPECT_EQ(var3->GetNameRaw(), "Reg2");
}

TEST_F(JBCStack2FEHelperTest, Dup2X2_Correct_S1) {
  helper.SetNStacks(8);
  helper.ClearStack();
  // before: Reg0_ref, Reg1_i32, Reg2_f32, Reg3_i32, Reg4_ref ->
  // after:  Reg0_ref, Reg3_i32, Reg4_ref, Reg1_i32, Reg2_f32, Reg3_i32, Reg4_ref ->
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(0), PTY_ref), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(1), PTY_i32), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(2), PTY_f32), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(3), PTY_i32), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(4), PTY_ref), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "RIFIR");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "NNNNN");
  EXPECT_EQ(helper.Dup(jbc::kOpDup2X2), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "RIRIFIR");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "NNNNNNN");
  PrimType pty;
  UniqueFEIRVar var1 = helper.PopItemAuto(pty);
  EXPECT_EQ(pty, PTY_ref);
  EXPECT_EQ(var1->GetNameRaw(), "Reg4");
  UniqueFEIRVar var2 = helper.PopItemAuto(pty);
  EXPECT_EQ(pty, PTY_i32);
  EXPECT_EQ(var2->GetNameRaw(), "Reg3");
  UniqueFEIRVar var3 = helper.PopItemAuto(pty);
  EXPECT_EQ(pty, PTY_f32);
  EXPECT_EQ(var3->GetNameRaw(), "Reg2");
  UniqueFEIRVar var4 = helper.PopItemAuto(pty);
  EXPECT_EQ(pty, PTY_i32);
  EXPECT_EQ(var4->GetNameRaw(), "Reg1");
  UniqueFEIRVar var5 = helper.PopItemAuto(pty);
  EXPECT_EQ(pty, PTY_ref);
  EXPECT_EQ(var5->GetNameRaw(), "Reg4");
  UniqueFEIRVar var6 = helper.PopItemAuto(pty);
  EXPECT_EQ(pty, PTY_i32);
  EXPECT_EQ(var6->GetNameRaw(), "Reg3");
}

TEST_F(JBCStack2FEHelperTest, Dup2X2_Correct_S2) {
  helper.SetNStacks(8);
  helper.ClearStack();
  // before: Reg0_ref, Reg1_i32, Reg2_f32, Reg3_i64 ->
  // after:  Reg0_ref, Reg3_i64, Reg1_i32, Reg2_f32, Reg3_i64 ->
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(0), PTY_ref), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(1), PTY_i32), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(2), PTY_f32), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(3), PTY_i64), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "RIFJJ");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "NNNWD");
  EXPECT_EQ(helper.Dup(jbc::kOpDup2X2), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "RJJIFJJ");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "NWDNNWD");
  PrimType pty;
  UniqueFEIRVar var1 = helper.PopItemAuto(pty);
  EXPECT_EQ(pty, PTY_i64);
  EXPECT_EQ(var1->GetNameRaw(), "Reg3");
  UniqueFEIRVar var2 = helper.PopItemAuto(pty);
  EXPECT_EQ(pty, PTY_f32);
  EXPECT_EQ(var2->GetNameRaw(), "Reg2");
  UniqueFEIRVar var3 = helper.PopItemAuto(pty);
  EXPECT_EQ(pty, PTY_i32);
  EXPECT_EQ(var3->GetNameRaw(), "Reg1");
  UniqueFEIRVar var4 = helper.PopItemAuto(pty);
  EXPECT_EQ(pty, PTY_i64);
  EXPECT_EQ(var4->GetNameRaw(), "Reg3");
}

TEST_F(JBCStack2FEHelperTest, Dup2X2_Correct_S3) {
  helper.SetNStacks(8);
  helper.ClearStack();
  // before: Reg0_ref, Reg1_i64, Reg3_i32, Reg4_f32 ->
  // after:  Reg0_ref, Reg3_i32, Reg4_f32, Reg1_i64, Reg3_i32, Reg4_f32 ->
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(0), PTY_ref), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(1), PTY_i64), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(3), PTY_i32), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(4), PTY_f32), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "RJJIF");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "NWDNN");
  EXPECT_EQ(helper.Dup(jbc::kOpDup2X2), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "RIFJJIF");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "NNNWDNN");
  PrimType pty;
  UniqueFEIRVar var1 = helper.PopItemAuto(pty);
  EXPECT_EQ(pty, PTY_f32);
  EXPECT_EQ(var1->GetNameRaw(), "Reg4");
  UniqueFEIRVar var2 = helper.PopItemAuto(pty);
  EXPECT_EQ(pty, PTY_i32);
  EXPECT_EQ(var2->GetNameRaw(), "Reg3");
  UniqueFEIRVar var3 = helper.PopItemAuto(pty);
  EXPECT_EQ(pty, PTY_i64);
  EXPECT_EQ(var3->GetNameRaw(), "Reg1");
  UniqueFEIRVar var4 = helper.PopItemAuto(pty);
  EXPECT_EQ(pty, PTY_f32);
  EXPECT_EQ(var4->GetNameRaw(), "Reg4");
  UniqueFEIRVar var5 = helper.PopItemAuto(pty);
  EXPECT_EQ(pty, PTY_i32);
  EXPECT_EQ(var5->GetNameRaw(), "Reg3");
}

TEST_F(JBCStack2FEHelperTest, Dup2X2_Correct_S4) {
  helper.SetNStacks(8);
  helper.ClearStack();
  // before: Reg0_ref, Reg1_i64, Reg3_f64 ->
  // after:  Reg0_ref, Reg3_f64, Reg1_i64, Reg3_f64 ->
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(0), PTY_ref), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(1), PTY_i64), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(3), PTY_f64), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "RJJDD");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "NWDWD");
  EXPECT_EQ(helper.Dup(jbc::kOpDup2X2), true);
  EXPECT_EQ(helper.DumpStackInJavaFormat(), "RDDJJDD");
  EXPECT_EQ(helper.DumpStackInInternalFormat(), "NWDWDWD");
  PrimType pty;
  UniqueFEIRVar var1 = helper.PopItemAuto(pty);
  EXPECT_EQ(pty, PTY_f64);
  EXPECT_EQ(var1->GetNameRaw(), "Reg3");
  UniqueFEIRVar var2 = helper.PopItemAuto(pty);
  EXPECT_EQ(pty, PTY_i64);
  EXPECT_EQ(var2->GetNameRaw(), "Reg1");
  UniqueFEIRVar var3 = helper.PopItemAuto(pty);
  EXPECT_EQ(pty, PTY_f64);
  EXPECT_EQ(var3->GetNameRaw(), "Reg3");
}

TEST_F(JBCStack2FEHelperTest, GenerateSwapStmts_Test1) {
  helper.SetNStacks(5);
  helper.SetNSwaps(5);
  helper.ClearStack();
  // stack: Reg0_i32, Ref1_i64, Reg3_f64
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(0, PTY_i32), PTY_i32), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(1, PTY_i64), PTY_i64), true);
  EXPECT_EQ(helper.PushItem(std::make_unique<FEIRVarReg>(3, PTY_f64), PTY_f64), true);
  std::list<UniqueFEIRStmt> stmts = helper.GenerateSwapStmts();
  ASSERT_EQ(stmts.size(), 3);
  std::list<UniqueFEIRStmt>::iterator it = stmts.begin();
  std::list<StmtNode*> mirStmts0 = it->get()->GenMIRStmts(mirBuilder);
  it++;
  std::list<StmtNode*> mirStmts1 = it->get()->GenMIRStmts(mirBuilder);
  it++;
  std::list<StmtNode*> mirStmts2 = it->get()->GenMIRStmts(mirBuilder);

  RedirectCout();
  // stmt 0: dassign %Reg5_f64 0 (dread f64 %Reg3_f64)
  ASSERT_EQ(mirStmts0.size(), 1);
  mirStmts0.front()->Dump();
  std::string dumpStr = GetBufferString();
  std::string pattern = std::string("dassign %Reg5_f64 0 \\(dread f64 %Reg3_f64\\)") + MPLFEUTRegx::Any();
  EXPECT_EQ(MPLFEUTRegx::Match(dumpStr, pattern), true);

  // stmt 1: dassign %Reg7_i64 0 (dread i64 %Reg1_i64)
  ClearBufferString();
  ASSERT_EQ(mirStmts1.size(), 1);
  mirStmts1.front()->Dump();
  dumpStr = GetBufferString();
  pattern = std::string("dassign %Reg7_i64 0 \\(dread i64 %Reg1_i64\\)") + MPLFEUTRegx::Any();
  EXPECT_EQ(MPLFEUTRegx::Match(dumpStr, pattern), true);

  // stmt 2: dassign %Reg9_i32 0 (dread i32 %Reg0_i32)
  ClearBufferString();
  ASSERT_EQ(mirStmts2.size(), 1);
  mirStmts2.front()->Dump();
  dumpStr = GetBufferString();
  pattern = std::string("dassign %Reg9_i32 0 \\(dread i32 %Reg0_i32\\)") + MPLFEUTRegx::Any();
  EXPECT_EQ(MPLFEUTRegx::Match(dumpStr, pattern), true);
  RestoreCout();
}

TEST_F(JBCStack2FEHelperTest, LoadSwapStack_Test1) {
  stackHelper.Reset();
  helper.SetNStacks(5);
  helper.SetNSwaps(5);
  stackHelper.PushItem(jbc::JBCPrimType::kTypeInt);
  stackHelper.PushItem(jbc::JBCPrimType::kTypeLong);
  stackHelper.PushItem(jbc::JBCPrimType::kTypeDouble);
  bool success;
  std::list<UniqueFEIRStmt> stmts = helper.LoadSwapStack(stackHelper, success);
  EXPECT_EQ(success, true);
  ASSERT_EQ(stmts.size(), 3);
  std::list<UniqueFEIRStmt>::iterator it = stmts.begin();
  std::list<StmtNode*> mirStmts0 = it->get()->GenMIRStmts(mirBuilder);
  it++;
  std::list<StmtNode*> mirStmts1 = it->get()->GenMIRStmts(mirBuilder);
  it++;
  std::list<StmtNode*> mirStmts2 = it->get()->GenMIRStmts(mirBuilder);

  // swap detail
  // swap stack: Reg9_i32, Reg7_i64, Reg5_f64
  // jbc stack:  Reg0_i32, Reg1_i64, Reg3_f64

  // stack check
  PrimType pty;
  UniqueFEIRVar var1 = helper.PopItemAuto(pty);
  EXPECT_EQ(pty, PTY_f64);
  EXPECT_EQ(var1->GetNameRaw(), "Reg3");
  UniqueFEIRVar var2 = helper.PopItemAuto(pty);
  EXPECT_EQ(pty, PTY_i64);
  EXPECT_EQ(var2->GetNameRaw(), "Reg1");
  UniqueFEIRVar var3 = helper.PopItemAuto(pty);
  EXPECT_EQ(pty, PTY_i32);
  EXPECT_EQ(var3->GetNameRaw(), "Reg0");

  // stmts check
  RedirectCout();
  // stmt 0: dassign %Reg0_i32 0 (dread i32 %Reg9_i32)
  ASSERT_EQ(mirStmts0.size(), 1);
  mirStmts0.front()->Dump();
  std::string dumpStr = GetBufferString();
  std::string pattern = std::string("dassign %Reg0_i32 0 \\(dread i32 %Reg9_i32\\)") + MPLFEUTRegx::Any();
  EXPECT_EQ(MPLFEUTRegx::Match(dumpStr, pattern), true);

  // stmt 1: dassign %Reg1_i64 0 (dread i64 %Reg7_i64)
  ClearBufferString();
  ASSERT_EQ(mirStmts1.size(), 1);
  mirStmts1.front()->Dump();
  dumpStr = GetBufferString();
  pattern = std::string("dassign %Reg1_i64 0 \\(dread i64 %Reg7_i64\\)") + MPLFEUTRegx::Any();
  EXPECT_EQ(MPLFEUTRegx::Match(dumpStr, pattern), true);

  // stmt 2: dassign %Reg3_f64 0 (dread f64 %Reg5_f64)
  ClearBufferString();
  ASSERT_EQ(mirStmts2.size(), 1);
  mirStmts2.front()->Dump();
  dumpStr = GetBufferString();
  pattern = std::string("dassign %Reg3_f64 0 \\(dread f64 %Reg5_f64\\)") + MPLFEUTRegx::Any();
  EXPECT_EQ(MPLFEUTRegx::Match(dumpStr, pattern), true);
  RestoreCout();
}
}  // namespace maple