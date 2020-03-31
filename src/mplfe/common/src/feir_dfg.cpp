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
#include "feir_dfg.h"

namespace maple {
void FEIRDFG::CalculateDefUseByUseDef(std::map<UniqueFEIRVar*, std::set<UniqueFEIRVar*>> &mapDefUse,
                                      const std::map<UniqueFEIRVar*, std::set<UniqueFEIRVar*>> &mapUseDef) {
  mapDefUse.clear();
  for (auto &it : mapUseDef) {
    for (UniqueFEIRVar *def : it.second) {
      if (mapDefUse[def].find(it.first) == mapDefUse[def].end()) {
        CHECK_FATAL(mapDefUse[def].insert(it.first).second, "map def use insert failed");
      }
    }
  }
}

void FEIRDFG::CalculateUseDefByDefUse(std::map<UniqueFEIRVar*, std::set<UniqueFEIRVar*>> &mapUseDef,
                                      const std::map<UniqueFEIRVar*, std::set<UniqueFEIRVar*>> &mapDefUse) {
  mapUseDef.clear();
  for (auto &it : mapDefUse) {
    for (UniqueFEIRVar *use : it.second) {
      if (mapUseDef[use].find(it.first) == mapUseDef[use].end()) {
        CHECK_FATAL(mapUseDef[use].insert(it.first).second, "map use def insert failed");
      }
    }
  }
}
}  // namespace maple