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
#include "yieldpoint.h"
#if TARGAARCH64
#include "aarch64_yieldpoint.h"
#endif
#if TARGARM32
#include "arm32_yieldpoint.h"
#endif
#include "cgfunc.h"

namespace maplebe {
using namespace maple;
AnalysisResult *CgYieldPointInsertion::Run(CGFunc *cgFunc, CgFuncResultMgr *cgFuncResultMgr) {
  (void)cgFuncResultMgr;
  ASSERT(cgFunc != nullptr, "expect a cgfunc in CgYieldPointInsertion");
  MemPool *memPool = NewMemPool();
  YieldPointInsertion *yieldPoint = nullptr;
#if TARGAARCH64
  yieldPoint = memPool->New<AArch64YieldPointInsertion>(*cgFunc);
#endif
#if TARGARM32
  yieldPoint = memPool->New<Arm32YieldPointInsertion>(*cgFunc);
#endif
  yieldPoint->Run();
  return nullptr;
}
}  /* namespace maplebe */
