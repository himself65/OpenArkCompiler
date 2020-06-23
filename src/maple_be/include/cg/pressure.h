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
#ifndef MAPLEBE_INCLUDE_CG_PRESSURE_H
#define MAPLEBE_INCLUDE_CG_PRESSURE_H

#include "cgbb.h"
#include "cgfunc.h"

namespace maplebe {
struct RegList {
  Insn *insn;
  RegList *next;
};

#define FOR_ALL_REGCLASS(i) \
  for (int32 i = 0; i < RegPressure::GetMaxRegClassNum(); ++i)

class RegPressure {
 public:
  explicit RegPressure(MapleAllocator &alloc)
      : regUses(alloc.Adapter()), regDefs(alloc.Adapter()),
        pressure(alloc.Adapter()), deadDefNum(alloc.Adapter()) {}

  virtual ~RegPressure() = default;

  void DumpRegPressure() const;

  void SetRegUses(regno_t regNO, RegList *regList) {
    regUses.insert(std::make_pair(regNO, regList));
  }

  void SetRegDefs(regno_t regNO, RegList *regList) {
    auto it = regDefs.find(regNO);
    if (it == regDefs.end()) {
      regDefs.insert(std::make_pair(regNO, regList));
    } else {
      it->second = regList;
    }
  }

  static void SetMaxRegClassNum(int32 maxClassNum) {
    maxRegClassNum = maxClassNum;
  }

  static int32 GetMaxRegClassNum() {
    return maxRegClassNum;
  }

  int32 GetPriority() const {
    return priority;
  }

  void SetPriority(int32 value) {
    priority = value;
  }

  int32 GetMaxDepth() const {
    return maxDepth;
  }

  void SetMaxDepth(int32 value) {
    maxDepth = value;
  }

  int32 GetNear() const {
    return near;
  }

  void SetNear(int32 value) {
    near = value;
  }

  int32 GetIncPressure() const {
    return incPressure;
  }

  void SetIncPressure(bool value) {
    incPressure = value;
  }
  const MapleVector<int32> &GetPressure() const {
    return pressure;
  }

  void IncPressureByIndex(int32 index) {
    ++pressure[index];
  }

  void DecPressureByIndex(int32 index) {
    --pressure[index];
  }

  void InitPressure() {
    pressure.resize(maxRegClassNum, 0);
    deadDefNum.resize(maxRegClassNum, 0);
    incPressure = false;
  }

  const MapleVector<int32> &GetDeadDefNum() const {
    return deadDefNum;
  }

  void IncDeadDefByIndex(int32 index) {
    ++deadDefNum[index];
  }

  const MapleUnorderedMap<regno_t, RegList*> &GetRegUses() const {
    return regUses;
  }

  const MapleUnorderedMap<regno_t, RegList*> &GetRegDefs() const {
    return regDefs;
  }

 private:
  /* save reglist of every uses'register */
  MapleUnorderedMap<regno_t, RegList*> regUses;
  /* save reglist of every defs'register */
  MapleUnorderedMap<regno_t, RegList*> regDefs;
  /* the number of the node needs registers */
  MapleVector<int32> pressure;
  /* the count of dead define registers */
  MapleVector<int32> deadDefNum;
  /* max number of reg's class */
  static int32 maxRegClassNum;
  int32 priority = 0;
  int32 maxDepth = 0;
  int32 near = 0;
  /* if a type register increase then set incPressure as true. */
  bool incPressure = false;
};
} /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_PRESSURE_H */
