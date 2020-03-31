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
#ifndef MPLFE_INCLUDE_FEIR_TYPE_INFER_H
#define MPLFE_INCLUDE_FEIR_TYPE_INFER_H
#include "fe_configs.h"
#include "feir_type.h"
#include "feir_var.h"

namespace maple {
class FEIRTypeMergeHelper {
 public:
  FEIRTypeMergeHelper();
  explicit FEIRTypeMergeHelper(const UniqueFEIRType &argTypeDefault);
  ~FEIRTypeMergeHelper() = default;
  void Reset();
  void ResetTypeDefault(const UniqueFEIRType &argTypeDefault);
  bool MergeType(const UniqueFEIRType &argType, bool parent = true);
  UniqueFEIRType GetResult() const;
  const std::string &GetError() const {
    return error;
  }

  const UniqueFEIRType &GetType() const {
    return type;
  }

  UniqueFEIRType GetTypeClone() const {
    return type->Clone();
  }

 LLT_PRIVATE:
  UniqueFEIRType typeDefault;
  UniqueFEIRType type;
  std::string error;
  bool firstType;
  void SetDefaultType(UniqueFEIRType &typeDst);
  void SetType(UniqueFEIRType &typeDst, const UniqueFEIRType &typeSrc);
  bool MergeType(UniqueFEIRType &typeDst, const UniqueFEIRType &typeSrc, bool parent = true);
};

class FEIRTypeInfer {
 public:
  FEIRTypeInfer(MIRSrcLang argSrcLang, const std::map<const UniqueFEIRVar*, std::set<UniqueFEIRVar*>> &argMapDefUse);
  ~FEIRTypeInfer() = default;
  void LoadTypeDefault();
  void Reset();
  UniqueFEIRType GetTypeForVarUse(const UniqueFEIRVar &varUse);
  UniqueFEIRType GetTypeForVarDef(const UniqueFEIRVar &varDef);
  UniqueFEIRType GetTypeByTransForVarUse(const UniqueFEIRVar &varUse);
  void ProcessVarDef(UniqueFEIRVar &varDef);

 private:
  MIRSrcLang srcLang;
  UniqueFEIRType typeDefault;
  FEIRTypeMergeHelper mergeHelper;
  const std::map<const UniqueFEIRVar*, std::set<UniqueFEIRVar*>> &mapDefUse;
  std::set<const UniqueFEIRVar*> visitVars;
  bool withCircle = false;
  bool first = false;
};
}  // namespace maple
#endif  // MPLFE_INCLUDE_FEIR_TYPE_INFER_H
