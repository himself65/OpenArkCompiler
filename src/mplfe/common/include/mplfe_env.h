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
#ifndef MPLFE_INCLUDE_COMMON_MPLFE_ENV_H
#define MPLFE_INCLUDE_COMMON_MPLFE_ENV_H
#include <fstream>
#include <string>
#include <map>
#include "mir_module.h"

namespace maple {
class MPLFEEnv {
 public:
  void Init();
  void Finish();
  uint32 NewSrcFileIdx(const GStrIdx &nameIdx);
  GStrIdx GetFileNameIdx(uint32 fileIdx) const;
  std::string GetFileName(uint32 fileIdx) const;
  static MPLFEEnv &GetInstance() {
    return instance;
  }

  uint32 GetGlobalLabelIdx() const {
    return globalLabelIdx;
  }

  void IncrGlobalLabelIdx() {
    globalLabelIdx++;
  }

 private:
  static MPLFEEnv instance;
  std::map<uint32, GStrIdx> srcFileIdxNameMap;
  uint32 globalLabelIdx = GStrIdx(0);
  MPLFEEnv() = default;
  ~MPLFEEnv() = default;
};  // class MPLFEEnv
}  // namespace maple
#endif  // MPLFE_INCLUDE_COMMON_MPLFE_ENV_H
