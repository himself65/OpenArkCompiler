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
#include "aarch64_strldr.h"
#include "aarch64_reaching.h"
#include "aarch64_cgfunc.h"
#include "common_utils.h"

namespace maplebe {
using namespace maple;

static MOperator SelectMovMop(bool isFloatOrSIMD, bool is64Bit) {
  return isFloatOrSIMD ? (is64Bit ? MOP_xvmovd : MOP_xvmovs)
                       : (is64Bit ? MOP_xmovrr : MOP_wmovrr);
}

void AArch64StoreLoadOpt::Run() {
  /* if the number of BB is too large, don't optimize. */
  if (cgFunc.NumBBs() > kMaxBBNum || cgFunc.GetRD()->GetMaxInsnNO() > kMaxInsnNum) {
    return;
  }
  DoStoreLoadOpt();
}

/*
 * Transfer: store x100, [MEM]
 *           ... // May exist branches.
 *           load  x200, [MEM]
 *       ==>
 *        OPT_VERSION_STR_LIVE:
 *           store x100, [MEM]
 *           ... // May exist branches. if x100 not dead here.
 *           mov   x200, x100
 *       OPT_VERSION_STR_DIE:
 *           store x100, [MEM]
 *           mov x9000(new reg), x100
 *           ... // May exist branches. if x100 dead here.
 *           mov   x200, x9000
 *  Params:
 *    strInsn: indicate store insn.
 *    strSrcIdx: index of source register operand of store insn. (x100 in this example)
 *    memSeq: represent first memOpreand or second memOperand
 *    memUseInsnSet: insns using memOperand
 */
void AArch64StoreLoadOpt::DoLoadToMoveTransfer(Insn &strInsn, short strSrcIdx,
                                               short memSeq, const InsnSet &memUseInsnSet) {
  /* stp instruction need two registers, str only need one register */
  ASSERT(strSrcIdx < kDivide2, "CG internal error.");
  /* Find x100's definition insn. */
  InsnSet regDefInsnSet = cgFunc.GetRD()->FindDefForRegOpnd(strInsn, strSrcIdx);
  ASSERT(!regDefInsnSet.empty(), "RegOperand is used before defined");
  if (regDefInsnSet.size() != 1) {
    return;
  }
  for (auto *ldrInsn : memUseInsnSet) {
    if (!ldrInsn->IsLoad() || (ldrInsn->GetResultNum() > 1) || ldrInsn->GetBB()->IsCleanup()) {
      continue;
    }

    /* ldr x200, [mem], mem index is 1, x200 index is 0 */
    InsnSet memDefInsnSet = cgFunc.GetRD()->FindDefForMemOpnd(*ldrInsn, kInsnSecondOpnd);
    ASSERT(!memDefInsnSet.empty(), "load insn should have definitions.");
    /* If load has multiple definition, continue. */
    if (memDefInsnSet.size() > 1) {
      continue;
    }

    Operand &resOpnd = ldrInsn->GetOperand(kInsnFirstOpnd);
    Operand &srcOpnd = strInsn.GetOperand(strSrcIdx);
    ASSERT(resOpnd.GetSize() == srcOpnd.GetSize(), "For stack location, the size of src and dst should be same.");

    auto &resRegOpnd = static_cast<RegOperand&>(resOpnd);
    auto &srcRegOpnd = static_cast<RegOperand&>(srcOpnd);
    if (resRegOpnd.GetRegisterType() != srcRegOpnd.GetRegisterType()) {
      continue;
    }

    /* Check if use operand of store is live at load insn. */
    if (cgFunc.GetRD()->RegIsLiveBetweenInsn(srcRegOpnd.GetRegisterNumber(), strInsn, *ldrInsn)) {
      GenerateMoveLiveInsn(resRegOpnd, srcRegOpnd, *ldrInsn, strInsn, memSeq);
    } else {
      GenerateMoveDeadInsn(resRegOpnd, srcRegOpnd, *ldrInsn, strInsn, memSeq);
    }

    if (CG_DEBUG_FUNC(&cgFunc)) {
      LogInfo::MapleLogger() << "Do store-load optimization 1: str version";
      LogInfo::MapleLogger() << cgFunc.GetName() << '\n';
      LogInfo::MapleLogger() << "Store insn: ";
      strInsn.Dump();
      LogInfo::MapleLogger() << "Load insn: ";
      ldrInsn->Dump();
    }
  }
}

void AArch64StoreLoadOpt::GenerateMoveLiveInsn(RegOperand &resRegOpnd, RegOperand &srcRegOpnd,
                                               Insn &ldrInsn, Insn &strInsn, short memSeq) {
  MOperator movMop = SelectMovMop(resRegOpnd.IsOfFloatOrSIMDClass(), resRegOpnd.GetSize() == k64BitSize);
  Insn *movInsn = nullptr;
  if (str2MovMap[&strInsn][memSeq] != nullptr) {
    Insn *movInsnOfStr = str2MovMap[&strInsn][memSeq];
    auto &vregOpnd = static_cast<RegOperand&>(movInsnOfStr->GetOperand(kInsnFirstOpnd));
    movInsn = &cgFunc.GetCG()->BuildInstruction<AArch64Insn>(movMop, resRegOpnd, vregOpnd);
  } else {
    movInsn = &cgFunc.GetCG()->BuildInstruction<AArch64Insn>(movMop, resRegOpnd, srcRegOpnd);
  }
  movInsn->SetId(ldrInsn.GetId());
  ldrInsn.GetBB()->ReplaceInsn(ldrInsn, *movInsn);
  /* Add comment. */
  MapleString newComment = ldrInsn.GetComment();
  if (strInsn.IsStorePair()) {
    newComment += ";  stp-load live version.";
  } else {
    newComment += ";  str-load live version.";
  }
  movInsn->SetComment(newComment);
  cgFunc.GetRD()->InitGenUse(*ldrInsn.GetBB(), false);
}

void AArch64StoreLoadOpt::GenerateMoveDeadInsn(RegOperand &resRegOpnd, RegOperand &srcRegOpnd,
                                               Insn &ldrInsn, Insn &strInsn, short memSeq) {
  Insn *newMovInsn = nullptr;
  RegOperand *vregOpnd = nullptr;

  if (str2MovMap[&strInsn][memSeq] == nullptr) {
    RegType regTy = srcRegOpnd.IsOfFloatOrSIMDClass() ? kRegTyFloat : kRegTyInt;
    regno_t vRegNO =
        cgFunc.NewVReg(regTy, srcRegOpnd.GetSize() <= k32BitSize ? k4ByteSize : k8ByteSize);
    /* generate a new vreg, check if the size of DataInfo is big enough */
    if (vRegNO >= cgFunc.GetRD()->GetRegSize(*strInsn.GetBB())) {
      cgFunc.GetRD()->EnlargeRegCapacity(vRegNO);
    }
    vregOpnd = &cgFunc.CreateVirtualRegisterOperand(vRegNO);
    MOperator newMop = SelectMovMop(resRegOpnd.IsOfFloatOrSIMDClass(), resRegOpnd.GetSize() == k64BitSize);
    newMovInsn = &cgFunc.GetCG()->BuildInstruction<AArch64Insn>(newMop, *vregOpnd, srcRegOpnd);
    newMovInsn->SetId(strInsn.GetId() + memSeq + 1);
    strInsn.GetBB()->InsertInsnAfter(strInsn, *newMovInsn);
    str2MovMap[&strInsn][memSeq] = newMovInsn;
    /* update DataInfo */
    cgFunc.GetRD()->UpdateInOut(*strInsn.GetBB(), true);
  } else {
    newMovInsn = str2MovMap[&strInsn][memSeq];
    vregOpnd = &static_cast<RegOperand&>(newMovInsn->GetOperand(kInsnFirstOpnd));
  }
  MOperator movMop = SelectMovMop(resRegOpnd.IsOfFloatOrSIMDClass(), resRegOpnd.GetSize() == k64BitSize);
  Insn &movInsn = cgFunc.GetCG()->BuildInstruction<AArch64Insn>(movMop, resRegOpnd, *vregOpnd);
  movInsn.SetId(ldrInsn.GetId());
  ldrInsn.GetBB()->ReplaceInsn(ldrInsn, movInsn);

  /* Add comment. */
  MapleString newComment = ldrInsn.GetComment();
  if (strInsn.IsStorePair()) {
    newComment += ";  stp-load die version.";
  } else {
    newComment += ";  str-load die version.";
  }
  movInsn.SetComment(newComment);
  cgFunc.GetRD()->InitGenUse(*ldrInsn.GetBB(), false);
}

/*
 * Transfer: store wzr, [MEM]
 *           ... // May exist branches.
 *           load  x200, [MEM]
 *        ==>
 *        OPT_VERSION_STP_ZERO / OPT_VERSION_STR_ZERO:
 *            store wzr, [MEM]
 *            ... // May exist branches. if x100 not dead here.
 *            mov   x200, wzr
 *
 *  Params:
 *    stInsn: indicate store insn.
 *    strSrcIdx: index of source register operand of store insn. (wzr in this example)
 *    memUseInsnSet: insns using memOperand
 */
void AArch64StoreLoadOpt::DoLoadZeroToMoveTransfer(const Insn &strInsn, short strSrcIdx,
                                                   const InsnSet &memUseInsnSet) const {
  /* comment for strInsn should be only added once */
  for (auto *ldrInsn : memUseInsnSet) {
    /* Currently we don't support useInsn is ldp insn. */
    if (!ldrInsn->IsLoad() || ldrInsn->GetResultNum() > 1) {
      continue;
    }
    /* ldr reg, [mem], the index of [mem] is 1 */
    InsnSet defInsnForUseInsns = cgFunc.GetRD()->FindDefForMemOpnd(*ldrInsn, 1);
    /* If load has multiple definition, continue. */
    if (defInsnForUseInsns.size() > 1) {
      continue;
    }

    auto &resOpnd = ldrInsn->GetOperand(0);
    auto &srcOpnd = strInsn.GetOperand(strSrcIdx);

    ASSERT(resOpnd.GetSize() == srcOpnd.GetSize(), "For stack location, the size of src and dst should be same.");
    RegOperand &resRegOpnd = static_cast<RegOperand&>(resOpnd);
    MOperator movMop = SelectMovMop(resRegOpnd.IsOfFloatOrSIMDClass(), resRegOpnd.GetSize() == k64BitSize);
    Insn &movInsn = cgFunc.GetCG()->BuildInstruction<AArch64Insn>(movMop, resOpnd, srcOpnd);
    movInsn.SetId(ldrInsn->GetId());
    ldrInsn->GetBB()->ReplaceInsn(*ldrInsn, movInsn);

    /* Add comment. */
    MapleString newComment = ldrInsn->GetComment();
    newComment += ",  str-load zero version";
    movInsn.SetComment(newComment);
  }
}

bool AArch64StoreLoadOpt::CheckStoreOpCode(MOperator opCode) const {
  switch (opCode) {
    case MOP_wstr:
    case MOP_xstr:
    case MOP_sstr:
    case MOP_dstr:
    case MOP_wstp:
    case MOP_xstp:
    case MOP_sstp:
    case MOP_dstp:
    case MOP_wstrb:
    case MOP_wstrh:
      return true;
    default:
      return false;
  }
}

/*
 * Optimize: store x100, [MEM]
 *           ... // May exist branches.
 *           load  x200, [MEM]
 *        ==>
 *        OPT_VERSION_STP_LIVE / OPT_VERSION_STR_LIVE:
 *           store x100, [MEM]
 *           ... // May exist branches. if x100 not dead here.
 *           mov   x200, x100
 *        OPT_VERSION_STP_DIE / OPT_VERSION_STR_DIE:
 *           store x100, [MEM]
 *           mov x9000(new reg), x100
 *           ... // May exist branches. if x100 dead here.
 *           mov   x200, x9000
 *
 *  Note: x100 may be wzr/xzr registers.
 */
void AArch64StoreLoadOpt::DoStoreLoadOpt() {
  FOR_ALL_BB(bb, &cgFunc) {
    FOR_BB_INSNS(insn, bb) {
      if (!insn->IsMachineInstruction() || !insn->IsStore() || !CheckStoreOpCode(insn->GetMachineOpcode())) {
        continue;
      }
      if (insn->IsStorePair()) {
        ProcessStrPair(*insn);
        continue;
      }
      ProcessStr(*insn);
    }
  }
}

void AArch64StoreLoadOpt::ProcessStrPair(Insn &insn) {
  const short memIndex = 2;
  short regIndex = 0;
  Operand &opnd = insn.GetOperand(memIndex);
  auto &memOpnd = static_cast<AArch64MemOperand&>(opnd);
  RegOperand *base = memOpnd.GetBaseRegister();
  if ((base == nullptr) || !(cgFunc.GetRD()->IsFrameReg(*base))) {
    return;
  }
  ASSERT(memOpnd.GetIndexRegister() == nullptr, "frame MemOperand must not be exist register index");
  InsnSet memUseInsnSet;
  for (int i = 0; i != kMaxMovNum; ++i) {
    memUseInsnSet.clear();
    if (i == 0) {
      regIndex = 0;
      memUseInsnSet = cgFunc.GetRD()->FindUseForMemOpnd(insn, memIndex);
    } else {
      regIndex = 1;
      memUseInsnSet = cgFunc.GetRD()->FindUseForMemOpnd(insn, memIndex, true);
    }
    if (memUseInsnSet.empty()) {
      return;
    }
    auto &regOpnd = static_cast<RegOperand&>(insn.GetOperand(regIndex));
    if (regOpnd.IsZeroRegister()) {
      DoLoadZeroToMoveTransfer(insn, regIndex, memUseInsnSet);
    } else {
      DoLoadToMoveTransfer(insn, regIndex, i, memUseInsnSet);
    }
  }
}

void AArch64StoreLoadOpt::ProcessStr(Insn &insn) {
  /* str x100, [mem], mem index is 1, x100 index is 0; */
  const short memIndex = 1;
  const short regIndex = 0;
  Operand &opnd = insn.GetOperand(memIndex);
  auto &memOpnd = static_cast<AArch64MemOperand&>(opnd);
  RegOperand *base = memOpnd.GetBaseRegister();
  if ((base == nullptr) || !(cgFunc.GetRD()->IsFrameReg(*base))) {
    return;
  }
  ASSERT(memOpnd.GetIndexRegister() == nullptr, "frame MemOperand must not be exist register index");

  InsnSet memUseInsnSet = cgFunc.GetRD()->FindUseForMemOpnd(insn, memIndex);
  if (memUseInsnSet.empty()) {
    return;
  }

  auto *regOpnd = static_cast<RegOperand*>(insn.GetOpnd(regIndex));
  CHECK_NULL_FATAL(regOpnd);
  if (regOpnd->IsZeroRegister()) {
    DoLoadZeroToMoveTransfer(insn, regIndex, memUseInsnSet);
  } else {
    DoLoadToMoveTransfer(insn, regIndex, 0, memUseInsnSet);
  }
}
}  /* namespace maplebe */
