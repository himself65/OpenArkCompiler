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
#include "driver_option_common.h"

namespace {
using namespace maple;
using namespace mapleOption;
const mapleOption::Descriptor usages[] = {
  // index, type , shortOption , longOption, enableBuildType, checkPolicy, help, exeName, extra bins
  { kUnknown,
    0,
    nullptr,
    nullptr,
    kBuildTypeAll,
    kArgCheckPolicyNone,
    "USAGE: maple [options]\n\n"
    "  Example 1: <Maple bin path>/maple --run=me:mpl2mpl:mplcg --option=\"[MEOPT]:[MPL2MPLOPT]:[MPLCGOPT]\"\n"
    "                                    --mplt=MPLTPATH inputFile.mpl\n"
    "  Example 2: <Maple bin path>/maple -O2 --mplt=mpltPath inputFile.dex\n\n"
    "==============================\n"
    "  Options:\n",
    "all",
    {} },
  { kHelp,
    0,
    "h",
    "help",
    kBuildTypeProduct,
    kArgCheckPolicyOptional,
    "  -h --help [COMPILERNAME]    \tPrint usage and exit.\n"
    "                              \tTo print the help info of specified compiler,\n"
    "                              \tyou can use jbc2mpl, me, mpl2mpl, mplcg... as the COMPILERNAME\n"
    "                              \teg: --help=mpl2mpl\n",
    "all",
    {} },
  { kVersion,
    0,
    nullptr,
    "version",
    kBuildTypeProduct,
    kArgCheckPolicyOptional,
    "  --version [command]         \tPrint version and exit.\n",
    "all",
    {} },
  { kInFile,
    0,
    nullptr,
    "infile",
    kBuildTypeProduct,
    kArgCheckPolicyRequired,
    "  --infile file1,file2,file3  \tInput files.\n",
    "all",
    {} },
  { kInMplt,
    0,
    nullptr,
    "mplt",
    kBuildTypeProduct,
    kArgCheckPolicyRequired,
    "  --mplt=file1,file2,file3    \tImport mplt files.\n",
    "all",
    { "jbc2mpl",
    } },
  { kOptimization0,
    0,
    "O0",
    nullptr,
    kBuildTypeProduct,
    kArgCheckPolicyNone,
    "  -O0                         \tNo optimization.\n",
    "all",
    {} },
  { kOptimization2,
    0,
    "O2",
    nullptr,
    kBuildTypeProduct,
    kArgCheckPolicyNone,
    "  -O2                         \tDo more optimization. (Default)\n",
    "all",
    {} },
  { kGCOnly,
    kEnable,
    nullptr,
    "gconly",
    kBuildTypeProduct,
    kArgCheckPolicyBool,
    "  --gconly                     \tMake gconly is enable\n"
    "  --no-gconly                  \tDon't make gconly is enable\n",
    "all",
    {
      "mplcg"
    } },
  { kMeOpt,
    0,
    nullptr,
    "me-opt",
    kBuildTypeProduct,
    kArgCheckPolicyNone,
    "  --me-opt                    \tSet options for me\n",
    "all",
    {} },
  { kMpl2MplOpt,
    0,
    nullptr,
    "mpl2mpl-opt",
    kBuildTypeProduct,
    kArgCheckPolicyNone,
    "  --mpl2mpl-opt               \tSet options for mpl2mpl\n",
    "all",
    {} },
  { kSaveTemps,
    0,
    nullptr,
    "save-temps",
    kBuildTypeProduct,
    kArgCheckPolicyNone,
    "  --save-temps                \tDo not delete intermediate files.\n"
    "                              \t--save-temps Save all intermediate files.\n"
    "                              \t--save-temps=file1,file2,file3 Save the\n"
    "                              \ttarget files.\n",
    "all",
    {} },
  { kRun,
    0,
    nullptr,
    "run",
    kBuildTypeProduct,
    kArgCheckPolicyRequired,
    "  --run=cmd1:cmd2             \tThe name of executables that are going\n"
    "                              \tto execute. IN SEQUENCE.\n"
    "                              \tSeparated by \":\".Available exe names:\n"
    "                              \tjbc2mpl, me, mpl2mpl, mplcg\n"
    "                              \tInput file must match the tool can\n"
    "                              \thandle\n",
    "all",
    {} },
  { kOption,
    0,
    nullptr,
    "option",
    kBuildTypeProduct,
    kArgCheckPolicyRequired,
    "  --option=\"opt1:opt2\"      \tOptions for each executable,\n"
    "                              \tseparated by \":\".\n"
    "                              \tThe sequence must match the sequence in\n"
    "                              \t--run.\n",
    "all",
    {} },
  { kTimePhases,
    0,
    "time-phases",
    nullptr,
    kBuildTypeExperimental,
    kArgCheckPolicyNone,
    "  -time-phases                \tTiming phases and print percentages\n",
    "all",
    {} },
  { kGenMeMpl,
    0,
    nullptr,
    "genmempl",
    kBuildTypeProduct,
    kArgCheckPolicyNone,
    "  --genmempl                  \tGenerate me.mpl file\n",
    "all",
    {} },
  { kGenVtableImpl,
    0,
    nullptr,
    "genVtableImpl",
    kBuildTypeProduct,
    kArgCheckPolicyNone,
    "  --genVtableImpl             \tGenerate VtableImpl.mpl file\n",
    "all",
    {} },
  { kVerbose,
    kEnable,
    nullptr,
    "verbose",
    kBuildTypeDebug,
    kArgCheckPolicyBool,
    "  -verbose                    \t: print informations\n",
    "all",
    { "jbc2mpl", "me", "mpl2mpl", "mplcg" } },
  { kAllDebug,
    0,
    nullptr,
    "debug",
    kBuildTypeDebug,
    kArgCheckPolicyNone,
    "  --debug                     \tPrint debug info.\n",
    "all",
    {} },
  { kHelpLevel,
    0,
    nullptr,
    "level",
    kBuildTypeProduct,
    kArgCheckPolicyNumeric,
    "  --level=NUM                 \tPrint the help info of specified level.\n"
    "                              \tNUM=0: All options (Default)\n"
    "                              \tNUM=1: Product options\n"
    "                              \tNUM=2: Experimental options\n"
    "                              \tNUM=3: Debug options\n",
    "all",
    {} },
  { kUnknown,
    0,
    nullptr,
    nullptr,
    kBuildTypeAll,
    kArgCheckPolicyNone,
    nullptr,
    "all",
    {} }
};
}  // namepsace

namespace maple {
using namespace mapleOption;
DriverOptionCommon &DriverOptionCommon::GetInstance() {
  static DriverOptionCommon instance;
  return instance;
}

DriverOptionCommon::DriverOptionCommon() {
  CreateUsages(usages);
}
}  // namespace maple