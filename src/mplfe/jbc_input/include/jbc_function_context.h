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
#ifndef MPLFE_INCLUDE_JBC_INPUT_JBC_FUNCTION_CONTEXT_H
#define MPLFE_INCLUDE_JBC_INPUT_JBC_FUNCTION_CONTEXT_H
#include "mir_type.h"
#include "jbc_class_const_pool.h"
#include "jbc_attr.h"
#include "jbc_stack2fe_helper.h"
#include "jbc_stmt.h"

namespace maple {
class JBCFunctionContext {
 public:
  JBCFunctionContext(const jbc::JBCConstPool &argConstPool,
                     JBCStack2FEHelper &argStack2feHelper,
                     const jbc::JBCAttrCode *argCode)
      : constPool(argConstPool),
        stack2feHelper(argStack2feHelper),
        code(argCode) {}

  ~JBCFunctionContext() {
    code = nullptr;
  }

  int32 RegisterJsrSlotRetAddr(uint16 slotIdx, uint32 nextPC);
  void ArrangeStmts();
  const FEIRType *GetSlotType(uint16 slotIdx, uint32 pc) const;
  const FEIRType *GetSlotType(uint16 slotIdx) const;

  JBCStack2FEHelper &GetStack2FEHelper() {
    return stack2feHelper;
  }

  const jbc::JBCConstPool &GetConstPool() const {
    return constPool;
  }

  const jbc::JBCAttrCode *GetCode() const {
    return code;
  }

  const std::map<uint32, GeneralStmt*> &GetMapPCStmtInst() const {
    return mapPCStmtInst;
  }

  const std::map<uint32, JBCStmtPesudoTry*> &GetMapPCTryStmt() const {
    return mapPCTryStmt;
  }

  const std::map<uint32, JBCStmtPesudoEndTry*> &GetMapPCEndTryStmt() const {
    return mapPCEndTryStmt;
  }

  const std::map<uint32, JBCStmtPesudoCatch*> &GetMapPCCatchStmt() const {
    return mapPCCatchStmt;
  }

  const std::map<uint32, JBCStmtPesudoLabel*> &GetMapPCLabelStmt() const {
    return mapPCLabelStmt;
  }

  const std::map<uint32, JBCStmtPesudoLOC*> &GetMapPCStmtLOC() const {
    return mapPCStmtLOC;
  }

  const std::map<uint32, JBCStmtPesudoComment*> &GetMapPCCommentStmt() const {
    return mapPCCommentStmt;
  }

  const std::map<uint16, std::map<int32, uint32>> &GetMapJsrSlotRetAddr() const {
    return mapJsrSlotRetAddr;
  }

  void UpdateMapPCStmtInst(uint32 pc, GeneralStmt *stmt) {
    mapPCStmtInst[pc] = stmt;
  }

  void UpdateMapPCTryStmt(uint32 pc, JBCStmtPesudoTry *stmt) {
    mapPCTryStmt[pc] = stmt;
  }

  void UpdateMapPCEndTryStmt(uint32 pc, JBCStmtPesudoEndTry *stmt) {
    mapPCEndTryStmt[pc] = stmt;
  }

  void UpdateMapPCCatchStmt(uint32 pc, JBCStmtPesudoCatch *stmt) {
    mapPCCatchStmt[pc] = stmt;
  }

  void UpdateMapPCLabelStmt(uint32 pc, JBCStmtPesudoLabel *stmt) {
    mapPCLabelStmt[pc] = stmt;
  }

  void UpdateMapPCStmtLOC(uint32 pc, JBCStmtPesudoLOC *stmt) {
    mapPCStmtLOC[pc] = stmt;
  }

  void UpdateMapPCCommentStmt(uint32 pc, JBCStmtPesudoComment *stmt) {
    mapPCCommentStmt[pc] = stmt;
  }

  const jbc::JBCAttrLocalVariableInfo &GetLocalVarInfo() const {
    CHECK_NULL_FATAL(code);
    return code->GetLocalVarInfo();
  }

  void SetCurrPC(uint32 pc) {
    currPC = pc;
  }

  uint32 GetCurrPC() const {
    return currPC;
  }

 private:
  const jbc::JBCConstPool &constPool;
  JBCStack2FEHelper &stack2feHelper;
  const jbc::JBCAttrCode *code;
  std::map<uint32, GeneralStmt*> mapPCStmtInst;
  std::map<uint32, JBCStmtPesudoTry*> mapPCTryStmt;  // key: tryStartPC, value: stmt
  std::map<uint32, JBCStmtPesudoEndTry*> mapPCEndTryStmt;  // key: tryEndPC, value: stmt
  std::map<uint32, JBCStmtPesudoCatch*> mapPCCatchStmt;  // key: handlePC, value: stmt
  std::map<uint32, JBCStmtPesudoLabel*> mapPCLabelStmt;  // key: labelPC, value: stmt
  std::map<uint32, JBCStmtPesudoLOC*> mapPCStmtLOC; // key: locPC, value: stmt
  std::map<uint32, JBCStmtPesudoComment*> mapPCCommentStmt; // key: commentPC, value: stmt
  std::map<uint16, std::map<int32, uint32>> mapJsrSlotRetAddr;  // key: slotIdx, value: map<jsrID, retAddr>
  uint32 currPC = 0;
};  // class JBCFunctionContext
}  // namespace maple
#endif  // MPLFE_INCLUDE_JBC_INPUT_JBC_FUNCTION_CONTEXT_H