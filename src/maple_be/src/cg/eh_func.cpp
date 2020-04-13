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
#include "eh_func.h"
#include "cgfunc.h"
#include "cg.h"
#include "mir_builder.h"
#include "switch_lowerer.h"

namespace maplebe {
using namespace maple;

void EHFunc::CollectEHInformation(std::vector<std::pair<LabelIdx, CatchNode*>> &catchVec) {
  MIRFunction &mirFunc = cgFunc->GetFunction();
  MIRModule *mirModule = mirFunc.GetModule();
  CHECK_FATAL(mirModule != nullptr, "mirModule is nullptr in CGFunc::BuildEHFunc");
  BlockNode *blkNode = mirFunc.GetBody();
  CHECK_FATAL(blkNode != nullptr, "current function body is nullptr in CGFunc::BuildEHFunc");
  EHTry *lastTry = nullptr;  /* record last try */
  /*
   * curTry: record the current try wrapping the current statement,
   *         reset to null when meet a endtry
   */
  EHTry *curTry = nullptr;
  StmtNode *nextStmt = nullptr;

  /* collect all try-catch blocks */
  for (StmtNode *stmt = blkNode->GetFirst(); stmt != nullptr; stmt = nextStmt) {
    nextStmt = stmt->GetNext();
    Opcode op = stmt->GetOpCode();
    switch (op) {
      case OP_try: {
        TryNode *tryNode = static_cast<TryNode*>(stmt);
        EHTry *ehTry = cgFunc->GetMemoryPool()->New<EHTry>(*(cgFunc->GetFuncScopeAllocator()), *tryNode);
        lastTry = ehTry;
        curTry = ehTry;
        AddTry(*ehTry);
        break;
      }
      case OP_endtry: {
        ASSERT(lastTry != nullptr, "lastTry is nullptr when current node is endtry");
        lastTry->SetEndtryNode(*stmt);
        lastTry = nullptr;
        curTry = nullptr;
        break;
      }
      case OP_catch: {
        CatchNode *catchNode = static_cast<CatchNode*>(stmt);
        ASSERT(stmt->GetPrev()->GetOpCode() == OP_label, "catch's previous node is not a label");
        LabelNode *labelStmt = static_cast<LabelNode*>(stmt->GetPrev());
        catchVec.push_back(std::pair<LabelIdx, CatchNode*>(labelStmt->GetLabelIdx(), catchNode));
        /* rename the type of <*void> to <*Throwable> */
        for (uint32 i = 0; i < catchNode->Size(); i++) {
          MIRType *ehType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(catchNode->GetExceptionTyIdxVecElement(i));
          ASSERT(ehType->GetKind() == kTypePointer, "ehType must be kTypePointer.");
          MIRPtrType *ehPointedTy = static_cast<MIRPtrType*>(ehType);
          if (ehPointedTy->GetPointedTyIdx() == (TyIdx)PTY_void) {
            ASSERT(mirModule->GetThrowableTyIdx() != 0, "throwable type id is 0");
            const MIRType *throwType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(mirModule->GetThrowableTyIdx());
            MIRType *pointerType = cgFunc->GetBecommon().BeGetOrCreatePointerType(*throwType);
            catchNode->SetExceptionTyIdxVecElement(pointerType->GetTypeIndex(), i);
          }
        }
        break;
      }
      case OP_throw: {
        if (!cgFunc->GetCG()->GetCGOptions().GenerateExceptionHandlingCode() ||
            (cgFunc->GetCG()->IsExclusiveEH() && cgFunc->GetCG()->IsExclusiveFunc(mirFunc))) {
          /* remove the statment */
          BlockNode *bodyNode = mirFunc.GetBody();
          bodyNode->RemoveStmt(stmt);
          break;
        }
        UnaryStmtNode *throwNode = static_cast<UnaryStmtNode*>(stmt);
        EHThrow *ehReThrow = cgFunc->GetMemoryPool()->New<EHThrow>(*throwNode);
        ehReThrow->SetJavaTry(curTry);
        AddRethrow(*ehReThrow);
        break;
      }
      case OP_block:
        CHECK_FATAL(false, "should've lowered earlier");
      default:
        break;
    }
  }
}

void EHTry::DumpEHTry(const MIRModule&) {
  if (tryNode != nullptr) {
    tryNode->Dump();
  }

  if (endTryNode != nullptr) {
    endTryNode->Dump();
  }

  for (const auto *currCatch : catchVec) {
    if (currCatch == nullptr) {
      continue;
    }
    currCatch->Dump();
  }
}

void EHThrow::ConvertThrowToRuntime(CGFunc &cgFunc, BaseNode &arg) {
  MIRFunction &mirFunc = cgFunc.GetFunction();
  MIRModule *mirModule = mirFunc.GetModule();
  MIRFunction *calleeFunc = mirModule->GetMIRBuilder()->GetOrCreateFunction("MCC_ThrowException", (TyIdx)(PTY_void));
  calleeFunc->SetNoReturn();
  MapleVector<BaseNode*> args(mirModule->GetMIRBuilder()->GetCurrentFuncCodeMpAllocator()->Adapter());
  args.push_back(&arg);
  CallNode *callAssign = mirModule->GetMIRBuilder()->CreateStmtCall(calleeFunc->GetPuidx(), args);
  mirFunc.GetBody()->ReplaceStmt1WithStmt2(rethrow, callAssign);
}

void EHThrow::ConvertThrowToRethrow(CGFunc &cgFunc) {
  MIRFunction &mirFunc = cgFunc.GetFunction();
  MIRModule *mirModule = mirFunc.GetModule();
  MIRBuilder *mirBuilder = mirModule->GetMIRBuilder();
  MIRFunction *unFunc = mirBuilder->GetOrCreateFunction("MCC_RethrowException", (TyIdx)PTY_void);
  unFunc->SetNoReturn();
  MapleVector<BaseNode*> args(mirBuilder->GetCurrentFuncCodeMpAllocator()->Adapter());
  args.push_back(rethrow->Opnd(0));
  CallNode *callNode = mirBuilder->CreateStmtCall(unFunc->GetPuidx(), args);
  mirFunc.GetBody()->ReplaceStmt1WithStmt2(rethrow, callNode);
}

void EHThrow::Lower(CGFunc &cgFunc) {
  BaseNode *opnd0 = rethrow->Opnd(0);
  ASSERT(((opnd0->GetPrimType() == LOWERED_PTR_TYPE) || (opnd0->GetPrimType() == PTY_ref)),
         "except a dread of a pointer to get its type");
  MIRFunction &mirFunc = cgFunc.GetFunction();
  MIRModule *mirModule = mirFunc.GetModule();
  MIRBuilder *mirBuilder = mirModule->GetMIRBuilder();
  ASSERT(mirBuilder != nullptr, "get mirBuilder failed in EHThrow::Lower");
  MIRSymbol *mirSymbol = nullptr;
  BaseNode *arg = nullptr;
  MIRType *pstType = nullptr;
  switch (opnd0->GetOpCode()) {
    case OP_dread: {
      DreadNode *drNode = static_cast<DreadNode*>(opnd0);
      mirSymbol = mirFunc.GetLocalOrGlobalSymbol(drNode->GetStIdx());
      ASSERT(mirSymbol != nullptr, "get symbol failed in EHThrow::Lower");
      pstType = mirSymbol->GetType();
      arg = drNode->CloneTree(mirModule->GetCurFuncCodeMPAllocator());
      break;
    }
    case OP_iread: {
      IreadNode *irNode = static_cast<IreadNode*>(opnd0);
      MIRPtrType *pointerTy =
          static_cast<MIRPtrType*>(GlobalTables::GetTypeTable().GetTypeFromTyIdx(irNode->GetTyIdx()));
      if (irNode->GetFieldID() != 0) {
        MIRType *pointedTy = GlobalTables::GetTypeTable().GetTypeFromTyIdx(pointerTy->GetPointedTyIdx());
        MIRStructType *structTy = nullptr;
        if (pointedTy->GetKind() != kTypeJArray) {
          structTy = static_cast<MIRStructType*>(pointedTy);
        } else {
          /* it's a Jarray type. using it's parent's field info: java.lang.Object */
          structTy = static_cast<MIRJarrayType*>(pointedTy)->GetParentType();
        }
        ASSERT(structTy != nullptr, "structTy is nullptr in EHThrow::Lower ");
        pstType = structTy->GetFieldType(irNode->GetFieldID());
      } else {
        pstType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(pointerTy->GetPointedTyIdx());
      }
      arg = irNode->CloneTree(mirModule->GetCurFuncCodeMPAllocator());
      break;
    }
    case OP_regread: {
      RegreadNode *rrNode = static_cast<RegreadNode*>(opnd0);
      MIRPreg *pReg = mirFunc.GetPregTab()->PregFromPregIdx(rrNode->GetRegIdx());
      ASSERT(pReg->GetPrimType() == LOWERED_PTR_TYPE, "must be a pointer type");
      pstType = pReg->GetMIRType();
      arg = rrNode->CloneTree(mirModule->GetCurFuncCodeMPAllocator());
      break;
    }
    case OP_retype: {
      RetypeNode *retypeNode = static_cast<RetypeNode*>(opnd0);
      pstType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(retypeNode->GetTyIdx());
      arg = retypeNode->CloneTree(mirModule->GetCurFuncCodeMPAllocator());
      break;
    }
    case OP_cvt: {
      TypeCvtNode *cvtNode = static_cast<TypeCvtNode*>(opnd0);
      PrimType prmType = cvtNode->GetPrimType();
      // prmType supposed to be Pointer.
      if ((prmType == PTY_ptr) || (prmType == PTY_ref) || (prmType == PTY_a32) || (prmType == PTY_a64)) {
        ConvertThrowToRethrow(cgFunc);
      }
      return;
    }
    default:
      ASSERT(false, " NYI throw something");
  }
  CHECK_FATAL(pstType != nullptr, "pstType is null in EHThrow::Lower");
  if (pstType->GetKind() != kTypePointer) {
    LogInfo::MapleLogger() << "Error in function " << mirFunc.GetName() << "\n";
    rethrow->Dump();
    LogInfo::MapleLogger() << "pstType is supposed to be Pointer, but is not";
    pstType->Dump(0);
    CHECK_FATAL(false, "throw operand type kind must be kTypePointer");
  }

  MIRType *stType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(static_cast<MIRPtrType*>(pstType)->GetPointedTyIdx());
  if (!IsUnderTry()) {
    /*
     * in this case the throw happens without a try...endtry wrapping it, need to generate lsda.
     * insert 2 labels before and after throw
     */
    LabelNode *throwBeginLbl = mirBuilder->CreateStmtLabel(mirBuilder->CreateLabIdx(mirFunc));
    LabelNode *throwEndLbl = mirBuilder->CreateStmtLabel(mirBuilder->CreateLabIdx(mirFunc));
    BlockNode *bodyNode = mirFunc.GetBody();
    bodyNode->InsertBefore(rethrow, throwBeginLbl);
    bodyNode->InsertAfter(rethrow, throwEndLbl);
    startLabel = throwBeginLbl;
    endLabel = throwEndLbl;
  }

  if (stType->GetKind() == kTypeClass) {
    ConvertThrowToRuntime(cgFunc, *arg);
  } else {
    ConvertThrowToRethrow(cgFunc);
  }
}

EHFunc::EHFunc(CGFunc &func)
    : cgFunc(&func),
      tryVec(func.GetFuncScopeAllocator()->Adapter()),
      ehTyTable(func.GetFuncScopeAllocator()->Adapter()),
      ty2IndexTable(std::less<TyIdx>(), func.GetFuncScopeAllocator()->Adapter()),
      rethrowVec(func.GetFuncScopeAllocator()->Adapter()) {}

EHFunc *CGFunc::BuildEHFunc() {
  EHFunc *newEHFunc = GetMemoryPool()->New<EHFunc>(*this);
  SetEHFunc(*newEHFunc);
  std::vector<std::pair<LabelIdx, CatchNode*>> catchVec;
  newEHFunc->CollectEHInformation(catchVec);
  newEHFunc->MergeCatchToTry(catchVec);
  newEHFunc->BuildEHTypeTable(catchVec);
  newEHFunc->InsertEHSwitchTable();
  newEHFunc->InsertCxaAfterEachCatch(catchVec);
  newEHFunc->GenerateCleanupLabel();

  GetBecommon().BeGetOrCreatePointerType(*GlobalTables::GetTypeTable().GetVoid());
  if (newEHFunc->NeedFullLSDA()) {
    newEHFunc->CreateLSDA();
  } else if (newEHFunc->HasThrow()) {
    newEHFunc->LowerThrow();
  }
  if (GetCG()->GetCGOptions().GenerateExceptionHandlingCode()) {
    newEHFunc->CreateTypeInfoSt();
  }

  return newEHFunc;
}

bool EHFunc::NeedFullLSDA() const {
  if (cgFunc->GetFunction().IsJava()) {
    return HasTry();
  } else {
    return false;
  }
}

bool EHFunc::NeedFastLSDA() const {
  if (cgFunc->GetFunction().IsJava()) {
    return !HasTry();
  } else {
    return false;
  }
}

bool EHFunc::HasTry() const {
  return !tryVec.empty();
}

void EHFunc::CreateTypeInfoSt() {
  MIRFunction &mirFunc = cgFunc->GetFunction();
  bool ctorDefined = false;
  if (mirFunc.GetAttr(FUNCATTR_constructor) && !mirFunc.GetAttr(FUNCATTR_static) && (mirFunc.GetBody() != nullptr)) {
    ctorDefined = true;
  }

  if (!ctorDefined) {
    return;
  }

  const auto *classType = static_cast<const MIRClassType*>(mirFunc.GetClassType());
  ASSERT(classType != nullptr, "");
  if (classType->GetMethods().empty() && (classType->GetFieldsSize() == 0)) {
    return;
  }

  if (classType->GetExceptionRootType() == nullptr) {
    return;  /* not a exception type */
  }
}

void EHFunc::LowerThrow() {
  MIRFunction &mirFunc = cgFunc->GetFunction();
  /* just lower without building LSDA */
  for (EHThrow *rethrow : rethrowVec) {
    BaseNode *opnd0 = rethrow->GetRethrow()->Opnd(0);
    /* except a dread of a point to get its type */
    switch (opnd0->GetOpCode()) {
      case OP_retype: {
        RetypeNode *retypeNode = static_cast<RetypeNode*>(opnd0);
        ASSERT(GlobalTables::GetTypeTable().GetTypeFromTyIdx(retypeNode->GetTyIdx())->GetKind() == kTypePointer,
               "expecting a pointer type");
        rethrow->ConvertThrowToRuntime(*cgFunc, *retypeNode->CloneTree(
            mirFunc.GetModule()->GetCurFuncCodeMPAllocator()));
        break;
      }
      case OP_dread: {
        DreadNode *drNode = static_cast<DreadNode*>(opnd0);
        ASSERT(mirFunc.GetLocalOrGlobalSymbol(drNode->GetStIdx())->GetType()->GetKind() == kTypePointer,
               "expect pointer type");
        rethrow->ConvertThrowToRuntime(*cgFunc, *drNode->CloneTree(
            mirFunc.GetModule()->GetCurFuncCodeMPAllocator()));
        break;
      }
      case OP_iread: {
        IreadNode *irNode = static_cast<IreadNode*>(opnd0);
        MIRPtrType *receiverPtrType = nullptr;
        if (irNode->GetFieldID() != 0) {
          MIRPtrType *pointerTy =
              static_cast<MIRPtrType*>(GlobalTables::GetTypeTable().GetTypeFromTyIdx(irNode->GetTyIdx()));
          MIRType *pointedTy = GlobalTables::GetTypeTable().GetTypeFromTyIdx(pointerTy->GetPointedTyIdx());
          MIRStructType *structTy = nullptr;
          if (pointedTy->GetKind() != kTypeJArray) {
            structTy = static_cast<MIRStructType*>(pointedTy);
          } else {
            /* it's a Jarray type. using it's parent's field info: java.lang.Object */
            structTy = static_cast<MIRJarrayType*>(pointedTy)->GetParentType();
          }
          ASSERT(structTy != nullptr, "structTy is nullptr in EHFunc::LowerThrow");
          receiverPtrType =
              static_cast<MIRPtrType*>(structTy->GetFieldType(irNode->GetFieldID()));
        } else {
          receiverPtrType =
              static_cast<MIRPtrType*>(GlobalTables::GetTypeTable().GetTypeFromTyIdx(irNode->GetTyIdx()));
          receiverPtrType = static_cast<MIRPtrType*>(
              GlobalTables::GetTypeTable().GetTypeFromTyIdx(receiverPtrType->GetPointedTyIdx()));
        }
        ASSERT(receiverPtrType->GetKind() == kTypePointer, "expecting a pointer type");
        rethrow->ConvertThrowToRuntime(*cgFunc, *irNode->CloneTree(
            mirFunc.GetModule()->GetCurFuncCodeMPAllocator()));
        break;
      }
      case OP_regread: {
        RegreadNode *rrNode = static_cast<RegreadNode*>(opnd0);
        ASSERT(mirFunc.GetPregTab()->PregFromPregIdx(rrNode->GetRegIdx())->GetPrimType() == LOWERED_PTR_TYPE,
               "expect LOWERED_PTR_TYPE");
        ASSERT(mirFunc.GetPregTab()->PregFromPregIdx(rrNode->GetRegIdx())->GetMIRType()->GetKind() == kTypePointer,
               "expect pointer type");
        rethrow->ConvertThrowToRuntime(*cgFunc, *rrNode->CloneTree(
            mirFunc.GetModule()->GetCurFuncCodeMPAllocator()));
        break;
      }
      case OP_constval: {
        ConstvalNode *constValNode = static_cast<ConstvalNode*>(opnd0);
        BaseNode *newNode = constValNode->CloneTree(mirFunc.GetModule()->GetCurFuncCodeMPAllocator());
        ASSERT(newNode != nullptr, "nullptr check");
        rethrow->ConvertThrowToRuntime(*cgFunc, *newNode);
        break;
      }
      default:
        ASSERT(false, "unexpected or NYI");
    }
  }
}

/*
 * merge catch to try
 */
void EHFunc::MergeCatchToTry(const std::vector<std::pair<LabelIdx, CatchNode*>> &catchVec) {
  size_t tryOffsetCount;
  for (auto *ehTry : tryVec) {
    tryOffsetCount = ehTry->GetTryNode()->GetOffsetsCount();
    for (size_t i = 0; i < tryOffsetCount; i++) {
      auto o = ehTry->GetTryNode()->GetOffset(i);
      for (const auto &catchVecPair : catchVec) {
        LabelIdx lbIdx = catchVecPair.first;
        if (lbIdx == o) {
          ehTry->PushBackCatchVec(*catchVecPair.second);
          break;
        }
      }
    }
    CHECK_FATAL(ehTry->GetCatchVecSize() == tryOffsetCount, "EHTry instance offset does not equal catch node amount.");
  }
}

/* catchvec is going to be released by the caller */
void EHFunc::BuildEHTypeTable(const std::vector<std::pair<LabelIdx, CatchNode*>> &catchVec) {
  if (!catchVec.empty()) {
    /* the first one assume to be <*void> */
    TyIdx voidTyIdx(PTY_void);
    ehTyTable.push_back(voidTyIdx);
    ty2IndexTable[voidTyIdx] = 0;
    /* create void pointer and update becommon's size table */
    cgFunc->GetBecommon().UpdateTypeTable(*GlobalTables::GetTypeTable().GetVoidPtr());
  }

  /* create the type table for this function, just iterate each catch */
  CatchNode *jCatchNode = nullptr;
  size_t catchNodeSize;
  for (const auto &catchVecPair : catchVec) {
    jCatchNode = catchVecPair.second;
    catchNodeSize = jCatchNode->Size();
    for (size_t i = 0; i < catchNodeSize; i++) {
      MIRType *mirTy = GlobalTables::GetTypeTable().GetTypeFromTyIdx(jCatchNode->GetExceptionTyIdxVecElement(i));
      ASSERT(mirTy->GetKind() == kTypePointer, "mirTy is not pointer type");
      TyIdx ehTyIdx = static_cast<MIRPtrType*>(mirTy)->GetPointedTyIdx();
      if (ty2IndexTable.find(ehTyIdx) != ty2IndexTable.end()) {
        continue;
      }

      ty2IndexTable[ehTyIdx] = ehTyTable.size();
      ehTyTable.push_back(ehTyIdx);
      MIRClassType *catchType = static_cast<MIRClassType*>(GlobalTables::GetTypeTable().GetTypeFromTyIdx(ehTyIdx));
      MIRClassType *rootType = catchType->GetExceptionRootType();
      if (rootType == nullptr) {
        rootType = static_cast<MIRClassType*>(GlobalTables::GetTypeTable().GetOrCreateClassType(
            "Ljava_2Flang_2FThrowable_3B", *GlobalTables::GetGsymTable().GetModule()));
        catchType->SetParentTyIdx(rootType->GetTypeIndex());
      }
    }
  }
}

void EHFunc::DumpEHFunc() const {
  MIRModule &mirModule = *cgFunc->GetFunction().GetModule();
  for (uint32 i = 0; i < this->tryVec.size(); i++) {
    LogInfo::MapleLogger() << "\n========== start " << i << " th eh:\n";
    EHTry *ehTry = tryVec[i];
    ehTry->DumpEHTry(mirModule);
    LogInfo::MapleLogger() << "========== end " << i << " th eh =========\n";
  }

  LogInfo::MapleLogger() << "\n========== start LSDA type table ========\n";
  for (uint32 i = 0; i < this->ehTyTable.size(); i++) {
    LogInfo::MapleLogger() << i << " vector to ";
    GlobalTables::GetTypeTable().GetTypeFromTyIdx(ehTyTable[i])->Dump(0);
    LogInfo::MapleLogger() << "\n";
  }
  LogInfo::MapleLogger() << "========== end LSDA type table ========\n";

  LogInfo::MapleLogger() << "\n========== start type-index map ========\n";
  for (const auto &ty2indexTablePair : ty2IndexTable) {
    GlobalTables::GetTypeTable().GetTypeFromTyIdx(ty2indexTablePair.first)->Dump(0);
    LogInfo::MapleLogger() << " map to ";
    LogInfo::MapleLogger() << ty2indexTablePair.second << "\n";
  }
  LogInfo::MapleLogger() << "========== end type-index map ========\n";
}

/*
 * cleanup_label is an LabelNode, and placed just before endLabel.
 * cleanup_label is the first statement of cleanupbb.
 * the layout of clean up code is:
 * //return bb
 *   ...
 * //cleanup bb = lastbb->prev; cleanupbb->PrependBB(retbb)
 *   cleanup_label:
 *     ...
 * //lastbb
 *   endLabel:
 *     .cfi_endproc
 *   .Label.xx.end:
 *     .size
 */
void EHFunc::GenerateCleanupLabel() {
  MIRModule *mirModule = cgFunc->GetFunction().GetModule();
  cgFunc->SetCleanupLabel(*mirModule->GetMIRBuilder()->CreateStmtLabel(CreateLabel(".LCLEANUP")));
  BlockNode *blockNode = cgFunc->GetFunction().GetBody();
  blockNode->InsertBefore(cgFunc->GetEndLabel(), cgFunc->GetCleanupLabel());
}

void EHFunc::InsertDefaultLabelAndAbortFunc(BlockNode &blkNode, SwitchNode &switchNode, StmtNode &beforeEndLabel) {
  MIRModule &mirModule = *cgFunc->GetFunction().GetModule();
  LabelIdx dfLabIdx = cgFunc->GetFunction().GetLabelTab()->CreateLabel();
  cgFunc->GetFunction().GetLabelTab()->AddToStringLabelMap(dfLabIdx);
  StmtNode *dfLabStmt = mirModule.GetMIRBuilder()->CreateStmtLabel(dfLabIdx);
  blkNode.InsertAfter(&beforeEndLabel, dfLabStmt);
  MIRFunction *calleeFunc = mirModule.GetMIRBuilder()->GetOrCreateFunction("abort", (TyIdx)(PTY_void));
  MapleVector<BaseNode*> args(mirModule.GetMIRBuilder()->GetCurrentFuncCodeMpAllocator()->Adapter());
  CallNode *callExit = mirModule.GetMIRBuilder()->CreateStmtCall(calleeFunc->GetPuidx(), args);
  blkNode.InsertAfter(dfLabStmt, callExit);
  switchNode.SetDefaultLabel(dfLabIdx);
}

void EHFunc::FillSwitchTable(SwitchNode &switchNode, const EHTry &ehTry) {
  CatchNode *catchNode = nullptr;
  MIRType *exceptionType = nullptr;
  MIRPtrType *ptType = nullptr;
  size_t catchVecSize = ehTry.GetCatchVecSize();
  /* update switch node's cases */
  for (size_t i = 0; i < catchVecSize; i++) {
    catchNode = ehTry.GetCatchNodeAt(i);
    for (size_t j = 0; j < catchNode->Size(); j++) {
      exceptionType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(catchNode->GetExceptionTyIdxVecElement(j));
      ptType = static_cast<MIRPtrType*>(exceptionType);
      MapleMap<TyIdx, uint32>::iterator ty2IdxIt = ty2IndexTable.find(ptType->GetPointedTyIdx());
      ASSERT(ty2IdxIt != ty2IndexTable.end(), "find tyIdx failed!");
      uint32 tableIdx = ty2IdxIt->second;
      LabelNode *catchLabelNode = static_cast<LabelNode*>(catchNode->GetPrev());
      CasePair p(tableIdx, catchLabelNode->GetLabelIdx());
      bool inserted = false;
      for (auto x : switchNode.GetSwitchTable()) {
        if (x == p) {
          inserted = true;
          break;
        }
      }
      if (!inserted) {
        switchNode.InsertCasePair(p);
      }
    }
  }
}

/* this is also the landing pad code. */
void EHFunc::InsertEHSwitchTable() {
  MIRModule &mirModule = *cgFunc->GetFunction().GetModule();
  BlockNode *blockNode = cgFunc->GetFunction().GetBody();
  CHECK_FATAL(blockNode != nullptr, "get function body failed in EHThrow::InsertEHSwitchTable");
  StmtNode *endLabelPrevNode = nullptr;
  SwitchNode *switchNode = nullptr;
  for (auto *ehTry : tryVec) {
    endLabelPrevNode = cgFunc->GetEndLabel()->GetPrev();
    /*
     * get the next statement of the trynode. when no throw happend in try block, jump to the statement directly
     * create a switch statement and insert after tryend;
     */
    switchNode = mirModule.CurFuncCodeMemPool()->New<SwitchNode>(mirModule);
    /* create a new label as default, and if program excute here, error it */
    InsertDefaultLabelAndAbortFunc(*blockNode, *switchNode, *endLabelPrevNode);
    /* create s special symbol that use the second return of __builtin_eh_return() */
    MIRSymbol *mirSymbol = mirModule.GetMIRBuilder()->CreateSymbol(TyIdx(PTY_i32), "__eh_index__", kStVar, kScAuto,
                                                                   &cgFunc->GetFunction(), kScopeLocal);
    switchNode->SetSwitchOpnd(mirModule.GetMIRBuilder()->CreateExprDread(*mirSymbol));
    FillSwitchTable(*switchNode, *ehTry);
    SwitchLowerer switchLower(mirModule, *switchNode, *cgFunc->GetFuncScopeAllocator());
    blockNode->InsertBlockAfter(*switchLower.LowerSwitch(), endLabelPrevNode);
    ehTry->SetFallthruGoto(endLabelPrevNode->GetNext());
  }
  if (!cgFunc->GetCG()->IsQuiet()) {
    cgFunc->GetFunction().Dump();
  }
}

LabelIdx EHFunc::CreateLabel(const std::string &cstr) {
  MIRSymbol *mirSymbol = GlobalTables::GetGsymTable().GetSymbolFromStidx(cgFunc->GetFunction().GetStIdx().Idx());
  CHECK_FATAL(mirSymbol != nullptr, "get function symbol failed in EHFunc::CreateLabel");
  std::string funcName = mirSymbol->GetName();
  std::string labStr = funcName.append(cstr).append(std::to_string(labelIdx++));
  return cgFunc->GetFunction().GetOrCreateLableIdxFromName(labStr);
}

/* think about moving this to BELowerer where LowerThrownval is already written */
void EHFunc::InsertCxaAfterEachCatch(const std::vector<std ::pair<LabelIdx, CatchNode*>> &catchVec) {
  MIRModule &mirModule = *cgFunc->GetFunction().GetModule();
  BlockNode *funcBody = cgFunc->GetFunction().GetBody();
  CatchNode *jCatchNode = nullptr;
  TyIdx voidPTy = GlobalTables::GetTypeTable().GetVoidPtr()->GetTypeIndex();
  for (const auto &catchVecPair : catchVec) {
    jCatchNode = catchVecPair.second;
    MIRFunction *calleeFunc = mirModule.GetMIRBuilder()->GetOrCreateFunction("MCC_JavaBeginCatch", voidPTy);
    RegreadNode *retRegRead0 = mirModule.CurFuncCodeMemPool()->New<RegreadNode>();
    retRegRead0->SetRegIdx(-kSregRetval0);
    retRegRead0->SetPrimType(LOWERED_PTR_TYPE);
    MapleVector<BaseNode*> args(mirModule.GetMIRBuilder()->GetCurrentFuncCodeMpAllocator()->Adapter());
    args.push_back(retRegRead0);
    CallNode *callAssign = mirModule.GetMIRBuilder()->CreateStmtCall(calleeFunc->GetPuidx(), args);
    funcBody->InsertAfter(jCatchNode, callAssign);
  }
}

void EHFunc::CreateLSDAHeader() {
  constexpr uint8 startEncoding = 0xff;
  constexpr uint8 typeEncoding = 0x9b;
  constexpr uint8 callSiteEncoding = 0x1;
  MIRBuilder *mirBuilder = cgFunc->GetFunction().GetModule()->GetMIRBuilder();

  LSDAHeader *lsdaHeaders = cgFunc->GetMemoryPool()->New<LSDAHeader>();
  LabelIdx lsdaHdLblIdx = CreateLabel("LSDAHD");  /* LSDA head */
  LabelNode *lsdaHdLblNode = mirBuilder->CreateStmtLabel(lsdaHdLblIdx);
  lsdaHeaders->SetLSDALabel(*lsdaHdLblNode);

  LabelIdx lsdaTTStartIdx = CreateLabel("LSDAALLS");  /* LSDA all start; */
  LabelNode *lsdaTTLblNode = mirBuilder->CreateStmtLabel(lsdaTTStartIdx);
  LabelIdx lsdaTTEndIdx = CreateLabel("LSDAALLE");  /* LSDA all end; */
  LabelNode *lsdaCSTELblNode = mirBuilder->CreateStmtLabel(lsdaTTEndIdx);
  lsdaHeaders->SetTTypeOffset(lsdaTTLblNode, lsdaCSTELblNode);

  lsdaHeaders->SetLPStartEncoding(startEncoding);
  lsdaHeaders->SetTTypeEncoding(typeEncoding);
  lsdaHeaders->SetCallSiteEncoding(callSiteEncoding);
  lsdaHeader = lsdaHeaders;
}

void EHFunc::FillLSDACallSiteTable() {
  constexpr uint8 callSiteFirstAction = 0x1;
  MIRBuilder *mirBuilder = cgFunc->GetFunction().GetModule()->GetMIRBuilder();
  BlockNode *bodyNode = cgFunc->GetFunction().GetBody();

  lsdaCallSiteTable = cgFunc->GetMemoryPool()->New<LSDACallSiteTable>(*cgFunc->GetFuncScopeAllocator());
  LabelIdx lsdaCSTStartIdx = CreateLabel("LSDACSTS");  /* LSDA callsite table start; */
  LabelNode *lsdaCSTStartLabel = mirBuilder->CreateStmtLabel(lsdaCSTStartIdx);
  LabelIdx lsdaCSTEndIdx = CreateLabel("LSDACSTE");  /* LSDA callsite table end; */
  LabelNode *lsdaCSTEndLabel = mirBuilder->CreateStmtLabel(lsdaCSTEndIdx);
  lsdaCallSiteTable->SetCSTable(lsdaCSTStartLabel, lsdaCSTEndLabel);

  /* create LDSACallSite for each EHTry instance */
  for (auto *ehTry : tryVec) {
    ASSERT(ehTry != nullptr, "null ptr check");
    /* replace try with a label which is the callsite_start */
    LabelIdx csStartLblIdx = CreateLabel("LSDACS");
    LabelNode *csLblNode = mirBuilder->CreateStmtLabel(csStartLblIdx);
    LabelIdx csEndLblIdx = CreateLabel("LSDACE");
    LabelNode *ceLblNode = mirBuilder->CreateStmtLabel(csEndLblIdx);
    TryNode *tryNode = ehTry->GetTryNode();
    bodyNode->ReplaceStmt1WithStmt2(tryNode, csLblNode);
    StmtNode *endTryNode = ehTry->GetEndtryNode();
    bodyNode->ReplaceStmt1WithStmt2(endTryNode, ceLblNode);

    LabelNode *ladpadEndLabel = nullptr;
    if (ehTry->GetFallthruGoto()) {
      ladpadEndLabel = mirBuilder->CreateStmtLabel(CreateLabel("LSDALPE"));
      bodyNode->InsertBefore(ehTry->GetFallthruGoto(), ladpadEndLabel);
    } else {
      ladpadEndLabel = ceLblNode;
    }
    /* When there is only one catch, the exception table is optimized. */
    if (ehTry->GetCatchVecSize() == 1) {
      ladpadEndLabel = static_cast<LabelNode*>(ehTry->GetCatchNodeAt(0)->GetPrev());
    }

    LSDACallSite *lsdaCallSite = cgFunc->GetMemoryPool()->New<LSDACallSite>();
    LabelPair csStart(cgFunc->GetStartLabel(), csLblNode);
    LabelPair csLength(csLblNode, ceLblNode);
    LabelPair csLandingPad(cgFunc->GetStartLabel(), ladpadEndLabel);
    lsdaCallSite->Init(csStart, csLength, csLandingPad, callSiteFirstAction);
    ehTry->SetLSDACallSite(*lsdaCallSite);
    lsdaCallSiteTable->PushBack(*lsdaCallSite);
  }
}

void EHFunc::CreateLSDA() {
  constexpr uint8 callSiteCleanUpAction = 0x0;
  /* create header */
  CreateLSDAHeader();
  /* create and fill callsite table */
  FillLSDACallSiteTable();

  for (auto *rethrow : rethrowVec) {
    ASSERT(rethrow != nullptr, "null ptr check");
    /* replace throw (void * obj) with call __java_rethrow and unwind resume */
    rethrow->Lower(*cgFunc);
    if (rethrow->HasLSDA()) {
      LSDACallSite *lsdaCallSite = cgFunc->GetMemoryPool()->New<LSDACallSite>();
      LabelPair csStart(cgFunc->GetStartLabel(), rethrow->GetStartLabel());
      LabelPair csLength(rethrow->GetStartLabel(), rethrow->GetEndLabel());
      LabelPair csLandingPad(nullptr, nullptr);
      lsdaCallSite->Init(csStart, csLength, csLandingPad, callSiteCleanUpAction);
      lsdaCallSiteTable->PushBack(*lsdaCallSite);
    }
  }

  /* LSDAAction table */
  CreateLSDAAction();
}

void EHFunc::CreateLSDAAction() {
  constexpr uint8 actionTableNextEncoding = 0x7d;
  /* iterate each try and its corresponding catch */
  LSDAActionTable *actionTable = cgFunc->GetMemoryPool()->New<LSDAActionTable>(*cgFunc->GetFuncScopeAllocator());
  lsdaActionTable = actionTable;

  for (auto *ehTry : tryVec) {
    LSDAAction *lastAction = nullptr;
    for (int32 j = ehTry->GetCatchVecSize() - 1; j >= 0; --j) {
      CatchNode *catchNode = ehTry->GetCatchNodeAt(j);
      ASSERT(catchNode != nullptr, "null ptr check");
      for (uint32 idx = 0; idx < catchNode->Size(); ++idx) {
        MIRPtrType *ptType = static_cast<MIRPtrType*>(
            GlobalTables::GetTypeTable().GetTypeFromTyIdx(catchNode->GetExceptionTyIdxVecElement(idx)));
        uint32 tyIndex = ty2IndexTable[ptType->GetPointedTyIdx()];  /* get the index of ptType of ehTyTable; */
        ASSERT(tyIndex != 0, "exception type index not allow equal zero");
        LSDAAction *lsdaAction =
            cgFunc->GetMemoryPool()->New<LSDAAction>(tyIndex, lastAction == nullptr ? 0 : actionTableNextEncoding);
        lastAction = lsdaAction;
        actionTable->PushBack(*lsdaAction);
      }
    }

    /* record actionTable group offset, per LSDAAction object in actionTable occupy 2 bytes */
    ehTry->SetCSAction((actionTable->Size() - 1) * 2 + 1);
  }
}

AnalysisResult *CgDoBuildEHFunc::Run(CGFunc *cgFunc, CgFuncResultMgr *cgFuncResultMgr) {
  (void)cgFuncResultMgr;
  CHECK_FATAL(cgFunc != nullptr, "expect a cgFunc in CgDoBuildEHFunc");
  cgFunc->BuildEHFunc();
  return nullptr;
}
}  /* namespace maplebe */
