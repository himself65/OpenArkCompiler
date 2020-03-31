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
#ifndef MPLFE_INCLUDE_JBC_CLASS_H
#define MPLFE_INCLUDE_JBC_CLASS_H
#include "mempool_allocator.h"
#include "fe_configs.h"
#include "jbc_class_access.h"
#include "jbc_class_header.h"
#include "jbc_class_const_pool.h"
#include "jbc_attr.h"

namespace maple {
namespace jbc {
class JBCClass;
class JBCClassElem {
 public:
  JBCClassElem(MapleAllocator &allocator, const JBCClass &argKlass);
  virtual ~JBCClassElem() = default;
  bool ParseFile(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool);
  const JBCConstPool &GetConstPool() const;
  std::string GetFullName() const;
  LLT_MOCK_TARGET uint16 GetAccessFlag() const {
    return accessFlag;
  }

  LLT_MOCK_TARGET bool IsStatic() const {
    return (accessFlag & kAccFieldStatic) != 0;
  }

  LLT_MOCK_TARGET std::string GetClassName() const;

  LLT_MOCK_TARGET std::string GetName(const JBCConstPool &constPool) const {
    return constPool.GetStringByConstUTF8(nameIdx);
  }

  LLT_MOCK_TARGET std::string GetName() const {
    return GetConstPool().GetStringByConstUTF8(nameIdx);
  }

  LLT_MOCK_TARGET std::string GetDescription(const JBCConstPool &constPool) const {
    return constPool.GetStringByConstUTF8(descIdx);
  }

  LLT_MOCK_TARGET std::string GetDescription() const {
    return GetConstPool().GetStringByConstUTF8(descIdx);
  }

  uint16 GetNameIdx() const {
    return nameIdx;
  }

  uint16 GetDescriptionIdx() const {
    return descIdx;
  }

  const JBCClass &GetClass() const {
    return klass;
  }

  const JBCAttr *GetAttr(JBCAttrKind kind) const {
    return attrMap.GetAttr(kind);
  }

  const std::list<JBCAttr*> GetAttrs(JBCAttrKind kind) const {
    return attrMap.GetAttrs(kind);
  }

  SimpleXMLElem *GenXmlElem(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) {
    return GenXmlElemImpl(allocator, constPool, idx);
  }

 protected:
  virtual SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) = 0;

  const JBCClass &klass;
  uint16 accessFlag;
  uint16 nameIdx;
  uint16 descIdx;
  uint16 nAttr;
  MapleVector<JBCAttr*> attrs;
  JBCAttrMap attrMap;
};

class JBCClassField : public JBCClassElem {
 public:
  JBCClassField(MapleAllocator &allocator, const JBCClass &argKlass);
  ~JBCClassField() = default;

 protected:
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) override;
};

class JBCClassMethod : public JBCClassElem {
 public:
  JBCClassMethod(MapleAllocator &allocator, const JBCClass &argKlass);
  ~JBCClassMethod() = default;
  bool PreProcess();
  const JBCAttrCode *GetCode() const;

 protected:
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) override;
};

class JBCClass {
 public:
  JBCClass(MapleAllocator &allocatorIn);
  ~JBCClass() = default;

  bool ParseFile(BasicIORead &io);
  bool PreProcess();
  GStrIdx GetClassNameIdxOrin() const;
  GStrIdx GetClassNameIdxMpl() const;
  LLT_MOCK_TARGET std::string GetClassNameOrin() const;
  LLT_MOCK_TARGET std::string GetClassNameMpl() const;
  LLT_MOCK_TARGET std::string GetSourceFileName() const;
  LLT_MOCK_TARGET std::string GetSuperClassName() const;
  LLT_MOCK_TARGET std::vector<std::string> GetInterfaceNames() const;
  static JBCClass *InClass(MapleAllocator &allocator, BasicIORead &io);

  uint16 GetAccessFlag() const {
    return header.accessFlag;
  }

  const JBCConstPool &GetConstPool() const {
    return constPool;
  }

  uint16 GetConstPoolCount() const {
    return header.constPoolCount;
  }

  uint16 GetFieldCount() const {
    return header.fieldsCount;
  }

  uint16 GetMethodCount() const {
    return header.methodsCount;
  }

  uint16 GetAttrCount() const {
    return header.attrsCount;
  }

  bool IsInterface() const {
    return (header.accessFlag & JBCClassAccessFlag::kAccClassInterface) != 0;
  }

  const MapleVector<JBCClassField*> &GetFields() const {
    return tbFields;
  }

  const MapleVector<JBCClassMethod*> &GetMethods() const {
    return tbMethods;
  }

  void SetFilePathName(const std::string &name) {
    filePathName = name;
  }

  const std::string GetFilePathName() const {
    return filePathName.length() == 0 ? "" : std::string(filePathName.c_str());
  }

  const std::string GetFileName() const {
    return fileName.length() == 0 ? "" : std::string(fileName.c_str());
  }

  void SetSrcFileInfoIdx(uint32 idx) {
    srcFileInfoIdx = idx;
  }

  uint32 GetSrcFileInfoIdx() const {
    return srcFileInfoIdx;
  }

  const JBCAttr *GetAttr(JBCAttrKind kind) const {
    return attrMap.GetAttr(kind);
  }

  std::list<JBCAttr*> GetAttrs(JBCAttrKind kind) const {
    return attrMap.GetAttrs(kind);
  }

 LLT_PRIVATE:
  void InitHeader();
  bool ParseFileForConstPool(BasicIORead &io);
  bool ParseFileForFields(BasicIORead &io);
  bool ParseFileForMethods(BasicIORead &io);
  bool ParseFileForAttrs(BasicIORead &io);

  struct {
    uint32 magic;
    uint16 minorVersion;
    uint16 majorVersion;
    uint16 constPoolCount;
    uint16 accessFlag;
    uint16 thisClass;
    uint16 superClass;
    uint16 interfacesCount;
    uint16 fieldsCount;
    uint16 methodsCount;
    uint16 attrsCount;
  } header;
  MapleAllocator &allocator;
  JBCConstPool constPool;
  MapleVector<uint16> tbInterfaces;
  MapleVector<JBCClassField*> tbFields;
  MapleVector<JBCClassMethod*> tbMethods;
  MapleVector<JBCAttr*> tbAttrs;
  JBCAttrMap attrMap;
  MapleString filePathName;
  MapleString fileName;
  uint32 srcFileInfoIdx = 0;
};
}  // namespace jbc
}  // namespace maple
#endif  // MPLFE_INCLUDE_JBC_CLASS_H
