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
#include "try_catch.h"
namespace maplebe {
BBT *TryCatchBlocksLower::CreateNewBB(StmtNode *first, StmtNode *last) {
  BBT *newBB = memPool.New<BBT>(first, last, &memPool);
  bbList.emplace_back(newBB);
  return newBB;
}

BBT *TryCatchBlocksLower::FindTargetBBlock(LabelIdx idx, const std::vector<BBT*> &bbs) {
  for (auto &target : bbs) {
    if (target->GetLabelIdx() == idx) {
      return target;
    }
  }
  return nullptr;
}

/* returns the first statement that is moved in into the try block. If none is moved in, nullptr is returned */
StmtNode *TryCatchBlocksLower::MoveCondGotoIntoTry(BBT &jtBB, BBT &condbrBB, const MapleVector<BBT*> &labeledBBsInTry) {
  StmtNode *firstStmtMovedIn = nullptr;
  const MapleVector<BBT*> &bbs = labeledBBsInTry;
  StmtNode *jtStmt = jtBB.GetKeyStmt();
#if DEBUG
  StmtNode *js = jtBB.GetFirstStmt();
  while (js->GetOpCode() != OP_try) {
    js = js->GetNext();
  }
  CHECK_FATAL(js == jtStmt, "make sure js equal jtStmt");
#endif
  StmtNode *ts = jtBB.GetFirstStmt()->GetPrev();
  while ((ts != nullptr) && (ts->GetOpCode() == OP_comment)) {
    ts = ts->GetPrev();
  }

  if (ts != nullptr && ts->IsCondBr()) {
    CHECK_FATAL(ts->GetNext() == jtBB.GetFirstStmt(), "make sure ts's next equal jtBB's firstStmt");
    StmtNode *firstStmtNode = jtBB.GetFirstStmt();
    /* [ jtbb_b..jtstmt ]; either jtbb_b is a comment or jtbb_b == jtstmt */
    LabelIdx id = static_cast<CondGotoNode*>(ts)->GetOffset();
    for (auto &lbb : bbs) {
      if (lbb->GetLabelIdx() == id) {
        /*
         * this cond goto jumps into the try block; let the try block enclose it
         * first find the preceding comment statements if any
         */
        StmtNode *brS = ts;
        while ((ts->GetPrev() != nullptr) && (ts->GetPrev()->GetOpCode() == OP_comment)) {
          ts = ts->GetPrev();
        }
        StmtNode *secondStmtNode = ts;  /* beginning statement of branch block */
        /* [ brbb_b..br_s ]; either brbb_b is a comment or brbb_b == br_s */
        firstStmtNode->SetPrev(secondStmtNode->GetPrev());
        if (secondStmtNode->GetPrev()) {
          secondStmtNode->GetPrev()->SetNext(firstStmtNode);
        }
        jtStmt->GetNext()->SetPrev(brS);
        brS->SetNext(jtStmt->GetNext());
        secondStmtNode->SetPrev(jtStmt);
        jtStmt->SetNext(secondStmtNode);
        condbrBB.SetLastStmt(*firstStmtNode->GetPrev());
        CHECK_FATAL(condbrBB.GetFallthruBranch() == &jtBB, "make sure condbrBB's fallthruBranch equal &jtBB");
        condbrBB.SetFallthruBranch(&jtBB);
        condbrBB.SetCondJumpBranch(nullptr);
        firstStmtMovedIn = secondStmtNode;
        break;
      }
    }
  }
  return firstStmtMovedIn;
}

void TryCatchBlocksLower::RecoverBasicBlock() {
  std::vector<BBT*> condbrBBs;
  std::vector<BBT*> switchBBs;
  std::vector<BBT*> labeledBBs;
  using BBTPair = std::pair<BBT*, BBT*>;
  std::vector<BBTPair> tryBBs;
  std::vector<BBT*> catchBBs;

  CHECK_FATAL(body.GetFirst() != nullptr, "body should not be NULL");
  bodyFirst = body.GetFirst();
  StmtNode *next = bodyFirst;
  /*
   * comment block [ begin, end ], We treat comment statements as if they are parts
   * of the immediately following non-comment statement
   */
  StmtNode *commentB = nullptr;
  StmtNode *commentE = nullptr;

  BBT *curBB = nullptr;
  BBT *lastBB = nullptr;
  BBT *openTry = nullptr;

  /* recover basic blocks */
  for (StmtNode *stmt = next; stmt != nullptr; stmt = next) {
    next = stmt->GetNext();

    if (stmt->GetOpCode() == OP_comment) {
      if (commentB == nullptr) {
        commentB = stmt;
        commentE = stmt;
      } else {
        CHECK_FATAL(commentE != nullptr, "nullptr is not expected");
        CHECK_FATAL(commentE->GetNext() == stmt, "make sure commentE's next is stmt");
        commentE = stmt;
      }
      continue;
    }

    CHECK_FATAL(stmt->GetOpCode() != OP_comment, "make sure stmt's opcde not equal OP_comment");
    CHECK_FATAL(commentB == nullptr || (commentE != nullptr && commentE->GetNext() == stmt),
                "make sure commentB is nullptr or commentE's next is stmt");

    if (curBB != nullptr) {
      if (stmt->GetOpCode() != OP_label && stmt->GetOpCode() != OP_try && stmt->GetOpCode() != OP_endtry) {
        curBB->Extend(commentB, stmt);
      } else {
        /* java catch blockes always start with a label (i.e., OP_catch) */
        CHECK_FATAL(curBB->GetCondJumpBranch() == nullptr, "expect curBB's condJumpBranch is nullptr");
        CHECK_FATAL(curBB->GetFallthruBranch() == nullptr, "expect curBB's fallthruBranch is nullptr");
        /* a 'label' statement starts a new basic block */
        BBT *newBB = CreateNewBB(commentB, stmt);
        /*
         * if the immediately preceding statement (discounting comments) was throw, goto or return,
         * curBB is to be reset to nullptr, so the control  won't come here.
         */
        curBB->SetFallthruBranch(newBB);
        curBB = newBB;
      }
    } else {
      /* start a new basic block with 'comment_b -- stmt' */
      curBB = CreateNewBB(commentB, stmt);
      if (lastBB != nullptr) {
        Opcode lastBBLastStmtOp = lastBB->GetLastStmt()->GetOpCode();
        if (lastBB->GetLastStmt()->IsCondBr() || lastBBLastStmtOp == OP_endtry) {
          lastBB->SetFallthruBranch(curBB);
        }
        /* else don't connect curBB to last_bb */
      }
    }
    commentB = nullptr;
    commentE = nullptr;

    switch (stmt->GetOpCode()) {
      case OP_throw:
      case OP_return:
      case OP_goto:
        /* start a new bb at the next stmt */
        lastBB = curBB;
        curBB = nullptr;
        break;
      case OP_label: {
        LabelNode *labelStmt = static_cast<LabelNode*>(stmt);
        labeledBBs.emplace_back(curBB);
        curBB->SetLabelIdx((LabelIdx)labelStmt->GetLabelIdx());
      } break;
      case OP_brtrue:
      case OP_brfalse:
        condbrBBs.emplace_back(curBB);
        lastBB = curBB;
        curBB = nullptr;
        break;
      case OP_switch:
        switchBBs.emplace_back(curBB);
        lastBB = curBB;
        curBB = nullptr;
        break;
      /*
       * We deal try and endtry slightly differently.
       * 1. try begins a basic block which includes the try statement and the subsequent statements up to one that
       *    results in non-sequential control transfer such as unconditional/conditional branches.
       * 2. endtry will create its own basic block which contains the endtry statement and nothing else.
       */
      case OP_try:
      case OP_endtry: {
        /* because a label statement is inserted at the function entry */
        CHECK_FATAL(curBB != nullptr, "expect curBB is not nullptr");
        CHECK_FATAL(curBB->GetCondJumpBranch() == nullptr, "expect curBB's condJumpBranch is nullptr");
        CHECK_FATAL(curBB->GetFallthruBranch() == nullptr, "expect curBB's fallthruBranch is nullptr");
        CHECK_FATAL(curBB->GetLastStmt()->GetOpCode() == stmt->GetOpCode(),
                    "the opcode of curBB's lastStmt should equal stmt's opcocde");
        if (stmt->GetOpCode() == OP_try) {
          CHECK_FATAL(openTry == nullptr, "trys are not expected to be nested");
          curBB->SetType(BBT::kBBTry, *stmt);
          openTry = curBB;
          prevBBOfTry[openTry] = lastBB;
        } else {
          tryBBs.emplace_back(BBTPair(openTry, curBB));
          openTry = nullptr;
          curBB->SetType(BBT::kBBEndTry, *stmt);
          lastBB = curBB;
          curBB = nullptr;
        }
        break;
      }
      case OP_catch: {
#if DEBUG
        StmtNode *ss = stmt->GetPrev();
        while ((ss != nullptr) && (ss->GetOpCode() == OP_comment)) {
          ss = ss->GetPrev();
        }
        CHECK_FATAL(ss != nullptr, "expect ss is not nullptr");
        CHECK_FATAL(ss->GetOpCode() == OP_label, "expect op equal OP_label");
        for (auto &tb : catchBBs) {
          CHECK_FATAL(tb != curBB, "tb should not equal curBB");
        }
#endif
        catchBBs.emplace_back(curBB);
        curBB->SetType(BBT::kBBCatch, *stmt);
        break;
      }
      case OP_block:
        CHECK_FATAL(0, "should not run here");
      default:
        break;
    }
  }

  for (auto &cbBB : condbrBBs) {
    CHECK_FATAL(cbBB->GetLastStmt()->IsCondBr(), "cbBB's lastStmt is not condBr");
    CondGotoNode *s = static_cast<CondGotoNode*>(cbBB->GetLastStmt());
    cbBB->SetCondJumpBranch(FindTargetBBlock((LabelIdx)s->GetOffset(), labeledBBs));
  }

  for (auto &swBB : switchBBs) {
    CHECK_FATAL(swBB->GetLastStmt()->GetOpCode() == OP_switch, "the opcode of sw's lastStmt should equal OP_switch");
    SwitchNode *ss = static_cast<SwitchNode*>(swBB->GetLastStmt());

    swBB->AddSuccs(FindTargetBBlock(ss->GetDefaultLabel(), labeledBBs));
    for (auto &cp : ss->GetSwitchTable()) {
      swBB->AddSuccs(FindTargetBBlock(cp.second, labeledBBs));
    }
  }

  for (auto &bb : bbList) {
    firstStmtToBBMap[bb->GetFirstStmt()] = bb;
  }
  CHECK_FATAL(openTry == nullptr, "trys are not expected to be nested");
}

/* if catchBB is in try-endtry block and catch is own to current try-endtry, process it and return true */
bool TryCatchBlocksLower::CheckAndProcessCatchNodeInCurrTryBlock(BBT &origLowerBB, LabelIdx ebbLabel,
                                                                 uint32 index) {
  MapleVector<BBT*> &enclosedBBs = tryEndTryBlock.GetEnclosedBBs();
  MapleVector<BBT*> &bbsToRelocate = tryEndTryBlock.GetBBsToRelocate();
  BBT *endTryBB = tryEndTryBlock.GetEndTryBB();
  StmtNode *tryStmt = tryEndTryBlock.GetTryStmtNode();
  bool found = false;
  for (size_t tempIndex = 0; tempIndex < static_cast<TryNode*>(tryStmt)->GetOffsetsCount(); ++tempIndex) {
    auto id = static_cast<TryNode*>(tryStmt)->GetOffset(tempIndex);
    /*
     * if this labeled bb is a catch block,
     * remove it from the list of blocks enclosed in this try-block'
     */
    if (ebbLabel == id) {
      found = true;
      enclosedBBs[index] = nullptr;
      std::vector<BBT*> currBBThread;
      BBT *lowerBB = &origLowerBB;
      /* append it to the list of blocks placed after the end try block */
      currBBThread.emplace_back(lowerBB);
      while (lowerBB->GetFallthruBranch() != nullptr) {
        lowerBB = lowerBB->GetFallthruBranch();
        CHECK_FATAL(!lowerBB->IsTry(), "ebb must not be tryBB");
        if (lowerBB->IsEndTry()) {
          CHECK_FATAL(lowerBB == endTryBB, "lowerBB should equal endTryBB");
          break;
        }
        for (uint32 j = 0; j < enclosedBBs.size(); ++j) {
          if (enclosedBBs[j] == lowerBB) {
            enclosedBBs[j] = nullptr;
            break;
          }
        }
        currBBThread.emplace_back(lowerBB);
      }

      if (!lowerBB->IsEndTry()) {
        for (auto &e : currBBThread) {
          bbsToRelocate.emplace_back(e);
        }
      } else {
        /*
         * We have the following case.
         * bb_head -> bb_1 -> .. bb_n -> endtry_bb -> succ
         * For this particular case, we swap endtry bb and curr_bb_thread because the bblock that
         * contains the endtry statement does not contain any other statements!!
         */
        CHECK_FATAL(endTryBB->GetFirstStmt()->GetOpCode() == OP_comment ||
                    endTryBB->GetFirstStmt()->GetOpCode() == OP_endtry,
                    "the opcode of endTryBB's firstStmt should be OP_comment or OP_endtry");
        CHECK_FATAL(endTryBB->GetLastStmt()->GetOpCode() == OP_endtry,
                    "the opcode of endTryBB's lastStmt should be OP_endtry");

        /* we move endtry_bb before thread_head */
        BBT *threadHead = currBBThread.front();
        CHECK_FATAL(threadHead->GetFirstStmt()->GetPrev() != nullptr,
                    "the prev node of threadHead's firstStmt should be not nullptr");
        CHECK_FATAL(threadHead->GetFirstStmt()->GetOpCode() == OP_comment ||
                    threadHead->GetFirstStmt()->GetOpCode() == OP_label,
                    "the opcode of threadHead's firstStmt should be OP_comment or OP_label");
        CHECK_FATAL(threadHead->GetFirstStmt()->GetPrev()->GetNext() == threadHead->GetFirstStmt(),
                    "the next of the prev of threadHead's firstStmt should equal threadHead's firstStmt");
        threadHead->GetFirstStmt()->GetPrev()->SetNext(endTryBB->GetFirstStmt());
        endTryBB->GetFirstStmt()->SetPrev(threadHead->GetFirstStmt()->GetPrev());
        BBT *threadTail = currBBThread.back();
        threadTail->GetLastStmt()->SetNext(endTryBB->GetLastStmt()->GetNext());
        if (endTryBB->GetLastStmt()->GetNext() != nullptr) {
          endTryBB->GetLastStmt()->GetNext()->SetPrev(threadTail->GetLastStmt());
        }
        endTryBB->GetLastStmt()->SetNext(threadHead->GetFirstStmt());

        CHECK_FATAL(endTryBB->GetCondJumpBranch() == nullptr, "endTryBB's condJumpBranch must be nullptr");
        if (threadTail->GetFallthruBranch() != nullptr) {
          threadTail->SetFallthruBranch(firstStmtToBBMap[threadTail->GetLastStmt()->GetNext()]);
        }
        endTryBB->SetFallthruBranch(nullptr);
        if (bodyEndWithEndTry) {
          body.SetLast(threadTail->GetLastStmt());
        }
      }
      break;
    }
  }
  return found;
}

/* collect catchbb->fallthru(0-n) into currBBThread, when encounter a new catch, return it, else return nullptr */
BBT *TryCatchBlocksLower::CollectCatchAndFallthruUntilNextCatchBB(BBT *&lowerBB, uint32 &nextEnclosedIdx,
                                                                  std::vector<BBT*> &currBBThread) {
  MapleVector<BBT*> &enclosedBBs = tryEndTryBlock.GetEnclosedBBs();
  BBT *endTryBB = tryEndTryBlock.GetEndTryBB();

  BBT *nextBBThreadHead = nullptr;
  while (lowerBB->GetFallthruBranch() != nullptr) {
    lowerBB = lowerBB->GetFallthruBranch();
    ++nextEnclosedIdx;
    if (lowerBB->IsEndTry()) {
      CHECK_FATAL(lowerBB == endTryBB, "lowerBB should equal endTryBB");
      break;
    }

    for (uint32 j = 0; j < enclosedBBs.size(); ++j) {
      if (enclosedBBs[j] == lowerBB) {
        enclosedBBs[j] = nullptr;
        break;
      }
    }
    if (lowerBB->IsCatch()) {
      nextBBThreadHead = lowerBB;
      break;
    }
    currBBThread.emplace_back(lowerBB);
  }

  if (nextBBThreadHead == nullptr && lowerBB->GetFallthruBranch() == nullptr && lowerBB != endTryBB &&
      nextEnclosedIdx < enclosedBBs.size() && enclosedBBs[nextEnclosedIdx]) {
    /*
     * Using a loop to find the next_bb_thread_head when it's a catch_BB or a normal_BB which
     * is after a catch_BB. Other condition, push_back into the curr_bb_thread.
     */
    do {
      lowerBB = enclosedBBs[nextEnclosedIdx];
      enclosedBBs[nextEnclosedIdx++] = nullptr;
      BBT *head = currBBThread.front();
      if (head->IsCatch() || lowerBB->IsCatch()) {
        nextBBThreadHead = lowerBB;
        break;
      }
      currBBThread.emplace_back(lowerBB);
    } while (nextEnclosedIdx < enclosedBBs.size());
  }

  return nextBBThreadHead;
}

void TryCatchBlocksLower::ProcessThreadTail(BBT &threadTail, BBT *&nextBBThreadHead, bool hasMoveEndTry) {
  BBT *endTryBB = tryEndTryBlock.GetEndTryBB();
  StmtNode *newEndTry = endTryBB->GetKeyStmt()->CloneTree(mirModule.GetCurFuncCodeMPAllocator());
  newEndTry->SetPrev(threadTail.GetLastStmt());
  newEndTry->SetNext(threadTail.GetLastStmt()->GetNext());
  if (bodyEndWithEndTry && hasMoveEndTry) {
    if (threadTail.GetLastStmt()->GetNext()) {
      threadTail.GetLastStmt()->GetNext()->SetPrev(newEndTry);
    }
  } else {
    CHECK_FATAL(threadTail.GetLastStmt()->GetNext() != nullptr,
                "the next of threadTail's lastStmt should not be nullptr");
    threadTail.GetLastStmt()->GetNext()->SetPrev(newEndTry);
  }
  threadTail.GetLastStmt()->SetNext(newEndTry);

  threadTail.SetLastStmt(*newEndTry);
  if (hasMoveEndTry && nextBBThreadHead == nullptr) {
    body.SetLast(threadTail.GetLastStmt());
  }
}

/* Wrap this catch block with try-endtry block */
void TryCatchBlocksLower::WrapCatchWithTryEndTryBlock(std::vector<BBT*> &currBBThread, BBT *&nextBBThreadHead,
                                                      uint32 &nextEnclosedIdx, bool hasMoveEndTry) {
  BBT *endTryBB = tryEndTryBlock.GetEndTryBB();
  StmtNode *tryStmt = tryEndTryBlock.GetTryStmtNode();
  MapleVector<BBT*> &enclosedBBs = tryEndTryBlock.GetEnclosedBBs();
  for (auto &e : currBBThread) {
    CHECK_FATAL(!e->IsTry(), "expect e is not try");
  }
  BBT *threadHead = currBBThread.front();
  if (threadHead->IsCatch()) {
    StmtNode *jcStmt = threadHead->GetKeyStmt();
    CHECK_FATAL(jcStmt->GetNext() != nullptr, "jcStmt's next should not be nullptr");
    TryNode *jtCopy = static_cast<TryNode*>(tryStmt)->CloneTree(mirModule.GetCurFuncCodeMPAllocator());
    jtCopy->SetNext(jcStmt->GetNext());
    jtCopy->SetPrev(jcStmt);
    jcStmt->GetNext()->SetPrev(jtCopy);
    jcStmt->SetNext(jtCopy);

    BBT *threadTail = currBBThread.back();

    /* for this endtry stmt, we don't need to create a basic block */
    ProcessThreadTail(*threadTail, nextBBThreadHead, hasMoveEndTry);
  } else {
    /* For cases try->catch->normal_bb->normal_bb->endtry, Combine normal bb first. */
    while (nextEnclosedIdx < enclosedBBs.size()) {
      if (nextBBThreadHead != nullptr) {
        if (nextBBThreadHead->IsCatch()) {
          break;
        }
      }
      BBT *ebbSecond = enclosedBBs[nextEnclosedIdx];
      enclosedBBs[nextEnclosedIdx++] = nullptr;
      CHECK_FATAL(ebbSecond != endTryBB, "ebbSecond should not equal endTryBB");
      if (ebbSecond->IsCatch()) {
        nextBBThreadHead = ebbSecond;
        break;
      }
      currBBThread.emplace_back(ebbSecond);
    }
    /* normal bb. */
    StmtNode *stmt = threadHead->GetFirstStmt();

    TryNode *jtCopy = static_cast<TryNode*>(tryStmt)->CloneTree(mirModule.GetCurFuncCodeMPAllocator());
    jtCopy->SetNext(stmt);
    jtCopy->SetPrev(stmt->GetPrev());
    stmt->GetPrev()->SetNext(jtCopy);
    stmt->SetPrev(jtCopy);
    threadHead->SetFirstStmt(*jtCopy);

    BBT *threadTail = currBBThread.back();

    /* for this endtry stmt, we don't need to create a basic block */
    ProcessThreadTail(*threadTail, nextBBThreadHead, hasMoveEndTry);
  }
}

/*
 * We have the following case.
 * bb_head -> bb_1 -> .. bb_n -> endtry_bb -> succ
 * For this particular case, we swap EndTry bb and curr_bb_thread, because the bblock that contains the endtry
 * statement does not contain any other statements!!
 */
void TryCatchBlocksLower::SwapEndTryBBAndCurrBBThread(const std::vector<BBT*> &currBBThread, bool &hasMoveEndTry,
                                                      BBT *nextBBThreadHead) {
  BBT *endTryBB = tryEndTryBlock.GetEndTryBB();
  CHECK_FATAL(endTryBB->GetFirstStmt()->GetOpCode() == OP_comment ||
              endTryBB->GetFirstStmt()->GetOpCode() == OP_endtry,
              "the opcode of endTryBB's firstStmt should be OP_comment or OP_endtry");
  CHECK_FATAL(endTryBB->GetLastStmt()->GetOpCode() == OP_endtry,
              "the opcode of endTryBB's lastStmt should be OP_endtry");

  /* we move endtry_bb before bb_head */
  BBT *threadHead = currBBThread.front();
  CHECK_FATAL(threadHead->GetFirstStmt()->GetPrev() != nullptr,
              "the prev of threadHead's firstStmt should not nullptr");
  CHECK_FATAL(threadHead->GetFirstStmt()->GetOpCode() == OP_comment ||
              threadHead->GetFirstStmt()->GetOpCode() == OP_label,
              "the opcode of threadHead's firstStmt should be OP_comment or OP_label");
  CHECK_FATAL(threadHead->GetFirstStmt()->GetPrev()->GetNext() == threadHead->GetFirstStmt(),
              "the next of the prev of threadHead's firstStmt should equal threadHead's firstStmt");

  endTryBB->GetFirstStmt()->GetPrev()->SetNext(endTryBB->GetLastStmt()->GetNext());
  if (endTryBB->GetLastStmt()->GetNext() != nullptr) {
    endTryBB->GetLastStmt()->GetNext()->SetPrev(endTryBB->GetFirstStmt()->GetPrev());
  }

  threadHead->GetFirstStmt()->GetPrev()->SetNext(endTryBB->GetFirstStmt());
  endTryBB->GetFirstStmt()->SetPrev(threadHead->GetFirstStmt()->GetPrev());

  endTryBB->GetLastStmt()->SetNext(threadHead->GetFirstStmt());
  threadHead->GetFirstStmt()->SetPrev(endTryBB->GetLastStmt());

  CHECK_FATAL(endTryBB->GetCondJumpBranch() == nullptr, "endTryBB's condJumpBranch must be nullptr");
  endTryBB->SetFallthruBranch(nullptr);
  if (bodyEndWithEndTry) {
    hasMoveEndTry = true;
    if (nextBBThreadHead == nullptr) {
      body.SetLast(currBBThread.back()->GetLastStmt());
    }
  }
}

void TryCatchBlocksLower::ProcessEnclosedBBBetweenTryEndTry() {
  MapleVector<BBT*> &enclosedBBs = tryEndTryBlock.GetEnclosedBBs();
  MapleVector<BBT*> &labeledBBsInTry = tryEndTryBlock.GetLabeledBBsInTry();

  for (uint32 i = 0; i < enclosedBBs.size(); ++i) {
    BBT *lowerBB = enclosedBBs[i];
    uint32 nextEnclosedIdx = i + 1;
    if (lowerBB == nullptr) {
      continue;  /* we may have removed the element */
    }
    if (!lowerBB->IsLabeled()) {
      continue;
    }
    labeledBBsInTry.emplace_back(lowerBB);

    /*
     * It seems the way a finally is associated with its try is to put the catch block inside
     * the java-try-end-try block. So, keep the 'catch(void*)' in it.
     */
    LabelIdx ebbLabel = lowerBB->GetLabelIdx();
    bool found = CheckAndProcessCatchNodeInCurrTryBlock(*lowerBB, ebbLabel, i);
    /* fill cur_bb_thread until meet the next catch */
    if (!found && lowerBB->IsCatch()) {
      enclosedBBs[i] = nullptr;
      std::vector<BBT*> currBBThread;
      BBT *nextBBThreadHead = nullptr;
      bool isFirstTime = true;
      bool hasMoveEndTry = false;
      do {
        if (nextBBThreadHead != nullptr) {
          isFirstTime = false;
        }
        nextBBThreadHead = nullptr;
        currBBThread.clear();
        currBBThread.emplace_back(lowerBB);
        nextBBThreadHead = CollectCatchAndFallthruUntilNextCatchBB(lowerBB, nextEnclosedIdx, currBBThread);
        WrapCatchWithTryEndTryBlock(currBBThread, nextBBThreadHead, nextEnclosedIdx, hasMoveEndTry);
        if (isFirstTime) {
          SwapEndTryBBAndCurrBBThread(currBBThread, hasMoveEndTry, nextBBThreadHead);
        }
      } while (nextBBThreadHead != nullptr);
    }
  }
}

void TryCatchBlocksLower::ConnectRemainBB() {
  MapleVector<BBT*> &enclosedBBs = tryEndTryBlock.GetEnclosedBBs();
  BBT *startTryBB = tryEndTryBlock.GetStartTryBB();
  BBT *endTryBB = tryEndTryBlock.GetEndTryBB();
  size_t nEnclosedBBs = enclosedBBs.size();
  size_t k = 0;
  while ((k < nEnclosedBBs) && (enclosedBBs[k] == nullptr)) {
    ++k;
  }

  if (k < nEnclosedBBs) {
    BBT *prevBB = enclosedBBs[k];

    startTryBB->GetLastStmt()->SetNext(prevBB->GetFirstStmt());
    prevBB->GetFirstStmt()->SetPrev(startTryBB->GetLastStmt());

    for (++k; k < nEnclosedBBs; ++k) {
      BBT *lowerBB = enclosedBBs[k];
      if (lowerBB == nullptr) {
        continue;
      }
      prevBB->GetLastStmt()->SetNext(lowerBB->GetFirstStmt());
      lowerBB->GetFirstStmt()->SetPrev(prevBB->GetLastStmt());
      prevBB = lowerBB;
    }

    prevBB->GetLastStmt()->SetNext(endTryBB->GetFirstStmt());
    endTryBB->GetFirstStmt()->SetPrev(prevBB->GetLastStmt());
  } else {
    startTryBB->GetLastStmt()->SetNext(endTryBB->GetFirstStmt());
    endTryBB->GetFirstStmt()->SetPrev(startTryBB->GetLastStmt());
  }
}

BBT *TryCatchBlocksLower::FindInsertAfterBB() {
  BBT *insertAfter = tryEndTryBlock.GetEndTryBB();
  CHECK_FATAL(tryEndTryBlock.GetEndTryBB()->GetLastStmt()->GetOpCode() == OP_endtry, "LowerBB type check");
  BBT *iaOpenTry = nullptr;
  while (insertAfter->GetFallthruBranch() != nullptr || iaOpenTry != nullptr) {
    if (insertAfter->GetFallthruBranch() != nullptr) {
      insertAfter = insertAfter->GetFallthruBranch();
    } else {
      CHECK_FATAL(iaOpenTry != nullptr, "iaOpenTry should not be nullptr");
      insertAfter = firstStmtToBBMap[insertAfter->GetLastStmt()->GetNext()];
      CHECK_FATAL(!insertAfter->IsTry(), "insertAfter should not be try");
    }

    if (insertAfter->IsTry()) {
      iaOpenTry = insertAfter;
    } else if (insertAfter->IsEndTry()) {
      iaOpenTry = nullptr;
    }
  }
  return insertAfter;
}

void TryCatchBlocksLower::PlaceRelocatedBB(BBT &insertAfter) {
  StmtNode *iaLast = insertAfter.GetLastStmt();
  CHECK_FATAL(iaLast != nullptr, "iaLast should not nullptr");

  StmtNode *iaNext = iaLast->GetNext();
  if (iaNext == nullptr) {
    CHECK_FATAL(body.GetLast() == iaLast, "body's last should equal iaLast");
  }
  BBT *prevBB = &insertAfter;
  MapleVector<BBT*> &bbsToRelocate = tryEndTryBlock.GetBBsToRelocate();
  for (auto &rbb : bbsToRelocate) {
    prevBB->GetLastStmt()->SetNext(rbb->GetFirstStmt());
    rbb->GetFirstStmt()->SetPrev(prevBB->GetLastStmt());
    prevBB = rbb;
  }
  prevBB->GetLastStmt()->SetNext(iaNext);
  if (iaNext != nullptr) {
    iaNext->SetPrev(prevBB->GetLastStmt());
  } else {
    /* !ia_next means we started with insert_after that was the last bblock Refer to the above CHECK_FATAL. */
    body.SetLast(prevBB->GetLastStmt());
    body.GetLast()->SetNext(nullptr);
  }
}

void TryCatchBlocksLower::PalceCatchSeenSofar(BBT &insertAfter) {
  TryNode *tryNode = static_cast<TryNode*>(tryEndTryBlock.GetTryStmtNode());
  ASSERT(tryNode != nullptr, "tryNode should not be nullptr");
  MapleVector<BBT*> &bbsToRelocate = tryEndTryBlock.GetBBsToRelocate();

  for (size_t offsetIndex = 0; offsetIndex < tryNode->GetOffsetsCount(); ++offsetIndex) {
    auto id = tryNode->GetOffset(offsetIndex);
    bool myCatchBlock = false;
    for (auto &jcb : bbsToRelocate) {
      if (!jcb->IsLabeled()) {
        continue;
      }
      myCatchBlock = (id == jcb->GetLabelIdx());
      if (myCatchBlock) {
        break;
      }
    }
    /*
     * If the catch block is the one enclosed in this try-endtry block,
     * we just relocated it above, so we don't need to consider it again
     */
    if (myCatchBlock) {
      continue;
    }

    CHECK_FATAL(body.GetLast()->GetNext() == nullptr, "the next of body's last should be nullptr");
    for (auto &jcb : catchesSeenSoFar) {
      CHECK_FATAL(jcb->IsLabeled(), "jcb should be labeled");
      if (id == jcb->GetLabelIdx()) {
        /*
         * Remove jcb and all of the blocks that are reachable by following fallthruBranch.
         * If we hit a try block, cut there, append an unconditional jump to it to the preceding bblock,
         * and relocate them. We may need to insert a label in the try block
         */
        BBT *lastBB = jcb;
        while (lastBB->GetFallthruBranch() != nullptr && !lastBB->GetFallthruBranch()->IsTry()) {
          lastBB = lastBB->GetFallthruBranch();
        }

#if DEBUG
        BBT::ValidateStmtList(bodyFirst);
#endif
        if (lastBB->GetFallthruBranch() != nullptr) {
          BBT *jtBB = lastBB->GetFallthruBranch();
          CHECK_FATAL(jtBB->IsTry(), "jtBB should be try");
          if (!jtBB->IsLabeled()) {
            LabelIdx jtLabIdx = mirModule.GetMIRBuilder()->CreateLabIdx(*mirModule.CurFunction());
            jtBB->SetLabelIdx(jtLabIdx);
            StmtNode *labelStmt = mirModule.GetMIRBuilder()->CreateStmtLabel(jtLabIdx);
            bool adjustBBFirstStmt = (jtBB->GetKeyStmt() == jtBB->GetFirstStmt());
            labelStmt->SetNext(jtBB->GetKeyStmt());
            labelStmt->SetPrev(jtBB->GetKeyStmt()->GetPrev());
            CHECK_FATAL(jtBB->GetKeyStmt()->GetPrev() != nullptr, "the prev of jtBB's ketStmt shpould not be nullptr");
            jtBB->GetKeyStmt()->GetPrev()->SetNext(labelStmt);
            CHECK_FATAL(jtBB->GetKeyStmt()->GetNext() != nullptr, "the next of jtBB's ketStmt shpould not be nullptr");
            jtBB->GetKeyStmt()->SetPrev(labelStmt);
            if (adjustBBFirstStmt) {
              firstStmtToBBMap.erase(jtBB->GetFirstStmt());
              jtBB->SetFirstStmt(*labelStmt);
              firstStmtToBBMap[jtBB->GetFirstStmt()] = jtBB;
            }
          }
          CHECK_FATAL(jtBB->IsLabeled(), "jtBB should be labeled");
          CHECK_FATAL(lastBB->GetLastStmt()->GetOpCode() != OP_goto,
                      "the opcode of lastBB's lastStmt should not be OP_goto");
          StmtNode *gotoStmt = mirModule.GetMIRBuilder()->CreateStmtGoto(OP_goto, jtBB->GetLabelIdx());

          StmtNode *lastBBLastStmt = lastBB->GetLastStmt();
          gotoStmt->SetNext(lastBBLastStmt->GetNext());
          gotoStmt->SetPrev(lastBBLastStmt);
          if (lastBBLastStmt->GetNext()) {
            lastBBLastStmt->GetNext()->SetPrev(gotoStmt);
          }
          lastBBLastStmt->SetNext(gotoStmt);

          lastBB->SetLastStmt(*gotoStmt);
          lastBB->SetFallthruBranch(nullptr);

#if DEBUG
          CHECK_FATAL(body.GetLast()->GetNext() == nullptr, "the next of body's last should be nullptr");
          BBT::ValidateStmtList(bodyFirst);
#endif
        }

        /* we want to remove [jcb .. last_bb], inclusively. */
        if (jcb->GetFirstStmt() == body.GetFirst()) {
          body.SetFirst(lastBB->GetLastStmt()->GetNext());
          body.GetFirst()->SetPrev(nullptr);
          lastBB->GetLastStmt()->GetNext()->SetPrev(nullptr);
          bodyFirst = body.GetFirst();
        } else {
          CHECK_FATAL(jcb->GetFirstStmt()->GetPrev() != nullptr, "the prev of jcb's firstStmt should not be nullptr");
          CHECK_FATAL(jcb->GetFirstStmt()->GetPrev()->GetNext() == jcb->GetFirstStmt(),
                      "the next of the prev of jcb's firstStmt should equal jcb's firstStmt");
          if (lastBB->GetLastStmt()->GetNext() != nullptr) {
            jcb->GetFirstStmt()->GetPrev()->SetNext(lastBB->GetLastStmt()->GetNext());
            lastBB->GetLastStmt()->GetNext()->SetPrev(jcb->GetFirstStmt()->GetPrev());
          } else {
            CHECK_FATAL(lastBB->GetLastStmt() == body.GetLast(), "lastBB's lastStmt should equal body's last");
            body.SetLast(jcb->GetFirstStmt()->GetPrev());
            body.GetLast()->SetNext(nullptr);
            jcb->GetFirstStmt()->GetPrev()->SetNext(nullptr);
          }
        }
        jcb->GetFirstStmt()->SetPrev(nullptr);
        lastBB->GetLastStmt()->SetNext(nullptr);

#if DEBUG
        CHECK_FATAL(body.GetLast()->GetNext() == nullptr, "the next of body's last should be nullptr");
        BBT::ValidateStmtList(body.GetFirst(), jcb->GetFirstStmt());
#endif

        /* append it (i.e., [jcb->firstStmt .. last_bb->lastStmt]) after insert_after */
        CHECK_FATAL(insertAfter.GetFallthruBranch() == nullptr, "insertAfter's fallthruBranch should be nullptr");
        if (insertAfter.GetLastStmt() == body.GetLast()) {
          CHECK_FATAL(insertAfter.GetLastStmt()->GetNext() == nullptr,
                      "the next of insertAfter's lastStmt should not be nullptr");
        }

        jcb->GetFirstStmt()->SetPrev(insertAfter.GetLastStmt());
        lastBB->GetLastStmt()->SetNext(insertAfter.GetLastStmt()->GetNext());

        CHECK_FATAL(body.GetLast()->GetNext() == nullptr, "the next of body's last should be nullptr");

        if (insertAfter.GetLastStmt()->GetNext() != nullptr) {
          insertAfter.GetLastStmt()->GetNext()->SetPrev(lastBB->GetLastStmt());
          CHECK_FATAL(body.GetLast()->GetNext() == nullptr, "the next of body's last should be nullptr");
        } else {
          /*
           * note that we have a single BlockNode that contains  all the instructions of a method.
           * What that means is each instruction's next is not nullptr except for the very last instruction.
           * insert_after->lastStmt->next == nullptr, means insert_after->lastStmt is indeed the last instruction,
           * and we are moving instructions of 'last_bb' after it. Thus, we need to fix the BlockNode's last field.
           */
          body.SetLast(lastBB->GetLastStmt());
          CHECK_FATAL(body.GetLast()->GetNext() == nullptr, "the next of body's last should be nullptr");
        }
        insertAfter.GetLastStmt()->SetNext(jcb->GetFirstStmt());
        if (jcb->GetFirstStmt()->GetPrev() != nullptr) {
          CHECK_FATAL(jcb->GetFirstStmt()->GetPrev()->GetNext() == jcb->GetFirstStmt(),
                      "the next of the prev of jcb's firstStmt should equal jcb's firstStmt");
        }
        if (lastBB->GetLastStmt()->GetNext() != nullptr) {
          CHECK_FATAL(lastBB->GetLastStmt()->GetNext()->GetPrev() == lastBB->GetLastStmt(),
                      "thr prev of the next of lastBB's lastStmt should equal lastBB's lastStmt");
        }

        CHECK_FATAL(body.GetLast()->GetNext() == nullptr, "the next of body's last should be nullptr");
      }
    }
  }
}

void TryCatchBlocksLower::TraverseBBList() {
  tryEndTryBlock.Init();
  for (auto &bb : bbList) {
    if (bb->IsCatch() && tryEndTryBlock.GetStartTryBB() == nullptr) {
      /* Add to the list of catch blocks seen so far. */
      catchesSeenSoFar.emplace_back(bb);
    }
    bodyEndWithEndTry = false;

    if (tryEndTryBlock.GetStartTryBB() == nullptr) {
      if (bb->IsTry()) {
        StmtNode *firstNonCommentStmt = bb->GetFirstStmt();
        while (firstNonCommentStmt != nullptr && firstNonCommentStmt->GetOpCode() == OP_comment) {
          firstNonCommentStmt = firstNonCommentStmt->GetNext();
        }
        CHECK_FATAL(bb->GetLastStmt()->GetOpCode() != OP_try || bb->GetLastStmt() == firstNonCommentStmt ||
                    !generateEHCode, "make sure the opcode of bb's lastStmt is not OP_try"
                    "or the opcode of bb's lastStmt is OP_try but bb's lastStmt equals firstNonCommentStmt"
                    "or not generate EHCode");
        /* prepare for processing a java try block */
        tryEndTryBlock.Reset(*bb);
      }
      continue;
    }

    /* We should have not a try block enclosed in another java try block!! */
    CHECK_FATAL(!bb->IsTry(), "bb should not be try");
    if (!bb->IsEndTry()) {
      tryEndTryBlock.PushToEnclosedBBs(*bb);
    } else {
      tryEndTryBlock.SetEndTryBB(bb);
      if (tryEndTryBlock.GetEndTryBB()->GetLastStmt() == body.GetLast()) {
        bodyEndWithEndTry = true;
      }
#if DEBUG
      for (size_t i = 0; i < tryEndTryBlock.GetEnclosedBBsSize(); ++i) {
        CHECK_FATAL(tryEndTryBlock.GetEnclosedBBsElem(i), "there should not be nullptr in enclosedBBs");
      }
#endif
      ProcessEnclosedBBBetweenTryEndTry();
      /* Now, connect the remaining ones again n_enclosed_bbs includes 'nullptr's (i.e., deleted entries) */
      ConnectRemainBB();
      BBT *insertAfter = FindInsertAfterBB();
      PlaceRelocatedBB(*insertAfter);

#if DEBUG
      CHECK_FATAL(body.GetLast()->GetNext() == nullptr, "the next of body's last should be nullptr");
      BBT::ValidateStmtList(bodyFirst);
#endif
      if (prevBBOfTry[tryEndTryBlock.GetStartTryBB()]) {
        StmtNode *firstStmtMovedIn = MoveCondGotoIntoTry(*tryEndTryBlock.GetStartTryBB(),
                                                         *prevBBOfTry[tryEndTryBlock.GetStartTryBB()],
                                                         tryEndTryBlock.GetLabeledBBsInTry());
        if (firstStmtMovedIn == bodyFirst) {
          bodyFirst = tryEndTryBlock.GetStartTryBB()->GetFirstStmt();
          prevBBOfTry[tryEndTryBlock.GetStartTryBB()] = nullptr;
        }
      }
      /*
       * Now, examine each offset attached to this try and move any catch block
       * that is not in 'bbs_to_relocate' but in 'catches_seen_so_far'
       */
      PalceCatchSeenSofar(*insertAfter);

      /* close the try that is open */
      tryEndTryBlock.SetStartTryBB(nullptr);
    }
#if DEBUG
    CHECK_FATAL(body.GetLast()->GetNext() == nullptr, "the next of body's last should be nullptr");
    BBT::ValidateStmtList(bodyFirst);
#endif
  }

  body.SetFirst(bodyFirst);
}

void TryCatchBlocksLower::CheckTryCatchPattern() const {
  StmtNode *openJt = nullptr;
  for (StmtNode *stmt = body.GetFirst(); stmt; stmt = stmt->GetNext()) {
    switch (stmt->GetOpCode()) {
      case OP_try:
        openJt = stmt;
        break;
      case OP_endtry:
        openJt = nullptr;
        break;
      case OP_catch:
        if (openJt != nullptr) {
          CatchNode *jcn = static_cast<CatchNode*>(stmt);
          for (uint32 i = 0; i < jcn->Size(); ++i) {
            MIRType *type = GlobalTables::GetTypeTable().GetTypeFromTyIdx(jcn->GetExceptionTyIdxVecElement(i));
            MIRPtrType *ptr = static_cast<MIRPtrType*>(type);
            type = GlobalTables::GetTypeTable().GetTypeFromTyIdx(ptr->GetPointedTyIdx());
            CHECK_FATAL(type->GetPrimType() == PTY_void, "type's primType should be PTY_void");
          }
        }
        break;
      default:
        break;
    }
  }
}
} /* namespace maplebe */