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
#include "me_stmt_pre.h"

namespace maple {
void MeStmtPre::ResetFullyAvail(MePhiOcc &occ) {
  occ.SetIsCanBeAvail(false);
  // reset those phiocc nodes that have oc as one of its operands
  for (auto it = phiOccs.begin(); it != phiOccs.end(); ++it) {
    MePhiOcc *phiOcc = *it;
    for (MePhiOpndOcc *phiOpnd : phiOcc->GetPhiOpnds()) {
      if (phiOpnd->GetDef() != nullptr && phiOpnd->GetDef() == &occ) {
        // phiOpnd is a use of occ
        if (!phiOpnd->HasRealUse()) {
          ResetCanBeAvail(*phiOcc);
        }
      }
    }
  }
}

// the fullyavail attribute is stored in the isCanBeAvail field
void MeStmtPre::ComputeFullyAvail() {
  for (auto it = phiOccs.begin(); it != phiOccs.end(); ++it) {
    MePhiOcc *phiOcc = *it;
    // reset canbeavail if any phi operand is null
    bool existNullDef = false;
    for (MePhiOpndOcc *phiOpnd : phiOcc->GetPhiOpnds()) {
      if (phiOpnd->GetDef() == nullptr) {
        existNullDef = true;
        break;
      }
    }
    if (existNullDef) {
      ResetFullyAvail(*phiOcc);
    }
  }
}

bool MeStmtPre::AllVarsSameVersionStmtFre(MeRealOcc &topOcc, MeRealOcc &curOcc) const {
  ASSERT(topOcc.GetOpcodeOfMeStmt() == OP_dassign || topOcc.GetOpcodeOfMeStmt() == OP_callassigned,
      "AllVarsSameVersionStmtFre: only dassign or callassigned is handled");
  if (topOcc.GetMeStmt()->NumMeStmtOpnds() != curOcc.GetMeStmt()->NumMeStmtOpnds()) {
    return false;
  }
  for (size_t i = 0; i < topOcc.GetMeStmt()->NumMeStmtOpnds(); ++i) {
    if (topOcc.GetMeStmt()->GetOpnd(i) != curOcc.GetMeStmt()->GetOpnd(i)) {
      return false;
    }
  }
  return topOcc.GetMeStmt()->GetVarLHS() == curOcc.GetMeExpr();
}

void MeStmtPre::Rename1StmtFre() {
  std::stack<MeOccur*> occStack;
  rename2Set.clear();
  classCount = 1;
  // iterate the occurrence according to its preorder dominator tree
  for (MeOccur *occ : allOccs) {
    while (!occStack.empty() && !occStack.top()->IsDominate(*dom, *occ)) {
      occStack.pop();
    }
    switch (occ->GetOccType()) {
      case kOccReal: {
        if (occStack.empty()) {
          // assign new class
          occ->SetClassID(classCount++);
          occStack.push(occ);
          break;
        }
        MeOccur *topOccur = occStack.top();
        if (topOccur->GetOccType() == kOccMembar) {
          occ->SetClassID(classCount++);
          occStack.push(occ);
          break;
        }
        auto *realOcc = static_cast<MeRealOcc*>(occ);
        if (topOccur->GetOccType() == kOccReal) {
          auto *realTopOccur = static_cast<MeRealOcc*>(topOccur);
          if (AllVarsSameVersionStmtFre(*realTopOccur, *realOcc)) {
            // all corresponding variables are the same
            realOcc->SetClassID(realTopOccur->GetClassID());
            realOcc->SetDef(realTopOccur);
          } else {
            // assign new class
            occ->SetClassID(classCount++);
          }
          occStack.push(occ);
        } else {
          // top of stack is a PHI occurrence
          std::vector<MeExpr*> varVec;
          CollectVarForCand(*realOcc, varVec);
          bool isAllDom = true;
          for (auto varIt = varVec.begin(); varIt != varVec.end(); ++varIt) {
            MeExpr *varMeExpr = *varIt;
            if (!DefVarDominateOcc(varMeExpr, *topOccur)) {
              isAllDom = false;
            }
          }
          if (isAllDom) {
            realOcc->SetClassID(topOccur->GetClassID());
            realOcc->SetDef(topOccur);
            rename2Set.insert(realOcc->GetPosition());
            occStack.push(realOcc);
          } else {
            // assign new class
            occ->SetClassID(classCount++);
          }
          occStack.push(occ);
        }
        break;
      }
      case kOccPhiocc: {
        // assign new class
        occ->SetClassID(classCount++);
        occStack.push(occ);
        break;
      }
      case kOccPhiopnd: {
        if (occStack.empty() || occStack.top()->GetOccType() == kOccMembar) {
          occ->SetDef(nullptr);
        } else {
          MeOccur *topOccur = occStack.top();
          occ->SetDef(topOccur);
          occ->SetClassID(topOccur->GetClassID());
          if (topOccur->GetOccType() == kOccReal) {
            static_cast<MePhiOpndOcc*>(occ)->SetHasRealUse(true);
          }
        }
        break;
      }
      case kOccExit:
        break;
      case kOccMembar:
        if (occStack.empty() || occStack.top()->GetOccType() != kOccMembar) {
          occStack.push(occ);
        }
        break;
      default:
        ASSERT(false, "should not be here");
        break;
    }
  }
  if (GetSSAPreDebug()) {
    PreWorkCand *curCand = workCand;
    mirModule->GetOut() << "======== ssafre candidate " << curCand->GetIndex() <<
        " after rename1StmtFre ===================\n";
    for (MeOccur *occ : allOccs) {
      occ->Dump(*irMap);
      mirModule->GetOut() << "\n";
    }
    mirModule->GetOut() << "\n" << "rename2 set:\n";
    for (uint32 pos : rename2Set) {
      MeRealOcc *occur = workCand->GetRealOcc(pos);
      occur->Dump(*irMap);
      mirModule->GetOut() << " with def at\n";
      occur->GetDef()->Dump(*irMap);
      mirModule->GetOut() << "\n";
    }
    mirModule->GetOut() << "\n";
  }
}

void MeStmtPre::DoSSAFRE() {
  if (GetSSAPreDebug()) {
    mirModule->GetOut() << "{{{{{{{{ start of SSAFRE }}}}}}}}" << '\n';
  }
  // form new allOccs that has no use_occ and reflect insertions and deletions
  MapleVector<MeOccur*> newAllOccs(perCandAllocator.Adapter());
  int32 realOccCnt = 0;
  bool hasInsertion = false;  // if there is insertion, do not perform SSAFRE
  // because SSA form has not been updated
  for (MeOccur *occ : allOccs) {
    switch (occ->GetOccType()) {
      case kOccReal: {
        auto *realOcc = static_cast<MeRealOcc*>(occ);
        if (!realOcc->IsReload()) {
          realOcc->SetIsReload(false);
          realOcc->SetIsSave(false);
          realOcc->SetClassID(0);
          realOcc->SetDef(nullptr);
          newAllOccs.push_back(realOcc);
          ++realOccCnt;
        }
        break;
      }
      case kOccPhiopnd: {
        auto *phiOpnd = static_cast<MePhiOpndOcc*>(occ);
        if (phiOpnd->GetDefPhiOcc()->IsWillBeAvail()) {
          MeOccur *defOcc = phiOpnd->GetDef();
          if (defOcc != nullptr && defOcc->GetOccType() == kOccInserted && !phiOpnd->IsPhiOpndReload()) {
            hasInsertion = true;
            break;
          }
        }
        phiOpnd->SetIsProcessed(false);
        phiOpnd->SetHasRealUse(false);
        phiOpnd->SetIsInsertedOcc(false);
        phiOpnd->SetIsPhiOpndReload(false);
        phiOpnd->SetClassID(0);
        phiOpnd->SetDef(nullptr);
        newAllOccs.push_back(phiOpnd);
        break;
      }
      case kOccPhiocc: {
        auto *phiOcc = static_cast<MePhiOcc*>(occ);
        phiOcc->SetIsDownSafe(false);
        phiOcc->SetIsCanBeAvail(true);
        phiOcc->SetIsLater(false);
        phiOcc->SetIsExtraneous(false);
        phiOcc->SetIsRemoved(false);
        phiOcc->SetClassID(0);
        newAllOccs.push_back(phiOcc);
        break;
      }
      case kOccExit:
      case kOccMembar:
        newAllOccs.push_back(occ);
        break;
      case kOccUse:
        break;
      default:
        ASSERT(false, "should not be here");
        break;
    }
    if (hasInsertion) {
      break;
    }
  }
  if (hasInsertion || realOccCnt <= 1) {
    return;
  }
  allOccs = newAllOccs;
  Rename1StmtFre();
  Rename2();
  ComputeFullyAvail();
  Finalize1();
  Finalize2();
  CodeMotion();
}
} // namespace maple
