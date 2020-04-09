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
#ifndef MAPLEBE_INCLUDE_CG_CG_CFG_H
#define MAPLEBE_INCLUDE_CG_CG_CFG_H
#include "eh_func.h"
#include "cgbb.h"

namespace maplebe {
class CGCFG {
 public:
  explicit CGCFG(CGFunc &cgFunc) : cgFunc(&cgFunc) {}

  ~CGCFG() = default;

  void BuildCFG();
  static bool AreCommentAllPreds(const BB &bb);
  bool CanMerge(const BB &merger, const BB &mergee) const;
  bool BBJudge(const BB &first, const BB &second) const;
  /*
   * Merge all instructions in mergee into merger, each BB's successors and
   * predecessors should be modified accordingly.
   */
  static void MergeBB(BB &merger, BB &mergee, CGFunc &func);
  /* Loop up if the given label is in the exception tables in LSDA */
  static bool InLSDA(LabelIdx label, const EHFunc &ehFunc);
  static bool InSwitchTable(LabelIdx label, const CGFunc &func);
  Insn *FindLastCondBrInsn(BB &bb) const;
  static void FindAndMarkUnreachable(CGFunc &func);
  void FlushUnReachableStatusAndRemoveRelations(BB &curBB, const CGFunc &func) const;
  void UnreachCodeAnalysis();
  BB *FindLastRetBB();
 /* cgcfgvisitor */
 private:
  CGFunc *cgFunc;
  static void MergeBB(BB &merger, BB &mergee);
};  /* class CGCFG */
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_CG_CFG_H */