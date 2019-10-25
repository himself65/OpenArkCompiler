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
#ifndef MAPLE_DRIVER_INCLUDE_SAFE_EXE_H
#define MAPLE_DRIVER_INCLUDE_SAFE_EXE_H
#if __linux__ or __linux
#include <stdlib.h>
#endif

#if __linux__ or __linux
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "error_code.h"
#endif
#include "mpl_logging.h"
#include "string_utils.h"
#include "securec.h"

namespace maple {
class SafeExe {
 public:
  /**
   * Current tool is for linux only
   */
  static int Exe(const std::string &cmd, const std::string &args) {
    LogInfo::MapleLogger() << "Starting:" << cmd << args << '\n';
    int ret = ErrorCode::kErrorNoError;
    if (StringUtils::HasCommandInjectionChar(cmd) || StringUtils::HasCommandInjectionChar(args)) {
      LogInfo::MapleLogger() << "Error while Exe, cmd: " << cmd << " args: " << args << '\n';
      return -1;
    }

#if __linux__ or __linux
    std::vector<std::string> tmpArgs;
    StringUtils::Split(args, tmpArgs, ' ');
    // remove ' ' in vector
    for (auto iter = tmpArgs.begin(); iter != tmpArgs.end();) {
      if (*iter == " " || *iter == "") {
        iter = tmpArgs.erase(iter);
      } else {
        iter++;
      }
    }
    tmpArgs.insert(tmpArgs.begin(), cmd);
    // extra space for exe name and args
    char **argv = new char* [tmpArgs.size() + 1];
    // argv[0] is program name
    // copy args
    for (int j = 0;  j < tmpArgs.size();  ++j) {
       int strLength = tmpArgs[j].size();
       argv[j] = new char[strLength + 1];
       strncpy_s(argv[j], strLength + 1, tmpArgs[j].c_str(), strLength);
       argv[j][strLength] = '\0';
    }
    // end of arguments sentinel is NULL
    argv[tmpArgs.size()] = NULL;
    pid_t pid = fork();
    if (pid == 0) {
      // child process
      fflush(NULL);
      if (execv(cmd.c_str(), argv) < 0) {
        for (int j = 0;  j < tmpArgs.size();  ++j) {
          delete [] argv[j];
        }
        delete [] argv;
        exit(1);
      }
    } else {
      // parent process
      int status = -1;
      waitpid(pid, &status, 0);
      if (!WIFEXITED(status)) {
        LogInfo::MapleLogger() << "Error while Exe, cmd: " << cmd << " args: " << args << '\n';
        ret = ErrorCode::kErrorCompileFail;
      } else if (WEXITSTATUS(status) != 0) {
        LogInfo::MapleLogger() << "Error while Exe, cmd: " << cmd << " args: " << args << '\n';
        ret = ErrorCode::kErrorCompileFail;
      }
    }
    for (int j = 0;  j < tmpArgs.size();  ++j) {
      delete [] argv[j];
    }
    delete [] argv;
    return ret;
#else
    return -1;
#endif
  }
};
}  // namespace maple
#endif  // MAPLE_DRIVER_INCLUDE_SAFE_EXE_H
