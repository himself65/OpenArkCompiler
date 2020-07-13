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
#ifndef MAPLE_ME_INCLUDE_MESSI_H
#define MAPLE_ME_INCLUDE_MESSI_H
#include "me_function.h"
#include "me_irmap.h"
#include "me_ir.h"

namespace maple {
struct StmtComparator {
  bool operator()(const std::pair<MeStmt*, size_t> &lhs, const std::pair<MeStmt*, size_t> &rhs) const {
    if (lhs.first != rhs.first) {
      return lhs.first < rhs.first;
    } else {
      return lhs.second < rhs.second;
    }
  }
};

class DefPoint {
 public:
  enum DefineKind {
    kDefByPi,
    kDefByPhi
  };

  explicit DefPoint(DefineKind dk) : defKind(dk) {}
  ~DefPoint() = default;

  void SetDefPi(PiassignMeStmt &s) {
    CHECK_FATAL(defKind == kDefByPi, "must be");
    value.pi = &s;
  }

  PiassignMeStmt *GetPiStmt() {
    CHECK_FATAL(defKind == kDefByPi, "must be");
    return value.pi;
  }

  const PiassignMeStmt *GetPiStmt() const {
    CHECK_FATAL(defKind == kDefByPi, "must be");
    return value.pi;
  }

  void SetDefPhi(MePhiNode &s) {
    CHECK_FATAL(defKind == kDefByPhi, "must be");
    value.phi = &s;
  }

  BB *GetBB() const {
    if (defKind == kDefByPi) {
      return value.pi->GetBB();
    } else {
      return value.phi->GetDefBB();
    }
  }

  BB *GetGeneratedByBB() const {
    CHECK_FATAL(defKind == kDefByPi, "must be");
    return value.pi->GetGeneratedBy()->GetBB();
  }

  VarMeExpr *GetRHS() const {
    if (defKind == kDefByPi) {
      return value.pi->GetRHS();
    } else {
      return static_cast<VarMeExpr*>(value.phi->GetOpnd(0));
    }
  }

  VarMeExpr *GetLHS() const {
    if (defKind == kDefByPi) {
      return value.pi->GetLHS();
    } else {
      return  static_cast<VarMeExpr*>(value.phi->GetLHS());
    }
  }

  const OStIdx &GetOStIdx() const {
    if (defKind == kDefByPi) {
      return value.pi->GetRHS()->GetOStIdx();
    } else {
      return value.phi->GetOpnd(0)->GetOStIdx();
    }
  }

  bool IsPiStmt() const {
    return defKind == kDefByPi;
  }

  bool IsGeneratedByBr() const {
    CHECK_FATAL(defKind == kDefByPi, "must be");
    MeStmt *stmt = value.pi->GetGeneratedBy();
    if (stmt->GetOp() == OP_brtrue || stmt->GetOp() == OP_brfalse) {
      return true;
    }
    return false;
  }

  void RemoveFromBB() {
    if (defKind == kDefByPi) {
      if (IsGeneratedByBr()) {
        GetBB()->GetPiList().clear();
      } else {
        GetBB()->RemoveMeStmt(value.pi);
      }
    } else {
      GetBB()->GetMePhiList().erase(GetOStIdx());
    }
  }

  void Dump(const IRMap &irMap) {
    LogInfo::MapleLogger() << "New Def : " << '\n';
    if (defKind == kDefByPi) {
      value.pi->Dump(&irMap);
    } else {
      value.phi->Dump(&irMap);
    }
    LogInfo::MapleLogger() << '\n';
  }

 private:
  DefineKind defKind;
  union DefStmt {
    PiassignMeStmt *pi;
    MePhiNode *phi;
  };
  DefStmt value;
};

enum SSIOptType : uint8_t {
  kNullPointerCheckOpt = 0x1,
  kCheckCastOpt = 0x2,
  kCondBaseRCOpt = 0x4,
  kArrayBoundsCheckOpt = 0x8,
  kCondBasePropOpt = 0x10,
  kBranchResolve = 0x20,
};

class SSIType {
 public:
  SSIType() = default;
  ~SSIType() = default;

  void SetOptType(uint8_t t) {
    optType = t;
  }

  uint8_t GetOptType() {
    return optType;
  }

  bool GetOptKindType(SSIOptType x) const {
    return (optType & x) != 0;
  }

  void DumpOptType() const;

 private:
  uint8_t optType = 0;
};

class MeSSI {
 public:
  static bool isDebug;
  MeSSI(MeFunction &meFunction, Dominance &dom, MeIRMap &map, MemPool &pool,
        std::map<MeStmt*, NaryMeExpr*>* acs = nullptr,
        std::map<BB*, std::vector<MeStmt*>>* cBB = nullptr)
      : meFunc(&meFunction),
        dom(&dom),
        irMap(&map),
        memPool(&pool),
        allocator(&pool),
        arrayChecks(acs),
        containsBB(cBB) {}
  ~MeSSI() = default;
  void ConvertToSSI();
  void ConvertToSSA();
  void SetSSIType(uint8_t opt) {
    ssiType.SetOptType(opt);
  }
  MIRType *GetInferredType(MeExpr *expr);
 private:
  NaryMeExpr *GetInstanceOfType(MeExpr &e);
  void AddPiForABCOpt(BB &bb);
  void AddNullPointerInfoForVar();
  uint8_t AnalysisBranch(MeStmt &meStmt);
  void RemoveExtraNodes();
  void InsertPiNodes();
  bool ExistedPhiNode(BB &bb, VarMeExpr &rhs);
  void InsertPhiNodes();
  void Rename();
  void RenameStartPiBr(DefPoint &newDefPoint);
  void RenameStartPiArray(DefPoint &newDefPoint);
  void RenameStartPhi(DefPoint &newDefPoint);
  void ReplacePiPhiInSuccs(BB &bb, VarMeExpr &newVar);
  bool ReplaceStmt(MeStmt &meStmt, VarMeExpr &newVar, VarMeExpr &oldVar);
  void ReplaceBB(BB &bb, BB &parentBB, DefPoint &newDefPoint);
  bool ExistedPiNode(BB &bb, BB &parentBB, const VarMeExpr &rhs);
  void CreatePhi(VarMeExpr &rhs, BB &dfBB);
  VarMeExpr *CreateNewPiExpr(const MeExpr &opnd);
  void CreateNewPiStmt(VarMeExpr *lhs, MeExpr &rhs, BB &bb, MeStmt &generatedBy, bool isToken);
  void CreateNewPiStmt(VarMeExpr *lhs, MeExpr &rhs, MeStmt &generatedBy);
  MeExpr *ReplaceMeExprExpr(MeExpr &origExpr, MeExpr &oldVar, MeExpr &repExpr);
  MeExpr *NewMeExpr(MeExpr &meExpr);
  bool ReplaceMeExprStmtOpnd(uint32 opndID, MeStmt &meStmt, MeExpr &oldVar, MeExpr &newVar, bool update);
  bool ReplaceStmtWithNewVar(MeStmt &meStmt, MeExpr &oldVar, MeExpr &newVar, bool update);

  Dominance *GetDominace() {
    return dom;
  }

  MemPool *GetMemPool() {
    return memPool;
  }

  MapleAllocator &GetAllocator() {
    return allocator;
  }

  const MeIRMap *GetIRMap() const {
    return irMap;
  }

  MeFunction *meFunc;
  Dominance *dom;
  MeIRMap *irMap;
  MemPool *memPool;
  MapleAllocator allocator;
  SSIType ssiType{};
  std::vector<DefPoint*> newDefPoints;
  std::map<DefPoint*, VarMeExpr*> newDef2Old;
  std::map<std::pair<MeStmt*, size_t>, MeExpr*, StmtComparator> modifiedStmt;
  std::map<MePhiNode*, std::vector<ScalarMeExpr*>> modifiedPhi;
  std::set<BB*> visitedBBs;
  // used for ABC opt
  std::map<MeStmt*, NaryMeExpr*>* arrayChecks;
  std::map<BB*, std::vector<MeStmt*>>* containsBB;
  // used for check cast opt
  std::map<MeExpr*, MIRType*> inferredType;
};
}
#endif
