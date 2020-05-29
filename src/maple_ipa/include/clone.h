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
#ifndef MAPLE_IPA_INCLUDE_CLONE_H
#define MAPLE_IPA_INCLUDE_CLONE_H
#include "mir_module.h"
#include "mir_function.h"
#include "mir_builder.h"
#include "mempool.h"
#include "mempool_allocator.h"
#include "class_hierarchy.h"
#include "me_ir.h"
#include "module_phase.h"

static constexpr char kFullNameStr[] = "INFO_fullname";
static constexpr char kClassNameStr[] = "INFO_classname";
static constexpr char kFuncNameStr[] = "INFO_funcname";
static constexpr char kVoidRetSuffix[] = "CLONEDignoreret";
namespace maple {
class ReplaceRetIgnored {
 public:
  explicit ReplaceRetIgnored(MemPool *memPool);
  virtual ~ReplaceRetIgnored() = default;

  bool ShouldReplaceWithVoidFunc(const CallMeStmt &stmt, const MIRFunction &calleeFunc) const;
  std::string GenerateNewBaseName(const MIRFunction &originalFunc) const;
  std::string GenerateNewFullName(const MIRFunction &originalFunc) const;
  const MapleSet<MapleString> *GetTobeClonedFuncNames() const {
    return &toBeClonedFuncNames;
  }

  bool IsInCloneList(const std::string &funcName) const {
    return toBeClonedFuncNames.find(MapleString(funcName, memPool)) != toBeClonedFuncNames.end();
  }

  static bool IsClonedFunc(const std::string &funcName) {
    return funcName.find(kVoidRetSuffix) != std::string::npos;
  }

 private:
  MemPool *memPool;
  maple::MapleAllocator allocator;
  MapleSet<MapleString> toBeClonedFuncNames;
  bool RealShouldReplaceWithVoidFunc(Opcode op, size_t nRetSize, const MIRFunction &calleeFunc) const;
};

class Clone : public AnalysisResult {
 public:
  Clone(MIRModule *mod, MemPool *memPool, MIRBuilder &builder, KlassHierarchy *kh)
      : AnalysisResult(memPool), mirModule(mod), allocator(memPool), dexBuilder(builder), kh(kh),
        replaceRetIgnored(memPool->New<ReplaceRetIgnored>(memPool)) {}

  ~Clone() = default;

  static MIRSymbol *CloneLocalSymbol(const MIRSymbol &oldSym, const MIRFunction &newFunc);
  static void CloneSymbols(MIRFunction &newFunc, const MIRFunction &oldFunc);
  static void CloneLabels(MIRFunction &newFunc, const MIRFunction &oldFunc);
  MIRFunction *CloneFunction(MIRFunction &originalFunction, const std::string &newBaseFuncName,
      MIRType *returnType = nullptr) const;
  MIRFunction *CloneFunctionNoReturn(MIRFunction &originalFunction);
  void DoClone();
  void CopyFuncInfo(const MIRFunction &originalFunction, MIRFunction &newFunc) const;
  void UpdateFuncInfo(MIRFunction &newFunc);
  void CloneArgument(MIRFunction &riginalFunction, ArgVector &argument) const;
  ReplaceRetIgnored *GetReplaceRetIgnored() {
    return replaceRetIgnored;
  }

  void UpdateReturnVoidIfPossible(CallMeStmt *callMeStmt, const MIRFunction &targetFunc);

 private:
  MIRModule *mirModule;
  MapleAllocator allocator;
  MIRBuilder &dexBuilder;
  KlassHierarchy *kh;
  ReplaceRetIgnored *replaceRetIgnored;
};

class DoClone : public ModulePhase {
 public:
  explicit DoClone(ModulePhaseID id) : ModulePhase(id) {}

  virtual ~DoClone() = default;

  AnalysisResult *Run(MIRModule *module, ModuleResultMgr *m) override;
  std::string PhaseName() const override {
    return "clone";
  }
};
}  // namespace maple
#endif  // MAPLE_IPA_INCLUDE_CLONE_H
