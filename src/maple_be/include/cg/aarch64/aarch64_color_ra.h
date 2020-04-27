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
#ifndef MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_COLOR_RA_H
#define MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_COLOR_RA_H
#include "aarch64_reg_alloc.h"
#include "aarch64_operand.h"
#include "aarch64_insn.h"
#include "aarch64_abi.h"
#include "loop.h"

namespace maplebe {
#define RESERVED_REGS

#define USE_LRA
#define USE_SPLIT
#undef USE_BB_FREQUENCY
#define OPTIMIZE_FOR_PROLOG
#undef REUSE_SPILLMEM
#undef COLOR_SPLIT
#undef MOVE_COALESCE

/* for robust test */
#undef CONSISTENT_MEMOPND
#undef RANDOM_PRIORITY

constexpr uint32 k32 = sizeof(int) * CHAR_BIT;
constexpr uint32 k64 = sizeof(int64) * CHAR_BIT;
constexpr uint32 kU64 = sizeof(uint64) * CHAR_BIT;

template <typename T, typename Comparator = std::less<T>>
inline bool FindNotIn(const std::set<T, Comparator> &set, const T &item) {
  return set.find(item) == set.end();
}

template <typename T>
inline bool FindNotIn(const MapleSet<T> &set, const T &item) {
  return set.find(item) == set.end();
}

template <typename T>
inline bool FindNotIn(const MapleList<T> &list, const T &item) {
  return std::find(list.begin(), list.end(), item) == list.end();
}

template <typename T, typename Comparator = std::less<T>>
inline bool FindIn(const std::set<T, Comparator> &set, const T &item) {
  return set.find(item) != set.end();
}

template<typename T>
inline bool FindIn(const MapleSet<T> &set, const T &item) {
  return set.find(item) != set.end();
}

template<typename T>
inline bool FindIn(const MapleList<T> &list, const T &item) {
  return std::find(list.begin(), list.end(), item) != list.end();
}

inline bool IsBitArrElemSet(const uint64 *vec, const uint32 num) {
  size_t index = num / kU64;
  uint64 bit = num % kU64;
  return vec[index] & (1ULL << bit);
}

inline bool IsBBsetOverlap(const uint64 *vec1, const uint64 *vec2, uint32 bbBuckets) {
  for (uint32 i = 0; i < bbBuckets; ++i) {
    if ((vec1[i] & vec2[i]) != 0) {
      return true;
    }
  }
  return false;
}

/*
 * This is per bb per LR.
 * LU info is particular to a bb in a LR.
 */
class LiveUnit {
 public:
  LiveUnit() = default;
  ~LiveUnit() = default;

  void PrintLiveUnit() const;

  uint32 GetBegin() const {
    return begin;
  }

  void SetBegin(uint32 val) {
    begin = val;
  }

  uint32 GetEnd() const {
    return end;
  }

  void SetEnd(uint32 end) {
    this->end = end;
  }

  bool HasCall() const {
    return hasCall;
  }

  void SetHasCall(bool hasCall) {
    this->hasCall = hasCall;
  }

  uint32 GetDefNum() const {
    return defNum;
  }

  void SetDefNum(uint32 defNum) {
    this->defNum = defNum;
  }

  void IncDefNum() {
    ++defNum;
  }

  uint32 GetUseNum() const {
    return useNum;
  }

  void SetUseNum(uint32 useNum) {
    this->useNum = useNum;
  }

  void IncUseNum() {
    ++useNum;
  }

  bool NeedReload() const {
    return needReload;
  }

  void SetNeedReload(bool needReload) {
    this->needReload = needReload;
  }

  bool NeedRestore() const {
    return needRestore;
  }

  void SetNeedRestore(bool needRestore) {
    this->needRestore = needRestore;
  }

 private:
  uint32 begin = 0;      /* first encounter in bb */
  uint32 end = 0;        /* last encounter in bb */
  bool hasCall = false;  /* bb has a call */
  uint32 defNum = 0;
  uint32 useNum = 0;     /* used for priority calculation */
  bool needReload = false;
  bool needRestore = false;
};

struct SortedBBCmpFunc {
  bool operator()(const BB *lhs, const BB *rhs) const {
    return (lhs->GetLevel() < rhs->GetLevel());
  }
};

/* LR is for each global vreg. */
class LiveRange {
 public:
  explicit LiveRange(MapleAllocator &allocator)
      : pregveto(allocator.Adapter()),
        forbidden(allocator.Adapter()),
        prefs(allocator.Adapter()),
        luMap(allocator.Adapter()) {}

  ~LiveRange() = default;

  regno_t GetRegNO() const {
    return regNO;
  }

  void SetRegNO(regno_t val) {
    regNO = val;
  }

  uint32 GetID() const {
    return id;
  }

  void SetID(uint32 id) {
    this->id = id;
  }

  regno_t GetAssignedRegNO() const {
    return assignedRegNO;
  }

  void SetAssignedRegNO(regno_t val) {
    assignedRegNO = val;
  }

  uint32 GetNumCall() const {
    return numCall;
  }

  void SetNumCall(uint32 num) {
    numCall = num;
  }

  void IncNumCall() {
    ++numCall;
  }

  RegType GetRegType() const {
    return regType;
  }

  void SetRegType(RegType regType) {
    this->regType = regType;
  }

  float GetPriority() const {
    return priority;
  }

  void SetPriority(float priority) {
    this->priority = priority;
  }

  void SetBBBuckets(uint32 bucketNum) {
    bbBuckets = bucketNum;
  }

  void SetRegBuckets(uint32 bucketNum) {
    regBuckets = bucketNum;
  }

  uint32 GetNumBBMembers() const {
    return numBBMembers;
  }

  void IncNumBBMembers() {
    ++numBBMembers;
  }

  void DecNumBBMembers() {
    --numBBMembers;
  }

  void InitBBMember(MemPool &memPool, size_t size) {
    bbMember = memPool.NewArray<uint64>(size);
    errno_t ret = memset_s(bbMember, size * sizeof(uint64), 0, size * sizeof(uint64));
    CHECK_FATAL(ret == EOK, "call memset_s failed");
  }

  uint64 *GetBBMember() {
    return bbMember;
  }

  const uint64 *GetBBMember() const {
    return bbMember;
  }

  uint64 GetBBMemberElem(int32 index) const {
    return bbMember[index];
  }

  void SetBBMemberElem(int32 index, uint64 elem) {
    bbMember[index] = elem;
  }

  void SetMemberBitArrElem(uint32 bbID) {
    uint32 index = bbID / kU64;
    uint64 bit = bbID % kU64;
    uint64 mask = 1ULL << bit;
    if ((GetBBMemberElem(index) & mask) == 0) {
      IncNumBBMembers();
      SetBBMemberElem(index, GetBBMemberElem(index) | mask);
    }
  }

  void UnsetMemberBitArrElem(uint32 bbID) {
    uint32 index = bbID / kU64;
    uint64 bit = bbID % kU64;
    uint64 mask = 1ULL << bit;
    if ((GetBBMemberElem(index) & mask) != 0) {
      DecNumBBMembers();
      SetBBMemberElem(index, GetBBMemberElem(index) & (~mask));
    }
  }

  void SetConflictBitArrElem(regno_t regNO) {
    uint32 index = regNO / kU64;
    uint64 bit = regNO % kU64;
    uint64 mask = 1ULL << bit;
    if ((GetBBConflictElem(index) & mask) == 0) {
      IncNumBBConflicts();
      SetBBConflictElem(index, GetBBConflictElem(index) | mask);
    }
  }

  void UnsetConflictBitArrElem(regno_t regNO) {
    uint32 index = regNO / kU64;
    uint64 bit = regNO % kU64;
    uint64 mask = 1ULL << bit;
    if ((GetBBConflictElem(index) & mask) != 0) {
      DecNumBBConflicts();
      SetBBConflictElem(index, GetBBConflictElem(index) & (~mask));
    }
  }

  void InitPregveto() {
    pregveto.clear();
    pregveto.resize(kMaxRegNum);
  }

  bool GetPregveto(regno_t regNO) const {
    return pregveto[regNO];
  }

  size_t GetPregvetoSize() const {
    return numPregveto;
  }

  void InsertElemToPregveto(regno_t regNO) {
    if (!pregveto[regNO]) {
      pregveto[regNO] = true;
      ++numPregveto;
    }
  }

  void InitForbidden() {
    forbidden.clear();
    forbidden.resize(kMaxRegNum);
  }

  const MapleVector<bool> &GetForbidden() const {
    return forbidden;
  }

  bool GetForbidden(regno_t regNO) const {
    return forbidden[regNO];
  }

  size_t GetForbiddenSize() const {
    return numForbidden;
  }

  void InsertElemToForbidden(regno_t regNO) {
    if (!forbidden[regNO]) {
      forbidden[regNO] = true;
      ++numForbidden;
    }
  }

  void EraseElemFromForbidden(regno_t regNO) {
    if (forbidden[regNO]) {
      forbidden[regNO] = false;
      --numForbidden;
    }
  }

  void ClearForbidden() {
    forbidden.clear();
  }

  uint32 GetNumBBConflicts() const {
    return numBBConflicts;
  }

  void IncNumBBConflicts() {
    ++numBBConflicts;
  }

  void DecNumBBConflicts() {
    --numBBConflicts;
  }

  void InitBBConflict(MemPool &memPool, size_t size) {
    bbConflict = memPool.NewArray<uint64>(size);
    errno_t ret = memset_s(bbConflict, size * sizeof(uint64), 0, size * sizeof(uint64));
    CHECK_FATAL(ret == EOK, "call memset_s failed");
  }

  const uint64 *GetBBConflict() const {
    return bbConflict;
  }

  uint64 GetBBConflictElem(int32 index) const {
    ASSERT(index < regBuckets, "out of bbConflict");
    return bbConflict[index];
  }

  void SetBBConflictElem(int32 index, uint64 elem) {
    ASSERT(index < regBuckets, "out of bbConflict");
    bbConflict[index] = elem;
  }

  void SetOldConflict(uint64 *conflict) {
    oldConflict = conflict;
  }

  const uint64 *GetOldConflict() const {
    return oldConflict;
  }

  const MapleSet<regno_t> &GetPrefs() const {
    return prefs;
  }

  void InsertElemToPrefs(regno_t regNO) {
    prefs.insert(regNO);
  }

  const MapleMap<uint32, LiveUnit*> &GetLuMap() const {
    return luMap;
  }

  MapleMap<uint32, LiveUnit*>::iterator FindInLuMap(uint32 index) {
    return luMap.find(index);
  }

  MapleMap<uint32, LiveUnit*>::iterator EndOfLuMap() {
    return luMap.end();
  }

  MapleMap<uint32, LiveUnit*>::iterator EraseLuMap(MapleMap<uint32, LiveUnit*>::iterator it) {
    return luMap.erase(it);
  }

  void SetElemToLuMap(uint32 key, LiveUnit &value) {
    luMap[key] = &value;
  }

  LiveUnit *GetLiveUnitFromLuMap(uint32 key) {
    return luMap[key];
  }

  const LiveUnit *GetLiveUnitFromLuMap(uint32 key) const {
    auto it = luMap.find(key);
    ASSERT(it != luMap.end(), "can't find live unit");
    return it->second;
  }

  const LiveRange *GetSplitLr() const {
    return splitLr;
  }

  void SetSplitLr(LiveRange &lr) {
    splitLr = &lr;
  }

#ifdef OPTIMIZE_FOR_PROLOG
  uint32 GetNumDefs() const {
    return numDefs;
  }

  void IncNumDefs() {
    ++numDefs;
  }

  uint32 GetNumUses() const {
    return numUses;
  }

  void IncNumUses() {
    ++numUses;
  }

  uint32 GetFrequency() const {
    return frequency;
  }

  void SetFrequency(uint32 frequency) {
    this->frequency = frequency;
  }
#endif  /* OPTIMIZE_FOR_PROLOG */

  MemOperand *GetSpillMem() {
    return spillMem;
  }

  const MemOperand *GetSpillMem() const {
    return spillMem;
  }

  void SetSpillMem(MemOperand& memOpnd) {
    spillMem = &memOpnd;
  }

  regno_t GetSpillReg() const {
    return spillReg;
  }

  void SetSpillReg(regno_t spillReg) {
    this->spillReg = spillReg;
  }

  uint32 GetSpillSize() const {
    return spillSize;
  }

  void SetSpillSize(uint32 size) {
    spillSize = size;
  }

  bool IsSpilled() const {
    return spilled;
  }

  void SetSpilled(bool spill) {
    spilled = spill;
  }

  bool IsNonLocal() const {
    return isNonLocal;
  }

  void SetIsNonLocal(bool isNonLocal) {
    this->isNonLocal = isNonLocal;
  }

 private:
  regno_t regNO = 0;
  uint32 id     = 0;                  /* for priority tie breaker */
  regno_t assignedRegNO = 0;          /* color assigned */
  uint32 numCall  = 0;
  RegType regType = kRegTyUndef;
  float priority  = 0.0;
  uint32 bbBuckets = 0;               /* size of bit array for bb (each bucket == 64 bits) */
  uint32 regBuckets = 0;              /* size of bit array for reg (each bucket == 64 bits) */
  uint32 numBBMembers = 0;            /* number of bits set in bbMember */
  uint64 *bbMember = nullptr;         /* Same as smember, but use bit array */

  MapleVector<bool> pregveto;         /* pregs cannot be assigned   -- SplitLr may clear forbidden */
  MapleVector<bool> forbidden;        /* pregs cannot be assigned */
  uint32 numPregveto = 0;
  uint32 numForbidden = 0;

  uint32 numBBConflicts = 0;          /* number of bits set in bbConflict */
  uint64 *bbConflict   = nullptr;     /* vreg interference from graph neighbors (bit) */
  uint64 *oldConflict = nullptr;
  MapleSet<regno_t> prefs;            /* pregs that prefer */
  MapleMap<uint32, LiveUnit*> luMap;  /* info for each bb */
  LiveRange *splitLr = nullptr;       /* The 1st part of the split */
#ifdef OPTIMIZE_FOR_PROLOG
  uint32 numDefs = 0;
  uint32 numUses = 0;
  uint32 frequency = 0;
#endif                                /* OPTIMIZE_FOR_PROLOG */
  MemOperand *spillMem = nullptr;     /* memory operand used for spill, if any */
  regno_t spillReg = 0;               /* register operand for spill at current point */
  uint32 spillSize = 0;               /* 32 or 64 bit spill */
  bool spilled = false;               /* color assigned */
  bool isNonLocal = false;
};

/* One per bb, to communicate local usage to global RA */
class LocalRaInfo {
 public:
  explicit LocalRaInfo(MapleAllocator &allocator)
      : defCnt(allocator.Adapter()),
        useCnt(allocator.Adapter()),
        globalPreg(allocator.Adapter()) {}

  ~LocalRaInfo() = default;

  const MapleMap<regno_t, uint16> &GetDefCnt() const {
    return defCnt;
  }

  uint16 GetDefCntElem(regno_t regNO) {
    return defCnt[regNO];
  }

  void SetDefCntElem(regno_t key, uint16 value) {
    defCnt[key] = value;
  }

  const MapleMap<regno_t, uint16> &GetUseCnt() const {
    return useCnt;
  }

  uint16 GetUseCntElem(regno_t regNO) {
    return useCnt[regNO];
  }

  void SetUseCntElem(regno_t key, uint16 value) {
    useCnt[key] = value;
  }

  void InsertElemToGlobalPreg(regno_t regNO) {
    globalPreg.insert(regNO);
  }

  uint64 GetGlobalPregMask() const {
    return globalPregMask;
  }

  void SetGlobalPregMask(uint64 mask) {
    globalPregMask = mask;
  }

  uint64 GetLocalPregMask() const {
    return localPregMask;
  }

  void SetLocalPregMask(uint64 mask) {
    localPregMask = mask;
  }

 private:
  MapleMap<regno_t, uint16> defCnt;
  MapleMap<regno_t, uint16> useCnt;
  MapleSet<regno_t> globalPreg;
  uint64 globalPregMask = 0;  /* global phys reg used in bb */
  uint64 localPregMask  = 0;  /* local phys reg used in bb */
};

/* For each bb, record info pertain to allocation */
class BBAssignInfo {
 public:
  explicit BBAssignInfo(MapleAllocator &allocator)
      : globalsAssigned(allocator.Adapter()),
        regMap(allocator.Adapter()) {}

  ~BBAssignInfo() = default;

  uint32 GetIntLocalRegsNeeded() const {
    return intLocalRegsNeeded;
  }

  void SetIntLocalRegsNeeded(uint32 num) {
    intLocalRegsNeeded = num;
  }

  uint32 GetFpLocalRegsNeeded() const {
    return fpLocalRegsNeeded;
  }

  void SetFpLocalRegsNeeded(uint32 num) {
    fpLocalRegsNeeded = num;
  }

  void InitGlobalAssigned() {
    globalsAssigned.clear();
    globalsAssigned.resize(kMaxRegNum);
  }

  bool GetGlobalsAssigned(regno_t regNO) const {
    return globalsAssigned[regNO];
  }

  void InsertElemToGlobalsAssigned(regno_t regNO) {
    globalsAssigned[regNO] = true;
  }

  void EraseElemToGlobalsAssigned(regno_t regNO) {
    globalsAssigned[regNO] = false;
  }

  const MapleMap<regno_t, regno_t> &GetRegMap() const {
    return regMap;
  }

  bool HasRegMap(regno_t regNOKey) const  {
    return (regMap.find(regNOKey) != regMap.end());
  }

  regno_t GetRegMapElem(regno_t regNO) {
    return regMap[regNO];
  }

  void SetRegMapElem(regno_t regNOKey, regno_t regNOValue) {
    regMap[regNOKey] = regNOValue;
  }

 private:
  uint32 intLocalRegsNeeded = 0;      /* num local reg needs for each bb */
  uint32 fpLocalRegsNeeded  = 0;      /* num local reg needs for each bb */
  MapleVector<bool> globalsAssigned;  /* globals used in a bb */
  MapleMap<regno_t, regno_t> regMap;  /* local vreg to preg mapping */
};

class FinalizeRegisterInfo {
 public:
  explicit FinalizeRegisterInfo(MapleAllocator &allocator)
      : defOperands(allocator.Adapter()),
        defIdx(allocator.Adapter()),
        useOperands(allocator.Adapter()),
        useIdx(allocator.Adapter()) {}

  ~FinalizeRegisterInfo() = default;
  void ClearInfo() {
    memOperandIdx = 0;
    baseOperand = nullptr;
    offsetOperand = nullptr;
    defOperands.clear();
    defIdx.clear();
    useOperands.clear();
    useIdx.clear();
  }

  void SetBaseOperand(Operand &opnd, const int32 idx) {
    baseOperand = &opnd;
    memOperandIdx = idx;
  }

  void SetOffsetOperand(Operand &opnd) {
    offsetOperand = &opnd;
  }

  void SetDefOperand(Operand &opnd, const int32 idx) {
    defOperands.push_back(&opnd);
    defIdx.push_back(idx);
  }

  void SetUseOperand(Operand &opnd, const int32 idx) {
    useOperands.push_back(&opnd);
    useIdx.push_back(idx);
  }

  int32 GetMemOperandIdx() const {
    return memOperandIdx;
  }

  const Operand *GetBaseOperand() const {
    return baseOperand;
  }

  const Operand *GetOffsetOperand() const {
    return offsetOperand;
  }

  size_t GetDefOperandsSize() const {
    return defOperands.size();
  }

  const Operand *GetDefOperandsElem(size_t index) const {
    return defOperands[index];
  }

  int32 GetDefIdxElem(size_t index) const {
    return defIdx[index];
  }

  size_t GetUseOperandsSize() const {
    return useOperands.size();
  }

  const Operand *GetUseOperandsElem(size_t index) const {
    return useOperands[index];
  }

  int32 GetUseIdxElem(size_t index) const {
    return useIdx[index];
  }

 private:
  int32 memOperandIdx = 0;
  Operand *baseOperand = nullptr;
  Operand *offsetOperand = nullptr;
  MapleVector<Operand*> defOperands;
  MapleVector<int32> defIdx;
  MapleVector<Operand*> useOperands;
  MapleVector<int32> useIdx;
};

class LocalRegAllocator {
 public:
  LocalRegAllocator(CGFunc &cgFunc, MapleAllocator &allocator)
      : intRegAssignmentMap(allocator.Adapter()),
        fpRegAssignmentMap(allocator.Adapter()),
        useInfo(allocator.Adapter()),
        defInfo(allocator.Adapter()) {
    buckets = (cgFunc.GetMaxRegNum() / kU64) + 1;
    intRegAssigned = cgFunc.GetMemoryPool()->NewArray<uint64>(buckets);
    fpRegAssigned = cgFunc.GetMemoryPool()->NewArray<uint64>(buckets);
    intRegSpilled = cgFunc.GetMemoryPool()->NewArray<uint64>(buckets);
    fpRegSpilled = cgFunc.GetMemoryPool()->NewArray<uint64>(buckets);
  }

  ~LocalRegAllocator() = default;

  void ClearLocalRaInfo() {
    ClearBitArrElement(intRegAssigned);
    ClearBitArrElement(fpRegAssigned);
    intRegAssignmentMap.clear();
    fpRegAssignmentMap.clear();
    intPregUsed = 0;
    fpPregUsed = 0;
    ClearBitArrElement(intRegSpilled);
    ClearBitArrElement(fpRegSpilled);
    numIntPregUsed = 0;
    numFpPregUsed = 0;
  }

  regno_t RegBaseUpdate(bool isInt) const {
    return isInt ? 0 : V0 - R0;
  }

  bool IsInRegAssigned(regno_t regNO, bool isInt) const {
    uint64 *regAssigned = nullptr;
    if (isInt) {
      regAssigned = intRegAssigned;
    } else {
      regAssigned = fpRegAssigned;
    }
    return IsBitArrElemSet(regAssigned, regNO);;
  }

  void SetRegAssigned(regno_t regNO, bool isInt) {
    if (isInt) {
      SetBitArrElement(intRegAssigned, regNO);
    } else {
      SetBitArrElement(fpRegAssigned, regNO);
    }
  }

  regno_t GetRegAssignmentItem(bool isInt, regno_t regKey) {
    return isInt ? intRegAssignmentMap[regKey] : fpRegAssignmentMap[regKey];
  }

  void SetRegAssignmentMap(bool isInt, regno_t regKey, regno_t regValue) {
    if (isInt) {
      intRegAssignmentMap[regKey] = regValue;
    } else {
      fpRegAssignmentMap[regKey] = regValue;
    }
  }

  /* only for HandleLocalRaDebug */
  uint64 GetPregUsed(bool isInt) const {
    if (isInt) {
      return intPregUsed;
    } else {
      return fpPregUsed;
    }
  }

  void SetPregUsed(regno_t regNO, bool isInt) {
    uint64 mask = 0;
    if (isInt) {
      mask = 1ULL << (regNO - R0);
      if ((intPregUsed & mask) == 0) {
        ++numIntPregUsed;
        intPregUsed |= mask;
      }
    } else {
      mask = 1ULL << (regNO - V0);
      if ((fpPregUsed & mask) == 0) {
        ++numFpPregUsed;
        fpPregUsed |= mask;
      }
    }
  }

  bool isInRegSpilled(regno_t regNO, bool isInt) const {
    bool isSet;
    if (isInt) {
      isSet = IsBitArrElemSet(intRegSpilled, regNO);
    } else {
      isSet = IsBitArrElemSet(fpRegSpilled, regNO);
    }
    return isSet;
  }

  void SetRegSpilled(regno_t regNO, bool isInt) {
    if (isInt) {
      SetBitArrElement(intRegSpilled, regNO);
    } else {
      SetBitArrElement(fpRegSpilled, regNO);
    }
  }

  uint64 GetPregs(bool isInt) const {
    if (isInt) {
      return intPregs;
    } else {
      return fpPregs;
    }
  }

  void SetPregs(regno_t regNO, bool isInt) {
    if (isInt) {
      intPregs |= 1ULL << (regNO - RegBaseUpdate(true));
    } else {
      fpPregs |= 1ULL << (regNO - RegBaseUpdate(false));
    }
  }

  void ClearPregs(regno_t regNO, bool isInt) {
    if (isInt) {
      intPregs &= ~(1ULL << (regNO - RegBaseUpdate(true)));
    } else {
      fpPregs &= ~(1ULL << (regNO - RegBaseUpdate(false)));
    }
  }

  bool IsPregAvailable(regno_t regNO, bool isInt) const {
    bool isAvailable;
    if (isInt) {
      isAvailable = intPregs & (1ULL << (regNO - RegBaseUpdate(true)));
    } else {
      isAvailable = fpPregs & (1ULL << (regNO - RegBaseUpdate(false)));
    }
    return isAvailable;
  }

  void InitPregs(uint32 intMax, uint32 fpMax, bool hasYield, const MapleSet<uint32> &intSpillRegSet,
                 const MapleSet<uint32> &fpSpillRegSet) {
    uint32 intBase = R0;
    uint32 fpBase = V0;
    intPregs = (1ULL << (intMax + 1)) - 1;
    fpPregs = (1ULL << (((fpMax + 1) + fpBase) - RegBaseUpdate(false))) - 1;
    for (uint32 regNO : intSpillRegSet) {
      ClearPregs(regNO + intBase, true);
    }
    for (uint32 regNO : fpSpillRegSet) {
      ClearPregs(regNO + fpBase, false);
    }
    if (hasYield) {
      ClearPregs(RYP, true);
    }
#ifdef RESERVED_REGS
    intPregs &= ~(1ULL << R16);
    intPregs &= ~(1ULL << R17);
#endif  /* RESERVED_REGS */
  }

  const MapleMap<regno_t, regno_t> &GetIntRegAssignmentMap() const {
    return intRegAssignmentMap;
  }

  const MapleMap<regno_t, regno_t> &GetFpRegAssignmentMap() const {
    return fpRegAssignmentMap;
  }

  const MapleMap<regno_t, uint16> &GetUseInfo() const {
    return useInfo;
  }

  void SetUseInfoElem(regno_t regNO, uint16 info) {
    useInfo[regNO] = info;
  }

  void IncUseInfoElem(regno_t regNO) {
    if (useInfo.find(regNO) != useInfo.end()) {
      ++useInfo[regNO];
    }
  }

  uint16 GetUseInfoElem(regno_t regNO) {
    return useInfo[regNO];
  }

  void ClearUseInfo() {
    useInfo.clear();
  }

  const MapleMap<regno_t, uint16> &GetDefInfo() const {
    return defInfo;
  }

  void SetDefInfoElem(regno_t regNO, uint16 info) {
    defInfo[regNO] = info;
  }

  uint16 GetDefInfoElem(regno_t regNO) {
    return defInfo[regNO];
  }

  void IncDefInfoElem(regno_t regNO) {
    if (defInfo.find(regNO) != defInfo.end()) {
      ++defInfo[regNO];
    }
  }

  void ClearDefInfo() {
    defInfo.clear();
  }

  uint32 GetNumIntPregUsed() const {
    return numIntPregUsed;
  }

  uint32 GetNumFpPregUsed() const {
    return numFpPregUsed;
  }

 private:
  void ClearBitArrElement(uint64 *vec) {
    for (uint32 i = 0; i < buckets; ++i) {
      vec[i] = 0UL;
    }
  }

  void SetBitArrElement(uint64 *vec, regno_t regNO) {
    uint32 index = regNO / kU64;
    uint64 bit = regNO % kU64;
    vec[index] |= 1ULL << bit;
  }

  /* The following local vars keeps track of allocation information in bb. */
  uint64 *intRegAssigned;  /* in this set if vreg is assigned */
  uint64 *fpRegAssigned;
  MapleMap<regno_t, regno_t> intRegAssignmentMap;  /* vreg -> preg map, which preg is the vreg assigned */
  MapleMap<regno_t, regno_t> fpRegAssignmentMap;
  uint64 intPregUsed = 0;  /* pregs used in bb */
  uint64 fpPregUsed  = 0;
  uint64 *intRegSpilled;   /* on this list if vreg is spilled */
  uint64 *fpRegSpilled;

  uint64 intPregs = 0;     /* available regs for assignement */
  uint64 fpPregs  = 0;
  MapleMap<regno_t, uint16> useInfo;  /* copy of local ra info for useCnt */
  MapleMap<regno_t, uint16> defInfo;  /* copy of local ra info for defCnt */

  uint32 numIntPregUsed = 0;
  uint32 numFpPregUsed = 0;
  uint32 buckets;
};

class SplitBBInfo {
 public:
  SplitBBInfo() = default;

  ~SplitBBInfo() = default;

  BB *GetCandidateBB() {
    return candidateBB;
  }

  const BB *GetCandidateBB() const {
    return candidateBB;
  }

  const BB *GetStartBB() const {
    return startBB;
  }

  void SetCandidateBB(BB &bb) {
    candidateBB = &bb;
  }

  void SetStartBB(BB &bb) {
    startBB = &bb;
  }

 private:
  BB *candidateBB = nullptr;
  BB *startBB = nullptr;
};

class GraphColorRegAllocator : public AArch64RegAllocator {
 public:
  GraphColorRegAllocator(CGFunc &cgFunc, MemPool &memPool)
      : AArch64RegAllocator(cgFunc, memPool),
        bbVec(alloc.Adapter()),
        vregLive(alloc.Adapter()),
        pregLive(alloc.Adapter()),
        lrVec(alloc.Adapter()),
        localRegVec(alloc.Adapter()),
        bbRegInfo(alloc.Adapter()),
        unconstrained(alloc.Adapter()),
        constrained(alloc.Adapter()),
#ifdef OPTIMIZE_FOR_PROLOG
        intDelayed(alloc.Adapter()),
        fpDelayed(alloc.Adapter()),
#endif  /* OPTIMIZE_FOR_PROLOG */
        intCallerRegSet(alloc.Adapter()),
        intCalleeRegSet(alloc.Adapter()),
        intSpillRegSet(alloc.Adapter()),
        fpCallerRegSet(alloc.Adapter()),
        fpCalleeRegSet(alloc.Adapter()),
        fpSpillRegSet(alloc.Adapter()),
        intCalleeUsed(alloc.Adapter()),
        fpCalleeUsed(alloc.Adapter()) {
    numVregs = cgFunc.GetMaxVReg();
    lrVec.resize(numVregs);
    localRegVec.resize(cgFunc.NumBBs());
    bbRegInfo.resize(cgFunc.NumBBs());
  }

  ~GraphColorRegAllocator() override = default;

  bool AllocateRegisters() override;
  std::string PhaseName() const {
    return "regalloc";
  }

 private:
  struct SetLiveRangeCmpFunc {
    bool operator()(const LiveRange *lhs, const LiveRange *rhs) const {
      if (lhs->GetPriority() == rhs->GetPriority()) {
        /*
         * This is to ensure the ordering is consistent as the reg#
         * differs going through VtableImpl.mpl file.
         */
        if (lhs->GetID() == rhs->GetID()) {
          return lhs->GetRegNO() < rhs->GetRegNO();
        } else {
          return lhs->GetID() < rhs->GetID();
        }
      }
      return (lhs->GetPriority() > rhs->GetPriority());
    }
  };

  template <typename Func>
  void ForEachBBArrElem(const uint64 *vec, Func functor) const;

  template <typename Func>
  void ForEachBBArrElemWithInterrupt(const uint64 *vec, Func functor) const;

  template <typename Func>
  void ForEachRegArrElem(const uint64 *vec, Func functor) const;

  void PrintLiveUnitMap(const LiveRange &lr) const;
  void PrintLiveRangeConflicts(const LiveRange &lr) const;
  void PrintLiveBBBit(const LiveRange &li) const;
  void PrintLiveRange(const LiveRange &li, const std::string &str) const;
  void PrintLiveRanges() const;
  void PrintLocalRAInfo(const std::string &str) const;
  void PrintBBAssignInfo() const;
  void PrintBBs() const;

  uint32 MaxIntPhysRegNum() const;
  uint32 MaxFloatPhysRegNum() const;
  bool IsReservedReg(AArch64reg regNO) const;
  void InitFreeRegPool();
  void InitCCReg();
  bool IsUnconcernedReg(regno_t regNO) const;
  bool IsUnconcernedReg(const RegOperand &regOpnd) const;
  LiveRange *NewLiveRange();
  void CalculatePriority(LiveRange &lr) const;
  bool CreateLiveRangeHandleLocal(regno_t regNO, BB &bb, bool isDef);
  LiveRange *CreateLiveRangeAllocateAndUpdate(regno_t regNO, const BB &bb, bool isDef, uint32 currId);
  bool CreateLiveRange(regno_t regNO, BB &bb, bool isDef, uint32 currPoint, bool update_cnt);
  bool SetupLiveRangeByOpHandlePhysicalReg(RegOperand &op, Insn &insn, regno_t regNO, bool isDef);
  void SetupLiveRangeByOp(Operand &op, Insn &insn, bool isDef, uint32 &numUses);
  void SetupLiveRangeByRegNO(regno_t liveOut, BB &bb, uint32 currPoint);
  bool UpdateInsnCntAndSkipUseless(Insn &insn, uint32 &currPoint);
  void UpdateCallInfo(uint32 bbId);
  void ClassifyOperand(std::set<regno_t> &pregs, std::set<regno_t> &vregs, const Operand &opnd);
  void SetOpndConflict(const Insn &insn, bool onlyDef);
  void UpdateOpndConflict(const Insn &insn, bool multiDef);
  void ComputeLiveRangesForEachDefOperand(Insn &insn, bool &multiDef);
  void ComputeLiveRangesForEachUseOperand(Insn &insn);
  void ComputeLiveRangesUpdateIfInsnIsCall(const Insn &insn);
  void ComputeLiveRangesUpdateLiveUnitInsnRange(BB &bb, uint32 currPoint);
  void UpdateRegLive(BB &bb, BB &succBB);
  void ComputeLiveOut(BB &bb);
  void ComputeLiveRanges();
  MemOperand *CreateSpillMem(uint32 spillIdx);
  bool CheckOverlap(uint64 val, uint32 &lastBitSet, uint32 &overlapNum, uint32 i) const;
  void CheckInterference(LiveRange &lr1, LiveRange &lr2) const;
  void BuildInterferenceGraphSeparateIntFp(std::vector<LiveRange*> &intLrVec, std::vector<LiveRange*> &fpLrVec);
  void BuildInterferenceGraph();
  void SetBBInfoGlobalAssigned(uint32 bbID, regno_t regNO);
  bool HaveAvailableColor(const LiveRange &lr, uint32 num) const;
  void Separate();
  void SplitAndColor();
  void ColorForOptPrologEpilog();
  bool IsLocalReg(regno_t regNO) const;
  bool IsLocalReg(LiveRange &lr) const;
  void HandleLocalRaDebug(regno_t regNO, const LocalRegAllocator &localRa, bool isInt) const;
  void HandleLocalRegAssignment(regno_t regNO, LocalRegAllocator &localRa, bool isInt);
  void UpdateLocalRegDefUseCount(regno_t regNO, LocalRegAllocator &localRa, bool isDef, bool isInt) const;
  void UpdateLocalRegConflict(regno_t regNO, LocalRegAllocator &localRa, bool isInt);
  void HandleLocalReg(Operand &op, LocalRegAllocator &localRa, const BBAssignInfo *bbInfo, bool isDef, bool isInt);
  void LocalRaRegSetEraseReg(LocalRegAllocator &localRa, regno_t regNO);
  bool LocalRaInitRegSet(LocalRegAllocator &localRa, uint32 bbId);
  void LocalRaInitAllocatableRegs(LocalRegAllocator &localRa, uint32 bbId);
  void LocalRaForEachDefOperand(const Insn &insn, LocalRegAllocator &localRa, const BBAssignInfo *bbInfo);
  void LocalRaForEachUseOperand(const Insn &insn, LocalRegAllocator &localRa, const BBAssignInfo *bbInfo);
  void LocalRaPrepareBB(BB &bb, LocalRegAllocator &localRa);
  void LocalRaFinalAssignment(LocalRegAllocator &localRa, BBAssignInfo &bbInfo);
  void LocalRaDebug(BB &bb, LocalRegAllocator &localRa);
  void LocalRegisterAllocator(bool allocate);
  MemOperand *GetSpillOrReuseMem(LiveRange &lr, uint32 regSize, bool &isOutOfRange, Insn &insn, bool isDef);
  void SpillOperandForSpillPre(Insn &insn, const Operand &opnd, RegOperand &phyOpnd, uint32 spillIdx, bool needSpill);
  void SpillOperandForSpillPost(Insn &insn, const Operand &opnd, RegOperand &phyOpnd, uint32 spillIdx, bool needSpill);
  Insn *SpillOperand(Insn &insn, const Operand &opnd, bool isDef, RegOperand &phyOpnd);
  MemOperand *GetConsistentReuseMem(const uint64 *conflict, const std::set<MemOperand*> &usedMemOpnd, uint32 size,
                                    RegType regType);
  MemOperand *GetCommonReuseMem(const uint64 *conflict, const std::set<MemOperand*> &usedMemOpnd, uint32 size,
                                RegType regType);
  MemOperand *GetReuseMem(uint32 vregNO, uint32 size, RegType regType);
  MemOperand *GetSpillMem(uint32 vregNO, bool isDest, Insn &insn, AArch64reg regNO, bool &isOutOfRange);
  bool SetAvailableSpillReg(std::set<regno_t> &cannotUseReg, LiveRange &lr, uint64 &usedRegMask);
  void CollectCannotUseReg(std::set<regno_t> &cannotUseReg, LiveRange &lr, Insn &insn);
  regno_t PickRegForSpill(uint64 &usedRegMask, RegType regType, uint32 spillIdx, bool &needSpillLr);
  bool SetRegForSpill(LiveRange &lr, Insn &insn, uint32 spillIdx, uint64 &usedRegMask, bool isDef);
  bool GetSpillReg(Insn &insn, LiveRange &lr, uint32 &spillIdx, uint64 &usedRegMask, bool isDef);
  RegOperand *GetReplaceOpndForLRA(Insn &insn, const Operand &opnd, uint32 &spillIdx, uint64 &usedRegMask, bool isDef);
  RegOperand *GetReplaceOpnd(Insn &insn, const Operand &opnd, uint32 &spillIdx, uint64 &usedRegMask, bool isDef);
  void MarkCalleeSaveRegs();
  void MarkUsedRegs(Operand &opnd, BBAssignInfo *bbInfo, uint64 &usedRegMask);
  uint64 FinalizeRegisterPreprocess(BBAssignInfo *bbInfo, FinalizeRegisterInfo &fInfo, Insn &insn);
  void FinalizeRegisters();

  MapleVector<LiveRange*>::iterator GetHighPriorityLr(MapleVector<LiveRange*> &lrSet) const;
  void UpdateForbiddenForNeighbors(LiveRange &lr) const;
  void UpdatePregvetoForNeighbors(LiveRange &lr) const;
  regno_t FindColorForLr(const LiveRange &lr) const;
  bool ShouldUseCallee(LiveRange &lr, const MapleSet<regno_t> &calleeUsed,
                       const MapleVector<LiveRange*> &delayed) const;
  bool AssignColorToLr(LiveRange &lr, bool isDelayed = false);
  void PruneLrForSplit(LiveRange &lr, BB &bb, bool remove, std::set<CGFuncLoops*, CGFuncLoopCmp> &candidateInLoop,
                       std::set<CGFuncLoops*, CGFuncLoopCmp> &defInLoop);
  bool UseIsUncovered(BB &bb, const BB &startBB);
  void FindUseForSplit(LiveRange &lr, SplitBBInfo &bbInfo, bool &remove,
                       std::set<CGFuncLoops*, CGFuncLoopCmp> &candidateInLoop,
                       std::set<CGFuncLoops*, CGFuncLoopCmp> &defInLoop);
  void FindBBSharedInSplit(LiveRange &lr, std::set<CGFuncLoops*, CGFuncLoopCmp> &candidateInLoop,
                           std::set<CGFuncLoops*, CGFuncLoopCmp> &defInLoop);
  void ComputeBBForNewSplit(LiveRange &newLr, LiveRange &oldLr);
  void ClearLrBBFlags(const std::set<BB*, SortedBBCmpFunc> &member);
  void ComputeBBForOldSplit(LiveRange &newLr, LiveRange &oldLr);
  bool LrCanBeColored(LiveRange &lr, BB &bbAdded, std::set<regno_t> &conflictRegs);
  void MoveLrBBInfo(LiveRange &oldLr, LiveRange &newLr, BB &bb);
  bool ContainsLoop(const CGFuncLoops &loop, const std::set<CGFuncLoops*, CGFuncLoopCmp> &loops) const;
  void GetAllLrMemberLoops(LiveRange &lr, std::set<CGFuncLoops*, CGFuncLoopCmp> &loop);
  bool SplitLrShouldSplit(LiveRange &lr);
  bool SplitLrFindCandidateLr(LiveRange &lr, LiveRange &newLr, std::set<regno_t> &conflictRegs);
  void SplitLrHandleLoops(LiveRange &lr, LiveRange &newLr, const std::set<CGFuncLoops*, CGFuncLoopCmp> &oldLoops,
                          const std::set<CGFuncLoops*, CGFuncLoopCmp> &newLoops);
  void SplitLrFixNewLrCallsAndRlod(LiveRange &newLr, const std::set<CGFuncLoops*, CGFuncLoopCmp> &origLoops);
  void SplitLrFixOrigLrCalls(LiveRange &lr);
  void SplitLrUpdateInterference(LiveRange &lr);
  void SplitLrUpdateRegInfo(LiveRange &origLr, LiveRange &newLr, std::set<regno_t> &conflictRegs);
  void SplitLrErrorCheckAndDebug(LiveRange &origLr);
  void SplitLr(LiveRange &lr);

  static constexpr uint16 kMaxUint16 = 0x7fff;

  MapleVector<BB*> bbVec;
  MapleSet<regno_t> vregLive;
  MapleSet<regno_t> pregLive;
  MapleVector<LiveRange*> lrVec;
  MapleVector<LocalRaInfo*> localRegVec;  /* local reg info for each bb, no local reg if null */
  MapleVector<BBAssignInfo*> bbRegInfo;   /* register assignment info for each bb */
  MapleVector<LiveRange*> unconstrained;
  MapleVector<LiveRange*> constrained;
#ifdef OPTIMIZE_FOR_PROLOG
  MapleVector<LiveRange*> intDelayed;
  MapleVector<LiveRange*> fpDelayed;
#endif                               /* OPTIMIZE_FOR_PROLOG  */
  MapleSet<uint32> intCallerRegSet;  /* integer caller saved */
  MapleSet<uint32> intCalleeRegSet;  /*         callee       */
  MapleSet<uint32> intSpillRegSet;   /*         spill        */
  MapleSet<uint32> fpCallerRegSet;   /* float caller saved   */
  MapleSet<uint32> fpCalleeRegSet;   /*       callee         */
  MapleSet<uint32> fpSpillRegSet;    /*       spill          */
  MapleSet<regno_t> intCalleeUsed;
  MapleSet<regno_t> fpCalleeUsed;

  uint32 bbBuckets = 0;   /* size of bit array for bb (each bucket == 64 bits) */
  uint32 regBuckets = 0;  /* size of bit array for reg (each bucket == 64 bits) */
  uint32 intRegNum = 0;   /* total available int preg */
  uint32 fpRegNum = 0;    /* total available fp preg */
  uint32 numVregs = 0;    /* number of vregs when starting */
  regno_t ccReg = 0;
  /* For spilling of spill register if there are none available
   *   Example, all 3 operands spilled
   *                          sp_reg1 -> [spillMemOpnds[1]]
   *                          sp_reg2 -> [spillMemOpnds[2]]
   *                          ld sp_reg1 <- [addr-reg2]
   *                          ld sp_reg2 <- [addr-reg3]
   *   reg1 <- reg2, reg3     sp_reg1 <- sp_reg1, sp_reg2
   *                          st sp_reg1 -> [addr-reg1]
   *                          sp_reg1 <- [spillMemOpnds[1]]
   *                          sp_reg2 <- [spillMemOpnds[2]]
   */
  static constexpr size_t kSpillMemOpndNum = 4;
  std::array<MemOperand*, kSpillMemOpndNum> spillMemOpnds = { nullptr };
  bool needExtraSpillReg = false;
};
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_COLOR_RA_H */
