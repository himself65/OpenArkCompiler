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
#ifndef MAPLE_ME_INCLUDE_MESSUPRE_H
#define MAPLE_ME_INCLUDE_MESSUPRE_H
#include "me_function.h"
#include "me_irmap.h"
#include "me_dominance.h"

namespace maple {
enum SOccType {
  kSOccUndef,
  kSOccReal,
  kSOccLambda,
  kSOccLambdaRes,
  kSOccEntry,
  kSOccUse,
  kSOccPhi
};

class SOcc {
 public:
  SOcc(SOccType ty, BB &bb) : occTy(ty), classId(0), mirBB(bb), use(nullptr) {}

  virtual ~SOcc() = default;

  virtual void Dump() const = 0;
  bool IsPostDominate(Dominance *dom, const SOcc *occ) const {
    CHECK_NULL_FATAL(occ);
    CHECK_NULL_FATAL(dom);
    return dom->PostDominate(mirBB, occ->mirBB);
  }

  SOccType GetOccTy() const {
    return occTy;
  }

  void SetOccTy(SOccType type) {
    this->occTy = type;
  }

  uint32 GetClassId() const {
    return classId;
  }

  void SetClassId(uint32 id) {
    this->classId = id;
  }

  BB &GetBB() {
    return mirBB;
  }

  const BB &GetBB() const {
    return mirBB;
  }

  SOcc *GetUse() const {
    return use;
  }

  void SetUse(SOcc &currUse) {
    this->use = &currUse;
  }

 private:
  SOccType occTy;
  uint32 classId;
  BB &mirBB;  // the BB it occurs in
  SOcc *use;  // points to its single use
};

class SRealOcc : public SOcc {
 public:
  SRealOcc(BB &bb)
      : SOcc(kSOccReal, bb), meStmt(nullptr), vMeExpr(nullptr), realFromDef(false), redundant(true) {}
  explicit SRealOcc(MeStmt &s)
      : SOcc(kSOccReal, *s.GetBB()), meStmt(&s), vMeExpr(nullptr), realFromDef(false), redundant(true) {}
  SRealOcc(MeStmt &s, VarMeExpr &v)
      : SOcc(kSOccReal, *s.GetBB()), meStmt(&s), vMeExpr(&v), realFromDef(false), redundant(true) {}
  SRealOcc(BB &bb, VarMeExpr &v)
      : SOcc(kSOccReal, bb), meStmt(nullptr), vMeExpr(&v), realFromDef(false), redundant(true) {}
  virtual ~SRealOcc() = default;
  void Dump() const {
    LogInfo::MapleLogger() << "RealOcc at bb" << GetBB().GetBBId();
    if (realFromDef) {
      LogInfo::MapleLogger() << "(from-def)";
    }
    LogInfo::MapleLogger() << " classId" << GetClassId();
  }

  MeStmt *GetStmt() const {
    return meStmt;
  }

  VarMeExpr *GetVar() const {
    return vMeExpr;
  }

  bool GetRealFromDef() const {
    return realFromDef;
  }

  void SetRealFromDef(bool real) {
    this->realFromDef = real;
  }

  bool GetRedundant() const {
    return redundant;
  }

  void SetRedundant(bool isRedundant) {
    this->redundant = isRedundant;
  }

 private:
  MeStmt *meStmt;      // the stmt of this real occurrence; null for formal at entry
  VarMeExpr *vMeExpr;  // the varmeexpr of this real occurrence
  bool realFromDef;    // used only by placementRC
  bool redundant;
};

class SLambdaOcc;

class SLambdaResOcc : public SOcc {
 public:
  explicit SLambdaResOcc(BB &bb)
      : SOcc(kSOccLambdaRes, bb), useLambdaOcc(nullptr), hasRealUse(false), insertHere(false) {}

  virtual ~SLambdaResOcc() = default;
  void Dump() const {
    LogInfo::MapleLogger() << "LambdaResOcc at bb" << GetBB().GetBBId() << " classId" << GetClassId();
  }

  const SLambdaOcc *GetUseLambdaOcc() const {
    return useLambdaOcc;
  }

  void SetUseLambdaOcc(SLambdaOcc &currUseLambdaOcc) {
    this->useLambdaOcc = &currUseLambdaOcc;
  }

  bool GetHasRealUse() const {
    return hasRealUse;
  }

  void SetHasRealUse(bool has) {
    this->hasRealUse = has;
  }

  bool GetInsertHere() const {
    return insertHere;
  }

  void SetInsertHere(bool currInsertHere) {
    this->insertHere = currInsertHere;
  }

 private:
  SLambdaOcc *useLambdaOcc;  // its rhs use
  bool hasRealUse;
  bool insertHere;
};

class SLambdaOcc : public SOcc {
 public:
  SLambdaOcc(BB &bb, MapleAllocator &alloc)
      : SOcc(kSOccLambda, bb), isUpsafe(true), isCanBeAnt(true), isEarlier(true), lambdaRes(alloc.Adapter()) {}

  virtual ~SLambdaOcc() = default;
  bool WillBeAnt() const {
    return isCanBeAnt && !isEarlier;
  }

  void Dump() const {
    LogInfo::MapleLogger() << "LambdaOcc at bb" << GetBB().GetBBId() << " classId" << GetClassId() << " Lambda[";
    for (size_t i = 0; i < lambdaRes.size(); i++) {
      lambdaRes[i]->Dump();
      if (i != lambdaRes.size() - 1) {
        LogInfo::MapleLogger() << ", ";
      }
    }
    LogInfo::MapleLogger() << "]";
  }

  bool GetIsUpsafe() const {
    return isUpsafe;
  }

  void SetIsUpsafe(bool upsafe) {
    this->isUpsafe = upsafe;
  }

  bool GetIsCanBeAnt() const {
    return isCanBeAnt;
  }

  void SetIsCanBeAnt(bool canBeAnt) {
    this->isCanBeAnt = canBeAnt;
  }

  bool GetIsEarlier() const {
    return isEarlier;
  }

  void SetIsEarlier(bool earlier) {
    this->isEarlier = earlier;
  }

  MapleVector<SLambdaResOcc*> &GetLambdaRes() {
    return lambdaRes;
  }

 private:
  bool isUpsafe;
  bool isCanBeAnt;
  bool isEarlier;
  MapleVector<SLambdaResOcc*> lambdaRes;
};

class SEntryOcc : public SOcc {
 public:
  explicit SEntryOcc(BB &bb) : SOcc(kSOccEntry, bb) {}

  virtual ~SEntryOcc() = default;
  void Dump() const {
    LogInfo::MapleLogger() << "EntryOcc at bb" << GetBB().GetBBId();
  }
};

class SUseOcc : public SOcc {
 public:
  explicit SUseOcc(BB &bb) : SOcc(kSOccUse, bb) {}

  virtual ~SUseOcc() = default;
  void Dump() const {
    LogInfo::MapleLogger() << "UseOcc at bb" << GetBB().GetBBId();
  }
};

class SPhiOcc : public SOcc {
 public:
  SPhiOcc(BB &bb, MePhiNode &p, VarMeExpr &v) : SOcc(kSOccPhi, bb), phi(&p), vMeExpr(&v) {};

  virtual ~SPhiOcc() = default;

  MePhiNode *GetPhiNode() {
    return phi;
  }

  const MePhiNode *GetPhiNode() const {
    return phi;
  }

  VarMeExpr *GetVar() {
    return vMeExpr;
  }

  const VarMeExpr *GetVar() const {
    return vMeExpr;
  }

  void Dump() const {
    LogInfo::MapleLogger() << "PhiOcc at bb" << GetBB().GetBBId();
  }

 private:
  MePhiNode *phi;      // the phinode of this real occurrence;
  VarMeExpr *vMeExpr;  // the varmeexpr of this real occurrence
};

class SpreWorkCand {
 public:
  SpreWorkCand(MapleAllocator &alloc, const OriginalSt &ost)
      : next(nullptr),
        theOst(&ost),
        theVar(nullptr),
        realOccs(alloc.Adapter()),
        hasStoreOcc(false),
        hasCriticalEdge(false) {}

  ~SpreWorkCand() = default;

  const SpreWorkCand *GetNext() const {
    return next;
  }

  const OriginalSt *GetOst() const {
    return theOst;
  }

  const VarMeExpr *GetTheVar() const {
    return theVar;
  }

  void SetTheVar(VarMeExpr &var) {
    this->theVar = &var;
  }

  MapleVector<SOcc*> &GetRealOccs() {
    return realOccs;
  }

  bool GetHasStoreOcc() const {
    return hasStoreOcc;
  }

  void SetHasStoreOcc(bool has) {
    this->hasStoreOcc = has;
  }

  bool GetHasCriticalEdge() const {
    return hasCriticalEdge;
  }

  void SetHasCriticalEdge(bool criticalEdge) {
    this->hasCriticalEdge = criticalEdge;
  }

 private:
  SpreWorkCand *next;
  const OriginalSt *theOst;     // the stored symbol of this workcand
  VarMeExpr *theVar;            // any existing node of the lhs var
  MapleVector<SOcc*> realOccs;  // maintained in order of pdt_preorder
  bool hasStoreOcc;             // true if there is any store occurrence
  bool hasCriticalEdge;         // determined by Finalize step
};

class MeSSUPre {
 public:
  enum PreKind {
    kStorePre,
    kDecrefPre,
    kSecondDecrefPre,
    kSubsumePre
  } preKind;

  MeSSUPre(MeFunction &f, Dominance &dom, MemPool &memPool, PreKind kind, bool enabledDebug)
      : preKind(kind),
        func(&f),
        ssaTab(f.GetMeSSATab()),
        irMap(f.GetIRMap()),
        mirModule(&f.GetMeSSATab()->GetModule()),
        dom(&dom),
        spreMp(&memPool),
        spreAllocator(&memPool),
        workCandMap(std::less<OStIdx>(), spreAllocator.Adapter()),
        workCand(nullptr),
        lambdaDfns(std::less<uint32>(), spreAllocator.Adapter()),
        classCount(0),
        allOccs(spreAllocator.Adapter()),
        lambdaOccs(spreAllocator.Adapter()),
        entryOccs(spreAllocator.Adapter()),
        catchBlocks2Insert(spreAllocator.Adapter()),
        enabledDebug(enabledDebug) {}

  virtual ~MeSSUPre() = default;

  void ApplySSUPre();

 protected:
  // step 6 methods
  virtual void CodeMotion() = 0;
  // step 5 methods
  void Finalize();
  // step 4 methods
  void ResetCanBeAnt(SLambdaOcc &lambda) const;
  void ComputeCanBeAnt() const;
  void ResetEarlier(SLambdaOcc &lambda) const;
  void ComputeEarlier() const;
  void ResetCanBeFullyAnt(SLambdaOcc &lambda) const;
  void ComputeCanBeFullyAnt() const;
  // step 3 methods
  void ResetUpsafe(const SLambdaResOcc &lambdaRes) const;
  void ComputeUpsafe() const;
  // step 2 methods
  void Rename();
  // step 1 methods
  void GetIterPdomFrontier(const BB &bb, MapleSet<uint32> &pdfSet, std::vector<bool> &visitedMap);
  void FormLambdas();
  void FormLambdaRes();
  void CreateSortedOccs();
  // step 0 methods
  void CreateEntryOcc(BB &bb) {
    SEntryOcc *entryOcc = spreMp->New<SEntryOcc>(bb);
    entryOccs.push_back(entryOcc);
  }

  virtual void BuildWorkListBB(BB *bb) = 0;
  virtual void PerCandInit() = 0;
  virtual void CreateEmptyCleanupIntrinsics() {}

  MeFunction *func;
  SSATab *ssaTab;
  MeIRMap *irMap;
  MIRModule *mirModule;
  Dominance *dom;
  MemPool *spreMp;
  MapleAllocator spreAllocator;
  MapleMap<OStIdx, SpreWorkCand*> workCandMap;
  SpreWorkCand *workCand;  // current SpreWorkCand
  // step 1 lambda insertion data structures:
  // following are set of BBs in terms of their dfn's; index into
  // dominance->pdt_preorder to get their bbid's
  MapleSet<uint32> lambdaDfns;  // set by FormLambdas()
  // step 2 renaming
  uint32 classCount;  // for assigning new class id
  // the following 3 lists are all maintained in order of pdt_preorder
  MapleVector<SOcc*> allOccs;           // cleared at start of each workcand
  MapleVector<SLambdaOcc*> lambdaOccs;  // cleared at start of each workcand
  MapleVector<SEntryOcc*> entryOccs;    // this is shared by all workcands
  // used in steps 5 and 6
  MapleSet<BBId> catchBlocks2Insert;  // need insertions at entries to these catch blocks
  bool enabledDebug;
};
};  // namespace maple
#endif  // MAPLE_ME_INCLUDE_MESSUPRE_H
