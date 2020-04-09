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
#include "offset_adjust.h"
#if TARGAARCH64
#include "aarch64_offset_adjust.h"
#endif
#if TARGARM32
#include "arm32_offset_adjust.h"
#endif

#include "cgfunc.h"

namespace maplebe {
using namespace maple;
AnalysisResult *CgDoFPLROffsetAdjustment::Run(CGFunc *cgFunc, CgFuncResultMgr *cgFuncResultMgr) {
  (void)cgFuncResultMgr;
  MemPool *memPool = NewMemPool();
  FPLROffsetAdjustment *offsetAdjustment = nullptr;
  ASSERT(cgFunc != nullptr, "expect a cgfun in CgDoFPLROffsetAdjustment");
#if TARGAARCH64
  offsetAdjustment = memPool->New<AArch64FPLROffsetAdjustment>(*cgFunc);
#endif
#if TARGARM32
  offsetAdjustment = memPool->New<Arm32FPLROffsetAdjustment>(*cgFunc);
#endif
  offsetAdjustment->Run();
  return nullptr;
}
}  /* namespace maplebe */
