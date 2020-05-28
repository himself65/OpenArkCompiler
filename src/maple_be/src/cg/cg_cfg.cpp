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
#include "cg_cfg.h"
#if TARGAARCH64
#include "aarch64_insn.h"
#endif
#if TARGARM32
#include "arm32_insn.h"
#endif
#include "cg_option.h"
#include "mpl_logging.h"

namespace {
using namespace maplebe;
bool CanBBThrow(const BB &bb) {
  FOR_BB_INSNS_CONST(insn, &bb) {
    if (insn->CanThrow()) {
      return true;
    }
  }
  return false;
}
}

namespace maplebe{
void CGCFG::BuildCFG() {
  /*
   * Second Pass:
   * Link preds/succs in the BBs
   */
  BB *firstBB = cgFunc->GetFirstBB();
  EHFunc *ehFunc = cgFunc->GetEHFunc();
  for (BB *curBB = firstBB; curBB != nullptr; curBB = curBB->GetNext()) {
    BB::BBKind kind = curBB->GetKind();
    switch (kind) {
      case BB::kBBIntrinsic:
        /*
         * An intrinsic BB append a MOP_wcbnz instruction at the end, check
         * AArch64CGFunc::SelectIntrinCall(IntrinsiccallNode *intrinsiccallNode) for details
         */
        if (!curBB->GetLastInsn()->IsBranch()) {
          break;
        }
      /* else fall through */
      [[clang::fallthrough]];
      case BB::kBBIf: {
        BB *fallthruBB = curBB->GetNext();
        curBB->PushBackSuccs(*fallthruBB);
        fallthruBB->PushBackPreds(*curBB);
        Insn *branchInsn = curBB->GetLastMachineInsn();
        CHECK_FATAL(branchInsn != nullptr, "machine instruction must be exist in ifBB");
        ASSERT(branchInsn->IsCondBranch(), "must be a conditional branch generated from an intrinsic");
        /* Assume the last non-null operand is the branch target */
        int lastOpndIndex = curBB->GetLastInsn()->GetOperandSize() - 1;
        Operand &lastOpnd = branchInsn->GetOperand(lastOpndIndex);
        ASSERT(lastOpnd.IsLabelOpnd(), "label Operand must be exist in branch insn");
        auto &labelOpnd = static_cast<LabelOperand&>(lastOpnd);
        BB *brToBB = cgFunc->GetBBFromLab2BBMap(labelOpnd.GetLabelIndex());
        curBB->PushBackSuccs(*brToBB);
        brToBB->PushBackPreds(*curBB);
        break;
      }
      case BB::kBBGoto: {
        Insn *insn = curBB->GetLastMachineInsn();
        CHECK_FATAL(insn != nullptr, "machine insn must be exist in gotoBB");
        ASSERT(insn->IsUnCondBranch(), "insn must be a unconditional branch insn");
        LabelIdx labelIdx = static_cast<LabelOperand&>(insn->GetOperand(0)).GetLabelIndex();
        BB *gotoBB = cgFunc->GetBBFromLab2BBMap(labelIdx);
        CHECK_FATAL(gotoBB != nullptr, "gotoBB is null");
        curBB->PushBackSuccs(*gotoBB);
        gotoBB->PushBackPreds(*curBB);
        break;
      }
      case BB::kBBRangeGoto: {
        for (auto labelIdx : curBB->GetRangeGotoLabelVec()) {
          BB *gotoBB = cgFunc->GetBBFromLab2BBMap(labelIdx);
          curBB->PushBackSuccs(*gotoBB);
          gotoBB->PushBackPreds(*curBB);
        }
        break;
      }
      case BB::kBBThrow:
        break;
      case BB::kBBFallthru: {
        BB *fallthruBB = curBB->GetNext();
        if (fallthruBB != nullptr) {
          curBB->PushBackSuccs(*fallthruBB);
          fallthruBB->PushBackPreds(*curBB);
        }
        break;
      }
      default:
        break;
    }  /* end switch */

    /* Check exception table. If curBB is in a try block, add catch BB to its succs */
    if (ehFunc != nullptr && ehFunc->GetLSDACallSiteTable() != nullptr) {
      /* Determine if insn in bb can actually except */
      if (CanBBThrow(*curBB)) {
        const MapleVector<LSDACallSite*> &callsiteTable = ehFunc->GetLSDACallSiteTable()->GetCallSiteTable();
        for (size_t i = 0; i < callsiteTable.size(); ++i) {
          LSDACallSite *lsdaCallsite = callsiteTable[i];
          BB *endTry = cgFunc->GetBBFromLab2BBMap(lsdaCallsite->csLength.GetEndOffset()->GetLabelIdx());
          BB *startTry = cgFunc->GetBBFromLab2BBMap(lsdaCallsite->csLength.GetStartOffset()->GetLabelIdx());
          if (curBB->GetId() >= startTry->GetId() && curBB->GetId() <= endTry->GetId() &&
              lsdaCallsite->csLandingPad.GetEndOffset() != nullptr) {
            BB *landingPad = cgFunc->GetBBFromLab2BBMap(lsdaCallsite->csLandingPad.GetEndOffset()->GetLabelIdx());
            curBB->PushBackEhSuccs(*landingPad);
            landingPad->PushBackEhPreds(*curBB);
          }
        }
      }
    }
  }
}
/*
 * return true if:
 * mergee has only one predecessor which is merger, or mergee has
 * other comments only predecessors.
 * mergee can't have cfi instruction when postcfgo.
 */
bool CGCFG::BBJudge(const BB &first, const BB &second) const {
  if (first.GetKind() == BB::kBBReturn || second.GetKind() == BB::kBBReturn) {
    return false;
  }
  if (second.GetPreds().size() == 1 && second.GetPreds().front() == &first) {
    return true;
  }
  for (BB *bb : second.GetPreds()) {
    if (bb != &first && !AreCommentAllPreds(*bb)) {
      return false;
    }
  }
  return true;
}

/*
 * Check if a given BB mergee can be merged into BB merger.
 * Returns true if:
 * 1. mergee has only one predecessor which is merger, or mergee has
 *   other comments only predecessors.
 * 2. merge has only one successor which is mergee.
 * 3. mergee can't have cfi instruction when postcfgo.
 */
bool CGCFG::CanMerge(const BB &merger, const BB &mergee) const {
  if (!BBJudge(merger, mergee)) {
    return false;
  }
  if (mergee.GetFirstInsn() != nullptr && mergee.GetFirstInsn()->IsCfiInsn()) {
    return false;
  }
  return (merger.GetSuccs().size() == 1) && (merger.GetSuccs().front() == &mergee);
}

/* Check if the given BB contains only comments and all its predecessors are comments */
bool CGCFG::AreCommentAllPreds(const BB &bb) {
  if (!bb.IsCommentBB()) {
    return false;
  }
  for (BB *pred : bb.GetPreds()) {
    if (!AreCommentAllPreds(*pred)) {
      return false;
    }
  }
  return true;
}

/* Merge sucBB into curBB. */
void CGCFG::MergeBB(BB &merger, BB &mergee, CGFunc &func) {
  MergeBB(merger, mergee);
  if (mergee.GetKind() == BB::kBBReturn) {
    for (size_t i = 0; i < func.ExitBBsVecSize(); ++i) {
      if (func.GetExitBB(i) == &mergee) {
        func.EraseExitBBsVec(func.GetExitBBsVec().begin() + i);
      }
    }
    func.PushBackExitBBsVec(merger);
  }
}

void CGCFG::MergeBB(BB &merger, BB &mergee) {
  if (merger.GetKind() == BB::kBBGoto) {
    if (!merger.GetLastInsn()->IsBranch()) {
      CHECK_FATAL(false, "unexpected insn kind");
    }
    merger.RemoveInsn(*merger.GetLastInsn());
  }
  merger.AppendBBInsns(mergee);
  if (mergee.GetPrev() != nullptr) {
    mergee.GetPrev()->SetNext(mergee.GetNext());
  }
  if (mergee.GetNext() != nullptr) {
    mergee.GetNext()->SetPrev(mergee.GetPrev());
  }
  merger.RemoveSuccs(mergee);
  if (!merger.GetEhSuccs().empty()) {
#if DEBUG
    for (BB *bb : merger.GetEhSuccs()) {
      ASSERT((bb != &mergee), "CGCFG::MergeBB: Merging of EH bb");
    }
#endif
  }
  if (!mergee.GetEhSuccs().empty()) {
    for (BB *bb : mergee.GetEhSuccs()) {
      bb->RemoveEhPreds(mergee);
      bb->PushBackEhPreds(merger);
      merger.PushBackEhSuccs(*bb);
    }
  }
  for (BB *bb : mergee.GetSuccs()) {
    bb->RemovePreds(mergee);
    bb->PushBackPreds(merger);
    merger.PushBackSuccs(*bb);
  }
  merger.SetKind(mergee.GetKind());
  mergee.SetNext(nullptr);
  mergee.SetPrev(nullptr);
  mergee.ClearPreds();
  mergee.ClearSuccs();
  mergee.ClearEhPreds();
  mergee.ClearEhSuccs();
  mergee.SetFirstInsn(nullptr);
  mergee.SetLastInsn(nullptr);
}

/*
 * Find all reachable BBs by dfs in cgfunc and mark their field<unreachable> false, then all other bbs should be
 * unreachable.
 */
void CGCFG::FindAndMarkUnreachable(CGFunc &func) {
  BB *firstBB = func.GetFirstBB();
  std::stack<BB*> toBeAnalyzedBBs;
  toBeAnalyzedBBs.push(firstBB);
  std::set<BB*, BBIdCmp> instackBBs;

  BB *bb = firstBB;
  /* set all bb's unreacable to true */
  while (bb != nullptr) {
    /* Check if bb is the first or the last BB of the function */
    if (bb->GetFirstStmt() == func.GetCleanupLabel() || InSwitchTable(bb->GetLabIdx(), func) ||
        bb == func.GetFirstBB() || bb == func.GetLastBB()) {
      toBeAnalyzedBBs.push(bb);
    } else {
      bb->SetUnreachable(true);
    }
    bb = bb->GetNext();
  }

  /* do a dfs to see which bbs are reachable */
  while (!toBeAnalyzedBBs.empty()) {
    bb = toBeAnalyzedBBs.top();
    toBeAnalyzedBBs.pop();
    instackBBs.insert(bb);

    bb->SetUnreachable(false);

    for (BB *succBB : bb->GetSuccs()) {
      if (instackBBs.count(succBB) == 0) {
        toBeAnalyzedBBs.push(succBB);
        instackBBs.insert(succBB);
      }
    }
    for (BB *succBB : bb->GetEhSuccs()) {
      if (instackBBs.count(succBB) == 0) {
        toBeAnalyzedBBs.push(succBB);
        instackBBs.insert(succBB);
      }
    }
  }
}

/*
 * Theoretically, every time you remove from a bb's preds, you should consider invoking this method.
 *
 * @param bb
 * @param func
 */
void CGCFG::FlushUnReachableStatusAndRemoveRelations(BB &bb, const CGFunc &func) const {
  /* Check if bb is the first or the last BB of the function */
  bool isFirstBBInfunc = (&bb == func.GetFirstBB());
  bool isLastBBInfunc = (&bb == func.GetLastBB());
  if (bb.GetFirstStmt() == func.GetCleanupLabel() || InSwitchTable(bb.GetLabIdx(), func) || isFirstBBInfunc ||
      isLastBBInfunc) {
    return;
  }
  std::stack<BB*> toBeAnalyzedBBs;
  toBeAnalyzedBBs.push(&bb);
  std::set<uint32> instackBBs;
  BB *it = nullptr;
  while (!toBeAnalyzedBBs.empty()) {
    it = toBeAnalyzedBBs.top();
    instackBBs.insert(it->GetId());
    toBeAnalyzedBBs.pop();
    /* Check if bb is the first or the last BB of the function */
    isFirstBBInfunc = (it == func.GetFirstBB());
    isLastBBInfunc = (it == func.GetLastBB());
    bool needFlush = !isFirstBBInfunc && !isLastBBInfunc &&
                     it->GetFirstStmt() != func.GetCleanupLabel() &&
                     (it->GetPreds().empty() || (it->GetPreds().size() == 1 && it->GetEhPreds().front() == it)) &&
                     it->GetEhPreds().empty() &&
                     !InSwitchTable(it->GetLabIdx(), *cgFunc) &&
                     !cgFunc->IsExitBB(*it);
    if (!needFlush) {
      continue;
    }
    it->SetUnreachable(true);
    it->SetFirstInsn(nullptr);
    it->SetLastInsn(nullptr);
    for (BB *succ : it->GetSuccs()) {
      if (instackBBs.count(succ->GetId()) == 0) {
        toBeAnalyzedBBs.push(succ);
        instackBBs.insert(succ->GetId());
      }
      succ->RemovePreds(*it);
      succ->RemoveEhPreds(*it);
    }
    it->ClearSuccs();
    for (BB *succ : it->GetEhSuccs()) {
      if (instackBBs.count(succ->GetId()) == 0) {
        toBeAnalyzedBBs.push(succ);
        instackBBs.insert(succ->GetId());
      }
      succ->RemoveEhPreds(*it);
      succ->RemovePreds(*it);
    }
    it->ClearEhSuccs();
  }
}
bool CGCFG::InLSDA(LabelIdx label, const EHFunc &ehFunc) {
  if (!label || ehFunc.GetLSDACallSiteTable() == nullptr) {
    return false;
  }
  if (label == ehFunc.GetLSDACallSiteTable()->GetCSTable().GetEndOffset()->GetLabelIdx() ||
      label == ehFunc.GetLSDACallSiteTable()->GetCSTable().GetStartOffset()->GetLabelIdx()) {
    return true;
  }
  return ehFunc.GetLSDACallSiteTable()->InCallSiteTable(label);
}

bool CGCFG::InSwitchTable(LabelIdx label, const CGFunc &func) {
  if (!label) {
    return false;
  }
  for (auto *st : func.GetEmitStVec()) {
    CHECK_FATAL(st->GetKonst()->GetKind() == kConstAggConst, "not a kConstAggConst");
    MIRAggConst *arrayConst = safe_cast<MIRAggConst>(st->GetKonst());
    for (size_t i = 0; i < arrayConst->GetConstVec().size(); ++i) {
      CHECK_FATAL(arrayConst->GetConstVecItem(i)->GetKind() == kConstLblConst, "not a kConstLblConst");
      MIRLblConst *lblConst = safe_cast<MIRLblConst>(arrayConst->GetConstVecItem(i));
      if (label == lblConst->GetValue()) {
        return true;
      }
    }
  }
  return false;
}
Insn *CGCFG::FindLastCondBrInsn(BB &bb) const {
  if (bb.GetKind() != BB::kBBIf) {
    return nullptr;
  }
  FOR_BB_INSNS_REV(insn, (&bb)) {
    if (insn->IsBranch()) {
      return insn;
    }
  }
  return nullptr;
}

/*
 * analyse the CFG to find the BBs that are not reachable from function entries
 * and delete them
 */
void CGCFG::UnreachCodeAnalysis() {
  /*
   * Find all reachable BBs by dfs in cgfunc and mark their field<unreachable> false,
   * then all other bbs should be unreachable.
   */
  BB *firstBB = cgFunc->GetFirstBB();
  std::forward_list<BB*> toBeAnalyzedBBs;
  toBeAnalyzedBBs.push_front(firstBB);
  std::set<BB*, BBIdCmp> unreachBBs;

  BB *bb = firstBB;
  /* set all bb's unreacable to true */
  while (bb != nullptr) {
    /* Check if bb is the first or the last BB of the function */
    if (bb->GetFirstStmt() == cgFunc->GetCleanupLabel() || InSwitchTable(bb->GetLabIdx(), *cgFunc) ||
        bb == cgFunc->GetFirstBB() || bb == cgFunc->GetLastBB() || bb->GetKind() == BB::kBBReturn) {
      toBeAnalyzedBBs.push_front(bb);
    } else {
      unreachBBs.insert(bb);
    }
    bb->SetUnreachable(true);
    bb = bb->GetNext();
  }

  /* do a dfs to see which bbs are reachable */
  while (!toBeAnalyzedBBs.empty()) {
    bb = toBeAnalyzedBBs.front();
    toBeAnalyzedBBs.pop_front();
    if (!bb->IsUnreachable()) {
      continue;
    }
    bb->SetUnreachable(false);
    for (BB *succBB : bb->GetSuccs()) {
      toBeAnalyzedBBs.push_front(succBB);
      unreachBBs.erase(succBB);
    }
    for (BB *succBB : bb->GetEhSuccs()) {
      toBeAnalyzedBBs.push_front(succBB);
      unreachBBs.erase(succBB);
    }
  }
  /* Don't remove unreach code if withDwarf is enabled. */
  if (CGOptions::IsWithDwarf()) {
    return;
  }
  /* remove unreachable bb */
  std::set<BB*, BBIdCmp>::iterator it;
  for (it = unreachBBs.begin(); it != unreachBBs.end(); it++) {
    BB *unreachBB = *it;
    ASSERT(unreachBB != nullptr, "unreachBB must not be nullptr");
    if (cgFunc->IsExitBB(*unreachBB)) {
      unreachBB->SetUnreachable(false);
    }
    EHFunc* ehFunc = cgFunc->GetEHFunc();
    ASSERT(ehFunc != nullptr, "get ehfunc in cgfunc failed");
    /* if unreachBB InLSDA ,replace unreachBB's label with nextReachableBB before remove it. */
    if (ehFunc->NeedFullLSDA() && cgFunc->GetTheCFG()->InLSDA(unreachBB->GetLabIdx(), *ehFunc)) {
      /* find next reachable BB */
      BB* nextReachableBB = nullptr;
      for (BB* curBB = unreachBB; curBB != nullptr; curBB = curBB->GetNext()) {
        if (!curBB->IsUnreachable()) {
          nextReachableBB = curBB;
          break;
        }
      }
      CHECK_FATAL(nextReachableBB != nullptr, "nextReachableBB not be nullptr");
      if (nextReachableBB->GetLabIdx() == 0) {
        LabelIdx labelIdx = cgFunc->CreateLabel();
        nextReachableBB->AddLabel(labelIdx);
        cgFunc->SetLab2BBMap(labelIdx, *nextReachableBB);
      }

      ehFunc->GetLSDACallSiteTable()->UpdateCallSite(*unreachBB, *nextReachableBB);
    }

    unreachBB->GetPrev()->SetNext(unreachBB->GetNext());
    unreachBB->GetNext()->SetPrev(unreachBB->GetPrev());

    for (BB *sucBB : unreachBB->GetSuccs()) {
      sucBB->RemovePreds(*unreachBB);
    }
    for (BB *ehSucBB : unreachBB->GetEhSuccs()) {
      ehSucBB->RemoveEhPreds(*unreachBB);
    }

    unreachBB->ClearSuccs();
    unreachBB->ClearEhSuccs();

    /* Clear insns in GOT Map. */
    cgFunc->ClearUnreachableGotInfos(*unreachBB);
    cgFunc->ClearUnreachableConstInfos(*unreachBB);
  }
}

BB *CGCFG::FindLastRetBB() {
  FOR_ALL_BB_REV(bb, cgFunc) {
    if (bb->GetKind() == BB::kBBReturn) {
      return bb;
    }
  }
  return nullptr;
}
}  /* namespace maplebe */
