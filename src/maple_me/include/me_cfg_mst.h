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
#ifndef MAPLE_ME_INCLUDE_CFGMST_H
#define MAPLE_ME_INCLUDE_CFGMST_H

#include "me_function.h"
#include "bb.h"

namespace maple {
template <class Edge>
class CFGMST {
 public:
  CFGMST(MeFunction &func, MemPool &mp, bool dump) : func(&func), mp(&mp), dump(dump) {}
  BB *FindGroup(BB *bb);
  bool UnionGroups(BB *src, BB *dest);
  void ComputeMST();
  void SortEdges();
  void AddEdge(BB *src, BB *dest, uint64 w, bool isCritical = false, bool isFake = false);
  bool IsCritialEdge(const BB *src, const BB *dest) const {
    return src->GetSucc().size() > 1 && dest->GetPred().size() > 1;
  }
  const std::vector<Edge*> &GetAllEdges() const {
    return allEdges;
  }

  uint32 GetAllEdgesSize() const {
    return allEdges.size();
  }

  uint32 GetAllBBs() const {
    return totalBB;
  }

  void GetInstrumentEdges(std::vector<Edge*> &instrumentEdges) const {
    for (const auto &e : allEdges) {
      if (!e->IsInMST()) {
        instrumentEdges.push_back(e);
      }
    }
  }

  void BuildEdges();
  void DumpEdgesInfo() const;
 private:
  static constexpr int normalEdgeWeight = 2;
  static constexpr int exitEdgeWeight = 3;
  static constexpr int fakeExitEdgeWeight = 4;
  static constexpr int criticalEdgeWeight = 4;
  std::vector<Edge*> allEdges;
  MeFunction *func;
  MemPool *mp;
  bool dump;
  uint32 totalBB = 0;
};

template <class Edge>
void CFGMST<Edge>::DumpEdgesInfo() const {
  for (auto &edge : allEdges) {
    BB *src = edge->GetSrcBB();
    BB *dest = edge->GetDestBB();
    LogInfo::MapleLogger() << "BB" << src->GetBBId() << "->"
                           << "BB" << dest->GetBBId() << " weight " << edge->GetWeight();
    if (edge->IsInMST()) {
      LogInfo::MapleLogger() << " in Mst\n";
    } else {
      LogInfo::MapleLogger() << " not in  Mst\n";
    }
  }
}

template <class Edge>
BB *CFGMST<Edge>::FindGroup(BB *bb) {
  if (bb->GetGroup() != bb) {
    BB *group = bb->GetGroup();
    bb->SetGroup(FindGroup(group));
  }
  return bb->GetGroup();
}

template <class Edge>
bool CFGMST<Edge>::UnionGroups(BB *src, BB *dest) {
  BB *srcGroup = FindGroup(src);
  BB *destGroup = FindGroup(dest);
  if (srcGroup == destGroup) {
    return false;
  }
  srcGroup->SetGroup(destGroup);
  return true;
}

template <class Edge>
void CFGMST<Edge>::ComputeMST() {
  BuildEdges();
  SortEdges();
  /* only one edge means only one bb */
  if (allEdges.size() == 1) {
    if (dump) {
      LogInfo::MapleLogger() << "only one edge in " << func->GetMirFunc()->GetName() << std::endl;
    }
    return;
  }
  for (auto &e : allEdges) {
    if (UnionGroups(e->GetSrcBB(), e->GetDestBB())) {
      e->SetInMST();
    }
  }
}

template <class Edge>
void CFGMST<Edge>::AddEdge(BB *src, BB *dest, uint64 w, bool isCritical, bool isFake) {
  if (src == nullptr || dest == nullptr) {
    return;
  }
  bool found = false;
  for (auto &edge : allEdges) {
    if (edge->GetSrcBB() == src && edge->GetDestBB() == dest) {
      uint64 weight = edge->GetWeight();
      weight++;
      edge->SetWeight(weight);
      found = true;
    }
  }
  if (!found) {
    allEdges.emplace_back(mp->New<Edge>(src, dest, w, isCritical, isFake));
  }
}

template <class Edge>
void CFGMST<Edge>::SortEdges() {
  std::stable_sort(allEdges.begin(), allEdges.end(),
                   [](const Edge *edge1, const Edge *edge2) { return edge1->GetWeight() > edge2->GetWeight(); });
}

template <class Edge>
void CFGMST<Edge>::BuildEdges() {
  BB *entry = func->GetCommonEntryBB();
  BB *exit = nullptr;
  auto eIt = func->valid_end();
  for (auto bIt = func->valid_begin(); bIt != eIt; ++bIt) {
    auto *bb = *bIt;
    if (bIt == func->common_exit()) {
      exit = *bIt;
      continue;
    }
    totalBB++;
    for (auto *succBB : bb->GetSucc()) {
      /* exitBB incoming edge allocate high weight */
      if (succBB->GetKind() == BBKind::kBBReturn) {
        AddEdge(bb, succBB, exitEdgeWeight);
        continue;
      }
      AddEdge(bb, succBB, normalEdgeWeight);
    }
  }

  for (BB *bb : func->GetCommonExitBB()->GetPred()) {
    AddEdge(bb, exit, fakeExitEdgeWeight, false, true);
  }
  /* insert fake edge to keep consistent */
  AddEdge(exit, entry, UINT64_MAX, false, true);
}
}  // namespace maple
#endif  // MAPLE_ME_INCLUDE_CFGMST_H
