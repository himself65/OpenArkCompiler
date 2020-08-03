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
#include <unordered_set>
#include <unordered_map>
#include "feir_test_base.h"
#include "feir_stmt.h"
#include "feir_var.h"
#include "feir_var_reg.h"
#include "feir_var_name.h"
#include "feir_type_helper.h"
#include "feir_builder.h"
#include "mplfe_ut_regx.h"

namespace maple {
class FEIRBuilderTest : public FEIRTestBase {
 public:
  FEIRBuilderTest() = default;
  virtual ~FEIRBuilderTest() = default;
};

// ---------- FEIRStmtDAssign ----------
TEST_F(FEIRBuilderTest, CreateExprDRead) {
  UniqueFEIRExpr expr = FEIRBuilder::CreateExprDRead(FEIRBuilder::CreateVarReg(0, PTY_i32));
  BaseNode *mirNode = expr->GenMIRNode(mirBuilder);
  RedirectCout();
  mirNode->Dump();
  std::string dumpStr = GetBufferString();
  std::string pattern = std::string("dread i32 %Reg0_i32") + MPLFEUTRegx::Any();
  EXPECT_EQ(MPLFEUTRegx::Match(dumpStr, pattern), true);
  RestoreCout();
}
}  // namespace maple
