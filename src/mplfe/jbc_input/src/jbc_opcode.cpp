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
#include "jbc_opcode.h"
#include "jbc_class_const.h"
#include "jbc_class_const_pool.h"
#include "jbc_util.h"
#include "fe_type_manager.h"

namespace maple {
namespace jbc {
// ---------- JBCOpcodeInfo ----------
JBCOpcodeInfo::JBCOpcodeInfo() {
#define JBC_OP(mOp, mValue, mKind, mName, mFlag) \
  table[kOp##mOp].kind = kOpKind##mKind; \
  table[kOp##mOp].name = mName; \
  table[kOp##mOp].flags = mFlag;
#include "jbc_opcode.def"
#undef JBC_OP
}

// ---------- JBCOp ----------
JBCOpcodeInfo JBCOp::opcodeInfo;
std::vector<JBCPrimType> JBCOp::emptyPrimTypes;

JBCOp::JBCOp(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn)
    : op(opIn), kind(kindIn), wide(wideIn) {}

bool JBCOp::CheckNotWide(const BasicIORead &io) const {
  // wide only can be used with i/f/l/d/aload, i/f/l/d/astore, ret, and iinc
  if (wide) {
    ERR(kLncErr, "opcode %s @ pc=%u can not own wide prefix", GetOpcodeName().c_str(), io.GetPos() - 1);
    return false;
  }
  return true;
}

std::string JBCOp::DumpImpl(const JBCConstPool &constPool) const {
  return GetOpcodeName();
}

const std::vector<JBCPrimType> &JBCOp::GetInputTypesFromStackImpl() const {
  return emptyPrimTypes;
}

std::vector<JBCPrimType> JBCOp::GetInputTypesFromStackImpl(const JBCConstPool &constPool) const {
  return GetInputTypesFromStackImpl();
}

JBCPrimType JBCOp::GetOutputTypesToStackImpl() const {
  return kTypeDefault;
}

JBCPrimType JBCOp::GetOutputTypesToStackImpl(const JBCConstPool &constPool) const {
  return GetOutputTypesToStackImpl();
}

// ---------- JBCOpUnused ----------
JBCOpUnused::JBCOpUnused(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn)
    : JBCOp(allocator, opIn, kindIn, wideIn) {}

bool JBCOpUnused::ParseFileImpl(BasicIORead &io) {
  WARN(kLncWarn, "Unused opcode %s", GetOpcodeName().c_str());
  return true;
}

// ---------- JBCOpReversed ----------
JBCOpReversed::JBCOpReversed(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn)
    : JBCOp(allocator, opIn, kindIn, wideIn) {}

bool JBCOpReversed::ParseFileImpl(BasicIORead &io) {
  ERR(kLncErr, "Reversed opcode %s", GetOpcodeName().c_str());
  return false;
}

// ---------- JBCOpDefault ----------
std::map<JBCOpcode, std::vector<JBCPrimType>> JBCOpDefault::mapOpInputTypes = JBCOpDefault::InitMapOpInputTypes();
std::map<JBCOpcode, JBCPrimType> JBCOpDefault::mapOpOutputType = JBCOpDefault::InitMapOpOutputType();

JBCOpDefault::JBCOpDefault(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn)
    : JBCOp(allocator, opIn, kindIn, wideIn) {}

bool JBCOpDefault::ParseFileImpl(BasicIORead &io) {
  if (JBCOp::CheckNotWide(io) == false) {
    return false;
  }
  return true;
}

const std::vector<JBCPrimType> &JBCOpDefault::GetInputTypesFromStackImpl() const {
  auto it = mapOpInputTypes.find(op);
  CHECK_FATAL(it != mapOpInputTypes.end(), "Unsupported opcode %s", opcodeInfo.GetOpcodeName(op).c_str());
  return it->second;
}

JBCPrimType JBCOpDefault::GetOutputTypesToStackImpl() const {
  auto it = mapOpOutputType.find(op);
  CHECK_FATAL(it != mapOpOutputType.end(), "Unsupported opcode %s", opcodeInfo.GetOpcodeName(op).c_str());
  return it->second;
}

std::map<JBCOpcode, std::vector<JBCPrimType>> JBCOpDefault::InitMapOpInputTypes() {
  std::map<JBCOpcode, std::vector<JBCPrimType>> ans;
  ans[kOpNop] = emptyPrimTypes;
  InitMapOpInputTypesForArrayLoad(ans);
  InitMapOpInputTypesForArrayStore(ans);
  InitMapOpInputTypesForMathBinop(ans);
  InitMapOpInputTypesForMathUnop(ans);
  InitMapOpInputTypesForConvert(ans);
  InitMapOpInputTypesForCompare(ans);
  InitMapOpInputTypesForReturn(ans);
  InitMapOpInputTypesForThrow(ans);
  InitMapOpInputTypesForMonitor(ans);
  InitMapOpInputTypesForArrayLength(ans);
  return ans;
}

std::map<JBCOpcode, JBCPrimType> JBCOpDefault::InitMapOpOutputType() {
  std::map<JBCOpcode, JBCPrimType> ans;
  ans[kOpNop] = kTypeDefault;
  InitMapOpOutputTypesForArrayLoad(ans);
  InitMapOpOutputTypesForArrayStore(ans);
  InitMapOpOutputTypesForMathBinop(ans);
  InitMapOpOutputTypesForMathUnop(ans);
  InitMapOpOutputTypesForConvert(ans);
  InitMapOpOutputTypesForCompare(ans);
  InitMapOpOutputTypesForReturn(ans);
  InitMapOpOutputTypesForThrow(ans);
  InitMapOpOutputTypesForMonitor(ans);
  InitMapOpOutputTypesForArrayLength(ans);
  return ans;
}

void JBCOpDefault::InitMapOpInputTypesForArrayLoad(std::map<JBCOpcode, std::vector<JBCPrimType>> &ans) {
  ans[kOpIALoad] = std::vector<JBCPrimType>({ kTypeRef, kTypeInt });
  ans[kOpLALoad] = std::vector<JBCPrimType>({ kTypeRef, kTypeInt });
  ans[kOpFALoad] = std::vector<JBCPrimType>({ kTypeRef, kTypeInt });
  ans[kOpDALoad] = std::vector<JBCPrimType>({ kTypeRef, kTypeInt });
  ans[kOpAALoad] = std::vector<JBCPrimType>({ kTypeRef, kTypeInt });
  ans[kOpBALoad] = std::vector<JBCPrimType>({ kTypeRef, kTypeInt });
  ans[kOpCALoad] = std::vector<JBCPrimType>({ kTypeRef, kTypeInt });
  ans[kOpSALoad] = std::vector<JBCPrimType>({ kTypeRef, kTypeInt });
}

void JBCOpDefault::InitMapOpOutputTypesForArrayLoad(std::map<JBCOpcode, JBCPrimType> &ans) {
  ans[kOpIALoad] = kTypeInt;
  ans[kOpLALoad] = kTypeLong;
  ans[kOpFALoad] = kTypeFloat;
  ans[kOpDALoad] = kTypeDouble;
  ans[kOpAALoad] = kTypeRef;
  ans[kOpBALoad] = kTypeByteOrBoolean;
  ans[kOpCALoad] = kTypeChar;
  ans[kOpSALoad] = kTypeShort;
}

void JBCOpDefault::InitMapOpInputTypesForArrayStore(std::map<JBCOpcode, std::vector<JBCPrimType>> &ans) {
  ans[kOpIAStore] = std::vector<JBCPrimType>({ kTypeRef, kTypeInt, kTypeInt });
  ans[kOpLAStore] = std::vector<JBCPrimType>({ kTypeRef, kTypeInt, kTypeLong });
  ans[kOpFAStore] = std::vector<JBCPrimType>({ kTypeRef, kTypeInt, kTypeFloat });
  ans[kOpDAStore] = std::vector<JBCPrimType>({ kTypeRef, kTypeInt, kTypeDouble });
  ans[kOpAAStore] = std::vector<JBCPrimType>({ kTypeRef, kTypeInt, kTypeRef });
  ans[kOpBAStore] = std::vector<JBCPrimType>({ kTypeRef, kTypeInt, kTypeByteOrBoolean });
  ans[kOpCAStore] = std::vector<JBCPrimType>({ kTypeRef, kTypeInt, kTypeChar });
  ans[kOpSAStore] = std::vector<JBCPrimType>({ kTypeRef, kTypeInt, kTypeShort });
}

void JBCOpDefault::InitMapOpOutputTypesForArrayStore(std::map<JBCOpcode, JBCPrimType> &ans) {
  ans[kOpIAStore] = kTypeDefault;
  ans[kOpLAStore] = kTypeDefault;
  ans[kOpFAStore] = kTypeDefault;
  ans[kOpDAStore] = kTypeDefault;
  ans[kOpAAStore] = kTypeDefault;
  ans[kOpBAStore] = kTypeDefault;
  ans[kOpCAStore] = kTypeDefault;
  ans[kOpSAStore] = kTypeDefault;
}

void JBCOpDefault::InitMapOpInputTypesForMathBinop(std::map<JBCOpcode, std::vector<JBCPrimType>> &ans) {
  ans[kOpIAdd] = std::vector<JBCPrimType>({ kTypeInt, kTypeInt });
  ans[kOpLAdd] = std::vector<JBCPrimType>({ kTypeLong, kTypeLong });
  ans[kOpFAdd] = std::vector<JBCPrimType>({ kTypeFloat, kTypeFloat });
  ans[kOpDAdd] = std::vector<JBCPrimType>({ kTypeDouble, kTypeDouble });
  ans[kOpISub] = std::vector<JBCPrimType>({ kTypeInt, kTypeInt });
  ans[kOpLSub] = std::vector<JBCPrimType>({ kTypeLong, kTypeLong });
  ans[kOpFSub] = std::vector<JBCPrimType>({ kTypeFloat, kTypeFloat });
  ans[kOpDSub] = std::vector<JBCPrimType>({ kTypeDouble, kTypeDouble });
  ans[kOpIMul] = std::vector<JBCPrimType>({ kTypeInt, kTypeInt });
  ans[kOpLMul] = std::vector<JBCPrimType>({ kTypeLong, kTypeLong });
  ans[kOpFMul] = std::vector<JBCPrimType>({ kTypeFloat, kTypeFloat });
  ans[kOpDMul] = std::vector<JBCPrimType>({ kTypeDouble, kTypeDouble });
  ans[kOpIDiv] = std::vector<JBCPrimType>({ kTypeInt, kTypeInt });
  ans[kOpLDiv] = std::vector<JBCPrimType>({ kTypeLong, kTypeLong });
  ans[kOpFDiv] = std::vector<JBCPrimType>({ kTypeFloat, kTypeFloat });
  ans[kOpDDiv] = std::vector<JBCPrimType>({ kTypeDouble, kTypeDouble });
  ans[kOpIRem] = std::vector<JBCPrimType>({ kTypeInt, kTypeInt });
  ans[kOpLRem] = std::vector<JBCPrimType>({ kTypeLong, kTypeLong });
  ans[kOpFRem] = std::vector<JBCPrimType>({ kTypeFloat, kTypeFloat });
  ans[kOpDRem] = std::vector<JBCPrimType>({ kTypeDouble, kTypeDouble });
  ans[kOpIShl] = std::vector<JBCPrimType>({ kTypeInt, kTypeInt });
  ans[kOpLShl] = std::vector<JBCPrimType>({ kTypeLong, kTypeInt });
  ans[kOpIShr] = std::vector<JBCPrimType>({ kTypeInt, kTypeInt });
  ans[kOpLShr] = std::vector<JBCPrimType>({ kTypeLong, kTypeInt });
  ans[kOpIUShr] = std::vector<JBCPrimType>({ kTypeInt, kTypeInt });
  ans[kOpLUShr] = std::vector<JBCPrimType>({ kTypeLong, kTypeInt });
  ans[kOpIAnd] = std::vector<JBCPrimType>({ kTypeInt, kTypeInt });
  ans[kOpLAnd] = std::vector<JBCPrimType>({ kTypeLong, kTypeLong });
  ans[kOpIOr] = std::vector<JBCPrimType>({ kTypeInt, kTypeInt });
  ans[kOpLOr] = std::vector<JBCPrimType>({ kTypeLong, kTypeLong });
  ans[kOpIXor] = std::vector<JBCPrimType>({ kTypeInt, kTypeInt });
  ans[kOpLXor] = std::vector<JBCPrimType>({ kTypeLong, kTypeLong });
}

void JBCOpDefault::InitMapOpOutputTypesForMathBinop(std::map<JBCOpcode, JBCPrimType> &ans) {
  ans[kOpIAdd] = kTypeInt;
  ans[kOpLAdd] = kTypeLong;
  ans[kOpFAdd] = kTypeFloat;
  ans[kOpDAdd] = kTypeDouble;
  ans[kOpISub] = kTypeInt;
  ans[kOpLSub] = kTypeLong;
  ans[kOpFSub] = kTypeFloat;
  ans[kOpDSub] = kTypeDouble;
  ans[kOpIMul] = kTypeInt;
  ans[kOpLMul] = kTypeLong;
  ans[kOpFMul] = kTypeFloat;
  ans[kOpDMul] = kTypeDouble;
  ans[kOpIDiv] = kTypeInt;
  ans[kOpLDiv] = kTypeLong;
  ans[kOpFDiv] = kTypeFloat;
  ans[kOpDDiv] = kTypeDouble;
  ans[kOpIRem] = kTypeInt;
  ans[kOpLRem] = kTypeLong;
  ans[kOpFRem] = kTypeFloat;
  ans[kOpDRem] = kTypeDouble;
  ans[kOpIShl] = kTypeInt;
  ans[kOpLShl] = kTypeLong;
  ans[kOpIShr] = kTypeInt;
  ans[kOpLShr] = kTypeLong;
  ans[kOpIUShr] = kTypeInt;
  ans[kOpLUShr] = kTypeLong;
  ans[kOpIAnd] = kTypeInt;
  ans[kOpLAnd] = kTypeLong;
  ans[kOpIOr] = kTypeInt;
  ans[kOpLOr] = kTypeLong;
  ans[kOpIXor] = kTypeInt;
  ans[kOpLXor] = kTypeLong;
}

void JBCOpDefault::InitMapOpInputTypesForMathUnop(std::map<JBCOpcode, std::vector<JBCPrimType>> &ans) {
  ans[kOpINeg] = std::vector<JBCPrimType>({ kTypeInt });
  ans[kOpLNeg] = std::vector<JBCPrimType>({ kTypeLong });
  ans[kOpFNeg] = std::vector<JBCPrimType>({ kTypeFloat });
  ans[kOpDNeg] = std::vector<JBCPrimType>({ kTypeDouble });
}

void JBCOpDefault::InitMapOpOutputTypesForMathUnop(std::map<JBCOpcode, JBCPrimType> &ans) {
  ans[kOpINeg] = kTypeInt;
  ans[kOpLNeg] = kTypeLong;
  ans[kOpFNeg] = kTypeFloat;
  ans[kOpDNeg] = kTypeDouble;
}

void JBCOpDefault::InitMapOpInputTypesForConvert(std::map<JBCOpcode, std::vector<JBCPrimType>> &ans) {
  ans[kOpI2L] = std::vector<JBCPrimType>({ kTypeInt });
  ans[kOpI2F] = std::vector<JBCPrimType>({ kTypeInt });
  ans[kOpI2D] = std::vector<JBCPrimType>({ kTypeInt });
  ans[kOpL2I] = std::vector<JBCPrimType>({ kTypeLong });
  ans[kOpL2F] = std::vector<JBCPrimType>({ kTypeLong });
  ans[kOpL2D] = std::vector<JBCPrimType>({ kTypeLong });
  ans[kOpF2I] = std::vector<JBCPrimType>({ kTypeFloat });
  ans[kOpF2L] = std::vector<JBCPrimType>({ kTypeFloat });
  ans[kOpF2D] = std::vector<JBCPrimType>({ kTypeFloat });
  ans[kOpD2I] = std::vector<JBCPrimType>({ kTypeDouble });
  ans[kOpD2L] = std::vector<JBCPrimType>({ kTypeDouble });
  ans[kOpD2F] = std::vector<JBCPrimType>({ kTypeDouble });
  ans[kOpI2B] = std::vector<JBCPrimType>({ kTypeInt });
  ans[kOpI2C] = std::vector<JBCPrimType>({ kTypeInt });
  ans[kOpI2S] = std::vector<JBCPrimType>({ kTypeInt });
}

void JBCOpDefault::InitMapOpOutputTypesForConvert(std::map<JBCOpcode, JBCPrimType> &ans) {
  ans[kOpI2L] = kTypeLong;
  ans[kOpI2F] = kTypeFloat;
  ans[kOpI2D] = kTypeDouble;
  ans[kOpL2I] = kTypeInt;
  ans[kOpL2F] = kTypeFloat;
  ans[kOpL2D] = kTypeDouble;
  ans[kOpF2I] = kTypeInt;
  ans[kOpF2L] = kTypeLong;
  ans[kOpF2D] = kTypeDouble;
  ans[kOpD2I] = kTypeInt;
  ans[kOpD2L] = kTypeLong;
  ans[kOpD2F] = kTypeFloat;
  ans[kOpI2B] = kTypeByteOrBoolean;
  ans[kOpI2C] = kTypeChar;
  ans[kOpI2S] = kTypeShort;
}

void JBCOpDefault::InitMapOpInputTypesForCompare(std::map<JBCOpcode, std::vector<JBCPrimType>> &ans) {
  ans[kOpLCmp] = std::vector<JBCPrimType>({ kTypeLong, kTypeLong });
  ans[kOpFCmpl] = std::vector<JBCPrimType>({ kTypeFloat, kTypeFloat });
  ans[kOpFCmpg] = std::vector<JBCPrimType>({ kTypeFloat, kTypeFloat });
  ans[kOpDCmpl] = std::vector<JBCPrimType>({ kTypeDouble, kTypeDouble });
  ans[kOpDCmpg] = std::vector<JBCPrimType>({ kTypeDouble, kTypeDouble });
}

void JBCOpDefault::InitMapOpOutputTypesForCompare(std::map<JBCOpcode, JBCPrimType> &ans) {
  ans[kOpLCmp] = kTypeInt;
  ans[kOpFCmpl] = kTypeInt;
  ans[kOpFCmpg] = kTypeInt;
  ans[kOpDCmpl] = kTypeInt;
  ans[kOpDCmpg] = kTypeInt;
}

void JBCOpDefault::InitMapOpInputTypesForReturn(std::map<JBCOpcode, std::vector<JBCPrimType>> &ans) {
  ans[kOpIReturn] = std::vector<JBCPrimType>({ kTypeInt });
  ans[kOpLReturn] = std::vector<JBCPrimType>({ kTypeLong });
  ans[kOpFReturn] = std::vector<JBCPrimType>({ kTypeFloat });
  ans[kOpDReturn] = std::vector<JBCPrimType>({ kTypeDouble });
  ans[kOpAReturn] = std::vector<JBCPrimType>({ kTypeRef });
  ans[kOpReturn] = emptyPrimTypes;
}

void JBCOpDefault::InitMapOpOutputTypesForReturn(std::map<JBCOpcode, JBCPrimType> &ans) {
  ans[kOpIReturn] = kTypeDefault;
  ans[kOpLReturn] = kTypeDefault;
  ans[kOpFReturn] = kTypeDefault;
  ans[kOpDReturn] = kTypeDefault;
  ans[kOpAReturn] = kTypeDefault;
  ans[kOpReturn] = kTypeDefault;
}

void JBCOpDefault::InitMapOpInputTypesForThrow(std::map<JBCOpcode, std::vector<JBCPrimType>> &ans) {
  ans[kOpAThrow] = std::vector<JBCPrimType>({ kTypeRef });
}

void JBCOpDefault::InitMapOpOutputTypesForThrow(std::map<JBCOpcode, JBCPrimType> &ans) {
  ans[kOpAThrow] = kTypeDefault;
}

void JBCOpDefault::InitMapOpInputTypesForMonitor(std::map<JBCOpcode, std::vector<JBCPrimType>> &ans) {
  ans[kOpMonitorEnter] = std::vector<JBCPrimType>({ kTypeRef });
  ans[kOpMonitorExit] = std::vector<JBCPrimType>({ kTypeRef });
}

void JBCOpDefault::InitMapOpOutputTypesForMonitor(std::map<JBCOpcode, JBCPrimType> &ans) {
  ans[kOpMonitorEnter] = kTypeDefault;
  ans[kOpMonitorExit] = kTypeDefault;
}

void JBCOpDefault::InitMapOpInputTypesForArrayLength(std::map<JBCOpcode, std::vector<JBCPrimType>> &ans) {
  ans[kOpArrayLength] = std::vector<JBCPrimType>({ kTypeRef });
}

void JBCOpDefault::InitMapOpOutputTypesForArrayLength(std::map<JBCOpcode, JBCPrimType> &ans) {
  ans[kOpArrayLength] = kTypeInt;
}

// ---------- JBCOpConst ----------
std::map<JBCOpcode, int32> JBCOpConst::valueMapI = JBCOpConst::InitValueMapI();
std::map<JBCOpcode, int64> JBCOpConst::valueMapJ = JBCOpConst::InitValueMapJ();
std::map<JBCOpcode, float> JBCOpConst::valueMapF = JBCOpConst::InitValueMapF();
std::map<JBCOpcode, double> JBCOpConst::valueMapD = JBCOpConst::InitValueMapD();

JBCOpConst::JBCOpConst(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn)
    : JBCOp(allocator, opIn, kindIn, wideIn) {
  u.raw = 0;
}

bool JBCOpConst::ParseFileImpl(BasicIORead &io) {
  bool success = false;
  if (CheckNotWide(io) == false) {
    return false;
  }
  switch (op) {
    case jbc::kOpBiPush:
      u.bvalue = io.ReadInt8(success);
      break;
    case jbc::kOpSiPush:
      u.svalue = io.ReadInt16(success);
      break;
    case jbc::kOpLdc:
      u.index = io.ReadUInt8(success);
      break;
    case jbc::kOpLdcW:
    case jbc::kOpLdc2W:
      u.index = io.ReadUInt16(success);
      break;
    default:
      success = true;
      break;
  }
  return success;
}

JBCPrimType JBCOpConst::GetOutputTypesToStackImpl(const JBCConstPool &constPool) const {
  switch (op) {
    case jbc::kOpAConstNull:
      return kTypeRef;
    case jbc::kOpIConstM1:
    case jbc::kOpIConst0:
    case jbc::kOpIConst1:
    case jbc::kOpIConst2:
    case jbc::kOpIConst3:
    case jbc::kOpIConst4:
    case jbc::kOpIConst5:
      return kTypeInt;
    case jbc::kOpLConst0:
    case jbc::kOpLConst1:
      return kTypeLong;
    case jbc::kOpFConst0:
    case jbc::kOpFConst1:
    case jbc::kOpFConst2:
      return kTypeFloat;
    case jbc::kOpDConst0:
    case jbc::kOpDConst1:
      return kTypeDouble;
    case jbc::kOpBiPush:
      return kTypeByteOrBoolean;
    case jbc::kOpSiPush:
      return kTypeShort;
    case jbc::kOpLdc:
    case jbc::kOpLdcW:
    case jbc::kOpLdc2W: {
      const JBCConst *constRaw = constPool.GetConstByIdx(GetIndex());
      if (constRaw != nullptr) {
        switch (constRaw->GetTag()) {
          case kConstInteger:
            return jbc::kTypeInt;
          case kConstFloat:
            return jbc::kTypeFloat;
          case kConstLong:
            return jbc::kTypeLong;
          case kConstDouble:
            return jbc::kTypeDouble;
          case kConstClass:
          case kConstString:
            return jbc::kTypeRef;
          default:
            CHECK_FATAL(false, "Unsupported const tag %d", constRaw->GetTag());
        }
      }
      break;
    }
    default:
      CHECK_FATAL(false, "Unsupported opcode %s", opcodeInfo.GetOpcodeName(op).c_str());
  }
  return kTypeDefault;
}

std::string JBCOpConst::DumpImpl(const JBCConstPool &constPool) const {
  switch (op) {
    case jbc::kOpBiPush:
      return DumpBiPush();
    case jbc::kOpSiPush:
      return DumpSiPush();
    case jbc::kOpLdc:
    case jbc::kOpLdcW:
    case jbc::kOpLdc2W:
      return DumpLdc(constPool);
    default:
      return JBCOp::DumpImpl(constPool);
  }
}

std::string JBCOpConst::DumpBiPush() const {
  std::stringstream ss;
  ss << GetOpcodeName() << " byte " << static_cast<int32>(u.bvalue);
  return ss.str();
}

std::string JBCOpConst::DumpSiPush() const {
  std::stringstream ss;
  ss << GetOpcodeName() << " short " << static_cast<int32>(u.svalue);
  return ss.str();
}

std::string JBCOpConst::DumpLdc(const JBCConstPool &constPool) const {
  std::stringstream ss;
  ss << GetOpcodeName() << " ";
  const JBCConst *constRaw = constPool.GetConstByIdx(GetIndex());
  if (constRaw != nullptr) {
    switch (constRaw->GetTag()) {
      case kConstInteger:
      case kConstFloat: {
        const JBCConst4Byte *const4B = static_cast<const JBCConst4Byte*>(constRaw);
        if (constRaw->GetTag() == kConstInteger) {
          ss << "int " << const4B->GetInt32();
        } else {
          ss << "float " << const4B->GetFloat();
        }
        break;
      }
      case kConstLong:
      case kConstDouble: {
        const JBCConst8Byte *const8B = static_cast<const JBCConst8Byte*>(constRaw);
        if (constRaw->GetTag() == kConstLong) {
          ss << "long " << const8B->GetInt64();
        } else {
          ss << "double " << const8B->GetDouble();
        }
        break;
      }
      case kConstClass: {
        const JBCConstClass *constClass = static_cast<const JBCConstClass*>(constRaw);
        ss << "Class " << constClass->GetClassNameOrin();
        break;
      }
      case kConstString: {
        const JBCConstString *constString = static_cast<const JBCConstString*>(constRaw);
        ss << "String " << "\"" << constString->GetString() << "\"";
        break;
      }
      case kConstMethodType: {
        ss << "MethodType ";
        break;
      }
      case kConstMethodHandleInfo: {
        ss << "MethodHandle ";
        break;
      }
      default:
        ss << "Unsupported const tag " << constRaw->GetTag();
        break;
    }
  } else {
    ss << "invalid const index";
  }
  return ss.str();
}

int32 JBCOpConst::GetValueInt() const {
  auto it = valueMapI.find(op);
  CHECK_FATAL(it != valueMapI.end(), "unsupport opcode %s", opcodeInfo.GetOpcodeName(op).c_str());
  return it->second;
}

int64 JBCOpConst::GetValueLong() const {
  auto it = valueMapJ.find(op);
  CHECK_FATAL(it != valueMapJ.end(), "unsupport opcode %s", opcodeInfo.GetOpcodeName(op).c_str());
  return it->second;
}

float JBCOpConst::GetValueFloat() const {
  auto it = valueMapF.find(op);
  CHECK_FATAL(it != valueMapF.end(), "unsupport opcode %s", opcodeInfo.GetOpcodeName(op).c_str());
  return it->second;
}

double JBCOpConst::GetValueDouble() const {
  auto it = valueMapD.find(op);
  CHECK_FATAL(it != valueMapD.end(), "unsupport opcode %s", opcodeInfo.GetOpcodeName(op).c_str());
  return it->second;
}

int8 JBCOpConst::GetValueByte() const {
  CHECK_FATAL(op == kOpBiPush, "unsupport opcode %s", opcodeInfo.GetOpcodeName(op).c_str());
  return u.bvalue;
}

int16 JBCOpConst::GetValueShort() const {
  CHECK_FATAL(op == kOpSiPush, "unsupport opcode %s", opcodeInfo.GetOpcodeName(op).c_str());
  return u.svalue;
}

std::map<JBCOpcode, int32> JBCOpConst::InitValueMapI() {
  std::map<JBCOpcode, int32> valueMap;
  valueMap[jbc::kOpIConstM1] = -1;
  valueMap[jbc::kOpIConst0] = 0;
  valueMap[jbc::kOpIConst1] = 1;
  valueMap[jbc::kOpIConst2] = 2;
  valueMap[jbc::kOpIConst3] = 3;
  valueMap[jbc::kOpIConst4] = 4;
  valueMap[jbc::kOpIConst5] = 5;
  return valueMap;
}

std::map<JBCOpcode, int64> JBCOpConst::InitValueMapJ() {
  std::map<JBCOpcode, int64> valueMap;
  valueMap[jbc::kOpLConst0] = 0L;
  valueMap[jbc::kOpLConst1] = 1L;
  return valueMap;
}

std::map<JBCOpcode, float> JBCOpConst::InitValueMapF() {
  std::map<JBCOpcode, float> valueMap;
  valueMap[jbc::kOpFConst0] = 0.0F;
  valueMap[jbc::kOpFConst1] = 1.0F;
  valueMap[jbc::kOpFConst2] = 2.0F;
  return valueMap;
}

std::map<JBCOpcode, double> JBCOpConst::InitValueMapD() {
  std::map<JBCOpcode, double> valueMap;
  valueMap[jbc::kOpDConst0] = 0.0;
  valueMap[jbc::kOpDConst1] = 1.0;
  return valueMap;
}

// ---------- JBCOpSlotOpr ----------
std::map<JBCOpcode, std::pair<uint16, JBCPrimType>> JBCOpSlotOpr::mapSlotIdxAndType =
    JBCOpSlotOpr::InitMapSlotIdxAndType();
std::map<JBCOpcode, std::vector<JBCPrimType>> JBCOpSlotOpr::mapOpInputTypes = JBCOpSlotOpr::InitMapOpInputTypes();
std::map<JBCOpcode, JBCPrimType> JBCOpSlotOpr::mapOpOutputType = JBCOpSlotOpr::InitMapOpOutputType();

JBCOpSlotOpr::JBCOpSlotOpr(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn)
    : JBCOp(allocator, opIn, kindIn, wideIn), slotIdx(0) {}

bool JBCOpSlotOpr::ParseFileImpl(BasicIORead &io) {
  bool success = false;
  switch (op) {
    case jbc::kOpILoad:
    case jbc::kOpLLoad:
    case jbc::kOpFLoad:
    case jbc::kOpDLoad:
    case jbc::kOpALoad:
    case jbc::kOpIStore:
    case jbc::kOpLStore:
    case jbc::kOpFStore:
    case jbc::kOpDStore:
    case jbc::kOpAStore:
      if (wide) {
        slotIdx = io.ReadUInt16(success);
      } else {
        slotIdx = io.ReadUInt8(success);
      }
      break;
    default:
      std::map<JBCOpcode, std::pair<uint16, JBCPrimType>>::const_iterator it = mapSlotIdxAndType.find(op);
      if (it == mapSlotIdxAndType.end()) {
        ERR(kLncErr, "Unexpected opcode %s for SlotOpr", GetOpcodeName().c_str());
        success = false;
      } else {
        slotIdx = it->second.first;
        success = true;
      }
      break;
  }
  return success;
}

const std::vector<JBCPrimType> &JBCOpSlotOpr::GetInputTypesFromStackImpl() const {
  auto it = mapOpInputTypes.find(op);
  CHECK_FATAL(it != mapOpInputTypes.end(), "Unsupported opcode %s", opcodeInfo.GetOpcodeName(op).c_str());
  return it->second;
}

JBCPrimType JBCOpSlotOpr::GetOutputTypesToStackImpl() const {
  auto it = mapOpOutputType.find(op);
  CHECK_FATAL(it != mapOpOutputType.end(), "Unsupported opcode %s", opcodeInfo.GetOpcodeName(op).c_str());
  return it->second;
}

std::string JBCOpSlotOpr::DumpImpl(const JBCConstPool &constPool) const {
  std::stringstream ss;
  ss << GetOpcodeName() << " " << slotIdx;
  return ss.str();
}

std::map<JBCOpcode, std::pair<uint16, JBCPrimType>> JBCOpSlotOpr::InitMapSlotIdxAndType() {
  std::map<JBCOpcode, std::pair<uint16, JBCPrimType>> result;
  result[kOpILoad] = std::make_pair(0, kTypeInt);
  result[kOpLLoad] = std::make_pair(0, kTypeLong);
  result[kOpFLoad] = std::make_pair(0, kTypeFloat);
  result[kOpDLoad] = std::make_pair(0, kTypeDouble);
  result[kOpALoad] = std::make_pair(0, kTypeRef);
  result[kOpILoad0] = std::make_pair(0, kTypeInt);
  result[kOpILoad1] = std::make_pair(1, kTypeInt);
  result[kOpILoad2] = std::make_pair(2, kTypeInt);
  result[kOpILoad3] = std::make_pair(3, kTypeInt);
  result[kOpLLoad0] = std::make_pair(0, kTypeLong);
  result[kOpLLoad1] = std::make_pair(1, kTypeLong);
  result[kOpLLoad2] = std::make_pair(2, kTypeLong);
  result[kOpLLoad3] = std::make_pair(3, kTypeLong);
  result[kOpFLoad0] = std::make_pair(0, kTypeFloat);
  result[kOpFLoad1] = std::make_pair(1, kTypeFloat);
  result[kOpFLoad2] = std::make_pair(2, kTypeFloat);
  result[kOpFLoad3] = std::make_pair(3, kTypeFloat);
  result[kOpDLoad0] = std::make_pair(0, kTypeDouble);
  result[kOpDLoad1] = std::make_pair(1, kTypeDouble);
  result[kOpDLoad2] = std::make_pair(2, kTypeDouble);
  result[kOpDLoad3] = std::make_pair(3, kTypeDouble);
  result[kOpALoad0] = std::make_pair(0, kTypeRef);
  result[kOpALoad1] = std::make_pair(1, kTypeRef);
  result[kOpALoad2] = std::make_pair(2, kTypeRef);
  result[kOpALoad3] = std::make_pair(3, kTypeRef);
  result[kOpIStore] = std::make_pair(0, kTypeInt);
  result[kOpLStore] = std::make_pair(0, kTypeLong);
  result[kOpFStore] = std::make_pair(0, kTypeFloat);
  result[kOpDStore] = std::make_pair(0, kTypeDouble);
  result[kOpAStore] = std::make_pair(0, kTypeRef);
  result[kOpIStore0] = std::make_pair(0, kTypeInt);
  result[kOpIStore1] = std::make_pair(1, kTypeInt);
  result[kOpIStore2] = std::make_pair(2, kTypeInt);
  result[kOpIStore3] = std::make_pair(3, kTypeInt);
  result[kOpLStore0] = std::make_pair(0, kTypeLong);
  result[kOpLStore1] = std::make_pair(1, kTypeLong);
  result[kOpLStore2] = std::make_pair(2, kTypeLong);
  result[kOpLStore3] = std::make_pair(3, kTypeLong);
  result[kOpFStore0] = std::make_pair(0, kTypeFloat);
  result[kOpFStore1] = std::make_pair(1, kTypeFloat);
  result[kOpFStore2] = std::make_pair(2, kTypeFloat);
  result[kOpFStore3] = std::make_pair(3, kTypeFloat);
  result[kOpDStore0] = std::make_pair(0, kTypeDouble);
  result[kOpDStore1] = std::make_pair(1, kTypeDouble);
  result[kOpDStore2] = std::make_pair(2, kTypeDouble);
  result[kOpDStore3] = std::make_pair(3, kTypeDouble);
  result[kOpAStore0] = std::make_pair(0, kTypeRef);
  result[kOpAStore1] = std::make_pair(1, kTypeRef);
  result[kOpAStore2] = std::make_pair(2, kTypeRef);
  result[kOpAStore3] = std::make_pair(3, kTypeRef);
  return result;
}

std::map<JBCOpcode, std::vector<JBCPrimType>> JBCOpSlotOpr::InitMapOpInputTypes() {
  std::map<JBCOpcode, std::vector<JBCPrimType>> ans;
  ans[kOpILoad] = emptyPrimTypes;
  ans[kOpFLoad] = emptyPrimTypes;
  ans[kOpLLoad] = emptyPrimTypes;
  ans[kOpDLoad] = emptyPrimTypes;
  ans[kOpALoad] = emptyPrimTypes;
  ans[kOpILoad0] = emptyPrimTypes;
  ans[kOpILoad1] = emptyPrimTypes;
  ans[kOpILoad2] = emptyPrimTypes;
  ans[kOpILoad3] = emptyPrimTypes;
  ans[kOpLLoad0] = emptyPrimTypes;
  ans[kOpLLoad1] = emptyPrimTypes;
  ans[kOpLLoad2] = emptyPrimTypes;
  ans[kOpLLoad3] = emptyPrimTypes;
  ans[kOpFLoad0] = emptyPrimTypes;
  ans[kOpFLoad1] = emptyPrimTypes;
  ans[kOpFLoad2] = emptyPrimTypes;
  ans[kOpFLoad3] = emptyPrimTypes;
  ans[kOpDLoad0] = emptyPrimTypes;
  ans[kOpDLoad1] = emptyPrimTypes;
  ans[kOpDLoad2] = emptyPrimTypes;
  ans[kOpDLoad3] = emptyPrimTypes;
  ans[kOpALoad0] = emptyPrimTypes;
  ans[kOpALoad1] = emptyPrimTypes;
  ans[kOpALoad2] = emptyPrimTypes;
  ans[kOpALoad3] = emptyPrimTypes;
  ans[kOpIStore] = std::vector<JBCPrimType>({ kTypeInt });
  ans[kOpLStore] = std::vector<JBCPrimType>({ kTypeLong });
  ans[kOpFStore] = std::vector<JBCPrimType>({ kTypeFloat });
  ans[kOpDStore] = std::vector<JBCPrimType>({ kTypeDouble });
  ans[kOpAStore] = std::vector<JBCPrimType>({ kTypeRef });
  ans[kOpIStore0] = std::vector<JBCPrimType>({ kTypeInt });
  ans[kOpIStore1] = std::vector<JBCPrimType>({ kTypeInt });
  ans[kOpIStore2] = std::vector<JBCPrimType>({ kTypeInt });
  ans[kOpIStore3] = std::vector<JBCPrimType>({ kTypeInt });
  ans[kOpLStore0] = std::vector<JBCPrimType>({ kTypeLong });
  ans[kOpLStore1] = std::vector<JBCPrimType>({ kTypeLong });
  ans[kOpLStore2] = std::vector<JBCPrimType>({ kTypeLong });
  ans[kOpLStore3] = std::vector<JBCPrimType>({ kTypeLong });
  ans[kOpFStore0] = std::vector<JBCPrimType>({ kTypeFloat });
  ans[kOpFStore1] = std::vector<JBCPrimType>({ kTypeFloat });
  ans[kOpFStore2] = std::vector<JBCPrimType>({ kTypeFloat });
  ans[kOpFStore3] = std::vector<JBCPrimType>({ kTypeFloat });
  ans[kOpDStore0] = std::vector<JBCPrimType>({ kTypeDouble });
  ans[kOpDStore1] = std::vector<JBCPrimType>({ kTypeDouble });
  ans[kOpDStore2] = std::vector<JBCPrimType>({ kTypeDouble });
  ans[kOpDStore3] = std::vector<JBCPrimType>({ kTypeDouble });
  ans[kOpAStore0] = std::vector<JBCPrimType>({ kTypeRef });
  ans[kOpAStore1] = std::vector<JBCPrimType>({ kTypeRef });
  ans[kOpAStore2] = std::vector<JBCPrimType>({ kTypeRef });
  ans[kOpAStore3] = std::vector<JBCPrimType>({ kTypeRef });
  return ans;
}

std::map<JBCOpcode, JBCPrimType> JBCOpSlotOpr::InitMapOpOutputType() {
  std::map<JBCOpcode, JBCPrimType> ans;
  ans[kOpILoad] = kTypeInt;
  ans[kOpFLoad] = kTypeFloat;
  ans[kOpLLoad] = kTypeLong;
  ans[kOpDLoad] = kTypeDouble;
  ans[kOpALoad] = kTypeRef;
  ans[kOpILoad0] = kTypeInt;
  ans[kOpILoad1] = kTypeInt;
  ans[kOpILoad2] = kTypeInt;
  ans[kOpILoad3] = kTypeInt;
  ans[kOpLLoad0] = kTypeLong;
  ans[kOpLLoad1] = kTypeLong;
  ans[kOpLLoad2] = kTypeLong;
  ans[kOpLLoad3] = kTypeLong;
  ans[kOpFLoad0] = kTypeFloat;
  ans[kOpFLoad1] = kTypeFloat;
  ans[kOpFLoad2] = kTypeFloat;
  ans[kOpFLoad3] = kTypeFloat;
  ans[kOpDLoad0] = kTypeDouble;
  ans[kOpDLoad1] = kTypeDouble;
  ans[kOpDLoad2] = kTypeDouble;
  ans[kOpDLoad3] = kTypeDouble;
  ans[kOpALoad0] = kTypeRef;
  ans[kOpALoad1] = kTypeRef;
  ans[kOpALoad2] = kTypeRef;
  ans[kOpALoad3] = kTypeRef;
  ans[kOpIStore] = kTypeDefault;
  ans[kOpLStore] = kTypeDefault;
  ans[kOpFStore] = kTypeDefault;
  ans[kOpDStore] = kTypeDefault;
  ans[kOpAStore] = kTypeDefault;
  ans[kOpIStore0] = kTypeDefault;
  ans[kOpIStore1] = kTypeDefault;
  ans[kOpIStore2] = kTypeDefault;
  ans[kOpIStore3] = kTypeDefault;
  ans[kOpLStore0] = kTypeDefault;
  ans[kOpLStore1] = kTypeDefault;
  ans[kOpLStore2] = kTypeDefault;
  ans[kOpLStore3] = kTypeDefault;
  ans[kOpFStore0] = kTypeDefault;
  ans[kOpFStore1] = kTypeDefault;
  ans[kOpFStore2] = kTypeDefault;
  ans[kOpFStore3] = kTypeDefault;
  ans[kOpDStore0] = kTypeDefault;
  ans[kOpDStore1] = kTypeDefault;
  ans[kOpDStore2] = kTypeDefault;
  ans[kOpDStore3] = kTypeDefault;
  ans[kOpAStore0] = kTypeDefault;
  ans[kOpAStore1] = kTypeDefault;
  ans[kOpAStore2] = kTypeDefault;
  ans[kOpAStore3] = kTypeDefault;
  return ans;
}

// ---------- JBCOpMathInc ----------
JBCOpMathInc::JBCOpMathInc(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn)
    : JBCOp(allocator, opIn, kindIn, wideIn), index(0), incr(0) {}

bool JBCOpMathInc::ParseFileImpl(BasicIORead &io) {
  bool success = false;
  if (wide) {
    index = io.ReadUInt16(success);
    incr = io.ReadInt16(success);
  } else {
    index = io.ReadUInt8(success);
    incr = io.ReadInt8(success);
  }
  return success;
}

// ---------- JBCOpBranch ----------
std::map<JBCOpcode, std::vector<JBCPrimType>> JBCOpBranch::mapOpInputTypes = JBCOpBranch::InitMapOpInputTypes();

JBCOpBranch::JBCOpBranch(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn)
    : JBCOp(allocator, opIn, kindIn, wideIn), target(0) {}

bool JBCOpBranch::ParseFileImpl(BasicIORead &io) {
  if (JBCOp::CheckNotWide(io) == false) {
    return false;
  }
  bool success = true;
  int64 pc = io.GetPos() - 1;
  target = GetTargetbyInt64(pc + io.ReadInt16(success));
  return success;
}

std::string JBCOpBranch::DumpImpl(const JBCConstPool &constPool) const {
  std::stringstream ss;
  ss << GetOpcodeName() << " " << target;
  return ss.str();
}

const std::vector<JBCPrimType> &JBCOpBranch::GetInputTypesFromStackImpl() const {
  auto it = mapOpInputTypes.find(op);
  CHECK_FATAL(it != mapOpInputTypes.end(), "Unsupported opcode %s", opcodeInfo.GetOpcodeName(op).c_str());
  return it->second;
}

std::map<JBCOpcode, std::vector<JBCPrimType>> JBCOpBranch::InitMapOpInputTypes() {
  std::map<JBCOpcode, std::vector<JBCPrimType>> ans;
  ans[kOpIfeq] = std::vector<JBCPrimType>({ kTypeInt });
  ans[kOpIfne] = std::vector<JBCPrimType>({ kTypeInt });
  ans[kOpIflt] = std::vector<JBCPrimType>({ kTypeInt });
  ans[kOpIfge] = std::vector<JBCPrimType>({ kTypeInt });
  ans[kOpIfgt] = std::vector<JBCPrimType>({ kTypeInt });
  ans[kOpIfle] = std::vector<JBCPrimType>({ kTypeInt });
  ans[kOpIfeq] = std::vector<JBCPrimType>({ kTypeInt });
  ans[kOpIfICmpeq] = std::vector<JBCPrimType>({ kTypeInt, kTypeInt });
  ans[kOpIfICmpne] = std::vector<JBCPrimType>({ kTypeInt, kTypeInt });
  ans[kOpIfICmplt] = std::vector<JBCPrimType>({ kTypeInt, kTypeInt });
  ans[kOpIfICmpge] = std::vector<JBCPrimType>({ kTypeInt, kTypeInt });
  ans[kOpIfICmpgt] = std::vector<JBCPrimType>({ kTypeInt, kTypeInt });
  ans[kOpIfICmple] = std::vector<JBCPrimType>({ kTypeInt, kTypeInt });
  ans[kOpIfACmpeq] = std::vector<JBCPrimType>({ kTypeRef, kTypeRef });
  ans[kOpIfACmpne] = std::vector<JBCPrimType>({ kTypeRef, kTypeRef });
  ans[kOpIfNull] = std::vector<JBCPrimType>({ kTypeRef });
  ans[kOpIfNonNull] = std::vector<JBCPrimType>({ kTypeRef });
  return ans;
}

// ---------- JBCOpGoto ----------
JBCOpGoto::JBCOpGoto(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn)
    : JBCOp(allocator, opIn, kindIn, wideIn), target(0) {}

bool JBCOpGoto::ParseFileImpl(BasicIORead &io) {
  if (JBCOp::CheckNotWide(io) == false) {
    return false;
  }
  bool success = true;
  int64 pc = io.GetPos() - 1;
  switch (op) {
    case jbc::kOpGoto:
      target = GetTargetbyInt64(pc + io.ReadInt16(success));
      break;
    case jbc::kOpGotoW:
      target = GetTargetbyInt64(pc + io.ReadInt32(success));
      break;
    default:
      CHECK_FATAL(false, "Unexpected opcode %s for Goto", GetOpcodeName().c_str());
      return false;
  }
  return success;
}

std::string JBCOpGoto::DumpImpl(const JBCConstPool &constPool) const {
  std::stringstream ss;
  ss << GetOpcodeName() << " " << target;
  return ss.str();
}

// ---------- JBCOpSwitch ----------
std::map<JBCOpcode, std::vector<JBCPrimType>> JBCOpSwitch::mapOpInputTypes = JBCOpSwitch::InitMapOpInputTypes();

JBCOpSwitch::JBCOpSwitch(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn)
    : JBCOp(allocator, opIn, kindIn, wideIn),
      targets(std::less<int32>(), allocator.Adapter()),
      targetDefault(0)  {}

bool JBCOpSwitch::ParseFileImpl(BasicIORead &io) {
  bool success = false;
  int64 pc = io.GetPos() - 1;
  const uint32 kAlign32 = 4;
  if (op == jbc::kOpTableSwitch) {
    while (io.GetPos() % kAlign32 != 0) {
      (void)io.ReadUInt8(success);
    }
    targetDefault = GetTargetbyInt64(pc + io.ReadUInt32(success));
    int32 low = io.ReadInt32(success);
    int32 high = io.ReadInt32(success);
    for (int32 i = low; i <= high && success; ++i) {
      int32 offset = io.ReadInt32(success);
      CHECK_FATAL(targets.insert(std::make_pair(i, GetTargetbyInt64(offset + pc))).second, "targets insert failed");
    }
  } else if (op == jbc::kOpLookupSwitch) {
    while (io.GetPos() % kAlign32 != 0) {
      (void)io.ReadUInt8(success);
    }
    targetDefault = GetTargetbyInt64(pc + io.ReadUInt32(success));
    uint32 npairs = io.ReadUInt32(success);
    for (uint32 i = 0; i < npairs && success; ++i) {
      int32 value = io.ReadInt32(success);
      int32 offset = io.ReadInt32(success);
      CHECK_FATAL(targets.insert(std::make_pair(value, GetTargetbyInt64(offset + pc))).second, "targets insert failed");
    }
  }
  return success;
}

const std::vector<JBCPrimType> &JBCOpSwitch::GetInputTypesFromStackImpl() const {
  auto it = mapOpInputTypes.find(op);
  CHECK_FATAL(it != mapOpInputTypes.end(), "Unsupported opcode %s", opcodeInfo.GetOpcodeName(op).c_str());
  return it->second;
}

std::string JBCOpSwitch::DumpImpl(const JBCConstPool &constPool) const {
  std::stringstream ss;
  ss << GetOpcodeName() << " { default=" << targetDefault;
  for (auto it : targets) {
    ss << "," << it.first << ":" << it.second;
  }
  ss << " }";
  return ss.str();
}

std::map<JBCOpcode, std::vector<JBCPrimType>> JBCOpSwitch::InitMapOpInputTypes() {
  std::map<JBCOpcode, std::vector<JBCPrimType>> ans;
  ans[kOpTableSwitch] = std::vector<JBCPrimType>({ kTypeInt });
  ans[kOpLookupSwitch] = std::vector<JBCPrimType>({ kTypeInt });
  return ans;
}

// ---------- JBCOpFieldOpr ----------
JBCOpFieldOpr::JBCOpFieldOpr(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn)
    : JBCOp(allocator, opIn, kindIn, wideIn), fieldIdx(0) {}

bool JBCOpFieldOpr::ParseFileImpl(BasicIORead &io) {
  if (JBCOp::CheckNotWide(io) == false) {
    return false;
  }
  bool success = false;
  fieldIdx = io.ReadUInt16(success);
  return success;
}

const std::vector<JBCPrimType> &JBCOpFieldOpr::GetInputTypesFromStackImpl() const {
  CHECK_FATAL(false, "Restricted Calls");
  return emptyPrimTypes;
}

std::vector<JBCPrimType> JBCOpFieldOpr::GetInputTypesFromStackImpl(const JBCConstPool &constPool) const {
  switch (op) {
    case kOpGetField:
    case kOpGetStatic:
      return GetInputTypesFromStackForGet(constPool);
    case kOpPutField:
    case kOpPutStatic:
      return GetInputTypesFromStackForPut(constPool);
    default:
      CHECK_FATAL(false, "Unsupported opcode %s", opcodeInfo.GetOpcodeName(op).c_str());
      return emptyPrimTypes;
  }
}

JBCPrimType JBCOpFieldOpr::GetOutputTypesToStackImpl() const {
  CHECK_FATAL(false, "Restricted Calls");
  return kTypeDefault;
}

JBCPrimType JBCOpFieldOpr::GetOutputTypesToStackImpl(const JBCConstPool &constPool) const {
  switch (op) {
    case kOpGetField:
    case kOpGetStatic: {
      std::string desc = GetFieldType(constPool);
      return JBCUtil::GetPrimTypeForName(desc);
    }
    case kOpPutField:
    case kOpPutStatic:
      return kTypeDefault;
    default:
      CHECK_FATAL(false, "Unsupported opcode %s", opcodeInfo.GetOpcodeName(op).c_str());
      return kTypeDefault;
  }
}

std::string JBCOpFieldOpr::DumpImpl(const JBCConstPool &constPool) const {
  std::stringstream ss;
  ss << GetOpcodeName() << " ";
  const JBCConst *constRaw = constPool.GetConstByIdxWithTag(fieldIdx, JBCConstTag::kConstFieldRef);
  if (constRaw == nullptr) {
    ss << "unknown";
  } else {
    const JBCConstRef *constRef = static_cast<const JBCConstRef*>(constRaw);
    CHECK_NULL_FATAL(constRef->GetConstClass());
    ss << constRef->GetConstClass()->GetClassNameOrin() << "." << constRef->GetName() << ":" <<
          constRef->GetDesc();
  }
  return ss.str();
}

std::vector<JBCPrimType> JBCOpFieldOpr::GetInputTypesFromStackForGet(const JBCConstPool &constPool) const {
  std::vector<JBCPrimType> ans;
  if (op == kOpGetField) {
    ans.push_back(kTypeRef);
  }
  return ans;
}

std::vector<JBCPrimType> JBCOpFieldOpr::GetInputTypesFromStackForPut(const JBCConstPool &constPool) const {
  std::vector<JBCPrimType> ans;
  std::string desc = GetFieldType(constPool);
  if (op == kOpPutField) {
    ans.push_back(kTypeRef);
  }
  ans.push_back(JBCUtil::GetPrimTypeForName(desc));
  return ans;
}

std::string JBCOpFieldOpr::GetFieldType(const JBCConstPool &constPool) const {
  std::string desc = "";
  const JBCConst *constRaw = constPool.GetConstByIdxWithTag(fieldIdx, kConstFieldRef);
  if (constRaw != nullptr) {
    const JBCConstRef *constRef = static_cast<const JBCConstRef*>(constRaw);
    desc = constRef->GetDesc();
  }
  return desc;
}

// ---------- JBCOpInvoke ----------
JBCOpInvoke::JBCOpInvoke(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn)
    : JBCOp(allocator, opIn, kindIn, wideIn), methodIdx(0), count(0) {}

bool JBCOpInvoke::ParseFileImpl(BasicIORead &io) {
  if (JBCOp::CheckNotWide(io) == false) {
    return false;
  }
  bool success = false;
  switch (op) {
    case jbc::kOpInvokeVirtual:
    case jbc::kOpInvokeSpecial:
    case jbc::kOpInvokeStatic:
      methodIdx = io.ReadUInt16(success);
      break;
    case jbc::kOpInvokeInterface:
      methodIdx = io.ReadUInt16(success);
      count = io.ReadUInt8(success);
      (void)io.ReadUInt8(success);
      break;
    case jbc::kOpInvokeDynamic:
      methodIdx = io.ReadUInt16(success);
      (void)io.ReadUInt8(success);
      (void)io.ReadUInt8(success);
      break;
    default:
      ASSERT(false, "Unexpected opcode %s for Invoke", GetOpcodeName().c_str());
      return false;
  }
  return success;
}

const std::vector<JBCPrimType> &JBCOpInvoke::GetInputTypesFromStackImpl() const {
  CHECK_FATAL(false, "Restricted Calls");
  return emptyPrimTypes;
}

std::vector<JBCPrimType> JBCOpInvoke::GetInputTypesFromStackImpl(const JBCConstPool &constPool) const {
  std::vector<JBCPrimType> ans;
  std::string desc = GetMethodDescription(constPool);
  std::vector<std::string> typeNames = JBCUtil::SolveMethodSignature(desc);
  CHECK_FATAL(typeNames.size() > 0, "Invalid method description: %s", desc.c_str());
  if (op != kOpInvokeStatic && op != kOpInvokeDynamic) {
    ans.push_back(kTypeRef);
  }
  for (size_t i = 1; i < typeNames.size(); i++) {
    ans.push_back(JBCUtil::GetPrimTypeForName(typeNames[i]));
  }
  return ans;
}

JBCPrimType JBCOpInvoke::GetOutputTypesToStackImpl() const {
  CHECK_FATAL(false, "Restricted Calls");
  return kTypeDefault;
}

JBCPrimType JBCOpInvoke::GetOutputTypesToStackImpl(const JBCConstPool &constPool) const {
  std::string desc = GetMethodDescription(constPool);
  std::vector<std::string> typeNames = JBCUtil::SolveMethodSignature(desc);
  CHECK_FATAL(typeNames.size() > 0, "Invalid method description: %s", desc.c_str());
  return JBCUtil::GetPrimTypeForName(typeNames[0]);
}

std::string JBCOpInvoke::DumpImpl(const JBCConstPool &constPool) const {
  std::stringstream ss;
  ss << GetOpcodeName() << " ";
  const JBCConst *constRaw = constPool.GetConstByIdx(methodIdx);
  CHECK_NULL_FATAL(constRaw);
  JBCConstTag tag = constRaw->GetTag();
  if (op == jbc::kOpInvokeDynamic) {
    if (tag == kConstInvokeDynamic) {
      const JBCConstInvokeDynamic *constInvokdDynamic = static_cast<const JBCConstInvokeDynamic*>(constRaw);
      CHECK_NULL_FATAL(constInvokdDynamic->GetConstNameAndType());
      ss << constInvokdDynamic->GetConstNameAndType()->GetName() << ":" <<
            constInvokdDynamic->GetConstNameAndType()->GetDesc();
    } else {
      ss << "invalid const tag";
    }
  } else {
    if (tag == kConstMethodRef || tag == kConstInterfaceMethodRef) {
      const JBCConstRef *constRef = static_cast<const JBCConstRef*>(constRaw);
      CHECK_NULL_FATAL(constRef->GetConstClass());
      ss << constRef->GetConstClass()->GetClassNameOrin() << "." << constRef->GetName() << ":" <<
            constRef->GetDesc();
    } else {
      ss << "invalid const tag";
    }
  }
  return ss.str();
}

std::string JBCOpInvoke::GetMethodDescription(const JBCConstPool &constPool) const {
  std::string desc = "";
  switch (op) {
    case jbc::kOpInvokeVirtual: {
      const JBCConst *constRaw = constPool.GetConstByIdxWithTag(methodIdx, kConstMethodRef);
      if (constRaw != nullptr) {
        const JBCConstRef *constRef = static_cast<const JBCConstRef*>(constRaw);
        desc = constRef->GetDesc();
      }
      break;
    }
    case jbc::kOpInvokeInterface: {
      const JBCConst *constRaw = constPool.GetConstByIdxWithTag(methodIdx, kConstInterfaceMethodRef);
      if (constRaw != nullptr) {
        const JBCConstRef *constRef = static_cast<const JBCConstRef*>(constRaw);
        desc = constRef->GetDesc();
      }
      break;
    }
    case jbc::kOpInvokeStatic:
    case jbc::kOpInvokeSpecial: {
      const JBCConst *constRaw = constPool.GetConstByIdx(methodIdx);
      if (constRaw != nullptr && (constRaw->GetTag() == kConstMethodRef ||
                                  constRaw->GetTag() == kConstInterfaceMethodRef)) {
        const JBCConstRef *constRef = static_cast<const JBCConstRef*>(constRaw);
        desc = constRef->GetDesc();
      }
      break;
    }
    case jbc::kOpInvokeDynamic: {
      const JBCConst *constRaw = constPool.GetConstByIdxWithTag(methodIdx, kConstInvokeDynamic);
      if (constRaw != nullptr) {
        const JBCConstInvokeDynamic *constDynamic = static_cast<const JBCConstInvokeDynamic*>(constRaw);
        desc = constDynamic->GetConstNameAndType()->GetDesc();
      }
      break;
    }
    default:
      CHECK_FATAL(false, "Unsupported opcode %s", opcodeInfo.GetOpcodeName(op).c_str());
      break;
  }
  return desc;
}

// ---------- JBCOpJsrRet ----------
JBCOpJsr::JBCOpJsr(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn)
    : JBCOp(allocator, opIn, kindIn, wideIn), target(0) {}

bool JBCOpJsr::ParseFileImpl(BasicIORead &io) {
  if (JBCOp::CheckNotWide(io) == false) {
    return false;
  }
  bool success = false;
  int64 pc = io.GetPos() - 1;
  switch (op) {
    case jbc::kOpJsr:
      target = GetTargetbyInt64(pc + io.ReadInt16(success));
      break;
    case jbc::kOpJsrW:
      target = GetTargetbyInt64(pc + io.ReadInt32(success));
      break;
    default:
      ASSERT(false, "Unexpected opcode %s for JSR", GetOpcodeName().c_str());
      return false;
  }
  return success;
}

// ---------- JBCOpRet ----------
JBCOpRet::JBCOpRet(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn)
    : JBCOp(allocator, opIn, kindIn, wideIn), index(0) {}

bool JBCOpRet::ParseFileImpl(BasicIORead &io) {
  bool success = false;
  if (wide) {
    index = io.ReadUInt16(success);
  } else {
    index = io.ReadUInt8(success);
  }
  return success;
}

// ---------- JBCOpNew ----------
std::map<JBCOpcode, std::vector<JBCPrimType>> JBCOpNew::mapOpInputTypes = JBCOpNew::InitMapOpInputTypes();

JBCOpNew::JBCOpNew(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn)
    : JBCOp(allocator, opIn, kindIn, wideIn), refTypeIdx(0), primType(kPrimNone) {}

bool JBCOpNew::ParseFileImpl(BasicIORead &io) {
  bool success = false;
  switch (op) {
    case jbc::kOpNew:
    case jbc::kOpANewArray:
      refTypeIdx = io.ReadUInt16(success);
      break;
    case jbc::kOpNewArray:
      primType = io.ReadUInt8(success);
      break;
    default:
      ASSERT(false, "Unexpected opcode %s for New", GetOpcodeName().c_str());
      return false;
  }
  return success;
}

const std::vector<JBCPrimType> &JBCOpNew::GetInputTypesFromStackImpl() const {
  auto it = mapOpInputTypes.find(op);
  CHECK_FATAL(it != mapOpInputTypes.end(), "Unsupported opcode %s", opcodeInfo.GetOpcodeName(op).c_str());
  return it->second;
}

JBCPrimType JBCOpNew::GetOutputTypesToStackImpl() const {
  return kTypeRef;
}

std::string JBCOpNew::DumpImpl(const JBCConstPool &constPool) const {
  switch (op) {
    case jbc::kOpNew:
    case jbc::kOpANewArray: {
      std::stringstream ss;
      const JBCConst *constRaw = constPool.GetConstByIdxWithTag(refTypeIdx, JBCConstTag::kConstClass);
      CHECK_NULL_FATAL(constRaw);
      const JBCConstClass *constClass = static_cast<const JBCConstClass*>(constRaw);
      ss << GetOpcodeName() << " " << constClass->GetClassNameOrin();
      return ss.str();
    }
    case jbc::kOpNewArray: {
      std::stringstream ss;
      ss << GetOpcodeName() << " " << GetPrimTypeName();
      return ss.str();
    }
    default:
      ASSERT(false, "Unexpected opcode %s for New", GetOpcodeName().c_str());
      return "";
  }
}

GStrIdx JBCOpNew::GetTypeNameIdx(const JBCConstPool &constPool) const {
  switch (op) {
    case jbc::kOpNew:
    case jbc::kOpANewArray: {
      const JBCConst *constRaw = constPool.GetConstByIdxWithTag(refTypeIdx, JBCConstTag::kConstClass);
      CHECK_NULL_FATAL(constRaw);
      const JBCConstClass *constClass = static_cast<const JBCConstClass*>(constRaw);
      return constClass->GetClassNameIdxMpl();
    }
    case jbc::kOpNewArray:
      return GStrIdx(0);
    default:
      ASSERT(false, "Unexpected opcode %s for New", GetOpcodeName().c_str());
      return GStrIdx(0);
  }
}

std::string JBCOpNew::GetTypeName(const JBCConstPool &constPool) const {
  switch (op) {
    case jbc::kOpNew:
    case jbc::kOpANewArray: {
      const JBCConst *constRaw = constPool.GetConstByIdxWithTag(refTypeIdx, JBCConstTag::kConstClass);
      CHECK_NULL_FATAL(constRaw);
      const JBCConstClass *constClass = static_cast<const JBCConstClass*>(constRaw);
      return constClass->GetClassNameMpl();
    }
    case jbc::kOpNewArray:
      return GetPrimTypeName();
    default:
      ASSERT(false, "Unexpected opcode %s for New", GetOpcodeName().c_str());
      return "";
  }
}

const FEIRType *JBCOpNew::GetFEIRType(const JBCConstPool &constPool) const {
  switch (op) {
    case jbc::kOpNew:
    case jbc::kOpANewArray: {
      const JBCConst *constRaw = constPool.GetConstByIdxWithTag(refTypeIdx, JBCConstTag::kConstClass);
      CHECK_NULL_FATAL(constRaw);
      const JBCConstClass *constClass = static_cast<const JBCConstClass*>(constRaw);
      return constClass->GetFEIRType();
    }
    case jbc::kOpNewArray:
      return GetPrimFEIRType().get();
    default:
      CHECK_FATAL(false, "Unexpected opcode %s for New", GetOpcodeName().c_str());
      return FETypeManager::kPrimFEIRTypeUnknown.get();
  }
}

std::string JBCOpNew::GetPrimTypeName() const {
  switch (primType) {
    case kPrimInt:
      return "I";
    case kPrimBoolean:
      return "Z";
    case kPrimByte:
      return "B";
    case kPrimShort:
      return "S";
    case kPrimChar:
      return "C";
    case kPrimLong:
      return "J";
    case kPrimFloat:
      return "F";
    case kPrimDouble:
      return "D";
    default:
      return "undefined";
  }
}

const UniqueFEIRType &JBCOpNew::GetPrimFEIRType() const {
  switch (primType) {
    case kPrimInt:
      return FETypeManager::kPrimFEIRTypeI32;
    case kPrimBoolean:
      return FETypeManager::kPrimFEIRTypeU1;
    case kPrimByte:
      return FETypeManager::kPrimFEIRTypeI8;
    case kPrimShort:
      return FETypeManager::kPrimFEIRTypeI16;
    case kPrimChar:
      return FETypeManager::kPrimFEIRTypeU16;
    case kPrimLong:
      return FETypeManager::kPrimFEIRTypeI64;
    case kPrimFloat:
      return FETypeManager::kPrimFEIRTypeF32;
    case kPrimDouble:
      return FETypeManager::kPrimFEIRTypeF64;
    default:
      return FETypeManager::kPrimFEIRTypeUnknown;
  }
}

std::map<JBCOpcode, std::vector<JBCPrimType>> JBCOpNew::InitMapOpInputTypes() {
  std::map<JBCOpcode, std::vector<JBCPrimType>> ans;
  ans[kOpNew] = emptyPrimTypes;
  ans[kOpNewArray] = std::vector<JBCPrimType>({ kTypeInt });
  ans[kOpANewArray] = std::vector<JBCPrimType>({ kTypeInt });
  return ans;
}

// ---------- JBCOpMultiANewArray ----------
JBCOpMultiANewArray::JBCOpMultiANewArray(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn)
    : JBCOp(allocator, opIn, kindIn, wideIn), refTypeIdx(0), dim(0) {}

bool JBCOpMultiANewArray::ParseFileImpl(BasicIORead &io) {
  bool success = false;
  refTypeIdx = io.ReadUInt16(success);
  dim = io.ReadUInt8(success);
  return true;
}

const std::vector<JBCPrimType> &JBCOpMultiANewArray::GetInputTypesFromStackImpl() const {
  CHECK_FATAL(false, "Restricted Calls");
  return emptyPrimTypes;
}

std::vector<JBCPrimType> JBCOpMultiANewArray::GetInputTypesFromStackImpl(const JBCConstPool &constPool) const {
  std::vector<JBCPrimType> ans;
  for (uint8 i = 0; i < dim; i++) {
    ans.push_back(kTypeInt);
  }
  return ans;
}

JBCPrimType JBCOpMultiANewArray::GetOutputTypesToStackImpl() const {
  return kTypeRef;
}

std::string JBCOpMultiANewArray::DumpImpl(const JBCConstPool &constPool) const {
  std::stringstream ss;
  ss << GetOpcodeName() << " ";
  const JBCConst *constRaw = constPool.GetConstByIdx(refTypeIdx);
  CHECK_NULL_FATAL(constRaw);
  if (constRaw->GetTag() == kConstClass) {
    const JBCConstClass *constClass = static_cast<const JBCConstClass*>(constRaw);
    ss << constClass->GetClassNameOrin() << " dim=" << uint32{ dim };
  } else {
    ss << "invalid const tag";
  }
  return ss.str();
}

// ---------- JBCOpTypeCheck ----------
std::map<JBCOpcode, std::vector<JBCPrimType>> JBCOpTypeCheck::mapOpInputTypes = JBCOpTypeCheck::InitMapOpInputTypes();
std::map<JBCOpcode, JBCPrimType> JBCOpTypeCheck::mapOpOutputType = JBCOpTypeCheck::InitMapOpOutputType();

JBCOpTypeCheck::JBCOpTypeCheck(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn)
    : JBCOp(allocator, opIn, kindIn, wideIn), typeIdx(0) {}

bool JBCOpTypeCheck::ParseFileImpl(BasicIORead &io) {
  if (JBCOp::CheckNotWide(io) == false) {
    return false;
  }
  bool success = false;
  switch (op) {
    case jbc::kOpCheckCast:
    case jbc::kOpInstanceOf:
      typeIdx = io.ReadUInt16(success);
      break;
    default:
      ASSERT(false, "Unexpected opcode %s for TypeCheck", GetOpcodeName().c_str());
      return false;
  }
  return success;
}

const std::vector<JBCPrimType> &JBCOpTypeCheck::GetInputTypesFromStackImpl() const {
  auto it = mapOpInputTypes.find(op);
  CHECK_FATAL(it != mapOpInputTypes.end(), "Unsupported opcode %s", opcodeInfo.GetOpcodeName(op).c_str());
  return it->second;
}

JBCPrimType JBCOpTypeCheck::GetOutputTypesToStackImpl() const {
  auto it = mapOpOutputType.find(op);
  CHECK_FATAL(it != mapOpOutputType.end(), "Unsupported opcode %s", opcodeInfo.GetOpcodeName(op).c_str());
  return it->second;
}

std::string JBCOpTypeCheck::DumpImpl(const JBCConstPool &constPool) const {
  std::stringstream ss;
  ss << GetOpcodeName() << " ";
  const JBCConst *constRaw = constPool.GetConstByIdxWithTag(typeIdx, kConstClass);
  if (constRaw == nullptr) {
    ss << "invalid type idx";
    return ss.str();
  }
  const JBCConstClass *constClass = static_cast<const JBCConstClass*>(constRaw);
  ss << constClass->GetClassNameOrin();
  return ss.str();
}

std::map<JBCOpcode, std::vector<JBCPrimType>> JBCOpTypeCheck::InitMapOpInputTypes() {
  std::map<JBCOpcode, std::vector<JBCPrimType>> ans;
  ans[kOpCheckCast] = std::vector<JBCPrimType>({ kTypeRef });
  ans[kOpInstanceOf] = std::vector<JBCPrimType>({ kTypeRef });
  return ans;
}

std::map<JBCOpcode, JBCPrimType> JBCOpTypeCheck::InitMapOpOutputType() {
  std::map<JBCOpcode, JBCPrimType> ans;
  ans[kOpCheckCast] = kTypeRef;
  ans[kOpInstanceOf] = kTypeInt;
  return ans;
}
}  // namespace jbc
}  // namespace maple
