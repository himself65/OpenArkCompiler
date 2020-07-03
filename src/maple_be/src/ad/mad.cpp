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
#include "mad.h"
#include <string>
#include "aarch64_operand.h"
#include "schedule.h"
#include "insn.h"

namespace maplebe {
const std::string kUnitName[] = {
#include "mplad_unit_name.def"
  "None",
};
/* Unit */
Unit::Unit(enum UnitId theUnitId)
    : unitId(theUnitId), unitType(kUnitTypePrimart), occupancyTable(0), compositeUnits() {
  MAD::AddUnit(*this);
}

Unit::Unit(enum UnitType theUnitType, enum UnitId theUnitId, int numOfUnits, ...)
    : unitId(theUnitId), unitType(theUnitType), occupancyTable(0) {
  ASSERT(numOfUnits > 1, "CG internal error, composite unit with less than 2 unit elements.");
  va_list ap;
  va_start(ap, numOfUnits);

  for (int i = 0; i < numOfUnits; ++i) {
    compositeUnits.emplace_back(static_cast<Unit*>(va_arg(ap, Unit*)));
  }
  va_end(ap);

  MAD::AddUnit(*this);
}

/* return name of unit */
std::string Unit::GetName() const {
  ASSERT(GetUnitId() <= kUnitIdLast, "Unexpected UnitID");
  return kUnitName[GetUnitId()];
}

/* Check if unit is free at next "cycle" cycle. */
bool Unit::IsFree(uint32 cycle) const {
  if (GetUnitType() == kUnitTypeOr) {
    for (auto unit : compositeUnits) {
      if (unit->IsFree(cycle)) {
        return true;
      }
    }
    return false;
  } else if (GetUnitType() == kUnitTypeAnd) {
    for (auto unit : compositeUnits) {
      if (!unit->IsFree(cycle)) {
        return false;
      }
    }
    return true;
  }
  if ((occupancyTable & (1u << cycle)) != 0) {
    return false;
  }
  return true;
}

/* Occupy unit at next "cycle" cycle. */
void Unit::Occupy(const Insn &insn, uint32 cycle) {
  if (GetUnitType() == kUnitTypeOr) {
    for (auto unit : GetCompositeUnits()) {
      if (unit->IsFree(cycle)) {
        unit->Occupy(insn, cycle);
        return;
      }
    }

    ASSERT(false, "CG internal error, should not be reach here.");
    return;
  } else if (GetUnitType() == kUnitTypeAnd) {
    for (auto unit : GetCompositeUnits()) {
      unit->Occupy(insn, cycle);
    }
    return;
  }
  occupancyTable |= (1u << cycle);
}

/* Advance all units one cycle */
void Unit::AdvanceCycle() {
  if (GetUnitType() != kUnitTypePrimart) {
    return;
  }
  occupancyTable = (occupancyTable >> 1);
}

/* Release all units. */
void Unit::Release() {
  if (GetUnitType() != kUnitTypePrimart) {
    return;
  }
  occupancyTable = 0;
}

const std::vector<Unit*> &Unit::GetCompositeUnits() const {
  return compositeUnits;
}

void Unit::PrintIndent(int indent) const {
  for (int i = 0; i < indent; ++i) {
    LogInfo::MapleLogger() << " ";
  }
}

void Unit::Dump(int indent) const {
  PrintIndent(indent);
  LogInfo::MapleLogger() << "Unit " << GetName() << " (ID " << GetUnitId() << "): ";
  LogInfo::MapleLogger() << "occupancyTable = " << occupancyTable << '\n';
}

uint32 Unit::GetOccupancyTable() const {
  return occupancyTable;
}

/* MAD */
int MAD::parallelism;
std::vector<Unit*> MAD::allUnits;
std::vector<Reservation*> MAD::allReservations;
std::array<std::array<MAD::BypassVector, kLtLast>, kLtLast> MAD::bypassArrays;

MAD::~MAD() {
  for (auto unit : allUnits) {
    delete unit;
  }
  for (auto rev : allReservations) {
    delete rev;
  }
  for (auto &bypassArray : bypassArrays) {
    for (auto &bypassVector : bypassArray) {
      for (auto *bypass : bypassVector) {
        delete bypass;
      }
    }
  }
  allUnits.clear();
  allReservations.clear();
}

void MAD::InitUnits() {
#include "mplad_unit_define.def"
}

void MAD::InitReservation() {
#include "mplad_reservation_define.def"
}

void MAD::InitParallelism() {
#include "mplad_arch_define.def"
}

/* according insn's insnType to get a reservation */
Reservation *MAD::FindReservation(const Insn &insn) const {
  uint32 insnType = insn.GetLatencyType();
  for (auto reservation : allReservations) {
    if (reservation->IsEqual(insnType)) {
      return reservation;
    }
  }
  return nullptr;
}

/* Get latency that is def insn to use insn */
int MAD::GetLatency(const Insn &def, const Insn &use) const {
  int latency = BypassLatency(def, use);
  if (latency < 0) {
    latency = DefaultLatency(def);
  }
  return latency;
}

/* Get bypass latency that is  def insn to use insn */
int MAD::BypassLatency(const Insn &def, const Insn &use) const {
  int latency = -1;
  ASSERT(def.GetLatencyType() < kLtLast, "out of range");
  ASSERT(use.GetLatencyType() < kLtLast, "out of range");
  BypassVector &bypassVec = bypassArrays[def.GetLatencyType()][use.GetLatencyType()];
  for (auto bypass : bypassVec) {
    if (bypass->CanBypass(def, use)) {
      latency = bypass->GetLatency();
      break;
    }
  }
  return latency;
}

/* Get insn's default latency */
int MAD::DefaultLatency(const Insn &insn) const {
  Reservation *res = insn.GetDepNode()->GetReservation();
  return res != nullptr ? res->GetLatency() : 0;
}

void MAD::AdvanceCycle() {
  for (auto unit : allUnits) {
    unit->AdvanceCycle();
  }
}

void MAD::ReleaseAllUnits() {
  for (auto unit : allUnits) {
    unit->Release();
  }
}

void MAD::SaveStates(std::vector<uint32> &occupyTable, int size) const {
  int i = 0;
  for (auto unit : allUnits) {
    CHECK_FATAL(i < size, "unit number error");
    occupyTable[i] = unit->GetOccupancyTable();
    ++i;
  }
}

#define ADDBYPASS(DEFLTTY, USELTTY, LT) AddBypass(*(new Bypass(DEFLTTY, USELTTY, LT)))
#define ADDALUSHIFTBYPASS(DEFLTTY, USELTTY, LT) AddBypass(*(new AluShiftBypass(DEFLTTY, USELTTY, LT)))
#define ADDACCUMULATORBYPASS(DEFLTTY, USELTTY, LT) AddBypass(*(new AccumulatorBypass(DEFLTTY, USELTTY, LT)))
#define ADDSTOREBYPASS(DEFLTTY, USELTTY, LT) AddBypass(*(new StoreBypass(DEFLTTY, USELTTY, LT)))

void MAD::InitBypass() {
#include "mplad_bypass_define.def"
}

bool MAD::IsSlot0Free() const {
  if (GetUnitByUnitId(kUnitIdSlot0)->IsFree(0)) {
    return false;
  }
  return true;
}

bool MAD::IsFullIssued() const {
  if (GetUnitByUnitId(kUnitIdSlot0)->IsFree(0) || GetUnitByUnitId(kUnitIdSlot1)->IsFree(0)) {
    return false;
  }
  return true;
}

void MAD::RestoreStates(std::vector<uint32> &occupyTable, int size) {
  int i = 0;
  for (auto unit : allUnits) {
    CHECK_FATAL(i < size, "unit number error");
    unit->SetOccupancyTable(occupyTable[i]);
    ++i;
  }
}

bool Bypass::CanBypass(const Insn &defInsn, const Insn &useInsn) const {
  (void)defInsn;
  (void)useInsn;
  return true;
}

bool AluShiftBypass::CanBypass(const Insn &defInsn, const Insn &useInsn) const {
  /*
   * hook condition
   * true: r1=r2+x1 -> r3=r2<<0x2+r1
   * false:r1=r2+x1 -> r3=r1<<0x2+r2
   */
  return &(defInsn.GetOperand(kInsnFirstOpnd)) != &(useInsn.GetOperand(kInsnSecondOpnd));
}

bool AccumulatorBypass::CanBypass(const Insn &defInsn, const Insn &useInsn) const {
  /*
   * hook condition
   * true: r98=x0*x1 -> x0=x2*x3+r98
   * false:r98=x0*x1 -> x0=x2*r98+x3
   */
  return (&(defInsn.GetOperand(kInsnFirstOpnd)) != &(useInsn.GetOperand(kInsnSecondOpnd)) &&
          &(defInsn.GetOperand(kInsnFirstOpnd)) != &(useInsn.GetOperand(kInsnThirdOpnd)));
}

bool StoreBypass::CanBypass(const Insn &defInsn, const Insn &useInsn) const {
  /*
   * hook condition
   * true: r96=r92+x2 -> str r96, [r92]
   * false:r96=r92+x2 -> str r92, [r96]
   * false:r96=r92+x2 -> str r92, [r94, r96]
   */
  switch (useInsn.GetMachineOpcode()) {
    case MOP_wstrb:
    case MOP_wstrh:
    case MOP_wstr:
    case MOP_xstr:
    case MOP_sstr:
    case MOP_dstr: {
      auto &useMemOpnd = static_cast<AArch64MemOperand&>(useInsn.GetOperand(kInsnSecondOpnd));
      return (&(defInsn.GetOperand(kInsnFirstOpnd)) != useMemOpnd.GetOffset() &&
              &(defInsn.GetOperand(kInsnFirstOpnd)) != useMemOpnd.GetBaseRegister());
    }
    case MOP_wstp:
    case MOP_xstp: {
      auto &useMemOpnd = static_cast<AArch64MemOperand&>(useInsn.GetOperand(kInsnThirdOpnd));
      return (&(defInsn.GetOperand(kInsnFirstOpnd)) != useMemOpnd.GetOffset() &&
              &(defInsn.GetOperand(kInsnFirstOpnd)) != useMemOpnd.GetBaseRegister());
    }

    default:
      return false;
  }
}

/* Reservation */
Reservation::Reservation(LatencyType t, int l, int n, ...) : type(t), latency(l), unitNum(n) {
  ASSERT(l >= 0, "CG internal error, latency and unitNum should not be less than 0.");
  ASSERT(n >= 0, "CG internal error, latency and unitNum should not be less than 0.");

  errno_t ret = memset_s(units, sizeof(Unit*) * kMaxUnit, 0, sizeof(Unit*) * kMaxUnit);
  CHECK_FATAL(ret == EOK, "call memset_s failed in Reservation");

  va_list ap;
  va_start(ap, n);
  for (uint32 i = 0; i < unitNum; ++i) {
    units[i] = static_cast<Unit*>(va_arg(ap, Unit*));
  }
  va_end(ap);

  MAD::AddReservation(*this);
  /* init slot */
  if (n > 0) {
    /* if there are units, init slot by units[0] */
    slot = GetSlotType(units[0]->GetUnitId());
  } else {
    slot = kSlotNone;
  }
}

const std::string kSlotName[] = {
  "SlotNone",
  "Slot0",
  "Slot1",
  "SlotAny",
  "Slots",
};

const std::string &Reservation::GetSlotName() const {
  ASSERT(GetSlot() <= kSlots, "Unexpected slot");
  return kUnitName[GetSlot()];
}

/* Get slot type by unit id */
SlotType Reservation::GetSlotType(UnitId unitID) const {
  switch (unitID) {
    case kUnitIdSlot0:
    case kUnitIdSlot0LdAgu:
    case kUnitIdSlot0StAgu:
      return kSlot0;

    case kUnitIdSlot1:
      return kSlot1;

    case kUnitIdSlotS:
    case kUnitIdSlotSHazard:
    case kUnitIdSlotSMul:
    case kUnitIdSlotSBranch:
    case kUnitIdSlotSAgen:
      return kSlotAny;

    case kUnitIdSlotD:
    case kUnitIdSlotDAgen:
      return kSlots;

    default:
      ASSERT(false, "unknown slot type!");
      return kSlotNone;
  }
}
}  /* namespace maplebe */
