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
#ifndef MPLFE_INCLUDE_COMMON_GENERAL_CFG_H
#define MPLFE_INCLUDE_COMMON_GENERAL_CFG_H
#include <memory>
#include <list>
#include "general_stmt.h"
#include "general_bb.h"

namespace maple {
// using FuncGeneralBBNew = std::unique_ptr<GeneralBB> (*)();
class GeneralCFG {
 public:
  GeneralCFG(const GeneralStmt &argStmtHead, const GeneralStmt &argStmtTail);
  virtual ~GeneralCFG() {
    currBBNode = nullptr;
  }

  void Init();
  void BuildBB();
  bool BuildCFG();
  const GeneralBB *GetHeadBB();
  const GeneralBB *GetNextBB();
  GeneralBB *GetDummyHead() const {
    return bbHead.get();
  }

  GeneralBB *GetDummyTail() const {
    return bbTail.get();
  }

  std::unique_ptr<GeneralBB> NewGeneralBB() const {
    return NewGeneralBBImpl();
  }

 protected:
  virtual std::unique_ptr<GeneralBB> NewGeneralBBImpl() const = 0;
  void BuildBasicBB();
  void AppendAuxStmt();
  GeneralBB *NewBBAppend();

  const GeneralStmt &stmtHead;
  const GeneralStmt &stmtTail;
  FELinkListNode *currBBNode = nullptr;
  std::list<std::unique_ptr<GeneralBB>> listBB;
  std::unique_ptr<GeneralBB> bbHead;
  std::unique_ptr<GeneralBB> bbTail;
};  // class GeneralCFG
}  // namespace maple
#endif  // MPLFE_INCLUDE_COMMON_GENERAL_CFG_H