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
#include <memory>
#include <string>
#include <tuple>
#include "prim_types.h"
#include "types_def.h"
#include "mir_type.h"
#include "global_tables.h"
#include "fe_configs.h"

namespace maple {
enum FEIRTypeKind {
  kFEIRTypeDefault,
  kFEIRTypeByName,
  kFEIRTypePointer,
};

class FEIRType {
 public:
  explicit FEIRType(FEIRTypeKind argKind);
  virtual ~FEIRType() = default;
  static std::unique_ptr<FEIRType> NewType(FEIRTypeKind argKind = kFEIRTypeDefault);
  static std::map<MIRSrcLang, std::tuple<bool, PrimType>> InitLangConfig();
  MIRType *GenerateMIRTypeAuto(MIRSrcLang argSrcLang) const;
  MIRType *GenerateMIRTypeAuto() const {
    return GenerateMIRTypeAuto(srcLang);
  }

  bool IsSameKind(const FEIRType &type) const {
    return kind == type.kind;
  }

  FEIRTypeKind GetKind() const {
    return kind;
  }

  PrimType GetPrimType() const {
    return GetPrimTypeImpl();
  }

  void SetPrimType(PrimType pt) {
    SetPrimTypeImpl(pt);
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

  MIRType *GenerateMIRType(MIRSrcLang argSrcLang, bool usePtr) const {
    return GenerateMIRType(usePtr);
  }

  MIRType *GenerateMIRType() const {
    return GenerateMIRType(false);
  }

  bool IsPreciseRefType() const {
    return IsPrecise() && IsRef();
  }

  bool IsScalar() const {
    return IsScalarImpl();
  }

  bool IsRef() const {
    return IsRefImpl();
  }

  bool IsArray() const {
    return IsArrayImpl();
  }

  bool IsPrecise() const {
    return IsPreciseImpl();
  }

  bool IsValid() const {
    return IsValidImpl();
  }

  MIRType *GenerateMIRType(bool usePtr, PrimType ptyPtr = PTY_ref) const {
    return GenerateMIRTypeImpl(usePtr, ptyPtr);
  }

  TypeDim ArrayIncrDim(TypeDim delta = 1) {
    return ArrayIncrDimImpl(delta);
  }

  TypeDim ArrayDecrDim(TypeDim delta = 1) {
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

  std::string GetTypeName() const {
    return GetTypeNameImpl();
  }

  static std::map<MIRSrcLang, std::tuple<bool, PrimType>> langConfig;

 protected:
  virtual void CopyFromImpl(const FEIRType &type);
  virtual std::unique_ptr<FEIRType> CloneImpl() const = 0;
  virtual MIRType *GenerateMIRTypeImpl(bool usePtr, PrimType ptyPtr) const = 0;
  virtual bool IsScalarImpl() const = 0;
  virtual TypeDim ArrayIncrDimImpl(TypeDim delta) = 0;
  virtual TypeDim ArrayDecrDimImpl(TypeDim delta) = 0;
  virtual bool IsEqualToImpl(const std::unique_ptr<FEIRType> &argType) const;
  virtual bool IsEqualToImpl(const FEIRType &argType) const;
  virtual size_t HashImpl() const = 0;
  virtual PrimType GetPrimTypeImpl() const = 0;
  virtual void SetPrimTypeImpl(PrimType pt) = 0;
  virtual bool IsRefImpl() const = 0;
  virtual bool IsArrayImpl() const = 0;
  virtual bool IsPreciseImpl() const = 0;
  virtual bool IsValidImpl() const = 0;
  virtual std::string GetTypeNameImpl() const {
    return "";
  }

  FEIRTypeKind kind : 7;
  bool isZero : 1;
  MIRSrcLang srcLang : 8;
};  // class FEIRType

using UniqueFEIRType = std::unique_ptr<FEIRType>;

class FEIRTypeDefault : public FEIRType {
 public:
  FEIRTypeDefault();
  explicit FEIRTypeDefault(PrimType argPrimType);
  FEIRTypeDefault(PrimType argPrimType, const GStrIdx &argTypeNameIdx);
  FEIRTypeDefault(PrimType argPrimType, const GStrIdx &argTypeNameIdx, TypeDim argDim);
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

  TypeDim GetDim() const {
    return dim;
  }

  FEIRTypeDefault &SetDim(TypeDim argDim) {
    dim = argDim;
    return *this;
  }

 LLT_PROTECTED:
  void CopyFromImpl(const FEIRType &type) override;
  std::unique_ptr<FEIRType> CloneImpl() const override;
  MIRType *GenerateMIRTypeImpl(bool usePtr, PrimType ptyPtr) const override;
  TypeDim ArrayIncrDimImpl(TypeDim delta) override;
  TypeDim ArrayDecrDimImpl(TypeDim delta) override;
  bool IsEqualToImpl(const FEIRType &argType) const override;
  bool IsEqualToImpl(const std::unique_ptr<FEIRType> &argType) const override;
  size_t HashImpl() const override;
  bool IsScalarImpl() const override;
  PrimType GetPrimTypeImpl() const override;
  void SetPrimTypeImpl(PrimType pt) override;
  MIRType *GenerateMIRTypeInternal(const GStrIdx &argTypeNameIdx, bool usePtr) const;
  MIRType *GenerateMIRTypeInternal(const GStrIdx &argTypeNameIdx, bool usePtr, PrimType ptyPtr) const;
  std::string GetTypeNameImpl() const override;

  bool IsRefImpl() const override {
    return dim > 0 || !IsScalarPrimType(primType);
  }

  bool IsArrayImpl() const override {
    return dim > 0;
  }

  bool IsPreciseImpl() const override {
    return IsScalarPrimType(primType) || typeNameIdx != 0;
  }

  bool IsValidImpl() const override {
    return !IsScalarPrimType(primType) || typeNameIdx == 0;
  }

  static bool IsScalarPrimType(PrimType pty) {
    return pty != PTY_ref && (IsPrimitiveInteger(pty) || IsPrimitiveFloat(pty) || IsAddress(pty) || pty == PTY_void);
  }

  PrimType primType;
  GStrIdx typeNameIdx;
  TypeDim dim;
};

// ---------- FEIRTypeByName ----------
class FEIRTypeByName : public FEIRTypeDefault {
 public:
  FEIRTypeByName(PrimType argPrimType, const std::string &argTypeName, TypeDim argDim = 0);
  ~FEIRTypeByName() = default;
  FEIRTypeByName(const FEIRTypeByName&) = delete;
  FEIRTypeByName &operator=(const FEIRTypeByName&) = delete;

 protected:
  std::unique_ptr<FEIRType> CloneImpl() const override;
  MIRType *GenerateMIRTypeImpl(bool usePtr, PrimType ptyPtr) const override;
  bool IsEqualToImpl(const FEIRType &argType) const override;
  size_t HashImpl() const override;
  bool IsScalarImpl() const override;
  bool IsPreciseImpl() const override {
    return IsScalarPrimType(primType) || !typeName.empty();
  }

  bool IsValidImpl() const override {
    return !IsScalarPrimType(primType) || typeName.empty();
  }

 private:
  std::string typeName;
};

// ---------- FEIRTypePointer ----------
class FEIRTypePointer : public FEIRType {
 public:
  explicit FEIRTypePointer(std::unique_ptr<FEIRType> argBaseType, PrimType argPrimType = PTY_ref);
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
  MIRType *GenerateMIRTypeImpl(bool usePtr, PrimType ptyPtr) const override;
  bool IsEqualToImpl(const FEIRType &argType) const override;
  size_t HashImpl() const override;
  bool IsScalarImpl() const override;
  TypeDim ArrayIncrDimImpl(TypeDim delta) override;
  TypeDim ArrayDecrDimImpl(TypeDim delta) override;
  virtual PrimType GetPrimTypeImpl() const override;
  virtual void SetPrimTypeImpl(PrimType pt) override;
  virtual bool IsRefImpl() const override {
    return baseType->IsRef();
  }

  virtual bool IsArrayImpl() const override {
    return baseType->IsArray();
  }

  virtual bool IsPreciseImpl() const override {
    return baseType->IsPrecise();
  }

  virtual bool IsValidImpl() const override {
    return baseType->IsValid();
  }

 private:
  PrimType primType;
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