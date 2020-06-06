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
#ifndef MAPLEME_INCLUDE_ME_ABCOPT
#define MAPLEME_INCLUDE_ME_ABCOPT
#include "me_function.h"
#include "me_irmap.h"
#include "me_ir.h"
#include "me_inequality_graph.h"
#include "me_cfg.h"

namespace maple {
struct StmtComparator {
  bool operator()(const std::pair<MeStmt*, size_t> &lhs, const std::pair<MeStmt*, size_t> &rhs) const {
    if (lhs.first != rhs.first) {
      return lhs.first < rhs.first;
    } else {
      return lhs.second < rhs.second;
    }
  }
};

class CarePoint {
 public:
  enum CareKind {
    kMeStmt,
    kMePhi
  };

  explicit CarePoint(CareKind ck) : careKind(ck) {}
  ~CarePoint() = default;

  bool IsCareMeStmt() const {
    return careKind == kMeStmt;
  }

  MeStmt *GetMeStmt() {
    CHECK_FATAL(careKind == kMeStmt, "must be");
    return value.meStmt;
  }

  const MeStmt *GetMeStmt() const {
    CHECK_FATAL(careKind == kMeStmt, "must be");
    return value.meStmt;
  }

  MePhiNode *GetMePhi() {
    CHECK_FATAL(careKind == kMePhi, "must be");
    return value.phiNode;
  }

  const MePhiNode *GetMePhi() const {
    CHECK_FATAL(careKind == kMePhi, "must be");
    return value.phiNode;
  }

  void SetMeStmt(MeStmt &meStmt) {
    CHECK_FATAL(careKind == kMeStmt, "must be");
    value.meStmt = &meStmt;
  }

  void SetMePhi(MePhiNode &phi) {
    CHECK_FATAL(careKind == kMePhi, "must be");
    value.phiNode = &phi;
  }

 private:
  CareKind careKind;
  union CareStmt {
    MeStmt *meStmt;
    MePhiNode *phiNode;
  };
  CareStmt value;
};

class DefPoint {
 public:
  enum DefineKind {
    kDefByPi,
    kDefByPhi
  };

  explicit DefPoint(DefineKind dk) : defKind(dk) {}
  ~DefPoint() = default;

  void SetDefPi(PiassignMeStmt &s) {
    CHECK_FATAL(defKind == kDefByPi, "must be");
    value.pi = &s;
  }

  PiassignMeStmt *GetPiStmt() {
    CHECK_FATAL(defKind == kDefByPi, "must be");
    return value.pi;
  }

  const PiassignMeStmt *GetPiStmt() const {
    CHECK_FATAL(defKind == kDefByPi, "must be");
    return value.pi;
  }

  void SetDefPhi(MePhiNode &s) {
    CHECK_FATAL(defKind == kDefByPhi, "must be");
    value.phi = &s;
  }

  BB *GetBB() const {
    if (defKind == kDefByPi) {
      return value.pi->GetBB();
    } else {
      return value.phi->GetDefBB();
    }
  }

  BB *GetGeneratedByBB() const {
    CHECK_FATAL(defKind == kDefByPi, "must be");
    return value.pi->GetGeneratedBy()->GetBB();
  }

  VarMeExpr *GetRHS() const {
    if (defKind == kDefByPi) {
      return value.pi->GetRHS();
    } else {
      return static_cast<VarMeExpr*>(value.phi->GetOpnd(0));
    }
  }

  VarMeExpr *GetLHS() const {
    if (defKind == kDefByPi) {
      return value.pi->GetLHS();
    } else {
      return  static_cast<VarMeExpr*>(value.phi->GetLHS());
    }
  }

  const OStIdx &GetOStIdx() const {
    if (defKind == kDefByPi) {
      return value.pi->GetRHS()->GetOStIdx();
    } else {
      return value.phi->GetOpnd(0)->GetOStIdx();
    }
  }

  bool IsPiStmt() const {
    return defKind == kDefByPi;
  }

  bool IsGeneratedByBr() const {
    CHECK_FATAL(defKind == kDefByPi, "must be");
    MeStmt *stmt = value.pi->GetGeneratedBy();
    if (stmt->GetOp() == OP_brtrue || stmt->GetOp() == OP_brfalse) {
      return true;
    }
    return false;
  }

  void RemoveFromBB() {
    if (defKind == kDefByPi) {
      if (IsGeneratedByBr()) {
        GetBB()->GetPiList().clear();
      } else {
        GetBB()->RemoveMeStmt(value.pi);
      }
    } else {
      GetBB()->GetMePhiList().erase(GetOStIdx());
    }
  }

  void Dump(const IRMap &irMap) {
    LogInfo::MapleLogger() << "New Def : " << '\n';
    if (defKind == kDefByPi) {
      value.pi->Dump(&irMap);
    } else {
      value.phi->Dump(&irMap);
    }
    LogInfo::MapleLogger() << '\n';
  }

 private:
  DefineKind defKind;
  union DefStmt {
    PiassignMeStmt *pi;
    MePhiNode *phi;
  };
  DefStmt value;
};

class MeABC {
 public:
  static bool isDebug;
  MeABC(MeFunction &meFunction, Dominance &dom, MeIRMap &map, MemPool &pool)
      : meFunc(&meFunction),
        dom(&dom),
        irMap(&map),
        memPool(&pool),
        allocator(&pool),
        inequalityGraph(nullptr),
        prove(nullptr),
        forbidenPi(nullptr) {}
  ~MeABC() = default;
  void ExecuteABCO();

 private:
  bool CollectABC();
  void RemoveExtraNodes();
  void InsertPiNodes();
  bool ExistedPhiNode(BB &bb, VarMeExpr &rhs);
  void InsertPhiNodes();
  void Rename();
  void RenameStartPiBr(DefPoint &newDefPoint);
  void RenameStartPiArray(DefPoint &newDefPoint);
  void RenameStartPhi(DefPoint &newDefPoint);
  void ReplacePiPhiInSuccs(BB &bb, VarMeExpr &newVar);
  bool ReplaceStmt(MeStmt &meStmt, VarMeExpr &newVar, VarMeExpr &oldVar);
  void ReplaceBB(BB &bb, BB &parentBB, DefPoint &newDefPoint);
  bool IsLegal(MeStmt &meStmt);
  void ABCCollectArrayExpr(MeStmt &meStmt, MeExpr &meExpr, bool isUpdate = false);
  void CollectCareInsns();
  bool ExistedPiNode(BB &bb, BB &parentBB, const VarMeExpr &rhs);
  void CreatePhi(VarMeExpr &rhs, BB &dfBB);
  VarMeExpr *CreateNewPiExpr(const MeExpr &opnd);
  void CreateNewPiStmt(VarMeExpr *lhs, MeExpr &rhs, BB &bb, MeStmt &generatedBy, bool isToken);
  void CreateNewPiStmt(VarMeExpr *lhs, MeExpr &rhs, MeStmt &generatedBy);
  MeExpr *ReplaceMeExprExpr(MeExpr &origExpr, MeExpr &oldVar, MeExpr &repExpr);
  MeExpr *NewMeExpr(MeExpr &meExpr);
  bool ReplaceMeExprStmtOpnd(uint32 opndID, MeStmt &meStmt, MeExpr &oldVar, MeExpr &newVar, bool update);
  bool ReplaceStmtWithNewVar(MeStmt &meStmt, MeExpr &oldVar, MeExpr &newVar, bool update);
  bool IsVirtualVar(const VarMeExpr &var, const SSATab &ssaTab) const;
  ESSABaseNode *GetOrCreateRHSNode(MeExpr &expr);
  void BuildPhiInGraph(MePhiNode &phi);
  void BuildSoloPiInGraph(const PiassignMeStmt &piMeStmt);
  bool BuildArrayCheckInGraph(MeStmt &meStmt);
  bool BuildBrMeStmtInGraph(MeStmt &meStmt);
  bool BuildAssignInGraph(MeStmt &meStmt);
  MeExpr *TryToResolveVar(MeExpr &expr, std::set<MePhiNode*> &visitedPhi, MeExpr &dummyExpr, bool isConst);
  bool BuildStmtInGraph(MeStmt &meStmt);
  void AddUseDef(MeExpr &meExpr);
  void AddCareInsn(MeStmt &defS);
  void AddCarePhi(MePhiNode &defP);
  void BuildInequalityGraph();
  void FindRedundantABC(MeStmt &meStmt, NaryMeExpr &naryMeExpr);
  void InitNewStartPoint(const MeStmt &meStmt, const NaryMeExpr &nMeExpr);
  void DeleteABC();
  bool CleanABCInStmt(MeStmt &meStmt, NaryMeExpr &naryMeExpr);
  MeExpr *ReplaceArrayExpr(MeExpr &rhs, MeExpr &naryMeExpr, MeStmt *ivarStmt);

  Dominance *GetDominace() {
    return dom;
  }

  MemPool *GetMemPool() {
    return memPool;
  }

  MapleAllocator &GetAllocator() {
    return allocator;
  }

  const MeIRMap *GetIRMap() const {
    return irMap;
  }

  MeFunction *meFunc;
  Dominance *dom;
  MeIRMap *irMap;
  MemPool *memPool;
  MapleAllocator allocator;
  std::unique_ptr<InequalityGraph> inequalityGraph;
  std::unique_ptr<ABCD> prove;
  MeStmt *forbidenPi;
  std::map<MeStmt*, NaryMeExpr*> arrayChecks;
  std::map<MeStmt*, NaryMeExpr*> arrayNewChecks;
  std::set<MeStmt*> careMeStmts;
  std::set<MePhiNode*> careMePhis;
  std::map<BB*, std::vector<MeStmt*>> containsBB;
  std::vector<DefPoint*> newDefPoints;
  std::vector<CarePoint*> carePoints;
  std::map<DefPoint*, VarMeExpr*> newDef2Old;
  std::map<std::pair<MeStmt*, size_t>, MeExpr*, StmtComparator> modifiedStmt;
  std::map<MePhiNode*, std::vector<ScalarMeExpr*>> modifiedPhi;
  std::set<BB*> visitedBBs;
  std::set<MeStmt*> targetMeStmt;
};

class MeDoABCOpt : public MeFuncPhase {
 public:
  explicit MeDoABCOpt(MePhaseID id) : MeFuncPhase(id) {}
  ~MeDoABCOpt() = default;
  AnalysisResult *Run(MeFunction *func, MeFuncResultMgr *frm, ModuleResultMgr *mrm) override;

  std::string PhaseName() const override {
    return "abcopt";
  }
};
}
#endif
