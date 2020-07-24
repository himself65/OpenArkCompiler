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
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "mir_parser.h"
#include "bin_mplt.h"
#include "opcode_info.h"
#include "mir_function.h"
#include "mir_type.h"

using namespace maple;
#if MIR_FEATURE_FULL
void ConstantFoldModule(MIRModule &module) {
  MapleVector<MIRFunction*> &funcList = module.GetFunctionList();
  for (auto it = funcList.begin(); it != funcList.end(); ++it) {
    MIRFunction *curFunc = *it;
    module.SetCurFunction(curFunc);
  }
}

// hello.mpl
// flavor 1
// srclang 3
// type $person <struct {@age i64,
//                       @data <* f32>}>
// var $BOB i32 = 8
int main(int argc, char **argv) {
  constexpr int judgeNumber = 2;
  if (argc < judgeNumber) {
    (void)MIR_PRINTF(
        "usage: ./irbuild [i|e] <any number of mpl files>\n\n"
        "The optional 'i' flag will convert the binary mplt input file to ascii\n\n"
        "The optional 'e' flag will convert the textual mplt input file to binary\n");
    exit(1);
  }
  char flag = '\0';
  int32 i = 1;
  if (argv[1][0] == 'i' && argv[1][1] == '\0') {
    flag = 'i';
    i = judgeNumber;
  } else if (argv[1][0] == 'e' && argv[1][1] == '\0') {
    flag = 'e';
    i = judgeNumber;
  }
  while (i < argc) {
    MIRModule module{ argv[i] };
    if (flag == '\0') {
      MIRParser theParser(module);
      if (theParser.ParseMIR()) {
        ConstantFoldModule(module);
        module.OutputAsciiMpl(".irb");
      } else {
        theParser.EmitError(module.GetFileName());
        return 1;
      }
    } else if (flag == 'e') {
      MIRParser theParser(module);
      if (theParser.ParseMIR()) {
        ConstantFoldModule(module);
        BinaryMplt binMplt(module);
        const std::string &modID = module.GetFileName();
        binMplt.Export("bin." + modID);
      } else {
        theParser.EmitError(module.GetFileName());
        return 1;
      }
    } else if (flag == 'i') {
      module.SetFlavor(kFeProduced);
      module.SetSrcLang(kSrcLangJava);
      BinaryMplImport binMplt(module);
      binMplt.SetImported(false);
      const std::string &modID = module.GetFileName();
      (void)binMplt.Import(modID, true);
      module.OutputAsciiMpl(".irb");
    }
    ++i;
  }
  return 0;
}
#else
#warning "this module is compiled without MIR_FEATURE_FULL=1 defined"
#endif  // MIR_FEATURE_FULL
