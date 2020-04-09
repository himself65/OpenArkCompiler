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
#include "general_stmt.h"
#include <iostream>
#include <sstream>

namespace maple {
// ---------- GeneralStmt ----------
GeneralStmt::GeneralStmt()
    : genKind(kStmtDefault), isFallThru(true), isAuxPre(false), isAuxPost(false), id(0) {
}

GeneralStmt::GeneralStmt(GeneralStmtKind argGenKind)
    : genKind(argGenKind), isFallThru(true), isAuxPre(false), isAuxPost(false), id(0) {
}

void GeneralStmt::DumpImpl(const std::string &prefix) const {
  std::cout << prefix << "GeneralStmt" << id << "(kind=" << GetStmtKindName() << ")" << std::endl;
}

std::string GeneralStmt::DumpDotStringImpl() const {
  std::stringstream ss;
  ss << "<stmt" << id << "> " << id << ": " << GetStmtKindName();
  return ss.str();
}

std::string GeneralStmt::GetStmtKindNameImpl() const {
  switch (genKind) {
    case kStmtDefault:
      return "Default";
    case kStmtDummyBegin:
      return "DummyBegin";
    case kStmtDummyEnd:
      return "DummyEnd";
    case kStmtMultiIn:
      return "MultiIn";
    case kStmtMultiOut:
      return "MultiOut";
    default:
      return "Unknown";
  }
}

bool GeneralStmt::IsStmtInstImpl() const {
  return false;
}
}  // namespace maple