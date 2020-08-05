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
#include "me_critical_edge.h"
#include <iostream>
#include "me_cfg.h"
#include "me_option.h"
#include "dominance.h"
#include "me_function.h"

// This phase finds critical edges and split them into two, because their
// presence would restrict the optimizations performed by SSAPRE-based phases.
// An edge is a critical edge when its pred has more than one succ and its succ
// has more than one pred
//     pred
//    /    \
//  newbb   \   <-- newbb (newbb is an empty bb but may carry a label)
//  \ /      \
//  succ
//
// newbb is always appended at the end of bb_vec_ and pred/succ will be updated.
// The bblayout phase will determine the final layout order of the bbs.
namespace maple {
void MeDoSplitCEdge::UpdateNewBBInTry(BB &newBB, const BB &pred) const {
  newBB.SetAttributes(kBBAttrIsTry);
  for (auto *candCatch : pred.GetSucc()) {
    if (candCatch != nullptr && candCatch->GetAttributes(kBBAttrIsCatch)) {
      newBB.AddSucc(*candCatch);
    }
  }
}

void MeDoSplitCEdge::UpdateGotoLabel(BB &newBB, MeFunction &func, BB &pred, BB &succ) const {
  auto &gotoStmt = static_cast<CondGotoNode&>(pred.GetStmtNodes().back());
  BB *gotoBB = pred.GetSucc().at(1);
  LabelIdx oldLabelIdx = gotoStmt.GetOffset();
  if (oldLabelIdx != gotoBB->GetBBLabel()) {
    // original gotoBB is replaced by newBB
    LabelIdx label = func.GetOrCreateBBLabel(*gotoBB);
    gotoStmt.SetOffset(label);
  }
  if (DEBUGFUNC(&func)) {
    LogInfo::MapleLogger() << "******after break: dump updated condgoto_BB *****\n";
    pred.Dump(&func.GetMIRModule());
    newBB.Dump(&func.GetMIRModule());
    succ.Dump(&func.GetMIRModule());
  }
}

void MeDoSplitCEdge::UpdateCaseLabel(BB &newBB, MeFunction &func, BB &pred, BB &succ) const {
  auto &switchStmt = static_cast<SwitchNode&>(pred.GetStmtNodes().back());
  LabelIdx oldLabelIdx = succ.GetBBLabel();
  LabelIdx label = func.GetOrCreateBBLabel(newBB);
  if (switchStmt.GetDefaultLabel() == oldLabelIdx) {
    switchStmt.SetDefaultLabel(label);
  }
  for (size_t i = 0; i < switchStmt.GetSwitchTable().size(); ++i) {
    LabelIdx labelIdx = switchStmt.GetCasePair(i).second;
    if (labelIdx == oldLabelIdx) {
      switchStmt.UpdateCaseLabelAt(i, label);
    }
  }
  if (DEBUGFUNC(&func)) {
    LogInfo::MapleLogger() << "******after break: dump updated switchBB *****\n";
    pred.Dump(&func.GetMIRModule());
    newBB.Dump(&func.GetMIRModule());
    succ.Dump(&func.GetMIRModule());
  }
}

void MeDoSplitCEdge::BreakCriticalEdge(MeFunction &func, BB &pred, BB &succ) const {
  if (DEBUGFUNC(&func)) {
    LogInfo::MapleLogger() << "******before break : critical edge : BB" << pred.GetBBId() << " -> BB" <<
        succ.GetBBId() << "\n";
    pred.Dump(&func.GetMIRModule());
    succ.Dump(&func.GetMIRModule());
  }
  ASSERT(!succ.GetAttributes(kBBAttrIsCatch), "BreakCriticalEdge: cannot break an EH edge");
  // create newBB and set pred/succ
  BB *newBB = nullptr;
  // use replace instead of remove/add to keep position in pred/succ
  size_t index = succ.GetPred().size();
  if (&pred == func.GetCommonEntryBB()) {
    newBB = &func.InsertNewBasicBlock(*func.GetFirstBB());
    newBB->SetAttributes(kBBAttrIsEntry);
    succ.ClearAttributes(kBBAttrIsEntry);
    pred.RemoveEntry(succ);
    pred.AddEntry(*newBB);
  } else {
    newBB = func.NewBasicBlock();
    while (index > 0) {
      if (succ.GetPred(index - 1) == &pred) {
        break;
      }
      index--;
    }
    pred.ReplaceSucc(&succ, newBB);
  }
  // pred has been remove for pred vector of succ
  // means size reduced, so index reduced
  index--;
  succ.AddPred(*newBB, index);
  newBB->SetKind(kBBFallthru);  // default kind
  newBB->SetAttributes(kBBAttrArtificial);

  if (pred.GetAttributes(kBBAttrIsTry)) {
    UpdateNewBBInTry(*newBB, pred);
  }
  // update statement offset if succ is goto target
  if (pred.GetKind() == kBBCondGoto) {
    UpdateGotoLabel(*newBB, func, pred, succ);
  } else if (pred.GetKind() == kBBSwitch) {
    UpdateCaseLabel(*newBB, func, pred, succ);
  }
}

AnalysisResult *MeDoSplitCEdge::Run(MeFunction *func, MeFuncResultMgr *m, ModuleResultMgr*) {
  std::vector<std::pair<BB*, BB*>> criticalEdge;
  auto eIt = func->valid_end();
  for (auto bIt = func->valid_begin(); bIt != eIt; ++bIt) {
    if (bIt == func->common_exit()) {
      continue;
    }
    auto *bb = *bIt;
    const MapleVector<BB*> &preds = bb->GetPred();
    // skip fallthrough bb or bb is handler block
    if (preds.size() < 2 || bb->GetAttributes(kBBAttrIsCatch)) {
      continue;
    }
    // current BB is a merge
    for (BB *pred : preds) {
      if (pred->GetKind() == kBBGoto) {
        continue;
      }
      if (pred->GetSucc().size() > 1) {
        // pred has more than one succ
        criticalEdge.push_back(std::make_pair(pred, bb));
      }
    }
  }
  // separate treatment for commonEntryBB's succ BBs
  for (BB *entryBB : func->GetCommonEntryBB()->GetSucc()) {
    if (!entryBB->GetPred().empty()) {
      criticalEdge.push_back(std::make_pair(func->GetCommonEntryBB(), entryBB));
    }
  }
  if (!criticalEdge.empty()) {
    if (DEBUGFUNC(func)) {
      LogInfo::MapleLogger() << "*******************before break dump function*****************\n";
      func->DumpFunctionNoSSA();
      func->GetTheCfg()->DumpToFile("cfgbeforebreak");
    }
    for (auto it = criticalEdge.begin(); it != criticalEdge.end(); ++it) {
      BreakCriticalEdge(*func, *((*it).first), *((*it).second));
    }
    if (DEBUGFUNC(func)) {
      LogInfo::MapleLogger() << "******************after break dump function******************\n";
      func->Dump(true);
      func->GetTheCfg()->DumpToFile("cfgafterbreak");
    }
    m->InvalidAnalysisResult(MeFuncPhase_DOMINANCE, func);
  }
  return nullptr;
}
}  // namespace maple
