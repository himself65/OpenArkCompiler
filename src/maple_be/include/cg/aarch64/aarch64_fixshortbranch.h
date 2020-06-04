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
#ifndef MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_FIXSHORTBRANCH_H
#define MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_FIXSHORTBRANCH_H

#include <vector>
#include "aarch64_cg.h"
#include "optimize_common.h"
#include "mir_builder.h"

namespace maplebe {
class AArch64FixShortBranch {
 public:
  explicit AArch64FixShortBranch(CGFunc *cf) : cgFunc(cf) {
    cg = cgFunc->GetCG();
  }
  ~AArch64FixShortBranch() = default;
  void FixShortBranches();

 private:
  CGFunc *cgFunc;
  CG *cg;
  bool DistanceCheck(const BB &bb, LabelIdx targLabIdx, uint32 targId);
  void SetInsnId();
};  /* class AArch64ShortBranch */

CGFUNCPHASE(CgFixShortBranch, "fixshortbranch")
}  /* namespace maplebe */
#endif  /* MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_FIXSHORTBRANCH_H */
