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
#ifndef MAPLE_IR_INCLUDE_BIN_MPL_EXPORT_H
#define MAPLE_IR_INCLUDE_BIN_MPL_EXPORT_H
#include "mir_module.h"
#include "mir_nodes.h"
#include "mir_function.h"
#include "mir_preg.h"
#include "parser_opt.h"
#include "module_phase.h"

namespace maple {
enum : uint8 {
  kBinString = 1,
  kBinUsrString = kBinString,
  kBinInitConst = 2,
  kBinSymbol = 3,
  kBinFunction = 4,
  kBinCallinfo = 5,
  kBinKindTypeScalar = 6,
  kBinKindTypeByName = 7,
  kBinKindTypePointer = 8,
  kBinKindTypeFArray = 9,
  kBinKindTypeJarray = 10,
  kBinKindTypeArray = 11,
  kBinKindTypeFunction = 12,
  kBinKindTypeParam = 13,
  kBinKindTypeInstantVector = 14,
  kBinKindTypeGenericInstant = 15,
  kBinKindTypeBitField = 16,
  kBinKindTypeStruct = 17,     // for kTypeStruct, kTypeStructIncomplete and kTypeUnion
  kBinKindTypeClass = 18,      // for kTypeClass, and kTypeClassIncomplete
  kBinKindTypeInterface = 19,  // for kTypeInterface, and kTypeInterfaceIncomplete
  kBinKindConstInt = 20,
  kBinKindConstAddrof = 21,
  kBinKindConstAddrofFunc = 22,
  kBinKindConstStr = 23,
  kBinKindConstStr16 = 24,
  kBinKindConstFloat = 25,
  kBinKindConstDouble = 26,
  kBinKindConstAgg = 27,
  kBinKindConstSt = 28,
  kBinContentStart = 29,
  kBinStrStart = 30,
  kBinTypeStart = 31,
  kBinCgStart = 32,
  kBinSeStart = 33,
  kBinFinish = 34,
  kStartMethod = 35,
  kBinEaCgNode = 36,
  kBinEaCgActNode = 37,
  kBinEaCgFieldNode = 38,
  kBinEaCgRefNode = 39,
  kBinEaCgObjNode = 40,
  kBinEaCgStart = 41,
  kBinEaStart = 42
};

// this value is used to check wether a file is a binary mplt file
constexpr int32 kMpltMagicNumber = 0xC0FFEE;
class BinaryMplExport {
 public:
  explicit BinaryMplExport(MIRModule &md);
  virtual ~BinaryMplExport() = default;

  void Export(const std::string &fname);
  void WriteNum(int64 x);
  void Write(uint8 b);
  void OutputType(TyIdx tyIdx);
  void OutputConst(MIRConst *c);
  void OutputConstBase(const MIRConst &c);
  void OutputTypeBase(const MIRType &type);
  void OutputTypePairs(const MIRInstantVectorType &typ);
  void OutputStr(const GStrIdx &gstr);
  void OutputUsrStr(UStrIdx ustr);
  void OutputTypeAttrs(const TypeAttrs &ta);
  void OutputPragmaElement(const MIRPragmaElement &e);
  void OutputPragma(const MIRPragma &p);
  void OutputFieldPair(const FieldPair &fp);
  void OutputMethodPair(const MethodPair &memPool);
  void OutputFieldsOfStruct(const FieldVector &fields);
  void OutputMethodsOfStruct(const MethodVector &methods);
  void OutputStructTypeData(const MIRStructType &type);
  void OutputImplementedInterfaces(const std::vector<TyIdx> &interfaces);
  void OutputInfoIsString(const std::vector<bool> &infoIsString);
  void OutputInfo(const std::vector<MIRInfoPair> &info, const std::vector<bool> &infoIsString);
  void OutputPragmaVec(const std::vector<MIRPragma*> &pragmaVec);
  void OutputClassTypeData(const MIRClassType &type);
  void OutputSymbol(const MIRSymbol *sym);
  void OutputFunction(PUIdx puIdx);
  void OutWords(uint8 &typeTagged, int64 targetTag, uint16 size);
  void OutputInterfaceTypeData(const MIRInterfaceType &type);
  const MIRModule &GetMIRModule() const {
    return mod;
  }

 private:
  void WriteContentField(int fieldNum, uint64 *fieldStartP);
  void WriteStrField(uint64 contentIdx);
  void WriteTypeField(uint64 contentIdx);
  void Init();
  void WriteInt(int32 x);
  uint8 Read();
  int32 ReadInt();
  void WriteInt64(int64 x);
  void WriteAsciiStr(const std::string &str);
  void Fixup(size_t i, int32 x);
  void DumpBuf(const std::string &modid);
  void AppendAt(const std::string &fname, int32 ipaIdx);
  void ExpandFourBuffSize();

  MIRModule &mod;
  size_t bufI = 0;
  std::vector<uint8> buf;
  std::unordered_map<GStrIdx, int64, GStrIdxHash> gStrMark;
  std::unordered_map<MIRFunction*, int64> funcMark;
  std::string importFileName;
  std::unordered_map<UStrIdx, int64, UStrIdxHash> uStrMark;
  std::unordered_map<const MIRSymbol*, int64> symMark;
  std::unordered_map<MIRType*, int64> typMark;
  static int typeMarkOffset;  // offset of mark (tag in binmplimport) resulting from duplicated function
};

}  // namespace maple
#endif  // MAPLE_IR_INCLUDE_BIN_MPL_EXPORT_H
