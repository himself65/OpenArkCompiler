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
#ifndef MAPLE_PHASE_INCLUDE_PHASE_MANAGER_H
#define MAPLE_PHASE_INCLUDE_PHASE_MANAGER_H
#include <iomanip>
#include "phase.h"
#include "module_phase.h"

namespace maple {
class PhaseManager {
 public:
  PhaseManager(MemPool &memPool, const std::string &name)
      : managerName(name),
        allocator(&memPool),
        registeredPhases(std::less<PhaseID>(), allocator.Adapter()),
        phaseSequences(allocator.Adapter()),
        phaseTimers(allocator.Adapter()),
        extraMeTimers(allocator.Adapter()) {}

  virtual ~PhaseManager() = default;

  void AddPhase(const std::string &paseName) {
    for (auto it = RegPhaseBegin(); it != RegPhaseEnd(); ++it) {
      if (GetPhaseName(it) == paseName) {
        phaseSequences.push_back(GetPhaseId(it));
        phaseTimers.push_back(0);
        return;
      }
    }
    CHECK_FATAL(false, "%s is not a valid phase name", paseName.c_str());
  }

  void RegisterPhase(PhaseID id, Phase &p) {
    registeredPhases[id] = &p;
  }

  Phase *GetPhaseFromName(const std::string &phaseName) {
    for (auto it = RegPhaseBegin(); it != RegPhaseEnd(); ++it) {
      if (GetPhaseName(it) == phaseName) {
        return GetPhase(GetPhaseId(it));
      }
    }
    CHECK_FATAL(false, "not a valid phase");
    return nullptr;
  }

  Phase *GetPhase(PhaseID id) {
    if (registeredPhases.find(id) != registeredPhases.end()) {
      return registeredPhases[id];
    }
    CHECK_FATAL(false, "not a valid phase");
    return nullptr;
  }

  virtual void Run() {
    CHECK_FATAL(false, "should not run here");
  }

  virtual ModuleResultMgr *GetModResultMgr() {
    CHECK_FATAL(false, "should not run here");
    return nullptr;
  }

  MapleAllocator *GetMemAllocator() {
    return &allocator;
  }

  MemPool *GetMemPool() const {
    return allocator.GetMemPool();
  }

  const MapleVector<PhaseID> *GetPhaseSequence() const {
    return &phaseSequences;
  }

  // iterator for register_phases
  using iterator = MapleMap<PhaseID, Phase*>::iterator;
  iterator RegPhaseBegin() {
    return registeredPhases.begin();
  }

  iterator RegPhaseEnd() {
    return registeredPhases.end();
  }

  const std::string GetPhaseName(iterator it) {
    return it->second->PhaseName();
  }

  const std::string &GetMgrName() const {
    return managerName;
  }

  PhaseID GetPhaseId(iterator it) const {
    return it->first;
  }

  // iterator for phaseSeq
  using PhaseSeqIterator = MapleVector<PhaseID>::iterator;
  PhaseSeqIterator PhaseSequenceBegin() {
    return phaseSequences.begin();
  }

  PhaseSeqIterator PhaseSequenceEnd() {
    return phaseSequences.end();
  }

  PhaseID GetPhaseId(const PhaseSeqIterator &it) const {
    return (*it);
  }

  bool ExistPhase(const std::string &name) {
    for (auto it = RegPhaseBegin(); it != RegPhaseEnd(); ++it) {
      if (GetPhaseName(it) == name) {
        return true;
      }
    }
    return false;
  }

  long DumpTimers() {
    auto TimeLogger = [](const std::string &itemName, time_t itemTimeUs, time_t totalTimeUs) {
      LogInfo::MapleLogger() << std::left << std::setw(25) << itemName << std::setw(10)
                             << std::right << std::fixed << std::setprecision(2)
                             << (100.0 * itemTimeUs / totalTimeUs) << "%" << std::setw(10)
                             << std::setprecision(0) << (itemTimeUs / 1000.0) << "ms\n";
    };
    long total = 0;
    // extra timer
    for (auto it = extraMeTimers.cbegin(); it != extraMeTimers.cend(); ++it) {
      total += it->second;
    }
    bool isMePM = total > 0;
    for (size_t i = 0; i < phaseTimers.size(); ++i) {
      total += phaseTimers[i];
    }
    // Dump extra timer
    ASSERT(total != 0, "calculation check");
    std::ios::fmtflags f(LogInfo::MapleLogger().flags());
    if (isMePM) {
      for (auto it = extraMeTimers.cbegin(); it != extraMeTimers.cend(); ++it) {
        TimeLogger(it->first, it->second, total);
      }
    }
    for (size_t i = 0; i < phaseTimers.size(); ++i) {
      ASSERT(registeredPhases[phaseSequences[i]] != nullptr, "Phase null ptr check");
      TimeLogger(registeredPhases[phaseSequences[i]]->PhaseName(), phaseTimers[i], total);
    }
    LogInfo::MapleLogger().flags(f);
    return total;
  }

  const MapleMap<PhaseID, Phase*> &GetRegisteredPhases() const {
    return registeredPhases;
  }

 protected:
  std::string managerName;
  MapleAllocator allocator;
  MapleMap<PhaseID, Phase*> registeredPhases;
  MapleVector<PhaseID> phaseSequences;
  MapleVector<long> phaseTimers;
  MapleMap<std::string, long> extraMeTimers;
};
}  // namespace maple
#endif  // MAPLE_PHASE_INCLUDE_PHASE_MANAGER_H
