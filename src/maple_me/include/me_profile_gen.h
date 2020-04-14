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
#ifndef MAPLE_ME_INCLUDE_PROFILEGEN_H
#define MAPLE_ME_INCLUDE_PROFILEGEN_H

#include "me_phase.h"
#include "me_pgo_instrument.h"
#include "bb.h"
#include "me_irmap.h"

namespace maple {
class BBEdge;
class MeProfGen : public PGOInstrument<BBEdge> {
 public:
  MeProfGen(MeFunction &func, MemPool &mp, MeIRMap &hMap, bool dump)
      : PGOInstrument(func, mp, dump), func(&func), hMap(&hMap) {
    Init();
  }
  void InstrumentFunc();
 private:
  void Init();
  void InstrumentBB(BB &bb);
  void SaveProfile();
  MeFunction *func;
  MeIRMap *hMap;
  static uint64 counterIdx;
  static uint64 totalBB;
  static uint64 instrumentBB;
  static MIRSymbol *bbCounterTabSym;
  static bool firstRun;
};

class MeDoProfGen : public MeFuncPhase {
 public:
  explicit MeDoProfGen(MePhaseID id) : MeFuncPhase(id) {}

  ~MeDoProfGen() = default;

  AnalysisResult *Run(MeFunction *func, MeFuncResultMgr *m, ModuleResultMgr *mrm) override;
  std::string PhaseName() const override {
    return "profileGen";
  }
};
}  // namespace maple
#endif  // MAPLE_ME_INCLUDE_PROFILEGEN_H
