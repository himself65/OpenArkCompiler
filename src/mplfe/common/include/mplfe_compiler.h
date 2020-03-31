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
#ifndef MPLFE_INCLUDE_COMMON_MPLFE_COMPILER_NEW_H
#define MPLFE_INCLUDE_COMMON_MPLFE_COMPILER_NEW_H
#include <memory>
#include <list>
#include "mpl_logging.h"
#include "fe_macros.h"
#include "mplfe_compiler_component.h"

namespace maple {
class MPLFECompiler {
 public:
  explicit MPLFECompiler(MIRModule &argModule);
  ~MPLFECompiler();
  // common process
  void Init();
  void Release();
  void CheckInput();
  void SetupOutputPathAndName();
  bool LoadMplt();
  void ExportMpltFile();
  void ExportMplFile();

  // component process
  void RegisterCompilerComponent(std::unique_ptr<MPLFECompilerComponent> comp);
  void InitFromOptions();
  void ParseInputs();
  void PreProcessDecls();
  void ProcessDecls();
  void PreProcessWithFunctions();
  void ProcessFunctions();

 private:
  MIRModule &module;
  MemPool *mp;
  MapleAllocator allocator;
  std::string firstInputName;
  std::string selfMpltName;
  std::string outputPath;
  std::string outputName;
  std::list<std::unique_ptr<MPLFECompilerComponent>> components;
};
}  // namespace maple
#endif  // MPLFE_INCLUDE_COMMON_MPLFE_COMPILER_NEW_H