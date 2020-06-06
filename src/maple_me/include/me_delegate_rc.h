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
#ifndef MAPLE_ME_INCLUDE_MEDELEGATERC_H
#define MAPLE_ME_INCLUDE_MEDELEGATERC_H
#include "me_function.h"
#include "me_phase.h"
#include "me_irmap.h"

namespace maple {
class DelegateRC {
 public:
  DelegateRC(MeFunction &func, Dominance &dom, MemPool *memPool, bool enabledDebug)
      : func(func),
        irMap(*func.GetIRMap()),
        ssaTab(*func.GetMeSSATab()),
        dominance(dom),
        delegateRCAllocator(memPool),
        verStCantDelegate(irMap.GetVerst2MeExprTableSize(), false, delegateRCAllocator.Adapter()),
        verStUseCounts(irMap.GetVerst2MeExprTableSize(), 0, delegateRCAllocator.Adapter()),
        refVar2RegMap(delegateRCAllocator.Adapter()),
        verStDerefedCopied(irMap.GetVerst2MeExprTableSize(), false, delegateRCAllocator.Adapter()),
        verStCantDecrefEarly(irMap.GetVerst2MeExprTableSize(), false, delegateRCAllocator.Adapter()),
        enabledDebug(enabledDebug) {}

  virtual ~DelegateRC() = default;

  void SetCantDelegateAndCountUses();
  void DelegateStmtRC();
  std::set<OStIdx> RenameAndGetLiveLocalRefVar();
  void CleanUpDeadLocalRefVar(const std::set<OStIdx> &liveLocalrefvars);

 private:
  bool IsCopiedOrDerefedOp(Opcode op) const;
  void CollectVstCantDecrefEarly(MeExpr &opnd0, MeExpr &opnd1);
  void CollectUseCounts(const MeExpr &expr);
  void FindAndDecrUseCount(const VarMeExpr &rhsVar, const MeExpr &expr, int32 &remainingUses) const;
  bool MayThrowException(const MeStmt &stmt) const;
  bool ContainAllTheUses(const VarMeExpr &rhsVar, const MeStmt &fromStmt, const MeStmt &toStmt) const;
  RegMeExpr *RHSTempDelegated(MeExpr &rhs, const MeStmt &useStmt);
  bool FinalRefNoRC(const MeExpr &expr) const;
  void SetCantDelegate(const MapleMap<OStIdx, MePhiNode*> &meVarPhiList);
  void SaveDerefedOrCopiedVst(const MeExpr *expr);
  void CollectDerefedOrCopied(const MeStmt &stmt);
  void CollectDerefedOrCopied(const MeExpr &expr);
  void CollectUsesInfo(const MeExpr &expr);
  bool CanOmitRC4LHSVar(const MeStmt &stmt, bool &onlyWithDecref) const;
  void DelegateHandleNoRCStmt(MeStmt &stmt, bool addDecref);
  void DelegateRCTemp(MeStmt &stmt);
  void RenameDelegatedRefVarUses(MeStmt &meStmt, MeExpr &meExpr);

  MeFunction &func;
  IRMap &irMap;
  SSATab &ssaTab;
  Dominance &dominance;
  MapleAllocator delegateRCAllocator;
  MapleVector<bool> verStCantDelegate;             // true if it has appearance as phi opnd
  MapleVector<uint32> verStUseCounts;              // use counts of each SSA version
  MapleMap<VarMeExpr*, RegMeExpr*> refVar2RegMap;  // map to the replacement preg
  MapleVector<bool> verStDerefedCopied;            // true if it is dereferenced or copied or passed as parameter
  MapleVector<bool> verStCantDecrefEarly;          // true if it is unsafe to insert early decref in form B1 delegation
  bool enabledDebug;
};

class MeDoDelegateRC : public MeFuncPhase {
 public:
  explicit MeDoDelegateRC(MePhaseID id) : MeFuncPhase(id) {}

  virtual ~MeDoDelegateRC() = default;
  AnalysisResult *Run(MeFunction *func, MeFuncResultMgr *m, ModuleResultMgr *mrm) override;
  std::string PhaseName() const override {
    return "delegaterc";
  }
};
}  // namespace maple
#endif  // MAPLE_ME_INCLUDE_MEDELEGATERC_H
