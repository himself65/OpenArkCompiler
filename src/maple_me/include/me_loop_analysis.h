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
#ifndef MAPLE_ME_INCLUDE_MEIDENTLOOPS_H
#define MAPLE_ME_INCLUDE_MEIDENTLOOPS_H
#include "me_function.h"
#include "bb.h"
#include "me_phase.h"
#include "dominance.h"

namespace maple {
class IdentifyLoops;
// describes a specific loop, including the loop head, tail and sets of bb.
struct LoopDesc {
  BB *head;
  BB *tail;
  MapleSet<BBId> loopBBs;
  LoopDesc *parent;  // points to its closest nesting loop
  uint32 nestDepth;  // the nesting depth
  LoopDesc(MapleAllocator *alloc, BB *headBB, BB *tailBB)
      : head(headBB), tail(tailBB), loopBBs(alloc->Adapter()), parent(nullptr), nestDepth(0) {}

  bool Has(const BB *bb) const {
    return loopBBs.find(bb->GetBBId()) != loopBBs.end();
  }
};

// IdentifyLoop records all the loops in a MeFunction.
class IdentifyLoops : public AnalysisResult {
 public:
  IdentifyLoops(MemPool *memPool, MeFunction *mf, Dominance *dm)
      : AnalysisResult(memPool),
        meLoopMemPool(memPool),
        meLoopAlloc(memPool),
        func(mf),
        dominance(dm),
        meLoops(meLoopAlloc.Adapter()),
        bbLoopParent(func->GetAllBBs().size(), nullptr, meLoopAlloc.Adapter()) {}

  virtual ~IdentifyLoops() = default;

  const MapleVector<LoopDesc*> &GetMeLoops() const {
    return meLoops;
  }

  void SetMeLoop(size_t i, LoopDesc &desc) {
    meLoops[i] = &desc;
  }

  LoopDesc *CreateLoopDesc(BB &hd, BB &tail);
  void SetLoopParent4BB(const BB &bb, LoopDesc &loopDesc);
  void ProcessBB(BB *bb);
  void MarkBB();
  void Dump() const;

 private:
  MemPool *meLoopMemPool;
  MapleAllocator meLoopAlloc;
  MeFunction *func;
  Dominance *dominance;
  MapleVector<LoopDesc*> meLoops;
  MapleVector<LoopDesc*> bbLoopParent;  // gives closest nesting loop for each bb
};

class MeDoMeLoop : public MeFuncPhase {
 public:
  explicit MeDoMeLoop(MePhaseID id) : MeFuncPhase(id) {}

  virtual ~MeDoMeLoop() = default;
  AnalysisResult *Run(MeFunction *func, MeFuncResultMgr *m, ModuleResultMgr *mrm) override;
  std::string PhaseName() const override {
    return "identloops";
  }
};
}  // namespace maple
#endif  // MAPLE_ME_INCLUDE_MEIDENTLOOPS_H
