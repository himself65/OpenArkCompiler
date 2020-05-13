/*
 * Copyright (c) [2019-2020] Huawei Technologies Co.,Ltd.All rights reserved.
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
#include "dominance.h"
#include <iostream>

/* ================= for Dominance ================= */
namespace maple {
void Dominance::PostOrderWalk(const BB &bb, int32 &pid, std::vector<bool> &visitedMap) {
  ASSERT(bb.GetBBId() < visitedMap.size(), "index out of range in Dominance::PostOrderWalk");
  if (visitedMap[bb.GetBBId()]) {
    return;
  }
  visitedMap[bb.GetBBId()] = true;
  for (const BB *suc : bb.GetSucc()) {
    PostOrderWalk(*suc, pid, visitedMap);
  }
  ASSERT(bb.GetBBId() < postOrderIDVec.size(), "index out of range in Dominance::PostOrderWalk");
  postOrderIDVec[bb.GetBBId()] = pid++;
}

void Dominance::GenPostOrderID() {
  ASSERT(!bbVec.empty(), "size to be allocated is 0");
  std::vector<bool> visitedMap(bbVec.size(), false);
  int32 postOrderID = 0;
  PostOrderWalk(commonEntryBB, postOrderID, visitedMap);
  // initialize reversePostOrder
  int32 maxPostOrderID = postOrderID - 1;
  reversePostOrder.resize(maxPostOrderID + 1);
  for (size_t i = 0; i < postOrderIDVec.size(); ++i) {
    int32 postOrderNo = postOrderIDVec[i];
    if (postOrderNo == -1) {
      continue;
    }
    reversePostOrder[maxPostOrderID - postOrderNo] = bbVec[i];
  }
}

BB *Dominance::Intersect(BB &bb1, const BB &bb2) const {
  auto *ptrBB1 = &bb1;
  auto *ptrBB2 = &bb2;
  while (ptrBB1 != ptrBB2) {
    while (postOrderIDVec[ptrBB1->GetBBId()] < postOrderIDVec[ptrBB2->GetBBId()]) {
      ptrBB1 = doms.at(ptrBB1->GetBBId());
    }
    while (postOrderIDVec[ptrBB2->GetBBId()] < postOrderIDVec[ptrBB1->GetBBId()]) {
      ptrBB2 = doms.at(ptrBB2->GetBBId());
    }
  }
  return ptrBB1;
}

bool Dominance::CommonEntryBBIsPred(const BB &bb) const {
  for (const BB *suc : GetCommonEntryBB().GetSucc()) {
    if (suc == &bb) {
      return true;
    }
  }
  return false;
}

// Figure 3 in "A Simple, Fast Dominance Algorithm" by Keith Cooper et al.
void Dominance::ComputeDominance() {
  doms[commonEntryBB.GetBBId()] = &commonEntryBB;
  bool changed;
  do {
    changed = false;
    for (size_t i = 1; i < reversePostOrder.size(); ++i) {
      BB *bb = reversePostOrder[i];
      BB *pre = nullptr;
      if (CommonEntryBBIsPred(*bb) || bb->GetPred().empty()) {
        pre = &commonEntryBB;
      } else {
        pre = bb->GetPred(0);
      }
      size_t j = 1;
      while ((doms[pre->GetBBId()] == nullptr || pre == bb) && j < bb->GetPred().size()) {
        pre = bb->GetPred(j);
        ++j;
      }
      BB *newIDom = pre;
      for (; j < bb->GetPred().size(); ++j) {
        pre = bb->GetPred(j);
        if (doms[pre->GetBBId()] != nullptr && pre != bb) {
          newIDom = Intersect(*pre, *newIDom);
        }
      }
      if (doms[bb->GetBBId()] != newIDom) {
        doms[bb->GetBBId()] = newIDom;
        changed = true;
      }
    }
  } while (changed);
}

// Figure 5 in "A Simple, Fast Dominance Algorithm" by Keith Cooper et al.
void Dominance::ComputeDomFrontiers() {
  for (const BB *bb : bbVec) {
    if (bb == nullptr || bb == &commonExitBB) {
      continue;
    }
    if (bb->GetPred().size() < kBBVectorInitialSize) {
      continue;
    }
    for (BB *pre : bb->GetPred()) {
      BB *runner = pre;
      while (runner != doms[bb->GetBBId()] && runner != &commonEntryBB) {
        domFrontier[runner->GetBBId()].insert(bb->GetBBId());
        runner = doms[runner->GetBBId()];
      }
    }
  }
}

void Dominance::ComputeDomChildren() {
  for (const BB *bb : bbVec) {
    if (bb == nullptr) {
      continue;
    }
    if (doms[bb->GetBBId()] == nullptr) {
      continue;
    }
    BB *parent = doms[bb->GetBBId()];
    if (parent == bb) {
      continue;
    }
    domChildren[parent->GetBBId()].insert(bb->GetBBId());
  }
}

void Dominance::ComputeDtPreorder(const BB &bb, size_t &num) {
  CHECK_FATAL(num < dtPreOrder.size(), "index out of range in Dominance::ComputeDtPreorder");
  dtPreOrder[num++] = bb.GetBBId();
  for (BBId k : domChildren[bb.GetBBId()]) {
    ComputeDtPreorder(*bbVec[k], num);
  }
}

void Dominance::ComputeDtDfn() {
  for (size_t i = 0; i < dtPreOrder.size(); ++i) {
    dtDfn[dtPreOrder[i]] = i;
  }
}

// true if b1 dominates b2
bool Dominance::Dominate(const BB &bb1, const BB &bb2) {
  if (&bb1 == &bb2) {
    return true;
  }
  if (doms[bb2.GetBBId()] == nullptr) {
    return false;
  }
  const BB *immediateDom = &bb2;
  do {
    if (immediateDom == nullptr) {
      return false;
    }
    immediateDom = doms[immediateDom->GetBBId()];
    if (immediateDom == &bb1) {
      return true;
    }
  } while (immediateDom != &commonEntryBB);
  return false;
}

/* ================= for PostDominance ================= */
void Dominance::PdomPostOrderWalk(const BB &bb, int32 &pid, std::vector<bool> &visitedMap) {
  ASSERT(bb.GetBBId() < visitedMap.size(), "index out of range in  Dominance::PdomPostOrderWalk");
  if (bbVec[bb.GetBBId()] == nullptr) {
    return;
  }
  if (visitedMap[bb.GetBBId()]) {
    return;
  }
  visitedMap[bb.GetBBId()] = true;
  for (BB *pre : bb.GetPred()) {
    PdomPostOrderWalk(*pre, pid, visitedMap);
  }
  CHECK_FATAL(bb.GetBBId() < pdomPostOrderIDVec.size(), "index out of range in  Dominance::PdomPostOrderWalk");
  pdomPostOrderIDVec[bb.GetBBId()] = pid++;
}

void Dominance::PdomGenPostOrderID() {
  ASSERT(!bbVec.empty(), "call calloc failed in Dominance::PdomGenPostOrderID");
  std::vector<bool> visitedMap(bbVec.size(), false);
  int32 postOrderID = 0;
  PdomPostOrderWalk(commonExitBB, postOrderID, visitedMap);
  // initialize pdomReversePostOrder
  int32 maxPostOrderID = postOrderID - 1;
  pdomReversePostOrder.resize(maxPostOrderID + 1);
  for (size_t i = 0; i < pdomPostOrderIDVec.size(); ++i) {
    int32 postOrderNo = pdomPostOrderIDVec[i];
    if (postOrderNo == -1) {
      continue;
    }
    pdomReversePostOrder[maxPostOrderID - postOrderNo] = bbVec[i];
  }
}

BB *Dominance::PdomIntersect(BB &bb1, const BB &bb2) {
  auto *ptrBB1 = &bb1;
  auto *ptrBB2 = &bb2;
  while (ptrBB1 != ptrBB2) {
    while (pdomPostOrderIDVec[ptrBB1->GetBBId()] < pdomPostOrderIDVec[ptrBB2->GetBBId()]) {
      ptrBB1 = pdoms[ptrBB1->GetBBId()];
    }
    while (pdomPostOrderIDVec[ptrBB2->GetBBId()] < pdomPostOrderIDVec[ptrBB1->GetBBId()]) {
      ptrBB2 = pdoms[ptrBB2->GetBBId()];
    }
  }
  return ptrBB1;
}

// Figure 3 in "A Simple, Fast Dominance Algorithm" by Keith Cooper et al.
void Dominance::ComputePostDominance() {
  pdoms[commonExitBB.GetBBId()] = &commonExitBB;
  bool changed = false;
  do {
    changed = false;
    for (size_t i = 1; i < pdomReversePostOrder.size(); ++i) {
      BB *bb = pdomReversePostOrder[i];
      BB *suc = nullptr;
      if (bb->GetAttributes(kBBAttrIsExit) || bb->GetSucc().empty()) {
        suc = &commonExitBB;
      } else {
        suc = bb->GetSucc(0);
      }
      size_t j = 1;
      while ((pdoms[suc->GetBBId()] == nullptr || suc == bb) && j < bb->GetSucc().size()) {
        suc = bb->GetSucc(j);
        ++j;
      }
      if (pdoms[suc->GetBBId()] == nullptr) {
        suc = &commonExitBB;
      }
      BB *newIDom = suc;
      for (; j < bb->GetSucc().size(); ++j) {
        suc = bb->GetSucc(j);
        if (pdoms[suc->GetBBId()] != nullptr && suc != bb) {
          newIDom = PdomIntersect(*suc, *newIDom);
        }
      }
      if (pdoms[bb->GetBBId()] != newIDom) {
        pdoms[bb->GetBBId()] = newIDom;
        ASSERT(pdoms[newIDom->GetBBId()] != nullptr, "null ptr check");
        changed = true;
      }
    }
  } while (changed);
}

// Figure 5 in "A Simple, Fast Dominance Algorithm" by Keith Cooper et al.
void Dominance::ComputePdomFrontiers() {
  for (const BB *bb : bbVec) {
    if (bb == nullptr || bb == &commonEntryBB) {
      continue;
    }
    if (bb->GetSucc().size() < kBBVectorInitialSize) {
      continue;
    }
    for (BB *suc : bb->GetSucc()) {
      BB *runner = suc;
      while (runner != pdoms[bb->GetBBId()] && runner != &commonEntryBB) {
        pdomFrontier[runner->GetBBId()].insert(bb->GetBBId());
        ASSERT(pdoms[runner->GetBBId()] != nullptr, "ComputePdomFrontiers: pdoms[] is nullptr");
        runner = pdoms[runner->GetBBId()];
      }
    }
  }
}

void Dominance::ComputePdomChildren() {
  for (const BB *bb : bbVec) {
    if (bb == nullptr || pdoms[bb->GetBBId()] == nullptr) {
      continue;
    }
    const BB *parent = pdoms[bb->GetBBId()];
    if (parent == bb) {
      continue;
    }
    pdomChildren[parent->GetBBId()].insert(bb->GetBBId());
  }
}

void Dominance::ComputePdtPreorder(const BB &bb, size_t &num) {
  ASSERT(num < pdtPreOrder.size(), "index out of range in Dominance::ComputePdtPreOrder");
  pdtPreOrder[num++] = bb.GetBBId();
  for (BBId k : pdomChildren[bb.GetBBId()]) {
    ComputePdtPreorder(*bbVec[k], num);
  }
}

void Dominance::ComputePdtDfn() {
  for (size_t i = 0; i < pdtPreOrder.size(); ++i) {
    pdtDfn[pdtPreOrder[i]] = i;
  }
}

// true if b1 postdominates b2
bool Dominance::PostDominate(const BB &bb1, const BB &bb2) {
  if (&bb1 == &bb2) {
    return true;
  }
  if (pdoms[bb2.GetBBId()] == nullptr) {
    return false;
  }
  const BB *impdom = &bb2;
  do {
    if (impdom == nullptr) {
      return false;
    }
    impdom = pdoms[impdom->GetBBId()];
    if (impdom == &bb1) {
      return true;
    }
  } while (impdom != &commonExitBB);
  return false;
}

void Dominance::DumpDoms() {
  for (BB *bb : reversePostOrder) {
    LogInfo::MapleLogger() << "postorder no " << postOrderIDVec[bb->GetBBId()];
    LogInfo::MapleLogger() << " is bb:" << bb->GetBBId();
    LogInfo::MapleLogger() << " im_dom is bb:" << doms[bb->GetBBId()]->GetBBId();
    LogInfo::MapleLogger() << " domfrontier: [";
    for (BBId id : domFrontier[bb->GetBBId()]) {
      LogInfo::MapleLogger() << id << " ";
    }
    LogInfo::MapleLogger() << "] domchildren: [";
    for (BBId id : domChildren[bb->GetBBId()]) {
      LogInfo::MapleLogger() << id << " ";
    }
    LogInfo::MapleLogger() << "]\n";
  }
  LogInfo::MapleLogger() << "\npreorder traversal of dominator tree:";
  for (BBId id : dtPreOrder) {
    LogInfo::MapleLogger() << id << " ";
  }
  LogInfo::MapleLogger() << "\n\n";
}

void Dominance::DumpPdoms() {
  for (BB *bb : pdomReversePostOrder) {
    LogInfo::MapleLogger() << "pdom_postorder no " << pdomPostOrderIDVec[bb->GetBBId()];
    LogInfo::MapleLogger() << " is bb:" << bb->GetBBId();
    LogInfo::MapleLogger() << " im_pdom is bb:" << pdoms[bb->GetBBId()]->GetBBId();
    LogInfo::MapleLogger() << " pdomfrontier: [";
    for (BBId id : pdomFrontier[bb->GetBBId()]) {
      LogInfo::MapleLogger() << id << " ";
    }
    LogInfo::MapleLogger() << "] pdomchildren: [";
    for (BBId id : pdomChildren[bb->GetBBId()]) {
      LogInfo::MapleLogger() << id << " ";
    }
    LogInfo::MapleLogger() << "]\n";
  }
  LogInfo::MapleLogger() << "\n";
  LogInfo::MapleLogger() << "preorder traversal of post-dominator tree:";
  for (BBId id : pdtPreOrder) {
    LogInfo::MapleLogger() << id << " ";
  }
  LogInfo::MapleLogger() << "\n\n";
}
}  // namespace maple
