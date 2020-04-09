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
#include "me_profile_use.h"
#include <iostream>
#include "me_cfg.h"
#include "me_option.h"
#include "me_function.h"

namespace maple {
BBUseInfo *MeProfUse::GetOrCreateBBUseInfo(const BB &bb) {
  auto item = bbProfileInfo.find(&bb);
  if (item != bbProfileInfo.end()) {
    return item->second;
  } else {
    BBUseInfo *useInfo = mp->New<BBUseInfo>();
    bbProfileInfo.insert(std::make_pair(&bb, useInfo));
    return useInfo;
  }
}

BBUseInfo *MeProfUse::GetBBUseInfo(const BB &bb) const {
  auto item = bbProfileInfo.find(&bb);
  ASSERT(item->second != nullptr, "bb info not created");
  return item->second;
}

uint64 MeProfUse::SumEdgesCount(const std::vector<BBUseEdge*> &edges) const {
  uint64 count = 0;
  for (const auto &e : edges) {
    count += e->GetCount();
  }
  return count;
}

/* create BB use info */
void MeProfUse::InitBBEdgeInfo() {
  for (auto &e : GetAllEdges()) {
    BB *src = e->GetSrcBB();
    BB *dest = e->GetDestBB();
    auto srcUseInfo = GetOrCreateBBUseInfo(*src);
    srcUseInfo->AddOutEdge(e);
    auto destUseInfo = GetOrCreateBBUseInfo(*dest);
    destUseInfo->AddInEdge(e);
  }
  for (auto &e : GetAllEdges()) {
    if (e->IsInMST()) {
      continue;
    }
    BB *src = e->GetSrcBB();
    auto srcUseInfo = GetBBUseInfo(*src);
    if (srcUseInfo->GetStatus() && srcUseInfo->GetOutEdgeSize() == 1) {
      SetEdgeCount(*e, srcUseInfo->GetCount());
    } else {
      BB *dest = e->GetDestBB();
      auto destUseInfo = GetBBUseInfo(*dest);
      if (destUseInfo->GetStatus() && destUseInfo->GetInEdgeSize() == 1) {
        SetEdgeCount(*e, destUseInfo->GetCount());
      }
    }
    if (e->GetStatus()) {
      continue;
    }
    SetEdgeCount(*e, 0);
  }
}

// If all input edges or output edges determined, caculate BB freq
void MeProfUse::ComputeBBFreq(BBUseInfo &bbInfo, bool &change) {
  uint64 count = 0;
  if (!bbInfo.GetStatus()) {
    if (bbInfo.GetUnknownOutEdges() == 0) {
      count = SumEdgesCount(bbInfo.GetOutEdges());
      bbInfo.SetCount(count);
      change = true;
    } else if (bbInfo.GetUnknownInEdges() == 0) {
      count = SumEdgesCount(bbInfo.GetInEdges());
      bbInfo.SetCount(count);
      change = true;
    }
  }
}

/* compute all edge freq in the cfg without consider exception */
void MeProfUse::ComputeEdgeFreq() {
  bool change = true;
  size_t pass = 0;
  auto eIt = func->valid_end();
  while (change) {
    change = false;
    pass++;
    CHECK_FATAL(pass != UINT8_MAX, "parse all edges fail");
    /*
     * use the bb edge to infer the bb's count,when all bb's count is valid
     * then all edges count is valid
     */
    for (auto bIt = func->valid_begin(); bIt != eIt; ++bIt) {
      auto *bb = *bIt;
      BBUseInfo *useInfo = GetBBUseInfo(*bb);
      if (useInfo == nullptr) {
        continue;
      }
      ComputeBBFreq(*useInfo, change);
      if (useInfo->GetStatus()) {
        if (useInfo->GetUnknownOutEdges() == 1) {
          uint64 total = 0;
          uint64 outCount = SumEdgesCount(useInfo->GetOutEdges());
          if (useInfo->GetCount() > outCount) {
            total = useInfo->GetCount() - outCount;
          }
          SetEdgeCount(useInfo->GetOutEdges(), total);
          change = true;
        }
        if (useInfo->GetUnknownInEdges() == 1) {
          uint64 total = 0;
          uint64 inCount = SumEdgesCount(useInfo->GetInEdges());
          if (useInfo->GetCount() > inCount) {
            total = useInfo->GetCount() - inCount;
          }
          SetEdgeCount(useInfo->GetInEdges(), total);
          change = true;
        }
      }
    }
  }
  if (dump) {
    LogInfo::MapleLogger() << "parse all edges in " << pass << " pass" << '\n';
  }
}

/*
 * this used to set the edge count for the unknown edge
 * ensure only one unkown edge in the edges
 */
void MeProfUse::SetEdgeCount(std::vector<BBUseEdge*> &edges, uint64 value) {
  for (const auto &e : edges) {
    if (!e->GetStatus()) {
      e->SetCount(value);
      BBUseInfo *srcInfo = GetBBUseInfo(*(e->GetSrcBB()));
      BBUseInfo *destInfo = GetBBUseInfo(*(e->GetDestBB()));
      srcInfo->DecreaseUnKnownOutEdges();
      destInfo->DecreaseUnKnownInEdges();
      return;
    }
  }
  CHECK(false, "can't find unkown edge");
}

void MeProfUse::SetEdgeCount(BBUseEdge &e, uint32 value) {
  // edge counter already valid skip
  if (e.GetStatus()) {
    return;
  }
  e.SetCount(value);
  BBUseInfo *srcInfo = GetBBUseInfo(*(e.GetSrcBB()));
  BBUseInfo *destInfo = GetBBUseInfo(*(e.GetDestBB()));
  srcInfo->DecreaseUnKnownOutEdges();
  destInfo->DecreaseUnKnownInEdges();
  return;
}

bool MeProfUse::BuildEdgeCount() {
  Profile::BBInfo result;
  bool ret = true;
  ret = func->GetMIRModule().GetProfile().GetFunctionBBProf(func->GetName(), result);
  if (!ret) {
    if (dump) {
      LogInfo::MapleLogger() << func->GetName() << " isn't in profile" << '\n';
    }
    return false;
  }
  FindInstrumentEdges();
  uint64 hash = ComputeFuncHash();
  if (hash != result.funcHash) {
    if (dump) {
      LogInfo::MapleLogger() << func->GetName() << " hash doesn't match profile hash "
                             << result.funcHash << " func real hash " << hash << '\n';
    }
    return false;
  }
  std::vector<BB*> instrumentBBs;
  GetInstrumentBBs(instrumentBBs);
  if (dump) {
    DumpEdgeInfo();
  }
  if (instrumentBBs.size() != result.totalCounter) {
    if (dump) {
      LogInfo::MapleLogger() << func->GetName() << " counter doesn't match profile counter "
                             << result.totalCounter << " func real counter " <<  instrumentBBs.size() << '\n';
    }
    return false;
  }
  size_t i = 0;
  for (auto *bb : instrumentBBs) {
    auto *bbUseInfo = GetOrCreateBBUseInfo(*bb);
    bbUseInfo->SetCount(result.counter[i]);
    i++;
  }

  InitBBEdgeInfo();
  ComputeEdgeFreq();
  succCalcuAllEdgeFreq = true;
  if (Options::testCase) {
    LogInfo::MapleLogger() << func->GetName() << " succ compute all edges " << '\n';
  }
  return true;
}

void MeProfUse::SetFuncEdgeInfo() {
  auto eIt = func->valid_end();
  for (auto bIt = func->valid_begin(); bIt != eIt; ++bIt) {
    if (bIt == func->common_entry() || bIt == func->common_exit()) {
      continue;
    }
    auto *bb = *bIt;
    auto *bbInfo = GetBBUseInfo(*bb);
    bb->SetFrequency(bbInfo->GetCount());
    auto outEdges = bbInfo->GetOutEdges();
    for (auto *e : outEdges) {
      auto *destBB = e->GetDestBB();
      // common_exit's pred's BB doesn't have succ BB of common exit
      // so skip this edge
      if (destBB == func->GetCommonExitBB()) {
        continue;
      }
      bb->SetEdgeFreq(destBB, e->GetCount());
    }
  }
}

void MeProfUse::DumpFuncCFGEdgeFreq() const {
  LogInfo::MapleLogger() << "populate status " << succCalcuAllEdgeFreq << "\n";
  if (!succCalcuAllEdgeFreq) {
    return;
  }
  auto eIt = func->valid_end();
  for (auto bIt = func->valid_begin(); bIt != eIt; ++bIt) {
    if (bIt == func->common_entry() || bIt == func->common_exit()) {
      continue;
    }
    auto bb = *bIt;
    LogInfo::MapleLogger() << bb->GetBBId() << " freq " << bb->GetFrequency() << '\n';
    for (const auto *succBB : bb->GetSucc()) {
      LogInfo::MapleLogger() << "edge " << bb->GetBBId() << "->" << succBB->GetBBId() << " freq "
                             << bb->GetEdgeFreq(succBB) << '\n';
    }
  }
}

bool MeProfUse::Run() {
  if (!BuildEdgeCount()) {
    return false;
  }
  SetFuncEdgeInfo();
  return true;
}

AnalysisResult *MeDoProfUse::Run(MeFunction *func, MeFuncResultMgr *m, ModuleResultMgr*) {
  MemPool *tempMp = NewMemPool();
  MeProfUse profUse(*func, *tempMp, DEBUGFUNC(func));
  profUse.Run();
  if (DEBUGFUNC(func) && profUse.IsSuccUseProf()) {
    LogInfo::MapleLogger() << "******************after profile use  dump function******************\n";
    profUse.DumpFuncCFGEdgeFreq();
    func->GetTheCfg()->DumpToFile("afterProfileUse", false, true);
  }
  return nullptr;
}
}  // namespace maple
