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
#include "aarch64_reaching.h"
#include "aarch64_cg.h"
#include "aarch64_operand.h"
namespace maplebe {
/* MCC_ClearLocalStackRef clear 1 stack slot, and MCC_DecRefResetPair clear 2 stack slot,
 * the stack positins cleared are recorded in callInsn->clearStackOffset
 */
constexpr short kFirstClearMemIndex = 0;
constexpr short kSecondClearMemIndex = 1;

/* insert pseudo insn for parameters definition */
void AArch64ReachingDefinition::InitStartGen() {
  BB *bb = cgFunc->GetFirstBB();

  /* Parameters should be define first. */
  ParmLocator parmLocator(cgFunc->GetBecommon());
  PLocInfo pLoc;
  for (uint32 i = 0; i < cgFunc->GetFunction().GetFormalCount(); ++i) {
    MIRType *type = cgFunc->GetFunction().GetNthParamType(i);
    parmLocator.LocateNextParm(*type, pLoc);
    if (pLoc.reg0 == 0) {
      /* If is a large frame, parameter addressing mode is based vreg:Vra. */
      continue;
    }

    uint64 symSize = cgFunc->GetBecommon().GetTypeSize(type->GetTypeIndex());
    RegType regType = (pLoc.reg0 < V0) ? kRegTyInt : kRegTyFloat;
    uint32 srcBitSize = ((symSize < k4ByteSize) ? k4ByteSize : symSize) * kBitsPerByte;

    AArch64CGFunc *aarchCGFunc = static_cast<AArch64CGFunc*>(cgFunc);
    RegOperand &regOpnd = aarchCGFunc->GetOrCreatePhysicalRegisterOperand(pLoc.reg0, srcBitSize, regType);

    MOperator mOp;
    if (regType == kRegTyInt) {
      if (srcBitSize <= k32BitSize) {
        mOp = MOP_pseudo_param_def_w;
      } else {
        mOp = MOP_pseudo_param_def_x;
      }
    } else {
      if (srcBitSize <= k32BitSize) {
        mOp = MOP_pseudo_param_def_s;
      } else {
        mOp = MOP_pseudo_param_def_d;
      }
    }

    Insn &pseudoInsn = cgFunc->GetCG()->BuildInstruction<AArch64Insn>(mOp, regOpnd);
    bb->InsertInsnBegin(pseudoInsn);
    pseudoInsns.emplace_back(&pseudoInsn);

    {
      /*
       * define memory address since store param may be transfered to stp and which with the short offset range.
       *  we can not get the correct definition before RA.
       *  example:
       *   add  x8, sp, #712
       *   stp  x0, x1, [x8]    // store param: _this Reg40_R313644
       *   stp  x2, x3, [x8,#16]    // store param: Reg41_R333743 Reg42_R333622
       *   stp  x4, x5, [x8,#32]    // store param: Reg43_R401297 Reg44_R313834
       *   str  x7, [x8,#48]    // store param: Reg46_R401297
       */
      MIRSymbol *sym = cgFunc->GetFunction().GetFormal(i);
      if (!sym->IsPreg()) {
        MIRSymbol *firstSym = cgFunc->GetFunction().GetFormal(i);
        const AArch64SymbolAlloc *firstSymLoc =
            static_cast<AArch64SymbolAlloc*>(cgFunc->GetMemlayout()->GetSymAllocInfo(firstSym->GetStIndex()));
        int32 stOffset = cgFunc->GetBaseOffset(*firstSymLoc);
        MIRType *firstType = cgFunc->GetFunction().GetNthParamType(i);
        uint32 firstSymSize = cgFunc->GetBecommon().GetTypeSize(firstType->GetTypeIndex());
        uint32 firstStackSize = firstSymSize < k4ByteSize ? k4ByteSize : firstSymSize;

        AArch64MemOperand *memOpnd = cgFunc->GetMemoryPool()->New<AArch64MemOperand>(RFP, stOffset,
                                                                                     firstStackSize * kBitsPerByte);
        MOperator mopTemp = firstStackSize <= k4ByteSize ? MOP_pseudo_param_store_w : MOP_pseudo_param_store_x;
        Insn &pseudoInsnTemp = cgFunc->GetCG()->BuildInstruction<AArch64Insn>(mopTemp, *memOpnd);
        bb->InsertInsnBegin(pseudoInsnTemp);
        pseudoInsns.emplace_back(&pseudoInsnTemp);
      }
    }
  }

  /* if function has "bl  MCC_InitializeLocalStackRef", should define corresponding memory. */
  AArch64CGFunc *a64CGFunc = static_cast<AArch64CGFunc*>(cgFunc);

  for (uint32 i = 0; i < a64CGFunc->GetRefCount(); ++i) {
    AArch64MemOperand *memOpnd = cgFunc->GetMemoryPool()->New<AArch64MemOperand>(
        RFP, a64CGFunc->GetBeginOffset() + i * k8BitSize, k64BitSize);
    Insn &pseudoInsn = cgFunc->GetCG()->BuildInstruction<AArch64Insn>(MOP_pseudo_ref_init_x, *memOpnd);

    bb->InsertInsnBegin(pseudoInsn);
    pseudoInsns.emplace_back(&pseudoInsn);
  }
}

/* insert pseudoInsns for ehBB, R0 and R1 are defined in pseudoInsns */
void AArch64ReachingDefinition::InitEhDefine(BB &bb) {
  AArch64CGFunc *aarchCGFunc = static_cast<AArch64CGFunc*>(cgFunc);

  /* Insert MOP_pseudo_eh_def_x R1. */
  RegOperand &regOpnd1 = aarchCGFunc->GetOrCreatePhysicalRegisterOperand(R1, k64BitSize, kRegTyInt);
  Insn &pseudoInsn = cgFunc->GetCG()->BuildInstruction<AArch64Insn>(MOP_pseudo_eh_def_x, regOpnd1);
  bb.InsertInsnBegin(pseudoInsn);
  pseudoInsns.emplace_back(&pseudoInsn);

  /* insert MOP_pseudo_eh_def_x R0. */
  RegOperand &regOpnd2 = aarchCGFunc->GetOrCreatePhysicalRegisterOperand(R0, k64BitSize, kRegTyInt);
  Insn &newPseudoInsn = cgFunc->GetCG()->BuildInstruction<AArch64Insn>(MOP_pseudo_eh_def_x, regOpnd2);
  bb.InsertInsnBegin(newPseudoInsn);
  pseudoInsns.emplace_back(&newPseudoInsn);
}

/* insert pseudoInsns for return value R0/V0 */
void AArch64ReachingDefinition::AddRetPseudoInsn(BB &bb) {
  AArch64reg regNO = static_cast<AArch64CGFunc*>(cgFunc)->GetReturnRegisterNumber();
  if (regNO == kInvalidRegNO) {
    return;
  }

  if (regNO == R0) {
    RegOperand &regOpnd =
        static_cast<AArch64CGFunc*>(cgFunc)->GetOrCreatePhysicalRegisterOperand(regNO, k64BitSize, kRegTyInt);
    Insn &retInsn = cgFunc->GetCG()->BuildInstruction<AArch64Insn>(MOP_pseudo_ret_int, regOpnd);
    bb.AppendInsn(retInsn);
    pseudoInsns.emplace_back(&retInsn);
  } else {
    ASSERT(regNO == V0, "CG internal error. Return value should be R0 or V0.");
    RegOperand &regOpnd =
        static_cast<AArch64CGFunc*>(cgFunc)->GetOrCreatePhysicalRegisterOperand(regNO, k64BitSize, kRegTyFloat);
    Insn &retInsn = cgFunc->GetCG()->BuildInstruction<AArch64Insn>(MOP_pseudo_ret_float, regOpnd);
    bb.AppendInsn(retInsn);
    pseudoInsns.emplace_back(&retInsn);
  }
}

void AArch64ReachingDefinition::AddRetPseudoInsns() {
  uint32 exitBBSize = cgFunc->GetExitBBsVec().size();
  if (exitBBSize == 0) {
    if (cgFunc->GetLastBB()->GetPrev()->GetFirstStmt() == cgFunc->GetCleanupLabel() &&
        cgFunc->GetLastBB()->GetPrev()->GetPrev()) {
      AddRetPseudoInsn(*cgFunc->GetLastBB()->GetPrev()->GetPrev());
    } else {
      AddRetPseudoInsn(*cgFunc->GetLastBB()->GetPrev());
    }
  } else {
    for (uint32 i = 0; i < exitBBSize; ++i) {
      AddRetPseudoInsn(*cgFunc->GetExitBB(i));
    }
  }
}

/* all caller saved register are modified by call insn */
void AArch64ReachingDefinition::GenAllCallerSavedRegs(BB &bb) {
  for (uint32 i = R0; i <= V31; ++i) {
    if (IsCallerSavedReg(i)) {
      regGen[bb.GetId()]->SetBit(i);
    }
  }
}

/*
 * find definition for register between startInsn and endInsn.
 * startInsn and endInsn must be in same BB and startInsn and endInsn are included
 */
std::vector<Insn*> AArch64ReachingDefinition::FindRegDefBetweenInsn(uint32 regNO, Insn *startInsn,
                                                                    Insn *endInsn) const {
  std::vector<Insn*> defInsnVec;
  if (startInsn == nullptr || endInsn == nullptr) {
    return defInsnVec;
  }

  ASSERT(startInsn->GetBB() == endInsn->GetBB(), "two insns must be in a same BB");
  ASSERT(endInsn->GetId() >= startInsn->GetId(), "two insns must be in a same BB");
  if (!regGen[startInsn->GetBB()->GetId()]->TestBit(regNO)) {
    return defInsnVec;
  }

  for (Insn *insn = endInsn; insn != nullptr && insn != startInsn->GetPrev(); insn = insn->GetPrev()) {
    if (!insn->IsMachineInstruction()) {
      continue;
    }

    const AArch64MD *md = &AArch64CG::kMd[static_cast<AArch64Insn*>(insn)->GetMachineOpcode()];
    if (insn->IsCall() && IsCallerSavedReg(regNO)) {
      defInsnVec.emplace_back(insn);
      return defInsnVec;
    }
    uint32 opndNum = insn->GetOperandSize();
    for (uint32 i = 0; i < opndNum; ++i) {
      Operand &opnd = insn->GetOperand(i);
      AArch64OpndProp *regProp = static_cast<AArch64OpndProp*>(md->operand[i]);
      bool isDef = regProp->IsDef();
      if (!isDef && !opnd.IsMemoryAccessOperand()) {
        continue;
      }

      if (opnd.IsList()) {
        CHECK_FATAL(false, "Internal error, list operand should not be defined.");
      } else if (opnd.IsMemoryAccessOperand()) {
        auto &memOpnd = static_cast<AArch64MemOperand&>(opnd);
        RegOperand *base = memOpnd.GetBaseRegister();

        if (base != nullptr) {
          if (memOpnd.GetAddrMode() == AArch64MemOperand::kAddrModeBOi &&
              (memOpnd.IsPostIndexed() || memOpnd.IsPreIndexed()) &&
              base->GetRegisterNumber() == regNO) {
            defInsnVec.emplace_back(insn);
            return defInsnVec;
          }
        }
      } else if ((opnd.IsConditionCode() || opnd.IsRegister()) &&
                 (static_cast<RegOperand&>(opnd).GetRegisterNumber() == regNO)) {
        defInsnVec.emplace_back(insn);
        return defInsnVec;
      }
    }
  }
  return defInsnVec;
}

void AArch64ReachingDefinition::FindRegDefInBB(uint32 regNO, BB &bb, InsnSet &defInsnSet) const {
  if (!regGen[bb.GetId()]->TestBit(regNO)) {
    return;
  }

  FOR_BB_INSNS(insn, (&bb)) {
    if (!insn->IsMachineInstruction()) {
      continue;
    }

    const AArch64MD *md = &AArch64CG::kMd[static_cast<AArch64Insn*>(insn)->GetMachineOpcode()];
    if (insn->IsCall() && IsCallerSavedReg(regNO)) {
      defInsnSet.insert(insn);
      continue;
    }

    uint32 opndNum = insn->GetOperandSize();
    for (uint32 i = 0; i < opndNum; ++i) {
      Operand &opnd = insn->GetOperand(i);
      AArch64OpndProp *regProp = static_cast<AArch64OpndProp*>(md->GetOperand(i));
      bool isDef = regProp->IsDef();
      if (!isDef && !opnd.IsMemoryAccessOperand()) {
        continue;
      }

      if (opnd.IsList()) {
        ASSERT(false, "Internal error, list operand should not be defined.");
      } else if (opnd.IsMemoryAccessOperand()) {
        auto &memOpnd = static_cast<AArch64MemOperand&>(opnd);
        RegOperand *base = memOpnd.GetBaseRegister();

        if (base != nullptr) {
          if (memOpnd.GetAddrMode() == AArch64MemOperand::kAddrModeBOi &&
              (memOpnd.IsPostIndexed() || memOpnd.IsPreIndexed()) &&
              base->GetRegisterNumber() == regNO) {
            defInsnSet.insert(insn);
          }
        }
      } else if ((opnd.IsConditionCode() || opnd.IsRegister()) &&
                 (static_cast<RegOperand&>(opnd).GetRegisterNumber() == regNO)) {
        defInsnSet.insert(insn);
      }
    }
  }
}

/* check whether call insn changed the stack status or not. */
bool AArch64ReachingDefinition::CallInsnClearDesignateStackRef(const Insn &callInsn, int64 offset) const {
  return offset == callInsn.GetClearStackOffset(kFirstClearMemIndex) ||
         offset == callInsn.GetClearStackOffset(kSecondClearMemIndex);
}

/*
 * find definition for stack memory operand between startInsn and endInsn.
 * startInsn and endInsn must be in same BB and startInsn and endInsn are included
 * special case:
 *   MCC_ClearLocalStackRef clear designate stack position, the designate stack position is thought defined
 *    for example:
 *      add x0, x29, #24
 *      bl MCC_ClearLocalStackRef
 */
std::vector<Insn*> AArch64ReachingDefinition::FindMemDefBetweenInsn(uint32 offset, const Insn *startInsn,
                                                                    Insn *endInsn) const {
  std::vector<Insn*> defInsnVec;
  if (startInsn == nullptr || endInsn == nullptr) {
    return defInsnVec;
  }

  ASSERT(startInsn->GetBB() == endInsn->GetBB(), "two insns must be in a same BB");
  ASSERT(endInsn->GetId() >= startInsn->GetId(), "two insns must be in a same BB");
  if (!memGen[startInsn->GetBB()->GetId()]->TestBit(offset / kMemZoomSize)) {
    return defInsnVec;
  }

  for (Insn *insn = endInsn; insn != nullptr && insn != startInsn->GetPrev(); insn = insn->GetPrev()) {
    if (!insn->IsMachineInstruction()) {
      continue;
    }

    if (insn->IsCall()) {
      if (CallInsnClearDesignateStackRef(*insn, offset)) {
        defInsnVec.emplace_back(insn);
        return defInsnVec;
      }
      continue;
    }

    if (!(insn->IsStore() || insn->IsPseudoInstruction())) {
      continue;
    }

    uint32 opndNum = insn->GetOperandSize();
    for (uint32 i = 0; i < opndNum; ++i) {
      Operand &opnd = insn->GetOperand(i);

      if (opnd.IsMemoryAccessOperand()) {
        auto &memOpnd = static_cast<AArch64MemOperand&>(opnd);
        RegOperand *base = memOpnd.GetBaseRegister();
        RegOperand *index = memOpnd.GetIndexRegister();

        if (base == nullptr || !IsFrameReg(*base) || index != nullptr) {
          break;
        }

        ASSERT(memOpnd.GetOffsetImmediate() != nullptr, "offset must be a immediate value");
        int64 memOffset = memOpnd.GetOffsetImmediate()->GetOffsetValue();
        if ((offset == memOffset) ||
            (insn->IsStorePair() && offset == memOffset + GetEachMemSizeOfPair(insn->GetMachineOpcode()))) {
          defInsnVec.emplace_back(insn);
          return defInsnVec;
        }
      }
    }
  }
  return defInsnVec;
}

void AArch64ReachingDefinition::FindMemDefInBB(uint32 offset, BB &bb, InsnSet &defInsnSet) const {
  if (!memGen[bb.GetId()]->TestBit(offset / kMemZoomSize)) {
    return;
  }

  FOR_BB_INSNS(insn, (&bb)) {
    if (!insn->IsMachineInstruction()) {
      continue;
    }

    if (insn->IsCall()) {
      if (CallInsnClearDesignateStackRef(*insn, offset)) {
        defInsnSet.insert(insn);
      }
      continue;
    }

    if (!(insn->IsStore() || insn->IsPseudoInstruction())) {
      continue;
    }

    uint32 opndNum = insn->GetOperandSize();
    for (uint32 i = 0; i < opndNum; ++i) {
      Operand &opnd = insn->GetOperand(i);
      if (opnd.IsMemoryAccessOperand()) {
        auto &memOpnd = static_cast<AArch64MemOperand&>(opnd);
        RegOperand *base = memOpnd.GetBaseRegister();
        RegOperand *index = memOpnd.GetIndexRegister();

        if (base == nullptr || !IsFrameReg(*base) || index != nullptr) {
          break;
        }

        ASSERT(memOpnd.GetOffsetImmediate() != nullptr, "offset must be a immediate value");
        int64 memOffset = memOpnd.GetOffsetImmediate()->GetOffsetValue();
        if (offset == memOffset) {
          defInsnSet.insert(insn);
          break;
        }
        if (insn->IsStorePair() && offset == memOffset + GetEachMemSizeOfPair(insn->GetMachineOpcode())) {
          defInsnSet.insert(insn);
          break;
        }
      }
    }
  }
}

/*
 * find defininition for register Iteratively.
 *  input:
 *    startBB: find definnition starting from startBB
 *    regNO: the No of register to be find
 *    visitedBB: record these visited BB
 *    defInsnSet: insn defining register is saved in this set
 */
void AArch64ReachingDefinition::DFSFindDefForRegOpnd(const BB &startBB, uint32 regNO,
                                                     std::vector<VisitStatus> &visitedBB, InsnSet &defInsnSet) const {
  std::vector<Insn*> defInsnVec;
  for (auto predBB : startBB.GetPreds()) {
    if (visitedBB[predBB->GetId()] != kNotVisited) {
      continue;
    }
    visitedBB[predBB->GetId()] = kNormalVisited;
    if (regGen[predBB->GetId()]->TestBit(regNO)) {
      defInsnVec.clear();
      defInsnVec = FindRegDefBetweenInsn(regNO, predBB->GetFirstInsn(), predBB->GetLastInsn());
      ASSERT(!defInsnVec.empty(), "opnd must be defined in this bb");
      defInsnSet.insert(defInsnVec.begin(), defInsnVec.end());
    } else if (regIn[predBB->GetId()]->TestBit(regNO)) {
      DFSFindDefForRegOpnd(*predBB, regNO, visitedBB, defInsnSet);
    }
  }

  for (auto predEhBB : startBB.GetEhPreds()) {
    if (visitedBB[predEhBB->GetId()] == kEHVisited) {
      continue;
    }
    visitedBB[predEhBB->GetId()] = kEHVisited;
    if (regGen[predEhBB->GetId()]->TestBit(regNO)) {
      FindRegDefInBB(regNO, *predEhBB, defInsnSet);
    }

    if (regIn[predEhBB->GetId()]->TestBit(regNO)) {
      DFSFindDefForRegOpnd(*predEhBB, regNO, visitedBB, defInsnSet);
    }
  }
}

/*
 * find defininition for stack memory iteratively.
 *  input:
 *    startBB: find definnition starting from startBB
 *    offset: the offset of memory to be find
 *    visitedBB: record these visited BB
 *    defInsnSet: insn defining register is saved in this set
 */
void AArch64ReachingDefinition::DFSFindDefForMemOpnd(const BB &startBB, uint32 offset,
                                                     std::vector<VisitStatus> &visitedBB, InsnSet &defInsnSet) const {
  std::vector<Insn*> defInsnVec;
  for (auto predBB : startBB.GetPreds()) {
    if (visitedBB[predBB->GetId()] != kNotVisited) {
      continue;
    }
    visitedBB[predBB->GetId()] = kNormalVisited;
    if (memGen[predBB->GetId()]->TestBit(offset / kMemZoomSize)) {
      defInsnVec.clear();
      defInsnVec = FindMemDefBetweenInsn(offset, predBB->GetFirstInsn(), predBB->GetLastInsn());
      ASSERT(!defInsnVec.empty(), "opnd must be defined in this bb");
      defInsnSet.insert(defInsnVec.begin(), defInsnVec.end());
    } else if (memIn[predBB->GetId()]->TestBit(offset / kMemZoomSize)) {
      DFSFindDefForMemOpnd(*predBB, offset, visitedBB, defInsnSet);
    }
  }

  for (auto predEhBB : startBB.GetEhPreds()) {
    if (visitedBB[predEhBB->GetId()] == kEHVisited) {
      continue;
    }
    visitedBB[predEhBB->GetId()] = kEHVisited;
    if (memGen[predEhBB->GetId()]->TestBit(offset / kMemZoomSize)) {
      FindMemDefInBB(offset, *predEhBB, defInsnSet);
    }

    if (memIn[predEhBB->GetId()]->TestBit(offset / kMemZoomSize)) {
      DFSFindDefForMemOpnd(*predEhBB, offset, visitedBB, defInsnSet);
    }
  }
}

/*
 * find defininition for register.
 *  input:
 *    insn: the insn in which register is used
 *    indexOrRegNO: the index of register in insn or the No of register to be find
 *    isRegNO: if indexOrRegNO is index, this argument is false, else is true
 *  return:
 *    the set of definition insns for register
 */
InsnSet AArch64ReachingDefinition::FindDefForRegOpnd(Insn &insn, uint32 indexOrRegNO, bool isRegNO) const {
  uint32 regNO = indexOrRegNO;
  if (!isRegNO) {
    Operand &opnd = insn.GetOperand(indexOrRegNO);
    auto &regOpnd = static_cast<RegOperand&>(opnd);
    regNO = regOpnd.GetRegisterNumber();
  }

  std::vector<Insn*> defInsnVec;
  if (regGen[insn.GetBB()->GetId()]->TestBit(regNO)) {
    defInsnVec = FindRegDefBetweenInsn(regNO, insn.GetBB()->GetFirstInsn(), insn.GetPrev());
  }
  InsnSet defInsnSet;
  if (!defInsnVec.empty()) {
    defInsnSet.insert(defInsnVec.begin(), defInsnVec.end());
    return defInsnSet;
  }
  std::vector<VisitStatus> visitedBB(kMaxBBNum, kNotVisited);
  if (insn.GetBB()->IsCleanup()) {
    DFSFindDefForRegOpnd(*insn.GetBB(), regNO, visitedBB, defInsnSet);
    if (defInsnSet.empty()) {
      FOR_ALL_BB(bb, cgFunc) {
        if (bb->IsCleanup()) {
          continue;
        }
        if (regGen[bb->GetId()]->TestBit(regNO)) {
          FindRegDefInBB(regNO, *bb, defInsnSet);
        }
      }
    }
  } else {
    DFSFindDefForRegOpnd(*insn.GetBB(), regNO, visitedBB, defInsnSet);
  }
  return defInsnSet;
}

/*
 * find insn using register between startInsn and endInsn.
 * startInsn and endInsn must be in same BB and startInsn and endInsn are included
 */
bool AArch64ReachingDefinition::FindRegUseBetweenInsn(uint32 regNO, Insn *startInsn, Insn *endInsn,
                                                      InsnSet &regUseInsnSet) const {
  bool findFinish = false;
  if (startInsn == nullptr || endInsn == nullptr) {
    return findFinish;
  }

  ASSERT(startInsn->GetBB() == endInsn->GetBB(), "two insns must be in a same BB");
  ASSERT(endInsn->GetId() >= startInsn->GetId(), "two insns must be in a same BB");

  for (Insn *insn = startInsn; insn != nullptr && insn != endInsn->GetNext(); insn = insn->GetNext()) {
    if (!insn->IsMachineInstruction()) {
      continue;
    }
    /* if insn is call and regNO is caller-saved register, then regNO will not be used later */
    if (insn->IsCall() && IsCallerSavedReg(regNO)) {
      findFinish = true;
    }

    const AArch64MD *md = &AArch64CG::kMd[static_cast<AArch64Insn*>(insn)->GetMachineOpcode()];
    uint32 opndNum = insn->GetOperandSize();
    for (uint32 i = 0; i < opndNum; ++i) {
      Operand &opnd = insn->GetOperand(i);
      if (opnd.IsList()) {
        auto &listOpnd = static_cast<ListOperand&>(opnd);
        for (auto listElem : listOpnd.GetOperands()) {
          RegOperand *regOpnd = static_cast<RegOperand*>(listElem);
          ASSERT(regOpnd != nullptr, "parameter operand must be RegOperand");
          if (regNO == regOpnd->GetRegisterNumber()) {
            regUseInsnSet.insert(insn);
          }
        }
        continue;
      } else if (opnd.IsMemoryAccessOperand()) {
        auto &memOpnd = static_cast<AArch64MemOperand&>(opnd);
        RegOperand *baseOpnd = memOpnd.GetBaseRegister();
        if (baseOpnd != nullptr &&
            (memOpnd.GetAddrMode() == AArch64MemOperand::kAddrModeBOi) &&
            (memOpnd.IsPostIndexed() || memOpnd.IsPreIndexed()) &&
            baseOpnd->GetRegisterNumber() == regNO) {
          findFinish = true;
        }
      }

      AArch64OpndProp *regProp = static_cast<AArch64OpndProp*>(md->operand[i]);
      if (regProp->IsDef() &&
          (opnd.IsConditionCode() || opnd.IsRegister()) &&
          (static_cast<RegOperand&>(opnd).GetRegisterNumber() == regNO)) {
        findFinish = true;
      }

      if (!regProp->IsUse() && !opnd.IsMemoryAccessOperand()) {
        continue;
      }

      if (opnd.IsMemoryAccessOperand()) {
        auto &memOpnd = static_cast<AArch64MemOperand&>(opnd);
        RegOperand *base = memOpnd.GetBaseRegister();
        RegOperand *index = memOpnd.GetIndexRegister();
        if ((base != nullptr && base->GetRegisterNumber() == regNO) ||
            (index != nullptr && index->GetRegisterNumber() == regNO)) {
          regUseInsnSet.insert(insn);
        }
      } else if (opnd.IsConditionCode()) {
        Operand &rflagOpnd = cgFunc->GetOrCreateRflag();
        RegOperand &rflagReg = static_cast<RegOperand&>(rflagOpnd);
        if (rflagReg.GetRegisterNumber() == regNO) {
          regUseInsnSet.insert(insn);
        }
      } else if (opnd.IsRegister() && (static_cast<RegOperand&>(opnd).GetRegisterNumber() == regNO)) {
        regUseInsnSet.insert(insn);
      }
    }

    if (findFinish) {
      break;
    }
  }
  return findFinish;
}

/*
 * find insn using stack memory operand between startInsn and endInsn.
 * startInsn and endInsn must be in same BB and startInsn and endInsn are included
 */
bool AArch64ReachingDefinition::FindMemUseBetweenInsn(uint32 offset, Insn *startInsn, const Insn *endInsn,
                                                      InsnSet &memUseInsnSet) const {
  bool findFinish = false;
  if (startInsn == nullptr || endInsn == nullptr) {
    return findFinish;
  }

  ASSERT(startInsn->GetBB() == endInsn->GetBB(), "two insns must be in a same BB");
  ASSERT(endInsn->GetId() >= startInsn->GetId(), "end ID must be greater than or equal to start ID");

  for (Insn *insn = startInsn; insn != nullptr && insn != endInsn->GetNext(); insn = insn->GetNext()) {
    if (!insn->IsMachineInstruction()) {
      continue;
    }

    if (insn->IsCall()) {
      if (CallInsnClearDesignateStackRef(*insn, offset)) {
        return true;
      }
      continue;
    }

    const AArch64MD *md = &AArch64CG::kMd[static_cast<AArch64Insn*>(insn)->GetMachineOpcode()];
    uint32 opndNum = insn->GetOperandSize();
    for (uint32 i = 0; i < opndNum; ++i) {
      Operand &opnd = insn->GetOperand(i);
      if (!opnd.IsMemoryAccessOperand()) {
        continue;
      }

      auto &memOpnd = static_cast<AArch64MemOperand&>(opnd);
      RegOperand *base = memOpnd.GetBaseRegister();
      if (base == nullptr || !IsFrameReg(*base)) {
        continue;
      }

      ASSERT(memOpnd.GetIndexRegister() == nullptr, "offset must not be Register for frame MemOperand");
      ASSERT(memOpnd.GetOffsetImmediate() != nullptr, "offset must be a immediate value");
      int64 memOffset = memOpnd.GetOffsetImmediate()->GetValue();

      if (insn->IsStore() || insn->IsPseudoInstruction()) {
        if (memOffset == offset) {
          findFinish = true;
          continue;
        }
        if (insn->IsStorePair() && offset == memOffset + GetEachMemSizeOfPair(insn->GetMachineOpcode())) {
          findFinish = true;
          continue;
        }
      }

      AArch64OpndProp *regProp = static_cast<AArch64OpndProp*>(md->operand[i]);
      bool isUse = regProp->IsUse();
      if (!isUse) {
        continue;
      }

      if (offset == memOffset) {
        memUseInsnSet.insert(insn);
      } else if (insn->IsLoadPair() && offset == memOffset + GetEachMemSizeOfPair(insn->GetMachineOpcode())) {
        memUseInsnSet.insert(insn);
      }
    }

    if (findFinish) {
      break;
    }
  }
  return findFinish;
}

/* find all definition for stack memory operand insn.opnd[index] */
InsnSet AArch64ReachingDefinition::FindDefForMemOpnd(Insn &insn, uint32 indexOrOffset, bool isOffset) const {
  InsnSet defInsnSet;
  int64 memOffSet = 0;
  if (!isOffset) {
    Operand &opnd = insn.GetOperand(indexOrOffset);
    ASSERT(opnd.IsMemoryAccessOperand(), "opnd must be MemOperand");

    auto &memOpnd = static_cast<AArch64MemOperand&>(opnd);
    RegOperand *base = memOpnd.GetBaseRegister();
    RegOperand *indexReg = memOpnd.GetIndexRegister();

    if (base == nullptr || !IsFrameReg(*base) || indexReg) {
      return defInsnSet;
    }
    ASSERT(memOpnd.GetOffsetImmediate() != nullptr, "offset must be a immediate value");
    memOffSet = memOpnd.GetOffsetImmediate()->GetOffsetValue();
  } else {
    memOffSet = indexOrOffset;
  }
  std::vector<Insn*> defInsnVec;
  if (memGen[insn.GetBB()->GetId()]->TestBit(memOffSet / kMemZoomSize)) {
    defInsnVec = FindMemDefBetweenInsn(memOffSet, insn.GetBB()->GetFirstInsn(), insn.GetPrev());
  }

  if (!defInsnVec.empty()) {
    defInsnSet.insert(defInsnVec.begin(), defInsnVec.end());
    return defInsnSet;
  }
  std::vector<VisitStatus> visitedBB(kMaxBBNum, kNotVisited);
  if (insn.GetBB()->IsCleanup()) {
    DFSFindDefForMemOpnd(*insn.GetBB(), memOffSet, visitedBB, defInsnSet);
    if (defInsnSet.empty()) {
      FOR_ALL_BB(bb, cgFunc) {
        if (bb->IsCleanup()) {
          continue;
        }

        if (memGen[bb->GetId()]->TestBit(memOffSet / kMemZoomSize)) {
          FindMemDefInBB(memOffSet, *bb, defInsnSet);
        }
      }
    }
  } else {
    DFSFindDefForMemOpnd(*insn.GetBB(), memOffSet, visitedBB, defInsnSet);
  }

  return defInsnSet;
}

/*
 * find all insn using stack memory operand insn.opnd[index]
 * secondMem is used to represent the second stack memory opernad in store pair insn
 */
InsnSet AArch64ReachingDefinition::FindUseForMemOpnd(Insn &insn, uint8 index, bool secondMem) const {
  Operand &opnd = insn.GetOperand(index);
  ASSERT(opnd.IsMemoryAccessOperand(), "opnd must be MemOperand");
  auto &memOpnd = static_cast<AArch64MemOperand&>(opnd);
  RegOperand *base = memOpnd.GetBaseRegister();

  InsnSet useInsnSet;
  if (base == nullptr || !IsFrameReg(*base)) {
    return useInsnSet;
  }

  ASSERT(memOpnd.GetIndexRegister() == nullptr, "IndexRegister no nullptr");
  ASSERT(memOpnd.GetOffsetImmediate() != nullptr, "offset must be a immediate value");
  int64 memOffSet = memOpnd.GetOffsetImmediate()->GetOffsetValue();
  if (secondMem) {
    ASSERT(insn.IsStorePair(), "second MemOperand can only be defined in stp insn");
    memOffSet += GetEachMemSizeOfPair(insn.GetMachineOpcode());
  }
  /* memOperand may be redefined in current BB */
  bool findFinish = FindMemUseBetweenInsn(memOffSet, insn.GetNext(), insn.GetBB()->GetLastInsn(), useInsnSet);
  std::vector<bool> visitedBB(kMaxBBNum, false);
  if (findFinish || !memOut[insn.GetBB()->GetId()]->TestBit(memOffSet / kMemZoomSize)) {
    if (insn.GetBB()->GetEhSuccs().size() != 0) {
      DFSFindUseForMemOpnd(*insn.GetBB(), memOffSet, visitedBB, useInsnSet, true);
    }
  } else {
    DFSFindUseForMemOpnd(*insn.GetBB(), memOffSet, visitedBB, useInsnSet, false);
  }
  if (!insn.GetBB()->IsCleanup() && firstCleanUpBB) {
    if (memUse[firstCleanUpBB->GetId()]->TestBit(memOffSet / kMemZoomSize)) {
      findFinish = FindMemUseBetweenInsn(memOffSet, firstCleanUpBB->GetFirstInsn(),
                                         firstCleanUpBB->GetLastInsn(), useInsnSet);
      if (findFinish || !memOut[firstCleanUpBB->GetId()]->TestBit(memOffSet / kMemZoomSize)) {
        return useInsnSet;
      }
    }
    DFSFindUseForMemOpnd(*firstCleanUpBB, memOffSet, visitedBB, useInsnSet, false);
  }
  return useInsnSet;
}

/*
 * initialize bb.gen and bb.use
 * if it is not computed in first time, bb.gen and bb.use must be cleared firstly
 */
void AArch64ReachingDefinition::InitGenUse(BB &bb, bool firstTime) {
  if (!firstTime && (mode & kRDRegAnalysis)) {
    regGen[bb.GetId()]->ResetAllBit();
    regUse[bb.GetId()]->ResetAllBit();
  }
  if (!firstTime && (mode & kRDMemAnalysis)) {
    memGen[bb.GetId()]->ResetAllBit();
    memUse[bb.GetId()]->ResetAllBit();
  }

  if (bb.IsEmpty()) {
    return;
  }

  FOR_BB_INSNS(insn, (&bb)) {
    if (!insn->IsMachineInstruction()) {
      continue;
    }
    if (insn->IsCall()) {
      GenAllCallerSavedRegs(bb);
      InitMemInfoForClearStackCall(*insn);
    }
    const AArch64MD *md = &AArch64CG::kMd[static_cast<AArch64Insn*>(insn)->GetMachineOpcode()];
    uint32 opndNum = insn->GetOperandSize();
    for (uint32 i = 0; i < opndNum; ++i) {
      Operand &opnd = insn->GetOperand(i);
      AArch64OpndProp *regProp = static_cast<AArch64OpndProp*>(md->operand[i]);
      if (opnd.IsList() && (mode & kRDRegAnalysis)) {
        ASSERT(regProp->IsUse(), "ListOperand is used in insn");
        InitInfoForListOpnd(bb, opnd);
      } else if (opnd.IsMemoryAccessOperand()) {
        InitInfoForMemOperand(*insn, opnd, regProp->IsDef());
      } else if (opnd.IsConditionCode() && (mode & kRDRegAnalysis)) {
        ASSERT(regProp->IsUse(), "condition code is used in insn");
        InitInfoForConditionCode(bb);
      } else if (opnd.IsRegister() && (mode & kRDRegAnalysis)) {
        InitInfoForRegOpnd(bb, opnd, regProp->IsDef());
      }
    }
  }
}

void AArch64ReachingDefinition::InitMemInfoForClearStackCall(Insn &callInsn) {
  if (!(mode & kRDMemAnalysis) || !callInsn.IsClearDesignateStackCall()) {
    return;
  }
  int64 firstOffset = callInsn.GetClearStackOffset(kFirstClearMemIndex);
  constexpr int64 defaultValOfClearMemOffset = -1;
  if (firstOffset != defaultValOfClearMemOffset) {
    memGen[callInsn.GetBB()->GetId()]->SetBit(firstOffset / kMemZoomSize);
  }
  int64 secondOffset = callInsn.GetClearStackOffset(kSecondClearMemIndex);
  if (secondOffset != defaultValOfClearMemOffset) {
    memGen[callInsn.GetBB()->GetId()]->SetBit(secondOffset / kMemZoomSize);
  }
}

void AArch64ReachingDefinition::InitInfoForMemOperand(Insn &insn, Operand &opnd, bool isDef) {
  ASSERT(opnd.IsMemoryAccessOperand(), "opnd must be MemOperand");
  AArch64MemOperand &memOpnd = static_cast<AArch64MemOperand&>(opnd);
  RegOperand *base = memOpnd.GetBaseRegister();
  RegOperand *index = memOpnd.GetIndexRegister();

  if (base == nullptr) {
    return;
  }
  if ((mode & kRDMemAnalysis) && IsFrameReg(*base)) {
    CHECK_FATAL(index == nullptr, "Existing [x29 + index] Memory Address");
    ASSERT(memOpnd.GetOffsetImmediate(), "offset must be a immediate value");
    int32 offsetVal = memOpnd.GetOffsetImmediate()->GetOffsetValue();
    if ((offsetVal % kMemZoomSize) != 0) {
      SetAnalysisMode(kRDRegAnalysis);
    }

    if (!isDef) {
      memUse[insn.GetBB()->GetId()]->SetBit(offsetVal / kMemZoomSize);
      if (insn.IsLoadPair()) {
        int64 nextMemOffset = offsetVal + GetEachMemSizeOfPair(insn.GetMachineOpcode());
        memUse[insn.GetBB()->GetId()]->SetBit(nextMemOffset / kMemZoomSize);
      }
    } else if (isDef) {
      memGen[insn.GetBB()->GetId()]->SetBit(offsetVal / kMemZoomSize);
      if (insn.IsStorePair()) {
        int64 nextMemOffset = offsetVal + GetEachMemSizeOfPair(insn.GetMachineOpcode());
        memGen[insn.GetBB()->GetId()]->SetBit(nextMemOffset / kMemZoomSize);
      }
    }
  }

  if (mode & kRDRegAnalysis) {
    regUse[insn.GetBB()->GetId()]->SetBit(base->GetRegisterNumber());
    if (index != nullptr) {
      regUse[insn.GetBB()->GetId()]->SetBit(index->GetRegisterNumber());
    }
    if (memOpnd.GetAddrMode() == AArch64MemOperand::kAddrModeBOi &&
        (memOpnd.IsPostIndexed() || memOpnd.IsPreIndexed())) {
      /* Base operand has changed. */
      regGen[insn.GetBB()->GetId()]->SetBit(base->GetRegisterNumber());
    }
  }
}

void AArch64ReachingDefinition::InitInfoForListOpnd(const BB &bb, Operand &opnd) {
  ListOperand *listOpnd = static_cast<ListOperand*>(&opnd);
  for (auto listElem : listOpnd->GetOperands()) {
    RegOperand *regOpnd = static_cast<RegOperand*>(listElem);
    ASSERT(regOpnd != nullptr, "used Operand in call insn must be Register");
    regUse[bb.GetId()]->SetBit(regOpnd->GetRegisterNumber());
  }
}

void AArch64ReachingDefinition::InitInfoForConditionCode(const BB &bb) {
  Operand &rflagOpnd = cgFunc->GetOrCreateRflag();
  RegOperand &rflagReg = static_cast<RegOperand&>(rflagOpnd);
  regUse[bb.GetId()]->SetBit(rflagReg.GetRegisterNumber());
}

void AArch64ReachingDefinition::InitInfoForRegOpnd(const BB &bb, Operand &opnd, bool isDef) {
  RegOperand *regOpnd = static_cast<RegOperand*>(&opnd);
  if (!isDef) {
    regUse[bb.GetId()]->SetBit(regOpnd->GetRegisterNumber());
  } else {
    regGen[bb.GetId()]->SetBit(regOpnd->GetRegisterNumber());
  }
}

int32 AArch64ReachingDefinition::GetStackSize() const {
  const int sizeofFplr = kDivide2 * kIntregBytelen;
  return static_cast<AArch64MemLayout*>(cgFunc->GetMemlayout())->RealStackFrameSize() + sizeofFplr;
}

bool AArch64ReachingDefinition::IsCallerSavedReg(uint32 regNO) const {
  return (R0 <= regNO && regNO <= R18) || (V0 <= regNO && regNO <= V7) || (V16 <= regNO && regNO <= V31);
}

int64 AArch64ReachingDefinition::GetEachMemSizeOfPair(MOperator opCode) const {
  switch (opCode) {
    case MOP_wstp:
    case MOP_sstp:
    case MOP_wstlxp:
    case MOP_wldp:
    case MOP_xldpsw:
    case MOP_sldp:
    case MOP_wldaxp:
      return kWordByteNum;
    case MOP_xstp:
    case MOP_dstp:
    case MOP_xstlxp:
    case MOP_xldp:
    case MOP_dldp:
    case MOP_xldaxp:
      return kDoubleWordByteNum;
    default:
      return 0;
  }
}
}  /* namespace maplebe */
