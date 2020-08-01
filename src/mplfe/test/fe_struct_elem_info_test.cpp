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
#include "global_tables.h"
#include "namemangler.h"
#include "fe_struct_elem_info.h"
#include "fe_manager.h"
#include "mplfe_ut_environment.h"
#include "redirect_buffer.h"

namespace maple {
class FEStructFieldInfoTest : public testing::Test, public RedirectBuffer {
 public:
  FEStructFieldInfoTest()
      : mirBuilder(&MPLFEUTEnvironment::GetMIRModule()) {}

  virtual ~FEStructFieldInfoTest() = default;

  MIRBuilder mirBuilder;
};

TEST_F(FEStructFieldInfoTest, FEStructFieldInfo) {
  std::string fullNameJava = "Ljava/lang/Integer;|MIN_VALUE|I";
  std::string fullNameMpl = namemangler::EncodeName(fullNameJava);
  GStrIdx fullNameIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(fullNameMpl);
  FEStructFieldInfo info(fullNameIdx, kSrcLangJava, true);
  std::string structName = GlobalTables::GetStrTable().GetStringFromStrIdx(info.structNameIdx);
  std::string elemName = GlobalTables::GetStrTable().GetStringFromStrIdx(info.elemNameIdx);
  std::string signatureName = GlobalTables::GetStrTable().GetStringFromStrIdx(info.signatureNameIdx);
  EXPECT_EQ(structName, namemangler::EncodeName("Ljava/lang/Integer;"));
  EXPECT_EQ(elemName, namemangler::EncodeName("MIN_VALUE"));
  EXPECT_EQ(signatureName, namemangler::EncodeName("I"));
  EXPECT_EQ(info.fieldType->IsScalar(), true);
}

TEST_F(FEStructFieldInfoTest, SearchStructFieldJava) {
  std::string fullNameJava = "Ljava/lang/Integer;|MIN_VALUE|I";
  std::string fullNameMpl = namemangler::EncodeName(fullNameJava);
  GStrIdx fullNameIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(fullNameMpl);
  FEStructFieldInfo info(fullNameIdx, kSrcLangJava, true);
  MIRStructType *structType =
      FEManager::GetTypeManager().GetStructTypeFromName(namemangler::EncodeName("Ljava/lang/Integer;"));
  ASSERT_NE(structType, nullptr);
  EXPECT_EQ(info.SearchStructFieldJava(*structType, mirBuilder, true), true);
  EXPECT_EQ(info.SearchStructFieldJava(*structType, mirBuilder, false), false);
}
}  // namespace maple
