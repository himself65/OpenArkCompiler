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
#include "aarch64_proepilog.h"
#include "cg_option.h"

namespace maplebe {
using namespace maple;

namespace {
constexpr int32 kSoeChckOffset = 8192;

enum RegsPushPop : uint8 {
  kRegsPushOp,
  kRegsPopOp
};

enum PushPopType : uint8 {
  kPushPopSingle = 0,
  kPushPopPair = 1
};

MOperator pushPopOps[kRegsPopOp + 1][kRegTyFloat + 1][kPushPopPair + 1] = {
  { /* push */
    { 0 /* undef */ },
    { /* kRegTyInt */
      MOP_xstr, /* single */
      MOP_xstp, /* pair   */
    },
    { /* kRegTyFloat */
      MOP_dstr, /* single */
      MOP_dstp, /* pair   */
    },
  },
  { /* pop */
    { 0 /* undef */ },
    { /* kRegTyInt */
      MOP_xldr, /* single */
      MOP_xldp, /* pair   */
    },
    { /* kRegTyFloat */
      MOP_dldr, /* single */
      MOP_dldp, /* pair   */
    },
  }
};

inline void AppendInstructionTo(Insn &insn, CGFunc &func) {
  func.GetCurBB()->AppendInsn(insn);
}
}


void AArch64GenProEpilog::GenStackGuard(BB &bb) {
  auto &aarchCGFunc = static_cast<AArch64CGFunc&>(cgFunc);
  CG *currCG = cgFunc.GetCG();
  if (currCG->AddStackGuard()) {
    BB *formerCurBB = cgFunc.GetCurBB();
    aarchCGFunc.GetDummyBB()->ClearInsns();
    cgFunc.SetCurBB(*aarchCGFunc.GetDummyBB());

    MIRSymbol *stkGuardSym = GlobalTables::GetGsymTable().GetSymbolFromStrIdx(
        GlobalTables::GetStrTable().GetStrIdxFromName(std::string("__stack_chk_guard")));
    StImmOperand &stOpnd = aarchCGFunc.CreateStImmOperand(*stkGuardSym, 0, 0);
    AArch64RegOperand &stAddrOpnd =
        aarchCGFunc.GetOrCreatePhysicalRegisterOperand(R9, kSizeOfPtr * kBitsPerByte, kRegTyInt);
    aarchCGFunc.SelectAddrof(stAddrOpnd, stOpnd);

    AArch64MemOperand *guardMemOp =
        aarchCGFunc.GetMemoryPool()->New<AArch64MemOperand>(AArch64MemOperand::kAddrModeBOi, kSizeOfPtr * kBitsPerByte,
            stAddrOpnd, nullptr, &aarchCGFunc.GetOrCreateOfstOpnd(0, k32BitSize), stkGuardSym);
    MOperator mOp = aarchCGFunc.PickLdInsn(k64BitSize, PTY_u64);
    Insn &insn = currCG->BuildInstruction<AArch64Insn>(mOp, stAddrOpnd, *guardMemOp);
    insn.SetDoNotRemove(true);
    cgFunc.GetCurBB()->AppendInsn(insn);

    int32 stkSize = static_cast<AArch64MemLayout*>(cgFunc.GetMemlayout())->RealStackFrameSize() -
                    static_cast<AArch64MemLayout*>(cgFunc.GetMemlayout())->SizeOfArgsToStackPass();
    AArch64MemOperand *downStk = aarchCGFunc.GetMemoryPool()->New<AArch64MemOperand>(RFP, stkSize - kOffset8MemPos,
                                                                                     kSizeOfPtr * kBitsPerByte);
    if (downStk->GetMemVaryType() == kNotVary &&
        aarchCGFunc.IsImmediateOffsetOutOfRange(*downStk, k64BitSize)) {
      downStk = &aarchCGFunc.SplitOffsetWithAddInstruction(*downStk, k64BitSize, R10);
    }
    mOp = aarchCGFunc.PickStInsn(kSizeOfPtr * kBitsPerByte, PTY_u64);
    Insn &tmpInsn = currCG->BuildInstruction<AArch64Insn>(mOp, stAddrOpnd, *downStk);
    tmpInsn.SetDoNotRemove(true);
    cgFunc.GetCurBB()->AppendInsn(tmpInsn);

    bb.InsertAtBeginning(*aarchCGFunc.GetDummyBB());
    cgFunc.SetCurBB(*formerCurBB);
  }
}

BB &AArch64GenProEpilog::GenStackGuardCheckInsn(BB &bb) {
  CG *currCG = cgFunc.GetCG();
  if (!currCG->AddStackGuard()) {
    return bb;
  }

  BB *formerCurBB = cgFunc.GetCurBB();
  cgFunc.GetDummyBB()->ClearInsns();
  cgFunc.SetCurBB(*(cgFunc.GetDummyBB()));
  auto &aarchCGFunc = static_cast<AArch64CGFunc&>(cgFunc);

  const MIRSymbol *stkGuardSym = GlobalTables::GetGsymTable().GetSymbolFromStrIdx(
      GlobalTables::GetStrTable().GetStrIdxFromName(std::string("__stack_chk_guard")));
  StImmOperand &stOpnd = aarchCGFunc.CreateStImmOperand(*stkGuardSym, 0, 0);
  AArch64RegOperand &stAddrOpnd = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(R9, kSizeOfPtr * kBitsPerByte,
                                                                                 kRegTyInt);
  aarchCGFunc.SelectAddrof(stAddrOpnd, stOpnd);

  AArch64MemOperand *guardMemOp =
      cgFunc.GetMemoryPool()->New<AArch64MemOperand>(AArch64MemOperand::kAddrModeBOi,
                                                     kSizeOfPtr * kBitsPerByte, stAddrOpnd, nullptr,
                                                     &aarchCGFunc.GetOrCreateOfstOpnd(0, k32BitSize),
                                                     stkGuardSym);
  MOperator mOp = aarchCGFunc.PickLdInsn(k64BitSize, PTY_u64);
  Insn &insn = currCG->BuildInstruction<AArch64Insn>(mOp, stAddrOpnd, *guardMemOp);
  insn.SetDoNotRemove(true);
  cgFunc.GetCurBB()->AppendInsn(insn);

  AArch64RegOperand &checkOp =
      aarchCGFunc.GetOrCreatePhysicalRegisterOperand(R10, kSizeOfPtr * kBitsPerByte, kRegTyInt);
  int32 stkSize = static_cast<AArch64MemLayout*>(cgFunc.GetMemlayout())->RealStackFrameSize() -
                  static_cast<AArch64MemLayout*>(cgFunc.GetMemlayout())->SizeOfArgsToStackPass();
  AArch64MemOperand *downStk = aarchCGFunc.GetMemoryPool()->New<AArch64MemOperand>(RFP, stkSize - kOffset8MemPos,
                                                                                   kSizeOfPtr * kBitsPerByte);
  if (downStk->GetMemVaryType() == kNotVary && aarchCGFunc.IsImmediateOffsetOutOfRange(*downStk, k64BitSize)) {
    downStk = &aarchCGFunc.SplitOffsetWithAddInstruction(*static_cast<AArch64MemOperand*>(downStk), k64BitSize, R10);
  }
  mOp = aarchCGFunc.PickLdInsn(kSizeOfPtr * kBitsPerByte, PTY_u64);
  Insn &newInsn = currCG->BuildInstruction<AArch64Insn>(mOp, checkOp, *downStk);
  newInsn.SetDoNotRemove(true);
  cgFunc.GetCurBB()->AppendInsn(newInsn);

  cgFunc.SelectBxor(stAddrOpnd, stAddrOpnd, checkOp, PTY_u64);
  LabelIdx failLable = aarchCGFunc.CreateLabel();
  aarchCGFunc.SelectCondGoto(aarchCGFunc.GetOrCreateLabelOperand(failLable), OP_brtrue, OP_eq,
                             stAddrOpnd, aarchCGFunc.CreateImmOperand(0, k64BitSize, false), PTY_u64);

  MIRSymbol *failFunc = GlobalTables::GetGsymTable().GetSymbolFromStrIdx(
      GlobalTables::GetStrTable().GetStrIdxFromName(std::string("__stack_chk_fail")));
  AArch64ListOperand *srcOpnds =
      cgFunc.GetMemoryPool()->New<AArch64ListOperand>(*cgFunc.GetFuncScopeAllocator());
  Insn &callInsn = aarchCGFunc.AppendCall(*failFunc, *srcOpnds);
  callInsn.SetDoNotRemove(true);

  bb.AppendBBInsns(*(cgFunc.GetCurBB()));

  BB *newBB = cgFunc.CreateNewBB(failLable, bb.IsUnreachable(), bb.GetKind(), bb.GetFrequency());
  bb.AppendBB(*newBB);
  if (cgFunc.GetLastBB() == &bb) {
    cgFunc.SetLastBB(*newBB);
  }
  bb.SetKind(BB::kBBFallthru);
  bb.PushBackSuccs(*newBB);
  newBB->PushBackPreds(bb);

  cgFunc.SetCurBB(*formerCurBB);
  return *newBB;
}


AArch64MemOperand *AArch64GenProEpilog::SplitStpLdpOffsetForCalleeSavedWithAddInstruction(const AArch64MemOperand &mo,
                                                                                          uint32 bitLen,
                                                                                          AArch64reg baseRegNum) {
  auto &aarchCGFunc = static_cast<AArch64CGFunc&>(cgFunc);
  CHECK_FATAL(mo.GetAddrMode() == AArch64MemOperand::kAddrModeBOi, "mode should be kAddrModeBOi");
  AArch64OfstOperand *ofstOp = mo.GetOffsetImmediate();
  int32 offsetVal = ofstOp->GetOffsetValue();
  CHECK_FATAL(offsetVal > 0, "offsetVal should be greater than 0");
  CHECK_FATAL((static_cast<uint32>(offsetVal) & 0x7) == 0, "(offsetVal & 0x7) should be equal to 0");
  /*
   * Offset adjustment due to FP/SP has already been done
   * in AArch64GenProEpilog::GeneratePushRegs() and AArch64GenProEpilog::GeneratePopRegs()
   */
  AArch64RegOperand &br = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(baseRegNum, bitLen, kRegTyInt);
  if (aarchCGFunc.GetSplitBaseOffset() == 0) {
    aarchCGFunc.SetSplitBaseOffset(offsetVal);  /* remember the offset; don't forget to clear it */
    ImmOperand &immAddEnd = aarchCGFunc.CreateImmOperand(offsetVal, k64BitSize, true);
    RegOperand *origBaseReg = mo.GetBaseRegister();
    aarchCGFunc.SelectAdd(br, *origBaseReg, immAddEnd, PTY_i64);
  }
  offsetVal = offsetVal - aarchCGFunc.GetSplitBaseOffset();
  return &aarchCGFunc.CreateReplacementMemOperand(bitLen, br, offsetVal);
}

void AArch64GenProEpilog::AppendInstructionPushPair(AArch64reg reg0, AArch64reg reg1, RegType rty, int32 offset) {
  auto &aarchCGFunc = static_cast<AArch64CGFunc&>(cgFunc);
  CG *currCG = cgFunc.GetCG();
  MOperator mOp = pushPopOps[kRegsPushOp][rty][kPushPopPair];
  Operand &o0 = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(reg0, kSizeOfPtr * kBitsPerByte, rty);
  Operand &o1 = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(reg1, kSizeOfPtr * kBitsPerByte, rty);
  Operand *o2 = &aarchCGFunc.CreateStkTopOpnd(offset, kSizeOfPtr * kBitsPerByte);

  uint32 dataSize = kSizeOfPtr * kBitsPerByte;
  CHECK_FATAL(offset >= 0, "offset must >= 0");
  if (offset > kStpLdpImm64UpperBound) {
    o2 = SplitStpLdpOffsetForCalleeSavedWithAddInstruction(*static_cast<AArch64MemOperand*>(o2), dataSize, R16);
  }
  Insn &pushInsn = currCG->BuildInstruction<AArch64Insn>(mOp, o0, o1, *o2);
  std::string comment = "SAVE CALLEE REGISTER PAIR";
  pushInsn.SetComment(comment);
  AppendInstructionTo(pushInsn, cgFunc);

  /* Append CFi code */
  if (!CGOptions::IsNoCalleeCFI()) {
    int32 stackFrameSize = static_cast<AArch64MemLayout*>(cgFunc.GetMemlayout())->RealStackFrameSize();
    stackFrameSize -= cgFunc.GetMemlayout()->SizeOfArgsToStackPass();
    int32 cfiOffset = stackFrameSize - offset;
    BB *curBB = cgFunc.GetCurBB();
    Insn *newInsn = curBB->InsertInsnAfter(pushInsn, aarchCGFunc.CreateCfiOffsetInsn(reg0, -cfiOffset, k64BitSize));
    curBB->InsertInsnAfter(*newInsn, aarchCGFunc.CreateCfiOffsetInsn(reg1, -cfiOffset + kOffset8MemPos, k64BitSize));
  }
}

void AArch64GenProEpilog::AppendInstructionPushSingle(AArch64reg reg, RegType rty, int32 offset) {
  auto &aarchCGFunc = static_cast<AArch64CGFunc&>(cgFunc);
  CG *currCG = cgFunc.GetCG();
  MOperator mOp = pushPopOps[kRegsPushOp][rty][kPushPopSingle];
  Operand &o0 = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(reg, kSizeOfPtr * kBitsPerByte, rty);
  Operand *o1 = &aarchCGFunc.CreateStkTopOpnd(offset, kSizeOfPtr * kBitsPerByte);

  AArch64MemOperand *aarchMemO1 = static_cast<AArch64MemOperand*>(o1);
  uint32 dataSize = kSizeOfPtr * kBitsPerByte;
  if (aarchMemO1->GetMemVaryType() == kNotVary &&
      aarchCGFunc.IsImmediateOffsetOutOfRange(*aarchMemO1, dataSize)) {
    o1 = &aarchCGFunc.SplitOffsetWithAddInstruction(*aarchMemO1, dataSize, R9);
  }

  Insn &pushInsn = currCG->BuildInstruction<AArch64Insn>(mOp, o0, *o1);
  std::string comment = "SAVE CALLEE REGISTER";
  pushInsn.SetComment(comment);
  AppendInstructionTo(pushInsn, cgFunc);

  /* Append CFI code */
  if (!CGOptions::IsNoCalleeCFI()) {
    int32 stackFrameSize = static_cast<AArch64MemLayout*>(cgFunc.GetMemlayout())->RealStackFrameSize();
    stackFrameSize -= cgFunc.GetMemlayout()->SizeOfArgsToStackPass();
    int32 cfiOffset = stackFrameSize - offset;
    cgFunc.GetCurBB()->InsertInsnAfter(pushInsn,
                                       aarchCGFunc.CreateCfiOffsetInsn(reg, -cfiOffset, k64BitSize));
  }
}

Insn &AArch64GenProEpilog::AppendInstructionForAllocateOrDeallocateCallFrame(int64 argsToStkPassSize,
                                                                             AArch64reg reg0, AArch64reg reg1,
                                                                             RegType rty, bool isAllocate) {
  auto &aarchCGFunc = static_cast<AArch64CGFunc&>(cgFunc);
  CG *currCG = cgFunc.GetCG();
  MOperator mOp = isAllocate ? pushPopOps[kRegsPushOp][rty][kPushPopPair] : pushPopOps[kRegsPopOp][rty][kPushPopPair];
  if (argsToStkPassSize <= kStrLdrImm64UpperBound - kOffset8MemPos) {
    mOp = isAllocate ? pushPopOps[kRegsPushOp][rty][kPushPopSingle] : pushPopOps[kRegsPopOp][rty][kPushPopSingle];
    AArch64RegOperand &o0 = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(reg0, kSizeOfPtr * kBitsPerByte, rty);
    AArch64MemOperand *o2 = aarchCGFunc.GetMemoryPool()->New<AArch64MemOperand>(RSP, argsToStkPassSize,
                                                                                kSizeOfPtr * kBitsPerByte);
    Insn &insn1 = currCG->BuildInstruction<AArch64Insn>(mOp, o0, *o2);
    AppendInstructionTo(insn1, cgFunc);
    AArch64RegOperand &o1 = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(reg1, kSizeOfPtr * kBitsPerByte, rty);
    o2 = aarchCGFunc.GetMemoryPool()->New<AArch64MemOperand>(RSP, argsToStkPassSize + kSizeOfPtr,
                                                             kSizeOfPtr * kBitsPerByte);
    Insn &insn2 = currCG->BuildInstruction<AArch64Insn>(mOp, o1, *o2);
    AppendInstructionTo(insn2, cgFunc);
    return insn2;
  } else {
    AArch64RegOperand &oo = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(R9, kSizeOfPtr * kBitsPerByte, kRegTyInt);
    AArch64ImmOperand &io1 = aarchCGFunc.CreateImmOperand(argsToStkPassSize, k64BitSize, true);
    aarchCGFunc.SelectCopyImm(oo, io1, PTY_i64);
    AArch64RegOperand &o0 = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(reg0, kSizeOfPtr * kBitsPerByte, rty);
    AArch64RegOperand &rsp = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(RSP, kSizeOfPtr * kBitsPerByte, kRegTyInt);
    AArch64MemOperand *mo = aarchCGFunc.GetMemoryPool()->New<AArch64MemOperand>(
        AArch64MemOperand::kAddrModeBOrX, kSizeOfPtr * kBitsPerByte, rsp, oo, 0);
    Insn &insn1 = currCG->BuildInstruction<AArch64Insn>(isAllocate ? MOP_xstr : MOP_xldr, o0, *mo);
    AppendInstructionTo(insn1, cgFunc);
    AArch64ImmOperand &io2 = aarchCGFunc.CreateImmOperand(kSizeOfPtr, k64BitSize, true);
    aarchCGFunc.SelectAdd(oo, oo, io2, PTY_i64);
    AArch64RegOperand &o1 = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(reg1, kSizeOfPtr * kBitsPerByte, rty);
    mo = aarchCGFunc.GetMemoryPool()->New<AArch64MemOperand>(AArch64MemOperand::kAddrModeBOrX,
                                                             kSizeOfPtr * kBitsPerByte, rsp, oo, 0);
    Insn &insn2 = currCG->BuildInstruction<AArch64Insn>(isAllocate ? MOP_xstr : MOP_xldr, o1, *mo);
    AppendInstructionTo(insn2, cgFunc);
    return insn2;
  }
}

Insn &AArch64GenProEpilog::CreateAndAppendInstructionForAllocateCallFrame(int64 argsToStkPassSize,
                                                                          AArch64reg reg0, AArch64reg reg1,
                                                                          RegType rty) {
  auto &aarchCGFunc = static_cast<AArch64CGFunc&>(cgFunc);
  CG *currCG = cgFunc.GetCG();
  MOperator mOp = pushPopOps[kRegsPushOp][rty][kPushPopPair];
  Insn *allocInsn = nullptr;
  if (argsToStkPassSize > kStpLdpImm64UpperBound) {
    allocInsn = &AppendInstructionForAllocateOrDeallocateCallFrame(argsToStkPassSize, reg0, reg1, rty, true);
  } else {
    Operand &o0 = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(reg0, kSizeOfPtr * kBitsPerByte, rty);
    Operand &o1 = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(reg1, kSizeOfPtr * kBitsPerByte, rty);
    Operand *o2 = aarchCGFunc.GetMemoryPool()->New<AArch64MemOperand>(RSP, argsToStkPassSize,
                                                                      kSizeOfPtr * kBitsPerByte);
    allocInsn = &currCG->BuildInstruction<AArch64Insn>(mOp, o0, o1, *o2);
    AppendInstructionTo(*allocInsn, cgFunc);
  }
  if (currCG->NeedInsertInstrumentationFunction()) {
    aarchCGFunc.AppendCall(*currCG->GetInstrumentationFunction());
  } else if (currCG->InstrumentWithDebugTraceCall()) {
    aarchCGFunc.AppendCall(*currCG->GetDebugTraceEnterFunction());
  } else if (currCG->InstrumentWithProfile()) {
    aarchCGFunc.AppendCall(*currCG->GetProfileFunction());
  }
  return *allocInsn;
}

void AArch64GenProEpilog::AppendInstructionAllocateCallFrame(AArch64reg reg0, AArch64reg reg1, RegType rty) {
  auto &aarchCGFunc = static_cast<AArch64CGFunc&>(cgFunc);
  CG *currCG = cgFunc.GetCG();
  if (currCG->GenerateVerboseAsm()) {
    cgFunc.GetCurBB()->AppendInsn(aarchCGFunc.CreateCommentInsn("allocate activation frame"));
  }

  Insn *ipoint = nullptr;
  /*
   * stackFrameSize includes the size of args to stack-pass
   * if a function has neither VLA nor alloca.
   */
  int32 stackFrameSize = static_cast<AArch64MemLayout*>(cgFunc.GetMemlayout())->RealStackFrameSize();
  int64 argsToStkPassSize = cgFunc.GetMemlayout()->SizeOfArgsToStackPass();
  /*
   * ldp/stp's imm should be within -512 and 504;
   * if stp's imm > 512, we fall back to the stp-sub version
   */
  bool useStpSub = false;
  int64 offset = 0;
  int32 cfiOffset = 0;
  if (!cgFunc.HasVLAOrAlloca() && argsToStkPassSize > 0) {
    /*
     * stack_frame_size == size of formal parameters + callee-saved (including FP/RL)
     *                     + size of local vars
     *                     + size of actuals
     * (when passing more than 8 args, its caller's responsibility to
     *  allocate space for it. size of actuals represent largest such size in the function.
     */
    Operand &spOpnd = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(RSP, k64BitSize, kRegTyInt);
    Operand &immOpnd = aarchCGFunc.CreateImmOperand(stackFrameSize, k32BitSize, true);
    aarchCGFunc.SelectSub(spOpnd, spOpnd, immOpnd, PTY_u64);
    ipoint = cgFunc.GetCurBB()->GetLastInsn();
    cfiOffset = stackFrameSize;
  } else {
    if (stackFrameSize > kStpLdpImm64UpperBound) {
      useStpSub = true;
      offset = kOffset16MemPos;
      stackFrameSize -= offset;
    } else {
      offset = stackFrameSize;
    }
    MOperator mOp = pushPopOps[kRegsPushOp][rty][kPushPopPair];
    AArch64RegOperand &o0 = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(reg0, kSizeOfPtr * kBitsPerByte, rty);
    AArch64RegOperand &o1 = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(reg1, kSizeOfPtr * kBitsPerByte, rty);
    AArch64MemOperand &o2 = aarchCGFunc.CreateCallFrameOperand(-offset, kSizeOfPtr * kBitsPerByte);
    ipoint = &currCG->BuildInstruction<AArch64Insn>(mOp, o0, o1, o2);
    AppendInstructionTo(*ipoint, cgFunc);
    cfiOffset = offset;
    if (currCG->NeedInsertInstrumentationFunction()) {
      aarchCGFunc.AppendCall(*currCG->GetInstrumentationFunction());
    } else if (currCG->InstrumentWithDebugTraceCall()) {
      aarchCGFunc.AppendCall(*currCG->GetDebugTraceEnterFunction());
    } else if (currCG->InstrumentWithProfile()) {
      aarchCGFunc.AppendCall(*currCG->GetProfileFunction());
    }
  }

  ipoint = InsertCFIDefCfaOffset(cfiOffset, *ipoint);

  if (!cgFunc.HasVLAOrAlloca() && argsToStkPassSize > 0) {
    CHECK_FATAL(!useStpSub, "Invalid assumption");
    ipoint = &CreateAndAppendInstructionForAllocateCallFrame(argsToStkPassSize, reg0, reg1, rty);
  }

  if (useStpSub) {
    Operand &spOpnd = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(RSP, k64BitSize, kRegTyInt);
    Operand &immOpnd = aarchCGFunc.CreateImmOperand(stackFrameSize, k32BitSize, true);
    aarchCGFunc.SelectSub(spOpnd, spOpnd, immOpnd, PTY_u64);
    ipoint = cgFunc.GetCurBB()->GetLastInsn();
    aarchCGFunc.SetUsedStpSubPairForCallFrameAllocation(true);
  }

  CHECK_FATAL(ipoint != nullptr, "ipoint should not be nullptr at this point");
  int32 cfiOffsetSecond = 0;
  if (useStpSub) {
    cfiOffsetSecond = stackFrameSize;
    ipoint = InsertCFIDefCfaOffset(cfiOffsetSecond, *ipoint);
  }
  cfiOffsetSecond = GetOffsetFromCFA();
  if (!cgFunc.HasVLAOrAlloca()) {
    cfiOffsetSecond -= argsToStkPassSize;
  }
  BB *curBB = cgFunc.GetCurBB();
  ipoint = curBB->InsertInsnAfter(*ipoint, aarchCGFunc.CreateCfiOffsetInsn(RFP, -cfiOffsetSecond, k64BitSize));
  curBB->InsertInsnAfter(*ipoint, aarchCGFunc.CreateCfiOffsetInsn(RLR, -cfiOffsetSecond + kOffset8MemPos, k64BitSize));
}

void AArch64GenProEpilog::AppendInstructionAllocateCallFrameDebug(AArch64reg reg0, AArch64reg reg1, RegType rty) {
  auto &aarchCGFunc = static_cast<AArch64CGFunc&>(cgFunc);
  CG *currCG = cgFunc.GetCG();
  if (currCG->GenerateVerboseAsm()) {
    cgFunc.GetCurBB()->AppendInsn(aarchCGFunc.CreateCommentInsn("allocate activation frame for debugging"));
  }

  int32 stackFrameSize = static_cast<AArch64MemLayout*>(cgFunc.GetMemlayout())->RealStackFrameSize();
  int64 argsToStkPassSize = cgFunc.GetMemlayout()->SizeOfArgsToStackPass();

  Insn *ipoint = nullptr;
  int32 cfiOffset = 0;

  if (argsToStkPassSize > 0) {
    Operand &spOpnd = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(RSP, k64BitSize, kRegTyInt);
    Operand &immOpnd = aarchCGFunc.CreateImmOperand(stackFrameSize, k32BitSize, true);
    aarchCGFunc.SelectSub(spOpnd, spOpnd, immOpnd, PTY_u64);
    ipoint = cgFunc.GetCurBB()->GetLastInsn();
    cfiOffset = stackFrameSize;
    (void)InsertCFIDefCfaOffset(cfiOffset, *ipoint);
    ipoint = &CreateAndAppendInstructionForAllocateCallFrame(argsToStkPassSize, reg0, reg1, rty);
    CHECK_FATAL(ipoint != nullptr, "ipoint should not be nullptr at this point");
    cfiOffset = GetOffsetFromCFA();
    cfiOffset -= argsToStkPassSize;
  } else {
    bool useStpSub = false;

    if (stackFrameSize > kStpLdpImm64UpperBound) {
      useStpSub = true;
      AArch64RegOperand &spOpnd = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(RSP, k64BitSize, kRegTyInt);
      ImmOperand &immOpnd = aarchCGFunc.CreateImmOperand(stackFrameSize, k32BitSize, true);
      aarchCGFunc.SelectSub(spOpnd, spOpnd, immOpnd, PTY_u64);
      ipoint = cgFunc.GetCurBB()->GetLastInsn();
      cfiOffset = stackFrameSize;
      ipoint = InsertCFIDefCfaOffset(cfiOffset, *ipoint);
    } else {
      MOperator mOp = pushPopOps[kRegsPushOp][rty][kPushPopPair];
      AArch64RegOperand &o0 = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(reg0, kSizeOfPtr * kBitsPerByte, rty);
      AArch64RegOperand &o1 = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(reg1, kSizeOfPtr * kBitsPerByte, rty);
      AArch64MemOperand &o2 = aarchCGFunc.CreateCallFrameOperand(-stackFrameSize, kSizeOfPtr * kBitsPerByte);
      ipoint = &currCG->BuildInstruction<AArch64Insn>(mOp, o0, o1, o2);
      AppendInstructionTo(*ipoint, cgFunc);
      cfiOffset = stackFrameSize;
      ipoint = InsertCFIDefCfaOffset(cfiOffset, *ipoint);
    }

    if (useStpSub) {
      MOperator mOp = pushPopOps[kRegsPushOp][rty][kPushPopPair];
      AArch64RegOperand &o0 = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(reg0, kSizeOfPtr * kBitsPerByte, rty);
      AArch64RegOperand &o1 = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(reg1, kSizeOfPtr * kBitsPerByte, rty);
      AArch64MemOperand *o2 = aarchCGFunc.GetMemoryPool()->New<AArch64MemOperand>(RSP, 0, kSizeOfPtr * kBitsPerByte);
      ipoint = &currCG->BuildInstruction<AArch64Insn>(mOp, o0, o1, *o2);
      AppendInstructionTo(*ipoint, cgFunc);
    }

    if (currCG->NeedInsertInstrumentationFunction()) {
      aarchCGFunc.AppendCall(*currCG->GetInstrumentationFunction());
    } else if (currCG->InstrumentWithDebugTraceCall()) {
      aarchCGFunc.AppendCall(*currCG->GetDebugTraceEnterFunction());
    } else if (currCG->InstrumentWithProfile()) {
      aarchCGFunc.AppendCall(*currCG->GetProfileFunction());
    }

    CHECK_FATAL(ipoint != nullptr, "ipoint should not be nullptr at this point");
    cfiOffset = GetOffsetFromCFA();
  }
  BB *curBB = cgFunc.GetCurBB();
  ipoint = curBB->InsertInsnAfter(*ipoint, aarchCGFunc.CreateCfiOffsetInsn(RFP, -cfiOffset, k64BitSize));
  curBB->InsertInsnAfter(*ipoint, aarchCGFunc.CreateCfiOffsetInsn(RLR, -cfiOffset + kOffset8MemPos, k64BitSize));
}

/*
 *  From AArch64 Reference Manual
 *  C1.3.3 Load/Store Addressing Mode
 *  ...
 *  When stack alignment checking is enabled by system software and
 *  the base register is the SP, the current stack pointer must be
 *  initially quadword aligned, that is aligned to 16 bytes. Misalignment
 *  generates a Stack Alignment fault.  The offset does not have to
 *  be a multiple of 16 bytes unless the specific Load/Store instruction
 *  requires this. SP cannot be used as a register offset.
 */
void AArch64GenProEpilog::GeneratePushRegs() {
  auto &aarchCGFunc = static_cast<AArch64CGFunc&>(cgFunc);
  CG *currCG = cgFunc.GetCG();
  const MapleVector<AArch64reg> &regsToSave = aarchCGFunc.GetCalleeSavedRegs();

  CHECK_FATAL(!regsToSave.empty(), "FP/LR not added to callee-saved list?");

  AArch64reg intRegFirstHalf = kRinvalid;
  AArch64reg fpRegFirstHalf = kRinvalid;

  if (currCG->GenerateVerboseAsm()) {
    cgFunc.GetCurBB()->AppendInsn(aarchCGFunc.CreateCommentInsn("save callee-saved registers"));
  }

  /*
   * Even if we don't use RFP, since we push a pair of registers in one instruction
   * and the stack needs be aligned on a 16-byte boundary, push RFP as well if function has a call
   * Make sure this is reflected when computing callee_saved_regs.size()
   */
  if (!currCG->GenerateDebugFriendlyCode()) {
    AppendInstructionAllocateCallFrame(RFP, RLR, kRegTyInt);
  } else {
    AppendInstructionAllocateCallFrameDebug(RFP, RLR, kRegTyInt);
  }

  if (currCG->GenerateVerboseAsm()) {
    cgFunc.GetCurBB()->AppendInsn(aarchCGFunc.CreateCommentInsn("copy SP to FP"));
  }
  Operand &spOpnd = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(RSP, k64BitSize, kRegTyInt);
  Operand &fpOpnd = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(RFP, k64BitSize, kRegTyInt);
  int64 argsToStkPassSize = cgFunc.GetMemlayout()->SizeOfArgsToStackPass();
  if (argsToStkPassSize > 0) {
    Operand &immOpnd = aarchCGFunc.CreateImmOperand(argsToStkPassSize, k32BitSize, true);
    aarchCGFunc.SelectAdd(fpOpnd, spOpnd, immOpnd, PTY_u64);
    cgFunc.GetCurBB()->GetLastInsn()->SetFrameDef(true);
    cgFunc.GetCurBB()->AppendInsn(aarchCGFunc.CreateCfiDefCfaInsn(
        RFP, static_cast<AArch64MemLayout*>(cgFunc.GetMemlayout())->RealStackFrameSize() - argsToStkPassSize,
        k64BitSize));
  } else {
    aarchCGFunc.SelectCopy(fpOpnd, PTY_u64, spOpnd, PTY_u64);
    cgFunc.GetCurBB()->GetLastInsn()->SetFrameDef(true);
    cgFunc.GetCurBB()->AppendInsn(currCG->BuildInstruction<cfi::CfiInsn>(cfi::OP_CFI_def_cfa_register,
        aarchCGFunc.CreateCfiRegOperand(RFP, k64BitSize)));
  }

  MapleVector<AArch64reg>::const_iterator it = regsToSave.begin();
  /* skip the first two registers */
  CHECK_FATAL(*it == RFP, "The first callee saved reg is expected to be RFP");
  ++it;
  CHECK_FATAL(*it == RLR, "The second callee saved reg is expected to be RLR");
  ++it;

  int32 offset = static_cast<AArch64MemLayout*>(cgFunc.GetMemlayout())->RealStackFrameSize() -
                 (aarchCGFunc.SizeOfCalleeSaved() - (kDivide2 * kIntregBytelen) /* for FP/LR */) -
                 cgFunc.GetMemlayout()->SizeOfArgsToStackPass();

  for (; it != regsToSave.end(); ++it) {
    AArch64reg reg = *it;
    CHECK_FATAL(reg != RFP, "stray RFP in callee_saved_list?");
    CHECK_FATAL(reg != RLR, "stray RLR in callee_saved_list?");

    RegType regType = AArch64isa::IsGPRegister(reg) ? kRegTyInt : kRegTyFloat;
    AArch64reg &firstHalf = AArch64isa::IsGPRegister(reg) ? intRegFirstHalf : fpRegFirstHalf;
    if (firstHalf == kRinvalid) {
      /* remember it */
      firstHalf = reg;
    } else {
      AppendInstructionPushPair(firstHalf, reg, regType, offset);
      GetNextOffsetCalleeSaved(offset);
      firstHalf = kRinvalid;
    }
  }

  if (intRegFirstHalf != kRinvalid) {
    AppendInstructionPushSingle(intRegFirstHalf, kRegTyInt, offset);
    GetNextOffsetCalleeSaved(offset);
  }

  if (fpRegFirstHalf != kRinvalid) {
    AppendInstructionPushSingle(fpRegFirstHalf, kRegTyFloat, offset);
    GetNextOffsetCalleeSaved(offset);
  }

  /*
   * in case we split stp/ldp instructions,
   * so that we generate a load-into-base-register instruction
   * for pop pairs as well.
   */
  aarchCGFunc.SetSplitBaseOffset(0);
}

void AArch64GenProEpilog::AppendInstructionStackCheck(AArch64reg reg, RegType rty, int32 offset) {
  auto &aarchCGFunc = static_cast<AArch64CGFunc&>(cgFunc);
  CG *currCG = cgFunc.GetCG();
  /* sub x16, sp, #0x2000 */
  auto &x16Opnd = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(reg, k64BitSize, rty);
  auto &spOpnd = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(RSP, k64BitSize, rty);
  auto &imm1 = aarchCGFunc.CreateImmOperand(offset, k64BitSize, true);
  aarchCGFunc.SelectSub(x16Opnd, spOpnd, imm1, PTY_u64);

  /* ldr wzr, [x16] */
  auto &wzr = AArch64RegOperand::Get32bitZeroRegister();
  auto &refX16 = aarchCGFunc.CreateMemOpnd(reg, 0, k64BitSize);
  auto &soeInstr = currCG->BuildInstruction<AArch64Insn>(MOP_wldr, wzr, refX16);
  if (currCG->GenerateVerboseAsm()) {
    soeInstr.SetComment("soerror");
  }
  soeInstr.SetDoNotRemove(true);
  AppendInstructionTo(soeInstr, cgFunc);
}

void AArch64GenProEpilog::GenerateProlog(BB &bb) {
  auto &aarchCGFunc = static_cast<AArch64CGFunc&>(cgFunc);
  CG *currCG = cgFunc.GetCG();
  BB *formerCurBB = cgFunc.GetCurBB();
  aarchCGFunc.GetDummyBB()->ClearInsns();
  cgFunc.SetCurBB(*aarchCGFunc.GetDummyBB());
  Operand &spOpnd = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(RSP, k64BitSize, kRegTyInt);
  Operand &fpOpnd = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(RFP, k64BitSize, kRegTyInt);
  if (!cgFunc.GetHasProEpilogue()) {
    return;
  }

  const MapleVector<AArch64reg> &regsToSave = aarchCGFunc.GetCalleeSavedRegs();
  if (!regsToSave.empty()) {
    /*
     * Among other things, push the FP & LR pair.
     * FP/LR are added to the callee-saved list in AllocateRegisters()
     * We add them to the callee-saved list regardless of UseFP() being true/false.
     * Activation Frame is allocated as part of pushing FP/LR pair
     */
    GeneratePushRegs();
  } else {
    int32 stackFrameSize = static_cast<AArch64MemLayout*>(cgFunc.GetMemlayout())->RealStackFrameSize();
    if (stackFrameSize > 0) {
      if (currCG->GenerateVerboseAsm()) {
        cgFunc.GetCurBB()->AppendInsn(aarchCGFunc.CreateCommentInsn("allocate activation frame"));
      }
      Operand &immOpnd = aarchCGFunc.CreateImmOperand(stackFrameSize, k32BitSize, true);
      aarchCGFunc.SelectSub(spOpnd, spOpnd, immOpnd, PTY_u64);

      int32 offset = stackFrameSize;
      (void)InsertCFIDefCfaOffset(offset, *(cgFunc.GetCurBB()->GetLastInsn()));
    }
    if (currCG->GenerateVerboseAsm()) {
      cgFunc.GetCurBB()->AppendInsn(aarchCGFunc.CreateCommentInsn("copy SP to FP"));
    }
    int64 argsToStkPassSize = cgFunc.GetMemlayout()->SizeOfArgsToStackPass();
    if (argsToStkPassSize > 0) {
      Operand &immOpnd = aarchCGFunc.CreateImmOperand(argsToStkPassSize, k32BitSize, true);
      aarchCGFunc.SelectAdd(fpOpnd, spOpnd, immOpnd, PTY_u64);
      cgFunc.GetCurBB()->GetLastInsn()->SetFrameDef(true);
      cgFunc.GetCurBB()->AppendInsn(aarchCGFunc.CreateCfiDefCfaInsn(
          RFP, static_cast<AArch64MemLayout*>(cgFunc.GetMemlayout())->RealStackFrameSize() - argsToStkPassSize,
          k64BitSize));
    } else {
      aarchCGFunc.SelectCopy(fpOpnd, PTY_u64, spOpnd, PTY_u64);
      cgFunc.GetCurBB()->GetLastInsn()->SetFrameDef(true);
      cgFunc.GetCurBB()->AppendInsn(
          currCG->BuildInstruction<cfi::CfiInsn>(cfi::OP_CFI_def_cfa_register,
                                                 aarchCGFunc.CreateCfiRegOperand(RFP, k64BitSize)));
    }
  }
  if (currCG->DoCheckSOE()) {
    AppendInstructionStackCheck(R16, kRegTyInt, kSoeChckOffset);
  }
  bb.InsertAtBeginning(*aarchCGFunc.GetDummyBB());
  cgFunc.SetCurBB(*formerCurBB);
}

void AArch64GenProEpilog::GenerateRet(BB &bb) {
  CG *currCG = cgFunc.GetCG();
  bb.AppendInsn(currCG->BuildInstruction<AArch64Insn>(MOP_xret));
}

/*
 * If all the preds of exitBB made the TailcallOpt(replace blr/bl with br/b), return true, we don't create ret insn.
 * Otherwise, return false, create the ret insn.
 */
bool AArch64GenProEpilog::TestPredsOfRetBB(const BB &exitBB) {
  for (auto tmpBB : exitBB.GetPreds()) {
    Insn *firstInsn = tmpBB->GetFirstInsn();
    if ((firstInsn == nullptr || tmpBB->IsCommentBB()) && (!tmpBB->GetPreds().empty())) {
      if (!TestPredsOfRetBB(*tmpBB)) {
        return false;
      }
    } else {
      Insn *lastInsn = tmpBB->GetLastInsn();
      if (lastInsn == nullptr) {
        return false;
      }
      MOperator insnMop = lastInsn->GetMachineOpcode();
      if (insnMop != MOP_tail_call_opt_xbl && insnMop != MOP_tail_call_opt_xblr) {
        return false;
      }
    }
  }
  return true;
}

void AArch64GenProEpilog::AppendInstructionPopSingle(AArch64reg reg, RegType rty, int32 offset) {
  auto &aarchCGFunc = static_cast<AArch64CGFunc&>(cgFunc);
  CG *currCG = cgFunc.GetCG();
  MOperator mOp = pushPopOps[kRegsPopOp][rty][kPushPopSingle];
  Operand &o0 = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(reg, kSizeOfPtr * kBitsPerByte, rty);
  Operand *o1 = &aarchCGFunc.CreateStkTopOpnd(offset, kSizeOfPtr * kBitsPerByte);
  AArch64MemOperand *aarchMemO1 = static_cast<AArch64MemOperand*>(o1);
  uint32 dataSize = kSizeOfPtr * kBitsPerByte;
  if (aarchMemO1->GetMemVaryType() == kNotVary && aarchCGFunc.IsImmediateOffsetOutOfRange(*aarchMemO1, dataSize)) {
    o1 = &aarchCGFunc.SplitOffsetWithAddInstruction(*aarchMemO1, dataSize, R9);
  }

  Insn &popInsn = currCG->BuildInstruction<AArch64Insn>(mOp, o0, *o1);
  popInsn.SetComment("RESTORE");
  cgFunc.GetCurBB()->AppendInsn(popInsn);

  /* Append CFI code. */
  if (!CGOptions::IsNoCalleeCFI()) {
    cgFunc.GetCurBB()->AppendInsn(aarchCGFunc.CreateCfiRestoreInsn(reg, k64BitSize));
  }
}

void AArch64GenProEpilog::AppendInstructionPopPair(AArch64reg reg0, AArch64reg reg1, RegType rty, int32 offset) {
  auto &aarchCGFunc = static_cast<AArch64CGFunc&>(cgFunc);
  CG *currCG = cgFunc.GetCG();
  MOperator mOp = pushPopOps[kRegsPopOp][rty][kPushPopPair];
  Operand &o0 = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(reg0, kSizeOfPtr * kBitsPerByte, rty);
  Operand &o1 = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(reg1, kSizeOfPtr * kBitsPerByte, rty);
  Operand *o2 = &aarchCGFunc.CreateStkTopOpnd(offset, kSizeOfPtr * kBitsPerByte);

  uint32 dataSize = kSizeOfPtr * kBitsPerByte;
  CHECK_FATAL(offset >= 0, "offset must >= 0");
  if (offset > kStpLdpImm64UpperBound) {
    o2 = SplitStpLdpOffsetForCalleeSavedWithAddInstruction(*static_cast<AArch64MemOperand*>(o2), dataSize, R16);
  }
  Insn &popInsn = currCG->BuildInstruction<AArch64Insn>(mOp, o0, o1, *o2);
  popInsn.SetComment("RESTORE RESTORE");
  cgFunc.GetCurBB()->AppendInsn(popInsn);

  /* Append CFI code */
  if (!CGOptions::IsNoCalleeCFI()) {
    cgFunc.GetCurBB()->AppendInsn(aarchCGFunc.CreateCfiRestoreInsn(reg0, k64BitSize));
    cgFunc.GetCurBB()->AppendInsn(aarchCGFunc.CreateCfiRestoreInsn(reg1, k64BitSize));
  }
}


void AArch64GenProEpilog::AppendInstructionDeallocateCallFrame(AArch64reg reg0, AArch64reg reg1, RegType rty) {
  auto &aarchCGFunc = static_cast<AArch64CGFunc&>(cgFunc);
  CG *currCG = cgFunc.GetCG();
  MOperator mOp = pushPopOps[kRegsPopOp][rty][kPushPopPair];
  Operand &o0 = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(reg0, kSizeOfPtr * kBitsPerByte, rty);
  Operand &o1 = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(reg1, kSizeOfPtr * kBitsPerByte, rty);
  int32 stackFrameSize = static_cast<AArch64MemLayout*>(cgFunc.GetMemlayout())->RealStackFrameSize();
  int64 argsToStkPassSize = cgFunc.GetMemlayout()->SizeOfArgsToStackPass();
  /*
   * ldp/stp's imm should be within -512 and 504;
   * if ldp's imm > 504, we fall back to the ldp-add version
   */
  bool useLdpAdd = false;
  int64 offset = 0;

  Operand *o2 = nullptr;
  if (!cgFunc.HasVLAOrAlloca() && argsToStkPassSize > 0) {
    o2 = aarchCGFunc.GetMemoryPool()->New<AArch64MemOperand>(RSP, argsToStkPassSize, kSizeOfPtr * kBitsPerByte);
  } else {
    if (stackFrameSize > kStpLdpImm64UpperBound) {
      useLdpAdd = true;
      offset = kOffset16MemPos;
      stackFrameSize -= offset;
    } else {
      offset = stackFrameSize;
    }
    o2 = &aarchCGFunc.CreateCallFrameOperand(offset, kSizeOfPtr * kBitsPerByte);
  }

  if (useLdpAdd) {
    Operand &spOpnd = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(RSP, k64BitSize, kRegTyInt);
    Operand &immOpnd = aarchCGFunc.CreateImmOperand(stackFrameSize, k32BitSize, true);
    aarchCGFunc.SelectAdd(spOpnd, spOpnd, immOpnd, PTY_u64);
    int64 cfiOffset = GetOffsetFromCFA();
    BB *curBB = cgFunc.GetCurBB();
    curBB->InsertInsnAfter(*(curBB->GetLastInsn()),
                           aarchCGFunc.CreateCfiDefCfaInsn(RSP, cfiOffset - stackFrameSize, k64BitSize));
  }

  if (!cgFunc.HasVLAOrAlloca() && argsToStkPassSize > 0) {
    CHECK_FATAL(!useLdpAdd, "Invalid assumption");
    if (argsToStkPassSize > kStpLdpImm64UpperBound) {
      (void)AppendInstructionForAllocateOrDeallocateCallFrame(argsToStkPassSize, reg0, reg1, rty, false);
    } else {
      Insn &deallocInsn = currCG->BuildInstruction<AArch64Insn>(mOp, o0, o1, *o2);
      cgFunc.GetCurBB()->AppendInsn(deallocInsn);
    }
    Operand &spOpnd = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(RSP, k64BitSize, kRegTyInt);
    Operand &immOpnd = aarchCGFunc.CreateImmOperand(stackFrameSize, k32BitSize, true);
    aarchCGFunc.SelectAdd(spOpnd, spOpnd, immOpnd, PTY_u64);
  } else {
    Insn &deallocInsn = currCG->BuildInstruction<AArch64Insn>(mOp, o0, o1, *o2);
    cgFunc.GetCurBB()->AppendInsn(deallocInsn);
  }

  /* Append CFI restore */
  cgFunc.GetCurBB()->AppendInsn(aarchCGFunc.CreateCfiRestoreInsn(RFP, k64BitSize));
  cgFunc.GetCurBB()->AppendInsn(aarchCGFunc.CreateCfiRestoreInsn(RLR, k64BitSize));
}

void AArch64GenProEpilog::AppendInstructionDeallocateCallFrameDebug(AArch64reg reg0, AArch64reg reg1, RegType rty) {
  auto &aarchCGFunc = static_cast<AArch64CGFunc&>(cgFunc);
  CG *currCG = cgFunc.GetCG();
  MOperator mOp = pushPopOps[kRegsPopOp][rty][kPushPopPair];
  Operand &o0 = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(reg0, kSizeOfPtr * kBitsPerByte, rty);
  Operand &o1 = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(reg1, kSizeOfPtr * kBitsPerByte, rty);
  int32 stackFrameSize = static_cast<AArch64MemLayout*>(cgFunc.GetMemlayout())->RealStackFrameSize();
  int64 argsToStkPassSize = cgFunc.GetMemlayout()->SizeOfArgsToStackPass();
  /*
   * ldp/stp's imm should be within -512 and 504;
   * if ldp's imm > 504, we fall back to the ldp-add version
   */
  if (cgFunc.HasVLAOrAlloca() || argsToStkPassSize == 0) {
    stackFrameSize -= argsToStkPassSize;
    if (stackFrameSize > kStpLdpImm64UpperBound) {
      Operand *o2;
      o2 = aarchCGFunc.GetMemoryPool()->New<AArch64MemOperand>(RSP, 0, kSizeOfPtr * kBitsPerByte);
      Insn &deallocInsn = currCG->BuildInstruction<AArch64Insn>(mOp, o0, o1, *o2);
      cgFunc.GetCurBB()->AppendInsn(deallocInsn);
      /* Append CFI restore */
      cgFunc.GetCurBB()->AppendInsn(aarchCGFunc.CreateCfiRestoreInsn(RFP, k64BitSize));
      cgFunc.GetCurBB()->AppendInsn(aarchCGFunc.CreateCfiRestoreInsn(RLR, k64BitSize));
      Operand &spOpnd = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(RSP, k64BitSize, kRegTyInt);
      Operand &immOpnd = aarchCGFunc.CreateImmOperand(stackFrameSize, k32BitSize, true);
      aarchCGFunc.SelectAdd(spOpnd, spOpnd, immOpnd, PTY_u64);
    } else {
      AArch64MemOperand &o2 = aarchCGFunc.CreateCallFrameOperand(stackFrameSize, kSizeOfPtr * kBitsPerByte);
      Insn &deallocInsn = currCG->BuildInstruction<AArch64Insn>(mOp, o0, o1, o2);
      cgFunc.GetCurBB()->AppendInsn(deallocInsn);
      cgFunc.GetCurBB()->AppendInsn(aarchCGFunc.CreateCfiRestoreInsn(RFP, k64BitSize));
      cgFunc.GetCurBB()->AppendInsn(aarchCGFunc.CreateCfiRestoreInsn(RLR, k64BitSize));
    }
  } else {
    Operand *o2;
    o2 = aarchCGFunc.GetMemoryPool()->New<AArch64MemOperand>(RSP, argsToStkPassSize, kSizeOfPtr * kBitsPerByte);
    if (argsToStkPassSize > kStpLdpImm64UpperBound) {
      (void)AppendInstructionForAllocateOrDeallocateCallFrame(argsToStkPassSize, reg0, reg1, rty, false);
    } else {
      Insn &deallocInsn = currCG->BuildInstruction<AArch64Insn>(mOp, o0, o1, *o2);
      cgFunc.GetCurBB()->AppendInsn(deallocInsn);
    }

    cgFunc.GetCurBB()->AppendInsn(aarchCGFunc.CreateCfiRestoreInsn(RFP, k64BitSize));
    cgFunc.GetCurBB()->AppendInsn(aarchCGFunc.CreateCfiRestoreInsn(RLR, k64BitSize));
    Operand &spOpnd = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(RSP, k64BitSize, kRegTyInt);
    Operand &immOpnd = aarchCGFunc.CreateImmOperand(stackFrameSize, k32BitSize, true);
    aarchCGFunc.SelectAdd(spOpnd, spOpnd, immOpnd, PTY_u64);
  }
}

void AArch64GenProEpilog::GeneratePopRegs() {
  auto &aarchCGFunc = static_cast<AArch64CGFunc&>(cgFunc);
  CG *currCG = cgFunc.GetCG();
  const MapleVector<AArch64reg> &regsToRestore = aarchCGFunc.GetCalleeSavedRegs();

  CHECK_FATAL(!regsToRestore.empty(), "FP/LR not added to callee-saved list?");

  AArch64reg intRegFirstHalf = kRinvalid;
  AArch64reg fpRegFirstHalf = kRinvalid;

  if (currCG->GenerateVerboseAsm()) {
    cgFunc.GetCurBB()->AppendInsn(aarchCGFunc.CreateCommentInsn("restore callee-saved registers"));
  }

  MapleVector<AArch64reg>::const_iterator it = regsToRestore.begin();
  /*
   * Even if we don't use FP, since we push a pair of registers
   * in a single instruction (i.e., stp) and the stack needs be aligned
   * on a 16-byte boundary, push FP as well if the function has a call.
   * Make sure this is reflected when computing calleeSavedRegs.size()
   * skip the first two registers
   */
  CHECK_FATAL(*it == RFP, "The first callee saved reg is expected to be RFP");
  ++it;
  CHECK_FATAL(*it == RLR, "The second callee saved reg is expected to be RLR");
  ++it;

  int32 offset = static_cast<AArch64MemLayout*>(cgFunc.GetMemlayout())->RealStackFrameSize() -
                 (aarchCGFunc.SizeOfCalleeSaved() - (kDivide2 * kIntregBytelen) /* for FP/LR */) -
                 cgFunc.GetMemlayout()->SizeOfArgsToStackPass();

  /*
   * We are using a cleared dummy block; so insertPoint cannot be ret;
   * see GenerateEpilog()
   */
  for (; it != regsToRestore.end(); ++it) {
    AArch64reg reg = *it;
    CHECK_FATAL(reg != RFP, "stray RFP in callee_saved_list?");
    CHECK_FATAL(reg != RLR, "stray RLR in callee_saved_list?");

    RegType regType = AArch64isa::IsGPRegister(reg) ? kRegTyInt : kRegTyFloat;
    AArch64reg &firstHalf = AArch64isa::IsGPRegister(reg) ? intRegFirstHalf : fpRegFirstHalf;
    if (firstHalf == kRinvalid) {
      /* remember it */
      firstHalf = reg;
    } else {
      /* flush the pair */
      AppendInstructionPopPair(firstHalf, reg, regType, offset);
      GetNextOffsetCalleeSaved(offset);
      firstHalf = kRinvalid;
    }
  }

  if (intRegFirstHalf != kRinvalid) {
    AppendInstructionPopSingle(intRegFirstHalf, kRegTyInt, offset);
    GetNextOffsetCalleeSaved(offset);
  }

  if (fpRegFirstHalf != kRinvalid) {
    AppendInstructionPopSingle(fpRegFirstHalf, kRegTyFloat, offset);
    GetNextOffsetCalleeSaved(offset);
  }

  if (!currCG->GenerateDebugFriendlyCode()) {
    AppendInstructionDeallocateCallFrame(RFP, RLR, kRegTyInt);
  } else {
    AppendInstructionDeallocateCallFrameDebug(RFP, RLR, kRegTyInt);
  }

  cgFunc.GetCurBB()->AppendInsn(aarchCGFunc.CreateCfiDefCfaInsn(RSP, 0, k64BitSize));
  /*
   * in case we split stp/ldp instructions,
   * so that we generate a load-into-base-register instruction
   * for the next function, maybe? (seems not necessary, but...)
   */
  aarchCGFunc.SetSplitBaseOffset(0);
}

void AArch64GenProEpilog::AppendJump(const MIRSymbol &funcSymbol) {
  auto &aarchCGFunc = static_cast<AArch64CGFunc&>(cgFunc);
  CG *currCG = cgFunc.GetCG();
  Operand &targetOpnd = aarchCGFunc.CreateFuncLabelOperand(funcSymbol);
  cgFunc.GetCurBB()->AppendInsn(currCG->BuildInstruction<AArch64Insn>(MOP_xuncond, targetOpnd));
}

void AArch64GenProEpilog::GenerateEpilog(BB &bb) {
  if (!cgFunc.GetHasProEpilogue()) {
    if (bb.GetPreds().empty() || !TestPredsOfRetBB(bb)) {
      GenerateRet(bb);
    }
    return;
  }

  /* generate stack protected instruction */
  BB &epilogBB = GenStackGuardCheckInsn(bb);

  auto &aarchCGFunc = static_cast<AArch64CGFunc&>(cgFunc);
  CG *currCG = cgFunc.GetCG();
  BB *formerCurBB = cgFunc.GetCurBB();
  aarchCGFunc.GetDummyBB()->ClearInsns();
  cgFunc.SetCurBB(*aarchCGFunc.GetDummyBB());

  Operand &spOpnd = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(RSP, k64BitSize, kRegTyInt);
  Operand &fpOpnd = aarchCGFunc.GetOrCreatePhysicalRegisterOperand(RFP, k64BitSize, kRegTyInt);

  if (cgFunc.HasVLAOrAlloca()) {
    aarchCGFunc.SelectCopy(spOpnd, PTY_u64, fpOpnd, PTY_u64);
  }

  /* Hack: exit bb should always be reachable, since we need its existance for ".cfi_remember_state" */
  if (&epilogBB != cgFunc.GetLastBB() && epilogBB.GetNext() != nullptr) {
    BB *nextBB = epilogBB.GetNext();
    do {
      if (nextBB == cgFunc.GetLastBB() || !nextBB->IsEmpty()) {
        break;
      }
      nextBB = nextBB->GetNext();
    } while (nextBB != nullptr);
    if (nextBB != nullptr && !nextBB->IsEmpty()) {
      cgFunc.GetCurBB()->AppendInsn(currCG->BuildInstruction<cfi::CfiInsn>(cfi::OP_CFI_remember_state));
      nextBB->InsertInsnBefore(*nextBB->GetFirstInsn(),
                               currCG->BuildInstruction<cfi::CfiInsn>(cfi::OP_CFI_restore_state));
    }
  }

  const MapleVector<AArch64reg> &regsToSave = aarchCGFunc.GetCalleeSavedRegs();
  if (!regsToSave.empty()) {
    GeneratePopRegs();
  } else {
    int32 stackFrameSize = static_cast<AArch64MemLayout*>(cgFunc.GetMemlayout())->RealStackFrameSize();
    if (stackFrameSize > 0) {
      if (currCG->GenerateVerboseAsm()) {
        cgFunc.GetCurBB()->AppendInsn(aarchCGFunc.CreateCommentInsn("pop up activation frame"));
      }

      if (cgFunc.HasVLAOrAlloca()) {
        stackFrameSize -= static_cast<AArch64MemLayout*>(cgFunc.GetMemlayout())->GetSegArgsStkPass().GetSize();
      }

      if (stackFrameSize > 0) {
        Operand &immOpnd = aarchCGFunc.CreateImmOperand(stackFrameSize, k32BitSize, true);
        aarchCGFunc.SelectAdd(spOpnd, spOpnd, immOpnd, PTY_u64);
        cgFunc.GetCurBB()->AppendInsn(aarchCGFunc.CreateCfiDefCfaInsn(RSP, 0, k64BitSize));
      }
    }
  }

  if (currCG->InstrumentWithDebugTraceCall()) {
    AppendJump(*(currCG->GetDebugTraceExitFunction()));
  }

  GenerateRet(*(cgFunc.GetCurBB()));
  epilogBB.AppendBBInsns(*cgFunc.GetCurBB());

  cgFunc.SetCurBB(*formerCurBB);
}

void AArch64GenProEpilog::GenerateEpilogForCleanup(BB &bb) {
  auto &aarchCGFunc = static_cast<AArch64CGFunc&>(cgFunc);
  CG *currCG = cgFunc.GetCG();
  CHECK_FATAL(!cgFunc.GetExitBBsVec().empty(), "exit bb size is zero!");
  if (cgFunc.GetExitBB(0)->IsUnreachable()) {
    /* if exitbb is unreachable then exitbb can not be generated */
    GenerateEpilog(bb);
  } else if (aarchCGFunc.NeedCleanup()) {  /* bl to the exit epilogue */
    LabelOperand &targetOpnd = aarchCGFunc.GetOrCreateLabelOperand(cgFunc.GetExitBB(0)->GetLabIdx());
    bb.AppendInsn(currCG->BuildInstruction<AArch64Insn>(MOP_xuncond, targetOpnd));
  }
}

void AArch64GenProEpilog::Run() {
  CHECK_FATAL(cgFunc.GetFunction().GetBody()->GetFirst()->GetOpCode() == OP_label,
              "The first statement should be a label");
  cgFunc.SetHasProEpilogue(true);

  if (cgFunc.GetHasProEpilogue()) {
    GenStackGuard(*(cgFunc.GetFirstBB()));
  }

  if (cgFunc.IsExitBBsVecEmpty()) {
    if (cgFunc.GetLastBB()->GetPrev()->GetFirstStmt() == cgFunc.GetCleanupLabel() &&
        cgFunc.GetLastBB()->GetPrev()->GetPrev()) {
      cgFunc.PushBackExitBBsVec(*cgFunc.GetLastBB()->GetPrev()->GetPrev());
    } else {
      cgFunc.PushBackExitBBsVec(*cgFunc.GetLastBB()->GetPrev());
    }
  }

  GenerateProlog(*(cgFunc.GetFirstBB()));

  for (auto *exitBB : cgFunc.GetExitBBsVec()) {
    GenerateEpilog(*exitBB);
  }

  if (cgFunc.GetFunction().IsJava()) {
    GenerateEpilogForCleanup(*(cgFunc.GetCleanupBB()));
  }
}
}  /* namespace maplebe */
