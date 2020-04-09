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
#include "proepilog.h"
#if TARGAARCH64
#include "aarch64_proepilog.h"
#endif
#if TARGARM32
#include "arm32_proepilog.h"
#endif
#include "cgfunc.h"
#include "cg.h"

namespace maplebe {
using namespace maple;

Insn *GenProEpilog::InsertCFIDefCfaOffset(int32 &cfiOffset, Insn &insertAfter) {
  CG *currCG = cgFunc.GetCG();
  ASSERT(currCG != nullptr, "get cg failed in InsertCFIDefCfaOffset");
  cfiOffset = AddtoOffsetFromCFA(cfiOffset);
  Insn &cfiInsn = currCG->BuildInstruction<cfi::CfiInsn>(cfi::OP_CFI_def_cfa_offset,
                                                         cgFunc.CreateCfiImmOperand(cfiOffset, k64BitSize));
  Insn *newIPoint = cgFunc.GetCurBB()->InsertInsnAfter(insertAfter, cfiInsn);
  cgFunc.SetDbgCallFrameOffset(cfiOffset);
  return newIPoint;
}

AnalysisResult *CgDoGenProEpiLog::Run(CGFunc *cgFunc, CgFuncResultMgr *cgFuncResultMgr) {
  (void)cgFuncResultMgr;
  ASSERT(cgFunc != nullptr, "expect a cgfunc in CgDoGenProEpiLog");
  MemPool *memPool = NewMemPool();
  GenProEpilog *genPE = nullptr;
#if TARGAARCH64
  genPE = memPool->New<AArch64GenProEpilog>(*cgFunc);
#endif
#if TARGARM32
  genPE = memPool->New<Arm32GenProEpilog>(*cgFunc);
#endif
  genPE->Run();
  return nullptr;
}
}  /* namespace maplebe */
