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
#include "alias_analysis_table.h"

namespace maple {
OriginalSt *AliasAnalysisTable::GetPrevLevelNode(const OriginalSt &ost) {
  auto it = prevLevelNode.find(ost.GetIndex());
  if (it != prevLevelNode.end()) {
    return it->second;
  }
  return nullptr;
}

MapleVector<OriginalSt*> *AliasAnalysisTable::GetNextLevelNodes(const OriginalSt &ost) {
  auto it = nextLevelNodes.find(ost.GetIndex());
  if (it != nextLevelNodes.end()) {
    return it->second;
  }

  MapleVector<OriginalSt*> *newOriginalStVec =
      alloc.GetMemPool()->New<MapleVector<OriginalSt*>>(alloc.Adapter());
  nextLevelNodes.insert(std::make_pair(ost.GetIndex(), newOriginalStVec));
  return newOriginalStVec;
}

OriginalSt *AliasAnalysisTable::FindOrCreateAddrofSymbolOriginalSt(OriginalSt &ost) {
  auto it = prevLevelNode.find(ost.GetIndex());
  if (it != prevLevelNode.end()) {
    return it->second;
  }
  // create a new node
  OriginalStTable &originalStTab = ssaTab.GetOriginalStTable();
  OriginalSt *prevLevelOst = memPool->New<OriginalSt>(originalStTab.Size(), *ost.GetMIRSymbol(),
                                                      ost.GetPuIdx(), 0, originalStTab.GetAlloc());
  originalStTab.GetOriginalStVector().push_back(prevLevelOst);
  prevLevelOst->SetIndirectLev(-1);
  MIRPtrType pointType(ost.GetTyIdx(), PTY_ptr);
  TyIdx newTyIdx = GlobalTables::GetTypeTable().GetOrCreateMIRType(&pointType);
  prevLevelOst->SetTyIdx(newTyIdx);
  prevLevelOst->SetFieldID(ost.GetFieldID());
  GetNextLevelNodes(*prevLevelOst)->push_back(&ost);
  prevLevelNode.insert(std::make_pair(ost.GetIndex(), prevLevelOst));
  return prevLevelOst;
}

OriginalSt *AliasAnalysisTable::FindOrCreateExtraLevSymOrRegOriginalSt(OriginalSt &ost, TyIdx tyIdx, FieldID fld) {
  TyIdx ptyIdxOfOst = ost.GetTyIdx();
  FieldID fldIDInOst = fld;
  if (ptyIdxOfOst != tyIdx) {
    (void)klassHierarchy.UpdateFieldID(tyIdx, ptyIdxOfOst, fldIDInOst);
  }
  MapleVector<OriginalSt*> *nextLevelOsts = GetNextLevelNodes(ost);
  OriginalSt *nextLevOst = FindExtraLevOriginalSt(*nextLevelOsts, fldIDInOst);
  if (nextLevOst != nullptr) {
    return nextLevOst;
  }

  // create a new node
  OriginalStTable &originalStTab = ssaTab.GetOriginalStTable();
  if (ost.IsSymbolOst()) {
    nextLevOst = memPool->New<OriginalSt>(originalStTab.Size(), *ost.GetMIRSymbol(),
                                          ost.GetPuIdx(), fldIDInOst, originalStTab.GetAlloc());
  } else {
    nextLevOst = memPool->New<OriginalSt>(originalStTab.Size(), ost.GetPregIdx(),
                                          ost.GetPuIdx(), originalStTab.GetAlloc());
  }
  originalStTab.GetOriginalStVector().push_back(nextLevOst);
  CHECK_FATAL(ost.GetIndirectLev() < INT8_MAX, "boundary check");
  nextLevOst->SetIndirectLev(ost.GetIndirectLev() + 1);
  prevLevelNode.insert(std::make_pair(nextLevOst->GetIndex(), &ost));
  tyIdx = (tyIdx == 0u) ? ost.GetTyIdx() : tyIdx;
  if (tyIdx != 0u) {
    // use the tyIdx info from the instruction
    const MIRType *mirType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(tyIdx);
    if (mirType->GetKind() == kTypePointer) {
      const auto *ptType = static_cast<const MIRPtrType*>(mirType);
      TyIdxFieldAttrPair fieldPair = ptType->GetPointedTyIdxFldAttrPairWithFieldID(fld);
      nextLevOst->SetTyIdx(fieldPair.first);
      nextLevOst->SetIsFinal(fieldPair.second.GetAttr(FLDATTR_final));
      nextLevOst->SetIsPrivate(fieldPair.second.GetAttr(FLDATTR_private));
    } else {
      nextLevOst->SetTyIdx(TyIdx(PTY_void));
    }
  }
  ASSERT(!GlobalTables::GetTypeTable().GetTypeTable().empty(), "container check");
  if (GlobalTables::GetTypeTable().GetTypeFromTyIdx(ost.GetTyIdx())->PointsToConstString()) {
    nextLevOst->SetIsFinal(true);
  }
  GetNextLevelNodes(ost)->push_back(nextLevOst);
  ASSERT(originalStTab.GetOriginalStFromID(ost.GetIndex()) == &ost, "OriginalStTable:: index inconsistent");
  return nextLevOst;
}

OriginalSt *AliasAnalysisTable::FindOrCreateExtraLevOriginalSt(OriginalSt &ost, TyIdx ptyIdx, FieldID fld) {
  if (ost.IsSymbolOst() || ost.IsPregOst()) {
    return FindOrCreateExtraLevSymOrRegOriginalSt(ost, ptyIdx, fld);
  }
  return nullptr;
}

OriginalSt *AliasAnalysisTable::FindExtraLevOriginalSt(const MapleVector<OriginalSt*> &nextLevelOsts, FieldID fld) {
  for (OriginalSt *nextLevelOst : nextLevelOsts) {
    if (nextLevelOst->GetFieldID() == fld) {
      return nextLevelOst;
    }
  }
  return nullptr;
}
}  // namespace maple
