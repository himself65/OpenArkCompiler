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
#ifndef MAPLE_ME_INCLUDE_ME_SSA_LPRE_H
#define MAPLE_ME_INCLUDE_ME_SSA_LPRE_H
#include "me_irmap.h"
#include "ssa_pre.h"
#include "me_loop_analysis.h"

namespace maple {
constexpr size_t kDoLpreBBsLimit = 0x7fffff;
class MeSSALPre : public SSAPre {
 public:
  MeSSALPre(MeFunction *f, MeIRMap &hMap, Dominance &dom, MemPool &memPool, MemPool &mp2, PreKind kind, uint32 limit)
      : SSAPre(hMap, dom, memPool, mp2, kind, limit),
        irMap(&hMap),
        func(f),
        assignedFormals(ssaPreAllocator.Adapter()),
        loopHeadBBs(ssaPreAllocator.Adapter()) {}

  virtual ~MeSSALPre() = default;
  void FindLoopHeadBBs(IdentifyLoops *identLoops);

 private:
  void GenerateSaveRealOcc(MeRealOcc*) override;
  void GenerateReloadRealOcc(MeRealOcc*) override;
  MeExpr *PhiOpndFromRes(MeRealOcc*, size_t) override;
  void GetIterDomFrontier(BB &bb, MapleSet<uint32> &dfSet, std::vector<bool> &visitedMap) override;
  void ComputeVarAndDfPhis() override;
  bool ScreenPhiBB(BBId) const override {
    return true;
  }

  void CollectVarForMeExpr(MeExpr *meExpr, std::vector<MeExpr*> &varVec) override {
    if (meExpr->GetMeOp() == kMeOpAddrof || meExpr->GetMeOp() == kMeOpAddroffunc) {
      return;
    }
    varVec.push_back(meExpr);
  }

  void CollectVarForCand(MeRealOcc *realOcc, std::vector<MeExpr*> &varVec) override {
    if (realOcc->GetMeExpr()->GetMeOp() == kMeOpAddrof || realOcc->GetMeExpr()->GetMeOp() == kMeOpAddroffunc) {
      return;
    }
    varVec.push_back(realOcc->GetMeExpr());
  }

  void BuildEntryLHSOcc4Formals() override;
  void BuildWorkListLHSOcc(MeStmt *meStmt, int32 seqStmt) override;
  void CreateMembarOccAtCatch(BB &bb) override;
  void BuildWorkListExpr(MeStmt*, int32, MeExpr*, bool, MeExpr*, bool isRootExpr) override;
  void BuildWorkList() override;
  BB *GetBB(BBId id) override {
    return func->GetBBFromID(id);
  }

  PUIdx GetPUIdx() const override {
    return func->GetMirFunc()->GetPuidx();
  }

  bool IsLoopHeadBB(BBId bbId) const override {
    return loopHeadBBs.find(bbId) != loopHeadBBs.end();
  }

  MeIRMap *irMap;
  MeFunction *func;
  MapleSet<OStIdx> assignedFormals;  // set of formals that are assigned
  MapleSet<BBId> loopHeadBBs;
};

class MeDoSSALPre : public MeFuncPhase {
 public:
  explicit MeDoSSALPre(MePhaseID id) : MeFuncPhase(id) {}

  virtual ~MeDoSSALPre() = default;
  AnalysisResult *Run(MeFunction *irFunc, MeFuncResultMgr *funcMgr, ModuleResultMgr *moduleMgr) override;
  std::string PhaseName() const override {
    return "lpre";
  }
};
}  // namespace maple
#endif  // MAPLE_ME_INCLUDE_ME_SSA_LPRE_H
