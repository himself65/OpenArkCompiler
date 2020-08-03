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
#include "redirect_buffer.h"
#include "global_tables.h"
#include "mir_module.h"
#include "feir_type_helper.h"

namespace maple {
class FEIRTypeHelperTest : public testing::Test, public RedirectBuffer {
 public:
  FEIRTypeHelperTest() = default;
  ~FEIRTypeHelperTest() = default;
};

TEST_F(FEIRTypeHelperTest, CreateTypeByPrimType_i32) {
  std::unique_ptr<FEIRType> type1 = FEIRTypeHelper::CreateTypeByPrimType(PTY_i32, 0, false);
  std::unique_ptr<FEIRType> type2 = FEIRTypeHelper::CreateTypeByPrimType(PTY_i32, 0, true);
  RedirectCout();
  type1->GenerateMIRType()->Dump(0);
  EXPECT_EQ(GetBufferString(), "i32");
  ClearBufferString();
  type2->GenerateMIRType()->Dump(0);
  EXPECT_EQ(GetBufferString(), "<* i32>");
  RestoreCout();
}

TEST_F(FEIRTypeHelperTest, CreateTypeByJavaName_J) {
  std::unique_ptr<FEIRType> type1 = FEIRTypeHelper::CreateTypeByJavaName("J", false, false);
  std::unique_ptr<FEIRType> type2 = FEIRTypeHelper::CreateTypeByJavaName("J", false, true);
  RedirectCout();
  type1->GenerateMIRType()->Dump(0);
  EXPECT_EQ(GetBufferString(), "i64");
  ClearBufferString();
  type2->GenerateMIRType()->Dump(0);
  EXPECT_EQ(GetBufferString(), "<* i64>");
  RestoreCout();
}

TEST_F(FEIRTypeHelperTest, CreateTypeByJavaName_PrimType) {
  RedirectCout();
  FEIRTypeHelper::CreateTypeByJavaName("I", false, false)->GenerateMIRType()->Dump(0);
  EXPECT_EQ(GetBufferString(), "i32");
  ClearBufferString();
  FEIRTypeHelper::CreateTypeByJavaName("J", false, false)->GenerateMIRType()->Dump(0);
  EXPECT_EQ(GetBufferString(), "i64");
  ClearBufferString();
  FEIRTypeHelper::CreateTypeByJavaName("F", false, false)->GenerateMIRType()->Dump(0);
  EXPECT_EQ(GetBufferString(), "f32");
  ClearBufferString();
  FEIRTypeHelper::CreateTypeByJavaName("D", false, false)->GenerateMIRType()->Dump(0);
  EXPECT_EQ(GetBufferString(), "f64");
  ClearBufferString();
  FEIRTypeHelper::CreateTypeByJavaName("Z", false, false)->GenerateMIRType()->Dump(0);
  EXPECT_EQ(GetBufferString(), "u1");
  ClearBufferString();
  FEIRTypeHelper::CreateTypeByJavaName("B", false, false)->GenerateMIRType()->Dump(0);
  EXPECT_EQ(GetBufferString(), "i8");
  ClearBufferString();
  FEIRTypeHelper::CreateTypeByJavaName("S", false, false)->GenerateMIRType()->Dump(0);
  EXPECT_EQ(GetBufferString(), "i16");
  ClearBufferString();
  FEIRTypeHelper::CreateTypeByJavaName("C", false, false)->GenerateMIRType()->Dump(0);
  EXPECT_EQ(GetBufferString(), "u16");
  ClearBufferString();
  FEIRTypeHelper::CreateTypeByJavaName("V", false, false)->GenerateMIRType()->Dump(0);
  EXPECT_EQ(GetBufferString(), "void");
  RestoreCout();
}

TEST_F(FEIRTypeHelperTest, CreateTypeByJavaName_Object) {
  std::unique_ptr<FEIRType> type1 = FEIRTypeHelper::CreateTypeByJavaName("Ljava/lang/Object;", false, false);
  std::unique_ptr<FEIRType> type2 = FEIRTypeHelper::CreateTypeByJavaName("Ljava/lang/Object;", false, true);
  std::unique_ptr<FEIRType> type3 = FEIRTypeHelper::CreateTypeByJavaName("[Ljava/lang/Object;", false, false);
  std::unique_ptr<FEIRType> type4 = FEIRTypeHelper::CreateTypeByJavaName("[Ljava/lang/Object;", false, true);
  RedirectCout();
  type1->GenerateMIRType()->Dump(0);
  EXPECT_EQ(GetBufferString(), "<$Ljava_2Flang_2FObject_3B>");
  ClearBufferString();
  type2->GenerateMIRType()->Dump(0);
  EXPECT_EQ(GetBufferString(), "<* <$Ljava_2Flang_2FObject_3B>>");
  ClearBufferString();
  type3->GenerateMIRType()->Dump(0);
  EXPECT_EQ(GetBufferString(), "<[] <* <$Ljava_2Flang_2FObject_3B>>>");
  ClearBufferString();
  type4->GenerateMIRType()->Dump(0);
  EXPECT_EQ(GetBufferString(), "<* <[] <* <$Ljava_2Flang_2FObject_3B>>>>");
  RestoreCout();
}

TEST_F(FEIRTypeHelperTest, CreateTypeByJavaName_Undefined) {
  std::unique_ptr<FEIRType> type1 = FEIRTypeHelper::CreateTypeByJavaName("LUndefine4FEIRTypeHelper;", false, false);
  std::unique_ptr<FEIRType> type2 = FEIRTypeHelper::CreateTypeByJavaName("LUndefine4FEIRTypeHelper;", false, true);
  RedirectCout();
  type1->GenerateMIRType()->Dump(0);
  EXPECT_EQ(GetBufferString(), "<$LUndefine4FEIRTypeHelper_3B>");
  ClearBufferString();
  type2->GenerateMIRType()->Dump(0);
  EXPECT_EQ(GetBufferString(), "<* <$LUndefine4FEIRTypeHelper_3B>>");
  RestoreCout();
}

TEST_F(FEIRTypeHelperTest, CreateTypeByDimIncr) {
  UniqueFEIRType type1 = FEIRTypeHelper::CreateTypeByPrimType(PTY_i32, 0, false);
  UniqueFEIRType type1A = FEIRTypeHelper::CreateTypeByDimIncr(type1, 1);
  UniqueFEIRType type1AP = FEIRTypeHelper::CreateTypeByDimIncr(type1, 1, true);
  UniqueFEIRType type2 = FEIRTypeHelper::CreateTypeByPrimType(PTY_i32, 1, true);
  UniqueFEIRType type2A = FEIRTypeHelper::CreateTypeByDimIncr(type2, 2);
  UniqueFEIRType type2AP = FEIRTypeHelper::CreateTypeByDimIncr(type2, 2, true);
  RedirectCout();
  type1->GenerateMIRType()->Dump(0);
  EXPECT_EQ(GetBufferString(), "i32");
  ClearBufferString();
  type1A->GenerateMIRType()->Dump(0);
  EXPECT_EQ(GetBufferString(), "<[] i32>");
  ClearBufferString();
  type1AP->GenerateMIRType()->Dump(0);
  EXPECT_EQ(GetBufferString(), "<* <[] i32>>");
  ClearBufferString();
  type2->GenerateMIRType()->Dump(0);
  EXPECT_EQ(GetBufferString(), "<* <[] i32>>");
  ClearBufferString();
  type2A->GenerateMIRType()->Dump(0);
  EXPECT_EQ(GetBufferString(), "<* <[] <* <[] <* <[] i32>>>>>>");
  ClearBufferString();
  type2AP->GenerateMIRType()->Dump(0);
  EXPECT_EQ(GetBufferString(), "<* <[] <* <[] <* <[] i32>>>>>>");
  RestoreCout();
}

TEST_F(FEIRTypeHelperTest, CreateTypeByDimDecr) {
  UniqueFEIRType type2AP = FEIRTypeHelper::CreateTypeByPrimType(PTY_i32, 2, true);
  UniqueFEIRType type1AP = FEIRTypeHelper::CreateTypeByDimDecr(type2AP, 1);
  UniqueFEIRType type0 = FEIRTypeHelper::CreateTypeByDimDecr(type2AP, 2);
  RedirectCout();
  type2AP->GenerateMIRType()->Dump(0);
  EXPECT_EQ(GetBufferString(), "<* <[] <* <[] i32>>>>");
  ClearBufferString();
  type1AP->GenerateMIRType()->Dump(0);
  EXPECT_EQ(GetBufferString(), "<* <[] i32>>");
  ClearBufferString();
  type0->GenerateMIRType()->Dump(0);
  EXPECT_EQ(GetBufferString(), "i32");
  RestoreCout();
}

TEST_F(FEIRTypeHelperTest, CreateTypeByGetAddress) {
  UniqueFEIRType type0 = FEIRTypeHelper::CreateTypeByPrimType(PTY_i32, 0, false);
  UniqueFEIRType type0P = FEIRTypeHelper::CreateTypeByGetAddress(type0);
  RedirectCout();
  type0->GenerateMIRType()->Dump(0);
  EXPECT_EQ(GetBufferString(), "i32");
  ClearBufferString();
  type0P->GenerateMIRType()->Dump(0);
  EXPECT_EQ(GetBufferString(), "<* i32>");
  RestoreCout();
}

TEST_F(FEIRTypeHelperTest, CreateTypeByDereferrence) {
  UniqueFEIRType type0P = FEIRTypeHelper::CreateTypeByPrimType(PTY_i32, 0, true);
  UniqueFEIRType type0 = FEIRTypeHelper::CreateTypeByDereferrence(type0P);
  RedirectCout();
  type0P->GenerateMIRType()->Dump(0);
  EXPECT_EQ(GetBufferString(), "<* i32>");
  ClearBufferString();
  type0->GenerateMIRType()->Dump(0);
  EXPECT_EQ(GetBufferString(), "i32");
  RestoreCout();
}
}  // namespace maple