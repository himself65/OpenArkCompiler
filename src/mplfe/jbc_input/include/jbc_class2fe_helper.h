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
#ifndef MPLFE_INCLUDE_JBC2FE_INPUT_HELPER_STRUCT_H
#define MPLFE_INCLUDE_JBC2FE_INPUT_HELPER_STRUCT_H
#include "fe_configs.h"
#include "jbc_class.h"
#include "jbc_input.h"
#include "fe_input_helper.h"

namespace maple {
class JBCClass2FEHelper : public FEInputStructHelper {
 public:
  JBCClass2FEHelper(MapleAllocator &allocator, const jbc::JBCClass &klassIn);
  ~JBCClass2FEHelper() = default;
  const jbc::JBCConstPool &GetConstPool() const {
    return klass.GetConstPool();
  }

  LLT_MOCK_TARGET bool IsStaticFieldProguard() const {
    return isStaticFieldProguard;
  }

 protected:
  std::string GetStructNameOrinImpl() const override;
  std::string GetStructNameMplImpl() const override;
  std::vector<std::string> GetSuperClassNamesImpl() const override;
  std::vector<std::string> GetInterfaceNamesImpl() const override;
  std::string GetSourceFileNameImpl() const override;
  MIRStructType *CreateMIRStructTypeImpl(bool &error) const override;
  TypeAttrs GetStructAttributeFromInputImpl() const override;
  uint64 GetRawAccessFlagsImpl() const override;
  void InitFieldHelpersImpl() override;
  void InitMethodHelpersImpl() override;
  std::string GetSrcFileNameImpl() const override;

 private:
  const jbc::JBCClass &klass;
  bool isStaticFieldProguard;
};

class JBCClassField2FEHelper : public FEInputFieldHelper {
 public:
  JBCClassField2FEHelper(MapleAllocator &allocator, const jbc::JBCClassField &fieldIn)
      : FEInputFieldHelper(allocator),
        field(fieldIn) {}
  ~JBCClassField2FEHelper() = default;
  static FieldAttrs AccessFlag2Attribute(uint16 accessFlag);

 protected:
  bool ProcessDeclImpl(MapleAllocator &allocator) override;
  bool ProcessDeclWithContainerImpl(MapleAllocator &allocator, const FEInputStructHelper &structHelper) override;

 private:
  const jbc::JBCClassField &field;
};

class JBCClassMethod2FEHelper : public FEInputMethodHelper {
 public:
  JBCClassMethod2FEHelper(MapleAllocator &allocator, const jbc::JBCClassMethod &methodIn);
  ~JBCClassMethod2FEHelper() = default;
  const jbc::JBCClassMethod &GetMethod() const {
    return method;
  }

 protected:
  void SolveReturnAndArgTypesImpl(MapleAllocator &allocator) override;
  std::string GetMethodNameImpl(bool inMpl) const override;
  FuncAttrs GetAttrsImpl() const override;
  bool IsStaticImpl() const override;
  bool IsVargImpl() const override;
  bool HasThisImpl() const override;
  MIRType *GetTypeForThisImpl() const override;

 private:
  bool IsClinit() const;
  bool IsInit() const;

  const jbc::JBCClassMethod &method;
};
}  // namespace maple
#endif  // MPLFE_INCLUDE_JBC2FE_INPUT_HELPER_STRUCT_H