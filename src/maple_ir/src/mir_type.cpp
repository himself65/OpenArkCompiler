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
#include "mir_type.h"
#include <iostream>
#include <cstring>
#include "mir_symbol.h"
#include "printing.h"
#include "name_mangler.h"
#include "global_tables.h"
#include "mir_builder.h"
#include "cfg_primitive_types.h"
#if MIR_FEATURE_FULL

namespace maple {
#define LOAD_PRIMARY_TYPE_PROPERTY
#include "prim_types.def"

#define LOAD_ALGO_PRIMARY_TYPE
const PrimitiveTypeProperty &GetPrimitiveTypeProperty(PrimType pType) {
  switch (pType) {
    case PTY_begin:
      return PTProperty_begin;
#define PRIMTYPE(P) \
    case PTY_##P:   \
      return PTProperty_##P;
#include "prim_types.def"
#undef PRIMTYPE
    case PTY_end:
    default:
      return PTProperty_end;
  }
}

PrimType GetRegPrimType(PrimType primType) {
  switch (primType) {
    case PTY_i8:
    case PTY_i16:
      return PTY_i32;
    case PTY_u1:
    case PTY_u8:
    case PTY_u16:
      return PTY_u32;
    default:
      return primType;
  }
}

bool VerifyPrimType(PrimType primType1, PrimType primType2) {
  switch (primType1) {
    case PTY_u1:
    case PTY_u8:
    case PTY_u16:
    case PTY_u32:
    case PTY_a32:
      return IsUnsignedInteger(primType2);
    case PTY_i8:
    case PTY_i16:
    case PTY_i32:
      return IsSignedInteger(primType2);
    default:
      return primType1 == primType2;
  }
}

PrimType GetDynType(PrimType primType) {
#ifdef DYNAMICLANG
  switch (primType) {
    case PTY_u1:
      return PTY_dynbool;
    case PTY_i32:
      return PTY_dyni32;
    case PTY_simplestr:
      return PTY_dynstr;
    case PTY_simpleobj:
      return PTY_dynobj;
    case PTY_f32:
      return PTY_dynf32;
    case PTY_f64:
      return PTY_dynf64;
    default:
      return primType;
  }
#else
  return primType;
#endif
}

PrimType GetNonDynType(PrimType primType) {
#ifdef DYNAMICLANG
  switch (primType) {
    case PTY_dynbool:
      return PTY_u1;
    case PTY_dyni32:
      return PTY_i32;
    case PTY_dynstr:
      return PTY_simplestr;
    case PTY_dynobj:
      return PTY_simpleobj;
    case PTY_dynf32:
      return PTY_f32;
    case PTY_dynf64:
      return PTY_f64;
    default:
      return primType;
  }
#else
  return primType;
#endif
}

bool IsNoCvtNeeded(PrimType toType, PrimType fromType) {
  switch (toType) {
    case PTY_i32:
      return fromType == PTY_i16 || fromType == PTY_i8;
    case PTY_u32:
      return fromType == PTY_u16 || fromType == PTY_u8;
    case PTY_u1:
    case PTY_u8:
    case PTY_u16:
      return fromType == PTY_u32;
    case PTY_i8:
    case PTY_i16:
      return fromType == PTY_i32;
    default:
      return false;
  }
}

// answer in bytes; 0 if unknown
uint32 GetPrimTypeSize(PrimType primType) {
  switch (primType) {
    case PTY_void:
    case PTY_agg:
      return 0;
    case PTY_u1:
    case PTY_i8:
    case PTY_u8:
      return 1;
    case PTY_i16:
    case PTY_u16:
      return 2;
    case PTY_a32:
    case PTY_f32:
    case PTY_i32:
    case PTY_u32:
    case PTY_simplestr:
    case PTY_simpleobj:
      return 4;
    case PTY_a64:
    case PTY_c64:
    case PTY_f64:
    case PTY_i64:
    case PTY_u64:
      return 8;
    case PTY_c128:
    case PTY_f128:
      return 16;
#ifdef DYNAMICLANG
    case PTY_dynf32:
    case PTY_dyni32:
    case PTY_dynstr:
    case PTY_dynobj:
    case PTY_dynundef:
    case PTY_dynnull:
    case PTY_dynbool:
      return 8;
    case PTY_dynany:
    case PTY_dynf64:
      return 8;
#endif
    default:
      return 0;
  }
}

// answer is n if size in byte is (1<<n) (0: 1B; 1: 2B, 2: 4B, 3: 8B, 4:16B)
uint32 GetPrimTypeP2Size(PrimType primType) {
  switch (primType) {
    case PTY_u1:
    case PTY_i8:
    case PTY_u8:
      return 0;
    case PTY_i16:
    case PTY_u16:
      return 1;
    case PTY_a32:
    case PTY_f32:
    case PTY_i32:
    case PTY_u32:
    case PTY_simplestr:
    case PTY_simpleobj:
      return 2;
    case PTY_a64:
    case PTY_c64:
    case PTY_f64:
    case PTY_i64:
    case PTY_u64:
      return 3;
    case PTY_c128:
    case PTY_f128:
      return 4;
#ifdef DYNAMICLANG
    case PTY_dynf32:
    case PTY_dyni32:
    case PTY_dynstr:
    case PTY_dynobj:
    case PTY_dynundef:
    case PTY_dynnull:
    case PTY_dynbool:
    case PTY_dynany:
    case PTY_dynf64:
      return 3;
#endif
    default:
      ASSERT(false, "Power-of-2 size only applicable to sizes of 1, 2, 4, 8 or 16 bytes.");
      return 10;
  }
}

const char *GetPrimTypeName(PrimType primType) {
#define LOAD_ALGO_PRIMARY_TYPE
  switch (primType) {
    case kPtyInvalid:
      return "kPtyInvalid";
#define PRIMTYPE(P) \
    case PTY_##P:   \
      return #P;
#include "prim_types.def"
#undef PRIMTYPE
    case kPtyDerived:
      return "derived";  // just for test: no primitive type for derived
    default:
      return "kPtyInvalid";
  }
  // SIMD types to be defined
}

const char *GetPrimTypeJavaName(PrimType primType) {
  switch (primType) {
    case PTY_u1:
      return "Z";
    case PTY_i8:
      return "B";
    case PTY_i16:
      return "S";
    case PTY_u16:
      return "C";
    case PTY_i32:
      return "I";
    case PTY_i64:
      return "J";
    case PTY_f32:
      return "F";
    case PTY_f64:
      return "D";
    case PTY_void:
      return "V";
    case PTY_constStr:
      return kJstrTypeName.c_str();
    case kPtyInvalid:
      return "invalid";
    default:
      return "invalid";
  }
}

void TypeAttrs::DumpAttributes() const {
#define TYPE_ATTR
#define STRING(s) #s
#define ATTR(AT)          \
  if (GetAttr(ATTR_##AT)) \
    LogInfo::MapleLogger() << " " << STRING(AT);
#include "all_attributes.def"
#undef ATTR
#undef TYPE_ATTR
  if (attrAlign) {
    LogInfo::MapleLogger() << " align(" << GetAlign() << ")";
  }
}

void FieldAttrs::DumpAttributes() const {
#define FIELD_ATTR
#define STRING(s) #s
#define ATTR(AT)             \
  if (GetAttr(FLDATTR_##AT)) \
    LogInfo::MapleLogger() << " " << STRING(AT);
#include "all_attributes.def"
#undef ATTR
#undef FIELD_ATTR
  if (attrAlign) {
    LogInfo::MapleLogger() << " align(" << GetAlign() << ")";
  }
}

const std::string &MIRType::GetName() const {
  return GlobalTables::GetStrTable().GetStringFromStrIdx(nameStrIdx);
}

bool MIRType::ValidateClassOrInterface(const std::string &className, bool noWarning) const {
  if (primType == maple::PTY_agg && (typeKind == maple::kTypeClass || typeKind == maple::kTypeInterface) &&
      nameStrIdx != 0) {
    return true;
  }
  if (!noWarning) {
    size_t len = className.size();
    constexpr int minClassNameLen = 4;
    constexpr char suffix[] = "_3B";
    size_t suffixLen = std::strlen(suffix);
    if (len > minClassNameLen && strncmp(className.c_str() + len - suffixLen, suffix, suffixLen) == 0) {
      LogInfo::MapleLogger(kLlErr) << "error: missing proper mplt file for " << className << '\n';
    } else {
      LogInfo::MapleLogger(kLlErr) << "internal error: type is not java class or interface " << className << '\n';
    }
  }
  return false;
}

bool MIRType::PointsToConstString() const {
  return (typeKind == kTypePointer) ? static_cast<const MIRPtrType*>(this)->PointsToConstString() : false;
}

std::string MIRType::GetMplTypeName() const {
  if (typeKind == kTypeScalar) {
    return GetPrimTypeName(primType);
  }
  return "";
}

std::string MIRType::GetCompactMplTypeName() const {
  if (typeKind == kTypeScalar) {
    return GetPrimTypeJavaName(primType);
  }
  return "";
}

void MIRType::Dump(int indent, bool dontUseName) const {
  LogInfo::MapleLogger() << GetPrimTypeName(primType);
}

void MIRType::DumpAsCxx(int indent) const {
  switch (primType) {
    case PTY_void:
      LogInfo::MapleLogger() << "void";
      break;
    case PTY_i8:
      LogInfo::MapleLogger() << "int8 ";
      break;
    case PTY_i16:
      LogInfo::MapleLogger() << "int16";
      break;
    case PTY_i32:
      LogInfo::MapleLogger() << "int32";
      break;
    case PTY_i64:
      LogInfo::MapleLogger() << "int64";
      break;
    case PTY_u8:
      LogInfo::MapleLogger() << "uint8";
      break;
    case PTY_u16:
      LogInfo::MapleLogger() << "uint16";
      break;
    case PTY_u32:
      LogInfo::MapleLogger() << "uint32";
      break;
    case PTY_u64:
      LogInfo::MapleLogger() << "uint64";
      break;
    case PTY_u1:
      LogInfo::MapleLogger() << "bool  ";
      break;
    case PTY_ptr:
      LogInfo::MapleLogger() << "void* ";
      break;
    case PTY_ref:
      LogInfo::MapleLogger() << "void* ";
      break;
    case PTY_a32:
      LogInfo::MapleLogger() << "int32";
      break;
    case PTY_a64:
      LogInfo::MapleLogger() << "void* ";
      break;
    case PTY_f32:
      LogInfo::MapleLogger() << "float ";
      break;
    case PTY_f64:
      LogInfo::MapleLogger() << "double";
      break;
    case PTY_c64:
      LogInfo::MapleLogger() << "float complex";
      break;
    case PTY_c128:
      LogInfo::MapleLogger() << "double complex";
      break;
    default:
      ASSERT(false, "not yet implemented");
  }
}

bool MIRType::IsOfSameType(MIRType &type) {
  if (typeKind != type.typeKind) {
    return false;
  }

  if (typeKind == kTypePointer) {
    const auto &ptrType = static_cast<const MIRPtrType&>(*this);
    const auto &ptrTypeIt = static_cast<const MIRPtrType&>(type);
    if (ptrType.GetPointedTyIdx() == ptrTypeIt.GetPointedTyIdx()) {
      return true;
    } else {
      MIRType &mirTypeIt = *GlobalTables::GetTypeTable().GetTypeFromTyIdx(ptrTypeIt.GetPointedTyIdx());
      return GlobalTables::GetTypeTable().GetTypeFromTyIdx(ptrType.GetPointedTyIdx())->IsOfSameType(mirTypeIt);
    }
  } else if (typeKind == kTypeJArray) {
    auto &arrType1 = static_cast<MIRJarrayType&>(*this);
    auto &arrType2 = static_cast<MIRJarrayType&>(type);
    if (arrType1.GetDim() != arrType2.GetDim()) {
      return false;
    }
    return arrType1.GetElemType()->IsOfSameType(*arrType2.GetElemType());
  } else {
    return tyIdx == type.tyIdx;
  }
}

inline void DumpTypeName(GStrIdx strIdx, bool isLocal) {
  LogInfo::MapleLogger() << ((isLocal) ? "%" : "$") << GlobalTables::GetStrTable().GetStringFromStrIdx(strIdx);
}

static bool CheckAndDumpTypeName(GStrIdx strIdx, bool isLocal) {
  if (strIdx == 0) {
    return false;
  }
  LogInfo::MapleLogger() << "<";
  DumpTypeName(strIdx, isLocal);
  LogInfo::MapleLogger() << ">";
  return true;
}

void MIRFuncType::Dump(int indent, bool dontUseName) const {
  if (!dontUseName && CheckAndDumpTypeName(nameStrIdx, nameIsLocal)) {
    return;
  }
  LogInfo::MapleLogger() << "<func(";
  size_t size = paramTypeList.size();
  for (size_t i = 0; i < size; ++i) {
    GlobalTables::GetTypeTable().GetTypeFromTyIdx(paramTypeList[i])->Dump(indent + 1);
    if (size - 1 != i) {
      LogInfo::MapleLogger() << ",";
    }
  }
  if (isVarArgs) {
    LogInfo::MapleLogger() << ", ...";
  }
  LogInfo::MapleLogger() << ") ";
  GlobalTables::GetTypeTable().GetTypeFromTyIdx(retTyIdx)->Dump(indent + 1);
  LogInfo::MapleLogger() << ">";
}

void MIRArrayType::Dump(int indent, bool dontUseName) const {
  if (!dontUseName && CheckAndDumpTypeName(nameStrIdx, nameIsLocal)) {
    return;
  }
  LogInfo::MapleLogger() << "<";
  for (uint16 i = 0; i < dim; ++i) {
    LogInfo::MapleLogger() << "[" << GetSizeArrayItem(i) << "]";
  }
  LogInfo::MapleLogger() << " ";
  GlobalTables::GetTypeTable().GetTypeFromTyIdx(eTyIdx)->Dump(indent + 1);
  LogInfo::MapleLogger() << ">";
}

std::string MIRArrayType::GetCompactMplTypeName() const {
  std::stringstream ss;
  ss << "A";
  MIRType *elemType = GetElemType();
  ss << elemType->GetCompactMplTypeName();
  return ss.str();
}

void MIRFarrayType::Dump(int indent, bool dontUseName) const {
  if (!dontUseName && CheckAndDumpTypeName(nameStrIdx, nameIsLocal)) {
    return;
  }
  LogInfo::MapleLogger() << "<[] ";
  GlobalTables::GetTypeTable().GetTypeFromTyIdx(elemTyIdx)->Dump(indent + 1);
  LogInfo::MapleLogger() << ">";
}

std::string MIRFarrayType::GetCompactMplTypeName() const {
  std::stringstream ss;
  ss << "A";
  MIRType *elemType = GetElemType();
  ss << elemType->GetCompactMplTypeName();
  return ss.str();
}

const std::string &MIRJarrayType::GetJavaName() {
  if (javaNameStrIdx == 0) {
    DetermineName();
  }
  return GlobalTables::GetStrTable().GetStringFromStrIdx(javaNameStrIdx);
}

MIRStructType *MIRJarrayType::GetParentType() {
  if (parentTyIdx == 0) {
    GStrIdx jloStrIdx = GlobalTables::GetStrTable().GetStrIdxFromName(
        NameMangler::GetInternalNameLiteral(NameMangler::kJavaLangObjectStr));
    parentTyIdx = GlobalTables::GetTypeNameTable().GetTyIdxFromGStrIdx(jloStrIdx);
    ASSERT((parentTyIdx != 0), "cannot find type for java.lang.Object");
  }
  return static_cast<MIRStructType*>(GlobalTables::GetTypeTable().GetTypeFromTyIdx(parentTyIdx));
}

void MIRJarrayType::DetermineName() {
  if (javaNameStrIdx != 0) {
    return;  // already determined
  }
  MIRType *elemType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(GetElemTyIdx());
  dim = 1;
  std::string baseName;
  while (true) {
    if (elemType->GetKind() == kTypeScalar) {
      baseName = GetPrimTypeJavaName(elemType->GetPrimType());
      fromPrimitive = true;
      break;
    } else if (elemType->GetKind() == kTypePointer) {
      auto *pType = static_cast<MIRPtrType*>(elemType)->GetPointedType();
      ASSERT(pType != nullptr, "pType is null in MIRJarrayType::DetermineName");
      if (pType->GetKind() == kTypeByName || pType->GetKind() == kTypeClass || pType->GetKind() == kTypeInterface ||
          pType->GetKind() == kTypeClassIncomplete || pType->GetKind() == kTypeInterfaceIncomplete) {
        baseName = static_cast<MIRStructType*>(pType)->GetName();
        fromPrimitive = false;
        break;
      } else if (pType->GetKind() == kTypeJArray) {
        auto *tmpPtype = static_cast<MIRJarrayType*>(pType);
        elemType = tmpPtype->GetElemType();
        ASSERT(elemType != nullptr, "elemType is null in MIRJarrayType::DetermineName");
        ++dim;
      } else {
        ASSERT(false, "unexpected type!");
      }
    } else {
      ASSERT(false, "unexpected type!");
    }
  }
  std::string name;
  for (int i = dim; i > 0; --i) {
    name += JARRAY_PREFIX_STR;
  }
  name += baseName;
  javaNameStrIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(name);
}

MIRType *MIRPtrType::GetPointedType() const {
  return GlobalTables::GetTypeTable().GetTypeFromTyIdx(pointedTyIdx);
}

bool MIRType::IsVolatile(int fieldID) const {
  if (fieldID == 0) {
    return HasVolatileField();
  }
  return static_cast<const MIRStructType*>(this)->IsFieldVolatile(fieldID);
}

bool MIRPtrType::HasTypeParam() const {
  return GetPointedType()->HasTypeParam();
}

bool MIRPtrType::PointsToConstString() const {
  GStrIdx typeNameIdx = GetPointedType()->GetNameStrIdx();
  std::string typeName = GlobalTables::GetStrTable().GetStringFromStrIdx(typeNameIdx);
  return typeName == "Ljava_2Flang_2FString_3B";
}

void MIRPtrType::Dump(int indent, bool dontUseName) const {
  if (!dontUseName && CheckAndDumpTypeName(nameStrIdx, nameIsLocal)) {
    return;
  }
  MIRType *pointedType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(pointedTyIdx);
  if (pointedType->GetKind() == kTypeFunction) {  // no * for function pointer
    pointedType->Dump(indent);
  } else {
    LogInfo::MapleLogger() << "<* ";
    pointedType->Dump(indent + 1);
    LogInfo::MapleLogger() << ">";
  }
}

void MIRBitFieldType::Dump(int indent, bool dontUseName) const {
  LogInfo::MapleLogger() << ":" << static_cast<int>(fieldSize) << " " << GetPrimTypeName(primType);
}

size_t MIRClassType::GetSize() const {
  if (parentTyIdx == 0) {
    return MIRStructType::GetSize();
  }
  const auto *parentType = static_cast<const MIRClassType*>(GlobalTables::GetTypeTable().GetTypeFromTyIdx(parentTyIdx));
  size_t parentSize = parentType->GetSize();
  if (parentSize == 0) {
    return 0;
  }
  size_t structSize = MIRStructType::GetSize();
  if (structSize == 0) {
    return 0;
  }
  return parentSize + structSize;
}

FieldID MIRClassType::GetFirstLocalFieldID() const {
  if (!IsLocal()) {
    return 0;
  }
  if (parentTyIdx == 0) {
    return 1;
  }

  constexpr uint8 lastFieldIDOffset = 2;
  constexpr uint8 firstLocalFieldIDOffset = 1;
  const auto *parentClassType =
      static_cast<const MIRClassType*>(GlobalTables::GetTypeTable().GetTypeFromTyIdx(parentTyIdx));
  return !parentClassType->IsLocal() ? parentClassType->GetLastFieldID() + lastFieldIDOffset
                                     : parentClassType->GetFirstLocalFieldID() + firstLocalFieldIDOffset;
}

const MIRClassType *MIRClassType::GetExceptionRootType() const {
  GStrIdx ehTypeNameIdx = GlobalTables::GetStrTable().GetStrIdxFromName(NameMangler::kJavaLangObjectStr);
  const MIRClassType *subClassType = this;
  while (subClassType != nullptr && subClassType->nameStrIdx != ehTypeNameIdx) {
    subClassType =
        static_cast<const MIRClassType*>(GlobalTables::GetTypeTable().GetTypeFromTyIdx(subClassType->parentTyIdx));
  }
  return subClassType;
}

MIRClassType *MIRClassType::GetExceptionRootType() {
  return const_cast<MIRClassType*>(const_cast<const MIRClassType*>(this)->GetExceptionRootType());
}

bool MIRClassType::IsExceptionType() const {
  GStrIdx ehTypeNameIdx = GlobalTables::GetStrTable().GetStrIdxFromName("Ljava_2Flang_2FThrowable_3B");
  const MIRClassType *parentClassType = this;
  while (parentClassType != nullptr && parentClassType->nameStrIdx != ehTypeNameIdx) {
    parentClassType =
        static_cast<MIRClassType*>(GlobalTables::GetTypeTable().GetTypeFromTyIdx(parentClassType->parentTyIdx));
  }
  return parentClassType != nullptr;
}

FieldID MIRClassType::GetLastFieldID() const {
  FieldID fieldID = fields.size();
  if (parentTyIdx != 0) {
    const auto *parentClassType =
        static_cast<const MIRClassType*>(GlobalTables::GetTypeTable().GetTypeFromTyIdx(parentTyIdx));
    if (parentClassType != nullptr) {
      fieldID += parentClassType->GetLastFieldID() + 1;
    }
  }
  return fieldID;
}

static void DumpClassOrInterfaceInfo(const MIRStructType &type, int indent) {
  const std::vector<MIRInfoPair> &info = type.GetInfo();
  std::vector<bool> infoIsString = type.GetInfoIsString();
  size_t size = info.size();
  for (size_t i = 0; i < size; ++i) {
    LogInfo::MapleLogger() << '\n';
    PrintIndentation(indent);
    LogInfo::MapleLogger() << "@" << GlobalTables::GetStrTable().GetStringFromStrIdx(info[i].first) << " ";
    if (!infoIsString[i]) {
      LogInfo::MapleLogger() << info[i].second;
    } else {
      LogInfo::MapleLogger() << "\"" << GlobalTables::GetStrTable().GetStringFromStrIdx(GStrIdx(info[i].second))
                             << "\"";
    }
    if (i != size - 1) {
      LogInfo::MapleLogger() << ",";
    }
  }
}

static uint32 GetInfoFromStrIdx(const std::vector<MIRInfoPair> &info, const GStrIdx &strIdx) {
  for (MIRInfoPair infoPair : info) {
    if (infoPair.first == strIdx) {
      return infoPair.second;
    }
  }
  return 0;
}

uint32 MIRInterfaceType::GetInfo(GStrIdx strIdx) const {
  return GetInfoFromStrIdx(info, strIdx);
}

// return class id or superclass id accroding to input string
uint32 MIRInterfaceType::GetInfo(const std::string &infoStr) const {
  GStrIdx strIdx = GlobalTables::GetStrTable().GetStrIdxFromName(infoStr);
  return GetInfo(strIdx);
}
size_t MIRInterfaceType::GetSize() const {
  if (parentsTyIdx.empty()) {
    return MIRStructType::GetSize();
  }
  size_t size = MIRStructType::GetSize();
  if (size == 0) {
    return 0;
  }
  for (size_t i = 0; i < parentsTyIdx.size(); ++i) {
    const auto *parentType =
        static_cast<const MIRInterfaceType*>(GlobalTables::GetTypeTable().GetTypeFromTyIdx(parentsTyIdx[i]));
    size_t parentSize = parentType->GetSize();
    if (parentSize == 0) {
      return 0;
    }
    size += parentSize;
  }
  return size;
}

static void DumpStaticValue(const MIREncodedArray &staticValue, int indent) {
  if (staticValue.empty()) {
    return;
  }
  LogInfo::MapleLogger() << '\n';
  PrintIndentation(indent);
  LogInfo::MapleLogger() << "@staticvalue";
  constexpr uint32 typeLen = 5;
  constexpr uint32 typeMask = 0x1f;
  for (const auto &value : staticValue) {
    LogInfo::MapleLogger() << " [";
    uint8 valueArg = static_cast<uint32>(value.encodedValue[0]) >> typeLen;
    uint8 valueType = static_cast<uint32>(value.encodedValue[0]) & typeMask;
    // kValueNull kValueBoolean
    constexpr uint32 simpleOffset = 1;
    constexpr uint32 aggOffSet = 2;
    valueArg = (valueType == kValueNull || valueType == kValueBoolean) ? simpleOffset : valueArg + aggOffSet;
    for (uint32 k = 0; k < valueArg; ++k) {
      LogInfo::MapleLogger() << static_cast<uint32>(value.encodedValue[k]);
      if (k != static_cast<uint32>(valueArg - 1)) {
        LogInfo::MapleLogger() << " ";
      }
    }
    LogInfo::MapleLogger() << "]";
  }
}

static void DumpFields(FieldVector fields, int indent, bool otherFields = false) {
  size_t size = fields.size();
  for (size_t i = 0; i < size; ++i) {
    LogInfo::MapleLogger() << '\n';
    PrintIndentation(indent);
    LogInfo::MapleLogger() << ((!otherFields) ? "@" : "^")
                           << GlobalTables::GetStrTable().GetStringFromStrIdx(fields[i].first) << " ";
    GlobalTables::GetTypeTable().GetTypeFromTyIdx(fields[i].second.first)->Dump(indent + 1);
    FieldAttrs &fa = fields[i].second.second;
    fa.DumpAttributes();
    if (fa.GetAttr(FLDATTR_static) && fa.GetAttr(FLDATTR_final) &&
        (fa.GetAttr(FLDATTR_public) || fa.GetAttr(FLDATTR_protected))) {
      const char *fieldName = GlobalTables::GetStrTable().GetStringFromStrIdx(fields[i].first).c_str();
      MIRSymbol *fieldVar =
          GlobalTables::GetGsymTable().GetSymbolFromStrIdx(GlobalTables::GetStrTable().GetStrIdxFromName(fieldName));
      if (fieldVar != nullptr && fieldVar->GetKonst() != nullptr &&
          fieldVar->GetKonst()->GetKind() == kConstStr16Const) {
        LogInfo::MapleLogger() << " = ";
        fieldVar->GetKonst()->Dump();
      }
    }
    if (i != size - 1) {
      LogInfo::MapleLogger() << ",";
    }
  }
}

static void DumpFieldsAsCxx(const FieldVector &fields, int indent) {
  for (auto &f : fields) {
    PrintIndentation(indent);
    const FieldAttrs &fa = f.second.second;
    if (fa.GetAttr(FLDATTR_static)) {
      LogInfo::MapleLogger() << "// ";
    }
    LogInfo::MapleLogger() << "/* ";
    fa.DumpAttributes();
    GlobalTables::GetTypeTable().GetTypeFromTyIdx(f.second.first)->Dump(indent + 1);
    LogInfo::MapleLogger() << " */ ";
    GlobalTables::GetTypeTable().GetTypeFromTyIdx(f.second.first)->DumpAsCxx(indent + 1);
    LogInfo::MapleLogger() << " " << GlobalTables::GetStrTable().GetStringFromStrIdx(f.first) << ";" << '\n';
  }
}

static void DumpMethods(MethodVector methods, int indent) {
  size_t size = methods.size();
  for (size_t i = 0; i < size; ++i) {
    LogInfo::MapleLogger() << '\n';
    PrintIndentation(indent);
    LogInfo::MapleLogger() << "&" << GlobalTables::GetGsymTable().GetSymbolFromStidx(methods[i].first.Idx())->GetName();
    methods[i].second.second.DumpAttributes();
    LogInfo::MapleLogger() << " (";
    auto *funcType =
        static_cast<MIRFuncType*>(GlobalTables::GetTypeTable().GetTypeFromTyIdx(methods[i].second.first));
    size_t parmListSize = funcType->GetParamTypeList().size();
    for (size_t j = 0; j < parmListSize; ++j) {
      GlobalTables::GetTypeTable().GetTypeFromTyIdx(funcType->GetNthParamType(j))->Dump(indent + 1);
      if (j != parmListSize - 1) {
        LogInfo::MapleLogger() << ",";
      }
    }
    if (funcType->IsVarargs()) {
      LogInfo::MapleLogger() << ", ...";
    }
    LogInfo::MapleLogger() << ") ";
    GlobalTables::GetTypeTable().GetTypeFromTyIdx(funcType->GetRetTyIdx())->Dump(indent + 1);
    if (i != size - 1) {
      LogInfo::MapleLogger() << ",";
    }
  }
}

static void DumpConstructorsAsCxx(MethodVector methods, int indent) {
  unsigned int i = 0;
  for (auto &m : methods) {
    FuncAttrs &fa = m.second.second;
    if (!fa.GetAttr(FUNCATTR_constructor) || !fa.GetAttr(FUNCATTR_public)) {
      continue;
    }
    auto *funcType = static_cast<MIRFuncType*>(GlobalTables::GetTypeTable().GetTypeFromTyIdx(m.second.first));
    PrintIndentation(indent);
    LogInfo::MapleLogger() << "/* &" << GlobalTables::GetGsymTable().GetSymbolFromStidx(m.first.Idx())->GetName();
    fa.DumpAttributes();
    LogInfo::MapleLogger() << " (";
    unsigned int j = 0;
    size_t paramTypeListSize = funcType->GetParamTypeList().size();
    for (auto &p : funcType->GetParamTypeList()) {
      GlobalTables::GetTypeTable().GetTypeFromTyIdx(p)->Dump(indent + 1);
      if (j != paramTypeListSize - 1) {
        LogInfo::MapleLogger() << ", ";
      }
      ++j;
    }
    if (funcType->IsVarargs()) {
      LogInfo::MapleLogger() << ", ...";
    }
    LogInfo::MapleLogger() << ") ";
    GlobalTables::GetTypeTable().GetTypeFromTyIdx(funcType->GetRetTyIdx())->Dump(indent + 1);
    LogInfo::MapleLogger() << " */" << '\n';
    PrintIndentation(indent);
    LogInfo::MapleLogger() << "/* ";
    LogInfo::MapleLogger() << NameMangler::DecodeName(
        GlobalTables::GetGsymTable().GetSymbolFromStidx(m.first.Idx())->GetName());
    LogInfo::MapleLogger() << " */" << '\n';
    PrintIndentation(indent);
    LogInfo::MapleLogger() << "extern \"C\" ";
    // return type
    GlobalTables::GetTypeTable().GetTypeFromTyIdx(funcType->GetRetTyIdx())->DumpAsCxx(0);
    LogInfo::MapleLogger() << " " << GlobalTables::GetGsymTable().GetSymbolFromStidx(m.first.Idx())->GetName() << "( ";
    j = 0;
    for (auto &p : funcType->GetParamTypeList()) {
      GlobalTables::GetTypeTable().GetTypeFromTyIdx(p)->DumpAsCxx(indent + 1);
      if (j != paramTypeListSize - 1) {
        LogInfo::MapleLogger() << ", ";
      }
      ++j;
    }
    if (funcType->IsVarargs()) {
      LogInfo::MapleLogger() << ", ...";
    }
    LogInfo::MapleLogger() << ")";
    if (methods.size() - 1 != i++) {
      LogInfo::MapleLogger() << ";" << '\n';
    }
  }
}

static void DumpInterfaces(std::vector<TyIdx> interfaces, int indent) {
  size_t size = interfaces.size();
  for (size_t i = 0; i < size; ++i) {
    LogInfo::MapleLogger() << '\n';
    PrintIndentation(indent);
    GStrIdx stridx = GlobalTables::GetTypeTable().GetTypeFromTyIdx(interfaces[i])->GetNameStrIdx();
    LogInfo::MapleLogger() << "$" << GlobalTables::GetStrTable().GetStringFromStrIdx(stridx);
    if (i != size - 1) {
      LogInfo::MapleLogger() << ",";
    }
  }
}

size_t MIRStructType::GetSize() const {
  if (typeKind == kTypeUnion) {
    size_t maxSize = GetElemType(0)->GetSize();
    for (size_t i = 1; i < fields.size(); ++i) {
      size_t size = GetElemType(i)->GetSize();
      if (size == 0) {
        return 0;
      }
      if (maxSize < size) {
        maxSize = size;
      }
    }
    return maxSize;
  }
  size_t size = 0;
  for (size_t i = 0; i < fields.size(); ++i) {
    size_t fieldSize = GetElemType(i)->GetSize();
    if (fieldSize == 0) {
      return 0;
    }
    size += fieldSize;
  }
  return size;
}

void MIRStructType::DumpFieldsAndMethods(int indent, bool hasMethod) const {
  DumpFields(fields, indent);
  bool hasField = !fields.empty();
  bool hasStaticField = !staticFields.empty();
  if (hasField && hasStaticField) {
    LogInfo::MapleLogger() << ",";
  }
  DumpFields(staticFields, indent, true);
  bool hasFieldOrStaticField = hasField || hasStaticField;
  bool hasParentField = !parentFields.empty();
  if (hasFieldOrStaticField && hasParentField) {
    LogInfo::MapleLogger() << ",";
  }
  DumpFields(parentFields, indent, true);
  if ((hasFieldOrStaticField || hasParentField) && hasMethod) {
    LogInfo::MapleLogger() << ",";
  }
  DumpMethods(methods, indent);
}

void MIRStructType::Dump(int indent, bool dontUseName) const {
  if (!dontUseName && CheckAndDumpTypeName(nameStrIdx, nameIsLocal)) {
    return;
  }
  LogInfo::MapleLogger() << ((typeKind == kTypeStruct) ? "<struct {"
                                                       : ((typeKind == kTypeUnion) ? "<union {"
                                                                                   : "<structincomplete {"));
  DumpFieldsAndMethods(indent, !methods.empty());
  LogInfo::MapleLogger() << "}>";
}

uint32 MIRClassType::GetInfo(GStrIdx strIdx) const {
  return GetInfoFromStrIdx(info, strIdx);
}

// return class id or superclass id accroding to input string
uint32 MIRClassType::GetInfo(const std::string &infoStr) const {
  GStrIdx strIdx = GlobalTables::GetStrTable().GetStrIdxFromName(infoStr);
  return GetInfo(strIdx);
}

bool MIRClassType::IsFinal() const {
  uint32 attrStrIdx = GetInfo(GlobalTables::GetStrTable().GetOrCreateStrIdxFromName("INFO_attribute_string"));
  CHECK(attrStrIdx < GlobalTables::GetStrTable().StringTableSize(), "out of range of vector");
  const std::string &kAttrString = GlobalTables::GetStrTable().GetStringFromStrIdx(GStrIdx(attrStrIdx));
  return kAttrString.find(" final ") != std::string::npos;
}

bool MIRClassType::IsAbstract() const {
  uint32 attrStrIdx = GetInfo(GlobalTables::GetStrTable().GetOrCreateStrIdxFromName("INFO_attribute_string"));
  CHECK(attrStrIdx < GlobalTables::GetStrTable().StringTableSize(), "out of range of vector");
  const std::string &kAttrString = GlobalTables::GetStrTable().GetStringFromStrIdx(GStrIdx(attrStrIdx));
  return kAttrString.find(" abstract ") != std::string::npos;
}

bool MIRClassType::IsInner() const {
  const std::string &name = GetName();
  return name.find("_24") != std::string::npos;
}

static void DumpInfoPragmaStaticValue(const std::vector<MIRInfoPair> &info, const std::vector<MIRPragma*> &pragmaVec,
                                      const MIREncodedArray &staticValue, int indent, bool hasFieldMethodOrInterface) {
  bool hasPragma = pragmaVec.size();
  bool hasStaticValue = staticValue.size();
  if (!info.empty() && (hasPragma || hasStaticValue || hasFieldMethodOrInterface)) {
    LogInfo::MapleLogger() << ",";
  }
  size_t size = pragmaVec.size();
  for (size_t i = 0; i < size; ++i) {
    pragmaVec[i]->Dump(indent);
    if (i != size - 1) {
      LogInfo::MapleLogger() << ",";
    }
  }
  if (hasPragma && (hasStaticValue || hasFieldMethodOrInterface)) {
    LogInfo::MapleLogger() << ",";
  }
  DumpStaticValue(staticValue, indent);
  if (hasStaticValue && hasFieldMethodOrInterface) {
    LogInfo::MapleLogger() << ",";
  }
}

void MIRClassType::Dump(int indent, bool dontUseName) const {
  if (!dontUseName && CheckAndDumpTypeName(nameStrIdx, nameIsLocal)) {
    return;
  }
  LogInfo::MapleLogger() << ((typeKind == kTypeClass) ? "<class " : "<classincomplete ");
  if (parentTyIdx != 0) {
    GlobalTables::GetTypeTable().GetTypeFromTyIdx(parentTyIdx)->Dump(indent + 1);
    LogInfo::MapleLogger() << " ";
  }
  LogInfo::MapleLogger() << "{";
  DumpClassOrInterfaceInfo(*this, indent);
  bool hasFieldMethodOrInterface = !(fields.empty() && parentFields.empty() && staticFields.empty() &&
                                     methods.empty() && interfacesImplemented.empty());
  DumpInfoPragmaStaticValue(info, pragmaVec, staticValue, indent, hasFieldMethodOrInterface);

  bool hasMethod = !methods.empty();
  bool hasImplementedInterface = !interfacesImplemented.empty();
  DumpFieldsAndMethods(indent, hasMethod || hasImplementedInterface);
  if (hasMethod && hasImplementedInterface) {
    LogInfo::MapleLogger() << ",";
  }
  DumpInterfaces(interfacesImplemented, indent);
  LogInfo::MapleLogger() << "}>";
}

void MIRClassType::DumpAsCxx(int indent) const {
  LogInfo::MapleLogger() << "{\n";
  DumpFieldsAsCxx(fields, indent);
  LogInfo::MapleLogger() << "};\n";
  DumpConstructorsAsCxx(methods, 0);
}

void MIRInterfaceType::Dump(int indent, bool dontUseName) const {
  if (!dontUseName && CheckAndDumpTypeName(nameStrIdx, nameIsLocal)) {
    return;
  }
  LogInfo::MapleLogger() << ((typeKind == kTypeInterface) ? "<interface " : "<interfaceincomplete ");
  for (TyIdx idx : parentsTyIdx) {
    GlobalTables::GetTypeTable().GetTypeFromTyIdx(idx)->Dump(0);
    LogInfo::MapleLogger() << " ";
  }
  LogInfo::MapleLogger() << " {";
  DumpClassOrInterfaceInfo(*this, indent);
  bool hasFieldOrMethod = !(fields.empty() && staticFields.empty() && parentFields.empty() && methods.empty());
  DumpInfoPragmaStaticValue(info, pragmaVec, staticValue, indent, hasFieldOrMethod);
  DumpFieldsAndMethods(indent, !methods.empty());
  LogInfo::MapleLogger() << "}>";
}

void MIRTypeByName::Dump(int indent, bool dontUseName) const {
  const std::string &name = GlobalTables::GetStrTable().GetStringFromStrIdx(nameStrIdx);
  LogInfo::MapleLogger() << (nameIsLocal ? "<%" : "<$") << name << ">";
}

void MIRTypeParam::Dump(int indent, bool dontUseName) const {
  const std::string &name = GlobalTables::GetStrTable().GetStringFromStrIdx(nameStrIdx);
  LogInfo::MapleLogger() << "<!" << name << ">";
}

void MIRInstantVectorType::Dump(int indent, bool dontUseName) const {
  LogInfo::MapleLogger() << "{";
  for (size_t i = 0; i < instantVec.size(); ++i) {
    TypePair typePair = instantVec[i];
    if (i != 0) {
      LogInfo::MapleLogger() << ", ";
    }
    MIRType *typeParmType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(typePair.first);
    const std::string &name = GlobalTables::GetStrTable().GetStringFromStrIdx(typeParmType->GetNameStrIdx());
    LogInfo::MapleLogger() << "!" << name << "=";
    MIRType *realty = GlobalTables::GetTypeTable().GetTypeFromTyIdx(typePair.second);
    realty->Dump(0);
  }
  LogInfo::MapleLogger() << "}";
}

void MIRGenericInstantType::Dump(int indent, bool dontUseName) const {
  LogInfo::MapleLogger() << "<";
  const MIRType *genericType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(genericTyIdx);
  DumpTypeName(genericType->GetNameStrIdx(), genericType->IsNameIsLocal());
  MIRInstantVectorType::Dump(indent, dontUseName);
  LogInfo::MapleLogger() << ">";
}

bool MIRType::EqualTo(const MIRType &mirType) const {
  return typeKind == mirType.typeKind && primType == mirType.primType;
}

bool MIRPtrType::EqualTo(const MIRType &type) const {
  if (typeKind != type.GetKind() || GetPrimType() != type.GetPrimType()) {
    return false;
  }
  const auto &pType = static_cast<const MIRPtrType&>(type);
  return pointedTyIdx == pType.GetPointedTyIdx();
}

bool MIRArrayType::EqualTo(const MIRType &type) const {
  if (type.GetKind() != typeKind) {
    return false;
  }
  const auto &pType = static_cast<const MIRArrayType&>(type);
  if (dim != pType.GetDim() || eTyIdx != pType.GetElemTyIdx()) {
    return false;
  }
  for (size_t i = 0; i < dim; ++i) {
    if (GetSizeArrayItem(i) != pType.GetSizeArrayItem(i)) {
      return false;
    }
  }
  return true;
}

MIRType *MIRArrayType::GetElemType() const {
  return GlobalTables::GetTypeTable().GetTypeFromTyIdx(eTyIdx);
}

std::string MIRArrayType::GetMplTypeName() const {
  std::stringstream ss;
  ss << "<[] ";
  MIRType *elemType = GetElemType();
  ss << elemType->GetMplTypeName();
  ss << ">";
  return ss.str();
}

MIRType *MIRFarrayType::GetElemType() const {
  return GlobalTables::GetTypeTable().GetTypeFromTyIdx(elemTyIdx);
}

bool MIRFarrayType::EqualTo(const MIRType &type) const {
  if (type.GetKind() != typeKind) {
    return false;
  }
  const auto &pType = static_cast<const MIRFarrayType&>(type);
  return elemTyIdx == pType.GetElemTyIdx();
}

std::string MIRFarrayType::GetMplTypeName() const {
  std::stringstream ss;
  ss << "<[] ";
  MIRType *elemType = GetElemType();
  ss << elemType->GetMplTypeName();
  ss << ">";
  return ss.str();
}

bool MIRFuncType::EqualTo(const MIRType &type) const {
  if (type.GetKind() != typeKind) {
    return false;
  }
  const auto &pType = static_cast<const MIRFuncType&>(type);
  return (pType.retTyIdx == retTyIdx && pType.paramTypeList == paramTypeList &&
          pType.isVarArgs == isVarArgs && pType.paramAttrsList == paramAttrsList);
}

bool MIRBitFieldType::EqualTo(const MIRType &type) const {
  if (type.GetKind() != typeKind || type.GetPrimType() != primType) {
    return false;
  }
  const auto &pType = static_cast<const MIRBitFieldType&>(type);
  return pType.fieldSize == fieldSize;
}

bool MIRStructType::EqualTo(const MIRType &type) const {
  return (typeKind == type.GetKind() && nameStrIdx == type.GetNameStrIdx());
}

std::string MIRStructType::GetCompactMplTypeName() const {
  return GlobalTables::GetStrTable().GetStringFromStrIdx(nameStrIdx);
}

MIRType *MIRStructType::GetElemType(uint32 n) const {
  return GlobalTables::GetTypeTable().GetTypeFromTyIdx(GetElemTyIdx(n));
}

MIRType *MIRStructType::GetFieldType(FieldID fieldID) {
  const FieldPair &fieldPair = TraverseToField(fieldID);
  return GlobalTables::GetTypeTable().GetTypeFromTyIdx(fieldPair.second.first);
}

bool MIRStructType::IsLocal() const {
  return GlobalTables::GetGsymTable().GetStIdxFromStrIdx(nameStrIdx).Idx() != 0;
}

std::string MIRStructType::GetMplTypeName() const {
  std::stringstream ss;
  ss << "<$";
  ss << GlobalTables::GetStrTable().GetStringFromStrIdx(nameStrIdx);
  ss << ">";
  return ss.str();
}

bool MIRClassType::EqualTo(const MIRType &type) const {
  return (typeKind == type.GetKind() && nameStrIdx == type.GetNameStrIdx());
}

bool MIRInterfaceType::EqualTo(const MIRType &type) const {
  return (typeKind == type.GetKind() && nameStrIdx == type.GetNameStrIdx());
}

bool MIRTypeByName::EqualTo(const MIRType &type) const {
  return type.GetKind() == typeKind && type.GetNameStrIdx() == nameStrIdx && type.IsNameIsLocal() == nameIsLocal;
}

bool MIRTypeParam::EqualTo(const MIRType &type) const {
  return type.GetKind() == typeKind && type.GetNameStrIdx() == nameStrIdx;
}

bool MIRInstantVectorType::EqualTo(const MIRType &type) const {
  if (type.GetKind() != typeKind) {
    return false;
  }
  const auto &pty = static_cast<const MIRInstantVectorType&>(type);
  return (instantVec == pty.GetInstantVec());
}

bool MIRGenericInstantType::EqualTo(const MIRType &type) const {
  if (!MIRInstantVectorType::EqualTo(type)) {
    return false;
  }
  const auto &pType = static_cast<const MIRGenericInstantType&>(type);
  return genericTyIdx == pType.GetGenericTyIdx();
}

// in the search, curfieldid is being decremented until it reaches 1
FieldPair MIRStructType::TraverseToFieldRef(FieldID &fieldID) const {
  if (fields.empty()) {
    return FieldPair(GStrIdx(0), TyIdxFieldAttrPair(TyIdx(0), FieldAttrs()));
  }
  uint32 fieldIdx = 0;
  FieldPair curPair = fields[0];
  while (fieldID > 1) {
    --fieldID;
    MIRType *curFieldType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(curPair.second.first);
    MIRStructType *curFieldStructType = nullptr;
    switch (curFieldType->GetKind()) {
      case kTypeStruct:
      case kTypeStructIncomplete:
      case kTypeClass:
      case kTypeClassIncomplete:
      case kTypeInterface:
      case kTypeInterfaceIncomplete:
        curFieldStructType = static_cast<MIRStructType*>(curFieldType);
        curPair = curFieldStructType->TraverseToFieldRef(fieldID);
        if (fieldID == 1 && curPair.second.first != 0) {
          return curPair;
        }
        break;
      default:
        break;
    }
    ++fieldIdx;
    if (fieldIdx == fields.size()) {
      return FieldPair(GStrIdx(0), TyIdxFieldAttrPair(TyIdx(0), FieldAttrs()));
    }
    curPair = fields[fieldIdx];
  }
  return curPair;
}

FieldPair MIRStructType::TraverseToField(FieldID fieldID) const {
  if (fieldID >= 0) {
    return TraverseToFieldRef(fieldID);
  }
  // in parentfields
  uint32 parentFieldIdx = -fieldID;
  if (parentFields.empty() || parentFieldIdx > parentFields.size()) {
    return { GStrIdx(0), TyIdxFieldAttrPair(TyIdx(0), FieldAttrs()) };
  }
  return parentFields[parentFieldIdx - 1];
}

static bool TraverseToFieldInFields(const FieldVector &fields, const GStrIdx &fieldStrIdx, FieldPair &field) {
  for (auto &fp : fields) {
    if (fp.first == fieldStrIdx) {
      field = fp;
      return true;
    }
  }
  return false;
}

FieldPair MIRStructType::TraverseToField(GStrIdx fieldStrIdx) const {
  FieldPair fieldPair;
  if ((!fields.empty() && TraverseToFieldInFields(fields, fieldStrIdx, fieldPair)) ||
      (!staticFields.empty() && TraverseToFieldInFields(staticFields, fieldStrIdx, fieldPair)) ||
      TraverseToFieldInFields(parentFields, fieldStrIdx, fieldPair)) {
    return fieldPair;
  }
  return { GStrIdx(0), TyIdxFieldAttrPair(TyIdx(0), FieldAttrs()) };
}

bool MIRStructType::HasVolatileFieldInFields(const FieldVector &fieldsOfStruct) const {
  for (const auto &field : fieldsOfStruct) {
    if (field.second.second.GetAttr(FLDATTR_volatile) ||
        GlobalTables::GetTypeTable().GetTypeFromTyIdx(field.second.first)->HasVolatileField()) {
      hasVolatileField = true;
      return true;
    }
  }
  hasVolatileField = false;
  return false;
}

// go through all the fields to check if there is volatile attribute set;
bool MIRStructType::HasVolatileField() const {
  if (hasVolatileFieldSet) {
    return hasVolatileField;
  }
  hasVolatileFieldSet = true;
  return HasVolatileFieldInFields(fields) || HasVolatileFieldInFields(parentFields);
}

// set hasVolatileField to true if parent type has volatile field, otherwise flase.
static bool ParentTypeHasVolatileField(const TyIdx parentTyIdx, bool &hasVolatileField) {
  hasVolatileField = (GlobalTables::GetTypeTable().GetTypeFromTyIdx(parentTyIdx)->HasVolatileField());
  return hasVolatileField;
}

// go through all the fields to check if there is volatile attribute set;
bool MIRClassType::HasVolatileField() const {
  return MIRStructType::HasVolatileField() ||
         (parentTyIdx != 0 && ParentTypeHasVolatileField(parentTyIdx, hasVolatileField));
}

// go through all the fields to check if there is volatile attribute set;
bool MIRInterfaceType::HasVolatileField() const {
  if (MIRStructType::HasVolatileField()) {
    return true;
  }
  for (TyIdx parentTypeIdx : parentsTyIdx) {
    if (ParentTypeHasVolatileField(parentTypeIdx, hasVolatileField)) {
      return true;
    }
  }
  return false;
}

bool MIRStructType::HasTypeParamInFields(const FieldVector &fieldsOfStruct) const {
  for (const FieldPair &field : fieldsOfStruct) {
    if (field.second.second.GetAttr(FLDATTR_generic)) {
      return true;
    }
  }
  return false;
}

// go through all the fields to check if there is generic attribute set;
bool MIRStructType::HasTypeParam() const {
  return HasTypeParamInFields(fields) || HasTypeParamInFields(parentFields);
}

bool MIRClassType::HasTypeParam() const {
  return MIRStructType::HasTypeParam() ||
         (parentTyIdx != 0 && GlobalTables::GetTypeTable().GetTypeFromTyIdx(parentTyIdx)->HasTypeParam());
}

bool MIRInterfaceType::HasTypeParam() const {
  if (MIRStructType::HasTypeParam()) {
    return true;
  }
  // check if the parent classes have type parameter
  for (TyIdx parentTypeIdx : parentsTyIdx) {
    if (GlobalTables::GetTypeTable().GetTypeFromTyIdx(parentTypeIdx)->HasTypeParam()) {
      return true;
    }
  }
  return false;
}

FieldPair MIRClassType::TraverseToFieldRef(FieldID &fieldID) const {
  if (parentTyIdx != 0) {
    auto *parentClassType =
        static_cast<MIRClassType*>(GlobalTables::GetTypeTable().GetTypeFromTyIdx(parentTyIdx));
    if (parentClassType != nullptr) {
      --fieldID;
      const FieldPair &curPair = parentClassType->TraverseToFieldRef(fieldID);
      if (fieldID == 1 && curPair.second.first != 0) {
        return curPair;
      }
    }
  }
  return MIRStructType::TraverseToFieldRef(fieldID);
}

// fields in interface are all static and are global, won't be accessed through fields
FieldPair MIRInterfaceType::TraverseToFieldRef(FieldID &fieldID) const {
  return { GStrIdx(0), TyIdxFieldAttrPair(TyIdx(0), FieldAttrs()) };
}

bool MIRPtrType::IsPointedTypeVolatile(int fieldID) const {
  MIRType *pointedTy = GlobalTables::GetTypeTable().GetTypeFromTyIdx(GetPointedTyIdx());
  return pointedTy->IsVolatile(fieldID);
}

TyIdxFieldAttrPair MIRPtrType::GetPointedTyIdxFldAttrPairWithFieldID(FieldID fieldID) const {
  if (fieldID == 0) {
    return TyIdxFieldAttrPair(pointedTyIdx, FieldAttrs());
  }
  MIRType *pointedType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(pointedTyIdx);
  CHECK_FATAL(pointedType->IsStructType(),
      "MIRPtrType::GetPointedTyIdxWithFieldID(): cannot have non-zero fieldID for something other than a struct");
  auto *structType = static_cast<MIRStructType*>(pointedType);
  return structType->GetFieldTyIdxAttrPair(fieldID);
}

TyIdx MIRPtrType::GetPointedTyIdxWithFieldID(FieldID fieldID) const {
  return GetPointedTyIdxFldAttrPairWithFieldID(fieldID).first;
}

std::string MIRPtrType::GetMplTypeName() const {
  std::stringstream ss;
  ss << "<* ";
  MIRType *pointedType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(pointedTyIdx);
  CHECK_FATAL(pointedType != nullptr, "invalid ptr type");
  ss << pointedType->GetMplTypeName();
  ss << ">";
  return ss.str();
}

std::string MIRPtrType::GetCompactMplTypeName() const {
  MIRType *pointedType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(pointedTyIdx);
  CHECK_FATAL(pointedType != nullptr, "invalid ptr type");
  return pointedType->GetCompactMplTypeName();
}

TypeAttrs FieldAttrs::ConvertToTypeAttrs() {
  TypeAttrs attr;
  constexpr uint32 maxAttrNum = 64;
  for (uint32 i = 0; i < maxAttrNum; ++i) {
    if ((attrFlag & (1ULL << i)) == 0) {
      continue;
    }
    auto attrKind = static_cast<FieldAttrKind>(i);
    switch (attrKind) {
#define FIELD_ATTR
#define ATTR(STR)               \
    case FLDATTR_##STR:         \
      attr.SetAttr(ATTR_##STR); \
      break;
#include "all_attributes.def"
#undef ATTR
#undef FIELD_ATTR
      default:
        ASSERT(false, "unknown TypeAttrs");
        break;
    }
  }
  return attr;
}

TypeAttrs GenericAttrs::ConvertToTypeAttrs() {
  TypeAttrs attr;
  constexpr uint32 maxAttrNum = 64;
  for (uint32 i = 0; i < maxAttrNum; ++i) {
    if ((attrFlag & (1ULL << i)) == 0) {
      continue;
    }
    auto tA = static_cast<GenericAttrKind>(i);
    switch (tA) {
#define TYPE_ATTR
#define ATTR(STR)               \
    case GENATTR_##STR:         \
      attr.SetAttr(ATTR_##STR); \
      break;
#include "all_attributes.def"
#undef ATTR
#undef TYPE_ATTR
      default:
        ASSERT(false, "unknown TypeAttrs");
        break;
    }
  }
  return attr;
}

FuncAttrs GenericAttrs::ConvertToFuncAttrs() {
  FuncAttrs attr;
  constexpr uint32 maxAttrNum = 64;
  for (uint32 i = 0; i < maxAttrNum; ++i) {
    if ((attrFlag & (1ULL << i)) == 0) {
      continue;
    }
    auto tA = static_cast<GenericAttrKind>(i);
    switch (tA) {
#define FUNC_ATTR
#define ATTR(STR)                   \
    case GENATTR_##STR:             \
      attr.SetAttr(FUNCATTR_##STR); \
      break;
#include "all_attributes.def"
#undef ATTR
#undef FUNC_ATTR
      default:
        ASSERT(false, "unknown FuncAttrs");
        break;
    }
  }
  return attr;
}

FieldAttrs GenericAttrs::ConvertToFieldAttrs() {
  FieldAttrs attr;
  constexpr uint32 maxAttrNum = 64;
  for (uint32 i = 0; i < maxAttrNum; ++i) {
    if ((attrFlag & (1ULL << i)) == 0) {
      continue;
    }
    auto tA = static_cast<GenericAttrKind>(i);
    switch (tA) {
#define FIELD_ATTR
#define ATTR(STR)                  \
    case GENATTR_##STR:            \
      attr.SetAttr(FLDATTR_##STR); \
      break;
#include "all_attributes.def"
#undef ATTR
#undef FIELD_ATTR
      default:
        ASSERT(false, "unknown FieldAttrs");
        break;
    }
  }
  return attr;
}
}  // namespace maple
#endif  // MIR_FEATURE_FULL
