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
#include "mir_symbol.h"
#include <algorithm>
#include <unordered_set>
#include "mir_function.h"
#include "class_init.h"
#include "vtable_analysis.h"
#include "reflection_analysis.h"
#include "printing.h"
#include "native_stub_func.h"
#include "literal_str_name.h"
#include "string_utils.h"

namespace maple {
using namespace NameMangler;

bool MIRSymbol::IsTypeVolatile(int fieldID) const {
  const MIRType *ty = GlobalTables::GetTypeTable().GetTypeFromTyIdx(GetTyIdx());
  return ty->IsVolatile(fieldID);
}

void MIRSymbol::SetNameStrIdx(const std::string &name) {
  nameStrIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(name);
}

bool MIRSymbol::HasAddrOfValues() const {
  return  StringUtils::StartsWith(GetName(), VTAB_PREFIX_STR) ||
          StringUtils::StartsWith(GetName(), ITAB_PREFIX_STR) ||
          StringUtils::StartsWith(GetName(), kVtabOffsetTabStr) ||
          StringUtils::StartsWith(GetName(), kDecoupleStaticKeyStr) ||
          IsClassInitBridge() || IsReflectionInfo() || IsReflectionHashTabBucket() ||
          IsReflectionStrTab() || IsITabConflictInfo() || IsRegJNITab() ||
          IsRegJNIFuncTab() || IsLiteral();
}

bool MIRSymbol::IsLiteral() const {
  return StringUtils::StartsWith(GetName(), kConstString);
}

bool MIRSymbol::IsLiteralPtr() const {
  return StringUtils::StartsWith(GetName(), kConstStringPtr);
}

MIRType *MIRSymbol::GetType() const {
  return GlobalTables::GetTypeTable().GetTypeFromTyIdx(tyIdx);
}

bool MIRSymbol::PointsToConstString() const {
  MIRType *origType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(tyIdx);
  if (origType->GetKind() == kTypePointer) {
    return static_cast<MIRPtrType*>(origType)->PointsToConstString();
  }
  return false;
}

bool MIRSymbol::IsConstString() const {
  return typeAttrs.GetAttr(ATTR_static) && typeAttrs.GetAttr(ATTR_final) && PointsToConstString();
}

bool MIRSymbol::IsReflectionStrTab() const {
  return StringUtils::StartsWith(GetName(), kReflectionStrtabPrefixStr) ||
         StringUtils::StartsWith(GetName(), kReflectionStartHotStrtabPrefixStr) ||
         StringUtils::StartsWith(GetName(), kReflectionBothHotStrTabPrefixStr) ||
         StringUtils::StartsWith(GetName(), kReflectionRunHotStrtabPrefixStr) ||
         StringUtils::StartsWith(GetName(), kReflectionNoEmitStrtabPrefixStr);
}

bool MIRSymbol::IsRegJNITab() const {
  return StringUtils::StartsWith(GetName(), kRegJNITabPrefixStr);
}

bool MIRSymbol::IsRegJNIFuncTab() const {
  return StringUtils::StartsWith(GetName(), kRegJNIFuncTabPrefixStr);
}

bool MIRSymbol::IsMuidTab() const {
  return StringUtils::StartsWith(GetName(), kMuidPrefixStr);
}

bool MIRSymbol::IsMuidRoTab() const {
  return StringUtils::StartsWith(GetName(), kMuidRoPrefixStr);
}

bool MIRSymbol::IsCodeLayoutInfo() const {
  return StringUtils::StartsWith(GetName(), kFunctionLayoutStr);
}

std::string MIRSymbol::GetMuidTabName() const {
  if (!IsMuidTab()) {
    return "";
  }
  size_t idx = GetName().find(kFileNameSplitterStr);
  return (idx != std::string::npos) ? GetName().substr(0, idx) : "";
}

bool MIRSymbol::IsMuidFuncDefTab() const {
  return StringUtils::StartsWith(GetName(), kMuidFuncDefTabPrefixStr);
}

bool MIRSymbol::IsMuidFuncDefOrigTab() const {
  return StringUtils::StartsWith(GetName(), kMuidFuncDefOrigTabPrefixStr);
}

bool MIRSymbol::IsMuidFuncInfTab() const {
  return StringUtils::StartsWith(GetName(), kMuidFuncInfTabPrefixStr);
}

bool MIRSymbol::IsMuidFuncUndefTab() const {
  return StringUtils::StartsWith(GetName(), kMuidFuncUndefTabPrefixStr);
}

bool MIRSymbol::IsMuidDataDefTab() const {
  return StringUtils::StartsWith(GetName(), kMuidDataDefTabPrefixStr);
}

bool MIRSymbol::IsMuidDataDefOrigTab() const {
  return StringUtils::StartsWith(GetName(), kMuidDataDefOrigTabPrefixStr);
}

bool MIRSymbol::IsMuidDataUndefTab() const {
  return StringUtils::StartsWith(GetName(), kMuidDataUndefTabPrefixStr);
}

bool MIRSymbol::IsMuidFuncDefMuidTab() const {
  return StringUtils::StartsWith(GetName(), kMuidFuncDefMuidTabPrefixStr);
}

bool MIRSymbol::IsMuidFuncUndefMuidTab() const {
  return StringUtils::StartsWith(GetName(), kMuidFuncUndefMuidTabPrefixStr);
}

bool MIRSymbol::IsMuidDataDefMuidTab() const {
  return StringUtils::StartsWith(GetName(), kMuidDataDefMuidTabPrefixStr);
}

bool MIRSymbol::IsMuidDataUndefMuidTab() const {
  return StringUtils::StartsWith(GetName(), kMuidDataUndefMuidTabPrefixStr);
}

bool MIRSymbol::IsMuidFuncMuidIdxMuidTab() const {
  return StringUtils::StartsWith(GetName(), kMuidFuncMuidIdxTabPrefixStr);
}

bool MIRSymbol::IsMuidRangeTab() const {
  return StringUtils::StartsWith(GetName(), kMuidRangeTabPrefixStr);
}

bool MIRSymbol::IsClassInitBridge() const {
  return StringUtils::StartsWith(GetName(), CLASS_INIT_BRIDGE_PREFIX_STR);
}

bool MIRSymbol::IsReflectionHashTabBucket() const {
  return StringUtils::StartsWith(GetName(), kMuidClassMetadataBucketPrefixStr);
}

bool MIRSymbol::IsReflectionInfo() const {
  return IsReflectionClassInfo() || IsReflectionClassInfoRO() || IsReflectionFieldsInfo() ||
         IsReflectionFieldsInfoCompact() || IsReflectionMethodsInfo() || IsReflectionPrimitiveClassInfo() ||
         IsReflectionSuperclassInfo() || IsReflectionMethodsInfoCompact();
}

bool MIRSymbol::IsReflectionFieldsInfo() const {
  return StringUtils::StartsWith(GetName(), kFieldsInfoPrefixStr);
}

bool MIRSymbol::IsReflectionFieldsInfoCompact() const {
  return StringUtils::StartsWith(GetName(), kFieldsInfoCompactPrefixStr);
}

bool MIRSymbol::IsReflectionSuperclassInfo() const {
  return StringUtils::StartsWith(GetName(), SUPERCLASSINFO_PREFIX_STR);
}

bool MIRSymbol::IsReflectionFieldOffsetData() const {
  return StringUtils::StartsWith(GetName(), kFieldOffsetDataPrefixStr);
}

bool MIRSymbol::IsReflectionMethodAddrData() const {
  return (GetName().find(kMethodAddrDataPrefixStr) == 0);
}

bool MIRSymbol::IsReflectionClassInfo() const {
  return StringUtils::StartsWith(GetName(), CLASSINFO_PREFIX_STR);
}

bool MIRSymbol::IsReflectionArrayClassInfo() const {
  return StringUtils::StartsWith(GetName(), kArrayClassInfoPrefixStr);
}

bool MIRSymbol::IsReflectionClassInfoPtr() const {
  return StringUtils::StartsWith(GetName(), kClassINfoPtrPrefixStr);
}

bool MIRSymbol::IsReflectionClassInfoRO() const {
  return StringUtils::StartsWith(GetName(), CLASSINFO_RO_PREFIX_STR);
}

bool MIRSymbol::IsITabConflictInfo() const {
  return StringUtils::StartsWith(GetName(), ITAB_CONFLICT_PREFIX_STR);
}

bool MIRSymbol::IsVTabInfo() const {
  return StringUtils::StartsWith(GetName(), VTAB_PREFIX_STR);
}

bool MIRSymbol::IsITabInfo() const {
  return StringUtils::StartsWith(GetName(), ITAB_PREFIX_STR);
}

bool MIRSymbol::IsReflectionPrimitiveClassInfo() const {
  return StringUtils::StartsWith(GetName(), PRIMITIVECLASSINFO_PREFIX_STR);
}

bool MIRSymbol::IsReflectionMethodsInfo() const {
  return StringUtils::StartsWith(GetName(), kMethodsInfoPrefixStr);
}

bool MIRSymbol::IsReflectionMethodsInfoCompact() const {
  return StringUtils::StartsWith(GetName(), kMethodsInfoCompactPrefixStr);
}

bool MIRSymbol::IsPrimordialObject() const {
  return IsReflectionClassInfo() || IsReflectionPrimitiveClassInfo();
}

bool MIRSymbol::IsGctibSym() const {
  return StringUtils::StartsWith(GetName(), GCTIB_PREFIX_STR);
}

// [Note]
// Some symbols are ignored by reference counting as they represent objects not managed by us. These include
// string-based exact comparison for "current_vptr", "vtabptr", "itabptr", "funcptr", "env_ptr", "retvar_stubfunc".
GStrIdx MIRSymbol::reflectClassNameIdx;
GStrIdx MIRSymbol::reflectMethodNameIdx;
GStrIdx MIRSymbol::reflectFieldNameIdx;
bool MIRSymbol::IgnoreRC() const {
  if (isDeleted || GetAttr(ATTR_rcunowned)) {
    return true;
  }
  const std::string &name = GetName();
  // ignore %current_vptr, %vtabptr, %itabptr, %funcptr, %env_ptr
  if (name == "current_vptr" || name == "vtabptr" || name == "itabptr" || name == "funcptr" || name == "env_ptr" ||
      name == "retvar_stubfunc" || name == "_dummy_stub_object_retval") {
    return true;
  }
  if (IsReflectionInfo() || IsRegJNITab() || IsRegJNIFuncTab()) {
    return true;
  }
  MIRType *type = GetType();
  // only consider reference
  if (type == nullptr || type->GetPrimType() != PTY_ref) {
    return true;
  }
  if ((type->GetKind() == kTypeScalar) && (name != "__mapleRC__")) {
    return true;
  }
  // ignore ptr to types Ljava_2Flang_2FClass_3B,
  // Ljava_2Flang_2Freflect_2FMethod_3B
  const auto *pType = static_cast<MIRPtrType*>(type);
  GStrIdx strIdx = GlobalTables::GetTypeTable().GetTypeFromTyIdx(pType->GetPointedTyIdx())->GetNameStrIdx();
  if (reflectClassNameIdx == 0u) {
    reflectClassNameIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(
        NameMangler::GetInternalNameLiteral("Ljava_2Flang_2FClass_3B"));
  }
  return strIdx == reflectClassNameIdx;
}

void MIRSymbol::Dump(bool isLocal, int32 indent, bool suppressInit) const {
  // exclude unused symbols, formal symbols and extern functions
  if (GetStorageClass() == kScUnused || GetStorageClass() == kScFormal ||
      (GetStorageClass() == kScExtern && sKind == kStFunc)) {
    return;
  }
  if (GetTyIdx() >= GlobalTables::GetTypeTable().GetTypeTable().size()) {
    FATAL(kLncFatal, "valid maple_ir with illegal type");
  }
  if (GetStorageClass() == kScText && GetFunction() != nullptr) {
    // without body
    GetFunction()->Dump(true);
    return;
  }
  const char *ids = isLocal ? "%" : "$";
  PrintIndentation(indent);
  if (sKind == kStJavaClass) {
    LogInfo::MapleLogger() << "javaclass ";
  } else if (sKind == kStJavaInterface) {
    LogInfo::MapleLogger() << "javainterface ";
  } else if (isTmp) {
    LogInfo::MapleLogger() << "tempvar ";
  } else {
    LogInfo::MapleLogger() << "var ";
  }
  LogInfo::MapleLogger() << ids << GetName() << " ";
  if (GetStorageClass() == kScFstatic) {
    LogInfo::MapleLogger() << "fstatic ";
  } else if (GetStorageClass() == kScPstatic) {
    LogInfo::MapleLogger() << "pstatic ";
  } else if (GetStorageClass() == kScExtern) {
    LogInfo::MapleLogger() << "extern ";
  }
  if (GetTyIdx() != 0u) {
    GlobalTables::GetTypeTable().GetTypeFromTyIdx(GetTyIdx())->Dump(indent + 1);
  }
  typeAttrs.DumpAttributes();
  if (sKind == kStJavaClass || sKind == kStJavaInterface || GetStorageClass() == kScTypeInfoName ||
      GetStorageClass() == kScTypeInfo || GetStorageClass() == kScTypeCxxAbi) {
    LogInfo::MapleLogger() << '\n';
    return;
  }
  if (IsConst() && !suppressInit && !(IsLiteral() && GetStorageClass() == kScExtern)) {
    LogInfo::MapleLogger() << " = ";
    GetKonst()->Dump();
  }
  LogInfo::MapleLogger() << '\n';
}

void MIRSymbol::DumpAsLiteralVar() const {
  if (IsLiteral()) {
    LogInfo::MapleLogger() << GetName();
  }
}


void MIRSymbolTable::Dump(bool isLocal, int32 indent, bool printDeleted) const {
  size_t size = symbolTable.size();
  for (size_t i = 0; i < size; ++i) {
    MIRSymbol *symbol = symbolTable[i];
    if (symbol == nullptr) {
      continue;
    }
    if (!printDeleted && symbol->IsDeleted()) {
      continue;
    }
    symbol->Dump(isLocal, indent);
  }
}

LabelIdx MIRLabelTable::CreateLabelWithPrefix(char c) {
  LabelIdx labelIdx = labelTable.size();
  std::ostringstream labelNameStream;
  labelNameStream << "@" << c << labelIdx;
  std::string labelName = labelNameStream.str();
  GStrIdx nameIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(labelName);
  labelTable.push_back(nameIdx);
  strIdxToLabIdxMap[nameIdx] = labelIdx;
  return labelIdx;
}

const std::string &MIRLabelTable::GetName(LabelIdx labelIdx) const {
  CHECK_FATAL(labelIdx < labelTable.size(), "index out of range in MIRLabelTable::GetName");
  return GlobalTables::GetStrTable().GetStringFromStrIdx(labelTable[labelIdx]);
}

bool MIRLabelTable::AddToStringLabelMap(LabelIdx labelIdx) {
  CHECK_FATAL(labelIdx < labelTable.size(), "index out of range in MIRLabelTable::AddToStringLabelMap");
  if (labelTable[labelIdx] == 0u) {
    // generate a label name based on lab_idx
    std::ostringstream labelNameStream;
    labelNameStream << "@" << labelIdx;
    std::string labelName = labelNameStream.str();
    labelTable[labelIdx] = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(labelName);
  }
  GStrIdx strIdx = labelTable[labelIdx];
  strIdxToLabIdxMap[strIdx] = labelIdx;
  return true;
}

void MIRPregTable::DumpRef(int32 indent) {
  MapleVector<MIRPreg*> &pRegTable = pregTable;
  for (size_t i = 1; i < pRegTable.size(); ++i) {
    MIRPreg *mirPReg = pRegTable[i];
    if (mirPReg->GetPrimType() != PTY_ref) {
      continue;
    }
    if (mirPReg->GetMIRType() == nullptr) {
      continue;
    }
    PrintIndentation(indent);
    LogInfo::MapleLogger() << "reg ";
    LogInfo::MapleLogger() << "%" << mirPReg->GetPregNo();
    LogInfo::MapleLogger() << " ";
    mirPReg->GetMIRType()->Dump(0);
    LogInfo::MapleLogger() << " " << (mirPReg->NeedRC() ? 1 : 0);
    LogInfo::MapleLogger() << "\n";
  }
}
}  // namespace maple
