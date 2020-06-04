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
#include "aarch64_fixshortbranch.h"
#include "cg.h"
#include "mpl_logging.h"
#include "common_utils.h"

namespace maplebe {
/*
 * Check the distance between the first insn of BB with the lable(targ_labidx)
 * and the insn with targ_id. If the distance greater than kShortBRDistance
 * return false.
 */
bool AArch64FixShortBranch::DistanceCheck(const BB &bb, LabelIdx targLabIdx, uint32 targId) {
  for (auto *tBB : bb.GetSuccs()) {
    if (tBB->GetLabIdx() != targLabIdx) {
      continue;
    }
    Insn *tInsn = tBB->GetFirstInsn();
    while (tInsn == nullptr || !tInsn->IsMachineInstruction()) {
      if (tInsn == nullptr) {
        tBB = tBB->GetNext();
        tInsn = tBB->GetFirstInsn();
      } else {
        tInsn = tInsn->GetNext();
      }
    }
    uint32 tmp = (tInsn->GetId() > targId) ? (tInsn->GetId() - targId) : (targId - tInsn->GetId());
    return (tmp < kShortBRDistance);
  }
  CHECK_FATAL(false, "CFG error");
}

void AArch64FixShortBranch::SetInsnId(){
  uint32 i = 0;
  AArch64CGFunc *aarch64CGFunc = static_cast<AArch64CGFunc*>(cgFunc);
  FOR_ALL_BB(bb, aarch64CGFunc) {
    FOR_BB_INSNS(insn, bb) {
      if (!insn->IsMachineInstruction()) {
        continue;
      }
      i += insn->GetAtomicNum();
      insn->SetId(i);
      if (insn->GetMachineOpcode() == MOP_adrp_ldr && CGOptions::IsLazyBinding() && !cgFunc->GetCG()->IsLibcore()) {
        /* For 1 additional EmitLazyBindingRoutine in lazybinding
         * see function AArch64Insn::Emit in file aarch64_insn.cpp
         */
        ++i;
      }
    }
  }
}

/*
 * TBZ/TBNZ instruction is generated under -O2, these branch instructions only have a range of +/-32KB.
 * If the branch target is not reachable, we split tbz/tbnz into combination of ubfx and cbz/cbnz, which
 * will clobber one extra register. With LSRA under -O2, we can use one of the reserved registers R16 for
 * that purpose. To save compile time, we do this change when there are more than 32KB / 4 instructions
 * in the function.
 */
void AArch64FixShortBranch::FixShortBranches() {
  AArch64CGFunc *aarch64CGFunc = static_cast<AArch64CGFunc*>(cgFunc);
  SetInsnId();
  FOR_ALL_BB(bb, aarch64CGFunc) {
    /* Do a backward scan searching for short branches */
    FOR_BB_INSNS_REV(insn, bb) {
      if (!insn->IsMachineInstruction()) {
        continue;
      }
      MOperator thisMop = insn->GetMachineOpcode();
      if (thisMop != MOP_wtbz && thisMop != MOP_wtbnz && thisMop != MOP_xtbz && thisMop != MOP_xtbnz) {
        continue;
      }
      LabelOperand &label = static_cast<LabelOperand&>(insn->GetOperand(kInsnThirdOpnd));
      /*  should not be commented out after bug fix */
      if (DistanceCheck(*bb, label.GetLabelIndex(), insn->GetId())) {
        continue;
      }
      auto &reg = static_cast<AArch64RegOperand&>(insn->GetOperand(kInsnFirstOpnd));
      ImmOperand &bitSize = aarch64CGFunc->CreateImmOperand(1, k8BitSize, false);
      auto &bitPos = static_cast<ImmOperand&>(insn->GetOperand(kInsnSecondOpnd));
      MOperator ubfxOp = MOP_undef;
      MOperator cbOp = MOP_undef;
      switch (thisMop) {
        case MOP_wtbz:
          ubfxOp = MOP_wubfxrri5i5;
          cbOp = MOP_wcbz;
          break;
        case MOP_wtbnz:
          ubfxOp = MOP_wubfxrri5i5;
          cbOp = MOP_wcbnz;
          break;
        case MOP_xtbz:
          ubfxOp = MOP_xubfxrri6i6;
          cbOp = MOP_xcbz;
          break;
        case MOP_xtbnz:
          ubfxOp = MOP_xubfxrri6i6;
          cbOp = MOP_xcbnz;
          break;
        default:
          break;
      }
      AArch64RegOperand &tmp =
          aarch64CGFunc->GetOrCreatePhysicalRegisterOperand(R16, (ubfxOp == MOP_wubfxrri5i5) ? k32BitSize : k64BitSize,
                                                            kRegTyInt);
      (void)bb->InsertInsnAfter(*insn, cg->BuildInstruction<AArch64Insn>(cbOp, tmp, label));
      (void)bb->InsertInsnAfter(*insn, cg->BuildInstruction<AArch64Insn>(ubfxOp, tmp, reg, bitPos, bitSize));
      bb->RemoveInsn(*insn);
      break;
    }
  }
}

AnalysisResult *CgFixShortBranch::Run(CGFunc *cgFunc, CgFuncResultMgr *cgFuncResultMgr) {
  (void)cgFuncResultMgr;
  ASSERT(cgFunc != nullptr, "nullptr check");
  MemPool *memPool = memPoolCtrler.NewMemPool("fixShortBranches");
  auto *fixShortBranch = memPool->New<AArch64FixShortBranch>(cgFunc);
  CHECK_FATAL(fixShortBranch != nullptr, "AArch64FixShortBranch instance create failure");
  fixShortBranch->FixShortBranches();
  memPoolCtrler.DeleteMemPool(memPool);
  return nullptr;
}
}  /* namespace maplebe */

