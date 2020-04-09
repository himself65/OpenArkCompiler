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
#include "feir_var.h"
#include "global_tables.h"
#include "feir_type_helper.h"
#include "fe_config_parallel.h"

namespace maple {
// ---------- FEIRVarTrans ----------
FEIRVarTrans::FEIRVarTrans(FEIRVarTransKind argKind, std::unique_ptr<FEIRVar> &argVar)
    : kind(argKind), var(argVar) {
  param.dimDelta = 0;
  switch (kind) {
    case kFEIRVarTransArrayDimIncr:
    case kFEIRVarTransArrayDimDecr:
      param.dimDelta = 1;
      break;
    default:
      break;
  }
}

FEIRVarTrans::FEIRVarTrans(FEIRVarTransKind argKind, std::unique_ptr<FEIRVar> &argVar, uint8 dimDelta)
    : kind(argKind), var(argVar) {
  param.dimDelta = 0;
  switch (kind) {
    case kFEIRVarTransArrayDimIncr:
    case kFEIRVarTransArrayDimDecr:
      param.dimDelta = dimDelta;
      break;
    default:
      break;
  }
}

UniqueFEIRType FEIRVarTrans::GetType(const UniqueFEIRType &type, PrimType primType, bool usePtr) {
  ASSERT(type != nullptr, "nullptr check");
  UniqueFEIRType typeNew = type->Clone();
  switch (kind) {
    case kFEIRVarTransDirect:
      break;
    case kFEIRVarTransArrayDimIncr: {
      (void)typeNew->ArrayIncrDim(param.dimDelta);
      if (typeNew->GetKind() != FEIRTypeKind::kFEIRTypePointer && usePtr) {
        return FEIRTypeHelper::CreatePointerType(typeNew->Clone(), primType);
      } else {
        return typeNew;
      }
    }
    case kFEIRVarTransArrayDimDecr: {
      (void)typeNew->ArrayDecrDim(param.dimDelta);
      if (typeNew->GetKind() == FEIRTypeKind::kFEIRTypePointer) {
        FEIRTypePointer *ptrTypeNew = static_cast<FEIRTypePointer*>(typeNew.get());
        ASSERT(ptrTypeNew->GetBaseType() != nullptr, "nullptr check");
        if (ptrTypeNew->GetBaseType()->IsScalar()) {
          return ptrTypeNew->GetBaseType()->Clone();
        }
      } else if (usePtr && !typeNew->IsScalar()) {
        return FEIRTypeHelper::CreatePointerType(typeNew->Clone(), primType);
      } else {
        return typeNew;
      }
      break;
    }
    default:
      CHECK_FATAL(false, "unsupported trans kind");
  }
  return typeNew;
}

// ---------- FEIRVar ----------
FEIRVar::FEIRVar(FEIRVarKind argKind)
    : kind(argKind),
      isGlobal(false),
      isDef(false),
      type(std::make_unique<FEIRTypeDefault>()) {}

FEIRVar::FEIRVar(FEIRVarKind argKind, std::unique_ptr<FEIRType> argType)
    : FEIRVar(argKind) {
  SetType(std::move(argType));
}

MIRSymbol *FEIRVar::GenerateGlobalMIRSymbolImpl(MIRBuilder &builder) const {
  MPLFE_PARALLEL_FORBIDDEN();
  MIRType *mirType = type->GenerateMIRType();
  CHECK_NULL_FATAL(mirType);
  MIRType *mirPtrType = (type->IsPreciseRefType()) ?
      GlobalTables::GetTypeTable().GetOrCreatePointerType(*mirType) : mirType;
  CHECK_NULL_FATAL(mirPtrType);
  std::string name = GetName(*mirPtrType);
  return builder.GetOrCreateGlobalDecl(name, *mirPtrType);
}

MIRSymbol *FEIRVar::GenerateLocalMIRSymbolImpl(MIRBuilder &builder) const {
  MPLFE_PARALLEL_FORBIDDEN();
  MIRType *mirType = type->GenerateMIRType();
  CHECK_NULL_FATAL(mirType);
  MIRType *mirPtrType = (type->IsPreciseRefType()) ?
      GlobalTables::GetTypeTable().GetOrCreatePointerType(*mirType) : mirType;
  CHECK_NULL_FATAL(mirPtrType);
  std::string name = GetName(*mirPtrType);
  return builder.GetOrCreateLocalDecl(name, *mirPtrType);
}

MIRSymbol *FEIRVar::GenerateMIRSymbolImpl(MIRBuilder &builder) const {
  if (isGlobal) {
    return GenerateGlobalMIRSymbol(builder);
  } else {
    return GenerateLocalMIRSymbol(builder);
  }
}

void FEIRVar::SetType(std::unique_ptr<FEIRType> argType) {
  CHECK_FATAL(argType != nullptr, "input type is nullptr");
  type = std::move(argType);
}
}  // namespace maple
