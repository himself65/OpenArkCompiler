/*
 * Copyright (c) [2019] Huawei Technologies Co.,Ltd.All rights reserved.
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
#include "file_utils.h"
#include <cstdio>
#include "string_utils.h"

namespace maple {
const std::string FileUtils::GetFileName(const std::string &filePath, const bool isWithExtension) {
  std::string fullFileName = StringUtils::GetStrAfterLast(filePath, FileSeperator::kFileSeperatorStr);
  if (isWithExtension) {
    return fullFileName;
  }
  return StringUtils::GetStrBeforeLast(fullFileName, ".");
}

const std::string FileUtils::GetFileExtension(const std::string &filePath) {
  return StringUtils::GetStrAfterLast(filePath, ".", true);
}

const std::string FileUtils::GetFileFolder(const std::string &filePath) {
  std::string folder = StringUtils::GetStrBeforeLast(filePath, FileSeperator::kFileSeperatorStr, true);
  return folder.empty() ? folder : (folder + FileSeperator::kFileSeperatorStr);
}

const std::string FileUtils::ConvertPathIfNeeded(const std::string &src) {
  std::string ret = src;
  ret = StringUtils::Replace(ret, FileSeperator::kFileSeperatorWindowsStyleStr,
                             FileSeperator::kFileSeperatorLinuxStyleStr);
  return ret;
}

const int FileUtils::Remove(const std::string &filePath) {
  return remove(filePath.c_str());
}

}  // namespace maple
