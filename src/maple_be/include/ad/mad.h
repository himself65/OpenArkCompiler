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
#ifndef MAPLEBE_INCLUDE_AD_MAD_H
#define MAPLEBE_INCLUDE_AD_MAD_H
#include <vector>
#include "types_def.h"
#include "mpl_logging.h"
#include "insn.h"

namespace maplebe {
enum UnitId : maple::uint32 {
#include "target/mplad_unit_id.def"
  kUnitIdLast
};

enum UnitType : maple::uint8 {
  kUnitTypePrimart,
  kUnitTypeOr,
  kUnitTypeAnd,
  KUnitTypeNone
};


enum SlotType : maple::uint8 {
  kSlotNone,
  kSlot0,
  kSlot1,
  kSlotAny,
  kSlots,
};

/* machine model */
enum LatencyType : maple::uint32 {
  /* LT: latency */
#include "target/mplad_latency_type.def"
  kLtLast,
};

class Unit {
 public:
  explicit Unit(enum UnitId theUnitId);
  Unit(enum UnitType theUnitType, enum UnitId theUnitId, int numOfUnits, ...);
  ~Unit() = default;

  enum UnitType GetUnitType() const {
    return unitType;
  }

  enum UnitId GetUnitId() const {
    return unitId;
  };

  const std::vector<Unit*> &GetCompositeUnits() const;

  std::string GetName() const;
  bool IsFree(maple::uint32 cycle) const;
  void Occupy(const Insn &insn, maple::uint32 cycle);
  unsigned int GetUnitTypeNum() const;
  void Release();
  void AdvanceCycle();
  void Dump(int indent = 0) const;
  maple::uint32 GetOccupancyTable() const;

  void SetOccupancyTable(maple::uint32 table) {
    occupancyTable = table;
  }

 private:
  void PrintIndent(int indent) const;

  enum UnitId unitId;
  enum UnitType unitType;
  maple::uint32 occupancyTable;
  std::vector<Unit*> compositeUnits;
};

class Reservation {
 public:
  Reservation(LatencyType t, int l, int n, ...);
  ~Reservation() = default;

  bool IsEqual(maple::uint32 typ) const {
    return typ == type;
  }

  int GetLatency() const {
    return latency;
  }

  uint32 GetUnitNum() const {
    return unitNum;
  }

  enum SlotType GetSlot() const {
    return slot;
  }

  const std::string &GetSlotName() const;

  Unit * const *GetUnit() const {
    return units;
  }

 private:
  static const int kMaxUnit = 13;
  LatencyType type;
  int latency;
  uint32 unitNum;
  Unit *units[kMaxUnit];
  enum SlotType slot;

  SlotType GetSlotType(UnitId unitID) const;
};

class Bypass {
 public:
  Bypass(LatencyType d, LatencyType u, int l) : def(d), use(u), latency(l) {}
  virtual ~Bypass() = default;

  virtual bool CanBypass(const Insn &defInsn, const Insn &useInsn) const;

  int GetLatency() const {
    return latency;
  }

  LatencyType GetDefType() const {
    return def;
  }

  LatencyType GetUseType() const {
    return use;
  }

 private:
  LatencyType def;
  LatencyType use;
  int latency;
};

class MAD {
 public:
  MAD() {
    InitUnits();
    InitParallelism();
    InitReservation();
    InitBypass();
  }

  ~MAD();

  using BypassVector = std::vector<Bypass*>;

  void InitUnits();
  void InitParallelism();
  void InitReservation();
  void InitBypass();
  bool IsSlot0Free() const;
  bool IsFullIssued() const;
  int GetLatency(const Insn &def, const Insn &use) const;
  int DefaultLatency(const Insn &insn) const;
  Reservation *FindReservation(const Insn &insn) const;
  void AdvanceCycle();
  void ReleaseAllUnits();
  void SaveStates(std::vector<maple::uint32> &occupyTable, int size) const;
  void RestoreStates(std::vector<maple::uint32> &occupyTable, int size);

  int GetMaxParallelism() const {
    return parallelism;
  }

  const Unit *GetUnitByUnitId(enum UnitId uId) const {
    CHECK_FATAL(!allUnits.empty(), "CHECK_CONTAINER_EMPTY");
    return allUnits[uId];
  }

  static void AddUnit(Unit &u) {
    allUnits.push_back(&u);
  }

  static maple::uint32 GetAllUnitsSize() {
    return allUnits.size();
  }

  static void AddReservation(Reservation &rev) {
    allReservations.push_back(&rev);
  }

  static void AddBypass(Bypass &bp) {
    ASSERT(bp.GetDefType() < kLtLast, "out of range");
    ASSERT(bp.GetUseType() < kLtLast, "out of range");
    (bypassArrays[bp.GetDefType()][bp.GetUseType()]).push_back(&bp);
  }

 protected:
  void SetMaxParallelism(int num) {
    parallelism = num;
  }

  int BypassLatency(const Insn &def, const Insn &use) const;

 private:
  static int parallelism;
  static std::vector<Unit*> allUnits;
  static std::vector<Reservation*> allReservations;
  static std::array<std::array<BypassVector, kLtLast>, kLtLast> bypassArrays;
};

class AluShiftBypass : public Bypass {
 public:
  AluShiftBypass(LatencyType d, LatencyType u, int l) : Bypass(d, u, l) {}
  ~AluShiftBypass() = default;

  bool CanBypass(const Insn &defInsn, const Insn &useInsn) const;
};

class AccumulatorBypass : public Bypass {
 public:
  AccumulatorBypass(LatencyType d, LatencyType u, int l) : Bypass(d, u, l) {}
  ~AccumulatorBypass() = default;

  bool CanBypass(const Insn &defInsn, const Insn &useInsn) const;
};

class StoreBypass : public Bypass {
 public:
  StoreBypass(LatencyType d, LatencyType u, int l) : Bypass(d, u, l) {}
  ~StoreBypass() = default;

  bool CanBypass(const Insn &defInsn, const Insn &useInsn) const;
};
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_AD_MAD_H */
