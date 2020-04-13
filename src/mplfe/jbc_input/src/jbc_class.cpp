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
#include "jbc_class.h"
#include <sstream>

namespace maple {
namespace {
const uint32 kMagicJVMClass = 0xCAFEBABE;
const uint32 kJavaClassNoIndex = 0;
}

namespace jbc {
// ---------- JBCClassElem ----------
JBCClassElem::JBCClassElem(MapleAllocator &allocator, const JBCClass &argKlass)
    : klass(argKlass),
      accessFlag(0),
      nameIdx(0),
      descIdx(0),
      nAttr(0),
      attrs(allocator.Adapter()),
      attrMap(allocator) {}

bool JBCClassElem::ParseFile(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) {
  bool success = false;
  accessFlag = io.ReadUInt16(success);
  nameIdx = io.ReadUInt16(success);
  descIdx = io.ReadUInt16(success);
  nAttr = io.ReadUInt16(success);
  for (uint16 i = 0; i < nAttr; i++) {
    JBCAttr *attr = JBCAttr::InAttr(allocator, io, constPool);
    if (attr == nullptr) {
      return false;
    }
    attrs.push_back(attr);
    attrMap.RegisterAttr(attr);
  }
  return true;
}

std::string JBCClassElem::GetClassName() const {
  return klass.GetClassNameOrin();
}

const JBCConstPool &JBCClassElem::GetConstPool() const {
  return klass.GetConstPool();
}

std::string JBCClassElem::GetFullName() const {
  std::stringstream ss;
  ss << GetClassName() << "|" << GetName() << "|" << GetDescription();
  return ss.str();
}

// ---------- JBCClassField ----------
JBCClassField::JBCClassField(MapleAllocator &allocator, const JBCClass &argKlass)
    : JBCClassElem(allocator, argKlass) {}

SimpleXMLElem *JBCClassField::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) {
  return nullptr;
}

// ---------- JBCClassMethod ----------
JBCClassMethod::JBCClassMethod(MapleAllocator &allocator, const JBCClass &argKlass)
    : JBCClassElem(allocator, argKlass) {}

SimpleXMLElem *JBCClassMethod::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) {
  return nullptr;
}

bool JBCClassMethod::PreProcess() {
  bool success = true;
  for (JBCAttr *attr : attrs) {
    if (attr->GetKind() == kAttrCode) {
      success = success && attr->PreProcess(klass.GetConstPool());
    }
  }
  return success;
}

const JBCAttrCode *JBCClassMethod::GetCode() const {
  return static_cast<const JBCAttrCode*>(attrMap.GetAttr(jbc::JBCAttrKind::kAttrCode));
}

// ---------- JBCClass ----------
JBCClass::JBCClass(MapleAllocator &allocatorIn)
    : allocator(allocatorIn),
      constPool(allocator),
      tbInterfaces(allocator.Adapter()),
      tbFields(allocator.Adapter()),
      tbMethods(allocator.Adapter()),
      tbAttrs(allocator.Adapter()),
      attrMap(allocator),
      filePathName("", allocator.GetMemPool()),
      fileName("", allocator.GetMemPool()) {
  InitHeader();
}

bool JBCClass::ParseFile(BasicIORead &io) {
  // begin parsing
  bool success = false;
  header.magic = io.ReadUInt32(success);
  if (header.magic != kMagicJVMClass) {
    ERR(kLncErr, "JBCClass::ParseFile() failed: invalid java class file (wrong magic number).");
    return false;
  }
  header.minorVersion = io.ReadUInt16(success);
  header.majorVersion = io.ReadUInt16(success);
  if (!ParseFileForConstPool(io)) {
    return false;
  }
  header.accessFlag = io.ReadUInt16(success);
  header.thisClass = io.ReadUInt16(success);
  header.superClass = io.ReadUInt16(success);
  header.interfacesCount = io.ReadUInt16(success);
  for (uint16 i = 0; i < header.interfacesCount && success; i++) {
    uint16 idx = io.ReadUInt16(success);
    tbInterfaces.push_back(idx);
  }
  if (!success) {
    return success;
  }
  if (!ParseFileForFields(io)) {
    return false;
  }
  if (!ParseFileForMethods(io)) {
    return false;
  }
  if (!ParseFileForAttrs(io)) {
    return false;
  }
  return true;
}

bool JBCClass::ParseFileForConstPool(BasicIORead &io) {
  bool success = false;
  bool wide = false;
  const uint8 kIdxOff = 1;
  const uint8 kIdxOffWide = 2;
  header.constPoolCount = io.ReadUInt16(success);
  for (uint16_t i = 1; i < header.constPoolCount && success; i += (wide ? kIdxOffWide : kIdxOff)) {
    JBCConst *objConst = JBCConst::InConst(allocator, io);
    if (objConst != nullptr) {
      (void)constPool.InsertConst(objConst);
      wide = objConst->IsWide();
      if (wide) {
        constPool.InsertConstDummyForWide();
      }
    } else {
      ERR(kLncErr, "JBCClass::ParseFile() failed: invalid const @idx=%d", i);
      success = false;
    }
  }
  return success;
}

bool JBCClass::ParseFileForFields(BasicIORead &io) {
  bool success = false;
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp, "mempool is nullptr");
  header.fieldsCount = io.ReadUInt16(success);
  for (uint16 i = 0; i < header.fieldsCount && success; i++) {
    JBCClassField *field = mp->New<JBCClassField>(allocator, *this);
    success = field->ParseFile(allocator, io, constPool);
    tbFields.push_back(field);
  }
  return success;
}

bool JBCClass::ParseFileForMethods(BasicIORead &io) {
  bool success = false;
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp, "mempool is nullptr");
  header.methodsCount = io.ReadUInt16(success);
  for (uint16 i = 0; i < header.methodsCount && success; i++) {
    JBCClassMethod *method = mp->New<JBCClassMethod>(allocator, *this);
    success = method->ParseFile(allocator, io, constPool);
    tbMethods.push_back(method);
  }
  return success;
}

bool JBCClass::ParseFileForAttrs(BasicIORead &io) {
  bool success = false;
  header.attrsCount = io.ReadUInt16(success);
  for (uint16 i = 0; i < header.attrsCount && success; i++) {
    JBCAttr *attr = JBCAttr::InAttr(allocator, io, constPool);
    if (attr == nullptr) {
      return false;
    }
    tbAttrs.push_back(attr);
    attrMap.RegisterAttr(attr);
  }
  return success;
}

bool JBCClass::PreProcess() {
  bool success = true;
  success = success && constPool.PreProcess(header.majorVersion);
  success = success && constPool.PrepareFEStructElemInfo(GetClassNameOrin());
  for (JBCClassMethod *method : tbMethods) {
    success = success && method->PreProcess();
  }
  for (JBCAttr *attr : tbAttrs) {
    if (attr->GetKind() == JBCAttrKind::kAttrSourceFile) {
      success = success && attr->PreProcess(constPool);
    }
  }
  return success;
}

GStrIdx JBCClass::GetClassNameIdxOrin() const {
  const JBCConstClass *constClass =
      static_cast<const JBCConstClass*>(constPool.GetConstByIdxWithTag(header.thisClass, kConstClass));
  if (constClass != nullptr) {
    return constClass->GetClassNameIdxOrin();
  } else {
    return GStrIdx(0);
  }
}

GStrIdx JBCClass::GetClassNameIdxMpl() const {
  const JBCConstClass *constClass =
      static_cast<const JBCConstClass*>(constPool.GetConstByIdxWithTag(header.thisClass, kConstClass));
  if (constClass != nullptr) {
    return constClass->GetClassNameIdxMpl();
  } else {
    return GStrIdx(0);
  }
}

std::string JBCClass::GetClassNameOrin() const {
  GStrIdx idx = GetClassNameIdxOrin();
  if (idx.GetIdx() == 0) {
    return "";
  } else {
    return GlobalTables::GetStrTable().GetStringFromStrIdx(idx);
  }
}

std::string JBCClass::GetClassNameMpl() const {
  GStrIdx idx = GetClassNameIdxMpl();
  if (idx.GetIdx() == 0) {
    return "";
  } else {
    return GlobalTables::GetStrTable().GetStringFromStrIdx(idx);
  }
}

std::string JBCClass::GetSourceFileName() const {
  return "";
}

std::string JBCClass::GetSuperClassName() const {
  if (header.superClass == kJavaClassNoIndex) {
    return "";
  } else {
    return constPool.GetNameByClassInfoIdx(header.superClass);
  }
}

std::vector<std::string> JBCClass::GetInterfaceNames() const {
  std::vector<std::string> results;
  for (uint16 idx : tbInterfaces) {
    if (idx != kJavaClassNoIndex) {
      std::string name = constPool.GetNameByClassInfoIdx(idx);
      results.push_back(name);
    }
  }
  return results;
}

JBCClass *JBCClass::InClass(MapleAllocator &allocator, BasicIORead &io) {
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp, "mempool is nullptr");
  JBCClass *klass = mp->New<JBCClass>(allocator);
  if (klass->ParseFile(io) == false) {
    return nullptr;
  }
  if (klass->PreProcess() == false) {
    return nullptr;
  }
  // update fileName
  const JBCAttr *attrRaw = klass->attrMap.GetAttr(JBCAttrKind::kAttrSourceFile);
  if (attrRaw == nullptr) {
    klass->fileName = "unknown";
  } else {
    const JBCAttrSourceFile *attrSourceFile = static_cast<const JBCAttrSourceFile*>(attrRaw);
    const JBCConstUTF8 *constName = attrSourceFile->GetConstFileName();
    klass->fileName = constName->GetString();
  }
  return klass;
}

void JBCClass::InitHeader() {
  header.magic = 0;
  header.minorVersion = 0;
  header.majorVersion = 0;
  header.constPoolCount = 0;
  header.accessFlag = 0;
  header.thisClass = 0;
  header.superClass = 0;
  header.interfacesCount = 0;
  header.fieldsCount = 0;
  header.methodsCount = 0;
  header.attrsCount = 0;
}
}  // namespace jbc
}  // namespace maple
