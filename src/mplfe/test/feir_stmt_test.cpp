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
class FEIRStmtTest : public FEIRTestBase {
 public:
  FEIRStmtTest() = default;
  virtual ~FEIRStmtTest() = default;
};

// ---------- FEIRExprConst ----------
TEST_F(FEIRStmtTest, FEIRExprConst_i64) {
  std::unique_ptr<FEIRExprConst> exprConst = std::make_unique<FEIRExprConst>(int64{ 0x100 }, PTY_i64);
  std::unique_ptr<FEIRExpr> exprConst2 = exprConst->Clone();
  BaseNode *baseNode = exprConst2->GenMIRNode(mirBuilder);
  RedirectCout();
  baseNode->Dump();
  EXPECT_EQ(GetBufferString(), "constval i64 256\n");
  RestoreCout();
}

TEST_F(FEIRStmtTest, FEIRExprConst_u64) {
  std::unique_ptr<FEIRExprConst> exprConst = std::make_unique<FEIRExprConst>(uint64{ 0x100 }, PTY_u64);
  std::unique_ptr<FEIRExpr> exprConst2 = exprConst->Clone();
  BaseNode *baseNode = exprConst2->GenMIRNode(mirBuilder);
  RedirectCout();
  baseNode->Dump();
  EXPECT_EQ(GetBufferString(), "constval u64 256\n");
  RestoreCout();
}

TEST_F(FEIRStmtTest, FEIRExprConst_f32) {
  std::unique_ptr<FEIRExprConst> exprConst = std::make_unique<FEIRExprConst>(1.5f);
  std::unique_ptr<FEIRExpr> exprConst2 = exprConst->Clone();
  BaseNode *baseNode = exprConst2->GenMIRNode(mirBuilder);
  RedirectCout();
  baseNode->Dump();
  EXPECT_EQ(GetBufferString(), "constval f32 1.5f\n");
  RestoreCout();
}

TEST_F(FEIRStmtTest, FEIRExprConst_f64) {
  std::unique_ptr<FEIRExprConst> exprConst = std::make_unique<FEIRExprConst>(1.5);
  std::unique_ptr<FEIRExpr> exprConst2 = exprConst->Clone();
  BaseNode *baseNode = exprConst2->GenMIRNode(mirBuilder);
  RedirectCout();
  baseNode->Dump();
  EXPECT_EQ(GetBufferString(), "constval f64 1.5\n");
  RestoreCout();
}

TEST_F(FEIRStmtTest, FEIRExprConstUnsupported) {
  std::unique_ptr<FEIRExprConst> exprConst = std::make_unique<FEIRExprConst>(int64{ 0 }, PTY_unknown);
  BaseNode *baseNode = exprConst->GenMIRNode(mirBuilder);
  EXPECT_EQ(baseNode, nullptr);
}

// ---------- FEIRExprUnary ----------
TEST_F(FEIRStmtTest, FEIRExprUnary) {
  std::unique_ptr<FEIRVar> varReg = FEIRBuilder::CreateVarReg(0, PTY_i32);
  std::unique_ptr<FEIRExprDRead> exprDRead = std::make_unique<FEIRExprDRead>(std::move(varReg));
  std::unique_ptr<FEIRExprUnary> expr = std::make_unique<FEIRExprUnary>(OP_neg, std::move(exprDRead));
  expr->GetType()->SetPrimType(PTY_i32);
  std::unique_ptr<FEIRExpr> expr2 = expr->Clone();
  BaseNode *baseNode = expr2->GenMIRNode(mirBuilder);
  RedirectCout();
  baseNode->Dump();
  std::string dumpStr = GetBufferString();
  std::string pattern = std::string("neg i32 \\(dread i32 %Reg0_i32\\)") + MPLFEUTRegx::Any();
  EXPECT_EQ(MPLFEUTRegx::Match(dumpStr, pattern), true);
  RestoreCout();
  std::vector<FEIRVar*> varUses = expr2->GetVarUses();
  ASSERT_EQ(varUses.size(), 1);
}

// ---------- FEIRExprTypeCvt ----------
TEST_F(FEIRStmtTest, FEIRExprTypeCvtMode1) {
  std::unique_ptr<FEIRVar> varReg = FEIRBuilder::CreateVarReg(0, PTY_i32);
  std::unique_ptr<FEIRExprDRead> exprDRead = std::make_unique<FEIRExprDRead>(std::move(varReg));
  std::unique_ptr<FEIRExprUnary> expr = std::make_unique<FEIRExprTypeCvt>(OP_cvt, std::move(exprDRead));
  expr->GetType()->SetPrimType(PTY_f32);
  std::unique_ptr<FEIRExpr> expr2 = expr->Clone();
  BaseNode *baseNode = expr2->GenMIRNode(mirBuilder);
  RedirectCout();
  baseNode->Dump();
  std::string dumpStr = GetBufferString();
  std::string pattern = std::string("cvt f32 i32 \\(dread i32 %Reg0_i32\\)") + MPLFEUTRegx::Any();
  EXPECT_EQ(MPLFEUTRegx::Match(dumpStr, pattern), true);
  RestoreCout();
}

TEST_F(FEIRStmtTest, FEIRExprTypeCvtMode2) {
  std::unique_ptr<FEIRVar> varReg = FEIRBuilder::CreateVarReg(0, PTY_f32);
  std::unique_ptr<FEIRExprDRead> exprDRead = std::make_unique<FEIRExprDRead>(std::move(varReg));
  std::unique_ptr<FEIRExprTypeCvt> expr = std::make_unique<FEIRExprTypeCvt>(OP_round, std::move(exprDRead));
  expr->GetType()->SetPrimType(PTY_i32);
  std::unique_ptr<FEIRExpr> expr2 = expr->Clone();
  BaseNode *baseNode = expr2->GenMIRNode(mirBuilder);
  RedirectCout();
  baseNode->Dump();
  std::string dumpStr = GetBufferString();
  std::string pattern = std::string("round i32 f32 \\(dread f32 %Reg0_f32\\)") + MPLFEUTRegx::Any();
  EXPECT_EQ(MPLFEUTRegx::Match(dumpStr, pattern), true);
  RestoreCout();
}

TEST_F(FEIRStmtTest, FEIRExprTypeCvtMode3) {
  std::unique_ptr<FEIRType> typeVar = FEIRTypeHelper::CreateTypeByJavaName("Ljava/lang/Object;", false, true);
  std::unique_ptr<FEIRVarReg> varReg = std::make_unique<FEIRVarReg>(0, std::move(typeVar));
  std::unique_ptr<FEIRExprDRead> exprDRead = std::make_unique<FEIRExprDRead>(std::move(varReg));
  std::unique_ptr<FEIRType> typeDst = FEIRTypeHelper::CreateTypeByJavaName("Ljava/lang/String;", false, true);
  std::unique_ptr<FEIRExprTypeCvt> expr = std::make_unique<FEIRExprTypeCvt>(std::move(typeDst), OP_retype,
                                                                            std::move(exprDRead));
  std::unique_ptr<FEIRExpr> expr2 = expr->Clone();
  BaseNode *baseNode = expr2->GenMIRNode(mirBuilder);
  RedirectCout();
  baseNode->Dump();
  std::string dumpStr = GetBufferString();
  std::string pattern = std::string("retype ref <\\* <\\$Ljava_2Flang_2FString_3B>> \\(dread ref %Reg0_") +
                        MPLFEUTRegx::RefIndex(MPLFEUTRegx::kAnyNumber) + "\\)" + MPLFEUTRegx::Any();
  EXPECT_EQ(MPLFEUTRegx::Match(dumpStr, pattern), true);
  RestoreCout();
}

// ---------- FEIRExprExtractBits ----------
TEST_F(FEIRStmtTest, FEIRExprExtractBits) {
  std::unique_ptr<FEIRVar> varReg = FEIRBuilder::CreateVarReg(0, PTY_i32);
  std::unique_ptr<FEIRExprDRead> exprDRead = std::make_unique<FEIRExprDRead>(std::move(varReg));
  std::unique_ptr<FEIRExprExtractBits> expr =
      std::make_unique<FEIRExprExtractBits>(OP_extractbits, PTY_i32, 8, 16, std::move(exprDRead));
  std::unique_ptr<FEIRExpr> expr2 = expr->Clone();
  BaseNode *baseNode = expr2->GenMIRNode(mirBuilder);
  RedirectCout();
  baseNode->Dump();
  std::string dumpStr = GetBufferString();
  std::string pattern = std::string("extractbits i32 8 16 \\(dread i32 %Reg0_i32\\)") + MPLFEUTRegx::Any();
  EXPECT_EQ(MPLFEUTRegx::Match(dumpStr, pattern), true);
  RestoreCout();
}

TEST_F(FEIRStmtTest, FEIRExprExtractBits_sext) {
  std::unique_ptr<FEIRVar> varReg = FEIRBuilder::CreateVarReg(0, PTY_i8);
  std::unique_ptr<FEIRExprDRead> exprDRead = std::make_unique<FEIRExprDRead>(std::move(varReg));
  std::unique_ptr<FEIRExprExtractBits> expr =
      std::make_unique<FEIRExprExtractBits>(OP_sext, PTY_i32, std::move(exprDRead));
  std::unique_ptr<FEIRExpr> expr2 = expr->Clone();
  BaseNode *baseNode = expr2->GenMIRNode(mirBuilder);
  RedirectCout();
  baseNode->Dump();
  std::string dumpStr = GetBufferString();
  std::string pattern = std::string("sext i32 8 \\(dread i8 %Reg0_i8\\)") + MPLFEUTRegx::Any();
  EXPECT_EQ(MPLFEUTRegx::Match(dumpStr, pattern), true);
  RestoreCout();
}

TEST_F(FEIRStmtTest, FEIRExprExtractBits_zext) {
  std::unique_ptr<FEIRVar> varReg = FEIRBuilder::CreateVarReg(0, PTY_u16);
  std::unique_ptr<FEIRExprDRead> exprDRead = std::make_unique<FEIRExprDRead>(std::move(varReg));
  std::unique_ptr<FEIRExprExtractBits> expr =
      std::make_unique<FEIRExprExtractBits>(OP_zext, PTY_i32, std::move(exprDRead));
  std::unique_ptr<FEIRExpr> expr2 = expr->Clone();
  BaseNode *baseNode = expr2->GenMIRNode(mirBuilder);
  RedirectCout();
  baseNode->Dump();
  std::string dumpStr = GetBufferString();
  std::string pattern = std::string("zext i32 16 \\(dread u16 %Reg0_u16\\)") + MPLFEUTRegx::Any();
  EXPECT_EQ(MPLFEUTRegx::Match(dumpStr, pattern), true);
  RestoreCout();
}

// ---------- FEIRExprBinary ----------
TEST_F(FEIRStmtTest, FEIRExprBinary_add) {
  std::unique_ptr<FEIRVar> varReg0 = FEIRBuilder::CreateVarReg(0, PTY_i32);
  std::unique_ptr<FEIRVar> varReg1 = FEIRBuilder::CreateVarReg(1, PTY_i32);
  std::unique_ptr<FEIRExprDRead> exprDRead0 = std::make_unique<FEIRExprDRead>(std::move(varReg0));
  std::unique_ptr<FEIRExprDRead> exprDRead1 = std::make_unique<FEIRExprDRead>(std::move(varReg1));
  std::unique_ptr<FEIRExprBinary> expr =
      std::make_unique<FEIRExprBinary>(OP_add, std::move(exprDRead0), std::move(exprDRead1));
  std::unique_ptr<FEIRExpr> expr2 = expr->Clone();
  BaseNode *baseNode = expr2->GenMIRNode(mirBuilder);
  RedirectCout();
  baseNode->Dump();
  std::string dumpStr = GetBufferString();
  std::string pattern = std::string("add i32 \\(dread i32 %Reg0_i32, dread i32 %Reg1_i32\\)") + MPLFEUTRegx::Any();
  EXPECT_EQ(MPLFEUTRegx::Match(dumpStr, pattern), true);
  RestoreCout();
  std::vector<FEIRVar*> varUses = expr2->GetVarUses();
  ASSERT_EQ(varUses.size(), 2);
  EXPECT_EQ(expr2->IsNestable(), true);
  EXPECT_EQ(expr2->IsAddrof(), false);
}

TEST_F(FEIRStmtTest, FEIRExprBinary_cmpg) {
  std::unique_ptr<FEIRVar> varReg0 = FEIRBuilder::CreateVarReg(0, PTY_f64);
  std::unique_ptr<FEIRVar> varReg2 = FEIRBuilder::CreateVarReg(2, PTY_f64);
  std::unique_ptr<FEIRExprDRead> exprDRead0 = std::make_unique<FEIRExprDRead>(std::move(varReg0));
  std::unique_ptr<FEIRExprDRead> exprDRead2 = std::make_unique<FEIRExprDRead>(std::move(varReg2));
  std::unique_ptr<FEIRExprBinary> expr =
      std::make_unique<FEIRExprBinary>(OP_cmpg, std::move(exprDRead0), std::move(exprDRead2));
  std::unique_ptr<FEIRExpr> expr2 = expr->Clone();
  BaseNode *baseNode = expr2->GenMIRNode(mirBuilder);
  RedirectCout();
  baseNode->Dump();
  std::string dumpStr = GetBufferString();
  std::string pattern = std::string("cmpg i32 f64 \\(dread f64 %Reg0_f64, dread f64 %Reg2_f64\\)") + MPLFEUTRegx::Any();
  EXPECT_EQ(MPLFEUTRegx::Match(dumpStr, pattern), true);
  RestoreCout();
}

TEST_F(FEIRStmtTest, FEIRExprBinary_lshr) {
  std::unique_ptr<FEIRVar> varReg0 = FEIRBuilder::CreateVarReg(0, PTY_i32);
  std::unique_ptr<FEIRVar> varReg1 = FEIRBuilder::CreateVarReg(1, PTY_i8);
  std::unique_ptr<FEIRExprDRead> exprDRead0 = std::make_unique<FEIRExprDRead>(std::move(varReg0));
  std::unique_ptr<FEIRExprDRead> exprDRead1 = std::make_unique<FEIRExprDRead>(std::move(varReg1));
  std::unique_ptr<FEIRExprBinary> expr =
      std::make_unique<FEIRExprBinary>(OP_lshr, std::move(exprDRead0), std::move(exprDRead1));
  std::unique_ptr<FEIRExpr> expr2 = expr->Clone();
  BaseNode *baseNode = expr2->GenMIRNode(mirBuilder);
  RedirectCout();
  baseNode->Dump();
  std::string dumpStr = GetBufferString();
  std::string pattern = std::string("lshr i32 \\(dread i32 %Reg0_i32, dread i8 %Reg1_i8\\)") + MPLFEUTRegx::Any();
  EXPECT_EQ(MPLFEUTRegx::Match(dumpStr, pattern), true);
  RestoreCout();
}

TEST_F(FEIRStmtTest, FEIRExprBinary_band) {
  std::unique_ptr<FEIRVar> varReg0 = FEIRBuilder::CreateVarReg(0, PTY_i32);
  std::unique_ptr<FEIRVar> varReg1 = FEIRBuilder::CreateVarReg(1, PTY_i32);
  std::unique_ptr<FEIRExprDRead> exprDRead0 = std::make_unique<FEIRExprDRead>(std::move(varReg0));
  std::unique_ptr<FEIRExprDRead> exprDRead1 = std::make_unique<FEIRExprDRead>(std::move(varReg1));
  std::unique_ptr<FEIRExprBinary> expr =
      std::make_unique<FEIRExprBinary>(OP_band, std::move(exprDRead0), std::move(exprDRead1));
  std::unique_ptr<FEIRExpr> expr2 = expr->Clone();
  BaseNode *baseNode = expr2->GenMIRNode(mirBuilder);
  RedirectCout();
  baseNode->Dump();
  std::string dumpStr = GetBufferString();
  std::string pattern = std::string("band i32 \\(dread i32 %Reg0_i32, dread i32 %Reg1_i32\\)") + MPLFEUTRegx::Any();
  EXPECT_EQ(MPLFEUTRegx::Match(dumpStr, pattern), true);
  RestoreCout();
}

// ---------- FEIRExprTernary ----------
TEST_F(FEIRStmtTest, FEIRExprTernary_add) {
  std::unique_ptr<FEIRVar> varReg0 = FEIRBuilder::CreateVarReg(0, PTY_u1);
  std::unique_ptr<FEIRVar> varReg1 = FEIRBuilder::CreateVarReg(1, PTY_i32);
  std::unique_ptr<FEIRVar> varReg2 = FEIRBuilder::CreateVarReg(2, PTY_i32);
  std::unique_ptr<FEIRExprDRead> exprDRead0 = std::make_unique<FEIRExprDRead>(std::move(varReg0));
  std::unique_ptr<FEIRExprDRead> exprDRead1 = std::make_unique<FEIRExprDRead>(std::move(varReg1));
  std::unique_ptr<FEIRExprDRead> exprDRead2 = std::make_unique<FEIRExprDRead>(std::move(varReg2));
  std::unique_ptr<FEIRExprTernary> expr =
      std::make_unique<FEIRExprTernary>(OP_select, std::move(exprDRead0), std::move(exprDRead1), std::move(exprDRead2));
  std::unique_ptr<FEIRExpr> expr2 = expr->Clone();
  BaseNode *baseNode = expr2->GenMIRNode(mirBuilder);
  RedirectCout();
  baseNode->Dump();
  std::string dumpStr = GetBufferString();
  std::string pattern =
      std::string("select i32 \\(dread u1 %Reg0_u1, dread i32 %Reg1_i32, dread i32 %Reg2_i32\\)") + MPLFEUTRegx::Any();
  EXPECT_EQ(MPLFEUTRegx::Match(dumpStr, pattern), true);
  RestoreCout();
  std::vector<FEIRVar*> varUses = expr2->GetVarUses();
  ASSERT_EQ(varUses.size(), 3);
  EXPECT_EQ(expr2->IsNestable(), true);
  EXPECT_EQ(expr2->IsAddrof(), false);
}

// ---------- FEIRStmtDAssign ----------
TEST_F(FEIRStmtTest, FEIRStmtDAssign) {
  std::unique_ptr<FEIRType> type = FEIRTypeHelper::CreateTypeByJavaName("Ljava/lang/String;", false, true);
  std::unique_ptr<FEIRVarReg> dstVar = std::make_unique<FEIRVarReg>(0, type->Clone());
  std::unique_ptr<FEIRVarReg> srcVar = std::make_unique<FEIRVarReg>(1, type->Clone());
  std::unique_ptr<FEIRExprDRead> exprDRead = std::make_unique<FEIRExprDRead>(std::move(srcVar));
  std::unique_ptr<FEIRStmtDAssign> stmtDAssign =
      std::make_unique<FEIRStmtDAssign>(std::move(dstVar), std::move(exprDRead));
  std::list<StmtNode*> mirNodes = stmtDAssign->GenMIRStmts(mirBuilder);
  ASSERT_EQ(mirNodes.size(), 1);
  RedirectCout();
  mirNodes.front()->Dump();
  std::string dumpStr = GetBufferString();
  std::string pattern = std::string("dassign %Reg0_") + MPLFEUTRegx::RefIndex(MPLFEUTRegx::kAnyNumber) +
                        " 0 \\(dread ref %Reg1_" + MPLFEUTRegx::RefIndex(MPLFEUTRegx::kAnyNumber) + "\\)" +
                        MPLFEUTRegx::Any();
  EXPECT_EQ(MPLFEUTRegx::Match(dumpStr, pattern), true);
  RestoreCout();
}
}  // namespace maple