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
#ifndef MAPLEBE_INCLUDE_BE_TRY_CATCH_H
#define MAPLEBE_INCLUDE_BE_TRY_CATCH_H
#include "bbt.h"
/* MapleIR headers. */
#include "mir_nodes.h"
#include "mir_lower.h"

namespace maplebe {
using namespace maple;
class TryEndTryBlock {
 public:
  explicit TryEndTryBlock(MemPool &memPool)
      : allocator(&memPool), enclosedBBs(allocator.Adapter()),
        labeledBBsInTry(allocator.Adapter()), bbsToRelocate(allocator.Adapter()) {}

  ~TryEndTryBlock() = default;

  void Init() {
    startTryBB = nullptr;
    endTryBB = nullptr;
    tryStmt = nullptr;
    enclosedBBs.clear();
    labeledBBsInTry.clear();
    bbsToRelocate.clear();
  }

  void Reset(BBT &startBB) {
    startTryBB = &startBB;
    CHECK_NULL_FATAL(startTryBB->GetKeyStmt());
    tryStmt = startTryBB->GetKeyStmt();
    CHECK_FATAL(tryStmt->GetOpCode() == OP_try, "expect OPT_try");
    endTryBB = nullptr;
    enclosedBBs.clear();
    labeledBBsInTry.clear();
    bbsToRelocate.clear();
  }

  void SetStartTryBB(BBT *bb) {
    startTryBB = bb;
  }

  BBT *GetStartTryBB() {
    return startTryBB;
  }

  void SetEndTryBB(BBT *bb) {
    endTryBB = bb;
  }

  BBT *GetEndTryBB() {
    return endTryBB;
  }

  StmtNode *GetTryStmtNode() {
    return tryStmt;
  }

  MapleVector<BBT*> &GetEnclosedBBs() {
    return enclosedBBs;
  }

  size_t GetEnclosedBBsSize() const {
    return enclosedBBs.size();
  }

  const BBT *GetEnclosedBBsElem(size_t index) const{
    ASSERT(index < enclosedBBs.size(), "out of range");
    return enclosedBBs[index];
  }

  void PushToEnclosedBBs(BBT &bb) {
    enclosedBBs.push_back(&bb);
  }

  MapleVector<BBT*> &GetLabeledBBsInTry() {
    return labeledBBsInTry;
  }

  MapleVector<BBT*> &GetBBsToRelocate() {
    return bbsToRelocate;
  }

 private:
  MapleAllocator allocator;
  BBT *startTryBB = nullptr;
  BBT *endTryBB = nullptr;
  StmtNode *tryStmt = nullptr;
  MapleVector<BBT*> enclosedBBs;
  MapleVector<BBT*> labeledBBsInTry;
  MapleVector<BBT*> bbsToRelocate;
};

class TryCatchBlocksLower {
 public:
  TryCatchBlocksLower(MemPool &memPool, BlockNode &body, MIRModule &mirModule)
      : memPool(memPool), allocator(&memPool), body(body), mirModule(mirModule),
        tryEndTryBlock(memPool), bbList(allocator.Adapter()), prevBBOfTry(allocator.Adapter()),
        firstStmtToBBMap(allocator.Adapter()), catchesSeenSoFar(allocator.Adapter()) {}

  ~TryCatchBlocksLower() = default;
  void RecoverBasicBlock();
  void TraverseBBList();
  void CheckTryCatchPattern() const;

  void SetGenerateEHCode(bool val) {
    generateEHCode = val;
  }

 private:
  MemPool &memPool;
  MapleAllocator allocator;
  BlockNode &body;
  MIRModule &mirModule;
  TryEndTryBlock tryEndTryBlock;
  StmtNode *bodyFirst = nullptr;
  bool bodyEndWithEndTry = false;
  bool generateEHCode = false;
  MapleVector<BBT*> bbList;
  MapleMap<BBT*, BBT*> prevBBOfTry;
  MapleMap<StmtNode*, BBT*> firstStmtToBBMap;
  MapleVector<BBT*> catchesSeenSoFar;

  void ProcessEnclosedBBBetweenTryEndTry();
  void ConnectRemainBB();
  BBT *FindInsertAfterBB();
  void PlaceRelocatedBB(BBT &insertAfter);
  void PalceCatchSeenSofar(BBT &insertAfter);
  BBT *CreateNewBB(StmtNode *first, StmtNode *last);
  bool CheckAndProcessCatchNodeInCurrTryBlock(BBT &ebb, LabelIdx ebbLabel, uint32 index);
  BBT *CollectCatchAndFallthruUntilNextCatchBB(BBT *&ebb, uint32 &nextEnclosedIdx,
                                               std::vector<BBT*> &currBBThread);
  void WrapCatchWithTryEndTryBlock(std::vector<BBT*> &currBBThread, BBT *&nextBBThreadHead,
                                   uint32 &nextEnclosedIdx, bool hasMoveEndTry);
  void SwapEndTryBBAndCurrBBThread(const std::vector<BBT*> &currBBThread, bool &hasMoveEndTry, BBT *nextBBThreadHead);
  void ProcessThreadTail(BBT &threadTail, BBT *&nextBBThreadHead, bool hasMoveEndTry);
  static StmtNode *MoveCondGotoIntoTry(BBT &jtBB, BBT &condbrBB, const MapleVector<BBT*> &labeledBBsInTry);
  static BBT *FindTargetBBlock(LabelIdx idx, const std::vector<BBT*> &bbs);
};
} /* namespace maplebe */

#endif /* MAPLEBE_INCLUDE_BE_TRY_CATCH_H */