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
#ifndef MPL2MPL_INCLUDE_MUID_REPLACEMENT_H
#define MPL2MPL_INCLUDE_MUID_REPLACEMENT_H
#include "module_phase.h"
#include "phase_impl.h"
#include "muid.h"
#include "version.h"

namespace maple {
// For func def table.
constexpr uint32 kFuncDefAddrIndex = 0;
// For data def table.
constexpr uint32 kDataDefAddrIndex = 0;
// For func def info. table.
constexpr uint32 kFuncDefSizeIndex = 0;
constexpr uint32 kFuncDefNameIndex = 1;
constexpr uint32 kRangeBeginIndex = 0;
constexpr int32_t kDecoupleAndLazy = 3;

enum RangeIdx {
  // 0,1 entry is reserved for a stamp
  kVtab = 2,
  kItab = 3,
  kVtabOffset = 4,
  kFieldOffset = 5,
  kValueOffset = 6,
  kLocalClassInfo = 7,
  kConststr = 8,
  kSuperclass = 9,
  kGlobalRootlist = 10,
  kClassmetaData = 11,
  kClassBucket = 12,
  kJavatext = 13,
  kJavajni = 14,
  kJavajniFunc = 15,
  kOldMaxNum = 16, // Old num
  kDataSection = 17,
  kDecoupleStaticKey = 18,
  kDecoupleStaticValue = 19,
  kBssStart = 20,
  kLinkerSoHash = 21,
  kNewMaxNum = 22 // New num
};

class MUIDReplacement : public FuncOptimizeImpl {
 public:
  MUIDReplacement(MIRModule *mod, KlassHierarchy *kh, bool dump);
  ~MUIDReplacement() = default;

  FuncOptimizeImpl *Clone() override {
    return new MUIDReplacement(*this);
  }

  void ProcessFunc(MIRFunction *func) override;

  static void SetMplMd5(MUID muid) {
    mplMuid = muid;
  }

  static MUID &GetMplMd5() {
    return mplMuid;
  }

 private:
  using SymIdxPair = std::pair<MIRSymbol*, uint32>;
  enum LazyBindingOption : uint32 {
    kNoLazyBinding = 0,
    kConservativeLazyBinding = 1,
    kRadicalLazyBinding = 2
  };

  void GenerateTables();
  void GenerateFuncDefTable();
  void GenerateDataDefTable();
  void GenerateUnifiedUndefTable();
  void GenerateRangeTable();
  uint32 FindIndexFromDefTable(const MIRSymbol &mirSymbol, bool isFunc);
  uint32 FindIndexFromUndefTable(const MIRSymbol &mirSymbol, bool isFunc);
  void ReplaceAddroffuncConst(MIRConst *&entry, uint32 fieldID, bool isVtab);
  void ReplaceFuncTable(const std::string &name);
  void ReplaceAddrofConst(MIRConst *&entry);
  void ReplaceDataTable(const std::string &name);
  void ReplaceDirectInvokeOrAddroffunc(MIRFunction &currentFunc, StmtNode &stmt);
  void ReplaceDassign(MIRFunction &currentFunc, const DassignNode &dassignNode);
  void ReplaceDreadStmt(MIRFunction *currentFunc, StmtNode *stmt);
  void ClearVtabItab(const std::string &name);
  void ReplaceDecoupleKeyTable(MIRAggConst *oldConst);
  BaseNode *ReplaceDreadExpr(MIRFunction *currentFunc, StmtNode *stmt, BaseNode *expr);
  BaseNode *ReplaceDread(MIRFunction &currentFunc, const StmtNode *stmt, BaseNode *opnd);
  void CollectDread(MIRFunction &currentFunc, StmtNode &stmt, BaseNode &opnd);
  void DumpMUIDFile(bool isFunc);
  void ReplaceStmts();
  void GenerateGlobalRootList();
  void CollectImplicitUndefClassInfo(StmtNode &stmt);
  void ReplaceMethodMetaFuncAddr(MIRSymbol &funcSymbol, int64 index);
  void ReplaceFieldMetaStaticAddr(MIRSymbol &mirSymbol, int64 index);
  void CollectFuncAndDataFromKlasses();
  void CollectFuncAndDataFromGlobalTab();
  void CollectFuncAndDataFromFuncList();
  void GenerateCompilerVersionNum();
  static MIRSymbol *GetSymbolFromName(const std::string &name);
  // The following sets are for internal uses. Sorting order does not matter here.
  std::unordered_set<MIRFunction*> funcDefSet;
  std::unordered_set<MIRFunction*> funcUndefSet;
  std::unordered_set<MIRSymbol*> dataDefSet;
  std::unordered_set<MIRSymbol*> dataUndefSet;
  void AddDefFunc(MIRFunction *func) {
    funcDefSet.insert(func);
  }

  void AddUndefFunc(MIRFunction *func) {
    funcUndefSet.insert(func);
  }

  void AddDefData(MIRSymbol *sym) {
    dataDefSet.insert(sym);
  }

  void AddUndefData(MIRSymbol *sym) {
    dataUndefSet.insert(sym);
  }

#define __MRT_MAGIC_PASTE(x, y) __MRT_MAGIC_PASTE2(x, y)
#define __MRT_MAGIC_PASTE2(x, y) x##y
#define CLASS_PREFIX(classname) TO_STR(__MRT_MAGIC_PASTE(CLASSINFO_PREFIX, classname)),
  const std::unordered_set<std::string> preloadedClassInfo = {
  };
#undef CLASS_PREFIX
#undef __MRT_MAGIC_PASTE2
#undef __MRT_MAGIC_PASTE
  const std::unordered_set<std::string> reflectionList = {
  };
  bool isLibcore = false;
  MIRSymbol *funcDefTabSym = nullptr;
  MIRSymbol *funcDefOrigTabSym = nullptr;
  MIRSymbol *funcInfTabSym = nullptr;
  MIRSymbol *funcUndefTabSym = nullptr;
  MIRSymbol *dataDefTabSym = nullptr;
  MIRSymbol *dataDefOrigTabSym = nullptr;
  MIRSymbol *dataUndefTabSym = nullptr;
  MIRSymbol *funcDefMuidTabSym = nullptr;
  MIRSymbol *funcUndefMuidTabSym = nullptr;
  MIRSymbol *dataDefMuidTabSym = nullptr;
  MIRSymbol *dataUndefMuidTabSym = nullptr;
  MIRSymbol *funcMuidIdxTabSym = nullptr;
  MIRSymbol *rangeTabSym = nullptr;
  MIRSymbol *funcProfileTabSym = nullptr;
  std::map<MUID, SymIdxPair> funcDefMap;
  std::map<MUID, SymIdxPair> dataDefMap;
  std::map<MUID, SymIdxPair> funcUndefMap;
  std::map<MUID, SymIdxPair> dataUndefMap;
  std::map<MUID, uint32> defMuidIdxMap;
  static MUID mplMuid;
  std::string mplMuidStr;
};

class DoMUIDReplacement : public ModulePhase {
 public:
  explicit DoMUIDReplacement(ModulePhaseID id) : ModulePhase(id) {}

  ~DoMUIDReplacement() = default;

  std::string PhaseName() const override {
    return "MUIDReplacement";
  }

  AnalysisResult *Run(MIRModule *mod, ModuleResultMgr *mrm) override {
    OPT_TEMPLATE(MUIDReplacement);
    return nullptr;
  }
};
}  // namespace maple
#endif  // MPL2MPL_INCLUDE_MUID_REPLACEMENT_H
