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
#ifndef MAPLEBE_INCLUDE_CG_GLOBAL_H
#define MAPLEBE_INCLUDE_CG_GLOBAL_H

#include "cg_phase.h"

namespace maplebe {
class GlobalOpt {
 public:
  explicit GlobalOpt(CGFunc &func) : cgFunc(func) {}
  virtual ~GlobalOpt() = default;
  virtual void Run() {}
  std::string PhaseName() const {
    return "globalopt";
  }

 protected:
  /* if the number of bbs is more than 500 or the number of insns is more than 9000, don't optimize. */
  static constexpr uint32 kMaxBBNum = 500;
  static constexpr uint32 kMaxInsnNum = 9000;
  CGFunc &cgFunc;
};

CGFUNCPHASE_CANSKIP(CgDoGlobalOpt, "globalopt")
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_GLOBAL_H */