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
#ifndef MAPLE_ME_INCLUDE_ORIG_SYMBOL_H
#define MAPLE_ME_INCLUDE_ORIG_SYMBOL_H
#include "mir_module.h"
#include "mir_symbol.h"
#include "mir_preg.h"
#include "mir_function.h"
#include "mpl_number.h"

// This file defines the data structure OriginalSt that represents a program
// symbol occurring in the code of the program being optimized.
namespace maple {
class OStTag;
using OStIdx = utils::Index<OStTag, uint32>;

constexpr int kInitVersion = 0;
class VarMeExpr;  // circular dependency exists, no other choice
class OriginalSt {
 public:
  OriginalSt(uint32 index, PregIdx rIdx, PUIdx pIdx, MapleAllocator &alloc)
      : OriginalSt(OStIdx(index), alloc, true, false, 0, pIdx, kPregOst, false, { .pregIdx = rIdx }) {}

  OriginalSt(uint32 index, MIRSymbol &mirSt, PUIdx pIdx, FieldID fieldID, MapleAllocator &alloc)
      : OriginalSt(OStIdx(index), alloc, mirSt.IsLocal(), mirSt.GetStorageClass() == kScFormal, fieldID, pIdx,
                   kSymbolOst, mirSt.IgnoreRC(), { .mirSt = &mirSt }) {}

  ~OriginalSt() = default;

  void Dump() const;
  PregIdx GetPregIdx() const {
    ASSERT(ostType == kPregOst, "OriginalSt must be PregOst");
    return symOrPreg.pregIdx;
  }

  MIRPreg *GetMIRPreg() const {
    ASSERT(ostType == kPregOst, "OriginalSt must be PregOst");
    return GlobalTables::GetGsymTable().GetModule()->CurFunction()->GetPregTab()->PregFromPregIdx(symOrPreg.pregIdx);
  }

  const MIRSymbol *GetMIRSymbol() const {
    ASSERT(ostType == kSymbolOst, "OriginalSt must be SymbolOst");
    return symOrPreg.mirSt;
  }

  MIRSymbol *GetMIRSymbol() {
    ASSERT(ostType == kSymbolOst, "OriginalSt must be SymbolOst");
    return symOrPreg.mirSt;
  }

  bool HasAttr(AttrKind attrKind) const {
    if (ostType == kSymbolOst) {
      TypeAttrs typeAttr = symOrPreg.mirSt->GetAttrs();
      if (typeAttr.GetAttr(attrKind)) {
        return true;
      }
    }
    return false;
  }

  bool IsLocal() const {
    return isLocal;
  }

  bool IsFormal() const {
    return isFormal;
  }
  void SetIsFormal(bool isFormal) {
    this->isFormal = isFormal;
  }

  bool IsFinal() const {
    return isFinal;
  }
  void SetIsFinal(bool isFinal = true) {
    this->isFinal = isFinal;
  }

  bool IsPrivate() const {
    return isPrivate;
  }
  void SetIsPrivate(bool isPrivate) {
    this->isPrivate = isPrivate;
  }

  bool IsVolatile() const {
    return (ostType == kSymbolOst) ? symOrPreg.mirSt->IsVolatile() : false;
  }

  bool IsVrNeeded() const {
    return (ostType == kSymbolOst) ? symOrPreg.mirSt->GetIsTmp() : false;
  }

  bool Equal(const OriginalSt &ost) const;

  bool IsRealSymbol() const {
    return (ostType == kSymbolOst || ostType == kPregOst);
  }

  bool IsSymbolOst() const {
    return ostType == kSymbolOst;
  }

  bool IsPregOst() const {
    return (ostType == kPregOst);
  }

  bool IsSpecialPreg() const {
    return ostType == kPregOst && symOrPreg.pregIdx < 0;
  }

  int8 GetIndirectLev() const {
    return indirectLev;
  }

  void SetIndirectLev(int8 level) {
    indirectLev = level;
  }

  OStIdx GetIndex() const {
    return index;
  }

  size_t GetVersionIndex(size_t version) const {
    ASSERT(version < versionsIndex.size(), "version out of range");
    return versionsIndex.at(version);
  }

  const MapleVector<size_t> &GetVersionsIndex() const {
    return versionsIndex;
  }
  void PushbackVersionIndex(size_t index) {
    versionsIndex.push_back(index);
  }

  size_t GetZeroVersionIndex() const {
    return zeroVersionIndex;
  }

  void SetZeroVersionIndex(size_t zeroVersionIndexParam) {
    zeroVersionIndex = zeroVersionIndexParam;
  }

  TyIdx GetTyIdx() const {
    return tyIdx;
  }

  void SetTyIdx(TyIdx tyIdxPara) {
    tyIdx = tyIdxPara;
  }

  FieldID GetFieldID() const {
    return fieldID;
  }

  void SetFieldID(FieldID fieldID) {
    this->fieldID = fieldID;
  }

  bool IsIgnoreRC() const {
    return ignoreRC;
  }

  bool IsAddressTaken() const {
    return addressTaken;
  }

  bool IsEPreLocalRefVar() const {
    return epreLocalRefVar;
  }

  void SetEPreLocalRefVar(bool epreLocalrefvarPara = true) {
    epreLocalRefVar = epreLocalrefvarPara;
  }

  PUIdx GetPuIdx() const {
    return puIdx;
  }

 private:
  enum OSTType {
    kUnkonwnOst,
    kSymbolOst,
    kPregOst
  };
  union SymOrPreg {
    PregIdx pregIdx;
    MIRSymbol *mirSt;
  };
  OriginalSt(OStIdx index, MapleAllocator &alloc, bool local, bool isFormal, FieldID fieldID, PUIdx pIdx,
      OSTType ostType, bool ignoreRC, SymOrPreg sysOrPreg)
      : ostType(ostType),
        index(index),
        versionsIndex(alloc.Adapter()),
        fieldID(fieldID),
        isLocal(local),
        isFormal(isFormal),
        ignoreRC(ignoreRC),
        symOrPreg(sysOrPreg),
        puIdx(pIdx) {}

  OSTType ostType;
  OStIdx index;                       // index number in originalStVector
  MapleVector<size_t> versionsIndex;  // the i-th element refers the index of versionst in versionst table
  size_t zeroVersionIndex = 0;            // same as versionsIndex[0]
  TyIdx tyIdx{ 0 };                        // type of this symbol at this level; 0 for unknown
  FieldID fieldID;                    // at each level of indirection
  int8 indirectLev = 0;                   // level of indirection; -1 for address, 0 for itself
  bool isLocal;                       // get from defined stmt or use expr
  bool isFormal;  // it's from the formal parameters so the type must be kSymbolOst or kPregOst after rename2preg
  bool addressTaken = false;
  bool isFinal = false;          // if the field has final attribute, only when fieldID != 0
  bool isPrivate = false;        // if the field has private attribute, only when fieldID != 0
  bool ignoreRC = false;         // base on MIRSymbol's IgnoreRC()
  bool epreLocalRefVar = false;  // is a localrefvar temp created by epre phase
  SymOrPreg symOrPreg;
  PUIdx puIdx;
};

class SymbolFieldPair {
 public:
  SymbolFieldPair(const StIdx &stIdx, FieldID fld) : stIdx(stIdx), fldID(fld) {}
  ~SymbolFieldPair() = default;
  bool operator==(const SymbolFieldPair& pairA) const {
    return (pairA.stIdx == stIdx) && (pairA.fldID == fldID);
  }

  const StIdx &GetStIdx() const {
    return stIdx;
  }

  FieldID GetFieldID() const {
    return fldID;
  }

 private:
  StIdx stIdx;
  FieldID fldID;
};

struct HashSymbolFieldPair {
  size_t operator()(const SymbolFieldPair& symbolFldID) const {
    return symbolFldID.GetStIdx().FullIdx();
  }
};

// This Table is for original symobols only. There is no SSA info attached and SSA is built based on this table.
class OriginalStTable {
 public:
  OriginalStTable(MemPool &memPool, MIRModule &mod);
  ~OriginalStTable() = default;

  OriginalSt *FindOrCreateSymbolOriginalSt(MIRSymbol &mirSt, PUIdx puIdx, FieldID fld);
  OriginalSt *FindOrCreatePregOriginalSt(PregIdx pregIdx, PUIdx puIdx);
  OriginalSt *CreateSymbolOriginalSt(MIRSymbol &mirSt, PUIdx pidx, FieldID fld);
  OriginalSt *CreatePregOriginalSt(PregIdx pregIdx, PUIdx puIdx);
  OriginalSt *FindSymbolOriginalSt(MIRSymbol &mirSt);
  const OriginalSt *GetOriginalStFromID(OStIdx id, bool checkFirst = false) const {
    if (checkFirst && id >= originalStVector.size()) {
      return nullptr;
    }
    ASSERT(id < originalStVector.size(), "symbol table index out of range");
    return originalStVector[id];
  }
  OriginalSt *GetOriginalStFromID(OStIdx id, bool checkFirst = false) {
    return const_cast<OriginalSt *>(const_cast<const OriginalStTable*>(this)->GetOriginalStFromID(id, checkFirst));
  }

  size_t Size() const {
    return originalStVector.size();
  }

  const MIRSymbol *GetMIRSymbolFromOriginalSt(const OriginalSt &ost) const {
    ASSERT(ost.IsRealSymbol(), "runtime check error");
    return ost.GetMIRSymbol();
  }
  MIRSymbol *GetMIRSymbolFromOriginalSt(OriginalSt &ost) {
    return const_cast<MIRSymbol *>(const_cast<const OriginalStTable*>(this)->GetMIRSymbolFromOriginalSt(ost));
  }

  const MIRSymbol *GetMIRSymbolFromID(OStIdx id) const {
    return GetMIRSymbolFromOriginalSt(*GetOriginalStFromID(id, false));
  }
  MIRSymbol *GetMIRSymbolFromID(OStIdx id) {
    return GetMIRSymbolFromOriginalSt(*GetOriginalStFromID(id, false));
  }

  MapleAllocator &GetAlloc() {
    return alloc;
  }

  MapleVector<OriginalSt*> &GetOriginalStVector() {
    return originalStVector;
  }

  void SetEPreLocalRefVar(const OStIdx &id, bool epreLocalrefvarPara = true) {
    ASSERT(id < originalStVector.size(), "symbol table index out of range");
    originalStVector[id]->SetEPreLocalRefVar(epreLocalrefvarPara);
  }

  void SetZeroVersionIndex(const OStIdx &id, size_t zeroVersionIndexParam) {
    ASSERT(id < originalStVector.size(), "symbol table index out of range");
    originalStVector[id]->SetZeroVersionIndex(zeroVersionIndexParam);
  }

  size_t GetVersionsIndexSize(const OStIdx &id) const {
    ASSERT(id < originalStVector.size(), "symbol table index out of range");
    return originalStVector[id]->GetVersionsIndex().size();
  }

  void UpdateVarOstMap(const OStIdx &id, std::map<OStIdx, OriginalSt*> &varOstMap) {
    ASSERT(id < originalStVector.size(), "symbol table index out of range");
    varOstMap[id] = originalStVector[id];
  }

  void Dump();

 private:
  MapleAllocator alloc;
  MIRModule &mirModule;
  MapleVector<OriginalSt*> originalStVector;  // the vector that map a OriginalSt's index to its pointer
  // mir symbol to original table, this only exists for no-original variables.
  MapleUnorderedMap<SymbolFieldPair, OStIdx, HashSymbolFieldPair> mirSt2Ost;
  MapleUnorderedMap<PregIdx, OStIdx> preg2Ost;
  // mir type to virtual variables in original table. this only exists for no-original variables.
  MapleMap<TyIdx, OStIdx> pType2Ost;
  // malloc info to virtual variables in original table. this only exists for no-original variables.
  MapleMap<std::pair<BaseNode*, uint32>, OStIdx> malloc2Ost;
  MapleMap<uint32, OStIdx> thisField2Ost;  // field of this_memory to virtual variables in original table.
  OStIdx virtuaLostUnkownMem{ 0 };
  OStIdx virtuaLostConstMem{ 0 };
};
}  // namespace maple

namespace std {
template <>
struct hash<maple::OStIdx> {
  size_t operator()(const maple::OStIdx &x) const {
    return static_cast<size_t>(x);
  }
};
}  // namespace std
#endif  // MAPLE_ME_INCLUDE_ORIG_SYMBOL_H
