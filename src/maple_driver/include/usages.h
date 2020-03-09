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
#ifndef MAPLE_DRIVER_INCLUDE_USAGES_H
#define MAPLE_DRIVER_INCLUDE_USAGES_H

namespace maple {
enum OptionIndex : uint64 {
  kUnknown,
  kHelp,
  kVersion,
  kInFile,
  kInMplt,
  kOptimization0,
  kMeOpt,
  kMpl2MplOpt,
  kSaveTemps,
  kRun,
  kOption,
  kAllDebug,
  // ----------jbc2mpl begin-------
  kUseStringFactory,
  kJbc2mplOutMpl,
  kTimePhases,
  kGenMeMpl,
  kGenVtableImpl,
  kVerify,
  // ----------me begin-----------
  kMeHelp,
  kMeDumpPhases,
  kMeSkipPhases,
  kMeDumpFunc,
  kMeQuiet,
  kMeNoDot,
  kMeSkipFrom,
  kMeSkipAfter,
  kSetCalleeHasSideEffect,
  kNoSteensgaard,
  kNoTBAA,
  kAliasAnalysisLevel,
  kStmtNum,
  kRcLower,
  kNoRcLower,
  kMeDumpBefore,
  kMeDumpAfter,
  kMeOptL1,
  kMeOptL2,
  kMeRange,
  kEpreLimit,
  kEprepuLimit,
  kStmtPrepuLimit,
  kLpreLimit,
  kLprepulLimit,
  kPregreNameLimit,
  kDelrcpuLimit,
  kEpreIncludeRef,
  kNoEpreIncludeRef,
  kEpreLocalRefVar,
  kNoEpreLocalRefVar,
  kEprelhSivar,
  kLessThrowAlias,
  kNodeLegateRc,
  kNocondBasedRc,
  kNullcheckPre,
  kClinitPre,
  kDassignPre,
  kAssign2finalPre,
  kRegReadAtReturn,
  kLpreSpeculate,
  kNoLpreSpeculate,
  kSpillatCatch,
  // ----------mpl2mpl begin---------
  kMpl2MplHelp,
  kMpl2MplDumpPhase,
  kMpl2MplSkipPhase,
  kMpl2MplSkipFrom,
  kMpl2MplSkipAfter,
  kMpl2MplDumpFunc,
  kMpl2MplQuiet,
  kMpl2MplStubJniFunc,
  kMpl2MplSkipVirtual,
  kRegNativeDynamicOnly,
  kRegNativeStaticBindingList,
  kNativeWrapper,
  kMpl2MplDumpBefore,
  kMpl2MplDumpAfter,
  kMpl2MplMapleLinker,
  kMplnkDumpMuid,
  kEmitVtableImpl,
  kInlineWithProfile,
  kInlineWithoutProfile,
  kMpl2MplUseInline,
  kMpl2MplNoInlineFuncList,
  kMpl2MplUseCrossModuleInline,
  kInlineSmallFunctionThreshold,
  kInlineHotFunctionThreshold,
  kInlineModuleGrowth,
  kInlineColdFunctionThreshold,
  kProfileHotCount,
  kProfileColdCount,
  kProfileHotRate,
  kProfileColdRate,
  kMpl2MplNativeOpt,
  kMpl2MplOptL1,
  kMpl2MplOptL2,
  kMpl2MplNoDot,
  // ----------mplcg begin---------
  kCGQuiet,
  kPie,
  kPic,
  kVerbose,
  kCGMapleLinker,
  kDuplicateToDelPlt,
// ----------mplcg end-----------
};

enum EnabledIndex {
  kDisable,
  kEnable
};
}  // namespace maple
#endif  // MAPLE_DRIVER_INCLUDE_USAGES_H
