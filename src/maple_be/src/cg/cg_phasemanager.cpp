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
#include "cg_phasemanager.h"
#include <vector>
#include <string>

#include "cg_option.h"
#include "live.h"
#include "loop.h"
#include "mpl_timer.h"
#include "args.h"
#include "yieldpoint.h"
#include "label_creation.h"
#include "offset_adjust.h"
#include "proepilog.h"

namespace maplebe {
#define JAVALANG (module.IsJavaModule())
#define CLANG (module.GetSrcLang() == kSrcLangC)

void CgFuncPhaseManager::RunFuncPhase(CGFunc &func, FuncPhase &phase) {
  /*
   * 1. check options.enable(phase.id())
   * 2. options.tracebeforePhase(phase.id()) dumpIR before
   */
  if (!CGOptions::IsQuiet()) {
    LogInfo::MapleLogger() << "---Run Phase [ " << phase.PhaseName() << " ]---\n";
  }

  /* 3. run: skip mplcg phase except "emit" if no cfg in CGFunc */
  AnalysisResult *analysisRes = nullptr;
  if ((func.NumBBs() > 0) || (phase.GetPhaseID() == kCGFuncPhaesEMIT)) {
    analysisRes = phase.Run(&func, &arFuncManager);
    phase.ReleaseMemPool(analysisRes == nullptr ? nullptr : analysisRes->GetMempool());
  }

  if (analysisRes != nullptr) {
    /* if phase is an analysis Phase, add result to arm */
    arFuncManager.AddResult(phase.GetPhaseID(), func, *analysisRes);
  }
}

void CgFuncPhaseManager::RegisterFuncPhases() {
  /* register all Funcphases defined in cg_phases.def */
#define FUNCTPHASE(id, cgPhase)                                                                           \
  do {                                                                                                    \
    RegisterPhase(id, *(new (GetMemAllocator()->GetMemPool()->Malloc(sizeof(cgPhase(id)))) cgPhase(id))); \
  } while (0);

#define FUNCAPHASE(id, cgPhase)                                                                           \
  do {                                                                                                    \
    RegisterPhase(id, *(new (GetMemAllocator()->GetMemPool()->Malloc(sizeof(cgPhase(id)))) cgPhase(id))); \
    arFuncManager.AddAnalysisPhase(id, (static_cast<FuncPhase*>(GetPhase(id))));                          \
  } while (0);

#include "cg_phases.def"
#undef FUNCTPHASE
#undef FUNCAPHASE
}

#define ADDPHASE(phaseName)                 \
  if (!CGOptions::IsSkipPhase(phaseName)) { \
    phases.push_back(phaseName);            \
  }

void CgFuncPhaseManager::AddPhases(std::vector<std::string> &phases) {
  if (phases.empty()) {
    if (cgPhaseType == kCgPhaseMainOpt) {
      /* default phase sequence */
      ADDPHASE("layoutstackframe");
      ADDPHASE("createstartendlabel");
      if (!CLANG) {
        ADDPHASE("buildehfunc");
      }
      ADDPHASE("handlefunction");
      ADDPHASE("moveargs");

      ADDPHASE("regalloc");
      ADDPHASE("generateproepilog");
      ADDPHASE("offsetadjustforfplr");

      if (!CLANG) {
        ADDPHASE("gencfi");
      }
      if (JAVALANG && CGOptions::IsInsertYieldPoint()) {
        ADDPHASE("yieldpoint");
      }
      ADDPHASE("emit");
    }
  }
  for (const auto &phase : phases) {
    AddPhase(phase);
  }
  ASSERT(phases.size() == GetPhaseSequence()->size(), "invalid phase name");
}

void CgFuncPhaseManager::Emit(CGFunc &func) {
  PhaseID id = kCGFuncPhaesEMIT;
  FuncPhase *funcPhase = static_cast<FuncPhase*>(GetPhase(id));
  CHECK_FATAL(funcPhase != nullptr, "p is null in CgFuncPhaseManager::Run");

  const std::string kPhaseBeforeEmit = "fixshortbranch";
  funcPhase->SetPreviousPhaseName(kPhaseBeforeEmit); /* prev phase name is for filename used in emission after phase */
  MPLTimer timer;
  bool timePhases = CGOptions::IsEnableTimePhases();
  if (timePhases) {
    timer.Start();
  }
  RunFuncPhase(func, *funcPhase);
  if (timePhases) {
    timer.Stop();
    phaseTimers.back() += timer.ElapsedMicroseconds();
  }

  const std::string &phaseName = funcPhase->PhaseName();  /* new phase name */
  bool dumpPhases = CGOptions::DumpPhase(phaseName);
  bool dumpFunc = CGOptions::FuncFilter(func.GetName());
  if (((CGOptions::IsDumpAfter() && dumpPhases) || dumpPhases) && dumpFunc) {
    LogInfo::MapleLogger() << "******** CG IR After " << phaseName << ": *********" << "\n";
    func.DumpCGIR();
  }
}

void CgFuncPhaseManager::Run(CGFunc &func) {
  if (!CGOptions::IsQuiet()) {
    LogInfo::MapleLogger() << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>> Optimizing Function  < " << func.GetName() << " >---\n";
  }
  std::string phaseName = "";
  /* each function level phase */
  bool dumpFunc = CGOptions::FuncFilter(func.GetName());
  size_t phaseIndex = 0;
  bool timePhases = CGOptions::IsEnableTimePhases();
  bool skipFromFlag = false;
  bool skipAfterFlag = false;
  MPLTimer timer;
  for (auto it = PhaseSequenceBegin(); it != PhaseSequenceEnd(); it++, ++phaseIndex) {
    PhaseID id = GetPhaseId(it);
    if (id == kCGFuncPhaesEMIT) {
      continue;
    }
    FuncPhase *funcPhase = static_cast<FuncPhase*>(GetPhase(id));
    CHECK_FATAL(funcPhase != nullptr, "funcPhase is null in CgFuncPhaseManager::Run");

    if (!skipFromFlag && CGOptions::IsSkipFromPhase(funcPhase->PhaseName())) {
      skipFromFlag = true;
    }
    if (skipFromFlag) {
      while (funcPhase->CanSkip() && (++it != PhaseSequenceEnd())) {
        id = GetPhaseId(it);
        funcPhase = static_cast<FuncPhase*>(GetPhase(id));
        CHECK_FATAL(funcPhase != nullptr, "null ptr check ");
      }
    }

    funcPhase->SetPreviousPhaseName(phaseName); /* prev phase name is for filename used in emission after phase */
    phaseName = funcPhase->PhaseName();         /* new phase name */
    bool dumpPhase = IS_STR_IN_SET(CGOptions::GetDumpPhases(), phaseName);
    if (CGOptions::IsDumpBefore() && dumpFunc && dumpPhase) {
      LogInfo::MapleLogger() << "******** CG IR Before " << phaseName << ": *********" << "\n";
      func.DumpCGIR();
    }
    if (timePhases) {
      timer.Start();
    }
    RunFuncPhase(func, *funcPhase);
    if (timePhases) {
      timer.Stop();
      CHECK_FATAL(phaseIndex < phaseTimers.size(), "invalid index for phaseTimers");
      phaseTimers[phaseIndex] += timer.ElapsedMicroseconds();
    }
    bool dumpPhases = CGOptions::DumpPhase(phaseName);
    if (((CGOptions::IsDumpAfter() && dumpPhase) || dumpPhases) && dumpFunc) {
      if (id == kCGFuncPhaseBUILDEHFUNC) {
        LogInfo::MapleLogger() << "******** Maple IR After buildehfunc: *********" << "\n";
        func.GetFunction().Dump();
      }
      LogInfo::MapleLogger() << "******** CG IR After " << phaseName << ": *********" << "\n";
      func.DumpCGIR();
    }
    if (!skipAfterFlag && CGOptions::IsSkipAfterPhase(funcPhase->PhaseName())) {
      skipAfterFlag = true;
    }
    if (skipAfterFlag) {
      while (++it != PhaseSequenceEnd()) {
        id = GetPhaseId(it);
        funcPhase = static_cast<FuncPhase*>(GetPhase(id));
        CHECK_FATAL(funcPhase != nullptr, "null ptr check ");
        if (!funcPhase->CanSkip()) {
          break;
        }
      }
      --it;  /* restore iterator */
    }
  }
}

void CgFuncPhaseManager::ClearPhaseNameInfo() {
  for (auto it = PhaseSequenceBegin(); it != PhaseSequenceEnd(); ++it) {
    PhaseID id = GetPhaseId(it);
    FuncPhase *funcPhase = static_cast<FuncPhase*>(GetPhase(id));
    if (funcPhase == nullptr) {
      continue;
    }
    funcPhase->ClearString();
  }
}

int64 CgFuncPhaseManager::GetOptimizeTotalTime() const {
  int64 total = 0;
  for (size_t i = 0; i < phaseTimers.size(); ++i) {
    total += phaseTimers[i];
  }
  return total;
}

int64 CgFuncPhaseManager::DumpCGTimers() {
  int64 total = GetOptimizeTotalTime();
  total += extraTotal;
  for (size_t i = 0; i < phaseTimers.size(); ++i) {
    CHECK_FATAL(total != 0, "calculation check");
    /*
     * output information by specified format, setw function parameter specifies show width
     * setprecision function parameter specifies precision
     */
    LogInfo::MapleLogger() << std::left << std::setw(25) << registeredPhases[phaseSequences[i]]->PhaseName() <<
                              std::setw(10) << std::right << std::fixed << std::setprecision(2) <<
                              (kPercent * phaseTimers[i] / total) << "%" << std::setw(10) << std::setprecision(0) <<
                              (phaseTimers[i] / kMicroSecPerMilliSec) << "ms" << "\n";
  }
  LogInfo::flags();
  return total;
}
}  /* namespace maplebe */
