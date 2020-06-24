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
#ifndef MAPLE_IR_INCLUDE_MIR_TYPE_H
#define MAPLE_IR_INCLUDE_MIR_TYPE_H
#include <algorithm>
#include <array>
#include "prim_types.h"
#include "mir_pragma.h"
#include "mpl_logging.h"
#include "safe_cast.h"
#if MIR_FEATURE_FULL
#include "mempool.h"
#include "mempool_allocator.h"
#endif  // MIR_FEATURE_FULL

namespace maple {
constexpr int kTypeHashLength = 12289;  // hash length for mirtype, ref: planetmath.org/goodhashtableprimes

class FieldAttrs;  // circular dependency exists, no other choice
using TyIdxFieldAttrPair = std::pair<TyIdx, FieldAttrs>;
using FieldPair = std::pair<GStrIdx, TyIdxFieldAttrPair>;
using FieldVector = std::vector<FieldPair>;
using MIRTypePtr = MIRType*;

constexpr size_t kMaxArrayDim = 10;
const std::string kJstrTypeName = "constStr";
#if MIR_FEATURE_FULL
extern bool VerifyPrimType(PrimType primType1, PrimType primType2);       // verify if primType1 and primType2 match
extern uint32 GetPrimTypeSize(PrimType primType);                         // answer in bytes; 0 if unknown
extern uint32 GetPrimTypeP2Size(PrimType primType);                       // answer in bytes in power-of-two.
extern const char *GetPrimTypeName(PrimType primType);
extern const char *GetPrimTypeJavaName(PrimType primType);
inline uint32 GetPrimTypeBitSize(PrimType primType) {
  // 1 byte = 8 bits = 2^3 bits
  return GetPrimTypeSize(primType) << 3;
}

#endif  // MIR_FEATURE_FULL
// return the same type with size increased to register size
PrimType GetRegPrimType(PrimType primType);
PrimType GetDynType(PrimType primType);
PrimType GetNonDynType(PrimType primType);

inline bool IsAddress(PrimitiveType primitiveType) {
  return primitiveType.IsAddress();
}

inline bool IsPrimitivePureScalar(PrimitiveType primitiveType) {
  return primitiveType.IsInteger() && !primitiveType.IsAddress() && !primitiveType.IsDynamic();
}

inline bool IsUnsignedInteger(PrimitiveType primitiveType) {
  return primitiveType.IsInteger() && primitiveType.IsUnsigned() && !primitiveType.IsDynamic();
}

inline bool IsSignedInteger(PrimitiveType primitiveType) {
  return primitiveType.IsInteger() && !primitiveType.IsUnsigned() && !primitiveType.IsDynamic();
}

inline bool IsPrimitiveInteger(PrimitiveType primitiveType) {
  return primitiveType.IsInteger() && !primitiveType.IsDynamic();
}

inline bool IsPrimitiveDynType(PrimitiveType primitiveType) {
  return primitiveType.IsDynamic();
}

inline bool IsPrimitiveDynInteger(PrimitiveType primitiveType) {
  return primitiveType.IsDynamic() && primitiveType.IsInteger();
}

inline bool IsPrimitiveDynFloat(PrimitiveType primitiveType) {
  return primitiveType.IsDynamic() && primitiveType.IsFloat();
}

inline bool IsPrimitiveFloat(PrimitiveType primitiveType) {
  return primitiveType.IsFloat() && !primitiveType.IsDynamic();
}

inline bool IsPrimitiveScalar(PrimitiveType primitiveType) {
  return primitiveType.IsInteger() || primitiveType.IsFloat() ||
         (primitiveType.IsDynamic() && !primitiveType.IsDynamicNone()) ||
         primitiveType.IsSimple();
}

inline bool IsPrimitiveValid(PrimitiveType primitiveType) {
  return IsPrimitiveScalar(primitiveType) && !primitiveType.IsDynamicAny();
}

inline bool IsPrimitivePoint(PrimitiveType primitiveType) {
  return primitiveType.IsPointer();
}

bool IsNoCvtNeeded(PrimType toType, PrimType fromType);

inline bool IsRefOrPtrAssign(PrimType toType, PrimType fromType) {
  return (toType == PTY_ref && fromType == PTY_ptr) || (toType == PTY_ptr && fromType == PTY_ref);
}

enum MIRTypeKind : std::uint8_t {
  kTypeInvalid,
  kTypeUnknown,
  kTypeScalar,
  kTypeBitField,
  kTypeArray,
  kTypeFArray,
  kTypeJArray,
  kTypeStruct,
  kTypeUnion,
  kTypeClass,
  kTypeInterface,
  kTypeStructIncomplete,
  kTypeClassIncomplete,
  kTypeConstString,
  kTypeInterfaceIncomplete,
  kTypePointer,
  kTypeFunction,
  kTypeVoid,
  kTypeByName,          // type definition not yet seen
  kTypeParam,           // to support java generics
  kTypeInstantVector,   // represent a vector of instantiation pairs
  kTypeGenericInstant,  // type to be formed by instantiation of a generic type
};

enum AttrKind : unsigned {
#define TYPE_ATTR
#define ATTR(STR) ATTR_##STR,
#include "all_attributes.def"
#undef ATTR
#undef TYPE_ATTR
};

class TypeAttrs {
 public:
  TypeAttrs() = default;
  TypeAttrs(const TypeAttrs &ta) = default;
  TypeAttrs &operator=(const TypeAttrs &t) = default;
  ~TypeAttrs() = default;

  void SetAlignValue(uint8 align) {
    attrAlign = align;
  }

  uint8 GetAlignValue() const {
    return attrAlign;
  }

  void SetAttrFlag(uint64 flag) {
    attrFlag = flag;
  }

  uint64 GetAttrFlag() const {
    return attrFlag;
  }

  void SetAttr(AttrKind x) {
    attrFlag |= (1ULL << static_cast<unsigned int>(x));
  }

  void ResetAttr(AttrKind x) {
    attrFlag &= ~(1ULL << static_cast<unsigned int>(x));
  }

  bool GetAttr(AttrKind x) const {
    return (attrFlag & (1ULL << static_cast<unsigned int>(x))) != 0;
  }

  void SetAlign(uint32 x) {
    ASSERT((~(x - 1) & x) == x, "SetAlign called with non-power-of-2");
    attrAlign = 0;
    while (x != 1) {
      x >>= 1;
      ++attrAlign;
    }
  }

  uint32 GetAlign() const {
    if (attrAlign == 0) {
      return 1;
    }
    uint32 res = 1;
    uint32 exp = attrAlign;
    do {
      --exp;
      res *= 2;
    } while (exp != 0);
    return res;
  }

  bool operator==(const TypeAttrs &tA) const {
    return attrFlag == tA.attrFlag && attrAlign == tA.attrAlign;
  }

  bool operator!=(const TypeAttrs &tA) const {
    return !(*this == tA);
  }

  void DumpAttributes() const;

 private:
  uint64 attrFlag = 0;
  uint8 attrAlign = 0;  // alignment in bytes is 2 to the power of attrAlign
};

enum FieldAttrKind {
#define FIELD_ATTR
#define ATTR(STR) FLDATTR_##STR,
#include "all_attributes.def"
#undef ATTR
#undef FIELD_ATTR
};

class FieldAttrs {
 public:
  FieldAttrs() = default;
  FieldAttrs(const FieldAttrs &ta) = default;
  FieldAttrs &operator=(const FieldAttrs &p) = default;
  ~FieldAttrs() = default;

  void SetAlignValue(uint8 align) {
    attrAlign = align;
  }

  uint8 GetAlignValue() const {
    return attrAlign;
  }

  void SetAttrFlag(uint32 flag) {
    attrFlag = flag;
  }

  uint32 GetAttrFlag() const {
    return attrFlag;
  }

  void SetAttr(FieldAttrKind x) {
    attrFlag |= (1u << static_cast<unsigned int>(x));
  }

  bool GetAttr(FieldAttrKind x) const {
    return (attrFlag & (1u << static_cast<unsigned int>(x))) != 0;
  }

  void SetAlign(uint32 x) {
    ASSERT((~(x - 1) & x) == x, "SetAlign called with non-power-of-2");
    attrAlign = 0;
    while (x != 1) {
      x >>= 1;
      ++attrAlign;
    }
  }

  uint32 GetAlign() const {
    if (attrAlign == 0) {
      return 1;
    }
    uint32 res = 1;
    uint32 exp = attrAlign;
    do {
      --exp;
      res *= 2;
    } while (exp != 0);
    return res;
  }

  bool operator==(const FieldAttrs &tA) const {
    return attrFlag == tA.attrFlag && attrAlign == tA.attrAlign;
  }

  bool operator!=(const FieldAttrs &tA) const {
    return !(*this == tA);
  }

  void DumpAttributes() const;
  TypeAttrs ConvertToTypeAttrs();

 private:
  uint32 attrFlag = 0;
  uint8 attrAlign = 0;  // alignment in bytes is 2 to the power of attrAlign
};

enum FuncAttrKind : unsigned {
#define FUNC_ATTR
#define ATTR(STR) FUNCATTR_##STR,
#include "all_attributes.def"
#undef ATTR
#undef FUNC_ATTR
};

class FuncAttrs {
 public:
  FuncAttrs() = default;
  FuncAttrs(const FuncAttrs &ta) = default;
  FuncAttrs &operator=(const FuncAttrs &p) = default;
  ~FuncAttrs() = default;

  void SetAttr(FuncAttrKind x, bool unSet = false) {
    if (!unSet) {
      attrFlag |= (1ULL << x);
    } else {
      attrFlag &= ~(1ULL << x);
    }
  }

  void SetAttrFlag(uint64 flag) {
    attrFlag = flag;
  }

  uint64 GetAttrFlag() const {
    return attrFlag;
  }

  bool GetAttr(FuncAttrKind x) const {
    return (attrFlag & (1ULL << x)) != 0;
  }

  bool operator==(const FuncAttrs &tA) const {
    return attrFlag == tA.attrFlag;
  }

  bool operator!=(const FuncAttrs &tA) const {
    return !(*this == tA);
  }

  void DumpAttributes() const;

 private:
  uint64 attrFlag = 0;
};

// only for internal use, not emitted
enum GenericAttrKind {
#define FUNC_ATTR
#define TYPE_ATTR
#define FIELD_ATTR
#define ATTR(STR) GENATTR_##STR,
#include "all_attributes.def"
#undef ATTR
#undef FUNC_ATTR
#undef TYPE_ATTR
#undef FIELD_ATTR
};

class GenericAttrs {
 public:
  GenericAttrs() = default;
  GenericAttrs(const GenericAttrs &ta) = default;
  GenericAttrs &operator=(const GenericAttrs &p) = default;
  ~GenericAttrs() = default;

  void SetAttr(GenericAttrKind x) {
    attrFlag |= (1ULL << x);
  }

  bool GetAttr(GenericAttrKind x) const {
    return (attrFlag & (1ULL << x)) != 0;
  }

  bool operator==(const GenericAttrs &tA) const {
    return attrFlag == tA.attrFlag;
  }

  bool operator!=(const GenericAttrs &tA) const {
    return !(*this == tA);
  }

  FieldAttrs ConvertToFieldAttrs();
  TypeAttrs ConvertToTypeAttrs();
  FuncAttrs ConvertToFuncAttrs();

 private:
  uint64 attrFlag = 0;
};

#if MIR_FEATURE_FULL
constexpr int kShiftNumOfTypeKind = 8;
constexpr int kShiftNumOfNameStrIdx = 6;
class MIRType {
 public:
  MIRType(MIRTypeKind kind, PrimType pType) : typeKind(kind), primType(pType) {}

  MIRType(MIRTypeKind kind, PrimType pType, GStrIdx strIdx)
      : typeKind(kind), primType(pType), nameStrIdx(strIdx) {}

  virtual ~MIRType() = default;

  virtual void Dump(int indent, bool dontUseName = false) const;
  virtual void DumpAsCxx(int indent) const;
  virtual bool EqualTo(const MIRType &mirType) const;
  virtual bool IsStructType() {
    return false;
  }

  virtual MIRType *CopyMIRTypeNode() const {
    return new MIRType(*this);
  }

  PrimType GetPrimType() const {
    return primType;
  }
  void SetPrimType(const PrimType pt) {
    primType = pt;
  }

  TyIdx GetTypeIndex() const {
    return tyIdx;
  }
  void SetTypeIndex(TyIdx idx) {
    tyIdx = idx;
  }

  MIRTypeKind GetKind() const {
    return typeKind;
  }
  void SetMIRTypeKind(MIRTypeKind kind) {
    typeKind = kind;
  }

  bool IsNameIsLocal() const {
    return nameIsLocal;
  }
  void SetNameIsLocal(bool flag) {
    nameIsLocal = flag;
  }

  GStrIdx GetNameStrIdx() const {
    return nameStrIdx;
  }
  void SetNameStrIdx(GStrIdx strIdx) {
    nameStrIdx = strIdx;
  }
  void SetNameStrIdxItem(uint32 idx) {
    nameStrIdx.reset(idx);
  }

  virtual size_t GetSize() const {
    return GetPrimTypeSize(primType);
  }

  virtual bool HasVolatileField() const {
    return false;
  }

  virtual bool HasTypeParam() const {
    return false;
  }

  virtual bool IsIncomplete() const {
    return typeKind == kTypeStructIncomplete || typeKind == kTypeClassIncomplete ||
           typeKind == kTypeInterfaceIncomplete;
  }

  bool IsVolatile(int fieldID) const;

  bool IsMIRPtrType() const {
    return typeKind == kTypePointer;
  }

  bool IsMIRStructType() const {
    return (typeKind == kTypeStruct) || (typeKind == kTypeStructIncomplete);
  }

  bool IsMIRClassType() const {
    return (typeKind == kTypeClass) || (typeKind == kTypeClassIncomplete);
  }

  bool IsMIRInterfaceType() const {
    return (typeKind == kTypeInterface) || (typeKind == kTypeInterfaceIncomplete);
  }

  bool IsInstanceOfMIRStructType() const {
    return IsMIRStructType() || IsMIRClassType() || IsMIRInterfaceType();
  }

  bool IsMIRJarrayType() const {
    return typeKind == kTypeJArray;
  }

  bool IsMIRFuncType() const {
    return typeKind == kTypeFunction;
  }

  bool IsScalarType() const {
    return typeKind == kTypeScalar;
  }

  bool ValidateClassOrInterface(const std::string &className, bool noWarning) const;
  bool IsOfSameType(MIRType &type);
  const std::string &GetName() const;
  virtual std::string GetMplTypeName() const;
  virtual std::string GetCompactMplTypeName() const;
  virtual bool PointsToConstString() const;
  virtual size_t GetHashIndex() const {
    constexpr uint8 idxShift = 2;
    return ((static_cast<uint32>(primType) << idxShift) + (typeKind << kShiftNumOfTypeKind)) % kTypeHashLength;
  }

 protected:
  MIRTypeKind typeKind;
  PrimType primType;
  bool nameIsLocal = false;  // needed when printing the type name
  TyIdx tyIdx{ 0 };
  GStrIdx nameStrIdx{ 0 };  // name in global string table
};

class MIRPtrType : public MIRType {
 public:
  explicit MIRPtrType(TyIdx pTyIdx) : MIRType(kTypePointer, PTY_ptr), pointedTyIdx(pTyIdx) {}

  MIRPtrType(TyIdx pTyIdx, PrimType pty) : MIRType(kTypePointer, pty), pointedTyIdx(pTyIdx) {}

  MIRPtrType(PrimType primType, GStrIdx strIdx) : MIRType(kTypePointer, primType, strIdx), pointedTyIdx(0) {}

  ~MIRPtrType() override = default;

  MIRType *CopyMIRTypeNode() const override {
    return new MIRPtrType(*this);
  }

  MIRType *GetPointedType() const;

  TyIdx GetPointedTyIdx() const {
    return pointedTyIdx;
  }
  void SetPointedTyIdx(TyIdx idx) {
    pointedTyIdx = idx;
  }

  TypeAttrs &GetTypeAttrs() {
    return typeAttrs;
  }

  const TypeAttrs &GetTypeAttrs() const {
    return typeAttrs;
  }

  void SetTypeAttrs(TypeAttrs attrs) {
    typeAttrs = attrs;
  }

  bool EqualTo(const MIRType &type) const override;

  bool HasTypeParam() const override;
  bool IsPointedTypeVolatile(int fieldID) const;

  void Dump(int indent, bool dontUseName = false) const override;
  TyIdxFieldAttrPair GetPointedTyIdxFldAttrPairWithFieldID(FieldID fieldID) const;
  TyIdx GetPointedTyIdxWithFieldID(FieldID fieldID) const;
  size_t GetHashIndex() const override {
    constexpr uint8 idxShift = 4;
    constexpr uint8 attrShift = 3;
    size_t hIdx = (static_cast<size_t>(pointedTyIdx) << idxShift) + (typeKind << kShiftNumOfTypeKind);
    hIdx += (typeAttrs.GetAttrFlag() << attrShift) + typeAttrs.GetAlignValue();
    return hIdx % kTypeHashLength;
  }

  bool PointsToConstString() const override;

  std::string GetMplTypeName() const override;

  std::string GetCompactMplTypeName() const override;
 private:
  TyIdx pointedTyIdx;
  TypeAttrs typeAttrs;
};

class MIRArrayType : public MIRType {
 public:
  MIRArrayType() : MIRType(kTypeArray, PTY_agg) {}
  explicit MIRArrayType(GStrIdx strIdx) : MIRType(kTypeArray, PTY_agg, strIdx) {}

  MIRArrayType(TyIdx eTyIdx, const std::vector<uint32> &sizeArray)
      : MIRType(kTypeArray, PTY_agg),
        eTyIdx(eTyIdx),
        dim(sizeArray.size()) {
    for (size_t i = 0; i < kMaxArrayDim; ++i) {
      this->sizeArray[i] = (i < dim) ? sizeArray[i] : 0;
    }
  }

  MIRArrayType(const MIRArrayType &pat) = default;
  MIRArrayType &operator=(const MIRArrayType &p) = default;
  ~MIRArrayType() override = default;

  TyIdx GetElemTyIdx() const {
    return eTyIdx;
  }
  void SetElemTyIdx(TyIdx idx) {
    eTyIdx = idx;
  }

  uint32 GetSizeArrayItem(uint32 n) const {
    CHECK_FATAL((n >= 0 && n < kMaxArrayDim), "out of bound of array!");
    return sizeArray[n];
  }
  void SetSizeArrayItem(uint32 idx, uint32 value) {
    CHECK_FATAL((idx >= 0 && idx < kMaxArrayDim), "out of bound of array!");
    sizeArray[idx] = value;
  }

  bool EqualTo(const MIRType &type) const override;

  uint16 GetDim() const {
    return dim;
  }
  void SetDim(uint16 dim) {
    this->dim = dim;
  }

  MIRType *GetElemType() const;

  MIRType *CopyMIRTypeNode() const override {
    return new MIRArrayType(*this);
  }

  bool HasTypeParam() const override {
    return GetElemType()->HasTypeParam();
  }

  void Dump(int indent, bool dontUseName) const override;

  size_t GetSize() const override {
    size_t elemSize = GetElemType()->GetSize();
    if (elemSize == 0) {
      return 0;
    }
    size_t numElems = sizeArray[0];
    for (size_t i = 1; i < dim; ++i) {
      CHECK_FATAL(i < kMaxArrayDim, "array index out of range");
      numElems *= sizeArray[i];
    }
    return elemSize * numElems;
  }

  size_t GetHashIndex() const override {
    constexpr uint8 idxShift = 2;
    size_t hIdx = (static_cast<size_t>(eTyIdx) << idxShift) + (typeKind << kShiftNumOfTypeKind);
    for (size_t i = 0; i < dim; ++i) {
      CHECK_FATAL(i < kMaxArrayDim, "array index out of range");
      hIdx += (sizeArray[i] << i);
    }
    return hIdx % kTypeHashLength;
  }

  std::string GetMplTypeName() const override;
  std::string GetCompactMplTypeName() const override;
 private:
  TyIdx eTyIdx{ 0 };
  uint16 dim = 0;
  std::array<uint32, kMaxArrayDim> sizeArray{ {0} };
};

// flexible array type, must be last field of a top-level struct
class MIRFarrayType : public MIRType {
 public:
  MIRFarrayType() : MIRType(kTypeFArray, PTY_agg), elemTyIdx(TyIdx(0)) {};

  explicit MIRFarrayType(TyIdx elemTyIdx) : MIRType(kTypeFArray, PTY_agg), elemTyIdx(elemTyIdx) {}

  explicit MIRFarrayType(GStrIdx strIdx) : MIRType(kTypeFArray, PTY_agg, strIdx), elemTyIdx(TyIdx(0)) {}

  ~MIRFarrayType() override = default;

  MIRType *CopyMIRTypeNode() const override {
    return new MIRFarrayType(*this);
  };

  MIRType *GetElemType() const;

  bool HasTypeParam() const override {
    return GetElemType()->HasTypeParam();
  }

  TyIdx GetElemTyIdx() const {
    return elemTyIdx;
  }
  void SetElemtTyIdx(TyIdx idx) {
    elemTyIdx = idx;
  }

  bool EqualTo(const MIRType &type) const override;
  void Dump(int indent, bool dontUseName = false) const override;

  size_t GetHashIndex() const override {
    constexpr uint8 idxShift = 5;
    return ((static_cast<size_t>(elemTyIdx) << idxShift) + (typeKind << kShiftNumOfTypeKind)) % kTypeHashLength;
  }

  std::string GetMplTypeName() const override;
  std::string GetCompactMplTypeName() const override;

 private:
  TyIdx elemTyIdx;
};

using TyidxFuncAttrPair = std::pair<TyIdx, FuncAttrs>;
using MethodPair = std::pair<StIdx, TyidxFuncAttrPair>;
using MethodVector = std::vector<MethodPair>;
using MethodPtrVector = std::vector<MethodPair*>;
using MIREncodedArray = std::vector<EncodedValue>;
class GenericDeclare;
class AnnotationType;
class GenericType;
// used by kTypeStruct, kTypeStructIncomplete, kTypeUnion
class MIRStructType : public MIRType {
 public:
  explicit MIRStructType(MIRTypeKind typeKind) : MIRType(typeKind, PTY_agg) {}

  MIRStructType(MIRTypeKind typeKind, GStrIdx strIdx) : MIRType(typeKind, PTY_agg, strIdx) {}

  ~MIRStructType() override = default;

  bool IsStructType() override {
    return true;
  }

  FieldVector &GetFields() {
    return fields;
  }
  const FieldVector &GetFields() const {
    return fields;
  }
  void SetFields(const FieldVector &fields) {
    this->fields = fields;
  }

  const FieldPair &GetFieldsElemt(size_t n) const {
    ASSERT(n < fields.size(), "array index out of range");
    return fields.at(n);
  }

  FieldPair &GetFieldsElemt(size_t n) {
    ASSERT(n < fields.size(), "array index out of range");
    return fields.at(n);
  }

  size_t GetFieldsSize() const {
    return fields.size();
  }

  const std::vector<TyIdx> &GetFieldInferredTyIdx() const {
    return fieldInferredTyIdx;
  }

  FieldVector &GetStaticFields() {
    return staticFields;
  }
  const FieldVector &GetStaticFields() const {
    return staticFields;
  }

  const FieldPair &GetStaticFieldsPair(size_t i) const {
    return staticFields.at(i);
  }

  GStrIdx GetStaticFieldsGStrIdx(size_t i) const {
    return staticFields.at(i).first;
  }

  FieldVector &GetParentFields() {
    return parentFields;
  }
  void SetParentFields(const FieldVector &parentFields) {
    this->parentFields = parentFields;
  }
  const FieldVector &GetParentFields() const {
    return parentFields;
  }
  const FieldPair &GetParentFieldsElemt(size_t n) const {
    ASSERT(n < parentFields.size(), "array index out of range");
    return parentFields.at(n);
  }
  size_t GetParentFieldsSize() const {
    return parentFields.size();
  }

  MethodVector &GetMethods() {
    return methods;
  }
  const MethodVector &GetMethods() const {
    return methods;
  }

  const MethodPair &GetMethodsElement(size_t n) const {
    ASSERT(n < methods.size(), "array index out of range");
    return methods.at(n);
  }

  MethodPtrVector &GetVTableMethods() {
    return vTableMethods;
  }

  const MethodPair *GetVTableMethodsElemt(size_t n) const {
    ASSERT(n < vTableMethods.size(), "array index out of range");
    return vTableMethods.at(n);
  }

  size_t GetVTableMethodsSize() const {
    return vTableMethods.size();
  }

  const MethodPtrVector &GetItableMethods() const {
    return iTableMethods;
  }

  bool IsImported() const {
    return isImported;
  }

  void SetIsImported(bool flag) {
    isImported = flag;
  }

  bool IsUsed() const {
    return isUsed;
  }

  void SetIsUsed(bool flag) {
    isUsed = flag;
  }

  GStrIdx GetFieldGStrIdx(FieldID id) const {
    const FieldPair &fieldPair = TraverseToField(id);
    return fieldPair.first;
  }

  const TyIdxFieldAttrPair GetFieldTyIdxAttrPair(FieldID id) const {
    return TraverseToField(id).second;
  }

  TyIdx GetFieldTyIdx(FieldID id) const {
    const FieldPair &fieldPair = TraverseToField(id);
    return fieldPair.second.first;
  }

  FieldAttrs GetFieldAttrs(FieldID id) const {
    const FieldPair &fieldPair = TraverseToField(id);
    return fieldPair.second.second;
  }

  FieldAttrs GetFieldAttrs(GStrIdx fieldStrIdx) const {
    const FieldPair &fieldPair = TraverseToField(fieldStrIdx);
    return fieldPair.second.second;
  }

  bool IsFieldVolatile(FieldID id) const {
    const FieldPair &fieldPair = TraverseToField(id);
    return fieldPair.second.second.GetAttr(FLDATTR_volatile);
  }

  bool IsFieldFinal(FieldID id) const {
    const FieldPair &fieldPair = TraverseToField(id);
    return fieldPair.second.second.GetAttr(FLDATTR_final);
  }

  bool IsFieldRCUnownedRef(FieldID id) const {
    const FieldPair &fieldPair = TraverseToField(id);
    return fieldPair.second.second.GetAttr(FLDATTR_rcunowned);
  }

  bool IsFieldRCWeak(FieldID id) const {
    const FieldPair &fieldPair = TraverseToField(id);
    return fieldPair.second.second.GetAttr(FLDATTR_rcweak);
  }

  bool IsOwnField(FieldID id) const {
    const FieldPair &fieldPair = TraverseToField(id);
    return std::find(fields.begin(), fields.end(), fieldPair) != fields.end();
  }

  bool HasVolatileField() const override;
  bool HasTypeParam() const override;
  bool EqualTo(const MIRType &type) const override;
  MIRType *CopyMIRTypeNode() const override {
    return new MIRStructType(*this);
  }

  TyIdx GetElemTyIdx(size_t n) const {
    ASSERT(n < fields.size(), "array index out of range");
    return fields.at(n).second.first;
  }

  void SetElemtTyIdxSimple(size_t n, TyIdx tyIdx) {
    ASSERT(n < fields.size(), "array index out of range");
    fields.at(n).second.first = tyIdx;
  }

  TyIdx GetStaticElemtTyIdx(size_t n) const {
    ASSERT(n < staticFields.size(), "array index out of range");
    return staticFields.at(n).second.first;
  }

  void SetStaticElemtTyIdx(size_t n, TyIdx tyIdx) {
    staticFields.at(n).second.first = tyIdx;
  }

  void SetMethodTyIdx(size_t n, TyIdx tyIdx) {
    ASSERT(n < methods.size(), "array index out of range");
    methods.at(n).second.first = tyIdx;
  }

  MIRType *GetElemType(uint32 n) const;

  MIRType *GetFieldType(FieldID fieldID);

  void SetElemtTyIdx(size_t n, TyIdx tyIdx) {
    ASSERT(n < fields.size(), "array index out of range");
    fields.at(n).second = TyIdxFieldAttrPair(tyIdx, FieldAttrs());
  }

  GStrIdx GetElemStrIdx(size_t n) const {
    ASSERT(n < fields.size(), "array index out of range");
    return fields.at(n).first;
  }

  void SetElemStrIdx(size_t n, GStrIdx idx) {
    ASSERT(n < fields.size(), "array index out of range");
    fields.at(n).first = idx;
  }

  void SetElemInferredTyIdx(size_t n, TyIdx tyIdx) {
    if (n >= fieldInferredTyIdx.size()) {
      fieldInferredTyIdx.insert(fieldInferredTyIdx.end(), n + 1 - fieldInferredTyIdx.size(), kInitTyIdx);
    }
    ASSERT(n < fieldInferredTyIdx.size(), "array index out of range");
    fieldInferredTyIdx.at(n) = tyIdx;
  }

  TyIdx GetElemInferredTyIdx(size_t n) {
    if (n >= fieldInferredTyIdx.size()) {
      fieldInferredTyIdx.insert(fieldInferredTyIdx.end(), n + 1 - fieldInferredTyIdx.size(), kInitTyIdx);
    }
    ASSERT(n < fieldInferredTyIdx.size(), "array index out of range");
    return fieldInferredTyIdx.at(n);
  }

  void DumpFieldsAndMethods(int indent, bool hasMethod) const;
  void Dump(int indent, bool dontUseName = false) const override;

  virtual void SetComplete() {
    typeKind = (typeKind == kTypeUnion) ? typeKind : kTypeStruct;
  }

  // only meaningful for MIRClassType and MIRInterface types
  bool IsLocal() const;

  size_t GetSize() const override;

  size_t GetHashIndex() const override {
    return ((static_cast<size_t>(nameStrIdx) << kShiftNumOfNameStrIdx) + (typeKind << kShiftNumOfTypeKind)) %
           kTypeHashLength;
  }

  virtual void ClearContents() {
    fields.clear();
    staticFields.clear();
    parentFields.clear();
    methods.clear();
    vTableMethods.clear();
    iTableMethods.clear();
    isImported = false;
    isUsed = false;
    hasVolatileField = false;
    hasVolatileFieldSet = false;
  }

  virtual const std::vector<MIRInfoPair> &GetInfo() const {
    CHECK_FATAL(false, "can not use GetInfo");
  }

  virtual const MIRInfoPair &GetInfoElemt(size_t) const {
    CHECK_FATAL(false, "can not use GetInfoElemt");
  }

  virtual const std::vector<bool> &GetInfoIsString() const {
    CHECK_FATAL(false, "can not use GetInfoIsString");
  }

  virtual bool GetInfoIsStringElemt(size_t) const {
    CHECK_FATAL(false, "can not use GetInfoIsStringElemt");
  }

  virtual const std::vector<MIRPragma*> &GetPragmaVec() const {
    CHECK_FATAL(false, "can not use GetPragmaVec");
  }

  virtual std::vector<MIRPragma*> &GetPragmaVec() {
    CHECK_FATAL(false, "can not use GetPragmaVec");
  }

  std::vector<GenericDeclare*>& GetGenericDeclare() {
    return genericDeclare;
  }

  void AddClassGenericDeclare(GenericDeclare *gd) {
    genericDeclare.push_back(gd);
  }

  void AddFieldGenericDeclare(GStrIdx g, AnnotationType *a) {
    if (fieldGenericDeclare.find(g) != fieldGenericDeclare.end()) {
      CHECK_FATAL(fieldGenericDeclare[g] == a, "MUST BE");
    }
    fieldGenericDeclare[g] = a;
  }

  AnnotationType *GetFieldGenericDeclare(GStrIdx g) {
    if (fieldGenericDeclare.find(g) == fieldGenericDeclare.end()) {
      return nullptr;
    }
    return fieldGenericDeclare[g];
  }

  void AddInheritaceGeneric(GenericType *a) {
    inheritanceGeneric.push_back(a);
  }

  std::vector<GenericType*> &GetInheritanceGeneric() {
    return inheritanceGeneric;
  }

  virtual const MIREncodedArray &GetStaticValue() const {
    CHECK_FATAL(false, "can not use GetStaticValue");
  }

  virtual void PushbackMIRInfo(const MIRInfoPair&) {
    CHECK_FATAL(false, "can not use PushbackMIRInfo");
  }

  virtual void PushbackPragma(MIRPragma*) {
    CHECK_FATAL(false, "can not use PushbackPragma");
  }

  virtual void PushbackStaticValue(EncodedValue&) {
    CHECK_FATAL(false, "can not use PushbackStaticValue");
  }

  virtual void PushbackIsString(bool) {
    CHECK_FATAL(false, "can not use PushbackIsString");
  }

  virtual FieldPair TraverseToFieldRef(FieldID &fieldID) const;
  std::string GetMplTypeName() const override;
  std::string GetCompactMplTypeName() const override;

 protected:
  FieldVector fields{};
  std::vector<TyIdx> fieldInferredTyIdx{};
  FieldVector staticFields{};
  FieldVector parentFields{};       // fields belong to the ancestors not fully defined
  MethodVector methods{};           // for the list of member function prototypes
  MethodPtrVector vTableMethods{};  // the list of implmentation for all virtual functions for this type
  MethodPtrVector iTableMethods{};  // the list of all interface functions for this type; For classes, they are
  // implementation functions, For interfaces, they are abstact functions.
  // Weak indicates the actual definition is in another module.
  bool isImported = false;
  bool isUsed = false;
  mutable bool hasVolatileField = false;     // for caching computed value
  mutable bool hasVolatileFieldSet = false;  // if true, just read hasVolatileField;
                                             // otherwise compute to initialize hasVolatileField
  std::vector<GenericDeclare*> genericDeclare;
  std::map<GStrIdx, AnnotationType*> fieldGenericDeclare;
  std::vector<GenericType*> inheritanceGeneric;
 private:
  FieldPair TraverseToField(FieldID fieldID) const ;
  FieldPair TraverseToField(GStrIdx fieldStrIdx) const ;
  bool HasVolatileFieldInFields(const FieldVector &fieldsOfStruct) const;
  bool HasTypeParamInFields(const FieldVector &fieldsOfStruct) const;
};

// java array type, must not be nested inside another aggregate
class MIRJarrayType : public MIRFarrayType {
 public:
  MIRJarrayType() {
    typeKind = kTypeJArray;
  };

  explicit MIRJarrayType(TyIdx elemTyIdx) : MIRFarrayType(elemTyIdx) {
    typeKind = kTypeJArray;
  }

  explicit MIRJarrayType(GStrIdx strIdx) : MIRFarrayType(strIdx) {
    typeKind = kTypeJArray;
  }

  ~MIRJarrayType() override = default;

  MIRType *CopyMIRTypeNode() const override {
    return new MIRJarrayType(*this);
  }

  MIRStructType *GetParentType();
  const std::string &GetJavaName();

  bool IsPrimitiveArray() {
    if (javaNameStrIdx == 0u) {
      DetermineName();
    }
    return fromPrimitive;
  }

  int GetDim() {
    if (javaNameStrIdx == 0u) {
      DetermineName();
    }
    return dim;
  }

  size_t GetHashIndex() const override {
    constexpr uint8 idxShift = 5;
    return ((static_cast<size_t>(GetElemTyIdx()) << idxShift) + (typeKind << kShiftNumOfTypeKind)) % kTypeHashLength;
  }

 private:
  void DetermineName();    // determine the internal name of this type
  TyIdx parentTyIdx{ 0 };       // since Jarray is also an object, this is java.lang.Object
  GStrIdx javaNameStrIdx{ 0 };  // for internal java name of Jarray. nameStrIdx is used for other purpose
  bool fromPrimitive = false;      // the lowest dimension is primitive type
  int dim = 0;                 // the dimension if decidable at compile time. otherwise 0
};

// used by kTypeClass, kTypeClassIncomplete
class MIRClassType : public MIRStructType {
 public:
  explicit MIRClassType(MIRTypeKind tKind) : MIRStructType(tKind) {}
  MIRClassType(MIRTypeKind tKind, GStrIdx strIdx) : MIRStructType(tKind, strIdx) {}
  ~MIRClassType() override = default;

  bool EqualTo(const MIRType &type) const override;

  MIRType *CopyMIRTypeNode() const override {
    return new MIRClassType(*this);
  }

  const std::vector<MIRInfoPair> &GetInfo() const override {
    return info;
  }
  void PushbackMIRInfo(const MIRInfoPair &pair) override {
    info.push_back(pair);
  }
  uint32 GetInfo(const std::string &infoStr) const;
  uint32 GetInfo(GStrIdx strIdx) const;
  size_t GetInfoSize() const {
    return info.size();
  }

  const MIRInfoPair &GetInfoElemt(size_t n) const override {
    ASSERT(n < info.size(), "array index out of range");
    return info.at(n);
  }

  const std::vector<bool> &GetInfoIsString() const override {
    return infoIsString;
  }

  void PushbackIsString(bool isString) override {
    infoIsString.push_back(isString);
  }

  size_t GetInfoIsStringSize() const {
    return infoIsString.size();
  }

  bool GetInfoIsStringElemt(size_t n) const override {
    ASSERT(n < infoIsString.size(), "array index out of range");
    return infoIsString.at(n);
  }

  std::vector<MIRPragma*> &GetPragmaVec() override {
    return pragmaVec;
  }
  const std::vector<MIRPragma*> &GetPragmaVec() const override {
    return pragmaVec;
  }
  void PushbackPragma(MIRPragma *pragma) override {
    pragmaVec.push_back(pragma);
  }

  const MIREncodedArray &GetStaticValue() const override {
    return staticValue;
  }
  void PushbackStaticValue(EncodedValue &encodedValue) override {
    staticValue.push_back(encodedValue);
  }

  TyIdx GetParentTyIdx() const {
    return parentTyIdx;
  }
  void SetParentTyIdx(TyIdx idx) {
    parentTyIdx = idx;
  }

  std::vector<TyIdx> &GetInterfaceImplemented() {
    return interfacesImplemented;
  }
  const std::vector<TyIdx> &GetInterfaceImplemented() const {
    return interfacesImplemented;
  }
  TyIdx GetNthInterfaceImplemented(size_t i) const {
    ASSERT(i < interfacesImplemented.size(), "array index out of range");
    return interfacesImplemented.at(i);
  }

  void SetNthInterfaceImplemented(size_t i, TyIdx tyIdx) {
    ASSERT(i < interfacesImplemented.size(), "array index out of range");
    interfacesImplemented.at(i) = tyIdx;
  }
  void PushbackInterfaceImplemented(TyIdx idx) {
    interfacesImplemented.push_back(idx);
  }

  void Dump(int indent, bool dontUseName = false) const override;
  void DumpAsCxx(int indent) const override;
  void SetComplete() override {
    typeKind = kTypeClass;
  }

  bool IsFinal() const;
  bool IsAbstract() const;
  bool IsInner() const;
  bool HasVolatileField() const override;
  bool HasTypeParam() const override;
  FieldPair TraverseToFieldRef(FieldID &fieldID) const override;
  size_t GetSize() const override;

  FieldID GetLastFieldID() const;
  FieldID GetFirstFieldID() const {
    return GetLastFieldID() - fields.size() + 1;
  }

  FieldID GetFirstLocalFieldID() const;
  // return class id or superclass id accroding to input string
  MIRClassType *GetExceptionRootType();
  const MIRClassType *GetExceptionRootType() const;
  bool IsExceptionType() const;
  void AddImplementedInterface(TyIdx interfaceTyIdx) {
    if (std::find(interfacesImplemented.begin(), interfacesImplemented.end(), interfaceTyIdx) ==
        interfacesImplemented.end()) {
      interfacesImplemented.push_back(interfaceTyIdx);
    }
  }

  void ClearContents() override {
    MIRStructType::ClearContents();
    parentTyIdx = TyIdx(0);
    interfacesImplemented.clear();  // for the list of interfaces the class implements
    info.clear();
    infoIsString.clear();
    pragmaVec.clear();
    staticValue.clear();
  }

  size_t GetHashIndex() const override {
    return ((static_cast<size_t>(nameStrIdx) << kShiftNumOfNameStrIdx) + (typeKind << kShiftNumOfTypeKind)) %
           kTypeHashLength;
  }

 private:
  TyIdx parentTyIdx{ 0 };
  std::vector<TyIdx> interfacesImplemented{};  // for the list of interfaces the class implements
  std::vector<MIRInfoPair> info{};
  std::vector<bool> infoIsString{};
  std::vector<MIRPragma*> pragmaVec{};
  MIREncodedArray staticValue{};  // DELETE THIS
};

// used by kTypeInterface, kTypeInterfaceIncomplete
class MIRInterfaceType : public MIRStructType {
 public:
  explicit MIRInterfaceType(MIRTypeKind tKind) : MIRStructType(tKind) {}
  MIRInterfaceType(MIRTypeKind tKind, GStrIdx strIdx) : MIRStructType(tKind, strIdx) {}
  ~MIRInterfaceType() override = default;

  bool EqualTo(const MIRType &type) const override;

  MIRType *CopyMIRTypeNode() const override {
    return new MIRInterfaceType(*this);
  }

  const std::vector<MIRInfoPair> &GetInfo() const override {
    return info;
  }
  void PushbackMIRInfo(const MIRInfoPair &pair) override {
    info.push_back(pair);
  }
  uint32 GetInfo(const std::string &infoStr) const;
  uint32 GetInfo(GStrIdx strIdx) const;
  size_t GetInfoSize() const {
    return info.size();
  }

  const MIRInfoPair &GetInfoElemt(size_t n) const override {
    ASSERT(n < info.size(), "array index out of range");
    return info.at(n);
  }

  const std::vector<bool> &GetInfoIsString() const override {
    return infoIsString;
  }
  void PushbackIsString(bool isString) override {
    infoIsString.push_back(isString);
  }
  size_t GetInfoIsStringSize() const {
    return infoIsString.size();
  }
  bool GetInfoIsStringElemt(size_t n) const override {
    ASSERT(n < infoIsString.size(), "array index out of range");
    return infoIsString.at(n);
  }

  std::vector<MIRPragma*> &GetPragmaVec() override {
    return pragmaVec;
  }
  const std::vector<MIRPragma*> &GetPragmaVec() const override {
    return pragmaVec;
  }
  void PushbackPragma(MIRPragma *pragma) override {
    pragmaVec.push_back(pragma);
  }

  const MIREncodedArray &GetStaticValue() const override {
    return staticValue;
  }
  void PushbackStaticValue(EncodedValue &encodedValue) override {
    staticValue.push_back(encodedValue);
  }

  std::vector<TyIdx> &GetParentsTyIdx() {
    return parentsTyIdx;
  }
  void SetParentsTyIdx(const std::vector<TyIdx> &parents) {
    parentsTyIdx = parents;
  }
  const std::vector<TyIdx> &GetParentsTyIdx() const {
    return parentsTyIdx;
  }

  TyIdx GetParentsElementTyIdx(size_t i) const {
    ASSERT(i < parentsTyIdx.size(), "array index out of range");
    return parentsTyIdx[i];
  }

  void SetParentsElementTyIdx(size_t i, TyIdx tyIdx) {
    ASSERT(i < parentsTyIdx.size(), "array index out of range");
    parentsTyIdx[i] = tyIdx;
  }

  void Dump(int indent, bool dontUseName = false) const override;
  bool HasVolatileField() const override;
  bool HasTypeParam() const override;
  FieldPair TraverseToFieldRef(FieldID &fieldID) const override;
  void SetComplete() override {
    typeKind = kTypeInterface;
  }

  size_t GetSize() const override;

  void ClearContents() override {
    MIRStructType::ClearContents();
    parentsTyIdx.clear();
    info.clear();
    infoIsString.clear();
    pragmaVec.clear();
    staticValue.clear();
  }

  size_t GetHashIndex() const override {
    return ((static_cast<size_t>(nameStrIdx) << kShiftNumOfNameStrIdx) + (typeKind << kShiftNumOfTypeKind)) %
           kTypeHashLength;
  }

 private:
  std::vector<TyIdx> parentsTyIdx{};  // multiple inheritence
  std::vector<MIRInfoPair> info{};
  std::vector<bool> infoIsString{};
  std::vector<MIRPragma*> pragmaVec{};
  MIREncodedArray staticValue{};  // DELETE THIS
};


class MIRBitFieldType : public MIRType {
 public:
  MIRBitFieldType(uint8 field, PrimType pt) : MIRType(kTypeBitField, pt), fieldSize(field) {}
  MIRBitFieldType(uint8 field, PrimType pt, GStrIdx strIdx) : MIRType(kTypeBitField, pt, strIdx), fieldSize(field) {}
  ~MIRBitFieldType() override = default;

  uint8 GetFieldSize() const {
    return fieldSize;
  }

  bool EqualTo(const MIRType &type) const override;
  void Dump(int indent, bool dontUseName = false) const override;
  MIRType *CopyMIRTypeNode() const override {
    return new MIRBitFieldType(*this);
  }

  size_t GetSize() const override {
    return 0;
  }  // size not be in bytes

  size_t GetHashIndex() const override {
    return ((static_cast<uint32>(primType) << fieldSize) + (typeKind << kShiftNumOfTypeKind)) % kTypeHashLength;
  }

 private:
  uint8 fieldSize;
};

class MIRFuncType : public MIRType {
 public:
  MIRFuncType(MapleAllocator &ma)
      : MIRType(kTypeFunction, PTY_ptr),
        paramTypeList(ma.Adapter()),
        paramAttrsList(ma.Adapter()) {}

  explicit MIRFuncType(const GStrIdx &strIdx, MapleAllocator &ma)
      : MIRType(kTypeFunction, PTY_ptr, strIdx),
        paramTypeList(ma.Adapter()),
        paramAttrsList(ma.Adapter()) {}

  MIRFuncType(const TyIdx &retTyIdx, const std::vector<TyIdx> &vecTy,
              const std::vector<TypeAttrs> &vecAt, MapleAllocator &ma)
      : MIRType(kTypeFunction, PTY_ptr),
        retTyIdx(retTyIdx),
        paramTypeList(ma.Adapter()),
        paramAttrsList(ma.Adapter()) {
          for (auto tyIdx : vecTy) {
            paramTypeList.push_back(tyIdx);
          }
          for (auto tyAttr : vecAt) {
            paramAttrsList.push_back(tyAttr);
          }
        }

  ~MIRFuncType() override = default;

  bool EqualTo(const MIRType &type) const override;
  MIRType *CopyMIRTypeNode() const override {
    return new MIRFuncType(*this);
  }

  void Dump(int indent, bool dontUseName = false) const override;
  size_t GetSize() const override {
    return 0;
  }  // size unknown

  TyIdx GetRetTyIdx() const {
    return retTyIdx;
  }

  void SetRetTyIdx(TyIdx idx) {
    retTyIdx = idx;
  }

  const MapleVector<TyIdx> &GetParamTypeList() const {
    return paramTypeList;
  }

  MapleVector<TyIdx> &GetParamTypeList() {
    return paramTypeList;
  }

  TyIdx GetNthParamType(size_t i) const {
    ASSERT(i < paramTypeList.size(), "array index out of range");
    return paramTypeList[i];
  }

  void SetParamTypeList(const std::vector<TyIdx> &list) {
    paramTypeList.clear();
    for (auto tyIdx : list) {
      paramTypeList.push_back(tyIdx);
    }
  }

  const MapleVector<TypeAttrs> &GetParamAttrsList() const {
    return paramAttrsList;
  }

  MapleVector<TypeAttrs> &GetParamAttrsList() {
    return paramAttrsList;
  }

  const TypeAttrs &GetNthParamAttrs(size_t i) const {
    ASSERT(i < paramAttrsList.size(), "array index out of range");
    return paramAttrsList[i];
  }

  TypeAttrs &GetNthParamAttrs(size_t i) {
    ASSERT(i < paramAttrsList.size(), "array index out of range");
    return paramAttrsList[i];
  }

  void SetParamAttrsList(const std::vector<TypeAttrs> &list) {
    paramAttrsList.clear();
    for (auto tyAttr : list) {
      paramAttrsList.push_back(tyAttr);
    }
  }

  void SetNthParamAttrs(size_t i, const TypeAttrs &attrs) {
    ASSERT(i < paramAttrsList.size(), "array index out of range");
    paramAttrsList[i] = attrs;
  }

  bool IsVarargs() const {
    return isVarArgs;
  }

  void SetVarArgs(bool flag) {
    isVarArgs = flag;
  }

  size_t GetHashIndex() const override {
    constexpr uint8 idxShift = 6;
    size_t hIdx = (static_cast<size_t>(retTyIdx) << idxShift) + (typeKind << kShiftNumOfTypeKind);
    size_t size = paramTypeList.size();
    hIdx += (size ? (static_cast<size_t>(paramTypeList[0]) + size) : 0) << 4; // shift bit is 4
    return hIdx % kTypeHashLength;
  }

 private:
  TyIdx retTyIdx{ 0 };
  MapleVector<TyIdx> paramTypeList;
  MapleVector<TypeAttrs> paramAttrsList;
  bool isVarArgs = false;
};

class MIRTypeByName : public MIRType {
  // use nameStrIdx to store the name for both local and global
 public:
  explicit MIRTypeByName(GStrIdx gStrIdx) : MIRType(kTypeByName, PTY_void) {
    nameStrIdx = gStrIdx;
  }

  ~MIRTypeByName() override = default;

  MIRType *CopyMIRTypeNode() const override {
    return new MIRTypeByName(*this);
  }

  bool EqualTo(const MIRType &type) const override;

  void Dump(int indent, bool dontUseName = false) const override;
  size_t GetSize() const override {
    return 0;
  }  // size unknown

  size_t GetHashIndex() const override {
    constexpr uint8 idxShift = 2;
    return ((static_cast<size_t>(nameStrIdx) << idxShift) + nameIsLocal + (typeKind << kShiftNumOfTypeKind)) %
           kTypeHashLength;
  }
};

class MIRTypeParam : public MIRType {
  // use nameStrIdx to store the name
 public:
  explicit MIRTypeParam(GStrIdx gStrIdx) : MIRType(kTypeParam, PTY_gen) {
    nameStrIdx = gStrIdx;
  }

  ~MIRTypeParam() override = default;

  MIRType *CopyMIRTypeNode() const override {
    return new MIRTypeParam(*this);
  }

  bool EqualTo(const MIRType &type) const override;
  void Dump(int indent, bool dontUseName = false) const override;
  size_t GetSize() const override {
    return 0;
  }  // size unknown

  bool HasTypeParam() const override {
    return true;
  }

  size_t GetHashIndex() const override {
    constexpr uint8 idxShift = 3;
    return ((static_cast<size_t>(nameStrIdx) << idxShift) + (typeKind << kShiftNumOfTypeKind)) % kTypeHashLength;
  }
};

using TypePair = std::pair<TyIdx, TyIdx>;
using GenericInstantVector = std::vector<TypePair>;
class MIRInstantVectorType : public MIRType {
 public:
  MIRInstantVectorType() : MIRType(kTypeInstantVector, PTY_agg) {}

  explicit MIRInstantVectorType(MIRTypeKind kind) : MIRType(kind, PTY_agg) {}

  MIRInstantVectorType(MIRTypeKind kind, GStrIdx strIdx) : MIRType(kind, PTY_agg, strIdx) {}

  ~MIRInstantVectorType() override = default;

  MIRType *CopyMIRTypeNode() const override {
    return new MIRInstantVectorType(*this);
  }

  bool EqualTo(const MIRType &type) const override;
  void Dump(int indent, bool dontUseName = false) const override;
  size_t GetSize() const override {
    return 0;
  }  // size unknown

  const GenericInstantVector &GetInstantVec() const {
    return instantVec;
  }

  void AddInstant(TypePair typePair) {
    instantVec.push_back(typePair);
  }

  size_t GetHashIndex() const override {
    constexpr uint8 idxShift = 3;
    uint32 hIdx = typeKind << kShiftNumOfTypeKind;
    for (const TypePair &typePair : instantVec) {
      hIdx += static_cast<uint32>(typePair.first + typePair.second) << idxShift;
    }
    return hIdx % kTypeHashLength;
  }

 protected:
  GenericInstantVector instantVec{};  // in each pair, first is generic type, second is real type
};

class MIRGenericInstantType : public MIRInstantVectorType {
 public:
  explicit MIRGenericInstantType(TyIdx genTyIdx)
      : MIRInstantVectorType(kTypeGenericInstant), genericTyIdx(genTyIdx) {}

  explicit MIRGenericInstantType(GStrIdx strIdx)
      : MIRInstantVectorType(kTypeGenericInstant, strIdx), genericTyIdx(0) {}

  ~MIRGenericInstantType() override = default;

  MIRType *CopyMIRTypeNode() const override {
    return new MIRGenericInstantType(*this);
  }

  bool EqualTo(const MIRType &type) const override;
  void Dump(int indent, bool dontUseName = false) const override;

  size_t GetSize() const override {
    return 0;
  }  // size unknown

  TyIdx GetGenericTyIdx() const {
    return genericTyIdx;
  }
  void SetGenericTyIdx(TyIdx idx) {
    genericTyIdx = idx;
  }

  size_t GetHashIndex() const override {
    constexpr uint8 idxShift = 2;
    uint32 hIdx = (static_cast<uint32>(genericTyIdx) << idxShift) + (typeKind << kShiftNumOfTypeKind);
    for (const TypePair &typePair : instantVec) {
      hIdx += static_cast<uint32>(typePair.first + typePair.second) << 3; // shift bit is 3
    }
    return hIdx % kTypeHashLength;
  }

 private:
  TyIdx genericTyIdx;  // the generic type to be instantiated
};
#endif  // MIR_FEATURE_FULL
}  // namespace maple

#define LOAD_SAFE_CAST_FOR_MIR_TYPE
#include "ir_safe_cast_traits.def"

#endif  // MAPLE_IR_INCLUDE_MIR_TYPE_H
