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
#ifndef MAPLEBE_INCLUDE_CG_CG_PHASE_H
#define MAPLEBE_INCLUDE_CG_CG_PHASE_H

#include <string>
#include <map>
#include "phase.h"
#include "module_phase.h"

namespace maplebe {
using namespace maple;

enum CgPhaseID : uint8 {
  kCgPhaseDonothing,
#define FUNCAPHASE(CGPHASEID, CLASSNACG) CGPHASEID,
#define FUNCTPHASE(CGPHASEID, CLASSNACG) CGPHASEID,
#include "cg_phases.def"
#undef FUNCAPHASE
#undef FUNCTPHASE
  kCgPhaseMax
};

class FuncPhase;
class CGFunc;

using CgFuncResultMgr = AnalysisResultManager<CGFunc, CgPhaseID, FuncPhase>;

class FuncPhase : public Phase {
 public:
  /* init prev_phasename is nullptr */
  explicit FuncPhase(CgPhaseID id) : Phase(), phaseId(id), prevPhaseName("") {}
  ~FuncPhase() override = default;

  void ClearString() {
    prevPhaseName.clear();
    prevPhaseName.shrink_to_fit();
  }
  /*
   * if phase is analysis phase, return analysis result
   * else return nullptr */
  virtual AnalysisResult *Run(CGFunc *cgFunc, CgFuncResultMgr *cgFuncResultMgr) = 0;

  const std::string &GetPreviousPhaseName() const {
    return prevPhaseName;
  }

  void SetPreviousPhaseName(const std::string &phaseName) {
    prevPhaseName = phaseName;
  }

  CgPhaseID GetPhaseID() const {
    return phaseId;
  }

  std::string PhaseName() const override = 0;

  virtual bool CanSkip() const {
    return false;
  }
 private:
  CgPhaseID phaseId;
  std::string prevPhaseName; /* used in filename for emit */
};
}  /* namespace maplebe */

#define CGFUNCPHASE(CLASSNAME, PHASENAME)                                           \
  class CLASSNAME : public FuncPhase {                                              \
   public:                                                                          \
    CLASSNAME(CgPhaseID id) : FuncPhase(id) {}                                      \
    ~CLASSNAME() = default;                                                         \
    AnalysisResult *Run(CGFunc *cgFunc, CgFuncResultMgr *cgFuncResultMgr) override; \
    std::string PhaseName() const override {                                        \
      return PHASENAME;                                                             \
    }                                                                               \
  };
#define CGFUNCPHASE_CANSKIP(CLASSNAME, PHASENAME)                                   \
  class CLASSNAME : public FuncPhase {                                              \
   public:                                                                          \
    CLASSNAME(CgPhaseID id) : FuncPhase(id) {}                                      \
    ~CLASSNAME() = default;                                                         \
    AnalysisResult *Run(CGFunc *cgFunc, CgFuncResultMgr *cgFuncResultMgr) override; \
    std::string PhaseName() const override {                                        \
      return PHASENAME;                                                             \
    }                                                                               \
    bool CanSkip() const override {                                                 \
      return true;                                                                  \
    }                                                                               \
  };

#endif  /* MAPLEBE_INCLUDE_CG_CG_PHASE_H */