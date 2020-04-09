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
#ifndef MAPLEBE_INCLUDE_CG_REG_ALLOC_H
#define MAPLEBE_INCLUDE_CG_REG_ALLOC_H

#include <map>
#include <stack>
#include "isa.h"
#include "cg_phase.h"

namespace maplebe {
class VirtualRegNode {
 public:
  VirtualRegNode() = default;

  VirtualRegNode(RegType type, uint32 size) : regType(type), size(size), regNO(kInvalidRegNO) {}

  virtual ~VirtualRegNode() = default;

  void AssignPhysicalRegister(regno_t phyRegNO) {
    regNO = phyRegNO;
  }

  RegType GetType() const {
    return regType;
  }

  uint32 GetSize() const {
    return size;
  }

 private:
  RegType regType = kRegTyUndef;
  uint32 size     = 0;              /* size in bytes */
  regno_t regNO   = kInvalidRegNO;  /* physical register assigned by register allocation */
};

class RegAllocator {
 public:
  explicit RegAllocator(CGFunc &tempCGFunc) : cgFunc(&tempCGFunc) {}

  virtual ~RegAllocator() = default;

  virtual bool AllocateRegisters() = 0;

 protected:
  CGFunc *cgFunc;
};

CGFUNCPHASE(CgDoRegAlloc, "regalloc")
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_REG_ALLOC_H */