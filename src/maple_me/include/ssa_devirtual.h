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
#ifndef MAPLE_ME_INCLUDE_SSADEVIRTUAL_H
#define MAPLE_ME_INCLUDE_SSADEVIRTUAL_H
#include "me_ir.h"
#include "me_phase.h"
#include "me_irmap.h"
#include "dominance.h"
#include "class_hierarchy.h"
#include "clone.h"

namespace maple {
class SSADevirtual {
 public:
  static bool debug;
  SSADevirtual(MemPool *memPool, MIRModule *currMod, IRMap *irMap, KlassHierarchy *currKh,
               Dominance *currDom, size_t bbVecSize, Clone *currClone)
      : devirtualAlloc(memPool),
        mod(currMod),
        irMap(irMap),
        kh(currKh),
        dom(currDom),
        bbVisited(bbVecSize, false, devirtualAlloc.Adapter()),
        clone(currClone),
        retTy(kNotSeen),
        inferredRetTyIdx(0),
        totalVirtualCalls(0),
        optedVirtualCalls(0),
        totalInterfaceCalls(0),
        optedInterfaceCalls(0),
        nullCheckCount(0) {}

  virtual ~SSADevirtual() = default;

  void Perform(BB *entryBB);

 protected:
  virtual MIRFunction *GetMIRFunction() {
    return nullptr;
  }

  virtual BB *GetBB(BBId id) = 0;
  void TraversalBB(BB*);
  void TraversalMeStmt(MeStmt *Stmt);
  void VisitVarPhiNode(MeVarPhiNode*);
  void VisitMeExpr(MeExpr*);
  void PropVarInferredType(VarMeExpr*);
  void PropIvarInferredType(IvarMeExpr*);
  void ReturnTyIdxInferring(const RetMeStmt*);
  bool NeedNullCheck(MeExpr*) const;
  void InsertNullCheck(CallMeStmt*, MeExpr*);
  bool DevirtualizeCall(CallMeStmt*);
  void SSADevirtualize(CallNode *stmt);
  void ReplaceCall(CallMeStmt*, MIRFunction*);
  TyIdx GetInferredTyIdx(MeExpr *expr);

 private:
  MapleAllocator devirtualAlloc;
  MIRModule *mod;
  IRMap *irMap;
  KlassHierarchy *kh;
  Dominance *dom;
  MapleVector<bool> bbVisited;  // needed because dominator tree is a DAG in wpo
  Clone *clone;
  enum TagRetTyIdx {
    kNotSeen,
    kSeen,
    kFailed
  } retTy;
  TyIdx inferredRetTyIdx;
  unsigned int totalVirtualCalls;
  unsigned int optedVirtualCalls;
  unsigned int totalInterfaceCalls;
  unsigned int optedInterfaceCalls;
  unsigned int nullCheckCount;
};
}  // namespace maple
#endif  // MAPLE_ME_INCLUDE_SSADEVIRTUAL_H
