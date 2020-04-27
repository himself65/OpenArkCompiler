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
#include "fe_java_string_manager.h"
#include <fstream>
#include "global_tables.h"
#include "namemangler.h"
#include "muid.h"
#include "literalstrname.h"
#include "fe_config_parallel.h"
#include "feir_type.h"

namespace maple {
FEJavaStringManager::FEJavaStringManager(MIRModule &argModule)
    : module(argModule) {}

FEJavaStringManager::~FEJavaStringManager() {
  typeString = nullptr;
}

void FEJavaStringManager::LoadProfilingData(const std::string &profileFileName) {
  std::ifstream inFile(profileFileName);
  if (!inFile.is_open()) {
    WARN(kLncWarn, "Cannot open literal profile data file %s", profileFileName.c_str());
    return;
  }
  std::string literalName;
  while (std::getline(inFile, literalName)) {
    if (!literalName.empty()) {
      (void)preloadSet.insert(literalName);
    }
  }
  inFile.close();
}

MIRSymbol *FEJavaStringManager::GetLiteralPtrVar(const MIRSymbol *var) const {
  auto it = literalMap.find(var);
  if (it != literalMap.end()) {
    return it->second;
  } else {
    return nullptr;
  }
}

MIRSymbol *FEJavaStringManager::GetLiteralPtrVar(const std::string &str) const {
  MIRSymbol *literalVar = GetLiteralVar(str);
  return GetLiteralPtrVar(literalVar);
}

MIRSymbol *FEJavaStringManager::GetLiteralPtrVar(const std::u16string &strU16) const {
  MIRSymbol *literalVar = GetLiteralVar(strU16);
  return GetLiteralPtrVar(literalVar);
}

MIRSymbol *FEJavaStringManager::CreateLiteralVar(MIRBuilder &mirBuilder, const std::string &str, bool isFieldValue) {
  std::u16string strU16;
  (void)NameMangler::UTF8ToUTF16(strU16, str);
  return CreateLiteralVar(mirBuilder, strU16, isFieldValue);
}

MIRSymbol *FEJavaStringManager::CreateLiteralVar(MIRBuilder &mirBuilder, const std::u16string &strU16,
                                                 bool isFieldValue) {
  MPLFE_PARALLEL_FORBIDDEN();
  if (typeString == nullptr) {
    FEIRTypeDefault type(PTY_ref);
    type.LoadFromJavaTypeName("Ljava/lang/String;", false);
    typeString = type.GenerateMIRTypeAuto(kSrcLangJava);
  }
  MIRSymbol *literalVar = GetLiteralVar(strU16);
  if (literalVar != nullptr) {
    return literalVar;
  }
  std::string literalGlobalName = GetLiteralGlobalName(strU16);
  bool compress = useCompressedJavaString && IsAllASCII(strU16);
  MIRArrayType *byteArrayType = ConstructArrayType4Str(strU16, compress);
  literalVar = mirBuilder.GetOrCreateGlobalDecl(literalGlobalName.c_str(), *byteArrayType);
  MIRAggConst *strConst = CreateByteArrayConst(strU16, *byteArrayType, compress);
  literalVar->SetKonst(strConst);
  literalVar->SetAttr(ATTR_readonly);
  literalVar->SetStorageClass(kScFstatic);
  bool isHotLiteral = false;
  if (preloadSet.find(literalGlobalName) != preloadSet.end()) {
    isHotLiteral = true;
    (void)literalSet.insert(literalVar);
  }
  if (isFieldValue) {
    (void)fieldValueSet.insert(literalVar);
  }
  if (isFieldValue || isHotLiteral) {
    std::string literalGlobalPtrName = NameMangler::kPtrPrefixStr + literalGlobalName;
    MIRSymbol *literalVarPtr = mirBuilder.GetOrCreateGlobalDecl(literalGlobalPtrName.c_str(), *typeString);
    literalVarPtr->SetStorageClass(literalVar->GetStorageClass());
    AddrofNode *expr = mirBuilder.CreateExprAddrof(0, *literalVar, module.GetMemPool());
    MIRConst *cst = module.GetMemPool()->New<MIRAddrofConst>(
        expr->GetStIdx(), expr->GetFieldID(), *GlobalTables::GetTypeTable().GetPtr());
    literalVarPtr->SetKonst(cst);
    literalMap[literalVar] = literalVarPtr;
  }
  (void)GlobalTables::GetConstPool().GetConstU16StringPool().insert(std::make_pair(strU16, literalVar));
  return literalVar;
}

MIRSymbol *FEJavaStringManager::GetLiteralVar(const std::string &str) const {
  std::u16string strU16;
  (void)NameMangler::UTF8ToUTF16(strU16, str);
  return GetLiteralVar(strU16);
}

MIRSymbol *FEJavaStringManager::GetLiteralVar(const std::u16string &strU16) const {
  auto it = GlobalTables::GetConstPool().GetConstU16StringPool().find(strU16);
  if (it != GlobalTables::GetConstPool().GetConstU16StringPool().end()) {
    return it->second;
  }
  return nullptr;
}

std::string FEJavaStringManager::GetLiteralGlobalName(const std::u16string &strU16) {
  std::string literalGlobalName;
  std::vector<uint8> swapped = SwapBytes(strU16);
  if (strU16.length() == 0) {
    literalGlobalName = LiteralStrName::GetLiteralStrName(swapped.data(), 0);
  } else {
    literalGlobalName = LiteralStrName::GetLiteralStrName(swapped.data(), strU16.length() << 1);
  }
  return literalGlobalName;
}

bool FEJavaStringManager::IsAllASCII(const std::u16string &strU16) {
  if (strU16.length() == 0) {
    return false;
  }
  for (size_t i = 0; i < strU16.length(); ++i) {
    uint16 val = ExchangeBytesPosition(strU16[i]);
    if (val >= CHAR_MAX) {
      return false;
    }
  }
  return true;
}

MIRArrayType *FEJavaStringManager::ConstructArrayType4Str(const std::u16string &strU16, bool compressible) const {
  MPLFE_PARALLEL_FORBIDDEN();
  uint32 arraySize[1];
  int length = compressible ? strU16.length() : (strU16.length() * 2);  // use 2 bytes per char in uncompress mode
#ifdef JAVA_OBJ_IN_MFILE
#ifdef USE_32BIT_REF
  int sizeInBytes = 16 + length;                // shadow(4B)+monitor(4B)+count(4B)+hash(4B)+content
#else                                           // !USE_32BIT_REF
  int sizeInBytes = 20 + length;                // shadow(8B)+monitor(4B)+count(4B)+hash(4B)+content
#endif                                          // USE_32BIT_REF
#else                                           // !JAVA_OBJ_IN_MFILE
  int sizeInBytes = 8 + length;                 // count(4B)+hash(4B)+content
#endif                                          // JAVA_OBJ_IN_MFILE
  int sizeInLongs = (sizeInBytes - 1) / 8 + 1;  // round up to 8B units
  arraySize[0] = sizeInLongs;
  MIRArrayType *byteArrayType = static_cast<MIRArrayType*>(
      GlobalTables::GetTypeTable().GetOrCreateArrayType(*GlobalTables::GetTypeTable().GetUInt64(), 1, arraySize));
  return byteArrayType;
}

MIRAggConst *FEJavaStringManager::CreateByteArrayConst(const std::u16string &strU16, MIRArrayType &byteArrayType,
                                                       bool compressible) const {
  MIRAggConst *newconst = module.GetMemPool()->New<MIRAggConst>(module, byteArrayType);
  MIRType *uInt64 = GlobalTables::GetTypeTable().GetUInt64();
  MemPool *mp = module.GetMemPool();
  DWBuffer currData = { 0, 0 };

#ifdef JAVA_OBJ_IN_MFILE
  // @shadow
  // To avoid linker touch cold pages, the classinfo is not set ready in file.
  // It will be set at runtime
#ifdef USE_32BIT_REF
  AddDataIntoByteArray(*newconst, *mp, currData, static_cast<uint32>(0), *uInt64);
#else
  AddDataIntoByteArray(*newconst, *mp, currData, static_cast<uint64>(0), *uInt64);
#endif  // USE_32BIT_REF
  // @monitor
  AddDataIntoByteArray(*newconst, *mp, currData, static_cast<uint32>(0), *uInt64);
#endif  // JAVA_OBJ_IN_MFILE

  // @count
  uint32_t strCount = strU16.length() ? ((strU16.length() * 2) | compressible) : 0;
  AddDataIntoByteArray(*newconst, *mp, currData, strCount, *uInt64);

  // @hash
  uint32_t hash = LiteralStrName::CalculateHashSwapByte(strU16.data(), strU16.length());
  AddDataIntoByteArray(*newconst, *mp, currData, hash, *uInt64);

  // @content
  if (compressible) {
    for (size_t i = 0; i < strU16.size(); i++) {
      AddDataIntoByteArray(*newconst, *mp, currData, static_cast<uint8>(ExchangeBytesPosition(strU16[i])), *uInt64);
    }
  } else {
    for (size_t i = 0; i < strU16.size(); i++) {
      AddDataIntoByteArray(*newconst, *mp, currData, static_cast<uint16>(ExchangeBytesPosition(strU16[i])), *uInt64);
    }
  }
  // in case there're remaining data in the buffer
  FinishByteArray(*newconst, *mp, currData, *uInt64);
  return newconst;
}

std::vector<uint8> FEJavaStringManager::SwapBytes(const std::u16string &strU16) {
  std::vector<uint8> out;
  for (size_t i = 0; i < strU16.length(); ++i) {
    uint16 c16 = strU16[i];
    out.push_back((c16 & 0xFF00) >> 8);
    out.push_back(c16 & 0xFF);
  }
  out.push_back(0);
  out.push_back(0);
  return out;
}

uint16 FEJavaStringManager::ExchangeBytesPosition(uint16 input) {
  uint16 lowerByte = input << 8;
  uint16 higherByte = input >> 8;
  return lowerByte | higherByte;
}

template <typename T>
void FEJavaStringManager::AddDataIntoByteArray(MIRAggConst &newConst, MemPool &mp, DWBuffer &buf, T data,
                                               MIRType &uInt64) {
  if (buf.pos == 8) {  // buffer is already full
    newConst.PushBack(mp.New<MIRIntConst>(buf.data, uInt64));
    buf.data = 0;
    buf.pos = 0;
  }
  CHECK_FATAL(((buf.pos + sizeof(T)) <= 8), "inserted data exceeds current buffer capacity");
  buf.data |= ((static_cast<uint64>(data)) << (buf.pos * 8));
  buf.pos += sizeof(T);
}

void FEJavaStringManager::FinishByteArray(MIRAggConst &newConst, MemPool &mp, DWBuffer &buf, MIRType &uInt64) {
  if (buf.pos > 0) {  // there're data inside buffer
    newConst.PushBack(mp.New<MIRIntConst>(buf.data, uInt64));
    buf.data = 0;
    buf.pos = 0;
  }
}
}  // namespace maple