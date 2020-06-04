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
#include "lower.h"
#include <string>
#include <cinttypes>
#include <vector>
#include <map>
#include "mir_symbol.h"
#include "mir_function.h"
#include "cg_option.h"
#include "switch_lowerer.h"
#include "try_catch.h"
#include "intrinsic_op.h"
#include "mir_builder.h"
#include "opcode_info.h"
#include "aarch64_rt.h"
#include "securec.h"
#include "string_utils.h"

namespace maplebe {
namespace arrayNameForLower {
const std::set<std::string> kArrayKlassName{
#include "array_klass_name.def"
};

const std::set<std::string> kArrayBaseName{
#include "array_base_name.def"
};
}

using namespace maple;

#define JAVALANG (mirModule.IsJavaModule())

enum ExtFuncT : uint8 { kFmodDouble, kFmodFloat };

struct ExtFuncDescrT {
  ExtFuncT fid;
  const char *name;
  PrimType retType;
  PrimType argTypes[kMaxModFuncArgSize];
};

namespace {
std::pair<MIRIntrinsicID, const std::string> cgBuiltins[] = {
  { INTRN_JAVA_ARRAY_LENGTH, "MCC_JavaArrayLength" },
  { INTRN_JAVA_ARRAY_FILL, "MCC_JavaArrayFill" },
  { INTRN_JAVA_CHECK_CAST, "MCC_JavaCheckCast" },
  { INTRN_JAVA_INSTANCE_OF, "MCC_JavaInstanceOf" },
  { INTRN_JAVA_POLYMORPHIC_CALL, "MCC_JavaPolymorphicCall" },
  { INTRN_JAVA_CONST_CLASS, "MCC_GetReferenceToClass" },
  { INTRN_JAVA_GET_CLASS, "MCC_GetClass" },
  { INTRN_MPL_SET_CLASS, "MCC_SetJavaClass" },
  { INTRN_MPL_MEMSET_LOCALVAR, "memset_s" },
};

ExtFuncDescrT extFnDescrs[] = {
  { kFmodDouble, "fmod", PTY_f64, { PTY_f64, PTY_f64, kPtyInvalid } },
  { kFmodFloat, "fmodf", PTY_f32, { PTY_f32, PTY_f32, kPtyInvalid } },
};

std::vector<std::pair<ExtFuncT, PUIdx>> extFuncs;
}

const std::string CGLowerer::kIntrnRetValPrefix = "__iret";

MIRSymbol *CGLowerer::CreateNewRetVar(const MIRType &ty, const std::string &prefix) {
  const uint32 bufSize = 257;
  char buf[bufSize] = {'\0'};
  MIRFunction *func = GetCurrentFunc();
  MIRSymbol *var = func->GetSymTab()->CreateSymbol(kScopeLocal);
  int eNum = sprintf_s(buf, bufSize - 1, "%s%" PRId64, prefix.c_str(), ++seed);
  if (eNum == -1) {
    FATAL(kLncFatal, "sprintf_s failed");
  }
  std::string strBuf(buf);
  var->SetNameStrIdx(mirModule.GetMIRBuilder()->GetOrCreateStringIndex(strBuf));
  var->SetTyIdx(ty.GetTypeIndex());
  var->SetStorageClass(kScAuto);
  var->SetSKind(kStVar);
  func->GetSymTab()->AddToStringSymbolMap(*var);
  return var;
}

void CGLowerer::RegisterExternalLibraryFunctions() {
  for (uint32 i = 0; i < sizeof(extFnDescrs) / sizeof(extFnDescrs[0]); ++i) {
    ExtFuncT id = extFnDescrs[i].fid;
    CHECK_FATAL(id == i, "make sure id equal i");

    MIRFunction *func = mirModule.GetMIRBuilder()->GetOrCreateFunction(extFnDescrs[i].name,
                                                                       TyIdx(extFnDescrs[i].retType));
    MIRSymbol *funcSym = func->GetFuncSymbol();
    funcSym->SetStorageClass(kScExtern);
    /* return type */
    MIRType *retTy = GlobalTables::GetTypeTable().GetPrimType(extFnDescrs[i].retType);

    /* use void* for PTY_dynany */
    if (retTy->GetPrimType() == PTY_dynany) {
      retTy = GlobalTables::GetTypeTable().GetPtr();
    }
    func->SetReturnTyIdx(retTy->GetTypeIndex());

    for (uint32 j = 0; extFnDescrs[i].argTypes[j] != kPtyInvalid; ++j) {
      PrimType primTy = extFnDescrs[i].argTypes[j];
      MIRType *argTy = GlobalTables::GetTypeTable().GetPrimType(primTy);
      /* use void* for PTY_dynany */
      if (argTy->GetPrimType() == PTY_dynany) {
        argTy = GlobalTables::GetTypeTable().GetPtr();
      }
      MIRSymbol *argSt = func->GetSymTab()->CreateSymbol(kScopeLocal);
      const uint32 bufSize = 18;
      char buf[bufSize] = {'\0'};
      int eNum = sprintf_s(buf, bufSize - 1, "p%u", j);
      if (eNum == -1) {
        FATAL(kLncFatal, "sprintf_s failed");
      }
      std::string strBuf(buf);
      argSt->SetNameStrIdx(mirModule.GetMIRBuilder()->GetOrCreateStringIndex(strBuf));
      argSt->SetTyIdx(argTy->GetTypeIndex());
      argSt->SetStorageClass(kScFormal);
      argSt->SetSKind(kStVar);
      func->GetSymTab()->AddToStringSymbolMap(*argSt);
      func->AddArgument(argSt);
    }
    extFuncs.push_back(std::pair<ExtFuncT, PUIdx>(id, func->GetPuidx()));
  }
}

BaseNode *CGLowerer::NodeConvert(PrimType mType, BaseNode &expr) {
  PrimType srcType = expr.GetPrimType();
  if (GetPrimTypeSize(mType) == GetPrimTypeSize(srcType)) {
    return &expr;
  }
  TypeCvtNode *cvtNode = mirModule.CurFuncCodeMemPool()->New<TypeCvtNode>(OP_cvt);
  cvtNode->SetFromType(srcType);
  cvtNode->SetPrimType(mType);
  cvtNode->SetOpnd(&expr, 0);
  return cvtNode;
}

BaseNode *CGLowerer::LowerIaddrof(const IreadNode &iaddrof) {
  if (iaddrof.GetFieldID() == 0) {
    return iaddrof.Opnd(0);
  }
  MIRType *type = GlobalTables::GetTypeTable().GetTypeFromTyIdx(iaddrof.GetTyIdx());
  MIRPtrType *pointerTy = static_cast<MIRPtrType*>(type);
  CHECK_FATAL(pointerTy != nullptr, "LowerIaddrof: expect a pointer type at iaddrof node");
  MIRStructType *structTy = static_cast<MIRStructType*>(
      GlobalTables::GetTypeTable().GetTypeFromTyIdx(pointerTy->GetPointedTyIdx()));
  CHECK_FATAL(structTy != nullptr, "LowerIaddrof: non-zero fieldID for non-structure");
  int32 offset = beCommon.GetFieldOffset(*structTy, iaddrof.GetFieldID()).first;
  if (offset == 0) {
    return iaddrof.Opnd(0);
  }
  uint32 loweredPtrType = static_cast<uint32>(LOWERED_PTR_TYPE);
  MIRIntConst *offsetConst =
      GlobalTables::GetIntConstTable().GetOrCreateIntConst(
          offset, *GlobalTables::GetTypeTable().GetTypeTable().at(loweredPtrType));
  BaseNode *offsetNode = mirModule.CurFuncCodeMemPool()->New<ConstvalNode>(offsetConst);
  offsetNode->SetPrimType(LOWERED_PTR_TYPE);

  BinaryNode *addNode = mirModule.CurFuncCodeMemPool()->New<BinaryNode>(OP_add);
  addNode->SetPrimType(LOWERED_PTR_TYPE);
  addNode->SetBOpnd(iaddrof.Opnd(0), 0);
  addNode->SetBOpnd(offsetNode, 1);
  return addNode;
}

BaseNode *CGLowerer::LowerFarray(ArrayNode &array) {
  auto *farrayType = static_cast<MIRFarrayType*>(array.GetArrayType(GlobalTables::GetTypeTable()));
  size_t eSize = GlobalTables::GetTypeTable().GetTypeFromTyIdx(farrayType->GetElemTyIdx())->GetSize();
  if (farrayType->GetKind() == kTypeJArray) {
    if (farrayType->GetElemType()->GetKind() != kTypeScalar) {
      /* not the last dimension of primitive array */
      eSize = AArch64RTSupport::kRefFieldSize;
    }
  }

  MIRType &arrayType = *GlobalTables::GetTypeTable().GetTypeFromTyIdx(TyIdx(array.GetPrimType()));
  /* how about multi-dimension array? */
  if (array.GetIndex(0)->GetOpCode() == OP_constval) {
    const ConstvalNode *constvalNode = static_cast<const ConstvalNode*>(array.GetIndex(0));
    if (constvalNode->GetConstVal()->GetKind() == kConstInt) {
      const MIRIntConst *pIntConst = static_cast<const MIRIntConst*>(constvalNode->GetConstVal());
      CHECK_FATAL(JAVALANG || pIntConst->GetValue() >= 0, "Array index should >= 0.");
      int64 eleOffset = pIntConst->GetValue() * eSize;

      if (farrayType->GetKind() == kTypeJArray) {
        eleOffset += AArch64RTSupport::kArrayContentOffset;
      }

      BaseNode *baseNode = NodeConvert(array.GetPrimType(), *array.GetBase());
      if (eleOffset == 0) {
        return baseNode;
      }

      MIRIntConst *eleConst = GlobalTables::GetIntConstTable().GetOrCreateIntConst(eleOffset, arrayType);
      BaseNode *offsetNode = mirModule.CurFuncCodeMemPool()->New<ConstvalNode>(eleConst);
      offsetNode->SetPrimType(array.GetPrimType());

      BaseNode *rAdd = mirModule.CurFuncCodeMemPool()->New<BinaryNode>(OP_add);
      rAdd->SetPrimType(array.GetPrimType());
      rAdd->SetOpnd(baseNode, 0);
      rAdd->SetOpnd(offsetNode, 1);
      return rAdd;
    }
  }

  BaseNode *resNode = NodeConvert(array.GetPrimType(), *array.GetIndex(0));
  BaseNode *rMul = nullptr;

  if ((farrayType->GetKind() == kTypeJArray) && (resNode->GetOpCode() == OP_constval)) {
    ConstvalNode *idxNode = static_cast<ConstvalNode*>(resNode);
    int64 idx = safe_cast<MIRIntConst>(idxNode->GetConstVal())->GetValue();
    MIRIntConst *eConst = GlobalTables::GetIntConstTable().GetOrCreateIntConst(idx * eSize, arrayType);
    rMul = mirModule.CurFuncCodeMemPool()->New<ConstvalNode>(eConst);
    rMul->SetPrimType(array.GetPrimType());
  } else {
    MIRIntConst *eConst = GlobalTables::GetIntConstTable().GetOrCreateIntConst(eSize, arrayType);
    BaseNode *eSizeNode = mirModule.CurFuncCodeMemPool()->New<ConstvalNode>(eConst);
    eSizeNode->SetPrimType(array.GetPrimType());
    rMul = mirModule.CurFuncCodeMemPool()->New<BinaryNode>(OP_mul);
    rMul->SetPrimType(array.GetPrimType());
    rMul->SetOpnd(resNode, 0);
    rMul->SetOpnd(eSizeNode, 1);
  }

  BaseNode *baseNode = NodeConvert(array.GetPrimType(), *array.GetBase());

  if (farrayType->GetKind() == kTypeJArray) {
    BaseNode *jarrayBaseNode = mirModule.CurFuncCodeMemPool()->New<BinaryNode>(OP_add);
    MIRIntConst *arrayHeaderNode = GlobalTables::GetIntConstTable().GetOrCreateIntConst(
        static_cast<int64>(AArch64RTSupport::kArrayContentOffset), arrayType);
    BaseNode *arrayHeaderCstNode = mirModule.CurFuncCodeMemPool()->New<ConstvalNode>(arrayHeaderNode);
    arrayHeaderCstNode->SetPrimType(array.GetPrimType());
    jarrayBaseNode->SetPrimType(array.GetPrimType());
    jarrayBaseNode->SetOpnd(baseNode, 0);
    jarrayBaseNode->SetOpnd(arrayHeaderCstNode, 1);
    baseNode = jarrayBaseNode;
  }

  BaseNode *rAdd = mirModule.CurFuncCodeMemPool()->New<BinaryNode>(OP_add);
  rAdd->SetPrimType(array.GetPrimType());
  rAdd->SetOpnd(baseNode, 0);
  rAdd->SetOpnd(rMul, 1);
  return rAdd;
}

BaseNode *CGLowerer::LowerArrayDim(ArrayNode &array, int32 dim) {
  BaseNode *resNode = NodeConvert(array.GetPrimType(), *array.GetIndex(dim - 1));
  /* process left dimension index, resNode express the last dim, so dim need sub 2 */
  CHECK_FATAL(dim > (std::numeric_limits<int>::min)() + 1, "out of range");
  int leftDim = dim - 2;
  for (int i = leftDim; i >= 0; --i) {
    BaseNode *mpyNode = nullptr;
    BaseNode *item = NodeConvert(array.GetPrimType(), *array.GetDim(mirModule, GlobalTables::GetTypeTable(), dim - 1));
    for (int j = leftDim; j > i; --j) {
      BaseNode *mpyNodes = mirModule.CurFuncCodeMemPool()->New<BinaryNode>(OP_mul);
      mpyNodes->SetPrimType(array.GetPrimType());
      mpyNodes->SetOpnd(item, 0);
      mpyNodes->SetOpnd(NodeConvert(array.GetPrimType(), *array.GetDim(mirModule, GlobalTables::GetTypeTable(), j)), 1);
      item = mpyNodes;
    }
    mpyNode = mirModule.CurFuncCodeMemPool()->New<BinaryNode>(OP_mul);
    mpyNode->SetPrimType(array.GetPrimType());
    mpyNode->SetOpnd(NodeConvert(array.GetPrimType(), *array.GetIndex(i)), 0);
    mpyNode->SetOpnd(item, 1);

    BaseNode *newResNode = mirModule.CurFuncCodeMemPool()->New<BinaryNode>(OP_add);
    newResNode->SetPrimType(array.GetPrimType());
    newResNode->SetOpnd(resNode, 0);
    newResNode->SetOpnd(mpyNode, 1);
    resNode = newResNode;
  }
  return resNode;
}

BaseNode *CGLowerer::LowerArrayForLazyBiding(BaseNode &baseNode, BaseNode &offsetNode, const BaseNode &parent) {
  if (parent.GetOpCode() == OP_iread && (baseNode.GetOpCode() == maple::OP_addrof)) {
    const MIRSymbol *st =
        mirModule.CurFunction()->GetLocalOrGlobalSymbol(static_cast<AddrofNode&>(baseNode).GetStIdx());
    if (StringUtils::StartsWith(st->GetName(), NameMangler::kDecoupleStaticValueStr) ||
        ((StringUtils::StartsWith(st->GetName(), NameMangler::kMuidFuncUndefTabPrefixStr) ||
          StringUtils::StartsWith(st->GetName(), NameMangler::kMuidFuncDefTabPrefixStr) ||
          StringUtils::StartsWith(st->GetName(), NameMangler::kMuidDataDefTabPrefixStr) ||
          StringUtils::StartsWith(st->GetName(), NameMangler::kMuidDataUndefTabPrefixStr)) &&
         CGOptions::IsLazyBinding())) {
      /* for decouple static or lazybinding def/undef tables, replace it with intrinsic */
      MapleVector<BaseNode*> args(mirBuilder->GetCurrentFuncCodeMpAllocator()->Adapter());
      args.push_back(&baseNode);
      args.push_back(&offsetNode);
      return mirBuilder->CreateExprIntrinsicop(INTRN_MPL_READ_STATIC_OFFSET_TAB, OP_intrinsicop,
                                               *GlobalTables::GetTypeTable().GetPrimType(parent.GetPrimType()), args);
    }
  }
  return nullptr;
}

BaseNode *CGLowerer::LowerArray(ArrayNode &array, const BaseNode &parent) {
  MIRType *aType = array.GetArrayType(GlobalTables::GetTypeTable());
  if (aType->GetKind() == kTypeFArray || aType->GetKind() == kTypeJArray) {
    return LowerFarray(array);
  }
  MIRArrayType *arrayType = static_cast<MIRArrayType*>(aType);
  int32 dim = arrayType->GetDim();
  BaseNode *resNode = LowerArrayDim(array, dim);
  BaseNode *rMul = nullptr;
  size_t eSize = GlobalTables::GetTypeTable().GetTypeFromTyIdx(arrayType->GetElemTyIdx())->GetSize();
  Opcode opAdd = OP_add;
  MIRType &arrayTypes = *GlobalTables::GetTypeTable().GetTypeFromTyIdx(TyIdx(array.GetPrimType()));
  if (resNode->GetOpCode() == OP_constval) {
    /* index is a constant, we can calculate the offset now */
    ConstvalNode *idxNode = static_cast<ConstvalNode*>(resNode);
    int64 idx = safe_cast<MIRIntConst>(idxNode->GetConstVal())->GetValue();
    MIRIntConst *eConst = GlobalTables::GetIntConstTable().GetOrCreateIntConst(idx * eSize, arrayTypes);
    rMul = mirModule.CurFuncCodeMemPool()->New<ConstvalNode>(eConst);
    rMul->SetPrimType(array.GetPrimType());
  } else {
    MIRIntConst *eConst = GlobalTables::GetIntConstTable().GetOrCreateIntConst(eSize, arrayTypes);
    BaseNode *tmpNode = mirModule.CurFuncCodeMemPool()->New<ConstvalNode>(eConst);
    tmpNode->SetPrimType(array.GetPrimType());
    rMul = mirModule.CurFuncCodeMemPool()->New<BinaryNode>(OP_mul);
    rMul->SetPrimType(array.GetPrimType());
    rMul->SetOpnd(resNode, 0);
    rMul->SetOpnd(tmpNode, 1);
  }
  BaseNode *baseNode = NodeConvert(array.GetPrimType(), *array.GetBase());
  if (rMul->GetOpCode() == OP_constval) {
    BaseNode *intrnNode = LowerArrayForLazyBiding(*baseNode, *rMul, parent);
    if (intrnNode != nullptr) {
      return intrnNode;
    }
  }
  BaseNode *rAdd = mirModule.CurFuncCodeMemPool()->New<BinaryNode>(opAdd);
  rAdd->SetPrimType(array.GetPrimType());
  rAdd->SetOpnd(baseNode, 0);
  rAdd->SetOpnd(rMul, 1);
  return rAdd;
}

BaseNode *CGLowerer::LowerDreadBitfield(DreadNode &dread) {
  MIRSymbol *symbol = mirModule.CurFunction()->GetLocalOrGlobalSymbol(dread.GetStIdx());
  MIRStructType *structTy = static_cast<MIRStructType*>(symbol->GetType());
  CHECK_FATAL(structTy != nullptr, "LowerDreadBitfield: non-zero fieldID for non-structure");
  TyIdx fTyIdx = structTy->GetFieldTyIdx(dread.GetFieldID());
  CHECK_FATAL(fTyIdx != 0u, "LoweDreadBitField: field id out of range for the structure");
  MIRType *fType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(TyIdx(fTyIdx));
  if (fType->GetKind() != kTypeBitField) {
    return &dread;
  }
  uint8 fieldAlign = beCommon.GetTypeAlign(fTyIdx);
  std::pair<int32, int32> byteBitOffsets = beCommon.GetFieldOffset(*structTy, dread.GetFieldID());
  CHECK_FATAL((static_cast<uint32>(byteBitOffsets.first) % fieldAlign) == 0,
              "LowerDreadBitfield: bitfield offset not multiple of its alignment");

  AddrofNode *addrofNode = mirModule.CurFuncCodeMemPool()->New<AddrofNode>(OP_addrof);
  addrofNode->SetPrimType(LOWERED_PTR_TYPE);
  addrofNode->SetStIdx(dread.GetStIdx());

  ConstvalNode *constNode = mirModule.CurFuncCodeMemPool()->New<ConstvalNode>();
  constNode->SetPrimType(LOWERED_PTR_TYPE);
  uint32 loweredPtrType = static_cast<uint32>(LOWERED_PTR_TYPE);
  CHECK_FATAL(loweredPtrType < GlobalTables::GetTypeTable().GetTypeTable().size(),
              "LowerIassignBitField: subscript out of range");
  MIRType &type = *GlobalTables::GetTypeTable().GetTypeFromTyIdx(loweredPtrType);
  constNode->SetConstVal(GlobalTables::GetIntConstTable().GetOrCreateIntConst(byteBitOffsets.first, type));

  BinaryNode *addNode = mirModule.CurFuncCodeMemPool()->New<BinaryNode>(OP_add);
  addNode->SetPrimType(LOWERED_PTR_TYPE);
  addNode->SetBOpnd(addrofNode, 0);
  addNode->SetBOpnd(constNode, 1);

  IreadNode *ireadNode = mirModule.CurFuncCodeMemPool()->New<IreadNode>(OP_iread);
  ireadNode->SetPrimType(GetRegPrimType(fType->GetPrimType()));
  ireadNode->SetOpnd(addNode, 0);
  MIRType pointedType(kTypeScalar, fType->GetPrimType());
  TyIdx pointedTyIdx = GlobalTables::GetTypeTable().GetOrCreateMIRType(&pointedType);
  MIRPtrType pointType(pointedTyIdx);
  ireadNode->SetTyIdx(GlobalTables::GetTypeTable().GetOrCreateMIRType(&pointType));

  ExtractbitsNode *extrBitsNode = mirModule.CurFuncCodeMemPool()->New<ExtractbitsNode>(OP_extractbits);
  extrBitsNode->SetPrimType(GetRegPrimType(fType->GetPrimType()));
  extrBitsNode->SetBitsOffset(byteBitOffsets.second);
  extrBitsNode->SetBitsSize(static_cast<MIRBitFieldType*>(fType)->GetFieldSize());
  extrBitsNode->SetOpnd(ireadNode, 0);

  return extrBitsNode;
}

BaseNode *CGLowerer::LowerIreadBitfield(IreadNode &iread) {
  CHECK_FATAL(iread.GetTyIdx() < GlobalTables::GetTypeTable().GetTypeTable().size(),
              "LowerIassignBitField: subscript out of range");
  uint32 index = iread.GetTyIdx();
  MIRPtrType *pointerTy = static_cast<MIRPtrType*>(GlobalTables::GetTypeTable().GetTypeFromTyIdx(index));
  CHECK_FATAL(pointerTy != nullptr, "LowerIreadBitField: type in iread should be pointer type");
  MIRType *pointedTy = GlobalTables::GetTypeTable().GetTypeFromTyIdx(pointerTy->GetPointedTyIdx());
  /* Here pointed type can be Struct or JArray */
  MIRStructType *structTy = nullptr;
  if (pointedTy->GetKind() != kTypeJArray) {
    structTy = static_cast<MIRStructType*>(pointedTy);
  } else {
    /* it's a Jarray type. using it's parent's field info: java.lang.Object */
    structTy = static_cast<MIRJarrayType*>(pointedTy)->GetParentType();
  }
  CHECK_FATAL(structTy != nullptr, "LowerIreadBitField: type in iread does not point to a struct");
  TyIdx fTyIdx = structTy->GetFieldTyIdx(iread.GetFieldID());
  CHECK_FATAL(fTyIdx != 0u, "LowerIreadBitField: field id out of range for the structure");
  MIRType *fType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(TyIdx(fTyIdx));
  if (fType->GetKind() != kTypeBitField) {
    return &iread;
  }
  uint8 fieldAlign = beCommon.GetTypeAlign(fTyIdx);
  std::pair<int32, int32> byteBitOffsets = beCommon.GetFieldOffset(*structTy, iread.GetFieldID());
  CHECK_FATAL((static_cast<uint32>(byteBitOffsets.first) % fieldAlign) == 0,
              "LowerIreadBitfield: bitfield offset not multiple of its alignment");

  ConstvalNode *constNode = mirModule.CurFuncCodeMemPool()->New<ConstvalNode>();
  constNode->SetPrimType(LOWERED_PTR_TYPE);
  uint32 loweredPtrType = static_cast<uint32>(LOWERED_PTR_TYPE);
  MIRType &mirType = *GlobalTables::GetTypeTable().GetTypeFromTyIdx(loweredPtrType);
  constNode->SetConstVal(GlobalTables::GetIntConstTable().GetOrCreateIntConst(byteBitOffsets.first, mirType));

  BinaryNode *addNode = mirModule.CurFuncCodeMemPool()->New<BinaryNode>(OP_add);
  addNode->SetPrimType(LOWERED_PTR_TYPE);
  addNode->SetBOpnd(iread.Opnd(0), 0);
  addNode->SetBOpnd(constNode, 1);

  IreadNode *ireadNode = mirModule.CurFuncCodeMemPool()->New<IreadNode>(OP_iread);
  ireadNode->SetPrimType(GetRegPrimType(fType->GetPrimType()));
  ireadNode->SetOpnd(addNode, 0);
  MIRType pointedType(kTypeScalar, fType->GetPrimType());
  TyIdx pointedTyIdx = GlobalTables::GetTypeTable().GetOrCreateMIRType(&pointedType);
  MIRPtrType pointType(pointedTyIdx);
  ireadNode->SetTyIdx(GlobalTables::GetTypeTable().GetOrCreateMIRType(&pointType));

  ExtractbitsNode *extrBitsNode = mirModule.CurFuncCodeMemPool()->New<ExtractbitsNode>(OP_extractbits);
  extrBitsNode->SetPrimType(GetRegPrimType(fType->GetPrimType()));
  extrBitsNode->SetBitsOffset(byteBitOffsets.second);
  extrBitsNode->SetBitsSize(static_cast<MIRBitFieldType*>(fType)->GetFieldSize());
  extrBitsNode->SetOpnd(ireadNode, 0);

  return extrBitsNode;
}

void CGLowerer::LowerTypePtr(BaseNode &node) const {
  if ((node.GetPrimType() == PTY_ptr) || (node.GetPrimType() == PTY_ref)) {
    node.SetPrimType(LOWERED_PTR_TYPE);
  }

  if (kOpcodeInfo.IsTypeCvt(node.GetOpCode())) {
    auto &cvt = static_cast<TypeCvtNode&>(node);
    if ((cvt.FromType() == PTY_ptr) || (cvt.FromType() == PTY_ref)) {
      cvt.SetFromType(LOWERED_PTR_TYPE);
    }
  } else if (kOpcodeInfo.IsCompare(node.GetOpCode())) {
    auto &cmp = static_cast<CompareNode&>(node);
    if ((cmp.GetOpndType() == PTY_ptr) || (cmp.GetOpndType() == PTY_ref)) {
      cmp.SetOpndType(LOWERED_PTR_TYPE);
    }
  }
}


#if TARGARM32 || TARGAARCH64
BlockNode *CGLowerer::LowerReturnStruct(NaryStmtNode &retNode) {
  BlockNode *blk = mirModule.CurFuncCodeMemPool()->New<BlockNode>();
  for (size_t i = 0; i < retNode.GetNopndSize(); ++i) {
    retNode.SetOpnd(LowerExpr(retNode, *retNode.GetNopndAt(i), *blk), i);
  }
  BaseNode *opnd0 = retNode.Opnd(0);
  CHECK_FATAL(opnd0 != nullptr, "return struct should have a kid");
  CHECK_FATAL(opnd0->GetPrimType() == PTY_agg, "return struct should have a kid");

  MIRFunction *curFunc = GetCurrentFunc();
  MIRSymbol *retSt = curFunc->GetFormal(0);
  MIRPtrType *retTy = static_cast<MIRPtrType*>(retSt->GetType());
  IassignNode *iassign = mirModule.CurFuncCodeMemPool()->New<IassignNode>();
  iassign->SetTyIdx(retTy->GetTypeIndex());
  iassign->SetFieldID(0);
  iassign->SetRHS(opnd0);
  if (retSt->IsPreg()) {
    RegreadNode *regNode = mirModule.GetMIRBuilder()->CreateExprRegread(
        LOWERED_PTR_TYPE,
        curFunc->GetPregTab()->GetPregIdxFromPregno(retSt->GetPreg()->GetPregNo()));
    iassign->SetOpnd(regNode, 0);
  } else {
    AddrofNode *dreadNode = mirModule.CurFuncCodeMemPool()->New<AddrofNode>(OP_dread);
    dreadNode->SetPrimType(LOWERED_PTR_TYPE);
    dreadNode->SetStIdx(retSt->GetStIdx());
    iassign->SetOpnd(dreadNode, 0);
  }
  blk->AddStatement(iassign);
  retNode.GetNopnd().clear();
  retNode.SetNumOpnds(0);
  blk->AddStatement(&retNode);
  return blk;
}

#endif /* TARGARM32 || TARGAARCH64 */

BlockNode *CGLowerer::LowerReturn(NaryStmtNode &retNode) {
  BlockNode *blk = mirModule.CurFuncCodeMemPool()->New<BlockNode>();
  if (retNode.NumOpnds() != 0) {
    BaseNode *expr = retNode.Opnd(0);
    Opcode opr = expr->GetOpCode();
    if (opr == OP_dread) {
      AddrofNode *retExpr = static_cast<AddrofNode*>(expr);
      MIRFunction *mirFunc = mirModule.CurFunction();
      MIRSymbol *sym = mirFunc->GetLocalOrGlobalSymbol(retExpr->GetStIdx());
      if (sym->GetAttr(ATTR_localrefvar)) {
        mirFunc->InsertMIRSymbol(sym);
      }
    }
  }
  for (size_t i = 0; i < retNode.GetNopndSize(); ++i) {
    retNode.SetOpnd(LowerExpr(retNode, *retNode.GetNopndAt(i), *blk), i);
  }
  blk->AddStatement(&retNode);
  return blk;
}

StmtNode *CGLowerer::LowerDassignBitfield(DassignNode &dassign, BlockNode &newBlk) {
  dassign.SetRHS(LowerExpr(dassign, *dassign.GetRHS(), newBlk));
  MIRSymbol *symbol = mirModule.CurFunction()->GetLocalOrGlobalSymbol(dassign.GetStIdx());
  MIRStructType *structTy = static_cast<MIRStructType*>(symbol->GetType());
  CHECK_FATAL(structTy != nullptr, "LowerDassignBitfield: non-zero fieldID for non-structure");
  TyIdx fTyIdx = structTy->GetFieldTyIdx(dassign.GetFieldID());
  CHECK_FATAL(fTyIdx != 0u, "LowerDassignBitField: field id out of range for the structure");
  MIRType *fType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(TyIdx(fTyIdx));
  if (fType->GetKind() != kTypeBitField) {
    return &dassign;
  }
  uint8 fieldAlign = beCommon.GetTypeAlign(fTyIdx);
  std::pair<int32, int32> byteBitOffsets = beCommon.GetFieldOffset(*structTy, dassign.GetFieldID());
  CHECK_FATAL((static_cast<uint32>(byteBitOffsets.first) % fieldAlign) == 0,
              "LowerDassignBitfield: bitfield offset not multiple of its alignment");

  AddrofNode *addrofNode = mirModule.CurFuncCodeMemPool()->New<AddrofNode>(OP_addrof);
  addrofNode->SetPrimType(LOWERED_PTR_TYPE);
  addrofNode->SetStIdx(dassign.GetStIdx());

  ConstvalNode *constNode = mirModule.CurFuncCodeMemPool()->New<ConstvalNode>();
  constNode->SetPrimType(LOWERED_PTR_TYPE);
  uint32 loweredPtrType = static_cast<uint32>(LOWERED_PTR_TYPE);
  CHECK_FATAL(loweredPtrType < GlobalTables::GetTypeTable().GetTypeTable().size(),
              "LowerIassignBitField: subscript out of range");
  MIRType &mirType = *GlobalTables::GetTypeTable().GetTypeFromTyIdx(loweredPtrType);
  constNode->SetConstVal(GlobalTables::GetIntConstTable().GetOrCreateIntConst(byteBitOffsets.first, mirType));

  BinaryNode *addNode = mirModule.CurFuncCodeMemPool()->New<BinaryNode>(OP_add);
  addNode->SetPrimType(LOWERED_PTR_TYPE);
  addNode->SetBOpnd(addrofNode, 0);
  addNode->SetBOpnd(constNode, 1);

  IreadNode *ireadNode = mirModule.CurFuncCodeMemPool()->New<IreadNode>(OP_iread);
  ireadNode->SetPrimType(GetRegPrimType(fType->GetPrimType()));
  ireadNode->SetOpnd(addNode, 0);
  MIRType pointedType(kTypeScalar, fType->GetPrimType());
  TyIdx pointedTyIdx = GlobalTables::GetTypeTable().GetOrCreateMIRType(&pointedType);
  MIRPtrType pointType(pointedTyIdx);
  ireadNode->SetTyIdx(GlobalTables::GetTypeTable().GetOrCreateMIRType(&pointType));

  DepositbitsNode *depositBits = mirModule.CurFuncCodeMemPool()->New<DepositbitsNode>();
  depositBits->SetPrimType(GetRegPrimType(fType->GetPrimType()));
  depositBits->SetBitsOffset(byteBitOffsets.second);
  depositBits->SetBitsSize(static_cast<MIRBitFieldType*>(fType)->GetFieldSize());
  depositBits->SetBOpnd(ireadNode, 0);
  depositBits->SetBOpnd(dassign.GetRHS(), 1);

  IassignNode *iassignStmt = mirModule.CurFuncCodeMemPool()->New<IassignNode>();
  iassignStmt->SetTyIdx(GlobalTables::GetTypeTable().GetOrCreateMIRType(&pointType));
  iassignStmt->SetOpnd(addNode->CloneTree(mirModule.GetCurFuncCodeMPAllocator()), 0);
  iassignStmt->SetRHS(depositBits);

  return iassignStmt;
}

StmtNode *CGLowerer::LowerIassignBitfield(IassignNode &iassign, BlockNode &newBlk) {
  ASSERT(iassign.Opnd(0) != nullptr, "iassign.Opnd(0) should not be nullptr");
  iassign.SetOpnd(LowerExpr(iassign, *iassign.Opnd(0), newBlk), 0);
  iassign.SetRHS(LowerExpr(iassign, *iassign.GetRHS(), newBlk));

  CHECK_FATAL(iassign.GetTyIdx() < GlobalTables::GetTypeTable().GetTypeTable().size(),
              "LowerIassignBitField: subscript out of range");
  uint32 index = iassign.GetTyIdx();
  MIRPtrType *pointerTy = static_cast<MIRPtrType*>(GlobalTables::GetTypeTable().GetTypeFromTyIdx(index));
  CHECK_FATAL(pointerTy != nullptr, "LowerIassignBitField: type in iassign should be pointer type");
  MIRType *pointedTy = GlobalTables::GetTypeTable().GetTypeFromTyIdx(pointerTy->GetPointedTyIdx());
  /*
   * Here pointed type can be Struct or JArray
   * We should seriously consider make JArray also a Struct type
   */
  MIRStructType *structTy = nullptr;
  if (pointedTy->GetKind() != kTypeJArray) {
    structTy = static_cast<MIRStructType*>(pointedTy);
  } else {
    /* it's a Jarray type. using it's parent's field info: java.lang.Object */
    structTy = static_cast<MIRJarrayType*>(pointedTy)->GetParentType();
  }

  CHECK_FATAL(structTy != nullptr, "LowerIassignBitField: type in iassign does not point to a struct");
  TyIdx fTyIdx = structTy->GetFieldTyIdx(iassign.GetFieldID());
  CHECK_FATAL(fTyIdx != 0u, "LowerIassignBitField: field id out of range for the structure");
  MIRType *fType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(TyIdx(fTyIdx));
  if (fType->GetKind() != kTypeBitField) {
    return &iassign;
  }
  uint8 fieldAlign = beCommon.GetTypeAlign(fTyIdx);
  std::pair<int32, int32> byteBitOffsets = beCommon.GetFieldOffset(*structTy, iassign.GetFieldID());
  CHECK_FATAL((static_cast<uint32>(byteBitOffsets.first) % fieldAlign) == 0,
              "LowerIassignBitfield: bitfield offset not multiple of its alignment");

  ConstvalNode *constNode = mirModule.CurFuncCodeMemPool()->New<ConstvalNode>();
  constNode->SetPrimType(LOWERED_PTR_TYPE);
  uint32 loweredPtrType = static_cast<uint32>(LOWERED_PTR_TYPE);
  MIRType &mirType = *GlobalTables::GetTypeTable().GetTypeFromTyIdx(loweredPtrType);
  constNode->SetConstVal(GlobalTables::GetIntConstTable().GetOrCreateIntConst(byteBitOffsets.first, mirType));

  BinaryNode *addNode = mirModule.CurFuncCodeMemPool()->New<BinaryNode>(OP_add);
  addNode->SetPrimType(LOWERED_PTR_TYPE);
  addNode->SetBOpnd(iassign.Opnd(0), 0);
  addNode->SetBOpnd(constNode, 1);

  IreadNode *ireadNode = mirModule.CurFuncCodeMemPool()->New<IreadNode>(OP_iread);
  ireadNode->SetPrimType(GetRegPrimType(fType->GetPrimType()));
  ireadNode->SetOpnd(addNode, 0);
  MIRType pointedType(kTypeScalar, fType->GetPrimType());
  TyIdx pointedTyIdx = GlobalTables::GetTypeTable().GetOrCreateMIRType(&pointedType);
  MIRPtrType pointType(pointedTyIdx);
  ireadNode->SetTyIdx(GlobalTables::GetTypeTable().GetOrCreateMIRType(&pointType));

  DepositbitsNode *depositBits = mirModule.CurFuncCodeMemPool()->New<DepositbitsNode>();
  depositBits->SetPrimType(GetRegPrimType(fType->GetPrimType()));
  depositBits->SetBitsOffset(byteBitOffsets.second);
  depositBits->SetBitsOffset(static_cast<MIRBitFieldType*>(fType)->GetFieldSize());
  depositBits->SetBOpnd(ireadNode, 0);
  depositBits->SetBOpnd(iassign.GetRHS(), 1);

  IassignNode *iassignStmt = mirModule.CurFuncCodeMemPool()->New<IassignNode>();
  iassignStmt->SetTyIdx(GlobalTables::GetTypeTable().GetOrCreateMIRType(&pointType));
  iassignStmt->SetOpnd(addNode->CloneTree(mirModule.GetCurFuncCodeMPAllocator()), 0);
  iassignStmt->SetRHS(depositBits);

  return iassignStmt;
}

void CGLowerer::LowerIassign(IassignNode &iassign, BlockNode &newBlk) {
  StmtNode *newStmt = nullptr;
  if (iassign.GetFieldID() != 0) {
    newStmt = LowerIassignBitfield(iassign, newBlk);
  } else {
    CHECK_FATAL(iassign.GetPrimType() != PTY_ptr, "should have been lowered already");
    CHECK_FATAL(iassign.GetPrimType() != PTY_ref, "should have been lowered already");
    LowerStmt(iassign, newBlk);
    newStmt = &iassign;
  }
  newBlk.AddStatement(newStmt);
}

DassignNode *CGLowerer::SaveReturnValueInLocal(StIdx stIdx, uint16 fieldID) {
  MIRSymbol *var = mirModule.CurFunction()->GetSymTab()->GetSymbolFromStIdx(stIdx.Idx());
  CHECK_FATAL(var != nullptr, "var should not be nullptr");
  RegreadNode *regRead = mirModule.GetMIRBuilder()->CreateExprRegread(
      GlobalTables::GetTypeTable().GetTypeTable().at(var->GetTyIdx())->GetPrimType(), -kSregRetval0);
  return mirModule.GetMIRBuilder()->CreateStmtDassign(*var, fieldID, regRead);
}

BaseNode *CGLowerer::LowerRem(BaseNode &expr, BlockNode &blk) {
  auto &remExpr = static_cast<BinaryNode&>(expr);
  if (!IsPrimitiveFloat(remExpr.GetPrimType())) {
    return &expr;
  }
  ExtFuncT fmodFunc = remExpr.GetPrimType() == PTY_f32 ? kFmodFloat : kFmodDouble;
  uint32 i = 0;
  for (; i < extFuncs.size(); ++i) {
    if (extFuncs[i].first == fmodFunc) {
      break;
    }
  }
  CHECK_FATAL(i < extFuncs.size(), "rem expression primtype is not PTY_f32 nor PTY_f64.");
  MIRSymbol *ret = CreateNewRetVar(*GlobalTables::GetTypeTable().GetPrimType(remExpr.GetPrimType()),
                                   kIntrnRetValPrefix);
  MapleVector<BaseNode*> args(mirModule.GetMIRBuilder()->GetCurrentFuncCodeMpAllocator()->Adapter());
  args.push_back(remExpr.Opnd(0));
  args.push_back(remExpr.Opnd(1));
  CallNode *callStmt = mirModule.GetMIRBuilder()->CreateStmtCallAssigned(extFuncs[i].second, args, ret);
  blk.AppendStatementsFromBlock(*LowerCallAssignedStmt(*callStmt));
  MIRType *type = GlobalTables::GetTypeTable().GetPrimType(extFnDescrs[fmodFunc].retType);
  return mirModule.GetMIRBuilder()->CreateExprDread(*type, 0, *ret);
}

/* to lower call (including icall) and intrinsicall statements */
void CGLowerer::LowerCallStmt(StmtNode &stmt, StmtNode *&nextStmt, BlockNode &newBlk) {
  StmtNode *newStmt = nullptr;
  if (stmt.GetOpCode() == OP_intrinsiccall) {
    auto &intrnNode = static_cast<IntrinsiccallNode&>(stmt);
    newStmt = LowerIntrinsiccall(intrnNode, newBlk);
  } else {
    /* We note the function has a user-defined (i.e., not an intrinsic) call. */
    GetCurrentFunc()->SetHasCall();
    newStmt = &stmt;
  }

  if (newStmt == nullptr) {
    return;
  }

  if ((newStmt->GetOpCode() == OP_call) || (newStmt->GetOpCode() == OP_icall)) {
    newStmt = LowerCall(static_cast<CallNode&>(*newStmt), nextStmt, newBlk);
  }
  newStmt->SetSrcPos(stmt.GetSrcPos());
  newBlk.AddStatement(newStmt);
}

StmtNode *CGLowerer::GenCallNode(const StmtNode &stmt, PUIdx &funcCalled, CallNode& origCall) {
  StmtNode *newCall = nullptr;
  if (stmt.GetOpCode() == OP_callassigned) {
    newCall = mirModule.GetMIRBuilder()->CreateStmtCall(origCall.GetPUIdx(), origCall.GetNopnd());
  } else if (stmt.GetOpCode() == OP_virtualcallassigned) {
    newCall = mirModule.GetMIRBuilder()->CreateStmtVirtualCall(origCall.GetPUIdx(), origCall.GetNopnd());
  } else if (stmt.GetOpCode() == OP_superclasscallassigned) {
    newCall = mirModule.GetMIRBuilder()->CreateStmtSuperclassCall(origCall.GetPUIdx(), origCall.GetNopnd());
  } else if (stmt.GetOpCode() == OP_interfacecallassigned) {
    newCall = mirModule.GetMIRBuilder()->CreateStmtInterfaceCall(origCall.GetPUIdx(), origCall.GetNopnd());
  }
  CHECK_FATAL(newCall != nullptr, "nullptr is not expected");
  funcCalled = origCall.GetPUIdx();
  CHECK_FATAL((newCall->GetOpCode() == OP_call || newCall->GetOpCode() == OP_interfacecall),
              "virtual call or super class call are not expected");
  if (newCall->GetOpCode() == OP_interfacecall) {
    std::cerr << "interfacecall found\n";
  }
  return newCall;
}

StmtNode *CGLowerer::GenIntrinsiccallNode(const StmtNode &stmt, PUIdx &funcCalled, bool &handledAtLowerLevel,
                                          IntrinsiccallNode &origCall) {
  StmtNode *newCall = nullptr;
  handledAtLowerLevel = IsIntrinsicCallHandledAtLowerLevel(origCall.GetIntrinsic());
  if (handledAtLowerLevel) {
    /* If the lower level can handle the intrinsic, just let it pass through. */
    newCall = &origCall;
  } else {
    PUIdx bFunc = GetBuiltinToUse(origCall.GetIntrinsic());
    if (bFunc != kFuncNotFound) {
      newCall = mirModule.GetMIRBuilder()->CreateStmtCall(bFunc, origCall.GetNopnd());
    } else {
      if (stmt.GetOpCode() == OP_intrinsiccallassigned) {
        newCall = mirModule.GetMIRBuilder()->CreateStmtIntrinsicCall(origCall.GetIntrinsic(),
                                                                      origCall.GetNopnd());
      } else if (stmt.GetOpCode() == OP_xintrinsiccallassigned) {
        newCall = mirModule.GetMIRBuilder()->CreateStmtXintrinsicCall(origCall.GetIntrinsic(),
                                                                      origCall.GetNopnd());
      } else {
        newCall = mirModule.GetMIRBuilder()->CreateStmtIntrinsicCall(origCall.GetIntrinsic(), origCall.GetNopnd(),
                                                                     origCall.GetTyIdx());
      }
    }
    funcCalled = bFunc;
    CHECK_FATAL((newCall->GetOpCode() == OP_call || newCall->GetOpCode() == OP_intrinsiccall),
                "xintrinsic and intrinsiccallwithtype call is not expected");
  }
  return newCall;
}

StmtNode *CGLowerer::GenIcallNode(PUIdx &funcCalled, IcallNode &origCall) {
  StmtNode *newCall = nullptr;
  newCall = mirModule.GetMIRBuilder()->CreateStmtIcall(origCall.GetNopnd());
  CHECK_FATAL(newCall != nullptr, "nullptr is not expected");
  funcCalled = kFuncNotFound;
  return newCall;
}

BlockNode *CGLowerer::GenBlockNode(StmtNode &newCall, const CallReturnVector &p2nRets, const Opcode &opcode,
                                   const PUIdx &funcCalled, bool handledAtLowerLevel) {
  BlockNode *blk = mirModule.CurFuncCodeMemPool()->New<BlockNode>();
  blk->AddStatement(&newCall);
  if (!handledAtLowerLevel) {
    CHECK_FATAL(p2nRets.size() <= 1, "make sure p2nRets size <= 1");
    /* Create DassignStmt to save kSregRetval0. */
    StmtNode *dStmt = nullptr;
    if (p2nRets.size() == 1) {
      StIdx stIdx = p2nRets[0].first;
      RegFieldPair regFieldPair = p2nRets[0].second;
      if (!regFieldPair.IsReg()) {
        uint16 fieldID = static_cast<uint16>(regFieldPair.GetFieldID());
        DassignNode *dn = SaveReturnValueInLocal(stIdx, fieldID);
        CHECK_FATAL(dn->GetFieldID() == 0, "make sure dn's fieldID return 0");
        LowerDassign(*dn, *blk);
        CHECK_FATAL(&newCall == blk->GetLast() || newCall.GetNext() == blk->GetLast(), "");
        dStmt = (&newCall == blk->GetLast()) ? nullptr : blk->GetLast();
        CHECK_FATAL(newCall.GetNext() == dStmt, "make sure newCall's next equal dStmt");
      } else {
        PregIdx pregIdx = static_cast<PregIdx>(regFieldPair.GetPregIdx());
        MIRPreg *mirPreg = GetCurrentFunc()->GetPregTab()->PregFromPregIdx(pregIdx);
        RegreadNode *regNode = mirModule.GetMIRBuilder()->CreateExprRegread(mirPreg->GetPrimType(), -kSregRetval0);
        RegassignNode *regAssign =
            mirModule.GetMIRBuilder()->CreateStmtRegassign(mirPreg->GetPrimType(), regFieldPair.GetPregIdx(), regNode);
        blk->AddStatement(regAssign);
        dStmt = regAssign;
      }
    }
    blk->ResetBlock();
    /* if VerboseAsm, insert a comment */
    if (ShouldAddAdditionalComment()) {
      CommentNode *cmnt = mirModule.CurFuncCodeMemPool()->New<CommentNode>(mirModule);
      cmnt->SetComment(kOpcodeInfo.GetName(opcode));
      if (funcCalled == kFuncNotFound) {
        cmnt->Append(" : unknown");
      } else {
        cmnt->Append(" : ");
        cmnt->Append(GlobalTables::GetFunctionTable().GetFunctionFromPuidx(funcCalled)->GetName());
      }
      blk->AddStatement(cmnt);
    }
    CHECK_FATAL(dStmt == nullptr || dStmt->GetNext() == nullptr, "make sure dStmt or dStmt's next is nullptr");
    LowerCallStmt(newCall, dStmt, *blk);
    if (dStmt != nullptr) {
      dStmt->SetSrcPos(newCall.GetSrcPos());
      blk->AddStatement(dStmt);
    }
  }
  return blk;
}

BlockNode *CGLowerer::LowerCallAssignedStmt(StmtNode &stmt) {
  StmtNode *newCall = nullptr;
  CallReturnVector *p2nRets = nullptr;
  PUIdx funcCalled = kFuncNotFound;
  bool handledAtLowerLevel = false;
  switch (stmt.GetOpCode()) {
    case OP_callassigned:
    case OP_virtualcallassigned:
    case OP_superclasscallassigned:
    case OP_interfacecallassigned: {
      auto &origCall = static_cast<CallNode&>(stmt);
      newCall = GenCallNode(stmt, funcCalled, origCall);
      p2nRets = &origCall.GetReturnVec();
      break;
    }
    case OP_intrinsiccallassigned:
    case OP_xintrinsiccallassigned: {
      auto &origCall = static_cast<IntrinsiccallNode&>(stmt);
      newCall = GenIntrinsiccallNode(stmt, funcCalled, handledAtLowerLevel, origCall);
      p2nRets = &origCall.GetReturnVec();
      break;
    }
    case OP_intrinsiccallwithtypeassigned: {
      auto &origCall = static_cast<IntrinsiccallNode&>(stmt);
      newCall = GenIntrinsiccallNode(stmt, funcCalled, handledAtLowerLevel, origCall);
      p2nRets = &origCall.GetReturnVec();
      break;
    }
    case OP_icallassigned: {
      auto &origCall = static_cast<IcallNode&>(stmt);
      newCall = GenIcallNode(funcCalled, origCall);
      p2nRets = &origCall.GetReturnVec();
      break;
    }
    default:
      CHECK_FATAL(false, "NIY");
      return nullptr;
  }

  /* transfer srcPosition location info */
  newCall->SetSrcPos(stmt.GetSrcPos());
  return GenBlockNode(*newCall, *p2nRets, stmt.GetOpCode(), funcCalled, handledAtLowerLevel);
}

void CGLowerer::LowerStmt(StmtNode &stmt, BlockNode &newBlk) {
  CHECK_FATAL(stmt.GetPrimType() != PTY_ptr, "should have been lowered already");
  CHECK_FATAL(stmt.GetPrimType() != PTY_ref, "should have been lowered already");
  for (size_t i = 0; i < stmt.NumOpnds(); ++i) {
    stmt.SetOpnd(LowerExpr(stmt, *stmt.Opnd(i), newBlk), i);
  }
}

BlockNode *CGLowerer::LowerBlock(BlockNode &block) {
  BlockNode *newBlk = mirModule.CurFuncCodeMemPool()->New<BlockNode>();
  BlockNode *tmpBlockNode = nullptr;
  if (block.GetFirst() == nullptr) {
    return newBlk;
  }

  StmtNode *nextStmt = block.GetFirst();
  do {
    StmtNode *stmt = nextStmt;
    nextStmt = stmt->GetNext();
    stmt->SetNext(nullptr);
    currentBlock = newBlk;

    LowerTypePtr(*stmt);

    switch (stmt->GetOpCode()) {
      case OP_switch: {
        LowerStmt(*stmt, *newBlk);
        MemPool *switchMp = memPoolCtrler.NewMemPool("switchlowerer");
        MapleAllocator switchAllocator(switchMp);
        SwitchLowerer switchLowerer(mirModule, static_cast<SwitchNode&>(*stmt), switchAllocator);
        BlockNode *blk = switchLowerer.LowerSwitch();
        if (blk->GetFirst() != nullptr) {
          newBlk->AppendStatementsFromBlock(*blk);
        }
        memPoolCtrler.DeleteMemPool(switchMp);
        needBranchCleanup = true;
        break;
      }
      case OP_block:
        tmpBlockNode = LowerBlock(static_cast<BlockNode&>(*stmt));
        CHECK_FATAL(tmpBlockNode != nullptr, "nullptr is not expected");
        newBlk->AppendStatementsFromBlock(*tmpBlockNode);
        break;
      case OP_dassign: {
        LowerDassign(static_cast<DassignNode&>(*stmt), *newBlk);
        break;
      }
      case OP_regassign: {
        LowerRegassign(static_cast<RegassignNode&>(*stmt), *newBlk);
        break;
      }
      case OP_iassign: {
        LowerIassign(static_cast<IassignNode&>(*stmt), *newBlk);
        break;
      }
      case OP_callassigned:
      case OP_virtualcallassigned:
      case OP_superclasscallassigned:
      case OP_interfacecallassigned:
      case OP_icallassigned:
      case OP_intrinsiccallassigned:
      case OP_xintrinsiccallassigned:
      case OP_intrinsiccallwithtypeassigned:
        newBlk->AppendStatementsFromBlock(*LowerCallAssignedStmt(*stmt));
        break;
      case OP_intrinsiccall:
      case OP_call:
      case OP_icall:
#if TARGARM32 || TARGAARCH64
        LowerCallStmt(*stmt, nextStmt, *newBlk);
#else
        LowerStmt(*stmt, *newBlk);
#endif
        break;
      case OP_return: {
#if TARGARM32 || TARGAARCH64
        if (GetCurrentFunc()->IsReturnStruct()) {
          newBlk->AppendStatementsFromBlock(*LowerReturnStruct(static_cast<NaryStmtNode&>(*stmt)));
        } else {
#endif
        NaryStmtNode *retNode = static_cast<NaryStmtNode*>(stmt);
        if (retNode->GetNopndSize() == 0) {
          newBlk->AddStatement(stmt);
        } else {
          tmpBlockNode = LowerReturn(*retNode);
          CHECK_FATAL(tmpBlockNode != nullptr, "nullptr is not expected");
          newBlk->AppendStatementsFromBlock(*tmpBlockNode);
        }
#if TARGARM32 || TARGAARCH64
        }
#endif
        break;
      }
      case OP_comment:
        newBlk->AddStatement(stmt);
        break;
      case OP_try:
        LowerStmt(*stmt, *newBlk);
        newBlk->AddStatement(stmt);
        hasTry = true;
        break;
      case OP_endtry:
        LowerStmt(*stmt, *newBlk);
        newBlk->AddStatement(stmt);
        break;
      case OP_catch:
        LowerStmt(*stmt, *newBlk);
        newBlk->AddStatement(stmt);
        break;
      case OP_throw:
        if (mirModule.GetSrcLang() == kSrcLangJava) {
          if (GenerateExceptionHandlingCode()) {
            LowerStmt(*stmt, *newBlk);
            newBlk->AddStatement(stmt);
          }
        } else {
          LowerStmt(*stmt, *newBlk);
          newBlk->AddStatement(stmt);
        }
        break;
      case OP_syncenter:
      case OP_syncexit: {
        LowerStmt(*stmt, *newBlk);
        StmtNode *tmp = LowerSyncEnterSyncExit(*stmt);
        CHECK_FATAL(tmp != nullptr, "nullptr is not expected");
        newBlk->AddStatement(tmp);
        break;
      }
      case OP_decrefreset: {
        /*
         * only gconly can reach here
         * lower stmt (decrefreset (addrof ptr %RegX_RXXXX)) to (dassign %RegX_RXXXX 0 (constval ref 0))
         */
        CHECK_FATAL(CGOptions::IsGCOnly(), "OP_decrefreset is expected only in gconly.");
        LowerResetStmt(*stmt, *newBlk);
        break;
      }
      default:
        LowerStmt(*stmt, *newBlk);
        newBlk->AddStatement(stmt);
        break;
    }
  } while (nextStmt != nullptr);
  return newBlk;
}

MIRType *CGLowerer::GetArrayNodeType(BaseNode &baseNode) {
  MIRType *baseType = nullptr;
  auto curFunc = mirModule.CurFunction();
  if (baseNode.GetOpCode() == OP_regread) {
      RegreadNode *rrNode = static_cast<RegreadNode*>(&baseNode);
      MIRPreg *pReg = curFunc->GetPregTab()->PregFromPregIdx(rrNode->GetRegIdx());
      if (pReg->IsRef()) {
        baseType = pReg->GetMIRType();
      }
  }
  if (baseNode.GetOpCode() == OP_dread) {
    DreadNode *dreadNode = static_cast<DreadNode*>(&baseNode);
    MIRSymbol *symbol = curFunc->GetLocalOrGlobalSymbol(dreadNode->GetStIdx());
    baseType = symbol->GetType();
  }
  MIRType *arrayElemType = nullptr;
  if (baseType != nullptr) {
    MIRType *stType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(
        static_cast<MIRPtrType*>(baseType)->GetPointedTyIdx());
    while (kTypeJArray == stType->GetKind()) {
      MIRJarrayType *baseType1 = static_cast<MIRJarrayType*>(stType);
      MIRType *elemType = baseType1->GetElemType();
      if (elemType->GetKind() == kTypePointer) {
        const TyIdx &index = static_cast<MIRPtrType*>(elemType)->GetPointedTyIdx();
        stType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(index);
      } else {
        stType = elemType;
      }
    }

    arrayElemType = stType;
  }
  return arrayElemType;
}

void CGLowerer::SplitCallArg(CallNode &callNode, BaseNode *newOpnd, size_t i, BlockNode &newBlk) {
  if (newOpnd->GetOpCode() != OP_regread && newOpnd->GetOpCode() != OP_constval &&
      newOpnd->GetOpCode() != OP_dread && newOpnd->GetOpCode() != OP_addrof &&
      newOpnd->GetOpCode() != OP_iaddrof && newOpnd->GetOpCode() != OP_constval &&
      newOpnd->GetOpCode() != OP_conststr && newOpnd->GetOpCode() != OP_conststr16) {
    if (CGOptions::GetInstance().GetOptimizeLevel() == CGOptions::kLevel0) {
      MIRType *type = GlobalTables::GetTypeTable().GetPrimType(newOpnd->GetPrimType());
      MIRSymbol *ret = CreateNewRetVar(*type, kIntrnRetValPrefix);
      DassignNode *dassignNode = mirBuilder->CreateStmtDassign(*ret, 0, newOpnd);
      newBlk.AddStatement(dassignNode);
      callNode.SetOpnd(mirBuilder->CreateExprDread(*type, 0, *ret), i);
    } else {
      PregIdx pregIdx = mirModule.CurFunction()->GetPregTab()->CreatePreg(newOpnd->GetPrimType());
      RegassignNode *temp = mirBuilder->CreateStmtRegassign(newOpnd->GetPrimType(), pregIdx, newOpnd);
      newBlk.AddStatement(temp);
      callNode.SetOpnd(mirBuilder->CreateExprRegread(newOpnd->GetPrimType(), pregIdx), i);
    }
  } else {
    callNode.SetOpnd(newOpnd, i);
  }
}

StmtNode *CGLowerer::LowerCall(CallNode &callNode, StmtNode *&nextStmt, BlockNode &newBlk) {
  /*
   * nextStmt in-out
   * call $foo(constval u32 128)
   * dassign %jlt (dread agg %%retval)
   */
  bool isArrayStore = false;

  if (callNode.GetOpCode() == OP_call) {
    MIRFunction *calleeFunc = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(callNode.GetPUIdx());
    if ((calleeFunc->GetName() == "MCC_WriteRefField") && (callNode.Opnd(1)->GetOpCode() == OP_iaddrof)) {
      IreadNode *addrExpr = static_cast<IreadNode*>(callNode.Opnd(1));
      if (addrExpr->Opnd(0)->GetOpCode() == OP_array) {
        isArrayStore = true;
      }
    }
  }

  for (size_t i = 0; i < callNode.GetNopndSize(); ++i) {
    BaseNode *newOpnd = LowerExpr(callNode, *callNode.GetNopndAt(i), newBlk);
#if TARGAARCH64
    callNode.SetOpnd(newOpnd, i);
#else
    SplitCallArg(callNode, newOpnd, i, newBlk);
#endif
  }

  if (isArrayStore && checkLoadStore) {
    bool needCheckStore = true;
    MIRType *arrayElemType = GetArrayNodeType(*callNode.Opnd(0));
    MIRType *valueRealType = GetArrayNodeType(*callNode.Opnd(kNodeThirdOpnd));
    if ((arrayElemType != nullptr) && (valueRealType != nullptr) && (arrayElemType->GetKind() == kTypeClass) &&
        static_cast<MIRClassType*>(arrayElemType)->IsFinal() && (valueRealType->GetKind() == kTypeClass) &&
        static_cast<MIRClassType*>(valueRealType)->IsFinal() &&
        valueRealType->GetTypeIndex() == arrayElemType->GetTypeIndex()) {
      needCheckStore = false;
    }

    if (needCheckStore) {
      MIRFunction *fn = mirModule.GetMIRBuilder()->GetOrCreateFunction("MCC_Reflect_Check_Arraystore", TyIdx(PTY_void));
      MapleVector<BaseNode*> args(mirModule.GetMIRBuilder()->GetCurrentFuncCodeMpAllocator()->Adapter());
      args.push_back(callNode.Opnd(0));
      args.push_back(callNode.Opnd(kNodeThirdOpnd));
      StmtNode *checkStoreStmt = mirModule.GetMIRBuilder()->CreateStmtCall(fn->GetPuidx(), args);
      newBlk.AddStatement(checkStoreStmt);
    }
  }

  if (callNode.GetOpCode() == OP_icall) {
    return &callNode;
  }

  DassignNode *dassignNode = nullptr;
  if ((nextStmt != nullptr) && (nextStmt->GetOpCode() == OP_dassign)) {
    dassignNode = static_cast<DassignNode*>(nextStmt);
  }

  /* if nextStmt is not a dassign stmt, return */
  if (dassignNode == nullptr) {
    return &callNode;
  }

  MIRFunction *calleeFunc = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(callNode.GetPUIdx());
  MIRType *retType = calleeFunc->GetReturnType();
  if (calleeFunc->IsReturnStruct() && (retType->GetPrimType() == PTY_void)) {
    MIRPtrType *pretType = static_cast<MIRPtrType*>((calleeFunc->GetNthParamType(0)));
    CHECK_FATAL(pretType != nullptr, "nullptr is not expected");
    retType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(pretType->GetPointedTyIdx());
    CHECK_FATAL(retType->GetKind() == kTypeStruct, "make sure retType is a struct type");
  }

  /* if return type is not of a struct, return */
  if (retType->GetKind() != kTypeStruct) {
    return &callNode;
  }

  MIRSymbol *dsgnSt = mirModule.CurFunction()->GetLocalOrGlobalSymbol(dassignNode->GetStIdx());
  CHECK_FATAL(dsgnSt->GetType()->IsStructType(), "expects a struct type");
  MIRStructType *structTy = static_cast<MIRStructType*>(dsgnSt->GetType());
  CHECK_FATAL(structTy != nullptr, "expects that the assignee variable should have a struct type");

  RegreadNode *regReadNode = nullptr;
  if (dassignNode->Opnd(0)->GetOpCode() == OP_regread) {
    regReadNode = static_cast<RegreadNode*>(dassignNode->Opnd(0));
  }
  if (regReadNode == nullptr || (regReadNode->GetRegIdx() != -kSregRetval0)) {
    return &callNode;
  }

  MapleVector<BaseNode*> newNopnd(mirModule.CurFuncCodeMemPoolAllocator()->Adapter());
  AddrofNode *addrofNode = mirModule.CurFuncCodeMemPool()->New<AddrofNode>(OP_addrof);
  addrofNode->SetPrimType(LOWERED_PTR_TYPE);
  addrofNode->SetStIdx(dsgnSt->GetStIdx());
  addrofNode->SetFieldID(0);
  newNopnd.push_back(addrofNode);
  for (auto *opnd : callNode.GetNopnd()) {
    newNopnd.push_back(opnd);
  }
  callNode.SetNOpnd(newNopnd);
  callNode.SetNumOpnds(static_cast<uint8>(newNopnd.size()));
  CHECK_FATAL(nextStmt != nullptr, "nullptr is not expected");
  nextStmt = nextStmt->GetNext();
  return &callNode;
}

void CGLowerer::LowerEntry(MIRFunction &func) {
  if (func.IsReturnStruct()) {
    MIRSymbol *retSt = func.GetSymTab()->CreateSymbol(kScopeLocal);
    retSt->SetStorageClass(kScFormal);
    retSt->SetSKind(kStVar);
    std::string retName(".return.");
    MIRSymbol *funcSt = GlobalTables::GetGsymTable().GetSymbolFromStidx(func.GetStIdx().Idx());
    retName.append(funcSt->GetName());
    retSt->SetNameStrIdx(retName);
    MIRType *pointType = beCommon.BeGetOrCreatePointerType(*func.GetReturnType());

    retSt->SetTyIdx(pointType->GetTypeIndex());
    MapleVector<MIRSymbol*> formals(mirModule.GetMPAllocator().Adapter());
    formals.push_back(retSt);
    for (uint32 i = 0; i < func.GetFormalCount(); ++i) {
      auto formal = func.GetFormal(i);
      formals.push_back(formal);
    }
    func.ClearFormals();
    for (MapleVector<MIRSymbol*>::iterator it = formals.begin(); it != formals.end(); ++it) {
      func.AddArgument(*it);
    }
    func.SetReturnTyIdx(GlobalTables::GetTypeTable().GetTypeTable().at(static_cast<int>(PTY_void))->GetTypeIndex());
  }
}

void CGLowerer::LowerPseudoRegs(MIRFunction &func) {
  for (uint32 i = 1; i < func.GetPregTab()->Size(); ++i) {
    MIRPreg *ipr = func.GetPregTab()->PregFromPregIdx(i);
    PrimType primType = ipr->GetPrimType();
    if ((primType == PTY_ptr) || (primType == PTY_ref)) {
      ipr->SetPrimType(LOWERED_PTR_TYPE);
    } else if (primType == PTY_u1) {
      ipr->SetPrimType(PTY_u32);
    }
  }
}

void CGLowerer::CleanupBranches(MIRFunction &func) const {
  BlockNode *block = func.GetBody();
  StmtNode *prev = nullptr;
  StmtNode *next = nullptr;
  for (StmtNode *curr = block->GetFirst(); curr != nullptr; curr = next) {
    next = curr->GetNext();
    if (next != nullptr) {
      CHECK_FATAL(curr == next->GetPrev(), "unexpected node");
    }
    if ((next != nullptr) && (prev != nullptr) && (curr->GetOpCode() == OP_goto)) {
      /*
       * Skip until find a label.
       * Note that the CURRent 'goto' statement may be the last statement
       * when discounting comment statements.
       * Make sure we don't lose any comments.
       */
      StmtNode *cmtB = nullptr;
      StmtNode *cmtE = nullptr;
      bool isCleanable = true;
      while ((next != nullptr) && (next->GetOpCode() != OP_label)) {
        if ((next->GetOpCode() == OP_try) || (next->GetOpCode() == OP_endtry) || (next->GetOpCode() == OP_catch)) {
          isCleanable = false;
          break;
        }
        next = next->GetNext();
      }
      if ((next != nullptr) && (isCleanable == false)) {
        prev = next->GetPrev();
        continue;
      }

      next = curr->GetNext();

      while ((next != nullptr) && (next->GetOpCode() != OP_label)) {
        if (next->GetOpCode() == OP_comment) {
          if (cmtB == nullptr) {
            cmtB = next;
            cmtE = next;
          } else {
            CHECK_FATAL(cmtE != nullptr, "cmt_e is null in CGLowerer::CleanupBranches");
            cmtE->SetNext(next);
            next->SetPrev(cmtE);
            cmtE = next;
          }
        }
        next = next->GetNext();
      }

      curr->SetNext(next);

      if (next != nullptr) {
        next->SetPrev(curr);
      }

      StmtNode *insertAfter = nullptr;

      if ((next != nullptr) &&
          ((static_cast<GotoNode*>(curr))->GetOffset() == (static_cast<LabelNode*>(next))->GetLabelIdx())) {
        insertAfter = prev;
        prev->SetNext(next);  /* skip goto statement (which is pointed by curr) */
        next->SetPrev(prev);
        curr = next;        /* make curr point to the label statement */
        next = next->GetNext();  /* advance next to the next statement of the label statement */
      } else {
        insertAfter = curr;
      }

      /* insert comments before 'curr' */
      if (cmtB != nullptr) {
        CHECK_FATAL(cmtE != nullptr, "nullptr is not expected");
        StmtNode *iaNext = insertAfter->GetNext();
        if (iaNext != nullptr) {
          iaNext->SetPrev(cmtE);
        }
        cmtE->SetNext(iaNext);

        insertAfter->SetNext(cmtB);
        cmtB->SetPrev(insertAfter);

        if (insertAfter == curr) {
          curr = cmtE;
        }
      }
      if (next == nullptr) {
        func.GetBody()->SetLast(curr);
      }
    }
    prev = curr;
  }
  CHECK_FATAL(func.GetBody()->GetLast() == prev, "make sure the return value of GetLast equal prev");
}

/*
 * We want to place catch blocks so that they don't come before any of java trys that refer to them.
 * In order to do that, we take advantage of the fact that the mpl. source we get is already flattened and
 * no java-try-end-try block is enclosed in any other java-try-end-try block. they appear in the mpl file.
 * We process each bb in bbList from the front to the end, and while doing so, we maintain a list of catch blocks
 * we have seen. When we get to an end-try block, we examine each catch block label it has (offsets),
 * and if we find any catch block in the "seen" list, we move the block after the end-try block.
 * Note that we need to find a basic block which does not have 'fallthruBranch' control path.
 * (Appending the catch block to any basic block that has the 'fallthruBranch' control path
 * will alter the program semantics)
 */
void CGLowerer::LowerTryCatchBlocks(BlockNode &body) {
  if (!hasTry) {
    return;
  }

#if DEBUG
  BBT::ValidateStmtList(nullptr, nullptr);
#endif
  MemPool *memPool = memPoolCtrler.NewMemPool("CreateNewBB mempool");
  TryCatchBlocksLower tryCatchLower(*memPool, body, mirModule);
  tryCatchLower.RecoverBasicBlock();
  bool generateEHCode = GenerateExceptionHandlingCode();
  tryCatchLower.SetGenerateEHCode(generateEHCode);
  tryCatchLower.TraverseBBList();
#if DEBUG
  tryCatchLower.CheckTryCatchPattern();
#endif
  memPoolCtrler.DeleteMemPool(memPool);
}

inline bool IsAccessingTheSameMemoryLocation(const DassignNode &dassign,
                                             const RegreadNode &rRead, const CGLowerer &cgLowerer) {
  StIdx stIdx = cgLowerer.GetSymbolReferredToByPseudoRegister(rRead.GetRegIdx());
  return ((dassign.GetStIdx() == stIdx) && (dassign.GetFieldID() == 0));
}

inline bool IsAccessingTheSameMemoryLocation(const DassignNode &dassign, const DreadNode &dread) {
  return ((dassign.GetStIdx() == dread.GetStIdx()) && (dassign.GetFieldID() == dread.GetFieldID()));
}

inline bool IsDassignNOP(const DassignNode &dassign) {
  if (dassign.GetRHS()->GetOpCode() == OP_dread) {
    return IsAccessingTheSameMemoryLocation(dassign, static_cast<DreadNode&>(*dassign.GetRHS()));
  }
  return false;
}

inline bool IsConstvalZero(const BaseNode &n) {
  return ((n.GetOpCode() == OP_constval) && static_cast<const ConstvalNode&>(n).GetConstVal()->IsZero());
}

#define NEXT_ID(x) ((x) + 1)
#define INTRN_FIRST_SYNC_ENTER NEXT_ID(INTRN_LAST)
#define INTRN_SECOND_SYNC_ENTER NEXT_ID(INTRN_FIRST_SYNC_ENTER)
#define INTRN_THIRD_SYNC_ENTER NEXT_ID(INTRN_SECOND_SYNC_ENTER)
#define INTRN_FOURTH_SYNC_ENTER NEXT_ID(INTRN_THIRD_SYNC_ENTER)
#define INTRN_YNC_EXIT NEXT_ID(INTRN_FOURTH_SYNC_ENTER)

std::vector<std::pair<CGLowerer::BuiltinFunctionID, PUIdx>> CGLowerer::builtinFuncIDs;
std::unordered_map<IntrinDesc*, PUIdx> CGLowerer::intrinFuncIDs;

/* get well known framework class 1st..6th element as pair first element */
static std::vector<std::pair<std::string, uint32>> wellKnownFrameWorksClass{
};

static uint32 GetWellKnownFrameWorksClassFlag(const std::string &className) {
  for (auto it = wellKnownFrameWorksClass.begin(); it != wellKnownFrameWorksClass.end(); ++it) {
    if (className == (*it).first) {
      return (*it).second;
    }
  }
  return 0;
}

MIRFunction *CGLowerer::RegisterFunctionVoidStarToVoid(BuiltinFunctionID id, const std::string &name,
                                                       const std::string &paramName) {
  MIRFunction *func = mirBuilder->GetOrCreateFunction(name, GlobalTables::GetTypeTable().GetVoid()->GetTypeIndex());
  MIRSymbol *funcSym = func->GetFuncSymbol();
  funcSym->SetStorageClass(kScExtern);
  MIRType *argTy = GlobalTables::GetTypeTable().GetPtr();
  MIRSymbol *argSt = func->GetSymTab()->CreateSymbol(kScopeLocal);
  argSt->SetNameStrIdx(mirBuilder->GetOrCreateStringIndex(paramName));
  argSt->SetTyIdx(argTy->GetTypeIndex());
  argSt->SetStorageClass(kScFormal);
  argSt->SetSKind(kStVar);
  func->GetSymTab()->AddToStringSymbolMap(*argSt);
  func->AddArgument(argSt);
  if ((name == "MCC_SyncEnterFast0") || (name == "MCC_SyncEnterFast1") ||
      (name == "MCC_SyncEnterFast2") || (name == "MCC_SyncEnterFast3") ||
      (name == "MCC_SyncExitFast")) {
    MIRSymbol *argStMatch = func->GetSymTab()->CreateSymbol(kScopeLocal);
    argStMatch->SetNameStrIdx(mirBuilder->GetOrCreateStringIndex("monitor_slot"));
    argStMatch->SetTyIdx(argTy->GetTypeIndex());
    argStMatch->SetStorageClass(kScFormal);
    argStMatch->SetSKind(kStVar);
    func->GetSymTab()->AddToStringSymbolMap(*argStMatch);
    func->AddArgument(argStMatch);
  }

  builtinFuncIDs.push_back(std::pair<BuiltinFunctionID, PUIdx>(id, func->GetPuidx()));
  return func;
}

void CGLowerer::RegisterBuiltIns() {
  for (uint32 i = 0; i < sizeof(cgBuiltins) / sizeof(cgBuiltins[0]); ++i) {
    BuiltinFunctionID id = cgBuiltins[i].first;
    IntrinDesc &desc = IntrinDesc::intrinTable[id];

    MIRFunction *func = mirBuilder->GetOrCreateFunction(cgBuiltins[i].second,
                                                        GlobalTables::GetTypeTable().GetVoid()->GetTypeIndex());
    MIRSymbol *funcSym = func->GetFuncSymbol();
    funcSym->SetStorageClass(kScExtern);

    /* return type */
    MIRType *retTy = desc.GetReturnType();
    CHECK_FATAL(retTy != nullptr, "retTy should not be nullptr");
    /* use void* for PTY_dynany */
    if (retTy->GetPrimType() == PTY_dynany) {
      retTy = GlobalTables::GetTypeTable().GetPtr();
    }
    func->SetReturnTyIdx(retTy->GetTypeIndex());

    const std::string params[IntrinDesc::kMaxArgsNum] = { "p0", "p1", "p2", "p3", "p4", "p5" };
    for (uint32 j = 0; j < IntrinDesc::kMaxArgsNum; ++j) {
      MIRType *argTy = desc.GetArgType(j);
      if (argTy == nullptr) {
        break;
      }
      /* use void* for PTY_dynany */
      if (argTy->GetPrimType() == PTY_dynany) {
        argTy = GlobalTables::GetTypeTable().GetPtr();
      }
      MIRSymbol *argSt = func->GetSymTab()->CreateSymbol(kScopeLocal);
      argSt->SetNameStrIdx(mirBuilder->GetOrCreateStringIndex(params[j]));
      argSt->SetTyIdx(argTy->GetTypeIndex());
      argSt->SetStorageClass(kScFormal);
      argSt->SetSKind(kStVar);
      func->GetSymTab()->AddToStringSymbolMap(*argSt);
      func->AddArgument(argSt);
    }

    builtinFuncIDs.push_back(std::pair<BuiltinFunctionID, PUIdx>(id, func->GetPuidx()));
  }

  /* register __builtin_sync_enter */
  static_cast<void>(RegisterFunctionVoidStarToVoid(INTRN_FIRST_SYNC_ENTER, "MCC_SyncEnterFast0", "obj"));
  static_cast<void>(RegisterFunctionVoidStarToVoid(INTRN_SECOND_SYNC_ENTER, "MCC_SyncEnterFast1", "obj"));
  static_cast<void>(RegisterFunctionVoidStarToVoid(INTRN_THIRD_SYNC_ENTER, "MCC_SyncEnterFast2", "obj"));
  static_cast<void>(RegisterFunctionVoidStarToVoid(INTRN_FOURTH_SYNC_ENTER, "MCC_SyncEnterFast3", "obj"));
  /* register __builtin_sync_exit */
  static_cast<void>(RegisterFunctionVoidStarToVoid(INTRN_YNC_EXIT, "MCC_SyncExitFast", "obj"));
}

/*
 * From Maple IR Document as of Apr 14, 2017
 * Type Conversion Expression Opcodes
 * Conversions between integer types of different sizes require the cvt opcode.
 * Conversion between signed and unsigned integers of the same size does not
 * require any operation, not even retype.
 * cvt :
 * Convert the operand's value from <from-type> to <to-type>.
 * If the sizes of the two types are the same, the conversion must involve
 * altering the bits.
 * retype:
 * <opnd0> is converted to <prim-type> which has derived type <type> without
 * changing any bits.  The size of <opnd0> and <prim-type> must be the same.
 * <opnd0> may be of aggregate type.
 */
BaseNode *CGLowerer::MergeToCvtType(PrimType dType, PrimType sType, BaseNode &src) {
  CHECK_FATAL(IsPrimitiveInteger(dType) || IsPrimitiveFloat(dType),
              "dtype should be primitiveInteger or primitiveFloat");
  CHECK_FATAL(IsPrimitiveInteger(sType) || IsPrimitiveFloat(sType),
              "sType should be primitiveInteger or primitiveFloat");
  /* src i32, dest f32; src i64, dest f64 */
  CHECK_FATAL(
      (IsPrimitiveInteger(sType) && IsPrimitiveFloat(dType) &&
       (GetPrimTypeBitSize(sType) == GetPrimTypeBitSize(dType))) ||
      (IsPrimitiveInteger(sType) && IsPrimitiveInteger(dType)),
      "when sType is primitiveInteger and dType is primitiveFloat, sType's primTypeBitSize must equal dType's,"
      " or both sType and dType should primitiveInteger");

  /* src & dest are both of float type */
  MIRType *toType = GlobalTables::GetTypeTable().GetPrimType(dType);
  MIRType *fromType = GlobalTables::GetTypeTable().GetPrimType(sType);
  if (IsPrimitiveInteger(sType) && IsPrimitiveFloat(dType) &&
      (GetPrimTypeBitSize(sType) == GetPrimTypeBitSize(dType))) {
    return mirBuilder->CreateExprRetype(*toType, *fromType, &src);
  } else if (IsPrimitiveInteger(sType) && IsPrimitiveInteger(dType)) {
    if (GetPrimTypeBitSize(sType) >= GetPrimTypeBitSize(dType)) {
      if (dType == PTY_u1) {  /* e.g., type _Bool */
        toType = GlobalTables::GetTypeTable().GetPrimType(PTY_u8);
        return mirBuilder->CreateExprCompare(OP_ne, *toType, *fromType, &src, mirBuilder->CreateIntConst(0, sType));
      } else if (GetPrimTypeBitSize(sType) > GetPrimTypeBitSize(dType)) {
        return mirBuilder->CreateExprTypeCvt(OP_cvt, *toType, *fromType, &src);
      } else if (IsSignedInteger(sType) != IsSignedInteger(dType)) {
        return mirBuilder->CreateExprTypeCvt(OP_cvt, *toType, *fromType, &src);
      }
      src.SetPrimType(dType);
      return &src;
      /*
       * Force type cvt here because we currently do not run constant folding
       * or contanst propagation before CG. We may revisit this decision later.
       */
    } else if (GetPrimTypeBitSize(sType) < GetPrimTypeBitSize(dType)) {
      return mirBuilder->CreateExprTypeCvt(OP_cvt, *toType, *fromType, &src);
    } else if (IsConstvalZero(src)) {
      return mirBuilder->CreateIntConst(0, dType);
    }
    CHECK_FATAL(false, "should not run here");
  }
  CHECK_FATAL(false, "should not run here");
}

IreadNode &CGLowerer::GetLenNode(BaseNode &opnd0) {
  MIRIntConst *arrayHeaderNode = GlobalTables::GetIntConstTable().GetOrCreateIntConst(
      static_cast<int64>(AArch64RTSupport::kArrayLengthOffset),
      *GlobalTables::GetTypeTable().GetTypeFromTyIdx(opnd0.GetPrimType()));
  BaseNode *arrayHeaderCstNode = mirModule.CurFuncCodeMemPool()->New<ConstvalNode>(arrayHeaderNode);
  arrayHeaderCstNode->SetPrimType(opnd0.GetPrimType());
  MIRType *addrType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(opnd0.GetPrimType());
  BaseNode *refLenAddr = mirBuilder->CreateExprBinary(OP_add, *addrType, &opnd0, arrayHeaderCstNode);
  MIRType *infoLenType = GlobalTables::GetTypeTable().GetInt32();
  MIRType *ptrType = beCommon.BeGetOrCreatePointerType(*infoLenType);
  IreadNode *lenNode = mirBuilder->CreateExprIread(*infoLenType, *ptrType, 0, refLenAddr);
  return (*lenNode);
}

LabelIdx CGLowerer::GetLabelIdx(MIRFunction &curFunc) const {
  std::string suffix = std::to_string(curFunc.GetLabelTab()->GetLabelTableSize());
  GStrIdx labelStrIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName("__label_BC_" + suffix);
  LabelIdx labIdx = curFunc.GetLabelTab()->AddLabel(labelStrIdx);
  return labIdx;
}

void CGLowerer::ProcessArrayExpr(BaseNode &expr, BlockNode &blkNode) {
  bool needProcessArrayExpr = !ShouldOptarray() && (mirModule.GetSrcLang() == kSrcLangJava);
  if (!needProcessArrayExpr) {
    return;
  }
  /* Array boundary check */
  MIRFunction *curFunc = mirModule.CurFunction();
  auto &arrayNode = static_cast<ArrayNode&>(expr);
  StmtNode *boundaryCheckStmt = nullptr;
  if (arrayNode.GetBoundsCheck()) {
    CHECK_FATAL(arrayNode.GetNopndSize() == kOperandNumBinary, "unexpected nOpnd size");
    BaseNode *opnd0 = arrayNode.GetNopndAt(0);
    if (opnd0->GetOpCode() == OP_iread) {
      PregIdx pregIdx = curFunc->GetPregTab()->CreatePreg(opnd0->GetPrimType());
      RegassignNode *temp = mirBuilder->CreateStmtRegassign(opnd0->GetPrimType(), pregIdx, opnd0);
      blkNode.InsertAfter(blkNode.GetLast(), temp);
      arrayNode.SetNOpndAt(0, mirBuilder->CreateExprRegread(opnd0->GetPrimType(), pregIdx));
    }
    IreadNode &lenNode = GetLenNode(*opnd0);
    PregIdx lenPregIdx = curFunc->GetPregTab()->CreatePreg(lenNode.GetPrimType());
    RegassignNode *lenRegassignNode = mirBuilder->CreateStmtRegassign(lenNode.GetPrimType(), lenPregIdx, &lenNode);
    BaseNode *lenRegreadNode = mirBuilder->CreateExprRegread(PTY_u32, lenPregIdx);

    LabelIdx labIdx = GetLabelIdx(*curFunc);
    LabelNode *labelBC = mirBuilder->CreateStmtLabel(labIdx);;
    BaseNode *cond = mirBuilder->CreateExprCompare(OP_ge, *GlobalTables::GetTypeTable().GetUInt1(),
                                                   *GlobalTables::GetTypeTable().GetUInt32(),
                                                   arrayNode.GetNopndAt(1), lenRegreadNode);
    CondGotoNode *brFalseNode = mirBuilder->CreateStmtCondGoto(cond, OP_brfalse, labIdx);
    MIRFunction *fn = mirBuilder->GetOrCreateFunction("MCC_Array_Boundary_Check", TyIdx(PTY_void));
    MapleVector<BaseNode*> args(mirBuilder->GetCurrentFuncCodeMpAllocator()->Adapter());
    args.push_back(arrayNode.GetNopndAt(0));
    args.push_back(arrayNode.GetNopndAt(1));
    boundaryCheckStmt = mirBuilder->CreateStmtCall(fn->GetPuidx(), args);
    blkNode.InsertAfter(blkNode.GetLast(), lenRegassignNode);
    blkNode.InsertAfter(blkNode.GetLast(), brFalseNode);
    blkNode.InsertAfter(blkNode.GetLast(), boundaryCheckStmt);
    blkNode.InsertAfter(blkNode.GetLast(), labelBC);
  }
}

BaseNode *CGLowerer::LowerExpr(const BaseNode &parent, BaseNode &expr, BlockNode &blkNode) {
  if (expr.GetPrimType() == PTY_u1) {
    expr.SetPrimType(PTY_u8);
  }
  if (expr.GetOpCode() == OP_intrinsicopwithtype) {
    return LowerIntrinsicopwithtype(parent, static_cast<IntrinsicopNode&>(expr), blkNode);
  }

  LowerTypePtr(expr);

  if (expr.GetOpCode() == OP_iread && expr.Opnd(0)->GetOpCode() == OP_array) {
    /* iread ptr <* <$MUIDDataDefTabEntry>> 1 (
     *     array 0 ptr <* <[5] <$MUIDDataDefTabEntry>>> (addrof ...
     * ==>
     * intrinsicop a64 MPL_READ_STATIC_OFFSET_TAB (addrof ..
     */
    BaseNode *node = LowerExpr(expr, *expr.Opnd(0), blkNode);
    if (node->GetOpCode() == OP_intrinsicop) {
      auto *binNode = static_cast<IntrinsicopNode*>(node);
      CHECK_FATAL(binNode->GetIntrinsic() == INTRN_MPL_READ_STATIC_OFFSET_TAB, "Something wrong here");
      return binNode;
    } else {
      expr.SetOpnd(node, 0);
    }
  } else {
    for (size_t i = 0; i < expr.NumOpnds(); ++i) {
      expr.SetOpnd(LowerExpr(expr, *expr.Opnd(i), blkNode), i);
    }
  }
  switch (expr.GetOpCode()) {
    case OP_array: {
      ProcessArrayExpr(expr, blkNode);
      return LowerArray(static_cast<ArrayNode&>(expr), parent);
    }

    case OP_dread:
      return LowerDread(static_cast<DreadNode&>(expr));

    case OP_addrof:
      return LowerAddrof(static_cast<AddrofNode&>(expr));

    case OP_iread:
      return LowerIread(static_cast<IreadNode&>(expr));

    case OP_iaddrof:
      return LowerIaddrof(static_cast<IreadNode&>(expr));

    case OP_sizeoftype: {
      CHECK(static_cast<SizeoftypeNode&>(expr).GetTyIdx() < beCommon.GetSizeOfTypeSizeTable(),
            "index out of range in CGLowerer::LowerExpr");
      int64 typeSize = beCommon.GetTypeSize(static_cast<SizeoftypeNode&>(expr).GetTyIdx());
      return mirModule.GetMIRBuilder()->CreateIntConst(typeSize, PTY_u32);
    }

    case OP_fieldsdist: {
      auto &fdNode = static_cast<FieldsDistNode&>(expr);
      CHECK(fdNode.GetTyIdx() < beCommon.GetSizeOfTypeSizeTable(),
            "index out of range in CGLowerer::LowerExpr");
      MIRType *ty = GlobalTables::GetTypeTable().GetTypeFromTyIdx(fdNode.GetTyIdx());
      CHECK(ty->GetKind() == kTypeClass, "wrong type for FieldsDistNode");
      MIRClassType *classType = static_cast<MIRClassType*>(ty);
      const JClassLayout &layout = beCommon.GetJClassLayout(*classType);
      ASSERT(!layout.empty(), "container should not be empty");
      int32 i1 = fdNode.GetFiledID1() > 0 ? fdNode.GetFiledID1() - 1 : 0;
      int32 i2 = fdNode.GetFiledID2() > 0 ? fdNode.GetFiledID2() - 1 : 0;
      int64 offset = layout[i2].GetOffset() - layout[i1].GetOffset();
      return mirModule.GetMIRBuilder()->CreateIntConst(offset, PTY_u32);
    }

    case OP_intrinsicop:
      return LowerIntrinsicop(parent, static_cast<IntrinsicopNode&>(expr), blkNode);

    case OP_alloca: {
      return &expr;
    }
    case OP_rem:
      return LowerRem(expr, blkNode);

    case OP_cand:
      expr.SetOpCode(OP_land);
      return &expr;
    case OP_cior:
      expr.SetOpCode(OP_lior);
      return &expr;
    default:
      return &expr;
  }
}

BaseNode *CGLowerer::LowerDread(DreadNode &dread) {
  /* use PTY_u8 for boolean type in dread/iread */
  if (dread.GetPrimType() == PTY_u1) {
    dread.SetPrimType(PTY_u8);
  }
  return (dread.GetFieldID() == 0 ? &dread : LowerDreadBitfield(dread));
}

void CGLowerer::LowerRegassign(RegassignNode &regNode, BlockNode &newBlk) {
  CHECK_FATAL(regNode.GetPrimType() != PTY_ptr, "should have been lowered already");
  CHECK_FATAL(regNode.GetPrimType() != PTY_ref, "should have been lowered already");
  BaseNode *rhsOpnd = regNode.Opnd(0);
  Opcode op = rhsOpnd->GetOpCode();
  if ((op == OP_gcmalloc) || (op == OP_gcpermalloc)) {
    LowerGCMalloc(regNode, static_cast<GCMallocNode&>(*rhsOpnd), newBlk, op == OP_gcpermalloc);
    return;
  } else if ((op == OP_gcmallocjarray) || (op == OP_gcpermallocjarray)) {
    LowerJarrayMalloc(regNode, static_cast<JarrayMallocNode&>(*rhsOpnd), newBlk, op == OP_gcpermallocjarray);
    return;
  } else {
    regNode.SetOpnd(LowerExpr(regNode, *rhsOpnd, newBlk), 0);
    newBlk.AddStatement(&regNode);
  }
}

void CGLowerer::LowerDassign(DassignNode &dsNode, BlockNode &newBlk) {
  StmtNode *newStmt = nullptr;
  BaseNode *rhs = nullptr;
  Opcode op = dsNode.GetRHS()->GetOpCode();
  if (dsNode.GetFieldID() != 0) {
    newStmt = LowerDassignBitfield(dsNode, newBlk);
  } else if (op == OP_intrinsicop) {
    IntrinsicopNode *intrinNode = static_cast<IntrinsicopNode*>(dsNode.GetRHS());
    MIRType *retType = IntrinDesc::intrinTable[intrinNode->GetIntrinsic()].GetReturnType();
    CHECK_FATAL(retType != nullptr, "retType should not be nullptr");
    if (retType->GetKind() == kTypeStruct) {
      newStmt = LowerIntrinsicopDassign(dsNode, *intrinNode, newBlk);
    } else {
      rhs = LowerExpr(dsNode, *intrinNode, newBlk);
      dsNode.SetRHS(rhs);
      CHECK_FATAL(dsNode.GetRHS() != nullptr, "dsNode->rhs is null in CGLowerer::LowerDassign");
      if (!IsDassignNOP(dsNode)) {
        newStmt = &dsNode;
      }
    }
  } else if ((op == OP_gcmalloc) || (op == OP_gcpermalloc)) {
    LowerGCMalloc(dsNode, static_cast<GCMallocNode&>(*dsNode.GetRHS()), newBlk, op == OP_gcpermalloc);
    return;
  } else if ((op == OP_gcmallocjarray) || (op == OP_gcpermallocjarray)) {
    LowerJarrayMalloc(dsNode, static_cast<JarrayMallocNode&>(*dsNode.GetRHS()), newBlk, op == OP_gcpermallocjarray);
    return;
  } else {
    rhs = LowerExpr(dsNode, *dsNode.GetRHS(), newBlk);
    dsNode.SetRHS(rhs);
    newStmt = &dsNode;
  }

  if (newStmt != nullptr) {
    newBlk.AddStatement(newStmt);
  }
}

// Lower stmt Form
// Initial form: decrefreset (addrof ptr %RegX_RXXXX)
// Convert to form: dassign %RegX_RXXXX 0 (constval ref 0)
// Final form: str xzr, [x29,#XX]
void CGLowerer::LowerResetStmt(StmtNode &stmt, BlockNode &block) {
  UnaryStmtNode &unaryStmtNode = static_cast<UnaryStmtNode&>(stmt);
  AddrofNode *addrofNode = static_cast<AddrofNode*>(unaryStmtNode.GetRHS());
  MIRType &type = *GlobalTables::GetTypeTable().GetPrimType(PTY_ref);
  MIRConst *constVal = GlobalTables::GetIntConstTable().GetOrCreateIntConst(0, type);
  ConstvalNode *exprConst = mirModule.CurFuncCodeMemPool()->New<ConstvalNode>();
  exprConst->SetPrimType(type.GetPrimType());
  exprConst->SetConstVal(constVal);
  DassignNode *dassignNode = mirModule.CurFuncCodeMemPool()->New<DassignNode>();
  dassignNode->SetStIdx(addrofNode->GetStIdx());
  dassignNode->SetRHS(exprConst);
  dassignNode->SetFieldID(addrofNode->GetFieldID());
  block.AddStatement(dassignNode);
}

StmtNode *CGLowerer::LowerIntrinsicopDassign(const DassignNode &dsNode,
                                             IntrinsicopNode &intrinNode, BlockNode &newBlk) {
  for (size_t i = 0; i < intrinNode.GetNumOpnds(); ++i) {
    ASSERT(intrinNode.Opnd(i) != nullptr, "intrinNode.Opnd(i) should not be nullptr");
    intrinNode.SetOpnd(LowerExpr(intrinNode, *intrinNode.Opnd(i), newBlk), i);
  }
  MIRIntrinsicID intrnID = intrinNode.GetIntrinsic();
  IntrinDesc *intrinDesc = &IntrinDesc::intrinTable[intrnID];
  MIRSymbol *st = GlobalTables::GetGsymTable().CreateSymbol(kScopeGlobal);
  const std::string name = intrinDesc->name;
  CHECK_FATAL(intrinDesc->name != nullptr, "intrinDesc's name should not be nullptr");
  st->SetNameStrIdx(name);
  st->SetStorageClass(kScText);
  st->SetSKind(kStFunc);
  MIRFunction *fn = mirModule.GetMemPool()->New<MIRFunction>(&mirModule, st->GetStIdx());
  fn->Init();
  MapleVector<BaseNode*> &nOpnds = intrinNode.GetNopnd();
  st->SetFunction(fn);
  std::vector<TyIdx> fnTyVec;
  std::vector<TypeAttrs> fnTaVec;
  CHECK_FATAL(intrinDesc->IsJsOp(), "intrinDesc should be JsOp");
  /* setup parameters */
  for (uint32 i = 0; i < nOpnds.size(); ++i) {
    fnTyVec.push_back(GlobalTables::GetTypeTable().GetTypeFromTyIdx(PTY_a32)->GetTypeIndex());
    fnTaVec.push_back(TypeAttrs());
    BaseNode *addrNode = beCommon.GetAddressOfNode(*nOpnds[i]);
    CHECK_FATAL(addrNode != nullptr, "addrNode should not be nullptr");
    nOpnds[i] = addrNode;
  }
  MIRSymbol *dst = mirModule.CurFunction()->GetLocalOrGlobalSymbol(dsNode.GetStIdx());
  MIRType *ty = dst->GetType();
  MIRType *fnType = beCommon.BeGetOrCreateFunctionType(ty->GetTypeIndex(), fnTyVec, fnTaVec);
  st->SetTyIdx(fnType->GetTypeIndex());
  fn->SetMIRFuncType(static_cast<MIRFuncType*>(fnType));
  fn->SetReturnTyIdx(ty->GetTypeIndex());
  CHECK_FATAL(ty->GetKind() == kTypeStruct, "ty's kind should be struct type");
  CHECK_FATAL(dsNode.GetFieldID() == 0, "dsNode's filedId should equal");
  AddrofNode *addrofNode = mirBuilder->CreateAddrof(*dst, PTY_a32);
  MapleVector<BaseNode*> newOpnd(mirModule.CurFuncCodeMemPoolAllocator()->Adapter());
  newOpnd.push_back(addrofNode);
  newOpnd.insert(newOpnd.end(), nOpnds.begin(), nOpnds.end());
  CallNode *callStmt = mirModule.CurFuncCodeMemPool()->New<CallNode>(mirModule, OP_call);
  callStmt->SetPUIdx(st->GetFunction()->GetPuidx());
  callStmt->SetNOpnd(newOpnd);
  return callStmt;
}

/*   From maple_ir/include/dex2mpl/dexintrinsic.def
 *   JAVA_ARRAY_LENGTH
 *   JAVA_ARRAY_FILL
 *   JAVA_FILL_NEW_ARRAY
 *   JAVA_CHECK_CAST
 *   JAVA_CONST_CLASS
 *   JAVA_INSTANCE_OF
 *   JAVA_MERGE
 *   JAVA_RANDOM
 *   #if DEXHACK
 *   JAVA_PRINTLN
 *   #endif
 *   INTRN_<<name>>
 *   intrinsic
 */
BaseNode *CGLowerer::LowerJavascriptIntrinsicop(IntrinsicopNode &intrinNode, const IntrinDesc &desc) {
  MIRSymbol *st = GlobalTables::GetGsymTable().CreateSymbol(kScopeGlobal);
  CHECK_FATAL(desc.name != nullptr, "desc's name should not be nullptr");
  const std::string name = desc.name;
  st->SetNameStrIdx(name);
  st->SetStorageClass(kScText);
  st->SetSKind(kStFunc);
  MIRFunction *fn = mirModule.GetMemPool()->New<MIRFunction>(&mirModule, st->GetStIdx());
  fn->Init();
  MapleVector<BaseNode*> &nOpnds = intrinNode.GetNopnd();
  st->SetFunction(fn);
  std::vector<TyIdx> fnTyVec;
  std::vector<TypeAttrs> fnTaVec;
  CHECK_FATAL(desc.IsJsOp(), "desc should be jsOp");
  /* setup parameters */
  for (uint32 i = 0; i < nOpnds.size(); ++i) {
    fnTyVec.push_back(GlobalTables::GetTypeTable().GetTypeFromTyIdx(PTY_a32)->GetTypeIndex());
    fnTaVec.push_back(TypeAttrs());
    BaseNode *addrNode = beCommon.GetAddressOfNode(*nOpnds[i]);
    CHECK_FATAL(addrNode != nullptr, "can not get address");
    nOpnds[i] = addrNode;
  }

  MIRType *retType = desc.GetReturnType();
  CHECK_FATAL(retType != nullptr, "retType should not be nullptr");
  if (retType->GetKind() == kTypeStruct) {
    /* create a local symbol and dread it; */
    std::string tmpstr("__ret_struct_tmp_st");
    static uint32 tmpIdx = 0;
    tmpstr.append(std::to_string(tmpIdx++));
    MIRSymbol *tmpSt = mirBuilder->GetOrCreateDeclInFunc(tmpstr, *retType, *mirModule.CurFunction());
    MIRType *fnType = beCommon.BeGetOrCreateFunctionType(retType->GetTypeIndex(), fnTyVec, fnTaVec);
    st->SetTyIdx(fnType->GetTypeIndex());
    fn->SetMIRFuncType(static_cast<MIRFuncType*>(fnType));
    AddrofNode *addrofNode = mirBuilder->CreateAddrof(*tmpSt, PTY_a32);
    MapleVector<BaseNode*> newOpnd(mirModule.CurFuncCodeMemPoolAllocator()->Adapter());
    newOpnd.push_back(addrofNode);
    newOpnd.insert(newOpnd.end(), nOpnds.begin(), nOpnds.end());
    CallNode *callStmt = mirModule.CurFuncCodeMemPool()->New<CallNode>(mirModule, OP_call);
    callStmt->SetPUIdx(st->GetFunction()->GetPuidx());
    callStmt->SetNOpnd(newOpnd);
    currentBlock->AddStatement(callStmt);
    /* return the dread */
    AddrofNode *drRetSt = mirBuilder->CreateDread(*tmpSt, PTY_agg);
    return drRetSt;
  }
  CHECK_FATAL(st->GetStIdx().FullIdx() != 0, "the fullIdx of st's stIdx should not equal 0");
  CallNode *callStmt = static_cast<CallNode*>(mirBuilder->CreateStmtCall(st->GetStIdx().FullIdx(), nOpnds));
  currentBlock->AddStatement(callStmt);
  PrimType promotedPrimType = intrinNode.GetPrimType() == PTY_u1 ? PTY_u32 : intrinNode.GetPrimType();
  BaseNode *drRetSt = mirBuilder->CreateExprRegread(promotedPrimType, -kSregRetval0);
  /*
    * for safty dassign the return value to a register and return the dread to that register
    * to avoid such code:
    * call $__js_int32 (addrof ptr %temp_var_8 0)
    * call $__jsop_getelem (addrof a32 %temp_var_9 0, addrof a32 $arr 0, dread i32 %%retval 0)
    * for many target, the first actual parameter and return value would use R0, which would cause the above
    * case fail
    */
  PregIdx tmpRegIdx = GetCurrentFunc()->GetPregTab()->CreatePreg(promotedPrimType);
  RegassignNode *dstoReg = mirBuilder->CreateStmtRegassign(promotedPrimType, tmpRegIdx, drRetSt);
  currentBlock->AddStatement(dstoReg);
  RegreadNode *outDsNode = mirBuilder->CreateExprRegread(promotedPrimType, tmpRegIdx);
  return outDsNode;
}

StmtNode *CGLowerer::CreateStmtCallWithReturnValue(const IntrinsicopNode &intrinNode, const MIRSymbol &ret,
                                                   PUIdx bFunc, BaseNode *extraInfo) {
  MapleVector<BaseNode*> args(mirBuilder->GetCurrentFuncCodeMpAllocator()->Adapter());
  for (size_t i = 0; i < intrinNode.NumOpnds(); ++i) {
    args.push_back(intrinNode.Opnd(i));
  }
  if (extraInfo != nullptr) {
    args.push_back(extraInfo);
  }
  return mirBuilder->CreateStmtCallAssigned(bFunc, args, &ret, OP_callassigned);
}

StmtNode *CGLowerer::CreateStmtCallWithReturnValue(const IntrinsicopNode &intrinNode, PregIdx retpIdx, PUIdx bFunc,
                                                   BaseNode *extraInfo) {
  MapleVector<BaseNode*> args(mirBuilder->GetCurrentFuncCodeMpAllocator()->Adapter());
  for (size_t i = 0; i < intrinNode.NumOpnds(); ++i) {
    args.push_back(intrinNode.Opnd(i));
  }
  if (extraInfo != nullptr) {
    args.push_back(extraInfo);
  }
  return mirBuilder->CreateStmtCallRegassigned(bFunc, args, retpIdx, OP_callassigned);
}

BaseNode *CGLowerer::LowerIntrinJavaMerge(const BaseNode &parent, IntrinsicopNode &intrinNode) {
  BaseNode *resNode = &intrinNode;
  CHECK_FATAL(intrinNode.GetNumOpnds() > 0, "invalid JAVA_MERGE intrinsic node");
  BaseNode *candidate = intrinNode.Opnd(0);
  ASSERT(candidate != nullptr, "candidate should not be nullptr");
  resNode = candidate;
  if (parent.GetOpCode() == OP_regassign) {
    PrimType sTyp = resNode->GetPrimType();
    auto &regAssign = static_cast<const RegassignNode&>(parent);
    PrimType pType = GetCurrentFunc()->GetPregTab()->PregFromPregIdx(regAssign.GetRegIdx())->GetPrimType();
    if (sTyp != pType) {
      resNode = MergeToCvtType(pType, sTyp, *resNode);
    }
    return resNode;
  }
  if (parent.GetOpCode() == OP_dassign) {
    auto &dassign = static_cast<const DassignNode&>(parent);
    if (candidate->GetOpCode() == OP_constval) {
      MIRSymbol *dest = GetCurrentFunc()->GetLocalOrGlobalSymbol(dassign.GetStIdx());
      MIRType *toType = dest->GetType();
      PrimType dTyp = toType->GetPrimType();
      PrimType sTyp = resNode->GetPrimType();
      if (dTyp != sTyp) {
        resNode = MergeToCvtType(dTyp, sTyp, *resNode);
      }
      return resNode;
    }
    CHECK_FATAL((candidate->GetOpCode() == OP_dread) || (candidate->GetOpCode() == OP_regread),
                "candidate's opcode should be OP_dread or OP_regread");
    bool differentLocation =
        (candidate->GetOpCode() == OP_dread)
        ? !IsAccessingTheSameMemoryLocation(dassign, static_cast<DreadNode&>(*candidate))
        : !IsAccessingTheSameMemoryLocation(dassign, static_cast<RegreadNode&>(*candidate), *this);
    if (differentLocation) {
      bool simpleMove = false;
      /* res_node already contains the 0-th operand. */
      for (size_t i = 1; i < intrinNode.GetNumOpnds(); ++i) {
        candidate = intrinNode.Opnd(i);
        ASSERT(candidate != nullptr, "candidate should not be nullptr");
        bool sameLocation =
            (candidate->GetOpCode() == OP_dread)
            ? IsAccessingTheSameMemoryLocation(dassign, static_cast<DreadNode&>(*candidate))
            : IsAccessingTheSameMemoryLocation(dassign, static_cast<RegreadNode&>(*candidate), *this);
        if (sameLocation) {
          simpleMove = true;
          resNode = candidate;
          break;
        }
      }
      if (!simpleMove) {
        /* if source and destination types don't match, insert 'retype' */
        MIRSymbol *dest = GetCurrentFunc()->GetLocalOrGlobalSymbol(dassign.GetStIdx());
        MIRType *toType = dest->GetType();
        PrimType dTyp = toType->GetPrimType();
        CHECK_FATAL((dTyp != PTY_agg) && (dassign.GetFieldID() <= 0),
                    "dType should not be PTY_agg and dassign's filedId <= 0");
        PrimType sType = resNode->GetPrimType();
        if (dTyp != sType) {
          resNode = MergeToCvtType(dTyp, sType, *resNode);
        }
      }
    }
    return resNode;
  }
  CHECK_FATAL(false, "should not run here");
  return resNode;
}

BaseNode *CGLowerer::LowerIntrinJavaArrayLength(const BaseNode &parent, IntrinsicopNode &intrinNode){
  BaseNode *resNode = &intrinNode;
  PUIdx bFunc = GetBuiltinToUse(intrinNode.GetIntrinsic());
  CHECK_FATAL(bFunc != kFuncNotFound, "bFunc should not be kFuncNotFound");
  MIRFunction *biFunc = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(bFunc);

  BaseNode *arrAddr = intrinNode.Opnd(0);
  ASSERT(arrAddr != nullptr, "arrAddr should not be nullptr");
  if (((arrAddr->GetPrimType() == PTY_a64) || (arrAddr->GetPrimType() == PTY_ref)) &&
      ((parent.GetOpCode() == OP_regassign) || (parent.GetOpCode() == OP_dassign) || (parent.GetOpCode() == OP_ge))) {
    MIRType *addrType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(TyIdx(arrAddr->GetPrimType()));
    MIRIntConst *arrayHeaderNode = GlobalTables::GetIntConstTable().GetOrCreateIntConst(
        static_cast<int64>(AArch64RTSupport::kArrayLengthOffset), *addrType);
    BaseNode *arrayHeaderCstNode = mirModule.CurFuncCodeMemPool()->New<ConstvalNode>(arrayHeaderNode);
    arrayHeaderCstNode->SetPrimType(arrAddr->GetPrimType());

    BaseNode *refLenAddr = mirBuilder->CreateExprBinary(OP_add, *addrType, arrAddr, arrayHeaderCstNode);
    MIRType *infoLenType = GlobalTables::GetTypeTable().GetInt32();
    MIRType *ptrType = beCommon.BeGetOrCreatePointerType(*infoLenType);
    resNode = mirBuilder->CreateExprIread(*infoLenType, *ptrType, 0, refLenAddr);
    auto curFunc = mirModule.CurFunction();
    std::string suffix = std::to_string(curFunc->GetLabelTab()->GetLabelTableSize());
    GStrIdx labelStrIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName("__label_nonnull_" + suffix);
    LabelIdx labIdx = curFunc->GetLabelTab()->AddLabel(labelStrIdx);
    LabelNode *labelNonNull = mirBuilder->CreateStmtLabel(labIdx);

    BaseNode *cond = mirBuilder->CreateExprCompare(OP_ne,
                                                   *GlobalTables::GetTypeTable().GetUInt1(),
                                                   *GlobalTables::GetTypeTable().GetRef(), arrAddr,
                                                   mirBuilder->CreateIntConst(0, PTY_ref));
    CondGotoNode *brtureNode = mirBuilder->CreateStmtCondGoto(cond, OP_brtrue, labIdx);

    MIRFunction *newFunc =
        mirBuilder->GetOrCreateFunction("MCC_ThrowNullArrayNullPointerException",
                                        GlobalTables::GetTypeTable().GetVoid()->GetTypeIndex());
    MapleVector<BaseNode*> args(mirBuilder->GetCurrentFuncCodeMpAllocator()->Adapter());
    StmtNode *call = mirBuilder->CreateStmtCallAssigned(newFunc->GetPuidx(), args, nullptr, OP_callassigned);

    currentBlock->AddStatement(brtureNode);
    currentBlock->AppendStatementsFromBlock(*LowerCallAssignedStmt(*call));
    currentBlock->AddStatement(labelNonNull);
    return resNode;
  }

  if (parent.GetOpCode() == OP_regassign) {
    auto &regAssign = static_cast<const RegassignNode&>(parent);
    StmtNode *biCall = CreateStmtCallWithReturnValue(intrinNode, regAssign.GetRegIdx(), bFunc);
    currentBlock->AppendStatementsFromBlock(*LowerCallAssignedStmt(*biCall));
    PrimType pType = GetCurrentFunc()->GetPregTab()->PregFromPregIdx(regAssign.GetRegIdx())->GetPrimType();
    resNode = mirBuilder->CreateExprRegread(pType, regAssign.GetRegIdx());
    return resNode;
  }

  if (parent.GetOpCode() == OP_dassign) {
    auto &dassign = static_cast<const DassignNode&>(parent);
    MIRSymbol *ret = GetCurrentFunc()->GetLocalOrGlobalSymbol(dassign.GetStIdx());
    StmtNode *biCall = CreateStmtCallWithReturnValue(intrinNode, *ret, bFunc);
    currentBlock->AppendStatementsFromBlock(*LowerCallAssignedStmt(*biCall));
    resNode = mirBuilder->CreateExprDread(*biFunc->GetReturnType(), 0, *ret);
    return resNode;
  }
  CHECK_FATAL(false, "should not run here");
  return resNode;
}

BaseNode *CGLowerer::LowerIntrinsicop(const BaseNode &parent, IntrinsicopNode &intrinNode) {
  BaseNode *resNode = &intrinNode;
  if (intrinNode.GetIntrinsic() == INTRN_JAVA_MERGE) {
    resNode = LowerIntrinJavaMerge(parent, intrinNode);
  } else if (intrinNode.GetIntrinsic() == INTRN_JAVA_ARRAY_LENGTH) {
    resNode = LowerIntrinJavaArrayLength(parent, intrinNode);
  }

  return resNode;
}

void CGLowerer::ProcessClassInfo(MIRType &classType, bool &classInfoFromRt, std::string &classInfo){
  MIRPtrType &ptrType = static_cast<MIRPtrType&>(classType);
  MIRType *pType = ptrType.GetPointedType();
  CHECK_FATAL(pType != nullptr, "Class type not found for INTRN_JAVA_CONST_CLASS");
  MIRType *typeScalar = nullptr;

  if (pType->GetKind() == kTypeScalar) {
    typeScalar = pType;
  } else if (classType.GetKind() == kTypeScalar) {
    typeScalar = &classType;
  }
  if (typeScalar != nullptr) {
    std::string eName(GetPrimTypeJavaName(typeScalar->GetPrimType()));
    classInfo = PRIMITIVECLASSINFO_PREFIX_STR + eName;
  }
  if ((pType->GetKind() == kTypeByName) || (pType->GetKind() == kTypeClass) || (pType->GetKind() == kTypeInterface)) {
    MIRStructType *classTypeSecond = static_cast<MIRStructType*>(pType);
    classInfo = CLASSINFO_PREFIX_STR + classTypeSecond->GetName();
  } else if ((pType->GetKind() == kTypeArray) || (pType->GetKind() == kTypeJArray)) {
    MIRJarrayType *jarrayType = static_cast<MIRJarrayType*>(pType);
    CHECK_FATAL(jarrayType != nullptr, "jarrayType is null in CGLowerer::LowerIntrinsicopWithType");
    std::string baseName = jarrayType->GetJavaName();
    if (jarrayType->IsPrimitiveArray() && (jarrayType->GetDim() <= kThreeDimArray)) {
      classInfo = PRIMITIVECLASSINFO_PREFIX_STR + baseName;
    } else if (arrayNameForLower::kArrayBaseName.find(baseName) != arrayNameForLower::kArrayBaseName.end()) {
      classInfo = CLASSINFO_PREFIX_STR + baseName;
    } else {
      classInfoFromRt = true;
      classInfo = baseName;
    }
  }
}

BaseNode *CGLowerer::GetBaseNodeFromCurFunc(MIRFunction &curFunc, bool isFromJarray) {
  BaseNode *baseNode = nullptr;
  if (curFunc.IsStatic()) {
    /*
     * it's a static function.
     * pass caller functions's classinfo directly
     */
    std::string callerName = CLASSINFO_PREFIX_STR;
    callerName += mirModule.CurFunction()->GetBaseClassName();
    GStrIdx strIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(callerName);
    MIRSymbol *callerClassInfoSym = GlobalTables::GetGsymTable().GetSymbolFromStrIdx(strIdx);
    if (callerClassInfoSym == nullptr) {
      if (isFromJarray) {
        MIRType *mType = GlobalTables::GetTypeTable().GetVoidPtr();
        CHECK_FATAL(mType != nullptr, "type is null in CGLowerer::LowerJarrayMalloc");
        callerClassInfoSym = mirBuilder->CreateGlobalDecl(callerName.c_str(), *mType);
        callerClassInfoSym->SetStorageClass(kScExtern);
      } else {
        callerClassInfoSym = GlobalTables::GetGsymTable().CreateSymbol(kScopeGlobal);
        callerClassInfoSym->SetNameStrIdx(strIdx);
        callerClassInfoSym->SetStorageClass(kScGlobal);
        callerClassInfoSym->SetSKind(kStVar);
        /* it must be a local symbol */
        GlobalTables::GetGsymTable().AddToStringSymbolMap(*callerClassInfoSym);
        callerClassInfoSym->SetTyIdx((TyIdx)PTY_ptr);
      }
    }

    baseNode = mirBuilder->CreateExprAddrof(0, *callerClassInfoSym);
  } else {
    /*
     * it's an instance function.
     * pass caller function's this pointer
     */
    CHECK_FATAL(curFunc.GetFormalCount() != 0, "index out of range in CGLowerer::GetBaseNodeFromCurFunc");
    MIRSymbol *formalSt = curFunc.GetFormal(0);
    if (formalSt->IsPreg()) {
      if (isFromJarray) {
        baseNode = mirBuilder->CreateExprRegread(formalSt->GetType()->GetPrimType(),
            curFunc.GetPregTab()->GetPregIdxFromPregno(formalSt->GetPreg()->GetPregNo()));
      } else {
        CHECK_FATAL(curFunc.GetParamSize() != 0, "index out of range in CGLowerer::GetBaseNodeFromCurFunc");
        baseNode = mirBuilder->CreateExprRegread((curFunc.GetNthParamType(0))->GetPrimType(),
            curFunc.GetPregTab()->GetPregIdxFromPregno(formalSt->GetPreg()->GetPregNo()));
      }
    } else {
      baseNode = mirBuilder->CreateExprDread(*formalSt);
    }
  }
  return baseNode;
}

BaseNode *CGLowerer::GetClassInfoExprFromRuntime(const std::string &classInfo) {
  /*
   * generate runtime call to get class information
   * jclass __mrt_getclass(jobject caller, const char *name)
   * if the calling function is an instance function, it's the calling obj
   * if the calling function is a static function, it's the calling class
   */
  BaseNode *classInfoExpr = nullptr;
  PUIdx getClassFunc = GetBuiltinToUse(INTRN_JAVA_GET_CLASS);
  CHECK_FATAL(getClassFunc != kFuncNotFound, "classfunc is not found");
  /* return jclass */
  MIRType *voidPtrType = GlobalTables::GetTypeTable().GetPtr();
  MIRSymbol *ret0 = CreateNewRetVar(*voidPtrType, kIntrnRetValPrefix);

  BaseNode *arg0 = GetBaseNodeFromCurFunc(*mirModule.CurFunction(), false);
  BaseNode *arg1 = nullptr;
  /* classname */
  std::string klassJavaDescriptor;
  NameMangler::DecodeMapleNameToJavaDescriptor(classInfo, klassJavaDescriptor);
  UStrIdx classNameStrIdx = GlobalTables::GetUStrTable().GetOrCreateStrIdxFromName(klassJavaDescriptor);
  arg1 = mirModule.GetMemPool()->New<ConststrNode>(classNameStrIdx);
  arg1->SetPrimType(PTY_ptr);

  MapleVector<BaseNode*> args(mirBuilder->GetCurrentFuncCodeMpAllocator()->Adapter());
  args.push_back(arg0);
  args.push_back(arg1);
  StmtNode *getClassCall = mirBuilder->CreateStmtCallAssigned(getClassFunc, args, ret0, OP_callassigned);
  currentBlock->AppendStatementsFromBlock(*LowerCallAssignedStmt(*getClassCall));
  classInfoExpr = mirBuilder->CreateExprDread(*voidPtrType, 0, *ret0);
  return classInfoExpr;
}

BaseNode *CGLowerer::GetClassInfoExpr(const std::string &classInfo) {
  BaseNode *classInfoExpr = nullptr;
  GStrIdx strIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(classInfo);
  MIRSymbol *classInfoSym = GlobalTables::GetGsymTable().GetSymbolFromStrIdx(strIdx);
  if (classInfoSym != nullptr) {
    classInfoExpr = mirBuilder->CreateExprAddrof(0, *classInfoSym);
  } else {
    classInfoSym = GlobalTables::GetGsymTable().CreateSymbol(kScopeGlobal);
    classInfoSym->SetNameStrIdx(strIdx);
    classInfoSym->SetStorageClass(kScGlobal);
    classInfoSym->SetSKind(kStVar);
    if (CGOptions::IsPIC()) {
      classInfoSym->SetStorageClass(kScExtern);
    } else {
      classInfoSym->SetAttr(ATTR_weak);
    }
    GlobalTables::GetGsymTable().AddToStringSymbolMap(*classInfoSym);
    classInfoSym->SetTyIdx((TyIdx)PTY_ptr);

    classInfoExpr = mirBuilder->CreateExprAddrof(0, *classInfoSym);
  }
  return classInfoExpr;
}

BaseNode *CGLowerer::LowerIntrinsicopWithType(const BaseNode &parent, IntrinsicopNode &intrinNode) {
  BaseNode *resNode = &intrinNode;
  if ((intrinNode.GetIntrinsic() == INTRN_JAVA_CONST_CLASS) || (intrinNode.GetIntrinsic() == INTRN_JAVA_INSTANCE_OF)) {
    PUIdx bFunc = GetBuiltinToUse(intrinNode.GetIntrinsic());
    CHECK_FATAL(bFunc != kFuncNotFound, "bFunc not founded");
    MIRFunction *biFunc = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(bFunc);
    MIRType *classType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(intrinNode.GetTyIdx());
    std::string classInfo;
    BaseNode *classInfoExpr = nullptr;
    bool classInfoFromRt = false;  /* whether the classinfo is generated by RT */
    ProcessClassInfo(*classType, classInfoFromRt, classInfo);
    if (classInfoFromRt) {
      classInfoExpr = GetClassInfoExprFromRuntime(classInfo);
    } else {
      classInfoExpr = GetClassInfoExpr(classInfo);
      LowerTypePtr(*classInfoExpr);
    }

    if (intrinNode.GetIntrinsic() == INTRN_JAVA_CONST_CLASS) {
        CHECK_FATAL(classInfoExpr != nullptr, "classInfoExpr should not be nullptr");
        if ((classInfoExpr->GetPrimType() == PTY_ptr) || (classInfoExpr->GetPrimType() == PTY_ref)) {
          classInfoExpr->SetPrimType(LOWERED_PTR_TYPE);
        }
        resNode = classInfoExpr;
        return resNode;
      }

    if (parent.GetOpCode() == OP_regassign) {
      auto &regAssign = static_cast<const RegassignNode&>(parent);
      StmtNode *biCall = CreateStmtCallWithReturnValue(intrinNode, regAssign.GetRegIdx(), bFunc, classInfoExpr);
      currentBlock->AppendStatementsFromBlock(*LowerCallAssignedStmt(*biCall));
      PrimType pTyp = GetCurrentFunc()->GetPregTab()->PregFromPregIdx(regAssign.GetRegIdx())->GetPrimType();
      resNode = mirBuilder->CreateExprRegread(pTyp, regAssign.GetRegIdx());
      return resNode;
    }

    if (parent.GetOpCode() == OP_dassign) {
      auto &dassign = static_cast<const DassignNode&>(parent);
      MIRSymbol *ret = GetCurrentFunc()->GetLocalOrGlobalSymbol(dassign.GetStIdx());
      StmtNode *biCall = CreateStmtCallWithReturnValue(intrinNode, *ret, bFunc, classInfoExpr);
      currentBlock->AppendStatementsFromBlock(*LowerCallAssignedStmt(*biCall));
      resNode = mirBuilder->CreateExprDread(*biFunc->GetReturnType(), 0, *ret);
      return resNode;
    }
    CHECK_FATAL(false, "should not run here");
  }
  CHECK_FATAL(false, "should not run here");
  return resNode;
}

BaseNode *CGLowerer::LowerIntrinsicop(const BaseNode &parent, IntrinsicopNode &intrinNode, BlockNode &newBlk) {
  for (size_t i = 0; i < intrinNode.GetNumOpnds(); ++i) {
    intrinNode.SetOpnd(LowerExpr(intrinNode, *intrinNode.Opnd(i), newBlk), i);
  }

  MIRIntrinsicID intrnID = intrinNode.GetIntrinsic();
  IntrinDesc &intrinDesc = IntrinDesc::intrinTable[intrnID];
  if (intrinDesc.IsJS()) {
    return LowerJavascriptIntrinsicop(intrinNode, intrinDesc);
  }
  if (intrinDesc.IsJava()) {
    return LowerIntrinsicop(parent, intrinNode);
  }
  if (intrinNode.GetIntrinsic() == INTRN_MPL_READ_OVTABLE_ENTRY_LAZY) {
    return &intrinNode;
  }
  CHECK_FATAL(false, "unexpected intrinsic type in CGLowerer::LowerIntrinsicop");
  return &intrinNode;
}

BaseNode *CGLowerer::LowerIntrinsicopwithtype(const BaseNode &parent, IntrinsicopNode &intrinNode, BlockNode &blk) {
  for (size_t i = 0; i < intrinNode.GetNumOpnds(); ++i) {
    intrinNode.SetOpnd(LowerExpr(intrinNode, *intrinNode.Opnd(i), blk), i);
  }
  MIRIntrinsicID intrnID = intrinNode.GetIntrinsic();
  IntrinDesc *intrinDesc = &IntrinDesc::intrinTable[intrnID];
  CHECK_FATAL(!intrinDesc->IsJS(), "intrinDesc should not be js");
  if (intrinDesc->IsJava()) {
    return LowerIntrinsicopWithType(parent, intrinNode);
  }
  CHECK_FATAL(false, "should not run here");
  return &intrinNode;
}

StmtNode *CGLowerer::LowerIntrinsicMplClearStack(IntrinsiccallNode &intrincall, BlockNode &newBlk) {
  StmtNode *newStmt = mirBuilder->CreateStmtIassign(
      *beCommon.BeGetOrCreatePointerType(*GlobalTables::GetTypeTable().GetUInt8()), 0,
      intrincall.Opnd(0), mirBuilder->GetConstUInt8(0));
  newBlk.AddStatement(newStmt);

  BaseNode *length = intrincall.Opnd(1);
  PrimType pType = PTY_i64;
  PregIdx pIdx = GetCurrentFunc()->GetPregTab()->CreatePreg(pType);
  newStmt = mirBuilder->CreateStmtRegassign(pType, pIdx, mirBuilder->CreateIntConst(1, pType));
  newBlk.AddStatement(newStmt);
  MIRFunction *func = GetCurrentFunc();

  const std::string &name = func->GetName() + std::string("_Lalloca_");
  LabelIdx label1 = GetCurrentFunc()->GetOrCreateLableIdxFromName(name + std::to_string(labelIdx++));
  LabelIdx label2 = GetCurrentFunc()->GetOrCreateLableIdxFromName(name + std::to_string(labelIdx++));

  newStmt = mirBuilder->CreateStmtGoto(OP_goto, label2);
  newBlk.AddStatement(newStmt);
  LabelNode *ln = mirBuilder->CreateStmtLabel(label1);
  newBlk.AddStatement(ln);

  RegreadNode *regLen = mirBuilder->CreateExprRegread(pType, pIdx);

  BinaryNode *addr = mirBuilder->CreateExprBinary(OP_add,
                                                  *GlobalTables::GetTypeTable().GetAddr64(),
                                                  intrincall.Opnd(0), regLen);

  newStmt = mirBuilder->CreateStmtIassign(*beCommon.BeGetOrCreatePointerType(*GlobalTables::GetTypeTable().GetUInt8()),
                                          0, addr, mirBuilder->GetConstUInt8(0));
  newBlk.AddStatement(newStmt);

  BinaryNode *subLen = mirBuilder->CreateExprBinary(
      OP_add, *GlobalTables::GetTypeTable().GetPrimType(pType), regLen, mirBuilder->CreateIntConst(1, pType));
  newStmt = mirBuilder->CreateStmtRegassign(pType, pIdx, subLen);
  newBlk.AddStatement(newStmt);

  ln = mirBuilder->CreateStmtLabel(label2);
  newBlk.AddStatement(ln);

  CompareNode *cmpExp =
      mirBuilder->CreateExprCompare(OP_lt, *GlobalTables::GetTypeTable().GetUInt32(),
                                    *GlobalTables::GetTypeTable().GetPrimType(pType), regLen, length);
  newStmt = mirBuilder->CreateStmtCondGoto(cmpExp, OP_brtrue, label1);

  return newStmt;
}

StmtNode *CGLowerer::LowerIntrinsicRCCall(IntrinsiccallNode &intrincall) {
  /* If GCONLY enabled, lowering RC intrinsics in another way. */
  MIRIntrinsicID intrnID = intrincall.GetIntrinsic();
  IntrinDesc *intrinDesc = &IntrinDesc::intrinTable[intrnID];

  /* convert intrinsic call into function call. */
  if (intrinFuncIDs.find(intrinDesc) == intrinFuncIDs.end()) {
    /* add funcid into map */
    intrinFuncIDs[intrinDesc] = mirBuilder->GetOrCreateFunction(intrinDesc->name, TyIdx(PTY_void))->GetPuidx();
  }
  CallNode *callStmt = mirModule.CurFuncCodeMemPool()->New<CallNode>(mirModule, OP_call);
  callStmt->SetPUIdx(intrinFuncIDs.at(intrinDesc));
  for (size_t i = 0; i < intrincall.GetNopndSize(); ++i) {
    callStmt->GetNopnd().push_back(intrincall.GetNopndAt(i));
    callStmt->SetNumOpnds(callStmt->GetNumOpnds() + 1);
  }
  return callStmt;
}

void CGLowerer::LowerArrayStore(IntrinsiccallNode &intrincall, BlockNode &newBlk) {
  bool needCheckStore = true;
  BaseNode *arrayNode = intrincall.Opnd(0);
  MIRType *arrayElemType = GetArrayNodeType(*arrayNode);
  BaseNode *valueNode = intrincall.Opnd(kNodeThirdOpnd);
  MIRType *valueRealType = GetArrayNodeType(*valueNode);
  if ((arrayElemType != nullptr) && (valueRealType != nullptr) && (arrayElemType->GetKind() == kTypeClass) &&
      static_cast<MIRClassType*>(arrayElemType)->IsFinal() && (valueRealType->GetKind() == kTypeClass) &&
      static_cast<MIRClassType*>(valueRealType)->IsFinal() &&
      (valueRealType->GetTypeIndex() == arrayElemType->GetTypeIndex())) {
    needCheckStore = false;
  }

  if (needCheckStore) {
    MIRFunction *fn = mirBuilder->GetOrCreateFunction("MCC_Reflect_Check_Arraystore", TyIdx(PTY_void));
    MapleVector<BaseNode*> args(mirBuilder->GetCurrentFuncCodeMpAllocator()->Adapter());
    args.push_back(intrincall.Opnd(0));
    args.push_back(intrincall.Opnd(kNodeThirdOpnd));
    StmtNode *checkStoreStmt = mirBuilder->CreateStmtCall(fn->GetPuidx(), args);
    newBlk.AddStatement(checkStoreStmt);
  }
}

StmtNode *CGLowerer::LowerDefaultIntrinsicCall(IntrinsiccallNode &intrincall, MIRSymbol &st, MIRFunction &fn) {
  MIRIntrinsicID intrnID = intrincall.GetIntrinsic();
  IntrinDesc *intrinDesc = &IntrinDesc::intrinTable[intrnID];
  std::vector<TyIdx> funcTyVec;
  std::vector<TypeAttrs> fnTaVec;
  MapleVector<BaseNode*> &nOpnds = intrincall.GetNopnd();
  MIRType *retTy = intrinDesc->GetReturnType();
  CHECK_FATAL(retTy != nullptr, "retTy should not be nullptr");
  if (retTy->GetKind() == kTypeStruct) {
    funcTyVec.push_back(beCommon.BeGetOrCreatePointerType(*retTy)->GetTypeIndex());
    fnTaVec.push_back(TypeAttrs());
    fn.SetReturnStruct();
  }
  for (uint32 i = 0; i < nOpnds.size(); ++i) {
    MIRType *argTy = intrinDesc->GetArgType(i);
    CHECK_FATAL(argTy != nullptr, "argTy should not be nullptr");
    if (argTy->GetKind() == kTypeStruct) {
      funcTyVec.push_back(GlobalTables::GetTypeTable().GetTypeFromTyIdx(PTY_a32)->GetTypeIndex());
      fnTaVec.push_back(TypeAttrs());
      BaseNode *addrNode = beCommon.GetAddressOfNode(*nOpnds[i]);
      CHECK_FATAL(addrNode != nullptr, "can not get address");
      nOpnds[i] = addrNode;
    } else {
      funcTyVec.push_back(argTy->GetTypeIndex());
      fnTaVec.push_back(TypeAttrs());
    }
  }
  MIRType *funcType = beCommon.BeGetOrCreateFunctionType(retTy->GetTypeIndex(), funcTyVec, fnTaVec);
  st.SetTyIdx(funcType->GetTypeIndex());
  fn.SetMIRFuncType(static_cast<MIRFuncType*>(funcType));
  if (retTy->GetKind() == kTypeStruct) {
    fn.SetReturnTyIdx((TyIdx) PTY_void);
  } else {
    fn.SetReturnTyIdx(retTy->GetTypeIndex());
  }
  return static_cast<CallNode*>(mirBuilder->CreateStmtCall(fn.GetPuidx(), nOpnds));
}

StmtNode *CGLowerer::LowerIntrinsicMplCleanupLocalRefVarsSkip(IntrinsiccallNode &intrincall) {
  MIRFunction *mirFunc = mirModule.CurFunction();
  BaseNode *skipExpr = intrincall.Opnd(intrincall.NumOpnds() - 1);

  CHECK_FATAL(skipExpr != nullptr, "should be dread");
  CHECK_FATAL(skipExpr->GetOpCode() == OP_dread, "should be dread");
  DreadNode *refNode = static_cast<DreadNode*>(skipExpr);
  MIRSymbol *skipSym = mirFunc->GetLocalOrGlobalSymbol(refNode->GetStIdx());
  if (skipSym->GetAttr(ATTR_localrefvar)) {
    mirFunc->InsertMIRSymbol(skipSym);
  }
  return &intrincall;
}

StmtNode *CGLowerer::LowerIntrinsiccall(IntrinsiccallNode &intrincall, BlockNode &newBlk) {
  MIRIntrinsicID intrnID = intrincall.GetIntrinsic();
  for (size_t i = 0; i < intrincall.GetNumOpnds(); ++i) {
    intrincall.SetOpnd(LowerExpr(intrincall, *intrincall.Opnd(i), newBlk), i);
  }
  if (intrnID == INTRN_MPL_CLEAR_STACK) {
    return LowerIntrinsicMplClearStack(intrincall, newBlk);
  }
  IntrinDesc *intrinDesc = &IntrinDesc::intrinTable[intrnID];
  if (intrinDesc->IsSpecial()) {
    /* For special intrinsics we leave them to CGFunc::SelectIntrinCall() */
    return &intrincall;
  }
  /* default lowers intrinsic call to real function call. */
  MIRSymbol *st = GlobalTables::GetGsymTable().CreateSymbol(kScopeGlobal);
  CHECK_FATAL(intrinDesc->name != nullptr, "intrinsic's name should not be nullptr");
  const std::string name = intrinDesc->name;
  st->SetNameStrIdx(name);
  st->SetStorageClass(kScText);
  st->SetSKind(kStFunc);
  MIRFunction *fn = mirBuilder->GetOrCreateFunction(intrinDesc->name, TyIdx(0));
  st->SetFunction(fn);
  return LowerDefaultIntrinsicCall(intrincall, *st, *fn);
}

StmtNode *CGLowerer::LowerSyncEnterSyncExit(StmtNode &stmt) {
  CHECK_FATAL(stmt.GetOpCode() == OP_syncenter || stmt.GetOpCode() == OP_syncexit,
              "stmt's opcode should be OP_syncenter or OP_syncexit");

  auto &nStmt = static_cast<NaryStmtNode&>(stmt);
  BuiltinFunctionID id;
  if (nStmt.GetOpCode() == OP_syncenter) {
    if (nStmt.NumOpnds() == 1) {
      /* Just as ParseNaryStmt do for syncenter */
      MIRType &intType = *GlobalTables::GetTypeTable().GetTypeFromTyIdx((TyIdx)PTY_i32);
      /* default 2 for __sync_enter_fast() */
      MIRIntConst *intConst = GlobalTables::GetIntConstTable().GetOrCreateIntConst(2, intType);
      ConstvalNode *exprConst = mirModule.GetMemPool()->New<ConstvalNode>();
      exprConst->SetPrimType(PTY_i32);
      exprConst->SetConstVal(intConst);
      nStmt.GetNopnd().push_back(exprConst);
      nStmt.SetNumOpnds(nStmt.GetNopndSize());
    }
    CHECK_FATAL(nStmt.NumOpnds() == kOperandNumBinary, "wrong args for syncenter");
    CHECK_FATAL(nStmt.Opnd(1)->GetOpCode() == OP_constval, "wrong 2nd arg type for syncenter");
    ConstvalNode *cst = static_cast<ConstvalNode*>(nStmt.GetNopndAt(1));
    MIRIntConst *intConst = safe_cast<MIRIntConst>(cst->GetConstVal());
    switch (static_cast<uint32>(intConst->GetValue())) {
      case kMCCSyncEnterFast0:
        id = INTRN_FIRST_SYNC_ENTER;
        break;
      case kMCCSyncEnterFast1:
        id = INTRN_SECOND_SYNC_ENTER;
        break;
      case kMCCSyncEnterFast2:
        id = INTRN_THIRD_SYNC_ENTER;
        break;
      case kMCCSyncEnterFast3:
        id = INTRN_FOURTH_SYNC_ENTER;
        break;
      default:
        CHECK_FATAL(false, "wrong kind for syncenter");
        break;
    }
  } else {
    CHECK_FATAL(nStmt.NumOpnds() == 1, "wrong args for syncexit");
    id = INTRN_YNC_EXIT;
  }
  PUIdx bFunc = GetBuiltinToUse(id);
  CHECK_FATAL(bFunc != kFuncNotFound, "bFunc should be found");

  MapleVector<BaseNode*> args(mirBuilder->GetCurrentFuncCodeMpAllocator()->Adapter());
  args.push_back(nStmt.Opnd(0));
  return mirBuilder->CreateStmtCall(bFunc, args);
}

PUIdx CGLowerer::GetBuiltinToUse(BuiltinFunctionID id) const {
  /*
   * use std::vector & linear search as the number of entries is small.
   * we may revisit it if the number of entries gets larger.
   */
  for (const auto &funcID : builtinFuncIDs) {
    if (funcID.first == id) {
      return funcID.second;
    }
  }
  return kFuncNotFound;
}

void CGLowerer::LowerGCMalloc(const BaseNode &node, const GCMallocNode &gcmalloc, BlockNode &blkNode, bool perm) {
  MIRFunction *func = mirBuilder->GetOrCreateFunction((perm ? "MCC_NewPermanentObject" : "MCC_NewObj_fixed_class"),
                                                      (TyIdx)(LOWERED_PTR_TYPE));
  /* Get the classinfo */
  MIRStructType *classType = static_cast<MIRStructType*>(
      GlobalTables::GetTypeTable().GetTypeFromTyIdx(gcmalloc.GetTyIdx()));
  std::string classInfoName = CLASSINFO_PREFIX_STR + classType->GetName();
  MIRSymbol *classSym = GlobalTables::GetGsymTable().GetSymbolFromStrIdx(
      GlobalTables::GetStrTable().GetStrIdxFromName(classInfoName));
  if (classSym == nullptr) {
    MIRType *pointerType = beCommon.BeGetOrCreatePointerType(*GlobalTables::GetTypeTable().GetVoid());
    classSym = mirBuilder->CreateGlobalDecl(classInfoName, *pointerType);
    classSym->SetStorageClass(kScExtern);
  }
  CallNode *callAssign = nullptr;
  auto *curFunc = mirModule.CurFunction();
  if (classSym->GetAttr(ATTR_abstract) || classSym->GetAttr(ATTR_interface)) {
    MIRFunction *funcSecond = mirBuilder->GetOrCreateFunction("MCC_Reflect_ThrowInstantiationError",
                                                             (TyIdx)(LOWERED_PTR_TYPE));
    BaseNode *arg = mirBuilder->CreateExprAddrof(0, *classSym);
    if (node.GetOpCode() == OP_dassign) {
      auto &dsNode = static_cast<const DassignNode&>(node);
      MIRSymbol *ret = curFunc->GetLocalOrGlobalSymbol(dsNode.GetStIdx());
      MapleVector<BaseNode*> args(mirBuilder->GetCurrentFuncCodeMpAllocator()->Adapter());
      args.push_back(arg);
      callAssign = mirBuilder->CreateStmtCallAssigned(funcSecond->GetPuidx(), args, ret, OP_callassigned);
    } else {
      CHECK_FATAL(node.GetOpCode() == OP_regassign, "regassign expected");
      callAssign = mirBuilder->CreateStmtCallRegassigned(
          funcSecond->GetPuidx(), static_cast<const RegassignNode&>(node).GetRegIdx(), OP_callassigned, arg);
    }
    blkNode.AppendStatementsFromBlock(*LowerCallAssignedStmt(*callAssign));
    return;
  }
  BaseNode *arg = mirBuilder->CreateExprAddrof(0, *classSym);

  if (node.GetOpCode() == OP_dassign) {
    MIRSymbol *ret = curFunc->GetLocalOrGlobalSymbol(static_cast<const DassignNode&>(node).GetStIdx());
    MapleVector<BaseNode*> args(mirBuilder->GetCurrentFuncCodeMpAllocator()->Adapter());
    args.push_back(arg);
    callAssign = mirBuilder->CreateStmtCallAssigned(func->GetPuidx(), args, ret, OP_callassigned);
  } else {
    CHECK_FATAL(node.GetOpCode() == OP_regassign, "regassign expected");
    callAssign = mirBuilder->CreateStmtCallRegassigned(
        func->GetPuidx(), static_cast<const RegassignNode&>(node).GetRegIdx(), OP_callassigned, arg);
  }
  blkNode.AppendStatementsFromBlock(*LowerCallAssignedStmt(*callAssign));
}

std::string CGLowerer::GetNewArrayFuncName(const uint32 elemSize, const bool perm) const {
  if (elemSize == 1) {
    return perm ? "MCC_NewPermArray8" : "MCC_NewArray8";
  }
  if (elemSize == 2) {
    return perm ? "MCC_NewPermArray16" : "MCC_NewArray16";
  }
  if (elemSize == 4) {
    return perm ? "MCC_NewPermArray32" : "MCC_NewArray32";
  }
  CHECK_FATAL((elemSize == 8), "Invalid elemSize.");
  return perm ? "MCC_NewPermArray64" : "MCC_NewArray64";
}

void CGLowerer::LowerJarrayMalloc(const StmtNode &stmt, const JarrayMallocNode &node, BlockNode &blkNode, bool perm) {
  /* Extract jarray type */
  TyIdx tyIdx = node.GetTyIdx();
  MIRType *type = GlobalTables::GetTypeTable().GetTypeFromTyIdx(tyIdx);
  CHECK_FATAL(type->GetKind() == kTypeJArray, "Type param of gcmallocjarray is not a MIRJarrayType");
  auto jaryType = static_cast<MIRJarrayType*>(type);
  CHECK_FATAL(jaryType != nullptr, "Type param of gcmallocjarray is not a MIRJarrayType");

  /* Inspect element type */
  MIRType *elemType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(jaryType->GetElemTyIdx());
  PrimType elemPrimType = elemType->GetPrimType();
  uint32 elemSize = GetPrimTypeSize(elemPrimType);
  if (elemType->GetKind() != kTypeScalar) {  /* element is reference */
    elemSize = AArch64RTSupport::kRefFieldSize;
  }

  std::string klassName = jaryType->GetJavaName();
  std::string arrayClassInfoName;
  bool isPredefinedArrayClass = false;
  if (jaryType->IsPrimitiveArray() && (jaryType->GetDim() <= kThreeDimArray)) {
    arrayClassInfoName = PRIMITIVECLASSINFO_PREFIX_STR + klassName;
    isPredefinedArrayClass = true;
  } else if (arrayNameForLower::kArrayKlassName.find(klassName) != arrayNameForLower::kArrayKlassName.end()) {
    arrayClassInfoName = CLASSINFO_PREFIX_STR + klassName;
    isPredefinedArrayClass = true;
  }

  std::string funcName;
  MapleVector<BaseNode*> args(mirModule.GetMPAllocator().Adapter());
  auto *curFunc = mirModule.CurFunction();
  if (isPredefinedArrayClass) {
    funcName = GetNewArrayFuncName(elemSize, perm);
    args.push_back(node.Opnd(0));                                   /* n_elems */
    GStrIdx strIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(arrayClassInfoName);
    MIRSymbol *arrayClassSym = GlobalTables::GetGsymTable().GetSymbolFromStrIdx(
        GlobalTables::GetStrTable().GetStrIdxFromName(arrayClassInfoName));
    if (arrayClassSym == nullptr) {
      arrayClassSym = GlobalTables::GetGsymTable().CreateSymbol(kScopeGlobal);
      arrayClassSym->SetNameStrIdx(strIdx);
      arrayClassSym->SetStorageClass(kScGlobal);
      arrayClassSym->SetSKind(kStVar);
      if (CGOptions::IsPIC()) {
        arrayClassSym->SetStorageClass(kScExtern);
      } else {
        arrayClassSym->SetAttr(ATTR_weak);
      }
      GlobalTables::GetGsymTable().AddToStringSymbolMap(*arrayClassSym);
      arrayClassSym->SetTyIdx((TyIdx)PTY_ptr);
    }
    args.push_back(mirBuilder->CreateExprAddrof(0, *arrayClassSym));
  } else {
    funcName = perm ? "MCC_NewPermanentArray" : "MCC_NewObj_flexible_cname";
    args.push_back(mirBuilder->CreateIntConst(elemSize, PTY_u32));  /* elem_size */
    args.push_back(node.Opnd(0));                                   /* n_elems */
    std::string klassJavaDescriptor;
    NameMangler::DecodeMapleNameToJavaDescriptor(klassName, klassJavaDescriptor);
    UStrIdx classNameStrIdx = GlobalTables::GetUStrTable().GetOrCreateStrIdxFromName(klassJavaDescriptor);
    ConststrNode *classNameExpr = mirModule.GetMemPool()->New<ConststrNode>(classNameStrIdx);
    classNameExpr->SetPrimType(PTY_ptr);
    args.push_back(classNameExpr);  /* class_name */
    args.push_back(GetBaseNodeFromCurFunc(*curFunc, true));
    /* set class flag --> wellKnownClassFlag maybe 0 */
    uint32 wellKnownClassFlag = GetWellKnownFrameWorksClassFlag(jaryType->GetJavaName());
    args.push_back(mirBuilder->CreateIntConst(static_cast<int32>(wellKnownClassFlag), PTY_u32));
  }
  MIRFunction *func = mirBuilder->GetOrCreateFunction(funcName, (TyIdx)(LOWERED_PTR_TYPE));
  CallNode *callAssign = nullptr;
  if (stmt.GetOpCode() == OP_dassign) {
    auto &dsNode = static_cast<const DassignNode&>(stmt);
    MIRSymbol *ret = curFunc->GetLocalOrGlobalSymbol(dsNode.GetStIdx());

    callAssign = mirBuilder->CreateStmtCallAssigned(func->GetPuidx(), args, ret, OP_callassigned);
  } else {
    auto &regNode = static_cast<const RegassignNode&>(stmt);
    callAssign = mirBuilder->CreateStmtCallRegassigned(func->GetPuidx(), args, regNode.GetRegIdx(), OP_callassigned);
  }
  blkNode.AppendStatementsFromBlock(*LowerCallAssignedStmt(*callAssign));
}

bool CGLowerer::IsIntrinsicCallHandledAtLowerLevel(MIRIntrinsicID intrinsic) {
  /* only INTRN_MPL_ATOMIC_EXCHANGE_PTR now. */
  return intrinsic == INTRN_MPL_ATOMIC_EXCHANGE_PTR;
}

void CGLowerer::LowerFunc(MIRFunction &func) {
  labelIdx = 0;
  SetCurrentFunc(&func);
  hasTry = false;
  LowerEntry(func);
  LowerPseudoRegs(func);
  BlockNode *origBody = func.GetBody();
  CHECK_FATAL(origBody != nullptr, "origBody should not be nullptr");

  BlockNode *newBody = LowerBlock(*origBody);
  func.SetBody(newBody);
  if (needBranchCleanup) {
    CleanupBranches(func);
  }

  if (mirModule.GetSrcLang() == kSrcLangJava && func.GetBody()->GetFirst() &&
      GenerateExceptionHandlingCode()) {
    LowerTryCatchBlocks(*func.GetBody());
  }
}
}  /* namespace maplebe */
