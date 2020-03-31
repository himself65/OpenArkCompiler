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
#ifndef MPLFE_INCLUDE_COMMON_FE_TIMER_H
#define MPLFE_INCLUDE_COMMON_FE_TIMER_H
#include <string>
#include <functional>
#include "mpl_timer.h"
#include "mpl_logging.h"
#include "fe_options.h"

namespace maple {
class FETimer {
 public:
  FETimer() = default;
  ~FETimer() = default;
  void Start() {
    if (!FEOptions::GetInstance().IsDumpTime()) {
      return;
    }
    timer.Start();
  }

  void StartAndDump(const std::string &message) {
    if (!FEOptions::GetInstance().IsDumpTime()) {
      return;
    }
    CHECK_FATAL(!message.empty(), "message is empty");
    timer.Start();
    INFO(kLncInfo, "[Time] %s: starting...", message.c_str());
  }

  void StopAndDumpTimeMS(const std::string &message) {
    if (!FEOptions::GetInstance().IsDumpTime()) {
      return;
    }
    CHECK_FATAL(!message.empty(), "message is empty");
    timer.Stop();
    INFO(kLncInfo, "[Time] %s: finished (%ld ms)", message.c_str(), timer.ElapsedMilliseconds());
  }

  void StopAndDumpTimeS(const std::string &message) {
    if (!FEOptions::GetInstance().IsDumpTime()) {
      return;
    }
    CHECK_FATAL(!message.empty(), "message is empty");
    timer.Stop();
    INFO(kLncInfo, "[Time] %s: finished (%lf s)", message.c_str(), timer.ElapsedMilliseconds() / 1000.0);
  }

 private:
  MPLTimer timer;
};  // class FETimer
}  // namespace maple
#endif  // MPLFE_INCLUDE_COMMON_FE_TIMER_H