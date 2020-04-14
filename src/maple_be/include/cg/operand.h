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
#ifndef MAPLEBE_INCLUDE_CG_OPERAND_H
#define MAPLEBE_INCLUDE_CG_OPERAND_H

#include "becommon.h"
#include "isa.h"
#include "cg_option.h"

/* maple_ir */
#include "types_def.h"   /* need uint8 etc */
#include "prim_types.h"  /* for PrimType */
#include "mir_symbol.h"

/* Mempool */
#include "mempool_allocator.h"  /* MapleList */

namespace maplebe {
class Emitter;

namespace {
constexpr int32 kOffsetImmediateOpndSpace = 4; /* offset and immediate operand space is 4 */
};

class Operand {
 public:
  enum OperandType : uint8 {
    kOpdRegister,
    kOpdImmediate,
    kOpdFPImmediate,
    kOpdFPZeroImmediate,
    kOpdStImmediate,    /* use the symbol name as the offset */
    kOpdOffset,         /* for the offset operand in MemOperand */
    kOpdMem,
    kOpdBBAddress,
    kOpdList,           /*  for list operand */
    kOpdCond,           /*  for condition code */
    kOpdShift,          /*  for imm shift operand */
    kOpdRegShift,       /*  for reg shift operand */
    kOpdExtend,         /*  for extend operand */
    kOpdString,         /*  for comments */
    kOpdUndef
  };

  Operand(OperandType type, uint32 size) : opndKind(type), size(size) {}
  virtual ~Operand() = default;

  uint32 GetSize() const {
    return size;
  }

  void SetSize(uint32 sz) {
    size = sz;
  }

  OperandType GetKind() const {
    return opndKind;
  }

  bool IsIntImmediate() const {
    return opndKind == kOpdImmediate || opndKind == kOpdOffset;
  }

  bool IsConstImmediate() const {
    return opndKind == kOpdImmediate || opndKind == kOpdOffset ||
           opndKind == kOpdFPImmediate || opndKind == kOpdFPZeroImmediate;
  }

  bool IsOfstImmediate() const {
    return opndKind == kOpdOffset;
  }

  bool IsStImmediate() const {
    return opndKind == kOpdStImmediate;
  }

  bool IsImmediate() const {
    ASSERT(kOpdOffset - kOpdImmediate == kOffsetImmediateOpndSpace, "offset and immediate operand space should be 4");
    return (kOpdImmediate <= opndKind && opndKind <= kOpdOffset);
  }

  bool IsRegister() const {
    return opndKind == kOpdRegister;
  }

  bool IsList() const {
    return opndKind == kOpdList;
  }

  bool IsMemoryAccessOperand() const {
    return opndKind == kOpdMem;
  }

  bool IsConstant() const {
    return IsConstImmediate() || IsConstReg();
  }

  bool IsConstReg() const {
    if (!IsRegister()) {
      return false;
    }
    return IsZeroRegister();
  };

  virtual bool IsZeroRegister() const {
    return false;
  };

  bool IsLabel() const {
    return opndKind == kOpdBBAddress;
  }

  bool IsConditionCode() const {
    return opndKind == kOpdCond;
  }

  bool IsRegShift() const {
    return opndKind == kOpdRegShift;
  }

  virtual bool IsLabelOpnd() const {
    return false;
  }

  virtual bool IsFuncNameOpnd() const {
    return false;
  }

  virtual bool IsCommentOpnd() const {
    return false;
  }

  virtual Operand *Clone(MemPool &memPool) const = 0;

  /*
   * A simple implementation here.
   * Each subclass can elaborate on demand.
   */
  virtual bool Equals(Operand &op) const {
    return BasicEquals(op) && (&op == this);
  }

  bool BasicEquals(const Operand &op) const {
    return opndKind == op.GetKind() && size == op.GetSize();
  }

  virtual void Emit(Emitter&, const OpndProp*) const = 0;

  virtual void Dump() const = 0;

  virtual bool Less(const Operand &right) const = 0;

 protected:
  OperandType opndKind;  /* operand type */
  uint32 size;           /* size in bits */
};

/* RegOperand */
enum RegOperandState : uint32 {
  kRegOpndNone = 0,
  kRegOpndSetLow32 = 0x1,
  kRegOpndSetHigh32 = 0x2
};

class RegOperand : public Operand {
 public:
  RegOperand(regno_t regNum, uint32 size, RegType type)
      : Operand(kOpdRegister, size),
        regNO(regNum),
        regType(type),
        validBitsNum(size) {}

  ~RegOperand() override = default;

  void SetValidBitsNum(uint32 validNum) {
    validBitsNum = validNum;
  }

  uint32 GetValidBitsNum() const {
    return validBitsNum;
  }

  bool IsOfIntClass() const {
    return regType == kRegTyInt;
  }

  bool IsOfFloatOrSIMDClass() const {
    return regType == kRegTyFloat;
  }

  bool IsOfCC() const {
    return regType == kRegTyCc;
  }

  bool IsOfVary() const {
    return regType == kRegTyVary;
  }

  RegType GetRegisterType() const {
    return regType;
  }

  virtual bool IsVirtualRegister() const {
    return false;
  }

  virtual bool IsBBLocalVReg() const {
    return isBBLocal;
  }

  void SetRegNotBBLocal() {
    isBBLocal = false;
  }

  regno_t GetRegisterNumber() const {
    return regNO;
  }

  void SetRegisterNumber(regno_t regNum) {
    regNO = regNum;
  }

  virtual bool IsInvalidRegister() const = 0;
  virtual bool IsSaveReg(MIRType &mirType, BECommon &beCommon) const = 0;
  virtual bool IsPhysicalRegister() const = 0;
  virtual bool IsSPOrFP() const = 0;
  void Emit(Emitter &emitter, const OpndProp *opndProp) const override = 0;
  void Dump() const override = 0;

  bool Less(const Operand &right) const override {
    if (&right == this) {
      return false;
    }

    /* For different type. */
    if (opndKind != right.GetKind()) {
      return opndKind < right.GetKind();
    }

    auto *rightOpnd = static_cast<const RegOperand*>(&right);

    /* The same type. */
    return regNO < rightOpnd->regNO;
  }

  bool Less(const RegOperand &right) const {
    return regNO < right.regNO;
  }

  bool RegNumEqual(const RegOperand &right) const {
    return regNO == right.GetRegisterNumber();
  }

  int32 RegCompare(const RegOperand &right) const {
    return (regNO - right.GetRegisterNumber());
  }

  bool Equals(Operand &operand) const override {
    if (!operand.IsRegister()) {
      return false;
    }
    auto &op = static_cast<RegOperand&>(operand);
    if (&op == this) {
      return true;
    }
    return (BasicEquals(op) && regNO == op.GetRegisterNumber() && regType == op.GetRegisterType() &&
        IsBBLocalVReg() == op.IsBBLocalVReg());
  }

  static bool IsSameRegNO(const Operand &firstOpnd, const Operand &secondOpnd) {
    if (!firstOpnd.IsRegister() || !secondOpnd.IsRegister()) {
      return false;
    }
    auto &firstReg = static_cast<const RegOperand&>(firstOpnd);
    auto &secondReg = static_cast<const RegOperand&>(secondOpnd);
    return firstReg.RegNumEqual(secondReg);
  }

  static bool IsSameReg(const Operand &firstOpnd, const Operand &secondOpnd) {
    if (firstOpnd.GetSize() != secondOpnd.GetSize()) {
      return false;
    }
    return IsSameRegNO(firstOpnd, secondOpnd);
  }

 protected:
  regno_t regNO;
  RegType regType;

  /*
   * used for EBO(-O1), it can recognize the registers whose use and def are in different BB. It is
   * true by default. Sometime it should be false such as when handle intrinsiccall for target
   * aarch64(AArch64CGFunc::SelectIntrinCall).
   */
  bool isBBLocal = true;
  uint32 validBitsNum;

  void SetRegisterType(RegType type) {
    regType = type;
  }
};  /* class RegOperand */

enum VaryType : uint8 {
  kNotVary = 0,
  kUnAdjustVary,
  kAdjustVary
};

class ImmOperand : public Operand {
 public:
  ImmOperand(int64 val, uint32 size, bool isSigned, VaryType isVar = kNotVary)
      : Operand(kOpdImmediate, size), value(val), isSigned(isSigned), isVary(isVar) {}
  ImmOperand(OperandType type, int64 val, uint32 size, bool isSigned, VaryType isVar = kNotVary)
      : Operand(type, size), value(val), isSigned(isSigned), isVary(isVar) {}

  ~ImmOperand() override = default;

  virtual bool IsSingleInstructionMovable() const = 0;

  int64 GetValue() const {
    return value;
  }

  void SetValue(int64 val) {
    value = val;
  }

  void SetVary(VaryType flag) {
    isVary = flag;
  }

  bool IsZero() const {
    return value == 0;
  }

  VaryType GetVary() const {
    return isVary;
  }

  bool IsOne() const {
    return value == 1;
  }

  bool IsSignedValue() const {
    return isSigned;
  }

  bool IsInBitSize(uint8 size, uint8 nLowerZeroBits = 0) const {
    /* mask1 is a 64bits number that is all 1 shifts left size bits */
    const uint64 mask1 = 0xffffffffffffffffUL << size;
    /* mask2 is a 64 bits number that nlowerZeroBits are all 1, higher bits aro all 0 */
    uint64 mask2 = (static_cast<uint64>(1) << static_cast<uint64>(nLowerZeroBits)) - 1UL;
    return (mask2 & value) == 0UL && (mask1 & ((static_cast<uint64>(value)) >> nLowerZeroBits)) == 0UL;
  }

  bool IsInBitSizeRot(uint8 size) const {
    return IsInBitSizeRot(size, value);
  }

  static bool IsInBitSizeRot(uint8 size, int64 val) {
    /* to tell if the val is in a rotate window of size */
#if __GNU_C__ || __clang__
    if (val == 0) {
      return true;
    }
    int32 start = __builtin_ctzll(val);
    int32 end = sizeof(val) * kBitsPerByte - __builtin_clzll(val) - 1;
    return (size >= end - start + 1);
#else
    uint8 start = 0;
    uint8 end = 0;
    bool isFound = false;
    CHECK_FATAL(val > 0, "do not perform bit operator operations on signed integers");
    for (uint32 i = 0; i < k64BitSize; ++i) {
      /* check whether the ith bit of val is 1 or not */
      if (((static_cast<uint64>(val) >> i) & 0x1) == 0x1) {
        if (!isFound) {
          start = i;
          end = i;
          isFound = true;
        } else {
          end = i;
        }
      }
    }
    return !isFound || (size >= end - start + 1);
#endif
  }

  static bool IsInValueRange(int32 lowVal, int32 highVal, int32 val) {
    return val >= lowVal && val <= highVal;
  }

  bool IsNegative() const {
    return isSigned && value < 0;
  }

  void Add(int64 delta) {
    value += delta;
  }

  void Negate() {
    value = -value;
  }

  void BitwiseNegate() {
    value = ~(static_cast<uint64>(value)) & ((1ULL << size) - 1UL);
  }

  void DivideByPow2(int32 shift) {
    value = (static_cast<uint64>(value)) >> shift;
  }

  void ModuloByPow2(int32 shift) {
    value = (static_cast<uint64>(value)) & ((1ULL << shift) - 1UL);
  }

  bool IsAllOnes() const {
    return value == -1;
  }

  bool operator<(const ImmOperand &iOpnd) const {
    return value < iOpnd.value || (value == iOpnd.value && isSigned < iOpnd.isSigned) ||
           (value == iOpnd.value && isSigned == iOpnd.isSigned && size < iOpnd.GetSize());
  }

  bool operator==(const ImmOperand &iOpnd) const {
    return (value == iOpnd.value && isSigned == iOpnd.isSigned && size == iOpnd.GetSize());
  }

  void Emit(Emitter &emitter, const OpndProp *prop) const override = 0;

  void Dump() const override;

  bool Less(const Operand &right) const override {
    if (&right == this) {
      return false;
    }

    /* For different type. */
    if (opndKind != right.GetKind()) {
      return opndKind < right.GetKind();
    }

    auto *rightOpnd = static_cast<const ImmOperand*>(&right);

    /* The same type. */
    if (isSigned != rightOpnd->isSigned) {
      return isSigned;
    }

    if (isVary != rightOpnd->isVary) {
      return isVary;
    }

    return value < rightOpnd->value;
  }

  bool Equals(Operand &operand) const override {
    if (!operand.IsImmediate()) {
      return false;
    }
    auto &op = static_cast<ImmOperand&>(operand);
    if (&op == this) {
      return true;
    }
    return (BasicEquals(op) && value == op.GetValue() && isSigned == op.IsSignedValue());
  }

  bool ValueEquals(const ImmOperand &op) const {
    if (&op == this) {
      return true;
    }
    return (value == op.GetValue() && isSigned == op.IsSignedValue());
  }

 protected:
  int64 value;
  bool isSigned;
  VaryType isVary;
};

using OfstOperand = ImmOperand;

class MemOperand : public Operand {
 public:
  RegOperand *GetBaseRegister() const {
    return baseOpnd;
  }

  void SetBaseRegister(RegOperand &regOpnd) {
    baseOpnd = &regOpnd;
  }

  RegOperand *GetIndexRegister() const {
    return indexOpnd;
  }

  void SetIndexRegister(RegOperand &regOpnd) {
    indexOpnd = &regOpnd;
  }

  OfstOperand *GetOffsetOperand() const {
    return offsetOpnd;
  }

  void SetOffsetOperand(OfstOperand &oftOpnd) {
    offsetOpnd = &oftOpnd;
  }

  const Operand *GetScaleOperand() const {
    return scaleOpnd;
  }

  const MIRSymbol *GetSymbol() const {
    return symbol;
  }

  bool Equals(Operand &operand) const override {
    if (!operand.IsMemoryAccessOperand()) {
      return false;
    }
    auto &op = static_cast<MemOperand&>(operand);
    if (&op == this) {
      return true;
    }
    CHECK_FATAL(baseOpnd != nullptr, "baseOpnd is null in Equals");
    CHECK_FATAL(indexOpnd != nullptr, "indexOpnd is null in Equals");
    return (baseOpnd->Equals(*op.GetBaseRegister()) && indexOpnd->Equals(*op.GetIndexRegister()));
  }

  void SetMemoryOrdering(uint32 memOrder) {
    memoryOrder |= memOrder;
  }

  bool HasMemoryOrdering(uint32 memOrder) const {
    return (memoryOrder & memOrder) != 0;
  }

  virtual Operand *GetOffset() const {
    return nullptr;
  }

  virtual VaryType GetMemVaryType() {
    return kNotVary;
  }

  bool Less(const Operand &right) const override = 0;

  MemOperand(uint32 size, const MIRSymbol &mirSymbol)
      : Operand(Operand::kOpdMem, size),
        symbol(&mirSymbol) {}

  MemOperand(uint32 size, RegOperand *baseOp, RegOperand *indexOp, OfstOperand *ofstOp, const MIRSymbol *mirSymbol,
             Operand *scaleOp = nullptr)
      : Operand(Operand::kOpdMem, size),
        baseOpnd(baseOp),
        indexOpnd(indexOp),
        offsetOpnd(ofstOp),
        scaleOpnd(scaleOp),
        symbol(mirSymbol) {}

  /* Copy constructor */
  MemOperand(const MemOperand &memOpnd)
      : Operand(Operand::kOpdMem, memOpnd.GetSize()),
        baseOpnd(memOpnd.baseOpnd),
        indexOpnd(memOpnd.indexOpnd),
        offsetOpnd(memOpnd.offsetOpnd),
        scaleOpnd(memOpnd.scaleOpnd),
        symbol(memOpnd.symbol),
        memoryOrder(memOpnd.memoryOrder) {}

  MemOperand &operator=(const MemOperand &memOpnd) = default;

  ~MemOperand() override = default;

 private:
  RegOperand *baseOpnd = nullptr;     /* base register */
  RegOperand *indexOpnd = nullptr;    /* index register */
  OfstOperand *offsetOpnd = nullptr;  /* offset immediate */
  Operand *scaleOpnd = nullptr;
  const MIRSymbol *symbol;  /* AddrMode_Literal */
  uint32 memoryOrder = 0;
};

class LabelOperand : public Operand {
 public:
  LabelOperand(const char *parent, LabelIdx labIdx)
      : Operand(kOpdBBAddress, 0), labelIndex(labIdx), parentFunc(parent), orderID(-1u) {}

  ~LabelOperand() override = default;

  Operand *Clone(MemPool &memPool) const override {
    return memPool.Clone<LabelOperand>(*this);
  }

  bool IsLabelOpnd() const override {
    return true;
  }

  LabelIdx GetLabelIndex() const {
    return labelIndex;
  }
  const std::string GetParentFunc() const {
    return parentFunc;
  }

  LabelIDOrder GetLabelOrder() const {
    return orderID;
  }

  void SetLabelOrder(LabelIDOrder idx) {
    orderID = idx;
  }

  void Emit(Emitter &emitter, const OpndProp *opndProp) const override;

  void Dump() const override;

  bool Less(const Operand &right) const override {
    if (&right == this) {
      return false;
    }

    /* For different type. */
    if (opndKind != right.GetKind()) {
      return opndKind < right.GetKind();
    }

    auto *rightOpnd = static_cast<const LabelOperand*>(&right);

    int32 nRes = strcmp(parentFunc, rightOpnd->parentFunc);
    if (nRes == 0) {
      return labelIndex < rightOpnd->labelIndex;
    } else {
      return nRes < 0;
    }
  }

  bool Equals(Operand &operand) const override {
    if (!operand.IsLabel()) {
      return false;
    }
    auto &op = static_cast<LabelOperand&>(operand);
    return ((&op == this) || (op.GetLabelIndex() == labelIndex));
  }

 protected:
  LabelIdx labelIndex;
  const char *parentFunc;

 private:
  /* this index records the order this label is defined during code emit. */
  LabelIDOrder orderID = -1u;
};

class ListOperand : public Operand {
 public:
  explicit ListOperand(MapleAllocator &allocator) : Operand(Operand::kOpdList, 0), opndList(allocator.Adapter()) {}

  ~ListOperand() override = default;

  void PushOpnd(RegOperand &opnd) {
    opndList.push_back(&opnd);
  }

  void PushFront(RegOperand &opnd) {
    opndList.push_front(&opnd);
  }

  void removeOpnd(RegOperand &opnd) {
    opndList.remove(&opnd);
  }

  MapleList<RegOperand*> &GetOperands() {
    return opndList;
  }

  void Emit(Emitter &emitter, const OpndProp *opndProp) const override = 0;

  void Dump() const override {
    for (auto *regOpnd : opndList) {
      regOpnd->Dump();
    }
  }

  bool Less(const Operand &right) const override {
    /* For different type. */
    if (opndKind != right.GetKind()) {
      return opndKind < right.GetKind();
    }

    ASSERT(false, "We don't need to compare list operand.");
    return false;
  }

  bool Equals(Operand &operand) const override {
    if (!operand.IsList()) {
      return false;
    }
    auto &op = static_cast<ListOperand&>(operand);
    return (&op == this);
  }

 protected:
  MapleList<RegOperand*> opndList;
};
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_OPERAND_H */
