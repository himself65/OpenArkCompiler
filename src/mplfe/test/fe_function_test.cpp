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
#include "fe_function.h"
#include "redirect_buffer.h"
#include "mplfe_ut_environment.h"

namespace maple {
class GeneralStmtAuxPre : public GeneralStmt {
 public:
  GeneralStmtAuxPre() {
    isAuxPre = true;
  }
  ~GeneralStmtAuxPre() = default;
};

class GeneralStmtAuxPost : public GeneralStmt {
 public:
  GeneralStmtAuxPost() {
    isAuxPost = true;
  }
  ~GeneralStmtAuxPost() = default;
};

class FEFunctionDemo : public FEFunction {
 public:
  FEFunctionDemo(MapleAllocator &allocator, MIRFunction &argMIRFunction)
      : FEFunction(argMIRFunction, std::make_unique<FEFunctionPhaseResult>(true)),
        mapIdxStmt(allocator.Adapter()) {}
  ~FEFunctionDemo() = default;

  bool PreProcessTypeNameIdx() override {
    return false;
  }

  bool GenerateGeneralStmt(const std::string &phaseName) override {
    return true;
  }

  void GenerateGeneralStmtFailCallBack() override {}
  void GenerateGeneralDebugInfo() override {}
  bool GenerateArgVarList(const std::string &phaseName) override {
    return true;
  }

  bool HasThis() override {
    return false;
  }

  bool VerifyGeneral() override {
    return false;
  }

  void VerifyGeneralFailCallBack() override {}
  bool EmitToFEIRStmt(const std::string &phaseName) override {
    return true;
  }

  void LoadGenStmtDemo1();
  void LoadGenStmtDemo2();
  void LoadGenStmtDemo3();
  void LoadGenStmtDemo4();
  void LoadGenStmtDemo5();

  GeneralStmt *GetStmtByIdx(uint32 idx) {
    CHECK_FATAL(mapIdxStmt.find(idx) != mapIdxStmt.end(), "invalid idx");
    return mapIdxStmt[idx];
  }

  template <typename T>
  T *NewGenStmt(uint32 idx) {
    // T *stmt = new T(0);
    GeneralStmt *ptrStmt = RegisterGeneralStmt(std::make_unique<T>());
    genStmtTail->InsertBefore(ptrStmt);
    mapIdxStmt[idx] = ptrStmt;
    return static_cast<T*>(ptrStmt);
  }

  template <typename T>
  T *NewGenStmt(GeneralStmtKind genKind, uint32 idx) {
    // T *stmt = new T(0);
    GeneralStmt *ptrStmt = RegisterGeneralStmt(std::make_unique<T>(genKind));
    genStmtTail->InsertBefore(ptrStmt);
    mapIdxStmt[idx] = ptrStmt;
    return static_cast<T*>(ptrStmt);
  }

 private:
  MapleMap<uint32, GeneralStmt*> mapIdxStmt;
};

class FEFunctionTest : public testing::Test, public RedirectBuffer {
 public:
  static MemPool *mp;
  MapleAllocator allocator;
  MIRFunction func;
  FEFunctionDemo demoFunc;
  FEFunctionTest()
      : allocator(mp),
        func(&MPLFEUTEnvironment::GetMIRModule(), StIdx(0, 0)),
        demoFunc(allocator, func) {}
  ~FEFunctionTest() = default;

  static void SetUpTestCase() {
    mp = memPoolCtrler.NewMemPool("MemPool for FEFunctionTest");
  }

  static void TearDownTestCase() {
    memPoolCtrler.DeleteMemPool(mp);
    mp = nullptr;
  }
};
MemPool *FEFunctionTest::mp = nullptr;

/* GenStmtDemo1:BB
 * 0   StmtHead
 * 1   Stmt (fallthru = true)
 * 2   Stmt (fallthru = false)
 * 3   StmtTail
 */
void FEFunctionDemo::LoadGenStmtDemo1() {
  Init();
  mapIdxStmt.clear();
  (void)NewGenStmt<GeneralStmt>(1);
  GeneralStmt *stmt2 = NewGenStmt<GeneralStmt>(2);
  stmt2->SetFallThru(false);
}

TEST_F(FEFunctionTest, GeneralBBBuildForBB) {
  demoFunc.LoadGenStmtDemo1();
  bool resultBB = demoFunc.BuildGeneralBB("build Demo1 bb");
  ASSERT_EQ(resultBB, true);
  bool resultCFG = demoFunc.BuildGeneralCFG("build Demo1 cfg");
  ASSERT_EQ(resultCFG, true);
  demoFunc.LabelGenStmt();
  demoFunc.LabelGenBB();
  GeneralBB *bb1 = static_cast<GeneralBB*>(demoFunc.genBBHead->GetNext());
  EXPECT_EQ(bb1->GetNext(), demoFunc.genBBTail);
  EXPECT_EQ(bb1->GetStmtHead()->GetID(), 1);
  EXPECT_EQ(bb1->GetStmtTail()->GetID(), 2);
  EXPECT_EQ(demoFunc.HasDeadBB(), false);
}

/* GenStmtDemo2:BB_StmtAux
 * 0   StmtHead
 * 1   StmtAuxPre
 * 2   Stmt (fallthru = true)
 * 3   Stmt (fallthru = false)
 * 4   StmtAuxPost
 * 5   StmtTail
 */
void FEFunctionDemo::LoadGenStmtDemo2() {
  Init();
  mapIdxStmt.clear();
  (void)NewGenStmt<GeneralStmtAuxPre>(1);
  (void)NewGenStmt<GeneralStmt>(2);
  GeneralStmt *stmt3 = NewGenStmt<GeneralStmt>(3);
  (void)NewGenStmt<GeneralStmtAuxPost>(4);
  stmt3->SetFallThru(false);
}

TEST_F(FEFunctionTest, GeneralBBBuildForBB_StmtAux) {
  demoFunc.LoadGenStmtDemo2();
  bool resultBB = demoFunc.BuildGeneralBB("build Demo2 bb");
  ASSERT_EQ(resultBB, true);
  bool resultCFG = demoFunc.BuildGeneralCFG("build Demo2 cfg");
  ASSERT_EQ(resultCFG, true);
  demoFunc.LabelGenStmt();
  demoFunc.LabelGenBB();
  GeneralBB *bb1 = static_cast<GeneralBB*>(demoFunc.genBBHead->GetNext());
  EXPECT_EQ(bb1->GetNext(), demoFunc.genBBTail);
  EXPECT_EQ(bb1->GetStmtHead()->GetID(), 1);
  EXPECT_EQ(bb1->GetStmtTail()->GetID(), 4);
  EXPECT_EQ(bb1->GetStmtNoAuxHead()->GetID(), 2);
  EXPECT_EQ(bb1->GetStmtNoAuxTail()->GetID(), 3);
  EXPECT_EQ(demoFunc.HasDeadBB(), false);
}

/* GenStmtDemo3:CFG
 *     --- BB0 ---
 * 0   StmtHead
 *     --- BB1 ---
 * 1   StmtAuxPre
 * 2   StmtMultiOut (fallthru = true, out = {8})
 * 3   StmtAuxPost
 *     --- BB2 ---
 * 4   StmtAuxPre
 * 5   Stmt (fallthru = false)
 * 6   StmtAuxPost
 *     --- BB3 ---
 * 7   StmtAuxPre
 * 8   StmtMultiIn (fallthru = true, in = {2})
 * 9   Stmt (fallthru = false)
 * 10  StmtAuxPos
 *     --- BB4 ---
 * 11  StmtTail
 *
 * GenStmtDemo3_CFG:
 *      BB0
 *       |
 *      BB1
 *      / \
 *   BB2   BB3
 */
void FEFunctionDemo::LoadGenStmtDemo3() {
  Init();
  mapIdxStmt.clear();
  // --- BB1 ---
  (void)NewGenStmt<GeneralStmtAuxPre>(1);
  GeneralStmt *stmt2 = NewGenStmt<GeneralStmt>(GeneralStmtKind::kStmtMultiOut, 2);
  (void)NewGenStmt<GeneralStmtAuxPost>(3);
  // --- BB2 ---
  (void)NewGenStmt<GeneralStmtAuxPre>(4);
  GeneralStmt *stmt5 = NewGenStmt<GeneralStmt>(5);
  stmt5->SetFallThru(false);
  (void)NewGenStmt<GeneralStmtAuxPost>(6);
  // --- BB3 ---
  (void)NewGenStmt<GeneralStmtAuxPre>(7);
  GeneralStmt *stmt8 = NewGenStmt<GeneralStmt>(GeneralStmtKind::kStmtMultiIn, 8);
  GeneralStmt *stmt9 = NewGenStmt<GeneralStmt>(9);
  stmt9->SetFallThru(false);
  (void)NewGenStmt<GeneralStmtAuxPost>(10);
  // Link
  stmt2->AddSucc(stmt8);
  stmt8->AddPred(stmt2);
}

TEST_F(FEFunctionTest, GeneralBBBuildForCFG) {
  demoFunc.LoadGenStmtDemo3();
  bool resultBB = demoFunc.BuildGeneralBB("build Demo3 bb");
  ASSERT_EQ(resultBB, true);
  bool resultCFG = demoFunc.BuildGeneralCFG("build Demo3 cfg");
  ASSERT_EQ(resultCFG, true);
  demoFunc.LabelGenStmt();
  demoFunc.LabelGenBB();
  // Check BB
  GeneralBB *bb1 = static_cast<GeneralBB*>(demoFunc.genBBHead->GetNext());
  ASSERT_NE(bb1, demoFunc.genBBTail);
  GeneralBB *bb2 = static_cast<GeneralBB*>(bb1->GetNext());
  ASSERT_NE(bb2, demoFunc.genBBTail);
  GeneralBB *bb3 = static_cast<GeneralBB*>(bb2->GetNext());
  ASSERT_NE(bb3, demoFunc.genBBTail);
  // Check BB's detail
  EXPECT_EQ(bb1->GetStmtHead()->GetID(), 1);
  EXPECT_EQ(bb1->GetStmtNoAuxHead()->GetID(), 2);
  EXPECT_EQ(bb1->GetStmtNoAuxTail()->GetID(), 2);
  EXPECT_EQ(bb1->GetStmtTail()->GetID(), 3);
  EXPECT_EQ(bb2->GetStmtHead()->GetID(), 4);
  EXPECT_EQ(bb2->GetStmtNoAuxHead()->GetID(), 5);
  EXPECT_EQ(bb2->GetStmtNoAuxTail()->GetID(), 5);
  EXPECT_EQ(bb2->GetStmtTail()->GetID(), 6);
  EXPECT_EQ(bb3->GetStmtHead()->GetID(), 7);
  EXPECT_EQ(bb3->GetStmtNoAuxHead()->GetID(), 8);
  EXPECT_EQ(bb3->GetStmtNoAuxTail()->GetID(), 9);
  EXPECT_EQ(bb3->GetStmtTail()->GetID(), 10);
  // Check CFG
  EXPECT_EQ(bb1->GetPredBBs().size(), 1);
  EXPECT_EQ(bb1->IsPredBB(0U), true);
  EXPECT_EQ(bb1->GetSuccBBs().size(), 2);
  EXPECT_EQ(bb1->IsSuccBB(2), true);
  EXPECT_EQ(bb1->IsSuccBB(3), true);
  EXPECT_EQ(bb2->GetPredBBs().size(), 1);
  EXPECT_EQ(bb2->IsPredBB(1), true);
  EXPECT_EQ(bb2->GetSuccBBs().size(), 0);
  EXPECT_EQ(bb3->GetPredBBs().size(), 1);
  EXPECT_EQ(bb3->IsPredBB(1), true);
  EXPECT_EQ(bb3->GetSuccBBs().size(), 0);
  EXPECT_EQ(demoFunc.HasDeadBB(), false);
}

/* GenStmtDemo4:CFG_Fail
 * 0   StmtHead
 * 1   Stmt (fallthru = true)
 * 2   Stmt (fallthru = true)
 * 3   StmtTail
 */
void FEFunctionDemo::LoadGenStmtDemo4() {
  Init();
  mapIdxStmt.clear();
  (void)NewGenStmt<GeneralStmt>(1);
  (void)NewGenStmt<GeneralStmt>(2);
}

TEST_F(FEFunctionTest, GeneralBBBuildForCFG_Fail) {
  demoFunc.LoadGenStmtDemo4();
  bool resultBB = demoFunc.BuildGeneralBB("build Demo4 bb");
  ASSERT_EQ(resultBB, true);
  bool resultCFG = demoFunc.BuildGeneralCFG("build Demo4 cfg");
  ASSERT_EQ(resultCFG, false);
}

/* GenStmtDemo5:CFG_DeadBB
 *     --- BB0 ---
 * 0   StmtHead
 *     --- BB1 ---
 * 1   Stmt (fallthru = true)
 * 2   Stmt (fallthru = false)
 *     --- BB2 ---
 * 3   Stmt (fallthru = false)
 *     --- BB3 ---
 * 4   StmtTail
 *
 * GenStmtDemo5_CFG:
 *      BB0
 *       |
 *      BB1  BB2(DeadBB)
 */
void FEFunctionDemo::LoadGenStmtDemo5() {
  Init();
  mapIdxStmt.clear();
  (void)NewGenStmt<GeneralStmt>(1);
  GeneralStmt *stmt2 = NewGenStmt<GeneralStmt>(2);
  stmt2->SetFallThru(false);
  GeneralStmt *stmt3 = NewGenStmt<GeneralStmt>(3);
  stmt3->SetFallThru(false);
}

TEST_F(FEFunctionTest, GeneralBBBuildForCFG_DeadBB) {
  demoFunc.LoadGenStmtDemo5();
  bool resultBB = demoFunc.BuildGeneralBB("build Demo5 bb");
  ASSERT_EQ(resultBB, true);
  bool resultCFG = demoFunc.BuildGeneralCFG("build Demo5 cfg");
  ASSERT_EQ(resultCFG, true);
  demoFunc.LabelGenStmt();
  demoFunc.LabelGenBB();
  // Check BB
  GeneralBB *bb1 = static_cast<GeneralBB*>(demoFunc.genBBHead->GetNext());
  ASSERT_NE(bb1, demoFunc.genBBTail);
  GeneralBB *bb2 = static_cast<GeneralBB*>(bb1->GetNext());
  ASSERT_NE(bb2, demoFunc.genBBTail);
  // Check BB's detail
  EXPECT_EQ(bb1->GetStmtHead()->GetID(), 1);
  EXPECT_EQ(bb1->GetStmtTail()->GetID(), 2);
  EXPECT_EQ(bb2->GetStmtHead()->GetID(), 3);
  EXPECT_EQ(bb2->GetStmtTail()->GetID(), 3);
  // Check CFG
  EXPECT_EQ(bb1->GetPredBBs().size(), 1);
  EXPECT_EQ(bb1->IsPredBB(0U), true);
  EXPECT_EQ(bb2->GetSuccBBs().size(), 0);
  EXPECT_EQ(demoFunc.HasDeadBB(), true);
}
}  // namespace maple