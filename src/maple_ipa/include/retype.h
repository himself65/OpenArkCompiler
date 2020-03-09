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
#ifndef MAPLE_IPA_INCLUDE_RETYPE_H
#define MAPLE_IPA_INCLUDE_RETYPE_H
#include "module_phase.h"
#include "mir_nodes.h"
#include "class_hierarchy.h"

namespace maple {
class Retype {
 public:
  MIRModule *mirmodule;
  MapleAllocator allocator;
  MIRBuilder &dexbuilder;
  KlassHierarchy *klassh;

 public:
  explicit Retype(MIRModule *mod, MemPool *memPool, MIRBuilder &builder, KlassHierarchy *k)
      : mirmodule(mod), allocator(memPool), dexbuilder(builder), klassh(k) {}

  virtual ~Retype() {}

  void ReplaceRetypeExpr(const BaseNode *opnd);
  void Retypestmt(MIRFunction *func);
  void DoRetype();
};

}  // namespace maple
#endif  // MAPLE_IPA_INCLUDE_RETYPE_H
