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
#ifndef MPLFE_INCLUDE_JBC_STACK2FE_HELPER_H
#define MPLFE_INCLUDE_JBC_STACK2FE_HELPER_H
#include <list>
#include <vector>
#include <set>
#include <string>
#include "mempool_allocator.h"
#include "jbc_stack_helper.h"
#include "feir_builder.h"
#include "feir_var_reg.h"

namespace maple {
class JBCStack2FEHelper {
 public:
  static const uint8 kRegNumOff = 1;
  static const uint8 kRegNumOffWide = 2;
  explicit JBCStack2FEHelper(bool argUseNestExpr = false);
  ~JBCStack2FEHelper() = default;
  uint32 GetRegNumForSlot(uint32 slotNum) const;
  uint32 GetRegNumForStack() const;
  bool PushItem(UniqueFEIRVar var, PrimType pty);
  UniqueFEIRVar PushItem(PrimType pty);
  UniqueFEIRStmt PushItem(UniqueFEIRExpr expr, PrimType pty, bool hasException = false);
  UniqueFEIRStmt PushItem(UniqueFEIRExpr expr, UniqueFEIRType type, bool hasException = false);
  UniqueFEIRVar PopItem(PrimType pty);
  UniqueFEIRVar PopItem(UniqueFEIRType type);
  UniqueFEIRVar PopItem(bool isWide, PrimType &pty);
  UniqueFEIRVar PopItemAuto(PrimType &pty);
  bool Swap();
  bool Pop(jbc::JBCOpcode opcode);
  bool Dup(jbc::JBCOpcode opcode);
  std::list<UniqueFEIRStmt> GenerateSwapStmts();
  std::list<UniqueFEIRStmt> LoadSwapStack(const JBCStackHelper &stackHelper, bool &success);
  std::string DumpStackInJavaFormat() const;
  std::string DumpStackInInternalFormat() const;
  void ClearStack() {
    stack.clear();
  }

  void SetNStacks(uint32 argNStacks) {
    nStacks = argNStacks;
    regNumForStacks.clear();
    for (uint32 i = 0; i < argNStacks; i++) {
      CHECK_FATAL(regNumForStacks.insert(i).second, "regNumForStacks insert failed");
    }
  }

  uint32 GetNStacks() const {
    return nStacks;
  }

  void SetNSwaps(uint32 argNSwaps) {
    nSwaps = argNSwaps;
  }

  uint32 GetNSwaps() const {
    return nSwaps;
  }

  void SetNLocals(uint32 argNLocals) {
    nLocals = argNLocals;
  }

  uint32 GetNLocals() const {
    return nLocals;
  }

  void SetNArgs(uint32 argNArgs) {
    nArgs = argNArgs;
  }

  uint32 GetNArgs() const {
    return nArgs;
  }

  static PrimType JBCStackItemTypeToPrimType(jbc::JBCPrimType itemType);
  static PrimType SimplifyPrimType(PrimType pty);

 protected:
  bool useNestExpr;
  uint32 nStacks = 0;
  uint32 nSwaps = 0;
  uint32 nLocals = 0;
  uint32 nArgs = 0;
  using StackItem = std::pair<UniqueFEIRVar, PrimType>;
  std::vector<StackItem> stack;   // list<pair<expr, isWide>>
  std::set<uint32> regNumForStacks;
  bool Pop();
  bool Pop2();
  bool Dup();
  bool DupX1();
  bool DupX2();
  bool Dup2();
  bool Dup2X1();
  bool Dup2X2();
  std::vector<PrimType> JBCStackItemTypesToPrimTypes(const std::vector<jbc::JBCPrimType> itemTypes);
  bool CheckSwapValid(const std::vector<PrimType> items) const;
  std::vector<std::pair<uint32, PrimType>> GeneralSwapRegNum(const std::vector<PrimType> items);
  StackItem MakeStackItem(UniqueFEIRVar var, PrimType pty) const {
    return std::make_pair(std::move(var), pty);
  }

  bool IsItemDummy(const StackItem &item) const {
    return item.first == nullptr && IsPrimTypeWide(item.second);
  }

  bool IsItemNormal(const StackItem &item) const {
    return item.first != nullptr && IsPrimTypeNormal(item.second);
  }

  bool IsItemWide(const StackItem &item) const {
    return item.first != nullptr && IsPrimTypeWide(item.second);
  }

  bool IsPrimTypeNormal(PrimType pty) const {
    return pty == PTY_i32 || pty == PTY_f32 || pty == PTY_ref || pty == PTY_a32;
  }

  bool IsPrimTypeWide(PrimType pty) const {
    return pty == PTY_i64 || pty == PTY_f64;
  }
};  // class JBCStack2FEHelper
}  // namespace maple
#endif  // MPLFE_INCLUDE_JBC_STACK2FE_HELPER_H
