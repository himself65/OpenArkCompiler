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
#include "fe_function_phase_result.h"
#include "mpl_logging.h"

namespace maple {
void FEFunctionPhaseResult::Combine(const FEFunctionPhaseResult &result) {
  for (const std::string &name : result.phaseNames) {
    auto itResult = result.phaseTimes.find(name);
    CHECK_FATAL(itResult != result.phaseTimes.end(), "invalid result: without time recorded");
    int64 t = itResult->second;
    auto itLocal = phaseTimes.find(name);
    if (itLocal == phaseTimes.end()) {
      phaseNames.push_back(name);
      phaseTimes[name] = t;
    } else {
      phaseTimes[name] = itLocal->second + t;
    }
  }
}

void FEFunctionPhaseResult::Dump() {
  for (const std::string &name :phaseNames) {
    auto it = phaseTimes.find(name);
    CHECK_FATAL(it != phaseTimes.end(), "phase time is undefined for %s", name.c_str());
    INFO(kLncInfo, "[PhaseTime]  %s: %lld ns", name.c_str(), it->second);
  }
}

void FEFunctionPhaseResult::DumpMS() {
  for (const std::string &name :phaseNames) {
    auto it = phaseTimes.find(name);
    CHECK_FATAL(it != phaseTimes.end(), "phase time is undefined for %s", name.c_str());
    INFO(kLncInfo, "[PhaseTime]  %s: %.2lf ms", name.c_str(), it->second / 1000000.0);  // 1ms = 1000000 ns
  }
}

bool FEFunctionPhaseResult::Finish(bool isSuccess) {
  success = isSuccess;
  if (enable && recordTime) {
    timer.Stop();
    CHECK_FATAL(!currPhaseName.empty(), "Phase Name is empty");
    int64 t = timer.GetTimeNS();
    phaseTimes[currPhaseName] = t;
  }
  return success;
}
}  // namespace maple