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
#include "label_creation.h"
#include "cgfunc.h"
#include "cg.h"

namespace maplebe {
using namespace maple;

void LabelCreation::Run() {
  CreateStartEndLabel();
}

void LabelCreation::CreateStartEndLabel() {
  ASSERT(cgFunc != nullptr, "expect a cgfunc before CreateStartEndLabel");
  LabelIdx startLblIdx = cgFunc->CreateLabel();
  MIRBuilder *mirBuilder = cgFunc->GetFunction().GetModule()->GetMIRBuilder();
  ASSERT(mirBuilder != nullptr, "get mirbuilder failed in CreateStartEndLabel");
  LabelNode *startLabel = mirBuilder->CreateStmtLabel(startLblIdx);
  cgFunc->SetStartLabel(*startLabel);
  cgFunc->GetFunction().GetBody()->InsertFirst(startLabel);
  LabelIdx endLblIdx = cgFunc->CreateLabel();
  LabelNode *endLabel = mirBuilder->CreateStmtLabel(endLblIdx);
  cgFunc->SetEndLabel(*endLabel);
  cgFunc->GetFunction().GetBody()->InsertLast(endLabel);
  ASSERT(cgFunc->GetFunction().GetBody()->GetLast() == endLabel, "last stmt must be a endLabel");
}

AnalysisResult *CgDoCreateLabel::Run(CGFunc *cgFunc, CgFuncResultMgr *cgFuncResultMgr) {
  (void)cgFuncResultMgr;
  MemPool *memPool = NewMemPool();
  ASSERT(cgFunc != nullptr, "expect a cgfunc in CgDoCreateLabel");
  LabelCreation *labelCreate = memPool->New<LabelCreation>(*cgFunc);
  labelCreate->Run();
  return nullptr;
}
} /* namespace maplebe */
