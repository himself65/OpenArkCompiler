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
#include "fe_type_hierarchy.h"
#include "global_tables.h"

namespace maple {
TEST(FETypeHierarchy, IsParentOf) {
  GStrIdx idxObject = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName("Ljava_2Flang_2FObject_3B");
  GStrIdx idxString = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName("Ljava_2Flang_2FString_3B");
  GStrIdx idxInteger = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName("Ljava_2Flang_2FInteger_3B");
  EXPECT_EQ(FETypeHierarchy::GetInstance().IsParentOf(idxObject, idxString), true);
  EXPECT_EQ(FETypeHierarchy::GetInstance().IsParentOf(idxObject, idxInteger), true);
  EXPECT_EQ(FETypeHierarchy::GetInstance().IsParentOf(idxString, idxInteger), false);
  EXPECT_EQ(FETypeHierarchy::GetInstance().IsParentOf(idxInteger, idxString), false);
  EXPECT_EQ(FETypeHierarchy::GetInstance().IsParentOf(idxString, idxString), true);
}
}  // namespace maple