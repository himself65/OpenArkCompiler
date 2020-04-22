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
#ifndef MAPLEBE_MDGEN_INCLUDE_MDREORD_H
#define MAPLEBE_MDGEN_INCLUDE_MDREORD_H

#include <vector>
#include <unordered_map>
#include <map>
#include <set>
#include <limits.h>
#include "mempool_allocator.h"
#include "mempool.h"
#include "mpl_logging.h"
#include "types_def.h"

/* Define base data structure which is used to store information in  .md files */
namespace MDGen {
class MDClass; /* circular dependency */

enum RecordType : maple::uint32 {
  kClassName,
  kAnonClassName,
  kObjectName,
  kElementName,
  kIntType,
  kStringType,
  kTypeName,
  kTypeMemberName,
  kUndefinedStr
};

struct StrInfo {
  unsigned int idx;
  RecordType sType;
  StrInfo(unsigned int curIdx, RecordType curTy) : idx(curIdx), sType(curTy) {}
};

class MDElement {
 public:
  MDElement() = default;
  virtual ~MDElement() = default;
  enum ElementTy : maple::uint32 {
    kEleIntTy,
    kEleStrTy,
    kEleDefTyTy,
    kEleDefObjTy,
    kEleVecTy,
    kEleDefaultTy,
    kEleInValidTy
  };

  unsigned int GetContent() const {
    return DoGetContent();
  }

  ElementTy GetRecDataTy() const {
    return eleType;
  }

 protected:
  ElementTy eleType = kEleInValidTy;

 private:
  virtual unsigned int DoGetContent() const = 0;
};

class DefaultElement : public MDElement {
 public:
  DefaultElement() {
    eleType = kEleDefaultTy;
  }

  ~DefaultElement() override = default;

 private:
  unsigned int DoGetContent() const override {
    CHECK_FATAL(false, "Cannnot load default element's content");
    return UINT_MAX;
  }
};

class IntElement : public MDElement {
 public:
  explicit IntElement(unsigned int curVal) : intEleVal(curVal) {
    eleType = kEleIntTy;
  }

  ~IntElement() override = default;

 private:
  unsigned int intEleVal;
  unsigned int DoGetContent() const override {
    return intEleVal;
  }
};

class StringElement : public MDElement {
 public:
  explicit StringElement(unsigned int curIdx) : strElemntIdx(curIdx) {
    eleType = kEleStrTy;
  }

  ~StringElement() override = default;

 private:
  unsigned int strElemntIdx;
  unsigned int DoGetContent() const override {
    return strElemntIdx;
  }
};

class DefTyElement : public MDElement {
 public:
  DefTyElement() {
    eleType = kEleDefTyTy;
  }

  ~DefTyElement() override = default;

  bool SetContent(const StrInfo curInfo, const std::set<unsigned int> &childTySet);

 private:
  unsigned int elementIdx = UINT_MAX;
  unsigned int DoGetContent() const override {
    return elementIdx;
  }
};

class DefObjElement : public MDElement {
 public:
  DefObjElement() {
    eleType = kEleDefObjTy;
  }

  ~DefObjElement() override = default;

  bool SetContent(const StrInfo curInfo, const MDClass &parentClass);

 private:
  unsigned int elementIdx = UINT_MAX;
  unsigned int DoGetContent() const override {
    return elementIdx;
  }
};

class VecElement : public MDElement {
 public:
  explicit VecElement(maple::MemPool &mem) : alloc(&mem), vecData(alloc.Adapter()) {
    eleType = kEleVecTy;
  }

  ~VecElement() override = default;

  void appendElement(MDElement *curElement) {
    vecData.push_back(curElement);
  }

  const maple::MapleVector<MDElement*> GetVecData() const {
    return vecData;
  }

  size_t GetVecDataSize() const {
    return vecData.size();
  }

 private:
  maple::MapleAllocator alloc;
  maple::MapleVector<MDElement*> vecData;

  unsigned int DoGetContent() const override {
    CHECK_FATAL(false, "Vector element does not have a single content");
    return UINT_MAX;
  }
};

class MDObject {
 public:
  MDObject(unsigned int curIdx, MDClass &pClass, maple::MemPool &memPool)
      : objectIdx(curIdx), parentClass(&pClass), alloc(&memPool), mdElements(alloc.Adapter()) {}

  ~MDObject() = default;

  const MDElement *GetOneMDElement(size_t index) const;

  void AddMDElements(MDElement* curElement) {
    mdElements.push_back(curElement);
  }

  unsigned int GetIdx() const {
    return objectIdx;
  }

  const MDClass *GetParentClass() const {
    return parentClass;
  }

 private:
  unsigned int objectIdx;
  MDClass *parentClass;
  maple::MapleAllocator alloc;
  maple::MapleVector<MDElement*> mdElements;
};

class MDClass {
 public:
  MDClass(unsigned int classIdx, bool isAnonymous) {
    this->classIdx = classIdx;
    this->isAnonymous = isAnonymous;
  }
  ~MDClass() = default;

  const MDObject &GetOneMDObject(size_t index) const;
  void AddClassMember(MDObject inputObj);
  bool IsClassMember(unsigned int curIdx) const;
  bool IsValidStructEle(RecordType curTy) const;
  unsigned int GetClassIdx() const {
    return classIdx;
  }
  bool IsAnonymousClass() const {
    return isAnonymous;
  }
  const std::vector<std::pair<uint, bool>> GetFormalTypes() const {
    return formalTypes;
  }
  const std::set<unsigned int> GetchildObjNames() const {
    return childObjNames;
  }
  size_t GetFormalTypeSize() const {
    return formalTypes.size();
  }
  size_t GetMDObjectSize() const {
    return mdObjects.size();
  }
  void BuildFormalTypes(unsigned int memberIdx, bool isVec);

 private:
  unsigned int classIdx;
  bool isAnonymous;
  std::vector<MDObject> mdObjects;
  std::vector<std::pair<uint, bool>> formalTypes;
  std::set<unsigned int> childObjNames;
};

class MDClassRange {
 public:
  explicit MDClassRange(std::string module) : moduleName(module) {
    stringTable.clear();
    stringHashTable.clear();
    /* init common types such as unsigned int ,string , float */
    std::set<unsigned int> initTypes;
    AddDefinedType(CreateStrInTable("int", kIntType), initTypes);
    AddDefinedType(CreateStrInTable("string", kStringType), initTypes);
  }
  ~MDClassRange() = default;

  StrInfo GetStrInTable(const std::string &inStr);
  RecordType GetStrTyByIdx(size_t curIdx);
  const std::string &GetStrByIdx(size_t curIdx);
  void AddMDClass(MDClass curClass);
  MDClass GetOneMDClass(unsigned int givenIdx);
  std::set<unsigned int> GetOneSpcType(unsigned int givenTyIdx);
  size_t GetStringTableSize() const {
    return stringTable.size();
  }
  unsigned int CreateStrInTable(const std::string &inStr, RecordType curTy);
  void ModifyStrTyInTable(const std::string &inStr, RecordType newTy);
  void AddDefinedType(unsigned int typesName, std::set<unsigned int> typesSet);
  void FillMDClass(unsigned int givenIdx, const MDObject &insertObj);

 private:
  std::string moduleName;
  std::unordered_map<std::string, StrInfo> stringHashTable;
  std::vector<std::string> stringTable;
  unsigned int totalStr = 0;
  std::unordered_map<uint, std::set<unsigned int>> definedTypes;
  std::unordered_map<uint, MDClass> allClasses;
};
} /* namespace MDGen */

#endif /* MAPLEBE_MDGEN_INCLUDE_MDREORD_H */
