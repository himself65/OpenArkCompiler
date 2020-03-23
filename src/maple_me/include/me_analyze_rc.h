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
#ifndef MAPLE_ME_INCLUDE_MEANALYZERC_H
#define MAPLE_ME_INCLUDE_MEANALYZERC_H
#include "me_function.h"
#include "me_phase.h"
#include "me_alias_class.h"
#include "me_irmap.h"

namespace maple {
class RCItem {
 public:
  RCItem(const OriginalSt &o, MapleAllocator &alloc)
      : ost(o),
        noAlias(false),
        nonLocal(false),
        isFormal(false),
        needSomeRC(false),
        versionStack(alloc.Adapter()),
        occurBBs(std::less<BBId>(), alloc.Adapter()) {}

  virtual ~RCItem() = default;

  void Dump();
 private:
  friend class AnalyzeRC;
  const OriginalSt &ost;
  bool noAlias;
  bool nonLocal;            // need to do placement optimization if (!nonLocal)
  bool isFormal;            // is an incoming formal parameter
  bool needSomeRC;          // true if any definition has rhs that needs RC
  MapleStack<MeExpr*> versionStack;
  MapleSet<BBId> occurBBs;  // set of BBs where the pointer occurs; only for local ref pointers
};

class AnalyzeRC {
 public:
  AnalyzeRC(MeFunction &f, Dominance &dom, AliasClass &ac, MemPool *memPool)
      : func(f),
        irMap(*f.GetIRMap()),
        ssaTab(*f.GetMeSSATab()),
        dominance(dom),
        aliasClass(ac),
        analyzeRCMp(memPool),
        analyzeRCAllocator(memPool),
        meBuilder(irMap.GetIRMapAlloc()),
        rcItemsMap(std::less<OStIdx>(), analyzeRCAllocator.Adapter()),
        skipLocalRefVars(false) {}

  virtual ~AnalyzeRC() = default;
  void Run();

 private:
  void IdentifyRCStmts();
  void CreateCleanupIntrinsics();
  void RenameRefPtrs(BB *bb);
  void OptimizeRC();
  void RemoveUnneededCleanups();
  void RenameUses(MeStmt &meStmt);
  RCItem *FindOrCreateRCItem(const OriginalSt &ost);
  OriginalSt *GetOriginalSt(const MeExpr &refLHS) const;
  VarMeExpr *GetZeroVersionVarMeExpr(const VarMeExpr &var);
  bool NeedIncref(const MeStmt &stmt) const;
  UnaryMeStmt *CreateIncrefZeroVersion(const OriginalSt &ost);
  DassignMeStmt *CreateDassignInit(OriginalSt &ost, BB &bb);
  void TraverseStmt(BB &bb);
  bool NeedDecRef(RCItem &rcItem, MeExpr &expr) const;
  bool NeedDecRef(IvarMeExpr &ivar) const;
  bool NeedDecRef(const VarMeExpr &var) const;

  friend class MeDoAnalyzeRC;
  MeFunction &func;
  IRMap &irMap;
  SSATab &ssaTab;
  Dominance &dominance;
  AliasClass &aliasClass;
  MemPool *analyzeRCMp;
  MapleAllocator analyzeRCAllocator;
  MeBuilder meBuilder;
  MapleMap<OStIdx, RCItem*> rcItemsMap;
  bool skipLocalRefVars;
};

class MeDoAnalyzeRC : public MeFuncPhase {
 public:
  explicit MeDoAnalyzeRC(MePhaseID id) : MeFuncPhase(id) {}

  virtual ~MeDoAnalyzeRC() = default;
  AnalysisResult *Run(MeFunction *func, MeFuncResultMgr *m, ModuleResultMgr *mrm) override;
  std::string PhaseName() const override {
    return "analyzerc";
  }
};
}  // namespace maple
#endif  // MAPLE_ME_INCLUDE_MEANALYZERC_H
