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
  HDSE(MIRModule &mod, const MapleVector<BB*> &bbVec, BB &commonEntryBB, BB &commonExitBB, SSATab &stab,
       Dominance &pDom, IRMap &map, bool enabledDebug = false)
      : hdseDebug(enabledDebug),
        mirModule(mod),
        bbVec(bbVec),
        commonEntryBB(commonEntryBB),
        commonExitBB(commonExitBB),
        ssaTab(stab),
        postDom(pDom),
        irMap(map),
        bbRequired(bbVec.size(), false) {}

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
  void MarkMuListRequired(MapleMap<OStIdx, VarMeExpr*> &);
  void MarkChiNodeRequired(ChiMeNode &chiNode);
  bool ExprNonDeletable(MeExpr &expr);
  bool StmtMustRequired(const MeStmt &stmt, const BB &bb);
  void MarkStmtRequired(MeStmt &stmt);
  bool HasNonDeletableExpr(const MeStmt &stmt);
  void MarkStmtUseLive(MeStmt &meStmt);
  void MarkSingleUseLive(MeExpr &meExpr);
  void MarkControlDependenceLive(BB &bb);
  void MarkLastBranchStmtInBBRequired(BB &bb);
  void MarkLastStmtInPDomBBRequired(const BB &bb);
  void MarkLastUnconditionalGotoInPredBBRequired(BB &bb);
  void MarkVarDefByStmt(VarMeExpr &varMeExpr);
  void MarkRegDefByStmt(RegMeExpr &regMeExpr);

  bool IsExprNeeded(const MeExpr *meExpr) {
    return exprLive.at(meExpr->GetExprID());
  }
  void SetExprNeeded(const MeExpr *meExpr) {
    exprLive.at(meExpr->GetExprID()) = true;
  }

  void PropagateLive() {
    while (!worklist.empty()) {
      MeExpr *meExpr = worklist.front();
      worklist.pop_front();
      PropagateUseLive(*meExpr);
    }
  }

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
  std::forward_list<MeExpr*> worklist;
};
}  // namespace maple
#endif  // MAPLE_ME_INCLUDE_HDSE_H
