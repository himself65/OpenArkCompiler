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
#ifndef MPL2MPL_INCLUDE_ANNOTATION_ANALYSIS_H
#define MPL2MPL_INCLUDE_ANNOTATION_ANALYSIS_H
#include "class_hierarchy.h"
#include "namemangler.h"

namespace maple {
enum ATokenKind {
  kEnd,
  kChar,
  kTemplateStart,
  kTemplateEnd,
  kAClassName,
  kPrimeTypeName,
  kTemplateName,
  kExtend,
  kSuper,
  kArgumentStart,
  kArgumentEnd,
  kMatch,
  kArray,
  kString,
  kSubClass,
  kSemiComma
};

enum AKind {
  kGenericType,
  kGenericDeclare,
  kGenericMatch,
  kPrimeType,
  kExtendType
};

enum EInfo {
  kHierarchyExtend,
  kHierarchyHSuper,
  kArrayType
};

class AnnotationType {
 public:
  static int atime;
  AnnotationType(AKind ak, GStrIdx strIdx) : id(atime++), kind(ak), typeName(strIdx) {}
  ~AnnotationType() = default;
  virtual const std::string GetName() const {
    return GlobalTables::GetStrTable().GetStringFromStrIdx(typeName);
  }

  GStrIdx GetGStrIdx() {
    return typeName;
  }

  AKind GetKind() const {
    return kind;
  }

  int GetId() {
    return id;
  }

  virtual void Dump();
  virtual void ReWriteType(std::string &) {
    CHECK_FATAL(false, "impossible");
  }

 protected:
  int id;
  AKind kind;
  GStrIdx typeName;
};

class GenericDeclare;
class GenericType : public AnnotationType {
 public:
  GenericType(GStrIdx strIdx, MIRType *ms, MapleAllocator &alloc)
      :AnnotationType(kGenericType, strIdx),
       mirStructType(ms),
       GenericArg(alloc.Adapter()),
       ArgOrder(alloc.Adapter()) {}
  ~GenericType() = default;
  void AddGenericPair(GenericDeclare *k, AnnotationType *v) {
    GenericArg[k] = v;
    ArgOrder.push_back(v);
  }

  MIRStructType *GetMIRStructType() {
    if (mirStructType->IsInstanceOfMIRStructType()) {
      return static_cast<MIRStructType*>(mirStructType);
    }
    CHECK_FATAL(false, "must be");
    return nullptr;
  }

  MIRType *GetMIRType() {
    return mirStructType;
  }

  virtual const std::string GetName() const override {
    return mirStructType->GetMplTypeName();
  }

  MapleMap<GenericDeclare*, AnnotationType*> &GetGenericMap() {
    return GenericArg;
  }

  MapleVector<AnnotationType*> &GetGenericArg() {
    return ArgOrder;
  }

  virtual void Dump() override;
  virtual void ReWriteType(std::string &subClass) override;
 private:
  MIRType *mirStructType;
  MapleMap<GenericDeclare*, AnnotationType*> GenericArg;
  MapleVector<AnnotationType*> ArgOrder;
};

class GenericDeclare : public AnnotationType {
 public:
  explicit GenericDeclare(GStrIdx strIdx)
      :AnnotationType(kGenericDeclare, strIdx),
       defaultType(nullptr) {}
  ~GenericDeclare() = default;
  AnnotationType *GetDefaultType() {
    return defaultType;
  }

  void SetDefaultType(AnnotationType *gt) {
    defaultType = gt;
  }

  std::string GetBelongToName() {
    if (defKind == defByStruct) {
      return belongsTo.structType->GetName();
    } else {
      return belongsTo.func->GetName();
    }
  }

  virtual void Dump() override;

  void SetBelongToStruct(MIRStructType *s) {
    defKind = defByStruct;
    belongsTo.structType = s;
  }

  void SetBelongToFunc(MIRFunction *f) {
    defKind = defByFunc;
    belongsTo.func = f;
  }

 private:
  AnnotationType *defaultType;
  union DefPoint {
    MIRStructType *structType;
    MIRFunction *func;
  };
  DefPoint belongsTo;
  enum DefKind {
    defByNone,
    defByStruct,
    defByFunc
  };
  DefKind defKind;
};

class ExtendGeneric: public AnnotationType {
 public:
  ExtendGeneric(AnnotationType *c, EInfo h) : AnnotationType(kExtendType, GStrIdx(0)), contains(c), eInfo(h) {
    CHECK_FATAL(c->GetKind() != kGenericMatch, "must be");
  }

  virtual void Dump() override {
    std::cout << (eInfo == kHierarchyExtend ? '+' : (eInfo == kArrayType ? '[' : '-'));
    if (contains->GetKind() == kGenericDeclare) {
      std::cout << "T" << contains->GetName() << ";";
    } else {
      contains->Dump();
    }
  }

  EInfo GetExtendInfo() {
    return eInfo;
  }

  AnnotationType *GetContainsGeneric() {
    return contains;
  }
 private:
  AnnotationType *contains;
  EInfo eInfo;
};

class AnnotationParser {
 public:
  explicit AnnotationParser(std::string &s) : signature(s), curIndex(0) {}
  ~AnnotationParser() = default;
  ATokenKind GetNextToken(char *endC = nullptr);
  GenericDeclare *GetOrCreateDeclare(GStrIdx gStrIdx, MemPool &mp, bool check = false, MIRStructType *sType = nullptr);
  AnnotationType *GetOrCreateArrayType(AnnotationType *containsType, MemPool &pragmaMemPool);
  void BackOne() {
    --curIndex;
  }

  std::string GetCurIndexStr() {
    return signature.substr(0, curIndex < signature.size() ? curIndex : signature.size());
  }

  void InitClassGenericDeclare(MemPool &pragmaMemPool, MIRStructType &mirStruct);
  void InitFuncGenericDeclare(MemPool &pragmaMemPool, MIRFunction &mirFunc);
  std::string GetNextClassName(bool forSubClass = false);
  std::string GetCurStrToken() {
    return curStrToken;
  }

  void ReplaceSignature(std::string &s) {
    signature = s;
    curIndex = 0;
  }

 private:
  std::string &signature;
  int curIndex;
  std::map<GStrIdx, GenericDeclare*> created;
  std::map<AnnotationType*, ExtendGeneric*> createdArrayType;
  std::string curStrToken;
};

class AnnotationAnalysis : public AnalysisResult {
 public:
  static char annoDeclare;
  static char annoSemiColon;
  AnnotationAnalysis(MIRModule *mod, MemPool *tmpMp, MemPool *pragmaMp, KlassHierarchy *kh)
      : AnalysisResult(pragmaMp),
        mirModule(mod),
        tmpAllocator(tmpMp),
        pragmaMemPool(pragmaMp),
        pragmaAllocator(pragmaMp),
        klassH(kh) {
          genericMatch = pragmaMp->New<AnnotationType>(kGenericMatch, GStrIdx(0));
          GStrIdx strIdx = GlobalTables::GetStrTable().GetStrIdxFromName("Ljava_2Flang_2FObject_3B");
          MIRType &classType = GetTypeFromTyIdx(GlobalTables::GetTypeNameTable().GetTyIdxFromGStrIdx(strIdx));
          dummyObj = pragmaMp->New<GenericType>(strIdx, &static_cast<MIRStructType&>(classType), pragmaAllocator);
        };
  ~AnnotationAnalysis() = default;
  void Run();
 private:
  void AnalysisAnnotation();
  void AnalysisAnnotationForClass(MIRPragma &classPragma);
  void AnalysisAnnotationForVar(const MIRPragma &varPragma, MIRStructType &structType);
  void AnalysisAnnotationForFunc(MIRPragma &funcPragma, MIRStructType &structType);
  AnnotationType *ReadInGenericType(AnnotationParser &aParser, MIRStructType *sType);
  GenericDeclare *ReadInGenericDeclare(AnnotationParser &aParser, MIRStructType *mirStructType = nullptr);
  std::string ReadInAllSubString(const MIRPragma &classPragma);
  void AAForClassInfo(MIRStructType &structType);
  void AAForFuncVarInfo(MIRStructType &structType);
  void AnalysisAnnotationForFuncLocalVar(MIRFunction &func, AnnotationParser &aParser, MIRStructType &structType);
  void ByPassFollowingInfo(AnnotationParser &aParser, MIRStructType *sType);
  MIRModule *mirModule;
  MapleAllocator tmpAllocator;
  MemPool *pragmaMemPool;
  MapleAllocator pragmaAllocator;
  KlassHierarchy *klassH;
  MapleSet<MIRStructType*> analysised{tmpAllocator.Adapter()};
  MapleSet<MIRFunction*> analysisedFunc{tmpAllocator.Adapter()};
  AnnotationType *genericMatch;
  GenericType *dummyObj;
};

class DoAnnotationAnalysis : public ModulePhase {
 public:
  explicit DoAnnotationAnalysis(ModulePhaseID id) : ModulePhase(id) {}

  ~DoAnnotationAnalysis() = default;

  AnalysisResult *Run(MIRModule *module, ModuleResultMgr *moduleResultMgr) override;
  std::string PhaseName() const override {
    return "annotationanalysis";
  }
};
}
#endif
