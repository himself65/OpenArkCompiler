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
#include <vector>
#include <cstdint>
#include <sys/stat.h>
#include "cfi.h"
#include "mpl_logging.h"
#include "aarch64_rt.h"
#include "opcode_info.h"
#include "mir_builder.h"
#include "mpl_atomic.h"
#include "metadata_layout.h"
#include "emit.h"

namespace maplebe {
using namespace maple;
CondOperand AArch64CGFunc::ccOperands[kCcLast] = {
#define CONDCODE(a) CondOperand(CC_##a),
#include "aarch64_cc.def"
#undef CONDCODE
};

namespace {
constexpr int32 kSignedDimension = 2;        /* signed and unsigned */
constexpr int32 kIntByteSizeDimension = 4;   /* 1 byte, 2 byte, 4 bytes, 8 bytes */
constexpr int32 kFloatByteSizeDimension = 2; /* 4 bytes, 8 bytes */
constexpr int32 kShiftAmount12 = 12;         /* for instruction that can use shift, shift amount must be 0 or 12 */

MOperator ldIs[kSignedDimension][kIntByteSizeDimension] = {
  /* unsigned == 0 */
  { MOP_wldrb, MOP_wldrh, MOP_wldr, MOP_xldr },
  /* signed == 1 */
  { MOP_wldrsb, MOP_wldrsh, MOP_wldr, MOP_xldr }
};

MOperator stIs[kSignedDimension][kIntByteSizeDimension] = {
  /* unsigned == 0 */
  { MOP_wstrb, MOP_wstrh, MOP_wstr, MOP_xstr },
  /* signed == 1 */
  { MOP_wstrb, MOP_wstrh, MOP_wstr, MOP_xstr }
};

MOperator ldIsAcq[kSignedDimension][kIntByteSizeDimension] = {
  /* unsigned == 0 */
  { MOP_wldarb, MOP_wldarh, MOP_wldar, MOP_xldar },
  /* signed == 1 */
  { MOP_undef, MOP_undef, MOP_wldar, MOP_xldar }
};

MOperator stIsRel[kSignedDimension][kIntByteSizeDimension] = {
  /* unsigned == 0 */
  { MOP_wstlrb, MOP_wstlrh, MOP_wstlr, MOP_xstlr },
  /* signed == 1 */
  { MOP_wstlrb, MOP_wstlrh, MOP_wstlr, MOP_xstlr }
};

MOperator ldFs[kFloatByteSizeDimension] = { MOP_sldr, MOP_dldr };
MOperator stFs[kFloatByteSizeDimension] = { MOP_sstr, MOP_dstr };

MOperator ldFsAcq[kFloatByteSizeDimension] = { MOP_undef, MOP_undef };
MOperator stFsRel[kFloatByteSizeDimension] = { MOP_undef, MOP_undef };

MOperator PickLdStInsn(bool isLoad, uint32 bitSize, PrimType primType, AArch64isa::MemoryOrdering memOrd) {
  ASSERT(__builtin_popcount(static_cast<uint32>(memOrd)) <= 1, "must be kMoNone or kMoAcquire");
  ASSERT(primType != PTY_ptr, "should have been lowered");
  ASSERT(primType != PTY_ref, "should have been lowered");
  ASSERT(bitSize >= k8BitSize, "PTY_u1 should have been lowered?");
  ASSERT(__builtin_popcount(bitSize) == 1, "PTY_u1 should have been lowered?");
  ASSERT((isLoad && ((memOrd == AArch64isa::kMoNone) || (memOrd == AArch64isa::kMoAcquire) ||
                    (memOrd == AArch64isa::kMoAcquireRcpc) || (memOrd == AArch64isa::kMoLoacquire))) ||
         (!isLoad && ((memOrd == AArch64isa::kMoNone) || (memOrd == AArch64isa::kMoRelease) ||
                     (memOrd == AArch64isa::kMoLorelease))),
         "unknown Memory Order");

  /* __builtin_ffs(x) returns: 0 -> 0, 1 -> 1, 2 -> 2, 4 -> 3, 8 -> 4 */
  if (IsPrimitiveInteger(primType)) {
    MOperator(*table)[kIntByteSizeDimension];
    if (isLoad) {
      table = (memOrd == AArch64isa::kMoAcquire) ? ldIsAcq : ldIs;
    } else {
      table = (memOrd == AArch64isa::kMoRelease) ? stIsRel : stIs;
    }

    int32 signedUnsigned = IsUnsignedInteger(primType) ? 0 : 1;
    /* __builtin_ffs(x) returns: 8 -> 4, 16 -> 5, 32 -> 6, 64 -> 7 */
    uint32 size = static_cast<uint32>(__builtin_ffs(static_cast<int32>(bitSize))) - 4;
    ASSERT(size <= 3, "wrong bitSize");
    return table[signedUnsigned][size];
  } else {
    MOperator *table = nullptr;
    if (isLoad) {
      table = (memOrd == AArch64isa::kMoAcquire) ? ldFsAcq : ldFs;
    } else {
      table = (memOrd == AArch64isa::kMoRelease) ? stFsRel : stFs;
    }

    /* __builtin_ffs(x) returns: 32 -> 6, 64 -> 7 */
    uint32 size = static_cast<uint32>(__builtin_ffs(static_cast<int32>(bitSize))) - 6;
    ASSERT(size <= 1, "size must be 0 or 1");
    return table[size];
  }
}
}

MOperator AArch64CGFunc::PickLdInsn(uint32 bitSize, PrimType primType, AArch64isa::MemoryOrdering memOrd) {
  return PickLdStInsn(true, bitSize, primType, memOrd);
}

MOperator AArch64CGFunc::PickStInsn(uint32 bitSize, PrimType primType, AArch64isa::MemoryOrdering memOrd) {
  return PickLdStInsn(false, bitSize, primType, memOrd);
}

MOperator AArch64CGFunc::PickMovInsn(PrimType primType) {
  switch (primType) {
    case PTY_u8:
    case PTY_u16:
    case PTY_u32:
    case PTY_i8:
    case PTY_i16:
    case PTY_i32:
      return MOP_wmovrr;
    case PTY_a32:
      ASSERT(false, "Invalid primitive type for AArch64");
      return MOP_undef;
    case PTY_ptr:
    case PTY_ref:
      ASSERT(false, "PTY_ref and PTY_ptr should have been lowered");
      return MOP_undef;
    case PTY_a64:
    case PTY_u64:
    case PTY_i64:
      return MOP_xmovrr;
    case PTY_f32:
      return MOP_xvmovs;
    case PTY_f64:
      return MOP_xvmovd;
    default:
      ASSERT(false, "NYI PickMovInsn");
      return MOP_undef;
  }
}

MOperator AArch64CGFunc::PickMovInsn(RegOperand &lhs, RegOperand &rhs) {
  CHECK_FATAL(lhs.GetRegisterType() == rhs.GetRegisterType(), "PickMovInsn: unequal kind NYI");
  CHECK_FATAL(lhs.GetSize() == rhs.GetSize(), "PickMovInsn: unequal size NYI");
  ASSERT(((lhs.GetSize() < k64BitSize) || (lhs.GetRegisterType() == kRegTyFloat)),
         "should split the 64 bits or more mov");
  if (lhs.GetRegisterType() == kRegTyInt) {
    return MOP_wmovrr;
  }
  if (lhs.GetRegisterType() == kRegTyFloat) {
    return (lhs.GetSize() <= k32BitSize) ? MOP_xvmovs : MOP_xvmovd;
  }
  ASSERT(false, "PickMovInsn: kind NYI");
  return MOP_undef;
}

MOperator AArch64CGFunc::PickMovInsn(uint32 bitLen, RegType regType) {
  ASSERT((bitLen == k32BitSize) || (bitLen == k64BitSize), "size check");
  ASSERT((regType == kRegTyInt) || (regType == kRegTyFloat), "type check");
  if (regType == kRegTyInt) {
    return (bitLen == k32BitSize) ? MOP_wmovrr : MOP_xmovrr;
  }
  return (bitLen == k32BitSize) ? MOP_xvmovs : MOP_xvmovd;
}

void AArch64CGFunc::SelectLoadAcquire(Operand &dest, PrimType dtype, Operand &src, PrimType stype,
                                      AArch64isa::MemoryOrdering memOrd, bool isDirect) {
  ASSERT(src.GetKind() == Operand::kOpdMem, "Just checking");
  ASSERT(memOrd != AArch64isa::kMoNone, "Just checking");

  uint32 ssize = isDirect ? src.GetSize() : GetPrimTypeBitSize(dtype);
  uint32 dsize = GetPrimTypeBitSize(dtype);
  MOperator mOp = PickLdInsn(ssize, stype, memOrd);

  Operand *newSrc = &src;
  auto &memOpnd = static_cast<AArch64MemOperand&>(src);
  AArch64OfstOperand *immOpnd = memOpnd.GetOffsetImmediate();
  int32 offset = immOpnd->GetOffsetValue();
  RegOperand *origBaseReg = memOpnd.GetBaseRegister();
  if (offset != 0) {
    RegOperand &resOpnd = CreateRegisterOperandOfType(PTY_i64);
    SelectAdd(resOpnd, *origBaseReg, *immOpnd, PTY_i64);
    newSrc = &CreateReplacementMemOperand(ssize, resOpnd, 0);
  }

  std::string key;
  if (isDirect && GetCG()->GenerateVerboseAsm()) {
    const MIRSymbol *sym = static_cast<AArch64MemOperand*>(&src)->GetSymbol();
    if (sym != nullptr) {
      MIRStorageClass sc = sym->GetStorageClass();
      if (sc == kScFormal) {
        key = "param: ";
      } else if (sc == kScAuto) {
        key = "local var: ";
      } else {
        key = "global: ";
      }
      key.append(sym->GetName());
    }
  }

  /* Check if the right load-acquire instruction is available. */
  if (mOp != MOP_undef) {
    Insn &insn = GetCG()->BuildInstruction<AArch64Insn>(mOp, dest, *newSrc);
    if (isDirect && GetCG()->GenerateVerboseAsm()) {
      insn.SetComment(key);
    }
    GetCurBB()->AppendInsn(insn);
  } else {
    if (IsPrimitiveFloat(stype)) {
      /* Uses signed integer version ldar followed by a floating-point move(fmov).  */
      ASSERT(stype == dtype, "Just checking");
      PrimType itype = (stype == PTY_f32) ? PTY_i32 : PTY_i64;
      RegOperand &regOpnd = CreateRegisterOperandOfType(itype);
      Insn &insn = GetCG()->BuildInstruction<AArch64Insn>(PickLdInsn(ssize, itype, memOrd), regOpnd, *newSrc);
      if (isDirect && GetCG()->GenerateVerboseAsm()) {
        insn.SetComment(key);
      }
      GetCurBB()->AppendInsn(insn);
      mOp = (stype == PTY_f32) ? MOP_xvmovsr : MOP_xvmovdr;
      GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, dest, regOpnd));
    } else {
      /* Use unsigned version ldarb/ldarh followed by a sign-extension instruction(sxtb/sxth).  */
      ASSERT((ssize == k8BitSize) || (ssize == k16BitSize), "Just checking");
      PrimType utype = (ssize == k8BitSize) ? PTY_u8 : PTY_u16;
      Insn &insn = GetCG()->BuildInstruction<AArch64Insn>(PickLdInsn(ssize, utype, memOrd), dest, *newSrc);
      if (isDirect && GetCG()->GenerateVerboseAsm()) {
        insn.SetComment(key);
      }
      GetCurBB()->AppendInsn(insn);
      mOp = ((dsize == k32BitSize) ? ((ssize == k8BitSize) ? MOP_xsxtb32 : MOP_xsxth32)
                                   : ((ssize == k8BitSize) ? MOP_xsxtb64 : MOP_xsxth64));
      GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, dest, dest));
    }
  }
}

void AArch64CGFunc::SelectStoreRelease(Operand &dest, PrimType dtype, Operand &src, PrimType stype,
                                       AArch64isa::MemoryOrdering memOrd, bool isDirect) {
  ASSERT(dest.GetKind() == Operand::kOpdMem, "Just checking");
  ASSERT(memOrd != AArch64isa::kMoNone, "Just checking");

  uint32 dsize = isDirect ? dest.GetSize() : GetPrimTypeBitSize(stype);
  MOperator mOp = PickStInsn(dsize, stype, memOrd);

  Operand *newDest = &dest;
  AArch64MemOperand *memOpnd = static_cast<AArch64MemOperand*>(&dest);
  AArch64OfstOperand *immOpnd = memOpnd->GetOffsetImmediate();
  int32 offset = immOpnd->GetOffsetValue();
  RegOperand *origBaseReg = memOpnd->GetBaseRegister();
  if (offset != 0) {
    RegOperand &resOpnd = CreateRegisterOperandOfType(PTY_i64);
    SelectAdd(resOpnd, *origBaseReg, *immOpnd, PTY_i64);
    newDest = &CreateReplacementMemOperand(dsize, resOpnd, 0);
  }

  std::string key;
  if (isDirect && GetCG()->GenerateVerboseAsm()) {
    const MIRSymbol *sym = static_cast<AArch64MemOperand*>(&dest)->GetSymbol();
    if (sym != nullptr) {
      MIRStorageClass sc = sym->GetStorageClass();
      if (sc == kScFormal) {
        key = "param: ";
      } else if (sc == kScAuto) {
        key = "local var: ";
      } else {
        key = "global: ";
      }
      key.append(sym->GetName());
    }
  }

  /* Check if the right store-release instruction is available. */
  if (mOp != MOP_undef) {
    Insn &insn = GetCG()->BuildInstruction<AArch64Insn>(mOp, src, *newDest);
    if (isDirect && GetCG()->GenerateVerboseAsm()) {
      insn.SetComment(key);
    }
    GetCurBB()->AppendInsn(insn);
  } else {
    /* Use a floating-point move(fmov) followed by a stlr.  */
    ASSERT(IsPrimitiveFloat(stype), "must be float type");
    CHECK_FATAL(stype == dtype, "Just checking");
    PrimType itype = (stype == PTY_f32) ? PTY_i32 : PTY_i64;
    RegOperand &regOpnd = CreateRegisterOperandOfType(itype);
    mOp = (stype == PTY_f32) ? MOP_xvmovrs : MOP_xvmovrd;
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, regOpnd, src));
    Insn &insn = GetCG()->BuildInstruction<AArch64Insn>(PickStInsn(dsize, itype, memOrd), regOpnd, *newDest);
    if (isDirect && GetCG()->GenerateVerboseAsm()) {
      insn.SetComment(key);
    }
    GetCurBB()->AppendInsn(insn);
  }
}

void AArch64CGFunc::SelectCopyImm(Operand &dest, ImmOperand &src, PrimType dtype) {
  uint32 dsize = GetPrimTypeBitSize(dtype);
  ASSERT(IsPrimitiveInteger(dtype), "The type of destination operand must be Integer");
  ASSERT(((dsize == k8BitSize) || (dsize == k16BitSize) || (dsize == k32BitSize) || (dsize == k64BitSize)),
         "The destination operand must be >= 8-bit");
  if (src.IsSingleInstructionMovable()) {
    MOperator mOp = (dsize == k32BitSize) ? MOP_xmovri32 : MOP_xmovri64;
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, dest, src));
    return;
  }
  uint64 srcVal = static_cast<uint64>(src.GetValue());
  /* using mov/movk to load the immediate value */
  if (dsize == k8BitSize) {
    /* compute lower 8 bits value */
    if (dtype == PTY_u8) {
      /* zero extend */
      srcVal = (srcVal << 56) >> 56;
      dtype = PTY_u16;
    } else {
      /* sign extend */
      srcVal = ((static_cast<int64>(srcVal)) << 56) >> 56;
      dtype = PTY_i16;
    }
    dsize = k16BitSize;
  }
  if (dsize == k16BitSize) {
    if (dtype == PTY_u16) {
      /* check lower 16 bits and higher 16 bits respectively */
      ASSERT((srcVal & 0x0000FFFFULL) != 0, "unexpected value");
      ASSERT(((srcVal >> k16BitSize) & 0x0000FFFFULL) == 0, "unexpected value");
      ASSERT((srcVal & 0x0000FFFFULL) != 0xFFFFULL, "unexpected value");
      /* create an imm opereand which represents lower 16 bits of the immediate */
      ImmOperand &srcLower = CreateImmOperand((srcVal & 0x0000FFFFULL), k16BitSize, false);
      GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_xmovri32, dest, srcLower));
      return;
    } else {
      /* sign extend and let `dsize == 32` case take care of it */
      srcVal = ((static_cast<int64>(srcVal)) << 48) >> 48;
      dsize = k32BitSize;
    }
  }
  if (dsize == k32BitSize) {
    /* check lower 16 bits and higher 16 bits respectively */
    ASSERT((srcVal & 0x0000FFFFULL) != 0, "unexpected val");
    ASSERT(((srcVal >> k16BitSize) & 0x0000FFFFULL) != 0, "unexpected val");
    ASSERT((srcVal & 0x0000FFFFULL) != 0xFFFFULL, "unexpected val");
    ASSERT(((srcVal >> k16BitSize) & 0x0000FFFFULL) != 0xFFFFULL, "unexpected val");
    /* create an imm opereand which represents lower 16 bits of the immediate */
    ImmOperand &srcLower = CreateImmOperand((srcVal & 0x0000FFFFULL), k16BitSize, false);
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_xmovri32, dest, srcLower));
    /* create an imm opereand which represents upper 16 bits of the immediate */
    ImmOperand &srcUpper = CreateImmOperand(((srcVal >> k16BitSize) & 0x0000FFFFULL), k16BitSize, false);
    LogicalShiftLeftOperand *lslOpnd = GetLogicalShiftLeftOperand(k16BitSize, false);
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_wmovkri16, dest, srcUpper, *lslOpnd));
  } else {
    /*
     * partition it into 4 16-bit chunks
     * if more 0's than 0xFFFF's, use movz as the initial instruction.
     * otherwise, movn.
     */
    bool useMovz = BetterUseMOVZ(srcVal);
    bool useMovk = false;
    /* get lower 32 bits of the immediate */
    uint64 chunkLval = srcVal & 0xFFFFFFFFULL;
    /* get upper 32 bits of the immediate */
    uint64 chunkHval = (srcVal >> k32BitSize) & 0xFFFFFFFFULL;
    int32 maxLoopTime = 4;

    if (chunkLval == chunkHval) {
      /* compute lower 32 bits, and then copy to higher 32 bits, so only 2 chunks need be processed */
      maxLoopTime = 2;
    }

    uint64 sa = 0;

    for (int64 i = 0; i < maxLoopTime; ++i, sa += k16BitSize) {
      /* create an imm opereand which represents the i-th 16-bit chunk of the immediate */
      uint64 chunkVal = (srcVal >> (static_cast<uint64>(sa))) & 0x0000FFFFULL;
      if (useMovz ? (chunkVal == 0) : (chunkVal == 0x0000FFFFULL)) {
        continue;
      }
      ImmOperand &src16 = CreateImmOperand(chunkVal, k16BitSize, false);
      LogicalShiftLeftOperand *lslOpnd = GetLogicalShiftLeftOperand(sa, true);
      if (!useMovk) {
        /* use movz or movn */
        if (!useMovz) {
          src16.BitwiseNegate();
        }
        GetCurBB()->AppendInsn(
            GetCG()->BuildInstruction<AArch64Insn>(useMovz ? MOP_xmovzri16 : MOP_xmovnri16, dest, src16, *lslOpnd));
        useMovk = true;
      } else {
        GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_xmovkri16, dest, src16, *lslOpnd));
      }
    }

    if (maxLoopTime == 2) {
      /* copy lower 32 bits to higher 32 bits */
      AArch64ImmOperand &immOpnd = CreateImmOperand(k32BitSize, k8BitSize, false);
      GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MPO_xbfirri6i6, dest, dest, immOpnd, immOpnd));
    }
  }
}

void AArch64CGFunc::SelectCopyMemOpnd(Operand &dest, PrimType dtype, uint32 dsize,
                                      Operand &src, PrimType stype) {
  AArch64isa::MemoryOrdering memOrd = AArch64isa::kMoNone;
  const MIRSymbol *sym = static_cast<AArch64MemOperand*>(&src)->GetSymbol();
  if ((sym != nullptr) && (sym->GetStorageClass() == kScGlobal) && sym->GetAttr(ATTR_memory_order_acquire)) {
    memOrd = AArch64isa::kMoAcquire;
  }

  if (memOrd != AArch64isa::kMoNone) {
    AArch64CGFunc::SelectLoadAcquire(dest, dtype, src, stype, memOrd, true);
    return;
  }
  Insn *insn = nullptr;
  uint32 ssize = src.GetSize();
  if (!IsPrimitiveFloat(stype)) {
    insn = &GetCG()->BuildInstruction<AArch64Insn>(PickLdInsn(ssize, stype), dest, src);
  } else {
    CHECK_FATAL(dsize == ssize, "dsize %u expect equals ssize %u", dtype, ssize);
    insn = &GetCG()->BuildInstruction<AArch64Insn>(PickLdInsn(ssize, stype), dest, src);
  }

  if (GetCG()->GenerateVerboseAsm()) {
    const MIRSymbol *symSecond = static_cast<AArch64MemOperand*>(&src)->GetSymbol();
    if (symSecond != nullptr) {
      std::string key;
      MIRStorageClass sc = symSecond->GetStorageClass();
      if (sc == kScFormal) {
        key = "param: ";
      } else if (sc == kScAuto) {
        key = "local var: ";
      } else {
        key = "global: ";
      }
      insn->SetComment(key.append(symSecond->GetName()));
    }
  }

  GetCurBB()->AppendInsn(*insn);
}

bool AArch64CGFunc::IsImmediateValueInRange(MOperator mOp, int64 immVal, bool is64Bits,
                                            bool isIntactIndexed, bool isPostIndexed, bool isPreIndexed) const {
  bool isInRange = false;
  switch (mOp) {
    case MOP_xstr:
    case MOP_wstr:
      isInRange =
          (isIntactIndexed &&
           ((!is64Bits && (immVal >= kStrAllLdrAllImmLowerBound) && (immVal <= kStrLdrImm32UpperBound)) ||
            (is64Bits && (immVal >= kStrAllLdrAllImmLowerBound) && (immVal <= kStrLdrImm64UpperBound)))) ||
          ((isPostIndexed || isPreIndexed) && (immVal >= kStrLdrPerPostLowerBound) &&
           (immVal <= kStrLdrPerPostUpperBound));
      break;
    case MOP_wstrb:
      isInRange =
          (isIntactIndexed && (immVal >= kStrAllLdrAllImmLowerBound) && (immVal <= kStrbLdrbImmUpperBound)) ||
          ((isPostIndexed || isPreIndexed) && (immVal >= kStrLdrPerPostLowerBound) &&
           (immVal <= kStrLdrPerPostUpperBound));
      break;
    case MOP_wstrh:
      isInRange =
          (isIntactIndexed && (immVal >= kStrAllLdrAllImmLowerBound) && (immVal <= kStrhLdrhImmUpperBound)) ||
          ((isPostIndexed || isPreIndexed) && (immVal >= kStrLdrPerPostLowerBound) &&
           (immVal <= kStrLdrPerPostUpperBound));
      break;
    default:
      break;
  }
  return isInRange;
}

bool AArch64CGFunc::IsStoreMop(MOperator mOp) const {
  switch (mOp) {
    case MOP_xstr:
    case MOP_wstr:
    case MOP_wstrb:
    case MOP_wstrh:
      return true;
    default:
      return false;
  }
}

void AArch64CGFunc::SplitMovImmOpndInstruction(int64 immVal, RegOperand &destReg) {
  bool useMovz = BetterUseMOVZ(immVal);
  bool useMovk = false;
  /* get lower 32 bits of the immediate */
  uint64 chunkLval = static_cast<uint64>(immVal) & 0xFFFFFFFFULL;
  /* get upper 32 bits of the immediate */
  uint64 chunkHval = (static_cast<uint64>(immVal) >> k32BitSize) & 0xFFFFFFFFULL;
  int32 maxLoopTime = 4;

  if (chunkLval == chunkHval) {
    /* compute lower 32 bits, and then copy to higher 32 bits, so only 2 chunks need be processed */
    maxLoopTime = 2;
  }

  uint64 sa = 0;
  for (int64 i = 0 ; i < maxLoopTime; ++i, sa += k16BitSize) {
    /* create an imm opereand which represents the i-th 16-bit chunk of the immediate */
    uint64 chunkVal = (static_cast<uint64>(immVal) >> sa) & 0x0000FFFFULL;
    if (useMovz ? (chunkVal == 0) : (chunkVal == 0x0000FFFFULL)) {
      continue;
    }
    ImmOperand &src16 = CreateImmOperand(chunkVal, k16BitSize, false);
    LogicalShiftLeftOperand *lslOpnd = GetLogicalShiftLeftOperand(sa, true);
    if (!useMovk) {
      /* use movz or movn */
      if (!useMovz) {
        src16.BitwiseNegate();
      }
      MOperator mOpCode = useMovz ? MOP_xmovzri16 : MOP_xmovnri16;
      GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOpCode, destReg, src16, *lslOpnd));
      useMovk = true;
    } else {
      GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_xmovkri16, destReg, src16, *lslOpnd));
    }
  }

  if (maxLoopTime == 2) {
    /* copy lower 32 bits to higher 32 bits */
    AArch64ImmOperand &immOpnd = CreateImmOperand(k32BitSize, k8BitSize, false);
    Insn &insn = GetCG()->BuildInstruction<AArch64Insn>(MPO_xbfirri6i6, destReg, destReg, immOpnd, immOpnd);
    GetCurBB()->AppendInsn(insn);
  }
}

void AArch64CGFunc::SelectCopyRegOpnd(Operand &dest, PrimType dtype, Operand::OperandType opndType,
                                      uint32 dsize, Operand &src, PrimType stype) {
  if (opndType != Operand::kOpdMem) {
    ASSERT(stype != PTY_a32, "");
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(PickMovInsn(stype), dest, src));
    return;
  }
  AArch64isa::MemoryOrdering memOrd = AArch64isa::kMoNone;
  const MIRSymbol *sym = static_cast<AArch64MemOperand*>(&dest)->GetSymbol();
  if ((sym != nullptr) && (sym->GetStorageClass() == kScGlobal) && sym->GetAttr(ATTR_memory_order_release)) {
    memOrd = AArch64isa::kMoRelease;
  }

  if (memOrd != AArch64isa::kMoNone) {
    AArch64CGFunc::SelectStoreRelease(dest, dtype, src, stype, memOrd, true);
    return;
  }

  bool is64Bits = (dest.GetSize() == k64BitSize) ? true : false;
  MOperator strMop = PickStInsn(dsize, stype);
  if (!dest.IsMemoryAccessOperand()) {
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(strMop, src, dest));
    return;
  }

  AArch64MemOperand *memOpnd = static_cast<AArch64MemOperand*>(&dest);
  ASSERT(memOpnd, "memOpnd should not be nullptr");
  if (memOpnd->GetAddrMode() == AArch64MemOperand::kAddrModeLo12Li) {
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(strMop, src, dest));
    return;
  }
  if (memOpnd->GetOffsetOperand() == nullptr) {
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(strMop, src, dest));
    return;
  }
  ImmOperand *immOpnd = static_cast<ImmOperand*>(memOpnd->GetOffsetOperand());
  ASSERT(immOpnd, "immOpnd should not be nullptr");
  int64 immVal = immOpnd->GetValue();
  bool isIntactIndexed = memOpnd->IsIntactIndexed();
  bool isPostIndexed = memOpnd->IsPostIndexed();
  bool isPreIndexed = memOpnd->IsPreIndexed();
  bool isInRange = IsImmediateValueInRange(strMop, immVal, is64Bits, isIntactIndexed, isPostIndexed, isPreIndexed);
  bool isMopStr = IsStoreMop(strMop);
  if (isInRange || !isMopStr) {
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(strMop, src, dest));
    return;
  }
  if (isIntactIndexed) {
    RegOperand &reg = CreateRegisterOperandOfType(PTY_i64);
    AArch64ImmOperand *aarch64ImmOpnd = static_cast<AArch64ImmOperand*>(immOpnd);
    if (aarch64ImmOpnd->IsSingleInstructionMovable()) {
      MOperator mOp = MOP_xmovri64;
      GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, reg, *immOpnd));
    } else {
      SplitMovImmOpndInstruction(immVal, reg);
    }
    MemOperand &newDest = GetOrCreateMemOpnd(AArch64MemOperand::kAddrModeBOrX, GetPrimTypeBitSize(dtype),
                                             memOpnd->GetBaseRegister(), &reg, nullptr, nullptr);
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(strMop, src, newDest));
  } else if (isPostIndexed || isPreIndexed) {
    RegOperand &reg = CreateRegisterOperandOfType(PTY_i64);
    MOperator mopMov = MOP_xmovri64;
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mopMov, reg, *immOpnd));
    MOperator mopAdd = MOP_xaddrrr;
    MemOperand &newDest =
        GetOrCreateMemOpnd(AArch64MemOperand::kAddrModeBOi, GetPrimTypeBitSize(dtype), memOpnd->GetBaseRegister(),
                           nullptr, &GetOrCreateOfstOpnd(0, k32BitSize), nullptr);
    Insn &insn1 = GetCG()->BuildInstruction<AArch64Insn>(strMop, src, newDest);
    Insn &insn2 = GetCG()->BuildInstruction<AArch64Insn>(mopAdd, *newDest.GetBaseRegister(),
                                                         *newDest.GetBaseRegister(), reg);
    if (isPostIndexed) {
      GetCurBB()->AppendInsn(insn1);
      GetCurBB()->AppendInsn(insn2);
    } else {
      /* isPreIndexed */
      GetCurBB()->AppendInsn(insn2);
      GetCurBB()->AppendInsn(insn1);
    }
  }
}

void AArch64CGFunc::SelectCopy(Operand &dest, PrimType dtype, Operand &src, PrimType stype) {
  ASSERT(dest.IsRegister() || dest.IsMemoryAccessOperand(), "");
  uint32 dsize = GetPrimTypeBitSize(dtype);
  if (dest.IsRegister()) {
    dsize = dest.GetSize();
  }
  Operand::OperandType opnd0Type = dest.GetKind();
  Operand::OperandType opnd1Type = src.GetKind();
  ASSERT(((dsize >= src.GetSize()) || (opnd0Type == Operand::kOpdMem)), "NYI");
  ASSERT(((opnd0Type == Operand::kOpdRegister) || (src.GetKind() == Operand::kOpdRegister)),
         "either src or dest should be register");

  switch (opnd1Type) {
    case Operand::kOpdMem:
      SelectCopyMemOpnd(dest, dtype, dsize, src, stype);
      break;
    case Operand::kOpdOffset:
    case Operand::kOpdImmediate:
      SelectCopyImm(dest, static_cast<ImmOperand&>(src), stype);
      break;
    case Operand::kOpdFPZeroImmediate:
      GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>((dsize == k32BitSize) ? MOP_xvmovsr : MOP_xvmovdr,
                                                                    dest, AArch64RegOperand::GetZeroRegister(dsize)));
      break;
    case Operand::kOpdRegister:
      SelectCopyRegOpnd(dest, dtype, opnd0Type, dsize, src, stype);
      break;
    default:
      CHECK_FATAL(false, "NYI");
  }
}

/* This function copies src to a register, the src can be an imm, mem or a label */
RegOperand &AArch64CGFunc::SelectCopy(Operand &src, PrimType stype, PrimType dtype) {
  RegOperand &dest = CreateRegisterOperandOfType(dtype);
  SelectCopy(dest, dtype, src, stype);
  return dest;
}

/*
 * We need to adjust the offset of a stack allocated local variable
 * if we store FP/SP before any other local variables to save an instruction.
 * See AArch64CGFunc::OffsetAdjustmentForFPLR() in aarch64_cgfunc.cpp
 *
 * That is when we !UsedStpSubPairForCallFrameAllocation().
 *
 * Because we need to use the STP/SUB instruction pair to store FP/SP 'after'
 * local variables when the call frame size is greater that the max offset
 * value allowed for the STP instruction (we cannot use STP w/ prefix, LDP w/
 * postfix), if UsedStpSubPairForCallFrameAllocation(), we don't need to
 * adjust the offsets.
 */
bool AArch64CGFunc::IsImmediateOffsetOutOfRange(AArch64MemOperand &memOpnd, uint32 bitLen) {
  ASSERT(bitLen >= k8BitSize, "bitlen error");
  ASSERT(bitLen <= k64BitSize, "bitlen error");
  ASSERT((bitLen & (bitLen - 1)) == 0, "bitlen error");
  AArch64MemOperand::AArch64AddressingMode mode = memOpnd.GetAddrMode();
  if ((mode == AArch64MemOperand::kAddrModeBOi) && memOpnd.IsIntactIndexed()) {
    int32 offsetValue = memOpnd.GetOffsetImmediate()->GetOffsetValue();
    if (memOpnd.GetOffsetImmediate()->GetVary() == kUnAdjustVary) {
      offsetValue += static_cast<AArch64MemLayout*>(GetMemlayout())->RealStackFrameSize() + 0xff;
    }
    offsetValue += 2 * kIntregBytelen;  /* Refer to the above comment */
    return AArch64MemOperand::IsPIMMOffsetOutOfRange(offsetValue, bitLen);
  } else {
    return false;
  }
}

AArch64MemOperand &AArch64CGFunc::CreateReplacementMemOperand(uint32 bitLen,
                                                              RegOperand &baseReg, int32 offset) {
  return static_cast<AArch64MemOperand&>(CreateMemOpnd(baseReg, offset, bitLen));
}

bool AArch64CGFunc::CheckIfSplitOffsetWithAdd(const AArch64MemOperand &memOpnd, uint32 bitLen) {
  if (memOpnd.GetAddrMode() != AArch64MemOperand::kAddrModeBOi || !memOpnd.IsIntactIndexed()) {
    return false;
  }
  AArch64OfstOperand *ofstOpnd = memOpnd.GetOffsetImmediate();
  int32 opndVal = ofstOpnd->GetOffsetValue();
  int32 maxPimm = memOpnd.GetMaxPIMM(bitLen);
  int32 q0 = opndVal / maxPimm;
  int32 addend = q0 * maxPimm;
  int32 r0 = opndVal - addend;
  int32 alignment = memOpnd.GetImmediateOffsetAlignment(bitLen);
  int32 r1 = static_cast<uint32>(r0) & ((1u << static_cast<uint32>(alignment)) - 1);
  addend = addend + r1;
  return (addend > 0);
}

AArch64MemOperand &AArch64CGFunc::SplitOffsetWithAddInstruction(const AArch64MemOperand &memOpnd, uint32 bitLen,
                                                                AArch64reg baseRegNum, uint32 isDest, Insn *insn) {
  ASSERT((memOpnd.GetAddrMode() == AArch64MemOperand::kAddrModeBOi), "expect kAddrModeBOi memOpnd");
  ASSERT(memOpnd.IsIntactIndexed(), "expect intactIndexed memOpnd");
  AArch64OfstOperand *ofstOpnd = memOpnd.GetOffsetImmediate();
  int32 opndVal = ofstOpnd->GetOffsetValue();

  /*
   * opndVal == Q0 * 32760(16380) + R0
   * R0 == Q1 * 8(4) + R1
   * ADDEND == Q0 * 32760(16380) + R1
   * NEW_OFFSET = Q1 * 8(4)
   * we want to generate two instructions:
   * ADD TEMP_REG, X29, ADDEND
   * LDR/STR TEMP_REG, [ TEMP_REG, #NEW_OFFSET ]
   */
  int32 maxPimm = memOpnd.GetMaxPIMM(bitLen);
  int32 q0 = opndVal / maxPimm;
  int32 addend = q0 * maxPimm;
  int32 r0 = opndVal - addend;
  int32 alignment = memOpnd.GetImmediateOffsetAlignment(bitLen);
  int32 q1 = static_cast<uint32>(r0) >> static_cast<uint32>(alignment);
  int32 r1 = static_cast<uint32>(r0) & ((1u << static_cast<uint32>(alignment)) - 1);
  addend = addend + r1;
  RegOperand *origBaseReg = memOpnd.GetBaseRegister();
  if (addend > 0) {
    int32 t = addend;
    constexpr uint32 suffixClear = 0xfffff000;
    addend = (static_cast<uint32>(addend) & suffixClear);
    q1 = (static_cast<uint32>(q1) << static_cast<uint32>(alignment)) + (t - addend);
    if (AArch64MemOperand::IsPIMMOffsetOutOfRange(q1, bitLen)) {
      addend = (static_cast<uint32>(opndVal) & suffixClear);
      q1 = opndVal - addend;
    }
    ImmOperand &immAddend = CreateImmOperand(addend, k64BitSize, true);
    RegOperand &resOpnd = (baseRegNum == AArch64reg::kRinvalid)
                           ? CreateRegisterOperandOfType(PTY_i64)
                           : GetOrCreatePhysicalRegisterOperand(baseRegNum, kSizeOfPtr * kBitsPerByte, kRegTyInt);
    if (insn == nullptr) {
      SelectAdd(resOpnd, *origBaseReg, immAddend, PTY_i64);
    } else {
      SelectAddAfterInsn(resOpnd, *origBaseReg, immAddend, PTY_i64, isDest, *insn);
    }
    AArch64MemOperand &newMemOpnd = CreateReplacementMemOperand(bitLen, resOpnd, q1);
    newMemOpnd.SetStackMem(memOpnd.IsStackMem());
    return newMemOpnd;
  } else {
    AArch64MemOperand &newMemOpnd = CreateReplacementMemOperand(
        bitLen, *origBaseReg, (static_cast<uint32>(q1) << static_cast<uint32>(alignment)));
    newMemOpnd.SetStackMem(memOpnd.IsStackMem());
    return newMemOpnd;
  }
}

void AArch64CGFunc::SelectDassign(DassignNode &stmt, Operand &opnd0) {
  SelectDassign(stmt.GetStIdx(), stmt.GetFieldID(), stmt.GetRHS()->GetPrimType(), opnd0);
}

/*
 * Used for SelectDassign when do optimization for volatile store, because the stlr instruction only allow
 * store to the memory addrress with the register base offset 0.
 * STLR <Wt>, [<Xn|SP>{,#0}], 32-bit variant (size = 10)
 * STLR <Xt>, [<Xn|SP>{,#0}], 64-bit variant (size = 11)
 * So the function do the prehandle of the memory operand to satisify the Store-Release..
 */
RegOperand *AArch64CGFunc::ExtractNewMemBase(MemOperand &memOpnd) {
  const MIRSymbol *sym = memOpnd.GetSymbol();
  AArch64MemOperand::AArch64AddressingMode mode = static_cast<AArch64MemOperand*>(&memOpnd)->GetAddrMode();
  if (mode == AArch64MemOperand::kAddrModeLiteral) {
    return nullptr;
  }
  RegOperand *baseOpnd = memOpnd.GetBaseRegister();
  RegOperand &resultOpnd = CreateRegisterOperandOfType(baseOpnd->GetRegisterType(), baseOpnd->GetSize() / kBitsPerByte);
  bool is64Bits = (baseOpnd->GetSize() == k64BitSize);
  if (mode == AArch64MemOperand::kAddrModeLo12Li) {
    StImmOperand &stImm = CreateStImmOperand(*sym, 0, 0);
    Insn &addInsn = GetCG()->BuildInstruction<AArch64Insn>(MOP_xadrpl12, resultOpnd, *baseOpnd, stImm);
    addInsn.SetComment("new add insn");
    GetCurBB()->AppendInsn(addInsn);
  } else if (mode == AArch64MemOperand::kAddrModeBOi) {
    AArch64OfstOperand *offsetOpnd = static_cast<AArch64MemOperand*>(&memOpnd)->GetOffsetImmediate();
    if (offsetOpnd->GetOffsetValue() != 0) {
      MOperator mOp = is64Bits ? MOP_xaddrri12 : MOP_waddrri12;
      GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, resultOpnd, *baseOpnd, *offsetOpnd));
    } else {
      return baseOpnd;
    }
  } else {
    CHECK_FATAL(mode == AArch64MemOperand::kAddrModeBOrX, "unexpect addressing mode.");
    RegOperand *regOpnd = static_cast<AArch64MemOperand*>(&memOpnd)->GetOffsetRegister();
    MOperator mOp = is64Bits ? MOP_xaddrrr : MOP_waddrrr;
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, resultOpnd, *baseOpnd, *regOpnd));
  }
  return &resultOpnd;
}

/*
 * NOTE: I divided SelectDassign so that we can create "virtual" assignments
 * when selecting other complex Maple IR instructions. For example, the atomic
 * exchange and other intrinsics will need to assign its results to local
 * variables. Such Maple IR instructions are pltform-specific (e.g.
 * atomic_exchange can be implemented as one single machine intruction on x86_64
 * and ARMv8.1, but ARMv8.0 needs an LL/SC loop), therefore they cannot (in
 * principle) be lowered at BELowerer or CGLowerer.
 */
void AArch64CGFunc::SelectDassign(StIdx stIdx, FieldID fieldId, PrimType rhsPType, Operand &opnd0) {
  MIRSymbol *symbol = GetFunction().GetLocalOrGlobalSymbol(stIdx);
  int32 offset = 0;
  if (fieldId != 0) {
    MIRStructType *structType = static_cast<MIRStructType*>(symbol->GetType());
    ASSERT(structType != nullptr, "SelectDassign: non-zero fieldID for non-structure");
    offset = GetBecommon().GetFieldOffset(*structType, fieldId).first;
  }
  uint32 regSize = GetPrimTypeBitSize(rhsPType);
  MIRType *type = symbol->GetType();
  Operand &stOpnd = LoadIntoRegister(opnd0, IsPrimitiveInteger(rhsPType), regSize,
                                     IsSignedInteger(type->GetPrimType()));
  MOperator mOp = MOP_undef;
  if (type->GetKind() == kTypeStruct) {
    MIRStructType *structType = static_cast<MIRStructType*>(type);
    type = structType->GetFieldType(fieldId);
  } else if (type->GetKind() == kTypeClass) {
    MIRClassType *classType = static_cast<MIRClassType*>(type);
    type = classType->GetFieldType(fieldId);
  }

  uint32 dataSize = GetPrimTypeBitSize(type->GetPrimType());
  MemOperand *memOpnd = &GetOrCreateMemOpnd(*symbol, offset, dataSize);
  AArch64MemOperand &archMemOperand = *static_cast<AArch64MemOperand*>(memOpnd);
  if ((memOpnd->GetMemVaryType() == kNotVary) && IsImmediateOffsetOutOfRange(archMemOperand, dataSize)) {
    memOpnd = &SplitOffsetWithAddInstruction(archMemOperand, dataSize);
  }

  ASSERT(((type->GetKind() == kTypeScalar) || (type->GetKind() == kTypePointer)), "NYI dassign type");
  PrimType ptyp = type->GetPrimType();

  AArch64isa::MemoryOrdering memOrd = AArch64isa::kMoNone;
  if (isVolStore) {
    RegOperand *baseOpnd = ExtractNewMemBase(*memOpnd);
    if (baseOpnd != nullptr) {
      memOpnd = &CreateMemOpnd(*baseOpnd, 0, dataSize);
      memOrd = AArch64isa::kMoRelease;
      isVolStore = false;
    }
  }
  if (memOrd == AArch64isa::kMoNone) {
    mOp = PickStInsn(GetPrimTypeBitSize(ptyp), ptyp);
    Insn &insn = GetCG()->BuildInstruction<AArch64Insn>(mOp, stOpnd, *memOpnd);

    if (GetCG()->GenerateVerboseAsm()) {
      const MIRSymbol *symSecond = static_cast<AArch64MemOperand*>(memOpnd)->GetSymbol();
      if (symSecond != nullptr) {
        std::string key;
        MIRStorageClass sc = symSecond->GetStorageClass();
        if (sc == kScFormal) {
          key = "param: ";
        } else if (sc == kScAuto) {
          key = "local var: ";
        } else {
          key = "global: ";
        }
        insn.SetComment(key.append(symSecond->GetName()));
      }
    }

    GetCurBB()->AppendInsn(insn);
  } else {
    AArch64CGFunc::SelectStoreRelease(*memOpnd, ptyp, stOpnd, ptyp, memOrd, true);
  }
}

void AArch64CGFunc::SelectAssertNull(UnaryStmtNode &stmt) {
  Operand *opnd0 = HandleExpr(stmt, *stmt.Opnd(0));
  RegOperand &baseReg = LoadIntoRegister(*opnd0, PTY_a64);
  auto &zwr = AArch64RegOperand::Get32bitZeroRegister();
  auto &mem = CreateMemOpnd(baseReg, 0, k32BitSize);
  Insn &loadRef = GetCG()->BuildInstruction<AArch64Insn>(MOP_wldr, zwr, mem);
  loadRef.SetDoNotRemove(true);
  if (GetCG()->GenerateVerboseAsm()) {
    loadRef.SetComment("null pointer check");
  }
  GetCurBB()->AppendInsn(loadRef);
}

void AArch64CGFunc::SelectRegassign(RegassignNode &stmt, Operand &opnd0) {
  RegOperand *regOpnd = nullptr;
  PregIdx pregIdx = stmt.GetRegIdx();
  if (IsSpecialPseudoRegister(pregIdx)) {
    /* if it is one of special registers */
    ASSERT(-pregIdx != kSregRetval0, "the dest of RegAssign node must not be kSregRetval0");
    regOpnd = &GetOrCreateSpecialRegisterOperand(-pregIdx);
  } else {
    regOpnd = &GetOrCreateVirtualRegisterOperand(GetVirtualRegNOFromPseudoRegIdx(pregIdx));
  }
  /* look at rhs */
  PrimType rhsType = stmt.Opnd(0)->GetPrimType();
  PrimType dtype = rhsType;
  if (GetPrimTypeBitSize(dtype) < k32BitSize) {
    ASSERT(IsPrimitiveInteger(dtype), "");
    dtype = IsSignedInteger(dtype) ? PTY_i32 : PTY_u32;
  }
  ASSERT(regOpnd != nullptr, "null ptr check!");
  SelectCopy(*regOpnd, dtype, opnd0, rhsType);

  if ((Globals::GetInstance()->GetOptimLevel() == 0) && (pregIdx >= 0)) {
    MemOperand *dest = GetPseudoRegisterSpillMemoryOperand(pregIdx);
    PrimType stype = GetTypeFromPseudoRegIdx(pregIdx);
    MIRPreg *preg = GetFunction().GetPregTab()->PregFromPregIdx(pregIdx);
    uint32 srcBitLength = GetPrimTypeSize(preg->GetPrimType()) * kBitsPerByte;
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(PickStInsn(srcBitLength, stype), *regOpnd, *dest));
  }
}

void AArch64CGFunc::SelectAggDassign(DassignNode &stmt) {
  MIRSymbol *lhsSymbol = GetFunction().GetLocalOrGlobalSymbol(stmt.GetStIdx());
  int32 lhsOffset = 0;
  MIRType *lhsType = lhsSymbol->GetType();
  if (stmt.GetFieldID() != 0) {
    MIRStructType *structType = static_cast<MIRStructType*>(lhsSymbol->GetType());
    ASSERT(structType != nullptr, "SelectAggDassign: non-zero fieldID for non-structure");
    lhsType = structType->GetFieldType(stmt.GetFieldID());
    lhsOffset = GetBecommon().GetFieldOffset(*structType, stmt.GetFieldID()).first;
  }
  uint32 lhsAlign = GetBecommon().GetTypeAlign(lhsType->GetTypeIndex());
  uint64 lhsSize = GetBecommon().GetTypeSize(lhsType->GetTypeIndex());

  uint32 rhsAlign;
  uint32 alignUsed;
  int32 rhsOffset = 0;
  if (stmt.GetRHS()->GetOpCode() == OP_dread) {
    AddrofNode *rhsDread = static_cast<AddrofNode*>(stmt.GetRHS());
    MIRSymbol *rhsSymbol = GetFunction().GetLocalOrGlobalSymbol(rhsDread->GetStIdx());
    MIRType *rhsType = rhsSymbol->GetType();
    if (rhsDread->GetFieldID() != 0) {
      MIRStructType *structType = static_cast<MIRStructType*>(rhsSymbol->GetType());
      ASSERT(structType != nullptr, "SelectAggDassign: non-zero fieldID for non-structure");
      rhsType = structType->GetFieldType(rhsDread->GetFieldID());
      rhsOffset = GetBecommon().GetFieldOffset(*structType, rhsDread->GetFieldID()).first;
    }
    rhsAlign = GetBecommon().GetTypeAlign(rhsType->GetTypeIndex());
    alignUsed = std::min(lhsAlign, rhsAlign);
    ASSERT(alignUsed != 0, "expect non-zero");
    for (uint32 i = 0; i < (lhsSize / alignUsed); i++) {
      /* generate the load */
      Operand &rhsMemOpnd = GetOrCreateMemOpnd(*rhsSymbol, rhsOffset + i * alignUsed, alignUsed * k8BitSize);
      regno_t vRegNO = NewVReg(kRegTyInt, std::max(4u, alignUsed));
      RegOperand &result = CreateVirtualRegisterOperand(vRegNO);
      GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(PickLdInsn(alignUsed * k8BitSize, PTY_u32),
                                                                    result, rhsMemOpnd));
      /* generate the store */
      Operand &lhsMemOpnd = GetOrCreateMemOpnd(*lhsSymbol, lhsOffset + i * alignUsed, alignUsed * k8BitSize);
      GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(PickStInsn(alignUsed * k8BitSize, PTY_u32),
                                                                    result, lhsMemOpnd));
    }
    /* take care of extra content at the end less than the unit of alignUsed */
    uint64 lhsSizeCovered = (lhsSize / alignUsed) * alignUsed;
    uint32 newAlignUsed = alignUsed;
    while (lhsSizeCovered < lhsSize) {
      newAlignUsed = newAlignUsed >> 1;
      CHECK_FATAL(newAlignUsed != 0, "expect non-zero");
      if ((lhsSizeCovered + newAlignUsed) > lhsSize) {
        continue;
      }
      /* generate the load */
      Operand &rhsMemOpnd = GetOrCreateMemOpnd(*rhsSymbol, rhsOffset + lhsSizeCovered, newAlignUsed * k8BitSize);
      regno_t vRegNO = NewVReg(kRegTyInt, std::max(4u, newAlignUsed));
      RegOperand &result = CreateVirtualRegisterOperand(vRegNO);
      GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(PickLdInsn(newAlignUsed * k8BitSize, PTY_u32),
                                                                    result, rhsMemOpnd));
      /* generate the store */
      Operand &lhsMemOpnd = GetOrCreateMemOpnd(*lhsSymbol, lhsOffset + lhsSizeCovered, newAlignUsed * k8BitSize);
      GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(PickStInsn(newAlignUsed * k8BitSize, PTY_u32),
                                                                    result, lhsMemOpnd));
      lhsSizeCovered += newAlignUsed;
    }
  } else {  /* rhs is iread */
    ASSERT(stmt.GetRHS()->GetOpCode() == OP_iread, "SelectAggDassign: NYI");
    IreadNode *rhsIread = static_cast<IreadNode*>(stmt.GetRHS());
    RegOperand *addrOpnd = static_cast<RegOperand*>(HandleExpr(*rhsIread, *rhsIread->Opnd(0)));
    addrOpnd = &LoadIntoRegister(*addrOpnd, rhsIread->Opnd(0)->GetPrimType());
    MIRPtrType *rhsPointerType = static_cast<MIRPtrType*>(
        GlobalTables::GetTypeTable().GetTypeFromTyIdx(rhsIread->GetTyIdx()));
    MIRType *rhsType = static_cast<MIRStructType*>(
        GlobalTables::GetTypeTable().GetTypeFromTyIdx(rhsPointerType->GetPointedTyIdx()));
    bool isRefField = false;
    if (rhsIread->GetFieldID() != 0) {
      MIRStructType *rhsStructType = static_cast<MIRStructType*>(rhsType);
      ASSERT(rhsStructType != nullptr, "SelectAggDassign: non-zero fieldID for non-structure");
      rhsType = rhsStructType->GetFieldType(rhsIread->GetFieldID());
      rhsOffset = GetBecommon().GetFieldOffset(*rhsStructType, rhsIread->GetFieldID()).first;
      isRefField = GetBecommon().IsRefField(*rhsStructType, rhsIread->GetFieldID());
    }
    rhsAlign = GetBecommon().GetTypeAlign(rhsType->GetTypeIndex());
    alignUsed = std::min(lhsAlign, rhsAlign);
    ASSERT(alignUsed != 0, "expect non-zero");
    for (uint32 i = 0; i < (lhsSize / alignUsed); i++) {
      /* generate the load */
      AArch64OfstOperand &ofstOpnd = GetOrCreateOfstOpnd(rhsOffset + i * alignUsed, k32BitSize);
      Operand &rhsMemOpnd = GetOrCreateMemOpnd(AArch64MemOperand::kAddrModeBOi, alignUsed * k8BitSize,
                                               addrOpnd, nullptr, &ofstOpnd, nullptr);
      regno_t vRegNO = NewVReg(kRegTyInt, std::max(4u, alignUsed));
      RegOperand &result = CreateVirtualRegisterOperand(vRegNO);
      Insn &insn =
          GetCG()->BuildInstruction<AArch64Insn>(PickLdInsn(alignUsed * k8BitSize, PTY_u32), result, rhsMemOpnd);
      insn.MarkAsAccessRefField(isRefField);
      GetCurBB()->AppendInsn(insn);
      /* generate the store */
      Operand &lhsMemOpnd = GetOrCreateMemOpnd(*lhsSymbol, lhsOffset + i * alignUsed, alignUsed * k8BitSize);
      GetCurBB()->AppendInsn(
          GetCG()->BuildInstruction<AArch64Insn>(PickStInsn(alignUsed * k8BitSize, PTY_u32), result, lhsMemOpnd));
    }
    /* take care of extra content at the end less than the unit of alignUsed */
    uint64 lhsSizeCovered = (lhsSize / alignUsed) * alignUsed;
    uint32 newAlignUsed = alignUsed;
    while (lhsSizeCovered < lhsSize) {
      newAlignUsed = newAlignUsed >> 1;
      CHECK_FATAL(newAlignUsed != 0, "expect non-zero");
      if ((lhsSizeCovered + newAlignUsed) > lhsSize) {
        continue;
      }
      /* generate the load */
      AArch64OfstOperand &ofstOpnd = GetOrCreateOfstOpnd(rhsOffset + lhsSizeCovered, k32BitSize);
      Operand &rhsMemOpnd = GetOrCreateMemOpnd(AArch64MemOperand::kAddrModeBOi, newAlignUsed * k8BitSize,
                                               addrOpnd, nullptr, &ofstOpnd, nullptr);
      regno_t vRegNO = NewVReg(kRegTyInt, std::max(4u, newAlignUsed));
      RegOperand &result = CreateVirtualRegisterOperand(vRegNO);
      Insn &insn =
          GetCG()->BuildInstruction<AArch64Insn>(PickLdInsn(newAlignUsed * k8BitSize, PTY_u32), result, rhsMemOpnd);
      insn.MarkAsAccessRefField(isRefField);
      GetCurBB()->AppendInsn(insn);
      /* generate the store */
      Operand &lhsMemOpnd = GetOrCreateMemOpnd(*lhsSymbol, lhsOffset + lhsSizeCovered, newAlignUsed * k8BitSize);
      GetCurBB()->AppendInsn(
          GetCG()->BuildInstruction<AArch64Insn>(PickStInsn(newAlignUsed * k8BitSize, PTY_u32), result, lhsMemOpnd));
      lhsSizeCovered += newAlignUsed;
    }
  }
}

static MIRType *GetPointedToType(MIRPtrType &pointerType) {
  MIRType *aType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(pointerType.GetPointedTyIdx());
  if (aType->GetKind() == kTypeArray) {
    MIRArrayType *arrayType = static_cast<MIRArrayType*>(aType);
    return GlobalTables::GetTypeTable().GetTypeFromTyIdx(arrayType->GetElemTyIdx());
  }
  if (aType->GetKind() == kTypeFArray || aType->GetKind() == kTypeJArray) {
    MIRFarrayType *farrayType = static_cast<MIRFarrayType*>(aType);
    return GlobalTables::GetTypeTable().GetTypeFromTyIdx(farrayType->GetElemTyIdx());
  }
  return aType;
}

void AArch64CGFunc::SelectIassign(IassignNode &stmt) {
  int32 offset = 0;
  MIRPtrType *pointerType = static_cast<MIRPtrType*>(GlobalTables::GetTypeTable().GetTypeFromTyIdx(stmt.GetTyIdx()));
  ASSERT(pointerType != nullptr, "expect a pointer type at iassign node");
  MIRType *pointedType = nullptr;
  bool isRefField = false;
  AArch64isa::MemoryOrdering memOrd = AArch64isa::kMoNone;

  if (stmt.GetFieldID() != 0) {
    MIRType *pointedTy = GlobalTables::GetTypeTable().GetTypeFromTyIdx(pointerType->GetPointedTyIdx());
    MIRStructType *structType = nullptr;
    if (pointedTy->GetKind() != kTypeJArray) {
      structType = static_cast<MIRStructType*>(pointedTy);
    } else {
      /* it's a Jarray type. using it's parent's field info: java.lang.Object */
      structType = static_cast<MIRJarrayType*>(pointedTy)->GetParentType();
    }
    ASSERT(structType != nullptr, "SelectIassign: non-zero fieldID for non-structure");
    pointedType = structType->GetFieldType(stmt.GetFieldID());
    offset = GetBecommon().GetFieldOffset(*structType, stmt.GetFieldID()).first;
    isRefField = GetBecommon().IsRefField(*structType, stmt.GetFieldID());
  } else {
    pointedType = GetPointedToType(*pointerType);
    if (GetFunction().IsJava() && (pointedType->GetKind() == kTypePointer)) {
      MIRType *nextPointedType =
          GlobalTables::GetTypeTable().GetTypeFromTyIdx(static_cast<MIRPtrType*>(pointedType)->GetPointedTyIdx());
      if (nextPointedType->GetKind() != kTypeScalar) {
        isRefField = true;  /* write into an object array or a high-dimensional array */
      }
    }
    if (pointedType->GetPrimType() == PTY_agg) {
      maple::logInfo.MapleLogger(kLlErr) << "Error: cannot find field in " <<
          GlobalTables::GetStrTable().GetStringFromStrIdx(pointedType->GetNameStrIdx()) << '\n';
      exit(-1);
    }
  }

  PrimType styp = stmt.GetRHS()->GetPrimType();
  Operand *valOpnd = HandleExpr(stmt, *stmt.GetRHS());
  Operand &srcOpnd = LoadIntoRegister(*valOpnd, IsPrimitiveInteger(styp), GetPrimTypeBitSize(styp));

  PrimType destType = pointedType->GetPrimType();
  if (destType == PTY_agg) {
    destType = PTY_a64;
  }
  ASSERT(stmt.Opnd(0) != nullptr, "null ptr check");
  MemOperand &memOpnd = CreateMemOpnd(destType, stmt, *stmt.Opnd(0), offset);
  if (isVolStore && static_cast<AArch64MemOperand&>(memOpnd).GetAddrMode() == AArch64MemOperand::kAddrModeBOi) {
    memOrd = AArch64isa::kMoRelease;
    isVolStore = false;
  }

  if (memOrd == AArch64isa::kMoNone) {
    SelectCopy(memOpnd, destType, srcOpnd, destType);
  } else {
    AArch64CGFunc::SelectStoreRelease(memOpnd, destType, srcOpnd, destType, memOrd, false);
  }
  GetCurBB()->GetLastInsn()->MarkAsAccessRefField(isRefField);
}

void AArch64CGFunc::SelectAggIassign(IassignNode &stmt, Operand &AddrOpnd) {
  ASSERT(stmt.Opnd(0) != nullptr, "null ptr check");
  Operand &lhsAddrOpnd = LoadIntoRegister(AddrOpnd, stmt.Opnd(0)->GetPrimType());
  int32 lhsOffset = 0;
  MIRPtrType *lhsPointerType =
      static_cast<MIRPtrType*>(GlobalTables::GetTypeTable().GetTypeFromTyIdx(stmt.GetTyIdx()));
  MIRType *lhsType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(lhsPointerType->GetPointedTyIdx());
  if (stmt.GetFieldID() != 0) {
    MIRStructType *structType = static_cast<MIRStructType*>(lhsType);
    ASSERT(structType != nullptr, "SelectAggIassign: non-zero fieldID for non-structure");
    lhsType = structType->GetFieldType(stmt.GetFieldID());
    lhsOffset = GetBecommon().GetFieldOffset(*structType, stmt.GetFieldID()).first;
  } else if (lhsType->GetKind() == kTypeArray) {
    MIRArrayType *arrayLhsType = static_cast<MIRArrayType*>(lhsType);
    /* access an array element */
    lhsType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(arrayLhsType->GetElemTyIdx());
#if DEBUG
    MIRTypeKind typeKind = lhsType->GetKind();
    ASSERT(((typeKind == kTypeScalar) || (typeKind == kTypeStruct) || (typeKind == kTypeClass) ||
            (typeKind == kTypePointer)),
           "unexpected array element type in iassign");
#endif
  } else if (lhsType->GetKind() == kTypeFArray) {
    MIRFarrayType *farrayLhsType = static_cast<MIRFarrayType*>(lhsType);
    /* access an array element */
    lhsType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(farrayLhsType->GetElemTyIdx());
#if DEBUG
    MIRTypeKind typeKind = lhsType->GetKind();
    ASSERT(((typeKind == kTypeScalar) || (typeKind == kTypeStruct) || (typeKind == kTypeClass) ||
            (typeKind == kTypePointer)),
           "unexpected array element type in iassign");
#endif
  }
  uint32 lhsAlign = GetBecommon().GetTypeAlign(lhsType->GetTypeIndex());
  uint64 lhsSize = GetBecommon().GetTypeSize(lhsType->GetTypeIndex());

  uint32 rhsAlign;
  uint32 alignUsed;
  int32 rhsOffset = 0;
  if (stmt.GetRHS()->GetOpCode() == OP_dread) {
    AddrofNode *rhsDread = static_cast<AddrofNode*>(stmt.GetRHS());
    MIRSymbol *rhsSymbol = GetFunction().GetLocalOrGlobalSymbol(rhsDread->GetStIdx());
    MIRType *rhsType = rhsSymbol->GetType();
    if (rhsDread->GetFieldID() != 0) {
      MIRStructType *structType = static_cast<MIRStructType*>(rhsSymbol->GetType());
      ASSERT(structType != nullptr, "SelectAggIassign: non-zero fieldID for non-structure");
      rhsType = structType->GetFieldType(rhsDread->GetFieldID());
      rhsOffset = GetBecommon().GetFieldOffset(*structType, rhsDread->GetFieldID()).first;
    }
    rhsAlign = GetBecommon().GetTypeAlign(rhsType->GetTypeIndex());
    alignUsed = std::min(lhsAlign, rhsAlign);
    ASSERT(alignUsed != 0, "expect non-zero");
    for (uint32 i = 0; i < (lhsSize / alignUsed); ++i) {
      /* generate the load */
      Operand &rhsMemOpnd = GetOrCreateMemOpnd(*rhsSymbol, rhsOffset + i * alignUsed, alignUsed * k8BitSize);
      regno_t vRegNO = NewVReg(kRegTyInt, std::max(4u, alignUsed));
      RegOperand &result = CreateVirtualRegisterOperand(vRegNO);
      GetCurBB()->AppendInsn(
          GetCG()->BuildInstruction<AArch64Insn>(PickLdInsn(alignUsed * k8BitSize, PTY_u32), result, rhsMemOpnd));
      /* generate the store */
      AArch64OfstOperand &ofstOpnd = GetOrCreateOfstOpnd(lhsOffset + i * alignUsed, k32BitSize);
      Operand &lhsMemOpnd = GetOrCreateMemOpnd(AArch64MemOperand::kAddrModeBOi, alignUsed * k8BitSize,
          static_cast<AArch64RegOperand*>(&lhsAddrOpnd), nullptr, &ofstOpnd, nullptr);
      GetCurBB()->AppendInsn(
          GetCG()->BuildInstruction<AArch64Insn>(PickStInsn(alignUsed * k8BitSize, PTY_u32), result, lhsMemOpnd));
    }
    /* take care of extra content at the end less than the unit of alignUsed */
    uint64 lhsSizeCovered = (lhsSize / alignUsed) * alignUsed;
    uint32 newAlignUsed = alignUsed;
    while (lhsSizeCovered < lhsSize) {
      newAlignUsed = newAlignUsed >> 1;
      CHECK_FATAL(newAlignUsed != 0, "expect non-zero");
      if ((lhsSizeCovered + newAlignUsed) > lhsSize) {
        continue;
      }
      /* generate the load */
      Operand &rhsMemOpnd = GetOrCreateMemOpnd(*rhsSymbol, rhsOffset + lhsSizeCovered, newAlignUsed * k8BitSize);
      regno_t vRegNO = NewVReg(kRegTyInt, std::max(4u, newAlignUsed));
      Operand &result = CreateVirtualRegisterOperand(vRegNO);
      GetCurBB()->AppendInsn(
          GetCG()->BuildInstruction<AArch64Insn>(PickLdInsn(newAlignUsed * k8BitSize, PTY_u32), result, rhsMemOpnd));
      /* generate the store */
      AArch64OfstOperand &ofstOpnd = GetOrCreateOfstOpnd(lhsOffset + lhsSizeCovered, k32BitSize);
      Operand &lhsMemOpnd = GetOrCreateMemOpnd(AArch64MemOperand::kAddrModeBOi, newAlignUsed * k8BitSize,
          static_cast<AArch64RegOperand*>(&lhsAddrOpnd), nullptr, &ofstOpnd, static_cast<MIRSymbol*>(nullptr));
      GetCurBB()->AppendInsn(
          GetCG()->BuildInstruction<AArch64Insn>(PickStInsn(newAlignUsed * k8BitSize, PTY_u32), result, lhsMemOpnd));
      lhsSizeCovered += newAlignUsed;
    }
  } else {  /* rhs is iread */
    ASSERT(stmt.GetRHS()->GetOpCode() == OP_iread, "SelectAggDassign: NYI");
    IreadNode *rhsIread = static_cast<IreadNode*>(stmt.GetRHS());
    RegOperand *rhsAddrOpnd = static_cast<RegOperand*>(HandleExpr(*rhsIread, *rhsIread->Opnd(0)));
    rhsAddrOpnd = &LoadIntoRegister(*rhsAddrOpnd, rhsIread->Opnd(0)->GetPrimType());
    MIRPtrType *rhsPointerType =
        static_cast<MIRPtrType*>(GlobalTables::GetTypeTable().GetTypeFromTyIdx(rhsIread->GetTyIdx()));
    MIRType *rhsType = static_cast<MIRStructType*>(
        GlobalTables::GetTypeTable().GetTypeFromTyIdx(rhsPointerType->GetPointedTyIdx()));
    bool isRefField = false;
    if (rhsIread->GetFieldID() != 0) {
      MIRStructType *rhsStructType = static_cast<MIRStructType*>(rhsType);
      ASSERT(rhsStructType, "SelectAggDassign: non-zero fieldID for non-structure");
      rhsType = rhsStructType->GetFieldType(rhsIread->GetFieldID());
      rhsOffset = GetBecommon().GetFieldOffset(*rhsStructType, rhsIread->GetFieldID()).first;
      isRefField = GetBecommon().IsRefField(*rhsStructType, rhsIread->GetFieldID());
    }
    rhsAlign = GetBecommon().GetTypeAlign(rhsType->GetTypeIndex());
    alignUsed = std::min(lhsAlign, rhsAlign);
    ASSERT(alignUsed != 0, "expect non-zero");
    for (uint32 i = 0; i < (lhsSize / alignUsed); i++) {
      /* generate the load */
      AArch64OfstOperand &rhsOfstOpnd = GetOrCreateOfstOpnd(rhsOffset + i * alignUsed, k32BitSize);
      Operand &rhsMemOpnd = GetOrCreateMemOpnd(AArch64MemOperand::kAddrModeBOi, alignUsed * k8BitSize,
          static_cast<AArch64RegOperand*>(rhsAddrOpnd), nullptr, &rhsOfstOpnd, nullptr);
      regno_t vRegNO = NewVReg(kRegTyInt, std::max(4u, alignUsed));
      RegOperand &result = CreateVirtualRegisterOperand(vRegNO);
      Insn &insn =
          GetCG()->BuildInstruction<AArch64Insn>(PickLdInsn(alignUsed * k8BitSize, PTY_u32), result, rhsMemOpnd);
      insn.MarkAsAccessRefField(isRefField);
      GetCurBB()->AppendInsn(insn);
      /* generate the store */
      AArch64OfstOperand &lhsOfstOpnd = GetOrCreateOfstOpnd(lhsOffset + i * alignUsed, k32BitSize);
      Operand &lhsMemOpnd = GetOrCreateMemOpnd(AArch64MemOperand::kAddrModeBOi, alignUsed * k8BitSize,
          static_cast<AArch64RegOperand*>(&lhsAddrOpnd), nullptr, &lhsOfstOpnd, nullptr);
      GetCurBB()->AppendInsn(
          GetCG()->BuildInstruction<AArch64Insn>(PickStInsn(alignUsed * k8BitSize, PTY_u32), result, lhsMemOpnd));
    }
    /* take care of extra content at the end less than the unit of alignUsed */
    uint64 lhsSizeCovered = (lhsSize / alignUsed) * alignUsed;
    uint32 newAlignUsed = alignUsed;
    while (lhsSizeCovered < lhsSize) {
      newAlignUsed = newAlignUsed >> 1;
      CHECK_FATAL(newAlignUsed != 0, "expect non-zero");
      if ((lhsSizeCovered + newAlignUsed) > lhsSize) {
        continue;
      }
      /* generate the load */
      AArch64OfstOperand &rhsOfstOpnd = GetOrCreateOfstOpnd(rhsOffset + lhsSizeCovered, k32BitSize);
      Operand &rhsMemOpnd = GetOrCreateMemOpnd(AArch64MemOperand::kAddrModeBOi, newAlignUsed * k8BitSize,
          static_cast<AArch64RegOperand*>(rhsAddrOpnd), nullptr, &rhsOfstOpnd, nullptr);
      regno_t vRegNO = NewVReg(kRegTyInt, std::max(4u, newAlignUsed));
      RegOperand &result = CreateVirtualRegisterOperand(vRegNO);
      Insn &insn =
          GetCG()->BuildInstruction<AArch64Insn>(PickLdInsn(newAlignUsed * k8BitSize, PTY_u32), result, rhsMemOpnd);
      insn.MarkAsAccessRefField(isRefField);
      GetCurBB()->AppendInsn(insn);
      /* generate the store */
      AArch64OfstOperand &lhsOfstOpnd = GetOrCreateOfstOpnd(lhsOffset + lhsSizeCovered, k32BitSize);
      Operand &lhsMemOpnd = GetOrCreateMemOpnd(AArch64MemOperand::kAddrModeBOi, newAlignUsed * k8BitSize,
          static_cast<AArch64RegOperand*>(&lhsAddrOpnd), nullptr, &lhsOfstOpnd, nullptr);
      GetCurBB()->AppendInsn(
          GetCG()->BuildInstruction<AArch64Insn>(PickStInsn(newAlignUsed * k8BitSize, PTY_u32), result, lhsMemOpnd));
      lhsSizeCovered += newAlignUsed;
    }
  }
}

Operand *AArch64CGFunc::SelectDread(DreadNode &expr) {
  MIRSymbol *symbol = GetFunction().GetLocalOrGlobalSymbol(expr.GetStIdx());
  if (symbol->IsEhIndex()) {
    MIRType *type = GlobalTables::GetTypeTable().GetTypeFromTyIdx((TyIdx)PTY_i32);
    /* use the second register return by __builtin_eh_return(). */
    ReturnMechanism retMech(*type, GetBecommon());
    retMech.SetupSecondRetReg(*type);
    return &GetOrCreatePhysicalRegisterOperand(retMech.GetReg1(), k64BitSize, kRegTyInt);
  }

  PrimType symType = symbol->GetType()->GetPrimType();
  int32 offset = 0;
  if (expr.GetFieldID() != 0) {
    MIRStructType *structType = static_cast<MIRStructType*>(symbol->GetType());
    ASSERT(structType != nullptr, "SelectDread: non-zero fieldID for non-structure");
    symType = structType->GetFieldType(expr.GetFieldID())->GetPrimType();
    offset = GetBecommon().GetFieldOffset(*structType, expr.GetFieldID()).first;
  }
  CHECK_FATAL(symType != PTY_agg, "dread type error");
  uint32 dataSize = GetPrimTypeSize(symType) * kBitsPerByte;
  MemOperand &memOpnd = GetOrCreateMemOpnd(*symbol, offset, dataSize);
  if ((memOpnd.GetMemVaryType() == kNotVary) &&
      IsImmediateOffsetOutOfRange(static_cast<AArch64MemOperand&>(memOpnd), dataSize)) {
    return &SplitOffsetWithAddInstruction(static_cast<AArch64MemOperand&>(memOpnd), dataSize);
  }
  return &memOpnd;
}

RegOperand *AArch64CGFunc::SelectRegread(RegreadNode &expr) {
  PregIdx pregIdx = expr.GetRegIdx();
  if (IsSpecialPseudoRegister(pregIdx)) {
    /* if it is one of special registers */
    return &GetOrCreateSpecialRegisterOperand(-pregIdx, expr.GetPrimType());
  }
  RegOperand &reg = GetOrCreateVirtualRegisterOperand(GetVirtualRegNOFromPseudoRegIdx(pregIdx));
  if (Globals::GetInstance()->GetOptimLevel() == 0) {
    MemOperand *src = GetPseudoRegisterSpillMemoryOperand(pregIdx);
    PrimType stype = GetTypeFromPseudoRegIdx(pregIdx);
    MIRPreg *preg = GetFunction().GetPregTab()->PregFromPregIdx(pregIdx);
    uint32 srcBitLength = GetPrimTypeSize(preg->GetPrimType()) * kBitsPerByte;
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(PickLdInsn(srcBitLength, stype), reg, *src));
  }
  return &reg;
}

void AArch64CGFunc::SelectAddrof(Operand &result, StImmOperand &stImm) {
  const MIRSymbol *symbol = stImm.GetSymbol();
  if ((symbol->GetStorageClass() == kScAuto) || (symbol->GetStorageClass() == kScFormal)) {
    if (!GetCG()->IsQuiet()) {
      maple::logInfo.MapleLogger(kLlErr) <<
          "Warning: we expect AddrOf with StImmOperand is not used for local variables";
    }
    AArch64SymbolAlloc *symLoc =
        static_cast<AArch64SymbolAlloc*>(GetMemlayout()->GetSymAllocInfo(symbol->GetStIndex()));
    AArch64ImmOperand *offset = nullptr;
    if (symLoc->GetMemSegment()->GetMemSegmentKind() == kMsArgsStkPassed) {
      offset = &CreateImmOperand(GetBaseOffset(*symLoc) + stImm.GetOffset(), k64BitSize, false, kUnAdjustVary);
    } else if (symLoc->GetMemSegment()->GetMemSegmentKind() == kMsRefLocals) {
      auto it = immOpndsRequiringOffsetAdjustmentForRefloc.find(symLoc);
      if (it != immOpndsRequiringOffsetAdjustmentForRefloc.end()) {
        offset = (*it).second;
      } else {
        offset = &CreateImmOperand(GetBaseOffset(*symLoc) + stImm.GetOffset(), k64BitSize, false);
        immOpndsRequiringOffsetAdjustmentForRefloc[symLoc] = offset;
      }
    } else {
      auto it = immOpndsRequiringOffsetAdjustment.find(symLoc);
      if (it != immOpndsRequiringOffsetAdjustment.end()) {
        offset = (*it).second;
      } else {
        offset = &CreateImmOperand(GetBaseOffset(*symLoc) + stImm.GetOffset(), k64BitSize, false);
        if (symbol->GetType()->GetKind() != kTypeClass) {
          immOpndsRequiringOffsetAdjustment[symLoc] = offset;
        }
      }
    }

    SelectAdd(result, *GetBaseReg(*symLoc), *offset, PTY_u64);
    if (GetCG()->GenerateVerboseAsm()) {
      /* Add a comment */
      Insn *insn = GetCurBB()->GetLastInsn();
      std::string comm = "local/formal var: ";
      comm.append(symbol->GetName());
      insn->SetComment(comm);
    }
  } else {
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_xadrp, result, stImm));
    if (CGOptions::IsPIC() && ((symbol->GetStorageClass() == kScGlobal) || (symbol->GetStorageClass() == kScExtern))) {
      /* ldr     x0, [x0, #:got_lo12:Ljava_2Flang_2FSystem_3B_7Cout] */
      AArch64OfstOperand &offset = CreateOfstOpnd(*stImm.GetSymbol(), stImm.GetOffset(), stImm.GetRelocs());
      AArch64MemOperand &memOpnd = GetOrCreateMemOpnd(AArch64MemOperand::kAddrModeBOi, kSizeOfPtr * kBitsPerByte,
          static_cast<AArch64RegOperand*>(&result), nullptr, &offset, nullptr);
      GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_xldr, result, memOpnd));
    } else {
      GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_xadrpl12, result, result, stImm));
    }
  }
}

void AArch64CGFunc::SelectAddrof(Operand &result, AArch64MemOperand &memOpnd) {
  const MIRSymbol *symbol = memOpnd.GetSymbol();
  if (symbol->GetStorageClass() == kScAuto) {
    auto *offsetOpnd = static_cast<AArch64OfstOperand*>(memOpnd.GetOffsetImmediate());
    Operand &immOpnd = CreateImmOperand(offsetOpnd->GetOffsetValue(), PTY_u32, false);
    SelectAdd(result, *memOpnd.GetBaseRegister(), immOpnd, PTY_u32);
  } else {
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_xadrp, result, memOpnd));
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_xadrpl12, result, result, memOpnd));
  }
}

Operand *AArch64CGFunc::SelectAddrof(AddrofNode &expr) {
  MIRSymbol *symbol = GetFunction().GetLocalOrGlobalSymbol(expr.GetStIdx());
  int32 offset = 0;
  if (expr.GetFieldID() != 0) {
    MIRStructType *structType = static_cast<MIRStructType*>(symbol->GetType());
    ASSERT(structType != nullptr, "SelectAddrof: non-zero fieldID for non-structure");
    offset = GetBecommon().GetFieldOffset(*structType, expr.GetFieldID()).first;
  }
  PrimType ptype = expr.GetPrimType();
  regno_t vRegNO = NewVReg(kRegTyInt, GetPrimTypeSize(ptype));
  Operand &result = CreateVirtualRegisterOperand(vRegNO);
  if (symbol->IsReflectionClassInfo() && !symbol->IsReflectionArrayClassInfo() && !GetCG()->IsLibcore()) {
    /*
     * Turn addrof __cinf_X  into a load of _PTR__cinf_X
     * adrp    x1, _PTR__cinf_Ljava_2Flang_2FSystem_3B
     * ldr     x1, [x1, #:lo12:_PTR__cinf_Ljava_2Flang_2FSystem_3B]
     */
    std::string ptrName = NameMangler::kPtrPrefixStr + symbol->GetName();
    MIRType *ptrType = GlobalTables::GetTypeTable().GetPtr();
    symbol = GetMirModule().GetMIRBuilder()->GetOrCreateGlobalDecl(ptrName, *ptrType);
    symbol->SetStorageClass(kScFstatic);

    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_adrp_ldr, result,
                                                                  CreateStImmOperand(*symbol, 0, 0)));
    return &result;
  }

  SelectAddrof(result, CreateStImmOperand(*symbol, offset, 0));
  return &result;
}

Operand &AArch64CGFunc::SelectAddrofFunc(AddroffuncNode &expr) {
  uint32 instrSize = static_cast<uint32>(expr.SizeOfInstr());
  regno_t vRegNO = NewVReg(kRegTyInt, instrSize);
  Operand &operand = CreateVirtualRegisterOperand(vRegNO);
  MIRFunction *mirFunction = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(expr.GetPUIdx());
  SelectAddrof(operand, CreateStImmOperand(*mirFunction->GetFuncSymbol(), 0, 0));
  return operand;
}

Operand *AArch64CGFunc::SelectIread(const BaseNode &parent, IreadNode &expr) {
  int32 offset = 0;
  MIRType *type = GlobalTables::GetTypeTable().GetTypeFromTyIdx(expr.GetTyIdx());
  MIRPtrType *pointerType = static_cast<MIRPtrType*>(type);
  ASSERT(pointerType != nullptr, "expect a pointer type at iread node");
  MIRType *pointedType = nullptr;
  bool isRefField = false;
  AArch64isa::MemoryOrdering memOrd = AArch64isa::kMoNone;

  if (expr.GetFieldID() != 0) {
    MIRType *pointedTy = GlobalTables::GetTypeTable().GetTypeFromTyIdx(pointerType->GetPointedTyIdx());
    MIRStructType *structType = nullptr;
    if (pointedTy->GetKind() != kTypeJArray) {
      structType = static_cast<MIRStructType*>(pointedTy);
    } else {
      /* it's a Jarray type. using it's parent's field info: java.lang.Object */
      structType = static_cast<MIRJarrayType*>(pointedTy)->GetParentType();
    }

    ASSERT(structType != nullptr, "SelectIread: non-zero fieldID for non-structure");
    pointedType = structType->GetFieldType(expr.GetFieldID());
    offset = GetBecommon().GetFieldOffset(*structType, expr.GetFieldID()).first;
    isRefField = GetBecommon().IsRefField(*structType, expr.GetFieldID());
  } else {
    pointedType = GetPointedToType(*pointerType);
    if (GetFunction().IsJava() && (pointedType->GetKind() == kTypePointer)) {
      MIRType *nextPointedType =
          GlobalTables::GetTypeTable().GetTypeFromTyIdx(static_cast<MIRPtrType*>(pointedType)->GetPointedTyIdx());
      if (nextPointedType->GetKind() != kTypeScalar) {
        isRefField = true;  /* read from an object array, or an high-dimentional array */
      }
    }
    if (pointedType->GetPrimType() == PTY_agg) {
      maple::logInfo.MapleLogger(kLlErr) << "Error: cannot find field in " <<
          GlobalTables::GetStrTable().GetStringFromStrIdx(pointedType->GetNameStrIdx()) << '\n';
      CHECK_FATAL(false, "cannot find field");
      return nullptr;
    }
  }

  RegType regType = GetRegTyFromPrimTy(expr.GetPrimType());
  uint32 regSize = GetPrimTypeSize(expr.GetPrimType());
  if (regSize < k4ByteSize) {
    regSize = k4ByteSize;  /* 32-bit */
  }
  regno_t vRegNO;
  Operand *result = nullptr;
  if (parent.GetOpCode() == OP_eval) {
    /* regSize << 3, that is regSize * 8, change bytes to bits */
    result = &AArch64RegOperand::GetZeroRegister(regSize << 3);
  } else {
    vRegNO = NewVReg(regType, regSize);
    result = &CreateVirtualRegisterOperand(vRegNO);
  }

  PrimType destType = pointedType->GetPrimType();

  uint32 bitSize = 0;
  if ((pointedType->GetKind() == kTypeStructIncomplete) || (pointedType->GetKind() == kTypeClassIncomplete) ||
      (pointedType->GetKind() == kTypeInterfaceIncomplete)) {
    bitSize = GetPrimTypeBitSize(expr.GetPrimType());
    maple::logInfo.MapleLogger(kLlErr) << "Warning: objsize is zero! \n";
  } else {
    if (pointedType->IsStructType()) {
      MIRStructType *structType = static_cast<MIRStructType*>(pointedType);
      /* size << 3, that is size * 8, change bytes to bits */
      bitSize = structType->GetSize() << 3;
    } else {
      bitSize = GetPrimTypeBitSize(destType);
    }
  }

  MemOperand *memOpnd = &CreateMemOpnd(destType, expr, *expr.Opnd(0), offset, memOrd);
  if (isVolLoad && (static_cast<AArch64MemOperand*>(memOpnd)->GetAddrMode() == AArch64MemOperand::kAddrModeBOi)) {
    memOrd = AArch64isa::kMoAcquire;
    isVolLoad = false;
  }

  if ((memOpnd->GetMemVaryType() == kNotVary) &&
      IsImmediateOffsetOutOfRange(*static_cast<AArch64MemOperand*>(memOpnd), bitSize)) {
    memOpnd = &SplitOffsetWithAddInstruction(*static_cast<AArch64MemOperand*>(memOpnd), bitSize);
  }

  if (memOrd == AArch64isa::kMoNone) {
    MOperator mOp = PickLdInsn(bitSize, destType);
    Insn &insn = GetCG()->BuildInstruction<AArch64Insn>(mOp, *result, *memOpnd);
    if (parent.GetOpCode() == OP_eval && result->IsRegister() &&
        static_cast<AArch64RegOperand*>(result)->IsZeroRegister()) {
      insn.SetComment("null-check");
    }
    GetCurBB()->AppendInsn(insn);
  } else {
    AArch64CGFunc::SelectLoadAcquire(*result, destType, *memOpnd, destType, memOrd, false);
  }
  GetCurBB()->GetLastInsn()->MarkAsAccessRefField(isRefField);
  return result;
}

Operand *AArch64CGFunc::SelectIntConst(MIRIntConst &intConst) {
  return &CreateImmOperand(intConst.GetValue(), GetPrimTypeSize(intConst.GetType().GetPrimType()) * kBitsPerByte,
                           false);
}

template <typename T>
Operand *SelectLiteral(T *c, MIRFunction *func, uint32 labelIdx, AArch64CGFunc *cgFunc) {
  MIRSymbol *st = func->GetSymTab()->CreateSymbol(kScopeLocal);
  std::string lblStr(".LB_");
  MIRSymbol *funcSt = GlobalTables::GetGsymTable().GetSymbolFromStidx(func->GetStIdx().Idx());
  std::string funcName = funcSt->GetName();
  lblStr.append(funcName).append(std::to_string(labelIdx));
  st->SetNameStrIdx(lblStr);
  st->SetStorageClass(kScPstatic);
  st->SetSKind(kStConst);
  st->SetKonst(c);
  PrimType primType = c->GetType().GetPrimType();
  st->SetTyIdx(TyIdx(primType));
  uint32 typeBitSize = GetPrimTypeBitSize(primType);

  if ((c->GetPrimType() == PTY_f32) || (c->GetPrimType() == PTY_f64)) {
    return c->IsZero() ? static_cast<Operand*>(&cgFunc->GetOrCreateFpZeroOperand(typeBitSize))
                       : static_cast<Operand*>(&cgFunc->GetOrCreateMemOpnd(*st, 0, typeBitSize));
  }
  CHECK_FATAL(false, "Unsupported const type");
  return nullptr;
}

Operand *AArch64CGFunc::SelectFloatConst(MIRFloatConst &floatConst) {
  uint32 labelIdxTmp = GetLabelIdx();
  Operand *result = SelectLiteral(&floatConst, &GetFunction(), labelIdxTmp++, this);
  SetLabelIdx(labelIdxTmp);
  return result;
}

Operand *AArch64CGFunc::SelectDoubleConst(MIRDoubleConst &doubleConst) {
  uint32 labelIdxTmp = GetLabelIdx();
  Operand *result = SelectLiteral(&doubleConst, &GetFunction(), labelIdxTmp++, this);
  SetLabelIdx(labelIdxTmp);
  return result;
}

template <typename T>
Operand *SelectStrLiteral(T &c, AArch64CGFunc &cgFunc) {
  std::string labelStr;
  if (c.GetKind() == kConstStrConst) {
    labelStr.append(".LUstr_");
  } else if (c.GetKind() == kConstStr16Const) {
    labelStr.append(".LUstr16_");
  } else {
    CHECK_FATAL(false, "Unsupported literal type");
  }
  labelStr.append(std::to_string(c.GetValue()));

  MIRSymbol *labelSym = GlobalTables::GetGsymTable().GetSymbolFromStrIdx(
    GlobalTables::GetStrTable().GetStrIdxFromName(labelStr));
  if (labelSym == nullptr) {
    labelSym = cgFunc.GetMirModule().GetMIRBuilder()->CreateGlobalDecl(labelStr, c.GetType());
    labelSym->SetStorageClass(kScFstatic);
    labelSym->SetSKind(kStConst);
    /* c may be local, we need a global node here */
    labelSym->SetKonst(cgFunc.NewMirConst(c));
  }

  if (c.GetPrimType() == PTY_ptr) {
    StImmOperand &stOpnd = cgFunc.CreateStImmOperand(*labelSym, 0, 0);
    RegOperand &addrOpnd = cgFunc.CreateRegisterOperandOfType(PTY_a64);
    cgFunc.SelectAddrof(addrOpnd, stOpnd);
    return &addrOpnd;
  }
  CHECK_FATAL(false, "Unsupported const string type");
  return nullptr;
}

Operand *AArch64CGFunc::SelectStrConst(MIRStrConst &strConst) {
  return SelectStrLiteral(strConst, *this);
}

Operand *AArch64CGFunc::SelectStr16Const(MIRStr16Const &str16Const) {
  return SelectStrLiteral(str16Const, *this);
}

static inline void AppendInstructionTo(Insn &i, CGFunc &f) {
  f.GetCurBB()->AppendInsn(i);
}

/*
 * Returns the number of leading 0-bits in x, starting at the most significant bit position.
 * If x is 0, the result is -1.
 */
static int32 GetHead0BitNum(int64 val) {
  uint32 bitNum = 0;
  for (; bitNum < k64BitSize; bitNum++) {
    if ((0x8000000000000000ULL >> static_cast<uint32>(bitNum)) & static_cast<uint64>(val)) {
      break;
    }
  }
  if (bitNum == k64BitSize) {
    return -1;
  }
  return bitNum;
}

/*
 * Returns the number of trailing 0-bits in x, starting at the least significant bit position.
 * If x is 0, the result is -1.
 */
static int32 GetTail0BitNum(int64 val) {
  uint32 bitNum = 0;
  for (; bitNum < k64BitSize; bitNum++) {
    if ((static_cast<uint64>(1) << static_cast<uint32>(bitNum)) & static_cast<uint64>(val)) {
      break;
    }
  }
  if (bitNum == k64BitSize) {
    return -1;
  }
  return bitNum;
}

/*
 * If the input integer is power of 2, return log2(input)
 * else return -1
 */
static inline int32 IsPowerOf2(int64 val) {
  if (__builtin_popcountll(val) == 1) {
    return __builtin_ffsll(val) - 1;
  }
  return -1;
}

MOperator AArch64CGFunc::PickJmpInsn(Opcode brOp, Opcode cmpOp, bool isFloat, bool isSigned) {
  switch (cmpOp) {
    case OP_ne:
      return (brOp == OP_brtrue) ? MOP_bne : MOP_beq;
    case OP_eq:
      return (brOp == OP_brtrue) ? MOP_beq : MOP_bne;
    case OP_lt:
      return (brOp == OP_brtrue) ? (isSigned ? MOP_blt : MOP_blo)
                                 : (isFloat ? MOP_bpl : (isSigned ? MOP_bge : MOP_bhs));
    case OP_le:
      return (brOp == OP_brtrue) ? (isSigned ? MOP_ble : MOP_bls)
                                 : (isFloat ? MOP_bhi : (isSigned ? MOP_bgt : MOP_bhi));
    case OP_gt:
      return (brOp == OP_brtrue) ? (isFloat ? MOP_bhi : (isSigned ? MOP_bgt : MOP_bhi))
                                 : (isSigned ? MOP_ble : MOP_bls);
    case OP_ge:
      return (brOp == OP_brtrue) ? (isFloat ? MOP_bpl : (isSigned ? MOP_bge : MOP_bhs))
                                 : (isSigned ? MOP_blt : MOP_blo);
    default:
      CHECK_FATAL(false, "PickJmpInsn error");
  }
}

bool AArch64CGFunc::GenerateCompareWithZeroInstruction(Opcode jmpOp, Opcode cmpOp, bool is64Bits,
                                                       LabelOperand &targetOpnd, Operand &opnd0) {
  bool finish = true;
  MOperator mOpCode = MOP_undef;
  switch (cmpOp) {
    case OP_ne: {
      if (jmpOp == OP_brtrue) {
        mOpCode = is64Bits ? MOP_xcbnz : MOP_wcbnz;
      } else {
        mOpCode = is64Bits ? MOP_xcbz : MOP_wcbz;
      }
      GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOpCode, opnd0, targetOpnd));
      break;
    }
    case OP_eq: {
      if (jmpOp == OP_brtrue) {
        mOpCode = is64Bits ? MOP_xcbz : MOP_wcbz;
      } else {
        mOpCode = is64Bits ? MOP_xcbnz : MOP_wcbnz;
      }
      GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOpCode, opnd0, targetOpnd));
      break;
    }
    /*
     * TBZ/TBNZ instruction have a range of +/-32KB, need to check if the jump target is reachable in a later
     * phase. If the branch target is not reachable, then we change tbz/tbnz into combination of ubfx and
     * cbz/cbnz, which will clobber one extra register. With LSRA under O2, we can use of the reserved registers
     * for that purpose.
     */
    case OP_lt: {
      ImmOperand &signBit = CreateImmOperand(is64Bits ? kHighestBitOf64Bits : kHighestBitOf32Bits, k8BitSize, false);
      if (jmpOp == OP_brtrue) {
        mOpCode = is64Bits ? MOP_xtbnz : MOP_wtbnz;
      } else {
        mOpCode = is64Bits ? MOP_xtbz : MOP_wtbz;
      }
      GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOpCode, opnd0, signBit, targetOpnd));
      break;
    }
    case OP_ge: {
      ImmOperand &signBit = CreateImmOperand(is64Bits ? kHighestBitOf64Bits : kHighestBitOf32Bits, k8BitSize, false);
      if (jmpOp == OP_brtrue) {
        mOpCode = is64Bits ? MOP_xtbz : MOP_wtbz;
      } else {
        mOpCode = is64Bits ? MOP_xtbnz : MOP_wtbnz;
      }
      GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOpCode, opnd0, signBit, targetOpnd));
      break;
    }
    default:
      finish = false;
      break;
  }
  return finish;
}

void AArch64CGFunc::SelectCondGoto(LabelOperand &targetOpnd, Opcode jmpOp, Opcode cmpOp, Operand &origOpnd0,
                                   Operand &origOpnd1, PrimType primType) {
  Operand *opnd0 = &origOpnd0;
  Operand *opnd1 = &origOpnd1;
  if ((Globals::GetInstance()->GetOptimLevel() > 0) &&
      opnd0->IsIntImmediate() &&
      static_cast<AArch64ImmOperand*>(opnd0)->IsZero() &&
      (!opnd1->IsIntImmediate() || static_cast<AArch64ImmOperand*>(opnd1)->IsZero())) {
    opnd0 = (opnd0->GetSize() <= k32BitSize) ? &AArch64RegOperand::Get32bitZeroRegister()
                                             : &AArch64RegOperand::Get64bitZeroRegister();
  } else {
    opnd0 = &LoadIntoRegister(origOpnd0, primType);
  }

  bool is64Bits = GetPrimTypeBitSize(primType) == k64BitSize;
  bool isFloat = IsPrimitiveFloat(primType);
  Operand &rflag = GetOrCreateRflag();
  if (isFloat) {
    opnd1 = &LoadIntoRegister(origOpnd1, primType);
    MOperator mOp = is64Bits ? MOP_dcmperr : ((GetPrimTypeBitSize(primType) == k32BitSize) ? MOP_scmperr : MOP_hcmperr);
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, rflag, *opnd0, *opnd1));
  } else {
    bool isImm = ((origOpnd1.GetKind() == Operand::kOpdImmediate) || (origOpnd1.GetKind() == Operand::kOpdOffset));
    if ((origOpnd1.GetKind() != Operand::kOpdRegister) && !isImm) {
      opnd1 = &SelectCopy(origOpnd1, primType, primType);
    }
    MOperator mOp = is64Bits ? MOP_xcmprr : MOP_wcmprr;

    if (isImm) {
      /* Special cases, i.e., comparing with zero */
      if (static_cast<AArch64ImmOperand*>(opnd1)->IsZero() && (Globals::GetInstance()->GetOptimLevel() > 0)) {
        bool finish = GenerateCompareWithZeroInstruction(jmpOp, cmpOp, is64Bits, targetOpnd, *opnd0);
        if (finish) {
          return;
        }
      }

      /*
       * aarch64 assembly takes up to 24-bits immediate, generating
       * either cmp or cmp with shift 12 encoding
       */
      ImmOperand *immOpnd = static_cast<ImmOperand*>(opnd1);
      if (immOpnd->IsInBitSize(kMaxAarch64ImmVal12Bits) ||
          immOpnd->IsInBitSize(kMaxAarch64ImmVal12Bits, kMaxAarch64ImmVal12Bits)) {
        mOp = is64Bits ? MOP_xcmpri : MOP_wcmpri;
      } else {
        opnd1 = &SelectCopy(*opnd1, primType, primType);
      }
    }
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, rflag, *opnd0, *opnd1));
  }

  MOperator jmpOperator = PickJmpInsn(jmpOp, cmpOp, isFloat, IsSignedInteger(primType));
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(jmpOperator, rflag, targetOpnd));
}

/*
 *   brtrue @label0 (ge u8 i32 (
 *   cmp i32 i64 (dread i64 %Reg2_J, dread i64 %Reg4_J),
 *   constval i32 0))
 *  ===>
 *   cmp r1, r2
 *   bge Cond, label0
 */
void AArch64CGFunc::SelectCondSpecialCase1(CondGotoNode &stmt, BaseNode &expr) {
  ASSERT(expr.GetOpCode() == OP_cmp, "unexpect opcode");
  Operand *opnd0 = HandleExpr(expr, *expr.Opnd(0));
  Operand *opnd1 = HandleExpr(expr, *expr.Opnd(1));
  CompareNode *node = static_cast<CompareNode*>(&expr);
  bool isFloat = IsPrimitiveFloat(node->GetOpndType());
  opnd0 = &LoadIntoRegister(*opnd0, node->GetOpndType());
  /*
   * most of FP constants are passed as AArch64MemOperand
   * except 0.0 which is passed as kOpdFPZeroImmediate
   */
  Operand::OperandType opnd1Type = opnd1->GetKind();
  if ((opnd1Type != Operand::kOpdImmediate) && (opnd1Type != Operand::kOpdFPZeroImmediate) &&
      (opnd1Type != Operand::kOpdOffset)) {
    opnd1 = &LoadIntoRegister(*opnd1, node->GetOpndType());
  }
  SelectAArch64Cmp(*opnd0, *opnd1, !isFloat, GetPrimTypeBitSize(node->GetOpndType()));
  /* handle condgoto now. */
  LabelIdx labelIdx = stmt.GetOffset();
  BaseNode *condNode = stmt.Opnd(0);
  LabelOperand &targetOpnd = GetOrCreateLabelOperand(labelIdx);
  Opcode cmpOp = condNode->GetOpCode();
  PrimType pType = static_cast<CompareNode*>(condNode)->GetOpndType();
  isFloat = IsPrimitiveFloat(pType);
  Operand &rflag = GetOrCreateRflag();
  MOperator jmpOp = PickJmpInsn(stmt.GetOpCode(), cmpOp, isFloat, IsSignedInteger(pType));
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(jmpOp, rflag, targetOpnd));
}

/*
 * Special case:
 * brfalse(ge (cmpg (op0, op1), 0) ==>
 * fcmp op1, op2
 * blo
 */
void AArch64CGFunc::SelectCondSpecialCase2(const CondGotoNode &stmt, BaseNode &expr) {
  auto &cmpNode = static_cast<CompareNode&>(expr);
  Operand *opnd0 = HandleExpr(cmpNode, *cmpNode.Opnd(0));
  Operand *opnd1 = HandleExpr(cmpNode, *cmpNode.Opnd(1));
  PrimType operandType = cmpNode.GetOpndType();
  opnd0 = opnd0->IsRegister() ? static_cast<RegOperand*>(opnd0)
                              : &SelectCopy(*opnd0, operandType, operandType);
  Operand::OperandType opnd1Type = opnd1->GetKind();
  if ((opnd1Type != Operand::kOpdImmediate) && (opnd1Type != Operand::kOpdFPZeroImmediate) &&
      (opnd1Type != Operand::kOpdOffset)) {
    opnd1 = opnd1->IsRegister() ? static_cast<RegOperand*>(opnd1)
                                : &SelectCopy(*opnd1, operandType, operandType);
  }
#ifdef DEBUG
  bool isFloat = IsPrimitiveFloat(operandType);
  if (!isFloat) {
    ASSERT(false, "incorrect operand types");
  }
#endif
  SelectTargetFPCmpQuiet(*opnd0, *opnd1, GetPrimTypeBitSize(operandType));
  Operand &rFlag = GetOrCreateRflag();
  LabelIdx tempLabelIdx = stmt.GetOffset();
  LabelOperand &targetOpnd = GetOrCreateLabelOperand(tempLabelIdx);
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_blo, rFlag, targetOpnd));
}

void AArch64CGFunc::SelectCondGoto(CondGotoNode &stmt, Operand &opnd0, Operand &opnd1) {
  /*
   * handle brfalse/brtrue op, opnd0 can be a compare node or non-compare node
   * such as a dread for example
   */
  LabelIdx labelIdx = stmt.GetOffset();
  BaseNode *condNode = stmt.Opnd(0);
  LabelOperand &targetOpnd = GetOrCreateLabelOperand(labelIdx);
  Opcode cmpOp;

  if (opnd0.IsRegister() && (static_cast<RegOperand*>(&opnd0)->GetValidBitsNum() == 1) &&
      (condNode->GetOpCode() == OP_lior)) {
    ImmOperand &condBit = CreateImmOperand(0, k8BitSize, false);
    if (stmt.GetOpCode() == OP_brtrue) {
      GetCurBB()->AppendInsn(
          GetCG()->BuildInstruction<AArch64Insn>(MOP_wtbnz, static_cast<RegOperand&>(opnd0), condBit, targetOpnd));
    } else {
      GetCurBB()->AppendInsn(
          GetCG()->BuildInstruction<AArch64Insn>(MOP_wtbz, static_cast<RegOperand&>(opnd0), condBit, targetOpnd));
    }
    return;
  }

  PrimType pType;
  if (kOpcodeInfo.IsCompare(condNode->GetOpCode())) {
    cmpOp = condNode->GetOpCode();
    pType = static_cast<CompareNode*>(condNode)->GetOpndType();
  } else {
    /* not a compare node; dread for example, take its pType */
    cmpOp = OP_ne;
    pType = condNode->GetPrimType();
  }

  SelectCondGoto(targetOpnd, stmt.GetOpCode(), cmpOp, opnd0, opnd1, pType);
}

void AArch64CGFunc::SelectGoto(GotoNode &stmt) {
  Operand &targetOpnd = GetOrCreateLabelOperand(stmt.GetOffset());
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_xuncond, targetOpnd));
}

Operand *AArch64CGFunc::SelectAdd(BinaryNode &node, Operand &opnd0, Operand &opnd1) {
  PrimType dtype = node.GetPrimType();
  bool isSigned = IsSignedInteger(dtype);
  uint32 dsize = GetPrimTypeBitSize(dtype);
  bool is64Bits = (dsize == k64BitSize);
  bool isFloat = IsPrimitiveFloat(dtype);
  /* promoted type */
  PrimType primType =
      isFloat ? dtype : ((is64Bits ? (isSigned ? PTY_i64 : PTY_u64) : (isSigned ? PTY_i32 : PTY_u32)));
  RegOperand &resOpnd = CreateRegisterOperandOfType(primType);
  SelectAdd(resOpnd, opnd0, opnd1, primType);
  return &resOpnd;
}

void AArch64CGFunc::SelectAdd(Operand &resOpnd, Operand &opnd0, Operand &opnd1, PrimType primType) {
  Operand::OperandType opnd0Type = opnd0.GetKind();
  Operand::OperandType opnd1Type = opnd1.GetKind();
  uint32 dsize = GetPrimTypeBitSize(primType);
  bool is64Bits = (dsize == k64BitSize);
  if (opnd0Type != Operand::kOpdRegister) {
    /* add #imm, #imm */
    if (opnd1Type != Operand::kOpdRegister) {
      SelectAdd(resOpnd, SelectCopy(opnd0, primType, primType), opnd1, primType);
      return;
    }
    /* add #imm, reg */
    SelectAdd(resOpnd, opnd1, opnd0, primType);  /* commutative */
    return;
  }
  /* add reg, reg */
  if (opnd1Type == Operand::kOpdRegister) {
    ASSERT(IsPrimitiveFloat(primType) || IsPrimitiveInteger(primType), "NYI add");
    MOperator mOp = IsPrimitiveFloat(primType) ?
        (is64Bits ? MOP_dadd : MOP_sadd) : (is64Bits ? MOP_xaddrrr : MOP_waddrrr);
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, resOpnd, opnd0, opnd1));
    return;
  } else if (!((opnd1Type == Operand::kOpdImmediate) || (opnd1Type == Operand::kOpdOffset))) {
    /* add reg, otheregType */
    SelectAdd(resOpnd, opnd0, SelectCopy(opnd1, primType, primType), primType);
    return;
  } else {
    /* add reg, #imm */
    AArch64ImmOperand *immOpnd = static_cast<AArch64ImmOperand*>(&opnd1);
    if (immOpnd->IsNegative()) {
      immOpnd->Negate();
      SelectSub(resOpnd, opnd0, *immOpnd, primType);
      return;
    }
    if (immOpnd->IsInBitSize(kMaxAarch64ImmVal24Bits)) {
      /*
       * ADD Wd|WSP, Wn|WSP, #imm{, shift} ; 32-bit general registers
       * ADD Xd|SP,  Xn|SP,  #imm{, shift} ; 64-bit general registers
       * imm : 0 ~ 4095, shift: none, LSL #0, or LSL #12
       * aarch64 assembly takes up to 24-bits, if the lower 12 bits is all 0
       */
      MOperator mOpCode = MOP_undef;
      Operand *newOpnd0 = &opnd0;
      if (!(immOpnd->IsInBitSize(kMaxAarch64ImmVal12Bits) ||
            immOpnd->IsInBitSize(kMaxAarch64ImmVal12Bits, kMaxAarch64ImmVal12Bits))) {
        /* process higher 12 bits */
        ImmOperand &immOpnd2 = CreateImmOperand(static_cast<uint64>(immOpnd->GetValue()) >> kMaxAarch64ImmVal12Bits,
                                                immOpnd->GetSize(), immOpnd->IsSignedValue());
        mOpCode = is64Bits ? MOP_xaddrri24 : MOP_waddrri24;
        Insn &newInsn = GetCG()->BuildInstruction<AArch64Insn>(mOpCode, resOpnd, opnd0, immOpnd2, addSubLslOperand);
        GetCurBB()->AppendInsn(newInsn);
        immOpnd->ModuloByPow2(kMaxAarch64ImmVal12Bits);
        newOpnd0 = &resOpnd;
      }
      /* process lower 12  bits */
      mOpCode = is64Bits ? MOP_xaddrri12 : MOP_waddrri12;
      Insn &newInsn = GetCG()->BuildInstruction<AArch64Insn>(mOpCode, resOpnd, *newOpnd0, *immOpnd);
      GetCurBB()->AppendInsn(newInsn);
      return;
    }
    /* load into register */
    int64 immVal = immOpnd->GetValue();
    int32 tail0bitNum = GetTail0BitNum(immVal);
    int32 head0bitNum = GetHead0BitNum(immVal);
    const int32 bitNum = k64BitSize - head0bitNum - tail0bitNum;
    RegOperand &regOpnd = CreateRegisterOperandOfType(primType);

    if (bitNum <= k16ValidBit) {
      int64 newImm = (static_cast<uint64>(immVal) >> static_cast<uint32>(tail0bitNum)) & 0xFFFF;
      AArch64ImmOperand &immOpnd1 = CreateImmOperand(newImm, k16BitSize, false);
      SelectCopyImm(regOpnd, immOpnd1, primType);
      uint32 mopBadd = is64Bits ? MOP_xaddrrrs : MOP_waddrrrs;
      int32 bitLen = is64Bits ? kBitLenOfShift64Bits : kBitLenOfShift32Bits;
      BitShiftOperand &bitShiftOpnd = CreateBitShiftOperand(BitShiftOperand::kLSL, tail0bitNum, bitLen);
      Insn &newInsn = GetCG()->BuildInstruction<AArch64Insn>(mopBadd, resOpnd, opnd0, regOpnd, bitShiftOpnd);
      GetCurBB()->AppendInsn(newInsn);
      return;
    }

    SelectCopyImm(regOpnd, *immOpnd, primType);
    MOperator mOpCode = is64Bits ? MOP_xaddrrr : MOP_waddrrr;
    Insn &newInsn = GetCG()->BuildInstruction<AArch64Insn>(mOpCode, resOpnd, opnd0, regOpnd);
    GetCurBB()->AppendInsn(newInsn);
  }
}

Operand &AArch64CGFunc::SelectCGArrayElemAdd(BinaryNode &node) {
  BaseNode *opnd0 = node.Opnd(0);
  BaseNode *opnd1 = node.Opnd(1);
  ASSERT(opnd0->GetOpCode() == OP_addrof, "Internal error, opnd0->op should be OP_addrof.");
  ASSERT(opnd1->GetOpCode() == OP_constval, "Internal error, opnd1->op should be OP_constval.");

  /* OP_addrof */
  AddrofNode *addrofNode = static_cast<AddrofNode*>(opnd0);
  MIRSymbol *symbol = GetFunction().GetLocalOrGlobalSymbol(addrofNode->GetStIdx());
  ASSERT(addrofNode->GetFieldID() == 0, "For debug SelectCGArrayElemAdd.");

  PrimType pType = addrofNode->GetPrimType();
  regno_t vRegNO = NewVReg(kRegTyInt, GetPrimTypeSize(pType));
  Operand &result = CreateVirtualRegisterOperand(vRegNO);

  /* OP_constval */
  ConstvalNode *constvalNode = static_cast<ConstvalNode*>(opnd1);
  MIRConst *mirConst = constvalNode->GetConstVal();
  ASSERT(mirConst != nullptr, "nullptr check");
  CHECK_FATAL(mirConst->GetKind() == kConstInt, "expect Constint Type");
  MIRIntConst *mirIntConst = safe_cast<MIRIntConst>(mirConst);
  SelectAddrof(result, CreateStImmOperand(*symbol, mirIntConst->GetValue(), 0));

  return result;
}

void AArch64CGFunc::SelectSub(Operand &resOpnd, Operand &opnd0, Operand &opnd1, PrimType primType) {
  Operand::OperandType opnd1Type = opnd1.GetKind();
  uint32 dsize = GetPrimTypeBitSize(primType);
  bool is64Bits = (dsize == k64BitSize);
  bool isFloat = IsPrimitiveFloat(primType);
  Operand *opnd0Bak = &LoadIntoRegister(opnd0, primType);
  if (opnd1Type == Operand::kOpdRegister) {
    MOperator mOp = isFloat ? (is64Bits ? MOP_dsub : MOP_ssub) : (is64Bits ? MOP_xsubrrr : MOP_wsubrrr);
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, resOpnd, *opnd0Bak, opnd1));
    return;
  }

  if ((opnd1Type != Operand::kOpdImmediate) && (opnd1Type != Operand::kOpdOffset)) {
    SelectSub(resOpnd, *opnd0Bak, SelectCopy(opnd1, primType, primType), primType);
    return;
  }

  AArch64ImmOperand *immOpnd = static_cast<AArch64ImmOperand*>(&opnd1);
  if (immOpnd->IsNegative()) {
    immOpnd->Negate();
    SelectAdd(resOpnd, *opnd0Bak, *immOpnd, primType);
    return;
  }

  if (immOpnd->IsInBitSize(kMaxAarch64ImmVal24Bits)) {
    /*
     * SUB Wd|WSP, Wn|WSP, #imm{, shift} ; 32-bit general registers
     * SUB Xd|SP,  Xn|SP,  #imm{, shift} ; 64-bit general registers
     * imm : 0 ~ 4095, shift: none, LSL #0, or LSL #12
     * aarch64 assembly takes up to 24-bits, if the lower 12 bits is all 0
     */
    MOperator mOpCode = MOP_undef;
    if (!(immOpnd->IsInBitSize(kMaxAarch64ImmVal12Bits) ||
          immOpnd->IsInBitSize(kMaxAarch64ImmVal12Bits, kMaxAarch64ImmVal12Bits))) {
      /* process higher 12 bits */
      ImmOperand &immOpnd2 = CreateImmOperand(static_cast<uint64>(immOpnd->GetValue()) >> kMaxAarch64ImmVal12Bits,
                                              immOpnd->GetSize(), immOpnd->IsSignedValue());
      mOpCode = is64Bits ? MOP_xsubrri24 : MOP_wsubrri24;
      Insn &newInsn = GetCG()->BuildInstruction<AArch64Insn>(mOpCode, resOpnd, *opnd0Bak, immOpnd2, addSubLslOperand);
      GetCurBB()->AppendInsn(newInsn);
      immOpnd->ModuloByPow2(kMaxAarch64ImmVal12Bits);
      opnd0Bak = &resOpnd;
    }
    /* process lower 12 bits */
    mOpCode = is64Bits ? MOP_xsubrri12 : MOP_wsubrri12;
    Insn &newInsn = GetCG()->BuildInstruction<AArch64Insn>(mOpCode, resOpnd, *opnd0Bak, *immOpnd);
    GetCurBB()->AppendInsn(newInsn);
    return;
  }

  /* load into register */
  int64 immVal = immOpnd->GetValue();
  int32 tail0bitNum = GetTail0BitNum(immVal);
  int32 head0bitNum = GetHead0BitNum(immVal);
  const int32 bitNum = k64BitSize - head0bitNum - tail0bitNum;
  RegOperand &regOpnd = CreateRegisterOperandOfType(primType);

  if (bitNum <= k16ValidBit) {
    int64 newImm = (static_cast<uint64>(immVal) >> static_cast<uint32>(tail0bitNum)) & 0xFFFF;
    AArch64ImmOperand &immOpnd1 = CreateImmOperand(newImm, k16BitSize, false);
    SelectCopyImm(regOpnd, immOpnd1, primType);
    uint32 mopBsub = is64Bits ? MOP_xsubrrrs : MOP_wsubrrrs;
    int32 bitLen = is64Bits ? kBitLenOfShift64Bits : kBitLenOfShift32Bits;
    BitShiftOperand &bitShiftOpnd = CreateBitShiftOperand(BitShiftOperand::kLSL, tail0bitNum, bitLen);
    GetCurBB()->AppendInsn(
        GetCG()->BuildInstruction<AArch64Insn>(mopBsub, resOpnd, *opnd0Bak, regOpnd, bitShiftOpnd));
    return;
  }

  SelectCopyImm(regOpnd, *immOpnd, primType);
  MOperator mOpCode = is64Bits ? MOP_xsubrrr : MOP_wsubrrr;
  Insn &newInsn = GetCG()->BuildInstruction<AArch64Insn>(mOpCode, resOpnd, *opnd0Bak, regOpnd);
  GetCurBB()->AppendInsn(newInsn);
}

Operand *AArch64CGFunc::SelectSub(BinaryNode &node, Operand &opnd0, Operand &opnd1) {
  PrimType dtype = node.GetPrimType();
  bool isSigned = IsSignedInteger(dtype);
  uint32 dsize = GetPrimTypeBitSize(dtype);
  bool is64Bits = (dsize == k64BitSize);
  bool isFloat = IsPrimitiveFloat(dtype);
  /* promoted type */
  PrimType primType =
      isFloat ? dtype : ((is64Bits ? (isSigned ? PTY_i64 : PTY_u64) : (isSigned ? PTY_i32 : PTY_u32)));
  RegOperand &resOpnd = CreateRegisterOperandOfType(primType);
  SelectSub(resOpnd, opnd0, opnd1, primType);
  return &resOpnd;
}

Operand *AArch64CGFunc::SelectMpy(BinaryNode &node, Operand &opnd0, Operand &opnd1) {
  PrimType dtype = node.GetPrimType();
  bool isSigned = IsSignedInteger(dtype);
  uint32 dsize = GetPrimTypeBitSize(dtype);
  bool is64Bits = (dsize == k64BitSize);
  bool isFloat = IsPrimitiveFloat(dtype);
  /* promoted type */
  PrimType primType =
      isFloat ? dtype : ((is64Bits ? (isSigned ? PTY_i64 : PTY_u64) : (isSigned ? PTY_i32 : PTY_u32)));
  RegOperand &resOpnd = CreateRegisterOperandOfType(primType);
  SelectMpy(resOpnd, opnd0, opnd1, primType);
  return &resOpnd;
}

void AArch64CGFunc::SelectMpy(Operand &resOpnd, Operand &opnd0, Operand &opnd1, PrimType primType) {
  Operand::OperandType opnd0Type = opnd0.GetKind();
  Operand::OperandType opnd1Type = opnd1.GetKind();
  uint32 dsize = GetPrimTypeBitSize(primType);
  bool is64Bits = (dsize == k64BitSize);

  if (((opnd0Type == Operand::kOpdImmediate) || (opnd0Type == Operand::kOpdOffset) ||
       (opnd1Type == Operand::kOpdImmediate) || (opnd1Type == Operand::kOpdOffset)) &&
      IsPrimitiveInteger(primType)) {
    ImmOperand *imm =
        ((opnd0Type == Operand::kOpdImmediate) || (opnd0Type == Operand::kOpdOffset)) ? static_cast<ImmOperand*>(&opnd0)
                                                                                  : static_cast<ImmOperand*>(&opnd1);
    Operand *otherOp = ((opnd0Type == Operand::kOpdImmediate) || (opnd0Type == Operand::kOpdOffset)) ? &opnd1 : &opnd0;
    int64 immValue = llabs(imm->GetValue());
    if (immValue != 0 && (static_cast<uint64>(immValue) & (static_cast<uint64>(immValue) - 1)) == 0) {
      /* immValue is 1 << n */
      if (otherOp->GetKind() != Operand::kOpdRegister) {
        otherOp = &SelectCopy(*otherOp, primType, primType);
      }
      AArch64ImmOperand &shiftNum = CreateImmOperand(__builtin_ffsll(immValue) - 1, dsize, false);
      SelectShift(resOpnd, *otherOp, shiftNum, kShiftLeft, primType);
      if (imm->GetValue() < 0) {
        SelectNeg(resOpnd, resOpnd, primType);
      }

      return;
    } else if (immValue > 2) {
      uint32 zeroNum = __builtin_ffsll(immValue) - 1;
      int64 headVal = static_cast<uint64>(immValue) >> zeroNum;
      /*
       * if (headVal + 1) & (headVal) == 0, that is (immVal >> zeroNum) + 1 == 1 << n
       *   otherOp * immVal = (otherOp * (immVal >> zeroNum) * (1 << zeroNum)
       *                    = (otherOp * ((immVal >> zeroNum) + 1) - otherOp) * (1 << zeroNum)
       */
      if (((static_cast<uint64>(headVal) + 1) & static_cast<uint64>(headVal)) == 0) {
        if (otherOp->GetKind() != Operand::kOpdRegister) {
          otherOp = &SelectCopy(*otherOp, primType, primType);
        }
        AArch64ImmOperand &shiftNum1 = CreateImmOperand(__builtin_ffsll(headVal + 1) - 1, dsize, false);
        RegOperand &tmpOpnd = CreateRegisterOperandOfType(primType);
        SelectShift(tmpOpnd, *otherOp, shiftNum1, kShiftLeft, primType);
        SelectSub(resOpnd, tmpOpnd, *otherOp, primType);
        AArch64ImmOperand &shiftNum2 = CreateImmOperand(zeroNum, dsize, false);
        SelectShift(resOpnd, resOpnd, shiftNum2, kShiftLeft, primType);
        if (imm->GetValue() < 0) {
          SelectNeg(resOpnd, resOpnd, primType);
        }

        return;
      }
      /*
       * if (headVal - 1) & (headVal - 2) == 0, that is (immVal >> zeroNum) - 1 == 1 << n
       * otherOp * immVal = (otherOp * (immVal >> zeroNum) * (1 << zeroNum)
       * = (otherOp * ((immVal >> zeroNum) - 1) + otherOp) * (1 << zeroNum)
       */
      if (((static_cast<uint64>(headVal) - 1) & (static_cast<uint64>(headVal) - 2)) == 0) {
        if (otherOp->GetKind() != Operand::kOpdRegister) {
          otherOp = &SelectCopy(*otherOp, primType, primType);
        }
        AArch64ImmOperand &shiftNum1 = CreateImmOperand(__builtin_ffsll(headVal - 1) - 1, dsize, false);
        RegOperand &tmpOpnd = CreateRegisterOperandOfType(primType);
        SelectShift(tmpOpnd, *otherOp, shiftNum1, kShiftLeft, primType);
        SelectAdd(resOpnd, tmpOpnd, *otherOp, primType);
        AArch64ImmOperand &shiftNum2 = CreateImmOperand(zeroNum, dsize, false);
        SelectShift(resOpnd, resOpnd, shiftNum2, kShiftLeft, primType);
        if (imm->GetValue() < 0) {
          SelectNeg(resOpnd, resOpnd, primType);
        }

        return;
      }
    }
  }

  if ((opnd0Type != Operand::kOpdRegister) && (opnd1Type != Operand::kOpdRegister)) {
    SelectMpy(resOpnd, SelectCopy(opnd0, primType, primType), opnd1, primType);
  } else if ((opnd0Type == Operand::kOpdRegister) && (opnd1Type != Operand::kOpdRegister)) {
    SelectMpy(resOpnd, opnd0, SelectCopy(opnd1, primType, primType), primType);
  } else if ((opnd0Type != Operand::kOpdRegister) && (opnd1Type == Operand::kOpdRegister)) {
    SelectMpy(resOpnd, opnd1, opnd0, primType);
  } else {
    ASSERT(IsPrimitiveFloat(primType) || IsPrimitiveInteger(primType), "NYI Mpy");
    MOperator mOp = IsPrimitiveFloat(primType) ? (is64Bits ? MOP_xvmuld : MOP_xvmuls)
                                               : (is64Bits ? MOP_xmulrrr : MOP_wmulrrr);
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, resOpnd, opnd0, opnd1));
  }
}

void AArch64CGFunc::SelectDiv(Operand &resOpnd, Operand &origOpnd0, Operand &opnd1, PrimType primType) {
  Operand &opnd0 = LoadIntoRegister(origOpnd0, primType);
  Operand::OperandType opnd0Type = opnd0.GetKind();
  Operand::OperandType opnd1Type = opnd1.GetKind();
  uint32 dsize = GetPrimTypeBitSize(primType);
  bool is64Bits = (dsize == k64BitSize);

  if (Globals::GetInstance()->GetOptimLevel() > 0) {
    if (((opnd1Type == Operand::kOpdImmediate) || (opnd1Type == Operand::kOpdOffset)) && IsSignedInteger(primType)) {
      ImmOperand *imm = static_cast<ImmOperand*>(&opnd1);
      int64 immValue = llabs(imm->GetValue());
      if ((immValue != 0) && (static_cast<uint64>(immValue) & (static_cast<uint64>(immValue) - 1)) == 0) {
        if (immValue == 1) {
          if (imm->GetValue() > 0) {
            uint32 mOp = is64Bits ? MOP_xmovrr : MOP_wmovrr;
            GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, resOpnd, opnd0));
          } else {
            SelectNeg(resOpnd, opnd0, primType);
          }

          return;
        }
        int32 shiftNumber = __builtin_ffsll(immValue) - 1;
        AArch64ImmOperand &shiftNum = CreateImmOperand(shiftNumber, dsize, false);
        SelectShift(resOpnd, opnd0, CreateImmOperand(dsize - 1, dsize, false), kShiftAright, primType);
        uint32 mopBadd = is64Bits ? MOP_xaddrrrs : MOP_waddrrrs;
        int32 bitLen = is64Bits ? kBitLenOfShift64Bits : kBitLenOfShift32Bits;
        BitShiftOperand &shiftOpnd = CreateBitShiftOperand(BitShiftOperand::kLSR, dsize - shiftNumber, bitLen);
        GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mopBadd, resOpnd, opnd0, resOpnd, shiftOpnd));
        SelectShift(resOpnd, resOpnd, shiftNum, kShiftAright, primType);
        if (imm->GetValue() < 0) {
          SelectNeg(resOpnd, resOpnd, primType);
        }

        return;
      }
    } else if (((opnd1Type == Operand::kOpdImmediate) || (opnd1Type == Operand::kOpdOffset)) &&
               IsUnsignedInteger(primType)) {
      ImmOperand *imm = static_cast<ImmOperand*>(&opnd1);
      if (imm->GetValue() != 0) {
        if ((imm->GetValue() > 0) &&
            ((static_cast<uint64>(imm->GetValue()) & (static_cast<uint64>(imm->GetValue()) - 1)) == 0)) {
          AArch64ImmOperand &shiftNum = CreateImmOperand(__builtin_ffsll(imm->GetValue()) - 1, dsize, false);
          SelectShift(resOpnd, opnd0, shiftNum, kShiftLright, primType);

          return;
        } else if (imm->GetValue() < 0) {
          SelectAArch64Cmp(opnd0, *imm, true, dsize);
          SelectAArch64CSet(resOpnd, GetCondOperand(CC_CS), is64Bits);

          return;
        }
      }
    }
  }

  if (opnd0Type != Operand::kOpdRegister) {
    SelectDiv(resOpnd, SelectCopy(opnd0, primType, primType), opnd1, primType);
  } else if (opnd1Type != Operand::kOpdRegister) {
    SelectDiv(resOpnd, opnd0, SelectCopy(opnd1, primType, primType), primType);
  } else {
    ASSERT(IsPrimitiveFloat(primType) || IsPrimitiveInteger(primType), "NYI Div");
    MOperator mOp = IsPrimitiveFloat(primType) ? (is64Bits ? MOP_ddivrrr : MOP_sdivrrr)
                                               : (IsSignedInteger(primType) ? (is64Bits ? MOP_xsdivrrr : MOP_wsdivrrr)
                                                                            : (is64Bits ? MOP_xudivrrr : MOP_wudivrrr));
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, resOpnd, opnd0, opnd1));
  }
}

Operand *AArch64CGFunc::SelectDiv(BinaryNode &node, Operand &opnd0, Operand &opnd1) {
  PrimType dtype = node.GetPrimType();
  bool isSigned = IsSignedInteger(dtype);
  uint32 dsize = GetPrimTypeBitSize(dtype);
  bool is64Bits = (dsize == k64BitSize);
  bool isFloat = IsPrimitiveFloat(dtype);
  /* promoted type */
  PrimType primType =
      isFloat ? dtype : ((is64Bits ? (isSigned ? PTY_i64 : PTY_u64) : (isSigned ? PTY_i32 : PTY_u32)));
  RegOperand &resOpnd = CreateRegisterOperandOfType(primType);
  SelectDiv(resOpnd, opnd0, opnd1, primType);
  return &resOpnd;
}

void AArch64CGFunc::SelectRem(Operand &resOpnd, Operand &lhsOpnd, Operand &rhsOpnd, PrimType primType, bool isSigned,
                              bool is64Bits) {
  Operand &opnd0 = LoadIntoRegister(lhsOpnd, primType);
  Operand &opnd1 = LoadIntoRegister(rhsOpnd, primType);

  ASSERT(IsPrimitiveInteger(primType), "Wrong type for REM");
 /*
  * printf("%d \n", 29 % 7 );
  * -> 1
  * printf("%u %d \n", (unsigned)-7, (unsigned)(-7) % 7 );
  * -> 4294967289 4
  * printf("%d \n", (-7) % 7 );
  * -> 0
  * printf("%d \n", 237 % -7 );
  * 6->
  * printf("implicit i->u conversion %d \n", ((unsigned)237) % -7 );
  * implicit conversion 237

  * http://stackoverflow.com/questions/35351470/obtaining-remainder-using-single-aarch64-instruction
  * input: x0=dividend, x1=divisor
  * udiv|sdiv x2, x0, x1
  * msub x3, x2, x1, x0  -- multply-sub : x3 <- x0 - x2*x1
  * result: x2=quotient, x3=remainder
  *
  * allocate temporary register
  */
  RegOperand &temp = CreateRegisterOperandOfType(primType);
  Insn *movImmInsn = GetCurBB()->GetLastInsn();
  /*
   * mov     w1, #2
   * sdiv    wTemp, w0, w1
   * msub    wRespond, wTemp, w1, w0
   * ========>
   * asr     wTemp, w0, #31
   * lsr     wTemp, wTemp, #31  (#30 for 4, #29 for 8, ...)
   * add     wRespond, w0, wTemp
   * and     wRespond, wRespond, #1   (#3 for 4, #7 for 8, ...)
   * sub     wRespond, wRespond, w2
   *
   * if divde by 2
   * ========>
   * lsr     wTemp, w0, #31
   * add     wRespond, w0, wTemp
   * and     wRespond, wRespond, #1
   * sub     wRespond, wRespond, w2
   */
  if ((Globals::GetInstance()->GetOptimLevel() >= CGOptions::kLevel2) && movImmInsn &&
      ((movImmInsn->GetMachineOpcode() == MOP_xmovri32) || (movImmInsn->GetMachineOpcode() == MOP_xmovri64)) &&
       movImmInsn->GetOperand(0).Equals(opnd1)) {
    auto &imm = static_cast<AArch64ImmOperand&>(movImmInsn->GetOperand(kInsnSecondOpnd));
    /* positive or negative do not have effect on the result */
    const int64 dividor = (imm.GetValue() >= 0) ? imm.GetValue() : ((-1) * imm.GetValue());
    const int64 Log2OfDividor = IsPowerOf2(dividor);
    if ((dividor != 0) && (Log2OfDividor > 0)) {
      GetCurBB()->RemoveInsn(*GetCurBB()->GetLastInsn());
      if (is64Bits) {
        CHECK_FATAL(Log2OfDividor < k64BitSize, "imm out of bound");
        AArch64ImmOperand &rightShiftValue = CreateImmOperand(k64BitSize - Log2OfDividor, k64BitSize, isSigned);
        if (Log2OfDividor != 1) {
          /* 63->shift ALL , 32 ->32bit register */
          AArch64ImmOperand &rightShiftAll = CreateImmOperand(63, k64BitSize, isSigned);
          GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_xasrrri6, temp, opnd0, rightShiftAll));

          GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_xlsrrri6, temp, temp, rightShiftValue));
        } else {
          GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_xlsrrri6, temp, opnd0, rightShiftValue));
        }

        GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_xaddrrr, resOpnd, opnd0, temp));

        AArch64ImmOperand &remBits = CreateImmOperand(dividor - 1, k64BitSize, isSigned);
        GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_xandrri13, resOpnd, resOpnd, remBits));

        GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_xsubrrr, resOpnd, resOpnd, temp));
        return;
      } else {
        CHECK_FATAL(Log2OfDividor < k32BitSize, "imm out of bound");
        AArch64ImmOperand &rightShiftValue = CreateImmOperand(k32BitSize - Log2OfDividor, k32BitSize, isSigned);
        if (Log2OfDividor != 1) {
          /* 31->shift ALL , 32 ->32bit register */
          AArch64ImmOperand &rightShiftAll = CreateImmOperand(31, k32BitSize, isSigned);
          GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_wasrrri5, temp, opnd0, rightShiftAll));

          GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_wlsrrri5, temp, temp, rightShiftValue));
        } else {
          GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_wlsrrri5, temp, opnd0, rightShiftValue));
        }

        GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_waddrrr, resOpnd, opnd0, temp));

        AArch64ImmOperand &remBits = CreateImmOperand(dividor - 1, k32BitSize, isSigned);
        GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_wandrri12, resOpnd, resOpnd, remBits));

        GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_wsubrrr, resOpnd, resOpnd, temp));
        return;
      }
    }
  }
  uint32 mopDiv = is64Bits ? (isSigned ? MOP_xsdivrrr : MOP_xudivrrr) : (isSigned ? MOP_wsdivrrr : MOP_wudivrrr);
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mopDiv, temp, opnd0, opnd1));

  uint32 mopSub = is64Bits ? MOP_xmsubrrrr : MOP_wmsubrrrr;
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mopSub, resOpnd, temp, opnd1, opnd0));
}

Operand *AArch64CGFunc::SelectRem(BinaryNode &node, Operand &opnd0, Operand &opnd1) {
  PrimType dtype = node.GetPrimType();
  ASSERT(IsPrimitiveInteger(dtype), "wrong type for rem");
  bool isSigned = IsSignedInteger(dtype);
  uint32 dsize = GetPrimTypeBitSize(dtype);
  bool is64Bits = (dsize == k64BitSize);

  /* promoted type */
  PrimType primType = ((is64Bits ? (isSigned ? PTY_i64 : PTY_u64) : (isSigned ? PTY_i32 : PTY_u32)));
  RegOperand &resOpnd = CreateRegisterOperandOfType(primType);
  SelectRem(resOpnd, opnd0, opnd1, primType, isSigned, is64Bits);
  return &resOpnd;
}

Operand *AArch64CGFunc::SelectLand(BinaryNode &node, Operand &lhsOpnd, Operand &rhsOpnd) {
  PrimType primType = node.GetPrimType();
  ASSERT(IsPrimitiveInteger(primType), "Land should be integer type");
  bool is64Bits = (GetPrimTypeBitSize(primType) == k64BitSize);
  RegOperand &resOpnd = CreateRegisterOperandOfType(is64Bits ? PTY_u64 : PTY_u32);
  /*
   * OP0 band Op1
   * cmp  OP0, 0     # compare X0 with 0, sets Z bit
   * ccmp OP1, 0, 4 //==0100b, ne     # if(OP0!=0) cmp Op1 and 0, else NZCV <- 0100 makes OP0==0
   * cset RES, ne     # if Z==1(i.e., OP0==0||OP1==0) RES<-0, RES<-1
   */
  Operand &opnd0 = LoadIntoRegister(lhsOpnd, primType);
  SelectAArch64Cmp(opnd0, CreateImmOperand(0, primType, false), true, GetPrimTypeBitSize(primType));
  Operand &opnd1 = LoadIntoRegister(rhsOpnd, primType);
  SelectAArch64CCmp(opnd1, CreateImmOperand(0, primType, false), CreateImmOperand(4, PTY_u8, false),
                    GetCondOperand(CC_NE), is64Bits);
  SelectAArch64CSet(resOpnd, GetCondOperand(CC_NE), is64Bits);
  return &resOpnd;
}

Operand *AArch64CGFunc::SelectLor(BinaryNode &node, Operand &opnd0, Operand &opnd1, bool parentIsBr) {
  PrimType primType = node.GetPrimType();
  ASSERT(IsPrimitiveInteger(primType), "Lior should be integer type");
  bool is64Bits = (GetPrimTypeBitSize(primType) == k64BitSize);
  RegOperand &resOpnd = CreateRegisterOperandOfType(is64Bits ? PTY_u64 : PTY_u32);
  /*
   * OP0 band Op1
   * cmp  OP0, 0     # compare X0 with 0, sets Z bit
   * ccmp OP1, 0, 0 //==0100b, eq     # if(OP0==0,eq) cmp Op1 and 0, else NZCV <- 0000 makes OP0!=0
   * cset RES, ne     # if Z==1(i.e., OP0==0&&OP1==0) RES<-0, RES<-1
   */
  if (parentIsBr && !is64Bits && opnd0.IsRegister() && (static_cast<RegOperand*>(&opnd0)->GetValidBitsNum() == 1) &&
      opnd1.IsRegister() && (static_cast<RegOperand*>(&opnd1)->GetValidBitsNum() == 1)) {
    uint32 mOp = MOP_wiorrrr;
    static_cast<RegOperand&>(resOpnd).SetValidBitsNum(1);
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, resOpnd, opnd0, opnd1));
  } else {
    SelectBior(resOpnd, opnd0, opnd1, primType);
    SelectAArch64Cmp(resOpnd, CreateImmOperand(0, primType, false), true, GetPrimTypeBitSize(primType));
    SelectAArch64CSet(resOpnd, GetCondOperand(CC_NE), is64Bits);
  }
  return &resOpnd;
}

void AArch64CGFunc::SelectCmpOp(Operand &resOpnd, Operand &lhsOpnd, Operand &rhsOpnd,
                                Opcode opcode, PrimType primType) {
  uint32 dsize = resOpnd.GetSize();
  bool isFloat = IsPrimitiveFloat(primType);
  Operand &opnd0 = LoadIntoRegister(lhsOpnd, primType);

  /*
   * most of FP constants are passed as AArch64MemOperand
   * except 0.0 which is passed as kOpdFPZeroImmediate
   */
  Operand::OperandType opnd1Type = rhsOpnd.GetKind();
  Operand *opnd1 = &rhsOpnd;
  if ((opnd1Type != Operand::kOpdImmediate) && (opnd1Type != Operand::kOpdFPZeroImmediate) &&
      (opnd1Type != Operand::kOpdOffset)) {
    opnd1 = &LoadIntoRegister(rhsOpnd, primType);
  }

  bool unsignedIntegerComparison = !isFloat && !IsSignedInteger(primType);
  /*
   * OP_cmp, OP_cmpl, OP_cmpg
   * <cmp> OP0, OP1  ; fcmp for OP_cmpl/OP_cmpg, cmp/fcmpe for OP_cmp
   * CSINV RES, WZR, WZR, GE
   * CSINC RES, RES, WZR, LE
   * if OP_cmpl, CSINV RES, RES, WZR, VC (no overflow)
   * if OP_cmpg, CSINC RES, RES, WZR, VC (no overflow)
   */
  AArch64RegOperand &xzr = AArch64RegOperand::GetZeroRegister(dsize);
  if ((opcode == OP_cmpl) || (opcode == OP_cmpg)) {
    ASSERT(isFloat, "incorrect operand types");
    SelectTargetFPCmpQuiet(opnd0, *opnd1, GetPrimTypeBitSize(primType));
    SelectAArch64CSINV(resOpnd, xzr, xzr, GetCondOperand(CC_GE), (dsize == k64BitSize));
    SelectAArch64CSINC(resOpnd, resOpnd, xzr, GetCondOperand(CC_LE), (dsize == k64BitSize));
    if (opcode == OP_cmpl) {
      SelectAArch64CSINV(resOpnd, resOpnd, xzr, GetCondOperand(CC_VC), (dsize == k64BitSize));
    } else {
      SelectAArch64CSINC(resOpnd, resOpnd, xzr, GetCondOperand(CC_VC), (dsize == k64BitSize));
    }
    return;
  }

  if (opcode == OP_cmp) {
    SelectAArch64Cmp(opnd0, *opnd1, !isFloat, GetPrimTypeBitSize(primType));
    if (unsignedIntegerComparison) {
      SelectAArch64CSINV(resOpnd, xzr, xzr, GetCondOperand(CC_HS), (dsize == k64BitSize));
      SelectAArch64CSINC(resOpnd, resOpnd, xzr, GetCondOperand(CC_LS), (dsize == k64BitSize));
    } else {
      SelectAArch64CSINV(resOpnd, xzr, xzr, GetCondOperand(CC_GE), (dsize == k64BitSize));
      SelectAArch64CSINC(resOpnd, resOpnd, xzr, GetCondOperand(CC_LE), (dsize == k64BitSize));
    }
    return;
  }

  static_cast<RegOperand*>(&resOpnd)->SetValidBitsNum(1);
  if ((opcode == OP_lt) && opnd0.IsRegister() && opnd1->IsImmediate() &&
      (static_cast<ImmOperand*>(opnd1)->GetValue() == 0)) {
    bool is64Bits = (opnd0.GetSize() == k64BitSize);
    if (!unsignedIntegerComparison) {
      int32 bitLen = is64Bits ? kBitLenOfShift64Bits : kBitLenOfShift32Bits;
      ImmOperand &shiftNum = CreateImmOperand(is64Bits ? kHighestBitOf64Bits : kHighestBitOf32Bits, bitLen, false);
      MOperator mOpCode = is64Bits ? MOP_xlsrrri6 : MOP_wlsrrri5;
      GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOpCode, resOpnd, opnd0, shiftNum));
      return;
    }
    ImmOperand &constNum = CreateImmOperand(0, is64Bits ? k64BitSize : k32BitSize, false);
    GetCurBB()->AppendInsn(
        GetCG()->BuildInstruction<AArch64Insn>(is64Bits ? MOP_xmovri64 : MOP_xmovri32, resOpnd, constNum));
    return;
  }
  SelectAArch64Cmp(opnd0, *opnd1, !isFloat, GetPrimTypeBitSize(primType));

  AArch64CC_t cc = CC_EQ;
  switch (opcode) {
    case OP_eq:
      cc = CC_EQ;
      break;
    case OP_ne:
      cc = CC_NE;
      break;
    case OP_le:
      cc = unsignedIntegerComparison ? CC_LS : CC_LE;
      break;
    case OP_ge:
      cc = unsignedIntegerComparison ? CC_HS : CC_GE;
      break;
    case OP_gt:
      cc = unsignedIntegerComparison ? CC_HI : CC_GT;
      break;
    case OP_lt:
      cc = unsignedIntegerComparison ? CC_LO : CC_LT;
      break;
    default:
      CHECK_FATAL(false, "illegal logical operator");
  }
  SelectAArch64CSet(resOpnd, GetCondOperand(cc), (dsize == k64BitSize));
}

Operand *AArch64CGFunc::SelectCmpOp(CompareNode &node, Operand &opnd0, Operand &opnd1) {
  RegOperand &resOpnd = CreateRegisterOperandOfType(node.GetPrimType());
  SelectCmpOp(resOpnd, opnd0, opnd1, node.GetOpCode(), node.GetOpndType());
  return &resOpnd;
}

void AArch64CGFunc::SelectTargetFPCmpQuiet(Operand &o0, Operand &o1, uint32 dsize) {
  MOperator mOpCode = 0;
  if (o1.GetKind() == Operand::kOpdFPZeroImmediate) {
    mOpCode = (dsize == k64BitSize) ? MOP_dcmpqri : (dsize == k32BitSize) ? MOP_scmpqri : MOP_hcmpqri;
  } else if (o1.GetKind() == Operand::kOpdRegister) {
    mOpCode = (dsize == k64BitSize) ? MOP_dcmpqrr : (dsize == k32BitSize) ? MOP_scmpqrr : MOP_hcmpqrr;
  } else {
    CHECK_FATAL(false, "unsupported operand type");
  }
  Operand &rflag = GetOrCreateRflag();
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOpCode, rflag, o0, o1));
}

void AArch64CGFunc::SelectAArch64Cmp(Operand &o0, Operand &o1, bool isIntType, uint32 dsize) {
  MOperator mOpCode = 0;
  Operand *newO1 = &o1;
  if (isIntType) {
    if ((o1.GetKind() == Operand::kOpdImmediate) || (o1.GetKind() == Operand::kOpdOffset)) {
      ImmOperand *immOpnd = static_cast<ImmOperand*>(&o1);
      /*
       * imm : 0 ~ 4095, shift: none, LSL #0, or LSL #12
       * aarch64 assembly takes up to 24-bits, if the lower 12 bits is all 0
       */
      if (immOpnd->IsInBitSize(12) || immOpnd->IsInBitSize(12, 12)) {
        mOpCode = (dsize == k64BitSize) ? MOP_xcmpri : MOP_wcmpri;
      } else {
        /* load into register */
        PrimType ptype = (dsize == k64BitSize) ? PTY_i64 : PTY_i32;
        newO1 = &SelectCopy(o1, ptype, ptype);
        mOpCode = (dsize == k64BitSize) ? MOP_xcmprr : MOP_wcmprr;
      }
    } else if (o1.GetKind() == Operand::kOpdRegister) {
      mOpCode = (dsize == k64BitSize) ? MOP_xcmprr : MOP_wcmprr;
    } else {
      CHECK_FATAL(false, "unsupported operand type");
    }
  } else { /* float */
    if (o1.GetKind() == Operand::kOpdFPZeroImmediate) {
      mOpCode = (dsize == k64BitSize) ? MOP_dcmperi : ((dsize == k32BitSize) ? MOP_scmperi : MOP_hcmperi);
    } else if (o1.GetKind() == Operand::kOpdRegister) {
      mOpCode = (dsize == k64BitSize) ? MOP_dcmperr : ((dsize == k32BitSize) ? MOP_scmperr : MOP_hcmperr);
    } else {
      CHECK_FATAL(false, "unsupported operand type");
    }
  }
  ASSERT(mOpCode != 0, "mOpCode undefined");
  Operand &rflag = GetOrCreateRflag();
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOpCode, rflag, o0, *newO1));
}

void AArch64CGFunc::SelectAArch64CCmp(Operand &o, Operand &i, Operand &nzcv, CondOperand &cond, bool is64Bits) {
  uint32 mOpCode = is64Bits ? MOP_xccmpriic : MOP_wccmpriic;
  Operand &rflag = GetOrCreateRflag();
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOpCode, rflag, o, i, nzcv, cond));
}

void AArch64CGFunc::SelectAArch64CSet(Operand &r, CondOperand &cond, bool is64Bits) {
  MOperator mOpCode = is64Bits ? MOP_xcsetrc : MOP_wcsetrc;
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOpCode, r, cond));
}

void AArch64CGFunc::SelectAArch64CSINV(Operand &res, Operand &o0, Operand &o1, CondOperand &cond, bool is64Bits) {
  MOperator mOpCode = is64Bits ? MOP_xcsinvrrrc : MOP_wcsinvrrrc;
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOpCode, res, o0, o1, cond));
}

void AArch64CGFunc::SelectAArch64CSINC(Operand &res, Operand &o0, Operand &o1, CondOperand &cond, bool is64Bits) {
  MOperator mOpCode = is64Bits ? MOP_xcsincrrrc : MOP_wcsincrrrc;
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOpCode, res, o0, o1, cond));
}

Operand *AArch64CGFunc::SelectBand(BinaryNode &node, Operand &opnd0, Operand &opnd1) {
  return SelectRelationOperator(kAND, node, opnd0, opnd1);
}

void AArch64CGFunc::SelectBand(Operand &resOpnd, Operand &opnd0, Operand &opnd1, PrimType primType) {
  SelectRelationOperator(kAND, resOpnd, opnd0, opnd1, primType);
}

Operand *AArch64CGFunc::SelectRelationOperator(RelationOperator operatorCode, const BinaryNode &node, Operand &opnd0,
                                               Operand &opnd1) {
  PrimType dtype = node.GetPrimType();
  bool isSigned = IsSignedInteger(dtype);
  uint32 dsize = GetPrimTypeBitSize(dtype);
  bool is64Bits = (dsize == k64BitSize);
  PrimType primType = is64Bits ? (isSigned ? PTY_i64 : PTY_u64) : (isSigned ? PTY_i32 : PTY_u32);  /* promoted type */
  RegOperand &resOpnd = CreateRegisterOperandOfType(primType);
  SelectRelationOperator(operatorCode, resOpnd, opnd0, opnd1, primType);
  return &resOpnd;
}

MOperator AArch64CGFunc::SelectRelationMop(RelationOperator operatorCode,
                                           RelationOperatorOpndPattern opndPattern, bool is64Bits,
                                           bool isBitmaskImmediate, bool isBitNumLessThan16) const {
  MOperator mOp = MOP_undef;
  if (opndPattern == kRegReg) {
    switch (operatorCode) {
      case kAND:
        mOp = is64Bits ? MOP_xandrrr : MOP_wandrrr;
        break;
      case kIOR:
        mOp = is64Bits ? MOP_xiorrrr : MOP_wiorrrr;
        break;
      case kEOR:
        mOp = is64Bits ? MOP_xeorrrr : MOP_weorrrr;
        break;
      default:
        break;
    }
    return mOp;
  }
  /* opndPattern == KRegImm */
  if (isBitmaskImmediate) {
    switch (operatorCode) {
      case kAND:
        mOp = is64Bits ? MOP_xandrri13 : MOP_wandrri12;
        break;
      case kIOR:
        mOp = is64Bits ? MOP_xiorrri13 : MOP_wiorrri12;
        break;
      case kEOR:
        mOp = is64Bits ? MOP_xeorrri13 : MOP_weorrri12;
        break;
      default:
        break;
    }
    return mOp;
  }
  /* normal imm value */
  if (isBitNumLessThan16) {
    switch (operatorCode) {
      case kAND:
        mOp = is64Bits ? MOP_xandrrrs : MOP_wandrrrs;
        break;
      case kIOR:
        mOp = is64Bits ? MOP_xiorrrrs : MOP_wiorrrrs;
        break;
      case kEOR:
        mOp = is64Bits ? MOP_xeorrrrs : MOP_weorrrrs;
        break;
      default:
        break;
    }
    return mOp;
  }
  return mOp;
}

void AArch64CGFunc::SelectRelationOperator(RelationOperator operatorCode, Operand &resOpnd, Operand &opnd0,
                                           Operand &opnd1, PrimType primType) {
  Operand::OperandType opnd0Type = opnd0.GetKind();
  Operand::OperandType opnd1Type = opnd1.GetKind();
  uint32 dsize = GetPrimTypeBitSize(primType);
  bool is64Bits = (dsize == k64BitSize);
  /* op #imm. #imm */
  if ((opnd0Type != Operand::kOpdRegister) && (opnd1Type != Operand::kOpdRegister)) {
    SelectRelationOperator(operatorCode, resOpnd, SelectCopy(opnd0, primType, primType), opnd1, primType);
    return;
  }
  /* op #imm, reg -> op reg, #imm */
  if ((opnd0Type != Operand::kOpdRegister) && (opnd1Type == Operand::kOpdRegister)) {
    SelectRelationOperator(operatorCode, resOpnd, opnd1, opnd0, primType);
    return;
  }
  /* op reg, reg */
  if ((opnd0Type == Operand::kOpdRegister) && (opnd1Type == Operand::kOpdRegister)) {
    ASSERT(IsPrimitiveInteger(primType), "NYI band");
    MOperator mOp = SelectRelationMop(operatorCode, kRegReg, is64Bits, false, false);
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, resOpnd, opnd0, opnd1));
    return;
  }
  /* op reg, #imm */
  if ((opnd0Type == Operand::kOpdRegister) && (opnd1Type != Operand::kOpdRegister)) {
    if (!((opnd1Type == Operand::kOpdImmediate) || (opnd1Type == Operand::kOpdOffset))) {
      SelectRelationOperator(operatorCode, resOpnd, opnd0, SelectCopy(opnd1, primType, primType), primType);
      return;
    }

    AArch64ImmOperand *immOpnd = static_cast<AArch64ImmOperand*>(&opnd1);
    if (immOpnd->IsZero()) {
      if (operatorCode == kAND) {
        uint32 mopMv = is64Bits ? MOP_xmovrr : MOP_wmovrr;
        GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mopMv, resOpnd,
                                                                      AArch64RegOperand::GetZeroRegister(dsize)));
      } else if ((operatorCode == kIOR) || (operatorCode == kEOR)) {
        SelectCopy(resOpnd, primType, opnd0, primType);
      }
    } else if (immOpnd->IsAllOnes()) {
      if (operatorCode == kAND) {
        SelectCopy(resOpnd, primType, opnd0, primType);
      } else if (operatorCode == kIOR) {
        uint32 mopMovn = is64Bits ? MOP_xmovnri16 : MOP_wmovnri16;
        ImmOperand &src16 = CreateImmOperand(0, k16BitSize, false);
        LogicalShiftLeftOperand *lslOpnd = GetLogicalShiftLeftOperand(0, is64Bits);
        GetCurBB()->AppendInsn(
            GetCG()->BuildInstruction<AArch64Insn>(mopMovn, resOpnd, src16, *lslOpnd));
      } else if (operatorCode == kEOR) {
        SelectMvn(resOpnd, opnd0, primType);
      }
    } else if (immOpnd->IsBitmaskImmediate()) {
      MOperator mOp = SelectRelationMop(operatorCode, kRegImm, is64Bits, true, false);
      GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, resOpnd, opnd0, opnd1));
    } else {
      int64 immVal = immOpnd->GetValue();
      int32 tail0BitNum = GetTail0BitNum(immVal);
      int32 head0BitNum = GetHead0BitNum(immVal);
      const int32 bitNum = k64BitSize - head0BitNum - tail0BitNum;
      RegOperand &regOpnd = CreateRegisterOperandOfType(primType);

      if (bitNum <= k16ValidBit) {
        int64 newImm = (static_cast<uint64>(immVal) >> static_cast<uint32>(tail0BitNum)) & 0xFFFF;
        AArch64ImmOperand &immOpnd1 = CreateImmOperand(newImm, k16BitSize, false);
        SelectCopyImm(regOpnd, immOpnd1, primType);
        MOperator mOp = SelectRelationMop(operatorCode, kRegImm, is64Bits, false, true);
        int32 bitLen = is64Bits ? kBitLenOfShift64Bits : kBitLenOfShift32Bits;
        BitShiftOperand &shiftOpnd = CreateBitShiftOperand(BitShiftOperand::kLSL, tail0BitNum, bitLen);
        GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, resOpnd, opnd0, regOpnd, shiftOpnd));
      } else {
        SelectCopyImm(regOpnd, *immOpnd, primType);
        MOperator mOp = SelectRelationMop(operatorCode, kRegReg, is64Bits, false, false);
        GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, resOpnd, opnd0, regOpnd));
      }
    }
  }
}

Operand *AArch64CGFunc::SelectBior(BinaryNode &node, Operand &opnd0, Operand &opnd1) {
  return SelectRelationOperator(kIOR, node, opnd0, opnd1);
}

void AArch64CGFunc::SelectBior(Operand &resOpnd, Operand &opnd0, Operand &opnd1, PrimType primType) {
  SelectRelationOperator(kIOR, resOpnd, opnd0, opnd1, primType);
}

Operand *AArch64CGFunc::SelectMinOrMax(bool isMin, const BinaryNode &node, Operand &opnd0, Operand &opnd1) {
  PrimType dtype = node.GetPrimType();
  bool isSigned = IsSignedInteger(dtype);
  uint32 dsize = GetPrimTypeBitSize(dtype);
  bool is64Bits = (dsize == k64BitSize);
  bool isFloat = IsPrimitiveFloat(dtype);
  /* promoted type */
  PrimType primType = isFloat ? dtype : (is64Bits ? (isSigned ? PTY_i64 : PTY_u64) : (isSigned ? PTY_i32 : PTY_u32));
  RegOperand &resOpnd = CreateRegisterOperandOfType(primType);
  SelectMinOrMax(isMin, resOpnd, opnd0, opnd1, primType);
  return &resOpnd;
}

void AArch64CGFunc::SelectMinOrMax(bool isMin, Operand &resOpnd, Operand &opnd0, Operand &opnd1, PrimType primType) {
  uint32 dsize = GetPrimTypeBitSize(primType);
  bool is64Bits = (dsize == k64BitSize);
  if (IsPrimitiveInteger(primType)) {
    RegOperand &regOpnd0 = LoadIntoRegister(opnd0, primType);
    Operand *newOpnd1 = &opnd1;
    if ((opnd1.GetKind() != Operand::kOpdImmediate) && (opnd1.GetKind() != Operand::kOpdOffset)) {
      newOpnd1 = &LoadIntoRegister(opnd1, primType);
    }
    SelectAArch64Cmp(regOpnd0, *newOpnd1, true, dsize);
    Operand &newResOpnd = LoadIntoRegister(resOpnd, primType);
    if (isMin) {
      CondOperand &cc = IsSignedInteger(primType) ? GetCondOperand(CC_LT) : GetCondOperand(CC_LO);
      SelectAArch64Select(newResOpnd, regOpnd0, *newOpnd1, cc, true, dsize);
    } else {
      CondOperand &cc = IsSignedInteger(primType) ? GetCondOperand(CC_GT) : GetCondOperand(CC_HI);
      SelectAArch64Select(newResOpnd, regOpnd0, *newOpnd1, cc, true, dsize);
    }
  } else if (IsPrimitiveFloat(primType)) {
    RegOperand &regOpnd0 = LoadIntoRegister(opnd0, primType);
    RegOperand &regOpnd1 = LoadIntoRegister(opnd1, primType);
    SelectFMinFMax(resOpnd, regOpnd0, regOpnd1, is64Bits, isMin);
  } else {
    CHECK_FATAL(false, "NIY type max or min");
  }
}

Operand *AArch64CGFunc::SelectMin(BinaryNode &node, Operand &opnd0, Operand &opnd1) {
  return SelectMinOrMax(true, node, opnd0, opnd1);
}

void AArch64CGFunc::SelectMin(Operand &resOpnd, Operand &opnd0, Operand &opnd1, PrimType primType) {
  SelectMinOrMax(true, resOpnd, opnd0, opnd1, primType);
}

Operand *AArch64CGFunc::SelectMax(BinaryNode &node, Operand &opnd0, Operand &opnd1) {
  return SelectMinOrMax(false, node, opnd0, opnd1);
}

void AArch64CGFunc::SelectMax(Operand &resOpnd, Operand &opnd0, Operand &opnd1, PrimType primType) {
  SelectMinOrMax(false, resOpnd, opnd0, opnd1, primType);
}

void AArch64CGFunc::SelectFMinFMax(Operand &resOpnd, Operand &opnd0, Operand &opnd1, bool is64Bits, bool isMin) {
  uint32 mOpCode = isMin ? (is64Bits ? MOP_xfminrrr : MOP_wfminrrr) : (is64Bits ? MOP_xfmaxrrr : MOP_wfmaxrrr);
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOpCode, resOpnd, opnd0, opnd1));
}

Operand *AArch64CGFunc::SelectBxor(BinaryNode &node, Operand &opnd0, Operand &opnd1) {
  return SelectRelationOperator(kEOR, node, opnd0, opnd1);
}

void AArch64CGFunc::SelectBxor(Operand &resOpnd, Operand &opnd0, Operand &opnd1, PrimType primType) {
  SelectRelationOperator(kEOR, resOpnd, opnd0, opnd1, primType);
}

Operand *AArch64CGFunc::SelectShift(BinaryNode &node, Operand &opnd0, Operand &opnd1) {
  PrimType dtype = node.GetPrimType();
  bool isSigned = IsSignedInteger(dtype);
  uint32 dsize = GetPrimTypeBitSize(dtype);
  bool is64Bits = (dsize == k64BitSize);
  bool isFloat = IsPrimitiveFloat(dtype);
  /* promoted type */
  PrimType primType = isFloat ? dtype : (is64Bits ? (isSigned ? PTY_i64 : PTY_u64) : (isSigned ? PTY_i32 : PTY_u32));
  RegOperand &resOpnd = CreateRegisterOperandOfType(primType);
  Opcode opcode = node.GetOpCode();
  ShiftDirection direct = (opcode == OP_lshr) ? kShiftLright : ((opcode == OP_ashr) ? kShiftAright : kShiftLeft);
  SelectShift(resOpnd, opnd0, opnd1, direct, primType);
  return &resOpnd;
}

void AArch64CGFunc::SelectBxorShift(Operand &resOpnd, Operand *opnd0, Operand *opnd1, Operand &opnd2,
                                    PrimType primType) {
  opnd0 = &LoadIntoRegister(*opnd0, primType);
  opnd1 = &LoadIntoRegister(*opnd1, primType);
  uint32 dsize = GetPrimTypeBitSize(primType);
  bool is64Bits = (dsize == k64BitSize);
  MOperator mopBxor = is64Bits ? MOP_xeorrrrs : MOP_weorrrrs;
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mopBxor, resOpnd, *opnd0, *opnd1, opnd2));
}

void AArch64CGFunc::SelectShift(Operand &resOpnd, Operand &opnd0, Operand &opnd1, ShiftDirection direct,
                                PrimType primType) {
  Operand::OperandType opnd1Type = opnd1.GetKind();
  uint32 dsize = GetPrimTypeBitSize(primType);
  bool is64Bits = (dsize == k64BitSize);
  Operand *firstOpnd = &LoadIntoRegister(opnd0, primType);

  MOperator mopShift;
  if ((opnd1Type == Operand::kOpdImmediate) || (opnd1Type == Operand::kOpdOffset)) {
    AArch64ImmOperand *immOpnd1 = static_cast<AArch64ImmOperand*>(&opnd1);
    const int64 kVal = immOpnd1->GetValue();
    const uint32 kShiftamt = is64Bits ? kHighestBitOf64Bits : kHighestBitOf32Bits;
    if (kVal == 0) {
      SelectCopy(resOpnd, primType, *firstOpnd, primType);
      return;
    }
    /* e.g. a >> -1 */
    if ((kVal < 0) || (kVal > kShiftamt)) {
      SelectShift(resOpnd, *firstOpnd, SelectCopy(opnd1, primType, primType), direct, primType);
      return;
    }
    switch (direct) {
      case kShiftLeft:
        if (kVal == 1) {
          SelectAdd(resOpnd, *firstOpnd, *firstOpnd, primType);
          return;
        }
        mopShift = is64Bits ? MOP_xlslrri6 : MOP_wlslrri5;
        break;
      case kShiftAright:
        mopShift = is64Bits ? MOP_xasrrri6 : MOP_wasrrri5;
        break;
      case kShiftLright:
        mopShift = is64Bits ? MOP_xlsrrri6 : MOP_wlsrrri5;
        break;
    }
  } else if (opnd1Type != Operand::kOpdRegister) {
    SelectShift(resOpnd, *firstOpnd, SelectCopy(opnd1, primType, primType), direct, primType);
    return;
  } else {
    switch (direct) {
      case kShiftLeft:
        mopShift = is64Bits ? MOP_xlslrrr : MOP_wlslrrr;
        break;
      case kShiftAright:
        mopShift = is64Bits ? MOP_xasrrrr : MOP_wasrrrr;
        break;
      case kShiftLright:
        mopShift = is64Bits ? MOP_xlsrrrr : MOP_wlsrrrr;
        break;
    }
  }

  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mopShift, resOpnd, *firstOpnd, opnd1));
}

Operand *AArch64CGFunc::SelectAbs(UnaryNode &node, Operand &opnd0) {
  PrimType dtyp = node.GetPrimType();
  if (IsPrimitiveFloat(dtyp)) {
    CHECK_FATAL(GetPrimTypeBitSize(dtyp) >= k32BitSize, "We don't support hanf-word FP operands yet");
    bool is64Bits = (GetPrimTypeBitSize(dtyp) == k64BitSize);
    Operand &newOpnd0 = LoadIntoRegister(opnd0, dtyp);
    RegOperand &resOpnd = CreateRegisterOperandOfType(dtyp);
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(is64Bits ? MOP_dabsrr : MOP_sabsrr,
                                                                  resOpnd, newOpnd0));
    return &resOpnd;
  } else {
    bool is64Bits = (GetPrimTypeBitSize(dtyp) == k64BitSize);
    /* promoted type */
    PrimType primType = is64Bits ? (PTY_i64) : (PTY_i32);
    RegOperand &resOpnd = CreateRegisterOperandOfType(primType);
    Operand &newOpnd0 = LoadIntoRegister(opnd0, primType);
    SelectAArch64Cmp(newOpnd0, CreateImmOperand(0, is64Bits ? PTY_u64 : PTY_u32, false),
                     true, GetPrimTypeBitSize(dtyp));
    uint32 mopCsneg = is64Bits ? MOP_xcsnegrrrc : MOP_wcsnegrrrc;
    /* ABS requires the operand be interpreted as a signed integer */
    CondOperand &condOpnd = GetCondOperand(CC_GE);
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mopCsneg, resOpnd, newOpnd0, newOpnd0, condOpnd));
    return &resOpnd;
  }
}

Operand *AArch64CGFunc::SelectBnot(UnaryNode &node, Operand &opnd0) {
  PrimType dtype = node.GetPrimType();
  ASSERT(IsPrimitiveInteger(dtype), "bnot expect integer or NYI");
  bool is64Bits = (GetPrimTypeBitSize(dtype) == k64BitSize);
  bool isSigned = IsSignedInteger(dtype);
  /* promoted type */
  PrimType primType = is64Bits ? (isSigned ? PTY_i64 : PTY_u64) : (isSigned ? PTY_i32 : PTY_u32);
  RegOperand &resOpnd = CreateRegisterOperandOfType(primType);

  Operand &newOpnd0 = LoadIntoRegister(opnd0, primType);

  uint32 mopBnot = is64Bits ? MOP_xnotrr : MOP_wnotrr;
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mopBnot, resOpnd, newOpnd0));

  return &resOpnd;
}

Operand *AArch64CGFunc::SelectExtractbits(ExtractbitsNode &node, Operand &srcOpnd) {
  PrimType dtype = node.GetPrimType();
  RegOperand &resOpnd = CreateRegisterOperandOfType(dtype);
  bool isSigned = IsSignedInteger(dtype);
  uint8 bitOffset = node.GetBitsOffset();
  uint8 bitSize = node.GetBitsSize();
  bool is64Bits = (GetPrimTypeBitSize(dtype) == k64BitSize);
  uint32 immWidth = is64Bits ? kMaxAarch64ImmVal13Bits : kMaxAarch64ImmVal12Bits;
  Operand &opnd0 = LoadIntoRegister(srcOpnd, dtype);
  if ((bitOffset == 0) && !isSigned && (bitSize < immWidth)) {
    SelectBand(resOpnd, opnd0, CreateImmOperand((static_cast<uint64>(1) << bitSize) - 1, immWidth, false), dtype);
    return &resOpnd;
  }
  uint32 mopBfx =
      is64Bits ? (isSigned ? MOP_xsbfxrri6i6 : MOP_xubfxrri6i6) : (isSigned ? MOP_wsbfxrri5i5 : MOP_wubfxrri5i5);
  AArch64ImmOperand &immOpnd1 = CreateImmOperand(bitOffset, k8BitSize, false);
  AArch64ImmOperand &immOpnd2 = CreateImmOperand(bitSize, k8BitSize, false);
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mopBfx, resOpnd, opnd0, immOpnd1, immOpnd2));
  return &resOpnd;
}

/*
 *  operand fits in MOVK if
 *     is64Bits && boffst == 0, 16, 32, 48 && bSize == 16, so boffset / 16 == 0, 1, 2, 3; (boffset / 16 ) & (~3) == 0
 *  or is32Bits && boffset == 0, 16 && bSize == 16, so boffset / 16 == 0, 1; (boffset / 16) & (~1) == 0
 */
inline bool IsMoveWideKeepable(uint32 bitOffset, uint32 bitSize, bool is64Bits) {
  ASSERT(is64Bits || (bitOffset < k32BitSize), "");
  return (bitSize == k16BitSize && ((bitOffset >> k16BitShift) & ~static_cast<uint32>(is64Bits ? 0x3 : 0x1)) == 0);
}

/* we use the fact that A ^ B ^ A == B, A ^ 0 = A */
void AArch64CGFunc::SelectDepositBits(Operand &resOpnd, Operand &opnd0, Operand &opnd1, uint32 bitOffset,
                                      uint32 bitSize, PrimType regType) {
  RegOperand &t1opnd = CreateRegisterOperandOfType(regType);
  bool is64Bits = GetPrimTypeBitSize(regType) == k64BitSize;
  /*
   * if operand 1 is immediate and fits in MOVK, use it
   * MOVK Wd, #imm{, LSL #shift} ; 32-bit general registers
   * MOVK Xd, #imm{, LSL #shift} ; 64-bit general registers
   */
  if (opnd1.IsIntImmediate() && IsMoveWideKeepable(bitOffset, bitSize, is64Bits)) {
    SelectCopy(resOpnd, regType, opnd0, regType);
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>((is64Bits ? MOP_xmovkri16 : MOP_wmovkri16),
                                                                  resOpnd, opnd1,
                                                                  *GetLogicalShiftLeftOperand(bitOffset, is64Bits)));
  } else {
    /*
     * Merge-form of Itanium deposit
     * 1. (opnd0>>bitsOffset) ^ opnd1
     */
    int32 bitLen = is64Bits ? kBitLenOfShift64Bits : kBitLenOfShift32Bits;
    Operand &shiftOpnd = CreateBitShiftOperand(BitShiftOperand::kLSR, bitOffset, bitLen);
    /* bit-shift the first operand to the right by offset and XOR with the second operand */
    SelectBxorShift(t1opnd, &opnd1, &opnd0, shiftOpnd, regType);
    /*
     * bit-shift the result to the left by offset, retain size bits from offset, clear the rest.
     * ubfiz t1opnd, bitsOffset, size
     */
    uint32 mopUbfiz = is64Bits ? MOP_xubfizrri6i6 : MOP_wubfizrri5i5;
    /* XOR the result with the first operand */
    AArch64ImmOperand &immOpnd1 = CreateImmOperand(bitOffset, k8BitSize, false);
    AArch64ImmOperand &immOpnd2 = CreateImmOperand(bitSize, k8BitSize, false);
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mopUbfiz, t1opnd, t1opnd, immOpnd1, immOpnd2));
    /* opnd0 ^ t1opnd */
    SelectBxor(resOpnd, opnd0, t1opnd, regType);
  }
}

Operand *AArch64CGFunc::SelectDepositBits(DepositbitsNode &node, Operand &opnd0, Operand &opnd1) {
  PrimType dtype = node.GetPrimType();
  RegOperand &resOpnd = CreateRegisterOperandOfType(dtype);
  SelectDepositBits(resOpnd, opnd0, opnd1, node.GetBitsOffset(), node.GetBitsSize(), dtype);
  return &resOpnd;
}

Operand *AArch64CGFunc::SelectLnot(UnaryNode &node, Operand &srcOpnd) {
  PrimType dtype = node.GetPrimType();
  RegOperand &resOpnd = CreateRegisterOperandOfType(dtype);
  bool is64Bits = (GetPrimTypeBitSize(dtype) == k64BitSize);
  Operand &opnd0 = LoadIntoRegister(srcOpnd, dtype);
  SelectAArch64Cmp(opnd0, CreateImmOperand(0, is64Bits ? PTY_u64 : PTY_u32, false), true, GetPrimTypeBitSize(dtype));
  SelectAArch64CSet(resOpnd, GetCondOperand(CC_EQ), is64Bits);
  return &resOpnd;
}

Operand *AArch64CGFunc::SelectNeg(UnaryNode &node, Operand &opnd0) {
  PrimType dtype = node.GetPrimType();
  bool is64Bits = (GetPrimTypeBitSize(dtype) == k64BitSize);
  PrimType primType;
  if (IsPrimitiveFloat(dtype)) {
    primType = dtype;
  } else {
    primType = is64Bits ? (PTY_i64) : (PTY_i32);  /* promoted type */
  }
  RegOperand &resOpnd = CreateRegisterOperandOfType(primType);
  SelectNeg(resOpnd, opnd0, primType);
  return &resOpnd;
}

void AArch64CGFunc::SelectNeg(Operand &dest, Operand &srcOpnd, PrimType primType) {
  Operand &opnd0 = LoadIntoRegister(srcOpnd, primType);
  bool is64Bits = (GetPrimTypeBitSize(primType) == k64BitSize);
  MOperator mOp;
  if (IsPrimitiveFloat(primType)) {
    mOp = is64Bits ? MOP_xfnegrr : MOP_wfnegrr;
  } else {
    mOp = is64Bits ? MOP_xinegrr : MOP_winegrr;
  }
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, dest, opnd0));
}

void AArch64CGFunc::SelectMvn(Operand &dest, Operand &src, PrimType primType) {
  Operand &opnd0 = LoadIntoRegister(src, primType);
  bool is64Bits = (GetPrimTypeBitSize(primType) == k64BitSize);
  MOperator mOp;
  ASSERT(!IsPrimitiveFloat(primType), "Instruction 'mvn' do not have float version.");
  mOp = is64Bits ? MOP_xnotrr : MOP_wnotrr;
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, dest, opnd0));
}

Operand *AArch64CGFunc::SelectRecip(UnaryNode &node, Operand &src) {
  /*
   * fconsts s15, #112
   * fdivs s0, s15, s0
   */
  PrimType dtype = node.GetPrimType();
  if (!IsPrimitiveFloat(dtype)) {
    ASSERT(false, "should be float type");
    return nullptr;
  }
  Operand &opnd0 = LoadIntoRegister(src, dtype);
  RegOperand &resOpnd = CreateRegisterOperandOfType(dtype);
  Operand *one = nullptr;
  if (GetPrimTypeBitSize(dtype) == k64BitSize) {
    MIRDoubleConst *c = memPool->New<MIRDoubleConst>(1.0, *GlobalTables::GetTypeTable().GetTypeTable().at(PTY_f64));
    one = SelectDoubleConst(*c);
  } else if (GetPrimTypeBitSize(dtype) == k32BitSize) {
    MIRFloatConst *c = memPool->New<MIRFloatConst>(1.0f, *GlobalTables::GetTypeTable().GetTypeTable().at(PTY_f32));
    one = SelectFloatConst(*c);
  } else {
    CHECK_FATAL(false, "we don't support half-precision fp operations yet");
  }
  SelectDiv(resOpnd, *one, opnd0, dtype);
  return &resOpnd;
}

Operand *AArch64CGFunc::SelectSqrt(UnaryNode &node, Operand &src) {
  /*
   * gcc generates code like below for better accurate
   * fsqrts  s15, s0
   * fcmps s15, s15
   * fmstat
   * beq .L4
   * push  {r3, lr}
   * bl  sqrtf
   * pop {r3, pc}
   * .L4:
   * fcpys s0, s15
   * bx  lr
   */
  PrimType dtype = node.GetPrimType();
  if (!IsPrimitiveFloat(dtype)) {
    ASSERT(false, "should be float type");
    return nullptr;
  }
  bool is64Bits = (GetPrimTypeBitSize(dtype) == k64BitSize);
  Operand &opnd0 = LoadIntoRegister(src, dtype);
  RegOperand &resOpnd = CreateRegisterOperandOfType(dtype);
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(is64Bits ? MOP_vsqrtd : MOP_vsqrts, resOpnd, opnd0));
  return &resOpnd;
}

void AArch64CGFunc::SelectCvtFloat2Int(Operand &resOpnd, Operand &srcOpnd, PrimType itype, PrimType ftype) {
  bool is64BitsFloat = (ftype == PTY_f64);
  MOperator mOp = 0;

  ASSERT(((ftype == PTY_f64) || (ftype == PTY_f32)), "wrong from type");
  Operand &opnd0 = LoadIntoRegister(srcOpnd, ftype);
  switch (itype) {
    case PTY_i32:
      mOp = !is64BitsFloat ? MOP_vcvtrf : MOP_vcvtrd;
      break;
    case PTY_u32:
      mOp = !is64BitsFloat ? MOP_vcvturf : MOP_vcvturd;
      break;
    case PTY_i64:
      mOp = !is64BitsFloat ? MOP_xvcvtrf : MOP_xvcvtrd;
      break;
    case PTY_u64:
      mOp = !is64BitsFloat ? MOP_xvcvturf : MOP_xvcvturd;
      break;
    default:
      CHECK_FATAL(false, "unexpected type");
  }
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, resOpnd, opnd0));
}

void AArch64CGFunc::SelectCvtInt2Float(Operand &resOpnd, Operand &origOpnd0, PrimType toType, PrimType fromType) {
  ASSERT((toType == PTY_f32) || (toType == PTY_f64), "unexpected type");
  bool is64BitsFloat = (toType == PTY_f64);
  MOperator mOp = 0;
  uint32 fsize = GetPrimTypeBitSize(fromType);

  PrimType itype = (GetPrimTypeBitSize(fromType) == k64BitSize) ? (IsSignedInteger(fromType) ? PTY_i64 : PTY_u64)
                                                                : (IsSignedInteger(fromType) ? PTY_i32 : PTY_u32);

  Operand *opnd0 = &LoadIntoRegister(origOpnd0, itype);

  /* need extension before cvt */
  ASSERT(opnd0->IsRegister(), "opnd should be a register operand");
  Operand *srcOpnd = opnd0;
  if (IsSignedInteger(fromType) && (fsize < k32BitSize)) {
    srcOpnd = &CreateRegisterOperandOfType(itype);
    mOp = (fsize == k8BitSize) ? MOP_xsxtb32 : MOP_xsxth32;
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, *srcOpnd, *opnd0));
  }

  switch (itype) {
    case PTY_i32:
      mOp = !is64BitsFloat ? MOP_vcvtfr : MOP_vcvtdr;
      break;
    case PTY_u32:
      mOp = !is64BitsFloat ? MOP_vcvtufr : MOP_vcvtudr;
      break;
    case PTY_i64:
      mOp = !is64BitsFloat ? MOP_xvcvtfr : MOP_xvcvtdr;
      break;
    case PTY_u64:
      mOp = !is64BitsFloat ? MOP_xvcvtufr : MOP_xvcvtudr;
      break;
    default:
      CHECK_FATAL(false, "unexpected type");
  }
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, resOpnd, *srcOpnd));
}

Operand *AArch64CGFunc::SelectRoundOperator(RoundType roundType, const TypeCvtNode &node, Operand &opnd0) {
  PrimType ftype = node.FromType();
  PrimType itype = node.GetPrimType();
  ASSERT(((ftype == PTY_f64) || (ftype == PTY_f32)), "wrong float type");
  bool is64Bits = (ftype == PTY_f64);
  RegOperand &resOpnd = CreateRegisterOperandOfType(itype);
  RegOperand &regOpnd0 = LoadIntoRegister(opnd0, ftype);
  MOperator mop = MOP_undef;
  if (roundType == kCeil) {
    mop = is64Bits ? MOP_xvcvtps : MOP_vcvtps;
  } else if (roundType == kFloor) {
    mop = is64Bits ? MOP_xvcvtms : MOP_vcvtms;
  } else {
    mop = is64Bits ? MOP_xvcvtas : MOP_vcvtas;
  }
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mop, resOpnd, regOpnd0));
  return &resOpnd;
}

Operand *AArch64CGFunc::SelectCeil(TypeCvtNode &node, Operand &opnd0) {
  return SelectRoundOperator(kCeil, node, opnd0);
}

/* float to int floor */
Operand *AArch64CGFunc::SelectFloor(TypeCvtNode &node, Operand &opnd0) {
  return SelectRoundOperator(kFloor, node, opnd0);
}

Operand *AArch64CGFunc::SelectRound(TypeCvtNode &node, Operand &opnd0) {
  return SelectRoundOperator(kRound, node, opnd0);
}

static bool LIsPrimitivePointer(PrimType ptype) {
  return ((PTY_ptr <= ptype) && (ptype <= PTY_a64));
}

Operand *AArch64CGFunc::SelectRetype(TypeCvtNode &node, Operand &opnd0) {
  PrimType fromType = node.FromType();
  PrimType toType = node.GetPrimType();
  ASSERT(GetPrimTypeSize(fromType) == GetPrimTypeSize(toType), "retype bit widith doesn' match");
  if (LIsPrimitivePointer(fromType) && LIsPrimitivePointer(toType)) {
    return &LoadIntoRegister(opnd0, toType);
  }
  Operand::OperandType opnd0Type = opnd0.GetKind();
  RegOperand *resOpnd = &CreateRegisterOperandOfType(toType);
  if (IsPrimitiveInteger(fromType) || IsPrimitiveFloat(fromType)) {
    bool isFromInt = IsPrimitiveInteger(fromType);
    bool is64Bits = GetPrimTypeBitSize(fromType) == k64BitSize;
    PrimType itype =
        isFromInt ? ((GetPrimTypeBitSize(fromType) == k64BitSize) ? (IsSignedInteger(fromType) ? PTY_i64 : PTY_u64)
                                                                  : (IsSignedInteger(fromType) ? PTY_i32 : PTY_u32))
                  : (is64Bits ? PTY_f64 : PTY_f32);

    /*
     * if source operand is in memory,
     * simply read it as a value of 'toType 'into the dest operand
     * and return
     */
    if (opnd0Type == Operand::kOpdMem) {
      resOpnd = &SelectCopy(opnd0, toType, toType);
      return resOpnd;
    }
    /* according to aarch64 encoding format, convert int to float expression */
    bool isImm = false;
    ImmOperand *imm = static_cast<ImmOperand*>(&opnd0);
    uint64 val = static_cast<uint64>(imm->GetValue());
    uint64 canRepreset = is64Bits ? (val & 0xffffffffffff) : (val & 0x7ffff);
    uint32 val1 = is64Bits ? (val >> 61) & 0x3 : (val >> 29) & 0x3;
    uint32 val2 = is64Bits ? (val >> 54) & 0xff : (val >> 25) & 0x1f;
    bool isSame = is64Bits ? ((val2 == 0) || (val2 == 0xff)) : ((val2 == 0) || (val2 == 0x1f));
    canRepreset = (canRepreset == 0) && ((val1 & 0x1) ^ ((val1 & 0x2) >> 1)) && isSame;
    Operand *newOpnd0 = &opnd0;
    if (IsPrimitiveInteger(fromType) && IsPrimitiveFloat(toType) && canRepreset) {
      uint64 temp1 = is64Bits ? (val >> 63) << 7 : (val >> 31) << 7;
      uint64 temp2 = is64Bits ? val >> 48 : val >> 19;
      int64 imm8 = (temp2 & 0x7f) | temp1;
      newOpnd0 = &CreateImmOperand(imm8, k8BitSize, false, kNotVary, true);
      isImm = true;
    } else {
      newOpnd0 = &LoadIntoRegister(opnd0, itype);
    }
    uint32 mopFmov =
        isImm ? is64Bits ? MOP_xdfmovri : MOP_wsfmovri
              : isFromInt ? (is64Bits ? MOP_xvmovdr : MOP_xvmovsr) : (is64Bits ? MOP_xvmovrd : MOP_xvmovrs);
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mopFmov, *resOpnd, *newOpnd0));
    return resOpnd;
  } else {
    CHECK_FATAL(false, "NYI retype");
  }
  return nullptr;
}

void AArch64CGFunc::SelectCvtFloat2Float(Operand &resOpnd, Operand &srcOpnd, PrimType fromType, PrimType toType) {
  Operand &opnd0 = LoadIntoRegister(srcOpnd, fromType);
  MOperator mOp = 0;
  switch (toType) {
    case PTY_f32: {
      CHECK_FATAL(fromType == PTY_f64, "unexpected cvt from type");
      mOp = MOP_xvcvtfd;
      break;
    }
    case PTY_f64: {
      CHECK_FATAL(fromType == PTY_f32, "unexpected cvt from type");
      mOp = MOP_xvcvtdf;
      break;
    }
    default:
      CHECK_FATAL(false, "unexpected cvt to type");
  }

  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, resOpnd, opnd0));
}

/*
 * This should be regarded only as a reference.
 *
 * C11 specification.
 * 6.3.1.3 Signed and unsigned integers
 * 1 When a value with integer type is converted to another integer
 *  type other than _Bool, if the value can be represented by the
 *  new type, it is unchanged.
 * 2 Otherwise, if the new type is unsigned, the value is converted
 *  by repeatedly adding or subtracting one more than the maximum
 *  value that can be represented in the new type until the value
 *  is in the range of the new type.60)
 * 3 Otherwise, the new type is signed and the value cannot be
 *  represented in it; either the result is implementation-defined
 *  or an implementation-defined signal is raised.
 */
void AArch64CGFunc::SelectCvtInt2Int(const BaseNode *parent, Operand *&resOpnd, Operand *opnd0, PrimType fromType,
                                     PrimType toType) {
  uint32 fsize = GetPrimTypeBitSize(fromType);
  uint32 tsize = GetPrimTypeBitSize(toType);
  bool isExpand = tsize > fsize;
  bool is64Bit = (tsize == k64BitSize);
  if ((parent != nullptr) && opnd0->IsIntImmediate() &&
      ((parent->GetOpCode() == OP_band) || (parent->GetOpCode() == OP_bior) || (parent->GetOpCode() == OP_bxor) ||
       (parent->GetOpCode() == OP_ashr) || (parent->GetOpCode() == OP_lshr) || (parent->GetOpCode() == OP_shl))) {
    ImmOperand *simm = static_cast<ImmOperand*>(opnd0);
    ASSERT(simm != nullptr, "simm is nullptr in AArch64CGFunc::SelectCvtInt2Int");
    bool isSign = false;
    int64 origValue = simm->GetValue();
    int64 newValue = origValue;
    int64 signValue = 0;
    if (!isExpand) {
      /* 64--->32 */
      if (fsize > tsize) {
        if (IsSignedInteger(toType)) {
          if (origValue < 0) {
            signValue = 0xFFFFFFFFFFFFFFFF & (1ULL << static_cast<uint32>(tsize));
          }
          newValue = static_cast<uint64>(origValue) & ((1ULL << static_cast<uint32>(tsize)) - 1u) &
                     static_cast<uint64>(signValue);
        } else {
          newValue = static_cast<uint64>(origValue) & ((1ULL << static_cast<uint32>(tsize)) - 1u);
        }
      }
    }
    if (IsSignedInteger(toType)) {
      isSign = true;
    }
    resOpnd = &static_cast<Operand&>(CreateImmOperand(newValue, GetPrimTypeSize(toType) * kBitsPerByte, isSign));
    return;
  }
  if (isExpand) {  /* Expansion */
    /* if cvt expr's parent is add,and,xor and some other,we can use the imm version */
    PrimType primType =
      ((fsize == k64BitSize) ? (IsSignedInteger(fromType) ? PTY_i64 : PTY_u64) : (IsSignedInteger(fromType) ?
                                                                                PTY_i32 : PTY_u32));
    opnd0 = &LoadIntoRegister(*opnd0, primType);

    if (IsSignedInteger(fromType)) {
      ASSERT((is64Bit || (fsize == k8BitSize || fsize == k16BitSize)), "incorrect from size");

      MOperator mOp =
          (is64Bit ? ((fsize == k8BitSize) ? MOP_xsxtb64 : ((fsize == k16BitSize) ? MOP_xsxth64 : MOP_xsxtw64))
                   : ((fsize == k8BitSize) ? MOP_xsxtb32 : MOP_xsxth32));
      GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, *resOpnd, *opnd0));
    } else {
      /* Unsigned */
      if (is64Bit) {
        GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_xuxtw64, *resOpnd, *opnd0));
      } else {
        ASSERT(((fsize == k8BitSize) || (fsize == k16BitSize)), "incorrect from size");
        if (fsize == k8BitSize) {
          static_cast<RegOperand*>(opnd0)->SetValidBitsNum(k8BitSize);
          static_cast<RegOperand*>(resOpnd)->SetValidBitsNum(k8BitSize);
        }
        if (fromType == PTY_u1) {
          static_cast<RegOperand*>(opnd0)->SetValidBitsNum(1);
          static_cast<RegOperand*>(resOpnd)->SetValidBitsNum(1);
        }
        GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>((fsize == k8BitSize) ? MOP_xuxtb32 : MOP_xuxth32,
                                                                      *resOpnd, *opnd0));
      }
    }
  } else {  /* Same size or truncate */
#ifdef CNV_OPTIMIZE
    /*
     * No code needed for aarch64 with same reg.
     * Just update regno.
     */
    RegOperand *reg = static_cast<RegOperand*>(resOpnd);
    reg->regNo = static_cast<RegOperand*>(opnd0)->regNo;
#else
    /*
     *  This is not really needed if opnd0 is result from a load.
     * Hopefully the FE will get rid of the redundant conversions for loads.
     */
    PrimType primType = ((fsize == k64BitSize) ? (IsSignedInteger(fromType) ? PTY_i64 : PTY_u64)
                                               : (IsSignedInteger(fromType) ? PTY_i32 : PTY_u32));
    opnd0 = &LoadIntoRegister(*opnd0, primType);

    if (fsize > tsize) {
      if (fsize == k64BitSize) {
        MOperator mOp = IsSignedInteger(toType) ? MOP_xsbfxrri6i6 : MOP_xubfxrri6i6;
        GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, *resOpnd, *opnd0,
                                                                      CreateImmOperand(0, k8BitSize, false),
                                                                      CreateImmOperand(tsize, k8BitSize, false)));
      } else {
        MOperator mOp = IsSignedInteger(toType) ? MOP_wsbfxrri5i5 : MOP_wubfxrri5i5;
        GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, *resOpnd, *opnd0,
                                                                      CreateImmOperand(0, k8BitSize, false),
                                                                      CreateImmOperand(tsize, k8BitSize, false)));
      }
    } else {
      /* same size, so resOpnd can be set */
      AArch64RegOperand *reg = static_cast<AArch64RegOperand*>(resOpnd);
      reg->SetRegisterNumber(static_cast<AArch64RegOperand*>(opnd0)->GetRegisterNumber());
    }
#endif
  }
}

Operand *AArch64CGFunc::SelectCvt(const BaseNode &parent, TypeCvtNode &node, Operand &opnd0) {
  PrimType fromType = node.FromType();
  PrimType toType = node.GetPrimType();
  if (fromType == toType) {
    return &opnd0;  /* noop */
  }
  Operand *resOpnd = &static_cast<Operand&>(CreateRegisterOperandOfType(toType));
  if (IsPrimitiveFloat(toType) && IsPrimitiveInteger(fromType)) {
    SelectCvtInt2Float(*resOpnd, opnd0, toType, fromType);
  } else if (IsPrimitiveFloat(fromType) && IsPrimitiveInteger(toType)) {
    SelectCvtFloat2Int(*resOpnd, opnd0, toType, fromType);
  } else if (IsPrimitiveInteger(fromType) && IsPrimitiveInteger(toType)) {
    SelectCvtInt2Int(&parent, resOpnd, &opnd0, fromType, toType);
  } else {  /* both are float type */
    SelectCvtFloat2Float(*resOpnd, opnd0, fromType, toType);
  }
  return resOpnd;
}

Operand *AArch64CGFunc::SelectTrunc(TypeCvtNode &node, Operand &opnd0) {
  PrimType ftype = node.FromType();
  bool is64Bits = (GetPrimTypeBitSize(node.GetPrimType()) == k64BitSize);
  PrimType itype = (is64Bits) ? (IsSignedInteger(node.GetPrimType()) ? PTY_i64 : PTY_u64)
                              : (IsSignedInteger(node.GetPrimType()) ? PTY_i32 : PTY_u32);  /* promoted type */
  RegOperand &resOpnd = CreateRegisterOperandOfType(itype);
  SelectCvtFloat2Int(resOpnd, opnd0, itype, ftype);
  return &resOpnd;
}

void AArch64CGFunc::SelectSelect(Operand &resOpnd, Operand &condOpnd, Operand &trueOpnd, Operand &falseOpnd,
                                 PrimType dtype, PrimType ctype) {
  ASSERT(&resOpnd != &condOpnd, "resOpnd cannot be the same as condOpnd");
  Operand &newCondOpnd = LoadIntoRegister(condOpnd, ctype);
  Operand &newTrueOpnd = LoadIntoRegister(trueOpnd, dtype);
  Operand &newFalseOpnd = LoadIntoRegister(falseOpnd, dtype);

  bool isIntType = IsPrimitiveInteger(dtype);

  SelectAArch64Cmp(newCondOpnd, CreateImmOperand(0, ctype, false), true, GetPrimTypeBitSize(ctype));
  ASSERT((IsPrimitiveInteger(dtype) || IsPrimitiveFloat(dtype)), "unknown type for select");
  Operand &newResOpnd = LoadIntoRegister(resOpnd, dtype);
  SelectAArch64Select(newResOpnd, newTrueOpnd, newFalseOpnd,
                      GetCondOperand(CC_NE), isIntType, GetPrimTypeBitSize(dtype));
}

Operand *AArch64CGFunc::SelectSelect(TernaryNode &node, Operand &opnd0, Operand &opnd1, Operand &opnd2) {
  PrimType dtype = node.GetPrimType();
  PrimType ctype = node.Opnd(0)->GetPrimType();
  RegOperand &resOpnd = CreateRegisterOperandOfType(dtype);
  SelectSelect(resOpnd, opnd0, opnd1, opnd2, dtype, ctype);
  return &resOpnd;
}

/*
 * syntax: select <prim-type> (<opnd0>, <opnd1>, <opnd2>)
 * <opnd0> must be of integer type.
 * <opnd1> and <opnd2> must be of the type given by <prim-type>.
 * If <opnd0> is not 0, return <opnd1>.  Otherwise, return <opnd2>.
 */
void AArch64CGFunc::SelectAArch64Select(Operand &dest, Operand &o0, Operand &o1, CondOperand &cond, bool isIntType,
                                        uint32 dsize) {
  uint32 mOpCode = isIntType ? ((dsize == k64BitSize) ? MOP_xcselrrrc : MOP_wcselrrrc)
                             : ((dsize == k64BitSize) ? MOP_dcselrrrc
                                                      : ((dsize == k32BitSize) ? MOP_scselrrrc : MOP_hcselrrrc));
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOpCode, dest, o0, o1, cond));
}

void AArch64CGFunc::SelectRangeGoto(RangeGotoNode &rangeGotoNode, Operand &srcOpnd) {
  const SmallCaseVector &switchTable = rangeGotoNode.GetRangeGotoTable();
  MIRType *etype = GlobalTables::GetTypeTable().GetTypeFromTyIdx((TyIdx)PTY_a64);
  /*
   * we store 8-byte displacement ( jump_label - offset_table_address )
   * in the table. Refer to AArch64Emit::Emit() in aarch64emit.cpp
   */
  std::vector<uint32> sizeArray;
  sizeArray.push_back(switchTable.size());
  MIRArrayType *arrayType = memPool->New<MIRArrayType>(etype->GetTypeIndex(), sizeArray);
  MIRAggConst *arrayConst = memPool->New<MIRAggConst>(mirModule, *arrayType);
  for (const auto &itPair : switchTable) {
    LabelIdx labelIdx = itPair.second;
    GetCurBB()->PushBackRangeGotoLabel(labelIdx);
    MIRConst *mirConst = memPool->New<MIRLblConst>(labelIdx, *etype);
    arrayConst->PushBack(mirConst);
  }

  MIRSymbol *lblSt = GetFunction().GetSymTab()->CreateSymbol(kScopeLocal);
  lblSt->SetStorageClass(kScFstatic);
  lblSt->SetSKind(kStConst);
  lblSt->SetTyIdx(arrayType->GetTypeIndex());
  lblSt->SetKonst(arrayConst);
  std::string lblStr(".LB_");
  MIRSymbol *funcSt = GlobalTables::GetGsymTable().GetSymbolFromStidx(GetFunction().GetStIdx().Idx());
  uint32 labelIdxTmp = GetLabelIdx();
  lblStr.append(funcSt->GetName()).append(std::to_string(labelIdxTmp++));
  SetLabelIdx(labelIdxTmp);
  lblSt->SetNameStrIdx(lblStr);
  AddEmitSt(*lblSt);

  PrimType itype = rangeGotoNode.Opnd(0)->GetPrimType();
  Operand &opnd0 = LoadIntoRegister(srcOpnd, itype);

  regno_t vRegNO = NewVReg(kRegTyInt, 8u);
  RegOperand *addOpnd = &CreateVirtualRegisterOperand(vRegNO);

  int32 minIdx = switchTable[0].first;
  SelectAdd(*addOpnd, opnd0,
            CreateImmOperand(-minIdx - rangeGotoNode.GetTagOffset(), GetPrimTypeBitSize(itype), true), itype);

  /* contains the index */
  if (addOpnd->GetSize() != GetPrimTypeBitSize(PTY_u64)) {
    addOpnd = static_cast<AArch64RegOperand*>(&SelectCopy(*addOpnd, PTY_u64, PTY_u64));
  }

  RegOperand &baseOpnd = CreateRegisterOperandOfType(PTY_u64);
  StImmOperand &stOpnd = CreateStImmOperand(*lblSt, 0, 0);

  /* load the address of the switch table */
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_xadrp, baseOpnd, stOpnd));
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_xadrpl12, baseOpnd, baseOpnd, stOpnd));

  /* load the displacement into a register by accessing memory at base + index*8 */
  Operand *disp =
      memPool->New<AArch64MemOperand>(AArch64MemOperand::kAddrModeBOrX, k64BitSize, baseOpnd, *addOpnd, k8BitShift);
  RegOperand &tgt = CreateRegisterOperandOfType(PTY_a64);
  SelectAdd(tgt, baseOpnd, *disp, PTY_u64);
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_xbr, tgt));
}

Operand *AArch64CGFunc::SelectLazyLoad(Operand &opnd0, PrimType primType) {
  ASSERT(opnd0.IsRegister(), "wrong type.");
  RegOperand &resOpnd = CreateRegisterOperandOfType(primType);
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_lazy_ldr, resOpnd, opnd0));
  return &resOpnd;
}

Operand *AArch64CGFunc::SelectLazyLoadStatic(MIRSymbol &st, int64 offset, PrimType primType) {
  StImmOperand &srcOpnd = CreateStImmOperand(st, offset, 0);
  RegOperand &resOpnd = CreateRegisterOperandOfType(primType);
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_lazy_ldr_static, resOpnd, srcOpnd));
  return &resOpnd;
}

Operand *AArch64CGFunc::SelectAlloca(UnaryNode &node, Operand &opnd0) {
  ASSERT((node.GetPrimType() == PTY_a64), "wrong type");
  PrimType stype = node.Opnd(0)->GetPrimType();
  Operand *resOpnd = &opnd0;
  if (GetPrimTypeBitSize(stype) < GetPrimTypeBitSize(PTY_u64)) {
    resOpnd = &CreateRegisterOperandOfType(PTY_u64);
    SelectCvtInt2Int(nullptr, resOpnd, &opnd0, stype, PTY_u64);
  }

  RegOperand &aliOp = CreateRegisterOperandOfType(PTY_u64);

  SelectAdd(aliOp, *resOpnd, CreateImmOperand(kAarch64StackPtrAlignment - 1, k64BitSize, true), PTY_u64);
  Operand &shifOpnd = CreateImmOperand(__builtin_ctz(kAarch64StackPtrAlignment), k64BitSize, true);
  SelectShift(aliOp, aliOp, shifOpnd, kShiftLright, PTY_u64);
  SelectShift(aliOp, aliOp, shifOpnd, kShiftLeft, PTY_u64);
  Operand &spOpnd = GetOrCreatePhysicalRegisterOperand(RSP, k64BitSize, kRegTyInt);
  SelectSub(spOpnd, spOpnd, aliOp, PTY_u64);
  int64 argsToStkpassSize = GetMemlayout()->SizeOfArgsToStackPass();
  if (argsToStkpassSize > 0) {
    RegOperand &resallo = CreateRegisterOperandOfType(PTY_u64);
    SelectAdd(resallo, spOpnd, CreateImmOperand(argsToStkpassSize, k64BitSize, true), PTY_u64);
    return &resallo;
  } else {
    return &SelectCopy(spOpnd, PTY_u64, PTY_u64);
  }
}

Operand *AArch64CGFunc::SelectMalloc(UnaryNode &node, Operand &opnd0) {
  PrimType retType = node.GetPrimType();
  ASSERT((retType == PTY_a64), "wrong type");

  std::vector<Operand*> opndVec;
  RegOperand &resOpnd = CreateRegisterOperandOfType(retType);
  opndVec.push_back(&resOpnd);
  opndVec.push_back(&opnd0);
  /* Use calloc to make sure allocated memory is zero-initialized */
  const std::string &funcName = "calloc";
  Operand &opnd1 = CreateImmOperand(1, PTY_u32, false);
  opndVec.push_back(&opnd1);
  SelectLibCall(funcName, opndVec, PTY_u32, retType);
  return &resOpnd;
}

Operand *AArch64CGFunc::SelectGCMalloc(GCMallocNode &node) {
  PrimType retType = node.GetPrimType();
  ASSERT((retType == PTY_a64), "wrong type");

  /* Get the size and alignment of the type. */
  TyIdx tyIdx = node.GetTyIdx();
  uint64 size = GetBecommon().GetTypeSize(tyIdx);
  uint8 align = AArch64RTSupport::kObjectAlignment;

  /* Generate the call to MCC_NewObj */
  Operand &opndSize = CreateImmOperand(size, k64BitSize, false);
  Operand &opndAlign = CreateImmOperand(align, k64BitSize, false);

  RegOperand &resOpnd = CreateRegisterOperandOfType(retType);

  std::vector<Operand*> opndVec{ &resOpnd, &opndSize, &opndAlign };

  const std::string &funcName = "MCC_NewObj";
  SelectLibCall(funcName, opndVec, PTY_u64, retType);

  return &resOpnd;
}

Operand *AArch64CGFunc::SelectJarrayMalloc(JarrayMallocNode &node, Operand &opnd0) {
  PrimType retType = node.GetPrimType();
  ASSERT((retType == PTY_a64), "wrong type");

  /* Extract jarray type */
  TyIdx tyIdx = node.GetTyIdx();
  MIRType *type = GlobalTables::GetTypeTable().GetTypeFromTyIdx(tyIdx);
  ASSERT(type != nullptr, "nullptr check");
  CHECK_FATAL(type->GetKind() == kTypeJArray, "expect MIRJarrayType");
  auto jaryType = static_cast<MIRJarrayType*>(type);
  uint64 fixedSize = AArch64RTSupport::kArrayContentOffset;
  uint8 align = AArch64RTSupport::kObjectAlignment;

  MIRType *elemType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(jaryType->GetElemTyIdx());
  PrimType elemPrimType = elemType->GetPrimType();
  uint64 elemSize = GetPrimTypeSize(elemPrimType);

  /* Generate the cal to MCC_NewObj_flexible */
  Operand &opndFixedSize = CreateImmOperand(PTY_u64, fixedSize);
  Operand &opndElemSize = CreateImmOperand(PTY_u64, elemSize);

  Operand *opndNElems = &opnd0;

  Operand *opndNElems64 = &static_cast<Operand&>(CreateRegisterOperandOfType(PTY_u64));
  SelectCvtInt2Int(nullptr, opndNElems64, opndNElems, PTY_u32, PTY_u64);

  Operand &opndAlign = CreateImmOperand(PTY_u64, align);

  RegOperand &resOpnd = CreateRegisterOperandOfType(retType);

  std::vector<Operand*> opndVec{ &resOpnd, &opndFixedSize, &opndElemSize, opndNElems64, &opndAlign };

  const std::string &funcName = "MCC_NewObj_flexible";
  SelectLibCall(funcName, opndVec, PTY_u64, retType);

  /* Generate the store of the object length field */
  MemOperand &opndArrayLengthField = CreateMemOpnd(resOpnd, AArch64RTSupport::kArrayLengthOffset, k4BitSize);
  RegOperand *regOpndNElems = &SelectCopy(*opndNElems, PTY_u32, PTY_u32);
  ASSERT(regOpndNElems != nullptr, "null ptr check!");
  SelectCopy(opndArrayLengthField, PTY_u32, *regOpndNElems, PTY_u32);

  return &resOpnd;
}

Operand &AArch64CGFunc::GetZeroOpnd(uint32 size) {
  return AArch64RegOperand::GetZeroRegister(size <= k32BitSize ? k32BitSize : k64BitSize);
}

bool AArch64CGFunc::IsFrameReg(const RegOperand &opnd) const {
  if (opnd.GetRegisterNumber() == RFP) {
    return true;
  } else {
    return false;
  }
}

/*
 * This function returns true to indicate that the clean up code needs to be generated,
 * otherwise it does not need. In GCOnly mode, it always returns false.
 */
bool AArch64CGFunc::NeedCleanup() {
  if (CGOptions::IsGCOnly()) {
    return false;
  }
  AArch64MemLayout *layout = static_cast<AArch64MemLayout*>(GetMemlayout());
  if (layout->GetSizeOfRefLocals() > 0) {
    return true;
  }
  for (uint32 i = 0; i < GetFunction().GetFormalCount(); i++) {
    TypeAttrs ta = GetFunction().GetNthParamAttr(i);
    if (ta.GetAttr(ATTR_localrefvar)) {
      return true;
    }
  }

  return false;
}

/*
 * bb must be the cleanup bb.
 * this function must be invoked before register allocation.
 * extended epilogue is specific for fast exception handling and is made up of
 * clean up code and epilogue.
 * clean up code is generated here while epilogue is generated in GeneratePrologEpilog()
 */
void AArch64CGFunc::GenerateCleanupCodeForExtEpilog(BB &bb) {
  ASSERT(GetLastBB()->GetPrev()->GetFirstStmt() == GetCleanupLabel(), "must be");

  if (NeedCleanup()) {
    /* this is necessary for code insertion. */
    SetCurBB(bb);

    AArch64RegOperand &regOpnd0 =
        GetOrCreatePhysicalRegisterOperand(R0, kSizeOfPtr * kBitsPerByte, GetRegTyFromPrimTy(PTY_a64));
    AArch64RegOperand &regOpnd1 =
        GetOrCreatePhysicalRegisterOperand(R1, kSizeOfPtr * kBitsPerByte, GetRegTyFromPrimTy(PTY_a64));
    /* allocate 16 bytes to store reg0 and reg1 (each reg has 8 bytes) */
    AArch64MemOperand &frameAlloc = CreateCallFrameOperand(-16, kSizeOfPtr * kBitsPerByte);
    Insn &allocInsn = GetCG()->BuildInstruction<AArch64Insn>(MOP_xstp, regOpnd0, regOpnd1, frameAlloc);
    allocInsn.SetDoNotRemove(true);
    AppendInstructionTo(allocInsn, *this);

    /* invoke MCC_CleanupLocalStackRef(). */
    HandleRCCall(false);
    /* deallocate 16 bytes which used to store reg0 and reg1 */
    AArch64MemOperand &frameDealloc = CreateCallFrameOperand(16, kSizeOfPtr * kBitsPerByte);
    GenRetCleanup(cleanEANode, true);
    Insn &deallocInsn = GetCG()->BuildInstruction<AArch64Insn>(MOP_xldp, regOpnd0, regOpnd1, frameDealloc);
    deallocInsn.SetDoNotRemove(true);
    AppendInstructionTo(deallocInsn, *this);
    /* Update cleanupbb since bb may have been splitted */
    SetCleanupBB(*GetCurBB());
  }
}

/*
 * bb must be the cleanup bb.
 * this function must be invoked before register allocation.
 */
void AArch64CGFunc::GenerateCleanupCode(BB &bb) {
  ASSERT(GetLastBB()->GetPrev()->GetFirstStmt() == GetCleanupLabel(), "must be");
  if (!NeedCleanup()) {
    return;
  }

  /* this is necessary for code insertion. */
  SetCurBB(bb);

  /* R0 is lived-in for clean-up code, save R0 before invocation */
  AArch64RegOperand &livein = GetOrCreatePhysicalRegisterOperand(R0, k64BitSize, GetRegTyFromPrimTy(PTY_a64));

  if (!GetCG()->GenLocalRC()) {
    /* by pass local RC operations. */
  } else if (Globals::GetInstance()->GetOptimLevel() > 0) {
    regno_t vreg = NewVReg(GetRegTyFromPrimTy(PTY_a64), GetPrimTypeSize(PTY_a64));
    RegOperand &backupRegOp = CreateVirtualRegisterOperand(vreg);
    backupRegOp.SetRegNotBBLocal();
    SelectCopy(backupRegOp, PTY_a64, livein, PTY_a64);

    /* invoke MCC_CleanupLocalStackRef(). */
    HandleRCCall(false);
    SelectCopy(livein, PTY_a64, backupRegOp, PTY_a64);
  } else {
    /*
     * Register Allocation for O0 can not handle this case, so use a callee saved register directly.
     * If yieldpoint is enabled, we use R20 instead R19.
     */
    AArch64reg backupRegNO = GetCG()->GenYieldPoint() ? R20 : R19;
    RegOperand &backupRegOp = GetOrCreatePhysicalRegisterOperand(backupRegNO, k64BitSize, GetRegTyFromPrimTy(PTY_a64));
    SelectCopy(backupRegOp, PTY_a64, livein, PTY_a64);
    /* invoke MCC_CleanupLocalStackRef(). */
    HandleRCCall(false);
    SelectCopy(livein, PTY_a64, backupRegOp, PTY_a64);
  }

  /* invoke _Unwind_Resume */
  std::string funcName("_Unwind_Resume");
  MIRSymbol *sym = GlobalTables::GetGsymTable().CreateSymbol(kScopeGlobal);
  sym->SetNameStrIdx(funcName);
  sym->SetStorageClass(kScText);
  sym->SetSKind(kStFunc);
  AArch64ListOperand *srcOpnds = memPool->New<AArch64ListOperand>(*GetFuncScopeAllocator());
  srcOpnds->PushOpnd(livein);
  AppendCall(*sym, *srcOpnds);
  /*
   * this instruction is unreachable, but we need it as the return address of previous
   * "bl _Unwind_Resume" for stack unwinding.
   */
  Insn &nop = GetCG()->BuildInstruction<AArch64Insn>(MOP_xblr, livein, *srcOpnds);
  GetCurBB()->AppendInsn(nop);
  GetCurBB()->SetHasCall();

  /* Update cleanupbb since bb may have been splitted */
  SetCleanupBB(*GetCurBB());
}

/* if offset < 0, allocation; otherwise, deallocation */
AArch64MemOperand &AArch64CGFunc::CreateCallFrameOperand(int32 offset, int32 size) {
  return *memPool->New<AArch64MemOperand>(RSP, offset, size,
                                          (offset < 0) ? AArch64MemOperand::kPreIndex : AArch64MemOperand::kPostIndex);
}

AArch64CGFunc::MovkLslOperandArray AArch64CGFunc::movkLslOperands = {
  LogicalShiftLeftOperand(0, 4),          LogicalShiftLeftOperand(16, 4),
  LogicalShiftLeftOperand(static_cast<uint32>(-1), 0),  /* invalid entry */
  LogicalShiftLeftOperand(static_cast<uint32>(-1), 0),  /* invalid entry */
  LogicalShiftLeftOperand(0, 6),          LogicalShiftLeftOperand(16, 6),
  LogicalShiftLeftOperand(32, 6),         LogicalShiftLeftOperand(48, 6),
};

/* kShiftAmount12 = 12, less than 16, use 4 bit to store, bitLen is 4 */
LogicalShiftLeftOperand AArch64CGFunc::addSubLslOperand(kShiftAmount12, 4);

AArch64MemOperand &AArch64CGFunc::CreateStkTopOpnd(int32 offset, int32 size) {
  return *memPool->New<AArch64MemOperand>(RFP, offset, size);
}

void AArch64CGFunc::GenSaveMethodInfoCode(BB &bb) {
  if (GetCG()->UseFastUnwind()) {
    BB *formerCurBB = GetCurBB();
    GetDummyBB()->ClearInsns();
    SetCurBB(*GetDummyBB());
    /*
     * FUNCATTR_bridge for function: Ljava_2Flang_2FString_3B_7CcompareTo_7C_28Ljava_2Flang_2FObject_3B_29I, to
     * exclude this funciton this function is a bridge function generated for Java Genetic
     */
    if ((GetFunction().GetAttr(FUNCATTR_native) || GetFunction().GetAttr(FUNCATTR_fast_native)) &&
        !GetFunction().GetAttr(FUNCATTR_critical_native) && !GetFunction().GetAttr(FUNCATTR_bridge)) {
      RegOperand &fpReg = GetOrCreatePhysicalRegisterOperand(RFP, kSizeOfPtr * kBitsPerByte, kRegTyInt);

      AArch64ListOperand *srcOpnds = memPool->New<AArch64ListOperand>(*GetFuncScopeAllocator());
      AArch64RegOperand &parmRegOpnd1 = GetOrCreatePhysicalRegisterOperand(R0, k64BitSize, kRegTyInt);
      srcOpnds->PushOpnd(parmRegOpnd1);
      Operand &immOpnd = CreateImmOperand(0, k64BitSize, false);
      GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_xadri64, parmRegOpnd1, immOpnd));
      AArch64RegOperand &parmRegOpnd2 = GetOrCreatePhysicalRegisterOperand(R1, k64BitSize, kRegTyInt);
      srcOpnds->PushOpnd(parmRegOpnd2);
      SelectCopy(parmRegOpnd2, PTY_a64, fpReg, PTY_a64);

      MIRSymbol *sym = GlobalTables::GetGsymTable().CreateSymbol(kScopeGlobal);
      std::string funcName("MCC_SetRiskyUnwindContext");
      sym->SetNameStrIdx(funcName);

      sym->SetStorageClass(kScText);
      sym->SetSKind(kStFunc);
      AppendCall(*sym, *srcOpnds);
      bb.SetHasCall();
    }

    bb.InsertAtBeginning(*GetDummyBB());
    SetCurBB(*formerCurBB);
  }
}

bool AArch64CGFunc::HasStackLoadStore() {
  FOR_ALL_BB(bb, this) {
    FOR_BB_INSNS(insn, bb) {
      uint32 opndNum = insn->GetOperandSize();
      for (uint32 i = 0; i < opndNum; ++i) {
        Operand &opnd = insn->GetOperand(i);
        if (opnd.IsMemoryAccessOperand()) {
          auto &memOpnd = static_cast<MemOperand&>(opnd);
          Operand *base = memOpnd.GetBaseRegister();

          if ((base != nullptr) && base->IsRegister()) {
            RegOperand *regOpnd = static_cast<RegOperand*>(base);
            RegType regType = regOpnd->GetRegisterType();
            uint32 regNO = regOpnd->GetRegisterNumber();
            if (((regType != kRegTyCc) && ((regNO == R29) || (regNO == RSP))) || (regType == kRegTyVary)) {
              return true;
            }
          }
        }
      }
    }
  }
  return false;
}

void AArch64CGFunc::GenerateYieldpoint(BB &bb) {
  /* ldr wzr, [RYP]  # RYP hold address of the polling page. */
  auto &wzr = AArch64RegOperand::Get32bitZeroRegister();
  auto &pollingPage = CreateMemOpnd(RYP, 0, k32BitSize);
  auto &yieldPoint = GetCG()->BuildInstruction<AArch64Insn>(MOP_wldr, wzr, pollingPage);
  if (GetCG()->GenerateVerboseAsm()) {
    yieldPoint.SetComment("yieldpoint");
  }
  bb.AppendInsn(yieldPoint);
}

Operand &AArch64CGFunc::GetTargetRetOperand(PrimType primType) {
  uint32 bitSize = GetPrimTypeBitSize(primType) < k32BitSize ? k32BitSize : GetPrimTypeBitSize(primType);
  return GetOrCreatePhysicalRegisterOperand(IsPrimitiveFloat(primType) ? S0 : R0, bitSize,
                                            GetRegTyFromPrimTy(primType));
}

RegOperand &AArch64CGFunc::CreateRegisterOperandOfType(PrimType primType) {
  RegType regType = GetRegTyFromPrimTy(primType);
  uint32 byteLength = GetPrimTypeSize(primType);
  return CreateRegisterOperandOfType(regType, byteLength);
}

RegOperand &AArch64CGFunc::CreateRegisterOperandOfType(RegType regty, uint32 byteLen) {
  /* BUG: if half-precision floating point operations are supported? */
  if (byteLen < k4ByteSize) {
    byteLen = k4ByteSize;  /* AArch64 has 32-bit and 64-bit registers only */
  }
  regno_t vRegNO = NewVReg(regty, byteLen);
  return CreateVirtualRegisterOperand(vRegNO);
}

RegOperand &AArch64CGFunc::CreateRflagOperand() {
  /* AArch64 has Status register that is 32-bit wide. */
  regno_t vRegNO = NewVRflag();
  return CreateVirtualRegisterOperand(vRegNO);
}

void AArch64CGFunc::MergeReturn() {
  ASSERT(GetCurBB()->GetPrev()->GetFirstStmt() == GetCleanupLabel(), "must be");

  uint32 exitBBSize = GetExitBBsVec().size();
  if (exitBBSize == 0) {
    return;
  }
  if ((exitBBSize == 1) && GetExitBB(0) == GetCurBB()) {
    return;
  }
  if (exitBBSize == 1) {
    BB *onlyExitBB = GetExitBB(0);
    BB *onlyExitBBNext = onlyExitBB->GetNext();
    StmtNode *stmt = onlyExitBBNext->GetFirstStmt();
    /* only deal with the return_BB in the middle */
    if (stmt != GetCleanupLabel()) {
      LabelIdx labidx = CreateLabel();
      BB *retBB = CreateNewBB(labidx, onlyExitBB->IsUnreachable(), BB::kBBReturn, onlyExitBB->GetFrequency());
      onlyExitBB->AppendBB(*retBB);
      /* modify the original return BB. */
      ASSERT(onlyExitBB->GetKind() == BB::kBBReturn, "Error: suppose to merge multi return bb");
      onlyExitBB->SetKind(BB::kBBFallthru);

      GetExitBBsVec().pop_back();
      GetExitBBsVec().push_back(retBB);
      return;
    }
  }

  LabelIdx labidx = CreateLabel();
  LabelOperand &targetOpnd = GetOrCreateLabelOperand(labidx);
  uint32 freq = 0;
  for (auto *tmpBB : GetExitBBsVec()) {
    ASSERT(tmpBB->GetKind() == BB::kBBReturn, "Error: suppose to merge multi return bb");
    tmpBB->SetKind(BB::kBBGoto);
    tmpBB->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_xuncond, targetOpnd));
    freq += tmpBB->GetFrequency();
  }
  BB *retBB = CreateNewBB(labidx, false, BB::kBBReturn, freq);
  GetCleanupBB()->PrependBB(*retBB);

  GetExitBBsVec().clear();
  GetExitBBsVec().push_back(retBB);
}

void AArch64CGFunc::HandleRetCleanup(NaryStmtNode &retNode) {
  if (!GetCG()->GenLocalRC()) {
    /* handle local rc is disabled. */
    return;
  }

  Opcode ops[11] = { OP_label, OP_goto,      OP_brfalse,   OP_brtrue,  OP_return, OP_call,
                     OP_icall, OP_rangegoto, OP_catch, OP_try, OP_endtry };
  std::set<Opcode> branchOp(ops, ops + 11);

  /* get cleanup intrinsic */
  bool found = false;
  StmtNode *cleanupNode = retNode.GetPrev();
  cleanEANode = nullptr;
  while (cleanupNode != nullptr) {
    if (branchOp.find(cleanupNode->GetOpCode()) != branchOp.end()) {
      if (cleanupNode->GetOpCode() == OP_call) {
        CallNode *callNode = static_cast<CallNode*>(cleanupNode);
        MIRFunction *fn = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(callNode->GetPUIdx());
        MIRSymbol *fsym = GetFunction().GetLocalOrGlobalSymbol(fn->GetStIdx(), false);
        if ((fsym->GetName() == "MCC_DecRef_NaiveRCFast") || (fsym->GetName() == "MCC_IncRef_NaiveRCFast") ||
            (fsym->GetName() == "MCC_IncDecRef_NaiveRCFast") || (fsym->GetName() == "MCC_LoadRefStatic") ||
            (fsym->GetName() == "MCC_LoadRefField") || (fsym->GetName() == "MCC_LoadReferentField") ||
            (fsym->GetName() == "MCC_LoadRefField_NaiveRCFast") || (fsym->GetName() == "MCC_LoadVolatileField") ||
            (fsym->GetName() == "MCC_LoadVolatileStaticField") || (fsym->GetName() == "MCC_LoadWeakField") ||
            (fsym->GetName() == "MCC_CheckObjMem")) {
          cleanupNode = cleanupNode->GetPrev();
          continue;
        } else {
          break;
        }
      } else {
        break;
      }
    }

    if (OP_intrinsiccall == cleanupNode->GetOpCode()) {
      IntrinsiccallNode *tempNode = static_cast<IntrinsiccallNode*>(cleanupNode);
      if ((tempNode->GetIntrinsic() == INTRN_MPL_CLEANUP_LOCALREFVARS) ||
          (tempNode->GetIntrinsic() == INTRN_MPL_CLEANUP_LOCALREFVARS_SKIP)) {
        GenRetCleanup(tempNode);
        if (cleanEANode != nullptr) {
          GenRetCleanup(cleanEANode, true);
        }
        found = true;
        break;
      }
      if (tempNode->GetIntrinsic() == INTRN_MPL_CLEANUP_NORETESCOBJS) {
        cleanEANode = tempNode;
      }
    }
    cleanupNode = cleanupNode->GetPrev();
  }

  if (!found) {
    MIRSymbol *retRef = nullptr;
    if (retNode.NumOpnds() != 0) {
      retRef = GetRetRefSymbol(*static_cast<NaryStmtNode&>(retNode).Opnd(0));
    }
    HandleRCCall(false, retRef);
  }
}

bool AArch64CGFunc::GenRetCleanup(const IntrinsiccallNode *cleanupNode, bool forEA) {
#undef CC_DEBUG_INFO

#ifdef CC_DEBUG_INFO
  LogInfo::MapleLogger() << "==============" << GetFunction().GetName() << "==============" << '\n';
#endif

  if (cleanupNode == nullptr) {
    return false;
  }

  int32 minByteOffset = INT_MAX;
  int32 maxByteOffset = 0;

  int32 skipIndex = -1;
  MIRSymbol *skipSym = nullptr;
  size_t refSymNum = 0;
  if (cleanupNode->GetIntrinsic() == INTRN_MPL_CLEANUP_LOCALREFVARS) {
    refSymNum = cleanupNode->GetNopndSize();
    if (refSymNum < 1) {
      return true;
    }
  } else if (cleanupNode->GetIntrinsic() == INTRN_MPL_CLEANUP_LOCALREFVARS_SKIP) {
    refSymNum = cleanupNode->GetNopndSize();
    /* refSymNum == 0, no local refvars; refSymNum == 1 and cleanup skip, so nothing to do */
    if (refSymNum < 2) {
      return true;
    }
    BaseNode *skipExpr = cleanupNode->Opnd(refSymNum - 1);

    CHECK_FATAL(skipExpr->GetOpCode() == OP_dread, "should be dread");
    DreadNode *refNode = static_cast<DreadNode*>(skipExpr);
    skipSym = GetFunction().GetLocalOrGlobalSymbol(refNode->GetStIdx());

    refSymNum -= 1;
  } else if (cleanupNode->GetIntrinsic() == INTRN_MPL_CLEANUP_NORETESCOBJS) {
    refSymNum = cleanupNode->GetNopndSize();
    /* the number of operands of intrinsic call INTRN_MPL_CLEANUP_NORETESCOBJS must be more than 1 */
    if (refSymNum < 2) {
      return true;
    }
    BaseNode *skipexpr = cleanupNode->Opnd(0);
    CHECK_FATAL(skipexpr->GetOpCode() == OP_dread, "should be dread");
    DreadNode *refnode = static_cast<DreadNode*>(skipexpr);
    skipSym = GetFunction().GetLocalOrGlobalSymbol(refnode->GetStIdx());
  }

  /* now compute the offset range */
  std::vector<int32> offsets;
  AArch64MemLayout *memLayout = static_cast<AArch64MemLayout*>(this->GetMemlayout());
  for (size_t i = 0; i < refSymNum; ++i) {
    BaseNode *argExpr = cleanupNode->Opnd(i);
    CHECK_FATAL(argExpr->GetOpCode() == OP_dread, "should be dread");
    DreadNode *refNode = static_cast<DreadNode*>(argExpr);
    MIRSymbol *refSymbol = GetFunction().GetLocalOrGlobalSymbol(refNode->GetStIdx());
    if (memLayout->GetSymAllocTable().size() <= refSymbol->GetStIndex()) {
      ERR(kLncErr, "access memLayout->GetSymAllocTable() failed");
      return false;
    }
    AArch64SymbolAlloc *symLoc =
          static_cast<AArch64SymbolAlloc*>(memLayout->GetSymAllocInfo(refSymbol->GetStIndex()));
    int32 tempOffset = GetBaseOffset(*symLoc);
    offsets.push_back(tempOffset);
#ifdef CC_DEBUG_INFO
    LogInfo::MapleLogger() << "refsym " << refSymbol->GetName() << " offset " << tempOffset << '\n';
#endif
    minByteOffset = (minByteOffset > tempOffset) ? tempOffset : minByteOffset;
    maxByteOffset = (maxByteOffset < tempOffset) ? tempOffset : maxByteOffset;
  }

  /* get the skip offset */
  int32 skipOffset = -1;
  if (skipSym != nullptr) {
    AArch64SymbolAlloc *symLoc = static_cast<AArch64SymbolAlloc*>(memLayout->GetSymAllocInfo(skipSym->GetStIndex()));
    CHECK_FATAL(GetBaseOffset(*symLoc) < std::numeric_limits<int32>::max(), "out of range");
    skipOffset = GetBaseOffset(*symLoc);
    offsets.push_back(skipOffset);

#ifdef CC_DEBUG_INFO
    LogInfo::MapleLogger() << "skip " << skipSym->GetName() << " offset " << skipOffset << '\n';
#endif

    skipIndex = symLoc->GetOffset() / kOffsetAlign;
  }

  /* call runtime cleanup */
  if (minByteOffset < INT_MAX) {
    int32 refLocBase = memLayout->GetRefLocBaseLoc();
    uint32 refNum = memLayout->GetSizeOfRefLocals() / kOffsetAlign;
    CHECK_FATAL((refLocBase + (refNum - 1) * kIntregBytelen) < std::numeric_limits<int32>::max(), "out of range");
    int32 refLocEnd = refLocBase + (refNum - 1) * kIntregBytelen;
    int32 realMin = minByteOffset < refLocBase ? refLocBase : minByteOffset;
    int32 realMax = maxByteOffset > refLocEnd ? refLocEnd : maxByteOffset;
    if (forEA) {
      std::sort(offsets.begin(), offsets.end());
      int32 prev = offsets[0];
      for (size_t i = 1; i < offsets.size(); i++) {
        CHECK_FATAL((offsets[i] == prev) || ((offsets[i] - prev) == kIntregBytelen), "must be");
        prev = offsets[i];
      }
      CHECK_FATAL((refLocBase - prev) == kIntregBytelen, "must be");
      realMin = minByteOffset;
      realMax = maxByteOffset;
    }
#ifdef CC_DEBUG_INFO
    LogInfo::MapleLogger() << " realMin " << realMin << " realMax " << realMax << '\n';
#endif
    if (realMax < realMin) {
      /* maybe there is a cleanup intrinsic bug, use CHECK_FATAL instead? */
      CHECK_FATAL(false, "must be");
    }

    /* optimization for little slot cleanup */
    if (realMax == realMin && !forEA) {
      RegOperand &phyOpnd = GetOrCreatePhysicalRegisterOperand(R0, k64BitSize, GetRegTyFromPrimTy(PTY_a64));
      Operand &stackLoc = CreateStkTopOpnd(realMin, kSizeOfPtr * kBitsPerByte);
      Insn &ldrInsn = GetCG()->BuildInstruction<AArch64Insn>(PickLdInsn(k64BitSize, PTY_a64), phyOpnd, stackLoc);
      GetCurBB()->AppendInsn(ldrInsn);

      AArch64ListOperand *srcOpnds = memPool->New<AArch64ListOperand>(*GetFuncScopeAllocator());
      srcOpnds->PushOpnd(phyOpnd);
      MIRSymbol *callSym = GlobalTables::GetGsymTable().CreateSymbol(kScopeGlobal);
      std::string funcName("MCC_DecRef_NaiveRCFast");
      callSym->SetNameStrIdx(funcName);
      callSym->SetStorageClass(kScText);
      callSym->SetSKind(kStFunc);
      Insn &callInsn = AppendCall(*callSym, *srcOpnds, true);
      static_cast<AArch64cleancallInsn&>(callInsn).SetRefSkipIndex(skipIndex);
      GetCurBB()->SetHasCall();
      /* because of return stmt is often the last stmt */
      GetCurBB()->SetFrequency(frequency);

      return true;
    }
    AArch64ListOperand *srcOpnds = memPool->New<AArch64ListOperand>(*GetFuncScopeAllocator());

    AArch64ImmOperand &beginOpnd = CreateImmOperand(realMin, k64BitSize, true);
    regno_t vRegNO0 = NewVReg(GetRegTyFromPrimTy(PTY_a64), GetPrimTypeSize(PTY_a64));
    RegOperand &vReg0 = CreateVirtualRegisterOperand(vRegNO0);
    RegOperand &fpOpnd = GetOrCreateStackBaseRegOperand();
    SelectAdd(vReg0, fpOpnd, beginOpnd, PTY_i64);

    AArch64RegOperand &parmRegOpnd1 = GetOrCreatePhysicalRegisterOperand(R0, k64BitSize, GetRegTyFromPrimTy(PTY_a64));
    srcOpnds->PushOpnd(parmRegOpnd1);
    SelectCopy(parmRegOpnd1, PTY_a64, vReg0, PTY_a64);

    uint32 realRefNum = (realMax - realMin) / kOffsetAlign + 1;

    AArch64ImmOperand &countOpnd = CreateImmOperand(realRefNum, k64BitSize, true);

    AArch64RegOperand &parmRegOpnd2 = GetOrCreatePhysicalRegisterOperand(R1, k64BitSize, GetRegTyFromPrimTy(PTY_a64));
    srcOpnds->PushOpnd(parmRegOpnd2);
    SelectCopyImm(parmRegOpnd2, countOpnd, PTY_i64);

    MIRSymbol *funcSym = GlobalTables::GetGsymTable().CreateSymbol(kScopeGlobal);
    if ((skipSym != nullptr) && (skipOffset >= realMin) && (skipOffset <= realMax)) {
      /* call cleanupskip */
      uint32 stOffset = (skipOffset - realMin) / kOffsetAlign;
      AArch64ImmOperand &retLoc = CreateImmOperand(stOffset, k64BitSize, true);

      AArch64RegOperand &parmRegOpnd3 = GetOrCreatePhysicalRegisterOperand(R2, k64BitSize, GetRegTyFromPrimTy(PTY_a64));
      srcOpnds->PushOpnd(parmRegOpnd3);
      SelectCopyImm(parmRegOpnd3, retLoc, PTY_i64);

      std::string funcName;
      if (forEA) {
        funcName = "MCC_CleanupNonRetEscObj";
      } else {
        funcName = "MCC_CleanupLocalStackRefSkip_NaiveRCFast";
      }
      funcSym->SetNameStrIdx(funcName);
#ifdef CC_DEBUG_INFO
      LogInfo::MapleLogger() << "num " << real_ref_num << " skip loc " << stOffset << '\n';
#endif
    } else {
      /* call cleanup */
      CHECK_FATAL(!forEA, "must be");
      std::string funcName("MCC_CleanupLocalStackRef_NaiveRCFast");
      funcSym->SetNameStrIdx(funcName);
#ifdef CC_DEBUG_INFO
      LogInfo::MapleLogger() << "num " << real_ref_num << '\n';
#endif
    }

    funcSym->SetStorageClass(kScText);
    funcSym->SetSKind(kStFunc);
    Insn &callInsn = AppendCall(*funcSym, *srcOpnds, true);
    static_cast<AArch64cleancallInsn&>(callInsn).SetRefSkipIndex(skipIndex);
    GetCurBB()->SetHasCall();
    GetCurBB()->SetFrequency(frequency);
  }
  return true;
}

RegOperand &AArch64CGFunc::CreateVirtualRegisterOperand(regno_t vRegNO) {
  ASSERT((vRegOperandTable.find(vRegNO) == vRegOperandTable.end()) || IsVRegNOForPseudoRegister(vRegNO), "");
  uint8 bitSize = static_cast<uint8>((static_cast<uint32>(vRegTable[vRegNO].GetSize())) * kBitsPerByte);
  RegOperand *res = memPool->New<AArch64RegOperand>(vRegNO, bitSize, vRegTable.at(vRegNO).GetType());
  vRegOperandTable[vRegNO] = res;
  return *res;
}

RegOperand &AArch64CGFunc::GetOrCreateVirtualRegisterOperand(regno_t vRegNO) {
  auto it = vRegOperandTable.find(vRegNO);
  return (it != vRegOperandTable.end()) ? *(it->second) : CreateVirtualRegisterOperand(vRegNO);
}

/*
 * Traverse all call insn to determine return type of it
 * If the following insn is mov/str/blr and use R0/V0, it means the call insn have reture value
 */
void AArch64CGFunc::DetermineReturnTypeofCall() {
  FOR_ALL_BB(bb, this) {
    if (bb->IsUnreachable() || !bb->HasCall()) {
      continue;
    }
    FOR_BB_INSNS(insn, bb) {
      if (!insn->IsCall()) {
        continue;
      }
      Insn *nextInsn = insn->GetNextMachineInsn();
      if (nextInsn == nullptr) {
        continue;
      }
      if ((nextInsn->IsMove() && nextInsn->GetOperand(kInsnSecondOpnd).IsRegister()) ||
          nextInsn->IsStore() ||
          (nextInsn->IsCall() && nextInsn->GetOperand(kInsnFirstOpnd).IsRegister())) {
        auto *srcOpnd = static_cast<RegOperand*>(nextInsn->GetOpnd(kInsnFirstOpnd));
        CHECK_FATAL(srcOpnd != nullptr, "nullptr");
        if (!srcOpnd->IsPhysicalRegister()) {
          continue;
        }
        if (srcOpnd->GetRegisterNumber() == R0) {
          insn->SetRetType(Insn::kRegInt);
          continue;
        }
        if (srcOpnd->GetRegisterNumber() == V0) {
          insn->SetRetType(Insn::kRegFloat);
        }
      }
    }
  }
}

void AArch64CGFunc::HandleRCCall(bool begin, const MIRSymbol *retRef) {
  if (!GetCG()->GenLocalRC() && !begin) {
    /* handle local rc is disabled. */
    return;
  }

  AArch64MemLayout *memLayout = static_cast<AArch64MemLayout*>(this->GetMemlayout());
  int32 refNum = memLayout->GetSizeOfRefLocals() / kOffsetAlign;
  if (!refNum) {
    if (begin) {
      GenerateYieldpoint(*GetCurBB());
      yieldPointInsn = GetCurBB()->GetLastInsn();
    }
    return;
  }

  /* no MCC_CleanupLocalStackRefSkip when ret_ref is the only ref symbol */
  if ((refNum == 1) && (retRef != nullptr)) {
    if (begin) {
      GenerateYieldpoint(*GetCurBB());
      yieldPointInsn = GetCurBB()->GetLastInsn();
    }
    return;
  }
  CHECK_FATAL(refNum < 0xFFFF, "not enough room for size.");
  int32 refLocBase = memLayout->GetRefLocBaseLoc();
  CHECK_FATAL((refLocBase >= 0) && (refLocBase < 0xFFFF), "not enough room for offset.");
  int32 formalRef = 0;
  /* avoid store zero to formal localrefvars. */
  if (begin) {
    for (uint32 i = 0; i < GetFunction().GetFormalCount(); ++i) {
      if (GetFunction().GetNthParamAttr(i).GetAttr(ATTR_localrefvar)) {
        refNum--;
        formalRef++;
      }
    }
  }
  /*
   * if the number of local refvar is less than 12, use stp or str to init local refvar
   * else call function MCC_InitializeLocalStackRef to init.
   */
  if (begin && (refNum <= kRefNum12) && ((refLocBase + kIntregBytelen * (refNum - 1)) < kStpLdpImm64UpperBound)) {
    int32 pairNum = refNum / kDivide2;
    int32 singleNum = refNum % kDivide2;
    const int32 pairRefBytes = 16; /* the size of each pair of ref is 16 bytes */
    int32 ind = 0;
    while (ind < pairNum) {
      int32 offset = memLayout->GetRefLocBaseLoc() + kIntregBytelen * formalRef + pairRefBytes * ind;
      Operand &zeroOp = GetZeroOpnd(k64BitSize);
      Operand &stackLoc = CreateStkTopOpnd(offset, kSizeOfPtr * kBitsPerByte);
      Insn &setInc = GetCG()->BuildInstruction<AArch64Insn>(MOP_xstp, zeroOp, zeroOp, stackLoc);
      GetCurBB()->AppendInsn(setInc);
      ind++;
    }
    if (singleNum > 0) {
      int32 offset = memLayout->GetRefLocBaseLoc() + kIntregBytelen * formalRef + kIntregBytelen * (refNum - 1);
      Operand &zeroOp = GetZeroOpnd(k64BitSize);
      Operand &stackLoc = CreateStkTopOpnd(offset, kSizeOfPtr * kBitsPerByte);
      Insn &setInc = GetCG()->BuildInstruction<AArch64Insn>(MOP_xstr, zeroOp, stackLoc);
      GetCurBB()->AppendInsn(setInc);
    }
    /* Insert Yield Point just after localrefvar are initialized. */
    GenerateYieldpoint(*GetCurBB());
    yieldPointInsn = GetCurBB()->GetLastInsn();
    return;
  }

  /* refNum is 1 and refvar is not returned, this refvar need to call MCC_DecRef_NaiveRCFast. */
  if ((refNum == 1) && !begin && (retRef == nullptr)) {
    RegOperand &phyOpnd = GetOrCreatePhysicalRegisterOperand(R0, k64BitSize, GetRegTyFromPrimTy(PTY_a64));
    Operand &stackLoc = CreateStkTopOpnd(memLayout->GetRefLocBaseLoc(), kSizeOfPtr * kBitsPerByte);
    Insn &ldrInsn = GetCG()->BuildInstruction<AArch64Insn>(PickLdInsn(k64BitSize, PTY_a64), phyOpnd, stackLoc);
    GetCurBB()->AppendInsn(ldrInsn);

    AArch64ListOperand *srcOpnds = memPool->New<AArch64ListOperand>(*GetFuncScopeAllocator());
    srcOpnds->PushOpnd(phyOpnd);
    MIRSymbol *callSym = GlobalTables::GetGsymTable().CreateSymbol(kScopeGlobal);
    std::string funcName("MCC_DecRef_NaiveRCFast");
    callSym->SetNameStrIdx(funcName);
    callSym->SetStorageClass(kScText);
    callSym->SetSKind(kStFunc);

    AppendCall(*callSym, *srcOpnds);
    GetCurBB()->SetHasCall();
    if (frequency != 0) {
      GetCurBB()->SetFrequency(frequency);
    }
    return;
  }

  /* refNum is 2 and one of refvar is returned, only another one is needed to call MCC_DecRef_NaiveRCFast. */
  if ((refNum == 2) && !begin && retRef != nullptr) {
    AArch64SymbolAlloc *symLoc =
        static_cast<AArch64SymbolAlloc*>(memLayout->GetSymAllocInfo(retRef->GetStIndex()));
    int32 stOffset = symLoc->GetOffset() / kOffsetAlign;
    RegOperand &phyOpnd = GetOrCreatePhysicalRegisterOperand(R0, k64BitSize, GetRegTyFromPrimTy(PTY_a64));
    Operand *stackLoc = nullptr;
    if (stOffset == 0) {
      /* just have to Dec the next one. */
      stackLoc = &CreateStkTopOpnd(memLayout->GetRefLocBaseLoc() + kIntregBytelen, kSizeOfPtr * kBitsPerByte);
    } else {
      /* just have to Dec the current one. */
      stackLoc = &CreateStkTopOpnd(memLayout->GetRefLocBaseLoc(), kSizeOfPtr * kBitsPerByte);
    }
    Insn &ldrInsn = GetCG()->BuildInstruction<AArch64Insn>(PickLdInsn(k64BitSize, PTY_a64), phyOpnd, *stackLoc);
    GetCurBB()->AppendInsn(ldrInsn);

    AArch64ListOperand *srcOpnds = memPool->New<AArch64ListOperand>(*GetFuncScopeAllocator());
    srcOpnds->PushOpnd(phyOpnd);
    MIRSymbol *callSym = GlobalTables::GetGsymTable().CreateSymbol(kScopeGlobal);
    std::string funcName("MCC_DecRef_NaiveRCFast");
    callSym->SetNameStrIdx(funcName);
    callSym->SetStorageClass(kScText);
    callSym->SetSKind(kStFunc);
    Insn &callInsn = AppendCall(*callSym, *srcOpnds, true);
    static_cast<AArch64cleancallInsn&>(callInsn).SetRefSkipIndex(stOffset);
    GetCurBB()->SetHasCall();
    if (frequency != 0) {
      GetCurBB()->SetFrequency(frequency);
    }
    return;
  }

  bool needSkip = false;
  AArch64ListOperand *srcOpnds = memPool->New<AArch64ListOperand>(*GetFuncScopeAllocator());

  AArch64ImmOperand *beginOpnd =
      &CreateImmOperand(memLayout->GetRefLocBaseLoc() + kIntregBytelen * formalRef, k64BitSize, true);
  AArch64ImmOperand *countOpnd = &CreateImmOperand(refNum, k64BitSize, true);
  int32 refSkipIndex = -1;
  if (!begin && retRef != nullptr) {
    AArch64SymbolAlloc *symLoc =
        static_cast<AArch64SymbolAlloc*>(memLayout->GetSymAllocInfo(retRef->GetStIndex()));
    int32 stOffset = symLoc->GetOffset() / kOffsetAlign;
    refSkipIndex = stOffset;
    if (stOffset == 0) {
      /* ret_ref at begin. */
      beginOpnd = &CreateImmOperand(memLayout->GetRefLocBaseLoc() + kIntregBytelen, k64BitSize, true);
      countOpnd = &CreateImmOperand(refNum - 1, k64BitSize, true);
    } else if (stOffset == (refNum - 1)) {
      /* ret_ref at end. */
      countOpnd = &CreateImmOperand(refNum - 1, k64BitSize, true);
    } else {
      needSkip = true;
    }
  }

  regno_t vRegNO0 = NewVReg(GetRegTyFromPrimTy(PTY_a64), GetPrimTypeSize(PTY_a64));
  RegOperand &vReg0 = CreateVirtualRegisterOperand(vRegNO0);
  RegOperand &fpOpnd = GetOrCreateStackBaseRegOperand();
  SelectAdd(vReg0, fpOpnd, *beginOpnd, PTY_i64);

  AArch64RegOperand &parmRegOpnd1 = GetOrCreatePhysicalRegisterOperand(R0, k64BitSize, GetRegTyFromPrimTy(PTY_a64));
  srcOpnds->PushOpnd(parmRegOpnd1);
  SelectCopy(parmRegOpnd1, PTY_a64, vReg0, PTY_a64);

  regno_t vRegNO1 = NewVReg(GetRegTyFromPrimTy(PTY_a64), GetPrimTypeSize(PTY_a64));
  RegOperand &vReg1 = CreateVirtualRegisterOperand(vRegNO1);
  SelectCopyImm(vReg1, *countOpnd, PTY_i64);

  AArch64RegOperand &parmRegOpnd2 = GetOrCreatePhysicalRegisterOperand(R1, k64BitSize, GetRegTyFromPrimTy(PTY_a64));
  srcOpnds->PushOpnd(parmRegOpnd2);
  SelectCopy(parmRegOpnd2, PTY_a64, vReg1, PTY_a64);

  MIRSymbol *sym = GlobalTables::GetGsymTable().CreateSymbol(kScopeGlobal);
  if (begin) {
    std::string funcName("MCC_InitializeLocalStackRef");
    sym->SetNameStrIdx(funcName);
    CHECK_FATAL(countOpnd->GetValue() > 0, "refCount should be greater than 0.");
    refCount = static_cast<uint32>(countOpnd->GetValue());
    beginOffset = beginOpnd->GetValue();
  } else if (!needSkip) {
    std::string funcName("MCC_CleanupLocalStackRef_NaiveRCFast");
    sym->SetNameStrIdx(funcName);
  } else {
    CHECK_NULL_FATAL(retRef);
    if (retRef->GetStIndex() >= memLayout->GetSymAllocTable().size()) {
      CHECK_FATAL(false, "index out of range in AArch64CGFunc::HandleRCCall");
    }
    AArch64SymbolAlloc *symLoc = static_cast<AArch64SymbolAlloc*>(memLayout->GetSymAllocInfo(retRef->GetStIndex()));
    int32 stOffset = symLoc->GetOffset() / kOffsetAlign;
    AArch64ImmOperand &retLoc = CreateImmOperand(stOffset, k64BitSize, true);

    regno_t vRegNO2 = NewVReg(GetRegTyFromPrimTy(PTY_a64), GetPrimTypeSize(PTY_a64));
    RegOperand &vReg2 = CreateVirtualRegisterOperand(vRegNO2);
    SelectCopyImm(vReg2, retLoc, PTY_i64);

    AArch64RegOperand &parmRegOpnd3 = GetOrCreatePhysicalRegisterOperand(R2, k64BitSize, GetRegTyFromPrimTy(PTY_a64));
    srcOpnds->PushOpnd(parmRegOpnd3);
    SelectCopy(parmRegOpnd3, PTY_a64, vReg2, PTY_a64);

    std::string funcName("MCC_CleanupLocalStackRefSkip_NaiveRCFast");
    sym->SetNameStrIdx(funcName);
  }
  sym->SetStorageClass(kScText);
  sym->SetSKind(kStFunc);

  Insn &callInsn = AppendCall(*sym, *srcOpnds, true);
  static_cast<AArch64cleancallInsn&>(callInsn).SetRefSkipIndex(refSkipIndex);
  if (frequency != 0) {
    GetCurBB()->SetFrequency(frequency);
  }
  GetCurBB()->SetHasCall();
  if (begin) {
    /* Insert Yield Point just after localrefvar are initialized. */
    GenerateYieldpoint(*GetCurBB());
    yieldPointInsn = GetCurBB()->GetLastInsn();
  }
}

/*
   SelectParmList generates an instrunction for each of the parameters
   to load the parameter value into the corresponding register.
   We return a list of registers to the call instruction because
   they may be needed in the register allocation phase.
 */
void AArch64CGFunc::SelectParmList(StmtNode &naryNode, AArch64ListOperand &srcOpnds, bool isCallNative) {
  ParmLocator parmLocator(GetBecommon());
  PLocInfo ploc;
  size_t i = 0;
  if ((naryNode.GetOpCode() == OP_icall) || isCallNative) {
    i++;
  }

  for (; i < naryNode.NumOpnds(); ++i) {
    MIRType *ty = nullptr;
    BaseNode *argExpr = naryNode.Opnd(i);
    PrimType primType = argExpr->GetPrimType();
    ASSERT(primType != PTY_void, "primType should not be void");
    /* use alloca  */
    CHECK_FATAL(primType != PTY_agg, "NYI");
    ty = GlobalTables::GetTypeTable().GetTypeTable()[static_cast<uint32>(primType)];
    RegOperand *expRegOpnd = nullptr;
    Operand *opnd = HandleExpr(naryNode, *argExpr);
    if (!opnd->IsRegister()) {
      opnd = &LoadIntoRegister(*opnd, primType);
    }
    expRegOpnd = static_cast<RegOperand*>(opnd);

    parmLocator.LocateNextParm(*ty, ploc);
    if (ploc.reg0 != kRinvalid) {  /* load to the register. */
      CHECK_FATAL(expRegOpnd != nullptr, "null ptr check");
      AArch64RegOperand &parmRegOpnd = GetOrCreatePhysicalRegisterOperand(ploc.reg0, expRegOpnd->GetSize(),
                                                                          GetRegTyFromPrimTy(primType));
      SelectCopy(parmRegOpnd, primType, *expRegOpnd, primType);
      srcOpnds.PushOpnd(parmRegOpnd);
    } else {  /* store to the memory segment for stack-passsed arguments. */
      Operand &actMemOpnd = CreateMemOpnd(RSP, ploc.memOffset, GetPrimTypeBitSize(primType));
      GetCurBB()->AppendInsn(
          GetCG()->BuildInstruction<AArch64Insn>(PickStInsn(GetPrimTypeBitSize(primType), primType), *expRegOpnd,
                                                 actMemOpnd));
      }
    ASSERT(ploc.reg1 == 0, "SelectCall NYI");
  }
}

/*
 * for MCC_DecRefResetPair(addrof ptr %Reg17_R5592, addrof ptr %Reg16_R6202) or
 * MCC_ClearLocalStackRef(addrof ptr %Reg17_R5592), the parameter (addrof ptr xxx) is converted to asm as follow:
 * add vreg, x29, #imm
 * mov R0/R1, vreg
 * this function is used to prepare parameters, the generated vreg is returned, and #imm is saved in offsetValue.
 */
Operand *AArch64CGFunc::SelectClearStackCallParam(const AddrofNode &expr, int64 &offsetValue) {
  MIRSymbol *symbol = GetMirModule().CurFunction()->GetLocalOrGlobalSymbol(expr.GetStIdx());
  PrimType ptype = expr.GetPrimType();
  regno_t vRegNO = NewVReg(kRegTyInt, GetPrimTypeSize(ptype));
  Operand &result = CreateVirtualRegisterOperand(vRegNO);
  CHECK_FATAL(expr.GetFieldID() == 0, "the fieldID of parameter in clear stack reference call must be 0");
  if (!GetCG()->IsQuiet()) {
    maple::logInfo.MapleLogger(kLlErr) <<
        "Warning: we expect AddrOf with StImmOperand is not used for local variables";
  }
  auto *symLoc = static_cast<AArch64SymbolAlloc*>(GetMemlayout()->GetSymAllocInfo(symbol->GetStIndex()));
  AArch64ImmOperand *offset = nullptr;
  if (symLoc->GetMemSegment()->GetMemSegmentKind() == kMsArgsStkPassed) {
    offset = &CreateImmOperand(GetBaseOffset(*symLoc), k64BitSize, false, kUnAdjustVary);
  } else if (symLoc->GetMemSegment()->GetMemSegmentKind() == kMsRefLocals) {
    auto it = immOpndsRequiringOffsetAdjustmentForRefloc.find(symLoc);
    if (it != immOpndsRequiringOffsetAdjustmentForRefloc.end()) {
      offset = (*it).second;
    } else {
      offset = &CreateImmOperand(GetBaseOffset(*symLoc), k64BitSize, false);
      immOpndsRequiringOffsetAdjustmentForRefloc[symLoc] = offset;
    }
  } else {
    CHECK_FATAL(false, "the symLoc of parameter in clear stack reference call is unreasonable");
  }
  offsetValue = offset->GetValue();
  SelectAdd(result, *GetBaseReg(*symLoc), *offset, PTY_u64);
  if (GetCG()->GenerateVerboseAsm()) {
    /* Add a comment */
    Insn *insn = GetCurBB()->GetLastInsn();
    std::string comm = "local/formal var: ";
    comm.append(symbol->GetName());
    insn->SetComment(comm);
  }
  return &result;
}

/* select paramters for MCC_DecRefResetPair and MCC_ClearLocalStackRef function */
void AArch64CGFunc::SelectClearStackCallParmList(const StmtNode &naryNode, AArch64ListOperand &srcOpnds,
                                                 std::vector<int64> &stackPostion) {
  ParmLocator parmLocator(GetBecommon());
  PLocInfo ploc;
  for (size_t i = 0; i < naryNode.NumOpnds(); ++i) {
    MIRType *ty = nullptr;
    BaseNode *argExpr = naryNode.Opnd(i);
    PrimType primType = argExpr->GetPrimType();
    ASSERT(primType != PTY_void, "primType check");
    /* use alloc */
    CHECK_FATAL(primType != PTY_agg, "the type of argument is unreasonable");
    ty = GlobalTables::GetTypeTable().GetTypeTable()[static_cast<uint32>(primType)];
    CHECK_FATAL(argExpr->GetOpCode() == OP_addrof, "the argument of clear stack call is unreasonable");
    auto *expr = static_cast<AddrofNode*>(argExpr);
    int64 offsetValue = 0;
    Operand *opnd = SelectClearStackCallParam(*expr, offsetValue);
    stackPostion.push_back(offsetValue);
    auto *expRegOpnd = static_cast<RegOperand*>(opnd);
    parmLocator.LocateNextParm(*ty, ploc);
    CHECK_FATAL(ploc.reg0 != 0, "the parameter of ClearStackCall must be passed by register");
    CHECK_FATAL(expRegOpnd != nullptr, "null ptr check");
    AArch64RegOperand &parmRegOpnd = GetOrCreatePhysicalRegisterOperand(ploc.reg0, expRegOpnd->GetSize(),
                                                                        GetRegTyFromPrimTy(primType));
    SelectCopy(parmRegOpnd, primType, *expRegOpnd, primType);
    srcOpnds.PushOpnd(parmRegOpnd);
    ASSERT(ploc.reg1 == 0, "SelectCall NYI");
  }
}

/*
 * intrinsify Unsafe.getAndAddInt and Unsafe.getAndAddLong
 * generate an intrinsic instruction instead of a function call
 * intrinsic_get_add_int w0, xt, ws, ws, x1, x2, w3, label
 */
void AArch64CGFunc::IntrinsifyGetAndAddInt(AArch64ListOperand &srcOpnds, PrimType pty) {
  MapleList<RegOperand*> &opnds = srcOpnds.GetOperands();
  /* Unsafe.getAndAddInt has more than 4 parameters */
  ASSERT(opnds.size() >= 4, "ensure the operands number");
  auto iter = opnds.begin();
  RegOperand *objOpnd = *(++iter);
  RegOperand *offOpnd = *(++iter);
  RegOperand *deltaOpnd = *(++iter);
  auto &retVal = static_cast<RegOperand&>(GetTargetRetOperand(pty));
  LabelIdx labIdx = CreateLabel();
  LabelOperand &targetOpnd = GetOrCreateLabelOperand(labIdx);
  RegOperand &tempOpnd0 = CreateRegisterOperandOfType(PTY_i64);
  RegOperand &tempOpnd1 = CreateRegisterOperandOfType(pty);
  RegOperand &tempOpnd2 = CreateRegisterOperandOfType(PTY_i32);
  MOperator mOp = (pty == PTY_i64) ? MOP_get_and_addL : MOP_get_and_addI;
  std::vector<Operand*> intrnOpnds;
  intrnOpnds.push_back(&retVal);
  intrnOpnds.push_back(&tempOpnd0);
  intrnOpnds.push_back(&tempOpnd1);
  intrnOpnds.push_back(&tempOpnd2);
  intrnOpnds.push_back(objOpnd);
  intrnOpnds.push_back(offOpnd);
  intrnOpnds.push_back(deltaOpnd);
  intrnOpnds.push_back(&targetOpnd);
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, intrnOpnds));
}

/*
 * intrinsify Unsafe.getAndSetInt and Unsafe.getAndSetLong
 * generate an intrinsic instruction instead of a function call
 */
void AArch64CGFunc::IntrinsifyGetAndSetInt(AArch64ListOperand &srcOpnds, PrimType pty) {
  MapleList<RegOperand*> &opnds = srcOpnds.GetOperands();
  /* Unsafe.getAndSetInt has 4 parameters */
  ASSERT(opnds.size() == 4, "ensure the operands number");
  auto iter = opnds.begin();
  RegOperand *objOpnd = *(++iter);
  RegOperand *offOpnd = *(++iter);
  RegOperand *newValueOpnd = *(++iter);
  auto &retVal = static_cast<RegOperand&>(GetTargetRetOperand(pty));
  LabelIdx labIdx = CreateLabel();
  LabelOperand &targetOpnd = GetOrCreateLabelOperand(labIdx);
  RegOperand &tempOpnd0 = CreateRegisterOperandOfType(PTY_i64);
  RegOperand &tempOpnd1 = CreateRegisterOperandOfType(PTY_i32);

  MOperator mOp = (pty == PTY_i64) ? MOP_get_and_setL : MOP_get_and_setI;
  std::vector<Operand*> intrnOpnds;
  intrnOpnds.push_back(&retVal);
  intrnOpnds.push_back(&tempOpnd0);
  intrnOpnds.push_back(&tempOpnd1);
  intrnOpnds.push_back(objOpnd);
  intrnOpnds.push_back(offOpnd);
  intrnOpnds.push_back(newValueOpnd);
  intrnOpnds.push_back(&targetOpnd);
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, intrnOpnds));
}

/*
 * intrinsify Unsafe.compareAndSwapInt and Unsafe.compareAndSwapLong
 * generate an intrinsic instruction instead of a function call
 */
void AArch64CGFunc::IntrinsifyCompareAndSwapInt(AArch64ListOperand &srcOpnds, PrimType pty) {
  MapleList<RegOperand*> &opnds = srcOpnds.GetOperands();
  /* Unsafe.compareAndSwapInt has more than 5 parameters */
  ASSERT(opnds.size() >= 5, "ensure the operands number");
  auto iter = opnds.begin();
  RegOperand *objOpnd = *(++iter);
  RegOperand *offOpnd = *(++iter);
  RegOperand *expectedValueOpnd = *(++iter);
  RegOperand *newValueOpnd = *(++iter);
  auto &retVal = static_cast<RegOperand&>(GetTargetRetOperand(PTY_i64));
  RegOperand &tempOpnd0 = CreateRegisterOperandOfType(PTY_i64);
  RegOperand &tempOpnd1 = CreateRegisterOperandOfType(pty);
  LabelIdx labIdx1 = CreateLabel();
  LabelOperand &label1Opnd = GetOrCreateLabelOperand(labIdx1);
  LabelIdx labIdx2 = CreateLabel();
  LabelOperand &label2Opnd = GetOrCreateLabelOperand(labIdx2);
  MOperator mOp = (pty == PTY_i32) ? MOP_compare_and_swapI : MOP_compare_and_swapL;
  std::vector<Operand*> intrnOpnds;
  intrnOpnds.push_back(&retVal);
  intrnOpnds.push_back(&tempOpnd0);
  intrnOpnds.push_back(&tempOpnd1);
  intrnOpnds.push_back(objOpnd);
  intrnOpnds.push_back(offOpnd);
  intrnOpnds.push_back(expectedValueOpnd);
  intrnOpnds.push_back(newValueOpnd);
  intrnOpnds.push_back(&label1Opnd);
  intrnOpnds.push_back(&label2Opnd);
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, intrnOpnds));
}

/*
 * the lowest bit of count field is used to indicate whether or not the string is compressed
 * if the string is not compressed, jump to jumpLabIdx
 */
RegOperand *AArch64CGFunc::CheckStringIsCompressed(BB &bb, RegOperand &str, int32 countOffset, PrimType countPty,
                                                   LabelIdx jumpLabIdx) {
  MemOperand &memOpnd = CreateMemOpnd(str, countOffset, str.GetSize());
  uint32 bitSize = GetPrimTypeBitSize(countPty);
  MOperator loadOp = PickLdInsn(bitSize, countPty);
  RegOperand &countOpnd = CreateRegisterOperandOfType(countPty);
  bb.AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(loadOp, countOpnd, memOpnd));
  ImmOperand &immValueOne = CreateImmOperand(countPty, 1);
  RegOperand &countLowestBitOpnd = CreateRegisterOperandOfType(countPty);
  MOperator andOp = bitSize == k64BitSize ? MOP_xandrri13 : MOP_wandrri12;
  bb.AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(andOp, countLowestBitOpnd, countOpnd, immValueOne));
  AArch64RegOperand &wzr = AArch64RegOperand::GetZeroRegister(bitSize);
  MOperator cmpOp = (bitSize == k64BitSize) ? MOP_xcmprr : MOP_wcmprr;
  Operand &rflag = GetOrCreateRflag();
  bb.AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(cmpOp, rflag, wzr, countLowestBitOpnd));
  bb.AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_beq, rflag, GetOrCreateLabelOperand(jumpLabIdx)));
  bb.SetKind(BB::kBBIf);
  return &countOpnd;
}

/*
 * count field stores the length shifted one bit to the left
 * if the length is less than eight, jump to jumpLabIdx
 */
RegOperand *AArch64CGFunc::CheckStringLengthLessThanEight(BB &bb, RegOperand &countOpnd, PrimType countPty,
                                                          LabelIdx jumpLabIdx) {
  RegOperand &lengthOpnd = CreateRegisterOperandOfType(countPty);
  uint32 bitSize = GetPrimTypeBitSize(countPty);
  MOperator lsrOp = (bitSize == k64BitSize) ? MOP_xlsrrri6 : MOP_wlsrrri5;
  ImmOperand &immValueOne = CreateImmOperand(countPty, 1);
  bb.AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(lsrOp, lengthOpnd, countOpnd, immValueOne));
  constexpr int kConstIntEight = 8;
  ImmOperand &immValueEight = CreateImmOperand(countPty, kConstIntEight);
  MOperator cmpImmOp = (bitSize == k64BitSize) ? MOP_xcmpri : MOP_wcmpri;
  Operand &rflag = GetOrCreateRflag();
  bb.AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(cmpImmOp, rflag, lengthOpnd, immValueEight));
  bb.AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_blt, rflag, GetOrCreateLabelOperand(jumpLabIdx)));
  bb.SetKind(BB::kBBIf);
  return &lengthOpnd;
}

void AArch64CGFunc::GenerateIntrnInsnForStrIndexOf(BB &bb, RegOperand &srcString, RegOperand &patternString,
                                                   RegOperand &srcCountOpnd, RegOperand &patternLengthOpnd,
                                                   PrimType countPty, LabelIdx jumpLabIdx) {
  RegOperand &srcLengthOpnd = CreateRegisterOperandOfType(countPty);
  ImmOperand &immValueOne = CreateImmOperand(countPty, 1);
  uint32 bitSize = GetPrimTypeBitSize(countPty);
  MOperator lsrOp = (bitSize == k64BitSize) ? MOP_xlsrrri6 : MOP_wlsrrri5;
  bb.AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(lsrOp, srcLengthOpnd, srcCountOpnd, immValueOne));
#ifdef USE_32BIT_REF
  const int64 stringBaseObjSize = 16;  // shadow(4)+monitor(4)+count(4)+hash(4)
#else
  const int64 stringBaseObjSize = 20;  // shadow(8)+monitor(4)+count(4)+hash(4)
#endif  // USE_32BIT_REF
  PrimType pty = (srcString.GetSize() == k64BitSize) ? PTY_i64 : PTY_i32;
  ImmOperand &immStringBaseOffset = CreateImmOperand(pty, stringBaseObjSize);
  MOperator addOp = (pty == PTY_i64) ? MOP_xaddrri12 : MOP_waddrri12;
  RegOperand &srcStringBaseOpnd = CreateRegisterOperandOfType(pty);
  bb.AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(addOp, srcStringBaseOpnd, srcString, immStringBaseOffset));
  RegOperand &patternStringBaseOpnd = CreateRegisterOperandOfType(pty);
  bb.AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(addOp, patternStringBaseOpnd, patternString,
                                                       immStringBaseOffset));
  auto &retVal = static_cast<RegOperand&>(GetTargetRetOperand(PTY_i32));
  std::vector<Operand*> intrnOpnds;
  intrnOpnds.push_back(&retVal);
  intrnOpnds.push_back(&srcStringBaseOpnd);
  intrnOpnds.push_back(&srcLengthOpnd);
  intrnOpnds.push_back(&patternStringBaseOpnd);
  intrnOpnds.push_back(&patternLengthOpnd);
  const uint32 tmpRegOperandNum = 6;
  for (uint32 i = 0; i < tmpRegOperandNum - 1; ++i) {
    RegOperand &tmpOpnd = CreateRegisterOperandOfType(PTY_i64);
    intrnOpnds.push_back(&tmpOpnd);
  }
  intrnOpnds.push_back(&CreateRegisterOperandOfType(PTY_i32));
  const uint32 labelNum = 7;
  for (uint32 i = 0; i < labelNum; ++i) {
    LabelIdx labIdx = CreateLabel();
    LabelOperand &labelOpnd = GetOrCreateLabelOperand(labIdx);
    intrnOpnds.push_back(&labelOpnd);
  }
  bb.AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_string_indexof, intrnOpnds));
  bb.AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_xuncond, GetOrCreateLabelOperand(jumpLabIdx)));
  bb.SetKind(BB::kBBGoto);
}

/*
 * intrinsify String.indexOf
 * generate an intrinsic instruction instead of a function call if both the source string and the specified substring
 * are compressed and the length of the substring is not less than 8, i.e.
 * bl  String.indexOf, srcString, patternString ===>>
 *
 * ldr srcCountOpnd, [srcString, offset]
 * and srcCountLowestBitOpnd, srcCountOpnd, #1
 * cmp wzr, srcCountLowestBitOpnd
 * beq Label.call
 * ldr patternCountOpnd, [patternString, offset]
 * and patternCountLowestBitOpnd, patternCountOpnd, #1
 * cmp wzr, patternCountLowestBitOpnd
 * beq Label.call
 * lsr patternLengthOpnd, patternCountOpnd, #1
 * cmp patternLengthOpnd, #8
 * blt Label.call
 * lsr srcLengthOpnd, srcCountOpnd, #1
 * add srcStringBaseOpnd, srcString, immStringBaseOffset
 * add patternStringBaseOpnd, patternString, immStringBaseOffset
 * intrinsic_string_indexof retVal, srcStringBaseOpnd, srcLengthOpnd, patternStringBaseOpnd, patternLengthOpnd,
 *                          tmpOpnd1, tmpOpnd2, tmpOpnd3, tmpOpnd4, tmpOpnd5, tmpOpnd6,
 *                          label1, label2, label3, lable3, label4, label5, label6, label7
 * b   Label.joint
 * Label.call:
 * bl  String.indexOf, srcString, patternString
 * Label.joint:
 */
void AArch64CGFunc::IntrinsifyStringIndexOf(AArch64ListOperand &srcOpnds, const MIRSymbol &funcSym) {
  MapleList<RegOperand*> &opnds = srcOpnds.GetOperands();
  /* String.indexOf opnd size must be more than 2 */
  ASSERT(opnds.size() >= 2, "ensure the operands number");
  auto iter = opnds.begin();
  RegOperand *srcString = *iter;
  RegOperand *patternString = *(++iter);
  GStrIdx gStrIdx = GlobalTables::GetStrTable().GetStrIdxFromName(NameMangler::kJavaLangStringStr);
  MIRType *type =
      GlobalTables::GetTypeTable().GetTypeFromTyIdx(GlobalTables::GetTypeNameTable().GetTyIdxFromGStrIdx(gStrIdx));
  auto stringType = static_cast<MIRStructType*>(type);
  CHECK_FATAL(stringType != nullptr, "Ljava_2Flang_2FString_3B type can not be null");
  FieldID fieldID = GetMirModule().GetMIRBuilder()->GetStructFieldIDFromFieldNameParentFirst(stringType, "count");
  MIRType *fieldType = stringType->GetFieldType(fieldID);
  PrimType countPty = fieldType->GetPrimType();
  int32 offset = GetBecommon().GetFieldOffset(*stringType, fieldID).first;
  LabelIdx callBBLabIdx = CreateLabel();
  RegOperand *srcCountOpnd = CheckStringIsCompressed(*GetCurBB(), *srcString, offset, countPty, callBBLabIdx);

  BB *srcCompressedBB = CreateNewBB();
  GetCurBB()->AppendBB(*srcCompressedBB);
  RegOperand *patternCountOpnd = CheckStringIsCompressed(*srcCompressedBB, *patternString, offset, countPty,
      callBBLabIdx);

  BB *patternCompressedBB = CreateNewBB();
  RegOperand *patternLengthOpnd = CheckStringLengthLessThanEight(*patternCompressedBB, *patternCountOpnd, countPty,
      callBBLabIdx);

  BB *intrinsicBB = CreateNewBB();
  LabelIdx jointLabIdx = CreateLabel();
  GenerateIntrnInsnForStrIndexOf(*intrinsicBB, *srcString, *patternString, *srcCountOpnd, *patternLengthOpnd,
      countPty, jointLabIdx);

  BB *callBB = CreateNewBB();
  callBB->AddLabel(callBBLabIdx);
  SetLab2BBMap(callBBLabIdx, *callBB);
  SetCurBB(*callBB);
  Insn &callInsn = AppendCall(funcSym, srcOpnds);
  MIRType *retType = funcSym.GetFunction()->GetReturnType();
  if (retType != nullptr) {
    callInsn.SetRetSize(retType->GetSize());
  }
  GetFunction().SetHasCall();

  BB *jointBB = CreateNewBB();
  jointBB->AddLabel(jointLabIdx);
  SetLab2BBMap(jointLabIdx, *jointBB);
  srcCompressedBB->AppendBB(*patternCompressedBB);
  patternCompressedBB->AppendBB(*intrinsicBB);
  intrinsicBB->AppendBB(*callBB);
  callBB->AppendBB(*jointBB);
  SetCurBB(*jointBB);
}
void AArch64CGFunc::SelectCall(CallNode &callNode) {
  MIRFunction *fn = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(callNode.GetPUIdx());
  MIRSymbol *fsym = GetFunction().GetLocalOrGlobalSymbol(fn->GetStIdx(), false);
  MIRType *retType = fn->GetReturnType();

  if (GetCG()->GenerateVerboseAsm()) {
    const std::string &comment = fsym->GetName();
    GetCurBB()->AppendInsn(CreateCommentInsn(comment));
  }

  AArch64ListOperand *srcOpnds = memPool->New<AArch64ListOperand>(*GetFuncScopeAllocator());
  bool callNative = false;
  if ((fsym->GetName() == "MCC_CallFastNative") || (fsym->GetName() == "MCC_CallFastNativeExt") ||
      (fsym->GetName() == "MCC_CallSlowNative0") || (fsym->GetName() == "MCC_CallSlowNative1") ||
      (fsym->GetName() == "MCC_CallSlowNative2") || (fsym->GetName() == "MCC_CallSlowNative3") ||
      (fsym->GetName() == "MCC_CallSlowNative4") || (fsym->GetName() == "MCC_CallSlowNative5") ||
      (fsym->GetName() == "MCC_CallSlowNative6") || (fsym->GetName() == "MCC_CallSlowNative7") ||
      (fsym->GetName() == "MCC_CallSlowNative8") || (fsym->GetName() == "MCC_CallSlowNativeExt")) {
    callNative = true;
  }

  std::vector<int64> stackPosition;
  if ((fsym->GetName() == "MCC_DecRefResetPair") || (fsym->GetName() == "MCC_ClearLocalStackRef")) {
    SelectClearStackCallParmList(callNode, *srcOpnds, stackPosition);
  } else {
    SelectParmList(callNode, *srcOpnds, callNative);
  }
  if (callNative) {
    GetCurBB()->AppendInsn(CreateCommentInsn("call native func"));

    BaseNode *funcArgExpr = callNode.Opnd(0);
    PrimType ptype = funcArgExpr->GetPrimType();
    Operand *funcOpnd = HandleExpr(callNode, *funcArgExpr);
    AArch64RegOperand &livein = GetOrCreatePhysicalRegisterOperand(R9, kSizeOfPtr * kBitsPerByte,
                                                                   GetRegTyFromPrimTy(PTY_a64));
    SelectCopy(livein, ptype, *funcOpnd, ptype);

    AArch64RegOperand &extraOpnd = GetOrCreatePhysicalRegisterOperand(R9, kSizeOfPtr * kBitsPerByte, kRegTyInt);
    srcOpnds->PushOpnd(extraOpnd);
  }
  Insn &callInsn = AppendCall(*fsym, *srcOpnds);
  GetCurBB()->SetHasCall();
  if (retType != nullptr) {
    callInsn.SetRetSize(retType->GetSize());
  }

  GetFunction().SetHasCall();
  if ((fsym->GetName() == "MCC_ThrowException") || (fsym->GetName() == "MCC_RethrowException") ||
      (fsym->GetName() == "MCC_ThrowArithmeticException") ||
      (fsym->GetName() == "MCC_ThrowArrayIndexOutOfBoundsException") ||
      (fsym->GetName() == "MCC_ThrowNullPointerException") ||
      (fsym->GetName() == "MCC_ThrowStringIndexOutOfBoundsException") || (fsym->GetName() == "abort") ||
      (fsym->GetName() == "exit") || (fsym->GetName() == "MCC_Array_Boundary_Check")) {
    callInsn.SetIsThrow(true);
    GetCurBB()->SetKind(BB::kBBThrow);
  } else if ((fsym->GetName() == "MCC_DecRefResetPair") || (fsym->GetName() == "MCC_ClearLocalStackRef")) {
    for (size_t i = 0; i < stackPosition.size(); ++i) {
      callInsn.SetClearStackOffset(i, stackPosition[i]);
    }
  }
}

void AArch64CGFunc::SelectIcall(IcallNode &icallNode, Operand &srcOpnd) {
  AArch64ListOperand *srcOpnds = memPool->New<AArch64ListOperand>(*GetFuncScopeAllocator());
  SelectParmList(icallNode, *srcOpnds);

  Operand *fptrOpnd = &srcOpnd;
  if (fptrOpnd->GetKind() != Operand::kOpdRegister) {
    PrimType ty = icallNode.Opnd(0)->GetPrimType();
    fptrOpnd = &SelectCopy(srcOpnd, ty, ty);
  }
  ASSERT(fptrOpnd->IsRegister(), "SelectIcall: function pointer not RegOperand");
  RegOperand *regOpnd = static_cast<RegOperand*>(fptrOpnd);
  Insn &callInsn = GetCG()->BuildInstruction<AArch64Insn>(MOP_xblr, *regOpnd, *srcOpnds);

  MIRType *retType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(icallNode.GetRetTyIdx());
  if (retType != nullptr) {
    callInsn.SetRetSize(retType->GetSize());
  }

  GetCurBB()->AppendInsn(callInsn);
  GetCurBB()->SetHasCall();
  ASSERT(GetCurBB()->GetLastInsn()->IsCall(), "lastInsn should be a call");
  GetFunction().SetHasCall();
}

void AArch64CGFunc::HandleCatch() {
  if (Globals::GetInstance()->GetOptimLevel() >= 1) {
    regno_t regNO = uCatch.regNOCatch;
    RegOperand &vregOpnd = GetOrCreateVirtualRegisterOperand(regNO);
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_xmovrr, vregOpnd,
        GetOrCreatePhysicalRegisterOperand(R0, k64BitSize, kRegTyInt)));
  } else {
    GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(PickStInsn(uCatch.opndCatch->GetSize(), PTY_a64),
        GetOrCreatePhysicalRegisterOperand(R0, k64BitSize, kRegTyInt), *uCatch.opndCatch));
  }
}

void AArch64CGFunc::SelectMembar(StmtNode &membar) {
  switch (membar.GetOpCode()) {
    case OP_membaracquire:
      GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_dmb_ishld));
      break;
    case OP_membarrelease:
      GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_dmb_ish));
      break;
    case OP_membarstoreload:
      GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_dmb_ish));
      break;
    case OP_membarstorestore:
      GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_dmb_ishst));
      break;
    default:
      ASSERT(false, "NYI");
      break;
  }
}

void AArch64CGFunc::SelectComment(CommentNode &comment) {
  GetCurBB()->AppendInsn(CreateCommentInsn(comment.GetComment()));
}

void AArch64CGFunc::SelectReturn(Operand *opnd0) {
  ReturnMechanism retMech(*(GetFunction().GetReturnType()), GetBecommon());
  if (retMech.GetRegCount() > 0) {
    CHECK_FATAL(opnd0 != nullptr, "opnd0 must not be nullptr");
    if (opnd0->IsRegister()) {
      RegOperand *regOpnd = static_cast<RegOperand*>(opnd0);
      if (regOpnd->GetRegisterNumber() != retMech.GetReg0()) {
        AArch64RegOperand &retOpnd =
            GetOrCreatePhysicalRegisterOperand(retMech.GetReg0(), regOpnd->GetSize(),
                                               GetRegTyFromPrimTy(retMech.GetPrimTypeOfReg0()));
        SelectCopy(retOpnd, retMech.GetPrimTypeOfReg0(), *regOpnd, retMech.GetPrimTypeOfReg0());
      }
    } else if (opnd0->IsMemoryAccessOperand()) {
      AArch64MemOperand *memopnd = static_cast<AArch64MemOperand*>(opnd0);
      AArch64RegOperand &retOpnd = GetOrCreatePhysicalRegisterOperand(retMech.GetReg0(),
          GetPrimTypeBitSize(retMech.GetPrimTypeOfReg0()), GetRegTyFromPrimTy(retMech.GetPrimTypeOfReg0()));
      MOperator mOp = PickLdInsn(memopnd->GetSize(), retMech.GetPrimTypeOfReg0());
      GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, retOpnd, *memopnd));
    } else if (opnd0->IsConstImmediate()) {
      ImmOperand *immOpnd = static_cast<ImmOperand*>(opnd0);
      AArch64RegOperand &retOpnd = GetOrCreatePhysicalRegisterOperand(retMech.GetReg0(),
          GetPrimTypeBitSize(retMech.GetPrimTypeOfReg0()), GetRegTyFromPrimTy(retMech.GetPrimTypeOfReg0()));
      SelectCopy(retOpnd, retMech.GetPrimTypeOfReg0(), *immOpnd, retMech.GetPrimTypeOfReg0());
    } else {
      CHECK_FATAL(false, "nyi");
    }
  } else if (opnd0 != nullptr) {  /* pass in memory */
    CHECK_FATAL(false, "SelectReturn: return in memory NYI");
  }
  GetExitBBsVec().push_back(GetCurBB());
}

RegOperand &AArch64CGFunc::GetOrCreateSpecialRegisterOperand(PregIdx sregIdx, PrimType primType) {
  AArch64reg reg = R0;
  switch (sregIdx) {
    case kSregSp:
      reg = RSP;
      break;
    case kSregFp:
      reg = RFP;
      break;
    case kSregThrownval: { /* uses x0 == R0 */
      ASSERT(uCatch.regNOCatch > 0, "regNOCatch should greater than 0.");
      if (Globals::GetInstance()->GetOptimLevel() == 0) {
        RegOperand &regOpnd = GetOrCreateVirtualRegisterOperand(NewVReg(kRegTyInt, k8BitSize));
        GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(
            PickLdInsn(uCatch.opndCatch->GetSize(), PTY_a64), regOpnd, *uCatch.opndCatch));
        return regOpnd;
      } else {
        return GetOrCreateVirtualRegisterOperand(uCatch.regNOCatch);
      }
    }
    case kSregRetval0:
      if (!IsPrimitiveInteger(primType)) {
        reg = V0;
      }
      break;
    case kSregMethodhdl:
      if (methodHandleVreg == regno_t(-1)) {
        methodHandleVreg = NewVReg(kRegTyInt, k8BitSize);
      }
      return GetOrCreateVirtualRegisterOperand(methodHandleVreg);
    default:
      ASSERT(false, "Special pseudo registers NYI");
      break;
  }
  return GetOrCreatePhysicalRegisterOperand(reg, k64BitSize, kRegTyInt);
}

AArch64RegOperand &AArch64CGFunc::GetOrCreatePhysicalRegisterOperand(AArch64reg regNO, uint32 size,
                                                                     RegType kind, uint32 flag) {
  size = (size <= k32BitSize) ? k32BitSize : k64BitSize;

  auto it = phyRegOperandTable.find(AArch64RegOperand(regNO, size, kind, flag));
  if (it != phyRegOperandTable.end()) {
    return *(it->second);
  }

  AArch64RegOperand *o = memPool->New<AArch64RegOperand>(regNO, size, kind, flag);
  phyRegOperandTable[*o] = o;
  return *o;
}

const LabelOperand *AArch64CGFunc::GetLabelOperand(LabelIdx labIdx) const {
  const MapleMap<LabelIdx, LabelOperand*>::const_iterator it = hashLabelOpndTable.find(labIdx);
  if (it != hashLabelOpndTable.end()) {
    return it->second;
  }
  return nullptr;
}

LabelOperand &AArch64CGFunc::GetOrCreateLabelOperand(LabelIdx labIdx) {
  MapleMap<LabelIdx, LabelOperand*>::iterator it = hashLabelOpndTable.find(labIdx);
  if (it != hashLabelOpndTable.end()) {
    return *(it->second);
  }
  MIRSymbol *funcSt = GlobalTables::GetGsymTable().GetSymbolFromStidx(GetFunction().GetStIdx().Idx());
  const char *funcName = MapleString(funcSt->GetName(), memPool).c_str();
  LabelOperand *res = memPool->New<LabelOperand>(funcName, labIdx);
  hashLabelOpndTable[labIdx] = res;
  return *res;
}

LabelOperand &AArch64CGFunc::GetOrCreateLabelOperand(BB &bb) {
  LabelIdx labelIdx = bb.GetLabIdx();
  if (labelIdx == MIRLabelTable::GetDummyLabel()) {
    labelIdx = CreateLabel();
    bb.AddLabel(labelIdx);
  }
  return GetOrCreateLabelOperand(labelIdx);
}

LabelOperand &AArch64CGFunc::CreateFuncLabelOperand(const MIRSymbol &funcSymbol) {
  const char *funcName = memPool->New<std::string>(funcSymbol.GetName())->c_str();
  return *memPool->New<FunctionLabelOperand>(funcName);
}

AArch64OfstOperand &AArch64CGFunc::GetOrCreateOfstOpnd(uint32 offset, uint32 size) {
  AArch64OfstOperand tOfstOpnd(offset, size);
  auto it = hashOfstOpndTable.find(tOfstOpnd);
  if (it != hashOfstOpndTable.end()) {
    return *it->second;
  }
  AArch64OfstOperand *res = memPool->New<AArch64OfstOperand>(offset, size);
  hashOfstOpndTable[tOfstOpnd] = res;
  return *res;
}

MemOperand &AArch64CGFunc::GetOrCreateMemOpnd(const MIRSymbol &symbol, int32 offset, uint32 size, bool forLocalRef) {
  MIRStorageClass storageClass = symbol.GetStorageClass();
  if ((storageClass == kScAuto) || (storageClass == kScFormal)) {
    AArch64SymbolAlloc *symLoc =
      static_cast<AArch64SymbolAlloc*>(GetMemlayout()->GetSymAllocInfo(symbol.GetStIndex()));
    if (forLocalRef) {
      auto p = GetMemlayout()->GetLocalRefLocMap().find(symbol.GetStIdx());
      CHECK_FATAL(p != GetMemlayout()->GetLocalRefLocMap().end(), "sym loc should have been defined");
      symLoc = static_cast<AArch64SymbolAlloc*>(p->second);
    }
    ASSERT(symLoc != nullptr, "sym loc should have been defined");
    /* At this point, we don't know which registers the callee needs to save. */
    ASSERT((IsFPLRAddedToCalleeSavedList() || (SizeOfCalleeSaved() == 0)),
           "CalleeSaved won't be known until after Register Allocation");
    StIdx idx = symbol.GetStIdx();
    auto it = memOpndsRequiringOffsetAdjustment.find(idx);
    ASSERT((!IsFPLRAddedToCalleeSavedList() ||
            ((it != memOpndsRequiringOffsetAdjustment.end()) || (storageClass == kScFormal))),
           "Memory operand of this symbol should have been added to the hash table");
    if (it != memOpndsRequiringOffsetAdjustment.end()) {
      if (GetMemlayout()->IsLocalRefLoc(symbol)) {
        if (!forLocalRef) {
          return *(it->second);
        }
      } else {
        return *(it->second);
      }
    }
    it = memOpndsForStkPassedArguments.find(idx);
    if (it != memOpndsForStkPassedArguments.end()) {
      if (GetMemlayout()->IsLocalRefLoc(symbol)) {
        if (!forLocalRef) {
          return *(it->second);
        }
      } else {
        return *(it->second);
      }
    }

    AArch64RegOperand *baseOpnd = static_cast<AArch64RegOperand*>(GetBaseReg(*symLoc));
    int32 stOffset = GetBaseOffset(*symLoc);
    int32 totalOffset = stOffset + offset;
    /* needs a fresh copy of OfstOperand as we may adjust its offset at a later stage. */
    AArch64OfstOperand *offsetOpnd = memPool->New<AArch64OfstOperand>(totalOffset, k64BitSize);
    if (symLoc->GetMemSegment()->GetMemSegmentKind() == kMsArgsStkPassed &&
        AArch64MemOperand::IsPIMMOffsetOutOfRange(totalOffset, size)) {
      AArch64ImmOperand *offsetOprand;
      offsetOprand = &CreateImmOperand(totalOffset, k64BitSize, true, kUnAdjustVary);
      Operand *resImmOpnd = &SelectCopy(*offsetOprand, PTY_i64, PTY_i64);
      return *memPool->New<AArch64MemOperand>(AArch64MemOperand::kAddrModeBOrX, size, *baseOpnd,
                                              static_cast<AArch64RegOperand&>(*resImmOpnd), nullptr, symbol, true);
    } else {
      if (symLoc->GetMemSegment()->GetMemSegmentKind() == kMsArgsStkPassed) {
        offsetOpnd->SetVary(kUnAdjustVary);
      }
      AArch64MemOperand *res = memPool->New<AArch64MemOperand>(AArch64MemOperand::kAddrModeBOi, size, *baseOpnd,
                                                               nullptr, offsetOpnd, &symbol);
      if ((symbol.GetType()->GetKind() != kTypeClass) && !forLocalRef) {
        memOpndsRequiringOffsetAdjustment[idx] = res;
      }
      return *res;
    }
  } else if ((storageClass == kScGlobal) || (storageClass == kScExtern)) {
    StImmOperand &stOpnd = CreateStImmOperand(symbol, offset, 0);
    AArch64RegOperand &stAddrOpnd = static_cast<AArch64RegOperand&>(CreateRegisterOperandOfType(PTY_u64));
    SelectAddrof(stAddrOpnd, stOpnd);
    /* AArch64MemOperand::AddrMode_B_OI */
    return *memPool->New<AArch64MemOperand>(AArch64MemOperand::kAddrModeBOi, size, stAddrOpnd,
                                            nullptr, &GetOrCreateOfstOpnd(offset, k32BitSize), &symbol);
  } else if ((storageClass == kScPstatic) || (storageClass == kScFstatic)) {
    if (symbol.GetSKind() == kStConst) {
      ASSERT(offset == 0, "offset should be 0 for constant literals");
      return *memPool->New<AArch64MemOperand>(AArch64MemOperand::kAddrModeLiteral, size, symbol);
    } else {
      StImmOperand &stOpnd = CreateStImmOperand(symbol, offset, 0);
      AArch64RegOperand &stAddrOpnd = static_cast<AArch64RegOperand&>(CreateRegisterOperandOfType(PTY_u64));
      //  adrp    x1, _PTR__cinf_Ljava_2Flang_2FSystem_3B
      Insn &insn = GetCG()->BuildInstruction<AArch64Insn>(MOP_xadrp, stAddrOpnd, stOpnd);
      GetCurBB()->AppendInsn(insn);
      //  ldr     x1, [x1, #:lo12:_PTR__cinf_Ljava_2Flang_2FSystem_3B]
      return *memPool->New<AArch64MemOperand>(AArch64MemOperand::kAddrModeLo12Li, size, stAddrOpnd, nullptr,
                                              &GetOrCreateOfstOpnd(offset, k32BitSize), &symbol);
    }
  } else {
    CHECK_FATAL(false, "NYI");
  }
}

AArch64MemOperand &AArch64CGFunc::GetOrCreateMemOpnd(AArch64MemOperand::AArch64AddressingMode mode, uint32 size,
                                                     RegOperand *base, RegOperand *index, OfstOperand *offset,
                                                     const MIRSymbol *st) {
  AArch64MemOperand tMemOpnd(mode, size, *base, index, offset, st);
  auto it = hashMemOpndTable.find(tMemOpnd);
  if (it != hashMemOpndTable.end()) {
    return *(it->second);
  }
  AArch64MemOperand *res = memPool->New<AArch64MemOperand>(tMemOpnd);
  hashMemOpndTable[tMemOpnd] = res;
  return *res;
}

AArch64MemOperand &AArch64CGFunc::GetOrCreateMemOpnd(AArch64MemOperand::AArch64AddressingMode mode, uint32 size,
                                                     RegOperand *base, RegOperand *index, int32 shift,
                                                     bool isSigned) {
  AArch64MemOperand tMemOpnd(mode, size, *base, *index, shift, isSigned);
  auto it = hashMemOpndTable.find(tMemOpnd);
  if (it != hashMemOpndTable.end()) {
    return *(it->second);
  }
  AArch64MemOperand *res = memPool->New<AArch64MemOperand>(tMemOpnd);
  hashMemOpndTable[tMemOpnd] = res;
  return *res;
}

/* offset: base offset from FP or SP */
MemOperand &AArch64CGFunc::CreateMemOpnd(RegOperand &baseOpnd, int32 offset, uint32 size) {
  AArch64OfstOperand &offsetOpnd = CreateOfstOpnd(offset, k32BitSize);
  if (!ImmOperand::IsInBitSizeRot(kMaxAarch64ImmVal12Bits, offset)) {
    Operand *resImmOpnd = &SelectCopy(CreateImmOperand(offset, k32BitSize, true), PTY_i32, PTY_i32);
    return *memPool->New<AArch64MemOperand>(AArch64MemOperand::kAddrModeBOi, size, baseOpnd,
                                            static_cast<AArch64RegOperand*>(resImmOpnd), nullptr, nullptr);
  } else {
    ASSERT(!AArch64MemOperand::IsPIMMOffsetOutOfRange(offset, size), "should not be PIMMOffsetOutOfRange");
    return *memPool->New<AArch64MemOperand>(AArch64MemOperand::kAddrModeBOi, size, baseOpnd,
                                            nullptr, &offsetOpnd, nullptr);
  }
}

/* offset: base offset + #:lo12:Label+immediate */
MemOperand &AArch64CGFunc::CreateMemOpnd(RegOperand &baseOpnd, int32 offset, uint32 size, const MIRSymbol &sym) {
  AArch64OfstOperand &offsetOpnd = CreateOfstOpnd(offset, k32BitSize);
  ASSERT(ImmOperand::IsInBitSizeRot(kMaxAarch64ImmVal12Bits, offset), "");
  return *memPool->New<AArch64MemOperand>(AArch64MemOperand::kAddrModeBOi, size, baseOpnd, nullptr, &offsetOpnd, &sym);
}

/* iread a64 <* <* void>> 0 (add a64 (
 *   addrof a64 $__reg_jni_func_tab$$libcore_all_dex,
 *   mul a64 (
 *     cvt a64 i32 (constval i32 21),
 *     constval a64 8)))
 */
MemOperand *AArch64CGFunc::CheckAndCreateExtendMemOpnd(PrimType ptype, BaseNode &addrExpr, int32 offset,
                                                       AArch64isa::MemoryOrdering memOrd) {
  if (memOrd != AArch64isa::kMoNone || !IsPrimitiveInteger(ptype) || addrExpr.GetOpCode() != OP_add || offset != 0) {
    return nullptr;
  }
  BaseNode *baseExpr = addrExpr.Opnd(0);
  BaseNode *addendExpr = addrExpr.Opnd(1);
  if (addendExpr->GetOpCode() != OP_mul) {
    return nullptr;
  }
  BaseNode *indexExpr, *scaleExpr;
  indexExpr = addendExpr->Opnd(0);
  scaleExpr = addendExpr->Opnd(1);
  if (scaleExpr->GetOpCode() != OP_constval) {
    return nullptr;
  }
  ConstvalNode *constValNode = static_cast<ConstvalNode*>(scaleExpr);
  CHECK_FATAL(constValNode->GetConstVal()->GetKind() == kConstInt, "expect MIRIntConst");
  MIRIntConst *mirIntConst = safe_cast<MIRIntConst>(constValNode->GetConstVal());
  CHECK_FATAL(mirIntConst != nullptr, "just checking");
  int32 scale = mirIntConst->GetValue();
  if (scale < 0) {
    return nullptr;
  }
  uint32 unsignedScale = static_cast<uint32>(scale);
  if (unsignedScale != GetPrimTypeSize(ptype) || indexExpr->GetOpCode() != OP_cvt) {
    return nullptr;
  }
  /* 8 is 1 << 3; 4 is 1 << 2; 2 is 1 << 1; 1 is 1 << 0 */
  int32 shift = (unsignedScale == 8) ? 3 : ((unsignedScale == 4) ? 2 : ((unsignedScale == 2) ? 1 : 0));
  RegOperand &base = static_cast<RegOperand&>(LoadIntoRegister(*HandleExpr(addrExpr, *baseExpr), PTY_a64));
  TypeCvtNode *typeCvtNode = static_cast<TypeCvtNode*>(indexExpr);
  PrimType fromType = typeCvtNode->FromType();
  PrimType toType = typeCvtNode->GetPrimType();
  MemOperand *memOpnd = nullptr;
  if ((fromType == PTY_i32) && (toType == PTY_a64)) {
    RegOperand &index =
        static_cast<RegOperand&>(LoadIntoRegister(*HandleExpr(*indexExpr, *indexExpr->Opnd(0)), PTY_i32));
    memOpnd = &GetOrCreateMemOpnd(AArch64MemOperand::kAddrModeBOrX, GetPrimTypeBitSize(ptype), &base, &index,
                                  shift, true);
  } else if ((fromType == PTY_u32) && (toType == PTY_a64)) {
    RegOperand &index =
        static_cast<RegOperand&>(LoadIntoRegister(*HandleExpr(*indexExpr, *indexExpr->Opnd(0)), PTY_u32));
    memOpnd = &GetOrCreateMemOpnd(AArch64MemOperand::kAddrModeBOrX, GetPrimTypeBitSize(ptype), &base, &index,
                                  shift, false);
  }
  return memOpnd;
}

/*
 * Create a memory operand with specified data type and memory ordering, making
 * use of aarch64 extend register addressing mode when possible.
 */
MemOperand &AArch64CGFunc::CreateMemOpnd(PrimType ptype, const BaseNode &parent, BaseNode &addrExpr, int32 offset,
                                         AArch64isa::MemoryOrdering memOrd) {
  MemOperand *memOpnd = CheckAndCreateExtendMemOpnd(ptype, addrExpr, offset, memOrd);
  if (memOpnd != nullptr) {
    return *memOpnd;
  }
  Operand *addrOpnd = HandleExpr(parent, addrExpr);
  addrOpnd = static_cast<RegOperand*>(&LoadIntoRegister(*addrOpnd, PTY_a64));
  AArch64OfstOperand &ofstOpnd = GetOrCreateOfstOpnd(offset, k32BitSize);
  return GetOrCreateMemOpnd(AArch64MemOperand::kAddrModeBOi, GetPrimTypeBitSize(ptype),
                            static_cast<AArch64RegOperand*>(addrOpnd), nullptr, &ofstOpnd, nullptr);
}

Operand &AArch64CGFunc::GetOrCreateFuncNameOpnd(const MIRSymbol &symbol) {
  return *memPool->New<FuncNameOperand>(symbol);
}

Operand &AArch64CGFunc::GetOrCreateRflag() {
  if (rcc == nullptr) {
    rcc = &CreateRflagOperand();
  }
  return *rcc;
}

const Operand *AArch64CGFunc::GetRflag() const {
  return rcc;
}

Operand &AArch64CGFunc::GetOrCreatevaryreg() {
  if (vary == nullptr) {
    regno_t vRegNO = NewVReg(kRegTyVary, k8ByteSize);
    vary = &CreateVirtualRegisterOperand(vRegNO);
  }
  return *vary;
}

/* the first operand in opndvec is return opnd */
void AArch64CGFunc::SelectLibCall(const std::string &funcName, std::vector<Operand*> &opndVec, PrimType primType,
                                  PrimType retPrimType, bool is2ndRet) {
  MIRSymbol *st = GlobalTables::GetGsymTable().CreateSymbol(kScopeGlobal);
  st->SetNameStrIdx(funcName);
  st->SetStorageClass(kScExtern);
  st->SetSKind(kStFunc);
  /* setup the type of the callee function */
  std::vector<TyIdx> vec;
  std::vector<TypeAttrs> vecAt;
  for (size_t i = 1; i < opndVec.size(); ++i) {
    vec.push_back(GlobalTables::GetTypeTable().GetTypeTable()[static_cast<int32>(primType)]->GetTypeIndex());
    vecAt.push_back(TypeAttrs());
  }

  MIRType *retType = GlobalTables::GetTypeTable().GetTypeTable().at(static_cast<int32>(primType));
  st->SetTyIdx(GetBecommon().BeGetOrCreateFunctionType(retType->GetTypeIndex(), vec, vecAt)->GetTypeIndex());

  if (GetCG()->GenerateVerboseAsm()) {
    const std::string &comment = "lib call : " + funcName;
    GetCurBB()->AppendInsn(CreateCommentInsn(comment));
  }

  ParmLocator parmLocator(GetBecommon());
  PLocInfo ploc;
  ASSERT(primType != PTY_void, "primType check");
  /* setup actual parameters */
  AArch64ListOperand *srcOpnds = memPool->New<AArch64ListOperand>(*GetFuncScopeAllocator());
  for (size_t i = 1; i < opndVec.size(); ++i) {
    MIRType *ty;
    ty = GlobalTables::GetTypeTable().GetTypeTable()[static_cast<uint32>(primType)];
    Operand *stOpnd = opndVec[i];
    if (stOpnd->GetKind() != Operand::kOpdRegister) {
      stOpnd = &SelectCopy(*stOpnd, primType, primType);
    }
    RegOperand *expRegOpnd = static_cast<RegOperand*>(stOpnd);
    parmLocator.LocateNextParm(*ty, ploc);
    if (ploc.reg0 != 0) {  /* load to the register */
      AArch64RegOperand &parmRegOpnd =
          GetOrCreatePhysicalRegisterOperand(ploc.reg0, expRegOpnd->GetSize(), GetRegTyFromPrimTy(primType));
      SelectCopy(parmRegOpnd, primType, *expRegOpnd, primType);
      srcOpnds->PushOpnd(parmRegOpnd);
    }
    ASSERT(ploc.reg1 == 0, "SelectCall NYI");
  }

  MIRSymbol *sym = GetFunction().GetLocalOrGlobalSymbol(st->GetStIdx(), false);
  Insn &callInsn = AppendCall(*sym, *srcOpnds);
  MIRType *callRetType = GlobalTables::GetTypeTable().GetTypeTable().at(static_cast<int32>(retPrimType));
  if (callRetType != nullptr) {
    callInsn.SetRetSize(callRetType->GetSize());
  }
  GetFunction().SetHasCall();
  /* get return value */
  Operand *opnd0 = opndVec[0];
  ReturnMechanism retMech(*(GlobalTables::GetTypeTable().GetTypeTable().at(retPrimType)), GetBecommon());
  if (retMech.GetRegCount() <= 0) {
    CHECK_FATAL(false, "should return from register");
  }
  if (!opnd0->IsRegister()) {
    CHECK_FATAL(false, "nyi");
  }
  RegOperand *regOpnd = static_cast<RegOperand*>(opnd0);
  AArch64reg regNum = is2ndRet ? retMech.GetReg1() : retMech.GetReg0();
  if (regOpnd->GetRegisterNumber() != regNum) {
    AArch64RegOperand &retOpnd = GetOrCreatePhysicalRegisterOperand(regNum, regOpnd->GetSize(),
                                                                    GetRegTyFromPrimTy(retPrimType));
    SelectCopy(*opnd0, retPrimType, retOpnd, retPrimType);
  }
}

Operand *AArch64CGFunc::GetBaseReg(const AArch64SymbolAlloc &symAlloc) {
  MemSegmentKind sgKind = symAlloc.GetMemSegment()->GetMemSegmentKind();
  ASSERT(((sgKind == kMsArgsRegPassed) || (sgKind == kMsLocals) || (sgKind == kMsRefLocals) ||
          (sgKind == kMsArgsToStkPass) || (sgKind == kMsArgsStkPassed)), "NYI");

  if (sgKind == kMsArgsStkPassed) {
    return &GetOrCreatevaryreg();
  }

  if (fsp == nullptr) {
    fsp = &GetOrCreatePhysicalRegisterOperand(RFP, kSizeOfPtr * kBitsPerByte, kRegTyInt);
  }
  return fsp;
}

uint32 AArch64CGFunc::GetBaseOffset(const SymbolAlloc &sa) {
  const AArch64SymbolAlloc *symAlloc = static_cast<const AArch64SymbolAlloc*>(&sa);
  /* Call Frame layout of AArch64
   * Refer to V2 in aarch64_memlayout.h.
   * Do Not change this unless you know what you do
   */
  const int32 sizeofFplr = 2 * kIntregBytelen;
  MemSegmentKind sgKind = symAlloc->GetMemSegment()->GetMemSegmentKind();
  AArch64MemLayout *memLayout = static_cast<AArch64MemLayout*>(this->GetMemlayout());
  if (sgKind == kMsArgsStkPassed) {  /* for callees */
    uint32 offset = symAlloc->GetOffset();
    return offset;
  } else if (sgKind == kMsArgsRegPassed) {
    int32 baseOffset = memLayout->GetSizeOfLocals() + symAlloc->GetOffset() + memLayout->GetSizeOfRefLocals();
    return baseOffset + sizeofFplr;
  } else if (sgKind == kMsRefLocals) {
    int32 baseOffset = symAlloc->GetOffset() + memLayout->GetSizeOfLocals();
    return baseOffset + sizeofFplr;
  } else if (sgKind == kMsLocals) {
    int32 baseOffset = symAlloc->GetOffset();
    return baseOffset + sizeofFplr;
  } else if (sgKind == kMsSpillReg) {
    int32 baseOffset = symAlloc->GetOffset() + memLayout->SizeOfArgsRegisterPassed() + memLayout->GetSizeOfLocals() +
                     memLayout->GetSizeOfRefLocals();
    return baseOffset + sizeofFplr;
  } else if (sgKind == kMsArgsToStkPass) {  /* this is for callers */
    return static_cast<uint32>(symAlloc->GetOffset());
  } else {
    CHECK_FATAL(false, "sgKind check");
  }
  return 0;
}

void AArch64CGFunc::AppendCall(const MIRSymbol &funcSymbol) {
  AArch64ListOperand *srcOpnds = memPool->New<AArch64ListOperand>(*GetFuncScopeAllocator());
  AppendCall(funcSymbol, *srcOpnds);
}

void AArch64CGFunc::SelectAddAfterInsn(Operand &resOpnd, Operand &opnd0, Operand &opnd1, PrimType primType,
                                       uint32 isDest, Insn &insn) {
  uint32 dsize = GetPrimTypeBitSize(primType);
  bool is64Bits = (dsize == k64BitSize);
  ASSERT(opnd0.GetKind() == Operand::kOpdRegister, "Spill memory operand should based on register");
  ASSERT((opnd1.GetKind() == Operand::kOpdImmediate || opnd1.GetKind() == Operand::kOpdOffset),
         "Spill memory operand should be with a immediate offset.");

  AArch64ImmOperand *immOpnd = static_cast<AArch64ImmOperand*>(&opnd1);
  ASSERT(!immOpnd->IsNegative(), "Spill offset should be positive number.");

  MOperator mOpCode = MOP_undef;
  /* lower 24 bits has 1, higher bits are all 0 */
  if (immOpnd->IsInBitSize(kMaxAarch64ImmVal24Bits)) {
    /* lower 12 bits and higher 12 bits both has 1 */
    Operand *newOpnd0 = &opnd0;
    if (!(immOpnd->IsInBitSize(kMaxAarch64ImmVal12Bits) ||
          immOpnd->IsInBitSize(kMaxAarch64ImmVal12Bits, kMaxAarch64ImmVal12Bits))) {
      /* process higher 12 bits */
      ImmOperand &immOpnd2 = CreateImmOperand(static_cast<uint64>(immOpnd->GetValue()) >> kMaxAarch64ImmVal12Bits,
                                              immOpnd->GetSize(), immOpnd->IsSignedValue());
      mOpCode = is64Bits ? MOP_xaddrri24 : MOP_waddrri24;
      Insn &newInsn = GetCG()->BuildInstruction<AArch64Insn>(mOpCode, resOpnd, opnd0, immOpnd2, addSubLslOperand);
      if (isDest) {
        insn.GetBB()->InsertInsnAfter(insn, newInsn);
      } else {
        insn.GetBB()->InsertInsnBefore(insn, newInsn);
      }
      /* get lower 12 bits value */
      immOpnd->ModuloByPow2(kMaxAarch64ImmVal12Bits);
      newOpnd0 = &resOpnd;
    }
    /* process lower 12 bits value */
    mOpCode = is64Bits ? MOP_xaddrri12 : MOP_waddrri12;
    Insn &newInsn = GetCG()->BuildInstruction<AArch64Insn>(mOpCode, resOpnd, *newOpnd0, *immOpnd);
    if (isDest) {
      insn.GetBB()->InsertInsnAfter(insn, newInsn);
    } else {
      insn.GetBB()->InsertInsnBefore(insn, newInsn);
    }
  } else {
    /* load into register */
    RegOperand &regOpnd = CreateRegisterOperandOfType(primType);
    SelectCopyImm(regOpnd, *immOpnd, primType);
    mOpCode = is64Bits ? MOP_xaddrrr : MOP_waddrrr;
    Insn &newInsn = GetCG()->BuildInstruction<AArch64Insn>(mOpCode, resOpnd, opnd0, regOpnd);
    if (isDest) {
      insn.GetBB()->InsertInsnAfter(insn, newInsn);
    } else {
      insn.GetBB()->InsertInsnBefore(insn, newInsn);
    }
  }
}

MemOperand *AArch64CGFunc::AdjustMemOperandIfOffsetOutOfRange(
    MemOperand *memOpnd, regno_t vrNum, uint8 isDest, Insn &insn, AArch64reg regNum, uint8 &isOutOfRange) {
  if (vrNum >= vRegTable.size()) {
    CHECK_FATAL(false, "index out of range in AArch64CGFunc::AdjustMemOperandIfOffsetOutOfRange");
  }
  uint32 dataSize = vRegTable[vrNum].GetSize() * kBitsPerByte;
  auto *a64MemOpnd = static_cast<AArch64MemOperand*>(memOpnd);
  if (IsImmediateOffsetOutOfRange(*a64MemOpnd, dataSize)) {
    if (CheckIfSplitOffsetWithAdd(*a64MemOpnd, dataSize)) {
      isOutOfRange = 1;
    }
    memOpnd =
        &SplitOffsetWithAddInstruction(*a64MemOpnd, dataSize, regNum, isDest, &insn);
  } else {
    isOutOfRange = 0;
  }
  return memOpnd;
}

void AArch64CGFunc::FreeSpillRegMem(regno_t vrNum) {
  MemOperand *memOpnd = nullptr;

  auto p = spillRegMemOperands.find(vrNum);
  if (p != spillRegMemOperands.end()) {
    memOpnd = p->second;
  }

  if ((memOpnd == nullptr) && IsVRegNOForPseudoRegister(vrNum)) {
    auto pSecond = pRegSpillMemOperands.find(GetPseudoRegIdxFromVirtualRegNO(vrNum));
    if (pSecond != pRegSpillMemOperands.end()) {
      memOpnd = pSecond->second;
    }
  }

  if (memOpnd == nullptr) {
    ASSERT(false, "free spillreg have no mem");
    return;
  }

  uint32 size = memOpnd->GetSize();
  MapleMap<uint32, SpillMemOperandSet*>::iterator iter;
  if ((iter = reuseSpillLocMem.find(size)) != reuseSpillLocMem.end()) {
    iter->second->Add(*memOpnd);
  } else {
    reuseSpillLocMem[size] = memPool->New<SpillMemOperandSet>(*GetFuncScopeAllocator());
    reuseSpillLocMem[size]->Add(*memOpnd);
  }
}

MemOperand *AArch64CGFunc::GetOrCreatSpillMem(regno_t vrNum) {
  /* NOTES: must used in RA, not used in other place. */
  if (IsVRegNOForPseudoRegister(vrNum)) {
    auto p = pRegSpillMemOperands.find(GetPseudoRegIdxFromVirtualRegNO(vrNum));
    if (p != pRegSpillMemOperands.end()) {
      return p->second;
    }
  }

  auto p = spillRegMemOperands.find(vrNum);
  if (p == spillRegMemOperands.end()) {
    if (vrNum >= vRegTable.size()) {
      CHECK_FATAL(false, "index out of range in AArch64CGFunc::FreeSpillRegMem");
    }
    uint32 dataSize = vRegTable[vrNum].GetSize() * kBitsPerByte;
    auto it = reuseSpillLocMem.find(dataSize);
    if (it != reuseSpillLocMem.end()) {
      MemOperand *memOpnd = it->second->GetOne();
      if (memOpnd != nullptr) {
        spillRegMemOperands.insert(std::pair<regno_t, MemOperand*>(vrNum, memOpnd));
        return memOpnd;
      }
    }

    RegOperand &baseOpnd = GetOrCreateStackBaseRegOperand();
    int32 offset = GetOrCreatSpillRegLocation(vrNum);
    AArch64OfstOperand *offsetOpnd = memPool->New<AArch64OfstOperand>(offset, k64BitSize);
    MemOperand *memOpnd = memPool->New<AArch64MemOperand>(AArch64MemOperand::kAddrModeBOi, dataSize, baseOpnd,
                                                          nullptr, offsetOpnd, nullptr);
    spillRegMemOperands.insert(std::pair<regno_t, MemOperand*>(vrNum, memOpnd));
    spillRegMemOperandsAdj.insert(memOpnd);
    return memOpnd;
  } else {
    return p->second;
  }
}

MemOperand *AArch64CGFunc::GetPseudoRegisterSpillMemoryOperand(PregIdx i) {
  auto p = pRegSpillMemOperands.find(i);
  if (p != pRegSpillMemOperands.end()) {
    return p->second;
  }
  int64 offset = GetPseudoRegisterSpillLocation(i);
  MIRPreg *preg = GetFunction().GetPregTab()->PregFromPregIdx(i);
  uint32 bitLen = GetPrimTypeSize(preg->GetPrimType()) * kBitsPerByte;
  RegOperand &base = GetOrCreateFramePointerRegOperand();

  AArch64OfstOperand &ofstOpnd = GetOrCreateOfstOpnd(offset, k32BitSize);
  MemOperand &memOpnd = GetOrCreateMemOpnd(AArch64MemOperand::kAddrModeBOi, bitLen, &base, nullptr, &ofstOpnd, nullptr);
  if (IsImmediateOffsetOutOfRange(static_cast<AArch64MemOperand&>(memOpnd), bitLen)) {
    MemOperand &newMemOpnd = SplitOffsetWithAddInstruction(static_cast<AArch64MemOperand&>(memOpnd), bitLen);
    pRegSpillMemOperands.insert(std::pair<PregIdx, MemOperand*>(i, &newMemOpnd));
    return &newMemOpnd;
  }
  pRegSpillMemOperands.insert(std::pair<PregIdx, MemOperand*>(i, &memOpnd));
  return &memOpnd;
}

/* Get the number of return register of current function. */
AArch64reg AArch64CGFunc::GetReturnRegisterNumber() {
  ReturnMechanism retMech(*(GetFunction().GetReturnType()), GetBecommon());
  if (retMech.GetRegCount() > 0) {
    return retMech.GetReg0();
  }
  return kRinvalid;
}

bool AArch64CGFunc::CanLazyBinding(const Insn &ldrInsn) {
  Operand &memOpnd = ldrInsn.GetOperand(1);
  auto &aarchMemOpnd = static_cast<AArch64MemOperand&>(memOpnd);
  if (aarchMemOpnd.GetAddrMode() != AArch64MemOperand::kAddrModeLo12Li) {
    return false;
  }

  const MIRSymbol *sym = aarchMemOpnd.GetSymbol();
  CHECK_FATAL(sym != nullptr, "sym can't be nullptr");
  if (sym->IsMuidFuncDefTab() || sym->IsMuidFuncUndefTab() ||
      sym->IsMuidDataDefTab() || sym->IsMuidDataUndefTab() ||
      (sym->IsReflectionClassInfo() && !sym->IsReflectionArrayClassInfo())) {
    return true;
  }

  return false;
}

/*
 *  add reg, reg, __PTR_C_STR_...
 *  ldr reg1, [reg]
 *  =>
 *  ldr reg1, [reg, #:lo12:__Ptr_C_STR_...]
 */
void AArch64CGFunc::ConvertAdrpl12LdrToLdr() {
  FOR_ALL_BB(bb, this) {
    FOR_BB_INSNS_SAFE(insn, bb, nextInsn) {
      nextInsn = insn->GetNextMachineInsn();
      if (nextInsn == nullptr) {
        break;
      }
      if (!insn->IsMachineInstruction()) {
        continue;
      }
      /* check first insn */
      MOperator thisMop = insn->GetMachineOpcode();
      if (thisMop != MOP_xadrpl12) {
        continue;
      }
      /* check second insn */
      MOperator nextMop = nextInsn->GetMachineOpcode();
      if (!(((nextMop >= MOP_wldrsb) && (nextMop <= MOP_dldp)) || ((nextMop >= MOP_wstrb) && (nextMop <= MOP_dstp)))) {
        continue;
      }

      /* Check if base register of nextInsn and the dest operand of insn are identical. */
      AArch64MemOperand *memOpnd = static_cast<AArch64MemOperand*>(nextInsn->GetMemOpnd());
      CHECK_FATAL(memOpnd != nullptr, "memOpnd can't be nullptr");

      /* Only for AddrMode_B_OI addressing mode. */
      if (memOpnd->GetAddrMode() != AArch64MemOperand::kAddrModeBOi) {
        continue;
      }

      /* Only for intact memory addressing. */
      if (!memOpnd->IsIntactIndexed()) {
        continue;
      }

      auto &regOpnd = static_cast<RegOperand&>(insn->GetOperand(0));

      /* Check if dest operand of insn is idential with base register of nextInsn. */
      RegOperand *baseReg = memOpnd->GetBaseRegister();
      CHECK_FATAL(baseReg != nullptr, "baseReg can't be nullptr");
      if (baseReg->GetRegisterNumber() != regOpnd.GetRegisterNumber()) {
        continue;
      }

      StImmOperand &stImmOpnd = static_cast<StImmOperand&>(insn->GetOperand(kInsnThirdOpnd));
      AArch64OfstOperand &ofstOpnd = GetOrCreateOfstOpnd(
          stImmOpnd.GetOffset() + memOpnd->GetOffsetImmediate()->GetOffsetValue(), k32BitSize);
      RegOperand &newBaseOpnd = static_cast<RegOperand&>(insn->GetOperand(kInsnSecondOpnd));
      AArch64MemOperand &newMemOpnd = GetOrCreateMemOpnd(AArch64MemOperand::kAddrModeLo12Li, memOpnd->GetSize(),
                                                         &newBaseOpnd, nullptr, &ofstOpnd, stImmOpnd.GetSymbol());
      nextInsn->SetOperand(1, newMemOpnd);
      bb->RemoveInsn(*insn);
    }
  }
}

/*
 * adrp reg1, __muid_func_undef_tab..
 * ldr reg2, [reg1, #:lo12:__muid_func_undef_tab..]
 * =>
 * intrinsic_adrp_ldr reg2, __muid_func_undef_tab...
 */
void AArch64CGFunc::ConvertAdrpLdrToIntrisic() {
  FOR_ALL_BB(bb, this) {
    FOR_BB_INSNS_SAFE(insn, bb, nextInsn) {
      nextInsn = insn->GetNextMachineInsn();
      if (nextInsn == nullptr) {
        break;
      }
      if (!insn->IsMachineInstruction()) {
        continue;
      }

      MOperator firstMop = insn->GetMachineOpcode();
      MOperator secondMop = nextInsn->GetMachineOpcode();
      if (!((firstMop == MOP_xadrp) && ((secondMop == MOP_wldr) || (secondMop == MOP_xldr)))) {
        continue;
      }

      if (CanLazyBinding(*nextInsn)) {
        bb->ReplaceInsn(*insn, GetCG()->BuildInstruction<AArch64Insn>(MOP_adrp_ldr, nextInsn->GetOperand(0),
                                                                      insn->GetOperand(1)));
        bb->RemoveInsn(*nextInsn);
      }
    }
  }
}

void AArch64CGFunc::ProcessLazyBinding() {
  ConvertAdrpl12LdrToLdr();
  ConvertAdrpLdrToIntrisic();
}

/*
 * Generate global long call
 *  adrp  VRx, symbol
 *  ldr VRx, [VRx, #:lo12:symbol]
 *  blr VRx
 *
 * Input:
 *  insn       : insert new instruction after the 'insn'
 *  func       : the symbol of the function need to be called
 *  srcOpnds   : list operand of the function need to be called
 *  isCleanCall: when generate clean call insn, set isCleanCall as true
 * Return: the 'blr' instruction
 */
Insn &AArch64CGFunc::GenerateGlobalLongCallAfterInsn(const MIRSymbol &func, AArch64ListOperand &srcOpnds,
                                                     bool isCleanCall) {
  MIRSymbol *symbol = GetFunction().GetLocalOrGlobalSymbol(func.GetStIdx());
  symbol->SetStorageClass(kScGlobal);
  RegOperand &tmpReg = CreateRegisterOperandOfType(PTY_u64);
  StImmOperand &stOpnd = CreateStImmOperand(*symbol, 0, 0);
  AArch64OfstOperand &offsetOpnd = CreateOfstOpnd(*symbol, 0);
  Insn &adrpInsn = GetCG()->BuildInstruction<AArch64Insn>(MOP_xadrp, tmpReg, stOpnd);
  GetCurBB()->AppendInsn(adrpInsn);
  AArch64MemOperand &memOrd = GetOrCreateMemOpnd(AArch64MemOperand::kAddrModeLo12Li, kSizeOfPtr * kBitsPerByte,
                                                 static_cast<AArch64RegOperand*>(&tmpReg),
                                                 nullptr, &offsetOpnd, symbol);
  Insn &ldrInsn = GetCG()->BuildInstruction<AArch64Insn>(MOP_xldr, tmpReg, memOrd);
  GetCurBB()->AppendInsn(ldrInsn);

  if (isCleanCall) {
    Insn &callInsn = GetCG()->BuildInstruction<AArch64cleancallInsn>(MOP_xblr, tmpReg, srcOpnds);
    GetCurBB()->AppendInsn(callInsn);
    GetCurBB()->SetHasCall();
    return callInsn;
  } else {
    Insn &callInsn = GetCG()->BuildInstruction<AArch64Insn>(MOP_xblr, tmpReg, srcOpnds);
    GetCurBB()->AppendInsn(callInsn);
    GetCurBB()->SetHasCall();
    return callInsn;
  }
}

/*
 * Generate local long call
 *  adrp  VRx, symbol
 *  add VRx, VRx, #:lo12:symbol
 *  blr VRx
 *
 * Input:
 *  insn       : insert new instruction after the 'insn'
 *  func       : the symbol of the function need to be called
 *  srcOpnds   : list operand of the function need to be called
 *  isCleanCall: when generate clean call insn, set isCleanCall as true
 * Return: the 'blr' instruction
 */
Insn &AArch64CGFunc::GenerateLocalLongCallAfterInsn(const MIRSymbol &func, AArch64ListOperand &srcOpnds,
                                                    bool isCleanCall) {
  RegOperand &tmpReg = CreateRegisterOperandOfType(PTY_u64);
  StImmOperand &stOpnd = CreateStImmOperand(func, 0, 0);
  Insn &adrpInsn = GetCG()->BuildInstruction<AArch64Insn>(MOP_xadrp, tmpReg, stOpnd);
  GetCurBB()->AppendInsn(adrpInsn);
  Insn &addInsn = GetCG()->BuildInstruction<AArch64Insn>(MOP_xadrpl12, tmpReg, tmpReg, stOpnd);
  GetCurBB()->AppendInsn(addInsn);
  Insn *callInsn = nullptr;
  if (isCleanCall) {
    callInsn = &GetCG()->BuildInstruction<AArch64cleancallInsn>(MOP_xblr, tmpReg, srcOpnds);
    GetCurBB()->AppendInsn(*callInsn);
  } else {
    callInsn = &GetCG()->BuildInstruction<AArch64Insn>(MOP_xblr, tmpReg, srcOpnds);
    GetCurBB()->AppendInsn(*callInsn);
  }
  GetCurBB()->SetHasCall();
  return *callInsn;
}

Insn &AArch64CGFunc::AppendCall(const MIRSymbol &sym, AArch64ListOperand &srcOpnds, bool isCleanCall) {
  Insn *callInsn = nullptr;
  if (CGOptions::IsLongCalls()) {
    MIRFunction *mirFunc = sym.GetFunction();
    if (IsDuplicateAsmList(sym) || (mirFunc && mirFunc->GetAttr(FUNCATTR_local))) {
      callInsn = &GenerateLocalLongCallAfterInsn(sym, srcOpnds, isCleanCall);
    } else {
      callInsn = &GenerateGlobalLongCallAfterInsn(sym, srcOpnds, isCleanCall);
    }
  } else {
    Operand &targetOpnd = GetOrCreateFuncNameOpnd(sym);
    if (isCleanCall) {
      callInsn = &GetCG()->BuildInstruction<AArch64cleancallInsn>(MOP_xbl, targetOpnd, srcOpnds);
      GetCurBB()->AppendInsn(*callInsn);
    } else {
      callInsn = &GetCG()->BuildInstruction<AArch64Insn>(MOP_xbl, targetOpnd, srcOpnds);
      GetCurBB()->AppendInsn(*callInsn);
    }
    GetCurBB()->SetHasCall();
  }
  return *callInsn;
}

bool AArch64CGFunc::IsDuplicateAsmList(const MIRSymbol &sym) const {
  if (CGOptions::IsDuplicateAsmFileEmpty()) {
    return false;
  }

  const std::string &name = sym.GetName();
  if ((name == "strlen") ||
      (name == "strncmp") ||
      (name == "memcpy") ||
      (name == "memmove") ||
      (name == "strcmp") ||
      (name == "memcmp") ||
      (name == "memcmpMpl")) {
    return true;
  }
  return false;
}

void AArch64CGFunc::SelectMPLProfCounterInc(IntrinsiccallNode &intrnNode) {
  ASSERT(intrnNode.NumOpnds() == 1, "must be 1 operand");
  BaseNode *arg1 = intrnNode.Opnd(0);
  ASSERT(arg1 != nullptr, "nullptr check");
  regno_t vRegNO1 = NewVReg(GetRegTyFromPrimTy(PTY_a64), GetPrimTypeSize(PTY_a64));
  RegOperand &vReg1 = CreateVirtualRegisterOperand(vRegNO1);
  vReg1.SetRegNotBBLocal();
  static MIRSymbol *bbProfileTab = nullptr;
  if (!bbProfileTab) {
    std::string bbProfileName = NameMangler::kBBProfileTabPrefixStr + GetMirModule().GetFileNameAsPostfix();
    bbProfileTab = GetMirModule().GetMIRBuilder()->GetGlobalDecl(bbProfileName);
    CHECK_FATAL(bbProfileTab != nullptr, "expect bb profile tab");
  }
  ConstvalNode *constvalNode = static_cast<ConstvalNode*>(arg1);
  MIRConst *mirConst = constvalNode->GetConstVal();
  ASSERT(mirConst != nullptr, "nullptr check");
  CHECK_FATAL(mirConst->GetKind() == kConstInt, "expect MIRIntConst type");
  MIRIntConst *mirIntConst = safe_cast<MIRIntConst>(mirConst);
  uint32 idx = GetPrimTypeSize(PTY_u32) * mirIntConst->GetValue();
  if (!GetCG()->IsQuiet()) {
    maple::logInfo.MapleLogger(kLlErr) << "Id index " << idx << std::endl;
  }
  StImmOperand &stOpnd = CreateStImmOperand(*bbProfileTab, idx, 0);
  Insn &newInsn = GetCG()->BuildInstruction<AArch64Insn>(MOP_counter, vReg1, stOpnd);
  newInsn.SetDoNotRemove(true);
  GetCurBB()->AppendInsn(newInsn);
}

void AArch64CGFunc::SelectMPLClinitCheck(IntrinsiccallNode &intrnNode) {
  ASSERT(intrnNode.NumOpnds() == 1, "must be 1 operand");
  BaseNode *arg = intrnNode.Opnd(0);
  Operand *stOpnd = nullptr;
  bool bClinitSeperate = false;
  ASSERT(CGOptions::IsPIC(), "must be doPIC");
  if (arg->GetOpCode() == OP_addrof) {
    AddrofNode *addrof = static_cast<AddrofNode*>(arg);
    MIRSymbol *symbol = GetFunction().GetLocalOrGlobalSymbol(addrof->GetStIdx());
    ASSERT(symbol->GetName().find(CLASSINFO_PREFIX_STR) == 0, "must be a symbol with __classinfo__");

    if (!symbol->IsMuidDataUndefTab()) {
      std::string ptrName = NameMangler::kPtrPrefixStr + symbol->GetName();
      MIRType *ptrType = GlobalTables::GetTypeTable().GetPtr();
      symbol = GetMirModule().GetMIRBuilder()->GetOrCreateGlobalDecl(ptrName, *ptrType);
      bClinitSeperate = true;
      symbol->SetStorageClass(kScFstatic);
    }
    stOpnd = &CreateStImmOperand(*symbol, 0, 0);
  } else {
    arg = arg->Opnd(0);
    BaseNode *arg0 = arg->Opnd(0);
    BaseNode *arg1 = arg->Opnd(1);
    ASSERT(arg0 != nullptr, "nullptr check");
    ASSERT(arg1 != nullptr, "nullptr check");
    ASSERT(arg0->GetOpCode() == OP_addrof, "expect the operand to be addrof");
    AddrofNode *addrof = static_cast<AddrofNode*>(arg0);
    MIRSymbol *symbol = GetFunction().GetLocalOrGlobalSymbol(addrof->GetStIdx());
    ASSERT(addrof->GetFieldID() == 0, "For debug SelectMPLClinitCheck.");
    ConstvalNode *constvalNode = static_cast<ConstvalNode*>(arg1);
    MIRConst *mirConst = constvalNode->GetConstVal();
    ASSERT(mirConst != nullptr, "nullptr check");
    CHECK_FATAL(mirConst->GetKind() == kConstInt, "expect MIRIntConst type");
    MIRIntConst *mirIntConst = safe_cast<MIRIntConst>(mirConst);
    stOpnd = &CreateStImmOperand(*symbol, mirIntConst->GetValue(), 0);
  }

  regno_t vRegNO2 = NewVReg(GetRegTyFromPrimTy(PTY_a64), GetPrimTypeSize(PTY_a64));
  RegOperand &vReg2 = CreateVirtualRegisterOperand(vRegNO2);
  vReg2.SetRegNotBBLocal();
  if (bClinitSeperate) {
    /* Seperate MOP_clinit to MOP_adrp_ldr + MOP_clinit_tail. */
    Insn &newInsn = GetCG()->BuildInstruction<AArch64Insn>(MOP_adrp_ldr, vReg2, *stOpnd);
    GetCurBB()->AppendInsn(newInsn);
    newInsn.SetDoNotRemove(true);
    Insn &insn = GetCG()->BuildInstruction<AArch64Insn>(MOP_clinit_tail, vReg2);
    insn.SetDoNotRemove(true);
    GetCurBB()->AppendInsn(insn);
  } else {
    Insn &newInsn = GetCG()->BuildInstruction<AArch64Insn>(MOP_clinit, vReg2, *stOpnd);
    GetCurBB()->AppendInsn(newInsn);
  }
}

void AArch64CGFunc::SelectIntrinCall(IntrinsiccallNode &intrinsiccallNode) {
  MIRIntrinsicID intrinsic = intrinsiccallNode.GetIntrinsic();

  if (GetCG()->GenerateVerboseAsm()) {
    std::string comment = GetIntrinsicName(intrinsic);
    GetCurBB()->AppendInsn(CreateCommentInsn(comment));
  }

  /*
   * At this moment, we eagerly evaluates all argument expressions.  In theory,
   * there could be intrinsics that extract meta-information of variables, such as
   * their locations, rather than computing their values.  Applications
   * include building stack maps that help runtime libraries to find the values
   * of local variables (See @stackmap in LLVM), in which case knowing their
   * locations will suffice.
   */
  if (intrinsic == INTRN_MPL_CLINIT_CHECK) {  /* special case */
    SelectMPLClinitCheck(intrinsiccallNode);
    return;
  }
  if (intrinsic == INTRN_MPL_PROF_COUNTER_INC) {  /* special case */
    SelectMPLProfCounterInc(intrinsiccallNode);
    return;
  }
  if ((intrinsic == INTRN_MPL_CLEANUP_LOCALREFVARS) || (intrinsic == INTRN_MPL_CLEANUP_LOCALREFVARS_SKIP) ||
      (intrinsic == INTRN_MPL_CLEANUP_NORETESCOBJS)) {
    return;
  }
  std::vector<Operand*> operands;  /* Temporary.  Deallocated on return. */
  AArch64ListOperand *srcOpnds = memPool->New<AArch64ListOperand>(*GetFuncScopeAllocator());
  for (size_t i = 0; i < intrinsiccallNode.NumOpnds(); i++) {
    BaseNode *argExpr = intrinsiccallNode.Opnd(i);
    Operand *opnd = HandleExpr(intrinsiccallNode, *argExpr);
    operands.push_back(opnd);
    if (!opnd->IsRegister()) {
      opnd = &LoadIntoRegister(*opnd, argExpr->GetPrimType());
    }
    RegOperand *expRegOpnd = static_cast<RegOperand*>(opnd);
    srcOpnds->PushOpnd(*expRegOpnd);
  }
  CallReturnVector *retVals = &intrinsiccallNode.GetReturnVec();

  switch (intrinsic) {
    case INTRN_MPL_ATOMIC_EXCHANGE_PTR: {
      BB *origFtBB = GetCurBB()->GetNext();
      Operand *loc = operands[kInsnFirstOpnd];
      Operand *newVal = operands[kInsnSecondOpnd];
      Operand *memOrd = operands[kInsnThirdOpnd];

      MemOrd ord = OperandToMemOrd(*memOrd);
      bool isAcquire = MemOrdIsAcquire(ord);
      bool isRelease = MemOrdIsRelease(ord);

      const PrimType kValPrimType = PTY_a64;

      RegOperand &locReg = LoadIntoRegister(*loc, PTY_a64);
      /* Because there is no live analysis when -O1 */
      if (Globals::GetInstance()->GetOptimLevel() == 0) {
        locReg.SetRegNotBBLocal();
      }
      AArch64MemOperand &locMem = GetOrCreateMemOpnd(AArch64MemOperand::kAddrModeBOi,
          k64BitSize, &locReg, nullptr, &GetOrCreateOfstOpnd(0, k32BitSize), nullptr);
      RegOperand &newValReg = LoadIntoRegister(*newVal, PTY_a64);
      if (Globals::GetInstance()->GetOptimLevel() == 0) {
        newValReg.SetRegNotBBLocal();
      }
      GetCurBB()->SetKind(BB::kBBFallthru);

      LabelIdx retryLabIdx = CreateLabeledBB(intrinsiccallNode);

      RegOperand *oldVal = SelectLoadExcl(kValPrimType, locMem, isAcquire);
      if (Globals::GetInstance()->GetOptimLevel() == 0) {
        oldVal->SetRegNotBBLocal();
      }
      RegOperand *succ = SelectStoreExcl(kValPrimType, locMem, newValReg, isRelease);
      if (Globals::GetInstance()->GetOptimLevel() == 0) {
        succ->SetRegNotBBLocal();
      }

      GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(MOP_wcbnz, *succ,
                                                                    GetOrCreateLabelOperand(retryLabIdx)));
      GetCurBB()->SetKind(BB::kBBIntrinsic);
      GetCurBB()->SetNext(origFtBB);

      SaveReturnValueInLocal(*retVals, 0, kValPrimType, *oldVal, intrinsiccallNode);
      break;
    }
    case INTRN_GET_AND_ADDI: {
      IntrinsifyGetAndAddInt(*srcOpnds, PTY_i32);
      break;
    }
    case INTRN_GET_AND_ADDL: {
      IntrinsifyGetAndAddInt(*srcOpnds, PTY_i64);
      break;
    }
    case INTRN_GET_AND_SETI: {
      IntrinsifyGetAndSetInt(*srcOpnds, PTY_i32);
      break;
    }
    case INTRN_GET_AND_SETL: {
      IntrinsifyGetAndSetInt(*srcOpnds, PTY_i64);
      break;
    }
    case INTRN_COMP_AND_SWAPI: {
      IntrinsifyCompareAndSwapInt(*srcOpnds, PTY_i32);
      break;
    }
    case INTRN_COMP_AND_SWAPL: {
      IntrinsifyCompareAndSwapInt(*srcOpnds, PTY_i64);
      break;
    }
    default: {
      CHECK_FATAL(false, "Intrinsic %d: %s not implemented by the AArch64 CG.", intrinsic, GetIntrinsicName(intrinsic));
      break;
    }
  }
}

/*
 * NOTE: consider moving the following things into aarch64_cg.cpp  They may
 * serve not only inrinsics, but other MapleIR instructions as well.
 * Do it as if we are adding a label in straight-line assembly code.
 */
LabelIdx AArch64CGFunc::CreateLabeledBB(StmtNode &stmt) {
  LabelIdx labIdx = CreateLabel();
  BB *newBB = StartNewBBImpl(false, stmt);
  newBB->AddLabel(labIdx);
  SetLab2BBMap(labIdx, *newBB);
  SetCurBB(*newBB);
  return labIdx;
}

/* Save value into the local variable for the index-th return value; */
void AArch64CGFunc::SaveReturnValueInLocal(CallReturnVector &retVals, size_t index, PrimType primType, Operand &value,
                                           StmtNode &parentStmt) {
  CallReturnPair &pair = retVals.at(index);
  BB tempBB(static_cast<uint32>(-1), *GetFuncScopeAllocator());
  BB *realCurBB = GetCurBB();
  SetCurBB(tempBB);
  CHECK_FATAL(!pair.second.IsReg(), "NYI");
  SelectDassign(pair.first, pair.second.GetFieldID(), primType, value);
  CHECK_FATAL(realCurBB->GetNext() == nullptr, "current BB must has not nextBB");
  realCurBB->SetLastStmt(parentStmt);
  realCurBB->SetNext(StartNewBBImpl(true, parentStmt));
  realCurBB->GetNext()->SetKind(BB::kBBFallthru);
  realCurBB->GetNext()->SetPrev(realCurBB);

  realCurBB->GetNext()->InsertAtBeginning(*GetCurBB());
  /* restore it */
  SetCurBB(*realCurBB->GetNext());
}

/* The following are translation of LL/SC and atomic RMW operations */
MemOrd AArch64CGFunc::OperandToMemOrd(Operand &opnd) {
  CHECK_FATAL(opnd.IsImmediate(), "Memory order must be an int constant.");
  auto immOpnd = static_cast<ImmOperand*>(&opnd);
  int32 val = immOpnd->GetValue();
  CHECK_FATAL(val >= 0, "val must be non-negtive");
  return MemOrdFromU32(static_cast<uint32>(val));
}

/*
 * Generate ldxr or ldaxr instruction.
 * byte_p2x: power-of-2 size of operand in bytes (0: 1B, 1: 2B, 2: 4B, 3: 8B).
 */
MOperator AArch64CGFunc::PickLoadStoreExclInsn(uint32 byteP2Size, bool store, bool acqRel) const {
  CHECK_FATAL(byteP2Size < kIntByteSizeDimension, "Illegal argument p2size: %d", byteP2Size);

  static MOperator operators[4][2][2] = { { { MOP_wldxrb, MOP_wldaxrb }, { MOP_wstxrb, MOP_wstlxrb } },
                                          { { MOP_wldxrh, MOP_wldaxrh }, { MOP_wstxrh, MOP_wstlxrh } },
                                          { { MOP_wldxr, MOP_wldaxr }, { MOP_wstxr, MOP_wstlxr } },
                                          { { MOP_xldxr, MOP_xldaxr }, { MOP_xstxr, MOP_xstlxr } } };

  MOperator optr = operators[byteP2Size][store][acqRel];
  CHECK_FATAL(optr != MOP_undef, "Unsupported type p2size: %d", byteP2Size);

  return optr;
}

RegOperand *AArch64CGFunc::SelectLoadExcl(PrimType valPrimType, AArch64MemOperand &loc, bool acquire) {
  uint32 p2size = GetPrimTypeP2Size(valPrimType);

  RegOperand &result = CreateRegisterOperandOfType(valPrimType);
  MOperator mOp = PickLoadStoreExclInsn(p2size, false, acquire);
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, result, loc));

  return &result;
}

RegOperand *AArch64CGFunc::SelectStoreExcl(PrimType valPty, AArch64MemOperand &loc, RegOperand &newVal, bool release) {
  uint32 p2size = GetPrimTypeP2Size(valPty);

  /* the result (success/fail) is to be stored in a 32-bit register */
  RegOperand &result = CreateRegisterOperandOfType(PTY_u32);

  MOperator mOp = PickLoadStoreExclInsn(p2size, true, release);
  GetCurBB()->AppendInsn(GetCG()->BuildInstruction<AArch64Insn>(mOp, result, newVal, loc));

  return &result;
}
}  /* namespace maplebe */
