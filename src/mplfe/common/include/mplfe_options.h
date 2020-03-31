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
#ifndef MPLFE_INCLUDE_COMMON_MPLFE_OPTIONS_H
#define MPLFE_INCLUDE_COMMON_MPLFE_OPTIONS_H
#include <string>
#include <list>
#include "factory.h"
#include "parser_opt.h"
#include "option_parser.h"
#include "types_def.h"

namespace maple {
class MPLFEOptions {
 public:
  static MPLFEOptions &GetInstance() {
    return options;
  }
  void Init();
  static bool InitFactory();
  bool SolveArgs(int argc, char **argv);
  void DumpUsage() const;
  void DumpVersion() const;
  template <typename Out>
  static void Split(const std::string &s, char delim, Out result);
  static std::list<std::string> SplitByComma(const std::string &s);

 private:
  using OptionProcessFactory = FunctionFactory<uint32, bool, MPLFEOptions*, const mapleOption::Option&>;
  static MPLFEOptions options;

  MPLFEOptions();
  ~MPLFEOptions() = default;

  // option process
  bool ProcessHelp(const mapleOption::Option &opt);
  bool ProcessVersion(const mapleOption::Option &opt);

  // input control options
  bool ProcessInClass(const mapleOption::Option &opt);
  bool ProcessInJar(const mapleOption::Option &opt);
  bool ProcessInputMplt(const mapleOption::Option &opt);
  bool ProcessInputMpltFromSys(const mapleOption::Option &opt);
  bool ProcessInputMpltFromApk(const mapleOption::Option &opt);

  // output control options
  bool ProcessOutputPath(const mapleOption::Option &opt);
  bool ProcessOutputName(const mapleOption::Option &opt);
  bool ProcessGenMpltOnly(const mapleOption::Option &opt);
  bool ProcessGenAsciiMplt(const mapleOption::Option &opt);
  bool ProcessDumpInstComment(const mapleOption::Option &opt);

  // debug info control options
  bool ProcessDumpLevel(const mapleOption::Option &opt);
  bool ProcessDumpTime(const mapleOption::Option &opt);
  bool ProcessDumpPhaseTime(const mapleOption::Option &opt);
  bool ProcessDumpPhaseTimeDetail(const mapleOption::Option &opt);

  // java compiler options
  bool ProcessModeForJavaStaticFieldName(const mapleOption::Option &opt);
  bool ProcessJBCInfoUsePathName(const mapleOption::Option &opt);
  bool ProcessDumpJBCStmt(const mapleOption::Option &opt);
  bool ProcessDumpJBCBB(const mapleOption::Option &opt);
  bool ProcessDumpJBCAll(const mapleOption::Option &opt);
  bool ProcessDumpJBCErrorOnly(const mapleOption::Option &opt);
  bool ProcessDumpJBCFuncName(const mapleOption::Option &opt);

  // general stmt/bb/cfg debug options
  bool ProcessDumpGeneralCFGGraph(const mapleOption::Option &opt);

  // multi-thread control options
  bool ProcessNThreads(const mapleOption::Option &opt);
  bool ProcessDumpThreadTime(const mapleOption::Option &opt);
  bool ProcessReleaseAfterEmit(const mapleOption::Option &opt);

  // non-option process
  void ProcessInputFiles(const std::vector<std::string> &inputs);
};  // class MPLFEOptions
}  // namespace maple
#endif  // MPLFE_INCLUDE_COMMON_MPLFE_OPTIONS_H