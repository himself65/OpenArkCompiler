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
#ifndef MPLFE_INCLUDE_FE_INPUT_HELPER_H
#define MPLFE_INCLUDE_FE_INPUT_HELPER_H
#include "mempool_allocator.h"
#include "mir_type.h"
#include "mir_pragma.h"
#include "mir_symbol.h"
#include "feir_type.h"
#include "fe_function.h"

namespace maple {
class FEInputContainer {
 public:
  FEInputContainer() = default;
  virtual ~FEInputContainer() = default;
  MIRStructType *GetContainer() {
    return GetContainerImpl();
  }

 protected:
  virtual MIRStructType *GetContainerImpl() = 0;
};

class FEInputPragmaHelper {
 public:
  FEInputPragmaHelper() = default;
  virtual ~FEInputPragmaHelper() = default;
  MIRPragma *GenerateMIRPragma(MapleAllocator &allocator) {
    return GenerateMIRPragmaImpl(allocator);
  }

 protected:
  virtual MIRPragma *GenerateMIRPragmaImpl(MapleAllocator &allocator) = 0;
};

class FEInputStructHelper;
class FEInputFieldHelper {
 public:
  FEInputFieldHelper(MapleAllocator &allocator)
      : pragmaHelpers(allocator.Adapter()) {}
  virtual ~FEInputFieldHelper() = default;
  const FieldPair &GetMIRFieldPair() const {
    return mirFieldPair;
  }

  bool IsStatic() const {
    return mirFieldPair.second.second.GetAttr(FLDATTR_static);
  }

  bool ProcessDecl(MapleAllocator &allocator) {
    return ProcessDeclImpl(allocator);
  }

  bool ProcessDeclWithContainer(MapleAllocator &allocator, const FEInputStructHelper &structHelper) {
    return ProcessDeclWithContainerImpl(allocator, structHelper);
  }

 protected:
  virtual bool ProcessDeclImpl(MapleAllocator &allocator) = 0;
  virtual bool ProcessDeclWithContainerImpl(MapleAllocator &allocator, const FEInputStructHelper &structHelper) = 0;

  FieldPair mirFieldPair;
  MapleList<FEInputPragmaHelper*> pragmaHelpers;
};

class FEInputMethodHelper {
 public:
  FEInputMethodHelper(MapleAllocator &allocator)
      : srcLang(kSrcLangUnknown),
        feFunc(nullptr),
        mirFunc(nullptr),
        pragmaHelpers(allocator.Adapter()),
        retType(nullptr),
        argTypes(allocator.Adapter()),
        methodNameIdx(GStrIdx(0)) {}

  virtual ~FEInputMethodHelper() {
    feFunc = nullptr;
    mirFunc = nullptr;
    retType = nullptr;
  }

  const MethodPair &GetMIRMethodPair() const {
    return mirMethodPair;
  }

  const FEIRType *GetReturnType() const {
    return retType;
  }

  const MapleVector<FEIRType*> &GetArgTypes() const {
    return argTypes;
  }

  bool ProcessDecl(MapleAllocator &allocator) {
    return ProcessDeclImpl(allocator);
  }

  void SolveReturnAndArgTypes(MapleAllocator &allocator) {
    SolveReturnAndArgTypesImpl(allocator);
  }

  std::string GetMethodName(bool inMpl) const {
    return GetMethodNameImpl(inMpl);
  }

  FuncAttrs GetAttrs() const {
    return GetAttrsImpl();
  }

  bool IsStatic() const {
    return IsStaticImpl();
  }

  bool IsVarg() const {
    return IsVargImpl();
  }

  bool HasThis() const {
    return HasThisImpl();
  }

  MIRType *GetTypeForThis() const {
    return GetTypeForThisImpl();
  }

  GStrIdx GetMethodNameIdx() const {
    return methodNameIdx;
  }

 protected:
  virtual bool ProcessDeclImpl(MapleAllocator &allocator);
  virtual void SolveReturnAndArgTypesImpl(MapleAllocator &allocator) = 0;
  virtual std::string GetMethodNameImpl(bool inMpl) const = 0;
  virtual FuncAttrs GetAttrsImpl() const = 0;
  virtual bool IsStaticImpl() const = 0;
  virtual bool IsVargImpl() const = 0;
  virtual bool HasThisImpl() const = 0;
  virtual MIRType *GetTypeForThisImpl() const = 0;

  MIRSrcLang srcLang;
  FEFunction *feFunc;
  MIRFunction *mirFunc;
  MethodPair mirMethodPair;
  MapleList<FEInputPragmaHelper*> pragmaHelpers;
  FEIRType *retType;
  MapleVector<FEIRType*> argTypes;
  GStrIdx methodNameIdx;
};

class FEInputStructHelper : public FEInputContainer {
 public:
  explicit FEInputStructHelper(MapleAllocator &allocatorIn)
      : allocator(allocatorIn),
        mirStructType(nullptr),
        mirSymbol(nullptr),
        fieldHelpers(allocator.Adapter()),
        methodHelpers(allocator.Adapter()),
        pragmaHelpers(allocator.Adapter()),
        isSkipped(false),
        srcLang(kSrcLangUnknown) {}

  virtual ~FEInputStructHelper() {
    mirStructType = nullptr;
    mirSymbol = nullptr;
  }

  bool IsSkipped() const {
    return isSkipped;
  }

  const TypeAttrs GetStructAttributeFromSymbol() const {
    if (mirSymbol != nullptr) {
      return mirSymbol->GetAttrs();
    }
    return TypeAttrs();
  }

  MIRSrcLang GetSrcLang() const {
    return srcLang;
  }

  const MapleList<FEInputMethodHelper*> &GetMethodHelpers() const {
    return methodHelpers;
  }

  bool PreProcessDecl() {
    return PreProcessDeclImpl();
  }

  bool ProcessDecl() {
    return ProcessDeclImpl();
  }

  std::string GetStructNameOrin() const {
    return GetStructNameOrinImpl();
  }

  std::string GetStructNameMpl() const {
    return GetStructNameMplImpl();
  }

  std::vector<std::string> GetSuperClassNames() const {
    return GetSuperClassNamesImpl();
  }

  std::vector<std::string> GetInterfaceNames() const {
    return GetInterfaceNamesImpl();
  }

  std::string GetSourceFileName() const {
    return GetSourceFileNameImpl();
  }

  std::string GetSrcFileName() const {
    return GetSrcFileNameImpl();
  }

  MIRStructType *CreateMIRStructType(bool &error) const {
    return CreateMIRStructTypeImpl(error);
  }

  TypeAttrs GetStructAttributeFromInput() const {
    return GetStructAttributeFromInputImpl();
  }

  uint64 GetRawAccessFlags() const {
    return GetRawAccessFlagsImpl();
  }

  void InitFieldHelpers() {
    InitFieldHelpersImpl();
  }

  void InitMethodHelpers() {
    InitMethodHelpersImpl();
  }

 protected:
  MIRStructType *GetContainerImpl();
  virtual bool PreProcessDeclImpl();
  virtual bool ProcessDeclImpl();
  virtual std::string GetStructNameOrinImpl() const = 0;
  virtual std::string GetStructNameMplImpl() const = 0;
  virtual std::vector<std::string> GetSuperClassNamesImpl() const = 0;
  virtual std::vector<std::string> GetInterfaceNamesImpl() const = 0;
  virtual std::string GetSourceFileNameImpl() const = 0;
  virtual std::string GetSrcFileNameImpl() const;
  virtual MIRStructType *CreateMIRStructTypeImpl(bool &error) const = 0;
  virtual TypeAttrs GetStructAttributeFromInputImpl() const = 0;
  virtual uint64 GetRawAccessFlagsImpl() const = 0;
  virtual void InitFieldHelpersImpl() = 0;
  virtual void InitMethodHelpersImpl() = 0;
  void CreateSymbol();
  void ProcessDeclSuperClass();
  void ProcessDeclSuperClassForJava();
  void ProcessDeclImplements();
  void ProcessDeclDefInfo();
  void ProcessDeclDefInfoSuperNameForJava();
  void ProcessDeclDefInfoImplementNameForJava();
  void ProcessFieldDef();
  void ProcessMethodDef();

  MapleAllocator &allocator;
  MIRStructType *mirStructType;
  MIRSymbol *mirSymbol;
  MapleList<FEInputFieldHelper*> fieldHelpers;
  MapleList<FEInputMethodHelper*> methodHelpers;
  MapleList<FEInputPragmaHelper*> pragmaHelpers;
  bool isSkipped;
  MIRSrcLang srcLang;
};

class FEInputHelper {
 public:
  explicit FEInputHelper(MapleAllocator &allocatorIn)
      : allocator(allocatorIn),
        fieldHelpers(allocator.Adapter()),
        methodHelpers(allocator.Adapter()),
        structHelpers(allocator.Adapter()) {}
  ~FEInputHelper() = default;
  bool PreProcessDecl();
  bool ProcessDecl();
  bool ProcessImpl() const;
  void RegisterFieldHelper(FEInputFieldHelper *helper) {
    fieldHelpers.push_back(helper);
  }

  void RegisterMethodHelper(FEInputMethodHelper *helper) {
    methodHelpers.push_back(helper);
  }

  void RegisterStructHelper(FEInputStructHelper *helper) {
    structHelpers.push_back(helper);
  }

 private:
  MapleAllocator &allocator;
  MapleList<FEInputFieldHelper*> fieldHelpers;
  MapleList<FEInputMethodHelper*> methodHelpers;
  MapleList<FEInputStructHelper*> structHelpers;
};
}  // namespace maple
#endif  // MPLFE_INCLUDE_FE_INPUT_HELPER_H
