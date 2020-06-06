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
#include "mir_const.h"
#include <iostream>
#include <iomanip>
#include "mir_function.h"
#include "global_tables.h"
#include "printing.h"
#if MIR_FEATURE_FULL

namespace maple {
void MIRConst::Dump() const {
  if (fieldID) {
    LogInfo::MapleLogger() << fieldID << "= ";
  }
}

void MIRIntConst::Dump() const {
  MIRConst::Dump();
  constexpr int64 valThreshold = 1024;
  if (value <= valThreshold) {
    LogInfo::MapleLogger() << value;
  } else {
    LogInfo::MapleLogger() << std::hex << "0x" << value << std::dec;
  }
}

bool MIRIntConst::operator==(const MIRConst &rhs) const {
  if (&rhs == this) {
    return true;
  }
  if (GetKind() != rhs.GetKind()) {
    return false;
  }
  const auto &intConst = static_cast<const MIRIntConst&>(rhs);
  return ((&intConst.GetType() == &GetType()) && (intConst.value == value));
}

uint8 MIRIntConst::GetBitWidth() const {
  if (value == 0) {
    return 1;
  }
  uint8 width = 0;
  uint64 tmp = value < 0 ? -(value + 1) : value;
  while (tmp != 0) {
    ++width;
    tmp = tmp >> 1u;
  }
  return width;
}

void MIRIntConst::Trunc(uint8 width) {
  const int32 shiftBitNum = static_cast<int32>(64u - width);
  if (shiftBitNum < 0) {
    CHECK_FATAL(false, "shiftBitNum should not be less than zero");
  }
  auto unsignShiftBitNum = static_cast<uint32>(shiftBitNum);
  if (IsSignedInteger(GetType().GetPrimType())) {
    value = (value << unsignShiftBitNum) >> unsignShiftBitNum;
  } else {
    value = ((static_cast<uint64>(value)) << unsignShiftBitNum) >> unsignShiftBitNum;
  }
}

int64 MIRIntConst::GetValueUnderType() const {
  uint32 bitSize = GetPrimTypeBitSize(GetNonDynType(GetType().GetPrimType()));
  const int32 shiftBitNum = static_cast<int32>(64u - bitSize);
  if (shiftBitNum < 0) {
    CHECK_FATAL(false, "shiftBitNum should not be less than zero");
  }
  if (IsSignedInteger(GetType().GetPrimType())) {
    return static_cast<int64>(((value) << shiftBitNum) >> shiftBitNum);
  }
  auto unsignedVal = static_cast<uint64>(value);
  return static_cast<int64>((unsignedVal << shiftBitNum) >> shiftBitNum);
}

void MIRAddrofConst::Dump() const {
  MIRConst::Dump();
  LogInfo::MapleLogger() << "addrof " << GetPrimTypeName(PTY_ptr);
  ASSERT(stIdx.IsGlobal(), "MIRAddrofConst can only point to a global symbol");
  MIRSymbol *sym = GlobalTables::GetGsymTable().GetSymbolFromStidx(stIdx.Idx());
  LogInfo::MapleLogger() << " $" << sym->GetName();
  if (fldID > 0) {
    LogInfo::MapleLogger() << " " << fldID;
  }
}

bool MIRAddrofConst::operator==(const MIRConst &rhs) const {
  if (&rhs == this) {
    return true;
  }
  if (GetKind() != rhs.GetKind()) {
    return false;
  }
  const auto &rhsA = static_cast<const MIRAddrofConst&>(rhs);
  if (&GetType() != &rhs.GetType()) {
    return false;
  }
  return (stIdx == rhsA.stIdx) && (fldID == rhsA.fldID);
}

void MIRAddroffuncConst::Dump() const {
  MIRConst::Dump();
  LogInfo::MapleLogger() << "addroffunc " << GetPrimTypeName(PTY_ptr);
  MIRFunction *func = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(puIdx);
  LogInfo::MapleLogger() << " &" << GlobalTables::GetGsymTable().GetSymbolFromStidx(func->GetStIdx().Idx())->GetName();
}

bool MIRAddroffuncConst::operator==(const MIRConst &rhs) const {
  if (&rhs == this) {
    return true;
  }
  if (GetKind() != rhs.GetKind()) {
    return false;
  }
  const auto &rhsAf = static_cast<const MIRAddroffuncConst&>(rhs);
  return (&GetType() == &rhs.GetType()) && (puIdx == rhsAf.puIdx);
}

bool MIRLblConst::operator==(const MIRConst &rhs) const {
  if (&rhs == this) {
    return true;
  }
  if (GetKind() != rhs.GetKind()) {
    return false;
  }
  const auto &lblConst = static_cast<const MIRLblConst&>(rhs);
  return (lblConst.value == value);
}

bool MIRFloatConst::operator==(const MIRConst &rhs) const {
  if (&rhs == this) {
    return true;
  }
  if (GetKind() != rhs.GetKind()) {
    return false;
  }
  const auto &floatConst = static_cast<const MIRFloatConst&>(rhs);
  if (std::isnan(floatConst.value.floatValue)) {
    return std::isnan(value.floatValue);
  }
  if (std::isnan(value.floatValue)) {
    return std::isnan(floatConst.value.floatValue);
  }
  return (fabs(floatConst.value.floatValue - value.floatValue) <= 1e-6);
}

bool MIRDoubleConst::operator==(const MIRConst &rhs) const {
  if (&rhs == this) {
    return true;
  }
  if (GetKind() != rhs.GetKind()) {
    return false;
  }
  const auto &floatConst = static_cast<const MIRDoubleConst&>(rhs);
  if (std::isnan(floatConst.value.dValue)) {
    return std::isnan(value.dValue);
  }
  if (std::isnan(value.dValue)) {
    return std::isnan(floatConst.value.dValue);
  }
  return (fabs(floatConst.value.dValue - value.dValue) <= 1e-15);
}

bool MIRFloat128Const::operator==(const MIRConst &rhs) const {
  if (&rhs == this) {
    return true;
  }
  if (GetKind() != rhs.GetKind()) {
    return false;
  }
  const auto &floatConst = static_cast<const MIRFloat128Const&>(rhs);
  if ((value[0] == floatConst.value[0]) && (value[1] == floatConst.value[1])) {
    return true;
  }
  return false;
}

bool MIRAggConst::operator==(const MIRConst &rhs) const {
  if (&rhs == this) {
    return true;
  }
  if (GetKind() != rhs.GetKind()) {
    return false;
  }
  const auto &aggregateConst = static_cast<const MIRAggConst&>(rhs);
  if (aggregateConst.constVec.size() != constVec.size()) {
    return false;
  }
  for (size_t i = 0; i < constVec.size(); ++i) {
    if (!(*aggregateConst.constVec[i] == *constVec[i])) {
      return false;
    }
  }
  return true;
}

void MIRFloatConst::Dump() const {
  MIRConst::Dump();
  LogInfo::MapleLogger() << std::setprecision(std::numeric_limits<float>::max_digits10) << value.floatValue << "f";
}

void MIRDoubleConst::Dump() const {
  MIRConst::Dump();
  LogInfo::MapleLogger() << std::setprecision(std::numeric_limits<double>::max_digits10) << value.dValue;
}

void MIRFloat128Const::Dump() const {
  constexpr int fieldWidth = 16;
  MIRConst::Dump();
  std::ios::fmtflags f(LogInfo::MapleLogger().flags());
  LogInfo::MapleLogger().setf(std::ios::uppercase);
  LogInfo::MapleLogger() << "0xL" << std::hex << std::setfill('0') << std::setw(fieldWidth) << value[0]
                         << std::setfill('0') << std::setw(fieldWidth) << value[1];
  LogInfo::MapleLogger().flags(f);
}

void MIRAggConst::Dump() const {
  MIRConst::Dump();
  LogInfo::MapleLogger() << "[";
  size_t size = constVec.size();
  for (size_t i = 0; i < size; ++i) {
    constVec[i]->Dump();
    if (i != size - 1) {
      LogInfo::MapleLogger() << ", ";
    }
  }
  LogInfo::MapleLogger() << "]";
}

MIRStrConst::MIRStrConst(const std::string &str, MIRType &type)
    : MIRConst(type, kConstStrConst), value(GlobalTables::GetUStrTable().GetOrCreateStrIdxFromName(str)) {}

void MIRStrConst::Dump() const {
  MIRConst::Dump();
  LogInfo::MapleLogger() << "conststr " << GetPrimTypeName(GetType().GetPrimType());
  const std::string &dumpStr = GlobalTables::GetUStrTable().GetStringFromStrIdx(value);
  PrintString(dumpStr);
}

bool MIRStrConst::operator==(const MIRConst &rhs) const {
  if (&rhs == this) {
    return true;
  }
  if (GetKind() != rhs.GetKind()) {
    return false;
  }
  const auto &rhsCs = static_cast<const MIRStrConst&>(rhs);
  return (&rhs.GetType() == &GetType()) && (value == rhsCs.value);
}

MIRStr16Const::MIRStr16Const(const std::u16string &str, MIRType &type)
    : MIRConst(type, kConstStr16Const), value(GlobalTables::GetU16StrTable().GetOrCreateStrIdxFromName(str)) {}

void MIRStr16Const::Dump() const {
  MIRConst::Dump();
  LogInfo::MapleLogger() << "conststr16 " << GetPrimTypeName(GetType().GetPrimType());
  std::u16string str16 = GlobalTables::GetU16StrTable().GetStringFromStrIdx(value);
  // UTF-16 string are dumped as UTF-8 string in mpl to keep the printable chars in ascii form
  std::string str;
  NameMangler::UTF16ToUTF8(str, str16);
  PrintString(str);
}

bool MIRStr16Const::operator==(const MIRConst &rhs) const {
  if (&rhs == this) {
    return true;
  }
  if (GetKind() != rhs.GetKind()) {
    return false;
  }
  const auto &rhsCs = static_cast<const MIRStr16Const&>(rhs);
  return (&GetType() == &rhs.GetType()) && (value == rhsCs.value);
}
}  // namespace maple
#endif  // MIR_FEATURE_FULL
