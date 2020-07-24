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
#ifndef MAPLEBE_INCLUDE_CG_EMIT_H
#define MAPLEBE_INCLUDE_CG_EMIT_H

/* C++ headers */
#include <fstream>
#include <functional>
#include <climits>
#include <map>
#include <array>
#include "isa.h"
#include "lsda.h"
#include "asm_info.h"
#include "cg.h"

/* Maple IR headers */
#include "mir_module.h"
#include "mir_const.h"
#include "mempool_allocator.h"
#include "muid_replacement.h"
#include "namemangler.h"

namespace maplebe {
constexpr int32 kSizeOfDecoupleStaticStruct = 4;
constexpr uint32 kHugeSoInsnCountThreshold = 0x1f00000; /* 124M (4bytes per Insn), leave 4M rooms for 128M */
constexpr char kHugeSoPostFix[] = "$$hugeso_";
class StructEmitInfo {
 public:
  /* default ctor */
  StructEmitInfo() = default;

  ~StructEmitInfo() = default;

  uint8 GetNextFieldOffset() const {
    return nextFieldOffset;
  }

  void SetNextFieldOffset(uint8 offset) {
    nextFieldOffset = offset;
  }

  void IncreaseNextFieldOffset(uint8 value) {
    nextFieldOffset += value;
  }

  uint8 GetCombineBitFieldWidth() const {
    return combineBitFieldWidth;
  }

  void SetCombineBitFieldWidth(uint8 offset) {
    combineBitFieldWidth = offset;
  }

  void IncreaseCombineBitFieldWidth(uint8 value) {
    combineBitFieldWidth += value;
  }

  void DecreaseCombineBitFieldWidth(uint8 value) {
    combineBitFieldWidth -= value;
  }

  uint64 GetCombineBitFieldValue() const {
    return combineBitFieldValue;
  }

  void SetCombineBitFieldValue(uint64 value) {
    combineBitFieldValue = value;
  }

  uint64 GetTotalSize() const {
    return totalSize;
  }

  void SetTotalSize(uint64 value) {
    totalSize = value;
  }

  void IncreaseTotalSize(uint64 value) {
    totalSize += value;
  }

 private:
  /* Next field offset in struct. */
  uint8 nextFieldOffset = 0;
  uint8 combineBitFieldWidth = 0;
  uint64 combineBitFieldValue = 0;
  /* Total size emitted in current struct. */
  uint64 totalSize = 0;
};

class FuncEmitInfo {
 public:
  CGFunc &GetCGFunc() {
    return cgFunc;
  }

  const CGFunc &GetCGFunc() const {
    return cgFunc;
  }

 protected:
  explicit FuncEmitInfo(CGFunc &func) : cgFunc(func) {}
  ~FuncEmitInfo() = default;

 private:
  CGFunc &cgFunc;
};

class Emitter {
 public:
  void CloseOutput() {
    if (outStream.is_open()) {
      outStream.close();
    }
    rangeIdx2PrefixStr.clear();
    hugeSoTargets.clear();
  }

  MOperator GetCurrentMOP() const {
    return currentMop;
  }

  void SetCurrentMOP(const MOperator &mOp) {
    currentMop = mOp;
  }

  void EmitAsmLabel(AsmLabel label);
  void EmitAsmLabel(const MIRSymbol &mirSymbol, AsmLabel label);
  void EmitFileInfo(const std::string &fileName);
  /* a symbol start/end a block */
  void EmitBlockMarker(const std::string &markerName, const std::string &sectionName,
                       bool withAddr, const std::string &addrName = "");
  void EmitNullConstant(uint32 size);
  void EmitCombineBfldValue(StructEmitInfo &structEmitInfo);
  void EmitBitFieldConstant(StructEmitInfo &structEmitInfo, MIRConst &mirConst, const MIRType *nextType,
                            uint64 fieldOffset);
  void EmitScalarConstant(MIRConst &mirConst, bool newLine = true, bool flag32 = false);
  void EmitStrConstant(const MIRStrConst &mirStrConst);
  void EmitStr16Constant(const MIRStr16Const &mirStr16Const);
  void EmitIntConst(const MIRSymbol &mirSymbol, MIRAggConst &aggConst, uint32 itabConflictIndex,
                    const std::map<GStrIdx, MIRType*> &strIdx2Type, size_t idx);
  void EmitAddrofFuncConst(const MIRSymbol &mirSymbol, MIRConst &elemConst, size_t idx);
  void EmitAddrofSymbolConst(const MIRSymbol &mirSymbol, MIRConst &elemConst, size_t idx);
  void EmitConstantTable(const MIRSymbol &mirSymbol, MIRConst &ct, const std::map<GStrIdx, MIRType*> &strIdx2Type);
  void EmitClassInfoSequential(const MIRSymbol &mirSymbol, const std::map<GStrIdx, MIRType*> &strIdx2Type,
                               const std::string &sectionName);
  void EmitMethodFieldSequential(const MIRSymbol &mirSymbol, const std::map<GStrIdx, MIRType*> &strIdx2Type,
                                 const std::string &sectionName);
  void EmitLiterals(std::vector<std::pair<MIRSymbol*, bool>> &literals,
                    const std::map<GStrIdx, MIRType*> &strIdx2Type);
  void EmitFuncLayoutInfo(const MIRSymbol &layout);
  void EmitGlobalVars(std::vector<std::pair<MIRSymbol*, bool>> &globalVars);
  void EmitGlobalVar(const MIRSymbol &globalVar);
  void EmitStaticFields(const std::vector<MIRSymbol*> &fields);
  void EmitLiteral(const MIRSymbol &literal, const std::map<GStrIdx, MIRType*> &strIdx2Type);
  void GetHotAndColdMetaSymbolInfo(const std::vector<MIRSymbol*> &mirSymbolVec,
                                   std::vector<MIRSymbol*> &hotFieldInfoSymbolVec,
                                   std::vector<MIRSymbol*> &coldFieldInfoSymbolVec, const std::string &prefixStr,
                                   bool forceCold = false);
  void EmitMetaDataSymbolWithMarkFlag(const std::vector<MIRSymbol*> &mirSymbolVec,
                                      const std::map<GStrIdx, MIRType*> &strIdx2Type,
                                      const std::string &prefixStr, const std::string &sectionName,
                                      bool isHotFlag);
  void EmitMethodDeclaringClass(const MIRSymbol &mirSymbol, const std::string &sectionName);
  void MarkVtabOrItabEndFlag(const std::vector<MIRSymbol*> &mirSymbolVec);
  void EmitArrayConstant(MIRConst &mirConst);
  void EmitStructConstant(MIRConst &mirConst);
  void EmitGlobalVariable();
  void EmitGlobalRootList(const MIRSymbol &mirSymbol);
  void EmitMuidTable(const std::vector<MIRSymbol*> &vec, const std::map<GStrIdx, MIRType*> &strIdx2Type,
                     const std::string &sectionName);
  MIRAddroffuncConst *GetAddroffuncConst(const MIRSymbol &st, MIRAggConst &aggConst);
  int64 GetFieldOffsetValue(const std::string &className, const MIRIntConst &intConst,
                            const std::map<GStrIdx, MIRType*> &strIdx2Type);

  Emitter &Emit(int64 val) {
    outStream << val;
    return *this;
  }

  Emitter &Emit(const MapleString &str) {
    ASSERT(str.c_str() != nullptr, "nullptr check");
    outStream << str;
    return *this;
  }

  Emitter &Emit(const std::string &str) {
    outStream << str;
    return *this;
  }

  void EmitLabelRef(const std::string &name, LabelIdx labIdx);
  void EmitStmtLabel(const std::string &name, LabelIdx labIdx);
  void EmitLabelPair(const std::string &name, const LabelPair &pairLabel);

  /* Emit signed/unsigned integer literals in decimal or hexadecimal */
  void EmitDecSigned(int64 num);
  void EmitDecUnsigned(uint64 num);
  void EmitHexUnsigned(uint64 num);
  void EmitDIHeader();
  void EmitDIFooter();
  void EmitDIHeaderFileInfo();

#if 1 /* REQUIRE TO SEPERATE TARGAARCH64 TARGARM32 */
/* Following code is under TARGAARCH64 condition */
  void EmitHugeSoRoutines(bool lastRoutine = false);

  uint64 GetJavaInsnCount() const {
    return javaInsnCount;
  }

  uint64 GetFuncInsnCount() const {
    return funcInsnCount;
  }

  void ClearFuncInsnCount() {
    funcInsnCount = 0;
  }

  void IncreaseJavaInsnCount(uint64 n = 1, bool alignToQuad = false) {
    if (alignToQuad) {
      javaInsnCount = (javaInsnCount + 1) & (~0x1UL);
      funcInsnCount = (funcInsnCount + 1) & (~0x1UL);
    }
    javaInsnCount += n;
    funcInsnCount += n;
#ifdef EMIT_INSN_COUNT
    Emit(" /* InsnCount: ");
    Emit(javaInsnCount *);
    Emit("*/ ");
#endif
  }

  bool NeedToDealWithHugeSo() const {
    return javaInsnCount > kHugeSoInsnCountThreshold;
  }

  std::string HugeSoPostFix() {
    return std::string(kHugeSoPostFix) + std::to_string(hugeSoSeqence);
  }

  void InsertHugeSoTarget(const std::string &target) {
    (void)hugeSoTargets.insert(target);
  }
#endif

 protected:
  Emitter(CG &cg, const std::string &fileName)
      : cg(&cg),
        rangeIdx2PrefixStr(cg.GetMIRModule()->GetMPAllocator().Adapter()),
        hugeSoTargets(cg.GetMIRModule()->GetMPAllocator().Adapter()) {
    outStream.open(fileName, std::ios::trunc);
    MIRModule &mirModule = *cg.GetMIRModule();
    memPool = mirModule.GetMemPool();
    asmInfo = memPool->New<AsmInfo>(*memPool);
  }

  ~Emitter() = default;

 private:
  AsmLabel GetTypeAsmInfoName(PrimType primType) const;
  void EmitDWRef(const std::string &name);
  void InitRangeIdx2PerfixStr();
  void EmitAddressString(const std::string &address);

  CG *cg;
  MOperator currentMop = UINT_MAX;
  MapleUnorderedMap<int, std::string> rangeIdx2PrefixStr;
  const AsmInfo *asmInfo;
  std::ofstream outStream;
  MemPool *memPool;
#if 1/* REQUIRE TO SEPERATE TARGAARCH64 TARGARM32 */
/* Following code is under TARGAARCH64 condition */
  uint64 javaInsnCount = 0;
  uint64 funcInsnCount = 0;
  MapleSet<std::string> hugeSoTargets;
  uint32 hugeSoSeqence = 2;
#endif
};
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_EMIT_H */
