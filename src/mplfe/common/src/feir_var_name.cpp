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
#include "feir_var_name.h"

namespace maple {
// ---------- FEIRVarName ----------
std::string FEIRVarName::GetNameImpl(const MIRType &mirType) const {
  std::stringstream ss;
  ASSERT(nameIdx.GetIdx() != 0, "invalid name idx");
  std::string name = GlobalTables::GetStrTable().GetStringFromStrIdx(nameIdx);
  ss << name;
  if (withType) {
    ss << "_";
    if (type->IsPreciseRefType()) {
      ss << "R" << mirType.GetTypeIndex().GetIdx();
    } else {
      ss << GetPrimTypeName(type->GetPrimType());
    }
  }
  return ss.str();
}

std::string FEIRVarName::GetNameRawImpl() const {
  return GlobalTables::GetStrTable().GetStringFromStrIdx(nameIdx);
}

std::unique_ptr<FEIRVar> FEIRVarName::CloneImpl() const {
  std::unique_ptr<FEIRVar> var = std::make_unique<FEIRVarName>(nameIdx, type->Clone(), withType);
  var->SetGlobal(isGlobal);
  return var;
}

bool FEIRVarName::EqualsToImpl(const std::unique_ptr<FEIRVar> &var) const {
  if (var->GetKind() != kind) {
    return false;
  }
  FEIRVarName *ptrVarName = static_cast<FEIRVarName*>(var.get());
  ASSERT(ptrVarName != nullptr, "ptr var is nullptr");
  return ptrVarName->nameIdx == nameIdx;
}

size_t FEIRVarName::HashImpl() const {
  return std::hash<uint32>{}(nameIdx);
}

// ---------- FEIRVarNameSpec ----------
std::string FEIRVarNameSpec::GetNameImpl(const MIRType &mirType) const {
  std::stringstream ss;
  ss << name;
  if (withType) {
    ss << "_";
    if (type->IsPreciseRefType()) {
      ss << "R" << mirType.GetTypeIndex().GetIdx();
    } else {
      ss << GetPrimTypeName(type->GetPrimType());
    }
  }
  return ss.str();
}

std::string FEIRVarNameSpec::GetNameRawImpl() const {
  return name;
}

std::unique_ptr<FEIRVar> FEIRVarNameSpec::CloneImpl() const {
  std::unique_ptr<FEIRVar> var = std::make_unique<FEIRVarNameSpec>(name, type->Clone(), withType);
  var->SetGlobal(isGlobal);
  return var;
}

bool FEIRVarNameSpec::EqualsToImpl(const std::unique_ptr<FEIRVar> &var) const {
  if (var->GetKind() != kind) {
    return false;
  }
  FEIRVarNameSpec *ptrVarNameSpec = static_cast<FEIRVarNameSpec*>(var.get());
  ASSERT(ptrVarNameSpec != nullptr, "ptr var is nullptr");
  return ptrVarNameSpec->name.compare(name) == 0;
}

size_t FEIRVarNameSpec::HashImpl() const {
  return std::hash<std::string>{}(name);
}
}  // namespace maple