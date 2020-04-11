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
#include "feir_builder.h"
#include "mpl_logging.h"
#include "global_tables.h"
#include "feir_var_reg.h"
#include "feir_var_name.h"
#include "fe_type_manager.h"

namespace maple {
UniqueFEIRType FEIRBuilder::CreateType(PrimType basePty, const GStrIdx &baseNameIdx, uint32 dim) {
  UniqueFEIRType type = std::make_unique<FEIRTypeDefault>(basePty, baseNameIdx, dim);
  CHECK_NULL_FATAL(type);
  return type;
}

UniqueFEIRType FEIRBuilder::CreateRefType(const GStrIdx &baseNameIdx, uint32 dim) {
  return CreateType(PTY_ref, baseNameIdx, dim);
}

UniqueFEIRType FEIRBuilder::CreateTypeByJavaName(const std::string &typeName, bool inMpl) {
  UniqueFEIRType type = std::make_unique<FEIRTypeDefault>(PTY_ref);
  CHECK_NULL_FATAL(type);
  FEIRTypeDefault *ptrType = static_cast<FEIRTypeDefault*>(type.get());
  ptrType->LoadFromJavaTypeName(typeName, inMpl);
  return type;
}

UniqueFEIRVar FEIRBuilder::CreateVarReg(uint32 regNum, PrimType primType, bool isGlobal) {
  UniqueFEIRVar var = std::make_unique<FEIRVarReg>(regNum, primType);
  CHECK_NULL_FATAL(var);
  var->SetGlobal(isGlobal);
  return var;
}

UniqueFEIRVar FEIRBuilder::CreateVarReg(uint32 regNum, UniqueFEIRType type, bool isGlobal) {
  UniqueFEIRVar var = std::make_unique<FEIRVarReg>(regNum, std::move(type));
  CHECK_NULL_FATAL(var);
  var->SetGlobal(isGlobal);
  return var;
}

UniqueFEIRVar FEIRBuilder::CreateVarName(GStrIdx nameIdx, PrimType primType, bool isGlobal, bool withType) {
  UniqueFEIRVar var = std::make_unique<FEIRVarName>(nameIdx, withType);
  CHECK_NULL_FATAL(var);
  var->GetType()->SetPrimType(primType);
  var->SetGlobal(isGlobal);
  return var;
}

UniqueFEIRVar FEIRBuilder::CreateVarName(const std::string &name, PrimType primType, bool isGlobal,
                                         bool withType) {
  GStrIdx nameIdx = GlobalTables::GetStrTable().GetStrIdxFromName(name);
  if (nameIdx == 0) {
    UniqueFEIRVar var = std::make_unique<FEIRVarNameSpec>(name, withType);
    var->GetType()->SetPrimType(primType);
    var->SetGlobal(isGlobal);
    return var;
  } else {
    return CreateVarName(nameIdx, primType, isGlobal, withType);
  }
}

UniqueFEIRExpr FEIRBuilder::CreateExprDRead(UniqueFEIRVar srcVar) {
  UniqueFEIRExpr expr = std::make_unique<FEIRExprDRead>(std::move(srcVar));
  CHECK_NULL_FATAL(expr);
  return expr;
}

UniqueFEIRExpr FEIRBuilder::CreateExprConstRefNull() {
  return std::make_unique<FEIRExprConst>(int64{ 0 }, PTY_ref);
}

UniqueFEIRExpr FEIRBuilder::CreateExprConstI32(int32 val) {
  return std::make_unique<FEIRExprConst>(int64{ val }, PTY_i32);
}

UniqueFEIRExpr FEIRBuilder::CreateExprConstI64(int64 val) {
  return std::make_unique<FEIRExprConst>(val, PTY_i64);
}

UniqueFEIRExpr FEIRBuilder::CreateExprConstF32(float val) {
  return std::make_unique<FEIRExprConst>(val);
}

UniqueFEIRExpr FEIRBuilder::CreateExprConstF64(double val) {
  return std::make_unique<FEIRExprConst>(val);
}

UniqueFEIRExpr FEIRBuilder::CreateExprMathUnary(Opcode op, UniqueFEIRVar var0) {
  UniqueFEIRExpr opnd0 = CreateExprDRead(std::move(var0));
  return std::make_unique<FEIRExprUnary>(op, std::move(opnd0));
}

UniqueFEIRExpr FEIRBuilder::CreateExprMathBinary(Opcode op, UniqueFEIRVar var0, UniqueFEIRVar var1) {
  UniqueFEIRExpr opnd0 = CreateExprDRead(std::move(var0));
  UniqueFEIRExpr opnd1 = CreateExprDRead(std::move(var1));
  return std::make_unique<FEIRExprBinary>(op, std::move(opnd0), std::move(opnd1));
}

UniqueFEIRExpr FEIRBuilder::CreateExprMathBinary(Opcode op, UniqueFEIRExpr expr0, UniqueFEIRExpr expr1) {
  return std::make_unique<FEIRExprBinary>(op, std::move(expr0), std::move(expr1));
}

UniqueFEIRExpr FEIRBuilder::CreateExprSExt(UniqueFEIRVar srcVar) {
  return std::make_unique<FEIRExprExtractBits>(OP_sext, PTY_i32,
      std::make_unique<FEIRExprDRead>(std::move(srcVar)));
}

UniqueFEIRExpr FEIRBuilder::CreateExprSExt(UniqueFEIRExpr srcExpr) {
  return std::make_unique<FEIRExprExtractBits>(OP_sext, PTY_i32, std::move(srcExpr));
}

UniqueFEIRExpr FEIRBuilder::CreateExprZExt(UniqueFEIRVar srcVar) {
  return std::make_unique<FEIRExprExtractBits>(OP_zext, PTY_i32,
      std::make_unique<FEIRExprDRead>(std::move(srcVar)));
}

UniqueFEIRExpr FEIRBuilder::CreateExprZExt(UniqueFEIRExpr srcExpr) {
  return std::make_unique<FEIRExprExtractBits>(OP_zext, PTY_i32, std::move(srcExpr));
}

UniqueFEIRExpr FEIRBuilder::CreateExprCvtPrim(UniqueFEIRVar srcVar, PrimType dstType) {
  return CreateExprCvtPrim(CreateExprDRead(std::move(srcVar)), dstType);
}

UniqueFEIRExpr FEIRBuilder::CreateExprCvtPrim(UniqueFEIRExpr srcExpr, PrimType dstType) {
  UniqueFEIRExpr expr = std::make_unique<FEIRExprTypeCvt>(OP_cvt, std::move(srcExpr));
  CHECK_NULL_FATAL(expr);
  FEIRExprTypeCvt *ptrExpr = static_cast<FEIRExprTypeCvt*>(expr.get());
  ptrExpr->GetType()->SetPrimType(dstType);
  return expr;
}

UniqueFEIRExpr FEIRBuilder::CreateExprJavaNewInstance(UniqueFEIRType type) {
  UniqueFEIRExpr expr = std::make_unique<FEIRExprJavaNewInstance>(std::move(type));
  CHECK_NULL_FATAL(expr);
  return expr;
}

UniqueFEIRExpr FEIRBuilder::CreateExprJavaNewArray(UniqueFEIRType type, UniqueFEIRExpr exprSize) {
  UniqueFEIRExpr expr = std::make_unique<FEIRExprJavaNewArray>(std::move(type), std::move(exprSize));
  CHECK_NULL_FATAL(expr);
  return expr;
}

UniqueFEIRExpr FEIRBuilder::CreateExprJavaArrayLength(UniqueFEIRExpr exprArray) {
  UniqueFEIRExpr expr = std::make_unique<FEIRExprJavaArrayLength>(std::move(exprArray));
  CHECK_NULL_FATAL(expr);
  return expr;
}

UniqueFEIRStmt FEIRBuilder::CreateStmtDAssign(UniqueFEIRVar dstVar, UniqueFEIRExpr srcExpr, bool hasException) {
  UniqueFEIRStmt stmt = std::make_unique<FEIRStmtDAssign>(std::move(dstVar), std::move(srcExpr));
  FEIRStmtDAssign *ptrStmt = static_cast<FEIRStmtDAssign*>(stmt.get());
  ptrStmt->SetHasException(hasException);
  return stmt;
}

UniqueFEIRStmt FEIRBuilder::CreateStmtGoto(uint32 targetLabelIdx) {
  UniqueFEIRStmt stmt = std::make_unique<FEIRStmtGoto>(targetLabelIdx);
  CHECK_NULL_FATAL(stmt);
  return stmt;
}

UniqueFEIRStmt FEIRBuilder::CreateStmtCondGoto(uint32 targetLabelIdx, Opcode op, UniqueFEIRExpr expr) {
  UniqueFEIRStmt stmt = std::make_unique<FEIRStmtCondGoto>(op, targetLabelIdx, std::move(expr));
  CHECK_NULL_FATAL(stmt);
  return stmt;
}

UniqueFEIRStmt FEIRBuilder::CreateStmtSwitch(UniqueFEIRExpr expr) {
  UniqueFEIRStmt stmt = std::make_unique<FEIRStmtSwitch>(std::move(expr));
  CHECK_NULL_FATAL(stmt);
  return stmt;
}

UniqueFEIRStmt FEIRBuilder::CreateStmtJavaConstClass(UniqueFEIRVar dstVar, UniqueFEIRType type) {
  UniqueFEIRType dstType = FETypeManager::kFEIRTypeJavaClass->Clone();
  dstVar->SetType(std::move(dstType));
  UniqueFEIRStmt stmt = std::make_unique<FEIRStmtJavaConstClass>(std::move(dstVar), std::move(type));
  return stmt;
}

UniqueFEIRStmt FEIRBuilder::CreateStmtJavaConstString(UniqueFEIRVar dstVar, const GStrIdx &strIdx) {
  UniqueFEIRType dstType = FETypeManager::kFEIRTypeJavaClass->Clone();
  dstVar->SetType(std::move(dstType));
  UniqueFEIRStmt stmt = std::make_unique<FEIRStmtJavaConstString>(std::move(dstVar), strIdx);
  return stmt;
}

UniqueFEIRStmt FEIRBuilder::CreateStmtJavaCheckCast(UniqueFEIRVar dstVar, UniqueFEIRVar srcVar, UniqueFEIRType type) {
  UniqueFEIRExpr expr = CreateExprDRead(std::move(srcVar));
  UniqueFEIRStmt stmt = std::make_unique<FEIRStmtJavaTypeCheck>(std::move(dstVar), std::move(expr), std::move(type),
                                                                FEIRStmtJavaTypeCheck::CheckKind::kCheckCast);
  return stmt;
}

UniqueFEIRStmt FEIRBuilder::CreateStmtJavaInstanceOf(UniqueFEIRVar dstVar, UniqueFEIRVar srcVar, UniqueFEIRType type) {
  UniqueFEIRExpr expr = CreateExprDRead(std::move(srcVar));
  UniqueFEIRStmt stmt = std::make_unique<FEIRStmtJavaTypeCheck>(std::move(dstVar), std::move(expr), std::move(type),
                                                                FEIRStmtJavaTypeCheck::CheckKind::kInstanceOf);
  return stmt;
}

std::list<UniqueFEIRStmt> FEIRBuilder::CreateStmtArrayStore(UniqueFEIRVar varElem, UniqueFEIRVar varArray,
                                                            UniqueFEIRVar varIndex, PrimType elemPrimType) {
  std::list<UniqueFEIRStmt> ans;
  UniqueFEIRExpr exprElem = CreateExprDRead(std::move(varElem));
  UniqueFEIRExpr exprArray = CreateExprDRead(std::move(varArray));
  UniqueFEIRExpr exprIndex = CreateExprDRead(std::move(varIndex));
  UniqueFEIRType arrayType = std::make_unique<FEIRTypeDefault>(elemPrimType);
  (void)arrayType->ArrayIncrDim();
  UniqueFEIRStmt stmt = std::make_unique<FEIRStmtArrayStore>(std::move(exprElem), std::move(exprArray),
                                                             std::move(exprIndex), std::move(arrayType));
  ans.push_back(std::move(stmt));
  return ans;
}

std::list<UniqueFEIRStmt> FEIRBuilder::CreateStmtArrayLoad(UniqueFEIRVar varElem, UniqueFEIRVar varArray,
                                                           UniqueFEIRVar varIndex, PrimType elemPrimType) {
  std::list<UniqueFEIRStmt> ans;
  UniqueFEIRExpr exprArray = CreateExprDRead(std::move(varArray));
  UniqueFEIRExpr exprIndex = CreateExprDRead(std::move(varIndex));
  UniqueFEIRType arrayType = std::make_unique<FEIRTypeDefault>(elemPrimType);
  (void)arrayType->ArrayIncrDim();
  UniqueFEIRExpr expr = std::make_unique<FEIRExprArrayLoad>(std::move(exprArray), std::move(exprIndex),
                                                            std::move(arrayType));
  UniqueFEIRStmt stmt = CreateStmtDAssign(std::move(varElem), std::move(expr), true);
  ans.push_back(std::move(stmt));
  return ans;
}

UniqueFEIRStmt FEIRBuilder::CreateStmtComment(const std::string &comment) {
  UniqueFEIRStmt stmt = std::make_unique<FEIRStmtPesudoComment>(comment);
  return stmt;
}
}  // namespace maple
