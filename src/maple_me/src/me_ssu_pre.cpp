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
#include "me_ssu_pre.h"

namespace maple {
#define JAVALANG (mirmodule->IsJavaModule())

// ================ Step 5: Finalize ================
void MeSSUPre::Finalize() {
  std::vector<SOcc*> anticipatedDefVec(classCount + 1, nullptr);
  // preorder traversal of post-dominator tree
  for (SOcc *occ : allOccs) {
    size_t classx = static_cast<size_t>(occ->GetClassId());
    switch (occ->GetOccTy()) {
      case kSOccLambda: {
        auto *lambdaOcc = static_cast<SLambdaOcc*>(occ);
        if (lambdaOcc->WillBeAnt()) {
          anticipatedDefVec[classx] = lambdaOcc;
        }
        break;
      }
      case kSOccReal: {
        auto *realocc = static_cast<SRealOcc*>(occ);
        if (anticipatedDefVec[classx] == nullptr || !anticipatedDefVec[classx]->IsPostDominate(dom, occ)) {
          realocc->SetRedundant(false);
          anticipatedDefVec[classx] = realocc;
        } else {
          realocc->SetRedundant(true);
        }
        break;
      }
      case kSOccLambdaRes: {
        auto *lambdaResOcc = static_cast<SLambdaResOcc*>(occ);
        const SLambdaOcc *lambdaOcc = lambdaResOcc->GetUseLambdaOcc();
        if (lambdaOcc->WillBeAnt()) {
          if (lambdaResOcc->GetUse() == nullptr || (!lambdaResOcc->GetHasRealUse() &&
                                                    lambdaResOcc->GetUse()->GetOccTy() == kSOccLambda &&
                                                    !static_cast<SLambdaOcc*>(lambdaResOcc->GetUse())->WillBeAnt())) {
            // insert a store
            BB *insertBB = lambdaResOcc->GetBB();
            if (insertBB->GetAttributes(kBBAttrIsCatch)) {
              if (preKind == kDecrefPre) {
                catchBlocks2Insert.insert(insertBB->GetBBId());
              } // else { kStorePre: omit insertion at entry of catch blocks }
              break;
            }
            if (lambdaResOcc->GetBB()->GetPred().size() != 1) {  // critical edge
              if (preKind != kDecrefPre && preKind != kSecondDecrefPre) {
                CHECK_FATAL(false, "MeSSUPre::Finalize: insertion at critical edge");
                workCand->SetHasCriticalEdge(true);
                return;
              }
            }
            if (insertBB->GetAttributes(kBBAttrArtificial) && insertBB->GetAttributes(kBBAttrIsExit)) {
              // do not insert at fake BBs created due to infinite loops
              break;
            }
            lambdaResOcc->SetInsertHere(true);
          } else {
            lambdaResOcc->SetUse(anticipatedDefVec[classx]);
          }
        }
        break;
      }
      case kSOccEntry:
      case kSOccUse:
      case kSOccPhi:
        break;
      default:
        ASSERT(false, "Finalize: unexpected occ type");
    }
  }
  if (enabledDebug) {
    LogInfo::MapleLogger() << " _______ after finalize _______" << '\n';
    for (SOcc *occ : allOccs) {
      if (occ->GetOccTy() == kSOccReal) {
        auto *realocc = static_cast<SRealOcc*>(occ);
        if (realocc->GetRedundant()) {
          occ->Dump();
          LogInfo::MapleLogger() << " deleted" << '\n';
        }
      } else if (occ->GetOccTy() == kSOccLambdaRes) {
        auto *lambdaResOcc = static_cast<SLambdaResOcc*>(occ);
        if (lambdaResOcc->GetInsertHere()) {
          occ->Dump();
          LogInfo::MapleLogger() << " insertHere" << '\n';
        }
      }
    }
  }
}

// ================ Step 4: WillBeAnt Computation ================
void MeSSUPre::ResetCanBeFullyAnt(SLambdaOcc *lambda) {
  CHECK_NULL_FATAL(lambda);
  lambda->SetIsCanBeAnt(false);
  for (SLambdaOcc *lambdaOcc : lambdaOccs) {
    for (SLambdaResOcc *lambdaResOcc : lambdaOcc->GetLambdaRes()) {
      if (lambdaResOcc->GetUse() && lambdaResOcc->GetUse() == lambda &&
          !lambdaResOcc->GetHasRealUse() && lambdaOcc->GetIsCanBeAnt()) {
          ResetCanBeFullyAnt(lambdaOcc);
      }
    }
  }
}

void MeSSUPre::ComputeCanBeFullyAnt() {
  for (SLambdaOcc *lambdaOcc : lambdaOccs) {
    bool existNullUse = false;
    for (SLambdaResOcc *lambdaResOcc : lambdaOcc->GetLambdaRes()) {
      if (lambdaResOcc->GetUse() == nullptr) {
        existNullUse = true;
        break;
      }
    }
    if (existNullUse) {
      ResetCanBeFullyAnt(lambdaOcc);
    }
  }
}

void MeSSUPre::ResetCanBeAnt(SLambdaOcc *lambda) {
  CHECK_NULL_FATAL(lambda);
  lambda->SetIsCanBeAnt(false);
  // the following loop finds lambda's defs and reset them
  for (SLambdaOcc *lambdaOcc : lambdaOccs) {
    for (SLambdaResOcc *lambdaResOcc : lambdaOcc->GetLambdaRes()) {
      if (lambdaResOcc->GetUse() && lambdaResOcc->GetUse() == lambda) {
        if (!lambdaResOcc->GetHasRealUse() && !lambdaOcc->GetIsUpsafe() && lambdaOcc->GetIsCanBeAnt()) {
          ResetCanBeAnt(lambdaOcc);
        }
      }
    }
  }
}

void MeSSUPre::ComputeCanBeAnt() {
  for (SLambdaOcc *lambdaOcc : lambdaOccs) {
    if (!lambdaOcc->GetIsUpsafe() && lambdaOcc->GetIsCanBeAnt()) {
      bool existNullUse = false;
      for (SLambdaResOcc *lambdaResOcc : lambdaOcc->GetLambdaRes()) {
        if (lambdaResOcc->GetUse() == nullptr) {
          existNullUse = true;
          break;
        }
      }
      if (existNullUse) {
        ResetCanBeAnt(lambdaOcc);
      }
    }
  }
}

void MeSSUPre::ResetEarlier(SLambdaOcc *lambda) {
  CHECK_NULL_FATAL(lambda);
  lambda->SetIsEarlier(false);
  // the following loop finds lambda's defs and reset them
  for (SLambdaOcc *lambdaOcc : lambdaOccs) {
    for (SLambdaResOcc *lambdaResOcc : lambdaOcc->GetLambdaRes()) {
      if (lambdaResOcc->GetUse() && lambdaResOcc->GetUse() == lambda) {
        if (lambdaOcc->GetIsEarlier()) {
          ResetEarlier(lambdaOcc);
        }
      }
    }
  }
}

void MeSSUPre::ComputeEarlier() {
  for (SLambdaOcc *lambdaOcc : lambdaOccs) {
    lambdaOcc->SetIsEarlier(lambdaOcc->GetIsCanBeAnt());
  }
  for (SLambdaOcc *lambdaOcc : lambdaOccs) {
    if (lambdaOcc->GetIsEarlier()) {
      bool existNonNullUse = false;
      for (SLambdaResOcc *lambdaResOcc : lambdaOcc->GetLambdaRes()) {
        if (lambdaResOcc->GetUse() && lambdaResOcc->GetHasRealUse()) {
          existNonNullUse = true;
          break;
        }
      }
      if (existNonNullUse) {
        ResetEarlier(lambdaOcc);
      }
    }
  }
  if (enabledDebug) {
    LogInfo::MapleLogger() << " _______ after earlier computation _______" << '\n';
    for (SLambdaOcc *lambdaOcc : lambdaOccs) {
      lambdaOcc->Dump();
      if (lambdaOcc->GetIsCanBeAnt()) {
        LogInfo::MapleLogger() << " canbeant";
      }
      if (lambdaOcc->GetIsEarlier()) {
        LogInfo::MapleLogger() << " earlier";
      }
      if (lambdaOcc->GetIsCanBeAnt() && !lambdaOcc->GetIsEarlier()) {
        LogInfo::MapleLogger() << " will be ant";
      }
      LogInfo::MapleLogger() << '\n';
    }
  }
}

// ================ Step 3: Upsafe Computation ================
void MeSSUPre::ResetUpsafe(SLambdaResOcc *lambdaRes) {
  CHECK_NULL_FATAL(lambdaRes);
  if (lambdaRes->GetHasRealUse()) {
    return;
  }
  SOcc *useOcc = lambdaRes->GetUse();
  if (useOcc == nullptr || useOcc->GetOccTy() != kSOccLambda) {
    return;
  }
  auto *useLambdaOcc = static_cast<SLambdaOcc*>(useOcc);
  if (!useLambdaOcc->GetIsUpsafe()) {
    return;
  }
  useLambdaOcc->SetIsUpsafe(false);
  for (SLambdaResOcc *lambdaResOcc : useLambdaOcc->GetLambdaRes()) {
    ResetUpsafe(lambdaResOcc);
  }
}

void MeSSUPre::ComputeUpsafe() {
  for (SLambdaOcc *lambdaOcc : lambdaOccs) {
    if (!lambdaOcc->GetIsUpsafe()) {
      // propagate not-upsafe forward along def-use edges
      for (SLambdaResOcc *lambdaResOcc : lambdaOcc->GetLambdaRes()) {
        ResetUpsafe(lambdaResOcc);
      }
    }
  }
  if (enabledDebug) {
    LogInfo::MapleLogger() << " _______ after upsafe computation _______" << '\n';
    for (SLambdaOcc *lambdaOcc : lambdaOccs) {
      lambdaOcc->Dump();
      if (lambdaOcc->GetIsUpsafe()) {
        LogInfo::MapleLogger() << " upsafe";
      }
      LogInfo::MapleLogger() << '\n';
    }
  }
}

// ================ Step 2: rename ================
void MeSSUPre::Rename() {
  std::stack<SOcc*> occStack;
  classCount = 0;
  // iterate thru the occurrences in order of preorder traversal of
  // post-dominator tree
  for (SOcc *occ : allOccs) {
    while (!occStack.empty() && !occStack.top()->IsPostDominate(dom, occ)) {
      occStack.pop();
    }
    switch (occ->GetOccTy()) {
      case kSOccUse:
        if (!occStack.empty()) {
          SOcc *topOcc = occStack.top();
          if (topOcc->GetOccTy() == kSOccLambda) {
            static_cast<SLambdaOcc*>(topOcc)->SetIsUpsafe(false);
          }
        }
        occStack.push(occ);
        break;
      case kSOccEntry:
        if (!occStack.empty()) {
          SOcc *topOcc = occStack.top();
          if (topOcc->GetOccTy() == kSOccLambda) {
            // make sure this lambda is dominated by at least one kill occurrence
            for (SOcc *realocc : workCand->GetRealOccs()) {
              if (realocc->GetOccTy() == kSOccUse || realocc->GetOccTy() == kSOccPhi) {
                continue;
              }
              CHECK_FATAL(realocc->GetOccTy() == kSOccReal, "just check");
              if ((preKind == kDecrefPre || preKind == kSecondDecrefPre) &&
                  !static_cast<SRealOcc*>(realocc)->GetRealFromDef()) {
                continue;
              }
              CHECK_NULL_FATAL(dom);
              if (!dom->Dominate(*realocc->GetBB(), *topOcc->GetBB())) {
                continue;
              }
              static_cast<SLambdaOcc*>(topOcc)->SetIsUpsafe(false);
              break;
            }
          }
        }
        break;
      case kSOccLambda:
        // assign new class
        occ->SetClassId(++classCount);
        occStack.push(occ);
        break;
      case kSOccReal: {
        if (occStack.empty()) {
          // assign new class
          occ->SetClassId(++classCount);
          occStack.push(occ);
          break;
        }
        SOcc *topOcc = occStack.top();
        if (topOcc->GetOccTy() == kSOccUse) {
          // assign new class
          occ->SetClassId(++classCount);
          occStack.push(occ);
          break;
        }
        ASSERT(topOcc->GetOccTy() == kSOccLambda || topOcc->GetOccTy() == kSOccReal,
               "Rename: unexpected top-of-stack occ");
        occ->SetClassId(topOcc->GetClassId());
        if (topOcc->GetOccTy() == kSOccLambda) {
          occStack.push(occ);
        }
        break;
      }
      case kSOccLambdaRes: {
        if (occStack.empty()) {
          // leave classId as 0
          break;
        }
        SOcc *topOcc = occStack.top();
        if (topOcc->GetOccTy() == kSOccUse) {
          // leave classId as 0
          break;
        }
        ASSERT(topOcc->GetOccTy() == kSOccLambda || topOcc->GetOccTy() == kSOccReal,
               "Rename: unexpected top-of-stack occ");
        occ->SetUse(topOcc);
        occ->SetClassId(topOcc->GetClassId());
        if (topOcc->GetOccTy() == kSOccReal) {
          static_cast<SLambdaResOcc*>(occ)->SetHasRealUse(true);
        }
        break;
      }
      case kSOccPhi:
        break;
      default:
        ASSERT(false, "Rename: unexpected type of occurrence");
    }
  }
  if (enabledDebug) {
    LogInfo::MapleLogger() << " _______ after rename _______" << '\n';
    for (SOcc *occ : allOccs) {
      occ->Dump();
      LogInfo::MapleLogger() << '\n';
    }
  }
}

// ================ Step 1: insert lambdas ================
void MeSSUPre::GetIterPdomFrontier(const BB *bb, MapleSet<uint32> *pdfSet, std::vector<bool> &visitedMap) {
  CHECK_NULL_FATAL(bb);
  CHECK_FATAL(bb->GetBBId() < visitedMap.size(), "index out of range in MeSSUPre::GetIterPdomFrontier");
  if (visitedMap[bb->GetBBId()]) {
    return;
  }
  CHECK_FATAL(!visitedMap.empty(), "visitedMap in MeSSUPre::GetIterPdomFrontier is empty");
  visitedMap[bb->GetBBId()] = true;
  CHECK_NULL_FATAL(pdfSet);
  for (BBId frontierBBId : dom->GetPdomFrontierItem(bb->GetBBId())) {
    pdfSet->insert(dom->GetPdtDfnItem(frontierBBId));
  }
  CHECK_NULL_FATAL(func);
  for (BBId frontierBBId : dom->GetPdomFrontierItem(bb->GetBBId())) {
    BB *frontierBB = func->GetAllBBs().at(frontierBBId);
    GetIterPdomFrontier(frontierBB, pdfSet, visitedMap);
  }
}

// form lambda occ based on the real occ in workCand->realOccs; result is
// stored in lambdaDfns
void MeSSUPre::FormLambdas() {
  lambdaDfns.clear();
  std::vector<bool> visitedMap(func->NumBBs(), false);
  CHECK_NULL_FATAL(workCand);
  for (SOcc *occ : workCand->GetRealOccs()) {
    GetIterPdomFrontier(occ->GetBB(), &lambdaDfns, visitedMap);
  }
}

// form allOccs inclusive of real, use, lambda, lambdaRes, entry occurrences;
// form lambdaOccs containing only the lambdas
void MeSSUPre::CreateSortedOccs() {
  // form lambdaRes occs based on the succs of the lambda occs ; result is
  // stored in lambdaResDfns
  std::multiset<uint32> lambdaResDfns;
  for (uint32 dfn : lambdaDfns) {
    const BBId &bbId = dom->GetPdtPreOrderItem(dfn);
    BB *bb = func->GetAllBBs().at(bbId);
    for (BB *succ : bb->GetSucc()) {
      lambdaResDfns.insert(dom->GetPdtDfnItem(succ->GetBBId()));
    }
  }
  allOccs.clear();
  lambdaOccs.clear();
  std::unordered_map<BBId, std::forward_list<SLambdaResOcc*>> bb2lambdaResMap;
  auto realOccIt = workCand->GetRealOccs().begin();
  auto entryOccIt = entryOccs.begin();
  auto lambdaDfnIt = lambdaDfns.begin();
  auto lambdaResDfnIt = lambdaResDfns.begin();
  SOcc *nextRealOcc = nullptr;
  if (realOccIt != workCand->GetRealOccs().end()) {
    nextRealOcc = *realOccIt;
  }
  SEntryOcc *nextEntryOcc = nullptr;
  if (entryOccIt != entryOccs.end()) {
    nextEntryOcc = *entryOccIt;
  }
  SLambdaOcc *nextLambdaOcc = nullptr;
  if (lambdaDfnIt != lambdaDfns.end()) {
    nextLambdaOcc =
        spreMp->New<SLambdaOcc>(func->GetAllBBs().at(dom->GetPdtPreOrderItem(*lambdaDfnIt)), &spreAllocator);
  }
  SLambdaResOcc *nextLambdaResOcc = nullptr;
  if (lambdaResDfnIt != lambdaResDfns.end()) {
    nextLambdaResOcc = spreMp->New<SLambdaResOcc>(func->GetAllBBs().at(dom->GetPdtPreOrderItem(*lambdaResDfnIt)));
    auto it = bb2lambdaResMap.find(dom->GetPdtPreOrderItem(*lambdaResDfnIt));
    if (it == bb2lambdaResMap.end()) {
      std::forward_list<SLambdaResOcc*> newlist = { nextLambdaResOcc };
      bb2lambdaResMap[dom->GetPdtPreOrderItem(*lambdaResDfnIt)] = newlist;
    } else {
      it->second.push_front(nextLambdaResOcc);
    }
  }
  SOcc *pickedOcc = nullptr;  // the next picked occ in order of preorder traversal of post-dominator tree
  do {
    pickedOcc = nullptr;
    if (nextLambdaOcc != nullptr) {
      pickedOcc = nextLambdaOcc;
    }
    if (nextRealOcc && (pickedOcc == nullptr || dom->GetPdtDfnItem(nextRealOcc->GetBB()->GetBBId()) <
                       dom->GetPdtDfnItem(pickedOcc->GetBB()->GetBBId()))) {
      pickedOcc = nextRealOcc;
    }
    if (nextLambdaResOcc &&
        (pickedOcc == nullptr || *lambdaResDfnIt < dom->GetPdtDfnItem(pickedOcc->GetBB()->GetBBId()))) {
      pickedOcc = nextLambdaResOcc;
    }
    if (nextEntryOcc && (pickedOcc == nullptr || dom->GetPdtDfnItem(nextEntryOcc->GetBB()->GetBBId()) <
                        dom->GetPdtDfnItem(pickedOcc->GetBB()->GetBBId()))) {
      pickedOcc = nextEntryOcc;
    }
    if (pickedOcc != nullptr) {
      allOccs.push_back(pickedOcc);
      switch (pickedOcc->GetOccTy()) {
        case kSOccReal:
        case kSOccUse:
        case kSOccPhi:
          // get the next real/use occ
          CHECK_FATAL(realOccIt != workCand->GetRealOccs().end(), "iterator check");
          ++realOccIt;
          if (realOccIt != workCand->GetRealOccs().end()) {
            nextRealOcc = *realOccIt;
          } else {
            nextRealOcc = nullptr;
          }
          break;
        case kSOccEntry:
          CHECK_FATAL(entryOccIt != entryOccs.end(), "iterator check");
          ++entryOccIt;
          if (entryOccIt != entryOccs.end()) {
            nextEntryOcc = *entryOccIt;
          } else {
            nextEntryOcc = nullptr;
          }
          break;
        case kSOccLambda:
          lambdaOccs.push_back(static_cast<SLambdaOcc*>(pickedOcc));
          CHECK_FATAL(lambdaDfnIt != lambdaDfns.end(), "iterator check");
          ++lambdaDfnIt;
          if (lambdaDfnIt != lambdaDfns.end()) {
            nextLambdaOcc =
                spreMp->New<SLambdaOcc>(func->GetAllBBs().at(dom->GetPdtPreOrderItem(*lambdaDfnIt)), &spreAllocator);
          } else {
            nextLambdaOcc = nullptr;
          }
          break;
        case kSOccLambdaRes:
          CHECK_FATAL(lambdaResDfnIt != lambdaResDfns.end(), "iterator check");
          ++lambdaResDfnIt;
          if (lambdaResDfnIt != lambdaResDfns.end()) {
            nextLambdaResOcc =
                spreMp->New<SLambdaResOcc>(func->GetAllBBs().at(dom->GetPdtPreOrderItem(*lambdaResDfnIt)));
            CHECK_NULL_FATAL(dom);
            auto it = bb2lambdaResMap.find(dom->GetPdtPreOrderItem(*lambdaResDfnIt));
            if (it == bb2lambdaResMap.end()) {
              std::forward_list<SLambdaResOcc*> newlist = { nextLambdaResOcc };
              bb2lambdaResMap[dom->GetPdtPreOrderItem(*lambdaResDfnIt)] = newlist;
            } else {
              it->second.push_front(nextLambdaResOcc);
            }
          } else {
            nextLambdaResOcc = nullptr;
          }
          break;
        default:
          ASSERT(false, "CreateSortedOccs: unexpected occTy");
      }
    }
  } while (pickedOcc != nullptr);
  // initialize lambdaRes vector in each SLambdaOcc node
  for (SLambdaOcc *lambdaOcc : lambdaOccs) {
    for (BB *succ : lambdaOcc->GetBB()->GetSucc()) {
      SLambdaResOcc *lambdaResOcc = bb2lambdaResMap[succ->GetBBId()].front();
      lambdaOcc->GetLambdaRes().push_back(lambdaResOcc);
      lambdaResOcc->SetUseLambdaOcc(lambdaOcc);
      bb2lambdaResMap[succ->GetBBId()].pop_front();
    }
  }
  if (enabledDebug) {
    LogInfo::MapleLogger() << " _______ after lambda insertion _______" << '\n';
    for (SOcc *occ : allOccs) {
      occ->Dump();
      LogInfo::MapleLogger() << '\n';
    }
  }
}

void MeSSUPre::ApplySSUPre() {
  BuildWorkListBB(func->GetCommonExitBB());
  if (preKind != kSecondDecrefPre) {  // #0 build worklist
    CreateEmptyCleanupIntrinsics();
  }
  if (enabledDebug) {
    LogInfo::MapleLogger() << "------ worklist initial size " << workCandMap.size() << '\n';
  }
  int32 candNum = 0;
  for (std::pair<OStIdx, SpreWorkCand*> wkCandPair : workCandMap) {
    if (preKind == kStorePre) {
      if (!wkCandPair.second->GetHasStoreOcc()) {
        continue;
      }
    } else if (preKind == kDecrefPre) {
      if (wkCandPair.second->GetOst()->IsFormal() && !wkCandPair.second->GetHasStoreOcc()) {
        continue;
      }
    }
    workCand = wkCandPair.second;
    if (enabledDebug) {
      LogInfo::MapleLogger() << "||||||| SPRE candidate " << candNum << " (ostidx "
                             << workCand->GetOst()->GetIndex() << "): ";
      workCand->GetOst()->Dump();
      LogInfo::MapleLogger() << '\n';
    }
    PerCandInit();
    // #1 insert lambdas; results in allOccs and lambdaOccs
    FormLambdas();  // result put in the set lambda_bbs
    CreateSortedOccs();
    // #2 rename
    Rename();
    if (!lambdaOccs.empty()) {
      // #3 UpSafety
      ComputeUpsafe();
      // #4 CanBeAnt
      ComputeCanBeAnt();
      ComputeEarlier();
    }
    // #5 Finalize
    Finalize();
    // #6 Code Mmotion
    CHECK_NULL_FATAL(workCand);
    if (!workCand->GetHasCriticalEdge()) {
      CodeMotion();
    }
    candNum++;
  }
}
}  // namespace maple
