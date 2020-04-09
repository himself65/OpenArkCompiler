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
#ifndef MPLFE_INCLUDE_FE_MACROS_H
#define MPLFE_INCLUDE_FE_MACROS_H
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <sys/time.h>
#include <iostream>
#include <inttypes.h>
#include "fe_options.h"
#include "mpl_logging.h"

#define TIMEIT(message, stmt)                                                                                      \
  if (FEOptions::GetInstance().IsDumpTime()) {                                                                     \
    struct timeval start, end;                                                                                     \
    (void)gettimeofday(&start, NULL);                                                                              \
    stmt;                                                                                                          \
    (void)gettimeofday(&end, NULL);                                                                                \
    float t = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)) * 1.0 / 1000000.0; \
    INFO(kLncInfo, "[TIME] %s: %.3f sec", message, t);                                                             \
  } else {                                                                                                         \
    stmt;                                                                                                          \
  }

#define PHASE_TIMER(message, stmt)                                                    \
  if (FEOptions::GetInstance().IsDumpPhaseTime()) {                                   \
    struct timespec start = {0, 0};                                                   \
    struct timespec end = {0, 0};                                                     \
    (void)clock_gettime(CLOCK_REALTIME, &start);                                      \
    stmt;                                                                             \
    (void)clock_gettime(CLOCK_REALTIME, &end);                                        \
    constexpr int64_t nsInS = 1000000000;                                             \
    int64_t t = (end.tv_sec - start.tv_sec) * nsInS + (end.tv_nsec - start.tv_nsec);  \
    INFO(kLncInfo, "[PhaseTime]   %s: %lld ns", message, t);                          \
  } else {                                                                            \
    stmt;                                                                             \
  }

#define FE_INFO_LEVEL(level, fmt, ...)                    \
  if (FEOptions::GetInstance().GetDumpLevel() >= level) { \
    INFO(kLncInfo, fmt, ##__VA_ARGS__);                   \
  }

#define BIT_XOR(v1, v2) (((v1) && (!(v2))) || ((!(v1)) && (v2)))
#endif  // ~MPLFE_INCLUDE_FE_MACROS_H