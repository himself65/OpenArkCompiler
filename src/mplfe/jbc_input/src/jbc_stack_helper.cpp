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
#include "jbc_stack_helper.h"

namespace maple {
namespace {
const uint32 kSize1 = 1;
const uint32 kSize2 = 2;
const uint32 kSize3 = 3;
const uint32 kSize4 = 4;
}  // namespace

void JBCStackHelper::Reset() {
  stack.clear();
}

bool JBCStackHelper::StackChange(const jbc::JBCOp &op, const jbc::JBCConstPool &constPool) {
  switch (op.GetOpcodeKind()) {
    case jbc::kOpKindConst: {
      jbc::JBCPrimType stackOutType = op.GetOutputTypesToStack(constPool);
      if (stackOutType != jbc::JBCPrimType::kTypeDefault) {
        PushItem(stackOutType);
      } else {
        return false;
      }
      return true;
    }
    case jbc::kOpKindPop:
      return Pop(op.GetOpcode());
    case jbc::kOpKindDup:
      return Dup(op.GetOpcode());
    case jbc::kOpKindSwap:
      return Swap();
    case jbc::kOpKindFieldOpr:
    case jbc::kOpKindStaticFieldOpr:
    case jbc::kOpKindInvoke:
    case jbc::kOpKindMultiANewArray: {
      std::vector<jbc::JBCPrimType> stackInTypes = op.GetInputTypesFromStack(constPool);
      jbc::JBCPrimType stackOutType = op.GetOutputTypesToStack(constPool);
      bool success = PopItems(stackInTypes);
      if (success && stackOutType != jbc::JBCPrimType::kTypeDefault) {
        PushItem(stackOutType);
      }
      return success;
    }
    default: {
      const std::vector<jbc::JBCPrimType> &stackInTypes = op.GetInputTypesFromStack();
      jbc::JBCPrimType stackOutType = op.GetOutputTypesToStack();
      bool success = PopItems(stackInTypes);
      if (success && stackOutType != jbc::JBCPrimType::kTypeDefault) {
        PushItem(stackOutType);
      }
      return success;
    }
  }
}

void JBCStackHelper::PushItem(jbc::JBCPrimType type) {
  stack.push_back(GetGeneralType(type));
  if (type == jbc::JBCPrimType::kTypeLong) {
    stack.push_back(jbc::JBCPrimType::kTypeLongDummy);
  } else if (type == jbc::JBCPrimType::kTypeDouble) {
    stack.push_back(jbc::JBCPrimType::kTypeDoubleDummy);
  } else {
    // nothing to be done
  }
}

void JBCStackHelper::PushItems(const std::vector<jbc::JBCPrimType> &types) {
  for (jbc::JBCPrimType type : types) {
    PushItem(type);
  }
}

bool JBCStackHelper::PopItem(jbc::JBCPrimType type) {
  if (stack.size() == 0) {
    return false;
  }
  type = GetGeneralType(type);
  switch (type) {
    case jbc::JBCPrimType::kTypeInt:
    case jbc::JBCPrimType::kTypeFloat:
    case jbc::JBCPrimType::kTypeRef:
    case jbc::JBCPrimType::kTypeAddress:
      if (stack.size() >= kSize1 && stack.back() == type) {
        stack.pop_back();
        return true;
      }
      break;
    case jbc::JBCPrimType::kTypeLong:
      if (stack.size() >= kSize2 && stack.back() == jbc::JBCPrimType::kTypeLongDummy) {
        stack.pop_back();
        stack.pop_back();
        return true;
      }
      break;
    case jbc::JBCPrimType::kTypeDouble:
      if (stack.size() >= kSize2 && stack.back() == jbc::JBCPrimType::kTypeDoubleDummy) {
        stack.pop_back();
        stack.pop_back();
        return true;
      }
      break;
    case jbc::JBCPrimType::kTypeDefault:
      return true;
    default:
      CHECK_FATAL(false, "Should not run here: invalid type");
      break;
  }
  return false;
}

bool JBCStackHelper::PopItems(const std::vector<jbc::JBCPrimType> &types) {
  size_t idx = types.size() - 1;
  for (size_t i = 0; i < types.size(); i++, idx--) {
    if (PopItem(types[idx]) == false) {
      return false;
    }
  }
  return true;
}

bool JBCStackHelper::Pop(jbc::JBCOpcode opcode) {
  switch (opcode) {
    case jbc::kOpPop:
      if (stack.size() >= kSize1 && IsType1(stack.back())) {
        stack.pop_back();
        return true;
      }
      break;
    case jbc::kOpPop2:
      if (stack.size() >= kSize2 && IsType2Dummy(stack.back())) {
        stack.pop_back();
        stack.pop_back();
        return true;
      }
      break;
    default:
      CHECK_FATAL(false, "Unsupported opcode: %s", jbc::JBCOp::GetOpcodeInfo().GetOpcodeName(opcode).c_str());
      break;
  }
  return false;
}

bool JBCStackHelper::Dup(jbc::JBCOpcode opcode) {
  switch (opcode) {
    case jbc::kOpDup:
      return Dup();
    case jbc::kOpDupX1:
      return DupX1();
    case jbc::kOpDupX2:
      return DupX2();
    case jbc::kOpDup2:
      return Dup2();
    case jbc::kOpDup2X1:
      return Dup2X1();
    case jbc::kOpDup2X2:
      return Dup2X2();
    default:
      CHECK_FATAL(false, "Unsupported opcode: %s", jbc::JBCOp::GetOpcodeInfo().GetOpcodeName(opcode).c_str());
      break;
  }
  return false;
}

bool JBCStackHelper::Dup() {
  if (stack.size() < kSize1) {
    return false;
  }
  jbc::JBCPrimType t = stack.back();
  if (IsType1(t)) {
    stack.push_back(t);
    return true;
  }
  return false;
}

bool JBCStackHelper::DupX1() {
  if (stack.size() < kSize2) {
    return false;
  }
  jbc::JBCPrimType t1 = stack.back();
  stack.pop_back();
  jbc::JBCPrimType t2 = stack.back();
  stack.pop_back();
  bool success = (IsType1(t1) && IsType1(t2));
  if (success) {
    stack.push_back(t1);
  }
  stack.push_back(t2);
  stack.push_back(t1);
  return success;
}

bool JBCStackHelper::DupX2() {
  if (stack.size() < kSize3) {
    return false;
  }
  jbc::JBCPrimType t1 = stack.back();
  stack.pop_back();
  jbc::JBCPrimType t2 = stack.back();
  stack.pop_back();
  jbc::JBCPrimType t3 = stack.back();
  stack.pop_back();
  bool success = IsType1(t1);
  if (success) {
    stack.push_back(t1);
  }
  stack.push_back(t3);
  stack.push_back(t2);
  stack.push_back(t1);
  return success;
}

bool JBCStackHelper::Dup2() {
  if (stack.size() < kSize2) {
    return false;
  }
  jbc::JBCPrimType t1 = stack.back();
  stack.pop_back();
  jbc::JBCPrimType t2 = stack.back();
  stack.pop_back();
  bool success = ((IsType1(t1) && IsType1(t2)) || (IsType2Dummy(t1) && IsType2(t2)));
  if (success) {
    stack.push_back(t2);
    stack.push_back(t1);
  }
  stack.push_back(t2);
  stack.push_back(t1);
  return success;
}

bool JBCStackHelper::Dup2X1() {
  if (stack.size() < kSize3) {
    return false;
  }
  jbc::JBCPrimType t1 = stack.back();
  stack.pop_back();
  jbc::JBCPrimType t2 = stack.back();
  stack.pop_back();
  jbc::JBCPrimType t3 = stack.back();
  stack.pop_back();
  bool success = IsType1(t3);
  if (success) {
    stack.push_back(t2);
    stack.push_back(t1);
  }
  stack.push_back(t3);
  stack.push_back(t2);
  stack.push_back(t1);
  return success;
}

bool JBCStackHelper::Dup2X2() {
  if (stack.size() < kSize4) {
    return false;
  }
  jbc::JBCPrimType t1 = stack.back();
  stack.pop_back();
  jbc::JBCPrimType t2 = stack.back();
  stack.pop_back();
  jbc::JBCPrimType t3 = stack.back();
  stack.pop_back();
  jbc::JBCPrimType t4 = stack.back();
  stack.pop_back();
  stack.push_back(t2);
  stack.push_back(t1);
  stack.push_back(t4);
  stack.push_back(t3);
  stack.push_back(t2);
  stack.push_back(t1);
  return true;
}

bool JBCStackHelper::Swap() {
  if (stack.size() < kSize2) {
    return false;
  }
  jbc::JBCPrimType t1 = stack.back();
  stack.pop_back();
  jbc::JBCPrimType t2 = stack.back();
  stack.pop_back();
  if (IsType1(t1) && IsType1(t2)) {
    stack.push_back(t1);
    stack.push_back(t2);
    return true;
  } else {
    stack.push_back(t2);
    stack.push_back(t1);
    return false;
  }
}

bool JBCStackHelper::IsType1(jbc::JBCPrimType type) const {
  if (type == jbc::JBCPrimType::kTypeInt || type == jbc::JBCPrimType::kTypeFloat ||
      type == jbc::JBCPrimType::kTypeByteOrBoolean || type == jbc::JBCPrimType::kTypeChar ||
      type == jbc::JBCPrimType::kTypeShort || type == jbc::JBCPrimType::kTypeRef ||
      type == jbc::JBCPrimType::kTypeAddress) {
    return true;
  } else {
    return false;
  }
}

bool JBCStackHelper::IsType2(jbc::JBCPrimType type) const {
  if (type == jbc::JBCPrimType::kTypeLong || type == jbc::JBCPrimType::kTypeDouble) {
    return true;
  } else {
    return false;
  }
}

bool JBCStackHelper::IsType2Dummy(jbc::JBCPrimType type) const {
  if (type == jbc::JBCPrimType::kTypeLongDummy || type == jbc::JBCPrimType::kTypeDoubleDummy) {
    return true;
  } else {
    return false;
  }
}

jbc::JBCPrimType JBCStackHelper::GetGeneralType(jbc::JBCPrimType type) const {
  if (type == jbc::JBCPrimType::kTypeByteOrBoolean || type == jbc::JBCPrimType::kTypeChar ||
      type == jbc::JBCPrimType::kTypeShort) {
    return jbc::JBCPrimType::kTypeInt;
  } else {
    return type;
  }
}

void JBCStackHelper::CopyFrom(const JBCStackHelper &src) {
  for (jbc::JBCPrimType item : src.stack) {
    stack.push_back(item);
  }
}

bool JBCStackHelper::EqualTo(const JBCStackHelper &src) {
  if (stack.size() != src.stack.size()) {
    return false;
  }
  for (size_t i = 0; i < stack.size(); i++) {
    if (stack[i] != src.stack[i]) {
      return false;
    }
  }
  return true;
}

bool JBCStackHelper::Contains(const JBCStackHelper &src) {
  // this contains src (this >= src)
  if (EqualTo(src)) {
    return true;
  }
  size_t size = stack.size();
  size_t sizeSrc = src.stack.size();
  if (size < sizeSrc) {
    return false;
  }
  for (size_t i = 1; i <= sizeSrc; i++) {
    if (stack[size - i] != src.stack[sizeSrc - 1]) {
      return false;
    }
  }
  return true;
}

void JBCStackHelper::Dump() const {
  std::cout << "{";
  for (jbc::JBCPrimType item : stack) {
    std::cout << GetTypeName(item) << " ";
  }
  std::cout << "}";
}

std::string JBCStackHelper::GetTypeName(jbc::JBCPrimType type) {
  switch (type) {
    case jbc::JBCPrimType::kTypeDefault:
      return "default";
    case jbc::JBCPrimType::kTypeInt:
      return "int";
    case jbc::JBCPrimType::kTypeLong:
      return "long";
    case jbc::JBCPrimType::kTypeFloat:
      return "float";
    case jbc::JBCPrimType::kTypeDouble:
      return "double";
    case jbc::JBCPrimType::kTypeByteOrBoolean:
      return "byte/boolean";
    case jbc::JBCPrimType::kTypeChar:
      return "char";
    case jbc::JBCPrimType::kTypeShort:
      return "short";
    case jbc::JBCPrimType::kTypeRef:
      return "ref";
    case jbc::JBCPrimType::kTypeAddress:
      return "address";
    case jbc::JBCPrimType::kTypeLongDummy:
      return "long-dummy";
    case jbc::JBCPrimType::kTypeDoubleDummy:
      return "double-dummy";
    default:
      return "unknown";
  }
}

std::vector<jbc::JBCPrimType> JBCStackHelper::GetStackItems() const {
  std::vector<jbc::JBCPrimType> ans;
  for (jbc::JBCPrimType item : stack) {
    if (item != jbc::JBCPrimType::kTypeLongDummy && item != jbc::JBCPrimType::kTypeDoubleDummy) {
      ans.push_back(item);
    }
  }
  return ans;
}
}  // namespace maple
