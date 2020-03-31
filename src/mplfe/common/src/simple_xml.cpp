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
#include "simple_xml.h"

namespace maple {
SimpleXMLElem::SimpleXMLElem(MapleAllocator &alloc, const std::string &cat)
    : tagID(kXMLIDNone),
      tagValue(kXMLValueNone),
      allocator(alloc),
      catalog(cat),
      id(0),
      valueString(""),
      content(""),
      elems(allocator.Adapter()) {
  value.raw = 0;
  CHECK_FATAL(allocator.GetMemPool(), "mp is null");
}

SimpleXMLElem::SimpleXMLElem(MapleAllocator &alloc, const std::string &cat, uint32 argID)
    : tagID(kXMLID),
      tagValue(kXMLValueNone),
      allocator(alloc),
      catalog(cat),
      id(argID),
      valueString(""),
      content(""),
      elems(allocator.Adapter()) {
  value.raw = 0;
  CHECK_FATAL(allocator.GetMemPool(), "mp is null");
}

void SimpleXMLElem::DumpImpl(std::ostream &os, const std::string &prefix) const {
  DumpHead(os, prefix);
  if (elems.size() > 0) {
    os << std::endl;
    for (auto elem : elems) {
      elem->Dump(os, prefix + "  ");
    }
    os << prefix;
  } else {
    if (content != "") {
      os << "\"" << XMLString(content) << "\"";
    }
  }
  DumpTail(os, "");
  os << std::endl;
}

void SimpleXMLElem::DumpHead(std::ostream &os, const std::string &prefix) const {
  os << prefix << "<" << catalog;
  if (tagID == kXMLID) {
    os << " id=\"" << id << "\"";
  }
  switch (tagValue) {
    case kXMLValueNone:
      break;
    case kXMLValueI8:
      os << " value=\"" << static_cast<int32>(value.i8) << "\"";
      break;
    case kXMLValueU8:
      os << " value=\"" << static_cast<uint32>(value.u8) << "\"";
      break;
    case kXMLValueI16:
      os << " value=\"" << static_cast<int32>(value.i16) << "\"";
      break;
    case kXMLValueU16:
      os << " value=\"" << static_cast<uint32>(value.u16) << "\"";
      break;
    case kXMLValueI32:
      os << " value=\"" << value.i32 << "\"";
      break;
    case kXMLValueU32:
      os << " value=\"" << value.u32 << "\"";
      break;
    case kXMLValueI64:
      os << " value=\"" << value.i64 << "\"";
      break;
    case kXMLValueU64:
      os << " value=\"" << value.u64 << "\"";
      break;
    case kXMLValueFloat:
      os << " value=\"" << value.f << "\"";
      break;
    case kXMLValueDouble:
      os << " value=\"" << value.d << "\"";
      break;
    case kXMLValueString:
      os << " value=\"" << XMLString(valueString) << "\"";
      break;
    default:
      break;
  }
  os << ">";
}

void SimpleXMLElem::DumpTail(std::ostream &os, const std::string &prefix) const {
  os << prefix << "</" << catalog << ">";
}

void SimpleXMLElem::AddElement(SimpleXMLElem &elem) {
  elems.push_back(&elem);
}

SimpleXMLElem *SimpleXMLElem::AddNewElement(const std::string &cat) {
  MemPool *mp = allocator.GetMemPool();
  CHECK_NULL_FATAL(mp);
  SimpleXMLElem *elem = mp->New<SimpleXMLElem>(allocator, cat);
  AddElement(*elem);
  return elem;
}

SimpleXMLElem *SimpleXMLElem::AddNewElement(const std::string &cat, uint32 argID) {
  MemPool *mp = allocator.GetMemPool();
  CHECK_NULL_FATAL(mp);
  SimpleXMLElem *elem = mp->New<SimpleXMLElem>(allocator, cat, argID);
  AddElement(*elem);
  return elem;
}

void SimpleXMLElem::SetValue(int8 v) {
  tagValue = kXMLValueI8;
  value.i8 = v;
}

void SimpleXMLElem::SetValue(uint8 v) {
  tagValue = kXMLValueU8;
  value.u8 = v;
}

void SimpleXMLElem::SetValue(int16 v) {
  tagValue = kXMLValueI16;
  value.i16 = v;
}

void SimpleXMLElem::SetValue(uint16 v) {
  tagValue = kXMLValueU16;
  value.u16 = v;
}

void SimpleXMLElem::SetValue(int32 v) {
  tagValue = kXMLValueI32;
  value.i32 = v;
}

void SimpleXMLElem::SetValue(uint32 v) {
  tagValue = kXMLValueU32;
  value.u32 = v;
}

void SimpleXMLElem::SetValue(int64 v) {
  tagValue = kXMLValueI64;
  value.i64 = v;
}

void SimpleXMLElem::SetValue(uint64 v) {
  tagValue = kXMLValueU64;
  value.u64 = v;
}

void SimpleXMLElem::SetValue(float v) {
  tagValue = kXMLValueFloat;
  value.f = v;
}

void SimpleXMLElem::SetValue(double v) {
  tagValue = kXMLValueDouble;
  value.d = v;
}

void SimpleXMLElem::SetValue(const std::string &str) {
  tagValue = kXMLValueString;
  valueString = str;
}

void SimpleXMLElem::SetContent(const std::string &str) {
  content = str;
}

std::string SimpleXMLElem::XMLString(const std::string &strIn) {
  std::string str(strIn);
  size_t npos = str.find("<");
  while (npos != std::string::npos) {
    (void)str.replace(npos, 1, "&lt;");
    npos = str.find("<");
  }
  npos = str.find(">");
  while (npos != std::string::npos) {
    (void)str.replace(npos, 1, "&gt;");
    npos = str.find(">");
  }
  return str;
}

SimpleXMLElemMultiLine::SimpleXMLElemMultiLine(MapleAllocator &alloc, const std::string &cat)
    : SimpleXMLElem(alloc, cat), lines(alloc.Adapter()) {}

SimpleXMLElemMultiLine::SimpleXMLElemMultiLine(MapleAllocator &alloc, const std::string &cat, uint32 argID)
    : SimpleXMLElem(alloc, cat, argID), lines(alloc.Adapter()) {}

void SimpleXMLElemMultiLine::AddLine(const std::string &line) {
  lines.emplace_back(MapleString(line, allocator.GetMemPool()));
}

void SimpleXMLElemMultiLine::DumpImpl(std::ostream &os, const std::string &prefix) const {
  DumpHead(os, prefix);
  if (lines.size() > 0) {
    os << std::endl;
    for (const MapleString &str : lines) {
      os << prefix << "  " << str << std::endl;
    }
  }
  DumpTail(os, prefix);
  os << std::endl;
}

SimpleXML::SimpleXML(MapleAllocator &alloc) : allocator(alloc), roots(allocator.Adapter()) {}

void SimpleXML::DumpImpl(std::ostream &os) const {
  os << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>" << std::endl;
  for (auto it : roots) {
    it->Dump(os, "");
  }
}

void SimpleXML::AddRoot(SimpleXMLElem &elem) {
  roots.push_back(&elem);
}
}  // namespace maple
