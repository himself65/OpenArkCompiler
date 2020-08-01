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
#ifndef MPLFE_INCLUDE_MPLFE_UT_OPTIONS_H
#define MPLFE_INCLUDE_MPLFE_UT_OPTIONS_H
#include <string>
#include <list>
#include "driver_option_common.h"

namespace maple {
class MPLFEUTOptions : public maple::MapleDriverOptionBase {
 public:
  MPLFEUTOptions();
  ~MPLFEUTOptions() = default;
  void DumpUsage() const;
  bool SolveArgs(int argc, char **argv);
  template <typename Out>
  static void Split(const std::string &s, char delim, Out result);

  static MPLFEUTOptions &GetInstance() {
    static MPLFEUTOptions options;
    return options;
  }

  bool GetRunAll() const {
    return runAll;
  }

  bool GetRunAllWithCore() const {
    return runAllWithCore;
  }

  bool GetGenBase64() const {
    return genBase64;
  }

  std::string GetBase64SrcFileName() const {
    return base64SrcFileName;
  }

  std::string GetCoreMpltName() const {
    return coreMpltName;
  }

  const std::list<std::string> &GetClassFileList() const {
    return classFileList;
  }

  const std::list<std::string> &GetJarFileList() const {
    return jarFileList;
  }

  const std::list<std::string> &GetMpltFileList() const {
    return mpltFileList;
  }

 private:
  bool runAll;
  bool runAllWithCore;
  bool genBase64;
  std::string base64SrcFileName;
  std::string coreMpltName;
  std::list<std::string> classFileList;
  std::list<std::string> jarFileList;
  std::list<std::string> mpltFileList;
};
}  // namespace maple

#endif  // MPLFE_INCLUDE_MPLFE_UT_OPTIONS_H