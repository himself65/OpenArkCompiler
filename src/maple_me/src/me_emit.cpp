/*
 * Copyright (c) [2019-2020] Huawei Technologies Co.,Ltd.All rights reserved.
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
#include "me_emit.h"
#include "me_bb_layout.h"
#include "me_irmap.h"
#include "me_cfg.h"

namespace maple {
// emit IR to specified file
AnalysisResult *MeDoEmit::Run(MeFunction *func, MeFuncResultMgr*, ModuleResultMgr*) {
  if (func->NumBBs() > 0) {
    CHECK_FATAL(func->GetIRMap() != nullptr, "Why not hssa?");
    // generate bblist after layout (bb physical position)
    CHECK_FATAL(func->HasLaidOut(), "Check/Run bb layout phase.");
    auto layoutBBs = func->GetLaidOutBBs();
    MIRFunction *mirFunction = func->GetMirFunc();
    if (mirFunction->GetCodeMempool() != nullptr) {
      mirFunction->GetCodeMempool()->Release();
    }
    mirFunction->SetCodeMemPool(memPoolCtrler.NewMemPool("IR from IRMap::Emit()"));
    mirFunction->GetCodeMPAllocator().SetMemPool(mirFunction->GetCodeMempool());
    mirFunction->SetBody(mirFunction->GetCodeMempool()->New<BlockNode>());
    // initialize is_deleted field to true; will reset when emitting Maple IR
    for (size_t k = 1; k < mirFunction->GetSymTab()->GetSymbolTableSize(); ++k) {
      MIRSymbol *sym = mirFunction->GetSymTab()->GetSymbolFromStIdx(k);
      if (sym->GetSKind() == kStVar) {
        sym->SetIsDeleted();
      }
    }
    for (BB *bb : layoutBBs) {
      ASSERT(bb != nullptr, "Check bblayout phase");
      func->GetIRMap()->EmitBB(*bb, *mirFunction->GetBody());
    }
    if (DEBUGFUNC(func)) {
      LogInfo::MapleLogger() << "\n==============after meemit =============" << '\n';
      func->GetMirFunc()->Dump();
    }
    if (DEBUGFUNC(func)) {
      func->GetTheCfg()->DumpToFile("emit", true);
    }
  }
  return nullptr;
}
}  // namespace maple
