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
#ifndef MAPLEBE_INCLUDE_CG_DEPS_H
#define MAPLEBE_INCLUDE_CG_DEPS_H

#include "mad.h"
#include "pressure.h"
#include <array>
namespace maplebe {
#define PRINT_STR_VAL(STR, VAL) \
  LogInfo::MapleLogger() << std::left << std::setw(12) << STR << VAL << " | ";
#define PRINT_VAL(VAL) \
  LogInfo::MapleLogger() << std::left << std::setw(12) << VAL << " | ";

enum DepType : uint8 {
  kDependenceTypeTrue,
  kDependenceTypeOutput,
  kDependenceTypeAnti,
  kDependenceTypeControl,
  kDependenceTypeMembar,
  kDependenceTypeThrow,
  kDependenceTypeSeparator,
  kDependenceTypeNone
};

const std::array<std::string, kDependenceTypeNone + 1> kDepTypeName = {
  "true-dep",
  "output-dep",
  "anti-dep",
  "control-dep",
  "membar-dep",
  "throw-dep",
  "separator-dep",
  "none-dep",
};

enum NodeType : uint8 {
  kNodeTypeNormal,
  kNodeTypeSeparator,
  kNodeTypeEmpty
};

enum ScheduleState : uint8 {
  kNormal,
  kReady,
  kScheduled,
};

class DepNode;

class DepLink {
 public:
  DepLink(DepNode &fromNode, DepNode &toNode, DepType typ) : from(fromNode), to(toNode), depType(typ), latency(0) {}
  virtual ~DepLink() = default;

  DepNode &GetFrom() const {
    return from;
  }
  DepNode &GetTo() const {
    return to;
  }
  void SetDepType(DepType dType) {
    depType = dType;
  }
  DepType GetDepType() const {
    return depType;
  }
  void SetLatency(uint32 lat) {
    latency = lat;
  }
  uint32 GetLatency() const {
    return latency;
  }

 private:
  DepNode &from;
  DepNode &to;
  DepType depType;
  uint32 latency;
};

class DepNode {
 public:
  bool CanBeScheduled() const;
  void OccupyUnits();
  uint32 GetUnitKind() const;

  DepNode(Insn &insn, MapleAllocator &alloc)
      : insn(&insn), units(nullptr), reservation(nullptr), unitNum(0),
        eStart(0), lStart(0), visit(0), type(kNodeTypeNormal), state(kNormal), index(0), simulateCycle(0),
        schedCycle(0), bruteForceSchedCycle(0), validPredsSize(0), validSuccsSize(0),
        preds(alloc.Adapter()), succs(alloc.Adapter()), comments(alloc.Adapter()),
        cfiInsns(alloc.Adapter()), clinitInsns(alloc.Adapter()), locInsn(nullptr), regPressure(nullptr) {}

  DepNode(Insn &insn, MapleAllocator &alloc, Unit * const *unit, uint32 num, Reservation &rev)
      : insn(&insn), units(unit),
        reservation(&rev), unitNum(num), eStart(0), lStart(0), visit(0), type(kNodeTypeNormal), state(kNormal),
        index(0), simulateCycle(0), schedCycle(0), bruteForceSchedCycle(0), validPredsSize(0), validSuccsSize(0),
        preds(alloc.Adapter()), succs(alloc.Adapter()), comments(alloc.Adapter()), cfiInsns(alloc.Adapter()),
        clinitInsns(alloc.Adapter()), locInsn(nullptr), regPressure(nullptr) {}
  virtual ~DepNode() = default;

  Insn *GetInsn() const {
    return insn;
  }
  void SetInsn(Insn &rvInsn) {
    insn = &rvInsn;
  }
  void SetUnits(Unit * const *unit) {
    units = unit;
  }
  const Unit *GetUnitByIndex(uint32 idx) const {
    ASSERT(index < unitNum, "out of units");
    return units[idx];
  }
  Reservation *GetReservation() const {
    return reservation;
  }
  void SetReservation(Reservation &rev) {
    reservation = &rev;
  }
  uint32 GetUnitNum() const {
    return unitNum;
  }
  void SetUnitNum(uint32 num) {
    unitNum = num;
  }
  uint32 GetEStart() const {
    return eStart;
  }
  void SetEStart(uint32 start) {
    eStart = start;
  }
  uint32 GetLStart() const {
    return lStart;
  }
  void SetLStart(uint32 start) {
    lStart = start;
  }
  uint32 GetVisit() const {
    return visit;
  }
  void SetVisit(uint32 visitVal) {
    visit = visitVal;
  }
  void IncreaseVisit() {
    ++visit;
  }
  NodeType GetType() const {
    return type;
  }
  void SetType(NodeType nodeType) {
    type = nodeType;
  }
  ScheduleState GetState() const {
    return state;
  }
  void SetState(ScheduleState scheduleState) {
    state = scheduleState;
  }
  uint32 GetIndex() const {
    return index;
  }
  void SetIndex(uint32 idx) {
    index = idx;
  }
  void SetSchedCycle(uint32 cycle) {
    schedCycle = cycle;
  }
  uint32 GetSchedCycle() const {
    return schedCycle;
  }
  void SetSimulateCycle(uint32 cycle) {
    simulateCycle = cycle;
  }
  uint32 GetSimulateCycle() const {
    return simulateCycle;
  }
  void SetBruteForceSchedCycle(uint32 cycle) {
    bruteForceSchedCycle = cycle;
  }
  uint32 GetBruteForceSchedCycle() const {
    return bruteForceSchedCycle;
  }
  void SetValidPredsSize(uint32 validSize) {
    validPredsSize = validSize;
  }
  uint32 GetValidPredsSize() const {
    return validPredsSize;
  }
  void DescreaseValidPredsSize() {
    --validPredsSize;
  }
  void IncreaseValidPredsSize() {
    ++validPredsSize;
  }
  uint32 GetValidSuccsSize() const {
    return validSuccsSize;
  }
  void SetValidSuccsSize(uint32 size) {
    validSuccsSize = size;
  }
  const MapleVector<DepLink*> &GetPreds() const {
    return preds;
  }
  void AddPred(DepLink &depLink) {
    preds.push_back(&depLink);
  }
  void RemovePred() {
    preds.pop_back();
  }
  const MapleVector<DepLink*> &GetSuccs() const{
    return succs;
  }
  void AddSucc(DepLink &depLink) {
    succs.push_back(&depLink);
  }
  void RemoveSucc() {
    succs.pop_back();
  }
  const MapleVector<Insn*> &GetComments() const {
    return comments;
  }
  void SetComments(MapleVector<Insn*> com) {
    comments = com;
  }
  void AddComments(Insn &insn) {
    comments.push_back(&insn);
  }
  void ClearComments() {
    comments.clear();
  }
  const MapleVector<Insn*> &GetCfiInsns() const {
    return cfiInsns;
  }
  void SetCfiInsns(MapleVector<Insn*> insns) {
    cfiInsns = insns;
  }
  void AddCfiInsn(Insn &insn) {
    cfiInsns.push_back(&insn);
  }
  void ClearCfiInsns() {
    cfiInsns.clear();
  }
  const MapleVector<Insn*> &GetClinitInsns() const {
    return clinitInsns;
  }
  void SetClinitInsns(MapleVector<Insn*> insns) {
    clinitInsns = insns;
  }
  void AddClinitInsn(Insn &insn) {
    clinitInsns.push_back(&insn);
  }
  const RegPressure *GetRegPressure() const {
    return regPressure;
  }
  void SetRegPressure(RegPressure &pressure) {
    regPressure = &pressure;
  }
  void DumpRegPressure() const {
    if (regPressure) {
      regPressure->DumpRegPressure();
    }
  }
  void InitPressure() {
    regPressure->InitPressure();
  }
  const MapleVector<int32> &GetPressure() const {
    return regPressure->GetPressure();
  }

  void IncPressureByIndex(int32 idx) {
    regPressure->IncPressureByIndex(idx);
  }
  void DecPressureByIndex(int32 idx) {
    regPressure->DecPressureByIndex(idx);
  }

  const MapleVector<int32> &GetDeadDefNum() const {
    return regPressure->GetDeadDefNum();
  }
  void IncDeadDefByIndex(int32 idx) {
    regPressure->IncDeadDefByIndex(idx);
  }

  void SetRegUses(regno_t regNO, RegList &regList) {
    regPressure->SetRegUses(regNO, &regList);
  }
  void SetRegDefs(regno_t regNO, RegList *regList) {
    regPressure->SetRegDefs(regNO, regList);
  }

  int32 GetIncPressure() const {
    return regPressure->GetIncPressure();
  }
  void SetIncPressure(bool value) {
    regPressure->SetIncPressure(value);
  }
  int32 GetMaxDepth() const {
    return regPressure->GetMaxDepth();
  }
  void SetMaxDepth(int32 value) {
    regPressure->SetMaxDepth(value);
  }
  int32 GetNear() const {
    return regPressure->GetNear();
  }
  void SetNear(int32 value) {
    regPressure->SetNear(value);
  }
  int32 GetPriority() const {
    return regPressure->GetPriority();
  }
  void SetPriority(int32 value) {
    regPressure->SetPriority(value);
  }
  const MapleUnorderedMap<regno_t, RegList*> &GetRegUses() const {
    return regPressure->GetRegUses();
  }
  const MapleUnorderedMap<regno_t, RegList*> &GetRegDefs() const {
    return regPressure->GetRegDefs();
  }

  const Insn *GetLocInsn() const {
    return locInsn;
  }
  void SetLocInsn(const Insn &insn) {
    locInsn = &insn;
  }

  /* printf dep-node's information of scheduling */
  void DumpSchedInfo() const {
    PRINT_STR_VAL("estart: ", eStart);
    PRINT_STR_VAL("lstart: ", lStart);
    PRINT_STR_VAL("visit: ", visit);
    PRINT_STR_VAL("state: ", state);
    PRINT_STR_VAL("index: ", index);
    PRINT_STR_VAL("validPredsSize: ", validPredsSize);
    PRINT_STR_VAL("validSuccsSize: ", validSuccsSize);
    LogInfo::MapleLogger() << '\n';
  }

 private:
  Insn *insn;
  Unit * const *units;
  Reservation *reservation;
  uint32 unitNum;
  uint32 eStart;
  uint32 lStart;
  uint32 visit;
  NodeType type;
  ScheduleState state;
  uint32 index;
  uint32 simulateCycle;
  uint32 schedCycle;
  uint32 bruteForceSchedCycle;

  /* For scheduling, denotes unscheduled preds/succs number. */
  uint32 validPredsSize;
  uint32 validSuccsSize;

  /* Dependence links. */
  MapleVector<DepLink*> preds;
  MapleVector<DepLink*> succs;

  /* Non-machine instructions prior to insn, such as comments. */
  MapleVector<Insn*> comments;

  /* Non-machine instructions which follows insn, such as cfi instructions. */
  MapleVector<Insn*> cfiInsns;

  /* Special instructions which follows insn, such as clinit instructions. */
  MapleVector<Insn*> clinitInsns;

  /* loc insn which indicate insn location in source file */
  const Insn *locInsn;

  /* For register pressure analysis */
  RegPressure *regPressure;
};
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_DEPS_H */
