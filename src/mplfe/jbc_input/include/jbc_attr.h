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
#ifndef MPLFE_INCLUDE_JBC_ATTR_H
#define MPLFE_INCLUDE_JBC_ATTR_H
#include "mempool_allocator.h"
#include "types_def.h"
#include "fe_configs.h"
#include "basic_io.h"
#include "jbc_class_const_pool.h"
#include "jbc_attr_item.h"
#include "jbc_opcode.h"

namespace maple {
namespace jbc {
const static uint32 kInvalidPC = UINT32_MAX;
const static uint32 kMaxPC32 = 0x0000FFFF;

enum JBCAttrKind : uint8 {
  kAttrUnknown,
  kAttrRaw,
#undef JBC_ATTR
#define JBC_ATTR(name, type) kAttr##type,
#include "jbc_attr.def"
#undef JBC_ATTR
};

class JBCAttr {
 public:
  JBCAttr(JBCAttrKind kindIn, uint16 nameIdxIn, uint32 lengthIn);
  virtual ~JBCAttr() = default;
  static JBCAttr *InAttr(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool);
  static JBCAttrKind AttrKind(const std::string &str);
  JBCAttrKind GetKind() const {
    return kind;
  }

  bool ParseFile(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) {
    return ParseFileImpl(allocator, io, constPool);
  }

  bool PreProcess(const JBCConstPool &constPool) {
    return PreProcessImpl(constPool);
  }

  SimpleXMLElem *GenXmlElem(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const {
    return GenXmlElemImpl(allocator, constPool, idx);
  }

 protected:
  virtual bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) = 0;
  virtual bool PreProcessImpl(const JBCConstPool &constPool) = 0;
  virtual SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const = 0;

  JBCAttrKind kind;
  uint16 nameIdx;
  uint32 length;
};

class JBCAttrMap {
 public:
  explicit JBCAttrMap(MapleAllocator &allocatorIn);
  ~JBCAttrMap() = default;
  void RegisterAttr(JBCAttr *attr);
  std::list<JBCAttr*> GetAttrs(JBCAttrKind kind) const;
  const JBCAttr *GetAttr(JBCAttrKind kind) const;
  bool PreProcess(const JBCConstPool &constPool);

 private:
  MapleAllocator &allocator;
  MapleMap<JBCAttrKind, MapleList<JBCAttr*>*> mapAttrs;
};

class JBCAttrRaw : public JBCAttr {
 public:
  JBCAttrRaw(MapleAllocator &allocator, uint16 nameIdx, uint32 length);
  ~JBCAttrRaw();

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint8 *rawData;
};

struct JavaAttrLocalVariableInfoItem {
  JavaAttrLocalVariableInfoItem() : slotIdx(0), start(0), length(0), nameIdx(0), typeNameIdx(0), signatureNameIdx(0) {}

  uint16 slotIdx;
  uint16 start;
  uint16 length;
  GStrIdx nameIdx;          // in java format
  GStrIdx typeNameIdx;      // in java format
  GStrIdx signatureNameIdx; // in java format
};

class JBCAttrLocalVariableInfo {
 public:
  explicit JBCAttrLocalVariableInfo(MapleAllocator &argAllocator);
  ~JBCAttrLocalVariableInfo() = default;
  void RegisterItem(const attr::LocalVariableTableItem &itemAttr);
  void RegisterTypeItem(const attr::LocalVariableTypeTableItem &itemAttr);
  const JavaAttrLocalVariableInfoItem &GetItemByStart(uint16 slotIdx, uint16 start) const;
  uint32 GetStart(uint16 slotIdx, uint16 pc) const;
  std::list<std::string> EmitToStrings() const;
  static bool IsInvalidLocalVariableInfoItem(const JavaAttrLocalVariableInfoItem &item);

 private:
  void AddSlotStartMap(uint16 slotIdx, uint16 startPC);
  void CheckItemAvaiable(uint16 slotIdx, uint16 start) const;
  JavaAttrLocalVariableInfoItem *GetItemByStartInternal(uint16 slotIdx, uint16 start);

  MapleAllocator &allocator;
  MapleMap<uint16, MapleSet<uint16>> slotStartMap;  // map<slotIdx, set<start>>
  MapleMap<std::pair<uint16, uint16>, JavaAttrLocalVariableInfoItem> itemMap;
  static JavaAttrLocalVariableInfoItem kInvalidInfoItem;
};

// ConstantValue Attribute
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.2
class JBCAttrConstantValue : public JBCAttr {
 public:
  JBCAttrConstantValue(MapleAllocator &allocator, uint16 nameIdx, uint32 length);
  ~JBCAttrConstantValue();
  const JBCConst *GetConstValue() const {
    return constValue;
  }

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 constIdx;
  JBCConst *constValue;
};

// Code Attribute
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.3
class JBCAttrLocalVariableInfo;
class JBCOp;
class JBCAttrCode : public JBCAttr {
 public:
  JBCAttrCode(MapleAllocator &allocator, uint16 nameIdx, uint32 length);
  ~JBCAttrCode();
  void InitLocalVarInfo();
  void SetLoadStoreType() const;
  const MapleMap<uint32, JBCOp*> &GetInstMap() const {
    return instructions;
  }

  uint16 GetMaxStack() const {
    return maxStack;
  }

  uint16 GetMaxLocals() const {
    return maxLocals;
  }

  uint32 GetCodeLength() const {
    return codeLength;
  }

  LLT_MOCK_TARGET const MapleVector<attr::ExceptionTableItem*> &GetExceptionInfos() const {
    return exceptions;
  }

  const JBCAttr *GetAttr(JBCAttrKind kind) const {
    return attrMap.GetAttr(kind);
  }

  const std::list<JBCAttr*> GetAttrs(JBCAttrKind kind) const {
    return attrMap.GetAttrs(kind);
  }

  std::list<std::string> GetLocalVarInfoByString() const {
    return localVarInfo.EmitToStrings();
  }

  const JBCAttrLocalVariableInfo &GetLocalVarInfo() const {
    return localVarInfo;
  }

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  bool ParseOpcodes(MapleAllocator &allocator);

  uint16 maxStack;
  uint16 maxLocals;
  uint32 codeLength;
  uint8 *code;
  uint16 nException;
  MapleVector<attr::ExceptionTableItem*> exceptions;
  uint16 nAttr;
  MapleVector<JBCAttr*> attrs;
  MapleMap<uint32, JBCOp*> instructions;
  JBCAttrMap attrMap;
  JBCAttrLocalVariableInfo localVarInfo;
};

// StackMapTable Attribute
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.4
class JBCAttrStackMapTable : public JBCAttr {
 public:
  JBCAttrStackMapTable(MapleAllocator &allocator, uint16 nameIdx, uint32 length);
  ~JBCAttrStackMapTable() = default;

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 count;
  MapleVector<attr::StackMapFrameItem*> entries;
};

// Exceptions Attribute
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.5
class JBCAttrException : public JBCAttr {
 public:
  JBCAttrException(MapleAllocator &allocator, uint16 nameIdx, uint32 length);
  ~JBCAttrException() = default;

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 count;
  MapleVector<uint16> tbExceptionIdx;
};

// InnerClass Attribute
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.6
class JBCAttrInnerClass : public JBCAttr {
 public:
  JBCAttrInnerClass(MapleAllocator &allocator, uint16 nameIdx, uint32 length);
  ~JBCAttrInnerClass() = default;

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 count;
  MapleVector<attr::InnerClassItem*> tbClasses;
};

// EnclosingMethod Attribute
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.7
class JBCAttrEnclosingMethod : public JBCAttr {
 public:
  JBCAttrEnclosingMethod(MapleAllocator &allocator, uint16 nameIdx, uint32 length);
  ~JBCAttrEnclosingMethod();
  const JBCConstClass *GetConstClass() const {
    return constClass;
  }

  const JBCConstNameAndType *GetConstNameAndType() const {
    return constNameAndType;
  }

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 classIdx;
  uint16 methodIdx;
  JBCConstClass *constClass;
  JBCConstNameAndType *constNameAndType;
};

// Synthetic Attribute
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.8
class JBCAttrSynthetic : public JBCAttr {
 public:
  JBCAttrSynthetic(MapleAllocator &allocator, uint16 nameIdx, uint32 length);
  ~JBCAttrSynthetic() = default;

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;
};

// Signature Attribute
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.9
class JBCAttrSignature : public JBCAttr {
 public:
  JBCAttrSignature(MapleAllocator &allocator, uint16 nameIdx, uint32 length);
  ~JBCAttrSignature();
  const JBCConstUTF8 *GetConstSignatureName() const {
    return constSignatureName;
  }

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 signatureIdx;
  JBCConstUTF8 *constSignatureName;
};

// SourceFile Attribute
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.10
class JBCAttrSourceFile : public JBCAttr {
 public:
  JBCAttrSourceFile(MapleAllocator &allocator, uint16 nameIdx, uint32 length);
  ~JBCAttrSourceFile();
  const JBCConstUTF8 *GetConstFileName() const {
    return constFileName;
  }

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 sourceFileIdx;
  const JBCConstUTF8 *constFileName;
};

// SourceDebugExtension Attribute
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.11
class JBCAttrSourceDebugEx : public JBCAttr {
 public:
  JBCAttrSourceDebugEx(MapleAllocator &allocator, uint16 nameIdx, uint32 length);
  ~JBCAttrSourceDebugEx();

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  char *data;
};

// LineNumberTable Attribute
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.12
class JBCAttrLineNumberTable : public JBCAttr {
 public:
  JBCAttrLineNumberTable(MapleAllocator &allocator, uint16 nameIdx, uint32 length);
  ~JBCAttrLineNumberTable() = default;
  const MapleVector<attr::LineNumberTableItem*> &GetLineNums() const {
    return lineNums;
  }

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 size;
  MapleVector<attr::LineNumberTableItem*> lineNums;
};

// LocalVariableTable Attribute
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.13
class JBCAttrLocalVariableTable : public JBCAttr {
 public:
  JBCAttrLocalVariableTable(MapleAllocator &allocator, uint16 nameIdx, uint32 length);
  ~JBCAttrLocalVariableTable() = default;
  const MapleVector<attr::LocalVariableTableItem*> &GetLocalVarInfos() const {
    return localVarInfos;
  }

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 size;
  MapleVector<attr::LocalVariableTableItem*> localVarInfos;
};

// LocalVariableTypeTable Attribute
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.14
class JBCAttrLocalVariableTypeTable : public JBCAttr {
 public:
  JBCAttrLocalVariableTypeTable(MapleAllocator &allocator, uint16 nameIdx, uint32 length);
  ~JBCAttrLocalVariableTypeTable() = default;
  const MapleVector<attr::LocalVariableTypeTableItem*> &GetLocalVarTypeInfos() const {
    return localVarTypeInfos;
  }

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 size;
  MapleVector<attr::LocalVariableTypeTableItem*> localVarTypeInfos;
};

// Deprecated Attribute
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.15
class JBCAttrDeprecated : public JBCAttr {
 public:
  JBCAttrDeprecated(MapleAllocator &allocator, uint16 nameIdx, uint32 length);
  ~JBCAttrDeprecated() = default;

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;
};

// RuntimeAnnoations Attribute
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.16
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.17
class JBCAttrRTAnnotations : public JBCAttr {
 public:
  JBCAttrRTAnnotations(MapleAllocator &allocator, JBCAttrKind kindIn, uint16 nameIdx, uint32 length);
  virtual ~JBCAttrRTAnnotations() = default;

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

  uint16 size;
  MapleVector<attr::Annotation*> annotations;
};

class JBCAttrRTVisAnnotations : public JBCAttrRTAnnotations {
 public:
  JBCAttrRTVisAnnotations(MapleAllocator &allocator, uint16 nameIdx, uint32 length);
  ~JBCAttrRTVisAnnotations() = default;
};

class JBCAttrRTInvisAnnotations : public JBCAttrRTAnnotations {
 public:
  JBCAttrRTInvisAnnotations(MapleAllocator &allocator, uint16 nameIdx, uint32 length);
  ~JBCAttrRTInvisAnnotations() = default;
};

// RuntimeParamAnnoations Attribute
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.18
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.19
class JBCAttrRTParamAnnotations : public JBCAttr {
 public:
  JBCAttrRTParamAnnotations(MapleAllocator &allocator, JBCAttrKind kindIn, uint16 nameIdx, uint32 length);
  virtual ~JBCAttrRTParamAnnotations() = default;

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

  uint8 size;
  MapleVector<attr::ParamAnnotationItem*> annotations;
};

class JBCAttrRTVisParamAnnotations : public JBCAttrRTParamAnnotations {
 public:
  JBCAttrRTVisParamAnnotations(MapleAllocator &allocator, uint16 nameIdx, uint32 length);
  ~JBCAttrRTVisParamAnnotations() {}
};

class JBCAttrRTInvisParamAnnotations : public JBCAttrRTParamAnnotations {
 public:
  JBCAttrRTInvisParamAnnotations(MapleAllocator &allocator, uint16 nameIdx, uint32 length);
  ~JBCAttrRTInvisParamAnnotations() {}
};

// RuntimeVisibleTypeAnnotations Attribute
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.20
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.21
class JBCAttrRTTypeAnnotations : public JBCAttr {
 public:
  JBCAttrRTTypeAnnotations(MapleAllocator &allocator, JBCAttrKind kindIn, uint16 nameIdx, uint32 length);
  virtual ~JBCAttrRTTypeAnnotations() = default;

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

  uint16 size;
  MapleVector<attr::TypeAnnotationItem*> annotations;
};

class JBCAttrRTVisTypeAnnotations : public JBCAttrRTTypeAnnotations {
 public:
  JBCAttrRTVisTypeAnnotations(MapleAllocator &allocator, uint16 nameIdx, uint32 length);
  ~JBCAttrRTVisTypeAnnotations() = default;
};

class JBCAttrRTInvisTypeAnnotations : public JBCAttrRTTypeAnnotations {
 public:
  JBCAttrRTInvisTypeAnnotations(MapleAllocator &allocator, uint16 nameIdx, uint32 length);
  ~JBCAttrRTInvisTypeAnnotations() = default;
};

// AnnotationDefault Attribute
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.22
class JBCAttrAnnotationDefault : public JBCAttr {
 public:
  JBCAttrAnnotationDefault(MapleAllocator &allocator, uint16 nameIdx, uint32 length);
  ~JBCAttrAnnotationDefault();

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  attr::ElementValue *value;
};

// BootstrapMethods Attribute
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.23
class JBCAttrBootstrapMethods : public JBCAttr {
 public:
  JBCAttrBootstrapMethods(MapleAllocator &allocator, uint16 nameIdx, uint32 length);
  ~JBCAttrBootstrapMethods() = default;

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 size;
  MapleVector<attr::BootstrapMethodItem*> methods;
};

// MethodParameters Attribute
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.24
class JBCAttrMethodParameters : public JBCAttr {
 public:
  JBCAttrMethodParameters(MapleAllocator &allocator, uint16 nameIdx, uint32 length);
  ~JBCAttrMethodParameters() = default;

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io, const JBCConstPool &constPool) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint8 size;
  MapleVector<attr::MethodParamItem*> params;
};
}  // namespace jbc
}  // namespace maple
#endif  // MPLFE_INCLUDE_JBC_ATTR_H
