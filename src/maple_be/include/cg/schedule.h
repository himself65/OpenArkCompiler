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
#ifndef MAPLEBE_INCLUDE_CG_SCHEDULE_H
#define MAPLEBE_INCLUDE_CG_SCHEDULE_H

#include "insn.h"
#include "mad.h"
#include "dependence.h"
#include "live.h"

namespace maplebe {
#define LIST_SCHED_DUMP CG_DEBUG_FUNC(cgFunc)
#define LIST_SCHED_DUMP_REF CG_DEBUG_FUNC(&cgFunc)

class RegPressureSchedule {
 public:
  RegPressureSchedule (CGFunc &func, MapleAllocator &alloc)
      : cgFunc(func), liveReg(alloc.Adapter()), scheduledNode(alloc.Adapter()),
        readyList(alloc.Adapter()) {}
  virtual ~RegPressureSchedule() = default;

  void InitBBInfo(BB &b, MemPool &memPool, const MapleVector<DepNode*> &nodes);
  void BuildPhyRegInfo(const std::vector<int32> &regNumVec);
  void Init(const MapleVector<DepNode*> &nodes);
  void UpdateBBPressure(const DepNode &node);
  void CalculatePressure(DepNode &node, regno_t reg, bool def);
  void SortReadyList();
  bool IsLastUse(const DepNode &node, regno_t reg) const;
  void ReCalculateDepNodePressure(DepNode &node);
  void UpdateLiveReg(const DepNode &node, regno_t reg, bool def);
  bool CanSchedule(const DepNode &node) const;
  void UpdateReadyList(const DepNode &node);
  void UpdatePriority(DepNode &node);
  void CalculateMaxDepth(const MapleVector<DepNode*> &nodes);
  void CalculateNear(const DepNode &node);
  static bool DepNodePriorityCmp(const DepNode *node1, const DepNode *node2);
  DepNode *ChooseNode();
  void DoScheduling(MapleVector<DepNode*> &nodes);

 private:
  void DumpBBPressureInfo() const;
  void DumpBBLiveInfo() const;
  void DumpReadyList() const;
  void DumpSelectInfo(const DepNode &node) const;
  RegType GetRegisterType(regno_t reg) const;

  CGFunc &cgFunc;
  BB *bb = nullptr;
  int32 *maxPressure = nullptr;
  int32 *curPressure = nullptr;
  MapleUnorderedSet<regno_t> liveReg;
  /* save node that has been scheduled. */
  MapleVector<DepNode*> scheduledNode;
  MapleVector<DepNode*> readyList;
  /* save the amount of every type register. */
  int32 *physicalRegNum = nullptr;
  int32 maxPriority = 0;
};

enum SimulateType : uint8 {
  kListSchedule,
  kBruteForce,
  kSimulateOnly
};

class Schedule {
 public:
  Schedule(CGFunc &func, MemPool &memPool, LiveAnalysis &liveAnalysis, const std::string &phase)
      : phaseName(phase),
        cgFunc(func),
        memPool(memPool),
        alloc(&memPool),
        live(liveAnalysis),
        nodes(alloc.Adapter()),
        readyList(alloc.Adapter()) {}

  virtual ~Schedule() = default;
  virtual void MemoryAccessPairOpt() = 0;
  virtual void ClinitPairOpt() = 0;
  virtual void FindAndCombineMemoryAccessPair(const std::vector<DepNode*> &readyList) = 0;
  virtual void RegPressureScheduling(BB &bb, MapleVector<DepNode*> &depNodes) = 0;
  virtual uint32 DoSchedule() = 0;
  virtual uint32 DoBruteForceSchedule() = 0;
  virtual uint32 SimulateOnly() = 0;
  virtual void UpdateBruteForceSchedCycle() = 0;
  virtual void IterateBruteForce(DepNode &targetNode, MapleVector<DepNode*> &readyList, uint32 currCycle,
                                 MapleVector<DepNode*> &scheduledNodes, uint32 &maxCycleCount,
                                 MapleVector<DepNode*> &optimizedScheduledNodes) = 0;
  virtual void UpdateReadyList(DepNode &targetNode, MapleVector<DepNode*> &readyList, bool updateEStart) = 0;
  virtual void ListScheduling(bool beforeRA) = 0;
  virtual void FinalizeScheduling(BB &bb, const DepAnalysis &depAnalysis) = 0;

 protected:
  virtual void Init() = 0;
  virtual uint32 ComputeEstart(uint32 cycle) = 0;
  virtual void ComputeLstart(uint32 maxEstart) = 0;
  virtual void UpdateELStartsOnCycle(uint32 cycle) = 0;
  virtual void RandomTest() = 0;
  virtual void EraseNodeFromReadyList(const DepNode &target) = 0;
  virtual void EraseNodeFromNodeList(const DepNode &target, MapleVector<DepNode*> &readyList) = 0;
  virtual uint32 GetNextSepIndex() const = 0;
  virtual void CountUnitKind(const DepNode &depNode, uint32 array[], const uint32 arraySize) const = 0;
  virtual bool CanCombine(const Insn &insn) const = 0;
  void InitIDAndLoc();
  std::string PhaseName() const {
    return phaseName;
  }

  const std::string phaseName;
  CGFunc &cgFunc;
  MemPool &memPool;
  MapleAllocator alloc;
  LiveAnalysis &live;
  DepAnalysis *depAnalysis = nullptr;
  MAD *mad = nullptr;
  uint32 lastSeparatorIndex = 0;
  uint32 nodeSize = 0;
  MapleVector<DepNode*> nodes;      /* Dependence graph */
  MapleVector<DepNode*> readyList;  /* Ready list. */
};

CGFUNCPHASE_CANSKIP(CgDoPreScheduling, "prescheduling")
CGFUNCPHASE_CANSKIP(CgDoScheduling, "scheduling")
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_SCHEDULE_H */
