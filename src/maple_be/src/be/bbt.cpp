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
#include "bbt.h"
namespace maplebe {
#if DEBUG
void BBT::Dump(const MIRModule &mod) const {
  if (IsTry()) {
    LogInfo::MapleLogger() << "Try" << '\n';
  } else if (IsEndTry()) {
    LogInfo::MapleLogger() << "EndTry" << '\n';
  } else if (IsCatch()) {
    LogInfo::MapleLogger() << "Catch" << '\n';
  } else {
    LogInfo::MapleLogger() << "Plain" << '\n';
  }
  if (firstStmt != nullptr) {
    firstStmt->Dump(0);
    LogInfo::MapleLogger() << '\n';
    if (keyStmt != nullptr) {
      keyStmt->Dump(0);
      LogInfo::MapleLogger() << '\n';
    } else {
      LogInfo::MapleLogger() << "<<No-Key-Stmt>>" << '\n';
    }
    if (lastStmt != nullptr) {
      lastStmt->Dump(0);
    }
    LogInfo::MapleLogger() << '\n';
  } else {
    LogInfo::MapleLogger() << "<<Empty>>" << '\n';
  }
}

void BBT::ValidateStmtList(StmtNode *head, StmtNode *detached) {
  static int nStmts = 0;
  int n = 0;
  int m = 0;
  if (head == nullptr && detached == nullptr) {
    nStmts = 0;
    return;
  }
  for (StmtNode *s = head; s != nullptr; s = s->GetNext()) {
    if (s->GetNext() != nullptr) {
      CHECK_FATAL(s->GetNext()->GetPrev() == s, "make sure the prev node of s' next is s");
    }
    if (s->GetPrev() != nullptr) {
      CHECK_FATAL(s->GetPrev()->GetNext() == s, "make sure the next node of s' prev is s");
    }
    ++n;
  }
  for (StmtNode *s = detached; s != nullptr; s = s->GetNext()) {
    if (s->GetNext() != nullptr) {
      CHECK_FATAL(s->GetNext()->GetPrev() == s, "make sure the prev node of s' next is s");
    }
    if (s->GetPrev() != nullptr) {
      CHECK_FATAL(s->GetPrev()->GetNext() == s, "make sure the next node of s' prev is s");
    }
    ++m;
  }
  CHECK_FATAL(nStmts <= n + m, "make sure nStmts <= n + m");
  nStmts = n + m;
}
#endif
} /* namespace maplebe */
