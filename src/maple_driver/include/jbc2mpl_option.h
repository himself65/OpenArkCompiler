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
#ifndef MAPLE_JBC2MPL_OPTION_H
#define MAPLE_JBC2MPL_OPTION_H
#include "option_descriptor.h"

namespace maple {
enum JbcOptionIndex {
  kUseStringFactory,
  kOutMpl,
};

const mapleOption::Descriptor jbcUsage[] = {
  { kUnknown, 0, "", "", mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyUnknown,
    "========================================\n"
    " Usage: jbc2mpl [ options ]\n"
    " options:\n",
    "jbc2mpl",
    {} },
  { kUseStringFactory, 0, "", "use-string-factory", mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyNone,
    "  -use-string-factory    : Replace String.<init> by StringFactory call",
    "jbc2mpl",
    {} },
  { kOutMpl, 0, "o", "out", mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyRequired,
    "  -o, -out output.mpl    : output mpl name",
    "jbc2mpl",
    {} },
  { kUnknown, 0, nullptr, nullptr, mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyNone,
    nullptr,
    "jbc2mpl",
    {} }
};
} // namespace maple
#endif //MAPLE_JBC2MPL_OPTION_H
