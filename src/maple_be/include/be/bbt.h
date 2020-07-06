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
#ifndef MAPLEBE_INCLUDE_BE_BBT_H
#define MAPLEBE_INCLUDE_BE_BBT_H
/* MapleIR headers. */
#include "mir_nodes.h"
#include "mir_lower.h"
namespace maplebe {
using namespace maple;

class BBT {
  /*
   * if stmt is a switch/rangegoto, succs gets defined, and condJumpBranch == fallthruBranch == nullptr.
   * otherwise, succs.size() ==0 &&
   *  1. for cond br stmt, both condJumpBranch and fallthruBranch are defined.
   *  2. if bb ends with 'throw', both fields get nullptr.
   *  3. for the others, condJumpBranch == nullptr && only fallthruBranch is defined
   */
 public:
  enum BBTType : uint8 {
    kBBPlain,
    kBBTry,
    kBBEndTry,
    kBBCatch
  };

  BBT(StmtNode *s, StmtNode *e, MemPool *memPool)
      : alloc(memPool),
        type(kBBPlain),
        succs(alloc.Adapter()),
        labelIdx(MIRLabelTable::GetDummyLabel()),
        firstStmt(s != nullptr ? s : e),
        lastStmt(e) {}

  ~BBT() = default;

  void Extend(const StmtNode *sNode, StmtNode *eNode) {
    CHECK_FATAL(lastStmt != nullptr, "nullptr check");
    CHECK_FATAL(sNode != nullptr ? lastStmt->GetNext() == sNode : lastStmt->GetNext() == eNode, "Extend fail");
    lastStmt = eNode;
  }

  void SetLabelIdx(LabelIdx li) {
    labelIdx = li;
  }

  bool IsLabeled() const {
    return labelIdx != MIRLabelTable::GetDummyLabel();
  }

  LabelIdx GetLabelIdx() const {
    return labelIdx;
  }

  void SetType(BBTType t, StmtNode &k) {
    type = t;
    keyStmt = &k;
  }

  bool IsTry() const {
    return type == kBBTry;
  }

  bool IsEndTry() const {
    return type == kBBEndTry;
  }

  bool IsCatch() const {
    return type == kBBCatch;
  }

  void AddSuccs(BBT *bb) {
    succs.emplace_back(bb);
  }

  void SetCondJumpBranch(BBT *bb) {
    condJumpBranch = bb;
  }

  BBT *GetCondJumpBranch() {
    return condJumpBranch;
  }
  void SetFallthruBranch(BBT *bb) {
    fallthruBranch = bb;
  }

  BBT *GetFallthruBranch() {
    return fallthruBranch;
  }

  StmtNode *GetFirstStmt() {
    return firstStmt;
  }

  void SetFirstStmt(StmtNode &stmt) {
    firstStmt = &stmt;
  }

  StmtNode *GetLastStmt() {
    return lastStmt;
  }

  void SetLastStmt(StmtNode &stmt) {
    lastStmt = &stmt;
  }

  StmtNode *GetKeyStmt() {
    return keyStmt;
  }

#if DEBUG
  void Dump(const MIRModule &mod) const;
  static void ValidateStmtList(StmtNode *head, StmtNode *detached = nullptr);
#endif
 private:
  MapleAllocator alloc;
  BBTType type;
  BBT *condJumpBranch = nullptr;
  BBT *fallthruBranch = nullptr;
  MapleVector<BBT*> succs;
  LabelIdx labelIdx;
  StmtNode *firstStmt;
  StmtNode *lastStmt;
  StmtNode *keyStmt = nullptr;
};
} /* namespace maplebe */

#endif /* MAPLEBE_INCLUDE_BE_BBT_H */