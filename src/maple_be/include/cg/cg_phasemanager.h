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
#ifndef MAPLEBE_INCLUDE_CG_CG_PHASEMANAGER_H
#define MAPLEBE_INCLUDE_CG_CG_PHASEMANAGER_H
#include <vector>
#include <string>
#include "mempool.h"
#include "mempool_allocator.h"
#include "phase_manager.h"
#include "mir_module.h"
#include "cgfunc.h"
#include "cg_phase.h"
#include "cg_option.h"
namespace maplebe {
enum CgPhaseType : uint8 {
  kCgPhaseInvalid,
  kCgPhaseMainOpt,
  kCgPhaseLno
};

/* driver of Cg */
class CgFuncPhaseManager : public PhaseManager {
 public:
  CgFuncPhaseManager(MemPool &memPool, MIRModule &mod)
      : PhaseManager(memPool, "cg manager"),
        cgPhaseType(kCgPhaseInvalid),
        arFuncManager(GetMemAllocator()),
        module(mod) {}

  ~CgFuncPhaseManager() {
    arFuncManager.InvalidAllResults();
    if (CGOptions::IsEnableTimePhases()) {
      DumpCGTimers();
    }
  }

  void RunFuncPhase(CGFunc &func, FuncPhase &phase);
  void RegisterFuncPhases();
  void AddPhases(std::vector<std::string> &phases);

  void SetCGPhase(CgPhaseType cgPhase) {
    cgPhaseType = cgPhase;
  }
  void ClearPhaseNameInfo();
  void Emit(CGFunc &func);
  void Run(CGFunc &func);
  void Run() override {}

  void SetExtraTotalTime(int64 total) {
    extraTotal = total;
  }
  int64 GetOptimizeTotalTime() const;
  int64 DumpCGTimers();

  CgFuncResultMgr *GetAnalysisResultManager() {
    return &arFuncManager;
  }
  CgPhaseType cgPhaseType;

 private:
  /* analysis phase result manager */
  CgFuncResultMgr arFuncManager;
  MIRModule &module;
  int64 extraTotal = 0L;
};
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_CG_PHASEMANAGER_H */