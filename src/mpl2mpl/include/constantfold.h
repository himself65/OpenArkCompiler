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
#ifndef MPL2MPL_INCLUDE_CONSTANTFOLD_H
#define MPL2MPL_INCLUDE_CONSTANTFOLD_H
#include "mir_nodes.h"
#include "module_phase.h"
#include "phase_impl.h"

namespace maple {
class ConstantFold : public FuncOptimizeImpl {
 public:
  ConstantFold(MIRModule &mod, KlassHierarchy *kh, bool trace) : FuncOptimizeImpl(mod, kh, trace), mirModule(&mod) {}

  explicit ConstantFold(MIRModule &mod) : FuncOptimizeImpl(mod, nullptr, false), mirModule(&mod) {}

  // Fold an expression.
  //
  // It returns a new expression if there was something to fold, or
  // nullptr otherwise.
  BaseNode *Fold(BaseNode *node);

  // Simplify a statement
  //
  // It returns the original statement or the changed statement if a
  // simplification happened. If the statement can be deleted after a
  // simplification, it returns nullptr.
  StmtNode *Simplify(StmtNode *node);

  FuncOptimizeImpl *Clone() {
    return new ConstantFold(*this);
  }

  void ProcessFunc(MIRFunction *func);
  virtual ~ConstantFold() = default;

  MIRConst *FoldFloorMIRConst(const MIRConst&, PrimType, PrimType) const;
  MIRConst *FoldRoundMIRConst(const MIRConst&, PrimType, PrimType) const;
  MIRConst *FoldTypeCvtMIRConst(const MIRConst&, PrimType, PrimType) const;
  MIRConst *FoldSignExtendMIRConst(Opcode, PrimType, uint8, const MIRConst&) const;
  MIRConst *FoldConstComparisonMIRConst(Opcode, PrimType, PrimType, const MIRConst&, const MIRConst&);

 private:
  StmtNode *SimplifyBinary(BinaryStmtNode *node);
  StmtNode *SimplifyBlock(BlockNode *node);
  StmtNode *SimplifyCondGoto(CondGotoNode *node);
  StmtNode *SimplifyCondGotoSelect(CondGotoNode *node) const;
  StmtNode *SimplifyDassign(DassignNode *node);
  StmtNode *SimplifyIassign(IassignNode *node);
  StmtNode *SimplifyNary(NaryStmtNode *node);
  StmtNode *SimplifyIcall(IcallNode *node);
  StmtNode *SimplifyIf(IfStmtNode *node);
  StmtNode *SimplifySwitch(SwitchNode *node);
  StmtNode *SimplifyUnary(UnaryStmtNode *node);
  StmtNode *SimplifyWhile(WhileStmtNode *node);
  std::pair<BaseNode*, int64> FoldArray(ArrayNode *node);
  std::pair<BaseNode*, int64> FoldBase(BaseNode *node) const;
  std::pair<BaseNode*, int64> FoldBinary(BinaryNode *node);
  std::pair<BaseNode*, int64> FoldCompare(CompareNode *node);
  std::pair<BaseNode*, int64> FoldDepositbits(DepositbitsNode *node);
  std::pair<BaseNode*, int64> FoldExtractbits(ExtractbitsNode *node);
  ConstvalNode *FoldSignExtend(Opcode opcode, PrimType resultType, uint8 size, const ConstvalNode &cst) const;
  std::pair<BaseNode*, int64> FoldIread(IreadNode *node);
  std::pair<BaseNode*, int64> FoldSizeoftype(SizeoftypeNode *node) const;
  std::pair<BaseNode*, int64> FoldRetype(RetypeNode *node);
  std::pair<BaseNode*, int64> FoldGcmallocjarray(JarrayMallocNode *node);
  std::pair<BaseNode*, int64> FoldUnary(UnaryNode *node);
  std::pair<BaseNode*, int64> FoldTernary(TernaryNode *node);
  std::pair<BaseNode*, int64> FoldTypeCvt(TypeCvtNode *node);
  ConstvalNode *FoldCeil(const ConstvalNode &cst, PrimType fromType, PrimType toType) const;
  ConstvalNode *FoldFloor(const ConstvalNode &cst, PrimType fromType, PrimType toType) const;
  ConstvalNode *FoldRound(const ConstvalNode &cst, PrimType fromType, PrimType toType) const;
  ConstvalNode *FoldTrunk(const ConstvalNode &cst, PrimType fromType, PrimType toType) const;
  ConstvalNode *FoldTypeCvt(const ConstvalNode &cst, PrimType fromType, PrimType toType) const;
  ConstvalNode *FoldConstComparison(Opcode opcode, PrimType resultType, PrimType opndType, const ConstvalNode &const0,
                                    const ConstvalNode &const1) const;
  ConstvalNode *FoldConstBinary(Opcode opcode, PrimType resultType, const ConstvalNode &const0,
                                const ConstvalNode &const1) const;
  ConstvalNode *FoldIntConstComparison(Opcode opcode, PrimType resultType, const ConstvalNode &const0,
                                       const ConstvalNode &const1) const;
  MIRIntConst *FoldIntConstComparisonMIRConst(Opcode, PrimType, const MIRIntConst&, const MIRIntConst&) const;
  ConstvalNode *FoldIntConstBinary(Opcode opcode, PrimType resultType, const ConstvalNode &const0,
                                   const ConstvalNode &const1) const;
  ConstvalNode *FoldFPConstComparison(Opcode opcode, PrimType resultType, PrimType opndType, const ConstvalNode &const0,
                                      const ConstvalNode &const1) const;
  MIRIntConst *FoldFPConstComparisonMIRConst(Opcode opcode, PrimType resultType, PrimType opndType,
                                             const MIRConst &const0, const MIRConst &const1) const;
  ConstvalNode *FoldFPConstBinary(Opcode opcode, PrimType resultType, const ConstvalNode &const0,
                                  const ConstvalNode &const1) const;
  ConstvalNode *FoldConstUnary(Opcode opcode, PrimType resultType, ConstvalNode *constNode) const;
  ConstvalNode *FoldIntConstUnary(Opcode opcode, PrimType resultType, const ConstvalNode *constNode) const;
  template <typename T>
  ConstvalNode *FoldFPConstUnary(Opcode opcode, PrimType resultType, ConstvalNode *constNode) const;
  BaseNode *NegateTree(BaseNode *node) const;
  BaseNode *Negate(BaseNode *node) const;
  BaseNode *Negate(UnaryNode *node) const;
  BaseNode *Negate(const ConstvalNode *node) const;
  BinaryNode *NewBinaryNode(BinaryNode *old, Opcode op, PrimType primeType, BaseNode *lhs, BaseNode *rhs) const;
  UnaryNode *NewUnaryNode(UnaryNode *old, Opcode op, PrimType primeType, BaseNode *expr) const;
  std::pair<BaseNode*, int64> DispatchFold(BaseNode *node);
  BaseNode *PairToExpr(PrimType resultType, const std::pair<BaseNode*, int64> &pair) const;
  BaseNode *SimplifyDoubleCompare(CompareNode &node) const;
  MIRModule *mirModule;
};

class DoConstantFold : public ModulePhase {
 public:
  explicit DoConstantFold(ModulePhaseID id) : ModulePhase(id) {}

  ~DoConstantFold() = default;

  std::string PhaseName() const override {
    return "ConstantFold";
  }

  AnalysisResult *Run(MIRModule *mod, ModuleResultMgr *mrm) override {
    OPT_TEMPLATE(ConstantFold);
    return nullptr;
  }
};
}  // namespace maple
#endif  // MPL2MPL_INCLUDE_CONSTANTFOLD_H
