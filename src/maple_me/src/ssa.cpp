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
void SSA::InitRenameStack(OriginalStTable &otable, size_t bbsize, VersionStTable &versttab) {
  vstStacks.resize(otable.Size());
  vstVersions.resize(otable.Size(), 0);
  bbRenamed.resize(bbsize, false);
  for (size_t i = 1; i < otable.Size(); i++) {
    MapleStack<VersionSt*> *vstack = ssaAlloc.GetMemPool()->New<MapleStack<VersionSt*>>(ssaAlloc.Adapter());
    const OriginalSt *ost = otable.GetOriginalStFromID(OStIdx(i));
    VersionSt *temp = (ost->GetIndirectLev() >= 0) ? versttab.GetVersionStFromID(ost->GetZeroVersionIndex(), true)
                                                   : &versttab.GetDummyVersionSt();
    vstack->push(temp);
    vstStacks[i] = vstack;
  }
}

VersionSt *SSA::CreateNewVersion(VersionSt &vsym, BB &defBB) {
  CHECK_FATAL(vsym.GetVersion() == 0, "rename before?");
  // volatile variables will keep zero version.
  if (vsym.GetOrigSt()->IsVolatile()) {
    return &vsym;
  }
  ASSERT(vsym.GetOrigIdx().idx < vstVersions.size(), "index out of range in SSA::CreateNewVersion");
  VersionSt *newVersionSym = ssaTab->GetVersionStTable().CreateVSymbol(&vsym, ++vstVersions[vsym.GetOrigIdx().idx]);
  vstStacks[vsym.GetOrigIdx().idx]->push(newVersionSym);
  newVersionSym->SetDefBB(&defBB);
  return newVersionSym;
}

void SSA::RenamePhi(BB &bb) {
  for (auto phiIt = bb.GetPhiList().begin(); phiIt != bb.GetPhiList().end(); phiIt++) {
    VersionSt *vsym = (*phiIt).second.GetResult();
    // It shows that this BB has been renamed.
    if (vsym->GetVersion() > 0) {
      return;
    }
    VersionSt *newVersionSym = CreateNewVersion(*vsym, bb);
    (*phiIt).second.SetResult(*newVersionSym);
    newVersionSym->SetDefType(VersionSt::kPhi);
    newVersionSym->SetPhi(&(*phiIt).second);
  }
}

void SSA::RenameDefs(StmtNode &stmt, BB &defBB) {
  Opcode opcode = stmt.GetOpCode();
  if (opcode == OP_regassign) {
    RegassignNode &regNode = static_cast<RegassignNode&>(stmt);
    if (regNode.GetRegIdx() < 0) {
      return;
    }
    AccessSSANodes *theSSAPart = ssaTab->GetStmtsSSAPart().SSAPartOf(stmt);
    VersionSt *newVersionSym = CreateNewVersion(*(theSSAPart->GetSSAVar()), defBB);
    newVersionSym->SetDefType(VersionSt::kRegassign);
    newVersionSym->SetRegassignNode(&regNode);
    theSSAPart->SetSSAVar(*newVersionSym);
    return;
  } else if (opcode == OP_dassign) {
    AccessSSANodes *theSSAPart = ssaTab->GetStmtsSSAPart().SSAPartOf(stmt);
    VersionSt *newVersionSym = CreateNewVersion(*theSSAPart->GetSSAVar(), defBB);
    newVersionSym->SetDefType(VersionSt::kDassign);
    newVersionSym->SetDassignNode(static_cast<DassignNode*>(&stmt));
    theSSAPart->SetSSAVar(*newVersionSym);
  }
  if (HasMayDefPart(stmt)) {
    MapleMap<OStIdx, MayDefNode> &mayDefList = SSAGenericGetMayDefNodes(stmt, ssaTab->GetStmtsSSAPart());
    for (auto it = mayDefList.begin(); it != mayDefList.end(); it++) {
      MayDefNode &mayDef = it->second;
      VersionSt *vsym = mayDef.GetResult();
      ASSERT(vsym->GetOrigIdx().idx < vstStacks.size(), "index out of range in SSA::RenameMayDefs");
      mayDef.SetOpnd(vstStacks[vsym->GetOrigIdx().idx]->top());
      VersionSt *newVersionSym = CreateNewVersion(*vsym, defBB);
      mayDef.SetResult(newVersionSym);
      newVersionSym->SetDefType(VersionSt::kMayDef);
      newVersionSym->SetMayDef(&mayDef);
    }
  }
}

void SSA::RenameMustDefs(const StmtNode &stmt, BB &defBB) {
  Opcode opcode = stmt.GetOpCode();
  if (kOpcodeInfo.IsCallAssigned(opcode)) {
    MapleVector<MustDefNode> &mustDefs = SSAGenericGetMustDefNode(stmt, ssaTab->GetStmtsSSAPart());
    for (MustDefNode &mustDefNode : mustDefs) {
      VersionSt *newVersionSym = CreateNewVersion(*mustDefNode.GetResult(), defBB);
      mustDefNode.SetResult(newVersionSym);
      newVersionSym->SetDefType(VersionSt::kMustDef);
      newVersionSym->SetMustDef(&(mustDefNode));
    }
  }
}

void SSA::RenameMayUses(BaseNode &node) {
  if (node.GetOpCode() == OP_iread) {
    IreadSSANode &iread = static_cast<IreadSSANode&>(node);
    VersionSt *vsym = iread.GetSSAVar();
    CHECK_FATAL(vsym != nullptr, "SSA::RenameMayUses: iread has no mayUse opnd");
    ASSERT(vsym->GetOrigIdx().idx < vstStacks.size(), "index out of range in SSA::RenameMayUses");
    iread.SetSSAVar(vstStacks[vsym->GetOrigIdx().idx]->top());
    return;
  }
  MapleMap<OStIdx, MayUseNode> &mayUseList =
      SSAGenericGetMayUseNode(static_cast<StmtNode&>(node), ssaTab->GetStmtsSSAPart());
  MapleMap<OStIdx, MayUseNode>::iterator it = mayUseList.begin();
  for (; it != mayUseList.end(); it++) {
    MayUseNode &mayuse = it->second;
    VersionSt *vsym = mayuse.GetOpnd();
    ASSERT(vsym->GetOrigIdx().idx < vstStacks.size(), "index out of range in SSA::RenameMayUses");
    mayuse.SetOpnd(vstStacks.at(vsym->GetOrigIdx().idx)->top());
  }
}

void SSA::RenameExpr(BaseNode &expr) {
  if (expr.GetOpCode() == OP_addrof || expr.GetOpCode() == OP_dread) {
    AddrofSSANode &addrofNode = static_cast<AddrofSSANode&>(expr);
    VersionSt *vsym = addrofNode.GetSSAVar();
    ASSERT(vsym->GetOrigIdx().idx < vstStacks.size(), "index out of range in SSA::RenameExpr");
    addrofNode.SetSSAVar(vstStacks[vsym->GetOrigIdx().idx]->top());
    return;
  } else if (expr.GetOpCode() == OP_regread) {
    RegreadSSANode &regNode = static_cast<RegreadSSANode&>(expr);
    if (regNode.GetRegIdx() < 0) {
      return;
    }
    VersionSt *vsym = regNode.GetSSAVar();
    ASSERT(vsym->GetOrigIdx().idx < vstStacks.size(), "index out of range in SSA::RenameExpr");
    regNode.SetSSAVar(vstStacks[vsym->GetOrigIdx().idx]->top());
    return;
  } else if (expr.GetOpCode() == OP_iread) {
    RenameMayUses(expr);
    RenameExpr(*expr.Opnd(0));
  } else {
    for (size_t i = 0; i < expr.NumOpnds(); i++) {
      RenameExpr(*expr.Opnd(i));
    }
  }
}

void SSA::RenameUses(StmtNode &stmt) {
  if (HasMayUsePart(stmt)) {
    RenameMayUses(stmt);
  }
  for (int i = 0; i < stmt.NumOpnds(); i++) {
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
    for (auto phiIt = succBB->GetPhiList().begin(); phiIt != succBB->GetPhiList().end(); phiIt++) {
      PhiNode &phiNode = phiIt->second;
      ASSERT(phiNode.GetPhiOpnd(index)->GetOrigIdx().idx < vstStacks.size(), "out of range SSA::RenamePhiUseInSucc");
      phiNode.SetPhiOpnd(index, *vstStacks.at(phiNode.GetPhiOpnd(index)->GetOrigIdx().idx)->top());
    }
  }
}

void PhiNode::Dump(const MIRModule *mod) {
  GetResult()->Dump(mod);
  LogInfo::MapleLogger() << " = PHI(";
  for (size_t i = 0; i < GetPhiOpnds().size(); i++) {
    GetPhiOpnd(i)->Dump(mod);
    if (i < GetPhiOpnds().size() - 1) {
      LogInfo::MapleLogger() << ',';
    }
  }
  LogInfo::MapleLogger() << ")" << '\n';
}
}  // namespace maple
