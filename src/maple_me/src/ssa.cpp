/*
 * Copyright (c) [2019] Huawei Technologies Co.,Ltd.All rights reserved.
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
#include "ssa.h"
#include <iostream>
#include "ssa_tab.h"
#include "ssa_mir_nodes.h"
#include "ver_symbol.h"

namespace maple {
void SSA::InitRenameStack(OriginalStTable &oTable, size_t bbSize, VersionStTable &verStTab) {
  vstStacks.resize(oTable.Size());
  vstVersions.resize(oTable.Size(), 0);
  bbRenamed.resize(bbSize, false);
  for (size_t i = 1; i < oTable.Size(); ++i) {
    MapleStack<VersionSt*> *vStack = ssaAlloc.GetMemPool()->New<MapleStack<VersionSt*>>(ssaAlloc.Adapter());
    const OriginalSt *ost = oTable.GetOriginalStFromID(OStIdx(i));
    VersionSt *temp = (ost->GetIndirectLev() >= 0) ? verStTab.GetVersionStFromID(ost->GetZeroVersionIndex(), true)
                                                   : &verStTab.GetDummyVersionSt();
    vStack->push(temp);
    vstStacks[i] = vStack;
  }
}

VersionSt *SSA::CreateNewVersion(VersionSt &vSym, BB &defBB) {
  CHECK_FATAL(vSym.GetVersion() == 0, "rename before?");
  // volatile variables will keep zero version.
  OriginalSt *oSt = vSym.GetOrigSt();
  if (oSt->IsVolatile() || oSt->IsSpecialPreg()) {
    return &vSym;
  }
  CHECK_FATAL(vSym.GetOrigIdx().idx < vstVersions.size(), "index out of range in SSA::CreateNewVersion");
  VersionSt *newVersionSym = ssaTab->GetVersionStTable().CreateVSymbol(&vSym, ++vstVersions[vSym.GetOrigIdx().idx]);
  vstStacks[vSym.GetOrigIdx().idx]->push(newVersionSym);
  newVersionSym->SetDefBB(&defBB);
  return newVersionSym;
}

void SSA::RenamePhi(BB &bb) {
  for (auto phiIt = bb.GetPhiList().begin(); phiIt != bb.GetPhiList().end(); ++phiIt) {
    VersionSt *vSym = (*phiIt).second.GetResult();
    // It shows that this BB has been renamed.
    if (vSym->GetVersion() > 0) {
      return;
    }
    VersionSt *newVersionSym = CreateNewVersion(*vSym, bb);
    (*phiIt).second.SetResult(*newVersionSym);
    newVersionSym->SetDefType(VersionSt::kPhi);
    newVersionSym->SetPhi(&(*phiIt).second);
  }
}

void SSA::RenameDefs(StmtNode &stmt, BB &defBB) {
  Opcode opcode = stmt.GetOpCode();
  AccessSSANodes *theSSAPart = ssaTab->GetStmtsSSAPart().SSAPartOf(stmt);
  if (kOpcodeInfo.AssignActualVar(opcode)) {
    VersionSt *newVersionSym = CreateNewVersion(*theSSAPart->GetSSAVar(), defBB);
    newVersionSym->SetDefType(VersionSt::kAssign);
    newVersionSym->SetAssignNode(&stmt);
    theSSAPart->SetSSAVar(*newVersionSym);
  }
  if (kOpcodeInfo.HasSSADef(opcode)) {
    MapleMap<OStIdx, MayDefNode> &mayDefList = theSSAPart->GetMayDefNodes();
    for (auto it = mayDefList.begin(); it != mayDefList.end(); it++) {
      MayDefNode &mayDef = it->second;
      VersionSt *vSym = mayDef.GetResult();
      CHECK_FATAL(vSym->GetOrigIdx().idx < vstStacks.size(), "index out of range in SSA::RenameMayDefs");
      mayDef.SetOpnd(vstStacks[vSym->GetOrigIdx().idx]->top());
      VersionSt *newVersionSym = CreateNewVersion(*vSym, defBB);
      mayDef.SetResult(newVersionSym);
      newVersionSym->SetDefType(VersionSt::kMayDef);
      newVersionSym->SetMayDef(&mayDef);
    }
  }
}

void SSA::RenameMustDefs(const StmtNode &stmt, BB &defBB) {
  Opcode opcode = stmt.GetOpCode();
  if (kOpcodeInfo.IsCallAssigned(opcode)) {
    MapleVector<MustDefNode> &mustDefs = ssaTab->GetStmtsSSAPart().GetMustDefNodesOf(stmt);
    for (MustDefNode &mustDefNode : mustDefs) {
      VersionSt *newVersionSym = CreateNewVersion(*mustDefNode.GetResult(), defBB);
      mustDefNode.SetResult(newVersionSym);
      newVersionSym->SetDefType(VersionSt::kMustDef);
      newVersionSym->SetMustDef(&(mustDefNode));
    }
  }
}

void SSA::RenameMayUses(BaseNode &node) {
  MapleMap<OStIdx, MayUseNode> &mayUseList = ssaTab->GetStmtsSSAPart().GetMayUseNodesOf(static_cast<StmtNode&>(node));
  MapleMap<OStIdx, MayUseNode>::iterator it = mayUseList.begin();
  for (; it != mayUseList.end(); it++) {
    MayUseNode &mayUse = it->second;
    VersionSt *vSym = mayUse.GetOpnd();
    CHECK_FATAL(vSym->GetOrigIdx().idx < vstStacks.size(), "index out of range in SSA::RenameMayUses");
    mayUse.SetOpnd(vstStacks.at(vSym->GetOrigIdx().idx)->top());
  }
}

void SSA::RenameExpr(BaseNode &expr) {
  if (expr.IsSSANode()) {
    auto &ssaNode = static_cast<AddrofSSANode&>(expr);
    VersionSt *vSym = ssaNode.GetSSAVar();
    CHECK_FATAL(vSym->GetOrigIdx().idx < vstStacks.size(), "index out of range in SSA::RenameExpr");
    ssaNode.SetSSAVar(*vstStacks[vSym->GetOrigIdx().idx]->top());
  }
  for (size_t i = 0; i < expr.NumOpnds(); ++i) {
    RenameExpr(*expr.Opnd(i));
  }
}

void SSA::RenameUses(StmtNode &stmt) {
  if (kOpcodeInfo.HasSSAUse(stmt.GetOpCode())) {
    RenameMayUses(stmt);
  }
  for (int i = 0; i < stmt.NumOpnds(); ++i) {
    RenameExpr(*stmt.Opnd(i));
  }
}

void SSA::RenamePhiUseInSucc(BB &bb) {
  for (BB *succBB : bb.GetSucc()) {
    // find index of bb in succ_bb->pred[]
    size_t index = 0;
    while (index < succBB->GetPred().size()) {
      if (succBB->GetPred(index) == &bb) {
        break;
      }
      index++;
    }
    CHECK_FATAL(index < succBB->GetPred().size(), "RenamePhiUseInSucc: cannot find corresponding pred");
    // rename the phiOpnds[index] in all the phis in succ_bb
    for (auto phiIt = succBB->GetPhiList().begin(); phiIt != succBB->GetPhiList().end(); ++phiIt) {
      PhiNode &phiNode = phiIt->second;
      CHECK_FATAL(phiNode.GetPhiOpnd(index)->GetOrigIdx().idx < vstStacks.size(),
                  "out of range SSA::RenamePhiUseInSucc");
      phiNode.SetPhiOpnd(index, *vstStacks.at(phiNode.GetPhiOpnd(index)->GetOrigIdx().idx)->top());
    }
  }
}

void PhiNode::Dump(const MIRModule *mod) {
  GetResult()->Dump(mod);
  LogInfo::MapleLogger() << " = PHI(";
  for (size_t i = 0; i < GetPhiOpnds().size(); ++i) {
    GetPhiOpnd(i)->Dump(mod);
    if (i < GetPhiOpnds().size() - 1) {
      LogInfo::MapleLogger() << ',';
    }
  }
  LogInfo::MapleLogger() << ")" << '\n';
}
}  // namespace maple
