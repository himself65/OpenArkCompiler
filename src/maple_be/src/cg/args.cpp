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
#include "args.h"
#if TARGAARCH64
#include "aarch64_args.h"
#endif
#if TARGARM32
#include "arm32_args.h"
#endif
#include "cgfunc.h"

namespace maplebe {
using namespace maple;
AnalysisResult *CgDoMoveRegArgs::Run(CGFunc *cgFunc, CgFuncResultMgr *cgFuncResultMgr) {
  (void)cgFuncResultMgr;
  MemPool *memPool = NewMemPool();
  MoveRegArgs *movRegArgs = nullptr;
  ASSERT(cgFunc != nullptr, "expect a cgfunc in CgDoMoveRegArgs");
#if TARGAARCH64
  movRegArgs = memPool->New<AArch64MoveRegArgs>(*cgFunc);
#endif
#if TARGARM32
  movRegArgs = memPool->New<Arm32MoveRegArgs>(*cgFunc);
#endif
  movRegArgs->Run();

  return nullptr;
}
}  /* namespace maplebe */
