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
#include "aarch64_emitter.h"
#include <sys/stat.h>
#include "aarch64_cgfunc.h"

namespace {
using namespace maple;
constexpr uint32 kQuadInsnCount = 2;
constexpr uint32 kInsnSize = 4;

void GetMethodLabel(const std::string &methodName, std::string &methodLabel) {
  methodLabel = ".Lmethod_desc." + methodName;
}
}

namespace maplebe {
using namespace maple;

void AArch64Emitter::EmitRefToMethodDesc(Emitter &emitter) {
  if (!cgFunc->GetFunction().IsJava()) {
    return;
  }
  std::string methodDescLabel;
  GetMethodLabel(cgFunc->GetFunction().GetName(), methodDescLabel);
  emitter.Emit("\t.word " + methodDescLabel + "-.\n");
  emitter.IncreaseJavaInsnCount();
}

void AArch64Emitter::EmitRefToMethodInfo(Emitter &emitter) {
  if (cgFunc->GetFunction().GetModule()->IsJavaModule()) {
    std::string labelName = ".Label.name." + cgFunc->GetFunction().GetName();
    emitter.Emit("\t.word " + labelName + " - .\n");
  }
}

/*
 * emit java method description which contains address and size of local reference area
 * as well as method metadata.
 */
void AArch64Emitter::EmitMethodDesc(Emitter &emitter) {
  if (!cgFunc->GetFunction().IsJava()) {
    return;
  }
  emitter.Emit("\t.section\t.rodata\n");
  emitter.Emit("\t.align\t2\n");
  std::string methodInfoLabel;
  GetMethodLabel(cgFunc->GetFunction().GetName(), methodInfoLabel);
  emitter.Emit(methodInfoLabel + ":\n");
  EmitRefToMethodInfo(emitter);
  /* local reference area */
  AArch64MemLayout *memLayout = static_cast<AArch64MemLayout*>(cgFunc->GetMemlayout());
  int32 refOffset = memLayout->GetRefLocBaseLoc();
  uint32 refNum = memLayout->GetSizeOfRefLocals() / kOffsetAlign;
  /* for ea usage */
  AArch64CGFunc *aarchCGFunc = static_cast<AArch64CGFunc*>(cgFunc);
  IntrinsiccallNode *cleanEANode = aarchCGFunc->GetCleanEANode();
  if (cleanEANode != nullptr) {
    refNum += cleanEANode->NumOpnds();
    refOffset -= cleanEANode->NumOpnds() * kIntregBytelen;
  }
  emitter.Emit("\t.short ").Emit(refOffset).Emit("\n");
  emitter.Emit("\t.short ").Emit(refNum).Emit("\n");
}

/* the fast_exception_handling lsda */
void AArch64Emitter::EmitFastLSDA() {
  AArch64CGFunc *aarchCGFunc = static_cast<AArch64CGFunc*>(cgFunc);
  CG *currCG = cgFunc->GetCG();

  Emitter *emitter = currCG->GetEmitter();
  const std::string &funcName = std::string(cgFunc->GetShortFuncName().c_str());
  /*
   * .word 0xFFFFFFFF
   * .word .Label.LTest_3B_7C_3Cinit_3E_7C_28_29V3-func_start_label
   */
  emitter->Emit("\t.word 0xFFFFFFFF\n");
  emitter->Emit("\t.word .L." + funcName + ".");
  if (aarchCGFunc->NeedCleanup()) {
    emitter->Emit(cgFunc->GetCleanupLabel()->GetLabelIdx());
  } else {
    ASSERT(!cgFunc->GetExitBBsVec().empty(), "exitbbsvec is empty in AArch64Emitter::EmitFastLSDA");
    emitter->Emit(cgFunc->GetExitBB(0)->GetLabIdx());
  }
  emitter->Emit("-.L." + funcName + ".")
      .Emit(cgFunc->GetStartLabel()->GetLabelIdx())
      .Emit("\n");
  emitter->IncreaseJavaInsnCount();
}

/* the normal gcc_except_table */
void AArch64Emitter::EmitFullLSDA() {
  AArch64CGFunc *aarchCGFunc = static_cast<AArch64CGFunc*>(cgFunc);
  CG *currCG = cgFunc->GetCG();
  EHFunc *ehFunc = cgFunc->GetEHFunc();
  Emitter *emitter = currCG->GetEmitter();
  /* emit header */
  emitter->Emit("\t.align 2\n");
  emitter->Emit("\t.section .gcc_except_table,\"a\",@progbits\n");
  emitter->Emit("\t.align 2\n");
  /* emit LSDA header */
  LSDAHeader *lsdaHeader = ehFunc->GetLSDAHeader();
  const std::string &funcName = std::string(cgFunc->GetShortFuncName().c_str());
  emitter->EmitStmtLabel(funcName, lsdaHeader->GetLSDALabel()->GetLabelIdx());
  emitter->Emit("\t.byte ").Emit(lsdaHeader->GetLPStartEncoding()).Emit("\n");
  emitter->Emit("\t.byte ").Emit(lsdaHeader->GetTTypeEncoding()).Emit("\n");
  emitter->Emit("\t.uleb128 ");
  emitter->EmitLabelPair(funcName, lsdaHeader->GetTTypeOffset());
  emitter->EmitStmtLabel(funcName, lsdaHeader->GetTTypeOffset().GetStartOffset()->GetLabelIdx());
  /* emit call site table */
  emitter->Emit("\t.byte ").Emit(lsdaHeader->GetCallSiteEncoding()).Emit("\n");
  /* callsite table size */
  emitter->Emit("\t.uleb128 ");
  emitter->EmitLabelPair(funcName, ehFunc->GetLSDACallSiteTable()->GetCSTable());
  /* callsite start */
  emitter->EmitStmtLabel(funcName, ehFunc->GetLSDACallSiteTable()->GetCSTable().GetStartOffset()->GetLabelIdx());
  ehFunc->GetLSDACallSiteTable()->SortCallSiteTable([aarchCGFunc](LSDACallSite *a, LSDACallSite *b) {
    CHECK_FATAL(a != nullptr, "nullptr check");
    CHECK_FATAL(b != nullptr, "nullptr check");
    LabelIDOrder id1 = aarchCGFunc->GetLabelOperand(a->csStart.GetEndOffset()->GetLabelIdx())->GetLabelOrder();
    LabelIDOrder id2 = aarchCGFunc->GetLabelOperand(b->csStart.GetEndOffset()->GetLabelIdx())->GetLabelOrder();
    /* id1 and id2 should not be default value -1u */
    CHECK_FATAL(id1 != 0xFFFFFFFF, "illegal label order assigned");
    CHECK_FATAL(id2 != 0xFFFFFFFF, "illegal label order assigned");
    return id1 < id2;
  });
  const MapleVector<LSDACallSite*> &callSiteTable = ehFunc->GetLSDACallSiteTable()->GetCallSiteTable();
  for (size_t i = 0; i < callSiteTable.size(); ++i) {
    LSDACallSite *lsdaCallSite = callSiteTable[i];
    emitter->Emit("\t.uleb128 ");
    emitter->EmitLabelPair(funcName, lsdaCallSite->csStart);

    emitter->Emit("\t.uleb128 ");
    emitter->EmitLabelPair(funcName, lsdaCallSite->csLength);

    if (lsdaCallSite->csLandingPad.GetStartOffset()) {
      emitter->Emit("\t.uleb128 ");
      emitter->EmitLabelPair(funcName, lsdaCallSite->csLandingPad);
    } else {
      ASSERT(lsdaCallSite->csAction == 0, "csAction error!");
      emitter->Emit("\t.uleb128 ");
      if (aarchCGFunc->NeedCleanup()) {
        /* if landing pad is 0, we emit this call site as cleanup code */
        LabelPair cleaupCode;
        cleaupCode.SetStartOffset(cgFunc->GetStartLabel());
        cleaupCode.SetEndOffset(cgFunc->GetCleanupLabel());
        emitter->EmitLabelPair(funcName, cleaupCode);
      } else if (cgFunc->GetFunction().IsJava()) {
        ASSERT(!cgFunc->GetExitBBsVec().empty(), "exitbbsvec is empty in AArch64Emitter::EmitFullLSDA");
        emitter->Emit(".L." + funcName).Emit(".").Emit(cgFunc->GetExitBB(0)->GetLabIdx());
        emitter->Emit(" - .L." + funcName).Emit(".").Emit(cgFunc->GetStartLabel()->GetLabelIdx()).Emit("\n");
      } else {
        emitter->Emit("0\n");
      }
    }
    emitter->Emit("\t.uleb128 ").Emit(lsdaCallSite->csAction).Emit("\n");
  }

  /*
   * quick hack: insert a call site entry for the whole function body.
   * this will hand in any pending (uncaught) exception to its caller. Note that
   * __gxx_personality_v0 in libstdc++ is coded so that if exception table exists,
   * the call site table must have an entry for any possibly raised exception,
   * otherwise __cxa_call_terminate will be invoked immediately, thus the caller
   * does not get the chance to take charge.
   */
  if (aarchCGFunc->NeedCleanup() || cgFunc->GetFunction().IsJava()) {
    /* call site for clean-up */
    LabelPair funcStart;
    funcStart.SetStartOffset(cgFunc->GetStartLabel());
    funcStart.SetEndOffset(cgFunc->GetStartLabel());
    emitter->Emit("\t.uleb128 ");
    emitter->EmitLabelPair(funcName, funcStart);
    LabelPair funcLength;
    funcLength.SetStartOffset(cgFunc->GetStartLabel());
    funcLength.SetEndOffset(cgFunc->GetCleanupLabel());
    emitter->Emit("\t.uleb128 ");
    emitter->EmitLabelPair(funcName, funcLength);
    LabelPair cleaupCode;
    cleaupCode.SetStartOffset(cgFunc->GetStartLabel());
    cleaupCode.SetEndOffset(cgFunc->GetCleanupLabel());
    emitter->Emit("\t.uleb128 ");
    if (aarchCGFunc->NeedCleanup()) {
      emitter->EmitLabelPair(funcName, cleaupCode);
    } else {
      ASSERT(!cgFunc->GetExitBBsVec().empty(), "exitbbsvec is empty in AArch64Emitter::EmitFullLSDA");
      emitter->Emit(".L." + funcName).Emit(".").Emit(cgFunc->GetExitBB(0)->GetLabIdx());
      emitter->Emit(" - .L." + funcName).Emit(".").Emit(cgFunc->GetStartLabel()->GetLabelIdx()).Emit("\n");
    }
    emitter->Emit("\t.uleb128 0\n");
    if (!cgFunc->GetFunction().IsJava()) {
      /* call site for stack unwind */
      LabelPair unwindStart;
      unwindStart.SetStartOffset(cgFunc->GetStartLabel());
      unwindStart.SetEndOffset(cgFunc->GetCleanupLabel());
      emitter->Emit("\t.uleb128 ");
      emitter->EmitLabelPair(funcName, unwindStart);
      LabelPair unwindLength;
      unwindLength.SetStartOffset(cgFunc->GetCleanupLabel());
      unwindLength.SetEndOffset(cgFunc->GetEndLabel());
      emitter->Emit("\t.uleb128 ");
      emitter->EmitLabelPair(funcName, unwindLength);
      emitter->Emit("\t.uleb128 0\n");
      emitter->Emit("\t.uleb128 0\n");
    }
  }
  /* callsite end label */
  emitter->EmitStmtLabel(funcName, ehFunc->GetLSDACallSiteTable()->GetCSTable().GetEndOffset()->GetLabelIdx());
  /* tt */
  const LSDAActionTable *lsdaActionTable = ehFunc->GetLSDAActionTable();
  for (size_t i = 0; i < lsdaActionTable->Size(); ++i) {
    LSDAAction *lsdaAction = lsdaActionTable->GetActionTable().at(i);
    emitter->Emit("\t.byte ").Emit(lsdaAction->GetActionIndex()).Emit("\n");
    emitter->Emit("\t.byte ").Emit(lsdaAction->GetActionFilter()).Emit("\n");
  }
  emitter->Emit("\t.align 2\n");
  for (int32 i = ehFunc->GetEHTyTableSize() - 1; i >= 0; i--) {
    MIRType *mirType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(ehFunc->GetEHTyTableMember(i));
    MIRTypeKind typeKind = mirType->GetKind();
    if (((typeKind == kTypeScalar) && (mirType->GetPrimType() == PTY_void)) || (typeKind == kTypeStructIncomplete) ||
        (typeKind == kTypeInterfaceIncomplete)) {
      continue;
    }
    CHECK_FATAL((typeKind == kTypeClass) || (typeKind == kTypeClassIncomplete), "NYI");
    const std::string &tyName = GlobalTables::GetStrTable().GetStringFromStrIdx(mirType->GetNameStrIdx());
    std::string dwRefString(".LDW.ref.");
    dwRefString += CLASSINFO_PREFIX_STR;
    dwRefString += tyName;
    dwRefString += " - .";
    emitter->Emit("\t.4byte " + dwRefString + "\n");
  }
  /* end of lsda */
  emitter->EmitStmtLabel(funcName, lsdaHeader->GetTTypeOffset().GetEndOffset()->GetLabelIdx());
}

void AArch64Emitter::EmitBBHeaderLabel(const std::string &name, LabelIdx labIdx) {
  AArch64CGFunc *aarchCGFunc = static_cast<AArch64CGFunc*>(cgFunc);
  CG *currCG = cgFunc->GetCG();
  Emitter &emitter = *(currCG->GetEmitter());
  LabelOperand &label = aarchCGFunc->GetOrCreateLabelOperand(labIdx);
  /* if label order is default value -1, set new order */
  if (label.GetLabelOrder() == 0xFFFFFFFF) {
    label.SetLabelOrder(currCG->GetLabelOrderCnt());
    currCG->IncreaseLabelOrderCnt();
  }
  if (currCG->GenerateVerboseCG()) {
    emitter.Emit(".L.").Emit(name).Emit(".").Emit(labIdx).Emit(":\t//label order ").Emit(label.GetLabelOrder());
    emitter.Emit("\n");
  } else {
    emitter.Emit(".L.").Emit(name).Emit(".").Emit(labIdx).Emit(":\n");
  }
}

void AArch64Emitter::EmitJavaInsnAddr() {
  if (cgFunc->GetFunction().IsJava()) {
    Emitter *emitter = cgFunc->GetCG()->GetEmitter();
    /* emit a comment of current address from the begining of java text section */
    std::stringstream ss;
    ss << "\n\t// addr: 0x" << std::hex << (emitter->GetJavaInsnCount() * kInsnSize) << "\n";
    cgFunc->GetCG()->GetEmitter()->Emit(ss.str());
  }
}

void AArch64Emitter::Run() {
  AArch64CGFunc *aarchCGFunc = static_cast<AArch64CGFunc*>(cgFunc);
  CG *currCG = cgFunc->GetCG();
  /* emit header of this function */
  Emitter &emitter = *currCG->GetEmitter();
  // insert for  __cxx_global_var_init
  if (cgFunc->GetName() == "__cxx_global_var_init") {
    emitter.Emit("\t.section\t.init_array,\"aw\"\n");
    emitter.Emit("\t.quad\t").Emit(cgFunc->GetName()).Emit("\n");
  }
  emitter.Emit("\n");
  EmitMethodDesc(emitter);
  /* emit java code to the java section. */
  if (cgFunc->GetFunction().IsJava()) {
    std::string sectionName = namemangler::kMuidJavatextPrefixStr;
    emitter.Emit("\t.section  ." + sectionName + ",\"ax\"\n");
  } else {
    emitter.Emit("\t.text\n");
  }
  emitter.Emit("\t.align 2\n");
  MIRSymbol *funcSt = GlobalTables::GetGsymTable().GetSymbolFromStidx(cgFunc->GetFunction().GetStIdx().Idx());
  const std::string &funcName = std::string(cgFunc->GetShortFuncName().c_str());


  std::string funcStName = funcSt->GetName();
  if (funcSt->GetFunction()->GetAttr(FUNCATTR_weak)) {
    emitter.Emit("\t.weak\t" + funcStName + "\n");
    emitter.Emit("\t.hidden\t" + funcStName + "\n");
  } else if (funcSt->GetFunction()->GetAttr(FUNCATTR_local)) {
    emitter.Emit("\t.local\t" + funcStName + "\n");
  } else {
    bool isExternFunction = false;
    emitter.Emit("\t.globl\t").Emit(funcSt->GetName()).Emit("\n");
    if (!currCG->GetMIRModule()->IsCModule() || !isExternFunction) {
      emitter.Emit("\t.hidden\t").Emit(funcSt->GetName()).Emit("\n");
    }
  }
  emitter.Emit("\t.type\t" + funcStName + ", %function\n");
  /* add these messege , solve the simpleperf tool error */
  EmitRefToMethodDesc(emitter);
  emitter.Emit(funcStName + ":\n");
  /* if the last  insn is call, then insert nop */
  bool found = false;
  FOR_ALL_BB_REV(bb, aarchCGFunc) {
    FOR_BB_INSNS_REV(insn, bb) {
      if (insn->IsMachineInstruction()) {
        if (insn->IsCall()) {
          Insn &newInsn = currCG->BuildInstruction<AArch64Insn>(MOP_nop);
          bb->InsertInsnAfter(*insn, newInsn);
        }
        found = true;
        break;
      }
    }
    if (found) {
      break;
    }
  }
  /* emit instructions */
  FOR_ALL_BB(bb, aarchCGFunc) {
    if (currCG->GenerateVerboseCG()) {
      emitter.Emit("#    freq:").Emit(bb->GetFrequency()).Emit("\n");
    }
    /* emit bb headers */
    if (bb->GetLabIdx() != 0) {
      EmitBBHeaderLabel(funcName, bb->GetLabIdx());
    }

    FOR_BB_INSNS(insn, bb) {
      insn->Emit(*currCG, emitter);
    }
  }
  if (CGOptions::IsMapleLinker()) {
    /* Emit a label for calculating method size */
    emitter.Emit(".Label.end." + funcStName + ":\n");
  }
  emitter.Emit("\t.size\t" + funcStName + ", .-").Emit(funcStName + "\n");

  EHFunc *ehFunc = cgFunc->GetEHFunc();
  /* emit LSDA */
  if (ehFunc != nullptr) {
    if (!cgFunc->GetHasProEpilogue()) {
      emitter.Emit("\t.word 0x55555555\n");
      emitter.IncreaseJavaInsnCount();
    } else if (ehFunc->NeedFullLSDA()) {
      LSDAHeader *lsdaHeader = ehFunc->GetLSDAHeader();
      /*  .word .Label.lsda_label-func_start_label */
      emitter.Emit("\t.word .L." + funcName).Emit(".").Emit(lsdaHeader->GetLSDALabel()->GetLabelIdx());
      emitter.Emit("-.L." + funcName).Emit(".").Emit(cgFunc->GetStartLabel()->GetLabelIdx()).Emit("\n");
      emitter.IncreaseJavaInsnCount();
    } else if (ehFunc->NeedFastLSDA()) {
      EmitFastLSDA();
    }
  }
  uint32 size = cgFunc->GetFunction().GetSymTab()->GetSymbolTableSize();
  for (size_t i = 0; i < size; ++i) {
    MIRSymbol *st = cgFunc->GetFunction().GetSymTab()->GetSymbolFromStIdx(i);
    if (st == nullptr) {
      continue;
    }
    MIRStorageClass storageClass = st->GetStorageClass();
    MIRSymKind symKind = st->GetSKind();
    if (storageClass == kScPstatic && symKind == kStConst) {
      emitter.Emit("\t.align 2\n" + st->GetName() + ":\n");
      if (st->GetKonst()->GetKind() == kConstStr16Const) {
        MIRStr16Const *str16Const = safe_cast<MIRStr16Const>(st->GetKonst());
        emitter.EmitStr16Constant(*str16Const);
        emitter.Emit("\n");
        continue;
      }
      if (st->GetKonst()->GetKind() == kConstStrConst) {
        MIRStrConst *strConst = safe_cast<MIRStrConst>(st->GetKonst());
        emitter.EmitStrConstant(*strConst);
        emitter.Emit("\n");
        continue;
      }

      switch (st->GetKonst()->GetType().GetPrimType()) {
        case PTY_u32: {
          MIRIntConst *intConst = safe_cast<MIRIntConst>(st->GetKonst());
          emitter.Emit("\t.long ").Emit(static_cast<uint32>(intConst->GetValue())).Emit("\n");
          emitter.IncreaseJavaInsnCount();
          break;
        }
        case PTY_f32: {
          MIRFloatConst *floatConst = safe_cast<MIRFloatConst>(st->GetKonst());
          emitter.Emit("\t.word ").Emit(static_cast<uint32>(floatConst->GetIntValue())).Emit("\n");
          emitter.IncreaseJavaInsnCount();
          break;
        }
        case PTY_f64: {
          MIRDoubleConst *doubleConst = safe_cast<MIRDoubleConst>(st->GetKonst());
          emitter.Emit("\t.word ").Emit(doubleConst->GetIntLow32()).Emit("\n");
          emitter.IncreaseJavaInsnCount();
          emitter.Emit("\t.word ").Emit(doubleConst->GetIntHigh32()).Emit("\n");
          emitter.IncreaseJavaInsnCount();
          break;
        }
        default:
          ASSERT(false, "NYI");
          break;
      }
    }
  }

  for (auto *st : cgFunc->GetEmitStVec()) {
    /* emit switch table only here */
    ASSERT(st->IsReadOnly(), "NYI");
    emitter.Emit("\n");
    emitter.Emit("\t.align 3\n");
    emitter.IncreaseJavaInsnCount(0, true); /* just aligned */
    emitter.Emit(st->GetName() + ":\n");
    MIRAggConst *arrayConst = safe_cast<MIRAggConst>(st->GetKonst());
    CHECK_FATAL(arrayConst != nullptr, "null ptr check");
    for (size_t i = 0; i < arrayConst->GetConstVec().size(); i++) {
      MIRLblConst *lblConst = safe_cast<MIRLblConst>(arrayConst->GetConstVecItem(i));
      CHECK_FATAL(lblConst != nullptr, "null ptr check");
      emitter.Emit("\t.quad\t.L." + funcName).Emit(".").Emit(lblConst->GetValue());
      emitter.Emit(" - " + st->GetName() + "\n");
      emitter.IncreaseJavaInsnCount(kQuadInsnCount);
    }
  }

  for (const auto &mpPair : cgFunc->GetLabelAndValueMap()) {
    LabelOperand &labelOpnd = aarchCGFunc->GetOrCreateLabelOperand(mpPair.first);
    labelOpnd.Emit(emitter, nullptr);
    emitter.Emit(":\n");
    emitter.Emit("\t.quad ").Emit(mpPair.second).Emit("\n");
    emitter.IncreaseJavaInsnCount(kQuadInsnCount);
  }

  if (ehFunc != nullptr && ehFunc->NeedFullLSDA()) {
    EmitFullLSDA();
  }
#ifdef EMIT_INSN_COUNT
  if (cgFunc->GetFunction().IsJava()) {
    EmitJavaInsnAddr();
  }
#endif /* ~EMIT_INSN_COUNT */
}

AnalysisResult *CgDoEmission::Run(CGFunc *cgFunc, CgFuncResultMgr *cgFuncResultMgr) {
  (void)cgFuncResultMgr;
  ASSERT(cgFunc != nullptr, "null ptr check");
  MemPool *memPool = NewMemPool();
  AArch64Emitter *aarch64Emitter = memPool->New<AArch64Emitter>(*cgFunc);
  aarch64Emitter->Run();
  return nullptr;
}
}  /* namespace maplebe */
