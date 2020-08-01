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
#include <unordered_set>
#include "feir_type.h"
#include "feir_type_helper.h"
#include "redirect_buffer.h"
#include "global_tables.h"
#include "mir_module.h"

namespace maple {
class FEIRTypeDefaultTest : public testing::Test, public RedirectBuffer {
 public:
  FEIRTypeDefaultTest() = default;
  ~FEIRTypeDefaultTest() = default;
};

TEST(FEIRTypeDefault, IsScalarPrimType) {
  EXPECT_EQ(FEIRTypeDefault::IsScalarPrimType(PTY_i32), true);
  EXPECT_EQ(FEIRTypeDefault::IsScalarPrimType(PTY_ref), false);
}

TEST_F(FEIRTypeDefaultTest, FEIRTypeDefaultTest_Ref_Array_Precise_Valid) {
  GStrIdx idxZero = GStrIdx(0);
  GStrIdx idxNonZero = GStrIdx(100);

  FEIRTypeDefault typeScalar(PTY_i32, idxZero, 0);
  EXPECT_EQ(typeScalar.IsRef(), false);
  EXPECT_EQ(typeScalar.IsArray(), false);
  EXPECT_EQ(typeScalar.IsPrecise(), true);
  EXPECT_EQ(typeScalar.IsValid(), true);

  FEIRTypeDefault typeScalarArray(PTY_i32, idxZero, 1);
  EXPECT_EQ(typeScalarArray.IsRef(), true);
  EXPECT_EQ(typeScalarArray.IsArray(), true);
  EXPECT_EQ(typeScalarArray.IsPrecise(), true);
  EXPECT_EQ(typeScalarArray.IsValid(), true);

  FEIRTypeDefault typeRefNP(PTY_ref, idxZero, 0);
  EXPECT_EQ(typeRefNP.IsRef(), true);
  EXPECT_EQ(typeRefNP.IsArray(), false);
  EXPECT_EQ(typeRefNP.IsPrecise(), false);
  EXPECT_EQ(typeRefNP.IsValid(), true);

  FEIRTypeDefault typeRefNPArray(PTY_ref, idxZero, 1);
  EXPECT_EQ(typeRefNPArray.IsRef(), true);
  EXPECT_EQ(typeRefNPArray.IsArray(), true);
  EXPECT_EQ(typeRefNPArray.IsPrecise(), false);
  EXPECT_EQ(typeRefNPArray.IsValid(), true);

  FEIRTypeDefault typeRef(PTY_ref, idxNonZero, 0);
  EXPECT_EQ(typeRef.IsRef(), true);
  EXPECT_EQ(typeRef.IsArray(), false);
  EXPECT_EQ(typeRef.IsPrecise(), true);
  EXPECT_EQ(typeRef.IsValid(), true);

  FEIRTypeDefault typeRefArray(PTY_ref, idxNonZero, 1);
  EXPECT_EQ(typeRefArray.IsRef(), true);
  EXPECT_EQ(typeRefArray.IsArray(), true);
  EXPECT_EQ(typeRefArray.IsPrecise(), true);
  EXPECT_EQ(typeRefArray.IsValid(), true);

  FEIRTypeDefault typeInvalid(PTY_i32, idxNonZero, 0);
  EXPECT_EQ(typeInvalid.IsValid(), false);

  FEIRTypeDefault typeInvalidArray(PTY_i32, idxNonZero, 1);
  EXPECT_EQ(typeInvalidArray.IsValid(), false);
}

TEST_F(FEIRTypeDefaultTest, FEIRTypeDefaultTest_default) {
  FEIRTypeDefault type;
  RedirectCout();
  type.GenerateMIRType(kSrcLangJava, false)->Dump(0);
  EXPECT_EQ(GetBufferString(), "void");
  ClearBufferString();
  type.GenerateMIRType(kSrcLangJava, true)->Dump(0);
  EXPECT_EQ(GetBufferString(), "void");
  RestoreCout();
}

TEST_F(FEIRTypeDefaultTest, FEIRTypeDefaultTest_void) {
  FEIRTypeDefault type(PTY_void);
  RedirectCout();
  type.GenerateMIRType(kSrcLangJava, false)->Dump(0);
  EXPECT_EQ(GetBufferString(), "void");
  ClearBufferString();
  type.GenerateMIRType(kSrcLangJava, true)->Dump(0);
  EXPECT_EQ(GetBufferString(), "void");
  RestoreCout();
}

TEST_F(FEIRTypeDefaultTest, FEIRTypeDefaultTest_ref) {
  FEIRTypeDefault type(PTY_ref);
  RedirectCout();
  type.GenerateMIRType(kSrcLangJava, false)->Dump(0);
  EXPECT_EQ(GetBufferString(), "ref");
  ClearBufferString();
  type.GenerateMIRType(kSrcLangJava, true)->Dump(0);
  EXPECT_EQ(GetBufferString(), "ref");
  RestoreCout();
}

TEST_F(FEIRTypeDefaultTest, FEIRTypeDefaultTest_refArray) {
  FEIRTypeDefault type(PTY_ref, GStrIdx(0), 1);
  RedirectCout();
  type.GenerateMIRType(kSrcLangJava, false)->Dump(0);
  EXPECT_EQ(GetBufferString(), "<[] ref>");
  ClearBufferString();
  type.GenerateMIRType(kSrcLangJava, true)->Dump(0);
  EXPECT_EQ(GetBufferString(), "<* <[] ref>>");
  RestoreCout();
}

TEST_F(FEIRTypeDefaultTest, FEIRTypeDefaultTest_Object) {
  GStrIdx typeNameIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName("Ljava_2Flang_2FObject_3B");
  FEIRTypeDefault type(PTY_ref, typeNameIdx, 0);
  RedirectCout();
  type.GenerateMIRType(kSrcLangJava, false)->Dump(0);
  EXPECT_EQ(GetBufferString(), "<$Ljava_2Flang_2FObject_3B>");
  ClearBufferString();
  type.GenerateMIRType(kSrcLangJava, true)->Dump(0);
  EXPECT_EQ(GetBufferString(), "<* <$Ljava_2Flang_2FObject_3B>>");
  RestoreCout();
}

TEST_F(FEIRTypeDefaultTest, LoadFromJavaTypeName_V) {
  FEIRTypeDefault type;
  type.LoadFromJavaTypeName("V");
  RedirectCout();
  type.GenerateMIRType(kSrcLangJava, false)->Dump(0);
  EXPECT_EQ(GetBufferString(), "void");
  ClearBufferString();
  type.GenerateMIRType(kSrcLangJava, true)->Dump(0);
  EXPECT_EQ(GetBufferString(), "void");
  RestoreCout();
}

TEST_F(FEIRTypeDefaultTest, LoadFromJavaTypeName_I) {
  FEIRTypeDefault type;
  type.LoadFromJavaTypeName("I");
  RedirectCout();
  type.GenerateMIRType(kSrcLangJava, false)->Dump(0);
  EXPECT_EQ(GetBufferString(), "i32");
  ClearBufferString();
  type.GenerateMIRType(kSrcLangJava, true)->Dump(0);
  EXPECT_EQ(GetBufferString(), "i32");
  RestoreCout();
}

TEST_F(FEIRTypeDefaultTest, LoadFromJavaTypeName_J) {
  FEIRTypeDefault type;
  type.LoadFromJavaTypeName("J");
  RedirectCout();
  type.GenerateMIRType(kSrcLangJava, false)->Dump(0);
  EXPECT_EQ(GetBufferString(), "i64");
  ClearBufferString();
  type.GenerateMIRType(kSrcLangJava, true)->Dump(0);
  EXPECT_EQ(GetBufferString(), "i64");
  RestoreCout();
}

TEST_F(FEIRTypeDefaultTest, LoadFromJavaTypeName_F) {
  FEIRTypeDefault type;
  type.LoadFromJavaTypeName("F");
  RedirectCout();
  type.GenerateMIRType(kSrcLangJava, false)->Dump(0);
  EXPECT_EQ(GetBufferString(), "f32");
  ClearBufferString();
  type.GenerateMIRType(kSrcLangJava, true)->Dump(0);
  EXPECT_EQ(GetBufferString(), "f32");
  RestoreCout();
}

TEST_F(FEIRTypeDefaultTest, LoadFromJavaTypeName_D) {
  FEIRTypeDefault type;
  type.LoadFromJavaTypeName("D");
  RedirectCout();
  type.GenerateMIRType(kSrcLangJava, false)->Dump(0);
  EXPECT_EQ(GetBufferString(), "f64");
  ClearBufferString();
  type.GenerateMIRType(kSrcLangJava, true)->Dump(0);
  EXPECT_EQ(GetBufferString(), "f64");
  RestoreCout();
}

TEST_F(FEIRTypeDefaultTest, LoadFromJavaTypeName_Z) {
  FEIRTypeDefault type;
  type.LoadFromJavaTypeName("Z");
  RedirectCout();
  type.GenerateMIRType(kSrcLangJava, false)->Dump(0);
  EXPECT_EQ(GetBufferString(), "u1");
  ClearBufferString();
  type.GenerateMIRType(kSrcLangJava, true)->Dump(0);
  EXPECT_EQ(GetBufferString(), "u1");
  RestoreCout();
}

TEST_F(FEIRTypeDefaultTest, LoadFromJavaTypeName_B) {
  FEIRTypeDefault type;
  type.LoadFromJavaTypeName("B");
  RedirectCout();
  type.GenerateMIRType(kSrcLangJava, false)->Dump(0);
  EXPECT_EQ(GetBufferString(), "i8");
  ClearBufferString();
  type.GenerateMIRType(kSrcLangJava, true)->Dump(0);
  EXPECT_EQ(GetBufferString(), "i8");
  RestoreCout();
}

TEST_F(FEIRTypeDefaultTest, LoadFromJavaTypeName_C) {
  FEIRTypeDefault type;
  type.LoadFromJavaTypeName("C");
  RedirectCout();
  type.GenerateMIRType(kSrcLangJava, false)->Dump(0);
  EXPECT_EQ(GetBufferString(), "u16");
  ClearBufferString();
  type.GenerateMIRType(kSrcLangJava, true)->Dump(0);
  EXPECT_EQ(GetBufferString(), "u16");
  RestoreCout();
}

TEST_F(FEIRTypeDefaultTest, LoadFromJavaTypeName_S) {
  FEIRTypeDefault type;
  type.LoadFromJavaTypeName("S");
  RedirectCout();
  type.GenerateMIRType(kSrcLangJava, false)->Dump(0);
  EXPECT_EQ(GetBufferString(), "i16");
  ClearBufferString();
  type.GenerateMIRType(kSrcLangJava, true)->Dump(0);
  EXPECT_EQ(GetBufferString(), "i16");
  RestoreCout();
}

TEST_F(FEIRTypeDefaultTest, LoadFromJavaTypeName_Object) {
  FEIRTypeDefault type;
  type.LoadFromJavaTypeName("Ljava_2Flang_2FObject_3B");
  RedirectCout();
  type.GenerateMIRType(kSrcLangJava, false)->Dump(0);
  EXPECT_EQ(GetBufferString(), "<$Ljava_2Flang_2FObject_3B>");
  ClearBufferString();
  type.GenerateMIRType(kSrcLangJava, true)->Dump(0);
  EXPECT_EQ(GetBufferString(), "<* <$Ljava_2Flang_2FObject_3B>>");
  RestoreCout();
}

TEST_F(FEIRTypeDefaultTest, LoadFromJavaTypeName_AI) {
  FEIRTypeDefault type;
  type.LoadFromJavaTypeName("AI");
  RedirectCout();
  type.GenerateMIRType(kSrcLangJava, false)->Dump(0);
  EXPECT_EQ(GetBufferString(), "<[] i32>");
  ClearBufferString();
  type.GenerateMIRType(kSrcLangJava, true)->Dump(0);
  EXPECT_EQ(GetBufferString(), "<* <[] i32>>");
  ClearBufferString();
  type.GenerateMIRTypeAuto(kSrcLangJava)->Dump(0);
  EXPECT_EQ(GetBufferString(), "<* <[] i32>>");
  RestoreCout();
}

TEST_F(FEIRTypeDefaultTest, UseFEIRTypeKey) {
  std::unordered_set<FEIRTypeKey, FEIRTypeKeyHash> testSet;
  UniqueFEIRType type1 = FEIRTypeHelper::CreateTypeByJavaName("Ljava/lang/Object;", false, false);
  UniqueFEIRType type2 = type1->Clone();
  UniqueFEIRType type3 = FEIRTypeHelper::CreateTypeByJavaName("Ljava/lang/Object;", false, true);
  UniqueFEIRType type4 = type3->Clone();
  testSet.insert(FEIRTypeKey(type1));
  testSet.insert(FEIRTypeKey(type2));
  testSet.insert(FEIRTypeKey(type3));
  testSet.insert(FEIRTypeKey(type4));
  EXPECT_EQ(testSet.size(), 2);
  EXPECT_NE(testSet.find(FEIRTypeKey(type1)), testSet.end());
  EXPECT_NE(testSet.find(FEIRTypeKey(type2)), testSet.end());
  EXPECT_NE(testSet.find(FEIRTypeKey(type3)), testSet.end());
  EXPECT_NE(testSet.find(FEIRTypeKey(type4)), testSet.end());
}
}  // namespace maple