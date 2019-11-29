/*
 * Copyright (c) [2019] Huawei Technologies Co.,Ltd.All rights reserved.
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
#ifndef MPL2MPL_INCLUDE_REFLECTION_ANALYSIS_H
#define MPL2MPL_INCLUDE_REFLECTION_ANALYSIS_H
#include "class_hierarchy.h"

namespace maple {
// maple field index definition
enum class ClassRO : uint32 {
  kClassName,
  kIfields,
  kMethods,
  kSuperclass,
  kNumOfFields,
  kNumofMethods,
#ifndef USE_32BIT_REF
  kFlag,
  kNumOfSup,
  kPadding,
#endif  //! USE_32BIT_REF
  kMod,
  kAnnotation,
  kClinitAddr
};

enum class ClassProperty : uint32 {
  kShadow,
  kMonitor,
  kClassloader,
  kObjsize,
#ifdef USE_32BIT_REF
  FLAG,
  NUMOFSUP,
#endif  // USE_32BIT_REF
  kItab,
  kVtab,
  kGctib,
  kInfoRo,
  kClint
};

enum class MethodProperty : uint32 {
  kVtabIndex,
  kDeclarclass,
  kPaddrData,
  kMod,
  kMethodName,
  kSigName,
  kAnnotation,
  kFlag,
  kArgsize,
#ifndef USE_32BIT_REF
  padding
#endif
};

enum class MethodInfoCompact : uint32 {
  kVtabIndex,
  kPaddrData
};

enum class FieldProperty : uint32 {
  kPOffset,
  kMod,
  kFlag,
  kIndex,
  kTypeName,
  kName,
  kAnnotation,
  kDeclarclass
};

enum class FieldPropertyCompact : uint32 {
  kPOffset,
  kMod,
  kTypeName,
  kIndex,
  kName,
  kAnnotation
};

class ReflectionAnalysis : public AnalysisResult {
 public:
  ReflectionAnalysis(MIRModule *mod, MemPool *memPool, KlassHierarchy *kh, MIRBuilder &builder)
      : AnalysisResult(memPool),
        mirModule(mod),
        allocator(memPool),
        klassH(kh),
        mirBuilder(builder),
        classTab(allocator.Adapter()),
        isLibcore(false),
        isLazyBindingOrDecouple(false) {
    Klass *objectKlass = kh->GetKlassFromLiteral(NameMangler::kJavaLangObjectStr);
    if (objectKlass != nullptr && objectKlass->GetMIRStructType()->IsLocal()) {
      isLibcore = true;
    }
  }
  ~ReflectionAnalysis() = default;

  static void GenStrTab(MIRModule &mirmodule);
  static uint32 FindOrInsertRepeatString(const std::string &str, bool isHot = false, uint8 hotType = kLayoutUnused);
  static BaseNode *GenClassInfoAddr(BaseNode *obj, MIRBuilder &builder);
  void Run();
  static void ConvertMethodSig(std::string &signature);
  static TyIdx GetClassMetaDataTyIdx() {
    return classMetadataTyIdx;
  }

 private:
  static std::unordered_map<std::string, uint32> &GetStr2IdxMap() {
    return str2IdxMap;
  }

  static void SetStr2IdxMap(const std::string &str, uint32 index) {
    str2IdxMap[str] = index;
  }

  static const std::string &GetStrTab() {
    return strTab;
  }

  static const std::string &GetStrTabStartHot() {
    return strTabStartHot;
  }

  static const std::string &GetStrTabBothHot() {
    return strTabBothHot;
  }

  static const std::string &GetStrTabRunHot() {
    return strTabRunHot;
  }

  static void AddStrTab(const std::string &str) {
    strTab += str;
  }

  static void AddStrTabStartHot(const std::string &str) {
    strTabStartHot += str;
  }

  static void AddStrTabBothHot(const std::string &str) {
    strTabBothHot += str;
  }

  static void AddStrTabRunHot(const std::string &str) {
    strTabRunHot += str;
  }

  static uint32 FirstFindOrInsertRepeatString(const std::string &str, bool isHot, uint8 hotType);
  MIRSymbol *GetOrCreateSymbol(const std::string &name, TyIdx tyIdx, bool needInit);
  MIRSymbol *GetSymbol(const std::string &name, TyIdx tyIdx);
  MIRSymbol *CreateSymbol(GStrIdx strIdx, TyIdx tyIdx);
  MIRSymbol *GetSymbol(GStrIdx strIdx, TyIdx tyIdx);
  void GenClassMetaData(Klass &klass);
  std::string GetAnnoValueNoArray(const MIRPragmaElement &annoElem);
  std::string GetArrayValue(const MapleVector<MIRPragmaElement*> &subElemVector);
  std::string GetAnnotationValue(const MapleVector<MIRPragmaElement*> &subElemVector, GStrIdx typeStrIdx);
  MIRSymbol *GenSuperClassMetaData(const Klass &klass, std::list<Klass*> superClassList);
  MIRSymbol *GenFieldOffsetData(const Klass &klass, std::pair<FieldPair, int> &fieldInfo);
  MIRSymbol *GenFieldsMetaData(const Klass &klass);
  MIRSymbol *GenMethodsMetaData(const Klass &klass);
  MIRSymbol *GenFieldsMeta(const Klass &klass, std::vector<std::pair<FieldPair, int>> &fieldsVector,
                           std::vector<std::pair<FieldPair, uint16>> &fieldHashvec);
  void GenFieldMeta(const Klass &klass, MIRStructType &fieldsInfoType, std::pair<FieldPair, int> &fieldInfo,
                    MIRAggConst &aggConst, int idx, std::vector<std::pair<FieldPair, uint16>> &fieldHashVec);
  MIRSymbol *GenMethodsMeta(const Klass &klass, std::vector<std::pair<MethodPair*, int>> &methodInfoVec,
                            std::unordered_map<uint32, std::string> &baseNameMp,
                            std::unordered_map<uint32, std::string> &fullNameMp);
  void GenMethodMeta(const Klass &klass, MIRStructType &methodsInfoType,
                     MIRSymbol &funcSym, MIRAggConst &aggConst, int idx,
                     std::unordered_map<uint32, std::string> &baseNameMp,
                     std::unordered_map<uint32, std::string> &fullNameMp);
  uint32 GetMethodModifier(const Klass &klass, const MIRFunction &func) const;
  uint32 GetMethodFlag(const MIRFunction &func) const;
  void GenFieldOffsetConst(MIRAggConst &newConst, const Klass &klass, const MIRStructType &type,
                           std::pair<FieldPair, int> &fieldInfo, uint32 metaFieldID);
  MIRSymbol *GenMethodAddrData(const Klass &klass, const MIRSymbol &funcSym);
  static void GenMetadataType(MIRModule &mirModule);
  static MIRType *GetRefFieldType();
  static TyIdx GenMetaStructType(MIRModule &mirModule, MIRStructType &metaType, const std::string &str);
  int64 GetHashIndex(const std::string &strName);
  static void GenHotClassNameString(const Klass &klass);
  uint32 FindOrInsertReflectString(const std::string &str);
  static void InitReflectString();
  int64 BKDRHash(const std::string &strName, uint32 seed);
  void GenClassHashMetaData();
  void MarkWeakMethods();

  bool VtableFunc(const MIRFunction &func) const;
  void GenPrimitiveClass();
  void GenAllMethodHash(std::vector<std::pair<MethodPair*, int>> &methodInfoVec,
                        std::unordered_map<uint32, std::string> &baseNameMap,
                        std::unordered_map<uint32, std::string> &fullNameMap);
  void GenAllFieldHash(std::vector<std::pair<FieldPair, uint16>> &fieldV) const;
  void GenAnnotation(std::map<int, int> &idxNumMap, std::string &annoArr, MIRClassType &classType,
                      PragmaKind paragKind, const std::string &paragName, TyIdx fieldTypeIdx,
                      std::map<int, int> *paramNumArray = nullptr, int *paramIndex = nullptr);
  void AppendValueByType(std::string &annoArr, const MIRPragmaElement &elem);
  void SetAnnoFieldConst(const MIRStructType &metadataRoType, MIRAggConst &newConst, uint32 fieldID,
                         std::map<int, int> &idxNumMap, const std::string &annoArr);
  bool IsAnonymousClass(const std::string &annotationString);
  bool IsPrivateClass(const MIRClassType &classType) const;
  bool IsStaticClass(const MIRClassType &classType) const;
  int8 JudgePara(MIRClassType &ctype);
  void CheckPrivateInnerAndNoSubClass(Klass &clazz, const std::string &annoArr);
  void ConvertMapleClassName(const std::string &mplClassName, std::string &javaDsp);

  int GetDeflateStringIdx(const std::string &subStr);
  uint32 GetAnnoCstrIndex(std::map<int, int> &idxNumMap, const std::string &annoArr);
  uint32 GetMethodInVtabIndex(const Klass &clazz, const MIRFunction &func) const;
  void GetSignatureTypeNames(std::string &signature, std::vector<std::string> &typeNames);
  MIRSymbol *GetClinitFuncSymbol(const Klass &klass);
  int SolveAnnotation(MIRClassType &classType, MIRFunction &func);
  uint32 GetTypeNameIdxFromType(const MIRType &type, const Klass &klass, const std::string &fieldName);

  MIRModule *mirModule;
  MapleAllocator allocator;
  KlassHierarchy *klassH;
  MIRBuilder &mirBuilder;
  MapleVector<MIRSymbol*> classTab;
  bool isLibcore;
  bool isLazyBindingOrDecouple;
  std::string reflectionMuidStr;
  static const char *klassPtrName;
  static TyIdx classMetadataTyIdx;
  static TyIdx classMetadataRoTyIdx;
  static TyIdx methodsInfoTyIdx;
  static TyIdx methodsInfoCompactTyIdx;
  static TyIdx fieldsInfoTyIdx;
  static TyIdx fieldsInfoCompactTyIdx;
  static TyIdx superclassMetadataTyIdx;
  static TyIdx fieldOffsetDataTyIdx;
  static TyIdx methodAddrDataTyIdx;
  static std::string strTab;
  static std::unordered_map<std::string, uint32> str2IdxMap;
  static std::string strTabStartHot;
  static std::string strTabBothHot;
  static std::string strTabRunHot;
  static bool strTabInited;
  static TyIdx invalidIdx;
  static constexpr uint16 kNoHashBits = 6u;
  static constexpr char annoDelimiterPrefix = '`';
  static constexpr char annoDelimiter = '!';
  static constexpr char annoArrayStartDelimiter = '{';
  static constexpr char annoArrayEndDelimiter = '}';
};

class DoReflectionAnalysis : public ModulePhase {
 public:
  explicit DoReflectionAnalysis(ModulePhaseID id) : ModulePhase(id) {}

  ~DoReflectionAnalysis() = default;

  AnalysisResult *Run(MIRModule *module, ModuleResultMgr *moduleResultMgr) override;
  std::string PhaseName() const override {
    return "reflectionanalysis";
  }
};
}  // namespace maple
#endif  // MPL2MPL_INCLUDE_REFLECTION_ANALYSIS_H
