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
#include "jbc_attr.h"
#include "jbc_attr_item.h"

namespace maple {
namespace jbc {
// ---------- JBCAttr ----------
JBCAttr::JBCAttr(JBCAttrKind kindIn, uint16 nameIdxIn, uint32 lengthIn)
    : kind(kindIn), nameIdx(nameIdxIn), length(lengthIn) {}

JBCAttr* JBCAttr::InAttr(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) {
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp, "mempool is nullptr");
  JBCAttr *attrInfo = nullptr;
  uint16 nameIdx = io.ReadUInt16();
  uint32 length = io.ReadUInt32();
  uint32 posStart = io.GetPos();
  const JBCConst *constNameRaw = constPool.GetConstByIdxWithTag(nameIdx, kConstUTF8);
  if (constNameRaw == nullptr) {
    ERR(kLncErr, "invalid nameIdx %d for attr name.", nameIdx);
    return nullptr;
  }
  const JBCConstUTF8 *constName = static_cast<const JBCConstUTF8*>(constNameRaw);
  std::string strName = constName->GetString();
  if (strName.empty()) {
    ERR(kLncErr, "AttrInfo@0x%x parse error: empty attr name.", posStart);
#define JBC_ATTR(name, Type)                             \
  } else if (strName.compare(name) == 0) {               \
    attrInfo = mp->New<JBCAttr##Type>(allocator, nameIdx, length);  \
    attrInfo->ParseFile(allocator, io, constPool);
#include "jbc_attr.def"
#undef JBC_ATTR
  } else {
    attrInfo = mp->New<JBCAttrRaw>(allocator, nameIdx, length);
    if (!attrInfo->ParseFile(allocator, io, constPool)) {
      CHECK_FATAL(false, "failed to parse attr info");
    }
  }
  if (io.GetPos() - posStart != length) {
    ERR(kLncErr, "AttrInfo@0x%x parse error: incorrect data length.", posStart);
    io.SetPos(posStart + length);
  }
  return attrInfo;
}

JBCAttrKind JBCAttr::AttrKind(const std::string &str) {
#define JBC_ATTR(name, type)        \
  if (str.compare(name) == 0) {     \
    return kAttr##type;        \
  }
#include "jbc_attr.def"
#undef JBC_ATTR
  return kAttrUnknown;
}

// ---------- JBCAttrMap ----------
JBCAttrMap::JBCAttrMap(MapleAllocator &allocatorIn)
    : allocator(allocatorIn), mapAttrs(std::less<JBCAttrKind>(), allocator.Adapter()) {}

void JBCAttrMap::RegisterAttr(JBCAttr *attr) {
  CHECK_FATAL(attr != nullptr, "input attr is nullptr");
  JBCAttrKind kind = attr->GetKind();
  MapleMap<JBCAttrKind, MapleList<JBCAttr*>*>::const_iterator it = mapAttrs.find(kind);
  if (it == mapAttrs.end()) {
    MemPool *mp = allocator.GetMemPool();
    MapleList<JBCAttr*> *attrList = mp->New<MapleList<JBCAttr*>>(allocator.Adapter());
    attrList->push_back(attr);
    CHECK_FATAL(mapAttrs.insert(std::make_pair(kind, attrList)).second, "mapAttrs insert error");
  } else {
    it->second->push_back(attr);
  }
}

std::list<JBCAttr*> JBCAttrMap::GetAttrs(JBCAttrKind kind) const {
  auto it = mapAttrs.find(kind);
  if (it == mapAttrs.end()) {
    return std::list<JBCAttr*>();
  } else {
    std::list<JBCAttr*> ans;
    for (JBCAttr *attr : *(it->second)) {
      ans.push_back(attr);
    }
    return ans;
  }
}

const JBCAttr *JBCAttrMap::GetAttr(JBCAttrKind kind) const {
  auto it = mapAttrs.find(kind);
  if (it == mapAttrs.end()) {
    return nullptr;
  } else {
    CHECK_FATAL(it->second->size() == 1, "more than one attrs");
    return *(it->second->begin());
  }
}

bool JBCAttrMap::PreProcess(const JBCConstPool &constPool) {
  bool success = true;
  for (auto itList : mapAttrs) {
    for (JBCAttr *attr : *(itList.second)) {
      switch (attr->GetKind()) {
        case kAttrLocalVariableTable:
        case kAttrLocalVariableTypeTable:
          success = success && attr->PreProcess(constPool);
          break;
        default:
          break;
      }
    }
  }
  return success;
}

// ---------- JBCAttrRaw ----------
JBCAttrRaw::JBCAttrRaw(MapleAllocator &allocator, uint16 nameIdx, uint32 length)
    : JBCAttr(kAttrRaw, nameIdx, length),
      rawData(nullptr) {}

JBCAttrRaw::~JBCAttrRaw() {
  rawData = nullptr;
}

bool JBCAttrRaw::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) {
  bool success = false;
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp, "mempool is nullptr");
  rawData = static_cast<uint8*>(mp->Malloc(length));
  CHECK_NULL_FATAL(rawData);
  io.ReadBufferUInt8(rawData, length, success);
  return success;
}

bool JBCAttrRaw::PreProcessImpl(const JBCConstPool &constPool) {
  return false;
}

SimpleXMLElem *JBCAttrRaw::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const {
  return nullptr;
}

// ---------- JBCAttrLocalVariableInfo ----------
JavaAttrLocalVariableInfoItem JBCAttrLocalVariableInfo::kInvalidInfoItem;

JBCAttrLocalVariableInfo::JBCAttrLocalVariableInfo(MapleAllocator &argAllocator)
    : allocator(argAllocator),
      slotStartMap(allocator.Adapter()),
      itemMap(allocator.Adapter()) {}

void JBCAttrLocalVariableInfo::RegisterItem(const attr::LocalVariableTableItem &itemAttr) {
  uint16 slotIdx = itemAttr.GetIndex();
  uint16 startPC = itemAttr.GetStartPC();
  uint16 length = itemAttr.GetLength();
  JavaAttrLocalVariableInfoItem *item = GetItemByStartInternal(slotIdx, startPC);
  if (item == nullptr) {
    CheckItemAvaiable(slotIdx, startPC);
    AddSlotStartMap(slotIdx, startPC);
    JavaAttrLocalVariableInfoItem &itemRef = itemMap[std::make_pair(slotIdx, startPC)];
    itemRef.slotIdx = slotIdx;
    itemRef.start = startPC;
    itemRef.length = length;
    itemRef.nameIdx = itemAttr.GetNameStrIdx();
    itemRef.feirType = itemAttr.GetFEIRType();
  } else {
    if (item->start == startPC && item->length == length && item->nameIdx == itemAttr.GetNameStrIdx()) {
      CHECK_FATAL(item->feirType == nullptr, "Item already defined");
      item->feirType = itemAttr.GetFEIRType();
    } else {
      CHECK_FATAL(false, "Item mismatch in RegisterItem()");
    }
  }
}

void JBCAttrLocalVariableInfo::RegisterTypeItem(const attr::LocalVariableTypeTableItem &itemAttr) {
  uint16 slotIdx = itemAttr.GetIndex();
  uint16 startPC = itemAttr.GetStartPC();
  uint16 length = itemAttr.GetLength();
  JavaAttrLocalVariableInfoItem *item = GetItemByStartInternal(slotIdx, startPC);
  if (item == nullptr) {
    CheckItemAvaiable(slotIdx, startPC);
    AddSlotStartMap(slotIdx, startPC);
    JavaAttrLocalVariableInfoItem &itemRef = itemMap[std::make_pair(slotIdx, startPC)];
    itemRef.slotIdx = slotIdx;
    itemRef.start = startPC;
    itemRef.length = length;
    itemRef.nameIdx = itemAttr.GetNameStrIdx();
    itemRef.signatureNameIdx = itemAttr.GetSignatureStrIdx();
  } else {
    if (item->start == startPC && item->length == length && item->nameIdx == itemAttr.GetNameStrIdx()) {
      CHECK_FATAL(item->signatureNameIdx == 0, "Item already defined");
      item->signatureNameIdx = itemAttr.GetSignatureStrIdx();
    } else {
      CHECK_FATAL(false, "Item mismatch in RegisterItem()");
    }
  }
}

const JavaAttrLocalVariableInfoItem &JBCAttrLocalVariableInfo::GetItemByStart(uint16 slotIdx, uint16 start) const {
  uint32 itemPCStart = GetStart(slotIdx, start);
  if (itemPCStart != start) {
    return kInvalidInfoItem;
  }
  std::map<std::pair<uint16, uint16>, JavaAttrLocalVariableInfoItem>::const_iterator it =
      itemMap.find(std::make_pair(slotIdx, itemPCStart));
  if (it != itemMap.end()) {
    return it->second;
  } else {
    return kInvalidInfoItem;
  }
}

JavaAttrLocalVariableInfoItem *JBCAttrLocalVariableInfo::GetItemByStartInternal(uint16 slotIdx, uint16 start) {
  uint32 itemPCStart = GetStart(slotIdx, start);
  if (itemPCStart != start) {
    return nullptr;
  }
  std::map<std::pair<uint16, uint16>, JavaAttrLocalVariableInfoItem>::iterator it =
      itemMap.find(std::make_pair(slotIdx, itemPCStart));
  CHECK_FATAL(it != itemMap.end(), "Item@%d not found", start);
  return &(it->second);
}

uint16 JBCAttrLocalVariableInfo::GetStart(uint16 slotIdx, uint16 pc) const {
  MapleMap<uint16, MapleSet<uint16>>::const_iterator it = slotStartMap.find(slotIdx);
  if (it == slotStartMap.end()) {
    return jbc::kInvalidPC16;
  }
  uint16 startLast = jbc::kInvalidPC16;
  for (uint16 start : it->second) {
    if (pc == start) {
      return start;
    } else if (pc < start) {
      return startLast;
    }
    startLast = start;
  }
  return startLast;
}

std::list<std::string> JBCAttrLocalVariableInfo::EmitToStrings() const {
  std::list<std::string> ans;
  std::stringstream ss;
  ans.emplace_back("===== Local Variable Info =====");
  for (const std::pair<std::pair<uint16, uint16>, JavaAttrLocalVariableInfoItem> &itemPair : itemMap) {
    const JavaAttrLocalVariableInfoItem &item = itemPair.second;
    ss.str("");
    ss << "slot[" << item.slotIdx << "]: ";
    ss << "start=" << item.start << ", ";
    ss << "lenght=" << item.length << ", ";
    ss << "name=\'" << GlobalTables::GetStrTable().GetStringFromStrIdx(item.nameIdx) << "\', ";
    ss << "type=\'" << item.feirType->GetTypeName() << "\', ";
    ss << "signature=\'" << GlobalTables::GetStrTable().GetStringFromStrIdx(item.signatureNameIdx) << "\'";
    ans.push_back(ss.str());
  }
  ans.emplace_back("===============================");
  return ans;
}

bool JBCAttrLocalVariableInfo::IsInvalidLocalVariableInfoItem(const JavaAttrLocalVariableInfoItem &item) {
  return (item.nameIdx == 0 && item.feirType == nullptr && item.signatureNameIdx == 0);
}

void JBCAttrLocalVariableInfo::AddSlotStartMap(uint16 slotIdx, uint16 startPC) {
  auto it = slotStartMap.find(slotIdx);
  if (it == slotStartMap.end()) {
    MapleSet<uint16> startSet(allocator.Adapter());
    CHECK_FATAL(startSet.insert(startPC).second, "insert failed");
    CHECK_FATAL(slotStartMap.insert(std::make_pair(slotIdx, startSet)).second, "insert failed");
  } else {
    CHECK_FATAL(it->second.insert(startPC).second, "insert failed");
  }
}

void JBCAttrLocalVariableInfo::CheckItemAvaiable(uint16 slotIdx, uint16 start) const {
  uint32 itemPCStart = GetStart(slotIdx, start);
  if (itemPCStart == jbc::kInvalidPC16) {
    return;
  }
  CHECK_FATAL(itemPCStart <= jbc::kMaxPC32, "Invalid PC");
  uint16 itemPCStart16 = static_cast<uint16>(itemPCStart);
  const JavaAttrLocalVariableInfoItem &item = GetItemByStart(slotIdx, itemPCStart16);
  CHECK_FATAL(!JBCAttrLocalVariableInfo::IsInvalidLocalVariableInfoItem(item), "Item@%d not found", itemPCStart16);
  CHECK_FATAL(start >= item.start + item.length, "PC range overlapped");
}

// ---------- JBCAttrConstantValue ----------
JBCAttrConstantValue::JBCAttrConstantValue(MapleAllocator &allocator, uint16 nameIdx, uint32 length)
    : JBCAttr(kAttrConstantValue, nameIdx, length),
      constIdx(0),
      constValue(nullptr) {}

JBCAttrConstantValue::~JBCAttrConstantValue() {
  constValue = nullptr;
}

bool JBCAttrConstantValue::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) {
  bool success = false;
  constIdx = io.ReadUInt16(success);
  return success;
}

bool JBCAttrConstantValue::PreProcessImpl(const JBCConstPool &constPool) {
  return false;
}

SimpleXMLElem *JBCAttrConstantValue::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                    uint32 idx) const {
  return nullptr;
}

// ---------- JBCAttrCode ----------
JBCAttrCode::JBCAttrCode(MapleAllocator &allocator, uint16 nameIdx, uint32 length)
    : JBCAttr(kAttrCode, nameIdx, length),
      maxStack(0),
      maxLocals(0),
      codeLength(0),
      code(nullptr),
      nException(0),
      exceptions(allocator.Adapter()),
      nAttr(0),
      attrs(allocator.Adapter()),
      instructions(allocator.Adapter()),
      attrMap(allocator),
      localVarInfo(allocator) {}

JBCAttrCode::~JBCAttrCode() {
  code = nullptr;
}

bool JBCAttrCode::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) {
  bool success = false;
  MemPool *mp = allocator.GetMemPool();
  CHECK_FATAL(mp, "mempool is nullptr");
  maxStack = io.ReadUInt16(success);
  maxLocals = io.ReadUInt16(success);
  codeLength = io.ReadUInt32(success);
  code = static_cast<uint8*>(mp->Malloc(codeLength));
  CHECK_NULL_FATAL(code);
  io.ReadBufferUInt8(code, codeLength, success);
  nException = io.ReadUInt16(success);
  for (uint16 i = 0; i < nException; i++) {
    attr::ExceptionTableItem *item = mp->New<attr::ExceptionTableItem>();
    success = item->ParseFile(allocator, io);
    exceptions.push_back(item);
  }
  nAttr = io.ReadUInt16(success);
  for (uint16 i = 0; i < nAttr; i++) {
    JBCAttr *attr = nullptr;
    attr = JBCAttr::InAttr(allocator, io, constPool);
    if (attr == nullptr) {
      return false;
    }
    attrs.push_back(attr);
    attrMap.RegisterAttr(attr);
  }
  // ParseOpcode
  success = ParseOpcodes(allocator);
  return success;
}

bool JBCAttrCode::PreProcessImpl(const JBCConstPool &constPool) {
  bool success = true;
  for (attr::ExceptionTableItem *item : exceptions) {
    success = success && item->PreProcess(constPool);
  }
  success = success && attrMap.PreProcess(constPool);
  InitLocalVarInfo();
  return success;
}

SimpleXMLElem *JBCAttrCode::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const {
  return nullptr;
}

// Remove const when implemented
void JBCAttrCode::InitLocalVarInfo() {
  // LocalVariableTable
  std::list<JBCAttr*> localVars = attrMap.GetAttrs(jbc::kAttrLocalVariableTable);
  for (JBCAttr *attrRaw : localVars) {
    CHECK_NULL_FATAL(attrRaw);
    JBCAttrLocalVariableTable *attr = static_cast<JBCAttrLocalVariableTable*>(attrRaw);
    for (attr::LocalVariableTableItem *itemAttr : attr->GetLocalVarInfos()) {
      localVarInfo.RegisterItem(*itemAttr);
    }
  }
  // LocalVariableTypeTable
  std::list<JBCAttr*> localVarTypes = attrMap.GetAttrs(jbc::kAttrLocalVariableTypeTable);
  for (JBCAttr *attrRaw : localVarTypes) {
    CHECK_NULL_FATAL(attrRaw);
    JBCAttrLocalVariableTypeTable *attr = static_cast<JBCAttrLocalVariableTypeTable*>(attrRaw);
    for (attr::LocalVariableTypeTableItem *itemAttr : attr->GetLocalVarTypeInfos()) {
      localVarInfo.RegisterTypeItem(*itemAttr);
    }
  }
}

// Remove const when implemented
void JBCAttrCode::SetLoadStoreType() const {
}

bool JBCAttrCode::ParseOpcodes(MapleAllocator &allocator) {
  BasicIOMapFile file("code", code, codeLength);
  BasicIORead io(file, true);
  bool success = true;
  bool wide = false;
  JBCOp *objOP = nullptr;
  uint32 pc = 0;
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp, "mempool is nullptr");
  while (io.GetPos() < codeLength && success) {
    if (wide == false) {
      pc = io.GetPos();
    }
    JBCOpcode opcode = static_cast<JBCOpcode>(io.ReadUInt8(success));
    if (opcode == kOpWide) {
      wide = true;
      continue;
    }
    JBCOpcodeKind kind = JBCOp::GetOpcodeInfo().GetOpcodeKind(opcode);
    switch (kind) {
      case kOpKindArrayLoad:
      case kOpKindArrayStore:
      case kOpKindPop:
      case kOpKindDup:
      case kOpKindSwap:
      case kOpKindStack:
      case kOpKindMathBinop:
      case kOpKindMathUnop:
      case kOpKindConvert:
      case kOpKindCompare:
      case kOpKindReturn:
      case kOpKindThrow:
      case kOpKindMonitor:
      case kOpKindArrayLength:
        objOP = mp->New<JBCOpDefault>(allocator, opcode, kind, wide);
        break;
      case kOpKindUnused:
        objOP = mp->New<JBCOpUnused>(allocator, opcode, kind, wide);
        break;
      case kOpKindReversed:
        objOP = mp->New<JBCOpReversed>(allocator, opcode, kind, wide);
        break;
      case kOpKindConst:
        objOP = mp->New<JBCOpConst>(allocator, opcode, kind, wide);
        break;
      case kOpKindLoad:
      case kOpKindStore:
        objOP = mp->New<JBCOpSlotOpr>(allocator, opcode, kind, wide);
        break;
      case kOpKindMathInc:
        objOP = mp->New<JBCOpMathInc>(allocator, opcode, kind, wide);
        break;
      case kOpKindBranch:
        objOP = mp->New<JBCOpBranch>(allocator, opcode, kind, wide);
        break;
      case kOpKindGoto:
        objOP = mp->New<JBCOpGoto>(allocator, opcode, kind, wide);
        break;
      case kOpKindSwitch:
        objOP = mp->New<JBCOpSwitch>(allocator, opcode, kind, wide);
        break;
      case kOpKindStaticFieldOpr:
      case kOpKindFieldOpr:
        objOP = mp->New<JBCOpFieldOpr>(allocator, opcode, kind, wide);
        break;
      case kOpKindInvoke:
        objOP = mp->New<JBCOpInvoke>(allocator, opcode, kind, wide);
        break;
      case kOpKindJsr:
        objOP = mp->New<JBCOpJsr>(allocator, opcode, kind, wide);
        break;
      case kOpKindRet:
        objOP = mp->New<JBCOpRet>(allocator, opcode, kind, wide);
        break;
      case kOpKindNew:
        objOP = mp->New<JBCOpNew>(allocator, opcode, kind, wide);
        break;
      case kOpKindMultiANewArray:
        objOP = mp->New<JBCOpMultiANewArray>(allocator, opcode, kind, wide);
        break;
      case kOpKindTypeCheck:
        objOP = mp->New<JBCOpTypeCheck>(allocator, opcode, kind, wide);
        break;
      default:
        CHECK_NULL_FATAL(objOP);
        break;
    }
    wide = false;
    success = objOP->ParseFile(io);
    if (success) {
      instructions[pc] = objOP;
    }
  }
  return success;
}

// ---------- JBCAttrStackMapTable ----------
JBCAttrStackMapTable::JBCAttrStackMapTable(MapleAllocator &allocator, uint16 nameIdx, uint32 length)
    : JBCAttr(kAttrStackMapTable, nameIdx, length),
      count(0),
      entries(allocator.Adapter()) {}

bool JBCAttrStackMapTable::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) {
  bool success = false;
  count = io.ReadUInt16(success);
  for (uint16 i = 0; i < count; i++) {
    uint8 frameType = io.ReadUInt8();
    attr::StackMapFrameItem *item = attr::StackMapFrameItem::NewItem(allocator, io, frameType);
    if (item == nullptr) {
      return false;
    }
    entries.push_back(item);
  }
  return success;
}

bool JBCAttrStackMapTable::PreProcessImpl(const JBCConstPool &constPool) {
  return false;
}

SimpleXMLElem *JBCAttrStackMapTable::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                    uint32 idx) const {
  return nullptr;
}

// ---------- JBCAttrException ----------
JBCAttrException::JBCAttrException(MapleAllocator &allocator, uint16 nameIdx, uint32 length)
    : JBCAttr(kAttrException, nameIdx, length),
      count(0),
      tbExceptionIdx(allocator.Adapter()) {}

bool JBCAttrException::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) {
  bool success = false;
  count = io.ReadUInt16(success);
  for (uint16 i = 0; i < count; i++) {
    uint16 idx = io.ReadUInt16(success);
    tbExceptionIdx.push_back(idx);
  }
  return success;
}

bool JBCAttrException::PreProcessImpl(const JBCConstPool &constPool) {
  return false;
}

SimpleXMLElem *JBCAttrException::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                uint32 idx) const {
  return nullptr;
}

// ---------- JBCAttrInnerClass ----------
JBCAttrInnerClass::JBCAttrInnerClass(MapleAllocator &allocator, uint16 nameIdx, uint32 length)
    : JBCAttr(kAttrInnerClass, nameIdx, length),
      count(0),
      tbClasses(allocator.Adapter()) {}

bool JBCAttrInnerClass::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) {
  bool success = false;
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp, "mempool is nullptr");
  count = io.ReadUInt16(success);
  for (uint16 i = 0; i < count; i++) {
    attr::InnerClassItem *item = mp->New<attr::InnerClassItem>();
    success = item->ParseFile(allocator, io);
    tbClasses.push_back(item);
  }
  return success;
}

bool JBCAttrInnerClass::PreProcessImpl(const JBCConstPool &constPool) {
  return false;
}

SimpleXMLElem *JBCAttrInnerClass::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                 uint32 idx) const {
  return nullptr;
}

// ---------- JBCAttrEnclosingMethod ----------
JBCAttrEnclosingMethod::JBCAttrEnclosingMethod(MapleAllocator &allocator, uint16 nameIdx, uint32 length)
    : JBCAttr(kAttrEnclosingMethod, nameIdx, length),
      classIdx(0),
      methodIdx(0),
      constClass(nullptr),
      constNameAndType(nullptr) {}

JBCAttrEnclosingMethod::~JBCAttrEnclosingMethod() {
  constClass = nullptr;
  constNameAndType = nullptr;
}

bool JBCAttrEnclosingMethod::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) {
  bool success = false;
  classIdx = io.ReadUInt16(success);
  methodIdx = io.ReadUInt16(success);
  return success;
}

bool JBCAttrEnclosingMethod::PreProcessImpl(const JBCConstPool &constPool) {
  return false;
}

SimpleXMLElem *JBCAttrEnclosingMethod::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                      uint32 idx) const {
  return nullptr;
}

// ---------- JBCAttrSynthetic ----------
JBCAttrSynthetic::JBCAttrSynthetic(MapleAllocator &allocator, uint16 nameIdx, uint32 length)
    : JBCAttr(kAttrSynthetic, nameIdx, length) {}

bool JBCAttrSynthetic::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) {
  return true;
}

bool JBCAttrSynthetic::PreProcessImpl(const JBCConstPool &constPool) {
  return false;
}

SimpleXMLElem *JBCAttrSynthetic::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                uint32 idx) const {
  return nullptr;
}

// ---------- JBCAttrSignature ----------
JBCAttrSignature::JBCAttrSignature(MapleAllocator &allocator, uint16 nameIdx, uint32 length)
    : JBCAttr(kAttrSignature, nameIdx, length),
      signatureIdx(0),
      constSignatureName(nullptr) {}

JBCAttrSignature::~JBCAttrSignature() {
  constSignatureName = nullptr;
}

bool JBCAttrSignature::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) {
  bool success = false;
  signatureIdx = io.ReadUInt16(success);
  return success;
}

bool JBCAttrSignature::PreProcessImpl(const JBCConstPool &constPool) {
  return false;
}

SimpleXMLElem *JBCAttrSignature::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                uint32 idx) const {
  return nullptr;
}

// ---------- JBCAttrSourceFile ----------
JBCAttrSourceFile::JBCAttrSourceFile(MapleAllocator &allocator, uint16 nameIdx, uint32 length)
    : JBCAttr(kAttrSourceFile, nameIdx, length),
      sourceFileIdx(0),
      constFileName(nullptr) {}

JBCAttrSourceFile::~JBCAttrSourceFile() {
  constFileName = nullptr;
}

bool JBCAttrSourceFile::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) {
  bool success = false;
  sourceFileIdx = io.ReadUInt16(success);
  return success;
}

bool JBCAttrSourceFile::PreProcessImpl(const JBCConstPool &constPool) {
  const JBCConst *constRaw = constPool.GetConstByIdxWithTag(sourceFileIdx, JBCConstTag::kConstUTF8);
  if (constRaw == nullptr) {
    return false;
  }
  constFileName = static_cast<const JBCConstUTF8*>(constRaw);
  return true;
}

SimpleXMLElem *JBCAttrSourceFile::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                 uint32 idx) const {
  return nullptr;
}

// ---------- JBCAttrSourceDebugEx ----------
JBCAttrSourceDebugEx::JBCAttrSourceDebugEx(MapleAllocator &allocator, uint16 nameIdx, uint32 length)
    : JBCAttr(kAttrSourceDebugEx, nameIdx, length),
      data(nullptr) {}

JBCAttrSourceDebugEx::~JBCAttrSourceDebugEx() {
  data = nullptr;
}

bool JBCAttrSourceDebugEx::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) {
  bool success = false;
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp, "mempool is nullptr");
  if (length > 0) {
    data = static_cast<char*>(mp->Malloc(length + 1));
    CHECK_NULL_FATAL(data);
    io.ReadBufferChar(data, length, success);
    data[length] = 0;
  }
  return success;
}

bool JBCAttrSourceDebugEx::PreProcessImpl(const JBCConstPool &constPool) {
  return false;
}

SimpleXMLElem *JBCAttrSourceDebugEx::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                    uint32 idx) const {
  return nullptr;
}

// ---------- JBCAttrLineNumberTable ----------
JBCAttrLineNumberTable::JBCAttrLineNumberTable(MapleAllocator &allocator, uint16 nameIdx, uint32 length)
    : JBCAttr(kAttrLineNumberTable, nameIdx, length),
      size(0),
      lineNums(allocator.Adapter()) {}

bool JBCAttrLineNumberTable::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) {
  bool success = false;
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp, "mempool is nullptr");
  size = io.ReadUInt16(success);
  for (uint16 i = 0; i < size && success; i++) {
    attr::LineNumberTableItem *item = mp->New<attr::LineNumberTableItem>();
    success = item->ParseFile(allocator, io);
    lineNums.push_back(item);
  }
  return success;
}

bool JBCAttrLineNumberTable::PreProcessImpl(const JBCConstPool &constPool) {
  return false;
}

SimpleXMLElem *JBCAttrLineNumberTable::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                      uint32 idx) const {
  return nullptr;
}

// ---------- JBCAttrLocalVariableTable ----------
JBCAttrLocalVariableTable::JBCAttrLocalVariableTable(MapleAllocator &allocator, uint16 nameIdx, uint32 length)
    : JBCAttr(kAttrLocalVariableTable, nameIdx, length), size(0), localVarInfos(allocator.Adapter()) {}

bool JBCAttrLocalVariableTable::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io,
                                              const JBCConstPool &constPool) {
  bool success = false;
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp, "mempool is nullptr");
  size = io.ReadUInt16(success);
  for (uint16 i = 0; i < size && success; i++) {
    attr::LocalVariableTableItem *item = mp->New<attr::LocalVariableTableItem>();
    success = item->ParseFile(allocator, io);
    localVarInfos.push_back(item);
  }
  return success;
}

bool JBCAttrLocalVariableTable::PreProcessImpl(const JBCConstPool &constPool) {
  bool success = true;
  for (attr::LocalVariableTableItem *info : localVarInfos) {
    success = success && info->PreProcess(constPool);
  }
  return success;
}

SimpleXMLElem *JBCAttrLocalVariableTable::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                         uint32 idx) const {
  return nullptr;
}

// ---------- JBCAttrLocalVariableTypeTable ----------
JBCAttrLocalVariableTypeTable::JBCAttrLocalVariableTypeTable(MapleAllocator &allocator, uint16 nameIdx, uint32 length)
    : JBCAttr(kAttrLocalVariableTypeTable, nameIdx, length), size(0), localVarTypeInfos(allocator.Adapter()) {}

bool JBCAttrLocalVariableTypeTable::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io,
                                                  const JBCConstPool &constPool) {
  bool success = false;
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp, "mempool is nullptr");
  size = io.ReadUInt16(success);
  for (uint16 i = 0; i < size && success; i++) {
    attr::LocalVariableTypeTableItem *item = mp->New<attr::LocalVariableTypeTableItem>();
    success = item->ParseFile(allocator, io);
    localVarTypeInfos.push_back(item);
  }
  return success;
}

bool JBCAttrLocalVariableTypeTable::PreProcessImpl(const JBCConstPool &constPool) {
  bool success = true;
  for (attr::LocalVariableTypeTableItem *info : localVarTypeInfos) {
    success = success && info->PreProcess(constPool);
  }
  return success;
}

SimpleXMLElem *JBCAttrLocalVariableTypeTable::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                             uint32 idx) const {
  return nullptr;
}

// ---------- JBCAttrDeprecated ----------
JBCAttrDeprecated::JBCAttrDeprecated(MapleAllocator &allocator, uint16 nameIdx, uint32 length)
    : JBCAttr(kAttrDeprecated, nameIdx, length) {}

bool JBCAttrDeprecated::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) {
  return true;
}

bool JBCAttrDeprecated::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *JBCAttrDeprecated::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                 uint32 idx) const {
  return nullptr;
}

// ---------- JBCAttrRTAnnotations ----------
JBCAttrRTAnnotations::JBCAttrRTAnnotations(MapleAllocator &allocator, JBCAttrKind kindIn, uint16 nameIdx, uint32 length)
    : JBCAttr(kindIn, nameIdx, length), size(0), annotations(allocator.Adapter()) {}

bool JBCAttrRTAnnotations::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) {
  bool success = false;
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp, "mempool is nullptr");
  size = io.ReadUInt16(success);
  for (uint16 i = 0; i < size && success; i++) {
    attr::Annotation *item = mp->New<attr::Annotation>(allocator);
    success = item->ParseFile(allocator, io);
    annotations.push_back(item);
  }
  return success;
}

bool JBCAttrRTAnnotations::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *JBCAttrRTAnnotations::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                    uint32 idx) const {
  return nullptr;
}

// ---------- JBCAttrRTVisAnnotations ----------
JBCAttrRTVisAnnotations::JBCAttrRTVisAnnotations(MapleAllocator &allocator, uint16 nameIdx, uint32 length)
    : JBCAttrRTAnnotations(allocator, kAttrRTVisAnnotations, nameIdx, length) {}

// ---------- JBCAttrRTInvisAnnotations ----------
JBCAttrRTInvisAnnotations::JBCAttrRTInvisAnnotations(MapleAllocator &allocator, uint16 nameIdx, uint32 length)
    : JBCAttrRTAnnotations(allocator, kAttrRTInvisAnnotations, nameIdx, length) {}

// ---------- JBCAttrRTParamAnnotations ----------
JBCAttrRTParamAnnotations::JBCAttrRTParamAnnotations(MapleAllocator &allocator, JBCAttrKind kindIn, uint16 nameIdx,
                                                     uint32 length)
    : JBCAttr(kindIn, nameIdx, length), size(0), annotations(allocator.Adapter()) {}

bool JBCAttrRTParamAnnotations::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io,
                                              const JBCConstPool &constPool) {
  bool success = false;
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp, "mempool is nullptr");
  size = io.ReadUInt8(success);
  for (uint16 i = 0; i < size && success; i++) {
    attr::ParamAnnotationItem *item = mp->New<attr::ParamAnnotationItem>(allocator);
    success = item->ParseFile(allocator, io);
    annotations.push_back(item);
  }
  return success;
}

bool JBCAttrRTParamAnnotations::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *JBCAttrRTParamAnnotations::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                         uint32 idx) const {
  return nullptr;
}

// ---------- JBCAttrRTVisParamAnnotations ----------
JBCAttrRTVisParamAnnotations::JBCAttrRTVisParamAnnotations(MapleAllocator &allocator, uint16 nameIdx, uint32 length)
    : JBCAttrRTParamAnnotations(allocator, kAttrRTVisParamAnnotations, nameIdx, length) {}

// ---------- JBCAttrRTInvisParamAnnotations ----------
JBCAttrRTInvisParamAnnotations::JBCAttrRTInvisParamAnnotations(MapleAllocator &allocator, uint16 nameIdx,
                                                               uint32 length)
    : JBCAttrRTParamAnnotations(allocator, kAttrRTInvisParamAnnotations, nameIdx, length) {}

// ---------- JBCAttrRTTypeAnnotations ----------
JBCAttrRTTypeAnnotations::JBCAttrRTTypeAnnotations(MapleAllocator &allocator, JBCAttrKind kindIn, uint16 nameIdx,
                                                   uint32 length)
    : JBCAttr(kindIn, nameIdx, length), size(0), annotations(allocator.Adapter()) {}

bool JBCAttrRTTypeAnnotations::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io,
                                             const JBCConstPool &constPool) {
  bool success = false;
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp, "mempool is nullptr");
  size = io.ReadUInt16(success);
  for (uint16 i = 0; i < size && success; i++) {
    attr::TypeAnnotationItem *item = mp->New<attr::TypeAnnotationItem>(allocator);
    success = item->ParseFile(allocator, io);
    annotations.push_back(item);
  }
  return success;
}

bool JBCAttrRTTypeAnnotations::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *JBCAttrRTTypeAnnotations::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                        uint32 idx) const {
  return nullptr;
}

// ---------- JBCAttrRTVisTypeAnnotations ----------
JBCAttrRTVisTypeAnnotations::JBCAttrRTVisTypeAnnotations(MapleAllocator &allocator, uint16 nameIdx, uint32 length)
    : JBCAttrRTTypeAnnotations(allocator, kAttrRTVisTypeAnnotations, nameIdx, length) {}

// ---------- JBCAttrRTInvisTypeAnnotations ----------
JBCAttrRTInvisTypeAnnotations::JBCAttrRTInvisTypeAnnotations(MapleAllocator &allocator, uint16 nameIdx, uint32 length)
    : JBCAttrRTTypeAnnotations(allocator, kAttrRTInvisTypeAnnotations, nameIdx, length) {}

// ---------- JBCAttrAnnotationDefault ----------
JBCAttrAnnotationDefault::JBCAttrAnnotationDefault(MapleAllocator &allocator, uint16 nameIdx, uint32 length)
    : JBCAttr(kAttrAnnotationDefault, nameIdx, length), value(nullptr) {}

JBCAttrAnnotationDefault::~JBCAttrAnnotationDefault() {
  value = nullptr;
}

bool JBCAttrAnnotationDefault::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io,
                                             const JBCConstPool &constPool) {
  bool success = false;
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp != nullptr, "mempool is nullptr");
  value = mp->New<attr::ElementValue>();
  success = value->ParseFile(allocator, io);
  return success;
}

bool JBCAttrAnnotationDefault::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *JBCAttrAnnotationDefault::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                        uint32 idx) const {
  return nullptr;
}

// ---------- JBCAttrBootstrapMethods ----------
JBCAttrBootstrapMethods::JBCAttrBootstrapMethods(MapleAllocator &allocator, uint16 nameIdx, uint32 length)
    : JBCAttr(kAttrBootstrapMethods, nameIdx, length), size(0), methods(allocator.Adapter()) {}

bool JBCAttrBootstrapMethods::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) {
  bool success = false;
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp, "mempool is nullptr");
  size = io.ReadUInt16(success);
  for (uint16 i = 0; i < size && success; i++) {
    attr::BootstrapMethodItem *item = mp->New<attr::BootstrapMethodItem>(allocator);
    success = item->ParseFile(allocator, io);
    methods.push_back(item);
  }
  return success;
}

bool JBCAttrBootstrapMethods::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *JBCAttrBootstrapMethods::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                       uint32 idx) const {
  return nullptr;
}

// ---------- JBCAttrMethodParameters ----------
JBCAttrMethodParameters::JBCAttrMethodParameters(MapleAllocator &allocator, uint16 nameIdx, uint32 length)
    : JBCAttr(kAttrMethodParameters, nameIdx, length), size(0), params(allocator.Adapter()) {}

bool JBCAttrMethodParameters::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) {
  bool success = false;
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp, "mempool is nullptr");
  size = io.ReadUInt8(success);
  for (uint8 i = 0; i < size && success; i++) {
    attr::MethodParamItem *item = mp->New<attr::MethodParamItem>();
    success = item->ParseFile(allocator, io);
    params.push_back(item);
  }
  return success;
}

bool JBCAttrMethodParameters::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *JBCAttrMethodParameters::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                       uint32 idx) const {
  return nullptr;
}
}  // namespace jbc
}  // namespace maple
