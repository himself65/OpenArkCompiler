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
#include "feir_var_type_scatter.h"

namespace maple {
FEIRVarTypeScatter::FEIRVarTypeScatter(UniqueFEIRVar argVar)
    : FEIRVar(FEIRVarKind::kFEIRVarTypeScatter) {
  ASSERT(argVar != nullptr, "nullptr check");
  ASSERT(argVar->GetKind() != FEIRVarKind::kFEIRVarTypeScatter, "invalid input var type");
  var = std::move(argVar);
}

void FEIRVarTypeScatter::AddScatterType(const UniqueFEIRType &type) {
  if (var->GetType()->IsEqualTo(type)) {
    return;
  }
  FEIRTypeKey key(type);
  if (scatterTypes.find(key) == scatterTypes.end()) {
    CHECK_FATAL(scatterTypes.insert(key).second, "scatterTypes insert failed");
  }
}

std::string FEIRVarTypeScatter::GetNameImpl(const MIRType &mirType) const {
  return var->GetName(mirType);
}

std::string FEIRVarTypeScatter::GetNameRawImpl() const {
  return var->GetNameRaw();
}

std::unique_ptr<FEIRVar> FEIRVarTypeScatter::CloneImpl() const {
  std::unique_ptr<FEIRVar> ans = std::make_unique<FEIRVarTypeScatter>(var->Clone());
  FEIRVarTypeScatter *ptrAns = static_cast<FEIRVarTypeScatter*>(ans.get());
  ASSERT(ptrAns != nullptr, "nullptr check");
  for (const FEIRTypeKey &key : scatterTypes) {
    ptrAns->AddScatterType(key.GetType());
  }
  return std::unique_ptr<FEIRVar>(ans.release());
}

bool FEIRVarTypeScatter::EqualsToImpl(const std::unique_ptr<FEIRVar> &argVar) const {
  return false;
}

size_t FEIRVarTypeScatter::HashImpl() const {
  return 0;
}
}  // namespace maple