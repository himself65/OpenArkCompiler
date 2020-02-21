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
#ifndef MAPLE_ME_INCLUDE_MECONDBASED_H
#define MAPLE_ME_INCLUDE_MECONDBASED_H

#include "me_function.h"
#include "me_phase.h"
#include "me_irmap.h"

namespace maple {
class MeCondBased {
 public:
  MeCondBased(MeFunction *func, Dominance *dom) : func(func), dominance(dom) {}

  ~MeCondBased() = default;
  bool NullValueFromTestCond(VarMeExpr&, BB&, bool);
  bool IsNotNullValue(VarMeExpr&, UnaryMeStmt&, BB*);

  const MeFunction *GetFunc() const {
    return func;
  }

 private:
  bool NullValueFromOneTestCond(VarMeExpr&, BB&, BB&, bool);
  bool PointerWasDereferencedBefore(VarMeExpr&, UnaryMeStmt&, BB*);
  bool PointerWasDereferencedRightAfter(VarMeExpr&, UnaryMeStmt&);
  bool IsIreadWithTheBase(VarMeExpr&, MeExpr&);
  bool StmtHasDereferencedBase(MeStmt&, VarMeExpr&);

  MeFunction *func;
  Dominance *dominance;
};
}  // namespace maple
#endif  // MAPLE_ME_INCLUDE_MECONDBASED_H
