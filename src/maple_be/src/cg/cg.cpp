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
#include <set>
#include "emit.h"

namespace maplebe {
using namespace maple;

#define JAVALANG (mirModule->IsJavaModule())

CGFunc *CG::currentCGFunction = nullptr;

CG::~CG() {
  if (emitter != nullptr) {
    emitter->CloseOutput();
  }
  memPool = nullptr;
  mirModule = nullptr;
  emitter = nullptr;
  currentCGFunction = nullptr;
  instrumentationFunction = nullptr;
  dbgTraceEnter = nullptr;
  dbgTraceExit = nullptr;
  dbgFuncProfile = nullptr;
}
/* This function intends to be a more general form of GenFieldOffsetmap. */
void CG::GenExtraTypeMetadata(const std::string &classListFileName, const std::string &outputBaseName) {
  const std::string &cMacroDefSuffix = ".macros.def";
  BECommon *beCommon = Globals::GetInstance()->GetBECommon();
  std::vector<MIRClassType*> classesToGenerate;

  if (classListFileName.empty()) {
    /*
     * Class list not specified. Visit all classes.
     */
    std::set<std::string> visited;

    for (const auto &tyId : mirModule->GetClassList()) {
      MIRType *mirType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(tyId);
      if ((mirType->GetKind() != kTypeClass) && (mirType->GetKind() != kTypeClassIncomplete)) {
        continue;  /* Skip non-class. Too paranoid. We just enumerated classlist_! */
      }
      MIRClassType *classType = static_cast<MIRClassType*>(mirType);
      const std::string &name = classType->GetName();

      if (visited.find(name) != visited.end()) {
        continue;  /* Skip duplicated class definitions. */
      }

      visited.insert(name);
      classesToGenerate.push_back(classType);
    }
  } else {
    /* Visit listed classes. */
    std::ifstream inFile(classListFileName);
    CHECK_FATAL(inFile.is_open(), "Failed to open file: %s", classListFileName.c_str());
    std::string str;

    /* check each class name first and expose all unknown classes */
    while (inFile >> str) {
      MIRType *type = GlobalTables::GetTypeTable().GetOrCreateClassType(str, *mirModule);
      MIRClassType *classType = static_cast<MIRClassType*>(type);
      if (classType == nullptr) {
        LogInfo::MapleLogger() << " >>>>>>>> unknown class: " << str.c_str() << "\n";
        return;
      }

      classesToGenerate.push_back(classType);
    }
  }

  if (cgOption.GenDef()) {
    const std::string &outputFileName = outputBaseName + cMacroDefSuffix;
    FILE *outputFile = fopen(outputFileName.c_str(), "w");
    if (outputFile == nullptr) {
      FATAL(kLncFatal, "open file failed in CG::GenExtraTypeMetadata");
    }

    for (auto classType : classesToGenerate) {
      beCommon->GenObjSize(*classType, *outputFile);
      beCommon->GenFieldOffsetMap(*classType, *outputFile);
    }
    fclose(outputFile);
  }

  if (cgOption.GenGctib()) {
    maple::LogInfo::MapleLogger(kLlErr) << "--gen-gctib-file option not implemented";
  }
}

void CG::GenPrimordialObjectList(const std::string &outputBaseName) {
  const std::string &kPrimorListSuffix = ".primordials.txt";
  if (!cgOption.GenPrimorList()) {
    return;
  }

  const std::string &outputFileName = outputBaseName + kPrimorListSuffix;
  FILE *outputFile = fopen(outputFileName.c_str(), "w");
  if (outputFile == nullptr) {
    FATAL(kLncFatal, "open file failed in CG::GenPrimordialObjectList");
  }

  for (StIdx stIdx : mirModule->GetSymbolSet()) {
    MIRSymbol *symbol = GlobalTables::GetGsymTable().GetSymbolFromStidx(stIdx.Idx());
    ASSERT(symbol != nullptr, "get symbol from st idx failed");
    if (symbol->IsPrimordialObject()) {
      const std::string &name = symbol->GetName();
      fprintf(outputFile, "%s\n", name.c_str());
    }
  }

  fclose(outputFile);
}

void CG::AddStackGuardvar() {
  MIRSymbol *chkGuard = GlobalTables::GetGsymTable().CreateSymbol(kScopeGlobal);
  chkGuard->SetNameStrIdx(std::string("__stack_chk_guard"));
  chkGuard->SetStorageClass(kScExtern);
  chkGuard->SetSKind(kStVar);
  CHECK_FATAL(GlobalTables::GetTypeTable().GetTypeTable().size() > PTY_u64, "out of vector range");
  chkGuard->SetTyIdx(GlobalTables::GetTypeTable().GetTypeTable()[PTY_u64]->GetTypeIndex());
  GlobalTables::GetGsymTable().AddToStringSymbolMap(*chkGuard);

  MIRSymbol *chkFunc = GlobalTables::GetGsymTable().CreateSymbol(kScopeGlobal);
  chkFunc->SetNameStrIdx(std::string("__stack_chk_fail"));
  chkFunc->SetStorageClass(kScText);
  chkFunc->SetSKind(kStFunc);
  GlobalTables::GetGsymTable().AddToStringSymbolMap(*chkFunc);
}

void CG::SetInstrumentationFunction(const std::string &name) {
  instrumentationFunction = GlobalTables::GetGsymTable().CreateSymbol(kScopeGlobal);
  instrumentationFunction->SetNameStrIdx(std::string("__").append(name).append("__"));
  instrumentationFunction->SetStorageClass(kScText);
  instrumentationFunction->SetSKind(kStFunc);
}

#define DBG_TRACE_ENTER MplDtEnter
#define DBG_TRACE_EXIT MplDtExit
#define XSTR(s) str(s)
#define str(s) #s

void CG::DefineDebugTraceFunctions() {
  dbgTraceEnter = GlobalTables::GetGsymTable().CreateSymbol(kScopeGlobal);
  dbgTraceEnter->SetNameStrIdx(std::string("__" XSTR(DBG_TRACE_ENTER) "__"));
  dbgTraceEnter->SetStorageClass(kScText);
  dbgTraceEnter->SetSKind(kStFunc);

  dbgTraceExit = GlobalTables::GetGsymTable().CreateSymbol(kScopeGlobal);
  dbgTraceExit->SetNameStrIdx(std::string("__" XSTR(DBG_TRACE_EXIT) "__"));
  dbgTraceExit->SetStorageClass(kScText);
  dbgTraceExit->SetSKind(kStFunc);

  dbgFuncProfile = GlobalTables::GetGsymTable().CreateSymbol(kScopeGlobal);
  dbgFuncProfile->SetNameStrIdx(std::string("__" XSTR(MplFuncProfile) "__"));
  dbgFuncProfile->SetStorageClass(kScText);
  dbgFuncProfile->SetSKind(kStFunc);
}

/*
 * Add the fields of curStructType to the result. Used to handle recursive
 * structures.
 */
static void AppendReferenceOffsets64(const BECommon &beCommon, MIRStructType &curStructType, int64 &curOffset,
                                     std::vector<int64> &result) {
  /*
   * We are going to reimplement BECommon::GetFieldOffset so that we can do
   * this in one pass through all fields.
   *
   * The tricky part is to make sure the object layout described here is
   * compatible with the rest of the system. This implies that we need
   * something like a "Maple ABI" documented for each platform.
   */
  if (curStructType.GetKind() == kTypeClass) {
    MIRClassType &curClassTy = static_cast<MIRClassType&>(curStructType);
    auto maybeParent = GlobalTables::GetTypeTable().GetTypeFromTyIdx(curClassTy.GetParentTyIdx());
    if (maybeParent != nullptr) {
      if (maybeParent->GetKind() == kTypeClass) {
        auto parentClassType = static_cast<MIRClassType*>(maybeParent);
        AppendReferenceOffsets64(beCommon, *parentClassType, curOffset, result);
      } else {
        LogInfo::MapleLogger() << "WARNING:: generating objmap for incomplete class\n";
      }
    }
  }

  for (const auto &fieldPair : curStructType.GetFields()) {
    auto fieldNameIdx = fieldPair.first;
    auto fieldTypeIdx = fieldPair.second.first;

    auto &fieldName = GlobalTables::GetStrTable().GetStringFromStrIdx(fieldNameIdx);
    auto fieldType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(fieldTypeIdx);
    auto &fieldTypeName = GlobalTables::GetStrTable().GetStringFromStrIdx(fieldType->GetNameStrIdx());
    auto fieldTypeKind = fieldType->GetKind();

    auto fieldSize = beCommon.GetTypeSize(fieldTypeIdx);
    auto fieldAlign = beCommon.GetTypeAlign(fieldTypeIdx);
    int64 myOffset = static_cast<int64>(RoundUp(curOffset, fieldAlign));
    int64 nextOffset = myOffset + fieldSize;

    if (!CGOptions::IsQuiet()) {
      LogInfo::MapleLogger() << "    field: " << fieldName << "\n";
      LogInfo::MapleLogger() << "      type: " << fieldTypeIdx << ": " << fieldTypeName << "\n";
      LogInfo::MapleLogger() << "      type kind: " << fieldTypeKind << "\n";
      LogInfo::MapleLogger() << "      size: " << fieldSize << "\n";                               /* int64 */
      LogInfo::MapleLogger() << "      align: " << static_cast<uint32>(fieldAlign) << "\n";  /* int8_t */
      LogInfo::MapleLogger() << "      field offset:" << myOffset << "\n";                         /* int64 */
    }

    if (fieldTypeKind == kTypePointer) {
      if (!CGOptions::IsQuiet()) {
        LogInfo::MapleLogger() << "      ** Is a pointer field.\n";
      }
      result.push_back(myOffset);
    }

    if ((fieldTypeKind == kTypeArray) || (fieldTypeKind == kTypeStruct) || (fieldTypeKind == kTypeClass) ||
        (fieldTypeKind == kTypeInterface)) {
      if (!CGOptions::IsQuiet()) {
        LogInfo::MapleLogger() << "    ** ERROR: We are not expecting nested aggregate type. ";
        LogInfo::MapleLogger() << "All Java classes are flat -- no nested structs. ";
        LogInfo::MapleLogger() << "Please extend me if we are going to work with non-java languages.\n";
      }
    }

    curOffset = nextOffset;
  }
}

/* Return a list of offsets of reference fields. */
std::vector<int64> CG::GetReferenceOffsets64(const BECommon &beCommon, MIRStructType &structType) {
  std::vector<int64> result;
  /* java class layout has already been done in previous phase. */
  if (structType.GetKind() == kTypeClass) {
    for (auto fieldInfo : beCommon.GetJClassLayout(static_cast<MIRClassType&>(structType))) {
      if (fieldInfo.IsRef()) {
        result.push_back(static_cast<int64>(fieldInfo.GetOffset()));
      }
    }
  } else if (structType.GetKind() != kTypeInterface) {  /* interface doesn't have reference fields */
    int64 curOffset = 0;
    AppendReferenceOffsets64(beCommon, structType, curOffset, result);
  }

  return result;
}
}  /* namespace maplebe */
