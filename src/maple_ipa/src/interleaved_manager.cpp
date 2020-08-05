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
#include "interleaved_manager.h"
#include <string>
#include <vector>
#include <iomanip>
#include "module_phase.h"
#include "mir_function.h"
#include "me_option.h"
#include "mempool.h"
#include "phase_manager.h"
#include "mpl_timer.h"

namespace maple {
std::vector<std::pair<std::string, time_t>> InterleavedManager::interleavedTimer;
void InterleavedManager::AddPhases(const std::vector<std::string> &phases, bool isModulePhase, bool timePhases,
                                   bool genMpl) {
  ModuleResultMgr *mrm = nullptr;
  if (!phaseManagers.empty()) {
    // ModuleResult such class hierarchy need to be carried on
    PhaseManager *pm = phaseManagers.back();
    mrm = pm->GetModResultMgr();
  }

  if (isModulePhase) {
    auto *mpm = GetMemPool()->New<ModulePhaseManager>(GetMemPool(), mirModule, mrm);
    mpm->RegisterModulePhases();
    mpm->AddModulePhases(phases);
    if (timePhases) {
      mpm->SetTimePhases(true);
    }
    phaseManagers.push_back(mpm);
  } else {  // MeFuncPhase
    auto *fpm = GetMemPool()->New<MeFuncPhaseManager>(GetMemPool(), mirModule, mrm);
    fpm->RegisterFuncPhases();
    if (genMpl) {
      fpm->SetGenMeMpl(true);
    }
    if (timePhases) {
      fpm->SetTimePhases(true);
    }
    fpm->AddPhasesNoDefault(phases);
    phaseManagers.push_back(fpm);
  }
}


void InterleavedManager::OptimizeFuncs(MeFuncPhaseManager &fpm, MapleVector<MIRFunction*> &compList) {
  for (size_t i = 0; i < compList.size(); ++i) {
    MIRFunction *func = compList[i];
    ASSERT_NOT_NULL(func);
    // skip empty func, and skip the func out of range  if `useRange` is true
    if (func->GetBody() == nullptr || (MeOption::useRange && (i < MeOption::range[0] || i > MeOption::range[1]))) {
      continue;
    }
    mirModule.SetCurFunction(func);
    // lower, create BB and build cfg
    fpm.Run(func, i, meInput);
  }
}


void InterleavedManager::Run() {
  MPLTimer optTimer;
  for (auto *pm : phaseManagers) {
    if (pm == nullptr) {
      continue;
    }
    auto *fpm = dynamic_cast<MeFuncPhaseManager*>(pm);
    if (fpm == nullptr) {
      optTimer.Start();
      pm->Run();
      optTimer.Stop();
      LogInfo::MapleLogger() << "[mpl2mpl]" << " Module phases cost " << optTimer.ElapsedMilliseconds() << "ms\n";
      continue;
    }
    if (fpm->GetPhaseSequence()->empty()) {
      continue;
    }
    RunMeOptimize(*fpm);
  }
}

void InterleavedManager::RunMeOptimize(MeFuncPhaseManager &fpm) {
  MapleVector<MIRFunction*> *compList;
  if (!mirModule.GetCompilationList().empty()) {
    if ((mirModule.GetCompilationList().size() != mirModule.GetFunctionList().size()) &&
        (mirModule.GetCompilationList().size() !=
          mirModule.GetFunctionList().size() - mirModule.GetOptFuncsSize())) {
      ASSERT(false, "should be equal");
    }
    compList = &mirModule.GetCompilationList();
  } else {
    compList = &mirModule.GetFunctionList();
  }
  MPLTimer optTimer;
  optTimer.Start();
  std::string logPrefix = mirModule.IsInIPA() ? "[ipa]" : "[me]";
  OptimizeFuncs(fpm, *compList);
  optTimer.Stop();
  LogInfo::MapleLogger() << logPrefix << " Function phases cost " << optTimer.ElapsedMilliseconds() << "ms\n";
  optTimer.Start();
  if (fpm.GetGenMeMpl()) {
    mirModule.Emit("comb.me.mpl");
  }
  optTimer.Stop();
  genMeMplTime += optTimer.ElapsedMicroseconds();
}

void InterleavedManager::DumpTimers() {
  std::ios_base::fmtflags f(LogInfo::MapleLogger().flags());
  auto TimeLogger = [](const std::string &itemName, time_t itemTimeUs, time_t totalTimeUs) {
    LogInfo::MapleLogger() << std::left << std::setw(25) << itemName << std::setw(10)
                           << std::right << std::fixed << std::setprecision(2)
                           << (100.0 * itemTimeUs / totalTimeUs) << "%" << std::setw(10)
                           << std::setprecision(0) << (itemTimeUs / 1000.0) << "ms\n";
  };
  std::vector<std::pair<std::string, time_t>> timeVec;
  long total = 0;
  long parserTotal = 0;
  LogInfo::MapleLogger() << "==================== PARSER ====================\n";
  for (const auto &parserTimer : interleavedTimer) {
    parserTotal += parserTimer.second;
  }
  for (const auto &parserTimer : interleavedTimer) {
    TimeLogger(parserTimer.first, parserTimer.second, parserTotal);
  }
  total += parserTotal;
  timeVec.emplace_back(std::pair<std::string, time_t>("parser", parserTotal));
  LogInfo::MapleLogger() << "================== TIMEPHASES ==================\n";
  for (auto *manager : phaseManagers) {
    long temp = manager->DumpTimers();
    total += temp;
    timeVec.push_back(std::pair<std::string, time_t>(manager->GetMgrName(), temp));
    LogInfo::MapleLogger() << "================================================\n";
  }
  total += genMeMplTime;
  total += emitVtableImplMplTime;
  timeVec.emplace_back(std::pair<std::string, time_t>("genMeMplFile", genMeMplTime));
  timeVec.emplace_back(std::pair<std::string, time_t>("emitVtableImplMpl", emitVtableImplMplTime));
  timeVec.emplace_back(std::pair<std::string, time_t>("Total", total));
  LogInfo::MapleLogger() << "=================== SUMMARY ====================\n";
  CHECK_FATAL(total != 0, "calculation check");
  for (const auto &lapse : timeVec) {
    TimeLogger(lapse.first, lapse.second, total);
  }
  LogInfo::MapleLogger() << "================================================\n";
  LogInfo::MapleLogger().flags(f);
}

void InterleavedManager::InitSupportPhaseManagers() {
  ASSERT(supportPhaseManagers.empty(), "Phase managers already initialized");

  auto *mpm = GetMemPool()->New<ModulePhaseManager>(GetMemPool(), mirModule, nullptr);
  mpm->RegisterModulePhases();
  supportPhaseManagers.push_back(mpm);

  ModuleResultMgr *mrm = mpm->GetModResultMgr();

  auto *fpm = GetMemPool()->New<MeFuncPhaseManager>(GetMemPool(), mirModule, mrm);
  fpm->RegisterFuncPhases();
  supportPhaseManagers.push_back(fpm);
}

const PhaseManager *InterleavedManager::GetSupportPhaseManager(const std::string &phase) {
  if (supportPhaseManagers.empty()) {
    InitSupportPhaseManagers();
  }

  for (auto pm : supportPhaseManagers) {
    if (pm->ExistPhase(phase)) {
      return pm;
    }
  }

  return nullptr;
}
} // namespace maple
