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
#ifndef MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_YIELDPOINT_H
#define MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_YIELDPOINT_H

#include "yieldpoint.h"

namespace maplebe {
using namespace maple;

class AArch64YieldPointInsertion : public YieldPointInsertion {
 public:
  explicit AArch64YieldPointInsertion(CGFunc &func) : YieldPointInsertion(func) {}

  ~AArch64YieldPointInsertion() override = default;

  void Run() override;

 private:
  void InsertYieldPoint();
};
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_YIELDPOINT_H */