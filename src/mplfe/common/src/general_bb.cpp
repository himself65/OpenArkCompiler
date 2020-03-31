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
#include "general_bb.h"

namespace maple {
GeneralBB::GeneralBB(uint8 argKind)
    : kind(argKind),
      stmtHead(nullptr),
      stmtTail(nullptr),
      stmtNoAuxHead(nullptr),
      stmtNoAuxTail(nullptr),
      id(0) {}

GeneralBB::GeneralBB()
    : GeneralBB(GeneralBBKind::kBBKindDefault) {}

GeneralBB::~GeneralBB() {
  stmtHead = nullptr;
  stmtTail = nullptr;
  stmtNoAuxHead = nullptr;
  stmtNoAuxTail = nullptr;
}

void GeneralBB::AppendStmt(const GeneralStmt *stmt) {
  if (stmt == nullptr) {
    return;
  }
  if (stmtHead == nullptr) {
    stmtHead = stmt;
  }
  stmtTail = stmt;
  if (stmt->IsAux() == false) {
    if (stmtNoAuxHead == nullptr) {
      stmtNoAuxHead = stmt;
    }
    stmtNoAuxTail = stmt;
  }
}

void GeneralBB::AddStmtAuxPre(const GeneralStmt *stmt) {
  if (stmt == nullptr || stmt->IsAuxPre() == false) {
    return;
  }
  stmtHead = stmt;
}

void GeneralBB::AddStmtAuxPost(const GeneralStmt *stmt) {
  if (stmt == nullptr || stmt->IsAuxPost() == false) {
    return;
  }
  stmtTail = stmt;
}

bool GeneralBB::IsPredBB(uint32 bbID) {
  for (GeneralBB *bb : predBBs) {
    if (bb->GetID() == bbID) {
      return true;
    }
  }
  return false;
}

bool GeneralBB::IsSuccBB(uint32 bbID) {
  for (GeneralBB *bb : succBBs) {
    if (bb->GetID() == bbID) {
      return true;
    }
  }
  return false;
}

bool GeneralBB::IsDeadImpl() {
  return predBBs.size() == 0;
}

void GeneralBB::DumpImpl() const {
  std::cout << "GeneralBB (id=" << id << ", kind=" << GetBBKindName() <<
               ", preds={";
  for (GeneralBB *bb : predBBs) {
    std::cout << bb->GetID() << " ";
  }
  std::cout << "}, succs={";
  for (GeneralBB *bb : succBBs) {
    std::cout << bb->GetID() << " ";
  }
  std::cout << "})" << std::endl;
}

std::string GeneralBB::GetBBKindNameImpl() const {
  switch (kind) {
    case kBBKindDefault:
      return "Default";
    case kBBKindPesudoHead:
      return "PesudoHead";
    case kBBKindPesudoTail:
      return "PesudoTail";
    default:
      return "unknown";
  }
}
}  // namespace maple