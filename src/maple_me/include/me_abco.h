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
#include "mir_module.h"
#include "mir_builder.h"
#include "me_ssi.h"

namespace maple {
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
        currentCheck(nullptr) {
          ssi = std::make_unique<MeSSI>(meFunction, dom, map, pool, &arrayChecks, &containsBB);
          ssi->SetSSIType(kArrayBoundsCheckOpt);
        }
  ~MeABC() = default;
  void ExecuteABCO();

 private:
  bool CollectABC();
  void ABCCollectArrayExpr(MeStmt &meStmt, MeExpr &meExpr, bool isUpdate = false);
  void CollectCareInsns();
  bool IsVirtualVar(const VarMeExpr &var, const SSATab &ssaTab) const;
  ESSABaseNode *GetOrCreateRHSNode(MeExpr &expr);
  void BuildPhiInGraph(MePhiNode &phi);
  void BuildSoloPiInGraph(const PiassignMeStmt &piMeStmt);
  bool PiExecuteBeforeCurrentCheck(const PiassignMeStmt &piMeStmt);
  void AddEdgePair(ESSABaseNode &from, ESSABaseNode &to, int64 value, EdgeType type);
  bool BuildArrayCheckInGraph(MeStmt &meStmt);
  bool BuildBrMeStmtInGraph(MeStmt &meStmt);
  bool BuildAssignInGraph(MeStmt &meStmt);
  MeExpr *TryToResolveVar(MeExpr &expr, bool isConst);
  MeExpr *TryToResolveVar(MeExpr &expr, std::set<MePhiNode*> &visitedPhi, MeExpr &dummyExpr, bool isConst);
  bool BuildStmtInGraph(MeStmt &meStmt);
  void AddUseDef(MeExpr &meExpr);
  void AddCareInsn(MeStmt &defS);
  void AddCarePhi(MePhiNode &defP);
  void BuildInequalityGraph();
  bool IsLessOrEuqal(const MeExpr &opnd1, const MeExpr &opnd2);
  void ProcessCallParameters(CallMeStmt &callNode);
  void FindRedundantABC(MeStmt &meStmt, NaryMeExpr &naryMeExpr);
  void InitNewStartPoint(MeStmt &meStmt, MeExpr &opnd1, MeExpr &opnd2, bool clearGraph = true);
  void DeleteABC();
  bool CleanABCInStmt(MeStmt &meStmt, NaryMeExpr &naryMeExpr);
  MeExpr *ReplaceArrayExpr(MeExpr &rhs, MeExpr &naryMeExpr, MeStmt *ivarStmt);
  bool HasRelativeWithLength(MeExpr &meExpr);
  bool ProveGreaterZ(const MeExpr &weight);
  void ReSolveEdge();
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
  MeStmt *currentCheck;
  std::unique_ptr<MeSSI> ssi;
  std::map<MeStmt*, NaryMeExpr*> arrayChecks;
  std::map<MeStmt*, NaryMeExpr*> arrayNewChecks;
  std::set<MeStmt*> careMeStmts;
  std::set<MePhiNode*> careMePhis;
  std::map<BB*, std::vector<MeStmt*>> containsBB;
  std::vector<CarePoint*> carePoints;
  std::set<MeStmt*> targetMeStmt;
  std::set<MePhiNode*> visited;
  // map<std::pair<a, b>, c>; a = b + c  b is relative with length, c is var weight
  std::map<std::pair<MeExpr*, MeExpr*>, MeExpr*> unresolveEdge;
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
