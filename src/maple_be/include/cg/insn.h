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
#ifndef MAPLEBE_INCLUDE_CG_INSN_H
#define MAPLEBE_INCLUDE_CG_INSN_H
/* C++ headers */
#include <cstddef>  /* for nullptr */
#include <string>
#include <vector>
#include <list>
#include "operand.h"
#include "mpl_logging.h"

/* Maple IR header */
#include "types_def.h"  /* for uint32 */
#include "common_utils.h"

namespace maplebe {
using MOperator = uint32;

/* forward declaration */
class BB;
class CG;
class Emitter;
class DepNode;


class Insn {
 public:
  enum RetType : uint8 {
    kRegNull,   /* no return type */
    kRegFloat,  /* return register is V0 */
    kRegInt     /* return register is R0 */
  };
  /* MCC_DecRefResetPair clear 2 stack position, MCC_ClearLocalStackRef clear 1 stack position */
  static constexpr uint8 kMaxStackOffsetSize = 2;

  Insn(MemPool &memPool, MOperator opc)
      : mOp(opc), localAlloc(&memPool), opnds(localAlloc.Adapter()), comment(&memPool) {
#if !RELEASE
    Check();
#endif
  }
  Insn(MemPool &memPool, MOperator opc, Operand &opnd0) : Insn(memPool, opc) { opnds.push_back(&opnd0); }
  Insn(MemPool &memPool, MOperator opc, Operand &opnd0, Operand &opnd1) : Insn(memPool, opc) {
    opnds.push_back(&opnd0);
    opnds.push_back(&opnd1);
  }
  Insn(MemPool &memPool, MOperator opc, Operand &opnd0, Operand &opnd1, Operand &opnd2) : Insn(memPool, opc) {
    opnds.push_back(&opnd0);
    opnds.push_back(&opnd1);
    opnds.push_back(&opnd2);
  }
  Insn(MemPool &memPool, MOperator opc, Operand &opnd0, Operand &opnd1, Operand &opnd2, Operand &opnd3)
      : Insn(memPool, opc) {
    opnds.push_back(&opnd0);
    opnds.push_back(&opnd1);
    opnds.push_back(&opnd2);
    opnds.push_back(&opnd3);
  }
  Insn(MemPool &memPool, MOperator opc, Operand &opnd0, Operand &opnd1, Operand &opnd2, Operand &opnd3, Operand &opnd4)
      : Insn(memPool, opc) {
    opnds.push_back(&opnd0);
    opnds.push_back(&opnd1);
    opnds.push_back(&opnd2);
    opnds.push_back(&opnd3);
    opnds.push_back(&opnd4);
  }
  virtual ~Insn() = default;

  MOperator GetMachineOpcode() const {
    return mOp;
  }

  void SetMOP(MOperator mOp) {
    this->mOp = mOp;
  }

  void AddOperand(Operand &opnd) {
    opnds.push_back(&opnd);
  }
  void PopBackOperand() {
    opnds.pop_back();
  }
  Operand &GetOperand(int32 index) const {
    ASSERT(index < opnds.size(), "index out of range");
    return *opnds[index];
  }
  uint32 GetOperandSize() const {
    return static_cast<uint32>(opnds.size());
  }
  void SetOperand(uint32 index, Operand &opnd) {
    ASSERT(index <= opnds.size(), "index out of range");
    opnds[index] = &opnd;
  }

  virtual void SetResult(uint32 index, Operand &res) {
    (void)index;
    (void)res;
  }

  void SetRetSize(uint32 size) {
    ASSERT(IsCall(), "Insn should be a call.");
    retSize = size;
  }

  uint32 GetRetSize() {
    ASSERT(IsCall(), "Insn should be a call.");
    return retSize;
  }

  virtual bool IsMachineInstruction() const = 0;

  virtual bool IsPseudoInstruction() const {
    return false;
  }

  virtual bool OpndIsDef(uint32) const {
    return false;
  }

  virtual bool OpndIsUse(uint32) const {
    return false;
  }

  virtual bool OpndIsMayDef(uint32) const {
    return false;
  }

  virtual bool IsEffectiveCopy() const {
    return false;
  }

  virtual int32 CopyOperands() const {
    return -1;
  }

  virtual bool IsSameRes() {
    return false;
  }

  virtual bool IsPCLoad() const {
    return false;
  }

  virtual uint32 GetOpndNum() const {
    return 0;
  }

  virtual uint32 GetResultNum() const {
    return 0;
  }

  virtual Operand *GetOpnd(uint32 index) const {
    (void)index;
    return nullptr;
  }

  virtual Operand *GetMemOpnd() const {
    return nullptr;
  }

  virtual Operand *GetResult(uint32 index) const{
    (void)index;
    return nullptr;
  }

  virtual void SetOpnd(uint32 index, Operand &opnd) {
    (void)index;
    (void)opnd;
  }

  virtual bool IsGlobal() const {
    return false;
  }

  virtual bool IsDecoupleStaticOp() const {
    return false;
  }

  virtual bool IsCall() const {
    return false;
  }

  virtual bool IsTailCall() const {
    return false;
  }

  virtual bool IsClinit() const {
    return false;
  }

  virtual bool IsLazyLoad() const {
    return false;
  }

  virtual bool IsAdrpLdr() const {
    return false;
  }

  virtual bool IsReturn() const {
    return false;
  }

  virtual bool IsFixedInsn() const {
    return false;
  }

  virtual bool CanThrow() const {
    return false;
  }

  virtual bool MayThrow() {
    return false;
  }

  virtual bool IsIndirectCall() const {
    return false;
  }

  virtual bool IsCallToFunctionThatNeverReturns() {
    return false;
  }

  virtual bool IsLoadLabel() const {
    return false;
  }

  virtual bool IsBranch() const {
    return false;
  }

  virtual bool IsCondBranch() const {
    return false;
  }

  virtual bool IsUnCondBranch() const {
    return false;
  }

  virtual bool IsMove() const{
    return false;
  }

  virtual bool IsLoad() const {
    return false;
  }

  virtual bool IsStore() const {
    return false;
  }

  virtual bool IsLoadPair() const {
    return false;
  }

  virtual bool IsStorePair() const {
    return false;
  }

  virtual bool IsLoadAddress() const {
    return false;
  }

  virtual bool IsAtomic() const {
    return false;
  }

  virtual bool NoAlias() const {
    return false;
  }

  virtual bool IsVolatile() const {
    return false;
  }

  virtual bool IsMemAccessBar() const {
    return false;
  }

  virtual bool IsMemAccess() const {
    return false;
  }

  virtual bool HasSideEffects() const {
    return false;
  }

  virtual bool HasLoop() const {
    return false;
  }

  virtual bool IsSpecialIntrinsic() const {
    return false;
  }

  virtual bool IsComment() const {
    return false;
  }

  virtual bool IsGoto() const {
    return false;
  }

  virtual bool IsImmaterialInsn() const {
    return false;
  }

  virtual bool IsYieldPoint() const {
    return false;
  }

  virtual bool IsPartDef() const {
    return false;
  }

  virtual bool IsTargetInsn() const {
    return false;
  }

  virtual bool IsCfiInsn() const {
    return false;
  }

  virtual bool IsDbgInsn() const {
    return false;
  }

  virtual bool IsFallthruCall() const {
    return false;
  }

  virtual bool IsDMBInsn() const {
    return false;
  }

  virtual Operand *GetCallTargetOperand() const {
    return nullptr;
  }

  virtual uint32 GetAtomicNum() const {
    return 1;
  }
  /*
   * returns a ListOperand
   * Note that we don't really need this for Emit
   * Rather, we need it for register allocation, to
   * correctly state the live ranges for operands
   * use for passing call arguments
   */
  virtual ListOperand *GetCallArgumentOperand() {
    return nullptr;
  }

  bool IsAtomicStore() const {
    return IsStore() && IsAtomic();
  }

  void SetCondDef() {
    flags |= kOpCondDef;
  }

  bool IsCondDef() const {
    return flags & kOpCondDef;
  }

  bool AccessMem() const {
    return IsLoad() || IsStore();
  }

  bool IsFrameDef() const {
    return isFrameDef;
  }

  void SetFrameDef(bool b) {
    isFrameDef = b;
  }

  virtual uint32 GetUnitType() {
    return 0;
  }

  virtual void Emit(const CG&, Emitter&) const = 0;

  virtual void Dump() const = 0;

#if !RELEASE
  virtual bool Check() const {
    return true;
  }

#else
  virtual bool Check() const = 0;
#endif

  void SetComment(const std::string &str) {
    comment = str;
  }

  void SetComment(const MapleString &str) {
    comment = str;
  }

  const MapleString &GetComment() const {
    return comment;
  }

  void AppendComment(const std::string &str) {
    comment += str;
  }

  void MarkAsSaveRetValToLocal() {
    flags |= kOpDassignToSaveRetValToLocal;
  }

  bool IsSaveRetValToLocal() const {
    return ((flags & kOpDassignToSaveRetValToLocal) != 0);
  }

  void MarkAsAccessRefField(bool cond) {
    if (cond) {
      flags |= kOpAccessRefField;
    }
  }

  bool IsAccessRefField() const {
    return ((flags & kOpAccessRefField) != 0);
  }

  virtual bool IsDefinition() const = 0;

  virtual bool IsDestRegAlsoSrcReg() const {
    return false;
  }

  Insn *GetPreviousMachineInsn() const {
    for (Insn *returnInsn = prev; returnInsn != nullptr; returnInsn = returnInsn->prev) {
      ASSERT(returnInsn->bb == bb, "insn and it's prev insn must have same bb");
      if (returnInsn->IsMachineInstruction()) {
        return returnInsn;
      }
    }
    return nullptr;
  }

  Insn *GetNextMachineInsn() const {
    for (Insn *returnInsn = next; returnInsn != nullptr; returnInsn = returnInsn->next) {
      ASSERT(returnInsn->bb == bb, "insn and it's next insn must have same bb");
      if (returnInsn->IsMachineInstruction()) {
        return returnInsn;
      }
    }
    return nullptr;
  }

  virtual uint32 GetLatencyType() const {
    return 0;
  }

  void SetMOperator(MOperator mOp) {
    this->mOp = mOp;
  }

  void SetPrev(Insn *prev) {
    this->prev = prev;
  }

  Insn *GetPrev() {
    return prev;
  }

  const Insn *GetPrev() const {
    return prev;
  }

  void SetNext(Insn *next) {
    this->next = next;
  }

  Insn *GetNext() const {
    return next;
  }

  void SetBB(BB *bb) {
    this->bb = bb;
  }

  BB *GetBB() {
    return bb;
  }

  const BB *GetBB() const {
    return bb;
  }

  void SetId(uint32 id) {
    this->id = id;
  }

  uint32 GetId() const {
    return id;
  }

  void SetIsThrow(bool isThrow) {
    this->isThrow = isThrow;
  }

  bool GetIsThrow() const {
    return isThrow;
  }

  void SetDoNotRemove(bool doNotRemove) {
    this->doNotRemove = doNotRemove;
  }

  bool GetDoNotRemove() const {
    return doNotRemove;
  }

  void SetRetType(RetType retType) {
    this->retType = retType;
  }

  RetType GetRetType() const {
    return retType;
  }

  void SetClearStackOffset(short index, int64 offset) {
    CHECK_FATAL(index < kMaxStackOffsetSize, "out of clearStackOffset's range");
    clearStackOffset[index] = offset;
  }

  int64 GetClearStackOffset(short index) const {
    CHECK_FATAL(index < kMaxStackOffsetSize, "out of clearStackOffset's range");
    return clearStackOffset[index];
  }

  /* if function name is MCC_ClearLocalStackRef or MCC_DecRefResetPair, will clear designate stack slot */
  bool IsClearDesignateStackCall() const {
    return clearStackOffset[0] != -1 || clearStackOffset[1] != -1;
  }

  void SetDepNode(DepNode &depNode) {
    this->depNode = &depNode;
  }

  DepNode *GetDepNode() {
    return depNode;
  }

  const DepNode *GetDepNode() const {
    return depNode;
  }

  void InitWithOriginalInsn(const Insn &originalInsn, MemPool &memPool) {
    prev = originalInsn.prev;
    next = originalInsn.next;
    bb = originalInsn.bb;
    flags = originalInsn.flags;
    mOp = originalInsn.mOp;
    uint32 opndNum = originalInsn.GetOperandSize();
    for (uint32 i = 0; i < opndNum; i++) {
      opnds.push_back(originalInsn.opnds[i]->Clone(memPool));
    }
  }

 protected:
  MOperator mOp;
  MapleAllocator localAlloc;
  MapleVector<Operand*> opnds;
  Insn *prev = nullptr;
  Insn *next = nullptr;
  BB *bb = nullptr;        /* BB to which this insn belongs */
  uint32 flags = 0;

 private:
  enum OpKind : uint32 {
    kOpUnknown = 0,
    kOpCondDef = 0x1,
    kOpAccessRefField = (1ULL << 30),  /* load-from/store-into a ref flag-fieldGetMachineOpcode() */
    kOpDassignToSaveRetValToLocal = (1ULL << 31) /* save return value to local flag */
  };

  uint32 id = 0;
  bool isThrow = false;
  bool doNotRemove = false;  /* caller reg cross call */
  RetType retType = kRegNull;    /* if this insn is call, it represent the return register type R0/V0 */
  uint32 retSize = 0;  /* Byte size of the return value if insn is a call. */
  /* record the stack cleared by MCC_ClearLocalStackRef or MCC_DecRefResetPair */
  int64 clearStackOffset[kMaxStackOffsetSize] = { -1, -1 };
  DepNode *depNode = nullptr; /* For dependence analysis, pointing to a dependence node. */
  MapleString comment;
  bool isFrameDef = false;
};

struct InsnIdCmp {
  bool operator()(const Insn *lhs, const Insn *rhs) const {
    CHECK_FATAL(lhs != nullptr, "lhs is nullptr in InsnIdCmp");
    CHECK_FATAL(rhs != nullptr, "rhs is nullptr in InsnIdCmp");
    return lhs->GetId() < rhs->GetId();
  }
};
using InsnSet = std::set<Insn*, InsnIdCmp>;
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_INSN_H */
