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
#ifndef MAPLE_IR_INCLUDE_TYPES_DEF_H
#define MAPLE_IR_INCLUDE_TYPES_DEF_H

// NOTE: Since we already committed to -std=c++0x, we should eventually use the
// standard definitions in the <cstdint> and <limits> headers rather than
// reinventing our own primitive types.
#include <cstdint>
#include <cstddef>

namespace maple {
// Let's keep the following definitions so that existing code will continue to work.
using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;
using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;
class StIdx {  // scope nesting level + symbol table index
 public:
  union un {
    struct {
      uint32 idx;
      uint8 scope;  // scope level, with the global scope is at level 1
    } scopeIdx;

    uint32 fullIdx;
  };

  StIdx() {
    u.fullIdx = 0;
  }

  StIdx(uint32 level, uint32 i) {
    u.scopeIdx.scope = level;
    u.scopeIdx.idx = i;
  }

  ~StIdx() = default;

  uint32 Idx() const {
    return u.scopeIdx.idx;
  }

  void SetIdx(uint32 idx) {
    u.scopeIdx.idx = idx;
  }

  uint32 Scope() const {
    return u.scopeIdx.scope;
  }

  uint32 FullIdx() const {
    return u.fullIdx;
  }

  void SetFullIdx(uint32 idx) {
    u.fullIdx = idx;
  }

  bool Islocal() const {
    return u.scopeIdx.scope > 1;
  }

  bool IsGlobal() const {
    return u.scopeIdx.scope == 1;
  }

  bool operator==(const StIdx &x) const {
    return u.fullIdx == x.u.fullIdx;
  }

  bool operator!=(const StIdx &x) const {
    return !(*this == x);
  }

  bool operator<(const StIdx &x) const {
    return u.fullIdx < x.u.fullIdx;
  }

 private:
  un u;
};

using LabelIdx = uint32;
using LabelIDOrder = uint32;
using PUIdx = uint32;
using PregIdx = int32;
using PregIdx16 = int16;
using ExprIdx = int32;
using FieldID = int32;

// T is integer, category is Idx kind.
// this template is for instantiating some kinds of Idx Class such as TyIdx, GStrIdx, UStrIdx, U16StrIdx
template <typename T, typename Category>
class IdxTemplate {
 public:
  IdxTemplate() = default;
  explicit IdxTemplate(T i) : idx(i) {}
  IdxTemplate(const IdxTemplate&) = default;
  IdxTemplate &operator=(const IdxTemplate&) = default;
  ~IdxTemplate() = default;

  void operator=(T id) {
    idx = id;
  }
  bool operator==(const IdxTemplate &x) const {
    return idx == x.idx;
  }

  bool operator!=(const IdxTemplate &x) const {
    return !(*this == x);
  }

  bool operator==(T id) const {
    return idx == id;
  }

  bool operator!=(T id) const {
    return !(*this == id);
  }

  bool operator<(const IdxTemplate &x) const {
    return idx < x.idx;
  }

  T GetIdx() const {
    return idx;
  }

  void SetIdx(T i) {
    idx = i;
  }

 private:
  T idx = 0;
};

struct TyIdxCategory {};
struct GStrIdxCategory {};
struct UStrIdxCategory {};
struct U16StrIdxCategory {};

using TyIdx = IdxTemplate<uint32, TyIdxCategory>;          // global type table index
using GStrIdx = IdxTemplate<uint32, GStrIdxCategory>;      // global string table index
using UStrIdx = IdxTemplate<uint32, UStrIdxCategory>;      // user string table index (from the conststr opcode)
using U16StrIdx = IdxTemplate<uint32, U16StrIdxCategory>;  // user string table index (from the conststr opcode)

const TyIdx kInitTyIdx = TyIdx(0);
const TyIdx kNoneTyIdx = TyIdx(UINT32_MAX);

constexpr int kOperandNumUnary = 1;
constexpr int kOperandNumBinary = 2;
constexpr int kOperandNumTernary = 3;
}  // namespace maple
#endif  // MAPLE_IR_INCLUDE_TYPES_DEF_H
