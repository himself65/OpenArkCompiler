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
#include <cstdio>
#include <unistd.h>
#include <sys/stat.h>
#include <iomanip>
#include <algorithm>
#include "mdgenerator.h"

namespace MDGen {
std::string MDCodeGen::targetArchName = "";

void MDCodeGen::EmitCheckPtr(std::ofstream &outputFile, const std::string &emitName, const std::string &name,
                             const std::string &ptrType) {
  outputFile << "if(" << emitName << " == nullptr) {\n" <<
                "  maple::LogInfo::MapleLogger(maple::kLlErr) << \"" << ptrType << " allocation for " << name <<
                " failed.\" << std::endl;\n" << "}\n" <<
                "ASSERT(" << emitName << ", \"" << ptrType << " allocation for " << name <<
                " failed.\");\n" << "\n";
}

void MDCodeGen::EmitFileHead(std::ofstream &outputFile, const std::string &headInfo) {
  outputFile << "/* " << targetArchName << " " << headInfo << " definition : */\n";
}

MDClass MDCodeGen::GetSpecificClass(const std::string &className) {
  unsigned int classIdx = curKeeper.GetStrInTable(className).idx;
  CHECK_FATAL(classIdx != UINT_MAX, "Load Class Failed!");
  return curKeeper.GetOneMDClass(classIdx);
}

const std::string &SchedInfoGen::GetArchName() {
  MDClass archClass = GetSpecificClass("ArchitectureName");
  const MDObject &archObj = archClass.GetOneMDObject(0);
  auto *archStrEle = static_cast<const StringElement*>(archObj.GetOneMDElement(0));
  return curKeeper.GetStrByIdx(archStrEle->GetContent());
}

void SchedInfoGen::EmitArchDef() {
  MDClass parallelClass = GetSpecificClass("Parallelism");
  CHECK_FATAL(parallelClass.GetMDObjectSize() > 0, "specific class failed, maybe illegal input");
  const MDObject &paralleObj = parallelClass.GetOneMDObject(0);
  auto *parallelEle = static_cast<const IntElement*>(paralleObj.GetOneMDElement(0));
  outFile.open(GetOFileDir() + "/mplad_arch_define.def", std::ios::out);
  EmitFileHead(outFile, "Architecture");
  outFile << "SetMaxParallelism(" << parallelEle->GetContent() << ");\n";
  outFile.close();
}

void SchedInfoGen::EmitUnitIdDef() {
  MDClass unitClass = GetSpecificClass("Unit");
  outFile.open(GetOFileDir() + "/mplad_unit_id.def", std::ios::out);
  CHECK_FATAL(outFile.is_open(), "Failed to open output file: %s/mplad_unit_id.def", GetOFileDir().c_str());
  EmitFileHead(outFile, "function unit ID");
  for (auto unitIdx : unitClass.GetchildObjNames()) {
    outFile << "  " << curKeeper.GetStrByIdx(unitIdx) << ",\n";
  }
  outFile.close();
}

void SchedInfoGen::EmitUnitNameDef() {
  MDClass unitClass = GetSpecificClass("Unit");
  outFile.open(GetOFileDir() + "/mplad_unit_name.def", std::ios::out);
  CHECK_FATAL(outFile.is_open(), "Failed to open output file: %s/mplad_unit_name.def", GetOFileDir().c_str());
  EmitFileHead(outFile, "function unit name");
  for (auto unitIdx : unitClass.GetchildObjNames()) {
    std::string unitPureName = curKeeper.GetStrByIdx(unitIdx);
    std::string unitPrefix = "kUnitId";
    if (unitPrefix.length() < unitPureName.length()) {
      unitPureName = unitPureName.substr(unitPrefix.length());
      outFile << "\"" << unitPureName << "\",\n";
    }
  }
  outFile.close();
}

void SchedInfoGen::EmitUnitDef() {
  MDClass unitClass = GetSpecificClass("Unit");
  outFile.open(GetOFileDir() + "/mplad_unit_define.def", std::ios::out);
  CHECK_FATAL(outFile.is_open(), "Failed to open output file: %s/mplad_unit_define.def", GetOFileDir().c_str());
  EmitFileHead(outFile, "function units ");
  bool isUnitNumDef = false;
  for (size_t i = 0; i < unitClass.GetMDObjectSize(); ++i) {
    const MDObject &singleUnit = unitClass.GetOneMDObject(i);
    if (singleUnit.GetOneMDElement(0)->GetRecDataTy() == MDElement::kEleDefaultTy) {
      continue;
    }
    auto *curUnitTy = static_cast<const DefTyElement*>(singleUnit.GetOneMDElement(0));
    std::string curUnitName = curKeeper.GetStrByIdx(singleUnit.GetIdx());
    std::string emitUnitName = "instance" + curUnitName;
    std::string unitPrefix = "Unit *" + emitUnitName + " = new Unit(";
    if (!isUnitNumDef) {
      outFile << "\n";
      outFile << "const unsigned int kunitNum = 2;\n";
      isUnitNumDef = true;
    }
    outFile << unitPrefix;
    if (curUnitTy->GetContent() == curKeeper.GetStrInTable("Primary").idx) {
      outFile << curUnitName << ");\n";
    } else {
      std::string unitTypeStr = "";
      if (curUnitTy->GetContent() == curKeeper.GetStrInTable("And").idx) {
        unitTypeStr = "kUnitTypeAnd";
      } else if(curUnitTy->GetContent() == curKeeper.GetStrInTable("Or").idx) {
        unitTypeStr = "kUnitTypeOr";
      }
      CHECK_FATAL(unitTypeStr.size() != 0, "Haven't support this kind of Unit yet");
      outFile << unitTypeStr << ", " << curUnitName << ", kunitNum,\n";
      outFile << std::setiosflags(std::ios::right) << std::setw(unitPrefix.length()) << std::setfill(' ') << " ";
      unsigned int dependUnitsIndex = 1;
      auto *dependUnitEle = static_cast<const VecElement*>(singleUnit.GetOneMDElement(dependUnitsIndex));
      for (size_t k = 0; k < dependUnitEle->GetVecDataSize(); ++k) {
        auto *dependUnit = static_cast<DefObjElement*>(dependUnitEle->GetVecData()[k]);
        outFile << "instance" <<  curKeeper.GetStrByIdx(dependUnit->GetContent());
        if (k != dependUnitEle->GetVecDataSize() - 1) {
          outFile << ", ";
        }
      }
      outFile << ");\n";
    }
    EmitCheckPtr(outFile, emitUnitName, curUnitName, "Unit");
  }
  outFile.close();
}

void SchedInfoGen::EmitLatencyDef() {
  MDClass resvClass = GetSpecificClass("Reservation");
  outFile.open(GetOFileDir() + "/mplad_latency_type.def", std::ios::out);
  CHECK_FATAL(outFile.is_open(), "Failed to open output file: %s/mplad_latency_type.def", GetOFileDir().c_str());
  EmitFileHead(outFile, " latency type definition ");
  for (auto resvIdx : resvClass.GetchildObjNames()) {
    outFile << "  " << curKeeper.GetStrByIdx(resvIdx) << ",\n";
  }
  outFile.close();
}

void SchedInfoGen::EmitResvDef() {
  MDClass resvClass = GetSpecificClass("Reservation");
  outFile.open(GetOFileDir() + "/mplad_reservation_define.def", std::ios::out);
  CHECK_FATAL(outFile.is_open(), "Failed to open output file: %s/mplad_reservation_define.def",
              GetOFileDir().c_str());
  EmitFileHead(outFile, "reservations");
  for (size_t i = 0; i < resvClass.GetMDObjectSize(); ++i) {
    const MDObject &singleResv = resvClass.GetOneMDObject(i);
    if (singleResv.GetOneMDElement(0)->GetRecDataTy() == MDElement::kEleDefaultTy) {
      continue;
    }
    auto *curResvLatency = static_cast<const IntElement*>(singleResv.GetOneMDElement(0));
    std::string curResvName = curKeeper.GetStrByIdx(singleResv.GetIdx());
    std::string emitResvName = "resvInst" + curResvName;
    std::string resvPrefix = "Reservation *" + emitResvName + " = new Reservation(";
    outFile << resvPrefix << curResvName << ", " << curResvLatency->GetContent() << ", ";
    if (singleResv.GetOneMDElement(1)->GetRecDataTy() == MDElement::kEleDefaultTy) {
      outFile << "0);\n";
    } else {
      size_t dependUnitsIndex = 1;
      auto *dependUnitEle = static_cast<const VecElement*>(singleResv.GetOneMDElement(dependUnitsIndex));
      outFile << dependUnitEle->GetVecDataSize() << ",\n";
      for (size_t k = 0; k < dependUnitEle->GetVecDataSize(); ++k) {
        auto *dependUnit = static_cast<DefObjElement*>(dependUnitEle->GetVecData()[k]);
        if (curKeeper.GetStrByIdx(dependUnit->GetContent()) != "nothing") {
          outFile << std::setiosflags(std::ios::right) << std::setw(resvPrefix.length()) << std::setfill(' ')
                  << "GetUnitByUnitId(" << curKeeper.GetStrByIdx(dependUnit->GetContent()) << ")";
        } else {
          outFile << std::setiosflags(std::ios::right) << std::setw(resvPrefix.length()) << std::setfill(' ')
                  << "nullptr";
        }
        if (k < dependUnitEle->GetVecDataSize() - 1) {
          outFile << ",\n";
        }
      }
      outFile << ");\n";
    }
    EmitCheckPtr(outFile, emitResvName, curResvName, "Reservation");
  }
  outFile.close();
}

void SchedInfoGen::EmitBypassDef() {
  MDClass bypassClass = GetSpecificClass("Bypass");
  outFile.open(GetOFileDir() + "/mplad_bypass_define.def", std::ios::out);
  for (size_t i = 0; i < bypassClass.GetMDObjectSize(); ++i) {
    const MDObject &singleBypass = bypassClass.GetOneMDObject(i);
    if (singleBypass.GetOneMDElement(0)->GetRecDataTy() == MDElement::kEleDefaultTy) {
      continue;
    }
    constexpr size_t fromVecIndex = 1;
    constexpr size_t toVecIndex = 2;
    constexpr size_t curBpTyIndex = 3;
    auto *bpTyEle = singleBypass.GetOneMDElement(curBpTyIndex);
    std::string curBypassTy = (bpTyEle->GetRecDataTy() == MDElement::kEleDefaultTy) ?
        "" :  curKeeper.GetStrByIdx(bpTyEle->GetContent());
    transform(curBypassTy.begin(), curBypassTy.end(), curBypassTy.begin(), ::toupper);

    CHECK_FATAL(singleBypass.GetOneMDElement(0)->GetRecDataTy() == MDElement::ElementTy::kEleIntTy, "Bypass illegal");
    CHECK_FATAL(singleBypass.GetOneMDElement(fromVecIndex)->GetRecDataTy() == MDElement::ElementTy::kEleVecTy,
        "Bypass illegal");
    CHECK_FATAL(singleBypass.GetOneMDElement(toVecIndex)->GetRecDataTy() == MDElement::ElementTy::kEleVecTy,
        "Bypass illegal");

    unsigned int bypassNum = static_cast<const IntElement*>(singleBypass.GetOneMDElement(0))->GetContent();
    auto *fromVec = static_cast<const VecElement*>(singleBypass.GetOneMDElement(fromVecIndex));
    auto *toVec = static_cast<const VecElement*>(singleBypass.GetOneMDElement(toVecIndex));
    for (auto itTo : toVec->GetVecData()) {
      for (auto itFrom : fromVec->GetVecData()) {
        auto *fromResv = static_cast<DefObjElement*>(itFrom);
        auto *toResv = static_cast<DefObjElement*>(itTo);
        outFile << "ADD" << curBypassTy << "BYPASS(" << curKeeper.GetStrByIdx(fromResv->GetContent()) << ", "
                << curKeeper.GetStrByIdx(toResv->GetContent()) << ", " << bypassNum <<");\n";
      }
    }
  }
  outFile.close();
}

void SchedInfoGen::Run() {
  SetTargetArchName(GetArchName());
  EmitArchDef();
  EmitResvDef();
  EmitBypassDef();
  EmitUnitDef();
  EmitUnitNameDef();
  EmitLatencyDef();
  EmitUnitIdDef();
}
} /* namespace MDGen */
