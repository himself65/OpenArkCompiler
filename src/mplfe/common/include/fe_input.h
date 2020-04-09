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
#ifndef MPLFE_INCLUDE_FE_INPUT_H
#define MPLFE_INCLUDE_FE_INPUT_H
#include <vector>
#include "global_tables.h"
#include "mir_module.h"
#include "mpl_logging.h"

namespace maple {
class FEInputUnit {
 public:
  FEInputUnit() {}
  virtual ~FEInputUnit() = default;
  GStrIdx GetNameIdxOrin() {
    return GetNameIdxOrinImpl();
  }

  GStrIdx GetNameIdxMpl() {
    return GetNameIdxMplImpl();
  }

  std::string GetCatagoryName() {
    return GetCatagoryNameImpl();
  }

 protected:
  virtual GStrIdx GetNameIdxOrinImpl() = 0;
  virtual GStrIdx GetNameIdxMplImpl() = 0;
  virtual std::string GetCatagoryNameImpl() = 0;
};

enum FEInputSameNamePolicy {
  kUseFirst,
  kUseNewest,
  kFatalOnce,
  kFatalAll
};

template <typename T>
class FEInputContent {
 public:
  FEInputContent(MapleAllocator &alloc);
  ~FEInputContent() = default;
  void RegisterItem(T *item);
  void CheckSameName();

 private:
  MapleAllocator &allocator;
  MapleList<T*> items;
  MapleUnorderedMap<GStrIdx, T*, GStrIdxHash> nameItemMap;
  MapleList<GStrIdx> sameNames;
  FEInputSameNamePolicy policySameName;

  void EraseItem(GStrIdx nameIdxMpl);
};

class FEInputUnitExt {
};

class FEInputProgramUnit {
};

class FEInputUnitMethod : public FEInputUnit {
 public:
  FEInputUnitMethod(MapleAllocator &alloc);
  ~FEInputUnitMethod() = default;

 protected:
  std::string GetCatagoryNameImpl() override;

  MapleAllocator &allocator;
};

class FEInputUnitVariable : public FEInputUnit {
 public:
  FEInputUnitVariable(MapleAllocator &alloc);
  ~FEInputUnitVariable() = default;

 protected:
  std::string GetCatagoryNameImpl() override;

  MapleAllocator &allocator;
};

class FEInputUnitStruct : public FEInputUnit {
 public:
  explicit FEInputUnitStruct(MapleAllocator &alloc);
  virtual ~FEInputUnitStruct() = default;
  MIRTypeKind GetMIRTypeKind() const {
    return typeKind;
  }

 protected:
  std::string GetCatagoryNameImpl() override;

  MapleAllocator &allocator;
  FEInputContent<FEInputUnitMethod> methods;
  FEInputContent<FEInputUnitMethod> methodsStatic;
  FEInputContent<FEInputUnitVariable> fields;
  FEInputContent<FEInputUnitVariable> fieldsStatic;
  MIRTypeKind typeKind;
};
}  // namespace maple
#endif
