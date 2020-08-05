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
#ifndef MAPLE_ME_INCLUDE_HDSE_H
#define MAPLE_ME_INCLUDE_HDSE_H
#include "bb.h"
#include "irmap.h"
#include "dominance.h"

namespace maple {
class MeIRMap;
class HDSE {
 public:
  HDSE(MIRModule &mod, const MapleVector<BB*> &bbVec, BB &commonEntryBB, BB &commonExitBB, SSATab &ssaTab,
       Dominance &pDom, IRMap &map, bool enabledDebug = false, bool decouple = false)
      : hdseDebug(enabledDebug),
        mirModule(mod),
        bbVec(bbVec),
        commonEntryBB(commonEntryBB),
        commonExitBB(commonExitBB),
        ssaTab(ssaTab),
        postDom(pDom),
        irMap(map),
        bbRequired(bbVec.size(), false),
        decoupleStatic(decouple) {}

  virtual ~HDSE() = default;

  void DoHDSE();
  void InvokeHDSEUpdateLive();

 protected:
  bool hdseDebug;
  bool hdseKeepRef = false;

 private:
  void DseInit();
  void MarkSpecialStmtRequired();
  void PropagateUseLive(MeExpr &meExpr);
  void RemoveNotRequiredStmtsInBB(BB &bb);
  template <class VarOrRegPhiNode>
  void MarkPhiRequired(VarOrRegPhiNode &mePhiNode);
  void MarkMuListRequired(MapleMap<OStIdx, VarMeExpr*>&);
  void MarkChiNodeRequired(ChiMeNode &chiNode);
  bool ExprHasSideEffect(const MeExpr &meExpr) const;
  bool ExprNonDeletable(const MeExpr &expr) const;
  bool StmtMustRequired(const MeStmt &stmt, const BB &bb) const;
  void MarkStmtRequired(MeStmt &stmt);
  bool HasNonDeletableExpr(const MeStmt &stmt) const;
  void MarkStmtUseLive(MeStmt &meStmt);
  void MarkSingleUseLive(MeExpr &meExpr);
  void MarkControlDependenceLive(BB &bb);
  void MarkLastBranchStmtInBBRequired(BB &bb);
  void MarkLastStmtInPDomBBRequired(const BB &bb);
  void MarkLastUnconditionalGotoInPredBBRequired(const BB &bb);
  void MarkDefStmt(ScalarMeExpr &scalarExpr);
  void MarkRegDefByStmt(RegMeExpr &regMeExpr);
  void CollectNotNullExpr(MeStmt &stmt);
  // NotNullExpr means it is impossible value of the expr is nullptr after go through this stmt.
  // exprType must be one kind of NODE_TYPE_NORMAL、NODE_TYPE_IVAR、NODE_TYPE_NOTNULL
  void CollectNotNullExpr(MeStmt &stmt, MeExpr &meExpr, uint8 exprType = 0);
  bool NeedNotNullCheck(MeExpr &meExpr, const BB &bb);

  bool IsExprNeeded(const MeExpr &meExpr) const {
    return exprLive.at(static_cast<size_t>(static_cast<uint32>(meExpr.GetExprID())));
  }
  void SetExprNeeded(const MeExpr &meExpr) {
    exprLive.at(static_cast<size_t>(static_cast<uint32>(meExpr.GetExprID()))) = true;
  }

  void AddNewUse(MeExpr &meExpr) {
    if (IsExprNeeded(meExpr)) {
      return;
    }
    SetExprNeeded(meExpr);
    workList.push_front(&meExpr);
  }

  void PropagateLive() {
    while (!workList.empty()) {
      MeExpr *meExpr = workList.front();
      workList.pop_front();
      PropagateUseLive(*meExpr);
    }
  }

  void RemoveNotRequiredCallAssignPart(MeStmt &stmt);

  void RemoveNotRequiredStmts() {
    for (auto *bb : bbVec) {
      if (bb == nullptr) {
        continue;
      }
      RemoveNotRequiredStmtsInBB(*bb);
    }
  }

  MIRModule &mirModule;
  MapleVector<BB*> bbVec;
  BB &commonEntryBB;
  BB &commonExitBB;
  SSATab &ssaTab;
  Dominance &postDom;
  IRMap &irMap;
  std::vector<bool> bbRequired;
  std::vector<bool> exprLive;
  std::forward_list<MeExpr*> workList;
  std::unordered_map<MeStmt*, std::vector<MeExpr*>> stmt2NotNullExpr;
  std::unordered_map<MeExpr*, std::vector<MeStmt*>> notNullExpr2Stmt;
  // Initial type of all meExpr
  static const uint8 kExprTypeNormal = 0;
  // IreadMeExpr
  static const uint8 kExprTypeIvar = 1;
  // NPE will be throw if the value of this meExpr is nullptr when stmt is executed
  // Or the meExpr is opnd of a same type meExpr
  static const uint8 kExprTypeNotNull = 2;
  bool decoupleStatic = false;
};
}  // namespace maple
#endif  // MAPLE_ME_INCLUDE_HDSE_H
