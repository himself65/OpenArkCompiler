/*
 * Copyright (c) [2019] Huawei Technologies Co.,Ltd.All rights reserved.
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
#include "muid_replacement.h"
#include <fstream>
#include "vtable_analysis.h"
#include "reflection_analysis.h"

// MUIDReplacement
// This phase is mainly to enable the maple linker about the text and data structure.
// It will do the following things:
// A) It collect the methods, classinfo, vtable, itable , and etc.And then it will generate the
// basic data structures like func_def, func_undef, data_def, data_undef using these symbols.
//
// B) It will replace the relevant reference about the methods and static variable with def or undef
// table.And then we can close these symbols to reduce the code size.

namespace maple {
MUID MUIDReplacement::mplMuid;

MUIDReplacement::MUIDReplacement(MIRModule *mod, KlassHierarchy *kh, bool dump)
    : FuncOptimizeImpl(mod, kh, dump),
      funcDefMap(std::less<MUID>()),
      dataDefMap(std::less<MUID>()),
      funcUndefMap(std::less<MUID>()),
      dataUndefMap(std::less<MUID>()),
      defMuidIdxMap(std::less<MUID>()) {
  isLibcore = (GetSymbolFromName(NameMangler::GetInternalNameLiteral(NameMangler::kJavaLangObjectStr)) != nullptr);
  GenericTables();
}

MIRSymbol *MUIDReplacement::GetSymbolFromName(const std::string &name) {
  GStrIdx gStrIdx = GlobalTables::GetStrTable().GetStrIdxFromName(name);
  return GlobalTables::GetGsymTable().GetSymbolFromStrIdx(gStrIdx);
}

void MUIDReplacement::DumpMUIDFile(bool isFunc) {
  std::ofstream outFile;
  const std::string &mplName = GetMIRModule().GetFileName();
  CHECK_FATAL(mplName.rfind(".mpl") != std::string::npos, "can not find .mpl");
  std::string prefix = mplName.substr(0, mplName.rfind(".mpl"));
  std::string outFileName;
  if (isFunc) {
    outFileName = prefix + ".func.muid";
  } else {
    outFileName = prefix + ".data.muid";
  }
  outFile.open(outFileName);
  if (outFile.fail()) {
    return;
  }
  size_t begin = mplName.find("libmaple");
  size_t end = mplName.find("_", begin);
  std::string outName;
  if (begin != std::string::npos && end != std::string::npos && end > begin) {
    outName = mplName.substr(begin, end - begin);
  } else {
    outName = mplName;
  }
  if (isFunc) {
    for (auto const &keyVal : funcDefMap) {
      outFile << outName << " ";
      MIRSymbol *mirFunc = keyVal.second.first;
      outFile << mirFunc->GetName() << " ";
      outFile << keyVal.first.ToStr() << "\n";
    }
  } else {
    for (auto const &keyVal : dataDefMap) {
      outFile << outName << " ";
      MIRSymbol *mirSymbol = keyVal.second.first;
      outFile << mirSymbol->GetName() << " ";
      outFile << keyVal.first.ToStr() << "\n";
    }
  }
}

void MUIDReplacement::CollectFuncAndDataFromKlasses() {
  // Iterate klasses
  for (Klass *klass : klassHierarchy->GetTopoSortedKlasses()) {
    MIRStructType *sType = klass->GetMIRStructType();
    // DefTable and UndefTable are placed where a class is defined
    if (sType == nullptr || !sType->IsLocal()) {
      continue;
    }
    // Collect FuncDefSet
    for (MethodPair &methodPair : sType->GetMethods()) {
      MIRSymbol *funcSymbol = GlobalTables::GetGsymTable().GetSymbolFromStidx(methodPair.first.Idx());
      MIRFunction *mirFunc = funcSymbol->GetFunction();
      if (mirFunc != nullptr && mirFunc->GetBody()) {
        AddDefFunc(mirFunc);
      }
    }
    // Cases where an external method can be referred:
    // 1. vtable entry (what we are dealing with here)
    // 2. direct call (collected later when iterating function bodies)
    if (!klass->IsInterface()) {
      for (MethodPair *vMethodPair : sType->GetVTableMethods()) {
        if (vMethodPair != nullptr) {
          MIRSymbol *funcSymbol = GlobalTables::GetGsymTable().GetSymbolFromStidx(vMethodPair->first.Idx());
          MIRFunction *mirFunc = funcSymbol->GetFunction();
          if (mirFunc != nullptr && mirFunc->GetBody() == nullptr && !mirFunc->IsAbstract()) {
            AddUndefFunc(mirFunc);
          }
        }
      }
    }
  }
}

void MUIDReplacement::CollectFuncAndDataFromGlobalTab() {
  // Iterate global symbols
  for (size_t i = 1; i < GlobalTables::GetGsymTable().GetSymbolTableSize(); i++) {
    // entry 0 is reserved as nullptr
    MIRSymbol *mirSymbol = GlobalTables::GetGsymTable().GetSymbolFromStidx(i);
    CHECK_FATAL(mirSymbol != nullptr, "Invalid global data symbol at index %u", i);
    if (mirSymbol->GetStorageClass() == kScGlobal) {
      if (mirSymbol->IsReflectionClassInfo()) {
        if (mirSymbol->GetKonst() != nullptr) {
          // Use this to exclude forward-declared classinfo symbol
          AddDefData(mirSymbol);
        }
      } else if (mirSymbol->IsStatic()) {
        AddDefData(mirSymbol);
      }
    } else if (mirSymbol->GetStorageClass() == kScExtern &&
               (mirSymbol->IsReflectionClassInfo() || mirSymbol->IsStatic())) {
      AddUndefData(mirSymbol);
    }
  }
}

void MUIDReplacement::CollectFuncAndDataFromFuncList() {
  // Iterate function bodies
  for (MIRFunction *mirFunc : GetMIRModule().GetFunctionList()) {
    if (mirFunc->GetBody() == nullptr) {
      continue;
    }
    StmtNode *stmt = mirFunc->GetBody()->GetFirst();
    while (stmt != nullptr) {
      PUIdx puidx = 0;
      switch (stmt->GetOpCode()) {
        case OP_call:
        case OP_callassigned: {
          puidx = static_cast<CallNode*>(stmt)->GetPUIdx();
          break;
        }
        case OP_dassign: {
          // epre in ME may have splitted a direct call into addroffunc and an indirect call
          auto *rhs = static_cast<DassignNode*>(stmt)->GetRHS();
          if (rhs != nullptr && rhs->GetOpCode() == OP_addroffunc) {
            puidx = static_cast<AddroffuncNode*>(rhs)->GetPUIdx();
          }
          break;
        }
        case OP_regassign: {
          auto *rhs = static_cast<RegassignNode*>(stmt)->Opnd();
          if (rhs != nullptr && rhs->GetOpCode() == OP_addroffunc) {
            puidx = static_cast<AddroffuncNode*>(rhs)->GetPUIdx();
          }
          break;
        }
        default:
          break;
      }
      if (puidx != 0) {
        MIRFunction *undefMIRFunc = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(puidx);
        ASSERT(undefMIRFunc != nullptr, "Invalid MIRFunction");
        if (undefMIRFunc->GetBody() == nullptr &&
            (undefMIRFunc->IsJava() || !undefMIRFunc->GetBaseClassName().empty())) {
          AddUndefFunc(undefMIRFunc);
        }
      }
      // Some stmt requires classinfo but is lowered in CG. Handle them here.
      CollectImplicitUndefClassInfo(*stmt);
      stmt = stmt->GetNext();
    }
  }
}

void MUIDReplacement::CollectImplicitUndefClassInfo(StmtNode &stmt) {
  BaseNode *rhs = nullptr;
  std::vector<MIRStructType*> classTyVec;
  if (stmt.GetOpCode() == OP_dassign) {
    DassignNode *dnode = static_cast<DassignNode*>(&stmt);
    rhs = dnode->GetRHS();
  } else if (stmt.GetOpCode() == OP_regassign) {
    RegassignNode *rnode = static_cast<RegassignNode*>(&stmt);
    rhs = rnode->Opnd();
  } else if (stmt.GetOpCode() == OP_catch) {
    CatchNode *jnode = static_cast<CatchNode*>(&stmt);
    for (TyIdx typeIdx : jnode->GetExceptionTyIdxVec()) {
      MIRPtrType *pointerType = static_cast<MIRPtrType*>(GlobalTables::GetTypeTable().GetTypeFromTyIdx(typeIdx));
      MIRType *type = pointerType->GetPointedType();
      if (type != nullptr) {
        if (type->GetKind() == kTypeClass || type->GetKind() == kTypeInterface) {
          classTyVec.push_back(static_cast<MIRStructType*>(type));
        } else if (type == GlobalTables::GetTypeTable().GetVoid()) {
          Klass *objectKlass = klassHierarchy->GetKlassFromLiteral(NameMangler::kJavaLangObjectStr);
          if (objectKlass != nullptr) {
            classTyVec.push_back(objectKlass->GetMIRStructType());
          }
        }
      }
    }
  }
  if (rhs != nullptr && rhs->GetOpCode() == OP_gcmalloc) {
    // GCMalloc may require more classinfo than what we have seen so far
    GCMallocNode *gcMalloc = static_cast<GCMallocNode*>(rhs);
    classTyVec.push_back(
        static_cast<MIRStructType*>(GlobalTables::GetTypeTable().GetTypeFromTyIdx(gcMalloc->GetTyIdx())));
  } else if (rhs != nullptr && rhs->GetOpCode() == OP_intrinsicopwithtype) {
    IntrinsicopNode *intrinNode = static_cast<IntrinsicopNode*>(rhs);
    if (intrinNode->GetIntrinsic() == INTRN_JAVA_CONST_CLASS || intrinNode->GetIntrinsic() == INTRN_JAVA_INSTANCE_OF) {
      MIRPtrType *pointerType =
          static_cast<MIRPtrType*>(GlobalTables::GetTypeTable().GetTypeFromTyIdx(intrinNode->GetTyIdx()));
      MIRType *type = pointerType->GetPointedType();
      if (type != nullptr && (type->GetKind() == kTypeClass || type->GetKind() == kTypeInterface)) {
        classTyVec.push_back(static_cast<MIRStructType*>(type));
      }
    }
  }
  for (MIRStructType *classType : classTyVec) {
    if (classType == nullptr) {
      continue;
    }
    std::string classInfoName = CLASSINFO_PREFIX_STR + classType->GetName();
    MIRSymbol *classSym = GetSymbolFromName(classInfoName);
    if (classSym == nullptr) {
      classSym = builder->CreateGlobalDecl(classInfoName.c_str(), *GlobalTables::GetTypeTable().GetPtr());
      classSym->SetStorageClass(kScExtern);
      AddUndefData(classSym);
    }
  }
}


void MUIDReplacement::GenericFuncDefTable() {
  // Use funcDefMap to make sure funcDefTab is sorted by an increasing order of MUID
  for (MIRFunction *mirFunc : funcDefSet) {
    MUID muid = GetMUID(mirFunc->GetName());
    CHECK_FATAL(funcDefMap.find(muid) == funcDefMap.end(), "MUID has been used before, possible collision");
    // Use 0 as the index for now. It will be back-filled once we have the whole map.
    funcDefMap[muid] = SymIdxPair(mirFunc->GetFuncSymbol(), 0);
  }
  uint32 idx = 0;
  size_t arraySize = funcDefMap.size();
  MIRArrayType &muidIdxArrayType =
      *GlobalTables::GetTypeTable().GetOrCreateArrayType(*GlobalTables::GetTypeTable().GetUInt32(), arraySize);
  MIRAggConst *muidIdxTabConst = GetMIRModule().GetMemPool()->New<MIRAggConst>(GetMIRModule(), muidIdxArrayType);
  for (auto &keyVal : funcDefMap) {
    // Fill in the real index
    keyVal.second.second = idx++;
    // Use the muid index for now. It will be back-filled once we have the whole vector.
    MIRIntConst *indexConst =
      GetMIRModule().GetMemPool()->New<MIRIntConst>(keyVal.second.second, *GlobalTables::GetTypeTable().GetUInt32());
    muidIdxTabConst->GetConstVec().push_back(indexConst);
  }
  FieldVector parentFields;
  FieldVector fields;
  GlobalTables::GetTypeTable().PushIntoFieldVector(fields, "funcUnifiedAddr",
                                                   *GlobalTables::GetTypeTable().GetCompactPtr());
  auto *funcDefTabEntryType = static_cast<MIRStructType*>(
      GlobalTables::GetTypeTable().GetOrCreateStructType("MUIDFuncDefTabEntry", fields, parentFields, GetMIRModule()));
  FieldVector funcinffields;
  GlobalTables::GetTypeTable().PushIntoFieldVector(funcinffields, "funcSize",
                                                   *GlobalTables::GetTypeTable().GetUInt32());
  GlobalTables::GetTypeTable().PushIntoFieldVector(funcinffields, "funcName",
                                                   *GlobalTables::GetTypeTable().GetUInt32());
  auto *funcInfTabEntryType = static_cast<MIRStructType*>(GlobalTables::GetTypeTable().GetOrCreateStructType(
      "MUIDFuncInfTabEntry", funcinffields, parentFields, GetMIRModule()));
  FieldVector muidFields;
#ifdef USE_64BIT_MUID
  GlobalTables::GetTypeTable().PushIntoFieldVector(muidFields, "muidLow", *GlobalTables::GetTypeTable().GetUInt32());
  GlobalTables::GetTypeTable().PushIntoFieldVector(muidFields, "muidHigh", *GlobalTables::GetTypeTable().GetUInt32());
#else
  GlobalTables::GetTypeTable().PushIntoFieldVector(muidFields, "muidLow", *GlobalTables::GetTypeTable().GetUInt64());
  GlobalTables::GetTypeTable().PushIntoFieldVector(muidFields, "muidHigh", *GlobalTables::GetTypeTable().GetUInt64());
#endif  // USE_64BIT_MUID
  auto *funcDefMuidTabEntryType =
      static_cast<MIRStructType*>(GlobalTables::GetTypeTable().GetOrCreateStructType(
          "MUIDFuncDefMuidTabEntry", muidFields, parentFields, GetMIRModule()));
  MIRArrayType &arrayType = *GlobalTables::GetTypeTable().GetOrCreateArrayType(*funcDefTabEntryType, arraySize);
  MIRAggConst *funcDefTabConst = GetMIRModule().GetMemPool()->New<MIRAggConst>(GetMIRModule(), arrayType);
  MIRArrayType &funcInfArrayType = *GlobalTables::GetTypeTable().GetOrCreateArrayType(*funcInfTabEntryType, arraySize);
  MIRAggConst *funcInfTabConst = GetMIRModule().GetMemPool()->New<MIRAggConst>(GetMIRModule(), funcInfArrayType);
  MIRArrayType &muidArrayType = *GlobalTables::GetTypeTable().GetOrCreateArrayType(*funcDefMuidTabEntryType, arraySize);
  MIRAggConst *funcDefMuidTabConst = GetMIRModule().GetMemPool()->New<MIRAggConst>(GetMIRModule(), muidArrayType);
  // Create funcDefSet to store functions sorted by address
  std::vector<std::pair<MIRSymbol*, MUID>> funcDefArray;
  idx = 0;
  for (MIRFunction *mirFunc : GetMIRModule().GetFunctionList()) {
    ASSERT(mirFunc != nullptr, "null ptr check!");
    MUID muid = GetMUID(mirFunc->GetName());
    MapleMap<MUID, SymIdxPair>::iterator iter = funcDefMap.find(muid);
    if (mirFunc->GetBody() == nullptr || iter == funcDefMap.end()) {
      continue;
    }
    funcDefArray.push_back(std::make_pair(mirFunc->GetFuncSymbol(), muid));
    // Create muidIdxTab to store the index in funcDefTab and funcDefMuidTab
    // With muidIdxTab, we can use index sorted by muid to find the index in funcDefTab and funcDefMuidTab
    // Use the left 1 bit of muidIdx to mark wether the function is weak or not. 1 is for weak
    MIRIntConst *indexConst = nullptr;
    if (reflectionList.find(mirFunc->GetName()) != reflectionList.end()) {
      constexpr uint32 weakFuncFlag = 0x80000000; // 0b10000000 00000000 00000000 00000000
      indexConst =
        GetMIRModule().GetMemPool()->New<MIRIntConst>(weakFuncFlag | idx, *GlobalTables::GetTypeTable().GetUInt32());
    } else {
      indexConst = GetMIRModule().GetMemPool()->New<MIRIntConst>(idx, *GlobalTables::GetTypeTable().GetUInt32());
    }
    uint32 muidIdx = iter->second.second;
    muidIdxTabConst->SetConstVecItem(muidIdx, *indexConst);
        // Store the real idx of funcdefTab, for ReplaceAddroffuncConst->FindIndexFromDefTable
    defMuidIdxMap[muid] = idx;
    idx++;
    if (trace) {
      LogInfo::MapleLogger() << "funcDefMap, MUID: " << muid.ToStr()
                             << ", Function Name: " << iter->second.first->GetName()
                             << ", Offset in addr order: " << (idx - 1)
                             << ", Offset in muid order: " << iter->second.second << "\n";
    }
  }
  // Create funcDefTab, funcInfoTab and funcMuidTab sorted by address, funcMuidIdxTab sorted by muid
  for (auto keyVal : funcDefArray) {
    MIRSymbol *funcSymbol = keyVal.first;
    MUID muid = keyVal.second;
    MIRAggConst *entryConst = GetMIRModule().GetMemPool()->New<MIRAggConst>(GetMIRModule(), *funcDefTabEntryType);
    uint32 fieldID = 1;
    MIRAggConst *funcInfEntryConst = GetMIRModule().GetMemPool()->New<MIRAggConst>(GetMIRModule(), *funcInfTabEntryType);
    uint32 funcInfFieldID = 1;
    MIRAggConst *muidEntryConst = GetMIRModule().GetMemPool()->New<MIRAggConst>(GetMIRModule(), *funcDefMuidTabEntryType);
    uint32 muidFieldID = 1;
    // To be processed by runtime
    builder->AddAddroffuncFieldConst(*funcDefTabEntryType, *entryConst, fieldID++, *funcSymbol);
    funcDefTabConst->GetConstVec().push_back(entryConst);
    // To be emitted as method size by CG
    builder->AddAddroffuncFieldConst(*funcInfTabEntryType, *funcInfEntryConst, funcInfFieldID++, *funcSymbol);
    // To be emitted as method name by CG
    builder->AddAddroffuncFieldConst(*funcInfTabEntryType, *funcInfEntryConst, funcInfFieldID++, *funcSymbol);
    funcInfTabConst->GetConstVec().push_back(funcInfEntryConst);
    builder->AddIntFieldConst(*funcDefMuidTabEntryType, *muidEntryConst, muidFieldID++, muid.data.words[0]);
    builder->AddIntFieldConst(*funcDefMuidTabEntryType, *muidEntryConst, muidFieldID++, muid.data.words[1]);
    funcDefMuidTabConst->GetConstVec().push_back(muidEntryConst);
    mplMuidStr += muid.ToStr();
  }
  if (!funcDefTabConst->GetConstVec().empty()) {
    std::string funcDefTabName = NameMangler::kMuidFuncDefTabPrefixStr + GetMIRModule().GetFileNameAsPostfix();
    funcDefTabSym = builder->CreateGlobalDecl(funcDefTabName.c_str(), arrayType);
    funcDefTabSym->SetKonst(funcDefTabConst);
    funcDefTabSym->SetStorageClass(kScFstatic);
  }
  if (!funcInfTabConst->GetConstVec().empty()) {
    std::string funcInfTabName = NameMangler::kMuidFuncInfTabPrefixStr + GetMIRModule().GetFileNameAsPostfix();
    funcInfTabSym = builder->CreateGlobalDecl(funcInfTabName.c_str(), funcInfArrayType);
    funcInfTabSym->SetKonst(funcInfTabConst);
    funcInfTabSym->SetStorageClass(kScFstatic);
  }
  if (!funcDefMuidTabConst->GetConstVec().empty()) {
    std::string funcDefMuidTabName = NameMangler::kMuidFuncDefMuidTabPrefixStr + GetMIRModule().GetFileNameAsPostfix();
    funcDefMuidTabSym = builder->CreateGlobalDecl(funcDefMuidTabName.c_str(), muidArrayType);
    funcDefMuidTabSym->SetKonst(funcDefMuidTabConst);
    funcDefMuidTabSym->SetStorageClass(kScFstatic);
  }
  if (!muidIdxTabConst->GetConstVec().empty()) {
    std::string muidIdxTabName = NameMangler::kMuidFuncMuidIdxTabPrefixStr + GetMIRModule().GetFileNameAsPostfix();
    funcMuidIdxTabSym = builder->CreateGlobalDecl(muidIdxTabName.c_str(), muidIdxArrayType);
    funcMuidIdxTabSym->SetKonst(muidIdxTabConst);
    funcMuidIdxTabSym->SetStorageClass(kScFstatic);
  }
  if (Options::dumpMuidFile) {
    DumpMUIDFile(true);
  }
}

void MUIDReplacement::GenericDataDefTable() {
  // Use dataDefMap to make sure dataDefTab is sorted by an increasing order of MUID
  for (MIRSymbol *mirSymbol : dataDefSet) {
    MUID muid = GetMUID(mirSymbol->GetName());
    CHECK_FATAL(dataDefMap.find(muid) == dataDefMap.end(), "MUID has been used before, possible collision");
    // Use 0 as the index for now. It will be back-filled once we have the whole map.
    dataDefMap[muid] = SymIdxPair(mirSymbol, 0);
  }
  uint32 idx = 0;
  for (auto &keyVal : dataDefMap) {
    // Fill in the real index
    keyVal.second.second = idx++;
  }
  FieldVector parentFields;
  FieldVector fields;
  GlobalTables::GetTypeTable().PushIntoFieldVector(fields, "dataUnifiedAddr",
                                                   *GlobalTables::GetTypeTable().GetCompactPtr());
  auto *dataDefTabEntryType = static_cast<MIRStructType*>(
      GlobalTables::GetTypeTable().GetOrCreateStructType("MUIDDataDefTabEntry", fields, parentFields, GetMIRModule()));
  FieldVector muidFields;
#ifdef USE_64BIT_MUID  // USE_64BIT_MUID
  GlobalTables::GetTypeTable().PushIntoFieldVector(muidFields, "muidLow", *GlobalTables::GetTypeTable().GetUInt32());
  GlobalTables::GetTypeTable().PushIntoFieldVector(muidFields, "muidHigh", *GlobalTables::GetTypeTable().GetUInt32());
#else  // USE_128BIT_MUID
  GlobalTables::GetTypeTable().PushIntoFieldVector(muidFields, "muidLow", *GlobalTables::GetTypeTable().GetUInt64());
  GlobalTables::GetTypeTable().PushIntoFieldVector(muidFields, "muidHigh", *GlobalTables::GetTypeTable().GetUInt64());
#endif
  auto *dataDefMuidTabEntryType =
      static_cast<MIRStructType*>(GlobalTables::GetTypeTable().GetOrCreateStructType(
          std::string("MUIDDataDefMuidTabEntry"), muidFields, parentFields, GetMIRModule()));
  size_t arraySize = dataDefMap.size();
  MIRArrayType &arrayType = *GlobalTables::GetTypeTable().GetOrCreateArrayType(*dataDefTabEntryType, arraySize);
  MIRAggConst *dataDefTabConst = GetMIRModule().GetMemPool()->New<MIRAggConst>(GetMIRModule(), arrayType);
  MIRArrayType &muidArrayType = *GlobalTables::GetTypeTable().GetOrCreateArrayType(*dataDefMuidTabEntryType, arraySize);
  MIRAggConst *dataDefMuidTabConst = GetMIRModule().GetMemPool()->New<MIRAggConst>(GetMIRModule(), muidArrayType);
  for (auto keyVal : dataDefMap) {
    MIRSymbol *mirSymbol = keyVal.second.first;
    MIRAggConst *entryConst = GetMIRModule().GetMemPool()->New<MIRAggConst>(GetMIRModule(), *dataDefTabEntryType);
    uint32 fieldID = 1;
    MUID muid = keyVal.first;
    MIRAggConst *muidEntryConst = GetMIRModule().GetMemPool()->New<MIRAggConst>(GetMIRModule(), *dataDefMuidTabEntryType);
    uint32 muidFieldID = 1;
    // Will be emitted as 0 and processed by runtime
    builder->AddAddrofFieldConst(*dataDefTabEntryType, *entryConst, fieldID++, *mirSymbol);
    dataDefTabConst->GetConstVec().push_back(entryConst);
    builder->AddIntFieldConst(*dataDefMuidTabEntryType, *muidEntryConst, muidFieldID++, muid.data.words[0]);
    builder->AddIntFieldConst(*dataDefMuidTabEntryType, *muidEntryConst, muidFieldID++, muid.data.words[1]);
    dataDefMuidTabConst->GetConstVec().push_back(muidEntryConst);
    mplMuidStr += muid.ToStr();
    if (trace) {
      LogInfo::MapleLogger() << "dataDefMap, MUID: " << muid.ToStr() << ", Variable Name: " << mirSymbol->GetName()
                             << ", Offset: " << keyVal.second.second << "\n";
    }
  }
  if (Options::dumpMuidFile) {
    DumpMUIDFile(false);
  }
  if (!dataDefTabConst->GetConstVec().empty()) {
    std::string dataDefTabName = NameMangler::kMuidDataDefTabPrefixStr + GetMIRModule().GetFileNameAsPostfix();
    dataDefTabSym = builder->CreateGlobalDecl(dataDefTabName.c_str(), arrayType);
    dataDefTabSym->SetKonst(dataDefTabConst);
    dataDefTabSym->SetStorageClass(kScFstatic);
  }
  if (!dataDefMuidTabConst->GetConstVec().empty()) {
    std::string dataDefMuidTabName = NameMangler::kMuidDataDefMuidTabPrefixStr + GetMIRModule().GetFileNameAsPostfix();
    dataDefMuidTabSym = builder->CreateGlobalDecl(dataDefMuidTabName.c_str(), muidArrayType);
    dataDefMuidTabSym->SetKonst(dataDefMuidTabConst);
    dataDefMuidTabSym->SetStorageClass(kScFstatic);
  }
}

void MUIDReplacement::GenericUnifiedUndefTable() {
  for (MIRFunction *mirFunc : funcUndefSet) {
    MUID muid = GetMUID(mirFunc->GetName());
    CHECK_FATAL(funcUndefMap.find(muid) == funcUndefMap.end(), "MUID has been used before, possible collision");
    // Use 0 as the index for now. It will be back-filled once we have the whole map.
    funcUndefMap[muid] = SymIdxPair(mirFunc->GetFuncSymbol(), 0);
  }
  for (MIRSymbol *mirSymbol : dataUndefSet) {
    MUID muid = GetMUID(mirSymbol->GetName());
    CHECK_FATAL(dataUndefMap.find(muid) == dataUndefMap.end(), "MUID has been used before, possible collision");
    // Use 0 as the index for now. It will be back-filled once we have the whole map.
    dataUndefMap[muid] = SymIdxPair(mirSymbol, 0);
  }
  // Fill in the real index.
  uint32 idx = 0;
  for (auto &keyVal : funcUndefMap) {
    keyVal.second.second = idx++;
  }
  idx = 0;
  for (auto &keyVal : dataUndefMap) {
    keyVal.second.second = idx++;
  }
  FieldVector parentFields;
  FieldVector fields;
  GlobalTables::GetTypeTable().PushIntoFieldVector(fields, "globalAddress",
                                                   *GlobalTables::GetTypeTable().GetCompactPtr());
  auto *unifiedUndefTabEntryType =
      static_cast<MIRStructType*>(GlobalTables::GetTypeTable().GetOrCreateStructType(
          std::string("MUIDUnifiedUndefTabEntry"), fields, parentFields, GetMIRModule()));
  FieldVector muidFields;
#ifdef USE_64BIT_MUID
  GlobalTables::GetTypeTable().PushIntoFieldVector(muidFields, "muidLow", *GlobalTables::GetTypeTable().GetUInt32());
  GlobalTables::GetTypeTable().PushIntoFieldVector(muidFields, "muidHigh", *GlobalTables::GetTypeTable().GetUInt32());
#else
  GlobalTables::GetTypeTable().PushIntoFieldVector(muidFields, "muidLow", *GlobalTables::GetTypeTable().GetUInt64());
  GlobalTables::GetTypeTable().PushIntoFieldVector(muidFields, "muidHigh", *GlobalTables::GetTypeTable().GetUInt64());
#endif
  auto *unifiedUndefMuidTabEntryType =
      static_cast<MIRStructType*>(GlobalTables::GetTypeTable().GetOrCreateStructType(
          "MUIDUnifiedUndefMuidTabEntry", muidFields, parentFields, GetMIRModule()));
  size_t arraySize = funcUndefMap.size();
  MIRArrayType &funcArrayType = *GlobalTables::GetTypeTable().GetOrCreateArrayType(*unifiedUndefTabEntryType, arraySize);
  MIRAggConst *funcUndefTabConst = GetMIRModule().GetMemPool()->New<MIRAggConst>(GetMIRModule(), funcArrayType);
  MIRArrayType &funcMuidArrayType =
      *GlobalTables::GetTypeTable().GetOrCreateArrayType(*unifiedUndefMuidTabEntryType, arraySize);
  MIRAggConst *funcUndefMuidTabConst = GetMIRModule().GetMemPool()->New<MIRAggConst>(GetMIRModule(), funcMuidArrayType);
  for (auto keyVal : funcUndefMap) {
    MUID muid = keyVal.first;
    mplMuidStr += muid.ToStr();
    if (trace) {
      LogInfo::MapleLogger() << "funcUndefMap, MUID: " << muid.ToStr()
                             << ", Function Name: " << keyVal.second.first->GetName()
                             << ", Offset: " << keyVal.second.second << "\n";
    }
    MIRAggConst *entryConst = GetMIRModule().GetMemPool()->New<MIRAggConst>(GetMIRModule(), *unifiedUndefTabEntryType);
    uint32 fieldID = 1;
    MIRAggConst *muidEntryConst =
      GetMIRModule().GetMemPool()->New<MIRAggConst>(GetMIRModule(), *unifiedUndefMuidTabEntryType);
    uint32 muidFieldID = 1;
    // to be filled by runtime
    builder->AddIntFieldConst(*unifiedUndefTabEntryType, *entryConst, fieldID++, 0);
    funcUndefTabConst->GetConstVec().push_back(entryConst);
    builder->AddIntFieldConst(*unifiedUndefMuidTabEntryType, *muidEntryConst, muidFieldID++, muid.data.words[0]);
    builder->AddIntFieldConst(*unifiedUndefMuidTabEntryType, *muidEntryConst, muidFieldID++, muid.data.words[1]);
    funcUndefMuidTabConst->GetConstVec().push_back(muidEntryConst);
  }
  if (!funcUndefTabConst->GetConstVec().empty()) {
    std::string funcUndefTabName = NameMangler::kMuidFuncUndefTabPrefixStr + GetMIRModule().GetFileNameAsPostfix();
    funcUndefTabSym = builder->CreateGlobalDecl(funcUndefTabName.c_str(), funcArrayType);
    funcUndefTabSym->SetKonst(funcUndefTabConst);
    funcUndefTabSym->SetStorageClass(kScFstatic);
  }
  if (!funcUndefMuidTabConst->GetConstVec().empty()) {
    std::string funcUndefMuidTabName =
      NameMangler::kMuidFuncUndefMuidTabPrefixStr + GetMIRModule().GetFileNameAsPostfix();
    funcUndefMuidTabSym = builder->CreateGlobalDecl(funcUndefMuidTabName.c_str(), funcMuidArrayType);
    funcUndefMuidTabSym->SetKonst(funcUndefMuidTabConst);
    funcUndefMuidTabSym->SetStorageClass(kScFstatic);
  }
  // Continue to generate dataUndefTab
  arraySize = dataUndefMap.size();
  MIRArrayType &dataArrayType = *GlobalTables::GetTypeTable().GetOrCreateArrayType(*unifiedUndefTabEntryType, arraySize);
  MIRAggConst *dataUndefTabConst = GetMIRModule().GetMemPool()->New<MIRAggConst>(GetMIRModule(), dataArrayType);
  MIRArrayType &dataMuidArrayType =
      *GlobalTables::GetTypeTable().GetOrCreateArrayType(*unifiedUndefMuidTabEntryType, arraySize);
  MIRAggConst *dataUndefMuidTabConst = GetMIRModule().GetMemPool()->New<MIRAggConst>(GetMIRModule(), dataMuidArrayType);
  for (auto keyVal : dataUndefMap) {
    MIRAggConst *entryConst = GetMIRModule().GetMemPool()->New<MIRAggConst>(GetMIRModule(), *unifiedUndefTabEntryType);
    uint32 fieldID = 1;
    MIRSymbol *mirSymbol = keyVal.second.first;
    MUID muid = keyVal.first;
    MIRAggConst *muidEntryConst =
      GetMIRModule().GetMemPool()->New<MIRAggConst>(GetMIRModule(), *unifiedUndefMuidTabEntryType);
    uint32 muidFieldID = 1;
    // Will be emitted as 0 and filled by runtime
    builder->AddAddrofFieldConst(*unifiedUndefTabEntryType, *entryConst, fieldID++, *mirSymbol);
    dataUndefTabConst->GetConstVec().push_back(entryConst);
    builder->AddIntFieldConst(*unifiedUndefMuidTabEntryType, *muidEntryConst, muidFieldID++, muid.data.words[0]);
    builder->AddIntFieldConst(*unifiedUndefMuidTabEntryType, *muidEntryConst, muidFieldID++, muid.data.words[1]);
    dataUndefMuidTabConst->GetConstVec().push_back(muidEntryConst);
    mplMuidStr += muid.ToStr();
    if (trace) {
      LogInfo::MapleLogger() << "dataUndefMap, MUID: " << muid.ToStr() << ", Variable Name: " << mirSymbol->GetName()
                             << ", Offset: " << keyVal.second.second << "\n";
    }
  }
  if (!dataUndefTabConst->GetConstVec().empty()) {
    std::string dataUndefTabName = NameMangler::kMuidDataUndefTabPrefixStr + GetMIRModule().GetFileNameAsPostfix();
    dataUndefTabSym = builder->CreateGlobalDecl(dataUndefTabName.c_str(), dataArrayType);
    dataUndefTabSym->SetKonst(dataUndefTabConst);
    dataUndefTabSym->SetStorageClass(kScFstatic);
  }
  if (!dataUndefMuidTabConst->GetConstVec().empty()) {
    std::string dataUndefMuidTabName =
      NameMangler::kMuidDataUndefMuidTabPrefixStr + GetMIRModule().GetFileNameAsPostfix();
    dataUndefMuidTabSym = builder->CreateGlobalDecl(dataUndefMuidTabName.c_str(), dataMuidArrayType);
    dataUndefMuidTabSym->SetKonst(dataUndefMuidTabConst);
    dataUndefMuidTabSym->SetStorageClass(kScFstatic);
  }
}

// RangeTable stores begin and end of all MUID tables
void MUIDReplacement::GenericRangeTable() {
  FieldVector parentFields;
  FieldVector fields;
  GlobalTables::GetTypeTable().PushIntoFieldVector(fields, "tabBegin", *GlobalTables::GetTypeTable().GetVoidPtr());
  GlobalTables::GetTypeTable().PushIntoFieldVector(fields, "tabEnd", *GlobalTables::GetTypeTable().GetVoidPtr());
  MIRStructType &rangeTabEntryType = static_cast<MIRStructType&>(
      *GlobalTables::GetTypeTable().GetOrCreateStructType("MUIDRangeTabEntry", fields, parentFields, GetMIRModule()));
  MIRArrayType &rangeArrayType = *GlobalTables::GetTypeTable().GetOrCreateArrayType(rangeTabEntryType, 0);
  MIRAggConst *rangeTabConst = GetMIRModule().GetMemPool()->New<MIRAggConst>(GetMIRModule(), rangeArrayType);
  // First entry is reserved for a compile-time-stamp
  // Second entry is reserved for a decouple-stamp
  mplMuidStr += kMplLinkerVersionNumber;
  const std::string muidStr[2] = { mplMuidStr, mplMuidStr + GetMplMd5().ToStr() };
  for (auto &item : muidStr) {
    uint32 fieldID = 1;
    MIRAggConst *entryConst = GetMIRModule().GetMemPool()->New<MIRAggConst>(GetMIRModule(), rangeTabEntryType);
    MUID mplMd5 = GetMUID(item);
    builder->AddIntFieldConst(rangeTabEntryType, *entryConst, fieldID++, mplMd5.data.words[0]);
    builder->AddIntFieldConst(rangeTabEntryType, *entryConst, fieldID++, mplMd5.data.words[1]);
    rangeTabConst->GetConstVec().push_back(entryConst);
  }
  for (uint32 i = RangeIdx::kVtab; i < RangeIdx::kMaxNum; i++) {
    // Use an integer to mark which entry is for which table
    MIRAggConst *entryConst = GetMIRModule().GetMemPool()->New<MIRAggConst>(GetMIRModule(), rangeTabEntryType);
    uint32 fieldID = 1;
    if (i == RangeIdx::kGlobalRootlist) {
      MIRSymbol *st = GetSymbolFromName(NameMangler::kGcRootList);
      if (st == nullptr) {
        builder->AddIntFieldConst(rangeTabEntryType, *entryConst, fieldID++, 0);
        builder->AddIntFieldConst(rangeTabEntryType, *entryConst, fieldID++, 0);
        rangeTabConst->GetConstVec().push_back(entryConst);
        continue;
      }
    }
    builder->AddIntFieldConst(rangeTabEntryType, *entryConst, fieldID++, i);
    builder->AddIntFieldConst(rangeTabEntryType, *entryConst, fieldID++, i);
    rangeTabConst->GetConstVec().push_back(entryConst);
  }
  // Please refer to mrt/compiler-rt/include/mpl_linker.h for the layout
  std::vector<MIRSymbol*> workList = {
    funcDefTabSym,
    funcDefOrigTabSym,
    funcInfTabSym,
    funcUndefTabSym,
    dataDefTabSym,
    dataDefOrigTabSym,
    dataUndefTabSym,
    funcDefMuidTabSym,
    funcUndefMuidTabSym,
    dataDefMuidTabSym,
    dataUndefMuidTabSym,
    funcMuidIdxTabSym,
    funcProfileTabSym
  };
  for (MIRSymbol *mirSymbol : workList) {
    MIRAggConst *entryConst = GetMIRModule().GetMemPool()->New<MIRAggConst>(GetMIRModule(), rangeTabEntryType);
    uint32 fieldID = 1;
    if (mirSymbol != nullptr) {
      builder->AddAddrofFieldConst(rangeTabEntryType, *entryConst, fieldID++, *mirSymbol);
      builder->AddAddrofFieldConst(rangeTabEntryType, *entryConst, fieldID++, *mirSymbol);
    } else {
      builder->AddIntFieldConst(rangeTabEntryType, *entryConst, fieldID++, 0);
      builder->AddIntFieldConst(rangeTabEntryType, *entryConst, fieldID++, 0);
    }
    rangeTabConst->GetConstVec().push_back(entryConst);
  }
  if (!rangeTabConst->GetConstVec().empty()) {
    rangeArrayType.SetSizeArrayItem(0, rangeTabConst->GetConstVec().size());
    std::string rangeTabName = NameMangler::kMuidRangeTabPrefixStr + GetMIRModule().GetFileNameAsPostfix();
    rangeTabSym = builder->CreateGlobalDecl(rangeTabName.c_str(), rangeArrayType);
    rangeTabSym->SetKonst(rangeTabConst);
    rangeTabSym->SetStorageClass(kScFstatic);
  }
}

uint32 MUIDReplacement::FindIndexFromDefTable(const MIRSymbol &mirSymbol, bool isFunc) {
  MUID muid = GetMUID(mirSymbol.GetName());
  if (isFunc) {
    CHECK_FATAL(defMuidIdxMap.find(muid) != defMuidIdxMap.end(), "Local function %s not found in funcDefMap",
                mirSymbol.GetName().c_str());
    return defMuidIdxMap[muid];
  } else {
    CHECK_FATAL(dataDefMap.find(muid) != dataDefMap.end(), "Local variable %s not found in dataDefMap",
                mirSymbol.GetName().c_str());
    return dataDefMap[muid].second;
  }
}

uint32 MUIDReplacement::FindIndexFromUndefTable(const MIRSymbol &mirSymbol, bool isFunc) {
  MUID muid = GetMUID(mirSymbol.GetName());
  if (isFunc) {
    CHECK_FATAL(funcUndefMap.find(muid) != funcUndefMap.end(), "Extern function %s not found in funcUndefMap",
                mirSymbol.GetName().c_str());
    return funcUndefMap[muid].second;
  } else {
    CHECK_FATAL(dataUndefMap.find(muid) != dataUndefMap.end(), "Extern variable %s not found in dataUndefMap",
                mirSymbol.GetName().c_str());
    return dataUndefMap[muid].second;
  }
}

void MUIDReplacement::ClearVtabItab(const std::string &name) {
  MIRSymbol *tabSym = GetSymbolFromName(name);
  if (tabSym == nullptr) {
    return;
  }
  auto *oldConst = tabSym->GetKonst();
  if (oldConst == nullptr || oldConst->GetKind() != kConstAggConst) {
    return;
  }
  static_cast<MIRAggConst*>(oldConst)->GetConstVec().clear();
  return;
}

void MUIDReplacement::ReplaceFuncTable(const std::string &name) {
  MIRSymbol *tabSym = GetSymbolFromName(name);
  if (tabSym == nullptr) {
    return;
  }
  auto *oldConst = tabSym->GetKonst();
  if (oldConst == nullptr || oldConst->GetKind() != kConstAggConst) {
    return;
  }
  bool isVtab = false;
  if (tabSym->GetName().find(VTAB_PREFIX_STR) == 0) {
    isVtab = true;
  }
  for (auto *&oldTabEntry : static_cast<MIRAggConst*>(oldConst)->GetConstVec()) {
    if (oldTabEntry->GetKind() == kConstAggConst) {
      auto *aggrC = static_cast<MIRAggConst*>(oldTabEntry);
      for (size_t i = 0; i < aggrC->GetConstVec().size(); i++) {
        ReplaceAddroffuncConst(aggrC->GetConstVecItem(i), i + 1, isVtab);
      }
    } else if (oldTabEntry->GetKind() == kConstAddrofFunc) {
      ReplaceAddroffuncConst(oldTabEntry, 0xffffffff, isVtab);
    }
  }
}

void MUIDReplacement::ReplaceAddroffuncConst(MIRConst *&entry, uint32 fieldID, bool isVtab = false) {
  if (entry->GetKind() != kConstAddrofFunc) {
    return;
  }
  MIRType &voidType = *GlobalTables::GetTypeTable().GetVoidPtr();
  auto *funcAddr = static_cast<MIRAddroffuncConst*>(entry);
  MIRFunction *func = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(funcAddr->GetValue());
  ASSERT(func != nullptr, "Invalid MIRFunction");
  uint64 offset = 0;
  MIRIntConst *constNode = nullptr;
  constexpr uint64 reservedBits = 2u;
  if (func->GetBody() != nullptr) {
    ASSERT(func->GetFuncSymbol() != nullptr, "null ptr check!");
    offset = FindIndexFromDefTable(*(func->GetFuncSymbol()), true);
    // Left shifting is needed because in itable 0 and 1 are reserved.
    // 0 marks no entry and 1 marks a conflict.
    // The second least significant bit is set to 1, indicating
    // this is an index into the funcDefTab
    constexpr uint64 idxIntoFuncDefTabFlag = 2u;
    constNode = GetMIRModule().GetMemPool()->New<MIRIntConst>(((offset + 1) << reservedBits) + idxIntoFuncDefTabFlag,
                                                                  voidType);
  } else if (isVtab && func->IsAbstract()) {
    MIRType &type = *GlobalTables::GetTypeTable().GetVoidPtr();
    constNode = GetMIRModule().GetMemPool()->New<MIRIntConst>(0, type);
  } else {
    ASSERT(func->GetFuncSymbol() != nullptr, "null ptr check!");
    offset = FindIndexFromUndefTable(*(func->GetFuncSymbol()), true);
    // The second least significant bit is set to 0, indicating
    // this is an index into the funcUndefTab
    constNode = GetMIRModule().GetMemPool()->New<MIRIntConst>((offset + 1) << reservedBits, voidType);
  }
  if (fieldID != 0xffffffff) {
    constNode->SetFieldID(fieldID);
  }
  entry = constNode;
}

void MUIDReplacement::ReplaceDataTable(const std::string &name) {
  MIRSymbol *tabSym = GetSymbolFromName(name);
  if (tabSym == nullptr) {
    return;
  }
  auto *oldConst = static_cast<MIRAggConst*>(tabSym->GetKonst());
  if (oldConst == nullptr) {
    return;
  }
  for (MIRConst *&oldTabEntry : oldConst->GetConstVec()) {
    ASSERT(oldTabEntry != nullptr, "null ptr check!");
    if (oldTabEntry->GetKind() == kConstAggConst) {
      auto *aggrC = static_cast<MIRAggConst*>(oldTabEntry);
      for (size_t i = 0; i < aggrC->GetConstVec().size(); i++) {
        ASSERT(aggrC->GetConstVecItem(i) != nullptr, "null ptr check!");
        ReplaceAddrofConst(aggrC->GetConstVecItem(i));
        aggrC->GetConstVecItem(i)->SetFieldID(i + 1);
      }
    } else if (oldTabEntry->GetKind() == kConstAddrof) {
      ReplaceAddrofConst(oldTabEntry);
    }
  }
}

void MUIDReplacement::ReplaceAddrofConst(MIRConst *&entry) {
  if (entry->GetKind() != kConstAddrof) {
    return;
  }
  MIRType &voidType = *GlobalTables::GetTypeTable().GetVoidPtr();
  auto *addr = static_cast<MIRAddrofConst*>(entry);
  MIRSymbol *addrSym = GlobalTables::GetGsymTable().GetSymbolFromStidx(addr->GetSymbolIndex().Idx());
  ASSERT(addrSym != nullptr, "Invalid MIRSymbol");
  if (!addrSym->IsReflectionClassInfo() && !addrSym->IsStatic()) {
    return;
  }
  uint64 offset = 0;
  MIRIntConst *constNode = nullptr;
  if (addrSym->GetStorageClass() != kScExtern) {
    offset = FindIndexFromDefTable(*addrSym, false);
    constNode = GetMIRModule().GetMemPool()->New<MIRIntConst>(offset | kFromDefIndexMask, voidType);
  } else {
    offset = FindIndexFromUndefTable(*addrSym, false);
    constNode = GetMIRModule().GetMemPool()->New<MIRIntConst>(offset | kFromUndefIndexMask, voidType);
  }
  entry = constNode;
}

void MUIDReplacement::ReplaceDirectInvokeOrAddroffunc(MIRFunction &currentFunc, StmtNode &stmt) {
  PUIdx puidx;
  CallNode *callNode = nullptr;
  DassignNode *dassignNode = nullptr;
  RegassignNode *regassignNode = nullptr;
  if (stmt.GetOpCode() == OP_callassigned || stmt.GetOpCode() == OP_call) {
    callNode = static_cast<CallNode*>(&stmt);
    puidx = callNode->GetPUIdx();
  } else if (stmt.GetOpCode() == OP_dassign) {
    dassignNode = static_cast<DassignNode*>(&stmt);
    if (dassignNode->GetRHS()->GetOpCode() != OP_addroffunc) {
      return;
    }
    puidx = static_cast<AddroffuncNode*>(dassignNode->GetRHS())->GetPUIdx();
  } else if (stmt.GetOpCode() == OP_regassign) {
    regassignNode = static_cast<RegassignNode*>(&stmt);
    if (regassignNode->Opnd()->GetOpCode() != OP_addroffunc) {
      return;
    }
    puidx = static_cast<AddroffuncNode*>(regassignNode->Opnd())->GetPUIdx();
  } else {
    CHECK_FATAL(false, "unexpected stmt type in ReplaceDirectInvokeOrAddroffunc");
  }
  MIRFunction *calleeFunc = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(puidx);
  if (calleeFunc == nullptr || (!calleeFunc->IsJava() && calleeFunc->GetBaseClassName().empty())) {
    return;
  }
  // Load the function pointer
  AddrofNode *baseExpr = nullptr;
  uint32 index = 0;
  MIRArrayType *arrayType = nullptr;
  if (calleeFunc->GetBody() != nullptr) {
    // Local function is accessed through funcDefTab
    // Add a comment to store the original function name
    std::string commentLabel = NameMangler::kMarkMuidFuncDefStr + calleeFunc->GetName();
    currentFunc.GetBody()->InsertBefore(&stmt, builder->CreateStmtComment(commentLabel.c_str()));

    std::string moduleName = GetMIRModule().GetFileNameAsPostfix();
    std::string baseName = calleeFunc->GetBaseClassName();
    baseExpr = builder->CreateExprAddrof(0, *funcDefTabSym, GetMIRModule().GetMemPool());
    ASSERT(calleeFunc->GetFuncSymbol() != nullptr, "null ptr check!");
    index = FindIndexFromDefTable(*(calleeFunc->GetFuncSymbol()), true);
    arrayType = static_cast<MIRArrayType*>(funcDefTabSym->GetType());
  } else {
    // External function is accessed through funcUndefTab
    // Add a comment to store the original function name
    std::string commentLabel = NameMangler::kMarkMuidFuncUndefStr + calleeFunc->GetName();
    currentFunc.GetBody()->InsertBefore(&stmt, builder->CreateStmtComment(commentLabel.c_str()));

    baseExpr = builder->CreateExprAddrof(0, *funcUndefTabSym, GetMIRModule().GetMemPool());
    ASSERT(calleeFunc->GetFuncSymbol() != nullptr, "null ptr check!");
    index = FindIndexFromUndefTable(*(calleeFunc->GetFuncSymbol()), true);
    arrayType = static_cast<MIRArrayType*>(funcUndefTabSym->GetType());
  }
  ConstvalNode *offsetExpr = builder->CreateIntConst(index, PTY_i64);
  ArrayNode *arrayExpr = builder->CreateExprArray(*arrayType, baseExpr, offsetExpr);
  arrayExpr->SetBoundsCheck(false);
  MIRType *elemType = arrayType->GetElemType();
  BaseNode *ireadPtrExpr =
      builder->CreateExprIread(*GlobalTables::GetTypeTable().GetVoidPtr(),
                               *GlobalTables::GetTypeTable().GetOrCreatePointerType(*elemType), 1, arrayExpr);
  PregIdx funcPtrPreg = 0;
  MIRSymbol *funcPtrSym = nullptr;
  BaseNode *readFuncPtr = nullptr;
  if (Options::usePreg) {
    funcPtrPreg = currentFunc.GetPregTab()->CreatePreg(PTY_ptr);
    RegassignNode *funcPtrPregAssign = builder->CreateStmtRegassign(PTY_ptr, funcPtrPreg, ireadPtrExpr);
    currentFunc.GetBody()->InsertBefore(&stmt, funcPtrPregAssign);
    readFuncPtr = builder->CreateExprRegread(PTY_ptr, funcPtrPreg);
  } else {
    funcPtrSym = builder->GetOrCreateLocalDecl(kMuidSymPtrStr, *GlobalTables::GetTypeTable().GetVoidPtr());
    DassignNode *addrNode = builder->CreateStmtDassign(*funcPtrSym, 0, ireadPtrExpr);
    currentFunc.GetBody()->InsertBefore(&stmt, addrNode);
    readFuncPtr = builder->CreateExprDread(*funcPtrSym);
  }
  if (callNode != nullptr) {
    // Create icallNode to replace callNode
    IcallNode *icallNode = GetMIRModule().CurFuncCodeMemPool()->New<IcallNode>(
      GetMIRModule(), callNode->GetOpCode() == OP_call ? OP_icall : OP_icallassigned);
    icallNode->SetNumOpnds(callNode->GetNumOpnds() + 1);
    icallNode->GetNopnd().resize(icallNode->GetNumOpnds());
    icallNode->SetNOpndAt(0, readFuncPtr);
    for (size_t i = 1; i < icallNode->GetNopndSize(); i++) {
      icallNode->SetNOpndAt(i, callNode->GetNopnd()[i - 1]->CloneTree(GetMIRModule().GetCurFuncCodeMPAllocator()));
    }
    icallNode->SetRetTyIdx(calleeFunc->GetReturnTyIdx());
    if (callNode->GetOpCode() == OP_callassigned) {
      icallNode->SetReturnVec(callNode->GetReturnVec());
    }
    currentFunc.GetBody()->ReplaceStmt1WithStmt2(callNode, icallNode);
  } else if (dassignNode != nullptr) {
    dassignNode->SetRHS(readFuncPtr);
  } else if (regassignNode != nullptr) {
    regassignNode->SetOpnd(readFuncPtr);
  }
}

void MUIDReplacement::ReplaceDassign(MIRFunction &currentFunc, DassignNode &dassignNode) {
  MIRSymbol *mirSymbol = currentFunc.GetLocalOrGlobalSymbol(dassignNode.GetStIdx());
  ASSERT(mirSymbol != nullptr, "null ptr check!");
  if (!mirSymbol->IsStatic()) {
    return;
  }
  // Add a comment to store the original symbol name
  currentFunc.GetBody()->InsertBefore(&dassignNode,
                                      builder->CreateStmtComment(("Assign to: " + mirSymbol->GetName()).c_str()));
  // Load the symbol pointer
  AddrofNode *baseExpr = nullptr;
  uint32 index = 0;
  MIRArrayType *arrayType = nullptr;
  if (mirSymbol->GetStorageClass() != kScExtern) {
    // Local static member is accessed through dataDefTab
    baseExpr = builder->CreateExprAddrof(0, *dataDefTabSym);
    index = FindIndexFromDefTable(*mirSymbol, false);
    arrayType = static_cast<MIRArrayType*>(dataDefTabSym->GetType());
  } else {
    // External static member is accessed through dataUndefTab
    baseExpr = builder->CreateExprAddrof(0, *dataUndefTabSym);
    index = FindIndexFromUndefTable(*mirSymbol, false);
    arrayType = static_cast<MIRArrayType*>(dataUndefTabSym->GetType());
  }
  ConstvalNode *offsetExpr = builder->CreateIntConst(index, PTY_i64);
  ArrayNode *arrayExpr = builder->CreateExprArray(*arrayType, baseExpr, offsetExpr);
  arrayExpr->SetBoundsCheck(false);
  MIRType *elemType = arrayType->GetElemType();
  MIRType *mVoidPtr = GlobalTables::GetTypeTable().GetVoidPtr();
  CHECK_FATAL(mVoidPtr != nullptr, "null ptr check");
  BaseNode *ireadPtrExpr =
      builder->CreateExprIread(*mVoidPtr, *GlobalTables::GetTypeTable().GetOrCreatePointerType(*elemType), 1, arrayExpr);
  PregIdx symPtrPreg = 0;
  MIRSymbol *symPtrSym = nullptr;
  BaseNode *destExpr = nullptr;
  if (Options::usePreg) {
    symPtrPreg = currentFunc.GetPregTab()->CreatePreg(PTY_ptr);
    RegassignNode *symPtrPregAssign = builder->CreateStmtRegassign(PTY_ptr, symPtrPreg, ireadPtrExpr);
    currentFunc.GetBody()->InsertBefore(&dassignNode, symPtrPregAssign);
    destExpr = builder->CreateExprRegread(PTY_ptr, symPtrPreg);
  } else {
    symPtrSym = builder->GetOrCreateLocalDecl(kMuidFuncPtrStr, *mVoidPtr);
    DassignNode *addrNode = builder->CreateStmtDassign(*symPtrSym, 0, ireadPtrExpr);
    currentFunc.GetBody()->InsertBefore(&dassignNode, addrNode);
    destExpr = builder->CreateExprDread(*symPtrSym);
  }
  // Replace dassignNode with iassignNode
  MIRType *destPtrType = GlobalTables::GetTypeTable().GetOrCreatePointerType(*mirSymbol->GetType());
  StmtNode *iassignNode = builder->CreateStmtIassign(*destPtrType, 0, destExpr, dassignNode.Opnd(0));
  currentFunc.GetBody()->ReplaceStmt1WithStmt2(&dassignNode, iassignNode);
}


void MUIDReplacement::ReplaceDreadStmt(MIRFunction *currentFunc, StmtNode *stmt) {
  if (currentFunc == nullptr || stmt == nullptr) {
    return;
  }
  switch (stmt->GetOpCode()) {
    case OP_if: {
      auto *inode = static_cast<IfStmtNode*>(stmt);
      inode->SetOpnd(ReplaceDreadExpr(currentFunc, stmt, inode->Opnd(0)), 0);
      ReplaceDreadStmt(currentFunc, inode->GetThenPart());
      ReplaceDreadStmt(currentFunc, inode->GetElsePart());
      break;
    }
    case OP_while: {
      auto *wnode = static_cast<WhileStmtNode*>(stmt);
      wnode->SetOpnd(ReplaceDreadExpr(currentFunc, stmt, wnode->Opnd(0)), 0);
      ReplaceDreadStmt(currentFunc, wnode->GetBody());
      break;
    }
    case OP_block: {
      auto *bnode = static_cast<BlockNode*>(stmt);
      for (auto &s : bnode->GetStmtNodes()) {
        ReplaceDreadStmt(currentFunc, &s);
      }
      break;
    }
    default: {
      for (size_t i = 0; i < stmt->NumOpnds(); i++) {
        stmt->SetOpnd(ReplaceDreadExpr(currentFunc, stmt, stmt->Opnd(i)), i);
      }
      break;
    }
  }
}


// Turn dread into iread
BaseNode *MUIDReplacement::ReplaceDreadExpr(MIRFunction *currentFunc, StmtNode *stmt, BaseNode *expr) {
  if (currentFunc == nullptr || stmt == nullptr || expr == nullptr) {
    return nullptr;
  }
  size_t i = 0;
  UnaryNode *uOpnd = nullptr;
  BinaryNode *bopnds = nullptr;
  TernaryNode *topnds = nullptr;
  switch (expr->GetOpCode()) {
    case OP_dread:
    case OP_addrof: {
      return ReplaceDread(*currentFunc, stmt, expr);
    }
    case OP_select: {
      topnds = static_cast<TernaryNode*>(expr);
      for (i = 0; i < topnds->NumOpnds(); i++) {
        topnds->SetOpnd(ReplaceDreadExpr(currentFunc, stmt, topnds->Opnd(i)), i);
      }
      break;
    }
    default: {
      if (expr->IsUnaryNode()) {
        uOpnd = static_cast<UnaryNode*>(expr);
        uOpnd->SetOpnd(ReplaceDreadExpr(currentFunc, stmt, uOpnd->Opnd()), i);
      } else if (expr->IsBinaryNode()) {
        bopnds = static_cast<BinaryNode*>(expr);
        for (i = 0; i < bopnds->NumOpnds(); i++) {
          bopnds->SetOpnd(ReplaceDreadExpr(currentFunc, stmt, bopnds->GetBOpnd(i)), i);
        }
      } else {
        for (i = 0; i < expr->NumOpnds(); i++) {
          expr->SetOpnd(ReplaceDreadExpr(currentFunc, stmt, expr->Opnd(i)), i);
        }
      }
      break;
    }
  }
  return expr;
}

BaseNode *MUIDReplacement::ReplaceDread(MIRFunction &currentFunc, StmtNode *stmt, BaseNode *opnd) {
  if (opnd == nullptr || (opnd->GetOpCode() != OP_dread && opnd->GetOpCode() != OP_addrof)) {
    return opnd;
  }
  auto *dreadNode = static_cast<DreadNode*>(opnd);
  MIRSymbol *mirSymbol = currentFunc.GetLocalOrGlobalSymbol(dreadNode->GetStIdx());
  ASSERT(mirSymbol != nullptr, "null ptr check!");
  if (!mirSymbol->IsStatic()) {
    return opnd;
  }
  // Add a comment to store the original symbol name
  currentFunc.GetBody()->InsertBefore(stmt, builder->CreateStmtComment(("Read from: " + mirSymbol->GetName()).c_str()));
  // Load the symbol pointer
  AddrofNode *baseExpr = nullptr;
  uint32 index = 0;
  MIRArrayType *arrayType = nullptr;
  if (mirSymbol->GetStorageClass() != kScExtern) {
    // Local static member is accessed through dataDefTab
    baseExpr = builder->CreateExprAddrof(0, *dataDefTabSym);
    index = FindIndexFromDefTable(*mirSymbol, false);
    arrayType = static_cast<MIRArrayType*>(dataDefTabSym->GetType());
  } else {
    // External static member is accessed through dataUndefTab
    baseExpr = builder->CreateExprAddrof(0, *dataUndefTabSym);
    index = FindIndexFromUndefTable(*mirSymbol, false);
    arrayType = static_cast<MIRArrayType*>(dataUndefTabSym->GetType());
  }
  ConstvalNode *offsetExpr = builder->CreateIntConst(index, PTY_i64);
  ArrayNode *arrayExpr = builder->CreateExprArray(*arrayType, baseExpr, offsetExpr);
  arrayExpr->SetBoundsCheck(false);
  MIRType *elemType = arrayType->GetElemType();
  BaseNode *ireadPtrExpr =
      builder->CreateExprIread(*GlobalTables::GetTypeTable().GetVoidPtr(),
                               *GlobalTables::GetTypeTable().GetOrCreatePointerType(*elemType), 1, arrayExpr);
  if (opnd->GetOpCode() == OP_addrof) {
    return ireadPtrExpr;
  }
  MIRType *destType = mirSymbol->GetType();
  MIRType *destPtrType = GlobalTables::GetTypeTable().GetOrCreatePointerType(*destType);
  return builder->CreateExprIread(*destType, *destPtrType, 0, ireadPtrExpr);
}

void MUIDReplacement::ProcessFunc(MIRFunction *func) {
  // Libcore-all module is self-contained, so no need to do all these replacement
  ASSERT(func != nullptr, "null ptr check!");
  if (isLibcore || func->IsEmpty()) {
    return;
  }
  SetCurrentFunction(*func);
  StmtNode *stmt = func->GetBody()->GetFirst();
  StmtNode *next = nullptr;
  while (stmt != nullptr) {
    next = stmt->GetNext();
    ReplaceDreadStmt(func, stmt);
    // Replace direct func invoke
    if (stmt->GetOpCode() == OP_callassigned || stmt->GetOpCode() == OP_call) {
      ReplaceDirectInvokeOrAddroffunc(*func, *stmt);
    } else if (stmt->GetOpCode() == OP_dassign) {
      ReplaceDirectInvokeOrAddroffunc(*func, *stmt);
      auto *dassignNode = static_cast<DassignNode*>(stmt);
      ReplaceDassign(*func, *dassignNode);
    } else if (stmt->GetOpCode() == OP_regassign) {
      ReplaceDirectInvokeOrAddroffunc(*func, *stmt);
    }
    stmt = next;
  }
}

// Create GC Root
void MUIDReplacement::GenericGlobalRootList() {
  MIRType *voidType = GlobalTables::GetTypeTable().GetVoidPtr();
  MIRArrayType *arrayType = GlobalTables::GetTypeTable().GetOrCreateArrayType(*voidType, 0);
  MIRAggConst *newConst = GetMIRModule().GetMemPool()->New<MIRAggConst>(GetMIRModule(), *arrayType);
  for (StIdx stidx : GetMIRModule().GetSymbolSet()) {
    MIRSymbol *symbol = GlobalTables::GetGsymTable().GetSymbolFromStidx(stidx.Idx());
    MIRSymKind st = symbol->GetSKind();
    MIRStorageClass sc = symbol->GetStorageClass();
    if (!(st == kStVar && sc == kScGlobal)) {
      continue;
    }
    TyIdx typeIdx = symbol->GetTyIdx();
    MIRType *type = GlobalTables::GetTypeTable().GetTypeFromTyIdx(typeIdx);
    PrimType pty = type->GetPrimType();
    if (!(pty == PTY_ptr || pty == PTY_ref)) {
      continue;
    }
    // It is a pointer/ref type.  Check its pointed type.
    if (pty == PTY_ptr) {
      if (type->GetKind() != kTypePointer) {
        continue;
      }
      auto *pointType = static_cast<MIRPtrType*>(type);
      MIRType *pointedType = pointType->GetPointedType();
      if (!(pointedType->GetKind() == kTypeClass)) {
        continue;
      }
    }
    // Now it is a pointer/ref to a class.  Record it for GC scanning.
    ASSERT(PTY_ptr < GlobalTables::GetTypeTable().GetTypeTable().size(), "index out of bound");
    MIRType &ptrType = *GlobalTables::GetTypeTable().GetTypeTable()[PTY_ptr];
    MIRConst *constNode = GetMIRModule().GetMemPool()->New<MIRAddrofConst>(symbol->GetStIdx(), 0, ptrType);
    newConst->GetConstVec().push_back(constNode);
  }
  std::string gcRootsName = NameMangler::kGcRootList;
  if (!newConst->GetConstVec().empty()) {
    MIRSymbol *gcRootsSt = builder->CreateSymbol(newConst->GetType().GetTypeIndex(), gcRootsName.c_str(), kStVar,
                                                 kScAuto, nullptr, kScopeGlobal);
    arrayType->SetSizeArrayItem(0, newConst->GetConstVec().size());
    gcRootsSt->SetKonst(newConst);
  }
}

void MUIDReplacement::GenericCompilerVersionNum() {
  MIRType *ptrType = GlobalTables::GetTypeTable().GetVoidPtr();
  MIRArrayType &arrayType = *GlobalTables::GetTypeTable().GetOrCreateArrayType(*ptrType, 0);
  MIRAggConst *newConst = GetMIRModule().GetMemPool()->New<MIRAggConst>(GetMIRModule(), arrayType);
  MIRType &type = *GlobalTables::GetTypeTable().GetInt32();
  MIRConst *firstConst = GetMIRModule().GetMemPool()->New<MIRIntConst>(Version::kMajorMplVersion, type);
  MIRConst *secondConst = GetMIRModule().GetMemPool()->New<MIRIntConst>(Version::kMinorCompilerVersion, type);
  newConst->GetConstVec().push_back(firstConst);
  newConst->GetConstVec().push_back(secondConst);
  std::string symName = NameMangler::kCompilerVersionNum + GetMIRModule().GetFileNameAsPostfix();
  MIRSymbol *versionNum = builder->CreateGlobalDecl(symName.c_str(), arrayType);
  versionNum->SetKonst(newConst);
}

void MUIDReplacement::GenericTables() {
  GenericGlobalRootList();
  CollectFuncAndDataFromKlasses();
  CollectFuncAndDataFromGlobalTab();
  CollectFuncAndDataFromFuncList();
  GenericFuncDefTable();
  GenericDataDefTable();
  GenericUnifiedUndefTable();
  GenericRangeTable();
  // When MapleLinker is enabled, MUIDReplacement becomes the last
  // phase that updates the reflection string table, thus the table
  // is emitted here.
  ReflectionAnalysis::GenStrTab(GetMIRModule());
  // Replace undef entries in vtab/itab/reflectionMetaData
  for (Klass *klass : klassHierarchy->GetTopoSortedKlasses()) {
    ReplaceDataTable(SUPERCLASSINFO_PREFIX_STR + klass->GetKlassName());
    ReplaceFuncTable(VTAB_PREFIX_STR + klass->GetKlassName());
    ReplaceFuncTable(ITAB_PREFIX_STR + klass->GetKlassName());
    ReplaceFuncTable(ITAB_CONFLICT_PREFIX_STR + klass->GetKlassName());
  }
  ReplaceDataTable(NameMangler::kGcRootList);
  GenericCompilerVersionNum();
}
}  // namespace maple
