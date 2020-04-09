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
#ifndef MPLFE_INCLUDE_JBC_STACK_HELPER_H
#define MPLFE_INCLUDE_JBC_STACK_HELPER_H
#include "fe_configs.h"
#include "jbc_opcode.h"
#include "jbc_class_const_pool.h"

namespace maple {
class JBCStackHelper {
 public:
  JBCStackHelper() = default;
  ~JBCStackHelper() = default;
  void Reset();
  bool StackChange(const jbc::JBCOp &op, const jbc::JBCConstPool &constPool);
  void PushItem(jbc::JBCPrimType type);
  void PushItems(const std::vector<jbc::JBCPrimType> &types);
  bool PopItem(jbc::JBCPrimType type);
  bool PopItems(const std::vector<jbc::JBCPrimType> &types);
  void CopyFrom(const JBCStackHelper &src);
  bool EqualTo(const JBCStackHelper &src);
  bool Contains(const JBCStackHelper &src);
  void Dump() const;
  static std::string GetTypeName(jbc::JBCPrimType type);
  std::vector<jbc::JBCPrimType> GetStackItems() const;
  uint32 GetStackSize() const {
    size_t stackSize = stack.size();
    CHECK_FATAL(stackSize < UINT32_MAX, "stack size is too large");
    return static_cast<uint32>(stackSize);
  }

 LLT_PRIVATE:
  std::vector<jbc::JBCPrimType> stack;
  bool Pop(jbc::JBCOpcode opcode);
  bool Dup(jbc::JBCOpcode opcode);
  bool Dup();
  bool DupX1();
  bool DupX2();
  bool Dup2();
  bool Dup2X1();
  bool Dup2X2();
  bool Swap();
  bool IsType1(jbc::JBCPrimType type) const;
  bool IsType2(jbc::JBCPrimType type) const;
  bool IsType2Dummy(jbc::JBCPrimType type) const;
  jbc::JBCPrimType GetGeneralType(jbc::JBCPrimType type) const;
};  // class JBCStack
}  // namespace maple
#endif  // MPLFE_INCLUDE_JBC_STACK_HELPER_H
