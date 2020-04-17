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
#include "jbc_stmt.h"
#include <string>
#include <iostream>
#include <algorithm>
#include "opcodes.h"
#include "feir_stmt.h"
#include "feir_builder.h"
#include "fe_struct_elem_info.h"
#include "jbc_function_context.h"

namespace maple {
// ---------- JBCStmtKindHelper ----------
std::string JBCStmtKindHelper::JBCStmtKindName(JBCStmtKind kind) {
  switch (kind) {
    case kJBCStmtDefault:
      return "JBCStmtDefault";
    case kJBCStmtFuncBeing:
      return "JBCStmtFuncBegin";
    case kJBCStmtFuncEnd:
      return "JBCStmtFuncEnd";
    case kJBCStmtInst:
      return "JBCStmtInst";
    case kJBCStmtInstBranch:
      return "JBCStmtInstBranch";
    case kJBCStmtPesudoComment:
      return "JBCStmtPesudoComment";
    case kJBCStmtPesudoLabel:
      return "JBCStmtPesudoLabel";
    case kJBCStmtPesudoLOC:
      return "JBCStmtPesudoLOC";
    case kJBCStmtPesudoTry:
      return "JBCStmtPesudoTry";
    case kJBCStmtPesudoEndTry:
      return "JBCStmtPesudoEndTry";
    case kJBCStmtPesudoCatch:
      return "JBCStmtPesudoCatch";
    default:
      return "unknown";
  }
}

// ---------- JBCStmtInst ----------
std::map<jbc::JBCOpcodeKind, JBCStmtInst::FuncPtrEmitToFEIR> JBCStmtInst::funcPtrMapForEmitToFEIR =
    JBCStmtInst::InitFuncPtrMapForEmitToFEIR();
std::map<jbc::JBCOpcode, Opcode> JBCStmtInst::opcodeMapForMathBinop = JBCStmtInst::InitOpcodeMapForMathBinop();
std::map<jbc::JBCOpcode, Opcode> JBCStmtInst::opcodeMapForMathUnop = JBCStmtInst::InitOpcodeMapForMathUnop();
std::map<jbc::JBCOpcode, Opcode> JBCStmtInst::opcodeMapForMonitor = JBCStmtInst::InitOpcodeMapForMonitor();

JBCStmtInst::JBCStmtInst(const jbc::JBCOp &argOp)
    : JBCStmt(kJBCStmtInst),
      op(argOp) {
  SetFallThru(op.IsFallThru());
}

bool JBCStmtInst::IsStmtInstImpl() const {
  return true;
}

void JBCStmtInst::DumpImpl(const std::string &prefix) const {
  std::cout << prefix << "JBCStmtInst (id=" << id << ", " <<
               "kind=" << JBCStmtKindHelper::JBCStmtKindName(kind) << ", " <<
               "op=" << op.GetOpcodeName() <<
               ")" << std::endl;
}

std::string JBCStmtInst::DumpDotStringImpl() const {
  std::stringstream ss;
  ss << "<stmt" << id << "> " << id << ": " << op.GetOpcodeName();
  return ss.str();
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRImpl(JBCFunctionContext &context, bool &success) const {
  auto it = funcPtrMapForEmitToFEIR.find(op.GetOpcodeKind());
  if (it != funcPtrMapForEmitToFEIR.end()) {
    return (this->*(it->second))(context, success);
  } else {
    return EmitToFEIRCommon(context, success);
  }
}

std::map<jbc::JBCOpcodeKind, JBCStmtInst::FuncPtrEmitToFEIR> JBCStmtInst::InitFuncPtrMapForEmitToFEIR() {
  std::map<jbc::JBCOpcodeKind, FuncPtrEmitToFEIR> ans;
  ans[jbc::JBCOpcodeKind::kOpKindConst] = &JBCStmtInst::EmitToFEIRForOpConst;
  ans[jbc::JBCOpcodeKind::kOpKindLoad] = &JBCStmtInst::EmitToFEIRForOpLoad;
  ans[jbc::JBCOpcodeKind::kOpKindStore] = &JBCStmtInst::EmitToFEIRForOpStore;
  ans[jbc::JBCOpcodeKind::kOpKindArrayLoad] = &JBCStmtInst::EmitToFEIRForOpArrayLoad;
  ans[jbc::JBCOpcodeKind::kOpKindArrayStore] = &JBCStmtInst::EmitToFEIRForOpArrayStore;
  ans[jbc::JBCOpcodeKind::kOpKindPop] = &JBCStmtInst::EmitToFEIRForOpPop;
  ans[jbc::JBCOpcodeKind::kOpKindDup] = &JBCStmtInst::EmitToFEIRForOpDup;
  ans[jbc::JBCOpcodeKind::kOpKindSwap] = &JBCStmtInst::EmitToFEIRForOpSwap;
  ans[jbc::JBCOpcodeKind::kOpKindMathBinop] = &JBCStmtInst::EmitToFEIRForOpMathBinop;
  ans[jbc::JBCOpcodeKind::kOpKindMathUnop] = &JBCStmtInst::EmitToFEIRForOpMathUnop;
  ans[jbc::JBCOpcodeKind::kOpKindMathInc] = &JBCStmtInst::EmitToFEIRForOpMathInc;
  ans[jbc::JBCOpcodeKind::kOpKindConvert] = &JBCStmtInst::EmitToFEIRForOpConvert;
  ans[jbc::JBCOpcodeKind::kOpKindCompare] = &JBCStmtInst::EmitToFEIRForOpMathBinop;
  ans[jbc::JBCOpcodeKind::kOpKindReturn] = &JBCStmtInst::EmitToFEIRForOpReturn;
  ans[jbc::JBCOpcodeKind::kOpKindStaticFieldOpr] = &JBCStmtInst::EmitToFEIRForOpStaticFieldOpr;
  ans[jbc::JBCOpcodeKind::kOpKindFieldOpr] = &JBCStmtInst::EmitToFEIRForOpFieldOpr;
  ans[jbc::JBCOpcodeKind::kOpKindInvoke] = &JBCStmtInst::EmitToFEIRForOpInvoke;
  ans[jbc::JBCOpcodeKind::kOpKindNew] = &JBCStmtInst::EmitToFEIRForOpNew;
  ans[jbc::JBCOpcodeKind::kOpKindMultiANewArray] = &JBCStmtInst::EmitToFEIRForOpMultiANewArray;
  ans[jbc::JBCOpcodeKind::kOpKindThrow] = &JBCStmtInst::EmitToFEIRForOpThrow;
  ans[jbc::JBCOpcodeKind::kOpKindTypeCheck] = &JBCStmtInst::EmitToFEIRForOpTypeCheck;
  ans[jbc::JBCOpcodeKind::kOpKindMonitor] = &JBCStmtInst::EmitToFEIRForOpMonitor;
  ans[jbc::JBCOpcodeKind::kOpKindArrayLength] = &JBCStmtInst::EmitToFEIRForOpArrayLength;
  return ans;
}

std::map<jbc::JBCOpcode, Opcode> JBCStmtInst::InitOpcodeMapForMathBinop() {
  std::map<jbc::JBCOpcode, Opcode> ans;
  ans[jbc::kOpIAdd] = OP_add;
  ans[jbc::kOpLAdd] = OP_add;
  ans[jbc::kOpFAdd] = OP_add;
  ans[jbc::kOpDAdd] = OP_add;
  ans[jbc::kOpISub] = OP_sub;
  ans[jbc::kOpLSub] = OP_sub;
  ans[jbc::kOpFSub] = OP_sub;
  ans[jbc::kOpDSub] = OP_sub;
  ans[jbc::kOpIMul] = OP_mul;
  ans[jbc::kOpLMul] = OP_mul;
  ans[jbc::kOpFMul] = OP_mul;
  ans[jbc::kOpDMul] = OP_mul;
  ans[jbc::kOpIDiv] = OP_div;
  ans[jbc::kOpLDiv] = OP_div;
  ans[jbc::kOpFDiv] = OP_div;
  ans[jbc::kOpDDiv] = OP_div;
  ans[jbc::kOpIRem] = OP_rem;
  ans[jbc::kOpLRem] = OP_rem;
  ans[jbc::kOpFRem] = OP_rem;
  ans[jbc::kOpDRem] = OP_rem;
  ans[jbc::kOpIShl] = OP_shl;
  ans[jbc::kOpLShl] = OP_shl;
  ans[jbc::kOpIShr] = OP_ashr;
  ans[jbc::kOpLShr] = OP_ashr;
  ans[jbc::kOpIUShr] = OP_lshr;
  ans[jbc::kOpLUShr] = OP_lshr;
  ans[jbc::kOpIAnd] = OP_band;
  ans[jbc::kOpLAnd] = OP_band;
  ans[jbc::kOpIOr] = OP_bior;
  ans[jbc::kOpLOr] = OP_bior;
  ans[jbc::kOpIXor] = OP_bxor;
  ans[jbc::kOpLXor] = OP_bxor;
  ans[jbc::kOpLCmp] = OP_cmp;
  ans[jbc::kOpFCmpl] = OP_cmpl;
  ans[jbc::kOpFCmpg] = OP_cmpg;
  ans[jbc::kOpDCmpl] = OP_cmpl;
  ans[jbc::kOpDCmpg] = OP_cmpg;
  return ans;
}

std::map<jbc::JBCOpcode, Opcode> JBCStmtInst::InitOpcodeMapForMathUnop() {
  std::map<jbc::JBCOpcode, Opcode> ans;
  ans[jbc::kOpINeg] = OP_neg;
  ans[jbc::kOpLNeg] = OP_neg;
  ans[jbc::kOpFNeg] = OP_neg;
  ans[jbc::kOpDNeg] = OP_neg;
  return ans;
}

std::map<jbc::JBCOpcode, Opcode> JBCStmtInst::InitOpcodeMapForMonitor() {
  std::map<jbc::JBCOpcode, Opcode> ans;
  ans[jbc::kOpMonitorEnter] = OP_syncenter;
  ans[jbc::kOpMonitorExit] = OP_syncexit;
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpConst(JBCFunctionContext &context, bool &success) const {
  switch (op.GetOpcode()) {
    case jbc::kOpAConstNull:
      return EmitToFEIRForOpAConstNull(context, success);
    case jbc::kOpIConstM1:
    case jbc::kOpIConst0:
    case jbc::kOpIConst1:
    case jbc::kOpIConst2:
    case jbc::kOpIConst3:
    case jbc::kOpIConst4:
    case jbc::kOpIConst5:
      return EmitToFEIRForOpIConst(context, success);
    case jbc::kOpLConst0:
    case jbc::kOpLConst1:
      return EmitToFEIRForOpLConst(context, success);
    case jbc::kOpFConst0:
    case jbc::kOpFConst1:
    case jbc::kOpFConst2:
      return EmitToFEIRForOpFConst(context, success);
    case jbc::kOpDConst0:
    case jbc::kOpDConst1:
      return EmitToFEIRForOpDConst(context, success);
    case jbc::kOpBiPush:
      return EmitToFEIRForOpBiPush(context, success);
    case jbc::kOpSiPush:
      return EmitToFEIRForOpSiPush(context, success);
    case jbc::kOpLdc:
    case jbc::kOpLdcW:
    case jbc::kOpLdc2W:
      return EmitToFEIRForOpLdc(context, success);
    default:
      ERR(kLncErr, "EmitToFEIRForOpConst: unsupport jbc opcode %s",
          jbc::JBCOp::GetOpcodeInfo().GetOpcodeName(op.GetOpcode()).c_str());
      success = false;
      return std::list<UniqueFEIRStmt>();
  }
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpConstCommon(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  const jbc::JBCConstPool &constPool = context.GetConstPool();
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  jbc::JBCPrimType stackOutType = op.GetOutputTypesToStack(constPool);
  if (stackOutType != jbc::JBCPrimType::kTypeDefault) {
    PrimType pty = JBCStack2FEHelper::JBCStackItemTypeToPrimType(stackOutType);
    UniqueFEIRVar var = stack2feHelper.PushItem(pty);
    if (var == nullptr) {
      success = false;
    }
  } else {
    success = false;
  }
  if (!success) {
    ERR(kLncErr, "Error when EmitToFEIRForOpConst");
  }
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpAConstNull(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  UniqueFEIRExpr exprConst = FEIRBuilder::CreateExprConstRefNull();
  UniqueFEIRVar varDst = stack2feHelper.PushItem(PTY_ref);
  if (varDst == nullptr) {
    success = false;
    return ans;
  }
  UniqueFEIRStmt stmtDAssign = FEIRBuilder::CreateStmtDAssign(std::move(varDst), std::move(exprConst));
  ans.push_back(std::move(stmtDAssign));
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpIConst(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  const jbc::JBCOpConst &opConst = static_cast<const jbc::JBCOpConst&>(op);
  int32 val = opConst.GetValueInt();
  UniqueFEIRStmt stmtDAssign = GenerateStmtForConstI32(stack2feHelper, val, success);
  if (success) {
    ans.push_back(std::move(stmtDAssign));
  }
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpLConst(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  const jbc::JBCOpConst &opConst = static_cast<const jbc::JBCOpConst&>(op);
  int64 val = opConst.GetValueLong();
  UniqueFEIRStmt stmtDAssign = GenerateStmtForConstI64(stack2feHelper, val, success);
  if (success) {
    ans.push_back(std::move(stmtDAssign));
  }
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpFConst(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  const jbc::JBCOpConst &opConst = static_cast<const jbc::JBCOpConst&>(op);
  float val = opConst.GetValueFloat();
  UniqueFEIRStmt stmtDAssign = GenerateStmtForConstF32(stack2feHelper, val, success);
  if (success) {
    ans.push_back(std::move(stmtDAssign));
  }
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpDConst(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  const jbc::JBCOpConst &opConst = static_cast<const jbc::JBCOpConst&>(op);
  double val = opConst.GetValueDouble();
  UniqueFEIRStmt stmtDAssign = GenerateStmtForConstF64(stack2feHelper, val, success);
  if (success) {
    ans.push_back(std::move(stmtDAssign));
  }
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpBiPush(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  const jbc::JBCOpConst &opConst = static_cast<const jbc::JBCOpConst&>(op);
  int8 val = opConst.GetValueByte();
  UniqueFEIRStmt stmtDAssign = GenerateStmtForConstI32(stack2feHelper, int32{ val }, success);
  if (success) {
    ans.push_back(std::move(stmtDAssign));
  }
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpSiPush(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  const jbc::JBCOpConst &opConst = static_cast<const jbc::JBCOpConst&>(op);
  int16 val = opConst.GetValueShort();
  UniqueFEIRStmt stmtDAssign = GenerateStmtForConstI32(stack2feHelper, int32{ val }, success);
  if (success) {
    ans.push_back(std::move(stmtDAssign));
  }
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpLdc(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  const jbc::JBCConstPool &constPool = context.GetConstPool();
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  const jbc::JBCOpConst &opConst = static_cast<const jbc::JBCOpConst&>(op);
  const jbc::JBCConst *constRaw = constPool.GetConstByIdx(opConst.GetIndex());
  if (constRaw == nullptr) {
    success = false;
    return ans;
  }
  UniqueFEIRStmt stmtDAssign;
  switch (constRaw->GetTag()) {
    case jbc::kConstInteger: {
      const jbc::JBCConst4Byte *const4B = static_cast<const jbc::JBCConst4Byte*>(constRaw);
      stmtDAssign = GenerateStmtForConstI32(stack2feHelper, const4B->GetInt32(), success);
      break;
    }
    case jbc::kConstFloat: {
      const jbc::JBCConst4Byte *const4B = static_cast<const jbc::JBCConst4Byte*>(constRaw);
      stmtDAssign = GenerateStmtForConstF32(stack2feHelper, const4B->GetFloat(), success);
      break;
    }
    case jbc::kConstLong: {
      const jbc::JBCConst8Byte *const8B = static_cast<const jbc::JBCConst8Byte*>(constRaw);
      stmtDAssign = GenerateStmtForConstI64(stack2feHelper, const8B->GetInt64(), success);
      break;
    }
    case jbc::kConstDouble: {
      const jbc::JBCConst8Byte *const8B = static_cast<const jbc::JBCConst8Byte*>(constRaw);
      stmtDAssign = GenerateStmtForConstF64(stack2feHelper, const8B->GetDouble(), success);
      break;
    }
    case jbc::kConstString: {
      const jbc::JBCConstString *constString = static_cast<const jbc::JBCConstString*>(constRaw);
      UniqueFEIRVar varDst = stack2feHelper.PushItem(PTY_ref);
      stmtDAssign = FEIRBuilder::CreateStmtJavaConstString(std::move(varDst), constString->GetStrIdx());
      break;
    }
    case jbc::kConstClass: {
      const jbc::JBCConstClass *constClass = static_cast<const jbc::JBCConstClass*>(constRaw);
      UniqueFEIRVar varDst = stack2feHelper.PushItem(PTY_ref);
      stmtDAssign = FEIRBuilder::CreateStmtJavaConstClass(std::move(varDst), constClass->GetFEIRType()->Clone());
      break;
    }
    default:
      ERR(kLncErr, "EmitToFEIRForOpLdc: unsupported const kind");
      success = false;
  }
  if (success && stmtDAssign != nullptr) {
    ans.push_back(std::move(stmtDAssign));
  }
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpLoad(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  const jbc::JBCOpSlotOpr &opLoad = static_cast<const jbc::JBCOpSlotOpr&>(op);
  std::vector<jbc::JBCPrimType> stackInTypes = op.GetInputTypesFromStack();
  CHECK_FATAL(stackInTypes.empty(), "no items should be popped");
  jbc::JBCPrimType stackOutType = op.GetOutputTypesToStack();
  PrimType pty = JBCStack2FEHelper::JBCStackItemTypeToPrimType(stackOutType);
  uint32 regNum = stack2feHelper.GetRegNumForSlot(opLoad.GetSlotIdx());
  const FEIRType *slotType = context.GetSlotType(opLoad.GetSlotIdx(), pc);
  UniqueFEIRVar var;
  UniqueFEIRVar varStack = stack2feHelper.PushItem(pty);
  if (slotType != nullptr) {
    var = FEIRBuilder::CreateVarReg(regNum, slotType->Clone());
    varStack->SetType(slotType->Clone());
  } else {
    var = FEIRBuilder::CreateVarReg(regNum, pty);
  }
  success = success && (varStack != nullptr);
  UniqueFEIRExpr expr = FEIRBuilder::CreateExprDRead(std::move(var));
  UniqueFEIRStmt stmt = FEIRBuilder::CreateStmtDAssign(std::move(varStack), std::move(expr));
  ans.push_back(std::move(stmt));
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpStore(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  const jbc::JBCOpSlotOpr &opStore = static_cast<const jbc::JBCOpSlotOpr&>(op);
  if (opStore.IsAddressOpr()) {
    UniqueFEIRVar varTmp = stack2feHelper.PopItem(PTY_a32);
    if (varTmp.get() == nullptr) {
      success = false;
    }
    return ans;
  }
  std::vector<jbc::JBCPrimType> stackInTypes = op.GetInputTypesFromStack();
  CHECK_FATAL(stackInTypes.size() == 1, "store op need one stack opnd");
  PrimType pty = JBCStack2FEHelper::JBCStackItemTypeToPrimType(stackInTypes[0]);
  uint32 regSlot = stack2feHelper.GetRegNumForSlot(opStore.GetSlotIdx());
  const FEIRType *slotType = context.GetSlotType(opStore.GetSlotIdx(), pc);
  UniqueFEIRVar varDst;
  UniqueFEIRVar varSrc;
  if (slotType != nullptr) {
    varDst = FEIRBuilder::CreateVarReg(regSlot, slotType->Clone());
    varSrc = stack2feHelper.PopItem(pty);
    varSrc->SetType(slotType->Clone());
  } else {
    varDst = FEIRBuilder::CreateVarReg(regSlot, pty);
    varSrc = stack2feHelper.PopItem(pty);
  }
  if (varSrc == nullptr) {
    success = false;
    return ans;
  }
  UniqueFEIRExpr expr = FEIRBuilder::CreateExprDRead(std::move(varSrc));
  UniqueFEIRStmt stmt = FEIRBuilder::CreateStmtDAssign(std::move(varDst), std::move(expr));
  ans.push_back(std::move(stmt));
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpArrayLoad(JBCFunctionContext &context, bool &success) const {
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  // Process In
  std::vector<jbc::JBCPrimType> stackInTypes = op.GetInputTypesFromStack();
  // ArrayLoad need 2 input opnds
  CHECK_FATAL(stackInTypes.size() == 2, "invalid in types for ArrayLoad");
  PrimType ptyArray = JBCStack2FEHelper::JBCStackItemTypeToPrimType(stackInTypes[0]);  // opnd0: array
  PrimType ptyIndex = JBCStack2FEHelper::JBCStackItemTypeToPrimType(stackInTypes[1]);  // opnd1: index
  UniqueFEIRVar varIndex = stack2feHelper.PopItem(ptyIndex);
  UniqueFEIRVar varArray = stack2feHelper.PopItem(ptyArray);
  // Process Out
  jbc::JBCPrimType stackOutType = op.GetOutputTypesToStack();
  CHECK_FATAL(stackOutType != jbc::JBCPrimType::kTypeDefault, "invalid out type for ArrayLoad");
  PrimType ptyElem = JBCStack2FEHelper::JBCStackItemTypeToPrimType(stackOutType);
  UniqueFEIRVar varElem = stack2feHelper.PushItem(ptyElem);
  return FEIRBuilder::CreateStmtArrayLoad(std::move(varElem), std::move(varArray), std::move(varIndex), ptyElem);
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpArrayStore(JBCFunctionContext &context, bool &success) const {
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  // Process In
  std::vector<jbc::JBCPrimType> stackInTypes = op.GetInputTypesFromStack();
  // ArrayStore need 3 input opnds
  CHECK_FATAL(stackInTypes.size() == 3, "invalid in types for ArrayStore");
  PrimType ptyArray = JBCStack2FEHelper::JBCStackItemTypeToPrimType(stackInTypes[0]);  // opnd0: array
  PrimType ptyIndex = JBCStack2FEHelper::JBCStackItemTypeToPrimType(stackInTypes[1]);  // opnd1: index
  PrimType ptyElem = JBCStack2FEHelper::JBCStackItemTypeToPrimType(stackInTypes[2]);  // opnd2: elem
  UniqueFEIRVar varElem = stack2feHelper.PopItem(ptyElem);
  UniqueFEIRVar varIndex = stack2feHelper.PopItem(ptyIndex);
  UniqueFEIRVar varArray = stack2feHelper.PopItem(ptyArray);
  return FEIRBuilder::CreateStmtArrayStore(std::move(varElem), std::move(varArray), std::move(varIndex), ptyElem);
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpPop(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  success = stack2feHelper.Pop(op.GetOpcode());
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpDup(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  success = stack2feHelper.Dup(op.GetOpcode());
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpSwap(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  success = stack2feHelper.Swap();
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpMathBinop(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  // obtain in/out types
  std::vector<jbc::JBCPrimType> stackInTypes = op.GetInputTypesFromStack();
  CHECK_FATAL(stackInTypes.size() == 2, "Not enough input opnds for math binary op"); // 2 : opnds num limit
  PrimType pty0 = JBCStack2FEHelper::JBCStackItemTypeToPrimType(stackInTypes[0]);
  PrimType pty1 = JBCStack2FEHelper::JBCStackItemTypeToPrimType(stackInTypes[1]);
  jbc::JBCPrimType stackOutType = op.GetOutputTypesToStack();
  PrimType ptyOut = JBCStack2FEHelper::JBCStackItemTypeToPrimType(stackOutType);
  // stack operation
  UniqueFEIRVar var1 = stack2feHelper.PopItem(pty1);
  UniqueFEIRVar var0 = stack2feHelper.PopItem(pty0);
  UniqueFEIRVar varOut = stack2feHelper.PushItem(ptyOut);
  if (var1 == nullptr || var0 == nullptr || varOut == nullptr) {
    success = false;
    return ans;
  }
  auto it = opcodeMapForMathBinop.find(op.GetOpcode());
  if (it == opcodeMapForMathBinop.end()) {
    success = false;
    ERR(kLncErr, "EmitToFEIRForOpMathBinop: unsupport opcode %s", op.GetOpcodeName().c_str());
    return ans;
  }
  UniqueFEIRExpr expr = FEIRBuilder::CreateExprMathBinary(it->second, std::move(var0), std::move(var1));
  UniqueFEIRStmt stmt = FEIRBuilder::CreateStmtDAssign(std::move(varOut), std::move(expr));
  ans.push_back(std::move(stmt));
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpMathUnop(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  // obtain in/out types
  std::vector<jbc::JBCPrimType> stackInTypes = op.GetInputTypesFromStack();
  ASSERT(stackInTypes.size() == 1, "Not enough input opnds for math unary op");
  PrimType pty0 = JBCStack2FEHelper::JBCStackItemTypeToPrimType(stackInTypes[0]);
  jbc::JBCPrimType stackOutType = op.GetOutputTypesToStack();
  PrimType ptyOut = JBCStack2FEHelper::JBCStackItemTypeToPrimType(stackOutType);
  // stack operation
  UniqueFEIRVar var0 = stack2feHelper.PopItem(pty0);
  UniqueFEIRVar varOut = stack2feHelper.PushItem(ptyOut);
  if (var0 == nullptr || varOut == nullptr) {
    success = false;
    return ans;
  }
  auto it = opcodeMapForMathUnop.find(op.GetOpcode());
  if (it == opcodeMapForMathUnop.end()) {
    success = false;
    ERR(kLncErr, "EmitToFEIRForOpMathUnop: unsupport opcode %s", op.GetOpcodeName().c_str());
    return ans;
  }
  UniqueFEIRExpr expr = FEIRBuilder::CreateExprMathUnary(it->second, std::move(var0));
  UniqueFEIRStmt stmt = FEIRBuilder::CreateStmtDAssign(std::move(varOut), std::move(expr));
  ans.push_back(std::move(stmt));
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpMathInc(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  const jbc::JBCOpMathInc &opInc = static_cast<const jbc::JBCOpMathInc&>(op);
  // no stack operation
  uint32 regNum = stack2feHelper.GetRegNumForSlot(opInc.GetIndex());
  UniqueFEIRVar var0 = FEIRBuilder::CreateVarReg(regNum, PTY_i32);
  UniqueFEIRVar varOut = var0->Clone();
  UniqueFEIRExpr opnd0 = FEIRBuilder::CreateExprDRead(std::move(var0));
  UniqueFEIRExpr opnd1 = FEIRBuilder::CreateExprConstI32(int32{ opInc.GetIncr() });
  UniqueFEIRExpr expr = FEIRBuilder::CreateExprMathBinary(OP_add, std::move(opnd0), std::move(opnd1));
  UniqueFEIRStmt stmt = FEIRBuilder::CreateStmtDAssign(std::move(varOut), std::move(expr));
  ans.push_back(std::move(stmt));
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpConvert(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  std::vector<jbc::JBCPrimType> stackInTypes = op.GetInputTypesFromStack();
  ASSERT(stackInTypes.size() == 1, "invalid in type for convert");
  PrimType ptyIn = JBCStack2FEHelper::JBCStackItemTypeToPrimType(stackInTypes[0]);
  jbc::JBCPrimType stackOutType = op.GetOutputTypesToStack();
  ASSERT(stackOutType != jbc::JBCPrimType::kTypeDefault, "invalid out type for convert");
  PrimType ptyOut = JBCStack2FEHelper::JBCStackItemTypeToPrimType(stackOutType);
  UniqueFEIRVar varIn = stack2feHelper.PopItem(ptyIn);
  if (varIn == nullptr) {
    success = false;
    return ans;
  }
  UniqueFEIRVar varOut = stack2feHelper.PushItem(ptyOut);
  if (varOut == nullptr) {
    success = false;
    return ans;
  }
  UniqueFEIRExpr expr(nullptr);
  uint8 bitSize = 0;
  Opcode opExt = OP_sext;
  switch (op.GetOpcode()) {
    case jbc::kOpI2B:
      bitSize = 8;
      opExt = OP_sext;
      break;
    case jbc::kOpI2C:
      bitSize = 16;
      opExt = OP_zext;
      break;
    case jbc::kOpI2S:
      bitSize = 16;
      opExt = OP_sext;
      break;
    default:
      expr = FEIRBuilder::CreateExprCvtPrim(std::move(varIn), ptyOut);
      break;
  }
  if (expr == nullptr) {
    expr = (opExt == OP_sext) ? FEIRBuilder::CreateExprSExt(std::move(varIn)) :
                                FEIRBuilder::CreateExprZExt(std::move(varIn));
    FEIRExprExtractBits *ptrExprExt = static_cast<FEIRExprExtractBits*>(expr.get());
    ptrExprExt->SetBitSize(bitSize);
  }
  UniqueFEIRStmt stmt = FEIRBuilder::CreateStmtDAssign(std::move(varOut), std::move(expr));
  ans.push_back(std::move(stmt));
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpCompare(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpReturn(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  // obtain in/out types
  std::vector<jbc::JBCPrimType> stackInTypes = op.GetInputTypesFromStack();
  if (op.GetOpcode() == jbc::kOpReturn) {
    ASSERT(stackInTypes.empty(), "Not enough input opnds for return op");
    UniqueFEIRStmt stmt = std::make_unique<FEIRStmtReturn>(UniqueFEIRExpr(nullptr));
    ans.push_back(std::move(stmt));
  } else {
    ASSERT(stackInTypes.size() == 1, "Not enough input opnds for return op");
    PrimType pty = JBCStack2FEHelper::JBCStackItemTypeToPrimType(stackInTypes[0]);
    UniqueFEIRVar var = stack2feHelper.PopItem(pty);
    if (var == nullptr) {
      success = false;
      return ans;
    }
    UniqueFEIRExpr expr = FEIRBuilder::CreateExprDRead(std::move(var));
    UniqueFEIRStmt stmt = std::make_unique<FEIRStmtReturn>(std::move(expr));
    ans.push_back(std::move(stmt));
  }
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpStaticFieldOpr(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  const jbc::JBCConstPool &constPool = context.GetConstPool();
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  const jbc::JBCOpFieldOpr &opField = static_cast<const jbc::JBCOpFieldOpr&>(op);
  const jbc::JBCConst *constRaw =
      constPool.GetConstByIdxWithTag(opField.GetFieldIdx(), jbc::JBCConstTag::kConstFieldRef);
  CHECK_NULL_FATAL(constRaw);
  const jbc::JBCConstRef *constRef = static_cast<const jbc::JBCConstRef*>(constRaw);
  FEStructFieldInfo *fieldInfo = static_cast<FEStructFieldInfo*>(constRef->GetFEStructElemInfo());
  CHECK_NULL_FATAL(fieldInfo);
  const UniqueFEIRType &fieldType = fieldInfo->GetType();
  PrimType pty = fieldType->IsScalar() ? fieldType->GetPrimType() : PTY_ref;
  pty = JBCStack2FEHelper::SimplifyPrimType(pty);
  if (op.GetOpcode() == jbc::kOpGetStatic) {
    UniqueFEIRVar var = stack2feHelper.PushItem(pty);
    UniqueFEIRStmt stmt = std::make_unique<FEIRStmtFieldLoad>(nullptr, std::move(var), *fieldInfo, true);
    ans.push_back(std::move(stmt));
  } else {
    UniqueFEIRVar var = stack2feHelper.PopItem(pty);
    UniqueFEIRStmt stmt = std::make_unique<FEIRStmtFieldStore>(nullptr, std::move(var), *fieldInfo, true);
    ans.push_back(std::move(stmt));
  }
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpFieldOpr(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  const jbc::JBCConstPool &constPool = context.GetConstPool();
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  const jbc::JBCOpFieldOpr &opField = static_cast<const jbc::JBCOpFieldOpr&>(op);
  const jbc::JBCConst *constRaw =
      constPool.GetConstByIdxWithTag(opField.GetFieldIdx(), jbc::JBCConstTag::kConstFieldRef);
  CHECK_NULL_FATAL(constRaw);
  const jbc::JBCConstRef *constRef = static_cast<const jbc::JBCConstRef*>(constRaw);
  FEStructFieldInfo *fieldInfo = static_cast<FEStructFieldInfo*>(constRef->GetFEStructElemInfo());
  CHECK_NULL_FATAL(fieldInfo);
  const UniqueFEIRType &fieldType = fieldInfo->GetType();
  PrimType pty = fieldType->IsScalar() ? fieldType->GetPrimType() : PTY_ref;
  pty = JBCStack2FEHelper::SimplifyPrimType(pty);
  if (op.GetOpcode() == jbc::kOpGetField) {
    UniqueFEIRVar varObj = stack2feHelper.PopItem(PTY_ref);
    UniqueFEIRVar var = stack2feHelper.PushItem(pty);
    UniqueFEIRStmt stmt = std::make_unique<FEIRStmtFieldLoad>(std::move(varObj), std::move(var), *fieldInfo, false);
    ans.push_back(std::move(stmt));
  } else {
    UniqueFEIRVar var = stack2feHelper.PopItem(pty);
    UniqueFEIRVar varObj = stack2feHelper.PopItem(PTY_ref);
    UniqueFEIRStmt stmt = std::make_unique<FEIRStmtFieldStore>(std::move(varObj), std::move(var), *fieldInfo, false);
    ans.push_back(std::move(stmt));
  }
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpInvoke(JBCFunctionContext &context, bool &success) const {
  switch (op.GetOpcode()) {
    case jbc::kOpInvokeVirtual:
      return EmitToFEIRForOpInvokeVirtual(context, success);
    case jbc::kOpInvokeSpecial:
      return EmitToFEIRForOpInvokeSpecial(context, success);
    case jbc::kOpInvokeStatic:
      return EmitToFEIRForOpInvokeStatic(context, success);
    case jbc::kOpInvokeInterface:
      return EmitToFEIRForOpInvokeInterface(context, success);
    case jbc::kOpInvokeDynamic:
      return EmitToFEIRForOpInvokeDynamic(context, success);
    default:
      CHECK_FATAL(false, "unsupported op: %s", op.GetOpcodeName().c_str());
      break;
  }
  return EmitToFEIRCommon2(context, success);
}

void JBCStmtInst::PrepareInvokeParametersAndReturn(JBCStack2FEHelper &stack2feHelper,
                                                   const FEStructMethodInfo &info,
                                                   FEIRStmtCallAssign &callStmt,
                                                   bool isStatic) const {
  const std::vector<UniqueFEIRType> &argTypes = info.GetArgTypes();
  for (int i = argTypes.size() - 1; i >= 0; --i) {
    const UniqueFEIRType &argType = argTypes[static_cast<uint32>(i)];
    PrimType pty = argType->GetPrimType();
    UniqueFEIRVar var = stack2feHelper.PopItem(JBCStack2FEHelper::SimplifyPrimType(pty));
    UniqueFEIRExpr expr = FEIRBuilder::CreateExprDRead(std::move(var));
    callStmt.AddExprArgReverse(std::move(expr));
  }
  if (!isStatic) {
    // push this
    const UniqueFEIRType &thisType = info.GetOwnerType();
    PrimType pty = thisType->GetPrimType();
    UniqueFEIRVar var = stack2feHelper.PopItem(JBCStack2FEHelper::SimplifyPrimType(pty));
    UniqueFEIRExpr expr = FEIRBuilder::CreateExprDRead(std::move(var));
    callStmt.AddExprArgReverse(std::move(expr));
  }
  if (!info.IsReturnVoid()) {
    const UniqueFEIRType &retType = info.GetReturnType();
    PrimType pty = retType->GetPrimType();
    UniqueFEIRVar var = stack2feHelper.PushItem(JBCStack2FEHelper::SimplifyPrimType(pty));
    callStmt.SetVar(std::move(var));
  }
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpInvokeVirtual(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  const jbc::JBCConstPool &constPool = context.GetConstPool();
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  const jbc::JBCOpInvoke &opInvoke = static_cast<const jbc::JBCOpInvoke&>(op);
  const jbc::JBCConst *constRaw = constPool.GetConstByIdx(opInvoke.GetMethodIdx());
  if (constRaw == nullptr ||
      (constRaw->GetTag() != jbc::JBCConstTag::kConstMethodRef &&
       constRaw->GetTag() != jbc::JBCConstTag::kConstInterfaceMethodRef)) {
    success = false;
    return ans;
  }
  const jbc::JBCConstRef *constRef = static_cast<const jbc::JBCConstRef*>(constRaw);
  FEStructMethodInfo *methodInfo = static_cast<FEStructMethodInfo*>(constRef->GetFEStructElemInfo());
  UniqueFEIRStmt stmt = std::make_unique<FEIRStmtCallAssign>(*methodInfo, OP_virtualcall, nullptr, false);
  FEIRStmtCallAssign *ptrStmt = static_cast<FEIRStmtCallAssign*>(stmt.get());
  PrepareInvokeParametersAndReturn(stack2feHelper, *methodInfo, *ptrStmt, false);
  ans.push_back(std::move(stmt));
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpInvokeStatic(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  const jbc::JBCConstPool &constPool = context.GetConstPool();
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  const jbc::JBCOpInvoke &opInvoke = static_cast<const jbc::JBCOpInvoke&>(op);
  const jbc::JBCConst *constRaw = constPool.GetConstByIdx(opInvoke.GetMethodIdx());
  if (constRaw == nullptr ||
      (constRaw->GetTag() != jbc::JBCConstTag::kConstMethodRef &&
       constRaw->GetTag() != jbc::JBCConstTag::kConstInterfaceMethodRef)) {
    success = false;
    return ans;
  }
  const jbc::JBCConstRef *constRef = static_cast<const jbc::JBCConstRef*>(constRaw);
  FEStructMethodInfo *methodInfo = static_cast<FEStructMethodInfo*>(constRef->GetFEStructElemInfo());
  UniqueFEIRStmt stmt = std::make_unique<FEIRStmtCallAssign>(*methodInfo, OP_call, nullptr, true);
  FEIRStmtCallAssign *ptrStmt = static_cast<FEIRStmtCallAssign*>(stmt.get());
  PrepareInvokeParametersAndReturn(stack2feHelper, *methodInfo, *ptrStmt, true);
  ans.push_back(std::move(stmt));
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpInvokeInterface(JBCFunctionContext &context,
                                                                      bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  const jbc::JBCConstPool &constPool = context.GetConstPool();
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  const jbc::JBCOpInvoke &opInvoke = static_cast<const jbc::JBCOpInvoke&>(op);
  const jbc::JBCConst *constRaw = constPool.GetConstByIdx(opInvoke.GetMethodIdx());
  if (constRaw == nullptr ||
      (constRaw->GetTag() != jbc::JBCConstTag::kConstMethodRef &&
       constRaw->GetTag() != jbc::JBCConstTag::kConstInterfaceMethodRef)) {
    success = false;
    return ans;
  }
  const jbc::JBCConstRef *constRef = static_cast<const jbc::JBCConstRef*>(constRaw);
  FEStructMethodInfo *methodInfo = static_cast<FEStructMethodInfo*>(constRef->GetFEStructElemInfo());
  UniqueFEIRStmt stmt = std::make_unique<FEIRStmtCallAssign>(*methodInfo, OP_interfacecall, nullptr, false);
  FEIRStmtCallAssign *ptrStmt = static_cast<FEIRStmtCallAssign*>(stmt.get());
  PrepareInvokeParametersAndReturn(stack2feHelper, *methodInfo, *ptrStmt, false);
  ans.push_back(std::move(stmt));
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpInvokeSpecial(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  const jbc::JBCConstPool &constPool = context.GetConstPool();
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  const jbc::JBCOpInvoke &opInvoke = static_cast<const jbc::JBCOpInvoke&>(op);
  const jbc::JBCConst *constRaw = constPool.GetConstByIdx(opInvoke.GetMethodIdx());
  if (constRaw == nullptr ||
      (constRaw->GetTag() != jbc::JBCConstTag::kConstMethodRef &&
       constRaw->GetTag() != jbc::JBCConstTag::kConstInterfaceMethodRef)) {
    success = false;
    return ans;
  }
  const jbc::JBCConstRef *constRef = static_cast<const jbc::JBCConstRef*>(constRaw);
  FEStructMethodInfo *methodInfo = static_cast<FEStructMethodInfo*>(constRef->GetFEStructElemInfo());
  Opcode mirOp = methodInfo->IsConstructor() ? OP_call : OP_superclasscall;
  UniqueFEIRStmt stmt = std::make_unique<FEIRStmtCallAssign>(*methodInfo, mirOp, nullptr, false);
  FEIRStmtCallAssign *ptrStmt = static_cast<FEIRStmtCallAssign*>(stmt.get());
  PrepareInvokeParametersAndReturn(stack2feHelper, *methodInfo, *ptrStmt, false);
  ans.push_back(std::move(stmt));
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpInvokeDynamic(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  const jbc::JBCConstPool &constPool = context.GetConstPool();
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  const jbc::JBCOpInvoke &opInvoke = static_cast<const jbc::JBCOpInvoke&>(op);
  const jbc::JBCConst *constRaw = constPool.GetConstByIdx(opInvoke.GetMethodIdx());
  if (constRaw == nullptr || constRaw->GetTag() != jbc::JBCConstTag::kConstInvokeDynamic) {
    success = false;
    return ans;
  }
  const jbc::JBCConstInvokeDynamic *constDynamic = static_cast<const jbc::JBCConstInvokeDynamic*>(constRaw);
  FEStructMethodInfo *methodInfo = static_cast<FEStructMethodInfo*>(constDynamic->GetFEStructElemInfo());
  UniqueFEIRStmt stmt = std::make_unique<FEIRStmtCallAssign>(*methodInfo, OP_call, nullptr, true);
  FEIRStmtCallAssign *ptrStmt = static_cast<FEIRStmtCallAssign*>(stmt.get());
  PrepareInvokeParametersAndReturn(stack2feHelper, *methodInfo, *ptrStmt, true);
  ans.push_back(std::move(stmt));
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpNew(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  const jbc::JBCConstPool &constPool = context.GetConstPool();
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  const jbc::JBCOpNew &opNew = static_cast<const jbc::JBCOpNew&>(op);
  UniqueFEIRType type = opNew.GetFEIRType(constPool)->Clone();
  UniqueFEIRExpr expr(nullptr);
  switch (op.GetOpcode()) {
    case jbc::kOpNew: {
      expr = FEIRBuilder::CreateExprJavaNewInstance(type->Clone());
      break;
    }
    case jbc::kOpNewArray:
    case jbc::kOpANewArray: {
      UniqueFEIRVar varSize = stack2feHelper.PopItem(PTY_i32);
      if (varSize == nullptr) {
        success = false;
        return ans;
      }
      (void)type->ArrayIncrDim();
      UniqueFEIRExpr exprSize = FEIRBuilder::CreateExprDRead(std::move(varSize));
      expr = FEIRBuilder::CreateExprJavaNewArray(type->Clone(), std::move(exprSize));
      break;
    }
    default:
      FATAL(kLncFatal, "should not run here");
      return ans;
  }
  UniqueFEIRVar varDst = stack2feHelper.PushItem(PTY_ref);
  if (varDst == nullptr || expr == nullptr) {
    success = false;
    return ans;
  }
  varDst->SetType(std::move(type));
  UniqueFEIRStmt stmt = FEIRBuilder::CreateStmtDAssign(std::move(varDst), std::move(expr), true);
  ans.push_back(std::move(stmt));
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpMultiANewArray(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  const jbc::JBCConstPool &constPool = context.GetConstPool();
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  const jbc::JBCOpMultiANewArray &opArray = static_cast<const jbc::JBCOpMultiANewArray&>(op);
  const jbc::JBCConst *constRaw = constPool.GetConstByIdxWithTag(opArray.GetRefTypeIdx(),
                                                                 jbc::JBCConstTag::kConstClass);
  if (constRaw == nullptr) {
    success = false;
    return ans;
  }
  const jbc::JBCConstClass *constClass = static_cast<const jbc::JBCConstClass*>(constRaw);
  UniqueFEIRStmt stmt = std::make_unique<FEIRStmtJavaMultiANewArray>(nullptr, constClass->GetFEIRType()->Clone());
  std::vector<jbc::JBCPrimType> stackInTypes = op.GetInputTypesFromStack(constPool);
  std::reverse(stackInTypes.begin(), stackInTypes.end());
  for (jbc::JBCPrimType popType : stackInTypes) {
    PrimType pty = JBCStack2FEHelper::JBCStackItemTypeToPrimType(popType);
    UniqueFEIRVar var = stack2feHelper.PopItem(pty);
    if (var == nullptr) {
      success = false;
      return ans;
    }
    static_cast<FEIRStmtJavaMultiANewArray*>(stmt.get())->AddVarSizeRev(std::move(var));
  }
  jbc::JBCPrimType stackOutType = op.GetOutputTypesToStack(constPool);
  if (stackOutType == jbc::JBCPrimType::kTypeDefault) {
    success = false;
    return ans;
  }
  PrimType pty = JBCStack2FEHelper::JBCStackItemTypeToPrimType(stackOutType);
  UniqueFEIRVar varRet = stack2feHelper.PushItem(pty);
  static_cast<FEIRStmtJavaMultiANewArray*>(stmt.get())->SetVar(std::move(varRet));
  ans.push_back(std::move(stmt));
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpThrow(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  // obtain in/out types
  std::vector<jbc::JBCPrimType> stackInTypes = op.GetInputTypesFromStack();
  ASSERT(stackInTypes.size() == 1, "Not enough input opnds for return op");
  PrimType pty = JBCStack2FEHelper::JBCStackItemTypeToPrimType(stackInTypes[0]);
  UniqueFEIRVar var = stack2feHelper.PopItem(pty);
  if (var == nullptr) {
    success = false;
    return ans;
  }
  UniqueFEIRExpr expr = FEIRBuilder::CreateExprDRead(std::move(var));
  UniqueFEIRStmt stmt = std::make_unique<FEIRStmtUseOnly>(OP_throw, std::move(expr));
  ans.push_back(std::move(stmt));
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpTypeCheck(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  const jbc::JBCConstPool &constPool = context.GetConstPool();
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  jbc::JBCOpcode opcode = op.GetOpcode();
  const jbc::JBCOpTypeCheck &opTypeCheck = static_cast<const jbc::JBCOpTypeCheck&>(op);
  PrimType ptyIn = PTY_ref;
  PrimType ptyOut = (opcode == jbc::kOpCheckCast) ? PTY_ref : PTY_i32;
  UniqueFEIRVar varIn = stack2feHelper.PopItem(ptyIn);
  UniqueFEIRVar varOut = stack2feHelper.PushItem(ptyOut);
  uint16 constIdx = opTypeCheck.GetTypeIdx();
  const jbc::JBCConst *constRaw = constPool.GetConstByIdxWithTag(constIdx, jbc::JBCConstTag::kConstClass);
  if (constRaw == nullptr) {
    success = false;
    return ans;
  }
  const jbc::JBCConstClass *constClass = static_cast<const jbc::JBCConstClass*>(constRaw);
  UniqueFEIRType type = constClass->GetFEIRType()->Clone();
  if (opcode == jbc::kOpCheckCast) {
    UniqueFEIRStmt stmt = FEIRBuilder::CreateStmtJavaCheckCast(std::move(varOut), std::move(varIn), std::move(type));
    ans.push_back(std::move(stmt));
  } else {
    UniqueFEIRStmt stmt = FEIRBuilder::CreateStmtJavaInstanceOf(std::move(varOut), std::move(varIn), std::move(type));
    ans.push_back(std::move(stmt));
  }
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpMonitor(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  // stack operation
  UniqueFEIRVar var = stack2feHelper.PopItem(PTY_ref);
  if (var == nullptr) {
    success = false;
    return ans;
  }
  auto it = opcodeMapForMonitor.find(op.GetOpcode());
  if (it == opcodeMapForMonitor.end()) {
    success = false;
    ERR(kLncErr, "EmitToFEIRForOpMathUnop: unsupport opcode %s", op.GetOpcodeName().c_str());
    return ans;
  }
  UniqueFEIRExpr expr = FEIRBuilder::CreateExprDRead(std::move(var));
  UniqueFEIRStmt stmt = std::make_unique<FEIRStmtUseOnly>(it->second, std::move(expr));
  ans.push_back(std::move(stmt));
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRForOpArrayLength(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  // stack operation
  // in
  UniqueFEIRVar varArray = stack2feHelper.PopItem(PTY_ref);
  if (varArray == nullptr) {
    success = false;
    return ans;
  }
  // out
  UniqueFEIRVar varDst = stack2feHelper.PushItem(PTY_i32);
  if (varDst == nullptr) {
    success = false;
    return ans;
  }
  UniqueFEIRExpr exprArray = FEIRBuilder::CreateExprDRead(std::move(varArray));
  UniqueFEIRExpr exprArrayLength = FEIRBuilder::CreateExprJavaArrayLength(std::move(exprArray));
  UniqueFEIRStmt stmt = FEIRBuilder::CreateStmtDAssign(std::move(varDst), std::move(exprArrayLength), true);
  ans.push_back(std::move(stmt));
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRCommon(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  std::vector<jbc::JBCPrimType> stackInTypes = op.GetInputTypesFromStack();
  std::reverse(stackInTypes.begin(), stackInTypes.end());
  jbc::JBCPrimType stackOutType = op.GetOutputTypesToStack();
  for (jbc::JBCPrimType popType : stackInTypes) {
    PrimType pty = JBCStack2FEHelper::JBCStackItemTypeToPrimType(popType);
    UniqueFEIRVar var = stack2feHelper.PopItem(pty);
    if (var == nullptr) {
      success = false;
      break;
    }
  }
  if (success && stackOutType != jbc::JBCPrimType::kTypeDefault) {
    PrimType pty = JBCStack2FEHelper::JBCStackItemTypeToPrimType(stackOutType);
    UniqueFEIRVar var = stack2feHelper.PushItem(pty);
    if (var == nullptr) {
      success = false;
    }
  }
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInst::EmitToFEIRCommon2(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  const jbc::JBCConstPool &constPool = context.GetConstPool();
  std::vector<jbc::JBCPrimType> stackInTypes = op.GetInputTypesFromStack(constPool);
  std::reverse(stackInTypes.begin(), stackInTypes.end());
  jbc::JBCPrimType stackOutType = op.GetOutputTypesToStack(constPool);
  for (jbc::JBCPrimType popType : stackInTypes) {
    PrimType pty = JBCStack2FEHelper::JBCStackItemTypeToPrimType(popType);
    UniqueFEIRVar var = stack2feHelper.PopItem(pty);
    if (var == nullptr) {
      success = false;
      break;
    }
  }
  if (success && stackOutType != jbc::JBCPrimType::kTypeDefault) {
    PrimType pty = JBCStack2FEHelper::JBCStackItemTypeToPrimType(stackOutType);
    UniqueFEIRVar var = stack2feHelper.PushItem(pty);
    if (var == nullptr) {
      success = false;
    }
  }
  return ans;
}

UniqueFEIRStmt JBCStmtInst::GenerateStmtForConstI32(JBCStack2FEHelper &stack2feHelper, int32 val, bool &success) const {
  UniqueFEIRExpr exprConst = FEIRBuilder::CreateExprConstI32(val);
  UniqueFEIRVar varDst = stack2feHelper.PushItem(PTY_i32);
  if (varDst == nullptr) {
    success = false;
    return UniqueFEIRStmt(nullptr);
  }
  UniqueFEIRStmt stmtDAssign = FEIRBuilder::CreateStmtDAssign(std::move(varDst), std::move(exprConst));
  return stmtDAssign;
}

UniqueFEIRStmt JBCStmtInst::GenerateStmtForConstI64(JBCStack2FEHelper &stack2feHelper, int64 val, bool &success) const {
  UniqueFEIRExpr exprConst = FEIRBuilder::CreateExprConstI64(val);
  UniqueFEIRVar varDst = stack2feHelper.PushItem(PTY_i64);
  if (varDst == nullptr) {
    success = false;
    return UniqueFEIRStmt(nullptr);
  }
  UniqueFEIRStmt stmtDAssign = FEIRBuilder::CreateStmtDAssign(std::move(varDst), std::move(exprConst));
  return stmtDAssign;
}

UniqueFEIRStmt JBCStmtInst::GenerateStmtForConstF32(JBCStack2FEHelper &stack2feHelper, float val, bool &success) const {
  UniqueFEIRExpr exprConst = FEIRBuilder::CreateExprConstF32(val);
  UniqueFEIRVar varDst = stack2feHelper.PushItem(PTY_f32);
  if (varDst == nullptr) {
    success = false;
    return UniqueFEIRStmt(nullptr);
  }
  UniqueFEIRStmt stmtDAssign = FEIRBuilder::CreateStmtDAssign(std::move(varDst), std::move(exprConst));
  return stmtDAssign;
}

UniqueFEIRStmt JBCStmtInst::GenerateStmtForConstF64(JBCStack2FEHelper &stack2feHelper, double val,
                                                    bool &success) const {
  UniqueFEIRExpr exprConst = FEIRBuilder::CreateExprConstF64(val);
  UniqueFEIRVar varDst = stack2feHelper.PushItem(PTY_f64);
  if (varDst == nullptr) {
    success = false;
    return UniqueFEIRStmt(nullptr);
  }
  UniqueFEIRStmt stmtDAssign = FEIRBuilder::CreateStmtDAssign(std::move(varDst), std::move(exprConst));
  return stmtDAssign;
}

// ---------- JBCStmtInstBranch ----------
std::map<jbc::JBCOpcode, std::tuple<Opcode, Opcode, uint8>> JBCStmtInstBranch::opcodeMapForCondGoto =
    JBCStmtInstBranch::InitOpcodeMapForCondGoto();

std::map<jbc::JBCOpcodeKind, JBCStmtInstBranch::FuncPtrEmitToFEIR> JBCStmtInstBranch::funcPtrMapForEmitToFEIR =
    JBCStmtInstBranch::InitFuncPtrMapForEmitToFEIR();

JBCStmtInstBranch::JBCStmtInstBranch(const jbc::JBCOp &argOp)
    : JBCStmt(GeneralStmtKind::kStmtMultiOut, kJBCStmtInstBranch),
      op(argOp) {
  SetFallThru(op.IsFallThru());
}

bool JBCStmtInstBranch::IsStmtInstImpl() const {
  return true;
}

void JBCStmtInstBranch::DumpImpl(const std::string &prefix) const {
  std::cout << prefix << "JBCStmtInstBranch (id=" << id << ", " <<
               "kind=" << JBCStmtKindHelper::JBCStmtKindName(kind) << ", " <<
               "op=" << op.GetOpcodeName() << ", " <<
               "targets={";
  for (GeneralStmt *stmt : predsOrSuccs) {
    std::cout << stmt->GetID() << " ";
  }
  std::cout << "})" << std::endl;
}

std::string JBCStmtInstBranch::DumpDotStringImpl() const {
  std::stringstream ss;
  ss << "<stmt" << id << "> " << id << ": " << op.GetOpcodeName();
  return ss.str();
}

std::list<UniqueFEIRStmt> JBCStmtInstBranch::EmitToFEIRImpl(JBCFunctionContext &context, bool &success) const {
  auto it = funcPtrMapForEmitToFEIR.find(op.GetOpcodeKind());
  if (it != funcPtrMapForEmitToFEIR.end()) {
    return (this->*(it->second))(context, success);
  } else {
    return EmitToFEIRCommon(context, success);
  }
}

JBCStmtPesudoLabel *JBCStmtInstBranch::GetTarget(const std::map<uint32, JBCStmtPesudoLabel*> &mapPCStmtLabel,
                                                 uint32 pc) const {
  auto itTarget = mapPCStmtLabel.find(pc);
  if (itTarget == mapPCStmtLabel.end()) {
    ERR(kLncErr, "target@pc=%u not found", pc);
    return nullptr;
  }
  return itTarget->second;
}

std::map<jbc::JBCOpcodeKind, JBCStmtInstBranch::FuncPtrEmitToFEIR> JBCStmtInstBranch::InitFuncPtrMapForEmitToFEIR() {
  std::map<jbc::JBCOpcodeKind, FuncPtrEmitToFEIR> ans;
  ans[jbc::JBCOpcodeKind::kOpKindGoto] = &JBCStmtInstBranch::EmitToFEIRForOpGoto;
  ans[jbc::JBCOpcodeKind::kOpKindBranch] = &JBCStmtInstBranch::EmitToFEIRForOpBranch;
  ans[jbc::JBCOpcodeKind::kOpKindSwitch] = &JBCStmtInstBranch::EmitToFEIRForOpSwitch;
  ans[jbc::JBCOpcodeKind::kOpKindJsr] = &JBCStmtInstBranch::EmitToFEIRForOpJsr;
  ans[jbc::JBCOpcodeKind::kOpKindRet] = &JBCStmtInstBranch::EmitToFEIRForOpRet;
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInstBranch::EmitToFEIRForOpGoto(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  const std::map<uint32, JBCStmtPesudoLabel*> &mapPCStmtLabel = context.GetMapPCLabelStmt();
  const jbc::JBCOpGoto &opGoto = static_cast<const jbc::JBCOpGoto&>(op);
  auto it = mapPCStmtLabel.find(opGoto.GetTarget());
  if (it == mapPCStmtLabel.end()) {
    ERR(kLncErr, "target not found for inst branch");
    success = false;
  } else {
    JBCStmtPesudoLabel *stmtLabel = it->second;
    CHECK_NULL_FATAL(stmtLabel);
    UniqueFEIRStmt stmt = FEIRBuilder::CreateStmtGoto(stmtLabel->GetLabelIdx());
    ans.push_back(std::move(stmt));
  }
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInstBranch::EmitToFEIRForOpBranch(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  const std::map<uint32, JBCStmtPesudoLabel*> &mapPCStmtLabel = context.GetMapPCLabelStmt();
  const jbc::JBCOpBranch &opBranch = static_cast<const jbc::JBCOpBranch&>(op);
  auto itTarget = mapPCStmtLabel.find(opBranch.GetTarget());
  if (itTarget == mapPCStmtLabel.end()) {
    ERR(kLncErr, "target not found for inst branch");
    success = false;
    return ans;
  }
  JBCStmtPesudoLabel *stmtLabel = itTarget->second;
  CHECK_NULL_FATAL(stmtLabel);
  auto it = opcodeMapForCondGoto.find(op.GetOpcode());
  if (it == opcodeMapForCondGoto.end()) {
    ERR(kLncErr, "unsupport opcode %s", op.GetOpcodeName().c_str());
    success = false;
    return ans;
  }
  Opcode opStmt = std::get<0>(it->second);
  Opcode opCompExpr = std::get<1>(it->second);
  uint8 mode = std::get<2>(it->second);
  // opnds
  UniqueFEIRExpr expr0;
  UniqueFEIRExpr expr1;
  PrimType pty = ((mode & kModeUseRef) == 0) ? PTY_i32 : PTY_ref;
  if ((mode & kModeUseZeroAsSecondOpnd) == 0) {
    UniqueFEIRVar var1 = stack2feHelper.PopItem(pty);
    if (var1 == nullptr) {
      success = false;
      return ans;
    }
    expr1 = FEIRBuilder::CreateExprDRead(std::move(var1));
  } else {
    expr1 = (pty == PTY_ref) ? FEIRBuilder::CreateExprConstRefNull() : FEIRBuilder::CreateExprConstI32(0);
  }
  UniqueFEIRVar var0 = stack2feHelper.PopItem(pty);
  if (var0 == nullptr) {
    success = false;
    return ans;
  }
  expr0 = FEIRBuilder::CreateExprDRead(std::move(var0));
  UniqueFEIRExpr exprComp = FEIRBuilder::CreateExprMathBinary(opCompExpr, std::move(expr0), std::move(expr1));
  UniqueFEIRStmt stmt = FEIRBuilder::CreateStmtCondGoto(stmtLabel->GetLabelIdx(), opStmt, std::move(exprComp));
  ans.push_back(std::move(stmt));
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInstBranch::EmitToFEIRForOpSwitch(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  const std::map<uint32, JBCStmtPesudoLabel*> &mapPCStmtLabel = context.GetMapPCLabelStmt();
  const jbc::JBCOpSwitch &opSwitch = static_cast<const jbc::JBCOpSwitch&>(op);
  UniqueFEIRVar var = stack2feHelper.PopItem(PTY_i32);
  if (var == nullptr) {
    success = false;
    return ans;
  }
  UniqueFEIRExpr exprValue = FEIRBuilder::CreateExprDRead(std::move(var));
  UniqueFEIRStmt stmt = FEIRBuilder::CreateStmtSwitch(std::move(exprValue));
  FEIRStmtSwitch *ptrStmtSwitch = static_cast<FEIRStmtSwitch*>(stmt.get());
  CHECK_NULL_FATAL(ptrStmtSwitch);
  // default target
  auto itTargetDefault = mapPCStmtLabel.find(opSwitch.GetDefaultTarget());
  if (itTargetDefault == mapPCStmtLabel.end()) {
    ERR(kLncErr, "target not found for inst switch");
    success = false;
    return ans;
  }
  CHECK_NULL_FATAL(itTargetDefault->second);
  ptrStmtSwitch->SetDefaultLabelIdx(itTargetDefault->second->GetLabelIdx());
  // value targets
  for (const auto &itItem : opSwitch.GetTargets()) {
    auto itTarget = mapPCStmtLabel.find(itItem.second);
    if (itTarget == mapPCStmtLabel.end()) {
      ERR(kLncErr, "target not found for inst switch");
      success = false;
      return ans;
    }
    CHECK_NULL_FATAL(itTarget->second);
    ptrStmtSwitch->AddTarget(itItem.first, itTarget->second->GetLabelIdx());
  }
  ans.push_back(std::move(stmt));
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInstBranch::EmitToFEIRForOpJsr(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  const std::map<uint32, JBCStmtPesudoLabel*> &mapPCStmtLabel = context.GetMapPCLabelStmt();
  const jbc::JBCOpJsr &opJsr = static_cast<const jbc::JBCOpJsr&>(op);
  auto itTarget = mapPCStmtLabel.find(opJsr.GetTarget());
  if (itTarget == mapPCStmtLabel.end()) {
    ERR(kLncErr, "target not found for inst jsr");
    success = false;
    return ans;
  }
  JBCStmtPesudoLabel *stmtLabel = itTarget->second;
  CHECK_NULL_FATAL(stmtLabel);
  uint32 slotRegNum = stack2feHelper.GetRegNumForSlot(opJsr.GetSlotIdx());
  UniqueFEIRVar var = FEIRBuilder::CreateVarReg(slotRegNum, PTY_i32, false);
  (void)stack2feHelper.PushItem(var->Clone(), PTY_a32);
  UniqueFEIRExpr exprConst = FEIRBuilder::CreateExprConstI32(opJsr.GetJsrID());
  UniqueFEIRStmt stmtJsr = FEIRBuilder::CreateStmtDAssign(std::move(var), std::move(exprConst));
  UniqueFEIRStmt stmtGoto = FEIRBuilder::CreateStmtGoto(stmtLabel->GetLabelIdx());
  ans.push_back(std::move(stmtJsr));
  ans.push_back(std::move(stmtGoto));
  return ans;
}

std::list<UniqueFEIRStmt> JBCStmtInstBranch::EmitToFEIRForOpRet(JBCFunctionContext &context, bool &success) const {
  return EmitToFEIRForOpRetImpl(context, success);
}

std::list<UniqueFEIRStmt> JBCStmtInstBranch::EmitToFEIRCommon(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  JBCStack2FEHelper &stack2feHelper = context.GetStack2FEHelper();
  std::vector<jbc::JBCPrimType> stackInTypes = op.GetInputTypesFromStack();
  std::reverse(stackInTypes.begin(), stackInTypes.end());
  jbc::JBCPrimType stackOutType = op.GetOutputTypesToStack();
  for (jbc::JBCPrimType popType : stackInTypes) {
    PrimType pty = JBCStack2FEHelper::JBCStackItemTypeToPrimType(popType);
    UniqueFEIRVar var = stack2feHelper.PopItem(pty);
    if (var == nullptr) {
      success = false;
      break;
    }
  }
  if (success && stackOutType != jbc::JBCPrimType::kTypeDefault) {
    PrimType pty = JBCStack2FEHelper::JBCStackItemTypeToPrimType(stackOutType);
    UniqueFEIRVar var = stack2feHelper.PushItem(pty);
    if (var == nullptr) {
      success = false;
    }
  }
  return ans;
}

std::map<jbc::JBCOpcode, std::tuple<Opcode, Opcode, uint8>> JBCStmtInstBranch::InitOpcodeMapForCondGoto() {
  std::map<jbc::JBCOpcode, std::tuple<Opcode, Opcode, uint8>> ans;
  ans[jbc::kOpIfeq] = std::make_tuple(OP_brtrue, OP_eq, kModeUseZeroAsSecondOpnd);
  ans[jbc::kOpIfne] = std::make_tuple(OP_brfalse, OP_eq, kModeUseZeroAsSecondOpnd);
  ans[jbc::kOpIflt] = std::make_tuple(OP_brtrue, OP_lt, kModeUseZeroAsSecondOpnd);
  ans[jbc::kOpIfge] = std::make_tuple(OP_brtrue, OP_ge, kModeUseZeroAsSecondOpnd);
  ans[jbc::kOpIfgt] = std::make_tuple(OP_brtrue, OP_gt, kModeUseZeroAsSecondOpnd);
  ans[jbc::kOpIfle] = std::make_tuple(OP_brtrue, OP_le, kModeUseZeroAsSecondOpnd);
  ans[jbc::kOpIfICmpeq] = std::make_tuple(OP_brtrue, OP_eq, kModeDefault);
  ans[jbc::kOpIfICmpne] = std::make_tuple(OP_brfalse, OP_eq, kModeDefault);
  ans[jbc::kOpIfICmplt] = std::make_tuple(OP_brtrue, OP_lt, kModeDefault);
  ans[jbc::kOpIfICmpge] = std::make_tuple(OP_brtrue, OP_ge, kModeDefault);
  ans[jbc::kOpIfICmpgt] = std::make_tuple(OP_brtrue, OP_gt, kModeDefault);
  ans[jbc::kOpIfICmple] = std::make_tuple(OP_brtrue, OP_le, kModeDefault);
  ans[jbc::kOpIfACmpeq] = std::make_tuple(OP_brtrue, OP_eq, kModeUseRef);
  ans[jbc::kOpIfACmpne] = std::make_tuple(OP_brfalse, OP_eq, kModeUseRef);
  ans[jbc::kOpIfNull] = std::make_tuple(OP_brtrue, OP_eq, kModeUseRef | kModeUseZeroAsSecondOpnd);
  ans[jbc::kOpIfNonNull] = std::make_tuple(OP_brfalse, OP_eq, kModeUseRef | kModeUseZeroAsSecondOpnd);
  return ans;
}

// ---------- JBCStmtInstBranchRet ----------
JBCStmtInstBranchRet::JBCStmtInstBranchRet(const jbc::JBCOp &argOp)
    : JBCStmtInstBranch(argOp) {
  kind = kJBCStmtInstBranchRet;
}

std::list<UniqueFEIRStmt> JBCStmtInstBranchRet::EmitToFEIRForOpRetImpl(JBCFunctionContext &context,
                                                                       bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  const std::map<uint32, JBCStmtPesudoLabel*> &mapPCStmtLabel = context.GetMapPCLabelStmt();
  const std::map<uint16, std::map<int32, uint32>> &mapJsrSlotRetAddr = context.GetMapJsrSlotRetAddr();
  const jbc::JBCOpRet &opRet = static_cast<const jbc::JBCOpRet&>(op);
  uint16 slotIdx = opRet.GetIndex();
  auto itJsrInfo = mapJsrSlotRetAddr.find(slotIdx);
  if (itJsrInfo == mapJsrSlotRetAddr.end()) {
    WARN(kLncWarn, "jsr info not found");
    success = false;
    return ans;
  }
  const std::map<int32, uint32> &jsrInfo = itJsrInfo->second;
  if (jsrInfo.size() == 1) {
    auto itInfo = jsrInfo.begin();
    JBCStmtPesudoLabel *stmtLabel = GetTarget(mapPCStmtLabel, itInfo->second);
    if (stmtLabel == nullptr) {
      success = false;
      return ans;
    }
    UniqueFEIRStmt stmtGoto = FEIRBuilder::CreateStmtGoto(stmtLabel->GetLabelIdx());
    ans.push_back(std::move(stmtGoto));
  } else {
    uint32 idx = 0;
    UniqueFEIRVar var = FEIRBuilder::CreateVarReg(slotIdx, PTY_i32);
    UniqueFEIRExpr exprValue = FEIRBuilder::CreateExprDRead(std::move(var));
    UniqueFEIRStmt stmtSwitch = FEIRBuilder::CreateStmtSwitch(std::move(exprValue));
    FEIRStmtSwitch *ptrStmtSwitch = static_cast<FEIRStmtSwitch*>(stmtSwitch.get());
    for (auto itInfo : jsrInfo) {
      JBCStmtPesudoLabel *stmtLabel = GetTarget(mapPCStmtLabel, itInfo.second);
      if (stmtLabel == nullptr) {
        success = false;
        return ans;
      }
      if (idx == jsrInfo.size() - 1) {
        ptrStmtSwitch->SetDefaultLabelIdx(stmtLabel->GetLabelIdx());
      } else {
        ptrStmtSwitch->AddTarget(itInfo.first, stmtLabel->GetLabelIdx());
      }
      ++idx;
    }
    ans.push_back(std::move(stmtSwitch));
  }
  return ans;
}

// ---------- JBCStmtPesudoLabel ----------
void JBCStmtPesudoLabel::DumpImpl(const std::string &prefix) const {
  std::cout << prefix << "JBCStmtPesudoLabel (id=" << id << "," <<
               "kind=" << JBCStmtKindHelper::JBCStmtKindName(kind) << ", " <<
               "preds={";
  for (GeneralStmt *stmt : predsOrSuccs) {
    std::cout << stmt->GetID() << " ";
  }
  std::cout << "})" << std::endl;
}

std::string JBCStmtPesudoLabel::DumpDotStringImpl() const {
  std::stringstream ss;
  ss << "<stmt" << id << "> " << id << ": label" << labelIdx;
  return ss.str();
}

std::list<UniqueFEIRStmt> JBCStmtPesudoLabel::EmitToFEIRImpl(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  UniqueFEIRStmt stmt = std::make_unique<FEIRStmtPesudoLabel>(labelIdx);
  ans.push_back(std::move(stmt));
  return ans;
}

// ---------- JBCStmtPesudoCatch ----------
void JBCStmtPesudoCatch::DumpImpl(const std::string &prefix) const {
  std::cout << prefix << "JBCStmtPesudoCatch (id=" << id << "," <<
               "kind=" << JBCStmtKindHelper::JBCStmtKindName(kind) << ", " <<
               "preds={";
  for (GeneralStmt *stmt : predsOrSuccs) {
    std::cout << stmt->GetID() << " ";
  }
  std::cout << "})" << std::endl;
}

std::string JBCStmtPesudoCatch::DumpDotStringImpl() const {
  std::stringstream ss;
  ss << "<stmt" << id << "> " << id << ": catch" << labelIdx;
  return ss.str();
}

std::list<UniqueFEIRStmt> JBCStmtPesudoCatch::EmitToFEIRImpl(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  UniqueFEIRStmt stmt = std::make_unique<FEIRStmtPesudoCatch>(labelIdx);
  FEIRStmtPesudoCatch *feirStmt = static_cast<FEIRStmtPesudoCatch*>(stmt.get());
  for (GStrIdx typeNameIdx : catchTypeNames) {
    feirStmt->AddCatchTypeNameIdx(typeNameIdx);
  }
  ans.push_back(std::move(stmt));
  return ans;
}

// ---------- JBCStmtPesudoTry ----------
void JBCStmtPesudoTry::DumpImpl(const std::string &prefix) const {
  std::cout << prefix << "JBCStmtPesudoTry (id=" << id << "," <<
               "kind=" << JBCStmtKindHelper::JBCStmtKindName(kind) << ", " <<
               "succs={";
  for (JBCStmtPesudoCatch *stmt : catchStmts) {
    std::cout << stmt->GetID() << " ";
  }
  std::cout << "})" << std::endl;
}

std::string JBCStmtPesudoTry::DumpDotStringImpl() const {
  std::stringstream ss;
  ss << "<stmt" << id << "> " << id << ": try";
  return ss.str();
}

std::list<UniqueFEIRStmt> JBCStmtPesudoTry::EmitToFEIRImpl(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  UniqueFEIRStmt stmt = std::make_unique<FEIRStmtPesudoJavaTry>();
  FEIRStmtPesudoJavaTry *feirStmt = static_cast<FEIRStmtPesudoJavaTry*>(stmt.get());
  for (JBCStmtPesudoCatch *stmtCatch : catchStmts) {
    feirStmt->AddCatchLabelIdx(stmtCatch->GetLabelIdx());
  }
  ans.push_back(std::move(stmt));
  return ans;
}

// ---------- JBCStmtPesudoEndTry ----------
void JBCStmtPesudoEndTry::DumpImpl(const std::string &prefix) const {
  std::cout << prefix << "JBCStmtPesudoEndTry (id=" << id << ", " <<
               "kind=" << JBCStmtKindHelper::JBCStmtKindName(kind) <<
               ")" << std::endl;
}

std::string JBCStmtPesudoEndTry::DumpDotStringImpl() const {
  std::stringstream ss;
  ss << "<stmt" << id << "> " << id << ": endtry";
  return ss.str();
}

std::list<UniqueFEIRStmt> JBCStmtPesudoEndTry::EmitToFEIRImpl(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  UniqueFEIRStmt stmt = std::make_unique<FEIRStmtPesudoEndTry>();
  ans.push_back(std::move(stmt));
  return ans;
}

// ---------- JBCStmtPesudoComment ----------
void JBCStmtPesudoComment::DumpImpl(const std::string &prefix) const {
}

std::string JBCStmtPesudoComment::DumpDotStringImpl() const {
  return "";
}

std::list<UniqueFEIRStmt> JBCStmtPesudoComment::EmitToFEIRImpl(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  UniqueFEIRStmt stmt = std::make_unique<FEIRStmtPesudoComment>(content);
  ans.push_back(std::move(stmt));
  return ans;
}

// ---------- JBCStmtPesudoLOC ----------
void JBCStmtPesudoLOC::DumpImpl(const std::string &prefix) const {
  std::cout << prefix << "LOC " << srcFileIdx << " " << lineNumber << std::endl;
}

std::string JBCStmtPesudoLOC::DumpDotStringImpl() const {
  std::stringstream ss;
  ss << "<stmt" << id << "> " << id << ": LOC " << srcFileIdx << " " << lineNumber;
  return ss.str();
}

std::list<UniqueFEIRStmt> JBCStmtPesudoLOC::EmitToFEIRImpl(JBCFunctionContext &context, bool &success) const {
  std::list<UniqueFEIRStmt> ans;
  return ans;
}
}  // namespace maple
