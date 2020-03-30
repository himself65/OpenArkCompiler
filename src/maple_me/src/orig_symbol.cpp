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
#include "orig_symbol.h"

namespace maple {
bool OriginalSt::Equal(const OriginalSt &ost) const {
  if (IsSymbolOst()) {
    return symOrPreg.mirSt == ost.symOrPreg.mirSt &&
           fieldID == ost.GetFieldID() &&
           GetIndirectLev() == ost.GetIndirectLev();
  }
  if (IsPregOst()) {
    return symOrPreg.pregIdx == ost.symOrPreg.pregIdx && GetIndirectLev() == ost.GetIndirectLev();
  }
  return false;
}

void OriginalSt::Dump() const {
  if (IsSymbolOst()) {
    LogInfo::MapleLogger() << (symOrPreg.mirSt->IsGlobal() ? "$" : "%") << symOrPreg.mirSt->GetName();
    if (fieldID != 0) {
      LogInfo::MapleLogger() << "{" << fieldID << "}";
    }
    LogInfo::MapleLogger() << "<" << static_cast<int32>(GetIndirectLev()) << ">";
    if (IsFinal()) {
      LogInfo::MapleLogger() << "F";
    }
    if (IsPrivate()) {
      LogInfo::MapleLogger() << "P";
    }
  } else if (IsPregOst()) {
    LogInfo::MapleLogger() << "%" << GetMIRPreg()->GetPregNo();
  }
}

OriginalStTable::OriginalStTable(MemPool &memPool, MIRModule &mod)
    : alloc(&memPool),
      mirModule(mod),
      originalStVector({ nullptr }, alloc.Adapter()),
      mirSt2Ost(alloc.Adapter()),
      preg2Ost(alloc.Adapter()),
      pType2Ost(std::less<TyIdx>(), alloc.Adapter()),
      malloc2Ost(alloc.Adapter()),
      thisField2Ost(std::less<uint32>(), alloc.Adapter()) {}

void OriginalStTable::Dump() {
  mirModule.GetOut() << "==========original st table===========\n";
  for (size_t i = 1; i < Size(); ++i) {
    const OriginalSt *verst = GetOriginalStFromID(OStIdx(i));
    verst->Dump();
  }
  mirModule.GetOut() << "\n=======end original st table===========\n";
}

OriginalSt *OriginalStTable::FindOrCreateSymbolOriginalSt(MIRSymbol &mirst, PUIdx pidx, FieldID fld) {
  auto it = mirSt2Ost.find(SymbolFieldPair(mirst.GetStIndex(), fld));
  if (it == mirSt2Ost.end()) {
    // create a new OriginalSt
    return CreateSymbolOriginalSt(mirst, pidx, fld);
  }
  CHECK_FATAL(it->second < originalStVector.size(),
              "index out of range in OriginalStTable::FindOrCreateSymbolOriginalSt");
  return originalStVector[it->second];
}

OriginalSt *OriginalStTable::FindOrCreatePregOriginalSt(PregIdx regidx, PUIdx pidx) {
  auto it = preg2Ost.find(regidx);
  return (it == preg2Ost.end()) ? CreatePregOriginalSt(regidx, pidx)
                                : originalStVector.at(it->second);
}

OriginalSt *OriginalStTable::CreateSymbolOriginalSt(MIRSymbol &mirst, PUIdx pidx, FieldID fld) {
  auto *ost = alloc.GetMemPool()->New<OriginalSt>(originalStVector.size(), mirst, pidx, fld, alloc);
  if (fld == 0) {
    ost->SetTyIdx(mirst.GetTyIdx());
    ost->SetIsFinal(mirst.IsFinal());
    ost->SetIsPrivate(mirst.IsPrivate());
  } else {
    auto *structType = static_cast<MIRStructType*>(GlobalTables::GetTypeTable().GetTypeFromTyIdx(mirst.GetTyIdx()));
    ASSERT(structType, "CreateSymbolOriginalSt: non-zero fieldID for non-structure");
    ost->SetTyIdx(structType->GetFieldTyIdx(fld));
    FieldAttrs fattrs = structType->GetFieldAttrs(fld);
    ost->SetIsFinal(fattrs.GetAttr(FLDATTR_final) && !mirModule.CurFunction()->IsConstructor());
    ost->SetIsPrivate(fattrs.GetAttr(FLDATTR_private));
  }
  originalStVector.push_back(ost);
  mirSt2Ost[SymbolFieldPair(mirst.GetStIndex(), fld)] = ost->GetIndex();
  return ost;
}

OriginalSt *OriginalStTable::CreatePregOriginalSt(PregIdx regidx, PUIdx pidx) {
  auto *ost = alloc.GetMemPool()->New<OriginalSt>(originalStVector.size(), regidx, pidx, alloc);
  if (regidx < 0) {
    ost->SetTyIdx(TyIdx(PTY_unknown));
  } else {
    ost->SetTyIdx(GlobalTables::GetTypeTable().GetPrimType(ost->GetMIRPreg()->GetPrimType())->GetTypeIndex());
  }
  originalStVector.push_back(ost);
  preg2Ost[regidx] = ost->GetIndex();
  return ost;
}

OriginalSt *OriginalStTable::FindSymbolOriginalSt(MIRSymbol &mirst) {
  auto it = mirSt2Ost.find(SymbolFieldPair(mirst.GetStIndex(), 0));
  if (it == mirSt2Ost.end()) {
    return nullptr;
  }
  CHECK_FATAL(it->second < originalStVector.size(), "index out of range in OriginalStTable::FindSymbolOriginalSt");
  return originalStVector[it->second];
}
}  // namespace maple
