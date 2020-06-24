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
#ifndef MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_PEEP_H
#define MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_PEEP_H

#include <vector>
#include "peep.h"
#include "aarch64_cg.h"
#include "optimize_common.h"
#include "mir_builder.h"

namespace maplebe {
/*
 * Looking for identical mem insn to eliminate.
 * If two back-to-back is:
 * 1. str + str
 * 2. str + ldr
 * And the [MEM] is pattern of [base + offset]
 * 1. The [MEM] operand is exactly same then first
 *    str can be eliminate.
 * 2. The [MEM] operand is exactly same and src opnd
 *    of str is same as the dest opnd of ldr then
 *    ldr can be eliminate
 */
class RemoveIdenticalLoadAndStoreAArch64 : public PeepPattern {
 public:
  explicit RemoveIdenticalLoadAndStoreAArch64(CGFunc &cgFunc) : PeepPattern(cgFunc) {}
  ~RemoveIdenticalLoadAndStoreAArch64() override = default;
  void Run(BB &bb, Insn &insn) override;

 private:
  bool IsMemOperandsIdentical(const Insn &insn1, const Insn &insn2) const;
};

/* Remove redundant mov which src and dest opnd is exactly same */
class RemoveMovingtoSameRegAArch64 : public PeepPattern {
 public:
  explicit RemoveMovingtoSameRegAArch64(CGFunc &cgFunc) : PeepPattern(cgFunc) {}
  ~RemoveMovingtoSameRegAArch64() override = default;
  void Run(BB &bb, Insn &insn) override;
};

/* Combining 2 STRs into 1 stp or 2 LDRs into 1 ldp, when they are
 * back to back and the [MEM] they access is conjointed.
 */
class CombineContiLoadAndStoreAArch64 : public PeepPattern {
 public:
  explicit CombineContiLoadAndStoreAArch64(CGFunc &cgFunc) : PeepPattern(cgFunc) {}
  ~CombineContiLoadAndStoreAArch64() override = default;
  void Run(BB &bb, Insn &insn) override;
};

/* Eliminate the sxt[b|h|w] w0, w0;, when w0 is satisify following:
 * i)  mov w0, #imm (#imm is not out of range)
 * ii) ldrs[b|h] w0, [MEM]
 */
class EliminateSpecifcSXTAArch64 : public PeepPattern {
 public:
  explicit EliminateSpecifcSXTAArch64(CGFunc &cgFunc) : PeepPattern(cgFunc) {}
  ~EliminateSpecifcSXTAArch64() override = default;
  void Run(BB &bb, Insn &insn) override;
};

/* Eliminate the uxt[b|h|w] w0, w0;when w0 is satisify following:
 * i)  mov w0, #imm (#imm is not out of range)
 * ii) mov w0, R0(Is return value of call and return size is not of range)
 * iii)w0 is defined and used by special load insn and uxt[] pattern
 */
class EliminateSpecifcUXTAArch64 : public PeepPattern {
 public:
  explicit EliminateSpecifcUXTAArch64(CGFunc &cgFunc) : PeepPattern(cgFunc) {}
  ~EliminateSpecifcUXTAArch64() override = default;
  void Run(BB &bb, Insn &insn) override;
};

/* fmov ireg1 <- freg1   previous insn
 * fmov ireg2 <- freg1   current insn
 * use  ireg2            may or may not be present
 * =>
 * fmov ireg1 <- freg1   previous insn
 * mov  ireg2 <- ireg1   current insn
 * use  ireg1            may or may not be present
 */
class FmovRegAArch64 : public PeepPattern {
 public:
  explicit FmovRegAArch64(CGFunc &cgFunc) : PeepPattern(cgFunc) {}
  ~FmovRegAArch64() override = default;
  void Run(BB &bb, Insn &insn) override;
};

/* cbnz x0, labelA
 * mov x0, 0
 * b  return-bb
 * labelA:
 * =>
 * cbz x0, return-bb
 * labelA:
 */
class CbnzToCbzAArch64 : public PeepPattern {
 public:
  explicit CbnzToCbzAArch64(CGFunc &cgFunc) : PeepPattern(cgFunc) {}
  ~CbnzToCbzAArch64() override = default;
  void Run(BB &bb, Insn &insn) override;
};

/* When exist load after load or load after store, and [MEM] is
 * totally same. Then optimize them.
 */
class ContiLDRorSTRToSameMEMAArch64 : public PeepPattern {
 public:
  explicit ContiLDRorSTRToSameMEMAArch64(CGFunc &cgFunc) : PeepPattern(cgFunc) {}
  ~ContiLDRorSTRToSameMEMAArch64() override = default;
  void Run(BB &bb, Insn &insn) override;
};

/*
 *  Remove following patterns:
 *  mov     x1, x0
 *  bl      MCC_IncDecRef_NaiveRCFast
 */
class RemoveIncDecRefAArch64 : public PeepPattern {
 public:
  explicit RemoveIncDecRefAArch64(CGFunc &cgFunc) : PeepPattern(cgFunc) {}
  ~RemoveIncDecRefAArch64() override = default;
  void Run(BB &bb, Insn &insn) override;
};

/*
 * When GCONLY is enabled, the read barriers can be inlined.
 * we optimize it with the following pattern:
 * #if USE_32BIT_REF
 *   bl MCC_LoadRefField             ->  ldr  w0, [x1]
 *   bl MCC_LoadVolatileField        ->  ldar w0, [x1]
 *   bl MCC_LoadRefStatic            ->  ldr  w0, [x0]
 *   bl MCC_LoadVolatileStaticField  ->  ldar w0, [x0]
 *   bl MCC_Dummy                    ->  omitted
 * #else
 *   bl MCC_LoadRefField             ->  ldr  x0, [x1]
 *   bl MCC_LoadVolatileField        ->  ldar x0, [x1]
 *   bl MCC_LoadRefStatic            ->  ldr  x0, [x0]
 *   bl MCC_LoadVolatileStaticField  ->  ldar x0, [x0]
 *   bl MCC_Dummy                    ->  omitted
 * #endif
 *
 * if we encounter a tail call optimized read barrier call,
 * such as:
 *   b MCC_LoadRefField
 * a return instruction will be added just after the load:
 *   ldr w0, [x1]
 *   ret
 */
class InlineReadBarriersAArch64 : public PeepPattern {
 public:
  explicit InlineReadBarriersAArch64(CGFunc &cgFunc) : PeepPattern(cgFunc) {}
  ~InlineReadBarriersAArch64() override = default;
  void Run(BB &bb, Insn &insn) override;
};

/*
 *    mov     w1, #34464
 *    movk    w1, #1,  LSL #16
 *    sdiv    w2, w0, w1
 *  ========>
 *    mov     w1, #34464         // may deleted if w1 not live anymore.
 *    movk    w1, #1,  LSL #16   // may deleted if w1 not live anymore.
 *    mov     w16, #0x588f
 *    movk    w16, #0x4f8b, LSL #16
 *    smull   x16, w0, w16
 *    asr     x16, x16, #32
 *    add     x16, x16, w0, SXTW
 *    asr     x16, x16, #17
 *    add     x2, x16, x0, LSR #31
 */
class ReplaceDivToMultiAArch64 : public PeepPattern {
 public:
  explicit ReplaceDivToMultiAArch64(CGFunc &cgFunc) : PeepPattern(cgFunc) {}
  ~ReplaceDivToMultiAArch64() override = default;
  void Run(BB &bb, Insn &insn) override;
};

/*
 * Optimize the following patterns:
 *  and  w0, w0, #1  ====> and  w0, w0, #1
 *  cmp  w0, #1
 *  cset w0, EQ
 *
 *  and  w0, w0, #1  ====> and  w0, w0, #1
 *  cmp  w0, #0
 *  cset w0, NE
 *  ---------------------------------------------------
 *  and  w0, w0, #imm  ====> ubfx  w0, w0, pos, size
 *  cmp  w0, #imm
 *  cset w0, EQ
 *
 *  and  w0, w0, #imm  ====> ubfx  w0, w0, pos, size
 *  cmp  w0, #0
 *  cset w0, NE
 *  conditions:
 *  imm is pos power of 2
 */
class AndCmpBranchesToCsetAArch64 : public PeepPattern {
 public:
  explicit AndCmpBranchesToCsetAArch64(CGFunc &cgFunc) : PeepPattern(cgFunc) {}
  ~AndCmpBranchesToCsetAArch64() override = default;
  void Run(BB &bb, Insn &insn) override;
};

/*
 * We optimize the following pattern in this function:
 * cmp w[0-9]*, wzr  ====> tbz w[0-9]*, #31, .label
 * bge .label
 *
 * cmp wzr, w[0-9]*  ====> tbz w[0-9]*, #31, .label
 * ble .label
 *
 * cmp w[0-9]*,wzr   ====> tbnz w[0-9]*, #31, .label
 * blt .label
 *
 * cmp wzr, w[0-9]*  ====> tbnz w[0-9]*, #31, .label
 * bgt .label
 *
 * cmp w[0-9]*, #0   ====> tbz w[0-9]*, #31, .label
 * bge .label
 *
 * cmp w[0-9]*, #0   ====> tbnz w[0-9]*, #31, .label
 * blt .label
 */
class ZeroCmpBranchesAArch64 : public PeepPattern {
 public:
  explicit ZeroCmpBranchesAArch64(CGFunc &cgFunc) : PeepPattern(cgFunc) {}
  ~ZeroCmpBranchesAArch64() override = default;
  void Run(BB &bb, Insn &insn) override;
};

/*
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
class CmpCsetAArch64 : public PeepPattern {
 public:
  explicit CmpCsetAArch64(CGFunc &cgFunc) : PeepPattern(cgFunc) {}
  ~CmpCsetAArch64() override = default;
  void Run(BB &bb, Insn &insn) override;

 private:
  bool CheckOpndDefPoints(Insn &checkInsn, int opndIdx);
  const Insn *DefInsnOfOperandInBB(const Insn &startInsn, const Insn &checkInsn, int opndIdx);
  bool OpndDefByOneValidBit(const Insn &defInsn);
  bool FlagUsedLaterInCurBB(const BB &bb, Insn &startInsn) const;
};

/*
 *  add     x0, x1, x0
 *  ldr     x2, [x0]
 *  ==>
 *  ldr     x2, [x1, x0]
 */
class ComplexMemOperandAddAArch64 : public PeepPattern {
 public:
  explicit ComplexMemOperandAddAArch64(CGFunc &cgFunc) : PeepPattern(cgFunc) {}
  ~ComplexMemOperandAddAArch64() override = default;
  void Run(BB &bb, Insn &insn) override;
 private:

  bool IsExpandBaseOpnd(const Insn &insn, Insn &prevInsn);
};

/*
 * cbnz w0, @label
 * ....
 * mov  w0, #0 (elseBB)        -->this instruction can be deleted
 *
 * cbz  w0, @label
 * ....
 * mov  w0, #0 (ifBB)          -->this instruction can be deleted
 *
 * condition:
 *  1.there is not predefine points of w0 in elseBB(ifBB)
 *  2.the first opearnd of cbnz insn is same as the first Operand of mov insn
 *  3.w0 is defined by move 0
 *  4.all preds of elseBB(ifBB) end with cbnz or cbz
 *
 *  NOTE: if there are multiple preds and there is not define point of w0 in one pred,
 *        (mov w0, 0) can't be deleted, avoiding use before def.
 */
class DeleteMovAfterCbzOrCbnzAArch64 : public PeepPattern {
 public:
  explicit DeleteMovAfterCbzOrCbnzAArch64(CGFunc &cgFunc) : PeepPattern(cgFunc) {
    cgcfg = cgFunc.GetTheCFG();
    cgcfg->InitInsnVisitor(cgFunc);
  }
  ~DeleteMovAfterCbzOrCbnzAArch64() override = default;
  void Run(BB &bb, Insn &insn) override;

 private:
  bool PredBBCheck(BB &bb, bool checkCbz, const Operand &opnd) const;
  bool OpndDefByMovZero(const Insn &insn) const;
  bool NoPreDefine(Insn &testInsn) const;
  void ProcessBBHandle(BB *processBB, const BB &bb, const Insn &insn);
  CGCFG *cgcfg;
};

/*
 * We optimize the following pattern in this function:
 * if w0's valid bits is one
 * uxtb w0, w0
 * eor w0, w0, #1
 * cbz w0, .label
 * =>
 * tbnz w0, .label
 * &&
 * if there exists uxtb w0, w0 and w0's valid bits is
 * less than 8, eliminate it.
 */
class OneHoleBranchesPreAArch64 : public PeepPattern {
 public:
  explicit OneHoleBranchesPreAArch64(CGFunc &cgFunc) : PeepPattern(cgFunc) {}
  ~OneHoleBranchesPreAArch64() override = default;
  void Run(BB &bb, Insn &insn) override;
 private:
  MOperator FindNewMop(const BB &bb, const Insn &insn) const;
};

/*
 * We optimize the following pattern in this function:
 * movz x0, #11544, LSL #0
 * movk x0, #21572, LSL #16
 * movk x0, #8699, LSL #32
 * movk x0, #16393, LSL #48
 * =>
 * ldr x0, label_of_constant_1
 */
class LoadFloatPointAArch64 : public PeepPattern {
 public:
  explicit LoadFloatPointAArch64(CGFunc &cgFunc) : PeepPattern(cgFunc) {}
  ~LoadFloatPointAArch64() override = default;
  void Run(BB &bb, Insn &insn) override;
 private:
  bool FindLoadFloatPoint(std::vector<Insn*> &optInsn, Insn &insn);
  bool IsPatternMatch(const std::vector<Insn*> &optInsn);
};

/*
 * Optimize the following patterns:
 *  orr  w21, w0, #0  ====> mov  w21, w0
 *  orr  w21, #0, w0  ====> mov  w21, w0
 */
class ReplaceOrrToMovAArch64 : public PeepPattern {
 public:
  explicit ReplaceOrrToMovAArch64(CGFunc &cgFunc) : PeepPattern(cgFunc) {}
  ~ReplaceOrrToMovAArch64() override = default;
  void Run(BB &bb, Insn &insn) override;
};

/*
 * Optimize the following patterns:
 *  ldr  w0, [x21,#68]        ldr  w0, [x21,#68]
 *  mov  w1, #-1              mov  w1, #-1
 *  cmp  w0, w1     ====>     cmn  w0, #-1
 */
class ReplaceCmpToCmnAArch64 : public PeepPattern {
 public:
  explicit ReplaceCmpToCmnAArch64(CGFunc &cgFunc) : PeepPattern(cgFunc) {}
  ~ReplaceCmpToCmnAArch64() override = default;
  void Run(BB &bb, Insn &insn) override;
};

/*
 * Remove following patterns:
 *   mov x0, XX
 *   mov x1, XX
 *    bl  MCC_IncDecRef_NaiveRCFast
 */
class RemoveIncRefAArch64 : public PeepPattern {
 public:
  explicit RemoveIncRefAArch64(CGFunc &cgFunc) : PeepPattern(cgFunc) {}
  ~RemoveIncRefAArch64() override = default;
  void Run(BB &bb, Insn &insn) override;
};

/*
 * opt long int compare with 0
 *  *cmp x0, #0
 *  csinv w0, wzr, wzr, GE
 *  csinc w0, w0, wzr, LE
 *  cmp w0, #0
 *  =>
 *  cmp x0, #0
 */
class LongIntCompareWithZAArch64 : public PeepPattern {
 public:
  explicit LongIntCompareWithZAArch64(CGFunc &cgFunc) : PeepPattern(cgFunc) {}
  ~LongIntCompareWithZAArch64() override = default;
  void Run(BB &bb, Insn &insn) override;

 private:
  bool FindLondIntCmpWithZ(std::vector<Insn*> &optInsn, Insn &insn);
  bool IsPatternMatch(const std::vector<Insn*> &optInsn);
};

/*
 *  add     x0, x1, #:lo12:Ljava_2Futil_2FLocale_241_3B_7C_24SwitchMap_24java_24util_24Locale_24Category
 *  ldr     x2, [x0]
 *  ==>
 *  ldr     x2, [x1, #:lo12:Ljava_2Futil_2FLocale_241_3B_7C_24SwitchMap_24java_24util_24Locale_24Category]
 */
class ComplexMemOperandAArch64 : public PeepPattern {
 public:
  explicit ComplexMemOperandAArch64(CGFunc &cgFunc) : PeepPattern(cgFunc) {}
  ~ComplexMemOperandAArch64() override = default;
  void Run(BB &bb, Insn &insn) override;
};

/*
 *  add     x0, x1, x0
 *  ldr     x2, [x0]
 *  ==>
 *  ldr     x2, [x1, x0]
 */
class ComplexMemOperandPreAddAArch64 : public PeepPattern {
 public:
  explicit ComplexMemOperandPreAddAArch64(CGFunc &cgFunc) : PeepPattern(cgFunc) {}
  ~ComplexMemOperandPreAddAArch64() override = default;
  void Run(BB &bb, Insn &insn) override;
};

/*
 * add     x0, x0, x1, LSL #2
 * ldr     x2, [x0]
 * ==>
 * ldr     x2, [x0,x1,LSL #2]
 */
class ComplexMemOperandLSLAArch64 : public PeepPattern {
 public:
  explicit ComplexMemOperandLSLAArch64(CGFunc &cgFunc) : PeepPattern(cgFunc) {}
  ~ComplexMemOperandLSLAArch64() override = default;
  bool CheckShiftValid(const AArch64MemOperand &memOpnd, BitShiftOperand &lsl) const;
  void Run(BB &bb, Insn &insn) override;
};

/*
 * ldr     x0, label_of_constant_1
 * fmov    d4, x0
 * ==>
 * ldr     d4, label_of_constant_1
 */
class ComplexMemOperandLabelAArch64 : public PeepPattern {
 public:
  explicit ComplexMemOperandLabelAArch64(CGFunc &cgFunc) : PeepPattern(cgFunc) {}
  ~ComplexMemOperandLabelAArch64() override = default;
  void Run(BB &bb, Insn &insn) override;
};

/*
 * mov R0, vreg1 / R0         mov R0, vreg1
 * add vreg2, vreg1, #imm1    add vreg2, vreg1, #imm1
 * mov R1, vreg2              mov R1, vreg2
 * mov R2, vreg3              mov R2, vreg3
 * ...                        ...
 * mov R0, vreg1
 * add vreg4, vreg1, #imm2 -> str vreg5, [vreg1, #imm2]
 * mov R1, vreg4
 * mov R2, vreg5
 */
class WriteFieldCallAArch64 : public PeepPattern {
 public:
  struct WriteRefFieldParam {
    Operand *objOpnd = nullptr;
    RegOperand *fieldBaseOpnd = nullptr;
    int64 fieldOffset = 0;
    Operand *fieldValue = nullptr;
  };
  explicit WriteFieldCallAArch64(CGFunc &cgFunc) : PeepPattern(cgFunc) {}
  ~WriteFieldCallAArch64() override = default;
  void Run(BB &bb, Insn &insn) override;
  void Reset() {
    hasWriteFieldCall = false;
    prevCallInsn = nullptr;
  }

 private:
  bool hasWriteFieldCall = false;
  Insn *prevCallInsn = nullptr;
  WriteRefFieldParam firstCallParam;
  bool WriteFieldCallOptPatternMatch(const Insn &writeFieldCallInsn, WriteRefFieldParam &param,
                                     std::vector<Insn*> &paramDefInsns);
  bool IsWriteRefFieldCallInsn(const Insn &insn);
};

/*
 * Remove following patterns:
 *     mov     x0, xzr/#0
 *     bl      MCC_DecRef_NaiveRCFast
 */
class RemoveDecRefAArch64 : public PeepPattern {
 public:
  explicit RemoveDecRefAArch64(CGFunc &cgFunc) : PeepPattern(cgFunc) {}
  ~RemoveDecRefAArch64() override = default;
  void Run(BB &bb, Insn &insn) override;
};

/*
 * We optimize the following pattern in this function:
 * add x1, x1, #16
 * add w2, w10, w10
 * add w2, w2, #1
 * sxtw x2, w2
 * add x1, x1, x2, LSL #3
 * =>
 * add x1, x1, w10, SXTW #(3+1) combine origin insn 2 (self-added operation)
 * add x1, x1, #24
 */
class ComputationTreeAArch64 : public PeepPattern {
 public:
  explicit ComputationTreeAArch64(CGFunc &cgFunc) : PeepPattern(cgFunc) {}
  ~ComputationTreeAArch64() override = default;
  void Run(BB &bb, Insn &insn) override;

 private:
  bool IsPatternMatch(const std::vector<Insn*> &optInsn) const;
  bool FindComputationTree(std::vector<Insn*> &optInsn, Insn &insn);
};

/*
 * We optimize the following pattern in this function:
 * and x1, x1, #imm (is n power of 2)
 * cbz/cbnz x1, .label
 * =>
 * and x1, x1, #imm (is n power of 2)
 * tbnz/tbz x1, #n, .label
 */
class OneHoleBranchesAArch64 : public PeepPattern {
 public:
  explicit OneHoleBranchesAArch64(CGFunc &cgFunc) : PeepPattern(cgFunc) {}
  ~OneHoleBranchesAArch64() override = default;
  void Run(BB &bb, Insn &insn) override;
};

/*
 * Replace following pattern:
 * mov x1, xzr
 * bl MCC_IncDecRef_NaiveRCFast
 * =>
 * bl MCC_IncRef_NaiveRCFast
 */
class ReplaceIncDecWithIncAArch64 : public PeepPattern {
 public:
  explicit ReplaceIncDecWithIncAArch64(CGFunc &cgFunc) : PeepPattern(cgFunc) {}
  ~ReplaceIncDecWithIncAArch64() override = default;
  void Run(BB &bb, Insn &insn) override;
};

/*
 * Optimize the following patterns:
 *  and  w0, w6, #1  ====> tbz  w6, 0, .label
 *  cmp  w0, #1
 *  bne  .label
 *
 *  and  w0, w6, #16  ====> tbz  w6, 4, .label
 *  cmp  w0, #16
 *  bne  .label
 *
 *  and  w0, w6, #32  ====> tbnz  w6, 5, .label
 *  cmp  w0, #32
 *  beq  .label
 *
 *  and  x0, x6, #32  ====> tbz  x6, 5, .label
 *  cmp  x0, #0
 *  beq  .label
 *
 *  and  x0, x6, #32  ====> tbnz  x6, 5, .label
 *  cmp  x0, #0
 *  bne  .label
 */
class AndCmpBranchesToTbzAArch64 : public PeepPattern {
 public:
  explicit AndCmpBranchesToTbzAArch64(CGFunc &cgFunc) : PeepPattern(cgFunc) {}
  ~AndCmpBranchesToTbzAArch64() override = default;
  void Run(BB &bb, Insn &insn) override;
};

class AArch64PeepHole : public PeepPatternMatch {
 public:
  AArch64PeepHole(CGFunc &oneCGFunc, MemPool *memPool) : PeepPatternMatch(oneCGFunc, memPool) {}
  ~AArch64PeepHole() override = default;
  void InitOpts() override;
  void Run(BB &bb, Insn &insn) override;

 private:
  enum PeepholeOpts : int32 {
    kRemoveIdenticalLoadAndStoreOpt = 0,
    kRemoveMovingtoSameRegOpt,
    kCombineContiLoadAndStoreOpt,
    kEliminateSpecifcSXTOpt,
    kEliminateSpecifcUXTOpt,
    kFmovRegOpt,
    kCbnzToCbzOpt,
    kContiLDRorSTRToSameMEMOpt,
    kRemoveIncDecRefOpt,
    kInlineReadBarriersOpt,
    kReplaceDivToMultiOpt,
    kAndCmpBranchesToCsetOpt,
    kZeroCmpBranchesOpt,
    kPeepholeOptsNum
  };
};

class AArch64PeepHole0 : public PeepPatternMatch {
 public:
  AArch64PeepHole0(CGFunc &oneCGFunc, MemPool *memPool) : PeepPatternMatch(oneCGFunc, memPool) {}
  ~AArch64PeepHole0() override = default;
  void InitOpts() override;
  void Run(BB &bb, Insn &insn) override;

 private:
  enum PeepholeOpts : int32 {
    kRemoveIdenticalLoadAndStoreOpt = 0,
    kCmpCsetOpt,
    kComplexMemOperandOptAdd,
    kDeleteMovAfterCbzOrCbnzOpt,
    kPeepholeOptsNum
  };
};

class AArch64PrePeepHole : public PeepPatternMatch {
 public:
  AArch64PrePeepHole(CGFunc &oneCGFunc, MemPool *memPool) : PeepPatternMatch(oneCGFunc, memPool) {}
  ~AArch64PrePeepHole() override = default;
  void InitOpts() override;
  void Run(BB &bb, Insn &insn) override;

 private:
  enum PeepholeOpts : int32 {
    kOneHoleBranchesPreOpt = 0,
    kLoadFloatPointOpt,
    kReplaceOrrToMovOpt,
    kReplaceCmpToCmnOpt,
    kRemoveIncRefOpt,
    kLongIntCompareWithZOpt,
    kComplexMemOperandOpt,
    kComplexMemOperandPreOptAdd,
    kComplexMemOperandOptLSL,
    kComplexMemOperandOptLabel,
    kWriteFieldCallOpt,
    kPeepholeOptsNum
  };
};

class AArch64PrePeepHole1 : public PeepPatternMatch {
 public:
  AArch64PrePeepHole1(CGFunc &oneCGFunc, MemPool *memPool) : PeepPatternMatch(oneCGFunc, memPool) {}
  ~AArch64PrePeepHole1() override = default;
  void InitOpts() override;
  void Run(BB &bb, Insn &insn) override;

 private:
  enum PeepholeOpts : int32 {
    kRemoveDecRefOpt = 0,
    kComputationTreeOpt,
    kOneHoleBranchesOpt,
    kReplaceIncDecWithIncOpt,
    kAndCmpBranchesToTbzOpt,
    kPeepholeOptsNum
  };
};
}  /* namespace maplebe */
#endif  /* MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_PEEP_H */
