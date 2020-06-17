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
#ifndef MAPLEBE_INCLUDE_CG_CG_H
#define MAPLEBE_INCLUDE_CG_CG_H

/* C++ headers. */
#include <cstddef>
#include <string>
/* MapleIR headers. */
#include "operand.h"
#include "insn.h"
#include "cgfunc.h"
#include "cg_option.h"
#include "opcode_info.h"
#include "global_tables.h"
#include "mir_function.h"
#include "mad.h"

namespace maplebe {
class Globals {
 public:
  static Globals *GetInstance() {
    static Globals instance;
    return &instance;
  }

  ~Globals() = default;

  void SetBECommon(BECommon &bc) {
    beCommon = &bc;
  }

  BECommon *GetBECommon() {
    return beCommon;
  }

  const BECommon *GetBECommon() const {
    return beCommon;
  }

  void SetMAD(MAD &m) {
    mad = &m;
  }

  MAD *GetMAD() {
    return mad;
  }

  const MAD *GetMAD() const {
    return mad;
  }

  void SetOptimLevel(int32 opLevel) {
    optimLevel = opLevel;
  }

  int32 GetOptimLevel() const {
    return optimLevel;
  }

  void ReleaseMAD() {
    if (mad != nullptr) {
      delete mad;
      mad = nullptr;
    }
  }

  void ReleaseBECommon() {
    if (beCommon != nullptr) {
      delete beCommon;
      beCommon = nullptr;
    }
  }

 private:
  BECommon *beCommon = nullptr;
  MAD *mad = nullptr;
  int32 optimLevel = 0;
  Globals() = default;
};

class CG {
 public:
  using GenerateFlag = uint64;

 public:
  CG(MIRModule &mod, const CGOptions &cgOptions)
      : memPool(memPoolCtrler.NewMemPool("maplecg mempool")),
        allocator(memPool),
        mirModule(&mod),
        emitter(nullptr),
        labelOrderCnt(0),
        cgOption(cgOptions),
        instrumentationFunction(nullptr) {
    const std::string &internalNameLiteral = namemangler::GetInternalNameLiteral(namemangler::kJavaLangObjectStr);
    GStrIdx strIdxFromName = GlobalTables::GetStrTable().GetStrIdxFromName(internalNameLiteral);
    isLibcore = (GlobalTables::GetGsymTable().GetSymbolFromStrIdx(strIdxFromName) != nullptr);
    DefineDebugTraceFunctions();
  }

  virtual ~CG();
  void GenExtraTypeMetadata(const std::string &classListFileName, const std::string &outputBaseName);
  void GenPrimordialObjectList(const std::string &outputBaseName);
  const std::string ExtractFuncName(const std::string &str);

  template <typename I>
  Insn &BuildInstruction(MOperator opCode) {
    currentCGFunction->IncTotalNumberOfInstructions();
    MemPool *memPool = currentCGFunction->GetMemoryPool();
    Insn *insn = memPool->New<I>(*memPool, opCode);
    return *insn;
  }

  template <typename I>
  Insn &BuildInstruction(MOperator opCode, Operand &opnd0) {
    currentCGFunction->IncTotalNumberOfInstructions();
    MemPool *memPool = currentCGFunction->GetMemoryPool();
    Insn *insn = memPool->New<I>(*memPool, opCode);
    insn->AddOperand(opnd0);
    return *insn;
  }

  template <typename I>
  Insn &BuildInstruction(MOperator opCode, Operand &opnd0, Operand &opnd1) {
    currentCGFunction->IncTotalNumberOfInstructions();
    MemPool *memPool = currentCGFunction->GetMemoryPool();
    Insn *insn = memPool->New<I>(*memPool, opCode);
    insn->AddOperand(opnd0);
    insn->AddOperand(opnd1);
    return *insn;
  }

  template <typename I>
  Insn &BuildInstruction(MOperator opCode, Operand &opnd0, Operand &opnd1, Operand &opnd2) {
    currentCGFunction->IncTotalNumberOfInstructions();
    MemPool *memPool = currentCGFunction->GetMemoryPool();
    Insn *insn = memPool->New<I>(*memPool, opCode);
    insn->AddOperand(opnd0);
    insn->AddOperand(opnd1);
    insn->AddOperand(opnd2);
    return *insn;
  }

  template <typename I>
  Insn &BuildInstruction(MOperator opCode, Operand &opnd0, Operand &opnd1, Operand &opnd2, Operand &opnd3) {
    currentCGFunction->IncTotalNumberOfInstructions();
    MemPool *memPool = currentCGFunction->GetMemoryPool();
    Insn *insn = memPool->New<I>(*memPool, opCode);
    insn->AddOperand(opnd0);
    insn->AddOperand(opnd1);
    insn->AddOperand(opnd2);
    insn->AddOperand(opnd3);
    return *insn;
  }

  template <typename I>
  Insn &BuildInstruction(MOperator opCode, Operand &opnd0, Operand &opnd1, Operand &opnd2, Operand &opnd3,
                         Operand &opnd4) {
    currentCGFunction->IncTotalNumberOfInstructions();
    MemPool *memPool = currentCGFunction->GetMemoryPool();
    Insn *insn = memPool->New<I>(*memPool, opCode);
    insn->AddOperand(opnd0);
    insn->AddOperand(opnd1);
    insn->AddOperand(opnd2);
    insn->AddOperand(opnd3);
    insn->AddOperand(opnd4);
    return *insn;
  }

  template <typename I>
  Insn &BuildInstruction(MOperator opCode, std::vector<Operand*> &opnds) {
    currentCGFunction->IncTotalNumberOfInstructions();
    MemPool *memPool = currentCGFunction->GetMemoryPool();
    Insn *insn = memPool->New<I>(*memPool, opCode);
    for (auto *opnd : opnds) {
      insn->AddOperand(*opnd);
    }
    return *insn;
  }

  virtual CGFunc *CreateCGFunc(MIRModule &mod, MIRFunction&, BECommon&, MemPool&, MapleAllocator&, uint32) = 0;

  bool IsExclusiveEH() const {
    return CGOptions::IsExclusiveEH();
  }

  bool IsQuiet() const {
    return CGOptions::IsQuiet();
  }

  virtual bool IsExclusiveFunc(MIRFunction &mirFunc) = 0;

  /* NOTE: Consider making be_common a field of CG. */
  virtual void GenerateObjectMaps(BECommon &beCommon) = 0;

  /* Used for GCTIB pattern merging */
  virtual std::string FindGCTIBPatternName(const std::string &name) const = 0;

  bool GenerateVerboseAsm() const {
    return cgOption.GenerateVerboseAsm();
  }

  bool GenerateVerboseCG() const {
    return cgOption.GenerateVerboseCG();
  }

  bool DoPrologueEpilogue() const {
    return cgOption.DoPrologueEpilogue();
  }

  bool DoCheckSOE() const {
    return cgOption.DoCheckSOE();
  }

  bool GenerateDebugFriendlyCode() const {
    return cgOption.GenerateDebugFriendlyCode();
  }

  int32 GetOptimizeLevel() const {
    return cgOption.GetOptimizeLevel();
  }

  bool UseFastUnwind() const {
    return true;
  }

  bool AddStackGuard() const {
    return cgOption.AddStackGuard();
  }

  bool NeedInsertInstrumentationFunction() const {
    return cgOption.NeedInsertInstrumentationFunction();
  }

  void SetInstrumentationFunction(const std::string &name);
  const MIRSymbol *GetInstrumentationFunction() const {
    return instrumentationFunction;
  }

  bool InstrumentWithDebugTraceCall() const {
    return cgOption.InstrumentWithDebugTraceCall();
  }

  bool InstrumentWithProfile() const {
    return cgOption.InstrumentWithProfile();
  }

  bool GenYieldPoint() const {
    return cgOption.GenYieldPoint();
  }

  bool GenLocalRC() const {
    return cgOption.GenLocalRC();
  }

  bool GenerateExceptionHandlingCode() const {
    return cgOption.GenerateExceptionHandlingCode();
  }

  bool DoConstFold() const {
    return cgOption.DoConstFold();
  }

  void AddStackGuardvar();
  void DefineDebugTraceFunctions();
  MIRModule *GetMIRModule() {
    return mirModule;
  }

  void SetEmitter(Emitter &emitter) {
    this->emitter = &emitter;
  }

  Emitter *GetEmitter() const {
    return emitter;
  }

  void IncreaseLabelOrderCnt() {
    labelOrderCnt++;
  }

  LabelIDOrder GetLabelOrderCnt() const {
    return labelOrderCnt;
  }

  static void SetCurCGFunc(CGFunc &cgFunc) {
    currentCGFunction = &cgFunc;
  }

  static const CGFunc *GetCurCGFunc() {
    return currentCGFunction;
  }

  static CGFunc *GetCurCGFuncNoConst() {
    return currentCGFunction;
  }

  const CGOptions &GetCGOptions() const {
    return cgOption;
  }

  bool IsLibcore() const {
    return isLibcore;
  }

  MIRSymbol *GetDebugTraceEnterFunction() {
    return dbgTraceEnter;
  }

  const MIRSymbol *GetDebugTraceEnterFunction() const {
    return dbgTraceEnter;
  }

  MIRSymbol *GetProfileFunction() {
    return dbgFuncProfile;
  }

  const MIRSymbol *GetProfileFunction() const {
    return dbgFuncProfile;
  }

  const MIRSymbol *GetDebugTraceExitFunction() const {
    return dbgTraceExit;
  }

  /* Object map generation helper */
  std::vector<int64> GetReferenceOffsets64(const BECommon &beCommon, MIRStructType &structType);

 protected:
  MemPool *memPool;
  MapleAllocator allocator;

 private:
  MIRModule *mirModule;
  Emitter *emitter;
  LabelIDOrder labelOrderCnt;
  static CGFunc *currentCGFunction;  /* current cg function being compiled */
  CGOptions cgOption;
  bool isLibcore;
  MIRSymbol *instrumentationFunction;
  MIRSymbol *dbgTraceEnter;
  MIRSymbol *dbgTraceExit;
  MIRSymbol *dbgFuncProfile;
};  /* class CG */
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_CG_H */
