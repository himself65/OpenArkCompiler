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
#include "mplfe_env.h"
#include "global_tables.h"
#include "mpl_logging.h"
#include "fe_options.h"

namespace maple {
MPLFEEnv MPLFEEnv::instance;

void MPLFEEnv::Init() {
  srcFileIdxNameMap.clear();
  srcFileIdxNameMap[0] = GStrIdx(0);
  globalLabelIdx = 1;
}

void MPLFEEnv::Finish() {
  srcFileIdxNameMap.clear();
}

uint32 MPLFEEnv::NewSrcFileIdx(const GStrIdx &nameIdx) {
  size_t idx = srcFileIdxNameMap.size();
  CHECK_FATAL(idx < UINT32_MAX, "idx is out of range");
  srcFileIdxNameMap[idx] = nameIdx;
  return static_cast<uint32>(idx);
}

GStrIdx MPLFEEnv::GetFileNameIdx(uint32 fileIdx) const {
  auto it = srcFileIdxNameMap.find(fileIdx);
  if (it == srcFileIdxNameMap.end()) {
    return GStrIdx(0);
  } else {
    return it->second;
  }
}

std::string MPLFEEnv::GetFileName(uint32 fileIdx) const {
  auto it = srcFileIdxNameMap.find(fileIdx);
  if (it == srcFileIdxNameMap.end() || it->second == 0) {
    return "unknown";
  } else {
    return GlobalTables::GetStrTable().GetStringFromStrIdx(it->second);
  }
}
}  // namespace maple
