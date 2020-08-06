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
#include "me_bb_layout.h"
#include "me_cfg.h"
#include "bb.h"
#include "me_irmap.h"
#include "me_option.h"

// This BB layout strategy strictly obeys source ordering when inside try blocks.
// This Optimization will reorder the bb layout. it start from the first bb of func.
// All bbs will be put into layoutBBs and it gives the determined layout order.
// The entry of the optimization is MeDoBBLayout::Run. If func's IR profile is
// valid,use the Pettis & Hansen intra func bb layout.refer the following paper:
// Profile Guided Code Positioning
// The idea is when there's a branch, put the most probility target next to
// the branch to get the minimum jump distance.
// If the profile is invalid, do the normal bb layout:
// It starts from the first bb.
// 1. If curBB is condtion goto or goto kind, do OptimizeBranchTarget for bb.
// 2. Find curBB's next bb nextBB, and based on nextBB do the following:
// 3. (1) For fallthru/catch/finally, fix curBB's fallthru
//    (2) For condtion goto curBB:
//        i) If the target bb can be moved, then put it as currBB's next
//           and retarget curBB to it's fallthru bb. add targetBB as next.
//        ii) If curBB's fallthru is not its next bb add fallthru as its next if
//            fallthru can be moved else create a new fallthru contains a goto to
//            the original fallthru
//    (3) For goto curBB see if goto target can be placed as next.
// 5. do step 3 for nextBB until all bbs are laid out
namespace maple {
static void CreateGoto(BB &bb, MeFunction &func, BB &fallthru) {
  LabelIdx label = func.GetOrCreateBBLabel(fallthru);
  if (func.GetIRMap() != nullptr) {
    GotoNode stmt(OP_goto);
    auto *newGoto = func.GetIRMap()->New<GotoMeStmt>(&stmt);
    newGoto->SetOffset(label);
    bb.AddMeStmtLast(newGoto);
  } else {
    auto *newGoto = func.GetMirFunc()->GetCodeMempool()->New<GotoNode>(OP_goto);
    newGoto->SetOffset(label);
    bb.AddStmtNode(newGoto);
  }
  bb.SetKind(kBBGoto);
}

// return true if bb is empty and its kind is fallthru.
bool BBLayout::BBEmptyAndFallthru(const BB &bb) {
  if (bb.GetAttributes(kBBAttrIsTryEnd)) {
    return false;
  }
  if (bb.GetKind() == kBBFallthru) {
    if (func.GetIRMap() != nullptr) {
      return bb.IsMeStmtEmpty();
    }
    return bb.IsEmpty();
  }
  return false;
}

// Return true if bb only has conditonal branch stmt except comment
bool BBLayout::BBContainsOnlyCondGoto(const BB &bb) const {
  if (bb.GetKind() != kBBCondGoto || bb.GetAttributes(kBBAttrIsTryEnd)) {
    return false;
  }

  if (func.GetIRMap() != nullptr) {
    auto &meStmts = bb.GetMeStmts();
    if (meStmts.empty()) {
      return false;
    }
    for (auto itMeStmt = meStmts.begin(); itMeStmt != meStmts.rbegin().base(); ++itMeStmt) {
      if (!itMeStmt->IsCondBr() && itMeStmt->GetOp() != OP_comment) {
        return false;
      }
    }
    return meStmts.back().IsCondBr();
  }
  auto &stmtNodes = bb.GetStmtNodes();
  if (stmtNodes.empty()) {
    return false;
  }
  for (auto itStmt = stmtNodes.begin(); itStmt != stmtNodes.rbegin().base(); ++itStmt) {
    if (!itStmt->IsCondBr() && itStmt->GetOpCode() != OP_comment) {
      return false;
    }
  }
  return bb.GetStmtNodes().back().IsCondBr();
}

bool BBLayout::ChooseTargetAsFallthru(const BB &bb, const BB &targetBB, const BB &oldFallThru,
                                      const BB &fallthru) const {
  if (&targetBB == &fallthru) {
    return false;
  }
  if (profValid) {
    uint64 freqToTargetBB = bb.GetEdgeFreq(&targetBB);
    uint64 freqToFallthru = bb.GetEdgeFreq(&fallthru);
    if (enabledDebug) {
      LogInfo::MapleLogger() << func.GetName() << " " << bb.GetBBId() << "->" << targetBB.GetBBId() << " freq "
                             << freqToTargetBB << " " << bb.GetBBId() << "->" << fallthru.GetBBId() << " freq "
                             << freqToFallthru << '\n';
    }
    if ((freqToTargetBB > freqToFallthru) && BBCanBeMovedBasedProf(targetBB, bb)) {
      if (enabledDebug) {
        LogInfo::MapleLogger() << func.GetName() << bb.GetBBId() << " move targeBB " << targetBB.GetBBId()
                               << " to fallthru" << '\n';
      }
      return true;
    }
  } else {
    if ((&oldFallThru != &fallthru || fallthru.GetPred().size() > 1)
        && BBCanBeMoved(targetBB, bb)) {
      return true;
    }
  }
  return false;
}
// Return the opposite opcode for condition/compare opcode.
static Opcode GetOppositeOp(Opcode opcInput) {
  Opcode opc = OP_undef;
  switch (opcInput) {
    case OP_brtrue:
      opc = OP_brfalse;
      break;
    case OP_brfalse:
      opc = OP_brtrue;
      break;
    case OP_ne:
      opc = OP_eq;
      break;
    case OP_eq:
      opc = OP_ne;
      break;
    case OP_gt:
      opc = OP_le;
      break;
    case OP_le:
      opc = OP_gt;
      break;
    case OP_lt:
      opc = OP_ge;
      break;
    case OP_ge:
      opc = OP_lt;
      break;
    default:
      break;
  }
  return opc;
}

bool BBLayout::BBContainsOnlyGoto(const BB &bb) const {
  if (bb.GetKind() != kBBGoto || bb.GetAttributes(kBBAttrIsTryEnd)) {
    return false;
  }

  if (func.GetIRMap() != nullptr) {
    auto &meStmts = bb.GetMeStmts();
    if (meStmts.empty()) {
      return false;
    }
    for (auto itMeStmt = meStmts.begin(); itMeStmt != meStmts.rbegin().base(); ++itMeStmt) {
      if (itMeStmt->GetOp() != OP_goto && itMeStmt->GetOp() != OP_comment) {
        return false;
      }
    }
    return meStmts.back().GetOp() == OP_goto;
  }
  auto &stmtNodes = bb.GetStmtNodes();
  if (stmtNodes.empty()) {
    return false;
  }
  for (auto itStmt = stmtNodes.begin(); itStmt != stmtNodes.rbegin().base(); ++itStmt) {
    if (itStmt->GetOpCode() != OP_goto && itStmt->GetOpCode() != OP_comment) {
      return false;
    }
  }
  return bb.GetStmtNodes().back().GetOpCode() == OP_goto;
}

// Return true if all the following are satisfied:
// 1.fromBB only has one predecessor
// 2.fromBB has not been laid out.
// 3.fromBB has only one succor when fromBB is artifical or fromBB and
//   toafter_bb are both not in try block.
// The other case is fromBB has one predecessor and one successor and
// contains only goto stmt.
bool BBLayout::BBCanBeMoved(const BB &fromBB, const BB &toAfterBB) const {
  if (fromBB.GetPred().size() > 1) {
    return false;
  }
  if (laidOut[fromBB.GetBBId()]) {
    return false;
  }
  if (fromBB.GetAttributes(kBBAttrArtificial) ||
      (!fromBB.GetAttributes(kBBAttrIsTry) && !toAfterBB.GetAttributes(kBBAttrIsTry))) {
    return fromBB.GetSucc().size() == 1;
  }
  return BBContainsOnlyGoto(fromBB);
}

// Return true if all the following are satisfied:
// 1.fromBB has not been laid out.
// 2.fromBB has only one succor when fromBB is artifical or fromBB and
//   toafter_bb are both not in try block.
// The other case is fromBB contains only goto stmt.
bool BBLayout::BBCanBeMovedBasedProf(const BB &fromBB, const BB &toAfterBB) const {
  if (laidOut[fromBB.GetBBId()]) {
    return false;
  }
  if (fromBB.GetAttributes(kBBAttrArtificial) ||
      (!fromBB.GetAttributes(kBBAttrIsTry) && !toAfterBB.GetAttributes(kBBAttrIsTry))) {
    return fromBB.GetSucc().size() <= 1;
  }
  return BBContainsOnlyGoto(fromBB);
}

// Return true if bb1 and bb2 has the branch conditon.such as
// bb1 : brfalse (a > 3)  bb2: brfalse (a > 3)/ brtrue (a <= 3)
bool BBLayout::HasSameBranchCond(BB &bb1, BB &bb2) const {
  if (func.GetIRMap() == nullptr) {
    return false;
  }
  auto &meStmt1 = static_cast<CondGotoMeStmt&>(bb1.GetMeStmts().back());
  auto &meStmt2 = static_cast<CondGotoMeStmt&>(bb2.GetMeStmts().back());
  MeExpr *expr1 = meStmt1.GetOpnd();
  MeExpr *expr2 = meStmt2.GetOpnd();
  // Compare the opcode:  brtrue/brfalse
  if (!(meStmt1.GetOp() == meStmt2.GetOp() && expr1->GetOp() == expr2->GetOp()) &&
      !(meStmt1.GetOp() == GetOppositeOp(meStmt2.GetOp()) && expr1->GetOp() == GetOppositeOp(expr2->GetOp()))) {
    return false;
  }
  if (!(expr1->GetMeOp() == expr2->GetMeOp() && expr1->GetMeOp() == kMeOpOp)) {
    return false;
  }
  auto *opMeExpr1 = static_cast<OpMeExpr*>(expr1);
  auto *opMeExpr2 = static_cast<OpMeExpr*>(expr2);
  // Compare the two operands to make sure they are both equal.
  if (opMeExpr1->GetOpnd(0) != opMeExpr2->GetOpnd(0)) {
    return false;
  }
  // If one side is const, assume it is always the rhs.
  if ((opMeExpr1->GetOpnd(1) != opMeExpr2->GetOpnd(1)) &&
      !(opMeExpr1->GetOpnd(1)->IsZero() && opMeExpr2->GetOpnd(1)->IsZero())) {
    return false;
  }
  return true;
}

// (1) bb's last statement is a conditional or unconditional branch; if the branch
// target is a BB with only a single goto statement, optimize the branch target
// to the eventual target
// (2) bb's last statement is a conditonal branch, if the branch target is a BB with a single
// condtioal branch statement and has the same condtion as bb's last statement, optimize the
// branch target to the eventual target.
void BBLayout::OptimizeBranchTarget(BB &bb) {
  if (func.GetIRMap() != nullptr) {
    auto &meStmts = bb.GetMeStmts();
    if (meStmts.empty()) {
      return;
    }
    if (meStmts.back().GetOp() != OP_goto && !meStmts.back().IsCondBr()) {
      return;
    }
  } else {
    auto &stmtNodes = bb.GetStmtNodes();
    if (stmtNodes.empty()) {
      return;
    }
    if (stmtNodes.back().GetOpCode() != OP_goto && !stmtNodes.back().IsCondBr()) {
      return;
    }
  }
  do {
    ASSERT(!bb.GetSucc().empty(), "container check");
    BB *brTargetBB = bb.GetKind() == kBBCondGoto ? bb.GetSucc().back() : bb.GetSucc().front();
    if (brTargetBB->GetAttributes(kBBAttrWontExit)) {
      return;
    }
    if (!BBContainsOnlyGoto(*brTargetBB) && !BBEmptyAndFallthru(*brTargetBB) &&
        !(bb.GetKind() == kBBCondGoto && brTargetBB->GetKind() == kBBCondGoto && &bb != brTargetBB &&
          BBContainsOnlyCondGoto(*brTargetBB) && HasSameBranchCond(bb, *brTargetBB))) {
      return;
    }
    // optimize stmt
    BB *newTargetBB = brTargetBB->GetSucc().front();
    if (brTargetBB->GetKind() == kBBCondGoto) {
      newTargetBB = brTargetBB->GetSucc().back();
    }
    LabelIdx newTargetLabel = func.GetOrCreateBBLabel(*newTargetBB);
    if (func.GetIRMap() != nullptr) {
      auto &lastStmt = bb.GetMeStmts().back();
      if (lastStmt.GetOp() == OP_goto) {
        auto &gotoMeStmt = static_cast<GotoMeStmt&>(lastStmt);
        ASSERT(brTargetBB->GetBBLabel() == gotoMeStmt.GetOffset(), "OptimizeBranchTarget: wrong branch target BB");
        gotoMeStmt.SetOffset(newTargetLabel);
      } else {
        auto &gotoMeStmt = static_cast<CondGotoMeStmt&>(lastStmt);
        ASSERT(brTargetBB->GetBBLabel() == gotoMeStmt.GetOffset(), "OptimizeBranchTarget: wrong branch target BB");
        gotoMeStmt.SetOffset(newTargetLabel);
      }
    } else {
      StmtNode &lastStmt = bb.GetStmtNodes().back();
      if (lastStmt.GetOpCode() == OP_goto) {
        auto &gotoNode = static_cast<GotoNode&>(lastStmt);
        ASSERT(brTargetBB->GetBBLabel() == gotoNode.GetOffset(), "OptimizeBranchTarget: wrong branch target BB");
        gotoNode.SetOffset(newTargetLabel);
      } else {
        auto &gotoNode = static_cast<CondGotoNode&>(lastStmt);
        ASSERT(brTargetBB->GetBBLabel() == gotoNode.GetOffset(), "OptimizeBranchTarget: wrong branch target BB");
        gotoNode.SetOffset(newTargetLabel);
      }
    }
    // update CFG
    bb.ReplaceSucc(brTargetBB, newTargetBB);
    if (brTargetBB->GetPred().empty()) {
      laidOut[brTargetBB->GetBBId()] = true;
      RemoveUnreachable(*brTargetBB);
      if (needDealWithTryBB) {
        DealWithStartTryBB();
      }
    }
  } while (true);
}

void BBLayout::AddBB(BB &bb) {
  CHECK_FATAL(bb.GetBBId() < laidOut.size(), "index out of range in BBLayout::AddBB");
  ASSERT(!laidOut[bb.GetBBId()], "AddBB: bb already laid out");
  layoutBBs.push_back(&bb);
  laidOut[bb.GetBBId()] = true;
  if (enabledDebug) {
    LogInfo::MapleLogger() << "bb id " << bb.GetBBId() << " kind is " << bb.StrAttribute();
  }
  bool isTry = false;
  if (func.GetIRMap() != nullptr) {
    isTry = !bb.GetMeStmts().empty() && bb.GetMeStmts().front().GetOp() == OP_try;
  } else {
    isTry = !bb.GetStmtNodes().empty() && bb.GetStmtNodes().front().GetOpCode() == OP_try;
  }
  if (isTry) {
    ASSERT(!tryOutstanding, "BBLayout::AddBB: cannot lay out another try without ending the last one");
    tryOutstanding = true;
    if (enabledDebug) {
      LogInfo::MapleLogger() << " try";
    }
  }
  if (bb.GetAttributes(kBBAttrIsTryEnd) && func.GetMIRModule().IsJavaModule()) {
    tryOutstanding = false;
    if (enabledDebug) {
      LogInfo::MapleLogger() << " endtry";
    }
  }
  if (enabledDebug) {
    LogInfo::MapleLogger() << '\n';
  }
}

BB *BBLayout::GetFallThruBBSkippingEmpty(BB &bb) {
  ASSERT(bb.GetKind() == kBBFallthru || bb.GetKind() == kBBCondGoto, "GetFallThruSkippingEmpty: unexpected BB kind");
  ASSERT(!bb.GetSucc().empty(), "container check");
  BB *fallthru = bb.GetSucc().front();
  do {
    if (fallthru->GetPred().size() > 1 || fallthru->GetAttributes(kBBAttrIsTryEnd)) {
      return fallthru;
    }
    if (func.GetIRMap() != nullptr) {
      if (!fallthru->IsMeStmtEmpty()) {
        return fallthru;
      }
    } else {
      if (!fallthru->IsEmpty()) {
        return fallthru;
      }
    }
    laidOut[fallthru->GetBBId()] = true;
    BB *oldFallThru = fallthru;
    fallthru = fallthru->GetSucc().front();
    bb.ReplaceSucc(oldFallThru, fallthru);
    if (oldFallThru->GetPred().empty()) {
      RemoveUnreachable(*oldFallThru);
      if (needDealWithTryBB) {
        DealWithStartTryBB();
      }
    }
  } while (true);
}

// bb end with a goto statement; remove the goto stmt if its target
// is its fallthru nextBB.
void BBLayout::ChangeToFallthruFromGoto(BB &bb) {
  ASSERT(bb.GetKind() == kBBGoto, "ChangeToFallthruFromGoto: unexpected BB kind");
  if (func.GetIRMap() != nullptr) {
    bb.RemoveMeStmt(to_ptr(bb.GetMeStmts().rbegin()));
  } else {
    bb.RemoveLastStmt();
  }
  bb.SetKind(kBBFallthru);
}

// bb does not end with a branch statement; if its fallthru is not nextBB,
// perform the fix by either laying out the fallthru immediately or adding a goto
void BBLayout::ResolveUnconditionalFallThru(BB &bb, BB &nextBB) {
  if (bb.GetKind() != kBBFallthru) {
    return;
  }
  ASSERT(bb.GetAttributes(kBBAttrIsTry) || bb.GetAttributes(kBBAttrWontExit) || bb.GetSucc().size() == 1,
         "runtime check error");
  BB *fallthru = GetFallThruBBSkippingEmpty(bb);
  if (fallthru != &nextBB) {
    if (BBCanBeMoved(*fallthru, bb)) {
      AddBB(*fallthru);
      ResolveUnconditionalFallThru(*fallthru, nextBB);
      OptimizeBranchTarget(*fallthru);
    } else {
      CreateGoto(bb, func, *fallthru);
      OptimizeBranchTarget(bb);
    }
  }
}

void BBLayout::FixEndTryBB(BB &bb) {
  BBId prevID = bb.GetBBId() - 1UL;
  for (BBId id = prevID; id != 0; --id) {
    auto prevBB = func.GetBBFromID(id);
    if (prevBB != nullptr) {
      if (prevBB->GetAttributes(kBBAttrIsTry) && !prevBB->GetAttributes(kBBAttrIsTryEnd)) {
        prevBB->SetAttributes(kBBAttrIsTryEnd);
        func.SetTryBBByOtherEndTryBB(prevBB, &bb);
      }
      break;
    }
  }
}

void BBLayout::FixTryBB(BB &startTryBB, BB &nextBB) {
  startTryBB.RemoveAllPred();
  for (size_t i = 0; i < nextBB.GetPred().size(); ++i) {
    nextBB.GetPred(i)->ReplaceSucc(&nextBB, &startTryBB);
  }
  nextBB.RemoveAllPred();
  ASSERT(startTryBB.GetSucc().empty(), "succ of try should have been removed");
  startTryBB.AddSucc(nextBB);
}

void BBLayout::DealWithStartTryBB() {
  size_t size = startTryBBVec.size();
  for (size_t i = 0; i < size; ++i) {
    if (!startTryBBVec[i]) {
      continue;
    }
    auto curBB = func.GetBBFromID(BBId(i));
    for (size_t j = i + 1; j < size && !startTryBBVec[j]; ++j) {
      auto nextBB = func.GetBBFromID(BBId(j));
      if (nextBB != nullptr) {
        if (nextBB->GetAttributes(kBBAttrIsTry)) {
          FixTryBB(*curBB, *nextBB);
        } else {
          curBB->RemoveAllSucc();
          func.NullifyBBByID(curBB->GetBBId());
        }
        break;
      } else if (j == size - 1) {
        curBB->RemoveAllSucc();
        func.NullifyBBByID(curBB->GetBBId());
      }
    }
    startTryBBVec[i] = false;
  }
  needDealWithTryBB = false;
}

// remove unnessary bb whose pred size is zero
// keep cfg correct to rebuild dominance
void BBLayout::RemoveUnreachable(BB &bb) {
  if (bb.GetAttributes(kBBAttrIsEntry)) {
    return;
  }

  while (!bb.GetSucc().empty()) {
    BB *succ = bb.GetSucc(0);
    succ->RemovePred(bb, false);
    if (succ->GetPred().empty()) {
      RemoveUnreachable(*succ);
    }
  }

  if (bb.GetAttributes(kBBAttrIsTry) && !bb.GetAttributes(kBBAttrIsTryEnd)) {
    // identify if try bb is the start try bb
    if (!bb.GetMeStmts().empty() && bb.GetMeStmts().front().GetOp() == OP_try) {
      startTryBBVec[bb.GetBBId()] = true;
      needDealWithTryBB = true;
      return;
    }
  }
  if (bb.GetAttributes(kBBAttrIsTryEnd)) {
    FixEndTryBB(bb);
  }
  bb.RemoveAllSucc();
  func.NullifyBBByID(bb.GetBBId());
}

// create a new fallthru that contains a goto to the original fallthru
// bb              bb
//  |     ====>    |
//  |              [NewCreated]
//  |              |
// fallthru       fallthru
BB *BBLayout::CreateGotoBBAfterCondBB(BB &bb, BB &fallthru) {
  ASSERT(bb.GetKind() == kBBCondGoto, "CreateGotoBBAfterCondBB: unexpected BB kind");
  BB *newFallthru = func.NewBasicBlock();
  newFallthru->SetAttributes(kBBAttrArtificial);
  AddLaidOut(false);
  newFallthru->SetKind(kBBGoto);
  SetNewBBInLayout();
  LabelIdx fallthruLabel = func.GetOrCreateBBLabel(fallthru);
  if (func.GetIRMap() != nullptr) {
    GotoNode stmt(OP_goto);
    auto *newGoto = func.GetIRMap()->New<GotoMeStmt>(&stmt);
    newGoto->SetOffset(fallthruLabel);
    newFallthru->SetFirstMe(newGoto);
    newFallthru->SetLastMe(to_ptr(newFallthru->GetMeStmts().begin()));
  } else {
    auto *newGoto = func.GetMirFunc()->GetCodeMempool()->New<GotoNode>(OP_goto);
    newGoto->SetOffset(fallthruLabel);
    newFallthru->SetFirst(newGoto);
    newFallthru->SetLast(newFallthru->GetStmtNodes().begin().d());
  }
  // replace pred and succ
  size_t index = fallthru.GetPred().size();
  while (index > 0) {
    if (fallthru.GetPred(index - 1) == &bb) {
      break;
    }
    index--;
  }
  bb.ReplaceSucc(&fallthru, newFallthru);
  // pred has been remove for pred vector of succ
  // means size reduced, so index reduced
  index--;
  fallthru.AddPred(*newFallthru, index);
  newFallthru->SetFrequency(fallthru.GetFrequency());
  if (enabledDebug) {
    LogInfo::MapleLogger() << "Created fallthru and goto original fallthru" << '\n';
  }
  AddBB(*newFallthru);
  return newFallthru;
}

void BBLayout::DumpBBPhyOrder() const {
  LogInfo::MapleLogger() << func.GetName() << " final BB order " <<  '\n';
  for (auto bb : layoutBBs) {
    LogInfo::MapleLogger() << bb->GetBBId();
    if (bb != layoutBBs.back()) {
      LogInfo::MapleLogger() << "-->";
    }
  }
  LogInfo::MapleLogger() << '\n';
}

void BBLayout::OptimiseCFG() {
  auto eIt = func.valid_end();
  for (auto bIt = func.valid_begin(); bIt != eIt; ++bIt) {
    if (bIt == func.common_entry() || bIt == func.common_exit()) {
      continue;
    }
    auto *bb = *bIt;
    if (bb->GetKind() == kBBCondGoto || bb->GetKind() == kBBGoto) {
      OptimizeBranchTarget(*bb);
    }
  }
}

void BBLayout::LayoutWithoutProf() {
  BB *bb = func.GetFirstBB();
  while (bb != nullptr) {
    AddBB(*bb);
    if (bb->GetKind() == kBBCondGoto || bb->GetKind() == kBBGoto) {
      OptimizeBranchTarget(*bb);
    }
    BB *nextBB = NextBB();
    if (nextBB != nullptr) {
      // check try-endtry correspondence
      bool isTry = false;
      if (func.GetIRMap() != nullptr) {
        isTry = !nextBB->GetMeStmts().empty() && nextBB->GetMeStmts().front().GetOp() == OP_try;
      } else {
        auto &stmtNodes = nextBB->GetStmtNodes();
        isTry = !stmtNodes.empty() && stmtNodes.front().GetOpCode() == OP_try;
      }
      ASSERT(!(isTry && GetTryOutstanding()), "cannot emit another try if last try has not been ended");
      if (nextBB->GetAttributes(kBBAttrIsTryEnd)) {
        ASSERT(func.GetTryBBFromEndTryBB(nextBB) == nextBB ||
               IsBBLaidOut(func.GetTryBBFromEndTryBB(nextBB)->GetBBId()),
               "cannot emit endtry bb before its corresponding try bb");
      }
    }
    // based on nextBB, may need to fix current bb's fall-thru
    if (bb->GetKind() == kBBFallthru) {
      ResolveUnconditionalFallThru(*bb, *nextBB);
    } else if (bb->GetKind() == kBBCondGoto) {
      BB *oldFallThru = bb->GetSucc(0);
      BB *fallthru = GetFallThruBBSkippingEmpty(*bb);
      BB *brTargetBB = bb->GetSucc(1);
      if (ChooseTargetAsFallthru(*bb, *brTargetBB, *oldFallThru, *fallthru)) {
        // flip the sense of the condgoto and lay out brTargetBB right here
        LabelIdx fallthruLabel = func.GetOrCreateBBLabel(*fallthru);
        if (func.GetIRMap() != nullptr) {
          auto &condGotoMeStmt = static_cast<CondGotoMeStmt&>(bb->GetMeStmts().back());
          ASSERT(brTargetBB->GetBBLabel() == condGotoMeStmt.GetOffset(), "bbLayout: wrong branch target BB");
          condGotoMeStmt.SetOffset(fallthruLabel);
          condGotoMeStmt.SetOp((condGotoMeStmt.GetOp() == OP_brtrue) ? OP_brfalse : OP_brtrue);
        } else {
          auto &condGotoNode = static_cast<CondGotoNode&>(bb->GetStmtNodes().back());
          ASSERT(brTargetBB->GetBBLabel() == condGotoNode.GetOffset(), "bbLayout: wrong branch target BB");
          condGotoNode.SetOffset(fallthruLabel);
          condGotoNode.SetOpCode((condGotoNode.GetOpCode() == OP_brtrue) ? OP_brfalse : OP_brtrue);
        }
        AddBB(*brTargetBB);
        ResolveUnconditionalFallThru(*brTargetBB, *nextBB);
        OptimizeBranchTarget(*brTargetBB);
      } else if (fallthru != nextBB) {
        if (BBCanBeMoved(*fallthru, *bb)) {
          AddBB(*fallthru);
          ResolveUnconditionalFallThru(*fallthru, *nextBB);
          OptimizeBranchTarget(*fallthru);
        } else {
          BB *newFallthru = CreateGotoBBAfterCondBB(*bb, *fallthru);
          OptimizeBranchTarget(*newFallthru);
        }
      }
    }
    if (bb->GetKind() == kBBGoto) {
      // see if goto target can be placed here
      BB *gotoTarget = bb->GetSucc().front();
      CHECK_FATAL(gotoTarget != nullptr, "null ptr check");

      if (gotoTarget != nextBB && BBCanBeMoved(*gotoTarget, *bb)) {
        AddBB(*gotoTarget);
        ChangeToFallthruFromGoto(*bb);
        ResolveUnconditionalFallThru(*gotoTarget, *nextBB);
        OptimizeBranchTarget(*gotoTarget);
      } else if (gotoTarget->GetKind() == kBBCondGoto && gotoTarget->GetPred().size() == 1) {
        BB *targetNext = gotoTarget->GetSucc().front();
        if (targetNext != nextBB && BBCanBeMoved(*targetNext, *bb)) {
          AddBB(*gotoTarget);
          ChangeToFallthruFromGoto(*bb);
          OptimizeBranchTarget(*gotoTarget);
          AddBB(*targetNext);
          ResolveUnconditionalFallThru(*targetNext, *nextBB);
          OptimizeBranchTarget(*targetNext);
        }
      }
    }
    if (nextBB != nullptr && IsBBLaidOut(nextBB->GetBBId())) {
      nextBB = NextBB();
    }
    bb = nextBB;
  }
}

void BBLayout::AddBBProf(BB &bb) {
  if (layoutBBs.empty()) {
    AddBB(bb);
    return;
  }
  BB *curBB = layoutBBs.back();
  if (curBB->GetKind() == kBBFallthru || curBB->GetKind() == kBBGoto) {
    BB *targetBB = curBB->GetSucc().front();
    if (curBB->GetKind() == kBBFallthru && (&bb != targetBB)) {
      CreateGoto(*curBB, func, *targetBB);
    }
    if (curBB->GetKind() == kBBGoto && (&bb == targetBB)) {
      // delete the goto stmt
      ChangeToFallthruFromGoto(*curBB);
    }
  }
  if (curBB->GetKind() == kBBCondGoto) {
    BB *fallthru = curBB->GetSucc(0);
    BB *targetBB = curBB->GetSucc(1);
    if (targetBB == &bb) {
      LabelIdx fallthruLabel = func.GetOrCreateBBLabel(*fallthru);
      if (func.GetIRMap() != nullptr) {
        auto &condGotoMeStmt = static_cast<CondGotoMeStmt&>(curBB->GetMeStmts().back());
        ASSERT(targetBB->GetBBLabel() == condGotoMeStmt.GetOffset(), "bbLayout: wrong branch target BB");
        condGotoMeStmt.SetOffset(fallthruLabel);
        condGotoMeStmt.SetOp((condGotoMeStmt.GetOp() == OP_brtrue) ? OP_brfalse : OP_brtrue);
      } else {
        auto &condGotoNode = static_cast<CondGotoNode&>(curBB->GetStmtNodes().back());
        ASSERT(targetBB->GetBBLabel() == condGotoNode.GetOffset(), "bbLayout: wrong branch target BB");
        condGotoNode.SetOffset(fallthruLabel);
        condGotoNode.SetOpCode((condGotoNode.GetOpCode() == OP_brtrue) ? OP_brfalse : OP_brtrue);
      }
    } else if (&bb != fallthru) {
      CreateGotoBBAfterCondBB(*curBB, *fallthru);
    }
  }
  AddBB(bb);
}

void BBLayout::BuildEdges() {
  auto eIt = func.valid_end();
  for (auto bIt = func.valid_begin(); bIt != eIt; ++bIt) {
    if (bIt == func.common_entry() || bIt == func.common_exit()) {
      continue;
    }
    auto *bb = *bIt;
    for (size_t i = 0; i < bb->GetSucc().size(); ++i) {
      BB *dest = bb->GetSucc(i);
      uint64 w = bb->GetEdgeFreq(i);
      allEdges.emplace_back(layoutAlloc.GetMemPool()->New<BBEdge>(bb, dest, w));
    }
  }
  std::stable_sort(allEdges.begin(), allEdges.end(), [](const BBEdge *edge1, const BBEdge *edge2) {
      return edge1->GetWeight() > edge2->GetWeight(); });
}

BB *BBLayout::GetBBFromEdges() {
  while (edgeIdx < allEdges.size()) {
    BBEdge *edge = allEdges[edgeIdx];
    BB *srcBB = edge->GetSrcBB();
    BB *destBB = edge->GetDestBB();
    if (enabledDebug) {
      LogInfo::MapleLogger() << srcBB->GetBBId() << "->" << destBB->GetBBId() << " freq "
                             << srcBB->GetEdgeFreq(destBB) << '\n';
    }

    if (!laidOut[srcBB->GetBBId()]) {
      if (enabledDebug) {
        LogInfo::MapleLogger() << "choose srcBB " << srcBB->GetBBId() << '\n';
      }
      return srcBB;
    }
    if (!laidOut[destBB->GetBBId()]) {
      if (enabledDebug) {
        LogInfo::MapleLogger() << "choose destBB " << destBB->GetBBId() << '\n';
      }
      return destBB;
    }
    if (enabledDebug) {
      LogInfo::MapleLogger() << "skip this edge " << '\n';
    }
    edgeIdx++;
  }
  return nullptr;
}
// find the most probility succ,if bb's succ more then one
// if no succ,choose the most weight edge in the edges
BB *BBLayout::NextBBProf(BB &bb) {
  if (bb.GetSucc().size() == 0) {
    return GetBBFromEdges();
  }

  if (bb.GetSucc().size() == 1) {
    BB *succBB = bb.GetSucc(0);
    if (!laidOut[succBB->GetBBId()]) {
      return succBB;
    }
    return NextBBProf(*succBB);
  }
  // max freq intial
  uint64 maxFreq  = 0;
  size_t idx = 0;
  bool found = false;
  for (size_t i = 0; i < bb.GetSucc().size(); ++i) {
    BB *succBB = bb.GetSucc(i);
    if (!laidOut[succBB->GetBBId()]) {
      uint64 edgeFreqFromBB = bb.GetEdgeFreq(i);
      // if bb isn't executed, choose the first valid bb
      if (bb.GetFrequency() == 0) {
        idx = i;
        found = true;
        break;
      }
      if (edgeFreqFromBB > maxFreq) {
        maxFreq = edgeFreqFromBB;
        idx = i;
        found = true;
      }
    }
  }
  if (found) {
    return bb.GetSucc(idx);
  }
  return GetBBFromEdges();
}

void BBLayout::LayoutWithProf() {
  OptimiseCFG();
  BuildEdges();
  BB *bb = func.GetFirstBB();
  while (bb != nullptr) {
    AddBBProf(*bb);
    bb = NextBBProf(*bb);
  }
  // adjust the last BB if kind is fallthru or condtion BB
  BB *lastBB = layoutBBs.empty() ? nullptr : layoutBBs.back();
  if (lastBB != nullptr) {
    if (lastBB->GetKind() == kBBFallthru) {
      BB *targetBB = lastBB->GetSucc().front();
      CreateGoto(*lastBB, func, *targetBB);
    } else if (lastBB->GetKind() == kBBCondGoto) {
      BB *fallthru = lastBB->GetSucc(0);
      CreateGotoBBAfterCondBB(*lastBB, *fallthru);
    }
  }
}

void BBLayout::RunLayout() {
  if (profValid) {
    LayoutWithProf();
  } else {
    LayoutWithoutProf();
  }
}

AnalysisResult *MeDoBBLayout::Run(MeFunction *func, MeFuncResultMgr *funcResMgr, ModuleResultMgr*) {
  // mempool used in analysisresult
  MemPool *layoutMp = NewMemPool();
  auto *bbLayout = layoutMp->New<BBLayout>(*layoutMp, *func, DEBUGFUNC(func));
  // assume common_entry_bb is always bb 0
  ASSERT(func->front() == func->GetCommonEntryBB(), "assume bb[0] is the commont entry bb");
  if (DEBUGFUNC(func)) {
    func->GetTheCfg()->DumpToFile("beforeBBLayout", false);
  }
  bbLayout->RunLayout();
  func->SetLaidOutBBs(bbLayout->GetBBs());
  funcResMgr->InvalidAnalysisResult(MeFuncPhase_DOMINANCE, func);
  if (DEBUGFUNC(func)) {
    bbLayout->DumpBBPhyOrder();
    func->GetTheCfg()->DumpToFile("afterBBLayout", false);
  }
  return nullptr;
}
}  // namespace maple
