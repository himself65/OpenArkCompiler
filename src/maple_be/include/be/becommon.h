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
#ifndef MAPLEBE_INCLUDE_BE_BECOMMON_H
#define MAPLEBE_INCLUDE_BE_BECOMMON_H
/* C++ headers. */
#include <cstddef>
#include <utility>
/* Basic Maple-independent utility functions */
#include "common_utils.h"
/* MapleIR headers. */
#include "mir_nodes.h"   /* maple_ir/include, for BaseNode */
#include "mir_type.h"    /* maple_ir/include, for MIRType */
#include "mir_module.h"  /* maple_ir/include, for mirModule */

namespace maplebe {
using namespace maple;

#if TARGX86_64 || TARGAARCH64
#define LOWERED_PTR_TYPE PTY_a64
constexpr uint8 kSizeOfPtr = 8;
#elif TARGX86 || TARGARM32 || TARGVM
#define LOWERED_PTR_TYPE PTY_a32
constexpr uint8 kSizeOfPtr = 4;
#else
#error "Unsupported target"
#endif

enum BitsPerByte : uint8 {
  kBitsPerByte = 8,
  kLog2BitsPerByte = 3
};

class JClassFieldInfo {  /* common java class field info */
 public:
  /* constructors */
  JClassFieldInfo() : isRef(false), isUnowned(false), isWeak(false), offset(0) {}

  JClassFieldInfo(bool isRef, bool isUnowned, bool isWeak, uint32 offset)
      : isRef(isRef), isUnowned(isUnowned), isWeak(isWeak), offset(offset) {}

  ~JClassFieldInfo() = default;

  bool IsRef() const {
    return isRef;
  }

  bool IsUnowned() const {
    return isUnowned;
  }

  bool IsWeak() const {
    return isWeak;
  }

  uint32 GetOffset() const {
    return offset;
  }

 private:
  bool isRef;            /* used to generate object-map */
  bool isUnowned;        /* used to mark unowned fields for RC */
  bool isWeak;           /* used to mark weak fields for RC */
  uint32 offset;         /* offset from the start of the java object */
};

using JClassLayout = MapleVector<JClassFieldInfo>;  /* java class layout info */

class BECommon {
 public:
  explicit BECommon(MIRModule &mod);

  ~BECommon() = default;

  void LowerTypeAttribute(MIRType &ty);

  void LowerJavaTypeAttribute(MIRType &ty);

  void LowerJavaVolatileInClassType(MIRClassType &ty);

  void LowerJavaVolatileForSymbol(MIRSymbol &sym);

  void ComputeTypeSizesAligns(MIRType &type, uint8 align = 0);

  void GenFieldOffsetMap(const std::string &className);

  void GenFieldOffsetMap(MIRClassType &classType, FILE &outFile);

  void GenObjSize(MIRClassType &classType, FILE &outFile);

  std::pair<int32, int32> GetFieldOffset(MIRStructType &structType, FieldID fieldID);

  bool IsRefField(MIRStructType &structType, FieldID fieldID);

  /* some class may has incomplete type definition. provide an interface to check them. */
  bool HasJClassLayout(MIRClassType &klass) const {
    return (jClassLayoutTable.find(&klass) != jClassLayoutTable.end());
  }

  const JClassLayout &GetJClassLayout(MIRClassType &klass) const {
    return *(jClassLayoutTable.at(&klass));
  }

  void AddElementToJClassLayout(MIRClassType &klass, JClassFieldInfo info);

  MIRType *BeGetOrCreatePointerType(const MIRType &pointedType);

  MIRType *BeGetOrCreateFunctionType(TyIdx tyIdx, const std::vector<TyIdx> &vecTy,
                                     const std::vector<TypeAttrs> &vecAt);

  BaseNode *GetAddressOfNode(const BaseNode &node);

  PrimType GetAddressPrimType() const {
    return LOWERED_PTR_TYPE;
  }

  /* update typeSizeTable and tableAlignTable when new type is created */
  void UpdateTypeTable(MIRType &ty) {
    if (!TyIsInSizeAlignTable(ty)) {
      AddAndComputeSizeAlign(ty);
    }
  }

  uint32 GetFieldIdxIncrement(const MIRType &ty) const {
    if (ty.GetKind() == kTypeClass) {
    /* number of fields + 2 */
      return static_cast<const MIRClassType&>(ty).GetFieldsSize() + 2;
    } else if (ty.GetKind() == kTypeStruct) {
    /* number of fields + 1 */
      return static_cast<const MIRStructType&>(ty).GetFieldsSize() + 1;
    }
    return 1;
  }

  const MIRModule &GetMIRModule() const {
    return mirModule;
  }

  uint64 GetTypeSize(uint32 idx) const {
    return typeSizeTable.at(idx);
  }
  uint32 GetSizeOfTypeSizeTable() const {
    return typeSizeTable.size();
  }
  bool IsEmptyOfTypeSizeTable() const {
    return typeSizeTable.empty();
  }
  void SetTypeSize(uint32 idx, uint64 value) {
    typeSizeTable.at(idx) = value;
  }
  void AddTypeSize(uint64 value) {
    typeSizeTable.emplace_back(value);
  }

  void AddTypeSizeAndAlign(TyIdx tyIdx, uint64 value) {
    if (typeSizeTable.size() == tyIdx) {
      typeSizeTable.emplace_back(value);
      tableAlignTable.emplace_back(value);
    } else {
      CHECK_FATAL(typeSizeTable.size() > tyIdx, "there are some types haven't set type size and align, %d");
    }
  }

  uint8 GetTypeAlign(uint32 idx) const {
    return tableAlignTable.at(idx);
  }
  uint32 GetSizeOfTypeAlignTable() const {
    return tableAlignTable.size();
  }
  bool IsEmptyOfTypeAlignTable() const {
    return tableAlignTable.empty();
  }
  void SetTypeAlign(uint32 idx, uint8 value) {
    tableAlignTable.at(idx) = value;
  }
  void AddTypeAlign(uint8 value) {
    tableAlignTable.emplace_back(value);
  }

  FieldID GetStructFieldCount(uint32 idx) const {
    return structFieldCountTable.at(idx);
  }
  uint32 GetSizeOfStructFieldCountTable() const {
    return structFieldCountTable.size();
  }
  void SetStructFieldCount(uint32 idx, FieldID value) {
    structFieldCountTable.at(idx) = value;
  }
  void AppendStructFieldCount(uint32 idx, FieldID value) {
    structFieldCountTable.at(idx) += value;
  }

 private:
  bool TyIsInSizeAlignTable(const MIRType&) const;
  void AddAndComputeSizeAlign(MIRType&);
  void ComputeStructTypeSizesAligns(MIRType &ty, const TyIdx &tyIdx);
  void ComputeClassTypeSizesAligns(MIRType &ty, const TyIdx &tyIdx, uint8 align = 0);
  void ComputeArrayTypeSizesAligns(MIRType &ty, const TyIdx &tyIdx);
  void ComputeFArrayOrJArrayTypeSizesAligns(MIRType &ty, const TyIdx &tyIdx);

  MIRModule &mirModule;
  MapleVector<uint64> typeSizeTable;           /* index is TyIdx */
  MapleVector<uint8> tableAlignTable;          /* index is TyIdx */
  /*
   * gives number of fields inside
   * each struct inclusive of nested structs, for speeding up
   * traversal for locating the field for a given fieldID
   */
  MapleVector<FieldID> structFieldCountTable;
  /*
   * a lookup table for class layout. the vector is indexed by field-id
   * Note: currently only for java class types.
   */
  MapleUnorderedMap<MIRClassType*, JClassLayout*> jClassLayoutTable;
}; /* class BECommon */
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_BE_BECOMMON_H */
