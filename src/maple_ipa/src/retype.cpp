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
#include "retype.h"
#include <iostream>
#include <algorithm>

namespace maple {
void Retype::ReplaceRetypeExpr(const BaseNode &expr) const {
  if (expr.NumOpnds() == 0) {
    return;
  }
  for (size_t i = 0; i < expr.NumOpnds(); ++i) {
    BaseNode *opnd = expr.Opnd(i);
    if (opnd->GetOpCode() == OP_retype) {
      opnd->SetOpnd(opnd->Opnd(0), i);
      continue;
    }
    ReplaceRetypeExpr(*opnd);
  }
}

void Retype::RetypeStmt(MIRFunction &func) const {
  if (func.IsEmpty()) {
    return;
  }
  for (auto &stmt : func.GetBody()->GetStmtNodes()) {
    if (stmt.GetOpCode() == OP_comment) {
      continue;
    }
    for (size_t i = 0; i < stmt.NumOpnds(); i++) {
      BaseNode *opnd = stmt.Opnd(i);
      if (opnd->GetOpCode() == OP_retype) {
        stmt.SetOpnd(opnd->Opnd(0), i);
        continue;
      } else {
        ReplaceRetypeExpr(*opnd);
      }
    }
  }
}

void Retype::DoRetype() const {
  for (MIRFunction *func : mirModule->GetFunctionList()) {
    if (func->IsEmpty()) {
      continue;
    }
    RetypeStmt(*func);
  }
}
}  // namespace maple
