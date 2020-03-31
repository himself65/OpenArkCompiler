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
#include "fe_input.h"
#include "mempool_allocator.h"

namespace maple {
// ---------- FEInputContent ----------
template <typename T>
FEInputContent<T>::FEInputContent(MapleAllocator &alloc)
    : allocator(alloc),
      items(allocator.Adapter()),
      nameItemMap(allocator.Adapter()),
      sameNames(allocator.Adapter()),
      policySameName(FEInputSameNamePolicy::kFatalOnce) {}

template <typename T>
void FEInputContent<T>::RegisterItem(T *item) {
  FEInputUnit *itemUnit = dynamic_cast<FEInputUnit*>(item);
  ASSERT(itemUnit != nullptr, "Invalid item type");
  ASSERT(itemUnit->GetNameIdxMpl().GetIdx() != 0, "Invalid name idx (not set)");
  GStrIdx nameIdxMpl = itemUnit->GetNameIdxMpl();
  if (nameItemMap.find(nameIdxMpl) != nameItemMap.end()) {
    // name is defined
    std::string catagoryName = itemUnit->GetCatagoryName();
    std::string name = GlobalTables::GetStrTable().GetStringFromStrIdx(nameIdxMpl);
    switch (policySameName) {
      case FEInputSameNamePolicy::kFatalOnce:
        FATAL(kLncFatal, "%s with the same name %s existed. exit.", catagoryName.c_str(), name.c_str());
        break;
      case FEInputSameNamePolicy::kFatalAll:
        WARN(kLncWarn, "%s with the same name %s existed.", catagoryName.c_str(), name.c_str());
        sameNames.push_back(nameIdxMpl);
        break;
      case FEInputSameNamePolicy::kUseFirst:
        WARN(kLncWarn, "%s with the same name %s existed. Use first.", catagoryName.c_str(), name.c_str());
        break;
      case FEInputSameNamePolicy::kUseNewest:
        WARN(kLncWarn, "%s with the same name %s existed. Use newest.", catagoryName.c_str(), name.c_str());
        EraseItem(nameIdxMpl);
        items.push_back(item);
        nameItemMap[nameIdxMpl] = item;
        break;
    }
  } else {
    // name is undefined
    items.push_back(item);
    nameItemMap[nameIdxMpl] = item;
  }
}

template <typename T>
void FEInputContent<T>::CheckSameName() {
  if (policySameName != FEInputSameNamePolicy::kFatalAll) {
    return;
  }
  if (sameNames.size() > 0) {
    T *item = items.front();
    FEInputUnit *itemUnit = dynamic_cast<FEInputUnit*>(item);
    ASSERT(itemUnit != nullptr, "Invalid item type");
    FATAL(kLncFatal, "%s with the same name existed. Exit", itemUnit->GetCatagoryName().c_str());
  }
}

template <typename T>
void FEInputContent<T>::EraseItem(GStrIdx nameIdxMpl) {
  typename MapleList<T*>::iterator it;
  for (it = items.begin(); it != items.end();) {
    FEInputUnit *itemUnit = dynamic_cast<FEInputUnit*>(*it);
    ASSERT(itemUnit != nullptr, "Invalid item type");
    if (itemUnit->GetNameIdxMpl() == nameIdxMpl) {
      it = items.erase(it);
    } else {
      ++it;
    }
  }
}

// ---------- FEInputUnitMethod ----------
FEInputUnitMethod::FEInputUnitMethod(MapleAllocator &alloc)
    : allocator(alloc) {}

std::string FEInputUnitMethod::GetCatagoryNameImpl() {
  return "Method";
}

// ---------- FEInputUnitVariable ----------
FEInputUnitVariable::FEInputUnitVariable(MapleAllocator &alloc)
    : allocator(alloc) {}

std::string FEInputUnitVariable::GetCatagoryNameImpl() {
  return "Variable/Field";
}

// ---------- FEInputUnitStruct ----------
FEInputUnitStruct::FEInputUnitStruct(MapleAllocator &alloc)
    : allocator(alloc),
      methods(allocator),
      methodsStatic(allocator),
      fields(allocator),
      fieldsStatic(allocator),
      typeKind(kTypeInvalid) {}

std::string FEInputUnitStruct::GetCatagoryNameImpl() {
  return "Variable/Field";
}
}  // namespace maple