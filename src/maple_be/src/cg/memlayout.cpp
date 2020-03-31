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
#include "memlayout.h"
#include "cgfunc.h"

namespace maplebe {
using namespace maple;

/*
 * Go over all outgoing calls in the function body and get the maximum space
 * needed for storing the actuals based on the actual parameters and the ABI.
 * These are usually those arguments that cannot be passed
 * through registers because a call passes more than 8 arguments, or
 * they cannot be fit in a pair of registers.

 * This assumes that all nesting of statements has been removed,
 * so that all the statements are at only one block level.
 */
uint32 MemLayout::FindLargestActualArea() {
  StmtNode *stmt = mirFunction->GetBody()->GetFirst();
  if (stmt == nullptr) {
    return 0;
  }
  uint32 maxActualSize = 0;
  for (; stmt != nullptr; stmt = stmt->GetNext()) {
    Opcode opCode = stmt->GetOpCode();
    if (opCode < OP_call || opCode > OP_xintrinsiccallassigned) {
      continue;
    }
    if (opCode == OP_intrinsiccallwithtypeassigned || opCode == OP_intrinsiccallwithtype ||
        opCode == OP_intrinsiccallassigned || opCode == OP_intrinsiccall) {
      /*
       * Some intrinsics, such as MPL_ATOMIC_EXCHANGE_PTR, are handled by CG,
       * and map to machine code sequences.  We ignore them because they are not
       * function calls.
       */
      continue;
    }
    /*
     * if the following check fails, most likely dex has invoke-custom etc
     * that is not supported yet
     */
    DCHECK((opCode == OP_call || opCode == OP_icall), "Not lowered to call or icall?");
    uint32 size = ComputeStackSpaceRequirementForCall(*stmt, opCode == OP_icall);
    if (size > maxActualSize) {
      maxActualSize = size;
    }
  }
  /* kSizeOfPtr * 2's pow 2 is 4, set the low 4 bit of maxActualSize to 0 */
  maxActualSize = RoundUp(maxActualSize, kSizeOfPtr * 2);
  return maxActualSize;
}

AnalysisResult *CgDoLayoutSF::Run(CGFunc *cgFunc, CgFuncResultMgr *cgFuncResultMgr) {
  (void)cgFuncResultMgr;
  ASSERT(cgFunc != nullptr, "expect a cgfunc in CgDoLayoutSF");
  if (CGOptions::IsPrintFunction()) {
    LogInfo::MapleLogger() << cgFunc->GetName() << "\n";
  }

  cgFunc->LayoutStackFrame();

  return nullptr;
}
}  /* namespace maplebe */