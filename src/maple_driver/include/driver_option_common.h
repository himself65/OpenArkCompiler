/*
 * Copyright (c) [2019-2020] Huawei Technologies Co.,Ltd.All rights reserved.
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
#ifndef DRIVER_OPTIONS_COMMON_H
#define DRIVER_OPTIONS_COMMON_H
#include "option_descriptor.h"

namespace maple {
enum DriverOptionIndex {
  kUnknown,
  kHelp,
  kVersion,
  kInFile,
  kInMplt,
  kOptimization0,
  kOptimization2,
  kGCOnly,
  kMplcgOpt,
  kMeOpt,
  kMpl2MplOpt,
  kSaveTemps,
  kRun,
  kOption,
  kTimePhases,
  kGenMeMpl,
  kGenVtableImpl,
  kVerbose,
  kAllDebug,
  kHelpLevel,
  kCommonOptionEnd,
};

class MapleDriverOptionBase {
 public:
  void CreateUsages(const mapleOption::Descriptor usage[]) {
    for (size_t i = 0; usage[i].help != ""; ++i) {
      usageVec.push_back(usage[i]);
    }
  }
  const std::vector<mapleOption::Descriptor> &GetUsageVec() const {
    return usageVec;
  }
  MapleDriverOptionBase() = default;
  ~MapleDriverOptionBase() = default;

 private:
  std::vector<mapleOption::Descriptor> usageVec;
};

class DriverOptionCommon : public MapleDriverOptionBase {
 public:
  ~DriverOptionCommon() = default;
  static DriverOptionCommon &GetInstance();

 private:
  DriverOptionCommon();
};
}  // namespace maple
#endif //DRIVER_OPTIONS_COMMON_H
