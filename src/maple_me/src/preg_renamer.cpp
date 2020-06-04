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
#include "preg_renamer.h"
#include "alias_class.h"
#include "mir_builder.h"
#include "me_irmap.h"

namespace maple {
void PregRenamer::EnqueDefUses(std::list<RegMeExpr*> &qu, RegMeExpr *node, std::set<RegMeExpr*> &curVisited) const {
  CHECK_NULL_FATAL(node);
  // get its define
  if (node->GetDefBy() == kDefByPhi) {
    MePhiNode &defPhi = node->GetDefPhi();
    for (auto it : defPhi.GetOpnds()) {
      RegMeExpr *neibNode = static_cast<RegMeExpr*>(it);  // node's connected register node
      if (neibNode != node && curVisited.find(neibNode) == curVisited.end()) {
        qu.push_back(neibNode);
        (void)curVisited.insert(neibNode);
      }
    }
  }
  // get the phi which uses node as an operand
  MapleSet<MePhiNode*> &phiUseSet = node->GetPhiUseSet();
  for (auto setIt : phiUseSet) {
    MePhiNode *meRegPhi = setIt;
    RegMeExpr *lhsReg = static_cast<RegMeExpr*>(meRegPhi->GetLHS());
    if (lhsReg != node && curVisited.find(lhsReg) == curVisited.end()) {
      qu.push_back(lhsReg);
      (void)curVisited.insert(lhsReg);
    }
    for (auto opdIt : meRegPhi->GetOpnds()) {
      RegMeExpr *opndReg = static_cast<RegMeExpr*>(opdIt);
      if (opndReg != node && curVisited.find(opndReg) == curVisited.end()) {
        qu.push_back(opndReg);
        (void)curVisited.insert(opndReg);
      }
    }
  }
}

void PregRenamer::RunSelf() const {
  // BFS the graph of register phi node;
  std::set<RegMeExpr*> curVisited;
  const MapleVector<RegMeExpr*> &regMeExprTable = irMap->GetRegMeExprTable();
  MIRPregTable *pregTab = func->GetMirFunc()->GetPregTab();
  std::vector<bool> firstAppearTable(pregTab->GetPregTable().size());
  uint32 renameCount = 0;
  for (auto it : regMeExprTable) {
    RegMeExpr *regMeExpr = it;
    if (regMeExpr->GetRegIdx() < 0) {
      continue;  // special register
    }
    if (curVisited.find(regMeExpr) != curVisited.end()) {
      continue;
    }
    // BFS the node and add all related nodes to the vector;
    std::vector<RegMeExpr*> candidates;
    std::list<RegMeExpr*> qu;
    qu.push_back(regMeExpr);
    candidates.push_back(regMeExpr);
    bool useDefFromZeroVersion = false;
    bool definedInTryBlock = false;
    while (!qu.empty()) {
      RegMeExpr *curNode = qu.back();
      qu.pop_back();
      // put all its neighbors into the queue
      EnqueDefUses(qu, curNode, curVisited);
      (void)curVisited.insert(curNode);
      candidates.push_back(curNode);
      if (curNode->GetDefBy() == kDefByNo) {
        // if any use are from zero version, we stop renaming all the candidates related to it issue #1420
        useDefFromZeroVersion = true;
      } else if (curNode->DefByBB() != nullptr && curNode->DefByBB()->GetAttributes(kBBAttrIsTry)) {
        definedInTryBlock = true;
      }
    }
    if (useDefFromZeroVersion || definedInTryBlock) {
      continue;  // must be zero version. issue #1420
    }
    // get all the nodes in candidates the same register
    PregIdx newPregIdx = regMeExpr->GetRegIdx();
    ASSERT(static_cast<size_t>(newPregIdx) < firstAppearTable.size(), "oversize ");
    if (!firstAppearTable[newPregIdx]) {
      // use the previous register
      firstAppearTable[newPregIdx] = true;
      continue;
    }
    newPregIdx = (regMeExpr->GetPrimType() == PTY_ref) ?
                 pregTab->CreateRefPreg(*pregTab->PregFromPregIdx(regMeExpr->GetRegIdx())->GetMIRType()) :
                 pregTab->CreatePreg(regMeExpr->GetPrimType());
    ++renameCount;
    if (enabledDebug) {
      LogInfo::MapleLogger() << "%" <<
          pregTab->PregFromPregIdx(static_cast<PregIdx>(regMeExpr->GetRegIdx()))->GetPregNo();
      LogInfo::MapleLogger() << " renamed to %" << pregTab->PregFromPregIdx(newPregIdx)->GetPregNo() << '\n';
    }
    // reneme all the register
    for (auto candiIt : candidates) {
      RegMeExpr *candiRegNode = candiIt;
      candiRegNode->SetRegIdx(newPregIdx);  // rename it to a new register
    }
    if (renameCount == MeOption::pregRenameLimit) {
      break;
    }
  }
}

AnalysisResult *MeDoPregRename::Run(MeFunction *func, MeFuncResultMgr *m, ModuleResultMgr*) {
  auto *irMap = static_cast<MeIRMap*>(m->GetAnalysisResult(MeFuncPhase_IRMAP, func));
  PregRenamer pregRenamer(*NewMemPool(), *func, *irMap, DEBUGFUNC(func));
  pregRenamer.RunSelf();
  if (DEBUGFUNC(func)) {
    LogInfo::MapleLogger() << "------------after pregrename:-------------------\n";
    func->Dump(false);
  }
  return nullptr;
}
}  // namespace maple
