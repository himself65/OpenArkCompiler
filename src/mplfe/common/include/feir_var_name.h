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
#ifndef MPLFE_INCLUDE_FEIR_VAR_NAME_H
#define MPLFE_INCLUDE_FEIR_VAR_NAME_H
#include <string>
#include "mir_symbol.h"
#include "types_def.h"
#include "global_tables.h"
#include "feir_var.h"

namespace maple {
// ---------- FEIRVarName ----------
class FEIRVarName : public FEIRVar {
 public:
  FEIRVarName(const GStrIdx &argNameIdx, bool argWithType = false)
      : FEIRVar(FEIRVarKind::kFEIRVarName),
        nameIdx(argNameIdx),
        withType(argWithType) {}

  FEIRVarName(const GStrIdx &argNameIdx, std::unique_ptr<FEIRType> argType, bool argWithType = false)
      : FEIRVar(FEIRVarKind::kFEIRVarName, std::move(argType)),
        nameIdx(argNameIdx),
        withType(argWithType) {}

  virtual ~FEIRVarName() = default;

 protected:
  std::string GetNameImpl(const MIRType &mirType) const override;
  std::string GetNameRawImpl() const override;
  std::unique_ptr<FEIRVar> CloneImpl() const override;
  bool EqualsToImpl(const std::unique_ptr<FEIRVar> &var) const override;
  size_t HashImpl() const override;

  GStrIdx nameIdx;
  bool withType : 1;
};

// ---------- FEIRVarNameSpec ----------
class FEIRVarNameSpec : public FEIRVarName {
 public:
  FEIRVarNameSpec(const std::string &argName, bool argWithType = false)
      : FEIRVarName(GStrIdx(0), argWithType),
        name(argName) {
    kind = FEIRVarKind::kFEIRVarNameSpec;
  }

  FEIRVarNameSpec(const std::string &argName, std::unique_ptr<FEIRType> argType, bool argWithType = false)
      : FEIRVarName(GStrIdx(0), std::move(argType), argWithType),
        name(argName) {
    kind = FEIRVarKind::kFEIRVarNameSpec;
  }

  ~FEIRVarNameSpec() = default;

 protected:
  std::string GetNameImpl(const MIRType &mirType) const override;
  std::string GetNameRawImpl() const override;
  std::unique_ptr<FEIRVar> CloneImpl() const override;
  bool EqualsToImpl(const std::unique_ptr<FEIRVar> &var) const override;
  size_t HashImpl() const override;

 private:
  std::string name;
};
}  // namespace maple
#endif  // MPLFE_INCLUDE_FEIR_VAR_REG_H