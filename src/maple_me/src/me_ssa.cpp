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
#include "me_ssa.h"
#include <iostream>
#include "bb.h"
#include "me_option.h"
#include "ssa_mir_nodes.h"
#include "ver_symbol.h"
#include "dominance.h"
#include "me_function.h"

// This phase builds the SSA form of a function. Before this we have got the dominator tree
// and each bb's dominance frontiers. Then the algorithm follows this outline:
//
// Step 1: Inserting phi-node.
// With dominance frontiers, we can determine more
// precisely where phi-node might be needed. The basic idea is simple.
// A definition of x in block b forces a phi-node at every node in b's
// dominance frontiers. Since that phi-node is a new definition of x,
// it may, in turn, force the insertion of additional phi-node.
//
// Step 2: Renaming.
// Renames both definitions and uses of each symbol in
// a preorder walk over the dominator tree. In each block, we first
// rename the values defined by phi-node at the head of the block, then
// visit each stmt in the block: we rewrite each uses of a symbol with current
// SSA names(top of the stack which holds the current SSA version of the corresponding symbol),
// then it creates a new SSA name for the result of the stmt.
// This latter act makes the new name current. After all the stmts in the
// block have been rewritten, we rewrite the appropriate phi-node's
// parameters in each cfg successor of the block, using the current SSA names.
// Finally, it recurs on any children of the block in the dominator tree. When it
// returns from those recursive calls, we restores the stack of current SSA names to
// the state that existed before the current block was visited.
namespace maple {
void MeSSA::BuildSSA() {
  InsertPhiNode();
  InitRenameStack(func->GetMeSSATab()->GetOriginalStTable(), func->GetAllBBs().size(),
                  func->GetMeSSATab()->GetVersionStTable());
  // recurse down dominator tree in pre-order traversal
  const MapleSet<BBId> &children = dom->GetDomChildren(func->GetCommonEntryBB()->GetBBId());
  for (const auto &child : children) {
    RenameBB(*func->GetBBFromID(child));
  }
}

void MeSSA::CollectDefBBs(std::map<OStIdx, std::set<BBId>> &ostDefBBs) {
  auto eIt = func->valid_end();
  for (auto bIt = func->valid_begin(); bIt != eIt; ++bIt) {
    auto *bb = *bIt;
    for (auto &stmt : bb->GetStmtNodes()) {
      if (!kOpcodeInfo.HasSSADef(stmt.GetOpCode())) {
        continue;
      }
      MapleMap<OStIdx, MayDefNode> &mayDefs = GetSSATab()->GetStmtsSSAPart().GetMayDefNodesOf(stmt);
      for (auto iter = mayDefs.begin(); iter != mayDefs.end(); ++iter) {
        const OriginalSt *ost = func->GetMeSSATab()->GetOriginalStFromID(iter->first);
        if (ost != nullptr && (!ost->IsFinal() || func->GetMirFunc()->IsConstructor())) {
          ostDefBBs[iter->first].insert(bb->GetBBId());
        } else if (stmt.GetOpCode() == OP_intrinsiccallwithtype) {
          auto &inNode = static_cast<IntrinsiccallNode&>(stmt);
          if (inNode.GetIntrinsic() == INTRN_JAVA_CLINIT_CHECK) {
            ostDefBBs[iter->first].insert(bb->GetBBId());
          }
        }
      }
      if (stmt.GetOpCode() == OP_dassign || stmt.GetOpCode() == OP_maydassign) {
        VersionSt *vst = GetSSATab()->GetStmtsSSAPart().GetAssignedVarOf(stmt);
        OriginalSt *ost = vst->GetOrigSt();
        if (ost != nullptr && (!ost->IsFinal() || func->GetMirFunc()->IsConstructor())) {
          ostDefBBs[vst->GetOrigIdx()].insert(bb->GetBBId());
        }
      }
      // Needs to handle mustDef in callassigned stmt
      if (!kOpcodeInfo.IsCallAssigned(stmt.GetOpCode())) {
        continue;
      }
      MapleVector<MustDefNode> &mustDefs = GetSSATab()->GetStmtsSSAPart().GetMustDefNodesOf(stmt);
      for (auto iter = mustDefs.begin(); iter != mustDefs.end(); ++iter) {
        OriginalSt *ost = iter->GetResult()->GetOrigSt();
        if (ost != nullptr && (!ost->IsFinal() || func->GetMirFunc()->IsConstructor())) {
          ostDefBBs[ost->GetIndex()].insert(bb->GetBBId());
        }
      }
    }
  }
}

void MeSSA::InsertPhiNode() {
  std::map<OStIdx, std::set<BBId>> ost2DefBBs;
  CollectDefBBs(ost2DefBBs);
  OriginalStTable *otable = &func->GetMeSSATab()->GetOriginalStTable();
  for (size_t i = 1; i < otable->Size(); ++i) {
    OriginalSt *ost = otable->GetOriginalStFromID(OStIdx(i));
    VersionSt *vst = func->GetMeSSATab()->GetVersionStTable().GetVersionStFromID(ost->GetZeroVersionIndex(), true);
    CHECK_FATAL(vst != nullptr, "null ptr check");
    if (ost2DefBBs[ost->GetIndex()].empty()) {
      continue;
    }
    // volatile variables will not have ssa form.
    if (ost->IsVolatile()) {
      continue;
    }
    std::deque<BB*> *workList = new std::deque<BB*>();
    for (auto it = ost2DefBBs[ost->GetIndex()].begin(); it != ost2DefBBs[ost->GetIndex()].end(); ++it) {
      BB *defBB = func->GetAllBBs()[*it];
      if (defBB != nullptr) {
        workList->push_back(defBB);
      }
    }
    while (!workList->empty()) {
      BB *defBB = workList->front();
      workList->pop_front();
      MapleSet<BBId> &dfs = dom->GetDomFrontier(defBB->GetBBId());
      for (auto &bbID : dfs) {
        BB *dfBB = func->GetBBFromID(bbID);
        CHECK_FATAL(dfBB != nullptr, "null ptr check");
        if (dfBB->PhiofVerStInserted(*vst) == nullptr) {
          workList->push_back(dfBB);
          dfBB->InsertPhi(&func->GetAlloc(), vst);
          if (enabledDebug) {
            ost->Dump();
            LogInfo::MapleLogger() << " Defined In: BB" << defBB->GetBBId() << " Insert Phi Here: BB"
                                   << dfBB->GetBBId() << '\n';
          }
        }
      }
    }
    delete workList;
  }
}

MeSSA::MeSSA(MeFunction &func, Dominance &dom, MemPool &memPool, bool enabledDebug)
    : SSA(memPool, *func.GetMeSSATab()), AnalysisResult(&memPool), func(&func), dom(&dom), enabledDebug(enabledDebug) {}

void MeSSA::RenameBB(BB &bb) {
  if (GetBBRenamed(bb.GetBBId())) {
    return;
  }

  SetBBRenamed(bb.GetBBId(), true);

  // record stack size for variable versions before processing rename. It is used for stack pop up.
  std::vector<uint32> oriStackSize;
  oriStackSize.resize(GetVstStacks().size());
  for (size_t i = 1; i < GetVstStacks().size(); ++i) {
    oriStackSize[i] = GetVstStack(i)->size();
  }
  RenamePhi(bb);
  for (auto &stmt : bb.GetStmtNodes()) {
    RenameUses(stmt);
    RenameDefs(stmt, bb);
    RenameMustDefs(stmt, bb);
  }
  RenamePhiUseInSucc(bb);
  // Rename child in Dominator Tree.
  ASSERT(bb.GetBBId() < dom->GetDomChildrenSize(), "index out of range in MeSSA::RenameBB");
  const MapleSet<BBId> &children = dom->GetDomChildren(bb.GetBBId());
  for (const BBId &child : children) {
    RenameBB(*func->GetBBFromID(child));
  }
  for (size_t i = 1; i < GetVstStacks().size(); ++i) {
    while (GetVstStack(i)->size() > oriStackSize[i]) {
      PopVersionSt(i);
    }
  }
}

void MeSSA::VerifySSAOpnd(const BaseNode &node) const {
  Opcode op = node.GetOpCode();
  size_t vtableSize = func->GetMeSSATab()->GetVersionStTable().GetVersionStVectorSize();
  if (op == OP_dread || op == OP_addrof) {
    const auto &addrOfSSANode = static_cast<const AddrofSSANode&>(node);
    const VersionSt *verSt = addrOfSSANode.GetSSAVar();
    CHECK_FATAL(verSt->GetIndex() < vtableSize, "runtime check error");
    return;
  }
  if (op == OP_regread) {
    const auto &regNode = static_cast<const RegreadSSANode&>(node);
    const VersionSt *verSt = regNode.GetSSAVar();
    CHECK_FATAL(verSt->GetIndex() < vtableSize, "runtime check error");
    return;
  }

  for (size_t i = 0; i < node.NumOpnds(); ++i) {
    VerifySSAOpnd(*node.Opnd(i));
  }
}

void MeSSA::VerifySSA() const {
  size_t vtableSize = func->GetMeSSATab()->GetVersionStTable().GetVersionStVectorSize();
  auto eIt = func->valid_end();
  for (auto bIt = func->valid_begin(); bIt != eIt; ++bIt) {
    auto *bb = *bIt;
    Opcode opcode;
    for (auto &stmt : bb->GetStmtNodes()) {
      opcode = stmt.GetOpCode();
      if (opcode == OP_dassign || opcode == OP_regassign) {
        VersionSt *verSt = func->GetMeSSATab()->GetStmtsSSAPart().GetAssignedVarOf(stmt);
        CHECK_FATAL(verSt != nullptr && verSt->GetIndex() < vtableSize, "runtime check error");
      }
      for (size_t i = 0; i < stmt.NumOpnds(); ++i) {
        VerifySSAOpnd(*stmt.Opnd(i));
      }
    }
  }
}

AnalysisResult *MeDoSSA::Run(MeFunction *func, MeFuncResultMgr *funcResMgr, ModuleResultMgr *moduleResMgr) {
  auto *dom = static_cast<Dominance*>(funcResMgr->GetAnalysisResult(MeFuncPhase_DOMINANCE, func));
  CHECK_FATAL(dom != nullptr, "dominance phase has problem");
  auto *ssaTab = static_cast<SSATab*>(funcResMgr->GetAnalysisResult(MeFuncPhase_SSATAB, func));
  CHECK_FATAL(ssaTab != nullptr, "ssaTab phase has problem");
  MemPool *ssaMp = NewMemPool();
  MeSSA *ssa = ssaMp->New<MeSSA>(*func, *dom, *ssaMp, DEBUGFUNC(func));
  ssa->BuildSSA();
  ssa->VerifySSA();
  if (DEBUGFUNC(func)) {
    ssaTab->GetVersionStTable().Dump(&ssaTab->GetModule());
  }
  return ssa;
}
}  // namespace maple
