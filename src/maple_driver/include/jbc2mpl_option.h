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
using namespace mapleOption;

enum JbcOptionIndex {
  kUseStringFactory,
  kOutMpl,
};

const Descriptor jbcUsage[] = {
  { kUnknown, 0, "", "", kBuildTypeAll, kArgCheckPolicyUnknown,
    "========================================\n"
    " Usage: jbc2mpl [ options ]\n"
    " options:\n",
    "jbc2mpl",
    {} },
  { kUseStringFactory, 0, "", "use-string-factory", kBuildTypeAll, kArgCheckPolicyNone,
    "  -use-string-factory    : Replace String.<init> by StringFactory call",
    "jbc2mpl",
    {} },
  { kOutMpl, 0, "o", "out", kBuildTypeAll, kArgCheckPolicyRequired,
    "  -o, -out output.mpl    : output mpl name",
    "jbc2mpl",
    {} },
  { kUnknown, 0, nullptr, nullptr, kBuildTypeAll, kArgCheckPolicyNone,
    nullptr,
    "jbc2mpl",
    {} }
};
} // namespace maple
#endif //MAPLE_JBC2MPL_OPTION_H
