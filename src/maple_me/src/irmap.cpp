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
  BBId bbid = bb.GetBBId();
  if (bbIRMapProcessed[bbid]) {
    return;
  }
  bbIRMapProcessed[bbid] = true;
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
  ASSERT(bbid < dom.GetDomChildrenSize(), " index out of range in IRMap::BuildBB");
  const MapleSet<BBId> &domChildren = dom.GetDomChildren(bbid);
  for (auto bbit = domChildren.begin(); bbit != domChildren.end(); ++bbit) {
    BBId childbbid = *bbit;
    BuildBB(*GetBB(childbbid), bbIRMapProcessed);
  }
}

void IRMap::BuildPhiMeNode(BB &bb) {
  for (auto &phi : bb.GetPhiList()) {
    const OriginalSt *origst = phi.first;
    VersionSt *verSt = phi.second.GetResult();
    if (origst->IsPregOst()) {
      MeRegPhiNode *phiMeNode = NewInPool<MeRegPhiNode>();
      RegMeExpr *medef = GetOrCreateRegFromVerSt(*verSt);
      phiMeNode->UpdateLHS(*medef);
      phiMeNode->SetDefBB(&bb);
      // build phi operands
      for (VersionSt *opnd : phi.second.GetPhiOpnds()) {
        phiMeNode->GetOpnds().push_back(GetOrCreateRegFromVerSt(*opnd));
      }
      bb.GetMeregphiList().insert(std::make_pair(medef->GetOstIdx(), phiMeNode));
    } else {
      MeVarPhiNode *phimenode = NewInPool<MeVarPhiNode>();
      VarMeExpr *medef = GetOrCreateVarFromVerSt(*verSt);
      phimenode->UpdateLHS(*medef);
      phimenode->SetDefBB(&bb);
      // build phi operands
      for (VersionSt *opnd : phi.second.GetPhiOpnds()) {
        phimenode->GetOpnds().push_back(GetOrCreateVarFromVerSt(*opnd));
      }
      bb.GetMevarPhiList().insert(std::make_pair(medef->GetOStIdx(), phimenode));
    }
  }
}

VarMeExpr *IRMap::CreateVarMeExprVersion(const VarMeExpr &origExpr) {
  VarMeExpr *varmeexpr = New<VarMeExpr>(&irMapAlloc, exprID++, origExpr.GetOStIdx(), verst2MeExprTable.size());
  verst2MeExprTable.push_back(varmeexpr);
  varmeexpr->InitBase(origExpr.GetOp(), origExpr.GetPrimType(), origExpr.GetNumOpnds());
  varmeexpr->SetFieldID(origExpr.GetFieldID());
  return varmeexpr;
}

MeExpr *IRMap::CreateAddrofMeExpr(MeExpr &expr) {
  if (expr.GetMeOp() == kMeOpVar) {
    VarMeExpr &varmeexpr = static_cast<VarMeExpr&>(expr);
    AddrofMeExpr addrofme(kInvalidExprID, varmeexpr.GetOStIdx());
    addrofme.SetOp(OP_addrof);
    addrofme.SetPtyp(PTY_ptr);
    return HashMeExpr(addrofme);
  } else {
    ASSERT(expr.GetMeOp() == kMeOpIvar, "expecting IVarMeExpr");
    IvarMeExpr &ivarexpr = static_cast<IvarMeExpr&>(expr);
    OpMeExpr opmeexpr(kInvalidExprID);
    opmeexpr.SetFieldID(ivarexpr.GetFieldID());
    opmeexpr.SetTyIdx(ivarexpr.GetTyIdx());
    opmeexpr.SetOpnd(0, ivarexpr.GetBase());
    opmeexpr.SetOp(OP_iaddrof);
    opmeexpr.SetPtyp(PTY_ptr);
    opmeexpr.SetNumOpnds(1);
    return HashMeExpr(opmeexpr);
  }
}

VarMeExpr *IRMap::CreateNewVarMeExpr(OStIdx oStIdx, PrimType pType, FieldID fieldID) {
  VarMeExpr *varMeExpr = meBuilder.BuildVarMeExpr(exprID++, oStIdx, verst2MeExprTable.size(), pType, fieldID);
  PushBackVerst2MeExprTable(varMeExpr);
  return varMeExpr;
}

VarMeExpr *IRMap::CreateNewVarMeExpr(OriginalSt &oSt, PrimType pType, FieldID fieldID) {
  VarMeExpr *varMeExpr = CreateNewVarMeExpr(oSt.GetIndex(), pType, fieldID);
  oSt.PushbackVersionIndex(varMeExpr->GetVstIdx());
  return varMeExpr;
}

VarMeExpr *IRMap::CreateNewGlobalTmp(GStrIdx strIdx, PrimType ptyp) {
  MIRSymbol *st =
      mirModule.GetMIRBuilder()->CreateSymbol((TyIdx)ptyp, strIdx, kStVar, kScGlobal, nullptr, kScopeGlobal);
  st->SetIsTmp(true);
  OriginalSt *oSt = ssaTab.CreateSymbolOriginalSt(*st, 0, 0);
  VarMeExpr *varx = New<VarMeExpr>(&irMapAlloc, exprID++, oSt->GetIndex(), oSt->GetZeroVersionIndex());
  varx->InitBase(OP_dread, ptyp, 0);
  return varx;
}

VarMeExpr *IRMap::CreateNewLocalRefVarTmp(GStrIdx strIdx, TyIdx tIdx) {
  MIRSymbol *st =
      mirModule.GetMIRBuilder()->CreateSymbol(tIdx, strIdx, kStVar, kScAuto, mirModule.CurFunction(), kScopeLocal);
  st->SetInstrumented();
  OriginalSt *oSt = ssaTab.CreateSymbolOriginalSt(*st, mirModule.CurFunction()->GetPuidx(), 0);
  oSt->SetZeroVersionIndex(verst2MeExprTable.size());
  verst2MeExprTable.push_back(nullptr);
  oSt->PushbackVersionIndex(oSt->GetZeroVersionIndex());
  VarMeExpr *newlocalrefvar = New<VarMeExpr>(&irMapAlloc, exprID++, oSt->GetIndex(), verst2MeExprTable.size());
  verst2MeExprTable.push_back(newlocalrefvar);
  newlocalrefvar->InitBase(OP_dread, PTY_ref, 0);
  return newlocalrefvar;
}

RegMeExpr *IRMap::CreateRegMeExprVersion(const OriginalSt &pregOrgst) {
  RegMeExpr *regreadexpr =
      NewInPool<RegMeExpr>(exprID++, pregOrgst.GetPregIdx(), pregOrgst.GetPuIdx(), pregOrgst.GetIndex(), 0);
  regreadexpr->InitBase(OP_regread, pregOrgst.GetMIRPreg()->GetPrimType(), 0);
  regMeExprTable.push_back(regreadexpr);
  return regreadexpr;
}

RegMeExpr *IRMap::CreateRegMeExprVersion(const RegMeExpr &origExpr) {
  RegMeExpr *regreadexpr =
      NewInPool<RegMeExpr>(exprID++, origExpr.GetRegIdx(), origExpr.GetPuIdx(), origExpr.GetOstIdx(), 0);
  regreadexpr->InitBase(origExpr.GetOp(), origExpr.GetPrimType(), origExpr.GetNumOpnds());
  regMeExprTable.push_back(regreadexpr);
  return regreadexpr;
}

RegMeExpr *IRMap::CreateRefRegMeExpr(const MIRSymbol &mirSt) {
  MIRFunction *mirfunc = mirModule.CurFunction();
  MIRType *sttype = mirSt.GetType();
  PrimType ptyp = sttype->GetPrimType();
  ASSERT(ptyp == PTY_ref, "only PTY_ref needed");
  PregIdx regidx = mirfunc->GetPregTab()->CreateRefPreg(*sttype);
  ASSERT(regidx <= 0xffff, "register oversized");
  MIRPreg *preg = mirfunc->GetPregTab()->PregFromPregIdx(regidx);
  if (!mirSt.IgnoreRC()) {
    preg->SetNeedRC();
  }
  OriginalSt *oSt = ssaTab.GetOriginalStTable().CreatePregOriginalSt(regidx, mirfunc->GetPuidx());
  RegMeExpr *regreadexpr = NewInPool<RegMeExpr>(exprID++, regidx, mirfunc->GetPuidx(), oSt->GetIndex(), 0);
  regreadexpr->InitBase(OP_regread, ptyp, 0);
  regMeExprTable.push_back(regreadexpr);
  return regreadexpr;
}

RegMeExpr *IRMap::CreateRegMeExpr(PrimType ptyp) {
  MIRFunction *mirfunc = mirModule.CurFunction();
  PregIdx regidx = mirfunc->GetPregTab()->CreatePreg(ptyp);
  ASSERT(regidx <= 0xffff, "register oversized");
  OriginalSt *oSt = ssaTab.GetOriginalStTable().CreatePregOriginalSt(regidx, mirfunc->GetPuidx());
  RegMeExpr *regreadexpr = NewInPool<RegMeExpr>(exprID++, regidx, mirfunc->GetPuidx(), oSt->GetIndex(), 0);
  regreadexpr->InitBase(OP_regread, ptyp, 0);
  regMeExprTable.push_back(regreadexpr);
  return regreadexpr;
}

RegMeExpr *IRMap::CreateRegRefMeExpr(MIRType &mirType) {
  MIRFunction *mirfunc = mirModule.CurFunction();
  PregIdx regidx = mirfunc->GetPregTab()->CreateRefPreg(mirType);
  ASSERT(regidx <= 0xffff, "register oversized");
  OriginalSt *oSt = ssaTab.GetOriginalStTable().CreatePregOriginalSt(regidx, mirfunc->GetPuidx());
  RegMeExpr *regreadexpr = NewInPool<RegMeExpr>(exprID++, regidx, mirfunc->GetPuidx(), oSt->GetIndex(), 0);
  regreadexpr->InitBase(OP_regread, mirType.GetPrimType(), 0);
  regMeExprTable.push_back(regreadexpr);
  return regreadexpr;
}

RegMeExpr *IRMap::CreateRegRefMeExpr(MeExpr &meExpr) {
  MIRType *mirType = nullptr;
  switch (meExpr.GetMeOp()) {
    case kMeOpVar: {
      VarMeExpr &varmeexpr = static_cast<VarMeExpr&>(meExpr);
      const OriginalSt *oSt = ssaTab.GetOriginalStFromID(varmeexpr.GetOStIdx());
      ASSERT(oSt->GetTyIdx() != 0, "expect oSt->tyIdx to be initialized");
      mirType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(oSt->GetTyIdx());
      break;
    }
    case kMeOpIvar: {
      IvarMeExpr &ivarmeexpr = static_cast<IvarMeExpr&>(meExpr);
      MIRType *ptrmirtype = GlobalTables::GetTypeTable().GetTypeFromTyIdx(ivarmeexpr.GetTyIdx());
      ASSERT(ptrmirtype->GetKind() == kTypePointer, "must be point type for ivar");
      MIRPtrType *realmirtype = static_cast<MIRPtrType*>(ptrmirtype);
      FieldID fieldID = ivarmeexpr.GetFieldID();
      if (fieldID > 0) {
        mirType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(realmirtype->GetPointedTyIdxWithFieldID(fieldID));
      } else {
        mirType = realmirtype->GetPointedType();
      }
      ASSERT(mirType->GetPrimType() == meExpr.GetPrimType() ||
             !(IsAddress(mirType->GetPrimType()) && IsAddress(meExpr.GetPrimType())),
             "inconsistent type");
      ASSERT(mirType->GetPrimType() == PTY_ref, "CreateRegRefMeExpr: only ref type expected");
      break;
    }
    case kMeOpOp:
      if (meExpr.GetOp() == OP_retype) {
        OpMeExpr &opmeexpr = static_cast<OpMeExpr&>(meExpr);
        ASSERT(opmeexpr.GetTyIdx() != 0, "expect opmeexpr.tyIdx to be initialized");
        mirType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(opmeexpr.GetTyIdx());
        break;
      }
    // fall thru
    default:
      return CreateRegMeExpr(PTY_ptr);
  }
  return CreateRegRefMeExpr(*mirType);
}

VarMeExpr *IRMap::GetOrCreateVarFromVerSt(const VersionSt &verSt) {
  size_t vindex = verSt.GetIndex();
  ASSERT(vindex < verst2MeExprTable.size(), "GetOrCreateVarFromVerSt: index %d is out of range", vindex);
  MeExpr *meExpr = verst2MeExprTable.at(vindex);
  if (meExpr != nullptr) {
    return static_cast<VarMeExpr*>(meExpr);
  }
  const OriginalSt *oSt = verSt.GetOrigSt();
  ASSERT(oSt->IsSymbolOst(), "GetOrCreateVarFromVerSt: wrong ost_type");
  VarMeExpr *varx = New<VarMeExpr>(&irMapAlloc, exprID++, oSt->GetIndex(), vindex);
  ASSERT(!GlobalTables::GetTypeTable().GetTypeTable().empty(), "container check");
  varx->InitBase(OP_dread, GlobalTables::GetTypeTable().GetTypeFromTyIdx(oSt->GetTyIdx())->GetPrimType(), 0);
  varx->SetFieldID(oSt->GetFieldID());
  verst2MeExprTable[vindex] = varx;
  return varx;
}

VarMeExpr *IRMap::GetOrCreateZeroVersionVarMeExpr(const OriginalSt &oSt) {
  ASSERT(oSt.GetZeroVersionIndex() < verst2MeExprTable.size(),
         "GetOrCreateZeroVersionVarMeExpr: version index of osym's kInitVersion out of range");
  if (oSt.GetZeroVersionIndex() == 0) {
    ssaTab.SetZeroVersionIndex(oSt.GetIndex(), verst2MeExprTable.size());
    verst2MeExprTable.push_back(nullptr);
  }
  if (verst2MeExprTable[oSt.GetZeroVersionIndex()] == nullptr) {
    VarMeExpr *varmeexpr = New<VarMeExpr>(&irMapAlloc, exprID++, oSt.GetIndex(), oSt.GetZeroVersionIndex());
    varmeexpr->SetFieldID(oSt.GetFieldID());
    varmeexpr->SetOp(OP_dread);
    ASSERT(!GlobalTables::GetTypeTable().GetTypeTable().empty(), "container check");
    varmeexpr->SetPtyp(GlobalTables::GetTypeTable().GetTypeFromTyIdx(oSt.GetTyIdx())->GetPrimType());
    varmeexpr->SetNumOpnds(0);
    verst2MeExprTable[oSt.GetZeroVersionIndex()] = varmeexpr;
    return varmeexpr;
  }
  return static_cast<VarMeExpr*>(verst2MeExprTable[oSt.GetZeroVersionIndex()]);
}

RegMeExpr *IRMap::GetOrCreateRegFromVerSt(const VersionSt &verSt) {
  size_t vindex = verSt.GetIndex();
  ASSERT(vindex < verst2MeExprTable.size(), " GetOrCreateRegFromVerSt: index %d is out of range", vindex);
  MeExpr *meExpr = verst2MeExprTable[vindex];
  if (meExpr != nullptr) {
    return static_cast<RegMeExpr*>(meExpr);
  }
  const OriginalSt *oSt = verSt.GetOrigSt();
  ASSERT(oSt->IsPregOst(), "GetOrCreateRegFromVerSt: PregOST expected");
  RegMeExpr *regx =
      NewInPool<RegMeExpr>(exprID++, oSt->GetPregIdx(), mirModule.CurFunction()->GetPuidx(), oSt->GetIndex(), vindex);
  regx->InitBase(OP_regread, oSt->GetMIRPreg()->GetPrimType(), 0);
  regMeExprTable.push_back(regx);
  verst2MeExprTable[vindex] = regx;
  return regx;
}

MeExpr *IRMap::BuildLHSVar(const VersionSt &verSt, DassignMeStmt &defMeStmt) {
  VarMeExpr *medef = GetOrCreateVarFromVerSt(verSt);
  medef->SetDefStmt(&defMeStmt);
  medef->SetDefBy(kDefByStmt);
  verst2MeExprTable.at(verSt.GetIndex()) = medef;
  return medef;
}

MeExpr *IRMap::BuildLHSReg(const VersionSt &verSt, RegassignMeStmt &defMeStmt, const RegassignNode &regassign) {
  RegMeExpr *medef = GetOrCreateRegFromVerSt(verSt);
  medef->SetPtyp(regassign.GetPrimType());
  medef->SetDefStmt(&defMeStmt);
  medef->SetDefBy(kDefByStmt);
  verst2MeExprTable.at(verSt.GetIndex()) = medef;
  return medef;
}

IvarMeExpr *IRMap::BuildLHSIvar(MeExpr &baseAddr, IassignMeStmt &iassignMeStmt, FieldID fieldID) {
  IvarMeExpr *medef = New<IvarMeExpr>(exprID++);
  medef->SetFieldID(fieldID);
  medef->SetTyIdx(iassignMeStmt.GetTyIdx());
  medef->SetBase(&baseAddr);
  medef->SetDefStmt(&iassignMeStmt);
  medef->SetOp(OP_iread);
  medef->SetPtyp(iassignMeStmt.GetRHS()->GetPrimType());
  PutToBucket(medef->GetHashIndex() % mapHashLength, *medef);
  return medef;
}

IvarMeExpr *IRMap::BuildLHSIvarFromIassMeStmt(IassignMeStmt &iassignMeStmt) {
  return BuildLHSIvar(*iassignMeStmt.GetLHSVal()->GetBase(), iassignMeStmt, iassignMeStmt.GetLHSVal()->GetFieldID());
}

// build Me chilist from MayDefNode list
void IRMap::BuildChiList(MeStmt &meStmt, MapleMap<OStIdx, MayDefNode> &mayDefNodes,
                         MapleMap<OStIdx, ChiMeNode*> &outList) {
  for (auto it = mayDefNodes.begin(); it != mayDefNodes.end(); ++it) {
    MayDefNode &maydefNode = it->second;
    VersionSt *opndst = maydefNode.GetOpnd();
    VersionSt *resst = maydefNode.GetResult();
    ChiMeNode *chimestmt = New<ChiMeNode>(&meStmt);
    chimestmt->SetRHS(GetOrCreateVarFromVerSt(*opndst));
    VarMeExpr *lhs = GetOrCreateVarFromVerSt(*resst);
    lhs->SetDefBy(kDefByChi);
    lhs->SetDefChi(*chimestmt);
    chimestmt->SetLHS(lhs);
    outList.insert(std::make_pair(lhs->GetOStIdx(), chimestmt));
  }
}

void IRMap::BuildMustDefList(MeStmt &meStmt, MapleVector<MustDefNode> &mustdeflist,
                             MapleVector<MustDefMeNode> &mustdefList) {
  for (auto it = mustdeflist.begin(); it != mustdeflist.end(); ++it) {
    MustDefNode &mustdefnode = *it;
    VersionSt *verSt = mustdefnode.GetResult();
    VarMeExpr *lhs = GetOrCreateVarFromVerSt(*verSt);
    ASSERT(lhs->GetMeOp() == kMeOpReg || lhs->GetMeOp() == kMeOpVar, "unexpected opcode");
    mustdefList.push_back(MustDefMeNode(lhs, &meStmt));
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
      CondGotoNode &condGotoNode = static_cast<CondGotoNode&>(stmt);
      CondGotoMeStmt *condGotoMeStmt = New<CondGotoMeStmt>(&stmt);
      condGotoMeStmt->SetOpnd(0, BuildExpr(*condGotoNode.Opnd(0)));
      return condGotoMeStmt;
    }
    case OP_try: {
      TryNode &tryNode = static_cast<TryNode&>(stmt);
      TryMeStmt *tryMeStmt = NewInPool<TryMeStmt>(&stmt);
      for (size_t i = 0; i < tryNode.GetOffsetsCount(); ++i) {
        tryMeStmt->OffsetsPushBack(tryNode.GetOffset(i));
      }
      return tryMeStmt;
    }
    case OP_assertnonnull:
    case OP_eval:
    case OP_free:
    case OP_switch: {
      UnaryStmtNode &unaryStmt = static_cast<UnaryStmtNode&>(stmt);
      UnaryMeStmt *unMeStmt =
          static_cast<UnaryMeStmt*>((op == OP_switch) ? NewInPool<SwitchMeStmt>(&stmt) : New<UnaryMeStmt>(&stmt));
      unMeStmt->SetOpnd(0, BuildExpr(*unaryStmt.Opnd(0)));
      return unMeStmt;
    }
    default:
      CHECK_FATAL(false, "NYI");
  }
}

MeStmt *IRMap::BuildDassignMeStmt(StmtNode &stmt, AccessSSANodes &ssaPart) {
  DassignMeStmt *meStmt = NewInPool<DassignMeStmt>(&stmt);
  DassignNode &dassiNode = static_cast<DassignNode&>(stmt);
  meStmt->SetRHS(BuildExpr(*dassiNode.GetRHS()));
  VarMeExpr *varlhs = static_cast<VarMeExpr*>(BuildLHSVar(*ssaPart.GetSSAVar(), *meStmt));
  meStmt->SetLHS(varlhs);
  BuildChiList(*meStmt, ssaPart.GetMayDefNodes(), *meStmt->GetChiList());
  return meStmt;
}

MeStmt *IRMap::BuildRegassignMeStmt(StmtNode &stmt, AccessSSANodes &ssaPart) {
  RegassignMeStmt *meStmt = New<RegassignMeStmt>(&stmt);
  RegassignNode &regNode = static_cast<RegassignNode&>(stmt);
  meStmt->SetRHS(BuildExpr(*regNode.Opnd(0)));
  RegMeExpr *regLHS = static_cast<RegMeExpr*>(BuildLHSReg(*ssaPart.GetSSAVar(), *meStmt, regNode));
  meStmt->SetLHS(regLHS);
  return meStmt;
}

MeStmt *IRMap::BuildIassignMeStmt(StmtNode &stmt, AccessSSANodes &ssaPart) {
  IassignNode &iasNode = static_cast<IassignNode&>(stmt);
  IassignMeStmt *meStmt = NewInPool<IassignMeStmt>(&stmt);
  meStmt->SetTyIdx(iasNode.GetTyIdx());
  meStmt->SetRHS(BuildExpr(*iasNode.GetRHS()));
  meStmt->SetLHSVal(BuildLHSIvar(*BuildExpr(*iasNode.Opnd(0)), *meStmt, iasNode.GetFieldID()));
  BuildChiList(*meStmt, ssaPart.GetMayDefNodes(), *(meStmt->GetChiList()));
  return meStmt;
}

MeStmt *IRMap::BuildMaydassignMeStmt(StmtNode &stmt, AccessSSANodes &ssaPart) {
  MaydassignMeStmt *meStmt = NewInPool<MaydassignMeStmt>(&stmt);
  DassignNode &dassiNode = static_cast<DassignNode&>(stmt);
  meStmt->SetRHS(BuildExpr(*dassiNode.GetRHS()));
  meStmt->SetMayDassignSym(ssaPart.GetSSAVar()->GetOrigSt());
  meStmt->SetFieldID(dassiNode.GetFieldID());
  BuildChiList(*meStmt, ssaPart.GetMayDefNodes(), *(meStmt->GetChiList()));
  return meStmt;
}

MeStmt *IRMap::BuildCallMeStmt(StmtNode &stmt, AccessSSANodes &ssaPart) {
  CallMeStmt *callMeStmt = NewInPool<CallMeStmt>(&stmt);
  CallNode &intrinNode = static_cast<CallNode&>(stmt);
  callMeStmt->SetPUIdx(intrinNode.GetPUIdx());
  for (size_t i = 0; i < intrinNode.NumOpnds(); ++i) {
    callMeStmt->GetOpnds().push_back(BuildExpr(*intrinNode.Opnd(i)));
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
  NaryStmtNode &naryStmtNode = static_cast<NaryStmtNode&>(stmt);
  for (size_t i = 0; i < naryStmtNode.NumOpnds(); ++i) {
    naryMeStmt->GetOpnds().push_back(BuildExpr(*naryStmtNode.Opnd(i)));
  }
  BuildMuList(ssaPart.GetMayUseNodes(), *(naryMeStmt->GetMuList()));
  if (kOpcodeInfo.IsCallAssigned(op)) {
    BuildMustDefList(*naryMeStmt, ssaPart.GetMustDefNodes(), *(naryMeStmt->GetMustDefList()));
  }
  BuildChiList(*naryMeStmt, ssaPart.GetMayDefNodes(), *(naryMeStmt->GetChiList()));
  return naryMeStmt;
}

MeStmt *IRMap::BuildRetMeStmt(StmtNode &stmt, AccessSSANodes &ssaPart) {
  NaryStmtNode &retStmt = static_cast<NaryStmtNode&>(stmt);
  RetMeStmt *meStmt = NewInPool<RetMeStmt>(&stmt);
  for (size_t i = 0; i < retStmt.NumOpnds(); ++i) {
    meStmt->GetOpnds().push_back(BuildExpr(*retStmt.Opnd(i)));
  }
  BuildMuList(ssaPart.GetMayUseNodes(), *(meStmt->GetMuList()));
  return meStmt;
}

MeStmt *IRMap::BuildWithMuMeStmt(StmtNode &stmt, AccessSSANodes &ssaPart) {
  WithMuMeStmt *retSub = NewInPool<WithMuMeStmt>(&stmt);
  BuildMuList(ssaPart.GetMayUseNodes(), *(retSub->GetMuList()));
  return retSub;
}

MeStmt *IRMap::BuildGosubMeStmt(StmtNode &stmt, AccessSSANodes &ssaPart) {
  GosubMeStmt *goSub = NewInPool<GosubMeStmt>(&stmt);
  BuildMuList(ssaPart.GetMayUseNodes(), *(goSub->GetMuList()));
  return goSub;
}

MeStmt *IRMap::BuildThrowMeStmt(StmtNode &stmt, AccessSSANodes &ssaPart) {
  UnaryStmtNode &unaryNode = static_cast<UnaryStmtNode&>(stmt);
  ThrowMeStmt *tmeStmt = NewInPool<ThrowMeStmt>(&stmt);
  tmeStmt->SetMeStmtOpndValue(BuildExpr(*unaryNode.Opnd(0)));
  BuildMuList(ssaPart.GetMayUseNodes(), *(tmeStmt->GetMuList()));
  return tmeStmt;
}

MeStmt *IRMap::BuildSyncMeStmt(StmtNode &stmt, AccessSSANodes &ssaPart) {
  NaryStmtNode &naryNode = static_cast<NaryStmtNode&>(stmt);
  SyncMeStmt *naryStmt = NewInPool<SyncMeStmt>(&stmt);
  for (size_t i = 0; i < naryNode.NumOpnds(); ++i) {
    naryStmt->GetOpnds().push_back(BuildExpr(*naryNode.Opnd(i)));
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
  ASSERT(func != nullptr, "NYI");
  return func != nullptr ? func(this, stmt, *ssaPart) : nullptr;
}

void IRMap::BuildMuList(MapleMap<OStIdx, MayUseNode> &mayuseList, MapleMap<OStIdx, VarMeExpr*> &mulist) {
  for (std::pair<OStIdx, MayUseNode> mapitem : mayuseList) {
    MayUseNode &mayusenode = mapitem.second;
    VersionSt *verSt = mayusenode.GetOpnd();
    VarMeExpr *varmeexpr = GetOrCreateVarFromVerSt(*verSt);
    mulist.insert(std::make_pair(varmeexpr->GetOStIdx(), varmeexpr));
  }
}

MeExpr *IRMap::BuildExpr(BaseNode &mirNode) {
  Opcode op = mirNode.GetOpCode();
  if (op == OP_dread) {
    AddrofSSANode &addrofnode = static_cast<AddrofSSANode &>(mirNode);
    VersionSt *verSt = addrofnode.GetSSAVar();
    VarMeExpr *varmeexpr = GetOrCreateVarFromVerSt(*verSt);
    varmeexpr->InitBase(mirNode.GetOpCode(), mirNode.GetPrimType(), mirNode.GetNumOpnds());
    if (verSt->GetOrigSt()->IsRealSymbol()) {
      ASSERT(!verSt->GetOrigSt()->IsPregOst(), "not expect preg symbol here");
      varmeexpr->SetPtyp(GlobalTables::GetTypeTable().GetTypeFromTyIdx(verSt->GetOrigSt()->GetTyIdx())->GetPrimType());
      varmeexpr->SetFieldID(addrofnode.GetFieldID());
    }
    return varmeexpr;
  }

  if (op == OP_regread) {
    RegreadSSANode &regnode = static_cast<RegreadSSANode &>(mirNode);
    VersionSt *verSt = regnode.GetSSAVar();
    RegMeExpr *regmeexpr = GetOrCreateRegFromVerSt(*verSt);
    regmeexpr->InitBase(mirNode.GetOpCode(), mirNode.GetPrimType(), mirNode.GetNumOpnds());
    return regmeexpr;
  }

  MeExpr *meExpr = meBuilder.BuildMeExpr(mirNode);
  SetMeExprOpnds(*meExpr, mirNode);

  if (op == OP_iread) {
    IvarMeExpr *ivarMeExpr = static_cast<IvarMeExpr*>(meExpr);
    IreadSSANode &ireadSSANode = static_cast<IreadSSANode&>(mirNode);
    ivarMeExpr->SetBase(BuildExpr(*ireadSSANode.Opnd(0)));
    VersionSt *verSt = ireadSSANode.GetSSAVar();
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
  OpMeExpr &opMeExpr = static_cast<OpMeExpr&>(meExpr);
  if (mirNode.IsUnaryNode()) {
    if (mirNode.GetOpCode() != OP_iread) {
      opMeExpr.SetOpnd(0, BuildExpr(*static_cast<UnaryNode&>(mirNode).Opnd(0)));
    }
  } else if (mirNode.IsBinaryNode()) {
    BinaryNode &binaryNode = static_cast<BinaryNode&>(mirNode);
    opMeExpr.SetOpnd(0, BuildExpr(*binaryNode.Opnd(0)));
    opMeExpr.SetOpnd(1, BuildExpr(*binaryNode.Opnd(1)));
  } else if (mirNode.IsTernaryNode()) {
    TernaryNode &ternaryNode = static_cast<TernaryNode&>(mirNode);
    opMeExpr.SetOpnd(0, BuildExpr(*ternaryNode.Opnd(0)));
    opMeExpr.SetOpnd(1, BuildExpr(*ternaryNode.Opnd(1)));
    opMeExpr.SetOpnd(2, BuildExpr(*ternaryNode.Opnd(2)));
  } else if (mirNode.IsNaryNode()) {
    NaryMeExpr &naryMeExpr = static_cast<NaryMeExpr&>(meExpr);
    NaryNode &naryNode = static_cast<NaryNode&>(mirNode);
    for (size_t i = 0; i < naryNode.NumOpnds(); ++i) {
      naryMeExpr.GetOpnds().push_back(BuildExpr(*naryNode.Opnd(i)));
    }
  } else {
    // No need to do anything
  }
}

void IRMap::PutToBucket(uint32 hashidx, MeExpr &meExpr) {
  MeExpr *headexpr = hashTable[hashidx];
  if (headexpr != nullptr) {
    meExpr.SetNext(headexpr);
  }
  hashTable[hashidx] = &meExpr;
}

MeExpr *IRMap::HashMeExpr(MeExpr &meExpr) {
  MeExpr *resultExpr = nullptr;
  uint32 hidx = meExpr.GetHashIndex() % mapHashLength;
  MeExpr *hashedExpr = hashTable[hidx];

  if (hashedExpr != nullptr && meExpr.GetMeOp() != kMeOpGcmalloc) {
    resultExpr = meExpr.GetIdenticalExpr(*hashedExpr);
  }

  if (resultExpr == nullptr) {
    resultExpr = meBuilder.CreateMeExpr(exprID++, meExpr);
    if (resultExpr != nullptr) {
      PutToBucket(hidx, *resultExpr);
    }
  }

  return resultExpr;
}

MeExpr *IRMap::ReplaceMeExprExpr(MeExpr &origExpr, MeExpr &newExpr, size_t opndsSize, MeExpr &meExpr, MeExpr &repExpr) {
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
MeExpr *IRMap::ReplaceMeExprExpr(MeExpr &origExpr, MeExpr &meExpr, MeExpr &repExpr) {
  if (origExpr.IsLeaf()) {
    return &origExpr;
  }

  switch (origExpr.GetMeOp()) {
    case kMeOpOp: {
      OpMeExpr &opMeExpr = static_cast<OpMeExpr&>(origExpr);
      OpMeExpr newMeExpr(opMeExpr, kInvalidExprID);
      return ReplaceMeExprExpr(opMeExpr, newMeExpr, kOperandNumTernary, meExpr, repExpr);
    }
    case kMeOpNary: {
      NaryMeExpr &narymeexpr = static_cast<NaryMeExpr&>(origExpr);
      NaryMeExpr newmeexpr(&irMapAlloc, kInvalidExprID, narymeexpr);
      return ReplaceMeExprExpr(narymeexpr, newmeexpr, narymeexpr.GetOpnds().size(), meExpr, repExpr);
    }
    case kMeOpIvar: {
      IvarMeExpr &ivarexpr = static_cast<IvarMeExpr&>(origExpr);
      IvarMeExpr newmeexpr(kInvalidExprID, ivarexpr);
      bool needRehash = false;
      if (ivarexpr.GetBase() == &meExpr) {
        newmeexpr.SetBase(&repExpr);
        needRehash = true;
      } else if (!ivarexpr.GetBase()->IsLeaf()) {
        newmeexpr.SetBase(ReplaceMeExprExpr(*newmeexpr.GetBase(), meExpr, repExpr));
        if (newmeexpr.GetBase() != ivarexpr.GetBase()) {
          needRehash = true;
        }
      }
      return needRehash ? HashMeExpr(newmeexpr) : &origExpr;
    }
    default:
      ASSERT(false, "NYI");
      return nullptr;
  }
}

bool IRMap::ReplaceMeExprStmtOpnd(uint32 opndID, MeStmt &meStmt, MeExpr &meExpr, MeExpr &repExpr) {
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
bool IRMap::ReplaceMeExprStmt(MeStmt &meStmt, MeExpr &meExpr, MeExpr &repexpr) {
  bool isReplaced = false;
  Opcode op = meStmt.GetOp();

  for (size_t i = 0; i < meStmt.NumMeStmtOpnds(); ++i) {
    if (op == OP_intrinsiccall || op == OP_xintrinsiccall || op == OP_intrinsiccallwithtype ||
        op == OP_intrinsiccallassigned || op == OP_xintrinsiccallassigned ||
        op == OP_intrinsiccallwithtypeassigned) {
      MeExpr *opnd = meStmt.GetOpnd(i);
      if (opnd->IsLeaf() && opnd->GetMeOp() == kMeOpVar) {
        VarMeExpr *varmeexpr = static_cast<VarMeExpr*>(opnd);
        const OriginalSt *orgsym = ssaTab.GetOriginalStFromID(varmeexpr->GetOStIdx());
        if (orgsym->IsSymbolOst() && orgsym->GetMIRSymbol()->GetAttr(ATTR_static)) {
          // its address may be taken
          continue;
        }
      }
    }

    bool curOpndReplaced = ReplaceMeExprStmtOpnd(i, meStmt, meExpr, repexpr);
    isReplaced = isReplaced || curOpndReplaced;

    if (i == 0 && curOpndReplaced && op == OP_iassign) {
      IassignMeStmt &ivarstmt = static_cast<IassignMeStmt&>(meStmt);
      ivarstmt.SetLHSVal(BuildLHSIvar(*ivarstmt.GetOpnd(0), ivarstmt, ivarstmt.GetLHSVal()->GetFieldID()));
    }
  }

  return isReplaced;
}

MeRegPhiNode *IRMap::CreateMeRegPhi(RegMeExpr &mevar) {
  MeRegPhiNode *phimereg = NewInPool<MeRegPhiNode>();
  phimereg->UpdateLHS(mevar);
  return phimereg;
}

MeVarPhiNode *IRMap::CreateMeVarPhi(VarMeExpr &mevar) {
  MeVarPhiNode *phimevar = NewInPool<MeVarPhiNode>();
  phimevar->UpdateLHS(mevar);
  return phimevar;
}

DassignMeStmt *IRMap::CreateDassignMeStmt(MeExpr &lhs, MeExpr &rhs, BB &curbb) {
  DassignMeStmt *meStmt = NewInPool<DassignMeStmt>();
  meStmt->SetRHS(&rhs);
  VarMeExpr &var = static_cast<VarMeExpr&>(lhs);
  meStmt->SetLHS(&var);
  var.SetDefBy(kDefByStmt);
  var.SetDefStmt(meStmt);
  meStmt->SetBB(&curbb);
  return meStmt;
}

RegassignMeStmt *IRMap::CreateRegassignMeStmt(MeExpr &lhs, MeExpr &rhs, BB &curbb) {
  RegassignMeStmt *meStmt = New<RegassignMeStmt>();
  ASSERT(lhs.GetMeOp() == kMeOpReg, "Create regassign without lhs == regread");
  meStmt->SetRHS(&rhs);
  RegMeExpr &reg = static_cast<RegMeExpr&>(lhs);
  meStmt->SetLHS(&reg);
  reg.SetDefBy(kDefByStmt);
  reg.SetDefStmt(meStmt);
  meStmt->SetBB(&curbb);
  return meStmt;
}

// get the false goto bb, if condgoto is brtrue, take the other bb of brture @lable
// otherwise, take the bb of @lable
BB *IRMap::GetFalseBrBB(CondGotoMeStmt &condgoto) {
  LabelIdx lblidx = (LabelIdx)condgoto.GetOffset();
  BB *gotobb = GetBBForLabIdx(lblidx);
  BB *bb = condgoto.GetBB();
  ASSERT(bb->GetSucc().size() == kBBVectorInitialSize, "array size error");
  if (condgoto.GetOp() == OP_brfalse) {
    return gotobb;
  } else {
    return gotobb == bb->GetSucc(0) ? bb->GetSucc(1) : bb->GetSucc(0);
  }
}

MeExpr *IRMap::CreateConstMeExpr(PrimType ptyp, MIRConst &mirConst) {
  ConstMeExpr constMeExpr(kInvalidExprID, &mirConst);
  constMeExpr.SetOp(OP_constval);
  constMeExpr.SetPtyp(ptyp);
  return HashMeExpr(constMeExpr);
}

MeExpr *IRMap::CreateIntConstMeExpr(int64 value, PrimType ptyp) {
  MIRIntConst *intConst =
      mirModule.GetMemPool()->New<MIRIntConst>(value, *GlobalTables::GetTypeTable().GetPrimType(ptyp));
  return CreateConstMeExpr(ptyp, *intConst);
}

MeExpr *IRMap::CreateMeExprBinary(Opcode op, PrimType ptyp, MeExpr &expr0, MeExpr &expr1) {
  OpMeExpr opmeexpr(kInvalidExprID);
  opmeexpr.SetOpnd(0, &expr0);
  opmeexpr.SetOpnd(1, &expr1);
  opmeexpr.InitBase(op, ptyp, kOperandNumBinary);
  return HashMeExpr(opmeexpr);
}

MeExpr *IRMap::CreateMeExprSelect(PrimType ptyp, MeExpr &expr0, MeExpr &expr1, MeExpr &expr2) {
  OpMeExpr opmeexpr(kInvalidExprID);
  opmeexpr.SetOpnd(0, &expr0);
  opmeexpr.SetOpnd(1, &expr1);
  opmeexpr.SetOpnd(2, &expr2);
  opmeexpr.InitBase(OP_select, ptyp, kOperandNumTernary);
  return HashMeExpr(opmeexpr);
}

MeExpr *IRMap::CreateMeExprCompare(Opcode op, PrimType resptyp, PrimType opndptyp, MeExpr &opnd0, MeExpr &opnd1) {
  OpMeExpr opmeexpr(kInvalidExprID);
  opmeexpr.SetOpnd(0, &opnd0);
  opmeexpr.SetOpnd(1, &opnd1);
  opmeexpr.InitBase(op, resptyp, kOperandNumBinary);
  opmeexpr.SetOpndType(opndptyp);
  MeExpr *retmeexpr = HashMeExpr(opmeexpr);
  static_cast<OpMeExpr*>(retmeexpr)->SetOpndType(opndptyp);
  return retmeexpr;
}

MeExpr *IRMap::CreateMeExprTypeCvt(PrimType ptyp, PrimType opndptyp, MeExpr &opnd0) {
  OpMeExpr opmeexpr(kInvalidExprID);
  opmeexpr.SetOpnd(0, &opnd0);
  opmeexpr.InitBase(OP_cvt, ptyp, kOperandNumUnary);
  opmeexpr.SetOpndType(opndptyp);
  return HashMeExpr(opmeexpr);
}

IntrinsiccallMeStmt *IRMap::CreateIntrinsicCallMeStmt(MIRIntrinsicID idx, std::vector<MeExpr*> &opnds, TyIdx tyidx) {
  IntrinsiccallMeStmt *meStmt =
      NewInPool<IntrinsiccallMeStmt>(tyidx == 0 ? OP_intrinsiccall : OP_intrinsiccallwithtype, idx, tyidx);
  for (MeExpr *opnd : opnds) {
    meStmt->GetOpnds().push_back(opnd);
  }
  return meStmt;
}

IntrinsiccallMeStmt *IRMap::CreateIntrinsicCallAssignedMeStmt(MIRIntrinsicID idx, std::vector<MeExpr*> &opnds,
                                                              MeExpr *ret, TyIdx tyidx) {
  IntrinsiccallMeStmt *meStmt = NewInPool<IntrinsiccallMeStmt>(
      tyidx == 0 ? OP_intrinsiccallassigned : OP_intrinsiccallwithtypeassigned, idx, tyidx);
  for (MeExpr *opnd : opnds) {
    meStmt->GetOpnds().push_back(opnd);
  }
  if (ret != nullptr) {
    ASSERT(ret->GetMeOp() == kMeOpReg || ret->GetMeOp() == kMeOpVar, "unexpected opcode");
    MustDefMeNode *mustdef = New<MustDefMeNode>(ret, meStmt);
    meStmt->GetMustDefList()->push_back(*mustdef);
  }
  return meStmt;
}

MeExpr *IRMap::CreateAddrofMeExprFromNewSymbol(MIRSymbol &st, PUIdx puidx) {
  OriginalSt *baseOst = ssaTab.CreateSymbolOriginalSt(st, puidx, 0);
  AddrofMeExpr addrofme(kInvalidExprID, baseOst->GetIndex());
  addrofme.SetOp(OP_addrof);
  addrofme.SetPtyp(PTY_ptr);
  return HashMeExpr(addrofme);
}

void IRMap::InitMeStmtFactory() const {
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
}
}  // namespace maple
