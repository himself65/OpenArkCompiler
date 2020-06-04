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
#ifndef MAPLE_IR_INCLUDE_MIR_FUNCTION_H
#define MAPLE_IR_INCLUDE_MIR_FUNCTION_H
#include <string>
#include "mir_module.h"
#include "mir_const.h"
#include "mir_symbol.h"
#include "mir_preg.h"
#include "intrinsics.h"
#include "file_layout.h"
#include "mir_nodes.h"
#include "profile.h"


namespace maple {
// mapping src (java) variable to mpl variables to display debug info
struct MIRAliasVars {
  GStrIdx memPoolStrIdx;
  TyIdx tyIdx;
  GStrIdx sigStrIdx;
};

enum FuncAttrProp : uint32_t {
  kNoThrowException = 0x1,
  kNoRetNewlyAllocObj = 0x2,
  kNoDefEffect = 0x4,
  kNoDefArgEffect = 0x8,
  kPureFunc = 0x10,
  kIpaSeen = 0x20,
  kUseEffect = 0x40,
  kDefEffect = 0x80
};

class MeFunction;  // circular dependency exists, no other choice
class EAConnectionGraph;  // circular dependency exists, no other choice
class MIRFunction {
 public:
  MIRFunction(MIRModule *mod, StIdx idx)
      : module(mod),
        symbolTableIdx(idx) {}

  ~MIRFunction() = default;

  void Init();

  void Dump(bool withoutBody = false);
  void DumpUpFormal(int32 indent) const;
  void DumpFrame(int32 indent) const;
  void DumpFuncBody(int32 indent);
  const MIRSymbol *GetFuncSymbol() const;
  MIRSymbol *GetFuncSymbol();

  void SetBaseClassFuncNames(GStrIdx strIdx);
  void SetMemPool(MemPool *memPool) {
    SetCodeMemPool(memPool);
    codeMemPoolAllocator.SetMemPool(codeMemPool);
  }

  /// update signature_strIdx, basefunc_strIdx, baseclass_strIdx, basefunc_withtype_strIdx
  /// without considering baseclass_strIdx, basefunc_strIdx's original non-zero values
  /// \param strIdx full_name strIdx of the new function name
  void OverrideBaseClassFuncNames(GStrIdx strIdx);
  const std::string &GetName() const;

  GStrIdx GetNameStrIdx() const;

  const std::string &GetBaseClassName() const;

  const std::string &GetBaseFuncName() const;

  const std::string &GetBaseFuncNameWithType() const;


  const std::string &GetSignature() const;

  GStrIdx GetBaseClassNameStrIdx() const {
    return baseClassStrIdx;
  }

  GStrIdx GetBaseFuncNameStrIdx() const {
    return baseFuncStrIdx;
  }

  GStrIdx GetBaseFuncNameWithTypeStrIdx() const {
    return baseFuncWithTypeStrIdx;
  }

  GStrIdx GetBaseFuncSigStrIdx() const {
    return baseFuncSigStrIdx;
  }

  void SetBaseClassNameStrIdx(GStrIdx id) {
    baseClassStrIdx = id;
  }

  void SetBaseFuncNameStrIdx(GStrIdx id) {
    baseFuncStrIdx = id;
  }

  void SetBaseFuncNameWithTypeStrIdx(GStrIdx id) {
    baseFuncWithTypeStrIdx = id;
  }

  const MIRType *GetReturnType() const;
  MIRType *GetReturnType();
  bool IsReturnVoid() const {
    return GetReturnType()->GetPrimType() == PTY_void;
  }
  TyIdx GetReturnTyIdx() const {
    CHECK_FATAL(funcType != nullptr, "funcType is nullptr");
    return funcType->GetRetTyIdx();
  }
  void SetReturnTyIdx(TyIdx tyidx) {
    CHECK_FATAL(funcType != nullptr, "funcType is nullptr");
    funcType->SetRetTyIdx(tyidx);
  }

  const MIRType *GetClassType() const;
  TyIdx GetClassTyIdx() const {
    return classTyIdx;
  }
  void SetClassTyIdx(TyIdx tyIdx) {
    classTyIdx = tyIdx;
  }
  void SetClassTyIdx(uint32 idx) {
    classTyIdx.reset(idx);
  }

  size_t GetParamSize() const {
    CHECK_FATAL(funcType != nullptr, "funcType is nullptr");
    return funcType->GetParamTypeList().size();
  }
  const MapleVector<TyIdx> &GetParamTypes() const {
    CHECK_FATAL(funcType != nullptr, "funcType is nullptr");
    return funcType->GetParamTypeList();
  }
  TyIdx GetNthParamTyIdx(size_t i) const {
    ASSERT(i < funcType->GetParamTypeList().size(), "array index out of range");
    return funcType->GetParamTypeList()[i];
  }
  const MIRType *GetNthParamType(size_t i) const;
  MIRType *GetNthParamType(size_t i);
  const TypeAttrs &GetNthParamAttr(size_t i) const {
    CHECK_FATAL(funcType != nullptr, "funcType is nullptr");
    ASSERT(i < funcType->GetParamAttrsList().size(), "array index out of range");
    return funcType->GetNthParamAttrs(i);
  }

  void SetNthParamAttr(size_t i, TypeAttrs attrs) {
    CHECK_FATAL(funcType != nullptr, "funcType is nullptr");
    ASSERT(i < funcType->GetParamAttrsList().size(), "array index out of range");
    funcType->SetNthParamAttrs(i, attrs);
  }

  void SetNthParamAttrKind(size_t i, AttrKind x) const {
    CHECK_FATAL(funcType != nullptr, "funcType is nullptr");
    CHECK_FATAL(i < funcType->GetParamAttrsList().size(), "array index out of range");
    funcType->GetNthParamAttrs(i).SetAttr(x);
  }

  void AddArgument(MIRSymbol *symbol) {
    formals.push_back(symbol);
    CHECK_FATAL(funcType != nullptr, "funcType is nullptr");
    funcType->GetParamTypeList().push_back(symbol->GetTyIdx());
    funcType->GetParamAttrsList().push_back(symbol->GetAttrs());
  }

  LabelIdx GetOrCreateLableIdxFromName(const std::string &name);
  GStrIdx GetLabelStringIndex(LabelIdx labelIdx) const {
    CHECK_FATAL(labelTab != nullptr, "labelTab is nullptr");
    ASSERT(labelIdx < labelTab->Size(), "index out of range in GetLabelStringIndex");
    return labelTab->GetSymbolFromStIdx(labelIdx);
  }
  const std::string &GetLabelName(LabelIdx labelIdx) const {
    GStrIdx strIdx = GetLabelStringIndex(labelIdx);
    return GlobalTables::GetStrTable().GetStringFromStrIdx(strIdx);
  }

  const MIRSymbol *GetLocalOrGlobalSymbol(const StIdx &idx, bool checkFirst = false) const;
  MIRSymbol *GetLocalOrGlobalSymbol(const StIdx &idx, bool checkFirst = false);

  void SetAttrsFromSe(uint8 specialEffect);
  bool GetAttr(FuncAttrKind attrKind) const {
    return funcAttrs.GetAttr(attrKind);
  }
  void SetAttr(FuncAttrKind attrKind) {
    funcAttrs.SetAttr(attrKind);
  }
  void UnSetAttr(FuncAttrKind attrKind) {
    funcAttrs.SetAttr(attrKind, true);
  }

  bool IsVarargs() const {
    return funcAttrs.GetAttr(FUNCATTR_varargs);
  }

  bool IsWeak() const {
    return funcAttrs.GetAttr(FUNCATTR_weak);
  }

  bool IsStatic() const {
    return funcAttrs.GetAttr(FUNCATTR_static);
  }

  bool IsNative() const {
    return funcAttrs.GetAttr(FUNCATTR_native);
  }

  bool IsFinal() const {
    return funcAttrs.GetAttr(FUNCATTR_final);
  }

  bool IsAbstract() const {
    return funcAttrs.GetAttr(FUNCATTR_abstract);
  }

  bool IsPublic() const {
    return funcAttrs.GetAttr(FUNCATTR_public);
  }

  bool IsPrivate() const {
    return funcAttrs.GetAttr(FUNCATTR_private);
  }

  bool IsProtected() const {
    return funcAttrs.GetAttr(FUNCATTR_protected);
  }

  bool IsConstructor() const {
    return funcAttrs.GetAttr(FUNCATTR_constructor);
  }

  bool IsLocal() const {
    return funcAttrs.GetAttr(FUNCATTR_local);
  }

  bool IsNoDefArgEffect() const {
    return funcAttrs.GetAttr(FUNCATTR_nodefargeffect);
  }

  bool IsNoDefEffect() const {
    return funcAttrs.GetAttr(FUNCATTR_nodefeffect);
  }

  bool IsNoRetGlobal() const {
    return funcAttrs.GetAttr(FUNCATTR_noretglobal);
  }

  bool IsNoThrowException() const {
    return funcAttrs.GetAttr(FUNCATTR_nothrow_exception);
  }

  bool IsNoRetArg() const {
    return funcAttrs.GetAttr(FUNCATTR_noretarg);
  }

  bool IsNoPrivateDefEffect() const {
    return funcAttrs.GetAttr(FUNCATTR_noprivate_defeffect);
  }

  bool IsIpaSeen() const {
    return funcAttrs.GetAttr(FUNCATTR_ipaseen);
  }

  bool IsPure() const {
    return funcAttrs.GetAttr(FUNCATTR_pure);
  }

  void SetVarArgs() {
    funcAttrs.SetAttr(FUNCATTR_varargs);
  }

  void SetNoDefArgEffect() {
    funcAttrs.SetAttr(FUNCATTR_nodefargeffect);
  }

  void SetNoDefEffect() {
    funcAttrs.SetAttr(FUNCATTR_nodefeffect);
  }

  void SetNoRetGlobal() {
    funcAttrs.SetAttr(FUNCATTR_noretglobal);
  }

  void SetNoThrowException() {
    funcAttrs.SetAttr(FUNCATTR_nothrow_exception);
  }

  void SetNoRetArg() {
    funcAttrs.SetAttr(FUNCATTR_noretarg);
  }

  void SetNoPrivateDefEffect() {
    funcAttrs.SetAttr(FUNCATTR_noprivate_defeffect);
  }

  void SetIpaSeen() {
    funcAttrs.SetAttr(FUNCATTR_ipaseen);
  }

  void SetPure() {
    funcAttrs.SetAttr(FUNCATTR_pure);
  }

  void UnsetNoDefArgEffect() {
    funcAttrs.SetAttr(FUNCATTR_nodefargeffect, true);
  }

  void UnsetNoDefEffect() {
    funcAttrs.SetAttr(FUNCATTR_nodefeffect, true);
  }

  void UnsetNoRetGlobal() {
    funcAttrs.SetAttr(FUNCATTR_noretglobal, true);
  }

  void UnsetNoThrowException() {
    funcAttrs.SetAttr(FUNCATTR_nothrow_exception, true);
  }

  void UnsetPure() {
    funcAttrs.SetAttr(FUNCATTR_pure, true);
  }

  void UnsetNoRetArg() {
    funcAttrs.SetAttr(FUNCATTR_noretarg, true);
  }

  void UnsetNoPrivateDefEffect() {
    funcAttrs.SetAttr(FUNCATTR_noprivate_defeffect, true);
  }

  bool HasCall() const;
  void SetHasCall();

  bool IsReturnStruct() const;
  void SetReturnStruct();
  void SetReturnStruct(MIRType &retType);

  bool IsUserFunc() const;
  void SetUserFunc();

  bool IsInfoPrinted() const;
  void SetInfoPrinted();
  void ResetInfoPrinted();

  void SetNoReturn();
  bool NeverReturns() const;

  bool IsEmpty() const;
  bool IsClinit() const;
  uint32 GetInfo(GStrIdx strIdx) const;
  uint32 GetInfo(const std::string &str) const;
  bool IsAFormal(const MIRSymbol *symbol) const {
    for (const MIRSymbol *fSymbol : formals) {
      if (symbol == fSymbol) {
        return true;
      }
    }
    return false;
  }

  uint32 GetFormalIndex(const MIRSymbol *symbol) const {
    for (size_t i = 0; i < formals.size(); ++i)
      if (formals[i] == symbol) {
        return i;
      }
    return 0xffffffff;
  }

  // tell whether this function is a Java method
  bool IsJava() const {
    return classTyIdx != 0u;
  }

  const MIRType *GetNodeType(const BaseNode &node) const;

  void SetUpGDBEnv();
  void ResetGDBEnv();

  MemPool *GetCodeMempool() {
    if (codeMemPool == nullptr) {
      codeMemPool = memPoolCtrler.NewMemPool("func code mempool");
      codeMemPoolAllocator.SetMemPool(codeMemPool);
    }
    return codeMemPool;
  }

  MapleAllocator &GetCodeMemPoolAllocator() {
    GetCodeMempool();
    return codeMemPoolAllocator;
  }

  MapleAllocator &GetCodeMempoolAllocator() {
    if (codeMemPool == nullptr) {
      codeMemPool = memPoolCtrler.NewMemPool("func code mempool");
      codeMemPoolAllocator.SetMemPool(codeMemPool);
    }
    return codeMemPoolAllocator;
  }

  void NewBody();

  MIRModule *GetModule() {
    return module;
  }

  PUIdx GetPuidx() const {
    return puIdx;
  }
  void SetPuidx(PUIdx idx) {
    puIdx = idx;
  }

  PUIdx GetPuidxOrigin() const {
    return puIdxOrigin;
  }
  void SetPuidxOrigin(PUIdx idx) {
    puIdxOrigin = idx;
  }

  StIdx GetStIdx() const {
    return symbolTableIdx;
  }
  void SetStIdx(StIdx stIdx) {
    symbolTableIdx = stIdx;
  }


  MIRFuncType *GetMIRFuncType() {
    return funcType;
  }
  void SetMIRFuncType(MIRFuncType *type) {
    funcType = type;
  }

  TyIdx GetInferredReturnTyIdx() const {
    return inferredReturnTyIdx;
  }
  void SetInferredReturnTyIdx(TyIdx tyIdx) {
    inferredReturnTyIdx = tyIdx;
  }

  const MapleVector<TyIdx> &GetArgumentsTyIdx() const {
    return argumentsTyIdx;
  }
  void ClearArgumentsTyIdx() {
    argumentsTyIdx.clear();
  }
  TyIdx GetArgumentsTyIdxItem(size_t i) const {
    ASSERT(i < argumentsTyIdx.size(), "array index out of range");
    return argumentsTyIdx.at(i);
  }
  size_t GetArgumentsTyIdxSize() const {
    return argumentsTyIdx.size();
  }

  const MapleVector<TypeAttrs> &GetArgumentsAttrs() const {
    return argumentsAttrs;
  }
  void ClearArgumentsAttrs() {
    argumentsAttrs.clear();
  }
  bool HaveTypeNameTab() {
    return typeNameTab != nullptr;
  }
  const MapleMap<GStrIdx, TyIdx> &GetGStrIdxToTyIdxMap() const {
    CHECK_FATAL(typeNameTab != nullptr, "typeNameTab is nullptr");
    return typeNameTab->GetGStrIdxToTyIdxMap();
  }
  TyIdx GetTyIdxFromGStrIdx(GStrIdx idx) const {
    CHECK_FATAL(typeNameTab != nullptr, "typeNameTab is nullptr");
    return typeNameTab->GetTyIdxFromGStrIdx(idx);
  }
  void SetGStrIdxToTyIdx(GStrIdx gStrIdx, TyIdx tyIdx) {
    CHECK_FATAL(typeNameTab != nullptr, "typeNameTab is nullptr");
    typeNameTab->SetGStrIdxToTyIdx(gStrIdx, tyIdx);
  }

  const std::string &GetLabelTabItem(LabelIdx labelIdx) const {
    CHECK_FATAL(labelTab != nullptr, "labelTab is nullptr");
    return labelTab->GetName(labelIdx);
  }

  MIRPregTable *GetPregTab() {
    return pregTab;
  }
  const MIRPregTable *GetPregTab() const {
    return pregTab;
  }
  void SetPregTab(MIRPregTable *tab) {
    pregTab = tab;
  }
  MIRPreg *GetPregItem(PregIdx idx) {
    return const_cast<MIRPreg*>(const_cast<const MIRFunction*>(this)->GetPregItem(idx));
  }
  const MIRPreg *GetPregItem(PregIdx idx) const {
    return pregTab->PregFromPregIdx(idx);
  }

  BlockNode *GetBody() {
    return body;
  }
  const BlockNode *GetBody() const {
    return body;
  }
  void SetBody(BlockNode *node) {
    body = node;
  }

  SrcPosition &GetSrcPosition() {
    return srcPosition;
  }
  void SetSrcPosition(const SrcPosition &position) {
    srcPosition = position;
  }

  const FuncAttrs &GetFuncAttrs() const {
    return funcAttrs;
  }
  void SetFuncAttrs(const FuncAttrs &attrs) {
    funcAttrs = attrs;
  }
  void SetFuncAttrs(uint64 attrFlag) {
    funcAttrs.SetAttrFlag(attrFlag);
  }

  uint32 GetFlag() const {
    return flag;
  }
  void SetFlag(uint32 newFlag) {
    flag = newFlag;
  }

  uint16 GetHashCode() const {
    return hashCode;
  }
  void SetHashCode(uint16 newHashCode) {
    hashCode = newHashCode;
  }

  void SetFileIndex(uint32 newFileIndex) {
    fileIndex = newFileIndex;
  }

  const MIRInfoVector &GetInfoVector() const {
    return info;
  }
  void PushbackMIRInfo(const MIRInfoPair &pair) {
    info.push_back(pair);
  }
  void SetMIRInfoNum(size_t idx, uint32 num) {
    info[idx].second = num;
  }

  const MapleVector<bool> &InfoIsString() const {
    return infoIsString;
  }
  void PushbackIsString(bool isString) {
    infoIsString.push_back(isString);
  }

  bool NeedEmitAliasInfo() {
    return aliasVarMap != nullptr;
  }

  const MapleMap<GStrIdx, MIRAliasVars> &GetAliasVarMap() {
    if (aliasVarMap == nullptr) {
      aliasVarMap = module->GetMemPool()->New<MapleMap<GStrIdx, MIRAliasVars>>(module->GetMPAllocator().Adapter());
    }
    return *aliasVarMap;
  }
  void SetAliasVarMap(GStrIdx idx, MIRAliasVars &vars) {
    if (aliasVarMap == nullptr) {
      aliasVarMap = module->GetMemPool()->New<MapleMap<GStrIdx, MIRAliasVars>>(module->GetMPAllocator().Adapter());
    }
    (*aliasVarMap)[idx] = vars;
  }


  bool WithLocInfo() const {
    return withLocInfo;
  }
  void SetWithLocInfo(bool withInfo) {
    withLocInfo = withInfo;
  }


  uint8 GetLayoutType() const {
    return layoutType;
  }
  void SetLayoutType(uint8 type) {
    layoutType = type;
  }

  uint32 GetCallTimes() const {
    return callTimes;
  }
  void SetCallTimes(uint32 times) {
    callTimes = times;
  }

  uint16 GetFrameSize() const {
    return frameSize;
  }
  void SetFrameSize(uint16 size) {
    frameSize = size;
  }

  uint16 GetUpFormalSize() const {
    return upFormalSize;
  }
  void SetUpFormalSize(uint16 size) {
    upFormalSize = size;
  }

  uint16 GetModuleId() const {
    return moduleID;
  }
  void SetModuleID(uint16 id) {
    moduleID = id;
  }

  uint32 GetFuncSize() const {
    return funcSize;
  }
  void SetFuncSize(uint32 size) {
    funcSize = size;
  }

  uint32 GetTempCount() const {
    return tempCount;
  }
  void IncTempCount() {
    ++tempCount;
  }

  const uint8 *GetFormalWordsTypeTagged() const {
    return formalWordsTypeTagged;
  }
  void SetFormalWordsTypeTagged(uint8 *tagged) {
    formalWordsTypeTagged = tagged;
  }
  uint8 **GetFwtAddress() {
    return &formalWordsTypeTagged;
  }

  const uint8 *GetLocalWordsTypeTagged() const {
    return localWordsTypeTagged;
  }
  void SetLocalWordsTypeTagged(uint8 *tagged) {
    localWordsTypeTagged = tagged;
  }
  uint8 **GetLwtAddress() {
    return &localWordsTypeTagged;
  }

  const uint8 *GetFormalWordsRefCounted() const {
    return formalWordsRefCounted;
  }
  void SetFormalWordsRefCounted(uint8 *counted) {
    formalWordsRefCounted = counted;
  }
  uint8 **GetFwrAddress() {
    return &formalWordsRefCounted;
  }

  const uint8 *GetLocalWordsRefCounted() const {
    return localWordsRefCounted;
  }
  void SetLocalWordsRefCounted(uint8 *counted) {
    localWordsRefCounted = counted;
  }

  MeFunction *GetMeFunc() {
    return meFunc;
  }

  void SetMeFunc(MeFunction *func) {
    meFunc = func;
  }

  EAConnectionGraph *GetEACG() {
    return eacg;
  }
  void SetEACG(EAConnectionGraph *eacgVal) {
    eacg = eacgVal;
  }

  void SetFormals(const MapleVector<MIRSymbol*> &currFormals) {
    formals = currFormals;
  }
  MIRSymbol *GetFormal(size_t i) {
    return const_cast<MIRSymbol*>(const_cast<const MIRFunction*>(this)->GetFormal(i));
  }
  const MIRSymbol *GetFormal(size_t i) const {
    ASSERT(i < formals.size(), "array index out of range");
    return formals[i];
  }
  void SetFormal(size_t index, MIRSymbol *value) {
    ASSERT(index < formals.size(), "array index out of range");
    formals[index] = value;
  }
  size_t GetFormalCount() const {
    return formals.size();
  }
  void AddFormal(MIRSymbol *formal) {
    formals.push_back(formal);
  }
  void ClearFormals() {
    formals.clear();
  }

  size_t GetSymbolTabSize() const {
    ASSERT(symTab != nullptr, "symTab is nullptr");
    return symTab->GetSymbolTableSize();
  }
  MIRSymbol *GetSymbolTabItem(uint32 idx, bool checkFirst = false) const {
    return symTab->GetSymbolFromStIdx(idx, checkFirst);
  }
  const MIRSymbolTable *GetSymTab() const {
    return symTab;
  }
  MIRSymbolTable *GetSymTab() {
    return symTab;
  }

  const MIRLabelTable *GetLabelTab() const {
    CHECK_FATAL(labelTab != nullptr, "must be");
    return labelTab;
  }
  MIRLabelTable *GetLabelTab() {
    if (labelTab == nullptr) {
      labelTab = module->GetMemPool()->New<MIRLabelTable>(module->GetMPAllocator());
    }
    return labelTab;
  }
  void SetLabelTab(MIRLabelTable *currLabelTab) {
    labelTab = currLabelTab;
  }

  const MapleSet<MIRSymbol*> &GetRetRefSym() const {
    return retRefSym;
  }
  void InsertMIRSymbol(MIRSymbol *sym) {
    retRefSym.insert(sym);
  }

  MemPool *GetDataMemPool() const {
    return module->GetMemPool();
  }

  MemPool *GetCodeMemPool() {
    if (codeMemPool == nullptr) {
      codeMemPool = memPoolCtrler.NewMemPool("func code mempool");
      codeMemPoolAllocator.SetMemPool(codeMemPool);
    }
    return codeMemPool;
  }

  void SetCodeMemPool(MemPool *currCodeMemPool) {
    codeMemPool = currCodeMemPool;
  }

  MapleAllocator &GetCodeMPAllocator() {
    GetCodeMemPool();
    return codeMemPoolAllocator;
  }


  void AddProfileDesc(uint64 hash, uint32 start, uint32 end) {
    profileDesc = module->GetMemPool()->New<IRProfileDesc>(hash, start, end);
  }

  const IRProfileDesc *GetProfInf() {
    if (profileDesc == nullptr) {
      // return profileDesc with default value
      profileDesc = module->GetMemPool()->New<IRProfileDesc>();
    }
    return profileDesc;
  }

 private:
  MIRModule *module;     // the module that owns this function
  PUIdx puIdx = 0;           // the PU index of this function
  PUIdx puIdxOrigin = 0;     // the original puIdx when initial generation
  StIdx symbolTableIdx;  // the symbol table index of this function
  MIRFuncType *funcType = nullptr;
  TyIdx returnTyIdx{0};                // the declared return type of this function
  TyIdx inferredReturnTyIdx{0};        // the actual return type of of this function (may be a
                                    // subclass of the above). 0 means can not be inferred.
  TyIdx classTyIdx{0};                 // class/interface type this function belongs to
  MapleVector<MIRSymbol*> formals{module->GetMPAllocator().Adapter()};  // formal parameter symbols of this function
  MapleSet<MIRSymbol*> retRefSym{module->GetMPAllocator().Adapter()};
  MapleVector<TyIdx> argumentsTyIdx{module->GetMPAllocator().Adapter()};  // arguments types of this function
  MapleVector<TypeAttrs> argumentsAttrs{module->GetMPAllocator().Adapter()};

  MIRSymbolTable *symTab = nullptr;
  MIRTypeNameTable *typeNameTab = nullptr;
  MIRLabelTable *labelTab = nullptr;
  MIRPregTable *pregTab = nullptr;
  MemPool *codeMemPool = nullptr;
  MapleAllocator codeMemPoolAllocator{nullptr};
  uint32 callTimes = 0;
  BlockNode *body = nullptr;
  SrcPosition srcPosition{};
  FuncAttrs funcAttrs{};
  uint32 flag = 0;
  uint16 hashCode = 0;   // for methodmetadata order
  uint32 fileIndex = 0;  // this function belongs to which file, used by VM for plugin manager
  MIRInfoVector info{module->GetMPAllocator().Adapter()};
  MapleVector<bool> infoIsString{module->GetMPAllocator().Adapter()};  // tells if an entry has string value
  MapleMap<GStrIdx, MIRAliasVars> *aliasVarMap = nullptr;  // source code alias variables
                                                                                    // for debuginfo
  bool withLocInfo = true;

  uint8_t layoutType = kLayoutUnused;
  uint16 frameSize = 0;
  uint16 upFormalSize = 0;
  uint16 moduleID = 0;
  uint32 funcSize = 0;                         // size of code in words
  uint32 tempCount = 0;
  uint8 *formalWordsTypeTagged = nullptr;  // bit vector where the Nth bit tells whether
  // the Nth word in the formal parameters area
  // addressed upward from %%FP (that means
  // the word at location (%%FP + N*4)) has
  // typetag; if yes, the typetag is the word
  // at (%%FP + N*4 + 4); the bitvector's size
  // is given by BlockSize2BitvectorSize(upFormalSize)
  uint8 *localWordsTypeTagged = nullptr;  // bit vector where the Nth bit tells whether
  // the Nth word in the local stack frame
  // addressed downward from %%FP (that means
  // the word at location (%%FP - N*4)) has
  // typetag; if yes, the typetag is the word
  // at (%%FP - N*4 + 4); the bitvector's size
  // is given by BlockSize2BitvectorSize(frameSize)
  uint8 *formalWordsRefCounted = nullptr;  // bit vector where the Nth bit tells whether
  // the Nth word in the formal parameters area
  // addressed upward from %%FP (that means
  // the word at location (%%FP + N*4)) points to
  // a dynamic memory block that needs reference
  // count; the bitvector's size is given by
  // BlockSize2BitvectorSize(upFormalSize)
  uint8 *localWordsRefCounted = nullptr;  // bit vector where the Nth bit tells whether
  // the Nth word in the local stack frame
  // addressed downward from %%FP (that means
  // the word at location (%%FP - N*4)) points to
  // a dynamic memory block that needs reference
  // count; the bitvector's size is given by
  // BlockSize2BitvectorSize(frameSize)
  // uint16 numlabels; // removed. label table size
  // StmtNode **lbl2stmt; // lbl2stmt table, removed;
  // to hold unmangled class and function names
  MeFunction *meFunc = nullptr;
  EAConnectionGraph *eacg = nullptr;
  IRProfileDesc *profileDesc = nullptr;
  GStrIdx baseClassStrIdx{0};  // the string table index of base class name
  GStrIdx baseFuncStrIdx{0};   // the string table index of base function name
  // the string table index of base function name mangled with type info
  GStrIdx baseFuncWithTypeStrIdx{0};
  // funcname + types of args, no type of retv
  GStrIdx baseFuncSigStrIdx{0};
  GStrIdx signatureStrIdx{0};

  void DumpFlavorLoweredThanMmpl() const;
};
}  // namespace maple
#endif  // MAPLE_IR_INCLUDE_MIR_FUNCTION_H
