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
#include "cgfunc.h"
#if DEBUG
#include <iomanip>
#endif
#include "cg.h"
#include "insn.h"
#include "loop.h"
#include "mir_builder.h"
#include "factory.h"

namespace maplebe {
using namespace maple;

#define JAVALANG (GetMirModule().IsJavaModule())

Operand *HandleDread(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  auto &dreadNode = static_cast<AddrofNode&>(expr);
  return cgFunc.SelectDread(dreadNode);
}

Operand *HandleRegread(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  auto &regReadNode = static_cast<RegreadNode&>(expr);
  if (regReadNode.GetRegIdx() == -kSregRetval0) {
    return &cgFunc.ProcessReturnReg(regReadNode.GetPrimType());
  }
  return cgFunc.SelectRegread(regReadNode);
}

Operand *HandleConstVal(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  auto &constValNode = static_cast<ConstvalNode&>(expr);
  MIRConst *mirConst = constValNode.GetConstVal();
  ASSERT(mirConst != nullptr, "get constval of constvalnode failed");
  if (mirConst->GetKind() == kConstInt) {
    auto *mirIntConst = safe_cast<MIRIntConst>(mirConst);
    return cgFunc.SelectIntConst(*mirIntConst);
  } else if (mirConst->GetKind() == kConstFloatConst) {
    auto *mirFloatConst = safe_cast<MIRFloatConst>(mirConst);
    return cgFunc.SelectFloatConst(*mirFloatConst);
  } else if (mirConst->GetKind() == kConstDoubleConst) {
    auto *mirDoubleConst = safe_cast<MIRDoubleConst>(mirConst);
    return cgFunc.SelectDoubleConst(*mirDoubleConst);
  } else {
    CHECK_FATAL(false, "NYI");
  }
  return nullptr;
}

Operand *HandleConstStr(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  auto &constStrNode = static_cast<ConststrNode&>(expr);
#if TARGAARCH64
  return cgFunc.SelectStrConst(*cgFunc.GetMemoryPool()->New<MIRStrConst>(
      constStrNode.GetStrIdx(), *GlobalTables::GetTypeTable().GetTypeFromTyIdx((TyIdx)PTY_a64)));
#else
  return cgFunc.SelectStrConst(*cgFunc.GetMemoryPool()->New<MIRStrConst>(
      constStrNode.GetStrIdx(), *GlobalTables::GetTypeTable().GetTypeFromTyIdx((TyIdx)PTY_a32)));
#endif
}

Operand *HandleConstStr16(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  auto &constStr16Node = static_cast<Conststr16Node&>(expr);
#if TARGAARCH64
  return cgFunc.SelectStr16Const(*cgFunc.GetMemoryPool()->New<MIRStr16Const>(
      constStr16Node.GetStrIdx(), *GlobalTables::GetTypeTable().GetTypeFromTyIdx((TyIdx)PTY_a64)));
#else
  return cgFunc.SelectStr16Const(*cgFunc.GetMemoryPool()->New<MIRStr16Const>(
      constStr16Node.GetStrIdx(), *GlobalTables::GetTypeTable().GetTypeFromTyIdx((TyIdx)PTY_a32)));
#endif
}

Operand *HandleAdd(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  return cgFunc.SelectAdd(static_cast<BinaryNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)),
                          *cgFunc.HandleExpr(expr, *expr.Opnd(1)));
}

Operand *HandleCGArrayElemAdd(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  return &cgFunc.SelectCGArrayElemAdd(static_cast<BinaryNode&>(expr));
}

Operand *HandleShift(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  return cgFunc.SelectShift(static_cast<BinaryNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)),
                            *cgFunc.HandleExpr(expr, *expr.Opnd(1)));
}

Operand *HandleMpy(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  return cgFunc.SelectMpy(static_cast<BinaryNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)),
                          *cgFunc.HandleExpr(expr, *expr.Opnd(1)));
}

Operand *HandleDiv(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  return cgFunc.SelectDiv(static_cast<BinaryNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)),
                          *cgFunc.HandleExpr(expr, *expr.Opnd(1)));
}

Operand *HandleRem(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  return cgFunc.SelectRem(static_cast<BinaryNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)),
                          *cgFunc.HandleExpr(expr, *expr.Opnd(1)));
}

Operand *HandleAddrof(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  auto &addrofNode = static_cast<AddrofNode&>(expr);
  return cgFunc.SelectAddrof(addrofNode);
}

Operand *HandleAddroffunc(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  auto &addroffuncNode = static_cast<AddroffuncNode&>(expr);
  return &cgFunc.SelectAddrofFunc(addroffuncNode);
}

Operand *HandleIread(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  auto &ireadNode = static_cast<IreadNode&>(expr);
  return cgFunc.SelectIread(parent, ireadNode);
}

Operand *HandleSub(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  return cgFunc.SelectSub(static_cast<BinaryNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)),
                          *cgFunc.HandleExpr(expr, *expr.Opnd(1)));
}

Operand *HandleBand(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  return cgFunc.SelectBand(static_cast<BinaryNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)),
                           *cgFunc.HandleExpr(expr, *expr.Opnd(1)));
}

Operand *HandleBior(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  return cgFunc.SelectBior(static_cast<BinaryNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)),
                           *cgFunc.HandleExpr(expr, *expr.Opnd(1)));
}

Operand *HandleBxor(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  return cgFunc.SelectBxor(static_cast<BinaryNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)),
                           *cgFunc.HandleExpr(expr, *expr.Opnd(1)));
}

Operand *HandleAbs(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  return cgFunc.SelectAbs(static_cast<UnaryNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)));
}

Operand *HandleBnot(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  return cgFunc.SelectBnot(static_cast<UnaryNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)));
}

Operand *HandleExtractBits(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  return cgFunc.SelectExtractbits(static_cast<ExtractbitsNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)));
}

Operand *HandleDepositBits(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  return cgFunc.SelectDepositBits(static_cast<DepositbitsNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)),
                                  *cgFunc.HandleExpr(expr, *expr.Opnd(1)));
}

Operand *HandleLnot(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  return cgFunc.SelectLnot(static_cast<UnaryNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)));
}

Operand *HandleLand(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  return cgFunc.SelectLand(static_cast<BinaryNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)),
                           *cgFunc.HandleExpr(expr, *expr.Opnd(1)));
}

Operand *HandleLor(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  if (parent.IsCondBr()) {
    return cgFunc.SelectLor(static_cast<BinaryNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)),
                            *cgFunc.HandleExpr(expr, *expr.Opnd(1)), true);
  } else {
    return cgFunc.SelectLor(static_cast<BinaryNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)),
                            *cgFunc.HandleExpr(expr, *expr.Opnd(1)));
  }
}

Operand *HandleMin(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  return cgFunc.SelectMin(static_cast<BinaryNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)),
                          *cgFunc.HandleExpr(expr, *expr.Opnd(1)));
}

Operand *HandleMax(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  return cgFunc.SelectMax(static_cast<BinaryNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)),
                          *cgFunc.HandleExpr(expr, *expr.Opnd(1)));
}

Operand *HandleNeg(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  return cgFunc.SelectNeg(static_cast<UnaryNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)));
}

Operand *HandleRecip(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  return cgFunc.SelectRecip(static_cast<UnaryNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)));
}

Operand *HandleSqrt(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  return cgFunc.SelectSqrt(static_cast<UnaryNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)));
}

Operand *HandleCeil(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  return cgFunc.SelectCeil(static_cast<TypeCvtNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)));
}

Operand *HandleFloor(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  return cgFunc.SelectFloor(static_cast<TypeCvtNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)));
}

Operand *HandleRetype(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  return cgFunc.SelectRetype(static_cast<TypeCvtNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)));
}

Operand *HandleCvt(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  return cgFunc.SelectCvt(parent, static_cast<TypeCvtNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)));
}

Operand *HandleRound(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  return cgFunc.SelectRound(static_cast<TypeCvtNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)));
}

Operand *HandleTrunc(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  return cgFunc.SelectTrunc(static_cast<TypeCvtNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)));
}

Operand *HandleSelect(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  /* 0,1,2 represent the first opnd and the second opnd and the third opnd of expr */
  return cgFunc.SelectSelect(static_cast<TernaryNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)),
                             *cgFunc.HandleExpr(expr, *expr.Opnd(1)), *cgFunc.HandleExpr(expr, *expr.Opnd(2)));
}

Operand *HandleCmp(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  return cgFunc.SelectCmpOp(static_cast<CompareNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)),
                            *cgFunc.HandleExpr(expr, *expr.Opnd(1)));
}

Operand *HandleAlloca(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  return cgFunc.SelectAlloca(static_cast<UnaryNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)));
}

Operand *HandleMalloc(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  return cgFunc.SelectMalloc(static_cast<UnaryNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)));
}

Operand *HandleGCMalloc(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  return cgFunc.SelectGCMalloc(static_cast<GCMallocNode&>(expr));
}

Operand *HandleJarrayMalloc(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  return cgFunc.SelectJarrayMalloc(static_cast<JarrayMallocNode&>(expr), *cgFunc.HandleExpr(expr, *expr.Opnd(0)));
}

Operand *HandleIntrinOp(const BaseNode &parent, BaseNode &expr, CGFunc &cgFunc) {
  (void)parent;
  auto &intrinsicopNode = static_cast<IntrinsicopNode&>(expr);
  switch (intrinsicopNode.GetIntrinsic()) {
    case INTRN_MPL_READ_OVTABLE_ENTRY_LAZY: {
      Operand *srcOpnd = cgFunc.HandleExpr(intrinsicopNode, *intrinsicopNode.Opnd(0));
      return cgFunc.SelectLazyLoad(*srcOpnd, intrinsicopNode.GetPrimType());
    }
    case INTRN_MPL_READ_STATIC_OFFSET_TAB: {
      auto addrOfNode = static_cast<AddrofNode*>(intrinsicopNode.Opnd(0));
      MIRSymbol *st = cgFunc.GetMirModule().CurFunction()->GetLocalOrGlobalSymbol(addrOfNode->GetStIdx());
      auto constNode = static_cast<ConstvalNode*>(intrinsicopNode.Opnd(1));
      CHECK_FATAL(constNode != nullptr, "null ptr check");
      auto mirIntConst = static_cast<MIRIntConst*>(constNode->GetConstVal());
      return cgFunc.SelectLazyLoadStatic(*st, mirIntConst->GetValue(), intrinsicopNode.GetPrimType());
    }
    case INTRN_MPL_READ_ARRAYCLASS_CACHE_ENTRY: {
      auto addrOfNode = static_cast<AddrofNode*>(intrinsicopNode.Opnd(0));
      MIRSymbol *st = cgFunc.GetMirModule().CurFunction()->GetLocalOrGlobalSymbol(addrOfNode->GetStIdx());
      auto constNode = static_cast<ConstvalNode*>(intrinsicopNode.Opnd(1));
      CHECK_FATAL(constNode != nullptr, "null ptr check");
      auto mirIntConst = static_cast<MIRIntConst*>(constNode->GetConstVal());
      return cgFunc.SelectLoadArrayClassCache(*st, mirIntConst->GetValue(), intrinsicopNode.GetPrimType());
    }
    default:
      ASSERT(false, "Should not reach here.");
      return nullptr;
  }
}

using HandleExprFactory = FunctionFactory<Opcode, maplebe::Operand*, const BaseNode&, BaseNode&, CGFunc&>;
void InitHandleExprFactory() {
  RegisterFactoryFunction<HandleExprFactory>(OP_dread, HandleDread);
  RegisterFactoryFunction<HandleExprFactory>(OP_regread, HandleRegread);
  RegisterFactoryFunction<HandleExprFactory>(OP_constval, HandleConstVal);
  RegisterFactoryFunction<HandleExprFactory>(OP_conststr, HandleConstStr);
  RegisterFactoryFunction<HandleExprFactory>(OP_conststr16, HandleConstStr16);
  RegisterFactoryFunction<HandleExprFactory>(OP_add, HandleAdd);
  RegisterFactoryFunction<HandleExprFactory>(OP_ashr, HandleShift);
  RegisterFactoryFunction<HandleExprFactory>(OP_lshr, HandleShift);
  RegisterFactoryFunction<HandleExprFactory>(OP_shl, HandleShift);
  RegisterFactoryFunction<HandleExprFactory>(OP_mul, HandleMpy);
  RegisterFactoryFunction<HandleExprFactory>(OP_div, HandleDiv);
  RegisterFactoryFunction<HandleExprFactory>(OP_rem, HandleRem);
  RegisterFactoryFunction<HandleExprFactory>(OP_addrof, HandleAddrof);
  RegisterFactoryFunction<HandleExprFactory>(OP_addroffunc, HandleAddroffunc);
  RegisterFactoryFunction<HandleExprFactory>(OP_iread, HandleIread);
  RegisterFactoryFunction<HandleExprFactory>(OP_sub, HandleSub);
  RegisterFactoryFunction<HandleExprFactory>(OP_band, HandleBand);
  RegisterFactoryFunction<HandleExprFactory>(OP_bior, HandleBior);
  RegisterFactoryFunction<HandleExprFactory>(OP_bxor, HandleBxor);
  RegisterFactoryFunction<HandleExprFactory>(OP_abs, HandleAbs);
  RegisterFactoryFunction<HandleExprFactory>(OP_bnot, HandleBnot);
  RegisterFactoryFunction<HandleExprFactory>(OP_sext, HandleExtractBits);
  RegisterFactoryFunction<HandleExprFactory>(OP_zext, HandleExtractBits);
  RegisterFactoryFunction<HandleExprFactory>(OP_extractbits, HandleExtractBits);
  RegisterFactoryFunction<HandleExprFactory>(OP_depositbits, HandleDepositBits);
  RegisterFactoryFunction<HandleExprFactory>(OP_lnot, HandleLnot);
  RegisterFactoryFunction<HandleExprFactory>(OP_land, HandleLand);
  RegisterFactoryFunction<HandleExprFactory>(OP_lior, HandleLor);
  RegisterFactoryFunction<HandleExprFactory>(OP_min, HandleMin);
  RegisterFactoryFunction<HandleExprFactory>(OP_max, HandleMax);
  RegisterFactoryFunction<HandleExprFactory>(OP_neg, HandleNeg);
  RegisterFactoryFunction<HandleExprFactory>(OP_recip, HandleRecip);
  RegisterFactoryFunction<HandleExprFactory>(OP_sqrt, HandleSqrt);
  RegisterFactoryFunction<HandleExprFactory>(OP_ceil, HandleCeil);
  RegisterFactoryFunction<HandleExprFactory>(OP_floor, HandleFloor);
  RegisterFactoryFunction<HandleExprFactory>(OP_retype, HandleRetype);
  RegisterFactoryFunction<HandleExprFactory>(OP_cvt, HandleCvt);
  RegisterFactoryFunction<HandleExprFactory>(OP_round, HandleRound);
  RegisterFactoryFunction<HandleExprFactory>(OP_trunc, HandleTrunc);
  RegisterFactoryFunction<HandleExprFactory>(OP_select, HandleSelect);
  RegisterFactoryFunction<HandleExprFactory>(OP_le, HandleCmp);
  RegisterFactoryFunction<HandleExprFactory>(OP_ge, HandleCmp);
  RegisterFactoryFunction<HandleExprFactory>(OP_gt, HandleCmp);
  RegisterFactoryFunction<HandleExprFactory>(OP_lt, HandleCmp);
  RegisterFactoryFunction<HandleExprFactory>(OP_ne, HandleCmp);
  RegisterFactoryFunction<HandleExprFactory>(OP_eq, HandleCmp);
  RegisterFactoryFunction<HandleExprFactory>(OP_cmp, HandleCmp);
  RegisterFactoryFunction<HandleExprFactory>(OP_cmpl, HandleCmp);
  RegisterFactoryFunction<HandleExprFactory>(OP_cmpg, HandleCmp);
  RegisterFactoryFunction<HandleExprFactory>(OP_alloca, HandleAlloca);
  RegisterFactoryFunction<HandleExprFactory>(OP_malloc, HandleMalloc);
  RegisterFactoryFunction<HandleExprFactory>(OP_gcmalloc, HandleGCMalloc);
  RegisterFactoryFunction<HandleExprFactory>(OP_gcpermalloc, HandleGCMalloc);
  RegisterFactoryFunction<HandleExprFactory>(OP_gcmallocjarray, HandleJarrayMalloc);
  RegisterFactoryFunction<HandleExprFactory>(OP_gcpermallocjarray, HandleJarrayMalloc);
  RegisterFactoryFunction<HandleExprFactory>(OP_intrinsicop, HandleIntrinOp);
}

void HandleLabel(StmtNode &stmt, CGFunc &cgFunc) {
  ASSERT(stmt.GetOpCode() == OP_label, "error");
  auto &label = static_cast<LabelNode&>(stmt);
  BB *newBB = cgFunc.StartNewBBImpl(false, label);
  newBB->AddLabel(label.GetLabelIdx());
  cgFunc.SetLab2BBMap(newBB->GetLabIdx(), *newBB);
  cgFunc.SetCurBB(*newBB);
}

void HandleGoto(StmtNode &stmt, CGFunc &cgFunc) {
  auto &gotoNode = static_cast<GotoNode&>(stmt);
  cgFunc.SetCurBBKind(BB::kBBGoto);
  cgFunc.SelectGoto(gotoNode);
  cgFunc.SetCurBB(*cgFunc.StartNewBB(gotoNode));
  ASSERT(&stmt == &gotoNode, "stmt must be same as gotoNoe");

  if ((gotoNode.GetNext() != nullptr) && (gotoNode.GetNext()->GetOpCode() != OP_label)) {
    ASSERT(cgFunc.GetCurBB()->GetPrev()->GetLastStmt() == &stmt, "check the relation between BB and stmt");
  }
}

void HandleCondbr(StmtNode &stmt, CGFunc &cgFunc) {
  auto &condGotoNode = static_cast<CondGotoNode&>(stmt);
  BaseNode *condNode = condGotoNode.Opnd(0);
  ASSERT(condNode != nullptr, "expect first operand of cond br");
  Opcode condOp = condGotoNode.GetOpCode();
  if (condNode->GetOpCode() == OP_constval) {
    auto *constValNode = static_cast<ConstvalNode*>(condNode);
    if ((constValNode->GetConstVal()->IsZero() && (OP_brfalse == condOp)) ||
        (!constValNode->GetConstVal()->IsZero() && (OP_brtrue == condOp))) {
      auto *gotoStmt = cgFunc.GetMemoryPool()->New<GotoNode>(OP_goto);
      gotoStmt->SetOffset(condGotoNode.GetOffset());
      HandleGoto(*gotoStmt, cgFunc);
      auto *labelStmt = cgFunc.GetMemoryPool()->New<LabelNode>();
      labelStmt->SetLabelIdx(cgFunc.CreateLabel());
      HandleLabel(*labelStmt, cgFunc);
    }
    return;
  }
  cgFunc.SetCurBBKind(BB::kBBIf);
  /* if condNode is not a cmp node, cmp it with zero. */
  if (!kOpcodeInfo.IsCompare(condNode->GetOpCode())) {
    Operand *opnd0 = cgFunc.HandleExpr(condGotoNode, *condNode);
    PrimType primType = condNode->GetPrimType();
    Operand *zeroOpnd = nullptr;
    if (IsPrimitiveInteger(primType)) {
      zeroOpnd = &cgFunc.CreateImmOperand(primType, 0);
    } else {
      ASSERT(((PTY_f32 == primType) || (PTY_f64 == primType)), "we don't support half-precision FP operands yet");
      zeroOpnd = &cgFunc.CreateFPImmZero(primType);
    }
    cgFunc.SelectCondGoto(condGotoNode, *opnd0, *zeroOpnd);
    cgFunc.SetCurBB(*cgFunc.StartNewBB(condGotoNode));
    return;
  }
  /*
   * Special case:
   * bgt (cmp (op0, op1), 0) ==>
   * bgt (op0, op1)
   * but skip the case cmp(op0, 0)
   */
  BaseNode *op0 = condNode->Opnd(0);
  ASSERT(op0 != nullptr, "get first opnd of a condNode failed");
  BaseNode *op1 = condNode->Opnd(1);
  ASSERT(op1 != nullptr, "get second opnd of a condNode failed");
  if ((op0->GetOpCode() == OP_cmp) && (op1->GetOpCode() == OP_constval)) {
    auto *constValNode = static_cast<ConstvalNode*>(op1);
    MIRConst *mirConst = constValNode->GetConstVal();
    auto *cmpNode = static_cast<CompareNode*>(op0);
    bool skip = false;
    if (cmpNode->Opnd(1)->GetOpCode() == OP_constval) {
      auto *constVal = static_cast<ConstvalNode*>(cmpNode->Opnd(1))->GetConstVal();
      if (constVal->IsZero()) {
        skip = true;
      }
    }
    if (!skip && mirConst->IsZero()) {
      cgFunc.SelectCondSpecialCase1(condGotoNode, *op0);
      cgFunc.SetCurBB(*cgFunc.StartNewBB(condGotoNode));
      return;
    }
  }
  /*
   * Special case:
   * brfalse(ge (cmpg (op0, op1), 0) ==>
   * fcmp op1, op2
   * blo
   */
  if ((condGotoNode.GetOpCode() == OP_brfalse) && (condNode->GetOpCode() == OP_ge) &&
      (op0->GetOpCode() == OP_cmpg) && (op1->GetOpCode() == OP_constval)) {
    auto *constValNode = static_cast<ConstvalNode*>(op1);
    MIRConst *mirConst = constValNode->GetConstVal();
    if (mirConst->IsZero()) {
      cgFunc.SelectCondSpecialCase2(condGotoNode, *op0);
      cgFunc.SetCurBB(*cgFunc.StartNewBB(condGotoNode));
      return;
    }
  }
  Operand *opnd0 = cgFunc.HandleExpr(*condNode, *condNode->Opnd(0));
  Operand *opnd1 = cgFunc.HandleExpr(*condNode, *condNode->Opnd(1));
  cgFunc.SelectCondGoto(condGotoNode, *opnd0, *opnd1);
  cgFunc.SetCurBB(*cgFunc.StartNewBB(condGotoNode));
}

void HandleReturn(StmtNode &stmt, CGFunc &cgFunc) {
  auto &retNode = static_cast<NaryStmtNode&>(stmt);
  cgFunc.HandleRetCleanup(retNode);
  ASSERT(retNode.NumOpnds() <= 1, "NYI return nodes number > 1");
  Operand *opnd = nullptr;
  if (retNode.NumOpnds() != 0) {
    opnd = cgFunc.HandleExpr(retNode, *retNode.Opnd(0));
  }
  cgFunc.SelectReturn(opnd);
  cgFunc.SetCurBBKind(BB::kBBReturn);
  cgFunc.SetCurBB(*cgFunc.StartNewBB(retNode));
}

void HandleCall(StmtNode &stmt, CGFunc &cgFunc) {
  auto &callNode = static_cast<CallNode&>(stmt);
  cgFunc.SelectCall(callNode);
  if (cgFunc.GetCurBB()->GetKind() != BB::kBBFallthru) {
    cgFunc.SetCurBB(*cgFunc.StartNewBB(callNode));
  }

  StmtNode *prevStmt = stmt.GetPrev();
  if (prevStmt == nullptr || prevStmt->GetOpCode() != OP_catch) {
    return;
  }
  if ((stmt.GetNext() != nullptr) && (stmt.GetNext()->GetOpCode() == OP_label)) {
    cgFunc.SetCurBB(*cgFunc.StartNewBBImpl(true, stmt));
  }
  cgFunc.HandleCatch();
}

void HandleICall(StmtNode &stmt, CGFunc &cgFunc) {
  auto &icallNode = static_cast<IcallNode&>(stmt);
  cgFunc.GetCurBB()->SetHasCall();
  Operand *opnd0 = cgFunc.HandleExpr(stmt, *icallNode.GetNopndAt(0));
  cgFunc.SelectIcall(icallNode, *opnd0);
  if (cgFunc.GetCurBB()->GetKind() != BB::kBBFallthru) {
    cgFunc.SetCurBB(*cgFunc.StartNewBB(icallNode));
  }
}

void HandleIntrinCall(StmtNode &stmt, CGFunc &cgFunc) {
  auto &call = static_cast<IntrinsiccallNode&>(stmt);
  cgFunc.SelectIntrinCall(call);
}

void HandleDassign(StmtNode &stmt, CGFunc &cgFunc) {
  auto &dassignNode = static_cast<DassignNode&>(stmt);
  ASSERT(dassignNode.GetOpCode() == OP_dassign, "expect dassign");
  BaseNode *rhs = dassignNode.GetRHS();
  ASSERT(rhs != nullptr, "get rhs of dassignNode failed");
  if (rhs->GetPrimType() == PTY_agg) {
    cgFunc.SelectAggDassign(dassignNode);
    return;
  }
  bool isSaveRetvalToLocal = false;
  if (rhs->GetOpCode() == OP_regread) {
    isSaveRetvalToLocal = (static_cast<RegreadNode*>(rhs)->GetRegIdx() == -kSregRetval0);
  }
  Operand *opnd0 = cgFunc.HandleExpr(dassignNode, *rhs);
  cgFunc.SelectDassign(dassignNode, *opnd0);
  if (isSaveRetvalToLocal) {
    cgFunc.GetCurBB()->GetLastInsn()->MarkAsSaveRetValToLocal();
  }
}

void HandleRegassign(StmtNode &stmt, CGFunc &cgFunc) {
  ASSERT(stmt.GetOpCode() == OP_regassign, "expect regAssign");
  auto &regAssignNode = static_cast<RegassignNode&>(stmt);
  bool isSaveRetvalToLocal = false;
  BaseNode *operand = regAssignNode.Opnd(0);
  ASSERT(operand != nullptr, "get operand of regassignNode failed");
  if (operand->GetOpCode() == OP_regread) {
    isSaveRetvalToLocal = (static_cast<RegreadNode*>(operand)->GetRegIdx() == -kSregRetval0);
  }
  Operand *opnd0 = cgFunc.HandleExpr(regAssignNode, *operand);
  cgFunc.SelectRegassign(regAssignNode, *opnd0);
  if (isSaveRetvalToLocal) {
    cgFunc.GetCurBB()->GetLastInsn()->MarkAsSaveRetValToLocal();
  }
}

void HandleIassign(StmtNode &stmt, CGFunc &cgFunc) {
  ASSERT(stmt.GetOpCode() == OP_iassign, "expect stmt");
  auto &iassignNode = static_cast<IassignNode&>(stmt);
  if ((iassignNode.GetRHS() != nullptr) && iassignNode.GetRHS()->GetPrimType() != PTY_agg) {
    cgFunc.SelectIassign(iassignNode);
  } else {
    BaseNode *addrNode = iassignNode.Opnd(0);
    if (addrNode == nullptr) {
      return;
    }
    cgFunc.SelectAggIassign(iassignNode, *cgFunc.HandleExpr(stmt, *addrNode));
  }
}

void HandleEval(StmtNode &stmt, CGFunc &cgFunc) {
  cgFunc.HandleExpr(stmt, *static_cast<UnaryStmtNode&>(stmt).Opnd(0));
}

void HandleRangeGoto(StmtNode &stmt, CGFunc &cgFunc) {
  auto &rangeGotoNode = static_cast<RangeGotoNode&>(stmt);
  cgFunc.SetCurBBKind(BB::kBBRangeGoto);
  cgFunc.SelectRangeGoto(rangeGotoNode, *cgFunc.HandleExpr(rangeGotoNode, *rangeGotoNode.Opnd(0)));
  cgFunc.SetCurBB(*cgFunc.StartNewBB(rangeGotoNode));
}

void HandleMembar(StmtNode &stmt, CGFunc &cgFunc) {
  cgFunc.SelectMembar(stmt);
  if (stmt.GetOpCode() != OP_membarrelease) {
    return;
  }
#if TARGAARCH64
  if (CGOptions::UseBarriersForVolatile()) {
    return;
  }
#endif
  StmtNode *secondStmt = stmt.GetRealNext();
  if (secondStmt == nullptr ||
      ((secondStmt->GetOpCode() != OP_iassign) && (secondStmt->GetOpCode() != OP_dassign))) {
    return;
  }
  StmtNode *thirdStmt = secondStmt->GetRealNext();
  if (thirdStmt == nullptr || thirdStmt->GetOpCode() != OP_membarstoreload) {
    return;
  }
  cgFunc.SetVolStore(true);
  cgFunc.SetVolReleaseInsn(cgFunc.GetCurBB()->GetLastInsn());
}

void HandleComment(StmtNode &stmt, CGFunc &cgFunc) {
  if (cgFunc.GetCG()->GenerateVerboseAsm() || cgFunc.GetCG()->GenerateVerboseCG()) {
    cgFunc.SelectComment(static_cast<CommentNode&>(stmt));
  }
}

void HandleCatchOp(StmtNode &stmt, CGFunc &cgFunc) {
  (void)stmt;
  (void)cgFunc;
  ASSERT(stmt.GetNext()->GetOpCode() == OP_call, "The next statement of OP_catch should be OP_call.");
}

void HandleAssertNull(StmtNode &stmt, CGFunc &cgFunc) {
  auto &cgAssertNode = static_cast<UnaryStmtNode&>(stmt);
  cgFunc.SelectAssertNull(cgAssertNode);
}

using HandleStmtFactory = FunctionFactory<Opcode, void, StmtNode&, CGFunc&>;
void InitHandleStmtFactory() {
  RegisterFactoryFunction<HandleStmtFactory>(OP_label, HandleLabel);
  RegisterFactoryFunction<HandleStmtFactory>(OP_goto, HandleGoto);
  RegisterFactoryFunction<HandleStmtFactory>(OP_brfalse, HandleCondbr);
  RegisterFactoryFunction<HandleStmtFactory>(OP_brtrue, HandleCondbr);
  RegisterFactoryFunction<HandleStmtFactory>(OP_return, HandleReturn);
  RegisterFactoryFunction<HandleStmtFactory>(OP_call, HandleCall);
  RegisterFactoryFunction<HandleStmtFactory>(OP_icall, HandleICall);
  RegisterFactoryFunction<HandleStmtFactory>(OP_intrinsiccall, HandleIntrinCall);
  RegisterFactoryFunction<HandleStmtFactory>(OP_intrinsiccallassigned, HandleIntrinCall);
  RegisterFactoryFunction<HandleStmtFactory>(OP_intrinsiccallwithtype, HandleIntrinCall);
  RegisterFactoryFunction<HandleStmtFactory>(OP_intrinsiccallwithtypeassigned, HandleIntrinCall);
  RegisterFactoryFunction<HandleStmtFactory>(OP_dassign, HandleDassign);
  RegisterFactoryFunction<HandleStmtFactory>(OP_regassign, HandleRegassign);
  RegisterFactoryFunction<HandleStmtFactory>(OP_iassign, HandleIassign);
  RegisterFactoryFunction<HandleStmtFactory>(OP_eval, HandleEval);
  RegisterFactoryFunction<HandleStmtFactory>(OP_rangegoto, HandleRangeGoto);
  RegisterFactoryFunction<HandleStmtFactory>(OP_membarrelease, HandleMembar);
  RegisterFactoryFunction<HandleStmtFactory>(OP_membaracquire, HandleMembar);
  RegisterFactoryFunction<HandleStmtFactory>(OP_membarstoreload, HandleMembar);
  RegisterFactoryFunction<HandleStmtFactory>(OP_membarstorestore, HandleMembar);
  RegisterFactoryFunction<HandleStmtFactory>(OP_comment, HandleComment);
  RegisterFactoryFunction<HandleStmtFactory>(OP_catch, HandleCatchOp);
  RegisterFactoryFunction<HandleStmtFactory>(OP_assertnonnull, HandleAssertNull);
}

CGFunc::CGFunc(MIRModule &mod, CG &cg, MIRFunction &mirFunc, BECommon &beCommon, MemPool &memPool,
               MapleAllocator &allocator, uint32 funcId)
    : vRegTable(allocator.Adapter()),
      vRegOperandTable(std::less<regno_t>(), allocator.Adapter()),
      pRegSpillMemOperands(std::less<PregIdx>(), allocator.Adapter()),
      spillRegMemOperands(std::less<regno_t>(), allocator.Adapter()),
      spillRegMemOperandsAdj(allocator.Adapter()),
      reuseSpillLocMem(std::less<uint32>(), allocator.Adapter()),
      labelMap(std::less<LabelIdx>(), allocator.Adapter()),
      cg(&cg),
      mirModule(mod),
      memPool(&memPool),
      func(mirFunc),
      exitBBVec(allocator.Adapter()),
      lab2BBMap(std::less<LabelIdx>(), allocator.Adapter()),
      beCommon(beCommon),
      funcScopeAllocator(&allocator),
      emitStVec(allocator.Adapter()),
#if TARGARM32
      sortedBBs(allocator.Adapter()),
      lrVec(allocator.Adapter()),
#endif  /* TARGARM32 */
      loops(allocator.Adapter()),
      shortFuncName(cg.ExtractFuncName(mirFunc.GetName()) + "." + std::to_string(funcId), &memPool) {
  mirModule.SetCurFunction(&func);
  dummyBB = CreateNewBB();
  vRegCount = firstMapleIrVRegNO + func.GetPregTab()->Size();
  firstNonPregVRegNO = vRegCount;
  /* maximum register count initial be increased by 1024 */
  maxRegCount = vRegCount + 1024;

  vRegTable.resize(maxRegCount);
  /* func.GetPregTab()->_preg_table[0] is nullptr, so skip it */
  ASSERT(func.GetPregTab()->PregFromPregIdx(0) == nullptr, "PregFromPregIdx(0) must be nullptr");
  for (size_t i = 1; i < func.GetPregTab()->Size(); ++i) {
    PrimType primType = func.GetPregTab()->PregFromPregIdx(i)->GetPrimType();
    uint32 byteLen = GetPrimTypeSize(primType);
    if (byteLen < k4ByteSize) {
      byteLen = k4ByteSize;
    }
    new (&GetVirtualRegNodeFromPseudoRegIdx(i)) VirtualRegNode(GetRegTyFromPrimTy(primType), byteLen);
  }
  firstCGGenLabelIdx = func.GetLabelTab()->GetLabelTableSize();
}

CGFunc::~CGFunc() {
  mirModule.SetCurFunction(nullptr);
}

Operand *CGFunc::HandleExpr(const BaseNode &parent, BaseNode &expr) {
  auto function = CreateProductFunction<HandleExprFactory>(expr.GetOpCode());
  CHECK_FATAL(function != nullptr, "unsupported opCode in HandleExpr()");
  return function(parent, expr, *this);
}

StmtNode *CGFunc::HandleFirstStmt() {
  BlockNode *block = func.GetBody();

  ASSERT(block != nullptr, "get func body block failed in CGFunc::GenerateInstruction");
  StmtNode *stmt = block->GetFirst();
  if (stmt == nullptr) {
    return nullptr;
  }
  ASSERT(stmt->GetOpCode() == OP_label, "The first statement should be a label");
  HandleLabel(*stmt, *this);
  firstBB = curBB;
  stmt = stmt->GetNext();
  if (stmt == nullptr) {
    return nullptr;
  }
  curBB = StartNewBBImpl(false, *stmt);
  curBB->SetFrequency(frequency);
  if (JAVALANG) {
    HandleRCCall(true);
  }
  return stmt;
}

bool CGFunc::CheckSkipMembarOp(StmtNode &stmt) {
  StmtNode *nextStmt = stmt.GetRealNext();
  if (nextStmt == nullptr) {
    return false;
  }

  Opcode opCode = stmt.GetOpCode();
  if (((opCode == OP_membaracquire) || (opCode == OP_membarrelease)) && (nextStmt->GetOpCode() == stmt.GetOpCode())) {
    return true;
  }
  if ((opCode == OP_membarstorestore) && (nextStmt->GetOpCode() == OP_membarrelease)) {
    return true;
  }
  if ((opCode == OP_membarstorestore) && func.IsConstructor() && MemBarOpt(stmt)) {
    return true;;
  }
#if TARGARM32
  if (nextStmt->GetOpCode() == OP_membaracquire) {
    isVolLoad = true;
  }
#else
  if ((!CGOptions::UseBarriersForVolatile()) && (nextStmt->GetOpCode() == OP_membaracquire)) {
    isVolLoad = true;
  }
#endif /* TARGARM32 */
  return false;
}

void CGFunc::GenerateInstruction() {
  InitHandleExprFactory();
  InitHandleStmtFactory();
  StmtNode *secondStmt = HandleFirstStmt();

  /* First Pass: Creates the doubly-linked list of BBs (next,prev) */
  volReleaseInsn = nullptr;
  for (StmtNode *stmt = secondStmt; stmt != nullptr; stmt = stmt->GetNext()) {
    isVolLoad = false;
    if (CheckSkipMembarOp(*stmt)) {
      continue;
    }
    bool tempLoad = isVolLoad;

    auto function = CreateProductFunction<HandleStmtFactory>(stmt->GetOpCode());
    CHECK_FATAL(function != nullptr, "unsupported opCode or has been lowered before");
    function(*stmt, *this);

    /* skip the membar acquire if it is just after the iread. ldr + membaraquire->ldar */
    if (tempLoad && !isVolLoad) {
      stmt = stmt->GetNext();
    }

#if TARGARM32
    isVolLoad = false;
#endif

    /*
     * skip the membarstoreload if there is the pattern for volatile write( membarrelease + store + membarstoreload )
     * membarrelease + store + membarstoreload -> stlr
     */
    if (volReleaseInsn != nullptr) {
      if ((stmt->GetOpCode() != OP_membarrelease) && (stmt->GetOpCode() != OP_comment)) {
        if (!isVolStore) {
          /* remove the generated membar release insn. */
          curBB->RemoveInsn(*volReleaseInsn);
          /* skip the membarstoreload. */
          stmt = stmt->GetNext();
        }
        volReleaseInsn = nullptr;
        isVolStore = false;
      }
    }
  }

  /* Set lastbb's frequency */
  BlockNode *block = func.GetBody();
  ASSERT(block != nullptr, "get func body block failed in CGFunc::GenerateInstruction");
  curBB->SetLastStmt(*block->GetLast());
  curBB->SetFrequency(frequency);
  lastBB = curBB;
  cleanupBB = lastBB->GetPrev();
  /* All stmts are handled */
  frequency = 0;
}

LabelIdx CGFunc::CreateLabel() {
  MIRSymbol *funcSt = GlobalTables::GetGsymTable().GetSymbolFromStidx(func.GetStIdx().Idx());
  ASSERT(funcSt != nullptr, "Get func failed at CGFunc::CreateLabel");
  std::string funcName = funcSt->GetName();
  std::string labelStr = funcName.append(std::to_string(labelIdx++));
  return func.GetOrCreateLableIdxFromName(labelStr);
}

MIRSymbol *CGFunc::GetRetRefSymbol(BaseNode &expr) {
  Opcode opcode = expr.GetOpCode();
  if (opcode != OP_dread) {
    return nullptr;
  }
  auto &retExpr = static_cast<AddrofNode&>(expr);
  MIRSymbol *symbol = mirModule.CurFunction()->GetLocalOrGlobalSymbol(retExpr.GetStIdx());
  ASSERT(symbol != nullptr, "get symbol in mirmodule failed");
  if (symbol->IsRefType()) {
    MIRSymbol *sym = nullptr;
    for (uint32 i = 0; i < func.GetFormalCount(); i++) {
      sym = func.GetFormal(i);
      if (sym == symbol) {
        return nullptr;
      }
    }
    return symbol;
  }
  return nullptr;
}

void CGFunc::GenerateCfiPrologEpilog() {
  Insn &ipoint = GetCG()->BuildInstruction<cfi::CfiInsn>(cfi::OP_CFI_startproc);
  /* prolog */
  if (firstBB->GetFirstInsn() != nullptr) {
    firstBB->InsertInsnBefore(*firstBB->GetFirstInsn(), ipoint);
  } else {
    firstBB->AppendInsn(ipoint);
  }

#if !defined(TARGARM32)
  /*
   * always generate ".cfi_personality 155, DW.ref.__mpl_personality_v0" for Java methods.
   * we depend on this to tell whether it is a java method.
   */
  if (mirModule.IsJavaModule() && func.IsJava()) {
    Insn &personality = GetCG()->BuildInstruction<cfi::CfiInsn>(cfi::OP_CFI_personality_symbol,
                                                                CreateCfiImmOperand(EHFunc::kTypeEncoding, k8BitSize),
                                                                CreateCfiStrOperand("DW.ref.__mpl_personality_v0"));
    firstBB->InsertInsnAfter(ipoint, personality);
  }
#endif

  /* epilog */
  lastBB->AppendInsn(GetCG()->BuildInstruction<cfi::CfiInsn>(cfi::OP_CFI_endproc));
}

void CGFunc::TraverseAndClearCatchMark(BB &bb) {
  /* has bb been visited */
  if (bb.GetInternalFlag3()) {
    return;
  }
  bb.SetIsCatch(false);
  bb.SetInternalFlag3(1);
  for (auto succBB : bb.GetSuccs()) {
    TraverseAndClearCatchMark(*succBB);
  }
}

/*
 * Two types of successor edges, normal and eh. Any bb which is not
 * reachable by a normal successor edge is considered to be in a
 * catch block.
 * Marking it as a catch block does not automatically make it into
 * a catch block. Unreachables can be marked as such too.
 */
void CGFunc::MarkCatchBBs() {
  /* First, suspect all bb to be in catch */
  FOR_ALL_BB(bb, this) {
    bb->SetIsCatch(true);
    bb->SetInternalFlag3(0);  /* mark as not visited */
  }
  /* Eliminate cleanup section from catch */
  FOR_ALL_BB(bb, this) {
    if (bb->GetFirstStmt() == cleanupLabel) {
      bb->SetIsCatch(false);
      ASSERT(bb->GetSuccs().size() <= 1, "MarkCatchBBs incorrect cleanup label");
      BB *succ = nullptr;
      if (!bb->GetSuccs().empty()) {
        succ = bb->GetSuccs().front();
      } else {
        continue;
      }
      ASSERT(succ != nullptr, "Get front succsBB failed");
      while (1) {
        ASSERT(succ->GetSuccs().size() <= 1, "MarkCatchBBs incorrect cleanup label");
        succ->SetIsCatch(false);
        if (!succ->GetSuccs().empty()) {
          succ = succ->GetSuccs().front();
        } else {
          break;
        }
      }
    }
  }
  /* Unmark all normally reachable bb as NOT catch. */
  TraverseAndClearCatchMark(*firstBB);
}

/*
 * Mark CleanupEntryBB
 * Note: Cleanup bbs and func body bbs are seperated, no edges between them.
 * No ehSuccs or eh_prevs between cleanup bbs.
 */
void CGFunc::MarkCleanupEntryBB() {
  BB *cleanupEntry = nullptr;
  FOR_ALL_BB(bb, this) {
    bb->SetIsCleanup(0);      /* Use to mark cleanup bb */
    bb->SetInternalFlag3(0);  /* Use to mark if visited. */
    if (bb->GetFirstStmt() == this->cleanupLabel) {
      cleanupEntry = bb;
    }
  }
  /* If a function without cleanup bb, return. */
  if (cleanupEntry == nullptr) {
    return;
  }
  /* after merge bb, update cleanupBB. */
  if (cleanupEntry->GetSuccs().empty()) {
    this->cleanupBB = cleanupEntry;
  }
  SetCleanupLabel(*cleanupEntry);
  ASSERT(cleanupEntry->GetEhSuccs().empty(), "CG internal error. Cleanup bb should not have ehSuccs.");
#if DEBUG  /* Please don't remove me. */
  /* Check if all of the cleanup bb is at bottom of the function. */
  bool isCleanupArea = true;
  if (!mirModule.IsCModule()) {
    FOR_ALL_BB_REV(bb, this) {
      if (isCleanupArea) {
        ASSERT(bb->IsCleanup(), "CG internal error, cleanup BBs should be at the bottom of the function.");
      } else {
        ASSERT(!bb->IsCleanup(), "CG internal error, cleanup BBs should be at the bottom of the function.");
      }

      if (bb == cleanupEntry) {
        isCleanupArea = false;
      }
    }
  }
#endif  /* DEBUG */
  this->cleanupEntryBB = cleanupEntry;
}

/* Tranverse from current bb's successor and set isCleanup true. */
void CGFunc::SetCleanupLabel(BB &cleanupEntry) {
  /* If bb hasn't been visited, return. */
  if (cleanupEntry.GetInternalFlag3()) {
    return;
  }
  cleanupEntry.SetInternalFlag3(1);
  cleanupEntry.SetIsCleanup(1);
  for (auto tmpBB : cleanupEntry.GetSuccs()) {
    if (tmpBB->GetKind() != BB::kBBReturn) {
      SetCleanupLabel(*tmpBB);
    } else {
      ASSERT(ExitbbNotInCleanupArea(cleanupEntry), "exitBB created in cleanupArea.");
    }
  }
}

bool CGFunc::ExitbbNotInCleanupArea(const BB &bb) const {
  for (const BB *nextBB = bb.GetNext(); nextBB != nullptr; nextBB = nextBB->GetNext()) {
    if (nextBB->GetKind() == BB::kBBReturn) {
      return false;
    }
  }
  return true;
}

/*
 * Do mem barrier optimization for constructor funcs as follow:
 * membarstorestore
 * write field of this_  ==> write field of this_
 * membarrelease             membarrelease.
 */
bool CGFunc::MemBarOpt(StmtNode &membar) {
  if (func.GetFormalCount() == 0) {
    return false;
  }
  MIRSymbol *thisSym = func.GetFormal(0);
  if (thisSym == nullptr) {
    return false;
  }
  StmtNode *stmt = membar.GetNext();
  for (; stmt != nullptr; stmt = stmt->GetNext()) {
    BaseNode *base = nullptr;
    if (stmt->GetOpCode() == OP_comment) {
      continue;
    } else if (stmt->GetOpCode() == OP_iassign) {
      base = static_cast<IassignNode *>(stmt)->Opnd(0);
    } else if (stmt->GetOpCode() == OP_call) {
      auto *callNode = static_cast<CallNode*>(stmt);
      MIRFunction *fn = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(callNode->GetPUIdx());
      MIRSymbol *fsym = GetMirModule().CurFunction()->GetLocalOrGlobalSymbol(fn->GetStIdx(), false);
      if (fsym->GetName() == "MCC_WriteRefFieldNoDec") {
        base = callNode->Opnd(0);
      }
    }
    if (base != nullptr) {
      Opcode op = base->GetOpCode();
      if (op == OP_regread && thisSym->IsPreg() &&
          thisSym->GetPreg()->GetPregNo() == static_cast<RegreadNode*>(base)->GetRegIdx()) {
        continue;
      }
      if ((op == OP_dread || op == OP_addrof) && !thisSym->IsPreg() &&
          static_cast<AddrofNode*>(base)->GetStIdx() == thisSym->GetStIdx()) {
        continue;
      }
    }
    break;
  }

  CHECK_NULL_FATAL(stmt);
  return stmt->GetOpCode() == OP_membarrelease;
}

void CGFunc::ProcessExitBBVec() {
  if (exitBBVec.empty()) {
    LabelIdx newLabelIdx = CreateLabel();
    BB *retBB = CreateNewBB(newLabelIdx, cleanupBB->IsUnreachable(), BB::kBBReturn, cleanupBB->GetFrequency());
    cleanupBB->PrependBB(*retBB);
    exitBBVec.push_back(retBB);
    return;
  }
  /* split an empty exitBB */
  BB *bb = exitBBVec[0];
  if (bb->NumInsn() > 0) {
    BB *retBBPart = CreateNewBB(false, BB::kBBFallthru, bb->GetFrequency());
    ASSERT(retBBPart != nullptr, "retBBPart should not be nullptr");
    LabelIdx retBBPartLabelIdx = bb->GetLabIdx();
    if (retBBPartLabelIdx != MIRLabelTable::GetDummyLabel()) {
      retBBPart->AddLabel(retBBPartLabelIdx);
      lab2BBMap[retBBPartLabelIdx] = retBBPart;
    }
    Insn *insn = bb->GetFirstInsn();
    while (insn != nullptr) {
      bb->RemoveInsn(*insn);
      retBBPart->AppendInsn(*insn);
      insn = bb->GetFirstInsn();
    }
    bb->PrependBB(*retBBPart);
    LabelIdx newLabelIdx = CreateLabel();
    bb->AddLabel(newLabelIdx);
    lab2BBMap[newLabelIdx] = bb;
  }
}

void CGFunc::HandleFunction() {
  /* select instruction */
  GenerateInstruction();
  /* merge multi return */
  if (!func.GetModule()->IsCModule()) {
    MergeReturn();
  }
  ASSERT(exitBBVec.size() <= 1, "there are more than one BB_return in func");
  ProcessExitBBVec();

  if (func.IsJava()) {
    GenerateCleanupCodeForExtEpilog(*cleanupBB);
  } else if (!func.GetModule()->IsCModule()) {
    GenerateCleanupCode(*cleanupBB);
  }
  GenSaveMethodInfoCode(*firstBB);
  /* build control flow graph */
  theCFG = memPool->New<CGCFG>(*this);
  theCFG->BuildCFG();
  if (mirModule.GetSrcLang() != kSrcLangC) {
    MarkCatchBBs();
  }
  MarkCleanupEntryBB();
  DetermineReturnTypeofCall();
  theCFG->UnreachCodeAnalysis();
  SplitStrLdrPair();
  if (CGOptions::IsLazyBinding() && !GetCG()->IsLibcore()) {
    ProcessLazyBinding();
  }
}

void CGFunc::DumpCFG() const {
  MIRSymbol *funcSt = GlobalTables::GetGsymTable().GetSymbolFromStidx(func.GetStIdx().Idx());
  LogInfo::MapleLogger() << "\n****** CFG built by CG for " << funcSt->GetName() << " *******\n";
  FOR_ALL_BB_CONST(bb, this) {
    LogInfo::MapleLogger() << "=== BB ( " << std::hex << bb << std::dec << " ) <" << bb->GetKindName() << "> ===\n";
    LogInfo::MapleLogger() << "BB id:" << bb->GetId() << "\n";
    if (!bb->GetPreds().empty()) {
      LogInfo::MapleLogger() << " pred [ ";
      for (auto *pred : bb->GetPreds()) {
        LogInfo::MapleLogger() << std::hex << pred << std::dec << " ";
      }
      LogInfo::MapleLogger() << "]\n";
    }
    if (!bb->GetSuccs().empty()) {
      LogInfo::MapleLogger() << " succ [ ";
      for (auto *succ : bb->GetSuccs()) {
        LogInfo::MapleLogger() << std::hex << succ << std::dec << " ";
      }
      LogInfo::MapleLogger() << "]\n";
    }
    const StmtNode *stmt = bb->GetFirstStmt();
    if (stmt != nullptr) {
      bool done = false;
      do {
        done = stmt == bb->GetLastStmt();
        stmt->Dump(1);
        LogInfo::MapleLogger() << "\n";
        stmt = stmt->GetNext();
      } while (!done);
    } else {
      LogInfo::MapleLogger() << "<empty BB>\n";
    }
  }
}

void CGFunc::DumpCGIR() const {
  MIRSymbol *funcSt = GlobalTables::GetGsymTable().GetSymbolFromStidx(func.GetStIdx().Idx());
  LogInfo::MapleLogger() << "\n******  CGIR for " << funcSt->GetName() << " *******\n";
  FOR_ALL_BB_CONST(bb, this) {
    LogInfo::MapleLogger() << "=== BB " << " <" << bb->GetKindName();
    if (bb->GetLabIdx() != MIRLabelTable::GetDummyLabel()) {
      LogInfo::MapleLogger() << "[labeled with " << bb->GetLabIdx() << "]";
    }

    LogInfo::MapleLogger() << "> <" << bb->GetId() << "> ";
    if (bb->IsCleanup()) {
      LogInfo::MapleLogger() << "[is_cleanup] ";
    }
    if (bb->IsUnreachable()) {
      LogInfo::MapleLogger() << "[unreachable] ";
    }
    if (bb->GetFirstStmt() == cleanupLabel) {
      LogInfo::MapleLogger() << "cleanup ";
    }
    if (!bb->GetSuccs().empty()) {
      LogInfo::MapleLogger() << "succs: ";
      for (auto *succBB : bb->GetSuccs()) {
        LogInfo::MapleLogger() << succBB->GetId() << " ";
      }
    }
    if (!bb->GetEhSuccs().empty()) {
      LogInfo::MapleLogger() << "eh_succs: ";
      for (auto *ehSuccBB : bb->GetEhSuccs()) {
        LogInfo::MapleLogger() << ehSuccBB->GetId() << " ";
      }
    }
    LogInfo::MapleLogger() << "===\n";
    LogInfo::MapleLogger() << "frequency:" << bb->GetFrequency() << "\n";

    FOR_BB_INSNS_CONST(insn, bb) {
      insn->Dump();
    }
  }
}

void CGFunc::DumpLoop() const {
  for (const auto *lp : loops) {
    lp->PrintLoops(*lp);
  }
}

void CGFunc::ClearLoopInfo() {
  loops.clear();
  loops.shrink_to_fit();
  FOR_ALL_BB(bb, this) {
    bb->ClearLoopPreds();
    bb->ClearLoopSuccs();
  }
}

AnalysisResult *CgDoHandleFunc::Run(CGFunc *cgFunc, CgFuncResultMgr *cgFuncResultMgr) {
  (void)cgFuncResultMgr;
  ASSERT(cgFunc != nullptr, "Expect a cgfunc in CgDoHandleFunc");
  cgFunc->HandleFunction();
  if (!cgFunc->GetCG()->GetCGOptions().DoEmitCode() || cgFunc->GetCG()->GetCGOptions().DoDumpCFG()) {
    cgFunc->DumpCFG();
  }
  return nullptr;
}
}  /* namespace maplebe */
