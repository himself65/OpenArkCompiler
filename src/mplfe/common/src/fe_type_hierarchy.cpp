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
#include "fe_type_hierarchy.h"
#include "global_tables.h"
#include "fe_config_parallel.h"

namespace maple {
FETypeHierarchy FETypeHierarchy::instance;

void FETypeHierarchy::InitByGlobalTable() {
  MPLFE_PARALLEL_FORBIDDEN();
  mapIdxChildParent.clear();
  for (const MIRType *type : GlobalTables::GetTypeTable().GetTypeTable()) {
    if (type == nullptr) {
      continue;
    }
    switch (type->GetKind()) {
      case kTypeClass: {
        const MIRClassType *typeClass = static_cast<const MIRClassType*>(type);
        AddMIRType(*typeClass);
        break;
      }
      case kTypeInterface: {
        const MIRInterfaceType *typeInterface = static_cast<const MIRInterfaceType*>(type);
        AddMIRType(*typeInterface);
        break;
      }
      default:
        break;
    }
  }
}

void FETypeHierarchy::AddMIRType(const MIRClassType &type) {
  MIRType *parentType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(type.GetParentTyIdx());
  if (parentType != nullptr) {
    CHECK_FATAL(parentType->IsStructType(), "parent must be struct type");
    AddParentChildRelation(parentType->GetNameStrIdx(), type.GetNameStrIdx());
  }
  for (TyIdx tyIdx : type.GetInterfaceImplemented()) {
    MIRType *implType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(tyIdx);
    CHECK_FATAL(implType->IsStructType(), "parent must be struct type");
    AddParentChildRelation(implType->GetNameStrIdx(), type.GetNameStrIdx());
  }
}

void FETypeHierarchy::AddMIRType(const MIRInterfaceType &type) {
  for (TyIdx tyIdx : type.GetParentsTyIdx()) {
    MIRType *parentType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(tyIdx);
    CHECK_FATAL(parentType->IsStructType(), "parent must be struct type");
    AddParentChildRelation(parentType->GetNameStrIdx(), type.GetNameStrIdx());
  }
}

bool FETypeHierarchy::IsParentOf(const GStrIdx &parentIdx, const GStrIdx &childIdx) {
  if (parentIdx == childIdx) {
    return true;
  }
  if (cache.find(std::make_pair(childIdx, parentIdx)) != cache.end()) {
    return true;
  }
  auto it = mapIdxChildParent.find(childIdx);
  if (it == mapIdxChildParent.end()) {
    return false;
  }
  for (GStrIdx idx : it->second) {
    if (IsParentOf(parentIdx, idx)) {
      std::pair<GStrIdx, GStrIdx> item = std::make_pair(childIdx, parentIdx);
      if (cache.find(item) == cache.end()) {
        CHECK_FATAL(cache.insert(item).second, "cache insert failed");
      }
      return true;
    }
  }
  return false;
}

void FETypeHierarchy::AddParentChildRelation(const GStrIdx &parentIdx, const GStrIdx &childIdx) {
  if (mapIdxChildParent[childIdx].find(parentIdx) == mapIdxChildParent[childIdx].end()) {
    CHECK_FATAL(mapIdxChildParent[childIdx].insert(parentIdx).second, "mapIdxChildParent insert failed");
  }
}
}  // namespace maple
