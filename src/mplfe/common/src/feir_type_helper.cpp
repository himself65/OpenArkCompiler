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
#include "feir_type_helper.h"
#include "fe_type_manager.h"

namespace maple {
UniqueFEIRType FEIRTypeHelper::CreateTypeByPrimType(PrimType primType, TypeDim dim, bool usePtr) {
  UniqueFEIRType type = std::make_unique<FEIRTypeDefault>(primType, GStrIdx(0), dim);
  if (usePtr) {
    return FEIRTypeHelper::CreatePointerType(std::move(type));
  } else {
    return type;
  }
}

UniqueFEIRType FEIRTypeHelper::CreateTypeByJavaName(const std::string typeName, bool inMpl, bool usePtr) {
  uint32 dim = 0;
  std::string baseName = FETypeManager::GetBaseTypeName(typeName, dim, inMpl);
  CHECK_FATAL(dim <= FEConstants::kDimMax, "invalid array type %s (dim is too big)", typeName.c_str());
  uint8 dim8 = static_cast<uint8>(dim);
  UniqueFEIRType newType;
  if (baseName.length() == 1) {
    newType = CreateTypeByJavaNamePrim(baseName[0], dim8);
    CHECK_FATAL(newType != nullptr, "unsupported java type name %s: ", typeName.c_str());
  } else if (baseName[0] == 'L') {
    baseName = inMpl ? baseName : NameMangler::EncodeName(baseName);
    GStrIdx typeNameIdx = GlobalTables::GetStrTable().GetStrIdxFromName(baseName);
    if (typeNameIdx == 0) {
      newType = std::make_unique<FEIRTypeByName>(PTY_ref, baseName, dim8);
    } else {
      newType = std::make_unique<FEIRTypeDefault>(PTY_ref, typeNameIdx, dim8);
    }
  } else {
    CHECK_FATAL(false, "unsupported java type name %s: ", typeName.c_str());
    return nullptr;
  }
  if (usePtr) {
    return FEIRTypeHelper::CreatePointerType(std::move(newType), PTY_ref);
  } else {
    return newType;
  }
}

UniqueFEIRType FEIRTypeHelper::CreatePointerType(UniqueFEIRType baseType, PrimType primType) {
  UniqueFEIRType newType = std::make_unique<FEIRTypePointer>(std::move(baseType), primType);
  return newType;
}

UniqueFEIRType FEIRTypeHelper::CreateTypeByJavaNamePrim(char primTypeFlag, uint8 dim8) {
  PrimType primType = PTY_unknown;
  switch (primTypeFlag) {
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
      return nullptr;
  }
  return std::make_unique<FEIRTypeDefault>(primType, GStrIdx(0), dim8);
}

UniqueFEIRType FEIRTypeHelper::CreateTypeByDimIncr(const UniqueFEIRType &srcType, uint8 delta, bool usePtr,
                                                   PrimType primType) {
  UniqueFEIRType type = srcType->Clone();
  (void)type->ArrayIncrDim(delta);
  if (!usePtr || type->GetKind() == FEIRTypeKind::kFEIRTypePointer) {
    return type;
  } else {
    return CreatePointerType(std::move(type), primType);
  }
}

UniqueFEIRType FEIRTypeHelper::CreateTypeByDimDecr(const UniqueFEIRType &srcType, uint8 delta) {
  UniqueFEIRType type = srcType->Clone();
  uint8 dim = type->ArrayDecrDim(delta);
  if (srcType->GetKind() == FEIRTypeKind::kFEIRTypePointer && dim == 0 && IsPrimitiveScalar(type->GetPrimType())) {
    const FEIRTypePointer *ptrType = static_cast<const FEIRTypePointer*>(type.get());
    ASSERT(ptrType != nullptr, "nullptr check");
    return ptrType->GetBaseType()->Clone();
  } else {
    return type;
  }
}

UniqueFEIRType FEIRTypeHelper::CreateTypeByGetAddress(const UniqueFEIRType &srcType, PrimType primType) {
  UniqueFEIRType type = std::make_unique<FEIRTypePointer>(srcType->Clone(), primType);
  return type;
}

UniqueFEIRType FEIRTypeHelper::CreateTypeByDereferrence(const UniqueFEIRType &srcType) {
  CHECK_FATAL(srcType->GetKind() == FEIRTypeKind::kFEIRTypePointer, "input is not pointer type");
  const FEIRTypePointer *ptrSrcType = static_cast<const FEIRTypePointer*>(srcType.get());
  ASSERT(ptrSrcType != nullptr, "nullptr check");
  return ptrSrcType->GetBaseType()->Clone();
}

UniqueFEIRType FEIRTypeHelper::CreateTypeDefault(PrimType primType, const GStrIdx &typeNameIdx, TypeDim dim) {
  UniqueFEIRType type = std::make_unique<FEIRTypeDefault>(primType, typeNameIdx, dim);
  CHECK_FATAL(type->IsValid(), "invalid type");
  return type;
}
}  // namespace maple