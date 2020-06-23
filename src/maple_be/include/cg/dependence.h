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
#ifndef MAPLEBE_INCLUDE_CG_DEPENDENCE_H
#define MAPLEBE_INCLUDE_CG_DEPENDENCE_H

#include "deps.h"
#include "cgbb.h"

namespace maplebe {
using namespace maple;
namespace {
constexpr maple::uint32 kMaxDependenceNum = 200;
};


class DepAnalysis {
 public:
  DepAnalysis(CGFunc &func, MemPool &memPool, MAD &mad, bool beforeRA)
      : cgFunc(func), memPool(memPool), alloc(&memPool), beforeRA(beforeRA), mad(mad),
        lastComments(alloc.Adapter()) {}

  virtual ~DepAnalysis() = default;

  virtual void Run(BB &bb, MapleVector<DepNode*> &nodes) = 0;

  const MapleVector<Insn*> &GetLastComments() const {
    return lastComments;
  }
  virtual void CombineClinit(DepNode &firstNode, DepNode &secondNode, bool isAcrossSeparator) = 0;
  virtual void CombineDependence(DepNode &firstNode, DepNode &secondNode, bool isAcrossSeparator,
                                 bool isMemCombine = false) = 0;
  virtual void CombineMemoryAccessPair(DepNode &firstNode, DepNode &secondNode, bool useFirstOffset) = 0;

  virtual const std::string &GetDepTypeName(DepType depType) const = 0;
  virtual void DumpDepNode(DepNode &node) const = 0;
  virtual void DumpDepLink(DepLink &link, const DepNode *node) const = 0;

 protected:
  CGFunc &cgFunc;
  MemPool &memPool;
  MapleAllocator alloc;
  bool beforeRA;
  MAD &mad;
  MapleVector<Insn*> lastComments;

  virtual void Init(BB &bb, MapleVector<DepNode*> &nodes) = 0;
  virtual void ClearAllDepData() = 0;
  virtual void AnalysisAmbiInsns(BB &bb) = 0;
  virtual void AppendRegUseList(Insn &insn, regno_t regNO) = 0;
  virtual void AddDependence(DepNode &fromNode, DepNode &toNode, DepType depType) = 0;
  virtual void RemoveSelfDeps(Insn &insn) = 0;
  virtual void BuildDepsUseReg(Insn &insn, regno_t regNO) = 0;
  virtual void BuildDepsDefReg(Insn &insn, regno_t regNO) = 0;
  virtual void BuildDepsAmbiInsn(Insn &insn) = 0;
  virtual void BuildDepsMayThrowInsn(Insn &insn) = 0;
  virtual void BuildDepsUseMem(Insn &insn, MemOperand &memOpnd) = 0;
  virtual void BuildDepsDefMem(Insn &insn, MemOperand &memOpnd) = 0;
  virtual void BuildDepsMemBar(Insn &insn) = 0;
  virtual void BuildDepsSeparator(DepNode &newSepNode, MapleVector<DepNode*> &nodes) = 0;
  virtual void BuildDepsControlAll(DepNode &depNode, const MapleVector<DepNode*> &nodes) = 0;
  virtual void BuildDepsAccessStImmMem(Insn &insn, bool isDest) = 0;
  virtual void BuildCallerSavedDeps(Insn &insn) = 0;
  virtual void BuildDepsBetweenControlRegAndCall(Insn &insn, bool isDest) = 0;
  virtual void BuildStackPassArgsDeps(Insn &insn) = 0;
  virtual void BuildDepsDirtyStack(Insn &insn) = 0;
  virtual void BuildDepsUseStack(Insn &insn) = 0;
  virtual void BuildDepsDirtyHeap(Insn &insn) = 0;
  virtual DepNode *BuildSeparatorNode() = 0;
  virtual bool IfInAmbiRegs(regno_t regNO) const = 0;
  virtual bool IsFrameReg(const RegOperand&) const = 0;
};
}

#endif  /* MAPLEBE_INCLUDE_CG_DEPENDENCE_H */
