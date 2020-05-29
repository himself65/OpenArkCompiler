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
#if TARGAARCH64
#include "aarch64_schedule.h"
#endif
#if TARGARM32
#include "arm32_schedule.h"
#endif
#include "cg.h"
#include "optimize_common.h"

namespace maplebe {
/* ---- RegPressureSchedule function ---- */
void RegPressureSchedule::InitBBInfo(BB &b, MemPool &memPool, const MapleVector<DepNode*> &nodes) {
  bb = &b;
  liveReg.clear();
  scheduledNode.clear();
  readyList.clear();
  maxPriority = 0;
  maxPressure = memPool.NewArray<int32>(RegPressure::GetMaxRegClassNum());
  curPressure = memPool.NewArray<int32>(RegPressure::GetMaxRegClassNum());
  physicalRegNum = memPool.NewArray<int32>(RegPressure::GetMaxRegClassNum());
  for (auto node : nodes) {
    node->SetState(kNormal);
  }
}

/* Get amount of every physical register */
void RegPressureSchedule::BuildPhyRegInfo(const std::vector<int32> &regNumVec) {
  FOR_ALL_REGCLASS(i) {
    physicalRegNum[i] = regNumVec[i];
  }
}

void RegPressureSchedule::InitReadyList(const MapleVector<DepNode*> &nodes) {
  for (auto node : nodes) {
    /* If a node is not been scheduled and there is no pred node of the node, add the node reaylist. */
    if ((node->GetState() == kNormal) && node->GetPreds().empty()) {
      readyList.push_back(node);
      node->SetState(kReady);
    }
  }
}

/* initialize register pressure information according to bb's live-in data. */
void RegPressureSchedule::InitPressure() {
  FOR_ALL_REGCLASS(i) {
    curPressure[i] = 0;
  }
  /* add all bb's live-in register to liveReg. */
  for (auto reg : bb->GetLiveInRegNO()) {
    RegType regType = cgFunc.GetRegisterType(reg);
    if (liveReg.find(reg) == liveReg.end()) {
      liveReg.insert(reg);
      /* increase the pressure of the register type. */
      ++curPressure[regType];
    }
  }

  FOR_ALL_REGCLASS(i) {
    maxPressure[i] = curPressure[i];
  }
}

void RegPressureSchedule::SortReadyList() {
  std::sort(readyList.begin(), readyList.end(), DepNodePriorityCmp);
}

/* return true if nodes1 first. */
bool RegPressureSchedule::DepNodePriorityCmp(const DepNode *node1, const DepNode *node2) {
  CHECK_NULL_FATAL(node1);
  CHECK_NULL_FATAL(node2);
  if (node1->GetPriority() != node2->GetPriority()) {
    return node1->GetPriority() > node2->GetPriority();
  }

  int32 depthS1 = node1->GetMaxDepth() + node1->GetNear();
  int32 depthS2 = node2->GetMaxDepth() + node2->GetNear();
  if (depthS1 != depthS2) {
    return depthS1 > depthS2;
  }

  return node1->GetNear() > node2->GetNear();
}

/* calculate a node register pressure base on current scheduling */
void RegPressureSchedule::ReCalculateDepNodePressure(DepNode &node) {
  /* initialize */
  node.InitPressure();
  /* calculate the node uses'register pressure */
  for (auto reg : node.GetUses()) {
    CalculatePressure(node, reg, false);
  }
  /* calculate the node defs'register pressure */
  for (auto reg : node.GetDefs()) {
    CalculatePressure(node, reg, true);
  }

  /* if there is a type of register pressure increases, set incPressure as true. */
  const int32 *pressure = node.GetPressure();
  FOR_ALL_REGCLASS(i) {
    if (pressure[i] > 0) {
      node.SetIncPressure(true);
      break;
    }
  }
}

/* calculate the maxDepth of every node in nodes. */
void RegPressureSchedule::CalculateMaxDepth(const MapleVector<DepNode*> &nodes) {
  /* from the last node to first node. */
  for (auto it = nodes.rbegin(); it != nodes.rend(); ++it) {
    /* traversing each successor of it. */
    for (auto succ : (*it)->GetSuccs()) {
      DepNode &to = succ->GetTo();
      if ((*it)->GetMaxDepth() < (to.GetMaxDepth() + 1)) {
        (*it)->SetMaxDepth(to.GetMaxDepth() + 1);
      }
    }
  }
}

/* calculate the near of every successor of the node. */
void RegPressureSchedule::CalculateNear(const DepNode &node) {
  for (auto succ : node.GetSuccs()) {
    DepNode &to = succ->GetTo();
    if (to.GetNear() < node.GetNear() + 1) {
      to.SetNear(node.GetNear() + 1);
    }
  }
}

/* return true if it is last time using the regNO. */
bool RegPressureSchedule::IsLastUse(const DepNode &node, regno_t regNO) const {
  auto it = node.GetRegUses().find(regNO);
  ASSERT(it->second != nullptr, "valid iterator check");
  ASSERT(it != node.GetRegUses().end(), "not find reg!");
  RegList *regList = it->second;
  if (bb->GetLiveOutRegNO().find(regNO) != bb->GetLiveOutRegNO().end()) {
    return false;
  }

  /*
   * except the node, if there are insn that has no scheduled in regNO'sregList,
   * then it is not the last time using the regNO, return false.
   */
  while (regList != nullptr) {
    CHECK_NULL_FATAL(regList->insn);
    DepNode *useNode = regList->insn->GetDepNode();
    ASSERT(useNode != nullptr, "get depend node failed in RegPressureSchedule::IsLastUse");
    if ((regList->insn != node.GetInsn()) && (useNode->GetState() != kScheduled)) {
      return false;
    }
    regList = regList->next;
  }
  return true;
}

void RegPressureSchedule::CalculatePressure(DepNode &node, regno_t reg, bool def) {
  RegType regType = cgFunc.GetRegisterType(reg);
  /* if def a register, register pressure increase. */
  if (def) {
    if (liveReg.find(reg) == liveReg.end()) {
      node.IncPressureByIndex(regType);
    }
  } else {
    /* if it is the last time using the reg, register pressure decrease. */
    if (IsLastUse(node, reg)) {
      if (liveReg.find(reg) != liveReg.end()) {
        node.DecPressureByIndex(regType);
      }
    }
  }
}

/* update live reg information. */
void RegPressureSchedule::UpdateLiveReg(const DepNode &node, regno_t reg, bool def) {
  if (def) {
    if (liveReg.find(reg) == liveReg.end()) {
      liveReg.insert(reg);
    }
  } else {
    if (IsLastUse(node, reg)) {
      if (liveReg.find(reg) != liveReg.end()) {
        liveReg.erase(reg);
      }
    }
  }
}

/* update register pressure information. */
void RegPressureSchedule::UpdateBBPressure(const DepNode &node) {
  const MapleSet<regno_t> &uses = node.GetUses();
  const MapleSet<regno_t> &defs = node.GetDefs();

  for (auto reg : uses) {
    UpdateLiveReg(node, reg, false);
  }
  for (auto reg : defs) {
    UpdateLiveReg(node, reg, true);
  }
  const int32 *pressure = node.GetPressure();
  ASSERT(pressure != nullptr, "get pressure in node failed in RegPressureSchedule::UpdateBBPressure");
  FOR_ALL_REGCLASS(i) {
    curPressure[i] += pressure[i];
    if (curPressure[i] > maxPressure[i]) {
      maxPressure[i] = curPressure[i];
    }
  }
}

/* update node priority and try to update the priority of all node's ancestor. */
void RegPressureSchedule::UpdatePriority(DepNode &node) {
  std::queue<DepNode*> workQueue;
  workQueue.push(&node);
  node.SetPriority(maxPriority++);
  do {
    DepNode *nowNode = workQueue.front();
    workQueue.pop();
    for (auto pred : nowNode->GetPreds()) {
      DepNode &from = pred->GetFrom();
      if (from.GetState() != kScheduled) {
        from.SetPriority(maxPriority);
        workQueue.push(&from);
      }
    }
  } while (!workQueue.empty());
}

/* return true if all node's pred has been scheduled. */
bool RegPressureSchedule::CanSchedule(const DepNode &node) const {
  for (auto pred : node.GetPreds()) {
    DepNode &from = pred->GetFrom();
    if (from.GetState() != kScheduled) {
      return false;
    }
  }
  return true;
}

/*
 * add the successor of node to readyList when
 *  1. successor has no been scheduled;
 *  2. successor's has been scheduled or the dependence between node and successor is true-dependence.
 */
void RegPressureSchedule::UpdateReadyList(const DepNode &node) {
  for (auto *succ : node.GetSuccs()) {
    DepNode &succNode = succ->GetTo();
    if (((succ->GetDepType() == kDependenceTypeTrue) || CanSchedule(succNode)) && (succNode.GetState() == kNormal)) {
      readyList.push_back(&succNode);
      succNode.SetState(kReady);
    }
  }
}

/* choose a node to schedule */
DepNode *RegPressureSchedule::ChooseNode() {
  DepNode *node = nullptr;
  for (auto *it : readyList) {
    if (!it->GetIncPressure()) {
      if (CanSchedule(*it)) {
        return it;
      } else if (node == nullptr) {
        node = it;
      }
    }
  }
  if (node == nullptr) {
    node = readyList.front();
  }
  return node;
}

void RegPressureSchedule::DoScheduling(MapleVector<DepNode*> &nodes) {
  /* initialize register pressure information and readylist. */
  InitPressure();
  InitReadyList(nodes);
  CalculateMaxDepth(nodes);
  SortReadyList();
  while (!readyList.empty()) {
    /* calculate register pressure */
    for (auto *it : readyList) {
      ReCalculateDepNodePressure(*it);
    }

    /* choose a node can be scheduled currently. */
    DepNode *node = ChooseNode();
    while (!CanSchedule(*node)) {
      UpdatePriority(*node);
      SortReadyList();
      node = readyList.front();
    }

    scheduledNode.push_back(node);
    /* mark node has scheduled */
    node->SetState(kScheduled);
    UpdateBBPressure(*node);
    CalculateNear(*node);
    /* delete node from readylist */
    for (auto it = readyList.begin(); it != readyList.end(); ++it) {
      if (*it == node) {
        readyList.erase(it);
        break;
      }
    }
    UpdateReadyList(*node);
    SortReadyList();
  }

  /* update nodes according to scheduledNode. */
  nodes.clear();
  for (auto node : scheduledNode) {
    nodes.push_back(node);
  }
}

/*
 * ------------- Schedule function ----------
 * calculate and mark each insn id, each BB's firstLoc and lastLoc.
 */
void Schedule::InitIDAndLoc() {
  uint32 id = 0;
  FOR_ALL_BB(bb, &cgFunc) {
    bb->SetLastLoc(bb->GetPrev() ? bb->GetPrev()->GetLastLoc() : nullptr);
    FOR_BB_INSNS(insn, bb) {
      insn->SetId(id++);
#if DEBUG
      insn->AppendComment(" Insn id: " + std::to_string(insn->GetId()));
#endif
      if (insn->IsImmaterialInsn() && !insn->IsComment()) {
        bb->SetLastLoc(insn);
      } else if (!bb->GetFirstLoc() && insn->IsMachineInstruction()) {
        bb->SetFirstLoc(*bb->GetLastLoc());
      }
    }
  }
}
AnalysisResult* CgDoPreScheduling::Run(CGFunc *cgFunc, CgFuncResultMgr *cgFuncResMgr) {
  ASSERT(cgFunc != nullptr, "expect a cgfunc in CgDoPreScheduling");
  CHECK_NULL_FATAL(cgFuncResMgr);
  if (LIST_SCHED_DUMP) {
    LogInfo::MapleLogger() << "Before CgDoPreScheduling : " << cgFunc->GetName() << "\n";
    DotGenerator::GenerateDot("preschedule", *cgFunc, cgFunc->GetMirModule(), true);
  }

  auto *live = static_cast<LiveAnalysis*>(cgFuncResMgr->GetAnalysisResult(kCGFuncPhaseLIVE, cgFunc));
  /* revert liveanalysis result container. */
  ASSERT(live != nullptr, "nullptr check");
  live->ResetLiveSet();

  MemPool *scheduleMp = NewMemPool();
  Schedule *schedule = nullptr;
#if TARGAARCH64
  schedule = scheduleMp->New<AArch64Schedule>(*cgFunc, *scheduleMp, *live, PhaseName());
#endif
#if TARGARM32
  schedule = scheduleMp->New<Arm32Schedule>(*cgFunc, *scheduleMp, *live, PhaseName());
#endif
  schedule->ListScheduling(true);
  live->ClearInOutDataInfo();
  cgFuncResMgr->InvalidAnalysisResult(kCGFuncPhaseLIVE, cgFunc);

  return nullptr;
}

AnalysisResult* CgDoScheduling::Run(CGFunc *cgFunc, CgFuncResultMgr *cgFuncResMgr) {
  ASSERT(cgFunc != nullptr, "expect a cgfunc in CgDoScheduling");
  CHECK_NULL_FATAL(cgFuncResMgr);
  if (LIST_SCHED_DUMP) {
    LogInfo::MapleLogger() << "Before CgDoScheduling : " << cgFunc->GetName() << "\n";
    DotGenerator::GenerateDot("scheduling", *cgFunc, cgFunc->GetMirModule(), true);
  }

  auto *live = static_cast<LiveAnalysis*>(cgFuncResMgr->GetAnalysisResult(kCGFuncPhaseLIVE, cgFunc));
  /* revert liveanalysis result container. */
  ASSERT(live != nullptr, "nullptr check");
  live->ResetLiveSet();

  MemPool *scheduleMp = NewMemPool();
  Schedule *schedule = nullptr;
#if TARGAARCH64
  schedule = scheduleMp->New<AArch64Schedule>(*cgFunc, *scheduleMp, *live, PhaseName());
#endif
#if TARGARM32
  schedule = scheduleMp->New<Arm32Schedule>(*cgFunc, *scheduleMp, *live, PhaseName());
#endif
  schedule->ListScheduling(false);
  live->ClearInOutDataInfo();
  cgFuncResMgr->InvalidAnalysisResult(kCGFuncPhaseLIVE, cgFunc);

  return nullptr;
}
}  /* namespace maplebe */
