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
#include "irmap.h"
#include <queue>
#include "ssa_mir_nodes.h"
#include "ssa.h"
#include "mir_builder.h"
#include "factory.h"

namespace maple {
using MeStmtFactory = FunctionFactory<Opcode, MeStmt*, IRMap*, StmtNode&, AccessSSANodes&>;

// recursively invoke itself in a pre-order traversal of the dominator tree of
// the CFG to build the HSSA representation for the code in each BB
void IRMap::BuildBB(BB &bb, std::vector<bool> &bbIRMapProcessed) {
  BBId bbID = bb.GetBBId();
  if (bbIRMapProcessed[bbID]) {
    return;
  }
  bbIRMapProcessed[bbID] = true;
  curBB = &bb;
  SetCurFunction(bb);
  // iterate phi list to update the definition by phi
  BuildPhiMeNode(bb);
  if (!bb.IsEmpty()) {
    for (auto &stmt : bb.GetStmtNodes()) {
      MeStmt *meStmt = BuildMeStmt(stmt);
      bb.AddMeStmtLast(meStmt);
    }
  }
  // travesal bb's dominated tree
  ASSERT(bbID < dom.GetDomChildrenSize(), " index out of range in IRMap::BuildBB");
  const MapleSet<BBId> &domChildren = dom.GetDomChildren(bbID);
  for (auto bbIt = domChildren.begin(); bbIt != domChildren.end(); ++bbIt) {
    BBId childBBId = *bbIt;
    BuildBB(*GetBB(childBBId), bbIRMapProcessed);
  }
}

void IRMap::BuildPhiMeNode(BB &bb) {
  for (auto &phi : bb.GetPhiList()) {
    const OriginalSt *oSt = ssaTab.GetOriginalStFromID(phi.first);
    VersionSt *vSt = phi.second.GetResult();

    auto *phiMeNode = NewInPool<MePhiNode>();
    phiMeNode->SetDefBB(&bb);
    (void)bb.GetMePhiList().insert(std::make_pair(oSt->GetIndex(), phiMeNode));
    if (oSt->IsPregOst()) {
      RegMeExpr *meDef = GetOrCreateRegFromVerSt(*vSt);
      phiMeNode->UpdateLHS(*meDef);
      // build phi operands
      for (VersionSt *opnd : phi.second.GetPhiOpnds()) {
        phiMeNode->GetOpnds().push_back(GetOrCreateRegFromVerSt(*opnd));
      }
    } else {
      VarMeExpr *meDef = GetOrCreateVarFromVerSt(*vSt);
      phiMeNode->UpdateLHS(*meDef);
      // build phi operands
      for (VersionSt *opnd : phi.second.GetPhiOpnds()) {
        phiMeNode->GetOpnds().push_back(GetOrCreateVarFromVerSt(*opnd));
      }
    }
  }
}

VarMeExpr *IRMap::CreateVarMeExprVersion(const VarMeExpr &origExpr) {
  auto *varMeExpr = New<VarMeExpr>(&irMapAlloc, exprID++, origExpr.GetOStIdx(), vst2MeExprTable.size());
  vst2MeExprTable.push_back(varMeExpr);
  varMeExpr->InitBase(origExpr.GetOp(), origExpr.GetPrimType(), origExpr.GetNumOpnds());
  varMeExpr->SetFieldID(origExpr.GetFieldID());
  return varMeExpr;
}

MeExpr *IRMap::CreateAddrofMeExpr(OStIdx ostIdx) {
  AddrofMeExpr addrofMeExpr(-1, ostIdx);
  addrofMeExpr.SetOp(OP_addrof);
  addrofMeExpr.SetPtyp(PTY_ptr);
  addrofMeExpr.SetNumOpnds(0);
  return HashMeExpr(addrofMeExpr);
}

MeExpr *IRMap::CreateAddrofMeExpr(MeExpr &expr) {
  if (expr.GetMeOp() == kMeOpVar) {
    auto &varMeExpr = static_cast<VarMeExpr&>(expr);
    return CreateAddrofMeExpr(varMeExpr.GetOStIdx());
  } else {
    ASSERT(expr.GetMeOp() == kMeOpIvar, "expecting IVarMeExpr");
    auto &ivarExpr = static_cast<IvarMeExpr&>(expr);
    OpMeExpr opMeExpr(kInvalidExprID);
    opMeExpr.SetFieldID(ivarExpr.GetFieldID());
    opMeExpr.SetTyIdx(ivarExpr.GetTyIdx());
    opMeExpr.SetOpnd(0, ivarExpr.GetBase());
    opMeExpr.SetOp(OP_iaddrof);
    opMeExpr.SetPtyp(PTY_ptr);
    opMeExpr.SetNumOpnds(1);
    return HashMeExpr(opMeExpr);
  }
}

MeExpr *IRMap::CreateAddroffuncMeExpr(PUIdx puIdx) {
  AddroffuncMeExpr addroffuncMeExpr(-1, puIdx);
  addroffuncMeExpr.SetOp(OP_addroffunc);
  addroffuncMeExpr.SetPtyp(PTY_ptr);
  addroffuncMeExpr.SetNumOpnds(0);
  return HashMeExpr(addroffuncMeExpr);
}

MeExpr *IRMap::CreateIaddrofMeExpr(MeExpr &expr, TyIdx tyIdx, MeExpr &base) {
  ASSERT(expr.GetMeOp() == kMeOpIvar, "expecting IVarMeExpr");
  auto &ivarExpr = static_cast<IvarMeExpr&>(expr);
  OpMeExpr opMeExpr(kInvalidExprID);
  opMeExpr.SetFieldID(ivarExpr.GetFieldID());
  opMeExpr.SetTyIdx(tyIdx);
  opMeExpr.SetOpnd(0, &base);
  opMeExpr.SetOp(OP_iaddrof);
  opMeExpr.SetPtyp(PTY_ptr);
  opMeExpr.SetNumOpnds(1);
  return HashMeExpr(opMeExpr);
}

MeExpr *IRMap::CreateIvarMeExpr(MeExpr &expr, TyIdx tyIdx, MeExpr &base) {
  ASSERT(expr.GetMeOp() == kMeOpVar, "expecting IVarMeExpr");
  auto &varMeExpr = static_cast<VarMeExpr&>(expr);
  IvarMeExpr ivarMeExpr(-1);
  ivarMeExpr.SetFieldID(varMeExpr.GetFieldID());
  ivarMeExpr.SetTyIdx(tyIdx);
  ivarMeExpr.SetBase(&base);
  ivarMeExpr.InitBase(varMeExpr.GetOp(), varMeExpr.GetPrimType(), 1);
  ivarMeExpr.SetOp(OP_iread);
  ivarMeExpr.SetMuVal(&varMeExpr);
  return HashMeExpr(ivarMeExpr);
}

VarMeExpr *IRMap::CreateNewVarMeExpr(OStIdx ostIdx, PrimType pType, FieldID fieldID) {
  VarMeExpr *varMeExpr = meBuilder.BuildVarMeExpr(exprID++, ostIdx, vst2MeExprTable.size(), pType, fieldID);
  PushBackVerst2MeExprTable(varMeExpr);
  return varMeExpr;
}

VarMeExpr *IRMap::CreateNewVarMeExpr(OriginalSt &oSt, PrimType pType, FieldID fieldID) {
  VarMeExpr *varMeExpr = CreateNewVarMeExpr(oSt.GetIndex(), pType, fieldID);
  oSt.PushbackVersionIndex(varMeExpr->GetVstIdx());
  return varMeExpr;
}

VarMeExpr *IRMap::CreateNewGlobalTmp(GStrIdx strIdx, PrimType pType) {
  MIRSymbol *st =
      mirModule.GetMIRBuilder()->CreateSymbol((TyIdx)pType, strIdx, kStVar, kScGlobal, nullptr, kScopeGlobal);
  st->SetIsTmp(true);
  OriginalSt *oSt = ssaTab.CreateSymbolOriginalSt(*st, 0, 0);
  auto *varx = New<VarMeExpr>(&irMapAlloc, exprID++, oSt->GetIndex(), oSt->GetZeroVersionIndex());
  varx->InitBase(OP_dread, pType, 0);
  return varx;
}

VarMeExpr *IRMap::CreateNewLocalRefVarTmp(GStrIdx strIdx, TyIdx tIdx) {
  MIRSymbol *st =
      mirModule.GetMIRBuilder()->CreateSymbol(tIdx, strIdx, kStVar, kScAuto, mirModule.CurFunction(), kScopeLocal);
  st->SetInstrumented();
  OriginalSt *oSt = ssaTab.CreateSymbolOriginalSt(*st, mirModule.CurFunction()->GetPuidx(), 0);
  oSt->SetZeroVersionIndex(vst2MeExprTable.size());
  vst2MeExprTable.push_back(nullptr);
  oSt->PushbackVersionIndex(oSt->GetZeroVersionIndex());
  auto *newLocalRefVar = New<VarMeExpr>(&irMapAlloc, exprID++, oSt->GetIndex(), vst2MeExprTable.size());
  vst2MeExprTable.push_back(newLocalRefVar);
  newLocalRefVar->InitBase(OP_dread, PTY_ref, 0);
  return newLocalRefVar;
}

RegMeExpr *IRMap::CreateRegMeExprVersion(const OriginalSt &pregOSt) {
  auto *regReadExpr =
      NewInPool<RegMeExpr>(exprID++, pregOSt.GetPregIdx(), pregOSt.GetPuIdx(), pregOSt.GetIndex(), 0);
  regReadExpr->InitBase(OP_regread, pregOSt.GetMIRPreg()->GetPrimType(), 0);
  regMeExprTable.push_back(regReadExpr);
  return regReadExpr;
}

RegMeExpr *IRMap::CreateRegMeExprVersion(const RegMeExpr &origExpr) {
  auto *regReadExpr =
      NewInPool<RegMeExpr>(exprID++, origExpr.GetRegIdx(), origExpr.GetPuIdx(), origExpr.GetOstIdx(), 0);
  regReadExpr->InitBase(origExpr.GetOp(), origExpr.GetPrimType(), origExpr.GetNumOpnds());
  regMeExprTable.push_back(regReadExpr);
  return regReadExpr;
}

RegMeExpr *IRMap::CreateRefRegMeExpr(const MIRSymbol &mirSt) {
  MIRFunction *mirFunc = mirModule.CurFunction();
  MIRType *stType = mirSt.GetType();
  PrimType pType = stType->GetPrimType();
  ASSERT(pType == PTY_ref, "only PTY_ref needed");
  PregIdx regIdx = mirFunc->GetPregTab()->CreateRefPreg(*stType);
  ASSERT(regIdx <= 0xffff, "register oversized");
  MIRPreg *preg = mirFunc->GetPregTab()->PregFromPregIdx(regIdx);
  if (!mirSt.IgnoreRC()) {
    preg->SetNeedRC();
  }
  OriginalSt *oSt = ssaTab.GetOriginalStTable().CreatePregOriginalSt(regIdx, mirFunc->GetPuidx());
  auto *regreadexpr = NewInPool<RegMeExpr>(exprID++, regIdx, mirFunc->GetPuidx(), oSt->GetIndex(), 0);
  regreadexpr->InitBase(OP_regread, pType, 0);
  regMeExprTable.push_back(regreadexpr);
  return regreadexpr;
}

RegMeExpr *IRMap::CreateRegMeExpr(PrimType pType) {
  MIRFunction *mirFunc = mirModule.CurFunction();
  PregIdx regIdx = mirFunc->GetPregTab()->CreatePreg(pType);
  ASSERT(regIdx <= 0xffff, "register oversized");
  OriginalSt *ost = ssaTab.GetOriginalStTable().CreatePregOriginalSt(regIdx, mirFunc->GetPuidx());
  auto *regReadExpr = NewInPool<RegMeExpr>(exprID++, regIdx, mirFunc->GetPuidx(), ost->GetIndex(), 0);
  regReadExpr->InitBase(OP_regread, pType, 0);
  regMeExprTable.push_back(regReadExpr);
  return regReadExpr;
}

RegMeExpr *IRMap::CreateRegRefMeExpr(MIRType &mirType) {
  MIRFunction *mirFunc = mirModule.CurFunction();
  PregIdx regIdx = mirFunc->GetPregTab()->CreateRefPreg(mirType);
  ASSERT(regIdx <= 0xffff, "register oversized");
  OriginalSt *ost = ssaTab.GetOriginalStTable().CreatePregOriginalSt(regIdx, mirFunc->GetPuidx());
  auto *regReadExpr = NewInPool<RegMeExpr>(exprID++, regIdx, mirFunc->GetPuidx(), ost->GetIndex(), 0);
  regReadExpr->InitBase(OP_regread, mirType.GetPrimType(), 0);
  regMeExprTable.push_back(regReadExpr);
  return regReadExpr;
}

RegMeExpr *IRMap::CreateRegRefMeExpr(const MeExpr &meExpr) {
  MIRType *mirType = nullptr;
  switch (meExpr.GetMeOp()) {
    case kMeOpVar: {
      auto &varMeExpr = static_cast<const VarMeExpr&>(meExpr);
      const OriginalSt *ost = ssaTab.GetOriginalStFromID(varMeExpr.GetOStIdx());
      ASSERT(ost->GetTyIdx() != 0u, "expect ost->tyIdx to be initialized");
      mirType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(ost->GetTyIdx());
      break;
    }
    case kMeOpIvar: {
      auto &ivarMeExpr = static_cast<const IvarMeExpr&>(meExpr);
      MIRType *ptrMirType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(ivarMeExpr.GetTyIdx());
      ASSERT(ptrMirType->GetKind() == kTypePointer, "must be point type for ivar");
      auto *realMirType = static_cast<MIRPtrType*>(ptrMirType);
      FieldID fieldID = ivarMeExpr.GetFieldID();
      if (fieldID > 0) {
        mirType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(realMirType->GetPointedTyIdxWithFieldID(fieldID));
      } else {
        mirType = realMirType->GetPointedType();
      }
      ASSERT(mirType->GetPrimType() == meExpr.GetPrimType() ||
             !(IsAddress(mirType->GetPrimType()) && IsAddress(meExpr.GetPrimType())),
             "inconsistent type");
      ASSERT(mirType->GetPrimType() == PTY_ref, "CreateRegRefMeExpr: only ref type expected");
      break;
    }
    case kMeOpOp:
      if (meExpr.GetOp() == OP_retype) {
        auto &opMeExpr = static_cast<const OpMeExpr&>(meExpr);
        ASSERT(opMeExpr.GetTyIdx() != 0u, "expect opMeExpr.tyIdx to be initialized");
        mirType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(opMeExpr.GetTyIdx());
        break;
      }
    // fall thru
    [[clang::fallthrough]];
    default:
      return CreateRegMeExpr(PTY_ptr);
  }
  return CreateRegRefMeExpr(*mirType);
}

VarMeExpr *IRMap::GetOrCreateVarFromVerSt(const VersionSt &vst) {
  size_t vindex = vst.GetIndex();
  ASSERT(vindex < vst2MeExprTable.size(), "GetOrCreateVarFromVerSt: index %d is out of range", vindex);
  MeExpr *meExpr = vst2MeExprTable.at(vindex);
  if (meExpr != nullptr) {
    return static_cast<VarMeExpr*>(meExpr);
  }
  const OriginalSt *ost = vst.GetOrigSt();
  ASSERT(ost->IsSymbolOst(), "GetOrCreateVarFromVerSt: wrong ost_type");
  auto *varx = New<VarMeExpr>(&irMapAlloc, exprID++, ost->GetIndex(), vindex);
  ASSERT(!GlobalTables::GetTypeTable().GetTypeTable().empty(), "container check");
  varx->InitBase(OP_dread, GlobalTables::GetTypeTable().GetTypeFromTyIdx(ost->GetTyIdx())->GetPrimType(), 0);
  varx->SetFieldID(ost->GetFieldID());
  vst2MeExprTable[vindex] = varx;
  return varx;
}

VarMeExpr *IRMap::GetOrCreateZeroVersionVarMeExpr(const OriginalSt &ost) {
  ASSERT(ost.GetZeroVersionIndex() < vst2MeExprTable.size(),
         "GetOrCreateZeroVersionVarMeExpr: version index of osym's kInitVersion out of range");
  if (ost.GetZeroVersionIndex() == 0) {
    ssaTab.SetZeroVersionIndex(ost.GetIndex(), vst2MeExprTable.size());
    vst2MeExprTable.push_back(nullptr);
  }
  if (vst2MeExprTable[ost.GetZeroVersionIndex()] == nullptr) {
    auto *varMeExpr = New<VarMeExpr>(&irMapAlloc, exprID++, ost.GetIndex(), ost.GetZeroVersionIndex());
    varMeExpr->SetFieldID(ost.GetFieldID());
    varMeExpr->SetOp(OP_dread);
    ASSERT(!GlobalTables::GetTypeTable().GetTypeTable().empty(), "container check");
    varMeExpr->SetPtyp(GlobalTables::GetTypeTable().GetTypeFromTyIdx(ost.GetTyIdx())->GetPrimType());
    varMeExpr->SetNumOpnds(0);
    vst2MeExprTable[ost.GetZeroVersionIndex()] = varMeExpr;
    return varMeExpr;
  }
  return static_cast<VarMeExpr*>(vst2MeExprTable[ost.GetZeroVersionIndex()]);
}

RegMeExpr *IRMap::GetOrCreateRegFromVerSt(const VersionSt &vst) {
  size_t vindex = vst.GetIndex();
  ASSERT(vindex < vst2MeExprTable.size(), " GetOrCreateRegFromVerSt: index %d is out of range", vindex);
  MeExpr *meExpr = vst2MeExprTable[vindex];
  if (meExpr != nullptr) {
    return static_cast<RegMeExpr*>(meExpr);
  }
  const OriginalSt *ost = vst.GetOrigSt();
  ASSERT(ost->IsPregOst(), "GetOrCreateRegFromVerSt: PregOST expected");
  auto *regx =
      NewInPool<RegMeExpr>(exprID++, ost->GetPregIdx(), mirModule.CurFunction()->GetPuidx(), ost->GetIndex(), vindex);
  regx->InitBase(OP_regread, ost->GetMIRPreg()->GetPrimType(), 0);
  regMeExprTable.push_back(regx);
  vst2MeExprTable[vindex] = regx;
  return regx;
}

MeExpr *IRMap::BuildLHSVar(const VersionSt &vst, DassignMeStmt &defMeStmt) {
  VarMeExpr *meDef = GetOrCreateVarFromVerSt(vst);
  meDef->SetDefStmt(&defMeStmt);
  meDef->SetDefBy(kDefByStmt);
  vst2MeExprTable.at(vst.GetIndex()) = meDef;
  return meDef;
}

MeExpr *IRMap::BuildLHSReg(const VersionSt &vst, RegassignMeStmt &defMeStmt, const RegassignNode &regassign) {
  RegMeExpr *meDef = GetOrCreateRegFromVerSt(vst);
  meDef->SetPtyp(regassign.GetPrimType());
  meDef->SetDefStmt(&defMeStmt);
  meDef->SetDefBy(kDefByStmt);
  vst2MeExprTable.at(vst.GetIndex()) = meDef;
  return meDef;
}

IvarMeExpr *IRMap::BuildLHSIvar(MeExpr &baseAddr, IassignMeStmt &iassignMeStmt, FieldID fieldID) {
  auto *meDef = New<IvarMeExpr>(exprID++);
  meDef->SetFieldID(fieldID);
  meDef->SetTyIdx(iassignMeStmt.GetTyIdx());
  meDef->SetBase(&baseAddr);
  meDef->SetDefStmt(&iassignMeStmt);
  meDef->SetOp(OP_iread);
  meDef->SetPtyp(iassignMeStmt.GetRHS()->GetPrimType());
  PutToBucket(meDef->GetHashIndex() % mapHashLength, *meDef);
  return meDef;
}

IvarMeExpr *IRMap::BuildIvarFromOpMeExpr(OpMeExpr &opMeExpr) {
  IvarMeExpr *ivar = New<IvarMeExpr>(exprID++);
  ivar->SetFieldID(opMeExpr.GetFieldID());
  ivar->SetTyIdx(opMeExpr.GetTyIdx());
  ivar->SetBase(opMeExpr.GetOpnd(0));
  return ivar;
}

IvarMeExpr *IRMap::BuildLHSIvarFromIassMeStmt(IassignMeStmt &iassignMeStmt) {
  return BuildLHSIvar(*iassignMeStmt.GetLHSVal()->GetBase(), iassignMeStmt, iassignMeStmt.GetLHSVal()->GetFieldID());
}

// build Me chilist from MayDefNode list
void IRMap::BuildChiList(MeStmt &meStmt, MapleMap<OStIdx, MayDefNode> &mayDefNodes,
                         MapleMap<OStIdx, ChiMeNode*> &outList) {
  for (auto it = mayDefNodes.begin(); it != mayDefNodes.end(); ++it) {
    MayDefNode &mayDefNode = it->second;
    VersionSt *opndSt = mayDefNode.GetOpnd();
    VersionSt *resSt = mayDefNode.GetResult();
    auto *chiMeStmt = New<ChiMeNode>(&meStmt);
    chiMeStmt->SetRHS(GetOrCreateVarFromVerSt(*opndSt));
    VarMeExpr *lhs = GetOrCreateVarFromVerSt(*resSt);
    lhs->SetDefBy(kDefByChi);
    lhs->SetDefChi(*chiMeStmt);
    chiMeStmt->SetLHS(lhs);
    outList.insert(std::make_pair(lhs->GetOStIdx(), chiMeStmt));
  }
}

void IRMap::BuildMustDefList(MeStmt &meStmt, MapleVector<MustDefNode> &mustDefList,
                             MapleVector<MustDefMeNode> &mustDefMeList) {
  for (auto it = mustDefList.begin(); it != mustDefList.end(); ++it) {
    MustDefNode &mustDefNode = *it;
    VersionSt *vst = mustDefNode.GetResult();
    VarMeExpr *lhs = GetOrCreateVarFromVerSt(*vst);
    ASSERT(lhs->GetMeOp() == kMeOpReg || lhs->GetMeOp() == kMeOpVar, "unexpected opcode");
    mustDefMeList.push_back(MustDefMeNode(lhs, &meStmt));
  }
}

MeStmt *IRMap::BuildMeStmtWithNoSSAPart(StmtNode &stmt) {
  Opcode op = stmt.GetOpCode();
  switch (op) {
    case OP_jscatch:
    case OP_finally:
    case OP_endtry:
    case OP_cleanuptry:
    case OP_membaracquire:
    case OP_membarrelease:
    case OP_membarstorestore:
    case OP_membarstoreload:
      return New<MeStmt>(&stmt);
    case OP_goto:
      return New<GotoMeStmt>(&stmt);
    case OP_comment:
      return NewInPool<CommentMeStmt>(&stmt);
    case OP_jstry:
      return New<JsTryMeStmt>(&stmt);
    case OP_catch:
      return NewInPool<CatchMeStmt>(&stmt);
    case OP_brfalse:
    case OP_brtrue: {
      auto &condGotoNode = static_cast<CondGotoNode&>(stmt);
      auto *condGotoMeStmt = New<CondGotoMeStmt>(&stmt);
      condGotoMeStmt->SetOpnd(0, BuildExpr(*condGotoNode.Opnd(0)));
      return condGotoMeStmt;
    }
    case OP_try: {
      auto &tryNode = static_cast<TryNode&>(stmt);
      auto *tryMeStmt = NewInPool<TryMeStmt>(&stmt);
      for (size_t i = 0; i < tryNode.GetOffsetsCount(); ++i) {
        tryMeStmt->OffsetsPushBack(tryNode.GetOffset(i));
      }
      return tryMeStmt;
    }
    case OP_assertnonnull:
    case OP_eval:
    case OP_free:
    case OP_switch: {
      auto &unaryStmt = static_cast<UnaryStmtNode&>(stmt);
      auto *unMeStmt =
          static_cast<UnaryMeStmt*>((op == OP_switch) ? NewInPool<SwitchMeStmt>(&stmt) : New<UnaryMeStmt>(&stmt));
      unMeStmt->SetOpnd(0, BuildExpr(*unaryStmt.Opnd(0)));
      return unMeStmt;
    }
    default:
      CHECK_FATAL(false, "NYI");
  }
}

MeStmt *IRMap::BuildDassignMeStmt(StmtNode &stmt, AccessSSANodes &ssaPart) {
  auto *meStmt = NewInPool<DassignMeStmt>(&stmt);
  auto &dassiNode = static_cast<DassignNode&>(stmt);
  meStmt->SetRHS(BuildExpr(*dassiNode.GetRHS()));
  auto *varLHS = static_cast<VarMeExpr*>(BuildLHSVar(*ssaPart.GetSSAVar(), *meStmt));
  meStmt->SetLHS(varLHS);
  BuildChiList(*meStmt, ssaPart.GetMayDefNodes(), *meStmt->GetChiList());
  return meStmt;
}

MeStmt *IRMap::BuildRegassignMeStmt(StmtNode &stmt, AccessSSANodes &ssaPart) {
  auto *meStmt = New<RegassignMeStmt>(&stmt);
  auto &regNode = static_cast<RegassignNode&>(stmt);
  meStmt->SetRHS(BuildExpr(*regNode.Opnd(0)));
  auto *regLHS = static_cast<RegMeExpr*>(BuildLHSReg(*ssaPart.GetSSAVar(), *meStmt, regNode));
  meStmt->SetLHS(regLHS);
  return meStmt;
}

MeStmt *IRMap::BuildIassignMeStmt(StmtNode &stmt, AccessSSANodes &ssaPart) {
  auto &iasNode = static_cast<IassignNode&>(stmt);
  auto *meStmt = NewInPool<IassignMeStmt>(&stmt);
  meStmt->SetTyIdx(iasNode.GetTyIdx());
  meStmt->SetRHS(BuildExpr(*iasNode.GetRHS()));
  meStmt->SetLHSVal(BuildLHSIvar(*BuildExpr(*iasNode.Opnd(0)), *meStmt, iasNode.GetFieldID()));
  BuildChiList(*meStmt, ssaPart.GetMayDefNodes(), *(meStmt->GetChiList()));
  return meStmt;
}

MeStmt *IRMap::BuildMaydassignMeStmt(StmtNode &stmt, AccessSSANodes &ssaPart) {
  auto *meStmt = NewInPool<MaydassignMeStmt>(&stmt);
  auto &dassiNode = static_cast<DassignNode&>(stmt);
  meStmt->SetRHS(BuildExpr(*dassiNode.GetRHS()));
  meStmt->SetMayDassignSym(ssaPart.GetSSAVar()->GetOrigSt());
  meStmt->SetFieldID(dassiNode.GetFieldID());
  BuildChiList(*meStmt, ssaPart.GetMayDefNodes(), *(meStmt->GetChiList()));
  return meStmt;
}

MeStmt *IRMap::BuildCallMeStmt(StmtNode &stmt, AccessSSANodes &ssaPart) {
  auto *callMeStmt = NewInPool<CallMeStmt>(&stmt);
  auto &intrinNode = static_cast<CallNode&>(stmt);
  callMeStmt->SetPUIdx(intrinNode.GetPUIdx());
  for (size_t i = 0; i < intrinNode.NumOpnds(); ++i) {
    callMeStmt->PushBackOpnd(BuildExpr(*intrinNode.Opnd(i)));
  }
  BuildMuList(ssaPart.GetMayUseNodes(), *(callMeStmt->GetMuList()));
  if (kOpcodeInfo.IsCallAssigned(stmt.GetOpCode())) {
    BuildMustDefList(*callMeStmt, ssaPart.GetMustDefNodes(), *(callMeStmt->GetMustDefList()));
  }
  BuildChiList(*callMeStmt, ssaPart.GetMayDefNodes(), *(callMeStmt->GetChiList()));
  return callMeStmt;
}


MeStmt *IRMap::BuildNaryMeStmt(StmtNode &stmt, AccessSSANodes &ssaPart) {
  Opcode op = stmt.GetOpCode();
  NaryMeStmt *naryMeStmt = (op == OP_icall || op == OP_icallassigned)
                           ? static_cast<NaryMeStmt*>(NewInPool<IcallMeStmt>(&stmt))
                           : static_cast<NaryMeStmt*>(NewInPool<IntrinsiccallMeStmt>(&stmt));
  auto &naryStmtNode = static_cast<NaryStmtNode&>(stmt);
  for (size_t i = 0; i < naryStmtNode.NumOpnds(); ++i) {
    naryMeStmt->PushBackOpnd(BuildExpr(*naryStmtNode.Opnd(i)));
  }
  BuildMuList(ssaPart.GetMayUseNodes(), *(naryMeStmt->GetMuList()));
  if (kOpcodeInfo.IsCallAssigned(op)) {
    BuildMustDefList(*naryMeStmt, ssaPart.GetMustDefNodes(), *(naryMeStmt->GetMustDefList()));
  }
  BuildChiList(*naryMeStmt, ssaPart.GetMayDefNodes(), *(naryMeStmt->GetChiList()));
  return naryMeStmt;
}

MeStmt *IRMap::BuildRetMeStmt(StmtNode &stmt, AccessSSANodes &ssaPart) {
  auto &retStmt = static_cast<NaryStmtNode&>(stmt);
  auto *meStmt = NewInPool<RetMeStmt>(&stmt);
  for (size_t i = 0; i < retStmt.NumOpnds(); ++i) {
    meStmt->PushBackOpnd(BuildExpr(*retStmt.Opnd(i)));
  }
  BuildMuList(ssaPart.GetMayUseNodes(), *(meStmt->GetMuList()));
  return meStmt;
}

MeStmt *IRMap::BuildWithMuMeStmt(StmtNode &stmt, AccessSSANodes &ssaPart) {
  auto *retSub = NewInPool<WithMuMeStmt>(&stmt);
  BuildMuList(ssaPart.GetMayUseNodes(), *(retSub->GetMuList()));
  return retSub;
}

MeStmt *IRMap::BuildGosubMeStmt(StmtNode &stmt, AccessSSANodes &ssaPart) {
  auto *goSub = NewInPool<GosubMeStmt>(&stmt);
  BuildMuList(ssaPart.GetMayUseNodes(), *(goSub->GetMuList()));
  return goSub;
}

MeStmt *IRMap::BuildThrowMeStmt(StmtNode &stmt, AccessSSANodes &ssaPart) {
  auto &unaryNode = static_cast<UnaryStmtNode&>(stmt);
  auto *tmeStmt = NewInPool<ThrowMeStmt>(&stmt);
  tmeStmt->SetMeStmtOpndValue(BuildExpr(*unaryNode.Opnd(0)));
  BuildMuList(ssaPart.GetMayUseNodes(), *(tmeStmt->GetMuList()));
  return tmeStmt;
}

MeStmt *IRMap::BuildSyncMeStmt(StmtNode &stmt, AccessSSANodes &ssaPart) {
  auto &naryNode = static_cast<NaryStmtNode&>(stmt);
  auto *naryStmt = NewInPool<SyncMeStmt>(&stmt);
  for (size_t i = 0; i < naryNode.NumOpnds(); ++i) {
    naryStmt->PushBackOpnd(BuildExpr(*naryNode.Opnd(i)));
  }
  BuildMuList(ssaPart.GetMayUseNodes(), *(naryStmt->GetMuList()));
  BuildChiList(*naryStmt, ssaPart.GetMayDefNodes(), *(naryStmt->GetChiList()));
  return naryStmt;
}

MeStmt *IRMap::BuildMeStmt(StmtNode &stmt) {
  AccessSSANodes *ssaPart = ssaTab.GetStmtsSSAPart().SSAPartOf(stmt);
  if (ssaPart == nullptr) {
    return BuildMeStmtWithNoSSAPart(stmt);
  }

  auto func = CreateProductFunction<MeStmtFactory>(stmt.GetOpCode());
  CHECK_FATAL(func != nullptr, "func nullptr check");
  return func(this, stmt, *ssaPart);
}

void IRMap::BuildMuList(MapleMap<OStIdx, MayUseNode> &mayUseList, MapleMap<OStIdx, VarMeExpr*> &muList) {
  for (std::pair<OStIdx, MayUseNode> mapItem : mayUseList) {
    MayUseNode &mayUseNode = mapItem.second;
    VersionSt *vst = mayUseNode.GetOpnd();
    VarMeExpr *varMeExpr = GetOrCreateVarFromVerSt(*vst);
    muList.insert(std::make_pair(varMeExpr->GetOStIdx(), varMeExpr));
  }
}

MeExpr *IRMap::BuildExpr(BaseNode &mirNode) {
  Opcode op = mirNode.GetOpCode();
  if (op == OP_dread) {
    auto &addrOfNode = static_cast<AddrofSSANode &>(mirNode);
    VersionSt *vst = addrOfNode.GetSSAVar();
    VarMeExpr *varMeExpr = GetOrCreateVarFromVerSt(*vst);
    varMeExpr->InitBase(mirNode.GetOpCode(), mirNode.GetPrimType(), mirNode.GetNumOpnds());
    if (vst->GetOrigSt()->IsRealSymbol()) {
      ASSERT(!vst->GetOrigSt()->IsPregOst(), "not expect preg symbol here");
      varMeExpr->SetPtyp(GlobalTables::GetTypeTable().GetTypeFromTyIdx(vst->GetOrigSt()->GetTyIdx())->GetPrimType());
      varMeExpr->SetFieldID(addrOfNode.GetFieldID());
    }
    return varMeExpr;
  }

  if (op == OP_regread) {
    auto &regNode = static_cast<RegreadSSANode &>(mirNode);
    VersionSt *vst = regNode.GetSSAVar();
    RegMeExpr *regMeExpr = GetOrCreateRegFromVerSt(*vst);
    regMeExpr->InitBase(mirNode.GetOpCode(), mirNode.GetPrimType(), mirNode.GetNumOpnds());
    return regMeExpr;
  }

  MeExpr *meExpr = meBuilder.BuildMeExpr(mirNode);
  SetMeExprOpnds(*meExpr, mirNode);

  if (op == OP_iread) {
    auto *ivarMeExpr = static_cast<IvarMeExpr*>(meExpr);
    auto &iReadSSANode = static_cast<IreadSSANode&>(mirNode);
    ivarMeExpr->SetBase(BuildExpr(*iReadSSANode.Opnd(0)));
    VersionSt *verSt = iReadSSANode.GetSSAVar();
    if (verSt != nullptr) {
      VarMeExpr *varMeExpr = GetOrCreateVarFromVerSt(*verSt);
      ivarMeExpr->SetMuVal(varMeExpr);
    }
  }

  MeExpr *retMeExpr = HashMeExpr(*meExpr);

  if (op == OP_iread) {
    ASSERT(static_cast<IvarMeExpr*>(retMeExpr)->GetMu() != nullptr, "BuildExpr: ivar node cannot have mu == nullptr");
  }

  return retMeExpr;
}

void IRMap::SetMeExprOpnds(MeExpr &meExpr, BaseNode &mirNode) {
  auto &opMeExpr = static_cast<OpMeExpr&>(meExpr);
  if (mirNode.IsUnaryNode()) {
    if (mirNode.GetOpCode() != OP_iread) {
      opMeExpr.SetOpnd(0, BuildExpr(*static_cast<UnaryNode&>(mirNode).Opnd(0)));
    }
  } else if (mirNode.IsBinaryNode()) {
    auto &binaryNode = static_cast<BinaryNode&>(mirNode);
    opMeExpr.SetOpnd(0, BuildExpr(*binaryNode.Opnd(0)));
    opMeExpr.SetOpnd(1, BuildExpr(*binaryNode.Opnd(1)));
  } else if (mirNode.IsTernaryNode()) {
    auto &ternaryNode = static_cast<TernaryNode&>(mirNode);
    opMeExpr.SetOpnd(0, BuildExpr(*ternaryNode.Opnd(0)));
    opMeExpr.SetOpnd(1, BuildExpr(*ternaryNode.Opnd(1)));
    opMeExpr.SetOpnd(2, BuildExpr(*ternaryNode.Opnd(2)));
  } else if (mirNode.IsNaryNode()) {
    auto &naryMeExpr = static_cast<NaryMeExpr&>(meExpr);
    auto &naryNode = static_cast<NaryNode&>(mirNode);
    for (size_t i = 0; i < naryNode.NumOpnds(); ++i) {
      naryMeExpr.GetOpnds().push_back(BuildExpr(*naryNode.Opnd(i)));
    }
  } else {
    // No need to do anything
  }
}

void IRMap::PutToBucket(uint32 hashIdx, MeExpr &meExpr) {
  MeExpr *headExpr = hashTable[hashIdx];
  if (headExpr != nullptr) {
    meExpr.SetNext(headExpr);
  }
  hashTable[hashIdx] = &meExpr;
}

MeExpr *IRMap::HashMeExpr(MeExpr &meExpr) {
  MeExpr *resultExpr = nullptr;
  uint32 hashIdx = meExpr.GetHashIndex() % mapHashLength;
  MeExpr *hashedExpr = hashTable[hashIdx];

  if (hashedExpr != nullptr && meExpr.GetMeOp() != kMeOpGcmalloc) {
    resultExpr = meExpr.GetIdenticalExpr(*hashedExpr, mirModule.CurFunction()->IsConstructor());
  }

  if (resultExpr == nullptr) {
    resultExpr = &meBuilder.CreateMeExpr(exprID++, meExpr);
    PutToBucket(hashIdx, *resultExpr);
  }
  return resultExpr;
}

MeExpr *IRMap::ReplaceMeExprExpr(MeExpr &origExpr, MeExpr &newExpr, size_t opndsSize,
                                 const MeExpr &meExpr, MeExpr &repExpr) {
  bool needRehash = false;

  for (size_t i = 0; i < opndsSize; ++i) {
    MeExpr *origOpnd = origExpr.GetOpnd(i);
    if (origOpnd == nullptr) {
      continue;
    }

    if (origOpnd == &meExpr) {
      needRehash = true;
      newExpr.SetOpnd(i, &repExpr);
    } else if (!origOpnd->IsLeaf()) {
      newExpr.SetOpnd(i, ReplaceMeExprExpr(*newExpr.GetOpnd(i), meExpr, repExpr));
      if (newExpr.GetOpnd(i) != origOpnd) {
        needRehash = true;
      }
    }
  }

  return needRehash ? HashMeExpr(newExpr) : &origExpr;
}

// replace meExpr with repexpr. meExpr must be a kid of origexpr
// return repexpr's parent if replaced, otherwise return nullptr
MeExpr *IRMap::ReplaceMeExprExpr(MeExpr &origExpr, const MeExpr &meExpr, MeExpr &repExpr) {
  if (origExpr.IsLeaf()) {
    return &origExpr;
  }

  switch (origExpr.GetMeOp()) {
    case kMeOpOp: {
      auto &opMeExpr = static_cast<OpMeExpr&>(origExpr);
      OpMeExpr newMeExpr(opMeExpr, kInvalidExprID);
      return ReplaceMeExprExpr(opMeExpr, newMeExpr, kOperandNumTernary, meExpr, repExpr);
    }
    case kMeOpNary: {
      auto &naryMeExpr = static_cast<NaryMeExpr&>(origExpr);
      NaryMeExpr newMeExpr(&irMapAlloc, kInvalidExprID, naryMeExpr);
      return ReplaceMeExprExpr(naryMeExpr, newMeExpr, naryMeExpr.GetOpnds().size(), meExpr, repExpr);
    }
    case kMeOpIvar: {
      auto &ivarExpr = static_cast<IvarMeExpr&>(origExpr);
      IvarMeExpr newMeExpr(kInvalidExprID, ivarExpr);
      bool needRehash = false;
      if (ivarExpr.GetBase() == &meExpr) {
        newMeExpr.SetBase(&repExpr);
        needRehash = true;
      } else if (!ivarExpr.GetBase()->IsLeaf()) {
        newMeExpr.SetBase(ReplaceMeExprExpr(*newMeExpr.GetBase(), meExpr, repExpr));
        if (newMeExpr.GetBase() != ivarExpr.GetBase()) {
          needRehash = true;
        }
      }
      return needRehash ? HashMeExpr(newMeExpr) : &origExpr;
    }
    default:
      ASSERT(false, "NYI");
      return nullptr;
  }
}

bool IRMap::ReplaceMeExprStmtOpnd(uint32 opndID, MeStmt &meStmt, const MeExpr &meExpr, MeExpr &repExpr) {
  MeExpr *opnd = meStmt.GetOpnd(opndID);

  if (opnd == &meExpr) {
    meStmt.SetOpnd(opndID, &repExpr);
    return true;
  } else if (!opnd->IsLeaf()) {
    meStmt.SetOpnd(opndID, ReplaceMeExprExpr(*opnd, meExpr, repExpr));
    return meStmt.GetOpnd(opndID) != opnd;
  }

  return false;
}

// replace meExpr in meStmt with repexpr
bool IRMap::ReplaceMeExprStmt(MeStmt &meStmt, const MeExpr &meExpr, MeExpr &repexpr) {
  bool isReplaced = false;
  Opcode op = meStmt.GetOp();

  for (size_t i = 0; i < meStmt.NumMeStmtOpnds(); ++i) {
    if (op == OP_intrinsiccall || op == OP_xintrinsiccall || op == OP_intrinsiccallwithtype ||
        op == OP_intrinsiccallassigned || op == OP_xintrinsiccallassigned ||
        op == OP_intrinsiccallwithtypeassigned) {
      MeExpr *opnd = meStmt.GetOpnd(i);
      if (opnd->IsLeaf() && opnd->GetMeOp() == kMeOpVar) {
        auto *varMeExpr = static_cast<VarMeExpr*>(opnd);
        const OriginalSt *ost = ssaTab.GetOriginalStFromID(varMeExpr->GetOStIdx());
        if (ost->IsSymbolOst() && ost->GetMIRSymbol()->GetAttr(ATTR_static)) {
          // its address may be taken
          continue;
        }
      }
    }

    bool curOpndReplaced = false;
    if (i == 0 && op == OP_iassign) {
      auto &ivarStmt = static_cast<IassignMeStmt&>(meStmt);
      MeExpr *oldBase = ivarStmt.GetLHS()->GetOpnd(0);
      MeExpr *newBase = nullptr;
      if (oldBase == &meExpr) {
        newBase = &repexpr;
        curOpndReplaced = true;
      } else if (!oldBase->IsLeaf()) {
        newBase = ReplaceMeExprExpr(*oldBase, meExpr, repexpr);
        curOpndReplaced = (newBase != oldBase);
      }
      if (curOpndReplaced) {
        ASSERT_NOT_NULL(newBase);
        ivarStmt.SetLHSVal(BuildLHSIvar(*newBase, ivarStmt, ivarStmt.GetLHSVal()->GetFieldID()));
      }
    } else {
      curOpndReplaced = ReplaceMeExprStmtOpnd(i, meStmt, meExpr, repexpr);
    }
    isReplaced = isReplaced || curOpndReplaced;
  }

  return isReplaced;
}

MePhiNode *IRMap::CreateMePhi(ScalarMeExpr &meExpr) {
  auto *phiMeVar = NewInPool<MePhiNode>();
  phiMeVar->UpdateLHS(meExpr);
  return phiMeVar;
}

DassignMeStmt *IRMap::CreateDassignMeStmt(MeExpr &lhs, MeExpr &rhs, BB &currBB) {
  auto *meStmt = NewInPool<DassignMeStmt>();
  meStmt->SetRHS(&rhs);
  auto &var = static_cast<VarMeExpr&>(lhs);
  meStmt->SetLHS(&var);
  var.SetDefBy(kDefByStmt);
  var.SetDefStmt(meStmt);
  meStmt->SetBB(&currBB);
  return meStmt;
}

IassignMeStmt *IRMap::CreateIassignMeStmt(TyIdx tyIdx, IvarMeExpr &lhs, MeExpr &rhs,
                                          const MapleMap<OStIdx, ChiMeNode*> &clist) {
  return NewInPool<IassignMeStmt>(tyIdx, &lhs, &rhs, &clist);
}

RegassignMeStmt *IRMap::CreateRegassignMeStmt(MeExpr &lhs, MeExpr &rhs, BB &currBB) {
  auto *meStmt = New<RegassignMeStmt>();
  ASSERT(lhs.GetMeOp() == kMeOpReg, "Create regassign without lhs == regread");
  meStmt->SetRHS(&rhs);
  auto &reg = static_cast<RegMeExpr&>(lhs);
  meStmt->SetLHS(&reg);
  reg.SetDefBy(kDefByStmt);
  reg.SetDefStmt(meStmt);
  meStmt->SetBB(&currBB);
  return meStmt;
}

// get the false goto bb, if condgoto is brtrue, take the other bb of brture @lable
// otherwise, take the bb of @lable
BB *IRMap::GetFalseBrBB(const CondGotoMeStmt &condgoto) {
  LabelIdx lblIdx = (LabelIdx)condgoto.GetOffset();
  BB *gotoBB = GetBBForLabIdx(lblIdx);
  BB *bb = condgoto.GetBB();
  ASSERT(bb->GetSucc().size() == kBBVectorInitialSize, "array size error");
  if (condgoto.GetOp() == OP_brfalse) {
    return gotoBB;
  } else {
    return gotoBB == bb->GetSucc(0) ? bb->GetSucc(1) : bb->GetSucc(0);
  }
}

MeExpr *IRMap::CreateConstMeExpr(PrimType pType, MIRConst &mirConst) {
  ConstMeExpr constMeExpr(kInvalidExprID, &mirConst);
  constMeExpr.SetOp(OP_constval);
  constMeExpr.SetPtyp(pType);
  return HashMeExpr(constMeExpr);
}

MeExpr *IRMap::CreateIntConstMeExpr(int64 value, PrimType pType) {
  auto *intConst =
      GlobalTables::GetIntConstTable().GetOrCreateIntConst(value, *GlobalTables::GetTypeTable().GetPrimType(pType));
  return CreateConstMeExpr(pType, *intConst);
}

MeExpr *IRMap::CreateMeExprBinary(Opcode op, PrimType pType, MeExpr &expr0, MeExpr &expr1) {
  OpMeExpr opMeExpr(kInvalidExprID);
  opMeExpr.SetOpnd(0, &expr0);
  opMeExpr.SetOpnd(1, &expr1);
  opMeExpr.InitBase(op, pType, kOperandNumBinary);
  return HashMeExpr(opMeExpr);
}

MeExpr *IRMap::CreateMeExprSelect(PrimType pType, MeExpr &expr0, MeExpr &expr1, MeExpr &expr2) {
  OpMeExpr opMeExpr(kInvalidExprID);
  opMeExpr.SetOpnd(0, &expr0);
  opMeExpr.SetOpnd(1, &expr1);
  opMeExpr.SetOpnd(2, &expr2);
  opMeExpr.InitBase(OP_select, pType, kOperandNumTernary);
  return HashMeExpr(opMeExpr);
}

MeExpr *IRMap::CreateMeExprCompare(Opcode op, PrimType resptyp, PrimType opndptyp, MeExpr &opnd0, MeExpr &opnd1) {
  OpMeExpr opMeExpr(kInvalidExprID);
  opMeExpr.SetOpnd(0, &opnd0);
  opMeExpr.SetOpnd(1, &opnd1);
  opMeExpr.InitBase(op, resptyp, kOperandNumBinary);
  opMeExpr.SetOpndType(opndptyp);
  MeExpr *retMeExpr = HashMeExpr(opMeExpr);
  static_cast<OpMeExpr*>(retMeExpr)->SetOpndType(opndptyp);
  return retMeExpr;
}

MeExpr *IRMap::CreateMeExprTypeCvt(PrimType pType, PrimType opndptyp, MeExpr &opnd0) {
  OpMeExpr opMeExpr(kInvalidExprID);
  opMeExpr.SetOpnd(0, &opnd0);
  opMeExpr.InitBase(OP_cvt, pType, kOperandNumUnary);
  opMeExpr.SetOpndType(opndptyp);
  return HashMeExpr(opMeExpr);
}

IntrinsiccallMeStmt *IRMap::CreateIntrinsicCallMeStmt(MIRIntrinsicID idx, std::vector<MeExpr*> &opnds, TyIdx tyIdx) {
  auto *meStmt =
      NewInPool<IntrinsiccallMeStmt>(tyIdx == 0u ? OP_intrinsiccall : OP_intrinsiccallwithtype, idx, tyIdx);
  for (MeExpr *opnd : opnds) {
    meStmt->PushBackOpnd(opnd);
  }
  return meStmt;
}

IntrinsiccallMeStmt *IRMap::CreateIntrinsicCallAssignedMeStmt(MIRIntrinsicID idx, std::vector<MeExpr*> &opnds,
                                                              MeExpr *ret, TyIdx tyIdx) {
  auto *meStmt = NewInPool<IntrinsiccallMeStmt>(
      tyIdx == 0u ? OP_intrinsiccallassigned : OP_intrinsiccallwithtypeassigned, idx, tyIdx);
  for (MeExpr *opnd : opnds) {
    meStmt->PushBackOpnd(opnd);
  }
  if (ret != nullptr) {
    ASSERT(ret->GetMeOp() == kMeOpReg || ret->GetMeOp() == kMeOpVar, "unexpected opcode");
    auto *mustDef = New<MustDefMeNode>(ret, meStmt);
    meStmt->GetMustDefList()->push_back(*mustDef);
  }
  return meStmt;
}

MeExpr *IRMap::CreateAddrofMeExprFromSymbol(MIRSymbol &st, PUIdx puIdx) {
  OriginalSt *baseOst = ssaTab.FindOrCreateSymbolOriginalSt(st, puIdx, 0);
  AddrofMeExpr addrOfMe(kInvalidExprID, baseOst->GetIndex());
  addrOfMe.SetOp(OP_addrof);
  addrOfMe.SetPtyp(PTY_ptr);
  return HashMeExpr(addrOfMe);
}

bool IRMap::InitMeStmtFactory() {
  RegisterFactoryFunction<MeStmtFactory>(OP_dassign, &IRMap::BuildDassignMeStmt);
  RegisterFactoryFunction<MeStmtFactory>(OP_regassign, &IRMap::BuildRegassignMeStmt);
  RegisterFactoryFunction<MeStmtFactory>(OP_iassign, &IRMap::BuildIassignMeStmt);
  RegisterFactoryFunction<MeStmtFactory>(OP_maydassign, &IRMap::BuildMaydassignMeStmt);
  RegisterFactoryFunction<MeStmtFactory>(OP_call, &IRMap::BuildCallMeStmt);
  RegisterFactoryFunction<MeStmtFactory>(OP_virtualcall, &IRMap::BuildCallMeStmt);
  RegisterFactoryFunction<MeStmtFactory>(OP_virtualicall, &IRMap::BuildCallMeStmt);
  RegisterFactoryFunction<MeStmtFactory>(OP_superclasscall, &IRMap::BuildCallMeStmt);
  RegisterFactoryFunction<MeStmtFactory>(OP_interfacecall, &IRMap::BuildCallMeStmt);
  RegisterFactoryFunction<MeStmtFactory>(OP_interfaceicall, &IRMap::BuildCallMeStmt);
  RegisterFactoryFunction<MeStmtFactory>(OP_customcall, &IRMap::BuildCallMeStmt);
  RegisterFactoryFunction<MeStmtFactory>(OP_polymorphiccall, &IRMap::BuildCallMeStmt);
  RegisterFactoryFunction<MeStmtFactory>(OP_callassigned, &IRMap::BuildCallMeStmt);
  RegisterFactoryFunction<MeStmtFactory>(OP_virtualcallassigned, &IRMap::BuildCallMeStmt);
  RegisterFactoryFunction<MeStmtFactory>(OP_virtualicallassigned, &IRMap::BuildCallMeStmt);
  RegisterFactoryFunction<MeStmtFactory>(OP_superclasscallassigned, &IRMap::BuildCallMeStmt);
  RegisterFactoryFunction<MeStmtFactory>(OP_interfacecallassigned, &IRMap::BuildCallMeStmt);
  RegisterFactoryFunction<MeStmtFactory>(OP_interfaceicallassigned, &IRMap::BuildCallMeStmt);
  RegisterFactoryFunction<MeStmtFactory>(OP_customcallassigned, &IRMap::BuildCallMeStmt);
  RegisterFactoryFunction<MeStmtFactory>(OP_polymorphiccallassigned, &IRMap::BuildCallMeStmt);
  RegisterFactoryFunction<MeStmtFactory>(OP_icall, &IRMap::BuildNaryMeStmt);
  RegisterFactoryFunction<MeStmtFactory>(OP_icallassigned, &IRMap::BuildNaryMeStmt);
  RegisterFactoryFunction<MeStmtFactory>(OP_intrinsiccall, &IRMap::BuildNaryMeStmt);
  RegisterFactoryFunction<MeStmtFactory>(OP_xintrinsiccall, &IRMap::BuildNaryMeStmt);
  RegisterFactoryFunction<MeStmtFactory>(OP_intrinsiccallwithtype, &IRMap::BuildNaryMeStmt);
  RegisterFactoryFunction<MeStmtFactory>(OP_intrinsiccallassigned, &IRMap::BuildNaryMeStmt);
  RegisterFactoryFunction<MeStmtFactory>(OP_intrinsiccallwithtypeassigned, &IRMap::BuildNaryMeStmt);
  RegisterFactoryFunction<MeStmtFactory>(OP_return, &IRMap::BuildRetMeStmt);
  RegisterFactoryFunction<MeStmtFactory>(OP_retsub, &IRMap::BuildWithMuMeStmt);
  RegisterFactoryFunction<MeStmtFactory>(OP_gosub, &IRMap::BuildGosubMeStmt);
  RegisterFactoryFunction<MeStmtFactory>(OP_throw, &IRMap::BuildThrowMeStmt);
  RegisterFactoryFunction<MeStmtFactory>(OP_syncenter, &IRMap::BuildSyncMeStmt);
  RegisterFactoryFunction<MeStmtFactory>(OP_syncexit, &IRMap::BuildSyncMeStmt);
  return true;
}
}  // namespace maple
