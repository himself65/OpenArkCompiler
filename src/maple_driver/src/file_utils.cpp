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
#include <cstdio>
#include <cstring>
#include <climits>
#include "file_utils.h"
#include "string_utils.h"
#include "mpl_logging.h"

#ifdef _WIN32
#include <shlwapi.h>
#endif

namespace {
const char kFileSeperatorLinuxStyleChar = '/';
const char kFileSeperatorWindowsStyleChar = '\\';
const std::string kFileSeperatorLinuxStyleStr = std::string(1, kFileSeperatorLinuxStyleChar);
const std::string kFileSeperatorWindowsStyleStr = std::string(1, kFileSeperatorWindowsStyleChar);
} // namespace

namespace maple {
const char kFileSeperatorChar = kFileSeperatorLinuxStyleChar;

const std::string kFileSeperatorStr = kFileSeperatorLinuxStyleStr;

std::string FileUtils::GetRealPath(const std::string &filePath) {
#ifdef _WIN32
  char *path = nullptr;
  if (filePath.size() > PATH_MAX  || !PathCanonicalize(path, filePath.c_str())) {
    CHECK_FATAL(false, "invalid file path");
  }
#else
  char path[PATH_MAX] = {0};
  if (filePath.size() > PATH_MAX  || realpath(filePath.c_str(), path) == nullptr) {
    CHECK_FATAL(false, "invalid file path");
  }
#endif
  std::string result(path, path + strlen(path));
  return result;
}

std::string FileUtils::GetFileName(const std::string &filePath, bool isWithExtension) {
  std::string fullFileName = StringUtils::GetStrAfterLast(filePath, kFileSeperatorStr);
#ifdef _WIN32
  fullFileName = StringUtils::GetStrAfterLast(fullFileName, kFileSeperatorLinuxStyleStr);
#endif
  if (isWithExtension) {
    return fullFileName;
  }
  return StringUtils::GetStrBeforeLast(fullFileName, ".");
}

std::string FileUtils::GetFileExtension(const std::string &filePath) {
  return StringUtils::GetStrAfterLast(filePath, ".", true);
}

std::string FileUtils::GetFileFolder(const std::string &filePath) {
  std::string folder = StringUtils::GetStrBeforeLast(filePath, kFileSeperatorStr, true);
  std::string curSlashType = kFileSeperatorStr;
#ifdef _WIN32
  if (folder.empty()) {
    curSlashType = kFileSeperatorLinuxStyleStr;
    folder = StringUtils::GetStrBeforeLast(filePath, curSlashType, true);
  }
#endif
  return folder.empty() ? ("." + curSlashType) : (folder + curSlashType);
}

int FileUtils::Remove(const std::string &filePath) {
  return remove(filePath.c_str());
}

std::string FileUtils::AppendMapleRootIfNeeded(bool needRootPath, const std::string &path,
                                               const std::string &defaultRoot) {
  if (!needRootPath) {
    return path;
  }
  std::ostringstream ostrStream;
  if (getenv(kMapleRoot) == nullptr) {
    ostrStream << defaultRoot << path;
  } else {
    ostrStream << getenv(kMapleRoot) << kFileSeperatorStr << path;
  }
  return ostrStream.str();
}
}  // namespace maple
