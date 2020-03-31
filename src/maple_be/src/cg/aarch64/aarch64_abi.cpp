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
#include "aarch64_cgfunc.h"
#include "becommon.h"

namespace maplebe {
using namespace maple;

namespace {
constexpr int kMaxRegCount = 2;

/*
 * return the class resulted from merging the two classes, based on rules
 * described by the ARM ABI
 */
AArch64ArgumentClass MergeClasses(AArch64ArgumentClass class0, AArch64ArgumentClass class1) {
  /*
   * maybe return ( class0 | class 1 ) would do if
   * ( class0 != kAArch64MemoryClass && class1 != kAArch64MemoryClass ) always hold
   */
  if (class0 == class1) {
    return class0;
  }
  if (class0 == kAArch64NoClass) {
    return class1;
  }
  if (class1 == kAArch64NoClass) {
    return class0;
  }
  if ((class0 == kAArch64MemoryClass) || (class1 == kAArch64MemoryClass)) {
    return kAArch64MemoryClass;
  }
  if ((class0 == kAArch64IntegerClass) || (class1 == kAArch64IntegerClass)) {
    return kAArch64IntegerClass;
  }
  ASSERT(false, "NYI");
  return kAArch64NoClass;
}

int32 ProcessNonStructAndNonArrayWhenClassifyAggregate(const MIRType &mirType,
                                                       AArch64ArgumentClass classes[kMaxRegCount],
                                                       size_t classesLength) {
  CHECK_FATAL(classesLength > 0, "classLength must > 0");
  /* scalar type */
  switch (mirType.GetPrimType()) {
    case PTY_u1:
    case PTY_u8:
    case PTY_i8:
    case PTY_u16:
    case PTY_i16:
    case PTY_a32:
    case PTY_u32:
    case PTY_i32:
    case PTY_a64:
    case PTY_ptr:
    case PTY_ref:
    case PTY_u64:
    case PTY_i64:
      classes[0] = kAArch64IntegerClass;
      return 1;
    case PTY_f32:
    case PTY_f64:
    case PTY_c64:
    case PTY_c128:
      classes[0] = kAArch64FloatClass;
      return 1;
    default:
      CHECK_FATAL(false, "NYI");
  }

  /* should not reach to this point */
  return 0;
}

void ProcessNonUnionWhenClassifyAggregate(const BECommon &be, const MIRType &fieldType, uint32 &fldBofst,
                                          uint64 &allocedSize, uint64 &allocedSizeInBits) {
  /* determine fld_bofst for this field */
  uint64 fieldTypeSize = be.GetTypeSize(fieldType.GetTypeIndex());
  ASSERT(fieldTypeSize != 0, "fieldTypeSize should not be 0");
  uint8 fieldAlign = be.GetTypeAlign(fieldType.GetTypeIndex());
  ASSERT(fieldAlign != 0, "fieldAlign should not be 0");
  if (fieldType.GetKind() == kTypeBitField) {
    uint32 fieldSize = static_cast<const MIRBitFieldType&>(fieldType).GetFieldSize();
    if ((allocedSizeInBits / (fieldAlign * k8ByteSize)) !=
        ((allocedSizeInBits + fieldSize - 1u) / (fieldAlign * k8ByteSize))) {
      /*
       * the field is crossing the align boundary of its base type;
       * align alloced_size_in_bits to fieldAlign
       */
      allocedSizeInBits = RoundUp(allocedSizeInBits, fieldAlign * k8ByteSize);
    }
    /* allocate the bitfield */
    fldBofst = allocedSizeInBits;
    allocedSizeInBits += fieldSize;
    allocedSize = std::max(allocedSize, RoundUp(allocedSizeInBits, fieldAlign * k8ByteSize) / k8ByteSize);
  } else {
    /* pad alloced_size according to the field alignment */
    allocedSize = RoundUp(allocedSize, fieldAlign);
    fldBofst = allocedSize * k8ByteSize;
    allocedSize += fieldTypeSize;
    allocedSizeInBits = allocedSize * k8ByteSize;
  }
}

int32 ClassifyAggregate(BECommon &be, MIRType &mirType, AArch64ArgumentClass classes[kMaxRegCount],
                        size_t classesLength);

void ProcessStructWhenClassifyAggregate(BECommon &be, MIRStructType &structType, int32 &subNumRegs,
                                        AArch64ArgumentClass classes[kMaxRegCount],
                                        size_t classesLength) {
  CHECK_FATAL(classesLength > 0, "classLength must > 0");
  int32 sizeOfTyInDwords = RoundUp(be.GetTypeSize(structType.GetTypeIndex()), k8ByteSize) >> k8BitShift;
  AArch64ArgumentClass subClasses[kMaxRegCount];
  uint32 fldBofst = 0;  /* offset of field in bits within immediate struct */
  uint64 allocedSize = 0;
  uint64 allocedSizeInBits = 0;
  for (uint32 f = 0; f < structType.GetFieldsSize(); ++f) {
    TyIdx fieldTyIdx = structType.GetFieldsElemt(f).second.first;
    MIRType *fieldType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(fieldTyIdx);
    subNumRegs = ClassifyAggregate(be, *fieldType, subClasses, sizeof(subClasses) / sizeof(AArch64ArgumentClass));
    ASSERT(subNumRegs > 0, "expect subNumRegs > 0");  /* we come here when the total size < 16? */
    if (subNumRegs == 0) {
      return;
    }
    if (structType.GetKind() != kTypeUnion) {
      ProcessNonUnionWhenClassifyAggregate(be, (*fieldType), fldBofst, allocedSize, allocedSizeInBits);
    } else {
      /*
       * for unions, bitfields are treated as non-bitfields
       * the parent aggregate is union, why are we increasing the alloced_size?
       * this would alter the next field's bit offset?
       */
      uint64 fieldTypeSize = be.GetTypeSize(fieldType->GetTypeIndex());
      ASSERT(fieldTypeSize != 0, "fieldTypeSize should not be 0");
      fldBofst = allocedSize * k8ByteSize;
      allocedSize = std::max(allocedSize, fieldTypeSize);
    }
    /* merge subClasses into classes */
    int32 idx = fldBofst >> 6;  /* index into the struct in doublewords */
    ASSERT(idx > 0, "expect idx > 0");
    ASSERT(idx < kMaxRegCount, "expect idx < kMaxRegCount");
    ASSERT(subNumRegs == 1, "subNumRegs should be equal to 1");
    ASSERT(subClasses[0] != kAArch64MemoryClass, "expect a kAArch64MemoryClass");
    for (int32 i = 0; i < subNumRegs; ++i) {
      classes[i + idx] = MergeClasses(classes[i + idx], subClasses[i]);
    }
  }
  if (subNumRegs < sizeOfTyInDwords) {
    for (int32 i = 1; i < sizeOfTyInDwords; ++i) {
      if (classes[i] == kAArch64NoClass) {
        classes[i] = classes[i - 1];
      }
    }
  }
}

void ProcessArrayWhenClassifyAggregate(BECommon &be, const MIRArrayType &mirArrayType, int32 &subNumRegs,
                                       AArch64ArgumentClass classes[kMaxRegCount], size_t classesLength) {
  CHECK_FATAL(classesLength > 0, "classLength must > 0");
  int32 sizeOfTyInDwords = RoundUp(be.GetTypeSize(mirArrayType.GetTypeIndex()), k8ByteSize) >> k8BitShift;
  AArch64ArgumentClass subClasses[kMaxRegCount];
  subNumRegs = ClassifyAggregate(be, *(GlobalTables::GetTypeTable().GetTypeFromTyIdx(mirArrayType.GetElemTyIdx())),
                                 subClasses, sizeof(subClasses) / sizeof(AArch64ArgumentClass));
  CHECK_FATAL(subNumRegs == 1, "subnumregs should be equal to 1");
  for (int32 i = 0; i < sizeOfTyInDwords; ++i) {
    classes[i] = subClasses[i];
  }
}

/*
 * Analyze the given aggregate using the rules given by the ARM 64-bit ABI and
 * return the number of doublewords to be passed in registers; the classes of
 * the doublewords are returned in parameter "classes"; if 0 is returned, it
 * means the whole aggregate is passed in memory.
 */
int32 ClassifyAggregate(BECommon &be, MIRType &mirType, AArch64ArgumentClass classes[kMaxRegCount],
                        size_t classesLength) {
  CHECK_FATAL(classesLength > 0, "invalid index");
  uint64 sizeOfTy = be.GetTypeSize(mirType.GetTypeIndex());
  /* Rule B.3.
   * If the argument type is a Composite Type that is larger than 16 bytes
   * then the argument is copied to memory allocated by the caller and
   * the argument is replaced by a pointer to the copy.
   */
  if ((sizeOfTy > k16ByteSize) || (sizeOfTy == 0)) {
    return 0;
  }

  /*
   * An argument of any Integer class takes up an integer register
   * which is a single double-word.
   * Rule B.4. The size of an argument of composite type is rounded up to the nearest
   * multiple of 8 bytes.
   */
  int32 sizeOfTyInDwords = RoundUp(sizeOfTy, k8ByteSize) >> k8BitShift;
  ASSERT(sizeOfTyInDwords > 0, "sizeOfTyInDwords should be sizeOfTyInDwords > 0");
  ASSERT(sizeOfTyInDwords <= kMaxRegCount, "sizeOfTyInDwords should be sizeOfTyInDwords <= kMaxRegCount");
  int32 i;
  for (i = 0; i < sizeOfTyInDwords; ++i) {
    classes[i] = kAArch64NoClass;
  }
  if ((mirType.GetKind() != kTypeStruct) && (mirType.GetKind() != kTypeArray)) {
    return ProcessNonStructAndNonArrayWhenClassifyAggregate(mirType, classes, classesLength);
  }
  int32 subNumRegs;
  if (mirType.GetKind() == kTypeStruct) {
    MIRStructType &structType = static_cast<MIRStructType&>(mirType);
    ProcessStructWhenClassifyAggregate(be, structType, subNumRegs, classes, classesLength);
    if (subNumRegs == 0) {
      return 0;
    }
  } else {
    /* mirType->_kind == TYPE_ARRAY */
    auto &mirArrayType = static_cast<MIRArrayType&>(mirType);
    ProcessArrayWhenClassifyAggregate(be, mirArrayType, subNumRegs, classes, classesLength);
  }
  /* post merger clean-up */
  for (i = 0; i < sizeOfTyInDwords; ++i) {
    if (classes[i] == kAArch64MemoryClass) {
      return 0;
    }
  }
  return sizeOfTyInDwords;
}
}

namespace AArch64Abi {
bool IsAvailableReg(AArch64reg reg) {
  switch (reg) {
/* integer registers */
#define INT_REG(ID, PREF32, PREF64, canBeAssigned, isCalleeSave, isParam, isSpill, isExtraSpill) \
    case R##ID:                                                                                  \
      return canBeAssigned;
#define INT_REG_ALIAS(ALIAS, ID, PREF32, PREF64)
#include "aarch64_int_regs.def"
#undef INT_REG
#undef INT_REG_ALIAS
/* fp-simd registers */
#define FP_SIMD_REG(ID, PV, P8, P16, P32, P64, P128, canBeAssigned, isCalleeSave, isParam, isSpill, isExtraSpill) \
    case V##ID:                                                                                                   \
      return canBeAssigned;
#define FP_SIMD_REG_ALIAS(ID)
#include "aarch64_fp_simd_regs.def"
#undef FP_SIMD_REG
#undef FP_SIMD_REG_ALIAS
    default:
      return false;
  }
}

bool IsCalleeSavedReg(AArch64reg reg) {
  switch (reg) {
/* integer registers */
#define INT_REG(ID, PREF32, PREF64, canBeAssigned, isCalleeSave, isParam, isSpill, isExtraSpill) \
    case R##ID:                                                                                  \
      return isCalleeSave;
#define INT_REG_ALIAS(ALIAS, ID, PREF32, PREF64)
#include "aarch64_int_regs.def"
#undef INT_REG
#undef INT_REG_ALIAS
/* fp-simd registers */
#define FP_SIMD_REG(ID, PV, P8, P16, P32, P64, P128, canBeAssigned, isCalleeSave, isParam, isSpill, isExtraSpill) \
    case V##ID:                                                                                                   \
      return isCalleeSave;
#define FP_SIMD_REG_ALIAS(ID)
#include "aarch64_fp_simd_regs.def"
#undef FP_SIMD_REG
#undef FP_SIMD_REG_ALIAS
    default:
      return false;
  }
}

bool IsParamReg(AArch64reg reg) {
  switch (reg) {
/* integer registers */
#define INT_REG(ID, PREF32, PREF64, canBeAssigned, isCalleeSave, isParam, isSpill, isExtraSpill) \
    case R##ID:                                                                                  \
      return isParam;
#define INT_REG_ALIAS(ALIAS, ID, PREF32, PREF64)
#include "aarch64_int_regs.def"
#undef INT_REG
#undef INT_REG_ALIAS
/* fp-simd registers */
#define FP_SIMD_REG(ID, PV, P8, P16, P32, P64, P128, canBeAssigned, isCalleeSave, isParam, isSpill, isExtraSpill) \
    case V##ID:                                                                                                   \
      return isParam;
#define FP_SIMD_REG_ALIAS(ID)
#include "aarch64_fp_simd_regs.def"
#undef FP_SIMD_REG
#undef FP_SIMD_REG_ALIAS
    default:
      return false;
  }
}

bool IsSpillReg(AArch64reg reg) {
  switch (reg) {
/* integer registers */
#define INT_REG(ID, PREF32, PREF64, canBeAssigned, isCalleeSave, isParam, isSpill, isExtraSpill) \
    case R##ID:                                                                                  \
      return isSpill;
#define INT_REG_ALIAS(ALIAS, ID, PREF32, PREF64)
#include "aarch64_int_regs.def"
#undef INT_REG
#undef INT_REG_ALIAS
/* fp-simd registers */
#define FP_SIMD_REG(ID, PV, P8, P16, P32, P64, P128, canBeAssigned, isCalleeSave, isParam, isSpill, isExtraSpill) \
    case V##ID:                                                                                                   \
      return isSpill;
#define FP_SIMD_REG_ALIAS(ID)
#include "aarch64_fp_simd_regs.def"
#undef FP_SIMD_REG
#undef FP_SIMD_REG_ALIAS
    default:
      return false;
  }
}

bool IsExtraSpillReg(AArch64reg reg) {
  switch (reg) {
/* integer registers */
#define INT_REG(ID, PREF32, PREF64, canBeAssigned, isCalleeSave, isParam, isSpill, isExtraSpill) \
    case R##ID:                                                                                  \
      return isExtraSpill;
#define INT_REG_ALIAS(ALIAS, ID, PREF32, PREF64)
#include "aarch64_int_regs.def"
#undef INT_REG
#undef INT_REG_ALIAS
/* fp-simd registers */
#define FP_SIMD_REG(ID, PV, P8, P16, P32, P64, P128, canBeAssigned, isCalleeSave, isParam, isSpill, isExtraSpill) \
    case V##ID:                                                                                                   \
      return isExtraSpill;
#define FP_SIMD_REG_ALIAS(ID)
#include "aarch64_fp_simd_regs.def"
#undef FP_SIMD_REG
#undef FP_SIMD_REG_ALIAS
    default:
      return false;
  }
}

bool IsSpillRegInRA(AArch64reg regNO, bool has3RegOpnd) {
  /* if has 3 RegOpnd, previous reg used to spill. */
  if (has3RegOpnd) {
    return AArch64Abi::IsSpillReg(regNO) || AArch64Abi::IsExtraSpillReg(regNO);
  }
  return AArch64Abi::IsSpillReg(regNO);
}
}  /* namespace AArch64Abi */

/*
 * Refer to ARM IHI 0055C_beta: Procedure Call Standard for
 * the ARM 64-bit Architecture. $5.4.2
 *
 * For internal only functions, we may want to implement
 * our own rules as Apple IOS has done. Maybe we want to
 * generate two versions for each of externally visible functions,
 * one conforming to the ARM standard ABI, and the other for
 * internal only use.
 *
 * LocateNextParm should be called with each parameter in the parameter list
 * starting from the beginning, one call per parameter in sequence; it returns
 * the information on how each parameter is passed in pLoc
 */
void ParmLocator::LocateNextParm(MIRType &mirType, PLocInfo &pLoc) {
  uint64 typeSize = beCommon.GetTypeSize(mirType.GetTypeIndex());
  int32 typeAlign = beCommon.GetTypeAlign(mirType.GetTypeIndex());
  pLoc.reg0 = kRinvalid;
  pLoc.reg1 = kRinvalid;
  pLoc.memOffset = nextStackArgAdress;
  /*
   * Rule C.12 states that we do round nextStackArgAdress up before we use its value
   * according to the alignment requirement of the argument being processed.
   * We do the rounding up at the end of LocateNextParm(),
   * so we want to make sure our rounding up is correct.
   */
  ASSERT((nextStackArgAdress & (std::max(typeAlign, static_cast<int32>(k8ByteSize)) - 1)) == 0,
         "C.12 alignment requirement is violated");
  pLoc.memSize = static_cast<int32>(typeSize);
  ++paramNum;

  switch (mirType.GetPrimType()) {
    case PTY_u1:
    case PTY_u8:
    case PTY_i8:
    case PTY_u16:
    case PTY_i16:
    case PTY_a32:
    case PTY_u32:
    case PTY_i32:
    case PTY_ptr:
    case PTY_ref:
    case PTY_a64:
    case PTY_u64:
    case PTY_i64:
      /* Rule C.7 */
      typeSize = k8ByteSize;
      pLoc.reg0 = AllocateGPRegister();
      ASSERT(nextGeneralRegNO <= AArch64Abi::kNumIntParmRegs, "RegNo should be pramRegNO");
      break;
    /*
     * for c64 complex numbers, we assume
     * - callers marshall the two f32 numbers into one f64 register
     * - callees de-marshall one f64 value into the real and the imaginery part
     */
    case PTY_f32:
    case PTY_f64:
    case PTY_c64:
      /* Rule C.1 */
      ASSERT(GetPrimTypeSize(PTY_f64) == k8ByteSize, "unexpected type size");
      typeSize = k8ByteSize;
      pLoc.reg0 = AllocateSIMDFPRegister();
      break;
    /*
     * for c128 complex numbers, we assume
     * - callers marshall the two f64 numbers into one f128 register
     * - callees de-marshall one f128 value into the real and the imaginery part
     */
    case PTY_c128:
      /* SIMD-FP registers have 128-bits. */
      pLoc.reg0 = AllocateSIMDFPRegister();
      ASSERT(nextFloatRegNO <= AArch64Abi::kNumFloatParmRegs, "regNO should not be greater than kNumFloatParmRegs");
      ASSERT(typeSize == k16ByteSize, "unexpected type size");
      break;
    /*
     * case of quad-word integer:
     * we don't support java yet.
     * if (has-16-byte-alignment-requirement)
     * nextGeneralRegNO = (nextGeneralRegNO+1) & ~1; // C.8 round it up to the next even number
     * try allocate two consecutive registers at once.
     */
    /* case PTY_agg */
    case PTY_agg: {
      ProcessPtyAggWhenLocateNextParm(mirType, pLoc, typeSize, typeAlign);
      break;
    }
    default:
      CHECK_FATAL(false, "NYI");
  }

  /* Rule C.12 */
  if (pLoc.reg0 == kRinvalid) {
    /* being passed in memory */
    nextStackArgAdress = pLoc.memOffset + typeSize;
  }
}

void ParmLocator::ProcessPtyAggWhenLocateNextParm(MIRType &mirType, PLocInfo &pLoc, uint64 &typeSize,
                                                  int32 typeAlign) {
  /*
   * In AArch64, integer-float or float-integer
   * argument passing is not allowed. All should go through
   * integer-integer.
   */
  AArch64ArgumentClass classes[kMaxRegCount] = { kAArch64NoClass };
#if DEBUG
  int32 saveIntParmNum = nextGeneralRegNO;
  int32 saveFloatParmNum = nextFloatRegNO;
#endif
  /*
   * alignment requirement
   * Note. This is one of a few things iOS diverges from
   * the ARM 64-bit standard. They don't observe the round-up requirement.
   */
  if (typeAlign == k16ByteSize) {
    RoundNGRNUpToNextEven();
  }

  int32 numRegs = ClassifyAggregate(beCommon, mirType, classes, sizeof(classes) / sizeof(AArch64ArgumentClass));
  if (numRegs == 1) {
    /* passing in registers */
    typeSize = k8ByteSize;
    if (classes[0] == kAArch64FloatClass) {
      pLoc.reg0 = AllocateSIMDFPRegister();
      ASSERT(nextFloatRegNO == saveFloatParmNum, "RegNo should be saved pramRegNO");
    } else {
      pLoc.reg0 = AllocateGPRegister();
      ASSERT(nextGeneralRegNO == saveIntParmNum, "RegNo should be saved pramRegNO");
      /* Rule C.11 */
      ASSERT((pLoc.reg0 != kRinvalid) || (nextGeneralRegNO == AArch64Abi::kNumIntParmRegs),
             "reg0 should not be kRinvalid or nextGeneralRegNO should equal kNumIntParmRegs");
    }
  } else if (numRegs == kMaxRegCount) {
    ASSERT(classes[0] == kAArch64IntegerClass, "class 0 must be integer class");
    ASSERT(classes[1] == kAArch64IntegerClass, "class 1 must be integer class");
    AllocateTwoGPRegisters(pLoc);
    /* Rule C.11 */
    if (pLoc.reg0 == kRinvalid) {
      nextGeneralRegNO = AArch64Abi::kNumIntParmRegs;
    }
    ASSERT(AArch64Abi::kNumIntParmRegs == 8, "AArch64Abi::kNumIntParmRegs should be set to 8");
  } else {
    /*
     * 0 returned from ClassifyAggregate(). This means the whole data
     * is passed thru memory.
     * Rule B.3.
     *  If the argument type is a Composite Type that is larger than 16
     *  bytes then the argument is copied to memory allocated by the
     *  caller and the argument is replaced by a pointer to the copy.
     *
     * Try to allocate an integer register
     */
    typeSize = k8ByteSize;
    pLoc.reg0 = AllocateGPRegister();
    pLoc.memSize = k8ByteSize;  /* byte size of a pointer in AArch64 */
    if (pLoc.reg0 != kRinvalid) {
      numRegs = 1;
    }
  }
  /* compute rightpad */
  if ((numRegs == 0) || (pLoc.reg0 == kRinvalid)) {
    /* passed in memory */
    typeSize = RoundUp(pLoc.memSize, k8ByteSize);
  }
}

/*
 * instantiated with the type of the function return value, it describes how
 * the return value is to be passed back to the caller
 *
 *  Refer to ARM IHI 0055C_beta: Procedure Call Standard for
 *  the ARM 64-bit Architecture. $5.5
 *  "If the type, T, of the result of a function is such that
 *     void func(T arg)
 *   would require that 'arg' be passed as a value in a register
 *   (or set of registers) according to the rules in $5.4 Parameter
 *   Passing, then the result is returned in the same registers
 *   as would be used for such an argument.
 */
ReturnMechanism::ReturnMechanism(MIRType &retTy, BECommon &be)
    : regCount(0), reg0(kRinvalid), reg1(kRinvalid), primTypeOfReg0(kPtyInvalid), primTypeOfReg1(kPtyInvalid) {
  PrimType pType = retTy.GetPrimType();
  switch (pType) {
    case PTY_void:
      break;
    case PTY_u1:
    case PTY_u8:
    case PTY_i8:
    case PTY_u16:
    case PTY_i16:
    case PTY_a32:
    case PTY_u32:
    case PTY_i32:
      regCount = 1;
      reg0 = AArch64Abi::intReturnRegs[0];
      primTypeOfReg0 = IsSignedInteger(pType) ? PTY_i32 : PTY_u32;  /* promote the type */
      return;

    case PTY_ptr:
    case PTY_ref:
      CHECK_FATAL(false, "PTY_ptr should have been lowered");
      return;

    case PTY_a64:
    case PTY_u64:
    case PTY_i64:
      regCount = 1;
      reg0 = AArch64Abi::intReturnRegs[0];
      primTypeOfReg0 = IsSignedInteger(pType) ? PTY_i64 : PTY_u64;  /* promote the type */
      return;

    /*
     * for c64 complex numbers, we assume
     * - callers marshall the two f32 numbers into one f64 register
     * - callees de-marshall one f64 value into the real and the imaginery part
     */
    case PTY_f32:
    case PTY_f64:
    case PTY_c64:

    /*
     * for c128 complex numbers, we assume
     * - callers marshall the two f64 numbers into one f128 register
     * - callees de-marshall one f128 value into the real and the imaginery part
     */
    case PTY_c128:
      regCount = 1;
      reg0 = AArch64Abi::floatReturnRegs[0];
      primTypeOfReg0 = pType;
      return;

    /*
     * Refer to ARM IHI 0055C_beta: Procedure Call Standard for
     * the ARM 64-bit Architecture. $5.5
     * "Otherwise, the caller shall reserve a block of memory of
     * sufficient size and alignment to hold the result. The
     * address of the memory block shall be passed as an additional
     * argument to the function in x8. The callee may modify the
     * result memory block at any point during the execution of the
     * subroutine (there is no requirement for the callee to preserve
     * the value stored in x8)."
     */
    case PTY_agg: {
      uint64 size = be.GetTypeSize(retTy.GetTypeIndex());
      if ((size > k16ByteSize) || (size == 0)) {
        /*
         * The return value is returned via memory.
         * The address is in X8 and passed by the caller.
         */
        SetupToReturnThroughMemory();
        return;
      }
      AArch64ArgumentClass classes[kMaxRegCount];
      regCount = static_cast<uint8>(ClassifyAggregate(be, retTy, classes,
                                                      sizeof(classes) / sizeof(AArch64ArgumentClass)));
      if (regCount == 0) {
        SetupToReturnThroughMemory();
        return;
      } else {
        if (regCount == 1) {
          /* passing in registers */
          if (classes[0] == kAArch64FloatClass) {
            reg0 = AArch64Abi::floatReturnRegs[0];
            primTypeOfReg0 = PTY_f64;
          } else {
            reg0 = AArch64Abi::intReturnRegs[0];
            primTypeOfReg0 = PTY_i64;
          }
        } else {
          ASSERT(regCount == kMaxRegCount, "reg count from ClassifyAggregate() should be 0, 1, or 2");
          ASSERT(classes[0] == kAArch64IntegerClass, "error val :classes[0]");
          ASSERT(classes[1] == kAArch64IntegerClass, "error val :classes[1]");
          reg0 = AArch64Abi::intReturnRegs[0];
          primTypeOfReg0 = PTY_i64;
          reg1 = AArch64Abi::intReturnRegs[1];
          primTypeOfReg1 = PTY_i64;
        }
        return;
      }
    }
    default:
      CHECK_FATAL(false, "NYI");
  }
}

void ReturnMechanism::SetupSecondRetReg(const MIRType &retTy2) {
  ASSERT(reg1 == kRinvalid, "make sure reg1 equal kRinvalid");
  PrimType pType = retTy2.GetPrimType();
  switch (pType) {
    case PTY_void:
      break;
    case PTY_u1:
    case PTY_u8:
    case PTY_i8:
    case PTY_u16:
    case PTY_i16:
    case PTY_a32:
    case PTY_u32:
    case PTY_i32:
    case PTY_ptr:
    case PTY_ref:
    case PTY_a64:
    case PTY_u64:
    case PTY_i64:
      reg1 = AArch64Abi::intReturnRegs[1];
      primTypeOfReg1 = IsSignedInteger(pType) ? PTY_i64 : PTY_u64;  /* promote the type */
      break;
    default:
      CHECK_FATAL(false, "NYI");
  }
}

/*
 * From "ARM Procedure Call Standard for ARM 64-bit Architecture"
 *     ARM IHI 0055C_beta, 6th November 2013
 * $ 5.1 machine Registers
 * $ 5.1.1 General-Purpose Registers
 *  <Table 2>                Note
 *  SP       Stack Pointer
 *  R30/LR   Link register   Stores the return address.
 *                           We push it into stack along with FP on function
 *                           entry using STP and restore it on function exit
 *                           using LDP even if the function is a leaf (i.e.,
 *                           it does not call any other function) because it
 *                           is free (we have to store FP anyway).  So, if a
 *                           function is a leaf, we may use it as a temporary
 *                           register.
 *  R29/FP   Frame Pointer
 *  R19-R28  Callee-saved
 *           registers
 *  R18      Platform reg    Can we use it as a temporary register?
 *  R16,R17  IP0,IP1         Maybe used as temporary registers. Should be
 *                           given lower priorities. (i.e., we push them
 *                           into the free register stack before the others)
 *  R9-R15                   Temporary registers, caller-saved
 *  Note:
 *  R16 and R17 may be used by a linker as a scratch register between
 *  a routine and any subroutine it calls. They can also be used within a
 *  routine to hold intermediate values between subroutine calls.
 *
 *  The role of R18 is platform specific. If a platform ABI has need of
 *  a dedicated general purpose register to carry inter-procedural state
 *  (for example, the thread context) then it should use this register for
 *  that purpose. If the platform ABI has no such requirements, then it should
 *  use R18 as an additional temporary register. The platform ABI specification
 *  must document the usage for this register.
 *
 *  A subroutine invocation must preserve the contents of the registers R19-R29
 *  and SP. All 64 bits of each value stored in R19-R29 must be preserved, even
 *  when using the ILP32 data model.
 *
 *  $ 5.1.2 SIMD and Floating-Point Registers
 *
 *  The first eight registers, V0-V7, are used to pass argument values into
 *  a subroutine and to return result values from a function. They may also
 *  be used to hold intermediate values within a routine.
 *
 *  V8-V15 must be preserved by a callee across subroutine calls; the
 *  remaining registers do not need to be preserved( or caller-saved).
 *  Additionally, only the bottom 64 bits of each value stored in V8-
 *  V15 need to be preserved.
 */
}  /* namespace maplebe */
