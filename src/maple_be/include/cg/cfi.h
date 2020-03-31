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
#ifndef MAPLEBE_INCLUDE_CG_CFI_H
#define MAPLEBE_INCLUDE_CG_CFI_H

#include "insn.h"
#include "mempool_allocator.h"
#include "mir_symbol.h"

/*
 * Reference:
 * GNU Binutils. AS documentation
 * https://sourceware.org/binutils/docs-2.28/as/index.html
 *
 * CFI blog
 * https://www.imperialviolet.org/2017/01/18/cfi.html
 *
 * System V Application Binary Interface
 * AMD64 Architecture Processor Supplement. Draft Version 0.99.7
 * https://www.uclibc.org/docs/psABI-x86_64.pdf $ 3.7 Figure 3.36
 * (RBP->6, RSP->7)
 *
 * System V Application Binary Interface
 * Inte386 Architecture Processor Supplement. Version 1.0
 * https://www.uclibc.org/docs/psABI-i386.pdf $ 2.5 Table 2.14
 * (EBP->5, ESP->4)
 *
 * DWARF for ARM Architecture (ARM IHI 0040B)
 * infocenter.arm.com/help/topic/com.arm.doc.ihi0040b/IHI0040B_aadwarf.pdf
 * $ 3.1 Table 1
 * (0-15 -> R0-R15)
 */
namespace cfi {
using namespace maple;

enum CfiOpcode : uint8 {
#define CFI_DEFINE(k, sub, n, o0, o1, o2) OP_CFI_##k##sub,
#include "cfi.def"
#undef CFI_DEFINE
  kOpCfiLast
};

class CfiInsn : public maplebe::Insn {
 public:
  CfiInsn(MemPool &memPool, maplebe::MOperator op) : Insn(memPool, op) {}

  CfiInsn(MemPool &memPool, maplebe::MOperator op, maplebe::Operand &opnd0) : Insn(memPool, op, opnd0) {}

  CfiInsn(MemPool &memPool, maplebe::MOperator op, maplebe::Operand &opnd0, maplebe::Operand &opnd1)
      : Insn(memPool, op, opnd0, opnd1) {}

  CfiInsn(const CfiInsn &originalInsn, MemPool &memPool) : Insn(memPool, originalInsn.mOp) {
    InitWithOriginalInsn(originalInsn, memPool);
  }

  ~CfiInsn() = default;

  bool IsMachineInstruction() const override {
    return false;
  }

  void Emit(const maplebe::CG &cg, maplebe::Emitter &emitter) const override;

  void Dump() const override;

  bool Check() const override;

  bool IsDefinition() const override {
    return false;
  }

  bool IsCfiInsn() const override {
    return true;
  }

 private:
  CfiInsn &operator=(const CfiInsn&);
};

class RegOperand : public maplebe::Operand {
 public:
  RegOperand(uint32 no, uint32 size) : Operand(kOpdRegister, size), regNO(no) {}

  ~RegOperand() = default;

  Operand *Clone(MemPool &memPool) const override {
    Operand *opnd = memPool.Clone<RegOperand>(*this);
    return opnd;
  }

  void Emit(maplebe::Emitter &emitter, const maplebe::OpndProp *prop) const override;

  void Dump() const override;

  bool Less(const Operand &right) const override {
    (void)right;
    return false;
  }

 private:
  uint32 regNO;
};

class ImmOperand : public maplebe::Operand {
 public:
  ImmOperand(int64 val, uint32 size) : Operand(kOpdImmediate, size), val(val) {}

  ~ImmOperand() = default;

  Operand *Clone(MemPool &memPool) const override {
    Operand *opnd =  memPool.Clone<ImmOperand>(*this);
    return opnd;
  }

  void Emit(maplebe::Emitter &emitter, const maplebe::OpndProp *prop) const override;

  void Dump() const override;

  bool Less(const Operand &right) const override {
    (void)right;
    return false;
  }

 private:
  int64 val;
};

class SymbolOperand : public maplebe::Operand {
 public:
  SymbolOperand(maple::MIRSymbol &mirSymbol, uint8 size) : Operand(kOpdStImmediate, size), symbol(&mirSymbol) {}
  ~SymbolOperand() = default;

  Operand *Clone(MemPool &memPool) const override {
    Operand *opnd =  memPool.Clone<SymbolOperand>(*this);
    return opnd;
  }

  void Emit(maplebe::Emitter &emitter, const maplebe::OpndProp *prop) const override;

  bool Less(const Operand &right) const override {
    (void)right;
    return false;
  }

  void Dump() const override {}

 private:
  maple::MIRSymbol *symbol;
};

class StrOperand : public maplebe::Operand {
 public:
  StrOperand(const std::string &str, MemPool &memPool) : Operand(kOpdString, 0), str(str, &memPool) {}

  ~StrOperand() = default;

  Operand *Clone(MemPool &memPool) const override {
    Operand *opnd = memPool.Clone<StrOperand>(*this);
    return opnd;
  }

  void Emit(maplebe::Emitter &emitter, const maplebe::OpndProp *prop) const override;

  bool Less(const Operand &right) const override {
    (void)right;
    return false;
  }

  void Dump() const override;

 private:
  const MapleString str;
};

class LabelOperand : public maplebe::Operand {
 public:
  LabelOperand(const std::string &parent, LabelIdx labIdx, MemPool &memPool)
      : Operand(kOpdBBAddress, 0), parentFunc(parent, &memPool), labelIndex(labIdx) {}

  ~LabelOperand() = default;

  Operand *Clone(MemPool &memPool) const override {
    Operand *opnd = memPool.Clone<LabelOperand>(*this);
    return opnd;
  }

  void Emit(maplebe::Emitter &emitter, const maplebe::OpndProp *opndProp) const override;

  bool Less(const Operand &right) const override {
    (void)right;
    return false;
  }

  void Dump() const override;

 private:
  const MapleString parentFunc;
  LabelIdx labelIndex;
};
}  /* namespace cfi */

#endif  /* MAPLEBE_INCLUDE_CG_CFI_H */