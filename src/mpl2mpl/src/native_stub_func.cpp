/*
 * Copyright (c) [2019-2020] Huawei Technologies Co.,Ltd.All rights reserved.
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
#include "native_stub_func.h"
#include <iostream>
#include <fstream>
#include "namemangler.h"
#include "vtable_analysis.h"
#include "reflection_analysis.h"
#include "metadata_layout.h"

// NativeStubFunc
// This phase is the processing of the java native function. It
// generates an extra stubFunc for each native function, preparing
// for the preparations before the actual native function is called,
// including the parameter mapping, GC preparation, and so on.
namespace maple {
NativeStubFuncGeneration::NativeStubFuncGeneration(MIRModule &mod, KlassHierarchy *kh, bool dump)
    : FuncOptimizeImpl(mod, kh, dump) {
  MIRType *jstrType = GlobalTables::GetTypeTable().GetOrCreateClassType(
      NameMangler::GetInternalNameLiteral(NameMangler::kJavaLangStringStr), mod);
  auto *jstrPointerType =
      static_cast<MIRPtrType*>(GlobalTables::GetTypeTable().GetOrCreatePointerType(*jstrType, PTY_ref));
  jstrPointerTypeIdx = jstrPointerType->GetTypeIndex();
  GenerateRegTableEntryType();
  GenerateHelperFuncDecl();
  GenerateRegFuncTabEntryType();
  InitStaticBindingMethodList();
}

MIRFunction &NativeStubFuncGeneration::GetOrCreateDefaultNativeFunc(MIRFunction &stubFunc) {
  // If only support dynamic binding , we won't stub any weak symbols
  if (Options::regNativeDynamicOnly && !(IsStaticBindingMethod(stubFunc.GetName()))) {
    return stubFunc;
  }
  std::string nativeName = NameMangler::NativeJavaName(stubFunc.GetName().c_str());
  // No need to create a default function with exact arguments here
  MIRFunction *nativeFunc = builder->GetOrCreateFunction(nativeName, stubFunc.GetReturnTyIdx());
  ASSERT(nativeFunc != nullptr, "null ptr check!");
  nativeFunc->GetSrcPosition().SetMplLineNum(stubFunc.GetSrcPosition().MplLineNum());
  if (nativeFunc->GetBody() == nullptr) {
    builder->SetCurrentFunction(*nativeFunc);
    nativeFunc->SetAttr(FUNCATTR_weak);
    nativeFunc->NewBody();
    // We would not throw exception here.
    // Use regnative-dynamic-only option when run case expr14301a_setFields__IF as qemu solution.
    MIRType *voidPointerType = GlobalTables::GetTypeTable().GetVoidPtr();
    // It will throw java.lang.UnsatisfiedLinkError, while issue a runtime
    // warning on Qemu/arm64-server (because it lacks most of the required native libraries)
    MIRFunction *findNativeFunc = nullptr;
    if (ReturnsJstr(stubFunc.GetReturnTyIdx())) {
      // a dialet for string
      findNativeFunc = builder->GetOrCreateFunction("MCC_CannotFindNativeMethod_S", voidPointerType->GetTypeIndex());
    } else {
      MIRType *returnType = stubFunc.GetReturnType();
      if ((returnType->GetKind() == kTypePointer) &&
          ((static_cast<MIRPtrType*>(returnType))->GetPointedType()->GetKind() == kTypeJArray)) {
        // a dialet for array
        findNativeFunc = builder->GetOrCreateFunction("MCC_CannotFindNativeMethod_A", voidPointerType->GetTypeIndex());
      }
    }
    // default callback
    if (findNativeFunc == nullptr) {
      findNativeFunc = builder->GetOrCreateFunction("MCC_CannotFindNativeMethod", voidPointerType->GetTypeIndex());
    }
    findNativeFunc->SetAttr(FUNCATTR_nosideeffect);
    // fatal message parameter
    std::string nativeSymbolName = stubFunc.GetName();
    UStrIdx strIdx = GlobalTables::GetUStrTable().GetOrCreateStrIdxFromName(nativeSymbolName);
    auto *signatureNode = nativeFunc->GetCodeMempool()->New<ConststrNode>(strIdx);
    signatureNode->SetPrimType(PTY_ptr);
    MapleVector<BaseNode*> args(builder->GetCurrentFuncCodeMpAllocator()->Adapter());
    args.push_back(signatureNode);
    CallNode *callGetFindNativeFunc =
        builder->CreateStmtCallAssigned(findNativeFunc->GetPuidx(), args, nullptr, OP_callassigned);
    nativeFunc->GetBody()->AddStatement(callGetFindNativeFunc);
    GetMIRModule().AddFunction(nativeFunc);
    builder->SetCurrentFunction(stubFunc);
  }
  return *nativeFunc;
}

//   Create function body of the stub func.
//   syncenter (dread ref %_this or classinfo) // if native func is synchronized
//   if (not critical_native)
//     callassigned MCC_PreNativeCall() {Env}
//     call native_func(Env, [classinfo], oringinal_args){retv};
//   else
//     call native_func(oringinal_args){};
//   if (type of retv is ref)
//     callassigned &MCC_DecodeReference(dread ref %retv) {dassign ref %retv}
//   if (not critical_native)
//     callassigned &__MRT_PostNativeCall (dread ptr %env_ptr) {}
//   syncexit (dread ref %_this or classinfo) // if native func is synchronized
//   if (not critical_native)
//     callassigned &MCC_CheckThrowPendingException () {}
void NativeStubFuncGeneration::ProcessFunc(MIRFunction *func) {
  // FUNCATTR_bridge for function to exclude
  ASSERT(func != nullptr, "null ptr check!");
  if (func->GetBody() == nullptr || func->GetAttr(FUNCATTR_bridge) ||
      (!func->GetAttr(FUNCATTR_native) &&
       !func->GetAttr(FUNCATTR_fast_native) &&
       !func->GetAttr(FUNCATTR_critical_native))) {
    return;
  }
  SetCurrentFunction(*func);
  // Has been processed by previous phases, such as simplify.
  if (func->GetBody()->GetFirst()) {
    GenerateRegTabEntry(*func);
    GenerateRegFuncTabEntry();
    func->UnSetAttr(FUNCATTR_native);
    func->UnSetAttr(FUNCATTR_fast_native);
    func->UnSetAttr(FUNCATTR_critical_native);
    return;
  }
  func->GetBody()->ResetBlock();
  NativeFuncProperty funcProperty;
  bool needNativeCall = (!func->GetAttr(FUNCATTR_critical_native)) && (funcProperty.jniType == kJniTypeNormal);
  GStrIdx classObjSymStrIdx =
      GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(CLASSINFO_PREFIX_STR + func->GetBaseClassName());
  MIRSymbol *classObjSymbol = GlobalTables::GetGsymTable().GetSymbolFromStrIdx(classObjSymStrIdx);
  ASSERT(classObjSymbol != nullptr, "Classinfo for %s is not found", func->GetBaseClassName().c_str());
  // Generate MonitorEnter if this is a synchronized method
  if (func->GetAttr(FUNCATTR_synchronized)) {
    BaseNode *monitor = nullptr;
    if (func->GetAttr(FUNCATTR_static)) {
      // Grab class object
      monitor = builder->CreateExprAddrof(0, *classObjSymbol);
    } else {
      // Grab _this pointer
      const size_t funcFormalsSize = func->GetFormalCount();
      CHECK_FATAL(funcFormalsSize > 0, "container check");
      MIRSymbol *formal0St = func->GetFormal(0);
      if (formal0St->GetSKind() == kStPreg) {
        monitor =
            builder->CreateExprRegread(formal0St->GetType()->GetPrimType(),
                                       func->GetPregTab()->GetPregIdxFromPregno(formal0St->GetPreg()->GetPregNo()));
      } else {
        monitor = builder->CreateExprDread(*formal0St);
      }
    }
    NaryStmtNode *syncEnter = builder->CreateStmtNary(OP_syncenter, monitor);
    func->GetBody()->AddStatement(syncEnter);
  }
  // Get Env pointer through MCC_PreNativeCall(){Env}
  MIRSymbol *envPtrSym = nullptr;
  PregIdx envPregIdx = 0;
  if (Options::usePreg) {
    envPregIdx = func->GetPregTab()->CreatePreg(PTY_ptr);
  } else {
    envPtrSym = builder->CreateSymbol(GlobalTables::GetTypeTable().GetVoidPtr()->GetTypeIndex(), "env_ptr", kStVar,
                                      kScAuto, func, kScopeLocal);
  }
  MapleVector<BaseNode*> args(func->GetCodeMempoolAllocator().Adapter());
  CallNode *preFuncCall =
      Options::usePreg
      ? builder->CreateStmtCallRegassigned(MRTPreNativeFunc->GetPuidx(), args, envPregIdx, OP_callassigned)
      : builder->CreateStmtCallAssigned(MRTPreNativeFunc->GetPuidx(), args, envPtrSym, OP_callassigned);
  // Use Env as an arg, MCC_PostNativeCall(Env)
  args.push_back(Options::usePreg ? (static_cast<BaseNode*>(builder->CreateExprRegread(PTY_ptr, envPregIdx)))
                                  : (static_cast<BaseNode*>(builder->CreateExprDread(*envPtrSym))));
  CallNode *postFuncCall =
      builder->CreateStmtCallAssigned(MRTPostNativeFunc->GetPuidx(), args, nullptr, OP_callassigned);

  MapleVector<BaseNode*> allocCallArgs(func->GetCodeMempoolAllocator().Adapter());
  if (!func->GetAttr(FUNCATTR_critical_native)) {
    if (needNativeCall) {
      func->GetBody()->AddStatement(preFuncCall);
      // set up env
      allocCallArgs.push_back(Options::usePreg
                              ? (static_cast<BaseNode*>(builder->CreateExprRegread(PTY_ptr, envPregIdx)))
                              : (static_cast<BaseNode*>(builder->CreateExprDread(*envPtrSym))));
    } else {
      // set up env
      allocCallArgs.push_back(builder->CreateIntConst(0, PTY_i32));
    }
    // set up class
    if (func->GetAttr(FUNCATTR_static)) {
      allocCallArgs.push_back(builder->CreateExprAddrof(0, *classObjSymbol));
    }
  }
  for (uint32 i = 0; i < func->GetFormalCount(); ++i) {
    auto argSt = func->GetFormal(i);
    BaseNode *argExpr = nullptr;
    if (argSt->GetSKind() == kStPreg) {
      argExpr = builder->CreateExprRegread(argSt->GetType()->GetPrimType(),
                                           func->GetPregTab()->GetPregIdxFromPregno(argSt->GetPreg()->GetPregNo()));
    } else {
      argExpr = builder->CreateExprDread(*argSt);
    }
    allocCallArgs.push_back(argExpr);
  }
  bool voidRet = (func->GetReturnType()->GetPrimType() == PTY_void);
  MIRSymbol *stubFuncRet = nullptr;
  if (!voidRet) {
    stubFuncRet = builder->CreateSymbol(func->GetReturnTyIdx(), "retvar_stubfunc", kStVar, kScAuto, func, kScopeLocal);
  }
  MIRFunction &nativeFunc = GetOrCreateDefaultNativeFunc(*func);
  if (Options::regNativeFunc) {
    GenerateRegisteredNativeFuncCall(*func, nativeFunc, allocCallArgs, stubFuncRet);
  }
  bool needDecodeRef = (func->GetReturnType()->GetPrimType() == PTY_ref);
  if (needDecodeRef) {
    MapleVector<BaseNode*> decodeArgs(func->GetCodeMempoolAllocator().Adapter());
    CHECK_FATAL(stubFuncRet != nullptr, "stubfunc_ret is nullptr");
    decodeArgs.push_back(builder->CreateExprDread(*stubFuncRet));
    CallNode *decodeFuncCall =
        builder->CreateStmtCallAssigned(MRTDecodeRefFunc->GetPuidx(), decodeArgs, stubFuncRet, OP_callassigned);
    func->GetBody()->AddStatement(decodeFuncCall);
  }
  if (needNativeCall) {
    func->GetBody()->AddStatement(postFuncCall);
  }
  // Generate MonitorExit if this is a synchronized method
  if (func->GetAttr(FUNCATTR_synchronized)) {
    BaseNode *monitor = nullptr;
    if (func->GetAttr(FUNCATTR_static)) {
      // Grab class object
      monitor = builder->CreateExprAddrof(0, *classObjSymbol);
    } else {
      // Grab _this pointer
      MIRSymbol *formal0St = func->GetFormal(0);
      if (formal0St->GetSKind() == kStPreg) {
        monitor =
            builder->CreateExprRegread(formal0St->GetType()->GetPrimType(),
                                       func->GetPregTab()->GetPregIdxFromPregno(formal0St->GetPreg()->GetPregNo()));
      } else {
        monitor = builder->CreateExprDread(*formal0St);
      }
    }
    NaryStmtNode *syncExit = builder->CreateStmtNary(OP_syncexit, monitor);
    func->GetBody()->AddStatement(syncExit);
  }
  bool needCheckExceptionCall = needNativeCall;
  // check pending exception just before leaving this stub frame except for critical natives
  if (needCheckExceptionCall) {
    MapleVector<BaseNode*> getExceptArgs(func->GetCodeMempoolAllocator().Adapter());
    CallNode *callGetExceptFunc = builder->CreateStmtCallAssigned(MRTCheckThrowPendingExceptionFunc->GetPuidx(),
                                                                  getExceptArgs, nullptr, OP_callassigned);
    func->GetBody()->AddStatement(callGetExceptFunc);
  } else if (!func->GetAttr(FUNCATTR_critical_native)) {
    MapleVector<BaseNode*> frameStatusArgs(func->GetCodeMempoolAllocator().Adapter());
    CallNode *callSetFrameStatusFunc = builder->CreateStmtCallAssigned(MCCSetReliableUnwindContextFunc->GetPuidx(),
                                                                       frameStatusArgs, nullptr, OP_callassigned);
    func->GetBody()->AddStatement(callSetFrameStatusFunc);
  }
  if (!voidRet) {
    StmtNode *stmt = builder->CreateStmtReturn(builder->CreateExprDread(*stubFuncRet));
    func->GetBody()->AddStatement(stmt);
  }
}

void NativeStubFuncGeneration::GenerateRegFuncTabEntryType() {
  MIRArrayType &arrayType =
      *GlobalTables::GetTypeTable().GetOrCreateArrayType(*GlobalTables::GetTypeTable().GetVoidPtr(), 0);
  regFuncTabConst = GetMIRModule().GetMemPool()->New<MIRAggConst>(GetMIRModule(), arrayType);
  std::string regFuncTab = NameMangler::kRegJNIFuncTabPrefixStr + GetMIRModule().GetFileNameAsPostfix();
  regFuncSymbol = builder->CreateSymbol(regFuncTabConst->GetType().GetTypeIndex(), regFuncTab, kStVar,
                                        kScGlobal, nullptr, kScopeGlobal);
}

void NativeStubFuncGeneration::GenerateRegFuncTabEntry() {
#ifdef USE_ARM32_MACRO
  constexpr int locIdxShift = 3;
#else
  constexpr int locIdxShift = 4;
#endif

#ifdef TARGARM32
  // LSB can not be used to mark unresolved entries in java native method table for arm, since this bit
  // is used to switch arm mode and thumb mode (when LSB of pc is set to 1)
  constexpr uint64 locIdxMask = 0;
#else
  constexpr uint64 locIdxMask = 0x01;
#endif

  uint64 locIdx = regFuncTabConst->GetConstVec().size();
  auto *newConst =
    GlobalTables::GetIntConstTable().GetOrCreateIntConst(static_cast<int64>((locIdx << locIdxShift) | locIdxMask),
                                                         *GlobalTables::GetTypeTable().GetVoidPtr());
  regFuncTabConst->PushBack(newConst);
}

void NativeStubFuncGeneration::GenerateRegFuncTab() {
  auto &arrayType = static_cast<MIRArrayType&>(regFuncTabConst->GetType());
  arrayType.SetSizeArrayItem(0, regFuncTabConst->GetConstVec().size());
  regFuncSymbol->SetKonst(regFuncTabConst);
}

void NativeStubFuncGeneration::GenerateRegTabEntry(const MIRFunction &func) {
  std::string tmp = func.GetName();
  tmp = NameMangler::DecodeName(tmp);
  std::string base = func.GetBaseClassName();
  base = NameMangler::DecodeName(base);
  if (tmp.length() > base.length() && tmp.find(base) != std::string::npos) {
    tmp.replace(tmp.find(base), base.length() + 1, "");
  }
  uint32 baseFuncNameWithTypeIdx = ReflectionAnalysis::FindOrInsertRepeatString(tmp, true);    // always used
  uint32 classIdx = ReflectionAnalysis::FindOrInsertRepeatString(base, true);  // always used
  // Using MIRIntConst instead of MIRStruct for RegTable.
  auto *baseConst =
      GlobalTables::GetIntConstTable().GetOrCreateIntConst(classIdx, *GlobalTables::GetTypeTable().GetVoidPtr());
  regTableConst->PushBack(baseConst);
  auto *newConst = GlobalTables::GetIntConstTable().GetOrCreateIntConst(baseFuncNameWithTypeIdx,
                                                                        *GlobalTables::GetTypeTable().GetVoidPtr());
  regTableConst->PushBack(newConst);
}

void NativeStubFuncGeneration::GenerateRegisteredNativeFuncCall(MIRFunction &func, const MIRFunction &nativeFunc,
                                                                MapleVector<BaseNode*> &args, const MIRSymbol *ret) {
  GenerateRegTabEntry(func);
  GenerateRegFuncTabEntry();
  CallReturnVector nrets(func.GetCodeMempoolAllocator().Adapter());
  if (ret != nullptr) {
    CHECK_FATAL((ret->GetStorageClass() == kScAuto || ret->GetStorageClass() == kScFormal ||
                 ret->GetStorageClass() == kScExtern || ret->GetStorageClass() == kScGlobal),
                "unknow classtype! check it!");
    nrets.push_back(CallReturnPair(ret->GetStIdx(), RegFieldPair(0, 0)));
  }
  size_t loc = regFuncTabConst->GetConstVec().size();
  auto &regArrayType = static_cast<MIRArrayType&>(regFuncTabConst->GetType());
  AddrofNode *regFuncExpr = builder->CreateExprAddrof(0, *regFuncSymbol);
  ArrayNode *arrayExpr = builder->CreateExprArray(regArrayType, regFuncExpr, builder->CreateIntConst(loc - 1, PTY_i32));
  arrayExpr->SetBoundsCheck(false);
  auto *elemType = static_cast<MIRArrayType&>(regArrayType).GetElemType();
  BaseNode *ireadExpr =
      builder->CreateExprIread(*elemType, *GlobalTables::GetTypeTable().GetOrCreatePointerType(*elemType),
                               0, arrayExpr);
  // assign registered func ptr to a preg.
  auto funcptrPreg = func.GetPregTab()->CreatePreg(PTY_ptr);
  RegassignNode *funcptrAssign = builder->CreateStmtRegassign(PTY_ptr, funcptrPreg, ireadExpr);
  // read func ptr from preg
  auto readFuncPtr = builder->CreateExprRegread(PTY_ptr, funcptrPreg);
  NativeFuncProperty funcProperty;
  bool needCheckThrowPendingExceptionFunc =
      (!func.GetAttr(FUNCATTR_critical_native)) && (funcProperty.jniType == kJniTypeNormal);
  bool needIndirectCall = func.GetAttr(FUNCATTR_critical_native) || func.GetAttr(FUNCATTR_fast_native);

  // Get current native method function ptr from reg_jni_func_tab slot
  BaseNode *regReadExpr = builder->CreateExprRegread(PTY_ptr, funcptrPreg);
#ifdef TARGARM32
  BaseNode *checkRegExpr =
      builder->CreateExprCompare(OP_lt, *GlobalTables::GetTypeTable().GetUInt1(),
                                 *GlobalTables::GetTypeTable().GetPtr(),
                                 regReadExpr, builder->CreateIntConst(MByteRef::PositiveOffsetBias, PTY_ptr));
#elif defined(TARGAARCH64)
  // define a temp register for bitwise-and operation
  constexpr int intConstLength = 1;
  BaseNode *andExpr = builder->CreateExprBinary(OP_band, *GlobalTables::GetTypeTable().GetPtr(), regReadExpr,
                                                builder->CreateIntConst(intConstLength, PTY_u32));
  auto funcPtrAndOpPreg = func.GetPregTab()->CreatePreg(PTY_ptr);
  RegassignNode *funcPtrAndOpAssign = builder->CreateStmtRegassign(PTY_ptr, funcPtrAndOpPreg, andExpr);
  auto readFuncPtrAndReg = builder->CreateExprRegread(PTY_ptr, funcPtrAndOpPreg);
  BaseNode *checkRegExpr =
      builder->CreateExprCompare(OP_eq, *GlobalTables::GetTypeTable().GetUInt1(),
                                 *GlobalTables::GetTypeTable().GetPtr(),
                                 readFuncPtrAndReg, builder->CreateIntConst(kInvalidCode, PTY_ptr));
#endif

  auto *ifStmt = static_cast<IfStmtNode*>(builder->CreateStmtIf(checkRegExpr));
  // get find_native_func function
  MIRType *voidPointerType = GlobalTables::GetTypeTable().GetVoidPtr();
  // set parameter of find_native_func
  MapleVector<BaseNode*> dynamicStubOpnds(func.GetCodeMempoolAllocator().Adapter());
  dynamicStubOpnds.push_back(arrayExpr);
  // Use native wrapper if required.
  if (Options::nativeWrapper) {
    // Now native binding have three mode: default mode, dynamic-only mode, static binding list mode
    // default mode, it will generate a weak function, which can link in compile time
    // dynamic only mode, it won't generate any weak function, it can't link in compile time
    // static binding list mode, it will generate a weak function only in list
    if (IsStaticBindingMethod(func.GetName())) {
      // Get current func_ptr (strong/weak symbol address)
      auto *nativeFuncAddr = builder->CreateExprAddroffunc(nativeFunc.GetPuidx());
      funcptrAssign = builder->CreateStmtRegassign(PTY_ptr, funcptrPreg, nativeFuncAddr);
      func.GetBody()->AddStatement(funcptrAssign);
      // Define wrapper function call
      StmtNode *wrapperCall = CreateNativeWrapperCallNode(func, readFuncPtr, args, ret, needIndirectCall);
      func.GetBody()->AddStatement(wrapperCall);
    } else if (!Options::regNativeDynamicOnly) { // Qemu
      func.GetBody()->AddStatement(funcptrAssign);
#ifdef TARGAARCH64
      func.GetBody()->AddStatement(funcPtrAndOpAssign);
#endif
      // Get find_native_func function
      MIRFunction *findNativeFunc = builder->GetOrCreateFunction(NameMangler::kFindNativeFuncNoeh,
                                                                 voidPointerType->GetTypeIndex());
      findNativeFunc->SetAttr(FUNCATTR_nosideeffect);
      // CallAssigned statement for unregistered situation
      CallNode *callGetFindNativeFunc = builder->CreateStmtCallRegassigned(findNativeFunc->GetPuidx(), dynamicStubOpnds,
                                                                           funcptrPreg, OP_callassigned);
      // Check return value of dynamic linking stub
      MIRFunction *dummyNativeFunc = builder->GetOrCreateFunction(NameMangler::kDummyNativeFunc,
                                                                  voidPointerType->GetTypeIndex());
      dummyNativeFunc->SetAttr(FUNCATTR_nosideeffect);
      auto dummyFuncPreg = func.GetPregTab()->CreatePreg(PTY_ptr);
      auto readDummyFuncPtr = builder->CreateExprRegread(PTY_ptr, dummyFuncPreg);
      MapleVector<BaseNode*> dummyFuncOpnds(func.GetCodeMempoolAllocator().Adapter());
      CallNode *callDummyNativeFunc = builder->CreateStmtCallRegassigned(dummyNativeFunc->GetPuidx(), dummyFuncOpnds,
                                                                         dummyFuncPreg, OP_callassigned);
      BaseNode *checkStubReturnExpr =
          builder->CreateExprCompare(OP_eq, *GlobalTables::GetTypeTable().GetUInt1(),
                                     *GlobalTables::GetTypeTable().GetPtr(), readFuncPtr, readDummyFuncPtr);
      auto *subIfStmt = static_cast<IfStmtNode*>(builder->CreateStmtIf(checkStubReturnExpr));
      // Assign with address of strong/weak symbol
      auto *nativeFuncAddr = builder->CreateExprAddroffunc(nativeFunc.GetPuidx());
      funcptrAssign = builder->CreateStmtRegassign(PTY_ptr, funcptrPreg, nativeFuncAddr);
      subIfStmt->GetThenPart()->AddStatement(funcptrAssign);
      // Rewrite reg_jni_func_tab with current funcIdx_ptr(weak/strong symbol address)
      auto nativeMethodPtr = builder->CreateExprRegread(PTY_ptr, funcptrPreg);
      IassignNode *nativeFuncTableEntry = builder->CreateStmtIassign(
          *GlobalTables::GetTypeTable().GetOrCreatePointerType(*elemType), 0, arrayExpr, nativeMethodPtr);
      subIfStmt->GetThenPart()->AddStatement(nativeFuncTableEntry);
      // Add if-statement to function body
      ifStmt->GetThenPart()->AddStatement(callGetFindNativeFunc);
      ifStmt->GetThenPart()->AddStatement(callDummyNativeFunc);
      ifStmt->GetThenPart()->AddStatement(subIfStmt);
      if (needCheckThrowPendingExceptionFunc) {
        func.GetBody()->AddStatement(ifStmt);
        StmtNode *wrapperCall = CreateNativeWrapperCallNode(func, readFuncPtr, args, ret, needIndirectCall);
        func.GetBody()->AddStatement(wrapperCall);
      } else {
        StmtNode *wrapperCall = CreateNativeWrapperCallNode(func, readFuncPtr, args, ret, needIndirectCall);
        ifStmt->GetThenPart()->AddStatement(wrapperCall);
        MapleVector<BaseNode*> opnds(builder->GetCurrentFuncCodeMpAllocator()->Adapter());
        CallNode *callGetExceptFunc = builder->CreateStmtCallAssigned(MRTCheckThrowPendingExceptionFunc->GetPuidx(),
                                                                      opnds, nullptr, OP_callassigned);
        ifStmt->GetThenPart()->AddStatement(callGetExceptFunc);
        auto *elseBlock = func.GetCodeMempool()->New<BlockNode>();
        ifStmt->SetElsePart(elseBlock);
        ifStmt->SetNumOpnds(kOperandNumTernary);
        wrapperCall = CreateNativeWrapperCallNode(func, readFuncPtr, args, ret, needIndirectCall);
        elseBlock->AddStatement(wrapperCall);
        func.GetBody()->AddStatement(ifStmt);
      }
    } else { // EMUI
      func.GetBody()->AddStatement(funcptrAssign);
#ifdef TARGAARCH64
      func.GetBody()->AddStatement(funcPtrAndOpAssign);
#endif
      MIRFunction *findNativeFunc = builder->GetOrCreateFunction(NameMangler::kFindNativeFunc,
                                                                 voidPointerType->GetTypeIndex());
      findNativeFunc->SetAttr(FUNCATTR_nosideeffect);
      // CallAssigned statement for unregistered situation
      CallNode *callGetFindNativeFunc = builder->CreateStmtCallRegassigned(findNativeFunc->GetPuidx(), dynamicStubOpnds,
                                                                           funcptrPreg, OP_callassigned);
      // Add if-statement to function body
      ifStmt->GetThenPart()->AddStatement(callGetFindNativeFunc);
      if (!needCheckThrowPendingExceptionFunc) {
        MapleVector<BaseNode*> opnds(builder->GetCurrentFuncCodeMpAllocator()->Adapter());
        CallNode *callGetExceptFunc = builder->CreateStmtCallAssigned(MRTCheckThrowPendingExceptionFunc->GetPuidx(),
                                                                      opnds, nullptr, OP_callassigned);
        ifStmt->GetThenPart()->AddStatement(callGetExceptFunc);
      }
      func.GetBody()->AddStatement(ifStmt);
      StmtNode *wrapperCall = CreateNativeWrapperCallNode(func, readFuncPtr, args, ret, needIndirectCall);
      func.GetBody()->AddStatement(wrapperCall);
    }
    return;
  }
  // Without native wrapper
  auto *icall = func.GetCodeMempool()->New<IcallNode>(GetMIRModule(), OP_icallassigned);
  icall->SetNumOpnds(args.size() + 1);
  icall->GetNopnd().resize(icall->GetNumOpnds());
  icall->SetReturnVec(nrets);
  for (size_t i = 1; i < icall->GetNopndSize(); ++i) {
    icall->SetNOpndAt(i, args[i - 1]->CloneTree(GetMIRModule().GetCurFuncCodeMPAllocator()));
  }
  icall->SetNOpndAt(0, readFuncPtr);
  icall->SetRetTyIdx(nativeFunc.GetReturnTyIdx());
  // Check if funcptr is Invalid
  MIRFunction *findNativeFunc = builder->GetOrCreateFunction(NameMangler::kFindNativeFunc,
                                                             voidPointerType->GetTypeIndex());
  findNativeFunc->SetAttr(FUNCATTR_nosideeffect);
  // CallAssigned statement for unregistered situation
  CallNode *callGetFindNativeFunc =
      builder->CreateStmtCallRegassigned(findNativeFunc->GetPuidx(), dynamicStubOpnds, funcptrPreg, OP_callassigned);
  ifStmt->GetThenPart()->AddStatement(callGetFindNativeFunc);
  if (!needCheckThrowPendingExceptionFunc) {
    MapleVector<BaseNode*> opnds(builder->GetCurrentFuncCodeMpAllocator()->Adapter());
    CallNode *callGetExceptFunc =
        builder->CreateStmtCallAssigned(MRTCheckThrowPendingExceptionFunc->GetPuidx(), opnds, nullptr, OP_callassigned);
    ifStmt->GetThenPart()->AddStatement(callGetExceptFunc);
  }
  func.GetBody()->AddStatement(ifStmt);
  func.GetBody()->AddStatement(icall);
}

StmtNode *NativeStubFuncGeneration::CreateNativeWrapperCallNode(MIRFunction &func, BaseNode *funcPtr,
                                                                MapleVector<BaseNode*> &args, const MIRSymbol *ret,
                                                                bool needIndirectCall) {
#ifdef USE_ARM32_MACRO
  constexpr size_t numOfArgs = 4;
#else
  constexpr size_t numOfArgs = 8;
#endif
  MIRFunction *wrapperFunc = nullptr;
  MapleVector<BaseNode*> wrapperArgs(func.GetCodeMPAllocator().Adapter());
  // The first arg is the natvie function pointer.
  wrapperArgs.push_back(funcPtr);
  // Push back all original args.
  wrapperArgs.insert(wrapperArgs.end(), args.begin(), args.end());
  // Do not need native wrapper for fast natives or critical natives.
  if (needIndirectCall) {
    if (ret == nullptr) {
      return builder->CreateStmtIcall(wrapperArgs);
    } else {
      return builder->CreateStmtIcallAssigned(wrapperArgs, *ret);
    }
  }
  if (args.size() > numOfArgs) {
    wrapperFunc = MRTCallSlowNativeExtFunc;
  } else {
    wrapperFunc = MRTCallSlowNativeFunc[args.size()];
  }
  if (ret == nullptr) {
    return builder->CreateStmtCall(wrapperFunc->GetPuidx(), wrapperArgs);
  } else {
    return builder->CreateStmtCallAssigned(wrapperFunc->GetPuidx(), wrapperArgs, ret, OP_callassigned);
  }
}

void NativeStubFuncGeneration::GenerateRegTableEntryType() {
  MIRArrayType &arrayType =
      *GlobalTables::GetTypeTable().GetOrCreateArrayType(*GlobalTables::GetTypeTable().GetVoidPtr(), 0);
  regTableConst = GetMIRModule().GetMemPool()->New<MIRAggConst>(GetMIRModule(), arrayType);
}

void NativeStubFuncGeneration::GenerateHelperFuncDecl() {
  MIRType *voidType = GlobalTables::GetTypeTable().GetVoid();
  MIRType *voidPointerType = GlobalTables::GetTypeTable().GetVoidPtr();
  MIRType *refType = GlobalTables::GetTypeTable().GetRef();
  // MRT_PendingException
  MRTCheckThrowPendingExceptionFunc =
      builder->GetOrCreateFunction(NameMangler::kCheckThrowPendingExceptionFunc, voidType->GetTypeIndex());
  CHECK_FATAL(MRTCheckThrowPendingExceptionFunc != nullptr, "MRTCheckThrowPendingExceptionFunc is null.");
  MRTCheckThrowPendingExceptionFunc->SetAttr(FUNCATTR_nosideeffect);
  MRTCheckThrowPendingExceptionFunc->SetBody(nullptr);
  // MRT_PreNativeCall
  ArgVector preArgs(GetMIRModule().GetMPAllocator().Adapter());
  MRTPreNativeFunc = builder->CreateFunction(NameMangler::kPreNativeFunc, *voidPointerType, preArgs);
  CHECK_FATAL(MRTPreNativeFunc != nullptr, "MRTPreNativeFunc is null.");
  MRTPreNativeFunc->SetBody(nullptr);
  // MRT_PostNativeCall
  ArgVector postArgs(GetMIRModule().GetMPAllocator().Adapter());
  postArgs.push_back(ArgPair("env", voidPointerType));
  MRTPostNativeFunc = builder->CreateFunction(NameMangler::kPostNativeFunc, *voidType, postArgs);
  CHECK_FATAL(MRTPostNativeFunc != nullptr, "MRTPostNativeFunc is null.");
  MRTPostNativeFunc->SetBody(nullptr);
  // MRT_DecodeReference
  ArgVector decodeArgs(GetMIRModule().GetMPAllocator().Adapter());
  decodeArgs.push_back(ArgPair("obj", refType));
  MRTDecodeRefFunc = builder->CreateFunction(NameMangler::kDecodeRefFunc, *refType, decodeArgs);
  CHECK_FATAL(MRTDecodeRefFunc != nullptr, "MRTDecodeRefFunc is null.");
  MRTDecodeRefFunc->SetAttr(FUNCATTR_nosideeffect);
  MRTDecodeRefFunc->SetBody(nullptr);
  // MCC_CallSlowNative
  ArgVector callArgs(GetMIRModule().GetMPAllocator().Adapter());
  callArgs.push_back(ArgPair("func", voidPointerType));
  for (int i = 0; i < kSlownativeFuncnum; ++i) {
    MRTCallSlowNativeFunc[i] = builder->CreateFunction(callSlowNativeFuncs[i], *voidPointerType, callArgs);
    CHECK_FATAL(MRTCallSlowNativeFunc[i] != nullptr, "MRTCallSlowNativeFunc is null.");
    MRTCallSlowNativeFunc[i]->SetBody(nullptr);
  }
  // MCC_CallSlowNativeExt
  ArgVector callExtArgs(GetMIRModule().GetMPAllocator().Adapter());
  callExtArgs.push_back(ArgPair("func", voidPointerType));
  MRTCallSlowNativeExtFunc = builder->CreateFunction(NameMangler::kCallSlowNativeExt, *voidPointerType, callExtArgs);
  CHECK_FATAL(MRTCallSlowNativeExtFunc != nullptr, "MRTCallSlowNativeExtFunc is null.");
  MRTCallSlowNativeExtFunc->SetBody(nullptr);
  // MCC_SetReliableUnwindContext
  MCCSetReliableUnwindContextFunc =
      builder->GetOrCreateFunction(NameMangler::kSetReliableUnwindContextFunc, voidType->GetTypeIndex());
  CHECK_FATAL(MCCSetReliableUnwindContextFunc != nullptr, "MCCSetReliableUnwindContextFunc is null");
  MCCSetReliableUnwindContextFunc->SetAttr(FUNCATTR_nosideeffect);
  MCCSetReliableUnwindContextFunc->SetBody(nullptr);
}

void NativeStubFuncGeneration::GenerateRegTable() {
  auto &arrayType = static_cast<MIRArrayType&>(regTableConst->GetType());
  arrayType.SetSizeArrayItem(0, regTableConst->GetConstVec().size());
  std::string regJniTabName = NameMangler::kRegJNITabPrefixStr + GetMIRModule().GetFileNameAsPostfix();
  MIRSymbol *regJNISt = builder->CreateSymbol(regTableConst->GetType().GetTypeIndex(), regJniTabName, kStVar,
                                              kScGlobal, nullptr, kScopeGlobal);
  regJNISt->SetKonst(regTableConst);
}

bool NativeStubFuncGeneration::IsStaticBindingListMode() const {
  return (Options::staticBindingList != "" && Options::staticBindingList.length());
}

void NativeStubFuncGeneration::InitStaticBindingMethodList() {
  if (!IsStaticBindingListMode()) {
    return;
  }
  std::fstream file(Options::staticBindingList);
  std::string content;
  while (std::getline(file, content)) {
    staticBindingMethodsSet.insert(content);
  }
}

bool NativeStubFuncGeneration::IsStaticBindingMethod(const std::string &methodName) const {
  if (!IsStaticBindingListMode()) {
    return false;
  }
  return (staticBindingMethodsSet.find(NameMangler::NativeJavaName(methodName.c_str())) !=
          staticBindingMethodsSet.end());
}


void NativeStubFuncGeneration::Finish() {
  if (!regTableConst->GetConstVec().empty()) {
    GenerateRegTable();
    GenerateRegFuncTab();
  }
  if (!Options::mapleLinker) {
    // If use maplelinker, we postpone this generation to MUIDReplacement
    ReflectionAnalysis::GenStrTab(GetMIRModule());
  }
}

#ifdef USE_ARM32_MACRO
const std::string NativeStubFuncGeneration::callSlowNativeFuncs[kSlownativeFuncnum] = {
    "MCC_CallSlowNative0", "MCC_CallSlowNative1", "MCC_CallSlowNative2", "MCC_CallSlowNative3", "MCC_CallSlowNative4"
};
#else
const std::string NativeStubFuncGeneration::callSlowNativeFuncs[kSlownativeFuncnum] = {
    "MCC_CallSlowNative0", "MCC_CallSlowNative1", "MCC_CallSlowNative2", "MCC_CallSlowNative3", "MCC_CallSlowNative4",
    "MCC_CallSlowNative5", "MCC_CallSlowNative6", "MCC_CallSlowNative7", "MCC_CallSlowNative8"
};
#endif
}  // namespace maple
