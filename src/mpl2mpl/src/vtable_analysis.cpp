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
#include "vtable_analysis.h"
#include "reflection_analysis.h"
#include "itab_util.h"

namespace {
using namespace maple;
// +1 is needed here because our field id starts with 0 pointing to the struct itself
constexpr uint32 kKlassItabFieldID = static_cast<uint32>(ClassProperty::kItab) + 1;
constexpr uint32 kKlassVtabFieldID = static_cast<uint32>(ClassProperty::kVtab) + 1;
} // namespace

// Vtableanalysis
// This phase is mainly to generate the virtual table && iterface table.
// The virtual table just store the virtual method address. And the interface
// table structure is a litter more complicated.we stored the hash number of methodname
// and function address.And we also move the hot function to the front iterface
// table.If the hash number is conflicted,we stored the whole completed methodname at the
// end of interface table.
namespace maple {
VtableAnalysis::VtableAnalysis(MIRModule &mod, KlassHierarchy *kh, bool dump) : FuncOptimizeImpl(mod, kh, dump) {
  voidPtrType = GlobalTables::GetTypeTable().GetVoidPtr();
  // zeroConst and oneConst are shared amony itab entries. It is safe to share them because
  // they are never removed by anybody.
  zeroConst = GlobalTables::GetIntConstTable().GetOrCreateIntConst(0, *voidPtrType);
  oneConst = GlobalTables::GetIntConstTable().GetOrCreateIntConst(1, *voidPtrType);
  for (Klass *klass : klassHierarchy->GetTopoSortedKlasses()) {
    ASSERT(klass != nullptr, "null ptr check!");
    GenVtableList(*klass);
    if (klass->IsClass() && klass->GetMIRStructType()->IsLocal()) {
      // We generate itable/vtable definition in the same place where the class is defined
      GenVtableDefinition(*klass);
      GenItableDefinition(*klass);
    }
    if (trace) {
      DumpVtableList(*klass);
    }
  }
}

bool VtableAnalysis::IsVtableCandidate(const MIRFunction &func) {
  return func.GetAttr(FUNCATTR_virtual) && !func.GetAttr(FUNCATTR_private) && !func.GetAttr(FUNCATTR_static);
}

// Return true if virtual functions can be set override relationship cross package
bool VtableAnalysis::CheckOverrideForCrossPackage(const MIRFunction &baseMethod,
    const MIRFunction &currMethod) const {
  const std::string &baseClassName = baseMethod.GetBaseClassName();
  size_t basePos = baseClassName.rfind(NameMangler::kPackageNameSplitterStr);
  std::string basePackageName = (basePos != std::string::npos) ? baseClassName.substr(0, basePos) : "";
  const std::string &currClassName = currMethod.GetBaseClassName();
  size_t currPos = currClassName.rfind(NameMangler::kPackageNameSplitterStr);
  std::string currPackageName = (currPos != std::string::npos) ? currClassName.substr(0, currPos) : "";
  // For the corss package inheritance, only if the base func is declared
  // as either 'public' or 'protected', we shall set override relationship.
  return (currPackageName == basePackageName) || baseMethod.GetAttr(FUNCATTR_public) ||
          baseMethod.GetAttr(FUNCATTR_protected);
}

// If the method is not in method_table yet, add it in, otherwise update it.
// Note: the method to add should already pass VtableCandidate test
void VtableAnalysis::AddMethodToTable(MethodPtrVector &methodTable, MethodPair &methodPair) {
  MIRFunction *method = builder->GetFunctionFromStidx(methodPair.first);
  ASSERT_NOT_NULL(method);
  GStrIdx strIdx = method->GetBaseFuncNameWithTypeStrIdx();
  for (size_t i = 0; i < methodTable.size(); ++i) {
    MIRFunction *currFunc = builder->GetFunctionFromStidx(methodTable[i]->first);
    ASSERT_NOT_NULL(currFunc);
    GStrIdx currStrIdx = currFunc->GetBaseFuncNameWithTypeStrIdx();
    if (strIdx == currStrIdx) {
      if (CheckOverrideForCrossPackage(*currFunc, *method)) {
        methodTable[i] = &methodPair;
        return;
      }
    }
  }
  methodTable.push_back(&methodPair);
}

void VtableAnalysis::GenVtableList(const Klass &klass) {
  if (klass.IsInterface()) {
    MIRInterfaceType *iType = klass.GetMIRInterfaceType();
    // add in methods from parent interfaces, note interfaces can declare/define same methods
    for (const Klass *parentKlass : klass.GetSuperKlasses()) {
      MIRInterfaceType *parentInterfaceType = parentKlass->GetMIRInterfaceType();
      for (MethodPair *methodPair : parentInterfaceType->GetVTableMethods()) {
        ASSERT_NOT_NULL(methodPair);
        AddMethodToTable(iType->GetVTableMethods(), *methodPair);
      }
    }
    // add in methods from this interface, note it can override methods of parents
    for (MethodPair &methodPair : iType->GetMethods()) {
      AddMethodToTable(iType->GetVTableMethods(), methodPair);
    }
  } else {  // it's a class
    MIRClassType *curType = klass.GetMIRClassType();
    // prepare vtable_methods
    // first is vtable from parents. since it's single inheritance, we copy it directly
    if (klass.HasSuperKlass()) {
      Klass *superKlass = klass.GetSuperKlass();
      MIRStructType *partenType = superKlass->GetMIRStructType();
      curType->GetVTableMethods() = partenType->GetVTableMethods();
    }
    // vtable from implemented interfaces, need to merge in. both default or none-default
    // Note, all interface methods are also virtual methods, need to be in vtable too.
    for (TyIdx tyIdx : curType->GetInterfaceImplemented()) {
      auto *iType = static_cast<MIRInterfaceType*>(GlobalTables::GetTypeTable().GetTypeFromTyIdx(tyIdx));
      for (MethodPair *methodPair : iType->GetVTableMethods()) {
        MIRFunction *method = builder->GetFunctionFromStidx(methodPair->first);
        GStrIdx strIdx = method->GetBaseFuncNameWithTypeStrIdx();
        Klass *iklass = klassHierarchy->GetKlassFromFunc(method);
        bool found = false;
        for (size_t i = 0; i < curType->GetVTableMethods().size(); ++i) {
          MIRFunction *curMethod = builder->GetFunctionFromStidx(curType->GetVTableMethods()[i]->first);
          GStrIdx currStrIdx = curMethod->GetBaseFuncNameWithTypeStrIdx();
          Klass *currKlass = klassHierarchy->GetKlassFromFunc(curMethod);
          // Interfaces implemented methods can't override methods from parent,
          // except the methods comes from another interface which is a parent of current interface
          if (strIdx == currStrIdx) {
            if (klassHierarchy->IsSuperKlassForInterface(currKlass, iklass)) {
              curType->GetVTableMethods()[i] = methodPair;
            }
            found = true;
            break;
          }
        }
        if (!found) {
          curType->GetVTableMethods().push_back(methodPair);
        }
      }
    }
    // then methods defined in this class
    for (MethodPair &methodPair : curType->GetMethods()) {
      MIRFunction *curMethod = builder->GetFunctionFromStidx(methodPair.first);
      ASSERT_NOT_NULL(curMethod);
      if (IsVtableCandidate(*curMethod)) {
        AddMethodToTable(curType->GetVTableMethods(), methodPair);
      }
      // Optimization: mark private methods as local
      if (curType->IsLocal() && curMethod->IsPrivate() && !curMethod->IsConstructor()) {
        curMethod->SetAttr(FUNCATTR_local);
      }
    }
    // Create initial cached vtable mapping
    for (size_t i = 0; i < curType->GetVTableMethods().size(); ++i) {
      MIRFunction *curMethod = builder->GetFunctionFromStidx(curType->GetVTableMethods()[i]->first);
      ASSERT_NOT_NULL(curMethod);
      puidxToVtabIndex[curMethod->GetPuidx()] = i;
    }
  }
}

void VtableAnalysis::GenVtableDefinition(const Klass &klass) {
  MIRStructType *curType = klass.GetMIRStructType();
  auto *newConst = GetMIRModule().GetMemPool()->New<MIRAggConst>(GetMIRModule(), *voidPtrType);
  ASSERT_NOT_NULL(newConst);
  for (MethodPair *methodPair : curType->GetVTableMethods()) {
    MIRFunction *vtabMethod = builder->GetFunctionFromStidx(methodPair->first);
    ASSERT_NOT_NULL(vtabMethod);
    AddroffuncNode *addrofFuncNode = builder->CreateExprAddroffunc(vtabMethod->GetPuidx(),
                                                                   GetMIRModule().GetMemPool());
    auto *constNode = GetMIRModule().GetMemPool()->New<MIRAddroffuncConst>(addrofFuncNode->GetPUIdx(),
                                                                           *voidPtrType);
    newConst->PushBack(constNode);
  }
  // We also need to generate vtable and itable even if the class does not
  // have any virtual method, or does not implement any interface.  Such a
  // class needs a TRIVIAL vtable, or a TRIVIAL itable, because we need to
  // inform the OBJECT ALLOCATOR that such a class does not need any
  // (non-trivial) vtable or itable.  One way (and the most uniform way) to
  // convey this information is having a trivial table.  Alternatively, we can
  // postpone this step to mplcg, where mplcg discovers all classes that does
  // not have any vtable or itable.
  if (newConst->GetConstVec().empty()) {
    newConst->PushBack(zeroConst);
  }
  GenTableSymbol(VTAB_PREFIX_STR, klass.GetKlassName(), *newConst);
}

std::string VtableAnalysis::DecodeBaseNameWithType(const MIRFunction &func) {
  const std::string &baseName = NameMangler::DecodeName(func.GetBaseFuncName());
  std::string signatureName = NameMangler::DecodeName(func.GetSignature());
  ReflectionAnalysis::ConvertMethodSig(signatureName);
  std::string baseNameWithType = baseName + "|" + signatureName;
  return baseNameWithType;
}

void VtableAnalysis::GenItableDefinition(const Klass &klass) {
  MIRStructType *curType = klass.GetMIRStructType();
  std::set<GStrIdx> signatureVisited;
  std::vector<MIRFunction*> firstItabVec(kItabFirstHashSize, nullptr);
  std::vector<bool> firstConflictFlag(kItabFirstHashSize, false);
  std::vector<MIRFunction*> firstConflictList;
  bool itabContainsMethod = false;
  for (Klass *implInterface : klass.GetImplInterfaces()) {
    CHECK_FATAL(implInterface->IsInterface(), "implInterface must be interface");
    MIRInterfaceType *interfaceType = implInterface->GetMIRInterfaceType();
    for (MethodPair &methodPair : interfaceType->GetMethods()) {
      MIRFunction *interfaceMethod = builder->GetFunctionFromStidx(methodPair.first);
      ASSERT_NOT_NULL(interfaceMethod);
      int64 hashCode = GetHashIndex(DecodeBaseNameWithType(*interfaceMethod).c_str());
      GStrIdx interfaceMethodStridx = interfaceMethod->GetBaseFuncNameWithTypeStrIdx();
      if (signatureVisited.find(interfaceMethodStridx) == signatureVisited.end()) {
        signatureVisited.insert(interfaceMethodStridx);
      } else {
        // some interfaces own methods with same signature, cache them in order to
        // prevent processing these methods multiple times
        continue;
      }
      // Search in vtable
      MIRFunction *vtabMethod = nullptr;
      for (MethodPair *curMethodPair : curType->GetVTableMethods()) {
        MIRFunction *method = builder->GetFunctionFromStidx(curMethodPair->first);
        if (interfaceMethodStridx == method->GetBaseFuncNameWithTypeStrIdx()) {
          vtabMethod = method;
          break;
        }
      }
      CHECK_FATAL(vtabMethod != nullptr, "Interface method %s is not implemented in class %s",
                  interfaceMethod->GetName().c_str(), klass.GetKlassName().c_str());
      if (!vtabMethod->IsAbstract()) {
        itabContainsMethod = true;
        if (!firstItabVec[hashCode] && !firstConflictFlag[hashCode]) {
          firstItabVec[hashCode] = vtabMethod;
        } else {  // a conflict found
          if (!firstConflictFlag[hashCode]) {
            // move itab element to conflict table when first conflict occurs
            firstConflictList.push_back(firstItabVec[hashCode]);
            firstItabVec[hashCode] = nullptr;
            firstConflictFlag[hashCode] = true;
          }
          firstConflictList.push_back(vtabMethod);
        }
      }
    }
  }
  if (!itabContainsMethod) {
    // No need to generate itable in this case
    return;
  }
  std::vector<MIRFunction*> secondItab(kItabSecondHashSize, nullptr);
  std::vector<bool> secondConflictFlag(kItabSecondHashSize, false);
  std::vector<MIRFunction*> secondConflictList;
  uint64 count = 0;
  for (MIRFunction *func : firstConflictList) {
    ASSERT(func != nullptr, "null ptr check!");
    uint32 secondHashCode = GetSecondHashIndex(DecodeBaseNameWithType(*func).c_str());
    if (!secondItab[secondHashCode] && !secondConflictFlag[secondHashCode]) {
      secondItab[secondHashCode] = func;
      count++;
    } else {
      if (secondItab[secondHashCode]) {
        secondConflictList.push_back(secondItab[secondHashCode]);
        secondItab[secondHashCode] = nullptr;
        secondConflictFlag[secondHashCode] = true;
      }
      secondConflictList.push_back(func);
    }
  }
  if (count == 0) {
    // If no conflict exists, reduce the unnecessary zero element at the end
    for (int i = kItabFirstHashSize - 1; i >= 0; --i) {
      if (!firstItabVec[i] && !firstConflictFlag[i]) {
        firstItabVec.pop_back();
      } else {
        break;
      }
    }
  }
  // Create the first-level itable, which is directly accessed as an array
  auto *firstItabEmitArray = GetMIRModule().GetMemPool()->New<MIRAggConst>(GetMIRModule(), *voidPtrType);
  ASSERT_NOT_NULL(firstItabEmitArray);
  for (MIRFunction *func : firstItabVec) {
    if (func != nullptr) {
      firstItabEmitArray->PushBack(
        GetMIRModule().GetMemPool()->New<MIRAddroffuncConst>(func->GetPuidx(), *voidPtrType));
    } else {
      firstItabEmitArray->PushBack(zeroConst);
    }
  }
  // initialize conflict solution array
  if (count != 0) {
    auto *secondItabEmitArray = GetMIRModule().GetMemPool()->New<MIRAggConst>(GetMIRModule(), *voidPtrType);
    // remember count in secondItabVec
    count = ((secondConflictList.size() | (1UL << (kShiftCountBit - 1))) << kShiftCountBit) + count;
    secondItabEmitArray->PushBack(GlobalTables::GetIntConstTable().GetOrCreateIntConst(count, *voidPtrType));
    secondItabEmitArray->PushBack(oneConst);  // padding
    for (uint32 i = 0; i < kItabSecondHashSize; ++i) {
      if (!secondItab[i] && !secondConflictFlag[i]) {
        continue;
      } else {
        secondItabEmitArray->PushBack(GlobalTables::GetIntConstTable().GetOrCreateIntConst(i, *voidPtrType));
        if (secondItab[i]) {
          secondItabEmitArray->PushBack(
            GetMIRModule().GetMemPool()->New<MIRAddroffuncConst>(secondItab[i]->GetPuidx(), *voidPtrType));
        } else {
          // it measn it was conflict again in the second hash
          secondItabEmitArray->PushBack(oneConst);
        }
      }
    }
    for (MIRFunction *func : secondConflictList) {
      ASSERT_NOT_NULL(func);
      const std::string &signatureName = DecodeBaseNameWithType(*func);
      uint32 nameIdx = ReflectionAnalysis::FindOrInsertRepeatString(signatureName);
      secondItabEmitArray->PushBack(GlobalTables::GetIntConstTable().GetOrCreateIntConst(nameIdx, *voidPtrType));
      secondItabEmitArray->PushBack(
        GetMIRModule().GetMemPool()->New<MIRAddroffuncConst>(func->GetPuidx(), *voidPtrType));
    }
    // Create the second-level itable, in which hashcode is looked up by binary searching
    GenTableSymbol(ITAB_CONFLICT_PREFIX_STR, klass.GetKlassName(), *secondItabEmitArray);
    // push the conflict symbol to the first-level itable
    StIdx symIdx = GlobalTables::GetGsymTable().GetStIdxFromStrIdx(
        GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(ITAB_CONFLICT_PREFIX_STR + klass.GetKlassName()));
    firstItabEmitArray->PushBack(GetMIRModule().GetMemPool()->New<MIRAddrofConst>(symIdx, 0, *voidPtrType));
  }
  GenTableSymbol(ITAB_PREFIX_STR, klass.GetKlassName(), *firstItabEmitArray);
}

void VtableAnalysis::GenTableSymbol(const std::string &prefix, const std::string klassName,
                                    MIRAggConst &newConst) const {
  size_t arraySize = newConst.GetConstVec().size();
  MIRArrayType *arrayType = GlobalTables::GetTypeTable().GetOrCreateArrayType(*voidPtrType, arraySize);
  MIRSymbol *vtabSt = builder->CreateGlobalDecl(prefix + klassName, *arrayType);
  if (klassName == NameMangler::GetInternalNameLiteral(NameMangler::kJavaLangObjectStr)) {
    vtabSt->SetStorageClass(kScGlobal);
  } else {
    vtabSt->SetStorageClass(kScFstatic);
  }
  vtabSt->SetKonst(&newConst);
}

void VtableAnalysis ::DumpVtableList(const Klass &klass) const {
  LogInfo::MapleLogger() << "=========" << klass.GetKlassName() << "========\n";
  for (MethodPair *vtableMethod : klass.GetMIRStructType()->GetVTableMethods()) {
    MIRFunction *method = builder->GetFunctionFromStidx(vtableMethod->first);
    ASSERT_NOT_NULL(method);
    LogInfo::MapleLogger() << method->GetName() << "\n";
  }
}

// add Null-Point-Exception check
void VtableAnalysis::AddNullPointExceptionCheck(MIRFunction &func, StmtNode &stmt) const {
  CallNode *cNode = static_cast<CallNode*>(&stmt);
  MIRFunction *callee = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(cNode->GetPUIdx());
  if (!callee->IsStatic() && !callee->GetBaseClassName().empty()) {
    BaseNode *inputOpnd = cNode->Opnd(0);
    UnaryStmtNode *nullCheck = builder->CreateStmtUnary(OP_assertnonnull, inputOpnd);
    func.GetBody()->InsertBefore(&stmt, nullCheck);
  }
}

void VtableAnalysis::ProcessFunc(MIRFunction *func) {
  if (func->IsEmpty()) {
    return;
  }
  SetCurrentFunction(*func);
  StmtNode *stmt = func->GetBody()->GetFirst();
  StmtNode *next = nullptr;
  while (stmt != nullptr) {
    next = stmt->GetNext();
    switch (stmt->GetOpCode()) {
      case OP_virtualcallassigned: {
        ReplaceVirtualInvoke(*(static_cast<CallNode*>(stmt)));
        break;
      }
      case OP_interfacecallassigned: {
        ReplaceInterfaceInvoke(*(static_cast<CallNode*>(stmt)));
        break;
      }
      case OP_superclasscallassigned: {
        ReplaceSuperclassInvoke(*(static_cast<CallNode*>(stmt)));
        break;
      }
      case OP_polymorphiccallassigned: {
        ReplacePolymorphicInvoke(*(static_cast<CallNode*>(stmt)));
        break;
      }
      case OP_call:
      case OP_callassigned: {
        AddNullPointExceptionCheck(*func, *stmt); // npe check
        break;
      }
      default:
        break;
    }
    stmt = next;
  }
}

void VtableAnalysis::ReplaceSuperclassInvoke(CallNode &stmt) {
  // Because the virtual method may be inherited from its parent, we need to find
  // the actual method target.
  MIRFunction *callee = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(stmt.GetPUIdx());
  Klass *klass = klassHierarchy->GetKlassFromFunc(callee);

  CHECK_FATAL(klass != nullptr, "Klass not found");
  MapleVector<MIRFunction*> *cands = klass->GetCandidates(callee->GetBaseFuncNameWithTypeStrIdx());
  // continue to search its implinterfaces
  if (cands == nullptr) {
    for (Klass *implInterface : klass->GetImplInterfaces()) {
      cands = implInterface->GetCandidates(callee->GetBaseFuncNameWithTypeStrIdx());
      if (cands != nullptr && !cands->empty()) {
        break;
      }
    }
  }
  if (cands == nullptr || cands->size() == 0) {
    if (klass->IsClass() || klass->IsInterface()) {
      LogInfo::MapleLogger() << "warning: func " << callee->GetName() << " is not found in SuperInvoke!\n";
      stmt.SetOpCode(OP_callassigned);
      return;
    }
  }
  CHECK_FATAL(cands != nullptr && !cands->empty(),
              "Dependency Error: function %s cannot be found in %s or any of its superclasses/interfaces",
              callee->GetBaseFuncNameWithType().c_str(), klass->GetKlassName().c_str());
  MIRFunction *actualMIRFunc = cands->at(0);
  CHECK_FATAL(actualMIRFunc != nullptr, "Can not find the implementation of %s", callee->GetName().c_str());
  stmt.SetOpCode(OP_callassigned);
  stmt.SetPUIdx(actualMIRFunc->GetPuidx());
  GetMIRModule().addSuperCall(actualMIRFunc->GetName());
}

void VtableAnalysis::ReplacePolymorphicInvoke(CallNode &stmt) {
  auto *intrinCall =
      GetMIRModule().CurFuncCodeMemPool()->New<IntrinsiccallNode>(GetMIRModule(), OP_xintrinsiccallassigned);
  intrinCall->SetIntrinsic(INTRN_JAVA_POLYMORPHIC_CALL);
  intrinCall->SetNumOpnds(stmt.GetNumOpnds());
  intrinCall->SetReturnVec(stmt.GetReturnVec());
  intrinCall->SetNOpnd(stmt.GetNopnd());
  currFunc->GetBody()->ReplaceStmt1WithStmt2(&stmt, intrinCall);
}

BaseNode *VtableAnalysis::GenVtabItabBaseAddr(BaseNode &obj, bool isVirtual) {
  ASSERT_NOT_NULL(builder);
  BaseNode *classInfoAddress = ReflectionAnalysis::GenClassInfoAddr(&obj, *builder);
  auto *classMetadataType = static_cast<MIRStructType*>(
      GlobalTables::GetTypeTable().GetTypeFromTyIdx(ReflectionAnalysis::GetClassMetaDataTyIdx()));
  return builder->CreateExprIread(*voidPtrType,
                                  *GlobalTables::GetTypeTable().GetOrCreatePointerType(*classMetadataType),
                                  (isVirtual ? kKlassVtabFieldID : kKlassItabFieldID), classInfoAddress);
}


void VtableAnalysis::ReplaceVirtualInvoke(CallNode &stmt) {
  MIRFunction *callee = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(stmt.GetPUIdx());
  CHECK_FATAL(callee->GetParamSize() != 0, "container check");
  auto *firstFormalArgType = static_cast<MIRPtrType*>(callee->GetNthParamType(0));
  MIRType *pointedType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(firstFormalArgType->GetPointedTyIdx());
  MIRStructType *structType = nullptr;
  if (pointedType->GetKind() == kTypeJArray) {
    structType = static_cast<MIRJarrayType*>(pointedType)->GetParentType();
  } else {
    Klass *klassFromFuncDependCallee = klassHierarchy->GetKlassFromFunc(callee);
    std::string missingClassName = GlobalTables::GetStrTable().GetStringFromStrIdx(pointedType->GetNameStrIdx());
    CHECK_FATAL(klassFromFuncDependCallee != nullptr, "Error: missing class %s", missingClassName.c_str());
    structType = klassFromFuncDependCallee->GetMIRStructType();
  }
  size_t entryOffset = SIZE_MAX;
  if (puidxToVtabIndex.find(stmt.GetPUIdx()) != puidxToVtabIndex.end() && puidxToVtabIndex[stmt.GetPUIdx()] >= 0) {
    entryOffset = puidxToVtabIndex[stmt.GetPUIdx()];
  } else {
    GStrIdx calleeStridx = callee->GetBaseFuncNameWithTypeStrIdx();
    ASSERT(structType != nullptr, "null ptr check!");
    for (size_t id = 0; id < structType->GetVTableMethods().size(); ++id) {
      MIRFunction *vtableMethod = builder->GetFunctionFromStidx(structType->GetVTableMethods()[id]->first);
      ASSERT(vtableMethod != nullptr, "null ptr check!");
      if (calleeStridx == vtableMethod->GetBaseFuncNameWithTypeStrIdx()) {
        entryOffset = id;
        puidxToVtabIndex[callee->GetPuidx()] = id;
        break;
      }
    }
    CHECK_FATAL(entryOffset != SIZE_MAX,
                "Error: method for virtual call cannot be found in all included mplt files. Call to %s in %s",
                callee->GetName().c_str(), currFunc->GetName().c_str());
  }
  BaseNode *offsetNode = builder->CreateIntConst(entryOffset * kTabEntrySize, PTY_u32);
  CHECK_FATAL(!stmt.GetNopnd().empty(), "container check");
  BaseNode *tabBaseAddress = GenVtabItabBaseAddr(*stmt.GetNopndAt(0), true);
  BaseNode *addrNode =
      builder->CreateExprBinary(OP_add, *GlobalTables::GetTypeTable().GetPtr(), tabBaseAddress, offsetNode);
  BaseNode *readFuncPtr = builder->CreateExprIread(
      *GlobalTables::GetTypeTable().GetCompactPtr(),
      *GlobalTables::GetTypeTable().GetOrCreatePointerType(*GlobalTables::GetTypeTable().GetCompactPtr()), 0, addrNode);
  stmt.SetOpCode(OP_virtualicallassigned);
  stmt.GetNopnd().insert(stmt.GetNopnd().begin(), readFuncPtr);
  stmt.SetNumOpnds(stmt.GetNumOpnds() + 1);
}


void VtableAnalysis::ReplaceInterfaceInvoke(CallNode &stmt) {
  CHECK_FATAL(!stmt.GetNopnd().empty(), "container check");
  BaseNode *tabBaseAddress = GenVtabItabBaseAddr(*stmt.GetNopndAt(0), false);
  MemPool *currentFuncMp = builder->GetCurrentFuncCodeMp();
  ASSERT_NOT_NULL(currentFuncMp);
  auto *resolveNode = currentFuncMp->New<ResolveFuncNode>(
      OP_resolveinterfacefunc, GlobalTables::GetTypeTable().GetCompactPtr()->GetPrimType(), stmt.GetPUIdx(),
      tabBaseAddress, builder->GetConstUInt32(0));
  stmt.SetOpCode(OP_interfaceicallassigned);
  stmt.GetNopnd().insert(stmt.GetNopnd().begin(), resolveNode);
  stmt.SetNumOpnds(stmt.GetNumOpnds() + 1);
}
}  // namespace maple
