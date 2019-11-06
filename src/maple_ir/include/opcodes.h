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
#ifndef MAPLE_IR_INCLUDE_OPCODES_H
#define MAPLE_IR_INCLUDE_OPCODES_H

namespace maple {
enum Opcode : std::uint8_t {
  kOpUndef,
#define OPCODE(STR, YY, ZZ, SS) OP_##STR,
#include "opcodes.def"
#undef OPCODE
  kOpLast,
};

inline constexpr bool IsDAssign(Opcode code) {
  return (code == OP_dassign || code == OP_maydassign);
}

inline constexpr bool IsCallAssigned(Opcode code) {
  return (code == OP_callassigned || code == OP_virtualcallassigned ||
          code == OP_virtualicallassigned || code == OP_superclasscallassigned ||
          code == OP_interfacecallassigned || code == OP_interfaceicallassigned ||
          code == OP_customcallassigned || code == OP_polymorphiccallassigned ||
          code == OP_icallassigned || code == OP_intrinsiccallassigned ||
          code == OP_xintrinsiccallassigned || code == OP_intrinsiccallwithtypeassigned);
  }

}  // namespace maple
#endif  // MAPLE_IR_INCLUDE_OPCODES_H
