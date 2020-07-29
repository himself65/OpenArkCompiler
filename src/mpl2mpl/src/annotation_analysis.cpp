/*
 * Copyright (c) [2019-2020] Huawei Technologies Co.,Ltd.All rights reserved.
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
#include "annotation_analysis.h"

namespace maple {
char AnnotationAnalysis::annoDeclare = ':';
char AnnotationAnalysis::annoSemiColon = ';';
int AnnotationType::atime = 0;
MIRStructType *GetClassTypeFromName(const std::string &className) {
  GStrIdx strIdx = GlobalTables::GetStrTable().GetStrIdxFromName(className);
  if (strIdx == 0) {
    strIdx = GlobalTables::GetStrTable().GetStrIdxFromName("Ljava_2Flang_2FObject_3B");
  }
  TyIdx classTyIdx = GlobalTables::GetTypeNameTable().GetTyIdxFromGStrIdx(strIdx);
  MIRType &classType = GetTypeFromTyIdx(classTyIdx);
  return &static_cast<MIRStructType&>(classType);
}

void GenericDeclare::Dump() {
  std::cout << GetName() << ":";
  if (defaultType->GetKind() == kGenericDeclare) {
    std::cout << "T" << defaultType->GetName() << ";";
  } else if (defaultType->GetKind() == kGenericType) {
    defaultType->Dump();
  } else {
    CHECK_FATAL(false, "must be");
  }
}

void GenericType::Dump() {
  std::string s = namemangler::DecodeName(GetName());
  s.pop_back();
  std::cout << s;
  if (GenericArg.size()) {
    std::cout << "<";
    for (AnnotationType *real : ArgOrder) {
      if (real->GetKind() == kGenericType) {
        real->Dump();
      } else if (real->GetKind() == kGenericDeclare) {
        std::cout << "T" << real->GetName() << ";";
      } else {
        real->Dump();
      }
    }
    std::cout << ">";
  }
  std::cout << ";";
}

void AnnotationType::Dump() {
  if (kind == kGenericMatch) {
    std::cout << "*";
  } else {
    std::cout << GetName();
  }
}

void GenericType::ReWriteType(std::string &subClass) {
  std::string className = mirStructType->GetName();
  size_t ClassMethodSplitterStrSize = strlen(namemangler::kClassMethodSplitterStr);
  (void)className.replace(className.size() - ClassMethodSplitterStrSize, ClassMethodSplitterStrSize, subClass);
  mirStructType = GetClassTypeFromName(className);
  typeName = GlobalTables::GetStrTable().GetStrIdxFromName(className);
  GenericArg.clear();
  ArgOrder.clear();
}

GenericDeclare *AnnotationParser::GetOrCreateDeclare(GStrIdx gStrIdx, MemPool &mp, bool check, MIRStructType *sType) {
  if (created.find(gStrIdx) != created.end()) {
    return created[gStrIdx];
  }
  if (check) {
    if (sType != nullptr) {
      for (GenericDeclare *gd : sType->GetGenericDeclare()) {
        if (gd->GetGStrIdx() == gStrIdx) {
          return gd;
        }
      }
    }
    return nullptr;
  }
  GenericDeclare *gd = mp.New<GenericDeclare>(gStrIdx);
  created[gStrIdx] = gd;
  return gd;
}

ATokenKind AnnotationParser::GetNextToken(const char *endC) {
  if (curIndex == signature.size()) {
    ++curIndex;
    return kEnd;
  }
  CHECK_FATAL(curIndex < signature.size(), "must be");
  if (endC == nullptr) {
    switch (signature[curIndex++]) {
      case '<':
        return kTemplateStart;
      case '>':
        return kTemplateEnd;
      case '(':
        return kArgumentStart;
      case ')':
        return kArgumentEnd;
      case '+':
        return kExtend;
      case '-':
        return kSuper;
      case '*':
        return kMatch;
      case '[':
        return kArray;
      case 'L':
        return kAClassName;
      case 'T':
        return kTemplateName;
      case 'I':
      case 'Z':
      case 'D':
      case 'F':
      case 'V':
      case 'S':
      case 'C':
      case 'B':
      case 'J':
        curStrToken = signature[curIndex - 1];
        return kPrimeTypeName;
      case '.':
        return kSubClass;
      case ';':
        return kSemiComma;
      default:
        curStrToken = signature[curIndex - 1];
        return kChar;
    }
  }
  size_t endPos = signature.find_first_of(endC, curIndex);
  CHECK_FATAL(endPos != std::string::npos, "must be");
  curStrToken = signature.substr(curIndex, endPos - curIndex);
  curIndex = endPos + 1u;
  if (*endC == ':' && signature[curIndex] == ':') {
    ++curIndex;
  }
  return kString;
}

// read until ; or <, whatever end of classname is ;
std::string AnnotationParser::GetNextClassName(bool forSubClass) {
  size_t pos1 = signature.find_first_of(';', curIndex);
  size_t pos2 = signature.find_first_of('<', curIndex);
  CHECK_FATAL(pos1 != pos2, "must be");
  size_t pos = pos1 < pos2 ? pos1 : pos2;
  curStrToken = signature.substr(curIndex - 1, pos - curIndex + 1);
  curStrToken += ";";
  curIndex = pos;
  if (forSubClass) {
    for (size_t i = 0; i < curStrToken.size(); ++i) {
      if (curStrToken[i] == '.') {
        curStrToken[i] = '$';
      }
    }
  }
  std::string className = namemangler::EncodeName(curStrToken);
  return className;
}

void AnnotationParser::InitFuncGenericDeclare(MemPool &pragmaMemPool, MIRFunction &mirFunc) {
  size_t declarePos = signature.find_first_of(':', curIndex);
  size_t prev = 1;
  size_t next = 0;
  while (declarePos != std::string::npos) {
    std::string declareName = signature.substr(prev, declarePos - prev);
    if (declareName != "") {
      GStrIdx gStrIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(declareName);
      GenericDeclare *tmp = GetOrCreateDeclare(gStrIdx, pragmaMemPool);
      tmp->SetBelongToFunc(&mirFunc);
      mirFunc.AddFuncGenericDeclare(tmp);
    }
    if (signature[declarePos + 1] == ':') {
      ++declarePos;
    }
    next = declarePos + 1;
    declarePos = signature.find_first_of(':', next);
    prev = signature.find_last_of (';', declarePos) + 1;
  }
}

void AnnotationParser::InitClassGenericDeclare(MemPool &pragmaMemPool, MIRStructType &mirStruct) {
  size_t declarePos = signature.find_first_of(':', curIndex);
  size_t prev = 1;
  size_t next = 0;
  while (declarePos != std::string::npos) {
    std::string declareName = signature.substr(prev, declarePos - prev);
    if (declareName != "") {
      GStrIdx gStrIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(declareName);
      GenericDeclare *tmp = GetOrCreateDeclare(gStrIdx, pragmaMemPool);
      tmp->SetBelongToStruct(&mirStruct);
      mirStruct.AddClassGenericDeclare(tmp);
    }
    if (signature[declarePos + 1] == ':') {
      ++declarePos;
    }
    next = declarePos + 1;
    declarePos = signature.find_first_of(':', next);
    prev = signature.find_last_of (';', declarePos) + 1;
  }
}

void AnnotationAnalysis::ByPassFollowingInfo(AnnotationParser &aParser, MIRStructType *sType) {
  ATokenKind t = aParser.GetNextToken();
  if (t == kTemplateStart) {
    t = aParser.GetNextToken();
    while (t != kTemplateEnd) {
      aParser.BackOne();
      (void)ReadInGenericType(aParser, sType);
      t = aParser.GetNextToken();
    }
    CHECK_FATAL(t == kTemplateEnd, "must be");
    t = aParser.GetNextToken();
  }
  while (t == kSubClass) {
    (void)aParser.GetNextClassName(true);
    t = aParser.GetNextToken();
    if (t == kTemplateStart) {
      t = aParser.GetNextToken();
      while (t != kTemplateEnd) {
        aParser.BackOne();
        (void)ReadInGenericType(aParser, sType);
        t = aParser.GetNextToken();
      }
      CHECK_FATAL(t == kTemplateEnd, "must be");
      t = aParser.GetNextToken();
    }
  }
  CHECK_FATAL(t == kSemiComma, "must be");
}

AnnotationType *AnnotationParser::GetOrCreateArrayType(AnnotationType *containsType, MemPool &pragmaMemPool) {
  if (createdArrayType.find(containsType) != createdArrayType.end()) {
    return createdArrayType[containsType];
  }
  ExtendGeneric *retType = pragmaMemPool.New<ExtendGeneric>(containsType, kArrayType);
  createdArrayType[containsType] = retType;
  return retType;
}

AnnotationType *AnnotationAnalysis::ReadInGenericType(AnnotationParser &aParser, MIRStructType *sType) {
  ATokenKind tk = aParser.GetNextToken();
  AnnotationType *retType = nullptr;
  switch (tk) {
    case kAClassName: {
      std::string className = aParser.GetNextClassName();
      GStrIdx gStrIdx = GlobalTables::GetStrTable().GetStrIdxFromName(className);
      MIRStructType *mirStruct = GetClassTypeFromName(className);
      if (mirStruct == nullptr) {
        ByPassFollowingInfo(aParser, sType);
        retType = dummyObj;
        break;
      }
      std::vector<GenericDeclare*>& gdVector = mirStruct->GetGenericDeclare();
      GenericType *gt = pragmaMemPool->New<GenericType>(gStrIdx, mirStruct, pragmaAllocator);
      ATokenKind t = aParser.GetNextToken();
      if (t == kTemplateStart) {
        AAForClassInfo(*mirStruct);
        for (size_t i = 0; i < gdVector.size(); ++i) {
          AnnotationType *tmp = ReadInGenericType(aParser, sType);
          gt->AddGenericPair(gdVector[i], tmp);
        }
        t = aParser.GetNextToken();
        while (t != kTemplateEnd) {
          aParser.BackOne();
          (void)ReadInGenericType(aParser, sType);
          t = aParser.GetNextToken();
        }
        CHECK_FATAL(t == kTemplateEnd, "must be");
        t = aParser.GetNextToken();
      }
      while (t == kSubClass) {
        std::string subClassName = aParser.GetNextClassName(true);
        gt->ReWriteType(subClassName);
        t = aParser.GetNextToken();
        if (t == kTemplateStart) {
          AAForClassInfo(*gt->GetMIRStructType());
          std::vector<GenericDeclare*>& gdTmpVector = gt->GetMIRStructType()->GetGenericDeclare();
          for (size_t i = 0; i < gdTmpVector.size(); ++i) {
            AnnotationType *tmp = ReadInGenericType(aParser, sType);
            gt->AddGenericPair(gdTmpVector[i], tmp);
          }
          t = aParser.GetNextToken();
          while (t != kTemplateEnd) {
            aParser.BackOne();
            (void)ReadInGenericType(aParser, sType);
            t = aParser.GetNextToken();
          }
          CHECK_FATAL(t == kTemplateEnd, "must be");
          t = aParser.GetNextToken();
        }
      }
      CHECK_FATAL(t == kSemiComma, "must be");
      retType = gt;
      break;
    }
    case kTemplateName: {
      (void)aParser.GetNextToken(&annoSemiColon);
      GStrIdx s = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(aParser.GetCurStrToken());
      retType = aParser.GetOrCreateDeclare(s, *pragmaMemPool, true, sType);
      if (retType == nullptr) {
        retType = dummyObj;
      }
      break;
    }
    case kExtend:
      retType = ReadInGenericType(aParser, sType);
      retType = pragmaMemPool->New<ExtendGeneric>(retType, kHierarchyExtend);
      break;
    case kSuper:
      retType = ReadInGenericType(aParser, sType);
      retType = pragmaMemPool->New<ExtendGeneric>(retType, kHierarchyHSuper);
      break;
    case kArray:
      retType = ReadInGenericType(aParser, sType);
      retType = aParser.GetOrCreateArrayType(retType, *pragmaMemPool);
      break;
    case kMatch:
      return genericMatch;
    case kPrimeTypeName: {
      GStrIdx primeStrIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(aParser.GetCurStrToken());
      retType = pragmaMemPool->New<AnnotationType>(kPrimeType, primeStrIdx);
      break;
    }
    default:
      CHECK_FATAL(false, "must be");
  }
  return retType;
}

GenericDeclare *AnnotationAnalysis::ReadInGenericDeclare(AnnotationParser &aParser, MIRStructType *mirStructType) {
  (void)aParser.GetNextToken(&annoDeclare);
  GenericDeclare *gDeclare = nullptr;
  if (aParser.GetCurStrToken() != "") {
    GStrIdx gStrIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(aParser.GetCurStrToken());
    gDeclare = aParser.GetOrCreateDeclare(gStrIdx, *pragmaMemPool, true, mirStructType);
    CHECK_FATAL(gDeclare->GetDefaultType() == nullptr, "must be");
  }
  AnnotationType *at = ReadInGenericType(aParser, mirStructType);
  if (gDeclare != nullptr) {
    gDeclare->SetDefaultType(at);
  }
  return gDeclare;
}

std::string AnnotationAnalysis::ReadInAllSubString(const MIRPragma &classPragma) {
  GStrIdx gStrIdx;
  std::string signature;
  CHECK_FATAL(classPragma.GetElementVector().size() == 1, "must be");
  for (MIRPragmaElement *pragmaElement : classPragma.GetElementVector()) {
    CHECK_FATAL(pragmaElement->GetType() == kValueArray, "must be");
    for (MIRPragmaElement *subPragmaEle : pragmaElement->GetSubElemVec()) {
      CHECK_FATAL(subPragmaEle->GetType() == kValueString, "must be");
      gStrIdx.reset(static_cast<uint32>(subPragmaEle->GetU64Val()));
      const std::string &str = GlobalTables::GetStrTable().GetStringFromStrIdx(gStrIdx);
      signature += str;
    }
  }
  return signature;
}

void FunctionGenericDump(MIRFunction &func) {
  if (func.GetFuncGenericDeclare().size()) {
    std::cout << '<';
    for (GenericDeclare *gd : func.GetFuncGenericDeclare()) {
      gd->Dump();
    }
    std::cout << '>';
  }
  std::cout << '(';
  for (AnnotationType *at : func.GetFuncGenericArg()) {
    if (at->GetKind() == kGenericDeclare) {
      std::cout << "T" << at->GetName() << ";";
    } else {
      at->Dump();
    }
  }
  std::cout << ')';
  AnnotationType *ret = func.GetFuncGenericRet();
  if (ret->GetKind() == kGenericDeclare) {
    std::cout << "T" << ret->GetName() << ";";
  } else {
    ret->Dump();
  }
  std::cout << std::endl;
}

void AnnotationAnalysis::AnalysisAnnotationForFuncLocalVar(MIRFunction &func, AnnotationParser &aParser,
                                                           MIRStructType &structType) {
  for (auto pair : func.GetAliasVarMap()) {
    MIRAliasVars aliasVar = pair.second;
    if (aliasVar.sigStrIdx) {
      std::string newSig = GlobalTables::GetStrTable().GetStringFromStrIdx(aliasVar.sigStrIdx);
      aParser.ReplaceSignature(newSig);
      AnnotationType *var = ReadInGenericType(aParser, &structType);
      func.AddFuncLocalGenericVar(aliasVar.memPoolStrIdx, var);
    }
  }
}

void AnnotationAnalysis::AnalysisAnnotationForFunc(const MIRPragma &funcPragma, MIRStructType &structType) {
  MIRSymbol *funcSymbol = GlobalTables::GetGsymTable().GetSymbolFromStrIdx(funcPragma.GetStrIdx());
  MIRFunction *func = funcSymbol->GetFunction();
  (void)analysisedFunc.insert(func);
  CHECK_FATAL(func->GetFuncGenericDeclare().size() == 0, "must be");
  CHECK_FATAL(func->GetFuncGenericArg().size() == 0, "must be");
  std::string signature = ReadInAllSubString(funcPragma);
  AnnotationParser aParser(signature);
  ATokenKind tk = aParser.GetNextToken();
  if (tk == kTemplateStart) {
    aParser.InitFuncGenericDeclare(*pragmaMemPool, *func);
    while (aParser.GetNextToken() != kTemplateEnd) {
      aParser.BackOne();
      (void)ReadInGenericDeclare(aParser, &structType);
    }
    tk = aParser.GetNextToken();
  }
  CHECK_FATAL(tk == kArgumentStart, "must be");
  tk = aParser.GetNextToken();
  while (tk != kArgumentEnd) {
    aParser.BackOne();
    AnnotationType *arg = ReadInGenericType(aParser, &structType);
    func->AddFuncGenericArg(arg);
    tk = aParser.GetNextToken();
  }
  AnnotationType *ret = ReadInGenericType(aParser, &structType);
  func->AddFuncGenericRet(ret);
  AnalysisAnnotationForFuncLocalVar(*func, aParser, structType);
}

void AnnotationAnalysis::AnalysisAnnotationForVar(const MIRPragma &varPragma, MIRStructType &structType) {
  std::string signature = ReadInAllSubString(varPragma);
  AnnotationParser aParser(signature);
  ATokenKind tk = aParser.GetNextToken();
  CHECK_FATAL(tk == kAClassName || tk == kTemplateName || tk == kArray, "must be");
  aParser.BackOne();
  AnnotationType *at = ReadInGenericType(aParser, &structType);
  structType.AddFieldGenericDeclare(varPragma.GetStrIdx(), at);
}

void AnnotationAnalysis::AnalysisAnnotationForClass(const MIRPragma &classPragma) {
  TyIdx classTyIdx = GlobalTables::GetTypeNameTable().GetTyIdxFromGStrIdx(classPragma.GetStrIdx());
  MIRType &classType = GetTypeFromTyIdx(classTyIdx);
  CHECK_FATAL(classType.GetKind() == kTypeClass || classType.GetKind() == kTypeInterface, "must be");
  std::string signature = ReadInAllSubString(classPragma);
  AnnotationParser aParser(signature);
  ATokenKind tk = aParser.GetNextToken();
  if (tk != kTemplateStart) {
    return;
  }
  aParser.InitClassGenericDeclare(*pragmaMemPool, static_cast<MIRStructType&>(classType));
  while (aParser.GetNextToken() != kTemplateEnd) {
    aParser.BackOne();
    (void)ReadInGenericDeclare(aParser);
  }
  while (aParser.GetNextToken() != kEnd) {
    aParser.BackOne();
    AnnotationType *at = ReadInGenericType(aParser, nullptr);
    CHECK_FATAL(at->GetKind() == kGenericType, "must be");
    if (static_cast<GenericType*>(at)->GetMIRStructType() != &classType) {
      static_cast<MIRStructType&>(classType).AddInheritaceGeneric(static_cast<GenericType*>(at));
    }
  }
  return;
}

void AnnotationAnalysis::AAForClassInfo(MIRStructType &structType) {
  if (analysised.find(&structType) != analysised.end() || structType.IsIncomplete()) {
    return;
  }
  (void)analysised.insert(&structType);
  std::vector<MIRPragma*> &pragmaVec = structType.GetPragmaVec();
  for (MIRPragma *pragma : pragmaVec) {
    MIRType *pragmaType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(pragma->GetTyIdx());
    if (pragmaType->GetName() != "Lark_2Fannotation_2FSignature_3B") {
      continue;
    }
    if (pragma->GetKind() != kPragmaClass) {
      continue;
    }
    AnalysisAnnotationForClass(*pragma);
  }
}

void AnnotationAnalysis::AAForFuncVarInfo(MIRStructType &structType) {
  std::vector<MIRPragma*> &pragmaVec = structType.GetPragmaVec();
  for (MIRPragma *pragma : pragmaVec) {
    MIRType *pragmaType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(pragma->GetTyIdx());
    if (pragmaType->GetName() != "Lark_2Fannotation_2FSignature_3B") {
      continue;
    }
    if (pragma->GetKind() == kPragmaVar) {
      AnalysisAnnotationForVar(*pragma, structType);
    } else if (pragma->GetKind() == kPragmaFunc) {
      AnalysisAnnotationForFunc(*pragma, structType);
    }
  }
}

void AnnotationAnalysis::AnalysisAnnotation() {
  const MapleVector<Klass*> &klasses = klassH->GetTopoSortedKlasses();
  for (Klass *klass : klasses) {
    if (klass->IsInterfaceIncomplete() || klass->IsClassIncomplete()) {
      continue;
    }
    MIRStructType *structType = klass->GetMIRStructType();
    AAForClassInfo(*structType);
  }
  for (Klass *klass : klasses) {
    if (klass->IsInterfaceIncomplete() || klass->IsClassIncomplete()) {
      continue;
    }
    MIRStructType *structType = klass->GetMIRStructType();
    AAForFuncVarInfo(*structType);
  }
  for (MIRFunction *func : mirModule->GetFunctionList()) {
    if (func->IsEmpty() || analysisedFunc.find(func) != analysisedFunc.end()) {
      continue;
    }
    MIRType *classType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(func->GetClassTyIdx());
    if (classType == nullptr) {
      continue;
    }
    std::string tmp;
    AnnotationParser aParser(tmp);
    AnalysisAnnotationForFuncLocalVar(*func, aParser, static_cast<MIRStructType&>(*classType));
  }
}

void AnnotationAnalysis::Run() {
  AnalysisAnnotation();
}

AnalysisResult *DoAnnotationAnalysis::Run(MIRModule *module, ModuleResultMgr *moduleResultMgr) {
  MemPool *memPool = memPoolCtrler.NewMemPool("AnnotationAnalysis mempool");
  MemPool *pragmaMemPool = memPoolCtrler.NewMemPool("New Pragma mempool");
  auto *kh = static_cast<KlassHierarchy*>(moduleResultMgr->GetAnalysisResult(MoPhase_CHA, module));
  ASSERT_NOT_NULL(kh);
  AnnotationAnalysis *aa = pragmaMemPool->New<AnnotationAnalysis>(module, memPool, pragmaMemPool, kh);
  aa->Run();
  memPoolCtrler.DeleteMemPool(memPool);
  return aa;
}
}
