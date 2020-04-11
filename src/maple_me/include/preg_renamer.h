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
#ifndef MAPLE_ME_INCLUDE_PREGRENAMER_H
#define MAPLE_ME_INCLUDE_PREGRENAMER_H
#include "me_irmap.h"
#include "ssa_pre.h"

namespace maple {
class PregRenamer {
 public:
  PregRenamer(MemPool &memPool, MeFunction &f, MeIRMap &irMap, bool enabledDebug)
      : alloc(&memPool), func(&f), irMap(&irMap), enabledDebug(enabledDebug) {}
  virtual ~PregRenamer() = default;
  void RunSelf() const;

 private:
  void EnqueDefUses(std::list<RegMeExpr*> &qu, RegMeExpr *node, std::set<RegMeExpr*> &curVisited) const;
  MapleAllocator alloc;
  MeFunction *func;
  MeIRMap *irMap;
  bool enabledDebug;
};

class MeDoPregRename : public MeFuncPhase {
 public:
  explicit MeDoPregRename(MePhaseID id) : MeFuncPhase(id) {}

  virtual ~MeDoPregRename() = default;
  AnalysisResult *Run(MeFunction *ir, MeFuncResultMgr *m, ModuleResultMgr *mrm) override;
  std::string PhaseName() const override {
    return "pregrename";
  }
};
}  // namespace maple
#endif  // MAPLE_ME_INCLUDE_PREGRENAMER_H
