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
#include "aarch64_operand.h"
#include <fstream>
#include <string>
#include "aarch64_abi.h"
#include "aarch64_cgfunc.h"
#include "aarch64_cg.h"

namespace maplebe {
AArch64RegOperand AArch64RegOperand::zero64(RZR, k64BitSize, kRegTyInt);
AArch64RegOperand AArch64RegOperand::zero32(RZR, k32BitSize, kRegTyInt);

const char *CondOperand::ccStrs[kCcLast] = {
#define CONDCODE(a) #a,
#include "aarch64_cc.def"
#undef CONDCODE
};

bool AArch64RegOperand::IsSaveReg(MIRType &type, BECommon &beCommon) const {
  ReturnMechanism retMechanism(type, beCommon);
  if (retMechanism.GetRegCount() > 0) {
    return GetRegisterNumber() == retMechanism.GetReg0() || GetRegisterNumber() == retMechanism.GetReg1();
  }
  return false;
}

bool AArch64RegOperand::IsSPOrFP() const {
  return (IsPhysicalRegister() && (regNO == RSP || regNO == RFP));
}

bool AArch64RegOperand::operator==(const AArch64RegOperand &o) const {
  regno_t myRn = GetRegisterNumber();
  uint32 mySz = GetSize();
  uint32 myFl = flag;
  regno_t otherRn = o.GetRegisterNumber();
  uint32 otherSz = o.GetSize();
  uint32 otherFl = o.flag;

  if (IsPhysicalRegister()) {
    return (myRn == otherRn && mySz == otherSz && myFl == otherFl);
  }
  return (myRn == otherRn && mySz == otherSz);
}

bool AArch64RegOperand::operator<(const AArch64RegOperand &o) const {
  regno_t myRn = GetRegisterNumber();
  uint32 mySz = GetSize();
  uint32 myFl = flag;
  regno_t otherRn = o.GetRegisterNumber();
  uint32 otherSz = o.GetSize();
  uint32 otherFl = o.flag;
  return myRn < otherRn || (myRn == otherRn && mySz < otherSz) ||
         (myRn == otherRn && mySz == otherSz && myFl < otherFl);
}

void AArch64RegOperand::Emit(Emitter &emitter, const OpndProp *opndProp) const {
  ASSERT((opndProp == nullptr || (static_cast<const AArch64OpndProp*>(opndProp)->IsRegister())),
         "operand type doesn't match");
  /* opndProp null means a sub emit, i.e from MemOperand */
  uint8 opndSize = (opndProp != nullptr) ? static_cast<const AArch64OpndProp*>(opndProp)->GetSize() : size;
  switch (GetRegisterType()) {
    case kRegTyInt: {
      ASSERT((opndSize == k32BitSize || opndSize == k64BitSize), "illegal register size");
#ifdef USE_32BIT_REF
      bool r32 = (opndSize == k32BitSize) || isRefField;
#else
      bool r32 = (opndSize == k32BitSize);
#endif  /* USE_32BIT_REF */
      emitter.Emit(AArch64CG::intRegNames[(r32 ? AArch64CG::kR32List : AArch64CG::kR64List)][regNO]);
      break;
    }
    case kRegTyFloat: {
      ASSERT((opndSize == k8BitSize || opndSize == k16BitSize || opndSize == k32BitSize || opndSize == k64BitSize),
             "illegal register size");
      /* FP reg cannot be reffield. 8~0, 16~1, 32~2, 64~3. 8 is 1000b, has 3 zero. */
      uint32 regSet = __builtin_ctz(opndSize) - 3;
      emitter.Emit(AArch64CG::intRegNames[regSet][regNO]);
      break;
    }
    default:
      ASSERT(false, "NYI");
      break;
  }
}

void AArch64ImmOperand::Emit(Emitter &emitter, const OpndProp *opndProp) const {
  if (!isFmov) {
    emitter.Emit((opndProp != nullptr && static_cast<const AArch64OpndProp*>(opndProp)->IsLoadLiteral()) ? "=" : "#")
           .Emit((size == k64BitSize) ? value : static_cast<int64>(static_cast<int32>(value)));
    return;
  }
  /*
   * compute float value
   * use top 4 bits expect MSB of value . then calculate its fourth power
   */
  int32 exp = (((static_cast<uint32>(value) & 0x70) >> 4) ^ 0x4) - 3;
  /* use the lower four bits of value in this expression */
  const float mantissa = 1.0 + (static_cast<float>(static_cast<uint64>(value) & 0xf) / 16.0);
  float result = std::pow(2, exp) * mantissa;

  std::stringstream ss;
  ss << std::setprecision(10) << result;
  std::string res;
  ss >> res;
  size_t dot = res.find('.');
  if (dot == std::string::npos) {
    res += ".0";
    dot = res.find('.');
    CHECK_FATAL(dot != std::string::npos, "cannot find in string");
  }
  res.erase(dot, 1);
  std::string integer(res, 0, 1);
  std::string fraction(res, 1);
  while (fraction.size() != 1 && fraction[fraction.size() - 1] == '0') {
    fraction.pop_back();
  }
  /* fetch the sign bit of this value */
  std::string sign = static_cast<uint64>(value) & 0x80 ? "-" : "";
  emitter.Emit(sign + integer + "." + fraction + "e+").Emit(dot - 1);
}

void AArch64OfstOperand::Emit(Emitter &emitter, const OpndProp *opndProp) const {
  if (IsImmOffset()) {
    emitter.Emit((opndProp != nullptr && static_cast<const AArch64OpndProp*>(opndProp)->IsLoadLiteral()) ? "=" : "#")
           .Emit((size == k64BitSize) ? GetValue() : static_cast<int64>(static_cast<int32>(GetValue())));
    return;
  }
  if (CGOptions::IsPIC() &&
      (symbol->GetStorageClass() == kScGlobal || symbol->GetStorageClass() == kScExtern)) {
    emitter.Emit(":got:" + symbol->GetName());
  } else {
    emitter.Emit(symbol->GetName());
  }
  if (GetValue() != 0) {
    emitter.Emit("+" + std::to_string(GetValue()));
  }
}

bool StImmOperand::Less(const Operand &right) const{
  if (&right == this) {
    return false;
  }

  /* For different type. */
  if (GetKind() != right.GetKind()) {
    return GetKind() < right.GetKind();
  }

  const StImmOperand *rightOpnd = static_cast<const StImmOperand*>(&right);
  if (symbol != rightOpnd->symbol) {
    return symbol < rightOpnd->symbol;
  }
  if (offset != rightOpnd->offset) {
    return offset < rightOpnd->offset;
  }
  return relocs < rightOpnd->relocs;
}

void StImmOperand::Emit(Emitter &emitter, const OpndProp *opndProp) const {
  CHECK_FATAL(opndProp != nullptr, "opndProp is nullptr in  StImmOperand::Emit");
  if (static_cast<const AArch64OpndProp*>(opndProp)->IsLiteralLow12()) {
    emitter.Emit("#:lo12:" + GetName());
    if (offset != 0) {
      emitter.Emit("+" + std::to_string(offset));
    }
    return;
  }
  if (CGOptions::IsPIC() && (symbol->GetStorageClass() == kScGlobal || symbol->GetStorageClass() == kScExtern)) {
    emitter.Emit(":got:" + GetName());
  } else {
    emitter.Emit(GetName());
  }
  if (offset != 0) {
    emitter.Emit("+" + std::to_string(offset));
  }
}

const int32 AArch64MemOperand::kMaxPimms[4] = { AArch64MemOperand::kMaxPimm8, AArch64MemOperand::kMaxPimm16,
                                                AArch64MemOperand::kMaxPimm32, AArch64MemOperand::kMaxPimm64 };

Operand *AArch64MemOperand::GetOffset() const {
  switch (addrMode) {
    case kAddrModeBOi:
      return GetOffsetOperand();
    case kAddrModeBOrX:
      return GetOffsetRegister();
    case kAddrModeLiteral:
      break;
    case kAddrModeLo12Li:
      break;
    default:
      ASSERT(false, "error memoperand dump");
      break;
  }
  return nullptr;
}

void AArch64MemOperand::Emit(Emitter &emitter, const OpndProp *opndProp) const {
  AArch64MemOperand::AArch64AddressingMode addressMode = GetAddrMode();
#if DEBUG
  const AArch64MD *md = &AArch64CG::kMd[emitter.GetCurrentMOP()];
  bool isLDSTpair = md->IsLoadStorePair();
  ASSERT(md->Is64Bit() || md->GetOperandSize() <= k32BitSize, "unexpected opnd size");
#endif
  if (addressMode == AArch64MemOperand::kAddrModeBOi) {
    emitter.Emit("[");
    auto *baseReg = static_cast<AArch64RegOperand*>(GetBaseRegister());
    ASSERT(baseReg != nullptr, "expect an AArch64RegOperand here");
    if (CGOptions::IsPIC() && (baseReg->GetSize() != k64BitSize)) {
      baseReg->SetSize(k64BitSize);
    }
    baseReg->Emit(emitter, nullptr);
    AArch64OfstOperand *offset = GetOffsetImmediate();
    if (offset != nullptr) {
#ifndef USE_32BIT_REF  /* can be load a ref here */
      ASSERT(!IsOffsetMisaligned(md->GetOperandSize()), "should not be OffsetMisaligned");
#endif  /* USE_32BIT_REF */
      if (IsPostIndexed()) {
        ASSERT(!IsSIMMOffsetOutOfRange(offset->GetOffsetValue(), md->Is64Bit(), isLDSTpair),
               "should not be SIMMOffsetOutOfRange");
        emitter.Emit("]");
        if (!offset->IsZero()) {
          emitter.Emit(", ");
          offset->Emit(emitter, nullptr);
        }
      } else if (IsPreIndexed()) {
        ASSERT(!IsSIMMOffsetOutOfRange(offset->GetOffsetValue(), md->Is64Bit(), isLDSTpair),
               "should not be SIMMOffsetOutOfRange");
        if (!offset->IsZero()) {
          emitter.Emit(",");
          offset->Emit(emitter, nullptr);
        }
        emitter.Emit("]!");
      } else {
        if (CGOptions::IsPIC() && (offset->IsSymOffset() || offset->IsSymAndImmOffset()) &&
            (offset->GetSymbol()->GetStorageClass() == kScGlobal ||
             offset->GetSymbol()->GetStorageClass() == kScExtern)) {
          emitter.Emit(",#:got_lo12:");
          emitter.Emit(offset->GetSymbolName());
        } else {
          ASSERT(!IsPIMMOffsetOutOfRange(offset->GetOffsetValue(), size), "should not be PIMMOffsetOutOfRange");
          if (!offset->IsZero()) {
            emitter.Emit(",");
            offset->Emit(emitter, nullptr);
          }
        }
        emitter.Emit("]");
      }
    } else {
      emitter.Emit("]");
    }
  } else if (addressMode == AArch64MemOperand::kAddrModeBOrX) {
    /*
     * Base plus offset   | [base{, #imm}]  [base, Xm{, LSL #imm}]   [base, Wm, (S|U)XTW {#imm}]
     *                      offset_opnds=nullptr
     *                                      offset_opnds=64          offset_opnds=32
     *                                      imm=0 or 3               imm=0 or 2, s/u
     */
    emitter.Emit("[");
    GetBaseRegister()->Emit(emitter, nullptr);
    emitter.Emit(",");
    GetOffsetRegister()->Emit(emitter, nullptr);
    if (ShouldEmitExtend()) {
      emitter.Emit(",");
      /* extend, #0, of #3/#2 */
      emitter.Emit(GetExtendAsString());
      if (GetExtendAsString() == "LSL" || ShiftAmount() != 0) {
        emitter.Emit(" #");
        emitter.Emit(ShiftAmount());
      }
    }
    emitter.Emit("]");
  } else if (addressMode == AArch64MemOperand::kAddrModeLiteral) {
    auto *prop = static_cast<const AArch64OpndProp*>(opndProp);
    CHECK_FATAL(prop != nullptr, "prop is nullptr in  AArch64MemOperand::Emit");
    if (prop->IsMemLow12()) {
      emitter.Emit("#:lo12:");
    }
    emitter.Emit(GetSymbol()->GetName());
  } else if (addressMode == AArch64MemOperand::kAddrModeLo12Li) {
    emitter.Emit("[");
    GetBaseRegister()->Emit(emitter, nullptr);

    AArch64OfstOperand *offset = GetOffsetImmediate();
    ASSERT(offset != nullptr, "nullptr check");

    emitter.Emit(", #:lo12:");
    emitter.Emit(GetSymbolName());
    if (!offset->IsZero()) {
      emitter.Emit("+");
      emitter.Emit(std::to_string(offset->GetOffsetValue()));
    }
    emitter.Emit("]");
  } else {
    ASSERT(false, "nyi");
  }
}

void AArch64MemOperand::Dump() const {
  LogInfo::MapleLogger() << "Mem:";
  switch (addrMode) {
    case kAddrModeBOi: {
      LogInfo::MapleLogger() << "base:";
      GetBaseRegister()->Dump();
      LogInfo::MapleLogger() << "offset:";
      GetOffsetOperand()->Dump();
      switch (idxOpt) {
        case kIntact:
          LogInfo::MapleLogger() << "  intact";
          break;
        case kPreIndex:
          LogInfo::MapleLogger() << "  pre-index";
          break;
        case kPostIndex:
          LogInfo::MapleLogger() << "  post-index";
          break;
        default:
          break;
      }
      break;
    }
    case kAddrModeBOrX: {
      LogInfo::MapleLogger() << "base:";
      GetBaseRegister()->Dump();
      LogInfo::MapleLogger() << "offset:";
      GetOffsetRegister()->Dump();
      LogInfo::MapleLogger() << " " << GetExtendAsString();
      LogInfo::MapleLogger() << " shift: " << ShiftAmount();
      break;
    }
    case kAddrModeLiteral:
      LogInfo::MapleLogger() << "literal: " << GetSymbolName();
      break;
    case kAddrModeLo12Li: {
      LogInfo::MapleLogger() << "base:";
      GetBaseRegister()->Dump();
      LogInfo::MapleLogger() << "offset:";
      AArch64OfstOperand *offOpnd = GetOffsetImmediate();
      LogInfo::MapleLogger() << "#:lo12:" << GetSymbolName() << "+" << std::to_string(offOpnd->GetOffsetValue());
      break;
    }
    default:
      ASSERT(false, "error memoperand dump");
      break;
  }
}

bool AArch64MemOperand::Equals(Operand &operand) const {
  if (!operand.IsMemoryAccessOperand()) {
    return false;
  }
  return Equals(static_cast<AArch64MemOperand&>(operand));
}

bool AArch64MemOperand::Equals(AArch64MemOperand &op) const {
  if (&op == this) {
    return true;
  }

  if (addrMode == op.GetAddrMode()) {
    switch (addrMode) {
      case kAddrModeBOi:
        return (GetBaseRegister()->Equals(*op.GetBaseRegister()) &&
                GetOffsetImmediate()->Equals(*op.GetOffsetImmediate()));
      case kAddrModeBOrX:
        return (GetBaseRegister()->Equals(*op.GetBaseRegister()) &&
                GetOffsetRegister()->Equals(*op.GetOffsetRegister()) &&
                GetExtendAsString() == op.GetExtendAsString() &&
                ShiftAmount() == op.ShiftAmount());
      case kAddrModeLiteral:
        return GetSymbolName() == op.GetSymbolName();
      case kAddrModeLo12Li:
        return (GetBaseRegister()->Equals(*op.GetBaseRegister()) &&
                GetSymbolName() == op.GetSymbolName() &&
                GetOffsetImmediate()->Equals(*op.GetOffsetImmediate()));
      default:
        ASSERT(false, "error memoperand");
        break;
    }
  }
  return false;
}

bool AArch64MemOperand::Less(const Operand &right) const {
  if (&right == this) {
    return false;
  }

  /* For different type. */
  if (GetKind() != right.GetKind()) {
    return GetKind() < right.GetKind();
  }

  const AArch64MemOperand *rightOpnd = static_cast<const AArch64MemOperand*>(&right);
  if (addrMode != rightOpnd->addrMode) {
    return addrMode < rightOpnd->addrMode;
  }

  switch (addrMode) {
    case kAddrModeBOi: {
      ASSERT(idxOpt == kIntact, "Should not compare pre/post index addressing.");

      RegOperand *baseReg = GetBaseRegister();
      RegOperand *rbaseReg = rightOpnd->GetBaseRegister();
      int32 nRet = baseReg->RegCompare(*rbaseReg);
      if (nRet == 0) {
        Operand *ofstOpnd = GetOffsetOperand();
        const Operand *rofstOpnd = rightOpnd->GetOffsetOperand();
        return ofstOpnd->Less(*rofstOpnd);
      }
      return nRet < 0;
    }
    case kAddrModeBOrX: {
      if (noExtend != rightOpnd->noExtend) {
        return noExtend;
      }
      if (!noExtend && extend != rightOpnd->extend) {
        return extend < rightOpnd->extend;
      }
      RegOperand *indexReg = GetIndexRegister();
      const RegOperand *rindexReg = rightOpnd->GetIndexRegister();
      return indexReg->Less(*rindexReg);
    }
    case kAddrModeLiteral: {
      return static_cast<const void*>(GetSymbol()) < static_cast<const void*>(rightOpnd->GetSymbol());
    }
    case kAddrModeLo12Li: {
      if (GetSymbol() != rightOpnd->GetSymbol()) {
        return static_cast<const void*>(GetSymbol()) < static_cast<const void*>(rightOpnd->GetSymbol());
      }
      Operand *ofstOpnd = GetOffsetOperand();
      const Operand *rofstOpnd = rightOpnd->GetOffsetOperand();
      return ofstOpnd->Less(*rofstOpnd);
    }
    default:
      ASSERT(false, "Internal error.");
      return false;
  }
}

bool AArch64MemOperand::NoAlias(AArch64MemOperand &rightOpnd) const {
  if (addrMode == kAddrModeBOi && rightOpnd.addrMode == kAddrModeBOi && idxOpt == kIntact &&
      rightOpnd.idxOpt == kIntact) {
    RegOperand *baseReg = GetBaseRegister();
    RegOperand *rbaseReg = rightOpnd.GetBaseRegister();

    if (baseReg->GetRegisterNumber() == RFP || rbaseReg->GetRegisterNumber() == RFP) {
      Operand *ofstOpnd = GetOffsetOperand();
      Operand *rofstOpnd = rightOpnd.GetOffsetOperand();

      ASSERT(ofstOpnd != nullptr, "offset operand should not be null.");
      ASSERT(rofstOpnd != nullptr, "offset operand should not be null.");
      OfstOperand *ofst = static_cast<OfstOperand*>(ofstOpnd);
      OfstOperand *rofst = static_cast<OfstOperand*>(rofstOpnd);
      ASSERT(ofst != nullptr, "CG internal error, invalid type.");
      ASSERT(rofst != nullptr, "CG internal error, invalid type.");

      return (!ofst->ValueEquals(*rofst));
    }
  }

  return false;
}

/* sort the register operand according to their number */
void AArch64ListOperand::Emit(Emitter &emitter, const OpndProp *opndProp) const {
  (void)opndProp;
  size_t nLeft = opndList.size();
  if (nLeft == 0) {
    return;
  }

  for (auto it = opndList.begin(); it != opndList.end(); ++it) {
    (*it)->Emit(emitter, nullptr);
    if (--nLeft >= 1) {
      emitter.Emit(", ");
    }
  }
}

bool CondOperand::Less(const Operand &right) const {
  if (&right == this) {
    return false;
  }

  /* For different type. */
  if (GetKind() != right.GetKind()) {
    return GetKind() < right.GetKind();
  }

  const CondOperand *rightOpnd = static_cast<const CondOperand*>(&right);

  /* The same type. */
  if (cc == CC_AL || rightOpnd->cc == CC_AL) {
    return false;
  }
  return cc < rightOpnd->cc;
}

bool ExtendShiftOperand::Less(const Operand &right) const {
  if (&right == this) {
    return false;
  }
  /* For different type. */
  if (GetKind() != right.GetKind()) {
    return GetKind() < right.GetKind();
  }

  const ExtendShiftOperand *rightOpnd = static_cast<const ExtendShiftOperand*>(&right);

  /* The same type. */
  if (extendOp != rightOpnd->extendOp) {
    return extendOp < rightOpnd->extendOp;
  }
  return shiftAmount < rightOpnd->shiftAmount;
}

bool BitShiftOperand::Less(const Operand &right) const {
  if (&right == this) {
    return false;
  }

  /* For different type. */
  if (GetKind() != right.GetKind()) {
    return GetKind() < right.GetKind();
  }

  const BitShiftOperand *rightOpnd = static_cast<const BitShiftOperand*>(&right);

  /* The same type. */
  if (shiftOp != rightOpnd->shiftOp) {
    return shiftOp < rightOpnd->shiftOp;
  }
  return shiftAmount < rightOpnd->shiftAmount;
}
}  /* namespace maplebe */
