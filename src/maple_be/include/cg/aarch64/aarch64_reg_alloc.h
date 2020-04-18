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
#ifndef MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_REG_ALLOC_H
#define MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_REG_ALLOC_H
#include "reg_alloc.h"
#include "aarch64_operand.h"
#include "aarch64_insn.h"
#include "aarch64_abi.h"

namespace maplebe {
class AArch64RegAllocator : public RegAllocator {
 public:
  AArch64RegAllocator(CGFunc &cgFunc, MemPool &memPool)
      : RegAllocator(cgFunc),
        alloc(&memPool),
        regMap(std::less<uint32>(), alloc.Adapter()),
        liveReg(std::less<uint8>(), alloc.Adapter()),
        allocatedSet(std::less<Operand*>(), alloc.Adapter()),
        visitedBBs(alloc.Adapter()),
        sortedBBs(alloc.Adapter()),
        rememberRegs(alloc.Adapter()) {
    for (int32 i = 0; i != kAllRegNum; i++) {
      availRegSet[i] = false;
    }
  }

  ~AArch64RegAllocator() override = default;

  void InitAvailReg();
  bool AllocatePhysicalRegister(RegOperand &opnd);
  void PreAllocate();
  void ReleaseReg(AArch64reg reg);
  void ReleaseReg(RegOperand &regOpnd);
  void GetPhysicalRegisterBank(RegType regType, uint8 &start, uint8 &end);
  void AllocHandleCallee(Insn &insn, const AArch64MD &md);
  bool IsYieldPointReg(AArch64reg regNO) const;
  bool IsSpecialReg(AArch64reg reg) const;
  bool IsUntouchableReg(uint32 regNO) const;
  void SaveCalleeSavedReg(RegOperand &opnd);

  bool AllPredBBVisited(BB &bb) const;
  BB *MarkStraightLineBBInBFS(BB*);
  BB *SearchForStraightLineBBs(BB&);
  void BFS(BB &bb);
  void ComputeBlockOrder();

  std::string PhaseName() const {
    return "regalloc";
  }

 protected:
  Operand *HandleRegOpnd(Operand &opnd);
  Operand *HandleMemOpnd(Operand &opnd);
  Operand *AllocSrcOpnd(Operand &opnd, OpndProp *opndProp = nullptr);
  Operand *AllocDestOpnd(Operand &opnd, const Insn &insn);

  MapleAllocator alloc;
  bool availRegSet[kAllRegNum];
  MapleMap<uint32, AArch64reg> regMap;  /* virtual-register-to-physical-register map */
  MapleSet<uint8> liveReg;              /* a set of currently live physical registers */
  MapleSet<Operand*> allocatedSet;      /* already allocated */
  MapleVector<bool> visitedBBs;
  MapleVector<BB*> sortedBBs;
  MapleVector<AArch64reg> rememberRegs;
};

class DefaultO0RegAllocator : public AArch64RegAllocator {
 public:
  DefaultO0RegAllocator(CGFunc &cgFunc, MemPool &memPool) : AArch64RegAllocator(cgFunc, memPool) {}

  ~DefaultO0RegAllocator() override = default;

  bool AllocateRegisters() override;
};
;
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_REG_ALLOC_H */