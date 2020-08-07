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
#ifndef MPLFE_INCLUDE_COMMON_FE_FUNCTION_H
#define MPLFE_INCLUDE_COMMON_FE_FUNCTION_H
#include <memory>
#include <vector>
#include <list>
#include "types_def.h"
#include "mempool_allocator.h"
#include "safe_ptr.h"
#include "mir_function.h"
#include "fe_utils.h"
#include "general_stmt.h"
#include "general_bb.h"
#include "feir_stmt.h"
#include "feir_bb.h"
#include "fe_timer_ns.h"
#include "general_cfg.h"
#include "fe_function_phase_result.h"

namespace maple {
class FEFunction {
 public:
  FEFunction(MIRFunction &argMIRFunction, const std::unique_ptr<FEFunctionPhaseResult> &argPhaseResultTotal);
  virtual ~FEFunction();
  void LabelGenStmt();
  void LabelGenBB();
  bool HasDeadBB();

  // element memory manage method
  GeneralStmt *RegisterGeneralStmt(std::unique_ptr<GeneralStmt> stmt);
  const std::unique_ptr<GeneralStmt> &RegisterGeneralStmtUniqueReturn(std::unique_ptr<GeneralStmt> stmt);
  GeneralBB *RegisterGeneralBB(std::unique_ptr<GeneralBB> bb);
  FEIRStmt *RegisterFEIRStmt(UniqueFEIRStmt stmt);
  GeneralBB *RegisterFEIRBB(std::unique_ptr<GeneralBB> bb);

  void SetSrcFileName(const std::string &fileName) {
    srcFileName = fileName;
  }

  void Init() {
    InitImpl();
  }

  void PreProcess() {
    PreProcessImpl();
  }

  void Process() {
    ProcessImpl();
  }

  void Finish() {
    FinishImpl();
  }

 LLT_PROTECTED:
  // run phase routines
  virtual bool GenerateGeneralStmt(const std::string &phaseName) = 0;
  virtual bool BuildGeneralBB(const std::string &phaseName);
  virtual bool BuildGeneralCFG(const std::string &phaseName);
  virtual bool CheckDeadBB(const std::string &phaseName);
  virtual bool LabelGeneralStmts(const std::string &phaseName);
  virtual bool LabelGeneralBBs(const std::string &phaseName);
  bool ProcessFEIRFunction();
  virtual bool GenerateArgVarList(const std::string &phaseName) = 0;
  virtual bool EmitToFEIRStmt(const std::string &phaseName) = 0;
  bool BuildMapLabelStmt(const std::string &phaseName);
  bool SetupFEIRStmtJavaTry(const std::string &phaseName);
  bool SetupFEIRStmtBranch(const std::string &phaseName);

  // finish phase routines
  bool BuildGeneralStmtBBMap(const std::string &phaseName);
  bool UpdateFormal(const std::string &phaseName);
  bool EmitToMIR(const std::string &phaseName);
  bool ReleaseGenStmts(const std::string &phaseName);

  // interface methods
  virtual void InitImpl();
  virtual void PreProcessImpl() {}
  virtual void ProcessImpl() {}
  virtual void FinishImpl() {}
  virtual bool PreProcessTypeNameIdx() = 0;
  virtual void GenerateGeneralStmtFailCallBack() = 0;
  virtual void GenerateGeneralDebugInfo() = 0;
  virtual bool VerifyGeneral() = 0;
  virtual void VerifyGeneralFailCallBack() = 0;
  virtual void DumpGeneralStmts();
  virtual void DumpGeneralBBs();
  virtual void DumpGeneralCFGGraph();
  virtual std::string GetGeneralFuncName() const;
  void EmitToMIRStmt();

  virtual GeneralBB *NewGeneralBB();
  virtual GeneralBB *NewGeneralBB(uint8 kind);
  void PhaseTimerStart(FETimerNS &timer);
  void PhaseTimerStopAndDump(FETimerNS &timer, const std::string &label);
  virtual void DumpGeneralCFGGraphForBB(std::ofstream &file, const GeneralBB &bb);
  virtual void DumpGeneralCFGGraphForCFGEdge(std::ofstream &file);
  virtual void DumpGeneralCFGGraphForDFGEdge(std::ofstream &file);
  virtual bool HasThis() = 0;
  void BuildMapLabelIdx();

  GeneralStmt *genStmtHead;
  GeneralStmt *genStmtTail;
  std::list<GeneralStmt*> genStmtListRaw;
  GeneralBB *genBBHead;
  GeneralBB *genBBTail;
  std::unique_ptr<GeneralCFG> generalCFG;
  FEIRStmt *feirStmtHead;
  FEIRStmt *feirStmtTail;
  GeneralBB *feirBBHead;
  GeneralBB *feirBBTail;
  std::unique_ptr<GeneralCFG> feirCFG;
  std::map<const GeneralStmt*, GeneralBB*> genStmtBBMap;
  std::list<std::unique_ptr<FEIRVar>> argVarList;
  std::map<uint32, LabelIdx> mapLabelIdx;
  std::map<uint32, FEIRStmtPesudoLabel*> mapLabelStmt;
  FEFunctionPhaseResult phaseResult;
  const std::unique_ptr<FEFunctionPhaseResult> &phaseResultTotal;
  std::string srcFileName = "";

 LLT_PRIVATE:
  void BuildFEIRBB();
  void BuildFEIRCFG();
  void BuildFEIRDFG();
  void BuildFEIRUDDU();
  void TypeInfer();
  bool SetupFEIRStmtGoto(FEIRStmtGoto &stmt);
  bool SetupFEIRStmtSwitch(FEIRStmtSwitch &stmt);
  FEIRStmtPesudoLOC *GetLOCForStmt(const FEIRStmt &feStmt);

  std::list<std::unique_ptr<GeneralStmt>> genStmtList;
  std::list<std::unique_ptr<GeneralBB>> genBBList;
  std::list<UniqueFEIRStmt> feirStmtList;
  std::list<std::unique_ptr<GeneralBB>> feirBBList;
  MIRFunction &mirFunction;
};
}  // namespace maple
#endif  // MPLFE_INCLUDE_COMMON_FE_FUNCTION_H