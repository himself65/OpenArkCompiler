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
#include "fe_manager.h"
#include "fe_type_manager.h"
#include "redirect_buffer.h"

namespace maple {
class FETypeManagerTest : public testing::Test, public RedirectBuffer {
 public:
  FETypeManagerTest() = default;
  ~FETypeManagerTest() = default;
};

TEST_F(FETypeManagerTest, GetClassOrInterfaceType) {
  MIRStructType *structType = FEManager::GetTypeManager().GetClassOrInterfaceType("Ljava_2Flang_2FObject_3B");
  EXPECT_NE(structType, nullptr);
  if (structType != nullptr) {
    std::string mplName = structType->GetCompactMplTypeName();
    EXPECT_EQ(mplName, "Ljava_2Flang_2FObject_3B");
  }
  MIRStructType *structTypeUnknown = FEManager::GetTypeManager().GetClassOrInterfaceType("LUnknown");
  EXPECT_EQ(structTypeUnknown, nullptr);
}

TEST_F(FETypeManagerTest, GetClassOrInterfaceTypeFlag) {
  FETypeFlag flag = FEManager::GetTypeManager().GetClassOrInterfaceTypeFlag("Ljava_2Flang_2FObject_3B");
  EXPECT_EQ(flag, FETypeFlag::kSrcMpltSys);
  FETypeFlag flagUnknown = FEManager::GetTypeManager().GetClassOrInterfaceTypeFlag("LUnknown");
  EXPECT_EQ(flagUnknown, FETypeFlag::kDefault);
}

TEST_F(FETypeManagerTest, CreateClassOrInterfaceType) {
  MIRStructType *structType1 = FEManager::GetTypeManager().CreateClassOrInterfaceType("LNewClass", false,
                                                                                      FETypeFlag::kSrcInput);
  EXPECT_NE(structType1, nullptr);
  if (structType1 != nullptr) {
    std::string mplName = structType1->GetCompactMplTypeName();
    EXPECT_EQ(mplName, "LNewClass");
    EXPECT_EQ(structType1->GetKind(), kTypeClassIncomplete);
  }
  MIRStructType *structType2 = FEManager::GetTypeManager().CreateClassOrInterfaceType("LNewInterface", true,
                                                                                      FETypeFlag::kSrcInput);
  EXPECT_NE(structType2, nullptr);
  if (structType2 != nullptr) {
    std::string mplName = structType2->GetCompactMplTypeName();
    EXPECT_EQ(mplName, "LNewInterface");
    EXPECT_EQ(structType2->GetKind(), kTypeInterfaceIncomplete);
  }
}

TEST_F(FETypeManagerTest, GetOrCreateClassOrInterfaceType) {
  bool isCreate = false;
  MIRStructType *structType1 = FEManager::GetTypeManager().GetOrCreateClassOrInterfaceType("Ljava_2Flang_2FObject_3B",
                                                                                           false,
                                                                                           FETypeFlag::kSrcUnknown,
                                                                                           isCreate);
  EXPECT_EQ(isCreate, false);
  ASSERT_NE(structType1, nullptr);
  std::string mplName1 = structType1->GetCompactMplTypeName();
  EXPECT_EQ(mplName1, "Ljava_2Flang_2FObject_3B");
  EXPECT_EQ(structType1->GetKind(), kTypeClass);
  MIRStructType *structType2 = FEManager::GetTypeManager().GetOrCreateClassOrInterfaceType("LNewClass2",
                                                                                           false,
                                                                                           FETypeFlag::kSrcUnknown,
                                                                                           isCreate);
  EXPECT_EQ(isCreate, true);
  ASSERT_NE(structType2, nullptr);
  std::string mplName2 = structType2->GetCompactMplTypeName();
  EXPECT_EQ(mplName2, "LNewClass2");
  EXPECT_EQ(structType2->GetKind(), kTypeClassIncomplete);
}

TEST_F(FETypeManagerTest, GetOrCreateClassOrInterfacePtrType) {
  bool isCreate = false;
  MIRType *ptrType = FEManager::GetTypeManager().GetOrCreateClassOrInterfacePtrType("Ljava_2Flang_2FObject_3B",
                                                                                    false, FETypeFlag::kSrcUnknown,
                                                                                    isCreate);
  ASSERT_NE(ptrType, nullptr);
  RedirectCout();
  ptrType->Dump(0);
  EXPECT_EQ(GetBufferString(), "<* <$Ljava_2Flang_2FObject_3B>>");
  RestoreCout();
}

TEST_F(FETypeManagerTest, GetOrCreateTypeFromName) {
  // Prim Type
  MIRType *typePrim = FEManager::GetTypeManager().GetOrCreateTypeFromName("I",
                                                                          FETypeFlag::kSrcUnknown, false);
  ASSERT_NE(typePrim, nullptr);
  EXPECT_EQ(typePrim, GlobalTables::GetTypeTable().GetInt32());
  // Object Type
  MIRType *typeObject = FEManager::GetTypeManager().GetOrCreateTypeFromName("Ljava_2Flang_2FObject_3B",
                                                                            FETypeFlag::kSrcUnknown, true);
  ASSERT_NE(typeObject, nullptr);
  RedirectCout();
  typeObject->Dump(0);
  EXPECT_EQ(GetBufferString(), "<* <$Ljava_2Flang_2FObject_3B>>");
  RestoreCout();
  // Array Type
  MIRType *typeArray = FEManager::GetTypeManager().GetOrCreateTypeFromName("ALjava_2Flang_2FObject_3B",
                                                                           FETypeFlag::kSrcUnknown, true);
  ASSERT_NE(typeArray, nullptr);
  RedirectCout();
  typeArray->Dump(0);
  EXPECT_EQ(GetBufferString(), "<* <[] <* <$Ljava_2Flang_2FObject_3B>>>>");
  RestoreCout();
  // Array Type2
  MIRType *typeArray2 = FEManager::GetTypeManager().GetOrCreateTypeFromName("AALjava_2Flang_2FObject_3B",
                                                                            FETypeFlag::kSrcUnknown, true);
  ASSERT_NE(typeArray2, nullptr);
  RedirectCout();
  typeArray2->Dump(0);
  EXPECT_EQ(GetBufferString(), "<* <[] <* <[] <* <$Ljava_2Flang_2FObject_3B>>>>>>");
  RestoreCout();
}
}  // namespace maple