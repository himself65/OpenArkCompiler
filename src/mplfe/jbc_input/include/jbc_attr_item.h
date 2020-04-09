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
#ifndef MPLFE_INCLUDE_JBC_ATTR_ITEM_H
#define MPLFE_INCLUDE_JBC_ATTR_ITEM_H
#include <map>
#include <string>
#include "mempool_allocator.h"
#include "types_def.h"
#include "jbc_class_const_pool.h"

namespace maple {
namespace jbc {
namespace attr {
// ElementValueItem
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.16
enum ElementValueKind : uint8 {
  kElementValueDefault = 0,
  kElementValueConst,
  kElementValueEnum,
  kElementValueClassInfo,
  kElementValueAnnotation,
  kElementValueArray
};

class JBCAttrItem {
 public:
  JBCAttrItem() = default;
  virtual ~JBCAttrItem() = default;
  bool ParseFile(MapleAllocator &allocator, BasicIORead &io) {
    return ParseFileImpl(allocator, io);
  }

  bool PreProcess(const JBCConstPool &constPool) {
    return PreProcessImpl(constPool);
  }

  SimpleXMLElem *GenXmlElem(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const {
    return GenXmlElemImpl(allocator, constPool, idx);
  }

 protected:
  virtual bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) = 0;
  virtual bool PreProcessImpl(const JBCConstPool &constPool) = 0;
  virtual SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const = 0;
};

// ExceptiionTableItem in Code Attr
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.3
class ExceptionTableItem : public JBCAttrItem {
 public:
  ExceptionTableItem();
  ~ExceptionTableItem();
  uint16 GetStartPC() const {
    return startPC;
  }

  uint16 GetEndPC() const {
    return endPC;
  }

  uint16 GetHandlerPC() const {
    return handlerPC;
  }

  const JBCConstClass *GetCatchType() const {
    return catchType;
  }

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 startPC;
  uint16 endPC;
  uint16 handlerPC;
  uint16 catchTypeIdx;
  const JBCConstClass *catchType;
};

// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.4
enum VerificationTypeInfoTag : uint8 {
  kVerTypeInfoItemTop = 0,
  kVerTypeInfoItemInteger = 1,
  kVerTypeInfoItemFloat = 2,
  kVerTypeInfoItemDouble = 3,
  kVerTypeInfoItemLong = 4,
  kVerTypeInfoItemNull = 5,
  kVerTypeInfoItemUninitializedThis = 6,
  kVerTypeInfoItemObject = 7,
  kVerTypeInfoItemUninitialized = 8,
  kVerTypeInfoItemUnknown = 0xFF,
};

class VerificationTypeInfo : public JBCAttrItem {
 public:
  VerificationTypeInfo();
  ~VerificationTypeInfo();
  static std::map<VerificationTypeInfoTag, std::string> InitTagNameMap();
  static std::string TagName(VerificationTypeInfoTag t);
  const JBCConstClass &GetClassInfoRef() const {
    CHECK_NULL_FATAL(classInfo);
    return *classInfo;
  }

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  VerificationTypeInfoTag tag;
  union {
    uint16 cpoolIdx;
    uint16 offset;
    uint16 raw;
  } data;
  JBCConstClass *classInfo;
  static std::map<VerificationTypeInfoTag, std::string> tagNameMap;
};

enum StackMapFrameItemTag : uint8 {
  kStackSame,
  kStackSameLocals1StackItem,
  kStackSameLocals1StackItemEx,
  kStackChop,
  kStackSameFrameEx,
  kStackAppend,
  kStackFullFrame,
  kStackReserved,
};

class StackMapFrameItem : public JBCAttrItem {
 public:
  StackMapFrameItem(uint8 frameTypeIn, StackMapFrameItemTag tagIn);
  virtual ~StackMapFrameItem() = default;
  static std::map<StackMapFrameItemTag, std::string> InitTagName();
  static std::string TagName(StackMapFrameItemTag tag);
  static StackMapFrameItemTag FrameType2Tag(uint8 frameType);
  static StackMapFrameItem *NewItem(MapleAllocator &allocator, BasicIORead &io, uint8 frameType);

 protected:
  StackMapFrameItemTag tag;
  uint8 frameType;
  static std::map<StackMapFrameItemTag, std::string> tagNameMap;
};

class StackMapFrameItemSame : public StackMapFrameItem {
 public:
  explicit StackMapFrameItemSame(uint8 frameType);
  ~StackMapFrameItemSame() = default;

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;
};

class StackMapFrameItemSameLocals1 : public StackMapFrameItem {
 public:
  explicit StackMapFrameItemSameLocals1(uint8 frameType);
  ~StackMapFrameItemSameLocals1();

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  VerificationTypeInfo *stack;
};

class StackMapFrameItemSameLocals1Ex : public StackMapFrameItem {
 public:
  explicit StackMapFrameItemSameLocals1Ex(uint8 frameType);
  ~StackMapFrameItemSameLocals1Ex();

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 offsetDelta;
  VerificationTypeInfo *stack;
};

class StackMapFrameItemChop : public StackMapFrameItem {
 public:
  explicit StackMapFrameItemChop(uint8 frameType);
  ~StackMapFrameItemChop() = default;

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 offsetDelta;
};

class StackMapFrameItemSameEx : public StackMapFrameItem {
 public:
  explicit StackMapFrameItemSameEx(uint8 frameType);
  ~StackMapFrameItemSameEx() = default;

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 offsetDelta;
};

class StackMapFrameItemAppend : public StackMapFrameItem {
 public:
  StackMapFrameItemAppend(MapleAllocator &allocator, uint8 frameType);
  ~StackMapFrameItemAppend() = default;

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 offsetDelta;
  MapleVector<VerificationTypeInfo*> locals;
};

class StackMapFrameItemFull : public StackMapFrameItem {
 public:
  StackMapFrameItemFull(MapleAllocator &allocator, uint8 frameType);
  ~StackMapFrameItemFull() = default;

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 offsetDelta;
  uint16 nLocals;
  MapleVector<VerificationTypeInfo*> locals;
  uint16 nStacks;
  MapleVector<VerificationTypeInfo*> stacks;
};

// InnerClassItem in Attr InnerClass
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.6
class InnerClassItem : public JBCAttrItem {
 public:
  InnerClassItem();
  ~InnerClassItem();
  const JBCConstClass *GetConstClassInnerRef() const {
    return constClassInner;
  }

  const JBCConstClass *GetConstClassOuter() const {
    return constClassOuter;
  }

  const JBCConstUTF8 *GetConstNameInner() const {
    return constNameInner;
  }

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 innerClassInfoIdx;
  uint16 outerClassInfoIdx;
  uint16 innerNameIdx;
  uint16 innerClassAccessFlag;
  JBCConstClass *constClassInner;
  JBCConstClass *constClassOuter;
  JBCConstUTF8 *constNameInner;
};

// LineNumberTableItem in Attr LineNumberTable
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.12
class LineNumberTableItem : public JBCAttrItem {
 public:
  LineNumberTableItem();
  ~LineNumberTableItem() = default;
  uint16 GetStartPC() const {
    return startPC;
  }

  uint16 GetLineNumber() const {
    return lineNumber;
  }

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 startPC;
  uint16 lineNumber;
};

// LocalVariableTableItem in Attr LocalVariableTable
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.13
class LocalVariableTableItem : public JBCAttrItem {
 public:
  LocalVariableTableItem();
  ~LocalVariableTableItem();
  const JBCConstUTF8 *GetConstName() const {
    return constName;
  }

  const JBCConstUTF8 *GetConstDesc() const {
    return constDesc;
  }

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 startPC;
  uint16 length;
  uint16 nameIdx;
  uint16 descIdx;
  uint16 index;
  JBCConstUTF8 *constName;
  JBCConstUTF8 *constDesc;
};

// LocalVariableTypeTableItem in Attr LocalVariableTypeTable
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.14
class LocalVariableTypeTableItem : public JBCAttrItem {
 public:
  LocalVariableTypeTableItem();
  ~LocalVariableTypeTableItem();
  const JBCConstUTF8 *GetConstName() const {
    return constName;
  }

  const JBCConstUTF8 *GetConstSignature() const {
    return constSignature;
  }

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 startPC;
  uint16 length;
  uint16 nameIdx;
  uint16 signatureIdx;
  uint16 index;
  JBCConstUTF8 *constName;
  JBCConstUTF8 *constSignature;
};

class ElementValueItem : public JBCAttrItem {
 public:
  ElementValueItem(ElementValueKind kindIn, char tagIn);
  virtual ~ElementValueItem() = default;
  static std::map<char, ElementValueKind> InitTagKindMap();
  static std::map<ElementValueKind, std::string> InitKindNameMap();
  static std::string KindName(ElementValueKind kind);
  static ElementValueKind TagToKind(char tag);
  static ElementValueItem *NewItem(MapleAllocator &allocator, BasicIORead &io, char tag);

 protected:
  ElementValueKind kind;
  char tag;
  static std::map<char, ElementValueKind> tagKindMap;
  static std::map<ElementValueKind, std::string> kindNameMap;
};

class ElementValue : public JBCAttrItem {
 public:
  ElementValue();
  ~ElementValue();
  ElementValueKind GetKind() const {
    return kind;
  }

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  char tag;
  ElementValueKind kind;
  ElementValueItem *value;
};

class ElementValuePair : public JBCAttrItem {
 public:
  ElementValuePair();
  ~ElementValuePair();

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 nameIdx;
  ElementValue *value;
};

// Annotation
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.16
class Annotation : public JBCAttrItem {
 public:
  explicit Annotation(MapleAllocator &allocator);
  ~Annotation();
  const JBCConstUTF8 *GetConstTypeName() const {
    return constTypeName;
  }

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 typeIdx;
  uint16 nElemPairs;
  MapleVector<ElementValuePair*> tbElemPairs;
  JBCConstUTF8 *constTypeName;
};

// ParamAnnotationItem in Attr ParamAnnotation
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.18
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.19
class ParamAnnotationItem : public JBCAttrItem {
 public:
  explicit ParamAnnotationItem(MapleAllocator &allocator);
  ~ParamAnnotationItem() = default;

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 count;
  MapleVector<Annotation*> annotations;
};

// BootstrapMethodItem in Attr BootstrapMethod
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.23
class BootstrapMethodItem : public JBCAttrItem {
 public:
  explicit BootstrapMethodItem(MapleAllocator &allocator);
  ~BootstrapMethodItem();
  const JBCConstMethodHandleInfo *GetConstMethodHandleInfo() const {
    return methodHandle;
  }

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 methodRefIdx;
  uint16 nArgs;
  MapleVector<uint16> argsIdx;
  JBCConstMethodHandleInfo *methodHandle;
  MapleVector<JBCConst*> args;
};

// MethodParamItem in Attr MethodParam
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.24
class MethodParamItem : public JBCAttrItem {
 public:
  MethodParamItem();
  ~MethodParamItem();
  const JBCConstUTF8 *GetConstName() const {
    return constName;
  }

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 nameIdx;
  uint16 accessFlag;
  JBCConstUTF8 *constName;
};

class ElementValuePair;

class ElementValueConst : public ElementValueItem {
 public:
  explicit ElementValueConst(uint8 t);
  ~ElementValueConst();
  const JBCConst *GetConstValue() const {
    return constValue;
  }

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 constValueIdx;
  JBCConst *constValue;
};

class ElementValueEnum : public ElementValueItem {
 public:
  ElementValueEnum();
  ~ElementValueEnum();
  const JBCConstUTF8 *GetConstTypeName() const {
    return constTypeName;
  }

  const JBCConstUTF8 *GetConstName() const {
    return constName;
  }

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 typeNameIdx;
  uint16 constNameIdx;
  JBCConstUTF8 *constTypeName;
  JBCConstUTF8 *constName;
};

class ElementValueClassInfo : public ElementValueItem {
 public:
  ElementValueClassInfo();
  ~ElementValueClassInfo();
  const JBCConstUTF8 *GetConstClassInfo() const {
    return constClassInfo;
  }

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 classInfoIdx;
  JBCConstUTF8 *constClassInfo;
};

class ElementValueAnnotation : public ElementValueItem {
 public:
  ElementValueAnnotation();
  ~ElementValueAnnotation();

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  Annotation *annotation;
};

class ElementValueArray : public ElementValueItem {
 public:
  explicit ElementValueArray(MapleAllocator &allocator);
  ~ElementValueArray() = default;

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 size;
  MapleVector<ElementValue*> values;
};

// TargetInfo
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.20.1
enum TargetInfoItemTag : uint8 {
  kTargetTagUndefine,
  kTargetTagTypeParam,
  kTargetTagSuperType,
  kTargetTagTypeParamBound,
  kTargetTagEmpty,
  kTargetTagFormalParam,
  kTargetTagThrows,
  kTargetTagLocalVar,
  kTargetTagCatch,
  kTargetTagOffset,
  kTargetTagTypeArg,
};

enum TargetInfoItemType : uint8 {
  kTargetTypeParamClass = 0x00,
  kTargetTypeParamMethod = 0x01,
  kTargetTypeHierarchy = 0x10,
  kTargetTypeBoundClass = 0x11,
  kTargetTypeBoundMethod = 0x12,
  kTargetTypeFieldDecl = 0x13,
  kTargetTypeReturn = 0x14,
  kTargetTypeReceiver = 0x15,
  kTargetTypeFormal = 0x16,
  kTargetTypeThrows = 0x17,
  kTargetTypeLocalVar = 0x40,
  kTargetTypeResourceVar = 0x41,
  kTargetTypeExpectionParam = 0x42,
  kTargetTypeInstanceof = 0x43,
  kTargetTypeNew = 0x44,
  kTargetTypeMethodRefNew = 0x45,
  kTargetTypeMethodRefIdentifier = 0x46,
  kTargetTypeCast = 0x47,
  kTargetTypeConstructorInvoke = 0x48,
  kTargetTypeMethodInvoke = 0x49,
  kTargetTypeConstructorNew = 0x4A,
  kTargetTypeConstructorIdentifier = 0x4B,
};

class TargetInfoItem : public JBCAttrItem {
 public:
  explicit TargetInfoItem(TargetInfoItemTag tagIn);
  virtual ~TargetInfoItem() = default;
  static std::map<TargetInfoItemType, TargetInfoItemTag> InitTypeTagMap();
  static TargetInfoItemTag TargetType2Tag(TargetInfoItemType type);
  static TargetInfoItem *NewItem(MapleAllocator &allocator, BasicIORead &io, TargetInfoItemType targetType);

 protected:
  TargetInfoItemTag tag;
  static std::map<TargetInfoItemType, TargetInfoItemTag> typeTagMap;
};

class TargetTypeParam : public TargetInfoItem {
 public:
  TargetTypeParam();
  ~TargetTypeParam() = default;

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint8 paramIdx;
};

class TargetSuperType : public TargetInfoItem {
 public:
  TargetSuperType();
  ~TargetSuperType() = default;

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 index;
};

class TargetTypeParamBound : public TargetInfoItem {
 public:
  TargetTypeParamBound();
  ~TargetTypeParamBound() = default;

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint8 paramIdx;
  uint8 boundIdx;
};

class TargetEmpty : public TargetInfoItem {
 public:
  TargetEmpty();
  ~TargetEmpty() = default;

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;
};

class TargetFormalParam : public TargetInfoItem {
 public:
  TargetFormalParam();
  ~TargetFormalParam() = default;

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint8 paramIdx;
};

class TargetThrows : public TargetInfoItem {
 public:
  TargetThrows();
  ~TargetThrows() = default;

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 typeIdx;
};

class TargetLocalVarItem : public TargetInfoItem {
 public:
  TargetLocalVarItem();
  ~TargetLocalVarItem() = default;

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 startPC;
  uint16 length;
  uint16 index;
};

class TargetLocalVar : public TargetInfoItem {
 public:
  explicit TargetLocalVar(MapleAllocator &allocator);
  ~TargetLocalVar() = default;

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 size;
  MapleVector<TargetLocalVarItem*> table;
};

class TargetCatch : public TargetInfoItem {
 public:
  TargetCatch();
  ~TargetCatch() = default;

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 exTableIdx;
};

class TargetOffset : public TargetInfoItem {
 public:
  TargetOffset();
  ~TargetOffset() = default;

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 offset;
};

class TargetTypeArg : public TargetInfoItem {
 public:
  TargetTypeArg();
  ~TargetTypeArg() = default;

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint16 offset;
  uint8 typeArgIdx;
};

class TypePathItem : public JBCAttrItem {
 public:
  TypePathItem();
  ~TypePathItem() = default;

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint8 typePathKind;
  uint8 typeArgIdx;
};

// TypePath
// ref: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.20.2
class TypePath : public JBCAttrItem {
 public:
  explicit TypePath(MapleAllocator &allocator);
  ~TypePath() = default;

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  uint8 pathLength;
  MapleVector<TypePathItem*> tbPath;
};

class TypeAnnotationItem : public JBCAttrItem {
 public:
  explicit TypeAnnotationItem(MapleAllocator &allocator);
  ~TypeAnnotationItem();

 protected:
  bool ParseFileImpl(MapleAllocator &allocator, BasicIORead &io) override;
  bool PreProcessImpl(const JBCConstPool &constPool) override;
  SimpleXMLElem *GenXmlElemImpl(MapleAllocator &allocator, const JBCConstPool &constPool, uint32 idx) const override;

 private:
  TargetInfoItemType targetType;
  TargetInfoItem *targetInfo;
  TypePath *targetPath;
  uint16 typeIdx;
  uint16 nElemPairs;
  MapleVector<ElementValuePair*> elemPairs;
};
}  // namespace attr
}  // namespace jbc
}  // namespace maple
#endif  // MPLFE_INCLUDE_JBC_ATTR_ITEM_H