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
#include "aarch64_offset_adjust.h"
#include "aarch64_cgfunc.h"

namespace maplebe {
void AArch64FPLROffsetAdjustment::Run() {
  AdjustmentOffsetForFPLR();
}

void AArch64FPLROffsetAdjustment::AdjustmentOffsetForOpnd(Insn &insn, AArch64CGFunc &aarchCGFunc) {
  uint32 opndNum = insn.GetOperandSize();
  MemLayout *memLayout = aarchCGFunc.GetMemlayout();
  for (uint32 i = 0; i < opndNum; ++i) {
    Operand &opnd = insn.GetOperand(i);
    if (opnd.IsRegister()) {
      auto &regOpnd = static_cast<RegOperand&>(opnd);
      if (regOpnd.IsOfVary()) {
        insn.SetOperand(i, aarchCGFunc.GetOrCreateStackBaseRegOperand());
      }
    } else if (opnd.IsMemoryAccessOperand()) {
      auto &memOpnd = static_cast<AArch64MemOperand&>(opnd);
      if (((memOpnd.GetAddrMode() == AArch64MemOperand::kAddrModeBOi) ||
           (memOpnd.GetAddrMode() == AArch64MemOperand::kAddrModeBOrX)) &&
          memOpnd.GetBaseRegister() != nullptr && memOpnd.GetBaseRegister()->IsOfVary()) {
        memOpnd.SetBaseRegister(static_cast<AArch64RegOperand&>(aarchCGFunc.GetOrCreateStackBaseRegOperand()));
      }
      if ((memOpnd.GetAddrMode() != AArch64MemOperand::kAddrModeBOi) || !memOpnd.IsIntactIndexed()) {
        continue;
      }
      AArch64OfstOperand *ofstOpnd = memOpnd.GetOffsetImmediate();
      if (ofstOpnd == nullptr) {
        continue;
      }
      if (ofstOpnd->GetVary() == kUnAdjustVary) {
        ofstOpnd->AdjustOffset(static_cast<AArch64MemLayout*>(memLayout)->RealStackFrameSize() -
                               memLayout->SizeOfArgsToStackPass());
        ofstOpnd->SetVary(kAdjustVary);
      }
      if (ofstOpnd->GetVary() == kAdjustVary) {
        if (aarchCGFunc.IsImmediateOffsetOutOfRange(memOpnd, memOpnd.GetSize())) {
          AArch64MemOperand &newMemOpnd = aarchCGFunc.SplitOffsetWithAddInstruction(
              memOpnd, memOpnd.GetSize(), static_cast<AArch64reg>(R17), false, &insn);
          insn.SetOperand(i, newMemOpnd);
        }
      }
    } else if (opnd.IsIntImmediate()) {
      auto &immOpnd = static_cast<ImmOperand&>(opnd);
      if (immOpnd.GetVary() == kUnAdjustVary) {
        immOpnd.Add(static_cast<AArch64MemLayout*>(memLayout)->RealStackFrameSize() -
                     memLayout->SizeOfArgsToStackPass());
      }
      immOpnd.SetVary(kAdjustVary);
    }
  }
}

void AArch64FPLROffsetAdjustment::AdjustmentOffsetForFPLR() {
  AArch64CGFunc *aarchCGFunc = static_cast<AArch64CGFunc*>(cgFunc);
  FOR_ALL_BB(bb, aarchCGFunc) {
    FOR_BB_INSNS(insn, bb) {
      if (!insn->IsMachineInstruction()) {
        continue;
      }
      AdjustmentOffsetForOpnd(*insn, *aarchCGFunc);
    }
  }

#undef STKLAY_DBUG
#ifdef STKLAY_DBUG
  AArch64MemLayout *aarch64memlayout = static_cast<AArch64MemLayout*>(cgFunc->GetMemlayout());
  LogInfo::MapleLogger() << "stkpass: " << aarch64memlayout->GetSegArgsStkpass().size << "\n";
  LogInfo::MapleLogger() << "local: " << aarch64memlayout->GetSizeOfLocals() << "\n";
  LogInfo::MapleLogger() << "ref local: " << aarch64memlayout->GetSizeOfRefLocals() << "\n";
  LogInfo::MapleLogger() << "regpass: " << aarch64memlayout->GetSegArgsRegPassed().size << "\n";
  LogInfo::MapleLogger() << "regspill: " << aarch64memlayout->GetSizeOfSpillReg() << "\n";
  LogInfo::MapleLogger() << "calleesave: " << SizeOfCalleeSaved() << "\n";

#endif
}
} /* namespace maplebe */
