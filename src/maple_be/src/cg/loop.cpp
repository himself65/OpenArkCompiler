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
#include "loop.h"
#include "cg.h"

namespace maplebe {
#define LOOP_ANALYSIS_DUMP CG_DEBUG_FUNC(cgFunc)

static void PrintLoopInfo(const LoopHierarchy &loop) {
  LogInfo::MapleLogger() << "header " << loop.GetHeader()->GetId();
  if (loop.GetOuterLoop() != nullptr) {
    LogInfo::MapleLogger() << " parent " << loop.GetOuterLoop()->GetHeader()->GetId();
  }
  LogInfo::MapleLogger() << " backedge ";
  for (auto *bb : loop.GetBackedge()) {
    LogInfo::MapleLogger() << bb->GetId() << " ";
  }
  LogInfo::MapleLogger() << "\n members ";
  for (auto *bb : loop.GetLoopMembers()) {
    LogInfo::MapleLogger() << bb->GetId() << " ";
  }
  if (!loop.GetInnerLoops().empty()) {
    LogInfo::MapleLogger() << "\n inner_loop_headers ";
    for (auto *inner : loop.GetInnerLoops()) {
      LogInfo::MapleLogger() << inner->GetHeader()->GetId() << " ";
    }
  }
  LogInfo::MapleLogger() << "\n";
}

static void PrintInner(const LoopHierarchy &loop, uint32 level) {
  for (auto *inner : loop.GetInnerLoops()) {
    LogInfo::MapleLogger() << "loop-level-" << level << "\n";
    PrintLoopInfo(*inner);
    PrintInner(*inner, level + 1);
  }
}

void LoopHierarchy::PrintLoops(const std::string &name) const {
  LogInfo::MapleLogger() << name << "\n";
  for (const LoopHierarchy *loop = this; loop != nullptr; loop = loop->next) {
    PrintLoopInfo(*loop);
  }
  for (const LoopHierarchy *loop = this; loop != nullptr; loop = loop->next) {
    PrintInner(*loop, 1);
  }
}

void CGFuncLoops::PrintLoops(const CGFuncLoops &funcLoop) const {
  LogInfo::MapleLogger() << "loop_level(" << funcLoop.loopLevel << ") ";
  LogInfo::MapleLogger() << "header " << funcLoop.GetHeader()->GetId() << " ";
  if (funcLoop.GetOuterLoop() != nullptr) {
    LogInfo::MapleLogger() << "parent " << funcLoop.GetOuterLoop()->GetHeader()->GetId() << " ";
  }
  LogInfo::MapleLogger() << "backedge ";
  for (auto *bb : funcLoop.GetBackedge()) {
    LogInfo::MapleLogger() << bb->GetId() << " ";
  }
  LogInfo::MapleLogger() << "\n members ";
  for (auto *bb : funcLoop.GetLoopMembers()) {
    LogInfo::MapleLogger() << bb->GetId() << " ";
  }
  LogInfo::MapleLogger() << "\n";
  if (!funcLoop.GetInnerLoops().empty()) {
    LogInfo::MapleLogger() << " inner_loop_headers ";
    for (auto *inner : funcLoop.GetInnerLoops()) {
      LogInfo::MapleLogger() << inner->GetHeader()->GetId() << " ";
    }
    LogInfo::MapleLogger() << "\n";
    for (auto *inner : funcLoop.GetInnerLoops()) {
      PrintLoops(*inner);
    }
  }
}

/* backege -> header */
bool LoopFinder::DetectLoopSub(BB &header,  BB &back, std::set<BB*, BBIdCmp> &traversed) {
  bool found = false;
  if (&header == &back) {
    return true;
  }

  traversed.insert(&header);
  for (auto succ : header.GetSuccs()) {
    bool alreadyInLoop = false;
    for (auto candi : candidate) {
      if (candi == succ) {
        alreadyInLoop = true;
        break;
      }
    }
    if (alreadyInLoop) {
      found = true;
      candidate.push_back(succ);
    }
    if (traversed.find(succ) != traversed.end()) {
      continue;
    }
    bool foundSub = DetectLoopSub(*succ, back, traversed);
    if (foundSub) {
      found = true;
      candidate.push_back(succ);
    }
  }

  for (auto ehSucc : header.GetEhSuccs()) {
    bool alreadyInLoop = false;
    for (auto candi : candidate) {
      if (candi == ehSucc) {
        alreadyInLoop = true;
        break;
      }
    }
    if (alreadyInLoop) {
      found = true;
      candidate.push_back(ehSucc);
    }
    if (traversed.find(ehSucc) != traversed.end()) {
      continue;
    }
    bool foundSub = DetectLoopSub(*ehSucc, back, traversed);
    if (foundSub) {
      found = true;
      candidate.push_back(ehSucc);
    }
  }
  return found;
}

/* backege -> header */
void LoopFinder::DetectLoop(BB &header, BB &back) {
  std::set<BB*, BBIdCmp> traversed;
  traversed.insert(&header);
  candidate.push_back(&header);
  candidate.push_back(&back);

  for (auto succ : header.GetSuccs()) {
    if (traversed.find(succ) != traversed.end()) {
      continue;
    }
    bool found = DetectLoopSub(*succ, back, traversed);
    if (found) {
      candidate.push_back(succ);
    }
  }

  for (auto ehSucc : header.GetEhSuccs()) {
    if (traversed.find(ehSucc) != traversed.end()) {
      continue;
    }
    bool found = DetectLoopSub(*ehSucc, back, traversed);
    if (found) {
      candidate.push_back(ehSucc);
    }
  }

  LoopHierarchy *simpleLoop = memPool->New<LoopHierarchy>(*memPool);
  for (auto *bb : candidate) {
    simpleLoop->InsertLoopMembers(*bb);
  }
  candidate.clear();
  simpleLoop->SetHeader(header);
  simpleLoop->InsertBackedge(back);

  if (loops != nullptr) {
    loops->SetPrev(simpleLoop);
  }
  simpleLoop->SetNext(loops);
  loops = simpleLoop;
}

void LoopFinder::FindBackedge() {
  while (!dfsBBs.empty()) {
    bool childPushed = false;
    BB *bb = dfsBBs.top();
    dfsBBs.pop();
    CHECK_FATAL(bb != nullptr, "bb is null in LoopFinder::FindBackedge");
    visitedBBs[bb->GetId()] = true;
    if (bb->GetLevel() == 0) {
      bb->SetLevel(1);
    }
    std::stack<BB*> succs;
    /* Mimic more of the recursive DFS by reversing the order of the succs. */
    for (auto *succBB : bb->GetSuccs()) {
      succs.push(succBB);
    }
    PushBackedge(*bb, succs, childPushed);
    for (auto *ehSuccBB : bb->GetEhSuccs()) {
      succs.push(ehSuccBB);
    }
    PushBackedge(*bb, succs, childPushed);
    /* Remove duplicate bb that are visited from top of stack */
    if (!dfsBBs.empty()) {
      BB *nextBB = dfsBBs.top();
      while (nextBB != nullptr) {
        if (visitedBBs[nextBB->GetId()]) {
          dfsBBs.pop();
        } else {
          break;
        }
        if (!dfsBBs.empty()) {
          nextBB = dfsBBs.top();
        } else {
          break;
        }
      }
    }
    if (!childPushed && !dfsBBs.empty()) {
      /* reached the bottom of visited chain, reset level to the next visit bb */
      bb->SetLevel(0);
      BB *nextBB = dfsBBs.top();
      if (sortedBBs[nextBB->GetId()]) {
        nextBB = sortedBBs[nextBB->GetId()];
        /* All bb up to the top of stack bb's parent's child (its sibling) */
        while (1) {
          /* get parent bb */
          BB *parentBB = sortedBBs[bb->GetId()];
          if ((parentBB == nullptr) || (parentBB == nextBB)) {
            break;
          }
          parentBB->SetLevel(0);
          bb = parentBB;
        }
      }
    }
  }
}

void LoopFinder::PushBackedge(BB &bb, std::stack<BB*> &succs, bool &childPushed) {
  while (!succs.empty()) {
    BB *succBB = succs.top();
    ASSERT(succBB != nullptr, "unexpected null bb in LoopFinder::PushBackedge");
    succs.pop();
    if (!visitedBBs[succBB->GetId()]) {
      childPushed = true;
      succBB->SetLevel(bb.GetLevel() + 1);
      sortedBBs[succBB->GetId()] = &bb;  /* tracking parent of traversed child */
      dfsBBs.push(succBB);
    } else if ((succBB->GetLevel() != 0) && (bb.GetLevel() >= succBB->GetLevel())) {
      /* Backedge bb -> succBB */
      DetectLoop(*succBB, bb);
      bb.PushBackLoopSuccs(*succBB);
      succBB->PushBackLoopPreds(bb);
    }
  }
}

void LoopFinder::MergeLoops() {
  for (LoopHierarchy *loopHierarchy1 = loops; loopHierarchy1 != nullptr; loopHierarchy1 = loopHierarchy1->GetNext()) {
    for (LoopHierarchy *loopHierarchy2 = loopHierarchy1->GetNext(); loopHierarchy2 != nullptr;
         loopHierarchy2 = loopHierarchy2->GetNext()) {
      if (loopHierarchy1->GetHeader() != loopHierarchy2->GetHeader()) {
        continue;
      }
      for (auto *bb : loopHierarchy2->GetLoopMembers()) {
        loopHierarchy1->InsertLoopMembers(*bb);
      }
      for (auto *bb : loopHierarchy2->GetBackedge()) {
        loopHierarchy1->InsertBackedge(*bb);
      }
      loopHierarchy2->GetPrev()->SetNext(loopHierarchy2->GetNext());
      if (loopHierarchy2->GetNext() != nullptr) {
        loopHierarchy2->GetNext()->SetPrev(loopHierarchy2->GetPrev());
      }
    }
  }
}

void LoopFinder::SortLoops() {
  LoopHierarchy *head = nullptr;
  LoopHierarchy *next1 = nullptr;
  LoopHierarchy *next2 = nullptr;
  bool swapped;
  do {
    swapped = false;
    for (LoopHierarchy *loopHierarchy1 = loops; loopHierarchy1 != nullptr;) {
      /* remember loopHierarchy1's prev in case if loopHierarchy1 moved */
      head = loopHierarchy1;
      next1 = loopHierarchy1->GetNext();
      for (LoopHierarchy *loopHierarchy2 = loopHierarchy1->GetNext(); loopHierarchy2 != nullptr;) {
        next2 = loopHierarchy2->GetNext();

        if (loopHierarchy1->GetLoopMembers().size() > loopHierarchy2->GetLoopMembers().size()) {
          if (head->GetPrev() == nullptr) {
            /* remove loopHierarchy2 from list */
            loopHierarchy2->GetPrev()->SetNext(loopHierarchy2->GetNext());
            if (loopHierarchy2->GetNext() != nullptr) {
              loopHierarchy2->GetNext()->SetPrev(loopHierarchy2->GetPrev());
            }
            /* link loopHierarchy2 as head */
            loops = loopHierarchy2;
            loopHierarchy2->SetPrev(nullptr);
            loopHierarchy2->SetNext(head);
            head->SetPrev(loopHierarchy2);
          } else {
            loopHierarchy2->GetPrev()->SetNext(loopHierarchy2->GetNext());
            if (loopHierarchy2->GetNext() != nullptr) {
              loopHierarchy2->GetNext()->SetPrev(loopHierarchy2->GetPrev());
            }
            head->GetPrev()->SetNext(loopHierarchy2);
            loopHierarchy2->SetPrev(head->GetPrev());
            loopHierarchy2->SetNext(head);
            head->SetPrev(loopHierarchy2);
          }
          head = loopHierarchy2;
          swapped = true;
        }
        loopHierarchy2 = next2;
      }
      loopHierarchy1 = next1;
    }
  } while (swapped);
}

void LoopFinder::CreateInnerLoop(LoopHierarchy &inner, LoopHierarchy &outer) {
  outer.InsertInnerLoops(inner);
  inner.SetOuterLoop(outer);
  MapleSet<BB*, BBIdCmp>::iterator ito;
  for (auto *bb : inner.GetLoopMembers()) {
    for (ito = outer.GetLoopMembers().begin(); ito != outer.GetLoopMembers().end();) {
      BB *rm = *ito;
      if (rm == bb) {
        ito = outer.EraseLoopMembers(ito);
      } else {
        ++ito;
      }
    }
  }
  if (loops == &inner) {
    loops = inner.GetNext();
  } else {
    LoopHierarchy *prev = loops;
    for (LoopHierarchy *loopHierarchy1 = loops->GetNext(); loopHierarchy1 != nullptr;
         loopHierarchy1 = loopHierarchy1->GetNext()) {
      if (loopHierarchy1 == &inner) {
        prev->SetNext(prev->GetNext()->GetNext());
      }
      prev = loopHierarchy1;
    }
  }
}

void LoopFinder::DetectInnerLoop() {
  bool innerCreated;
  do {
    innerCreated = false;
    for (LoopHierarchy *loopHierarchy1 = loops; loopHierarchy1 != nullptr;
         loopHierarchy1 = loopHierarchy1->GetNext()) {
      for (LoopHierarchy *loopHierarchy2 = loopHierarchy1->GetNext(); loopHierarchy2 != nullptr;
           loopHierarchy2 = loopHierarchy2->GetNext()) {
        if (loopHierarchy1->GetHeader() != loopHierarchy2->GetHeader()) {
          for (auto *bb : loopHierarchy2->GetLoopMembers()) {
            if (loopHierarchy1->GetHeader() != bb) {
              continue;
            }
            CreateInnerLoop(*loopHierarchy1, *loopHierarchy2);
            innerCreated = true;
            break;
          }
          if (innerCreated) {
            break;
          }
        }
      }
      if (innerCreated) {
        break;
      }
    }
  } while (innerCreated);
}

static void CopyLoopInfo(LoopHierarchy &from, CGFuncLoops &to, CGFuncLoops *parent, MemPool &memPool) {
  to.SetHeader(*const_cast<BB*>(from.GetHeader()));
  for (auto *bb : from.GetLoopMembers()) {
    to.AddLoopMembers(*bb);
    bb->SetLoop(to);
  }
  for (auto *bb : from.GetBackedge()) {
    to.AddBackedge(*bb);
  }
  if (!from.GetInnerLoops().empty()) {
    for (auto *inner : from.GetInnerLoops()) {
      CGFuncLoops *floop = memPool.New<CGFuncLoops>(memPool);
      to.AddInnerLoops(*floop);
      floop->SetLoopLevel(to.GetLoopLevel() + 1);
      CopyLoopInfo(*inner, *floop, &to, memPool);
    }
  }
  if (parent != nullptr) {
    to.SetOuterLoop(*parent);
  }
}

void LoopFinder::UpdateCGFunc() {
  for (LoopHierarchy *loop = loops; loop != nullptr; loop = loop->GetNext()) {
    CGFuncLoops *floop = cgFunc->GetMemoryPool()->New<CGFuncLoops>(*cgFunc->GetMemoryPool());
    cgFunc->PushBackLoops(*floop);
    floop->SetLoopLevel(1);    /* top level */
    CopyLoopInfo(*loop, *floop, nullptr, *cgFunc->GetMemoryPool());
  }
}

void LoopFinder::FormLoopHierarchy() {
  visitedBBs.clear();
  visitedBBs.resize(cgFunc->NumBBs(), false);
  sortedBBs.clear();
  sortedBBs.resize(cgFunc->NumBBs(), nullptr);
  FOR_ALL_BB(bb, cgFunc) {
    bb->SetLevel(0);
  }
  bool changed;
  do {
    changed = false;
    FOR_ALL_BB(bb, cgFunc) {
      if (!visitedBBs[bb->GetId()]) {
        dfsBBs.push(bb);
        FindBackedge();
        changed = true;
      }
    }
  } while (changed);
  /*
   * FIX : Should merge the partial loops at the time of initial
   * construction.  And make the linked list as a sorted set,
   * then the merge and sort phases below can go away.
   *
   * Start merging the loops with the same header
   */
  MergeLoops();
  /* order loops from least number of members */
  SortLoops();
  DetectInnerLoop();
  UpdateCGFunc();
}

AnalysisResult *CgDoLoopAnalysis::Run(CGFunc *cgFunc, CgFuncResultMgr *cgFuncResultMgr) {
  (void)cgFuncResultMgr;
  CHECK_FATAL(cgFunc != nullptr, "nullptr check");
  cgFunc->ClearLoopInfo();
  MemPool *loopMemPool = NewMemPool();
  LoopFinder *loopFinder = loopMemPool->New<LoopFinder>(*cgFunc, *loopMemPool);
  loopFinder->FormLoopHierarchy();

  return loopFinder;
}
}  /* namespace maplebe */
