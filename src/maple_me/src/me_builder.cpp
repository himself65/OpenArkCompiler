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
#include "me_builder.h"
#include "ssa_mir_nodes.h"
#include "factory.h"

namespace maple {
using MeExprBuildFactory = FunctionFactory<Opcode, MeExpr*, const MeBuilder*, BaseNode&>;

MeExpr *MeBuilder::CreateMeExpr(int32 exprId, MeExpr &meExpr) const {
  MeExpr *resultExpr = nullptr;
  switch (meExpr.GetMeOp()) {
    case kMeOpIvar:
      resultExpr = New<IvarMeExpr>(exprId, static_cast<IvarMeExpr&>(meExpr));
      break;
    case kMeOpOp:
      resultExpr = New<OpMeExpr>(static_cast<OpMeExpr&>(meExpr), exprId);
      break;
    case kMeOpConst:
      resultExpr = New<ConstMeExpr>(exprId, static_cast<ConstMeExpr&>(meExpr).GetConstVal());
      break;
    case kMeOpConststr:
      resultExpr = New<ConststrMeExpr>(exprId, static_cast<ConststrMeExpr&>(meExpr).GetStrIdx());
      break;
    case kMeOpConststr16:
      resultExpr = New<Conststr16MeExpr>(exprId, static_cast<Conststr16MeExpr&>(meExpr).GetStrIdx());
      break;
    case kMeOpSizeoftype:
      resultExpr = New<SizeoftypeMeExpr>(exprId, static_cast<SizeoftypeMeExpr&>(meExpr).GetTyIdx());
      break;
    case kMeOpFieldsDist: {
      FieldsDistMeExpr &expr = static_cast<FieldsDistMeExpr&>(meExpr);
      resultExpr = New<FieldsDistMeExpr>(exprId, expr.GetTyIdx(), expr.GetFieldID1(), expr.GetFieldID2());
      break;
    }
    case kMeOpAddrof:
      resultExpr = New<AddrofMeExpr>(exprId, static_cast<AddrofMeExpr&>(meExpr).GetOstIdx());
      static_cast<AddrofMeExpr*>(resultExpr)->SetFieldID(static_cast<AddrofMeExpr&>(meExpr).GetFieldID());
      break;
    case kMeOpNary:
      resultExpr = NewInPool<NaryMeExpr>(exprId, static_cast<NaryMeExpr&>(meExpr));
      break;
    case kMeOpAddroffunc:
      resultExpr = New<AddroffuncMeExpr>(exprId, static_cast<AddroffuncMeExpr&>(meExpr).GetPuIdx());
      break;
    case kMeOpGcmalloc:
      resultExpr = New<GcmallocMeExpr>(exprId, static_cast<GcmallocMeExpr&>(meExpr).GetTyIdx());
      break;
    default:
      ASSERT(false, "not yet implement");
      return nullptr;
  }
  if (resultExpr != nullptr) {
    resultExpr->InitBase(meExpr.GetOp(), meExpr.GetPrimType(), meExpr.GetNumOpnds());
    if (meExpr.GetMeOp() == kMeOpOp || meExpr.GetMeOp() == kMeOpNary) {
      resultExpr->UpdateDepth();
    }
  }
  return resultExpr;
}

MeExpr *MeBuilder::BuildMeExpr(BaseNode &mirNode) const {
  auto func = CreateProductFunction<MeExprBuildFactory>(mirNode.GetOpCode());
  ASSERT(func != nullptr, "NIY BuildExpe");
  return func(this, mirNode);
}

MeExpr *MeBuilder::BuildAddrofMeExpr(BaseNode &mirNode) const {
  AddrofSSANode &addrofNode = static_cast<AddrofSSANode&>(mirNode);
  AddrofMeExpr &meExpr = *New<AddrofMeExpr>(kInvalidExprID, addrofNode.GetSSAVar()->GetOrigSt()->GetIndex());
  meExpr.InitBase(mirNode.GetOpCode(), mirNode.GetPrimType(), mirNode.GetNumOpnds());
  meExpr.SetFieldID(addrofNode.GetFieldID());
  return &meExpr;
}

MeExpr *MeBuilder::BuildAddroffuncMeExpr(BaseNode &mirNode) const {
  AddroffuncMeExpr &meExpr = *New<AddroffuncMeExpr>(kInvalidExprID, static_cast<AddroffuncNode&>(mirNode).GetPUIdx());
  meExpr.InitBase(mirNode.GetOpCode(), mirNode.GetPrimType(), mirNode.GetNumOpnds());
  return &meExpr;
}

MeExpr *MeBuilder::BuildGCMallocMeExpr(BaseNode &mirNode) const {
  GcmallocMeExpr &meExpr = *New<GcmallocMeExpr>(kInvalidExprID, static_cast<GCMallocNode&>(mirNode).GetTyIdx());
  meExpr.InitBase(mirNode.GetOpCode(), mirNode.GetPrimType(), mirNode.GetNumOpnds());
  return &meExpr;
}

MeExpr *MeBuilder::BuildSizeoftypeMeExpr(BaseNode &mirNode) const {
  SizeoftypeMeExpr &meExpr = *New<SizeoftypeMeExpr>(kInvalidExprID, static_cast<SizeoftypeNode&>(mirNode).GetTyIdx());
  meExpr.InitBase(mirNode.GetOpCode(), mirNode.GetPrimType(), mirNode.GetNumOpnds());
  return &meExpr;
}

MeExpr *MeBuilder::BuildFieldsDistMeExpr(BaseNode &mirNode) const {
  FieldsDistNode &fieldsDistNode = static_cast<FieldsDistNode&>(mirNode);
  FieldsDistMeExpr &meExpr = *New<FieldsDistMeExpr>(kInvalidExprID, fieldsDistNode.GetTyIdx(),
                                                    fieldsDistNode.GetFiledID1(), fieldsDistNode.GetFiledID2());
  meExpr.InitBase(mirNode.GetOpCode(), mirNode.GetPrimType(), mirNode.GetNumOpnds());
  return &meExpr;
}

MeExpr *MeBuilder::BuildIvarMeExpr(BaseNode &mirNode) const {
  IreadSSANode &ireadSSANode = static_cast<IreadSSANode&>(mirNode);
  IvarMeExpr &meExpr = *New<IvarMeExpr>(kInvalidExprID);
  meExpr.SetFieldID(ireadSSANode.GetFieldID());
  meExpr.SetTyIdx(ireadSSANode.GetTyIdx());
  meExpr.InitBase(mirNode.GetOpCode(), mirNode.GetPrimType(), mirNode.GetNumOpnds());
  return &meExpr;
}

MeExpr *MeBuilder::BuildConstMeExpr(BaseNode &mirNode) const {
  ConstvalNode &constvalNode = static_cast<ConstvalNode &>(mirNode);
  ConstMeExpr &meExpr = *New<ConstMeExpr>(kInvalidExprID, constvalNode.GetConstVal());
  meExpr.SetOp(OP_constval);
  meExpr.SetPtyp(constvalNode.GetPrimType());
  return &meExpr;
}

MeExpr *MeBuilder::BuildConststrMeExpr(BaseNode &mirNode) const {
  ConststrMeExpr &meExpr = *New<ConststrMeExpr>(kInvalidExprID, static_cast<ConststrNode&>(mirNode).GetStrIdx());
  meExpr.InitBase(mirNode.GetOpCode(), mirNode.GetPrimType(), mirNode.GetNumOpnds());
  return &meExpr;
}

MeExpr *MeBuilder::BuildConststr16MeExpr(BaseNode &mirNode) const {
  Conststr16MeExpr &meExpr = *New<Conststr16MeExpr>(kInvalidExprID, static_cast<Conststr16Node&>(mirNode).GetStrIdx());
  meExpr.InitBase(mirNode.GetOpCode(), mirNode.GetPrimType(), mirNode.GetNumOpnds());
  return &meExpr;
}

MeExpr *MeBuilder::BuildOpMeExprForCompare(BaseNode &mirNode) const {
  OpMeExpr *meExpr = BuildOpMeExpr(mirNode);
  meExpr->SetOpndType(static_cast<CompareNode&>(mirNode).GetOpndType());
  return meExpr;
}

MeExpr *MeBuilder::BuildOpMeExprForTypeCvt(BaseNode &mirNode) const {
  OpMeExpr *meExpr = BuildOpMeExpr(mirNode);
  meExpr->SetOpndType(static_cast<TypeCvtNode&>(mirNode).FromType());
  return meExpr;
}

MeExpr *MeBuilder::BuildOpMeExprForRetype(BaseNode &mirNode) const {
  RetypeNode &retypeNode = static_cast<RetypeNode&>(mirNode);
  OpMeExpr *meExpr = BuildOpMeExpr(mirNode);
  meExpr->SetOpndType(retypeNode.FromType());
  meExpr->SetTyIdx(retypeNode.GetTyIdx());
  return meExpr;
}

MeExpr *MeBuilder::BuildOpMeExprForIread(BaseNode &mirNode) const {
  IreadNode &ireadNode = static_cast<IreadNode&>(mirNode);
  OpMeExpr *meExpr = BuildOpMeExpr(mirNode);
  meExpr->SetTyIdx(ireadNode.GetTyIdx());
  meExpr->SetFieldID(ireadNode.GetFieldID());
  return meExpr;
}

MeExpr *MeBuilder::BuildOpMeExprForExtractbits(BaseNode &mirNode) const {
  ExtractbitsNode &extractbitsNode = static_cast<ExtractbitsNode&>(mirNode);
  OpMeExpr *meExpr = BuildOpMeExpr(mirNode);
  meExpr->SetBitsOffSet(extractbitsNode.GetBitsOffset());
  meExpr->SetBitsSize(extractbitsNode.GetBitsSize());
  return meExpr;
}

MeExpr *MeBuilder::BuildOpMeExprForJarrayMalloc(BaseNode &mirNode) const {
  OpMeExpr *meExpr = BuildOpMeExpr(mirNode);
  meExpr->SetTyIdx(static_cast<JarrayMallocNode&>(mirNode).GetTyIdx());
  return meExpr;
}

MeExpr *MeBuilder::BuildOpMeExprForResolveFunc(BaseNode &mirNode) const {
  OpMeExpr *meExpr = BuildOpMeExpr(mirNode);
  meExpr->SetFieldID(static_cast<ResolveFuncNode&>(mirNode).GetPuIdx());
  return meExpr;
}

MeExpr *MeBuilder::BuildNaryMeExprForArray(BaseNode &mirNode) const {
  ArrayNode &arrayNode = static_cast<ArrayNode&>(mirNode);
  NaryMeExpr &meExpr =
      *NewInPool<NaryMeExpr>(kInvalidExprID, arrayNode.GetTyIdx(), INTRN_UNDEFINED, arrayNode.GetBoundsCheck());
  meExpr.InitBase(mirNode.GetOpCode(), mirNode.GetPrimType(), mirNode.GetNumOpnds());
  return &meExpr;
}

MeExpr *MeBuilder::BuildNaryMeExprForIntrinsicop(BaseNode &mirNode) const {
  NaryMeExpr &meExpr =
      *NewInPool<NaryMeExpr>(kInvalidExprID, TyIdx(0), static_cast<IntrinsicopNode&>(mirNode).GetIntrinsic(), false);
  meExpr.InitBase(mirNode.GetOpCode(), mirNode.GetPrimType(), mirNode.GetNumOpnds());
  return &meExpr;
}

MeExpr *MeBuilder::BuildNaryMeExprForIntrinsicWithType(BaseNode &mirNode) const {
  IntrinsicopNode &intrinNode = static_cast<IntrinsicopNode&>(mirNode);
  NaryMeExpr &meExpr = *NewInPool<NaryMeExpr>(kInvalidExprID, intrinNode.GetTyIdx(), intrinNode.GetIntrinsic(), false);
  meExpr.InitBase(mirNode.GetOpCode(), mirNode.GetPrimType(), mirNode.GetNumOpnds());
  return &meExpr;
}

void MeBuilder::InitMeExprBuildFactory() const {
  RegisterFactoryFunction<MeExprBuildFactory>(OP_addrof, &MeBuilder::BuildAddrofMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_addroffunc, &MeBuilder::BuildAddroffuncMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_gcmalloc, &MeBuilder::BuildGCMallocMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_gcpermalloc, &MeBuilder::BuildGCMallocMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_sizeoftype, &MeBuilder::BuildSizeoftypeMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_fieldsdist, &MeBuilder::BuildFieldsDistMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_iread, &MeBuilder::BuildIvarMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_constval, &MeBuilder::BuildConstMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_conststr, &MeBuilder::BuildConststrMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_conststr16, &MeBuilder::BuildConststr16MeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_eq, &MeBuilder::BuildOpMeExprForCompare);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_ne, &MeBuilder::BuildOpMeExprForCompare);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_lt, &MeBuilder::BuildOpMeExprForCompare);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_gt, &MeBuilder::BuildOpMeExprForCompare);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_le, &MeBuilder::BuildOpMeExprForCompare);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_ge, &MeBuilder::BuildOpMeExprForCompare);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_cmpg, &MeBuilder::BuildOpMeExprForCompare);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_cmpl, &MeBuilder::BuildOpMeExprForCompare);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_cmp, &MeBuilder::BuildOpMeExprForCompare);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_ceil, &MeBuilder::BuildOpMeExprForTypeCvt);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_cvt, &MeBuilder::BuildOpMeExprForTypeCvt);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_floor, &MeBuilder::BuildOpMeExprForTypeCvt);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_trunc, &MeBuilder::BuildOpMeExprForTypeCvt);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_retype, &MeBuilder::BuildOpMeExprForRetype);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_abs, &MeBuilder::BuildOpMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_bnot, &MeBuilder::BuildOpMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_lnot, &MeBuilder::BuildOpMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_neg, &MeBuilder::BuildOpMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_recip, &MeBuilder::BuildOpMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_sqrt, &MeBuilder::BuildOpMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_alloca, &MeBuilder::BuildOpMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_malloc, &MeBuilder::BuildOpMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_iaddrof, &MeBuilder::BuildOpMeExprForIread);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_sext, &MeBuilder::BuildOpMeExprForExtractbits);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_zext, &MeBuilder::BuildOpMeExprForExtractbits);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_extractbits, &MeBuilder::BuildOpMeExprForExtractbits);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_gcmallocjarray, &MeBuilder::BuildOpMeExprForJarrayMalloc);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_gcpermallocjarray, &MeBuilder::BuildOpMeExprForJarrayMalloc);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_resolveinterfacefunc, &MeBuilder::BuildOpMeExprForResolveFunc);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_resolvevirtualfunc, &MeBuilder::BuildOpMeExprForResolveFunc);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_sub, &MeBuilder::BuildOpMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_mul, &MeBuilder::BuildOpMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_div, &MeBuilder::BuildOpMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_rem, &MeBuilder::BuildOpMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_ashr, &MeBuilder::BuildOpMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_lshr, &MeBuilder::BuildOpMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_shl, &MeBuilder::BuildOpMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_max, &MeBuilder::BuildOpMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_min, &MeBuilder::BuildOpMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_band, &MeBuilder::BuildOpMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_bior, &MeBuilder::BuildOpMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_bxor, &MeBuilder::BuildOpMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_land, &MeBuilder::BuildOpMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_lior, &MeBuilder::BuildOpMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_add, &MeBuilder::BuildOpMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_select, &MeBuilder::BuildOpMeExpr);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_array, &MeBuilder::BuildNaryMeExprForArray);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_intrinsicop, &MeBuilder::BuildNaryMeExprForIntrinsicop);
  RegisterFactoryFunction<MeExprBuildFactory>(OP_intrinsicopwithtype, &MeBuilder::BuildNaryMeExprForIntrinsicWithType);
}
}  // namespace maple
