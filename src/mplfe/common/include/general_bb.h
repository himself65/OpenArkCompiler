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
#ifndef MPLFE_INCLUDE_COMMON_GENERAL_BB_H
#define MPLFE_INCLUDE_COMMON_GENERAL_BB_H
#include <vector>
#include "types_def.h"
#include "mempool_allocator.h"
#include "safe_ptr.h"
#include "fe_utils.h"
#include "general_stmt.h"

namespace maple {
enum GeneralBBKind : uint8 {
  kBBKindDefault,
  kBBKindPesudoHead,
  kBBKindPesudoTail,
  kBBKindExt
};

class GeneralBB : public FELinkListNode {
 public:
  GeneralBB();
  explicit GeneralBB(uint8 argKind);
  virtual ~GeneralBB();
  void AppendStmt(const GeneralStmt *stmt);
  void AddStmtAuxPre(const GeneralStmt *stmt);
  void AddStmtAuxPost(const GeneralStmt *stmt);
  bool IsPredBB(uint32 bbID);
  bool IsSuccBB(uint32 bbID);
  uint8 GetBBKind() const {
    return kind;
  }

  const GeneralStmt *GetStmtHead() const {
    return stmtHead;
  }

  const GeneralStmt *GetStmtTail() const {
    return stmtTail;
  }

  const GeneralStmt *GetStmtNoAuxHead() const {
    return stmtNoAuxHead;
  }

  const GeneralStmt *GetStmtNoAuxTail() const {
    return stmtNoAuxTail;
  }

  void AddPredBB(GeneralBB *bb) {
    if (predBBs.find(bb) == predBBs.end()) {
      CHECK_FATAL(predBBs.insert(bb).second, "predBBs insert failed");
    }
  }

  void AddSuccBB(GeneralBB *bb) {
    if (succBBs.find(bb) == succBBs.end()) {
      CHECK_FATAL(succBBs.insert(bb).second, "succBBs insert failed");
    }
  }

  const std::set<GeneralBB*> &GetPredBBs() const {
    return predBBs;
  }

  const std::set<GeneralBB*> &GetSuccBBs() const {
    return succBBs;
  }

  uint32 GetID() const {
    return id;
  }

  void SetID(uint32 arg) {
    id = arg;
  }

  bool IsPredBB(GeneralBB *bb) {
    return predBBs.find(bb) != predBBs.end();
  }

  bool IsSuccBB(GeneralBB *bb) {
    return succBBs.find(bb) != succBBs.end();
  }

  bool IsDead() {
    return IsDeadImpl();
  }

  void Dump() const {
    return DumpImpl();
  }

  std::string GetBBKindName() const {
    return GetBBKindNameImpl();
  }

 protected:
  virtual bool IsDeadImpl();
  virtual void DumpImpl() const;
  virtual std::string GetBBKindNameImpl() const;

  uint8 kind;
  const GeneralStmt *stmtHead;
  const GeneralStmt *stmtTail;
  const GeneralStmt *stmtNoAuxHead;
  const GeneralStmt *stmtNoAuxTail;
  std::set<GeneralBB*> predBBs;
  std::set<GeneralBB*> succBBs;
  uint32 id;
};
}  // namespace maple
#endif  // MPLFE_INCLUDE_COMMON_GENERAL_BB_H