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
#ifndef MAPLEBE_INCLUDE_CG_CG_OPTION_H
#define MAPLEBE_INCLUDE_CG_CG_OPTION_H
#include <vector>
#include "mempool.h"
#include "mempool_allocator.h"
#include "mir_module.h"
#include "types_def.h"
#include "driver_option_common.h"

namespace maplebe {
using namespace maple;
struct Range {
  bool enable;
  uint64 begin;
  uint64 end;
};

class CGOptions : public MapleDriverOptionBase {
 public:
  enum OptionEnum : uint64 {
    kUndefined = 0ULL,
    kDoCg = 1ULL << 0,
    kDoLinearScanRegAlloc = 1ULL << 1,
    kDoColorRegAlloc = 1ULL << 2,
    kConstFold = 1ULL << 3,
    kGenPic = 1ULL << 4,
    kGenPie = 1ULL << 5,
    kVerboseAsm = 1ULL << 6,
    kInsertCall = 1ULL << 7,
    kAddDebugTrace = 1ULL << 8,
    kGenYieldPoint = 1ULL << 9,
    kGenLocalRc = 1ULL << 10,
    kProEpilogueOpt = 1ULL << 11,
    kDebugFriendly = 1ULL << 20,
    kWithLoc = 1ULL << 21,
    kWithDwarf = 1ULL << 22,
    kWithMpl = 1ULL << 23,
    kWithSrc = 1ULL << 24,
    kWithAsm = 1ULL << 25,
    kWithProfileCode = 1ULL << 30,
    kUseStackGuard = 1ULL << 31,
    kSoeCheckInsert = 1ULL << 32,
    kAddFuncProfile = 1ULL << 33,
    /* undocumented */
    kDumpCFG = 1ULL << 61,
    kDumpCgir = 1ULL << 62,
    kSuppressFileInfo = 1ULL << 63,
  };

  using OptionFlag = uint64;

  enum GenerateEnum : uint64 {
    kCMacroDef = 1ULL << 0,
    kGctib = 1ULL << 1,
    kGrootList = 1ULL << 2,
    kPrimorList = 1ULL << 3,
  };

  using GenerateFlag = uint64;

  enum OptimizeLevel : uint8 {
    kLevel0 = 0,
    kLevel1 = 1,
    kLevel2 = 2,
  };

  enum ABIType : uint8 {
    kABIHard,
    kABISoft,
    kABISoftFP,
    kABIUndef
  };
  /*
   * The default CG option values are:
   * Don't BE_QUITE; verbose,
   * DO CG and generate .s file as output,
   * Generate EH,
   * Use frame pointer,
   * Generate CFI directives,
   * DO peephole optimization,
   * Generate position-independent executable,
   * Don't insert debug comments in .s file,
   * Don't insert a call to the named (instrumentation)
   * function at each function entry.
   */
  static const OptionFlag kDefaultOptions = OptionFlag(
#if TARGAARCH64 || TARGARM32
    kDoCg | kGenPie | kDoColorRegAlloc
#else
    kDoCg
#endif
  );

  /*
   * The default metadata generation flags values are:
   * Generate .macros.def for C preprocessors.
   * Generate .groots.txt for GC.
   * Generate .primordials.txt for GC.
   * Generate yieldpoints for GC.
   * Do not generate separate GCTIB file.
   */
  static const GenerateFlag kDefaultGflags = GenerateFlag(kGrootList | kPrimorList);

 public:
  static CGOptions &GetInstance();
  CGOptions();
  virtual ~CGOptions() = default;
  bool SolveOptions(const std::vector<mapleOption::Option> &opts, bool isDebug);
  void DecideMplcgRealLevel(const std::vector<mapleOption::Option> &inputOptions, bool isDebug);

  void DumpOptions();
  std::vector<std::string> &GetSequence() {
    return phaseSequence;
  }

  template <class T>
  void SetOrClear(T &dest, uint64 flag, bool truth) {
    if (truth) {
      dest |= flag;
    } else {
      dest &= ~flag;
    }
  }

  void ParseExclusiveFunc(const std::string &fileName);
  void ParseCyclePattern(const std::string &fileName);

  void EnableO0();
  void EnableO1();
  void EnableO2();

  bool GenDef() const {
    return generateFlag & kCMacroDef;
  }

  bool GenGctib() const {
    return generateFlag & kGctib;
  }

  bool GenGrootList() const {
    return generateFlag & kGrootList;
  }

  bool GenPrimorList() const {
    return generateFlag & kPrimorList;
  }

  bool GenYieldPoint() const {
    return generateFlag & kGenYieldPoint;
  }

  bool GenLocalRC() const {
    return (generateFlag & kGenLocalRc) && !gcOnly;
  }

  bool DoConstFold() const {
    return options & kConstFold;
  }

  bool DoEmitCode() const {
    return (options & kDoCg) != 0;
  }

  bool GenerateExceptionHandlingCode() const {
    return true;
  }

  bool DoLinearScanRegisterAllocation() const {
    return (options & kDoLinearScanRegAlloc) != 0;
  }
  bool DoColoringBasedRegisterAllocation() const {
    return (options & kDoColorRegAlloc) != 0;
  }

  bool GeneratePositionIndependentExecutable() const {
    return (options & kGenPie) != 0;
  }

  bool GenerateVerboseAsm() const {
    return (options & kVerboseAsm) != 0;
  }

  bool GenerateDebugFriendlyCode() const {
    return true;
  }

  bool DoPrologueEpilogue() const {
    return (options & kProEpilogueOpt) != 0;
  }

  bool AddStackGuard() const {
    return (options & kUseStackGuard) != 0;
  }

  bool WithLoc() const {
    return (options & kWithLoc) != 0;
  }

  bool WithDwarf() const {
    return (options & kWithDwarf) != 0;
  }

  bool WithSrc() const {
    return (options & kWithSrc) != 0;
  }

  bool WithMpl() const {
    return (options & kWithMpl) != 0;
  }

  bool WithAsm() const {
    return (options & kWithAsm) != 0;
  }

  bool NeedInsertInstrumentationFunction() const {
    return (options & kInsertCall) != 0;
  }

  bool InstrumentWithDebugTraceCall() const {
    return (options & kAddDebugTrace) != 0;
  }

  bool InstrumentWithProfile() const {
    return (options & kAddFuncProfile) != 0;
  }

  bool DoCheckSOE() const {
    return (options & kSoeCheckInsert) != 0;
  }

  bool SuppressFileInfo() const {
    return (options & kSuppressFileInfo) != 0;
  }

  bool DoDumpCFG() const {
    return (options & kDumpCFG) != 0;
  }

  void SetDefaultOptions(const MIRModule &mod);
  static bool DumpPhase(const std::string &phase);
  static bool FuncFilter(const std::string &name);
  void SplitPhases(const std::string &str, std::unordered_set<std::string> &set);

  int32 GetOptimizeLevel() const {
    return optimizeLevel;
  }

  bool IsRunCG() const {
    return runCGFlag;
  }

  void SetRunCGFlag(bool cgFlag) {
    runCGFlag = cgFlag;
  }

  bool IsInsertCall() const {
    return insertCall;
  }

  void SetInsertCall(bool insertFlag) {
    insertCall = insertFlag;
  }

  bool IsGenerateObjectMap() const {
    return generateObjectMap;
  }

  void SetGenerateObjectMap(bool flag) {
    generateObjectMap = flag;
  }

  void SetParserOption(uint32 option) {
    parserOption |= option;
  }

  uint32 GetParserOption() const {
    return parserOption;
  }

  GenerateFlag &GetGenerateFlags() {
    return generateFlag;
  }

  const GenerateFlag &GetGenerateFlags() const {
    return generateFlag;
  }

  void SetGenerateFlags(GenerateFlag flag) {
    generateFlag |= flag;
  }

  void SetOption(OptionFlag opFlag) {
    options |= opFlag;
  }

  void ClearOption(OptionFlag opFlag) {
    options &= ~opFlag;
  }

  const std::string &GetInstrumentationFunction() const {
    return instrumentationFunction;
  }

  void SetInstrumentationFunction(const std::string &function) {
    instrumentationFunction = function;
  }

  const std::string &GetClassListFile() const {
    return classListFile;
  }

  void SetClassListFile(const std::string &classList) {
    classListFile = classList;
  }

  void SetEHExclusiveFile(const std::string &ehExclusive) {
    ehExclusiveFile = ehExclusive;
  }

  void SetCyclePatternFile(const std::string &cyclePattern) {
    cyclePatternFile = cyclePattern;
  }

  static bool IsQuiet() {
    return quiet;
  }

  static void SetQuiet(bool flag) {
    quiet = flag;
  }

  static std::unordered_set<std::string> &GetDumpPhases() {
    return dumpPhases;
  }

  static std::unordered_set<std::string> &GetSkipPhases() {
    return skipPhases;
  }

  static bool IsSkipPhase(const std::string &phaseName) {
    return !(skipPhases.find(phaseName) == skipPhases.end());
  }

  const std::vector<std::string> &GetEHExclusiveFunctionNameVec() const {
    return ehExclusiveFunctionName;
  }

  static const std::unordered_map<std::string, std::vector<std::string>> &GetCyclePatternMap() {
    return cyclePatternMap;
  }

  static bool IsSkipFromPhase(const std::string &phaseName) {
    return skipFrom.compare(phaseName) == 0;
  }

  static void SetSkipFrom(const std::string &phaseName) {
    skipFrom = phaseName;
  }

  static bool IsSkipAfterPhase(const std::string &phaseName) {
    return skipAfter.compare(phaseName) == 0;
  }

  static void SetSkipAfter(const std::string &phaseName) {
    skipAfter = phaseName;
  }

  static const std::string &GetDumpFunc() {
    return dumpFunc;
  }

  static bool IsDumpFunc(const std::string &func) {
    return ((dumpFunc.compare("*") == 0) || (func.find(CGOptions::dumpFunc.c_str()) != std::string::npos));
  }

  static void SetDumpFunc(const std::string &func) {
    dumpFunc = func;
  }

  static const std::string &GetDuplicateAsmFile() {
    return duplicateAsmFile;
  }

  static bool IsDuplicateAsmFileEmpty() {
    return duplicateAsmFile.empty();
  }

  static void SetDuplicateAsmFile(const std::string &fileName) {
    duplicateAsmFile = fileName;
  }

  static void EnableBarriersForVolatile() {
    useBarriersForVolatile = true;
  }

  static void DisableBarriersForVolatile() {
    useBarriersForVolatile = false;
  }

  static bool UseBarriersForVolatile() {
    return useBarriersForVolatile;
  }

  static void EnableDumpBefore() {
    dumpBefore = true;
  }

  static void DisableDumpBefore() {
    dumpBefore = false;
  }

  static bool IsDumpBefore() {
    return dumpBefore;
  }

  static bool IsDumpAfter() {
    return dumpAfter;
  }

  static void EnableDumpAfter() {
    dumpAfter = true;
  }

  static void DisableDumpAfter() {
    dumpAfter = false;
  }

  static bool IsEnableTimePhases() {
    return timePhases;
  }

  static void EnableTimePhases() {
    timePhases = true;
  }

  static void DisableTimePhases() {
    timePhases = false;
  }

  static void EnableCheckArrayStore() {
    checkArrayStore = true;
  }

  static void DisableCheckArrayStore() {
    checkArrayStore = false;
  }

  static bool IsCheckArrayStore() {
    return checkArrayStore;
  }

  static void EnableExclusiveEH() {
    exclusiveEH = true;
  }

  static bool IsExclusiveEH() {
    return exclusiveEH;
  }

  static void EnablePIC() {
    doPIC = true;
  }

  static void DisablePIC() {
    doPIC = false;
  }

  static bool IsPIC() {
    return doPIC;
  }

  static void EnableNoCalleeCFI() {
    noCalleeCFI = true;
  }

  static void DisableNoCalleeCFI() {
    noCalleeCFI = false;
  }

  static bool IsNoCalleeCFI() {
    return noCalleeCFI;
  }

  static void EnableEmitCyclePattern() {
    emitCyclePattern = true;
  }

  static bool IsInsertYieldPoint() {
    return insertYieldPoint;
  }

  static void EnableMapleLinker() {
    mapleLinker = true;
  }

  static void DisableMapleLinker() {
    mapleLinker = false;
  }

  static bool IsMapleLinker() {
    return mapleLinker;
  }

  static void EnablePrintFunction() {
    printFunction = true;
  }

  static void DisablePrintFunction() {
    printFunction = false;
  }

  static bool IsPrintFunction() {
    return printFunction;
  }

  static std::string &GetGlobalVarProFile() {
    return globalVarProfile;
  }

  static bool IsGlobalVarProFileEmpty() {
    return globalVarProfile.empty();
  }

  static void EnableNativeOpt() {
    nativeOpt = true;
  }

  static void DisableNativeOpt() {
    nativeOpt = false;
  }

  static bool IsNativeOpt() {
    return nativeOpt;
  }

  static void EnableWithDwarf() {
    withDwarf = true;
  }

  static bool IsWithDwarf() {
    return withDwarf;
  }

  static void EnableLazyBinding() {
    lazyBinding = true;
  }

  static void DisableLazyBinding() {
    lazyBinding = false;
  }

  static bool IsLazyBinding() {
    return lazyBinding;
  }

  static void EnableHotFix() {
    hotFix = true;
  }

  static void DisableHotFix() {
    hotFix = false;
  }

  static bool IsHotFix() {
    return hotFix;
  }

  static void EnableLongCalls() {
    genLongCalls = true;
  }

  static void DisableLongCalls() {
    genLongCalls = false;
  }

  static bool IsLongCalls() {
    return genLongCalls;
  }

  static void EnableGCOnly() {
    gcOnly = true;
  }

  static void DisableGCOnly() {
    gcOnly = false;
  }

  static bool IsGCOnly() {
    return gcOnly;
  }

 private:
  std::vector<std::string> phaseSequence;

  bool insertCall = false;
  bool runCGFlag = true;
  bool generateObjectMap = true;
  uint32 parserOption = 0;
  int32 optimizeLevel = 0;

  GenerateFlag generateFlag = 0;
  OptionFlag options = kUndefined;
  std::string instrumentationFunction;

  std::string classListFile;
  std::string ehExclusiveFile;
  std::string cyclePatternFile;
  /* we don't do exception handling in this list */
  std::vector<std::string> ehExclusiveFunctionName;

  static bool quiet;
  static std::unordered_set<std::string> dumpPhases;
  static std::unordered_set<std::string> skipPhases;
  static std::unordered_map<std::string, std::vector<std::string>> cyclePatternMap;
  static std::string skipFrom;
  static std::string skipAfter;
  static std::string dumpFunc;
  static std::string duplicateAsmFile;
  static bool useBarriersForVolatile;
  static bool dumpBefore;
  static bool dumpAfter;
  static bool timePhases;
  static bool checkArrayStore;

  static bool exclusiveEH;
  static bool doPIC;
  static bool noCalleeCFI;
  static bool emitCyclePattern;
  static bool insertYieldPoint;
  static bool mapleLinker;
  static bool printFunction;

  static std::string globalVarProfile;
  static bool nativeOpt;
  static bool withDwarf;
  static bool lazyBinding;
  static bool hotFix;
  /* if true generate adrp/ldr/blr */
  static bool genLongCalls;
  static bool gcOnly;
};
}  /* namespace maplebe */

#define SET_FIND(SET, NAME) ((SET).find(NAME))
#define SET_END(SET) ((SET).end())
#define IS_STR_IN_SET(SET, NAME) (SET_FIND(SET, NAME) != SET_END(SET))

#define CG_DEBUG_FUNC(f)                                                 \
    ((maplebe::CGOptions::GetDumpPhases().find(PhaseName()) != maplebe::CGOptions::GetDumpPhases().end()) && \
     maplebe::CGOptions::IsDumpFunc((f)->GetName()))

#ifndef TRACE_PHASE
#define TRACE_PHASE (IS_STR_IN_SET(maplebe::CGOptions::GetDumpPhases(), PhaseName()))
#endif

#endif  /* MAPLEBE_INCLUDE_CG_CG_OPTION_H */
