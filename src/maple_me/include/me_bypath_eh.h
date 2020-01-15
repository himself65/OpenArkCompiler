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
#ifndef MAPLEME_INCLUDE_MEBYPATHEH_H
#define MAPLEME_INCLUDE_MEBYPATHEH_H

#include "me_phase.h"
#include "bb.h"
#include "class_hierarchy.h"
#include "mir_builder.h"

namespace maple {
class MeDoBypathEH : public MeFuncPhase {
 public:
  MeDoBypathEH(MePhaseID id) : MeFuncPhase(id) {}
  ~MeDoBypathEH() = default;

  AnalysisResult *Run(MeFunction *func, MeFuncResultMgr *m, ModuleResultMgr *mrm) override;
  std::string PhaseName() const override {
    return "bypatheh";
  }
 private:
  bool DoBypathException(BB *tryBB, BB *catchBB, Klass *catchClass, StIdx e, KlassHierarchy *kh, MeFunction *func,
                         StmtNode *syncExitStmt);
  StmtNode *IsSyncExit(BB *syncBB, MeFunction *func, LabelIdx secondLabel);
  void BypathException(MeFunction *func, KlassHierarchy *kh);
};
}  // namespace maple
#endif
