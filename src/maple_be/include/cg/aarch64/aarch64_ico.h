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
#ifndef MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_ICO_H
#define MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_ICO_H
#include "ico.h"
#include "aarch64_isa.h"
#include "optimize_common.h"
#include "live.h"

namespace maplebe {
class AArch64IfConversionOptimizer : public IfConversionOptimizer {
 public:
  AArch64IfConversionOptimizer(CGFunc &func, MemPool &memPool) : IfConversionOptimizer(func, memPool) {}

  ~AArch64IfConversionOptimizer() override = default;
  void InitOptimizePatterns() override;
};

/* If-Then-Else pattern */
class AArch64ICOPattern : public ICOPattern {
 public:
  explicit AArch64ICOPattern(CGFunc &func) : ICOPattern(func) {}
  ~AArch64ICOPattern() override = default;
 protected:
  bool DoOpt(BB &cmpBB, BB *ifBB, BB *elseBB, BB &joinBB) override;
  AArch64CC_t Encode(MOperator mOp, bool inverse) const;
  Insn *BuildCondSet(const Insn &branch, RegOperand &reg, bool inverse);
  Insn *BuildCondSel(const Insn &branch, MOperator mOp, RegOperand &dst, RegOperand &src1, RegOperand &src2);
  Insn *BuildCmpInsn(const Insn &condBr);
  bool IsSetInsn(const Insn &insn, Operand *&dest, Operand *&src) const;
  bool BuildCondMovInsn(BB &cmpBB, const BB &bb, const std::map<Operand*, Operand*> &ifDestSrcMap,
                        const std::map<Operand*, Operand*> &elseDestSrcMap, bool elseBBIsProcessed,
                        std::vector<Insn*> &generateInsn);
  void GenerateInsnForImm(const Insn &branchInsn, Operand &ifDest, Operand &elseDest, RegOperand &destReg,
                          std::vector<Insn*> &generateInsn);
  Operand *GetDestReg(const std::map<Operand*, Operand*> &destSrcMap, const RegOperand &destReg) const;
  void GenerateInsnForReg(const Insn &branchInsn, Operand &ifDest, Operand &elseDest, RegOperand &destReg,
                          std::vector<Insn*> &generateInsn);
  RegOperand *GenerateRegAndTempInsn(Operand &dest, const RegOperand &destReg, std::vector<Insn*> &generateInsn);
  bool CheckModifiedRegister(Insn &insn, std::map<Operand*, Operand*> &destSrcMap, Operand &src,
                             Operand &dest) const;
  bool CheckCondMoveBB(BB *bb, std::map<Operand*, Operand*> &destSrcMap,
                       std::vector<Operand*> &destRegs, Operand *flagReg) const;
};
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_ICO_H */
