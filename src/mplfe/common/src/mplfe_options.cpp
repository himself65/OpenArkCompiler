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
#include "mplfe_options.h"
#include <iostream>
#include <sstream>
#include "fe_options.h"
#include "fe_macros.h"
#include "option_parser.h"
#include "parser_opt.h"
#include "fe_file_type.h"

namespace maple {
using namespace mapleOption;

enum OptionIndex : uint32 {
  kMplfeHelp = kCommonOptionEnd + 1,
  // input control options
  kInClass,
  kInJar,
  // output control options
  kOutputPath,
  kOutputName,
  kGenMpltOnly,
  kGenAsciiMplt,
  kDumpInstComment,
  // debug info control options
  kDumpLevel,
  kDumpTime,
  kDumpPhaseTime,
  kDumpPhaseTimeDetail,
  // java bytecode compile options
  kJavaStaticFieldName,
  kJBCInfoUsePathName,
  kDumpJBCStmt,
  kDumpJBCBB,
  kDumpJBCAll,
  kDumpJBCErrorOnly,
  kDumpJBCFuncName,
  kEmitJBCLocalVarInfo,
  // general stmt/bb/cfg debug options
  kDumpGenCFGGraph,
  // multi-thread control options
  kNThreads,
  kDumpThreadTime,
  kReleaseAfterEmit,
};

const mapleOption::Descriptor kUsage[] = {
  { static_cast<uint32>(kUnknown), 0, "", "",
    mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyUnknown,
    "========================================\n"
    " Usage: mplfe [ options ] input1 input2 input3\n"
    " options:", "mplfe", {} },
  { static_cast<uint32>(kMplfeHelp), 0, "h", "help",
    mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyNone,
    "  -h, --help             : print usage and exit", "mplfe", {} },
  { static_cast<uint32>(kVersion), 0, "v", "version",
    mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyNone,
    "  -v, --version          : print version and exit", "mplfe", {} },

  // input control options
  { static_cast<uint32>(kUnknown), 0, "", "",
    mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyUnknown,
    "========== Input Control Options ==========", "mplfe", {} },
  { static_cast<uint32>(kInClass), 0, "", "in-class",
    mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyRequired,
    "  --in-class file1.jar,file2.jar\n"
    "                         : input class files", "mplfe", {} },
  { static_cast<uint32>(kInJar), 0, "", "in-jar",
    mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyRequired,
    "  --in-jar file1.jar,file2.jar\n"
    "                         : input jar files", "mplfe", {} },

  // output control options
  { static_cast<uint32>(kUnknown), 0, "", "",
    mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyUnknown,
    "========== Output Control Options ==========", "mplfe", {} },
  { static_cast<uint32>(kOutputPath), 0, "p", "output",
    mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyRequired,
    "  -p, --output            : output path", "mplfe", {} },
  { static_cast<uint32>(kOutputName), 0, "o", "output-name",
    mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyRequired,
    "  -o, --output-name       : output name", "mplfe", {} },
  { static_cast<uint32>(kGenMpltOnly), 0, "t", "",
    mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyNone,
    "  -t                     : generate mplt only", "mplfe", {} },
  { static_cast<uint32>(kGenAsciiMplt), 0, "", "asciimplt",
    mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyNone,
    "  --asciimplt            : generate mplt in ascii format", "mplfe", {} },
  { static_cast<uint32>(kDumpInstComment), 0, "", "dump-inst-comment",
    mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyNone,
    "  --dump-inst-comment    : dump instruction comment", "mplfe", {} },

  // debug info control options
  { static_cast<uint32>(kUnknown), 0, "", "",
    mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyUnknown,
    "========== Debug Info Control Options ==========", "mplfe", {} },
  { static_cast<uint32>(kDumpLevel), 0, "d", "dump-level",
    mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyNumeric,
    "  -d, --dump-level xx    : debug info dump level\n"
    "                             [0] disable\n"
    "                             [1] dump simple info\n"
    "                             [2] dump detail info\n"
    "                             [3] dump debug info", "mplfe", {} },
  { static_cast<uint32>(kDumpTime), 0, "", "dump-time",
    mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyNone,
    "  --dump-time            : dump time", "mplfe", {} },
  { static_cast<uint32>(kDumpPhaseTime), 0, "", "dump-phase-time",
    mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyNone,
    "  --dump-phase-time      : dump total phase time", "mplfe", {} },
  { static_cast<uint32>(kDumpPhaseTimeDetail), 0, "", "dump-phase-time-detail",
    mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyNone,
    "  --dump-phase-time-detail"
    "                         : dump phase time for each method", "mplfe", {} },
  // java bytecode compile options
  { static_cast<uint32>(kUnknown), 0, "", "",
    mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyUnknown,
    "========== Java Bytecode Compile Options ==========", "mplfe", {} },
  { static_cast<uint32>(kJavaStaticFieldName), 0, "", "java-staticfield-name",
    mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyRequired,
    "  --java-staticfield-name\n"
    "                         : java static field name mode\n"
    "                             [notype]  all static fields have no types in names\n"
    "                             [alltype] all static fields have types in names\n"
    "                             [smart]   only static fields in field-proguard class have types in names\n",
    "mplfe", {} },
  { static_cast<uint32>(kJBCInfoUsePathName), 0, "", "jbc-info-use-pathname",
    mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyNone,
    "  --jbc-info-use-pathname\n"
    "                         : use JBC pathname in file info", "mplfe", {} },
  { static_cast<uint32>(kDumpJBCStmt), 0, "", "dump-jbc-stmt",
    mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyNone,
    "  --dump-jbc-stmt        : dump JBC Stmt", "mplfe", {} },
  { static_cast<uint32>(kDumpJBCBB), 0, "", "dump-jbc-bb",
    mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyNone,
    "  --dump-jbc-bb          : dump JBC BB", "mplfe", {} },
  { static_cast<uint32>(kDumpJBCAll), 0, "", "dump-jbc-all",
    mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyNone,
    "  --dump-jbc-all         : dump all JBC function", "mplfe", {} },
  { static_cast<uint32>(kDumpJBCErrorOnly), 0, "", "dump-jbc-error-only",
    mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyNone,
    "  --dump-jbc-error-only\n"
    "                         : dump JBC functions with errors", "mplfe", {} },
  { static_cast<uint32>(kDumpJBCFuncName), 0, "", "dump-jbc-funcname",
    mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyRequired,
    "  --dump-jbc-funcname=name1,name2,...\n"
    "                         : dump JBC functions with specified names\n"
    "                         : name format: ClassName;|MethodName|Signature", "mplfe", {} },
  // general stmt/bb/cfg debug options
  { static_cast<uint32>(kDumpGenCFGGraph), 0, "", "dump-general-cfg-graph",
    mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyRequired,
    "  --dump-general-cfg-graph=graph.dot\n"
    "                         : dump General CFG into graphviz dot file", "mplfe", {} },
  { static_cast<uint32>(kEmitJBCLocalVarInfo), 0, "", "emit-jbc-localvar-info",
    mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyNone,
    "  --emit-jbc-localvar-info\n"
    "                         : emit jbc's LocalVar Info in mpl using comments", "mplfe", {} },
  // multi-thread control
  { static_cast<uint32>(kUnknown), 0, "", "",
    mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyUnknown,
    "========== Multi-Thread Control Options ==========", "mplfe", {} },
  { static_cast<uint32>(kNThreads), 0, "", "np",
    mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyRequired,
    "  --np num               : number of threads", "mplfe", {} },
  { static_cast<uint32>(kDumpThreadTime), 0, "", "dump-thread-time",
    mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyNone,
    "  --dump-thread-time     : dump thread time in mpl schedular", "mplfe", {} },
  { static_cast<uint32>(kReleaseAfterEmit), 0, "", "release-after-emit",
    mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyNone,
    "  --release-after-emit   : release temp memory after emit", "mplfe", {} },
  { 0, 0, nullptr, nullptr, mapleOption::kBuildTypeAll, mapleOption::kArgCheckPolicyNone, nullptr, "mplfe", {} }
};

MPLFEOptions MPLFEOptions::options;

MPLFEOptions::MPLFEOptions() {
  CreateUsages(kUsage);
  Init();
}

void MPLFEOptions::Init() {
  FEOptions::GetInstance().Init();
  bool success = InitFactory();
  CHECK_FATAL(success, "InitFactory failed. Exit.");
}

bool MPLFEOptions::InitFactory() {
  RegisterFactoryFunction<OptionProcessFactory>(static_cast<uint32>(kMplfeHelp),
                                                &MPLFEOptions::ProcessHelp);
  RegisterFactoryFunction<OptionProcessFactory>(static_cast<uint32>(kVersion),
                                                &MPLFEOptions::ProcessVersion);

  // input control options
  RegisterFactoryFunction<OptionProcessFactory>(static_cast<uint32>(kInClass),
                                                &MPLFEOptions::ProcessInClass);
  RegisterFactoryFunction<OptionProcessFactory>(static_cast<uint32>(kInJar),
                                                &MPLFEOptions::ProcessInJar);

  // output control options
  RegisterFactoryFunction<OptionProcessFactory>(static_cast<uint32>(kOutputPath),
                                                &MPLFEOptions::ProcessOutputPath);
  RegisterFactoryFunction<OptionProcessFactory>(static_cast<uint32>(kOutputName),
                                                &MPLFEOptions::ProcessOutputName);
  RegisterFactoryFunction<OptionProcessFactory>(static_cast<uint32>(kGenMpltOnly),
                                                &MPLFEOptions::ProcessGenMpltOnly);
  RegisterFactoryFunction<OptionProcessFactory>(static_cast<uint32>(kGenAsciiMplt),
                                                &MPLFEOptions::ProcessGenAsciiMplt);
  RegisterFactoryFunction<OptionProcessFactory>(static_cast<uint32>(kDumpInstComment),
                                                &MPLFEOptions::ProcessDumpInstComment);

  // debug info control options
  RegisterFactoryFunction<OptionProcessFactory>(static_cast<uint32>(kDumpLevel),
                                                &MPLFEOptions::ProcessDumpLevel);
  RegisterFactoryFunction<OptionProcessFactory>(static_cast<uint32>(kDumpTime),
                                                &MPLFEOptions::ProcessDumpTime);
  RegisterFactoryFunction<OptionProcessFactory>(static_cast<uint32>(kDumpPhaseTime),
                                                &MPLFEOptions::ProcessDumpPhaseTime);
  RegisterFactoryFunction<OptionProcessFactory>(static_cast<uint32>(kDumpPhaseTimeDetail),
                                                &MPLFEOptions::ProcessDumpPhaseTimeDetail);

  // java bytecode compile options
  RegisterFactoryFunction<OptionProcessFactory>(static_cast<uint32>(kJavaStaticFieldName),
                                                &MPLFEOptions::ProcessModeForJavaStaticFieldName);
  RegisterFactoryFunction<OptionProcessFactory>(static_cast<uint32>(kJBCInfoUsePathName),
                                                &MPLFEOptions::ProcessJBCInfoUsePathName);
  RegisterFactoryFunction<OptionProcessFactory>(static_cast<uint32>(kDumpJBCStmt),
                                                &MPLFEOptions::ProcessDumpJBCStmt);
  RegisterFactoryFunction<OptionProcessFactory>(static_cast<uint32>(kDumpJBCBB),
                                                &MPLFEOptions::ProcessDumpJBCBB);
  RegisterFactoryFunction<OptionProcessFactory>(static_cast<uint32>(kDumpJBCErrorOnly),
                                                &MPLFEOptions::ProcessDumpJBCErrorOnly);
  RegisterFactoryFunction<OptionProcessFactory>(static_cast<uint32>(kDumpJBCFuncName),
                                                &MPLFEOptions::ProcessDumpJBCFuncName);
  RegisterFactoryFunction<OptionProcessFactory>(static_cast<uint32>(kEmitJBCLocalVarInfo),
                                                &MPLFEOptions::ProcessEmitJBCLocalVarInfo);

  // general stmt/bb/cfg debug options
  RegisterFactoryFunction<OptionProcessFactory>(static_cast<uint32>(kDumpGenCFGGraph),
                                                &MPLFEOptions::ProcessDumpGeneralCFGGraph);

  // multi-thread control options
  RegisterFactoryFunction<OptionProcessFactory>(static_cast<uint32>(kNThreads),
                                                &MPLFEOptions::ProcessNThreads);
  RegisterFactoryFunction<OptionProcessFactory>(static_cast<uint32>(kDumpThreadTime),
                                                &MPLFEOptions::ProcessDumpThreadTime);
  RegisterFactoryFunction<OptionProcessFactory>(static_cast<uint32>(kReleaseAfterEmit),
                                                &MPLFEOptions::ProcessReleaseAfterEmit);
  return true;
}

bool MPLFEOptions::SolveOptions(const std::vector<mapleOption::Option> &opts, bool isDebug) {
  for (const mapleOption::Option &opt : opts) {
    if (isDebug) {
      LogInfo::MapleLogger() << "mplfe options: " << opt.Index() << " " << opt.OptionKey() << " " <<
                                opt.Args() << '\n';
    }
    auto func = CreateProductFunction<OptionProcessFactory>(opt.Index());
    if (func != nullptr) {
      if (!func(this, opt)) {
        return false;
      }
    }
  }
  return true;
}

bool MPLFEOptions::SolveArgs(int argc, char **argv) {
  OptionParser optionParser;
  optionParser.RegisteUsages(DriverOptionCommon::GetInstance());
  optionParser.RegisteUsages(MPLFEOptions::GetInstance());
  if (argc == 1) {
    DumpUsage();
    return false;
  }
  ErrorCode ret = optionParser.Parse(argc, argv, "mplfe");
  if (ret != ErrorCode::kErrorNoError) {
    DumpUsage();
    return false;
  }

  bool result = SolveOptions(optionParser.GetOptions(), false);
  if (!result) {
    return result;
  }

  if (optionParser.GetNonOptionsCount() >= 1) {
    const std::vector<std::string> &inputs = optionParser.GetNonOptions();
    ProcessInputFiles(inputs);
  }
  return true;
}

void MPLFEOptions::DumpUsage() const {
  for (unsigned int i = 0; kUsage[i].help != nullptr; i++) {
    std::cout << kUsage[i].help << std::endl;
  }
}

void MPLFEOptions::DumpVersion() const {
  std::cout << "Version: " << std::endl;
}

bool MPLFEOptions::ProcessHelp(const mapleOption::Option &opt) {
  DumpUsage();
  return false;
}

bool MPLFEOptions::ProcessVersion(const mapleOption::Option &opt) {
  DumpVersion();
  return false;
}

bool MPLFEOptions::ProcessInClass(const mapleOption::Option &opt) {
  std::list<std::string> listFiles = SplitByComma(opt.Args());
  for (const std::string &fileName : listFiles) {
    FEOptions::GetInstance().AddInputClassFile(fileName);
  }
  return true;
}

bool MPLFEOptions::ProcessInJar(const mapleOption::Option &opt) {
  std::list<std::string> listFiles = SplitByComma(opt.Args());
  for (const std::string &fileName : listFiles) {
    FEOptions::GetInstance().AddInputJarFile(fileName);
  }
  return true;
}


bool MPLFEOptions::ProcessInputMplt(const mapleOption::Option &opt) {
  std::list<std::string> listFiles = SplitByComma(opt.Args());
  for (const std::string &fileName : listFiles) {
    FEOptions::GetInstance().AddInputMpltFile(fileName);
  }
  return true;
}

bool MPLFEOptions::ProcessInputMpltFromSys(const mapleOption::Option &opt) {
  std::list<std::string> listFiles = SplitByComma(opt.Args());
  for (const std::string &fileName : listFiles) {
    FEOptions::GetInstance().AddInputMpltFileFromSys(fileName);
  }
  return true;
}

bool MPLFEOptions::ProcessInputMpltFromApk(const mapleOption::Option &opt) {
  std::list<std::string> listFiles = SplitByComma(opt.Args());
  for (const std::string &fileName : listFiles) {
    FEOptions::GetInstance().AddInputMpltFileFromApk(fileName);
  }
  return true;
}

bool MPLFEOptions::ProcessOutputPath(const mapleOption::Option &opt) {
  FEOptions::GetInstance().SetOutputPath(opt.Args());
  return true;
}

bool MPLFEOptions::ProcessOutputName(const mapleOption::Option &opt) {
  FEOptions::GetInstance().SetOutputName(opt.Args());
  return true;
}

bool MPLFEOptions::ProcessGenMpltOnly(const mapleOption::Option &opt) {
  FEOptions::GetInstance().SetIsGenMpltOnly(true);
  return true;
}

bool MPLFEOptions::ProcessGenAsciiMplt(const mapleOption::Option &opt) {
  FEOptions::GetInstance().SetIsGenAsciiMplt(true);
  return true;
}

bool MPLFEOptions::ProcessDumpInstComment(const mapleOption::Option &opt) {
  FEOptions::GetInstance().EnableDumpInstComment();
  return true;
}

bool MPLFEOptions::ProcessDumpLevel(const mapleOption::Option &opt) {
  FEOptions::GetInstance().SetDumpLevel(std::stoi(opt.Args()));
  return true;
}

bool MPLFEOptions::ProcessDumpTime(const mapleOption::Option &opt) {
  FEOptions::GetInstance().SetIsDumpTime(true);
  return true;
}

bool MPLFEOptions::ProcessDumpPhaseTime(const mapleOption::Option &opt) {
  FEOptions::GetInstance().SetIsDumpPhaseTime(true);
  return true;
}

bool MPLFEOptions::ProcessDumpPhaseTimeDetail(const mapleOption::Option &opt) {
  FEOptions::GetInstance().SetIsDumpPhaseTimeDetail(true);
  return true;
}

// java compiler options
bool MPLFEOptions::ProcessModeForJavaStaticFieldName(const mapleOption::Option &opt) {
  std::string arg = opt.Args();
  if (arg.compare("notype") == 0) {
    FEOptions::GetInstance().SetModeJavaStaticFieldName(FEOptions::ModeJavaStaticFieldName::kNoType);
  } else if (arg.compare("alltype") == 0) {
    FEOptions::GetInstance().SetModeJavaStaticFieldName(FEOptions::ModeJavaStaticFieldName::kAllType);
  } else if (arg.compare("smart") == 0) {
    FEOptions::GetInstance().SetModeJavaStaticFieldName(FEOptions::ModeJavaStaticFieldName::kSmart);
  } else {
    ERR(kLncErr, "unsupported options: %s", arg.c_str());
    return false;
  }
  return true;
}

bool MPLFEOptions::ProcessJBCInfoUsePathName(const mapleOption::Option &opt) {
  FEOptions::GetInstance().SetIsJBCInfoUsePathName(true);
  return true;
}

bool MPLFEOptions::ProcessDumpJBCStmt(const mapleOption::Option &opt) {
  FEOptions::GetInstance().SetIsDumpJBCStmt(true);
  return true;
}

bool MPLFEOptions::ProcessDumpJBCBB(const mapleOption::Option &opt) {
  FEOptions::GetInstance().SetIsDumpJBCBB(true);
  return true;
}

bool MPLFEOptions::ProcessDumpJBCAll(const mapleOption::Option &opt) {
  FEOptions::GetInstance().SetIsDumpJBCAll(true);
  return true;
}

bool MPLFEOptions::ProcessDumpJBCErrorOnly(const mapleOption::Option &opt) {
  FEOptions::GetInstance().SetIsDumpJBCErrorOnly(true);
  return true;
}

bool MPLFEOptions::ProcessDumpJBCFuncName(const mapleOption::Option &opt) {
  std::string arg = opt.Args();
  while (!arg.empty()) {
    size_t pos = arg.find(",");
    if (pos != std::string::npos) {
      FEOptions::GetInstance().AddDumpJBCFuncName(arg.substr(0, pos));
      arg = arg.substr(pos + 1);
    } else {
      FEOptions::GetInstance().AddDumpJBCFuncName(arg);
      arg = "";
    }
  }
  return true;
}

bool MPLFEOptions::ProcessEmitJBCLocalVarInfo(const mapleOption::Option &opt) {
  FEOptions::GetInstance().SetIsEmitJBCLocalVarInfo(true);
  return true;
}

// general stmt/bb/cfg debug options
bool MPLFEOptions::ProcessDumpGeneralCFGGraph(const mapleOption::Option &opt) {
  FEOptions::GetInstance().SetIsDumpGeneralCFGGraph(true);
  FEOptions::GetInstance().SetGeneralCFGGraphFileName(opt.Args());
  return true;
}

// multi-thread control options
bool MPLFEOptions::ProcessNThreads(const mapleOption::Option &opt) {
  std::string arg = opt.Args();
  int np = std::stoi(arg);
  if (np > 0) {
    FEOptions::GetInstance().SetNThreads(static_cast<uint32>(np));
  }
  return true;
}

bool MPLFEOptions::ProcessDumpThreadTime(const mapleOption::Option &opt) {
  FEOptions::GetInstance().SetDumpThreadTime(true);
  return true;
}

bool MPLFEOptions::ProcessReleaseAfterEmit(const mapleOption::Option &opt) {
  FEOptions::GetInstance().SetReleaseAfterEmit(true);
  return true;
}

void MPLFEOptions::ProcessInputFiles(const std::vector<std::string> &inputs) {
  FE_INFO_LEVEL(FEOptions::kDumpLevelInfo, "===== Process MPLFEOptions::ProcessInputFiles() =====");
  for (const std::string &inputName : inputs) {
    FEFileType::FileType type = FEFileType::GetInstance().GetFileTypeByPathName(inputName);
    switch (type) {
      case FEFileType::kClass:
        FE_INFO_LEVEL(FEOptions::kDumpLevelInfoDetail, "CLASS file detected: %s", inputName.c_str());
        FEOptions::GetInstance().AddInputClassFile(inputName);
        break;
      case FEFileType::kJar:
        FE_INFO_LEVEL(FEOptions::kDumpLevelInfoDetail, "JAR file detected: %s", inputName.c_str());
        FEOptions::GetInstance().AddInputJarFile(inputName);
        break;
      default:
        WARN(kLncErr, "unsupported file format (%s)", inputName.c_str());
        break;
    }
  }
}

template <typename Out>
void MPLFEOptions::Split(const std::string &s, char delim, Out result) {
  std::stringstream ss;
  ss.str(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    *(result++) = item;
  }
}

std::list<std::string> MPLFEOptions::SplitByComma(const std::string &s) {
  std::list<std::string> results;
  MPLFEOptions::Split(s, ',', std::back_inserter(results));
  return results;
}
}  // namespace maple
