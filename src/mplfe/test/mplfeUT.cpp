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
#include <gtest/gtest.h>
#include <iostream>
#include "mpl_logging.h"
#include "mir_module.h"
#include "mplfe_ut_options.h"
#include "basic_io.h"
#include "base64.h"
#include "jbc_input.h"
#include "fe_manager.h"
#include "fe_file_type.h"
#include "fe_type_hierarchy.h"
#include "mplfe_ut_environment.h"

using namespace maple;
void GenBase64ForFile(const std::string &fileName) {
  BasicIOMapFile file(fileName);
  if (!file.OpenAndMap()) {
    FATAL(kLncFatal, "Can not open the input file %s for base64 convert.", fileName.c_str());
  }
  std::string base64Str = Base64::Encode(file.GetPtr(), file.GetLength());
  size_t length = base64Str.length();
  size_t start = 0;
  const size_t width = 60;
  while (start < length) {
    std::cout << "\"" << base64Str.substr(start, width) << "\"" << std::endl;
    start += width;
  }
  file.Close();
}

int main(int argc, char **argv) {
  ::testing::AddGlobalTestEnvironment(new MPLFEUTEnvironment);
  FEManager::Init(MPLFEUTEnvironment::GetMIRModule());
  MPLFEUTOptions options;
  if (!options.SolveArgs(argc, argv)) {
    return 0;
  }
  if (options.GetRunAll()) {
    if (argc > 1) {
      argc--;
      argv++;
    }
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
  }
  if (options.GetRunAllWithCore()) {
    CHECK_FATAL(argc > 2, "In RunAllWithCore mode, argc must larger than 2");
    argc -= 2;
    argv += 2;
    FEManager::GetTypeManager().LoadMplt(options.GetCoreMpltName(), FETypeFlag::kSrcMpltSys);
    FETypeHierarchy::GetInstance().InitByGlobalTable();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
  }
  MPLFEUTEnvironment::GetMIRModule().SetFlavor(maple::kFeProduced);
  MPLFEUTEnvironment::GetMIRModule().SetSrcLang(maple::kSrcLangJava);
  if (options.GetGenBase64()) {
    GenBase64ForFile(options.GetBase64SrcFileName());
  }
  jbc::JBCInput jbcInput(MPLFEUTEnvironment::GetMIRModule());
  if (options.GetClassFileList().size() > 0) {
    jbcInput.ReadClassFiles(options.GetClassFileList());
  }
  if (options.GetJarFileList().size() > 0) {
    jbcInput.ReadJarFiles(options.GetJarFileList());
  }
  if (options.GetMpltFileList().size() > 0) {
    if (FEManager::GetTypeManager().LoadMplts(options.GetMpltFileList(), FETypeFlag::kSrcMplt, "Load mplt")) {
      INFO(kLncInfo, "Load mplt...success");
    } else {
      INFO(kLncInfo, "Load mplt...failed");
    }
  }
  return 0;
}
