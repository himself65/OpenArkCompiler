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
#ifndef MAPLEBE_INCLUDE_CG_LABEL_CREATION_H
#define MAPLEBE_INCLUDE_CG_LABEL_CREATION_H

#include "cgfunc.h"
#include "cg_phase.h"
#include "mir_builder.h"

namespace maplebe {
class LabelCreation {
 public:
  explicit LabelCreation(CGFunc &func) : cgFunc(&func) {}

  ~LabelCreation() = default;

  void Run();

  std::string PhaseName() const {
    return "createlabel";
  }

 private:
  CGFunc *cgFunc;
  void CreateStartEndLabel();
};

CGFUNCPHASE(CgDoCreateLabel, "createstartendlabel")
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_LABEL_CREATION_H */