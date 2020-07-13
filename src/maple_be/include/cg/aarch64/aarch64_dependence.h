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
#ifndef MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_DEPENDENCE_H
#define MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_DEPENDENCE_H

#include "dependence.h"
#include "cgfunc.h"
#include "aarch64_operand.h"

namespace maplebe {
class AArch64DepAnalysis : public DepAnalysis {
 public:
  AArch64DepAnalysis(CGFunc &func, MemPool &mp, MAD &mad, bool beforeRA);

  ~AArch64DepAnalysis() override = default;

  void Run(BB &bb, MapleVector<DepNode*> &nodes) override;
  const std::string &GetDepTypeName(DepType depType) const override;
  void DumpDepNode(DepNode &node) const override;
  void DumpDepLink(DepLink &link, const DepNode *node) const override;

 protected:
  void Init(BB &bb, MapleVector<DepNode*> &nodes) override;
  void ClearAllDepData() override;
  void AnalysisAmbiInsns(BB &bb) override;
  void AppendRegUseList(Insn &insn, regno_t regNO) override;
  void AddDependence(DepNode& fromNode, DepNode &toNode, DepType depType) override;
  void RemoveSelfDeps(Insn &insn) override;
  void CombineClinit(DepNode &firstNode, DepNode &secondNode, bool isAcrossSeparator) override;
  void CombineDependence(DepNode &firstNode, DepNode &secondNode, bool isAcrossSeparator,
                         bool isMemCombine = false) override;
  void CombineMemoryAccessPair(DepNode &firstNode, DepNode &secondNode, bool useFirstOffset) override;
  void BuildDepsUseReg(Insn &insn, regno_t regNO) override;
  void BuildDepsDefReg(Insn &insn, regno_t regNO) override;
  void BuildDepsAmbiInsn(Insn &insn) override;
  void BuildDepsMayThrowInsn(Insn &insn) override;
  bool NeedBuildDepsMem(const AArch64MemOperand &memOpnd, const AArch64MemOperand *nextMemOpnd, Insn &memInsn) const;
  void BuildDepsUseMem(Insn &insn, MemOperand &memOpnd) override;
  void BuildDepsDefMem(Insn &insn, MemOperand &memOpnd) override;
  void BuildAntiDepsDefStackMem(Insn &insn, const AArch64MemOperand &memOpnd, const AArch64MemOperand *nextMemOpnd);
  void BuildOutputDepsDefStackMem(Insn &insn, const AArch64MemOperand &memOpnd, const AArch64MemOperand *nextMemOpnd);
  void BuildDepsMemBar(Insn &insn) override;
  void BuildDepsSeparator(DepNode &newSepNode, MapleVector<DepNode*> &nodes) override;
  void BuildDepsControlAll(DepNode &depNode, const MapleVector<DepNode*> &nodes) override;
  void BuildDepsAccessStImmMem(Insn &insn, bool isDest) override;
  void BuildCallerSavedDeps(Insn &insn) override;
  void BuildDepsBetweenControlRegAndCall(Insn &insn, bool isDest) override;
  void BuildStackPassArgsDeps(Insn &insn) override;
  void BuildDepsDirtyStack(Insn &insn) override;
  void BuildDepsUseStack(Insn &insn) override;
  void BuildDepsDirtyHeap(Insn &insn) override;
  DepNode *BuildSeparatorNode() override;
  bool IfInAmbiRegs(regno_t regNO) const override;
  bool IsFrameReg(const RegOperand&) const override;

 private:
  AArch64MemOperand *GetNextMemOperand(Insn &insn, AArch64MemOperand &aarchMemOpnd) const;
  void BuildMemOpndDependency(Insn &insn, Operand &opnd, const AArch64OpndProp &regProp);
  void BuildOpndDependency(Insn &insn);
  void BuildSpecialInsnDependency(Insn &insn, DepNode &depNode, const MapleVector<DepNode*> &nodes);
  void SeperateDependenceGraph(MapleVector<DepNode*> &nodes, uint32 &nodeSum);
  DepNode *GenerateDepNode(Insn &insn, MapleVector<DepNode*> &nodes, int32 nodeSum, const MapleVector<Insn*> &comments);
  void BuildAmbiInsnDependency(Insn &insn);
  void BuildMayThrowInsnDependency(Insn &insn);
  void UpdateRegUseAndDef(Insn &insn, DepNode &depNode, MapleVector<DepNode*> &nodes);
  void UpdateStackAndHeapDependency(DepNode &depNode, Insn &insn, const Insn &locInsn);
  AArch64MemOperand *BuildNextMemOperandByByteSize(AArch64MemOperand &aarchMemOpnd, uint32 byteSize) const;
  void AddDependence4InsnInVectorByType(MapleVector<Insn*> &insns, Insn &insn, const DepType &type);
  void AddDependence4InsnInVectorByTypeAndCmp(MapleVector<Insn*> &insns, Insn &insn, const DepType &type);
  void ReplaceDepNodeWithNewInsn(DepNode &firstNode, DepNode &secondNode, Insn& newInsn, bool isFromClinit) const;
  void ClearDepNodeInfo(DepNode &depNode) const;
  void AddEndSeparatorNode(MapleVector<DepNode*> &nodes);

  Insn **regDefs = nullptr;
  RegList **regUses = nullptr;
  Insn *memBarInsn = nullptr;
  bool hasAmbiRegs = false;
  Insn *lastCallInsn = nullptr;
  uint32 separatorIndex = 0;
  Insn *lastFrameDef = nullptr;
  MapleVector<Insn*> stackUses;
  MapleVector<Insn*> stackDefs;
  MapleVector<Insn*> heapUses;
  MapleVector<Insn*> heapDefs;
  MapleVector<Insn*> mayThrows;
  /* instructions that can not across may throw instructions. */
  MapleVector<Insn*> ambiInsns;
  /* register number that catch bb and cleanup bb uses. */
  MapleSet<regno_t> ehInRegs;
  /* the bb to be scheduling currently */
  BB *curBB = nullptr;
};
}

#endif  /* MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_DEPENDENCE_H */
