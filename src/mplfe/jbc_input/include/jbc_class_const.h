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
#ifndef MPLFE_INCLUDE_JBC_CLASS_CONST_H
#define MPLFE_INCLUDE_JBC_CLASS_CONST_H
#include <map>
#include <string>
#include "mempool_allocator.h"
#include "maple_string.h"
#include "global_tables.h"
#include "types_def.h"
#include "simple_xml.h"
#include "basic_io.h"
#include "fe_struct_elem_info.h"
#include "feir_type.h"

namespace maple {
namespace jbc {
// Const Kind
enum JBCConstTag : uint8 {
  kConstUnknown = 0,
  kConstUTF8 = 1,
  kConstInteger = 3,
  kConstFloat = 4,
  kConstLong = 5,
  kConstDouble = 6,
  kConstClass = 7,
  kConstString = 8,
  kConstFieldRef = 9,
  kConstMethodRef = 10,
  kConstInterfaceMethodRef = 11,
  kConstNameAndType = 12,
  kConstMethodHandleInfo = 15,
  kConstMethodType = 16,
  kConstInvokeDynamic = 18,
};

enum JavaRefKind : uint8 {
  kRefGetField = 1,
  kRefGetStatic = 2,
  kRefPutField = 3,
  kRefPutStatic = 4,
  kRefInvokeVirtual = 5,
  kRefInvokeStatic = 6,
  kRefInvokeSpecial = 7,
  kRefNewInvokeSpecial = 8,
  kRefInvokeInterface = 9,
};

class JBCConstTagName {
 public:
  JBCConstTagName() = default;
  ~JBCConstTagName() = default;
  static std::map<JBCConstTag, std::string> InitTagNameMap();
  static std::string GetTagName(JBCConstTag tag);

 private:
  static std::map<JBCConstTag, std::string> tagNameMap;
};

class JBCConstPool;
class JBCConst {
 public:
  JBCConst(MapleAllocator &alloc, JBCConstTag t) : tag(t) {}
  virtual ~JBCConst() = default;
  static JBCConst *InConst(MapleAllocator &alloc, BasicIORead &io);
  static std::string InternalNameToFullName(const std::string &name);
  static std::string FullNameToInternalName(const std::string &name);
  JBCConstTag GetTag() const {
    return tag;
  }

  bool IsWide() const {
  return (tag == kConstLong || tag == kConstDouble);
  }

  bool IsValue() const {
    return (tag == kConstInteger || tag == kConstLong || tag == kConstFloat || tag == kConstDouble);
  }

  bool IsValue4Byte() const {
    return (tag == kConstInteger || tag == kConstFloat);
  }

  bool IsValue8Byte() const {
    return (tag == kConstLong || tag == kConstDouble);
  }

  bool ParseFile(BasicIORead &io) {
    return ParseFileImpl(io);
  }

  bool PreProcess(const JBCConstPool &constPool) {
    return PreProcessImpl(constPool);
  }

  SimpleXMLElem *GenXMLElem(MapleAllocator &alloc, uint32 id) {
    return GenXMLElemImpl(alloc, id);
  }

 protected:
  virtual bool ParseFileImpl(BasicIORead &io) = 0;
  virtual bool PreProcessImpl(const JBCConstPool &constPool) = 0;
  virtual SimpleXMLElem *GenXMLElemImpl(MapleAllocator &alloc, uint32 id) const;
  const std::string MapleStringToStd(const MapleString &mapleStr) const {
    return mapleStr.length() == 0 ? "" : std::string(mapleStr.c_str());
  }

  JBCConstTag tag;
};

using JBCConstPoolIdx = uint16;

class JBCConstUTF8 : public JBCConst {
 public:
  JBCConstUTF8(MapleAllocator &alloc, JBCConstTag t);
  JBCConstUTF8(MapleAllocator &alloc, JBCConstTag t, const std::string &argStr);
  ~JBCConstUTF8() = default;

  const std::string GetString() const {
    return MapleStringToStd(str);
  }

  GStrIdx GetStrIdx() const {
    return strIdx;
  }

 protected:
  bool ParseFileImpl(BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXMLElemImpl(MapleAllocator &alloc, uint32 id) const override;

 private:
  uint16 length;
  GStrIdx strIdx;
  MapleString str;
};

class JBCConst4Byte : public JBCConst {
 public:
  JBCConst4Byte(MapleAllocator &alloc, JBCConstTag t);
  JBCConst4Byte(MapleAllocator &alloc, JBCConstTag t, int32 arg);
  JBCConst4Byte(MapleAllocator &alloc, JBCConstTag t, float arg);
  ~JBCConst4Byte() = default;
  int32 GetInt32() const {
    return value.ivalue;
  }

  float GetFloat() const {
    return value.fvalue;
  }

  uint32 GetRaw() const {
    return value.raw;
  }

 protected:
  bool ParseFileImpl(BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXMLElemImpl(MapleAllocator &alloc, uint32 id) const override;

 private:
  union {
    int32 ivalue;
    float fvalue;
    uint32 raw;
  } value;
};

class JBCConst8Byte : public JBCConst {
 public:
  JBCConst8Byte(MapleAllocator &alloc, JBCConstTag t);
  JBCConst8Byte(MapleAllocator &alloc, JBCConstTag t, int64 arg);
  JBCConst8Byte(MapleAllocator &alloc, JBCConstTag t, double arg);
  ~JBCConst8Byte() = default;

  int64 GetInt64() const {
    return value.lvalue;
  }

  double GetDouble() const {
    return value.dvalue;
  }

  uint64 GetRaw() const {
    return value.raw;
  }

 protected:
  bool ParseFileImpl(BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXMLElemImpl(MapleAllocator &alloc, uint32 id) const override;

 private:
  union {
    int64 lvalue;
    double dvalue;
    uint64 raw;
  } value;
};

class JBCConstClass : public JBCConst {
 public:
  JBCConstClass(MapleAllocator &alloc, JBCConstTag t);
  JBCConstClass(MapleAllocator &alloc, JBCConstTag t, JBCConstPoolIdx argNameIdx);
  ~JBCConstClass();

  GStrIdx GetClassNameIdxOrin() const {
    return strIdxOrin;
  }

  GStrIdx GetClassNameIdxMpl() const {
    return strIdxMpl;
  }

  const std::string GetClassNameOrin() const {
    return MapleStringToStd(nameOrin);
  }

  const std::string GetClassNameMpl() const {
    return MapleStringToStd(nameMpl);
  }

  const FEIRType *GetFEIRType() const {
    return feType;
  }

 protected:
  bool ParseFileImpl(BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXMLElemImpl(MapleAllocator &alloc, uint32 id) const override;

 private:
  struct {
    JBCConstPoolIdx nameIdx;
  } rawData;
  const JBCConstUTF8 *constUTF8;
  GStrIdx strIdxOrin;
  GStrIdx strIdxMpl;
  MapleString nameOrin;
  MapleString nameMpl;
  FEIRType *feType = nullptr;
};

class JBCConstString : public JBCConst {
 public:
  JBCConstString(MapleAllocator &alloc, JBCConstTag t);
  JBCConstString(MapleAllocator &alloc, JBCConstTag t, JBCConstPoolIdx argStringIdx);
  ~JBCConstString() = default;
  void SetValue(const GStrIdx &argStrIdx) {
    strIdx = argStrIdx;
    str = GlobalTables::GetStrTable().GetStringFromStrIdx(strIdx);
  }

  GStrIdx GetStrIdx() const {
    return strIdx;
  }

  const std::string GetString() const {
    return MapleStringToStd(str);
  }

 protected:
  bool ParseFileImpl(BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXMLElemImpl(MapleAllocator &alloc, uint32 id) const override;

 private:
  struct {
    JBCConstPoolIdx stringIdx;
  } rawData;
  GStrIdx strIdx;
  MapleString str;
};

class JBCConstNameAndType : public JBCConst {
 public:
  JBCConstNameAndType(MapleAllocator &alloc, JBCConstTag t);
  JBCConstNameAndType(MapleAllocator &alloc, JBCConstTag t, JBCConstPoolIdx argNameIdx, JBCConstPoolIdx argDescIdx);
  ~JBCConstNameAndType();

  const std::string GetName() const {
    CHECK_FATAL(constName != nullptr, "invalid const index");
    return constName->GetString();
  }

  const std::string GetDesc() const {
    CHECK_FATAL(constDesc != nullptr, "invalid const index");
    return constDesc->GetString();
  }

 protected:
  bool ParseFileImpl(BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXMLElemImpl(MapleAllocator &alloc, uint32 id) const override;

 private:
  struct {
    JBCConstPoolIdx nameIdx;
    JBCConstPoolIdx descIdx;
  } rawData;
  const JBCConstUTF8 *constName;
  const JBCConstUTF8 *constDesc;
};

class JBCConstRef : public JBCConst {
 public:
  JBCConstRef(MapleAllocator &alloc, JBCConstTag t);
  JBCConstRef(MapleAllocator &alloc, JBCConstTag t, JBCConstPoolIdx argClassIdx,
              JBCConstPoolIdx argClassNameAndTypeIdx);
  ~JBCConstRef();
  bool PrepareFEStructElemInfo();
  const std::string GetName() const;
  const std::string GetDesc() const;

  const JBCConstClass *GetConstClass() const {
    return constClass;
  }

  FEStructElemInfo *GetFEStructElemInfo() const {
    CHECK_NULL_FATAL(feStructElemInfo);
    return feStructElemInfo;
  }

  const FEIRType *GetOwnerFEIRType() const {
    CHECK_NULL_FATAL(constClass);
    return constClass->GetFEIRType();
  }

 protected:
  bool ParseFileImpl(BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXMLElemImpl(MapleAllocator &alloc, uint32 id) const override;

 private:
  struct {
    JBCConstPoolIdx classIdx;
    JBCConstPoolIdx nameAndTypeIdx;
  } rawData;
  const JBCConstClass *constClass;
  const JBCConstNameAndType *constNameAndType;
  FEStructElemInfo *feStructElemInfo;
};

class JBCConstMethodHandleInfo : public JBCConst {
 public:
  JBCConstMethodHandleInfo(MapleAllocator &alloc, JBCConstTag t);
  ~JBCConstMethodHandleInfo();

 protected:
  bool ParseFileImpl(BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXMLElemImpl(MapleAllocator &alloc, uint32 id) const override;

 private:
  struct {
    JBCConstPoolIdx refKind;
    JBCConstPoolIdx refIdx;
  } rawData;
  const JBCConstRef *constRef;
};

class JBCConstMethodType : public JBCConst {
 public:
  JBCConstMethodType(MapleAllocator &alloc, JBCConstTag t);
  ~JBCConstMethodType();

 protected:
  bool ParseFileImpl(BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXMLElemImpl(MapleAllocator &alloc, uint32 id) const override;

 private:
  struct {
    JBCConstPoolIdx descIdx;
  } rawData;
  const JBCConstUTF8 *constDesc;
};

class JBCConstInvokeDynamic : public JBCConst {
 public:
  JBCConstInvokeDynamic(MapleAllocator &alloc, JBCConstTag t);
  ~JBCConstInvokeDynamic();

  const JBCConstNameAndType *GetConstNameAndType() const {
    return constNameAndType;
  }

 protected:
  bool ParseFileImpl(BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXMLElemImpl(MapleAllocator &alloc, uint32 id) const override;

 private:
  struct {
    JBCConstPoolIdx bsmAttrIdx;
    JBCConstPoolIdx nameAndTypeIdx;
  } rawData;
  const JBCConstNameAndType *constNameAndType;
};
}  // namespace jbc
}  // namespace maple
#endif  // MPLFE_INCLUDE_JBC_CLASS_CONST_H
