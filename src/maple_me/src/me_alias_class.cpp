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
#include "me_alias_class.h"
#include <cstdlib>
#include "me_option.h"
#include "mpl_logging.h"
#include "ssa_mir_nodes.h"
#include "ssa_tab.h"
#include "me_function.h"
#include "mpl_timer.h"

namespace maple {
// This phase performs alias analysis based on Steensgaard's algorithm and
// represent the resulting alias relationships in the Maple IR representation
bool MeAliasClass::HasWriteToStaticFinal() const {
  auto eIt = func.valid_end();
  for (auto bIt = func.valid_begin(); bIt != eIt; ++bIt) {
    for (auto &stmt : (*bIt)->GetStmtNodes()) {
      if (stmt.GetOpCode() == OP_dassign) {
        DassignNode &dass = static_cast<DassignNode&>(stmt);
        if (dass.GetStIdx().IsGlobal()) {
          MIRSymbol *sym = mirModule.CurFunction()->GetLocalOrGlobalSymbol(dass.GetStIdx());
          if (sym->IsFinal()) {
            return true;
          }
        }
      }
    }
  }
  return false;
}

void MeAliasClass::DoAliasAnalysis() {
  auto eIt = func.valid_end();
  for (auto bIt = func.valid_begin(); bIt != eIt; ++bIt) {
    for (auto &stmt : (*bIt)->GetStmtNodes()) {
      ApplyUnionForCopies(stmt);
    }
  }
  CreateAssignSets();
  if (DEBUGFUNC((&func))) {
    DumpAssignSets();
  }
  ReinitUnionFind();
  if (MeOption::noSteensgaard) {
    UnionAllPointedTos();
  } else {
    ApplyUnionForPointedTos();
    UnionForNotAllDefsSeen();
  }
  // TBAA
  if (!MeOption::noTBAA) {
    ReconstructAliasGroups();
  }
  CreateClassSets();
  if (DEBUGFUNC((&func))) {
    DumpClassSets();
  }
  // pass 2 through the program statements
  if (DEBUGFUNC((&func))) {
    LogInfo::MapleLogger() << "\n============ Alias Classification Pass 2 ============" << '\n';
  }

  eIt = func.valid_end();
  for (auto bIt = func.valid_begin(); bIt != eIt; ++bIt) {
    auto *bb = *bIt;
    for (auto &stmt : bb->GetStmtNodes()) {
      GenericInsertMayDefUse(stmt, bb->GetBBId());
    }
  }
}

AnalysisResult *MeDoAliasClass::Run(MeFunction *func, MeFuncResultMgr *m, ModuleResultMgr *mrm) {
  MPLTimer timer;
  timer.Start();
  (void)m->GetAnalysisResult(MeFuncPhase_SSATAB, func);
  MemPool *aliasClassMp = NewMemPool();
  KlassHierarchy *kh = static_cast<KlassHierarchy*>(mrm->GetAnalysisResult(MoPhase_CHA, &func->GetMIRModule()));
  MeAliasClass *aliasClass = aliasClassMp->New<MeAliasClass>(
      *aliasClassMp, func->GetMIRModule(), *func->GetMeSSATab(), *func, MeOption::lessThrowAlias,
      MeOption::finalFieldAlias, MeOption::ignoreIPA, DEBUGFUNC(func), MeOption::setCalleeHasSideEffect, kh);
  // pass 1 through the program statements
  if (DEBUGFUNC(func)) {
    LogInfo::MapleLogger() << "\n============ Alias Classification Pass 1 ============" << '\n';
  }
  aliasClass->DoAliasAnalysis();
  timer.Stop();
  if (DEBUGFUNC(func)) {
    LogInfo::MapleLogger() << "ssaTab + aliasClass passes consume cumulatively " << timer.Elapsed() << "seconds "
                           << '\n';
  }
  return aliasClass;
}
}  // namespace maple
