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
#include "fe_struct_elem_info.h"
#include "global_tables.h"
#include "mpl_logging.h"
#include "namemangler.h"
#include "fe_utils.h"
#include "fe_manager.h"
#include "jbc_util.h"

namespace maple {
// ---------- FEStructElemInfo ----------
FEStructElemInfo::FEStructElemInfo(const GStrIdx &argFullNameIdx, MIRSrcLang argSrcLang, bool argIsStatic)
    : fullNameIdx(argFullNameIdx),
      srcLang(argSrcLang),
      isStatic(argIsStatic),
      isMethod(false),
      isDefined(false),
      isFromDex(false),
      isPrepared(false),
      structNameIdx(0) {
      // fieldNameIdx(0),
      // typeNameIdx(0),
      // fieldID(0) {
  Init();
}

void FEStructElemInfo::Init() {
  switch (srcLang) {
    case kSrcLangJava:
      InitJava();
      break;
    default:
      WARN(kLncWarn, "unsupported language");
      break;
  }
}

void FEStructElemInfo::InitJava() {
  std::string fullNameMpl = GlobalTables::GetStrTable().GetStringFromStrIdx(fullNameIdx);
  std::string fullNameJava = NameMangler::DecodeName(fullNameMpl);
  std::vector<std::string> names = FEUtils::Split(fullNameJava, '|');
  // 3 parts: ClassName|ElemName|Signature
  CHECK_FATAL(names.size() == 3, "invalid elem name %s", fullNameJava.c_str());
  // names[0]: structName
  structNameIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(NameMangler::EncodeName(names[0]));
  // names[1]: elemName
  elemNameIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(NameMangler::EncodeName(names[1]));
  // names[2]: signature
  signatureNameIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(NameMangler::EncodeName(names[2]));
}

// ---------- FEStructFieldInfo ----------
FEStructFieldInfo::FEStructFieldInfo(const GStrIdx &argFullNameIdx, MIRSrcLang argSrcLang, bool argIsStatic)
    : FEStructElemInfo(argFullNameIdx, argSrcLang, argIsStatic),
      fieldType(nullptr),
      fieldNameIdx(0),
      fieldID(0) {
  isMethod = false;
  LoadFieldType();
}

void FEStructFieldInfo::PrepareImpl(MIRBuilder &mirBuilder, bool argIsStatic) {
  if (isPrepared && argIsStatic == isStatic) {
    return;
  }
  // Prepare
  const std::string &structName = GetStructName();
  std::string rawName = structName + NameMangler::kNameSplitterStr + GetElemName();
  fieldNameIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(rawName);
  fieldID = UINT32_MAX;
  MIRStructType *structType = FEManager::GetTypeManager().GetStructTypeFromName(structName);
  if (structType == nullptr) {
    isDefined = false;
    isPrepared = true;
    return;
  }
  isDefined = SearchStructFieldJava(*structType, mirBuilder, argIsStatic);
  if (isDefined) {
    return;
  }
  rawName = rawName + NameMangler::kNameSplitterStr + GetSignatureName();
  WARN(kLncErr, "use undefined %s field %s", argIsStatic ? "static" : "", rawName.c_str());
  isPrepared = true;
  isStatic = argIsStatic;
  return;
}

void FEStructFieldInfo::LoadFieldType() {
  switch (srcLang) {
    case kSrcLangJava:
      LoadFieldTypeJava();
      break;
    default:
      WARN(kLncWarn, "unsupported language");
      break;
  }
}

void FEStructFieldInfo::LoadFieldTypeJava() {
  fieldType = std::make_unique<FEIRTypeDefault>(PTY_unknown);
  static_cast<FEIRTypeDefault*>(fieldType.get())->LoadFromJavaTypeName(GetSignatureName(), true);
}

void FEStructFieldInfo::PrepareStaticField(const MIRStructType &structType) {
  std::string ownerStructName = structType.GetName();
  const std::string &fieldName = GetElemName();
  std::string fullName = ownerStructName + NameMangler::kNameSplitterStr + fieldName;
  fieldNameIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(fullName);
  isPrepared = true;
  isStatic = true;
}

void FEStructFieldInfo::PrepareNonStaticField(MIRStructType &structType, MIRBuilder &mirBuilder) {
  FEIRTypeDefault feType(PTY_unknown);
  feType.LoadFromJavaTypeName(GetSignatureName(), true);
  MIRType *fieldMIRType = feType.GenerateMIRTypeAuto(srcLang);
  uint32 idx = 0;
  uint32 idx1 = 0;
  mirBuilder.TraverseToNamedFieldWithType(structType, elemNameIdx, fieldMIRType->GetTypeIndex(), idx1, idx);
  fieldID = idx;
  isPrepared = true;
  isStatic = false;
}

bool FEStructFieldInfo::SearchStructFieldJava(MIRStructType &structType, MIRBuilder &mirBuilder, bool argIsStatic,
                                              bool allowPrivate) {
  if (structType.IsIncomplete()) {
    return false;
  }
  GStrIdx nameIdx = elemNameIdx;
  if (argIsStatic) {
    // suppose anti-proguard is off
    std::string fullName = structType.GetCompactMplTypeName() + NameMangler::kNameSplitterStr + GetElemName();
    nameIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(fullName);
  }
  const FieldVector &fields = argIsStatic ? structType.GetStaticFields() : structType.GetFields();
  for (const FieldPair &fieldPair : fields) {
    if (fieldPair.first != nameIdx) {
      continue;
    }
    if (fieldPair.second.second.GetAttr(FLDATTR_private) && !allowPrivate) {
      continue;
    }
    if (CompareFieldType(fieldPair)) {
      if (argIsStatic) {
        PrepareStaticField(structType);
      } else {
        PrepareNonStaticField(structType, mirBuilder);
      }
      return true;
    }
  }
  // search parent
  bool found = false;
  if (structType.GetKind() == kTypeClass) {
    // parent
    MIRClassType &classType = static_cast<MIRClassType&>(structType);
    found = SearchStructFieldJava(classType.GetParentTyIdx(), mirBuilder, argIsStatic, false);
    // implemented
    for (const TyIdx &tyIdx : classType.GetInterfaceImplemented()) {
      found = found || SearchStructFieldJava(tyIdx, mirBuilder, argIsStatic, false);
    }
  } else if (structType.GetKind() == kTypeInterface) {
    // parent
    MIRInterfaceType &interfaceType = static_cast<MIRInterfaceType&>(structType);
    for (const TyIdx &tyIdx : interfaceType.GetParentsTyIdx()) {
      found = found || SearchStructFieldJava(tyIdx, mirBuilder, argIsStatic, false);
    }
  } else {
    CHECK_FATAL(false, "not supported yet");
  }
  return found;
}

bool FEStructFieldInfo::SearchStructFieldJava(const TyIdx &tyIdx, MIRBuilder &mirBuilder, bool argIsStatic,
                                              bool allowPrivate) {
  MIRType *type = GlobalTables::GetTypeTable().GetTypeFromTyIdx(tyIdx);
  if (type == nullptr) {
    return false;
  }
  if (type->IsIncomplete()) {
    return false;
  }
  if (type->GetKind() == kTypeClass || type->GetKind() == kTypeInterface) {
    MIRStructType *structType = static_cast<MIRStructType*>(type);
    return SearchStructFieldJava(*structType, mirBuilder, argIsStatic, allowPrivate);
  } else {
    ERR(kLncErr, "parent type should be StructType");
    return false;
  }
}

bool FEStructFieldInfo::CompareFieldType(const FieldPair &fieldPair) const {
  MIRType *fieldMIRType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(fieldPair.second.first);
  std::string typeName = fieldMIRType->GetCompactMplTypeName();
  if (GetSignatureName().compare(typeName) == 0) {
    return true;
  } else {
    return false;
  }
}

// ---------- FEStructMethodInfo ----------
std::map<GStrIdx, std::set<GStrIdx>> FEStructMethodInfo::javaPolymorphicWhiteList;

FEStructMethodInfo::FEStructMethodInfo(const GStrIdx &argFullNameIdx, MIRSrcLang argSrcLang, bool argIsStatic)
    : FEStructElemInfo(argFullNameIdx, argSrcLang, argIsStatic),
      methodNameIdx(fullNameIdx),
      mirFunc(nullptr),
      isReturnVoid(false),
      isJavaPolymorphicCall(false),
      isJavaDynamicCall(false) {
  isMethod = true;
  LoadMethodType();
}

void FEStructMethodInfo::InitJavaPolymorphicWhiteList() {
  MPLFE_PARALLEL_FORBIDDEN();
  std::map<GStrIdx, std::set<GStrIdx>> &ans = javaPolymorphicWhiteList;
  StringTable<std::string, GStrIdx> &strTable = GlobalTables::GetStrTable();
  GStrIdx idxMethodHandle =
      strTable.GetOrCreateStrIdxFromName(NameMangler::EncodeName("Ljava/lang/invoke/MethodHandle;"));
  bool success = true;
  success = success && ans[idxMethodHandle].insert(strTable.GetOrCreateStrIdxFromName("invoke")).second;
  success = success && ans[idxMethodHandle].insert(strTable.GetOrCreateStrIdxFromName("invokeBasic")).second;
  success = success && ans[idxMethodHandle].insert(strTable.GetOrCreateStrIdxFromName("invokeExact")).second;
  CHECK_FATAL(success, "error occurs");
}

PUIdx FEStructMethodInfo::GetPuIdx() const {
  CHECK_NULL_FATAL(mirFunc);
  return mirFunc->GetPuidx();
}

void FEStructMethodInfo::PrepareImpl(MIRBuilder &mirBuilder, bool argIsStatic) {
  if (isPrepared && argIsStatic == isStatic) {
    return;
  }
  switch (srcLang) {
    case kSrcLangJava:
      PrepareImplJava(mirBuilder, argIsStatic);
      break;
    default:
      CHECK_FATAL(false, "unsupported src lang");
  }
  PrepareMethod();
}

void FEStructMethodInfo::PrepareImplJava(MIRBuilder &mirBuilder, bool argIsStatic) {
  // Prepare
  const std::string &structName = GetStructName();
  MIRStructType *structType = nullptr;
  if (!structName.empty() && structName[0] == 'A') {
    structType = FEManager::GetTypeManager().GetStructTypeFromName("Ljava_2Flang_2FObject_3B");
  } else {
    structType = FEManager::GetTypeManager().GetStructTypeFromName(structName);
  }
  isStatic = argIsStatic;
  isDefined = false;
  if (structType != nullptr) {
    isDefined = SearchStructMethodJava(*structType, mirBuilder, argIsStatic);
    if (isDefined) {
      return;
    }
  } else if (isJavaDynamicCall) {
    methodNameIdx = fullNameIdx;
    isDefined = true;
    PrepareMethod();
    return;
  }
  std::string methodName = GlobalTables::GetStrTable().GetStringFromStrIdx(fullNameIdx);
  WARN(kLncWarn, "undefined %s method: %s", isStatic ? "static" : "", methodName.c_str());
}

void FEStructMethodInfo::LoadMethodType() {
  switch (srcLang) {
    case kSrcLangJava:
      LoadMethodTypeJava();
      break;
    default:
      WARN(kLncWarn, "unsupported language");
      break;
  }
}

void FEStructMethodInfo::LoadMethodTypeJava() {
  std::string signatureJava =
      NameMangler::DecodeName(GlobalTables::GetStrTable().GetStringFromStrIdx(fullNameIdx));
  std::vector<std::string> typeNames = jbc::JBCUtil::SolveMethodSignature(signatureJava);
  CHECK_FATAL(typeNames.size() > 0, "invalid method signature: %s", signatureJava.c_str());
  // constructor check
  const std::string &funcName = GetElemName();
  isConstructor = (funcName.find("init_28") == 0);
  // return type
  retType = std::make_unique<FEIRTypeDefault>(PTY_unknown);
  if (typeNames[0].compare("V") == 0) {
    isReturnVoid = true;
  }
  static_cast<FEIRTypeDefault*>(retType.get())->LoadFromJavaTypeName(typeNames[0], false);
  // argument types
  argTypes.clear();
  for (size_t i = 1; i < typeNames.size(); i++) {
    UniqueFEIRType argType = std::make_unique<FEIRTypeDefault>(PTY_unknown);
    static_cast<FEIRTypeDefault*>(argType.get())->LoadFromJavaTypeName(typeNames[i], false);
    argTypes.push_back(std::move(argType));
  }
  // owner type
  ownerType = std::make_unique<FEIRTypeDefault>(PTY_unknown);
  static_cast<FEIRTypeDefault*>(ownerType.get())->LoadFromJavaTypeName(GetStructName(), true);
}

void FEStructMethodInfo::PrepareMethod() {
  mirFunc = FEManager::GetTypeManager().GetMIRFunction(methodNameIdx, isStatic);
  if (mirFunc == nullptr) {
    MIRType *mirRetType = retType->GenerateMIRTypeAuto(srcLang);
    // args type
    std::vector<TyIdx> argsTypeIdx;
    for (const UniqueFEIRType &argType : argTypes) {
      MIRType *mirArgType = argType->GenerateMIRTypeAuto(srcLang);
      argsTypeIdx.push_back(mirArgType->GetTypeIndex());
    }
    mirFunc = FEManager::GetTypeManager().CreateFunction(methodNameIdx, mirRetType->GetTypeIndex(), argsTypeIdx, false,
                                                         isStatic);
  }
  isPrepared = true;
}

bool FEStructMethodInfo::SearchStructMethodJava(MIRStructType &structType, MIRBuilder &mirBuilder, bool argIsStatic,
                                                bool allowPrivate) {
  if (structType.IsIncomplete()) {
    return false;
  }
  std::string fullName = structType.GetCompactMplTypeName() + NameMangler::kNameSplitterStr + GetElemName() +
                         NameMangler::kNameSplitterStr + GetSignatureName();
  GStrIdx nameIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(fullName);
  // PolymorphicCall Check
  if (CheckJavaPolymorphicCall()) {
    isJavaPolymorphicCall = true;
    methodNameIdx = nameIdx;
    PrepareMethod();
    return true;
  }
  for (const MethodPair &methodPair : structType.GetMethods()) {
    const MIRSymbol *sym = GlobalTables::GetGsymTable().GetSymbolFromStidx(methodPair.first.Idx(), true);
    CHECK_NULL_FATAL(sym);
    if (methodPair.second.second.GetAttr(FUNCATTR_private) && !allowPrivate) {
      continue;
    }
    if (methodPair.second.second.GetAttr(FUNCATTR_static) != argIsStatic) {
      continue;
    }
    if (sym->GetNameStrIdx() == nameIdx) {
      isStatic = argIsStatic;
      if (isStatic) {
        methodNameIdx = nameIdx;
      }
      PrepareMethod();
      return true;
    }
  }
  // search parent
  return SearchStructMethodJavaInParent(structType, mirBuilder, argIsStatic);
}

bool FEStructMethodInfo::SearchStructMethodJavaInParent(MIRStructType &structType, MIRBuilder &mirBuilder,
                                                        bool argIsStatic) {
  bool found = false;
  if (structType.GetKind() == kTypeClass) {
    // parent
    MIRClassType &classType = static_cast<MIRClassType&>(structType);
    found = SearchStructMethodJava(classType.GetParentTyIdx(), mirBuilder, argIsStatic, false);
    // implemented
    for (const TyIdx &tyIdx : classType.GetInterfaceImplemented()) {
      found = found || SearchStructMethodJava(tyIdx, mirBuilder, argIsStatic, false);
    }
  } else if (structType.GetKind() == kTypeInterface) {
    // parent
    MIRInterfaceType &interfaceType = static_cast<MIRInterfaceType&>(structType);
    for (const TyIdx &tyIdx : interfaceType.GetParentsTyIdx()) {
      found = found || SearchStructMethodJava(tyIdx, mirBuilder, argIsStatic, false);
    }
  } else {
    CHECK_FATAL(false, "not supported yet");
  }
  return found;
}

bool FEStructMethodInfo::SearchStructMethodJava(const TyIdx &tyIdx, MIRBuilder &mirBuilder, bool argIsStatic,
                                                bool allowPrivate) {
  MIRType *type = GlobalTables::GetTypeTable().GetTypeFromTyIdx(tyIdx);
  if (type == nullptr) {
    return false;
  }
  if (type->IsIncomplete()) {
    return false;
  }
  if (type->GetKind() == kTypeClass || type->GetKind() == kTypeInterface) {
    MIRStructType *structType = static_cast<MIRStructType*>(type);
    return SearchStructMethodJava(*structType, mirBuilder, argIsStatic, allowPrivate);
  } else {
    ERR(kLncErr, "parent type should be StructType");
    return false;
  }
}

bool FEStructMethodInfo::CheckJavaPolymorphicCall() const {
  auto it = javaPolymorphicWhiteList.find(structNameIdx);
  if (it == javaPolymorphicWhiteList.end()) {
    return false;
  }
  return it->second.find(elemNameIdx) != it->second.end();
}
}  // namespace maple
