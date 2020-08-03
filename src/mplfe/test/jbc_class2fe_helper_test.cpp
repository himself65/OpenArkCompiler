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
#include <climits>
#define protected public
#define private public
#include "jbc_class2fe_helper.h"
#undef protected
#undef private
#include "redirect_buffer.h"
#include "fe_options.h"

namespace maple {
class JBCClass2FEHelperTest : public testing::Test, public RedirectBuffer {
 public:
  JBCClass2FEHelperTest()
      : allocator(mp) {}
  ~JBCClass2FEHelperTest() = default;

  static void SetUpTestCase() {
    mp = memPoolCtrler.NewMemPool("MemPool for JBCClass2FEHelperTest");
  }

  static void TearDownTestCase() {
    memPoolCtrler.DeleteMemPool(mp);
    mp = nullptr;
  }

 protected:
  static MemPool *mp;
  MapleAllocator allocator;
};
MemPool *JBCClass2FEHelperTest::mp = nullptr;

class JBCClassField2FEHelperTest : public testing::Test, public RedirectBuffer {
 public:
  JBCClassField2FEHelperTest()
      : allocator(mp) {}
  ~JBCClassField2FEHelperTest() = default;

  static void SetUpTestCase() {
    mp = memPoolCtrler.NewMemPool("MemPool for JBCClassField2FEHelperTest");
  }

  static void TearDownTestCase() {
    memPoolCtrler.DeleteMemPool(mp);
    mp = nullptr;
  }

 protected:
  static MemPool *mp;
  MapleAllocator allocator;
};
MemPool *JBCClassField2FEHelperTest::mp = nullptr;

class MockJBCClass : public jbc::JBCClass {
 public:
  explicit MockJBCClass(MapleAllocator &allocator) : jbc::JBCClass(allocator) {}
  ~MockJBCClass() = default;
  MOCK_CONST_METHOD0(GetClassNameMpl, std::string());
  MOCK_CONST_METHOD0(GetClassNameOrin, std::string());
  MOCK_CONST_METHOD0(GetSuperClassName, std::string());
  MOCK_CONST_METHOD0(GetInterfaceNames, std::vector<std::string>());
};

class MockJBCClassField : public jbc::JBCClassField {
 public:
  MockJBCClassField(MapleAllocator &allocator, const jbc::JBCClass &klass)
      : jbc::JBCClassField(allocator, klass) {}
  ~MockJBCClassField() = default;
  MOCK_CONST_METHOD0(GetAccessFlag, uint16());
  MOCK_CONST_METHOD0(IsStatic, bool());
  MOCK_CONST_METHOD1(GetName, std::string(const jbc::JBCConstPool&));
  MOCK_CONST_METHOD1(GetDescription, std::string(const jbc::JBCConstPool&));
};

class MockJBCClass2FEHelper : public JBCClass2FEHelper {
 public:
  MockJBCClass2FEHelper(MapleAllocator &allocator, jbc::JBCClass &klassIn)
      : JBCClass2FEHelper(allocator, klassIn) {}
  ~MockJBCClass2FEHelper() = default;
  MOCK_CONST_METHOD0(IsStaticFieldProguard, bool());
};

TEST_F(JBCClass2FEHelperTest, PreProcessDecl_SameName) {
  MockJBCClass *klass = mp->New<MockJBCClass>(allocator);
  JBCClass2FEHelper helper(allocator, *klass);
  EXPECT_CALL(*klass, GetClassNameMpl())
      .Times(1000)
      .WillRepeatedly(::testing::Return("Ljava_2Flang_2FObject_3B"));
  EXPECT_CALL(*klass, GetClassNameOrin())
      .Times(1000)
      .WillRepeatedly(::testing::Return("Ljava/lang/Object;"));
  helper.PreProcessDecl();
  EXPECT_EQ(helper.IsSkipped(), true);
}

TEST_F(JBCClass2FEHelperTest, PreProcessDecl_NotSameName_Class) {
  MockJBCClass *klass = mp->New<MockJBCClass>(allocator);
  JBCClass2FEHelper helper(allocator, *klass);
  EXPECT_CALL(*klass, GetClassNameMpl())
      .Times(1000)
      .WillRepeatedly(::testing::Return("LNewClass1InJBCClass2FEHelperTest_3B"));
  EXPECT_CALL(*klass, GetClassNameOrin())
      .Times(1000)
      .WillRepeatedly(::testing::Return("LNewClass1InJBCClass2FEHelperTest;"));
  helper.PreProcessDecl();
  EXPECT_EQ(helper.IsSkipped(), false);
}

TEST_F(JBCClass2FEHelperTest, PreProcessDecl_NotSameName_Interface) {
  MockJBCClass *klass = mp->New<MockJBCClass>(allocator);
  JBCClass2FEHelper helper(allocator, *klass);
  EXPECT_CALL(*klass, GetClassNameMpl())
      .Times(1000)
      .WillRepeatedly(::testing::Return("LNewInterface1InJBCClass2FEHelperTest_3B"));
  EXPECT_CALL(*klass, GetClassNameOrin())
      .Times(1000)
      .WillRepeatedly(::testing::Return("LNewInterface1InJBCClass2FEHelperTest;"));
  klass->header.accessFlag = jbc::kAccClassInterface;
  helper.PreProcessDecl();
  EXPECT_EQ(helper.IsSkipped(), false);
}

TEST_F(JBCClass2FEHelperTest, ProcessDeclSuperClassForClass) {
  MockJBCClass *klass = mp->New<MockJBCClass>(allocator);
  JBCClass2FEHelper helper(allocator, *klass);
  EXPECT_CALL(*klass, GetClassNameMpl())
      .Times(1000)
      .WillRepeatedly(::testing::Return("LNewClass2InJBCClass2FEHelperTest_3B"));
  EXPECT_CALL(*klass, GetClassNameOrin())
      .Times(1000)
      .WillRepeatedly(::testing::Return("LNewClass2InJBCClass2FEHelperTest;"));
  EXPECT_CALL(*klass, GetSuperClassName())
      .Times(1000)
      .WillRepeatedly(::testing::Return("Ljava/lang/Object;"));
  helper.PreProcessDecl();
  helper.ProcessDeclSuperClass();
  MIRStructType *structType = helper.GetContainer();
  EXPECT_NE(structType, nullptr);
  EXPECT_EQ(structType->GetKind(), kTypeClass);
  MIRClassType *classType = static_cast<MIRClassType*>(structType);
  MIRType *superType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(classType->GetParentTyIdx());
  EXPECT_EQ(superType->GetKind(), kTypeClass);
  EXPECT_EQ(superType->GetCompactMplTypeName(), "Ljava_2Flang_2FObject_3B");
}

TEST_F(JBCClass2FEHelperTest, ProcessDeclImplementsForClass) {
  MockJBCClass *klass = mp->New<MockJBCClass>(allocator);
  JBCClass2FEHelper helper(allocator, *klass);
  EXPECT_CALL(*klass, GetClassNameMpl())
      .Times(1000)
      .WillRepeatedly(::testing::Return("LNewClass3InJBCClass2FEHelperTest_3B"));
  EXPECT_CALL(*klass, GetClassNameOrin())
      .Times(1000)
      .WillRepeatedly(::testing::Return("LNewClass3InJBCClass2FEHelperTest;"));
  EXPECT_CALL(*klass, GetInterfaceNames())
      .Times(1000)
      .WillRepeatedly(::testing::Return(std::vector<std::string>({ "LTestInterface1;", "LTestInterface2;" })));
  helper.PreProcessDecl();
  helper.ProcessDeclImplements();
  MIRStructType *structType = helper.GetContainer();
  EXPECT_NE(structType, nullptr);
  EXPECT_EQ(structType->GetKind(), kTypeClass);
  MIRClassType *classType = static_cast<MIRClassType*>(structType);
  const std::vector<TyIdx> &interfaces = classType->GetInterfaceImplemented();
  EXPECT_EQ(interfaces.size(), 2);
  MIRType *interfaceType0 = GlobalTables::GetTypeTable().GetTypeFromTyIdx(interfaces[0]);
  EXPECT_EQ(interfaceType0->GetKind(), kTypeInterfaceIncomplete);
  EXPECT_EQ(interfaceType0->GetCompactMplTypeName(), "LTestInterface1_3B");
  MIRType *interfaceType1 = GlobalTables::GetTypeTable().GetTypeFromTyIdx(interfaces[1]);
  EXPECT_EQ(interfaceType1->GetKind(), kTypeInterfaceIncomplete);
  EXPECT_EQ(interfaceType1->GetCompactMplTypeName(), "LTestInterface2_3B");
}

TEST_F(JBCClass2FEHelperTest, ProcessDeclSuperClassForInterface) {
  MockJBCClass *klass = mp->New<MockJBCClass>(allocator);
  klass->header.accessFlag = jbc::kAccClassInterface;
  JBCClass2FEHelper helper(allocator, *klass);
  EXPECT_CALL(*klass, GetClassNameMpl())
      .Times(1000)
      .WillRepeatedly(::testing::Return("LNewInterface2InJBCClass2FEHelperTest_3B"));
  EXPECT_CALL(*klass, GetClassNameOrin())
      .Times(1000)
      .WillRepeatedly(::testing::Return("LNewInterface2InJBCClass2FEHelperTest;"));
  EXPECT_CALL(*klass, GetSuperClassName())
      .Times(1000)
      .WillRepeatedly(::testing::Return("Ljava/io/Serializable;"));
  helper.PreProcessDecl();
  helper.ProcessDeclSuperClass();
  MIRStructType *structType = helper.GetContainer();
  EXPECT_NE(structType, nullptr);
  EXPECT_EQ(structType->GetKind(), kTypeInterface);
  MIRInterfaceType *interfaceType = static_cast<MIRInterfaceType*>(structType);
  const std::vector<TyIdx> &parents = interfaceType->GetParentsTyIdx();
  ASSERT_EQ(parents.size(), 1);
  MIRType *interfaceType0 = GlobalTables::GetTypeTable().GetTypeFromTyIdx(parents[0]);
  EXPECT_EQ(interfaceType0->GetKind(), kTypeInterface);
  EXPECT_EQ(interfaceType0->GetCompactMplTypeName(), "Ljava_2Fio_2FSerializable_3B");
}

TEST_F(JBCClass2FEHelperTest, ProcessDeclImplementsForInterface) {
  MockJBCClass *klass = mp->New<MockJBCClass>(allocator);
  klass->header.accessFlag = jbc::kAccClassInterface;
  JBCClass2FEHelper helper(allocator, *klass);
  EXPECT_CALL(*klass, GetClassNameMpl())
      .Times(1000)
      .WillRepeatedly(::testing::Return("LNewInterface3InJBCClass2FEHelperTest_3B"));
  EXPECT_CALL(*klass, GetClassNameOrin())
      .Times(1000)
      .WillRepeatedly(::testing::Return("LNewInterface3InJBCClass2FEHelperTest;"));
  EXPECT_CALL(*klass, GetInterfaceNames())
      .Times(1000)
      .WillRepeatedly(::testing::Return(std::vector<std::string>({ "LTestInterface3;", "LTestInterface4;" })));
  helper.PreProcessDecl();
  helper.ProcessDeclImplements();
  MIRStructType *structType = helper.GetContainer();
  EXPECT_NE(structType, nullptr);
  EXPECT_EQ(structType->GetKind(), kTypeInterface);
  MIRInterfaceType *interfaceType = static_cast<MIRInterfaceType*>(structType);
  const std::vector<TyIdx> &parents = interfaceType->GetParentsTyIdx();
  EXPECT_EQ(parents.size(), 2);
  MIRType *interfaceType0 = GlobalTables::GetTypeTable().GetTypeFromTyIdx(parents[0]);
  EXPECT_EQ(interfaceType0->GetKind(), kTypeInterfaceIncomplete);
  EXPECT_EQ(interfaceType0->GetCompactMplTypeName(), "LTestInterface3_3B");
  MIRType *interfaceType1 = GlobalTables::GetTypeTable().GetTypeFromTyIdx(parents[1]);
  EXPECT_EQ(interfaceType1->GetKind(), kTypeInterfaceIncomplete);
  EXPECT_EQ(interfaceType1->GetCompactMplTypeName(), "LTestInterface4_3B");
}

TEST_F(JBCClass2FEHelperTest, CreateSymbol_Class) {
  MockJBCClass *klass = mp->New<MockJBCClass>(allocator);
  JBCClass2FEHelper helper(allocator, *klass);
  EXPECT_CALL(*klass, GetClassNameMpl())
      .Times(1000)
      .WillRepeatedly(::testing::Return("LNewClass4InJBCClass2FEHelperTest_3B"));
  EXPECT_CALL(*klass, GetClassNameOrin())
      .Times(1000)
      .WillRepeatedly(::testing::Return("LNewClass4InJBCClass2FEHelperTest;"));
  helper.PreProcessDecl();
  helper.CreateSymbol();
  EXPECT_NE(helper.mirSymbol, nullptr);
  RedirectCout();
  helper.mirSymbol->Dump(false, 0);
  EXPECT_EQ(GetBufferString(),
      "javaclass $LNewClass4InJBCClass2FEHelperTest_3B <$LNewClass4InJBCClass2FEHelperTest_3B>\n");
  RestoreCout();
}

TEST_F(JBCClass2FEHelperTest, CreateSymbol_Interface) {
  MockJBCClass *klass = mp->New<MockJBCClass>(allocator);
  JBCClass2FEHelper helper(allocator, *klass);
  EXPECT_CALL(*klass, GetClassNameMpl())
      .Times(1000)
      .WillRepeatedly(::testing::Return("LNewInterface4InJBCClass2FEHelperTest_3B"));
  EXPECT_CALL(*klass, GetClassNameOrin())
      .Times(1000)
      .WillRepeatedly(::testing::Return("LNewInterface4InJBCClass2FEHelperTest;"));
  klass->header.accessFlag = jbc::kAccClassInterface;
  helper.PreProcessDecl();
  helper.CreateSymbol();
  EXPECT_NE(helper.mirSymbol, nullptr);
  RedirectCout();
  helper.mirSymbol->Dump(false, 0);
  EXPECT_EQ(GetBufferString(),
      "javainterface $LNewInterface4InJBCClass2FEHelperTest_3B <$LNewInterface4InJBCClass2FEHelperTest_3B>\n");
  RestoreCout();
}

TEST_F(JBCClass2FEHelperTest, ProcessDecl_Class) {
  MockJBCClass *klass = mp->New<MockJBCClass>(allocator);
  klass->header.accessFlag = jbc::kAccClassAbstract;
  JBCClass2FEHelper helper(allocator, *klass);
  EXPECT_CALL(*klass, GetClassNameMpl())
      .Times(1000)
      .WillRepeatedly(::testing::Return("LNewClass5InJBCClass2FEHelperTest_3B"));
  EXPECT_CALL(*klass, GetClassNameOrin())
      .Times(1000)
      .WillRepeatedly(::testing::Return("LNewClass5InJBCClass2FEHelperTest;"));
  EXPECT_CALL(*klass, GetSuperClassName())
      .Times(1000)
      .WillRepeatedly(::testing::Return("Ljava/lang/Object;"));
  EXPECT_CALL(*klass, GetInterfaceNames())
      .Times(1000)
      .WillRepeatedly(::testing::Return(std::vector<std::string>()));
  helper.PreProcessDecl();
  helper.ProcessDecl();
  EXPECT_NE(helper.mirSymbol, nullptr);
  RedirectCout();
  helper.mirSymbol->Dump(false, 0);
  EXPECT_EQ(GetBufferString(),
      "javaclass $LNewClass5InJBCClass2FEHelperTest_3B <$LNewClass5InJBCClass2FEHelperTest_3B> abstract\n");
  RestoreCout();
}

TEST_F(JBCClass2FEHelperTest, ProcessDecl_Interface) {
  MockJBCClass *klass = mp->New<MockJBCClass>(allocator);
  klass->header.accessFlag = jbc::kAccClassInterface | jbc::kAccClassPublic;
  JBCClass2FEHelper helper(allocator, *klass);
  EXPECT_CALL(*klass, GetClassNameMpl())
      .Times(1000)
      .WillRepeatedly(::testing::Return("LNewInterface5InJBCClass2FEHelperTest_3B"));
  EXPECT_CALL(*klass, GetClassNameOrin())
      .Times(1000)
      .WillRepeatedly(::testing::Return("LNewInterface5InJBCClass2FEHelperTest;"));
  EXPECT_CALL(*klass, GetSuperClassName())
      .Times(1000)
      .WillRepeatedly(::testing::Return("Ljava/lang/Object;"));
  EXPECT_CALL(*klass, GetInterfaceNames())
      .Times(1000)
      .WillRepeatedly(::testing::Return(std::vector<std::string>()));
  helper.PreProcessDecl();
  helper.ProcessDecl();
  EXPECT_NE(helper.mirSymbol, nullptr);
  RedirectCout();
  helper.mirSymbol->Dump(false, 0);
  EXPECT_EQ(GetBufferString(),
      "javainterface $LNewInterface5InJBCClass2FEHelperTest_3B <$LNewInterface5InJBCClass2FEHelperTest_3B> public\n");
  RestoreCout();
}

TEST_F(JBCClassField2FEHelperTest, ProcessDeclWithContainer_Instance) {
  MockJBCClass *klass = mp->New<MockJBCClass>(allocator);
  EXPECT_CALL(*klass, GetClassNameOrin()).Times(1000)
      .WillRepeatedly(::testing::Return("LTestPack/TestClass;"));
  MockJBCClassField *field = mp->New<MockJBCClassField>(allocator, *klass);
  EXPECT_CALL(*field, GetAccessFlag()).Times(1).WillOnce(::testing::Return(jbc::kAccFieldPublic));
  EXPECT_CALL(*field, IsStatic()).Times(100).WillRepeatedly(::testing::Return(false));
  EXPECT_CALL(*field, GetName(::testing::_)).Times(1).WillOnce(::testing::Return("field"));
  EXPECT_CALL(*field, GetDescription(::testing::_)).Times(1).WillOnce(::testing::Return("Ljava/lang/Object;"));
  JBCClass2FEHelper klassHelper(allocator, *klass);
  JBCClassField2FEHelper fieldHelper(allocator, *field);
  ASSERT_EQ(fieldHelper.ProcessDeclWithContainer(allocator, klassHelper), true);
  // check field name
  std::string fieldName = GlobalTables::GetStrTable().GetStringFromStrIdx(fieldHelper.mirFieldPair.first);
  EXPECT_EQ(fieldName, "field");
  // check field type
  MIRType *type = GlobalTables::GetTypeTable().GetTypeFromTyIdx(fieldHelper.mirFieldPair.second.first);
  EXPECT_EQ(type->GetKind(), kTypePointer);
  EXPECT_EQ(type->GetCompactMplTypeName(), "Ljava_2Flang_2FObject_3B");
  // check field attr
  EXPECT_EQ(fieldHelper.mirFieldPair.second.second.GetAttr(FLDATTR_public), true);
}

TEST_F(JBCClassField2FEHelperTest, ProcessDeclWithContainer_Static) {
  MockJBCClass *klass = mp->New<MockJBCClass>(allocator);
  EXPECT_CALL(*klass, GetClassNameOrin()).Times(1000)
      .WillRepeatedly(::testing::Return("LTestPack/TestClass;"));
  MockJBCClassField *field = mp->New<MockJBCClassField>(allocator, *klass);
  EXPECT_CALL(*field, GetAccessFlag()).Times(1).WillOnce(::testing::Return(jbc::kAccFieldProtected));
  EXPECT_CALL(*field, IsStatic()).Times(100).WillRepeatedly(::testing::Return(true));
  EXPECT_CALL(*field, GetName(::testing::_)).Times(1).WillOnce(::testing::Return("field_static"));
  EXPECT_CALL(*field, GetDescription(::testing::_)).Times(1).WillOnce(::testing::Return("Ljava/lang/Object;"));
  JBCClass2FEHelper klassHelper(allocator, *klass);
  JBCClassField2FEHelper fieldHelper(allocator, *field);
  ASSERT_EQ(fieldHelper.ProcessDeclWithContainer(allocator, klassHelper), true);
  // check field name
  std::string fieldName = GlobalTables::GetStrTable().GetStringFromStrIdx(fieldHelper.mirFieldPair.first);
  EXPECT_EQ(fieldName, "LTestPack_2FTestClass_3B_7Cfield__static");
  // check field type
  MIRType *type = GlobalTables::GetTypeTable().GetTypeFromTyIdx(fieldHelper.mirFieldPair.second.first);
  EXPECT_EQ(type->GetKind(), kTypePointer);
  EXPECT_EQ(type->GetCompactMplTypeName(), "Ljava_2Flang_2FObject_3B");
  // check field attr
  EXPECT_EQ(fieldHelper.mirFieldPair.second.second.GetAttr(FLDATTR_protected), true);
}

TEST_F(JBCClassField2FEHelperTest, ProcessDeclWithContainer_Static_AllType) {
  MockJBCClass *klass = mp->New<MockJBCClass>(allocator);
  EXPECT_CALL(*klass, GetClassNameOrin()).Times(1000)
      .WillRepeatedly(::testing::Return("LTestPack/TestClass;"));
  MockJBCClassField *field = mp->New<MockJBCClassField>(allocator, *klass);
  EXPECT_CALL(*field, GetAccessFlag()).Times(1).WillOnce(::testing::Return(jbc::kAccFieldPrivate));
  EXPECT_CALL(*field, IsStatic()).Times(100).WillRepeatedly(::testing::Return(true));
  EXPECT_CALL(*field, GetName(::testing::_)).Times(1).WillOnce(::testing::Return("field_static"));
  EXPECT_CALL(*field, GetDescription(::testing::_)).Times(1).WillOnce(::testing::Return("Ljava/lang/Object;"));
  JBCClass2FEHelper klassHelper(allocator, *klass);
  FEOptions::GetInstance().SetModeJavaStaticFieldName(FEOptions::kAllType);
  JBCClassField2FEHelper fieldHelper(allocator, *field);
  ASSERT_EQ(fieldHelper.ProcessDeclWithContainer(allocator, klassHelper), true);
  // check field name
  std::string fieldName = GlobalTables::GetStrTable().GetStringFromStrIdx(fieldHelper.mirFieldPair.first);
  EXPECT_EQ(fieldName, "LTestPack_2FTestClass_3B_7Cfield__static_7CLjava_2Flang_2FObject_3B");
  // check field type
  MIRType *type = GlobalTables::GetTypeTable().GetTypeFromTyIdx(fieldHelper.mirFieldPair.second.first);
  EXPECT_EQ(type->GetKind(), kTypePointer);
  EXPECT_EQ(type->GetCompactMplTypeName(), "Ljava_2Flang_2FObject_3B");
  // check field attr
  EXPECT_EQ(fieldHelper.mirFieldPair.second.second.GetAttr(FLDATTR_private), true);
}

TEST_F(JBCClassField2FEHelperTest, ProcessDeclWithContainer_Static_Smart) {
  MockJBCClass *klass = mp->New<MockJBCClass>(allocator);
  EXPECT_CALL(*klass, GetClassNameOrin()).Times(1000)
      .WillRepeatedly(::testing::Return("LTestPack/TestClass;"));
  MockJBCClassField *field = mp->New<MockJBCClassField>(allocator, *klass);
  EXPECT_CALL(*field, GetAccessFlag()).Times(1).WillOnce(::testing::Return(jbc::kAccFieldPrivate));
  EXPECT_CALL(*field, IsStatic()).Times(100).WillRepeatedly(::testing::Return(true));
  EXPECT_CALL(*field, GetName(::testing::_)).Times(1).WillOnce(::testing::Return("field_static"));
  EXPECT_CALL(*field, GetDescription(::testing::_)).Times(1).WillOnce(::testing::Return("Ljava/lang/Object;"));
  MockJBCClass2FEHelper klassHelper(allocator, *klass);
  EXPECT_CALL(klassHelper, IsStaticFieldProguard()).Times(1).WillOnce(::testing::Return(true));
  FEOptions::GetInstance().SetModeJavaStaticFieldName(FEOptions::kSmart);
  JBCClassField2FEHelper fieldHelper(allocator, *field);
  ASSERT_EQ(fieldHelper.ProcessDeclWithContainer(allocator, klassHelper), true);
  // check field name
  std::string fieldName = GlobalTables::GetStrTable().GetStringFromStrIdx(fieldHelper.mirFieldPair.first);
  EXPECT_EQ(fieldName, "LTestPack_2FTestClass_3B_7Cfield__static_7CLjava_2Flang_2FObject_3B");
  // check field type
  MIRType *type = GlobalTables::GetTypeTable().GetTypeFromTyIdx(fieldHelper.mirFieldPair.second.first);
  EXPECT_EQ(type->GetKind(), kTypePointer);
  EXPECT_EQ(type->GetCompactMplTypeName(), "Ljava_2Flang_2FObject_3B");
  // check field attr
  EXPECT_EQ(fieldHelper.mirFieldPair.second.second.GetAttr(FLDATTR_private), true);
}
}  // namespace maple
