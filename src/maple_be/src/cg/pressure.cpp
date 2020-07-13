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
#include "pressure.h"
#include "aarch64_schedule.h"
#include "deps.h"

namespace maplebe {
/* ------- RegPressure function -------- */
int32 RegPressure::maxRegClassNum = 0;

/* print regpressure information */
void RegPressure::DumpRegPressure() const {
  PRINT_STR_VAL("Priority: ", priority);
  PRINT_STR_VAL("maxDepth: ", maxDepth);
  PRINT_STR_VAL("near: ", near);
  PRINT_STR_VAL("callNum: ", callNum);

  LogInfo::MapleLogger() << "\n";
}
} /* namespace maplebe */

