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
#ifndef MPLFE_INCLUDE_COMMON_FE_FUNCTION_PHASE_RESULT_H
#define MPLFE_INCLUDE_COMMON_FE_FUNCTION_PHASE_RESULT_H
#include <map>
#include <string>
#include <list>
#include "types_def.h"
#include "fe_timer_ns.h"

namespace maple {
class FEFunctionPhaseResult {
 public:
  explicit FEFunctionPhaseResult(bool argRecordTime = false)
      : recordTime(argRecordTime),
        success(true),
        enable(true) {}

  ~FEFunctionPhaseResult() = default;
  void Combine(const FEFunctionPhaseResult &result);
  void Dump();
  void DumpMS();
  bool Finish(bool isSuccess);
  bool Finish() {
    return Finish(success);
  }

  void Start() {
    if (enable && recordTime) {
      timer.Start();
    }
  }

  bool IsSuccess() const {
    return success;
  }

  void Disable() {
    enable = false;
  }

  void Enable() {
    enable = true;
  }

  void RegisterPhaseName(const std::string &name) {
    if (enable && recordTime) {
      currPhaseName = name;
      phaseNames.push_back(name);
    }
  }

  void RegisterPhaseNameAndStart(const std::string &name) {
    if (enable && recordTime) {
      currPhaseName = name;
      phaseNames.push_back(name);
      timer.Start();
    }
  }

 private:
  FETimerNS timer;
  bool recordTime : 1;
  bool success : 1;
  bool enable : 1;
  std::string currPhaseName = "";
  std::list<std::string> phaseNames;
  std::map<std::string, int64> phaseTimes;
};
}  // namespace maple
#endif  // MPLFE_INCLUDE_COMMON_FE_FUNCTION_PHASE_RESULT_H