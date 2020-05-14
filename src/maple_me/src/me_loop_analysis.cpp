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
#include "me_loop_analysis.h"

// This phase analyses the CFG and identify the loops. The implementation is
// based on the idea that, given two basic block a and b, if b is a's pred and
// a dominates b, then there is a loop from a to b. Loop identification is done
// in a preorder traversal of the dominator tree. In this order, outer loop is
// always detected before its nested loop(s). The building of the LoopDesc data
// structure takes advantage of this ordering.
namespace maple {
LoopDesc *IdentifyLoops::CreateLoopDesc(BB &hd, BB &tail) {
  LoopDesc *newLoop = meLoopMemPool->New<LoopDesc>(meLoopAlloc, &hd, &tail);
  meLoops.push_back(newLoop);
  return newLoop;
}

void IdentifyLoops::SetLoopParent4BB(const BB &bb, LoopDesc &loopDesc) {
  if (bbLoopParent[bb.GetBBId()] != nullptr) {
    if (loopDesc.parent == nullptr) {
      loopDesc.parent = bbLoopParent[bb.GetBBId()];
      ASSERT_NOT_NULL(loopDesc.parent);
      loopDesc.nestDepth = loopDesc.parent->nestDepth + 1;
    }
  }
  bbLoopParent[bb.GetBBId()] = &loopDesc;
}

void IdentifyLoops::InsertExitBB(LoopDesc &loop) {
  std::set<BB*> traveledBBs;
  std::queue<BB*> inLoopBBs;
  inLoopBBs.push(loop.head);
  CHECK_FATAL(loop.inloopBB2exitBBs.empty(), "inloopBB2exitBBs must be empty");
  while (!inLoopBBs.empty()) {
    BB *curBB = inLoopBBs.front();
    inLoopBBs.pop();
    for (BB *succ : curBB->GetSucc()) {
      if (traveledBBs.count(succ) != 0) {
        continue;
      }
      if (loop.Has(*succ)) {
        inLoopBBs.push(succ);
        traveledBBs.insert(succ);
      } else {
        loop.InsertInloopBB2exitBBs(*curBB, *succ);
      }
      if (curBB->GetKind() == kBBCondGoto) {
        if (curBB->GetSucc().size() == 1) {
          // When the size of succs is one, one of succs may be commonExitBB. Need insert to loopBB2exitBBs.
          CHECK_FATAL(false, "return bb");
        }
      } else if (!curBB->GetStmtNodes().empty() && curBB->GetLast().GetOpCode() == OP_return) {
        CHECK_FATAL(false, "return bb");
      }
    }
  }
  for (auto pair : loop.inloopBB2exitBBs) {
    MapleVector<BB*> *succBB = pair.second;
    for (auto it : *succBB) {
      for (auto pred : it->GetPred()) {
        if (!loop.Has(*pred)) {
          loop.inloopBB2exitBBs.clear();
          return;
        }
      }
    }
  }
}

// process each BB in preorder traversal of dominator tree
void IdentifyLoops::ProcessBB(BB *bb) {
  if (bb == nullptr || bb == func.GetCommonExitBB()) {
    return;
  }
  for (BB *pred : bb->GetPred()) {
    if (dominance->Dominate(*bb, *pred)) {
      // create a loop with bb as loop head and pred as loop tail
      LoopDesc *loop = CreateLoopDesc(*bb, *pred);
      std::list<BB*> bodyList;
      bodyList.push_back(pred);
      while (!bodyList.empty()) {
        BB *curr = bodyList.front();
        bodyList.pop_front();
        // skip bb or if it has already been dealt with
        if (curr == bb || loop->loopBBs.count(curr->GetBBId()) == 1) {
          continue;
        }
        (void)loop->loopBBs.insert(curr->GetBBId());
        SetLoopParent4BB(*curr, *loop);
        for (BB *curPred : curr->GetPred()) {
          bodyList.push_back(curPred);
        }
      }
      (void)loop->loopBBs.insert(bb->GetBBId());
      SetLoopParent4BB(*bb, *loop);
    }
  }
  // recursive call
  const MapleSet<BBId> &domChildren = dominance->GetDomChildren(bb->GetBBId());
  for (auto bbIt = domChildren.begin(); bbIt != domChildren.end(); ++bbIt) {
    ProcessBB(func.GetAllBBs().at(*bbIt));
  }
}

void IdentifyLoops::Dump() const {
  for (LoopDesc *meLoop : meLoops) {
    // loop
    LogInfo::MapleLogger() << "nest depth: " << meLoop->nestDepth << " loop head BB: " << meLoop->head->GetBBId()
                           << " tail BB:" << meLoop->tail->GetBBId() << '\n';
    LogInfo::MapleLogger() << "loop body:";
    for (auto it = meLoop->loopBBs.begin(); it != meLoop->loopBBs.end(); ++it) {
      BBId bbId = *it;
      LogInfo::MapleLogger() << bbId << " ";
    }
    LogInfo::MapleLogger() << '\n';
  }
}

void IdentifyLoops::MarkBB() {
  for (LoopDesc *meLoop : meLoops) {
    for (BBId bbId : meLoop->loopBBs) {
      if (func.GetAllBBs().at(bbId) == nullptr) {
        continue;
      }
      func.GetAllBBs().at(bbId)->SetAttributes(kBBAttrIsInLoopForEA);
    }
  }
}

void IdentifyLoops::SetTryBB() {
  for (auto loop : meLoops) {
    for (auto pred : loop->head->GetPred()) {
      if (pred->GetAttributes(kBBAttrIsTry)) {
        loop->SetHasTryBB(true);
        break;
      }
    }
    if (loop->HasTryBB()) {
      continue;
    }
    for (auto bbId : loop->loopBBs) {
      BB *bb = func.GetBBFromID(bbId);
      if (bb->GetAttributes(kBBAttrIsTry) || bb->GetAttributes(kBBAttrWontExit)) {
        loop->SetHasTryBB(true);
        break;
      }
    }
  }
}

bool IdentifyLoops::ProcessPreheaderAndLatch(LoopDesc &loop) {
  // If predsize of head is one, it means that one is entry bb.
  if (loop.head->GetPred().size() == 1) {
    CHECK_FATAL(func.GetCommonEntryBB()->GetSucc(0) == loop.head, "succ of entry bb must be head");
    loop.preheader = func.GetCommonEntryBB();
    CHECK_FATAL(!loop.head->GetPred(0)->GetAttributes(kBBAttrIsTry), "must not be kBBAttrIsTry");
    loop.latch = loop.head->GetPred(0);
    return true;
  }
  /* for example: GetInstance.java : 152
   * There are two loop in identifyLoops, and one has no try no catch.
   * In loop canon whould ont merge loops with latch bb.
   * for () {
   *   if () {
   *     do somthing
   *     continue
   *   }
   *   try {
   *     do somthing
   *   } catch (NoSuchAlgorithmException e) {
   *     do somthing
   *   }
   * }
   */
  if (loop.head->GetPred().size() != 2) { // Head must has two preds.
    loop.SetIsCanonicalLoop(false);
    loop.SetHasTryBB(true);
    return false;
  }
  if (!loop.Has(*loop.head->GetPred(0))) {
    loop.preheader = loop.head->GetPred(0);
    CHECK_FATAL(loop.preheader->GetKind() == kBBFallthru, "must be kBBFallthru");
    CHECK_FATAL(loop.Has(*loop.head->GetPred(1)), "must be latch bb");
    loop.latch = loop.head->GetPred(1);
    CHECK_FATAL(!loop.latch->GetAttributes(kBBAttrIsTry), "must not be kBBAttrIsTry");
    return true;
  } else {
    loop.latch = loop.head->GetPred(0);
    CHECK_FATAL(!loop.latch->GetAttributes(kBBAttrIsTry), "must not be kBBAttrIsTry");
    CHECK_FATAL(!loop.Has(*loop.head->GetPred(1)), "must be latch preheader bb");
    loop.preheader = loop.head->GetPred(1);
    CHECK_FATAL(loop.preheader->GetKind() == kBBFallthru, "must be kBBFallthru");
    return true;
  }
}

AnalysisResult *MeDoMeLoop::Run(MeFunction *func, MeFuncResultMgr *m, ModuleResultMgr*) {
  auto *dom = static_cast<Dominance*>(m->GetAnalysisResult(MeFuncPhase_DOMINANCE, func));
  ASSERT(dom != nullptr, "dominance phase has problem");
  MemPool *meLoopMp = NewMemPool();
  IdentifyLoops *identLoops = meLoopMp->New<IdentifyLoops>(meLoopMp, *func, dom);
  identLoops->ProcessBB(func->GetCommonEntryBB());
  identLoops->SetTryBB();
  for (auto loop : identLoops->GetMeLoops()) {
    if (loop->HasTryBB()) {
      continue;
    }
    if (!identLoops->ProcessPreheaderAndLatch(*loop)) {
      continue;
    }
    identLoops->InsertExitBB(*loop);
    loop->SetIsCanonicalLoop(loop->inloopBB2exitBBs.size() == 0 ? false : true);
  }
  if (DEBUGFUNC(func)) {
    identLoops->Dump();
  }
  return identLoops;
}
}  // namespace maple
