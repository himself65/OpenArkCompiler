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
#ifndef MPLFE_INCLUDE_FEIR_TYPE_H
#define MPLFE_INCLUDE_FEIR_TYPE_H
#include <string>
#include <memory>
#include "prim_types.h"
#include "types_def.h"
#include "mir_type.h"
#include "global_tables.h"

namespace maple {
enum FEIRTypeKind {
  kFEIRTypeDefault,
  kFEIRTypeByName,
  kFEIRTypePointer,
};

class FEIRType {
 public:
  const static uint8 kDimMax = 255;
  explicit FEIRType(FEIRTypeKind argKind);
  FEIRType(FEIRTypeKind argKind, PrimType argPrimType);
  virtual ~FEIRType() = default;
  static std::unique_ptr<FEIRType> NewType(FEIRTypeKind argKind = kFEIRTypeDefault);
  MIRType *GenerateMIRTypeAuto(MIRSrcLang srcLang) const;
  bool IsSameKind(const FEIRType &type) const {
    return kind == type.kind;
  }

  FEIRTypeKind GetKind() const {
    return kind;
  }

  PrimType GetPrimType() const {
    return primType;
  }

  PrimType GetRealPrimType() const {
    return IsScalar() ? primType : PTY_ref;
  }

  void SetPrimType(PrimType argPrimType) {
    primType = argPrimType;
  }

  bool IsZero() const {
    return isZero;
  }

  void SetZero(bool arg) {
    isZero = arg;
  }

  void CopyFrom(const FEIRType &type) {
    return CopyFromImpl(type);
  }

  std::unique_ptr<FEIRType> Clone() const {
    return CloneImpl();
  }

  MIRType *GenerateMIRType(MIRSrcLang srcLang, bool usePtr) const {
    return GenerateMIRTypeImpl(srcLang, usePtr);
  }

  MIRType *GenerateMIRType(bool usePtr) const {
    return GenerateMIRTypeImpl(usePtr);
  }

  MIRType *GenerateMIRType() const {
    return GenerateMIRTypeImpl();
  }

  bool IsPreciseRefType() const {
    return IsPreciseRefTypeImpl();
  }

  bool IsPreciseType() const {
    return IsPreciseTypeImpl();
  }

  bool IsScalar() const {
    return IsScalarImpl();
  }

  uint8 ArrayIncrDim(uint8 delta = 1) {
    return ArrayIncrDimImpl(delta);
  }

  uint8 ArrayDecrDim(uint8 delta = 1) {
    return ArrayDecrDimImpl(delta);
  }

  bool IsEqualTo(const std::unique_ptr<FEIRType> &argType) const {
    return IsEqualToImpl(argType);
  }

  bool IsEqualTo(const FEIRType &argType) const {
    return IsEqualToImpl(argType);
  }

  size_t Hash() const {
    return HashImpl();
  }

 protected:
  virtual void CopyFromImpl(const FEIRType &type);
  virtual std::unique_ptr<FEIRType> CloneImpl() const = 0;
  virtual MIRType *GenerateMIRTypeImpl(MIRSrcLang srcLang, bool usePtr) const = 0;
  virtual MIRType *GenerateMIRTypeImpl(bool usePtr) const = 0;
  virtual MIRType *GenerateMIRTypeImpl() const = 0;
  virtual bool IsPreciseRefTypeImpl() const = 0;
  virtual bool IsPreciseTypeImpl() const = 0;
  virtual bool IsScalarImpl() const = 0;
  virtual uint8 ArrayIncrDimImpl(uint8 delta) = 0;
  virtual uint8 ArrayDecrDimImpl(uint8 delta) = 0;
  virtual bool IsEqualToImpl(const std::unique_ptr<FEIRType> &argType) const;
  virtual bool IsEqualToImpl(const FEIRType &argType) const;
  virtual size_t HashImpl() const = 0;

  FEIRTypeKind kind : 7;
  bool isZero : 1;
  PrimType primType;
};  // class FEIRType

using UniqueFEIRType = std::unique_ptr<FEIRType>;

class FEIRTypeDefault : public FEIRType {
 public:
  FEIRTypeDefault();
  explicit FEIRTypeDefault(PrimType argPrimType);
  FEIRTypeDefault(PrimType argPrimType, const GStrIdx &argTypeNameIdx);
  FEIRTypeDefault(PrimType argPrimType, const GStrIdx &argTypeNameIdx, uint8 argDim);
  ~FEIRTypeDefault() = default;
  FEIRTypeDefault(const FEIRTypeDefault&) = delete;
  FEIRTypeDefault &operator=(const FEIRTypeDefault&) = delete;
  void LoadFromJavaTypeName(const std::string &typeName, bool inMpl = true);
  MIRType *GenerateMIRTypeForPrim() const;
  FEIRTypeDefault &SetTypeNameIdx(const GStrIdx &argTypeNameIdx) {
    typeNameIdx = argTypeNameIdx;
    return *this;
  }

  FEIRTypeDefault &SetTypeName(const std::string &typeName) {
    GStrIdx idx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(typeName);
    return SetTypeNameIdx(idx);
  }

  FEIRTypeDefault &SetTypeNameWithoutCreate(const std::string &typeName) {
    GStrIdx idx = GlobalTables::GetStrTable().GetStrIdxFromName(typeName);
    CHECK_FATAL(idx.GetIdx() != 0, "typeNameIdx should be pre-created");
    return SetTypeNameIdx(idx);
  }

  GStrIdx GetTypeNameIdx() const {
    return typeNameIdx;
  }

  uint8 GetDim() const {
    return dim;
  }

  FEIRTypeDefault &SetDim(uint8 argDim) {
    dim = argDim;
    return *this;
  }

 protected:
  void CopyFromImpl(const FEIRType &type) override;
  std::unique_ptr<FEIRType> CloneImpl() const override;
  MIRType *GenerateMIRTypeImpl(MIRSrcLang srcLang, bool usePtr) const override;
  MIRType *GenerateMIRTypeImpl(bool usePtr) const override;
  MIRType *GenerateMIRTypeImpl() const override;
  uint8 ArrayIncrDimImpl(uint8 delta) override;
  uint8 ArrayDecrDimImpl(uint8 delta) override;
  bool IsEqualToImpl(const FEIRType &argType) const override;
  size_t HashImpl() const override;
  bool IsPreciseRefTypeImpl() const override;
  bool IsPreciseTypeImpl() const override;
  bool IsScalarImpl() const override;
  MIRType *GenerateMIRTypeInternal(const GStrIdx &argTypeNameIdx, bool usePtr) const;

  GStrIdx typeNameIdx;
  uint8 dim;
};

// ---------- FEIRTypeByName ----------
class FEIRTypeByName : public FEIRTypeDefault {
 public:
  FEIRTypeByName(PrimType argPrimType, const std::string &argTypeName, uint8 argDim = 0);
  ~FEIRTypeByName() = default;
  FEIRTypeByName(const FEIRTypeByName&) = delete;
  FEIRTypeByName &operator=(const FEIRTypeByName&) = delete;

 protected:
  std::unique_ptr<FEIRType> CloneImpl() const override;
  MIRType *GenerateMIRTypeImpl(MIRSrcLang srcLang, bool usePtr) const override;
  MIRType *GenerateMIRTypeImpl(bool usePtr) const override;
  MIRType *GenerateMIRTypeImpl() const override;
  bool IsEqualToImpl(const FEIRType &argType) const override;
  size_t HashImpl() const override;
  bool IsPreciseRefTypeImpl() const override;
  bool IsPreciseTypeImpl() const override;
  bool IsScalarImpl() const override;

 private:
  std::string typeName;
};

// ---------- FEIRTypePointer ----------
class FEIRTypePointer : public FEIRType {
 public:
  explicit FEIRTypePointer(std::unique_ptr<FEIRType> argBaseType, PrimType argPrimType = PTY_ptr);
  ~FEIRTypePointer() = default;
  FEIRTypePointer(const FEIRTypePointer&) = delete;
  FEIRTypePointer &operator=(const FEIRTypePointer&) = delete;
  const UniqueFEIRType &GetBaseType() const {
    return baseType;
  }

  void SetBaseType(UniqueFEIRType argBaseType) {
    CHECK_NULL_FATAL(argBaseType);
    baseType = std::move(argBaseType);
  }

 protected:
  std::unique_ptr<FEIRType> CloneImpl() const override;
  MIRType *GenerateMIRTypeImpl(MIRSrcLang srcLang, bool usePtr) const override;
  MIRType *GenerateMIRTypeImpl(bool usePtr) const override;
  MIRType *GenerateMIRTypeImpl() const override;
  bool IsEqualToImpl(const FEIRType &argType) const override;
  size_t HashImpl() const override;
  bool IsPreciseRefTypeImpl() const override;
  bool IsPreciseTypeImpl() const override;
  bool IsScalarImpl() const override;
  uint8 ArrayIncrDimImpl(uint8 delta) override;
  uint8 ArrayDecrDimImpl(uint8 delta) override;

 private:
  std::unique_ptr<FEIRType> baseType;
};

// ---------- FEIRTypeKey ----------
class FEIRTypeKey {
 public:
  explicit FEIRTypeKey(const UniqueFEIRType &argType) {
    ASSERT(argType != nullptr, "nullptr check");
    type = argType->Clone();
  }

  explicit FEIRTypeKey(const FEIRTypeKey &key) {
    ASSERT(key.type != nullptr, "nullptr check");
    type = key.type->Clone();
  }

  ~FEIRTypeKey() = default;
  FEIRTypeKey &operator=(const FEIRTypeKey &key) {
    if (&key != this) {
      CHECK_NULL_FATAL(key.type);
      SetType(key.type->Clone());
    }
    return *this;
  }

  bool operator==(const FEIRTypeKey &key) const {
    return type->IsEqualTo(key.type);
  }

  size_t Hash() const {
    return type->Hash();
  }

  const UniqueFEIRType &GetType() const {
    return type;
  }

  void SetType(UniqueFEIRType argType) {
    CHECK_NULL_FATAL(argType);
    type = std::move(argType);
  }

 private:
  UniqueFEIRType type;
};

struct FEIRTypeKeyHash {
  size_t operator()(const FEIRTypeKey &key) const {
    return key.Hash();
  }
};
}  // namespace maple
#endif  // MPLFE_INCLUDE_FEIR_TYPE_H