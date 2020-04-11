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
#ifndef MAPLE_ME_INCLUDE_BB_H
#define MAPLE_ME_INCLUDE_BB_H
#include "utils.h"
#include "mpl_number.h"
#include "ptr_list_ref.h"
#include "orig_symbol.h"
#include "ver_symbol.h"
#include "ssa.h"

namespace maple {
class MeStmt;  // circular dependency exists, no other choice
class MeVarPhiNode;  // circular dependency exists, no other choice
class MeRegPhiNode;  // circular dependency exists, no other choice
class PiassignMeStmt;  // circular dependency exists, no other choice
class IRMap;  // circular dependency exists, no other choice
enum BBKind {
  kBBUnknown,  // uninitialized
  kBBCondGoto,
  kBBGoto,  // unconditional branch
  kBBFallthru,
  kBBReturn,
  kBBAfterGosub,  // the BB that follows a gosub, as it is an entry point
  kBBSwitch,
  kBBInvalid
};

enum BBAttr : uint32 {
  kBBAttrIsEntry = utils::bit_field_v<1>,
  kBBAttrIsExit = utils::bit_field_v<2>,
  kBBAttrWontExit = utils::bit_field_v<3>,
  kBBAttrIsTry = utils::bit_field_v<4>,
  kBBAttrIsTryEnd = utils::bit_field_v<5>,
  kBBAttrIsJSCatch = utils::bit_field_v<6>,
  kBBAttrIsJSFinally = utils::bit_field_v<7>,
  kBBAttrIsCatch = utils::bit_field_v<8>,
  kBBAttrIsJavaFinally = utils::bit_field_v<9>,
  kBBAttrArtificial = utils::bit_field_v<10>,
  kBBAttrIsInLoop = utils::bit_field_v<11>,
  kBBAttrIsInLoopForEA = utils::bit_field_v<12>,
  kBBAttrIsInstrument = utils::bit_field_v<13>
};

constexpr uint32 kBBVectorInitialSize = 2;
using StmtNodes = PtrListRef<StmtNode>;
using MeStmts = PtrListRef<MeStmt>;

class BB {
 public:
  using BBId = utils::Index<BB>;

  BB(MapleAllocator *alloc, MapleAllocator *versAlloc, BBId id)
      : id(id),
        pred(kBBVectorInitialSize, nullptr, alloc->Adapter()),
        succ(kBBVectorInitialSize, nullptr, alloc->Adapter()),
        succFreq(alloc->Adapter()),
        phiList(versAlloc->Adapter()),
        meVarPhiList(alloc->Adapter()),
        meRegPhiList(alloc->Adapter()),
        meVarPiList(alloc->Adapter()),
        group(this) {
    pred.pop_back();
    pred.pop_back();
    succ.pop_back();
    succ.pop_back();
  }

  BB(MapleAllocator *alloc, MapleAllocator *versAlloc, BBId id, StmtNode *firstStmt, StmtNode *lastStmt)
      : id(id),
        pred(kBBVectorInitialSize, nullptr, alloc->Adapter()),
        succ(kBBVectorInitialSize, nullptr, alloc->Adapter()),
        succFreq(alloc->Adapter()),
        phiList(versAlloc->Adapter()),
        meVarPhiList(alloc->Adapter()),
        meRegPhiList(alloc->Adapter()),
        meVarPiList(alloc->Adapter()),
        stmtNodeList(firstStmt, lastStmt),
        group(this) {
    pred.pop_back();
    pred.pop_back();
    succ.pop_back();
    succ.pop_back();
  }

  virtual ~BB() = default;

  bool GetAttributes(uint32 attrKind) const {
    return (attributes & attrKind) != 0;
  }

  void SetAttributes(uint32 attrKind) {
    attributes |= attrKind;
  }

  void ClearAttributes(uint32 attrKind) {
    attributes &= (~attrKind);
  }

  virtual bool IsGoto() const {
    return kind == kBBGoto;
  }

  virtual bool AddBackEndTry() const {
    return GetAttributes(kBBAttrIsTryEnd);
  }

  void Dump(MIRModule *mod);
  void DumpHeader(MIRModule *mod) const;
  void DumpPhi();
  void DumpBBAttribute(const MIRModule *mod) const;
  std::string StrAttribute() const;

  void AddPredBB(BB *predVal) {
    ASSERT(predVal != nullptr, "null ptr check");
    pred.push_back(predVal);
    predVal->succ.push_back(this);
  }

  // This is to help new bb to keep some flags from original bb after logically splitting.
  void CopyFlagsAfterSplit(const BB &bb) {
    bb.GetAttributes(kBBAttrIsTry) ? SetAttributes(kBBAttrIsTry) : ClearAttributes(kBBAttrIsTry);
    bb.GetAttributes(kBBAttrIsTryEnd) ? SetAttributes(kBBAttrIsTryEnd) : ClearAttributes(kBBAttrIsTryEnd);
    bb.GetAttributes(kBBAttrIsExit) ? SetAttributes(kBBAttrIsExit) : ClearAttributes(kBBAttrIsExit);
    bb.GetAttributes(kBBAttrWontExit) ? SetAttributes(kBBAttrWontExit) : ClearAttributes(kBBAttrWontExit);
  }

  BBId GetBBId() const {
    return id;
  }

  void SetBBId(BBId idx) {
    id = idx;
  }

  uint32 UintID() const {
    return static_cast<uint32>(id);
  }

  StmtNode *GetTheOnlyStmtNode();
  bool IsEmpty() const {
    return stmtNodeList.empty();
  }

  void SetFirst(StmtNode *stmt) {
    stmtNodeList.update_front(stmt);
  }

  void SetLast(StmtNode *stmt) {
    stmtNodeList.update_back(stmt);
  }

  // should test IsEmpty first
  StmtNode &GetFirst() {
    return stmtNodeList.front();
  }
  // should test IsEmpty first
  const StmtNode &GetFirst() const {
    return stmtNodeList.front();
  }

  // should test IsEmpty first
  StmtNode &GetLast() {
    return stmtNodeList.back();
  }
  // should test IsEmpty first
  const StmtNode &GetLast() const {
    return stmtNodeList.back();
  }

  void SetFirstMe(MeStmt *stmt);
  void SetLastMe(MeStmt *stmt);
  MeStmt *GetLastMe();
  bool IsInList(const MapleVector<BB*> &bbList) const;
  bool IsPredBB(const BB &bb) const {
    // if this is a pred of bb return true;
    // otherwise return false;
    return IsInList(bb.pred);
  }

  bool IsSuccBB(const BB &bb) const {
    return IsInList(bb.succ);
  }
  void DumpMeBB(const IRMap &irMap);

  void AddSuccBB(BB *succPara) {
    succ.push_back(succPara);
    succPara->pred.push_back(this);
  }

  void ReplacePred(const BB *old, BB *newPred);
  void ReplaceSucc(const BB *old, BB *newSucc);
  void ReplaceSuccOfCommonEntryBB(const BB *old, BB *newSucc);
  void AddStmtNode(StmtNode *stmt);
  void PrependStmtNode(StmtNode *stmt);
  void RemoveStmtNode(StmtNode *stmt);
  void RemoveLastStmt();
  void InsertStmtBefore(StmtNode *stmt, StmtNode *newStmt);
  void ReplaceStmt(StmtNode *stmt, StmtNode *newStmt);
  int RemoveBBFromVector(MapleVector<BB*> &bbVec) const;
  void RemoveBBFromPred(BB *bb);
  void RemoveBBFromSucc(BB *bb);

  void RemovePred(BB *predBB) {
    predBB->RemoveBBFromSucc(this);
    RemoveBBFromPred(predBB);
  }

  void RemoveSucc(BB *succBB) {
    succBB->RemoveBBFromPred(this);
    RemoveBBFromSucc(succBB);
  }

  void InsertPi(BB &bb, PiassignMeStmt &s) {
    if (meVarPiList.find(&bb) == meVarPiList.end()) {
      std::vector<PiassignMeStmt*> tmp;
      meVarPiList[&bb] = tmp;
    }
    meVarPiList[&bb].push_back(&s);
  }

  MapleMap<BB*, std::vector<PiassignMeStmt*>> &GetPiList() {
    return meVarPiList;
  }

  bool IsMeStmtEmpty() const {
    return meStmtList.empty();
  }

  void FindReachableBBs(std::vector<bool> &visitedBBs) const;
  void FindWillExitBBs(std::vector<bool> &visitedBBs) const;
  const PhiNode *PhiofVerStInserted(const VersionSt &versionSt) const;
  void InsertPhi(MapleAllocator *alloc, VersionSt *versionSt);
  void PrependMeStmt(MeStmt *meStmt);
  void RemoveMeStmt(const MeStmt *meStmt);
  void AddMeStmtFirst(MeStmt *meStmt);
  void AddMeStmtLast(MeStmt *meStmt);
  void InsertMeStmtBefore(const MeStmt *meStmt, MeStmt *inStmt);
  void InsertMeStmtAfter(const MeStmt *meStmt, MeStmt *inStmt);
  void InsertMeStmtLastBr(MeStmt *inStmt);
  void ReplaceMeStmt(const MeStmt *stmt, MeStmt *newStmt);
  void DumpMeVarPhiList(const IRMap *irMap);
  void DumpMeRegPhiList(const IRMap *irMap);
  void DumpMeVarPiList(const IRMap *irMap);
  StmtNodes &GetStmtNodes() {
    return stmtNodeList;
  }

  const StmtNodes &GetStmtNodes() const {
    return stmtNodeList;
  }

  MeStmts &GetMeStmts() {
    return meStmtList;
  }

  const MeStmts &GetMeStmts() const {
    return meStmtList;
  }

  LabelIdx GetBBLabel() const {
    return bbLabel;
  }

  void SetBBLabel(LabelIdx idx) {
    bbLabel = idx;
  }

  uint32 GetFrequency() const {
    return frequency;
  }

  void SetFrequency(uint32 f) {
    frequency = f;
  }

  BBKind GetKind() const {
    return kind;
  }

  void SetKind(BBKind bbKind) {
    kind = bbKind;
  }

  void SetKindReturn() {
    SetKind(kBBReturn);
    SetAttributes(kBBAttrIsExit);
  }

  MapleVector<BB*> &GetPred() {
    return pred;
  }

  const MapleVector<BB*> &GetPred() const {
    return pred;
  }

  const BB *GetPred(size_t cnt) const {
    ASSERT(cnt < pred.size(), "out of range in BB::GetPred");
    return pred.at(cnt);
  }

  BB *GetPred(size_t cnt) {
    ASSERT(cnt < pred.size(), "out of range in BB::GetPred");
    return pred.at(cnt);
  }

  void SetPred(size_t cnt, BB *pp) {
    CHECK_FATAL(cnt < pred.size(), "out of range in BB::SetPred");
    pred[cnt] = pp;
  }

  MapleVector<BB*> &GetSucc() {
    return succ;
  }

  const MapleVector<BB*> &GetSucc() const {
    return succ;
  }

  const BB *GetSucc(size_t cnt) const {
    ASSERT(cnt < succ.size(), "out of range in BB::GetSucc");
    return succ.at(cnt);
  }

  BB *GetSucc(size_t cnt) {
    ASSERT(cnt < succ.size(), "out of range in BB::GetSucc");
    return succ.at(cnt);
  }

  void SetSucc(size_t cnt, BB *ss) {
    CHECK_FATAL(cnt < succ.size(), "out of range in BB::SetSucc");
    succ[cnt] = ss;
  }

  const MapleMap<const OriginalSt*, PhiNode> &GetPhiList() const {
    return phiList;
  }
  MapleMap<const OriginalSt*, PhiNode> &GetPhiList() {
    return phiList;
  }
  void ClearPhiList() {
    phiList.clear();
  }

  MapleMap<OStIdx, MeVarPhiNode*> &GetMevarPhiList() {
    return meVarPhiList;
  }

  const MapleMap<OStIdx, MeVarPhiNode*> &GetMevarPhiList() const {
    return meVarPhiList;
  }

  MapleMap<OStIdx, MeRegPhiNode*> &GetMeRegPhiList() {
    return meRegPhiList;
  }

  const MapleMap<OStIdx, MeRegPhiNode*> &GetMeRegPhiList() const {
    return meRegPhiList;
  }

  uint64 GetEdgeFreq(const BB *bb) const {
    auto edgeFreq = succFreq.find(bb);
    CHECK_FATAL(edgeFreq != succFreq.end(), "bb's edge freq must be set before use");
    return edgeFreq->second;
  }

  void SetEdgeFreq(const BB *bb, uint64 freq) {
    auto iter = std::find(succ.begin(), succ.end(), bb);
    CHECK_FATAL(iter != std::end(succ), "%d is not the successor of %d", bb->UintID(), this->UintID());
    succFreq[bb] = freq;
  }

  BB* GetGroup() const {
    return group;
  }

  void SetGroup(BB *g) {
    group = g;
  }

  void ClearGroup() {
    group = this;
  }
 private:
  BBId id;
  LabelIdx bbLabel = 0;       // the BB's label
  MapleVector<BB*> pred;  // predecessor list
  MapleVector<BB*> succ;  // successor list
  // record the edge freq from curBB to succ BB
  MapleMap<const BB*, uint64> succFreq;
  MapleMap<const OriginalSt*, PhiNode> phiList;
  MapleMap<OStIdx, MeVarPhiNode*> meVarPhiList;
  MapleMap<OStIdx, MeRegPhiNode*> meRegPhiList;
  MapleMap<BB*, std::vector<PiassignMeStmt*>> meVarPiList;
  uint32 frequency = 0;
  BBKind kind = kBBUnknown;
  uint32 attributes = 0;
  StmtNodes stmtNodeList;
  MeStmts meStmtList;
  BB *group;
};

using BBId = BB::BBId;

class SCCOfBBs {
 public:
  SCCOfBBs(uint32 index, BB *bb, MapleAllocator *alloc)
      : id(index),
        entry(bb),
        bbs(alloc->Adapter()),
        predSCC(std::less<SCCOfBBs*>(), alloc->Adapter()),
        succSCC(std::less<SCCOfBBs*>(), alloc->Adapter()) {}
  ~SCCOfBBs() = default;
  void Dump();
  void Verify(MapleVector<SCCOfBBs*> &sccOfBB);
  void SetUp(MapleVector<SCCOfBBs*> &sccOfBB);
  bool HasCycle() const;
  void AddBBNode(BB *bb) {
    bbs.push_back(bb);
  }
  void Clear() {
    bbs.clear();
  }
  uint32 GetID() const {
    return id;
  }
  const MapleVector<BB*> &GetBBs() const {
    return bbs;
  }
  const MapleSet<SCCOfBBs*> &GetSucc() const {
    return succSCC;
  }
  const MapleSet<SCCOfBBs*> &GetPred() const {
    return predSCC;
  }
  bool HasPred() const {
    return !predSCC.empty();
  }
  BB *GetEntry() {
    return entry;
  }
 private:
  uint32 id;
  BB *entry;
  MapleVector<BB*> bbs;
  MapleSet<SCCOfBBs*> predSCC;
  MapleSet<SCCOfBBs*> succSCC;
};

bool ControlFlowInInfiniteLoop(const BB &bb, Opcode opcode);
}  // namespace maple

namespace std {
template <>
struct hash<maple::BBId> {
  size_t operator()(const maple::BBId &x) const {
    return x;
  }
};
}  // namespace std
#endif  // MAPLE_ME_INCLUDE_BB_H
