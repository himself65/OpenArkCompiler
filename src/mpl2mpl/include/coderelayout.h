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
#ifndef MPL2MPL_INCLUDE_CODERELAYOUT_H
#define MPL2MPL_INCLUDE_CODERELAYOUT_H
#include <fstream>
#include "phase_impl.h"
#include "module_phase.h"
#include "file_layout.h"

namespace maple {
class CodeReLayout : public FuncOptimizeImpl {
 public:
  CodeReLayout(MIRModule *mod, KlassHierarchy *kh, bool dump);
  ~CodeReLayout() = default;

  FuncOptimizeImpl *Clone() override {
    return new CodeReLayout(*this);
  }

  void ProcessFunc(MIRFunction *func) override;
  void Finish() override;

 private:
  const std::string exeFuncTag = "executedFuncStart";
  const std::string profileStartTag = "#profile_start";
  const std::string profileSummaryTag = "#profile_summary";
  std::unordered_map<std::string, MIRSymbol*> str2SymMap;
  uint32 layoutCount[static_cast<uint32>(LayoutType::kLayoutTypeCount)] = {};
  std::string StaticFieldFilename(const std::string &mplFile) const;
  void GenLayoutSym();
  void AddStaticFieldRecord();
  CallNode *CreateRecordFieldStaticCall(BaseNode *node, const std::string &name);
  void FindDreadRecur(const StmtNode *stmt, BaseNode *node);
  void InsertProfileBeforeDread(const StmtNode *stmt, BaseNode *opnd);
  MIRSymbol *GetorCreateStaticFieldSym(const std::string &fieldName);
  MIRSymbol *GenStrSym(const std::string &str);
};

class DoCodeReLayout : public ModulePhase {
 public:
  explicit DoCodeReLayout(ModulePhaseID id) : ModulePhase(id) {}

  ~DoCodeReLayout() = default;

  std::string PhaseName() const override {
    return "CodeReLayout";
  }

  AnalysisResult *Run(MIRModule *mod, ModuleResultMgr *mrm) override {
    OPT_TEMPLATE(CodeReLayout);
    return nullptr;
  }
};
}  // namespace maple
#endif  // MPL2MPL_INCLUDE_CODERELAYOUT_H
