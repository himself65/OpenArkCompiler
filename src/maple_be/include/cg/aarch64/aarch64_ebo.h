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
#ifndef MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_EBO_H
#define MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_EBO_H

#include "ebo.h"
#include "aarch64_operand.h"
#include "aarch64_cgfunc.h"

namespace maplebe {
using namespace maple;

class AArch64Ebo : public Ebo {
 public:
  AArch64Ebo(CGFunc &func, MemPool &memPool, LiveAnalysis *live, bool before, const std::string &phase)
      : Ebo(func, memPool, live, before, phase),
        callerSaveRegTable(eboAllocator.Adapter()) {
    a64CGFunc = static_cast<AArch64CGFunc*>(cgFunc);
  }

  ~AArch64Ebo() override = default;

 protected:
  MapleVector<RegOperand*> callerSaveRegTable;
  AArch64CGFunc *a64CGFunc;
  int32 GetOffsetVal(const MemOperand &mem) const override;
  OpndInfo *OperandInfoDef(BB &currentBB, Insn &currentInsn, Operand &localOpnd) override;
  const RegOperand &GetRegOperand(const Operand &opnd) const override;
  bool IsGlobalNeeded(Insn &insn) const override;
  bool OperandEqSpecial(const Operand &op1, const Operand &op2) const override;
  bool DoConstProp(Insn &insn, uint32 i, Operand &opnd) override;
  bool DoConstantFold(Insn &insn, const MapleVector<Operand*> &opnds) override;
  bool ConstantOperand(Insn &insn, const MapleVector<Operand*> &opnds, const MapleVector<OpndInfo*> &opndInfo) override;
  void BuildCallerSaveRegisters() override;
  void DefineCallerSaveRegisters(InsnInfo &insnInfo) override;
  void DefineReturnUseRegister(Insn &insn) override;
  void DefineCallUseSpecialRegister(Insn &insn) override;
  void DefineClinitSpecialRegisters(InsnInfo &insnInfo) override;
  bool SpecialSequence(Insn &insn, const MapleVector<OpndInfo*> &origInfos) override;
  bool IsMovToSIMDVmov(Insn &insn, const Insn &replaceInsn) const override;
  bool ChangeLdrMop(Insn &insn, const Operand &opnd) const override;
  bool IsAdd(const Insn &insn) const override;
  bool IsFmov(const Insn &insn) const override;
  bool IsClinitCheck(const Insn &insn) const override;
  bool IsLastAndBranch(BB &bb, Insn &insn) const override;
  bool ResIsNotDefAndUse(Insn &insn) const override;

 private:
  /* The number of elements in callerSaveRegTable must less then 45. */
  static constexpr int32 kMaxCallerSaveReg = 45;
  bool IsZeroRegister(const Operand &opnd) const;
  bool CheckCondCode(const CondOperand &cond) const;
  AArch64CC_t GetReverseCond(const CondOperand &cond) const;
};
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_EBO_H */
