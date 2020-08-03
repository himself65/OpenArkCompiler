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
#include <vector>
#include <list>
#include "feir_test_base.h"
#include "feir_stmt.h"
#include "feir_var.h"
#include "feir_var_reg.h"
#include "feir_var_name.h"
#include "feir_type_helper.h"
#include "feir_builder.h"

namespace maple {
class FEIRDFGTest : public FEIRTestBase {
 public:
  std::vector<UniqueFEIRVar> vars;
  std::vector<UniqueFEIRStmt> checkPoints;
  FEIRDFGTest() = default;
  virtual ~FEIRDFGTest() = default;
  void Init(uint32 nVar, uint32 nCheckPoint) {
    vars.clear();
    checkPoints.clear();
    vars.resize(nVar);
    for (uint i = 0; i < nCheckPoint; i++) {
      checkPoints.push_back(std::make_unique<FEIRStmtCheckPoint>());
    }
  }

  void AddVar2DFGNodes(std::list<UniqueFEIRVar*> &nodes, uint32 idx, UniqueFEIRVar var, bool isDef) {
    CHECK_FATAL(idx < vars.size(), "index out of range");
    var->SetDef(isDef);
    vars[idx].reset(var.release());
    nodes.push_back(&vars[idx]);
  }

  void BuildDFG(const std::string &str) {
    if (str.length() == 0) {
      return;
    }
    std::string item;
    std::string remainStr = str;
    do {
      size_t npos = remainStr.find(',');
      item = remainStr.substr(0, npos);
      if (item.length() > 0) {
        size_t npos2 = item.find("->");
        std::string strSrc = item.substr(0, npos2);
        std::string strDst = item.substr(npos2 + 2);
        uint32 src = std::atoi(strSrc.c_str());
        uint32 dst = std::atoi(strDst.c_str());
        TransformCheckPoint(checkPoints[dst])->AddPredCheckPoint(checkPoints[src]);
      }
      if (npos != std::string::npos) {
        remainStr = remainStr.substr(npos + 1);
      } else {
        remainStr = "";
      }
    } while (remainStr.length() > 0);
  }

  FEIRStmtCheckPoint *TransformCheckPoint(const UniqueFEIRStmt &stmtCheckPoint) {
    if (stmtCheckPoint == nullptr || stmtCheckPoint->GetKind() != FEIRNodeKind::kStmtCheckPoint) {
      CHECK_FATAL(false, "invalid input");
    }
    return static_cast<FEIRStmtCheckPoint*>(stmtCheckPoint.get());
  }
};

// ---------- FEIRDFGNode ----------
TEST_F(FEIRDFGTest, FEIRDFGNode_VarReg_EqualsTo) {
  std::unique_ptr<FEIRVar> varReg1 = FEIRBuilder::CreateVarReg(0, PTY_i32);
  std::unique_ptr<FEIRVar> varReg2 = FEIRBuilder::CreateVarReg(0, PTY_i64);
  FEIRDFGNode node1(varReg1);
  FEIRDFGNode node2(varReg2);
  FEIRDFGNode node11(node1);
  FEIRDFGNode node21(node2);
  FEIRDFGNode node12 = node11;
  FEIRDFGNode node22 = node21;
  EXPECT_EQ(node12 == node22, true);
}

TEST_F(FEIRDFGTest, FEIRDFGNode_VarDiff_EqualsTo) {
  std::unique_ptr<FEIRVar> varReg1 = FEIRBuilder::CreateVarReg(0, PTY_i32);
  std::unique_ptr<FEIRVar> varReg2 = FEIRBuilder::CreateVarName("testVar2", PTY_i32, false, false);
  (void)GlobalTables::GetStrTable().GetOrCreateStrIdxFromName("testVar3");
  std::unique_ptr<FEIRVar> varReg3 = FEIRBuilder::CreateVarName("testVar3", PTY_i32, false, false);
  FEIRDFGNode node1(varReg1);
  FEIRDFGNode node2(varReg2);
  FEIRDFGNode node3(varReg3);
  EXPECT_EQ(node1 == node2, false);
  EXPECT_EQ(node1 == node3, false);
  EXPECT_EQ(node2 == node3, false);
}

TEST_F(FEIRDFGTest, FEIRDFGNode_VarReg_set) {
  std::unique_ptr<FEIRVar> varReg1 = FEIRBuilder::CreateVarReg(0, PTY_i32);
  std::unique_ptr<FEIRVar> varReg2 = FEIRBuilder::CreateVarReg(0, PTY_i64);
  std::unique_ptr<FEIRVar> varReg3 = FEIRBuilder::CreateVarReg(1, PTY_i32);
  std::unique_ptr<FEIRVar> varReg4 = FEIRBuilder::CreateVarReg(0, PTY_u1);
  std::unordered_set<FEIRDFGNode, FEIRDFGNodeHash> set1;
  set1.insert(FEIRDFGNode(varReg1));
  set1.insert(FEIRDFGNode(varReg2));
  EXPECT_EQ(set1.size(), 1);
  set1.insert(FEIRDFGNode(varReg3));
  EXPECT_EQ(set1.size(), 2);
  set1.erase(FEIRDFGNode(varReg4));
  EXPECT_EQ(set1.size(), 1);
}

// ---------- FEIRStmtCheckPoint ----------
/*
 * Test1
 *   DFG: mplfe/doc/images/ut_cases/DFG/Test1.dot
 *   Image: mplfe/doc/images/ut_cases/DFG/Test1.png
 */
TEST_F(FEIRDFGTest, CheckPointTest1) {
  Init(2, 1);
  // BB0
  std::list<UniqueFEIRVar*> stmt0;
  AddVar2DFGNodes(stmt0, 0, FEIRBuilder::CreateVarReg(0, PTY_i32), true);
  std::list<UniqueFEIRVar*> stmt1;
  AddVar2DFGNodes(stmt1, 1, FEIRBuilder::CreateVarReg(0, PTY_i32), false);
  TransformCheckPoint(checkPoints[0])->RegisterDFGNodes(stmt0);
  TransformCheckPoint(checkPoints[0])->RegisterDFGNodes(stmt1);
  // Build DFG
  BuildDFG("");
  std::set<UniqueFEIRVar*> &defs = TransformCheckPoint(checkPoints[0])->CalcuDef(vars[1]);
  ASSERT_EQ(defs.size(), 1);
  EXPECT_NE(defs.find(&vars[0]), defs.end());
}

/*
 * Test2
 *   DFG: mplfe/doc/images/ut_cases/DFG/Test2.dot
 *   Image: mplfe/doc/images/ut_cases/DFG/Test2.png
 */
TEST_F(FEIRDFGTest, CheckPointTest2) {
  Init(2, 2);
  // BB0
  std::list<UniqueFEIRVar*> stmt0;
  AddVar2DFGNodes(stmt0, 0, FEIRBuilder::CreateVarReg(0, PTY_i32), true);
  TransformCheckPoint(checkPoints[0])->RegisterDFGNodes(stmt0);
  // BB1
  std::list<UniqueFEIRVar*> stmt1;
  AddVar2DFGNodes(stmt1, 1, FEIRBuilder::CreateVarReg(0, PTY_i32), false);
  TransformCheckPoint(checkPoints[1])->RegisterDFGNodes(stmt1);
  // Build DFG
  BuildDFG("0->1");
  std::set<UniqueFEIRVar*> &defs = TransformCheckPoint(checkPoints[1])->CalcuDef(vars[1]);
  ASSERT_EQ(defs.size(), 1);
  EXPECT_NE(defs.find(&vars[0]), defs.end());
}

/*
 * Test3
 *   DFG: mplfe/doc/images/ut_cases/DFG/Test3.dot
 *   Image: mplfe/doc/images/ut_cases/DFG/Test3.png
 */
TEST_F(FEIRDFGTest, Test3) {
  Init(4, 4);
  // BB0
  std::list<UniqueFEIRVar*> stmt0;
  AddVar2DFGNodes(stmt0, 0, FEIRBuilder::CreateVarReg(0, PTY_i32), true);
  TransformCheckPoint(checkPoints[0])->RegisterDFGNodes(stmt0);
  // BB1
  std::list<UniqueFEIRVar*> stmt1;
  AddVar2DFGNodes(stmt1, 1, FEIRBuilder::CreateVarReg(0, PTY_i32), true);
  TransformCheckPoint(checkPoints[1])->RegisterDFGNodes(stmt1);
  // BB2
  std::list<UniqueFEIRVar*> stmt2;
  TransformCheckPoint(checkPoints[2])->RegisterDFGNodes(stmt2);
  // BB3
  std::list<UniqueFEIRVar*> stmt3;
  AddVar2DFGNodes(stmt3, 3, FEIRBuilder::CreateVarReg(0, PTY_i32), false);
  TransformCheckPoint(checkPoints[3])->RegisterDFGNodes(stmt3);
  // Build DFG
  BuildDFG("0->1,0->2,1->3,2->3");
  std::set<UniqueFEIRVar*> &defs = TransformCheckPoint(checkPoints[3])->CalcuDef(vars[3]);
  ASSERT_EQ(defs.size(), 2);
  EXPECT_NE(defs.find(&vars[0]), defs.end());
  EXPECT_NE(defs.find(&vars[1]), defs.end());
}

/*
 * Test4
 *   DFG: mplfe/doc/images/ut_cases/DFG/Test4.dot
 *   Image: mplfe/doc/images/ut_cases/DFG/Test4.png
 */
TEST_F(FEIRDFGTest, Test4) {
  Init(4, 4);
  // BB0
  std::list<UniqueFEIRVar*> stmt0;
  AddVar2DFGNodes(stmt0, 0, FEIRBuilder::CreateVarReg(0, PTY_i32), true);
  TransformCheckPoint(checkPoints[0])->RegisterDFGNodes(stmt0);
  // BB1
  std::list<UniqueFEIRVar*> stmt1;
  AddVar2DFGNodes(stmt1, 1, FEIRBuilder::CreateVarReg(0, PTY_i32), true);
  TransformCheckPoint(checkPoints[1])->RegisterDFGNodes(stmt1);
  // BB2
  std::list<UniqueFEIRVar*> stmt2;
  AddVar2DFGNodes(stmt2, 2, FEIRBuilder::CreateVarReg(0, PTY_i32), true);
  TransformCheckPoint(checkPoints[2])->RegisterDFGNodes(stmt2);
  // BB3
  std::list<UniqueFEIRVar*> stmt3;
  AddVar2DFGNodes(stmt3, 3, FEIRBuilder::CreateVarReg(0, PTY_i32), false);
  TransformCheckPoint(checkPoints[3])->RegisterDFGNodes(stmt3);
  // Build DFG
  BuildDFG("0->1,0->2,1->3,2->3");
  std::set<UniqueFEIRVar*> &defs = TransformCheckPoint(checkPoints[3])->CalcuDef(vars[3]);
  ASSERT_EQ(defs.size(), 2);
  EXPECT_EQ(defs.find(&vars[0]), defs.end());
  EXPECT_NE(defs.find(&vars[1]), defs.end());
  EXPECT_NE(defs.find(&vars[2]), defs.end());
}

/*
 * Test5
 *   DFG: mplfe/doc/images/ut_cases/DFG/Test5.dot
 *   Image: mplfe/doc/images/ut_cases/DFG/Test5.png
 */
TEST_F(FEIRDFGTest, Test5) {
  Init(4, 4);
  // BB0
  std::list<UniqueFEIRVar*> stmt0;
  AddVar2DFGNodes(stmt0, 0, FEIRBuilder::CreateVarReg(0, PTY_i32), true);
  TransformCheckPoint(checkPoints[0])->RegisterDFGNodes(stmt0);
  // BB1
  std::list<UniqueFEIRVar*> stmt1;
  AddVar2DFGNodes(stmt1, 1, FEIRBuilder::CreateVarReg(0, PTY_i32), false);
  TransformCheckPoint(checkPoints[1])->RegisterDFGNodes(stmt1);
  // BB2
  std::list<UniqueFEIRVar*> stmt2;
  AddVar2DFGNodes(stmt2, 2, FEIRBuilder::CreateVarReg(0, PTY_i32), true);
  TransformCheckPoint(checkPoints[2])->RegisterDFGNodes(stmt2);
  // BB3
  std::list<UniqueFEIRVar*> stmt3;
  AddVar2DFGNodes(stmt3, 3, FEIRBuilder::CreateVarReg(0, PTY_i32), false);
  TransformCheckPoint(checkPoints[3])->RegisterDFGNodes(stmt3);
  // Build DFG
  BuildDFG("0->1,1->2,2->3,2->1");
  std::set<UniqueFEIRVar*> &defs1 = TransformCheckPoint(checkPoints[1])->CalcuDef(vars[1]);
  ASSERT_EQ(defs1.size(), 2);
  EXPECT_NE(defs1.find(&vars[0]), defs1.end());
  EXPECT_NE(defs1.find(&vars[2]), defs1.end());
  std::set<UniqueFEIRVar*> &defs3 = TransformCheckPoint(checkPoints[3])->CalcuDef(vars[3]);
  ASSERT_EQ(defs3.size(), 1);
  EXPECT_NE(defs3.find(&vars[2]), defs3.end());
}
}  // namespace maple