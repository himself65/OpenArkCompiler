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
#ifndef MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_GLOBAL_H
#define MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_GLOBAL_H

#include "global.h"
#include "aarch64_operand.h"

namespace maplebe {
using namespace maple;

class AArch64GlobalOpt : public GlobalOpt {
 public:
  explicit AArch64GlobalOpt(CGFunc &func) : GlobalOpt(func) {}
  ~AArch64GlobalOpt() override = default;
  void Run() override;
};

class OptimizeManager {
 public:
  explicit OptimizeManager(CGFunc &cgFunc) : cgFunc(cgFunc) {}
  ~OptimizeManager() = default;
  template<typename OptimizePattern>
  void Optimize() {
    OptimizePattern optPattern(cgFunc);
    optPattern.Run();
  }
 private:
  CGFunc &cgFunc;
};

class OptimizePattern {
 public:
  explicit OptimizePattern(CGFunc &cgFunc) : cgFunc(cgFunc) {}
  virtual ~OptimizePattern() = default;
  virtual bool CheckCondition(Insn &insn) = 0;
  virtual void Optimize(Insn &insn) = 0;
  virtual void Run() = 0;
  bool OpndDefByOne(Insn &insn, int32 useIdx) const;
  bool OpndDefByZero(Insn &insn, int32 useIdx) const;
  bool OpndDefByOneOrZero(Insn &insn, int32 useIdx) const;
  void ReplaceAllUsedOpndWithNewOpnd(const InsnSet &useInsnSet, uint32 regNO,
                                     Operand &newOpnd, bool updateInfo) const;

  static bool InsnDefOne(Insn &insn);
  static bool InsnDefZero(Insn &insn);
  static bool InsnDefOneOrZero(Insn &insn);
 protected:
  virtual void Init() = 0;
  CGFunc &cgFunc;
};

/*
 * Do Forward prop when insn is mov
 * mov xx, x1
 * ... // BBs and x1 is live
 * mOp yy, xx
 *
 * =>
 * mov x1, x1
 * ... // BBs and x1 is live
 * mOp yy, x1
 */
class ForwardPropPattern : public OptimizePattern {
 public:
  explicit ForwardPropPattern(CGFunc &cgFunc) : OptimizePattern(cgFunc) {}
  ~ForwardPropPattern() override = default;
  bool CheckCondition(Insn &insn) final;
  void Optimize(Insn &insn) final;
  void Run() final;

 protected:
  void Init() final;
 private:
  InsnSet firstRegUseInsnSet;
  std::set<BB*, BBIdCmp> modifiedBB;
};

/*
 * Do back propagate of vreg/preg when encount following insn:
 *
 * mov vreg/preg1, vreg2
 *
 * back propagate reg1 to all vreg2's use points and def points, when all of them is in same bb
 */
class BackPropPattern : public OptimizePattern {
 public:
  explicit BackPropPattern(CGFunc &cgFunc) : OptimizePattern(cgFunc) {}
  ~BackPropPattern() override = default;
  bool CheckCondition(Insn &insn) final;
  void Optimize(Insn &insn) final;
  void Run() final;

 protected:
  void Init() final;

 private:
  bool CheckAndGetOpnd(Insn &insn);
  bool DestOpndHasUseInsns(Insn &insn);
  bool DestOpndLiveOutToEHSuccs(Insn &insn);
  bool CheckSrcOpndDefAndUseInsns(Insn &insn);
  bool CheckPredefineInsn(Insn &insn);
  bool CheckRedefineInsn(Insn &insn);
  RegOperand *firstRegOpnd = nullptr;
  RegOperand *secondRegOpnd = nullptr;
  uint32 firstRegNO = 0;
  uint32 secondRegNO = 0;
  InsnSet srcOpndUseInsnSet;
  Insn *defInsnForSecondOpnd = nullptr;
};

/*
 *  when w0 has only one valid bit, these tranformation will be done
 *  cmp  w0, #0
 *  cset w1, NE --> mov w1, w0
 *
 *  cmp  w0, #0
 *  cset w1, EQ --> eor w1, w0, 1
 *
 *  cmp  w0, #1
 *  cset w1, NE --> eor w1, w0, 1
 *
 *  cmp  w0, #1
 *  cset w1, EQ --> mov w1, w0
 *
 *  cmp w0,  #0
 *  cset w0, NE -->null
 *
 *  cmp w0, #1
 *  cset w0, EQ -->null
 *
 *  condition:
 *    1. the first operand of cmp instruction must has only one valid bit
 *    2. the second operand of cmp instruction must be 0 or 1
 *    3. flag register of cmp isntruction must not be used later
 */
class CmpCsetPattern : public OptimizePattern {
 public:
  explicit CmpCsetPattern(CGFunc &cgFunc) : OptimizePattern(cgFunc) {}
  ~CmpCsetPattern() override = default;
  bool CheckCondition(Insn &insn) final;
  void Optimize(Insn &insn) final;
  void Run() final;

 protected:
  void Init() final;

 private:
  Insn *nextInsn = nullptr;
  int64 cmpConstVal  = 0;
  Operand *cmpFirstOpnd = nullptr;
  Operand *cmpSecondOpnd = nullptr;
  Operand *csetFirstOpnd = nullptr;
};

/*
 * mov w5, #1
 *  ...                   --> cset w5, NE
 * mov w0, #0
 * csel w5, w5, w0, NE
 *
 * mov w5, #0
 *  ...                   --> cset w5,EQ
 * mov w0, #1
 * csel w5, w5, w0, NE
 *
 * condition:
 *    1.all define points of w5 are defined by:   mov w5, #1(#0)
 *    2.all define points of w0 are defined by:   mov w0, #0(#1)
 *    3.w0 will not be used after: csel w5, w5, w0, NE(EQ)
 */
class CselPattern : public OptimizePattern {
 public:
  explicit CselPattern(CGFunc &cgFunc) : OptimizePattern(cgFunc) {}
  ~CselPattern() override = default;
  bool CheckCondition(Insn &insn) final;
  void Optimize(Insn &insn) final;
  void Run() final;

 protected:
  void Init() final {}

 private:
  AArch64CC_t GetInverseCondCode(const CondOperand &cond) const;
};

/*
 * uxtb  w0, w0    -->   null
 * uxth  w0, w0    -->   null
 *
 * condition:
 * 1. validbits(w0)<=8,16,32
 * 2. the first operand is same as the second operand
 *
 * uxtb  w0, w1    -->   null
 * uxth  w0, w1    -->   null
 *
 * condition:
 * 1. validbits(w1)<=8,16,32
 * 2. the use points of w0 has only one define point, that is uxt w0, w1
 */
class RedundantUxtPattern : public OptimizePattern {
 public:
  explicit RedundantUxtPattern(CGFunc &cgFunc) : OptimizePattern(cgFunc) {}
  ~RedundantUxtPattern() override = default;
  bool CheckCondition(Insn &insn) final;
  void Optimize(Insn &insn) final;
  void Run() final;

 protected:
  void Init() final;

 private:
  uint32 GetMaximumValidBit(Insn &insn, uint8 udIdx, InsnSet &insnChecked) const;
  static uint32 GetInsnValidBit(Insn &insn);
  InsnSet useInsnSet;
  uint32 firstRegNO = 0;
  Operand *secondOpnd = nullptr;
};

/*
 *  bl  MCC_NewObj_flexible_cname                              bl  MCC_NewObj_flexible_cname
 *  mov x21, x0   //  [R203]
 *  str x0, [x29,#16]   // local var: Reg0_R6340 [R203]  -->   str x0, [x29,#16]   // local var: Reg0_R6340 [R203]
 *  ... (has call)                                             ... (has call)
 *  mov x2, x21  // use of x21                                 ldr x2, [x29, #16]
 *  bl ***                                                     bl ***
 */
class LocalVarSaveInsnPattern : public OptimizePattern {
 public:
  explicit LocalVarSaveInsnPattern(CGFunc &cgFunc) : OptimizePattern(cgFunc) {}
  ~LocalVarSaveInsnPattern() override = default;
  bool CheckCondition(Insn &insn) final;
  void Optimize(Insn &insn) final;
  void Run() final;

 protected:
  void Init() final;

 private:
  bool CheckFirstInsn(Insn &firstInsn);
  bool CheckSecondInsn();
  bool CheckAndGetUseInsn(Insn &firstInsn);
  bool CheckLiveRange(Insn &firstInsn);
  Operand *firstInsnSrcOpnd = nullptr;
  Operand *firstInsnDestOpnd = nullptr;
  Operand *secondInsnSrcOpnd = nullptr;
  Operand *secondInsnDestOpnd = nullptr;
  Insn *useInsn = nullptr;
  Insn *secondInsn = nullptr;
};
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_GLOBAL_H */