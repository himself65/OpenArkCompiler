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
#include "feir_type_infer.h"
#include "feir_builder.h"
#include "feir_var_type_scatter.h"
#include "feir_dfg.h"

namespace maple {
class FEIRTypeHelperTest : public testing::Test, public RedirectBuffer {
 public:
  FEIRTypeHelperTest() = default;
  ~FEIRTypeHelperTest() = default;
};

class FEIRTypeInferTest : public testing::Test, public RedirectBuffer {
 public:
  FEIRTypeInfer typeInfer;
  FEIRTypeInferTest()
      : typeInfer(kSrcLangJava) {}
  ~FEIRTypeInferTest() = default;
};

TEST_F(FEIRTypeHelperTest, MergeType_Parent) {
  UniqueFEIRType type0 = FEIRTypeHelper::CreateTypeByJavaName("Ljava/lang/Integer;", false, false);
  UniqueFEIRType type1 = FEIRTypeHelper::CreateTypeByJavaName("Ljava/lang/Number;", false, false);
  UniqueFEIRType type2 = FEIRTypeHelper::CreateTypeByJavaName("Ljava/lang/String;", false, false);
  UniqueFEIRType type3 = FEIRTypeHelper::CreateTypeByJavaName("[Ljava/lang/String;", false, false);
  UniqueFEIRType type4 = FEIRTypeHelper::CreateTypeByJavaName("I", false, false);
  UniqueFEIRType type5 = FEIRTypeHelper::CreateTypeByJavaName("J", false, false);
  UniqueFEIRType type6 = FEIRTypeHelper::CreateTypeByJavaName("LFEIRTypeHelperTest", false, false);
  UniqueFEIRType typeDefault = FEIRTypeHelper::CreateTypeByJavaName("Ljava/lang/Object;", false, false);
  UniqueFEIRType typeResult;
  FEIRTypeMergeHelper typeMergeHelper(typeDefault);
  // merge type0 and type1, expect Ljava/lang/Number;
  typeResult.reset(type0->Clone().release());
  EXPECT_EQ(typeMergeHelper.MergeType(typeResult, type1), true);
  EXPECT_EQ(typeResult->IsEqualTo(type1), true);

  // merge type1 and type0, expect Ljava/lang/Number;
  typeResult.reset(type1->Clone().release());
  EXPECT_EQ(typeMergeHelper.MergeType(typeResult, type0), true);
  EXPECT_EQ(typeResult->IsEqualTo(type1), true);

  // merge type0 and type2, expect Ljava/lang/Object;
  typeResult.reset(type0->Clone().release());
  EXPECT_EQ(typeMergeHelper.MergeType(typeResult, type2), true);
  EXPECT_EQ(typeResult->IsEqualTo(typeDefault), true);

  // merge type1 and type2, expect Ljava/lang/Object;
  typeResult.reset(type1->Clone().release());
  EXPECT_EQ(typeMergeHelper.MergeType(typeResult, type2), true);
  EXPECT_EQ(typeResult->IsEqualTo(typeDefault), true);

  // merge diff dim
  typeResult.reset(type2->Clone().release());
  EXPECT_EQ(typeMergeHelper.MergeType(typeResult, type3), false);

  // merge diff PrimType
  typeResult.reset(type4->Clone().release());
  EXPECT_EQ(typeMergeHelper.MergeType(typeResult, type5), false);

  // merge type0 and type6, expect Ljava/lang/Object;
  typeResult.reset(type0->Clone().release());
  EXPECT_EQ(typeMergeHelper.MergeType(typeResult, type6), true);
  EXPECT_EQ(typeResult->IsEqualTo(typeDefault), true);

  // merge type6 and type0, expect Ljava/lang/Object;
  typeResult.reset(type6->Clone().release());
  EXPECT_EQ(typeMergeHelper.MergeType(typeResult, type0), true);
  EXPECT_EQ(typeResult->IsEqualTo(typeDefault), true);
}

TEST_F(FEIRTypeHelperTest, MergeType_Child) {
  UniqueFEIRType type0 = FEIRTypeHelper::CreateTypeByJavaName("Ljava/lang/Integer;", false, false);
  UniqueFEIRType type1 = FEIRTypeHelper::CreateTypeByJavaName("Ljava/lang/Number;", false, false);
  UniqueFEIRType type2 = FEIRTypeHelper::CreateTypeByJavaName("Ljava/lang/String;", false, false);
  UniqueFEIRType typeDefault = FEIRTypeHelper::CreateTypeByJavaName("Ljava/lang/Object;", false, false);
  UniqueFEIRType typeResult;
  FEIRTypeMergeHelper typeMergeHelper(typeDefault);
  // merge type0 and type1, expect Ljava/lang/Integer;
  typeResult.reset(type0->Clone().release());
  EXPECT_EQ(typeMergeHelper.MergeType(typeResult, type1, false), true);
  EXPECT_EQ(typeResult->IsEqualTo(type0), true);

  // merge type1 and type0, expect Ljava/lang/Integer;
  typeResult.reset(type1->Clone().release());
  EXPECT_EQ(typeMergeHelper.MergeType(typeResult, type0, false), true);
  EXPECT_EQ(typeResult->IsEqualTo(type0), true);

  // merge type0 and type2, expect error
  typeResult.reset(type0->Clone().release());
  EXPECT_EQ(typeMergeHelper.MergeType(typeResult, type2, false), false);
}

//
// TypeInferTest: Test1
//   DFG: mplfe/doc/images/ut_cases/TypeInfer/Test1.dot
//   Image: mplfe/doc/images/ut_cases/TypeInfer/Test1.png
//   UseDef:
//     var1 -> var0
//   DefUse:
//     var0 -> var1
//
TEST_F(FEIRTypeInferTest, Test1) {
  typeInfer.Reset();
  std::map<UniqueFEIRVar*, std::set<UniqueFEIRVar*>> mapUseDef;
  std::map<UniqueFEIRVar*, std::set<UniqueFEIRVar*>> mapDefUse;
  UniqueFEIRVar var0 = FEIRBuilder::CreateVarReg(0, PTY_ref, false);
  UniqueFEIRVar var1 = FEIRBuilder::CreateVarReg(0, PTY_ref, false);
  UniqueFEIRType type0 = FEIRTypeHelper::CreateTypeByJavaName("Ljava/lang/String;", false, true);
  UniqueFEIRType type1 = FEIRTypeHelper::CreateTypeByJavaName("Ljava/lang/Object;", false, true);
  var0->SetType(type0->Clone());
  var1->SetType(type1->Clone());
  mapUseDef[&var1].insert(&var0);
  mapDefUse[&var0].insert(&var1);
  typeInfer.SetMapUseDef(&mapUseDef);
  typeInfer.SetMapDefUse(&mapDefUse);
  EXPECT_EQ(typeInfer.GetTypeForVarUse(&var1)->IsEqualTo(type1), true);
  typeInfer.ProcessVarDef(&var0);
  EXPECT_EQ(var0->GetKind(), FEIRVarKind::kFEIRVarTypeScatter);
  FEIRVarTypeScatter *ptrVar0 = static_cast<FEIRVarTypeScatter*>(var0.get());
  EXPECT_EQ(ptrVar0->GetType()->IsEqualTo(type0), true);
  EXPECT_EQ(ptrVar0->GetScatterTypes().size(), 1);
  EXPECT_NE(ptrVar0->GetScatterTypes().find(FEIRTypeKey(type1)), ptrVar0->GetScatterTypes().end());
}

//
// TypeInferTest: Test2
//   DFG: mplfe/doc/images/ut_cases/TypeInfer/Test2.dot
//   Image: mplfe/doc/images/ut_cases/TypeInfer/Test2.png
//   UseDef:
//     var1 -> var0
//     var3 -> var2
//   DefUse:
//     var0 -> var1
//     var2 -> var3
//
TEST_F(FEIRTypeInferTest, Test2) {
  typeInfer.Reset();
  std::map<UniqueFEIRVar*, std::set<UniqueFEIRVar*>> mapUseDef;
  std::map<UniqueFEIRVar*, std::set<UniqueFEIRVar*>> mapDefUse;
  UniqueFEIRVar var0 = FEIRBuilder::CreateVarReg(0, PTY_ref, false);
  UniqueFEIRVar var1 = FEIRBuilder::CreateVarReg(0, PTY_ref, false);
  UniqueFEIRVar var2 = FEIRBuilder::CreateVarReg(1, PTY_ref, false);
  UniqueFEIRVar var3 = FEIRBuilder::CreateVarReg(1, PTY_ref, false);
  UniqueFEIRType type0 = FEIRTypeHelper::CreateTypeByJavaName("Ljava/lang/String;", false, true);
  UniqueFEIRType type3 = FEIRTypeHelper::CreateTypeByJavaName("Ljava/lang/Object;", false, true);
  UniqueFEIRVarTrans trans0 = std::make_unique<FEIRVarTrans>(FEIRVarTransKind::kFEIRVarTransDirect, &var1);
  UniqueFEIRVarTrans trans1 = std::make_unique<FEIRVarTrans>(FEIRVarTransKind::kFEIRVarTransDirect, &var2);
  var1->SetTrans(std::move(trans1));
  var2->SetTrans(std::move(trans0));
  var0->SetType(type0->Clone());
  var3->SetType(type3->Clone());
  mapUseDef[&var1].insert(&var0);
  mapUseDef[&var3].insert(&var2);
  mapDefUse[&var0].insert(&var1);
  mapDefUse[&var2].insert(&var3);
  typeInfer.SetMapUseDef(&mapUseDef);
  typeInfer.SetMapDefUse(&mapDefUse);
  EXPECT_EQ(typeInfer.GetTypeForVarUse(&var1)->IsEqualTo(type3), true);
  typeInfer.Reset();
  EXPECT_EQ(typeInfer.GetTypeForVarUse(&var3)->IsEqualTo(type3), true);
  typeInfer.ProcessVarDef(&var0);
  typeInfer.ProcessVarDef(&var2);
  EXPECT_EQ(var0->GetKind(), FEIRVarKind::kFEIRVarTypeScatter);
  EXPECT_EQ(var2->GetKind(), FEIRVarKind::kFEIRVarReg);
}

//
// TypeInferTest: Test3
//   DFG: mplfe/doc/images/ut_cases/TypeInfer/Test3.dot
//   Image: mplfe/doc/images/ut_cases/TypeInfer/Test3.png
//   UseDef:
//     var1 -> {var0, var5}
//     var3 -> var2
//     var4 -> var2
TEST_F(FEIRTypeInferTest, Test3) {
  typeInfer.Reset();
  std::map<UniqueFEIRVar*, std::set<UniqueFEIRVar*>> mapUseDef;
  std::map<UniqueFEIRVar*, std::set<UniqueFEIRVar*>> mapDefUse;
  UniqueFEIRVar var0 = FEIRBuilder::CreateVarReg(0, PTY_ref, false);
  UniqueFEIRVar var1 = FEIRBuilder::CreateVarReg(0, PTY_ref, false);
  UniqueFEIRVar var2 = FEIRBuilder::CreateVarReg(1, PTY_ref, false);
  UniqueFEIRVar var3 = FEIRBuilder::CreateVarReg(1, PTY_ref, false);
  UniqueFEIRVar var4 = FEIRBuilder::CreateVarReg(1, PTY_ref, false);
  UniqueFEIRVar var5 = FEIRBuilder::CreateVarReg(0, PTY_ref, false);
  UniqueFEIRType type0 = FEIRTypeHelper::CreateTypeByJavaName("Ljava/lang/String;", false, true);
  UniqueFEIRVarTrans trans1_2 = std::make_unique<FEIRVarTrans>(FEIRVarTransKind::kFEIRVarTransDirect, &var1);
  UniqueFEIRVarTrans trans2_1 = std::make_unique<FEIRVarTrans>(FEIRVarTransKind::kFEIRVarTransDirect, &var2);
  UniqueFEIRVarTrans trans4_5 = std::make_unique<FEIRVarTrans>(FEIRVarTransKind::kFEIRVarTransDirect, &var4);
  UniqueFEIRVarTrans trans5_4 = std::make_unique<FEIRVarTrans>(FEIRVarTransKind::kFEIRVarTransDirect, &var5);
  var1->SetTrans(std::move(trans2_1));
  var2->SetTrans(std::move(trans1_2));
  var4->SetTrans(std::move(trans5_4));
  var5->SetTrans(std::move(trans4_5));
  var3->SetType(type0->Clone());
  mapUseDef[&var1].insert(&var0);
  mapUseDef[&var1].insert(&var5);
  mapUseDef[&var3].insert(&var2);
  mapUseDef[&var4].insert(&var2);
  FEIRDFG::CalculateDefUseByUseDef(mapDefUse, mapUseDef);
  typeInfer.SetMapUseDef(&mapUseDef);
  typeInfer.SetMapDefUse(&mapDefUse);
  EXPECT_EQ(typeInfer.GetTypeForVarUse(&var1)->IsEqualTo(type0), true);
  typeInfer.Reset();
  EXPECT_EQ(typeInfer.GetTypeForVarUse(&var3)->IsEqualTo(type0), true);
  typeInfer.Reset();
  EXPECT_EQ(typeInfer.GetTypeForVarUse(&var4)->IsEqualTo(type0), true);
  typeInfer.ProcessVarDef(&var0);
  typeInfer.ProcessVarDef(&var2);
  typeInfer.ProcessVarDef(&var5);
  EXPECT_EQ(var0->GetKind(), FEIRVarKind::kFEIRVarReg);
  EXPECT_EQ(var2->GetKind(), FEIRVarKind::kFEIRVarReg);
  EXPECT_EQ(var5->GetKind(), FEIRVarKind::kFEIRVarReg);
  EXPECT_EQ(var0->GetType()->IsEqualTo(type0), true);
  EXPECT_EQ(var2->GetType()->IsEqualTo(type0), true);
  EXPECT_EQ(var5->GetType()->IsEqualTo(type0), true);
}

//
// TypeInferTest: Test4
//   DFG: mplfe/doc/images/ut_cases/TypeInfer/Test4.dot
//   Image: mplfe/doc/images/ut_cases/TypeInfer/Test4.png
//   UseDef:
//     var1 -> var0
//     var3 -> var2
//
TEST_F(FEIRTypeInferTest, Test4) {
  typeInfer.Reset();
  std::map<UniqueFEIRVar*, std::set<UniqueFEIRVar*>> mapUseDef;
  std::map<UniqueFEIRVar*, std::set<UniqueFEIRVar*>> mapDefUse;
  UniqueFEIRVar var0 = FEIRBuilder::CreateVarReg(0, PTY_ref, false);
  UniqueFEIRVar var1 = FEIRBuilder::CreateVarReg(0, PTY_ref, false);
  UniqueFEIRVar var2 = FEIRBuilder::CreateVarReg(1, PTY_ref, false);
  UniqueFEIRVar var3 = FEIRBuilder::CreateVarReg(1, PTY_ref, false);
  UniqueFEIRType type0 = FEIRTypeHelper::CreateTypeByJavaName("Ljava/lang/Object;", false, true);
  UniqueFEIRType type1 = FEIRTypeHelper::CreateTypeByJavaName("[Ljava/lang/Object;", false, true);
  UniqueFEIRVarTrans trans1_2 = std::make_unique<FEIRVarTrans>(FEIRVarTransKind::kFEIRVarTransArrayDimDecr, &var1);
  UniqueFEIRVarTrans trans2_1 = std::make_unique<FEIRVarTrans>(FEIRVarTransKind::kFEIRVarTransArrayDimIncr, &var2);
  var3->SetType(type0->Clone());
  var1->SetTrans(std::move(trans2_1));
  var2->SetTrans(std::move(trans1_2));
  mapUseDef[&var1].insert(&var0);
  mapUseDef[&var3].insert(&var2);
  FEIRDFG::CalculateDefUseByUseDef(mapDefUse, mapUseDef);
  typeInfer.SetMapUseDef(&mapUseDef);
  typeInfer.SetMapDefUse(&mapDefUse);
  EXPECT_EQ(typeInfer.GetTypeForVarUse(&var1)->IsEqualTo(type1), true);
  typeInfer.ProcessVarDef(&var0);
  typeInfer.ProcessVarDef(&var2);
  EXPECT_EQ(var0->GetType()->IsEqualTo(type1), true);
  EXPECT_EQ(var2->GetType()->IsEqualTo(type0), true);
}
}  // namespace maple
