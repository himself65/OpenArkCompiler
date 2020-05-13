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
#ifndef MAPLEBE_INCLUDE_CG_CGFUNC_H
#define MAPLEBE_INCLUDE_CG_CGFUNC_H

#include "becommon.h"
#include "operand.h"
#include "eh_func.h"
#include "memlayout.h"
#include "cgbb.h"
#include "reg_alloc.h"
#include "cfi.h"
#include "cg_cfg.h"
/* MapleIR headers. */
#include "mir_parser.h"
#include "mir_function.h"

/* Maple MP header */
#include "mempool_allocator.h"

namespace maplebe {
constexpr int32 kFreqBase = 10000;
struct MemOpndCmp {
  bool operator()(const MemOperand *lhs, const MemOperand *rhs) const {
    CHECK_FATAL(lhs != nullptr, "null ptr check");
    CHECK_FATAL(rhs != nullptr, "null ptr check");
    if (lhs == rhs) {
      return false;
    }
    return (lhs->Less(*rhs));
  }
};

class SpillMemOperandSet {
 public:
  explicit SpillMemOperandSet(MapleAllocator &mallocator) : reuseSpillLocMem(mallocator.Adapter()) {}

  virtual ~SpillMemOperandSet() = default;

  void Add(MemOperand &op) {
    reuseSpillLocMem.insert(&op);
  }

  void Remove(MemOperand &op) {
    reuseSpillLocMem.erase(&op);
  }

  MemOperand *GetOne() {
    if (!reuseSpillLocMem.empty()) {
      MemOperand *res = *reuseSpillLocMem.begin();
      reuseSpillLocMem.erase(res);
      return res;
    }
    return nullptr;
  }

 private:
  MapleSet<MemOperand*, MemOpndCmp> reuseSpillLocMem;
};

constexpr uint32 kVRegisterNumber = 80;
class CGFunc {
 public:
  enum ShiftDirection : uint8 {
    kShiftLeft,
    kShiftAright,
    kShiftLright
  };

  CGFunc(MIRModule &mod, CG &cg, MIRFunction &mirFunc, BECommon &beCommon, MemPool &memPool,
         MapleAllocator &mallocator);
  virtual ~CGFunc();

  const std::string &GetName() const {
    return func.GetName();
  }

  const MapleMap<LabelIdx, uint64> &GetLabelAndValueMap() const {
    return labelMap;
  }

  void InsertLabelMap(LabelIdx idx, uint64 value) {
    ASSERT(labelMap.find(idx) == labelMap.end(), "idx already exist");
    labelMap[idx] = value;
  }

  void LayoutStackFrame() {
    CHECK_FATAL(memLayout != nullptr, "memLayout should has been initialized in constructor");
    memLayout->LayoutStackFrame();
  }

  bool HasCall() const {
    return func.HasCall();
  }

  bool HasVLAOrAlloca() const {
    return false;
  }


  EHFunc *BuildEHFunc();
  virtual void GenSaveMethodInfoCode(BB &bb) = 0;
  virtual void GenerateCleanupCode(BB &bb) = 0;
  virtual bool NeedCleanup() = 0;
  virtual void GenerateCleanupCodeForExtEpilog(BB &bb) = 0;

  void GenerateInstruction();
  bool MemBarOpt(StmtNode &membar);
  void HandleFunction();
  void ProcessExitBBVec();
  virtual void MergeReturn() = 0;
  void TraverseAndClearCatchMark(BB &bb);
  void MarkCatchBBs();
  void MarkCleanupEntryBB();
  void SetCleanupLabel(BB &cleanupEntry);
  bool ExitbbNotInCleanupArea(const BB &bb) const;
  uint32 GetMaxRegNum() const {
    return maxRegCount;
  };
  void DumpCFG() const;
  void DumpCGIR() const;
  void DumpLoop() const;
  void ClearLoopInfo();
  Operand *HandleExpr(const BaseNode &parent, BaseNode &expr);
  virtual void DetermineReturnTypeofCall() = 0;
  /* handle rc reset */
  virtual void HandleRCCall(bool begin, const MIRSymbol *retRef = nullptr) = 0;
  virtual void HandleRetCleanup(NaryStmtNode &retNode) = 0;
  /* select stmt */
  virtual void SelectDassign(DassignNode &stmt, Operand &opnd0) = 0;
  virtual void SelectRegassign(RegassignNode &stmt, Operand &opnd0) = 0;
  virtual void SelectAssertNull(UnaryStmtNode &stmt) = 0;
  virtual void SelectAggDassign(DassignNode &stmt) = 0;
  virtual void SelectIassign(IassignNode &stmt) = 0;
  virtual void SelectAggIassign(IassignNode &stmt, Operand &lhsAddrOpnd) = 0;
  virtual void SelectReturn(Operand *opnd) = 0;
  virtual void SelectCondGoto(CondGotoNode &stmt, Operand &opnd0, Operand &opnd1) = 0;
  virtual void SelectCondSpecialCase1(CondGotoNode &stmt, BaseNode &opnd0) = 0;
  virtual void SelectCondSpecialCase2(const CondGotoNode &stmt, BaseNode &opnd0) = 0;
  virtual void SelectGoto(GotoNode &stmt) = 0;
  virtual void SelectCall(CallNode &callNode) = 0;
  virtual void SelectIcall(IcallNode &icallNode, Operand &fptrOpnd) = 0;
  virtual void SelectIntrinCall(IntrinsiccallNode &intrinsiccallNode) = 0;
  virtual void SelectMembar(StmtNode &membar) = 0;
  virtual void SelectComment(CommentNode &comment) = 0;
  virtual void HandleCatch() = 0;

  /* select expr */
  virtual Operand *SelectDread(AddrofNode &expr) = 0;
  virtual RegOperand *SelectRegread(RegreadNode &expr) = 0;
  virtual Operand *SelectAddrof(AddrofNode &expr) = 0;
  virtual Operand &SelectAddrofFunc(AddroffuncNode &expr) = 0;
  virtual Operand *SelectIread(const BaseNode &parent, IreadNode &expr) = 0;
  virtual Operand *SelectIntConst(MIRIntConst &intConst) = 0;
  virtual Operand *SelectFloatConst(MIRFloatConst &floatConst) = 0;
  virtual Operand *SelectDoubleConst(MIRDoubleConst &doubleConst) = 0;
  virtual Operand *SelectStrConst(MIRStrConst &strConst) = 0;
  virtual Operand *SelectStr16Const(MIRStr16Const &strConst) = 0;
  virtual void SelectAdd(Operand &resOpnd, Operand &opnd0, Operand &opnd1, PrimType primType) = 0;
  virtual Operand *SelectAdd(BinaryNode &node, Operand &opnd0, Operand &opnd1) = 0;
  virtual Operand &SelectCGArrayElemAdd(BinaryNode &node) = 0;
  virtual Operand *SelectShift(BinaryNode &node, Operand &opnd0, Operand &opnd1) = 0;
  virtual void SelectMpy(Operand &resOpnd, Operand &opnd0, Operand &opnd1, PrimType primType) = 0;
  virtual Operand *SelectMpy(BinaryNode &node, Operand &opnd0, Operand &opnd1) = 0;
  virtual Operand *SelectRem(BinaryNode &node, Operand &opnd0, Operand &opnd1) = 0;
  virtual void SelectDiv(Operand &resOpnd, Operand &opnd0, Operand &opnd1, PrimType primType) = 0;
  virtual Operand *SelectDiv(BinaryNode &node, Operand &opnd0, Operand &opnd1) = 0;
  virtual Operand *SelectSub(BinaryNode &node, Operand &opnd0, Operand &opnd1) = 0;
  virtual void SelectSub(Operand &resOpnd, Operand &opnd0, Operand &opnd1, PrimType primType) = 0;
  virtual Operand *SelectBand(BinaryNode &node, Operand &opnd0, Operand &opnd1) = 0;
  virtual void SelectBand(Operand &resOpnd, Operand &opnd0, Operand &opnd1, PrimType primType) = 0;
  virtual Operand *SelectLand(BinaryNode &node, Operand &opnd0, Operand &opnd1) = 0;
  virtual Operand *SelectLor(BinaryNode &node, Operand &opnd0, Operand &opnd1, bool parentIsBr = false) = 0;
  virtual void SelectMin(Operand &resOpnd, Operand &opnd0, Operand &opnd1, PrimType primType) = 0;
  virtual Operand *SelectMin(BinaryNode &node, Operand &opnd0, Operand &opnd1) = 0;
  virtual void SelectMax(Operand &resOpnd, Operand &opnd0, Operand &opnd1, PrimType primType) = 0;
  virtual Operand *SelectMax(BinaryNode &node, Operand &opnd0, Operand &opnd1) = 0;
  virtual Operand *SelectCmpOp(CompareNode &node, Operand &opnd0, Operand &opnd1) = 0;
  virtual Operand *SelectBior(BinaryNode &node, Operand &opnd0, Operand &opnd1) = 0;
  virtual void SelectBior(Operand &resOpnd, Operand &opnd0, Operand &opnd1, PrimType primType) = 0;
  virtual Operand *SelectBxor(BinaryNode &node, Operand &opnd0, Operand &opnd1) = 0;
  virtual void SelectBxor(Operand &resOpnd, Operand &opnd0, Operand &opnd1, PrimType primType) = 0;
  virtual Operand *SelectAbs(UnaryNode &node, Operand &opnd0) = 0;
  virtual Operand *SelectBnot(UnaryNode &node, Operand &opnd0) = 0;
  virtual Operand *SelectExtractbits(ExtractbitsNode &node, Operand &opnd0) = 0;
  virtual Operand *SelectDepositBits(DepositbitsNode &node, Operand &opnd0, Operand &opnd1) = 0;
  virtual void SelectDepositBits(Operand &resOpnd, Operand &opnd0, Operand &opnd1, uint32 boffset, uint32 bitSize,
                                 PrimType primType1) = 0;
  virtual Operand *SelectLnot(UnaryNode &node, Operand &opnd0) = 0;
  virtual Operand *SelectNeg(UnaryNode &node, Operand &opnd0) = 0;
  virtual Operand *SelectRecip(UnaryNode &node, Operand &opnd0) = 0;
  virtual Operand *SelectSqrt(UnaryNode &node, Operand &opnd0) = 0;
  virtual Operand *SelectCeil(TypeCvtNode &node, Operand &opnd0) = 0;
  virtual Operand *SelectFloor(TypeCvtNode &node, Operand &opnd0) = 0;
  virtual Operand *SelectRetype(TypeCvtNode &node, Operand &opnd0) = 0;
  virtual Operand *SelectRound(TypeCvtNode &node, Operand &opnd0) = 0;
  virtual Operand *SelectCvt(const BaseNode &parent, TypeCvtNode &node, Operand &opnd0) = 0;
  virtual Operand *SelectTrunc(TypeCvtNode &node, Operand &opnd0) = 0;
  virtual Operand *SelectSelect(TernaryNode &node, Operand &opnd0, Operand &opnd1, Operand &opnd2) = 0;
  virtual Operand *SelectMalloc(UnaryNode &call, Operand &opnd0) = 0;
  virtual RegOperand &SelectCopy(Operand &src, PrimType srcType, PrimType dstType) = 0;
  virtual void SelectTargetFPCmpQuiet(Operand &o0, Operand &o1, uint32 size) = 0;
  virtual Operand *SelectAlloca(UnaryNode &call, Operand &opnd0) = 0;
  virtual Operand *SelectGCMalloc(GCMallocNode &call) = 0;
  virtual Operand *SelectJarrayMalloc(JarrayMallocNode &call, Operand &opnd0) = 0;
  virtual void SelectSelect(Operand &resOpnd, Operand &condOpnd, Operand &trueOpnd, Operand &falseOpnd,
                            PrimType dstType, PrimType primType) = 0;
  virtual void SelectRangeGoto(RangeGotoNode &rangeGotoNode, Operand &opnd0) = 0;
  virtual Operand *SelectLazyLoad(Operand &opnd0, PrimType primType) = 0;
  virtual Operand *SelectLazyLoadStatic(MIRSymbol &st, int64 offset, PrimType primType) = 0;
  virtual void GenerateYieldpoint(BB &bb) = 0;

  virtual Operand &GetOrCreateRflag() = 0;
  virtual const Operand *GetRflag() const = 0;
  virtual const LabelOperand *GetLabelOperand(LabelIdx labIdx) const = 0;
  virtual LabelOperand &GetOrCreateLabelOperand(LabelIdx labIdx) = 0;
  virtual LabelOperand &GetOrCreateLabelOperand(BB &bb) = 0;
  virtual RegOperand &CreateVirtualRegisterOperand(regno_t vRegNO) = 0;
  virtual RegOperand &GetOrCreateVirtualRegisterOperand(regno_t vRegNO) = 0;
  virtual RegOperand &GetOrCreateFramePointerRegOperand() = 0;
  virtual RegOperand &GetOrCreateStackBaseRegOperand() = 0;
  virtual int32 GetBaseOffset(const SymbolAlloc &symbolAlloc) = 0;
  virtual Operand &GetZeroOpnd(uint32 size) = 0;
  virtual Operand &CreateCfiRegOperand(uint32 reg, uint32 size) = 0;
  virtual Operand &GetTargetRetOperand(PrimType primType) = 0;
  virtual Operand &CreateImmOperand(PrimType primType, int64 val) = 0;
  virtual Operand *CreateZeroOperand(PrimType primType) = 0;

  virtual bool IsFrameReg(const RegOperand &opnd) const = 0;
  /* For ebo issue. */
  virtual Operand *GetTrueOpnd() {
    return nullptr;
  }
  virtual void ClearUnreachableGotInfos(BB &bb) {
    (void)bb;
  };
  virtual void SplitStrLdrPair() {}
  LabelIdx CreateLabel();

  virtual Operand &CreateFPImmZero(PrimType primType) = 0;

  RegOperand *GetVirtualRegisterOperand(regno_t vRegNO) {
    auto it = vRegOperandTable.find(vRegNO);
    ASSERT(it != vRegOperandTable.end(), "");
    return it->second;
  }

  Operand &CreateCfiImmOperand(int64 val, uint32 size) {
    return *memPool->New<cfi::ImmOperand>(val, size);
  }

  Operand &CreateCfiStrOperand(const std::string &str) {
    return *memPool->New<cfi::StrOperand>(str, *memPool);
  }

  bool IsSpecialPseudoRegister(PregIdx spr) const {
    return spr < 0;
  }

  regno_t NewVReg(RegType regType, uint32 size) {
    /* when vRegCount reach to maxRegCount, maxRegCount limit adds 80 every time */
    /* and vRegTable increases 80 elements. */
    if (vRegCount >= maxRegCount) {
      maxRegCount += kRegIncrStepLen;
      vRegTable.resize(maxRegCount);
    }
#if TARGAARCH64 || TARGX86_64
    if (size < k4ByteSize) {
      size = k4ByteSize;
    }
    ASSERT(size == k4ByteSize || size == k8ByteSize, "check size");
#endif
    new (&vRegTable[vRegCount]) VirtualRegNode(regType, size);
    return vRegCount++;
  }

  virtual regno_t NewVRflag() {
    return 0;
  }

  RegType GetRegTyFromPrimTy(PrimType primType) {
    switch (primType) {
      case PTY_u1:
      case PTY_i8:
      case PTY_u8:
      case PTY_i16:
      case PTY_u16:
      case PTY_i32:
      case PTY_u32:
      case PTY_i64:
      case PTY_u64:
      case PTY_a32:
      case PTY_a64:
      case PTY_ptr:
        return kRegTyInt;
      case PTY_f32:
      case PTY_f64:
        return kRegTyFloat;
      default:
        ASSERT(false, "Unexpected pty");
        return kRegTyUndef;
    }
  }

  /* return Register Type */
  RegType GetRegisterType(regno_t rNum) const {
    CHECK(rNum < vRegTable.size(), "index out of range in GetVRegSize");
    return vRegTable[rNum].GetType();
  }

  uint32 GetMaxVReg() const {
    return vRegCount;
  }

  uint32 GetVRegSize(regno_t vregNum) {
    CHECK(vregNum < vRegTable.size(), "index out of range in GetVRegSize");
    return vRegTable[vregNum].GetSize();
  }

  MIRSymbol *GetRetRefSymbol(BaseNode &expr);
  void GenerateCfiPrologEpilog();

  uint32 NumBBs() const {
    return bbCnt;
  }

#if DEBUG
  StIdx GetLocalVarReplacedByPreg(PregIdx reg) {
    auto it = pregsToVarsMap->find(reg);
    return it != pregsToVarsMap->end() ? it->second : StIdx();
  }
#endif

  void IncTotalNumberOfInstructions() {
    totalInsns++;
  }

  void DecTotalNumberOfInstructions() {
    totalInsns--;
  }

  int32 GetTotalNumberOfInstructions() const {
    return totalInsns;
  }

  virtual void ProcessLazyBinding() = 0;

  /* Debugging support */
  void SetDebugInfo(DebugInfo *dbgInfo) {
    debugInfo = dbgInfo;
  }

  /* Get And Set private members */
  CG *GetCG() {
    return cg;
  }

  const CG *GetCG() const {
    return cg;
  }

  const MIRModule &GetMirModule() const {
    return mirModule;
  }

  template <typename T>
  MIRConst *NewMirConst(T &mirConst) {
    MIRConst *newConst = mirModule.GetMemPool()->New<T>(mirConst.GetValue(), mirConst.GetType());
    return newConst;
  }

  uint32 GetMIRSrcFileEndLineNum() const {
    auto &srcFileInfo = mirModule.GetSrcFileInfo();
    if (!srcFileInfo.empty()) {
      return srcFileInfo.back().second;
    } else {
      return 0;
    }
  }

  MIRFunction &GetFunction() {
    return func;
  }

  const MIRFunction &GetFunction() const {
    return func;
  }

  EHFunc *GetEHFunc() {
    return ehFunc;
  }

  const EHFunc *GetEHFunc() const {
    return ehFunc;
  }

  void SetEHFunc(EHFunc &ehFunction) {
    ehFunc = &ehFunction;
  }

  uint32 GetLabelIdx() {
    return labelIdx;
  }

  uint32 GetLabelIdx() const {
    return labelIdx;
  }

  void SetLabelIdx(uint32 idx) {
    labelIdx = idx;
  }

  LabelNode *GetStartLabel() {
    return startLabel;
  }

  const LabelNode *GetStartLabel() const {
    return startLabel;
  }

  void SetStartLabel(LabelNode &label) {
    startLabel = &label;
  }

  LabelNode *GetEndLabel() {
    return endLabel;
  }

  const LabelNode *GetEndLabel() const {
    return endLabel;
  }

  void SetEndLabel(LabelNode &label) {
    endLabel = &label;
  }

  LabelNode *GetCleanupLabel() {
    return cleanupLabel;
  }

  const LabelNode *GetCleanupLabel() const {
    return cleanupLabel;
  }

  void SetCleanupLabel(LabelNode &node) {
    cleanupLabel = &node;
  }

  BB *GetFirstBB() {
    return firstBB;
  }

  const BB *GetFirstBB() const {
    return firstBB;
  }

  void SetFirstBB(BB &bb) {
    firstBB = &bb;
  }

  BB *GetCleanupBB() {
    return cleanupBB;
  }

  const BB *GetCleanupBB() const {
    return cleanupBB;
  }

  void SetCleanupBB(BB &bb) {
    cleanupBB = &bb;
  }

  const BB *GetCleanupEntryBB() const {
    return cleanupEntryBB;
  }

  void SetCleanupEntryBB(BB &bb) {
    cleanupEntryBB = &bb;
  }

  BB *GetLastBB() {
    return lastBB;
  }

  const BB *GetLastBB() const {
    return lastBB;
  }

  void SetLastBB(BB &bb) {
    lastBB = &bb;
  }

  BB *GetCurBB() {
    return curBB;
  }

  const BB *GetCurBB() const {
    return curBB;
  }

  void SetCurBB(BB &bb) {
    curBB = &bb;
  }

  BB *GetDummyBB() {
    return dummyBB;
  }

  const BB *GetDummyBB() const {
    return dummyBB;
  }

  LabelIdx GetFirstCGGenLabelIdx() const {
    return firstCGGenLabelIdx;
  }

  MapleVector<BB*> &GetExitBBsVec() {
    return exitBBVec;
  }

  const MapleVector<BB*> GetExitBBsVec() const {
    return exitBBVec;
  }

  size_t ExitBBsVecSize() {
    return exitBBVec.size();
  }

  bool IsExitBBsVecEmpty() const {
    return exitBBVec.empty();
  }

  void EraseExitBBsVec(MapleVector<BB*>::iterator it) {
    exitBBVec.erase(it);
  }

  void PushBackExitBBsVec(BB &bb) {
    exitBBVec.push_back(&bb);
  }

  void ClearExitBBsVec() {
    exitBBVec.clear();
  }

  bool IsExitBB(const BB &currentBB) {
    for (BB *exitBB : exitBBVec) {
      if (exitBB == &currentBB) {
        return true;
      }
    }
    return false;
  }

  BB *GetExitBB(int32 index) {
    return exitBBVec.at(index);
  }

  const BB *GetExitBB(int32 index) const {
    return exitBBVec.at(index);
  }

  void SetLab2BBMap(int32 index, BB &bb) {
    lab2BBMap[index] = &bb;
  }

  BB *GetBBFromLab2BBMap(int32 index) {
    return lab2BBMap[index];
  }

  BECommon &GetBecommon() {
    return beCommon;
  }

  const BECommon GetBecommon() const {
    return beCommon;
  }

  MemLayout *GetMemlayout() {
    return memLayout;
  }

  const MemLayout *GetMemlayout() const {
    return memLayout;
  }

  void SetMemlayout(MemLayout &layout) {
    memLayout = &layout;
  }

  MemPool *GetMemoryPool() {
    return memPool;
  }

  const MemPool *GetMemoryPool() const {
    return memPool;
  }

  MapleAllocator *GetFuncScopeAllocator() {
    return funcScopeAllocator;
  }

  const MapleAllocator *GetFuncScopeAllocator() const {
    return funcScopeAllocator;
  }

  const MapleVector<MIRSymbol*> GetEmitStVec() const {
    return emitStVec;
  }

  void AddEmitSt(MIRSymbol &symbol) {
    emitStVec.push_back(&symbol);
  }

  MapleVector<CGFuncLoops*> &GetLoops() {
    return loops;
  }

  const MapleVector<CGFuncLoops*> GetLoops() const {
    return loops;
  }

  void PushBackLoops(CGFuncLoops &loop) {
    loops.push_back(&loop);
  }

  CGCFG *GetTheCFG() {
    return theCFG;
  }

  const CGCFG *GetTheCFG() const {
    return theCFG;
  }

  regno_t GetVirtualRegNOFromPseudoRegIdx(PregIdx idx) const {
    return regno_t(idx + firstMapleIrVRegNO);
  }

  bool GetHasProEpilogue() const {
    return hasProEpilogue;
  }

  void SetHasProEpilogue(bool state) {
    hasProEpilogue = state;
  }

  int32 GetDbgCallFrameOffset() const {
    return dbgCallFrameOffset;
  }

  void SetDbgCallFrameOffset(int32 val) {
    dbgCallFrameOffset = val;
  }

  BB *CreateNewBB() {
    return memPool->New<BB>(bbCnt++, *funcScopeAllocator);
  }

  BB *CreateNewBB(bool unreachable, BB::BBKind kind, uint32 frequency) {
    BB *newBB = CreateNewBB();
    newBB->SetKind(kind);
    newBB->SetUnreachable(unreachable);
    newBB->SetFrequency(frequency);
    return newBB;
  }

  BB *CreateNewBB(LabelIdx label, bool unreachable, BB::BBKind kind, uint32 frequency) {
    BB *newBB = CreateNewBB(unreachable, kind, frequency);
    newBB->AddLabel(label);
    SetLab2BBMap(label, *newBB);
    return newBB;
  }


  BB *StartNewBBImpl(bool stmtIsCurBBLastStmt, StmtNode &stmt) {
    BB *newBB = CreateNewBB();
    ASSERT(newBB != nullptr, "newBB should not be nullptr");
    if (stmtIsCurBBLastStmt) {
      ASSERT(curBB != nullptr, "curBB should not be nullptr");
      curBB->SetLastStmt(stmt);
      curBB->AppendBB(*newBB);
      newBB->SetFirstStmt(*stmt.GetNext());
    } else {
      newBB->SetFirstStmt(stmt);
      if (curBB != nullptr) {
        if (stmt.GetPrev() != nullptr) {
          ASSERT(stmt.GetPrev()->GetNext() == &stmt, " the next of stmt's prev should be stmt self");
        }
        curBB->SetLastStmt(*stmt.GetPrev());
        curBB->AppendBB(*newBB);
      }
    }
    /* used for handle function, frequency is the laststmt->frequency. */
    if (curBB != nullptr) {
      curBB->SetFrequency(frequency);
    } else {
      newBB->SetFrequency(frequency);
    }
    ASSERT(newBB->GetLastStmt() == nullptr, "newBB's lastStmt must be nullptr");
    return newBB;
  }

  BB *StartNewBB(StmtNode &stmt) {
    BB *bb = curBB;
    if (stmt.GetNext() != nullptr && stmt.GetNext()->GetOpCode() != OP_label) {
      bb = StartNewBBImpl(true, stmt);
    }
    return bb;
  }

  void SetCurBBKind(BB::BBKind bbKind) {
    curBB->SetKind(bbKind);
  }

  void SetVolStore(bool val) {
    isVolStore = val;
  }

  void SetVolReleaseInsn(Insn *insn) {
    volReleaseInsn = insn;
  }


 protected:
  uint32 firstMapleIrVRegNO = 200;        /* positioned after physical regs */
  uint32 firstNonPregVRegNO;
  uint32 vRegCount;                       /* for assigning a number for each CG virtual register */
  uint32 maxRegCount;                     /* for the current virtual register number limit */
  MapleVector<VirtualRegNode> vRegTable;  /* table of CG's virtual registers indexed by v_reg no */
  MapleMap<regno_t, RegOperand*> vRegOperandTable;
  MapleMap<PregIdx, MemOperand*> pRegSpillMemOperands;
  MapleMap<regno_t, MemOperand*> spillRegMemOperands;
  MapleSet<MemOperand*> spillRegMemOperandsAdj;
  MapleMap<uint32, SpillMemOperandSet*> reuseSpillLocMem;
  LabelIdx firstCGGenLabelIdx;
  MapleMap<LabelIdx, uint64> labelMap;
#if DEBUG
  MapleMap<PregIdx, StIdx> *pregsToVarsMap = nullptr;
#endif
  int32 totalInsns = 0;
  bool hasProEpilogue = false;
  bool isVolLoad = false;
  bool isVolStore = false;
  uint32 frequency = 0;
  DebugInfo *debugInfo = nullptr;  /* debugging info */
  int32 dbgCallFrameOffset = 0;
  CG *cg;
  MIRModule &mirModule;
  MemPool *memPool;

  PregIdx GetPseudoRegIdxFromVirtualRegNO(const regno_t vRegNO) const {
    ASSERT(IsVRegNOForPseudoRegister(vRegNO), "");
    return PregIdx(vRegNO - firstMapleIrVRegNO);
  }

  bool IsVRegNOForPseudoRegister(regno_t vRegNum) const {
    /* 0 is not allowed for preg index */
    uint32 n = static_cast<uint32>(vRegNum);
    return (firstMapleIrVRegNO < n && n < firstNonPregVRegNO);
  }

  VirtualRegNode &GetVirtualRegNodeFromPseudoRegIdx(PregIdx idx) {
    return vRegTable.at(GetVirtualRegNOFromPseudoRegIdx(idx));
  }

  PrimType GetTypeFromPseudoRegIdx(PregIdx idx) {
    VirtualRegNode &vRegNode = GetVirtualRegNodeFromPseudoRegIdx(idx);
    RegType regType = vRegNode.GetType();
    ASSERT(regType == kRegTyInt || regType == kRegTyFloat, "");
    uint32 size = vRegNode.GetSize();  /* in bytes */
    ASSERT(size == sizeof(int32) || size == sizeof(int64), "");
    return (regType == kRegTyInt ? (size == sizeof(int32) ? PTY_i32 : PTY_i64)
                                 : (size == sizeof(float) ? PTY_f32 : PTY_f64));
  }

  int64 GetPseudoRegisterSpillLocation(PregIdx idx) {
    const SymbolAlloc *symLoc = memLayout->GetSpillLocOfPseduoRegister(idx);
    return static_cast<int64>(GetBaseOffset(*symLoc));
  }

  virtual MemOperand *GetPseudoRegisterSpillMemoryOperand(PregIdx idx) = 0;

  uint32 GetSpillLocation(uint32 size) {
    uint32 offset = RoundUp(nextSpillLocation, static_cast<uint64>(size));
    nextSpillLocation = offset + size;
    return offset;
  }

 private:
  CGFunc &operator=(const CGFunc &cgFunc);
  CGFunc(const CGFunc&);
  StmtNode *HandleFirstStmt();
  bool CheckSkipMembarOp(StmtNode &stmt);
  MIRFunction &func;
  EHFunc *ehFunc = nullptr;
  uint32 bbCnt = 0;
  uint32 labelIdx = 0;          /* local label index number */
  LabelNode *startLabel = nullptr;    /* start label of the function */
  LabelNode *endLabel = nullptr;      /* end label of the function */
  LabelNode *cleanupLabel = nullptr;  /* label to indicate the entry of cleanup code. */
  BB *firstBB = nullptr;
  BB *cleanupBB = nullptr;
  BB *cleanupEntryBB = nullptr;
  BB *lastBB = nullptr;
  BB *curBB = nullptr;
  BB *dummyBB;   /* use this bb for add some instructions to bb that is no curBB. */
  Insn *volReleaseInsn = nullptr;  /* use to record the release insn for volatile strore */
  MapleVector<BB*> exitBBVec;
  MapleMap<LabelIdx, BB*> lab2BBMap;
  BECommon &beCommon;
  MemLayout *memLayout = nullptr;
  MapleAllocator *funcScopeAllocator;
  MapleVector<MIRSymbol*> emitStVec;  /* symbol that needs to be emit as a local symbol. i.e, switch table */
  MapleVector<CGFuncLoops*> loops;
  CGCFG *theCFG = nullptr;
  uint32 nextSpillLocation = 0;
  static constexpr int kRegIncrStepLen = 80; /* reg number increate step length */
};  /* class CGFunc */

CGFUNCPHASE(CgDoLayoutSF, "layoutstackframe")
CGFUNCPHASE(CgDoHandleFunc, "handlefunction")
CGFUNCPHASE(CgDoGenCfi, "gencfi")
CGFUNCPHASE(CgDoEmission, "emit")
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_CGFUNC_H */
