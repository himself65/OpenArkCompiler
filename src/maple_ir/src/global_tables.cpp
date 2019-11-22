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
#include "global_tables.h"
#include <cmath>
#include <cstring>
#include "mir_type.h"
#include "mir_symbol.h"

#if MIR_FEATURE_FULL
namespace maple {
MIRType *TypeTable::CreateMirType(uint32 primTypeIdx) const {
  MIRTypeKind defaultKind = (primTypeIdx == PTY_constStr ? kTypeConstString : kTypeScalar);
  auto primType = static_cast<PrimType>(primTypeIdx);
  auto *mirType = new MIRType(defaultKind, primType);
  return mirType;
}

TypeTable::TypeTable() {
  // enter the primitve types in type_table_
  typeTable.push_back(static_cast<MIRType*>(nullptr));
  ASSERT(typeTable.size() == static_cast<size_t>(PTY_void), "use PTY_void as the first index to type table");
  for (auto primTypeIdx = static_cast<uint32>(PTY_void); primTypeIdx <= static_cast<uint32>(PTY_agg); ++primTypeIdx) {
    MIRType *type = CreateMirType(primTypeIdx);
    type->SetTypeIndex(TyIdx{ primTypeIdx });
    typeTable.push_back(type);
    PutToHashTable(type);
  }
  if (voidPtrType == nullptr) {
    voidPtrType = GetOrCreatePointerType(*GetVoid(), PTY_ptr);
  }
}

TypeTable::~TypeTable() {
  for (auto index = static_cast<uint32>(PTY_void); index < typeTable.size(); ++index) {
    delete typeTable[index];
    typeTable[index] = nullptr;
  }
}

void TypeTable::PutToHashTable(MIRType *mirType) {
  typeHashTable.insert(mirType);
}

TyIdx TypeTable::GetOrCreateMIRType(MIRType *pType) {
  const auto it = typeHashTable.find(pType);
  if (it != typeHashTable.end()) {
    return (*it)->GetTypeIndex();
  }

  MIRType *newTy = CreateType(*pType);
  PutToHashTable(newTy);
  return newTy->GetTypeIndex();
}

MIRType *TypeTable::voidPtrType = nullptr;
// get or create a type that pointing to pointedTyIdx
MIRType *TypeTable::GetOrCreatePointerType(TyIdx pointedTyIdx, PrimType primType) {
  MIRPtrType type(pointedTyIdx, primType);
  TyIdx tyIdx = GetOrCreateMIRType(&type);
  ASSERT(tyIdx.GetIdx() < typeTable.size(), "index out of range in TypeTable::GetOrCreatePointerType");
  return typeTable.at(tyIdx.GetIdx());
}

MIRType *TypeTable::GetOrCreatePointerType(const MIRType &pointTo, PrimType primType) {
  if (pointTo.GetPrimType() == PTY_constStr) {
    primType = PTY_ptr;
  }
  return GetOrCreatePointerType(pointTo.GetTypeIndex(), primType);
}

const MIRType *TypeTable::GetPointedTypeIfApplicable(MIRType &type) const {
  if (type.GetKind() != kTypePointer) {
    return &type;
  }
  auto &ptrType = static_cast<MIRPtrType&>(type);
  return GetTypeFromTyIdx(ptrType.GetPointedTyIdx());
}
MIRType *TypeTable::GetPointedTypeIfApplicable(MIRType &type) {
  return const_cast<MIRType*>(const_cast<const TypeTable*>(this)->GetPointedTypeIfApplicable(type));
}

MIRArrayType *TypeTable::GetOrCreateArrayType(const MIRType &elem, uint8 dim, const uint32 *sizeArray) {
  std::vector<uint32> sizeVector;
  for (size_t i = 0; i < dim; ++i) {
    sizeVector.push_back(sizeArray != nullptr ? sizeArray[i] : 0);
  }
  MIRArrayType arrayType(elem.GetTypeIndex(), sizeVector);
  TyIdx tyIdx = GetOrCreateMIRType(&arrayType);
  return static_cast<MIRArrayType*>(typeTable[tyIdx.GetIdx()]);
}

// For one dimension array
MIRArrayType *TypeTable::GetOrCreateArrayType(const MIRType &elem, uint32 size) {
  return GetOrCreateArrayType(elem, 1, &size);
}

MIRType *TypeTable::GetOrCreateFarrayType(const MIRType &elem) {
  MIRFarrayType type;
  type.SetElemtTyIdx(elem.GetTypeIndex());
  TyIdx tyIdx = GetOrCreateMIRType(&type);
  ASSERT(tyIdx.GetIdx() < typeTable.size(), "index out of range in TypeTable::GetOrCreateFarrayType");
  return typeTable.at(tyIdx.GetIdx());
}

MIRType *TypeTable::GetOrCreateJarrayType(const MIRType &elem) {
  MIRJarrayType type;
  type.SetElemtTyIdx(elem.GetTypeIndex());
  TyIdx tyIdx = GetOrCreateMIRType(&type);
  ASSERT(tyIdx.GetIdx() < typeTable.size(), "index out of range in TypeTable::GetOrCreateJarrayType");
  return typeTable.at(tyIdx.GetIdx());
}

MIRType *TypeTable::GetOrCreateFunctionType(MIRModule &module, TyIdx retTyIdx, const std::vector<TyIdx> &vecType,
                                            const std::vector<TypeAttrs> &vecAttrs, bool isVarg, bool isSimpCreate) {
  auto *funcType = module.GetMemPool()->New<MIRFuncType>(retTyIdx, vecType, vecAttrs);
  funcType->SetVarArgs(isVarg);
  if (isSimpCreate) {
    return funcType;
  }
  TyIdx tyIdx = GetOrCreateMIRType(funcType);
  ASSERT(tyIdx.GetIdx() < typeTable.size(), "index out of range in TypeTable::GetOrCreateFunctionType");
  return typeTable.at(tyIdx.GetIdx());
}

MIRType *TypeTable::GetOrCreateStructOrUnion(const std::string &name, const FieldVector &fields,
                                             const FieldVector &printFields, MIRModule &module, bool forStruct) {
  GStrIdx strIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(name);
  MIRStructType type(forStruct ? kTypeStruct : kTypeUnion, strIdx);
  type.GetFields() = fields;
  type.GetParentFields() = printFields;
  TyIdx tyIdx = GetOrCreateMIRType(&type);
  // Global?
  module.GetTypeNameTab()->SetGStrIdxToTyIdx(strIdx, tyIdx);
  module.PushbackTypeDefOrder(strIdx);
  ASSERT(tyIdx.GetIdx() < typeTable.size(), "index out of range in TypeTable::GetOrCreateStructOrUnion");
  return typeTable.at(tyIdx.GetIdx());
}

void TypeTable::PushIntoFieldVector(FieldVector &fields, const std::string &name, MIRType &type) {
  GStrIdx strIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(name);
  fields.push_back(FieldPair(strIdx, TyIdxFieldAttrPair(type.GetTypeIndex(), FieldAttrs())));
}

MIRType *TypeTable::GetOrCreateClassOrInterface(const std::string &name, MIRModule &module, bool forClass) {
  GStrIdx strIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(name);
  TyIdx tyIdx = module.GetTypeNameTab()->GetTyIdxFromGStrIdx(strIdx);
  if (!tyIdx.GetIdx()) {
    if (forClass) {
      MIRClassType type(kTypeClassIncomplete, strIdx);  // for class type
      tyIdx = GetOrCreateMIRType(&type);
    } else {
      MIRInterfaceType type(kTypeInterfaceIncomplete, strIdx);  // for interface type
      tyIdx = GetOrCreateMIRType(&type);
    }
    module.PushbackTypeDefOrder(strIdx);
    module.GetTypeNameTab()->SetGStrIdxToTyIdx(strIdx, tyIdx);
    if (typeTable[tyIdx.GetIdx()]->GetNameStrIdx() == 0) {
      typeTable[tyIdx.GetIdx()]->SetNameStrIdx(strIdx);
    }
  }
  ASSERT(tyIdx.GetIdx() < typeTable.size(), "index out of range in TypeTable::GetOrCreateClassOrInterface");
  return typeTable.at(tyIdx.GetIdx());
}

void TypeTable::AddFieldToStructType(MIRStructType &structType, const std::string &fieldName, MIRType &fieldType) {
  GStrIdx strIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(fieldName);
  FieldAttrs fieldAttrs;
  fieldAttrs.SetAttr(FLDATTR_final);  // Mark compiler-generated struct fields as final to improve AliasAnalysis
  structType.GetFields().push_back(FieldPair(strIdx, TyIdxFieldAttrPair(fieldType.GetTypeIndex(), fieldAttrs)));
}

void FPConstTable::PostInit() {
  MIRType &typeFloat = *GlobalTables::GetTypeTable().GetPrimType(PTY_f32);
  nanFloatConst = new MIRFloatConst(NAN, typeFloat);
  infFloatConst = new MIRFloatConst(INFINITY, typeFloat);
  minusInfFloatConst = new MIRFloatConst(-INFINITY, typeFloat);
  minusZeroFloatConst = new MIRFloatConst(-0.0, typeFloat);
  MIRType &typeDouble = *GlobalTables::GetTypeTable().GetPrimType(PTY_f64);
  nanDoubleConst = new MIRDoubleConst(NAN, typeDouble);
  infDoubleConst = new MIRDoubleConst(INFINITY, typeDouble);
  minusInfDoubleConst = new MIRDoubleConst(-INFINITY, typeDouble);
  minusZeroDoubleConst = new MIRDoubleConst(-0.0, typeDouble);
}

MIRFloatConst *FPConstTable::GetOrCreateFloatConst(float floatVal) {
  if (std::isnan(floatVal)) {
    return nanFloatConst;
  }
  if (std::isinf(floatVal)) {
    return (floatVal < 0) ? minusInfFloatConst : infFloatConst;
  }
  if (floatVal == 0.0 && std::signbit(floatVal)) {
    return minusZeroFloatConst;
  }
  const auto it = floatConstTable.find(floatVal);
  if (it == floatConstTable.cend()) {
    // create a new one
    auto *floatConst = new MIRFloatConst(floatVal, *GlobalTables::GetTypeTable().GetTypeFromTyIdx(TyIdx{ PTY_f32 }));
    floatConstTable[floatVal] = floatConst;
    return floatConst;
  }
  return it->second;
}

MIRDoubleConst *FPConstTable::GetOrCreateDoubleConst(double floatVal) {
  if (std::isnan(floatVal)) {
    return nanDoubleConst;
  }
  if (std::isinf(floatVal)) {
    return (floatVal < 0) ? minusInfDoubleConst : infDoubleConst;
  }
  if (floatVal == 0.0 && std::signbit(floatVal)) {
    return minusZeroDoubleConst;
  }
  const auto it = doubleConstTable.find(floatVal);
  if (it == doubleConstTable.cend()) {
    // create a new one
    auto *doubleConst = new MIRDoubleConst(floatVal,
                                           *GlobalTables::GetTypeTable().GetTypeFromTyIdx((TyIdx)PTY_f64));
    doubleConstTable[floatVal] = doubleConst;
    return doubleConst;
  }
  return it->second;
}

FPConstTable::~FPConstTable() {
  delete nanFloatConst;
  delete infFloatConst;
  delete minusInfFloatConst;
  delete minusZeroFloatConst;
  delete nanDoubleConst;
  delete infDoubleConst;
  delete minusInfDoubleConst;
  delete minusZeroDoubleConst;
  for (const auto &floatConst : floatConstTable) {
    delete floatConst.second;
  }
  for (const auto &doubleConst : doubleConstTable) {
    delete doubleConst.second;
  }
}

GSymbolTable::GSymbolTable() {
  symbolTable.push_back(static_cast<MIRSymbol*>(nullptr));
}

GSymbolTable::~GSymbolTable() {
  for (MIRSymbol *symbol : symbolTable) {
    delete symbol;
  }
}

MIRSymbol *GSymbolTable::CreateSymbol(uint8 scopeID) {
  auto *st = new MIRSymbol(symbolTable.size(), scopeID);
  symbolTable.push_back(st);
  module->AddSymbol(st);
  return st;
}

bool GSymbolTable::AddToStringSymbolMap(const MIRSymbol &st) {
  GStrIdx strIdx = st.GetNameStrIdx();
  if (strIdxToStIdxMap[strIdx].FullIdx() != 0) {
    return false;
  }
  strIdxToStIdxMap[strIdx] = st.GetStIdx();
  return true;
}

bool GSymbolTable::RemoveFromStringSymbolMap(const MIRSymbol &st) {
  const auto it = strIdxToStIdxMap.find(st.GetNameStrIdx());
  if (it != strIdxToStIdxMap.cend()) {
    strIdxToStIdxMap.erase(it);
    return true;
  }
  return false;
}

void GSymbolTable::Dump(bool isLocal, int32 indent) const {
  for (size_t i = 1; i < symbolTable.size(); ++i) {
    const MIRSymbol *symbol = symbolTable[i];
    if (symbol != nullptr) {
      symbol->Dump(module, isLocal, indent);
    }
  }
}

GlobalTables GlobalTables::globalTables;
GlobalTables &GlobalTables::GetGlobalTables() {
  return globalTables;
}
}  // namespace maple
#endif  // MIR_FEATURE_FULL
