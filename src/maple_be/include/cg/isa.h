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
#ifndef MAPLEBE_INCLUDE_CG_ISA_H
#define MAPLEBE_INCLUDE_CG_ISA_H

#include <cstdint>
#include "types_def.h"

namespace maplebe {
enum RegType : maple::uint8 {
  kRegTyUndef,
  kRegTyInt,
  kRegTyFloat,
  kRegTyCc,
  kRegTyX87,
  kRegTyVary,
  kRegTyFpsc,
  kRegTyIndex,
  kRegTyLast,
};

enum MopProperty : maple::uint8 {
  kPropIsMove,
  kPropIsLoad,
  kPropIsLoadPair,
  kPropIsStore,
  kPropIsStorePair,
  kPropIsLoadAddress,
  kPropIsAtomic,
  kPropIsCall,
  kPropIsConversion,
  kPropIsConditionalSet,
  kPropUseSpecReg,
  kPropIsX86style,
  kPropIsCondDef,
  kPropHasAcqure,
  kPropHasAcqureRCpc,
  kPropHasLOAcqure,
  kPropHasRelease,
  kPropHasLORelease,
  kPropCanThrow,
  kPropIsPartDefine,
  kPropIsDMB,
  kPropIsUnCondBr,
  kPropIsCondBr,
  kPropHasLoop,
};

#define ISMOVE (1ULL << kPropIsMove)
#define ISLOAD (1ULL << kPropIsLoad)
#define ISLOADPAIR (1ULL << kPropIsLoadPair)
#define ISSTORE (1ULL << kPropIsStore)
#define ISSTOREPAIR (1ULL << kPropIsStorePair)
#define ISLOADADDR (1ULL << kPropIsLoadAddress)
#define ISATOMIC (1ULL << kPropIsAtomic)
#define ISCALL (1ULL << kPropIsCall)
#define ISCONVERSION (1ULL << kPropIsConversion)
#define ISCONDSET (1ULL << kPropIsConditionalSet)
#define USESPECREG (1ULL << kPropUseSpecReg)
#define ISX86STYLE (1ULL << kPropIsX86style)
#define ISCONDDEF (1ULL << kPropIsCondDef)
#define HASACQUIRE (1ULL << kPropHasAcqure)
#define HASACQUIRERCPC (1ULL << kPropHasAcqureRCpc)
#define HASLOACQUIRE (1ULL << kPropHasLOAcqure)
#define HASRELEASE (1ULL << kPropHasRelease)
#define HASLORELEASE (1ULL << kPropHasLORelease)
#define CANTHROW (1ULL << kPropCanThrow)
#define ISPARTDEF (1ULL << kPropIsPartDefine)
#define ISDMB (1ULL << kPropIsDMB)
#define ISUNCONDBRANCH (1ULL << kPropIsUnCondBr)
#define ISCONDBRANCH (1ULL << kPropIsCondBr)
#define HASLOOP (1ULL << kPropHasLoop)

using regno_t = uint32_t;

constexpr regno_t kInvalidRegNO = 0;

/*
 * ARM64 has 32 int registes and 32 FP registers.
 * AMD64/X86_64 has 16 int registes, and 16 FP registers.
 * In either case, the corresponding calling conventions use
 * the smaller number of caller-saved registers.
 * 64 bit is not large enough?
 */
using CsrBitset = uint64_t;

/* empty class; just for parameter passing */
class OpndProp {};
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_ISA_H */
