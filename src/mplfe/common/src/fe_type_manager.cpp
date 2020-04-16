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
#include "fe_type_manager.h"
#include <fstream>
#include <sstream>
#include "mir_parser.h"
#include "bin_mplt.h"
#include "global_tables.h"
#include "fe_timer.h"
#include "fe_config_parallel.h"

namespace maple {
const UniqueFEIRType FETypeManager::kPrimFEIRTypeUnknown = std::make_unique<FEIRTypeDefault>(PTY_unknown);
const UniqueFEIRType FETypeManager::kPrimFEIRTypeU1 = std::make_unique<FEIRTypeDefault>(PTY_u1);
const UniqueFEIRType FETypeManager::kPrimFEIRTypeI8 = std::make_unique<FEIRTypeDefault>(PTY_i8);
const UniqueFEIRType FETypeManager::kPrimFEIRTypeU8 = std::make_unique<FEIRTypeDefault>(PTY_u8);
const UniqueFEIRType FETypeManager::kPrimFEIRTypeI16 = std::make_unique<FEIRTypeDefault>(PTY_i16);
const UniqueFEIRType FETypeManager::kPrimFEIRTypeU16 = std::make_unique<FEIRTypeDefault>(PTY_u16);
const UniqueFEIRType FETypeManager::kPrimFEIRTypeI32 = std::make_unique<FEIRTypeDefault>(PTY_i32);
const UniqueFEIRType FETypeManager::kPrimFEIRTypeU32 = std::make_unique<FEIRTypeDefault>(PTY_u32);
const UniqueFEIRType FETypeManager::kPrimFEIRTypeI64 = std::make_unique<FEIRTypeDefault>(PTY_i64);
const UniqueFEIRType FETypeManager::kPrimFEIRTypeU64 = std::make_unique<FEIRTypeDefault>(PTY_u64);
const UniqueFEIRType FETypeManager::kPrimFEIRTypeF32 = std::make_unique<FEIRTypeDefault>(PTY_f32);
const UniqueFEIRType FETypeManager::kPrimFEIRTypeF64 = std::make_unique<FEIRTypeDefault>(PTY_f64);
const UniqueFEIRType FETypeManager::kFEIRTypeJavaObject = std::make_unique<FEIRTypeDefault>(PTY_ref);
const UniqueFEIRType FETypeManager::kFEIRTypeJavaClass = std::make_unique<FEIRTypeDefault>(PTY_ref);
const UniqueFEIRType FETypeManager::kFEIRTypeJavaString = std::make_unique<FEIRTypeDefault>(PTY_ref);

FETypeManager::FETypeManager(MIRModule &moduleIn)
    : module(moduleIn),
      mp(memPoolCtrler.NewMemPool("mempool for FETypeManager")),
      allocator(mp),
      builder(&module),
      srcLang(kSrcLangJava),
      funcMCCGetOrInsertLiteral(nullptr) {
  static_cast<FEIRTypeDefault*>(kFEIRTypeJavaObject.get())->LoadFromJavaTypeName("Ljava/lang/Object;", false);
  static_cast<FEIRTypeDefault*>(kFEIRTypeJavaClass.get())->LoadFromJavaTypeName("Ljava/lang/Class;", false);
  static_cast<FEIRTypeDefault*>(kFEIRTypeJavaString.get())->LoadFromJavaTypeName("Ljava/lang/String;", false);
}

FETypeManager::~FETypeManager() {
  mp = nullptr;
}

void FETypeManager::ReleaseMemPool() {
  memPoolCtrler.DeleteMemPool(mp);
  mp = nullptr;
}

bool FETypeManager::LoadMplts(const std::list<std::string> &mpltNames, FETypeFlag flag, const std::string &phaseName) {
  FETimer timer;
  timer.StartAndDump(phaseName);
  bool success = true;
  for (const std::string &fileName : mpltNames) {
    success = success && LoadMplt(fileName, flag);
  }
  timer.StopAndDumpTimeMS(phaseName);
  return success;
}

bool FETypeManager::LoadMplt(const std::string &mpltName, FETypeFlag flag) {
  BinaryMplt binMplt(module);
  if (!binMplt.Import(mpltName)) {
    // not a binary mplt
    std::ifstream file(mpltName);
    if (!file.is_open()) {
      ERR(kLncErr, "unable to open mplt file %s", mpltName.c_str());
      return false;
    }
    MIRParser parser(module);
    if (!parser.ParseMPLTStandalone(file, mpltName)) {
      ERR(kLncErr, "Failed to parser mplt file %s\n%s", mpltName.c_str(), parser.GetError().c_str());
      file.close();
      return false;
    }
    file.close();
  }
  UpdateStructNameTypeMapFromTypeTable(mpltName, flag);
  UpdateNameFuncMapFromTypeTable();
  return true;
}

void FETypeManager::UpdateStructNameTypeMapFromTypeTable(const std::string &mpltName, FETypeFlag flag) {
  bool sameNameUseLastest = sameNamePolicy.IsUseLastest();
  GStrIdx mpltNameIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(mpltName);
  for (MIRType *type : GlobalTables::GetTypeTable().GetTypeTable()) {
    if ((type != nullptr) && IsStructType(*type)) {
      MIRStructType *structType = static_cast<MIRStructType*>(type);
      auto it = structNameTypeMap.insert(std::make_pair(structType->GetNameStrIdx(), std::make_pair(structType, flag)));
      if (it.second == false) {
        // type is existed
        structSameNameSrcList.push_back(std::make_pair(structType->GetNameStrIdx(),
                                                       structNameSrcMap[structType->GetNameStrIdx()]));
        structSameNameSrcList.push_back(std::make_pair(structType->GetNameStrIdx(), mpltNameIdx));
        if (sameNameUseLastest) {
          structNameTypeMap[structType->GetNameStrIdx()] = std::make_pair(structType, flag);
          structNameSrcMap[structType->GetNameStrIdx()] = mpltNameIdx;
        }
      } else {
        // type is not existed
        structNameSrcMap[structType->GetNameStrIdx()] = mpltNameIdx;
      }
    }
  }
}

void FETypeManager::UpdateNameFuncMapFromTypeTable() {
  for (uint32 i = 1; i < GlobalTables::GetGsymTable().GetSymbolTableSize(); i++) {
    MIRSymbol *symbol = GlobalTables::GetGsymTable().GetSymbol(i);
    CHECK_FATAL(symbol, "Symbol is null");
    if (symbol->GetSKind() == kStFunc) {
      CHECK_FATAL(symbol->GetFunction(), "Function in symbol is null");
      MIRFunction *func = symbol->GetFunction();
      if (func->GetAttr(FUNCATTR_static)) {
        mpltNameStaticFuncMap[symbol->GetNameStrIdx()] = symbol->GetFunction();
      } else {
        mpltNameFuncMap[symbol->GetNameStrIdx()] = symbol->GetFunction();
      }
    }
  }
}

void FETypeManager::CheckSameNamePolicy() const {
  std::unordered_map<GStrIdx, std::list<GStrIdx>, GStrIdxHash> result;
  for (const std::pair<GStrIdx, GStrIdx> &item : structSameNameSrcList) {
    result[item.first].push_back(item.second);
  }
  if (result.size() > 0) {
    WARN(kLncWarn, "========== Structs list with the same name ==========");
  }
  for (const std::pair<GStrIdx, std::list<GStrIdx>> &item : result) {
    std::string typeName = GlobalTables::GetStrTable().GetStringFromStrIdx(item.first);
    WARN(kLncWarn, "Type: %s", typeName.c_str());
    for (const GStrIdx &mpltNameIdx : item.second) {
      std::string mpltName = GlobalTables::GetStrTable().GetStringFromStrIdx(mpltNameIdx);
      WARN(kLncWarn, "  Defined in %s", mpltName.c_str());
    }
  }
  if (result.size() > 0 && sameNamePolicy.IsFatal()) {
    FATAL(kLncFatal, "Structs with the same name exsited. Exit.");
  }
}

MIRStructType *FETypeManager::GetClassOrInterfaceType(const GStrIdx &nameIdx) const {
  auto it = structNameTypeMap.find(nameIdx);
  if (it == structNameTypeMap.end()) {
    return nullptr;
  } else {
    return it->second.first;
  }
}

FETypeFlag FETypeManager::GetClassOrInterfaceTypeFlag(const GStrIdx &nameIdx) const {
  auto it = structNameTypeMap.find(nameIdx);
  if (it == structNameTypeMap.end()) {
    return FETypeFlag::kDefault;
  } else {
    return it->second.second;
  }
}

MIRStructType *FETypeManager::CreateClassOrInterfaceType(const GStrIdx &nameIdx, bool isInterface,
                                                         FETypeFlag typeFlag) {
  MIRType *type = nullptr;
  std::string name = GlobalTables::GetStrTable().GetStringFromStrIdx(nameIdx);
  if (isInterface) {
    type = GlobalTables::GetTypeTable().GetOrCreateInterfaceType(name.c_str(), module);
  } else {
    type = GlobalTables::GetTypeTable().GetOrCreateClassType(name.c_str(), module);
  }
  CHECK_NULL_FATAL(type);
  ASSERT(IsStructType(*type), "type is not struct type");
  MIRStructType *structType = static_cast<MIRStructType*>(type);
  structNameTypeMap[nameIdx] = std::make_pair(structType, typeFlag);
  return structType;
}

MIRStructType *FETypeManager::GetOrCreateClassOrInterfaceType(const GStrIdx &nameIdx, bool isInterface,
                                                              FETypeFlag typeFlag, bool &isCreate) {
  // same name policy: mpltSys > dex > mpltApk > mplt
  auto it = structNameTypeMap.find(nameIdx);
  if (it != structNameTypeMap.end()) {
    // type is existed
    uint16 flagExist = it->second.second & FETypeFlag::kSrcMask;
    uint16 flagNew = typeFlag & FETypeFlag::kSrcMask;
    if (flagNew > flagExist) {
      isCreate = false;
      return it->second.first;
    }
  }
  MIRStructType *structType = CreateClassOrInterfaceType(nameIdx, isInterface, typeFlag);
  isCreate = true;
  CHECK_NULL_FATAL(structType);
  return structType;
}

MIRType *FETypeManager::GetOrCreateClassOrInterfacePtrType(const GStrIdx &nameIdx, bool isInterface,
                                                           FETypeFlag typeFlag, bool &isCreate) {
  MIRStructType *structType = GetOrCreateClassOrInterfaceType(nameIdx, isInterface, typeFlag, isCreate);
  MIRType *ptrType = GetOrCreatePointerType(*structType);
  CHECK_NULL_FATAL(ptrType);
  return ptrType;
}

MIRStructType *FETypeManager::GetStructTypeFromName(const std::string &name) {
  GStrIdx nameIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(name);
  return GetStructTypeFromName(nameIdx);
}

MIRStructType *FETypeManager::GetStructTypeFromName(const GStrIdx &nameIdx) {
  auto it = structNameTypeMap.find(nameIdx);
  if (it == structNameTypeMap.end()) {
    return nullptr;
  } else {
    return it->second.first;
  }
}

MIRType *FETypeManager::GetOrCreateTypeFromName(const std::string &name, FETypeFlag typeFlag, bool usePtr) {
  CHECK_FATAL(!name.empty(), "type name is empty");
  PrimType pty = GetPrimType(name);
  if (pty != kPtyInvalid) {
    return GlobalTables::GetTypeTable().GetTypeTable().at(pty);
  }
  switch (name[0]) {
    case 'L': {
      bool isCreate = false;
      MIRStructType *structType = GetOrCreateClassOrInterfaceType(name, false, typeFlag, isCreate);
      if (usePtr) {
        return GetOrCreatePointerType(*structType);
      } else {
        return structType;
      }
    }
    case 'A': {
      uint32 dim = 0;
      bool isCreate = false;
      std::string elemTypeName = GetBaseTypeName(name, dim, true);
      MIRType *elemType = GetMIRTypeForPrim(elemTypeName);
      if (elemType == nullptr) {
        elemType = GetOrCreateClassOrInterfaceType(elemTypeName, false, typeFlag, isCreate);
        CHECK_NULL_FATAL(elemType);
        elemType = GetOrCreatePointerType(*elemType);
      }
      CHECK_FATAL(dim <= UINT8_MAX, "array dimension (%u) is out of range", dim);
      MIRType *type = GetOrCreateArrayType(*elemType, static_cast<uint8>(dim));
      ASSERT(type != nullptr, "Array type is null");
      if (usePtr) {
        return GetOrCreatePointerType(*type);
      } else {
        return type;
      }
    }
    default:
      MIRType *type = GetMIRTypeForPrim(name[0]);
      CHECK_FATAL(type, "Unresolved type %s", name.c_str());
      return type;
  }
}

MIRType *FETypeManager::GetOrCreatePointerType(const MIRType &type, PrimType ptyPtr) {
  MIRType *retType = GlobalTables::GetTypeTable().GetOrCreatePointerType(type, ptyPtr);
  CHECK_NULL_FATAL(retType);
  return retType;
}

MIRType *FETypeManager::GetOrCreateArrayType(MIRType &elemType, uint8 dim, PrimType ptyPtr) {
  switch (srcLang) {
    case kSrcLangJava:
      return GetOrCreateJArrayType(elemType, dim, ptyPtr);
    default:
      CHECK_FATAL(false, "unsupported src lang: %d", srcLang);
      return nullptr;
  }
}

MIRType *FETypeManager::GetOrCreateJArrayType(MIRType &elem, uint8 dim, PrimType ptyPtr) {
  MIRType *type = &elem;
  for (uint8 i = 0; i < dim; i++) {
    type = GlobalTables::GetTypeTable().GetOrCreateJarrayType(*type);
    CHECK_NULL_FATAL(type);
    if (i != dim - 1) {
      type = GetOrCreatePointerType(*type, ptyPtr);
      CHECK_NULL_FATAL(type);
    }
  }
  CHECK_NULL_FATAL(type);
  return type;
}

void FETypeManager::AddClassToModule(const MIRStructType &structType) {
  module.AddClass(structType.GetTypeIndex());
}

FEStructElemInfo *FETypeManager::RegisterStructFieldInfo(const GStrIdx &fullNameIdx, MIRSrcLang argSrcLang,
                                                         bool isStatic) {
  FEStructElemInfo *ptrInfo = GetStructElemInfo(fullNameIdx);
  if (ptrInfo != nullptr) {
    return ptrInfo;
  }
  UniqueFEStructElemInfo info = std::make_unique<FEStructFieldInfo>(fullNameIdx, argSrcLang, isStatic);
  ptrInfo = info.get();
  listStructElemInfo.push_back(std::move(info));
  CHECK_FATAL(mapStructElemInfo.insert(std::make_pair(fullNameIdx, ptrInfo)).second == true,
              "register struct elem info failed");
  return ptrInfo;
}

FEStructElemInfo *FETypeManager::RegisterStructMethodInfo(const GStrIdx &fullNameIdx, MIRSrcLang argSrcLang,
                                                          bool isStatic) {
  FEStructElemInfo *ptrInfo = GetStructElemInfo(fullNameIdx);
  if (ptrInfo != nullptr) {
    return ptrInfo;
  }
  UniqueFEStructElemInfo info = std::make_unique<FEStructMethodInfo>(fullNameIdx, argSrcLang, isStatic);
  ptrInfo = info.get();
  listStructElemInfo.push_back(std::move(info));
  CHECK_FATAL(mapStructElemInfo.insert(std::make_pair(fullNameIdx, ptrInfo)).second == true,
              "register struct elem info failed");
  return ptrInfo;
}

FEStructElemInfo *FETypeManager::GetStructElemInfo(const GStrIdx &fullNameIdx) const {
  auto it = mapStructElemInfo.find(fullNameIdx);
  if (it == mapStructElemInfo.end()) {
    return nullptr;
  }
  return it->second;
}

MIRFunction *FETypeManager::GetMIRFunction(const std::string &classMethodName, bool isStatic){
  GStrIdx nameIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(classMethodName);
  return GetMIRFunction(nameIdx, isStatic);
}

MIRFunction *FETypeManager::GetMIRFunction(const GStrIdx &nameIdx, bool isStatic) {
  const std::unordered_map<GStrIdx, MIRFunction*, GStrIdxHash> &funcMap = isStatic ? nameStaticFuncMap : nameFuncMap;
  auto it = funcMap.find(nameIdx);
  if (it != funcMap.end()) {
    return it->second;
  }
  const std::unordered_map<GStrIdx, MIRFunction*, GStrIdxHash> &mpltFuncMap = isStatic ? mpltNameStaticFuncMap :
                                                                                         mpltNameFuncMap;
  auto it2 = mpltFuncMap.find(nameIdx);
  if (it2 != mpltFuncMap.end()) {
    return it2->second;
  }
  return nullptr;
}

MIRFunction *FETypeManager::CreateFunction(const GStrIdx &nameIdx, const TyIdx &retTypeIdx,
                                           const std::vector<TyIdx> &argsTypeIdx, bool isVarg, bool isStatic) {
  MPLFE_PARALLEL_FORBIDDEN();
  MIRFunction *mirFunc = GetMIRFunction(nameIdx, isStatic);
  if (mirFunc != nullptr) {
    return mirFunc;
  }
  MIRSymbol *funcSymbol = GlobalTables::GetGsymTable().CreateSymbol(kScopeGlobal);
  funcSymbol->SetNameStrIdx(nameIdx);
  bool added = GlobalTables::GetGsymTable().AddToStringSymbolMap(*funcSymbol);
  if (!added) {
    mirFunc = funcSymbol->GetFunction();
    if (mirFunc != nullptr) {
      return mirFunc;
    }
  }
  funcSymbol->SetStorageClass(kScText);
  funcSymbol->SetSKind(kStFunc);
  MemPool *mpModule = module.GetMemPool();
  ASSERT(mpModule, "mem pool is nullptr");
  mirFunc = mpModule->New<MIRFunction>(&module, funcSymbol->GetStIdx());
  mirFunc->Init();
  size_t idx = GlobalTables::GetFunctionTable().GetFuncTable().size();
  CHECK_FATAL(idx < UINT32_MAX, "PUIdx is out of range");
  mirFunc->SetPuidx(static_cast<uint32>(idx));
  mirFunc->SetPuidxOrigin(mirFunc->GetPuidx());
  GlobalTables::GetFunctionTable().GetFuncTable().push_back(mirFunc);
  std::vector<TypeAttrs> argsAttr;
  for (uint32_t i = 0; i < argsTypeIdx.size(); i++) {
    argsAttr.emplace_back(TypeAttrs());
  }
  mirFunc->SetBaseClassFuncNames(nameIdx);
  funcSymbol->SetTyIdx(GlobalTables::GetTypeTable().GetOrCreateFunctionType(module, retTypeIdx,
                                                                            argsTypeIdx, argsAttr,
                                                                            isVarg)->GetTypeIndex());
  funcSymbol->SetFunction(mirFunc);
  MIRFuncType *functype = static_cast<MIRFuncType*>(funcSymbol->GetType());
  mirFunc->SetMIRFuncType(functype);
  mirFunc->SetReturnTyIdx(retTypeIdx);
  if (isStatic) {
    CHECK_FATAL(nameStaticFuncMap.insert(std::make_pair(nameIdx, mirFunc)).second, "nameStaticFuncMap insert failed");
  } else {
    CHECK_FATAL(nameFuncMap.insert(std::make_pair(nameIdx, mirFunc)).second, "nameFuncMap insert failed");
  }
  return mirFunc;
}

MIRFunction *FETypeManager::CreateFunction(const std::string &methodName, const std::string &returnTypeName,
                                           const std::vector<std::string> &argTypeNames, bool isVarg, bool isStatic) {
  MPLFE_PARALLEL_FORBIDDEN();
  GStrIdx nameIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(methodName);
  MIRFunction *mirFunc = GetMIRFunction(nameIdx, isStatic);
  if (mirFunc != nullptr) {
    return mirFunc;
  }
  MIRType *returnType = GetOrCreateTypeFromName(returnTypeName, FETypeFlag::kSrcUnknown, true);
  std::vector<TyIdx> argsTypeIdx;
  for (const std::string &typeName : argTypeNames) {
    MIRType *argType = GetOrCreateTypeFromName(typeName, FETypeFlag::kSrcUnknown, true);
    argsTypeIdx.push_back(argType->GetTypeIndex());
  }
  return CreateFunction(nameIdx, returnType->GetTypeIndex(), argsTypeIdx, isVarg, isStatic);
}

bool FETypeManager::IsAntiProguardFieldStruct(const GStrIdx &structNameIdx) {
  return setAntiProguardFieldStructIdx.find(structNameIdx) != setAntiProguardFieldStructIdx.end();
}

bool FETypeManager::IsStructType(const MIRType &type) {
  MIRTypeKind kind = type.GetKind();
  return kind == kTypeStruct || kind == kTypeStructIncomplete ||
         kind == kTypeClass || kind == kTypeClassIncomplete ||
         kind == kTypeInterface || kind == kTypeInterfaceIncomplete;
}

PrimType FETypeManager::GetPrimType(const std::string &name) {
#define LOAD_ALGO_PRIMARY_TYPE
#define PRIMTYPE(P)            \
  if (name.compare(#P) == 0) { \
    return PTY_##P;            \
  }
#include "prim_types.def"
#undef PRIMTYPE
  return kPtyInvalid;
}

MIRType *FETypeManager::GetMIRTypeForPrim(char c) {
  switch (c) {
    case 'B':
      return GlobalTables::GetTypeTable().GetInt8();
    case 'C':
      return GlobalTables::GetTypeTable().GetUInt16();
    case 'S':
      return GlobalTables::GetTypeTable().GetInt16();
    case 'Z':
      return GlobalTables::GetTypeTable().GetUInt1();
    case 'I':
      return GlobalTables::GetTypeTable().GetInt32();
    case 'J':
      return GlobalTables::GetTypeTable().GetInt64();
    case 'F':
      return GlobalTables::GetTypeTable().GetFloat();
    case 'D':
      return GlobalTables::GetTypeTable().GetDouble();
    case 'V':
      return GlobalTables::GetTypeTable().GetVoid();
    case 'R':
      return GlobalTables::GetTypeTable().GetRef();
    default:
      return nullptr;
  }
}

std::string FETypeManager::GetBaseTypeName(const std::string &name, uint32 &dim, bool inMpl) {
  dim = 0;
  char prefix = inMpl ? 'A' : '[';
  while (name[dim] == prefix) {
    dim++;
  }
  return name.substr(dim);
}

void FETypeManager::SetComplete(MIRStructType &structType) {
  switch (structType.GetKind()) {
    case kTypeClassIncomplete:
      structType.SetMIRTypeKind(kTypeClass);
      break;
    case kTypeInterfaceIncomplete:
      structType.SetMIRTypeKind(kTypeInterface);
      break;
    case kTypeStructIncomplete:
      structType.SetMIRTypeKind(kTypeStruct);
      break;
    default:
      break;
  }
}

std::string FETypeManager::TypeAttrsToString(const TypeAttrs &attrs) {
  std::stringstream ss;
#define TYPE_ATTR
#define ATTR(A)                \
  if (attrs.GetAttr(ATTR_##A)) \
    ss << " " << #A;
#include "all_attributes.def"
#undef ATTR
#undef TYPE_ATTR
  ss << " ";
  return ss.str();
}

void FETypeManager::InitMCCFunctions() {
  InitFuncMCCGetOrInsertLiteral();
}

void FETypeManager::InitFuncMCCGetOrInsertLiteral() {
  std::string funcName = "MCC_GetOrInsertLiteral";
  GStrIdx nameIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(funcName);
  MIRType *typeString = kFEIRTypeJavaString->GenerateMIRTypeAuto(kSrcLangJava);
  std::vector<TyIdx> argsType;
  funcMCCGetOrInsertLiteral = CreateFunction(nameIdx, typeString->GetTypeIndex(), argsType, false, false);
  nameMCCFuncMap[nameIdx] = funcMCCGetOrInsertLiteral;
}

MIRFunction *FETypeManager::GetMCCFunction(const std::string &funcName) const {
  GStrIdx funcNameIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(funcName);
  return GetMCCFunction(funcNameIdx);
}

MIRFunction *FETypeManager::GetMCCFunction(const GStrIdx &funcNameIdx) const {
  auto it = nameMCCFuncMap.find(funcNameIdx);
  if (it == nameMCCFuncMap.end()) {
    return nullptr;
  } else {
    return it->second;
  }
}
}  // namespace maple