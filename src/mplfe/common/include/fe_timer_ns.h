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
#ifndef MPLFE_INCLUDE_COMMON_FE_TIMER_NS_H
#define MPLFE_INCLUDE_COMMON_FE_TIMER_NS_H
#include <ctime>
#include <cstdlib>
#include <inttypes.h>

namespace maple {
class FETimerNS {
 public:
  FETimerNS() = default;
  ~FETimerNS() = default;
  void Start();
  void Stop();
  int64_t GetTimeNS() const;

 private:
  struct timespec timeStart = {0, 0};
  struct timespec timeEnd = {0, 0};
};  // class FETimerNS
}  // namespace maple
#endif  // MPLFE_INCLUDE_COMMON_FE_TIMER_NS_H