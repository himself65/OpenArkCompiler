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
#include "mdparser.h"

namespace MDGen {
bool MDParser::ParseFile(const std::string &inputFile) {
  lexer.PrepareFile(inputFile);
  if (!ParseObjectStart()) {
    return false;
  }
  if (lexer.GetCurKind() == kMDEOF) {
    return true;
  }
  return EmitError("Unexpected input at begin");
}

bool MDParser::IsObjectStart(MDTokenKind k) const {
  return (k == kMDDef || k == kMDClass || k == kMDDefType);
}

bool MDParser::ParseObjectStart() {
  while (IsObjectStart(lexer.NextToken())) {
    if (!ParseObject()) {
      return false;
    }
  }
  return true;
}

bool MDParser::ParseObject() {
  switch (lexer.GetCurKind()) {
    case kMDDefType:
      return ParseDefType();
    case kMDClass:
      return ParseMDClass();
    case kMDDef:
      return ParseMDObject();
    default:
      return EmitError("Unexpected key word at start");
  }
}

bool MDParser::ParseDefType() {
  if (lexer.NextToken() != kMDIdentifier) {
    return EmitError("Expect a name after a specific type defined");
  }
  unsigned int defTypeIdx = dataKeeper.CreateStrInTable(lexer.GetStrToken(), kTypeName);
  if (defTypeIdx == UINT_MAX) {
    return EmitError("InValid defType is defined");
  }
  if (lexer.NextToken() != kMDEqual) {
    return EmitError("Expect a equal when a specific type is going to be instantiated");
  }
  std::set<unsigned int> defTypeMembers;
  while (lexer.NextToken() != kMDSemi) {
    switch (lexer.GetCurKind()) {
      case kMDIdentifier: {
        unsigned int defTypeMemberIdx = dataKeeper.CreateStrInTable(lexer.GetStrToken(), kTypeMemberName);
        if (defTypeMemberIdx == UINT_MAX || !defTypeMembers.insert(defTypeMemberIdx).second) {
          return EmitError("InValid defType member is defined");
        }
        break;
      }
      case kMDComma:
        break;
      default:
        return  EmitError("Unexpected token kind");
    }
  }
  dataKeeper.AddDefinedType(defTypeIdx, defTypeMembers);
  return (lexer.GetCurKind() == kMDSemi) ? true : EmitError("Expected an ending with a semicolon");
}

bool MDParser::ParseMDClass() {
  if (lexer.NextToken() != kMDIdentifier) {
    return EmitError("Expect a name after a specific class defined");
  }
  unsigned int classIdx = dataKeeper.CreateStrInTable(lexer.GetStrToken(), kClassName);
  if (classIdx == UINT_MAX) {
    return EmitError("InValid class name. Please change a class name");
  }
  bool isAnon = true;
  if (lexer.NextToken() == kMDColon) {
    isAnon = false;
    if (lexer.NextToken() != kMDIdentifier) {
      return EmitError("Expect a name after a specific class defined");
    }
    if (lexer.GetStrToken() != "string") {
      return EmitError("Only Support string as a class name type at current stage");
    }
    static_cast<void>(lexer.NextToken());
  }
  if (isAnon) {
    dataKeeper.ModifyStrTyInTable(lexer.GetStrToken(), kAnonClassName);
  }
  MDClass oneMDclass(classIdx, isAnon);
  if (lexer.GetCurKind() != kMDLess) {
    return EmitError("Expect a 'less' before class structure being defined");
  }

  while (lexer.NextToken() != kMDGreater) {
    if (!ParseMDClassBody(oneMDclass)) {
      return false;
    }
  }
  dataKeeper.AddMDClass(oneMDclass);
  return (lexer.NextToken() == kMDSemi) ? true : EmitError("Expected an ending with a semicolon");
}

bool MDParser::ParseMDClassBody(MDClass &oneClass) {
  switch (lexer.GetCurKind()) {
    case kMDIdentifier: {
      StrInfo defTypeInfo = dataKeeper.GetStrInTable(lexer.GetStrToken());
      if (defTypeInfo.idx == UINT_MAX || !oneClass.IsValidStructEle(defTypeInfo.sType)) {
        return EmitError("Expect a defined Type to be a memeber of a class");
      }
      bool isVec = false;
      if (lexer.ViewNextChar() == '[') {
        if (lexer.NextToken() != kMDOpenSquare || lexer.NextToken() != kMDCloseSquare) {
          return EmitError("Expect a \"[]\" to represent a list element");
        }
        isVec = true;
      }
      oneClass.BuildFormalTypes(defTypeInfo.idx, isVec);
      break;
    }
    case kMDComma:
      break;
    default:
      return EmitError("Unexpected token kind");
  }
  return true;
}

bool MDParser::ParseMDObject() {
  if (lexer.NextToken() != kMDIdentifier) {
    return EmitError("Expect a name after a specific object defined");
  }
  StrInfo parentInfo = dataKeeper.GetStrInTable(lexer.GetStrToken());
  if (parentInfo.idx == UINT_MAX || (parentInfo.sType != kClassName && parentInfo.sType != kAnonClassName)) {
    return EmitError("A new object should be belong to a defined class");
  }
  MDClass parentClass = dataKeeper.GetOneMDClass(parentInfo.idx);
  unsigned int objectIdx = UINT_MAX;
  if (!parentClass.IsAnonymousClass()) {
    if (lexer.NextToken() != kMDColon) {
      return EmitError("Expect a colon when a object name is going to be defined");
    }
    if (lexer.NextToken() != kMDIdentifier) {
      return EmitError("Expect a name for a specific object");
    }
    objectIdx = dataKeeper.CreateStrInTable(lexer.GetStrToken(), kObjectName);
    if (objectIdx == UINT_MAX) {
      return EmitError("InValid ObjectName!");
    }
  }
  MDObject *curObj = mdMemPool->New<MDObject>(objectIdx, parentClass, *mdMemPool);
  if (lexer.NextToken() != kMDOpenBrace) {
    return EmitError("Expect a OpenBrace before a object body is defined");
  }
  if (!ParseMDObjBody(*curObj)) {
    return false;
  }
  dataKeeper.FillMDClass(parentInfo.idx, *curObj);
  return (lexer.NextToken() == kMDSemi) ? true : EmitError("Expected an ending with a semicolon");
}

bool MDParser::ParseMDObjBody(MDObject &curObj) {
  bool hasDefault = false;
  for (size_t i = 0; i < curObj.GetParentClass()->GetFormalTypeSize(); ++i) {
    if (hasDefault) {
      DefaultElement *defaultEle = mdMemPool->New<DefaultElement>();
      curObj.AddMDElements(defaultEle);
      continue;
    }
    MDTokenKind curKind = lexer.NextToken();
    if (i != 0 && (curKind != kMDComma && curKind != kMDCloseBrace)) {
      return EmitError("Unexpected Gramma when define a object");
    }
    if (curKind == kMDComma) {
      curKind = lexer.NextToken();
    }
    if (curKind == kMDCloseBrace) {
      hasDefault = true;
      DefaultElement *defaultEle = mdMemPool->New<DefaultElement>();
      curObj.AddMDElements(defaultEle);
      continue;
    }
    unsigned int typeIdx = curObj.GetParentClass()->GetFormalTypes().at(i).first;
    bool isVec = curObj.GetParentClass()->GetFormalTypes().at(i).second;
    if (dataKeeper.GetStrTyByIdx(typeIdx) == kIntType) {
      if (!ParseIntElement(curObj, isVec)) {
        return false;
      }
    } else if (dataKeeper.GetStrTyByIdx(typeIdx) == kStringType) {
      if (!ParseStrElement(curObj, isVec)) {
        return false;
      }
    } else if (dataKeeper.GetStrTyByIdx(typeIdx) == kTypeName) {
      std::set<unsigned int> childSet = dataKeeper.GetOneSpcType(typeIdx);
      if (!ParseDefTyElement(curObj, isVec, childSet)) {
        return false;
      }
    } else if (dataKeeper.GetStrTyByIdx(typeIdx) == kClassName) {
      MDClass pClass = dataKeeper.GetOneMDClass(typeIdx);
      if (!ParseDefObjElement(curObj, isVec, pClass)) {
        return false;
      }
    }
  }
  if (lexer.GetCurKind() == kMDCloseBrace) {
    return true;
  }
  return (lexer.NextToken() != kMDCloseBrace) ? EmitError("Expect a CloseBrace as end of object definition") : true;
}

bool MDParser::ParseIntElement(MDObject &curObj, bool isVec) {
  if (isVec) {
    if (lexer.GetCurKind() != kMDOpenSquare) {
      return EmitError("Expect a OpenSquare before a list element defined");
    }

    VecElement *curEle = mdMemPool->New<VecElement>(*mdMemPool);
    while (lexer.NextToken() != kMDCloseSquare) {
      switch (lexer.GetCurKind()) {
        case kMDIntVal: {
          IntElement *singleEle = mdMemPool->New<IntElement>(lexer.GetIntVal());
          curEle->appendElement(singleEle);
          break;
        }
        case kMDComma:
          break;
        default:
          return EmitError("Unexpected token kind");
      }
    }
    curObj.AddMDElements(curEle);
  } else {
    if (lexer.GetCurKind() != kMDIntVal) {
      return EmitError("Expect a integer elemet as defined");
    }
    IntElement *curEle = mdMemPool->New<IntElement>(lexer.GetIntVal());
    curObj.AddMDElements(curEle);
  }
  return true;
}

bool MDParser::ParseStrElement(MDObject &curObj, bool isVec) {
  if (isVec) {
    if (lexer.GetCurKind() != kMDOpenSquare) {
      return EmitError("Expect a OpenSquare before a list element defined");
    }
    VecElement *curEle = mdMemPool->New<VecElement>(*mdMemPool);
    while (lexer.NextToken() != kMDCloseSquare) {
      switch (lexer.GetCurKind()) {
        case kMDIdentifier: {
          unsigned int elementIdx = dataKeeper.CreateStrInTable(lexer.GetStrToken(), kElementName);
          if (elementIdx == UINT_MAX) {
            return EmitError("Duplicate string name has already been defined");
          }
          StringElement *singleEle = mdMemPool->New<StringElement>(elementIdx);
          curEle->appendElement(singleEle);
          break;
        }
        case kMDComma:
          break;
        default:
          return EmitError("Unexpected token kind");
      }
    }
    curObj.AddMDElements(curEle);
  } else {
    if (lexer.GetCurKind() != kMDIdentifier) {
      return EmitError("Expect a string elemet as defined");
    }
    unsigned int elementIdx = dataKeeper.CreateStrInTable(lexer.GetStrToken(), kElementName);
    if (elementIdx == UINT_MAX) {
      return EmitError("Duplicate string name has already been defined");
    }
    StringElement *curEle = mdMemPool->New<StringElement>(elementIdx);
    curObj.AddMDElements(curEle);
  }
  return true;
}

bool MDParser::ParseDefTyElement(MDObject &curObj, bool isVec, const std::set<unsigned int> &childSet) {
  if (isVec) {
    if (lexer.GetCurKind() != kMDOpenSquare) {
      return EmitError("Expect a OpenSquare before a list element defined");
    }
    VecElement *curEle = mdMemPool->New<VecElement>(*mdMemPool);
    while (lexer.NextToken() != kMDCloseSquare) {
      switch (lexer.GetCurKind()) {
        case kMDIdentifier: {
          StrInfo defTypeInfo = dataKeeper.GetStrInTable(lexer.GetStrToken());
          DefTyElement *singleEle = mdMemPool->New<DefTyElement>();
          if (!singleEle->SetContent(defTypeInfo, childSet)) {
            return EmitError("Expect a input element which has been defined as a type");
          }
          curEle->appendElement(singleEle);
          break;
        }
        case kMDComma:
          break;
        default:
          return EmitError("Unexpected token kind");
      }
    }
    curObj.AddMDElements(curEle);
  } else {
    if (lexer.GetCurKind() != kMDIdentifier) {
      return EmitError("Expect a string elemet as defined");
    }
    StrInfo defTypeInfo = dataKeeper.GetStrInTable(lexer.GetStrToken());
    DefTyElement *curEle = mdMemPool->New<DefTyElement>();
    if (!curEle->SetContent(defTypeInfo, childSet)) {
      return EmitError("Expect a input element which has been defined as a type");
    }
    curObj.AddMDElements(curEle);
  }
  return true;
}

bool MDParser::ParseDefObjElement(MDObject &curObj, bool isVec, const MDClass &pClass) {
  if (isVec) {
    if (lexer.GetCurKind() != kMDOpenSquare) {
      return EmitError("Expect a OpenSquare before a list element defined");
    }
    VecElement *curEle = mdMemPool->New<VecElement>(*mdMemPool);
    while (lexer.NextToken() != kMDCloseSquare) {
      switch (lexer.GetCurKind()) {
        case kMDIdentifier: {
          StrInfo defObjInfo = dataKeeper.GetStrInTable(lexer.GetStrToken());
          DefObjElement *singleEle = mdMemPool->New<DefObjElement>();
          if (!singleEle->SetContent(defObjInfo, pClass)) {
            return EmitError("Expect a input element which has been defined as a object");
          }
          curEle->appendElement(singleEle);
          break;
        }
        case kMDComma:
          break;
        default:
          return EmitError("Unexpected token kind");
      }
    }
    curObj.AddMDElements(curEle);
  } else {
    if (lexer.GetCurKind() != kMDIdentifier) {
      return EmitError("Expect a integer elemet as defined");
    }
    StrInfo defObjInfo = dataKeeper.GetStrInTable(lexer.GetStrToken());
    DefObjElement *curEle = mdMemPool->New<DefObjElement>();
    if (!curEle->SetContent(defObjInfo, pClass)) {
      return EmitError("Expect a input element which has been defined as a object");
    }
    curObj.AddMDElements(curEle);
  }
  return true;
}

bool MDParser::EmitError(const std::string &errMsg) {
  maple::LogInfo::MapleLogger() << errMsg <<  "\n";
  maple::LogInfo::MapleLogger() << "A Error Appear At Line " << lexer.GetLineNumber() << "\n";
  maple::LogInfo::MapleLogger() << "Source code : " << lexer.GetStrLine() << "\n";
  return false;
}
} /* namespace MDGen */
