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
#ifndef MPLFE_INCLUDE_COMMON_GENERAL_STMT_H
#define MPLFE_INCLUDE_COMMON_GENERAL_STMT_H
#include <vector>
#include <memory>
#include "types_def.h"
#include "mpl_logging.h"
#include "fe_utils.h"

namespace maple {
enum GeneralStmtKind : uint8 {
  kStmtDefault = 0,
  kStmtDummyBegin,
  kStmtDummyEnd,
  kStmtMultiIn,
  kStmtMultiOut
};

class GeneralStmt : public FELinkListNode {
 public:
  GeneralStmt();
  explicit GeneralStmt(GeneralStmtKind argGenKind);
  virtual ~GeneralStmt() = default;
  GeneralStmtKind GetGeneralStmtKind() const {
    return genKind;
  }

  bool IsFallThru() const {
    return isFallThru;
  }

  void SetFallThru(bool arg) {
    isFallThru = arg;
  }

  bool IsAuxPre() const {
    return isAuxPre;
  }

  bool IsAuxPost() const {
    return isAuxPost;
  }

  bool IsAux() const {
    return isAuxPre || isAuxPost;
  }

  uint32 GetID() const {
    return id;
  }

  void SetID(uint32 arg) {
    id = arg;
  }

  const std::vector<GeneralStmt*> &GetPredsOrSuccs() const {
    return predsOrSuccs;
  }

  void AddPredOrSucc(GeneralStmt *stmt) {
    ASSERT(stmt != nullptr, "null ptr check");
    predsOrSuccs.push_back(stmt);
  }

  const std::vector<GeneralStmt*> &GetPreds() const {
    ASSERT(genKind == GeneralStmtKind::kStmtMultiIn, "invalid general kind");
    return predsOrSuccs;
  }

  const std::vector<GeneralStmt*> &GetSuccs() const {
    ASSERT(genKind == GeneralStmtKind::kStmtMultiOut, "invalid general kind");
    return predsOrSuccs;
  }

  void AddPred(GeneralStmt *stmt) {
    ASSERT(genKind == GeneralStmtKind::kStmtMultiIn, "invalid general kind");
    ASSERT(stmt != nullptr, "null ptr check");
    predsOrSuccs.push_back(stmt);
  }

  void AddSucc(GeneralStmt *stmt) {
    ASSERT(genKind == GeneralStmtKind::kStmtMultiOut, "invalid general kind");
    ASSERT(stmt != nullptr, "null ptr check");
    predsOrSuccs.push_back(stmt);
  }

  void Dump(const std::string &prefix = "") const {
    return DumpImpl(prefix);
  }

  std::string DumpDotString() const {
    return DumpDotStringImpl();
  }

  std::string GetStmtKindName() const {
    return GetStmtKindNameImpl();
  }

  bool IsStmtInst() const {
    return IsStmtInstImpl();
  }

 protected:
  virtual void DumpImpl(const std::string &prefix) const;
  virtual std::string DumpDotStringImpl() const;
  virtual std::string GetStmtKindNameImpl() const;
  virtual bool IsStmtInstImpl() const;

  GeneralStmtKind genKind : 4;
  bool isFallThru : 1;
  bool isAuxPre : 1;
  bool isAuxPost : 1;
  uint32 id;
  std::vector<GeneralStmt*> predsOrSuccs;
};

using UniqueGeneralStmt = std::unique_ptr<GeneralStmt>;
}  // namespace maple
#endif  // MPLFE_INCLUDE_COMMON_GENERAL_STMT_H