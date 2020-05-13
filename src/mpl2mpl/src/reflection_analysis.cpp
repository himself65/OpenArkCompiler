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
#include "reflection_analysis.h"
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <iomanip>
#include <fstream>
#include "vtable_analysis.h"
#include "vtable_impl.h"
#include "option.h"
#include "muid_replacement.h"
#include "mir_builder.h"
#include "namemangler.h"
#include "itab_util.h"
#include "string_utils.h"
#include "metadata_layout.h"

namespace {
using namespace maple;
// If needed, we can make field type in two bits.
constexpr uint64 kMethodNotVirtual = 0x00000001;
constexpr uint64 kMethodFinalize = 0x00000002;
constexpr uint64 kMethodAbstract = 0x00000010;
constexpr uint64 kFieldOffsetIspOffset = 0x00000001;

constexpr int kModPublic = 1;                 // 0x00000001
constexpr int kModPrivate = 2;                // 0x00000002
constexpr int kModProtected = 3;              // 0x00000004
constexpr int kModStatic = 4;                 // 0x00000008
constexpr int kModFinal = 5;                  // 0x00000010
constexpr int kModSynchronized = 6;           // 0x00000020
constexpr int kModVolatile = 7;               // 0x00000040
constexpr int kModTransient = 8;              // 0x00000080
constexpr int kModNative = 9;                 // 0x00000100
constexpr int kModAbstract = 11;              // 0x00000400
constexpr int kModStrict = 12;                // 0x00000800
constexpr int kModSynthetic = 13;             // 0x00001000
constexpr int kModConstructor = 17;           // 0x00010000
constexpr int kModDefault = 22;               // 0x00400000
constexpr int kModBridge = 7;                 // 0x00000040
constexpr int kModVarargs = 8;                // 0x00000080
constexpr int kModEnum = 15;                  // 0x00004000
constexpr int kModDeclaredSynchronized = 18;  // 0x00020000
constexpr int kModifierRCUnowned = 24;        // 0x00800000
constexpr int kModifierRCWeak = 25;           // 0x01000000
constexpr int kModifierHiddenApiGrey = 26;    // 0x02000000
constexpr int kModifierHiddenApiBlack = 27;   // 0x04000000

// +1 is needed here because our field id starts with 0 pointing to the struct itself
constexpr uint32 kObjKlassFieldID = static_cast<uint32>(ClassProperty::kShadow) + 1;
constexpr uint32 kMetaDataKlassFieldID = static_cast<uint32>(ClassProperty::kShadow) + 1;
constexpr bool kRADebug = false;
constexpr uint32 kMethodFieldHashSize = 1022;
constexpr uint16 kHashConflictFlag = 1023;
constexpr int16 kMethodNotFound = -10;

constexpr char kModStr[] = "mod";
constexpr char kAddrStr[] = "addr";
constexpr char kFlagStr[] = "flag";
constexpr char kItabStr[] = "itab";
constexpr char kVtabStr[] = "vtab";
constexpr char kGctibStr[] = "gctib";
constexpr char kIndexStr[] = "index";
constexpr char kOffsetStr[] = "offset";
constexpr char kShadowStr[] = "shadow";
constexpr char kClassSuffix[] = "_3B";
constexpr char kArgsizeStr[] = "argsize";
constexpr char kIfieldsStr[] = "ifields";
constexpr char kMethodsStr[] = "methods";
constexpr char kMonitorStr[] = "monitor";
constexpr char kObjsizeStr[] = "objsize";
#ifndef USE_32BIT_REF
constexpr char kPaddingStr[] = "padding";
#else
constexpr char kInstanceOfCacheFalseStr[] = "instanceOfCacheFalse";
#endif // ~USE_32BIT_REF
constexpr char kTypeNameStr[] = "typeName";
constexpr char kClassNameStr[] = "classname";
constexpr char kFieldNameStr[] = "fieldname";
constexpr char kAccessFlags[] = "accessFlags";
constexpr char kAnnotationStr[] = "annotation";
constexpr char kClinitAddrStr[] = "clinitAddr";
constexpr char kMethodNameStr[] = "methodname";
constexpr char kInitFuntionStr[] = "_3Cinit_3E";
constexpr char kClassinforoStr[] = "classinforo";
constexpr char kClassloaderStr[] = "classloader";
constexpr char kLebPadding0Str[] = "lebPadding0";
constexpr char kNumOfFieldsStr[] = "numoffields";
constexpr char kClinitbridgeStr[] = "clinitbridge";
constexpr char kNumOfMethodsStr[] = "numofmethods";
constexpr char kSignatureNameStr[] = "signaturename";
constexpr char kDeclaringclassStr[] = "declaringclass";
constexpr char kFieldInfoTypeName[] = "__field_info__";
constexpr char kINFOAccessFlags[] = "INFO_access_flags";
constexpr char kSuperclassinfoStr[] = "superclassinfo";
constexpr char kFieldOffsetDataStr[] = "fieldOffsetData";
constexpr char kMethodAddrDataStr[] = "methodAddrData";
constexpr char kAnnotationvalueStr[] = "annotationvalue";
constexpr char kMethodInfoTypeName[] = "__method_info__";
constexpr char kClinitSuffixStr[] = "_3Cclinit_3E_7C_28_29V";
constexpr char kJavaLangEnumStr[] = "Ljava_2Flang_2FEnum_3B";
constexpr char kNumOfSuperclassesStr[] = "numofsuperclasses";
constexpr char kClassMetadataRoTypeName[] = "__class_meta_ro__";
constexpr char kMethodInVtabIndexStr[] = "method_in_vtab_index";
constexpr char kSuperclassMetadataTypeName[] = "__superclass_meta__";
constexpr char kFieldOffsetDataTypeName[] = "__fieldOffsetDataType__";
constexpr char kMethodAddrDataTypeName[] = "__methodAddrDataType__";
constexpr char kFieldInfoCompactTypeName[] = "__field_info_compact__";
constexpr char kMethodInfoCompactTypeName[] = "__method_info_compact__";
constexpr char kSuperclassOrComponentclassStr[] = "superclass_or_componentclass";
constexpr char kReflectionReferencePrefixStr[] = "Ljava_2Flang_2Fref_2FReference_3B";
constexpr char kJavaLangAnnotationRetentionStr[] = "Ljava_2Flang_2Fannotation_2FRetention_3B";
constexpr int kAnonymousClassIndex = 5;
constexpr char kAnonymousClassSuffix[] = "30";
constexpr char kInnerClassStr[] = "Lark/annotation/InnerClass;";
constexpr char kEnclosingClassStr[] = "Lark/annotation/EnclosingClass;";
constexpr char kArkAnnotationEnclosingClassStr[] = "Lark_2Fannotation_2FEnclosingClass_3B";
} // namespace

// Reflection metadata
// This file is used to generate the classmetadata and classhashmetadata.
//
// A. Classmetadata is consists of three parts:classinfo, fields, methods,
//    and we generates these data according to the structure which defined in
//    the reflection_analysis.h && metadata_layout.h and then add their address
//    to mirbuilder.
namespace maple {
std::string ReflectionAnalysis::strTab = std::string(1, '\0');
std::unordered_map<std::string, uint32> ReflectionAnalysis::str2IdxMap;
std::string ReflectionAnalysis::strTabStartHot = std::string(1, '\0');
std::string ReflectionAnalysis::strTabBothHot = std::string(1, '\0');
std::string ReflectionAnalysis::strTabRunHot = std::string(1, '\0');
bool ReflectionAnalysis::strTabInited = false;

bool ReflectionAnalysis::IsMemberClass(const std::string &annotationString) {
  uint32_t idx = ReflectionAnalysis::FindOrInsertReflectString(kEnclosingClassStr);
  std::string target = annoDelimiterPrefix + std::to_string(idx) + annoDelimiter;
  if (annotationString.find(target, 0) != std::string::npos) {
    return true;
  }
  return false;
}

int8_t ReflectionAnalysis::GetAnnoFlag(const std::string &annotationString) {
  constexpr int8_t kMemberPosValid = 1;
  constexpr int8_t kMemberPosValidOffset = 2;
  constexpr int8_t kIsMemberClassOffset = 1;
  constexpr int8_t kNewMeta = 1;
  bool isMemberClass = IsMemberClass(annotationString);
  int8_t value = (kMemberPosValid << kMemberPosValidOffset) +
                 (static_cast<uint8_t>(isMemberClass) << kIsMemberClassOffset) + kNewMeta;
  return value;
}

int ReflectionAnalysis::GetDeflateStringIdx(const std::string &subStr, bool needSpecialFlag) {
  std::string flag = needSpecialFlag ? (std::to_string(GetAnnoFlag(subStr)) + annoDelimiter) : "1!";
  return FindOrInsertReflectString(flag + subStr);
}

uint32 ReflectionAnalysis::FirstFindOrInsertRepeatString(const std::string &str, bool isHot, uint8 hotType) {
  auto it = ReflectionAnalysis::GetStr2IdxMap().find(str);
  if (it != ReflectionAnalysis::GetStr2IdxMap().end()) {
    return it->second;
  }

  uint32 index = 0;
  constexpr uint32 lengthShift = 2u;
  if (isHot) {
    if (hotType == kLayoutBootHot) {
      uint32 length = ReflectionAnalysis::GetStrTabStartHot().length();
      index = (length << lengthShift) | (kLayoutBootHot + kCStringShift);  // Use the LSB to indicate hotness.
      ReflectionAnalysis::AddStrTabStartHot(str + '\0');
    } else if (hotType == kLayoutBothHot) {
      uint32 length = ReflectionAnalysis::GetStrTabBothHot().length();
      index = (length << lengthShift) | (kLayoutBothHot + kCStringShift);  // Use the LSB to indicate hotness.
      ReflectionAnalysis::AddStrTabBothHot(str + '\0');
    } else {
      uint32 length = ReflectionAnalysis::GetStrTabRunHot().length();
      index = (length << lengthShift) | (kLayoutRunHot + kCStringShift);  // Use the LSB to indicate hotness.
      ReflectionAnalysis::AddStrTabRunHot(str + '\0');
    }
  } else {
    uint32 length = ReflectionAnalysis::GetStrTab().length();
    index = length << lengthShift;
    ReflectionAnalysis::AddStrTab(str + '\0');
  }
  ReflectionAnalysis::SetStr2IdxMap(str, index);

  return index;
}

void ReflectionAnalysis::InitReflectString() {
  std::string initHot[] = { "V", "Z", "B", "C", "S", "I", "J", "F", "D", "Ljava/lang/String;", "Ljava/lang/Object;" };
  for (auto const &innerType : initHot) {
    (void)FirstFindOrInsertRepeatString(innerType, true, kLayoutBothHot);
  }
}

uint32 ReflectionAnalysis::FindOrInsertRepeatString(const std::string &str, bool isHot, uint8 hotType) {
  if (strTabInited == false) {
    // Add default hot strings.
    strTabInited = true;
  }
  return FirstFindOrInsertRepeatString(str, isHot, hotType);
}

BaseNode *ReflectionAnalysis::GenClassInfoAddr(BaseNode *obj, MIRBuilder &builder) {
  GenMetadataType(builder.GetMirModule());
  auto *objectType = static_cast<MIRClassType*>(WKTypes::Util::GetJavaLangObjectType());
  BaseNode *classinfoAddress = nullptr;
  if (objectType != nullptr && objectType->GetKind() != kTypeClassIncomplete) {
    classinfoAddress = builder.CreateExprIread(*GlobalTables::GetTypeTable().GetRef(),
                                               *GlobalTables::GetTypeTable().GetOrCreatePointerType(*objectType),
                                               kObjKlassFieldID, obj);
  } else {
    // If java.lang.Object type is not defined, fall back to use the classinfo struct to retrieve the first field.
    auto *classMetadataType = static_cast<MIRStructType*>(
        GlobalTables::GetTypeTable().GetTypeFromTyIdx(ReflectionAnalysis::classMetadataTyIdx));
    classinfoAddress = builder.CreateExprIread(*GlobalTables::GetTypeTable().GetRef(),
                                               *GlobalTables::GetTypeTable().GetOrCreatePointerType(*classMetadataType),
                                               kMetaDataKlassFieldID, obj);
  }
  return classinfoAddress;
}

const char *ReflectionAnalysis::klassPtrName = NameMangler::kShadowClassName;
TyIdx ReflectionAnalysis::classMetadataTyIdx = TyIdx(0);
TyIdx ReflectionAnalysis::classMetadataRoTyIdx = TyIdx(0);
TyIdx ReflectionAnalysis::methodsInfoTyIdx = TyIdx(0);
TyIdx ReflectionAnalysis::methodsInfoCompactTyIdx = TyIdx(0);
TyIdx ReflectionAnalysis::fieldsInfoTyIdx = TyIdx(0);
TyIdx ReflectionAnalysis::fieldsInfoCompactTyIdx = TyIdx(0);
TyIdx ReflectionAnalysis::superclassMetadataTyIdx = TyIdx(0);
TyIdx ReflectionAnalysis::fieldOffsetDataTyIdx = TyIdx(0);
TyIdx ReflectionAnalysis::methodAddrDataTyIdx = TyIdx(0);
TyIdx ReflectionAnalysis::invalidIdx = TyIdx(-1);

uint32 ReflectionAnalysis::GetMethodModifier(const Klass &klass, const MIRFunction &func) const {
  const FuncAttrs &fa = func.GetFuncAttrs();
  uint32 mod =
      (static_cast<unsigned char>(fa.GetAttr(FUNCATTR_public)) << (kModPublic - 1)) |
      (static_cast<unsigned char>(fa.GetAttr(FUNCATTR_protected)) << (kModProtected - 1)) |
      (static_cast<unsigned char>(fa.GetAttr(FUNCATTR_private)) << (kModPrivate - 1)) |
      (static_cast<unsigned char>(fa.GetAttr(FUNCATTR_abstract)) << (kModAbstract - 1)) |
      (static_cast<unsigned char>(fa.GetAttr(FUNCATTR_static)) << (kModStatic - 1)) |
      (static_cast<unsigned char>(fa.GetAttr(FUNCATTR_final)) << (kModFinal - 1)) |
      (static_cast<unsigned char>(fa.GetAttr(FUNCATTR_declared_synchronized)) << (kModSynchronized - 1)) |
      (static_cast<unsigned char>(fa.GetAttr(FUNCATTR_declared_synchronized)) << (kModDeclaredSynchronized - 1)) |
      (static_cast<unsigned char>(fa.GetAttr(FUNCATTR_native)) << (kModNative - 1)) |
      (static_cast<unsigned char>(fa.GetAttr(FUNCATTR_strict)) << (kModStrict - 1)) |
      (static_cast<unsigned char>(fa.GetAttr(FUNCATTR_synthetic)) << (kModSynthetic - 1)) |
      (static_cast<unsigned char>(fa.GetAttr(FUNCATTR_bridge)) << (kModBridge - 1)) |
      (static_cast<unsigned char>(fa.GetAttr(FUNCATTR_constructor)) << (kModConstructor - 1)) |
      (static_cast<unsigned char>(fa.GetAttr(FUNCATTR_varargs)) << (kModVarargs - 1)) |
      (static_cast<unsigned char>(fa.GetAttr(FUNCATTR_hiddenapigrey)) << (kModifierHiddenApiGrey - 1)) |
      (static_cast<unsigned char>(fa.GetAttr(FUNCATTR_hiddenapiblack)) << (kModifierHiddenApiBlack - 1));
  // Add default attribute.
  if (klass.IsInterface() && !func.GetAttr(FUNCATTR_abstract) && !func.GetAttr(FUNCATTR_static)) {
    mod |= (1 << (kModDefault));
  }
  return mod;
}

uint32 GetFieldModifier(const FieldAttrs &fa) {
  return (static_cast<unsigned char>(fa.GetAttr(FLDATTR_public)) << (kModPublic - 1)) |
         (static_cast<unsigned char>(fa.GetAttr(FLDATTR_protected)) << (kModProtected - 1)) |
         (static_cast<unsigned char>(fa.GetAttr(FLDATTR_private)) << (kModPrivate - 1)) |
         (static_cast<unsigned char>(fa.GetAttr(FLDATTR_static)) << (kModStatic - 1)) |
         (static_cast<unsigned char>(fa.GetAttr(FLDATTR_final)) << (kModFinal - 1)) |
         (static_cast<unsigned char>(fa.GetAttr(FLDATTR_transient)) << (kModTransient - 1)) |
         (static_cast<unsigned char>(fa.GetAttr(FLDATTR_volatile)) << (kModVolatile - 1)) |
         (static_cast<unsigned char>(fa.GetAttr(FLDATTR_synthetic)) << (kModSynthetic - 1)) |
         (static_cast<unsigned char>(fa.GetAttr(FLDATTR_enum)) << (kModEnum - 1)) |
         (static_cast<unsigned char>(fa.GetAttr(FLDATTR_rcunowned)) << (kModifierRCUnowned - 1)) |
         (static_cast<unsigned char>(fa.GetAttr(FLDATTR_rcweak)) << (kModifierRCWeak - 1)) |
         (static_cast<unsigned char>(fa.GetAttr(FLDATTR_hiddenapigrey)) << (kModifierHiddenApiGrey - 1)) |
         (static_cast<unsigned char>(fa.GetAttr(FLDATTR_hiddenapiblack)) << (kModifierHiddenApiBlack - 1));
}

uint32 GetClassAccessFlags(const MIRStructType &classType) {
  int32 accessFlag = 0;
  for (const MIRPragma *prag : classType.GetPragmaVec()) {
    if (prag->GetKind() == kPragmaClass) {
      const MapleVector<MIRPragmaElement*> elemVector = prag->GetElementVector();
      for (MIRPragmaElement *elem : elemVector) {
        const std::string &name = GlobalTables::GetStrTable().GetStringFromStrIdx(elem->GetNameStrIdx());
        if (name == kAccessFlags) {
          accessFlag = elem->GetI32Val();
          return static_cast<uint32>(accessFlag);
        }
      }
    }
  }
  size_t size = classType.GetInfo().size();
  for (size_t i = 0; i < size; ++i) {
    if (GlobalTables::GetStrTable().GetStringFromStrIdx(classType.GetInfoElemt(i).first) == kINFOAccessFlags) {
      return classType.GetInfoElemt(i).second;
    }
  }
  return 0;
}

bool ReflectionAnalysis::IsStaticClass(const MIRStructType &classType) const {
  return GetClassAccessFlags(classType) & 0x00000008;  // #  Modifier_Static 0x00000008;
}

bool ReflectionAnalysis::IsPrivateClass(const MIRClassType &classType) const {
  return GetClassAccessFlags(classType) & 0x00000002;  // #  Modifier_Private 0x00000002;
}

static inline GStrIdx GetOrCreateGStrIdxFromName(const std::string &name) {
  return GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(NameMangler::GetInternalNameLiteral(name));
}

static bool IsFinalize(const std::string &funcName, const std::string &signature) {
  return funcName == "finalize" && signature == "()V";
}

static std::string GetSignatureFromFullName(const std::string &fullname) {
  size_t pos = fullname.find("|");
  if (pos != std::string::npos) {
    return fullname.substr(pos + 1);
  }
  FATAL(kLncFatal, "can not find \"|\" in fullname");
}

int ReflectionAnalysis::SolveAnnotation(MIRStructType &classType, MIRFunction &func) {
  std::string annoArray1;
  std::map<int, int> idxNumMap;
  GenAnnotation(idxNumMap, annoArray1, classType, kPragmaFunc, func.GetName(), invalidIdx);
  // Parameter annotation.
  std::string annoArray2;
  std::map<int, int> paramNumArray;
  int paramIndex = 0;
  std::map<int, int> paramIdxNumMap;
  GenAnnotation(paramIdxNumMap, annoArray2, classType, kPragmaParam, func.GetName(), invalidIdx, &paramNumArray,
                &paramIndex);
  std::string subStr = "";
  if (idxNumMap.empty() && paramIdxNumMap.empty()) {
    subStr += "0";
  } else {
    subStr += std::to_string(idxNumMap.size());
    subStr += "!";
    std::for_each(idxNumMap.begin(), idxNumMap.end(), [&subStr](const std::pair<const int, int> p) {
      subStr += std::to_string(p.second);
      subStr += "!";
    });
    subStr += annoArray1;
    // Parameter.
    subStr += '|';
    subStr += std::to_string(paramIdxNumMap.size());
    subStr += "!";
    std::for_each(paramIdxNumMap.begin(), paramIdxNumMap.end(), [&subStr](const std::pair<const int, int> p) {
      subStr += std::to_string(p.second);
      subStr += "!";
    });
    for (int z = 0; z < paramIndex; ++z) {
      subStr += std::to_string(paramNumArray[z]);
      subStr += "!";
    }
    subStr += annoArray2;
  }
  return GetDeflateStringIdx(subStr, false);
}

uint32 ReflectionAnalysis::GetTypeNameIdxFromType(const MIRType &type, const Klass &klass,
    const std::string &fieldName) {
  uint32 typeNameIdx = 0;
  switch (type.GetKind()) {
    case kTypeScalar: {
      std::string name(GetPrimTypeJavaName(type.GetPrimType()));
      typeNameIdx = FindOrInsertReflectString(name);
      break;
    }
    case kTypePointer: {
      auto *ptype = static_cast<const MIRPtrType&>(type).GetPointedType();
      if (ptype->GetKind() == kTypeArray || ptype->GetKind() == kTypeJArray) {
        CHECK_NULL_FATAL(ptype);
        const std::string &javaName = static_cast<MIRJarrayType*>(ptype)->GetJavaName();
        std::string klassJavaDescriptor;
        NameMangler::DecodeMapleNameToJavaDescriptor(javaName, klassJavaDescriptor);
        typeNameIdx = FindOrInsertReflectString(klassJavaDescriptor);
      } else if (ptype->GetKind() == kTypeByName || ptype->GetKind() == kTypeClass ||
                 ptype->GetKind() == kTypeInterface || ptype->GetKind() == kTypeClassIncomplete ||
                 ptype->GetKind() == kTypeInterfaceIncomplete || ptype->GetKind() == kTypeConstString) {
        std::string javaName = ptype->GetName();
        std::string klassJavaDescriptor;
        NameMangler::DecodeMapleNameToJavaDescriptor(javaName, klassJavaDescriptor);
        typeNameIdx = FindOrInsertReflectString(klassJavaDescriptor);
      } else {
        CHECK_FATAL(false, "In class %s: field %s 's type is UNKNOWN", klass.GetKlassName().c_str(),
                    fieldName.c_str());
      }
      break;
    }
    default: {
      CHECK_FATAL(false, "In class %s: field %s 's type is UNKNOWN", klass.GetKlassName().c_str(), fieldName.c_str());
    }
  }
  return typeNameIdx;
}

void ReflectionAnalysis::CheckPrivateInnerAndNoSubClass(Klass &clazz, const std::string &annoArr) {
  // LMain_24A_3B  `EC!`VL!24!LMain_3B!`IC!`AF!4!2!name!23!A!
  uint32_t idx = FindOrInsertReflectString(kEnclosingClassStr);
  std::string target = annoDelimiterPrefix + std::to_string(idx) + annoDelimiter;
  size_t pos = annoArr.find(kEnclosingClassStr, 0);
  if (pos == std::string::npos) {
    return;
  }
  ASSERT_NOT_NULL(clazz.GetMIRClassType());
  if (!IsPrivateClass(*clazz.GetMIRClassType())) {
    return;  // Check private.
  }
  if (clazz.HasSubKlass()) {
    clazz.SetPrivateInnerAndNoSubClass(false);
  } else {
    clazz.SetPrivateInnerAndNoSubClass(true);
  }
}

uint16 GetCharHashIndex(const std::string &name) {
  unsigned int hashCode = DJBHash(name.c_str());
  return static_cast<uint16>(hashCode % kMethodFieldHashSize);
}

uint16 GenMethodHashIndex(const std::string &name, const std::string &signature) {
  std::string fullName = name + signature;
  size_t p = fullName.find(')');
  CHECK_FATAL(p != std::string::npos, "can not find )");
  std::string subName = fullName.substr(0, p + 1);
  uint16 h = GetCharHashIndex(subName);
  return h;
}

void ReflectionAnalysis::ConvertMethodSig(std::string &signature) {
  size_t signatureSize = signature.size();
  for (size_t i = 1; i < signatureSize; ++i) {
    if (signature[i] == 'L') {
      while (signature[++i] != ';') {} // eg. Ljava/io/InputStream; so we do not check the boundary.
    } else if (signature[i] == 'A') {
      signature[i] = '[';
    }
  }
}

void ReflectionAnalysis::GenAllMethodHash(std::vector<std::pair<MethodPair*, int>> &methodInfoVec,
                                          std::unordered_map<uint32, std::string> &baseNameMap,
                                          std::unordered_map<uint32, std::string> &fullNameMap) {
  std::vector<MIRFunction*> methodVector;
  std::vector<uint16> hashVector;
  for (auto &methodInfo : methodInfoVec) {
    MIRSymbol *funcSym = GlobalTables::GetGsymTable().GetSymbolFromStidx(methodInfo.first->first.Idx());
    MIRFunction *func = funcSym->GetFunction();
    std::string baseName = func->GetBaseFuncName();
    baseName = NameMangler::DecodeName(baseName);
    baseNameMap[func->GetBaseFuncNameStrIdx()] = baseName;
    std::string fullName = func->GetBaseFuncNameWithType();
    fullName = NameMangler::DecodeName(fullName);
    fullNameMap[func->GetBaseFuncNameWithTypeStrIdx()] = fullName;
    CHECK_FATAL(fullName.find("|") != std::string::npos, "can not find |");
    std::string signature = fullName.substr(fullName.find("|") + 1);
    ConvertMethodSig(signature);
    uint16 h = GenMethodHashIndex(baseName, signature);
    if (IsFinalize(baseName, signature)) {
      h = kHashConflictFlag;
    }
    func->SetHashCode(h);
    hashVector.push_back(h);
  }
}

void ReflectionAnalysis::GenAllFieldHash(std::vector<std::pair<FieldPair, uint16>> &fieldV) const {
  std::vector<MIRFunction*> methodVector;
  std::vector<uint16> hashVector;
  for (auto &field : fieldV) {
    uint16 h = field.second;
    hashVector.push_back(h);
  }
}

uint16 GetFieldHash(const std::vector<std::pair<FieldPair, uint16>> &fieldV, const FieldPair &fieldSources) {
  for (const auto &field : fieldV) {
    const FieldPair &f = field.first;
    if (f == fieldSources) {
      return field.second;
    }
  }
  return 0;
}

MIRSymbol *ReflectionAnalysis::GetOrCreateSymbol(const std::string &name, TyIdx tyIdx, bool needInit = false) {
  const GStrIdx strIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(name);
  MIRSymbol *st = GetSymbol(strIdx, tyIdx);
  if (st != nullptr && !needInit) {
    // Previous symbol is a forward declaration, create a new symbol for definiton.
    return st;
  }
  st = CreateSymbol(strIdx, tyIdx);
  // Set classinfo symbol as extern if not defined locally.
  if (StringUtils::StartsWith(name, CLASSINFO_PREFIX_STR)) {
    std::string className = name.substr(strlen(CLASSINFO_PREFIX_STR));
    Klass *klass = klassH->GetKlassFromName(className);
    if (klass != nullptr && !klass->GetMIRStructType()->IsLocal()) {
      st->SetStorageClass(kScExtern);
    }
  }
  return st;
}

MIRSymbol *ReflectionAnalysis::GetSymbol(const std::string &name, TyIdx tyIdx) {
  const GStrIdx strIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(name);
  MIRSymbol *st = GetSymbol(strIdx, tyIdx);
  return st;
}

static bool IsSameType(TyIdx tyIdx1, TyIdx tyIdx2) {
  if (tyIdx1 == tyIdx2) {
    return true;
  }
  MIRType *type1 = GlobalTables::GetTypeTable().GetTypeFromTyIdx(tyIdx1);
  MIRType *type2 = GlobalTables::GetTypeTable().GetTypeFromTyIdx(tyIdx2);
  return type1->EqualTo(*type2);
}

MIRSymbol *ReflectionAnalysis::GetSymbol(GStrIdx strIdx, TyIdx tyIdx) {
  MIRSymbol *st = GlobalTables::GetGsymTable().GetSymbolFromStrIdx(strIdx);
  if (st != nullptr && st->GetSKind() == kStVar && IsSameType(st->GetTyIdx(), tyIdx)) {
    return st;
  }
  return nullptr;
}

MIRSymbol *ReflectionAnalysis::CreateSymbol(GStrIdx strIdx, TyIdx tyIdx) {
  MIRSymbol *st = GlobalTables::GetGsymTable().CreateSymbol(kScopeGlobal);
  st->SetStorageClass(kScGlobal);
  st->SetSKind(kStVar);
  st->SetNameStrIdx(strIdx);
  GlobalTables::GetGsymTable().AddToStringSymbolMap(*st);
  st->SetAttr(ATTR_public);
  st->SetTyIdx(tyIdx);
  return st;
}

bool ReflectionAnalysis::VtableFunc(const MIRFunction &func) const {
  return (func.GetAttr(FUNCATTR_virtual) && !func.GetAttr(FUNCATTR_private) && !func.GetAttr(FUNCATTR_static));
}

bool RtRetentionPolicyCheck(const MIRSymbol &clInfo) {
  GStrIdx strIdx;
  auto *annoType =
      static_cast<MIRClassType*>(GlobalTables::GetTypeTable().GetTypeFromTyIdx(clInfo.GetTyIdx()));
  for (MIRPragma *p : annoType->GetPragmaVec()) {
    if (GlobalTables::GetStrTable().GetStringFromStrIdx(
            GlobalTables::GetTypeTable().GetTypeFromTyIdx(p->GetTyIdx())->GetNameStrIdx()) ==
        (kJavaLangAnnotationRetentionStr)) {
      strIdx.reset(p->GetNthElement(0)->GetU64Val());
      std::string retentionType = GlobalTables::GetStrTable().GetStringFromStrIdx(strIdx);
      if (retentionType != "RUNTIME") {
        return false;
      }
      return true;
    }
  }
  return false;
}

uint16 ReflectionAnalysis::GetMethodInVtabIndex(const Klass &klass, const MIRFunction &func) const {
  uint16 methodInVtabIndex = 0;
  bool findMethod = false;
  const MIRStructType *classType = klass.GetMIRStructType();
  const MIRSymbol *vtableSymbol = GlobalTables::GetGsymTable().GetSymbolFromStrIdx(
      GlobalTables::GetStrTable().GetStrIdxFromName(VTAB_PREFIX_STR + klass.GetKlassName()));
  if (klass.IsClass() && vtableSymbol != nullptr) {
    const auto *vtableConst = static_cast<const MIRAggConst*>(vtableSymbol->GetKonst());
    for (const MIRConstPtr &node : vtableConst->GetConstVec()) {
      if (node->GetKind() == kConstAddrofFunc) {
        const auto *addr = static_cast<const MIRAddroffuncConst*>(node);
        MIRFunction *vtableFunc = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(addr->GetValue());
        if (func.GetNameStrIdx() == vtableFunc->GetNameStrIdx()) {
          findMethod = true;
          break;
        }
      }
      ++methodInVtabIndex;
    }
  } else if (klass.IsInterface()) {
    methodInVtabIndex = 0;
    for (const MethodPair &methodPair : classType->GetMethods()) {
      const MIRSymbol *currSym = GlobalTables::GetGsymTable().GetSymbolFromStidx(methodPair.first.Idx());
      const MIRFunction *currFunc = currSym->GetFunction();
      if (func.GetNameStrIdx() == currFunc->GetNameStrIdx()) {
        findMethod = true;
        break;
      }
      ++methodInVtabIndex;
    }
  }
  if (!findMethod) {
    methodInVtabIndex = static_cast<uint16>(kMethodNotFound);
  }
  return methodInVtabIndex;
}

void ReflectionAnalysis::GetSignatureTypeNames(std::string &signature, std::vector<std::string> &typeNames) {
  ConvertMethodSig(signature);
  size_t sigLen = signature.length();
  size_t i = 0;
  const char *methodSignature = signature.c_str();
  ++i;
  while (i < sigLen && methodSignature[i] != ')') {
    std::string descriptor;
    if (methodSignature[i] != 'L' && methodSignature[i] != '[') {
      descriptor += methodSignature[i];
    } else {
      if (methodSignature[i] == '[') {
        while (methodSignature[i] == '[') {
          descriptor += methodSignature[i++];
        }
      }
      if (methodSignature[i] != 'L') {
        descriptor += methodSignature[i];
      } else {
        while (methodSignature[i] != ';') {
          descriptor += methodSignature[i++];
        }
        descriptor += ';';
      }
    }
    typeNames.push_back(descriptor);
    ++i;
  }
  // Return type.
  ++i;
  typeNames.push_back(methodSignature + i);
}

struct HashCodeComparator {
  const std::unordered_map<uint32, std::string> &basenameMp;
  const std::unordered_map<uint32, std::string> &fullnameMp;
  HashCodeComparator(const std::unordered_map<uint32, std::string> &arg1,
                     const std::unordered_map<uint32, std::string> &arg2)
      : basenameMp(arg1), fullnameMp(arg2) {}

  bool operator()(std::pair<MethodPair*, int> a, std::pair<MethodPair*, int> b) {
    const MIRSymbol *funcSymA = GlobalTables::GetGsymTable().GetSymbolFromStidx(a.first->first.Idx());
    const MIRFunction *funcA = funcSymA->GetFunction();
    auto itB = basenameMp.find(funcA->GetBaseFuncNameStrIdx());
    ASSERT(itB != basenameMp.end(), "check funcAname!");
    const std::string &funcAName = itB->second;
    auto itF = fullnameMp.find(funcA->GetBaseFuncNameWithTypeStrIdx());
    ASSERT(itF != fullnameMp.end(), "check funcAname!");
    const std::string &fullNameA = itF->second;
    CHECK_FATAL(fullNameA.find("|") != fullNameA.npos, "not found |");
    const std::string &signatureA = fullNameA.substr(fullNameA.find("|") + 1);
    const MIRSymbol *funcSymB = GlobalTables::GetGsymTable().GetSymbolFromStidx(b.first->first.Idx());
    const MIRFunction *funcB = funcSymB->GetFunction();
    itB = basenameMp.find(funcB->GetBaseFuncNameStrIdx());
    ASSERT(itB != basenameMp.end(), "check funcBname!");
    const std::string &funcBName = itB->second;
    itF = fullnameMp.find(funcB->GetBaseFuncNameWithTypeStrIdx());
    ASSERT(itF != fullnameMp.end(), "check funcBname!");
    const std::string &fullNameB = itF->second;
    CHECK_FATAL(fullNameB.find("|") != std::string::npos, "not found |");
    const std::string &signatureB = fullNameB.substr(fullNameB.find("|") + 1);
    // Make bridge the end.
    if (funcA->GetHashCode() == funcB->GetHashCode() && funcA->GetBaseFuncName() == funcB->GetBaseFuncName())  {
      // Only deal with return type is different.
      if ((funcA->GetAttr(FUNCATTR_bridge)) && !(funcB->GetAttr(FUNCATTR_bridge))) {
        return true;
      }
      if (!(funcA->GetAttr(FUNCATTR_bridge)) && (funcB->GetAttr(FUNCATTR_bridge))) {
        return false;
      }
    }
    // As finalize()V is frequency, check it first, we put it at the end method_s.
    if (IsFinalize(funcAName, signatureA) && !IsFinalize(funcBName, signatureB)) {
      return false;
    }
    if (!IsFinalize(funcAName, signatureA) && IsFinalize(funcBName, signatureB)) {
      return true;
    }
    return funcA->GetHashCode() < funcB->GetHashCode();
  }
};

uint32 ReflectionAnalysis::GetMethodFlag(const MIRFunction &func) const {
  uint32 flag = 0;
  if (!VtableFunc(func)) {
    flag |= kMethodNotVirtual;
  }
  GStrIdx finalizeMethod = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName("finalize_7C_28_29V");
  if (func.GetBaseFuncNameWithTypeStrIdx() == finalizeMethod) {
    flag |= kMethodFinalize;
  }
  if (func.GetAttr(FUNCATTR_abstract)) {
    flag |= kMethodAbstract;
  }
  uint16 hash = func.GetHashCode();
  flag |= (hash << kNoHashBits);  // hash 10 bit
  return flag;
}

void ReflectionAnalysis::GenMethodMeta(const Klass &klass, MIRStructType &methodsInfoType,
                                       MIRSymbol &funcSym, MIRAggConst &aggConst, int idx,
                                       std::unordered_map<uint32, std::string> &baseNameMp,
                                       std::unordered_map<uint32, std::string> &fullNameMp) {
  MIRFunction &func = *funcSym.GetFunction();
  MIRAggConst &newConst = *mirModule->GetMemPool()->New<MIRAggConst>(*mirModule, methodsInfoType);
  uint32 fieldID = 1;
  // @method_in_vtable_index
  uint32 methodInVtabIndex = GetMethodInVtabIndex(klass, func);
  mirBuilder.AddIntFieldConst(methodsInfoType, newConst, fieldID++, methodInVtabIndex);
  // @declaringclass
  MIRSymbol *dklassSt = GetOrCreateSymbol(CLASSINFO_PREFIX_STR + func.GetBaseClassName(), classMetadataTyIdx);
  mirBuilder.AddAddrofFieldConst(methodsInfoType, newConst, fieldID++, *dklassSt);
  // @addr : Function address or point function addr if lazyBinding or decouple
  if (isLazyBindingOrDecouple) {
    MIRSymbol *methodAddrSt = GenMethodAddrData(funcSym);
    if (methodAddrSt != nullptr) {
      mirBuilder.AddAddrofFieldConst(methodsInfoType, newConst, fieldID++, *methodAddrSt);
    } else {
      mirBuilder.AddIntFieldConst(methodsInfoType, newConst, fieldID++, 0);
    }
  } else {
    mirBuilder.AddAddroffuncFieldConst(methodsInfoType, newConst, fieldID++, funcSym);
  }

  // @modifier
  uint32 mod = GetMethodModifier(klass, func);
  mirBuilder.AddIntFieldConst(methodsInfoType, newConst, fieldID++, mod);
  // @methodname
  std::string baseName = baseNameMp[func.GetBaseFuncNameStrIdx()];
  uint32 methodnameIdx = FindOrInsertReflectString(baseName);
  mirBuilder.AddIntFieldConst(methodsInfoType, newConst, fieldID++, methodnameIdx);
  // @methodsignature
  std::string fullname = fullNameMp[func.GetBaseFuncNameWithTypeStrIdx()];
  std::string signature = GetSignatureFromFullName(fullname);
  ConvertMethodSig(signature);
  std::vector<std::string> typeNames;
  GetSignatureTypeNames(signature, typeNames);
  uint32 signatureIdx = FindOrInsertReflectString(signature);
  mirBuilder.AddIntFieldConst(methodsInfoType, newConst, fieldID++, signatureIdx);
  // @annotation
  MIRStructType *classType = klass.GetMIRStructType();
  int annotationIdx = SolveAnnotation(*classType, func);
  mirBuilder.AddIntFieldConst(methodsInfoType, newConst, fieldID++, annotationIdx);
  // @flag
  uint32 flag = GetMethodFlag(func);
  mirBuilder.AddIntFieldConst(methodsInfoType, newConst, fieldID++, flag);
  // @argsize: Number of arguments.
  size_t argsSize = func.GetParamSize();
  mirBuilder.AddIntFieldConst(methodsInfoType, newConst, fieldID++, argsSize);
#ifndef USE_32BIT_REF
  // @padding
  mirBuilder.AddIntFieldConst(methodsInfoType, newConst, fieldID, 0);
#endif
  aggConst.PushBack(&newConst);
}

MIRSymbol *ReflectionAnalysis::GenMethodsMeta(const Klass &klass,
                                              std::vector<std::pair<MethodPair*, int>> &methodInfoVec,
                                              std::unordered_map<uint32, std::string> &baseNameMp,
                                              std::unordered_map<uint32, std::string> &fullNameMp) {
  MIRStructType *classType = klass.GetMIRStructType();
  size_t arraySize = classType->GetMethods().size();
  auto &methodsInfoType =
      static_cast<MIRStructType&>(*GlobalTables::GetTypeTable().GetTypeFromTyIdx(methodsInfoTyIdx));
  MIRArrayType &arrayType = *GlobalTables::GetTypeTable().GetOrCreateArrayType(methodsInfoType, arraySize);
  MIRAggConst *aggConst = mirModule->GetMemPool()->New<MIRAggConst>(*mirModule, arrayType);
  ASSERT(aggConst != nullptr, "null ptr check!");
  int idx = 0;
  for (auto &methodInfo : methodInfoVec) {
    MIRSymbol *funcSym = GlobalTables::GetGsymTable().GetSymbolFromStidx(methodInfo.first->first.Idx());
    reflectionMuidStr += funcSym->GetName();
    GenMethodMeta(klass, methodsInfoType, *funcSym, *aggConst, idx++, baseNameMp, fullNameMp);
  }
  MIRSymbol *methodsArraySt =
      GetOrCreateSymbol(NameMangler::kMethodsInfoPrefixStr + klass.GetKlassName(), arrayType.GetTypeIndex(), true);
  methodsArraySt->SetStorageClass(kScFstatic);
  methodsArraySt->SetKonst(aggConst);
  return methodsArraySt;
}

MIRSymbol *ReflectionAnalysis::GenMethodAddrData(const MIRSymbol &funcSym) {
  MIRModule &module = *mirModule;
  MIRStructType &methodAddrType =
      static_cast<MIRStructType&>(*GlobalTables::GetTypeTable().GetTypeFromTyIdx(methodAddrDataTyIdx));
  MIRArrayType &methodAddrArrayType = *GlobalTables::GetTypeTable().GetOrCreateArrayType(methodAddrType, 1);
  MIRAggConst *aggconst = module.GetMemPool()->New<MIRAggConst>(module, methodAddrArrayType);
  MIRAggConst *newconst = module.GetMemPool()->New<MIRAggConst>(module, methodAddrType);
  MIRFunction *func = funcSym.GetFunction();
  MIRSymbol *methodAddrSt = nullptr;
  // skip abstract func.
  if (!func->IsAbstract()) {
    mirBuilder.AddAddroffuncFieldConst(methodAddrType, *newconst, 1, funcSym);
    aggconst->GetConstVec().push_back(newconst);
    methodAddrSt = GetOrCreateSymbol(NameMangler::kMethodAddrDataPrefixStr + func->GetName(),
                                     methodAddrArrayType.GetTypeIndex(), true);
    methodAddrSt->SetStorageClass(kScFstatic);
    methodAddrSt->SetKonst(aggconst);
  }
  return methodAddrSt;
}

MIRSymbol *ReflectionAnalysis::GenMethodsMetaData(const Klass &klass) {
  MIRStructType *classType = klass.GetMIRStructType();
  if (classType == nullptr || classType->GetMethods().empty()) {
    return nullptr;
  }

  std::vector<std::pair<MethodPair*, int>> methodinfoVec;
  for (MethodPair &methodPair : classType->GetMethods()) {
    methodinfoVec.push_back(std::make_pair(&methodPair, -1));
  }

  std::unordered_map<uint32, std::string> baseNameMp, fullNameMp;
  GenAllMethodHash(methodinfoVec, baseNameMp, fullNameMp);
  // Sort constVec by hashcode.
  HashCodeComparator comparator(baseNameMp, fullNameMp);
  std::sort(methodinfoVec.begin(), methodinfoVec.end(), comparator);
  MIRSymbol *methodsArraySt = GenMethodsMeta(klass, methodinfoVec, baseNameMp, fullNameMp);
  return methodsArraySt;
}
void ReflectionAnalysis::GenFieldOffsetConst(MIRAggConst &newConst, const Klass &klass,
                                             const MIRStructType &type, std::pair<FieldPair, int> &fieldInfo,
                                             uint32 metaFieldID) {
  bool isStaticField = (fieldInfo.second == -1);
  FieldPair fieldP = fieldInfo.first;
  TyIdx fieldTyidx = fieldP.second.first;
  std::string originFieldname = GlobalTables::GetStrTable().GetStringFromStrIdx(fieldP.first);
  if (isStaticField) {
    // Offset of the static field, we fill the global var address.
    const GStrIdx stridx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(
        GlobalTables::GetStrTable().GetStringFromStrIdx(fieldP.first));
    MIRSymbol *gvarSt = GetSymbol(stridx, fieldTyidx);
    if (gvarSt == nullptr) {
      // If this static field is not used, the symbol will not be generated,
      // so we just generate a weak one here.
      gvarSt = CreateSymbol(stridx, fieldTyidx);
      gvarSt->SetAttr(ATTR_weak);
      gvarSt->SetAttr(ATTR_static);
    }
    mirBuilder.AddAddrofFieldConst(type, newConst, metaFieldID, *gvarSt);
  } else {
    // Offset of the instance field, we fill the index of fields here and let CG to fill in.
    MIRStructType *mirClassType = klass.GetMIRStructType();
    ASSERT(mirClassType != nullptr, "GetMIRClassType() returns null");
    FieldID fldID = mirBuilder.GetStructFieldIDFromNameAndTypeParentFirstFoundInChild(
        *mirClassType, originFieldname, fieldP.second.first);
    // set LSB 0, and set LSB 1 in muid_replacement
    CHECK_FATAL(fldID <= INT32_MAX, "filedId out of range");
    fldID = fldID * 2;
    mirBuilder.AddIntFieldConst(type, newConst, metaFieldID, fldID);
  }
}

MIRSymbol *ReflectionAnalysis::GenFieldOffsetData(const Klass &klass, std::pair<FieldPair, int> &fieldInfo) {
  MIRModule &module = *mirModule;
  auto &fieldOffsetType =
      static_cast<MIRStructType&>(*GlobalTables::GetTypeTable().GetTypeFromTyIdx(fieldOffsetDataTyIdx));
  MIRArrayType &fieldOffsetArrayType = *GlobalTables::GetTypeTable().GetOrCreateArrayType(fieldOffsetType, 1);
  MIRAggConst *aggConst = module.GetMemPool()->New<MIRAggConst>(module, fieldOffsetArrayType);

  MIRAggConst *newConst = module.GetMemPool()->New<MIRAggConst>(module, fieldOffsetType);
  constexpr uint32_t fieldId = 1;
  GenFieldOffsetConst(*newConst, klass, fieldOffsetType, fieldInfo, fieldId);
  aggConst->GetConstVec().push_back(newConst);

  FieldPair fieldP = fieldInfo.first;
  std::string originFieldname = GlobalTables::GetStrTable().GetStringFromStrIdx(fieldP.first);
  std::string fieldOffsetSymbolName = NameMangler::kFieldOffsetDataPrefixStr;
  bool isStaticField = (fieldInfo.second == -1);
  if (isStaticField) {
    fieldOffsetSymbolName += originFieldname;
  } else {
    MIRStructType *mirClassType = klass.GetMIRStructType();
    FieldID fldID = mirBuilder.GetStructFieldIDFromNameAndTypeParentFirstFoundInChild(
        *mirClassType, originFieldname, fieldP.second.first);
    fieldOffsetSymbolName += klass.GetKlassName() + "_FieldID_" + std::to_string(fldID);
  }
  MIRSymbol *fieldsOffsetSt = GetOrCreateSymbol(fieldOffsetSymbolName, fieldOffsetArrayType.GetTypeIndex(), true);
  fieldsOffsetSt->SetStorageClass(kScFstatic);
  fieldsOffsetSt->SetKonst(aggConst);
  return fieldsOffsetSt;
}

MIRSymbol *ReflectionAnalysis::GenSuperClassMetaData(const Klass &klass, std::list<Klass*> superClassList) {
  MIRModule &module = *mirModule;
  size_t size = superClassList.size();
  auto &superclassMetadataType =
      static_cast<MIRStructType&>(*GlobalTables::GetTypeTable().GetTypeFromTyIdx(superclassMetadataTyIdx));
  MIRArrayType &arrayType = *GlobalTables::GetTypeTable().GetOrCreateArrayType(superclassMetadataType, size);
  MIRAggConst *aggconst = module.GetMemPool()->New<MIRAggConst>(module, arrayType);
  for (auto it = superClassList.begin(); it != superClassList.end(); ++it) {
    MIRSymbol *dklassSt = GetOrCreateSymbol(CLASSINFO_PREFIX_STR + (*it)->GetKlassName(), classMetadataTyIdx);
    MIRAggConst *newConst = module.GetMemPool()->New<MIRAggConst>(module, superclassMetadataType);
    mirBuilder.AddAddrofFieldConst(superclassMetadataType, *newConst, 1, *dklassSt);
    aggconst->PushBack(newConst);
  }
  MIRSymbol *superclassArraySt =
      GetOrCreateSymbol(SUPERCLASSINFO_PREFIX_STR + klass.GetKlassName(), arrayType.GetTypeIndex(), true);
  // Direct access to superclassinfo is only possible within a .so.
  superclassArraySt->SetStorageClass(kScFstatic);
  superclassArraySt->SetKonst(aggconst);
  return superclassArraySt;
}

static void ConvertFieldName(std::string &fieldname, bool staticfield) {
  // Convert field name to java define name.
  if (staticfield) {
    // Remove class name prefix.
    std::string decodeFieldName;
    NameMangler::DecodeMapleNameToJavaDescriptor(fieldname, decodeFieldName);
    std::stringstream ss(decodeFieldName);
    std::string item;
    std::vector<std::string> res;
    while (std::getline(ss, item, '|')) {
      res.push_back(item);
    }
    CHECK_FATAL(res.size() > 1, "fieldname not found");
    fieldname = res[StaticFieldName::kFieldName];
  } else {
    fieldname = NameMangler::DecodeName(fieldname);
  }
}

void ReflectionAnalysis::GenFieldMeta(const Klass &klass, MIRStructType &fieldsInfoType,
                                      std::pair<FieldPair, int> &fieldInfo, MIRAggConst &aggConst,
                                      int idx, std::vector<std::pair<FieldPair, uint16>> &fieldHashVec) {
  FieldPair fieldP = fieldInfo.first;
  MIRAggConst *newConst = mirModule->GetMemPool()->New<MIRAggConst>(*mirModule, fieldsInfoType);
  ASSERT(newConst != nullptr, "null ptr check!");
  uint32 fieldID = 1;
  uint16 hash = GetFieldHash(fieldHashVec, fieldP);
  uint16 flag = (hash << kNoHashBits);  // Hash 10 bit.

  // @offset or pOffset
  if (isLazyBindingOrDecouple) {
    flag |= kFieldOffsetIspOffset;
    MIRSymbol *fieldsOffsetSt = GenFieldOffsetData(klass, fieldInfo);
    mirBuilder.AddAddrofFieldConst(fieldsInfoType, *newConst, fieldID++, *fieldsOffsetSt);
  } else {
    GenFieldOffsetConst(*newConst, klass, fieldsInfoType, fieldInfo, fieldID++);
  }

  // @modifier
  FieldAttrs fa = fieldP.second.second;
  uint32 modifier = GetFieldModifier(fa);
  mirBuilder.AddIntFieldConst(fieldsInfoType, *newConst, fieldID++, modifier);
  // @flag
  mirBuilder.AddIntFieldConst(fieldsInfoType, *newConst, fieldID++, flag);
  // @index
  mirBuilder.AddIntFieldConst(fieldsInfoType, *newConst, fieldID++, idx);
  // @type
  TyIdx fieldTyIdx = fieldP.second.first;
  std::string fieldName = GlobalTables::GetStrTable().GetStringFromStrIdx(fieldP.first);
  MIRType *ty = GlobalTables::GetTypeTable().GetTypeFromTyIdx(fieldTyIdx);
  uint32 typeNameIdx = GetTypeNameIdxFromType(*ty, klass, fieldName);
  mirBuilder.AddIntFieldConst(fieldsInfoType, *newConst, fieldID++, typeNameIdx);
  // @fieldName
  bool isStaticField = (fieldInfo.second == -1);
  ConvertFieldName(fieldName, isStaticField);
  uint32 fieldname32Idx = FindOrInsertReflectString(fieldName);
  mirBuilder.AddIntFieldConst(fieldsInfoType, *newConst, fieldID++, fieldname32Idx);
  // @annotation
  MIRStructType *classType = klass.GetMIRStructType();
  std::string annoArr;
  std::map<int, int> idxNumMap;
  GenAnnotation(idxNumMap, annoArr, *classType, kPragmaVar, fieldName, ty->GetTypeIndex());
  uint32 annotationIdx = GetAnnoCstrIndex(idxNumMap, annoArr, true);
  mirBuilder.AddIntFieldConst(fieldsInfoType, *newConst, fieldID++, annotationIdx);
  // @declaring class
  MIRSymbol *dklassSt = GetOrCreateSymbol(CLASSINFO_PREFIX_STR + klass.GetKlassName(), classMetadataTyIdx);
  mirBuilder.AddAddrofFieldConst(fieldsInfoType, *newConst, fieldID, *dklassSt);
  aggConst.GetConstVec().push_back(newConst);
}

MIRSymbol *ReflectionAnalysis::GenFieldsMeta(const Klass &klass, std::vector<std::pair<FieldPair, int>> &fieldsVector,
                                             std::vector<std::pair<FieldPair, uint16>> &fieldHashVec) {
  size_t size = fieldsVector.size();
  auto &fieldsInfoType =
      static_cast<MIRStructType&>(*GlobalTables::GetTypeTable().GetTypeFromTyIdx(fieldsInfoTyIdx));
  MIRArrayType *arraytype = GlobalTables::GetTypeTable().GetOrCreateArrayType(fieldsInfoType, size);
  MIRAggConst *aggConst = mirModule->GetMemPool()->New<MIRAggConst>(*mirModule, *arraytype);
  ASSERT(aggConst != nullptr, "null ptr check!");
  int idx = 0;
  for (auto &fieldInfo : fieldsVector) {
    FieldPair fieldP = fieldInfo.first;
    std::string fieldName = GlobalTables::GetStrTable().GetStringFromStrIdx(fieldP.first);
    TyIdx fieldTyIdx = fieldP.second.first;
    MIRType *ty = GlobalTables::GetTypeTable().GetTypeFromTyIdx(fieldTyIdx);
    ASSERT(ty != nullptr, "null ptr check!");
    // Collect the the information about the fieldName and fieldtyidx.
    reflectionMuidStr += fieldName;
    reflectionMuidStr += ty->GetName();
    GenFieldMeta(klass, fieldsInfoType, fieldInfo, *aggConst, idx++, fieldHashVec);
  }
  MIRSymbol *fieldsArraySt =
      GetOrCreateSymbol(NameMangler::kFieldsInfoPrefixStr + klass.GetKlassName(), arraytype->GetTypeIndex(), true);
  fieldsArraySt->SetStorageClass(kScFstatic);
  fieldsArraySt->SetKonst(aggConst);
  return fieldsArraySt;
}

MIRSymbol *ReflectionAnalysis::GenFieldsMetaData(const Klass &klass) {
  MIRStructType *classType = klass.GetMIRStructType();
  FieldVector fields = classType->GetFields();
  FieldVector staticFields = classType->GetStaticFields();
  ASSERT(fields.size() < fields.max_size() - staticFields.size(), "size too large");
  size_t size = fields.size() + staticFields.size();
  if (size == 0) {
    return nullptr;
  }
  std::vector<std::pair<FieldPair, uint16>> fieldHashvec(size);
  size_t i = 0;
  for (; i < fields.size(); ++i) {
    std::string fieldname = GlobalTables::GetStrTable().GetStringFromStrIdx(fields[i].first);
    ConvertFieldName(fieldname, false);
    uint32 hashcode = GetCharHashIndex(fieldname);
    fieldHashvec[i] = std::make_pair(fields[i], hashcode);
  }
  for (size_t j = 0; j < staticFields.size(); ++j) {
    std::string fieldname = GlobalTables::GetStrTable().GetStringFromStrIdx(staticFields[j].first);
    ConvertFieldName(fieldname, true);
    uint32 hashcode = GetCharHashIndex(fieldname);
    fieldHashvec[i++] = std::make_pair(staticFields[j], hashcode);
  }
  GenAllFieldHash(fieldHashvec);
  // Sort field_hashvec by hashcode.
  std::sort(fieldHashvec.begin(), fieldHashvec.end(),
            [](std::pair<FieldPair, uint16> a, std::pair<FieldPair, uint16> b) {
              maple::uint16 fieldHashA = a.second;
              maple::uint16 fieldHashB = b.second;
              return fieldHashA < fieldHashB;
            });
  std::vector<std::pair<FieldPair, int>> fieldinfoVec(size);
  size_t j = 0;
  size_t k = 0;
  for (auto it = fieldHashvec.begin(); it != fieldHashvec.end(); ++it) {
    FieldPair f = (*it).first;
    if (f.second.second.GetAttr(FLDATTR_static)) {
      fieldinfoVec[j] = std::make_pair(f, -1);
    } else {
      fieldinfoVec[j] = std::make_pair(f, k++);
    }
    ++j;
  }
  ASSERT(i == size, "In class %s: %d fields seen, BUT %d fields declared", klass.GetKlassName().c_str(), i, size);
  MIRSymbol *fieldsArraySt = GenFieldsMeta(klass, fieldinfoVec, fieldHashvec);
  return fieldsArraySt;
}

void ReflectionAnalysis::ConvertMapleClassName(const std::string &mplClassName, std::string &javaDsp) {
  // Convert classname end with _3B, 3 is strlen("_3B")
  unsigned int len = strlen(kClassSuffix);
  if (mplClassName.size() > len && mplClassName.rfind(kClassSuffix, mplClassName.size() - len) != std::string::npos) {
    NameMangler::DecodeMapleNameToJavaDescriptor(mplClassName, javaDsp);
  } else {
    javaDsp = mplClassName;
  }
}

void ReflectionAnalysis::AppendValueByType(std::string &annoArr, const MIRPragmaElement &elem) {
  std::ostringstream oss;
  std::string tmp;
  switch (elem.GetType()) {
    case kValueInt:
    case kValueByte:
    case kValueShort:
      annoArr += std::to_string(elem.GetI32Val());
      break;
    case kValueLong:
      annoArr += std::to_string(elem.GetI64Val());
      break;
    case kValueDouble:
      oss << tmp << std::setiosflags(std::ios::scientific) << std::setprecision(16) << elem.GetDoubleVal();
      annoArr += oss.str();
      break;
    case kValueFloat:
      oss << tmp << std::setiosflags(std::ios::scientific) << std::setprecision(7) << elem.GetFloatVal();
      annoArr += oss.str();
      break;
    case kValueBoolean:
    case kValueChar:
      annoArr += std::to_string(elem.GetU64Val());
      break;
    default: { // kValueString kValueEnum kValueType
      GStrIdx strIdx;
      strIdx.reset(elem.GetU64Val());
      std::string s = GlobalTables::GetStrTable().GetStringFromStrIdx(strIdx);
      uint32 idx = ReflectionAnalysis::FindOrInsertReflectString(s);
      annoArr += annoDelimiterPrefix;
      annoArr += std::to_string(idx);
    }
  }
}

#define COMMON_CASE \
case kValueInt:    \
case kValueLong:   \
case kValueDouble: \
case kValueFloat:  \
case kValueString: \
case kValueBoolean:\
case kValueByte:   \
case kValueShort:  \
case kValueChar:   \
case kValueEnum:   \
case kValueType:

std::string ReflectionAnalysis::GetAnnotationValue(const MapleVector<MIRPragmaElement*> &subelemVector,
                                                   GStrIdx typeStrIdx) {
  std::string annoArray;
  annoArray += (annoArrayStartDelimiter + std::to_string(subelemVector.size()) + annoDelimiter);
  std::string javaDscp;
  ConvertMapleClassName(GlobalTables::GetStrTable().GetStringFromStrIdx(typeStrIdx), javaDscp);
  uint32_t idx = ReflectionAnalysis::FindOrInsertReflectString(javaDscp);
  javaDscp = annoDelimiterPrefix + std::to_string(idx);
  annoArray += javaDscp;
  for (MIRPragmaElement *arrayElem : subelemVector) {
    annoArray += annoDelimiter;
    idx = FindOrInsertReflectString(GlobalTables::GetStrTable().GetStringFromStrIdx(arrayElem->GetNameStrIdx()));
    annoArray += (annoDelimiterPrefix + std::to_string(idx) + annoDelimiter + std::to_string(arrayElem->GetType()));
    annoArray += annoDelimiter;
    annoArray += GetAnnoValueNoArray(*arrayElem);
  }
  annoArray += annoDelimiter;
  annoArray += annoArrayEndDelimiter;
  return annoArray;
}

std::string ReflectionAnalysis::GetArrayValue(const MapleVector<MIRPragmaElement*> &subelemVector) {
  std::string annoArray;
  GStrIdx strIdx;
  uint32_t idx;
  annoArray += (annoArrayStartDelimiter + std::to_string(subelemVector.size()) + annoDelimiter);
  if (!subelemVector.empty()) {
    annoArray += std::to_string(subelemVector[0]->GetType());
    annoArray += annoDelimiter;

    std::string javaDsp;
    ConvertMapleClassName(GlobalTables::GetStrTable().GetStringFromStrIdx(subelemVector[0]->GetTypeStrIdx()), javaDsp);
    std::string typeStr = javaDsp;
    idx = ReflectionAnalysis::FindOrInsertReflectString(typeStr);
    annoArray += (annoDelimiterPrefix + std::to_string(idx) + annoDelimiter);
  }

  for (MIRPragmaElement *arrayElem : subelemVector) {
    MapleVector<MIRPragmaElement*> arrayElemVector = arrayElem->GetSubElemVec();
    switch (arrayElem->GetType()) {
      COMMON_CASE
        AppendValueByType(annoArray, *arrayElem);
        break;
      case kValueAnnotation: {
        annoArray += std::to_string(arrayElemVector.size());
        annoArray += annoDelimiter;
        for (MIRPragmaElement *annoElem : arrayElemVector) {
          std::string tt = GlobalTables::GetStrTable().GetStringFromStrIdx(annoElem->GetNameStrIdx());
          idx = ReflectionAnalysis::FindOrInsertReflectString(tt);
          tt = annoDelimiterPrefix + std::to_string(idx) + annoDelimiter + std::to_string(annoElem->GetType()) +
              annoDelimiter + GetAnnoValueNoArray(*annoElem) + annoDelimiter;
          annoArray += tt;
        }
        break;
      }
      default: {
        annoArray += (std::to_string(arrayElem->GetU64Val()) + annoDelimiter);
        annoArray += GlobalTables::GetStrTable().GetStringFromStrIdx(arrayElem->GetNameStrIdx());
        strIdx.reset(arrayElem->GetU64Val());
        annoArray += (GlobalTables::GetStrTable().GetStringFromStrIdx(strIdx) + annoDelimiter);
      }
    }
    annoArray += annoDelimiter;
  }
  annoArray += annoArrayEndDelimiter;
  return annoArray;
}

std::string ReflectionAnalysis::GetAnnoValueNoArray(const MIRPragmaElement &annoElem) {
  std::string annoArray;
  switch (annoElem.GetType()) {
    COMMON_CASE
      AppendValueByType(annoArray, annoElem);
      break;
    case kValueArray:
      annoArray += GetArrayValue(annoElem.GetSubElemVec());
      break;
    case kValueAnnotation:
      annoArray += GetAnnotationValue(annoElem.GetSubElemVec(), annoElem.GetTypeStrIdx());
      break;
    default: {
      GStrIdx strIdx;
      strIdx.reset(annoElem.GetU64Val());
      std::string javaDescriptor;
      ConvertMapleClassName(GlobalTables::GetStrTable().GetStringFromStrIdx(strIdx), javaDescriptor);
      uint32_t idx = ReflectionAnalysis::FindOrInsertReflectString(javaDescriptor);
      annoArray += annoDelimiterPrefix;
      annoArray += std::to_string(idx);
    }
  }
  return annoArray;
}

void ReflectionAnalysis::GenAnnotation(std::map<int, int> &idxNumMap, std::string &annoArr, MIRStructType &classType,
                                       PragmaKind paragKind, const std::string &paragName, TyIdx fieldTypeIdx,
                                       std::map<int, int> *paramnumArray, int *paramIndex) {
  int annoNum = 0;
  for (MIRPragma *prag : classType.GetPragmaVec()) {
    std::string cmpString = (paragKind == kPragmaVar) ? NameMangler::DecodeName(
        GlobalTables::GetStrTable().GetStringFromStrIdx(prag->GetStrIdx())) :
        GlobalTables::GetStrTable().GetStringFromStrIdx(prag->GetStrIdx());
    bool validTypeFlag = false;
    if (prag->GetTyIdxEx() == fieldTypeIdx || fieldTypeIdx == invalidIdx) {
      validTypeFlag = true;
    }
    if ((prag->GetKind() == paragKind) && (cmpString == paragName) && validTypeFlag) {
      const MapleVector<MIRPragmaElement*> &elemVector = prag->GetElementVector();
      MIRSymbol *classInfo = GlobalTables::GetGsymTable().GetSymbolFromStrIdx(
          GlobalTables::GetTypeTable().GetTypeFromTyIdx(prag->GetTyIdx())->GetNameStrIdx());
      if (classInfo != nullptr && !RtRetentionPolicyCheck(*classInfo)) {
        continue;
      }
      annoNum++;
      idxNumMap[annoNum - 1] = 0;
      GStrIdx gindex = GlobalTables::GetTypeTable().GetTypeFromTyIdx(prag->GetTyIdx())->GetNameStrIdx();
      std::string pregTypeString = GlobalTables::GetStrTable().GetStringFromStrIdx(gindex);
      std::string klassJavaDescriptor;
      ConvertMapleClassName(pregTypeString, klassJavaDescriptor);
      uint32 idx = ReflectionAnalysis::FindOrInsertReflectString(klassJavaDescriptor);
      annoArr += annoDelimiterPrefix;
      annoArr += std::to_string(idx);
      annoArr += annoDelimiter;
      if (paramnumArray != nullptr) {
        int8 x = JudgePara(classType);
        CHECK_FATAL(paramIndex != nullptr, "null ptr check");
        if (x && paragName.find(kInitFuntionStr) != std::string::npos) {
          (*paramnumArray)[(*paramIndex)++] = prag->GetParamNum() + x;
        } else {
          (*paramnumArray)[(*paramIndex)++] = prag->GetParamNum();
        }
      }
      for (MIRPragmaElement *elem : elemVector) {
        idxNumMap[annoNum - 1]++;
        std::string convertTmp =
            NameMangler::DecodeName(GlobalTables::GetStrTable().GetStringFromStrIdx(elem->GetNameStrIdx()));
        idx = ReflectionAnalysis::FindOrInsertReflectString(convertTmp);
        annoArr += (annoDelimiterPrefix + std::to_string(idx) + annoDelimiter +
            std::to_string(elem->GetType()) + annoDelimiter);
        annoArr += GetAnnoValueNoArray(*elem);
        annoArr += annoDelimiter;
      }
    }
  }
}

uint32 ReflectionAnalysis::GetAnnoCstrIndex(std::map<int, int> &idxNumMap, const std::string &annoArr, bool isField) {
  size_t annoNum = idxNumMap.size();
  uint32 signatureIdx = 0;
  if (annoNum == 0) {
    std::string flag = isField ? "1!" : std::to_string(GetAnnoFlag(annoArr)) + annoDelimiter;
    std::string subStr = flag + "0";
    signatureIdx = FindOrInsertReflectString(subStr);
  } else {
    std::string subStr = std::to_string(annoNum);
    subStr += annoDelimiter;
    std::for_each(idxNumMap.begin(), idxNumMap.end(), [&subStr](const std::pair<const int, int> p) {
      subStr += std::to_string(p.second);
      subStr += annoDelimiter;
    });
    subStr += annoArr;
    signatureIdx = static_cast<uint32>(GetDeflateStringIdx(subStr, !isField));
  }
  return signatureIdx;
}

uint32 ReflectionAnalysis::BKDRHash(const std::string &strName, uint32 seed) {
  const char *name = strName.c_str();
  uint32 hash = 0;
  while (*name) {
    uint8_t uName = *name++;
    hash = hash * seed + uName;
  }
  return hash;
}

uint32 ReflectionAnalysis::GetHashIndex(const std::string &strName) {
  constexpr int hashSeed = 211;
  return BKDRHash(strName, hashSeed);
}

void ReflectionAnalysis::GenHotClassNameString(const Klass &klass) {
  if (klass.IsInterface()) {
    return;
  }
  MIRStructType *classType = klass.GetMIRStructType();
  if (!classType->IsLocal()) {
    // External class.
    return;
  }
  if (!klass.HasNativeMethod()) {
    return;  // It's a cold class, we don't care.
  }
  std::string klassName = klass.GetKlassName();
  std::string klassJavaDescriptor;
  NameMangler::DecodeMapleNameToJavaDescriptor(klassName, klassJavaDescriptor);
  (void)ReflectionAnalysis::FindOrInsertRepeatString(klassJavaDescriptor, true);  // Always used.
}

uint32 ReflectionAnalysis::FindOrInsertReflectString(const std::string &str) {
  uint8 hotType = 0;
  return ReflectionAnalysis::FindOrInsertRepeatString(str, false, hotType);
}

MIRSymbol *ReflectionAnalysis::GetClinitFuncSymbol(const Klass &klass) {
  MIRStructType *classType = klass.GetMIRStructType();
  if (classType == nullptr || classType->GetMethods().empty()) {
    return nullptr;
  }
  MIRSymbol *clinitFuncSymbol = nullptr;
  for (MethodPair &methodPair : classType->GetMethods()) {
    MIRSymbol *funcSymA = GlobalTables::GetGsymTable().GetSymbolFromStidx(methodPair.first.Idx());
    MIRFunction *funcA = funcSymA->GetFunction();
    std::string funcName = funcA->GetBaseFuncNameWithType();
    if (funcName == kClinitSuffixStr) {
      clinitFuncSymbol = funcSymA;
      break;
    }
  }
  return clinitFuncSymbol;
}

void ReflectionAnalysis::GenClassMetaData(Klass &klass) {
  MIRModule &module = *mirModule;
  MIRStructType *structType = klass.GetMIRStructType();
  ASSERT(structType != nullptr, "null ptr check!");
  if (!structType->IsLocal()) {
    // External class.
    return;
  }


  std::string klassName = klass.GetKlassName();
  reflectionMuidStr += klassName;
  std::string klassJavaDescriptor;
  NameMangler::DecodeMapleNameToJavaDescriptor(klassName, klassJavaDescriptor);
  uint32 hashIndex = GetHashIndex(klassJavaDescriptor);
  if (kRADebug) {
    LogInfo::MapleLogger(kLlErr) << "========= Gen Class: " << klassJavaDescriptor
                                 << " (" << hashIndex << ") ========\n";
  }
  auto &classMetadataROType =
      static_cast<MIRStructType&>(*GlobalTables::GetTypeTable().GetTypeFromTyIdx(classMetadataRoTyIdx));
  MIRAggConst *newConst = module.GetMemPool()->New<MIRAggConst>(module, classMetadataROType);
  uint32 fieldID = 1;
  // @classname
  uint32 nameIdx = FindOrInsertReflectString(klassJavaDescriptor);
  mirBuilder.AddIntFieldConst(classMetadataROType, *newConst, fieldID++, nameIdx);
  // @iFields: All instance fields.
  uint32 numOfFields = 0;
  bool hasAdded = false;
  if (klass.GetKlassName() == NameMangler::GetInternalNameLiteral(NameMangler::kJavaLangObjectStr)) {
    const GStrIdx stridx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(
        NameMangler::kFieldsInfoPrefixStr + NameMangler::GetInternalNameLiteral(NameMangler::kJavaLangObjectStr));
    MIRSymbol *fieldsSt = GlobalTables::GetGsymTable().GetSymbolFromStrIdx(stridx);
    if (fieldsSt != nullptr) {
      mirBuilder.AddAddrofFieldConst(classMetadataROType, *newConst, fieldID++, *fieldsSt);
      hasAdded = true;
    }
  }
  if (!hasAdded) {
    MIRSymbol *fieldsSt = GenFieldsMetaData(klass);
    if (fieldsSt != nullptr) {
      numOfFields = safe_cast<MIRAggConst>(fieldsSt->GetKonst())->GetConstVec().size();
      // All meta data will be weak if dummy constructors.
      mirBuilder.AddAddrofFieldConst(classMetadataROType, *newConst, fieldID++, *fieldsSt);
    } else {
      mirBuilder.AddIntFieldConst(classMetadataROType, *newConst, fieldID++, 0);
    }
  }
  // @methods: All methods.
  uint32 numOfMethods = 0;
  MIRSymbol *methodsSt;
  methodsSt = GenMethodsMetaData(klass);
  if (methodsSt != nullptr) {
    numOfMethods = safe_cast<MIRAggConst>(methodsSt->GetKonst())->GetConstVec().size();
    mirBuilder.AddAddrofFieldConst(classMetadataROType, *newConst, fieldID++, *methodsSt);
  } else {
    mirBuilder.AddIntFieldConst(classMetadataROType, *newConst, fieldID++, 0);
  }
  // @superclass: Super class and a list of implemented interfaces.
  std::list<Klass*> superClassList;
  for (Klass *superClass : klass.GetSuperKlasses()) {
    superClassList.push_back(superClass);
  }
  if (structType->IsMIRClassType()) {
    MIRClassType *classType = static_cast<MIRClassType*>(structType);
    for (TyIdx const kTyIdx : classType->GetInterfaceImplemented()) {
      Klass *interface = klassH->GetKlassFromTyIdx(kTyIdx);
      if (interface == nullptr) {
        MIRType *type = GlobalTables::GetTypeTable().GetTypeFromTyIdx(kTyIdx);
        LogInfo::MapleLogger() << "Error: Interface " << static_cast<MIRStructType*>(type)->GetName() << " is not found"
                               << "\n";
        LogInfo::MapleLogger(kLlErr) << "Error: Missing interface for " << klass.GetKlassName() << "\n";
        CHECK_FATAL(false, "Missing interface");
      }
      std::list<Klass*>::iterator it = std::find(superClassList.begin(), superClassList.end(), interface);
      if (it == superClassList.end()) {
        superClassList.push_back(interface);
      }
    }
  }

  size_t superClassSize = superClassList.size();
  if (superClassSize >= 1) {
    MIRSymbol *superClassSymbolType = GenSuperClassMetaData(klass, superClassList);
    mirBuilder.AddAddrofFieldConst(classMetadataROType, *newConst, fieldID++, *superClassSymbolType);
  } else {
    mirBuilder.AddIntFieldConst(classMetadataROType, *newConst, fieldID++, 0);
  }
  // @numoffields: Num of fields (own).
  CHECK_FATAL(numOfFields <= 0xffff, "Error:the num of fields is too large");
  mirBuilder.AddIntFieldConst(classMetadataROType, *newConst, fieldID++, numOfFields);
  // @numofmethods: Num of methods in vtable.
  CHECK_FATAL(numOfMethods <= 0xffff, "Error:the num of methods is too large");
  mirBuilder.AddIntFieldConst(classMetadataROType, *newConst, fieldID++, numOfMethods);
  // Do annotation.
  std::string annoArray;
  std::map<int, int> idxNumMap;
  GenAnnotation(idxNumMap, annoArray, *structType, kPragmaClass, klass.GetKlassName(), invalidIdx);
  bool isAnonymous = IsAnonymousClass(annoArray);
  CheckPrivateInnerAndNoSubClass(klass, annoArray);
#ifndef USE_32BIT_REF
  // @flag
  // Array class and primitive class is not generated by compiler.
  uint32 flag = klass.GetFlag(kClassHasFinalizer | kClassReference | kClassFinalizerreferenceSentinel);
  flag = isAnonymous ? (flag | kClassIsanonymousclass) : flag;
  mirBuilder.AddIntFieldConst(classMetadataROType, *newConst, fieldID++, flag);
  // @numofsuperclasses
  CHECK_FATAL(superClassSize <= 0xffff, "Error:the size of superClass is too big");
  mirBuilder.AddIntFieldConst(classMetadataROType, *newConst, fieldID++, superClassSize);
  // @padding
  mirBuilder.AddIntFieldConst(classMetadataROType, *newConst, fieldID++, 0);
#endif  // USE_32BIT_REF
  // @modifier: For class fill ClassAccessFlags.
  uint32 modifier = GetClassAccessFlags(*structType);
  mirBuilder.AddIntFieldConst(classMetadataROType, *newConst, fieldID++, modifier);
  // @annotation: Set annotation field.
  uint32_t signatureIdx = GetAnnoCstrIndex(idxNumMap, annoArray, false);
  mirBuilder.AddIntFieldConst(classMetadataROType, *newConst, fieldID++, signatureIdx);
  // @ClinitFuncAddr
  MIRSymbol *clinitFuncSym = GetClinitFuncSymbol(klass);
  if (clinitFuncSym != nullptr) {
    mirBuilder.AddAddroffuncFieldConst(classMetadataROType, *newConst, fieldID++, *clinitFuncSym);
  } else {
    mirBuilder.AddIntFieldConst(classMetadataROType, *newConst, fieldID++, 0);
  }
  MIRSymbol *classMetadataROSymbolType =
      GetOrCreateSymbol(CLASSINFO_RO_PREFIX_STR + klass.GetKlassName(), classMetadataRoTyIdx, true);
  classMetadataROSymbolType->SetStorageClass(kScFstatic);
  classMetadataROSymbolType->SetKonst(newConst);
  // Class Metadata definition start here.
  auto &classMetadataType =
      static_cast<MIRStructType&>(*GlobalTables::GetTypeTable().GetTypeFromTyIdx(classMetadataTyIdx));
  newConst = module.GetMemPool()->New<MIRAggConst>(module, classMetadataType);
  fieldID = 1;
  // @shadow: multiplex used for def index.
  mirBuilder.AddIntFieldConst(classMetadataType, *newConst, fieldID++, 0);
  // @monitor: multiplex used for class hash.
  mirBuilder.AddIntFieldConst(classMetadataType, *newConst, fieldID++, hashIndex);
  // @class loader: Use maximum value unsigned(-1) as non-initialized flag.
  mirBuilder.AddIntFieldConst(classMetadataType, *newConst, fieldID++, static_cast<uint16>(-1));
  // @objsize: Fill this in the CG.
  mirBuilder.AddIntFieldConst(classMetadataType, *newConst, fieldID++, 0);
#ifdef USE_32BIT_REF
  // @flag
  // Array class and primitive class is not generated by compiler.
  uint32 flag = klass.GetFlag(kClassHasFinalizer | kClassReference | kClassFinalizerreferenceSentinel);
  flag = isAnonymous ? (flag | kClassIsanonymousclass) : flag;
  mirBuilder.AddIntFieldConst(classMetadataType, *newConst, fieldID++, flag);
  // @numofsuperclasses
  CHECK_FATAL(superClassSize <= 0xffff, "Error:the size of superClass is too big");
  mirBuilder.AddIntFieldConst(classMetadataType, *newConst, fieldID++, superClassSize);
#endif  // USE_32BIT_REF
  // @itab
  GStrIdx strIdx = GlobalTables::GetStrTable().GetStrIdxFromName(ITAB_PREFIX_STR + klass.GetKlassName());
  if (strIdx != 0u) {
    MIRSymbol *itableSymbolType = GlobalTables::GetGsymTable().GetSymbolFromStrIdx(strIdx);
    mirBuilder.AddAddrofFieldConst(classMetadataType, *newConst, fieldID++, *itableSymbolType);
  } else {
    mirBuilder.AddIntFieldConst(classMetadataType, *newConst, fieldID++, 0);
  }
  // @vtab
  strIdx = GlobalTables::GetStrTable().GetStrIdxFromName(VTAB_PREFIX_STR + klass.GetKlassName());
  if (strIdx != 0u) {
    MIRSymbol *vtableSymbolType = GlobalTables::GetGsymTable().GetSymbolFromStrIdx(strIdx);
    mirBuilder.AddAddrofFieldConst(classMetadataType, *newConst, fieldID++, *vtableSymbolType);
  } else {
    mirBuilder.AddIntFieldConst(classMetadataType, *newConst, fieldID++, 0);
  }
  // @gctib
  MIRSymbol *gctibSt = GetOrCreateSymbol(GCTIB_PREFIX_STR + klass.GetKlassName(),
                                         GlobalTables::GetTypeTable().GetVoidPtr()->GetTypeIndex(), false);
  if (klass.GetKlassName() != NameMangler::GetInternalNameLiteral(NameMangler::kJavaLangObjectStr)) {
    // Direct access to gctib is only possible within a .so, for most classes.
    gctibSt->SetStorageClass(kScFstatic);
  }
  mirBuilder.AddAddrofFieldConst(classMetadataType, *newConst, fieldID++, *gctibSt);
  // @classinfo ro.
  mirBuilder.AddAddrofFieldConst(classMetadataType, *newConst, fieldID++, *classMetadataROSymbolType);
#ifdef USE_32BIT_REF
  mirBuilder.AddIntFieldConst(classMetadataType, *newConst, fieldID++, 0);
#endif

  // Set default value to class initialization state.
  if (klassH->NeedClinitCheckRecursively(klass)) {
    mirBuilder.AddIntFieldConst(classMetadataType, *newConst, fieldID++, kSEGVAddrForClassUninitialized);
  } else {
    // If this class and its parents do not have <clinit> method, we do not do clinit-check for this class,
    // thus the class initialization state is modified to "Initialized".
    mirBuilder.AddIntFieldConst(classMetadataType, *newConst, fieldID++, kClassInitializedState);
  }

  // Finally generate class metadata here.
  MIRSymbol *classSt = GetOrCreateSymbol(CLASSINFO_PREFIX_STR + klass.GetKlassName(), classMetadataTyIdx, true);
  classSt->SetKonst(newConst);
  classTab.push_back(classSt);
}

int8 ReflectionAnalysis::JudgePara(MIRStructType &classType) {
  for (MIRPragma *prag : classType.GetPragmaVec()) {
    if (prag->GetKind() == kPragmaClass) {
      if ((GlobalTables::GetTypeTable().GetTypeFromTyIdx(prag->GetTyIdx())->GetName() ==
           kArkAnnotationEnclosingClassStr) &&
          !IsStaticClass(classType) && (classType.GetName() != kJavaLangEnumStr)) {
        return 1;
      }
    }
  }
  return 0;
}

bool ReflectionAnalysis::IsAnonymousClass(const std::string &annotationString) {
  // eg: `IC!`AF!4!0!name!30!!
  uint32_t idx = ReflectionAnalysis::FindOrInsertReflectString(kInnerClassStr);
  std::string target = annoDelimiterPrefix + std::to_string(idx) + annoDelimiter;
  size_t pos = annotationString.find(target, 0);
  if (pos != std::string::npos) {
    int i = kAnonymousClassIndex;
    while (i--) {
      pos = annotationString.find("!", pos + 1);
      CHECK_FATAL(pos != std::string::npos, "Error:annotationString in func: isAnonymousClass()");
    }
    int annotationLength = strlen(kAnonymousClassSuffix);
    if (annotationString.substr(pos + 1, annotationLength) == kAnonymousClassSuffix) {
      return true;
    }
  }
  return false;
}

TyIdx ReflectionAnalysis::GenMetaStructType(MIRModule &mirModule, MIRStructType &metaType, const std::string &str) {
  const GStrIdx strIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(str);
  TyIdx tyIdx = GlobalTables::GetTypeTable().GetOrCreateMIRType(&metaType);
  // Global?
  mirModule.GetTypeNameTab()->SetGStrIdxToTyIdx(strIdx, tyIdx);
  mirModule.PushbackTypeDefOrder(strIdx);
  if (GlobalTables::GetTypeTable().GetTypeFromTyIdx(tyIdx)->GetNameStrIdx() == 0u) {
    GlobalTables::GetTypeTable().GetTypeFromTyIdx(tyIdx)->SetNameStrIdx(strIdx);
  }
  return tyIdx;
}

MIRType *ReflectionAnalysis::GetRefFieldType() {
#ifdef USE_32BIT_REF
  return GlobalTables::GetTypeTable().GetUInt32();
#else
  return GlobalTables::GetTypeTable().GetVoidPtr();
#endif  // USE_32BIT_REF
}

void ReflectionAnalysis::GenMetadataType(MIRModule &mirModule) {
  if (classMetadataTyIdx != 0u) { // Types have been generated.
    return;
  }
  // ClassMetaType
  MIRType *typeU8 = GlobalTables::GetTypeTable().GetUInt8();
  MIRType *typeU16 = GlobalTables::GetTypeTable().GetUInt16();
  MIRType *typeI32 = GlobalTables::GetTypeTable().GetInt32();
  MIRType *typeU32 = GlobalTables::GetTypeTable().GetUInt32();
#ifndef USE_32BIT_REF
  MIRType *typeI64 = GlobalTables::GetTypeTable().GetInt64();
  MIRType *typeU64 = GlobalTables::GetTypeTable().GetUInt64();
#endif
  MIRType *typeVoidPtr = GlobalTables::GetTypeTable().GetVoidPtr();
  MIRStructType classMetadataType(kTypeStruct);
  GlobalTables::GetTypeTable().AddFieldToStructType(classMetadataType, kShadowStr, *GetRefFieldType());
  GlobalTables::GetTypeTable().AddFieldToStructType(classMetadataType, kMonitorStr, *typeI32);
  GlobalTables::GetTypeTable().AddFieldToStructType(classMetadataType, kClassloaderStr, *typeU16);
  GlobalTables::GetTypeTable().AddFieldToStructType(classMetadataType, kObjsizeStr, *typeU16);
#ifdef USE_32BIT_REF
  GlobalTables::GetTypeTable().AddFieldToStructType(classMetadataType, kFlagStr, *typeU16);
  GlobalTables::GetTypeTable().AddFieldToStructType(classMetadataType, kNumOfSuperclassesStr, *typeU16);
#endif  // USE_32BIT_REF
  GlobalTables::GetTypeTable().AddFieldToStructType(classMetadataType, kItabStr, *typeVoidPtr);
  GlobalTables::GetTypeTable().AddFieldToStructType(classMetadataType, kVtabStr, *typeVoidPtr);
  GlobalTables::GetTypeTable().AddFieldToStructType(classMetadataType, kGctibStr, *typeVoidPtr);
#ifdef USE_32BIT_REF
  GlobalTables::GetTypeTable().AddFieldToStructType(classMetadataType, kClassinforoStr, *typeI32);
  GlobalTables::GetTypeTable().AddFieldToStructType(classMetadataType, kInstanceOfCacheFalseStr, *typeU32);
#else
  GlobalTables::GetTypeTable().AddFieldToStructType(classMetadataType, kClassinforoStr, *typeVoidPtr);
#endif  // USE_32BIT_REF
  GlobalTables::GetTypeTable().AddFieldToStructType(classMetadataType, kClinitbridgeStr, *typeVoidPtr);
  classMetadataTyIdx = GenMetaStructType(mirModule, classMetadataType, NameMangler::kClassMetadataTypeName);
  MIRStructType classMetadataROType(kTypeStruct);
  GlobalTables::GetTypeTable().AddFieldToStructType(classMetadataROType, kClassNameStr, *typeVoidPtr);
  GlobalTables::GetTypeTable().AddFieldToStructType(classMetadataROType, kIfieldsStr, *typeVoidPtr);
  GlobalTables::GetTypeTable().AddFieldToStructType(classMetadataROType, kMethodsStr, *typeVoidPtr);
  // For array, this is component class; For primitive type, this is nullptr;
  // For general class, this is superclass (only one superclass), this is a pointer to a superclass&interface array.
  GlobalTables::GetTypeTable().AddFieldToStructType(classMetadataROType, kSuperclassOrComponentclassStr, *typeVoidPtr);
  GlobalTables::GetTypeTable().AddFieldToStructType(classMetadataROType, kNumOfFieldsStr, *typeU16);
  GlobalTables::GetTypeTable().AddFieldToStructType(classMetadataROType, kNumOfMethodsStr, *typeU16);
#ifndef USE_32BIT_REF
  GlobalTables::GetTypeTable().AddFieldToStructType(classMetadataROType, kFlagStr, *typeU16);
  GlobalTables::GetTypeTable().AddFieldToStructType(classMetadataROType, kNumOfSuperclassesStr, *typeU16);
  GlobalTables::GetTypeTable().AddFieldToStructType(classMetadataROType, kPaddingStr, *typeU32);
#endif  // USE_32BIT_REF
  GlobalTables::GetTypeTable().AddFieldToStructType(classMetadataROType, kModStr, *typeI32);
  GlobalTables::GetTypeTable().AddFieldToStructType(classMetadataROType, kAnnotationStr, *typeI32);
  GlobalTables::GetTypeTable().AddFieldToStructType(classMetadataROType, kClinitAddrStr, *typeI32);
  classMetadataRoTyIdx = GenMetaStructType(mirModule, classMetadataROType, kClassMetadataRoTypeName);
  // MethodInfoType.
  MIRStructType methodInfoType(kTypeStruct);
#ifdef USE_32BIT_REF
  GlobalTables::GetTypeTable().AddFieldToStructType(methodInfoType, kMethodInVtabIndexStr, *typeI32);
  GlobalTables::GetTypeTable().AddFieldToStructType(methodInfoType, kDeclaringclassStr, *typeI32);
  GlobalTables::GetTypeTable().AddFieldToStructType(methodInfoType, kAddrStr, *typeI32);
#else
  GlobalTables::GetTypeTable().AddFieldToStructType(methodInfoType, kMethodInVtabIndexStr, *typeI64);
  GlobalTables::GetTypeTable().AddFieldToStructType(methodInfoType, kDeclaringclassStr, *typeI64);
  GlobalTables::GetTypeTable().AddFieldToStructType(methodInfoType, kAddrStr, *typeI64);
#endif
  GlobalTables::GetTypeTable().AddFieldToStructType(methodInfoType, kModStr, *typeI32);
  GlobalTables::GetTypeTable().AddFieldToStructType(methodInfoType, kMethodNameStr, *typeI32);
  GlobalTables::GetTypeTable().AddFieldToStructType(methodInfoType, kSignatureNameStr, *typeI32);
  GlobalTables::GetTypeTable().AddFieldToStructType(methodInfoType, kAnnotationvalueStr, *typeI32);
  GlobalTables::GetTypeTable().AddFieldToStructType(methodInfoType, kFlagStr, *typeU16);
  GlobalTables::GetTypeTable().AddFieldToStructType(methodInfoType, kArgsizeStr, *typeU16);
#ifndef USE_32BIT_REF
  GlobalTables::GetTypeTable().AddFieldToStructType(methodInfoType, kPaddingStr, *typeU32);
#endif  // USE_32BIT_REF
  methodsInfoTyIdx = GenMetaStructType(mirModule, methodInfoType, kMethodInfoTypeName);
  // MethodInfoCompactType.
  MIRStructType methodInfoCompactType(kTypeStruct);
  GlobalTables::GetTypeTable().AddFieldToStructType(methodInfoCompactType, kMethodInVtabIndexStr, *typeI32);
  GlobalTables::GetTypeTable().AddFieldToStructType(methodInfoCompactType, kAddrStr, *typeI32);
  GlobalTables::GetTypeTable().AddFieldToStructType(methodInfoCompactType, kLebPadding0Str, *typeU8);
  methodsInfoCompactTyIdx = GenMetaStructType(mirModule, methodInfoCompactType, kMethodInfoCompactTypeName);
  // FieldInfoType.
  MIRStructType fieldInfoType(kTypeStruct);
#ifndef USE_32BIT_REF
  GlobalTables::GetTypeTable().AddFieldToStructType(fieldInfoType, kOffsetStr, *typeU64);
#else
  GlobalTables::GetTypeTable().AddFieldToStructType(fieldInfoType, kOffsetStr, *typeI32);
#endif  // USE_32BIT_REF
  GlobalTables::GetTypeTable().AddFieldToStructType(fieldInfoType, kModStr, *typeU32);
  GlobalTables::GetTypeTable().AddFieldToStructType(fieldInfoType, kFlagStr, *typeU16);
  GlobalTables::GetTypeTable().AddFieldToStructType(fieldInfoType, kIndexStr, *typeU16);
#ifndef USE_32BIT_REF
  GlobalTables::GetTypeTable().AddFieldToStructType(fieldInfoType, kTypeNameStr, *typeI64);
#else
  GlobalTables::GetTypeTable().AddFieldToStructType(fieldInfoType, kTypeNameStr, *typeI32);
#endif  // USE_32BIT_REF
  GlobalTables::GetTypeTable().AddFieldToStructType(fieldInfoType, kFieldNameStr, *typeU32);
  GlobalTables::GetTypeTable().AddFieldToStructType(fieldInfoType, kAnnotationStr, *typeU32);
  GlobalTables::GetTypeTable().AddFieldToStructType(fieldInfoType, kDeclaringclassStr, *GetRefFieldType());
  fieldsInfoTyIdx = GenMetaStructType(mirModule, fieldInfoType, kFieldInfoTypeName);
  // FieldInfoType Compact.
  MIRStructType fieldInfoCompactType(kTypeStruct);
  GlobalTables::GetTypeTable().AddFieldToStructType(fieldInfoCompactType, kOffsetStr, *typeU32);
  GlobalTables::GetTypeTable().AddFieldToStructType(fieldInfoCompactType, kLebPadding0Str, *typeU8);
  fieldsInfoCompactTyIdx = GenMetaStructType(mirModule, fieldInfoCompactType, kFieldInfoCompactTypeName);
  // SuperClassMetaType.
  MIRStructType superclassMetadataType(kTypeStruct);
  GlobalTables::GetTypeTable().AddFieldToStructType(superclassMetadataType, kSuperclassinfoStr, *typeVoidPtr);
  superclassMetadataTyIdx = GenMetaStructType(mirModule, superclassMetadataType, kSuperclassMetadataTypeName);
  // FieldOffsetDataType.
  MIRStructType fieldOffsetDataType(kTypeStruct);
  GlobalTables::GetTypeTable().AddFieldToStructType(fieldOffsetDataType, kFieldOffsetDataStr, *typeVoidPtr);
  fieldOffsetDataTyIdx = GenMetaStructType(mirModule, fieldOffsetDataType, kFieldOffsetDataTypeName);
  // MethodAddrDataType.
  MIRStructType methodAddrDataType(kTypeStruct);
  GlobalTables::GetTypeTable().AddFieldToStructType(methodAddrDataType, kMethodAddrDataStr, *typeVoidPtr);
  methodAddrDataTyIdx = GenMetaStructType(mirModule, methodAddrDataType, kMethodAddrDataTypeName);
}

void ReflectionAnalysis::GenClassHashMetaData() {
  MIRType *type = GlobalTables::GetTypeTable().GetVoidPtr();
  MIRModule &module = *mirModule;
  CHECK_FATAL(type != nullptr, "type is null in ReflectionAnalysis::GenClassHashMetaData");
  if (kRADebug) {
    LogInfo::MapleLogger(kLlErr) << "========= HASH TABLE ========\n";
  }
  if (classTab.empty()) {
    return;
  }
  std::string bucketName = NameMangler::kMuidClassMetadataBucketPrefixStr + module.GetFileNameAsPostfix();
  size_t bucketArraySize = classTab.size();
  MIRArrayType &bucketArraytype = *GlobalTables::GetTypeTable().GetOrCreateArrayType(*type, bucketArraySize);
  MIRSymbol *bucketSt = GetOrCreateSymbol(bucketName, bucketArraytype.GetTypeIndex(), true);
  MIRAggConst *bucketAggconst = module.GetMemPool()->New<MIRAggConst>(module, bucketArraytype);
  if (bucketAggconst == nullptr) {
    return;
  }
  for (MIRSymbol *classSt : classTab) {
    AddrofNode *classExpr = mirBuilder.CreateExprAddrof(0, *classSt);
    MIRType *ptrType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(PTY_ptr);
    MIRConst *classConst =
        module.GetMemPool()->New<MIRAddrofConst>(classExpr->GetStIdx(), classExpr->GetFieldID(), *ptrType);
    bucketAggconst->PushBack(classConst);
  }
  bucketSt->SetKonst(bucketAggconst);
}

static void ReflectionAnalysisGenStrTab(MIRModule &mirModule, const std::string &strTab,
                                        const std::string &strTabName) {
  MIRBuilder *mirBuilder = mirModule.GetMIRBuilder();
  size_t strTabSize = strTab.length();
  if (strTabSize == 1) {
    return;
  }
  MIRArrayType &strTabType =
      *GlobalTables::GetTypeTable().GetOrCreateArrayType(*GlobalTables::GetTypeTable().GetUInt8(), strTabSize);
  MIRSymbol *strTabSt = mirBuilder->CreateGlobalDecl(strTabName, strTabType);
  MIRAggConst *strTabAggconst = mirModule.GetMemPool()->New<MIRAggConst>(mirModule, strTabType);
  if (strTabAggconst == nullptr) {
    return;
  }
  strTabSt->SetStorageClass(kScFstatic);
  for (char c : strTab) {
    MIRConst *newConst = GlobalTables::GetIntConstTable().GetOrCreateIntConst(
        c, *GlobalTables::GetTypeTable().GetUInt8());
    strTabAggconst->PushBack(newConst);
  }
  strTabSt->SetKonst(strTabAggconst);
}

void ReflectionAnalysis::GenStrTab(MIRModule &mirModule) {
  // Hot string tab.
  std::string hotStrtabName = NameMangler::kReflectionStartHotStrtabPrefixStr + mirModule.GetFileNameAsPostfix();
  ReflectionAnalysisGenStrTab(mirModule, strTabStartHot, hotStrtabName);
  hotStrtabName = NameMangler::kReflectionBothHotStrTabPrefixStr + mirModule.GetFileNameAsPostfix();
  ReflectionAnalysisGenStrTab(mirModule, strTabBothHot, hotStrtabName);
  hotStrtabName = NameMangler::kReflectionRunHotStrtabPrefixStr + mirModule.GetFileNameAsPostfix();
  ReflectionAnalysisGenStrTab(mirModule, strTabRunHot, hotStrtabName);
  // Cold string tab.
  std::string strTabName = NameMangler::kReflectionStrtabPrefixStr + mirModule.GetFileNameAsPostfix();
  ReflectionAnalysisGenStrTab(mirModule, strTab, strTabName);
}

void ReflectionAnalysis::MarkWeakMethods() {
  if (!isLibcore) {
    return;
  }

  GStrIdx classNames[] = { GetOrCreateGStrIdxFromName(NameMangler::kJavaLangClassStr),
                           GetOrCreateGStrIdxFromName(NameMangler::kJavaLangObjectStr),
                           GetOrCreateGStrIdxFromName(NameMangler::kReflectionClassMethodStr),
                           GetOrCreateGStrIdxFromName(kReflectionReferencePrefixStr) };
  for (GStrIdx nameIdx : classNames) {
    Klass *klass = klassH->GetKlassFromStrIdx(nameIdx);
    if (klass == nullptr) {
      continue;
    }
    MIRClassType *classType = klass->GetMIRClassType();
    for (const MethodPair &methodPair : classType->GetMethods()) {
      MIRSymbol *funcSym = GlobalTables::GetGsymTable().GetSymbolFromStidx(methodPair.first.Idx());
      MIRFunction *mirfunc = funcSym->GetFunction();
      if (mirfunc != nullptr) {
        mirfunc->SetAttr(FUNCATTR_weak);  // It's marked weak since RT-first.
      }
    }
  }
}


void ReflectionAnalysis::Run() {
  MarkWeakMethods();
  GenMetadataType(*mirModule);
  const MapleVector<Klass*> &klasses = klassH->GetTopoSortedKlasses();
  if (kRADebug) {
    LogInfo::MapleLogger(kLlErr) << "========= Gen Class: Total " << klasses.size() << " ========\n";
  }
  // Cluster classname together in reflection string table to improve the locality.
  for (Klass *klass : klasses) {
    ASSERT_NOT_NULL(klass);
    GenHotClassNameString(*klass);
  }
  for (Klass *klass : klasses) {
    GenClassMetaData(*klass);
    // Collect the full information about the classmetadata.
    reflectionMuidStr = GetMUID(reflectionMuidStr).ToStr();
  }
  reflectionMuidStr += MUIDReplacement::GetMplMd5().ToStr();
  MUIDReplacement::SetMplMd5(GetMUID(reflectionMuidStr));
  reflectionMuidStr.clear();
  reflectionMuidStr.shrink_to_fit();
  GenClassHashMetaData();
  for (Klass *klass : klasses) {
    MIRStructType *mirStruct = klass->GetMIRStructType();
    mirStruct->GetPragmaVec().clear();
    mirStruct->GetPragmaVec().shrink_to_fit();
  }
  memPoolCtrler.DeleteMemPool(mirModule->GetPragmaMemPool());
}

AnalysisResult *DoReflectionAnalysis::Run(MIRModule *module, ModuleResultMgr *moduleResultMgr) {
  MemPool *memPool = memPoolCtrler.NewMemPool("ReflectionAnalysis mempool");
  auto *kh = static_cast<KlassHierarchy*>(moduleResultMgr->GetAnalysisResult(MoPhase_CHA, module));
  ASSERT_NOT_NULL(kh);
  maple::MIRBuilder mirBuilder(module);
  ReflectionAnalysis *rv = memPool->New<ReflectionAnalysis>(module, memPool, kh, mirBuilder);
  if (rv == nullptr) {
    CHECK_FATAL(false, "failed to allocate memory");
  }
  rv->Run();
  // This is a transform phase, delete mempool.
  memPoolCtrler.DeleteMemPool(memPool);
  return nullptr;
}
}  // namespace maple
