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

#ifndef MPLFE_INCLUDE_JBC_OPCODE_H
#define MPLFE_INCLUDE_JBC_OPCODE_H
#include <climits>
#include <map>
#include <string>
#include <vector>
#include "types_def.h"
#include "mempool_allocator.h"
#include "factory.h"
#include "basic_io.h"

namespace maple {
namespace jbc {
enum JBCOpcode : uint8 {
#define JBC_OP(op, value, type, name, flag) \
  kOp##op = value,
#include "jbc_opcode.def"
#undef JBC_OP
};

enum JBCOpcodeKind : uint8 {
  kOpKindDefault = 0,
#define JBC_OP_KIND(kind, name) \
  kOpKind##kind,
#include "jbc_opcode_kind.def"
#undef JBC_OP_KIND
  kOpKindSize
};

enum JBCPrimType : uint8 {
  kTypeDefault = 0,
  kTypeInt,
  kTypeLong,
  kTypeFloat,
  kTypeDouble,
  kTypeByteOrBoolean,
  kTypeChar,
  kTypeShort,
  kTypeRef,
  kTypeAddress,
  kTypeLongDummy,
  kTypeDoubleDummy,
  kTypeSize
};

enum JBCOpcodeFlag : uint8 {
  kOpFlagNone = 0,
  kOpFlagFallThru = 1,
  kOpFlagBranch = 1 << 1,
  kOpFlagThrowable = 1 << 2
};

struct JBCOpcodeDesc {
  JBCOpcodeKind kind;
  std::string name;
  uint8 flags;
};

class JBCOpcodeInfo {
 public:
  static const uint32 kOpSize = 0x100;
  JBCOpcodeInfo();
  ~JBCOpcodeInfo() = default;

  JBCOpcodeKind GetOpcodeKind(JBCOpcode op) const {
    return table[static_cast<uint8>(op)].kind;
  }

  const std::string &GetOpcodeName(JBCOpcode op) const {
    return table[static_cast<uint8>(op)].name;
  }

  bool IsFallThru(JBCOpcode op) const {
    return (table[static_cast<uint8>(op)].flags & kOpFlagFallThru) != 0;
  }

  bool IsBranch(JBCOpcode op) const {
    return (table[static_cast<uint8>(op)].flags & kOpFlagBranch) != 0;
  }

  bool IsThrowable(JBCOpcode op) const {
    return (table[static_cast<uint8>(op)].flags & kOpFlagThrowable) != 0;
  }

 private:
  JBCOpcodeDesc table[kOpSize];
};

class JBCConstPool;
class JBCOp {
 public:
  JBCOp(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn);
  virtual ~JBCOp() = default;
  bool CheckNotWide(const BasicIORead &io) const;
  bool ParseFile(BasicIORead &io) {
    return ParseFileImpl(io);
  }

  std::string Dump(const JBCConstPool &constPool) const {
    return DumpImpl(constPool);
  }

  const std::vector<JBCPrimType> &GetInputTypesFromStack() const {
    return GetInputTypesFromStackImpl();
  }

  std::vector<JBCPrimType> GetInputTypesFromStack(const JBCConstPool &constPool) const {
    return GetInputTypesFromStackImpl(constPool);
  }

  JBCPrimType GetOutputTypesToStack() const {
    return GetOutputTypesToStackImpl();
  }

  JBCPrimType GetOutputTypesToStack(const JBCConstPool &constPool) const {
    return GetOutputTypesToStackImpl(constPool);
  }

  JBCOpcode GetOpcode() const {
    return op;
  }

  JBCOpcodeKind GetOpcodeKind() const {
    return kind;
  }

  bool IsWide() const {
    return wide;
  }

  const std::string &GetOpcodeName() const {
    return opcodeInfo.GetOpcodeName(op);
  }

  bool IsFallThru() const {
    return opcodeInfo.IsFallThru(op);
  }

  bool IsBranch() const {
    return opcodeInfo.IsBranch(op);
  }

  bool IsThrowable() const {
    return opcodeInfo.IsThrowable(op);
  }

  static JBCOpcodeInfo &GetOpcodeInfo() {
    return opcodeInfo;
  }

  static uint32 GetTargetbyInt64(int64 pc) {
    if (pc < 0 || pc > UINT16_MAX) {
      CHECK_FATAL(false, "invalid PC: %ld", pc);
      return 0;
    } else {
      return static_cast<uint32>(pc);
    }
  }

 protected:
  virtual bool ParseFileImpl(BasicIORead &io) = 0;
  virtual const std::vector<JBCPrimType> &GetInputTypesFromStackImpl() const;
  virtual std::vector<JBCPrimType> GetInputTypesFromStackImpl(const JBCConstPool &constPool) const;
  virtual JBCPrimType GetOutputTypesToStackImpl() const;
  virtual JBCPrimType GetOutputTypesToStackImpl(const JBCConstPool &constPool) const;
  virtual std::string DumpImpl(const JBCConstPool &constPool) const;

  JBCOpcode op : 8;
  JBCOpcodeKind kind : 7;
  bool wide : 1;
  static JBCOpcodeInfo opcodeInfo;
  static std::vector<JBCPrimType> emptyPrimTypes;
};

class JBCOpUnused : public JBCOp {
 public:
  JBCOpUnused(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn);
  ~JBCOpUnused() = default;

 protected:
  bool ParseFileImpl(BasicIORead &io) override;
};

class JBCOpReversed : public JBCOp {
 public:
  JBCOpReversed(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn);
  ~JBCOpReversed() = default;

 protected:
  bool ParseFileImpl(BasicIORead &io) override;
};

class JBCOpDefault : public JBCOp {
 public:
  JBCOpDefault(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn);
  ~JBCOpDefault() = default;

 protected:
  bool ParseFileImpl(BasicIORead &io) override;
  const std::vector<JBCPrimType> &GetInputTypesFromStackImpl() const override;
  JBCPrimType GetOutputTypesToStackImpl() const override;

 private:
  static std::map<JBCOpcode, std::vector<JBCPrimType>> mapOpInputTypes;
  static std::map<JBCOpcode, std::vector<JBCPrimType>> InitMapOpInputTypes();
  static std::map<JBCOpcode, JBCPrimType> mapOpOutputType;
  static std::map<JBCOpcode, JBCPrimType> InitMapOpOutputType();
  static void InitMapOpInputTypesForArrayLoad(std::map<JBCOpcode, std::vector<JBCPrimType>> &ans);
  static void InitMapOpOutputTypesForArrayLoad(std::map<JBCOpcode, JBCPrimType> &ans);
  static void InitMapOpInputTypesForArrayStore(std::map<JBCOpcode, std::vector<JBCPrimType>> &ans);
  static void InitMapOpOutputTypesForArrayStore(std::map<JBCOpcode, JBCPrimType> &ans);
  static void InitMapOpInputTypesForMathBinop(std::map<JBCOpcode, std::vector<JBCPrimType>> &ans);
  static void InitMapOpOutputTypesForMathBinop(std::map<JBCOpcode, JBCPrimType> &ans);
  static void InitMapOpInputTypesForMathUnop(std::map<JBCOpcode, std::vector<JBCPrimType>> &ans);
  static void InitMapOpOutputTypesForMathUnop(std::map<JBCOpcode, JBCPrimType> &ans);
  static void InitMapOpInputTypesForConvert(std::map<JBCOpcode, std::vector<JBCPrimType>> &ans);
  static void InitMapOpOutputTypesForConvert(std::map<JBCOpcode, JBCPrimType> &ans);
  static void InitMapOpInputTypesForCompare(std::map<JBCOpcode, std::vector<JBCPrimType>> &ans);
  static void InitMapOpOutputTypesForCompare(std::map<JBCOpcode, JBCPrimType> &ans);
  static void InitMapOpInputTypesForReturn(std::map<JBCOpcode, std::vector<JBCPrimType>> &ans);
  static void InitMapOpOutputTypesForReturn(std::map<JBCOpcode, JBCPrimType> &ans);
  static void InitMapOpInputTypesForThrow(std::map<JBCOpcode, std::vector<JBCPrimType>> &ans);
  static void InitMapOpOutputTypesForThrow(std::map<JBCOpcode, JBCPrimType> &ans);
  static void InitMapOpInputTypesForMonitor(std::map<JBCOpcode, std::vector<JBCPrimType>> &ans);
  static void InitMapOpOutputTypesForMonitor(std::map<JBCOpcode, JBCPrimType> &ans);
  static void InitMapOpInputTypesForArrayLength(std::map<JBCOpcode, std::vector<JBCPrimType>> &ans);
  static void InitMapOpOutputTypesForArrayLength(std::map<JBCOpcode, JBCPrimType> &ans);
};

class JBCOpConst : public JBCOp {
 public:
  JBCOpConst(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn);
  ~JBCOpConst() = default;
  std::string DumpBiPush() const;
  std::string DumpSiPush() const;
  std::string DumpLdc(const JBCConstPool &constPool) const;
  int32 GetValueInt() const;
  int64 GetValueLong() const;
  float GetValueFloat() const;
  double GetValueDouble() const;
  int8 GetValueByte() const;
  int16 GetValueShort() const;
  uint16 GetIndex() const {
    return u.index;
  }

  void SetValue(int8 value) {
    u.bvalue = value;
  }

  void SetValue(int16 value) {
    u.svalue = value;
  }

  void SetIndex(uint16 argIdx) {
    u.index = argIdx;
  }

 protected:
  bool ParseFileImpl(BasicIORead &io) override;
  JBCPrimType GetOutputTypesToStackImpl(const JBCConstPool &constPool) const override;
  std::string DumpImpl(const JBCConstPool &constPool) const override;

 private:
  union {
    int8 bvalue;
    int16 svalue;
    uint16 index;
    uint16 raw;
  } u;

  static std::map<JBCOpcode, int32> InitValueMapI();
  static std::map<JBCOpcode, int64> InitValueMapJ();
  static std::map<JBCOpcode, float> InitValueMapF();
  static std::map<JBCOpcode, double> InitValueMapD();

  static std::map<JBCOpcode, int32> valueMapI;
  static std::map<JBCOpcode, int64> valueMapJ;
  static std::map<JBCOpcode, float> valueMapF;
  static std::map<JBCOpcode, double> valueMapD;
};

class JBCOpSlotOpr : public JBCOp {
 public:
  JBCOpSlotOpr(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn);
  ~JBCOpSlotOpr() = default;
  uint16 GetSlotIdx() const {
    return slotIdx;
  }

  void SetSlotIdx(uint16 argSlotIdx) {
    slotIdx = argSlotIdx;
  }

 protected:
  bool ParseFileImpl(BasicIORead &io) override;
  const std::vector<JBCPrimType> &GetInputTypesFromStackImpl() const override;
  JBCPrimType GetOutputTypesToStackImpl() const override;
  std::string DumpImpl(const JBCConstPool &constPool) const override;

 private:
  static std::map<JBCOpcode, std::pair<uint16, JBCPrimType>> InitMapSlotIdxAndType();
  static std::map<JBCOpcode, std::vector<JBCPrimType>> InitMapOpInputTypes();
  static std::map<JBCOpcode, JBCPrimType> InitMapOpOutputType();

  uint16 slotIdx;
  static std::map<JBCOpcode, std::pair<uint16, JBCPrimType>> mapSlotIdxAndType;
  static std::map<JBCOpcode, std::vector<JBCPrimType>> mapOpInputTypes;
  static std::map<JBCOpcode, JBCPrimType> mapOpOutputType;
};

class JBCOpMathInc : public JBCOp {
 public:
  JBCOpMathInc(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn);
  ~JBCOpMathInc() = default;
  uint16 GetIndex() const {
    return index;
  }

  void SetIndex(uint16 argIndex) {
    index = argIndex;
  }

  int16 GetIncr() const {
    return incr;
  }

  void SetIncr(int16 argIncr) {
    incr = argIncr;
  }

 protected:
  bool ParseFileImpl(BasicIORead &io) override;

 private:
  static std::map<JBCOpcode, std::vector<JBCPrimType>> InitMapOpInputTypes();
  static std::map<JBCOpcode, JBCPrimType> InitMapOpOutputType();

  uint16 index;
  int16 incr;
  static std::map<JBCOpcode, std::vector<JBCPrimType>> mapOpInputTypes;
  static std::map<JBCOpcode, JBCPrimType> mapOpOutputType;
};

class JBCOpBranch : public JBCOp {
 public:
  JBCOpBranch(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn);
  ~JBCOpBranch() = default;
  uint32 GetTarget() const {
    return target;
  }

  void SetTarget(uint32 argTarget) {
    target = argTarget;
  }

 protected:
  bool ParseFileImpl(BasicIORead &io) override;
  std::string DumpImpl(const JBCConstPool &constPool) const override;
  const std::vector<JBCPrimType> &GetInputTypesFromStackImpl() const override;

 private:
  static std::map<JBCOpcode, std::vector<JBCPrimType>> InitMapOpInputTypes();

  uint32 target;
  static std::map<JBCOpcode, std::vector<JBCPrimType>> mapOpInputTypes;
};

class JBCOpGoto : public JBCOp {
 public:
  JBCOpGoto(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn);
  ~JBCOpGoto() = default;
  uint32 GetTarget() const {
    return target;
  }

  void SetTarget(uint32 argTarget) {
    target = argTarget;
  }

 protected:
  bool ParseFileImpl(BasicIORead &io) override;
  std::string DumpImpl(const JBCConstPool &constPool) const override;

 private:
  uint32 target;
};

class JBCOpSwitch : public JBCOp {
 public:
  JBCOpSwitch(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn);
  ~JBCOpSwitch() = default;
  const MapleMap<int32, uint32> &GetTargets() const {
    return targets;
  }

  void ClearTargets() {
    targets.clear();
  }

  void AddOrSetTarget(int32 value, uint32 targetPC) {
    targets[value] = targetPC;
  }

  uint32 GetDefaultTarget() const {
    return targetDefault;
  }

  void SetDefaultTarget(uint32 targetPC) {
    targetDefault = targetPC;
  }

 protected:
  bool ParseFileImpl(BasicIORead &io) override;
  const std::vector<JBCPrimType> &GetInputTypesFromStackImpl() const override;
  std::string DumpImpl(const JBCConstPool &constPool) const override;

 private:
  static std::map<JBCOpcode, std::vector<JBCPrimType>> InitMapOpInputTypes();

  MapleMap<int32, uint32> targets;
  uint32 targetDefault;
  static std::map<JBCOpcode, std::vector<JBCPrimType>> mapOpInputTypes;
};

class JBCOpFieldOpr : public JBCOp {
 public:
  JBCOpFieldOpr(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn);
  ~JBCOpFieldOpr() = default;
  std::string GetFieldType(const JBCConstPool &constPool) const;
  uint16 GetFieldIdx() const {
    return fieldIdx;
  }

  void SetFieldIdx(uint16 idx) {
    fieldIdx = idx;
  }

 protected:
  bool ParseFileImpl(BasicIORead &io) override;
  const std::vector<JBCPrimType> &GetInputTypesFromStackImpl() const override;
  std::vector<JBCPrimType> GetInputTypesFromStackImpl(const JBCConstPool &constPool) const override;
  JBCPrimType GetOutputTypesToStackImpl() const override;
  JBCPrimType GetOutputTypesToStackImpl(const JBCConstPool &constPool) const override;
  std::string DumpImpl(const JBCConstPool &constPool) const override;

 private:
  std::vector<JBCPrimType> GetInputTypesFromStackForGet(const JBCConstPool &constPool) const;
  std::vector<JBCPrimType> GetInputTypesFromStackForPut(const JBCConstPool &constPool) const;

  uint16 fieldIdx;
};

class JBCOpInvoke : public JBCOp {
 public:
  JBCOpInvoke(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn);
  ~JBCOpInvoke() = default;
  std::string GetMethodDescription(const JBCConstPool &constPool) const;
  uint16 GetMethodIdx() const {
    return methodIdx;
  }

  void SetMethodIdx(uint16 idx) {
    methodIdx = idx;
  }

  uint8 GetCount() const {
    return count;
  }

  void SetCount(uint8 argCount) {
    count = argCount;
  }

 protected:
  bool ParseFileImpl(BasicIORead &io) override;
  const std::vector<JBCPrimType> &GetInputTypesFromStackImpl() const override;
  std::vector<JBCPrimType> GetInputTypesFromStackImpl(const JBCConstPool &constPool) const override;
  JBCPrimType GetOutputTypesToStackImpl() const override;
  JBCPrimType GetOutputTypesToStackImpl(const JBCConstPool &constPool) const override;
  std::string DumpImpl(const JBCConstPool &constPool) const override;

 private:
  uint16 methodIdx;
  uint8 count;
};

class JBCOpJsr : public JBCOp {
 public:
  JBCOpJsr(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn);
  ~JBCOpJsr() = default;
  uint32 GetTarget() const {
    return target;
  }

  void SetTarget(uint32 argTarget) {
    target = argTarget;
  }

 protected:
  bool ParseFileImpl(BasicIORead &io) override;

 private:
  uint32 target;
};

class JBCOpRet : public JBCOp {
 public:
  JBCOpRet(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn);
  ~JBCOpRet() = default;
  uint16 GetIndex() const {
    return index;
  }

  void SetIndex(uint16 argIndex) {
    index = argIndex;
  }

 protected:
  bool ParseFileImpl(BasicIORead &io) override;

 private:
  uint16 index;
};

class JBCOpNew : public JBCOp {
 public:
  enum PrimType : uint8 {
    kPrimNone = 0,
    kPrimBoolean = 4,
    kPrimChar = 5,
    kPrimFloat = 6,
    kPrimDouble = 7,
    kPrimByte = 8,
    kPrimShort = 9,
    kPrimInt = 10,
    kPrimLong = 11
  };

  JBCOpNew(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn);
  ~JBCOpNew() = default;

  GStrIdx GetTypeNameIdx(const JBCConstPool &constPool) const;
  std::string GetTypeName(const JBCConstPool &constPool) const;
  uint16 GetRefTypeIdx() const {
    return refTypeIdx;
  }

  void SetRefTypeIdx(uint16 idx) {
    refTypeIdx = idx;
  }

  uint8 GetPrimType() const {
    return primType;
  }

  void SetPrimType(uint8 type) {
    primType = type;
  }

 protected:
  bool ParseFileImpl(BasicIORead &io) override;
  const std::vector<JBCPrimType> &GetInputTypesFromStackImpl() const override;
  JBCPrimType GetOutputTypesToStackImpl() const override;
  std::string DumpImpl(const JBCConstPool &constPool) const override;

 private:
  static std::map<JBCOpcode, std::vector<JBCPrimType>> InitMapOpInputTypes();
  std::string GetPrimTypeName() const;

  uint16 refTypeIdx;
  uint8 primType;
  static std::map<JBCOpcode, std::vector<JBCPrimType>> mapOpInputTypes;
};

class JBCOpMultiANewArray : public JBCOp {
 public:
  JBCOpMultiANewArray(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn);
  ~JBCOpMultiANewArray() = default;
  uint16 GetRefTypeIdx() const {
    return refTypeIdx;
  }

  void SetRefTypeIdx(uint16 idx) {
    refTypeIdx = idx;
  }

  uint8 GetDim() const {
    return dim;
  }

  void SetDim(uint8 argDim) {
    dim = argDim;
  }

 protected:
  bool ParseFileImpl(BasicIORead &io) override;
  const std::vector<JBCPrimType> &GetInputTypesFromStackImpl() const override;
  std::vector<JBCPrimType> GetInputTypesFromStackImpl(const JBCConstPool &constPool) const override;
  JBCPrimType GetOutputTypesToStackImpl() const override;

 private:
  uint16 refTypeIdx;
  uint8 dim;
};

class JBCOpTypeCheck : public JBCOp {
 public:
  JBCOpTypeCheck(MapleAllocator &allocator, JBCOpcode opIn, JBCOpcodeKind kindIn, bool wideIn);
  ~JBCOpTypeCheck() = default;
  uint16 GetTypeIdx() const {
    return typeIdx;
  }

  void SetTypeIdx(uint16 idx) {
    typeIdx = idx;
  }

 protected:
  bool ParseFileImpl(BasicIORead &io) override;
  const std::vector<JBCPrimType> &GetInputTypesFromStackImpl() const override;
  JBCPrimType GetOutputTypesToStackImpl() const override;
  std::string DumpImpl(const JBCConstPool &constPool) const override;

 private:
  static std::map<JBCOpcode, std::vector<JBCPrimType>> InitMapOpInputTypes();
  static std::map<JBCOpcode, JBCPrimType> InitMapOpOutputType();

  uint16 typeIdx;
  static std::map<JBCOpcode, std::vector<JBCPrimType>> mapOpInputTypes;
  static std::map<JBCOpcode, JBCPrimType> mapOpOutputType;
};
}  // namespace jbc
}  // namespace maple
#endif  // MPLFE_INCLUDE_JBC_OPCODE_H
