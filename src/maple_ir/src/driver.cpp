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
void ConstantFoldModule(maple::MIRModule *module) {
  MapleVector<maple::MIRFunction*> &funcList = module->GetFunctionList();
  for (auto it = funcList.begin(); it != funcList.end(); it++) {
    maple::MIRFunction *curfun = *it;
    module->SetCurFunction(curfun);
  }
}

int main(int argc, char **argv) {
  if (argc < 2) {
    MIR_PRINTF(
        "usage: ./irbuild [i|e] <any number of mpl files>\n\n"
        "The optional 'i' flag will convert the binary mplt input file to ascii\n\n"
        "The optional 'e' flag will convert the textual mplt input file to binary\n");
    exit(1);
  }
  char flag = '\0';
  maple::int32 i = 1;
  if (argv[1][0] == 'i' && argv[1][1] == '\0') {
    flag = 'i';
    i = 2;
  } else if (argv[1][0] == 'e' && argv[1][1] == '\0') {
    flag = 'e';
    i = 2;
  }
  while (i < argc) {
    maple::MIRModule module{ argv[i] };
    if (flag == '\0') {
      maple::MIRParser theparser(module);
      if (theparser.ParseMIR()) {
        ConstantFoldModule(&module);
        module.OutputAsciiMpl(".irb");
      } else {
        theparser.EmitError(module.GetFileName().c_str());
        return 1;
      }
    } else if (flag == 'e') {
      maple::MIRParser theparser(module);
      if (theparser.ParseMIR()) {
        ConstantFoldModule(&module);
        BinaryMplt binmplt(module);
        std::string modid = module.GetFileName();
        binmplt.Export("bin." + modid);
      } else {
        theparser.EmitError(module.GetFileName().c_str());
        return 1;
      }
    } else if (flag == 'i') {
      module.SetFlavor(kFeProduced);
      module.SetSrcLang(kSrcLangJava);
      BinaryMplImport binmplt(module);
      binmplt.SetImported(false);
      std::string modid = module.GetFileName();
      binmplt.Import(modid, true);
      module.OutputAsciiMpl(".irb");
    }
    i++;
  }
  return 0;
}

/* hello.mpl
   flavor 1
   srclang 3

   type $person <struct {@age i64,
                      @data <* f32>}>
   var $BOB i32 = 8
 */
#else
#warning "this module is compiled without MIR_FEATURE_FULL=1 defined"
#endif  // MIR_FEATURE_FULL
