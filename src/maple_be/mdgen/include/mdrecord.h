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
  int idx;
  RecordType sType;
  StrInfo(int curIdx, RecordType curTy) : idx(curIdx), sType(curTy) {}
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

  virtual int GetContent() const = 0;
  ElementTy GetRecDataTy() const {
    return eleType;
  }

 protected:
  ElementTy eleType = kEleInValidTy;
};

class DefaultElement : public MDElement {
 public:
  DefaultElement() {
    eleType = kEleDefaultTy;
  }
  ~DefaultElement() override = default;

  int GetContent() const override {
    CHECK_FATAL(false, "Cannnot load default element's content");
    return -1;
  }
};

class IntElement : public MDElement {
 public:
  explicit IntElement(int curVal) : intEleVal(curVal) {
    eleType = kEleIntTy;
  }
  ~IntElement() override = default;
  int GetContent() const override {
    return intEleVal;
  }

 private:
  int intEleVal;
};

class StringElement : public MDElement {
 public:
  explicit StringElement(int curIdx) : strElemntIdx(curIdx) {
    eleType = kEleStrTy;
  }
  ~StringElement() override = default;
  int GetContent() const override {
    return strElemntIdx;
  }
 private:
  int strElemntIdx;
};

class DefTyElement : public MDElement {
 public:
  DefTyElement() {
    eleType = kEleDefTyTy;
  }
  ~DefTyElement() override = default;
  int GetContent() const override {
    return elementIdx;
  }
  bool SetContent(StrInfo curInfo, std::set<int> &childTySet);

 private:
  int elementIdx = -1;
};

class DefObjElement : public MDElement {
 public:
  DefObjElement() {
    eleType = kEleDefObjTy;
  }
  ~DefObjElement() override = default;
  int GetContent() const override {
    return elementIdx;
  }
  bool SetContent(StrInfo curInfo, MDClass &parentClass);

 private:
  int elementIdx = -1;
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
  int GetContent() const override {
    CHECK_FATAL(false, "Vector element does not have a single content");
    return -1;
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
};

class MDObject {
 public:
  MDObject(int curIdx, MDClass &pClass, maple::MemPool &memPool)
      : objectIdx(curIdx), parentClass(&pClass), alloc(&memPool), mdElements(alloc.Adapter()) {}
  ~MDObject() = default;

  const MDElement *GetOneMDElement(int index) const;
  void AddMDElements(MDElement* curElement) {
    mdElements.push_back(curElement);
  }
  int GetIdx() const {
    return objectIdx;
  }
  const MDClass *GetParentClass() const {
    return parentClass;
  }

 private:
  int objectIdx;
  MDClass *parentClass;
  maple::MapleAllocator alloc;
  maple::MapleVector<MDElement*> mdElements;
};

class MDClass {
 public:
  MDClass(int classIdx, bool isAnonymous) {
    this->classIdx = classIdx;
    this->isAnonymous = isAnonymous;
  }
  ~MDClass() = default;

  const MDObject &GetOneMDObject(int index) const;
  void AddClassMember(MDObject inputObj);
  bool IsClassMember(int curIdx);
  bool IsValidStructEle(RecordType curTy) const;
  int GetClassIdx() const {
    return classIdx;
  }
  bool IsAnonymousClass() const {
    return isAnonymous;
  }
  const std::vector<std::pair<int, bool>> GetFormalTypes() const {
    return formalTypes;
  }
  const std::set<int> GetchildObjNames() const {
    return childObjNames;
  }
  size_t GetFormalTypeSize() const {
    return formalTypes.size();
  }
  size_t GetMDObjectSize() const {
    return mdObjects.size();
  }
  void BuildFormalTypes(int memberIdx, bool isVec);

 private:
  int classIdx;
  bool isAnonymous;
  std::vector<MDObject> mdObjects;
  std::vector<std::pair<int, bool>> formalTypes;
  std::set<int> childObjNames;
};

class MDClassRange {
 public:
  explicit MDClassRange(std::string module) : moduleName(module) {
    stringTable.clear();
    stringHashTable.clear();
    /* init common types such as int ,string , float */
    std::set<int> initTypes;
    AddDefinedType(CreateStrInTable("int", kIntType), initTypes);
    AddDefinedType(CreateStrInTable("string", kStringType), initTypes);
  }
  ~MDClassRange() = default;

  StrInfo GetStrInTable(const std::string &inStr);
  RecordType GetStrTyByIdx(int curIdx);
  const std::string &GetStrByIdx(int curIdx);
  void AddMDClass(MDClass curClass);
  MDClass GetOneMDClass(int givenIdx);
  std::set<int> GetOneSpcType(int givenTyIdx);
  size_t GetStringTableSize() const {
    return stringTable.size();
  }
  int CreateStrInTable(const std::string &inStr, RecordType curTy);
  void ModifyStrTyInTable(const std::string &inStr, RecordType newTy);
  void AddDefinedType(int typesName, std::set<int> typesSet);
  void FillMDClass(int givenIdx, const MDObject &insertObj);

 private:
  std::string moduleName;
  std::unordered_map<std::string, StrInfo> stringHashTable;
  std::vector<std::string> stringTable;
  int totalStr = 0;
  std::unordered_map<int, std::set<int>> definedTypes;
  std::unordered_map<int, MDClass> allClasses;
};
} /* namespace MDGen */

#endif /* MAPLEBE_MDGEN_INCLUDE_MDREORD_H */
