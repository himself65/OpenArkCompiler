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
#ifndef MAPLEBE_INCLUDE_CG_LIVE_H
#define MAPLEBE_INCLUDE_CG_LIVE_H

#include "cg_phase.h"
#include "insn.h"
#include "cgbb.h"
#include "datainfo.h"

namespace maplebe {
class LiveAnalysis : public AnalysisResult {
 public:
  LiveAnalysis(CGFunc &func, MemPool &memPool)
      : AnalysisResult(&memPool), cgFunc(&func), memPool(&memPool) {}
  ~LiveAnalysis() override = default;

  void AnalysisLive();
  void Dump() const;
  void DumpInfo(const DataInfo &info) const;
  void InitBB(BB &bb);
  void InitAndGetDefUse();
  bool GenerateLiveOut(BB &bb);
  bool GenerateLiveIn(BB &bb);
  void BuildInOutforFunc();
  void DealWithInOutOfCleanupBB();
  void InsertInOutOfCleanupBB();
  void ResetLiveSet();
  void ClearInOutDataInfo();
  void EnlargeSpaceForLiveAnalysis(BB &currBB);

  DataInfo *NewLiveIn(uint32 maxRegCount) {
    return memPool->New<DataInfo>(maxRegCount);
  }

  DataInfo *NewLiveOut(uint32 maxRegCount) {
    return memPool->New<DataInfo>(maxRegCount);
  }

  DataInfo *NewDef(uint32 maxRegCount) {
    return memPool->New<DataInfo>(maxRegCount);
  }

  DataInfo *NewUse(uint32 maxRegCount) {
    return memPool->New<DataInfo>(maxRegCount);
  }

  virtual void GetBBDefUse(BB &bb) = 0;
  virtual bool CleanupBBIgnoreReg(uint32 reg) = 0;
  virtual void InitEhDefine(BB &bb) = 0;

 protected:
  int iteration = 0;
  CGFunc *cgFunc;
  MemPool *memPool;
};

CGFUNCPHASE(CgDoLiveAnalysis, "liveanalysis")
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_LIVE_H */