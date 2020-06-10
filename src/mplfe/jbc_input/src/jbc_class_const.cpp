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
#include "jbc_class_const.h"
#include <sstream>
#include "mpl_logging.h"
#include "jbc_class.h"
#include "jbc_util.h"
#include "fe_manager.h"

namespace maple {
namespace jbc {
// ---------- JBCConstTagName ----------
std::map<JBCConstTag, std::string> JBCConstTagName::tagNameMap = JBCConstTagName::InitTagNameMap();
std::map<JBCConstTag, std::string> JBCConstTagName::InitTagNameMap() {
  std::map<JBCConstTag, std::string> ret;
  ret[kConstUnknown] = "ConstUnknown";
#undef JBC_CONST
#define JBC_CONST(tag, tagName, className) \
  ret[tag] = tagName;
#include "jbc_class_const.def"
#undef JBC_CONST
  return ret;
}

std::string JBCConstTagName::GetTagName(JBCConstTag tag) {
  std::map<JBCConstTag, std::string>::const_iterator it = tagNameMap.find(tag);
  if (it != tagNameMap.end()) {
    return it->second;
  }
  std::stringstream ss;
  ss << "unknown tag (" << static_cast<uint32>(tag) << ")";
  return ss.str();
}

// ---------- JBCConst ----------
SimpleXMLElem *JBCConst::GenXMLElemImpl(MapleAllocator &alloc, uint32 id) const {
  return nullptr;
}

JBCConst *JBCConst::InConst(MapleAllocator &alloc, BasicIORead &io) {
  MemPool *mp = alloc.GetMemPool();
  ASSERT(mp, "mp is nullptr");
  JBCConst *constObj = nullptr;
  uint8 t = io.ReadUInt8();
  switch (t) {
#undef JBC_CONST
#define JBC_CONST(tag, tagName, ConstClassType) \
    case tag: \
      constObj = mp->New<ConstClassType>(alloc, static_cast<JBCConstTag>(t)); \
      if (constObj->ParseFile(io) == false) { \
        return nullptr; \
      } \
      break;
#include "jbc_class_const.def"
#undef JBC_CONST
    default:
      break;
  }
  return constObj;
}

std::string JBCConst::InternalNameToFullName(const std::string &name) {
  // ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.4.1
  if (name[0] == '[') {
    return name;
  } else {
    return "L" + name + ";";
  }
}

std::string JBCConst::FullNameToInternalName(const std::string &name) {
  if (name[0] == '[') {
    return name;
  } else {
    return name.substr(1, name.length() - 2); // 1 : start pos, name.length() - 2 : substr length
  }
}

// ---------- JBCConstUTF8 ----------
JBCConstUTF8::JBCConstUTF8(MapleAllocator &alloc, JBCConstTag t)
    : JBCConst(alloc, t), length(0), strIdx(0), str("", alloc.GetMemPool()) {}

JBCConstUTF8::JBCConstUTF8(MapleAllocator &alloc, JBCConstTag t, const std::string &argStr)
    : JBCConst(alloc, t), str(argStr, alloc.GetMemPool()) {
  CHECK_FATAL(t == kConstUTF8, "invalid tag");
  size_t rawLength = str.length();
  CHECK_FATAL(rawLength < UINT16_MAX, "input string is too long");
  length = static_cast<uint16>(rawLength);
  strIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(MapleStringToStd(str));
}

bool JBCConstUTF8::ParseFileImpl(BasicIORead &io) {
  bool success = false;
  length = io.ReadUInt16(success);
  str = io.ReadString(length, success);
  strIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(MapleStringToStd(str));
  return success;
}

bool JBCConstUTF8::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *JBCConstUTF8::GenXMLElemImpl(MapleAllocator &alloc, uint32 id) const {
  return nullptr;
}

// ---------- JBCConst4Byte ----------
JBCConst4Byte::JBCConst4Byte(MapleAllocator &alloc, JBCConstTag t) : JBCConst(alloc, t) {
  value.raw = 0;
}

JBCConst4Byte::JBCConst4Byte(MapleAllocator &alloc, JBCConstTag t, int32 arg) : JBCConst(alloc, t) {
  CHECK_FATAL(t == kConstInteger, "invalid tag");
  value.ivalue = arg;
}

JBCConst4Byte::JBCConst4Byte(MapleAllocator &alloc, JBCConstTag t, float arg) : JBCConst(alloc, t) {
  CHECK_FATAL(t == kConstFloat, "invalid tag");
  value.fvalue = arg;
}

bool JBCConst4Byte::ParseFileImpl(BasicIORead &io) {
  bool success = false;
  value.raw = io.ReadUInt32(success);
  return success;
}

bool JBCConst4Byte::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *JBCConst4Byte::GenXMLElemImpl(MapleAllocator &alloc, uint32 id) const {
  return nullptr;
}

// ---------- JBCConst8Byte ----------
JBCConst8Byte::JBCConst8Byte(MapleAllocator &alloc, JBCConstTag t) : JBCConst(alloc, t) {
  value.raw = 0;
}

JBCConst8Byte::JBCConst8Byte(MapleAllocator &alloc, JBCConstTag t, int64 arg) : JBCConst(alloc, t) {
  CHECK_FATAL(t == kConstLong, "invalid tag");
  value.lvalue = arg;
}

JBCConst8Byte::JBCConst8Byte(MapleAllocator &alloc, JBCConstTag t, double arg) : JBCConst(alloc, t) {
  CHECK_FATAL(t == kConstDouble, "invalid tag");
  value.dvalue = arg;
}

bool JBCConst8Byte::ParseFileImpl(BasicIORead &io) {
  bool success = false;
  value.raw = io.ReadUInt64(success);
  return success;
}

bool JBCConst8Byte::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *JBCConst8Byte::GenXMLElemImpl(MapleAllocator &alloc, uint32 id) const {
  return nullptr;
}

// ---------- JBCConstClass ----------
JBCConstClass::JBCConstClass(MapleAllocator &alloc, JBCConstTag t)
    : JBCConst(alloc, t),
      constUTF8(nullptr),
      strIdxOrin(0),
      strIdxMpl(0),
      nameOrin("", alloc.GetMemPool()),
      nameMpl("", alloc.GetMemPool()) {
  rawData.nameIdx = 0;
  feType = alloc.GetMemPool()->New<FEIRTypeDefault>(PTY_ref);
}

JBCConstClass::JBCConstClass(MapleAllocator &alloc, JBCConstTag t, JBCConstPoolIdx argNameIdx)
    : JBCConst(alloc, t),
      constUTF8(nullptr),
      strIdxOrin(0),
      strIdxMpl(0),
      nameOrin("", alloc.GetMemPool()),
      nameMpl("", alloc.GetMemPool()) {
  CHECK_FATAL(t == kConstClass, "invalid tag");
  rawData.nameIdx = argNameIdx;
  feType = alloc.GetMemPool()->New<FEIRTypeDefault>(PTY_ref);
}

JBCConstClass::~JBCConstClass() {
  constUTF8 = nullptr;
  feType = nullptr;
}

bool JBCConstClass::ParseFileImpl(BasicIORead &io) {
  bool success = false;
  rawData.nameIdx = io.ReadUInt16(success);
  return success;
}

bool JBCConstClass::PreProcessImpl(const JBCConstPool &constPool) {
  constUTF8 = static_cast<const JBCConstUTF8*>(constPool.GetConstByIdxWithTag(rawData.nameIdx, kConstUTF8));
  if (constUTF8 == nullptr) {
    return false;
  }
  const std::string &classNameInternal = constUTF8->GetString();
  nameOrin = JBCUtil::ClassInternalNameToFullName(classNameInternal);
  strIdxOrin = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(MapleStringToStd(nameOrin));
  nameMpl = namemangler::EncodeName(nameOrin.c_str());
  strIdxMpl = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(MapleStringToStd(nameMpl));
  static_cast<FEIRTypeDefault*>(feType)->LoadFromJavaTypeName(MapleStringToStd(nameMpl), true);
  return true;
}

SimpleXMLElem *JBCConstClass::GenXMLElemImpl(MapleAllocator &alloc, uint32 id) const {
  return nullptr;
}

// ---------- JBCConstString ----------
JBCConstString::JBCConstString(MapleAllocator &alloc, JBCConstTag t)
    : JBCConst(alloc, t), strIdx(0), str("", alloc.GetMemPool()) {
  rawData.stringIdx = 0;
}

JBCConstString::JBCConstString(MapleAllocator &alloc, JBCConstTag t, JBCConstPoolIdx argStringIdx)
    : JBCConst(alloc, t), strIdx(0), str("", alloc.GetMemPool()) {
  CHECK_FATAL(t == kConstString, "invalid tag");
  rawData.stringIdx = argStringIdx;
}

bool JBCConstString::ParseFileImpl(BasicIORead &io) {
  bool success = false;
  rawData.stringIdx = io.ReadUInt16(success);
  return success;
}

bool JBCConstString::PreProcessImpl(const JBCConstPool &constPool) {
  const JBCConstUTF8 *constUTF8 =
      static_cast<const JBCConstUTF8*>(constPool.GetConstByIdxWithTag(rawData.stringIdx, kConstUTF8));
  if (constUTF8 == nullptr) {
    return false;
  }
  strIdx = constUTF8->GetStrIdx();
  str = constUTF8->GetString();
  return true;
}

SimpleXMLElem *JBCConstString::GenXMLElemImpl(MapleAllocator &alloc, uint32 id) const {
  return nullptr;
}

// ---------- JBCConstRef ----------
JBCConstRef::JBCConstRef(MapleAllocator &alloc, JBCConstTag t)
    : JBCConst(alloc, t), constClass(nullptr), constNameAndType(nullptr), feStructElemInfo(nullptr) {
  rawData.classIdx = 0;
  rawData.nameAndTypeIdx = 0;
}

JBCConstRef::JBCConstRef(MapleAllocator &alloc, JBCConstTag t, JBCConstPoolIdx argClassIdx,
                         JBCConstPoolIdx argClassNameAndTypeIdx)
    : JBCConst(alloc, t), constClass(nullptr), constNameAndType(nullptr), feStructElemInfo(nullptr) {
  rawData.classIdx = argClassIdx;
  rawData.nameAndTypeIdx = argClassNameAndTypeIdx;
}

JBCConstRef::~JBCConstRef() {
  constClass = nullptr;
  constNameAndType = nullptr;
  feStructElemInfo = nullptr;
}

bool JBCConstRef::ParseFileImpl(BasicIORead &io) {
  bool success = false;
  rawData.classIdx = io.ReadUInt16(success);
  rawData.nameAndTypeIdx = io.ReadUInt16(success);
  return success;
}

bool JBCConstRef::PreProcessImpl(const JBCConstPool &constPool) {
  constClass = static_cast<const JBCConstClass*>(constPool.GetConstByIdxWithTag(rawData.classIdx, kConstClass));
  constNameAndType = static_cast<const JBCConstNameAndType*>(constPool.GetConstByIdxWithTag(rawData.nameAndTypeIdx,
                                                                                            kConstNameAndType));
  return (constClass != nullptr) && (constNameAndType != nullptr);
}

bool JBCConstRef::PrepareFEStructElemInfo() {
  CHECK_NULL_FATAL(constClass);
  CHECK_NULL_FATAL(constNameAndType);
  const std::string &className = constClass->GetClassNameOrin();
  const std::string &elemName = constNameAndType->GetName();
  const std::string &descName = constNameAndType->GetDesc();
  std::string fullName = namemangler::EncodeName(className + "|" + elemName + "|" + descName);
  GStrIdx fullNameIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(fullName);
  if (tag == kConstFieldRef) {
    feStructElemInfo = FEManager::GetTypeManager().RegisterStructFieldInfo(fullNameIdx, kSrcLangJava, false);
  } else {
    feStructElemInfo = FEManager::GetTypeManager().RegisterStructMethodInfo(fullNameIdx, kSrcLangJava, false);
  }
  return feStructElemInfo != nullptr;
}

SimpleXMLElem *JBCConstRef::GenXMLElemImpl(MapleAllocator &alloc, uint32 id) const {
  return nullptr;
}

const std::string JBCConstRef::GetName() const {
  CHECK_FATAL(constNameAndType != nullptr, "invalid const index");
  return constNameAndType->GetName();
}

const std::string JBCConstRef::GetDesc() const {
  CHECK_FATAL(constNameAndType != nullptr, "invalid const index");
  return constNameAndType->GetDesc();
}

// ---------- JBCConstNameAndType ----------
JBCConstNameAndType::JBCConstNameAndType(MapleAllocator &alloc, JBCConstTag t)
    : JBCConst(alloc, t), constName(nullptr), constDesc(nullptr) {
  rawData.nameIdx = 0;
  rawData.descIdx = 0;
}

JBCConstNameAndType::JBCConstNameAndType(MapleAllocator &alloc, JBCConstTag t, JBCConstPoolIdx argNameIdx,
                                         JBCConstPoolIdx argDescIdx)
    : JBCConst(alloc, t), constName(nullptr), constDesc(nullptr) {
  rawData.nameIdx = argNameIdx;
  rawData.descIdx = argDescIdx;
}

JBCConstNameAndType::~JBCConstNameAndType() {
  constName = nullptr;
  constDesc = nullptr;
}

bool JBCConstNameAndType::ParseFileImpl(BasicIORead &io) {
  bool success = false;
  rawData.nameIdx = io.ReadUInt16(success);
  rawData.descIdx = io.ReadUInt16(success);
  return success;
}

bool JBCConstNameAndType::PreProcessImpl(const JBCConstPool &constPool) {
  constName = static_cast<const JBCConstUTF8*>(constPool.GetConstByIdxWithTag(rawData.nameIdx, kConstUTF8));
  constDesc = static_cast<const JBCConstUTF8*>(constPool.GetConstByIdxWithTag(rawData.descIdx, kConstUTF8));
  return (constName != nullptr) && (constDesc != nullptr);
}

SimpleXMLElem *JBCConstNameAndType::GenXMLElemImpl(MapleAllocator &alloc, uint32 id) const {
  return nullptr;
}

// ---------- JBCConstMethodHandleInfo ----------
JBCConstMethodHandleInfo::JBCConstMethodHandleInfo(MapleAllocator &alloc, JBCConstTag t)
    : JBCConst(alloc, t), constRef(nullptr) {
  rawData.refKind = 0;
  rawData.refIdx = 0;
}

JBCConstMethodHandleInfo::~JBCConstMethodHandleInfo() {
  constRef = nullptr;
}

bool JBCConstMethodHandleInfo::ParseFileImpl(BasicIORead &io) {
  bool success = false;
  rawData.refKind = io.ReadUInt8(success);
  rawData.refIdx = io.ReadUInt16(success);
  return success;
}

bool JBCConstMethodHandleInfo::PreProcessImpl(const JBCConstPool &constPool) {
  constRef = nullptr;
  switch (rawData.refKind) {
    case kRefGetField:
    case kRefGetStatic:
    case kRefPutField:
    case kRefPutStatic:
      constRef = static_cast<const JBCConstRef*>(constPool.GetConstByIdxWithTag(rawData.refIdx, kConstFieldRef));
      break;
    case jbc::kRefInvokeVirtual:
    case jbc::kRefNewInvokeSpecial:
      constRef = static_cast<const JBCConstRef*>(constPool.GetConstByIdxWithTag(rawData.refIdx, kConstMethodRef));
      break;
    case jbc::kRefInvokeStatic:
    case jbc::kRefInvokeSpecial:
      if (constPool.GetMajorVersion() < 52) { // 52 : class file version number
        constRef = static_cast<const JBCConstRef*>(constPool.GetConstByIdxWithTag(rawData.refIdx, kConstMethodRef));
      } else {
        constRef = static_cast<const JBCConstRef*>(constPool.GetConstByIdx(rawData.refIdx));
        CHECK_NULL_FATAL(constRef);
        if (constRef->GetTag() != kConstMethodRef && constRef->GetTag() != kConstInterfaceMethodRef) {
          ERR(kLncErr, "Unexpected tag (%s) for const MethodHandle info. Expected %s or %s",
              JBCConstTagName::GetTagName(constRef->GetTag()).c_str(),
              JBCConstTagName::GetTagName(kConstMethodRef).c_str(),
              JBCConstTagName::GetTagName(kConstInterfaceMethodRef).c_str());
          return false;
        }
      }
      break;
    case jbc::kRefInvokeInterface:
      constRef = static_cast<const JBCConstRef*>(constPool.GetConstByIdxWithTag(rawData.refIdx,
                                                                                kConstInterfaceMethodRef));
      break;
    default:
      CHECK_FATAL(false, "Unsupported ref kind (%d)", rawData.refKind);
      break;
  }
  return constRef != nullptr;
}

SimpleXMLElem *JBCConstMethodHandleInfo::GenXMLElemImpl(MapleAllocator &alloc, uint32 id) const {
  return nullptr;
}

// ---------- JBCConstMethodType ----------
JBCConstMethodType::JBCConstMethodType(MapleAllocator &alloc, JBCConstTag t)
    : JBCConst(alloc, t), constDesc(nullptr) {
  rawData.descIdx = 0;
}

JBCConstMethodType::~JBCConstMethodType() {
  constDesc = nullptr;
}

bool JBCConstMethodType::ParseFileImpl(BasicIORead &io) {
  bool success = false;
  rawData.descIdx = io.ReadUInt16(success);
  return success;
}

bool JBCConstMethodType::PreProcessImpl(const JBCConstPool &constPool) {
  constDesc = static_cast<const JBCConstUTF8*>(constPool.GetConstByIdxWithTag(rawData.descIdx, kConstUTF8));
  return constDesc != nullptr;
}

SimpleXMLElem *JBCConstMethodType::GenXMLElemImpl(MapleAllocator &alloc, uint32 id) const {
  return nullptr;
}

// ---------- JBCConstInvokeDynamic ----------
JBCConstInvokeDynamic::JBCConstInvokeDynamic(MapleAllocator &alloc, JBCConstTag t)
    : JBCConst(alloc, t), constNameAndType(nullptr), feStructElemInfo(nullptr) {
  rawData.bsmAttrIdx = 0;
  rawData.nameAndTypeIdx = 0;
}

JBCConstInvokeDynamic::~JBCConstInvokeDynamic() {
  constNameAndType = nullptr;
  feStructElemInfo = nullptr;
}

bool JBCConstInvokeDynamic::PrepareFEStructElemInfo(const std::string &ownerClassName) {
  CHECK_NULL_FATAL(constNameAndType);
  const std::string &className = ownerClassName + "$DynamicCall$";
  const std::string &elemName = constNameAndType->GetName();
  const std::string &descName = constNameAndType->GetDesc();
  std::string fullName = namemangler::EncodeName(className + "|" + elemName + "|" + descName);
  GStrIdx fullNameIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(fullName);
  feStructElemInfo = FEManager::GetTypeManager().RegisterStructMethodInfo(fullNameIdx, kSrcLangJava, false);
  static_cast<FEStructMethodInfo*>(feStructElemInfo)->SetJavaDyamicCall();
  return feStructElemInfo != nullptr;
}

bool JBCConstInvokeDynamic::ParseFileImpl(BasicIORead &io) {
  bool success = false;
  rawData.bsmAttrIdx = io.ReadUInt16(success);
  rawData.nameAndTypeIdx = io.ReadUInt16(success);
  return success;
}

bool JBCConstInvokeDynamic::PreProcessImpl(const JBCConstPool &constPool) {
  constNameAndType =
      static_cast<const JBCConstNameAndType*>(constPool.GetConstByIdxWithTag(rawData.nameAndTypeIdx,
                                                                             kConstNameAndType));
  return constNameAndType != nullptr;
}

SimpleXMLElem *JBCConstInvokeDynamic::GenXMLElemImpl(MapleAllocator &alloc, uint32 id) const {
  return nullptr;
}
}  // namespace jbc
}  // namespace maple
