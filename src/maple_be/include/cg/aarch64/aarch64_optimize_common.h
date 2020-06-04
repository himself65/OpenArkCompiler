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
#ifndef MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_OPTIMIZE_COMMON_H
#define MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_OPTIMIZE_COMMON_H

#include "aarch64_isa.h"
#include "optimize_common.h"

namespace maplebe {
using namespace maple;


class AArch64InsnVisitor : public InsnVisitor {
 public:
  explicit AArch64InsnVisitor(CGFunc &func) : InsnVisitor(func) {}

  ~AArch64InsnVisitor() override = default;

  void ModifyJumpTarget(maple::LabelIdx targetLabel, BB &bb) override;
  void ModifyJumpTarget(Operand &targetOperand, BB &bb) override;
  void ModifyJumpTarget(BB &newTarget, BB &bb) override;
  /* Check if it requires to add extra gotos when relocate bb */
  MOperator FlipConditionOp(MOperator flippedOp, int &targetIdx) override;
  Insn *CloneInsn(Insn &originalInsn) override;
  LabelIdx GetJumpLabel(const Insn &insn) const override;
  bool IsCompareInsn(const Insn &insn) const override;
  bool IsCompareAndBranchInsn(const Insn &insn) const override;
  RegOperand *CreateVregFromReg(const RegOperand &pReg) override;

 private:
  int GetJumpTargetIdx(const Insn &insn) const;
};
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_OPTIMIZE_COMMON_H */