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
#ifndef MAPLEBE_INCLUDE_EH_EH_FUNC_H
#define MAPLEBE_INCLUDE_EH_EH_FUNC_H
#include "mir_parser.h"
#include "mir_function.h"
#include "lsda.h"
#include "cg_phase.h"

namespace maplebe {
class EHTry {
 public:
  EHTry(MapleAllocator &alloc, TryNode &tryNode)
      : tryNode(&tryNode),
        catchVec(alloc.Adapter()) {}
  ~EHTry() = default;

  TryNode *GetTryNode() const {
    return tryNode;
  }

  void SetEndtryNode(StmtNode &endtryNode) {
    this->endTryNode = &endtryNode;
  }

  StmtNode *GetEndtryNode() {
    return endTryNode;
  }

  void SetFallthruGoto(StmtNode *fallthruGoto) {
    this->fallThroughGoto = fallthruGoto;
  }

  StmtNode *GetFallthruGoto() {
    return fallThroughGoto;
  }

  size_t GetCatchVecSize() const {
    return catchVec.size();
  }

  void PushBackCatchVec(CatchNode &catchNode) {
    catchVec.push_back(&catchNode);
  }

  CatchNode *GetCatchNodeAt(size_t pos) const {
    CHECK_FATAL(pos < GetCatchVecSize(), "pos is out of range.");
    return catchVec.at(pos);
  }

  void SetLSDACallSite(LSDACallSite &lsdaCallSite) {
    this->lsdaCallSite = &lsdaCallSite;
  }

  void SetCSAction(uint32 action) {
    lsdaCallSite->csAction = action;
  }

  void DumpEHTry(const MIRModule &mirModule);

 private:
  TryNode *tryNode;
  StmtNode *endTryNode = nullptr;
  StmtNode *fallThroughGoto = nullptr;  /* no throw in the try block, the goto stmt to the fall through */
  MapleVector<CatchNode*> catchVec;
  LSDACallSite *lsdaCallSite = nullptr;  /* one try has a callsite */
};

class EHThrow {
 public:
  explicit EHThrow(UnaryStmtNode &rtNode)
      : rethrow(&rtNode) {}
  ~EHThrow() = default;

  bool IsUnderTry() const {
    return javaTry != nullptr;
  }

  bool HasLSDA() const {
    return startLabel != nullptr;
  }

  const UnaryStmtNode *GetRethrow() const {
    return rethrow;
  }

  void SetJavaTry(EHTry *javaTry) {
    this->javaTry = javaTry;
  }

  LabelNode *GetStartLabel() {
    return startLabel;
  }

  LabelNode *GetEndLabel() {
    return endLabel;
  }

  void Lower(CGFunc &cgFunc);
  void ConvertThrowToRethrow(CGFunc &cgFunc);
  void ConvertThrowToRuntime(CGFunc&, BaseNode&);

 private:
  UnaryStmtNode *rethrow;            /* must be a throw stmt */
  EHTry *javaTry = nullptr;          /* the try statement wrapping this throw */
  LabelNode *startLabel = nullptr;   /* the label that "MCC_RethrowException" or "MCC_ThrowException" begin */
  LabelNode *endLabel = nullptr;     /* the label that "MCC_RethrowException" or "MCC_ThrowException" end */
};

class EHFunc {
 public:
  static constexpr uint8 kTypeEncoding = 0x9b;  /* same thing as LSDAHeader.kTypeEncoding */
  explicit EHFunc(CGFunc &func);
  ~EHFunc() = default;

  void CollectEHInformation(std::vector<std::pair<LabelIdx, CatchNode*>> &catchVec);
  void InsertEHSwitchTable();
  void CreateLSDA();
  bool NeedFullLSDA() const;
  bool NeedFastLSDA() const;
  void InsertCxaAfterEachCatch(const std::vector<std::pair<LabelIdx, CatchNode*>> &catchVec);
  void GenerateCleanupLabel();
  void MergeCatchToTry(const std::vector<std::pair<LabelIdx, CatchNode*>> &catchVec);
  void BuildEHTypeTable(const std::vector<std::pair<LabelIdx, CatchNode*>> &catchVec);
  void LowerThrow();  /* for non-personality function */
  void CreateTypeInfoSt();
  void DumpEHFunc() const;

  bool HasThrow() const {
    return !rethrowVec.empty();
  }

  void AddTry(EHTry &ehTry) {
    tryVec.push_back(&ehTry);
  }

  size_t GetEHTyTableSize() const {
    return ehTyTable.size();
  }

  TyIdx &GetEHTyTableMember(int32 index) {
    CHECK_FATAL(static_cast<size_t>(index) < ehTyTable.size(), "out of ehTyTable");
    return ehTyTable[index];
  }

  LSDAHeader *GetLSDAHeader() {
    return lsdaHeader;
  }

  LSDACallSiteTable *GetLSDACallSiteTable() {
    return lsdaCallSiteTable;
  }

  const LSDACallSiteTable *GetLSDACallSiteTable() const {
    return lsdaCallSiteTable;
  }

  const LSDAActionTable *GetLSDAActionTable() const {
    return lsdaActionTable;
  }

  void AddRethrow(EHThrow &rethrow) {
    rethrowVec.push_back(&rethrow);
  }

 private:
  void CreateLSDAAction();
  void InsertDefaultLabelAndAbortFunc(BlockNode &blkNode, SwitchNode &switchNode, StmtNode &beforeEndLabel);
  void FillSwitchTable(SwitchNode &switchNode, const EHTry &ehTry);
  void CreateLSDAHeader();
  void FillLSDACallSiteTable();
  LabelIdx CreateLabel(const std::string &cstr);
  bool HasTry() const;

  CGFunc *cgFunc;
  LabelIdx labelIdx = 0;
  MapleVector<EHTry*> tryVec;             /* try stmt node */
  MapleVector<TyIdx> ehTyTable;           /* the type that would emit in LSDA */
  MapleMap<TyIdx, uint32> ty2IndexTable;  /* use the TyIdx to get the index of ehTyTable; */
  LSDAHeader *lsdaHeader = nullptr;
  LSDACallSiteTable *lsdaCallSiteTable = nullptr;
  LSDAActionTable *lsdaActionTable = nullptr;
  MapleVector<EHThrow*> rethrowVec;       /* EHRethrow */
};

CGFUNCPHASE(CgDoBuildEHFunc, "buildehfunc")
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_EH_EH_FUNC_H */