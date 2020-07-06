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
#include "aarch64_ico.h"
#include "ico.h"
#include "cg.h"
#include "cg_option.h"
#include "aarch64_isa.h"
#include "aarch64_insn.h"
#include "aarch64_cgfunc.h"

/*
 * This phase implements if-conversion optimization,
 * which tries to convert conditional branches into cset/csel instructions
 */
#define ICO_DUMP CG_DEBUG_FUNC(cgFunc)
namespace maplebe {
void AArch64IfConversionOptimizer::InitOptimizePatterns() {
  singlePassPatterns.emplace_back(memPool->New<AArch64ICOPattern>(*cgFunc));
}

Insn *AArch64ICOPattern::BuildCmpInsn(const Insn &condBr) {
  AArch64CGFunc *func = static_cast<AArch64CGFunc*>(cgFunc);
  RegOperand &reg = static_cast<RegOperand&>(condBr.GetOperand(0));
  PrimType ptyp = (reg.GetSize() == k64BitSize) ? PTY_u64 : PTY_u32;
  ImmOperand &numZero = func->CreateImmOperand(ptyp, 0);
  Operand &rflag = func->GetOrCreateRflag();
  MOperator mopCode = (reg.GetSize() == k64BitSize) ? MOP_xcmpri : MOP_wcmpri;
  Insn &cmpInsn = func->GetCG()->BuildInstruction<AArch64Insn>(mopCode, rflag, reg, numZero);
  return &cmpInsn;
}

bool AArch64ICOPattern::IsSetInsn(const Insn &insn, Operand *&dest, Operand *&src) const {
  MOperator mOpCode = insn.GetMachineOpcode();
  if (mOpCode >= MOP_xmovrr && mOpCode <= MOP_xvmovd) {
    dest = &(insn.GetOperand(0));
    src = &(insn.GetOperand(1));
    return true;
  }
  dest = nullptr;
  src = nullptr;
  return false;
}

AArch64CC_t AArch64ICOPattern::Encode(MOperator mOp, bool inverse) const {
  switch (mOp) {
    case MOP_bmi:
      return inverse ? CC_PL : CC_MI;
    case MOP_bvc:
      return inverse ? CC_VS : CC_VC;
    case MOP_bls:
      return inverse ? CC_HI : CC_LS;
    case MOP_blt:
      return inverse ? CC_GE : CC_LT;
    case MOP_ble:
      return inverse ? CC_GT : CC_LE;
    case MOP_beq:
      return inverse ? CC_NE : CC_EQ;
    case MOP_bne:
      return inverse ? CC_EQ : CC_NE;
    case MOP_blo:
      return inverse ? CC_HS : CC_LO;
    case MOP_bpl:
      return inverse ? CC_MI : CC_PL;
    case MOP_bhs:
      return inverse ? CC_LO : CC_HS;
    case MOP_bvs:
      return inverse ? CC_VC : CC_VS;
    case MOP_bhi:
      return inverse ? CC_LS : CC_HI;
    case MOP_bgt:
      return inverse ? CC_LE : CC_GT;
    case MOP_bge:
      return inverse ? CC_LT : CC_GE;
    case MOP_wcbnz:
      return inverse ? CC_EQ : CC_NE;
    case MOP_xcbnz:
      return inverse ? CC_EQ : CC_NE;
    case MOP_wcbz:
      return inverse ? CC_NE : CC_EQ;
    case MOP_xcbz:
      return inverse ? CC_NE : CC_EQ;
    default:
      return kCcLast;
  }
}

Insn *AArch64ICOPattern::BuildCondSet(const Insn &branch, RegOperand &reg, bool inverse) {
  AArch64CC_t ccCode = Encode(branch.GetMachineOpcode(), inverse);
  ASSERT(ccCode != kCcLast, "unknown cond, ccCode can't be kCcLast");
  AArch64CGFunc *func = static_cast<AArch64CGFunc*>(cgFunc);
  CondOperand &cond = func->GetCondOperand(ccCode);
  MOperator mopCode = (reg.GetSize() == k64BitSize) ? MOP_xcsetrc : MOP_wcsetrc;
  return &func->GetCG()->BuildInstruction<AArch64Insn>(mopCode, reg, cond);
}

Insn *AArch64ICOPattern::BuildCondSel(const Insn &branch, MOperator mOp, RegOperand &dst, RegOperand &src1,
                                      RegOperand &src2) {
  AArch64CC_t ccCode = Encode(branch.GetMachineOpcode(), false);
  ASSERT(ccCode != kCcLast, "unknown cond, ccCode can't be kCcLast");
  CondOperand &cond = static_cast<AArch64CGFunc*>(cgFunc)->GetCondOperand(ccCode);
  return &cgFunc->GetCG()->BuildInstruction<AArch64Insn>(mOp, dst, src1, src2, cond);
}

void AArch64ICOPattern::GenerateInsnForImm(const Insn &branchInsn, Operand &ifDest, Operand &elseDest,
                                           RegOperand &destReg, std::vector<Insn*> &generateInsn) {
  ImmOperand &imm1 = static_cast<ImmOperand&>(ifDest);
  ImmOperand &imm2 = static_cast<ImmOperand&>(elseDest);
  bool inverse = imm1.IsZero() && imm2.IsOne();
  if (inverse || (imm2.IsZero() && imm1.IsOne())) {
    Insn *csetInsn = BuildCondSet(branchInsn, destReg, inverse);
    ASSERT(csetInsn != nullptr, "build a insn failed");
    generateInsn.emplace_back(csetInsn);
  } else if (imm1.GetValue() == imm2.GetValue()) {
    MOperator mOp = (destReg.GetSize() == k64BitSize ? MOP_xmovri64 : MOP_xmovri32);
    Insn &tempInsn =
        cgFunc->GetTheCFG()->GetInsnModifier()->GetCGFunc()->GetCG()->BuildInstruction<AArch64Insn>(mOp, destReg,
                                                                                                    imm1);
    generateInsn.emplace_back(&tempInsn);
  } else {
    MOperator mOp = (destReg.GetSize() == k64BitSize ? MOP_xmovri64 : MOP_xmovri32);
    RegOperand *tempTarIf = cgFunc->GetTheCFG()->CreateVregFromReg(destReg);
    Insn &tempInsnIf =
        cgFunc->GetTheCFG()->GetInsnModifier()->GetCGFunc()->GetCG()->BuildInstruction<AArch64Insn>(mOp, *tempTarIf,
                                                                                                    imm1);
    generateInsn.emplace_back(&tempInsnIf);

    RegOperand *tempTarElse = cgFunc->GetTheCFG()->CreateVregFromReg(destReg);
    Insn &tempInsnElse =
        cgFunc->GetTheCFG()->GetInsnModifier()->GetCGFunc()->GetCG()->BuildInstruction<AArch64Insn>(mOp, *tempTarElse,
                                                                                                    imm2);
    generateInsn.emplace_back(&tempInsnElse);

    uint32 dSize = destReg.GetSize();
    bool isIntTy = destReg.IsOfIntClass();
    MOperator mOpCode = isIntTy ? (dSize == k64BitSize ? MOP_xcselrrrc : MOP_wcselrrrc)
                                : (dSize == k64BitSize ? MOP_dcselrrrc : (dSize == k32BitSize ?
                                                                          MOP_scselrrrc : MOP_hcselrrrc));
    Insn *cselInsn = BuildCondSel(branchInsn, mOpCode, destReg, *tempTarIf, *tempTarElse);
    CHECK_FATAL(cselInsn != nullptr, "build a csel insn failed");
    generateInsn.emplace_back(cselInsn);
  }
}

RegOperand *AArch64ICOPattern::GenerateRegAndTempInsn(Operand &dest, const RegOperand &destReg,
                                                      std::vector<Insn*> &generateInsn) {
  RegOperand *reg = nullptr;
  if (!dest.IsRegister()) {
    MOperator mOp = (destReg.GetSize() == k64BitSize ? MOP_xmovri64 : MOP_xmovri32);
    reg = cgFunc->GetTheCFG()->CreateVregFromReg(destReg);
    ImmOperand &tempSrcElse = static_cast<ImmOperand&>(dest);
    Insn &tempInsn =
      cgFunc->GetTheCFG()->GetInsnModifier()->GetCGFunc()->GetCG()->BuildInstruction<AArch64Insn>(mOp, *reg,
                                                                                                  tempSrcElse);
    generateInsn.emplace_back(&tempInsn);
    return reg;
  } else {
    return (static_cast<RegOperand*>(&dest));
  }
}

void AArch64ICOPattern::GenerateInsnForReg(const Insn &branchInsn, Operand &ifDest, Operand &elseDest,
                                           RegOperand &destReg, std::vector<Insn*> &generateInsn) {
  RegOperand *tReg = GenerateRegAndTempInsn(ifDest, destReg, generateInsn);
  RegOperand *eReg = GenerateRegAndTempInsn(elseDest, destReg, generateInsn);

  /* mov w0, w1   mov w0, w1  --> mov w0, w1 */
  if (eReg->GetRegisterNumber() == tReg->GetRegisterNumber()) {
    uint32 dSize = destReg.GetSize();
    bool srcIsIntTy = tReg->IsOfIntClass();
    bool destIsIntTy = destReg.IsOfIntClass();
    MOperator mOp;
    if (dSize == k64BitSize) {
      mOp = srcIsIntTy ? (destIsIntTy ? MOP_xmovrr : MOP_xvmovdr) : (destIsIntTy ? MOP_xvmovrd : MOP_xvmovd);
    } else {
      mOp = srcIsIntTy ? (destIsIntTy ? MOP_wmovrr : MOP_xvmovsr) : (destIsIntTy ? MOP_xvmovrs : MOP_xvmovs);
    }
    Insn &tempInsnIf =
        cgFunc->GetTheCFG()->GetInsnModifier()->GetCGFunc()->GetCG()->BuildInstruction<AArch64Insn>(mOp, destReg,
                                                                                                    *tReg);
    generateInsn.emplace_back(&tempInsnIf);
  } else {
    uint32 dSize = destReg.GetSize();
    bool isIntTy = destReg.IsOfIntClass();
    MOperator mOpCode = isIntTy ? (dSize == k64BitSize ? MOP_xcselrrrc : MOP_wcselrrrc)
                                : (dSize == k64BitSize ? MOP_dcselrrrc : (dSize == k32BitSize ?
                                                                          MOP_scselrrrc : MOP_hcselrrrc));
    Insn *cselInsn = BuildCondSel(branchInsn, mOpCode, destReg, *tReg, *eReg);
    CHECK_FATAL(cselInsn != nullptr, "build a csel insn failed");
    generateInsn.emplace_back(cselInsn);
  }
}

Operand *AArch64ICOPattern::GetDestReg(const std::map<Operand*, Operand*> &destSrcMap,
                                       const RegOperand &destReg) const {
  Operand *dest = nullptr;
  for (const auto &destSrcPair : destSrcMap) {
    ASSERT(destSrcPair.first->IsRegister(), "opnd must be register");
    RegOperand *destRegInMap = static_cast<RegOperand*>(destSrcPair.first);
    ASSERT(destRegInMap != nullptr, "nullptr check");
    if (destRegInMap->GetRegisterNumber() == destReg.GetRegisterNumber()) {
      dest = destSrcPair.second;
      break;
    }
  }
  return dest;
}

bool AArch64ICOPattern::BuildCondMovInsn(BB &cmpBB, const BB &bb, const std::map<Operand*, Operand*> &ifDestSrcMap,
                                         const std::map<Operand*, Operand*> &elseDestSrcMap,
                                         bool elseBBIsProcessed, std::vector<Insn*> &generateInsn) {
  Insn *branchInsn = cgFunc->GetTheCFG()->FindLastCondBrInsn(cmpBB);
  FOR_BB_INSNS_CONST(insn, (&bb)) {
    if (!insn->IsMachineInstruction() || insn->IsBranch()) {
      continue;
    }
    Operand *dest = nullptr;
    Operand *src = nullptr;

    if (!IsSetInsn(*insn, dest, src)) {
      ASSERT(false, "insn check");
    }
    ASSERT(dest->IsRegister(), "register check");
    RegOperand *destReg = static_cast<RegOperand*>(dest);

    Operand *elseDest = GetDestReg(elseDestSrcMap, *destReg);
    Operand *ifDest = GetDestReg(ifDestSrcMap, *destReg);

    if (elseBBIsProcessed) {
      if (elseDest != nullptr) {
        continue;
      }
      elseDest = dest;
      ASSERT(ifDest != nullptr, "null ptr check");
      if (!bb.GetLiveOut()->TestBit(destReg->GetRegisterNumber())) {
        continue;
      }
    } else {
      ASSERT(elseDest != nullptr, "null ptr check");
      if (ifDest == nullptr) {
        if (!bb.GetLiveOut()->TestBit(destReg->GetRegisterNumber())) {
          continue;
        }
        ifDest = dest;
      }
    }

    /* generate cset or csel instruction */
    ASSERT(ifDest != nullptr, "null ptr check");
    if (ifDest->IsIntImmediate() && elseDest->IsIntImmediate()) {
      GenerateInsnForImm(*branchInsn, *ifDest, *elseDest, *destReg, generateInsn);
    } else {
      GenerateInsnForReg(*branchInsn, *ifDest, *elseDest, *destReg, generateInsn);
    }
  }

  return true;
}

bool AArch64ICOPattern::CheckModifiedRegister(Insn &insn, std::map<Operand*, Operand*> &destSrcMap, Operand &src,
                                              Operand &dest) const {
/* src was modified in this blcok earlier */
  if (src.IsRegister()) {
    RegOperand &srcReg = static_cast<RegOperand&>(src);
    for (const auto &destSrcPair : destSrcMap) {
      ASSERT(destSrcPair.first->IsRegister(), "opnd must be register");
      RegOperand *mapSrcReg = static_cast<RegOperand*>(destSrcPair.first);
      if (mapSrcReg->GetRegisterNumber() == srcReg.GetRegisterNumber()) {
        return false;
      }
    }
  }

  /* dest register was modified earlier in this block */
  ASSERT(dest.IsRegister(), "opnd must be register");
  RegOperand &destReg = static_cast<RegOperand&>(dest);
  for (const auto &destSrcPair : destSrcMap) {
    ASSERT(destSrcPair.first->IsRegister(), "opnd must be register");
    RegOperand *mapSrcReg = static_cast<RegOperand*>(destSrcPair.first);
    if (mapSrcReg->GetRegisterNumber() == destReg.GetRegisterNumber()) {
      return false;
    }
  }

  /* src register is modified later in this block, will not be processed */
  if (src.IsRegister()) {
    RegOperand &srcReg = static_cast<RegOperand&>(src);
    if (destReg.IsOfFloatOrSIMDClass() && srcReg.IsZeroRegister()) {
      return false;
    }
    for (Insn *tmpInsn = &insn; tmpInsn != nullptr; tmpInsn = tmpInsn->GetNext()) {
      Operand *tmpDest = nullptr;
      Operand *tmpSrc = nullptr;
      if (IsSetInsn(*tmpInsn, tmpDest, tmpSrc) && tmpDest->Equals(src)) {
        ASSERT(tmpDest->IsRegister(), "opnd must be register");
        RegOperand *tmpDestReg = static_cast<RegOperand*>(tmpDest);
        if (srcReg.GetRegisterNumber() == tmpDestReg->GetRegisterNumber()) {
          return false;
        }
      }
    }
  }
  return true;
}

bool AArch64ICOPattern::CheckCondMoveBB(BB *bb, std::map<Operand*, Operand*> &destSrcMap,
                                        std::vector<Operand*> &destRegs, Operand *flagOpnd) const {
  if (bb == nullptr) {
    return false;
  }
  FOR_BB_INSNS(insn, bb) {
    if (!insn->IsMachineInstruction() || insn->IsBranch()) {
      continue;
    }
    Operand *dest = nullptr;
    Operand *src = nullptr;

    if (!IsSetInsn(*insn, dest, src)) {
      return false;
    }
    ASSERT(dest != nullptr, "null ptr check");
    ASSERT(src != nullptr, "null ptr check");

    if (!dest->IsRegister()) {
      return false;
    }

    if (!src->IsConstant() && !src->IsRegister()) {
      return false;
    }

    if (flagOpnd != nullptr) {
      RegOperand *flagReg = static_cast<RegOperand*>(flagOpnd);
      regno_t flagRegNO = flagReg->GetRegisterNumber();
      if (bb->GetLiveOut()->TestBit(flagRegNO)) {
        return false;
      }
    }

    if (!CheckModifiedRegister(*insn, destSrcMap, *src, *dest)) {
      return false;
    }

    (void)destSrcMap.insert(std::make_pair(dest, src));
    destRegs.emplace_back(dest);
  }
  return true;
}

/* Convert conditional branches into cset/csel instructions */
bool AArch64ICOPattern::DoOpt(BB &cmpBB, BB *ifBB, BB *elseBB, BB &joinBB) {
  Insn *condBr = cgFunc->GetTheCFG()->FindLastCondBrInsn(cmpBB);
  ASSERT(condBr != nullptr, "nullptr check");
  Insn *cmpInsn = FindLastCmpInsn(cmpBB);
  Operand *flagOpnd = nullptr;
  /* for cbnz and cbz institution */
  if (cgFunc->GetTheCFG()->IsCompareAndBranchInsn(*condBr)) {
    if (condBr->GetOperand(0).IsZeroRegister()) {
      return false;
    }
    cmpInsn = condBr;
    flagOpnd = &(condBr->GetOperand(0));
  }

  /* tbz will not be optimized */
  MOperator mOperator = condBr->GetMachineOpcode();
  if (mOperator == MOP_xtbz || mOperator == MOP_wtbz || mOperator == MOP_xtbnz || mOperator == MOP_wtbnz) {
    return false;
  }
  if (cmpInsn == nullptr) {
    return false;
  }

  std::vector<Operand*> ifDestRegs;
  std::vector<Operand*> elseDestRegs;

  std::map<Operand*, Operand*> ifDestSrcMap;
  std::map<Operand*, Operand*> elseDestSrcMap;

  if (!CheckCondMoveBB(elseBB, elseDestSrcMap, elseDestRegs, flagOpnd) ||
      (ifBB != nullptr && !CheckCondMoveBB(ifBB, ifDestSrcMap, ifDestRegs, flagOpnd))) {
    return false;
  }

  size_t count = elseDestRegs.size();

  for (auto *itr : ifDestRegs) {
    bool foundInElse = false;
    for (auto *elseItr : elseDestRegs) {
      RegOperand *elseDestReg = static_cast<RegOperand*>(elseItr);
      RegOperand *ifDestReg = static_cast<RegOperand*>(itr);
      if (ifDestReg->GetRegisterNumber() == elseDestReg->GetRegisterNumber()) {
        foundInElse = true;
        break;
      }
    }
    if (foundInElse) {
      continue;
    } else {
      ++count;
    }
  }
  if (count > kThreshold) {
    return false;
  }

  /* generate insns */
  std::vector<Insn*> elseGenerateInsn;
  std::vector<Insn*> ifGenerateInsn;
  bool elseBBProcessResult = false;
  if (elseBB != nullptr) {
    elseBBProcessResult = BuildCondMovInsn(cmpBB, *elseBB, ifDestSrcMap, elseDestSrcMap, false, elseGenerateInsn);
  }
  bool ifBBProcessResult = false;
  if (ifBB != nullptr) {
    ifBBProcessResult = BuildCondMovInsn(cmpBB, *ifBB, ifDestSrcMap, elseDestSrcMap, true, ifGenerateInsn);
  }
  if (!elseBBProcessResult || (ifBB != nullptr && !ifBBProcessResult)) {
    return false;
  }

  /* insert insn */
  if (cgFunc->GetTheCFG()->IsCompareAndBranchInsn(*condBr)) {
    Insn *innerCmpInsn = BuildCmpInsn(*condBr);
    cmpBB.InsertInsnBefore(*condBr, *innerCmpInsn);
    cmpInsn = innerCmpInsn;
  }

  if (elseBB != nullptr) {
    cmpBB.SetKind(elseBB->GetKind());
  } else {
    cmpBB.SetKind(ifBB->GetKind());
  }
  /* delete condBr */
  cmpBB.RemoveInsn(*condBr);
  /* Insert goto insn after csel insn. */
  if (cmpBB.GetKind() == BB::kBBGoto || cmpBB.GetKind() == BB::kBBIf) {
    if (elseBB != nullptr) {
      cmpBB.InsertInsnAfter(*cmpBB.GetLastInsn(), *elseBB->GetLastInsn());
    } else {
      cmpBB.InsertInsnAfter(*cmpBB.GetLastInsn(), *ifBB->GetLastInsn());
    }
  }

  /* Insert instructions in branches after cmpInsn */
  for (auto itr = elseGenerateInsn.rbegin(); itr != elseGenerateInsn.rend(); ++itr) {
    cmpBB.InsertInsnAfter(*cmpInsn, **itr);
  }
  for (auto itr = ifGenerateInsn.rbegin(); itr != ifGenerateInsn.rend(); ++itr) {
    cmpBB.InsertInsnAfter(*cmpInsn, **itr);
  }

  /* Remove branches and merge join */
  if (ifBB != nullptr) {
    cgFunc->GetTheCFG()->RemoveBB(*ifBB);
  }
  if (elseBB != nullptr) {
    cgFunc->GetTheCFG()->RemoveBB(*elseBB);
  }

  if (cmpBB.GetKind() != BB::kBBIf && cmpBB.GetNext() == &joinBB &&
      !maplebe::CGCFG::InLSDA(joinBB.GetLabIdx(), *cgFunc->GetEHFunc()) &&
      cgFunc->GetTheCFG()->CanMerge(cmpBB, joinBB)) {
    maplebe::CGCFG::MergeBB(cmpBB, joinBB, *cgFunc);
    keepPosition = true;
  }
  return true;
}
}  /* namespace maplebe */
