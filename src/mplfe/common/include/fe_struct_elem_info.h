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
#ifndef MPLFE_INCLUDE_COMMON_FE_STRUCT_ELEM_INFO_H
#define MPLFE_INCLUDE_COMMON_FE_STRUCT_ELEM_INFO_H
#include <memory>
#include "global_tables.h"

namespace maple {
class FEStructElemInfo {
 public:
  FEStructElemInfo(const GStrIdx &argFullNameIdxOrin, const GStrIdx &argFullNameIdxMpl)
      : fullNameIdxOrin(argFullNameIdxOrin),
        fullNameIdxMpl(argFullNameIdxMpl),
        isMethod(false),
        isStatic(false) {}

  ~FEStructElemInfo() = default;
  void SetIsMethod(bool flag) {
    isMethod = flag;
  }

  bool IsMethod() const {
    return isMethod;
  }

  void SetIsStatic(bool flag) {
    isStatic = flag;
  }

  bool IsStatic() const {
    return isStatic;
  }

  void SetFullNameIdxOrin(const GStrIdx &idx) {
    fullNameIdxOrin = idx;
  }

  GStrIdx GetFullNameIdxOrin() const {
    return fullNameIdxOrin;
  }

  void SetFullNameIdxMpl(const GStrIdx &idx) {
    fullNameIdxMpl = idx;
  }

  GStrIdx GetFullNameIdxMpl() const {
    return fullNameIdxMpl;
  }

  const std::string &GetFullNameOrin() const {
    return GlobalTables::GetStrTable().GetStringFromStrIdx(fullNameIdxOrin);
  }

  const std::string &GetFullNameMpl() const {
    return GlobalTables::GetStrTable().GetStringFromStrIdx(fullNameIdxMpl);
  }

 private:
  GStrIdx fullNameIdxOrin;
  GStrIdx fullNameIdxMpl;
  bool isMethod : 1;
  bool isStatic : 1;
};  // class FEStructElemInfo

using UniqueFEStructElemInfo = std::unique_ptr<FEStructElemInfo>;
}  // namespace maple
#endif  // MPLFE_INCLUDE_COMMON_FE_STRUCT_ELEM_INFO_H