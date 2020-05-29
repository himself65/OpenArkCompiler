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
#include "optimize_common.h"
#include "cgbb.h"
#include "cg.h"
#include "cg_option.h"
#include "securec.h"

/* This file provides common class and function for cfgo and ico. */
namespace maplebe {
void Optimizer::Run(const std::string &funcName, bool checkOnly) {
  /* Initialize cfg optimization patterns */
  InitOptimizePatterns();

  /* For each pattern, search cgFunc for optimization */
  for (OptimizationPattern *p : diffPassPatterns) {
    p->Search2Op(checkOnly);
  }
  /* Search the cgFunc for multiple possible optimizations in one pass */
  if (!singlePassPatterns.empty()) {
    BB *curBB = cgFunc->GetFirstBB();
    bool flag = false;
    while (curBB != nullptr) {
      for (OptimizationPattern *p : singlePassPatterns) {
        if (p->Optimize(*curBB)) {
          flag = p->IsKeepPosition();
          p->SetKeepPosition(false);
          break;
        }
      }

      if (flag) {
        flag = false;
      } else {
        curBB = curBB->GetNext();
      }
    }
  }

  if (CGOptions::IsDumpOptimizeCommonLog()) {
    constexpr int arrSize = 80;
    char post[arrSize];
    errno_t cpyRet = strcpy_s(post, arrSize, "post-");
    CHECK_FATAL(cpyRet == EOK, "call strcpy_s failed");
    errno_t catRes = strcat_s(post, arrSize, name);
    CHECK_FATAL(catRes == EOK, "call strcat_s failed ");
    OptimizeLogger::GetLogger().Print(funcName);
  }
  OptimizeLogger::GetLogger().ClearLocal();
}

void OptimizationPattern::Search2Op(bool noOptimize) {
  checkOnly = noOptimize;
  BB *curBB = cgFunc->GetFirstBB();
  while (curBB != nullptr) {
    static_cast<void>(Optimize(*curBB));
    if (keepPosition) {
      keepPosition = false;
    } else {
      curBB = curBB->GetNext();
    }
  }
}

void OptimizationPattern::Log(uint32 bbID) {
  OptimizeLogger::GetLogger().Log(patternName.c_str());
  DotGenerator::SetColor(bbID, dotColor.c_str());
}

std::map<uint32, std::string> DotGenerator::coloringMap;

void DotGenerator::SetColor(uint32 bbID, const std::string &color) {
  coloringMap[bbID] = color;
}

std::string DotGenerator::GetFileName(const MIRModule &mirModule, const std::string &filePreFix) {
  std::string fileName;
  if (!filePreFix.empty()) {
    fileName.append(filePreFix);
    fileName.append("-");
  }
  fileName.append(mirModule.GetFileName());
  for (uint32 i = 0; i < fileName.length(); i++) {
    if (fileName[i] == ';' || fileName[i] == '/' || fileName[i] == '|') {
      fileName[i] = '_';
    }
  }

  fileName.append(".dot");
  return fileName;
}

void DotGenerator::DumpEdge(const CGFunc &cgFunction, std::ofstream &cfgFileOfStream, bool isIncludeEH) {
  FOR_ALL_BB_CONST(bb, &cgFunction) {
    for (auto *succBB : bb->GetSuccs()) {
      cfgFileOfStream << "BB" << bb->GetId();
      cfgFileOfStream << " -> "
              << "BB" << succBB->GetId();
      cfgFileOfStream << ";\n";
    }
    if (isIncludeEH) {
      for (auto *ehSuccBB : bb->GetEhSuccs()) {
        cfgFileOfStream << "BB" << bb->GetId();
        cfgFileOfStream << " -> "
                << "BB" << ehSuccBB->GetId();
        cfgFileOfStream << "[color=red]";
        cfgFileOfStream << ";\n";
      }
    }
  }
}

bool DotGenerator::FoundListOpndRegNum(ListOperand &listOpnd, const Insn &insnObj, regno_t vReg) {
  bool exist = false;
  for (auto op : listOpnd.GetOperands()) {
    RegOperand *regOpnd = static_cast<RegOperand*>(op);
    if (op->IsRegister() && regOpnd->GetRegisterNumber() == vReg) {
      LogInfo::MapleLogger() << "BB" << insnObj.GetBB()->GetId() << " [style=filled, fillcolor=red];\n";
      exist = true;
      break;
    }
  }
  return exist;
}

bool DotGenerator::FoundMemAccessOpndRegNum(const MemOperand &memOperand, const Insn &insnObj, regno_t vReg) {
  Operand *base = memOperand.GetBaseRegister();
  Operand *offset = memOperand.GetIndexRegister();
  bool exist = false;
  if (base != nullptr && base->IsRegister()) {
    RegOperand *regOpnd = static_cast<RegOperand*>(base);
    if (regOpnd->GetRegisterNumber() == vReg) {
      LogInfo::MapleLogger() << "BB" << insnObj.GetBB()->GetId() << " [style=filled, fillcolor=red];\n";
      exist = true;
    }
  } else if (offset != nullptr && offset->IsRegister()) {
    RegOperand *regOpnd = static_cast<RegOperand*>(offset);
    if (regOpnd->GetRegisterNumber() == vReg) {
      LogInfo::MapleLogger() << "BB" << insnObj.GetBB()->GetId() << " [style=filled, fillcolor=red];\n";
      exist = true;
    }
  }
  return exist;
}

bool DotGenerator::FoundNormalOpndRegNum(RegOperand &regOpnd, const Insn &insnObj, regno_t vReg) {
  bool exist = false;
  if (regOpnd.GetRegisterNumber() == vReg) {
    LogInfo::MapleLogger() << "BB" << insnObj.GetBB()->GetId() << " [style=filled, fillcolor=red];\n";
    exist = true;
  }
  return exist;
}

void DotGenerator::DumpBBInstructions(const CGFunc &cgFunction, regno_t vReg, std::ofstream &cfgFile) {
  FOR_ALL_BB_CONST(bb, &cgFunction) {
    if (vReg != 0) {
      FOR_BB_INSNS_CONST(insn, bb) {
        bool found = false;
        uint32 opndNum = insn->GetOperandSize();
        for (uint32 i = 0; i < opndNum; ++i) {
          Operand &opnd = insn->GetOperand(i);
          if (opnd.IsList()) {
            auto &listOpnd = static_cast<ListOperand&>(opnd);
            found = FoundListOpndRegNum(listOpnd, *insn, vReg);
          } else if (opnd.IsMemoryAccessOperand()) {
            auto &memOpnd = static_cast<MemOperand&>(opnd);
            found = FoundMemAccessOpndRegNum(memOpnd, *insn, vReg);
          } else {
            if (opnd.IsRegister()) {
              auto &regOpnd = static_cast<RegOperand&>(opnd);
              found = FoundNormalOpndRegNum(regOpnd, *insn, vReg);
            }
          }
          if (found) {
            break;
          }
        }
        if (found) {
          break;
        }
      }
    }
    cfgFile << "BB" << bb->GetId() << "[";
    auto it = coloringMap.find(bb->GetId());
    if (it != coloringMap.end()) {
      cfgFile << "style=filled,fillcolor=" << it->second << ",";
    }
    if (bb->GetKind() == BB::kBBIf) {
      cfgFile << "shape=diamond,label= \" BB" << bb->GetId() << ":\n";
    } else {
      cfgFile << "shape=box,label= \" BB" << bb->GetId() << ":\n";
    }
    cfgFile << "{ ";
    cfgFile << bb->GetKindName() << "\n";
    cfgFile << "}\"];\n";
  }
}

/* Generate dot file for cfg */
void DotGenerator::GenerateDot(const std::string &preFix, CGFunc &cgFunc, const MIRModule &mod, bool includeEH,
                               regno_t vReg) {
  std::ofstream cfgFile;
  std::streambuf *coutBuf = std::cout.rdbuf(); /* keep original cout buffer */
  std::streambuf *buf = cfgFile.rdbuf();
  std::cout.rdbuf(buf);
  std::string fileName = GetFileName(mod, preFix);

  cfgFile.open(fileName, std::ios::trunc);
  CHECK_FATAL(cfgFile.is_open(), "Failed to open output file: %s", fileName.c_str());
  cfgFile << "digraph {\n";
  /* dump edge */
  DumpEdge(cgFunc, cfgFile, includeEH);

  /* dump instruction in each BB */
  DumpBBInstructions(cgFunc, vReg, cfgFile);

  cfgFile << "}\n";
  coloringMap.clear();
  cfgFile.flush();
  cfgFile.close();
  std::cout.rdbuf(coutBuf);
}

void OptimizeLogger::Print(const std::string &funcName) {
  if (!localStat.empty()) {
    LogInfo::MapleLogger() << funcName << '\n';
    for (const auto &localStatPair : localStat) {
      LogInfo::MapleLogger() << "Optimized " << localStatPair.first << ":" << localStatPair.second << "\n";
    }

    ClearLocal();
    LogInfo::MapleLogger() << "Total:" << '\n';
    for (const auto &globalStatPair : globalStat) {
      LogInfo::MapleLogger() << "Optimized " << globalStatPair.first << ":" << globalStatPair.second << "\n";
    }
  }
}

void OptimizeLogger::Log(const std::string &patternName) {
  auto itemInGlobal = globalStat.find(patternName);
  if (itemInGlobal != globalStat.end()) {
    itemInGlobal->second++;
  } else {
    globalStat.insert(std::pair<const std::string, int>(patternName, 1));
  }
  auto itemInLocal = localStat.find(patternName);
  if (itemInLocal != localStat.end()) {
    itemInLocal->second++;
  } else {
    localStat.insert(std::pair<const std::string, int>(patternName, 1));
  }
}

void OptimizeLogger::ClearLocal() {
  localStat.clear();
}
}  /* namespace maplebe */
