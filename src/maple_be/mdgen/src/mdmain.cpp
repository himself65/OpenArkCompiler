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
#include <getopt.h>
#include "mdparser.h"
#include "mdgenerator.h"

using namespace MDGen;
namespace {
  bool isGenSched = false;
  std::string schedSrcPath = "";
  std::string oFileDir = "";
}

static int PrintHelpAndExit() {
  maple::LogInfo::MapleLogger() << "Maplegen is usd to process md files and " <<
                            "generate architecture specific information in def files\n" <<
                            "usage: ./mplgen xxx.md outputdirectroy\n";
  return 1;
}

void ParseCommandLine(int argc, char **argv) {
  int opt;
  int gOptionIndex = 0;
  std::string optStr = "s:o:";
  static struct option longOptions[] = {
      {"genSchdInfo", required_argument, NULL, 's'},
      {"outDirectory", required_argument, NULL, 'o'},
      {0, 0, 0, 0}
  };
  while ((opt = getopt_long(argc, argv, optStr.c_str(), longOptions, &gOptionIndex)) != -1) {
    switch (opt) {
      case 's':
        isGenSched = true;
        schedSrcPath = optarg;
        break;
      case 'o':
        oFileDir = optarg;
        break;
      default:
        break;
    }
  }
}

bool GenSchedFiles(const std::string &fileName, const std::string &oFileDir, maple::MemPoolCtrler &mdMpCtrler) {
  maple::MemPool *schedInfoMemPool = mdMpCtrler.NewMemPool("schedInfoMp");
  MDClassRange moduleData("Schedule");
  MDParser parser(moduleData, schedInfoMemPool);
  if (!parser.ParseFile(fileName)) {
    mdMpCtrler.DeleteMemPool(schedInfoMemPool);
    return false;
  }
  SchedInfoGen schedEmiiter(moduleData, oFileDir);
  schedEmiiter.Run();
  mdMpCtrler.DeleteMemPool(schedInfoMemPool);
  return true;
}

int main(int argc, char **argv) {
  constexpr int minimumArgNum = 2;
  if (argc <= minimumArgNum) {
    return PrintHelpAndExit();
  }
  ParseCommandLine(argc, argv);
  maple::MemPoolCtrler mdMpCtrler;
  if (isGenSched) {
    if (!GenSchedFiles(schedSrcPath, oFileDir, mdMpCtrler)) {
      return 1;
    }
  }
  return 0;
}
