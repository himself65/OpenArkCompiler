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
#include "me_ssa_devirtual.h"
#include "me_function.h"
#include "me_option.h"

namespace maple {
AnalysisResult *MeDoSSADevirtual::Run(MeFunction *func, MeFuncResultMgr *frm, ModuleResultMgr *mrm) {
  auto *dom = static_cast<Dominance*>(frm->GetAnalysisResult(MeFuncPhase_DOMINANCE, func));
  ASSERT(dom != nullptr, "dominance phase has problem");
  auto *irMap = static_cast<MeIRMap*>(frm->GetAnalysisResult(MeFuncPhase_IRMAP, func));
  ASSERT(irMap != nullptr, "hssaMap has problem");
  CHECK_FATAL(mrm != nullptr, "Needs module result manager for ipa");
  auto *kh = static_cast<KlassHierarchy*>(mrm->GetAnalysisResult(MoPhase_CHA, &func->GetMIRModule()));
  ASSERT(kh != nullptr, "KlassHierarchy has problem");
  bool skipReturnTypeOpt = false;
  MeSSADevirtual meSSADevirtual(*NewMemPool(), func->GetMIRModule(), *func, *irMap, *kh, *dom, skipReturnTypeOpt);
  if (Options::O2) {
    Clone *clone = static_cast<Clone*>(mrm->GetAnalysisResult(MoPhase_CLONE, &func->GetMIRModule()));
    if (clone != nullptr) {
      meSSADevirtual.SetClone(*clone);
    }
  }

  if (DEBUGFUNC(func)) {
    SSADevirtual::debug = true;
  }
  meSSADevirtual.Perform(*func->GetCommonEntryBB());
  if (DEBUGFUNC(func)) {
    SSADevirtual::debug = false;
    LogInfo::MapleLogger() << "\n============== After SSA Devirtualization  =============" << "\n";
    func->Dump(false);
  }
  return nullptr;
}
}  // namespace maple
