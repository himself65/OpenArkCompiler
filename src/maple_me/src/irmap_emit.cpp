/*
 * Copyright (c) [2019-2020] Huawei Technologies Co.,Ltd.All rights reserved.
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
// This file contains methods to emit Maple IR nodes from MeExpr/MeStmt
#include "me_ir.h"
#include "irmap.h"
#include "mir_builder.h"
#include "orig_symbol.h"

namespace maple {
bool VarMeExpr::IsValidVerIdx(const SSATab &ssaTab) const {
  const OriginalSt *ost = ssaTab.GetOriginalStFromID(ostIdx);
  if (ost == nullptr || !ost->IsSymbolOst()) {
    return false;
  }
  StIdx stIdx = ost->GetMIRSymbol()->GetStIdx();
  return stIdx.Islocal() ? ssaTab.GetModule().CurFunction()->GetSymTab()->IsValidIdx(stIdx.Idx())
                         : GlobalTables::GetGsymTable().IsValidIdx(stIdx.Idx());
}

BaseNode &VarMeExpr::EmitExpr(SSATab &ssaTab) {
  MIRSymbol *symbol = ssaTab.GetMIRSymbolFromID(GetOStIdx());
  if (symbol->IsLocal()) {
    symbol->ResetIsDeleted();
  }
  auto *addrofNode = ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<AddrofNode>(
      OP_dread, PrimType(GetPrimType()), symbol->GetStIdx(), GetFieldID());
  ASSERT(addrofNode->GetPrimType() != kPtyInvalid, "runtime check error");
  ASSERT(IsValidVerIdx(ssaTab), "runtime check error");
  return *addrofNode;
}

BaseNode &RegMeExpr::EmitExpr(SSATab &ssaTab) {
  auto *regRead = ssaTab.GetModule().CurFunction()->GetCodeMemPool()->New<RegreadNode>();
  regRead->SetPrimType(GetPrimType());
  regRead->SetRegIdx(regIdx);
  ASSERT(regIdx < 0 ||
         static_cast<uint32>(static_cast<int32>(regIdx)) < ssaTab.GetModule().CurFunction()->GetPregTab()->Size(),
         "RegMeExpr::EmitExpr: pregIdx exceeds preg table size");
  return *regRead;
}

BaseNode &ConstMeExpr::EmitExpr(SSATab &ssaTab) {
  auto *exprConst =
      ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<ConstvalNode>(PrimType(GetPrimType()), constVal);
  // if int const has been promoted from dyn int const, remove the type tag
  if (IsPrimitiveInteger(exprConst->GetPrimType())) {
    auto *intConst = safe_cast<MIRIntConst>(exprConst->GetConstVal());
    MIRIntConst *newIntConst = GlobalTables::GetIntConstTable().GetOrCreateIntConst(intConst->GetValueUnderType(),
        intConst->GetType(), intConst->GetFieldId());
    exprConst->SetConstVal(newIntConst);
  }
  return *exprConst;
}

BaseNode &ConststrMeExpr::EmitExpr(SSATab &ssaTab) {
  auto *exprConst =
      ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<ConststrNode>(PrimType(GetPrimType()), strIdx);
  return *exprConst;
}

BaseNode &Conststr16MeExpr::EmitExpr(SSATab &ssaTab) {
  auto *exprConst =
      ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<Conststr16Node>(PrimType(GetPrimType()), strIdx);
  return *exprConst;
}

BaseNode &SizeoftypeMeExpr::EmitExpr(SSATab &ssaTab) {
  auto *exprSizeoftype =
      ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<SizeoftypeNode>(PrimType(GetPrimType()), tyIdx);
  return *exprSizeoftype;
}

BaseNode &FieldsDistMeExpr::EmitExpr(SSATab &ssaTab) {
  auto *exprSizeofType = ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<FieldsDistNode>(
      PrimType(GetPrimType()), tyIdx, fieldID1, fieldID2);
  return *exprSizeofType;
}

BaseNode &AddrofMeExpr::EmitExpr(SSATab &ssaTab) {
  MIRSymbol *symbol = ssaTab.GetMIRSymbolFromID(ostIdx);
  if (symbol->IsLocal()) {
    symbol->ResetIsDeleted();
  }
  auto *addrofNode = ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<AddrofNode>(
      OP_addrof, PrimType(GetPrimType()), symbol->GetStIdx(), fieldID);
  return *addrofNode;
}

BaseNode &AddroffuncMeExpr::EmitExpr(SSATab &ssaTab) {
  auto *addroffuncNode =
      ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<AddroffuncNode>(PrimType(GetPrimType()), puIdx);
  return *addroffuncNode;
}

BaseNode &GcmallocMeExpr::EmitExpr(SSATab &ssaTab) {
  auto *gcMallocNode = ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<GCMallocNode>(
      Opcode(GetOp()), PrimType(GetPrimType()), tyIdx);
  return *gcMallocNode;
}

BaseNode &OpMeExpr::EmitExpr(SSATab &ssaTab) {
  switch (GetOp()) {
    case OP_add:
    case OP_ashr:
    case OP_band:
    case OP_bior:
    case OP_bxor:
    case OP_div:
    case OP_land:
    case OP_lior:
    case OP_lshr:
    case OP_max:
    case OP_min:
    case OP_mul:
    case OP_rem:
    case OP_shl:
    case OP_sub: {
      auto *binaryNode =
          ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<BinaryNode>(Opcode(GetOp()),
                                                                              PrimType(GetPrimType()));
      binaryNode->SetBOpnd(&opnds[0]->EmitExpr(ssaTab), 0);
      binaryNode->SetBOpnd(&opnds[1]->EmitExpr(ssaTab), 1);
      return *binaryNode;
    }
    case OP_eq:
    case OP_ne:
    case OP_lt:
    case OP_gt:
    case OP_le:
    case OP_ge:
    case OP_cmpl:
    case OP_cmpg:
    case OP_cmp: {
      auto *cmpNode = ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<CompareNode>(
          Opcode(GetOp()), PrimType(GetPrimType()));
      cmpNode->SetBOpnd(&opnds[0]->EmitExpr(ssaTab), 0);
      cmpNode->SetBOpnd(&opnds[1]->EmitExpr(ssaTab), 1);
      cmpNode->SetOpndType(opndType);
      return *cmpNode;
    }
    case OP_abs:
    case OP_bnot:
    case OP_lnot:
    case OP_neg:
    case OP_recip:
    case OP_sqrt:
    case OP_alloca:
    case OP_malloc: {
      auto *unaryNode =
          ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<UnaryNode>(Opcode(GetOp()), PrimType(GetPrimType()));
      unaryNode->SetOpnd(&opnds[0]->EmitExpr(ssaTab), 0);
      return *unaryNode;
    }
    case OP_sext:
    case OP_zext:
    case OP_extractbits: {
      auto *unode = ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<ExtractbitsNode>(
          Opcode(GetOp()), PrimType(GetPrimType()));
      unode->SetOpnd(&opnds[0]->EmitExpr(ssaTab), 0);
      unode->SetBitsOffset(bitsOffset);
      unode->SetBitsSize(bitsSize);
      return *unode;
    }
    case OP_select: {
      auto *ternaryNode = ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<TernaryNode>(
          Opcode(GetOp()), PrimType(GetPrimType()));
      const size_t opndNumOfTernary = 3;
      for (size_t i = 0; i < opndNumOfTernary; ++i) {
        ternaryNode->SetOpnd(&opnds[i]->EmitExpr(ssaTab), i);
      }
      return *ternaryNode;
    }
    case OP_ceil:
    case OP_cvt:
    case OP_floor:
    case OP_trunc: {
      auto *cvtNode = ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<TypeCvtNode>(
          Opcode(GetOp()), PrimType(GetPrimType()));
      cvtNode->SetOpnd(&opnds[0]->EmitExpr(ssaTab), 0);
      cvtNode->SetFromType(opndType);
      return *cvtNode;
    }
    case OP_retype: {
      auto *cvtNode =
          ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<RetypeNode>(PrimType(GetPrimType()));
      cvtNode->SetOpnd(&opnds[0]->EmitExpr(ssaTab), 0);
      cvtNode->SetFromType(opndType);
      cvtNode->SetTyIdx(tyIdx);
      return *cvtNode;
    }
    case OP_gcmallocjarray:
    case OP_gcpermallocjarray: {
      auto *arrayMalloc = ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<JarrayMallocNode>(
          Opcode(GetOp()), PrimType(GetPrimType()));
      arrayMalloc->SetOpnd(&opnds[0]->EmitExpr(ssaTab), 0);
      arrayMalloc->SetTyIdx(tyIdx);
      return *arrayMalloc;
    }
    case OP_resolveinterfacefunc:
    case OP_resolvevirtualfunc: {
      auto *resolveNode = ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<ResolveFuncNode>(
          Opcode(GetOp()), PrimType(GetPrimType()));
      resolveNode->SetBOpnd(&opnds[0]->EmitExpr(ssaTab), 0);
      resolveNode->SetBOpnd(&opnds[1]->EmitExpr(ssaTab), 1);
      resolveNode->SetPUIdx(fieldID);
      return *resolveNode;
    }
    case OP_iaddrof: {
      auto *iaddrof =
          ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<IaddrofNode>(OP_iaddrof, PrimType(GetPrimType()));
      iaddrof->SetOpnd(&opnds[0]->EmitExpr(ssaTab), 0);
      iaddrof->SetTyIdx(tyIdx);
      iaddrof->SetFieldID(fieldID);
      return *iaddrof;
    }
    default:
      CHECK_FATAL(false, "unexpected op");
  }
}

BaseNode &NaryMeExpr::EmitExpr(SSATab &ssaTab) {
  BaseNode *nodeToReturn = nullptr;
  NaryOpnds *nopndPart = nullptr;
  if (GetOp() == OP_array) {
    auto *arrayNode = ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<ArrayNode>(
        ssaTab.GetModule(), PrimType(GetPrimType()), tyIdx);
    arrayNode->SetNumOpnds(GetNumOpnds());
    arrayNode->SetBoundsCheck(GetBoundCheck());
    nopndPart = arrayNode;
    nodeToReturn = arrayNode;
  } else {
    auto *intrinNode = ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<IntrinsicopNode>(
        ssaTab.GetModule(), GetOp(), PrimType(GetPrimType()), tyIdx);
    intrinNode->SetNumOpnds(GetNumOpnds());
    intrinNode->SetIntrinsic(intrinsic);
    nopndPart = intrinNode;
    nodeToReturn = intrinNode;
  }
  for (auto it = GetOpnds().begin(); it != GetOpnds().end(); ++it) {
    nopndPart->GetNopnd().push_back(&(*it)->EmitExpr(ssaTab));
  }
  return *nodeToReturn;
}

BaseNode &IvarMeExpr::EmitExpr(SSATab &ssaTab) {
  CHECK_NULL_FATAL(base);
  auto *ireadNode =
      ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<IreadNode>(OP_iread, PrimType(GetPrimType()));
  ireadNode->SetOpnd(&base->EmitExpr(ssaTab), 0);
  ireadNode->SetFieldID(fieldID);
  ireadNode->SetTyIdx(tyIdx);
  ASSERT(ireadNode->GetPrimType() != kPtyInvalid, "");
  ASSERT(tyIdx != 0, "wrong tyIdx for iread node in me emit");
  ireadNode->SetTyIdx(tyIdx);
  return *ireadNode;
}

StmtNode &MeStmt::EmitStmt(SSATab &ssaTab) {
  auto *stmt = ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<StmtNode>(Opcode(GetOp()));
  stmt->SetSrcPos(GetSrcPosition());
  return *stmt;
}

StmtNode &DassignMeStmt::EmitStmt(SSATab &ssaTab) {
  auto *dassignStmt = ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<DassignNode>();
  MIRSymbol *symbol = ssaTab.GetMIRSymbolFromID(GetVarLHS()->GetOStIdx());
  if (symbol->IsLocal()) {
    symbol->ResetIsDeleted();
  }
  dassignStmt->SetStIdx(symbol->GetStIdx());
  dassignStmt->SetFieldID(GetVarLHS()->GetFieldID());
  dassignStmt->SetRHS(&GetRHS()->EmitExpr(ssaTab));
  dassignStmt->SetSrcPos(GetSrcPosition());
  return *dassignStmt;
}

StmtNode &RegassignMeStmt::EmitStmt(SSATab &ssaTab) {
  CHECK_NULL_FATAL(lhs);
  CHECK_NULL_FATAL(rhs);
  RegassignNode *regassignStmt = ssaTab.GetModule().GetMIRBuilder()->CreateStmtRegassign(
      lhs->GetPrimType(), lhs->GetRegIdx(), &rhs->EmitExpr(ssaTab));
  regassignStmt->SetSrcPos(GetSrcPosition());
  return *regassignStmt;
}

StmtNode &MaydassignMeStmt::EmitStmt(SSATab &ssaTab) {
  CHECK_NULL_FATAL(rhs);
  auto *dassignStmt = ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<DassignNode>();
  MIRSymbol *symbol = mayDSSym->GetMIRSymbol();
  if (symbol->IsLocal()) {
    symbol->ResetIsDeleted();
  }
  dassignStmt->SetStIdx(symbol->GetStIdx());
  dassignStmt->SetFieldID(fieldID);
  dassignStmt->SetRHS(&rhs->EmitExpr(ssaTab));
  dassignStmt->SetSrcPos(GetSrcPosition());
  return *dassignStmt;
}

void MeStmt::EmitCallReturnVector(SSATab &ssaTab, CallReturnVector &nRets) {
  MapleVector<MustDefMeNode> *mustDefs = GetMustDefList();
  if (mustDefs == nullptr || mustDefs->empty()) {
    return;
  }
  MeExpr *meExpr = mustDefs->front().GetLHS();
  if (meExpr->GetMeOp() == kMeOpVar) {
    OriginalSt *ost = ssaTab.GetOriginalStFromID(static_cast<VarMeExpr*>(meExpr)->GetOStIdx());
    MIRSymbol *symbol = ost->GetMIRSymbol();
    nRets.push_back(CallReturnPair(symbol->GetStIdx(), RegFieldPair(0, 0)));
  } else if (meExpr->GetMeOp() == kMeOpReg) {
    nRets.push_back(CallReturnPair(StIdx(), RegFieldPair(0, static_cast<RegMeExpr*>(meExpr)->GetRegIdx())));
  }
}

StmtNode &IassignMeStmt::EmitStmt(SSATab &ssaTab) {
  CHECK_NULL_FATAL(lhsVar);
  CHECK_NULL_FATAL(rhs);
  auto *iassignNode = ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<IassignNode>();
  iassignNode->SetTyIdx(tyIdx);
  iassignNode->SetFieldID(lhsVar->GetFieldID());
  iassignNode->SetAddrExpr(&lhsVar->GetBase()->EmitExpr(ssaTab));
  iassignNode->SetRHS(&rhs->EmitExpr(ssaTab));
  iassignNode->SetSrcPos(GetSrcPosition());
  return *iassignNode;
}

const MIRFunction &CallMeStmt::GetTargetFunction() const {
  return *GlobalTables::GetFunctionTable().GetFunctionFromPuidx(puIdx);
}

MIRFunction &CallMeStmt::GetTargetFunction() {
  return *GlobalTables::GetFunctionTable().GetFunctionFromPuidx(puIdx);
}

StmtNode &CallMeStmt::EmitStmt(SSATab &ssaTab) {
  if (GetOp() != OP_icall && GetOp() != OP_icallassigned) {
    auto *callNode =
        ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<CallNode>(ssaTab.GetModule(), Opcode(GetOp()));
    callNode->SetPUIdx(puIdx);
    callNode->SetTyIdx(tyIdx);
    callNode->GetNopnd().resize(NumMeStmtOpnds());
    for (size_t i = 0; i < NumMeStmtOpnds(); ++i) {
      callNode->SetOpnd(&GetOpnd(i)->EmitExpr(ssaTab), i);
    }
    callNode->SetNumOpnds(callNode->GetNopndSize());
    callNode->SetSrcPos(GetSrcPosition());
    if (kOpcodeInfo.IsCallAssigned(GetOp())) {
      EmitCallReturnVector(ssaTab, callNode->GetReturnVec());
      for (size_t j = 0; j < callNode->GetReturnVec().size(); ++j) {
        CallReturnPair retPair = callNode->GetReturnVec()[j];
        if (!retPair.second.IsReg()) {
          StIdx stIdx = retPair.first;
          if (stIdx.Islocal()) {
            MIRSymbolTable *symbolTab = ssaTab.GetModule().CurFunction()->GetSymTab();
            MIRSymbol *symbol = symbolTab->GetSymbolFromStIdx(stIdx.Idx());
            symbol->ResetIsDeleted();
          }
        }
      }
    }
    return *callNode;
  }
  auto *icallNode =
      ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<IcallNode>(ssaTab.GetModule(), Opcode(GetOp()));
  icallNode->GetNopnd().resize(NumMeStmtOpnds());
  for (size_t i = 0; i < NumMeStmtOpnds(); ++i) {
    icallNode->SetOpnd(&GetOpnd(i)->EmitExpr(ssaTab), i);
  }
  icallNode->SetNumOpnds(icallNode->GetNopndSize());
  icallNode->SetSrcPos(GetSrcPosition());
  if (kOpcodeInfo.IsCallAssigned(GetOp())) {
    EmitCallReturnVector(ssaTab, icallNode->GetReturnVec());
    icallNode->SetRetTyIdx(TyIdx(PTY_void));
    for (size_t j = 0; j < icallNode->GetReturnVec().size(); ++j) {
      CallReturnPair retPair = icallNode->GetReturnVec()[j];
      if (!retPair.second.IsReg()) {
        StIdx stIdx = retPair.first;
        MIRSymbolTable *symbolTab = ssaTab.GetModule().CurFunction()->GetSymTab();
        MIRSymbol *symbol = symbolTab->GetSymbolFromStIdx(stIdx.Idx());
        icallNode->SetRetTyIdx(symbol->GetType()->GetTypeIndex());
        if (stIdx.Islocal()) {
          symbol->ResetIsDeleted();
        }
      } else {
        PregIdx pregIdx = (PregIdx)retPair.second.GetPregIdx();
        MIRPreg *preg = ssaTab.GetModule().CurFunction()->GetPregTab()->PregFromPregIdx(pregIdx);
        icallNode->SetRetTyIdx(TyIdx(preg->GetPrimType()));
      }
    }
  }
  return *icallNode;
}

StmtNode &IcallMeStmt::EmitStmt(SSATab &ssaTab) {
  auto *icallNode =
      ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<IcallNode>(ssaTab.GetModule(), Opcode(GetOp()));
  icallNode->GetNopnd().resize(NumMeStmtOpnds());
  for (size_t i = 0; i < NumMeStmtOpnds(); ++i) {
    icallNode->SetOpnd(&GetOpnd(i)->EmitExpr(ssaTab), i);
  }
  icallNode->SetNumOpnds(icallNode->GetNopndSize());
  icallNode->SetSrcPos(GetSrcPosition());
  if (kOpcodeInfo.IsCallAssigned(GetOp())) {
    EmitCallReturnVector(ssaTab, icallNode->GetReturnVec());
    icallNode->SetRetTyIdx(TyIdx(PTY_void));
    for (size_t j = 0; j < icallNode->GetReturnVec().size(); ++j) {
      CallReturnPair retPair = icallNode->GetReturnVec()[j];
      if (!retPair.second.IsReg()) {
        StIdx stIdx = retPair.first;
        MIRSymbolTable *symbolTab = ssaTab.GetModule().CurFunction()->GetSymTab();
        MIRSymbol *symbol = symbolTab->GetSymbolFromStIdx(stIdx.Idx());
        icallNode->SetRetTyIdx(symbol->GetType()->GetTypeIndex());
        if (stIdx.Islocal()) {
          symbol->ResetIsDeleted();
        }
      } else {
        PregIdx pregIdx = (PregIdx)retPair.second.GetPregIdx();
        MIRPreg *preg = ssaTab.GetModule().CurFunction()->GetPregTab()->PregFromPregIdx(pregIdx);
        icallNode->SetRetTyIdx(TyIdx(preg->GetPrimType()));
      }
    }
  }
  return *icallNode;
}

StmtNode &IntrinsiccallMeStmt::EmitStmt(SSATab &ssaTab) {
  auto *callNode =
      ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<IntrinsiccallNode>(ssaTab.GetModule(),
                                                                                 Opcode(GetOp()));
  callNode->SetIntrinsic(intrinsic);
  callNode->SetTyIdx(tyIdx);
  callNode->GetNopnd().resize(NumMeStmtOpnds());
  for (size_t i = 0; i < NumMeStmtOpnds(); ++i) {
    callNode->SetOpnd(&GetOpnd(i)->EmitExpr(ssaTab), i);
  }
  callNode->SetNumOpnds(callNode->GetNopndSize());
  callNode->SetSrcPos(GetSrcPosition());
  if (kOpcodeInfo.IsCallAssigned(GetOp())) {
    EmitCallReturnVector(ssaTab, callNode->GetReturnVec());
    for (size_t j = 0; j < callNode->GetReturnVec().size(); ++j) {
      CallReturnPair retPair = callNode->GetReturnVec()[j];
      if (!retPair.second.IsReg()) {
        StIdx stIdx = retPair.first;
        if (stIdx.Islocal()) {
          MIRSymbolTable *symbolTab = ssaTab.GetModule().CurFunction()->GetSymTab();
          MIRSymbol *symbol = symbolTab->GetSymbolFromStIdx(stIdx.Idx());
          symbol->ResetIsDeleted();
        }
      }
    }
  }
  return *callNode;
}

StmtNode &NaryMeStmt::EmitStmt(SSATab &ssaTab) {
  auto *naryStmt =
      ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<NaryStmtNode>(ssaTab.GetModule(), Opcode(GetOp()));
  naryStmt->GetNopnd().resize(NumMeStmtOpnds());
  for (size_t i = 0; i < NumMeStmtOpnds(); ++i) {
    naryStmt->SetOpnd(&opnds[i]->EmitExpr(ssaTab), i);
  }
  naryStmt->SetNumOpnds(naryStmt->GetNopndSize());
  naryStmt->SetSrcPos(GetSrcPosition());
  return *naryStmt;
}

StmtNode &UnaryMeStmt::EmitStmt(SSATab &ssaTab) {
  auto *unaryStmt = ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<UnaryStmtNode>(Opcode(GetOp()));
  CHECK_NULL_FATAL(opnd);
  unaryStmt->SetOpnd(&opnd->EmitExpr(ssaTab), 0);
  unaryStmt->SetSrcPos(GetSrcPosition());
  return *unaryStmt;
}

StmtNode &GotoMeStmt::EmitStmt(SSATab &ssaTab) {
  auto *gotoNode = ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<GotoNode>(OP_goto);
  gotoNode->SetOffset(offset);
  gotoNode->SetSrcPos(GetSrcPosition());
  return *gotoNode;
}

StmtNode &CondGotoMeStmt::EmitStmt(SSATab &ssaTab) {
  auto *cgNode = ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<CondGotoNode>(Opcode(GetOp()));
  cgNode->SetOffset(offset);
  cgNode->SetOpnd(&GetOpnd()->EmitExpr(ssaTab), 0);
  cgNode->SetSrcPos(GetSrcPosition());
  return *cgNode;
}

StmtNode &JsTryMeStmt::EmitStmt(SSATab &ssaTab) {
  auto *jtNode = ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<JsTryNode>();
  jtNode->SetCatchOffset(catchOffset);
  jtNode->SetFinallyOffset(finallyOffset);
  jtNode->SetSrcPos(GetSrcPosition());
  return *jtNode;
}

StmtNode &TryMeStmt::EmitStmt(SSATab &ssaTab) {
  auto *tryNode = ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<TryNode>(ssaTab.GetModule());
  tryNode->ResizeOffsets(offsets.size());
  for (size_t i = 0; i < offsets.size(); ++i) {
    tryNode->SetOffset(offsets[i], i);
  }
  tryNode->SetSrcPos(GetSrcPosition());
  return *tryNode;
}

StmtNode &CatchMeStmt::EmitStmt(SSATab &ssaTab) {
  auto *catchNode = ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<CatchNode>(ssaTab.GetModule());
  catchNode->SetExceptionTyIdxVec(exceptionTyIdxVec);
  catchNode->SetSrcPos(GetSrcPosition());
  return *catchNode;
}

StmtNode &SwitchMeStmt::EmitStmt(SSATab &ssaTab) {
  auto *switchNode = ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<SwitchNode>(ssaTab.GetModule());
  switchNode->SetDefaultLabel(defaultLabel);
  switchNode->SetSwitchTable(switchTable);
  switchNode->SetSwitchOpnd(&GetOpnd()->EmitExpr(ssaTab));
  switchNode->SetSrcPos(GetSrcPosition());
  return *switchNode;
}

StmtNode &CommentMeStmt::EmitStmt(SSATab &ssaTab) {
  auto *commentNode =
      ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<CommentNode>(ssaTab.GetModule());
  commentNode->SetComment(comment);
  commentNode->SetSrcPos(GetSrcPosition());
  return *commentNode;
}

StmtNode &ThrowMeStmt::EmitStmt(SSATab &ssaTab) {
  auto *unaryStmt = ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<UnaryStmtNode>(OP_throw);
  CHECK_NULL_FATAL(opnd);
  unaryStmt->SetOpnd(&opnd->EmitExpr(ssaTab), 0);
  unaryStmt->SetSrcPos(GetSrcPosition());
  return *unaryStmt;
}

StmtNode &GosubMeStmt::EmitStmt(SSATab &ssaTab) {
  auto *gosubNode = ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<GotoNode>(OP_gosub);
  gosubNode->SetOffset(offset);
  gosubNode->SetSrcPos(GetSrcPosition());
  return *gosubNode;
}

StmtNode &AssertMeStmt::EmitStmt(SSATab &ssaTab) {
  auto *assertStmt = ssaTab.GetModule().CurFunction()->GetCodeMempool()->New<AssertStmtNode>(Opcode(GetOp()));
  assertStmt->SetBOpnd(&opnds[0]->EmitExpr(ssaTab), 0);
  assertStmt->SetBOpnd(&opnds[1]->EmitExpr(ssaTab), 1);
  assertStmt->SetSrcPos(GetSrcPosition());
  return *assertStmt;
}
}  // namespace maple
