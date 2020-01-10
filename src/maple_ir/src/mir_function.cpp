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
#include "mir_function.h"
#include <cstdio>
#include <iostream>
#include "mir_nodes.h"
#include "printing.h"
#include "string_utils.h"

namespace {
using namespace maple;

enum FuncProp : uint32_t {
  kFuncPropHasCall = 1U,           // the function has call
  kFuncPropRetStruct = 1U << 1,    // the function returns struct
  kFuncPropUserFunc = 1U << 2,     // the function is a user func
  kFuncPropInfoPrinted = 1U << 3,  // to avoid printing framesize/moduleid/funcSize info more
                                   // than once per function since they
                                   // can only be printed at the beginning of a block
  kFuncPropNeverReturn = 1U << 4,  // the function when called never returns
};
}  // namespace

namespace maple {
void MIRFunction::Init() {
  // Initially allocate symTab and pregTab on the module mempool for storing
  // parameters. If later mirfunction turns out to be a definition, new
  // tables will be allocated on the local data mempool.
  symTab = module->GetMemPool()->New<MIRSymbolTable>(module->GetMPAllocator());
  pregTab = module->GetMemPool()->New<MIRPregTable>(module, &module->GetMPAllocator());
  typeNameTab = module->GetMemPool()->New<MIRTypeNameTable>(module->GetMPAllocator());
  labelTab = module->GetMemPool()->New<MIRLabelTable>(module->GetMPAllocator());
}

const MIRSymbol *MIRFunction::GetFuncSymbol() const {
  return GlobalTables::GetGsymTable().GetSymbolFromStidx(symbolTableIdx.Idx());
}
MIRSymbol *MIRFunction::GetFuncSymbol() {
  return const_cast<MIRSymbol*>(const_cast<const MIRFunction*>(this)->GetFuncSymbol());
}

const std::string &MIRFunction::GetName() const {
  return GlobalTables::GetGsymTable().GetSymbolFromStidx(symbolTableIdx.Idx())->GetName();
}

GStrIdx MIRFunction::GetNameStrIdx() const {
  return GlobalTables::GetGsymTable().GetSymbolFromStidx(symbolTableIdx.Idx())->GetNameStrIdx();
}

const std::string &MIRFunction::GetBaseClassName() const {
  return GlobalTables::GetStrTable().GetStringFromStrIdx(baseClassStrIdx);
}

const std::string &MIRFunction::GetBaseFuncName() const {
  return GlobalTables::GetStrTable().GetStringFromStrIdx(baseFuncStrIdx);
}

const std::string &MIRFunction::GetBaseFuncNameWithType() const {
  return GlobalTables::GetStrTable().GetStringFromStrIdx(baseFuncWithTypeStrIdx);
}


const std::string &MIRFunction::GetSignature() const {
  return GlobalTables::GetStrTable().GetStringFromStrIdx(signatureStrIdx);
}

const MIRType *MIRFunction::GetReturnType() const {
  return GlobalTables::GetTypeTable().GetTypeFromTyIdx(funcType->GetRetTyIdx());
}
MIRType *MIRFunction::GetReturnType() {
  return const_cast<MIRType*>(const_cast<const MIRFunction*>(this)->GetReturnType());
}
const MIRType *MIRFunction::GetClassType() const {
  return GlobalTables::GetTypeTable().GetTypeFromTyIdx(classTyIdx);
}
const MIRType *MIRFunction::GetNthParamType(size_t i) const {
  ASSERT(i < funcType->GetParamTypeList().size(), "array index out of range");
  return GlobalTables::GetTypeTable().GetTypeFromTyIdx(funcType->GetParamTypeList()[i]);
}
MIRType *MIRFunction::GetNthParamType(size_t i) {
  return const_cast<MIRType*>(const_cast<const MIRFunction*>(this)->GetNthParamType(i));
}

LabelIdx MIRFunction::GetOrCreateLableIdxFromName(const std::string &name) {
  GStrIdx strIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(name);
  LabelIdx labelIdx = GetLabelTab()->GetStIdxFromStrIdx(strIdx);
  if (labelIdx == 0) {
    labelIdx = GetLabelTab()->CreateLabel();
    GetLabelTab()->SetSymbolFromStIdx(labelIdx, strIdx);
    GetLabelTab()->AddToStringLabelMap(labelIdx);
  }
  return labelIdx;
}

bool MIRFunction::HasCall() const {
  return flag & kFuncPropHasCall;
}
void MIRFunction::SetHasCall() {
  flag |= kFuncPropHasCall;
}

bool MIRFunction::IsReturnStruct() const {
  return flag & kFuncPropRetStruct;
}
void MIRFunction::SetReturnStruct() {
  flag |= kFuncPropRetStruct;
}
void MIRFunction::SetReturnStruct(MIRType &retType) {
  if (retType.IsStructType()) {
    flag |= kFuncPropRetStruct;
  }
}

bool MIRFunction::IsUserFunc() const {
  return flag & kFuncPropUserFunc;
}
void MIRFunction::SetUserFunc() {
  flag |= kFuncPropUserFunc;
}

bool MIRFunction::IsInfoPrinted() const {
  return flag & kFuncPropInfoPrinted;
}
void MIRFunction::SetInfoPrinted() {
  flag |= kFuncPropInfoPrinted;
}
void MIRFunction::ResetInfoPrinted() {
  flag &= ~kFuncPropInfoPrinted;
}

void MIRFunction::SetNoReturn() {
  flag |= kFuncPropNeverReturn;
}
bool MIRFunction::NeverReturns() const {
  return flag & kFuncPropNeverReturn;
}

void MIRFunction::SetAttrsFromSe(uint8 specialEffect) {
  // NoPrivateDefEffect
  if ((specialEffect & kDefEffect) == kDefEffect) {
    funcAttrs.SetAttr(FUNCATTR_noprivate_defeffect);
  }
  // NoPrivateUseEffect
  if ((specialEffect & kUseEffect) == kUseEffect) {
    funcAttrs.SetAttr(FUNCATTR_noretarg);
  }
  // IpaSeen
  if ((specialEffect & kIpaSeen) == kIpaSeen) {
    funcAttrs.SetAttr(FUNCATTR_ipaseen);
  }
  // Pure
  if ((specialEffect & kPureFunc) == kPureFunc) {
    funcAttrs.SetAttr(FUNCATTR_pure);
  }
  // NoDefArgEffect
  if ((specialEffect & kNoDefArgEffect) == kNoDefArgEffect) {
    funcAttrs.SetAttr(FUNCATTR_nodefargeffect);
  }
  // NoDefEffect
  if ((specialEffect & kNoDefEffect) == kNoDefEffect) {
    funcAttrs.SetAttr(FUNCATTR_nodefeffect);
  }
  // NoRetNewlyAllocObj
  if ((specialEffect & kNoRetNewlyAllocObj) == kNoRetNewlyAllocObj) {
    funcAttrs.SetAttr(FUNCATTR_noretglobal);
  }
  // NoThrowException
  if ((specialEffect & kNoThrowException) == kNoThrowException) {
    funcAttrs.SetAttr(FUNCATTR_nothrow_exception);
  }
}

void FuncAttrs::DumpAttributes() const {
#define STRING(s) #s
#define FUNC_ATTR
#define ATTR(AT)              \
  if (GetAttr(FUNCATTR_##AT)) \
    LogInfo::MapleLogger() << " " << STRING(AT);
#include "all_attributes.def"
#undef ATTR
#undef FUNC_ATTR
}

void MIRFunction::DumpFlavorLoweredThanMmpl() const {
  LogInfo::MapleLogger() << " (";

  // Dump arguments
  size_t argSize = GetParamSize();
  for (size_t i = 0; i < argSize; ++i) {
    const MIRSymbol *symbol = formals[i];
    if (symbol != nullptr) {
      if (symbol->GetSKind() != kStPreg) {
        LogInfo::MapleLogger() << "var %" << symbol->GetName() << " ";
      } else {
        LogInfo::MapleLogger() << "reg %" << symbol->GetPreg()->GetPregNo() << " ";
      }
    }
    constexpr int kIndent = 2;
    const MIRType *type = GetNthParamType(i);
    type->Dump(kIndent);
    const TypeAttrs &attrs = GetNthParamAttr(i);
    attrs.DumpAttributes();
    if (i != (argSize - 1)) {
      LogInfo::MapleLogger() << ", ";
    }
  }
  if (IsVarargs()) {
    if (argSize == 0) {
      LogInfo::MapleLogger() << "...";
    } else {
      LogInfo::MapleLogger() << ", ...";
    }
  }

  LogInfo::MapleLogger() << ") ";
  GetReturnType()->Dump(1);
}

void MIRFunction::Dump(bool withoutBody) {
  // skip the functions that are added during process methods in
  // class and interface decls.  these has nothing in formals
  // they do have paramtypelist_. this can not skip ones without args
  // but for them at least the func decls are valid
  if (GetParamSize() != formals.size() || GetAttr(FUNCATTR_optimized)) {
    return;
  }

  // save the module's curfunction and set it to the one currently Dump()ing
  MIRFunction *savedFunc = module->CurFunction();
  module->SetCurFunction(this);

  MIRSymbol *symbol = GlobalTables::GetGsymTable().GetSymbolFromStidx(symbolTableIdx.Idx());
  ASSERT(symbol != nullptr, "symbol MIRSymbol is null");
  LogInfo::MapleLogger() << "func " << "&" << symbol->GetName();

  funcAttrs.DumpAttributes();

  if (module->GetFlavor() < kMmpl) {
    DumpFlavorLoweredThanMmpl();
  }

  // codeMemPool is nullptr, means maple_ir has been released for memory's sake
  if (codeMemPool == nullptr) {
    LogInfo::MapleLogger() << '\n';
    LogInfo::MapleLogger() << "# [WARNING] skipped dumping because codeMemPool is nullptr " << '\n';
  } else if (GetBody() != nullptr && !withoutBody && symbol->GetStorageClass() != kScExtern) {
    ResetInfoPrinted();  // this ensures funcinfo will be printed
    GetBody()->Dump(*module, 0, module->GetFlavor() < kMmpl ? GetSymTab() : nullptr,
                    module->GetFlavor() < kMmpl ? GetPregTab() : nullptr, false, true);  // Dump body
  } else {
    LogInfo::MapleLogger() << '\n';
  }

  // restore the curfunction
  module->SetCurFunction(savedFunc);
}

void MIRFunction::DumpUpFormal(int32 indent) const {
  PrintIndentation(indent + 1);

  LogInfo::MapleLogger() << "upFormalSize " << GetUpFormalSize() << '\n';
  if (localWordsTypeTagged != nullptr) {
    PrintIndentation(indent + 1);
    LogInfo::MapleLogger() << "formalWordsTypeTagged = [ ";
    const auto *p = reinterpret_cast<const uint32*>(localWordsTypeTagged);
    LogInfo::MapleLogger() << std::hex;
    while (p < reinterpret_cast<const uint32*>(localWordsTypeTagged + BlockSize2BitVectorSize(GetUpFormalSize()))) {
      LogInfo::MapleLogger() << std::hex << "0x" << *p << " ";
      ++p;
    }
    LogInfo::MapleLogger() << std::dec << "]\n";
  }

  if (formalWordsRefCounted != nullptr) {
    PrintIndentation(indent + 1);
    LogInfo::MapleLogger() << "formalWordsRefCounted = [ ";
    const uint32 *p = reinterpret_cast<const uint32*>(formalWordsRefCounted);
    LogInfo::MapleLogger() << std::hex;
    while (p < reinterpret_cast<const uint32*>(formalWordsRefCounted + BlockSize2BitVectorSize(GetUpFormalSize()))) {
      LogInfo::MapleLogger() << std::hex << "0x" << *p << " ";
      ++p;
    }
    LogInfo::MapleLogger() << std::dec << "]\n";
  }
}

void MIRFunction::DumpFrame(int32 indent) const {
  PrintIndentation(indent + 1);

  LogInfo::MapleLogger() << "frameSize " << static_cast<uint32>(GetFrameSize()) << '\n';
  if (localWordsTypeTagged != nullptr) {
    PrintIndentation(indent + 1);
    LogInfo::MapleLogger() << "localWordsTypeTagged = [ ";
    const uint32 *p = reinterpret_cast<const uint32*>(localWordsTypeTagged);
    LogInfo::MapleLogger() << std::hex;
    while (p < reinterpret_cast<const uint32*>(localWordsTypeTagged + BlockSize2BitVectorSize(GetFrameSize()))) {
      LogInfo::MapleLogger() << std::hex << "0x" << *p << " ";
      ++p;
    }
    LogInfo::MapleLogger() << std::dec << "]\n";
  }

  if (localWordsRefCounted != nullptr) {
    PrintIndentation(indent + 1);
    LogInfo::MapleLogger() << "localWordsRefCounted = [ ";
    const uint32 *p = reinterpret_cast<const uint32*>(localWordsRefCounted);
    LogInfo::MapleLogger() << std::hex;
    while (p < reinterpret_cast<const uint32*>(localWordsRefCounted + BlockSize2BitVectorSize(GetFrameSize()))) {
      LogInfo::MapleLogger() << std::hex << "0x" << *p << " ";
      ++p;
    }
    LogInfo::MapleLogger() << std::dec << "]\n";
  }
}

void MIRFunction::DumpFuncBody(int32 indent) {
  LogInfo::MapleLogger() << "  funcid " << GetPuidxOrigin() << '\n';

  if (IsInfoPrinted()) {
    return;
  }

  SetInfoPrinted();

  if (GetUpFormalSize() > 0) {
    DumpUpFormal(indent);
  }

  if (GetFrameSize() > 0) {
    DumpFrame(indent);
  }

  if (GetModuleId() > 0) {
    PrintIndentation(indent + 1);
    LogInfo::MapleLogger() << "moduleID " << static_cast<uint32>(GetModuleId()) << '\n';
  }

  if (GetFuncSize() > 0) {
    PrintIndentation(indent + 1);
    LogInfo::MapleLogger() << "funcSize " << GetFuncSize() << '\n';
  }

  if (GetInfoVector().empty()) {
    return;
  }

  const MIRInfoVector &funcInfo = GetInfoVector();
  const MapleVector<bool> &funcInfoIsString = InfoIsString();
  PrintIndentation(indent + 1);
  LogInfo::MapleLogger() << "funcinfo {\n";
  size_t size = funcInfo.size();
  constexpr int kIndentOffset = 2;
  for (size_t i = 0; i < size; ++i) {
    PrintIndentation(indent + kIndentOffset);
    LogInfo::MapleLogger() << "@" << GlobalTables::GetStrTable().GetStringFromStrIdx(funcInfo[i].first) << " ";
    if (!funcInfoIsString[i]) {
      LogInfo::MapleLogger() << funcInfo[i].second;
    } else {
      LogInfo::MapleLogger() << "\""
                             << GlobalTables::GetStrTable().GetStringFromStrIdx(GStrIdx(funcInfo[i].second))
                             << "\"";
    }
    if (i < size - 1) {
      LogInfo::MapleLogger() << ",\n";
    } else {
      LogInfo::MapleLogger() << "}\n";
    }
  }
  LogInfo::MapleLogger() << '\n';
}

bool MIRFunction::IsEmpty() const {
  return (body == nullptr || body->IsEmpty());
}

bool MIRFunction::IsClinit() const {
  const std::string clinitPostfix = "_7C_3Cclinit_3E_7C_28_29V";
  const std::string &funcName = this->GetName();
  // this does not work for smali files like art/test/511-clinit-interface/smali/BogusInterface.smali,
  // which is decorated without "constructor".
  return StringUtils::EndsWith(funcName, clinitPostfix);
}

uint32 MIRFunction::GetInfo(GStrIdx strIdx) const {
  for (const auto &item : info) {
    if (item.first == strIdx) {
      return item.second;
    }
  }
  ASSERT(false, "get info error");
  return 0;
}

uint32 MIRFunction::GetInfo(const std::string &string) const {
  GStrIdx strIdx = GlobalTables::GetStrTable().GetStrIdxFromName(string);
  return GetInfo(strIdx);
}

void MIRFunction::OverrideBaseClassFuncNames(GStrIdx strIdx) {
  baseClassStrIdx.reset();
  baseFuncStrIdx.reset();
  SetBaseClassFuncNames(strIdx);
}

// there are two ways to represent the delimiter: '|' or "_7C"
// where 7C is the ascii value of char '|' in hex
void MIRFunction::SetBaseClassFuncNames(GStrIdx strIdx) {
  if (baseClassStrIdx != 0 || baseFuncStrIdx != 0) {
    return;
  }
  const std::string name = GlobalTables::GetStrTable().GetStringFromStrIdx(strIdx);
  std::string delimiter = "|";
  uint32 width = 1;  // delimiter width
  size_t pos = name.find(delimiter);
  if (pos == std::string::npos) {
    delimiter = NameMangler::kNameSplitterStr;
    width = 3;  // delimiter width
    pos = name.find(delimiter);
    // make sure it is not __7C, but ___7C ok
    while (pos != std::string::npos && (name[pos - 1] == '_' && name[pos - 2] != '_')) {
      pos = name.find(delimiter, pos + width);
    }
  }
  if (pos != std::string::npos && pos > 0) {
    const std::string className = name.substr(0, pos);
    baseClassStrIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(className);
    std::string funcNameWithType = name.substr(pos + width, name.length() - pos - width);
    baseFuncWithTypeStrIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(funcNameWithType);
    size_t index = name.find(NameMangler::kRigthBracketStr);
    ASSERT(index != std::string::npos, "Invalid name, cannot find '_29' in name");
    size_t posEnd = index + (std::string(NameMangler::kRigthBracketStr)).length();
    funcNameWithType = name.substr(pos + width, posEnd - pos - width);
    size_t newPos = name.find(delimiter, pos + width);
    while (newPos != std::string::npos && (name[newPos - 1] == '_' && name[newPos - 2] != '_')) {
      newPos = name.find(delimiter, newPos + width);
    }
    if (newPos != 0) {
      std::string funcName = name.substr(pos + width, newPos - pos - width);
      baseFuncStrIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(funcName);
      std::string signature = name.substr(newPos + width, name.length() - newPos - width);
      signatureStrIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(signature);
    }
    return;
  }
  baseFuncStrIdx = strIdx;
}

const MIRSymbol *MIRFunction::GetLocalOrGlobalSymbol(const StIdx &idx, bool checkFirst) const {
  return idx.Islocal() ? GetSymbolTabItem(idx.Idx(), checkFirst)
                       : GlobalTables::GetGsymTable().GetSymbolFromStidx(idx.Idx(), checkFirst);
}
MIRSymbol *MIRFunction::GetLocalOrGlobalSymbol(const StIdx &idx, bool checkFirst) {
  return const_cast<MIRSymbol*>(const_cast<const MIRFunction*>(this)->GetLocalOrGlobalSymbol(idx, checkFirst));
}

const MIRType *MIRFunction::GetNodeType(const BaseNode &node) const {
  if (node.GetOpCode() == OP_dread) {
    const MIRSymbol *sym = GetLocalOrGlobalSymbol(static_cast<const DreadNode&>(node).GetStIdx());
    return GlobalTables::GetTypeTable().GetTypeFromTyIdx(sym->GetTyIdx());
  }
  if (node.GetOpCode() == OP_regread) {
    const auto &nodeReg = static_cast<const RegreadNode&>(node);
    const MIRPreg *pReg = GetPregTab()->PregFromPregIdx(nodeReg.GetRegIdx());
    if (pReg->GetPrimType() == PTY_ref) {
      return pReg->GetMIRType();
    }
  }
  return nullptr;
}

void MIRFunction::NewBody() {
  SetBody(codeMemPool->New<BlockNode>());
  // If mir_function.has been seen as a declaration, its symtab has to be moved
  // from module mempool to function mempool.
  MIRSymbolTable *oldSymTable = GetSymTab();
  MIRPregTable *oldPregTable = GetPregTab();
  MIRTypeNameTable *oldTypeNameTable = typeNameTab;
  MIRLabelTable *oldLabelTable = GetLabelTab();
  symTab = dataMemPool->New<MIRSymbolTable>(dataMPAllocator);
  SetPregTab(dataMemPool->New<MIRPregTable>(module, &dataMPAllocator));
  typeNameTab = dataMemPool->New<MIRTypeNameTable>(dataMPAllocator);
  SetLabelTab(dataMemPool->New<MIRLabelTable>(dataMPAllocator));
  if (oldSymTable != nullptr) {
    for (size_t i = 1; i < oldSymTable->GetSymbolTableSize(); ++i) {
      (void)GetSymTab()->AddStOutside(oldSymTable->GetSymbolFromStIdx(i));
    }
  }
  if (oldPregTable != nullptr) {
    for (size_t i = 1; i < oldPregTable->Size(); ++i) {
      GetPregTab()->AddPreg(oldPregTable->PregFromPregIdx(i));
    }
  }
  if (oldTypeNameTable != nullptr) {
    ASSERT(oldTypeNameTable->Size() == typeNameTab->Size(),
           "Does not expect to process typeNameTab in MIRFunction::NewBody");
  }
  if (oldLabelTable != nullptr) {
    ASSERT(oldLabelTable->Size() == GetLabelTab()->Size(),
           "Does not expect to process labelTab in MIRFunction::NewBody");
  }
}

void MIRFunction::SetUpGDBEnv() {
  if (codeMemPool != nullptr) {
    memPoolCtrler.DeleteMemPool(codeMemPool);
  }
  codeMemPool = memPoolCtrler.NewMemPool("tmp debug");
  codeMemPoolAllocator.SetMemPool(codeMemPool);
}

void MIRFunction::ResetGDBEnv() {
  memPoolCtrler.DeleteMemPool(codeMemPool);
  codeMemPool = nullptr;
}
}  // namespace maple
