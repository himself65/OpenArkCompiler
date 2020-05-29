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
#ifndef MAPLEBE_INCLUDE_CG_ICO_H
#define MAPLEBE_INCLUDE_CG_ICO_H
#include "optimize_common.h"
#include "live.h"

namespace maplebe {
class IfConversionOptimizer : public Optimizer {
 public:
  IfConversionOptimizer(CGFunc &func, MemPool &memPool) : Optimizer(func, memPool) {
    name = "ICO";
  }

  ~IfConversionOptimizer() override = default;
};

/* If-Then-Else pattern */
class ICOPattern : public OptimizationPattern {
 public:
  explicit ICOPattern(CGFunc &func) : OptimizationPattern(func) {
    dotColor = kIcoIte;
    patternName = "If-Then-Else";
  }
  ~ICOPattern() override = default;
  static constexpr int kThreshold = 2;
  bool Optimize(BB &curBB) override;

 protected:
  virtual bool DoOpt(BB &cmpBB, BB *ifBB, BB *elseBB, BB &joinBB) = 0;
  Insn *FindLastCmpInsn(BB &bb) const;
};

CGFUNCPHASE_CANSKIP(CgDoIco, "ico")
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_ICO_H */
