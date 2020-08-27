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
#include "jbc_class2fe_helper.h"
#include "fe_configs.h"
#include "fe_options.h"
#include "fe_macros.h"
#include "fe_manager.h"
#include "fe_utils_java.h"

namespace maple {
// ---------- JBCClass2FEHelper ----------
JBCClass2FEHelper::JBCClass2FEHelper(MapleAllocator &allocator, const jbc::JBCClass &klassIn)
    : FEInputStructHelper(allocator),
      klass(klassIn),
      isStaticFieldProguard(false) {
  srcLang = kSrcLangJava;
}

// interface implements
std::string JBCClass2FEHelper::GetStructNameOrinImpl() const {
  return klass.GetClassNameOrin();
}

std::string JBCClass2FEHelper::GetStructNameMplImpl() const {
  return klass.GetClassNameMpl();
}

std::vector<std::string> JBCClass2FEHelper::GetSuperClassNamesImpl() const {
  std::string superName = klass.GetSuperClassName();
  return std::vector<std::string>({ superName });
}

std::vector<std::string> JBCClass2FEHelper::GetInterfaceNamesImpl() const {
  return klass.GetInterfaceNames();
}

std::string JBCClass2FEHelper::GetSourceFileNameImpl() const {
  return klass.GetSourceFileName();
}

MIRStructType *JBCClass2FEHelper::CreateMIRStructTypeImpl(bool &error) const {
  std::string classNameOrin = klass.GetClassNameOrin();
  std::string classNameMpl = klass.GetClassNameMpl();
  if (classNameMpl.empty()) {
    error = true;
    ERR(kLncErr, "class name is empty");
    return nullptr;
  }
  FE_INFO_LEVEL(FEOptions::kDumpLevelInfoDetail, "CreateMIRStrucType for %s", classNameOrin.c_str());
  bool isCreate = false;
  MIRStructType *type = FEManager::GetTypeManager().GetOrCreateClassOrInterfaceType(classNameMpl, klass.IsInterface(),
                                                                                    FETypeFlag::kSrcInput, isCreate);
  error = false;
  return isCreate ? type : nullptr;
}

TypeAttrs JBCClass2FEHelper::GetStructAttributeFromInputImpl() const {
  const uint8 bitOfUInt16 = 16;
  TypeAttrs attrs;
  uint16 klassAccessFlag = klass.GetAccessFlag();
  for (uint8 bit = 0; bit < bitOfUInt16; bit++) {
    uint16 flag = static_cast<uint16>(klassAccessFlag & (1u << bit));
    switch (flag) {
      case jbc::kAccClassPublic:
        attrs.SetAttr(ATTR_public);
        break;
      case jbc::kAccClassFinal:
        attrs.SetAttr(ATTR_final);
        break;
      case jbc::kAccClassSuper:
      case jbc::kAccClassInterface:
        break;
      case jbc::kAccClassAbstract:
        attrs.SetAttr(ATTR_abstract);
        break;
      case jbc::kAccClassSynthetic:
        attrs.SetAttr(ATTR_synthetic);
        break;
      case jbc::kAccClassAnnotation:
        attrs.SetAttr(ATTR_annotation);
        break;
      case jbc::kAccClassEnum:
        attrs.SetAttr(ATTR_enum);
        break;
      default:
        break;
    }
  }
  return attrs;
}

uint64 JBCClass2FEHelper::GetRawAccessFlagsImpl() const {
  return uint64{ klass.GetAccessFlag() };
}

void JBCClass2FEHelper::InitFieldHelpersImpl() {
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp != nullptr, "mem pool is nullptr");
  for (jbc::JBCClassField *field : klass.GetFields()) {
    ASSERT(field != nullptr, "field is nullptr");
    JBCClassField2FEHelper *fieldHelper = mp->New<JBCClassField2FEHelper>(allocator, *field);
    fieldHelpers.push_back(fieldHelper);
  }
}

void JBCClass2FEHelper::InitMethodHelpersImpl() {
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp != nullptr, "mem pool is nullptr");
  for (jbc::JBCClassMethod *method : klass.GetMethods()) {
    ASSERT(method != nullptr, "method is nullptr");
    JBCClassMethod2FEHelper *methodHelper = mp->New<JBCClassMethod2FEHelper>(allocator, *method);
    methodHelpers.push_back(methodHelper);
  }
}

std::string JBCClass2FEHelper::GetSrcFileNameImpl() const {
  return klass.GetFileName();
}

// ---------- JBCClassField2FEHelper ----------
bool JBCClassField2FEHelper::ProcessDeclImpl(MapleAllocator &allocator) {
  CHECK_FATAL(false, "should not run here");
  return false;
}

bool JBCClassField2FEHelper::ProcessDeclWithContainerImpl(MapleAllocator &allocator,
                                                          const FEInputStructHelper &structHelper) {
  ASSERT(structHelper.GetSrcLang() == kSrcLangJava, "input type check failed");
  const JBCClass2FEHelper &klassHelper = static_cast<const JBCClass2FEHelper&>(structHelper);
  const jbc::JBCConstPool &constPool = klassHelper.GetConstPool();
  std::string klassName = klassHelper.GetStructNameOrin();
  std::string fieldName = field.GetName(constPool);
  std::string typeName = field.GetDescription(constPool);
  if (fieldName.empty()) {
    ERR(kLncErr, "invalid name_index(%d) for field in class %s", field.GetNameIdx(), klassName.c_str());
    return false;
  }
  if (typeName.empty()) {
    ERR(kLncErr, "invalid descriptor_index(%d) for field in class %s", field.GetDescriptionIdx(), klassName.c_str());
    return false;
  }
  FEOptions::ModeJavaStaticFieldName modeStaticField = FEOptions::GetInstance().GetModeJavaStaticFieldName();
  bool withType = (modeStaticField == FEOptions::kAllType) ||
                  (modeStaticField == FEOptions::kSmart && klassHelper.IsStaticFieldProguard());
  std::string name = field.IsStatic() ? (klassName + "|") : "";
  name += fieldName;
  name += withType ? ("|" + typeName) : "";
  std::string fullName = klassName + "|" + fieldName + "|" + typeName;
  GStrIdx idx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(namemangler::EncodeName(name));
  GStrIdx fullNameIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(namemangler::EncodeName(fullName));
  FEStructElemInfo *elemInfo = FEManager::GetTypeManager().RegisterStructFieldInfo(fullNameIdx, kSrcLangJava,
                                                                                   field.IsStatic());
  elemInfo->SetDefined();
  elemInfo->SetFromDex();
  FieldAttrs attrs = AccessFlag2Attribute(field.GetAccessFlag());
  std::string typeNameMpl = namemangler::EncodeName(typeName);
  MIRType *fieldType = FEManager::GetTypeManager().GetOrCreateTypeFromName(typeNameMpl, FETypeFlag::kSrcUnknown, true);
  ASSERT(fieldType != nullptr, "nullptr check for fieldType");
  mirFieldPair.first = idx;
  mirFieldPair.second.first = fieldType->GetTypeIndex();
  mirFieldPair.second.second = attrs;
  return true;
}

FieldAttrs JBCClassField2FEHelper::AccessFlag2Attribute(uint16 accessFlag) {
  const uint32 bitOfUInt16 = 16;
  FieldAttrs attrs;
  for (uint32 bit = 0; bit < bitOfUInt16; bit++) {
    uint16 flag = static_cast<uint16>(accessFlag & (1u << bit));
    switch (flag) {
      case jbc::JBCClassFieldAccessFlag::kAccFieldPublic:
        attrs.SetAttr(FLDATTR_public);
        break;
      case jbc::JBCClassFieldAccessFlag::kAccFieldPrivate:
        attrs.SetAttr(FLDATTR_private);
        break;
      case jbc::JBCClassFieldAccessFlag::kAccFieldProtected:
        attrs.SetAttr(FLDATTR_protected);
        break;
      case jbc::JBCClassFieldAccessFlag::kAccFieldStatic:
        attrs.SetAttr(FLDATTR_static);
        break;
      case jbc::JBCClassFieldAccessFlag::kAccFieldFinal:
        attrs.SetAttr(FLDATTR_final);
        break;
      case jbc::JBCClassFieldAccessFlag::kAccFieldVolatile:
        attrs.SetAttr(FLDATTR_volatile);
        break;
      case jbc::JBCClassFieldAccessFlag::kAccFieldTransient:
        attrs.SetAttr(FLDATTR_transient);
        break;
      case jbc::JBCClassFieldAccessFlag::kAccFieldSynthetic:
        attrs.SetAttr(FLDATTR_synthetic);
        break;
      case jbc::JBCClassFieldAccessFlag::kAccFieldEnum:
        attrs.SetAttr(FLDATTR_enum);
        break;
      default:
        break;
    }
  }
  return attrs;
}

// ---------- JBCClassMethod2FEHelper ----------
JBCClassMethod2FEHelper::JBCClassMethod2FEHelper(MapleAllocator &allocator, const jbc::JBCClassMethod &methodIn)
    : FEInputMethodHelper(allocator),
      method(methodIn) {
  srcLang = kSrcLangJava;
}

bool JBCClassMethod2FEHelper::ProcessDeclImpl(MapleAllocator &allocator) {
  std::string methodName = GetMethodName(true);
  GStrIdx methodNameIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(methodName);
  FEStructElemInfo *elemInfo = FEManager::GetTypeManager().RegisterStructMethodInfo(methodNameIdx, kSrcLangJava,
                                                                                    IsStatic());
  elemInfo->SetDefined();
  elemInfo->SetFromDex();
  return FEInputMethodHelper::ProcessDeclImpl(allocator);
}

void JBCClassMethod2FEHelper::SolveReturnAndArgTypesImpl(MapleAllocator &allocator) {
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp != nullptr, "mempool is nullptr");
  std::string klassName = method.GetClassName();
  std::string methodName = GetMethodName(false);
  if (HasThis()) {
    FEIRTypeDefault *type = mp->New<FEIRTypeDefault>();
    type->LoadFromJavaTypeName(klassName, false);
    argTypes.push_back(type);
  }
  std::vector<std::string> returnAndArgTypeNames = FEUtilJava::SolveMethodSignature(methodName, false);
  bool first = true;
  for (const std::string &typeName : returnAndArgTypeNames) {
    FEIRTypeDefault *type = mp->New<FEIRTypeDefault>();
    type->LoadFromJavaTypeName(typeName, false);
    if (first) {
      retType = type;
      first = false;
    } else {
      argTypes.push_back(type);
    }
  }
}

std::string JBCClassMethod2FEHelper::GetMethodNameImpl(bool inMpl, bool full) const {
  const jbc::JBCConstPool &constPool = method.GetConstPool();
  std::string klassName = method.GetClassName();
  std::string methodName = method.GetName(constPool);
  if (!full) {
    return inMpl ? namemangler::EncodeName(methodName) : methodName;
  }
  std::string descName = method.GetDescription(constPool);
  std::string fullName = klassName + "|" + methodName + "|" + descName;
  return inMpl ? namemangler::EncodeName(fullName) : fullName;
}

FuncAttrs JBCClassMethod2FEHelper::GetAttrsImpl() const {
  FuncAttrs attrs;
  const uint32 bitOfUInt16 = 16;
  uint16 accessFlag = method.GetAccessFlag();
  for (uint32 bit = 0; bit < bitOfUInt16; bit++) {
    uint16 flag = static_cast<uint16>(accessFlag & (1u << bit));
    switch (flag) {
      case jbc::JBCClassMethodAccessFlag::kAccMethodPublic:
        attrs.SetAttr(FUNCATTR_public);
        break;
      case jbc::JBCClassMethodAccessFlag::kAccMethodPrivate:
        attrs.SetAttr(FUNCATTR_private);
        break;
      case jbc::JBCClassMethodAccessFlag::kAccMethodProtected:
        attrs.SetAttr(FUNCATTR_protected);
        break;
      case jbc::JBCClassMethodAccessFlag::kAccMethodStatic:
        attrs.SetAttr(FUNCATTR_static);
        break;
      case jbc::JBCClassMethodAccessFlag::kAccMethodFinal:
        attrs.SetAttr(FUNCATTR_final);
        break;
      case jbc::JBCClassMethodAccessFlag::kAccMethodSynchronized:
        attrs.SetAttr(FUNCATTR_synchronized);
        break;
      case jbc::JBCClassMethodAccessFlag::kAccMethodBridge:
        attrs.SetAttr(FUNCATTR_bridge);
        break;
      case jbc::JBCClassMethodAccessFlag::kAccMethodVarargs:
        attrs.SetAttr(FUNCATTR_varargs);
        break;
      case jbc::JBCClassMethodAccessFlag::kAccMethodNative:
        attrs.SetAttr(FUNCATTR_native);
        break;
      case jbc::JBCClassMethodAccessFlag::kAccMethodAbstract:
        attrs.SetAttr(FUNCATTR_abstract);
        break;
      case jbc::JBCClassMethodAccessFlag::kAccMethodStrict:
        attrs.SetAttr(FUNCATTR_strict);
        break;
      case jbc::JBCClassMethodAccessFlag::kAccMethodSynthetic:
        attrs.SetAttr(FUNCATTR_synthetic);
        break;
      default:
        break;
    }
  }
  return attrs;
}

bool JBCClassMethod2FEHelper::IsStaticImpl() const {
  uint16 accessFlag = method.GetAccessFlag();
  if ((accessFlag & jbc::JBCClassMethodAccessFlag::kAccMethodStatic) != 0) {
    return true;
  }
  if (IsClinit()) {
    return true;
  }
  return false;
}

bool JBCClassMethod2FEHelper::IsVargImpl() const {
  return false;
}

bool JBCClassMethod2FEHelper::HasThisImpl() const {
  return !IsStatic();
}

bool JBCClassMethod2FEHelper::IsClinit() const {
  const jbc::JBCConstPool &constPool = method.GetConstPool();
  std::string methodName = method.GetName(constPool);
  return methodName.compare("<clinit>") == 0;
}

bool JBCClassMethod2FEHelper::IsInit() const {
  const jbc::JBCConstPool &constPool = method.GetConstPool();
  std::string methodName = method.GetName(constPool);
  return methodName.compare("<init>") == 0;
}

MIRType *JBCClassMethod2FEHelper::GetTypeForThisImpl() const {
  FEIRTypeDefault type;
  std::string klassName = method.GetClassName();
  type.LoadFromJavaTypeName(klassName, false);
  return type.GenerateMIRType(true);
}
}  // namespace maple
