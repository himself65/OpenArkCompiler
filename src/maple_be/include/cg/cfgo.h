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
#ifndef MAPLEBE_INCLUDE_CG_CFGO_H
#define MAPLEBE_INCLUDE_CG_CFGO_H
#include "cg_cfg.h"
#include "optimize_common.h"

namespace maplebe {
class ChainingPattern : public OptimizationPattern {
 public:
  explicit ChainingPattern(CGFunc &func) : OptimizationPattern(func) {
    patternName = "BB Chaining";
    dotColor = kCfgoChaining;
  }

  ~ChainingPattern() override = default;
  bool Optimize(BB &curBB) override;

 protected:
  bool NoInsnBetween(const BB &from, const BB &to) const;
  bool DoSameThing(const BB &bb1, const Insn &last1, const BB &bb2, const Insn &last2) const;
  bool MergeFallthuBB(BB &curBB);
  bool MergeGotoBB(BB &curBB, BB &sucBB);
  bool MoveSuccBBAsCurBBNext(BB &curBB, BB &sucBB);
  bool RemoveGotoInsn(BB &curBB, BB &sucBB);
  bool ClearCurBBAndResetTargetBB(BB &curBB, BB &sucBB);
};

class SequentialJumpPattern : public OptimizationPattern {
 public:
  explicit SequentialJumpPattern(CGFunc &func) : OptimizationPattern(func) {
    patternName = "Sequential Jump";
    dotColor = kCfgoSj;
  }

  ~SequentialJumpPattern() override = default;
  bool Optimize(BB &curBB) override;

 protected:
  void SkipSucBB(BB &curBB, BB &sucBB);
};

class FlipBRPattern : public OptimizationPattern {
 public:
  explicit FlipBRPattern(CGFunc &func) : OptimizationPattern(func) {
    patternName = "Condition Flip";
    dotColor = kCfgoFlipCond;
  }

  ~FlipBRPattern() override = default;
  bool Optimize(BB &curBB) override;

 protected:
  void RelocateThrowBB(BB &curBB);
};

/* This class represents the scenario that the BB is unreachable. */
class UnreachBBPattern : public OptimizationPattern {
 public:
  explicit UnreachBBPattern(CGFunc &func) : OptimizationPattern(func) {
    patternName = "Unreachable BB";
    dotColor = kCfgoUnreach;
    func.GetTheCFG()->FindAndMarkUnreachable(*cgFunc);
  }

  ~UnreachBBPattern() override = default;
  bool Optimize(BB &curBB) override;
};

/*
 * This class represents the scenario that a common jump BB can be duplicated
 * to one of its another predecessor.
 */
class DuplicateBBPattern : public OptimizationPattern {
 public:
  explicit DuplicateBBPattern(CGFunc &func) : OptimizationPattern(func) {
    patternName = "Duplicate BB";
    dotColor = kCfgoDup;
  }

  ~DuplicateBBPattern() override = default;
  bool Optimize(BB &curBB) override;

 private:
  static constexpr int kThreshold = 10;
};

/*
 * This class represents the scenario that a BB contains nothing.
 */
class EmptyBBPattern : public OptimizationPattern {
 public:
  explicit EmptyBBPattern(CGFunc &func) : OptimizationPattern(func) {
    patternName = "Empty BB";
    dotColor = kCfgoEmpty;
  }

  ~EmptyBBPattern() override = default;
  bool Optimize(BB &curBB) override;
};

class CFGOptimizer : public Optimizer {
 public:
  CFGOptimizer(CGFunc &func, MemPool &memPool) : Optimizer(func, memPool) {
    name = "CFGO";
  }

  ~CFGOptimizer() override = default;
  void InitOptimizePatterns() override;
};

CGFUNCPHASE_CANSKIP(CgDoCfgo, "cfgo")
CGFUNCPHASE_CANSKIP(CgDoPostCfgo, "postcfgo")
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_CFGO_H */