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
#include "driver_runner.h"
#include <iostream>
#include <typeinfo>
#include <sys/stat.h>
#include "mpl_timer.h"
#include "mir_function.h"
#include "mir_parser.h"
#include "file_utils.h"

#include "lower.h"
#if TARGAARCH64
#include "aarch64/aarch64_cg.h"
#include "aarch64/aarch64_emitter.h"
#elif TARGARM32
#include "arm32/arm32_cg.h"
#include "arm32/arm32_emitter.h"
#else
#error "Unsupported target"
#endif

using namespace maplebe;

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

#define ADD_EXTRA_PHASE(name, timephases, timeStart)                                                       \
  if (timephases) {                                                                                        \
    auto duration = std::chrono::system_clock::now() - (timeStart);                                        \
    extraPhasesTime.emplace_back(std::chrono::duration_cast<std::chrono::microseconds>(duration).count()); \
    extraPhasesName.emplace_back(name);                                                                    \
  }

namespace maple {
const std::string kMplCg = "mplcg";
const std::string kMpl2mpl = "mpl2mpl";
const std::string kMplMe = "me";

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
  if (mpl2mplOptions != nullptr || meOptions != nullptr) {
    std::string vtableImplFile = originBaseName;
    vtableImplFile.append(".VtableImpl.mpl");
    originBaseName.append(".VtableImpl");
    ProcessMpl2mplAndMePhases(outputFile, vtableImplFile);
  }
  ProcessCGPhase(outputFile, originBaseName);
  return kErrorNoError;
}

bool DriverRunner::IsFramework() const {
  return false;
}

std::string DriverRunner::GetPostfix() const {
  if (printOutExe == kMplMe) {
    return ".me.mpl";
  }
  if (printOutExe == kMpl2mpl) {
    return ".VtableImpl.mpl";
  }
  if (printOutExe == kMplCg) {
    return ".VtableImpl.s";
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
  bool parsed;
  if (!fileParsed) {
    MPLTimer parseMirTimer;
    parseMirTimer.Start();
    parsed = parser.ParseMIR(0, 0, false, true);
    parseMirTimer.Stop();
    InterleavedManager::interleavedTimer.emplace_back(
        std::pair<std::string, time_t>("parseMpl", parseMirTimer.ElapsedMicroseconds()));
    if (!parsed) {
      ret = kErrorExit;
      parser.EmitError(outputFile);
    }
  }
  timer.Stop();
  LogInfo::MapleLogger() << "Parse consumed " << timer.Elapsed() << "s" << '\n';
  return ret;
}

void DriverRunner::ProcessMpl2mplAndMePhases(const std::string &outputFile, const std::string &vtableImplFile) const {
  CHECK_MODULE();
  theMIRModule = theModule;
  if (mpl2mplOptions != nullptr || meOptions != nullptr) {
    LogInfo::MapleLogger() << "Processing mpl2mpl&mplme" << '\n';

    InterleavedManager mgr(optMp, theModule, meInput, timePhases);
    std::vector<std::string> phases;
#include "phases.def"
    InitPhases(mgr, phases);
    MPLTimer timer;
    timer.Start();
    mgr.Run();
    MPLTimer emitVtableMplTimer;
    emitVtableMplTimer.Start();
    // emit after module phase
    if (printOutExe == kMpl2mpl || printOutExe == kMplMe) {
      theModule->Emit(outputFile);
    } else if (genVtableImpl || Options::emitVtableImpl) {
      theModule->Emit(vtableImplFile);
    }
    emitVtableMplTimer.Stop();
    mgr.SetEmitVtableImplTime(emitVtableMplTimer.ElapsedMicroseconds());
    timer.Stop();
    LogInfo::MapleLogger() << " Mpl2mpl&mplme consumed " << timer.Elapsed() << "s" << '\n';
  }
}

void DriverRunner::InitPhases(InterleavedManager &mgr, const std::vector<std::string> &phases) const {
  if (phases.empty()) {
    return;
  }

  const PhaseManager *curManager = nullptr;
  std::vector<std::string> curPhases;

  for (const std::string &phase : phases) {
    const PhaseManager *supportManager = mgr.GetSupportPhaseManager(phase);
    if (supportManager != nullptr) {
      if (curManager != nullptr && curManager != supportManager && !curPhases.empty()) {
        AddPhases(mgr, curPhases, *curManager);
        curPhases.clear();
      }

      if (curManager != supportManager) {
        curManager = supportManager;
      }
      AddPhase(curPhases, phase, *supportManager);
    }
  }

  if (curManager != nullptr && !curPhases.empty()) {
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

void DriverRunner::ProcessCGPhase(const std::string &outputFile, const std::string &originBaseName) {
  CHECK_MODULE();
  if (cgOptions == nullptr) {
    return;
  }

  LogInfo::MapleLogger() << "Processing mplcg" << '\n';
  MPLTimer timer;
  timer.Start();

  cgOptions->SetDefaultOptions(*theModule);
  if (timePhases) {
    CGOptions::EnableTimePhases();
  }

  // store lower time, emit time and so on.
  std::vector<long> extraPhasesTime;
  std::vector<std::string> extraPhasesName;

  Globals::GetInstance()->SetOptimLevel(cgOptions->GetOptimizeLevel());
  MAD mad;
  Globals::GetInstance()->SetMAD(mad);

  CgFuncPhaseManager cgfpm(*optMp, *theModule);
  cgfpm.RegisterFuncPhases();
  cgfpm.SetCGPhase(kCgPhaseMainOpt);
  cgfpm.AddPhases(cgOptions->GetSequence());

  std::chrono::system_clock::time_point timeStart = std::chrono::system_clock::now();
  CG *cg = CreateCGAndBeCommon(outputFile, originBaseName);
  ADD_EXTRA_PHASE("createcg&becommon", CGOptions::IsEnableTimePhases(), timeStart);
  if (cgOptions->IsRunCG()) {
    // Generate the output file
    CHECK_FATAL(cg != nullptr, "cg is null");
    CHECK_FATAL(cg->GetEmitter(), "emitter is null");
    if (!cgOptions->SuppressFileInfo()) {
      cg->GetEmitter()->EmitFileInfo(actualInput);
    }
    // Run the cg optimizations phases
    RunCGFunctions(*cg, cgfpm, extraPhasesTime, extraPhasesName);

    // Emit global info
    timeStart = std::chrono::system_clock::now();
    EmitGlobalInfo(*cg);
    ADD_EXTRA_PHASE("emitglobalinfo", CGOptions::IsEnableTimePhases(), timeStart);
  } else {
    LogInfo::MapleLogger(kLlErr) << "Skipped generating .s because -no-cg is given" << '\n';
  }

  ProcessExtraTime(extraPhasesTime, extraPhasesName, cgfpm);

  RELEASE(cg);
  RELEASE(beCommon);

  timer.Stop();
  LogInfo::MapleLogger() << "Mplcg consumed " << timer.Elapsed() << "s" << '\n';
}

CG *DriverRunner::CreateCGAndBeCommon(const std::string &outputFile, const std::string &originBaseName) {
  CG *cg = nullptr;

#if TARGAARCH64
  cg = new AArch64CG(*theModule, *cgOptions, cgOptions->GetEHExclusiveFunctionNameVec(),
                     CGOptions::GetCyclePatternMap());
  cg->SetEmitter(*theModule->GetMemPool()->New<AArch64AsmEmitter>(*cg, outputFile));
#elif TARGARM32
  cg = new Arm32CG(*theModule, *cgOptions, cgOptions->GetEHExclusiveFunctionNameVec(),
                   CGOptions::GetCyclePatternMap());
  cg->SetEmitter(*theModule->GetMemPool()->New<Arm32AsmEmitter>(*cg, outputFile));
#else
#error "unknown platform"
#endif

  // Must be done before creating any BECommon instances.
  //
  // BECommon, when constructed, will calculate the type, size and align of all types.  As a side effect, it will also
  // lower ptr and ref types into a64. That will drop the information of what a ptr or ref points to.
  //
  // All metadata generation passes which depend on the pointed-to type must be done here.
  cg->GenPrimordialObjectList(originBaseName);

  // We initialize a couple of BECommon's tables using the size information of GlobalTables.type_table_.
  // So, BECommon must be allocated after all the parsing is done and user-defined types are all acounted.
  beCommon = new BECommon(*theModule);
  Globals::GetInstance()->SetBECommon(*beCommon);

  // If a metadata generation pass depends on object layout it must be done after creating BECommon.
  cg->GenExtraTypeMetadata(cgOptions->GetClassListFile(), originBaseName);

  if (cg->NeedInsertInstrumentationFunction()) {
    CHECK_FATAL(cgOptions->IsInsertCall(), "handling of --insert-call is not correct");
    cg->SetInstrumentationFunction(cgOptions->GetInstrumentationFunction());
  }

  return cg;
}


void DriverRunner::RunCGFunctions(CG &cg, CgFuncPhaseManager &cgfpm, std::vector<long> &extraPhasesTime,
                                  std::vector<std::string> &extraPhasesName) const {
  MPLTimer timer;
  long lowerTime = 0;
  long constFoldTime = 0;
  timer.Start();
  MIRLower mirLowerer(*theModule, nullptr);
  mirLowerer.Init();
  CGLowerer theLowerer(*theModule, *beCommon, cg.GenerateExceptionHandlingCode(), cg.GenerateVerboseCG());
  theLowerer.RegisterBuiltIns();
  theLowerer.InitArrayClassCacheTableIndex();
  theLowerer.RegisterExternalLibraryFunctions();
  theLowerer.SetCheckLoadStore(CGOptions::IsCheckArrayStore());
  timer.Stop();
  lowerTime += timer.ElapsedMicroseconds();

  if (cg.AddStackGuard()) {
    cg.AddStackGuardvar();
  }


  unsigned long rangeNum = 0;
  uint32 countFuncId = 0;
  for (auto it = theModule->GetFunctionList().begin(); it != theModule->GetFunctionList().end(); ++it) {
    MIRFunction *mirFunc = *it;
    if (mirFunc->GetBody() == nullptr) {
      continue;
    }

    // LowerIR.
    theModule->SetCurFunction(mirFunc);
    timer.Start();
    // if maple_me not run, needs extra lowering
    if (theModule->GetFlavor() <= kFeProduced) {
      mirLowerer.SetLowerCG();
      mirLowerer.LowerFunc(*mirFunc);
    }

    bool dumpAll = (CGOptions::GetDumpPhases().find("*") != CGOptions::GetDumpPhases().end());
    bool dumpFunc = CGOptions::FuncFilter(mirFunc->GetName());
    if (!cg.IsQuiet() || (dumpAll && dumpFunc)) {
      LogInfo::MapleLogger() << "************* before CGLowerer **************" << '\n';
      mirFunc->Dump();
    }

    theLowerer.LowerFunc(*mirFunc);

    if (!cg.IsQuiet() || (dumpAll && dumpFunc)) {
      LogInfo::MapleLogger() << "************* after  CGLowerer **************" << '\n';
      mirFunc->Dump();
      LogInfo::MapleLogger() << "************* end    CGLowerer **************" << '\n';
    }
    timer.Stop();
    lowerTime += timer.ElapsedMicroseconds();

    MIRSymbol *funcSt = GlobalTables::GetGsymTable().GetSymbolFromStidx(mirFunc->GetStIdx().Idx());
    MemPool *funcMp = memPoolCtrler.NewMemPool(funcSt->GetName());
    MapleAllocator funcScopeAllocator(funcMp);

    // Create CGFunc
    mirFunc->SetPuidxOrigin(++countFuncId);
    CGFunc *cgFunc = cg.CreateCGFunc(*theModule, *mirFunc, *beCommon, *funcMp, funcScopeAllocator, countFuncId);
    CHECK_FATAL(cgFunc != nullptr, "nullptr check");
    CG::SetCurCGFunc(*cgFunc);

    cgfpm.Run(*cgFunc);

    // Invalid all analysis result.
    cgfpm.Emit(*cgFunc);
    cg.GetEmitter()->EmitHugeSoRoutines();
    cgfpm.GetAnalysisResultManager()->InvalidIRbaseAnalysisResult(*cgFunc);
    cgfpm.ClearPhaseNameInfo();

    // Delete mempool.
    memPoolCtrler.DeleteMemPool(funcMp);
    memPoolCtrler.DeleteMemPool(mirFunc->GetCodeMempool());

    ++rangeNum;
  }
  cg.GetEmitter()->EmitHugeSoRoutines(true);
  extraPhasesTime.push_back(lowerTime);
  std::string lowerName = "lowerir";
  extraPhasesName.push_back(lowerName);
  extraPhasesTime.push_back(constFoldTime);
  std::string constFoldName = "constFold";
  extraPhasesName.push_back(constFoldName);
}

void DriverRunner::EmitGlobalInfo(CG &cg) const {
  EmitDuplicatedAsmFunc(cg);
  if (cgOptions->IsGenerateObjectMap()) {
    cg.GenerateObjectMaps(*beCommon);
  }
  cg.GetEmitter()->EmitGlobalVariable();
  cg.GetEmitter()->CloseOutput();
}

void DriverRunner::EmitDuplicatedAsmFunc(const CG &cg) const {
  if (cgOptions->IsDuplicateAsmFileEmpty()) {
    return;
  }

  struct stat buffer;
  if (stat(cgOptions->GetDuplicateAsmFile().c_str(), &buffer) != 0) {
    return;
  }

  std::ifstream duplicateAsmFileFD(cgOptions->GetDuplicateAsmFile());

  if (!duplicateAsmFileFD.is_open()) {
    duplicateAsmFileFD.close();
    ERR(kLncErr, " %s open failed!", cgOptions->GetDuplicateAsmFile().c_str());
    return;
  }
  std::string contend;
  bool onlyForFramework = false;
  bool isFramework = IsFramework();

  while (getline(duplicateAsmFileFD, contend)) {
    if (!contend.compare("#Libframework_start")) {
      onlyForFramework = true;
    }

    if (!contend.compare("#Libframework_end")) {
      onlyForFramework = false;
    }

    if (onlyForFramework && !isFramework) {
      continue;
    }

    cg.GetEmitter()->Emit(contend + "\n");
  }
  duplicateAsmFileFD.close();
}


void DriverRunner::ProcessExtraTime(const std::vector<long> &extraPhasesTime,
                                    const std::vector<std::string> &extraPhasesName, CgFuncPhaseManager &cgfpm) const {
  if (!CGOptions::IsEnableTimePhases()) {
    return;
  }

  for (size_t i = 0; i < extraPhasesTime.size(); ++i) {
    cgfpm.GetExtraPhasesTimer().insert({ extraPhasesName[i], extraPhasesTime[i] });
  }
}
}  // namespace maple
