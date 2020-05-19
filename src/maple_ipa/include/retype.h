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
#ifndef MAPLE_IPA_INCLUDE_RETYPE_H
#define MAPLE_IPA_INCLUDE_RETYPE_H
#include "module_phase.h"
#include "mir_nodes.h"
#include "class_hierarchy.h"

namespace maple {
class Retype {
 public:
  MIRModule *mirModule;
  MapleAllocator allocator;
  MIRBuilder &dexBuilder;
  KlassHierarchy *klassh;

 public:
  Retype(MIRModule *mod, MemPool *memPool, MIRBuilder &builder, KlassHierarchy *k)
      : mirModule(mod), allocator(memPool), dexBuilder(builder), klassh(k) {}

  virtual ~Retype() {}

  void ReplaceRetypeExpr(const BaseNode &opnd) const;
  void RetypeStmt(MIRFunction &func) const;
  void DoRetype() const;
};
}  // namespace maple
#endif  // MAPLE_IPA_INCLUDE_RETYPE_H
