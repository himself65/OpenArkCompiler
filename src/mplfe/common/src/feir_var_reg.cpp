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
#include "feir_var_reg.h"
#include <sstream>
#include <functional>
#include "mir_type.h"

namespace maple {
std::string FEIRVarReg::GetNameImpl(const MIRType &mirType) const {
  std::stringstream ss;
  ss << "Reg" << regNum << "_";
  if (type->IsPreciseRefType()) {
    ss << "R" << mirType.GetTypeIndex().GetIdx();
  } else {
    ss << GetPrimTypeName(type->GetPrimType());
  }
  return ss.str();
}

std::string FEIRVarReg::GetNameRawImpl() const {
  std::stringstream ss;
  ss << "Reg" << regNum;
  return ss.str();
}

std::unique_ptr<FEIRVar> FEIRVarReg::CloneImpl() const {
  std::unique_ptr<FEIRVar> var = std::make_unique<FEIRVarReg>(regNum, type->Clone());
  return var;
}

bool FEIRVarReg::EqualsToImpl(const std::unique_ptr<FEIRVar> &var) const {
  if (var->GetKind() != kind) {
    return false;
  }
  FEIRVarReg *ptrVarReg = static_cast<FEIRVarReg*>(var.get());
  ASSERT(ptrVarReg != nullptr, "ptr var is nullptr");
  return ptrVarReg->regNum == regNum;
}

size_t FEIRVarReg::HashImpl() const {
  return std::hash<uint32>{}(regNum);
}
}  // namespace maple