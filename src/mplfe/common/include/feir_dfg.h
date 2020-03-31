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
#ifndef MPLFE_INCLUDE_FEIR_DFG_H
#define MPLFE_INCLUDE_FEIR_DFG_H
#include "feir_var.h"

namespace maple {
class FEIRDFG {
 public:
  FEIRDFG() = default;
  ~FEIRDFG() = default;
  static void CalculateDefUseByUseDef(std::map<UniqueFEIRVar*, std::set<UniqueFEIRVar*>> &mapDefUse,
                                      const std::map<UniqueFEIRVar*, std::set<UniqueFEIRVar*>> &mapUseDef);
  static void CalculateUseDefByDefUse(std::map<UniqueFEIRVar*, std::set<UniqueFEIRVar*>> &mapUseDef,
                                      const std::map<UniqueFEIRVar*, std::set<UniqueFEIRVar*>> &mapDefUse);
};
}  // namespace maple
#endif  // MPLFE_INCLUDE_FEIR_DFG_H