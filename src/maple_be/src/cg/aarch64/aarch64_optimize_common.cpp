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
#include "aarch64_optimize_common.h"
#include "aarch64_isa.h"
#include "aarch64_cgfunc.h"
#include "cgbb.h"

namespace maplebe {
namespace {
constexpr int kTbxTargetIdx = 2;
};

MOperator AArch64InsnVisitor::FlipConditionOp(MOperator originalOp, int &targetIdx) {
  targetIdx = 1;
  switch (originalOp) {
    case AArch64MOP_t::MOP_beq:
      return AArch64MOP_t::MOP_bne;
    case AArch64MOP_t::MOP_bge:
      return AArch64MOP_t::MOP_blt;
    case AArch64MOP_t::MOP_bgt:
      return AArch64MOP_t::MOP_ble;
    case AArch64MOP_t::MOP_bhi:
      return AArch64MOP_t::MOP_bls;
    case AArch64MOP_t::MOP_bhs:
      return AArch64MOP_t::MOP_blo;
    case AArch64MOP_t::MOP_ble:
      return AArch64MOP_t::MOP_bgt;
    case AArch64MOP_t::MOP_blo:
      return AArch64MOP_t::MOP_bhs;
    case AArch64MOP_t::MOP_bls:
      return AArch64MOP_t::MOP_bhi;
    case AArch64MOP_t::MOP_blt:
      return AArch64MOP_t::MOP_bge;
    case AArch64MOP_t::MOP_bne:
      return AArch64MOP_t::MOP_beq;
    case AArch64MOP_t::MOP_xcbnz:
      return AArch64MOP_t::MOP_xcbz;
    case AArch64MOP_t::MOP_wcbnz:
      return AArch64MOP_t::MOP_wcbz;
    case AArch64MOP_t::MOP_xcbz:
      return AArch64MOP_t::MOP_xcbnz;
    case AArch64MOP_t::MOP_wcbz:
      return AArch64MOP_t::MOP_wcbnz;
    case AArch64MOP_t::MOP_wtbnz:
      targetIdx = kTbxTargetIdx;
      return AArch64MOP_t::MOP_wtbz;
    case AArch64MOP_t::MOP_wtbz:
      targetIdx = kTbxTargetIdx;
      return AArch64MOP_t::MOP_wtbnz;
    case AArch64MOP_t::MOP_xtbnz:
      targetIdx = kTbxTargetIdx;
      return AArch64MOP_t::MOP_xtbz;
    case AArch64MOP_t::MOP_xtbz:
      targetIdx = kTbxTargetIdx;
      return AArch64MOP_t::MOP_xtbnz;
    default:
      break;
  }
  return AArch64MOP_t::MOP_undef;
}

void AArch64InsnVisitor::ModifyJumpTarget(Operand &targetOperand, BB &bb) {
  bb.GetLastInsn()->SetOperand(GetJumpTargetIdx(*(bb.GetLastInsn())), targetOperand);
}

void AArch64InsnVisitor::ModifyJumpTarget(maple::LabelIdx targetLabel, BB &bb) {
  ModifyJumpTarget(static_cast<AArch64CGFunc*>(GetCGFunc())->GetOrCreateLabelOperand(targetLabel), bb);
}

void AArch64InsnVisitor::ModifyJumpTarget(BB &newTarget, BB &bb) {
  ModifyJumpTarget(newTarget.GetLastInsn()->GetOperand(GetJumpTargetIdx(*(newTarget.GetLastInsn()))), bb);
}

Insn *AArch64InsnVisitor::CloneInsn(Insn &originalInsn) {
  MemPool *memPool = const_cast<MemPool*>(CG::GetCurCGFunc()->GetMemoryPool());
  if (originalInsn.IsTargetInsn()) {
    return memPool->Clone<AArch64Insn>(*static_cast<AArch64Insn*>(&originalInsn));
  } else if (originalInsn.IsCfiInsn()) {
    return memPool->Clone<cfi::CfiInsn>(*static_cast<cfi::CfiInsn*>(&originalInsn));
  }
  CHECK_FATAL(false, "Cannot clone");
  return nullptr;
}

/*
 * Precondition: The given insn is a jump instruction.
 * Get the jump target label operand index from the given instruction.
 * Note: MOP_xbr is a jump instruction, but the target is unknown at compile time,
 * because a register instead of label. So we don't take it as a branching instruction.
 */
int AArch64InsnVisitor::GetJumpTargetIdx(const Insn &insn) const {
  MOperator mOp = insn.GetMachineOpcode();
  switch (mOp) {
    /* unconditional jump */
    case MOP_xuncond: {
      return 0;
    }
    /* conditional jump */
    case MOP_bmi:
    case MOP_bvc:
    case MOP_bls:
    case MOP_blt:
    case MOP_ble:
    case MOP_blo:
    case MOP_beq:
    case MOP_bpl:
    case MOP_bhs:
    case MOP_bvs:
    case MOP_bhi:
    case MOP_bgt:
    case MOP_bge:
    case MOP_bne:
    case MOP_wcbz:
    case MOP_xcbz:
    case MOP_wcbnz:
    case MOP_xcbnz: {
      return 1;
    }
    case MOP_wtbz:
    case MOP_xtbz:
    case MOP_wtbnz:
    case MOP_xtbnz: {
      return kTbxTargetIdx;
    }
    default:
      CHECK_FATAL(false, "Not a jump insn");
  }
  return 0;
}

/*
 * Precondition: The given insn is a jump instruction.
 * Get the jump target label from the given instruction.
 * Note: MOP_xbr is a branching instruction, but the target is unknown at compile time,
 * because a register instead of label. So we don't take it as a branching instruction.
 */
LabelIdx AArch64InsnVisitor::GetJumpLabel(const Insn &insn) const {
  int operandIdx = GetJumpTargetIdx(insn);
  if (insn.GetOperand(operandIdx).IsLabelOpnd()) {
    return static_cast<LabelOperand&>(insn.GetOperand(operandIdx)).GetLabelIndex();
  }
  ASSERT(false, "Operand is not label");
  return 0;
}

bool AArch64InsnVisitor::IsCompareInsn(const Insn &insn) const {
  switch (insn.GetMachineOpcode()) {
    case MOP_wcmpri:
    case MOP_wcmprr:
    case MOP_xcmpri:
    case MOP_xcmprr:
    case MOP_hcmperi:
    case MOP_hcmperr:
    case MOP_scmperi:
    case MOP_scmperr:
    case MOP_dcmperi:
    case MOP_dcmperr:
    case MOP_hcmpqri:
    case MOP_hcmpqrr:
    case MOP_scmpqri:
    case MOP_scmpqrr:
    case MOP_dcmpqri:
    case MOP_dcmpqrr:
    case MOP_wcmnri:
    case MOP_wcmnrr:
    case MOP_xcmnri:
    case MOP_xcmnrr:
      return true;
    default:
      return false;
  }
}

bool AArch64InsnVisitor::IsCompareAndBranchInsn(const Insn &insn) const {
  switch (insn.GetMachineOpcode()) {
    case MOP_wcbnz:
    case MOP_xcbnz:
    case MOP_wcbz:
    case MOP_xcbz:
      return true;
    default:
      return false;
  }
}

RegOperand *AArch64InsnVisitor::CreateVregFromReg(const RegOperand &pReg) {
  return &static_cast<AArch64CGFunc*>(GetCGFunc())->CreateRegisterOperandOfType(
      pReg.GetRegisterType(), pReg.GetSize() / k8BitSize);
}
}  /* namespace maplebe */
