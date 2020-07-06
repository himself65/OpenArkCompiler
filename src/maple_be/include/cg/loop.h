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
#ifndef MAPLEBE_INCLUDE_CG_LOOP_H
#define MAPLEBE_INCLUDE_CG_LOOP_H

#include "cg_phase.h"
#include "cgbb.h"
#include "insn.h"

namespace maplebe {
class LoopHierarchy {
 public:
  struct HeadIDCmp {
    bool operator()(const LoopHierarchy *loopHierarchy1, const LoopHierarchy *loopHierarchy2) const {
      CHECK_NULL_FATAL(loopHierarchy1);
      CHECK_NULL_FATAL(loopHierarchy2);
      return (loopHierarchy1->GetHeader()->GetId() < loopHierarchy2->GetHeader()->GetId());
    }
  };

  explicit LoopHierarchy(MemPool &memPool)
      : loopMemPool(&memPool),
        loopMembers(loopMemPool.Adapter()),
        backedge(loopMemPool.Adapter()),
        innerLoops(loopMemPool.Adapter()) {}

  virtual ~LoopHierarchy() = default;

  const BB *GetHeader() const {
    return header;
  }
  const MapleSet<BB*, BBIdCmp> &GetLoopMembers() const {
    return loopMembers;
  }
  const MapleSet<BB*, BBIdCmp> &GetBackedge() const {
    return backedge;
  }
  const MapleSet<LoopHierarchy*, HeadIDCmp> &GetInnerLoops() const {
    return innerLoops;
  }
  const LoopHierarchy *GetOuterLoop() const {
    return outerLoop;
  }
  LoopHierarchy *GetPrev() {
    return prev;
  }
  LoopHierarchy *GetNext() {
    return next;
  }

  MapleSet<BB*, BBIdCmp>::iterator EraseLoopMembers(MapleSet<BB*, BBIdCmp>::iterator it) {
    return loopMembers.erase(it);
  }
  void InsertLoopMembers(BB &bb) {
    loopMembers.insert(&bb);
  }
  void InsertBackedge(BB &bb) {
    backedge.insert(&bb);
  }
  void InsertInnerLoops(LoopHierarchy &loop) {
    innerLoops.insert(&loop);
  }
  void SetHeader(BB &bb) {
    header = &bb;
  }
  void SetOuterLoop(LoopHierarchy &loop) {
    outerLoop = &loop;
  }
  void SetPrev(LoopHierarchy *loop) {
    prev = loop;
  }
  void SetNext(LoopHierarchy *loop) {
    next = loop;
  }
  void PrintLoops(const std::string &name) const;

 protected:
  LoopHierarchy *prev = nullptr;
  LoopHierarchy *next = nullptr;

 private:
  MapleAllocator loopMemPool;
  BB *header = nullptr;
  MapleSet<BB*, BBIdCmp> loopMembers;
  MapleSet<BB*, BBIdCmp> backedge;
  MapleSet<LoopHierarchy*, HeadIDCmp> innerLoops;
  LoopHierarchy *outerLoop = nullptr;
};

class LoopFinder : public AnalysisResult {
 public:
  LoopFinder(CGFunc &func, MemPool &mem)
      : AnalysisResult(&mem),
        cgFunc(&func),
        memPool(&mem),
        loopMemPool(memPool),
        candidate(loopMemPool.Adapter()),
        visitedBBs(loopMemPool.Adapter()),
        sortedBBs(loopMemPool.Adapter()),
        dfsBBs(loopMemPool.Adapter()) {}

  ~LoopFinder() override = default;

  void DetectLoop(BB &header, BB &back);
  bool DetectLoopSub(BB &header, BB &back, std::set<BB*, BBIdCmp> &traversed);
  void FindBackedge();
  void PushBackedge(BB &bb, std::stack<BB*> &succs, bool &childPushed);
  void MergeLoops();
  void SortLoops();
  void CreateInnerLoop(LoopHierarchy &inner, LoopHierarchy &outer);
  void DetectInnerLoop();
  void UpdateCGFunc();
  void FormLoopHierarchy();

 private:
  CGFunc *cgFunc;
  MemPool *memPool;
  MapleAllocator loopMemPool;
  MapleList<BB*> candidate;  /* loop candidate */
  MapleVector<bool> visitedBBs;
  MapleVector<BB*> sortedBBs;
  MapleStack<BB*> dfsBBs;
  LoopHierarchy *loops = nullptr;
};

class CGFuncLoops {
 public:
  explicit CGFuncLoops(MemPool &memPool)
      : loopMemPool(&memPool),
        loopMembers(loopMemPool.Adapter()),
        backedge(loopMemPool.Adapter()),
        innerLoops(loopMemPool.Adapter()) {}

  ~CGFuncLoops() = default;

  void PrintLoops(const CGFuncLoops &loops) const;

  const BB *GetHeader() const {
    return header;
  }
  const MapleVector<BB*> &GetLoopMembers() const {
    return loopMembers;
  }
  const MapleVector<BB*> &GetBackedge() const {
    return backedge;
  }
  const MapleVector<CGFuncLoops*> &GetInnerLoops() const {
    return innerLoops;
  }
  const CGFuncLoops *GetOuterLoop() const {
    return outerLoop;
  }
  uint32 GetLoopLevel() const {
    return loopLevel;
  }

  void AddLoopMembers(BB &bb) {
    loopMembers.emplace_back(&bb);
  }
  void AddBackedge(BB &bb) {
    backedge.emplace_back(&bb);
  }
  void AddInnerLoops(CGFuncLoops &loop) {
    innerLoops.emplace_back(&loop);
  }
  void SetHeader(BB &bb) {
    header = &bb;
  }
  void SetOuterLoop(CGFuncLoops &loop) {
    outerLoop = &loop;
  }
  void SetLoopLevel(uint32 val) {
    loopLevel = val;
  }

 private:
  MapleAllocator loopMemPool;
  BB *header = nullptr;
  MapleVector<BB*> loopMembers;
  MapleVector<BB*> backedge;
  MapleVector<CGFuncLoops*> innerLoops;
  CGFuncLoops *outerLoop = nullptr;
  uint32 loopLevel = 0;
};

struct CGFuncLoopCmp {
  bool operator()(const CGFuncLoops *lhs, const CGFuncLoops *rhs) const {
    CHECK_NULL_FATAL(lhs);
    CHECK_NULL_FATAL(rhs);
    return lhs->GetHeader()->GetId() < rhs->GetHeader()->GetId();
  }
};

CGFUNCPHASE(CgDoLoopAnalysis, "loopanalysis")
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_LOOP_H */