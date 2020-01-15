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
#include "me_loop_canon.h"
#include <iostream>
#include <algorithm>
#include "me_cfg.h"
#include "me_option.h"
#include "dominance.h"

namespace maple {
// This phase changes the control flow graph to canonicalize loops so that the
// resulting loop form can provide places to insert loop-invariant code hoisted
// from the loop body.  This means converting loops to exhibit the
//      do {} while (condition)
// loop form.
//
// Step 1: Loops are identified by the existence of a BB that dominates one of
// its predecessors.  For each such backedge, the function NeedConvert() is
// called to check if it needs to be re-structured.  If so, the backEdges are
// collected.
//
// Step2: The collected backEdges are sorted so that their loop restructuring
// can be processed in just one pass.
//
// Step3: Perform the conversion for the loop represented by each collected
// backedge in sorted order.
// sort backEdges if bb is used as pred in one backedge and bb in another backedge
// deal with the bb used as pred first
static bool CompareBackedge(const std::pair<BB*, BB*> &a, const std::pair<BB*, BB*> &b) {
  // second is pred, first is bb
  if ((a.second)->GetBBId() == (b.first)->GetBBId()) {
    return true;
  }
  if ((a.first)->GetBBId() == (b.second)->GetBBId()) {
    return false;
  }
  return (a.first)->GetBBId() < (b.first)->GetBBId();
}

bool MeDoLoopCanon::NeedConvert(BB *bb, BB *pred, MapleAllocator &localAlloc, MapleMap<Key, bool> &swapSuccs) {
  CHECK_FATAL(bb != nullptr, "bb should not be null");
  CHECK_FATAL(pred != nullptr, "pred should not be null");
  bb->SetAttributes(kBBAttrIsInLoop);
  pred->SetAttributes(kBBAttrIsInLoop);
  // do not convert do-while loop
  if ((bb->GetKind() != kBBCondGoto) || (pred == bb) || bb->GetAttributes(kBBAttrIsTry) ||
      bb->GetAttributes(kBBAttrIsCatch)) {
    return false;
  }
  ASSERT(bb->GetSucc().size() == 2, "the number of bb's successors must equal 2");
  // if both succs are equal, return false
  if (bb->GetSucc().front() == bb->GetSucc().back()) {
    return false;
  }
  // check bb's succ both in loop body or not, such as
  //   1  <--
  //  / \   |
  //  2 |   |
  //  \ |   |
  //   3 ---|
  //  /
  MapleSet<BBId> inLoop(std::less<BBId>(), localAlloc.Adapter());
  MapleList<BB*> bodyList(localAlloc.Adapter());
  bodyList.push_back(pred);
  while (!bodyList.empty()) {
    BB *curr = bodyList.front();
    bodyList.pop_front();
    // skip bb and bb is already in loop body(has been dealt with)
    if (curr == bb || inLoop.count(curr->GetBBId()) == 1) {
      continue;
    }
    inLoop.insert(curr->GetBBId());
    for (BB *tmpPred : curr->GetPred()) {
      ASSERT(tmpPred != nullptr, "");
      bodyList.push_back(tmpPred);
      tmpPred->SetAttributes(kBBAttrIsInLoop);
    }
  }
  if ((inLoop.count(bb->GetSucc(0)->GetBBId()) == 1) && (inLoop.count(bb->GetSucc(1)->GetBBId()) == 1)) {
    return false;
  }
  // other case
  // fallthru is in loop body, latchBB need swap succs
  if (inLoop.count(bb->GetSucc().at(0)->GetBBId()) == 1) {
    swapSuccs.insert(make_pair(std::make_pair(bb, pred), true));
  }
  return true;
}

void MeDoLoopCanon::Convert(MeFunction *func, BB *bb, BB *pred, MapleMap<Key, bool> &swapSuccs) {
  ASSERT(bb != nullptr, "bb should not be null");
  ASSERT(pred != nullptr, "pred should not be null");
  // if bb->fallthru is in loopbody, latchBB need convert condgoto and make original target as its fallthru
  bool swapSuccOfLatch = (swapSuccs.find(std::make_pair(bb, pred)) != swapSuccs.end());
  if (DEBUGFUNC(func)) {
    LogInfo::MapleLogger() << "***loop convert: backedge bb->id_ " << bb->GetBBId() << " pred->id_ "
                           << pred->GetBBId();
    if (swapSuccOfLatch) {
      LogInfo::MapleLogger() << " need swap succs\n";
    } else {
      LogInfo::MapleLogger() << '\n';
    }
  }
  // new latchBB
  BB *latchBB = func->NewBasicBlock();
  latchBB->SetAttributes(kBBAttrArtificial);
  // update pred bb
  bb->RemoveBBFromPred(pred);
  pred->ReplaceSucc(bb, latchBB); // replace pred->succ with latchBB and set pred of latchBB
  // update pred stmt if needed
  if (pred->GetKind() == kBBGoto) {
    ASSERT(pred->GetAttributes(kBBAttrIsTry) || pred->GetSucc().size() == 1, "impossible");
    ASSERT(!pred->GetStmtNodes().empty(), "impossible");
    ASSERT(pred->GetStmtNodes().back().GetOpCode() == OP_goto, "impossible");
    // delete goto stmt and make pred is fallthru
    pred->RemoveLastStmt();
    pred->SetKind(kBBFallthru);
  } else if (pred->GetKind() == kBBCondGoto) {
    // if replaced bb is goto target
    ASSERT(pred->GetAttributes(kBBAttrIsTry) || pred->GetSucc().size() == 2,
           "pred should have attr kBBAttrIsTry or have 2 successors");
    ASSERT(!pred->GetStmtNodes().empty(), "pred's stmtNodeList should not be empty");
    auto &condGotoStmt = static_cast<CondGotoNode&>(pred->GetStmtNodes().back());
    if (latchBB == pred->GetSucc().at(1)) {
      // latchBB is the new target
      LabelIdx label = func->GetOrCreateBBLabel(*latchBB);
      condGotoStmt.SetOffset(label);
    }
  } else if (pred->GetKind() == kBBFallthru) {
    // donothing
  } else if (pred->GetKind() == kBBSwitch) {
    ASSERT(!pred->GetStmtNodes().empty(), "");
    auto &switchStmt = static_cast<SwitchNode&>(pred->GetStmtNodes().back());
    ASSERT(bb->GetBBLabel() != 0, "");
    LabelIdx oldlabIdx = bb->GetBBLabel();
    LabelIdx label = func->GetOrCreateBBLabel(*latchBB);
    if (switchStmt.GetDefaultLabel() == oldlabIdx) {
      switchStmt.SetDefaultLabel(label);
    }
    for (size_t i = 0; i < switchStmt.GetSwitchTable().size(); i++) {
      LabelIdx labelIdx = switchStmt.GetCasePair(i).second;
      if (labelIdx == oldlabIdx) {
        switchStmt.UpdateCaseLabelAt(i, label);
      }
    }
  } else {
    CHECK_FATAL(false, "unexpected pred kind");
  }
  // clone instructions of bb to latchBB
  func->CloneBasicBlock(*latchBB, *bb);
  // clone bb's succ to latchBB
  for (BB *succ : bb->GetSucc()) {
    ASSERT(!latchBB->GetAttributes(kBBAttrIsTry) || latchBB->GetSucc().empty(),
           "loopcanon TODO: tryblock should insert succ before handler");
    latchBB->AddSuccBB(succ);
  }
  latchBB->SetKind(bb->GetKind());
  // swap latchBB's succ if needed
  if (swapSuccOfLatch) {
    // modify condBr stmt
    ASSERT(latchBB->GetKind() == kBBCondGoto, "impossible");
    auto &condGotoStmt = static_cast<CondGotoNode&>(latchBB->GetStmtNodes().back());
    ASSERT(condGotoStmt.IsCondBr(), "impossible");
    condGotoStmt.SetOpCode((condGotoStmt.GetOpCode() == OP_brfalse) ? OP_brtrue : OP_brfalse);
    BB *fallthru = latchBB->GetSucc(0);
    LabelIdx label = func->GetOrCreateBBLabel(*fallthru);
    condGotoStmt.SetOffset(label);
    // swap succ
    BB *tmp = latchBB->GetSucc(0);
    latchBB->SetSucc(0, latchBB->GetSucc(1));
    latchBB->SetSucc(1, tmp);
  }
}

AnalysisResult *MeDoLoopCanon::Run(MeFunction *func, MeFuncResultMgr *m, ModuleResultMgr *mrm) {
  auto *dom = static_cast<Dominance*>(m->GetAnalysisResult(MeFuncPhase_DOMINANCE, func));
  ASSERT(dom != nullptr, "dom is null in MeDoLoopCanon::Run");
  // set MeCFG's has_do_while flag
  MeCFG *cfg = func->GetTheCfg();
  auto eIt = func->valid_end();
  for (auto bIt = func->valid_begin(); bIt != eIt; ++bIt) {
    if (bIt == func->common_entry() || bIt == func->common_exit()) {
      continue;
    }
    auto *bb = *bIt;
    if (bb->GetKind() != kBBCondGoto) {
      continue;
    }
    StmtNode *stmt = bb->GetStmtNodes().rbegin().base().d();
    if (stmt == nullptr) {
      continue;
    }
    CondGotoNode *condBr = safe_cast<CondGotoNode>(stmt);
    if (condBr == nullptr) {
      continue;
    }
    BB *brTargetbb = bb->GetSucc(1);
    if (dom->Dominate(*brTargetbb, *bb)) {
      cfg->SetHasDoWhile(true);
      break;
    }
  }
  MapleAllocator localAlloc(NewMemPool());
  MapleVector<std::pair<BB*, BB*>> backEdges(localAlloc.Adapter());
  using Key = std::pair<BB*, BB*>;
  MapleMap<Key, bool> swapSuccs(std::less<Key>(), localAlloc.Adapter());
  // collect backedge first: if bb dominator its pred, then the edge pred->bb is a backedge
  eIt = func->valid_end();
  for (auto bIt = func->valid_begin(); bIt != eIt; ++bIt) {
    if (bIt == func->common_entry() || bIt == func->common_exit()) {
      continue;
    }
    auto *bb = *bIt;
    MapleVector<BB*> &preds = bb->GetPred();
    for (BB *pred : preds) {
      ASSERT(func->GetCommonEntryBB(), "impossible");
      ASSERT(pred, "impossible");
      // bb is reachable from entry && bb dominator pred
      if (dom->Dominate(*func->GetCommonEntryBB(), *bb) && dom->Dominate(*bb, *pred) &&
          !pred->GetAttributes(kBBAttrWontExit) && (NeedConvert(bb, pred, localAlloc, swapSuccs))) {
        if (DEBUGFUNC(func)) {
          LogInfo::MapleLogger() << "find backedge " << bb->GetBBId() << " <-- " << pred->GetBBId() << '\n';
        }
        backEdges.push_back(std::make_pair(bb, pred));
      }
    }
  }
  // l with the edge which shared bb is used as pred
  // if backedge 4->3 is converted first, it will create a new backedge
  // <new latchBB-> BB1>, which needs iteration to deal with.
  //                 1  <---
  //               /  \    |
  //              6   2    |
  //                  /    |
  //          |---> 3 -----|
  //          |     |
  //          ------4
  //
  sort(backEdges.begin(), backEdges.end(), CompareBackedge);
  if (!backEdges.empty()) {
    if (DEBUGFUNC(func)) {
      LogInfo::MapleLogger() << "-----------------Dump mefunction before loop convert----------\n";
      func->Dump(true);
    }
    for (auto it = backEdges.begin(); it != backEdges.end(); it++) {
      Convert(func, (*it).first, (*it).second, swapSuccs);
      if (DEBUGFUNC(func)) {
        LogInfo::MapleLogger() << "-----------------Dump mefunction after loop convert-----------\n";
        func->Dump(true);
      }
    }
    m->InvalidAnalysisResult(MeFuncPhase_DOMINANCE, func);
  }
  return nullptr;
}
}  // namespace maple
