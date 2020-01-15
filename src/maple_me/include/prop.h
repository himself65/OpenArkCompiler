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
#ifndef MAPLE_ME_INCLUDE_PROP_H
#define MAPLE_ME_INCLUDE_PROP_H
#include <stack>
#include "me_ir.h"
#include "ver_symbol.h"
#include "bb.h"
#include "me_function.h"
#include "dominance.h"
#include "irmap.h"
#include "safe_ptr.h"

namespace maple {
class Prop {
 public:
  struct PropConfig {
    bool propagateBase;
    bool propagateIloadRef;
    bool propagateGlobalRef;
    bool propagateFinalIloadRef;
    bool propagateIloadRefNonParm;
    bool propagateAtPhi;
  };

  Prop(IRMap&, Dominance&, MemPool&, std::vector<BB*> &&bbVec, BB &commonEntryBB, const PropConfig &config);
  virtual ~Prop() = default;

  void DoProp();

 protected:
  Dominance &dom;

  virtual void UpdateCurFunction(BB &bb) {}

  virtual bool LocalToDifferentPU(StIdx stidx, const BB &bb) const {
    return false;
  }

 private:
  using SafeMeExprPtr = utils::SafePtr<MeExpr>;
  void TraversalBB(BB &bb);
  void PropUpdateDef(MeExpr &meExpr);
  void TraversalMeStmt(MeStmt &meStmt);
  void PropUpdateChiListDef(const MapleMap<OStIdx, ChiMeNode*> &chiList);
  void CollectSubVarMeExpr(const MeExpr &expr, std::vector<const MeExpr*> &exprVec) const;
  bool IsVersionConsistent(const std::vector<const MeExpr*> &vstVec,
                           const std::vector<std::stack<SafeMeExprPtr>> &vstLiveStack) const;
  bool IvarIsFinalField(const IvarMeExpr &ivarMeExpr) const;
  bool Propagatable(const MeExpr &expr, const BB &frombb, bool atParm) const;
  MeExpr &PropVar(VarMeExpr &varmeExpr, bool atParm, bool checkPhi);
  MeExpr &PropReg(RegMeExpr &regmeExpr, bool atParm);
  MeExpr &PropIvar(IvarMeExpr &ivarMeExpr);
  MeExpr &PropMeExpr(MeExpr &meExpr, bool &isproped, bool atParm);
  MeExpr *SimplifyMeExpr(OpMeExpr &opMeExpr);
  MeExpr *SimplifyCvtMeExpr(const OpMeExpr &opMeExpr);
  MeExpr *SimplifyCompareMeExpr(OpMeExpr &opMeExpr);
  MeExpr *SimplifyCompareSelectConstMeExpr(const OpMeExpr &opMeExpr, const MeExpr &opMeOpnd0, MeExpr &opnd1,
      MeExpr &opnd01, MeExpr &opnd02);

  IRMap &irMap;
  SSATab &ssaTab;
  MIRModule &mirModule;
  MapleAllocator propMapAlloc;
  std::vector<BB*> bbVec;
  BB &commonEntryBB;
  std::vector<std::stack<SafeMeExprPtr>> vstLiveStackVec;
  std::vector<bool> bbVisited;  // needed because dominator tree is a DAG in wpo
  BB *curBB = nullptr;          // gives the bb of the traversal
  PropConfig config;
};
}  // namespace maple
#endif  // MAPLE_ME_INCLUDE_PROP_H
