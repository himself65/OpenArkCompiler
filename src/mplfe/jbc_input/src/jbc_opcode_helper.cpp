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
#include "jbc_opcode_helper.h"
#include "jbc_class_const.h"
#include "jbc_util.h"
#include "fe_type_manager.h"

namespace maple {
std::map<jbc::JBCOpcode, std::vector<jbc::JBCPrimType>> JBCOpcodeHelper::mapOpInputPrimTypes =
    JBCOpcodeHelper::InitMapOpInputPrimTypes();

std::map<jbc::JBCOpcodeKind, JBCOpcodeHelper::FuncPtrGetBaseTypeName> JBCOpcodeHelper::funcPtrMapGetBaseTypeName =
    JBCOpcodeHelper::InitFuncPtrMapGetBaseTypeName();

JBCOpcodeHelper::JBCOpcodeHelper(const jbc::JBCClassMethod &argMethod) : method(argMethod) {}

std::vector<std::string> JBCOpcodeHelper::GetBaseTypeNamesForOP(const jbc::JBCOp &op, bool &success) {
  auto it = funcPtrMapGetBaseTypeName.find(op.GetOpcodeKind());
  if (it == funcPtrMapGetBaseTypeName.end()) {
    return GetBaseTypeNamesForOPDefault(op, success);
  } else {
    return (this->*(it->second))(op, success);
  }
}

std::map<jbc::JBCOpcodeKind, JBCOpcodeHelper::FuncPtrGetBaseTypeName> JBCOpcodeHelper::InitFuncPtrMapGetBaseTypeName() {
  std::map<jbc::JBCOpcodeKind, JBCOpcodeHelper::FuncPtrGetBaseTypeName> ans;
  ans[jbc::kOpKindConst] = &JBCOpcodeHelper::GetBaseTypeNamesForOPConst;
  ans[jbc::kOpKindStaticFieldOpr] = &JBCOpcodeHelper::GetBaseTypeNamesForOPFieldOpr;
  ans[jbc::kOpKindFieldOpr] = &JBCOpcodeHelper::GetBaseTypeNamesForOPFieldOpr;
  ans[jbc::kOpKindInvoke] = &JBCOpcodeHelper::GetBaseTypeNamesForOPInvoke;
  ans[jbc::kOpKindNew] = &JBCOpcodeHelper::GetBaseTypeNamesForOPNew;
  ans[jbc::kOpKindMultiANewArray] = &JBCOpcodeHelper::GetBaseTypeNamesForOPMultiANewArray;
  ans[jbc::kOpKindTypeCheck] = &JBCOpcodeHelper::GetBaseTypeNamesForOPTypeCheck;
  return ans;
}

std::vector<std::string> JBCOpcodeHelper::GetBaseTypeNamesForOPDefault(const jbc::JBCOp &op, bool &success) const {
  success = true;
  return std::vector<std::string>();
}

std::vector<std::string> JBCOpcodeHelper::GetBaseTypeNamesForOPConst(const jbc::JBCOp &op, bool &success) {
  std::vector<std::string> ans;
  const jbc::JBCOpConst &opConst = static_cast<const jbc::JBCOpConst&>(op);
  if (op.GetOpcode() == jbc::kOpLdc || op.GetOpcode() == jbc::kOpLdcW) {
    const jbc::JBCConstPool &constPool = method.GetConstPool();
    const jbc::JBCConst *constItem = constPool.GetConstByIdx(opConst.GetIndex());
    if (constItem == nullptr) {
      ssLastError.str("");
      ssLastError << "invalid const pool index (" << opConst.GetIndex() << ") in method "
                  << method.GetClassName() << "." << method.GetName() << ":" << method.GetDescription();
      success = false;
      return ans;
    }
    if (constItem->GetTag() == jbc::kConstClass) {
      success = true;
      const jbc::JBCConstClass *constClass = static_cast<const jbc::JBCConstClass*>(constItem);
      std::string typeName = constClass->GetClassNameOrin();
      std::string baseTypeName = FETypeManager::GetBaseTypeName(typeName, false);
      ans.push_back(baseTypeName);
    }
  }
  success = true;
  return ans;
}

std::vector<std::string> JBCOpcodeHelper::GetBaseTypeNamesForOPFieldOpr(const jbc::JBCOp &op, bool &success) {
  std::vector<std::string> ans;
  const jbc::JBCOpFieldOpr &opFieldOpr = static_cast<const jbc::JBCOpFieldOpr&>(op);
  const jbc::JBCConstPool &constPool = method.GetConstPool();
  const jbc::JBCConst *constItem = constPool.GetConstByIdxWithTag(opFieldOpr.GetFieldIdx(), jbc::kConstFieldRef);
  if (constItem == nullptr) {
    success = false;
    return ans;
  }
  const jbc::JBCConstRef *constRef = static_cast<const jbc::JBCConstRef*>(constItem);
  ans.push_back(FETypeManager::GetBaseTypeName(constRef->GetDesc(), false));
  success = true;
  return ans;
}

std::vector<std::string> JBCOpcodeHelper::GetBaseTypeNamesForOPInvoke(const jbc::JBCOp &op, bool &success) {
  std::vector<std::string> ans;
  const jbc::JBCOpInvoke &opInvoke = static_cast<const jbc::JBCOpInvoke&>(op);
  const jbc::JBCConstPool &constPool = method.GetConstPool();
  const jbc::JBCConst *constItem = constPool.GetConstByIdx(opInvoke.GetMethodIdx());
  if (constItem == nullptr ||
      (constItem->GetTag() != jbc::kConstMethodRef && constItem->GetTag() != jbc::kConstInterfaceMethodRef)) {
    success = false;
    return ans;
  }
  const jbc::JBCConstRef *constRef = static_cast<const jbc::JBCConstRef*>(constItem);
  const std::string desc = constRef->GetDesc();
  std::vector<std::string> typeNames = jbc::JBCUtil::SolveMethodSignature(desc);
  for (const std::string &typeName : typeNames) {
    ans.push_back(FETypeManager::GetBaseTypeName(typeName, false));
  }
  success = true;
  return ans;
}

std::vector<std::string> JBCOpcodeHelper::GetBaseTypeNamesForOPNew(const jbc::JBCOp &op, bool &success) {
  std::vector<std::string> ans;
  const jbc::JBCOpNew &opNew = static_cast<const jbc::JBCOpNew&>(op);
  const jbc::JBCConstPool &constPool = method.GetConstPool();
  if (op.GetOpcode() == jbc::kOpNew || op.GetOpcode() == jbc::kOpANewArray) {
    const jbc::JBCConst *constItem = constPool.GetConstByIdxWithTag(opNew.GetRefTypeIdx(), jbc::kConstClass);
    if (constItem == nullptr) {
      success = false;
      return ans;
    }
    const jbc::JBCConstClass *constClass = static_cast<const jbc::JBCConstClass*>(constItem);
    ans.push_back(FETypeManager::GetBaseTypeName(constClass->GetClassNameOrin(), false));
  }
  success = true;
  return ans;
}

std::vector<std::string> JBCOpcodeHelper::GetBaseTypeNamesForOPMultiANewArray(const jbc::JBCOp &op, bool &success) {
  std::vector<std::string> ans;
  const jbc::JBCOpMultiANewArray &opArray = static_cast<const jbc::JBCOpMultiANewArray&>(op);
  const jbc::JBCConstPool &constPool = method.GetConstPool();
  const jbc::JBCConst *constItem = constPool.GetConstByIdxWithTag(opArray.GetRefTypeIdx(), jbc::kConstClass);
  if (constItem == nullptr) {
    success = false;
    return ans;
  }
  const jbc::JBCConstClass *constClass = static_cast<const jbc::JBCConstClass*>(constItem);
  ans.push_back(FETypeManager::GetBaseTypeName(constClass->GetClassNameOrin(), false));
  success = true;
  return ans;
}

std::vector<std::string> JBCOpcodeHelper::GetBaseTypeNamesForOPTypeCheck(const jbc::JBCOp &op, bool &success) {
  std::vector<std::string> ans;
  const jbc::JBCOpTypeCheck &opTypeCheck = static_cast<const jbc::JBCOpTypeCheck&>(op);
  const jbc::JBCConstPool &constPool = method.GetConstPool();
  const jbc::JBCConst *constItem = constPool.GetConstByIdxWithTag(opTypeCheck.GetTypeIdx(), jbc::kConstClass);
  if (constItem == nullptr) {
    success = false;
    return ans;
  }
  const jbc::JBCConstClass *constClass = static_cast<const jbc::JBCConstClass*>(constItem);
  ans.push_back(FETypeManager::GetBaseTypeName(constClass->GetClassNameOrin(), false));
  success = true;
  return ans;
}

std::map<jbc::JBCOpcode, std::vector<jbc::JBCPrimType>> JBCOpcodeHelper::InitMapOpInputPrimTypes() {
  std::map<jbc::JBCOpcode, std::vector<jbc::JBCPrimType>> ans;
  InitMapOpInputPrimTypesForConst(ans);

  return ans;
}

void JBCOpcodeHelper::InitMapOpInputPrimTypesForConst(std::map<jbc::JBCOpcode, std::vector<jbc::JBCPrimType>> &ans) {
  ans[jbc::kOpAConstNull] = std::vector<jbc::JBCPrimType>();
  ans[jbc::kOpIConstM1] = std::vector<jbc::JBCPrimType>();
  ans[jbc::kOpIConst0] = std::vector<jbc::JBCPrimType>();
  ans[jbc::kOpIConst1] = std::vector<jbc::JBCPrimType>();
  ans[jbc::kOpIConst2] = std::vector<jbc::JBCPrimType>();
  ans[jbc::kOpIConst3] = std::vector<jbc::JBCPrimType>();
  ans[jbc::kOpIConst4] = std::vector<jbc::JBCPrimType>();
  ans[jbc::kOpIConst5] = std::vector<jbc::JBCPrimType>();
  ans[jbc::kOpLConst0] = std::vector<jbc::JBCPrimType>();
  ans[jbc::kOpLConst1] = std::vector<jbc::JBCPrimType>();
  ans[jbc::kOpFConst0] = std::vector<jbc::JBCPrimType>();
  ans[jbc::kOpFConst1] = std::vector<jbc::JBCPrimType>();
  ans[jbc::kOpFConst2] = std::vector<jbc::JBCPrimType>();
  ans[jbc::kOpDConst0] = std::vector<jbc::JBCPrimType>();
  ans[jbc::kOpDConst1] = std::vector<jbc::JBCPrimType>();
  ans[jbc::kOpBiPush] = std::vector<jbc::JBCPrimType>();
  ans[jbc::kOpSiPush] = std::vector<jbc::JBCPrimType>();
  ans[jbc::kOpLdc] = std::vector<jbc::JBCPrimType>();
  ans[jbc::kOpLdcW] = std::vector<jbc::JBCPrimType>();
  ans[jbc::kOpLdc2W] = std::vector<jbc::JBCPrimType>();
}
}  // namespace maple
