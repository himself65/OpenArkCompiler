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
#include "cfi.h"
#include "emit.h"

namespace cfi {
using maplebe::Operand;
using maplebe::MOperator;
using maplebe::CG;
using maplebe::Emitter;
using maplebe::OpndProp;

struct CfiDescr {
  const std::string name;
  int32 opndCount;
  /* create 3 OperandType array to store cfi instruction's operand type */
  std::array<Operand::OperandType, 3> opndTypes;
};

static CfiDescr cfiDescrTable[kOpCfiLast + 1] = {
#define CFI_DEFINE(k, sub, n, o0, o1, o2) \
  { ".cfi_" #k, n, { Operand::kOpd##o0, Operand::kOpd##o1, Operand::kOpd##o2 } },
#define ARM_DIRECTIVES_DEFINE(k, sub, n, o0, o1, o2) \
  { "." #k, n, { Operand::kOpd##o0, Operand::kOpd##o1, Operand::kOpd##o2 } },
#include "cfi.def"
#undef CFI_DEFINE
#undef ARM_DIRECTIVES_DEFINE
  { ".cfi_undef", 0, { Operand::kOpdUndef, Operand::kOpdUndef, Operand::kOpdUndef } }
};

void CfiInsn::Dump() const {
  MOperator mOp = GetMachineOpcode();
  CfiDescr &cfiDescr = cfiDescrTable[mOp];
  LogInfo::MapleLogger() << "CFI " << cfiDescr.name;
  for (int32 i = 0; i < cfiDescr.opndCount; ++i) {
    LogInfo::MapleLogger() << (i == 0 ? " : " : " ");
    Operand &curOperand = GetOperand(i);
    curOperand.Dump();
  }
  LogInfo::MapleLogger() << "\n";
}

bool CfiInsn::Check() const {
  CfiDescr &cfiDescr = cfiDescrTable[GetMachineOpcode()];
  /* cfi instruction's 3rd /4th/5th operand must be null */
  for (int32 i = 0; i < cfiDescr.opndCount; ++i) {
    Operand &opnd = GetOperand(i);
    if (opnd.GetKind() != cfiDescr.opndTypes[i]) {
      ASSERT(false, "incorrect operand");
      return false;
    }
  }
  return true;
}

void CfiInsn::Emit(const CG &cg, Emitter &emitter) const {
  (void)cg;
  MOperator mOp = GetMachineOpcode();
  CfiDescr &cfiDescr = cfiDescrTable[mOp];
  emitter.Emit("\t").Emit(cfiDescr.name);
  for (int32 i = 0; i < cfiDescr.opndCount; ++i) {
    emitter.Emit(" ");
    Operand &curOperand = GetOperand(i);
    curOperand.Emit(emitter, nullptr);
    if (i < (cfiDescr.opndCount - 1)) {
      emitter.Emit(",");
    }
  }
  emitter.Emit("\n");
}

void RegOperand::Emit(Emitter &emitter, const OpndProp*) const {
  emitter.Emit(regNO);
}

void RegOperand::Dump() const {
  LogInfo::MapleLogger() << "reg: " << regNO << "[ size: " << GetSize() << "] ";
}

void ImmOperand::Emit(Emitter &emitter, const OpndProp*) const {
  emitter.Emit(val);
}

void ImmOperand::Dump() const {
  LogInfo::MapleLogger() << "imm: " << val << "[ size: " << GetSize() << "] ";
}

void StrOperand::Emit(Emitter &emitter, const OpndProp *opndProp) const {
  (void)opndProp;
  emitter.Emit(str);
}

void StrOperand::Dump() const {
  LogInfo::MapleLogger() << str;
}

void LabelOperand::Emit(Emitter &emitter, const OpndProp *opndProp) const {
  (void)opndProp;
  emitter.Emit(".label.").Emit(parentFunc).Emit(labelIndex);
}

void LabelOperand::Dump() const {
  LogInfo::MapleLogger() << "label:" << labelIndex;
}
}  /* namespace cfi */

namespace maplebe {
AnalysisResult *CgDoGenCfi::Run(CGFunc *cgFunc, CgFuncResultMgr *cgFuncResultMgr) {
  (void)cgFuncResultMgr;
  ASSERT(cgFunc != nullptr, "expect a cgfunc in CgDoGenCfi");
  cgFunc->GenerateCfiPrologEpilog();
  return nullptr;
}
}  /* namespace maplebe */
