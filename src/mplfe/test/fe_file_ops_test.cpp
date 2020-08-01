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
#include "fe_file_ops.h"

namespace maple {
TEST(FEFileOps, GetFilePath) {
  EXPECT_EQ(FEFileOps::GetFilePath("name"), "");
  EXPECT_EQ(FEFileOps::GetFilePath("/name"), "/");
  EXPECT_EQ(FEFileOps::GetFilePath("path/"), "path/");
  EXPECT_EQ(FEFileOps::GetFilePath("path/name"), "path/");
  EXPECT_EQ(FEFileOps::GetFilePath("/path/name"), "/path/");
  EXPECT_EQ(FEFileOps::GetFilePath("../name"), "../");
}

TEST(FEFileOps, GetFileNameWithExt) {
  EXPECT_EQ(FEFileOps::GetFileNameWithExt("name"), "name");
  EXPECT_EQ(FEFileOps::GetFileNameWithExt("/name"), "name");
  EXPECT_EQ(FEFileOps::GetFileNameWithExt("path/"), "");
  EXPECT_EQ(FEFileOps::GetFileNameWithExt("path/name"), "name");
  EXPECT_EQ(FEFileOps::GetFileNameWithExt("/path/name"), "name");
  EXPECT_EQ(FEFileOps::GetFileNameWithExt("../name"), "name");
}

TEST(FEFileOps, GetFileName) {
  EXPECT_EQ(FEFileOps::GetFileName("name.ext"), "name");
  EXPECT_EQ(FEFileOps::GetFileName("name"), "name");
  EXPECT_EQ(FEFileOps::GetFileName(".ext"), "");
  EXPECT_EQ(FEFileOps::GetFileName("/name.ext"), "name");
  EXPECT_EQ(FEFileOps::GetFileName("path/name.ext"), "name");
}

TEST(FEFileOps, GetFileExtName) {
  EXPECT_EQ(FEFileOps::GetFileExtName("name.ext"), "ext");
  EXPECT_EQ(FEFileOps::GetFileExtName("name"), "");
  EXPECT_EQ(FEFileOps::GetFileExtName(".ext"), "ext");
  EXPECT_EQ(FEFileOps::GetFileExtName("/name.ext"), "ext");
  EXPECT_EQ(FEFileOps::GetFileExtName("path/name.ext"), "ext");
}
}  // namespace maple