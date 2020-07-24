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
#ifndef MAPLEBE_INCLUDE_CG_CGBB_H
#define MAPLEBE_INCLUDE_CG_CGBB_H

#include "isa.h"
#include "insn.h"
#include "datainfo.h"

/* Maple IR headers */
#include "mir_nodes.h"
#include "mir_symbol.h"

/* Maple MP header */
#include "mempool_allocator.h"

namespace maplebe {
/* For get bb */
#define FIRST_BB_OF_FUNC(FUNC) ((FUNC)->GetFirstBB())
#define LAST_BB_OF_FUNC(FUNC) ((FUNC)->GetLastBB())

/* For iterating over basic blocks. */
#define FOR_BB_BETWEEN(BASE, FROM, TO, DIR) for (BB * (BASE) = (FROM); (BASE) != (TO); (BASE) = (BASE)->DIR())
#define FOR_BB_BETWEEN_CONST(BASE, FROM, TO, DIR) \
  for (const BB * (BASE) = (FROM); (BASE) != (TO); (BASE) = (BASE)->DIR())

#define FOR_ALL_BB_CONST(BASE, FUNC) FOR_BB_BETWEEN_CONST(BASE, FIRST_BB_OF_FUNC(FUNC), nullptr, GetNext)
#define FOR_ALL_BB(BASE, FUNC) FOR_BB_BETWEEN(BASE, FIRST_BB_OF_FUNC(FUNC), nullptr, GetNext)
#define FOR_ALL_BB_REV(BASE, FUNC) FOR_BB_BETWEEN(BASE, LAST_BB_OF_FUNC(FUNC), nullptr, GetPrev)

/* For get insn */
#define FIRST_INSN(BLOCK) (BLOCK)->GetFirstInsn()
#define LAST_INSN(BLOCK) (BLOCK)->GetLastInsn()
#define NEXT_INSN(INSN) (INSN)->GetNext()
#define PREV_INSN(INSN) (INSN)->GetPrev()

/* For iterating over insns in basic block. */
#define FOR_INSN_BETWEEN(INSN, FROM, TO, DIR) \
  for (Insn * (INSN) = (FROM); (INSN) != nullptr && (INSN) != (TO); (INSN) = (INSN)->DIR)

#define FOR_BB_INSNS(INSN, BLOCK) \
  for (Insn * (INSN) = FIRST_INSN(BLOCK); (INSN) != nullptr; (INSN) = (INSN)->GetNext())
#define FOR_BB_INSNS_CONST(INSN, BLOCK) \
  for (const Insn * (INSN) = FIRST_INSN(BLOCK); (INSN) != nullptr; (INSN) = (INSN)->GetNext())

#define FOR_BB_INSNS_REV(INSN, BLOCK) \
  for (Insn * (INSN) = LAST_INSN(BLOCK); (INSN) != nullptr; (INSN) = (INSN)->GetPrev())

/* For iterating over insns in basic block when we might remove the current insn. */
#define FOR_BB_INSNS_SAFE(INSN, BLOCK, NEXT)                                                               \
  for (Insn * (INSN) = FIRST_INSN(BLOCK), *(NEXT) = (INSN) ? NEXT_INSN(INSN) : nullptr; (INSN) != nullptr; \
       (INSN) = (NEXT), (NEXT) = (INSN) ? NEXT_INSN(INSN) : nullptr)

#define FOR_BB_INSNS_REV_SAFE(INSN, BLOCK, NEXT)                                                          \
  for (Insn * (INSN) = LAST_INSN(BLOCK), *(NEXT) = (INSN) ? PREV_INSN(INSN) : nullptr; (INSN) != nullptr; \
       (INSN) = (NEXT), (NEXT) = (INSN) ? PREV_INSN(INSN) : nullptr)

class CGFuncLoops;

class BB {
 public:
  enum BBKind : uint8 {
    kBBFallthru,  /* default */
    kBBIf,        /* conditional branch */
    kBBGoto,      /* unconditional branch */
    kBBReturn,
    kBBIntrinsic,  /* BB created by inlining intrinsics; shares a lot with BB_if */
    kBBRangeGoto,
    kBBThrow,      /* For call __java_throw_* and call exit, which will run out of function. */
    kBBLast
  };

  BB(uint32 bbID, MapleAllocator &mallocator)
      : id(bbID),
        kind(kBBFallthru), /* kBBFallthru default kind */
        labIdx(MIRLabelTable::GetDummyLabel()),
        preds(mallocator.Adapter()),
        succs(mallocator.Adapter()),
        ehPreds(mallocator.Adapter()),
        ehSuccs(mallocator.Adapter()),
        loopPreds(mallocator.Adapter()),
        loopSuccs(mallocator.Adapter()),
        liveInRegNO(mallocator.Adapter()),
        liveOutRegNO(mallocator.Adapter()),
        callInsns(mallocator.Adapter()),
        rangeGotoLabelVec(mallocator.Adapter()) {}

  virtual ~BB() = default;

  virtual BB *Clone(MemPool &memPool) const {
    BB *bb = memPool.Clone<BB>(*this);
    return bb;
  }

  void Dump() const;
  bool IsCommentBB() const;
  bool IsEmptyOrCommentOnly() const;
  bool IsSoloGoto() const;

  bool IsEmpty() const {
    if (lastInsn == nullptr) {
      CHECK_FATAL(firstInsn == nullptr, "firstInsn must be nullptr");
      return true;
    } else {
      CHECK_FATAL(firstInsn != nullptr, "firstInsn must not be nullptr");
      return false;
    }
  }

  const std::string &GetKindName() const {
    ASSERT(kind < kBBLast, "out of range in GetKindName");
    return bbNames[kind];
  }

  void SetKind(BBKind bbKind) {
    kind = bbKind;
  }

  BBKind GetKind() const {
    return kind;
  }

  void AddLabel(LabelIdx idx) {
    labIdx = idx;
  }

  void AppendBB(BB &bb) {
    bb.prev = this;
    bb.next = next;
    if (next != nullptr) {
      next->prev = &bb;
    }
    next = &bb;
  }

  void PrependBB(BB &bb) {
    bb.next = this;
    bb.prev = this->prev;
    if (this->prev != nullptr) {
      this->prev->next = &bb;
    }
    this->prev = &bb;
  }

  Insn *InsertInsnBefore(Insn &existing, Insn &newInsn);

  /* returns newly inserted instruction */
  Insn *InsertInsnAfter(Insn &existing, Insn &newInsn);

  void InsertInsnBegin(Insn &insn) {
    if (lastInsn == nullptr) {
      firstInsn = lastInsn = &insn;
      insn.SetBB(this);
    } else {
      InsertInsnBefore(*firstInsn, insn);
    }
  }

  void AppendInsn(Insn &insn) {
    if (firstInsn != nullptr) {
      InsertInsnAfter(*lastInsn, insn);
    } else {
      firstInsn = lastInsn = &insn;
      insn.SetNext(nullptr);
      insn.SetPrev(nullptr);
      insn.SetBB(this);
    }
  }

  void ReplaceInsn(Insn &insn, Insn &newInsn);

  void RemoveInsn(Insn &insn);

  void RemoveInsnPair(Insn &insn, Insn &nextInsn);

  void RemoveInsnSequence(Insn &insn, Insn &nextInsn);

  /* append all insns from bb into this bb */
  void AppendBBInsns(BB &bb);

  /* append all insns from bb into this bb */
  void InsertAtBeginning(BB &bb);

  /* clear BB but don't remove insns of this */
  void ClearInsns() {
    firstInsn = lastInsn = nullptr;
  }

  uint32 NumPreds() const {
    return static_cast<uint32>(preds.size());
  }

  bool IsPredecessor(const BB &predBB) {
    for (const BB *bb : preds) {
      if (bb == &predBB) {
        return true;
      }
    }
    return false;
  }

  bool IsBackEdgeDest() const {
    return !loopPreds.empty();
  }

  void RemoveFromPredecessorList(const BB &bb) {
    for (auto i = preds.begin(); i != preds.end(); ++i) {
      if (*i == &bb) {
        preds.erase(i);
        return;
      }
    }
    CHECK_FATAL(false, "request to remove a non-existent element?");
  }

  void RemoveFromSuccessorList(const BB &bb) {
    for (auto i = succs.begin(); i != succs.end(); ++i) {
      if (*i == &bb) {
        succs.erase(i);
        return;
      }
    }
    CHECK_FATAL(false, "request to remove a non-existent element?");
  }

  uint32 NumSuccs() const {
    return static_cast<uint32>(succs.size());
  }

  bool HasCall() const {
    return hasCall;
  }

  void SetHasCall() {
    hasCall = true;
  }

  /* Number of instructions excluding DbgInsn and comments */
  int32 NumInsn() const;
  uint32 GetId() const {
    return id;
  }
  uint32 GetLevel() const {
    return level;
  }
  void SetLevel(uint32 arg) {
    level = arg;
  }
  uint32 GetFrequency() const {
    return frequency;
  }
  void SetFrequency(uint32 arg) {
    frequency = arg;
  }
  BB *GetNext() {
    return next;
  }
  const BB *GetNext() const {
    return next;
  }
  BB *GetPrev() {
    return prev;
  }
  const BB *GetPrev() const {
    return prev;
  }
  void SetNext(BB *arg) {
    next = arg;
  }
  void SetPrev(BB *arg) {
    prev = arg;
  }
  LabelIdx GetLabIdx() const {
    return labIdx;
  }
  void SetLabIdx(LabelIdx arg) {
    labIdx = arg;
  }
  StmtNode *GetFirstStmt() {
    return firstStmt;
  }
  const StmtNode *GetFirstStmt() const {
    return firstStmt;
  }
  void SetFirstStmt(StmtNode &arg) {
    firstStmt = &arg;
  }
  StmtNode *GetLastStmt() {
    return lastStmt;
  }
  const StmtNode *GetLastStmt() const {
    return lastStmt;
  }
  void SetLastStmt(StmtNode &arg) {
    lastStmt = &arg;
  }
  Insn *GetFirstInsn() {
    return firstInsn;
  }
  const Insn *GetFirstInsn() const {
    return firstInsn;
  }
  void SetFirstInsn(Insn *arg) {
    firstInsn = arg;
  }
  Insn *GetFirstMachineInsn() {
    FOR_BB_INSNS(insn, this) {
      if (insn->IsMachineInstruction()) {
        return insn;
      }
    }
    return nullptr;
  }
  Insn *GetLastMachineInsn() {
    FOR_BB_INSNS_REV(insn, this) {
      if (insn->IsMachineInstruction()) {
        return insn;
      }
    }
    return nullptr;
  }
  Insn *GetLastInsn() {
    return lastInsn;
  }
  const Insn *GetLastInsn() const {
    return lastInsn;
  }
  void SetLastInsn(Insn *arg) {
    lastInsn = arg;
  }
  const MapleList<BB*> &GetPreds() const {
    return preds;
  }
  const MapleList<BB*> &GetSuccs() const {
    return succs;
  }
  const MapleList<BB*> &GetEhPreds() const {
    return ehPreds;
  }
  const MapleList<BB*> &GetEhSuccs() const {
    return ehSuccs;
  }
  const MapleList<BB*> &GetLoopPreds() const {
    return loopPreds;
  }
  MapleList<BB*> &GetLoopSuccs() {
    return loopSuccs;
  }
  const MapleList<BB*> &GetLoopSuccs() const {
    return loopSuccs;
  }
  MapleList<BB*>::iterator GetPredsBegin() {
    return preds.begin();
  }
  MapleList<BB*>::iterator GetSuccsBegin() {
    return succs.begin();
  }
  MapleList<BB*>::iterator GetEhPredsBegin() {
    return ehPreds.begin();
  }
  MapleList<BB*>::iterator GetLoopSuccsBegin() {
    return loopSuccs.begin();
  }
  MapleList<BB*>::iterator GetPredsEnd() {
    return preds.end();
  }
  MapleList<BB*>::iterator GetSuccsEnd() {
    return succs.end();
  }
  MapleList<BB*>::iterator GetEhPredsEnd() {
    return ehPreds.end();
  }
  MapleList<BB*>::iterator GetLoopSuccsEnd() {
    return loopSuccs.end();
  }
  void PushBackPreds(BB &bb) {
    preds.push_back(&bb);
  }
  void PushBackSuccs(BB &bb) {
    succs.push_back(&bb);
  }
  void PushBackEhPreds(BB &bb) {
    ehPreds.push_back(&bb);
  }
  void PushBackEhSuccs(BB &bb) {
    ehSuccs.push_back(&bb);
  }
  void PushBackLoopPreds(BB &bb) {
    loopPreds.push_back(&bb);
  }
  void PushBackLoopSuccs(BB &bb) {
    loopSuccs.push_back(&bb);
  }
  void PushFrontPreds(BB &bb) {
    preds.push_front(&bb);
  }
  void PushFrontSuccs(BB &bb) {
    succs.push_front(&bb);
  }
  void ErasePreds(MapleList<BB*>::iterator it) {
    preds.erase(it);
  }
  void EraseSuccs(MapleList<BB*>::iterator it) {
    succs.erase(it);
  }
  void RemovePreds(BB &bb) {
    preds.remove(&bb);
  }
  void RemoveSuccs(BB &bb) {
    succs.remove(&bb);
  }
  void RemoveEhPreds(BB &bb) {
    ehPreds.remove(&bb);
  }
  void RemoveEhSuccs(BB &bb) {
    ehSuccs.remove(&bb);
  }
  void ClearPreds() {
    preds.clear();
  }
  void ClearSuccs() {
    succs.clear();
  }
  void ClearEhPreds() {
    ehPreds.clear();
  }
  void ClearEhSuccs() {
    ehSuccs.clear();
  }
  void ClearLoopPreds() {
    loopPreds.clear();
  }
  void ClearLoopSuccs() {
    loopSuccs.clear();
  }
  const MapleSet<regno_t> &GetLiveInRegNO() const {
    return liveInRegNO;
  }
  void InsertLiveInRegNO(regno_t arg) {
    (void)liveInRegNO.insert(arg);
  }
  void EraseLiveInRegNO(MapleSet<regno_t>::iterator it) {
    liveInRegNO.erase(it);
  }
  void EraseLiveInRegNO(regno_t arg) {
    liveInRegNO.erase(arg);
  }
  void ClearLiveInRegNO() {
    liveInRegNO.clear();
  }
  const MapleSet<regno_t> &GetLiveOutRegNO() const {
    return liveOutRegNO;
  }
  void InsertLiveOutRegNO(regno_t arg) {
    (void)liveOutRegNO.insert(arg);
  }
  void EraseLiveOutRegNO(MapleSet<regno_t>::iterator it) {
    liveOutRegNO.erase(it);
  }
  void ClearLiveOutRegNO() {
    liveOutRegNO.clear();
  }
  CGFuncLoops *GetLoop() const {
    return loop;
  }
  void SetLoop(CGFuncLoops &arg) {
    loop = &arg;
  }
  bool GetLiveInChange() {
    return liveInChange;
  }
  void SetLiveInChange(bool arg) {
    liveInChange = arg;
  }
  bool GetInsertUse() const {
    return insertUse;
  }
  void SetInsertUse(bool arg) {
    insertUse = arg;
  }
  bool IsUnreachable() const {
    return unreachable;
  }
  void SetUnreachable(bool arg) {
    unreachable = arg;
  }
  void SetFastPath(bool arg) {
    fastPath = arg;
  }
  bool IsCatch() const {
    return isCatch;
  }
  void SetIsCatch(bool arg) {
    isCatch = arg;
  }
  bool IsCleanup() const {
    return isCleanup;
  }
  void SetIsCleanup(bool arg) {
    isCleanup = arg;
  }
  long GetInternalFlag1() const {
    return internalFlag1;
  }
  void SetInternalFlag1(long arg) {
    internalFlag1 = arg;
  }
  long GetInternalFlag2() const {
    return internalFlag2;
  }
  void SetInternalFlag2(long arg) {
    internalFlag2 = arg;
  }
  long GetInternalFlag3() const {
    return internalFlag3;
  }
  void SetInternalFlag3(long arg) {
    internalFlag3 = arg;
  }
  const MapleList<Insn*> &GetCallInsns() const {
    return callInsns;
  }
  void PushBackCallInsns(Insn &insn) {
    callInsns.push_back(&insn);
  }
  void ClearCallInsns() {
    callInsns.clear();
  }
  const MapleVector<LabelIdx> &GetRangeGotoLabelVec() const {
    return rangeGotoLabelVec;
  }
  void PushBackRangeGotoLabel(LabelIdx labelIdx) {
    rangeGotoLabelVec.emplace_back(labelIdx);
  }
  const Insn *GetFirstLoc() const {
    return firstLoc;
  }
  void SetFirstLoc(const Insn &arg) {
    firstLoc = &arg;
  }
  const Insn *GetLastLoc() const {
    return lastLoc;
  }
  void SetLastLoc(const Insn *arg) {
    lastLoc = arg;
  }
  DataInfo *GetLiveIn() {
    return liveIn;
  }
  const DataInfo *GetLiveIn() const {
    return liveIn;
  }
  void SetLiveIn(DataInfo &arg) {
    liveIn = &arg;
  }
  void SetLiveInBit(uint32 arg) {
    liveIn->SetBit(arg);
  }
  void SetLiveInInfo(const DataInfo &arg) const {
    *liveIn = arg;
  }
  void LiveInOrBits(const DataInfo &arg) {
    liveIn->OrBits(arg);
  }
  void LiveInEnlargeCapacity(uint32 arg) {
    liveIn->EnlargeCapacityToAdaptSize(arg);
  }
  void LiveInClearDataInfo() {
    liveIn->ClearDataInfo();
  }
  DataInfo *GetLiveOut() {
    return liveOut;
  }
  const DataInfo *GetLiveOut() const {
    return liveOut;
  }
  void SetLiveOut(DataInfo &arg) {
    liveOut = &arg;
  }
  void SetLiveOutBit(uint32 arg) {
    liveOut->SetBit(arg);
  }
  void LiveOutOrBits(const DataInfo &arg) {
    liveOut->OrBits(arg);
  }
  void LiveOutEnlargeCapacity(uint32 arg) {
    liveOut->EnlargeCapacityToAdaptSize(arg);
  }
  void LiveOutClearDataInfo() {
    liveOut->ClearDataInfo();
  }
  const DataInfo *GetDef() const {
    return def;
  }
  void SetDef(DataInfo &arg) {
    def = &arg;
  }
  void SetDefBit(uint32 arg) {
    def->SetBit(arg);
  }
  void DefResetAllBit() {
    def->ResetAllBit();
  }
  void DefResetBit(uint32 arg) {
    def->ResetBit(arg);
  }
  void DefClearDataInfo() {
    def->ClearDataInfo();
  }
  const DataInfo *GetUse() const {
    return use;
  }
  void SetUse(DataInfo &arg) {
    use = &arg;
  }
  void SetUseBit(uint32 arg) {
    use->SetBit(arg);
  }
  void UseResetAllBit() {
    use->ResetAllBit();
  }
  void UseResetBit(uint32 arg) {
    use->ResetBit(arg);
  }
  void UseClearDataInfo() {
    use->ClearDataInfo();
  }

 private:
  static const std::string bbNames[kBBLast];
  uint32 id;
  uint32 level = 0;
  uint32 frequency = 0;
  BB *prev = nullptr;  /* Doubly linked list of BBs; */
  BB *next = nullptr;
  /* They represent the order in which blocks are to be emitted. */
  BBKind kind = kBBFallthru;  /* The BB's last statement (i.e. lastStmt) determines */
  /* what type this BB has. By default, kBbFallthru */
  LabelIdx labIdx;
  StmtNode *firstStmt = nullptr;
  StmtNode *lastStmt = nullptr;
  Insn *firstInsn = nullptr;        /* the first instruction */
  Insn *lastInsn = nullptr;         /* the last instruction */
  MapleList<BB*> preds;  /* preds, succs represent CFG */
  MapleList<BB*> succs;
  MapleList<BB*> ehPreds;
  MapleList<BB*> ehSuccs;
  MapleList<BB*> loopPreds;
  MapleList<BB*> loopSuccs;

  /* this is for live in out analysis */
  MapleSet<regno_t> liveInRegNO;
  MapleSet<regno_t> liveOutRegNO;
  CGFuncLoops *loop = nullptr;
  bool liveInChange = false;
  bool insertUse = false;
  bool hasCall = false;
  bool unreachable = false;
  bool fastPath = false;
  bool isCatch = false;  /* part of the catch bb, true does might also mean it is unreachable */
  /*
   * Since isCatch is set early and unreachable detected later, there
   * are some overlap here.
   */
  bool isCleanup = false;  /* true if the bb is cleanup bb. otherwise, false. */
  /*
   * Different meaning for each data flow analysis.
   * For aarchregalloc.cpp, the bb is part of cleanup at end of function.
   * For aarchcolorra.cpp, the bb is part of cleanup at end of function.
   *                       also used for live range splitting.
   * For live analysis, it indicates if bb is cleanupbb.
   */
  long internalFlag1 = 0;

  /*
   * Different meaning for each data flow analysis.
   * For aarchcolorra.cpp, used for live range splitting pruning of bb.
   */
  long internalFlag2 = 0;

  /*
   * Different meaning for each data flow analysis.
   * For cgfunc.cpp, it temporarily marks for catch bb discovery.
   * For live analysis, it indicates if bb is visited.
   * For peephole, used for live-out checking of bb.
   */
  long internalFlag3 = 0;
  MapleList<Insn*> callInsns;
  MapleVector<LabelIdx> rangeGotoLabelVec;

  const Insn *firstLoc = nullptr;
  const Insn *lastLoc = nullptr;
  DataInfo *liveIn = nullptr;
  DataInfo *liveOut = nullptr;
  DataInfo *def = nullptr;
  DataInfo *use = nullptr;
};  /* class BB */

struct BBIdCmp {
  bool operator()(const BB *lhs, const BB *rhs) const {
    CHECK_FATAL(lhs != nullptr, "null ptr check");
    CHECK_FATAL(rhs != nullptr, "null ptr check");
    return (lhs->GetId() < rhs->GetId());
  }
};
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_CGBB_H */
