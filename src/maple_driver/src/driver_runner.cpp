/*
 * Copyright (c) [2019] Huawei Technologies Co.,Ltd.All rights reserved.
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
#include "driver_runner.h"
#include <iostream>
#include <typeinfo>
#include <sys/stat.h>
#include "mpl_timer.h"
#include "mir_function.h"
#include "mir_parser.h"

#define JAVALANG (theModule->IsJavaModule())

#define CHECK_MODULE(errorCode...)                                              \
  do {                                                                          \
    if (theModule == nullptr) {                                                 \
      LogInfo::MapleLogger() << "Fatal error: the module is null" << '\n';      \
      return errorCode;                                                         \
    }                                                                           \
  } while (0)

#define RELEASE(pointer)      \
  do {                        \
    if (pointer != nullptr) { \
      delete pointer;         \
      pointer = nullptr;      \
    }                         \
  } while (0)

#define ADD_PHASE(name, condition)       \
  if ((condition)) {                     \
    phases.push_back(std::string(name)); \
  }

#define ADD_EXTRA_PHASE(name, timephases, timeStart)                                                    \
  if (timephases) {                                                                                     \
    auto duration = std::chrono::system_clock::now() - (timeStart);                                       \
    extraPhasesTime.push_back(std::chrono::duration_cast<std::chrono::microseconds>(duration).count()); \
    extraPhasesName.push_back(name);                                                                    \
  }

namespace maple {

const std::string mpl2Mpl = "mpl2mpl";
const std::string mplME = "me";

enum OptLevel {
  kLevelO0,
  kLevelO1,
  kLevelO2
};

ErrorCode DriverRunner::Run() {
  CHECK_MODULE(kErrorExit);

  if (exeNames.empty()) {
    LogInfo::MapleLogger() << "Fatal error: no exe specified" << '\n';
    return kErrorExit;
  }

  printOutExe = exeNames[exeNames.size() - 1];

  // Prepare output file
  auto lastDot = actualInput.find_last_of(".");
  std::string baseName = (lastDot == std::string::npos) ? actualInput : actualInput.substr(0, lastDot);
  std::string originBaseName = baseName;
  std::string outputFile = baseName.append(GetPostfix());

  ErrorCode ret = ParseInput(outputFile, originBaseName);

  if (ret != kErrorNoError) {
    return kErrorExit;
  }
  if (mpl2mplOptions || meOptions) {
    std::string vtableImplFile = originBaseName;
    vtableImplFile.append(".VtableImpl.mpl");
    originBaseName.append(".VtableImpl");
    ProcessMpl2mplAndMePhases(outputFile, vtableImplFile);
  }
  return kErrorNoError;
}

bool DriverRunner::FuncOrderLessThan(const MIRFunction *left, const MIRFunction *right) {
  return left->GetLayoutType() < right->GetLayoutType();
}

bool DriverRunner::IsFramework() const {
  return false;
}

std::string DriverRunner::GetPostfix() const {
  if (printOutExe == mplME) {
    return ".me.mpl";
  }
  if (printOutExe == mpl2Mpl) {
    return ".VtableImpl.mpl";
  }
  return "";
}

ErrorCode DriverRunner::ParseInput(const std::string &outputFile, const std::string &originBaseName) const {
  CHECK_MODULE(kErrorExit);

  LogInfo::MapleLogger() << "Starting parse input" << '\n';
  MPLTimer timer;
  timer.Start();

  MIRParser parser(*theModule);
  ErrorCode ret = kErrorNoError;
  bool parsed = parser.ParseMIR(0, 0, false, true);
  if (!parsed) {
    ret = kErrorExit;
    parser.EmitError(outputFile);
  }
  timer.Stop();
  LogInfo::MapleLogger() << "Parse consumed " << timer.Elapsed() << "s" << '\n';

  return ret;
}

bool DriverRunner::VerifyModule(MIRModulePtr &mModule) const {
  LogInfo::MapleLogger() << "========== Starting Verify Module =====================" << '\n';
  bool res = true;
  LogInfo::MapleLogger() << "========== Finished Verify Module =====================" << '\n';
  return res;
}

void DriverRunner::ProcessMpl2mplAndMePhases(const std::string &outputFile, const std::string &vtableImplFile) const {
  CHECK_MODULE();

  if (mpl2mplOptions || meOptions) {
    LogInfo::MapleLogger() << "Processing mpl2mpl&mplme" << '\n';
    MPLTimer timer;
    timer.Start();

    InterleavedManager mgr(optMp, theModule, meInput, timePhases);
    std::vector<std::string> phases;
#include "phases.def"
    InitPhases(mgr, phases);
    mgr.Run();

    theModule->Emit(vtableImplFile);

    timer.Stop();
    LogInfo::MapleLogger() << "Mpl2mpl&mplme consumed " << timer.Elapsed() << "s" << '\n';
  }
}

void DriverRunner::InitPhases(InterleavedManager &mgr, const std::vector<std::string> &phases) const {
  if (phases.empty()) {
    return;
  }

  const PhaseManager *curManager = nullptr;
  std::vector<std::string> curPhases;

  for (const std::string &phase : phases) {
    auto temp = mgr.GetSupportPhaseManager(phase);
    if (temp != nullptr) {
      if (temp != curManager) {
        if (curManager != nullptr) {
          AddPhases(mgr, curPhases, *curManager);
        }
        curManager = temp;
        curPhases.clear();
      }

      CHECK_FATAL(curManager != nullptr, "Invalid phase manager");
      AddPhase(curPhases, phase, *curManager);
    }
  }

  if (curManager != nullptr) {
    AddPhases(mgr, curPhases, *curManager);
  }
}

void DriverRunner::AddPhases(InterleavedManager &mgr, const std::vector<std::string> &phases,
                             const PhaseManager &phaseManager) const {
  const auto &type = typeid(phaseManager);
  if (type == typeid(ModulePhaseManager)) {
    mgr.AddPhases(phases, true, timePhases);
  } else if (type == typeid(MeFuncPhaseManager)) {
    mgr.AddPhases(phases, false, timePhases, genMeMpl);
  } else {
    CHECK_FATAL(false, "Should not reach here, phases should be handled");
  }
}

void DriverRunner::AddPhase(std::vector<std::string> &phases, const std::string phase,
                            const PhaseManager &phaseManager) const {
  if (typeid(phaseManager) == typeid(ModulePhaseManager)) {
    if (mpl2mplOptions && Options::skipPhase.compare(phase) != 0) {
      phases.push_back(phase);
    }
  } else if (typeid(phaseManager) == typeid(MeFuncPhaseManager)) {
    if (meOptions && meOptions->GetSkipPhases().find(phase) == meOptions->GetSkipPhases().end()) {
      phases.push_back(phase);
    }
  } else {
    CHECK_FATAL(false, "Should not reach here, phase should be handled");
  }
}

}  // namespace maple
