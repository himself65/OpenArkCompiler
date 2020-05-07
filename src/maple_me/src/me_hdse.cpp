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
#include "me_hdse.h"
#include <iostream>
#include "ssa_mir_nodes.h"
#include "ver_symbol.h"
#include "dominance.h"
#include "me_irmap.h"
#include "me_ssa.h"
#include "hdse.h"

namespace maple {
void MeDoHDSE::MakeEmptyTrysUnreachable(MeFunction &func) {
  auto eIt = func.valid_end();
  for (auto bIt = func.valid_begin(); bIt != eIt; ++bIt) {
    BB *tryBB = *bIt;
    // get next valid bb
    auto endTryIt = bIt;
    if (++endTryIt == eIt) {
      break;
    }
    BB *endTry = *endTryIt;
    auto &meStmts = tryBB->GetMeStmts();
    if (tryBB->GetAttributes(kBBAttrIsTry) && !meStmts.empty() &&
        meStmts.front().GetOp() == OP_try && tryBB->GetMevarPhiList().empty() &&
        tryBB->GetMeRegPhiList().empty() && endTry->GetAttributes(kBBAttrIsTryEnd) && endTry->IsMeStmtEmpty()) {
      // we found a try BB followed by an empty endtry BB
      BB *targetBB = endTry->GetSucc(0);
      for (auto *tryPred : tryBB->GetPred()) {
        // update targetbb's predecessors
        if (!tryPred->IsPredBB(*targetBB)) {
          ASSERT(endTry->IsPredBB(*targetBB), "MakeEmptyTrysUnreachable: processing error");
          for (size_t k = 0; k < targetBB->GetPred().size(); ++k) {
            if (targetBB->GetPred(k) == endTry) {
              // push additional phi operand for each phi at targetbb
              auto phiIter = targetBB->GetMevarPhiList().begin();
              for (; phiIter != targetBB->GetMevarPhiList().end(); ++phiIter) {
                MeVarPhiNode *meVarPhi = phiIter->second;
                meVarPhi->GetOpnds().push_back(meVarPhi->GetOpnds()[k]);
              }
            }
          }
        }
        // replace tryBB in the pred's succ list by targetbb
        tryPred->ReplaceSucc(tryBB, targetBB);
        // if needed, update branch label
        MeStmt *br = to_ptr(tryPred->GetMeStmts().rbegin());
        if (br != nullptr) {
          if (br->IsCondBr()) {
            if (static_cast<CondGotoMeStmt*>(br)->GetOffset() == tryBB->GetBBLabel()) {
              LabelIdx label = func.GetOrCreateBBLabel(*targetBB);
              static_cast<CondGotoMeStmt*>(br)->SetOffset(label);
            }
          } else if (br->GetOp() == OP_goto) {
            LabelIdx label = func.GetOrCreateBBLabel(*targetBB);
            ASSERT(static_cast<GotoMeStmt*>(br)->GetOffset() == tryBB->GetBBLabel(), "Wrong label");
            static_cast<GotoMeStmt*>(br)->SetOffset(label);
          } else if (br->GetOp() == OP_multiway || br->GetOp() == OP_rangegoto) {
            CHECK_FATAL(false, "OP_multiway and OP_rangegoto are not supported");
          } else if (br->GetOp() == OP_switch) {
            LabelIdx label = func.GetOrCreateBBLabel(*targetBB);
            auto *switchNode = static_cast<SwitchMeStmt*>(br);
            if (switchNode->GetDefaultLabel() == tryBB->GetBBLabel()) {
              switchNode->SetDefaultLabel(label);
            }
            for (size_t m = 0; m < switchNode->GetSwitchTable().size(); m++) {
              if (switchNode->GetSwitchTable()[m].second == tryBB->GetBBLabel()) {
                switchNode->SetCaseLabel(m, label);
              }
            }
          }
        }
      }
    }
  }
}

void MeHDSE::RunHDSE() {
  hdseKeepRef = MeOption::dseKeepRef;
  DoHDSE();
}

AnalysisResult *MeDoHDSE::Run(MeFunction *func, MeFuncResultMgr *m, ModuleResultMgr*) {
  auto *postDom = static_cast<Dominance*>(m->GetAnalysisResult(MeFuncPhase_DOMINANCE, func));
  CHECK_NULL_FATAL(postDom);
  auto *hMap = static_cast<MeIRMap*>(m->GetAnalysisResult(MeFuncPhase_IRMAP, func));
  CHECK_NULL_FATAL(hMap);
  MeHDSE hdse(*func, *postDom, *hMap, DEBUGFUNC(func));
  hdse.RunHDSE();
  MakeEmptyTrysUnreachable(*func);
  func->GetTheCfg()->UnreachCodeAnalysis(true);
  m->InvalidAnalysisResult(MeFuncPhase_DOMINANCE, func);
  if (DEBUGFUNC(func)) {
    LogInfo::MapleLogger() << "\n============== HDSE =============" << '\n';
    func->Dump(false);
  }
  return nullptr;
}
}  // namespace maple
