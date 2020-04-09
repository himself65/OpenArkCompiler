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
#include <mutex>
#include <thread>
#include <set>
#include "types_def.h"
#include "mpl_logging.h"

namespace maple {
class FEConfigParallel {
 public:
  FEConfigParallel();
  ~FEConfigParallel() = default;
  static FEConfigParallel &GetInstance() {
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

  void RegisterRunThreadID(std::thread::id tid) {
    mtx.lock();
    CHECK_FATAL(runThreadIDs.insert(tid).second == true, "failed to register thread id");
    mtx.unlock();
  }

  bool RunThreadParallelForbidden() {
    if (!enableParallel) {
      return false;
    }
    std::thread::id tid = std::this_thread::get_id();
    return runThreadIDs.find(tid) != runThreadIDs.end();
  }

 private:
  static FEConfigParallel instance;
  uint32 nThread;
  bool enableParallel;
  std::set<std::thread::id> runThreadIDs;
  std::mutex mtx;
};

#define MPLFE_PARALLEL_FORBIDDEN()                                                           \
  do {                                                                                       \
    if (FEConfigParallel::GetInstance().RunThreadParallelForbidden()) {                      \
      maple::logInfo.EmitErrorMessage("MPLFE_PARALLEL_FORBIDDEN", __FILE__, __LINE__, "\n"); \
      FATAL(kLncFatal, "Forbidden invocation in parallel run thread");                       \
    }                                                                                        \
  } while (0)
}  // namespace maple
#endif  // MPLFE_INCLUDE_COMMON_FE_PARALLEL_CONFIG_H
