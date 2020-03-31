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
#ifndef MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_ISA_H
#define MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_ISA_H

#include "operand.h"
#include "mad.h"

enum AArch64MOP_t : maple::uint32 {
#include "aarch64_isa.def"
  kMopLast
};

namespace maplebe {
/*
 * ARM Architecture Reference Manual (for ARMv8)
 * D1.8.2
 */
constexpr int kAarch64StackPtrAlignment = 16;

constexpr int32 kOffsetAlign = 8;
constexpr uint32 kIntregBytelen = 8;   /* 64-bit */
constexpr uint32 kFpregBytelen = 8;    /* only lower 64 bits are used */
constexpr int kSizeOfFplr = 16;

enum StpLdpImmBound : int {
  kStpLdpImm64LowerBound = -512,
  kStpLdpImm64UpperBound = 504,
  kStpLdpImm32LowerBound = -256,
  kStpLdpImm32UpperBound = 252
};

enum StrLdrPerPostBound : int64 {
  kStrLdrPerPostLowerBound = -256,
  kStrLdrPerPostUpperBound = 255
};
constexpr int64 kStrAllLdrAllImmLowerBound = 0;
enum StrLdrImmUpperBound : int64 {
  kStrLdrImm32UpperBound = 16380, /* must be a multiple of 4 */
  kStrLdrImm64UpperBound = 32760, /* must be a multiple of 8 */
  kStrbLdrbImmUpperBound = 4095,
  kStrhLdrhImmUpperBound = 8190
};

/* AArch64 Condition Code Suffixes */
enum AArch64CC_t {
#define CONDCODE(a) CC_##a,
#include "aarch64_cc.def"
#undef CONDCODE
  kCcLast
};

/*
 * ARM Compiler armasm User Guide version 6.6.
 * http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0473j/deb1353594352617.html
 * (retrieved on 3/24/2017)
 *
 * $ 4.1 Registers in AArch64 state
 * ...When you use the 32-bit form of an instruction, the upper
 * 32 bits of the source registers are ignored and
 * the upper 32 bits of the destination register are set to zero.
 * ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 *
 * There is no register named W31 or X31.
 * Depending on the instruction, register 31 is either the stack
 * pointer or the zero register. When used as the stack pointer,
 * you refer to it as "SP". When used as the zero register, you refer
 * to it as WZR in a 32-bit context or XZR in a 64-bit context.
 * The zero register returns 0 when read and discards data when
 * written (e.g., when setting the status register for testing).
 */
enum AArch64reg : uint32 {
  kRinvalid = kInvalidRegNO,
/* integer registers */
#define INT_REG(ID, PREF32, PREF64, canBeAssigned, isCalleeSave, isParam, isSpill, isExtraSpill) R##ID,
#define INT_REG_ALIAS(ALIAS, ID, PREF32, PREF64)
#include "aarch64_int_regs.def"
#undef INT_REG
#undef INT_REG_ALIAS
/* fp-simd registers */
#define FP_SIMD_REG(ID, PV, P8, P16, P32, P64, P128, canBeAssigned, isCalleeSave, isParam, isSpill, isExtraSpill) V##ID,
#define FP_SIMD_REG_ALIAS(ID)
#include "aarch64_fp_simd_regs.def"
#undef FP_SIMD_REG
#undef FP_SIMD_REG_ALIAS
  kMaxRegNum,
  kRFLAG,
  kAllRegNum,
/* alias */
#define INT_REG(ID, PREF32, PREF64, canBeAssigned, isCalleeSave, isParam, isSpill, isExtraSpill)
#define INT_REG_ALIAS(ALIAS, ID, PREF32, PREF64) R##ALIAS = R##ID,
#include "aarch64_int_regs.def"
#undef INT_REG
#undef INT_REG_ALIAS
#define FP_SIMD_REG(ID, PV, P8, P16, P32, P64, P128, canBeAssigned, isCalleeSave, isParam, isSpill, isExtraSpill)
#define FP_SIMD_REG_ALIAS(ID) S##ID = V##ID,
#include "aarch64_fp_simd_regs.def"
#undef FP_SIMD_REG
#undef FP_SIMD_REG_ALIAS
#define FP_SIMD_REG(ID, PV, P8, P16, P32, P64, P128, canBeAssigned, isCalleeSave, isParam, isSpill, isExtraSpill)
#define FP_SIMD_REG_ALIAS(ID) D##ID = V##ID,
#include "aarch64_fp_simd_regs.def"
#undef FP_SIMD_REG
#undef FP_SIMD_REG_ALIAS
  kNArmRegisters
};

namespace AArch64isa {
static inline bool IsGPRegister(AArch64reg r) {
  return R0 <= r && r <= RZR;
}

static inline bool IsFPSIMDRegister(AArch64reg r) {
  return V0 <= r && r <= V31;
}

static inline bool IsPhysicalRegister(regno_t r) {
  return r < kMaxRegNum;
}

static inline RegType GetRegType(AArch64reg r) {
  if (IsGPRegister(r)) {
    return kRegTyInt;
  }
  if (IsFPSIMDRegister(r)) {
    return kRegTyFloat;
  }
  ASSERT(false, "No suitable register type to return?");
  return kRegTyUndef;
}

enum MemoryOrdering : uint32 {
  kMoNone = 0,
  kMoAcquire = (1ULL << 0),      /* ARMv8 */
  kMoAcquireRcpc = (1ULL << 1),  /* ARMv8.3 */
  kMoLoacquire = (1ULL << 2),    /* ARMv8.1 */
  kMoRelease = (1ULL << 3),      /* ARMv8 */
  kMoLorelease = (1ULL << 4)     /* ARMv8.1 */
};
}  /* namespace AArch64isa */

enum RegPropState : uint32 {
  kRegPropUndef = 0,
  kRegPropDef = 0x1,
  kRegPropUse = 0x2
};
enum RegAddress : uint32 {
  kRegHigh = 0x4,
  kRegLow = 0x8
};
constexpr uint32 kMemLow12 = 0x10;
constexpr uint32 kLiteralLow12 = kMemLow12;
constexpr uint32 kPreInc = 0x20;
constexpr uint32 kPostInc = 0x40;
constexpr uint32 kLoadLiteral = 0x80;

class RegProp {
 public:
  RegProp(RegType t, AArch64reg r, uint32 d) : regType(t), physicalReg(r), defUse(d) {}
  virtual ~RegProp() = default;
  const RegType &GetRegType() const {
    return regType;
  }
  const AArch64reg &GetPhysicalReg() const {
    return physicalReg;
  }
  uint32 GetDefUse() const {
    return defUse;
  }
 private:
  RegType regType;
  AArch64reg physicalReg;
  uint32 defUse;  /* used for register use/define and other properties of other operand */
};

class AArch64OpndProp : public OpndProp {
 public:
  AArch64OpndProp(Operand::OperandType t, RegProp p, uint8 s) : opndType(t), regProp(p), size(s) {}
  virtual ~AArch64OpndProp() = default;
  Operand::OperandType GetOperandType() {
    return opndType;
  }

  const RegProp &GetRegProp() const {
    return regProp;
  }

  bool IsPhysicalRegister() const {
    return opndType == Operand::kOpdRegister && regProp.GetPhysicalReg() < kMaxRegNum;
  }

  bool IsRegister() const {
    return opndType == Operand::kOpdRegister;
  }

  bool IsRegDef() const {
    return opndType == Operand::kOpdRegister && (regProp.GetDefUse() & kRegPropDef);
  }

  bool IsRegUse() const {
    return opndType == Operand::kOpdRegister && (regProp.GetDefUse() & kRegPropUse);
  }

  bool IsMemLow12() const {
    return opndType == Operand::kOpdMem && (regProp.GetDefUse() & kMemLow12);
  }

  bool IsLiteralLow12() const {
    return opndType == Operand::kOpdStImmediate && (regProp.GetDefUse() & kLiteralLow12);
  }

  bool IsDef() const {
    return regProp.GetDefUse() & kRegPropDef;
  }

  bool IsUse() const {
    return regProp.GetDefUse() & kRegPropUse;
  }

  bool IsLoadLiteral() const {
    return regProp.GetDefUse() & kLoadLiteral;
  }

  uint8 GetSize() const {
    return size;
  }

  uint32 GetOperandSize() const {
    return static_cast<uint32>(size);
  }

 private:
  Operand::OperandType opndType;
  RegProp regProp;
  uint8 size;
};

struct AArch64MD {
  MOperator opc;
  std::vector<OpndProp*> operand;
  uint64 properties;
  LatencyType latencyType;
  const std::string &name;
  const std::string &format;
  uint32 atomicNum; /* indicate how many asm instructions it will emit. */

  bool UseSpecReg() const {
    return properties & USESPECREG;
  }

  uint32 GetAtomicNum() const {
    return atomicNum;
  }

  bool IsCall() const {
    return properties & ISCALL;
  }

  bool HasLoop() const {
    return properties & HASLOOP;
  }

  bool CanThrow() const {
    return properties & CANTHROW;
  }

  AArch64OpndProp *GetOperand(int nth) const {
    ASSERT(nth < operand.size(), "index of Operand should not be bigger than MaxOperandNum");
    return static_cast<AArch64OpndProp*>(operand[nth]);
  }

  uint32 GetOperandSize() const {
    if (properties & (ISLOAD | ISSTORE)) {
      /* use memory operand */
      return GetOperand(1)->GetOperandSize();
    }
    /* use dest operand */
    return GetOperand(0)->GetOperandSize();
  }

  bool Is64Bit() const {
    return GetOperandSize() == k64BitSize;
  }

  bool IsVolatile() const {
    return ((properties & HASRELEASE) != 0) || ((properties & HASACQUIRE) != 0);
  }

  bool IsMemAccessBar() const {
    return (properties & (HASRELEASE | HASACQUIRE | HASACQUIRERCPC | HASLOACQUIRE | HASLORELEASE)) != 0;
  }

  bool IsMemAccess() const {
    return (properties & (ISLOAD | ISSTORE | ISLOADPAIR | ISSTOREPAIR)) != 0;
  }

  bool IsBranch() const {
    return (properties & (ISCONDBRANCH | ISUNCONDBRANCH)) != 0;
  }

  bool IsCondBranch() const {
    return (properties & (ISCONDBRANCH)) != 0;
  }

  bool IsUnCondBranch() const {
    return (properties & (ISUNCONDBRANCH)) != 0;
  }

  bool IsMove() const {
    return (properties & (ISMOVE)) != 0;
  }

  bool IsDMB() const {
    return (properties & (ISDMB)) != 0;
  }

  bool IsLoad() const {
    return (properties & (ISLOAD)) != 0;
  }

  bool IsStore() const {
    return (properties & (ISSTORE)) != 0;
  }

  bool IsLoadPair() const {
    return (properties & (ISLOADPAIR)) != 0;
  }

  bool IsStorePair() const {
    return (properties & (ISSTOREPAIR)) != 0;
  }

  bool IsLoadStorePair() const {
    return (properties & (ISLOADPAIR | ISSTOREPAIR)) != 0;
  }

  bool IsLoadAddress() const {
    return (properties & (ISLOADADDR)) != 0;
  }

  bool IsAtomic() const {
    return (properties & ISATOMIC) != 0;
  }

  bool IsCondDef() const {
    return properties & ISCONDDEF;
  }

  bool IsPartDef() const {
    return properties & ISPARTDEF;
  }

  LatencyType GetLatencyType() const {
    return latencyType;
  }
};

/*
 * We save callee-saved registers from lower stack area to upper stack area.
 * If possible, we store a pair of registers (int/int and fp/fp) in the stack.
 * The Stack Pointer has to be aligned at 16-byte boundary.
 * On AArch64, kIntregBytelen == 8 (see the above)
 */
inline void GetNextOffsetCalleeSaved(int &offset) {
  offset += (kIntregBytelen << 1);
}

MOperator GetMopPair(MOperator mop);
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_ISA_H */