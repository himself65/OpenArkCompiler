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
#ifndef MAPLE_ME_INCLUDE_ME_IR_H
#define MAPLE_ME_INCLUDE_ME_IR_H
#include <array>
#include "orig_symbol.h"
#include "bb.h"

namespace maple {
class PhiNode;    // circular dependency exists, no other choice
class MeStmt;     // circular dependency exists, no other choice
class IRMap;      // circular dependency exists, no other choice
class SSATab;     // circular dependency exists, no other choice
class VarMeExpr;  // circular dependency exists, no other choice
class RegMeExpr;  // circular dependency exists, no other choice
class Dominance;  // circular dependency exists, no other choice
using MeStmtPtr = MeStmt*;
enum MeExprOp : uint8 {
  kMeOpUnknown,
  kMeOpVar,
  kMeOpIvar,
  kMeOpAddrof,
  kMeOpAddroffunc,
  kMeOpGcmalloc,
  kMeOpReg,
  kMeOpConst,
  kMeOpConststr,
  kMeOpConststr16,
  kMeOpSizeoftype,
  kMeOpFieldsDist,
  kMeOpOp,
  kMeOpNary
};  // cache the op to avoid dynamic cast

constexpr int kInvalidExprID = -1;
class MeExpr {
 public:
  MeExpr(int32 exprId, MeExprOp op)
      : meOp(op), exprID(exprId) {}

  virtual ~MeExpr() = default;

  void SetOp(Opcode opcode) {
    op = opcode;
  }

  void SetPtyp(PrimType primTypeVal) {
    primType = primTypeVal;
  }

  void SetNumOpnds(uint8 val) {
    numOpnds = val;
  }

  void SetTreeID(uint32 val) {
    treeID = val;
  }

  void SetNext(MeExpr *nextPara) {
    next = nextPara;
  }

  Opcode GetOp() const {
    return op;
  }

  PrimType GetPrimType() const {
    return primType;
  }

  uint8 GetNumOpnds() const {
    return numOpnds;
  }

  MeExprOp GetMeOp() const {
    return meOp;
  }

  int32 GetExprID() const {
    return exprID;
  }

  uint8 GetDepth() const {
    return depth;
  }

  uint32 GetTreeID() const {
    return treeID;
  }

  MeExpr *GetNext() const {
    return next;
  }

  void InitBase(Opcode opcode, PrimType primaryType, size_t opndsNum) {
    this->op = opcode;
    this->primType = primaryType;
    this->numOpnds = static_cast<uint8>(opndsNum);
  }

  virtual void Dump(const IRMap*, int32 indent = 0) const {
    (void)indent;
  }

  virtual bool IsZero() const {
    return false;
  }

  virtual bool IsUseSameSymbol(const MeExpr &expr) const {
    return exprID == expr.exprID;
  }

  virtual BaseNode &EmitExpr(SSATab&) = 0;
  bool IsLeaf() const {
    return numOpnds == 0;
  }

  bool IsGcmalloc() const {
    return op == OP_gcmalloc || op == OP_gcmallocjarray || op == OP_gcpermalloc || op == OP_gcpermallocjarray;
  }

  virtual bool IsVolatile(const SSATab&) const {
    return false;
  }

  bool IsTheSameWorkcand(const MeExpr&) const;
  virtual void SetDefByStmt(MeStmt&) {}

  virtual MeExpr *GetOpnd(size_t) const {
    return nullptr;
  }

  virtual void SetOpnd(size_t, MeExpr*) {
    ASSERT(false, "Should not reach here");
  }

  virtual MeExpr *GetIdenticalExpr(MeExpr&) const {
    ASSERT(false, "Should not reach here");
    return nullptr;
  }

  void UpdateDepth();                      // update the depth, suppose all sub nodes have already depth done.
  MeExpr &GetAddrExprBase();               // get the base of the address expression
  // in the expression; nullptr otherwise
  bool SymAppears(OStIdx oidx);  // check if symbol appears in the expression
  bool HasIvar() const;          // whether there is any iread node in the expression
  bool Pure() const {
    return !kOpcodeInfo.NotPure(op);
  }

  virtual bool IsSameVariableValue(const VarMeExpr&) const;
  MeExpr *ResolveMeExprValue();
  bool CouldThrowException() const;
  bool IsAllOpndsIdentical(const MeExpr &meExpr) const;
  bool PointsToSomethingThatNeedsIncRef();
  virtual uint32 GetHashIndex() const {
    return 0;
  }

 private:
  MeExpr *FindSymAppearance(OStIdx oidx);  // find the appearance of the symbol

  Opcode op = OP_undef;
  PrimType primType = kPtyInvalid;
  uint8 numOpnds = 0;
  MeExprOp meOp;
  int32 exprID;
  uint8 depth = 0;
  uint32 treeID = 0;  // for bookkeeping purpose during SSAPRE
  MeExpr *next = nullptr;
};

enum MeDefBy {
  kDefByNo,
  kDefByStmt,
  kDefByPhi,
  kDefByChi,
  kDefByMustDef  // only applies to callassigned and its siblings
};

class ChiMeNode;      // circular dependency exists, no other choice
class MustDefMeNode;  // circular dependency exists, no other choice
class IassignMeStmt;  // circular dependency exists, no other choice

// represant dread
class VarMeExpr final : public MeExpr {
 public:
  VarMeExpr(MapleAllocator *alloc, int32 exprid, OStIdx oidx, size_t vidx)
      : MeExpr(exprid, kMeOpVar),
        ostIdx(oidx),
        vstIdx(vidx),
        inferredTypeCandidates(alloc->Adapter()) {}

  ~VarMeExpr() = default;

  void Dump(const IRMap*, int32 indent = 0) const override;
  bool IsUseSameSymbol(const MeExpr&) const override;
  BaseNode &EmitExpr(SSATab&) override;
  bool IsValidVerIdx(const SSATab &ssaTab) const;
  void SetDefByStmt(MeStmt &defStmt) override {
    defBy = kDefByStmt;
    def.defStmt = &defStmt;
  }

  MeVarPhiNode *GetMeVarPhiDef() const {
    return IsDefByPhi() ? def.defPhi : nullptr;
  }

  BB *DefByBB();
  bool IsVolatile(const SSATab&) const override;
  // indicate if the variable is local variable but not a function formal variable
  bool IsPureLocal(const SSATab&, const MIRFunction&) const;
  bool IsZeroVersion(const SSATab&) const;
  BB *GetDefByBBMeStmt(const Dominance&, MeStmtPtr&) const;
  bool IsSameVariableValue(const VarMeExpr&) const override;
  VarMeExpr &ResolveVarMeValue();
  bool PointsToStringLiteral();

  const OStIdx &GetOStIdx() const {
    return ostIdx;
  }

  size_t GetVstIdx() const {
    return vstIdx;
  }

  void SetVstIdx(size_t vstIdxVal) {
    vstIdx = vstIdxVal;
  }

  FieldID GetFieldID() const {
    return fieldID;
  }

  void SetFieldID(FieldID fieldIDVal) {
    fieldID = fieldIDVal;
  }

  TyIdx GetInferredTyIdx() const {
    return inferredTyIdx;
  }

  void SetInferredTyIdx(TyIdx inferredTyIdxVal) {
    inferredTyIdx = inferredTyIdxVal;
  }

  const MapleVector<TyIdx> &GetInferredTypeCandidates() const {
    return inferredTypeCandidates;
  }

  void AddInferredTypeCandidate(TyIdx idx) {
    inferredTypeCandidates.push_back(idx);
  }

  void ClearInferredTypeCandidates() {
    inferredTypeCandidates.clear();
  }

  MeDefBy GetDefBy() const {
    return defBy;
  }

  void SetDefBy(MeDefBy defByVal) {
    defBy = defByVal;
  }

  bool GetMaybeNull() const {
    return maybeNull;
  }

  void SetMaybeNull(bool maybeNullVal) {
    maybeNull = maybeNullVal;
  }

  bool GetNoDelegateRC() const {
    return noDelegateRC;
  }

  void SetNoDelegateRC(bool noDelegateRCVal) {
    noDelegateRC = noDelegateRCVal;
  }

  MeStmt *GetDefStmt() const {
    return def.defStmt;
  }

  void SetDefStmt(MeStmt *defStmt) {
    def.defStmt = defStmt;
  }

  MeVarPhiNode &GetDefPhi() {
    return *def.defPhi;
  }

  const MeVarPhiNode &GetDefPhi() const {
    return *def.defPhi;
  }

  void SetDefPhi(MeVarPhiNode &defPhi) {
    def.defPhi = &defPhi;
  }

  ChiMeNode &GetDefChi() {
    return *def.defChi;
  }

  const ChiMeNode &GetDefChi() const {
    return *def.defChi;
  }

  void SetDefChi(ChiMeNode &defChi) {
    def.defChi = &defChi;
  }

  MustDefMeNode &GetDefMustDef() {
    return *def.defMustDef;
  }

  const MustDefMeNode &GetDefMustDef() const {
    return *def.defMustDef;
  }

  void SetDefMustDef(MustDefMeNode &defMustDef) {
    def.defMustDef = &defMustDef;
  }

 private:
  bool IsDefByPhi() const {
    return defBy == kDefByPhi;
  }
  bool noDelegateRC = false;  // true if this cannot be optimized by delegaterc
  union {
    MeStmt *defStmt = nullptr;            // definition stmt of this var
    MeVarPhiNode *defPhi;
    ChiMeNode *defChi;          // definition node by Chi
    MustDefMeNode *defMustDef;  // definition by callassigned
  } def;

  OStIdx ostIdx;  // the index in MEOptimizer's OriginalStTable;
  size_t vstIdx;  // the index in MEOptimizer's VersionStTable, 0 if not in VersionStTable
  FieldID fieldID = 0;
  TyIdx inferredTyIdx{ 0 }; /* Non zero if it has a known type (allocation type is seen). */
  MapleVector<TyIdx> inferredTypeCandidates;
  MeDefBy defBy = kDefByNo;
  bool maybeNull = true;  // false if definitely not null
};

class MeVarPhiNode {
 public:
  explicit MeVarPhiNode(MapleAllocator *alloc)
      : opnds(kOperandNumBinary, nullptr, alloc->Adapter()) {
    opnds.pop_back();
    opnds.pop_back();
  }

  MeVarPhiNode(VarMeExpr *var, MapleAllocator *alloc)
      : lhs(var), opnds(kOperandNumBinary, nullptr, alloc->Adapter()) {
    var->SetDefPhi(*this);
    var->SetDefBy(kDefByPhi);
    opnds.pop_back();
    opnds.pop_back();
  }

  ~MeVarPhiNode() = default;

  void UpdateLHS(VarMeExpr &var) {
    lhs = &var;
    var.SetDefBy(kDefByPhi);
    var.SetDefPhi(*this);
  }

  bool IsPureLocal(const SSATab &ssaTab, const MIRFunction &mirFunc);
  void Dump(const IRMap *irMap) const;

  VarMeExpr *GetOpnd(size_t idx) const {
    ASSERT(idx < opnds.size(), "out of range in MeVarPhiNode::GetOpnd");
    return opnds.at(idx);
  }

  void SetOpnd(size_t idx, VarMeExpr *opnd) {
    CHECK_FATAL(idx < opnds.size(), "out of range in MeVarPhiNode::SetOpnd");
    opnds[idx] = opnd;
  }

  MapleVector<VarMeExpr*> &GetOpnds() {
    return opnds;
  }

  void SetIsLive(bool isLiveVal) {
    isLive = isLiveVal;
  }

  bool GetIsLive() {
    return isLive;
  }

  void SetDefBB(BB *defBBVal) {
    defBB = defBBVal;
  }

  BB *GetDefBB() {
    return defBB;
  }

  const BB *GetDefBB() const {
    return defBB;
  }

  VarMeExpr *GetLHS() {
    return lhs;
  }

  void SetLHS(VarMeExpr *value) {
    lhs = value;
  }

  void SetPiAdded() {
    isPiAdded = true;
  }

  bool IsPiAdded() const {
    return isPiAdded;
  }

 private:
  VarMeExpr *lhs = nullptr;
  MapleVector<VarMeExpr*> opnds;
  bool isLive = true;
  BB *defBB = nullptr;  // the bb that defines this phi
  bool isPiAdded = false;
};

class RegMeExpr : public MeExpr {
 public:
  RegMeExpr(MapleAllocator *alloc, int32 exprid, PregIdx preg, PUIdx pidx, OStIdx oidx, uint32 vidx)
      : MeExpr(exprid, kMeOpReg),
        regIdx(preg),
        puIdx(pidx),
        ostIdx(oidx),
        vstIdx(vidx),
        phiUseSet(std::less<MeRegPhiNode*>(), alloc->Adapter()) {}

  ~RegMeExpr() = default;

  void Dump(const IRMap*, int32 indent = 0) const override;
  BaseNode &EmitExpr(SSATab&) override;
  void SetDefByStmt(MeStmt &defStmt) override {
    defBy = kDefByStmt;
    def.defStmt = &defStmt;
  }

  bool IsDefByPhi() const {
    return defBy == kDefByPhi;
  }

  MeRegPhiNode *GetMeRegPhiDef() const {
    return IsDefByPhi() ? def.defPhi : nullptr;
  }

  bool IsSameVariableValue(const VarMeExpr&) const override;

  bool IsUseSameSymbol(const MeExpr&) const override;
  BB *DefByBB();
  RegMeExpr *FindDefByStmt(std::set<RegMeExpr*> &visited);

  PregIdx16 GetRegIdx() const {
    return regIdx;
  }

  void SetRegIdx(PregIdx16 regIdxVal) {
    regIdx = regIdxVal;
  }

  MeDefBy GetDefBy() const {
    return defBy;
  }

  void SetDefBy(MeDefBy defByVal) {
    defBy = defByVal;
  }

  PUIdx GetPuIdx() const {
    return puIdx;
  }

  OStIdx GetOstIdx() const {
    return ostIdx;
  }

  MapleSet<MeRegPhiNode*> &GetPhiUseSet() {
    return phiUseSet;
  }

  MeStmt *GetDefStmt() const {
    return def.defStmt;
  }

  MeRegPhiNode &GetDefPhi() {
    return *def.defPhi;
  }

  const MeRegPhiNode &GetDefPhi() const {
    return *def.defPhi;
  }

  MustDefMeNode &GetDefMustDef() {
    return *def.defMustDef;
  }

  const MustDefMeNode &GetDefMustDef() const {
    return *def.defMustDef;
  }

  void SetDefStmt(MeStmt *defStmtVal) {
    def.defStmt = defStmtVal;
  }

  void SetDefPhi(MeRegPhiNode &defPhiVal) {
    def.defPhi = &defPhiVal;
  }

  void SetDefMustDef(MustDefMeNode &defMustDefVal) {
    def.defMustDef = &defMustDefVal;
  }

  bool IsNormalReg() const {
    return regIdx >= 0;
  }

 private:
  union {
    MeStmt *defStmt = nullptr;
    MeRegPhiNode *defPhi;
    MustDefMeNode *defMustDef;  // definition by callassigned
  } def;
  PregIdx16 regIdx;
  MeDefBy defBy = kDefByNo;
  bool recursivePtr = false;  // if pointer to recursive data structures;
  PUIdx puIdx;
  OStIdx ostIdx;                      // the index in MEOptimizer's OriginalStTable;
  uint32 vstIdx;                      // the index in MEOptimizer's VersionStTable, 0 if not in VersionStTable
  MapleSet<MeRegPhiNode*> phiUseSet;  // the use set of this reg node, used by preg renamer
};

class MeRegPhiNode {
 public:
  explicit MeRegPhiNode(MapleAllocator *alloc)
      : opnds(kOperandNumBinary, nullptr, alloc->Adapter()) {
    opnds.pop_back();
    opnds.pop_back();
  }

  ~MeRegPhiNode() = default;

  void UpdateLHS(RegMeExpr &reg) {
    lhs = &reg;
    reg.SetDefBy(kDefByPhi);
    reg.SetDefPhi(*this);
  }

  void Dump(const IRMap *irMap) const;

  RegMeExpr *GetLHS() {
    return lhs;
  }

  void SetLHS(RegMeExpr *lhsVal) {
    lhs = lhsVal;
  }

  MapleVector<RegMeExpr*> &GetOpnds() {
    return opnds;
  }

  RegMeExpr *GetOpnd(size_t idx) const {
    CHECK_FATAL(idx < opnds.size(), "out of range in MeRegPhiNode::GetOpnd");
    return opnds[idx];
  }

  void SetOpnd(size_t idx, RegMeExpr *opnd) {
    CHECK_FATAL(idx < opnds.size(), "out of range in MeRegPhiNode::SetOpnd");
    opnds[idx] = opnd;
  }

  void SetIsLive(bool isLiveVal) {
    isLive = isLiveVal;
  }

  bool GetIsLive() const {
    return isLive;
  }

  void SetDefBB(BB *defBBVal) {
    defBB = defBBVal;
  }

  BB *GetDefBB() const {
    return defBB;
  }

 private:
  RegMeExpr *lhs = nullptr;
  MapleVector<RegMeExpr*> opnds;
  bool isLive = true;
  BB *defBB = nullptr;  // the bb that defines this phi
};

class ConstMeExpr : public MeExpr {
 public:
  ConstMeExpr(int32 exprID, MIRConst *constValParam) : MeExpr(exprID, kMeOpConst), constVal(constValParam) {}
  ~ConstMeExpr() = default;

  void Dump(const IRMap*, int32 indent = 0) const override;
  BaseNode &EmitExpr(SSATab &) override;
  bool GeZero() const;
  bool GtZero() const;
  bool IsZero() const override;
  bool IsOne() const;
  int64 GetIntValue() const;

  MIRConst *GetConstVal() {
    return constVal;
  }

  MeExpr *GetIdenticalExpr(MeExpr &expr) const override;

  uint32 GetHashIndex() const override {
    CHECK_FATAL(constVal != nullptr, "constVal is null");
    if (constVal->GetKind() == kConstInt) {
      auto *intConst = safe_cast<MIRIntConst>(constVal);
      CHECK_NULL_FATAL(intConst);
      return intConst->GetValue();
    }
    if (constVal->GetKind() == kConstFloatConst) {
      auto *floatConst = safe_cast<MIRFloatConst>(constVal);
      CHECK_NULL_FATAL(floatConst);
      return floatConst->GetIntValue();
    }
    if (constVal->GetKind() == kConstDoubleConst) {
      auto *doubleConst = safe_cast<MIRDoubleConst>(constVal);
      CHECK_NULL_FATAL(doubleConst);
      return doubleConst->GetIntValue();
    }
    if (constVal->GetKind() == kConstLblConst) {
      auto *lblConst = safe_cast<MIRLblConst>(constVal);
      CHECK_NULL_FATAL(lblConst);
      return lblConst->GetValue();
    }
    ASSERT(false, "ComputeHash: const type not yet implemented");
    return 0;
  }

 private:
  MIRConst *constVal;
};

class ConststrMeExpr : public MeExpr {
 public:
  ConststrMeExpr(int32 exprID, UStrIdx idx) : MeExpr(exprID, kMeOpConststr), strIdx(idx) {}

  ~ConststrMeExpr() = default;

  void Dump(const IRMap*, int32 indent = 0) const override;
  BaseNode &EmitExpr(SSATab&) override;
  MeExpr *GetIdenticalExpr(MeExpr &expr) const override;

  UStrIdx GetStrIdx() const {
    return strIdx;
  }

  uint32 GetHashIndex() const override {
    constexpr uint32 kConststrHashShift = 6;
    return static_cast<uint32>(strIdx) << kConststrHashShift;
  }

 private:
  UStrIdx strIdx;
};

class Conststr16MeExpr : public MeExpr {
 public:
  Conststr16MeExpr(int32 exprID, U16StrIdx idx) : MeExpr(exprID, kMeOpConststr16), strIdx(idx) {}
  ~Conststr16MeExpr() = default;

  void Dump(const IRMap*, int32 indent = 0) const override;
  BaseNode &EmitExpr(SSATab&) override;
  MeExpr *GetIdenticalExpr(MeExpr &expr) const override;

  U16StrIdx GetStrIdx() {
    return strIdx;
  }

  uint32 GetHashIndex() const override {
    constexpr uint32 kConststr16HashShift = 6;
    return static_cast<uint32>(strIdx) << kConststr16HashShift;
  }

 private:
  U16StrIdx strIdx;
};

class SizeoftypeMeExpr : public MeExpr {
 public:
  SizeoftypeMeExpr(int32 exprid, TyIdx idx) : MeExpr(exprid, kMeOpSizeoftype), tyIdx(idx) {}
  ~SizeoftypeMeExpr() = default;

  void Dump(const IRMap*, int32 indent = 0) const override;
  BaseNode &EmitExpr(SSATab&) override;
  MeExpr *GetIdenticalExpr(MeExpr &expr) const override;

  TyIdx GetTyIdx() const {
    return tyIdx;
  }

  uint32 GetHashIndex() const override {
    constexpr uint32 sizeoftypeHashShift = 5;
    return static_cast<uint32>(tyIdx) << sizeoftypeHashShift;
  }

 private:
  TyIdx tyIdx;
};

class FieldsDistMeExpr : public MeExpr {
 public:
  FieldsDistMeExpr(int32 exprid, TyIdx idx, FieldID f1, FieldID f2)
      : MeExpr(exprid, kMeOpFieldsDist), tyIdx(idx), fieldID1(f1), fieldID2(f2) {}

  ~FieldsDistMeExpr() = default;
  void Dump(const IRMap*, int32 indent = 0) const override;
  BaseNode &EmitExpr(SSATab&) override;
  MeExpr *GetIdenticalExpr(MeExpr &expr) const override;

  TyIdx GetTyIdx() const {
    return tyIdx;
  }

  FieldID GetFieldID1() {
    return fieldID1;
  }

  FieldID GetFieldID2() {
    return fieldID2;
  }

  uint32 GetHashIndex() const override {
    constexpr uint32 kFieldsDistHashShift = 5;
    constexpr uint32 kTyIdxShiftFactor = 10;
    return (static_cast<uint32>(tyIdx) << kTyIdxShiftFactor) +
           (static_cast<uint32>(fieldID1) << kFieldsDistHashShift) + fieldID2;
  }

 private:
  TyIdx tyIdx;
  FieldID fieldID1;
  FieldID fieldID2;
};

class AddrofMeExpr : public MeExpr {
 public:
  AddrofMeExpr(int32 exprid, OStIdx idx) : MeExpr(exprid, kMeOpAddrof), ostIdx(idx), fieldID(0) {}

  ~AddrofMeExpr() = default;

  void Dump(const IRMap*, int32 indent = 0) const override;
  bool IsUseSameSymbol(const MeExpr&) const override;
  BaseNode &EmitExpr(SSATab&) override;
  MeExpr *GetIdenticalExpr(MeExpr &expr) const override;

  OStIdx GetOstIdx() const {
    return ostIdx;
  }

  FieldID GetFieldID() {
    return fieldID;
  }

  void SetFieldID(FieldID fieldIDVal) {
    fieldID = fieldIDVal;
  }

  uint32 GetHashIndex() const override {
    constexpr uint32 addrofHashShift = 4;
    return static_cast<uint32>(ostIdx) << addrofHashShift;
  }

 private:
  OStIdx ostIdx;  // the index in MEOptimizer: OriginalStTable;
  FieldID fieldID;
};

class AddroffuncMeExpr : public MeExpr {
 public:
  AddroffuncMeExpr(int32 exprID, PUIdx puIdx) : MeExpr(exprID, kMeOpAddroffunc), puIdx(puIdx) {}

  ~AddroffuncMeExpr() = default;

  void Dump(const IRMap*, int32 indent = 0) const override;
  BaseNode &EmitExpr(SSATab&) override;
  MeExpr *GetIdenticalExpr(MeExpr &expr) const override;

  PUIdx GetPuIdx() const {
    return puIdx;
  }

  uint32 GetHashIndex() const override {
    constexpr uint32 addroffuncHashShift = 5;
    return puIdx << addroffuncHashShift;
  }

 private:
  PUIdx puIdx;
};

class GcmallocMeExpr : public MeExpr {
 public:
  GcmallocMeExpr(int32 exprid, TyIdx tyid) : MeExpr(exprid, kMeOpGcmalloc), tyIdx(tyid) {}

  ~GcmallocMeExpr() = default;

  void Dump(const IRMap*, int32 indent = 0) const;
  BaseNode &EmitExpr(SSATab&);

  TyIdx GetTyIdx() const {
    return tyIdx;
  }

  uint32 GetHashIndex() const {
    constexpr uint32 kGcmallocHashShift = 4;
    return static_cast<uint32>(tyIdx) << kGcmallocHashShift;
  }

 private:
  TyIdx tyIdx;
};

class OpMeExpr : public MeExpr {
 public:
  explicit OpMeExpr(int32 exprID) : MeExpr(exprID, kMeOpOp), tyIdx(TyIdx(0)) {}

  OpMeExpr(const OpMeExpr &opMeExpr, int32 exprID)
      : MeExpr(exprID, kMeOpOp),
        opnds(opMeExpr.opnds),
        opndType(opMeExpr.opndType),
        bitsOffset(opMeExpr.bitsOffset),
        bitsSize(opMeExpr.bitsSize),
        tyIdx(opMeExpr.tyIdx),
        fieldID(opMeExpr.fieldID) {
    InitBase(opMeExpr.GetOp(), opMeExpr.GetPrimType(), opMeExpr.GetNumOpnds());
  }

  ~OpMeExpr() = default;

  OpMeExpr(const OpMeExpr&) = delete;
  OpMeExpr &operator=(const OpMeExpr&) = delete;

  bool IsIdentical(const OpMeExpr &meexpr) const;
  bool IsAllOpndsIdentical(const OpMeExpr &meExpr) const;
  bool IsCompareIdentical(const OpMeExpr &meExpr) const;
  void Dump(const IRMap*, int32 indent = 0) const override;
  bool IsUseSameSymbol(const MeExpr&) const override;
  MeExpr *GetIdenticalExpr(MeExpr &expr) const override;
  BaseNode &EmitExpr(SSATab&) override;
  MeExpr *GetOpnd(size_t i) const override {
    CHECK_FATAL(i < kOperandNumTernary, "OpMeExpr cannot have more than 3 operands");
    return opnds[i];
  }

  void SetOpnd(size_t idx, MeExpr *opndsVal) override {
    CHECK_FATAL(idx < kOperandNumTernary, "out of range in  OpMeExpr::SetOpnd");
    opnds[idx] = opndsVal;
  }

  PrimType GetOpndType() {
    return opndType;
  }

  PrimType GetOpndType() const {
    return opndType;
  }

  void SetOpndType(PrimType opndTypeVal) {
    opndType = opndTypeVal;
  }

  uint8 GetBitsOffSet() {
    return bitsOffset;
  }

  void SetBitsOffSet(uint8 bitsOffSetVal) {
    bitsOffset = bitsOffSetVal;
  }

  uint8 GetBitsSize() const {
    return bitsSize;
  }

  void SetBitsSize(uint8 bitsSizeVal) {
    bitsSize = bitsSizeVal;
  }

  TyIdx GetTyIdx() const {
    return tyIdx;
  }

  void SetTyIdx(TyIdx tyIdxVal) {
    tyIdx = tyIdxVal;
  }

  FieldID GetFieldID() const {
    return fieldID;
  }

  void SetFieldID(FieldID fieldIDVal) {
    fieldID = fieldIDVal;
  }

  uint32 GetHashIndex() const override {
    uint32 hashIdx = static_cast<uint32>(GetOp());
    constexpr uint32 kOpMeHashShift = 3;
    for (const auto &opnd : opnds) {
      if (opnd == nullptr) {
        break;
      }
      hashIdx += static_cast<uint32>(opnd->GetExprID()) << kOpMeHashShift;
    }
    return hashIdx;
  }

 private:
  std::array<MeExpr*, kOperandNumTernary> opnds = { nullptr };  // kid
  PrimType opndType = kPtyInvalid;  // from type
  uint8 bitsOffset = 0;
  uint8 bitsSize = 0;
  TyIdx tyIdx;
  FieldID fieldID = 0;  // this is also used to store puIdx
};

class IvarMeExpr : public MeExpr {
 public:
  explicit IvarMeExpr(int32 exprid)
      : MeExpr(exprid, kMeOpIvar) {
    SetNumOpnds(static_cast<uint8>(kOperandNumUnary));
  }

  IvarMeExpr(int32 exprid, const IvarMeExpr &ivarme)
      : MeExpr(exprid, kMeOpIvar),
        defStmt(ivarme.defStmt),
        base(ivarme.base),
        tyIdx(ivarme.tyIdx),
        fieldID(ivarme.fieldID) {
    InitBase(ivarme.GetOp(), ivarme.GetPrimType(), ivarme.GetNumOpnds());
    mu = ivarme.mu;
  }

  ~IvarMeExpr() = default;

  void Dump(const IRMap*, int32 indent = 0) const override;
  BaseNode &EmitExpr(SSATab&) override;
  bool IsVolatile(const SSATab&) const override {
    return IsVolatile();
  }

  bool IsVolatile() const;
  bool IsFinal();
  bool IsRCWeak() const;
  bool IsUseSameSymbol(const MeExpr&) const override;
  bool IsIdentical(IvarMeExpr &expr) const;
  MeExpr *GetIdenticalExpr(MeExpr &expr) const override;
  MeExpr *GetOpnd(size_t) const override {
    return base;
  }

  IassignMeStmt *GetDefStmt() const {
    return defStmt;
  }

  void SetDefStmt(IassignMeStmt *defStmtPara) {
    defStmt = defStmtPara;
  }

  const MeExpr *GetBase() const {
    return base;
  }

  MeExpr *GetBase() {
    return base;
  }

  void SetBase(MeExpr *value) {
    base = value;
  }

  TyIdx GetTyIdx() const {
    return tyIdx;
  }

  void SetTyIdx(TyIdx tyIdxVal) {
    tyIdx = tyIdxVal;
  }

  TyIdx GetInferredTyIdx() const {
    return inferredTyIdx;
  }

  void SetInferredTyidx(TyIdx inferredTyIdxVal) {
    inferredTyIdx = inferredTyIdxVal;
  }

  FieldID GetFieldID() const {
    return fieldID;
  }

  void SetFieldID(FieldID fieldIDVal) {
    fieldID = fieldIDVal;
  }

  bool GetMaybeNull() const {
    return maybeNull;
  }

  void SetMaybeNull(bool maybeNullVal) {
    maybeNull = maybeNullVal;
  }

  VarMeExpr *GetMu() {
    return mu;
  }
  const VarMeExpr *GetMu() const {
    return mu;
  }
  void SetMuVal(VarMeExpr *muVal) {
    mu = muVal;
  }

  uint32 GetHashIndex() const override {
    constexpr uint32 kIvarHashShift = 4;
    return static_cast<uint32>(OP_iread) + fieldID + (static_cast<uint32>(base->GetExprID()) << kIvarHashShift);
  }

 private:
  IassignMeStmt *defStmt = nullptr;
  MeExpr *base = nullptr;
  TyIdx tyIdx{ 0 };
  TyIdx inferredTyIdx{ 0 };  // may be a subclass of above tyIdx
  FieldID fieldID = 0;
  bool maybeNull = true;  // false if definitely not null
  VarMeExpr *mu = nullptr;   // use of mu, only one for IvarMeExpr
};

// for array, intrinsicop and intrinsicopwithtype
class NaryMeExpr : public MeExpr {
 public:
  NaryMeExpr(MapleAllocator *alloc, int32 expID, TyIdx tyIdx, MIRIntrinsicID intrinID, bool bCheck)
      : MeExpr(expID, kMeOpNary), tyIdx(tyIdx), intrinsic(intrinID), opnds(alloc->Adapter()), boundCheck(bCheck) {}

  NaryMeExpr(MapleAllocator *alloc, int32 expID, const NaryMeExpr &meExpr)
      : MeExpr(expID, kMeOpNary),
        tyIdx(meExpr.tyIdx),
        intrinsic(meExpr.intrinsic),
        opnds(alloc->Adapter()),
        boundCheck(meExpr.boundCheck) {
    InitBase(meExpr.GetOp(), meExpr.GetPrimType(), meExpr.GetNumOpnds());
    for (size_t i = 0; i < meExpr.opnds.size(); ++i) {
      opnds.push_back(meExpr.opnds[i]);
    }
  }

  ~NaryMeExpr() = default;

  void Dump(const IRMap*, int32 indent = 0) const override;
  bool IsIdentical(NaryMeExpr&) const;
  bool IsUseSameSymbol(const MeExpr&) const override;
  BaseNode &EmitExpr(SSATab&) override;
  MeExpr *GetIdenticalExpr(MeExpr &expr) const override;
  MeExpr *GetOpnd(size_t idx) const override {
    ASSERT(idx < opnds.size(), "NaryMeExpr operand out of bounds");
    return opnds[idx];
  }

  TyIdx GetTyIdx() const {
    return tyIdx;
  }

  MIRIntrinsicID GetIntrinsic() const {
    return intrinsic;
  }

  const MapleVector<MeExpr*> &GetOpnds() const {
    return opnds;
  }

  MapleVector<MeExpr*> &GetOpnds() {
    return opnds;
  }

  void SetOpnds(MapleVector<MeExpr*> &newOpnds) {
    ASSERT(newOpnds.size() == GetNumOpnds(), "inconsistent operand numbers");
    opnds = newOpnds;
  }

  void SetOpnd(size_t idx, MeExpr *val) override {
    ASSERT(idx < opnds.size(), "out of range in NaryMeStmt::GetOpnd");
    opnds[idx] = val;
  }

  bool GetBoundCheck() {
    return boundCheck;
  }

  bool GetBoundCheck() const {
    return boundCheck;
  }

  void SetBoundCheck(bool ch) {
    boundCheck = ch;
  }

  uint32 GetHashIndex() const override {
    auto hashIdx = static_cast<uint32>(GetOp());
    constexpr uint32 kNaryHashShift = 3;
    for (uint32 i = 0; i < GetNumOpnds(); ++i) {
      hashIdx += static_cast<uint32>(opnds[i]->GetExprID()) << kNaryHashShift;
    }
    hashIdx += static_cast<uint32>(boundCheck);
    return hashIdx;
  }

 private:
  TyIdx tyIdx;
  MIRIntrinsicID intrinsic;
  MapleVector<MeExpr*> opnds;
  bool boundCheck;
};

class MeStmt {
 public:
  explicit MeStmt(const StmtNode *sst) {
    ASSERT(sst != nullptr, "StmtNode nullptr check");
    op = sst->GetOpCode();
    srcPos = sst->GetSrcPos();
  }

  explicit MeStmt(Opcode op1) : op(op1) {}

  virtual ~MeStmt() = default;

  bool GetIsLive() const {
    return isLive;
  }

  void SetIsLive(bool value) {
    isLive = value;
  }

  virtual void Dump(const IRMap*) const;
  MeStmt *GetNextMeStmt() const;
  virtual size_t NumMeStmtOpnds() const {
    return 0;
  }

  virtual MeExpr *GetOpnd(size_t) const {
    return nullptr;
  }

  virtual void SetOpnd(size_t, MeExpr*) {}

  bool IsAssertBce() const {
    return op == OP_assertlt || op == OP_assertge;
  }

  bool IsReturn() const {
    return op == OP_gosub || op == OP_retsub || op == OP_throw || op == OP_return;
  }

  bool IsCondBr() const {
    return op == OP_brtrue || op == OP_brfalse;
  }

  bool IsAssign() const {
    return op == OP_dassign || op == OP_maydassign || op == OP_iassign || op == OP_regassign;
  }

  virtual MIRType *GetReturnType() const {
    return nullptr;
  }

  void EmitCallReturnVector(SSATab &ssaTab, CallReturnVector &nRets);
  virtual MapleVector<MustDefMeNode> *GetMustDefList() {
    return nullptr;
  }

  const virtual MapleVector<MustDefMeNode> &GetMustDefList() const {
    CHECK_FATAL(false, "should be implemented");
  }

  const virtual MeExpr *GetAssignedLHS() const {
    return nullptr;
  }

  virtual MeExpr *GetAssignedLHS() {
    return nullptr;
  }

  virtual MapleMap<OStIdx, ChiMeNode*> *GetChiList() {
    return nullptr;
  }

  virtual MapleMap<OStIdx, VarMeExpr*> *GetMuList() {
    return nullptr;
  }

  void CopyBase(MeStmt &meStmt) {
    bb = meStmt.bb;
    srcPos = meStmt.srcPos;
    isLive = meStmt.isLive;
  }

  bool IsTheSameWorkcand(const MeStmt&) const;
  virtual bool NeedDecref() const {
    return false;
  }

  virtual void EnableNeedDecref() {}

  virtual void DisableNeedDecref() {}

  virtual bool NeedIncref() const {
    return false;
  }

  virtual void EnableNeedIncref() {}

  virtual void DisableNeedIncref() {}

  virtual MeExpr *GetLHS() const {
    return nullptr;
  }

  virtual MeExpr *GetRHS() const {
    return nullptr;
  }

  virtual VarMeExpr *GetVarLHS() const {
    return nullptr;
  }

  virtual VarMeExpr *GetVarLHS() {
    return nullptr;
  }

  virtual RegMeExpr *GetRegLHS() {
    return nullptr;
  }

  virtual MeExpr *GetLHSRef(SSATab&, bool) {
    return nullptr;
  }

  virtual StmtNode &EmitStmt(SSATab &ssaTab);
  void RemoveNode() {
    // remove this node from the double link list
    if (prev != nullptr) {
      prev->next = next;
    }
    if (next != nullptr) {
      next->prev = prev;
    }
  }

  void AddNext(MeStmt *node) {
    // add node to the next of this list
    node->next = next;
    node->prev = this;
    if (next != nullptr) {
      next->prev = node;
    }
    next = node;
  }

  void AddPrev(MeStmt *node) {
    // add node to the prev of this list
    node->prev = prev;
    node->next = this;
    if (prev != nullptr) {
      prev->next = node;
    }
    prev = node;
  }

  BB *GetBB() const {
    return bb;
  }

  void SetBB(BB *curBB) {
    bb = curBB;
  }

  const SrcPosition &GetSrcPosition() const {
    return srcPos;
  }

  void SetSrcPos(SrcPosition pos) {
    srcPos = pos;
  }

  void SetPrev(MeStmt *v) {
    prev = v;
  }

  void SetNext(MeStmt *n) {
    next = n;
  }

  MeStmt *GetPrev() const {
    return prev;
  }

  MeStmt *GetNext() const {
    return next;
  }

  Opcode GetOp() const {
    return op;
  }

  void SetOp(Opcode currOp) {
    op = currOp;
  }

 private:
  Opcode op;
  bool isLive = true;
  BB *bb = nullptr;
  SrcPosition srcPos;
  MeStmt *prev = nullptr;
  MeStmt *next = nullptr;
};

class ChiMeNode {
 public:
  explicit ChiMeNode(MeStmt *meStmt) : base(meStmt) {}

  ~ChiMeNode() = default;

  bool GetIsLive() const {
    return isLive;
  }

  void SetIsLive(bool value) {
    isLive = value;
  }

  VarMeExpr *GetRHS() {
    return rhs;
  }

  const VarMeExpr *GetRHS() const {
    return rhs;
  }

  void SetRHS(VarMeExpr *value) {
    rhs = value;
  }

  VarMeExpr *GetLHS() const {
    return lhs;
  }

  void SetLHS(VarMeExpr *value) {
    lhs = value;
  }

  void Dump(const IRMap *irMap) const;

  MeStmt *GetBase() const {
    return base;
  }

  void SetBase(MeStmt *value) {
    base = value;
  }

 private:
  VarMeExpr *rhs = nullptr;
  VarMeExpr *lhs = nullptr;
  MeStmt *base;
  bool isLive = true;
};

class MustDefMeNode {
 public:
  MustDefMeNode(MeExpr *x, MeStmt *meStmt) : lhs(x), base(meStmt) {
    if (x->GetMeOp() == kMeOpReg) {
      auto *reg = static_cast<RegMeExpr*>(x);
      reg->SetDefBy(kDefByMustDef);
      reg->SetDefMustDef(*this);
    } else {
      CHECK(x->GetMeOp() == kMeOpVar, "unexpected opcode");
      auto *var = static_cast<VarMeExpr*>(x);
      var->SetDefBy(kDefByMustDef);
      var->SetDefMustDef(*this);
    }
  }

  const MeExpr *GetLHS() const {
    return lhs;
  }

  MeExpr *GetLHS() {
    return lhs;
  }

  void SetLHS(MeExpr *value) {
    lhs = value;
  }

  const MeStmt *GetBase() const {
    return base;
  }

  MeStmt *GetBase() {
    return base;
  }

  void SetBase(MeStmt *value) {
    base = value;
  }

  bool GetIsLive() const {
    return isLive;
  }

  void SetIsLive(bool value) {
    isLive = value;
  }

  MustDefMeNode(const MustDefMeNode &mustDef) {
    lhs = mustDef.lhs;
    base = mustDef.base;
    isLive = mustDef.isLive;
    UpdateLHS(*lhs);
  }

  MustDefMeNode &operator=(const MustDefMeNode &mustDef) {
    if (&mustDef != this) {
      lhs = mustDef.lhs;
      base = mustDef.base;
      isLive = mustDef.isLive;
      UpdateLHS(*lhs);
    }
    return *this;
  }

  void UpdateLHS(MeExpr &x) {
    lhs = &x;
    if (x.GetMeOp() == kMeOpReg) {
      auto &reg = static_cast<RegMeExpr&>(x);
      reg.SetDefBy(kDefByMustDef);
      reg.SetDefMustDef(*this);
    } else {
      ASSERT(lhs->GetMeOp() == kMeOpVar, "unexpected opcode");
      auto &var = static_cast<VarMeExpr&>(x);
      var.SetDefBy(kDefByMustDef);
      var.SetDefMustDef(*this);
    }
  }

  ~MustDefMeNode() = default;

  void Dump(const IRMap*) const;

 private:
  MeExpr *lhs;  // could be var or register, can we make this private?
  MeStmt *base;
  bool isLive = true;
};

class PiassignMeStmt : public MeStmt {
 public:
  explicit PiassignMeStmt(MapleAllocator*)
      : MeStmt(OP_piassign) {
  }
  ~PiassignMeStmt() = default;

  void SetLHS(VarMeExpr &l) {
    lhs = &l;
  }

  void SetRHS(VarMeExpr &r) {
    rhs = &r;
  }

  void SetGeneratedBy(MeStmt &meStmt) {
    generatedBy = &meStmt;
  }

  VarMeExpr *GetLHS() const {
    return lhs;
  }

  VarMeExpr *GetRHS() const {
    return rhs;
  }

  MeStmt *GetGeneratedBy() const{
    return generatedBy;
  }

  void SetIsToken(bool t) {
    isToken = t;
  }

  bool GetIsToken() {
    return isToken;
  }

  void Dump(const IRMap*) const;

 private:
  VarMeExpr *rhs = nullptr;
  VarMeExpr *lhs = nullptr;
  MeStmt *generatedBy = nullptr;
  bool isToken = false;
};

class DassignMeStmt : public MeStmt {
 public:
  DassignMeStmt(MapleAllocator *alloc, const StmtNode *stt)
      : MeStmt(stt),
        chiList(std::less<OStIdx>(), alloc->Adapter()) {}

  explicit DassignMeStmt(MapleAllocator *alloc)
      : MeStmt(OP_dassign),
        chiList(std::less<OStIdx>(), alloc->Adapter()) {}

  DassignMeStmt(MapleAllocator *alloc, const DassignMeStmt *dass)
      : MeStmt(dass->GetOp()),
        rhs(dass->rhs),
        lhs(dass->GetVarLHS()),
        chiList(std::less<OStIdx>(), alloc->Adapter()) {}

  ~DassignMeStmt() = default;

  size_t NumMeStmtOpnds() const {
    return kOperandNumUnary;
  }

  MeExpr *GetOpnd(size_t) const {
    return rhs;
  }

  void SetOpnd(size_t, MeExpr *val) {
    rhs = val;
  }

  MapleMap<OStIdx, ChiMeNode*> *GetChiList() {
    return &chiList;
  }

  void SetChiList(MapleMap<OStIdx, ChiMeNode*> &value) {
    chiList = value;
  }

  bool NeedDecref() const {
    return needDecref;
  }

  void SetNeedDecref(bool isNeedDecref) {
    needDecref = isNeedDecref;
  }

  void SetNeedIncref(bool isNeedIncref) {
    needIncref = isNeedIncref;
  }

  void EnableNeedDecref() {
    needDecref = true;
  }

  void DisableNeedDecref() {
    needDecref = false;
  }

  bool NeedIncref() const {
    return needIncref;
  }

  void EnableNeedIncref() {
    needIncref = true;
  }

  void DisableNeedIncref() {
    needIncref = false;
  }

  void SetNoNeedIncref() {
    needIncref = false;
  }

  bool Propagated() const {
    return propagated;
  }

  void SetPropagated(bool value) {
    propagated = value;
  }

  bool GetWasMayDassign() const {
    return wasMayDassign;
  }

  void SetWasMayDassign(bool value) {
    wasMayDassign = value;
  }

  void Dump(const IRMap*) const;
  MeExpr *GetLHS() const {
    return lhs;
  }

  void SetLHS(VarMeExpr *value) {
    lhs = value;
  }

  MeExpr *GetRHS() const {
    return rhs;
  }

  void SetRHS(MeExpr *value) {
    rhs = value;
  }

  VarMeExpr *GetVarLHS() const {
    return lhs;
  }

  VarMeExpr *GetVarLHS() {
    return lhs;
  }

  MeExpr *GetLHSRef(SSATab &ssaTab, bool excludeLocalRefVar);
  void UpdateLHS(VarMeExpr &var) {
    lhs = &var;
    var.SetDefBy(kDefByStmt);
    var.SetDefStmt(this);
  }

  StmtNode &EmitStmt(SSATab &ssaTab);

 private:
  MeExpr *rhs = nullptr;
  VarMeExpr *lhs = nullptr;
  MapleMap<OStIdx, ChiMeNode*> chiList;
  bool propagated = false;     // the RHS has been propagated forward
  bool needDecref = false;     // to be determined by analyzerc phase
  bool needIncref = false;     // to be determined by analyzerc phase
  bool wasMayDassign = false;  // was converted back to dassign by may2dassign phase
};

class RegassignMeStmt : public MeStmt {
 public:
  explicit RegassignMeStmt(const StmtNode *stt) : MeStmt(stt) {}

  RegassignMeStmt() : MeStmt(OP_regassign) {}

  RegassignMeStmt(RegMeExpr *reg, MeExpr *val) : MeStmt(OP_regassign), rhs(val), lhs(reg) {
    reg->SetDefBy(kDefByStmt);
    reg->SetDefStmt(this);
  }

  ~RegassignMeStmt() = default;

  size_t NumMeStmtOpnds() const {
    return kOperandNumUnary;
  }

  MeExpr *GetOpnd(size_t) const {
    return rhs;
  }

  void SetOpnd(size_t, MeExpr *val) {
    rhs = val;
  }

  void Dump(const IRMap*) const;
  bool NeedIncref() const {
    return needIncref;
  }

  void EnableNeedIncref() {
    needIncref = true;
  }

  void DisableNeedIncref() {
    needIncref = false;
  }

  MeExpr *GetLHS() const {
    return lhs;
  }

  MeExpr *GetRHS() const {
    return rhs;
  }

  void SetRHS(MeExpr *value) {
    rhs = value;
  }

  RegMeExpr *GetRegLHS() {
    return lhs;
  }

  void SetLHS(RegMeExpr *value) {
    lhs = value;
  }

  StmtNode &EmitStmt(SSATab &ssaTab);

 private:
  MeExpr *rhs = nullptr;
  RegMeExpr *lhs = nullptr;
  bool needIncref = false;  // to be determined by analyzerc phase
};

class MaydassignMeStmt : public MeStmt {
 public:
  MaydassignMeStmt(MapleAllocator *alloc, const StmtNode *stt)
      : MeStmt(stt),
        chiList(std::less<OStIdx>(), alloc->Adapter()) {}

  MaydassignMeStmt(MapleAllocator *alloc, MaydassignMeStmt &maydass)
      : MeStmt(maydass.GetOp()), rhs(maydass.GetRHS()), mayDSSym(maydass.GetMayDassignSym()),
        fieldID(maydass.GetFieldID()), chiList(std::less<OStIdx>(), alloc->Adapter()),
        needDecref(maydass.NeedDecref()), needIncref(maydass.NeedIncref()) {}

  ~MaydassignMeStmt() = default;

  size_t NumMeStmtOpnds() const {
    return kOperandNumUnary;
  }

  MeExpr *GetOpnd(size_t) const {
    return rhs;
  }

  void SetOpnd(size_t, MeExpr *val) {
    rhs = val;
  }

  MapleMap<OStIdx, ChiMeNode*> *GetChiList() {
    return &chiList;
  }

  void SetChiList(MapleMap<OStIdx, ChiMeNode*> &value) {
    chiList = value;
  }

  bool NeedDecref() const {
    return needDecref;
  }

  void EnableNeedDecref() {
    needDecref = true;
  }

  void DisableNeedDecref() {
    needDecref = false;
  }

  bool NeedIncref() const {
    return needIncref;
  }

  void EnableNeedIncref() {
    needIncref = true;
  }

  void DisableNeedIncref() {
    needIncref = false;
  }

  OriginalSt *GetMayDassignSym() {
    return mayDSSym;
  }

  const OriginalSt *GetMayDassignSym() const {
    return mayDSSym;
  }

  void SetMayDassignSym(OriginalSt *sym) {
    mayDSSym = sym;
  }

  FieldID GetFieldID() const {
    return fieldID;
  }

  void SetFieldID(FieldID fieldIDVal) {
    fieldID = fieldIDVal;
  }

  void Dump(const IRMap*) const;
  MeExpr *GetLHS() const {
    return chiList.begin()->second->GetLHS();
  }

  MeExpr *GetRHS() const {
    return rhs;
  }

  void SetRHS(MeExpr *value) {
    rhs = value;
  }

  VarMeExpr *GetVarLHS() const {
    return chiList.begin()->second->GetLHS();
  }

  VarMeExpr *GetVarLHS() {
    return chiList.begin()->second->GetLHS();
  }

  MeExpr *GetLHSRef(SSATab &ssaTab, bool excludeLocalRefVar);
  StmtNode &EmitStmt(SSATab &ssaTab);

 private:
  MeExpr *rhs = nullptr;
  OriginalSt *mayDSSym = nullptr;
  FieldID fieldID = 0;
  MapleMap<OStIdx, ChiMeNode*> chiList;
  bool needDecref = false;  // to be determined by analyzerc phase
  bool needIncref = false;  // to be determined by analyzerc phase
};

class IassignMeStmt : public MeStmt {
 public:
  IassignMeStmt(MapleAllocator *alloc, const StmtNode *stt)
      : MeStmt(stt),
        chiList(std::less<OStIdx>(), alloc->Adapter()) {}

  IassignMeStmt(MapleAllocator*, const IassignMeStmt &iss)
      : MeStmt(iss),
        tyIdx(iss.tyIdx),
        lhsVar(iss.lhsVar),
        rhs(iss.rhs),
        chiList(iss.chiList) {
  }

  IassignMeStmt(MapleAllocator*, TyIdx tidx, IvarMeExpr *l, MeExpr *r, const MapleMap<OStIdx, ChiMeNode*> *clist)
      : MeStmt(OP_iassign), tyIdx(tidx), lhsVar(l), rhs(r), chiList(*clist) {
    l->SetDefStmt(this);
  }

  IassignMeStmt(MapleAllocator *alloc, TyIdx tidx, IvarMeExpr &l, MeExpr &r)
      : MeStmt(OP_iassign), tyIdx(tidx), lhsVar(&l), rhs(&r), chiList(std::less<OStIdx>(), alloc->Adapter()) {
    l.SetDefStmt(this);
  }

  ~IassignMeStmt() = default;

  TyIdx GetTyIdx() const {
    return tyIdx;
  }

  void SetTyIdx(TyIdx idx) {
    tyIdx = idx;
  }

  size_t NumMeStmtOpnds() const {
    return kOperandNumBinary;
  }

  MeExpr *GetOpnd(size_t idx) const {
    return idx == 0 ? lhsVar->GetBase() : rhs;
  }

  void SetOpnd(size_t idx, MeExpr *val) {
    if (idx == 0) {
      lhsVar->SetBase(val);
    } else {
      rhs = val;
    }
  }

  MapleMap<OStIdx, ChiMeNode*> *GetChiList() {
    return &chiList;
  }

  void SetChiList(MapleMap<OStIdx, ChiMeNode*> &value) {
    chiList = value;
  }

  MeExpr *GetLHSRef(SSATab &ssaTab, bool excludeLocalRefVar);
  bool NeedDecref() const {
    return needDecref;
  }

  void EnableNeedDecref() {
    needDecref = true;
  }

  void DisableNeedDecref() {
    needDecref = false;
  }

  bool NeedIncref() const {
    return needIncref;
  }

  void EnableNeedIncref() {
    needIncref = true;
  }

  void DisableNeedIncref() {
    needIncref = false;
  }

  void Dump(const IRMap*) const;
  MeExpr *GetLHS() const {
    return lhsVar;
  }

  MeExpr *GetRHS() const {
    return rhs;
  }

  void SetRHS(MeExpr *val) {
    rhs = val;
  }

  IvarMeExpr *GetLHSVal() const {
    return lhsVar;
  }

  void SetLHSVal(IvarMeExpr *val) {
    lhsVar = val;
  }

  StmtNode &EmitStmt(SSATab &ssaTab);

 private:
  TyIdx tyIdx{ 0 };
  IvarMeExpr *lhsVar = nullptr;
  MeExpr *rhs = nullptr;
  MapleMap<OStIdx, ChiMeNode*> chiList;
  bool needDecref = false;  // to be determined by analyzerc phase
  bool needIncref = false;  // to be determined by analyzerc phase
};

class NaryMeStmt : public MeStmt {
 public:
  NaryMeStmt(MapleAllocator *alloc, const StmtNode *stt) : MeStmt(stt), opnds(alloc->Adapter()) {}

  NaryMeStmt(MapleAllocator *alloc, Opcode op) : MeStmt(op), opnds(alloc->Adapter()) {}

  NaryMeStmt(MapleAllocator *alloc, const NaryMeStmt *nstmt) : MeStmt(nstmt->GetOp()), opnds(alloc->Adapter()) {
    for (MeExpr *o : nstmt->opnds) {
      opnds.push_back(o);
    }
  }

  virtual ~NaryMeStmt() = default;

  size_t NumMeStmtOpnds() const {
    return opnds.size();
  }

  MeExpr *GetOpnd(size_t idx) const {
    ASSERT(idx < opnds.size(), "out of range in NaryMeStmt::GetOpnd");
    return opnds.at(idx);
  }

  void SetOpnd(size_t idx, MeExpr *val) {
    opnds[idx] = val;
  }

  const MapleVector<MeExpr*> &GetOpnds() const {
    return opnds;
  }

  void PushBackOpnd(MeExpr *val) {
    opnds.push_back(val);
  }

  void PopBackOpnd() {
    opnds.pop_back();
  }

  void SetOpnds(MapleVector<MeExpr*> &opndsVal) {
    opnds = opndsVal;
  }

  void EraseOpnds(const MapleVector<MeExpr*>::const_iterator begin, const MapleVector<MeExpr*>::const_iterator end) {
    opnds.erase(begin, end);
  }

  void EraseOpnds(const MapleVector<MeExpr *>::const_iterator it) {
    opnds.erase(it);
  }

  void InsertOpnds(const MapleVector<MeExpr*>::const_iterator begin, MeExpr *expr) {
    opnds.insert(begin, expr);
  }

  void DumpOpnds(const IRMap*) const;
  void Dump(const IRMap*) const;
  virtual MapleMap<OStIdx, VarMeExpr*> *GetMuList() {
    return nullptr;
  }

  StmtNode &EmitStmt(SSATab &ssaTab);

 private:
  MapleVector<MeExpr*> opnds;
};

class MuChiMePart {
 public:
  explicit MuChiMePart(MapleAllocator *alloc)
      : muList(std::less<OStIdx>(), alloc->Adapter()), chiList(std::less<OStIdx>(), alloc->Adapter()) {}

  virtual MapleMap<OStIdx, ChiMeNode*> *GetChiList() {
    return &chiList;
  }

  void SetChiList(MapleMap<OStIdx, ChiMeNode*> &value) {
    chiList = value;
  }

  virtual ~MuChiMePart() = default;

 protected:
  MapleMap<OStIdx, VarMeExpr*> muList;
  MapleMap<OStIdx, ChiMeNode*> chiList;
};

class AssignedPart {
 public:
  explicit AssignedPart(MapleAllocator *alloc) : mustDefList(alloc->Adapter()) {}

  explicit AssignedPart(const MapleVector<MustDefMeNode> &mustDefList)
      : mustDefList(mustDefList) {}

  virtual ~AssignedPart() = default;

  void DumpAssignedPart(const IRMap *irMap) const;
  VarMeExpr *GetAssignedPartLHSRef(SSATab &ssaTab, bool excludeLocalRefVar);

 protected:
  MapleVector<MustDefMeNode> mustDefList;
  bool needDecref = false;  // to be determined by analyzerc phase
  bool needIncref = false;  // to be determined by analyzerc phase
};

class CallMeStmt : public NaryMeStmt, public MuChiMePart, public AssignedPart {
 public:
  CallMeStmt(MapleAllocator *alloc, const StmtNode *stt)
      : NaryMeStmt(alloc, stt),
        MuChiMePart(alloc),
        AssignedPart(alloc),
        puIdx(static_cast<const CallNode*>(stt)->GetPUIdx()),
        stmtID(stt->GetStmtID()),
        tyIdx(static_cast<const CallNode*>(stt)->GetTyIdx()) {}

  CallMeStmt(MapleAllocator *alloc, Opcode op)
      : NaryMeStmt(alloc, op), MuChiMePart(alloc), AssignedPart(alloc) {}

  CallMeStmt(MapleAllocator *alloc, NaryMeStmt *cstmt, PUIdx idx)
      : NaryMeStmt(alloc, cstmt),
        MuChiMePart(alloc),
        AssignedPart(alloc),
        puIdx(idx) {}

  CallMeStmt(MapleAllocator *alloc, const CallMeStmt *cstmt)
      : NaryMeStmt(alloc, cstmt),
        MuChiMePart(alloc),
        AssignedPart(cstmt->mustDefList),
        puIdx(cstmt->GetPUIdx()) {}

  virtual ~CallMeStmt() = default;

  PUIdx GetPUIdx() const {
    return puIdx;
  }

  void SetPUIdx(PUIdx idx) {
    puIdx = idx;
  }

  uint32 GetStmtID() const {
    return stmtID;
  }

  void Dump(const IRMap*) const;
  MapleMap<OStIdx, VarMeExpr*> *GetMuList() {
    return &muList;
  }

  MapleMap<OStIdx, ChiMeNode*> *GetChiList() {
    return &chiList;
  }

  MapleVector<MustDefMeNode> *GetMustDefList() {
    return &mustDefList;
  }

  const MapleVector<MustDefMeNode> &GetMustDefList() const {
    return mustDefList;
  }

  MustDefMeNode &GetMustDefListItem(int i) {
    return mustDefList[i];
  }

  size_t MustDefListSize() const {
    return mustDefList.size();
  }

  const MeExpr *GetAssignedLHS() const {
    return mustDefList.empty() ? nullptr : mustDefList.front().GetLHS();
  }

  MeExpr *GetAssignedLHS() {
    return mustDefList.empty() ? nullptr : mustDefList.front().GetLHS();
  }

  MeExpr *GetLHSRef(SSATab &ssaTab, bool excludeLocalRefVar) {
    return GetAssignedPartLHSRef(ssaTab, excludeLocalRefVar);
  }

  VarMeExpr *GetVarLHS() {
    if (mustDefList.empty() || mustDefList.front().GetLHS()->GetMeOp() != kMeOpVar) {
      return nullptr;
    }
    return static_cast<VarMeExpr*>(mustDefList.front().GetLHS());
  }

  bool NeedDecref() const {
    return needDecref;
  }

  void EnableNeedDecref() {
    needDecref = true;
  }

  void DisableNeedDecref() {
    needDecref = false;
  }

  bool NeedIncref() const {
    return needIncref;
  }

  void EnableNeedIncref() {
    needIncref = true;
  }

  void DisableNeedIncref() {
    needIncref = false;
  }

  MIRType *GetReturnType() const {
    MIRFunction *callee = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(puIdx);
    return callee->GetReturnType();
  }

  const MIRFunction &GetTargetFunction() const;
  MIRFunction &GetTargetFunction();
  StmtNode &EmitStmt(SSATab &ssaTab);

  void SetCallReturn(MeExpr&);

 private:
  PUIdx puIdx = 0;
  // Used in trim call graph
  uint32 stmtID = 0;
  TyIdx tyIdx;
};

class IcallMeStmt : public NaryMeStmt, public MuChiMePart, public AssignedPart {
 public:
  IcallMeStmt(MapleAllocator *alloc, const StmtNode *stt)
      : NaryMeStmt(alloc, stt),
        MuChiMePart(alloc),
        AssignedPart(alloc),
        retTyIdx(static_cast<const IcallNode*>(stt)->GetRetTyIdx()),
        stmtID(stt->GetStmtID()) {}

  IcallMeStmt(MapleAllocator *alloc, Opcode op)
      : NaryMeStmt(alloc, op), MuChiMePart(alloc), AssignedPart(alloc) {}

  IcallMeStmt(MapleAllocator *alloc, const IcallMeStmt *cstmt)
      : NaryMeStmt(alloc, cstmt),
        MuChiMePart(alloc),
        AssignedPart(cstmt->mustDefList),
        retTyIdx(cstmt->retTyIdx),
        stmtID(cstmt->stmtID) {}

  virtual ~IcallMeStmt() = default;

  void Dump(const IRMap*) const;
  MapleMap<OStIdx, VarMeExpr*> *GetMuList() {
    return &muList;
  }

  MapleMap<OStIdx, ChiMeNode*> *GetChiList() {
    return &chiList;
  }

  MapleVector<MustDefMeNode> *GetMustDefList() {
    return &mustDefList;
  }

  const MapleVector<MustDefMeNode> &GetMustDefList() const {
    return mustDefList;
  }

  const MeExpr *GetAssignedLHS() const {
    return mustDefList.empty() ? nullptr : mustDefList.front().GetLHS();
  }

  MeExpr *GetLHSRef(SSATab &ssaTab, bool excludeLocalRefVar) {
    return GetAssignedPartLHSRef(ssaTab, excludeLocalRefVar);
  }

  bool NeedDecref() const {
    return needDecref;
  }

  void EnableNeedDecref() {
    needDecref = true;
  }

  void DisableNeedDecref() {
    needDecref = false;
  }

  bool NeedIncref() const {
    return needIncref;
  }

  void EnableNeedIncref() {
    needIncref = true;
  }

  void DisableNeedIncref() {
    needIncref = false;
  }

  MIRType *GetReturnType() const {
    return GlobalTables::GetTypeTable().GetTypeFromTyIdx(retTyIdx);
  }

  StmtNode &EmitStmt(SSATab &ssaTab);

  TyIdx GetRetTyIdx() const {
    return retTyIdx;
  }

  void SetRetTyIdx(TyIdx idx) {
    retTyIdx = idx;
  }

 private:
  // return type for callee
  TyIdx retTyIdx{ 0 };
  // Used in trim call graph
  uint32 stmtID = 0;
};

class IntrinsiccallMeStmt : public NaryMeStmt, public MuChiMePart, public AssignedPart {
 public:
  IntrinsiccallMeStmt(MapleAllocator *alloc, const StmtNode *stt)
      : NaryMeStmt(alloc, stt),
        MuChiMePart(alloc),
        AssignedPart(alloc),
        intrinsic(static_cast<const IntrinsiccallNode*>(stt)->GetIntrinsic()),
        tyIdx(static_cast<const IntrinsiccallNode*>(stt)->GetTyIdx()),
        retPType(stt->GetPrimType()) {}

  IntrinsiccallMeStmt(MapleAllocator *alloc, Opcode op, MIRIntrinsicID id, TyIdx tyid = TyIdx())
      : NaryMeStmt(alloc, op),
        MuChiMePart(alloc),
        AssignedPart(alloc),
        intrinsic(id),
        tyIdx(tyid) {}

  IntrinsiccallMeStmt(MapleAllocator *alloc, const IntrinsiccallMeStmt *intrn)
      : NaryMeStmt(alloc, intrn),
        MuChiMePart(alloc),
        AssignedPart(intrn->mustDefList),
        intrinsic(intrn->GetIntrinsic()),
        tyIdx(intrn->tyIdx),
        retPType(intrn->retPType) {}

  IntrinsiccallMeStmt(MapleAllocator *alloc, const NaryMeStmt *nary, MIRIntrinsicID id, TyIdx idx, PrimType type)
      : NaryMeStmt(alloc, nary),
        MuChiMePart(alloc),
        AssignedPart(alloc),
        intrinsic(id),
        tyIdx(idx),
        retPType(type) {}

  virtual ~IntrinsiccallMeStmt() = default;

  void Dump(const IRMap*) const;
  MapleMap<OStIdx, VarMeExpr*> *GetMuList() {
    return &muList;
  }

  MapleMap<OStIdx, ChiMeNode*> *GetChiList() {
    return &chiList;
  }

  MIRType *GetReturnType() const {
    if (!mustDefList.empty()) {
      return GlobalTables::GetTypeTable().GetPrimType(mustDefList.front().GetLHS()->GetPrimType());
    }
    return GlobalTables::GetTypeTable().GetPrimType(retPType);
  }

  MapleVector<MustDefMeNode> *GetMustDefList() {
    return &mustDefList;
  }

  const MapleVector<MustDefMeNode> &GetMustDefList() const {
    return mustDefList;
  }

  MustDefMeNode &GetMustDefListItem(int i) {
    return mustDefList[i];
  }

  const MeExpr *GetAssignedLHS() const {
    return mustDefList.empty() ? nullptr : mustDefList.front().GetLHS();
  }

  MeExpr *GetAssignedLHS() {
    return mustDefList.empty() ? nullptr : mustDefList.front().GetLHS();
  }

  MeExpr *GetLHSRef(SSATab &ssaTab, bool excludeLocalRefVar) {
    return GetAssignedPartLHSRef(ssaTab, excludeLocalRefVar);
  }

  bool NeedDecref() const {
    return needDecref;
  }

  void EnableNeedDecref() {
    needDecref = true;
  }

  void DisableNeedDecref() {
    needDecref = false;
  }

  bool NeedIncref() const {
    return needIncref;
  }

  void EnableNeedIncref() {
    needIncref = true;
  }

  void DisableNeedIncref() {
    needIncref = false;
  }

  StmtNode &EmitStmt(SSATab &ssaTab);

  MIRIntrinsicID GetIntrinsic() const {
    return intrinsic;
  }

  void SetIntrinsic(MIRIntrinsicID id) {
    intrinsic = id;
  }

  TyIdx GetTyIdx() const {
    return tyIdx;
  }

  PrimType GetReturnPrimType() const {
    return retPType;
  }

 private:
  MIRIntrinsicID intrinsic;
  TyIdx tyIdx;
  // Used to store return value type
  PrimType retPType = kPtyInvalid;
};

class RetMeStmt : public NaryMeStmt {
 public:
  RetMeStmt(MapleAllocator *alloc, const StmtNode *stt)
      : NaryMeStmt(alloc, stt), muList(std::less<OStIdx>(), alloc->Adapter()) {}

  ~RetMeStmt() = default;

  void Dump(const IRMap*) const;
  MapleMap<OStIdx, VarMeExpr*> *GetMuList() {
    return &muList;
  }

 private:
  MapleMap<OStIdx, VarMeExpr*> muList;
};

// eval, free, decref, incref, decrefreset, assertnonnull
class UnaryMeStmt : public MeStmt {
 public:
  explicit UnaryMeStmt(const StmtNode *stt) : MeStmt(stt) {}

  explicit UnaryMeStmt(Opcode o) : MeStmt(o) {}

  explicit UnaryMeStmt(const UnaryMeStmt *umestmt) : MeStmt(umestmt->GetOp()), opnd(umestmt->opnd) {}

  virtual ~UnaryMeStmt() = default;

  size_t NumMeStmtOpnds() const {
    return kOperandNumUnary;
  }

  MeExpr *GetOpnd(size_t) const {
    return opnd;
  }

  void SetOpnd(size_t, MeExpr *val) {
    opnd = val;
  }

  MeExpr *GetOpnd() const {
    return opnd;
  }

  void SetMeStmtOpndValue(MeExpr *val) {
    opnd = val;
  }

  void Dump(const IRMap*) const;

  StmtNode &EmitStmt(SSATab &ssaTab);

 private:
  MeExpr *opnd = nullptr;
};

class GotoMeStmt : public MeStmt {
 public:
  explicit GotoMeStmt(const StmtNode *stt) : MeStmt(stt), offset(static_cast<const GotoNode*>(stt)->GetOffset()) {}
  explicit GotoMeStmt(const GotoMeStmt &condGoto) : MeStmt(MeStmt(condGoto.GetOp())), offset(condGoto.GetOffset()) {}

  ~GotoMeStmt() = default;

  uint32 GetOffset() const {
    return offset;
  }

  void SetOffset(uint32 o) {
    offset = o;
  }

  StmtNode &EmitStmt(SSATab &ssaTab);

 private:
  uint32 offset;  // the label
};

class CondGotoMeStmt : public UnaryMeStmt {
 public:
  explicit CondGotoMeStmt(const StmtNode *stt)
      : UnaryMeStmt(stt), offset(static_cast<const CondGotoNode*>(stt)->GetOffset()) {}

  explicit CondGotoMeStmt(const CondGotoMeStmt &condGoto)
      : UnaryMeStmt(static_cast<const UnaryMeStmt*>(&condGoto)), offset(condGoto.GetOffset()) {}

  CondGotoMeStmt(const UnaryMeStmt &unaryMeStmt, uint32 o)
      : UnaryMeStmt(&unaryMeStmt), offset(o) {}

  ~CondGotoMeStmt() = default;

  uint32 GetOffset() const {
    return offset;
  }

  void SetOffset(uint32 currOffset) {
    offset = currOffset;
  }

  void Dump(const IRMap*) const;
  StmtNode &EmitStmt(SSATab &ssaTab);

 private:
  uint32 offset;  // the label
};

class JsTryMeStmt : public MeStmt {
 public:
  explicit JsTryMeStmt(const StmtNode *stt)
      : MeStmt(stt),
        catchOffset(static_cast<const JsTryNode*>(stt)->GetCatchOffset()),
        finallyOffset(static_cast<const JsTryNode*>(stt)->GetFinallyOffset()) {}

  ~JsTryMeStmt() = default;

  StmtNode &EmitStmt(SSATab &ssaTab);

 private:
  uint16 catchOffset;
  uint16 finallyOffset;
};

class TryMeStmt : public MeStmt {
 public:
  TryMeStmt(MapleAllocator *alloc, const StmtNode *stt) : MeStmt(stt), offsets(alloc->Adapter()) {}

  ~TryMeStmt() = default;

  void OffsetsPushBack(LabelIdx curr) {
    offsets.push_back(curr);
  }

  StmtNode &EmitStmt(SSATab &ssaTab);

 private:
  MapleVector<LabelIdx> offsets;
};

class CatchMeStmt : public MeStmt {
 public:
  CatchMeStmt(MapleAllocator *alloc, StmtNode *stt) : MeStmt(stt), exceptionTyIdxVec(alloc->Adapter()) {
    for (auto it : static_cast<CatchNode*>(stt)->GetExceptionTyIdxVec()) {
      exceptionTyIdxVec.push_back(it);
    }
  }

  ~CatchMeStmt() = default;

  StmtNode &EmitStmt(SSATab &ssaTab);

 private:
  MapleVector<TyIdx> exceptionTyIdxVec;
};

class SwitchMeStmt : public UnaryMeStmt {
 public:
  SwitchMeStmt(MapleAllocator *alloc, const StmtNode *stt)
      : UnaryMeStmt(stt),
        defaultLabel(static_cast<const SwitchNode*>(stt)->GetDefaultLabel()),
        switchTable(alloc->Adapter()) {
    switchTable = static_cast<const SwitchNode*>(stt)->GetSwitchTable();
  }

  ~SwitchMeStmt() = default;

  LabelIdx GetDefaultLabel() const {
    return defaultLabel;
  }

  void SetDefaultLabel(LabelIdx curr) {
    defaultLabel = curr;
  }

  void SetCaseLabel(uint32 caseIdx, LabelIdx label) {
    switchTable[caseIdx].second = label;
  }

  const CaseVector &GetSwitchTable() {
    return switchTable;
  }

  void Dump(const IRMap*) const;
  StmtNode &EmitStmt(SSATab &ssatab);

 private:
  LabelIdx defaultLabel;
  CaseVector switchTable;
};

class CommentMeStmt : public MeStmt {
 public:
  CommentMeStmt(MapleAllocator *alloc, const StmtNode *stt) : MeStmt(stt), comment(alloc->GetMemPool()) {
    comment = static_cast<const CommentNode*>(stt)->GetComment();
  }

  ~CommentMeStmt() = default;

  StmtNode &EmitStmt(SSATab &ssaTab);

 private:
  MapleString comment;
};

class WithMuMeStmt : public MeStmt {
 public:
  WithMuMeStmt(MapleAllocator *alloc, const StmtNode *stt)
      : MeStmt(stt), muList(std::less<OStIdx>(), alloc->Adapter()) {}

  virtual ~WithMuMeStmt() = default;

  MapleMap<OStIdx, VarMeExpr*> *GetMuList() {
    return &muList;
  }

  const MapleMap<OStIdx, VarMeExpr*> *GetMuList() const {
    return &muList;
  }

 private:
  MapleMap<OStIdx, VarMeExpr*> muList;
};

class GosubMeStmt : public WithMuMeStmt {
 public:
  GosubMeStmt(MapleAllocator *alloc, const StmtNode *stt)
      : WithMuMeStmt(alloc, stt), offset(static_cast<const GotoNode*>(stt)->GetOffset()) {}

  ~GosubMeStmt() = default;

  void Dump(const IRMap*) const;
  StmtNode &EmitStmt(SSATab &ssatab);

 private:
  uint32 offset;  // the label
};

class ThrowMeStmt : public WithMuMeStmt {
 public:
  ThrowMeStmt(MapleAllocator *alloc, const StmtNode *stt) : WithMuMeStmt(alloc, stt) {}

  ~ThrowMeStmt() = default;

  size_t NumMeStmtOpnds() const {
    return kOperandNumUnary;
  }

  MeExpr *GetOpnd(size_t) const {
    return opnd;
  }

  void SetOpnd(size_t, MeExpr *val) {
    opnd = val;
  }

  MeExpr *GetOpnd() const {
    return opnd;
  }

  void SetMeStmtOpndValue(MeExpr *val) {
    opnd = val;
  }

  void Dump(const IRMap*) const;
  StmtNode &EmitStmt(SSATab &ssaTab);

 private:
  MeExpr *opnd = nullptr;
};

class SyncMeStmt : public NaryMeStmt, public MuChiMePart {
 public:
  SyncMeStmt(MapleAllocator *alloc, const StmtNode *stt) : NaryMeStmt(alloc, stt), MuChiMePart(alloc) {}

  ~SyncMeStmt() = default;

  void Dump(const IRMap*) const;
  MapleMap<OStIdx, VarMeExpr*> *GetMuList() {
    return &muList;
  }

  MapleMap<OStIdx, ChiMeNode*> *GetChiList() {
    return &chiList;
  }
};

// assert ge or lt for boundary check
class AssertMeStmt : public MeStmt {
 public:
  explicit AssertMeStmt(const StmtNode *stt) : MeStmt(stt) {
    Opcode op = stt->GetOpCode();
    CHECK(op == OP_assertge || op == OP_assertlt, "runtime check error");
  }

  explicit AssertMeStmt(Opcode op1) : MeStmt(op1) {
    Opcode op = op1;
    CHECK(op == OP_assertge || op == OP_assertlt, "runtime check error");
  }

  size_t NumMeStmtOpnds() const override {
    return kOperandNumBinary;
  }

  AssertMeStmt(const AssertMeStmt &assMeStmt) : MeStmt(assMeStmt.GetOp()) {
    SetOp(assMeStmt.GetOp());
    opnds[0] = assMeStmt.opnds[0];
    opnds[1] = assMeStmt.opnds[1];
  }

  AssertMeStmt(MeExpr *arrExpr, MeExpr *idxExpr, bool ilt) : MeStmt(ilt ? OP_assertlt : OP_assertge) {
    opnds[0] = arrExpr;
    opnds[1] = idxExpr;
  }

  ~AssertMeStmt() = default;

  MeExpr *GetIndexExpr() const {
    return opnds[1];
  }

  MeExpr *GetArrayExpr() const {
    return opnds[0];
  }

  void SetOpnd(size_t i, MeExpr *opnd) override {
    CHECK_FATAL(i < kOperandNumBinary, "AssertMeStmt has two opnds");
    opnds[i] = opnd;
  }

  MeExpr *GetOpnd(size_t i) const override {
    CHECK_FATAL(i < kOperandNumBinary, "AssertMeStmt has two opnds");
    return opnds[i];
  }

  void Dump(const IRMap*) const override;
  StmtNode &EmitStmt(SSATab &ssaTab) override;

 private:
  MeExpr *opnds[kOperandNumBinary];
  AssertMeStmt &operator=(const AssertMeStmt &assMeStmt) {
    if (&assMeStmt == this) {
      return *this;
    }
    SetOp(assMeStmt.GetOp());
    opnds[0] = assMeStmt.opnds[0];
    opnds[1] = assMeStmt.opnds[1];
    return *this;
  }
};

MapleMap<OStIdx, ChiMeNode*> *GenericGetChiListFromVarMeExpr(VarMeExpr &expr);
void DumpMuList(const IRMap *irMap, const MapleMap<OStIdx, VarMeExpr*> &muList);
void DumpChiList(const IRMap *irMap, const MapleMap<OStIdx, ChiMeNode*> &chiList);
class DumpOptions {
 public:
  static bool GetSimpleDump() {
    return simpleDump;
  }

  static int GetDumpVsyNum() {
    return dumpVsymNum;
  }

 private:
  static bool simpleDump;
  static int dumpVsymNum;
};
}  // namespace maple
#define LOAD_SAFE_CAST_FOR_ME_EXPR
#define LOAD_SAFE_CAST_FOR_ME_STMT
#include "me_safe_cast_traits.def"
#endif  // MAPLE_ME_INCLUDE_ME_IR_H
