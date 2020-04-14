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
#ifndef MPLFE_INCLUDE_COMMON_FE_STRUCT_ELEM_INFO_H
#define MPLFE_INCLUDE_COMMON_FE_STRUCT_ELEM_INFO_H
#include <memory>
#include <unordered_set>
#include "global_tables.h"
#include "fe_configs.h"
#include "feir_type.h"

namespace maple {
class FEStructElemInfo {
 public:
  FEStructElemInfo(const GStrIdx &argFullNameIdx, MIRSrcLang argSrcLang, bool argIsStatic);
  virtual ~FEStructElemInfo() = default;

  void Prepare(MIRBuilder &mirBuilder, bool argIsStatic) {
    PrepareImpl(mirBuilder, argIsStatic);
  }

  const std::string &GetStructName() const {
    return GlobalTables::GetStrTable().GetStringFromStrIdx(structNameIdx);
  }

  const std::string &GetElemName() const {
    return GlobalTables::GetStrTable().GetStringFromStrIdx(elemNameIdx);
  }

  const std::string &GetSignatureName() const {
    return GlobalTables::GetStrTable().GetStringFromStrIdx(signatureNameIdx);
  }

  bool IsStatic() const {
    return isStatic;
  }

  bool IsMethod() const {
    return isMethod;
  }

  bool IsDefined() const {
    return isDefined;
  }

  void SetDefined() {
    isDefined = true;
  }

  void SetUndefined() {
    isDefined = false;
  }

  bool IsFromDex() const {
    return isFromDex;
  }

  void SetFromDex() {
    isFromDex = true;
  }

  MIRSrcLang GetSrcLang() const {
    return srcLang;
  }

  void SetSrcLang(MIRSrcLang lang) {
    srcLang = lang;
  }

 LLT_PROTECTED:
  void Init();
  void InitJava();
  virtual void PrepareImpl(MIRBuilder &mirBuilder, bool argIsStatic) = 0;

  GStrIdx fullNameIdx;  // in maple format
  MIRSrcLang srcLang : 8;
  bool isStatic : 1;
  bool isMethod : 1;
  bool isDefined : 1;
  bool isFromDex : 1;
  bool isPrepared : 1;
  GStrIdx structNameIdx;  // in maple format
  GStrIdx elemNameIdx;  // in maple format
  GStrIdx signatureNameIdx;  // in maple format
};

using UniqueFEStructElemInfo = std::unique_ptr<FEStructElemInfo>;

class FEStructFieldInfo : public FEStructElemInfo {
 public:
  FEStructFieldInfo(const GStrIdx &argFullNameIdx, MIRSrcLang argSrcLang, bool argIsStatic);
  ~FEStructFieldInfo() = default;
  GStrIdx GetFieldNameIdx() const {
    return fieldNameIdx;
  }

  FieldID GetFieldID() const {
    return fieldID;
  }

  const UniqueFEIRType &GetType() const {
    return fieldType;
  }

 LLT_PROTECTED:
  void PrepareImpl(MIRBuilder &mirBuilder, bool argIsStatic) override;

 LLT_PRIVATE:
  void LoadFieldType();
  void LoadFieldTypeJava();
  void PrepareStaticField(MIRStructType &structType);
  void PrepareNonStaticField(MIRStructType &structType, MIRBuilder &mirBuilder);
  bool SearchStructFieldJava(MIRStructType &structType, MIRBuilder &mirBuilder, bool argIsStatic,
                             bool allowPrivate = true);
  bool SearchStructFieldJava(const TyIdx &tyIdx, MIRBuilder &mirBuilder, bool argIsStatic, bool allowPrivate = true);
  bool CompareFieldType(const FieldPair &fieldPair) const;

  UniqueFEIRType fieldType;
  GStrIdx fieldNameIdx;
  FieldID fieldID;
};

class FEStructMethodInfo : public FEStructElemInfo {
 public:
  FEStructMethodInfo(const GStrIdx &argFullNameIdx, MIRSrcLang argSrcLang, bool argIsStatic);
  ~FEStructMethodInfo() = default;
  PUIdx GetPuIdx() const;
  bool IsConstructor() const {
    return isConstructor;
  }

  bool IsReturnVoid() const {
    return isReturnVoid;
  }

  bool IsJavaPolymorphicCall() const {
    return isJavaPolymorphicCall;
  }

  bool IsJavaDynamicCall() const {
    return isJavaDynamicCall;
  }

  void SetJavaDyamicCall() {
    isJavaDynamicCall = true;
  }

  void UnsetJavaDynamicCall() {
    isJavaDynamicCall = false;
  }

  const UniqueFEIRType &GetReturnType() const {
    return retType;
  }

  const UniqueFEIRType &GetOwnerType() const {
    return ownerType;
  }

  const std::vector<UniqueFEIRType> &GetArgTypes() const {
    return argTypes;
  }

  static void InitJavaPolymorphicWhiteList();

 LLT_PROTECTED:
  void PrepareImpl(MIRBuilder &mirBuilder, bool argIsStatic) override;

 LLT_PRIVATE:
  void LoadMethodType();
  void LoadMethodTypeJava();
  void PrepareMethod();
  void PrepareImplJava(MIRBuilder &mirBuilder, bool argIsStatic);
  bool SearchStructMethodJava(MIRStructType &structType, MIRBuilder &mirBuilder, bool argIsStatic,
                              bool allowPrivate = true);
  bool SearchStructMethodJavaInParent(MIRStructType &structType, MIRBuilder &mirBuilder, bool argIsStatic);
  bool SearchStructMethodJava(const TyIdx &tyIdx, MIRBuilder &mirBuilder, bool argIsStatic, bool allowPrivate = true);
  bool CheckJavaPolymorphicCall() const;
  std::vector<UniqueFEIRType> argTypes;
  UniqueFEIRType retType;
  UniqueFEIRType ownerType;
  GStrIdx methodNameIdx;
  MIRFunction *mirFunc;
  bool isReturnVoid;
  bool isConstructor;
  bool isJavaPolymorphicCall;
  bool isJavaDynamicCall;
  static std::map<GStrIdx, std::set<GStrIdx>> javaPolymorphicWhiteList;
};
}  // namespace maple
#endif  // MPLFE_INCLUDE_COMMON_FE_STRUCT_ELEM_INFO_H
