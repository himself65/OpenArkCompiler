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

#undef PRESCHED_DEBUG

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

/* return register type according to register number */
RegType RegPressureSchedule::GetRegisterType(regno_t reg) const {
  return cgFunc.GetRegisterType(reg);
}

/* Get amount of every physical register */
void RegPressureSchedule::BuildPhyRegInfo(const std::vector<int32> &regNumVec) {
  FOR_ALL_REGCLASS(i) {
    physicalRegNum[i] = regNumVec[i];
  }
}

/* initialize register pressure information according to bb's live-in data.
 * initialize node's valid preds size.
 */
void RegPressureSchedule::Init(const MapleVector<DepNode*> &nodes) {
  FOR_ALL_REGCLASS(i) {
    curPressure[i] = 0;
    maxPressure[i] = 0;
  }

  for (auto *node : nodes) {
    /* initialize */
    node->InitPressure();

    /* calculate the node uses'register pressure */
    for (auto &useReg : node->GetRegUses()) {
      CalculatePressure(*node, useReg.first, false);
    }

    /* calculate the node defs'register pressure */
    for (auto &defReg : node->GetRegDefs()) {
      CalculatePressure(*node, defReg.first, true);

      regno_t reg = defReg.first;
      RegType regType = GetRegisterType(reg);
      /* if no use list, a register is only defined, not be used */
      if (defReg.second == nullptr) {
        node->IncDeadDefByIndex(regType);
      }
    }

    node->SetValidPredsSize(node->GetPreds().size());
  }

  DepNode *firstNode = nodes.front();
  readyList.push_back(firstNode);
  firstNode->SetState(kReady);
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

  int32 near1 = node1->GetNear();
  int32 near2 = node2->GetNear();
  if (near1 != near2) {
    return node1->GetNear() > node2->GetNear();
  }

  return node1->GetSuccs().size() < node2->GetSuccs().size();
}

/* set a node's incPressure is true, when a class register inscrease */
void RegPressureSchedule::ReCalculateDepNodePressure(DepNode &node) {
  /* if there is a type of register pressure increases, set incPressure as true. */
  auto &pressures = node.GetPressure();
  node.SetIncPressure(pressures[kRegisterInt] > 0);
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
    if (succ->GetDepType() == kDependenceTypeTrue && to.GetNear() < node.GetNear() + 1) {
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
  RegType regType = GetRegisterType(reg);
  /* if def a register, register pressure increase. */
  if (def) {
    node.IncPressureByIndex(regType);
  } else {
    /* if it is the last time using the reg, register pressure decrease. */
    if (IsLastUse(node, reg)) {
      node.DecPressureByIndex(regType);
    }
  }
}

/* update live reg information. */
void RegPressureSchedule::UpdateLiveReg(const DepNode &node, regno_t reg, bool def) {
  if (def) {
    if (liveReg.find(reg) == liveReg.end()) {
      liveReg.insert(reg);
    }
    /* if no use list, a register is only defined, not be used */
    auto it = node.GetRegDefs().find(reg);
    ASSERT(it != node.GetRegDefs().end(), "not find reg!");
    if (it->second == nullptr) {
      liveReg.erase(reg);
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
  for (auto &useReg : node.GetRegUses()) {
    auto reg = useReg.first;

#ifdef PRESCHED_DEBUG
    UpdateLiveReg(node, reg, false);
    if (liveReg.find(reg) == liveReg.end()) {
      continue;
    }
#endif

    /* find all insn that use the reg, if a insn use the reg lastly, insn'pressure - 1 */
    auto it = node.GetRegUses().find(reg);
    ASSERT(it->second != nullptr, "valid iterator check");
    ASSERT(it != node.GetRegUses().end(), "not find reg!");
    RegList *regList = it->second;

    while (regList != nullptr) {
      CHECK_NULL_FATAL(regList->insn);
      DepNode *useNode = regList->insn->GetDepNode();
      if (useNode->GetState() == kScheduled) {
        regList = regList->next;
        continue;
      }

      if (IsLastUse(*useNode, reg)) {
        RegType regType = GetRegisterType(reg);
        useNode->DecPressureByIndex(regType);
      }
      break;
    }
  }

#ifdef PRESCHED_DEBUG
  for (auto &defReg : node.GetRegDefs()) {
    UpdateLiveReg(node, defReg.first, true);
  }
#endif

  const auto &pressures = node.GetPressure();
  const auto &deadDefNum = node.GetDeadDefNum();
#ifdef PRESCHED_DEBUG
  LogInfo::MapleLogger() << "node's pressure: ";
  for (auto pressure : pressures) {
    LogInfo::MapleLogger() << pressure[i] << " ";
  }
  LogInfo::MapleLogger() << "\n";
#endif

  FOR_ALL_REGCLASS(i) {
    curPressure[i] += pressures[i];
    if (curPressure[i] > maxPressure[i]) {
      maxPressure[i] = curPressure[i];
    }
    curPressure[i] -= deadDefNum[i];
  }
}

/* update node priority and try to update the priority of all node's ancestor. */
void RegPressureSchedule::UpdatePriority(DepNode &node) {
  std::vector<DepNode*> workQueue;
  workQueue.push_back(&node);
  node.SetPriority(maxPriority++);
  do {
    DepNode *nowNode = workQueue.front();
    workQueue.erase(workQueue.begin());
    for (auto pred : nowNode->GetPreds()) {
      DepNode &from = pred->GetFrom();
      if (from.GetState() != kScheduled && from.GetPriority() != maxPriority) {
        from.SetPriority(maxPriority);
        workQueue.push_back(&from);
      }
    }
  } while (!workQueue.empty());
}

/* return true if all node's pred has been scheduled. */
bool RegPressureSchedule::CanSchedule(const DepNode &node) const {
  return node.GetValidPredsSize() == 0;
}

/*
 * add the successor of node to readyList when
 *  1. successor has no been scheduled;
 *  2. successor's has been scheduled or the dependence between node and successor is true-dependence.
 */
void RegPressureSchedule::UpdateReadyList(const DepNode &node) {
  for (auto *succ : node.GetSuccs()) {
    DepNode &succNode = succ->GetTo();
    succNode.DescreaseValidPredsSize();

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

void RegPressureSchedule::DumpBBLiveInfo() const {
  LogInfo::MapleLogger() << "Live In: ";
  for (auto reg : bb->GetLiveInRegNO()) {
    LogInfo::MapleLogger() << "R" <<reg << " ";
  }
  LogInfo::MapleLogger() << "\n";

  LogInfo::MapleLogger() << "Live Out: ";
  for (auto reg : bb->GetLiveOutRegNO()) {
    LogInfo::MapleLogger() << "R" << reg << " ";
  }
  LogInfo::MapleLogger() << "\n";
}

void RegPressureSchedule::DumpReadyList() const {
  LogInfo::MapleLogger() << "readyList: " << "\n";
  for (DepNode *it : readyList) {
    if (CanSchedule(*it)) {
      LogInfo::MapleLogger() << it->GetInsn()->GetId() << "CS ";
    } else {
      LogInfo::MapleLogger() << it->GetInsn()->GetId() << "NO ";
    }
  }
  LogInfo::MapleLogger() << "\n";
}

void RegPressureSchedule::DumpSelectInfo(const DepNode &node) const {
  LogInfo::MapleLogger() << "select a node: " << "\n";
  node.DumpSchedInfo();
  node.DumpRegPressure();
  node.GetInsn()->Dump();

  LogInfo::MapleLogger() << "liveReg: ";
  for (auto reg : liveReg) {
    LogInfo::MapleLogger() << "R" << reg << " ";
  }
  LogInfo::MapleLogger() << "\n";

  LogInfo::MapleLogger() << "\n";
}

void RegPressureSchedule::DumpBBPressureInfo() const {
  LogInfo::MapleLogger() << "curPressure: ";
  FOR_ALL_REGCLASS(i) {
    LogInfo::MapleLogger() << curPressure[i] << " ";
  }
  LogInfo::MapleLogger() << "\n";

  LogInfo::MapleLogger() << "maxPressure: ";
  FOR_ALL_REGCLASS(i) {
    LogInfo::MapleLogger() << maxPressure[i] << " ";
  }
  LogInfo::MapleLogger() << "\n";
}


void RegPressureSchedule::DoScheduling(MapleVector<DepNode*> &nodes) {
#ifdef PRESCHED_DEBUG
  LogInfo::MapleLogger() << "--------------- bb " << bb->GetId() <<" begin scheduling -------------" << "\n";
  DumpBBLiveInfo();
#endif

  /* initialize register pressure information and readylist. */
  Init(nodes);
  CalculateMaxDepth(nodes);
  while (!readyList.empty()) {
    /* calculate register pressure */
    for (DepNode *it : readyList) {
      ReCalculateDepNodePressure(*it);
    }

    /* choose a node can be scheduled currently. */
    DepNode *node = ChooseNode();
#ifdef PRESCHED_DEBUG
    DumpBBPressureInfo();
    DumpReadyList();
    LogInfo::MapleLogger() << "first tmp select node: " << node->GetInsn()->GetId() << "\n";
#endif

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
#ifdef PRESCHED_DEBUG
    DumpSelectInfo(*node);
#endif
  }

#ifdef PRESCHED_DEBUG
  LogInfo::MapleLogger() << "---------------------------------- end --------------------------------" << "\n";
#endif
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
