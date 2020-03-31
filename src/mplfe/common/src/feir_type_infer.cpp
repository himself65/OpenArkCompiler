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
#include "feir_type_infer.h"
#include "fe_type_hierarchy.h"
#include "feir_type_helper.h"
#include "feir_var_type_scatter.h"

namespace maple {
// ---------- FEIRTypeMergeHelper ----------
FEIRTypeMergeHelper::FEIRTypeMergeHelper()
    : firstType(true) {}

FEIRTypeMergeHelper::FEIRTypeMergeHelper(const UniqueFEIRType &argTypeDefault)
    : firstType(true) {
  CHECK_FATAL(argTypeDefault.get() != nullptr, "nullptr check");
  ResetTypeDefault(argTypeDefault);
}

void FEIRTypeMergeHelper::Reset() {
  firstType = true;
  type.reset();
  error = "";
}

void FEIRTypeMergeHelper::ResetTypeDefault(const UniqueFEIRType &argTypeDefault) {
  typeDefault = argTypeDefault->Clone();
}

bool FEIRTypeMergeHelper::MergeType(const UniqueFEIRType &argType, bool parent) {
  if (firstType) {
    type = argType->Clone();
    firstType = false;
    return true;
  }
  return MergeType(type, argType, parent);
}

UniqueFEIRType FEIRTypeMergeHelper::GetResult() const {
  return type->Clone();
}

void FEIRTypeMergeHelper::SetDefaultType(UniqueFEIRType &typeDst) {
  SetType(typeDst, typeDefault);
}

void FEIRTypeMergeHelper::SetType(UniqueFEIRType &typeDst, const UniqueFEIRType &typeSrc) {
  ASSERT(typeDst.get() != nullptr, "nullptr check for type dst");
  ASSERT(typeSrc.get() != nullptr, "nullptr check for type src");
  if (typeDst.get()->GetKind() == typeSrc.get()->GetKind()) {
    typeDst.get()->CopyFrom(*(typeSrc.get()));
  } else {
    typeDst = typeSrc->Clone();
  }
}

bool FEIRTypeMergeHelper::MergeType(UniqueFEIRType &typeDst, const UniqueFEIRType &typeSrc, bool parent) {
  ASSERT(typeDst.get() != nullptr, "nullptr check for type dst");
  ASSERT(typeSrc.get() != nullptr, "nullptr check for type src");
  ASSERT(typeDefault.get() != nullptr, "nullptr check for type default");
  FEIRTypeKind kindDst = typeDst.get()->GetKind();
  FEIRTypeKind kindSrc = typeSrc.get()->GetKind();
  // boundary: equal
  if (typeDst->IsEqualTo(typeSrc)) {
    return true;
  }
  // boundary: zero
  if (typeSrc->IsZero()) {
    return true;
  }
  if (typeDst->IsZero()) {
    SetType(typeDst, typeSrc);
    return true;
  }
  if (kindDst == FEIRTypeKind::kFEIRTypePointer && kindSrc == FEIRTypeKind::kFEIRTypePointer) {
    FEIRTypePointer *ptrTypeDst = static_cast<FEIRTypePointer*>(typeDst.get());
    FEIRTypePointer *ptrTypeSrc = static_cast<FEIRTypePointer*>(typeSrc.get());
    UniqueFEIRType mergedType = ptrTypeDst->Clone();
    bool success = MergeType(mergedType, ptrTypeSrc->GetBaseType(), parent);
    if (success) {
      ptrTypeDst->SetBaseType(std::move(mergedType));
    }
    return success;
  }
  if (kindDst == FEIRTypeKind::kFEIRTypePointer && kindSrc != FEIRTypeKind::kFEIRTypePointer) {
    return false;
  }
  if (kindDst != FEIRTypeKind::kFEIRTypePointer && kindSrc == FEIRTypeKind::kFEIRTypePointer) {
    return false;
  }
  // boundary: default
  if (typeSrc->IsEqualTo(typeDefault)) {
    if (parent) {
      SetDefaultType(typeDst);
    }
    return true;
  }
  if (typeDst->IsEqualTo(typeDefault)) {
    if (!parent) {
      SetType(typeDst, typeSrc);
    }
    return true;
  }
  // hierarchy check
  FEIRTypeDefault *ptrTypeDst = static_cast<FEIRTypeDefault*>(typeDst.get());
  FEIRTypeDefault *ptrTypeSrc = static_cast<FEIRTypeDefault*>(typeSrc.get());
  GStrIdx idxDst = ptrTypeDst->GetTypeNameIdx();
  GStrIdx idxSrc = ptrTypeSrc->GetTypeNameIdx();
  if (ptrTypeDst->GetDim() != ptrTypeSrc->GetDim()) {
    return false;
  }
  if (ptrTypeDst->GetPrimType() != ptrTypeSrc->GetPrimType()) {
    return false;
  }
  if (FETypeHierarchy::GetInstance().IsParentOf(idxDst, idxSrc)) {
    // dst is parent of src
    if (!parent) {
      (void)ptrTypeDst->SetTypeNameIdx(idxSrc);
    }
  } else if (FETypeHierarchy::GetInstance().IsParentOf(idxSrc, idxDst)) {
    // src is parent of dst
    if (parent) {
      (void)ptrTypeDst->SetTypeNameIdx(idxSrc);
    }
  } else {
    if (parent) {
      SetDefaultType(typeDst);
    } else {
      return false;
    }
  }
  return true;
}

// ---------- FEIRTypeInfer ----------
FEIRTypeInfer::FEIRTypeInfer(MIRSrcLang argSrcLang,
                             const std::map<const UniqueFEIRVar*, std::set<UniqueFEIRVar*>> &argMapDefUse)
    : srcLang(argSrcLang),
      mapDefUse(argMapDefUse) {
  LoadTypeDefault();
}

void FEIRTypeInfer::LoadTypeDefault() {
  switch (srcLang) {
    case kSrcLangJava:
      typeDefault = FEIRTypeHelper::CreateTypeByJavaName("Ljava/lang/Object;", false, false);
      mergeHelper.ResetTypeDefault(typeDefault);
      break;
    default:
      typeDefault.reset();
      break;
  }
}

void FEIRTypeInfer::Reset() {
  visitVars.clear();
  withCircle = false;
  first = true;
}

UniqueFEIRType FEIRTypeInfer::GetTypeForVarUse(const UniqueFEIRVar &varUse) {
  CHECK_NULL_FATAL(varUse);
  if (varUse->GetType()->IsPreciseRefType()) {
    return varUse->GetType()->Clone();
  }
  if (varUse->GetTrans().get() == nullptr) {
    return typeDefault->Clone();
  }
  if (visitVars.find(&varUse) != visitVars.end()) {
    withCircle = true;
    return UniqueFEIRType();
  }
  CHECK_FATAL(visitVars.insert(&varUse).second, "visitVars insert failed");
  bool isFirst = first;
  first = false;
  UniqueFEIRType ans = GetTypeByTransForVarUse(varUse);
  if ((isFirst || (!withCircle)) && ans != nullptr) {
    varUse->SetType(ans->Clone());
    return ans->Clone();
  } else {
    return UniqueFEIRType();
  }
}

UniqueFEIRType FEIRTypeInfer::GetTypeForVarDef(const UniqueFEIRVar &varDef) {
  CHECK_NULL_FATAL(varDef);
  if (varDef->GetType()->IsPreciseRefType()) {
    return varDef->GetType()->Clone();
  }
  if (varDef->GetType()->IsZero()) {
      return UniqueFEIRType();
  }
  if (visitVars.find(&varDef) != visitVars.end()) {
    withCircle = true;
    return UniqueFEIRType();
  }
  CHECK_FATAL(visitVars.insert(&varDef).second, "visitVars insert failed");
  auto it = mapDefUse.find(&varDef);
  CHECK_FATAL(it != mapDefUse.end(), "use not found");
  std::unordered_set<FEIRTypeKey, FEIRTypeKeyHash> useTypes;
  for (const UniqueFEIRVar *use : it->second) {
    CHECK_NULL_FATAL(use);
    UniqueFEIRType useType = GetTypeForVarUse(*use);
    if (useType != nullptr && (!useType->IsEqualTo(varDef->GetType()))) {
      FEIRTypeKey key(useType);
      if (useTypes.find(key) == useTypes.end()) {
        CHECK_FATAL(useTypes.insert(key).second, "useTypes insert failed");
      }
    }
  }
  if (useTypes.size() > 0) {
    FEIRTypeMergeHelper mh(typeDefault);
    for (const FEIRTypeKey &typeKey : useTypes) {
      UniqueFEIRType defType = typeKey.GetType()->Clone();
      bool success = mh.MergeType(defType, false);
      CHECK_FATAL(success, "merge type error");
    }
    return mh.GetTypeClone();
  } else {
    return UniqueFEIRType();
  }
}

void FEIRTypeInfer::ProcessVarDef(UniqueFEIRVar &varDef) {
  CHECK_NULL_FATAL(varDef);
  auto it = mapDefUse.find(&varDef);
  if (it == mapDefUse.end()) {
    return;
  }
  std::unordered_set<FEIRTypeKey, FEIRTypeKeyHash> useTypes;
  for (const UniqueFEIRVar *use : it->second) {
    CHECK_NULL_FATAL(use);
    UniqueFEIRType useType = GetTypeForVarUse(*use);
    if (useType != nullptr && (!useType->IsEqualTo(varDef->GetType()))) {
      FEIRTypeKey key(useType);
      if (useTypes.find(key) == useTypes.end()) {
        CHECK_FATAL(useTypes.insert(key).second, "useTypes insert failed");
      }
    }
  }
  if (useTypes.size() == 1 && !(varDef->GetType()->IsPreciseType())) {
    varDef->SetType((*(useTypes.begin())).GetType()->Clone());
    return;
  }
  if (useTypes.size() > 0) {
    UniqueFEIRVar varNew = std::make_unique<FEIRVarTypeScatter>(UniqueFEIRVar(varDef->Clone()));
    FEIRVarTypeScatter *ptrVarNew = static_cast<FEIRVarTypeScatter*>(varNew.get());
    varDef->SetType(varDef->GetType()->Clone());
    for (const FEIRTypeKey &typeKey : useTypes) {
      ptrVarNew->AddScatterType(typeKey.GetType());
    }
    varDef.reset(varNew.release());
  }
}

UniqueFEIRType FEIRTypeInfer::GetTypeByTransForVarUse(const UniqueFEIRVar &varUse) {
  CHECK_NULL_FATAL(varUse);
  FEIRVarTrans *ptrVarUseTrans = varUse->GetTrans().get();
  CHECK_NULL_FATAL(ptrVarUseTrans);
  const UniqueFEIRVar &varDef = ptrVarUseTrans->GetVar();
  CHECK_NULL_FATAL(varDef);
  UniqueFEIRType defType = GetTypeForVarDef(varDef);
  if (defType != nullptr) {
    return ptrVarUseTrans->GetType(defType);
  } else {
    return defType;
  }
}
}  // namespace maple
