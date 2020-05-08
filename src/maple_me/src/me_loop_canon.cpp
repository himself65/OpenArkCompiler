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

bool MeDoLoopCanon::NeedConvert(BB &bb, BB &pred, MapleAllocator &localAlloc, MapleMap<Key, bool> &swapSuccs) const {
  bb.SetAttributes(kBBAttrIsInLoop);
  pred.SetAttributes(kBBAttrIsInLoop);
  // do not convert do-while loop
  if ((bb.GetKind() != kBBCondGoto) || (&pred == &bb) || bb.GetAttributes(kBBAttrIsTry) ||
      bb.GetAttributes(kBBAttrIsCatch)) {
    return false;
  }
  ASSERT(bb.GetSucc().size() == 2, "the number of bb's successors must equal 2");
  // if both succs are equal, return false
  if (bb.GetSucc().front() == bb.GetSucc().back()) {
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
  bodyList.push_back(&pred);
  while (!bodyList.empty()) {
    BB *curr = bodyList.front();
    bodyList.pop_front();
    // skip bb and bb is already in loop body(has been dealt with)
    if (curr == &bb || inLoop.count(curr->GetBBId()) == 1) {
      continue;
    }
    inLoop.insert(curr->GetBBId());
    for (BB *tmpPred : curr->GetPred()) {
      ASSERT_NOT_NULL(tmpPred);
      bodyList.push_back(tmpPred);
      tmpPred->SetAttributes(kBBAttrIsInLoop);
    }
  }
  if ((inLoop.count(bb.GetSucc(0)->GetBBId()) == 1) && (inLoop.count(bb.GetSucc(1)->GetBBId()) == 1)) {
    return false;
  }
  // other case
  // fallthru is in loop body, latchBB need swap succs
  if (inLoop.count(bb.GetSucc().at(0)->GetBBId()) == 1) {
    swapSuccs.insert(make_pair(std::make_pair(&bb, &pred), true));
  }
  return true;
}

void MeDoLoopCanon::Convert(MeFunction &func, BB &bb, BB &pred, MapleMap<Key, bool> &swapSuccs) {
  // if bb->fallthru is in loopbody, latchBB need convert condgoto and make original target as its fallthru
  bool swapSuccOfLatch = (swapSuccs.find(std::make_pair(&bb, &pred)) != swapSuccs.end());
  if (DEBUGFUNC(&func)) {
    LogInfo::MapleLogger() << "***loop convert: backedge bb->id_ " << bb.GetBBId() << " pred->id_ "
                           << pred.GetBBId();
    if (swapSuccOfLatch) {
      LogInfo::MapleLogger() << " need swap succs\n";
    } else {
      LogInfo::MapleLogger() << '\n';
    }
  }
  // new latchBB
  BB *latchBB = func.NewBasicBlock();
  latchBB->SetAttributes(kBBAttrArtificial);
  // update pred bb
  pred.ReplaceSucc(&bb, latchBB); // replace pred.succ with latchBB and set pred of latchBB
  // update pred stmt if needed
  if (pred.GetKind() == kBBGoto) {
    ASSERT(pred.GetAttributes(kBBAttrIsTry) || pred.GetSucc().size() == 1, "impossible");
    ASSERT(!pred.GetStmtNodes().empty(), "impossible");
    ASSERT(pred.GetStmtNodes().back().GetOpCode() == OP_goto, "impossible");
    // delete goto stmt and make pred is fallthru
    pred.RemoveLastStmt();
    pred.SetKind(kBBFallthru);
  } else if (pred.GetKind() == kBBCondGoto) {
    // if replaced bb is goto target
    ASSERT(pred.GetAttributes(kBBAttrIsTry) || pred.GetSucc().size() == 2,
           "pred should have attr kBBAttrIsTry or have 2 successors");
    ASSERT(!pred.GetStmtNodes().empty(), "pred's stmtNodeList should not be empty");
    auto &condGotoStmt = static_cast<CondGotoNode&>(pred.GetStmtNodes().back());
    if (latchBB == pred.GetSucc().at(1)) {
      // latchBB is the new target
      LabelIdx label = func.GetOrCreateBBLabel(*latchBB);
      condGotoStmt.SetOffset(label);
    }
  } else if (pred.GetKind() == kBBFallthru) {
    // do nothing
  } else if (pred.GetKind() == kBBSwitch) {
    ASSERT(!pred.GetStmtNodes().empty(), "bb is empty");
    auto &switchStmt = static_cast<SwitchNode&>(pred.GetStmtNodes().back());
    ASSERT(bb.GetBBLabel() != 0, "wrong bb label");
    LabelIdx oldLabIdx = bb.GetBBLabel();
    LabelIdx label = func.GetOrCreateBBLabel(*latchBB);
    if (switchStmt.GetDefaultLabel() == oldLabIdx) {
      switchStmt.SetDefaultLabel(label);
    }
    for (size_t i = 0; i < switchStmt.GetSwitchTable().size(); i++) {
      LabelIdx labelIdx = switchStmt.GetCasePair(i).second;
      if (labelIdx == oldLabIdx) {
        switchStmt.UpdateCaseLabelAt(i, label);
      }
    }
  } else {
    CHECK_FATAL(false, "unexpected pred kind");
  }
  // clone instructions of bb to latchBB
  func.CloneBasicBlock(*latchBB, bb);
  // clone bb's succ to latchBB
  for (BB *succ : bb.GetSucc()) {
    ASSERT(!latchBB->GetAttributes(kBBAttrIsTry) || latchBB->GetSucc().empty(),
           "loopcanon : tryblock should insert succ before handler");
    latchBB->AddSucc(*succ);
  }
  latchBB->SetKind(bb.GetKind());
  // swap latchBB's succ if needed
  if (swapSuccOfLatch) {
    // modify condBr stmt
    ASSERT(latchBB->GetKind() == kBBCondGoto, "impossible");
    auto &condGotoStmt = static_cast<CondGotoNode&>(latchBB->GetStmtNodes().back());
    ASSERT(condGotoStmt.IsCondBr(), "impossible");
    condGotoStmt.SetOpCode((condGotoStmt.GetOpCode() == OP_brfalse) ? OP_brtrue : OP_brfalse);
    BB *fallthru = latchBB->GetSucc(0);
    LabelIdx label = func.GetOrCreateBBLabel(*fallthru);
    condGotoStmt.SetOffset(label);
    // swap succ
    BB *tmp = latchBB->GetSucc(0);
    latchBB->SetSucc(0, latchBB->GetSucc(1));
    latchBB->SetSucc(1, tmp);
  }
}

void MeDoLoopCanon::ExecuteLoopCanon(MeFunction &func, MeFuncResultMgr &m, Dominance &dom) {
  // set MeCFG's has_do_while flag
  MeCFG *cfg = func.GetTheCfg();
  auto eIt = func.valid_end();
  for (auto bIt = func.valid_begin(); bIt != eIt; ++bIt) {
    if (bIt == func.common_entry() || bIt == func.common_exit()) {
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
    if (dom.Dominate(*brTargetbb, *bb)) {
      cfg->SetHasDoWhile(true);
      break;
    }
  }
  MapleAllocator localAlloc(NewMemPool());
  MapleVector<std::pair<BB*, BB*>> backEdges(localAlloc.Adapter());
  MapleMap<Key, bool> swapSuccs(std::less<Key>(), localAlloc.Adapter());
  // collect backedge first: if bb dominator its pred, then the edge pred->bb is a backedge
  eIt = func.valid_end();
  for (auto bIt = func.valid_begin(); bIt != eIt; ++bIt) {
    if (bIt == func.common_entry() || bIt == func.common_exit()) {
      continue;
    }
    auto *bb = *bIt;
    const MapleVector<BB*> &preds = bb->GetPred();
    for (BB *pred : preds) {
      ASSERT(func.GetCommonEntryBB() != nullptr, "impossible");
      ASSERT_NOT_NULL(pred);
      // bb is reachable from entry && bb dominator pred
      if (dom.Dominate(*func.GetCommonEntryBB(), *bb) && dom.Dominate(*bb, *pred) &&
          !pred->GetAttributes(kBBAttrWontExit) && (NeedConvert(*bb, *pred, localAlloc, swapSuccs))) {
        if (DEBUGFUNC(&func)) {
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
    if (DEBUGFUNC(&func)) {
      LogInfo::MapleLogger() << "-----------------Dump mefunction before loop convert----------\n";
      func.Dump(true);
    }
    for (auto it = backEdges.begin(); it != backEdges.end(); ++it) {
      BB *bb = it->first;
      BB *pred = it->second;
      ASSERT(bb != nullptr, "bb should not be nullptr");
      ASSERT(pred != nullptr, "pred should not be nullptr");
      Convert(func, *bb, *pred, swapSuccs);
      if (DEBUGFUNC(&func)) {
        LogInfo::MapleLogger() << "-----------------Dump mefunction after loop convert-----------\n";
        func.Dump(true);
      }
    }
    m.InvalidAnalysisResult(MeFuncPhase_DOMINANCE, &func);
  }
}

// Only when backege or preheader is not try bb, update head map.
void MeDoLoopCanon::FindHeadBBs(MeFunction &func, Dominance &dom, BB *bb) {
  if (bb == nullptr || bb == func.GetCommonExitBB()) {
    return;
  }
  bool hasTry = false;
  for (BB *pred : bb->GetPred()) {
    // backege or preheader is try bb
    if (pred->GetAttributes(kBBAttrIsTry)) {
      hasTry = true;
    }
  }
  if (!hasTry) {
    for (BB *pred : bb->GetPred()) {
      // add backege bb
      if (dom.Dominate(*bb, *pred)) {
        if (heads.find(bb) != heads.end()) {
          heads[bb].push_back(pred);
        } else {
          std::vector<BB*> tails;
          tails.push_back(pred);
          heads[bb] = tails;
        }
      }
    }
  }
  const MapleSet<BBId> &domChildren = dom.GetDomChildren(bb->GetBBId());
  for (auto bbit = domChildren.begin(); bbit != domChildren.end(); ++bbit) {
    FindHeadBBs(func, dom, func.GetAllBBs().at(*bbit));
  }
}

// merge backedges with the same headBB
void MeDoLoopCanon::Merge(MeFunction &func) {
  for (auto iter = heads.begin(); iter != heads.end(); ++iter) {
    BB *head = iter->first;
    auto *latchBB = func.NewBasicBlock();
    latchBB->SetAttributes(kBBAttrArtificial);
    latchBB->SetKind(kBBFallthru);
    for (BB *tail : iter->second) {
      tail->ReplaceSucc(head, latchBB);
      if (tail->GetStmtNodes().empty()) {
        continue;
      }
      if (tail->GetKind() == kBBCondGoto) {
        CondGotoNode &condGotoNode = static_cast<CondGotoNode&>(tail->GetStmtNodes().back());
        LabelIdx oldlabIdx = condGotoNode.GetOffset();
        if (oldlabIdx == head->GetBBLabel()) {
          LabelIdx label = func.GetOrCreateBBLabel(*latchBB);
          condGotoNode.SetOffset(label);
        }
      } else if (tail->GetKind() == kBBGoto) {
        GotoNode &gotoStmt = static_cast<GotoNode&>(tail->GetStmtNodes().back());
        LabelIdx oldlabIdx = gotoStmt.GetOffset();
        if (oldlabIdx == head->GetBBLabel()) {
          LabelIdx label = func.GetOrCreateBBLabel(*latchBB);
          gotoStmt.SetOffset(label);
        }
      } else if (tail->GetKind() == kBBSwitch) {
        SwitchNode &switchNode = static_cast<SwitchNode &>(tail->GetStmtNodes().back());
        if (switchNode.GetDefaultLabel() == head->GetBBLabel()) {
          switchNode.SetDefaultLabel(func.GetOrCreateBBLabel(*latchBB));
        }
        for (size_t i = 0; i < switchNode.GetSwitchTable().size(); ++i) {
          LabelIdx labelIdx = switchNode.GetCasePair(i).second;
          if (labelIdx == head->GetBBLabel()) {
            switchNode.UpdateCaseLabelAt(i, func.GetOrCreateBBLabel(*latchBB));
          }
        }
      } else if (tail->GetKind() != kBBFallthru) {
        CHECK_FATAL(false, "can not support");
      }
    }
    head->AddPred(*latchBB);
  }
}

void MeDoLoopCanon::AddPreheader(MeFunction &func) {
  for (auto iter = heads.begin(); iter != heads.end(); ++iter) {
    BB *head = iter->first;
    std::vector<BB*> preds;
    for (BB *pred : head->GetPred()) {
      // FindHeadBBs has filtered out this possibility.
      CHECK_FATAL(!pred->GetAttributes(kBBAttrIsTry), "can not support kBBAttrIsTry");
      if (std::find(iter->second.begin(), iter->second.end(), pred) == iter->second.end()) {
        preds.push_back(pred);
      }
    }
    // If the num of backages is zero or one and bb kind is kBBFallthru, Preheader is already canonical.
    if (preds.empty()) {
      continue;
    }
    if (preds.size() == 1 && preds.front()->GetKind() == kBBFallthru) {
      continue;
    }
    // add preheader
    auto *preheader = func.NewBasicBlock();
    preheader->SetAttributes(kBBAttrArtificial);
    preheader->SetKind(kBBFallthru);
    for (BB *pred : preds) {
      pred->ReplaceSucc(head, preheader);
      if (pred->GetStmtNodes().empty()) {
        continue;
      }
      if (pred->GetKind() == kBBCondGoto) {
        CondGotoNode &condGotoNode = static_cast<CondGotoNode&>(pred->GetStmtNodes().back());
        LabelIdx oldlabIdx = condGotoNode.GetOffset();
        if (oldlabIdx == head->GetBBLabel()) {
          LabelIdx label = func.GetOrCreateBBLabel(*preheader);
          condGotoNode.SetOffset(label);
        }
      } else if (pred->GetKind() == kBBGoto) {
        GotoNode &gotoStmt = static_cast<GotoNode&>(pred->GetStmtNodes().back());
        LabelIdx oldlabIdx = gotoStmt.GetOffset();
        if (oldlabIdx == head->GetBBLabel()) {
          LabelIdx label = func.GetOrCreateBBLabel(*preheader);
          gotoStmt.SetOffset(label);
        }
      } else if (pred->GetKind() == kBBSwitch) {
        SwitchNode &switchNode = static_cast<SwitchNode&>(pred->GetStmtNodes().back());
        if (switchNode.GetDefaultLabel() == head->GetBBLabel()) {
          switchNode.SetDefaultLabel(func.GetOrCreateBBLabel(*preheader));
        }
        for (size_t i = 0; i < switchNode.GetSwitchTable().size(); ++i) {
          LabelIdx labelIdx = switchNode.GetCasePair(i).second;
          if (labelIdx == head->GetBBLabel()) {
            switchNode.UpdateCaseLabelAt(i, func.GetOrCreateBBLabel(*preheader));
          }
        }
      } else if (pred->GetKind() != kBBFallthru) {
        CHECK_FATAL(false, "can not support");
      }
    }
    head->AddPred(*preheader);
  }
}

void MeDoLoopCanon::InsertNewExitBB(MeFunction &func, LoopDesc &loop) {
  for (auto pair : loop.inloopBB2exitBBs) {
    BB *curBB = pair.first;
    for (auto succBB : *pair.second) {
      bool needNewExitBB = false;
      for (auto pred : succBB->GetPred()) {
        if (!loop.Has(*pred)) {
          needNewExitBB = true;
        }
      }
      if (!needNewExitBB) {
        continue;
      } else {
        BB *newExitBB = func.NewBasicBlock();
        newExitBB->SetKind(kBBFallthru);
        size_t index = succBB->GetPred().size();
        while (index > 0) {
          if (succBB->GetPred(index - 1) == curBB) {
            break;
          }
          --index;
        }
        curBB->ReplaceSucc(succBB, newExitBB);
        --index;
        succBB->AddPred(*newExitBB, index);

        loop.ReplaceInloopBB2exitBBs(*curBB, *succBB, *newExitBB);
        if (curBB->GetStmtNodes().empty()) {
          continue;
        }
        StmtNode *lastStmt = &(curBB->GetStmtNodes().back());
        if ((lastStmt->GetOpCode() == OP_brtrue || lastStmt->GetOpCode() == OP_brfalse) &&
            static_cast<CondGotoNode*>(lastStmt)->GetOffset() == succBB->GetBBLabel()) {
          LabelIdx label = func.GetOrCreateBBLabel(*newExitBB);
          static_cast<CondGotoNode*>(lastStmt)->SetOffset(label);
        } else if (lastStmt->GetOpCode() == OP_goto &&
                   static_cast<GotoNode*>(lastStmt)->GetOffset() == succBB->GetBBLabel()) {
          LabelIdx label = func.GetOrCreateBBLabel(*newExitBB);
          static_cast<GotoNode*>(lastStmt)->SetOffset(label);
        }
      }
    }
  }
}

void MeDoLoopCanon::InsertExitBB(MeFunction &func, LoopDesc &loop) {
  std::set<BB*> traveledBBs;
  std::queue<BB*> inLoopBBs;
  inLoopBBs.push(loop.head);
  CHECK_FATAL(loop.inloopBB2exitBBs.empty(), "inloopBB2exitBBs must be empty");
  while (!inLoopBBs.empty()) {
    BB *curBB = inLoopBBs.front();
    inLoopBBs.pop();
    for (BB *succ : curBB->GetSucc()) {
      if (traveledBBs.count(succ) != 0) {
        continue;
      }
      if (loop.Has(*succ)) {
        inLoopBBs.push(succ);
        traveledBBs.insert(succ);
      } else {
        loop.InsertInloopBB2exitBBs(*curBB, *succ);
      }
      if (curBB->GetKind() == kBBCondGoto) {
        if (curBB->GetSucc().size() == 1) {
          CHECK_FATAL(false, "return bb");
          loop.InsertInloopBB2exitBBs(*curBB, *func.GetCommonExitBB());
        }
      } else if (!curBB->GetStmtNodes().empty() && curBB->GetLast().GetOpCode() == OP_return) {
        CHECK_FATAL(false, "return bb");
        loop.InsertInloopBB2exitBBs(*curBB, *func.GetCommonExitBB());
      }
    }
  }
  InsertNewExitBB(func, loop);
}

void MeDoLoopCanon::SplitCondGotBB(MeFunction &func, LoopDesc &loop) {
  auto exitBB = loop.inloopBB2exitBBs.begin()->first;
  StmtNode *lastStmt = &(exitBB->GetStmtNodes().back());
  if (lastStmt->GetOpCode() != OP_brfalse && lastStmt->GetOpCode() != OP_brtrue && lastStmt->GetOpCode() != OP_switch) {
    CHECK_FATAL(false, "must be OP_brfalse, OP_brtrue or OP_switch");
  }
  bool notOnlyHasBrStmt = true;
  for (auto &stmt : exitBB->GetStmtNodes()) {
    if (&stmt != &exitBB->GetLast()) {
      notOnlyHasBrStmt = false;
      break;
    }
  }
  if (notOnlyHasBrStmt) {
    return;
  }
  BB *newFallthru = func.NewBasicBlock();
  newFallthru->SetKind(kBBFallthru);
  newFallthru->SetAttributes(kBBAttrIsInLoop);
  func.CloneBasicBlock(*newFallthru, *exitBB);
  newFallthru->RemoveLastStmt();
  for (auto &stmt : exitBB->GetStmtNodes()) {
    if (&stmt != &exitBB->GetLast()) {
      exitBB->RemoveStmtNode(&stmt);
    }
  }
  while (exitBB->GetPred().size() > 0) {
    auto *pred = exitBB->GetPred(0);
    pred->ReplaceSucc(exitBB, newFallthru);
    if (pred->GetStmtNodes().empty()) {
      continue;
    }
    StmtNode *lastStmt = &(pred->GetStmtNodes().back());
    if ((lastStmt->GetOpCode() == OP_brtrue || lastStmt->GetOpCode() == OP_brfalse) &&
        static_cast<CondGotoNode*>(lastStmt)->GetOffset() == exitBB->GetBBLabel()) {
      LabelIdx label = func.GetOrCreateBBLabel(*newFallthru);
      static_cast<CondGotoNode*>(lastStmt)->SetOffset(label);
    } else if (lastStmt->GetOpCode() == OP_goto &&
               static_cast<GotoNode*>(lastStmt)->GetOffset() == exitBB->GetBBLabel()) {
      LabelIdx label = func.GetOrCreateBBLabel(*newFallthru);
      static_cast<GotoNode*>(lastStmt)->SetOffset(label);
    }
  }
  exitBB->RemoveAllPred();
  newFallthru->AddSucc(*exitBB);
}

bool MeDoLoopCanon::IsDoWhileLoop(const LoopDesc &loop) const {
  for (auto succ : loop.head->GetSucc()) {
    if (!loop.Has(*succ)) {
      return false;
    }
  }
  auto exitBB = loop.inloopBB2exitBBs.begin()->first;
  for (auto pred : exitBB->GetPred()) {
    if (!loop.Has(*pred)) {
      return false;
    }
  }
  return true;
}

void MeDoLoopCanon::ExecuteLoopNormalization(MeFunction &func,  MeFuncResultMgr *m, Dominance &dom) {
  if (DEBUGFUNC(&func)) {
    LogInfo::MapleLogger() << "-----------------Dump mefunction before loop normalization----------\n";
    func.Dump(true);
    func.GetTheCfg()->DumpToFile("cfgbeforLoopNormalization");
  }
  heads.clear();
  FindHeadBBs(func, dom, func.GetCommonEntryBB());
  AddPreheader(func);
  Merge(func);
  m->InvalidAnalysisResult(MeFuncPhase_DOMINANCE, &func);
  m->InvalidAnalysisResult(MeFuncPhase_MELOOP, &func);
  IdentifyLoops *meLoop = static_cast<IdentifyLoops*>(m->GetAnalysisResult(MeFuncPhase_MELOOP, &func));
  if (meLoop == nullptr) {
    return;
  }
  for (auto loop : meLoop->GetMeLoops()) {
    if (loop->HasTryBB()) {
      continue;
    }

    if (!loop->IsCanonicalLoop()) {
      CHECK_FATAL(loop->inloopBB2exitBBs.size() == 0, "must be zero");
      InsertExitBB(func, *loop);
      loop->SetIsCanonicalLoop(true);
    }
    if (loop->inloopBB2exitBBs.size() == 1 && loop->inloopBB2exitBBs.begin()->second->size() == 1 &&
        IsDoWhileLoop(*loop)) {
      SplitCondGotBB(func, *loop);
    }
  }
  if (DEBUGFUNC(&func)) {
    LogInfo::MapleLogger() << "-----------------Dump mefunction after loop normalization-----------\n";
    func.Dump(true);
    func.GetTheCfg()->DumpToFile("cfgafterLoopNormalization");
  }
  m->InvalidAnalysisResult(MeFuncPhase_DOMINANCE, &func);
  m->InvalidAnalysisResult(MeFuncPhase_MELOOP, &func);
}

AnalysisResult *MeDoLoopCanon::Run(MeFunction *func, MeFuncResultMgr *m, ModuleResultMgr*) {
  auto *dom = static_cast<Dominance*>(m->GetAnalysisResult(MeFuncPhase_DOMINANCE, func));
  ASSERT(dom != nullptr, "dom is null in MeDoLoopCanon::Run");
  ExecuteLoopCanon(*func, *m, *dom);
  dom = static_cast<Dominance*>(m->GetAnalysisResult(MeFuncPhase_DOMINANCE, func));
  ExecuteLoopNormalization(*func, m, *dom);
  return nullptr;
}
}  // namespace maple
