/*
 * Copyright (c) [2019] Huawei Technologies Co.,Ltd.All rights reserved.
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
/* emit IR to specified file */
AnalysisResult *MeDoEmit::Run(MeFunction *func, MeFuncResultMgr *m, ModuleResultMgr *mrm) {
  bool emitHssaOrAfter = (func->GetIRMap() != nullptr);
  if (func->NumBBs() > 0) {
    /* generate bblist after layout (bb physical position) */
    if (!MeOption::quiet) {
      CHECK_FATAL(m->GetAnalysisPhase(MeFuncPhase_BBLAYOUT) != nullptr, "null ptr check");
      LogInfo::MapleLogger() << "===== Check/run Depended Phase [ "
                             << m->GetAnalysisPhase(MeFuncPhase_BBLAYOUT)->PhaseName() << " ]=====\n";
    }
    BBLayout *layoutBBs = static_cast<BBLayout*>(m->GetAnalysisResult(MeFuncPhase_BBLAYOUT, func));
    if (!MeOption::quiet) {
      LogInfo::MapleLogger() << "===== Depended Phase ended =====\n";
    }
    ASSERT(layoutBBs != nullptr, "layout phase has problem");
    if (emitHssaOrAfter) {
      ASSERT(func->GetIRMap() != nullptr, "null ptr check");
      MIRFunction *mirFunction = func->GetMirFunc();
      if (mirFunction->GetCodeMempool() != nullptr) {
        mempoolctrler.DeleteMemPool(mirFunction->GetCodeMempool());
      }
      mirFunction->SetCodeMemPool(mempoolctrler.NewMemPool("IR from IRMap::Emit()"));
      mirFunction->GetCodeMPAllocator().SetMemPool(mirFunction->GetCodeMempool());
      mirFunction->SetBody(mirFunction->GetCodeMempool()->New<BlockNode>());
      // initialize is_deleted field to true; will reset when emitting Maple IR
      for (size_t k = 1; k < mirFunction->GetSymTab()->GetSymbolTableSize(); k++) {
        MIRSymbol *sym = mirFunction->GetSymTab()->GetSymbolFromStIdx(k);
        if (sym->GetSKind() == kStVar) {
          sym->SetIsDeleted();
        }
      }
      for (BB *bb : layoutBBs->GetBBs()) {
        ASSERT(bb != nullptr, "null ptr check");
        func->GetIRMap()->EmitBB(*bb, *mirFunction->GetBody());
      }
    } else {
      auto *mirFunc = func->GetMirFunc();
      if (mirFunc != nullptr) {
        func->EmitBeforeHSSA(*mirFunc, layoutBBs->GetBBs());
      }
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
