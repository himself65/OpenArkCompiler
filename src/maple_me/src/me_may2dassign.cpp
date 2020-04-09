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
#include "me_may2dassign.h"

// this phase converts all maydassign back to dassign
namespace maple {
void May2Dassign::DoIt() {
  auto eIt = func.valid_end();
  for (auto bIt = func.valid_begin(); bIt != eIt; ++bIt) {
    auto *bb = *bIt;
    for (auto &stmt : bb->GetMeStmts()) {
      if (stmt.GetOp() != OP_maydassign) {
        continue;
      }
      auto &mass = static_cast<MaydassignMeStmt&>(stmt);
      // chiList for Maydassign has only 1 element
      CHECK_FATAL(!mass.GetChiList()->empty(), "chiList is empty in DoIt");
      VarMeExpr *theLhs = mass.GetChiList()->begin()->second->GetLHS();
      ASSERT(mass.GetMayDassignSym() == ssaTab->GetOriginalStFromID(theLhs->GetOStIdx()),
             "MeDoMay2Dassign: cannot find maydassign lhs");
      auto *dass = static_cast<DassignMeStmt*>(irMap->CreateDassignMeStmt(*theLhs, *mass.GetRHS(), *bb));
      dass->SetNeedDecref(mass.NeedDecref());
      dass->SetNeedIncref(mass.NeedIncref());
      dass->SetWasMayDassign(true);
      dass->SetChiList(*mass.GetChiList());
      dass->GetChiList()->clear();
      bb->ReplaceMeStmt(&mass, dass);
    }
  }
}

AnalysisResult *MeDoMay2Dassign::Run(MeFunction *func, MeFuncResultMgr*, ModuleResultMgr*) {
  May2Dassign may2Dassign(*func);
  may2Dassign.DoIt();
  return nullptr;
}
}  // namespace maple
