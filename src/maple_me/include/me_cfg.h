/*
 * Copyright (c) [2019] Huawei Technologies Co.,Ltd.All rights reserved.
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
#ifndef MAPLE_ME_INCLUDE_ME_CFG_H
#define MAPLE_ME_INCLUDE_ME_CFG_H
#include "me_function.h"
#include "me_phase.h"

namespace maple {
class MeCFG {
 public:
  explicit MeCFG(MeFunction &f) : func(f) {}

  ~MeCFG() = default;

  void BuildMirCFG();
  void FixMirCFG();
  void ConvertPhis2IdentityAssigns(BB &meBB) const;
  void UnreachCodeAnalysis(bool updatePhi = false);
  void WontExitAnalysis();
  void Verify() const;
  void VerifyLabels() const;
  void Dump() const;
  void DumpToFile(const std::string &prefix, bool dumpInStrs = false) const;
  bool FindExprUse(const BaseNode &expr, StIdx stIdx) const;
  bool FindUse(const StmtNode &stmt, StIdx stIdx) const;
  bool FindDef(const StmtNode &stmt, StIdx stIdx) const;
  bool HasNoOccBetween(StmtNode &from, const StmtNode &to, StIdx stIdx) const;

  const MeFunction &GetFunc() const {
    return func;
  }

  bool GetHasDoWhile() const {
    return hasDoWhile;
  }

  void SetHasDoWhile(bool hdw) {
    hasDoWhile = hdw;
  }

 private:
  MeFunction &func;
  bool hasDoWhile = false;
};
}  // namespace maple
#endif  // MAPLE_ME_INCLUDE_ME_CFG_H
