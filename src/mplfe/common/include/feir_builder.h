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
#ifndef MPLFE_INCLUDE_COMMON_FEIR_BUILDER_H
#define MPLFE_INCLUDE_COMMON_FEIR_BUILDER_H
#include <memory>
#include "mir_function.h"
#include "mpl_logging.h"
#include "feir_var.h"
#include "feir_stmt.h"

namespace maple {
class FEIRBuilder {
 public:
  FEIRBuilder() = default;
  ~FEIRBuilder() = default;
  // Type
  static UniqueFEIRType CreateType(PrimType basePty, const GStrIdx &baseNameIdx, uint32 dim);
  static UniqueFEIRType CreateRefType(const GStrIdx &baseNameIdx, uint32 dim);
  static UniqueFEIRType CreateTypeByJavaName(const std::string &typeName, bool inMpl);
  // Var
  static UniqueFEIRVar CreateVarReg(uint32 regNum, PrimType primType, bool isGlobal = false);
  static UniqueFEIRVar CreateVarReg(uint32 regNum, UniqueFEIRType type, bool isGlobal = false);
  static UniqueFEIRVar CreateVarName(GStrIdx nameIdx, PrimType primType, bool isGlobal = false,
                                     bool withType = false);
  static UniqueFEIRVar CreateVarName(const std::string &name, PrimType primType, bool isGlobal = false,
                                     bool withType = false);
  // Expr
  static UniqueFEIRExpr CreateExprDRead(UniqueFEIRVar srcVar);
  static UniqueFEIRExpr CreateExprConstRefNull();
  static UniqueFEIRExpr CreateExprConstI32(int32 val);
  static UniqueFEIRExpr CreateExprConstI64(int64 val);
  static UniqueFEIRExpr CreateExprConstF32(float val);
  static UniqueFEIRExpr CreateExprConstF64(double val);
  static UniqueFEIRExpr CreateExprMathUnary(Opcode op, UniqueFEIRVar var0);
  static UniqueFEIRExpr CreateExprMathBinary(Opcode op, UniqueFEIRVar var0, UniqueFEIRVar var1);
  static UniqueFEIRExpr CreateExprMathBinary(Opcode op, UniqueFEIRExpr expr0, UniqueFEIRExpr expr1);
  static UniqueFEIRExpr CreateExprSExt(UniqueFEIRVar srcVar);
  static UniqueFEIRExpr CreateExprSExt(UniqueFEIRExpr srcExpr);
  static UniqueFEIRExpr CreateExprZExt(UniqueFEIRVar srcVar);
  static UniqueFEIRExpr CreateExprZExt(UniqueFEIRExpr srcExpr);
  static UniqueFEIRExpr CreateExprCvtPrim(UniqueFEIRVar srcVar, PrimType dstType);
  static UniqueFEIRExpr CreateExprCvtPrim(UniqueFEIRExpr srcExpr, PrimType dstType);
  static UniqueFEIRExpr CreateExprJavaNewInstance(UniqueFEIRType type);
  static UniqueFEIRExpr CreateExprJavaNewArray(UniqueFEIRType type, UniqueFEIRExpr exprSize);
  static UniqueFEIRExpr CreateExprJavaArrayLength(UniqueFEIRExpr exprArray);
  // Stmt
  static UniqueFEIRStmt CreateStmtDAssign(UniqueFEIRVar dstVar, UniqueFEIRExpr srcExpr, bool hasException = false);
  static UniqueFEIRStmt CreateStmtGoto(uint32 targetLabelIdx);
  static UniqueFEIRStmt CreateStmtCondGoto(uint32 targetLabelIdx, Opcode op, UniqueFEIRExpr expr);
  static UniqueFEIRStmt CreateStmtSwitch(UniqueFEIRExpr expr);
  static UniqueFEIRStmt CreateStmtJavaConstClass(UniqueFEIRVar dstVar, UniqueFEIRType type);
  static UniqueFEIRStmt CreateStmtJavaConstString(UniqueFEIRVar dstVar, const GStrIdx &strIdx);
  static UniqueFEIRStmt CreateStmtJavaCheckCast(UniqueFEIRVar dstVar, UniqueFEIRVar srcVar, UniqueFEIRType type);
  static UniqueFEIRStmt CreateStmtJavaInstanceOf(UniqueFEIRVar dstVar, UniqueFEIRVar srcVar, UniqueFEIRType type);
  static std::list<UniqueFEIRStmt> CreateStmtArrayStore(UniqueFEIRVar varElem, UniqueFEIRVar varArray,
                                                        UniqueFEIRVar varIndex, PrimType elemPrimType);
  static std::list<UniqueFEIRStmt> CreateStmtArrayLoad(UniqueFEIRVar varElem, UniqueFEIRVar varArray,
                                                       UniqueFEIRVar varIndex, PrimType elemPrimType);
  static UniqueFEIRStmt CreateStmtComment(const std::string &comment);
};  // class FEIRBuilder
}  // namespace maple
#endif  // MPLFE_INCLUDE_COMMON_FEIR_BUILDER_H