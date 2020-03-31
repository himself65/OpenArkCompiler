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
#ifndef MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_MEMLAYOUT_H
#define MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_MEMLAYOUT_H

#include "memlayout.h"
#include "aarch64_abi.h"

namespace maplebe {
class AArch64SymbolAlloc : public SymbolAlloc {
 public:
  AArch64SymbolAlloc() = default;

  ~AArch64SymbolAlloc() = default;

  void SetRegisters(AArch64reg r0, AArch64reg r1) {
    reg0 = r0;
    reg1 = r1;
  }

  inline bool IsRegister() {
    return reg0 != kRinvalid;
  }

 private:
  AArch64reg reg0 = kRinvalid;
  AArch64reg reg1 = kRinvalid;
};

/*
 * On AArch64, stack frames are structured as follows:
 *
 * The stack grows downward -- full descending (SP points
 * to a filled slot).
 *
 * Any of the parts of a frame is optional, i.e., it is
 * possible to write a caller-callee pair in such a way
 * that the particular part is absent in the frame.
 *
 * Before a call is made, the frame looks like:
 * |                            |
 * ||----------------------------|
 * | args passed on the stack   | (we call them up-formals)
 * ||----------------------------|<- Stack Pointer
 * |                            |
 *
 * V1.
 * Right after a call is made
 * |                            |
 * ||----------------------------|
 * | args passed on the stack   |
 * ||----------------------------|<- Stack Pointer
 * | PREV_FP, PREV_LR           |
 * ||----------------------------|<- Frame Pointer
 *
 * After the prologue has run,
 * |                            |
 * ||----------------------------|
 * | args passed on the stack   |
 * ||----------------------------|
 * | PREV_FP, PREV_LR           |
 * ||----------------------------|<- Frame Pointer
 * | callee-saved registers     |
 * ||----------------------------|
 * | empty space. should have   |
 * | at least 16-byte alignment |
 * ||----------------------------|
 * | local variables            |
 * ||----------------------------|
 * | variable-sized local vars  |
 * | (VLAs)                     |
 * ||----------------------------|<- Stack Pointer
 *
 * callee-saved registers include
 *  1. R19-R28
 *  2. R8 if return value needs to be returned
 *     thru memory and callee wants to use R8
 *  3. we don't need to save R19 if it is used
 *     as base register for PIE.
 *  4. V8-V15
 *
 * V2. (this way, we may be able to save
 *     on SP modifying instruction)
 * Right after a call is made
 * |                            |
 * ||----------------------------|
 * | args passed on the stack   |
 * ||----------------------------|<- Stack Pointer
 * |                            |
 * | empty space                |
 * |                            |
 * ||----------------------------|
 * | PREV_FP, PREV_LR           |
 * ||----------------------------|<- Frame Pointer
 *
 * After the prologue has run,
 * |                            |
 * ||----------------------------|
 * | args passed on the stack   |
 * ||----------------------------|
 * | callee-saved registers     |
 * | including those used for   |
 * | parameter passing          |
 * ||----------------------------|
 * | empty space. should have   |
 * | at least 16-byte alignment |
 * ||----------------------------|
 * | local variables            |
 * ||----------------------------|
 * | PREV_FP, PREV_LR           |
 * ||----------------------------|<- Frame Pointer
 * | variable-sized local vars  |
 * | (VLAs)                     |
 * ||----------------------------|
 * | args to pass through stack |
 * ||----------------------------|
 */
class AArch64MemLayout : public MemLayout {
 public:
  AArch64MemLayout(BECommon &b, MIRFunction &f, MapleAllocator &mallocator)
      : MemLayout(b, f, mallocator) {}

  ~AArch64MemLayout() override = default;

  /*
   * Returns stack space required for a call
   * which is used to pass arguments that cannot be
   * passed through registers
   */
  uint32 ComputeStackSpaceRequirementForCall(StmtNode &stmt, bool isIcall) override;

  void LayoutStackFrame() override;

  void AssignSpillLocationsToPseudoRegisters() override;

  SymbolAlloc *AssignLocationToSpillReg(regno_t vrNum) override;

  int32 StackFrameSize();

  int32 RealStackFrameSize();

  const MemSegment &locals() const {
    return segLocals;
  }

  int32 GetSizeOfSpillReg() const {
    return segSpillReg.GetSize();
  }

  int32 GetSizeOfLocals() const {
    return segLocals.GetSize();
  }

  int32 GetSizeOfRefLocals() {
    return segRefLocals.GetSize();
  }

  int32 GetRefLocBaseLoc() const;

 private:
  MemSegment segRefLocals = MemSegment(kMsRefLocals);
  /* callee saved register R19-R28 (10) */
  MemSegment segSpillReg = MemSegment(kMsSpillReg);
  MemSegment segLocals = MemSegment(kMsLocals);  /* these are accessed via Frame Pointer */
  int32 fixStackSize = 0;
  void setSegmentSize(AArch64SymbolAlloc &symbolAlloc, MemSegment &segment, uint32 typeIdx);
  void layoutFormalParams();
  void layoutActualParams();
  void layoutLocalVariales(std::vector<MIRSymbol*> &tempVar, std::vector<MIRSymbol*> &returnDelays);
  void layoutEAVariales(std::vector<MIRSymbol*> &tempVar);
  void layoutReturnRef(std::vector<MIRSymbol*> &returnDelays);
};
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_MEMLAYOUT_H */
