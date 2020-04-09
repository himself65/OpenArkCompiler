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
#ifndef MPLFE_INCLUDE_SIMPLE_XML_H
#define MPLFE_INCLUDE_SIMPLE_XML_H
#include <string>
#include <ios>
#include <iostream>
#include "types_def.h"
#include "mempool_allocator.h"
#include "maple_string.h"

namespace maple {
enum SimpleXMLElemIdTag {
  kXMLIDNone,
  kXMLID,
};

enum SimpleXMLElemValueTag {
  kXMLValueNone,
  kXMLValueI8,
  kXMLValueU8,
  kXMLValueI16,
  kXMLValueU16,
  kXMLValueI32,
  kXMLValueU32,
  kXMLValueI64,
  kXMLValueU64,
  kXMLValueFloat,
  kXMLValueDouble,
  kXMLValueString,
};

class SimpleXMLElem {
 public:
  SimpleXMLElem(MapleAllocator &alloc, const std::string &cat);
  SimpleXMLElem(MapleAllocator &alloc, const std::string &cat, uint32 argID);
  virtual ~SimpleXMLElem() = default;
  void AddElement(SimpleXMLElem &elem);
  SimpleXMLElem *AddNewElement(const std::string &cat);
  SimpleXMLElem *AddNewElement(const std::string &cat, uint32 argID);
  void SetValue(int8 v);
  void SetValue(uint8 v);
  void SetValue(int16 v);
  void SetValue(uint16 v);
  void SetValue(int32 v);
  void SetValue(uint32 v);
  void SetValue(int64 v);
  void SetValue(uint64 v);
  void SetValue(float v);
  void SetValue(double v);
  void SetValue(const std::string &str);
  void SetContent(const std::string &str);
  static std::string XMLString(const std::string &strIn);
  void Dump(std::ostream &os, const std::string &prefix = "") const {
    return DumpImpl(os, prefix);
  }

 protected:
  virtual void DumpImpl(std::ostream &os, const std::string &prefix) const;
  void DumpHead(std::ostream &os, const std::string &prefix = "") const;
  void DumpTail(std::ostream &os, const std::string &prefix = "") const;

  SimpleXMLElemIdTag tagID;
  SimpleXMLElemValueTag tagValue;
  MapleAllocator &allocator;
  std::string catalog;
  uint32 id;
  union {
    int8 i8;
    uint8 u8;
    int16 i16;
    uint16 u16;
    int32 i32;
    uint32 u32;
    int64 i64;
    uint64 u64;
    float f;
    double d;
    uint64 raw;
  } value;
  std::string valueString;
  std::string content;
  MapleList<SimpleXMLElem*> elems;
};

class SimpleXMLElemMultiLine : public SimpleXMLElem {
 public:
  SimpleXMLElemMultiLine(MapleAllocator &alloc, const std::string &cat);
  SimpleXMLElemMultiLine(MapleAllocator &alloc, const std::string &cat, uint32 argID);
  ~SimpleXMLElemMultiLine() = default;
  void AddLine(const std::string &line);

 protected:
  void DumpImpl(std::ostream &os, const std::string &prefix) const override;

 private:
  MapleList<MapleString> lines;
};

class SimpleXML {
 public:
  SimpleXML(MapleAllocator &alloc);
  ~SimpleXML() = default;
  void AddRoot(SimpleXMLElem &elem);

 protected:
  void DumpImpl(std::ostream &os) const;

 private:
  MapleAllocator &allocator;
  MapleList<SimpleXMLElem*> roots;
};
}  // namespace maple
#endif
