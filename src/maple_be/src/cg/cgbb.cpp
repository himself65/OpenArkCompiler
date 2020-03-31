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
#include "cgbb.h"

namespace maplebe {
const std::string BB::bbNames[BB::kBBLast] = {
  "BB_ft",
  "BB_if",
  "BB_goto",
  "BB_ret",
  "BB_intrinsic",
  "BB_rangegoto",
  "BB_throw"
};

Insn *BB::InsertInsnBefore(Insn &existing, Insn &newInsn) {
  Insn *pre = existing.GetPrev();
  newInsn.SetPrev(pre);
  newInsn.SetNext(&existing);
  existing.SetPrev(&newInsn);
  if (pre != nullptr) {
    pre->SetNext(&newInsn);
  }
  if (&existing == firstInsn) {
    firstInsn = &newInsn;
  }
  newInsn.SetBB(this);
  return &newInsn;
}

Insn *BB::InsertInsnAfter(Insn &existing, Insn &newInsn) {
  newInsn.SetPrev(&existing);
  newInsn.SetNext(existing.GetNext());
  existing.SetNext(&newInsn);
  if (&existing == lastInsn) {
    lastInsn = &newInsn;
  } else if (newInsn.GetNext()) {
    newInsn.GetNext()->SetPrev(&newInsn);
  }
  newInsn.SetBB(this);
  return &newInsn;
}

void BB::ReplaceInsn(Insn &insn, Insn &newInsn) {
  if (insn.IsAccessRefField()) {
    newInsn.MarkAsAccessRefField(true);
  }
  if (insn.GetDoNotRemove()) {
    newInsn.SetDoNotRemove(true);
  }
  newInsn.SetPrev(insn.GetPrev());
  newInsn.SetNext(insn.GetNext());
  if (&insn == lastInsn) {
    lastInsn = &newInsn;
  } else if (newInsn.GetNext() != nullptr) {
    newInsn.GetNext()->SetPrev(&newInsn);
  }
  if (firstInsn == &insn) {
    firstInsn = &newInsn;
  } else if (newInsn.GetPrev() != nullptr) {
    newInsn.GetPrev()->SetNext(&newInsn);
  }
  newInsn.SetBB(this);
}

void BB::RemoveInsn(Insn &insn) {
  if ((firstInsn == &insn) && (lastInsn == &insn)) {
    firstInsn = lastInsn = nullptr;
  } else if (firstInsn == &insn) {
    firstInsn = insn.GetNext();
  } else if (lastInsn == &insn) {
    lastInsn = insn.GetPrev();
  }
  /* remove insn from lir list */
  Insn *prevInsn = insn.GetPrev();
  Insn *nextInsn = insn.GetNext();
  if (prevInsn != nullptr) {
    prevInsn->SetNext(nextInsn);
  }
  if (nextInsn != nullptr) {
    nextInsn->SetPrev(prevInsn);
  }
}

void BB::RemoveInsnPair(Insn &insn, Insn &nextInsn) {
  ASSERT(insn.GetNext() == &nextInsn, "next_insn is supposed to follow insn");
  ASSERT(nextInsn.GetPrev() == &insn, "next_insn is supposed to follow insn");
  if ((firstInsn == &insn) && (lastInsn == &nextInsn)) {
    firstInsn = lastInsn = nullptr;
  } else if (firstInsn == &insn) {
    firstInsn = nextInsn.GetNext();
  } else if (lastInsn == &nextInsn) {
    lastInsn = insn.GetPrev();
  }
  if (insn.GetPrev() != nullptr) {
    insn.GetPrev()->SetNext(nextInsn.GetNext());
  }
  if (nextInsn.GetNext() != nullptr) {
    nextInsn.GetNext()->SetPrev(insn.GetPrev());
  }
}

/* Remove insns in this bb from insn1 to insn2. */
void BB::RemoveInsnSequence(Insn &insn1, Insn &insn2) {
  ASSERT(insn1.GetBB() == this, "remove insn sequence in one bb");
  ASSERT(insn2.GetBB() == this, "remove insn sequence in one bb");
  if ((firstInsn == &insn1) && (lastInsn == &insn2)) {
    firstInsn = lastInsn = nullptr;
  } else if (firstInsn == &insn1) {
    firstInsn = insn2.GetNext();
  } else if (lastInsn == &insn2) {
    lastInsn = insn1.GetPrev();
  }

  if (insn1.GetPrev() != nullptr) {
    insn1.GetPrev()->SetNext(insn2.GetNext());
  }
  if (insn2.GetNext() != nullptr) {
    insn2.GetNext()->SetPrev(insn1.GetPrev());
  }
}

/* append all insns from bb into this bb */
void BB::AppendBBInsns(BB &bb) {
  if (firstInsn == nullptr) {
    firstInsn = bb.firstInsn;
    lastInsn = bb.lastInsn;
    if (firstInsn != nullptr) {
      FOR_BB_INSNS(i, &bb) {
        i->SetBB(this);
      }
    }
    return;
  }
  if ((bb.firstInsn == nullptr) || (bb.lastInsn == nullptr)) {
    return;
  }
  FOR_BB_INSNS_SAFE(insn, &bb, nextInsn) {
    AppendInsn(*insn);
  }
}

/* append all insns from bb into this bb */
void BB::InsertAtBeginning(BB &bb) {
  if (bb.firstInsn == nullptr) { /* nothing to add */
    return;
  }

  FOR_BB_INSNS(insn, &bb) {
    insn->SetBB(this);
  }

  if (firstInsn == nullptr) {
    firstInsn = bb.firstInsn;
    lastInsn = bb.lastInsn;
  } else {
    bb.lastInsn->SetNext(firstInsn);
    firstInsn->SetPrev(bb.lastInsn);
    firstInsn = bb.firstInsn;
  }
  bb.firstInsn = bb.lastInsn = nullptr;
}

/* Number of instructions excluding DbgInsn and comments */
int32 BB::NumInsn() const {
  int32 bbSize = 0;
  FOR_BB_INSNS_CONST(i, this) {
    if (i->IsImmaterialInsn()) {
      continue;
    }
    ++bbSize;
  }
  return bbSize;
}

void BB::Dump() const {
  LogInfo::MapleLogger() << "=== BB " << this << " <" << GetKindName();
  if (labIdx) {
    LogInfo::MapleLogger() << "[labeled with " << labIdx << "]";
  }
  LogInfo::MapleLogger() << "> <" << id << "> ";
  if (isCleanup) {
    LogInfo::MapleLogger() << "[is_cleanup] ";
  }
  if (unreachable) {
    LogInfo::MapleLogger() << "[unreachable] ";
  }
  LogInfo::MapleLogger() << "frequency:" << frequency << "===\n";

  Insn *insn = firstInsn;
  while (insn != nullptr) {
    insn->Dump();
    insn = insn->GetNext();
  }
}

bool BB::IsCommentBB() const {
  if (GetKind() != kBBFallthru) {
    return false;
  }
  FOR_BB_INSNS_CONST(insn, this) {
    if (insn->IsMachineInstruction()) {
      return false;
    }
  }
  return true;
}

/* return true if bb has no real insns. */
bool BB::IsEmptyOrCommentOnly() const {
  return (IsEmpty() || IsCommentBB());
}

bool BB::IsSoloGoto() const {
  if (GetKind() != kBBGoto) {
    return false;
  }
  FOR_BB_INSNS_CONST(insn, this) {
    if (!insn->IsMachineInstruction()) {
      continue;
    }
    return (insn->IsGoto());
  }
  return false;
}
}  /* namespace maplebe */
