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
#include "mir_symbol_builder.h"
#include <string>
#include <iostream>

namespace maple {
MIRSymbol *MIRSymbolBuilder::GetLocalDecl(MIRSymbolTable &symbolTable, GStrIdx strIdx) const {
  if (strIdx != 0) {
    StIdx stIdx = symbolTable.GetStIdxFromStrIdx(strIdx);
    if (stIdx.FullIdx() != 0) {
      return symbolTable.GetSymbolFromStIdx(stIdx.Idx());
    }
  }
  return nullptr;
}

MIRSymbol *MIRSymbolBuilder::CreateLocalDecl(MIRSymbolTable &symbolTable, GStrIdx strIdx,
                                             const MIRType &type) const {
  MIRSymbol *st = symbolTable.CreateSymbol(kScopeLocal);
  st->SetNameStrIdx(strIdx);
  st->SetTyIdx(type.GetTypeIndex());
  (void)symbolTable.AddToStringSymbolMap(*st);
  st->SetStorageClass(kScAuto);
  st->SetSKind(kStVar);
  return st;
}

MIRSymbol *MIRSymbolBuilder::GetGlobalDecl(GStrIdx strIdx) const {
  if (strIdx != 0) {
    StIdx stIdx = GlobalTables::GetGsymTable().GetStIdxFromStrIdx(strIdx);
    if (stIdx.FullIdx() != 0) {
      return GlobalTables::GetGsymTable().GetSymbolFromStidx(stIdx.Idx());
    }
  }
  return nullptr;
}

MIRSymbol *MIRSymbolBuilder::CreateGlobalDecl(GStrIdx strIdx, const MIRType &type, MIRStorageClass sc) const {
  MIRSymbol *st = GlobalTables::GetGsymTable().CreateSymbol(kScopeGlobal);
  st->SetNameStrIdx(strIdx);
  st->SetTyIdx(type.GetTypeIndex());
  (void)GlobalTables::GetGsymTable().AddToStringSymbolMap(*st);
  st->SetStorageClass(sc);
  st->SetSKind(kStVar);
  return st;
}

// when sametype is true, it means match everything the of the symbol
MIRSymbol *MIRSymbolBuilder::GetSymbol(TyIdx tyIdx, GStrIdx strIdx, MIRSymKind mClass, MIRStorageClass sClass,
                                       uint8 scpID, bool sameType) const {
  if (scpID != kScopeGlobal) {
    ERR(kLncErr, "not yet implemented");
    return nullptr;
  }

  MIRSymbol *st = GlobalTables::GetGsymTable().GetSymbolFromStrIdx(strIdx);
  if (st == nullptr || st->GetTyIdx() != tyIdx) {
    return nullptr;
  }

  if (sameType) {
    if (st->GetStorageClass() == sClass && st->GetSKind() == mClass) {
      return st;
    }
    return nullptr;
  }
  ASSERT(mClass == st->GetSKind(),
         "trying to create a new symbol that has the same name and GtyIdx. might cause problem");
  ASSERT(sClass == st->GetStorageClass(),
         "trying to create a new symbol that has the same name and tyIdx. might cause problem");
  return st;
}

// when func is null, create global symbol, otherwise create local symbol
MIRSymbol *MIRSymbolBuilder::CreateSymbol(TyIdx tyIdx, GStrIdx strIdx, MIRSymKind mClass, MIRStorageClass sClass,
                                          MIRFunction *func, uint8 scpID) const {
  MIRSymbol *st =
      func != nullptr ? func->GetSymTab()->CreateSymbol(scpID) : GlobalTables::GetGsymTable().CreateSymbol(scpID);
  CHECK_FATAL(st != nullptr, "Failed to create MIRSymbol");
  st->SetStorageClass(sClass);
  st->SetSKind(mClass);
  st->SetNameStrIdx(strIdx);
  st->SetTyIdx(tyIdx);
  if (func) {
    (void)func->GetSymTab()->AddToStringSymbolMap(*st);
  } else {
    (void)GlobalTables::GetGsymTable().AddToStringSymbolMap(*st);
  }
  return st;
}

MIRSymbol *MIRSymbolBuilder::CreatePregFormalSymbol(TyIdx tyIdx, PregIdx pRegIdx, MIRFunction &func) const {
  MIRSymbol *st = func.GetSymTab()->CreateSymbol(kScopeLocal);
  CHECK_FATAL(st != nullptr, "Failed to create MIRSymbol");
  st->SetStorageClass(kScFormal);
  st->SetSKind(kStPreg);
  st->SetTyIdx(tyIdx);
  MIRPregTable *pRegTab = func.GetPregTab();
  st->SetPreg(pRegTab->PregFromPregIdx(pRegIdx));
  return st;
}
} // maple
