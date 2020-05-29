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
#ifndef MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_SCHEDULE_H
#define MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_SCHEDULE_H

#include "schedule.h"
#include "aarch64_operand.h"

namespace maplebe {
enum RegisterType : uint8 {
  kRegisterUndef,
  kRegisterInt,
  kRegisterFloat,
  kRegisterCc,
  kRegisterLast,
};

class ScheduleProcessInfo {
 public:
  explicit ScheduleProcessInfo(uint32 size) {
    availableReadyList.reserve(size);
    scheduledNodes.reserve(size);
  }

  ~ScheduleProcessInfo() = default;

  uint32 GetLastUpdateCycle() const {
    return lastUpdateCycle;
  }

  void SetLastUpdateCycle(uint32 updateCycle) {
    lastUpdateCycle = updateCycle;
  }

  uint32 GetCurrCycle() const {
    return currCycle;
  }

  void IncCurrCycle() {
    ++currCycle;
  }

  void DecAdvanceCycle() {
    advanceCycle--;
  }

  uint32 GetAdvanceCycle() const {
    return advanceCycle;
  }

  void SetAdvanceCycle(uint32 cycle) {
    advanceCycle = cycle;
  }

  void ClearAvailableReadyList() {
    availableReadyList.clear();
  }

  void PushElemIntoAvailableReadyList(DepNode *node) {
    availableReadyList.push_back(node);
  }

  size_t SizeOfAvailableReadyList() const {
    return availableReadyList.size();
  }

  bool AvailableReadyListIsEmpty() const {
    return availableReadyList.empty();
  }

  void SetAvailableReadyList(const std::vector<DepNode*> &tempReadyList) {
    availableReadyList = tempReadyList;
  }

  const std::vector<DepNode*> &GetAvailableReadyList() const {
    return availableReadyList;
  }

  const std::vector<DepNode*> &GetAvailableReadyList() {
    return availableReadyList;
  }

  void PushElemIntoScheduledNodes(DepNode *node) {
    node->SetState(kScheduled);
    node->SetSchedCycle(currCycle);
    node->OccupyUnits();
    scheduledNodes.push_back(node);
  }

  bool IsFirstSeparator() const {
    return isFirstSeparator;
  }

  void ResetIsFirstSeparator() {
    isFirstSeparator = false;
  }

  size_t SizeOfScheduledNodes() const {
    return scheduledNodes.size();
  }

  const std::vector<DepNode*> &GetScheduledNodes() const {
    return scheduledNodes;
  }

 private:
  std::vector<DepNode*> availableReadyList;
  std::vector<DepNode*> scheduledNodes;
  uint32 lastUpdateCycle = 0;
  uint32 currCycle = 0;
  uint32 advanceCycle = 0;
  bool isFirstSeparator = true;
};

class AArch64Schedule : public Schedule {
 public:
  AArch64Schedule(CGFunc &func, MemPool &memPool, LiveAnalysis &live, const std::string &phaseName)
      : Schedule(func, memPool, live, phaseName) {}
  ~AArch64Schedule() override = default;
 protected:
  void DumpDepGraph(const MapleVector<DepNode*> &nodes) const;
  void DumpScheduleResult(const MapleVector<DepNode*> &nodes, SimulateType type) const;
  void GenerateDot(const BB &bb, const MapleVector<DepNode*> &nodes) const;

 private:
  void Init() override;
  void MemoryAccessPairOpt() override;
  void ClinitPairOpt() override;
  void RegPressureScheduling(BB &bb, MapleVector<DepNode*> &nd) override;
  uint32 DoSchedule() override;
  uint32 DoBruteForceSchedule() override;
  uint32 SimulateOnly() override;
  void UpdateBruteForceSchedCycle() override;
  void IterateBruteForce(DepNode &targetNode, MapleVector<DepNode*> &readyList, uint32 currCycle,
                         MapleVector<DepNode*> &scheduledNodes, uint32 &maxCycleCount,
                         MapleVector<DepNode*> &optimizedScheduledNodes) override;
  void FindAndCombineMemoryAccessPair(const std::vector<DepNode*> &readyList) override;
  bool CanCombine(const Insn &insn) const override;
  void ListScheduling(bool beforeRA) override;
  void BruteForceScheduling(const BB &bb);
  void SimulateScheduling(const BB &bb);
  void FinalizeScheduling(BB &bb, const DepAnalysis &depAnalysis) override;
  uint32 ComputeEstart(uint32 cycle) override;
  void ComputeLstart(uint32 maxEstart) override;
  void UpdateELStartsOnCycle(uint32 cycle) override;
  void RandomTest() override;
  void EraseNodeFromReadyList(const DepNode &target) override;
  void EraseNodeFromNodeList(const DepNode &target, MapleVector<DepNode*> &readyList) override;
  uint32 GetNextSepIndex() const override;
  void CountUnitKind(const DepNode &depNode, uint32 array[], const uint32 arraySize) const override;
  static bool IfUseUnitKind(const DepNode &depNode, uint32 index);
  void UpdateReadyList(DepNode &targetNode, MapleVector<DepNode*> &readyList, bool updateEStart) override;
  void UpdateScheduleProcessInfo(ScheduleProcessInfo &info);
  bool CheckSchedulable(ScheduleProcessInfo &info) const;
  void SelectNode(ScheduleProcessInfo &scheduleInfo);
  static void DumpDebugInfo(const ScheduleProcessInfo &info);
  static bool CompareDepNode(const DepNode &node1, const DepNode &node2);
  void CalculateMaxUnitKindCount(ScheduleProcessInfo &scheduleInfo);
  static uint32 maxUnitIndex;
};
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_SCHEDULE_H */
