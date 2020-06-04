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
#ifndef MAPLEBE_INCLUDE_CG_OPTIMIZE_COMMON_H
#define MAPLEBE_INCLUDE_CG_OPTIMIZE_COMMON_H
#include "cgfunc.h"

namespace maplebe {
const std::string kCfgoChaining = "red";
const std::string kCfgoSj = "burlywood1";
const std::string kCfgoFlipCond = "cadetblue1";
const std::string kCfgoAlways = "green";
const std::string kCfgoUnreach = "yellow";
const std::string kCfgoDup = "orange";
const std::string kCfgoEmpty = "purple";
const std::string kIcoIte = "blue"; /* if conversion optimization, if-then-else */
const std::string kIcoIt = "grey";  /* if conversion optimization, if-then-else */

class OptimizationPattern {
 public:
  explicit OptimizationPattern(CGFunc &func)
      : patternName(func.GetMemoryPool()),
        cgFunc(&func),
        dotColor(func.GetMemoryPool()) {}
  virtual ~OptimizationPattern() = default;

  bool IsKeepPosition() const {
    return keepPosition;
  }

  void SetKeepPosition(bool flag) {
    keepPosition = flag;
  }

  bool IsLabelInLSDAOrSwitchTable(LabelIdx label) const {
    return cgFunc->GetTheCFG()->InLSDA(label, *cgFunc->GetEHFunc()) ||
           cgFunc->GetTheCFG()->InSwitchTable(label, *cgFunc);
  }

  void Search2Op(bool checkOnly);
  virtual bool Optimize(BB &curBB) = 0;

 protected:
  void Log(uint32 bbID);

  bool keepPosition = false;
  MapleString patternName;
  CGFunc *cgFunc;
  MapleString dotColor;
  bool checkOnly = false;
};

class Optimizer {
 public:
  Optimizer(CGFunc &func, MemPool &memPool)
      : cgFunc(&func),
        name(nullptr),
        memPool(&memPool),
        alloc(&memPool),
        diffPassPatterns(alloc.Adapter()),
        singlePassPatterns(alloc.Adapter()) {
        func.GetTheCFG()->InitInsnVisitor(func);
  }

  virtual ~Optimizer() = default;
  void Run(const std::string &funcName, bool checkOnly = false);
  virtual void InitOptimizePatterns() = 0;

 protected:
  CGFunc *cgFunc;
  const char *name;
  MemPool *memPool;
  MapleAllocator alloc;
  /* patterns need to run in different passes of cgFunc */
  MapleVector<OptimizationPattern*> diffPassPatterns;
  /* patterns can run in a single pass of cgFunc */
  MapleVector<OptimizationPattern*> singlePassPatterns;
};

class OptimizeLogger {
 public:
  static OptimizeLogger &GetLogger() {
    static OptimizeLogger instance;
    return instance;
  }

  void Log(const std::string &patternName);
  void ClearLocal();
  void Print(const std::string &funcName);

 private:
  OptimizeLogger() = default;

  ~OptimizeLogger() = default;

  OptimizeLogger(const OptimizeLogger&);
  OptimizeLogger &operator=(const OptimizeLogger&);

  std::map<const std::string, int> globalStat;
  std::map<const std::string, int> localStat;
};

class DotGenerator {
 public:
  static void SetColor(uint32 bbID, const std::string &color);
  static void GenerateDot(const std::string &preFix, CGFunc &cgFunc, const MIRModule &mod,
                          bool includeEH = false, regno_t vReg = 0);
 private:
  static std::map<uint32, std::string> coloringMap;
  static std::string GetFileName(const MIRModule &mirModule, const std::string &filePreFix);
  static void DumpEdge(const CGFunc &cgFunction, std::ofstream &cfgFileOfStream, bool isIncludeEH);
  static void DumpBBInstructions(const CGFunc &cgFunction, regno_t vReg, std::ofstream &cfgFile);
  static bool FoundListOpndRegNum(ListOperand &listOpnd, const Insn &insnObj, regno_t vReg);
  static bool FoundMemAccessOpndRegNum(const MemOperand &memOperand, const Insn &insnObj, regno_t vReg);
  static bool FoundNormalOpndRegNum(RegOperand &regOpnd, const Insn &insnObj, regno_t vReg);
};
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_OPTIMIZE_COMMON_H */