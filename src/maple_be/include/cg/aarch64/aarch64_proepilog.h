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
#ifndef MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_PROEPILOG_H
#define MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_PROEPILOG_H

#include "proepilog.h"
#include "cg.h"
#include "operand.h"
#include "aarch64_cgfunc.h"
#include "aarch64_operand.h"
#include "aarch64_insn.h"

namespace maplebe {
using namespace maple;

class AArch64GenProEpilog : public GenProEpilog {
 public:
  explicit AArch64GenProEpilog(CGFunc &func) : GenProEpilog(func) {}
  ~AArch64GenProEpilog() override = default;

  void Run() override;
 private:
  void GenStackGuard(BB&);
  BB &GenStackGuardCheckInsn(BB&);
  AArch64MemOperand *SplitStpLdpOffsetForCalleeSavedWithAddInstruction(const AArch64MemOperand &mo, uint32 bitLen,
                                                                       AArch64reg baseReg = AArch64reg::kRinvalid);
  void AppendInstructionPushPair(AArch64reg reg0, AArch64reg reg1, RegType rty, int offset);
  void AppendInstructionPushSingle(AArch64reg reg, RegType rty, int offset);
  void AppendInstructionAllocateCallFrame(AArch64reg reg0, AArch64reg reg1, RegType rty);
  void AppendInstructionAllocateCallFrameDebug(AArch64reg reg0, AArch64reg reg1, RegType rty);
  void GeneratePushRegs();
  void AppendInstructionStackCheck(AArch64reg reg, RegType rty, int offset);
  void GenerateProlog(BB&);

  void GenerateRet(BB &bb);
  bool TestPredsOfRetBB(const BB &exitBB);
  void AppendInstructionPopSingle(AArch64reg reg, RegType rty, int offset);
  void AppendInstructionPopPair(AArch64reg reg0, AArch64reg reg1, RegType rty, int offset);
  void AppendInstructionDeallocateCallFrame(AArch64reg reg0, AArch64reg reg1, RegType rty);
  void AppendInstructionDeallocateCallFrameDebug(AArch64reg reg0, AArch64reg reg1, RegType rty);
  void GeneratePopRegs();
  void AppendJump(const MIRSymbol &func);
  void GenerateEpilog(BB&);
  void GenerateEpilogForCleanup(BB&);
  Insn &CreateAndAppendInstructionForAllocateCallFrame(int64 argsToStkPassSize, AArch64reg reg0, AArch64reg reg1,
                                                       RegType rty);
  Insn &AppendInstructionForAllocateOrDeallocateCallFrame(int64 argsToStkPassSize, AArch64reg reg0, AArch64reg reg1,
                                                          RegType rty, bool isAllocate);
  static constexpr const int32 kOffset8MemPos = 8;
  static constexpr const int32 kOffset16MemPos = 16;
};
}  /* namespace maplebe */

#endif /* MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_PROEPILOG_H */