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
#ifndef FEIR_VAR_TYPE_SCATTER_H
#define FEIR_VAR_TYPE_SCATTER_H
#include <unordered_set>
#include "feir_var.h"
#include "feir_type.h"

namespace maple {
class FEIRVarTypeScatter : public FEIRVar {
 public:
  FEIRVarTypeScatter(UniqueFEIRVar argVar);
  ~FEIRVarTypeScatter() = default;
  void AddScatterType(const UniqueFEIRType &type);
  const std::unordered_set<FEIRTypeKey, FEIRTypeKeyHash> &GetScatterTypes() const {
    return scatterTypes;
  }

 protected:
  std::string GetNameImpl(const MIRType &mirType) const override;
  std::string GetNameRawImpl() const override;
  std::unique_ptr<FEIRVar> CloneImpl() const override;
  bool EqualsToImpl(const std::unique_ptr<FEIRVar> &argVar) const override;
  size_t HashImpl() const override;

 private:
  UniqueFEIRVar var;
  std::unordered_set<FEIRTypeKey, FEIRTypeKeyHash> scatterTypes;
};  // class FEIRVarTypeScatter
}  // namespace maple
#endif // FEIR_VAR_TYPE_SCATTER_H
