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
#ifndef MAPLE_ME_INCLUDE_OCCUR_H
#define MAPLE_ME_INCLUDE_OCCUR_H
// the data structures that represent occurrences and work candidates for SSAPRE
#include "me_function.h"
#include "irmap.h"
const int kWorkCandHashLength = 229;

namespace maple {
enum OccType {
  kOccUndef,
  kOccReal,
  kOccPhiocc,
  kOccPhiopnd,
  kOccExit,
  kOccInserted,
  kOccGcmalloc,
  kOccUse,     // for use appearances when candidate is dassign
  kOccMembar,  // for representing occurrence of memory barriers (use MeRealOcc)
};

class MePhiOcc;
class MeOccur {
 public:
  MeOccur(OccType ty, int cId, MeOccur *df) : occTy(ty), classID(cId), mirBB(nullptr), def(df) {}
  MeOccur(OccType ty, int cId, BB &bb, MeOccur *df) : occTy(ty), classID(cId), mirBB(&bb), def(df) {}
  virtual ~MeOccur() = default;
  virtual void Dump(const IRMap&) const;
  void DumpOccur(IRMap&);
  bool IsDominate(Dominance &dom, MeOccur&);
  const BB *GetBB() const {
    return mirBB;
  }

  BB *GetBB() {
    return mirBB;
  }

  void SetBB(BB &bb) {
    mirBB = &bb;
  }

  OccType GetOccType() const {
    return occTy;
  }

  void SetOccType(OccType occType) {
    occTy = occType;
  }

  int GetClassID() const {
    return classID;
  }

  void SetClassID(int id) {
    classID = id;
  }

  const MeOccur *GetDef() const {
    return def;
  }

  MeOccur *GetDef() {
    return def;
  }

  void SetDef(MeOccur *define) {
    def = define;
  }
  MeExpr *GetSavedExpr();

 private:
  OccType occTy;  // kinds of occ
  int classID;    // class id
  BB *mirBB;      // the BB it occurs in
  MeOccur *def;
};

class MeRealOcc : public MeOccur {
 public:
  MeRealOcc(MeStmt *stmt, int sq, MeExpr *expr)
      : MeOccur(kOccReal, 0, nullptr),
        meStmt(stmt),
        meExpr(expr),
        savedExpr(nullptr),
        seq(sq),
        position(0),
        isReload(false),
        isSave(false),
        isLHS(false),
        isFormalAtEntry(false) {
    if (stmt != nullptr) {
      SetBB(*stmt->GetBB());
    }
  }

  ~MeRealOcc() = default;
  void Dump(const IRMap&) const;
  const MeStmt *GetMeStmt() const {
    return meStmt;
  }

  MeStmt *GetMeStmt() {
    return meStmt;
  }

  void SetMeStmt(MeStmt &stmt) {
    meStmt = &stmt;
  }

  Opcode GetOpcodeOfMeStmt() const {
    return meStmt->GetOp();
  }

  const MeExpr *GetMeExpr() const {
    return meExpr;
  }

  MeExpr *GetMeExpr() {
    return meExpr;
  }

  void SetMeExpr(MeExpr &expr) {
    meExpr = &expr;
  }

  const MeExpr *GetSavedExpr() const {
    return savedExpr;
  }

  MeExpr *GetSavedExpr() {
    return savedExpr;
  }

  void SetSavedExpr(MeExpr &expr) {
    savedExpr = &expr;
  }

  int GetSequence() const {
    return seq;
  }

  void SetSequence(int sequence) {
    seq = sequence;
  }

  size_t GetPosition() const {
    return position;
  }

  void SetPosition(size_t pos) {
    position = pos;
  }

  bool IsReload() const {
    return isReload;
  }

  void SetIsReload(bool reload) {
    isReload = reload;
  }

  bool IsSave() const {
    return isSave;
  }

  void SetIsSave(bool save) {
    isSave = save;
  }

  bool IsLHS() const {
    return isLHS;
  }

  void SetIsLHS(bool lhs) {
    isLHS = lhs;
  }

  bool IsFormalAtEntry() const {
    return isFormalAtEntry;
  }

  void SetIsFormalAtEntry(bool isFormal) {
    isFormalAtEntry = isFormal;
  }

 private:
  MeStmt *meStmt;     // the stmt that has this occ
  MeExpr *meExpr;     // the expr it's corresponding to
  MeExpr *savedExpr;  // the reall occ saved to, must be a VarMeExpr/RegMeExpr
  int seq;            // meStmt sequence number in the bb
  size_t position;    // the position in the workCand->GetRealOccs() vector
  bool isReload;
  bool isSave;
  bool isLHS;
  bool isFormalAtEntry;  // the fake lhs occurrence at entry for formals
};

class MeInsertedOcc : public MeOccur {
 public:
  MeInsertedOcc(MeExpr *expr, MeStmt *stmt, BB &bb)
      : MeOccur(kOccInserted, 0, bb, nullptr), meExpr(expr), meStmt(stmt), savedExpr(nullptr) {}

  ~MeInsertedOcc() = default;
  void Dump(const IRMap&) const;
  const MeStmt *GetMeStmt() const {
    return meStmt;
  }

  MeStmt *GetMeStmt() {
    return meStmt;
  }

  void SetMeStmt(MeStmt &stmt) {
    meStmt = &stmt;
  }

  Opcode GetOpcodeOfMeStmt() const {
    return meStmt->GetOp();
  }

  const MeExpr *GetMeExpr() const {
    return meExpr;
  }

  MeExpr *GetMeExpr() {
    return meExpr;
  }

  void SetMeExpr(MeExpr &expr) {
    meExpr = &expr;
  }

  const MeExpr *GetSavedExpr() const {
    return savedExpr;
  }

  MeExpr *GetSavedExpr() {
    return savedExpr;
  }

  void SetSavedExpr(MeExpr &expr) {
    savedExpr = &expr;
  }

 private:
  MeExpr *meExpr;
  MeStmt *meStmt;
  MeExpr *savedExpr;
};

class MePhiOpndOcc : public MeOccur {
 public:
  explicit MePhiOpndOcc(BB &bb)
      : MeOccur(kOccPhiopnd, 0, bb, nullptr),
        isProcessed(false),
        hasRealUse(false),
        isInsertedOcc(false),
        isPhiOpndReload(false),
        defPhiOcc(nullptr) {
    currentExpr.meStmt = nullptr;
  }

  ~MePhiOpndOcc() = default;
  bool IsOkToInsert() const;
  void Dump(const IRMap&) const;
  bool IsProcessed() const {
    return isProcessed;
  }

  void SetIsProcessed(bool processed) {
    isProcessed = processed;
  }

  bool HasRealUse() const {
    return hasRealUse;
  }

  void SetHasRealUse(bool realUse) {
    hasRealUse = realUse;
  }

  bool IsInsertedOcc() const {
    return isInsertedOcc;
  }

  void SetIsInsertedOcc(bool inserted) {
    isInsertedOcc = inserted;
  }

  bool IsPhiOpndReload() const {
    return isPhiOpndReload;
  }

  void SetIsPhiOpndReload(bool phiOpndReload) {
    isPhiOpndReload = phiOpndReload;
  }

  const MePhiOcc *GetDefPhiOcc() const {
    return defPhiOcc;
  }

  MePhiOcc *GetDefPhiOcc() {
    return defPhiOcc;
  }

  void SetDefPhiOcc(MePhiOcc &occ) {
    defPhiOcc = &occ;
  }

  const MeExpr *GetCurrentMeExpr() const {
    return currentExpr.meExpr;
  }

  MeExpr *GetCurrentMeExpr() {
    return currentExpr.meExpr;
  }

  void SetCurrentMeExpr(MeExpr &expr) {
    currentExpr.meExpr = &expr;
  }

  const MeStmt *GetCurrentMeStmt() const {
    return currentExpr.meStmt;
  }

  MeStmt *GetCurrentMeStmt() {
    return currentExpr.meStmt;
  }

  void SetCurrentMeStmt(MeStmt &stmt) {
    currentExpr.meStmt = &stmt;
  }

 private:
  bool isProcessed;
  bool hasRealUse;
  bool isInsertedOcc;    // the phi operand was inserted by inserted occ
  bool isPhiOpndReload;  // if insertedocc and redefined the def, set this flag
  MePhiOcc *defPhiOcc;   // its lhs
  union {
    MeExpr *meExpr;  // the current expression at the end of the block containing this PhiOpnd
    MeStmt *meStmt;  // which will be inserted during finalize
  } currentExpr;
};

class MePhiOcc : public MeOccur {
 public:
  MePhiOcc(BB &bb, MapleAllocator &alloc)
      : MeOccur(kOccPhiocc, 0, bb, nullptr),
        isDownSafe(!bb.GetAttributes(kBBAttrIsCatch)),
        speculativeDownSafe(false),
        isCanBeAvail(true),
        isLater(true),
        isExtraneous(false),
        isRemoved(false),
        phiOpnds(alloc.Adapter()),
        regPhi(nullptr),
        varPhi(nullptr) {}

  virtual ~MePhiOcc() = default;
  bool IsWillBeAvail() const {
    return isCanBeAvail && !isLater;
  }

  bool IsDownSafe() const {
    return isDownSafe;
  }

  void SetIsDownSafe(bool downSafe) {
    isDownSafe = downSafe;
  }

  bool SpeculativeDownSafe() const {
    return speculativeDownSafe;
  }

  void SetSpeculativeDownSafe(bool downSafe) {
    speculativeDownSafe = downSafe;
  }

  bool IsCanBeAvail() const {
    return isCanBeAvail;
  }

  void SetIsCanBeAvail(bool canBeAvail) {
    isCanBeAvail = canBeAvail;
  }

  bool IsLater() const {
    return isLater;
  }

  void SetIsLater(bool later) {
    isLater = later;
  }

  bool IsExtraneous() const {
    return isExtraneous;
  }

  void SetIsExtraneous(bool extra) {
    isExtraneous = extra;
  }

  bool IsRemoved() const {
    return isRemoved;
  }

  void SetIsRemoved(bool removed) {
    isRemoved = removed;
  }

  const MapleVector<MePhiOpndOcc*> &GetPhiOpnds() const {
    return phiOpnds;
  }

  MapleVector<MePhiOpndOcc*> &GetPhiOpnds() {
    return phiOpnds;
  }

  MePhiOpndOcc *GetPhiOpnd(size_t idx) {
    ASSERT(idx < phiOpnds.size(), "out of range in MePhiOcc::GetPhiOpnd");
    return phiOpnds.at(idx);
  }

  const MePhiOpndOcc *GetPhiOpnd(size_t idx) const {
    ASSERT(idx < phiOpnds.size(), "out of range in MePhiOcc::GetPhiOpnd");
    return phiOpnds.at(idx);
  }

  void AddPhiOpnd(MePhiOpndOcc &opnd) {
    phiOpnds.push_back(&opnd);
  }

  const MeRegPhiNode *GetRegPhi() const {
    return regPhi;
  }

  MeRegPhiNode *GetRegPhi() {
    return regPhi;
  }

  void SetRegPhi(MeRegPhiNode &phi) {
    regPhi = &phi;
  }

  const MeVarPhiNode *GetVarPhi() const {
    return varPhi;
  }

  MeVarPhiNode *GetVarPhi() {
    return varPhi;
  }

  void SetVarPhi(MeVarPhiNode &phi) {
    varPhi = &phi;
  }

  bool IsOpndDefByRealOrInserted() const;
  void Dump(const IRMap&) const;
 private:
  bool isDownSafe;           // default is true
  bool speculativeDownSafe;  // is downsafe due to speculation
  bool isCanBeAvail;
  bool isLater;
  bool isExtraneous;
  bool isRemoved;  // during finalize2, marked this phiocc is removed or not
  MapleVector<MePhiOpndOcc*> phiOpnds;
  MeRegPhiNode *regPhi;  // the reg phi being inserted, maybe can delete it later
  MeVarPhiNode *varPhi;  // the Var phi being inserted, maybe can delete it later
};

// each singly linked list repersents each bucket in workCandHashTable
class PreWorkCand {
 public:
  PreWorkCand(MapleAllocator &alloc, int32 idx, MeExpr *meExpr, PUIdx pIdx)
      : next(nullptr),
        index(idx),
        realOccs(alloc.Adapter()),
        theMeExpr(meExpr),
        puIdx(pIdx),
        hasLocalOpnd(false),
        redo2HandleCritEdges(false),
        needLocalRefVar(false) {
    ASSERT(pIdx != 0, "PreWorkCand: initial puIdx cannot be 0");
  }

  virtual ~PreWorkCand() = default;
  void Dump(const IRMap &irMap) const {
    irMap.GetSSATab().GetModule().GetOut() << "========index: " << index << " has the following occ\n";
    for (MeOccur *occ : realOccs) {
      occ->Dump(irMap);
    }
  }

  void AddRealOccAsLast(MeRealOcc &occ, PUIdx pIdx) {
    realOccs.push_back(&occ);  // add as last
    ASSERT(pIdx != 0, "puIdx of realocc cannot be 0");
    if (pIdx != puIdx) {
      ASSERT(!hasLocalOpnd, "candidate with local opnd cannot have real occurrences in more than one PU");
      puIdx = 0;
    }
  }

  void AddRealOccSorted(const Dominance &dom, MeRealOcc &occ, PUIdx pIdx);
  PrimType GetPrimType() const {
    PrimType pTyp = theMeExpr->GetPrimType();
    CHECK_FATAL(pTyp != kPtyInvalid, "invalid primtype");
    return pTyp;
  }

  static uint32 ComputeWorkCandHashIndex(const MeExpr &meExpr);
  virtual void DumpCand(IRMap &irMap) {
    theMeExpr->Dump(&irMap);
  }

  const PreWorkCand *GetNext() const {
    return next;
  }

  PreWorkCand *GetNext() {
    return next;
  }

  void SetNext(PreWorkCand &workCand) {
    next = &workCand;
  }

  int32 GetIndex() const {
    return index;
  }

  void SetIndex(int idx) {
    index = idx;
  }

  const MapleVector<MeRealOcc*> &GetRealOccs() const {
    return realOccs;
  }

  MapleVector<MeRealOcc*> &GetRealOccs() {
    return realOccs;
  }

  const MeRealOcc *GetRealOcc(size_t idx) const {
    ASSERT(idx < realOccs.size(), "out of range in PreWorkCand::GetRealOccAt");
    return realOccs.at(idx);
  }

  MeRealOcc *GetRealOcc(size_t idx) {
    ASSERT(idx < realOccs.size(), "out of range in PreWorkCand::GetRealOccAt");
    return realOccs.at(idx);
  }

  const MeExpr *GetTheMeExpr() const {
    return theMeExpr;
  }

  MeExpr *GetTheMeExpr() {
    return theMeExpr;
  }

  void SetTheMeExpr(MeExpr &expr) {
    theMeExpr = &expr;
  }

  PUIdx GetPUIdx() const {
    return puIdx;
  }

  void SetPUIdx(PUIdx idx) {
    puIdx = idx;
  }

  bool HasLocalOpnd() const {
    return hasLocalOpnd;
  }

  void SetHasLocalOpnd(bool has) {
    hasLocalOpnd = has;
  }

  bool Redo2HandleCritEdges() const {
    return redo2HandleCritEdges;
  }

  void SetRedo2HandleCritEdges(bool redo) {
    redo2HandleCritEdges = redo;
  }

  bool NeedLocalRefVar() const {
    return needLocalRefVar;
  }

  void SetNeedLocalRefVar(bool need) {
    needLocalRefVar = need;
  }

  static std::array<PreWorkCand*, kWorkCandHashLength> &GetWorkcandHashTable() {
    return workCandHashTable;
  }

  static PreWorkCand *GetWorkcandFromIndex(size_t idx) {
    return workCandHashTable[idx];
  }

  static void SetWorkCandAt(size_t idx, PreWorkCand &workCand) {
    workCandHashTable[idx] = &workCand;
  }

 private:
  PreWorkCand *next;
  int32 index;
  MapleVector<MeRealOcc*> realOccs;  // maintained in order of dt_preorder
  MeExpr *theMeExpr;                 // the expression of this workcand
  PUIdx puIdx;                       // if 0, its occ span multiple PUs; initial value must
  // not be 0; if set to 0, will be stuck at 0
  bool hasLocalOpnd : 1;  // true if any opnd in the expression is local
  // puIdx cannot be 0 if hasLocalOpnd is true
  bool redo2HandleCritEdges : 1;  // redo to make critical edges affect canbevail
  bool needLocalRefVar : 1;       // for the candidate, if necessary to introduce
  // localrefvar in addition to the temp register to for saving the value
  static std::array<PreWorkCand*, kWorkCandHashLength> workCandHashTable;
  void InsertRealOccAt(MeRealOcc &occ, MapleVector<MeRealOcc*>::iterator it, PUIdx pIdx);
};

class PreStmtWorkCand : public PreWorkCand {
 public:
  PreStmtWorkCand(MapleAllocator &alloc, int32 idx, MeStmt &meStmt, PUIdx pIdx)
      : PreWorkCand(alloc, idx, nullptr, pIdx), theMeStmt(&meStmt), lhsIsFinal(false) {}

  virtual ~PreStmtWorkCand() = default;
  static uint32 ComputeStmtWorkCandHashIndex(const MeStmt &stmt);
  void DumpCand(IRMap &irMap) {
    theMeStmt->Dump(&irMap);
  }

  const MeStmt *GetTheMeStmt() const {
    return theMeStmt;
  }

  MeStmt *GetTheMeStmt() {
    return theMeStmt;
  }

  void SetTheMeStmt(MeStmt &stmt) {
    theMeStmt = &stmt;
  }

  bool LHSIsFinal() {
    return lhsIsFinal;
  }

  void SetLHSIsFinal(bool isFinal) {
    lhsIsFinal = isFinal;
  }

 private:
  MeStmt *theMeStmt;  // the statement of this workcand
  bool lhsIsFinal;    // used only if candidate is an assignment
};
}  // namespace maple
#endif  // MAPLE_ME_INCLUDE_OCCUR_H
