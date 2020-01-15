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

// This phase analyses the CFG and identify the loops.  The implementation is
// based on the idea that, given two basic block a and b, if b is a's pred and
// a dominates b, then there is a loop from a to b.  Loop identification is done
// in a preorder traversal of the dominator tree.  In this order, outer loop is
// always detected before its nested loop(s).  The building of the LoopDesc data
// structure takes advantage of this ordering.
namespace maple {
LoopDesc *IdentifyLoops::CreateLoopDesc(BB *hd, BB *tail) {
  LoopDesc *newLoop = meLoopMemPool->New<LoopDesc>(&meLoopAlloc, hd, tail);
  meLoops.push_back(newLoop);
  return newLoop;
}

void IdentifyLoops::SetLoopParent4BB(const BB *bb, LoopDesc *loop) {
  if (bbLoopParent[bb->GetBBId()] != nullptr) {
    if (loop->parent == nullptr) {
      loop->parent = bbLoopParent[bb->GetBBId()];
      loop->nestDepth = loop->parent->nestDepth + 1;
    }
  }
  bbLoopParent[bb->GetBBId()] = loop;
}

// process each BB in preorder traversal of dominator tree
void IdentifyLoops::ProcessBB(BB *bb) {
  if (bb == nullptr || bb == func->GetCommonExitBB()) {
    return;
  }
  for (BB *pred : bb->GetPred()) {
    if (dominance->Dominate(*bb, *pred)) {
      // create a loop with bb as loop head and pred as loop tail
      LoopDesc *loop = CreateLoopDesc(bb, pred);
      std::list<BB*> bodyList;
      bodyList.push_back(pred);
      while (!bodyList.empty()) {
        BB *curr = bodyList.front();
        bodyList.pop_front();
        // skip bb or if it has already been dealt with
        if (curr == bb || loop->loopBBs.count(curr->GetBBId()) == 1) {
          continue;
        }
        loop->loopBBs.insert(curr->GetBBId());
        SetLoopParent4BB(curr, loop);
        for (BB *curPred : curr->GetPred()) {
          bodyList.push_back(curPred);
        }
      }
      loop->loopBBs.insert(bb->GetBBId());
      SetLoopParent4BB(bb, loop);
    }
  }
  // recursive call
  const MapleSet<BBId> &domChildren = dominance->GetDomChildren(bb->GetBBId());
  for (auto bbIt = domChildren.begin(); bbIt != domChildren.end(); bbIt++) {
    ProcessBB(func->GetAllBBs().at(*bbIt));
  }
}

void IdentifyLoops::Dump() {
  for (LoopDesc *mploop : meLoops) {
    // loop
    LogInfo::MapleLogger() << "nest depth: " << mploop->nestDepth << " loop head BB: " << mploop->head->GetBBId()
                           << " tail BB:" << mploop->tail->GetBBId() << '\n';
    LogInfo::MapleLogger() << "loop body:";
    for (auto it = mploop->loopBBs.begin(); it != mploop->loopBBs.end(); it++) {
      BBId bbId = *it;
      LogInfo::MapleLogger() << bbId << " ";
    }
    LogInfo::MapleLogger() << '\n';
  }
}

void IdentifyLoops::MarkBB() {
  for (LoopDesc *mploop : meLoops) {
    for (BBId bbId : mploop->loopBBs) {
      if (func->GetAllBBs().at(bbId) == nullptr) {
        continue;
      }
      func->GetAllBBs().at(bbId)->SetAttributes(kBBAttrIsInLoopForEA);
    }
  }
}

AnalysisResult *MeDoMeLoop::Run(MeFunction *func, MeFuncResultMgr *m, ModuleResultMgr *mrm) {
  auto *dom = static_cast<Dominance*>(m->GetAnalysisResult(MeFuncPhase_DOMINANCE, func));
  ASSERT(dom, "dominance phase has problem");
  MemPool *meLoopMp = NewMemPool();
  IdentifyLoops *identLoops = meLoopMp->New<IdentifyLoops>(meLoopMp, func, dom);
  identLoops->ProcessBB(func->GetCommonEntryBB());
  if (DEBUGFUNC(func)) {
    identLoops->Dump();
  }
  return identLoops;
}
}  // namespace maple
