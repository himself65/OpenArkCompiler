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
#include "redirect_buffer.h"
#include "global_tables.h"
#include "mir_module.h"
#include "mir_builder.h"

namespace maple {
class FEIRTestBase : public testing::Test, public RedirectBuffer {
 public:
  static MemPool *mp;
  MapleAllocator allocator;
  MIRBuilder mirBuilder;
  MIRFunction func;
  FEIRTestBase();
  virtual ~FEIRTestBase() = default;

  static void SetUpTestCase();
  static void TearDownTestCase();
};
}  // namespace maple