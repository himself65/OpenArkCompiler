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
#include "aarch64_live.h"
#include "aarch64_cg.h"

namespace maplebe {
void AArch64LiveAnalysis::InitEhDefine(BB &bb) {
  AArch64CGFunc *aarchCGFunc = static_cast<AArch64CGFunc*>(cgFunc);

  /* Insert MOP_pseudo_eh_def_x R1. */
  RegOperand &regR1 = aarchCGFunc->GetOrCreatePhysicalRegisterOperand(R1, k64BitSize, kRegTyInt);
  Insn &pseudoInsn1 = cgFunc->GetCG()->BuildInstruction<AArch64Insn>(MOP_pseudo_eh_def_x, regR1);
  bb.InsertInsnBegin(pseudoInsn1);

  /* Insert MOP_pseudo_eh_def_x R0. */
  RegOperand &regR0 = aarchCGFunc->GetOrCreatePhysicalRegisterOperand(R0, k64BitSize, kRegTyInt);
  Insn &pseudoInsn2 = cgFunc->GetCG()->BuildInstruction<AArch64Insn>(MOP_pseudo_eh_def_x, regR0);
  bb.InsertInsnBegin(pseudoInsn2);
}

/* build use and def sets of each BB according to the type of regOpnd. */
void AArch64LiveAnalysis::CollectLiveInfo(BB &bb, const Operand &opnd, bool isDef, bool isUse) const {
  if (!opnd.IsRegister()) {
    return;
  }
  const RegOperand &regOpnd = static_cast<const RegOperand&>(opnd);
  regno_t regNO = regOpnd.GetRegisterNumber();
  RegType regType = regOpnd.GetRegisterType();
  if (regType == kRegTyVary) {
    return;
  }
  if (isDef) {
    bb.SetDefBit(regNO);
    if (!isUse) {
      bb.UseResetBit(regNO);
    }
  }
  if (isUse) {
    bb.SetUseBit(regNO);
    bb.DefResetBit(regNO);
  }
}

/*
 * entry of get def/use of bb.
 * getting the def or use info of each regopnd as parameters of CollectLiveInfo().
*/
void AArch64LiveAnalysis::GetBBDefUse(BB &bb) {
  if (bb.GetKind() == BB::kBBReturn) {
    GenerateReturnBBDefUse(bb);
  }
  if (bb.IsEmpty()) {
    return;
  }
  bb.DefResetAllBit();
  bb.UseResetAllBit();

  FOR_BB_INSNS_REV(insn, &bb) {
    if (!insn->IsMachineInstruction()) {
      continue;
    }

    const AArch64MD *md = &AArch64CG::kMd[static_cast<AArch64Insn*>(insn)->GetMachineOpcode()];
    if (insn->IsCall()) {
      ProcessCallInsnParam(bb);
    }
    uint32 opndNum = insn->GetOperandSize();
    for (uint32 i = 0; i < opndNum; ++i) {
      Operand &opnd = insn->GetOperand(i);
      AArch64OpndProp *regProp = static_cast<AArch64OpndProp*>(md->operand[i]);
      bool isDef = regProp->IsRegDef();
      bool isUse = regProp->IsRegUse();
      if (opnd.IsList()) {
        ProcessListOpnd(bb, opnd);
      } else if (opnd.IsMemoryAccessOperand()) {
        ProcessMemOpnd(bb, opnd);
      } else if (opnd.IsConditionCode()) {
        ProcessCondOpnd(bb);
      } else {
        CollectLiveInfo(bb, opnd, isDef, isUse);
      }
    }
  }
}

bool AArch64LiveAnalysis::CleanupBBIgnoreReg(uint32 reg) {
  uint32 regNO = reg + R0;
  if (regNO < R8 || (R29 <= regNO && regNO <= RZR)) {
    return true;
  }
  return false;
}

void AArch64LiveAnalysis::GenerateReturnBBDefUse(BB &bb) const {
  PrimType returnType = cgFunc->GetFunction().GetReturnType()->GetPrimType();
  AArch64CGFunc *aarchCGFunc = static_cast<AArch64CGFunc*>(cgFunc);
  if (IsPrimitiveFloat(returnType)) {
    Operand &phyOpnd =
        aarchCGFunc->GetOrCreatePhysicalRegisterOperand(static_cast<AArch64reg>(V0), k64BitSize, kRegTyFloat);
    CollectLiveInfo(bb, phyOpnd, false, true);
  } else if (IsPrimitiveInteger(returnType)) {
    Operand &phyOpnd =
        aarchCGFunc->GetOrCreatePhysicalRegisterOperand(static_cast<AArch64reg>(R0), k64BitSize, kRegTyInt);
    CollectLiveInfo(bb, phyOpnd, false, true);
  }
}

void AArch64LiveAnalysis::ProcessCallInsnParam(BB &bb) const {
  /* R0 ~ R7（R0 + 0  ~ R0 + 7） and V0 ~ V7 (V0 + 0 ~ V0 + 7) is parameter register */
  AArch64CGFunc *aarchCGFunc = static_cast<AArch64CGFunc*>(cgFunc);
  for (uint32 i = 0; i < 8; ++i) {
    Operand &phyOpndR =
        aarchCGFunc->GetOrCreatePhysicalRegisterOperand(static_cast<AArch64reg>(R0 + i), k64BitSize, kRegTyInt);
    CollectLiveInfo(bb, phyOpndR, true, false);
    Operand &phyOpndV =
        aarchCGFunc->GetOrCreatePhysicalRegisterOperand(static_cast<AArch64reg>(V0 + i), k64BitSize, kRegTyFloat);
    CollectLiveInfo(bb, phyOpndV, true, false);
  }
}

void AArch64LiveAnalysis::ProcessListOpnd(BB &bb, Operand &opnd) const {
  ListOperand &listOpnd = static_cast<ListOperand&>(opnd);
  for (auto op : listOpnd.GetOperands()) {
    CollectLiveInfo(bb, *op, false, true);
  }
}

void AArch64LiveAnalysis::ProcessMemOpnd(BB &bb, Operand &opnd) const {
  MemOperand &memOpnd = static_cast<MemOperand&>(opnd);
  Operand *base = memOpnd.GetBaseRegister();
  Operand *offset = memOpnd.GetIndexRegister();
  if (base != nullptr) {
    CollectLiveInfo(bb, *base, false, true);
  }
  if (offset != nullptr) {
    CollectLiveInfo(bb, *offset, false, true);
  }
}

void AArch64LiveAnalysis::ProcessCondOpnd(BB &bb) const {
  Operand &rflag = cgFunc->GetOrCreateRflag();
  CollectLiveInfo(bb, rflag, false, true);
}
}  /* namespace maplebe */
