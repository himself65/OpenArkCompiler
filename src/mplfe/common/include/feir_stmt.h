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
#ifndef MPLFE_INCLUDE_COMMON_FEIR_STMT_H
#define MPLFE_INCLUDE_COMMON_FEIR_STMT_H
#include <vector>
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <tuple>
#include "types_def.h"
#include "mempool_allocator.h"
#include "mir_builder.h"
#include "factory.h"
#include "safe_ptr.h"
#include "fe_utils.h"
#include "general_stmt.h"
#include "feir_var.h"

namespace maple {
class FEIRBuilder;

enum FEIRNodeKind : uint8 {
  kStmt,
  kStmtAssign,
  kStmtNonAssign,
  kStmtPesudo,
  kStmtDAssign,
  kStmtJavaTypeCheck,
  kStmtCallAssign,
  kStmtIntrinsicCallAssign,
  kStmtIAssign,
  kStmtUseOnly,
  kStmtReturn,
  kStmtBranch,
  kStmtGoto,
  kStmtCondGoto,
  kStmtSwitch,
  kStmtArrayStore,
  kExpr,
  kExprNestable,
  kExprNonNestable,
  kExprConst,
  kExprDRead,
  kExprConvert,
  kExprIntExt,
  kExprRetype,
  kExprCompare,
  kExprUnary,
  kExprBinary,
  kExprTernary,
  kExprNary,
  kExprArray,
  kExprIntrinsicop,
  kExprTypeCvt,
  kExprJavaNewInstance,
  kExprJavaNewArray,
  kExprJavaArrayLength,
  kExprJavaInstanceOf,
  kExprArrayLoad,
  kStmtPesudoFuncStart,
  kStmtPesudoFuncEnd,
  kStmtCheckPoint,
  kStmtPesudoLOC,
  kStmtPesudoLabel,
  kStmtPesudoJavaTry,
  kStmtPesudoEndTry,
  kStmtPesudoJavaCatch,
  kStmtPesudoComment,
  kStmtPesudoCommentForInst,
};

// ---------- FEIRNode ----------
class FEIRNode {
 public:
  explicit FEIRNode(FEIRNodeKind argKind)
      : kind(argKind) {}
  virtual ~FEIRNode() = default;

 protected:
  FEIRNodeKind kind;
};  // class FEIRNode

// ---------- FEIRDFGNode ----------
class FEIRDFGNode {
 public:
  explicit FEIRDFGNode(const UniqueFEIRVar &argVar)
      : var(argVar) {
    CHECK_NULL_FATAL(argVar);
  }

  virtual ~FEIRDFGNode() = default;
  bool operator==(const FEIRDFGNode &node) const {
    return var->EqualsTo(node.var);
  }

  size_t Hash() const {
    return var->Hash();
  }

  std::string GetNameRaw() const {
    return var->GetNameRaw();
  }

 private:
  const UniqueFEIRVar &var;
};

class FEIRDFGNodeHash {
 public:
  std::size_t operator()(const FEIRDFGNode &node) const {
    return node.Hash();
  }
};

using UniqueFEIRDFGNode = std::unique_ptr<FEIRDFGNode>;

// ---------- FEIRStmt ----------
class FEIRStmt : public GeneralStmt {
 public:
  explicit FEIRStmt(FEIRNodeKind argKind)
      : kind(argKind) {}

  FEIRStmt(GeneralStmtKind argGenKind, FEIRNodeKind argKind)
      : GeneralStmt(argGenKind),
        kind(argKind) {}

  virtual ~FEIRStmt() = default;
  std::list<StmtNode*> GenMIRStmts(MIRBuilder &mirBuilder) const {
    return GenMIRStmtsImpl(mirBuilder);
  }

  FEIRNodeKind GetKind() const {
    return kind;
  }

  void SetKind(FEIRNodeKind argKind) {
    kind = argKind;
  }

 protected:
  virtual std::list<StmtNode*> GenMIRStmtsImpl(MIRBuilder &mirBuilder) const;

  FEIRNodeKind kind;
};

using UniqueFEIRStmt = std::unique_ptr<FEIRStmt>;

// ---------- FEIRStmtCheckPoint ----------
class FEIRStmtCheckPoint : public FEIRStmt {
 public:
  FEIRStmtCheckPoint()
      : FEIRStmt(FEIRNodeKind::kStmtCheckPoint) {}
  ~FEIRStmtCheckPoint() = default;
  void Reset();
  void RegisterDFGNode(UniqueFEIRVar &var);
  void RegisterDFGNodes(const std::list<UniqueFEIRVar*> &vars);
  void AddPredCheckPoint(const UniqueFEIRStmt &stmtCheckPoint);
  std::set<UniqueFEIRVar*> &CalcuDef(const UniqueFEIRVar &use);

 private:
  void CalcuDefDFS(std::set<UniqueFEIRVar*> &result, const UniqueFEIRVar &use, const FEIRStmtCheckPoint &cp,
                   std::set<const FEIRStmtCheckPoint*> &visitSet) const;

  std::set<FEIRStmtCheckPoint*> predCPs;
  std::list<UniqueFEIRVar*> defs;
  std::list<UniqueFEIRVar*> uses;
  std::map<const UniqueFEIRVar*, std::set<UniqueFEIRVar*>> localUD;
  std::unordered_map<FEIRDFGNode, UniqueFEIRVar*, FEIRDFGNodeHash> lastDef;
  std::unordered_map<FEIRDFGNode, std::set<UniqueFEIRVar*>, FEIRDFGNodeHash> cacheUD;
};

// ---------- FEIRExpr ----------
class FEIRExpr {
 public:
  explicit FEIRExpr(FEIRNodeKind argKind);
  FEIRExpr(FEIRNodeKind argKind, std::unique_ptr<FEIRType> argType);
  virtual ~FEIRExpr() = default;
  FEIRExpr(const FEIRExpr&) = delete;
  FEIRExpr& operator=(const FEIRExpr&) = delete;
  std::unique_ptr<FEIRExpr> Clone() {
    return CloneImpl();
  }

  BaseNode *GenMIRNode(MIRBuilder &mirBuilder) const {
    return GenMIRNodeImpl(mirBuilder);
  }

  std::vector<FEIRVar*> GetVarUses() const {
    return GetVarUsesImpl();
  }

  bool IsNestable() const {
    return IsNestableImpl();
  }

  bool IsAddrof() const {
    return IsAddrofImpl();
  }

  bool HasException() const {
    return HasExceptionImpl();
  }

  void SetType(std::unique_ptr<FEIRType> argType) {
    CHECK_NULL_FATAL(argType);
    type = std::move(argType);
  }

  FEIRNodeKind GetKind() const {
    return kind;
  }

  FEIRType *GetType() const {
    ASSERT(type != nullptr, "type is nullptr");
    return type.get();
  }

  const FEIRType &GetTypeRef() const {
    ASSERT(type != nullptr, "type is nullptr");
    return *type.get();
  }

  PrimType GetPrimType() const {
    return type->GetPrimType();
  }

 protected:
  virtual std::unique_ptr<FEIRExpr> CloneImpl() const = 0;
  virtual BaseNode *GenMIRNodeImpl(MIRBuilder &mirBuilder) const = 0;
  virtual std::vector<FEIRVar*> GetVarUsesImpl() const;
  virtual bool IsNestableImpl() const;
  virtual bool IsAddrofImpl() const;
  virtual bool HasExceptionImpl() const;

  FEIRNodeKind kind;
  bool isNestable : 1;
  bool isAddrof : 1;
  bool hasException : 1;
  std::unique_ptr<FEIRType> type;
};  // class FEIRExpr

using UniqueFEIRExpr = std::unique_ptr<FEIRExpr>;

// ---------- FEIRExprConst ----------
class FEIRExprConst : public FEIRExpr {
 public:
  FEIRExprConst();
  FEIRExprConst(int64 val, PrimType argType);
  FEIRExprConst(uint64 val, PrimType argType);
  explicit FEIRExprConst(float val);
  explicit FEIRExprConst(double val);
  ~FEIRExprConst() = default;
  FEIRExprConst(const FEIRExprConst&) = delete;
  FEIRExprConst& operator=(const FEIRExprConst&) = delete;
  uint64 GetValueRaw() const {
    return value.raw;
  }

  void SetValueRaw(uint64 argValue) {
    value.raw = argValue;
  }

 protected:
  std::unique_ptr<FEIRExpr> CloneImpl() const override;
  BaseNode *GenMIRNodeImpl(MIRBuilder &mirBuilder) const override;

 private:
  union {
    int64 valueI64;
    uint64 valueU64;
    float valueF32;
    double valueF64;
    uint64 raw;
  } value;
};

// ---------- FEIRExprDRead ----------
class FEIRExprDRead : public FEIRExpr {
 public:
  explicit FEIRExprDRead(std::unique_ptr<FEIRVar> argVarSrc);
  FEIRExprDRead(std::unique_ptr<FEIRType> argType, std::unique_ptr<FEIRVar> argVarSrc);
  ~FEIRExprDRead() = default;
  void SetVarSrc(std::unique_ptr<FEIRVar> argVarSrc);

 protected:
  std::unique_ptr<FEIRExpr> CloneImpl() const override;
  BaseNode *GenMIRNodeImpl(MIRBuilder &mirBuilder) const override;
  std::vector<FEIRVar*> GetVarUsesImpl() const override;

 private:
  std::unique_ptr<FEIRVar> varSrc;
};

// ---------- FEIRExprUnary ----------
class FEIRExprUnary : public FEIRExpr {
 public:
  FEIRExprUnary(Opcode argOp, std::unique_ptr<FEIRExpr> argOpnd);
  FEIRExprUnary(std::unique_ptr<FEIRType> argType, Opcode argOp, std::unique_ptr<FEIRExpr> argOpnd);
  ~FEIRExprUnary() = default;
  void SetOpnd(std::unique_ptr<FEIRExpr> argOpnd);
  static std::map<Opcode, bool> InitMapOpNestableForExprUnary();

 protected:
  virtual std::unique_ptr<FEIRExpr> CloneImpl() const override;
  virtual BaseNode *GenMIRNodeImpl(MIRBuilder &mirBuilder) const override;
  std::vector<FEIRVar*> GetVarUsesImpl() const override;

  Opcode op;
  std::unique_ptr<FEIRExpr> opnd;

 private:
  void SetExprTypeByOp();

  static std::map<Opcode, bool> mapOpNestable;
};  // class FEIRExprUnary

// ---------- FEIRExprTypeCvt ----------
class FEIRExprTypeCvt : public FEIRExprUnary {
 public:
  FEIRExprTypeCvt(Opcode argOp, std::unique_ptr<FEIRExpr> argOpnd);
  FEIRExprTypeCvt(std::unique_ptr<FEIRType> exprType, Opcode argOp, std::unique_ptr<FEIRExpr> argOpnd);
  ~FEIRExprTypeCvt() = default;
  static std::map<Opcode, bool> InitMapOpNestableForTypeCvt();

 protected:
  std::unique_ptr<FEIRExpr> CloneImpl() const override;
  BaseNode *GenMIRNodeImpl(MIRBuilder &mirBuilder) const override;

 private:
  using FuncPtrGenMIRNode = BaseNode* (FEIRExprTypeCvt::*)(MIRBuilder &mirBuilder) const;
  static std::map<Opcode, FuncPtrGenMIRNode> InitFuncPtrMapForParseExpr();

  // GenMIRNodeMode1:
  //   MIR: op <to-type> <from-type> (<opnd0>)
  BaseNode *GenMIRNodeMode1(MIRBuilder &mirBuilder) const;

  // GenMIRNodeMode2:
  //   MIR: op <prim-type> <float-type> (<opnd0>)
  BaseNode *GenMIRNodeMode2(MIRBuilder &mirBuilder) const;

  // GenMIRNodeMode3:
  //   MIR: retype <prim-type> <type> (<opnd0>)
  BaseNode *GenMIRNodeMode3(MIRBuilder &mirBuilder) const;

  static std::map<Opcode, bool> mapOpNestable;
  static std::map<Opcode, FuncPtrGenMIRNode> funcPtrMapForParseExpr;
};  // FEIRExprTypeCvt

// ---------- FEIRExprExtractBits ----------
class FEIRExprExtractBits : public FEIRExprUnary {
 public:
  FEIRExprExtractBits(Opcode argOp, PrimType argPrimType, uint8 argBitOffset, uint8 argBitSize,
                      std::unique_ptr<FEIRExpr> argOpnd);
  FEIRExprExtractBits(Opcode argOp, PrimType argPrimType, std::unique_ptr<FEIRExpr> argOpnd);
  ~FEIRExprExtractBits() = default;
  static std::map<Opcode, bool> InitMapOpNestableForExtractBits();
  void SetBitOffset(uint8 offset) {
    bitOffset = offset;
  }

  void SetBitSize(uint8 size) {
    bitSize = size;
  }

 protected:
  std::unique_ptr<FEIRExpr> CloneImpl() const override;
  BaseNode *GenMIRNodeImpl(MIRBuilder &mirBuilder) const override;

 private:
  using FuncPtrGenMIRNode = BaseNode* (FEIRExprExtractBits::*)(MIRBuilder &mirBuilder) const;
  static std::map<Opcode, FuncPtrGenMIRNode> InitFuncPtrMapForParseExpr();
  BaseNode *GenMIRNodeForExtrabits(MIRBuilder &mirBuilder) const;
  BaseNode *GenMIRNodeForExt(MIRBuilder &mirBuilder) const;

  uint8 bitOffset;
  uint8 bitSize;
  static std::map<Opcode, bool> mapOpNestable;
  static std::map<Opcode, FuncPtrGenMIRNode> funcPtrMapForParseExpr;
};  // FEIRExprExtractBit

// ---------- FEIRExprIRead ----------
class FEIRExprIRead : public FEIRExprUnary {
 public:
  FEIRExprIRead(Opcode op, std::unique_ptr<FEIRExpr> argOpnd);
  ~FEIRExprIRead() = default;

 protected:
  std::unique_ptr<FEIRExpr> CloneImpl() const override;
  BaseNode *GenMIRNodeImpl(MIRBuilder &mirBuilder) const override;

 private:
  uint32 offset = 0;
};

// ---------- FEIRExprBinary ----------
class FEIRExprBinary : public FEIRExpr {
 public:
  FEIRExprBinary(Opcode argOp, std::unique_ptr<FEIRExpr> argOpnd0, std::unique_ptr<FEIRExpr> argOpnd1);
  FEIRExprBinary(std::unique_ptr<FEIRType> exprType, Opcode argOp, std::unique_ptr<FEIRExpr> argOpnd0,
                 std::unique_ptr<FEIRExpr> argOpnd1);
  ~FEIRExprBinary() = default;
  void SetOpnd0(std::unique_ptr<FEIRExpr> argOpnd);
  void SetOpnd1(std::unique_ptr<FEIRExpr> argOpnd);

 protected:
  std::unique_ptr<FEIRExpr> CloneImpl() const override;
  BaseNode *GenMIRNodeImpl(MIRBuilder &mirBuilder) const override;
  std::vector<FEIRVar*> GetVarUsesImpl() const override;
  bool IsNestableImpl() const override;
  bool IsAddrofImpl() const override;

 private:
  using FuncPtrGenMIRNode = BaseNode* (FEIRExprBinary::*)(MIRBuilder &mirBuilder) const;
  static std::map<Opcode, FuncPtrGenMIRNode> InitFuncPtrMapForGenMIRNode();
  BaseNode *GenMIRNodeNormal(MIRBuilder &mirBuilder) const;
  BaseNode *GenMIRNodeCompare(MIRBuilder &mirBuilder) const;
  BaseNode *GenMIRNodeCompareU1(MIRBuilder &mirBuilder) const;
  void SetExprTypeByOp();
  void SetExprTypeByOpNormal();
  void SetExprTypeByOpShift();
  void SetExprTypeByOpLogic();
  void SetExprTypeByOpCompare();

  Opcode op;
  std::unique_ptr<FEIRExpr> opnd0;
  std::unique_ptr<FEIRExpr> opnd1;
  static std::map<Opcode, FuncPtrGenMIRNode> funcPtrMapForGenMIRNode;
};  // class FEIRExprUnary

// ---------- FEIRExprTernary ----------
class FEIRExprTernary : public FEIRExpr {
 public:
  FEIRExprTernary(Opcode argOp, std::unique_ptr<FEIRExpr> argOpnd0, std::unique_ptr<FEIRExpr> argOpnd1,
                  std::unique_ptr<FEIRExpr> argOpnd2);
  ~FEIRExprTernary() = default;
  void SetOpnd(std::unique_ptr<FEIRExpr> argOpnd, uint32 idx);

 protected:
  std::unique_ptr<FEIRExpr> CloneImpl() const override;
  BaseNode *GenMIRNodeImpl(MIRBuilder &mirBuilder) const override;
  std::vector<FEIRVar*> GetVarUsesImpl() const override;
  bool IsNestableImpl() const override;
  bool IsAddrofImpl() const override;

 private:
  void SetExprTypeByOp();

  Opcode op;
  std::unique_ptr<FEIRExpr> opnd0;
  std::unique_ptr<FEIRExpr> opnd1;
  std::unique_ptr<FEIRExpr> opnd2;
};

// ---------- FEIRExprNary ----------
class FEIRExprNary : public FEIRExpr {
 public:
  explicit FEIRExprNary(Opcode argOp);
  ~FEIRExprNary() = default;
  void AddOpnd(std::unique_ptr<FEIRExpr> argOpnd);
  void AddOpnds(const std::vector<std::unique_ptr<FEIRExpr>> &argOpnds);
  void ResetOpnd();

 protected:
  std::vector<FEIRVar*> GetVarUsesImpl() const override;

  Opcode op;
  std::vector<std::unique_ptr<FEIRExpr>> opnds;
};  // class FEIRExprNary

// ---------- FEIRExprArray ----------
class FEIRExprArray : public FEIRExprNary {
 public:
  FEIRExprArray(Opcode argOp, std::unique_ptr<FEIRExpr> argArray, std::unique_ptr<FEIRExpr> argIndex);
  ~FEIRExprArray() = default;
  void SetOpndArray(std::unique_ptr<FEIRExpr> opndArray);
  void SetOpndIndex(std::unique_ptr<FEIRExpr> opndIndex);

 protected:
  std::unique_ptr<FEIRExpr> CloneImpl() const override;
  BaseNode *GenMIRNodeImpl(MIRBuilder &mirBuilder) const override;
  bool IsNestableImpl() const override;
  bool IsAddrofImpl() const override;
};  // class FEIRExprArray

// ---------- FEIRExprIntrinsicop ----------
class FEIRExprIntrinsicop : public FEIRExprNary {
 public:
  FEIRExprIntrinsicop(std::unique_ptr<FEIRType> exprType, MIRIntrinsicID argIntrinsicID);
  FEIRExprIntrinsicop(std::unique_ptr<FEIRType> exprType, MIRIntrinsicID argIntrinsicID,
                      std::unique_ptr<FEIRType> argParamType);
  FEIRExprIntrinsicop(std::unique_ptr<FEIRType> exprType, MIRIntrinsicID argIntrinsicID,
                      const std::vector<std::unique_ptr<FEIRExpr>> &argOpnds);
  FEIRExprIntrinsicop(std::unique_ptr<FEIRType> exprType, MIRIntrinsicID argIntrinsicID,
                      std::unique_ptr<FEIRType> argParamType,
                      const std::vector<std::unique_ptr<FEIRExpr>> &argOpnds);
  ~FEIRExprIntrinsicop() = default;

 protected:
  std::unique_ptr<FEIRExpr> CloneImpl() const override;
  BaseNode *GenMIRNodeImpl(MIRBuilder &mirBuilder) const override;
  bool IsNestableImpl() const override;
  bool IsAddrofImpl() const override;

 private:
  MIRIntrinsicID intrinsicID;
  std::unique_ptr<FEIRType> paramType;
};  // class FEIRExprIntrinsicop

// ---------- FEIRExprJavaNewInstance ----------
class FEIRExprJavaNewInstance : public FEIRExpr {
 public:
  explicit FEIRExprJavaNewInstance(UniqueFEIRType argType);
  ~FEIRExprJavaNewInstance() = default;

 protected:
  std::unique_ptr<FEIRExpr> CloneImpl() const override;
  BaseNode *GenMIRNodeImpl(MIRBuilder &mirBuilder) const override;
};

// ---------- FEIRExprJavaNewArray ----------
class FEIRExprJavaNewArray : public FEIRExpr {
 public:
  FEIRExprJavaNewArray(UniqueFEIRType argArrayType, UniqueFEIRExpr argExprSize);
  ~FEIRExprJavaNewArray() = default;
  void SetArrayType(UniqueFEIRType argArrayType) {
    CHECK_NULL_FATAL(argArrayType);
    arrayType = std::move(argArrayType);
  }

  void SetExprSize(UniqueFEIRExpr argExprSize) {
    CHECK_NULL_FATAL(argExprSize);
    exprSize = std::move(argExprSize);
  }

 protected:
  std::unique_ptr<FEIRExpr> CloneImpl() const override;
  BaseNode *GenMIRNodeImpl(MIRBuilder &mirBuilder) const override;

 private:
  UniqueFEIRType arrayType;
  UniqueFEIRExpr exprSize;
};

// ---------- FEIRExprJavaArrayLength ----------
class FEIRExprJavaArrayLength : public FEIRExpr {
 public:
  FEIRExprJavaArrayLength(UniqueFEIRExpr argExprArray);
  ~FEIRExprJavaArrayLength() = default;
  void SetExprArray(UniqueFEIRExpr argExprArray) {
    CHECK_NULL_FATAL(argExprArray);
    exprArray = std::move(argExprArray);
  }

 protected:
  std::unique_ptr<FEIRExpr> CloneImpl() const override;
  BaseNode *GenMIRNodeImpl(MIRBuilder &mirBuilder) const override;

 private:
  UniqueFEIRExpr exprArray;
};

// ---------- FEIRExprArrayLoad ----------
class FEIRExprArrayLoad : public FEIRExpr {
 public:
  FEIRExprArrayLoad(UniqueFEIRExpr argExprArray, UniqueFEIRExpr argExprIndex, UniqueFEIRType argTypeArray);
  ~FEIRExprArrayLoad() = default;
  const UniqueFEIRType GetElemType() const {
    UniqueFEIRType typeElem = typeArray->Clone();
    (void)typeElem->ArrayDecrDim();
    return typeElem;
  }

 protected:
  std::unique_ptr<FEIRExpr> CloneImpl() const override;
  BaseNode *GenMIRNodeImpl(MIRBuilder &mirBuilder) const override;

 private:
  UniqueFEIRExpr exprArray;
  UniqueFEIRExpr exprIndex;
  UniqueFEIRType typeArray;
};

// ---------- FEIRStmtAssign ----------
class FEIRStmtAssign : public FEIRStmt {
 public:
  FEIRStmtAssign(FEIRNodeKind argKind, std::unique_ptr<FEIRVar> argVar);
  ~FEIRStmtAssign() = default;
  FEIRVar *GetVar() const {
    return var.get();
  }

  void SetVar(std::unique_ptr<FEIRVar> argVar) {
    CHECK_FATAL(argVar != nullptr, "input var is nullptr");
    var = std::move(argVar);
  }

  bool HasException() const {
    return hasException;
  }

  void SetHasException(bool arg) {
    hasException = arg;
  }

 protected:
  bool hasException;
  std::unique_ptr<FEIRVar> var;
};

// ---------- FEIRStmtDAssign ----------
class FEIRStmtDAssign : public FEIRStmtAssign {
 public:
  FEIRStmtDAssign(std::unique_ptr<FEIRVar> argVar, std::unique_ptr<FEIRExpr> argExpr, int32 argFieldID = 0);
  ~FEIRStmtDAssign() = default;
  FEIRExpr *GetExpr() const {
    return expr.get();
  }

  void SetExpr(std::unique_ptr<FEIRExpr> argExpr) {
    expr = std::move(argExpr);
  }

 protected:
  std::list<StmtNode*> GenMIRStmtsImpl(MIRBuilder &mirBuilder) const override;
  std::unique_ptr<FEIRExpr> expr;
  int32 fieldID;
};

// ---------- FEIRStmtJavaTypeCheck ----------
class FEIRStmtJavaTypeCheck : public FEIRStmtAssign {
 public:
  enum CheckKind {
    kCheckCast,
    kInstanceOf
  };

  FEIRStmtJavaTypeCheck(std::unique_ptr<FEIRVar> argVar, std::unique_ptr<FEIRExpr> argExpr,
                        std::unique_ptr<FEIRType> argType, CheckKind argCheckKind);
  ~FEIRStmtJavaTypeCheck() = default;

 protected:
  std::list<StmtNode*> GenMIRStmtsImpl(MIRBuilder &mirBuilder) const override;
  CheckKind checkKind;
  std::unique_ptr<FEIRExpr> expr;
  std::unique_ptr<FEIRType> type;
};

// ---------- FEIRStmtUseOnly ----------
class FEIRStmtUseOnly : public FEIRStmt {
 public:
  FEIRStmtUseOnly(FEIRNodeKind argKind, Opcode argOp, std::unique_ptr<FEIRExpr> argExpr);
  FEIRStmtUseOnly(Opcode argOp, std::unique_ptr<FEIRExpr> argExpr);
  ~FEIRStmtUseOnly() = default;

 protected:
  std::list<StmtNode*> GenMIRStmtsImpl(MIRBuilder &mirBuilder) const override;

  Opcode op;
  std::unique_ptr<FEIRExpr> expr;
};

// ---------- FEIRStmtReturn ----------
class FEIRStmtReturn : public FEIRStmtUseOnly {
 public:
  explicit FEIRStmtReturn(std::unique_ptr<FEIRExpr> argExpr);
  ~FEIRStmtReturn() = default;

 protected:
  std::list<StmtNode*> GenMIRStmtsImpl(MIRBuilder &mirBuilder) const override;
};

// ---------- FEIRStmtPesudoLabel ----------
class FEIRStmtPesudoLabel : public FEIRStmt {
 public:
  FEIRStmtPesudoLabel(uint32 argLabelIdx);
  ~FEIRStmtPesudoLabel() = default;
  void GenerateLabelIdx(MIRBuilder &mirBuilder);

  uint32 GetLabelIdx() const {
    return labelIdx;
  }

  LabelIdx GetMIRLabelIdx() const {
    return mirLabelIdx;
  }

 protected:
  std::list<StmtNode*> GenMIRStmtsImpl(MIRBuilder &mirBuilder) const override;

  uint32 labelIdx;
  LabelIdx mirLabelIdx;
};

// ---------- FEIRStmtGoto ----------
class FEIRStmtGoto : public FEIRStmt {
 public:
  explicit FEIRStmtGoto(uint32 argLabelIdx);
  virtual ~FEIRStmtGoto();
  void SetLabelIdx(uint32 argLabelIdx) {
    labelIdx = argLabelIdx;
  }

  uint32 GetLabelIdx() const {
    return labelIdx;
  }

  void SetStmtTarget(FEIRStmtPesudoLabel *argStmtTarget) {
    stmtTarget = argStmtTarget;
  }

  const FEIRStmtPesudoLabel &GetStmtTargetRef() const {
    CHECK_NULL_FATAL(stmtTarget);
    return *stmtTarget;
  }

 protected:
  std::list<StmtNode*> GenMIRStmtsImpl(MIRBuilder &mirBuilder) const override;

  uint32 labelIdx;
  FEIRStmtPesudoLabel *stmtTarget;
};

// ---------- FEIRStmtCondGoto ----------
class FEIRStmtCondGoto : public FEIRStmtGoto {
 public:
  FEIRStmtCondGoto(Opcode argOp, uint32 argLabelIdx, UniqueFEIRExpr argExpr);
  ~FEIRStmtCondGoto() = default;
  void SetOpcode(Opcode argOp) {
    op = argOp;
  }

  Opcode GetOpcode() const {
    return op;
  }

  void SetExpr(UniqueFEIRExpr argExpr) {
    CHECK_NULL_FATAL(argExpr);
    expr = std::move(argExpr);
  }

 protected:
  std::list<StmtNode*> GenMIRStmtsImpl(MIRBuilder &mirBuilder) const override;

 private:
  Opcode op;
  UniqueFEIRExpr expr;
};

// ---------- FEIRStmtSwitch ----------
class FEIRStmtSwitch : public FEIRStmt {
 public:
  explicit FEIRStmtSwitch(UniqueFEIRExpr argExpr);
  ~FEIRStmtSwitch();
  void SetDefaultLabelIdx(uint32 labelIdx) {
    defaultLabelIdx = labelIdx;
  }

  uint32 GetDefaultLabelIdx() const {
    return defaultLabelIdx;
  }

  void SetDefaultTarget(FEIRStmtPesudoLabel *stmtTarget) {
    defaultTarget = stmtTarget;
  }

  const std::map<int32, uint32> &GetMapValueLabelIdx() const {
    return mapValueLabelIdx;
  }

  void AddTarget(int32 value, uint32 labelIdx) {
    mapValueLabelIdx[value] = labelIdx;
  }

  void AddTarget(int32 value, FEIRStmtPesudoLabel *target) {
    mapValueTargets[value] = target;
  }

  void SetExpr(UniqueFEIRExpr argExpr) {
    CHECK_NULL_FATAL(argExpr);
    expr = std::move(argExpr);
  }

 protected:
  std::list<StmtNode*> GenMIRStmtsImpl(MIRBuilder &mirBuilder) const override;

 private:
  uint32 defaultLabelIdx;
  FEIRStmtPesudoLabel *defaultTarget;
  std::map<int32, uint32> mapValueLabelIdx;
  std::map<int32, FEIRStmtPesudoLabel*> mapValueTargets;
  UniqueFEIRExpr expr;
};

// ---------- FEIRStmtArrayStore ----------
class FEIRStmtArrayStore : public FEIRStmt {
 public:
  FEIRStmtArrayStore(UniqueFEIRExpr argExprElem, UniqueFEIRExpr argExprArray, UniqueFEIRExpr argExprIndex,
                     UniqueFEIRType argTypeArray);
  ~FEIRStmtArrayStore() = default;

 protected:
  std::list<StmtNode*> GenMIRStmtsImpl(MIRBuilder &mirBuilder) const override;

 private:
  UniqueFEIRExpr exprElem;
  UniqueFEIRExpr exprArray;
  UniqueFEIRExpr exprIndex;
  UniqueFEIRType typeArray;
};

// ---------- FEIRStmtPesudoLOC ----------
class FEIRStmtPesudoLOC : public FEIRStmt {
 public:
  FEIRStmtPesudoLOC(uint32 argSrcFileIdx, uint32 argLineNumber);
  ~FEIRStmtPesudoLOC() = default;
  uint32 GetSrcFileIdx() const {
    return srcFileIdx;
  }

  uint32 GetLineNumber() const {
    return lineNumber;
  }

 protected:
  std::list<StmtNode*> GenMIRStmtsImpl(MIRBuilder &mirBuilder) const override;

 private:
  uint32 srcFileIdx;
  uint32 lineNumber;
};

// ---------- FEIRStmtPesudoJavaTry ----------
class FEIRStmtPesudoJavaTry : public FEIRStmt {
 public:
  FEIRStmtPesudoJavaTry();
  ~FEIRStmtPesudoJavaTry() = default;
  void AddCatchLabelIdx(uint32 labelIdx) {
    catchLabelIdxVec.push_back(labelIdx);
  }

  const std::vector<uint32> GetCatchLabelIdxVec() const {
    return catchLabelIdxVec;
  }

  void AddCatchTarget(FEIRStmtPesudoLabel *stmtLabel) {
    catchTargets.push_back(stmtLabel);
  }

 protected:
  std::list<StmtNode*> GenMIRStmtsImpl(MIRBuilder &mirBuilder) const override;

 private:
  std::vector<uint32> catchLabelIdxVec;
  std::vector<FEIRStmtPesudoLabel*> catchTargets;
};

// ---------- FEIRStmtPesudoEndTry ----------
class FEIRStmtPesudoEndTry : public FEIRStmt {
 public:
  FEIRStmtPesudoEndTry();
  ~FEIRStmtPesudoEndTry() = default;

 protected:
  std::list<StmtNode*> GenMIRStmtsImpl(MIRBuilder &mirBuilder) const override;
};

// ---------- FEIRStmtPesudoCatch ----------
class FEIRStmtPesudoCatch : public FEIRStmtPesudoLabel {
 public:
  explicit FEIRStmtPesudoCatch(uint32 argLabelIdx);
  ~FEIRStmtPesudoCatch() = default;
  void AddCatchTypeNameIdx(GStrIdx typeNameIdx);

 protected:
  std::list<StmtNode*> GenMIRStmtsImpl(MIRBuilder &mirBuilder) const override;

 private:
  std::list<UniqueFEIRType> catchTypes;
};

// ---------- FEIRStmtPesudoComment ----------
class FEIRStmtPesudoComment : public FEIRStmt {
 public:
  explicit FEIRStmtPesudoComment(FEIRNodeKind argKind = kStmtPesudoComment);
  explicit FEIRStmtPesudoComment(const std::string &argContent);
  ~FEIRStmtPesudoComment() = default;
  void SetContent(const std::string &argContent) {
    content = argContent;
  }

 protected:
  std::list<StmtNode*> GenMIRStmtsImpl(MIRBuilder &mirBuilder) const override;

  std::string content = "";
};

// ---------- FEIRStmtPesudoCommentForInst ----------
class FEIRStmtPesudoCommentForInst : public FEIRStmtPesudoComment {
 public:
  FEIRStmtPesudoCommentForInst();
  ~FEIRStmtPesudoCommentForInst() = default;
  void SetFileIdx(uint32 argFileIdx) {
    fileIdx = argFileIdx;
  }

  void SetLineNum(uint32 argLineNum) {
    lineNum = argLineNum;
  }

  void SetPC(uint32 argPC) {
    pc = argPC;
  }

 protected:
  std::list<StmtNode*> GenMIRStmtsImpl(MIRBuilder &mirBuilder) const override;

 private:
  constexpr static uint32 invalid = 0xFFFFFFFF;
  uint32 fileIdx = invalid;
  uint32 lineNum = invalid;
  uint32 pc = invalid;
};
}  // namespace maple
#endif  // MPLFE_INCLUDE_COMMON_FEIR_STMT_H
