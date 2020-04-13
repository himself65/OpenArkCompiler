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
#include "jbc_stack2fe_helper.h"
#include <algorithm>
#include "mpl_logging.h"

namespace maple {
JBCStack2FEHelper::JBCStack2FEHelper(bool argUseNestExpr)
    : useNestExpr(argUseNestExpr) {}

// Function Name: GetRegNumForSlot
// Description: adjust slotNum using following layout
//              set baseLocal as nStacks + nSwaps
//              set allLocals as nLocals + nArgs
//              0 ~ baseLocal - 1: stack reg (local)
//              baseLocal ~ baseLocal + nLocals - 1: local var
//                  (mapping to slotNum: nArgs ~ allLocals - 1)
//              baseLocal + nLocals ~ baseLocal + allLocals - 1: args var
//                  (mapping to slotNum: 0 ~ nArgs - 1)
//
uint32 JBCStack2FEHelper::GetRegNumForSlot(uint32 slotNum) const {
  uint32 baseLocal = nStacks + nSwaps;
  uint32 allLocals = nLocals + nArgs;
  CHECK_FATAL(slotNum < allLocals, "GetRegNumForSlot: out of range");
  if (slotNum >= nArgs) {
    // local var
    // nArgs ~ nArgs + nLocals - 1 --> baseLocal ~ baseLocal + nLocals - 1
    return baseLocal - nArgs + slotNum;
  } else {
    // arg var
    // 0 ~ nArgs - 1 --> baseLocal + nLocals ~ baseLocal + nLocals + nArgs - 1
    return baseLocal + nLocals + slotNum;
  }
}

uint32 JBCStack2FEHelper::GetRegNumForStack() const {
  std::set<uint32> setAvaliable = regNumForStacks;
  for (const StackItem &item : stack) {
    const UniqueFEIRVar &var = item.first;
    if (IsItemDummy(item)) {
      continue;
    }
    ASSERT(var->GetKind() == FEIRVarKind::kFEIRVarReg, "unsupported var kind");
    FEIRVarReg *ptrVarReg = static_cast<FEIRVarReg*>(var.get());
    (void)setAvaliable.erase(ptrVarReg->GetRegNum());
    if (IsItemWide(item)) {
      (void)setAvaliable.erase(ptrVarReg->GetRegNum() + 1);
    }
  }
  CHECK_FATAL(setAvaliable.size() > 0, "no avaliable reg number to use");
  return *(setAvaliable.begin());
}

bool JBCStack2FEHelper::PushItem(UniqueFEIRVar var, PrimType pty) {
  if (stack.size() >= nStacks) {
    ERR(kLncErr, "stack out of range");
    return false;
  }
  stack.push_back(MakeStackItem(std::move(var), pty));
  if (IsPrimTypeWide(pty)) {
    stack.push_back(MakeStackItem(UniqueFEIRVar(nullptr), pty));
  }
  return true;
}

UniqueFEIRVar JBCStack2FEHelper::PushItem(PrimType pty) {
  uint32 regNum = GetRegNumForStack();
  UniqueFEIRVar var = FEIRBuilder::CreateVarReg(regNum, pty);
  if (!PushItem(var->Clone(), pty)) {
    return UniqueFEIRVar(nullptr);
  } else {
    return var;
  }
}

UniqueFEIRStmt JBCStack2FEHelper::PushItem(UniqueFEIRExpr expr, PrimType pty, bool hasException) {
  uint32 regNum = GetRegNumForStack();
  UniqueFEIRVar varDst = FEIRBuilder::CreateVarReg(regNum, pty);
  UniqueFEIRStmt stmt = FEIRBuilder::CreateStmtDAssign(varDst->Clone(), std::move(expr), hasException);
  if (PushItem(std::move(varDst), pty)) {
    return UniqueFEIRStmt(nullptr);
  } else {
    return stmt;
  }
}

UniqueFEIRStmt JBCStack2FEHelper::PushItem(UniqueFEIRExpr expr, UniqueFEIRType type, bool hasException) {
  uint32 regNum = GetRegNumForStack();
  UniqueFEIRVar varDst = FEIRBuilder::CreateVarReg(regNum, std::move(type));
  UniqueFEIRStmt stmt = FEIRBuilder::CreateStmtDAssign(varDst->Clone(), std::move(expr), hasException);
  PrimType pty = PTY_unknown;
  if (!type->IsScalar()) {
    pty = PTY_ref;
  } else {
    pty = type->GetPrimType();
  }
  if (!PushItem(std::move(varDst), pty)) {
    return UniqueFEIRStmt(nullptr);
  } else {
    return stmt;
  }
}

UniqueFEIRVar JBCStack2FEHelper::PopItem(PrimType pty) {
  size_t size = stack.size();
  if (IsPrimTypeWide(pty)) {
    if (size < 2) {  // pop wide item operation need at least 2 items in stack
      ERR(kLncErr, "stack items are not enough for pop operation");
      return UniqueFEIRVar();
    }
    StackItem &item1 = stack[size - 1];  // the 1st item from top
    StackItem &item2 = stack[size - 2];  // the 2nd item from top
    if (IsItemDummy(item1) && IsItemWide(item2) && item2.second == pty) {
      UniqueFEIRVar ans = std::move(item2.first);
      stack.pop_back();
      stack.pop_back();
      return ans;
    } else {
      ERR(kLncErr, "invalid stack items for pop wide item operation");
      return UniqueFEIRVar(nullptr);
    }
  } else {
    if (size < 1) {  // pop normal item operation need at least 1 item in stack
      ERR(kLncErr, "stack items are not enough for pop operation");
      return UniqueFEIRVar(nullptr);
    }
    StackItem &item = stack[size - 1];  // the 1st item from top
    if (IsItemNormal(item) && item.second == pty) {
      UniqueFEIRVar ans = std::move(item.first);
      stack.pop_back();
      return ans;
    } else {
      ERR(kLncErr, "invalid stack items for pop item operation");
      return UniqueFEIRVar(nullptr);
    }
  }
}

UniqueFEIRVar JBCStack2FEHelper::PopItem(bool isWide, PrimType &pty) {
  size_t size = stack.size();
  pty = PTY_unknown;
  if (isWide) {
    if (size < 2) {  // pop wide item operation need at least 2 items in stack
      ERR(kLncErr, "stack items are not enough for pop operation");
      return UniqueFEIRVar(nullptr);
    }
    StackItem &item1 = stack[size - 1];  // the 1st item from top
    StackItem &item2 = stack[size - 2];  // the 2nd item from top
    if (IsItemDummy(item1) && IsItemWide(item2)) {
      UniqueFEIRVar ans = std::move(item2.first);
      pty = item2.second;
      stack.pop_back();
      stack.pop_back();
      return ans;
    } else {
      ERR(kLncErr, "invalid stack items for pop wide item operation");
      return UniqueFEIRVar(nullptr);
    }
  } else {
    if (size < 1) {  // pop normal item operation need at least 1 item in stack
      ERR(kLncErr, "stack items are not enough for pop operation");
      return UniqueFEIRVar(nullptr);
    }
    StackItem &item = stack[size - 1];  // the 1st item from top
    if (IsItemNormal(item)) {
      UniqueFEIRVar ans = std::move(item.first);
      pty = item.second;
      stack.pop_back();
      return ans;
    } else {
      ERR(kLncErr, "invalid stack items for pop wide item operation");
      return UniqueFEIRVar(nullptr);
    }
  }
}

UniqueFEIRVar JBCStack2FEHelper::PopItemAuto(PrimType &pty) {
  size_t size = stack.size();
  if (size < 1) {  // pop item operation need at least 1 item in stack
    ERR(kLncErr, "stack items are not enough for pop operation");
    return UniqueFEIRVar(nullptr);
  }
  pty = PTY_unknown;
  StackItem &item1 = stack[size - 1];  // the 1st item from top
  if (IsItemDummy(item1)) {
    if (size < 2) {  // pop wide item operation need at least 2 items in stack
      ERR(kLncErr, "stack items are not enough for pop operation");
      return UniqueFEIRVar(nullptr);
    }
    StackItem &item2 = stack[size - 2];  // the 2nd item from top
    if (IsItemWide(item2)) {
      UniqueFEIRVar ans = std::move(item2.first);
      pty = item2.second;
      stack.pop_back();
      stack.pop_back();
      return ans;
    } else {
      ERR(kLncErr, "invalid stack items for pop wide item operation");
      return UniqueFEIRVar(nullptr);
    }
  } else if (IsItemNormal(item1)) {
    UniqueFEIRVar ans = std::move(item1.first);
    pty = item1.second;
    stack.pop_back();
    return ans;
  } else {
    ERR(kLncErr, "invalid stack items for pop wide item operation");
    return UniqueFEIRVar(nullptr);
  }
}

bool JBCStack2FEHelper::Swap() {
  size_t size = stack.size();
  if (size < 2) {  // swap operation need at least 2 items in stack
    ERR(kLncErr, "stack is not enough for swap operation");
    return false;
  }
  StackItem &item1 = stack[size - 1];  // the 1st item from top
  StackItem &item2 = stack[size - 2];  // the 2nd item from top
  if (IsItemNormal(item1) && IsItemNormal(item2)) {
    UniqueFEIRVar var1 = std::move(item1.first);
    PrimType pty1 = item1.second;
    UniqueFEIRVar var2 = std::move(item2.first);
    PrimType pty2 = item2.second;
    stack.pop_back();
    stack.pop_back();
    stack.push_back(MakeStackItem(std::move(var1), pty1));
    stack.push_back(MakeStackItem(std::move(var2), pty2));
    return true;
  } else {
    ERR(kLncErr, "invalid stack items for swap operation");
    return false;
  }
}

bool JBCStack2FEHelper::Pop(jbc::JBCOpcode opcode) {
  switch (opcode) {
    case jbc::kOpPop:
      return Pop();
    case jbc::kOpPop2:
      return Pop2();
    default:
      ERR(kLncErr, "unsupported op: %s", jbc::JBCOp::GetOpcodeInfo().GetOpcodeName(opcode).c_str());
      return false;
  }
}

bool JBCStack2FEHelper::Dup(jbc::JBCOpcode opcode) {
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
      ERR(kLncErr, "unsupported op: %s", jbc::JBCOp::GetOpcodeInfo().GetOpcodeName(opcode).c_str());
      return false;
  }
}

std::list<UniqueFEIRStmt> JBCStack2FEHelper::GenerateSwapStmts() {
  std::list<UniqueFEIRStmt> ans;
  PrimType pty = PTY_unknown;
  UniqueFEIRVar varStack = PopItemAuto(pty);
  uint32 swapRegNum = nStacks;
  while (varStack != nullptr) {
    UniqueFEIRVar varSwap = std::make_unique<FEIRVarReg>(swapRegNum, pty);
    UniqueFEIRExpr exprDRead = FEIRBuilder::CreateExprDRead(std::move(varStack));
    UniqueFEIRStmt stmtDAssign = FEIRBuilder::CreateStmtDAssign(std::move(varSwap), std::move(exprDRead));
    ans.push_back(std::move(stmtDAssign));
    if (pty == PTY_i64 || pty == PTY_f64) {
      swapRegNum += kRegNumOffWide;
    } else {
      swapRegNum += kRegNumOff;
    }
    varStack = PopItemAuto(pty);
  }
  return ans;
}

std::list<UniqueFEIRStmt> JBCStack2FEHelper::LoadSwapStack(const JBCStackHelper &stackHelper, bool &success) {
  std::list<UniqueFEIRStmt> ans;
  std::vector<jbc::JBCPrimType> jbcStackItems = stackHelper.GetStackItems();
  std::vector<PrimType> primStackItems = JBCStackItemTypesToPrimTypes(jbcStackItems);
  stack.clear();
  if (!CheckSwapValid(primStackItems)) {
    success = false;
    return ans;
  }
  std::vector<std::pair<uint32, PrimType>> swapRegs = GeneralSwapRegNum(primStackItems);
  uint32 regNumStack = 0;
  for (const std::pair<uint32, PrimType> &item : swapRegs) {
    PrimType pty = item.second;
    UniqueFEIRVar varSwap = std::make_unique<FEIRVarReg>(item.first, pty);
    UniqueFEIRVar varStack = std::make_unique<FEIRVarReg>(regNumStack, pty);
    UniqueFEIRExpr exprDRead = FEIRBuilder::CreateExprDRead(std::move(varSwap));
    UniqueFEIRStmt stmtDAssign = FEIRBuilder::CreateStmtDAssign(varStack->Clone(), std::move(exprDRead));
    ans.push_back(std::move(stmtDAssign));
    stack.push_back(MakeStackItem(std::move(varStack), pty));
    if (pty == PTY_i64 || pty == PTY_f64) {
      regNumStack += kRegNumOffWide;
      stack.push_back(MakeStackItem(UniqueFEIRVar(nullptr), pty));
    } else {
      regNumStack += kRegNumOff;
    }
  }
  success = true;
  return ans;
}

PrimType JBCStack2FEHelper::JBCStackItemTypeToPrimType(jbc::JBCPrimType itemType) {
  switch (itemType) {
    case jbc::JBCPrimType::kTypeInt:
    case jbc::JBCPrimType::kTypeByteOrBoolean:
    case jbc::JBCPrimType::kTypeShort:
    case jbc::JBCPrimType::kTypeChar:
      return PTY_i32;
    case jbc::JBCPrimType::kTypeLong:
      return PTY_i64;
    case jbc::JBCPrimType::kTypeFloat:
      return PTY_f32;
    case jbc::JBCPrimType::kTypeDouble:
      return PTY_f64;
    case jbc::JBCPrimType::kTypeRef:
      return PTY_ref;
    case jbc::JBCPrimType::kTypeAddress:
      return PTY_a32;
    case jbc::JBCPrimType::kTypeDefault:
      return PTY_unknown;
    default:
      ERR(kLncErr, "Should not run here: unsupported type");
      return PTY_unknown;
  }
}

PrimType JBCStack2FEHelper::SimplifyPrimType(PrimType pty) {
  switch (pty) {
    case PTY_u1:   // boolean
    case PTY_i8:   // byte
    case PTY_i16:  // short
    case PTY_u16:  // char
      return PTY_i32;
    default:
      return pty;
  }
}

std::vector<PrimType> JBCStack2FEHelper::JBCStackItemTypesToPrimTypes(const std::vector<jbc::JBCPrimType> itemTypes) {
  std::vector<PrimType> primTypes;
  for (jbc::JBCPrimType itemType : itemTypes) {
    primTypes.push_back(JBCStackItemTypeToPrimType(itemType));
  }
  return primTypes;
}

bool JBCStack2FEHelper::CheckSwapValid(const std::vector<PrimType> items) const {
  uint32 size = 0;
  for (PrimType pty : items) {
    if (pty == PTY_i64 || pty == PTY_f64) {
      size += kRegNumOffWide;
    } else {
      size += kRegNumOff;
    }
  }
  if (size > nSwaps) {
    ERR(kLncErr, "swap stack out of range");
    return false;
  } else {
    return true;
  }
}

std::vector<std::pair<uint32, PrimType>> JBCStack2FEHelper::GeneralSwapRegNum(const std::vector<PrimType> items) {
  std::vector<std::pair<uint32, PrimType>> ans;
  size_t size = items.size();
  uint32 regNum = nStacks;
  for (size_t i = 1; i <= size; i++) {
    PrimType pty = items[size - i];
    ans.push_back(std::make_pair(regNum, pty));
    if (pty == PTY_i64 || pty == PTY_f64) {
      regNum += 2;  // wide type uses 2 stack items
    } else {
      regNum += 1;  // normal type uses 1 stack items
    }
  }
  std::reverse(ans.begin(), ans.end());
  return ans;
}

std::string JBCStack2FEHelper::DumpStackInJavaFormat() const {
  std::string ans;
  for (const StackItem &item : stack) {
    switch (item.second) {
      case PTY_i32:
        ans.push_back('I');
        break;
      case PTY_i64:
        ans.push_back('J');
        break;
      case PTY_f32:
        ans.push_back('F');
        break;
      case PTY_f64:
        ans.push_back('D');
        break;
      case PTY_ref:
        ans.push_back('R');
        break;
      default:
        return std::string("unsupport type") + GetPrimTypeName(item.second);
    }
  }
  return ans;
}

std::string JBCStack2FEHelper::DumpStackInInternalFormat() const {
  std::string ans;
  for (const StackItem &item : stack) {
    if (IsItemNormal(item)) {
      ans.push_back('N');
    } else if (IsItemWide(item)) {
      ans.push_back('W');
    } else if (IsItemDummy(item)) {
      ans.push_back('D');
    } else {
      return "unsupport item";
    }
  }
  return ans;
}

bool JBCStack2FEHelper::Pop() {
  size_t size = stack.size();
  if (size < 1) {
    ERR(kLncErr, "stack is not enough for pop operation");
    return false;
  }
  StackItem &item = stack[size - 1];  // the 1st item from top
  if (IsItemNormal(item)) {
    stack.pop_back();
    return true;
  } else {
    ERR(kLncErr, "invalid stack top item for pop operation");
    return false;
  }
}

bool JBCStack2FEHelper::Pop2() {
  size_t size = stack.size();
  if (size < 2) { // 2 : The minimum size of the stack
    ERR(kLncErr, "stack is not enough for pop2 operation");
    return false;
  }
  StackItem &item1 = stack[size - 1];  // the 1st item from top
  StackItem &item2 = stack[size - 2];  // the 2nd item from top
  if ((IsItemDummy(item1) && IsItemWide(item2)) || (IsItemNormal(item1) && IsItemNormal(item2))) {
    stack.pop_back();
    stack.pop_back();
    return true;
  } else {
    ERR(kLncErr, "invalid stack top item for pop2 operation");
    return false;
  }
}

// notation for value used in Dup
//   value(N): normal value (i32, f32)
//   value(W): wide value   (i64, f64)
//   value(D): dummy value of wide value
bool JBCStack2FEHelper::Dup() {
  size_t size = stack.size();
  if (size < 1) {  // dup operation need at least 1 item in stack
    ERR(kLncErr, "stack is not enough for dup operation");
    return false;
  }
  StackItem &item = stack[size - 1];  // the 1st item from top
  if (IsItemNormal(item)) {
    // before: ..., value(N) ->
    // after:  ..., value(N), value(N) ->
    UniqueFEIRVar var = item.first->Clone();
    PrimType pty = item.second;
    stack.push_back(MakeStackItem(std::move(var), pty));
    return true;
  } else {
    ERR(kLncErr, "invalid stack items for dup operation");
    return false;
  }
}

bool JBCStack2FEHelper::DupX1() {
  size_t size = stack.size();
  if (size < 2) {  // dup_x1 operation need at least 2 item in stack
    ERR(kLncErr, "stack is not enough for dup_x1 operation");
    return false;
  }
  StackItem &item1 = stack[size - 1];  // the 1st item from top
  StackItem &item2 = stack[size - 2];  // the 2nd item from top
  if (IsItemNormal(item1) && IsItemNormal(item2)) {
    // before: ..., value2(N), value1(N) ->
    // after:  ..., value1(N), value2(N), value1(N) ->
    UniqueFEIRVar var1 = item1.first->Clone();
    PrimType pty1 = item1.second;
    std::vector<StackItem>::iterator it = stack.end() - 2;  // the 2nd item position from top
    CHECK_FATAL(stack.insert(it, MakeStackItem(std::move(var1), pty1)) != stack.end(), "stack insert failed");
    return true;
  } else {
    ERR(kLncErr, "invalid stack items for dup_x1 operation");
    return false;
  }
}

bool JBCStack2FEHelper::DupX2() {
  size_t size = stack.size();
  if (size < 3) {  // dup_x1 operation need at least 3 item in stack
    ERR(kLncErr, "stack is not enough for dup_x2 operation");
    return false;
  }
  StackItem &item1 = stack[size - 1];  // the 1st item from top
  StackItem &item2 = stack[size - 2];  // the 2nd item from top
  StackItem &item3 = stack[size - 3];  // the 3rd item from top
  // situation 1
  //   before: ..., value3(N), value2(N), value1(N) ->
  //   after:  ..., value1(N), value3(N), value2(N), value1(N) ->
  bool isSituation1 = (IsItemNormal(item1) && IsItemNormal(item2) && IsItemNormal(item3));
  // situation 2
  //   before: ..., value3(W), value2(D), value1(N) ->
  //   after:  ..., value1(N), value3(W), value2(D), value1(N) ->
  bool isSituation2 = (IsItemNormal(item1) && IsItemDummy(item2) && IsItemWide(item3));
  if (!(isSituation1 || isSituation2)) {
    ERR(kLncErr, "invalid stack items for dup_x2 operation");
    return false;
  }
  //         situation1  situation2
  // value1      N           N
  UniqueFEIRVar var1 = item1.first->Clone();
  PrimType pty1 = item1.second;
  // insert copy of value1 before the 3rd item from top
  std::vector<StackItem>::iterator it = stack.end() - 3;
  CHECK_FATAL(stack.insert(it, MakeStackItem(std::move(var1), pty1)) != stack.end(), "stack insert failed");
  return true;
}

bool JBCStack2FEHelper::Dup2() {
  size_t size = stack.size();
  if (size < 2) {  // dup2 operation need at least 2 item in stack
    ERR(kLncErr, "stack is not enough for dup2 operation");
    return false;
  }
  StackItem &item1 = stack[size - 1];  // the 1st item from top
  StackItem &item2 = stack[size - 2];  // the 2nd item from top
  // situation 1
  //   before: ..., value2(N), value1(N) ->
  //   after:  ..., value2(N), value1(N), value2(N), value1(N) ->
  bool isSituation1 = (IsItemNormal(item1) && IsItemNormal(item2));
  // situation 2
  //   before: ..., value2(W), value1(D) ->
  //   after:  ..., value2(W), value1(D), value2(W), value1(D) ->
  bool isSituation2 = (IsItemDummy(item1) && IsItemWide(item2));
  if (!(isSituation1 || isSituation2)) {
    ERR(kLncErr, "invalid stack items for dup2 operation");
    return false;
  }
  //         situation1  situation2
  // value1      N           D
  // value2      N           W
  UniqueFEIRVar var1;
  UniqueFEIRVar var2;
  PrimType pty1 = item1.second;
  PrimType pty2 = item2.second;
  if (isSituation1) {
    var1 = item1.first->Clone();
    var2 = item2.first->Clone();
  } else {
    var1 = UniqueFEIRVar(nullptr);
    var2 = item2.first->Clone();
  }
  // insert copy of value2 before the 2nd item from top
  std::vector<StackItem>::iterator it = stack.end() - 2;
  CHECK_FATAL(stack.insert(it, MakeStackItem(std::move(var2), pty2)) != stack.end(), "stack insert failed");
  // insert copy of value1 before the 2nd item from top
  it = stack.end() - 2;
  CHECK_FATAL(stack.insert(it, MakeStackItem(std::move(var1), pty1)) != stack.end(), "stack insert failed");
  return true;
}

bool JBCStack2FEHelper::Dup2X1() {
  size_t size = stack.size();
  if (size < 3) {  // dup2_x1 operation need at least 3 item in stack
    ERR(kLncErr, "stack is not enough for dup2_x1 operation");
    return false;
  }
  StackItem &item1 = stack[size - 1];  // the 1st item from top
  StackItem &item2 = stack[size - 2];  // the 2nd item from top
  StackItem &item3 = stack[size - 3];  // the 3rd item from top
  // situation 1
  //   before: ..., value3(N), value2(N), value1(N) ->
  //   after:  ..., value2(N), value1(N), value3(N), value2(N), value1(N) ->
  bool isSituation1 = (IsItemNormal(item1) && IsItemNormal(item2) && IsItemNormal(item3));
  // situation 2
  //   before: ..., value3(N), value2(W), value1(D) ->
  //   after:  ..., value2(W), value1(D), value3(N), value2(W), value1(D) ->
  bool isSituation2 = (IsItemDummy(item1) && IsItemWide(item2) && IsItemNormal(item3));
  if (!(isSituation1 || isSituation2)) {
    ERR(kLncErr, "invalid stack items for dup2_x1 operation");
    return false;
  }
  //         situation1  situation2
  // value1      N           D
  // value2      N           W
  UniqueFEIRVar var1;
  UniqueFEIRVar var2;
  PrimType pty1 = item1.second;
  PrimType pty2 = item2.second;
  if (isSituation1) {
    var1 = item1.first->Clone();
    var2 = item2.first->Clone();
  } else {
    var1 = UniqueFEIRVar(nullptr);
    var2 = item2.first->Clone();
  }
  // insert copy of value2 before the 3rd item from top
  std::vector<StackItem>::iterator it = stack.end() - 3;
  CHECK_FATAL(stack.insert(it, MakeStackItem(std::move(var2), pty2)) != stack.end(), "stack insert failed");
  // insert copy of value1 before the 3rd item from top
  it = stack.end() - 3;
  CHECK_FATAL(stack.insert(it, MakeStackItem(std::move(var1), pty1)) != stack.end(), "stack insert failed");
  return true;
}

bool JBCStack2FEHelper::Dup2X2() {
  size_t size = stack.size();
  if (size < 4) {  // dup2_x2 operation need at least 4 item in stack
    ERR(kLncErr, "stack is not enough for dup2_x2 operation");
    return false;
  }
  StackItem &item1 = stack[size - 1];  // the 1st item from top
  StackItem &item2 = stack[size - 2];  // the 2nd item from top
  StackItem &item3 = stack[size - 3];  // the 3rd item from top
  StackItem &item4 = stack[size - 4];  // the 4th item from top
  // situation 1
  //   before: ..., value4(N), value3(N), value2(N), value1(N) ->
  //   after:  ..., value2(N), value1(N), value4(N), value3(N), value2(N), value1(N) ->
  bool isSituation1 = (IsItemNormal(item1) && IsItemNormal(item2) && IsItemNormal(item3) && IsItemNormal(item4));
  // situation 2
  //   before: ..., value4(N), value3(N), value2(W), value1(D) ->
  //   after:  ..., value2(W), value1(D), value4(N), value3(N), value2(W), value1(D) ->
  bool isSituation2 = (IsItemDummy(item1) && IsItemWide(item2) && IsItemNormal(item3) && IsItemNormal(item4));
  // situation 3
  //   before: ..., value4(W), value3(D), value2(N), value1(N) ->
  //   after:  ..., value2(N), value1(N), value4(W), value3(D), value2(W), value1(D) ->
  bool isSituation3 = (IsItemNormal(item1) && IsItemNormal(item2) && IsItemDummy(item3) && IsItemWide(item4));
  // situation 4
  //   before: ..., value4(W), value3(D), value2(W), value1(D) ->
  //   after:  ..., value2(W), value1(D), value4(W), value3(D), value2(W), value1(D) ->
  bool isSituation4 = (IsItemDummy(item1) && IsItemWide(item2) && IsItemDummy(item3) && IsItemWide(item4));
  if (!(isSituation1 || isSituation2 || isSituation3 || isSituation4)) {
    ERR(kLncErr, "invalid stack items for dup2_x1 operation");
    return false;
  }
  //         situation1  situation2  situation3  situation4
  // value1      N           D           N           D
  // value2      N           W           N           W
  UniqueFEIRVar var1;
  UniqueFEIRVar var2;
  PrimType pty1 = item1.second;
  PrimType pty2 = item2.second;
  if (isSituation1 || isSituation3) {
    var1 = item1.first->Clone();
    var2 = item2.first->Clone();
  } else {
    var1 = UniqueFEIRVar(nullptr);
    var2 = item2.first->Clone();
  }
  // insert copy of value2 before the 4th item from top
  std::vector<StackItem>::iterator it = stack.end() - 4;
  CHECK_FATAL(stack.insert(it, MakeStackItem(std::move(var2), pty2)) != stack.end(), "stack insert failed");
  // insert copy of value1 before the 4th item from top
  it = stack.end() - 4;
  CHECK_FATAL(stack.insert(it, MakeStackItem(std::move(var1), pty1)) != stack.end(), "stack insert failed");
  return true;
}
}  // namespace maple
