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
#ifndef MAPLE_ME_INCLUDE_SSAPRE_H
#define MAPLE_ME_INCLUDE_SSAPRE_H
#include "irmap.h"
#include "occur.h"
#include "securec.h"
namespace maple {
enum PreKind {
  kExprPre,
  kStmtPre,
  kLoadPre,
  kAddrPre
};

class SSAPre {
 public:
  SSAPre(IRMap &hMap, Dominance &currDom, MemPool &memPool, MemPool &mp2, PreKind kind, uint32 limit)
      : irMap(&hMap),
        ssaTab(&hMap.GetSSATab()),
        mirModule(&hMap.GetSSATab().GetModule()),
        dom(&currDom),
        ssaPreMemPool(&memPool),
        ssaPreAllocator(&memPool),
        perCandMemPool(&mp2),
        perCandAllocator(&mp2),
        workList(ssaPreAllocator.Adapter()),
        preKind(kind),
        allOccs(ssaPreAllocator.Adapter()),
        phiOccs(ssaPreAllocator.Adapter()),
        exitOccs(ssaPreAllocator.Adapter()),
        preLimit(limit),
        dfPhiDfns(std::less<uint32>(), ssaPreAllocator.Adapter()),
        varPhiDfns(std::less<uint32>(), ssaPreAllocator.Adapter()),
        rename2Set(std::less<uint32>(), ssaPreAllocator.Adapter()),
        temp2LocalRefVarMap(ssaPreAllocator.Adapter()) {
    PreWorkCand::GetWorkcandHashTable().fill(nullptr);
  }

  virtual ~SSAPre() = default;

  void ApplySSAPRE();
  bool DefVarDominateOcc(MeExpr *meExpr, MeOccur *meOcc);
  virtual void CollectVarForMeExpr(MeExpr *meExpr, std::vector<MeExpr*> &varVec) = 0;
  virtual void CollectVarForCand(MeRealOcc *realOcc, std::vector<MeExpr*> &varVec) = 0;
  const MapleVector<MeRealOcc*> &GetRealOccList() const {
    return workCand->GetRealOccs();
  }

  virtual MeExpr *CopyMeExpr(MeExpr &expr);
  virtual MeStmt *CopyMeStmt(MeStmt &meStmt);
  virtual IassignMeStmt *CopyIassignMeStmt(const IassignMeStmt &iaStmt);
  void IncTreeid() {
    curTreeId += 2;
  }

  virtual void DumpWorkList();
  virtual void DumpWorkListWrap();
  GStrIdx NewTempStrIdx();
  virtual BB *GetBB(BBId id) = 0;
  virtual PUIdx GetPUIdx() const = 0;
  virtual void SetCurFunction(PUIdx) {}

  virtual void GetIterDomFrontier(BB &bb, MapleSet<uint32> &dfSet, std::vector<bool> &visitedMap) = 0;
  void SetSpillAtCatch(bool status) {
    spillAtCatch = status;
  }

  bool GetSpillAtCatch() const {
    return spillAtCatch;
  }

  PreWorkCand* GetWorkCand() const {
    return workCand;
  }

  void SetAddedNewLocalRefVars(bool status) {
    addedNewLocalRefVars = status;
  }

  bool GetAddedNewLocalRefVars() const {
    return addedNewLocalRefVars;
  }

  void SetSSAPreDebug(bool status) {
    enableDebug = status;
  }

  bool GetSSAPreDebug() const {
    return enableDebug;
  }

  void SetPlacementRC(bool status) {
    placementRCEnabled = status;
  }

  bool GetPlacementRCOn() const {
    return placementRCEnabled;
  }

  void SetRcLoweringOn(bool status) {
    rcLoweringEnabled = status;
  }

  bool GetRcLoweringOn() const {
    return rcLoweringEnabled;
  }

  void SetRegReadAtReturn(bool status) {
    regReadAtReturn = status;
  }

  bool GetRegReadAtReturn() const {
    return regReadAtReturn;
  }

 protected:
  IRMap *irMap;
  SSATab *ssaTab;
  MIRModule *mirModule;
  Dominance *dom;
  MemPool *ssaPreMemPool;
  MapleAllocator ssaPreAllocator;
  MemPool *perCandMemPool;
  MapleAllocator perCandAllocator;
  MapleVector<PreWorkCand*> workList;
  PreWorkCand *workCand = nullptr;  // the current PreWorkCand
  PreKind preKind;

  uint32 curTreeId = 0;  // based on number of rooted trees processed in collecting
  // PRE work candidates; incremented by 2 for each tree;
  // purpose is to avoid processing a node the third time
  // inside a tree (which is a DAG)
  // the following 3 lists are all maintained in order of dt_preorder
  MapleVector<MeOccur*> allOccs;     // cleared at start of each workcand
  MapleVector<MePhiOcc*> phiOccs;    // cleared at start of each workcand
  MapleVector<MeOccur*> exitOccs;  // this is shared by all workcands
  uint32 preLimit;  // set by command-line option to limit the number of candidates optimized (for debugging purpose)
  // step 1 phi insertion data structures
  // following are set of BBs in terms of their dfn's; index into
  // dominance->pdt_preorder to get their bbid's
  MapleSet<uint32> dfPhiDfns;   // phis inserted due to dominance frontiers
  MapleSet<uint32> varPhiDfns;  // phis inserted due to the var operands
  // step 2 renaming data structures
  uint32 classCount = 0;            // count class created during renaming
  MapleSet<uint32> rename2Set;  // set created by rename1 for use rename2; value
  // is index into workCand->realOccs
  // step 6 codemotion data structures
  MeExpr *curTemp = nullptr;            // the created temp for current workCand
  VarMeExpr *curLocalRefVar = nullptr;  // the created localrefvar for ref-type iread
  MapleMap<RegMeExpr*, VarMeExpr*> temp2LocalRefVarMap;
  int32 reBuiltOccIndex = -1;  // stores the size of worklist every time when try to add new worklist, update before each
  // code motion
  uint32 strIdxCount = 0;  // ssapre will create a lot of temp variables if using var to store redundances, start from 0
  // step 6 codemotion methods
  MeExpr *CreateNewCurTemp(MeExpr *meExpr);
  VarMeExpr *CreateNewCurLocalRefVar();
  virtual void GenerateSaveRealOcc(MeRealOcc *realOcc) = 0;
  virtual void GenerateReloadRealOcc(MeRealOcc *realOcc) = 0;
  void GenerateSaveInsertedOcc(MeInsertedOcc *insertedOcc);
  void GenerateSavePhiOcc(MePhiOcc *phiOcc);
  void UpdateInsertedPhiOccOpnd();
  virtual void CodeMotion();
  // step 5 Finalize methods
  virtual void Finalize1();
  void SetSave(MeOccur *defX);
  void SetReplacement(MePhiOcc *occ, MeOccur *repDef);
  virtual void Finalize2();
  // step 4 willbevail methods
  void ComputeCanBeAvail();
  void ResetCanBeAvail(MePhiOcc *occ);
  void ComputeLater();
  void ResetLater(MePhiOcc *occ);
  // step 3 downsafety methods
  void ResetDS(MePhiOpndOcc *phiOpnd);
  void ComputeDS();
  // step 2 renaming methods
  virtual bool AllVarsSameVersion(MeRealOcc *realOcc1, MeRealOcc *realOcc2) {
    return realOcc1->GetMeExpr() == realOcc2->GetMeExpr();
  }

  void Rename1();
  MeExpr *GetReplaceMeExpr(MeExpr *opnd, const BB *ePhiBB, size_t j);
  virtual MeExpr *PhiOpndFromRes(MeRealOcc *realOcc, size_t i) = 0;
  virtual void Rename2();
  // step 1 phi insertion methods
  void SetVarPhis(MeExpr *meExpr);
  virtual void ComputeVarAndDfPhis() = 0;
  virtual void CreateSortedOccs();
  // phi insertion methods end
  virtual void BuildWorkList() = 0;
  virtual void BuildEntryLHSOcc4Formals() {}

  virtual void BuildWorkListLHSOcc(MeStmt*, int32) {}

  virtual void BuildWorkListIvarLHSOcc(MeStmt*, int32, bool, MeExpr*) {}

  virtual void BuildWorkListExpr(MeStmt *meStmt, int32 seqStmt, MeExpr *meExpr, bool isRebuilt, MeExpr *tempVar,
                                 bool isRootExpr) = 0;
  virtual void BuildWorkListStmt(MeStmt* meStmt, uint32 seqStmt, bool isRebuilt, MeExpr *tempVar = nullptr);
  virtual void BuildWorkListBB(BB *bb);
  virtual void ConstructUseOccurMap() {}

  void CreateMembarOcc(MeStmt &meStmt, int seqStmt);
  virtual void CreateMembarOccAtCatch(BB &bb);
  void CreateExitOcc(BB *bb) {
    MeOccur *exitOcc = ssaPreMemPool->New<MeOccur>(kOccExit, 0, bb, nullptr);
    exitOccs.push_back(exitOcc);
  }

  bool CheckIfAnyLocalOpnd(MeExpr *meExpr);
  MeRealOcc *CreateRealOcc(MeStmt &meStmt, int32 seqStmt, MeExpr &meExpr, bool isRebuilt, bool isLHS = false);
  virtual bool ScreenPhiBB(BBId bbId) const = 0;
  virtual bool EpreLocalRefVar() {
    return false;
  }

  virtual void EnterCandsForSSAUpdate(OStIdx, BB*) {}

  virtual bool IsLoopHeadBB(BBId) const {
    return false;
  }

 private:
  bool enableDebug = false;
  bool rcLoweringEnabled = false;
  bool regReadAtReturn = false;
  bool spillAtCatch = false;
  bool placementRCEnabled = false;
  bool addedNewLocalRefVars = false;
  virtual void DoSSAFRE() {};
};
}  // namespace maple
#endif  // MAPLE_ME_INCLUDE_SSAPRE_H
