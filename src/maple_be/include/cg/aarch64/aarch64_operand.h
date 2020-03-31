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
#ifndef MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_OPERAND_H
#define MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_OPERAND_H

#include <limits>
#include <string>
#include <cmath>
#include <iomanip>
#include "aarch64_isa.h"
#include "operand.h"
#include "cg.h"
#include "aarch64_immediate.h"
#include "emit.h"
#include "common_utils.h"


namespace maplebe {
using namespace maple;

class AArch64RegOperand : public RegOperand {
 public:
  AArch64RegOperand(regno_t regNO, uint32 size, RegType kind, uint32 flg = 0)
      : RegOperand(regNO, size, kind), flag(flg) {
    ASSERT(kind != kRegTyUndef, "Reg type must be specified");
  }

  ~AArch64RegOperand() override = default;

  void SetRefField(bool newIsRefField) {
    isRefField = newIsRefField;
  }

  bool IsInvalidRegister() const override {
    return (GetRegisterNumber() == AArch64reg::kRinvalid);
  }

  bool IsPhysicalRegister() const override {
    return AArch64isa::IsPhysicalRegister(GetRegisterNumber());
  }

  bool IsVirtualRegister() const override {
    return !IsPhysicalRegister();
  }

  bool IsBBLocalVReg() const override {
    return IsVirtualRegister() && RegOperand::IsBBLocalVReg();
  }

  bool IsSaveReg(MIRType &ty, BECommon &beCommon) const override;

  static AArch64RegOperand &Get32bitZeroRegister() {
    return zero32;
  }

  static AArch64RegOperand &Get64bitZeroRegister() {
    return zero64;
  }

  static AArch64RegOperand &GetZeroRegister(uint32 bitLen) {
    /*
     * It is possible to have a bitLen < 32, eg stb.
     * Set it to 32 if it is less than 32.
     */
    if (bitLen < k32BitSize) {
      bitLen = k32BitSize;
    }
    ASSERT((bitLen == k32BitSize || bitLen == k64BitSize), "illegal bit length = %d", bitLen);
    return (bitLen == k32BitSize) ? Get32bitZeroRegister() : Get64bitZeroRegister();
  }

  bool IsZeroRegister() const override {
    return GetRegisterNumber() == RZR;
  }

  Operand *Clone(MemPool &memPool) const override {
    return memPool.Clone<AArch64RegOperand>(*this);
  }

  bool operator==(const AArch64RegOperand &opnd) const;

  bool operator<(const AArch64RegOperand &opnd) const;

  void Emit(Emitter &emitter, const OpndProp *opndProp) const override;
  void Dump() const override {
    std::array<const std::string, kRegTyLast> prims = { "U", "R", "V", "C", "X", "Vra" };
    std::array<const std::string, kRegTyLast> classes = { "[U]", "[I]", "[F]", "[CC]", "[X87]", "[Vra]" };
    bool isVirtual = IsVirtualRegister();
    ASSERT(regType < kRegTyLast, "unexpected regType");
    regno_t reg = isVirtual ? regNO : (regNO - 1);
    LogInfo::MapleLogger() << (isVirtual ? "vreg:" : " reg:") << prims[regType] << reg
                           << " class: " << classes[regType] << " validBitNum: ["
                           << static_cast<uint32>(validBitsNum) << "]";
  }

  bool IsSPOrFP() const override;

 private:
  static AArch64RegOperand zero64;
  static AArch64RegOperand zero32;
  bool isRefField = false;
  uint32 flag;
};

/*
 * http://stackoverflow.com/questions/30904718/range-of-immediate-values-in-armv8-a64-assembly
 *
 * Unlike A32's "flexible second operand", there is no common
 * immediate format in A64. For immediate-operand data-processing
 * instructions (ignoring the boring and straightforward ones like shifts),
 *
 * 1. Arithmetic instructions (add{s}, sub{s}, cmp, cmn) take
 *    a 12-bit unsigned immediate with an optional 12-bit left shift.
 * 2. Move instructions (movz, movn, movk) take a 16-bit immediate
 *    optionally shifted to any 16-bit-aligned position within the register.
 * 3. Address calculations (adr, adrp) take a 21-bit signed immediate,
 *    although there's no actual syntax to specify it directly - to do
 *    so you'd have to resort to assembler expression trickery to generate
 *    an appropriate "label".
 * 4. Logical instructions (and{s}, orr, eor, tst) take a "bitmask immediate",
 *    which I'm not sure I can even explain, so I'll just quote the
 *    mind-bogglingly complicated definition:
 *    "Such an immediate is a 32-bit or 64-bit pattern viewed as a vector of
 *    identical elements of size e = 2, 4, 8, 16, 32, or 64 bits. Each element
 *    contains the same sub-pattern: a single run of 1 to e-1 non-zero bits,
 *    rotated by 0 to e-1 bits. This mechanism can generate 5,334 unique
 *    64-bit patterns (as 2,667 pairs of pattern and their bitwise inverse)."
 */
class AArch64ImmOperand : public ImmOperand {
 public:
  AArch64ImmOperand(int64 val, uint32 size, bool isSigned, VaryType varyType = kNotVary, bool isFmov = false)
      : ImmOperand(val, size, isSigned, varyType), isFmov(isFmov) {}

  ~AArch64ImmOperand() override = default;

  Operand *Clone(MemPool &memPool) const override {
    return memPool.Clone<AArch64ImmOperand>(*this);
  }

  bool IsBitmaskImmediate() const {
    ASSERT(!IsZero(), " 0 is reserved for bitmask immediate");
    ASSERT(!IsAllOnes(), " -1 is reserved for bitmask immediate");
    return maplebe::IsBitmaskImmediate(static_cast<uint64>(value), static_cast<uint32>(size));
  }

  bool IsBitmaskImmediate(uint32 destSize) {
    ASSERT(!IsZero(), " 0 is reserved for bitmask immediate");
    ASSERT(!IsAllOnes(), " -1 is reserved for bitmask immediate");
    return maplebe::IsBitmaskImmediate(static_cast<uint64>(value), static_cast<uint32>(destSize));
  }

  bool IsSingleInstructionMovable() const override {
    return (IsMoveWidableImmediate(static_cast<uint64>(value), static_cast<uint32>(size)) ||
            IsMoveWidableImmediate(~static_cast<uint64>(value), static_cast<uint32>(size)) ||
            IsBitmaskImmediate());
  }

  bool IsSingleInstructionMovable(uint32 destSize) {
    return (IsMoveWidableImmediate(static_cast<uint64>(value), static_cast<uint32>(destSize)) ||
            IsMoveWidableImmediate(~static_cast<uint64>(value), static_cast<uint32>(destSize)) ||
            IsBitmaskImmediate(destSize));
  }

  void Emit(Emitter &emitter, const OpndProp *prop) const override;

 private:
  bool isFmov;
};

class ImmFPZeroOperand : public Operand {
 public:
  explicit ImmFPZeroOperand(uint32 sz) : Operand(kOpdFPZeroImmediate, uint8(sz)) {}

  ~ImmFPZeroOperand() override = default;

  static ImmFPZeroOperand *allocate(uint8 sz) {
    CHECK_FATAL((sz == k32BitSize || sz == k64BitSize), "half-precession is yet to be supported");
    auto *memPool = static_cast<MemPool*>(CG::GetCurCGFuncNoConst()->GetMemoryPool());
    ImmFPZeroOperand *inst = memPool->New<ImmFPZeroOperand>(static_cast<uint32>(sz));
    return inst;
  }

  Operand *Clone(MemPool &memPool) const override {
    return memPool.Clone<ImmFPZeroOperand>(*this);
  }

  void Emit(Emitter &emitter, const OpndProp *opndProp) const override {
    (void)opndProp;
    emitter.Emit("#0.0");
  }

  bool Less(const Operand &right) const override {
    /* For different type. */
    return GetKind() < right.GetKind();
  }

  void Dump() const override {
    LogInfo::MapleLogger() << "imm fp" << size << ": 0.0";
  }
};

class AArch64OfstOperand : public OfstOperand {
 public:
  enum OfstType : uint8 {
    kSymbolOffset,
    kImmediateOffset,
    kSymbolImmediateOffset,
  };

  /* only for symbol offset */
  AArch64OfstOperand(const MIRSymbol &mirSymbol, uint32 size, int32 relocs)
      : OfstOperand(kOpdOffset, 0, size, true),
        offsetType(kSymbolOffset), symbol(&mirSymbol), relocs(relocs) {}
  /* only for Immediate offset */
  AArch64OfstOperand(int32 val, uint32 size, VaryType isVar = kNotVary)
      : OfstOperand(kOpdOffset, static_cast<int64>(val), size, true, isVar),
        offsetType(kImmediateOffset), symbol(nullptr), relocs(0) {}
  /* for symbol and Immediate offset */
  AArch64OfstOperand(const MIRSymbol &mirSymbol, int64 val, uint32 size, int32 relocs, VaryType isVar = kNotVary)
      : OfstOperand(kOpdOffset, val, size, true, isVar),
        offsetType(kSymbolImmediateOffset),
        symbol(&mirSymbol),
        relocs(relocs) {}

  ~AArch64OfstOperand() override = default;

  Operand *Clone(MemPool &memPool) const override {
    return memPool.Clone<AArch64OfstOperand>(*this);
  }

  bool IsSymOffset() const {
    return offsetType == kSymbolOffset;
  }
  bool IsImmOffset() const {
    return offsetType == kImmediateOffset;
  }
  bool IsSymAndImmOffset() const {
    return offsetType == kSymbolImmediateOffset;
  }

  const MIRSymbol *GetSymbol() const {
    return symbol;
  }

  const std::string &GetSymbolName() const {
    return symbol->GetName();
  }

  int32 GetOffsetValue() const {
    return GetValue();
  }

  void SetOffsetValue(int32 offVal) {
    SetValue(static_cast<int64>(offVal));
  }

  void AdjustOffset(int32 delta) {
    Add(static_cast<int64>(delta));
  }

  bool operator==(const AArch64OfstOperand &opnd) const {
    return (offsetType == opnd.offsetType && symbol == opnd.symbol &&
            OfstOperand::operator==(opnd) && relocs == opnd.relocs);
  }

  bool operator<(const AArch64OfstOperand &opnd) const {
    return (offsetType < opnd.offsetType ||
            (offsetType == opnd.offsetType && symbol < opnd.symbol) ||
            (offsetType == opnd.offsetType && symbol == opnd.symbol && GetValue() < opnd.GetValue()));
  }

  void Emit(Emitter &emitter, const OpndProp *prop) const override;

  void Dump() const override {
    if (IsImmOffset()) {
      LogInfo::MapleLogger() << "ofst:" << GetValue();
    } else {
      LogInfo::MapleLogger() << GetSymbolName();
      LogInfo::MapleLogger() << "+offset:" << GetValue();
    }
  }

  bool IsBitmaskImmediate() const {
    ASSERT(!IsZero(), "0 is reserved for bitmask immediate");
    ASSERT(!IsAllOnes(), "-1 is reserved for bitmask immediate");
    return maplebe::IsBitmaskImmediate(static_cast<uint64>(value), static_cast<uint32>(size));
  }

  bool IsSingleInstructionMovable() const override {
    return (IsMoveWidableImmediate(static_cast<uint64>(value), static_cast<uint32>(size)) ||
            IsMoveWidableImmediate(~static_cast<uint64>(value), static_cast<uint32>(size)) ||
            IsBitmaskImmediate());
  }

 private:
  OfstType offsetType;
  const MIRSymbol *symbol;
  int32 relocs;
};

/* representing for global variables address */
class StImmOperand : public Operand {
 public:
  StImmOperand(const MIRSymbol &symbol, int64 offset, int32 relocs)
      : Operand(kOpdStImmediate, 0), symbol(&symbol), offset(offset), relocs(relocs) {}

  ~StImmOperand() override = default;

  Operand *Clone(MemPool &memPool) const override {
    return memPool.Clone<StImmOperand>(*this);
  }

  const MIRSymbol *GetSymbol() const {
    return symbol;
  }

  const std::string &GetName() const {
    return symbol->GetName();
  }

  int64 GetOffset() const {
    return offset;
  }

  int32 GetRelocs() const {
    return relocs;
  }

  bool operator==(const StImmOperand &opnd) const {
    return (symbol == opnd.symbol && offset == opnd.offset && relocs == opnd.relocs);
  }

  bool operator<(const StImmOperand &opnd) const {
    return (symbol < opnd.symbol || (symbol == opnd.symbol && offset < opnd.offset) ||
            (symbol == opnd.symbol && offset == opnd.offset && relocs < opnd.relocs));
  }

  bool Less(const Operand &right) const override;

  void Emit(Emitter &emitter, const OpndProp *opndProp) const override;

  void Dump() const override {
    LogInfo::MapleLogger() << GetName();
    LogInfo::MapleLogger() << "+offset:" << offset;
  }

 private:
  const MIRSymbol *symbol;
  int64 offset;
  int32 relocs;
};

class FunctionLabelOperand : public LabelOperand {
 public:
  explicit FunctionLabelOperand(const char *func) : LabelOperand(func, 0) {}

  ~FunctionLabelOperand() override = default;

  Operand *Clone(MemPool &memPool) const override {
    return memPool.Clone<FunctionLabelOperand>(*this);
  }

  void Emit(Emitter &emitter, const OpndProp *opndProp) const override {
    (void)opndProp;
    emitter.Emit(parentFunc);
  }

  void Dump() const override {
    LogInfo::MapleLogger() << "func :" << parentFunc;
  }
};

/* Use StImmOperand instead? */
class FuncNameOperand : public Operand {
 public:
  explicit FuncNameOperand(const MIRSymbol &fsym) : Operand(kOpdBBAddress, 0), symbol(&fsym) {}

  ~FuncNameOperand() override = default;

  Operand *Clone(MemPool &memPool) const override {
    return memPool.Clone<FuncNameOperand>(*this);
  }

  const std::string &GetName() const {
    return symbol->GetName();
  }

  const MIRSymbol *GetFunctionSymbol() const {
    return symbol;
  }

  bool IsFuncNameOpnd() const override {
    return true;
  }

  void SetFunctionSymbol(const MIRSymbol &fsym) {
    symbol = &fsym;
  }

  void Emit(Emitter &emitter, const OpndProp *opndProp) const override {
    (void)opndProp;
    emitter.Emit(GetName());
  }

  bool Less(const Operand &right) const override {
    if (&right == this) {
      return false;
    }
    /* For different type. */
    if (GetKind() != right.GetKind()) {
      return GetKind() < right.GetKind();
    }

    auto *rightOpnd = static_cast<const FuncNameOperand*>(&right);

    return static_cast<const void*>(symbol) < static_cast<const void*>(rightOpnd->symbol);
  }

  void Dump() const override {
    LogInfo::MapleLogger() << GetName();
  }

 private:
  const MIRSymbol *symbol;
};

class AArch64CGFunc;

/*
 * Table C1-6 A64 Load/Store addressing modes
 * |         Offset
 * Addressing Mode    | Immediate     | Register             | Extended Register
 *
 * Base register only | [base{,#0}]   | -                    | -
 * (no offset)        | B_OI_NONE     |                      |
 *                   imm=0
 *
 * Base plus offset   | [base{,#imm}] | [base,Xm{,LSL #imm}] | [base,Wm,(S|U)XTW {#imm}]
 *                  B_OI_NONE     | B_OR_X               | B_OR_X
 *                                   imm=0,1 (0,3)        | imm=00,01,10,11 (0/2,s/u)
 *
 * Pre-indexed        | [base, #imm]! | -                    | -
 *
 * Post-indexed       | [base], #imm  | [base], Xm(a)        | -
 *
 * Literal            | label         | -                    | -
 * (PC-relative)
 *
 * a) The post-indexed by register offset mode can be used with the SIMD Load/Store
 * structure instructions described in Load/Store Vector on page C3-154. Otherwise
 * the post-indexed by register offset mode is not avacilable.
 */
class AArch64MemOperand : public MemOperand {
 public:
  enum AArch64AddressingMode : uint8 {
    kAddrModeUndef,
    /* AddrMode_BO, base, offset. EA = [base] + offset; */
    kAddrModeBOi,  /* INTACT: EA = [base]+immediate */
    /*
     * PRE: base += immediate, EA = [base]
     * POST: EA = [base], base += immediate
     */
    kAddrModeBOrX,  /* EA = [base]+Extend([offreg/idxreg]), OR=Wn/Xn */
    kAddrModeLiteral,  /* AArch64 insruction LDR takes literal and */
    /*
     * "calculates an address from the PC value and an immediate offset,
     * loads a word from memory, and writes it to a register."
     */
    kAddrModeLo12Li  // EA = [base] + #:lo12:Label+immediate. (Example: [x0, #:lo12:__Label300+456]
  };
  /*
   * ARMv8-A A64 ISA Overview by Matteo Franchin @ ARM
   * o Address to load from/store to is a 64-bit base register + an optional offset
   *   LDR X0, [X1] ; Load from address held in X1
   *   STR X0, [X1] ; Store to address held in X1
   *
   * o Offset can be an immediate or a register
   *   LDR X0, [X1, #8]  ; Load from address [X1 + 8 bytes]
   *   LDR X0, [X1, #-8] ; Load with negative offset
   *   LDR X0, [X1, X2]  ; Load from address [X1 + X2]
   *
   * o A Wn register offset needs to be extended to 64 bits
   *  LDR X0, [X1, W2, SXTW] ; Sign-extend offset in W2
   *   LDR X0, [X1, W2, UXTW] ; Zero-extend offset in W2
   *
   * o Both Xn and Wn register offsets can include an optional left-shift
   *   LDR X0, [X1, W2, UXTW #2] ; Zero-extend offset in W2 & left-shift by 2
   *   LDR X0, [X1, X2, LSL #2]  ; Left-shift offset in X2 by 2
   *
   * p.15
   * Addressing Modes                       Analogous C Code
   *                                       int *intptr = ... // X1
   *                                       int out; // W0
   * o Simple: X1 is not changed
   *   LDR W0, [X1]                        out = *intptr;
   * o Offset: X1 is not changed
   *   LDR W0, [X1, #4]                    out = intptr[1];
   * o Pre-indexed: X1 changed before load
   *   LDR W0, [X1, #4]! =|ADD X1,X1,#4    out = *(++intptr);
   * |LDR W0,[X1]
   * o Post-indexed: X1 changed after load
   *   LDR W0, [X1], #4  =|LDR W0,[X1]     out = *(intptr++);
   * |ADD X1,X1,#4
   */
  enum ExtendInfo : uint8 {
    kShiftZero = 0x1,
    kShiftOne = 0x2,
    kShiftTwo = 0x4,
    kShiftThree = 0x8,
    kUnsignedExtend = 0x10,
    kSignExtend = 0x20
  };

  enum IndexingOption : uint8 {
    kIntact,     /* base register stays the same */
    kPreIndex,   /* base register gets changed before load */
    kPostIndex,  /* base register gets changed after load */
  };

  AArch64MemOperand(AArch64reg reg, int32 offset, uint32 size, IndexingOption idxOpt = kIntact)
      : MemOperand(size,
                   CG::GetCurCGFuncNoConst()->GetMemoryPool()->New<AArch64RegOperand>(reg, k64BitSize, kRegTyInt),
                   nullptr,
                   CG::GetCurCGFuncNoConst()->GetMemoryPool()->New<AArch64OfstOperand>(offset, k32BitSize), nullptr),
        addrMode(kAddrModeBOi),
        extend(0),
        idxOpt(idxOpt),
        noExtend(false),
        isStackMem(false) {
    if (reg == RSP || reg == RFP) {
      isStackMem = true;
    }
  }

  AArch64MemOperand(AArch64AddressingMode mode, uint32 size, RegOperand &base, RegOperand *index,
                    OfstOperand *offset, const MIRSymbol *symbol)
      : MemOperand(size, &base, index, offset, symbol),
        addrMode(mode),
        extend(0),
        idxOpt(kIntact),
        noExtend(false),
        isStackMem(false) {
    if (base.GetRegisterNumber() == RSP || base.GetRegisterNumber() == RFP) {
      isStackMem = true;
    }
  }

  AArch64MemOperand(AArch64AddressingMode mode, uint32 size, RegOperand &base, RegOperand &index,
                    OfstOperand *offset, const MIRSymbol &symbol, bool noExtend)
      : MemOperand(size, &base, &index, offset, &symbol),
        addrMode(mode),
        extend(0),
        idxOpt(kIntact),
        noExtend(noExtend),
        isStackMem(false) {
    if (base.GetRegisterNumber() == RSP || base.GetRegisterNumber() == RFP) {
      isStackMem = true;
    }
  }

  AArch64MemOperand(AArch64AddressingMode mode, uint32 dSize, RegOperand &baseOpnd, RegOperand &indexOpnd,
                    uint32 shift, bool isSigned = false)
      : MemOperand(dSize, &baseOpnd, &indexOpnd, nullptr, nullptr),
        addrMode(mode),
        extend((isSigned ? kSignExtend : kUnsignedExtend) | (1U << shift)),
        idxOpt(kIntact),
        noExtend(false),
        isStackMem(false) {
    ASSERT(dSize == (k8BitSize << shift), "incompatible data size and shift amount");
    if (baseOpnd.GetRegisterNumber() == RSP || baseOpnd.GetRegisterNumber() == RFP) {
      isStackMem = true;
    }
  }

  AArch64MemOperand(AArch64AddressingMode mode, uint32 dSize, const MIRSymbol &sym)
      : MemOperand(dSize, nullptr, nullptr, nullptr, &sym),
        addrMode(mode),
        extend(0),
        idxOpt(kIntact),
        noExtend(false),
        isStackMem(false) {
    ASSERT(mode == kAddrModeLiteral, "This constructor version is supposed to be used with AddrMode_Literal only");
  }

  ~AArch64MemOperand() override = default;

  /*
     Copy constructor
   */
  explicit AArch64MemOperand(const AArch64MemOperand &memOpnd)
      : MemOperand(memOpnd), addrMode(memOpnd.addrMode), extend(memOpnd.extend), idxOpt(memOpnd.idxOpt),
        noExtend(memOpnd.noExtend), isStackMem(memOpnd.isStackMem) {}
  AArch64MemOperand &operator=(const AArch64MemOperand &memOpnd) = default;

  Operand *Clone(MemPool &memPool) const override {
    return memPool.Clone<AArch64MemOperand>(*this);
  }

  AArch64AddressingMode GetAddrMode() const {
    return addrMode;
  }

  const std::string &GetSymbolName() const {
    return GetSymbol()->GetName();
  }

  void SetBaseRegister(AArch64RegOperand &baseRegOpnd) {
    MemOperand::SetBaseRegister(baseRegOpnd);
  }

  bool IsStackMem() const {
    return isStackMem;
  }

  void SetStackMem(bool isStack) {
    isStackMem = isStack;
  }

  RegOperand *GetOffsetRegister() const {
    return MemOperand::GetIndexRegister();
  }

  Operand *GetOffset() const override;

  void SetOffsetRegister(AArch64RegOperand &osr) {
    MemOperand::SetIndexRegister(osr);
  }

  AArch64OfstOperand *GetOffsetImmediate() const {
    return static_cast<AArch64OfstOperand*>(GetOffsetOperand());
  }

  void SetOffsetImmediate(OfstOperand &ofstOpnd) {
    MemOperand::SetOffsetOperand(ofstOpnd);
  }

  /* Returns N where alignment == 2^N */
  static int32 GetImmediateOffsetAlignment(uint32 dSize) {
    ASSERT(dSize >= k8BitSize, "error val:dSize");
    ASSERT(dSize <= k64BitSize, "error val:dSize");
    ASSERT((dSize & (dSize - 1)) == 0, "error val:dSize");
    /* dSize==8: 0, dSize==16 : 1, dSize==32: 2, dSize==64: 3 */
    return __builtin_ctz(dSize) - kBaseOffsetAlignment;
  }

  static int32 GetMaxPIMM(uint32 dSize) {
    ASSERT(dSize >= k8BitSize, "error val:dSize");
    ASSERT(dSize <= k64BitSize, "error val:dSize");
    ASSERT((dSize & (dSize - 1)) == 0, "error val:dSize");
    int32 alignment = GetImmediateOffsetAlignment(dSize);
    /* alignment is between kAlignmentOf8Bit and kAlignmentOf64Bit */
    ASSERT(alignment >= kOffsetAlignmentOf8Bit, "error val:alignment");
    ASSERT(alignment <= kOffsetAlignmentOf64Bit, "error val:alignment");
    return (kMaxPimms[alignment]);
  }

  bool IsOffsetMisaligned(uint32 dSize) const {
    ASSERT(dSize >= k8BitSize, "error val:dSize");
    ASSERT(dSize <= k64BitSize, "error val:dSize");
    ASSERT((dSize & (dSize - 1)) == 0, "error val:dSize");
    if (dSize == k8BitSize || addrMode != kAddrModeBOi) {
      return false;
    }
    AArch64OfstOperand *ofstOpnd = GetOffsetImmediate();
    return ((static_cast<uint32>(ofstOpnd->GetOffsetValue()) &
             static_cast<uint32>((1U << static_cast<uint32>(GetImmediateOffsetAlignment(dSize))) - 1)) != 0);
  }

  static bool IsSIMMOffsetOutOfRange(int32 offset, bool is64bit, bool isLDSTPair) {
    if (!isLDSTPair) {
      return (offset < kLdStSimmLowerBound || offset > kLdStSimmUpperBound);
    }
    if (is64bit) {
      return (offset < kLdpStp64SimmLowerBound || offset > kLdpStp64SimmUpperBound);
    }
    return (offset < kLdpStp32SimmLowerBound || offset > kLdpStp32SimmUpperBound);
  }

  static bool IsPIMMOffsetOutOfRange(int32 offset, uint32 dSize) {
    ASSERT(dSize >= k8BitSize, "error val:dSize");
    ASSERT(dSize <= k64BitSize, "error val:dSize");
    ASSERT((dSize & (dSize - 1)) == 0, "error val:dSize");
    return (offset < 0 || offset > GetMaxPIMM(dSize));
  }

  bool operator<(const AArch64MemOperand &opnd) const {
    return addrMode < opnd.addrMode ||
           (addrMode == opnd.addrMode && GetBaseRegister() < opnd.GetBaseRegister()) ||
           (addrMode == opnd.addrMode && GetBaseRegister() == opnd.GetBaseRegister() &&
            GetIndexRegister() < opnd.GetIndexRegister()) ||
           (addrMode == opnd.addrMode && GetBaseRegister() == opnd.GetBaseRegister() &&
            GetIndexRegister() == opnd.GetIndexRegister() && GetOffsetOperand() < opnd.GetOffsetOperand()) ||
           (addrMode == opnd.addrMode && GetBaseRegister() == opnd.GetBaseRegister() &&
            GetIndexRegister() == opnd.GetIndexRegister() && GetOffsetOperand() == opnd.GetOffsetOperand() &&
            GetSymbol() < opnd.GetSymbol()) ||
           (addrMode == opnd.addrMode && GetBaseRegister() == opnd.GetBaseRegister() &&
            GetIndexRegister() == opnd.GetIndexRegister() && GetOffsetOperand() == opnd.GetOffsetOperand() &&
            GetSymbol() == opnd.GetSymbol() && GetSize() < opnd.GetSize());
  }

  bool Less(const Operand &right) const override;

  bool NoAlias(AArch64MemOperand &rightOpnd) const;

  VaryType GetMemVaryType() override {
    Operand *ofstOpnd = GetOffsetOperand();
    if (ofstOpnd != nullptr) {
      auto *opnd = static_cast<AArch64OfstOperand*>(ofstOpnd);
      return opnd->GetVary();
    }
    return kNotVary;
  }

  bool IsExtendedRegisterMode() const {
    return addrMode == kAddrModeBOrX;
  }

  bool SignedExtend() const {
    return IsExtendedRegisterMode() && ((extend & kSignExtend) != 0);
  }

  bool UnsignedExtend() const {
    return IsExtendedRegisterMode() && !SignedExtend();
  }

  int32 ShiftAmount() const {
    int32 scale = extend & 0xF;
    ASSERT(IsExtendedRegisterMode(), "Just checking");
    /* 8 is 1 << 3, 4 is 1 << 2, 2 is 1 << 1, 1 is 1 << 0; */
    return (scale == 8) ? 3 : ((scale == 4) ? 2 : ((scale == 2) ? 1 : 0));
  }

  bool ShouldEmitExtend() const {
    return !noExtend && ((extend & 0x3F) != 0);
  }

  bool IsIntactIndexed() const {
    return idxOpt == kIntact;
  }

  bool IsPostIndexed() const {
    return idxOpt == kPostIndex;
  }

  bool IsPreIndexed() const {
    return idxOpt == kPreIndex;
  }

  std::string GetExtendAsString() const {
    if (GetOffsetRegister()->GetSize() == k64BitSize) {
      return std::string("LSL");
    }
    return ((extend & kSignExtend) != 0) ? std::string("SXTW") : std::string("UXTW");
  }

  void Emit(Emitter &emitter, const OpndProp *opndProp) const override;

  void Dump() const override;

  /* Return true if given operand has the same base reg and offset with this. */
  bool Equals(Operand &operand) const override;
  bool Equals(AArch64MemOperand &opnd) const;

 private:
  static constexpr int32 kLdStSimmLowerBound = -256;
  static constexpr int32 kLdStSimmUpperBound = 255;

  static constexpr int32 kLdpStp32SimmLowerBound = -256;  /* multiple of 4 */
  static constexpr int32 kLdpStp32SimmUpperBound = 252;

  static constexpr int32 kLdpStp64SimmLowerBound = -512;  /* multiple of 8 */
  static constexpr int32 kLdpStp64SimmUpperBound = 504;

  static constexpr int32 kMaxPimm8 = 4095;
  static constexpr int32 kMaxPimm16 = 8190;
  static constexpr int32 kMaxPimm32 = 16380;
  static constexpr int32 kMaxPimm64 = 32760;

  static const int32 kMaxPimms[4];

  AArch64AddressingMode addrMode;

  uint32 extend;        /* used with offset register ; AddrMode_B_OR_X */

  IndexingOption idxOpt;  /* used with offset immediate ; AddrMode_B_OI */

  bool noExtend;

  bool isStackMem;
};

class AArch64ListOperand : public ListOperand {
 public:
  explicit AArch64ListOperand(MapleAllocator &allocator) : ListOperand(allocator) {}

  ~AArch64ListOperand() override = default;

  Operand *Clone(MemPool &memPool) const override {
    return memPool.Clone<AArch64ListOperand>(*this);
  }

  void Emit(Emitter &emitter, const OpndProp *opndProp) const override;
};

class CondOperand : public Operand {
 public:
  explicit CondOperand(AArch64CC_t cc) : Operand(Operand::kOpdCond, k4ByteSize), cc(cc) {}

  ~CondOperand() override = default;

  Operand *Clone(MemPool &memPool) const override {
    return memPool.New<CondOperand>(cc);
  }

  AArch64CC_t GetCode() const {
    return cc;
  }

  void Emit(Emitter &emitter, const OpndProp *opndProp) const override {
    (void)opndProp;
    emitter.Emit(ccStrs[cc]);
  }

  bool Less(const Operand &right) const override;

  void Dump() const override {
    LogInfo::MapleLogger() << "CC: " << ccStrs[cc];
  }

 private:
  static const char *ccStrs[kCcLast];
  AArch64CC_t cc;
};

/* used with MOVK */
class LogicalShiftLeftOperand : public Operand {
 public:
  /*
   * Do not make the constructor public unless you are sure you know what you are doing.
   * Only AArch64CGFunc is supposed to create LogicalShiftLeftOperand objects
   * as part of initialization
   */
  LogicalShiftLeftOperand(uint32 amt, int32 bitLen)
      : Operand(Operand::kOpdShift, bitLen), shiftAmount(amt) {} /* bitlength is equal to 4 or 6 */

  ~LogicalShiftLeftOperand() override = default;

  Operand *Clone(MemPool &memPool) const override {
    return memPool.Clone<LogicalShiftLeftOperand>(*this);
  }

  void Emit(Emitter &emitter, const OpndProp *opndProp) const override {
    (void)opndProp;
    emitter.Emit(" LSL #").Emit(shiftAmount);
  }

  bool Less(const Operand &right) const override {
    if (&right == this) {
      return false;
    }

    /* For different type. */
    if (GetKind() != right.GetKind()) {
      return GetKind() < right.GetKind();
    }

    auto *rightOpnd = static_cast<const LogicalShiftLeftOperand*>(&right);

    /* The same type. */
    return shiftAmount < rightOpnd->shiftAmount;
  }

  uint32 GetShiftAmount() const {
    return shiftAmount;
  }

  void Dump() const override {
    LogInfo::MapleLogger() << "LSL: " << shiftAmount;
  }

 private:
  uint32 shiftAmount;
};

class ExtendShiftOperand : public Operand {
 public:
  enum ExtendOp : uint8 {
    kSXTW,
  };

  ExtendShiftOperand(ExtendOp op, uint32 amt, int32 bitLen)
      : Operand(Operand::kOpdExtend, bitLen), extendOp(op), shiftAmount(amt) {}

  ~ExtendShiftOperand() override = default;

  Operand *Clone(MemPool &memPool) const override {
    return memPool.Clone<ExtendShiftOperand>(*this);
  }

  void Emit(Emitter &emitter, const OpndProp *prop) const override {
    (void)prop;
    switch (extendOp) {
      case kSXTW:
        emitter.Emit("SXTW #").Emit(shiftAmount);
        break;
      default:
        ASSERT(false, "should not be here");
        break;
    }
  }

  bool Less(const Operand &right) const override;

  void Dump() const override {
    switch (extendOp) {
      case kSXTW:
        LogInfo::MapleLogger() << "SXTW: ";
        break;
      default:
        ASSERT(false, "should not be here");
        break;
    }
    LogInfo::MapleLogger() << shiftAmount;
  }

 private:
  ExtendOp extendOp;
  uint32 shiftAmount;
};

class BitShiftOperand : public Operand {
 public:
  enum ShiftOp : uint8 {
    kLSL, /* logical shift left */
    kLSR, /* logical shift right */
    kASR, /* arithmetic shift right */
  };

  BitShiftOperand(ShiftOp op, uint32 amt, int32 bitLen)
      : Operand(Operand::kOpdShift, bitLen), shiftOp(op), shiftAmount(amt) {} /* bitlength is equal to 5 or 6 */

  ~BitShiftOperand() override = default;

  Operand *Clone(MemPool &memPool) const override {
    return memPool.Clone<BitShiftOperand>(*this);
  }

  void Emit(Emitter &emitter, const OpndProp *prop) const override {
    (void)prop;
    emitter.Emit((shiftOp == kLSL) ? "LSL #" : ((shiftOp == kLSR) ? "LSR #" : "ASR #")).Emit(shiftAmount);
  }

  bool Less(const Operand &right) const override;

  uint32 GetShiftAmount() {
    return shiftAmount;
  }

  void Dump() const override {
    LogInfo::MapleLogger() << ((shiftOp == kLSL) ? "LSL: " : ((shiftOp == kLSR) ? "LSR: " : "ASR: "));
    LogInfo::MapleLogger() << shiftAmount;
  }

 private:
  ShiftOp shiftOp;
  uint32 shiftAmount;
};

class CommentOperand : public Operand {
 public:
  CommentOperand(const char *str, MemPool &memPool)
      : Operand(Operand::kOpdString, 0), comment(str, &memPool) {}

  CommentOperand(const std::string &str, MemPool &memPool)
      : Operand(Operand::kOpdString, 0), comment(str, &memPool) {}

  ~CommentOperand() override = default;

  const MapleString &GetComment() const {
    return comment;
  }

  Operand *Clone(MemPool &memPool) const override {
    return memPool.Clone<CommentOperand>(*this);
  }

  bool IsCommentOpnd() const override {
    return true;
  }

  void Emit(Emitter &emitter, const OpndProp *opndProp) const override {
    (void)opndProp;
    emitter.Emit(comment);
  }

  bool Less(const Operand &right) const override {
    /* For different type. */
    return GetKind() < right.GetKind();
  }

  void Dump() const override {
    LogInfo::MapleLogger() << "# " << comment << std::endl;
  }

 private:
  const MapleString comment;
};
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_OPERAND_H */