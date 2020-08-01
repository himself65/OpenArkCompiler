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
#include "mplfe_ut_options.h"
#include <iostream>
#include "mpl_logging.h"
#include "option_parser.h"
#include "parser_opt.h"

namespace maple {
using namespace mapleOption;

enum OptionIndex {
  kMplfeUTHelp = kCommonOptionEnd + 1,   // -h
  kGenBase64,                            // -genBase64
  kInClass,                              // -in-class
  kInJar,                                // -in-jar
  kMplt,                                 // -mplt
};

const Descriptor kUsage[] = {
  { kUnknown, 0, "", "", kBuildTypeAll, kArgCheckPolicyUnknown,
    "========================================\n"
    " Run gtest: mplfeUT\n"
    " Run gtest: mplfeUT test [ options for gtest ]\n"
    " Run ext mode: mplfeUT ext [ options ]\n"
    "========= options for ext mode =========" },
  { kHelp, 0, "h", "help", kBuildTypeAll, kArgCheckPolicyNone,
    "  -h, --help                : print usage and exit", "mplfeUT", {} },
  { kGenBase64, 0, "", "gen-base64", kBuildTypeAll, kArgCheckPolicyRequired,
    "  -gen-base64 file.xx       : generate base64 string for file.xx", "mplfeUT", {} },
  { kInClass, 0, "", "in-class", kBuildTypeAll, kArgCheckPolicyRequired,
    "  -in-class file1.class,file2.class\n"
    "                            : input class files", "mplfeUT", {} },
  { kInJar, 0, "", "in-jar", kBuildTypeAll, kArgCheckPolicyRequired,
    "  -in-jar file1.jar,file2.jar\n"
    "                            : input jar files", "mplfeUT", {} },
  { kMplt, 0, "", "mplt", kBuildTypeAll, kArgCheckPolicyRequired,
    "  -mplt lib1.mplt,lib2.mplt\n"
    "                            : input mplt files", "mplfeUT", {} },
  { kUnknown, 0, "", "", kBuildTypeAll, kArgCheckPolicyNone, "", "mplfeUT", {} }
};

MPLFEUTOptions::MPLFEUTOptions()
    : runAll(false),
      runAllWithCore(false),
      genBase64(false),
      base64SrcFileName(""),
      coreMpltName("") {
        CreateUsages(kUsage);
      }

void MPLFEUTOptions::DumpUsage() const {
  for (unsigned int i = 0; kUsage[i].help != ""; i++) {
    std::cout << kUsage[i].help << std::endl;
  }
  exit(1);
}

bool MPLFEUTOptions::SolveArgs(int argc, char **argv) {
  if (argc == 1) {
    runAll = true;
    return true;
  }
  if (std::string(argv[1]).compare("test") == 0) {
    runAll = true;
    return true;
  }
  if (std::string(argv[1]).compare("testWithMplt") == 0) {
    runAllWithCore = true;
    CHECK_FATAL(argc > 2, "In TestWithMplt mode, core.mplt must be specified");
    coreMpltName = argv[2];
    return true;
  }
  if (std::string(argv[1]).compare("ext") != 0) {
    FATAL(kLncFatal, "Undefined mode");
    return false;
  }
  runAll = false;
  OptionParser optionParser;
  optionParser.RegisteUsages(DriverOptionCommon::GetInstance());
  optionParser.RegisteUsages(MPLFEUTOptions::GetInstance());

  ErrorCode ret = optionParser.Parse(argc, argv, "mplfeUT");
  if (ret != ErrorCode::kErrorNoError) {
    DumpUsage();
    return false;
  }

  for (auto opt : optionParser.GetOptions()) {
    switch (opt.Index()) {
      case kMplfeUTHelp:
        DumpUsage();
        return false;
      case kGenBase64:
        base64SrcFileName = opt.Args();
        genBase64 = true;
        break;
      case kInClass:
        Split(opt.Args(), ',', std::back_inserter(classFileList));
        break;
      case kInJar:
        Split(opt.Args(), ',', std::back_inserter(jarFileList));
        break;
      case kMplt:
        Split(opt.Args(), ',', std::back_inserter(mpltFileList));
        break;
      default:
        FATAL(kLncFatal, "Unsupport option %s", opt.OptionKey().c_str());
        DumpUsage();
        return false;
    }
  }
  return true;
}

template <typename Out>
void MPLFEUTOptions::Split(const std::string &s, char delim, Out result) {
  std::stringstream ss;
  ss.str(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    *(result++) = item;
  }
}
}  // namespace maple