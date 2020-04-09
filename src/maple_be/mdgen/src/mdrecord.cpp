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
#include "mdrecord.h"

namespace MDGen {
static int inValidStrIdx = -1;

bool DefTyElement::SetContent(StrInfo curInfo, std::set<int> &childTySet) {
  if (!childTySet.count(curInfo.idx)) {
    return false;
  }
  elementIdx = curInfo.idx;
  return true;
}

bool DefObjElement::SetContent(StrInfo curInfo, MDClass &parentClass) {
  if (!parentClass.IsClassMember(curInfo.idx)) {
    return false;
  }
  elementIdx = curInfo.idx;
  return true;
}

const MDElement *MDObject::GetOneMDElement (int index) const {
  CHECK_FATAL(index < mdElements.size(), "Array boundary check failed");
  return mdElements[index];
}

const MDObject &MDClass::GetOneMDObject(int index) const{
  CHECK_FATAL(index < mdObjects.size(), "Array boundary check failed");
  return mdObjects[index];
}

void MDClass::AddClassMember(MDObject inputObj) {
  mdObjects.push_back(inputObj);
  childObjNames.insert(inputObj.GetIdx());
}

bool MDClass::IsClassMember(int curIdx) {
  return childObjNames.count(curIdx);
}

void MDClass::BuildFormalTypes(int memberIdx, bool isVec) {
  formalTypes.push_back(std::make_pair(memberIdx, isVec));
}

bool MDClass::IsValidStructEle(RecordType curTy) const {
  return (curTy == kTypeName || curTy == kClassName || curTy == kIntType || curTy == kStringType);
}

int MDClassRange::CreateStrInTable(const std::string &inStr, RecordType curTy) {
  StrInfo curInfo (totalStr, curTy);
  auto ret = stringHashTable.insert(std::make_pair(inStr, curInfo));
  if (ret.second) {
    int temp = totalStr;
    stringTable.push_back(inStr);
    ++totalStr;
    return temp;
  }
  return inValidStrIdx;
}

StrInfo MDClassRange::GetStrInTable(const std::string &inStr) {
  auto ret = stringHashTable.find(inStr);
  StrInfo inValidInfo (-1, kUndefinedStr);
  return (ret != stringHashTable.end()) ? ret->second : inValidInfo;
}

RecordType MDClassRange::GetStrTyByIdx(int curIdx) {
  CHECK_FATAL(curIdx < stringTable.size(), "Array boundary check failed");
  return GetStrInTable(stringTable[curIdx]).sType;
}

const std::string &MDClassRange::GetStrByIdx(int curIdx) {
  CHECK_FATAL(curIdx < stringTable.size(), "Array boundary check failed");
  return stringTable[curIdx];
}

void MDClassRange::ModifyStrTyInTable(const std::string &inStr, RecordType newTy) {
  auto ret = stringHashTable.find(inStr);
  CHECK_FATAL(ret != stringHashTable.end(), "find string failed!");
  ret->second.sType = newTy;
}

void MDClassRange::AddDefinedType(int typesName, std::set<int> typesSet) {
  definedTypes.insert(std::make_pair(typesName, typesSet));
}

void MDClassRange::AddMDClass(MDClass curClass) {
  allClasses.insert(std::make_pair(curClass.GetClassIdx(), curClass));
}

void MDClassRange::FillMDClass(int givenIdx, const MDObject &insertObj) {
  auto ret = allClasses.find(givenIdx);
  CHECK_FATAL(ret != allClasses.end(), "Cannot achieve target MD Class");
  ret->second.AddClassMember(insertObj);
}

MDClass MDClassRange::GetOneMDClass(int givenIdx) {
  auto ret = allClasses.find(givenIdx);
  CHECK_FATAL(ret != allClasses.end(), "Cannot achieve target MD Class");
  return ret->second;
}

std::set<int> MDClassRange::GetOneSpcType(int givenTyIdx) {
  auto ret = definedTypes.find(givenTyIdx);
  CHECK_FATAL(ret != definedTypes.end(), "Cannot achieve a defined type");
  return ret->second;
}
} /* namespace MDGen */
