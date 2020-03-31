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
#ifndef MPLFE_INCLUDE_COMMON_FE_FILE_OPS_H
#define MPLFE_INCLUDE_COMMON_FE_FILE_OPS_H
#include <string>

namespace maple {
class FEFileOps {
 public:
  static std::string GetFilePath(const std::string &pathName);
  static std::string GetFileNameWithExt(const std::string &pathName);
  static std::string GetFileName(const std::string &pathName);
  static std::string GetFileExtName(const std::string &pathName);

 private:
  FEFileOps() = default;
  ~FEFileOps() = default;
};
} // namespace maple
#endif