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
#include "aarch64_global.h"
#include "aarch64_reaching.h"
#include "aarch64_cg.h"
#include "aarch64_live.h"

namespace maplebe {
using namespace maple;

void AArch64GlobalOpt::Run() {
  if (cgFunc.NumBBs() > kMaxBBNum || cgFunc.GetRD()->GetMaxInsnNO() > kMaxInsnNum) {
    return;
  }
  OptimizeManager optManager(cgFunc);
  optManager.Optimize<BackPropPattern>();
  optManager.Optimize<ForwardPropPattern>();
  optManager.Optimize<CselPattern>();
  optManager.Optimize<CmpCsetPattern>();
  optManager.Optimize<RedundantUxtPattern>();
  optManager.Optimize<LocalVarSaveInsnPattern>();
}

/* if used Operand in insn is defined by zero in all define insn, return true */
bool OptimizePattern::OpndDefByZero(Insn &insn, int32 useIdx) const {
  ASSERT(insn.GetOperand(useIdx).IsRegister(), "the used Operand must be Register");
  /* Zero Register don't need be defined */
  if (insn.GetOperand(useIdx).IsZeroRegister()) {
    return true;
  }

  InsnSet defInsns = cgFunc.GetRD()->FindDefForRegOpnd(insn, useIdx);
  ASSERT(!defInsns.empty(), "operand must be defined before used");
  for (auto &defInsn : defInsns) {
    if (!InsnDefZero(*defInsn)) {
      return false;
    }
  }
  return true;
}

/* if used Operand in insn is defined by one in all define insn, return true */
bool OptimizePattern::OpndDefByOne(Insn &insn, int32 useIdx) const {
  ASSERT(insn.GetOperand(useIdx).IsRegister(), "the used Operand must be Register");
  /* Zero Register don't need be defined */
  if (insn.GetOperand(useIdx).IsZeroRegister()) {
    return false;
  }
  InsnSet defInsns = cgFunc.GetRD()->FindDefForRegOpnd(insn, useIdx);
  ASSERT(!defInsns.empty(), "operand must be defined before used");
  for (auto &defInsn : defInsns) {
    if (!InsnDefOne(*defInsn)) {
      return false;
    }
  }
  return true;
}

/* if used Operand in insn is defined by one valid bit in all define insn, return true */
bool OptimizePattern::OpndDefByOneOrZero(Insn &insn, int32 useIdx) const {
  if (insn.GetOperand(useIdx).IsZeroRegister()) {
    return true;
  }

  InsnSet defInsnSet = cgFunc.GetRD()->FindDefForRegOpnd(insn, useIdx);
  ASSERT(!defInsnSet.empty(), "Operand must be defined before used");

  for (auto &defInsn : defInsnSet) {
    if (!InsnDefOneOrZero(*defInsn)) {
      return false;
    }
  }
  return true;
}

/* if defined operand(must be first insn currently) in insn is const one, return true */
bool OptimizePattern::InsnDefOne(Insn &insn) {
  MOperator defMop = insn.GetMachineOpcode();
  switch (defMop) {
    case MOP_xmovri32:
    case MOP_xmovri64: {
      Operand &srcOpnd = insn.GetOperand(1);
      ASSERT(srcOpnd.IsIntImmediate(), "expects ImmOperand");
      ImmOperand &srcConst = static_cast<ImmOperand&>(srcOpnd);
      int64 srcConstValue = srcConst.GetValue();
      if (srcConstValue == 1) {
        return true;
      }
      return false;
    }
    default:
      return false;
  }
}

/* if defined operand(must be first insn currently) in insn is const zero, return true */
bool OptimizePattern::InsnDefZero(Insn &insn) {
  MOperator defMop = insn.GetMachineOpcode();
  switch (defMop) {
    case MOP_xmovri32:
    case MOP_xmovri64: {
      Operand &srcOpnd = insn.GetOperand(kInsnSecondOpnd);
      ASSERT(srcOpnd.IsIntImmediate(), "expects ImmOperand");
      ImmOperand &srcConst = static_cast<ImmOperand&>(srcOpnd);
      int64 srcConstValue = srcConst.GetValue();
      if (srcConstValue == 0) {
        return true;
      }
      return false;
    }
    case MOP_xmovrr:
    case MOP_wmovrr:
      return insn.GetOperand(kInsnSecondOpnd).IsZeroRegister();
    default:
      return false;
  }
}

/* if defined operand(must be first insn currently) in insn has only one valid bit, return true */
bool OptimizePattern::InsnDefOneOrZero(Insn &insn) {
  MOperator defMop = insn.GetMachineOpcode();
  switch (defMop) {
    case MOP_wcsetrc:
    case MOP_xcsetrc:
      return true;
    case MOP_xmovri32:
    case MOP_xmovri64: {
      Operand &defOpnd = insn.GetOperand(kInsnSecondOpnd);
      ASSERT(defOpnd.IsIntImmediate(), "expects ImmOperand");
      auto &defConst = static_cast<ImmOperand&>(defOpnd);
      int64 defConstValue = defConst.GetValue();
      if (defConstValue != 0 && defConstValue != 1) {
        return false;
      } else {
        return true;
      }
    }
    case MOP_xmovrr:
    case MOP_wmovrr: {
      return insn.GetOperand(kInsnSecondOpnd).IsZeroRegister();
    }
    case MOP_wlsrrri5:
    case MOP_xlsrrri6: {
      Operand &opnd2 = insn.GetOperand(kInsnThirdOpnd);
      ASSERT(opnd2.IsIntImmediate(), "expects ImmOperand");
      ImmOperand &opndImm = static_cast<ImmOperand&>(opnd2);
      int64 shiftBits = opndImm.GetValue();
      if (((defMop == MOP_wlsrrri5) && (shiftBits == k32BitSize - 1)) ||
          ((defMop == MOP_xlsrrri6) && (shiftBits == k64BitSize - 1))) {
        return true;
      } else {
        return false;
      }
    }
    default:
      return false;
  }
}

void OptimizePattern::ReplaceAllUsedOpndWithNewOpnd(const InsnSet &useInsnSet, uint32 regNO,
                                                    Operand &newOpnd, bool updateInfo) const {
  for (auto useInsn : useInsnSet) {
    const AArch64MD *md = &AArch64CG::kMd[static_cast<AArch64Insn*>(useInsn)->GetMachineOpcode()];
    uint32 opndNum = useInsn->GetOperandSize();
    for (uint32 i = 0; i < opndNum; ++i) {
      Operand &opnd = useInsn->GetOperand(i);
      AArch64OpndProp *regProp = static_cast<AArch64OpndProp*>(md->operand[i]);
      if (!regProp->IsRegUse() && !opnd.IsMemoryAccessOperand()) {
        continue;
      }

      if (opnd.IsRegister() && (static_cast<RegOperand&>(opnd).GetRegisterNumber() == regNO)) {
        useInsn->SetOperand(i, newOpnd);
        if (updateInfo) {
          cgFunc.GetRD()->InitGenUse(*useInsn->GetBB(), false);
        }
      } else if (opnd.IsMemoryAccessOperand()) {
        AArch64MemOperand &memOpnd = static_cast<AArch64MemOperand&>(opnd);
        RegOperand *base = memOpnd.GetBaseRegister();
        RegOperand *index = memOpnd.GetIndexRegister();
        MemOperand *newMem = nullptr;
        if (base != nullptr && (base->GetRegisterNumber() == regNO)) {
          newMem = static_cast<MemOperand*>(opnd.Clone(*cgFunc.GetMemoryPool()));
          CHECK_FATAL(newMem != nullptr, "null ptr check");
          newMem->SetBaseRegister(*static_cast<RegOperand*>(&newOpnd));
          useInsn->SetOperand(i, *newMem);
          if (updateInfo) {
            cgFunc.GetRD()->InitGenUse(*useInsn->GetBB(), false);
          }
        }
        if (index != nullptr && (index->GetRegisterNumber() == regNO)) {
          newMem = static_cast<MemOperand*>(opnd.Clone(*cgFunc.GetMemoryPool()));
          CHECK_FATAL(newMem != nullptr, "null ptr check");
          newMem->SetIndexRegister(*static_cast<RegOperand*>(&newOpnd));
          useInsn->SetOperand(i, *newMem);
          if (updateInfo) {
            cgFunc.GetRD()->InitGenUse(*useInsn->GetBB(), false);
          }
        }
      }
    }
  }
}

bool ForwardPropPattern::CheckCondition(Insn &insn) {
  if (!insn.IsMachineInstruction()) {
    return false;
  }
  if ((insn.GetMachineOpcode() != MOP_xmovrr) && (insn.GetMachineOpcode() != MOP_wmovrr)) {
    return false;
  }
  Operand &firstOpnd = insn.GetOperand(kInsnFirstOpnd);
  Operand &secondOpnd = insn.GetOperand(kInsnSecondOpnd);
  RegOperand &firstRegOpnd = static_cast<RegOperand&>(firstOpnd);
  RegOperand &secondRegOpnd = static_cast<RegOperand&>(secondOpnd);
  uint32 firstRegNO = firstRegOpnd.GetRegisterNumber();
  uint32 secondRegNO = secondRegOpnd.GetRegisterNumber();
  if (firstRegOpnd.IsZeroRegister() || !firstRegOpnd.IsVirtualRegister() || !secondRegOpnd.IsVirtualRegister()) {
    return false;
  }
  firstRegUseInsnSet = cgFunc.GetRD()->FindUseForRegOpnd(insn, firstRegNO, true);
  if (firstRegUseInsnSet.empty()) {
    return false;
  }
  InsnSet secondRegDefInsnSet = cgFunc.GetRD()->FindDefForRegOpnd(insn, secondRegNO, true);
  if (secondRegDefInsnSet.size() != 1 || RegOperand::IsSameReg(firstOpnd, secondOpnd)) {
    return false;
  }
  bool toDoOpt = true;
  for (auto useInsn : firstRegUseInsnSet) {
    if (!cgFunc.GetRD()->RegIsLiveBetweenInsn(secondRegNO, insn, *useInsn)) {
      toDoOpt = false;
      break;
    }
    InsnSet defInsnSet = cgFunc.GetRD()->FindDefForRegOpnd(*useInsn, firstRegNO, true);
    if (defInsnSet.size() > 1) {
      toDoOpt = false;
      break;
    }
  }
  return toDoOpt;
}

void ForwardPropPattern::Optimize(Insn &insn) {
  Operand &firstOpnd = insn.GetOperand(kInsnFirstOpnd);
  Operand &secondOpnd = insn.GetOperand(kInsnSecondOpnd);
  RegOperand &firstRegOpnd = static_cast<RegOperand&>(firstOpnd);
  uint32 firstRegNO = firstRegOpnd.GetRegisterNumber();

  for (auto *useInsn : firstRegUseInsnSet) {
    const AArch64MD *md = &AArch64CG::kMd[static_cast<AArch64Insn*>(useInsn)->GetMachineOpcode()];
    uint32 opndNum = useInsn->GetOperandSize();
    for (uint32 i = 0; i < opndNum; ++i) {
      Operand &opnd = useInsn->GetOperand(i);
      const AArch64OpndProp *regProp = md->GetOperand(i);
      if (!regProp->IsRegUse() && !opnd.IsMemoryAccessOperand()) {
        continue;
      }

      if (opnd.IsRegister() && (static_cast<RegOperand&>(opnd).GetRegisterNumber() == firstRegNO)) {
        useInsn->SetOperand(i, secondOpnd);
        if (((useInsn->GetMachineOpcode() == MOP_xmovrr) || (useInsn->GetMachineOpcode() == MOP_wmovrr)) &&
            (static_cast<RegOperand&>(useInsn->GetOperand(kInsnSecondOpnd)).IsVirtualRegister()) &&
            (static_cast<RegOperand&>(useInsn->GetOperand(kInsnFirstOpnd)).IsVirtualRegister())) {
          (void)modifiedBB.insert(useInsn->GetBB());
        }
        cgFunc.GetRD()->InitGenUse(*useInsn->GetBB(), false);
      } else if (opnd.IsMemoryAccessOperand()) {
        AArch64MemOperand &memOpnd = static_cast<AArch64MemOperand&>(opnd);
        RegOperand *base = memOpnd.GetBaseRegister();
        RegOperand *index = memOpnd.GetIndexRegister();
        MemOperand *newMem = nullptr;
        if (base != nullptr && (base->GetRegisterNumber() == firstRegNO)) {
          newMem = static_cast<MemOperand*>(opnd.Clone(*cgFunc.GetMemoryPool()));
          CHECK_FATAL(newMem != nullptr, "null ptr check");
          newMem->SetBaseRegister(static_cast<RegOperand&>(secondOpnd));
          useInsn->SetOperand(i, *newMem);
          cgFunc.GetRD()->InitGenUse(*useInsn->GetBB(), false);
        }
        if ((index != nullptr) && (index->GetRegisterNumber() == firstRegNO)) {
          newMem = static_cast<MemOperand*>(opnd.Clone(*cgFunc.GetMemoryPool()));
          CHECK_FATAL(newMem != nullptr, "null ptr check");
          newMem->SetIndexRegister(static_cast<RegOperand&>(secondOpnd));
          useInsn->SetOperand(i, *newMem);
          cgFunc.GetRD()->InitGenUse(*useInsn->GetBB(), false);
        }
      }
    }
  }
  insn.SetOperand(0, secondOpnd);
  cgFunc.GetRD()->UpdateInOut(*insn.GetBB(), true);
}

void ForwardPropPattern::Init() {
  firstRegUseInsnSet.clear();
}

void ForwardPropPattern::Run() {
  bool secondTime = false;
  do {
    FOR_ALL_BB(bb, &cgFunc) {
      if (bb->IsUnreachable() || (secondTime && modifiedBB.find(bb) == modifiedBB.end())) {
        continue;
      }

      if (secondTime) {
        modifiedBB.erase(bb);
      }

      FOR_BB_INSNS(insn, bb) {
        Init();
        if (!CheckCondition(*insn)) {
          continue;
        }
        Optimize(*insn);
      }
    }
    secondTime = true;
  } while (!modifiedBB.empty());
}

bool BackPropPattern::CheckAndGetOpnd(Insn &insn) {
  if (!insn.IsMachineInstruction()) {
    return false;
  }
  if ((insn.GetMachineOpcode() != MOP_xmovrr) && (insn.GetMachineOpcode() != MOP_wmovrr)) {
    return false;
  }
  Operand &firstOpnd = insn.GetOperand(kInsnFirstOpnd);
  Operand &secondOpnd = insn.GetOperand(kInsnSecondOpnd);
  if (RegOperand::IsSameReg(firstOpnd, secondOpnd)) {
    return false;
  }

  firstRegOpnd = &static_cast<RegOperand&>(firstOpnd);
  secondRegOpnd = &static_cast<RegOperand&>(secondOpnd);
  if (firstRegOpnd->IsZeroRegister() || !secondRegOpnd->IsVirtualRegister()) {
    return false;
  }
  firstRegNO = firstRegOpnd->GetRegisterNumber();
  secondRegNO = secondRegOpnd->GetRegisterNumber();
  return true;
}

bool BackPropPattern::DestOpndHasUseInsns(Insn &insn) {
  BB &bb = *insn.GetBB();
  InsnSet useInsnSetOfFirstOpnd;
  bool findRes = cgFunc.GetRD()->FindRegUseBetweenInsn(firstRegNO, insn.GetNext(),
                                                       bb.GetLastInsn(), useInsnSetOfFirstOpnd);
  if ((findRes && useInsnSetOfFirstOpnd.empty()) ||
      (!findRes && useInsnSetOfFirstOpnd.empty() && !bb.GetLiveOut()->TestBit(firstRegNO))) {
    return false;
  }
  return true;
}

bool BackPropPattern::DestOpndLiveOutToEHSuccs(Insn &insn) {
  BB &bb = *insn.GetBB();
  for (auto ehSucc : bb.GetEhSuccs()) {
    if (ehSucc->GetLiveIn()->TestBit(firstRegNO)) {
      return true;
    }
  }
  return false;
}

bool BackPropPattern::CheckSrcOpndDefAndUseInsns(Insn &insn) {
  BB &bb = *insn.GetBB();
  /* secondOpnd is defined in other BB */
  std::vector<Insn*> defInsnVec = cgFunc.GetRD()->FindRegDefBetweenInsn(secondRegNO, bb.GetFirstInsn(), insn.GetPrev());
  if (defInsnVec.size() != 1) {
    return false;
  }
  defInsnForSecondOpnd = defInsnVec.back();
  /* part defined */
  if ((defInsnForSecondOpnd->GetMachineOpcode() == MOP_xmovkri16) ||
      (defInsnForSecondOpnd->GetMachineOpcode() == MOP_wmovkri16)) {
    return false;
  }
  bool findFinish = cgFunc.GetRD()->FindRegUseBetweenInsn(secondRegNO, defInsnForSecondOpnd->GetNext(),
                                                          bb.GetLastInsn(), srcOpndUseInsnSet);
  if (!findFinish && bb.GetLiveOut()->TestBit(secondRegNO)) {
    return false;
  }
  return true;
}

bool BackPropPattern::CheckPredefineInsn(Insn &insn) {
  if (insn.GetPrev() == defInsnForSecondOpnd) {
    return true;
  }
  std::vector<Insn*> preDefInsnForFirstOpndVec;
  BB &bb = *insn.GetBB();
  if (cgFunc.GetRD()->CheckRegGen(bb, firstRegNO)) {
    preDefInsnForFirstOpndVec =
        cgFunc.GetRD()->FindRegDefBetweenInsn(firstRegNO, defInsnForSecondOpnd->GetNext(), insn.GetPrev());
  }
  if (!preDefInsnForFirstOpndVec.empty()) {
    return false;
  }
  /* there is no predefine insn in current bb */
  InsnSet useInsnSetForFirstOpnd;
  cgFunc.GetRD()->FindRegUseBetweenInsn(firstRegNO, defInsnForSecondOpnd->GetNext(), insn.GetPrev(),
                                        useInsnSetForFirstOpnd);
  if (!useInsnSetForFirstOpnd.empty()) {
    return false;
  }
  return true;
}

bool BackPropPattern::CheckRedefineInsn(Insn &insn) {
  for (auto useInsn : srcOpndUseInsnSet) {
    if ((useInsn->GetId() > insn.GetId()) && (insn.GetNext() != useInsn) &&
        !cgFunc.GetRD()->FindRegDefBetweenInsn(firstRegNO, insn.GetNext(), useInsn->GetPrev()).empty()) {
      return false;
    }
  }
  return true;
}

bool BackPropPattern::CheckCondition(Insn &insn) {
  if (!CheckAndGetOpnd(insn)) {
    return false;
  }
  if (!DestOpndHasUseInsns(insn)) {
    return false;
  }
  /* first register must not be live out to eh_succs */
  if (DestOpndLiveOutToEHSuccs(insn)) {
    return false;
  }
  if (!CheckSrcOpndDefAndUseInsns(insn)) {
    return false;
  }
  /* check predefine insn */
  if (!CheckPredefineInsn(insn)) {
    return false;
  }
  /* check redefine insn */
  if (!CheckRedefineInsn(insn)) {
    return false;
  }
  return true;
}

void BackPropPattern::Optimize(Insn &insn) {
  Operand &firstOpnd = insn.GetOperand(kInsnFirstOpnd);
  ReplaceAllUsedOpndWithNewOpnd(srcOpndUseInsnSet, secondRegNO, firstOpnd, false);
  /* replace define insn */
  const AArch64MD *md = &AArch64CG::kMd[static_cast<AArch64Insn*>(defInsnForSecondOpnd)->GetMachineOpcode()];
  uint32 opndNum = defInsnForSecondOpnd->GetOperandSize();
  for (uint32 i = 0; i < opndNum; ++i) {
    Operand &opnd = defInsnForSecondOpnd->GetOperand(i);
    AArch64OpndProp *regProp = static_cast<AArch64OpndProp*>(md->operand[i]);
    if (!regProp->IsRegDef() && !opnd.IsMemoryAccessOperand()) {
      continue;
    }

    if (opnd.IsRegister() && (static_cast<RegOperand&>(opnd).GetRegisterNumber() == secondRegNO)) {
      defInsnForSecondOpnd->SetOperand(i, firstOpnd);
    } else if (opnd.IsMemoryAccessOperand()) {
      AArch64MemOperand &memOpnd = static_cast<AArch64MemOperand&>(opnd);
      RegOperand *base = memOpnd.GetBaseRegister();
      if (base != nullptr && memOpnd.GetAddrMode() == AArch64MemOperand::kAddrModeBOi &&
          (memOpnd.IsPostIndexed() || memOpnd.IsPreIndexed()) && base->GetRegisterNumber() == secondRegNO) {
        MemOperand *newMem = static_cast<MemOperand*>(opnd.Clone(*cgFunc.GetMemoryPool()));
        CHECK_FATAL(newMem != nullptr, "null ptr check");
        newMem->SetBaseRegister(static_cast<RegOperand&>(firstOpnd));
        defInsnForSecondOpnd->SetOperand(i, *newMem);
      }
    }
  }
  insn.GetBB()->RemoveInsn(insn);
}

void BackPropPattern::Init() {
  firstRegOpnd = nullptr;
  secondRegOpnd = nullptr;
  firstRegNO = 0;
  secondRegNO = 0;
  srcOpndUseInsnSet.clear();
  defInsnForSecondOpnd = nullptr;
}

void BackPropPattern::Run() {
  bool secondTime = false;
  std::set<BB*, BBIdCmp> modifiedBB;
  do {
    FOR_ALL_BB(bb, &cgFunc) {
      if (bb->IsUnreachable() || (secondTime && modifiedBB.find(bb) == modifiedBB.end())) {
        continue;
      }

      if (secondTime) {
        modifiedBB.erase(bb);
      }

      FOR_BB_INSNS_REV(insn, bb) {
        Init();
        if (!CheckCondition(*insn)) {
          continue;
        }
        (void)modifiedBB.insert(bb);
        Optimize(*insn);
      }
      cgFunc.GetRD()->UpdateInOut(*bb);
    }
    secondTime = true;
  } while (!modifiedBB.empty());
}

bool CmpCsetPattern::CheckCondition(Insn &insn) {
  nextInsn = insn.GetNextMachineInsn();
  if (nextInsn == nullptr || !insn.IsMachineInstruction()) {
    return false;
  }

  MOperator firstMop = insn.GetMachineOpcode();
  MOperator secondMop = nextInsn->GetMachineOpcode();
  if (!(((firstMop == MOP_wcmpri) || (firstMop == MOP_xcmpri)) &&
        ((secondMop == MOP_wcsetrc) || (secondMop == MOP_xcsetrc)))) {
    return false;
  }

  /* get cmp_first operand */
  cmpFirstOpnd = &(insn.GetOperand(kInsnSecondOpnd));
  /* get cmp second Operand, ImmOperand must be 0 or 1 */
  cmpSecondOpnd = &(insn.GetOperand(kInsnThirdOpnd));
  ASSERT(cmpSecondOpnd->IsIntImmediate(), "expects ImmOperand");
  ImmOperand *cmpConstOpnd = static_cast<ImmOperand*>(cmpSecondOpnd);
  cmpConstVal = cmpConstOpnd->GetValue();
  /* get cset first Operand */
  csetFirstOpnd = &(nextInsn->GetOperand(kInsnFirstOpnd));
  if (((cmpConstVal != 0) && (cmpConstVal != 1)) || (cmpFirstOpnd->GetSize() != csetFirstOpnd->GetSize()) ||
      !OpndDefByOneOrZero(insn, 1)) {
    return false;
  }

  InsnSet useInsnSet = cgFunc.GetRD()->FindUseForRegOpnd(insn, 0, false);
  if (useInsnSet.size() > 1) {
    return false;
  }
  return true;
}

void CmpCsetPattern::Optimize(Insn &insn) {
  Insn *csetInsn = nextInsn;
  BB &bb = *insn.GetBB();
  nextInsn = nextInsn->GetNextMachineInsn();
  /* get condition Operand */
  CondOperand &cond = static_cast<CondOperand&>(csetInsn->GetOperand(kInsnSecondOpnd));
  if (((cmpConstVal == 0) && (cond.GetCode() == CC_NE)) || ((cmpConstVal == 1) && (cond.GetCode() == CC_EQ))) {
    if (RegOperand::IsSameReg(*cmpFirstOpnd, *csetFirstOpnd)) {
      bb.RemoveInsn(insn);
      bb.RemoveInsn(*csetInsn);
    } else {
      MOperator mopCode = (cmpFirstOpnd->GetSize() == k64BitSize) ? MOP_xmovrr : MOP_wmovrr;
      Insn &newInsn = cgFunc.GetCG()->BuildInstruction<AArch64Insn>(mopCode, *csetFirstOpnd, *cmpFirstOpnd);
      newInsn.SetId(insn.GetId());
      bb.ReplaceInsn(insn, newInsn);
      bb.RemoveInsn(*csetInsn);
    }
  } else if (((cmpConstVal == 1) && (cond.GetCode() == CC_NE)) ||
             ((cmpConstVal == 0) && (cond.GetCode() == CC_EQ))) {
    MOperator mopCode = (cmpFirstOpnd->GetSize() == k64BitSize) ? MOP_xeorrri13 : MOP_weorrri12;
    constexpr int64 eorImm = 1;
    auto &aarch64CGFunc = static_cast<AArch64CGFunc&>(cgFunc);
    ImmOperand &one = aarch64CGFunc.CreateImmOperand(eorImm, k8BitSize, false);
    Insn &newInsn = cgFunc.GetCG()->BuildInstruction<AArch64Insn>(mopCode, *csetFirstOpnd, *cmpFirstOpnd, one);
    newInsn.SetId(insn.GetId());
    bb.ReplaceInsn(insn, newInsn);
    bb.RemoveInsn(*csetInsn);
  }

  cgFunc.GetRD()->UpdateInOut(bb, true);
}

void CmpCsetPattern::Init() {
  cmpConstVal  = 0;
  cmpFirstOpnd = nullptr;
  cmpSecondOpnd = nullptr;
  csetFirstOpnd = nullptr;
}

void CmpCsetPattern::Run() {
  FOR_ALL_BB(bb, &cgFunc) {
    FOR_BB_INSNS(insn, bb) {
      Init();
      if (!CheckCondition(*insn)) {
        continue;
      }
      Optimize(*insn);
    }
  }
}

AArch64CC_t CselPattern::GetInverseCondCode(const CondOperand &cond) const {
  switch (cond.GetCode()) {
    case CC_NE:
      return CC_EQ;
    case CC_EQ:
      return CC_NE;
    case CC_LT:
      return CC_GE;
    case CC_GE:
      return CC_LT;
    case CC_GT:
      return CC_LE;
    case CC_LE:
      return CC_GT;
    default:
      return kCcLast;
  }
}

bool CselPattern::CheckCondition(Insn &insn) {
  MOperator mopCode = insn.GetMachineOpcode();
  if ((mopCode != MOP_xcselrrrc) && (mopCode != MOP_wcselrrrc)) {
    return false;
  }
  return true;
}

void CselPattern::Optimize(Insn &insn) {
  BB &bb = *insn.GetBB();
  Operand &opnd0 = insn.GetOperand(kInsnFirstOpnd);
  Operand &cond = insn.GetOperand(kInsnFourthOpnd);
  MOperator newMop = ((opnd0.GetSize()) == k64BitSize ? MOP_xcsetrc : MOP_wcsetrc);

  if (OpndDefByOne(insn, kInsnSecondOpnd) && OpndDefByZero(insn, kInsnThirdOpnd)) {
    Insn &newInsn = cgFunc.GetCG()->BuildInstruction<AArch64Insn>(newMop, opnd0, cond);
    newInsn.SetId(insn.GetId());
    bb.ReplaceInsn(insn, newInsn);
    cgFunc.GetRD()->InitGenUse(bb, false);
  } else if (OpndDefByZero(insn, kInsnSecondOpnd) && OpndDefByOne(insn, kInsnThirdOpnd)) {
    CondOperand &originCond = static_cast<CondOperand&>(cond);
    AArch64CC_t inverseCondCode = GetInverseCondCode(originCond);
    if (inverseCondCode == kCcLast) {
      return;
    }
    auto &aarchCGFunc = static_cast<AArch64CGFunc&>(cgFunc);
    CondOperand &inverseCond = aarchCGFunc.GetCondOperand(inverseCondCode);
    Insn &newInsn = cgFunc.GetCG()->BuildInstruction<AArch64Insn>(newMop, opnd0, inverseCond);
    newInsn.SetId(insn.GetId());
    bb.ReplaceInsn(insn, newInsn);
    cgFunc.GetRD()->InitGenUse(bb, false);
  }
}

void CselPattern::Run() {
  FOR_ALL_BB(bb, &cgFunc) {
    FOR_BB_INSNS_SAFE(insn, bb, nextInsn) {
      if (!CheckCondition(*insn)) {
        continue;
      }
      Optimize(*insn);
    }
  }
}

uint32 RedundantUxtPattern::GetInsnValidBit(Insn &insn) {
  MOperator mOp = insn.GetMachineOpcode();
  uint32 nRet;
  switch (mOp) {
    case MOP_wcsetrc:
    case MOP_xcsetrc:
      nRet = 1;
      break;
    case MOP_wldrb:
    case MOP_wldrsb:
    case MOP_wldarb:
    case MOP_wldxrb:
    case MOP_wldaxrb:
      nRet = k8BitSize;
      break;
    case MOP_wldrh:
    case MOP_wldrsh:
    case MOP_wldarh:
    case MOP_wldxrh:
    case MOP_wldaxrh:
      nRet = k16BitSize;
      break;
    case MOP_wmovrr:
    case MOP_xmovri32:
    case MOP_wldli:
    case MOP_wldr:
    case MOP_wldp:
    case MOP_wldar:
    case MOP_wmovkri16:
    case MOP_wmovzri16:
    case MOP_wmovnri16:
    case MOP_wldxr:
    case MOP_wldaxr:
    case MOP_wldaxp:
    case MOP_wcsincrrrc:
    case MOP_wcselrrrc:
    case MOP_wcsinvrrrc:
      nRet = k32BitSize;
      break;
    default:
      nRet = k64BitSize;
      break;
  }
  return nRet;
}

uint32 RedundantUxtPattern::GetMaximumValidBit(Insn &insn, uint8 index, InsnSet &visitedInsn) const {
  InsnSet defInsnSet = cgFunc.GetRD()->FindDefForRegOpnd(insn, index);
  ASSERT(!defInsnSet.empty(), "operand must be defined before used");

  uint32 validBit = 0;
  uint32 nMaxValidBit = 0;
  for (auto &defInsn : defInsnSet) {
    if (visitedInsn.find(defInsn) != visitedInsn.end()) {
      continue;
    }

    (void)visitedInsn.insert(defInsn);
    MOperator mOp = defInsn->GetMachineOpcode();
    if ((mOp == MOP_wmovrr) || (mOp == MOP_xmovrr)) {
      validBit = GetMaximumValidBit(*defInsn, 1, visitedInsn);
    } else {
      validBit = GetInsnValidBit(*defInsn);
    }

    nMaxValidBit = nMaxValidBit < validBit ? validBit : nMaxValidBit;
  }
  return nMaxValidBit;
}

bool RedundantUxtPattern::CheckCondition(Insn &insn) {
  BB &bb = *insn.GetBB();
  InsnSet visitedInsn1;
  InsnSet visitedInsn2;
  if (!((insn.GetMachineOpcode() == MOP_xuxth32 &&
         GetMaximumValidBit(insn, kInsnSecondOpnd, visitedInsn1) <= k16BitSize) ||
        (insn.GetMachineOpcode() == MOP_xuxtb32 &&
         GetMaximumValidBit(insn, kInsnSecondOpnd, visitedInsn2) <= k8BitSize))) {
    return false;
  }

  Operand &firstOpnd = insn.GetOperand(kInsnFirstOpnd);
  secondOpnd = &(insn.GetOperand(kInsnSecondOpnd));
  if (RegOperand::IsSameReg(firstOpnd, *secondOpnd)) {
    bb.RemoveInsn(insn);
    /* update in/out */
    cgFunc.GetRD()->UpdateInOut(bb, true);
    return false;
  }
  useInsnSet = cgFunc.GetRD()->FindUseForRegOpnd(insn, 0, false);
  RegOperand &firstRegOpnd = static_cast<RegOperand&>(firstOpnd);
  firstRegNO = firstRegOpnd.GetRegisterNumber();
  /* for uxth R1, V501, R1 is parameter register, this can't be optimized. */
  if (firstRegOpnd.IsPhysicalRegister()) {
    return false;
  }

  if (useInsnSet.empty()) {
    bb.RemoveInsn(insn);
    /* update in/out */
    cgFunc.GetRD()->UpdateInOut(bb, true);
    return false;
  }

  RegOperand *secondRegOpnd = static_cast<RegOperand*>(secondOpnd);
  ASSERT(secondRegOpnd != nullptr, "secondRegOpnd should not be nullptr");
  uint32 secondRegNO = secondRegOpnd->GetRegisterNumber();
  for (auto useInsn : useInsnSet) {
    InsnSet defInsnSet = cgFunc.GetRD()->FindDefForRegOpnd(*useInsn, firstRegNO, true);
    if ((defInsnSet.size() > 1) || !(cgFunc.GetRD()->RegIsLiveBetweenInsn(secondRegNO, insn, *useInsn))) {
      return false;
    }
  }
  return true;
}

void RedundantUxtPattern::Optimize(Insn &insn) {
  BB &bb = *insn.GetBB();
  ReplaceAllUsedOpndWithNewOpnd(useInsnSet, firstRegNO, *secondOpnd, true);
  bb.RemoveInsn(insn);
  cgFunc.GetRD()->UpdateInOut(bb, true);
}

void RedundantUxtPattern::Init() {
  useInsnSet.clear();
  secondOpnd = nullptr;
}

void RedundantUxtPattern::Run() {
  FOR_ALL_BB(bb, &cgFunc) {
    if (bb->IsUnreachable()) {
      continue;
    }
    FOR_BB_INSNS_SAFE(insn, bb, nextInsn) {
      Init();
      if (!CheckCondition(*insn)) {
        continue;
      }
      Optimize(*insn);
    }
  }
}

bool LocalVarSaveInsnPattern::CheckFirstInsn(Insn &firstInsn) {
  MOperator mOp = firstInsn.GetMachineOpcode();
  if (mOp != MOP_xmovrr && mOp != MOP_wmovrr) {
    return false;
  }
  firstInsnSrcOpnd = &(firstInsn.GetOperand(kInsnSecondOpnd));
  RegOperand *firstInsnSrcReg = static_cast<RegOperand*>(firstInsnSrcOpnd);
  if (firstInsnSrcReg->GetRegisterNumber() != R0) {
    return false;
  }
  firstInsnDestOpnd = &(firstInsn.GetOperand(kInsnFirstOpnd));
  RegOperand *firstInsnDestReg = static_cast<RegOperand*>(firstInsnDestOpnd);
  if (firstInsnDestReg->IsPhysicalRegister()) {
    return false;
  }
  return true;
}

bool LocalVarSaveInsnPattern::CheckSecondInsn() {
  MOperator mOp = secondInsn->GetMachineOpcode();
  if (mOp != MOP_wstr && mOp != MOP_xstr) {
    return false;
  }
  secondInsnSrcOpnd = &(secondInsn->GetOperand(kInsnFirstOpnd));
  if (!RegOperand::IsSameReg(*firstInsnDestOpnd, *secondInsnSrcOpnd)) {
    return false;
  }
  /* check memOperand is stack memOperand, and x0 is stored in localref var region */
  secondInsnDestOpnd = &(secondInsn->GetOperand(kInsnSecondOpnd));
  AArch64MemOperand *secondInsnDestMem = static_cast<AArch64MemOperand*>(secondInsnDestOpnd);
  RegOperand *baseReg = secondInsnDestMem->GetBaseRegister();
  RegOperand *indexReg = secondInsnDestMem->GetIndexRegister();
  if ((baseReg == nullptr) || !(cgFunc.IsFrameReg(*baseReg)) || (indexReg != nullptr)) {
    return false;
  }
  return true;
}

bool LocalVarSaveInsnPattern::CheckAndGetUseInsn(Insn &firstInsn) {
  InsnSet useInsnSet = cgFunc.GetRD()->FindUseForRegOpnd(firstInsn, kInsnFirstOpnd, false);
  if (useInsnSet.size() != 2) { /* secondInsn and another useInsn */
    return false;
  }

  /* useInsnSet includes secondInsn and another useInsn */
  for (auto tmpUseInsn : useInsnSet) {
    if (tmpUseInsn->GetId() != secondInsn->GetId()) {
      useInsn = tmpUseInsn;
      break;
    }
  }
  return true;
}

bool LocalVarSaveInsnPattern::CheckLiveRange(Insn &firstInsn) {
  uint32 maxInsnNO = cgFunc.GetRD()->GetMaxInsnNO();
  uint32 useInsnID = useInsn->GetId();
  uint32 defInsnID = firstInsn.GetId();
  uint32 distance = useInsnID > defInsnID ? useInsnID - defInsnID : defInsnID - useInsnID;
  float liveRangeProportion = static_cast<float>(distance) / maxInsnNO;
  /* 0.3 is a balance for real optimization effect */
  if (liveRangeProportion < 0.3) {
    return false;
  }
  return true;
}

bool LocalVarSaveInsnPattern::CheckCondition(Insn &firstInsn) {
  secondInsn = firstInsn.GetNext();
  if (secondInsn == nullptr) {
    return false;
  }
  /* check firstInsn is : mov vreg, R0; */
  if (!CheckFirstInsn(firstInsn)) {
    return false;
  }
  /* check the secondInsn is : str vreg, stackMem */
  if (!CheckSecondInsn()) {
    return false;
  }
  /* find the uses of the vreg */
  if (!CheckAndGetUseInsn(firstInsn)) {
    return false;
  }
  /* simulate live range using insn distance */
  if (!CheckLiveRange(firstInsn)) {
    return false;
  }
  RegOperand *firstInsnDestReg = static_cast<RegOperand*>(firstInsnDestOpnd);
  regno_t firstInsnDestRegNO = firstInsnDestReg->GetRegisterNumber();
  InsnSet defInsnSet = cgFunc.GetRD()->FindDefForRegOpnd(*useInsn, firstInsnDestRegNO, true);
  if (defInsnSet.size() != 1) {
    return false;
  }
  ASSERT((*(defInsnSet.begin()))->GetId() == firstInsn.GetId(), "useInsn has only one define Insn : firstInsn");
  /* check whether the stack mem is changed or not */
  AArch64MemOperand *secondInsnDestMem = static_cast<AArch64MemOperand*>(secondInsnDestOpnd);
  int64 memOffset = secondInsnDestMem->GetOffsetImmediate()->GetOffsetValue();
  InsnSet memDefInsnSet = cgFunc.GetRD()->FindDefForMemOpnd(*useInsn, memOffset, true);
  if (memDefInsnSet.size() != 1) {
    return false;
  }
  if ((*(memDefInsnSet.begin()))->GetId() != secondInsn->GetId()) {
    return false;
  }
   /* check whether has call between use and def */
  if (!cgFunc.GetRD()->HasCallBetweenDefUse(firstInsn, *useInsn)) {
    return false;
  }
  return true;
}

void LocalVarSaveInsnPattern::Optimize(Insn &insn) {
  /* insert ldr insn before useInsn */
  MOperator ldrOpCode = secondInsnSrcOpnd->GetSize() == k64BitSize ? MOP_xldr : MOP_wldr;
  Insn &ldrInsn = cgFunc.GetCG()->BuildInstruction<AArch64Insn>(ldrOpCode, *secondInsnSrcOpnd, *secondInsnDestOpnd);
  ldrInsn.SetId(useInsn->GetId() - 1);
  useInsn->GetBB()->InsertInsnBefore(*useInsn, ldrInsn);
  cgFunc.GetRD()->UpdateInOut(*useInsn->GetBB(), true);
  secondInsn->SetOperand(kInsnFirstOpnd, *firstInsnSrcOpnd);
  BB *saveInsnBB = insn.GetBB();
  saveInsnBB->RemoveInsn(insn);
  cgFunc.GetRD()->UpdateInOut(*saveInsnBB, true);
}

void LocalVarSaveInsnPattern::Init() {
  firstInsnSrcOpnd = nullptr;
  firstInsnDestOpnd = nullptr;
  secondInsnSrcOpnd = nullptr;
  secondInsnDestOpnd = nullptr;
  useInsn = nullptr;
  secondInsn = nullptr;
}

void LocalVarSaveInsnPattern::Run() {
  FOR_ALL_BB(bb, &cgFunc) {
    if (bb->IsCleanup()) {
      continue;
    }
    FOR_BB_INSNS(insn, bb) {
      if (!insn->IsCall()) {
        continue;
      }
      Insn *firstInsn = insn->GetNextMachineInsn();
      if (firstInsn == nullptr) {
        continue;
      }
      Init();
      if (!CheckCondition(*firstInsn)) {
        continue;
      }
      Optimize(*firstInsn);
    }
  }
}
}  /* namespace maplebe */
