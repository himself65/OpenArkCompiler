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
#ifndef MAPLE_DRIVER_INCLUDE_COMPILER_H
#define MAPLE_DRIVER_INCLUDE_COMPILER_H
#include <map>
#include <unordered_set>
#include "error_code.h"
#include "mpl_options.h"
#include "me_option.h"
#include "option.h"
#include "mir_module.h"
#include "mir_parser.h"

namespace maple {
extern const std::string kBinNameJbc2mpl;
extern const std::string kBinNameMe;
extern const std::string kBinNameMpl2mpl;
extern const std::string kBinNameMplcg;
extern const std::string kBinNameMapleComb;

class Compiler {
 public:
  explicit Compiler(const std::string &name) : name(name) {}

  virtual ~Compiler() = default;

  virtual ErrorCode Compile(const MplOptions &options, MIRModulePtr &theModule);

  virtual void GetTmpFilesToDelete(const MplOptions &mplOptions, std::vector<std::string> &tempFiles) const {}

  virtual std::unordered_set<std::string> GetFinalOutputs(const MplOptions &mplOptions) const {
    return std::unordered_set<std::string>();
  }

  virtual void PrintCommand(const MplOptions &options) const {}

 protected:
  virtual std::string GetBinPath(const MplOptions &mplOptions) const;
  virtual std::string GetBinName() const {
    return "";
  }

  virtual std::string GetInputFileName(const MplOptions &options) const {
    std::ostringstream stream;
    for (const auto &inputFile : options.GetSplitsInputFiles()) {
      stream << " " << inputFile;
    }
    return stream.str();
  }

  virtual DefaultOption GetDefaultOptions(const MplOptions &options) const {
    return DefaultOption();
  }

  virtual std::string AppendSpecialOption(const MplOptions &options, const std::string &optionStr) const {
    return optionStr;
  }

  virtual std::string AppendOptimization(const MplOptions &options, const std::string &optionStr) const;

  virtual std::vector<std::string> GetBinNames() const {
    return std::vector<std::string>();
  }

  bool CanAppendOptimization() const;

 private:
  const std::string name;
  std::string MakeOption(const MplOptions &options) const;
  void AppendDefaultOptions(std::map<std::string, MplOption> &finalOptions,
                            const std::map<std::string, MplOption> &defaultOptions) const;
  void AppendUserOptions(std::map<std::string, MplOption> &finalOptions,
                         const std::vector<mapleOption::Option> &userOption) const;
  void AppendOptions(std::map<std::string, MplOption> &finalOptions, const std::string &key, const std::string &value,
                     const std::string &connectSymbol) const;
  void AppendExtraOptions(std::map<std::string, MplOption> &finalOptions,
                          const std::map<std::string, std::vector<MplOption>> &extraOptions) const;
  std::map<std::string, MplOption> MakeDefaultOptions(const MplOptions &options) const;
  int Exe(const MplOptions &mplOptions, const std::string &options) const;
  const std::string &GetName() const {
    return name;
  }
};

class Jbc2MplCompiler : public Compiler {
 public:
  explicit Jbc2MplCompiler(const std::string &name) : Compiler(name) {}

  ~Jbc2MplCompiler() = default;

 private:
  std::string GetBinName() const override;
  DefaultOption GetDefaultOptions(const MplOptions &options) const override;
  void GetTmpFilesToDelete(const MplOptions &mplOptions, std::vector<std::string> &tempFiles) const override;
  std::unordered_set<std::string> GetFinalOutputs(const MplOptions &mplOptions) const override;
  std::vector<std::string> GetBinNames() const override;
};

class MapleCombCompiler : public Compiler {
 public:
  explicit MapleCombCompiler(const std::string &name) : Compiler(name), realRunningExe("") {}

  ~MapleCombCompiler() = default;

  ErrorCode Compile(const MplOptions &options, MIRModulePtr &theModule) override;
  void PrintCommand(const MplOptions &options) const override;
  std::string GetInputFileName(const MplOptions &options) const override;

 private:
  std::string realRunningExe;
  std::unordered_set<std::string> GetFinalOutputs(const MplOptions &mplOptions) const override;
  MeOption *MakeMeOptions(const MplOptions &options, MemPool &memPool);
  Options *MakeMpl2MplOptions(const MplOptions &options, MemPool &memPool);
};

class MplcgCompiler : public Compiler {
 public:
  explicit MplcgCompiler(const std::string &name) : Compiler(name) {}

  ~MplcgCompiler() = default;


 private:
  std::string GetInputFileName(const MplOptions &options) const override;
  DefaultOption GetDefaultOptions(const MplOptions &options) const override;
  std::string GetBinName() const override;
  std::vector<std::string> GetBinNames() const override;
};

}  // namespace maple
#endif  // MAPLE_DRIVER_INCLUDE_COMPILER_H
