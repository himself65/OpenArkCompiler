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
#include "me_prop.h"
#include "me_option.h"
#include "me_dominance.h"

// This phase perform copy propagation optimization based on SSA representation.
// The propagation will not apply to ivar's of ref type unless the option
// --propiloadref is enabled.
//
// Copy propagation works by conducting a traversal over the program.  When it
// encounters a variable reference, it uses its SSA representation to look up
// its assigned value and try to do the substitution.

namespace maple {
AnalysisResult *MeDoMeProp::Run(MeFunction *func, MeFuncResultMgr *m, ModuleResultMgr*) {
  CHECK_NULL_FATAL(func);
  auto *dom = static_cast<Dominance*>(m->GetAnalysisResult(MeFuncPhase_DOMINANCE, func));
  CHECK_NULL_FATAL(dom);
  auto *hMap = static_cast<MeIRMap*>(m->GetAnalysisResult(MeFuncPhase_IRMAP, func));
  CHECK_NULL_FATAL(hMap);
  bool propIloadRef = MeOption::propIloadRef;
  MeProp meProp(*hMap, *dom, *NewMemPool(), Prop::PropConfig { MeOption::propBase, propIloadRef,
      MeOption::propGlobalRef, MeOption::propFinaliLoadRef, MeOption::propIloadRefNonParm, MeOption::propAtPhi });
  meProp.DoProp();
  if (DEBUGFUNC(func)) {
    LogInfo::MapleLogger() << "\n============== After Copy Propagation  =============" << '\n';
    func->Dump(false);
  }
  return nullptr;
}
}  // namespace maple
