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
#ifndef MAPLEBE_MDGEN_INCLUDE_MDGENERATOR_H
#define MAPLEBE_MDGEN_INCLUDE_MDGENERATOR_H
#include <fstream>
#include "mdrecord.h"
#include "mpl_logging.h"

namespace MDGen {
class MDCodeGen {
 public:
  MDCodeGen(const MDClassRange &inputRange, const std::string &oFileDirArg)
      : curKeeper (inputRange),
        outputFileDir(oFileDirArg) {}
  virtual ~MDCodeGen() = default;

  const std::string &GetOFileDir() const {
    return outputFileDir;
  }
  void SetTargetArchName(const std::string &archName) {
    targetArchName = archName;
  }

  void EmitCheckPtr(std::ofstream &outputFile, const std::string &emitName, const std::string &name,
                    const std::string &ptrType);
  void EmitFileHead(std::ofstream &outputFile, const std::string &headInfo);
  MDClass GetSpecificClass (const std::string &className);

 protected:
  MDClassRange curKeeper;

 private:
  static std::string targetArchName;
  std::string outputFileDir;
};

class SchedInfoGen : public MDCodeGen {
 public:
  SchedInfoGen(MDClassRange &inputRange, const std::string &oFileDirArg) : MDCodeGen(inputRange, oFileDirArg) {}
  ~SchedInfoGen() override {
    if (outFile.is_open()) {
      outFile.close();
    }
  }

  void EmitArchDef();
  const std::string &GetArchName();
  void EmitUnitIdDef();
  void EmitUnitDef();
  void EmitUnitNameDef();
  void EmitLatencyDef();
  void EmitResvDef();
  void EmitBypassDef();
  void Run();

 private:
  std::ofstream outFile;
};
} /* namespace MDGen */

#endif /* MAPLEBE_MDGEN_INCLUDE_MDGENERATOR_H */