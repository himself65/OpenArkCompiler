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
#ifndef MPLFE_INCLUDE_COMMON_FE_CONFIG_PARALLEL_H
#define MPLFE_INCLUDE_COMMON_FE_CONFIG_PARALLEL_H
#include "types_def.h"
#include "mpl_logging.h"

namespace maple {
class FEConfigParallel {
 public:
  FEConfigParallel();
  ~FEConfigParallel() = default;
  static FEConfigParallel GetInstance() {
    return instance;
  }

  uint32 GetNThread() const {
    return nThread;
  }

  void EnableParallel() {
    enableParallel = true;
  }

  void DisableParallel() {
    enableParallel = false;
  }

  bool IsInParallelMode() const {
    return enableParallel && (nThread > 1);
  }

 private:
  static FEConfigParallel instance;
  uint32 nThread;
  bool enableParallel;
};

#define MPLFE_PARALLEL_FORBIDDEN()                                         \
  CHECK_FATAL(FEConfigParallel::GetInstance().IsInParallelMode() == false, "this method is forbidden in parallel mode");
}  // namespace maple
#endif  // MPLFE_INCLUDE_COMMON_FE_PARALLEL_CONFIG_H
