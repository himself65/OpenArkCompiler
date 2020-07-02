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
#include "aarch64_dependence.h"
#include "aarch64_cg.h"
#include "aarch64_operand.h"
#include "pressure.h"

/* For building dependence graph, The entry is AArch64DepAnalysis::Run. */
namespace maplebe {
/* constructor */
AArch64DepAnalysis::AArch64DepAnalysis(CGFunc &func, MemPool &mp, MAD &mad, bool beforeRA)
    : DepAnalysis(func, mp, mad, beforeRA), stackUses(alloc.Adapter()),
      stackDefs(alloc.Adapter()), heapUses(alloc.Adapter()),
      heapDefs(alloc.Adapter()), mayThrows(alloc.Adapter()),
      ambiInsns(alloc.Adapter()), ehInRegs(alloc.Adapter()) {
  uint32 maxRegNum;
  if (beforeRA) {
    maxRegNum = cgFunc.GetMaxVReg();
  } else {
    maxRegNum = kAllRegNum;
  }
  regDefs = memPool.NewArray<Insn*>(maxRegNum);
  regUses = memPool.NewArray<RegList*>(maxRegNum);
}

/* print dep node information */
void AArch64DepAnalysis::DumpDepNode(DepNode &node) const {
  node.GetInsn()->Dump();
  uint32 num = node.GetUnitNum();
  LogInfo::MapleLogger() << "unit num : " << num << ", ";
  for (uint32 i = 0; i < num; ++i) {
    const Unit *unit = node.GetUnitByIndex(i);
    if (unit != nullptr) {
      PRINT_VAL(unit->GetName());
    } else {
      PRINT_VAL("none");
    }
  }
  LogInfo::MapleLogger() << '\n';
  node.DumpSchedInfo();
  if (beforeRA) {
    node.DumpRegPressure();
  }
}

/* print dep link information */
void AArch64DepAnalysis::DumpDepLink(DepLink &link, const DepNode *node) const {
  PRINT_VAL(GetDepTypeName(link.GetDepType()));
  PRINT_STR_VAL("Latency: ", link.GetLatency());
  if (node != nullptr) {
    node->GetInsn()->Dump();
    return;
  }
  LogInfo::MapleLogger() << "from : ";
  link.GetFrom().GetInsn()->Dump();
  LogInfo::MapleLogger() << "to : ";
  link.GetTo().GetInsn()->Dump();
}

/* Append use register to the list. */
void AArch64DepAnalysis::AppendRegUseList(Insn &insn, regno_t regNO) {
  RegList *regList = memPool.New<RegList>();
  regList->insn = &insn;
  regList->next = nullptr;
  if (regUses[regNO] == nullptr) {
    regUses[regNO] = regList;
    if (beforeRA) {
      Insn *defInsn = regDefs[regNO];
      if (defInsn == nullptr) {
        return;
      }
      DepNode *defNode = defInsn->GetDepNode();
      defNode->SetRegDefs(regNO, regList);
    }
    return;
  }
  RegList *lastRegList = regUses[regNO];
  while (lastRegList->next != nullptr) {
    lastRegList = lastRegList->next;
  }
  lastRegList->next = regList;
}

/*
 * Add dependence edge.
 * Two dependence node has a unique edge.
 * True dependence overwirtes other dependences.
 */
void AArch64DepAnalysis::AddDependence(DepNode &fromNode, DepNode &toNode, DepType depType) {
  /* Can not build a self loop dependence. */
  if (&fromNode == &toNode) {
    return;
  }
  /* Check if exist edge. */
  if (!fromNode.GetSuccs().empty()) {
    DepLink *depLink = fromNode.GetSuccs().back();
    if (&(depLink->GetTo()) == &toNode) {
      if (depLink->GetDepType() != kDependenceTypeTrue) {
        if (depType == kDependenceTypeTrue) {
          /* Has exist edge, replace it. */
          depLink->SetDepType(kDependenceTypeTrue);
          depLink->SetLatency(mad.GetLatency(*fromNode.GetInsn(), *toNode.GetInsn()));
        }
      }
      return;
    }
  }
  DepLink *depLink = memPool.New<DepLink>(fromNode, toNode, depType);
  if (depType == kDependenceTypeTrue) {
    depLink->SetLatency(mad.GetLatency(*fromNode.GetInsn(), *toNode.GetInsn()));
  }
  fromNode.AddSucc(*depLink);
  toNode.AddPred(*depLink);
}

void AArch64DepAnalysis::AddDependence4InsnInVectorByType(MapleVector<Insn*> &insns, Insn &insn, const DepType &type) {
  for (auto anyInsn : insns) {
    AddDependence(*anyInsn->GetDepNode(), *insn.GetDepNode(), type);
  }
}

void AArch64DepAnalysis::AddDependence4InsnInVectorByTypeAndCmp(MapleVector<Insn*> &insns, Insn &insn,
                                                                const DepType &type) {
  for (auto anyInsn : insns) {
    if (anyInsn != &insn) {
      AddDependence(*anyInsn->GetDepNode(), *insn.GetDepNode(), type);
    }
  }
}

/* Remove self dependence (self loop) in dependence graph. */
void AArch64DepAnalysis::RemoveSelfDeps(Insn &insn) {
  DepNode *node = insn.GetDepNode();
  ASSERT(node->GetSuccs().back()->GetTo().GetInsn() == &insn, "Is not a self dependence.");
  ASSERT(node->GetPreds().back()->GetFrom().GetInsn() == &insn, "Is not a self dependence.");
  node->RemoveSucc();
  node->RemovePred();
}

/* Build dependences of source register operand. */
void AArch64DepAnalysis::BuildDepsUseReg(Insn &insn, regno_t regNO) {
  DepNode *node = insn.GetDepNode();
  node->AddUseReg(regNO);
  if (regDefs[regNO] != nullptr) {
    /* Build true dependences. */
    AddDependence(*regDefs[regNO]->GetDepNode(), *insn.GetDepNode(), kDependenceTypeTrue);
  }
}

/* Build dependences of destination register operand. */
void AArch64DepAnalysis::BuildDepsDefReg(Insn &insn, regno_t regNO) {
  DepNode *node = insn.GetDepNode();
  node->AddDefReg(regNO);
  /* Build anti dependences. */
  RegList *regList = regUses[regNO];
  while (regList != nullptr) {
    CHECK_NULL_FATAL(regList->insn);
    AddDependence(*regList->insn->GetDepNode(), *node, kDependenceTypeAnti);
    regList = regList->next;
  }
  /* Build output depnedence. */
  if (regDefs[regNO] != nullptr) {
    AddDependence(*regDefs[regNO]->GetDepNode(), *node, kDependenceTypeOutput);
  }
}

void AArch64DepAnalysis::ReplaceDepNodeWithNewInsn(DepNode &firstNode, DepNode &secondNode, Insn& newInsn,
                                                   bool isFromClinit) const {
  if (isFromClinit) {
    firstNode.AddClinitInsn(*firstNode.GetInsn());
    firstNode.AddClinitInsn(*secondNode.GetInsn());
    firstNode.SetCfiInsns(secondNode.GetCfiInsns());
  } else {
    for (Insn *insn : secondNode.GetCfiInsns()) {
      firstNode.AddCfiInsn(*insn);
    }
    for (Insn *insn : secondNode.GetComments()) {
      firstNode.AddComments(*insn);
    }
    secondNode.ClearComments();
  }
  firstNode.SetInsn(newInsn);
  Reservation *rev = mad.FindReservation(newInsn);
  CHECK_FATAL(rev != nullptr, "reservation is nullptr.");
  firstNode.SetReservation(*rev);
  firstNode.SetUnits(rev->GetUnit());
  firstNode.SetUnitNum(rev->GetUnitNum());
  newInsn.SetDepNode(firstNode);
}

void AArch64DepAnalysis::ClearDepNodeInfo(DepNode &depNode) const {
  Insn &insn = cgFunc.GetCG()->BuildInstruction<AArch64Insn>(MOP_pseudo_none);
  insn.SetDepNode(depNode);
  Reservation *seRev = mad.FindReservation(insn);
  depNode.SetInsn(insn);
  depNode.SetType(kNodeTypeEmpty);
  depNode.SetReservation(*seRev);
  depNode.SetUnitNum(0);
  depNode.ClearCfiInsns();
  depNode.SetUnits(nullptr);
}

/* Combine adrpldr&clinit_tail to clinit. */
void AArch64DepAnalysis::CombineClinit(DepNode &firstNode, DepNode &secondNode, bool isAcrossSeparator) {
  ASSERT(firstNode.GetInsn()->GetMachineOpcode() == MOP_adrp_ldr, "first insn should be adrpldr");
  ASSERT(secondNode.GetInsn()->GetMachineOpcode() == MOP_clinit_tail, "second insn should be clinit_tail");
  ASSERT(firstNode.GetCfiInsns().empty(), "There should not be any comment/cfi instructions between clinit.");
  ASSERT(secondNode.GetComments().empty(), "There should not be any comment/cfi instructions between clinit.");
  Insn &newInsn = cgFunc.GetCG()->BuildInstruction<AArch64Insn>(
      MOP_clinit, firstNode.GetInsn()->GetOperand(0), firstNode.GetInsn()->GetOperand(1));
  newInsn.SetId(firstNode.GetInsn()->GetId());
  /* Replace first node with new insn. */
  ReplaceDepNodeWithNewInsn(firstNode, secondNode, newInsn, true);
  /* Clear second node information. */
  ClearDepNodeInfo(secondNode);
  CombineDependence(firstNode, secondNode, isAcrossSeparator);
}

/*
 *  Combine memory access pair:
 *   1.ldr to ldp.
 *   2.str to stp.
 */
void AArch64DepAnalysis::CombineMemoryAccessPair(DepNode &firstNode, DepNode &secondNode, bool useFirstOffset) {
  ASSERT(firstNode.GetInsn(), "the insn of first Node should not be nullptr");
  ASSERT(secondNode.GetInsn(), "the insn of second Node should not be nullptr");
  MOperator thisMop = firstNode.GetInsn()->GetMachineOpcode();
  MOperator mopPair = GetMopPair(thisMop);
  ASSERT(mopPair != 0, "mopPair should not be zero");
  Operand *opnd0 = nullptr;
  Operand *opnd1 = nullptr;
  Operand *opnd2 = nullptr;
  if (useFirstOffset) {
    opnd0 = &(firstNode.GetInsn()->GetOperand(0));
    opnd1 = &(secondNode.GetInsn()->GetOperand(0));
    opnd2 = &(firstNode.GetInsn()->GetOperand(1));
  } else {
    opnd0 = &(secondNode.GetInsn()->GetOperand(0));
    opnd1 = &(firstNode.GetInsn()->GetOperand(0));
    opnd2 = &(secondNode.GetInsn()->GetOperand(1));
  }
  Insn &newInsn = cgFunc.GetCG()->BuildInstruction<AArch64Insn>(mopPair, *opnd0, *opnd1, *opnd2);
  newInsn.SetId(firstNode.GetInsn()->GetId());
  std::string newComment;
  const MapleString &comment = firstNode.GetInsn()->GetComment();
  if (comment.c_str() != nullptr) {
    newComment += comment.c_str();
  }
  const MapleString &secondComment = secondNode.GetInsn()->GetComment();
  if (secondComment.c_str() != nullptr) {
    newComment += "  ";
    newComment += secondComment.c_str();
  }
  if ((newComment.c_str() != nullptr) && (strlen(newComment.c_str()) > 0)) {
    newInsn.SetComment(newComment);
  }
  /* Replace first node with new insn. */
  ReplaceDepNodeWithNewInsn(firstNode, secondNode, newInsn, false);
  /* Clear second node information. */
  ClearDepNodeInfo(secondNode);
  CombineDependence(firstNode, secondNode, false, true);
}

/* Combine two dependence nodes to one */
void AArch64DepAnalysis::CombineDependence(DepNode &firstNode, DepNode &secondNode, bool isAcrossSeparator,
                                           bool isMemCombine) {
  if (isAcrossSeparator) {
    /* Clear all latency of the second node. */
    for (auto predLink : secondNode.GetPreds()) {
      predLink->SetLatency(0);
    }
    for (auto succLink : secondNode.GetSuccs()) {
      succLink->SetLatency(0);
    }
    return;
  }
  std::set<DepNode*> uniqueNodes;

  for (auto predLink : firstNode.GetPreds()) {
    if (predLink->GetDepType() == kDependenceTypeTrue) {
      predLink->SetLatency(mad.GetLatency(*predLink->GetFrom().GetInsn(), *firstNode.GetInsn()));
    }
    uniqueNodes.insert(&predLink->GetFrom());
  }
  for (auto predLink : secondNode.GetPreds()) {
    if (&predLink->GetFrom() != &firstNode) {
      if (uniqueNodes.insert(&(predLink->GetFrom())).second) {
        AddDependence(predLink->GetFrom(), firstNode, predLink->GetDepType());
      }
    }
    predLink->SetLatency(0);
  }
  uniqueNodes.clear();
  for (auto succLink : firstNode.GetSuccs()) {
    if (succLink->GetDepType() == kDependenceTypeTrue) {
      succLink->SetLatency(mad.GetLatency(*succLink->GetFrom().GetInsn(), *firstNode.GetInsn()));
    }
    uniqueNodes.insert(&(succLink->GetTo()));
  }
  for (auto succLink : secondNode.GetSuccs()) {
    if (uniqueNodes.insert(&(succLink->GetTo())).second) {
      AddDependence(firstNode, succLink->GetTo(), succLink->GetDepType());
      if (isMemCombine) {
        succLink->GetTo().IncreaseValidPredsSize();
      }
    }
    succLink->SetLatency(0);
  }
}

/*
 * Build dependences of ambiguous instruction.
 * ambiguous instruction : instructions that can not across may throw instructions.
 */
void AArch64DepAnalysis::BuildDepsAmbiInsn(Insn &insn) {
  AddDependence4InsnInVectorByType(mayThrows, insn, kDependenceTypeThrow);
  ambiInsns.push_back(&insn);
}

/* Build dependences of may throw instructions. */
void AArch64DepAnalysis::BuildDepsMayThrowInsn(Insn &insn) {
  AddDependence4InsnInVectorByType(ambiInsns, insn, kDependenceTypeThrow);
}

bool AArch64DepAnalysis::IsFrameReg(const RegOperand &opnd) const {
  return (opnd.GetRegisterNumber() == RFP) || (opnd.GetRegisterNumber() == RSP);
}

AArch64MemOperand *AArch64DepAnalysis::BuildNextMemOperandByByteSize(AArch64MemOperand &aarchMemOpnd,
                                                                     uint32 byteSize) const {
  AArch64MemOperand *nextMemOpnd = nullptr;
  Operand *nextOpnd = aarchMemOpnd.Clone(memPool);
  nextMemOpnd = static_cast<AArch64MemOperand*>(nextOpnd);
  Operand *nextOfstOpnd = nextMemOpnd->GetOffsetImmediate()->Clone(memPool);
  AArch64OfstOperand *aarchNextOfstOpnd = static_cast<AArch64OfstOperand*>(nextOfstOpnd);
  CHECK_NULL_FATAL(aarchNextOfstOpnd);
  int32 offsetVal = aarchNextOfstOpnd->GetOffsetValue();
  aarchNextOfstOpnd->SetOffsetValue(offsetVal + byteSize);
  nextMemOpnd->SetOffsetImmediate(*aarchNextOfstOpnd);
  return nextMemOpnd;
}

/* Get the second memory access operand of stp/ldp instructions. */
AArch64MemOperand *AArch64DepAnalysis::GetNextMemOperand(Insn &insn, AArch64MemOperand &aarchMemOpnd) const {
  AArch64MemOperand *nextMemOpnd = nullptr;
  switch (insn.GetMachineOpcode()) {
    case MOP_wldp:
    case MOP_sldp:
    case MOP_xldpsw:
    case MOP_wstp:
    case MOP_sstp: {
      nextMemOpnd = BuildNextMemOperandByByteSize(aarchMemOpnd, k4ByteSize);
      break;
    }
    case MOP_xldp:
    case MOP_dldp:
    case MOP_xstp:
    case MOP_dstp: {
      nextMemOpnd = BuildNextMemOperandByByteSize(aarchMemOpnd, k8ByteSize);
      break;
    }
    default:
      break;
  }

  return nextMemOpnd;
}

/*
 * Build dependences of symbol memory access.
 * Memory access with symbol must be a heap memory access.
 */
void AArch64DepAnalysis::BuildDepsAccessStImmMem(Insn &insn, bool isDest) {
  if (isDest) {
    /*
     * Heap memory
     * Build anti dependences.
     */
    AddDependence4InsnInVectorByType(heapUses, insn, kDependenceTypeAnti);
    /* Build output depnedence. */
    AddDependence4InsnInVectorByType(heapDefs, insn, kDependenceTypeOutput);
    heapDefs.push_back(&insn);
  } else {
    /* Heap memory */
    AddDependence4InsnInVectorByType(heapDefs, insn, kDependenceTypeTrue);
    heapUses.push_back(&insn);
  }
  if (memBarInsn != nullptr) {
    AddDependence(*memBarInsn->GetDepNode(), *insn.GetDepNode(), kDependenceTypeMembar);
  }
}

/* Build dependences of stack memory and heap memory uses. */
void AArch64DepAnalysis::BuildDepsUseMem(Insn &insn, MemOperand &memOpnd) {
  RegOperand *baseRegister = memOpnd.GetBaseRegister();
  AArch64MemOperand &aarchMemOpnd = static_cast<AArch64MemOperand&>(memOpnd);
  AArch64MemOperand *nextMemOpnd = GetNextMemOperand(insn, aarchMemOpnd);
  if (((baseRegister != nullptr) && IsFrameReg(*baseRegister)) || aarchMemOpnd.IsStackMem()) {
    /* Stack memory address */
    for (auto defInsn : stackDefs) {
      if (defInsn->IsCall() || NeedBuildDepsMem(aarchMemOpnd, nextMemOpnd, *defInsn)) {
        AddDependence(*defInsn->GetDepNode(), *insn.GetDepNode(), kDependenceTypeTrue);
        continue;
      }
    }
    stackUses.push_back(&insn);
  } else {
    /* Heap memory */
    AddDependence4InsnInVectorByType(heapDefs, insn, kDependenceTypeTrue);
    heapUses.push_back(&insn);
  }
  if (memBarInsn != nullptr) {
    AddDependence(*memBarInsn->GetDepNode(), *insn.GetDepNode(), kDependenceTypeMembar);
  }
}

/* Return true if memInsn's memOpnd no alias with memOpnd and nextMemOpnd */
bool AArch64DepAnalysis::NeedBuildDepsMem(const AArch64MemOperand &memOpnd, const AArch64MemOperand *nextMemOpnd,
                                          Insn &memInsn) const {
  auto *memOpndOfmemInsn = static_cast<AArch64MemOperand*>(memInsn.GetMemOpnd());
  if (!memOpnd.NoAlias(*memOpndOfmemInsn) || ((nextMemOpnd != nullptr) && !nextMemOpnd->NoAlias(*memOpndOfmemInsn))) {
    return true;
  }
  AArch64MemOperand *nextMemOpndOfmemInsn = GetNextMemOperand(memInsn, *memOpndOfmemInsn);
  if (nextMemOpndOfmemInsn != nullptr) {
    if (!memOpnd.NoAlias(*nextMemOpndOfmemInsn) ||
        ((nextMemOpnd != nullptr) && !nextMemOpnd->NoAlias(*nextMemOpndOfmemInsn))) {
      return true;
    }
  }
  return false;
}

/*
 * Build anti dependences between insn and other insn that use stack memroy.
 * insn        : the instruction that defines stack memory.
 * memOpnd     : insn's memOpnd
 * nextMemOpnd : some memory pair operator instruction (like ldp/stp) defines two memory.
 */
void AArch64DepAnalysis::BuildAntiDepsDefStackMem(Insn &insn, const AArch64MemOperand &memOpnd,
                                                  const AArch64MemOperand *nextMemOpnd) {
  for (auto *useInsn : stackUses) {
    if (NeedBuildDepsMem(memOpnd, nextMemOpnd, *useInsn)) {
      AddDependence(*useInsn->GetDepNode(), *insn.GetDepNode(), kDependenceTypeAnti);
    }
  }
}

/*
 * Build output dependences between insn with other insn that define stack memroy.
 * insn        : the instruction that defines stack memory.
 * memOpnd     : insn's memOpnd
 * nextMemOpnd : some memory pair operator instruction (like ldp/stp) defines two memory.
 */
void AArch64DepAnalysis::BuildOutputDepsDefStackMem(Insn &insn, const AArch64MemOperand &memOpnd,
                                                    const AArch64MemOperand *nextMemOpnd) {
  for (auto defInsn : stackDefs) {
    if (defInsn->IsCall() || NeedBuildDepsMem(memOpnd, nextMemOpnd, *defInsn)) {
      AddDependence(*defInsn->GetDepNode(), *insn.GetDepNode(), kDependenceTypeOutput);
    }
  }
}

/* Build dependences of stack memory and heap memory definitions. */
void AArch64DepAnalysis::BuildDepsDefMem(Insn &insn, MemOperand &memOpnd) {
  RegOperand *baseRegister = memOpnd.GetBaseRegister();
  AArch64MemOperand &aarchMemOpnd = static_cast<AArch64MemOperand&>(memOpnd);
  AArch64MemOperand *nextMemOpnd = GetNextMemOperand(insn, aarchMemOpnd);

  if (((baseRegister != nullptr) && IsFrameReg(*baseRegister)) || aarchMemOpnd.IsStackMem()) {
    /* Build anti dependences. */
    BuildAntiDepsDefStackMem(insn, aarchMemOpnd, nextMemOpnd);
    /* Build output depnedence. */
    BuildOutputDepsDefStackMem(insn, aarchMemOpnd, nextMemOpnd);
    if (lastCallInsn != nullptr) {
      /* Build a dependence between stack passed arguments and call. */
      ASSERT(baseRegister != nullptr, "baseRegister shouldn't be null here");
      if (baseRegister->GetRegisterNumber() == RSP) {
        AddDependence(*lastCallInsn->GetDepNode(), *insn.GetDepNode(), kDependenceTypeControl);
      }
    }
    stackDefs.push_back(&insn);
  } else {
    /* Heap memory
     * Build anti dependences.
     */
    AddDependence4InsnInVectorByType(heapUses, insn, kDependenceTypeAnti);
    /* Build output depnedence. */
    AddDependence4InsnInVectorByType(heapDefs, insn, kDependenceTypeOutput);
    heapDefs.push_back(&insn);
  }
  if (memBarInsn != nullptr) {
    AddDependence(*memBarInsn->GetDepNode(), *insn.GetDepNode(), kDependenceTypeMembar);
  }
  /* Memory definition can not across may-throw insns. */
  AddDependence4InsnInVectorByType(mayThrows, insn, kDependenceTypeThrow);
}

/* Build dependences of memory barrior instructions. */
void AArch64DepAnalysis::BuildDepsMemBar(Insn &insn) {
  AddDependence4InsnInVectorByTypeAndCmp(stackUses, insn, kDependenceTypeMembar);
  AddDependence4InsnInVectorByTypeAndCmp(heapUses, insn, kDependenceTypeMembar);
  AddDependence4InsnInVectorByTypeAndCmp(stackDefs, insn, kDependenceTypeMembar);
  AddDependence4InsnInVectorByTypeAndCmp(heapDefs, insn, kDependenceTypeMembar);
  memBarInsn = &insn;
}

/* A pseudo separator node depends all the other nodes. */
void AArch64DepAnalysis::BuildDepsSeparator(DepNode &newSepNode, MapleVector<DepNode*> &nodes) {
  uint32 nextSepIndex = (separatorIndex + kMaxDependenceNum) < nodes.size() ? (separatorIndex + kMaxDependenceNum)
                                                                            : nodes.size() - 1;
  newSepNode.ReservePreds(nextSepIndex - separatorIndex);
  newSepNode.ReserveSuccs(nextSepIndex - separatorIndex);
  for (uint32 i = separatorIndex; i < nextSepIndex; ++i) {
    AddDependence(*nodes[i], newSepNode, kDependenceTypeSeparator);
  }
}


/* Build control dependence for branch/ret instructions. */
void AArch64DepAnalysis::BuildDepsControlAll(DepNode &depNode, const MapleVector<DepNode*> &nodes) {
  for (uint32 i = separatorIndex; i < depNode.GetIndex(); ++i) {
    AddDependence(*nodes[i], depNode, kDependenceTypeControl);
  }
}

/*
 * Build dependences of call instructions.
 * Caller-saved physical registers will defined by a call instruction.
 * Also a conditional register may modified by a call.
 */
void AArch64DepAnalysis::BuildCallerSavedDeps(Insn &insn) {
  /* Build anti dependence and output dependence. */
  for (uint32 i = R0; i <= R7; ++i) {
    BuildDepsDefReg(insn, i);
  }
  for (uint32 i = V0; i <= V7; ++i) {
    BuildDepsDefReg(insn, i);
  }
  if (!beforeRA) {
    for (uint32 i = R8; i <= R18; ++i) {
      BuildDepsDefReg(insn, i);
    }
    for (uint32 i = R29; i <= RSP; ++i) {
      BuildDepsUseReg(insn, i);
    }
    for (uint32 i = V16; i <= V31; ++i) {
      BuildDepsDefReg(insn, i);
    }
  }
  /* For condition operand, such as NE, EQ, and so on. */
  if (cgFunc.GetRflag() != nullptr) {
    BuildDepsDefReg(insn, kRFLAG);
  }
}

/*
 * Build dependence between control register and last call instruction.
 * insn : instruction that with control register operand.
 * isDest : if the control register operand is a destination operand.
 */
void AArch64DepAnalysis::BuildDepsBetweenControlRegAndCall(Insn &insn, bool isDest) {
  if (lastCallInsn == nullptr) {
    return;
  }
  if (isDest) {
    AddDependence(*lastCallInsn->GetDepNode(), *insn.GetDepNode(), kDependenceTypeOutput);
    return;
  }
  AddDependence(*lastCallInsn->GetDepNode(), *insn.GetDepNode(), kDependenceTypeAnti);
}

/*
 * Build dependence between stack-define-instruction that deal with call-insn's args and a call-instruction.
 * insn : a call instruction (call/tail-call)
 */
void AArch64DepAnalysis::BuildStackPassArgsDeps(Insn &insn) {
  for (auto stackDefInsn : stackDefs) {
    if (stackDefInsn->IsCall()) {
      continue;
    }
    Operand *opnd = stackDefInsn->GetMemOpnd();
    ASSERT(opnd->IsMemoryAccessOperand(), "make sure opnd is memOpnd");
    MemOperand *memOpnd = static_cast<MemOperand*>(opnd);
    RegOperand *baseReg = memOpnd->GetBaseRegister();
    if ((baseReg != nullptr) && (baseReg->GetRegisterNumber() == RSP)) {
      AddDependence(*stackDefInsn->GetDepNode(), *insn.GetDepNode(), kDependenceTypeControl);
    }
  }
}

/* Some insns may dirty all stack memory, such as "bl MCC_InitializeLocalStackRef". */
void AArch64DepAnalysis::BuildDepsDirtyStack(Insn &insn) {
  /* Build anti dependences. */
  AddDependence4InsnInVectorByType(stackUses, insn, kDependenceTypeAnti);
  /* Build output depnedence. */
  AddDependence4InsnInVectorByType(stackDefs, insn, kDependenceTypeOutput);
  stackDefs.push_back(&insn);
}

/* Some call insns may use all stack memory, such as "bl MCC_CleanupLocalStackRef_NaiveRCFast". */
void AArch64DepAnalysis::BuildDepsUseStack(Insn &insn) {
  /* Build true dependences. */
  AddDependence4InsnInVectorByType(stackDefs, insn, kDependenceTypeTrue);
}

/* Some insns may dirty all heap memory, such as a call insn. */
void AArch64DepAnalysis::BuildDepsDirtyHeap(Insn &insn) {
  /* Build anti dependences. */
  AddDependence4InsnInVectorByType(heapUses, insn, kDependenceTypeAnti);
  /* Build output depnedence. */
  AddDependence4InsnInVectorByType(heapDefs, insn, kDependenceTypeOutput);
  if (memBarInsn != nullptr) {
    AddDependence(*memBarInsn->GetDepNode(), *insn.GetDepNode(), kDependenceTypeMembar);
  }
  heapDefs.push_back(&insn);
}

/* Build a pseudo node to seperate dependence graph. */
DepNode *AArch64DepAnalysis::BuildSeparatorNode() {
  Insn &pseudoSepInsn = cgFunc.GetCG()->BuildInstruction<AArch64Insn>(MOP_pseudo_dependence_seperator);
  DepNode *separatorNode = memPool.New<DepNode>(pseudoSepInsn, alloc);
  separatorNode->SetType(kNodeTypeSeparator);
  pseudoSepInsn.SetDepNode(*separatorNode);
  if (beforeRA) {
    RegPressure *regPressure = memPool.New<RegPressure>(alloc);
    separatorNode->SetRegPressure(*regPressure);
    separatorNode->InitPressure();
  }
  return separatorNode;
}

/* Init depAnalysis data struction */
void AArch64DepAnalysis::Init(BB &bb, MapleVector<DepNode*> &nodes) {
  curBB = &bb;
  ClearAllDepData();
  lastComments.clear();
  /* Analysis live-in registers in catch BB. */
  AnalysisAmbiInsns(bb);
  /* Clear all dependence nodes and push the first separator node. */
  nodes.clear();
  DepNode *pseudoSepNode = BuildSeparatorNode();
  nodes.push_back(pseudoSepNode);
  separatorIndex = 0;

  if (beforeRA) {
    /* assump first pseudo_dependence_seperator insn of current bb define live-in's registers */
    Insn *pseudoSepInsn = pseudoSepNode->GetInsn();
    for (auto &regNO : bb.GetLiveInRegNO()) {
      regDefs[regNO] = pseudoSepInsn;
      pseudoSepNode->AddDefReg(regNO);
      pseudoSepNode->SetRegDefs(pseudoSepNode->GetDefRegnos().size(), nullptr);
    }
  }
}

/* When a separator build, it is the same as a new basic block. */
void AArch64DepAnalysis::ClearAllDepData() {
  uint32 maxRegNum;
  if (beforeRA) {
    maxRegNum = cgFunc.GetMaxVReg();
  } else {
    maxRegNum = kAllRegNum;
  }
  errno_t ret = memset_s(regDefs, sizeof(Insn*) * maxRegNum, 0, sizeof(Insn*) * maxRegNum);
  CHECK_FATAL(ret == EOK, "call memset_s failed in Unit");
  ret = memset_s(regUses, sizeof(RegList*) * maxRegNum, 0, sizeof(RegList*) * maxRegNum);
  CHECK_FATAL(ret == EOK, "call memset_s failed in Unit");
  memBarInsn = nullptr;
  lastCallInsn = nullptr;
  lastFrameDef = nullptr;

  stackUses.clear();
  stackDefs.clear();
  heapUses.clear();
  heapDefs.clear();
  mayThrows.clear();
  ambiInsns.clear();
}

/* Analysis live-in registers in catch bb and cleanup bb. */
void AArch64DepAnalysis::AnalysisAmbiInsns(BB &bb) {
  hasAmbiRegs = false;
  if (bb.GetEhSuccs().empty()) {
    return;
  }

  /* Union all catch bb */
  for (auto succBB : bb.GetEhSuccs()) {
    const MapleSet<regno_t> &liveInRegSet = succBB->GetLiveInRegNO();
    set_union(liveInRegSet.begin(), liveInRegSet.end(),
              ehInRegs.begin(), ehInRegs.end(),
              inserter(ehInRegs, ehInRegs.begin()));
  }

  /* Union cleanup entry bb. */
  const MapleSet<regno_t> &regNOSet = cgFunc.GetCleanupEntryBB()->GetLiveInRegNO();
  std::set_union(regNOSet.begin(), regNOSet.end(),
                 ehInRegs.begin(), ehInRegs.end(),
                 inserter(ehInRegs, ehInRegs.begin()));

  /* Subtract R0 and R1, that is defined by eh runtime. */
  ehInRegs.erase(R0);
  ehInRegs.erase(R1);
  if (ehInRegs.empty()) {
    return;
  }
  hasAmbiRegs = true;
}

/* Check if regNO is in ehInRegs. */
bool AArch64DepAnalysis::IfInAmbiRegs(regno_t regNO) const {
  if (!hasAmbiRegs) {
    return false;
  }
  if (ehInRegs.find(regNO) != ehInRegs.end()) {
    return true;
  }
  return false;
}

/*
 * Build dependences of memory operand.
 * insn : a instruction with the memory access operand.
 * opnd : the memory access operand.
 * regProp : operand property of the memory access operandess operand.
 */
void AArch64DepAnalysis::BuildMemOpndDependency(Insn &insn, Operand &opnd, const AArch64OpndProp &regProp) {
  ASSERT(opnd.IsMemoryAccessOperand(), "opnd must be memory Operand");
  AArch64MemOperand *memOpnd = static_cast<AArch64MemOperand*>(&opnd);
  RegOperand *baseRegister = memOpnd->GetBaseRegister();
  if (baseRegister != nullptr) {
    regno_t regNO = baseRegister->GetRegisterNumber();
    BuildDepsUseReg(insn, regNO);
    if ((memOpnd->GetAddrMode() == AArch64MemOperand::kAddrModeBOi) &&
        (memOpnd->IsPostIndexed() || memOpnd->IsPreIndexed())) {
      /* Base operand has changed. */
      BuildDepsDefReg(insn, regNO);
    }
  }
  RegOperand *indexRegister = memOpnd->GetIndexRegister();
  if (indexRegister != nullptr) {
    regno_t regNO = indexRegister->GetRegisterNumber();
    BuildDepsUseReg(insn, regNO);
  }
  if (regProp.IsUse()) {
    BuildDepsUseMem(insn, *memOpnd);
  } else {
    BuildDepsDefMem(insn, *memOpnd);
    BuildDepsAmbiInsn(insn);
  }
  if (insn.IsYieldPoint()) {
    BuildDepsMemBar(insn);
    BuildDepsDefReg(insn, kRFLAG);
  }
}

/* Build Dependency for each Operand of insn */
void AArch64DepAnalysis::BuildOpndDependency(Insn &insn) {
  const AArch64MD* md = &AArch64CG::kMd[static_cast<AArch64Insn*>(&insn)->GetMachineOpcode()];
  MOperator mOp = insn.GetMachineOpcode();
  uint32 opndNum = insn.GetOperandSize();
  for (uint32 i = 0; i < opndNum; ++i) {
    Operand &opnd = insn.GetOperand(i);
    AArch64OpndProp *regProp = static_cast<AArch64OpndProp*>(md->operand[i]);
    if (opnd.IsMemoryAccessOperand()) {
      BuildMemOpndDependency(insn, opnd, *regProp);
    } else if (opnd.IsStImmediate()) {
      if (mOp != MOP_xadrpl12) {
        BuildDepsAccessStImmMem(insn, false);
      }
    } else if (opnd.IsRegister()) {
      RegOperand &regOpnd = static_cast<RegOperand&>(opnd);
      regno_t regNO = regOpnd.GetRegisterNumber();

      if (regProp->IsUse()) {
        BuildDepsUseReg(insn, regNO);
      }

      if (regProp->IsDef()) {
        BuildDepsDefReg(insn, regNO);
      }
    } else if (opnd.IsConditionCode()) {
      /* For condition operand, such as NE, EQ, and so on. */
      if (regProp->IsUse()) {
        BuildDepsUseReg(insn, kRFLAG);
        BuildDepsBetweenControlRegAndCall(insn, false);
      }

      if (regProp->IsDef()) {
        BuildDepsDefReg(insn, kRFLAG);
        BuildDepsBetweenControlRegAndCall(insn, true);
      }
    } else if (opnd.IsList()) {
      ListOperand &listOpnd = static_cast<ListOperand&>(opnd);
      /* Build true dependences */
      for (auto lst : listOpnd.GetOperands()) {
        regno_t regNO = lst->GetRegisterNumber();
        BuildDepsUseReg(insn, regNO);
      }
    }
  }
}

/*
 * Build dependences in some special issue (stack/heap/throw/clinit/lazy binding/control flow).
 * insn :  a instruction.
 * depNode : insn's depNode.
 * nodes : the dependence nodes inclue insn's depNode.
 */
void AArch64DepAnalysis::BuildSpecialInsnDependency(Insn &insn, DepNode &depNode, const MapleVector<DepNode*> &nodes) {
  const AArch64MD *md = &AArch64CG::kMd[static_cast<AArch64Insn*>(&insn)->GetMachineOpcode()];
  MOperator mOp = insn.GetMachineOpcode();
  if (insn.IsCall() || insn.IsTailCall()) {
    /* Caller saved registers. */
    BuildCallerSavedDeps(insn);
    BuildStackPassArgsDeps(insn);

    if (mOp == MOP_xbl) {
      FuncNameOperand &target = static_cast<FuncNameOperand&>(insn.GetOperand(0));
      if ((target.GetName() == "MCC_InitializeLocalStackRef") ||
          (target.GetName() == "MCC_ClearLocalStackRef") ||
          (target.GetName() == "MCC_DecRefResetPair")) {
        /* Write stack memory. */
        BuildDepsDirtyStack(insn);
      } else if ((target.GetName() == "MCC_CleanupLocalStackRef_NaiveRCFast") ||
                 (target.GetName() == "MCC_CleanupLocalStackRefSkip_NaiveRCFast") ||
                 (target.GetName() == "MCC_CleanupLocalStackRefSkip")) {
        /* UseStackMemory. */
        BuildDepsUseStack(insn);
      }
    }
    BuildDepsDirtyHeap(insn);
    BuildDepsAmbiInsn(insn);
    if (lastCallInsn != nullptr) {
      AddDependence(*lastCallInsn->GetDepNode(), *insn.GetDepNode(), kDependenceTypeControl);
    }
    lastCallInsn = &insn;
  } else if (insn.IsClinit() || insn.IsLazyLoad() || insn.IsArrayClassCache()) {
    BuildDepsDirtyHeap(insn);
    BuildDepsDefReg(insn, kRFLAG);
    if (!insn.IsAdrpLdr()) {
      BuildDepsDefReg(insn, R16);
      BuildDepsDefReg(insn, R17);
    }
  } else if ((mOp == MOP_xret) || md->IsBranch()) {
    BuildDepsControlAll(depNode, nodes);
  } else if (insn.IsMemAccessBar()) {
    BuildDepsMemBar(insn);
  } else if (insn.IsSpecialIntrinsic()) {
    BuildDepsDirtyHeap(insn);
  }
}

/*
 * If the instruction's number of current basic block more than kMaxDependenceNum,
 * then insert some pseudo separator node to split baic block.
 */
void AArch64DepAnalysis::SeperateDependenceGraph(MapleVector<DepNode*> &nodes, uint32 &nodeSum) {
  if ((nodeSum > 0) && ((nodeSum % kMaxDependenceNum) == 0)) {
    ASSERT(nodeSum == nodes.size(), "CG internal error, nodeSum should equal to nodes.size.");
    /* Add a pseudo node to seperate dependence graph. */
    DepNode *separatorNode = BuildSeparatorNode();
    separatorNode->SetIndex(nodeSum);
    nodes.push_back(separatorNode);
    BuildDepsSeparator(*separatorNode, nodes);

    if (beforeRA) {
      /* for all live-out register of current bb */
      for (auto &regNO : curBB->GetLiveOutRegNO()) {
        if (regDefs[regNO] != nullptr) {
          AppendRegUseList(*(separatorNode->GetInsn()), regNO);
          separatorNode->AddUseReg(regNO);
          separatorNode->SetRegUses(*regUses[regNO]);
        }
      }
    }
    ClearAllDepData();
    separatorIndex = nodeSum++;
  }
}

/*
 * Generate a depNode,
 * insn  : create depNode for the instruction.
 * nodes : a vector to store depNode.
 * nodeSum  : the new depNode's index.
 * comments : those comment insn between last no-comment's insn and insn.
 */
DepNode *AArch64DepAnalysis::GenerateDepNode(Insn &insn, MapleVector<DepNode*> &nodes,
                                             int32 nodeSum, const MapleVector<Insn*> &comments) {
  DepNode *depNode = nullptr;
  Reservation *rev = mad.FindReservation(insn);
  ASSERT(rev != nullptr, "rev is nullptr");
  depNode = memPool.New<DepNode>(insn, alloc, rev->GetUnit(), rev->GetUnitNum(), *rev);
  if (beforeRA) {
    RegPressure *regPressure = memPool.New<RegPressure>(alloc);
    depNode->SetRegPressure(*regPressure);
    depNode->InitPressure();
  }
  depNode->SetIndex(nodeSum);
  nodes.push_back(depNode);
  insn.SetDepNode(*depNode);

  constexpr size_t vectorSize = 5;
  depNode->ReservePreds(vectorSize);
  depNode->ReserveSuccs(vectorSize);

  if (!comments.empty()) {
    depNode->SetComments(comments);
  }
  return depNode;
}

void AArch64DepAnalysis::BuildAmbiInsnDependency(Insn &insn) {
  const auto &defRegnos = insn.GetDepNode()->GetDefRegnos();
  for (const auto &regNO : defRegnos) {
    if (IfInAmbiRegs(regNO)) {
      BuildDepsAmbiInsn(insn);
      break;
    }
  }
}

void AArch64DepAnalysis::BuildMayThrowInsnDependency(Insn &insn) {
  /* build dependency for maythrow insn; */
  if (insn.MayThrow()) {
    BuildDepsMayThrowInsn(insn);
    if (lastFrameDef != nullptr) {
      AddDependence(*lastFrameDef->GetDepNode(), *insn.GetDepNode(), kDependenceTypeThrow);
    }
  }
}

void AArch64DepAnalysis::UpdateRegUseAndDef(Insn &insn, DepNode &depNode, MapleVector<DepNode*> &nodes) {
  const auto &useRegnos = depNode.GetUseRegnos();
  if (beforeRA) {
    depNode.InitRegUsesSize(useRegnos.size());
  }
  for (auto regNO : useRegnos) {
    AppendRegUseList(insn, regNO);
    if (beforeRA) {
      depNode.SetRegUses(*regUses[regNO]);
      if (regDefs[regNO] == nullptr) {
        regDefs[regNO] = nodes[separatorIndex]->GetInsn();
        nodes[separatorIndex]->AddDefReg(regNO);
        nodes[separatorIndex]->SetRegDefs(nodes[separatorIndex]->GetDefRegnos().size(), regUses[regNO]);
      }
    }
  }

  const auto &defRegnos = depNode.GetDefRegnos();
  size_t i = 0;
  if (beforeRA) {
    depNode.InitRegDefsSize(defRegnos.size());
  }
  for (const auto regNO : defRegnos) {
    regDefs[regNO] = &insn;
    regUses[regNO] = nullptr;
    if (beforeRA) {
      depNode.SetRegDefs(i, nullptr);
      if (regNO >= R0 && regNO <= R3) {
        depNode.SetHasPreg(true);
      } else if (regNO == R8) {
        depNode.SetHasNativeCallRegister(true);
      }
    }
    ++i;
  }
}

/* Update stack and heap dependency */
void AArch64DepAnalysis::UpdateStackAndHeapDependency(DepNode &depNode, Insn &insn, const Insn &locInsn) {
  if (!insn.MayThrow()) {
    return;
  }
  depNode.SetLocInsn(locInsn);
  mayThrows.push_back(&insn);
  AddDependence4InsnInVectorByType(stackDefs, insn, kDependenceTypeThrow);
  AddDependence4InsnInVectorByType(heapDefs, insn, kDependenceTypeThrow);
}

/* Add a separatorNode to the end of a nodes
 *  * before RA:  add all live-out registers to this separatorNode'Uses
 *   */
void AArch64DepAnalysis::AddEndSeparatorNode(MapleVector<DepNode*> &nodes) {
  DepNode *separatorNode = BuildSeparatorNode();
  nodes.emplace_back(separatorNode);
  BuildDepsSeparator(*separatorNode, nodes);

  if (beforeRA) {
    /* for all live-out register of current bb */
    for (auto &regNO : curBB->GetLiveOutRegNO()) {
      if (regDefs[regNO] != nullptr) {
        AppendRegUseList(*(separatorNode->GetInsn()), regNO);
        separatorNode->AddUseReg(regNO);
        separatorNode->SetRegUses(*regUses[regNO]);
      }
    }
  }
}

/*
 * Build dependence graph.
 * 1: Build dependence nodes.
 * 2: Build edges between dependence nodes. Edges are:
 *   2.1) True dependences
 *   2.2) Anti dependences
 *   2.3) Output dependences
 *   2.4) Barrier dependences
 */
void AArch64DepAnalysis::Run(BB &bb, MapleVector<DepNode*> &nodes) {
  /* Initial internal datas. */
  Init(bb, nodes);
  uint32 nodeSum = 1;
  MapleVector<Insn*> comments(alloc.Adapter());
  const Insn *locInsn = bb.GetFirstLoc();
  FOR_BB_INSNS(insn, (&bb)) {
    if (!insn->IsMachineInstruction()) {
      if (insn->IsImmaterialInsn()) {
        if (!insn->IsComment()) {
          locInsn = insn;
        } else {
          comments.push_back(insn);
        }
      } else if (insn->IsCfiInsn()) {
        if (!nodes.empty()) {
          nodes.back()->AddCfiInsn(*insn);
        }
      }
      continue;
    }
    /* Add a pseudo node to seperate dependence graph when appropriate */
    SeperateDependenceGraph(nodes, nodeSum);
    /* generate a DepNode */
    DepNode *depNode = GenerateDepNode(*insn, nodes, nodeSum, comments);
    ++nodeSum;
    comments.clear();
    /* Build Dependency for maythrow insn; */
    BuildMayThrowInsnDependency(*insn);
    /* Build Dependency for each Operand of insn */
    BuildOpndDependency(*insn);
    /* Build Dependency for special insn */
    BuildSpecialInsnDependency(*insn, *depNode, nodes);
    /* Build Dependency for AmbiInsn if needed */
    BuildAmbiInsnDependency(*insn);
    /* Update stack and heap dependency */
    UpdateStackAndHeapDependency(*depNode, *insn, *locInsn);
    if (insn->IsFrameDef()) {
      lastFrameDef = insn;
    }
    /* Seperator exists. */
    AddDependence(*nodes[separatorIndex], *insn->GetDepNode(), kDependenceTypeSeparator);
    /* Update register use and register def */
    UpdateRegUseAndDef(*insn, *depNode, nodes);
  }

  AddEndSeparatorNode(nodes);

  if (!comments.empty()) {
    lastComments = comments;
  }
  comments.clear();
}

/* return dependence type name */
const std::string &AArch64DepAnalysis::GetDepTypeName(DepType depType) const {
  ASSERT(depType <= kDependenceTypeNone, "array boundary check failed");
  return kDepTypeName[depType];
}
}  /* namespace maplebe */
