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
#include "aarch64_peep.h"
#include "cg.h"
#include "mpl_logging.h"
#include "common_utils.h"

namespace maplebe {
#define JAVALANG (cgFunc.GetMirModule().IsJavaModule())
namespace {
const std::string kMccLoadRef = "MCC_LoadRefField";
const std::string kMccLoadRefV = "MCC_LoadVolatileField";
const std::string kMccLoadRefS = "MCC_LoadRefStatic";
const std::string kMccLoadRefVS = "MCC_LoadVolatileStaticField";
const std::string kMccDummy = "MCC_Dummy";

const std::string GetReadBarrierName(const Insn &insn) {
  constexpr int32 totalBarrierNamesNum = 5;
  std::array<std::string, totalBarrierNamesNum> barrierNames = {
    kMccLoadRef, kMccLoadRefV, kMccLoadRefS, kMccLoadRefVS, kMccDummy
  };
  if (insn.GetMachineOpcode() == MOP_xbl ||
      insn.GetMachineOpcode() == MOP_tail_call_opt_xbl) {
    auto &op = static_cast<FuncNameOperand&>(insn.GetOperand(kInsnFirstOpnd));
    const std::string &funcName = op.GetName();
    for (const std::string &singleBarrierName : barrierNames) {
      if (funcName == singleBarrierName) {
        return singleBarrierName;
      }
    }
  }
  return "";
}

MOperator GetLoadOperator(uint32 refSize, bool isVolatile) {
  if (refSize == k32BitSize) {
    return isVolatile ? MOP_wldar : MOP_wldr;
  }
  return isVolatile ? MOP_xldar : MOP_xldr;
}
}

void AArch64PeepHole::InitOpts() {
  optimizations.resize(kPeepholeOptsNum);
  optimizations[kRemoveIdenticalLoadAndStoreOpt] = optOwnMemPool->New<RemoveIdenticalLoadAndStoreAArch64>(cgFunc);
  optimizations[kRemoveMovingtoSameRegOpt] = optOwnMemPool->New<RemoveMovingtoSameRegAArch64>(cgFunc);;
  optimizations[kCombineContiLoadAndStoreOpt] = optOwnMemPool->New<CombineContiLoadAndStoreAArch64>(cgFunc);
  optimizations[kEliminateSpecifcSXTOpt] = optOwnMemPool->New<EliminateSpecifcSXTAArch64>(cgFunc);
  optimizations[kEliminateSpecifcUXTOpt] = optOwnMemPool->New<EliminateSpecifcUXTAArch64>(cgFunc);
  optimizations[kFmovRegOpt] = optOwnMemPool->New<FmovRegAArch64>(cgFunc);
  optimizations[kCbnzToCbzOpt] = optOwnMemPool->New<CbnzToCbzAArch64>(cgFunc);
  optimizations[kContiLDRorSTRToSameMEMOpt] = optOwnMemPool->New<ContiLDRorSTRToSameMEMAArch64>(cgFunc);
  optimizations[kRemoveIncDecRefOpt] = optOwnMemPool->New<RemoveIncDecRefAArch64>(cgFunc);
  optimizations[kInlineReadBarriersOpt] = optOwnMemPool->New<InlineReadBarriersAArch64>(cgFunc);
  optimizations[kReplaceDivToMultiOpt] = optOwnMemPool->New<ReplaceDivToMultiAArch64>(cgFunc);
  optimizations[kAndCmpBranchesToCsetOpt] = optOwnMemPool->New<AndCmpBranchesToCsetAArch64>(cgFunc);
  optimizations[kZeroCmpBranchesOpt] = optOwnMemPool->New<ZeroCmpBranchesAArch64>(cgFunc);
}

void AArch64PeepHole::Run(BB &bb, Insn &insn) {
  MOperator thisMop = insn.GetMachineOpcode();
  switch (thisMop) {
    case MOP_wmovrr:
    case MOP_xmovrr:
    case MOP_xvmovs:
    case MOP_xvmovd: {
      (static_cast<RemoveMovingtoSameRegAArch64*>(optimizations[kRemoveMovingtoSameRegOpt]))->Run(bb, insn);
      break;
    }
    case MOP_xldr:
    case MOP_xstr:
    case MOP_wldr:
    case MOP_wstr:
    case MOP_dldr:
    case MOP_dstr:
    case MOP_sldr:
    case MOP_sstr: {
      (static_cast<CombineContiLoadAndStoreAArch64*>(optimizations[kCombineContiLoadAndStoreOpt]))->Run(bb, insn);
      (static_cast<ContiLDRorSTRToSameMEMAArch64*>(optimizations[kContiLDRorSTRToSameMEMOpt]))->Run(bb, insn);
      (static_cast<RemoveIdenticalLoadAndStoreAArch64*>(optimizations[kRemoveIdenticalLoadAndStoreOpt]))->Run(bb, insn);
      break;
    }
    case MOP_xsxtb32:
    case MOP_xsxth32:
    case MOP_xsxtb64:
    case MOP_xsxth64:
    case MOP_xsxtw64: {
      (static_cast<EliminateSpecifcSXTAArch64*>(optimizations[kEliminateSpecifcSXTOpt]))->Run(bb, insn);
      break;
    }
    case MOP_xuxtb32:
    case MOP_xuxth32:
    case MOP_xuxtw64: {
      (static_cast<EliminateSpecifcUXTAArch64*>(optimizations[kEliminateSpecifcUXTOpt]))->Run(bb, insn);
      break;
    }
    case MOP_xvmovrv:
    case MOP_xvmovrd: {
      (static_cast<FmovRegAArch64*>(optimizations[kFmovRegOpt]))->Run(bb, insn);
      break;
    }
    case MOP_wcbnz:
    case MOP_xcbnz: {
      (static_cast<CbnzToCbzAArch64*>(optimizations[kCbnzToCbzOpt]))->Run(bb, insn);
      break;
    }
    case MOP_xbl: {
      (static_cast<RemoveIncDecRefAArch64*>(optimizations[kRemoveIncDecRefOpt]))->Run(bb, insn);
      break;
    }
    case MOP_wsdivrrr: {
      (static_cast<ReplaceDivToMultiAArch64*>(optimizations[kReplaceDivToMultiOpt]))->Run(bb, insn);
      break;
    }
    case MOP_wcsetrc:
    case MOP_xcsetrc: {
      (static_cast<AndCmpBranchesToCsetAArch64*>(optimizations[kAndCmpBranchesToCsetOpt]))->Run(bb, insn);
      break;
    }
    default:
      break;
  }
  if (GetReadBarrierName(insn) != "") { /* skip if it is not a read barrier call. */
    (static_cast<InlineReadBarriersAArch64*>(optimizations[kInlineReadBarriersOpt]))->Run(bb, insn);
  }
  if (&insn == bb.GetLastInsn()) {
    (static_cast<ZeroCmpBranchesAArch64*>(optimizations[kZeroCmpBranchesOpt]))->Run(bb, insn);
  }
}

void AArch64PeepHole0::InitOpts() {
  optimizations.resize(kPeepholeOptsNum);
  optimizations[kRemoveIdenticalLoadAndStoreOpt] = optOwnMemPool->New<RemoveIdenticalLoadAndStoreAArch64>(cgFunc);
  optimizations[kCmpCsetOpt] = optOwnMemPool->New<CmpCsetAArch64>(cgFunc);
  optimizations[kComplexMemOperandOptAdd] = optOwnMemPool->New<ComplexMemOperandAddAArch64>(cgFunc);
  optimizations[kDeleteMovAfterCbzOrCbnzOpt] = optOwnMemPool->New<DeleteMovAfterCbzOrCbnzAArch64>(cgFunc);
}

void AArch64PeepHole0::Run(BB &bb, Insn &insn) {
  MOperator thisMop = insn.GetMachineOpcode();
  switch (thisMop) {
    case MOP_xstr:
    case MOP_wstr: {
      (static_cast<RemoveIdenticalLoadAndStoreAArch64*>(optimizations[kRemoveIdenticalLoadAndStoreOpt]))->Run(bb, insn);
      break;
    }
    case MOP_wcmpri:
    case MOP_xcmpri: {
      (static_cast<CmpCsetAArch64*>(optimizations[kCmpCsetOpt]))->Run(bb, insn);
      break;
    }
    case MOP_xaddrrr: {
      (static_cast<ComplexMemOperandAddAArch64*>(optimizations[kComplexMemOperandOptAdd]))->Run(bb, insn);
      break;
    }
    case MOP_wcbz:
    case MOP_xcbz:
    case MOP_wcbnz:
    case MOP_xcbnz: {
      (static_cast<DeleteMovAfterCbzOrCbnzAArch64*>(optimizations[kDeleteMovAfterCbzOrCbnzOpt]))->Run(bb, insn);
      break;
    }
    default:
      break;
  }
}

void AArch64PrePeepHole::InitOpts() {
  optimizations.resize(kPeepholeOptsNum);
  optimizations[kOneHoleBranchesPreOpt] = optOwnMemPool->New<OneHoleBranchesPreAArch64>(cgFunc);
  optimizations[kLoadFloatPointOpt] = optOwnMemPool->New<LoadFloatPointAArch64>(cgFunc);
  optimizations[kReplaceOrrToMovOpt] = optOwnMemPool->New<ReplaceOrrToMovAArch64>(cgFunc);
  optimizations[kReplaceCmpToCmnOpt] = optOwnMemPool->New<ReplaceCmpToCmnAArch64>(cgFunc);
  optimizations[kRemoveIncRefOpt] = optOwnMemPool->New<RemoveIncRefAArch64>(cgFunc);
  optimizations[kLongIntCompareWithZOpt] = optOwnMemPool->New<LongIntCompareWithZAArch64>(cgFunc);
  optimizations[kComplexMemOperandPreOptAdd] = optOwnMemPool->New<ComplexMemOperandPreAddAArch64>(cgFunc);
  optimizations[kComplexMemOperandOptLSL] = optOwnMemPool->New<ComplexMemOperandLSLAArch64>(cgFunc);
  optimizations[kComplexMemOperandOptLabel] = optOwnMemPool->New<ComplexMemOperandLabelAArch64>(cgFunc);
  optimizations[kWriteFieldCallOpt] = optOwnMemPool->New<WriteFieldCallAArch64>(cgFunc);
}

void AArch64PrePeepHole::Run(BB &bb, Insn &insn) {
  MOperator thisMop = insn.GetMachineOpcode();
  switch (thisMop) {
    case MOP_xmovzri16: {
      (static_cast<LoadFloatPointAArch64*>(optimizations[kLoadFloatPointOpt]))->Run(bb, insn);
      break;
    }
    case MOP_wiorri12r:
    case MOP_wiorrri12:
    case MOP_xiorri13r:
    case MOP_xiorrri13: {
      (static_cast<ReplaceOrrToMovAArch64*>(optimizations[kReplaceOrrToMovOpt]))->Run(bb, insn);
      break;
    }
    case MOP_xmovri32:
    case MOP_xmovri64: {
      (static_cast<ReplaceCmpToCmnAArch64*>(optimizations[kReplaceCmpToCmnOpt]))->Run(bb, insn);
      break;
    }
    case MOP_xbl: {
      (static_cast<RemoveIncRefAArch64*>(optimizations[kRemoveIncRefOpt]))->Run(bb, insn);
      if (CGOptions::IsGCOnly() && CGOptions::DoWriteRefFieldOpt()) {
        (static_cast<WriteFieldCallAArch64*>(optimizations[kWriteFieldCallOpt]))->Run(bb, insn);
      }
      break;
    }
    case MOP_xcmpri: {
      (static_cast<LongIntCompareWithZAArch64*>(optimizations[kLongIntCompareWithZOpt]))->Run(bb, insn);
      break;
    }
    case MOP_xaddrrr: {
      (static_cast<ComplexMemOperandPreAddAArch64*>(optimizations[kComplexMemOperandPreOptAdd]))->Run(bb, insn);
      break;
    }
    case MOP_xaddrrrs: {
      (static_cast<ComplexMemOperandLSLAArch64*>(optimizations[kComplexMemOperandOptLSL]))->Run(bb, insn);
      break;
    }
    case MOP_xldli: {
      (static_cast<ComplexMemOperandLabelAArch64*>(optimizations[kComplexMemOperandOptLabel]))->Run(bb, insn);
      break;
    }
    default:
      break;
  }
  if (&insn == bb.GetLastInsn()) {
    (static_cast<OneHoleBranchesPreAArch64*>(optimizations[kOneHoleBranchesPreOpt]))->Run(bb, insn);
    if (CGOptions::IsGCOnly() && CGOptions::DoWriteRefFieldOpt()) {
      (static_cast<WriteFieldCallAArch64*>(optimizations[kWriteFieldCallOpt]))->Reset();
    }
  }
}

void AArch64PrePeepHole1::InitOpts() {
  optimizations.resize(kPeepholeOptsNum);
  optimizations[kRemoveDecRefOpt] = optOwnMemPool->New<RemoveDecRefAArch64>(cgFunc);
  optimizations[kComputationTreeOpt] = optOwnMemPool->New<ComputationTreeAArch64>(cgFunc);
  optimizations[kOneHoleBranchesOpt] = optOwnMemPool->New<OneHoleBranchesAArch64>(cgFunc);
  optimizations[kReplaceIncDecWithIncOpt] = optOwnMemPool->New<ReplaceIncDecWithIncAArch64>(cgFunc);
  optimizations[kAndCmpBranchesToTbzOpt] = optOwnMemPool->New<AndCmpBranchesToTbzAArch64>(cgFunc);
}

void AArch64PrePeepHole1::Run(BB &bb, Insn &insn) {
  MOperator thisMop = insn.GetMachineOpcode();
  switch (thisMop) {
    case MOP_xbl: {
      if (JAVALANG) {
        (static_cast<RemoveDecRefAArch64*>(optimizations[kRemoveDecRefOpt]))->Run(bb, insn);
        (static_cast<ReplaceIncDecWithIncAArch64*>(optimizations[kReplaceIncDecWithIncOpt]))->Run(bb, insn);
      }
      break;
    }
    case MOP_xaddrri12: {
      (static_cast<ComputationTreeAArch64*>(optimizations[kComputationTreeOpt]))->Run(bb, insn);
      break;
    }
    default:
      break;
  }
  if (&insn == bb.GetLastInsn()) {
    switch (thisMop) {
      case MOP_wcbz:
      case MOP_wcbnz:
      case MOP_xcbz:
      case MOP_xcbnz: {
        (static_cast<OneHoleBranchesAArch64*>(optimizations[kOneHoleBranchesOpt]))->Run(bb, insn);
        break;
      }
      case MOP_beq:
      case MOP_bne: {
        (static_cast<AndCmpBranchesToTbzAArch64*>(optimizations[kAndCmpBranchesToTbzOpt]))->Run(bb, insn);
        break;
      }
      default:
        break;
    }
  }
}

void RemoveIdenticalLoadAndStoreAArch64::Run(BB &bb, Insn &insn) {
  Insn *nextInsn = insn.GetNext();
  if (nextInsn == nullptr) {
    return;
  }
  MOperator mop1 = insn.GetMachineOpcode();
  MOperator mop2 = nextInsn->GetMachineOpcode();
  if ((mop1 == MOP_wstr && mop2 == MOP_wstr) || (mop1 == MOP_xstr && mop2 == MOP_xstr)) {
    if (IsMemOperandsIdentical(insn, *nextInsn)) {
      bb.RemoveInsn(insn);
      insn = *nextInsn;
    }
  } else if ((mop1 == MOP_wstr && mop2 == MOP_wldr) || (mop1 == MOP_xstr && mop2 == MOP_xldr)) {
    if (IsMemOperandsIdentical(insn, *nextInsn)) {
      bb.RemoveInsn(*nextInsn);
    }
  }
}

bool RemoveIdenticalLoadAndStoreAArch64::IsMemOperandsIdentical(const Insn &insn1, const Insn &insn2) const {
  regno_t regNO1 = static_cast<RegOperand&>(insn1.GetOperand(kInsnFirstOpnd)).GetRegisterNumber();
  regno_t regNO2 = static_cast<RegOperand&>(insn2.GetOperand(kInsnFirstOpnd)).GetRegisterNumber();
  if (regNO1 != regNO2) {
    return false;
  }
  /* Match only [base + offset] */
  auto &memOpnd1 = static_cast<MemOperand&>(insn1.GetOperand(kInsnSecondOpnd));
  if (static_cast<AArch64MemOperand&>(memOpnd1).GetAddrMode() != AArch64MemOperand::kAddrModeBOi) {
    return false;
  }
  auto &memOpnd2 = static_cast<MemOperand&>(insn2.GetOperand(kInsnSecondOpnd));
  if (static_cast<AArch64MemOperand&>(memOpnd2).GetAddrMode() != AArch64MemOperand::kAddrModeBOi) {
    return false;
  }
  Operand *base1 = memOpnd1.GetBaseRegister();
  Operand *base2 = memOpnd2.GetBaseRegister();
  if (!((base1 != nullptr) && base1->IsRegister()) || !((base2 != nullptr) && base2->IsRegister())) {
    return false;
  }

  regno_t baseRegNO1 = static_cast<RegOperand*>(base1)->GetRegisterNumber();
  /* First insn re-write base addr   reg1 <- [ reg1 + offset ] */
  if (baseRegNO1 == regNO1) {
    return false;
  }

  regno_t baseRegNO2 = static_cast<RegOperand*>(base2)->GetRegisterNumber();
  if (baseRegNO1 != baseRegNO2) {
    return false;
  }

  if (static_cast<AArch64MemOperand&>(memOpnd1).GetOffsetImmediate()->GetOffsetValue() !=
      static_cast<AArch64MemOperand&>(memOpnd2).GetOffsetImmediate()->GetOffsetValue()) {
    return false;
  }
  return true;
}

void RemoveMovingtoSameRegAArch64::Run(BB &bb, Insn &insn) {
  ASSERT(insn.GetOperand(kInsnFirstOpnd).IsRegister(), "expects registers");
  ASSERT(insn.GetOperand(kInsnSecondOpnd).IsRegister(), "expects registers");
  auto &reg1 = static_cast<AArch64RegOperand&>(insn.GetOperand(kInsnFirstOpnd));
  auto &reg2 = static_cast<AArch64RegOperand&>(insn.GetOperand(kInsnSecondOpnd));

  if ((reg1.GetRegisterNumber() == reg2.GetRegisterNumber()) && (reg1.GetSize() == reg2.GetSize())) {
    bb.RemoveInsn(insn);
  }
}

/* Combining 2 STRs into 1 stp or 2 LDRs into 1 ldp */
void CombineContiLoadAndStoreAArch64::Run(BB &bb, Insn &insn) {
  MOperator thisMop = insn.GetMachineOpcode();
  Insn *nextInsn = insn.GetNext();
  if (nextInsn == nullptr || nextInsn->GetMachineOpcode() != thisMop) {
    return;
  }
  ASSERT(insn.GetOperand(kInsnSecondOpnd).IsMemoryAccessOperand(), "expects mem operands");
  ASSERT(nextInsn->GetOperand(kInsnSecondOpnd).IsMemoryAccessOperand(), "expects mem operands");
  auto &memOpnd1 = static_cast<AArch64MemOperand&>(insn.GetOperand(kInsnSecondOpnd));

  AArch64MemOperand::AArch64AddressingMode addrMode1 = memOpnd1.GetAddrMode();
  if (addrMode1 != AArch64MemOperand::kAddrModeBOi || (!memOpnd1.IsIntactIndexed())) {
    return;
  }

  auto *base1 = static_cast<AArch64RegOperand*>(memOpnd1.GetBaseRegister());
  ASSERT(base1 == nullptr || !base1->IsVirtualRegister(), "physical register has not been allocated?");
  AArch64OfstOperand *offset1 = memOpnd1.GetOffsetImmediate();

  auto &memOpnd2 = static_cast<AArch64MemOperand&>(nextInsn->GetOperand(kInsnSecondOpnd));

  AArch64MemOperand::AArch64AddressingMode addrMode2 = memOpnd2.GetAddrMode();
  if (addrMode2 != AArch64MemOperand::kAddrModeBOi || (!memOpnd2.IsIntactIndexed())) {
    return;
  }

  auto *base2 = static_cast<AArch64RegOperand*>(memOpnd2.GetBaseRegister());
  ASSERT(base2 == nullptr || !base2->IsVirtualRegister(), "physical register has not been allocated?");
  AArch64OfstOperand *offset2 = memOpnd2.GetOffsetImmediate();

  if (base1 == nullptr || base2 == nullptr || offset1 == nullptr || offset2 == nullptr) {
    return;
  }

  /*
   * In ARM Architecture Reference Manual ARMv8, for ARMv8-A architecture profile
   * LDP on page K1-6125 delcare that ldp can't use same reg
   */
  auto &reg1 = static_cast<AArch64RegOperand&>(insn.GetOperand(kInsnFirstOpnd));
  auto &reg2 = static_cast<AArch64RegOperand&>(nextInsn->GetOperand(kInsnFirstOpnd));
  if ((thisMop == MOP_xldr || thisMop == MOP_sldr || thisMop == MOP_dldr || thisMop == MOP_wldr) &&
    reg1.GetRegisterNumber() == reg2.GetRegisterNumber()) {
    return;
  }

  if (reg1.GetSize() != memOpnd1.GetSize() || reg2.GetSize() != memOpnd2.GetSize()) {
    return;
  }

  uint32 size = reg1.GetSize() >> kLog2BitsPerByte;
  int offsetVal1 = offset1->GetOffsetValue();
  int offsetVal2 = offset2->GetOffsetValue();
  if ((base1->GetRegisterNumber() == RFP || base1->GetRegisterNumber() == RSP) &&
      base1->GetRegisterNumber() == base2->GetRegisterNumber() &&
      reg1.GetRegisterType() == reg2.GetRegisterType() && reg1.GetSize() == reg2.GetSize() &&
      abs(offsetVal1 - offsetVal2) == static_cast<int>(size)) {
    /* pair instr for 8/4 byte registers must have multiple of 8/4 for imm */
    if ((static_cast<uint32>(offsetVal1) % size) != 0) {
      return;
    }
    /* For stp/ldp, the imm should be within -512 and 504. */
    if (size == kIntregBytelen) {
      if (offsetVal1 <= kStpLdpImm64LowerBound || offsetVal1 >= kStpLdpImm64UpperBound) {
        return;
      }
    }
    if (size == (kIntregBytelen >> 1)) {
      if (offsetVal1 <= kStpLdpImm32LowerBound || offsetVal1 >= kStpLdpImm32UpperBound) {
        return;
      }
    }

    MOperator mopPair = GetMopPair(thisMop);
    CG *cg = cgFunc.GetCG();
    if (offsetVal1 < offsetVal2) {
      bb.InsertInsnAfter(*nextInsn, cg->BuildInstruction<AArch64Insn>(mopPair, reg1, reg2, memOpnd1));
    } else {
      bb.InsertInsnAfter(*nextInsn, cg->BuildInstruction<AArch64Insn>(mopPair, reg2, reg1, memOpnd2));
    }

    /* keep the comment */
    Insn *nn = nextInsn->GetNext();
    std::string newComment = "";
    MapleString comment = insn.GetComment();
    if (comment.c_str() != nullptr && strlen(comment.c_str()) > 0) {
      newComment += comment.c_str();
    }
    comment = nextInsn->GetComment();
    if (newComment.c_str() != nullptr && strlen(newComment.c_str()) > 0) {
      newComment += "  ";
    }
    if (comment.c_str() != nullptr && strlen(comment.c_str()) > 0) {
      newComment += comment.c_str();
    }
    if (newComment.c_str() != nullptr && strlen(newComment.c_str()) > 0) {
      nn->SetComment(newComment);
    }
    bb.RemoveInsn(insn);
    bb.RemoveInsn(*nextInsn);
    insn = *nn;
  }  /* pattern found */
}

void EliminateSpecifcSXTAArch64::Run(BB &bb, Insn &insn) {
  MOperator thisMop = insn.GetMachineOpcode();
  Insn *prevInsn = insn.GetPrev();
  while (prevInsn != nullptr && !prevInsn->GetMachineOpcode()) {
    prevInsn = prevInsn->GetPrev();
  }
  if (prevInsn == nullptr) {
    return;
  }
  auto &regOpnd0 = static_cast<RegOperand&>(insn.GetOperand(kInsnFirstOpnd));
  auto &regOpnd1 = static_cast<RegOperand&>(insn.GetOperand(kInsnSecondOpnd));
  if (&insn != bb.GetFirstInsn() && regOpnd0.GetRegisterNumber() == regOpnd1.GetRegisterNumber() &&
      prevInsn->IsMachineInstruction()) {
    if (prevInsn->GetMachineOpcode() == MOP_xmovri32 || prevInsn->GetMachineOpcode() == MOP_xmovri64) {
      auto &dstMovOpnd = static_cast<RegOperand&>(prevInsn->GetOperand(kInsnFirstOpnd));
      if (dstMovOpnd.GetRegisterNumber() != regOpnd1.GetRegisterNumber()) {
        return;
      }
      Operand &opnd = prevInsn->GetOperand(kInsnSecondOpnd);
      if (opnd.IsIntImmediate()) {
        auto &immOpnd = static_cast<AArch64ImmOperand&>(opnd);
        int64 value = immOpnd.GetValue();
        if (thisMop == MOP_xsxtb32) {
          /* value should in range between -127 and 127 */
          if (value >= static_cast<int64>(0xFFFFFFFFFFFFFF80) && value <= 0x7F &&
              immOpnd.IsSingleInstructionMovable(regOpnd0.GetSize())) {
            bb.RemoveInsn(insn);
          }
        } else if (thisMop == MOP_xsxth32) {
          /* value should in range between -32678 and 32678 */
          if (value >= static_cast<int64>(0xFFFFFFFFFFFF8000) && value <= 0x7FFF &&
              immOpnd.IsSingleInstructionMovable(regOpnd0.GetSize())) {
            bb.RemoveInsn(insn);
          }
        } else {
          uint64 flag = 0xFFFFFFFFFFFFFF80; /* initialize the flag with fifty-nine 1s at top */
          if (thisMop == MOP_xsxth64) {
            flag = 0xFFFFFFFFFFFF8000;      /* specify the flag with forty-nine 1s at top in this case */
          } else if (thisMop == MOP_xsxtw64) {
            flag = 0xFFFFFFFF80000000;      /* specify the flag with thirty-three 1s at top in this case */
          }
          if (!(static_cast<uint64>(value) & flag) && immOpnd.IsSingleInstructionMovable(regOpnd0.GetSize())) {
            auto *aarch64CGFunc = static_cast<AArch64CGFunc*>(&cgFunc);
            RegOperand &dstOpnd = aarch64CGFunc->GetOrCreatePhysicalRegisterOperand(
                static_cast<AArch64reg>(dstMovOpnd.GetRegisterNumber()), k64BitSize, dstMovOpnd.GetRegisterType());
            prevInsn->SetOperand(kInsnFirstOpnd, dstOpnd);
            prevInsn->SetMOperator(MOP_xmovri64);
            bb.RemoveInsn(insn);
          }
        }
      }
    } else if (prevInsn->GetMachineOpcode() == MOP_wldrsb) {
      auto &dstMovOpnd = static_cast<RegOperand&>(prevInsn->GetOperand(kInsnFirstOpnd));
      if (dstMovOpnd.GetRegisterNumber() != regOpnd1.GetRegisterNumber()) {
        return;
      }
      if (thisMop == MOP_xsxtb32) {
        bb.RemoveInsn(insn);
      }
    } else if (prevInsn->GetMachineOpcode() == MOP_wldrsh) {
      auto &dstMovOpnd = static_cast<RegOperand&>(prevInsn->GetOperand(kInsnFirstOpnd));
      if (dstMovOpnd.GetRegisterNumber() != regOpnd1.GetRegisterNumber()) {
        return;
      }
      if (thisMop == MOP_xsxth32) {
        bb.RemoveInsn(insn);
      }
    }
  }
}

void EliminateSpecifcUXTAArch64::Run(BB &bb, Insn &insn) {
  MOperator thisMop = insn.GetMachineOpcode();
  Insn *prevInsn = insn.GetPreviousMachineInsn();
  if (prevInsn == nullptr) {
    return;
  }
  auto &regOpnd0 = static_cast<RegOperand&>(insn.GetOperand(kInsnFirstOpnd));
  auto &regOpnd1 = static_cast<RegOperand&>(insn.GetOperand(kInsnSecondOpnd));
  if (prevInsn->IsCall() &&
      regOpnd0.GetRegisterNumber() == regOpnd1.GetRegisterNumber() &&
      (regOpnd1.GetRegisterNumber() == R0 || regOpnd1.GetRegisterNumber() == V0)) {
    uint32 retSize = prevInsn->GetRetSize();
    if (retSize > 0 &&
        ((thisMop == MOP_xuxtb32 && retSize <= k1ByteSize) ||
         (thisMop == MOP_xuxth32 && retSize <= k2ByteSize) ||
         (thisMop == MOP_xuxtw64 && retSize <= k4ByteSize))) {
      bb.RemoveInsn(insn);
    }
    return;
  }
  if (&insn == bb.GetFirstInsn() || regOpnd0.GetRegisterNumber() != regOpnd1.GetRegisterNumber() ||
      !prevInsn->IsMachineInstruction()) {
    return;
  }
  if (thisMop == MOP_xuxtb32) {
    if (prevInsn->GetMachineOpcode() == MOP_xmovri32 || prevInsn->GetMachineOpcode() == MOP_xmovri64) {
      auto &dstMovOpnd = static_cast<RegOperand&>(prevInsn->GetOperand(kInsnFirstOpnd));
      if (dstMovOpnd.GetRegisterNumber() != regOpnd1.GetRegisterNumber()) {
        return;
      }
      Operand &opnd = prevInsn->GetOperand(kInsnSecondOpnd);
      if (opnd.IsIntImmediate()) {
        auto &immOpnd = static_cast<ImmOperand&>(opnd);
        int64 value = immOpnd.GetValue();
        /* check the top 56 bits of value */
        if (!(static_cast<uint64>(value) & 0xFFFFFFFFFFFFFF00)) {
          bb.RemoveInsn(insn);
        }
      }
    } else if (prevInsn->GetMachineOpcode() == MOP_wldrb) {
      auto &dstOpnd = static_cast<RegOperand&>(prevInsn->GetOperand(kInsnFirstOpnd));
      if (dstOpnd.GetRegisterNumber() != regOpnd1.GetRegisterNumber()) {
        return;
      }
      bb.RemoveInsn(insn);
    }
  } else if (thisMop == MOP_xuxth32) {
    if (prevInsn->GetMachineOpcode() == MOP_xmovri32 || prevInsn->GetMachineOpcode() == MOP_xmovri64) {
      Operand &opnd = prevInsn->GetOperand(kInsnSecondOpnd);
      if (opnd.IsIntImmediate()) {
        auto &immOpnd = static_cast<ImmOperand&>(opnd);
        int64 value = immOpnd.GetValue();
        if (!(static_cast<uint64>(value) & 0xFFFFFFFFFFFF0000)) {
          bb.RemoveInsn(insn);
        }
      }
    } else if (prevInsn->GetMachineOpcode() == MOP_wldrh) {
      auto &dstOpnd = static_cast<RegOperand&>(prevInsn->GetOperand(kInsnFirstOpnd));
      if (dstOpnd.GetRegisterNumber() != regOpnd1.GetRegisterNumber()) {
        return;
      }
      bb.RemoveInsn(insn);
    }
  } else {
    /* this_mop == MOP_xuxtw64 */
    if (prevInsn->GetMachineOpcode() == MOP_xmovri32 || prevInsn->GetMachineOpcode() == MOP_wldrsb ||
        prevInsn->GetMachineOpcode() == MOP_wldrb || prevInsn->GetMachineOpcode() == MOP_wldrsh ||
        prevInsn->GetMachineOpcode() == MOP_wldrh || prevInsn->GetMachineOpcode() == MOP_wldr) {
      auto &dstOpnd = static_cast<RegOperand&>(prevInsn->GetOperand(kInsnFirstOpnd));
      if (dstOpnd.GetRegisterNumber() != regOpnd1.GetRegisterNumber()) {
        return;
      }
      /* 32-bit ldr does zero-extension by default, so this conversion can be skipped */
      bb.RemoveInsn(insn);
    }
  }
}

void FmovRegAArch64::Run(BB &bb, Insn &insn) {
  MOperator thisMop = insn.GetMachineOpcode();
  Insn *nextInsn = insn.GetNext();
  if (&insn == bb.GetFirstInsn()) {
    return;
  }
  Insn *prevInsn = insn.GetPrev();
  MOperator prevMop = prevInsn->GetMachineOpcode();
  MOperator newMop;
  uint32 doOpt = 0;
  if (prevMop == MOP_xvmovrv && thisMop == MOP_xvmovrv) {
    doOpt = k32BitSize;
    newMop = MOP_wmovrr;
  } else if (prevMop == MOP_xvmovrd && thisMop == MOP_xvmovrd) {
    doOpt = k64BitSize;
    newMop = MOP_xmovrr;
  }
  if (doOpt == 0) {
    return;
  }
  auto &curSrcRegOpnd = static_cast<RegOperand&>(insn.GetOperand(kInsnSecondOpnd));
  auto &prevSrcRegOpnd = static_cast<RegOperand&>(prevInsn->GetOperand(kInsnSecondOpnd));
  /* same src freg */
  if (curSrcRegOpnd.GetRegisterNumber() != prevSrcRegOpnd.GetRegisterNumber()) {
    return;
  }
  auto &curDstRegOpnd = static_cast<RegOperand&>(insn.GetOperand(kInsnFirstOpnd));
  regno_t curDstReg = curDstRegOpnd.GetRegisterNumber();
  CG *cg = cgFunc.GetCG();
  /* optimize case 1 */
  auto &prevDstRegOpnd = static_cast<RegOperand&>(prevInsn->GetOperand(kInsnFirstOpnd));
  regno_t prevDstReg = prevDstRegOpnd.GetRegisterNumber();
  auto *aarch64CGFunc = static_cast<AArch64CGFunc*>(&cgFunc);
  RegOperand &dst =
      aarch64CGFunc->GetOrCreatePhysicalRegisterOperand(static_cast<AArch64reg>(curDstReg), doOpt, kRegTyInt);
  RegOperand &src =
      aarch64CGFunc->GetOrCreatePhysicalRegisterOperand(static_cast<AArch64reg>(prevDstReg), doOpt, kRegTyInt);
  Insn &newInsn = cg->BuildInstruction<AArch64Insn>(newMop, dst, src);
  bb.InsertInsnBefore(insn, newInsn);
  bb.RemoveInsn(insn);
  if (nextInsn == nullptr) {
    return;
  }
  RegOperand &newOpnd =
      aarch64CGFunc->GetOrCreatePhysicalRegisterOperand(static_cast<AArch64reg>(prevDstReg), doOpt, kRegTyInt);
  uint32 opndNum = nextInsn->GetOperandSize();
  for (uint32 opndIdx = 0; opndIdx < opndNum; ++opndIdx) {
    Operand &opnd = nextInsn->GetOperand(opndIdx);
    if (opnd.IsMemoryAccessOperand()) {
      auto &memOpnd = static_cast<MemOperand&>(opnd);
      Operand *base = memOpnd.GetBaseRegister();
      if (base != nullptr) {
        if (base->IsRegister()) {
          auto *reg = static_cast<RegOperand*>(base);
          if (reg->GetRegisterNumber() == curDstReg) {
            memOpnd.SetBaseRegister(newOpnd);
          }
        }
      }
      Operand *offset = memOpnd.GetIndexRegister();
      if (offset != nullptr) {
        if (offset->IsRegister()) {
          auto *reg = static_cast<RegOperand*>(offset);
          if (reg->GetRegisterNumber() == curDstReg) {
            memOpnd.SetIndexRegister(newOpnd);
          }
        }
      }
    } else if (opnd.IsRegister()) {
      /* Check if it is a source operand. */
      const AArch64MD *md = &AArch64CG::kMd[static_cast<AArch64Insn*>(nextInsn)->GetMachineOpcode()];
      auto *regProp = static_cast<AArch64OpndProp*>(md->operand[opndIdx]);
      if (regProp->IsUse()) {
        auto &reg = static_cast<RegOperand&>(opnd);
        if (reg.GetRegisterNumber() == curDstReg) {
          nextInsn->SetOperand(opndIdx, newOpnd);
        }
      }
    }
  }
}

void CbnzToCbzAArch64::Run(BB &bb, Insn &insn) {
  MOperator thisMop = insn.GetMachineOpcode();
  /* reg has to be R0, since return value is in R0 */
  auto &regOpnd0 = static_cast<RegOperand&>(insn.GetOperand(kInsnFirstOpnd));
  if (regOpnd0.GetRegisterNumber() != R0) {
    return;
  }
  BB *nextBB = bb.GetNext();
  /* Make sure nextBB can only be reached by bb */
  if (nextBB->GetPreds().size() > 1 || nextBB->GetEhPreds().empty()) {
    return;
  }
  BB *targetBB = nullptr;
  auto it = bb.GetSuccsBegin();
  if (*it == nextBB) {
    ++it;
  }
  targetBB = *it;
  /* Make sure when nextBB is empty, targetBB is fallthru of bb. */
  if (targetBB != nextBB->GetNext()) {
    return;
  }
  /* Is nextBB branch to the return-bb? */
  if (nextBB->GetSuccs().size() != 1) {
    return;
  }
  BB *nextBBTarget = *(nextBB->GetSuccsBegin());
  if (nextBBTarget->GetKind() != BB::kBBReturn) {
    return;
  }
  /* Next insn should be a mov R0 = 0 */
  Insn *movInsn = nextBB->GetFirstMachineInsn();
  if (movInsn == nullptr) {
    return;
  }
  MOperator movInsnMop = movInsn->GetMachineOpcode();
  if (movInsnMop != MOP_xmovri32 && movInsnMop != MOP_xmovri64) {
    return;
  }
  auto &movDest = static_cast<RegOperand&>(movInsn->GetOperand(kInsnFirstOpnd));
  if (movDest.GetRegisterNumber() != R0) {
    return;
  }
  auto &movImm = static_cast<ImmOperand&>(movInsn->GetOperand(kInsnSecondOpnd));
  if (movImm.GetValue() != 0) {
    return;
  }
  Insn *brInsn = movInsn->GetNextMachineInsn();
  if (brInsn == nullptr) {
    return;
  }
  if (brInsn->GetMachineOpcode() != MOP_xuncond) {
    return;
  }
  /* Control flow looks nice, instruction looks nice */
  Operand &brTarget = brInsn->GetOperand(kInsnFirstOpnd);
  insn.SetOperand(kInsnSecondOpnd, brTarget);
  if (thisMop == MOP_wcbnz) {
    insn.SetMOP(MOP_wcbz);
  } else {
    insn.SetMOP(MOP_xcbz);
  }
  nextBB->RemoveInsn(*movInsn);
  nextBB->RemoveInsn(*brInsn);
  /* nextBB is now a fallthru bb, not a goto bb */
  nextBB->SetKind(BB::kBBFallthru);
  /*
   * fix control flow, we have bb, nextBB, targetBB, nextBB_target
   * connect bb -> nextBB_target erase targetBB
   */
  it = bb.GetSuccsBegin();
  CHECK_FATAL(it != bb.GetSuccsEnd(), "succs is empty.");
  if (*it == targetBB) {
    bb.EraseSuccs(it);
    bb.PushFrontSuccs(*nextBBTarget);
  } else {
    ++it;
    bb.EraseSuccs(it);
    bb.PushBackSuccs(*nextBBTarget);
  }
  for (auto targetBBIt = targetBB->GetPredsBegin(); targetBBIt != targetBB->GetPredsEnd(); ++targetBBIt) {
    if (*targetBBIt == &bb) {
      targetBB->ErasePreds(targetBBIt);
      break;
    }
  }
  for (auto nextIt = nextBBTarget->GetPredsBegin(); nextIt != nextBBTarget->GetPredsEnd(); ++nextIt) {
    if (*nextIt == nextBB) {
      nextBBTarget->ErasePreds(nextIt);
      break;
    }
  }
  nextBBTarget->PushBackPreds(bb);

  /* nextBB has no target, originally just branch target */
  nextBB->EraseSuccs(nextBB->GetSuccsBegin());
  ASSERT(nextBB->GetSuccs().empty(), "peep: branch target incorrect");
  /* Now make nextBB fallthru to targetBB */
  nextBB->PushFrontSuccs(*targetBB);
  targetBB->PushBackPreds(*nextBB);
}

void ContiLDRorSTRToSameMEMAArch64::Run(BB &bb, Insn &insn) {
  Insn *prevInsn = insn.GetPrev();
  while (prevInsn != nullptr && !prevInsn->GetMachineOpcode() && prevInsn != bb.GetFirstInsn()) {
    prevInsn = prevInsn->GetPrev();
  }
  if (!insn.IsMachineInstruction() || prevInsn == nullptr) {
    return;
  }
  bool loadAfterStore = false;
  bool loadAfterLoad = false;
  MOperator thisMop = insn.GetMachineOpcode();
  MOperator prevMop = prevInsn->GetMachineOpcode();
  /*
   * store regB, RegC, offset
   * load regA, RegC, offset
   */
  if ((thisMop == MOP_xldr && prevMop == MOP_xstr) || (thisMop == MOP_wldr && prevMop == MOP_wstr) ||
      (thisMop == MOP_dldr && prevMop == MOP_dstr) || (thisMop == MOP_sldr && prevMop == MOP_sstr)) {
    loadAfterStore = true;
  }
  /*
   * load regA, RegC, offset
   * load regB, RegC, offset
   */
  if ((thisMop == MOP_xldr || thisMop == MOP_wldr || thisMop == MOP_dldr || thisMop == MOP_sldr) &&
      prevMop == thisMop) {
    loadAfterLoad = true;
  }
  if (!loadAfterStore && !loadAfterLoad) {
    return;
  }
  ASSERT(insn.GetOperand(kInsnSecondOpnd).IsMemoryAccessOperand(), "expects mem operands");
  ASSERT(prevInsn->GetOperand(kInsnSecondOpnd).IsMemoryAccessOperand(), "expects mem operands");

  auto &memOpnd1 = static_cast<AArch64MemOperand&>(insn.GetOperand(kInsnSecondOpnd));
  AArch64MemOperand::AArch64AddressingMode addrMode1 = memOpnd1.GetAddrMode();
  if (addrMode1 != AArch64MemOperand::kAddrModeBOi || (!memOpnd1.IsIntactIndexed())) {
    return;
  }

  auto *base1 = static_cast<AArch64RegOperand*>(memOpnd1.GetBaseRegister());
  ASSERT(base1 == nullptr || !base1->IsVirtualRegister(), "physical register has not been allocated?");
  AArch64OfstOperand *offset1 = memOpnd1.GetOffsetImmediate();

  auto &memOpnd2 = static_cast<AArch64MemOperand&>(prevInsn->GetOperand(kInsnSecondOpnd));
  AArch64MemOperand::AArch64AddressingMode addrMode2 = memOpnd2.GetAddrMode();
  if (addrMode2 != AArch64MemOperand::kAddrModeBOi || (!memOpnd2.IsIntactIndexed())) {
    return;
  }

  auto *base2 = static_cast<AArch64RegOperand*>(memOpnd2.GetBaseRegister());
  ASSERT(base2 == nullptr || !base2->IsVirtualRegister(), "physical register has not been allocated?");
  AArch64OfstOperand *offset2 = memOpnd2.GetOffsetImmediate();

  if (base1 == nullptr || base2 == nullptr || offset1 == nullptr || offset2 == nullptr) {
    return;
  }

  auto &reg1 = static_cast<AArch64RegOperand&>(insn.GetOperand(kInsnFirstOpnd));
  auto &reg2 = static_cast<AArch64RegOperand&>(prevInsn->GetOperand(kInsnFirstOpnd));
  int offsetVal1 = offset1->GetOffsetValue();
  int offsetVal2 = offset2->GetOffsetValue();
  if (base1->GetRegisterNumber() != base2->GetRegisterNumber() ||
      reg1.GetRegisterType() != reg2.GetRegisterType() || reg1.GetSize() != reg2.GetSize() ||
      offsetVal1 != offsetVal2) {
    return;
  }
  if (loadAfterStore && reg1.GetRegisterNumber() != reg2.GetRegisterNumber()) {
    /* replace it with mov */
    MOperator newOp = MOP_wmovrr;
    if (reg1.GetRegisterType() == kRegTyInt) {
      newOp = (reg1.GetSize() <= k32BitSize) ? MOP_wmovrr : MOP_xmovrr;
    } else if (reg1.GetRegisterType() == kRegTyFloat) {
      newOp = (reg1.GetSize() <= k32BitSize) ? MOP_xvmovs : MOP_xvmovd;
    }
    CG *cg = cgFunc.GetCG();
    bb.InsertInsnAfter(*prevInsn, cg->BuildInstruction<AArch64Insn>(newOp, reg1, reg2));
    bb.RemoveInsn(insn);
    insn = *(prevInsn->GetNext());
  } else if (reg1.GetRegisterNumber() == reg2.GetRegisterNumber() &&
             base1->GetRegisterNumber() != reg2.GetRegisterNumber()) {
    bb.RemoveInsn(insn);
    insn = *prevInsn;
  }
}

void RemoveIncDecRefAArch64::Run(BB &bb, Insn &insn) {
  ASSERT(insn.GetMachineOpcode() == MOP_xbl, "expect a xbl MOP at RemoveIncDecRef optimization");
  auto &target = static_cast<FuncNameOperand&>(insn.GetOperand(kInsnFirstOpnd));
  Insn *insnMov = insn.GetPreviousMachineInsn();
  if (insnMov == nullptr) {
    return;
  }
  MOperator mopMov = insnMov->GetMachineOpcode();
  if (target.GetName() == "MCC_IncDecRef_NaiveRCFast" && mopMov == MOP_xmovrr &&
      static_cast<RegOperand&>(insnMov->GetOperand(kInsnFirstOpnd)).GetRegisterNumber() == R1 &&
      static_cast<RegOperand&>(insnMov->GetOperand(kInsnSecondOpnd)).GetRegisterNumber() == R0) {
    bb.RemoveInsn(*insnMov);
    bb.RemoveInsn(insn);
    bb.SetKind(BB::kBBFallthru);
  }
}

#ifdef USE_32BIT_REF
constexpr uint32 kRefSize = 32;
#else
constexpr uint32 kRefSize = 64;
#endif

void InlineReadBarriersAArch64::Run(BB &bb, Insn &insn) {
  if (!CGOptions::IsGCOnly()) { /* Inline read barriers only enabled for GCONLY. */
    return;
  }
  const std::string &barrierName = GetReadBarrierName(insn);
  CG *cg = cgFunc.GetCG();
  if (barrierName == kMccDummy) {
    /* remove dummy call. */
    bb.RemoveInsn(insn);
  } else {
    /* replace barrier function call with load instruction. */
    bool isVolatile = (barrierName == kMccLoadRefV || barrierName == kMccLoadRefVS);
    bool isStatic = (barrierName == kMccLoadRefS || barrierName == kMccLoadRefVS);
    /* refSize is 32 if USE_32BIT_REF defined, otherwise 64. */
    const uint32 refSize = kRefSize;
    auto *aarch64CGFunc = static_cast<AArch64CGFunc*>(&cgFunc);
    MOperator loadOp = GetLoadOperator(refSize, isVolatile);
    RegOperand &regOp = aarch64CGFunc->GetOrCreatePhysicalRegisterOperand(R0, refSize, kRegTyInt);
    AArch64reg addrReg = isStatic ? R0 : R1;
    MemOperand &addr = aarch64CGFunc->CreateMemOpnd(addrReg, 0, refSize);
    Insn &loadInsn = cg->BuildInstruction<AArch64Insn>(loadOp, regOp, addr);
    bb.ReplaceInsn(insn, loadInsn);
  }
  bb.SetKind(BB::kBBFallthru);
  bool isTailCall = (insn.GetMachineOpcode() == MOP_tail_call_opt_xbl);
  if (isTailCall) {
    /* add 'ret' instruction for tail call optimized load barrier. */
    Insn &retInsn = cg->BuildInstruction<AArch64Insn>(MOP_xret);
    bb.AppendInsn(retInsn);
    bb.SetKind(BB::kBBReturn);
  }
}

void ReplaceDivToMultiAArch64::Run(BB &bb, Insn &insn) {
  Insn *prevInsn = insn.GetPreviousMachineInsn();
  if (prevInsn == nullptr) {
    return;
  }
  Insn *prePrevInsn = prevInsn->GetPreviousMachineInsn();
  auto &sdivOpnd1 = static_cast<RegOperand&>(insn.GetOperand(kInsnSecondOpnd));
  auto &sdivOpnd2 = static_cast<RegOperand&>(insn.GetOperand(kInsnThirdOpnd));
  if (sdivOpnd1.GetRegisterNumber() == sdivOpnd2.GetRegisterNumber() || sdivOpnd1.GetRegisterNumber() == R16 ||
      sdivOpnd2.GetRegisterNumber() == R16 || prePrevInsn == nullptr) {
    return;
  }
  MOperator prevMop = prevInsn->GetMachineOpcode();
  MOperator prePrevMop = prePrevInsn->GetMachineOpcode();
  if (prevMop && (prevMop == MOP_wmovkri16) && prePrevMop && (prePrevMop == MOP_xmovri32)) {
  /* Check if dest operand of insn is idential with  register of prevInsn and prePrevInsn. */
    if ((&(prevInsn->GetOperand(kInsnFirstOpnd)) != &sdivOpnd2) ||
        (&(prePrevInsn->GetOperand(kInsnFirstOpnd)) != &sdivOpnd2)) {
      return;
    }
    auto &prevLsl = static_cast<LogicalShiftLeftOperand&>(prevInsn->GetOperand(kInsnThirdOpnd));
    if (prevLsl.GetShiftAmount() != k16BitSize) {
      return;
    }
    auto &prevImmOpnd = static_cast<ImmOperand&>(prevInsn->GetOperand(kInsnSecondOpnd));
    auto &prePrevImmOpnd = static_cast<ImmOperand&>(prePrevInsn->GetOperand(kInsnSecondOpnd));
    /*
     * expect the immediate value of first mov is 0x086A0 which matches 0x186A0
     * because 0x10000 is ignored in 32 bits register
     */
    if ((prevImmOpnd.GetValue() != 1) || (prePrevImmOpnd.GetValue() != 34464)) {
      return;
    }
    auto *aarch64CGFunc = static_cast<AArch64CGFunc*>(&cgFunc);
    CG *cg = cgFunc.GetCG();
    /* mov   w16, #0x588f */
    RegOperand &tempOpnd = aarch64CGFunc->GetOrCreatePhysicalRegisterOperand(static_cast<AArch64reg>(R16),
                                                                             k64BitSize, kRegTyInt);
    /* create a immedate operand with this specific value */
    ImmOperand &multiplierLow = aarch64CGFunc->CreateImmOperand(0x588f, k32BitSize, false);
    Insn &multiplierLowInsn = cg->BuildInstruction<AArch64Insn>(MOP_xmovri32, tempOpnd, multiplierLow);
    bb.InsertInsnBefore(*prePrevInsn, multiplierLowInsn);

    /*
     * movk    w16, #0x4f8b, LSL #16
     * create a immedate operand with this specific value
     */
    ImmOperand &multiplierHigh = aarch64CGFunc->CreateImmOperand(0x4f8b, k32BitSize, false);
    LogicalShiftLeftOperand *multiplierHighLsl = aarch64CGFunc->GetLogicalShiftLeftOperand(k16BitSize, true);
    Insn &multiplierHighInsn =
        cg->BuildInstruction<AArch64Insn>(MOP_wmovkri16, tempOpnd, multiplierHigh, *multiplierHighLsl);
    bb.InsertInsnBefore(*prePrevInsn, multiplierHighInsn);

    /* smull   x16, w0, w16 */
    Insn &newSmullInsn =
        cg->BuildInstruction<AArch64Insn>(MOP_xsmullrrr, tempOpnd, sdivOpnd1, tempOpnd);
    bb.InsertInsnBefore(*prePrevInsn, newSmullInsn);

    /* asr     x16, x16, #32 */
    ImmOperand &dstLsrImmHigh = aarch64CGFunc->CreateImmOperand(k32BitSize, k32BitSize, false);
    Insn &dstLsrInsnHigh =
        cg->BuildInstruction<AArch64Insn>(MOP_xasrrri6, tempOpnd, tempOpnd, dstLsrImmHigh);
    bb.InsertInsnBefore(*prePrevInsn, dstLsrInsnHigh);

    /* add     x16, x16, w0, SXTW */
    Operand &sxtw = aarch64CGFunc->CreateExtendShiftOperand(ExtendShiftOperand::kSXTW, 0, 3);
    Insn &addInsn =
        cg->BuildInstruction<AArch64Insn>(MOP_xxwaddrrre, tempOpnd, tempOpnd, sdivOpnd1, sxtw);
    bb.InsertInsnBefore(*prePrevInsn, addInsn);

    /* asr     x16, x16, #17 */
    ImmOperand &dstLsrImmChange = aarch64CGFunc->CreateImmOperand(17, k32BitSize, false);
    Insn &dstLsrInsnChange =
        cg->BuildInstruction<AArch64Insn>(MOP_xasrrri6, tempOpnd, tempOpnd, dstLsrImmChange);
    bb.InsertInsnBefore(*prePrevInsn, dstLsrInsnChange);

    /* add     x2, x16, x0, LSR #31 */
    auto &sdivOpnd0 = static_cast<RegOperand&>(insn.GetOperand(kInsnFirstOpnd));
    regno_t sdivOpnd0RegNO = sdivOpnd0.GetRegisterNumber();
    RegOperand &extendSdivOpnd0 =
        aarch64CGFunc->GetOrCreatePhysicalRegisterOperand(static_cast<AArch64reg>(sdivOpnd0RegNO),
                                                          k64BitSize, kRegTyInt);

    regno_t sdivOpnd1RegNum = sdivOpnd1.GetRegisterNumber();
    RegOperand &extendSdivOpnd1 =
        aarch64CGFunc->GetOrCreatePhysicalRegisterOperand(static_cast<AArch64reg>(sdivOpnd1RegNum),
                                                          k64BitSize, kRegTyInt);
    /* shift bit amount is thirty-one at this insn */
    BitShiftOperand &addLsrOpnd = aarch64CGFunc->CreateBitShiftOperand(BitShiftOperand::kLSR, 31, 6);
    Insn &addLsrInsn = cg->BuildInstruction<AArch64Insn>(MOP_xaddrrrs, extendSdivOpnd0, tempOpnd,
                                                         extendSdivOpnd1, addLsrOpnd);
    bb.InsertInsnBefore(*prePrevInsn, addLsrInsn);

    /*
     * remove insns
     * Check if x1 is used after sdiv insn, and if it is in live-out.
     */
    if (sdivOpnd2.GetRegisterNumber() != sdivOpnd0.GetRegisterNumber()) {
      if (IfOperandIsLiveAfterInsn(sdivOpnd2, insn)) {
        /* Only remove div instruction. */
        bb.RemoveInsn(insn);
        return;
      }
    }

    bb.RemoveInsn(*prePrevInsn);
    bb.RemoveInsn(*prevInsn);
    bb.RemoveInsn(insn);
  }
}

void AndCmpBranchesToCsetAArch64::Run(BB &bb, Insn &insn) {
   /* prevInsn must be "cmp" insn */
  Insn *prevInsn = insn.GetPreviousMachineInsn();
  if (prevInsn == nullptr ||
      (prevInsn->GetMachineOpcode() != MOP_wcmpri && prevInsn->GetMachineOpcode() != MOP_xcmpri)) {
    return;
  }
  /* prevPrevInsn must be "and" insn */
  Insn *prevPrevInsn = prevInsn->GetPreviousMachineInsn();
  if (prevPrevInsn == nullptr ||
      (prevPrevInsn->GetMachineOpcode() != MOP_wandrri12 && prevPrevInsn->GetMachineOpcode() != MOP_xandrri13)) {
    return;
  }

  auto &csetCond = static_cast<CondOperand&>(insn.GetOperand(kInsnSecondOpnd));
  auto &cmpImm = static_cast<ImmOperand&>(prevInsn->GetOperand(kInsnThirdOpnd));
  int64 cmpImmVal = cmpImm.GetValue();
  auto &andImm = static_cast<ImmOperand&>(prevPrevInsn->GetOperand(kInsnThirdOpnd));
  int64 andImmVal = andImm.GetValue();
  if ((csetCond.GetCode() == CC_EQ && cmpImmVal == andImmVal) ||
      (csetCond.GetCode() == CC_NE && cmpImmVal == 0)) {
    /* if flag_reg of "cmp" is live later, we can't remove cmp insn. */
    auto &flagReg = static_cast<RegOperand&>(prevInsn->GetOperand(kInsnFirstOpnd));
    if (FindRegLiveOut(flagReg, *prevInsn->GetBB())) {
      return;
    }

    auto &csetReg = static_cast<RegOperand&>(insn.GetOperand(kInsnFirstOpnd));
    if (andImmVal == 1) {
      if (!RegOperand::IsSameRegNO(csetReg, prevInsn->GetOperand(kInsnSecondOpnd)) ||
          !RegOperand::IsSameRegNO(csetReg, prevPrevInsn->GetOperand(kInsnFirstOpnd))) {
        return;
      }
      /* save the "and" insn only. */
      bb.RemoveInsn(insn);
      bb.RemoveInsn(*prevInsn);
    } else {
      if (!RegOperand::IsSameReg(csetReg, prevInsn->GetOperand(kInsnSecondOpnd)) ||
          !RegOperand::IsSameReg(csetReg, prevPrevInsn->GetOperand(kInsnFirstOpnd)) ||
          !RegOperand::IsSameReg(csetReg, prevPrevInsn->GetOperand(kInsnSecondOpnd))) {
        return;
      }

      /* andImmVal is n power of 2 */
      int n = logValueAtBase2(andImmVal);
      if (n < 0) {
        return;
      }

      /* create ubfx insn */
      MOperator ubfxOp = (csetReg.GetSize() <= k32BitSize) ? MOP_wubfxrri5i5 : MOP_xubfxrri6i6;
      auto &reg = static_cast<AArch64RegOperand&>(csetReg);
      CG *cg = cgFunc.GetCG();
      auto *aarch64CGFunc = static_cast<AArch64CGFunc*>(&cgFunc);
      ImmOperand &bitPos = aarch64CGFunc->CreateImmOperand(n, k8BitSize, false);
      ImmOperand &bitSize = aarch64CGFunc->CreateImmOperand(1, k8BitSize, false);
      Insn &ubfxInsn = cg->BuildInstruction<AArch64Insn>(ubfxOp, reg, reg, bitPos, bitSize);
      bb.InsertInsnBefore(*prevPrevInsn, ubfxInsn);
      bb.RemoveInsn(insn);
      bb.RemoveInsn(*prevInsn);
      bb.RemoveInsn(*prevPrevInsn);
    }
  }
}

void ZeroCmpBranchesAArch64::Run(BB &bb, Insn &insn) {
  Insn *prevInsn = insn.GetPreviousMachineInsn();
  if (!insn.IsBranch() || insn.GetOperandSize() <= kInsnSecondOpnd || prevInsn == nullptr) {
    return;
  }
  if (!insn.GetOperand(kInsnSecondOpnd).IsLabel()) {
    return;
  }
  LabelOperand *label = &static_cast<LabelOperand&>(insn.GetOperand(kInsnSecondOpnd));
  RegOperand *regOpnd = nullptr;
  RegOperand *reg0 = nullptr;
  RegOperand *reg1 = nullptr;
  MOperator newOp = MOP_undef;
  ImmOperand *imm = nullptr;
  switch (prevInsn->GetMachineOpcode()) {
    case MOP_wcmpri:
    case MOP_xcmpri: {
      regOpnd = &static_cast<RegOperand&>(prevInsn->GetOperand(kInsnSecondOpnd));
      imm = &static_cast<ImmOperand&>(prevInsn->GetOperand(kInsnThirdOpnd));
      if (imm->GetValue() != 0) {
        return;
      }
      if (insn.GetMachineOpcode() == MOP_bge) {
        newOp = (regOpnd->GetSize() <= k32BitSize) ? MOP_wtbz : MOP_xtbz;
      } else if (insn.GetMachineOpcode() == MOP_blt) {
        newOp = (regOpnd->GetSize() <= k32BitSize) ? MOP_wtbnz : MOP_xtbnz;
      } else {
        return;
      }
      break;
    }
    case MOP_wcmprr:
    case MOP_xcmprr: {
      reg0 = &static_cast<RegOperand&>(prevInsn->GetOperand(kInsnSecondOpnd));
      reg1 = &static_cast<RegOperand&>(prevInsn->GetOperand(kInsnThirdOpnd));
      if (!reg0->IsZeroRegister() && !reg1->IsZeroRegister()) {
        return;
      }
      switch (insn.GetMachineOpcode()) {
        case MOP_bge:
          if (reg1->IsZeroRegister()) {
            regOpnd = &static_cast<RegOperand&>(prevInsn->GetOperand(kInsnSecondOpnd));
            newOp = (regOpnd->GetSize() <= k32BitSize) ? MOP_wtbz : MOP_xtbz;
          } else {
            return;
          }
          break;
        case MOP_ble:
          if (reg0->IsZeroRegister()) {
            regOpnd = &static_cast<RegOperand&>(prevInsn->GetOperand(kInsnThirdOpnd));
            newOp = (regOpnd->GetSize() <= k32BitSize) ? MOP_wtbz : MOP_xtbz;
          } else {
            return;
          }
          break;
        case MOP_blt:
          if (reg1->IsZeroRegister()) {
            regOpnd = &static_cast<RegOperand&>(prevInsn->GetOperand(kInsnSecondOpnd));
            newOp = (regOpnd->GetSize() <= k32BitSize) ? MOP_wtbnz : MOP_xtbnz;
          } else {
            return;
          }
          break;
        case MOP_bgt:
          if (reg0->IsZeroRegister()) {
            regOpnd = &static_cast<RegOperand&>(prevInsn->GetOperand(kInsnThirdOpnd));
            newOp = (regOpnd->GetSize() <= k32BitSize) ? MOP_wtbnz : MOP_xtbnz;
          } else {
            return;
          }
          break;
        default:
          return;
      }
      break;
    }
    default:
      return;
  }
  CG *cg = cgFunc.GetCG();
  auto aarch64CGFunc = static_cast<AArch64CGFunc*>(&cgFunc);
  ImmOperand &bitp = aarch64CGFunc->CreateImmOperand(
      (regOpnd->GetSize() <= k32BitSize) ? (k32BitSize - 1) : (k64BitSize - 1), k8BitSize, false);
  bb.InsertInsnAfter(
      insn, cg->BuildInstruction<AArch64Insn>(newOp, *static_cast<AArch64RegOperand*>(regOpnd), bitp, *label));
  bb.RemoveInsn(insn);
  bb.RemoveInsn(*prevInsn);
}

/*
 * if there is define point of checkInsn->GetOperand(opndIdx) between startInsn and  firstInsn
 * return define insn. else return nullptr
 */
const Insn *CmpCsetAArch64::DefInsnOfOperandInBB(const Insn &startInsn, const Insn &checkInsn, int opndIdx) {
  Insn *prevInsn = nullptr;
  for (const Insn *insn = &startInsn; insn != nullptr; insn = prevInsn) {
    prevInsn = insn->GetPreviousMachineInsn();
    if (!insn->IsMachineInstruction()) {
      continue;
    }
    /* checkInsn.GetOperand(opndIdx) is thought modified conservatively */
    if (insn->IsCall()) {
      return insn;
    }
    const AArch64MD *md = &AArch64CG::kMd[static_cast<const AArch64Insn*>(insn)->GetMachineOpcode()];
    uint32 opndNum = insn->GetOperandSize();
    for (uint32 i = 0; i < opndNum; ++i) {
      Operand &opnd = insn->GetOperand(i);
      AArch64OpndProp *regProp = static_cast<AArch64OpndProp*>(md->operand[i]);
      if (!regProp->IsDef()) {
        continue;
      }
      /* Operand is base reg of Memory, defined by str */
      if (opnd.IsMemoryAccessOperand()) {
        auto &memOpnd = static_cast<AArch64MemOperand&>(opnd);
        RegOperand *base = memOpnd.GetBaseRegister();
        ASSERT(base != nullptr, "nullptr check");
        ASSERT(base->IsRegister(), "expects RegOperand");
        if (RegOperand::IsSameRegNO(*base, checkInsn.GetOperand(opndIdx)) &&
            memOpnd.GetAddrMode() == AArch64MemOperand::kAddrModeBOi &&
            (memOpnd.IsPostIndexed() || memOpnd.IsPreIndexed())) {
          return insn;
        }
      } else {
        ASSERT(opnd.IsRegister(), "expects RegOperand");
        if (RegOperand::IsSameRegNO(checkInsn.GetOperand(opndIdx), opnd)) {
          return insn;
        }
      }
    }
  }
  return nullptr;
}

bool CmpCsetAArch64::OpndDefByOneValidBit(const Insn &defInsn) {
  MOperator defMop = defInsn.GetMachineOpcode();
  switch (defMop) {
    case MOP_wcsetrc:
    case MOP_xcsetrc:
      return true;
    case MOP_xmovri32:
    case MOP_xmovri64: {
      Operand &defOpnd = defInsn.GetOperand(kInsnSecondOpnd);
      ASSERT(defOpnd.IsIntImmediate(), "expects ImmOperand");
      auto &defConst = static_cast<ImmOperand&>(defOpnd);
      int64 defConstValue = defConst.GetValue();
      return (defConstValue == 0 || defConstValue == 1);
    }
    case MOP_xmovrr:
    case MOP_wmovrr:
      return defInsn.GetOperand(kInsnSecondOpnd).IsZeroRegister();
    case MOP_wlsrrri5:
    case MOP_xlsrrri6: {
      Operand &opnd2 = defInsn.GetOperand(kInsnThirdOpnd);
      ASSERT(opnd2.IsIntImmediate(), "expects ImmOperand");
      auto &opndImm = static_cast<ImmOperand&>(opnd2);
      int64 shiftBits = opndImm.GetValue();
      return ((defMop == MOP_wlsrrri5 && shiftBits == (k32BitSize - 1)) ||
              (defMop == MOP_xlsrrri6 && shiftBits == (k64BitSize - 1)));
    }
    default:
      return false;
  }
}

/*
 * help function for cmpcset optimize
 * if all define points of used opnd in insn has only one valid bit,return true.
 * for cmp reg,#0(#1), that is checking for reg
 */
bool CmpCsetAArch64::CheckOpndDefPoints(Insn &checkInsn, int opndIdx) {
  /* check current BB */
  const Insn *defInsn = DefInsnOfOperandInBB(checkInsn, checkInsn, opndIdx);
  if (defInsn != nullptr) {
    return OpndDefByOneValidBit(*defInsn);
  }
  /* check pred */
  for (auto predBB : checkInsn.GetBB()->GetPreds()) {
    const Insn *tempInsn = nullptr;
    if (predBB->GetLastInsn() != nullptr) {
      tempInsn = DefInsnOfOperandInBB(*predBB->GetLastInsn(), checkInsn, opndIdx);
    }
    if (tempInsn == nullptr || !OpndDefByOneValidBit(*tempInsn)) {
      return false;
    }
  }
  return true;
}

/* Check there is use point of rflag start from startInsn to current bb bottom */
bool CmpCsetAArch64::FlagUsedLaterInCurBB(const BB &bb, Insn &startInsn) const {
  if (&bb != startInsn.GetBB()) {
    return false;
  }
  Insn *nextInsn = nullptr;
  for (Insn *insn = &startInsn; insn != nullptr; insn = nextInsn) {
    nextInsn = insn->GetNextMachineInsn();
    const AArch64MD *md = &AArch64CG::kMd[static_cast<AArch64Insn*>(insn)->GetMachineOpcode()];
    uint32 opndNum = insn->GetOperandSize();
    for (uint32 i = 0; i < opndNum; ++i) {
      Operand &opnd = insn->GetOperand(i);
      /*
       * For condition operand, such as NE, EQ and so on, the register number should be
       * same with RFLAG, we only need check the property of use/def.
       */
      if (!opnd.IsConditionCode()) {
        continue;
      }
      AArch64OpndProp *regProp = static_cast<AArch64OpndProp*>(md->operand[i]);
      bool isUse = regProp->IsUse();
      if (isUse) {
        return true;
      } else {
        ASSERT(regProp->IsDef(), "register should be redefined.");
        return false;
      }
    }
  }
  return false;
}

void CmpCsetAArch64::Run(BB &bb, Insn &insn)  {
  Insn *nextInsn = insn.GetNextMachineInsn();
  if (nextInsn == nullptr) {
    return;
  }
  MOperator firstMop = insn.GetMachineOpcode();
  MOperator secondMop = nextInsn->GetMachineOpcode();
  if ((firstMop == MOP_wcmpri || firstMop == MOP_xcmpri) &&
      (secondMop == MOP_wcsetrc || secondMop == MOP_xcsetrc)) {
    Operand &cmpFirstOpnd = insn.GetOperand(kInsnSecondOpnd);
    /* get ImmOperand, must be 0 or 1 */
    Operand &cmpSecondOpnd = insn.GetOperand(kInsnThirdOpnd);
    auto &cmpFlagReg = static_cast<RegOperand&>(insn.GetOperand(kInsnFirstOpnd));
    ASSERT(cmpSecondOpnd.IsIntImmediate(), "expects ImmOperand");
    auto &cmpConst = static_cast<ImmOperand&>(cmpSecondOpnd);
    int64 cmpConstVal = cmpConst.GetValue();
    Operand &csetFirstOpnd = nextInsn->GetOperand(kInsnFirstOpnd);
    if ((cmpConstVal != 0 && cmpConstVal != 1) || !CheckOpndDefPoints(insn, 1) ||
        (nextInsn->GetNextMachineInsn() != nullptr &&
         FlagUsedLaterInCurBB(bb, *nextInsn->GetNextMachineInsn())) ||
        FindRegLiveOut(cmpFlagReg, *insn.GetBB())) {
      return;
    }

    Insn *csetInsn = nextInsn;
    nextInsn = nextInsn->GetNextMachineInsn();
    auto &cond = static_cast<CondOperand&>(csetInsn->GetOperand(kInsnSecondOpnd));
    if ((cmpConstVal == 0 && cond.GetCode() == CC_NE) || (cmpConstVal == 1 && cond.GetCode() == CC_EQ)) {
      if (RegOperand::IsSameRegNO(cmpFirstOpnd, csetFirstOpnd)) {
        bb.RemoveInsn(insn);
        bb.RemoveInsn(*csetInsn);
      } else {
        if (cmpFirstOpnd.GetSize() != csetFirstOpnd.GetSize()) {
          return;
        }
        MOperator mopCode = (cmpFirstOpnd.GetSize() == k64BitSize) ? MOP_xmovrr : MOP_wmovrr;
        Insn &newInsn = cgFunc.GetCG()->BuildInstruction<AArch64Insn>(mopCode, csetFirstOpnd, cmpFirstOpnd);
        bb.ReplaceInsn(insn, newInsn);
        bb.RemoveInsn(*csetInsn);
      }
    } else if ((cmpConstVal == 1 && cond.GetCode() == CC_NE) || (cmpConstVal == 0 && cond.GetCode() == CC_EQ)) {
      MOperator mopCode = (cmpFirstOpnd.GetSize() == k64BitSize) ? MOP_xeorrri13 : MOP_weorrri12;
      ImmOperand &one = static_cast<AArch64CGFunc*>(&cgFunc)->CreateImmOperand(1, k8BitSize, false);
      Insn &newInsn = cgFunc.GetCG()->BuildInstruction<AArch64Insn>(mopCode, csetFirstOpnd, cmpFirstOpnd, one);
      bb.ReplaceInsn(insn, newInsn);
      bb.RemoveInsn(*csetInsn);
    }
  }
}

/*
 * help function for DeleteMovAfterCbzOrCbnz
 * input:
 *        bb: the bb to be checked out
 *        checkCbz: to check out BB end with cbz or cbnz, if cbz, input true
 *        opnd: for MOV reg, #0, opnd indicate reg
 * return:
 *        according to cbz, return true if insn is cbz or cbnz and the first operand of cbz(cbnz) is same as input
 *      operand
 */
bool DeleteMovAfterCbzOrCbnzAArch64::PredBBCheck(BB &bb, bool checkCbz, const Operand &opnd) const {
  if (bb.GetKind() != BB::kBBIf) {
    return false;
  }

  Insn *condBr = cgcfg->FindLastCondBrInsn(bb);
  ASSERT(condBr != nullptr, "condBr must be found");
  if (!cgcfg->IsCompareAndBranchInsn(*condBr)) {
    return false;
  }
  MOperator mOp = condBr->GetMachineOpcode();
  if (checkCbz && mOp != MOP_wcbz && mOp != MOP_xcbz) {
    return false;
  }
  if (!checkCbz && mOp != MOP_xcbnz && mOp != MOP_wcbnz) {
    return false;
  }
  return RegOperand::IsSameRegNO(condBr->GetOperand(kInsnFirstOpnd), opnd);
}

bool DeleteMovAfterCbzOrCbnzAArch64::OpndDefByMovZero(const Insn &insn) const {
  MOperator defMop = insn.GetMachineOpcode();
  switch (defMop) {
    case MOP_xmovri32:
    case MOP_xmovri64: {
      Operand &defOpnd = insn.GetOperand(kInsnSecondOpnd);
      ASSERT(defOpnd.IsIntImmediate(), "expects ImmOperand");
      auto &defConst = static_cast<ImmOperand&>(defOpnd);
      int64 defConstValue = defConst.GetValue();
      if (defConstValue == 0) {
        return true;
      }
      return false;
    }
    case MOP_xmovrr:
    case MOP_wmovrr: {
      Operand &secondOpnd = insn.GetOperand(kInsnSecondOpnd);
      ASSERT(secondOpnd.IsRegister(), "expects RegOperand here");
      auto &regOpnd = static_cast<AArch64RegOperand&>(secondOpnd);
      return regOpnd.IsZeroRegister();
    }
    default:
      return false;
  }
}

/* check whether predefine insn of first operand of test_insn is exist in current BB */
bool DeleteMovAfterCbzOrCbnzAArch64::NoPreDefine(Insn &testInsn) const {
  Insn *nextInsn = nullptr;
  for (Insn *insn = testInsn.GetBB()->GetFirstInsn(); insn != nullptr && insn != &testInsn; insn = nextInsn) {
    nextInsn = insn->GetNextMachineInsn();
    if (!insn->IsMachineInstruction()) {
      continue;
    }
    ASSERT(!insn->IsCall(), "CG internal error, call insn should not be at the middle of the BB.");
    const AArch64MD *md = &AArch64CG::kMd[static_cast<AArch64Insn*>(insn)->GetMachineOpcode()];
    uint32 opndNum = insn->GetOperandSize();
    for (uint32 i = 0; i < opndNum; ++i) {
      Operand &opnd = insn->GetOperand(i);
      AArch64OpndProp *regProp = static_cast<AArch64OpndProp*>(md->operand[i]);
      if (!regProp->IsDef()) {
        continue;
      }
      if (opnd.IsMemoryAccessOperand()) {
        auto &memOpnd = static_cast<AArch64MemOperand&>(opnd);
        RegOperand *base = memOpnd.GetBaseRegister();
        ASSERT(base != nullptr, "nullptr check");
        ASSERT(base->IsRegister(), "expects RegOperand");
        if (RegOperand::IsSameRegNO(*base, testInsn.GetOperand(kInsnFirstOpnd)) &&
            memOpnd.GetAddrMode() == AArch64MemOperand::kAddrModeBOi &&
            (memOpnd.IsPostIndexed() || memOpnd.IsPreIndexed())) {
          return false;
        }
      } else {
        ASSERT(opnd.IsRegister(), "expects RegOperand");
        if (RegOperand::IsSameRegNO(testInsn.GetOperand(kInsnFirstOpnd), opnd)) {
          return false;
        }
      }
    }
  }
  return true;
}
void DeleteMovAfterCbzOrCbnzAArch64::ProcessBBHandle(BB *processBB, const BB &bb, const Insn &insn) {
  FOR_BB_INSNS_SAFE(processInsn, processBB, nextProcessInsn) {
    nextProcessInsn = processInsn->GetNextMachineInsn();
    if (!processInsn->IsMachineInstruction()) {
      continue;
    }
    /* register may be a caller save register */
    if (processInsn->IsCall()) {
      break;
    }
    if (!OpndDefByMovZero(*processInsn) || !NoPreDefine(*processInsn) ||
        !RegOperand::IsSameRegNO(processInsn->GetOperand(kInsnFirstOpnd), insn.GetOperand(kInsnFirstOpnd))) {
      continue;
    }
    bool toDoOpt = true;
    MOperator condBrMop = insn.GetMachineOpcode();
    /* process elseBB, other preds must be cbz */
    if (condBrMop == MOP_wcbnz || condBrMop == MOP_xcbnz) {
      /* check out all preds of process_bb */
      for (auto *processBBPred : processBB->GetPreds()) {
        if (processBBPred == &bb) {
          continue;
        }
        if (!PredBBCheck(*processBBPred, true, processInsn->GetOperand(kInsnFirstOpnd))) {
          toDoOpt = false;
          break;
        }
      }
    } else {
      /* process ifBB, other preds can be cbz or cbnz(one at most) */
      for (auto processBBPred : processBB->GetPreds()) {
        if (processBBPred == &bb) {
          continue;
        }
        /* for cbnz pred, there is one at most */
        if (!PredBBCheck(*processBBPred, processBBPred != processBB->GetPrev(),
                         processInsn->GetOperand(kInsnFirstOpnd))) {
          toDoOpt = false;
          break;
        }
      }
    }
    if (!toDoOpt) {
      continue;
    }
    processBB->RemoveInsn(*processInsn);
  }
}

/* ldr wn, [x1, wn, SXTW]
 * add x2, wn, x2
 */
bool ComplexMemOperandAddAArch64::IsExpandBaseOpnd(const Insn &insn, Insn &prevInsn) {
  MOperator prevMop = prevInsn.GetMachineOpcode();
  if (prevMop >= MOP_wldrsb && prevMop <= MOP_xldr &&
      prevInsn.GetOperand(kInsnFirstOpnd).Equals(insn.GetOperand(kInsnSecondOpnd))) {
    return true;
  }
  return false;
}

void ComplexMemOperandAddAArch64::Run(BB &bb, Insn &insn) {
  AArch64CGFunc *aarch64CGFunc = static_cast<AArch64CGFunc*>(&cgFunc);
  Insn *nextInsn = insn.GetNextMachineInsn();
  if (nextInsn == nullptr) {
    return;
  }
  Insn *prevInsn = insn.GetPreviousMachineInsn();
  MOperator thisMop = insn.GetMachineOpcode();
  if (thisMop != MOP_xaddrrr && thisMop != MOP_waddrrr) {
    return;
  }
  MOperator nextMop = nextInsn->GetMachineOpcode();
  if (nextMop &&
      ((nextMop >= MOP_wldrsb && nextMop <= MOP_dldr) || (nextMop >= MOP_wstrb && nextMop <= MOP_dstr))) {
    if (!IsMemOperandOptPattern(insn, *nextInsn)) {
      return;
    }
    AArch64MemOperand *memOpnd = static_cast<AArch64MemOperand*>(nextInsn->GetMemOpnd());
    auto newBaseOpnd = static_cast<RegOperand*>(&insn.GetOperand(kInsnSecondOpnd));
    auto newIndexOpnd = static_cast<RegOperand*>(&insn.GetOperand(kInsnThirdOpnd));
    regno_t memBaseOpndRegNO = newBaseOpnd->GetRegisterNumber();
    if (newBaseOpnd->GetSize() <= k32BitSize && prevInsn != nullptr && IsExpandBaseOpnd(insn, *prevInsn)) {
      newBaseOpnd = &aarch64CGFunc->GetOrCreatePhysicalRegisterOperand(static_cast<AArch64reg>(memBaseOpndRegNO),
                                                                       k64BitSize, kRegTyInt);
    }
    if (newBaseOpnd->GetSize() != k64BitSize) {
      return;
    }
    if (newIndexOpnd->GetSize() <= k32BitSize) {
      AArch64MemOperand &newMemOpnd =
          aarch64CGFunc->GetOrCreateMemOpnd(AArch64MemOperand::kAddrModeBOrX, memOpnd->GetSize(), newBaseOpnd,
                                            newIndexOpnd, 0, false);
      nextInsn->SetOperand(kInsnSecondOpnd, newMemOpnd);
    } else {
      AArch64MemOperand &newMemOpnd =
          aarch64CGFunc->GetOrCreateMemOpnd(AArch64MemOperand::kAddrModeBOrX, memOpnd->GetSize(), newBaseOpnd,
                                            newIndexOpnd, nullptr, nullptr);
      nextInsn->SetOperand(kInsnSecondOpnd, newMemOpnd);
    }
    bb.RemoveInsn(insn);
  }
}

void DeleteMovAfterCbzOrCbnzAArch64::Run(BB &bb, Insn &insn) {
  if (bb.GetKind() != BB::kBBIf) {
    return;
  }
  if (&insn != cgcfg->FindLastCondBrInsn(bb)) {
    return;
  }
  if (!cgcfg->IsCompareAndBranchInsn(insn)) {
    return;
  }
  BB *processBB = nullptr;
  if (bb.GetNext() == maplebe::CGCFG::GetTargetSuc(bb)) {
    return;
  }

  MOperator condBrMop = insn.GetMachineOpcode();
  if (condBrMop == MOP_wcbnz || condBrMop == MOP_xcbnz) {
    processBB = bb.GetNext();
  } else {
    processBB = maplebe::CGCFG::GetTargetSuc(bb);
  }

  ASSERT(processBB != nullptr, "process_bb is null in DeleteMovAfterCbzOrCbnzAArch64::Run");
  ProcessBBHandle(processBB, bb, insn);
}

MOperator OneHoleBranchesPreAArch64::FindNewMop(const BB &bb, const Insn &insn) const {
  MOperator newOp = MOP_undef;
  if (&insn != bb.GetLastInsn()) {
    return newOp;
  }
  MOperator thisMop = insn.GetMachineOpcode();
  if (thisMop != MOP_wcbz && thisMop != MOP_wcbnz && thisMop != MOP_xcbz && thisMop != MOP_xcbnz) {
    return newOp;
  }
  switch (thisMop) {
    case MOP_wcbz:
      newOp = MOP_wtbnz;
      break;
    case MOP_wcbnz:
      newOp = MOP_wtbz;
      break;
    case MOP_xcbz:
      newOp = MOP_xtbnz;
      break;
    case MOP_xcbnz:
      newOp = MOP_xtbz;
      break;
    default:
      CHECK_FATAL(false, "can not touch here");
      break;
  }
  return newOp;
}

void OneHoleBranchesPreAArch64::Run(BB &bb, Insn &insn) {
  AArch64CGFunc *aarch64CGFunc = static_cast<AArch64CGFunc*>(&cgFunc);
  MOperator newOp = FindNewMop(bb, insn);
  if (newOp == MOP_undef) {
    return;
  }
  Insn *prevInsn = insn.GetPreviousMachineInsn();
  LabelOperand &label = static_cast<LabelOperand&>(insn.GetOperand(kInsnSecondOpnd));
  if (prevInsn != nullptr && prevInsn->GetMachineOpcode() == MOP_xuxtb32 &&
      (static_cast<RegOperand&>(prevInsn->GetOperand(kInsnSecondOpnd)).GetValidBitsNum() <= k8BitSize ||
       static_cast<RegOperand&>(prevInsn->GetOperand(kInsnFirstOpnd)).GetValidBitsNum() <= k8BitSize)) {
    if (&(prevInsn->GetOperand(kInsnFirstOpnd)) != &(insn.GetOperand(kInsnFirstOpnd))) {
      return;
    }
    insn.SetOperand(kInsnFirstOpnd, prevInsn->GetOperand(kInsnSecondOpnd));
    bb.RemoveInsn(*prevInsn);
  }
  if (prevInsn != nullptr &&
      (prevInsn->GetMachineOpcode() == MOP_xeorrri13 || prevInsn->GetMachineOpcode() == MOP_weorrri12) &&
      static_cast<ImmOperand&>(prevInsn->GetOperand(kInsnThirdOpnd)).GetValue() == 1) {
    if (&(prevInsn->GetOperand(kInsnFirstOpnd)) != &(insn.GetOperand(kInsnFirstOpnd))) {
      return;
    }
    Insn *prevPrevInsn = prevInsn->GetPreviousMachineInsn();
    if (prevPrevInsn == nullptr) {
      return;
    }
    if (prevPrevInsn->GetMachineOpcode() != MOP_xuxtb32 ||
        static_cast<RegOperand&>(prevPrevInsn->GetOperand(kInsnSecondOpnd)).GetValidBitsNum() != 1) {
      return;
    }
    if (&(prevPrevInsn->GetOperand(kInsnFirstOpnd)) != &(prevInsn->GetOperand(kInsnSecondOpnd))) {
      return;
    }
    ImmOperand &oneHoleOpnd = aarch64CGFunc->CreateImmOperand(0, k8BitSize, false);
    auto &regOperand = static_cast<AArch64RegOperand&>(prevPrevInsn->GetOperand(kInsnSecondOpnd));
    bb.InsertInsnAfter(insn, cgFunc.GetCG()->BuildInstruction<AArch64Insn>(newOp, regOperand, oneHoleOpnd, label));
    bb.RemoveInsn(insn);
    bb.RemoveInsn(*prevInsn);
    bb.RemoveInsn(*prevPrevInsn);
  }
}

bool LoadFloatPointAArch64::FindLoadFloatPoint(std::vector<Insn*> &optInsn, Insn &insn) {
  MOperator mOp = insn.GetMachineOpcode();
  optInsn.clear();
  if (mOp != MOP_xmovzri16) {
    return false;
  }
  optInsn.push_back(&insn);

  Insn *insnMov2 = insn.GetNextMachineInsn();
  if (insnMov2 == nullptr) {
    return false;
  }
  if (insnMov2->GetMachineOpcode() != MOP_xmovkri16) {
    return false;
  }
  optInsn.push_back(insnMov2);

  Insn *insnMov3 = insnMov2->GetNextMachineInsn();
  if (insnMov3 == nullptr) {
    return false;
  }
  if (insnMov3->GetMachineOpcode() != MOP_xmovkri16) {
    return false;
  }
  optInsn.push_back(insnMov3);

  Insn *insnMov4 = insnMov3->GetNextMachineInsn();
  if (insnMov4 == nullptr) {
    return false;
  }
  if (insnMov4->GetMachineOpcode() != MOP_xmovkri16) {
    return false;
  }
  optInsn.push_back(insnMov4);
  return true;
}

bool LoadFloatPointAArch64::IsPatternMatch(const std::vector<Insn*> &optInsn) {
  int insnNum = 0;
  Insn *insn1 = optInsn[insnNum];
  Insn *insn2 = optInsn[++insnNum];
  Insn *insn3 = optInsn[++insnNum];
  Insn *insn4 = optInsn[++insnNum];
  if ((static_cast<RegOperand&>(insn1->GetOperand(kInsnFirstOpnd)).GetRegisterNumber() !=
       static_cast<RegOperand&>(insn2->GetOperand(kInsnFirstOpnd)).GetRegisterNumber()) ||
      (static_cast<RegOperand&>(insn2->GetOperand(kInsnFirstOpnd)).GetRegisterNumber() !=
       static_cast<RegOperand&>(insn3->GetOperand(kInsnFirstOpnd)).GetRegisterNumber()) ||
      (static_cast<RegOperand&>(insn3->GetOperand(kInsnFirstOpnd)).GetRegisterNumber() !=
       static_cast<RegOperand&>(insn4->GetOperand(kInsnFirstOpnd)).GetRegisterNumber())) {
    return false;
  }
  if ((static_cast<LogicalShiftLeftOperand&>(insn1->GetOperand(kInsnThirdOpnd)).GetShiftAmount() != 0) ||
      (static_cast<LogicalShiftLeftOperand&>(insn2->GetOperand(kInsnThirdOpnd)).GetShiftAmount() !=
       k16BitSize) ||
      (static_cast<LogicalShiftLeftOperand&>(insn3->GetOperand(kInsnThirdOpnd)).GetShiftAmount() !=
       k32BitSize) ||
      (static_cast<LogicalShiftLeftOperand&>(insn4->GetOperand(kInsnThirdOpnd)).GetShiftAmount() !=
       (k16BitSize + k32BitSize))) {
    return false;
  }
  return true;
}

void LoadFloatPointAArch64::Run(BB &bb, Insn &insn) {
  AArch64CGFunc *aarch64CGFunc = static_cast<AArch64CGFunc*>(&cgFunc);
  /* logical shift left values in three optimized pattern */
  std::vector<Insn*> optInsn;
  if (FindLoadFloatPoint(optInsn, insn) && IsPatternMatch(optInsn)) {
    int insnNum = 0;
    Insn *insn1 = optInsn[insnNum];
    Insn *insn2 = optInsn[++insnNum];
    Insn *insn3 = optInsn[++insnNum];
    Insn *insn4 = optInsn[++insnNum];
    auto &movConst1 = static_cast<AArch64ImmOperand&>(insn1->GetOperand(kInsnSecondOpnd));
    auto &movConst2 = static_cast<AArch64ImmOperand&>(insn2->GetOperand(kInsnSecondOpnd));
    auto &movConst3 = static_cast<AArch64ImmOperand&>(insn3->GetOperand(kInsnSecondOpnd));
    auto &movConst4 = static_cast<AArch64ImmOperand&>(insn4->GetOperand(kInsnSecondOpnd));
    /* movk/movz's immOpnd is 16-bit unsigned immediate */
    uint64 value = static_cast<uint64>(movConst1.GetValue()) +
                   (static_cast<uint64>(movConst2.GetValue()) << k16BitSize) +
                   (static_cast<uint64>(movConst3.GetValue()) << k32BitSize) +
                   (static_cast<uint64>(movConst4.GetValue()) << (k16BitSize + k32BitSize));

    LabelIdx lableIdx = cgFunc.CreateLabel();
    LabelOperand &target = aarch64CGFunc->GetOrCreateLabelOperand(lableIdx);
    cgFunc.InsertLabelMap(lableIdx, value);
    Insn &newInsn = cgFunc.GetCG()->BuildInstruction<AArch64Insn>(MOP_xldli, insn4->GetOperand(kInsnFirstOpnd),
                                                                  target);
    bb.InsertInsnAfter(*insn4, newInsn);
    bb.RemoveInsn(*insn1);
    bb.RemoveInsn(*insn2);
    bb.RemoveInsn(*insn3);
    bb.RemoveInsn(*insn4);
  }
}

void ReplaceOrrToMovAArch64::Run(BB &bb, Insn &insn){
  Operand *opndOfOrr = nullptr;
  ImmOperand *immOpnd = nullptr;
  AArch64RegOperand *reg1 = nullptr;
  AArch64RegOperand *reg2 = nullptr;
  MOperator thisMop = insn.GetMachineOpcode();
  MOperator newMop = MOP_undef;
  switch (thisMop) {
    case MOP_wiorri12r: {  /* opnd1 is Reg32 and opnd2 is immediate. */
      opndOfOrr = &(insn.GetOperand(kInsnSecondOpnd));
      reg2 = &static_cast<AArch64RegOperand&>(insn.GetOperand(kInsnThirdOpnd));
      newMop = MOP_wmovrr;
      break;
    }
    case MOP_wiorrri12: {  /* opnd1 is reg32 and opnd3 is immediate. */
      opndOfOrr = &(insn.GetOperand(kInsnThirdOpnd));
      reg2 = &static_cast<AArch64RegOperand&>(insn.GetOperand(kInsnSecondOpnd));
      newMop = MOP_wmovrr;
      break;
    }
    case MOP_xiorri13r: {  /* opnd1 is Reg64 and opnd2 is immediate. */
      opndOfOrr = &(insn.GetOperand(kInsnSecondOpnd));
      reg2 = &static_cast<AArch64RegOperand&>(insn.GetOperand(kInsnThirdOpnd));
      newMop = MOP_xmovrr;
      break;
    }
    case MOP_xiorrri13: {  /* opnd1 is reg64 and opnd3 is immediate. */
      opndOfOrr = &(insn.GetOperand(kInsnThirdOpnd));
      reg2 = &static_cast<AArch64RegOperand&>(insn.GetOperand(kInsnSecondOpnd));
      newMop = MOP_xmovrr;
      break;
    }
    default:
      break;
  }
  ASSERT(opndOfOrr->IsIntImmediate(), "expects immediate operand");
  immOpnd = static_cast<ImmOperand*>(opndOfOrr);
  if (immOpnd->GetValue() == 0) {
    reg1 = &static_cast<AArch64RegOperand&>(insn.GetOperand(kInsnFirstOpnd));
    bb.ReplaceInsn(insn, cgFunc.GetCG()->BuildInstruction<AArch64Insn>(newMop, *reg1, *reg2));
  }
}

void ReplaceCmpToCmnAArch64::Run(BB &bb, Insn &insn) {
  AArch64CGFunc *aarch64CGFunc = static_cast<AArch64CGFunc*>(&cgFunc);
  MOperator thisMop = insn.GetMachineOpcode();
  MOperator nextMop = MOP_undef;
  MOperator newMop = MOP_undef;
  switch (thisMop) {
    case MOP_xmovri32: {
      nextMop = MOP_wcmprr;
      newMop = MOP_wcmnri;
      break;
    }
    case MOP_xmovri64: {
      nextMop = MOP_xcmprr;
      newMop = MOP_xcmnri;
      break;
    }
    default:
      break;
  }
  Operand *opnd1OfMov = &(insn.GetOperand(kInsnFirstOpnd));
  Operand *opnd2OfMov = &(insn.GetOperand(kInsnSecondOpnd));
  if (opnd2OfMov->IsIntImmediate()) {
    ImmOperand *immOpnd = static_cast<ImmOperand*>(opnd2OfMov);
    int64 iVal = immOpnd->GetValue();
    if (kNegativeImmLowerLimit <= iVal && iVal < 0) {
      Insn *nextInsn = insn.GetNextMachineInsn();  /* get the next insn to judge if it is a cmp instruction. */
      if (nextInsn != nullptr) {
        if (nextInsn->GetMachineOpcode() == nextMop) {
          Operand *opndCmp2 = &(nextInsn->GetOperand(kInsnSecondOpnd));
          Operand *opndCmp3 = &(nextInsn->GetOperand(kInsnThirdOpnd));  /* get the third operand of cmp */
          /* if the first operand of mov equals the third operand of cmp, match the pattern. */
          if (opnd1OfMov == opndCmp3) {
            ImmOperand &newOpnd = aarch64CGFunc->CreateImmOperand(iVal * (-1), immOpnd->GetSize(), false);
            Operand &regFlag = nextInsn->GetOperand(kInsnFirstOpnd);
            bb.ReplaceInsn(*nextInsn, cgFunc.GetCG()->BuildInstruction<AArch64Insn>(MOperator(newMop), regFlag,
                                                                                    *opndCmp2, newOpnd));
          }
        }
      }
    }
  }
}

void RemoveIncRefAArch64::Run(BB &bb, Insn &insn) {
  MOperator mOp = insn.GetMachineOpcode();
  if (mOp != MOP_xbl) {
    return;
  }
  auto &target = static_cast<FuncNameOperand&>(insn.GetOperand(kInsnFirstOpnd));
  if (target.GetName() != "MCC_IncDecRef_NaiveRCFast") {
    return;
  }
  Insn *insnMov2 = insn.GetPreviousMachineInsn();
  if (insnMov2 == nullptr) {
    return;
  }
  MOperator mopMov2 = insnMov2->GetMachineOpcode();
  if (mopMov2 != MOP_xmovrr) {
    return;
  }
  Insn *insnMov1 = insnMov2->GetPreviousMachineInsn();
  if (insnMov1 == nullptr) {
    return;
  }
  MOperator mopMov1 = insnMov1->GetMachineOpcode();
  if (mopMov1 != MOP_xmovrr) {
    return;
  }
  if (static_cast<RegOperand&>(insnMov1->GetOperand(kInsnSecondOpnd)).GetRegisterNumber() !=
      static_cast<RegOperand&>(insnMov2->GetOperand(kInsnSecondOpnd)).GetRegisterNumber()) {
    return;
  }
  auto &mov2Dest = static_cast<RegOperand&>(insnMov2->GetOperand(kInsnFirstOpnd));
  auto &mov1Dest = static_cast<RegOperand&>(insnMov1->GetOperand(kInsnFirstOpnd));
  if (mov1Dest.IsVirtualRegister() || mov2Dest.IsVirtualRegister() || mov1Dest.GetRegisterNumber() != R0 ||
      mov2Dest.GetRegisterNumber() != R1) {
    return;
  }
  bb.RemoveInsn(insn);
  bb.RemoveInsn(*insnMov2);
  bb.RemoveInsn(*insnMov1);
  bb.SetKind(BB::kBBFallthru);
}

bool LongIntCompareWithZAArch64::FindLondIntCmpWithZ(std::vector<Insn*> &optInsn, Insn &insn) {
  MOperator thisMop = insn.GetMachineOpcode();
  optInsn.clear();
  /* first */
  if (thisMop != MOP_xcmpri) {
    return false;
  }
  optInsn.push_back(&insn);

  /* second */
  Insn *nextInsn1 = insn.GetNextMachineInsn();
  if (nextInsn1 == nullptr) {
    return false;
  }
  MOperator nextMop1 = nextInsn1->GetMachineOpcode();
  if (nextMop1 != MOP_wcsinvrrrc) {
    return false;
  }
  optInsn.push_back(nextInsn1);

  /* third */
  Insn *nextInsn2 = nextInsn1->GetNextMachineInsn();
  if (nextInsn2 == nullptr) {
    return false;
  }
  MOperator nextMop2 = nextInsn2->GetMachineOpcode();
  if (nextMop2 != MOP_wcsincrrrc) {
    return false;
  }
  optInsn.push_back(nextInsn2);

  /* forth */
  Insn *nextInsn3 = nextInsn2->GetNextMachineInsn();
  if (nextInsn3 == nullptr) {
    return false;
  }
  MOperator nextMop3 = nextInsn3->GetMachineOpcode();
  if (nextMop3 != MOP_wcmpri) {
    return false;
  }
  optInsn.push_back(nextInsn3);
  return true;
}

bool LongIntCompareWithZAArch64::IsPatternMatch(const std::vector<Insn*> &optInsn) {
  constexpr int insnLen = 4;
  if (optInsn.size() != insnLen) {
    return false;
  }
  int insnNum = 0;
  Insn *insn1 = optInsn[insnNum];
  Insn *insn2 = optInsn[++insnNum];
  Insn *insn3 = optInsn[++insnNum];
  Insn *insn4 = optInsn[++insnNum];
  ASSERT(insnNum == 3, " this specific case has three insns");
  if (insn2->GetOperand(kInsnSecondOpnd).IsZeroRegister() && insn2->GetOperand(kInsnThirdOpnd).IsZeroRegister() &&
      insn3->GetOperand(kInsnThirdOpnd).IsZeroRegister() &&
      &(insn3->GetOperand(kInsnFirstOpnd)) == &(insn3->GetOperand(kInsnSecondOpnd)) &&
      static_cast<CondOperand&>(insn2->GetOperand(kInsnFourthOpnd)).GetCode() == CC_GE &&
      static_cast<CondOperand&>(insn3->GetOperand(kInsnFourthOpnd)).GetCode() == CC_LE &&
      static_cast<ImmOperand&>(insn1->GetOperand(kInsnThirdOpnd)).GetValue() == 0 &&
      static_cast<ImmOperand&>(insn4->GetOperand(kInsnThirdOpnd)).GetValue() == 0) {
    return true;
  }
  return false;
}

void LongIntCompareWithZAArch64::Run(BB &bb, Insn &insn) {
  std::vector<Insn*> optInsn;
  /* found pattern */
  if (FindLondIntCmpWithZ(optInsn, insn) && IsPatternMatch(optInsn)) {
    Insn &newInsn = cgFunc.GetCG()->BuildInstruction<AArch64Insn>(optInsn[0]->GetMachineOpcode(),
                                                                  optInsn[0]->GetOperand(kInsnFirstOpnd),
                                                                  optInsn[0]->GetOperand(kInsnSecondOpnd),
                                                                  optInsn[0]->GetOperand(kInsnThirdOpnd));
    /* use newInsn to replace  the third optInsn */
    bb.ReplaceInsn(*optInsn[3], newInsn);
    optInsn.clear();
  }
}

void ComplexMemOperandAArch64::Run(BB &bb, Insn &insn) {
  AArch64CGFunc *aarch64CGFunc = static_cast<AArch64CGFunc*>(&cgFunc);
  Insn *nextInsn = insn.GetNextMachineInsn();
  if (nextInsn == nullptr) {
    return;
  }
  MOperator thisMop = insn.GetMachineOpcode();
  if (thisMop != MOP_xadrpl12) {
    return;
  }
  MOperator nextMop = nextInsn->GetMachineOpcode();
  if (nextMop &&
      ((nextMop >= MOP_wldrsb && nextMop <= MOP_dldp) || (nextMop >= MOP_wstrb && nextMop <= MOP_dstp))) {
    /* Check if base register of nextInsn and the dest operand of insn are identical. */
    AArch64MemOperand *memOpnd = static_cast<AArch64MemOperand*>(nextInsn->GetMemOpnd());
    ASSERT(memOpnd != nullptr, "memOpnd is null in AArch64Peep::ComplexMemOperandAArch64");

    /* Only for AddrMode_B_OI addressing mode. */
    if (memOpnd->GetAddrMode() != AArch64MemOperand::kAddrModeBOi) {
      return;
    }

    /* Only for intact memory addressing. */
    if (!memOpnd->IsIntactIndexed()) {
      return;
    }

    auto &regOpnd = static_cast<RegOperand&>(insn.GetOperand(kInsnFirstOpnd));

    /* Check if dest operand of insn is idential with base register of nextInsn. */
    if (memOpnd->GetBaseRegister() != &regOpnd) {
      return;
    }

    /* Check if x0 is used after ldr insn, and if it is in live-out. */
    if (IfOperandIsLiveAfterInsn(regOpnd, *nextInsn)) {
      return;
    }

    auto &stImmOpnd = static_cast<StImmOperand&>(insn.GetOperand(kInsnThirdOpnd));
    AArch64OfstOperand &offOpnd = aarch64CGFunc->GetOrCreateOfstOpnd(
        stImmOpnd.GetOffset() + memOpnd->GetOffsetImmediate()->GetOffsetValue(), k32BitSize);
    auto &newBaseOpnd = static_cast<RegOperand&>(insn.GetOperand(kInsnSecondOpnd));
    AArch64MemOperand &newMemOpnd =
        aarch64CGFunc->GetOrCreateMemOpnd(AArch64MemOperand::kAddrModeLo12Li, memOpnd->GetSize(),
                                          &newBaseOpnd, nullptr, &offOpnd, stImmOpnd.GetSymbol());

    nextInsn->SetOperand(kInsnSecondOpnd, newMemOpnd);
    bb.RemoveInsn(insn);
    CHECK_FATAL(!CGOptions::IsLazyBinding() || cgFunc.GetCG()->IsLibcore(),
        "this pattern can't be found in this phase");
  }
}

void ComplexMemOperandPreAddAArch64::Run(BB &bb, Insn &insn) {
  AArch64CGFunc *aarch64CGFunc = static_cast<AArch64CGFunc*>(&cgFunc);
  Insn *nextInsn = insn.GetNextMachineInsn();
  if (nextInsn == nullptr) {
    return;
  }
  MOperator thisMop = insn.GetMachineOpcode();
  if (thisMop != MOP_xaddrrr && thisMop != MOP_waddrrr) {
    return;
  }
  MOperator nextMop = nextInsn->GetMachineOpcode();
  if (nextMop &&
      ((nextMop >= MOP_wldrsb && nextMop <= MOP_dldr) || (nextMop >= MOP_wstrb && nextMop <= MOP_dstr))) {
    if (!IsMemOperandOptPattern(insn, *nextInsn)) {
      return;
    }
    AArch64MemOperand *memOpnd = static_cast<AArch64MemOperand*>(nextInsn->GetMemOpnd());
    auto &newBaseOpnd = static_cast<RegOperand&>(insn.GetOperand(kInsnSecondOpnd));
    auto &newIndexOpnd = static_cast<RegOperand&>(insn.GetOperand(kInsnThirdOpnd));
    if (newBaseOpnd.GetSize() != k64BitSize) {
      return;
    }
    if (newIndexOpnd.GetSize() <= k32BitSize) {
      AArch64MemOperand &newMemOpnd =
          aarch64CGFunc->GetOrCreateMemOpnd(AArch64MemOperand::kAddrModeBOrX, memOpnd->GetSize(), &newBaseOpnd,
                                            &newIndexOpnd, 0, false);
      nextInsn->SetOperand(kInsnSecondOpnd, newMemOpnd);
    } else {
      AArch64MemOperand &newMemOpnd =
          aarch64CGFunc->GetOrCreateMemOpnd(AArch64MemOperand::kAddrModeBOrX, memOpnd->GetSize(), &newBaseOpnd,
                                            &newIndexOpnd, nullptr, nullptr);
      nextInsn->SetOperand(kInsnSecondOpnd, newMemOpnd);
    }
    bb.RemoveInsn(insn);
  }
}

bool ComplexMemOperandLSLAArch64::CheckShiftValid(const AArch64MemOperand &memOpnd, BitShiftOperand &lsl) const {
  /* check if shift amount is valid */
  uint32 lslAmount = lsl.GetShiftAmount();
  constexpr uint8 twoShiftBits = 2;
  constexpr uint8 threeShiftBits = 3;
  if ((memOpnd.GetSize() == k32BitSize && (lsl.GetShiftAmount() != 0 && lslAmount != twoShiftBits)) ||
      (memOpnd.GetSize() == k64BitSize && (lsl.GetShiftAmount() != 0 && lslAmount != threeShiftBits))) {
    return false;
  }
  if (memOpnd.GetSize() != (k8BitSize << lslAmount)) {
    return false;
  }
  return true;
}

void ComplexMemOperandLSLAArch64::Run(BB &bb, Insn &insn) {
  AArch64CGFunc *aarch64CGFunc = static_cast<AArch64CGFunc*>(&cgFunc);
  Insn *nextInsn = insn.GetNextMachineInsn();
  if (nextInsn == nullptr) {
    return;
  }
  MOperator thisMop = insn.GetMachineOpcode();
  if (thisMop != MOP_xaddrrrs) {
    return;
  }
  MOperator nextMop = nextInsn->GetMachineOpcode();
  if (nextMop &&
      ((nextMop >= MOP_wldrsb && nextMop <= MOP_dldr) || (nextMop >= MOP_wstrb && nextMop <= MOP_dstr))) {
    /* Check if base register of nextInsn and the dest operand of insn are identical. */
    AArch64MemOperand *memOpnd = static_cast<AArch64MemOperand*>(nextInsn->GetMemOpnd());
    ASSERT(memOpnd != nullptr, "null ptr check");

    /* Only for AddrMode_B_OI addressing mode. */
    if (memOpnd->GetAddrMode() != AArch64MemOperand::kAddrModeBOi) {
      return;
    }

    /* Only for immediate is  0. */
    if (memOpnd->GetOffsetImmediate()->GetOffsetValue() != 0) {
      return;
    }

    /* Only for intact memory addressing. */
    if (!memOpnd->IsIntactIndexed()) {
      return;
    }

    auto &regOpnd = static_cast<RegOperand&>(insn.GetOperand(kInsnFirstOpnd));

    /* Check if dest operand of insn is idential with base register of nextInsn. */
    if (memOpnd->GetBaseRegister() != &regOpnd) {
      return;
    }

#ifdef USE_32BIT_REF
    if (nextInsn->IsAccessRefField() && nextInsn->GetOperand(kInsnFirstOpnd).GetSize() > k32BitSize) {
      return;
    }
#endif

    /* Check if x0 is used after ldr insn, and if it is in live-out. */
    if (IfOperandIsLiveAfterInsn(regOpnd, *nextInsn)) {
      return;
    }
    auto &lsl = static_cast<BitShiftOperand&>(insn.GetOperand(kInsnFourthOpnd));
    if (!CheckShiftValid(*memOpnd, lsl)) {
      return;
    }
    auto &newBaseOpnd = static_cast<RegOperand&>(insn.GetOperand(kInsnSecondOpnd));
    auto &newIndexOpnd = static_cast<RegOperand&>(insn.GetOperand(kInsnThirdOpnd));
    AArch64MemOperand &newMemOpnd =
        aarch64CGFunc->GetOrCreateMemOpnd(AArch64MemOperand::kAddrModeBOrX, memOpnd->GetSize(), &newBaseOpnd,
                                          &newIndexOpnd, lsl.GetShiftAmount(), false);
    nextInsn->SetOperand(kInsnSecondOpnd, newMemOpnd);
    bb.RemoveInsn(insn);
  }
}


void ComplexMemOperandLabelAArch64::Run(BB &bb, Insn &insn) {
  Insn *nextInsn = insn.GetNextMachineInsn();
  if (nextInsn == nullptr) {
    return;
  }
  MOperator thisMop = insn.GetMachineOpcode();
  if (thisMop != MOP_xldli) {
    return;
  }
  MOperator nextMop = nextInsn->GetMachineOpcode();
  if (nextMop != MOP_xvmovdr) {
    return;
  }
  auto &regOpnd = static_cast<RegOperand&>(insn.GetOperand(kInsnFirstOpnd));
  if (regOpnd.GetRegisterNumber() !=
      static_cast<RegOperand&>(nextInsn->GetOperand(kInsnSecondOpnd)).GetRegisterNumber()) {
    return;
  }

  /* Check if x0 is used after ldr insn, and if it is in live-out. */
  if (IfOperandIsLiveAfterInsn(regOpnd, *nextInsn)) {
    return;
  }

  Insn &newInsn = cgFunc.GetCG()->BuildInstruction<AArch64Insn>(MOP_dldli, nextInsn->GetOperand(kInsnFirstOpnd),
                                                                insn.GetOperand(kInsnSecondOpnd));
  bb.InsertInsnAfter(*nextInsn, newInsn);
  bb.RemoveInsn(insn);
  bb.RemoveInsn(*nextInsn);
}

/*
 * mov R0, vreg1 / R0      -> objDesignateInsn
 * add vreg2, vreg1, #imm  -> fieldDesignateInsn
 * mov R1, vreg2           -> fieldParamDefInsn
 * mov R2, vreg3           -> fieldValueDefInsn
 */
bool WriteFieldCallAArch64::WriteFieldCallOptPatternMatch(const Insn &writeFieldCallInsn, WriteRefFieldParam &param,
                                                          std::vector<Insn*> &paramDefInsns) {
  Insn *fieldValueDefInsn = writeFieldCallInsn.GetPreviousMachineInsn();
  if (fieldValueDefInsn == nullptr || fieldValueDefInsn->GetMachineOpcode() != MOP_xmovrr) {
    return false;
  }
  Operand &fieldValueDefInsnDestOpnd = fieldValueDefInsn->GetOperand(kInsnFirstOpnd);
  auto &fieldValueDefInsnDestReg = static_cast<RegOperand&>(fieldValueDefInsnDestOpnd);
  if (fieldValueDefInsnDestReg.GetRegisterNumber() != R2) {
    return false;
  }
  paramDefInsns.push_back(fieldValueDefInsn);
  param.fieldValue = &(fieldValueDefInsn->GetOperand(kInsnSecondOpnd));
  Insn *fieldParamDefInsn = fieldValueDefInsn->GetPreviousMachineInsn();
  if (fieldParamDefInsn == nullptr || fieldParamDefInsn->GetMachineOpcode() != MOP_xmovrr) {
    return false;
  }
  Operand &fieldParamDestOpnd = fieldParamDefInsn->GetOperand(kInsnFirstOpnd);
  auto &fieldParamDestReg = static_cast<RegOperand&>(fieldParamDestOpnd);
  if (fieldParamDestReg.GetRegisterNumber() != R1) {
    return false;
  }
  paramDefInsns.push_back(fieldParamDefInsn);
  Insn *fieldDesignateInsn = fieldParamDefInsn->GetPreviousMachineInsn();
  if (fieldDesignateInsn == nullptr || fieldDesignateInsn->GetMachineOpcode() != MOP_xaddrri12) {
    return false;
  }
  Operand &fieldParamDefSrcOpnd = fieldParamDefInsn->GetOperand(kInsnSecondOpnd);
  Operand &fieldDesignateDestOpnd = fieldDesignateInsn->GetOperand(kInsnFirstOpnd);
  if (!RegOperand::IsSameReg(fieldParamDefSrcOpnd, fieldDesignateDestOpnd)) {
    return false;
  }
  Operand &fieldDesignateBaseOpnd = fieldDesignateInsn->GetOperand(kInsnSecondOpnd);
  param.fieldBaseOpnd = &(static_cast<RegOperand&>(fieldDesignateBaseOpnd));
  auto &immOpnd = static_cast<AArch64ImmOperand&>(fieldDesignateInsn->GetOperand(kInsnThirdOpnd));
  param.fieldOffset = immOpnd.GetValue();
  paramDefInsns.push_back(fieldDesignateInsn);
  Insn *objDesignateInsn = fieldDesignateInsn->GetPreviousMachineInsn();
  if (objDesignateInsn == nullptr || objDesignateInsn->GetMachineOpcode() != MOP_xmovrr) {
    return false;
  }
  Operand &objDesignateDestOpnd = objDesignateInsn->GetOperand(kInsnFirstOpnd);
  auto &objDesignateDestReg = static_cast<RegOperand&>(objDesignateDestOpnd);
  if (objDesignateDestReg.GetRegisterNumber() != R0) {
    return false;
  }
  Operand &objDesignateSrcOpnd = objDesignateInsn->GetOperand(kInsnSecondOpnd);
  if (RegOperand::IsSameReg(objDesignateDestOpnd, objDesignateSrcOpnd) ||
      !RegOperand::IsSameReg(objDesignateSrcOpnd, fieldDesignateBaseOpnd)) {
    return false;
  }
  param.objOpnd = &(objDesignateInsn->GetOperand(kInsnSecondOpnd));
  paramDefInsns.push_back(objDesignateInsn);
  return true;
}

bool WriteFieldCallAArch64::IsWriteRefFieldCallInsn(const Insn &insn) {
  if (!insn.IsCall() || insn.IsIndirectCall()) {
    return false;
  }
  Operand *targetOpnd = insn.GetCallTargetOperand();
  ASSERT(targetOpnd != nullptr, "targetOpnd must not be nullptr");
  if (!targetOpnd->IsFuncNameOpnd()) {
    return false;
  }
  FuncNameOperand *target = static_cast<FuncNameOperand*>(targetOpnd);
  const MIRSymbol *funcSt = target->GetFunctionSymbol();
  ASSERT(funcSt->GetSKind() == kStFunc, "the kind of funcSt is unreasonable");
  const std::string &funcName = funcSt->GetName();
  return funcName == "MCC_WriteRefField" || funcName == "MCC_WriteVolatileField";
}

static bool MayThrowBetweenInsn(const Insn &prevCallInsn, const Insn &currCallInsn) {
  for (Insn *insn = prevCallInsn.GetNext(); insn != nullptr && insn != &currCallInsn; insn = insn->GetNext()) {
    if (insn->MayThrow()) {
      return true;
    }
  }
  return false;
}

void WriteFieldCallAArch64::Run(BB &bb, Insn &insn) {
  AArch64CGFunc *aarch64CGFunc = static_cast<AArch64CGFunc*>(&cgFunc);
  std::vector<Insn*> paramDefInsns;
  Insn *nextInsn = insn.GetNextMachineInsn();
  if (!IsWriteRefFieldCallInsn(insn)) {
    return;
  }
  if (!hasWriteFieldCall) {
    if (!WriteFieldCallOptPatternMatch(insn, firstCallParam, paramDefInsns)) {
      return;
    }
    prevCallInsn = &insn;
    hasWriteFieldCall = true;
    return;
  }
  WriteRefFieldParam currentCallParam;
  if (!WriteFieldCallOptPatternMatch(insn, currentCallParam, paramDefInsns)) {
    return;
  }
  if (prevCallInsn == nullptr || MayThrowBetweenInsn(*prevCallInsn, insn)) {
    return;
  }
  if (firstCallParam.objOpnd == nullptr || currentCallParam.objOpnd == nullptr ||
      currentCallParam.fieldBaseOpnd == nullptr) {
    return;
  }
  if (!RegOperand::IsSameReg(*firstCallParam.objOpnd, *currentCallParam.objOpnd)) {
    return;
  }
  MemOperand &addr =
      aarch64CGFunc->CreateMemOpnd(*currentCallParam.fieldBaseOpnd, currentCallParam.fieldOffset, k64BitSize);
  Insn &strInsn = cgFunc.GetCG()->BuildInstruction<AArch64Insn>(MOP_xstr, *currentCallParam.fieldValue, addr);
  strInsn.AppendComment("store reference field");
  strInsn.MarkAsAccessRefField(true);
  bb.InsertInsnAfter(insn, strInsn);
  for (Insn *paramDefInsn : paramDefInsns) {
    bb.RemoveInsn(*paramDefInsn);
  }
  bb.RemoveInsn(insn);
  prevCallInsn = &strInsn;
  nextInsn = strInsn.GetNextMachineInsn();
}

void RemoveDecRefAArch64::Run(BB &bb, Insn &insn) {
  if (insn.GetMachineOpcode() != MOP_xbl) {
    return;
  }
  auto &target = static_cast<FuncNameOperand&>(insn.GetOperand(kInsnFirstOpnd));
  if (target.GetName() != "MCC_DecRef_NaiveRCFast") {
    return;
  }
  Insn *insnMov = insn.GetPreviousMachineInsn();
  if (insnMov == nullptr) {
    return;
  }
  MOperator mopMov = insnMov->GetMachineOpcode();
  if ((mopMov != MOP_xmovrr && mopMov != MOP_xmovri64) ||
      static_cast<RegOperand&>(insnMov->GetOperand(kInsnFirstOpnd)).GetRegisterNumber() != R0) {
    return;
  }
  Operand &srcOpndOfMov = insnMov->GetOperand(kInsnSecondOpnd);
  if (!srcOpndOfMov.IsZeroRegister() &&
      !(srcOpndOfMov.IsImmediate() && static_cast<ImmOperand&>(srcOpndOfMov).GetValue() == 0)) {
    return;
  }
  bb.RemoveInsn(*insnMov);
  bb.RemoveInsn(insn);
  bb.SetKind(BB::kBBFallthru);
}

/*
 * Find 5 insn with certain OP code
 * 1 : MOP_xaddrri12
 * 2 : MOP_waddrrr
 * 3 : MOP_waddrri12
 * 4 : MOP_xsxtw64
 * 5 : MOP_xaddrrrs
 */
bool ComputationTreeAArch64::FindComputationTree(std::vector<Insn*> &optInsn, Insn &insn) {
  MOperator thisMop = insn.GetMachineOpcode();
  optInsn.clear();
  /* first */
  if (thisMop != MOP_xaddrri12) {
    return false;
  }
  optInsn.push_back(&insn);
  /* second */
  Insn *nextInsn1 = insn.GetNextMachineInsn();
  if (nextInsn1 == nullptr) {
    return false;
  }
  MOperator nextMop1 = nextInsn1->GetMachineOpcode();
  if (nextMop1 != MOP_waddrrr) {
    return false;
  }
  optInsn.push_back(nextInsn1);
  /* third */
  Insn *nextInsn2 = nextInsn1->GetNextMachineInsn();
  if (nextInsn2 == nullptr) {
    return false;
  }
  MOperator nextMop2 = nextInsn2->GetMachineOpcode();
  if (nextMop2 != MOP_waddrri12) {
    return false;
  }
  optInsn.push_back(nextInsn2);
  /* forth */
  Insn *nextInsn3 = nextInsn2->GetNextMachineInsn();
  if (nextInsn3 == nullptr) {
    return false;
  }
  MOperator nextMop3 = nextInsn3->GetMachineOpcode();
  if (nextMop3 != MOP_xsxtw64) {
    return false;
  }
  optInsn.push_back(nextInsn3);
  /* fifth */
  Insn *nextInsn4 = nextInsn3->GetNextMachineInsn();
  if (nextInsn4 == nullptr) {
    return false;
  }
  MOperator nextMop4 = nextInsn4->GetMachineOpcode();
  if (nextMop4 != MOP_xaddrrrs) {
    return false;
  }
  optInsn.push_back(nextInsn4);
  return true;
}

/*
 * Make sure the insn in opt_insn match the pattern as following:
 * add x1, x1, #16
 * add w2, w10, w10
 * add w2, w2, #1
 * sxtw x2, w2
 * add x1, x1, x2, LSL #3
 * bl MCC_LoadRefField_NaiveRCFast
 */
bool ComputationTreeAArch64::IsPatternMatch(const std::vector<Insn*> &optInsn) const {
  /* this speific pattern has exactly four insns */
  if (optInsn.size() <= 4) {
    ERR(kLncErr, "access opt_insn failed");
    return false;
  }
  int insnNum = 0;
  Insn *insn1 = optInsn[insnNum];
  Insn *insn2 = optInsn[++insnNum];
  Insn *insn3 = optInsn[++insnNum];
  Insn *insn4 = optInsn[++insnNum];
  Insn *insn5 = optInsn[++insnNum];
  ASSERT(insnNum == 4, "match pattern failed in AArch64Peep::PatternIsMatch");
  Insn *insn6 = insn5->GetNext();
  if (insn6 != nullptr && insn6->GetMachineOpcode() != MOP_xbl && insn6->GetMachineOpcode() != MOP_tail_call_opt_xbl) {
    return false;
  }
  CHECK_FATAL(insn6 != nullptr, "Insn null ptr check");
  auto &funcNameOpnd = static_cast<FuncNameOperand&>(insn6->GetOperand(kInsnFirstOpnd));
  if (&(insn1->GetOperand(kInsnFirstOpnd)) == &(insn5->GetOperand(kInsnSecondOpnd)) &&
      &(insn2->GetOperand(kInsnSecondOpnd)) == &(insn2->GetOperand(kInsnThirdOpnd)) &&
      &(insn2->GetOperand(kInsnFirstOpnd)) == &(insn3->GetOperand(kInsnSecondOpnd)) &&
      &(insn3->GetOperand(kInsnFirstOpnd)) == &(insn4->GetOperand(kInsnSecondOpnd)) &&
      &(insn4->GetOperand(kInsnFirstOpnd)) == &(insn5->GetOperand(kInsnThirdOpnd)) &&
      funcNameOpnd.GetName() == "MCC_LoadRefField_NaiveRCFast" &&
      static_cast<ImmOperand&>(insn1->GetOperand(kInsnThirdOpnd)).GetValue() == k16BitSize &&
      static_cast<ImmOperand&>(insn3->GetOperand(kInsnThirdOpnd)).GetValue() == 1) {
    return true;
  }
  return false;
}

void ComputationTreeAArch64::Run(BB &bb, Insn &insn) {
  std::vector<Insn*> optInsn;
  AArch64CGFunc *aarch64CGFunc = static_cast<AArch64CGFunc*>(&cgFunc);
  if (!insn.IsMachineInstruction()) {
    return;
  }
  /* found pattern */
  if (FindComputationTree(optInsn, insn) && IsPatternMatch(optInsn)) {
    Insn *sxtwInsn = optInsn[4]; // The pattern must has four insns.
    CHECK_FATAL(sxtwInsn->GetOperand(kInsnFourthOpnd).GetKind() == Operand::kOpdShift, "should not happened");
    auto &lsl = static_cast<BitShiftOperand&>(sxtwInsn->GetOperand(kInsnFourthOpnd));
    Operand *sxtw = nullptr;
    Operand *imm = nullptr;
    int32 lslBitLenth = 3;
    uint32 lslShiftAmountCaseA = 3;
    uint32 lslShiftAmountCaseB = 2;
    int32 oriAddEnd = 16;
    if (lsl.GetShiftAmount() == lslShiftAmountCaseA) {
      sxtw = &aarch64CGFunc->CreateExtendShiftOperand(ExtendShiftOperand::kSXTW,
                                                      lslShiftAmountCaseA + 1, lslBitLenth);
      imm = &aarch64CGFunc->CreateImmOperand(oriAddEnd + (1ULL << lslShiftAmountCaseA), kMaxImmVal12Bits, true);
    } else if (lsl.GetShiftAmount() == lslShiftAmountCaseB) {
      sxtw = &aarch64CGFunc->CreateExtendShiftOperand(ExtendShiftOperand::kSXTW,
                                                      lslShiftAmountCaseB + 1, lslBitLenth);
      imm = &aarch64CGFunc->CreateImmOperand(oriAddEnd + (1ULL << lslShiftAmountCaseB), kMaxImmVal12Bits, true);
    }
    Insn &newInsn = cgFunc.GetCG()->BuildInstruction<AArch64Insn>(MOP_xxwaddrrre,
                                                                  sxtwInsn->GetOperand(kInsnFirstOpnd),
                                                                  optInsn[0]->GetOperand(kInsnSecondOpnd),
                                                                  optInsn[1]->GetOperand(kInsnSecondOpnd), *sxtw);
    bb.ReplaceInsn(*sxtwInsn, newInsn);
    Insn &newAdd =
        cgFunc.GetCG()->BuildInstruction<AArch64Insn>(MOP_xaddrri12, sxtwInsn->GetOperand(kInsnFirstOpnd),
                                                      sxtwInsn->GetOperand(kInsnFirstOpnd), *imm);
    (void)bb.InsertInsnAfter(newInsn, newAdd);
    optInsn.clear();
  }
}

/*
 * We optimize the following pattern in this function:
 * and x1, x1, #imm (is n power of 2)
 * cbz/cbnz x1, .label
 * =>
 * and x1, x1, #imm (is n power of 2)
 * tbnz/tbz x1, #n, .label
 */
void OneHoleBranchesAArch64::Run(BB &bb, Insn &insn) {
  AArch64CGFunc *aarch64CGFunc = static_cast<AArch64CGFunc*>(&cgFunc);
  if (&insn != bb.GetLastInsn()) {
    return;
  }
  /* check cbz/cbnz insn */
  MOperator thisMop = insn.GetMachineOpcode();
  if (thisMop != MOP_wcbz && thisMop != MOP_wcbnz && thisMop != MOP_xcbz && thisMop != MOP_xcbnz) {
    return;
  }
  /* check and insn */
  Insn *prevInsn = insn.GetPreviousMachineInsn();
  if (prevInsn == nullptr) {
    return;
  }
  MOperator prevMop = prevInsn->GetMachineOpcode();
  if (prevMop != MOP_wandrri12 && prevMop != MOP_xandrri13) {
    return;
  }
  /* check opearnd of two insns */
  if (&(prevInsn->GetOperand(kInsnFirstOpnd)) != &(insn.GetOperand(kInsnFirstOpnd))) {
    return;
  }
  auto &imm = static_cast<ImmOperand&>(prevInsn->GetOperand(kInsnThirdOpnd));
  int n = logValueAtBase2(imm.GetValue());
  if (n < 0) {
    return;
  }

  /* replace insn */
  auto &label = static_cast<LabelOperand&>(insn.GetOperand(kInsnSecondOpnd));
  MOperator newOp = MOP_undef;
  switch (thisMop) {
    case MOP_wcbz:
      newOp = MOP_wtbz;
      break;
    case MOP_wcbnz:
      newOp = MOP_wtbnz;
      break;
    case MOP_xcbz:
      newOp = MOP_xtbz;
      break;
    case MOP_xcbnz:
      newOp = MOP_xtbnz;
      break;
    default:
      CHECK_FATAL(false, "can not touch here");
      break;
  }
  ImmOperand &oneHoleOpnd = aarch64CGFunc->CreateImmOperand(n, k8BitSize, false);
  (void)bb.InsertInsnAfter(insn, cgFunc.GetCG()->BuildInstruction<AArch64Insn>(
      newOp, prevInsn->GetOperand(kInsnSecondOpnd), oneHoleOpnd, label));
  bb.RemoveInsn(insn);
}

void ReplaceIncDecWithIncAArch64::Run(BB &bb, Insn &insn) {
  if (insn.GetMachineOpcode() != MOP_xbl) {
    return;
  }
  auto &target = static_cast<FuncNameOperand&>(insn.GetOperand(kInsnFirstOpnd));
  if (target.GetName() != "MCC_IncDecRef_NaiveRCFast") {
    return;
  }
  Insn *insnMov = insn.GetPreviousMachineInsn();
  if (insnMov == nullptr) {
    return;
  }
  MOperator mopMov = insnMov->GetMachineOpcode();
  if (mopMov != MOP_xmovrr) {
    return;
  }
  if (static_cast<RegOperand&>(insnMov->GetOperand(kInsnFirstOpnd)).GetRegisterNumber() != R1 ||
      !insnMov->GetOperand(kInsnSecondOpnd).IsZeroRegister()) {
    return;
  }
  std::string funcName = "MCC_IncRef_NaiveRCFast";
  GStrIdx strIdx = GlobalTables::GetStrTable().GetStrIdxFromName(funcName);
  MIRSymbol *st = GlobalTables::GetGsymTable().GetSymbolFromStrIdx(strIdx, true);
  if (st == nullptr) {
    LogInfo::MapleLogger() << "WARNING: Replace IncDec With Inc fail due to no MCC_IncRef_NaiveRCFast func\n";
    return;
  }
  bb.RemoveInsn(*insnMov);
  target.SetFunctionSymbol(*st);
}


void AndCmpBranchesToTbzAArch64::Run(BB &bb, Insn &insn) {
  AArch64CGFunc *aarch64CGFunc = static_cast<AArch64CGFunc*>(&cgFunc);
  if (&insn != bb.GetLastInsn()) {
    return;
  }
  MOperator mopB = insn.GetMachineOpcode();
  if (mopB != MOP_beq && mopB != MOP_bne) {
    return;
  }
  auto &label = static_cast<LabelOperand&>(insn.GetOperand(kInsnSecondOpnd));
  /* get the instruction before bne/beq, expects its type is cmp. */
  Insn *prevInsn = insn.GetPreviousMachineInsn();
  if (prevInsn == nullptr) {
    return;
  }
  MOperator prevMop = prevInsn->GetMachineOpcode();
  if (prevMop != MOP_wcmpri && prevMop != MOP_xcmpri) {
    return;
  }

  /* get the instruction before "cmp", expect its type is "and". */
  Insn *prevPrevInsn = prevInsn->GetPreviousMachineInsn();
  if (prevPrevInsn == nullptr) {
    return;
  }
  MOperator mopAnd = prevPrevInsn->GetMachineOpcode();
  if (mopAnd != MOP_wandrri12 && mopAnd != MOP_xandrri13) {
    return;
  }

  /*
   * check operand
   *
   * the real register of "cmp" and "and" must be the same.
   */
  if (&(prevInsn->GetOperand(kInsnSecondOpnd)) != &(prevPrevInsn->GetOperand(kInsnFirstOpnd))) {
    return;
  }

  int opndIdx = 2;
  if (!prevPrevInsn->GetOperand(opndIdx).IsIntImmediate() || !prevInsn->GetOperand(opndIdx).IsIntImmediate()) {
    return;
  }
  auto &immAnd = static_cast<ImmOperand&>(prevPrevInsn->GetOperand(opndIdx));
  auto &immCmp = static_cast<ImmOperand&>(prevInsn->GetOperand(opndIdx));
  if (immCmp.GetValue() == 0) {
    int n = logValueAtBase2(immAnd.GetValue());
    if (n < 0) {
      return;
    }
    /* judge whether the flag_reg and "w0" is live later. */
    auto &flagReg = static_cast<RegOperand&>(prevInsn->GetOperand(kInsnFirstOpnd));
    auto &cmpReg = static_cast<RegOperand&>(prevInsn->GetOperand(kInsnSecondOpnd));
    if (FindRegLiveOut(flagReg, *prevInsn->GetBB()) || FindRegLiveOut(cmpReg, *prevInsn->GetBB())) {
      return;
    }
    MOperator mopNew = MOP_undef;
    switch (mopB) {
      case MOP_beq:
        if (mopAnd == MOP_wandrri12) {
          mopNew = MOP_wtbz;
        } else if (mopAnd == MOP_xandrri13) {
          mopNew = MOP_xtbz;
        }
        break;
      case MOP_bne:
        if (mopAnd == MOP_wandrri12) {
          mopNew = MOP_wtbnz;
        } else if (mopAnd == MOP_xandrri13) {
          mopNew = MOP_xtbnz;
        }
        break;
      default:
        CHECK_FATAL(false, "expects beq or bne insn");
        break;
    }
    ImmOperand &newImm = aarch64CGFunc->CreateImmOperand(n, k8BitSize, false);
    (void)bb.InsertInsnAfter(insn, cgFunc.GetCG()->BuildInstruction<AArch64Insn>(mopNew,
        prevPrevInsn->GetOperand(kInsnSecondOpnd), newImm, label));
    bb.RemoveInsn(insn);
    bb.RemoveInsn(*prevInsn);
    bb.RemoveInsn(*prevPrevInsn);
  } else {
    int n = logValueAtBase2(immAnd.GetValue());
    int m = logValueAtBase2(immCmp.GetValue());
    if (n < 0 || m < 0 || n != m) {
      return;
    }
    /* judge whether the flag_reg and "w0" is live later. */
    auto &flagReg = static_cast<RegOperand&>(prevInsn->GetOperand(kInsnFirstOpnd));
    auto &cmpReg = static_cast<RegOperand&>(prevInsn->GetOperand(kInsnSecondOpnd));
    if (FindRegLiveOut(flagReg, *prevInsn->GetBB()) || FindRegLiveOut(cmpReg, *prevInsn->GetBB())) {
      return;
    }
    MOperator mopNew = MOP_undef;
    switch (mopB) {
      case MOP_beq:
        if (mopAnd == MOP_wandrri12) {
          mopNew = MOP_wtbnz;
        } else if (mopAnd == MOP_xandrri13) {
          mopNew = MOP_xtbnz;
        }
        break;
      case MOP_bne:
        if (mopAnd == MOP_wandrri12) {
          mopNew = MOP_wtbz;
        } else if (mopAnd == MOP_xandrri13) {
          mopNew = MOP_xtbz;
        }
        break;
      default:
        CHECK_FATAL(false, "expects beq or bne insn");
        break;
    }
    ImmOperand &newImm = aarch64CGFunc->CreateImmOperand(n, k8BitSize, false);
    (void)bb.InsertInsnAfter(insn, cgFunc.GetCG()->BuildInstruction<AArch64Insn>(mopNew,
        prevPrevInsn->GetOperand(kInsnSecondOpnd), newImm, label));
    bb.RemoveInsn(insn);
    bb.RemoveInsn(*prevInsn);
    bb.RemoveInsn(*prevPrevInsn);
  }
}
}  /* namespace maplebe */
