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
#ifndef MAPLE_IR_INCLUDE_INTRINSICS_H
#define MAPLE_IR_INCLUDE_INTRINSICS_H
#include "prim_types.h"
#include "intrinsic_op.h"

namespace maple {
enum IntrinProperty {
  kIntrnUndef,
  kIntrnIsJs,
  kIntrnIsJsUnary,
  kIntrnIsJsBinary,
  kIntrnIsJava,
  kIntrnIsJavaUnary,
  kIntrnIsJavaBinary,
  kIntrnIsReturnStruct,
  kIntrnNoSideEffect,
  kIntrnIsLoadMem,
  kIntrnIsPure,
  kIntrnNeverReturn,
  kIntrnIsAtomic,
  kIntrnIsRC,
  kIntrnIsSpecial
};

enum IntrinArgType {
  kArgTyUndef,
  kArgTyVoid,
  kArgTyI8,
  kArgTyI16,
  kArgTyI32,
  kArgTyI64,
  kArgTyU8,
  kArgTyU16,
  kArgTyU32,
  kArgTyU64,
  kArgTyU1,
  kArgTyPtr,
  kArgTyRef,
  kArgTyA32,
  kArgTyA64,
  kArgTyF32,
  kArgTyF64,
  kArgTyF128,
  kArgTyC64,
  kArgTyC128,
  kArgTyAgg,
#ifdef DYNAMICLANG
  kArgTyDynany,
  kArgTyDynu32,
  kArgTyDyni32,
  kArgTyDynundef,
  kArgTyDynnull,
  kArgTyDynhole,
  kArgTyDynbool,
  kArgTyDynf64,
  kArgTyDynf32,
  kArgTySimplestr,
  kArgTyDynstr,
  kArgTySimpleobj,
  kArgTyDynobj
#endif
};

constexpr uint32 INTRNISJS = 1U << kIntrnIsJs;
constexpr uint32 INTRNISJSUNARY = 1U << kIntrnIsJsUnary;
constexpr uint32 INTRNISJSBINARY = 1U << kIntrnIsJsBinary;
constexpr uint32 INTRNISJAVA = 1U << kIntrnIsJava;
constexpr uint32 INTRNNOSIDEEFFECT = 1U << kIntrnNoSideEffect;
constexpr uint32 INTRNRETURNSTRUCT = 1U << kIntrnIsReturnStruct;
constexpr uint32 INTRNLOADMEM = 1U << kIntrnIsLoadMem;
constexpr uint32 INTRNISPURE = 1U << kIntrnIsPure;
constexpr uint32 INTRNNEVERRETURN = 1U << kIntrnNeverReturn;
constexpr uint32 INTRNATOMIC = 1U << kIntrnIsAtomic;
constexpr uint32 INTRNISRC = 1U << kIntrnIsRC;
constexpr uint32 INTRNISSPECIAL = 1U << kIntrnIsSpecial;
class MIRType;    // circular dependency exists, no other choice
class MIRModule;  // circular dependency exists, no other choice
struct IntrinDesc {
  static constexpr int kMaxArgsNum = 7;
  const char *name;
  uint32 properties;
  IntrinArgType argTypes[1 + kMaxArgsNum];  // argTypes[0] is the return type
  bool IsJS() const {
    return static_cast<bool>(properties & INTRNISJS);
  }

  bool IsJava() const {
    return static_cast<bool>(properties & INTRNISJAVA);
  }

  bool IsJsUnary() const {
    return static_cast<bool>(properties & INTRNISJSUNARY);
  }

  bool IsJsBinary() const {
    return static_cast<bool>(properties & INTRNISJSBINARY);
  }

  bool IsJsOp() const {
    return static_cast<bool>(properties & INTRNISJSUNARY) || static_cast<bool>(properties & INTRNISJSBINARY);
  }

  bool IsLoadMem() const {
    return static_cast<bool>(properties & INTRNLOADMEM);
  }

  bool IsJsReturnStruct() const {
    return static_cast<bool>(properties & INTRNRETURNSTRUCT);
  }

  bool IsPure() const {
    return static_cast<bool>(properties & INTRNISPURE);
  }

  bool IsNeverReturn() const {
    return static_cast<bool>(properties & INTRNNEVERRETURN);
  }

  bool IsAtomic() const {
    return static_cast<bool>(properties & INTRNATOMIC);
  }

  bool IsRC() const {
    return static_cast<bool>(properties & INTRNISRC);
  }

  bool IsSpecial() const {
    return static_cast<bool>(properties & INTRNISSPECIAL);
  }

  bool HasNoSideEffect() const {
    return properties & INTRNNOSIDEEFFECT;
  }

  MIRType *GetReturnType() const;
  MIRType *GetArgType(uint32 index) const;
  MIRType *GetTypeFromArgTy(IntrinArgType argType) const;
  static MIRType *jsValueType;
  static MIRModule *mirModule;
  static void InitMIRModule(MIRModule *mirModule);
  static MIRType *GetOrCreateJSValueType();
  static IntrinDesc intrinTable[INTRN_LAST + 1];
};
}  // namespace maple
#endif  // MAPLE_IR_INCLUDE_INTRINSICS_H
