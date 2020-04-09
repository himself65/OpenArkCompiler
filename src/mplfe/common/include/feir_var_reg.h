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
#ifndef MPLFE_INCLUDE_FEIR_VAR_REG_H
#define MPLFE_INCLUDE_FEIR_VAR_REG_H
#include "mir_symbol.h"
#include "feir_var.h"

namespace maple {
class FEIRVarReg : public FEIRVar {
 public:
  FEIRVarReg(uint32 argRegNum)
      : FEIRVar(FEIRVarKind::kFEIRVarReg),
        regNum(argRegNum) {}

  FEIRVarReg(uint32 argRegNum, PrimType argPrimType)
      : FEIRVarReg(argRegNum) {
    type->SetPrimType(argPrimType);
  }

  FEIRVarReg(uint32 argRegNum, std::unique_ptr<FEIRType> argType)
      : FEIRVar(FEIRVarKind::kFEIRVarReg, std::move(argType)),
        regNum(argRegNum) {}

  ~FEIRVarReg() = default;
  uint32 GetRegNum() const {
    return regNum;
  }

 protected:
  std::string GetNameImpl(const MIRType &mirType) const override;
  std::string GetNameRawImpl() const override;
  std::unique_ptr<FEIRVar> CloneImpl() const override;
  bool EqualsToImpl(const std::unique_ptr<FEIRVar> &var) const override;
  size_t HashImpl() const override;

 private:
  uint32 regNum;
};
}  // namespace maple
#endif  // MPLFE_INCLUDE_FEIR_VAR_REG_H