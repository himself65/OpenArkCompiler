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
#include "fe_file_ops.h"

namespace maple {
std::string FEFileOps::GetFilePath(const std::string &pathName) {
  size_t pos = pathName.find_last_of('/');
  if (pos == std::string::npos) {
    return "";
  } else {
    return pathName.substr(0, pos + 1);
  }
}

std::string FEFileOps::GetFileNameWithExt(const std::string &pathName) {
  size_t pos = pathName.find_last_of('/');
  if (pos == std::string::npos) {
    return pathName;
  } else {
    return pathName.substr(pos + 1);
  }
}

std::string FEFileOps::GetFileName(const std::string &pathName) {
  std::string nameWithExt = GetFileNameWithExt(pathName);
  size_t pos = nameWithExt.find_last_of(".");
  if (pos == std::string::npos) {
    return nameWithExt;
  } else {
    return nameWithExt.substr(0, pos);
  }
}

std::string FEFileOps::GetFileExtName(const std::string &pathName) {
  std::string nameWithExt = GetFileNameWithExt(pathName);
  size_t pos = nameWithExt.find_last_of(".");
  if (pos == std::string::npos) {
    return "";
  } else {
    return nameWithExt.substr(pos + 1);
  }
}
}  // namespace maple