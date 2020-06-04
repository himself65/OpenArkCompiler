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
#ifndef MAPLEBE_INCLUDE_CG_PEEP_H
#define MAPLEBE_INCLUDE_CG_PEEP_H

#include "cg.h"
#include "optimize_common.h"

namespace maplebe {
enum ReturnType : uint8 {
  kResUseFirst,
  kResDefFirst,
  kResNotFind
};

class PeepPattern {
 public:
  explicit PeepPattern(CGFunc &oneCGFunc) : cgFunc(oneCGFunc) {}
  virtual ~PeepPattern() = default;
  virtual void Run(BB &bb, Insn &insn) = 0;
  /* optimization support function */
  bool IfOperandIsLiveAfterInsn(const RegOperand &regOpnd, Insn &insn);
  bool FindRegLiveOut(const RegOperand &regOpnd, const BB &bb);
  bool CheckOpndLiveinSuccs(const RegOperand &regOpnd, const BB &bb) const;
  ReturnType IsOpndLiveinBB(const RegOperand &regOpnd, const BB &bb) const;
  int logValueAtBase2(int64 val) const;
  bool IsMemOperandOptPattern(const Insn &insn, Insn &nextInsn);

 protected:
  CGFunc &cgFunc;
};

class PeepHoleOptimizer {
 public:
  explicit PeepHoleOptimizer(CGFunc *cf) : cgFunc(cf) {
    cg = cgFunc->GetCG();
  }
  ~PeepHoleOptimizer() = default;
  void Peephole0();
  void PeepholeOpt();
  void PrePeepholeOpt();
  void PrePeepholeOpt1();

 private:
  CGFunc *cgFunc;
  CG *cg;
};  /* class PeepHoleOptimizer */

class PeepPatternMatch {
 public:
  PeepPatternMatch(CGFunc &oneCGFunc, MemPool *memPool)
      : optOwnMemPool(memPool),
        peepAllocator(memPool),
        optimizations(peepAllocator.Adapter()),
        cgFunc(oneCGFunc) {}
  virtual ~PeepPatternMatch() = default;
  virtual void Run(BB &bb, Insn &insn) = 0;
  virtual void InitOpts() = 0;
 protected:
  MemPool *optOwnMemPool;
  MapleAllocator peepAllocator;
  MapleVector<PeepPattern*> optimizations;
  CGFunc &cgFunc;
};

class PeepOptimizer {
 public:
  PeepOptimizer(CGFunc &oneCGFunc, MemPool *memPool)
      : cgFunc(oneCGFunc),
        peepOptMemPool(memPool) {
    index = 0;
  }
  ~PeepOptimizer() = default;
  template<typename T>
  void Run();
  static int32 index;

 private:
  CGFunc &cgFunc;
  MemPool *peepOptMemPool;
};

CGFUNCPHASE_CANSKIP(CgDoPrePeepHole, "prepeephole")
CGFUNCPHASE_CANSKIP(CgDoPrePeepHole1, "prepeephole1")
CGFUNCPHASE_CANSKIP(CgDoPeepHole0, "peephole0")
CGFUNCPHASE_CANSKIP(CgDoPeepHole, "peephole")
}  /* namespace maplebe */
#endif  /* MAPLEBE_INCLUDE_CG_PEEP_H */
