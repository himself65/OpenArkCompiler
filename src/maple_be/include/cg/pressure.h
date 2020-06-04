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
      : uses(alloc.Adapter()), defs(alloc.Adapter()),
        regUses(alloc.Adapter()) {}

  virtual ~RegPressure() = default;

  void DumpRegPressure() const;

  void AddUseReg(regno_t regNO) {
    uses.insert(regNO);
  }

  void AddDefReg(regno_t regNO) {
    defs.insert(regNO);
  }

  void SetRegUses(regno_t regNO, RegList *regList) {
    regUses.insert(std::pair<regno_t, RegList*>(regNO, regList));
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

  const int32 *GetPressure() const {
    return pressure;
  }

  void SetPressure(int32 *pressure) {
    this->pressure = pressure;
  }

  void IncPressureByIndex(int32 index) {
    ++pressure[index];
  }

  void DecPressureByIndex(int32 index) {
    --pressure[index];
  }

  void InitPressure() {
    FOR_ALL_REGCLASS(i) {
      pressure[i] = 0;
      incPressure = false;
    }
  }

  const MapleSet<regno_t> &GetUses() const {
    return uses;
  }

  const MapleSet<regno_t> &GetDefs() const {
    return defs;
  }

  const MapleMap<regno_t, RegList*> &GetRegUses() const {
    return regUses;
  }

 private:
  MapleSet<regno_t> uses;
  MapleSet<regno_t> defs;
  /* save reglist of every uses'register */
  MapleMap<regno_t, RegList*> regUses;
  int32 *pressure = nullptr;
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