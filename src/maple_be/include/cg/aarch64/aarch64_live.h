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
#ifndef MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_LIVE_H
#define MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_LIVE_H

#include "live.h"

namespace maplebe {
class AArch64LiveAnalysis : public LiveAnalysis {
 public:
  AArch64LiveAnalysis(CGFunc &func, MemPool &memPool) : LiveAnalysis(func, memPool) {}
  ~AArch64LiveAnalysis() override = default;
  void GetBBDefUse(BB &bb) override;
  bool CleanupBBIgnoreReg(uint32 reg) override;
  void InitEhDefine(BB &bb) override;
 private:
  void CollectLiveInfo(BB &bb, const Operand &opnd, bool isDef, bool isUse) const;
  void GenerateReturnBBDefUse(BB &bb) const;
  void ProcessCallInsnParam(BB &bb) const;
  void ProcessListOpnd(BB &bb, Operand &opnd) const;
  void ProcessMemOpnd(BB &bb, Operand &opnd) const;
  void ProcessCondOpnd(BB &bb) const;
};
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_LIVE_H */