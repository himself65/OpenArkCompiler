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
#include "feir_type.h"
#include <functional>
#include "global_tables.h"
#include "mpl_logging.h"
#include "fe_manager.h"
#include "fe_config_parallel.h"

namespace maple {
// ---------- FEIRType ----------
FEIRType::FEIRType(FEIRTypeKind argKind)
    : kind(argKind), isZero(false), primType(PTY_void) {}

FEIRType::FEIRType(FEIRTypeKind argKind, PrimType argPrimType)
    : kind(argKind), isZero(false), primType(argPrimType) {}

void FEIRType::CopyFromImpl(const FEIRType &type) {
  kind = type.kind;
  primType = type.primType;
}

bool FEIRType::IsEqualToImpl(const std::unique_ptr<FEIRType> &argType) const {
  ASSERT(argType != nullptr, "nullptr check");
  if (kind != argType->kind) {
    return false;
  }
  return IsEqualTo(*(argType.get()));
}

bool FEIRType::IsEqualToImpl(const FEIRType &argType) const {
  if (kind == argType.kind && isZero == argType.isZero && primType == argType.primType) {
    return true;
  } else {
    return false;
  }
}

std::unique_ptr<FEIRType> FEIRType::NewType(FEIRTypeKind argKind) {
  switch (argKind) {
    case kFEIRTypeDefault:
      return std::make_unique<FEIRTypeDefault>();
    default:
      CHECK_FATAL(false, "unsupported FEIRType Kind");
      return std::make_unique<FEIRTypeDefault>();
  }
}

// ---------- FEIRTypeDefault ----------
FEIRTypeDefault::FEIRTypeDefault()
    : FEIRTypeDefault(PTY_void, GStrIdx(0), 0) {}

FEIRTypeDefault::FEIRTypeDefault(PrimType argPrimType)
    : FEIRTypeDefault(argPrimType, GStrIdx(0), 0) {}

FEIRTypeDefault::FEIRTypeDefault(PrimType argPrimType, const GStrIdx &argTypeNameIdx)
    : FEIRTypeDefault(argPrimType, argTypeNameIdx, 0) {}

FEIRTypeDefault::FEIRTypeDefault(PrimType argPrimType, const GStrIdx &argTypeNameIdx, uint8 argDim)
    : FEIRType(kFEIRTypeDefault, argPrimType),
      typeNameIdx(argTypeNameIdx),
      dim(argDim) {}

void FEIRTypeDefault::CopyFromImpl(const FEIRType &type) {
  CHECK_FATAL(type.GetKind() == kFEIRTypeDefault, "invalid FEIRType Kind");
  FEIRType::CopyFromImpl(type);
  const FEIRTypeDefault &typeDefault = static_cast<const FEIRTypeDefault&>(type);
  typeNameIdx = typeDefault.typeNameIdx;
  dim = typeDefault.dim;
}

std::unique_ptr<FEIRType> FEIRTypeDefault::CloneImpl() const {
  std::unique_ptr<FEIRType> type = std::make_unique<FEIRTypeDefault>(primType, typeNameIdx, dim);
  return type;
}

MIRType *FEIRTypeDefault::GenerateMIRTypeImpl(MIRSrcLang srcLang, bool usePtr) const {
  MPLFE_PARALLEL_FORBIDDEN();
  return GenerateMIRType(usePtr);
}

MIRType *FEIRTypeDefault::GenerateMIRTypeImpl(bool usePtr) const {
  MPLFE_PARALLEL_FORBIDDEN();
  return GenerateMIRTypeInternal(typeNameIdx, usePtr);
}

MIRType *FEIRTypeDefault::GenerateMIRTypeImpl() const {
  MPLFE_PARALLEL_FORBIDDEN();
  return GenerateMIRType(false);
}

uint8 FEIRTypeDefault::ArrayIncrDimImpl(uint8 delta) {
  CHECK_FATAL(kDimMax - dim >= delta, "dim delta is too large");
  dim += delta;
  return dim;
}

uint8 FEIRTypeDefault::ArrayDecrDimImpl(uint8 delta) {
  CHECK_FATAL(dim >= delta, "dim delta is too large");
  dim -= delta;
  return dim;
}

bool FEIRTypeDefault::IsEqualToImpl(const FEIRType &argType) const {
  if (!FEIRType::IsEqualToImpl(argType)) {
    return false;
  }
  const FEIRTypeDefault &argTypeDefault = static_cast<const FEIRTypeDefault&>(argType);
  if (typeNameIdx == argTypeDefault.typeNameIdx && dim == argTypeDefault.dim) {
    return true;
  } else {
    return false;
  }
}

size_t FEIRTypeDefault::HashImpl() const {
  return std::hash<uint32>{}(typeNameIdx);
}

bool FEIRTypeDefault::IsPreciseRefTypeImpl() const {
  return primType == PTY_ref && typeNameIdx.GetIdx() != 0;
}

bool FEIRTypeDefault::IsPreciseTypeImpl() const {
  return (primType != PTY_ref && IsPrimitiveInteger(primType)) ||
         IsPrimitiveFloat(primType) ||
         (primType == PTY_ref && typeNameIdx != 0) ||
         isZero;
}

bool FEIRTypeDefault::IsScalarImpl() const {
  return (primType != PTY_ref && IsPrimitiveScalar(primType) && dim == 0);
}

void FEIRTypeDefault::LoadFromJavaTypeName(const std::string &typeName, bool inMpl) {
  MPLFE_PARALLEL_FORBIDDEN();
  uint32 dimLocal = 0;
  std::string baseName = FETypeManager::GetBaseTypeName(typeName, dimLocal, inMpl);
  CHECK_FATAL(dimLocal <= kDimMax, "invalid array type %s (dim is too big)", typeName.c_str());
  dim = static_cast<uint8>(dimLocal);
  if (baseName.length() == 1) {
    typeNameIdx = GStrIdx(0);
    switch (baseName[0]) {
      case 'I':
        primType = PTY_i32;
        break;
      case 'J':
        primType = PTY_i64;
        break;
      case 'F':
        primType = PTY_f32;
        break;
      case 'D':
        primType = PTY_f64;
        break;
      case 'Z':
        primType = PTY_u1;
        break;
      case 'B':
        primType = PTY_i8;
        break;
      case 'S':
        primType = PTY_i16;
        break;
      case 'C':
        primType = PTY_u16;
        break;
      case 'V':
        primType = PTY_void;
        break;
      default:
        CHECK_FATAL(false, "unsupported java type %s", typeName.c_str());
    }
  } else if (baseName[0] == 'L') {
    primType = PTY_ref;
    baseName = inMpl ? baseName : NameMangler::EncodeName(baseName);
    typeNameIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(baseName);
  }
}

MIRType *FEIRTypeDefault::GenerateMIRTypeForPrim() const {
  switch (primType) {
    case PTY_i32:
      return GlobalTables::GetTypeTable().GetInt32();
    case PTY_i64:
      return GlobalTables::GetTypeTable().GetInt64();
    case PTY_f32:
      return GlobalTables::GetTypeTable().GetFloat();
    case PTY_f64:
      return GlobalTables::GetTypeTable().GetDouble();
    case PTY_u1:
      return GlobalTables::GetTypeTable().GetUInt1();
    case PTY_i8:
      return GlobalTables::GetTypeTable().GetInt8();
    case PTY_i16:
      return GlobalTables::GetTypeTable().GetInt16();
    case PTY_u16:
      return GlobalTables::GetTypeTable().GetUInt16();
    case PTY_void:
      return GlobalTables::GetTypeTable().GetVoid();
    default:
      CHECK_FATAL(false, "unsupported prim type");
  }
  return GlobalTables::GetTypeTable().GetDynundef();
}

MIRType *FEIRTypeDefault::GenerateMIRTypeInternal(const GStrIdx &argTypeNameIdx, bool usePtr) const {
  MIRType *baseType = nullptr;
  MIRType *type = nullptr;
  if (primType == PTY_ref) {
    if (argTypeNameIdx.GetIdx() == 0) {
      baseType = GlobalTables::GetTypeTable().GetRef();
    } else {
      bool isCreate = false;
      baseType = FEManager::GetTypeManager().GetOrCreateClassOrInterfaceType(GStrIdx(argTypeNameIdx), false,
                                                                             FETypeFlag::kSrcUnknown, isCreate);
    }
    if (dim > 0) {
      baseType = FEManager::GetTypeManager().GetOrCreatePointerType(*baseType);
    }
    type = FEManager::GetTypeManager().GetOrCreateArrayType(*baseType, dim);
  } else {
    baseType = GenerateMIRTypeForPrim();
    type = FEManager::GetTypeManager().GetOrCreateArrayType(*baseType, dim);
  }
  if (primType != PTY_ref || argTypeNameIdx.GetIdx() == 0) {
    return type;
  }
  return usePtr ? FEManager::GetTypeManager().GetOrCreatePointerType(*type) : type;
}

// ---------- FEIRTypeByName ----------
FEIRTypeByName::FEIRTypeByName(PrimType argPrimType, const std::string &argTypeName, uint8 argDim)
    : FEIRTypeDefault(argPrimType, GStrIdx(0), argDim),
      typeName(argTypeName) {
  kind = kFEIRTypeByName;
}

std::unique_ptr<FEIRType> FEIRTypeByName::CloneImpl() const {
  std::unique_ptr<FEIRType> newType = std::make_unique<FEIRTypeByName>(primType, typeName, dim);
  return newType;
}

MIRType *FEIRTypeByName::GenerateMIRTypeImpl(MIRSrcLang srcLang, bool usePtr) const {
  MPLFE_PARALLEL_FORBIDDEN();
  GStrIdx argTypeNameIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(typeName.c_str());
  return GenerateMIRTypeInternal(argTypeNameIdx, usePtr);
}

MIRType *FEIRTypeByName::GenerateMIRTypeImpl(bool usePtr) const {
  MPLFE_PARALLEL_FORBIDDEN();
  GStrIdx argTypeNameIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(typeName.c_str());
  return GenerateMIRTypeInternal(argTypeNameIdx, usePtr);
}

MIRType *FEIRTypeByName::GenerateMIRTypeImpl() const {
  MPLFE_PARALLEL_FORBIDDEN();
  GStrIdx argTypeNameIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(typeName.c_str());
  return GenerateMIRTypeInternal(argTypeNameIdx, false);
}

bool FEIRTypeByName::IsEqualToImpl(const FEIRType &argType) const {
  if (!FEIRTypeDefault::IsEqualTo(argType)) {
    return false;
  }
  const FEIRTypeByName &argTypeName = static_cast<const FEIRTypeByName&>(argType);
  if (typeName.compare(argTypeName.typeName) == 0) {
    return true;
  } else {
    return false;
  }
}

size_t FEIRTypeByName::HashImpl() const {
  return std::hash<std::string>{}(typeName);
}

bool FEIRTypeByName::IsPreciseRefTypeImpl() const {
  return primType == PTY_ref && (!typeName.empty());
}

bool FEIRTypeByName::IsPreciseTypeImpl() const {
  return IsPrimitiveInteger(primType) || IsPrimitiveFloat(primType) ||
         (primType == PTY_ref && (!typeName.empty()));
}

bool FEIRTypeByName::IsScalarImpl() const {
  return false;
}

// ---------- FEIRTypePointer ----------
FEIRTypePointer::FEIRTypePointer(std::unique_ptr<FEIRType> argBaseType, PrimType argPrimType)
    : FEIRType(kFEIRTypePointer, argPrimType) {
  CHECK_FATAL(argBaseType != nullptr, "input type is nullptr");
  baseType = std::move(argBaseType);
}

std::unique_ptr<FEIRType> FEIRTypePointer::CloneImpl() const {
  std::unique_ptr<FEIRType> newType = std::make_unique<FEIRTypePointer>(baseType->Clone(), primType);
  return newType;
}

MIRType *FEIRTypePointer::GenerateMIRTypeImpl(MIRSrcLang srcLang, bool usePtr) const {
  ASSERT(baseType != nullptr, "base type is nullptr");
  MIRType *mirBaseType = baseType->GenerateMIRType(srcLang, usePtr);
  ASSERT(mirBaseType != nullptr, "base mir type is nullptr");
  return GlobalTables::GetTypeTable().GetOrCreatePointerType(*mirBaseType, primType);
}

MIRType *FEIRTypePointer::GenerateMIRTypeImpl(bool usePtr) const {
  ASSERT(baseType != nullptr, "base type is nullptr");
  MIRType *mirBaseType = baseType->GenerateMIRType(usePtr);
  ASSERT(mirBaseType != nullptr, "base mir type is nullptr");
  return GlobalTables::GetTypeTable().GetOrCreatePointerType(*mirBaseType, primType);
}

MIRType *FEIRTypePointer::GenerateMIRTypeImpl() const {
  ASSERT(baseType != nullptr, "base type is nullptr");
  MIRType *mirBaseType = baseType->GenerateMIRType();
  ASSERT(mirBaseType != nullptr, "base mir type is nullptr");
  return GlobalTables::GetTypeTable().GetOrCreatePointerType(*mirBaseType, primType);
}

bool FEIRTypePointer::IsEqualToImpl(const FEIRType &argType) const {
  const FEIRTypePointer &argTypePointer = static_cast<const FEIRTypePointer&>(argType);
  return baseType->IsEqualTo(argTypePointer.baseType);
}

size_t FEIRTypePointer::HashImpl() const {
  ASSERT(baseType != nullptr, "base type is nullptr");
  return baseType->Hash();
}

bool FEIRTypePointer::IsPreciseRefTypeImpl() const {
  ASSERT(baseType != nullptr, "base type is nullptr");
  return baseType->IsPreciseRefType();
}

bool FEIRTypePointer::IsPreciseTypeImpl() const {
  ASSERT(baseType != nullptr, "base type is nullptr");
  return baseType->IsPreciseType();
}

bool FEIRTypePointer::IsScalarImpl() const {
  return false;
}

uint8 FEIRTypePointer::ArrayIncrDimImpl(uint8 delta) {
  ASSERT(baseType != nullptr, "base type is nullptr");
  return baseType->ArrayIncrDim(delta);
}

uint8 FEIRTypePointer::ArrayDecrDimImpl(uint8 delta) {
  ASSERT(baseType != nullptr, "base type is nullptr");
  return baseType->ArrayDecrDim(delta);
}
}  // namespace maple
