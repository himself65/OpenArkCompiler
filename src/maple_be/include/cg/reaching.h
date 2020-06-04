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
#ifndef MAPLEBE_INCLUDE_CG_REACHING_H
#define MAPLEBE_INCLUDE_CG_REACHING_H

#include "cg_phase.h"
#include "cgbb.h"
#include "datainfo.h"

namespace maplebe {
enum VisitStatus : uint8 {
  kNotVisited,
  kNormalVisited,
  kEHVisited
};

enum AnalysisType : uint8 {
  kRDRegAnalysis = 1,
  kRDMemAnalysis = 2,
  kRDAllAnalysis = 3
};

enum DumpType : uint32 {
  kDumpAll = 0xFFF,
  kDumpRegGen = 0x001,
  kDumpRegUse = 0x002,
  kDumpRegIn = 0x004,
  kDumpRegOut = 0x008,
  kDumpMemGen = 0x010,
  kDumpMemIn = 0x020,
  kDumpMemOut = 0x040,
  kDumpMemUse = 0x080,
  kDumpBBCGIR = 0x100
};

class ReachingDefinition : public AnalysisResult {
 public:
  ReachingDefinition(CGFunc &func, MemPool &memPool);
  ~ReachingDefinition() override = default;
  void AnalysisStart();
  void Dump(uint32) const;
  void DumpInfo(const BB&, DumpType) const;
  void DumpBBCGIR(const BB&) const;
  void ClearDefUseInfo();
  void UpdateInOut(BB &changedBB);
  void UpdateInOut(BB &changedBB, bool isReg);
  void SetAnalysisMode(AnalysisType analysisMode) {
    mode = analysisMode;
  }

  bool OnlyAnalysisReg() const {
    return mode == kRDRegAnalysis;
  }

  uint32 GetMaxInsnNO() const {
    return maxInsnNO;
  }

  size_t GetRegSize(const BB &bb) const {
    return regUse[bb.GetId()]->Size();
  }

  bool CheckRegGen(const BB &bb, uint32 regNO) const {
    return regGen[bb.GetId()]->TestBit(regNO);
  }

  void EnlargeRegCapacity(uint32 size);
  bool IsFrameReg(const Operand &opnd) const;
  InsnSet FindUseForRegOpnd(Insn &insn, uint32 indexOrRegNO, bool isRegNO) const;
  bool RegIsLiveBetweenInsn(uint32 regNO, Insn &startInsn, Insn &endInsn) const;
  bool IsLiveInAllPathBB(uint32 regNO, const BB &startBB, const BB &endBB, std::vector<bool> &visitedBB) const;
  bool HasCallBetweenDefUse(const Insn &defInsn, const Insn &useInsn) const;

  virtual void InitGenUse(BB &bb, bool firstTime = true) = 0;
  virtual InsnSet FindDefForMemOpnd(Insn &insn, uint32 indexOrOffset, bool isOffset = false) const = 0;
  virtual InsnSet FindUseForMemOpnd(Insn &insn, uint8 index, bool secondMem = false) const = 0;
  virtual std::vector<Insn*> FindMemDefBetweenInsn(uint32 offset, const Insn *startInsn, Insn *endInsn) const = 0;
  virtual std::vector<Insn*> FindRegDefBetweenInsn(uint32 regNO, Insn *startInsn, Insn *endInsn) const = 0;
  virtual bool FindRegUseBetweenInsn(uint32 regNO, Insn *startInsn, Insn *endInsn, InsnSet &useInsnSet) const = 0;
  virtual bool FindMemUseBetweenInsn(uint32 offset, Insn *startInsn, const Insn *endInsn,
                                     InsnSet &useInsnSet) const = 0;
  virtual InsnSet FindDefForRegOpnd(Insn &insn, uint32 indexOrRegNO, bool isRegNO = false) const = 0;
  static constexpr int32 kWordByteNum = 4;
  static constexpr int32 kDoubleWordByteNum = 8;
  /* to save storage space, the offset of stack memory is devided by 4 and then saved in DataInfo */
  static constexpr int32 kMemZoomSize = 4;
  /* number the insn interval 3. make sure no repeated insn number when new insn inserted */
  static constexpr uint32 kInsnNoInterval = 3;

 protected:
  virtual void InitStartGen() = 0;
  virtual void InitEhDefine(BB &bb) = 0;
  virtual void GenAllCallerSavedRegs(BB &bb) = 0;
  virtual void AddRetPseudoInsn(BB &bb) = 0;
  virtual void AddRetPseudoInsns() = 0;
  virtual int32 GetStackSize() const = 0;
  virtual bool IsCallerSavedReg(uint32 regNO) const = 0;
  virtual void FindRegDefInBB(uint32 regNO, BB &bb, InsnSet &defInsnSet) const = 0;
  virtual void FindMemDefInBB(uint32 offset, BB &bb, InsnSet &defInsnSet) const = 0;
  virtual void DFSFindDefForRegOpnd(const BB &startBB, uint32 regNO, std::vector<VisitStatus> &visitedBB,
                                    InsnSet &defInsnSet) const = 0;
  virtual void DFSFindDefForMemOpnd(const BB &startBB, uint32 offset, std::vector<VisitStatus> &visitedBB,
                                    InsnSet &defInsnSet) const = 0;
  void DFSFindUseForMemOpnd(const BB &startBB, uint32 offset, std::vector<bool> &visitedBB,
                            InsnSet &useInsnSet, bool onlyFindForEhSucc) const;
  CGFunc *cgFunc;
  MapleAllocator rdAlloc;
  MapleVector<Insn*> pseudoInsns;
  AnalysisType mode = kRDRegAnalysis;
  BB *firstCleanUpBB = nullptr;
  std::vector<DataInfo*> regGen;
  std::vector<DataInfo*> regUse;
  std::vector<DataInfo*> regIn;
  std::vector<DataInfo*> regOut;
  std::vector<DataInfo*> memGen;
  std::vector<DataInfo*> memUse;
  std::vector<DataInfo*> memIn;
  std::vector<DataInfo*> memOut;
  const uint32 kMaxBBNum;
 private:
  void Initialize();
  void InitDataSize();
  void BuildInOutForFuncBody();
  void BuildInOutForCleanUpBB();
  void BuildInOutForCleanUpBB(bool isReg, const std::set<uint32> &index);
  void InitRegAndMemInfo(const BB &bb);
  void InitOut(const BB &bb);
  bool GenerateIn(const BB &bb);
  bool GenerateIn(const BB &bb, const std::set<uint32> &index, const bool isReg);
  bool GenerateOut(const BB &bb);
  bool GenerateOut(const BB &bb, const std::set<uint32> &index, const bool isReg);
  bool GenerateInForFirstCleanUpBB();
  bool GenerateInForFirstCleanUpBB(bool isReg, const std::set<uint32> &index);
  void DFSFindUseForRegOpnd(const BB &startBB, uint32 regNO, std::vector<bool> &visitedBB,
                            InsnSet &useInsnSet, bool onlyFindForEhSucc) const;
  bool RegIsUsedInOtherBB(const BB &startBB, uint32 regNO, std::vector<bool> &visitedBB) const;
  bool RegHasUsePoint(uint32 regNO, Insn &startInsn) const;
  bool CanReachEndBBFromCurrentBB(const BB &currentBB, const BB &endBB, std::vector<bool> &traversedBBSet) const;
  bool HasCallBetweenInsnInSameBB(const Insn &startInsn, const Insn &endInsn) const;
  bool HasCallInPath(const BB &startBB, const BB &endBB, std::vector<bool> &visitedBB) const;
  bool RegIsUsedInCleanUpBB(uint32 regNO) const;

  MapleSet<BB*, BBIdCmp> normalBBSet;
  MapleSet<BB*, BBIdCmp> cleanUpBBSet;
  uint32 maxInsnNO = 0;
};

CGFUNCPHASE(CgDoReachingDefinition, "reachingdefinition")
CGFUNCPHASE(CgDoClearRDInfo, "clearrdinfo")
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_REACHING_H */
