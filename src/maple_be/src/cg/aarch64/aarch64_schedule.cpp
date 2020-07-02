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
#include "aarch64_schedule.h"
#include <ctime>
#include "aarch64_cg.h"
#include "aarch64_operand.h"
#include "aarch64_dependence.h"
#include "pressure.h"

/*
 * This phase is Instruction Scheduling.
 * There is a local list scheduling, it is scheduling in basic block.
 * The entry is AArch64Schedule::ListScheduling, will traversal all basic block,
 * for a basic block:
 *  1. build a dependence graph;
 *  2. combine clinit pairs and str&ldr pairs;
 *  3. reorder instructions.
 */
namespace maplebe {
namespace {
constexpr uint32 kClinitAdvanceCycle = 10;
constexpr uint32 kAdrpLdrAdvanceCycle = 2;
constexpr uint32 kClinitTailAdvanceCycle = 4;
}

uint32 AArch64Schedule::maxUnitIndex = 0;
/* Init schedule's data struction. */
void AArch64Schedule::Init() {
  readyList.clear();
  nodeSize = nodes.size();
  lastSeparatorIndex = 0;
  mad->ReleaseAllUnits();
  DepNode *node = nodes[0];

  ASSERT(node->GetType() == kNodeTypeSeparator, "CG internal error, the first node should be a separator node.");

  if (CGOptions::IsDruteForceSched() || CGOptions::IsSimulateSched()) {
    for (auto nodeTemp : nodes) {
      nodeTemp->SetVisit(0);
      nodeTemp->SetState(kNormal);
      nodeTemp->SetSchedCycle(0);
      nodeTemp->SetEStart(0);
      nodeTemp->SetLStart(0);
    }
  }

  readyList.push_back(node);
  node->SetState(kReady);

  /* Init validPredsSize and validSuccsSize. */
  for (auto nodeTemp : nodes) {
    nodeTemp->SetValidPredsSize(nodeTemp->GetPreds().size());
    nodeTemp->SetValidSuccsSize(nodeTemp->GetSuccs().size());
  }
}

/*
 *  A insn which can be combine should meet this conditions:
 *  1. it is str/ldr insn;
 *  2. address mode is kAddrModeBOi, [baseReg, offset];
 *  3. the register operand size equal memory operand size;
 *  4. if define USE_32BIT_REF, register operand size should be 4 byte;
 *  5. for stp/ldp, the imm should be within -512 and 504(64bit), or -256 and 252(32bit);
 *  6. pair instr for 8/4 byte registers must have multiple of 8/4 for imm.
 *  If insn can be combine, return true.
 */
bool AArch64Schedule::CanCombine(const Insn &insn) const {
  MOperator opCode = insn.GetMachineOpcode();
  if ((opCode != MOP_xldr) && (opCode != MOP_wldr) && (opCode != MOP_dldr) && (opCode != MOP_sldr) &&
      (opCode != MOP_xstr) && (opCode != MOP_wstr) && (opCode != MOP_dstr) && (opCode != MOP_sstr)) {
    return false;
  }

  ASSERT(insn.GetOperand(1).IsMemoryAccessOperand(), "expects mem operands");
  auto &memOpnd = static_cast<AArch64MemOperand&>(insn.GetOperand(1));
  AArch64MemOperand::AArch64AddressingMode addrMode = memOpnd.GetAddrMode();
  if ((addrMode != AArch64MemOperand::kAddrModeBOi) || !memOpnd.IsIntactIndexed()) {
    return false;
  }

  auto &regOpnd = static_cast<AArch64RegOperand&>(insn.GetOperand(0));
  if (regOpnd.GetSize() != memOpnd.GetSize()) {
    return false;
  }

  uint32 size = regOpnd.GetSize() >> kLog2BitsPerByte;
#ifdef USE_32BIT_REF
  if (insn.IsAccessRefField() && (size > (kIntregBytelen >> 1))) {
    return false;
  }
#endif /* USE_32BIT_REF */

  AArch64OfstOperand *offset = memOpnd.GetOffsetImmediate();
  if (offset == nullptr) {
    return false;
  }
  int32 offsetValue = offset->GetOffsetValue();
  if (size == kIntregBytelen) {              /* 64 bit */
    if ((offsetValue <= kStpLdpImm64LowerBound) || (offsetValue >= kStpLdpImm64UpperBound)) {
      return false;
    }
  } else if (size == (kIntregBytelen >> 1)) { /* 32 bit */
    if ((offsetValue <= kStpLdpImm32LowerBound) || (offsetValue >= kStpLdpImm32UpperBound)) {
      return false;
    }
  }

  /* pair instr for 8/4 byte registers must have multiple of 8/4 for imm */
  if ((static_cast<uint32>(offsetValue) % size) != 0) {
    return false;
  }
  return true;
}

/* After building dependence graph, combine str&ldr pairs. */
void AArch64Schedule::MemoryAccessPairOpt() {
  Init();
  std::vector<DepNode*> memList;

  while ((!readyList.empty()) || !memList.empty()) {
    DepNode *readNode = nullptr;
    if (!readyList.empty()) {
      readNode = readyList[0];
      readyList.erase(readyList.begin());
    } else {
      if (memList[0]->GetType() != kNodeTypeEmpty) {
        FindAndCombineMemoryAccessPair(memList);
      }
      readNode = memList[0];
      memList.erase(memList.begin());
    }

    /* schedule readNode */
    CHECK_FATAL(readNode != nullptr, "readNode is null in MemoryAccessPairOpt");
    readNode->SetState(kScheduled);

    /* add readNode's succs to readyList or memList. */
    for (auto succLink : readNode->GetSuccs()) {
      DepNode &succNode = succLink->GetTo();
      succNode.DescreaseValidPredsSize();
      if (succNode.GetValidPredsSize() == 0) {
        ASSERT(succNode.GetState() == kNormal, "schedule state should be kNormal");
        succNode.SetState(kReady);
        ASSERT(succNode.GetInsn() != nullptr, "insn can't be nullptr!");
        if (CanCombine(*succNode.GetInsn())) {
          memList.push_back(&succNode);
        } else {
          readyList.push_back(&succNode);
        }
      }
    }
  }

  for (auto node : nodes) {
    node->SetVisit(0);
    node->SetState(kNormal);
  }
}

/* Find and combine correct MemoryAccessPair for memList[0]. */
void AArch64Schedule::FindAndCombineMemoryAccessPair(const std::vector<DepNode*> &memList) {
  ASSERT(!memList.empty(), "memList should not be empty");
  CHECK_FATAL(memList[0]->GetInsn() != nullptr, "memList[0]'s insn should not be nullptr");
  AArch64MemOperand *currMemOpnd = static_cast<AArch64MemOperand*>(memList[0]->GetInsn()->GetMemOpnd());
  ASSERT(currMemOpnd != nullptr, "opnd should not be nullptr");
  ASSERT(currMemOpnd->IsMemoryAccessOperand(), "opnd should be memOpnd");
  int32 currOffsetVal = currMemOpnd->GetOffsetImmediate()->GetOffsetValue();
  MOperator currMop = memList[0]->GetInsn()->GetMachineOpcode();
  /* find a depNode to combine with memList[0], and break; */
  for (auto it = std::next(memList.begin(), 1); it != memList.end(); ++it) {
    ASSERT((*it)->GetInsn() != nullptr, "null ptr check");

    if (currMop == (*it)->GetInsn()->GetMachineOpcode()) {
      AArch64MemOperand *nextMemOpnd = static_cast<AArch64MemOperand*>((*it)->GetInsn()->GetMemOpnd());
      CHECK_FATAL(nextMemOpnd != nullptr, "opnd should not be nullptr");
      CHECK_FATAL(nextMemOpnd->IsMemoryAccessOperand(), "opnd should be MemOperand");
      int32 nextOffsetVal = nextMemOpnd->GetOffsetImmediate()->GetOffsetValue();
      uint32 size = currMemOpnd->GetSize() >> kLog2BitsPerByte;
      if ((nextMemOpnd->GetBaseRegister() == currMemOpnd->GetBaseRegister()) &&
          (nextMemOpnd->GetSize() == currMemOpnd->GetSize()) &&
          (static_cast<uint32>(abs(nextOffsetVal - currOffsetVal)) == size)) {
        /*
         * In ARM Architecture Reference Manual ARMv8, for ARMv8-A architecture profile
         * LDP on page K1-6125 declare that ldp can't use same reg
         */
        if (((currMop == MOP_xldr) || (currMop == MOP_sldr) || (currMop == MOP_dldr) || (currMop == MOP_wldr)) &&
            &(memList[0]->GetInsn()->GetOperand(0)) == &((*it)->GetInsn()->GetOperand(0))) {
          continue;
        }

        if (LIST_SCHED_DUMP_REF) {
          LogInfo::MapleLogger() << "Combine inse: " << "\n";
          memList[0]->GetInsn()->Dump();
          (*it)->GetInsn()->Dump();
        }
        depAnalysis->CombineMemoryAccessPair(*memList[0], **it, nextOffsetVal > currOffsetVal);
        if (LIST_SCHED_DUMP_REF) {
          LogInfo::MapleLogger() << "To: " << "\n";
          memList[0]->GetInsn()->Dump();
        }
        break;
      }
    }
  }
}

/* combine clinit pairs. */
void AArch64Schedule::ClinitPairOpt() {
  for (auto it = nodes.begin(); it != nodes.end(); ++it) {
    auto nextIt = std::next(it, 1);
    if (nextIt == nodes.end()) {
      return;
    }

    if ((*it)->GetInsn()->GetMachineOpcode() == MOP_adrp_ldr) {
      if ((*nextIt)->GetInsn()->GetMachineOpcode() == MOP_clinit_tail) {
        depAnalysis->CombineClinit(**it, **(nextIt), false);
      } else if ((*nextIt)->GetType() == kNodeTypeSeparator) {
        nextIt = std::next(nextIt, 1);
        if (nextIt == nodes.end()) {
          return;
        }
        if ((*nextIt)->GetInsn()->GetMachineOpcode() == MOP_clinit_tail) {
          /* Do something. */
          depAnalysis->CombineClinit(**it, **(nextIt), true);
        }
      }
    }
  }
}

/* Return the next node's index who is kNodeTypeSeparator. */
uint32 AArch64Schedule::GetNextSepIndex() const {
  return ((lastSeparatorIndex + kMaxDependenceNum) < nodeSize) ? (lastSeparatorIndex + kMaxDependenceNum)
                                                               : (nodes.size() - 1);
}

/* Do register pressure schduling. */
void AArch64Schedule::RegPressureScheduling(BB &bb, MapleVector<DepNode*> &nodes) {
  RegPressureSchedule *regSchedule = memPool.New<RegPressureSchedule>(cgFunc, alloc);
  /*
   * Get physical register amount currently
   * undef, Int Reg, Float Reg, Flag Reg
   */
  const std::vector<int32> kRegNumVec = { 0, V0, kMaxRegNum - V0 + 1, 1 };
  regSchedule->InitBBInfo(bb, memPool, nodes);
  regSchedule->BuildPhyRegInfo(kRegNumVec);
  regSchedule->DoScheduling(nodes);
}

/*
 * Compute earliest start of the node,
 * return value : the maximum estart.
 */
uint32 AArch64Schedule::ComputeEstart(uint32 cycle) {
  std::vector<DepNode*> readyNodes;
  uint32 maxIndex = GetNextSepIndex();

  if (CGOptions::IsDebugSched()) {
    /* Check validPredsSize. */
    for (uint32 i = lastSeparatorIndex; i <= maxIndex; ++i) {
      DepNode *node = nodes[i];
      int32 schedNum = 0;
      for (const auto *predLink : node->GetPreds()) {
        if (predLink->GetFrom().GetState() == kScheduled) {
          ++schedNum;
        }
      }
      ASSERT((node->GetPreds().size() - schedNum) == node->GetValidPredsSize(), "validPredsSize error.");
    }
  }

  ASSERT(nodes[maxIndex]->GetType() == kNodeTypeSeparator,
         "CG internal error, nodes[maxIndex] should be a separator node.");

  readyNodes.insert(readyNodes.begin(), readyList.begin(), readyList.end());

  uint32 maxEstart = cycle;
  for (uint32 i = lastSeparatorIndex; i <= maxIndex; ++i) {
    DepNode *node = nodes[i];
    node->SetVisit(0);
  }

  for (auto *node : readyNodes) {
    ASSERT(node->GetState() == kReady, "CG internal error, all nodes in ready list should be ready.");
    if (node->GetEStart() < cycle) {
      node->SetEStart(cycle);
    }
  }

  while (!readyNodes.empty()) {
    DepNode *node = readyNodes.front();
    readyNodes.erase(readyNodes.begin());

    for (const auto *succLink : node->GetSuccs()) {
      DepNode &succNode = succLink->GetTo();
      if (succNode.GetType() == kNodeTypeSeparator) {
        continue;
      }

      if (succNode.GetEStart() < (node->GetEStart() + succLink->GetLatency())) {
        succNode.SetEStart(node->GetEStart() + succLink->GetLatency());
      }
      maxEstart = (maxEstart < succNode.GetEStart() ? succNode.GetEStart() : maxEstart);
      succNode.IncreaseVisit();
      if ((succNode.GetVisit() >= succNode.GetValidPredsSize()) && (succNode.GetType() != kNodeTypeSeparator)) {
        readyNodes.push_back(&succNode);
      }
      ASSERT(succNode.GetVisit() <= succNode.GetValidPredsSize(), "CG internal error.");
    }
  }

  return maxEstart;
}

/* Compute latest start of the node. */
void AArch64Schedule::ComputeLstart(uint32 maxEstart) {
  /* std::vector is better than std::queue in run time */
  std::vector<DepNode*> readyNodes;
  uint32 maxIndex = GetNextSepIndex();

  ASSERT(nodes[maxIndex]->GetType() == kNodeTypeSeparator,
         "CG internal error, nodes[maxIndex] should be a separator node.");

  for (uint32 i = lastSeparatorIndex; i <= maxIndex; ++i) {
    DepNode *node = nodes[i];
    node->SetLStart(maxEstart);
    node->SetVisit(0);
  }

  readyNodes.push_back(nodes[maxIndex]);
  while (!readyNodes.empty()) {
    DepNode *node = readyNodes.front();
    readyNodes.erase(readyNodes.begin());
    for (const auto *predLink : node->GetPreds()) {
      DepNode &predNode = predLink->GetFrom();
      if (predNode.GetState() == kScheduled) {
        continue;
      }

      if (predNode.GetLStart() > (node->GetLStart() - predLink->GetLatency())) {
        predNode.SetLStart(node->GetLStart() - predLink->GetLatency());
      }
      predNode.IncreaseVisit();
      if ((predNode.GetVisit() >= predNode.GetValidSuccsSize()) && (predNode.GetType() != kNodeTypeSeparator)) {
        readyNodes.push_back(&predNode);
      }

      ASSERT(predNode.GetVisit() <= predNode.GetValidSuccsSize(), "CG internal error.");
    }
  }
}

/* Compute earliest start and latest start of the node that is in readyList and not be scheduled. */
void AArch64Schedule::UpdateELStartsOnCycle(uint32 cycle) {
  ComputeLstart(ComputeEstart(cycle));
}

/*
 * If all unit of this node need when it be scheduling is free, this node can be scheduled,
 * Return true.
 */
bool DepNode::CanBeScheduled() const {
  for (uint32 i = 0; i < unitNum; ++i) {
    Unit *unit = units[i];
    if (unit != nullptr) {
      if (!unit->IsFree(i)) {
        return false;
      }
    }
  }

  return true;
}

/* Mark those unit that this node need occupy unit when it is being scheduled. */
void DepNode::OccupyUnits() {
  for (uint32 i = 0; i < unitNum; ++i) {
    Unit *unit = units[i];
    if (unit != nullptr) {
      unit->Occupy(*insn, i);
    }
  }
}

/* Get unit kind of this node's units[0]. */
uint32 DepNode::GetUnitKind() const {
  uint32 retValue = 0;
  if ((units == nullptr) || (units[0] == nullptr)) {
    return retValue;
  }

  switch (units[0]->GetUnitId()) {
    case kUnitIdSlotD:
      retValue |= kUnitKindSlot0;
      break;
    case kUnitIdAgen:
    case kUnitIdSlotSAgen:
      retValue |= kUnitKindAgen;
      break;
    case kUnitIdSlotDAgen:
      retValue |= kUnitKindAgen;
      retValue |= kUnitKindSlot0;
      break;
    case kUnitIdHazard:
    case kUnitIdSlotSHazard:
      retValue |= kUnitKindHazard;
      break;
    case kUnitIdCrypto:
      retValue |= kUnitKindCrypto;
      break;
    case kUnitIdMul:
    case kUnitIdSlotSMul:
      retValue |= kUnitKindMul;
      break;
    case kUnitIdDiv:
      retValue |= kUnitKindDiv;
      break;
    case kUnitIdBranch:
    case kUnitIdSlotSBranch:
      retValue |= kUnitKindBranch;
      break;
    case kUnitIdStAgu:
      retValue |= kUnitKindStAgu;
      break;
    case kUnitIdLdAgu:
      retValue |= kUnitKindLdAgu;
      break;
    case kUnitIdFpAluS:
    case kUnitIdFpAluD:
      retValue |= kUnitKindFpAlu;
      break;
    case kUnitIdFpMulS:
    case kUnitIdFpMulD:
      retValue |= kUnitKindFpMul;
      break;
    case kUnitIdFpDivS:
    case kUnitIdFpDivD:
      retValue |= kUnitKindFpDiv;
      break;
    case kUnitIdSlot0LdAgu:
      retValue |= kUnitKindSlot0;
      retValue |= kUnitKindLdAgu;
      break;
    case kUnitIdSlot0StAgu:
      retValue |= kUnitKindSlot0;
      retValue |= kUnitKindStAgu;
      break;
    default:
      break;
  }

  return retValue;
}

/* Count unit kinds to an array. Each element of the array indicates the unit kind number of a node set. */
void AArch64Schedule::CountUnitKind(const DepNode &depNode, uint32 array[], const uint32 arraySize) const {
  (void)arraySize;
  ASSERT(arraySize >= kUnitKindLast, "CG internal error. unit kind number is not correct.");
  uint32 unitKind = depNode.GetUnitKind();
  int32 index = __builtin_ffs(unitKind);
  while (index) {
    ASSERT(index < kUnitKindLast, "CG internal error. index error.");
    ++array[index];
    unitKind &= ~(1u << (index - 1u));
    index = __builtin_ffs(unitKind);
  }
}

/* Check if a node use a specific unit kind. */
bool AArch64Schedule::IfUseUnitKind(const DepNode &depNode, uint32 index) {
  uint32 unitKind = depNode.GetUnitKind();
  int32 idx = __builtin_ffs(unitKind);
  while (idx) {
    ASSERT(index < kUnitKindLast, "CG internal error. index error.");
    if (idx == index) {
      return true;
    }
    unitKind &= ~(1u << (idx - 1u));
    idx = __builtin_ffs(unitKind);
  }

  return false;
}

/* A sample schedule according dependence graph only, to verify correctness of dependence graph. */
void AArch64Schedule::RandomTest() {
  Init();
  nodes.clear();

  while (!readyList.empty()) {
    DepNode *currNode = readyList.back();
    currNode->SetState(kScheduled);
    readyList.pop_back();
    nodes.push_back(currNode);

    for (auto succLink : currNode->GetSuccs()) {
      DepNode &succNode = succLink->GetTo();
      bool ready = true;
      for (auto predLink : succNode.GetPreds()) {
        DepNode &predNode = predLink->GetFrom();
        if (predNode.GetState() != kScheduled) {
          ready = false;
          break;
        }
      }

      if (ready) {
        ASSERT(succNode.GetState() == kNormal, "succNode must be kNormal");
        readyList.push_back(&succNode);
        succNode.SetState(kReady);
      }
    }
  }
}

/* Remove target from readyList. */
void AArch64Schedule::EraseNodeFromReadyList(const DepNode &target) {
  EraseNodeFromNodeList(target, readyList);
}

/* Remove target from nodeList. */
void AArch64Schedule::EraseNodeFromNodeList(const DepNode &target, MapleVector<DepNode*> &nodeList) {
  for (auto it = nodeList.begin(); it != nodeList.end(); ++it) {
    if ((*it) == &target) {
      nodeList.erase(it);
      return;
    }
  }

  ASSERT(false, "CG internal error, erase node fail.");
}

/* Dump all node of availableReadyList schedule information in current cycle. */
void AArch64Schedule::DumpDebugInfo(const ScheduleProcessInfo &scheduleInfo) {
  LogInfo::MapleLogger() << "Current cycle[ " << scheduleInfo.GetCurrCycle() << " ], Available in readyList is : \n";
  for (auto node : scheduleInfo.GetAvailableReadyList()) {
    LogInfo::MapleLogger() << "NodeIndex[ " << node->GetIndex()
                           << " ], Estart[ " << node->GetEStart() << " ], Lstart[ ";
    LogInfo::MapleLogger() << node->GetLStart() << " ], slot[ ";
    LogInfo::MapleLogger() <<
        (node->GetReservation() == nullptr ? "SlotNone" : node->GetReservation()->GetSlotName()) << " ], ";
    LogInfo::MapleLogger() << "succNodeNum[ " << node->GetSuccs().size() << " ], ";
    node->GetInsn()->Dump();
    LogInfo::MapleLogger() << '\n';
  }
}

/*
 * Select a node from availableReadyList according to some heuristic rules, then:
 *   1. change targetNode's schedule information;
 *   2. try to add successors of targetNode to readyList;
 *   3. update unscheduled node set, when targetNode is last kNodeTypeSeparator;
 *   4. update AdvanceCycle.
 */
void AArch64Schedule::SelectNode(ScheduleProcessInfo &scheduleInfo) {
  auto &availableReadyList = scheduleInfo.GetAvailableReadyList();
  auto it = availableReadyList.begin();
  DepNode *targetNode = *it;
  if (availableReadyList.size() > 1) {
    CalculateMaxUnitKindCount(scheduleInfo);
    ++it;
    for (; it != availableReadyList.end(); ++it) {
      if (CompareDepNode(**it, *targetNode)) {
        targetNode = *it;
      }
    }
  }
  scheduleInfo.PushElemIntoScheduledNodes(targetNode);
  EraseNodeFromReadyList(*targetNode);

  if (CGOptions::IsDebugSched()) {
    LogInfo::MapleLogger() << "TargetNode : ";
    targetNode->GetInsn()->Dump();
    LogInfo::MapleLogger() << "\n";
  }

  /* Update readyList. */
  UpdateReadyList(*targetNode, readyList, true);

  if (targetNode->GetType() == kNodeTypeSeparator) {
    /* If target node is separator node, update lastSeparatorIndex and calculate those depNodes's estart and lstart
     * between current separator node  and new Separator node.
     */
    if (!scheduleInfo.IsFirstSeparator()) {
      lastSeparatorIndex += kMaxDependenceNum;
      UpdateELStartsOnCycle(scheduleInfo.GetCurrCycle());
    } else {
      scheduleInfo.ResetIsFirstSeparator();
    }
  }

  switch (targetNode->GetInsn()->GetLatencyType()) {
    case kLtClinit:
      scheduleInfo.SetAdvanceCycle(kClinitAdvanceCycle);
      break;
    case kLtAdrpLdr:
      scheduleInfo.SetAdvanceCycle(kAdrpLdrAdvanceCycle);
      break;
    case kLtClinitTail:
      scheduleInfo.SetAdvanceCycle(kClinitTailAdvanceCycle);
      break;
    default:
      break;
  }

  if ((scheduleInfo.GetAdvanceCycle() == 0) && mad->IsFullIssued()) {
    scheduleInfo.SetAdvanceCycle(1);
  }
}

/*
 * Advance mad's cycle until info's advanceCycle equal zero,
 * and then clear info's availableReadyList.
 */
void AArch64Schedule::UpdateScheduleProcessInfo(ScheduleProcessInfo &info) {
  while (info.GetAdvanceCycle() > 0) {
    info.IncCurrCycle();
    mad->AdvanceCycle();
    info.DecAdvanceCycle();
  }
  info.ClearAvailableReadyList();
}

/*
 * Forward traversal readyList, if a node in readyList can be Schedule, add it to availableReadyList.
 * Return true, if availableReadyList is not empty.
 */
bool AArch64Schedule::CheckSchedulable(ScheduleProcessInfo &info) const {
  for (auto node : readyList) {
    if (node->CanBeScheduled() && node->GetEStart() <= info.GetCurrCycle()) {
      info.PushElemIntoAvailableReadyList(node);
    }
  }

  if (info.AvailableReadyListIsEmpty()) {
    return false;
  }
  return true;
}

/* After building dependence graph, schedule insns. */
uint32 AArch64Schedule::DoSchedule() {
  ScheduleProcessInfo scheduleInfo(nodeSize);
  Init();
  UpdateELStartsOnCycle(scheduleInfo.GetCurrCycle());

  while (!readyList.empty()) {
    UpdateScheduleProcessInfo(scheduleInfo);
    /* Check if schedulable */
    if (!CheckSchedulable(scheduleInfo)) {
      /* Advance cycle. */
      scheduleInfo.SetAdvanceCycle(1);
      continue;
    }

    if (scheduleInfo.GetLastUpdateCycle() < scheduleInfo.GetCurrCycle()) {
      scheduleInfo.SetLastUpdateCycle(scheduleInfo.GetCurrCycle());
    }

    if (CGOptions::IsDebugSched()) {
      DumpDebugInfo(scheduleInfo);
    }

    /* Select a node to scheduling */
    SelectNode(scheduleInfo);
  }

  ASSERT(scheduleInfo.SizeOfScheduledNodes() == nodes.size(), "CG internal error, Not all nodes scheduled.");

  nodes.clear();
  nodes.insert(nodes.begin(), scheduleInfo.GetScheduledNodes().begin(), scheduleInfo.GetScheduledNodes().end());
  /* the second to last node is the true last node, because the last is kNodeTypeSeparator node */
  ASSERT(nodes.size() - 2 >= 0, "size of nodes should be greater than or equal 2");
  return (nodes[nodes.size() - 2]->GetSchedCycle());
}

/*
 * Comparing priorities of node1 and node2 according to some heuristic rules
 * return true if node1's priority is higher
 */
bool AArch64Schedule::CompareDepNode(const DepNode &node1, const DepNode &node2) {
  /* less LStart first */
  if (node1.GetLStart() != node2.GetLStart()) {
    return node1.GetLStart() < node2.GetLStart();
  }

  /* max unit kind use */
  bool use1 = IfUseUnitKind(node1, maxUnitIndex);
  bool use2 = IfUseUnitKind(node2, maxUnitIndex);
  if (use1 != use2) {
    return use1;
  }

  /* slot0 first */
  SlotType slotType1 = node1.GetReservation()->GetSlot();
  SlotType slotType2 = node2.GetReservation()->GetSlot();
  if (slotType1 == kSlots) {
    slotType1 = kSlot0;
  }
  if (slotType2 == kSlots) {
    slotType2 = kSlot0;
  }
  if (slotType1 != slotType2) {
    return slotType1 < slotType2;
  }

  /* more succNodes fisrt */
  if (node1.GetSuccs().size() != node2.GetSuccs().size()) {
    return node1.GetSuccs().size() > node2.GetSuccs().size();
  }

  /* default order */
  return node1.GetInsn()->GetId() < node2.GetInsn()->GetId();
}

/*
 * Calculate number of every unit that used by avaliableReadyList's nodes and save the max in maxUnitIndex
 */
void AArch64Schedule::CalculateMaxUnitKindCount(ScheduleProcessInfo &scheduleInfo) {
  uint32 unitKindCount[kUnitKindLast] = { 0 };
  for (auto node : scheduleInfo.GetAvailableReadyList()) {
    CountUnitKind(*node, unitKindCount, kUnitKindLast);
  }

  uint32 maxCount = 0;
  maxUnitIndex = 0;
  for (size_t i = 1; i < kUnitKindLast; ++i) {
    if (maxCount < unitKindCount[i]) {
      maxCount = unitKindCount[i];
      maxUnitIndex = i;
    }
  }
}

/*
 * A simulated schedule:
 * scheduling instruction in original order to calculate original execute cycles.
 */
uint32 AArch64Schedule::SimulateOnly() {
  uint32 currCycle = 0;
  uint32 advanceCycle = 0;
  Init();

  for (uint32 i = 0; i < nodes.size();) {
    while (advanceCycle > 0) {
      ++currCycle;
      mad->AdvanceCycle();
      --advanceCycle;
    }

    DepNode *targetNode = nodes[i];
    if ((currCycle >= targetNode->GetEStart()) && targetNode->CanBeScheduled()) {
      targetNode->SetSimulateCycle(currCycle);
      targetNode->OccupyUnits();

      /* Update estart. */
      for (auto succLink : targetNode->GetSuccs()) {
        DepNode &succNode = succLink->GetTo();
        uint32 eStart = currCycle + succLink->GetLatency();
        if (succNode.GetEStart() < eStart) {
          succNode.SetEStart(eStart);
        }
      }

      if (CGOptions::IsDebugSched()) {
        LogInfo::MapleLogger() << "[Simulate] TargetNode : ";
        targetNode->GetInsn()->Dump();
        LogInfo::MapleLogger() << "\n";
      }

      switch (targetNode->GetInsn()->GetLatencyType()) {
        case kLtClinit:
          advanceCycle = kClinitAdvanceCycle;
          break;
        case kLtAdrpLdr:
          advanceCycle = kAdrpLdrAdvanceCycle;
          break;
        case kLtClinitTail:
          advanceCycle = kClinitTailAdvanceCycle;
          break;
        default:
          break;
      }

      ++i;
    } else {
      advanceCycle = 1;
    }
  }
  /* the second to last node is the true last node, because the last is kNodeTypeSeparator nod */
  ASSERT(nodes.size() - 2 >= 0, "size of nodes should be greater than or equal 2");
  return (nodes[nodes.size() - 2]->GetSimulateCycle());
}

/* Restore dependence graph to normal CGIR. */
void AArch64Schedule::FinalizeScheduling(BB &bb, const DepAnalysis &depAnalysis) {
  bb.ClearInsns();

  const Insn *prevLocInsn = (bb.GetPrev() != nullptr ? bb.GetPrev()->GetLastLoc() : nullptr);
  for (auto node : nodes) {
    /* Append comments first. */
    for (auto comment : node->GetComments()) {
      bb.AppendInsn(*comment);
    }
    /* Append insn. */
    if (!node->GetClinitInsns().empty()) {
      for (auto clinit : node->GetClinitInsns()) {
        bb.AppendInsn(*clinit);
      }
    } else if (node->GetType() == kNodeTypeNormal) {
      bb.AppendInsn(*node->GetInsn());
    }

    /* Append cfi instructions. */
    for (auto cfi : node->GetCfiInsns()) {
      bb.AppendInsn(*cfi);
    }
  }
  bb.SetLastLoc(prevLocInsn);

  for (auto lastComment : depAnalysis.GetLastComments()) {
    bb.AppendInsn(*lastComment);
  }
}

/* For every node of nodes, update it's bruteForceSchedCycle. */
void AArch64Schedule::UpdateBruteForceSchedCycle() {
  for (auto node : nodes) {
    node->SetBruteForceSchedCycle(node->GetSchedCycle());
  }
}

/* Recursively schedule all of the possible node. */
void AArch64Schedule::IterateBruteForce(DepNode &targetNode, MapleVector<DepNode*> &readyList, uint32 currCycle,
                                        MapleVector<DepNode*> &scheduledNodes, uint32 &maxCycleCount,
                                        MapleVector<DepNode*> &optimizedScheduledNodes) {
  /* Save states. */
  constexpr int32 unitSize = 31;
  ASSERT(unitSize == mad->GetAllUnitsSize(), "CG internal error.");
  std::vector<uint32> occupyTable;
  occupyTable.resize(unitSize, 0);
  mad->SaveStates(occupyTable, unitSize);

  /* Schedule targetNode first. */
  targetNode.SetState(kScheduled);
  targetNode.SetSchedCycle(currCycle);
  scheduledNodes.push_back(&targetNode);

  MapleVector<DepNode*> tempList = readyList;
  EraseNodeFromNodeList(targetNode, tempList);
  targetNode.OccupyUnits();

  /* Update readyList. */
  UpdateReadyList(targetNode, tempList, true);

  if (targetNode.GetType() == kNodeTypeSeparator) {
    /* If target node is separator node, update lastSeparatorIndex. */
    lastSeparatorIndex += kMaxDependenceNum;
  }

  if (tempList.empty()) {
    ASSERT(scheduledNodes.size() == nodes.size(), "CG internal error, Not all nodes scheduled.");
    if (currCycle < maxCycleCount) {
      maxCycleCount = currCycle;
      UpdateBruteForceSchedCycle();
      optimizedScheduledNodes = scheduledNodes;
    }
  } else {
    uint32 advanceCycle = 0;
    switch (targetNode.GetInsn()->GetLatencyType()) {
      case kLtClinit:
        advanceCycle = kClinitAdvanceCycle;
        break;
      case kLtAdrpLdr:
        advanceCycle = kAdrpLdrAdvanceCycle;
        break;
      case kLtClinitTail:
        advanceCycle = kClinitTailAdvanceCycle;
        break;
      default:
        break;
    }

    do {
      std::vector<DepNode*> availableReadyList;
      std::vector<DepNode*> tempAvailableList;
      while (advanceCycle > 0) {
        ++currCycle;
        mad->AdvanceCycle();
        --advanceCycle;
      }
      /* Check EStart. */
      for (auto node : tempList) {
        if (node->GetEStart() <= currCycle) {
          tempAvailableList.push_back(node);
        }
      }

      if (tempAvailableList.empty()) {
        /* Advance cycle. */
        advanceCycle = 1;
        continue;
      }

      /* Check if schedulable */
      for (auto node : tempAvailableList) {
        if (node->CanBeScheduled()) {
          availableReadyList.push_back(node);
        }
      }

      if (availableReadyList.empty()) {
        /* Advance cycle. */
        advanceCycle = 1;
        continue;
      }

      for (auto node : availableReadyList) {
        IterateBruteForce(*node, tempList, currCycle, scheduledNodes, maxCycleCount, optimizedScheduledNodes);
      }

      break;
    } while (true);
  }

  /*
   * Recover states.
   * Restore targetNode first.
   */
  targetNode.SetState(kReady);
  targetNode.SetSchedCycle(0);
  scheduledNodes.pop_back();
  mad->RestoreStates(occupyTable, unitSize);

  /* Update readyList. */
  for (auto succLink : targetNode.GetSuccs()) {
    DepNode &succNode = succLink->GetTo();
    succNode.IncreaseValidPredsSize();
    succNode.SetState(kNormal);
  }

  if (targetNode.GetType() == kNodeTypeSeparator) {
    /* If target node is separator node, update lastSeparatorIndex. */
    lastSeparatorIndex -= kMaxDependenceNum;
  }
}

/*
 * Brute force schedule:
 * Finding all possibile schedule list of current bb, and calculate every list's execute cycles,
 * return the optimal schedule list and it's cycles.
 */
uint32 AArch64Schedule::DoBruteForceSchedule() {
  MapleVector<DepNode*> scheduledNodes(alloc.Adapter());
  MapleVector<DepNode*> optimizedScheduledNodes(alloc.Adapter());

  uint32 currCycle = 0;
  uint32 maxCycleCount = 0xFFFFFFFF;
  Init();

  /* Schedule First separator. */
  DepNode *targetNode = readyList.front();
  targetNode->SetState(kScheduled);
  targetNode->SetSchedCycle(currCycle);
  scheduledNodes.push_back(targetNode);
  readyList.clear();

  /* Update readyList. */
  UpdateReadyList(*targetNode, readyList, false);

  ASSERT(targetNode->GetType() == kNodeTypeSeparator, "The first node should be separator node.");
  ASSERT(!readyList.empty(), "readyList should not be empty.");

  for (auto targetNodeTemp : readyList) {
    IterateBruteForce(*targetNodeTemp, readyList, currCycle, scheduledNodes, maxCycleCount, optimizedScheduledNodes);
  }

  nodes = optimizedScheduledNodes;
  return maxCycleCount;
}

/*
 * Update ready list after the targetNode has been scheduled.
 * For every targetNode's successor, if it's all predecessors have been scheduled,
 * add it to ready list and update it's information (like state, estart).
 */
void AArch64Schedule::UpdateReadyList(DepNode &targetNode, MapleVector<DepNode*> &readyList, bool updateEStart) {
  for (auto succLink : targetNode.GetSuccs()) {
    DepNode &succNode = succLink->GetTo();
    succNode.DescreaseValidPredsSize();
    if (succNode.GetValidPredsSize() == 0) {
      readyList.push_back(&succNode);
      succNode.SetState(kReady);

      /* Set eStart. */
      if (updateEStart) {
        uint32 maxEstart = 0;
        for (auto predLink : succNode.GetPreds()) {
          DepNode &predNode = predLink->GetFrom();
          uint32 eStart = predNode.GetSchedCycle() + predLink->GetLatency();
          maxEstart = (maxEstart < eStart ? eStart : maxEstart);
        }
        succNode.SetEStart(maxEstart);
      }
    }
  }
}

/* For every node of nodes, dump it's Depdence information. */
void AArch64Schedule::DumpDepGraph(const MapleVector<DepNode*> &nodes) const {
  for (auto node : nodes) {
    depAnalysis->DumpDepNode(*node);
    LogInfo::MapleLogger() << "---------- preds ----------" << "\n";
    for (auto pred : node->GetPreds()) {
      depAnalysis->DumpDepLink(*pred, &(pred->GetFrom()));
    }
    LogInfo::MapleLogger() << "---------- succs ----------" << "\n";
    for (auto succ : node->GetSuccs()) {
      depAnalysis->DumpDepLink(*succ, &(succ->GetTo()));
    }
    LogInfo::MapleLogger() << "---------------------------" << "\n";
  }
}

/* For every node of nodes, dump it's schedule time according simulate type and instruction information. */
void AArch64Schedule::DumpScheduleResult(const MapleVector<DepNode*> &nodes, SimulateType type) const {
  for (auto node : nodes) {
    LogInfo::MapleLogger() << "cycle[ ";
    switch (type) {
      case kListSchedule:
        LogInfo::MapleLogger() << node->GetSchedCycle();
        break;
      case kBruteForce:
        LogInfo::MapleLogger() << node->GetBruteForceSchedCycle();
        break;
      case kSimulateOnly:
        LogInfo::MapleLogger() << node->GetSimulateCycle();
        break;
    }
    LogInfo::MapleLogger() << " ]  ";
    node->GetInsn()->Dump();
    LogInfo::MapleLogger() << "\n";
  }
}

/* Print bb's dependence dot graph information to a file. */
void AArch64Schedule::GenerateDot(const BB &bb, const MapleVector<DepNode*> &nodes) const {
  std::streambuf *coutBuf = std::cout.rdbuf();  /* keep original cout buffer */
  std::ofstream dgFile;
  std::streambuf *buf = dgFile.rdbuf();
  std::cout.rdbuf(buf);

  /* construct the file name */
  std::string fileName;
  fileName.append(phaseName);
  fileName.append("_");
  fileName.append(cgFunc.GetName());
  fileName.append("_BB");
  auto str = std::to_string(bb.GetId());
  fileName.append(str);
  fileName.append("_dep_graph.dot");

  dgFile.open(fileName.c_str(), std::ios::trunc);
  if (!dgFile.is_open()) {
    LogInfo::MapleLogger(kLlWarn) << "fileName:" << fileName << " open failure.\n";
    return;
  }
  dgFile << "digraph {\n";
  for (auto node : nodes) {
    for (auto succ : node->GetSuccs()) {
      dgFile << "insn" << node->GetInsn() << " -> " << "insn" << succ->GetTo().GetInsn();
      dgFile << " [";
      if (succ->GetDepType() == kDependenceTypeTrue) {
        dgFile << "color=red,";
      }
      dgFile << "label= \"" << succ->GetLatency() << "\"";
      dgFile << "];\n";
    }
  }

  for (auto node : nodes) {
    MOperator mOp = node->GetInsn()->GetMachineOpcode();
    const AArch64MD *md = &AArch64CG::kMd[mOp];
    dgFile << "insn" << node->GetInsn() << "[";
    dgFile << "shape=box,label= \" " << node->GetInsn()->GetId() << ":\n";
    dgFile << "{ ";
    dgFile << md->name << "\n";
    dgFile << "}\"];\n";
  }
  dgFile << "}\n";
  dgFile.flush();
  dgFile.close();
  std::cout.rdbuf(coutBuf);
}

/* Do brute force scheduling and dump scheduling information */
void AArch64Schedule::BruteForceScheduling(const BB &bb) {
  LogInfo::MapleLogger() << "\n\n$$ Function: " << cgFunc.GetName();
  LogInfo::MapleLogger() << "\n    BB id = " << bb.GetId() << "; nodes.size = " << nodes.size() << "\n";

  constexpr uint32 maxBruteForceNum = 50;
  if (nodes.size() < maxBruteForceNum) {
    GenerateDot(bb, nodes);
    uint32 maxBruteForceCycle = DoBruteForceSchedule();
    MapleVector<DepNode*> bruteNodes = nodes;
    uint32 maxSchedCycle = DoSchedule();
    if (maxBruteForceCycle < maxSchedCycle) {
      LogInfo::MapleLogger() << "maxBruteForceCycle = " << maxBruteForceCycle << "; maxSchedCycle = ";
      LogInfo::MapleLogger() << maxSchedCycle << "\n";
      LogInfo::MapleLogger() << "\n    ## Dump dependence graph ##    " << "\n";
      DumpDepGraph(nodes);
      LogInfo::MapleLogger() << "\n    ** Dump bruteForce scheduling result." << "\n";
      DumpScheduleResult(bruteNodes, kBruteForce);
      LogInfo::MapleLogger() << "\n    ^^ Dump list scheduling result." << "\n";
      DumpScheduleResult(nodes, kListSchedule);
    }
  } else {
    LogInfo::MapleLogger() << "Skip BruteForce scheduling." << "\n";
    DoSchedule();
  }
}

/* Do simulate scheduling and dump scheduling information */
void AArch64Schedule::SimulateScheduling(const BB &bb) {
  uint32 originCycle = SimulateOnly();
  MapleVector<DepNode*> oldNodes = nodes;
  uint32 schedCycle = DoSchedule();
  if (originCycle < schedCycle) {
    LogInfo::MapleLogger() << "Worse cycle [ " << (schedCycle - originCycle) << " ]; ";
    LogInfo::MapleLogger() << "originCycle = " << originCycle << "; schedCycle = ";
    LogInfo::MapleLogger() << schedCycle << "; nodes.size = " << nodes.size();
    LogInfo::MapleLogger() << ";    $$ Function: " << cgFunc.GetName();
    LogInfo::MapleLogger() << ";    BB id = " << bb.GetId() << "\n";
    LogInfo::MapleLogger() << "\n    ** Dump original result." << "\n";
    DumpScheduleResult(oldNodes, kSimulateOnly);
    LogInfo::MapleLogger() << "\n    ^^ Dump list scheduling result." << "\n";
    DumpScheduleResult(nodes, kListSchedule);
  } else if (originCycle > schedCycle) {
    LogInfo::MapleLogger() << "Advance cycle [ " << (originCycle - schedCycle) << " ]; ";
    LogInfo::MapleLogger() << "originCycle = " << originCycle << "; schedCycle = ";
    LogInfo::MapleLogger() << schedCycle << "; nodes.size = " << nodes.size();
    LogInfo::MapleLogger() << ";    $$ Function: " << cgFunc.GetName();
    LogInfo::MapleLogger() << ";    BB id = " << bb.GetId() << "\n";
  } else {
    LogInfo::MapleLogger() << "Equal cycle [ 0 ]; originCycle = " << originCycle;
    LogInfo::MapleLogger() << " ], ignore. nodes.size = " << nodes.size() << "\n";
  }
}

/*
 * A local list scheduling.
 * Schedule insns in basic blocks.
 */
void AArch64Schedule::ListScheduling(bool beforeRA) {
  InitIDAndLoc();

  mad = Globals::GetInstance()->GetMAD();
  if (beforeRA) {
    RegPressure::SetMaxRegClassNum(kRegisterLast);
  }
  depAnalysis = memPool.New<AArch64DepAnalysis>(cgFunc, memPool, *mad, beforeRA);

  FOR_ALL_BB(bb, &cgFunc) {
    depAnalysis->Run(*bb, nodes);

    if (LIST_SCHED_DUMP_REF) {
      GenerateDot(*bb, nodes);
      DumpDepGraph(nodes);
    }
    if (beforeRA) {
      RegPressureScheduling(*bb, nodes);
    } else {
      ClinitPairOpt();
      MemoryAccessPairOpt();
      if (CGOptions::IsDruteForceSched()) {
        BruteForceScheduling(*bb);
      } else if (CGOptions::IsSimulateSched()) {
        SimulateScheduling(*bb);
      } else {
        DoSchedule();
      }
    }

    FinalizeScheduling(*bb, *depAnalysis);
  }
}
}  /* namespace maplebe */
