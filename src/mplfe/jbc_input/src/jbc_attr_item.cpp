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
#include "jbc_attr_item.h"

namespace maple {
namespace jbc {
namespace attr {
inline GStrIdx GetOrCreateGStrIdx(const std::string &str) {
  return GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(str);
}

inline GStrIdx GetOrCreateGStrIdxWithMangler(const std::string &str) {
  return GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(NameMangler::EncodeName(str));
}

// ---------- ExceptionTableItem ----------
ExceptionTableItem::ExceptionTableItem() : startPC(0), endPC(0), handlerPC(0), catchTypeIdx(0), catchType(nullptr) {}

ExceptionTableItem::~ExceptionTableItem() {
  catchType = nullptr;
}

bool ExceptionTableItem::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  startPC = io.ReadUInt16(success);
  endPC = io.ReadUInt16(success);
  handlerPC = io.ReadUInt16(success);
  catchTypeIdx = io.ReadUInt16(success);
  return success;
}

bool ExceptionTableItem::PreProcessImpl(const JBCConstPool &constPool) {
  if (catchTypeIdx == 0) {
    return true;
  }
  const JBCConst *constRaw = constPool.GetConstByIdxWithTag(catchTypeIdx, kConstClass);
  if (constRaw == nullptr) {
    catchType = nullptr;
    return false;
  } else {
    catchType = static_cast<const JBCConstClass*>(constRaw);
    return true;
  }
}

SimpleXMLElem *ExceptionTableItem::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                  uint32 idx) const {
  return nullptr;
}

// ---------- VerificationTypeInfo ----------
std::map<VerificationTypeInfoTag, std::string> VerificationTypeInfo::tagNameMap =
    VerificationTypeInfo::InitTagNameMap();

VerificationTypeInfo::VerificationTypeInfo()
    : tag(kVerTypeInfoItemUnknown),
      classInfo(nullptr) {
  data.raw = 0;
}

VerificationTypeInfo::~VerificationTypeInfo() {
  classInfo = nullptr;
}

bool VerificationTypeInfo::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  uint8 t = io.ReadUInt8(success);
  tag = static_cast<VerificationTypeInfoTag>(t);
  switch (t) {
    case kVerTypeInfoItemTop:
    case kVerTypeInfoItemInteger:
    case kVerTypeInfoItemFloat:
    case kVerTypeInfoItemDouble:
    case kVerTypeInfoItemLong:
    case kVerTypeInfoItemNull:
    case kVerTypeInfoItemUninitializedThis:
      break;
    case kVerTypeInfoItemObject:
      data.cpoolIdx = io.ReadUInt16(success);
      break;
    case kVerTypeInfoItemUninitialized:
      data.offset = io.ReadUInt16(success);
      break;
    default:
      ERR(kLncErr, "undefined tag %d for verification_type_info", tag);
      return false;
  }
  return success;
}

bool VerificationTypeInfo::PreProcessImpl(const JBCConstPool &constPool) {
  return false;
}

SimpleXMLElem *VerificationTypeInfo::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                    uint32 idx) const {
  return nullptr;
}

std::map<VerificationTypeInfoTag, std::string> VerificationTypeInfo::InitTagNameMap() {
  std::map<VerificationTypeInfoTag, std::string> result;
  result[kVerTypeInfoItemTop] = "ITEM_Top";
  result[kVerTypeInfoItemInteger] = "ITEM_Integer";
  result[kVerTypeInfoItemFloat] = "ITEM_Float";
  result[kVerTypeInfoItemDouble] = "ITEM_Double";
  result[kVerTypeInfoItemLong] = "ITEM_Long";
  result[kVerTypeInfoItemNull] = "ITEM_Null";
  result[kVerTypeInfoItemUninitializedThis] = "ITEM_UninitializedThis";
  result[kVerTypeInfoItemObject] = "ITEM_Object";
  result[kVerTypeInfoItemUninitialized] = "ITEM_Uninitialized";
  return result;
}

std::string VerificationTypeInfo::TagName(VerificationTypeInfoTag t) {
  std::map<VerificationTypeInfoTag, std::string>::const_iterator it = tagNameMap.find(t);
  if (it != tagNameMap.end()) {
    return it->second;
  }
  return "ITEM_Unknown";
}

// ---------- StackMapFrameItem ----------
std::map<StackMapFrameItemTag, std::string> StackMapFrameItem::tagNameMap = StackMapFrameItem::InitTagName();

StackMapFrameItem::StackMapFrameItem(uint8 frameTypeIn, StackMapFrameItemTag tagIn)
    : tag(tagIn), frameType(frameTypeIn) {}

std::map<StackMapFrameItemTag, std::string> StackMapFrameItem::InitTagName() {
  std::map<StackMapFrameItemTag, std::string> result;
  result[kStackSame] = "SAME";
  result[kStackSameLocals1StackItem] = "SAME_LOCALS_1_STACK_ITEM";
  result[kStackSameLocals1StackItemEx] = "SAME_LOCALS_1_STACK_ITEM_EXTENDED";
  result[kStackChop] = "CHOP";
  result[kStackSameFrameEx] = "SAME_FRAME_EXTENDED";
  result[kStackAppend] = "APPEND";
  result[kStackFullFrame] = "FULL_FRAME";
  result[kStackReserved] = "RESERVED";
  return result;
}

std::string StackMapFrameItem::TagName(StackMapFrameItemTag tag) {
  std::map<StackMapFrameItemTag, std::string>::const_iterator it = tagNameMap.find(tag);
  if (it != tagNameMap.end()) {
    return it->second;
  }
  return "UNKNOWN";
}

StackMapFrameItemTag StackMapFrameItem::FrameType2Tag(uint8 frameType) {
  // frame type tag : 64, 128, 247, 251, 255
  if (frameType < 64) {
    return kStackSame;
  } else if (frameType < 128) {
    return kStackSameLocals1StackItem;
  } else if (frameType < 247) {
    ERR(kLncErr, "Reserved frametype %d for stack_map_frame", frameType);
    return kStackReserved;
  } else if (frameType == 247) {
    return kStackSameLocals1StackItemEx;
  } else if (frameType < 251) {
    return kStackChop;
  } else if (frameType < 255) {
    return kStackAppend;
  } else {
    // frameType == 255
    return kStackFullFrame;
  }
}

StackMapFrameItem *StackMapFrameItem::NewItem(MapleAllocator &allocator, BasicIORead &io, uint8 frameType) {
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp, "mempool is nullptr");
  StackMapFrameItem *item = nullptr;
  StackMapFrameItemTag tag = StackMapFrameItem::FrameType2Tag(frameType);
  switch (tag) {
    case kStackSame:
      item = mp->New<StackMapFrameItemSame>(frameType);
      break;
    case kStackSameLocals1StackItem:
      item = mp->New<StackMapFrameItemSameLocals1>(frameType);
      break;
    case kStackSameLocals1StackItemEx:
      item = mp->New<StackMapFrameItemSameLocals1Ex>(frameType);
      break;
    case kStackChop:
      item = mp->New<StackMapFrameItemChop>(frameType);
      break;
    case kStackSameFrameEx:
      item = mp->New<StackMapFrameItemSameEx>(frameType);
      break;
    case kStackAppend:
      item = mp->New<StackMapFrameItemAppend>(allocator, frameType);
      break;
    case kStackFullFrame:
      item = mp->New<StackMapFrameItemFull>(allocator, frameType);
      break;
    default:
      CHECK_FATAL(false, "Should not run here");
  }
  if (!item->ParseFile(allocator, io)) {
    CHECK_FATAL(false, "Failed to new StackMapFrame item");
    return nullptr;
  }
  return item;
}

// ---------- StackMapFrameItemSame ----------
StackMapFrameItemSame::StackMapFrameItemSame(uint8 frameType) : StackMapFrameItem(frameType, kStackSame) {}

bool StackMapFrameItemSame::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  return true;
}

bool StackMapFrameItemSame::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *StackMapFrameItemSame::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                     uint32 idx) const {
  return nullptr;
}

// ---------- StackMapFrameItemSameLocals1 ----------
StackMapFrameItemSameLocals1::StackMapFrameItemSameLocals1(uint8 frameType)
    : StackMapFrameItem(frameType, kStackSameLocals1StackItem),
      stack(nullptr) {}

StackMapFrameItemSameLocals1::~StackMapFrameItemSameLocals1() {
  stack = nullptr;
}

bool StackMapFrameItemSameLocals1::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp, "mempool is nullptr");
  stack = mp->New<VerificationTypeInfo>();
  return stack->ParseFile(allocator, io);
}

bool StackMapFrameItemSameLocals1::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *StackMapFrameItemSameLocals1::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                            uint32 idx) const {
  return nullptr;
}

// ---------- StackMapFrameItemSameLocals1Ex ----------
StackMapFrameItemSameLocals1Ex::StackMapFrameItemSameLocals1Ex(uint8 frameType)
    : StackMapFrameItem(frameType, kStackSameLocals1StackItemEx),
      offsetDelta(0),
      stack(nullptr) {}

StackMapFrameItemSameLocals1Ex::~StackMapFrameItemSameLocals1Ex() {
  stack = nullptr;
}

bool StackMapFrameItemSameLocals1Ex::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp, "mempool is nullptr");
  offsetDelta = io.ReadUInt16(success);
  stack = mp->New<VerificationTypeInfo>();
  success = stack->ParseFile(allocator, io);
  return success;
}

bool StackMapFrameItemSameLocals1Ex::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *StackMapFrameItemSameLocals1Ex::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                              uint32 idx) const {
  return nullptr;
}

// ---------- StackMapFrameItemChop ----------
StackMapFrameItemChop::StackMapFrameItemChop(uint8 frameType)
    : StackMapFrameItem(frameType, kStackChop),
      offsetDelta(0) {}

bool StackMapFrameItemChop::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  offsetDelta = io.ReadUInt16(success);
  return success;
}

bool StackMapFrameItemChop::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *StackMapFrameItemChop::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                     uint32 idx) const {
  return nullptr;
}

// ---------- StackMapFrameItemSameEx ----------
StackMapFrameItemSameEx::StackMapFrameItemSameEx(uint8 frameType)
    : StackMapFrameItem(frameType, kStackSameFrameEx),
      offsetDelta(0) {}

bool StackMapFrameItemSameEx::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  offsetDelta = io.ReadUInt16(success);
  return success;
}

bool StackMapFrameItemSameEx::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *StackMapFrameItemSameEx::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                       uint32 idx) const {
  return nullptr;
}

// ---------- StackMapFrameItemAppend ----------
StackMapFrameItemAppend::StackMapFrameItemAppend(MapleAllocator &allocator, uint8 frameType)
    : StackMapFrameItem(frameType, kStackAppend),
      offsetDelta(0),
      locals(allocator.Adapter()) {}

bool StackMapFrameItemAppend::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp, "mempool is nullptr");
  offsetDelta = io.ReadUInt16(success);
  for (uint8 i = 0; i < frameType - 251; i++) { // 251 : frame type tag
    VerificationTypeInfo *local = mp->New<VerificationTypeInfo>();
    success = local->ParseFile(allocator, io);
    locals.push_back(local);
  }
  return success;
}

bool StackMapFrameItemAppend::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *StackMapFrameItemAppend::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                       uint32 idx) const {
  return nullptr;
}

// ---------- StackMapFrameItemFull ----------
StackMapFrameItemFull::StackMapFrameItemFull(MapleAllocator &allocator, uint8 frameType)
    : StackMapFrameItem(frameType, kStackFullFrame),
      offsetDelta(0),
      nLocals(0),
      locals(allocator.Adapter()),
      nStacks(0),
      stacks(allocator.Adapter()) {}

bool StackMapFrameItemFull::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp, "mempool is nullptr");
  offsetDelta = io.ReadUInt16(success);
  nLocals = io.ReadUInt16(success);
  for (uint32 i = 0; i < nLocals; i++) {
    VerificationTypeInfo *local = mp->New<VerificationTypeInfo>();
    success = local->ParseFile(allocator, io);
    locals.push_back(local);
  }
  nStacks = io.ReadUInt16(success);
  for (uint32 i = 0; i < nStacks; i++) {
    VerificationTypeInfo *stack = mp->New<VerificationTypeInfo>();
    success = stack->ParseFile(allocator, io);
    stacks.push_back(stack);
  }
  return success;
}

bool StackMapFrameItemFull::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *StackMapFrameItemFull::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                     uint32 idx) const {
  return nullptr;
}

// ---------- InnerClassItem ----------
InnerClassItem::InnerClassItem()
    : innerClassInfoIdx(0),
      outerClassInfoIdx(0),
      innerNameIdx(0),
      innerClassAccessFlag(0),
      constClassInner(nullptr),
      constClassOuter(nullptr),
      constNameInner(nullptr) {}

InnerClassItem::~InnerClassItem() {
  constClassInner = nullptr;
  constClassOuter = nullptr;
  constNameInner = nullptr;
}

bool InnerClassItem::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  innerClassInfoIdx = io.ReadUInt16(success);
  outerClassInfoIdx = io.ReadUInt16(success);
  innerNameIdx = io.ReadUInt16(success);
  innerClassAccessFlag = io.ReadUInt16(success);
  return success;
}

bool InnerClassItem::PreProcessImpl(const JBCConstPool &constPool) {
  return false;
}

SimpleXMLElem *InnerClassItem::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                              uint32 idx) const {
  return nullptr;
}

// ---------- LineNumberTableItem ----------
LineNumberTableItem::LineNumberTableItem() : startPC(0), lineNumber(0) {}

bool LineNumberTableItem::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  startPC = io.ReadUInt16(success);
  lineNumber = io.ReadUInt16(success);
  return success;
}

bool LineNumberTableItem::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *LineNumberTableItem::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                   uint32 idx) const {
  return nullptr;
}

// ---------- LocalVariableTableItem ----------
LocalVariableTableItem::LocalVariableTableItem()
    : startPC(0),
      length(0),
      nameIdx(0),
      descIdx(0),
      index(0),
      constName(nullptr),
      constDesc(nullptr),
      nameIdxMpl(0),
      descNameIdxMpl(0) {}

LocalVariableTableItem::~LocalVariableTableItem() {
  constName = nullptr;
  constDesc = nullptr;
}

bool LocalVariableTableItem::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  startPC = io.ReadUInt16(success);
  length = io.ReadUInt16(success);
  nameIdx = io.ReadUInt16(success);
  descIdx = io.ReadUInt16(success);
  index = io.ReadUInt16(success);
  return success;
}

bool LocalVariableTableItem::PreProcessImpl(const JBCConstPool &constPool) {
  constName = static_cast<const JBCConstUTF8*>(constPool.GetConstByIdxWithTag(nameIdx, JBCConstTag::kConstUTF8));
  constDesc = static_cast<const JBCConstUTF8*>(constPool.GetConstByIdxWithTag(descIdx, JBCConstTag::kConstUTF8));
  if (constName == nullptr || constDesc == nullptr) {
    return false;
  }
  nameIdxMpl = GetOrCreateGStrIdx(constName->GetString());
  descNameIdxMpl = GetOrCreateGStrIdxWithMangler(constDesc->GetString());
  return true;
}

SimpleXMLElem *LocalVariableTableItem::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                      uint32 idx) const {
  return nullptr;
}

// ---------- LocalVariableTypeTableItem ----------
LocalVariableTypeTableItem::LocalVariableTypeTableItem()
    : startPC(0), length(0), nameIdx(0), signatureIdx(0), index(0), constName(nullptr), constSignature(nullptr) {}

LocalVariableTypeTableItem::~LocalVariableTypeTableItem() {
  constName = nullptr;
  constSignature = nullptr;
}

bool LocalVariableTypeTableItem::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  startPC = io.ReadUInt16(success);
  length = io.ReadUInt16(success);
  nameIdx = io.ReadUInt16(success);
  signatureIdx = io.ReadUInt16(success);
  index = io.ReadUInt16(success);
  return success;
}

bool LocalVariableTypeTableItem::PreProcessImpl(const JBCConstPool &constPool) {
  constName = static_cast<const JBCConstUTF8*>(constPool.GetConstByIdxWithTag(nameIdx, JBCConstTag::kConstUTF8));
  constSignature =
      static_cast<const JBCConstUTF8*>(constPool.GetConstByIdxWithTag(signatureIdx, JBCConstTag::kConstUTF8));
  if (constName == nullptr || constSignature == nullptr) {
    return false;
  }
  nameIdxMpl = GetOrCreateGStrIdx(constName->GetString());
  signatureNameIdxMpl = GetOrCreateGStrIdxWithMangler(constSignature->GetString());
  return true;
}

SimpleXMLElem *LocalVariableTypeTableItem::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                          uint32 idx) const {
  return nullptr;
}

// ---------- ParamAnnotationItem ----------
ParamAnnotationItem::ParamAnnotationItem(MapleAllocator &allocator)
    : count(0), annotations(allocator.Adapter()) {}

bool ParamAnnotationItem::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp, "mempool is nullptr");
  count = io.ReadUInt16(success);
  for (uint16 i = 0; i < count; i++) {
    Annotation *anno = mp->New<Annotation>(allocator);
    success = anno->ParseFile(allocator, io);
    annotations.push_back(anno);
  }
  return success;
}

bool ParamAnnotationItem::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *ParamAnnotationItem::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                   uint32 idx) const {
  return nullptr;
}

// ---------- BootstrapMethodItem ----------
BootstrapMethodItem::BootstrapMethodItem(MapleAllocator &allocator)
    : methodRefIdx(0),
      nArgs(0),
      argsIdx(allocator.Adapter()),
      methodHandle(nullptr),
      args(allocator.Adapter()) {}

BootstrapMethodItem::~BootstrapMethodItem() {
  methodHandle = nullptr;
}

bool BootstrapMethodItem::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  methodRefIdx = io.ReadUInt16(success);
  nArgs = io.ReadUInt16(success);
  for (uint16 i = 0; i < nArgs; i++) {
    uint16 idx = io.ReadUInt16(success);
    argsIdx.push_back(idx);
  }
  return success;
}

bool BootstrapMethodItem::PreProcessImpl(const JBCConstPool &constPool) {
  return false;
}

SimpleXMLElem *BootstrapMethodItem::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                   uint32 idx) const {
  return nullptr;
}

// ---------- MethodParamItem ----------
MethodParamItem::MethodParamItem()
    : nameIdx(0), accessFlag(0), constName(nullptr) {}

MethodParamItem::~MethodParamItem() {
  constName = nullptr;
}

bool MethodParamItem::MethodParamItem::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  nameIdx = io.ReadUInt16(success);
  accessFlag = io.ReadUInt16(success);
  return success;
}

bool MethodParamItem::PreProcessImpl(const JBCConstPool &constPool) {
  return false;
}

SimpleXMLElem *MethodParamItem::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                               uint32 idx) const {
  return nullptr;
}

// ---------- Annotation ----------
Annotation::Annotation(MapleAllocator &allocator)
    : typeIdx(0), nElemPairs(0), tbElemPairs(allocator.Adapter()), constTypeName(nullptr) {}

Annotation::~Annotation() {
  constTypeName = nullptr;
}

bool Annotation::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp, "mempool is nullptr");
  typeIdx = io.ReadUInt16(success);
  nElemPairs = io.ReadUInt16(success);
  for (uint16 i = 0; i < nElemPairs; i++) {
    ElementValuePair *pair = mp->New<ElementValuePair>();
    success = pair->ParseFile(allocator, io);
    tbElemPairs.push_back(pair);
  }
  return success;
}

bool Annotation::PreProcessImpl(const JBCConstPool &constPool) {
  return false;
}

SimpleXMLElem *Annotation::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const {
  return nullptr;
}

// ---------- ElementValueItem ----------
std::map<char, ElementValueKind> ElementValueItem::tagKindMap = ElementValueItem::InitTagKindMap();
std::map<ElementValueKind, std::string> ElementValueItem::kindNameMap = ElementValueItem::InitKindNameMap();

ElementValueItem::ElementValueItem(ElementValueKind kindIn, char tagIn) : kind(kindIn), tag(tagIn) {}

std::map<char, ElementValueKind> ElementValueItem::InitTagKindMap() {
  std::map<char, ElementValueKind> result;
  result['B'] = kElementValueConst;
  result['C'] = kElementValueConst;
  result['D'] = kElementValueConst;
  result['F'] = kElementValueConst;
  result['I'] = kElementValueConst;
  result['J'] = kElementValueConst;
  result['S'] = kElementValueConst;
  result['Z'] = kElementValueConst;
  result['s'] = kElementValueConst;
  result['e'] = kElementValueEnum;
  result['c'] = kElementValueClassInfo;
  result['@'] = kElementValueAnnotation;
  result['['] = kElementValueArray;
  return result;
}

std::map<ElementValueKind, std::string> ElementValueItem::InitKindNameMap() {
  std::map<ElementValueKind, std::string> result;
  result[kElementValueConst] = "ElementValueConst";
  result[kElementValueEnum] = "ElementValueEnum";
  result[kElementValueClassInfo] = "ElementValueClassInfo";
  result[kElementValueAnnotation] = "ElementValueAnnotation";
  result[kElementValueArray] = "ElementValueArray";
  return result;
}

std::string ElementValueItem::KindName(ElementValueKind kind) {
  std::map<ElementValueKind, std::string>::const_iterator it = kindNameMap.find(kind);
  if (it != kindNameMap.end()) {
    return it->second;
  }
  return "Unknown";
}

ElementValueKind ElementValueItem::TagToKind(char tag) {
  std::map<char, ElementValueKind>::const_iterator it = tagKindMap.find(tag);
  if (it != tagKindMap.end()) {
    return it->second;
  }
  return kElementValueDefault;
}

ElementValueItem *ElementValueItem::NewItem(MapleAllocator &allocator, BasicIORead &io, char tag) {
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp, "mempool is nullptr");
  ElementValueKind kind = ElementValueItem::TagToKind(tag);
  ElementValueItem *item = nullptr;
  switch (kind) {
    case kElementValueConst:
      item = mp->New<ElementValueConst>(tag);
      break;
    case kElementValueEnum:
      item = mp->New<ElementValueEnum>();
      break;
    case kElementValueClassInfo:
      item = mp->New<ElementValueClassInfo>();
      break;
    case kElementValueAnnotation:
      item = mp->New<ElementValueAnnotation>();
      break;
    case kElementValueArray:
      item = mp->New<ElementValueArray>(allocator);
      break;
    default:
      ERR(kLncErr, "unsupported kind");
      return nullptr;
  }
  if (item->ParseFile(allocator, io) == false) {
    return nullptr;
  }
  return item;
}

// ---------- ElementValueConst ----------
ElementValueConst::ElementValueConst(uint8 t)
    : ElementValueItem(kElementValueConst, t), constValueIdx(0), constValue(nullptr) {}

ElementValueConst::~ElementValueConst() {
  constValue = nullptr;
}

bool ElementValueConst::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  constValueIdx = io.ReadUInt16(success);
  return success;
}

bool ElementValueConst::PreProcessImpl(const JBCConstPool &constPool) {
  return false;
}

SimpleXMLElem *ElementValueConst::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                 uint32 idx) const {
  return nullptr;
}

// ---------- ElementValueEnum ----------
ElementValueEnum::ElementValueEnum()
    : ElementValueItem(kElementValueEnum, 'e'),
      typeNameIdx(0),
      constNameIdx(0),
      constTypeName(nullptr),
      constName(nullptr) {}

ElementValueEnum::~ElementValueEnum() {
  constTypeName = nullptr;
  constName = nullptr;
}

bool ElementValueEnum::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  typeNameIdx = io.ReadUInt16(success);
  constNameIdx = io.ReadUInt16(success);
  return success;
}

bool ElementValueEnum::PreProcessImpl(const JBCConstPool &constPool) {
  return false;
}

SimpleXMLElem *ElementValueEnum::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                uint32 idx) const {
  return nullptr;
}

// ---------- ElementValueClassInfo ----------
ElementValueClassInfo::ElementValueClassInfo()
    : ElementValueItem(kElementValueClassInfo, 'c'),
      classInfoIdx(0),
      constClassInfo(nullptr) {}

ElementValueClassInfo::~ElementValueClassInfo() {
  constClassInfo = nullptr;
}

bool ElementValueClassInfo::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  classInfoIdx = io.ReadUInt16(success);
  return success;
}

bool ElementValueClassInfo::PreProcessImpl(const JBCConstPool &constPool) {
  return false;
}

SimpleXMLElem *ElementValueClassInfo::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                     uint32 idx) const {
  return nullptr;
}

// ---------- ElementValueAnnotation ----------
ElementValueAnnotation::ElementValueAnnotation()
    : ElementValueItem(kElementValueAnnotation, '@'), annotation(nullptr) {}

ElementValueAnnotation::~ElementValueAnnotation() {
  annotation = nullptr;
}

bool ElementValueAnnotation::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp != nullptr, "mempool is nullptr");
  annotation = mp->New<Annotation>(allocator);
  return annotation->ParseFile(allocator, io);
}

bool ElementValueAnnotation::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *ElementValueAnnotation::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                      uint32 idx) const {
  return nullptr;
}

// ---------- ElementValueArray ----------
ElementValueArray::ElementValueArray(MapleAllocator &allocator)
    : ElementValueItem(kElementValueArray, '['), size(0), values(allocator.Adapter()) {}

bool ElementValueArray::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp, "mempool is nullptr");
  size = io.ReadUInt16(success);
  for (uint16 i = 0; i < size; i++) {
    ElementValue *elem = mp->New<ElementValue>();
    success = elem->ParseFile(allocator, io);
    values.push_back(elem);
  }
  return success;
}

bool ElementValueArray::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *ElementValueArray::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                 uint32 idx) const {
  return nullptr;
}

// ---------- ElementValue ----------
ElementValue::ElementValue()
    : tag('\0'), kind(kElementValueDefault), value(nullptr) {}

ElementValue::~ElementValue() {
  value = nullptr;
}

bool ElementValue::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  tag = io.ReadChar(success);
  value = ElementValueItem::NewItem(allocator, io, tag);
  return success && value != nullptr;
}

bool ElementValue::PreProcessImpl(const JBCConstPool &constPool) {
  return false;
}

SimpleXMLElem *ElementValue::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                            uint32 idx) const {
  return nullptr;
}

// ---------- ElementValuePair ----------
ElementValuePair::ElementValuePair()
    : nameIdx(0), value(nullptr) {}

ElementValuePair::~ElementValuePair() {
  value = nullptr;
}

bool ElementValuePair::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp, "mempool is nullptr");
  nameIdx = io.ReadUInt16(success);
  value = mp->New<ElementValue>();
  return (success && value->ParseFile(allocator, io));
}

bool ElementValuePair::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *ElementValuePair::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                uint32 idx) const {
  return nullptr;
}

// ---------- TargetInfoItem ----------
std::map<TargetInfoItemType, TargetInfoItemTag> TargetInfoItem::typeTagMap = TargetInfoItem::InitTypeTagMap();

TargetInfoItem::TargetInfoItem(TargetInfoItemTag tagIn) : tag(tagIn) {}

std::map<TargetInfoItemType, TargetInfoItemTag> TargetInfoItem::InitTypeTagMap() {
  std::map<TargetInfoItemType, TargetInfoItemTag> result;
  result[kTargetTypeParamClass] = kTargetTagTypeParam;
  result[kTargetTypeParamMethod] = kTargetTagTypeParam;
  result[kTargetTypeHierarchy] = kTargetTagSuperType;
  result[kTargetTypeBoundClass] = kTargetTagTypeParamBound;
  result[kTargetTypeBoundMethod] = kTargetTagTypeParamBound;
  result[kTargetTypeFieldDecl] = kTargetTagEmpty;
  result[kTargetTypeReturn] = kTargetTagEmpty;
  result[kTargetTypeReceiver] = kTargetTagEmpty;
  result[kTargetTypeFormal] = kTargetTagFormalParam;
  result[kTargetTypeThrows] = kTargetTagThrows;
  result[kTargetTypeLocalVar] = kTargetTagLocalVar;
  result[kTargetTypeResourceVar] = kTargetTagLocalVar;
  result[kTargetTypeExpectionParam] = kTargetTagCatch;
  result[kTargetTypeInstanceof] = kTargetTagOffset;
  result[kTargetTypeNew] = kTargetTagOffset;
  result[kTargetTypeMethodRefNew] = kTargetTagOffset;
  result[kTargetTypeMethodRefIdentifier] = kTargetTagOffset;
  result[kTargetTypeCast] = kTargetTagTypeArg;
  result[kTargetTypeConstructorInvoke] = kTargetTagTypeArg;
  result[kTargetTypeMethodInvoke] = kTargetTagTypeArg;
  result[kTargetTypeConstructorNew] = kTargetTagTypeArg;
  result[kTargetTypeConstructorIdentifier] = kTargetTagTypeArg;
  return result;
}

TargetInfoItemTag TargetInfoItem::TargetType2Tag(TargetInfoItemType type) {
  std::map<TargetInfoItemType, TargetInfoItemTag>::const_iterator it = typeTagMap.find(type);
  if (it != typeTagMap.end()) {
    return it->second;
  }
  return kTargetTagUndefine;
}

TargetInfoItem *TargetInfoItem::NewItem(MapleAllocator &allocator, BasicIORead &io, TargetInfoItemType targetType) {
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp, "mempool is nullptr");
  TargetInfoItemTag tag = TargetInfoItem::TargetType2Tag(targetType);
  TargetInfoItem *item = nullptr;
  switch (tag) {
    case kTargetTagTypeParam:
      item = mp->New<TargetTypeParam>();
      break;
    case kTargetTagSuperType:
      item = mp->New<TargetSuperType>();
      break;
    case kTargetTagTypeParamBound:
      item = mp->New<TargetTypeParamBound>();
      break;
    case kTargetTagEmpty:
      item = mp->New<TargetEmpty>();
      break;
    case kTargetTagFormalParam:
      item = mp->New<TargetFormalParam>();
      break;
    case kTargetTagThrows:
      item = mp->New<TargetThrows>();
      break;
    case kTargetTagLocalVar:
      item = mp->New<TargetLocalVar>(allocator);
      break;
    case kTargetTagCatch:
      item = mp->New<TargetCatch>();
      break;
    case kTargetTagOffset:
      item = mp->New<TargetOffset>();
      break;
    case kTargetTagTypeArg:
      item = mp->New<TargetTypeArg>();
      break;
    default:
      ERR(kLncErr, "TargetInfoItem::NewItem(): undefined tag");
      return nullptr;
  }
  if (item->ParseFile(allocator, io) == false) {
    return nullptr;
  }
  return item;
}

// ---------- TargetTypeParam ----------
TargetTypeParam::TargetTypeParam() : TargetInfoItem(kTargetTagTypeParam), paramIdx(0) {}

bool TargetTypeParam::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  paramIdx = io.ReadUInt8(success);
  return success;
}

bool TargetTypeParam::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *TargetTypeParam::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                               uint32 idx) const {
  return nullptr;
}

// ---------- TargetSuperType ----------
TargetSuperType::TargetSuperType() : TargetInfoItem(kTargetTagSuperType), index(0) {}

bool TargetSuperType::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  index = io.ReadUInt16(success);
  return success;
}

bool TargetSuperType::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *TargetSuperType::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                               uint32 idx) const {
  return nullptr;
}

// ---------- TargetTypeParamBound ----------
TargetTypeParamBound::TargetTypeParamBound() : TargetInfoItem(kTargetTagTypeParamBound), paramIdx(0), boundIdx(0) {}

bool TargetTypeParamBound::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  paramIdx = io.ReadUInt8(success);
  boundIdx = io.ReadUInt8(success);
  return success;
}

bool TargetTypeParamBound::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *TargetTypeParamBound::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                    uint32 idx) const {
  return nullptr;
}

// ---------- TargetEmpty ----------
TargetEmpty::TargetEmpty() : TargetInfoItem(kTargetTagEmpty) {}

bool TargetEmpty::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  return true;
}

bool TargetEmpty::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *TargetEmpty::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const {
  return nullptr;
}

// ---------- TargetFormalParam ----------
TargetFormalParam::TargetFormalParam() : TargetInfoItem(kTargetTagFormalParam), paramIdx(0) {}

bool TargetFormalParam::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  paramIdx = io.ReadUInt8(success);
  return success;
}

bool TargetFormalParam::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *TargetFormalParam::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                 uint32 idx) const {
  return nullptr;
}

// ---------- TargetThrows ----------
TargetThrows::TargetThrows() : TargetInfoItem(kTargetTagThrows), typeIdx(0) {}

bool TargetThrows::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  typeIdx = io.ReadUInt16(success);
  return success;
}

bool TargetThrows::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *TargetThrows::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                            uint32 idx) const {
  return nullptr;
}

// ---------- TargetLocalVarItem ----------
TargetLocalVarItem::TargetLocalVarItem() : TargetInfoItem(kTargetTagLocalVar), startPC(0), length(0), index(0) {}

bool TargetLocalVarItem::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  startPC = io.ReadUInt16(success);
  length = io.ReadUInt16(success);
  index = io.ReadUInt16(success);
  return success;
}
bool TargetLocalVarItem::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *TargetLocalVarItem::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                  uint32 idx) const {
  return nullptr;
}

// ---------- TargetLocalVar ----------
TargetLocalVar::TargetLocalVar(MapleAllocator &allocator)
    : TargetInfoItem(kTargetTagLocalVar), size(0), table(allocator.Adapter()) {}

bool TargetLocalVar::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp, "mempool is nullptr");
  size = io.ReadUInt16(success);
  for (uint16 i = 0; i < size; i++) {
    TargetLocalVarItem *item = mp->New<TargetLocalVarItem>();
    success = item->ParseFile(allocator, io);
    table.push_back(item);
  }
  return success;
}

bool TargetLocalVar::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *TargetLocalVar::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                              uint32 idx) const {
  return nullptr;
}

// ---------- TargetCatch ----------
TargetCatch::TargetCatch() : TargetInfoItem(kTargetTagCatch), exTableIdx(0) {}

bool TargetCatch::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  exTableIdx = io.ReadUInt16(success);
  return true;
}

bool TargetCatch::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *TargetCatch::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const {
  return nullptr;
}

// ---------- TargetOffset ----------
TargetOffset::TargetOffset() : TargetInfoItem(kTargetTagOffset), offset(0) {}

bool TargetOffset::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  offset = io.ReadUInt16(success);
  return success;
}

bool TargetOffset::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *TargetOffset::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                            uint32 idx) const {
  return nullptr;
}

// ---------- TargetTypeArg ----------
TargetTypeArg::TargetTypeArg() : TargetInfoItem(kTargetTagTypeArg), offset(0), typeArgIdx(0) {}

bool TargetTypeArg::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  offset = io.ReadUInt16(success);
  typeArgIdx = io.ReadUInt8(success);
  return success;
}

bool TargetTypeArg::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *TargetTypeArg::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                             uint32 idx) const {
  return nullptr;
}

// ---------- TypeAnnotationItem ----------
TypeAnnotationItem::TypeAnnotationItem(MapleAllocator &allocator)
    : targetType(kTargetTypeParamClass),
      targetInfo(nullptr),
      targetPath(nullptr),
      typeIdx(0),
      nElemPairs(0),
      elemPairs(allocator.Adapter()) {}

TypeAnnotationItem::~TypeAnnotationItem() {
  targetInfo = nullptr;
  targetPath = nullptr;
}

bool TypeAnnotationItem::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp, "mempool is nullptr");
  targetType = static_cast<TargetInfoItemType>(io.ReadUInt8(success));
  targetInfo = TargetInfoItem::NewItem(allocator, io, targetType);
  targetPath = mp->New<TypePath>(allocator);
  success = targetPath->ParseFile(allocator, io);
  typeIdx = io.ReadUInt16(success);
  nElemPairs = io.ReadUInt16(success);
  for (uint16 i = 0; i < nElemPairs; i++) {
    ElementValuePair *e = mp->New<ElementValuePair>();
    success = e->ParseFile(allocator, io);
    elemPairs.push_back(e);
  }
  return success;
}

bool TypeAnnotationItem::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *TypeAnnotationItem::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                                  uint32 idx) const {
  return nullptr;
}

// ---------- TypePathItem ----------
TypePathItem::TypePathItem() : typePathKind(0), typeArgIdx(0) {}

bool TypePathItem::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  typePathKind = io.ReadUInt8(success);
  typeArgIdx = io.ReadUInt8(success);
  return success;
}

bool TypePathItem::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *TypePathItem::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool,
                                            uint32 idx) const {
  return nullptr;
}

// ---------- TypePath ----------
TypePath::TypePath(MapleAllocator &allocator) : pathLength(0), tbPath(allocator.Adapter()) {}

bool TypePath::ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) {
  bool success = false;
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp, "mempool is nullptr");
  pathLength = io.ReadUInt8(success);
  for (uint8 i = 0; i < pathLength; i++) {
    TypePathItem *path = mp->New<TypePathItem>();
    success = path->ParseFile(allocator, io);
    tbPath.push_back(path);
  }
  return success;
}

bool TypePath::PreProcessImpl(const JBCConstPool &constPool) {
  return true;
}

SimpleXMLElem *TypePath::GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const {
  return nullptr;
}
}  // namespace attr
}  // namespace jbc
}  // namespace maple
