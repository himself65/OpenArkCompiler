/*
 * Copyright (c) [2019] Huawei Technologies Co.,Ltd.All rights reserved.
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

  ConstantFold(MIRModule *mod, KlassHierarchy *kh, bool trace) : FuncOptimizeImpl(mod, kh, trace), mirModule(mod) {}

  ConstantFold(MIRModule *mod) : FuncOptimizeImpl(mod, nullptr, false), mirModule(mod) {}

  FuncOptimizeImpl *Clone() {
    return new ConstantFold(*this);
  }

  void ProcessFunc(MIRFunction *func);
  virtual ~ConstantFold() = default;

  MIRConst *FoldFloorMIRConst(MIRConst*, PrimType, PrimType);
  MIRConst *FoldRoundMIRConst(MIRConst*, PrimType, PrimType);
  MIRConst *FoldTypeCvtMIRConst(MIRConst*, PrimType, PrimType);
  MIRConst *FoldSignExtendMIRConst(Opcode, PrimType, uint8, MIRConst*);
  MIRConst *FoldConstComparisonMIRConst(Opcode, PrimType, PrimType, MIRConst&, MIRConst&);

 private:
  MIRModule *mirModule;
  StmtNode *SimplifyBinary(BinaryStmtNode *node);
  StmtNode *SimplifyBlock(BlockNode *node);
  StmtNode *SimplifyCondGoto(CondGotoNode *node);
  StmtNode *SimplifyCondGotoSelect(CondGotoNode *node);
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
  ConstvalNode *FoldSignExtend(Opcode opcode, PrimType resultType, uint8 size, ConstvalNode *cst);
  std::pair<BaseNode*, int64> FoldIread(IreadNode *node);
  std::pair<BaseNode*, int64> FoldSizeoftype(SizeoftypeNode *node);
  std::pair<BaseNode*, int64> FoldRetype(RetypeNode *node);
  std::pair<BaseNode*, int64> FoldGcmallocjarray(JarrayMallocNode *node);
  std::pair<BaseNode*, int64> FoldUnary(UnaryNode *node);
  std::pair<BaseNode*, int64> FoldTernary(TernaryNode *node);
  std::pair<BaseNode*, int64> FoldTypeCvt(TypeCvtNode *node);
  ConstvalNode *FoldCeil(ConstvalNode *cst, PrimType fromType, PrimType toType);
  ConstvalNode *FoldFloor(ConstvalNode *cst, PrimType fromType, PrimType toType);
  ConstvalNode *FoldRound(ConstvalNode *cst, PrimType fromType, PrimType toType);
  ConstvalNode *FoldTrunk(ConstvalNode *cst, PrimType fromType, PrimType toType);
  ConstvalNode *FoldTypeCvt(ConstvalNode *cst, PrimType fromType, PrimType toType);
  ConstvalNode *FoldConstComparison(Opcode opcode, PrimType resultType, PrimType opndType, ConstvalNode &const0,
                                    ConstvalNode &const1);
  ConstvalNode *FoldConstBinary(Opcode opcode, PrimType resultType, ConstvalNode &const0, ConstvalNode &const1);
  ConstvalNode *FoldIntConstComparison(Opcode opcode, PrimType resultType, ConstvalNode &const0, ConstvalNode &const1);
  MIRIntConst *FoldIntConstComparisonMIRConst(Opcode, PrimType, const MIRIntConst&, const MIRIntConst&);
  ConstvalNode *FoldIntConstBinary(Opcode opcode, PrimType resultType, ConstvalNode &const0, ConstvalNode &const1);
  ConstvalNode *FoldFPConstComparison(Opcode opcode, PrimType resultType, PrimType opndType, ConstvalNode &const0,
                                      ConstvalNode &const1);
  MIRIntConst *FoldFPConstComparisonMIRConst(Opcode opcode, PrimType resultType, PrimType opndType, MIRConst &const0,
                                             MIRConst &const1);
  ConstvalNode *FoldFPConstBinary(Opcode opcode, PrimType resultType, ConstvalNode &const0, ConstvalNode &const1);
  ConstvalNode *FoldConstUnary(Opcode opcode, PrimType resultType, ConstvalNode *c);
  ConstvalNode *FoldIntConstUnary(Opcode opcode, PrimType resultType, ConstvalNode *c);
  template <typename T>
  ConstvalNode *FoldFPConstUnary(Opcode opcode, PrimType resultType, ConstvalNode *c);
  BaseNode *NegateTree(BaseNode *node);
  BaseNode *Negate(BaseNode *node);
  BaseNode *Negate(UnaryNode *node);
  BaseNode *Negate(ConstvalNode *node);
  BinaryNode *NewBinaryNode(BinaryNode *old, Opcode op, PrimType primeType, BaseNode *l, BaseNode *r);
  UnaryNode *NewUnaryNode(UnaryNode *old, Opcode op, PrimType primeType, BaseNode *e);
  std::pair<BaseNode*, int64> DispatchFold(BaseNode *node);
  BaseNode *PairToExpr(PrimType resultType, const std::pair<BaseNode*, int64> &p);
  BaseNode *SimplifyDoubleCompare(CompareNode *node);
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
