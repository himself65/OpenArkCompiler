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
#ifndef MAPLE_ME_INCLUDE_MESTMTPRE_H
#define MAPLE_ME_INCLUDE_MESTMTPRE_H

#include "me_function.h"
#include "me_irmap.h"
#include "ssa_epre.h"

namespace maple {
class MeStmtPre : public SSAEPre {
 public:
  // a symbol is a candidate for ssaupdate if its ostidx key exists in the map;
  // the mapped set gives bbs where dassign's are inserted by stmtpre for the symbol
  MeStmtPre(MeFunction &func, IRMap &map, Dominance &dom, MemPool &memPool, MemPool &mp2, uint32 limit)
      : SSAEPre(map, dom, memPool, mp2, kStmtPre, limit, true, false),
        candsForSSAUpdate(std::less<OStIdx>(), ssaPreAllocator.Adapter()),
        func(&func),
        versionStackVec(ssaTab->GetOriginalStTable().GetOriginalStVector().size(), nullptr, ssaPreAllocator.Adapter()),
        useOccurMap(std::less<OStIdx>(), ssaPreAllocator.Adapter()) {}

  virtual ~MeStmtPre() = default;
  void GetIterDomFrontier(const BB &bb, MapleSet<uint32> &dfSet, std::vector<bool> &visitedMap) const override;
  bool ScreenPhiBB(BBId) const override {
    return true;
  }

  MapleMap<OStIdx, MapleSet<BBId>*> &GetCandsForSSAUpdate() {
    return candsForSSAUpdate;
  }

 protected:
  MapleMap<OStIdx, MapleSet<BBId>*> candsForSSAUpdate;
  MeFunction *func;
  MapleVector<MapleStack<ScalarMeExpr*>*> versionStackVec;  // top of stack gives last version during BuildWorkList()
  MapleMap<OStIdx, MapleSet<uint32>*> useOccurMap;  // give the set of BBs (in dfn) that contain uses of the symbol

 private:
  // code motion phase
  void CodeMotion() override;
  // finalize phase
  void Finalize1() override;
  void Finalize2() override {};
  // fully available (replaces downsafety, canbeavail and later under SSAFRE)
  void ResetFullyAvail(MePhiOcc &occ);
  void ComputeFullyAvail();
  // rename phase
  bool AllVarsSameVersion(const MeRealOcc &realOcc1, const MeRealOcc &realOcc2) const override;
  bool AllVarsSameVersionStmtFre(MeRealOcc &topOcc, MeRealOcc &curOcc) const;
  void CollectVarForMeStmt(const MeStmt &meStmt, MeExpr *meExpr, std::vector<MeExpr*> &varVec) const;
  void CollectVarForCand(MeRealOcc &realOcc, std::vector<MeExpr*> &varVec) const override;
  MeStmt *CopyMeStmt(const MeStmt &meStmt) const override;
  MeStmt *PhiOpndFromRes4Stmt(MeRealOcc &realZ, size_t j, MeExpr *&lhsVar) const;
  void Rename1StmtFre();
  void Rename2() override;
  // phi insertion phase
  void ComputeVarAndDfPhis() override;
  void CreateSortedOccs() override;
  void ConstructUseOccurMapExpr(uint32 bbDfn, const MeExpr &meExpr);
  void ConstructUseOccurMap() override;  // build useOccurMap for dassign candidates
  PreStmtWorkCand *CreateStmtRealOcc(MeStmt &meStmt, int seqStmt);
  void VersionStackChiListUpdate(const MapleMap<OStIdx, ChiMeNode*> &chiList);
  void BuildWorkListBB(BB *bb) override;
  void BuildWorkList() override;
  void RemoveUnnecessaryDassign(DassignMeStmt &dssMeStmt);
  void DoSSAFRE() override;
  BB *GetBB(BBId id) const override {
    return func->GetBBFromID(id);
  }

  PUIdx GetPUIdx() const override {
    return func->GetMirFunc()->GetPuidx();
  }
};

class MeDoStmtPre : public MeFuncPhase {
 public:
  explicit MeDoStmtPre(MePhaseID id) : MeFuncPhase(id) {}

  virtual ~MeDoStmtPre() = default;
  AnalysisResult *Run(MeFunction *func, MeFuncResultMgr *m, ModuleResultMgr *mrm) override;
  std::string PhaseName() const override {
    return "stmtpre";
  }
};
}  // namespace maple
#endif  // MAPLE_ME_INCLUDE_MESTMTPRE_H
