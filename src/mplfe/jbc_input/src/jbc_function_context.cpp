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
#include "jbc_function_context.h"

namespace maple {
int32 JBCFunctionContext::RegisterJsrSlotRetAddr(uint16 slotIdx, uint32 nextPC) {
  auto itInfo = mapJsrSlotRetAddr.find(slotIdx);
  int32 jsrID;
  if (itInfo == mapJsrSlotRetAddr.end()) {
    jsrID = 0;
  } else {
    size_t size = itInfo->second.size();
    CHECK_FATAL(size < INT32_MAX, "jsr ID out of range");
    jsrID = static_cast<int32>(itInfo->second.size());
  }
  mapJsrSlotRetAddr[slotIdx][jsrID] = nextPC;
  return jsrID;
}

void JBCFunctionContext::ArrangeStmts() {
  /* Type of stmt: inst, label, try, endtry, catch, comment, loc
   *   endtry
   *   loc
   *   catch
   *   label
   *   try
   *   comment
   *   inst
   */
  for (const std::pair<uint32, GeneralStmt*> &pcInst : mapPCStmtInst) {
    uint32 pc = pcInst.first;
    GeneralStmt *stmtInst = pcInst.second;
    if (mapPCEndTryStmt.find(pc) != mapPCEndTryStmt.end()) {
      stmtInst->InsertBefore(mapPCEndTryStmt[pc]);
    }
    if (mapPCStmtLOC.find(pc) != mapPCStmtLOC.end()) {
      stmtInst->InsertBefore(mapPCStmtLOC[pc]);
    }
    if (mapPCCatchStmt.find(pc) != mapPCCatchStmt.end()) {
      stmtInst->InsertBefore(mapPCCatchStmt[pc]);
    }
    if (mapPCLabelStmt.find(pc) != mapPCLabelStmt.end()) {
      stmtInst->InsertBefore(mapPCLabelStmt[pc]);
    }
    if (mapPCTryStmt.find(pc) != mapPCTryStmt.end()) {
      stmtInst->InsertBefore(mapPCTryStmt[pc]);
    }
    if (mapPCCommentStmt.find(pc) != mapPCCommentStmt.end()) {
      stmtInst->InsertBefore(mapPCCommentStmt[pc]);
    }
  }
}

const FEIRType *JBCFunctionContext::GetSlotType(uint16 slotIdx, uint32 pc) const {
  CHECK_NULL_FATAL(code);
  const jbc::JBCAttrLocalVariableInfo &localVarInfo = code->GetLocalVarInfo();
  CHECK_FATAL(pc < UINT16_MAX, "pc out of range");
  uint16 startPC = localVarInfo.GetStart(slotIdx, static_cast<uint16>(pc));
  const jbc::JavaAttrLocalVariableInfoItem &info = localVarInfo.GetItemByStart(slotIdx, startPC);
  return info.feirType;
}

const FEIRType *JBCFunctionContext::GetSlotType(uint16 slotIdx) const {
  CHECK_NULL_FATAL(code);
  const jbc::JBCAttrLocalVariableInfo &localVarInfo = code->GetLocalVarInfo();
  CHECK_FATAL(currPC < UINT16_MAX, "pc out of range");
  uint16 startPC = localVarInfo.GetStart(slotIdx, static_cast<uint16>(currPC));
  const jbc::JavaAttrLocalVariableInfoItem &info = localVarInfo.GetItemByStart(slotIdx, startPC);
  return info.feirType;
}
}  // namespace maple