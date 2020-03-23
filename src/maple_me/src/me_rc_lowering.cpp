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
#include "me_rc_lowering.h"
#include <cstring>
#include "me_option.h"

// RCLowering phase generate RC intrinsic for reference assignment
// based on previous analyze results. RC intrinsic will later be lowered
// in Code Generation
namespace {
}

namespace maple {
static inline bool CheckOp(const MeStmt *stmt, Opcode op) {
  return stmt != nullptr && stmt->GetOp() == op;
}

static inline void CheckRemove(const MeStmt *stmt, Opcode op) {
  if (CheckOp(stmt, op)) {
    stmt->GetBB()->RemoveMeStmt(stmt);
  }
}

void RCLowering::Prepare() {
  MIRFunction *mirFunction = func.GetMirFunc();
  ASSERT(mirFunction->GetModule()->CurFunction() == mirFunction, "unexpected CurFunction");
  if (enabledDebug) {
    LogInfo::MapleLogger() << "Handling function " << mirFunction->GetName() << '\n';
  }
  func.SetHints(func.GetHints() | kRcLowered);
  isAnalyzed = (func.GetHints() & kAnalyzeRCed) != 0;
}

void RCLowering::PreRCLower() {
  // preparation steps before going through basicblocks
  MarkLocalRefVar();

  // if analyze rc phase has run, skip this
  if (isAnalyzed) {
    return;
  }

  MarkAllRefOpnds();
  CreateCleanupIntrinsics();
}

void RCLowering::MarkLocalRefVar() {
  MIRFunction *mirFunction = func.GetMirFunc();
  size_t bsize = mirFunction->GetSymTab()->GetSymbolTableSize();
  for (size_t i = 0; i < bsize; ++i) {
    MIRSymbol *sym = mirFunction->GetSymTab()->GetSymbolFromStIdx(i);
    if (sym != nullptr && sym->GetStorageClass() == kScAuto && !sym->IgnoreRC()) {
      sym->SetLocalRefVar();
    }
  }
}

void RCLowering::MarkAllRefOpnds() {
  // to prevent valid_end from being called repeatedly, don't modify the definition of eIt
  for (auto bIt = func.valid_begin(), eIt = func.valid_end(); bIt != eIt; ++bIt) {
    for (auto &stmt : (*bIt)->GetMeStmts()) {
      MeExpr *lhsRef = stmt.GetLHSRef(ssaTab, false);
      if (lhsRef == nullptr) {
        continue;
      }
      if (lhsRef->GetMeOp() == kMeOpVar) {
        auto *var = static_cast<VarMeExpr*>(lhsRef);
        cleanUpVars[var->GetOStIdx()] = var;
        ssaTab.UpdateVarOstMap(var->GetOStIdx(), varOStMap);
      }
      stmt.EnableNeedDecref();
      MeExpr *rhs = stmt.GetRHS();
      if (rhs == nullptr) {
        continue;
      }
      if (rhs->PointsToSomethingThatNeedsIncRef()) {
        stmt.EnableNeedIncref();
      } else {
        stmt.DisableNeedIncref();
      }
    }
  }
}

void RCLowering::CreateCleanupIntrinsics() {
  for (BB *bb : func.GetCommonExitBB()->GetPred()) {
    MeStmt *lastMeStmt = to_ptr(bb->GetMeStmts().rbegin());
    if (!CheckOp(lastMeStmt, OP_return)) {
      continue;
    }
    std::vector<MeExpr*> opnds;
    for (const auto &item : cleanUpVars) {
      if (!varOStMap[item.first]->IsLocal() || varOStMap[item.first]->IsFormal()) {
        continue;
      }
      opnds.push_back(item.second);
    }
    IntrinsiccallMeStmt *intrn = irMap.CreateIntrinsicCallMeStmt(INTRN_MPL_CLEANUP_LOCALREFVARS, opnds);
    bb->InsertMeStmtBefore(to_ptr(bb->GetMeStmts().rbegin()), intrn);
  }
}

// move to MeFunction::CreateVarMeExprFromSym as func has ssaTab and irMap
VarMeExpr *RCLowering::CreateVarMeExprFromSym(MIRSymbol &sym) const {
  OriginalSt *ost = ssaTab.FindOrCreateSymbolOriginalSt(sym, func.GetMirFunc()->GetPuidx(), 0);
  return irMap.GetOrCreateZeroVersionVarMeExpr(*ost);
}

// note that RCInstrinsic creation will check the ref assignment and reuse lhs if possible
IntrinsiccallMeStmt *RCLowering::CreateRCIntrinsic(MIRIntrinsicID intrnID, const MeStmt &stmt,
                                                   std::vector<MeExpr*> &opnds, bool assigned) {
  IntrinsiccallMeStmt *intrn = nullptr;
  if (assigned) {
    MeExpr *ret = (stmt.GetOp() == OP_regassign) ? stmt.GetLHS() : irMap.CreateRegMeExpr(PTY_ref);
    intrn = irMap.CreateIntrinsicCallAssignedMeStmt(intrnID, opnds, ret);
  } else {
    intrn = irMap.CreateIntrinsicCallMeStmt(intrnID, opnds);
  }
  intrn->SetSrcPos(stmt.GetSrcPosition());
  return intrn;
}

MIRIntrinsicID RCLowering::PrepareVolatileCall(const MeStmt &stmt, MIRIntrinsicID intrnId) {
  bool isLoad = (intrnId == INTRN_MCCLoadRefSVol || intrnId == INTRN_MCCLoadWeakVol || intrnId == INTRN_MCCLoadRefVol);
  if (isLoad) {
    CheckRemove(stmt.GetNext(), OP_membaracquire);
  } else {  // volatile store case
    CheckRemove(stmt.GetPrev(), OP_membarrelease);
    CheckRemove(stmt.GetNext(), OP_membarstoreload);
  }
  return intrnId;
}

IntrinsiccallMeStmt *RCLowering::GetVarRHSHandleStmt(const MeStmt &stmt) {
  auto *var = static_cast<VarMeExpr*>(stmt.GetRHS());
  const MIRSymbol *sym = ssaTab.GetMIRSymbolFromID(var->GetOStIdx());
  if (!sym->IsGlobal() || sym->IsFinal()) {
    return nullptr;
  }
  // load global into temp and update rhs to temp
  std::vector<MeExpr*> opnds;
  bool isVolatile = var->IsVolatile(ssaTab);
  MIRIntrinsicID rcCallID = INTRN_UNDEFINED;
  rcCallID = isVolatile ? PrepareVolatileCall(stmt, INTRN_MCCLoadRefSVol) : INTRN_MCCLoadRefS;
  opnds.push_back(irMap.CreateAddrofMeExpr(*var));

  // rhs is not special, skip
  if (rcCallID == INTRN_UNDEFINED) {
    return nullptr;
  }
  // rhs is special, need handle
  return CreateRCIntrinsic(rcCallID, stmt, opnds, true);
}

IntrinsiccallMeStmt *RCLowering::GetIvarRHSHandleStmt(const MeStmt &stmt) {
  auto *ivar = static_cast<IvarMeExpr*>(stmt.GetRHS());
  if (ivar->IsFinal()) {
    return nullptr;
  }
  // load global into temp and update rhs to temp
  std::vector<MeExpr*> opnds;
  MIRIntrinsicID rcCallId = ivar->IsVolatile() ? PrepareVolatileCall(stmt, INTRN_MCCLoadRefVol) : INTRN_MCCLoadRef;
  opnds.push_back(&ivar->GetBase()->GetAddrExprBase());
  opnds.push_back(irMap.CreateAddrofMeExpr(*ivar));
  // rhs is not special, skip
  if (rcCallId == INTRN_UNDEFINED) {
    return nullptr;
  }
  // rhs is special, need handle
  return CreateRCIntrinsic(rcCallId, stmt, opnds, true);
}

// this function returns true if we generated new MRT calls and replaced rhs
void RCLowering::HandleAssignMeStmtRHS(MeStmt &stmt) {
  if (!stmt.NeedIncref()) {
    return;
  }
  MeExpr *rhs = stmt.GetRHS();
  CHECK_FATAL(rhs != nullptr, "rhs is nullptr in RCLowering::HandleAssignMeStmtRHS");
  IntrinsiccallMeStmt *loadCall = nullptr;
  std::vector<MeExpr*> opnds;
  if (rhs->GetMeOp() == kMeOpVar) {
    loadCall = GetVarRHSHandleStmt(stmt);
  } else if (rhs->GetMeOp() == kMeOpIvar) {
    loadCall = GetIvarRHSHandleStmt(stmt);
  }
  // rhs is not special, skip
  if (loadCall == nullptr) {
    return;
  }
  if (stmt.GetOp() == OP_regassign) {
    stmt.GetBB()->ReplaceMeStmt(&stmt, loadCall);
    if (rhs->IsVolatile(ssaTab)) {
      stmt.SetOpnd(1, loadCall->GetMustDefList()->front().GetLHS());
    }
  } else {
    stmt.GetBB()->InsertMeStmtBefore(&stmt, loadCall);
    stmt.SetOpnd(1, loadCall->GetMustDefList()->front().GetLHS());
  }
  stmt.DisableNeedIncref();
}

void RCLowering::HandleCallAssignedMeStmt(MeStmt &stmt, MeExpr *pendingDec) {
  MapleVector<MustDefMeNode> *mustDefs = stmt.GetMustDefList();
  ASSERT_NOT_NULL(mustDefs);

  BB *bb = stmt.GetBB();
  CHECK_FATAL(bb != nullptr, "bb null ptr check");
  if (mustDefs->empty()) {
    // introduce a ret and decref on it as callee has +1 return ref
    IntroduceRegRetIntoCallAssigned(stmt);
    return;
  }

  // decref for old value
  MeExpr *lhs = mustDefs->front().GetLHS();
  if (lhs->GetMeOp() != kMeOpVar) {
    return;
  }
  auto *ost = ssaTab.GetOriginalStFromID(static_cast<VarMeExpr*>(lhs)->GetOStIdx());
  if (!ost->IsSymbolOst()) {
    return;
  }
  const MIRSymbol *retSym = ost->GetMIRSymbol();
  if (retSym == nullptr) {
    return;
  }
  assignedPtrSym.insert(retSym);
  if (!stmt.NeedDecref()) {
    return;
  }

  CHECK_FATAL(pendingDec != nullptr, "pendingDec null ptr check");
  HandleRetOfCallAssignedMeStmt(stmt, *pendingDec);
}

void RCLowering::IntroduceRegRetIntoCallAssigned(MeStmt &stmt) {
  RegMeExpr *curTmp = irMap.CreateRegMeExpr(PTY_ref);
  stmt.GetMustDefList()->push_back(MustDefMeNode(curTmp, &stmt));
  std::vector<MeExpr*> opnds = { curTmp };
  IntrinsiccallMeStmt *decRefCall = CreateRCIntrinsic(INTRN_MCCDecRef, stmt, opnds);
  stmt.GetBB()->InsertMeStmtAfter(&stmt, decRefCall);
}

void RCLowering::HandleRetOfCallAssignedMeStmt(MeStmt &stmt, MeExpr &pendingDec) {
  BB *bb = stmt.GetBB();
  CHECK_FATAL(bb != nullptr, "bb null ptr check");
  RegassignMeStmt *backup = irMap.CreateRegassignMeStmt(*irMap.CreateRegMeExpr(PTY_ref), pendingDec, *bb);
  std::vector<MeExpr*> opnds = { backup->GetRegLHS() };
  IntrinsiccallMeStmt *decRefCall = CreateRCIntrinsic(INTRN_MCCDecRef, stmt, opnds);
  if (!instance_of<CallMeStmt>(stmt)) {
    bb->InsertMeStmtBefore(&stmt, backup);
    bb->InsertMeStmtAfter(&stmt, decRefCall);
  } else {
    // simple optimization for callassign
    // instead of change callassign {dassign} to backup; callassign {dassign}; decref
    // callassign {regassign}; backup; dassign (regread); decref
    RegMeExpr *curTmp = irMap.CreateRegMeExpr(PTY_ref);
    MeStmt *regToVar = irMap.CreateDassignMeStmt(*stmt.GetAssignedLHS(), *curTmp, *bb);
    stmt.GetMustDefList()->front().UpdateLHS(*curTmp);
    bb->InsertMeStmtAfter(&stmt, decRefCall);
    bb->InsertMeStmtAfter(&stmt, regToVar);
    bb->InsertMeStmtAfter(&stmt, backup);
  }
}

bool RCLowering::RCFirst(MeExpr &rhs) {
  // null, local var/reg read
  if (rhs.GetMeOp() == kMeOpConst) {
    return static_cast<ConstMeExpr&>(rhs).IsZero();
  }
  if (rhs.GetMeOp() == kMeOpVar) {
    auto &rhsVar = static_cast<VarMeExpr&>(rhs);
    const MIRSymbol *sym = ssaTab.GetMIRSymbolFromID(rhsVar.GetOStIdx());
    return sym->IsLocal();
  }
  return rhs.GetMeOp() == kMeOpReg;
}

void RCLowering::PreprocessAssignMeStmt(MeStmt &stmt) {
  BB *bb = stmt.GetBB();
  MeExpr *lhs = stmt.GetLHS();
  CHECK_FATAL(lhs != nullptr, "null ptr check");
  MeExpr *rhs = stmt.GetRHS();
  CHECK_FATAL(rhs != nullptr, "null ptr check");
  // Check whether there is any statement exist whose opnd is the throw value in catch BB
  if (needSpecialHandleException && rhs->GetMeOp() == kMeOpReg &&
      static_cast<RegMeExpr*>(rhs)->GetRegIdx() == -kSregThrownval) {
    needSpecialHandleException = false;
  }
  if (lhs->GetMeOp() != kMeOpVar) {
    return;
  }
  const MIRSymbol *lsym = ssaTab.GetMIRSymbolFromID(stmt.GetVarLHS()->GetOStIdx());
  if (stmt.GetOp() == OP_dassign && (lsym->GetStorageClass() == kScAuto || lsym->GetStorageClass() == kScFormal)) {
    assignedPtrSym.insert(lsym);
  }
  if (rhs->GetMeOp() != kMeOpGcmalloc) {
    return;
  }
  gcMallocObjects.insert(lhs);
  if (lsym->GetAttr(ATTR_rcunowned)) {
    // if new obj is assigned to unowned refvar, we need a localrefvar
    // to decref at exit
    // introduce new localrefvar = lhs after current stmt
    MeStmt *backup = irMap.CreateDassignMeStmt(*CreateNewTmpVarMeExpr(true), *lhs, *bb);
    // backup will not have any incref/decref
    bb->InsertMeStmtAfter(&stmt, backup);
  }
}

void RCLowering::HandleAssignMeStmtRegLHS(MeStmt &stmt) {
  if (!stmt.NeedIncref()) {
    return;
  }
  std::vector<MeExpr*> opnds = { stmt.GetLHS() };
  IntrinsiccallMeStmt *incCall = CreateRCIntrinsic(INTRN_MCCIncRef, stmt, opnds);
  stmt.GetBB()->InsertMeStmtAfter(&stmt, incCall);
}

void RCLowering::HandleAssignMeStmtVarLHS(MeStmt &stmt, MeExpr *pendingDec) {
  const MIRSymbol *lsym = ssaTab.GetMIRSymbolFromID(stmt.GetVarLHS()->GetOStIdx());
  if (lsym->IsGlobal()) {
    // decref could be optimized away after if null check
    HandleAssignToGlobalVar(stmt);
  } else {
    // local assign, backup old value and insert Inc and Dec after
    HandleAssignToLocalVar(stmt, pendingDec);
  }
  assignedPtrSym.insert(lsym);
}

MIRType *RCLowering::GetArrayNodeType(const VarMeExpr &var) {
  const MIRSymbol *arrayElemSym = ssaTab.GetMIRSymbolFromID(var.GetOStIdx());
  MIRType *baseType = arrayElemSym->GetType();
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

void RCLowering::CheckArrayStore(const IntrinsiccallMeStmt &writeRefCall) {
  if (!Options::checkArrayStore) {
    return;
  }
  MIRIntrinsicID intrnID = writeRefCall.GetIntrinsic();
  if (!((INTRN_MCCWriteVolNoInc <= intrnID) && (intrnID <= INTRN_MCCWrite))) {
    return;
  }
  if (writeRefCall.GetOpnd(1)->GetOp() != OP_iaddrof) {
    return;
  }
  OpMeExpr *opExpr = static_cast<OpMeExpr*>(writeRefCall.GetOpnd(1));
  if (opExpr->GetOpnd(0)->GetOp() != OP_array) {
    return;
  }
  MeExpr *arrayNode = writeRefCall.GetOpnd(0);
  if (arrayNode->GetMeOp() != kMeOpVar) {
    return;
  }
  VarMeExpr *arrayVar = static_cast<VarMeExpr*>(arrayNode);
  MeExpr *valueNode = writeRefCall.GetOpnd(2);
  if (valueNode->GetMeOp() != kMeOpVar) {
    return;
  }
  MIRType *arrayElemType = GetArrayNodeType(*arrayVar);
  MIRType *valueRealType = GetArrayNodeType(*(static_cast<VarMeExpr*>(valueNode)));
  if ((arrayElemType != nullptr) && (arrayElemType->GetKind() == kTypeClass) &&
      static_cast<MIRClassType*>(arrayElemType)->IsFinal() &&
      (valueRealType != nullptr) && (valueRealType->GetKind() == kTypeClass) &&
      static_cast<MIRClassType*>(valueRealType)->IsFinal() &&
      (valueRealType->GetTypeIndex() == arrayElemType->GetTypeIndex())) {
    return;
  }
  std::vector<MeExpr*> opnds = { arrayNode, valueNode };
  IntrinsiccallMeStmt *checkStmt = irMap.CreateIntrinsicCallMeStmt(INTRN_MCCCheckArrayStore, opnds);
  writeRefCall.GetBB()->InsertMeStmtBefore(&writeRefCall, checkStmt);
}

void RCLowering::HandleAssignToGlobalVar(const MeStmt &stmt) {
  MeExpr *lhs = stmt.GetLHS();
  CHECK_FATAL(lhs != nullptr, "null ptr check");
  MeExpr *rhs = stmt.GetRHS();
  CHECK_FATAL(rhs != nullptr, "null ptr check");
  BB *bb = stmt.GetBB();
  CHECK_FATAL(bb != nullptr, "bb null ptr check");

  std::vector<MeExpr*> opnds = { irMap.CreateAddrofMeExpr(*lhs), rhs };
  IntrinsiccallMeStmt *writeRefCall = CreateRCIntrinsic(SelectWriteBarrier(stmt), stmt, opnds);
  bb->ReplaceMeStmt(&stmt, writeRefCall);
  CheckArrayStore(*writeRefCall);
}

void RCLowering::HandleAssignToLocalVar(MeStmt &stmt, MeExpr *pendingDec) {
  MeExpr *lhs = stmt.GetLHS();
  CHECK_FATAL(lhs != nullptr, "null ptr check");
  MeExpr *rhs = stmt.GetRHS();
  CHECK_FATAL(rhs != nullptr, "null ptr check");
  BB *bb = stmt.GetBB();
  CHECK_FATAL(bb != nullptr, "bb null ptr check");
  bool incWithLHS = stmt.NeedIncref();

  if (stmt.NeedDecref()) {
    if (RCFirst(*rhs)) {
      // temp is not needed
      if (incWithLHS) {
        std::vector<MeExpr*> opnds = { rhs };
        IntrinsiccallMeStmt *incCall = CreateRCIntrinsic(INTRN_MCCIncRef, stmt, opnds);
        bb->InsertMeStmtBefore(&stmt, incCall);
        incWithLHS = false;
      }
      std::vector<MeExpr*> opnds = { pendingDec };
      IntrinsiccallMeStmt *decCall = CreateRCIntrinsic(INTRN_MCCDecRef, stmt, opnds);
      bb->InsertMeStmtBefore(&stmt, decCall);
    } else {
      RegassignMeStmt *backup = irMap.CreateRegassignMeStmt(*irMap.CreateRegMeExpr(PTY_ref), *pendingDec, *bb);
      bb->InsertMeStmtBefore(&stmt, backup);
      std::vector<MeExpr*> opnds = { backup->GetLHS() };
      IntrinsiccallMeStmt *decCall = CreateRCIntrinsic(INTRN_MCCDecRef, stmt, opnds);
      bb->InsertMeStmtAfter(&stmt, decCall);
    }
  }
  if (incWithLHS) {
    std::vector<MeExpr*> opnds = { lhs };
    IntrinsiccallMeStmt *incCall = CreateRCIntrinsic(INTRN_MCCIncRef, stmt, opnds);
    bb->InsertMeStmtAfter(&stmt, incCall);
  }
}

bool RCLowering::IsInitialized(IvarMeExpr &ivar) {
  MeExpr *base = ivar.GetBase();
  if (base->GetMeOp() != kMeOpVar) {
    return true;
  }
  if (gcMallocObjects.find(base) == gcMallocObjects.end()) {
    return true;
  }
  FieldID fieldID = ivar.GetFieldID();
  MapleSet<FieldID> *fieldSet = initializedFields[base];
  if (fieldSet != nullptr && (fieldSet->count(fieldID) > 0 || fieldSet->count(0) > 0)) {
    return true;
  }
  MIRType *baseType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(ivar.GetTyIdx());
  auto *ptype = utils::ToRef(safe_cast<MIRPtrType>(baseType)).GetPointedType();
  auto *classType = safe_cast<MIRClassType>(ptype);
  return classType == nullptr || !classType->IsOwnField(fieldID);
}

void RCLowering::HandleAssignMeStmtIvarLHS(MeStmt &stmt) {
  auto &iassign = static_cast<IassignMeStmt&>(stmt);
  IvarMeExpr *lhsInner = iassign.GetLHSVal();
  FieldID fieldID = lhsInner->GetFieldID();
  MIRType *baseType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(lhsInner->GetTyIdx());
  auto *ptype = utils::ToRef(safe_cast<MIRPtrType>(baseType)).GetPointedType();
  auto *classType = safe_cast<MIRClassType>(ptype);
  // skip RC operation if the field is unowned
  if (classType != nullptr && classType->IsFieldRCUnownedRef(fieldID)) {
    return;
  }
  if (!IsInitialized(*lhsInner)) {
    stmt.DisableNeedDecref();
    MapleSet<FieldID> *fieldSet = initializedFields[lhsInner->GetBase()];
    if (fieldSet == nullptr) {
      fieldSet =
          mirModule.GetMemPool()->New<MapleSet<FieldID>>(mirModule.GetMPAllocator().Adapter());
    }
    fieldSet->insert(fieldID);
  }
  MeExpr *rhsInner = stmt.GetRHS();
  MIRIntrinsicID intrinsicID = SelectWriteBarrier(stmt);
  std::vector<MeExpr*> opnds =
      { &lhsInner->GetBase()->GetAddrExprBase(), irMap.CreateAddrofMeExpr(*lhsInner), rhsInner };
  IntrinsiccallMeStmt *writeRefCall = CreateRCIntrinsic(intrinsicID, stmt, opnds);
  stmt.GetBB()->ReplaceMeStmt(&stmt, writeRefCall);
  CheckArrayStore(*writeRefCall);
}

void RCLowering::HandleAssignMeStmt(MeStmt &stmt, MeExpr *pendingDec) {
  PreprocessAssignMeStmt(stmt);
  if (!stmt.NeedIncref() && !stmt.NeedDecref()) {
    return;
  }
  HandleAssignMeStmtRHS(stmt);
  MeExpr *lhs = stmt.GetLHS();
  CHECK_FATAL(lhs != nullptr, "null ptr check");
  if (lhs->GetMeOp() == kMeOpReg) {
    HandleAssignMeStmtRegLHS(stmt);
  } else if (lhs->GetMeOp() == kMeOpVar) {
    HandleAssignMeStmtVarLHS(stmt, pendingDec);
  } else if (lhs->GetMeOp() == kMeOpIvar) {
    HandleAssignMeStmtIvarLHS(stmt);
  }
}


// align with order in rcinsertion, otherwise missing weak volatile
// note that we are generating INTRN_MCCWriteNoRC so write_barrier is supported,
// otherwise iassign would be enough.
MIRIntrinsicID RCLowering::SelectWriteBarrier(const MeStmt &stmt) {
  bool incWithLHS = stmt.NeedIncref();
  bool decWithLHS = stmt.NeedDecref();
  MeExpr *lhs = stmt.GetLHS();
  CHECK_FATAL(lhs != nullptr, "null ptr check");
  MeExprOp meOp = lhs->GetMeOp();
  CHECK_FATAL((meOp == kMeOpVar || meOp == kMeOpIvar), "Not Expected meOp");
  if (lhs->IsVolatile(ssaTab)) {
    if (meOp == kMeOpVar) {
      return PrepareVolatileCall(stmt, incWithLHS ? (decWithLHS ? INTRN_MCCWriteSVol : INTRN_MCCWriteSVolNoDec)
                                                  : (decWithLHS ? INTRN_MCCWriteSVolNoInc : INTRN_MCCWriteSVolNoRC));
    }
    if (static_cast<IvarMeExpr*>(lhs)->IsRCWeak()) {
      return PrepareVolatileCall(stmt, INTRN_MCCWriteVolWeak);
    }
    return PrepareVolatileCall(stmt, incWithLHS ? INTRN_MCCWriteVol : INTRN_MCCWriteVolNoInc);
  }
  if (meOp == kMeOpVar) {
    return incWithLHS ? (decWithLHS ? INTRN_MCCWriteS : INTRN_MCCWriteSNoDec)
                      : (decWithLHS ? INTRN_MCCWriteSNoInc : INTRN_MCCWriteSNoRC);
  }
  if (static_cast<IvarMeExpr*>(lhs)->IsRCWeak()) {
    return INTRN_MCCWriteWeak;
  }
  return incWithLHS ? (decWithLHS ? INTRN_MCCWrite : INTRN_MCCWriteNoDec)
                    : (decWithLHS ? INTRN_MCCWriteNoInc : INTRN_MCCWriteNoRC);
}

void RCLowering::RCLower() {
  // to prevent valid_end from being called repeatedly, don't modify the definition of eIt
  for (auto bIt = func.valid_begin(), eIt = func.valid_end(); bIt != eIt; ++bIt) {
    if (bIt == func.common_entry() || bIt == func.common_exit()) {
      continue;
    }
    BBLower(**bIt);
  }
}

MeExpr *RCLowering::HandleIncRefAndDecRefStmt(const MeStmt &stmt) {
  Opcode opCode = stmt.GetOp();
  if (opCode == OP_decref) {
    stmt.GetBB()->RemoveMeStmt(&stmt);
    return stmt.GetOpnd(0);
  }

  if (opCode != OP_incref && opCode != OP_decrefreset) {
    return nullptr;
  }

  MIRIntrinsicID rcCallID = opCode == OP_incref ? INTRN_MCCIncRef : INTRN_MCCDecRefReset;
  std::vector<MeExpr*> opnds = { stmt.GetOpnd(0) };
  IntrinsiccallMeStmt *rcCall = CreateRCIntrinsic(rcCallID, stmt, opnds);
  stmt.GetBB()->ReplaceMeStmt(&stmt, rcCall);
  return nullptr;
}

// if a var me expr is initialized by constructor, record it's initialized map
// if a field id is not in initialized map, means the field has not been assigned a value
// dec ref is not necessary in it's first assignment.
void RCLowering::InitializedObjectFields(MeStmt &stmt) {
  if (stmt.GetOp() != OP_callassigned) {
    return;
  }
  auto &call = static_cast<CallMeStmt&>(stmt);
  MIRFunction &callee = call.GetTargetFunction();
  if (call.NumMeStmtOpnds() == 0 || call.GetOpnd(0)->GetMeOp() != kMeOpVar) {
    return;
  }
  MeExpr *firstOpnd = call.GetOpnd(0);
  bool isNew = gcMallocObjects.find(firstOpnd) != gcMallocObjects.end();
  bool hasNotInitialized = initializedFields.find(firstOpnd) == initializedFields.end();
  bool inInitializedMap =
      mirModule.GetPuIdxFieldInitializedMap().find(call.GetPUIdx()) != mirModule.GetPuIdxFieldInitializedMap().end();
  if (callee.IsConstructor() && isNew && hasNotInitialized && inInitializedMap) {
    initializedFields[firstOpnd] = mirModule.GetPUIdxFieldInitializedMapItem(call.GetPUIdx());
  } else {
    for (auto iter : call.GetOpnds()) {
      gcMallocObjects.erase(iter);
    }
  }
}

void RCLowering::BBLower(BB &bb) {
  MeExpr *pendingDec = nullptr;
  gcMallocObjects.clear();
  initializedFields.clear();
  needSpecialHandleException = bb.GetAttributes(kBBAttrIsCatch);
  for (auto &stmt : bb.GetMeStmts()) {
    if ((func.GetHints() & kAnalyzeRCed) == 0) {
      pendingDec = stmt.GetLHSRef(ssaTab, false);
    }
    Opcode opcode = stmt.GetOp();
    if (opcode == OP_return) {
      rets.push_back(&stmt);
    } else if (kOpcodeInfo.IsCallAssigned(opcode)) {
      MIRType *retType = stmt.GetReturnType();
      if (retType != nullptr && retType->GetPrimType() == PTY_ref) {
        HandleCallAssignedMeStmt(stmt, pendingDec);
      }
      InitializedObjectFields(stmt);
    } else if (stmt.IsAssign()) {
      HandleAssignMeStmt(stmt, pendingDec);
    } else {
      MeExpr *ret = HandleIncRefAndDecRefStmt(stmt);
      pendingDec = ret == nullptr ? pendingDec : ret;
    }
  }
  // there is no any statement exist whose opnd is the throw value, handle it
  if (needSpecialHandleException) {
    // create throw value
    OriginalSt *ost =
        ssaTab.GetOriginalStTable().FindOrCreatePregOriginalSt(-kSregThrownval, func.GetMirFunc()->GetPuidx());
    ost->SetTyIdx(GlobalTables::GetTypeTable().GetPrimType(PTY_ref)->GetTypeIndex());
    RegMeExpr *regReadExpr = irMap.CreateRegMeExprVersion(*ost);
    regReadExpr->SetPtyp(PTY_ref);
    MeStmt *firstMeStmt = to_ptr(bb.GetMeStmts().begin());
    std::vector<MeExpr*> opnds = { regReadExpr };
    IntrinsiccallMeStmt *decRefcall = CreateRCIntrinsic(INTRN_MCCDecRef, *firstMeStmt, opnds);
    bb.InsertMeStmtAfter(firstMeStmt, decRefcall);
  }
}

IntrinsiccallMeStmt *FindCleanupIntrinsic(const MeStmt &ret) {
  auto &meStmts = ret.GetBB()->GetMeStmts();
  for (auto iter = meStmts.rbegin(); iter != meStmts.rend(); ++iter) {
    if (CheckOp(to_ptr(iter), OP_intrinsiccall)) {
      auto *intrinsicCall = static_cast<IntrinsiccallMeStmt*>(to_ptr(iter));
      if (intrinsicCall->GetIntrinsic() == INTRN_MPL_CLEANUP_LOCALREFVARS) {
        return intrinsicCall;
      }
    }
  }
  return nullptr;
}

void RCLowering::HandleReturnVar(RetMeStmt &ret) {
  auto *retVar = static_cast<VarMeExpr*>(ret.GetOpnd(0));
  CHECK_FATAL(retVar != nullptr, "retVal null ptr check");
  MIRSymbol *sym = ssaTab.GetMIRSymbolFromID(retVar->GetOStIdx());
  if (sym != nullptr && sym->IgnoreRC()) {
    return;
  }
  // if dread is local assigned var, no need inc because we will skip its dec
  // unless we are change it to regread in later optimization
  if (assignedPtrSym.count(sym) > 0 && sym->GetStorageClass() == kScAuto && !MeOption::regreadAtReturn) {
    return;
  }
  if (sym != nullptr && sym->IsGlobal() && !sym->IsFinal()) {
    HandleReturnGlobal(ret);
  } else if (assignedPtrSym.count(sym) > 0 && sym->GetStorageClass() == kScAuto && sym->GetAttr(ATTR_localrefvar)) {
    // must be regreadAtReturn
    // checking localrefvar because some objects are meta
    HandleReturnRegread(ret);
  } else {
    HandleReturnFormal(ret);
  }
}

void RCLowering::HandleReturnGlobal(RetMeStmt &ret) {
  BB *bb = ret.GetBB();
  CHECK_FATAL(bb != nullptr, "bb null ptr check");
  auto *retVar = static_cast<VarMeExpr*>(ret.GetOpnd(0));
  CHECK_FATAL(retVar != nullptr, "retVal null ptr check");
  std::vector<MeExpr*> opnds = { irMap.CreateAddrofMeExpr(*retVar) };
  IntrinsiccallMeStmt *loadCall = CreateRCIntrinsic(INTRN_MCCLoadRefS, ret, opnds, true);
  bb->InsertMeStmtBefore(&ret, loadCall);
  ret.SetOpnd(0, loadCall->GetMustDefList()->front().GetLHS());

}

void RCLowering::HandleReturnRegread(RetMeStmt &ret) {
  BB *bb = ret.GetBB();
  CHECK_FATAL(bb != nullptr, "bb null ptr check");
  auto *retVar = static_cast<VarMeExpr*>(ret.GetOpnd(0));
  CHECK_FATAL(retVar != nullptr, "retVal null ptr check");
  IntrinsiccallMeStmt *cleanup = FindCleanupIntrinsic(ret);
  if (cleanup == nullptr) {
    std::vector<MeExpr*> opnds = { retVar };
    IntrinsiccallMeStmt *incCall = CreateRCIntrinsic(INTRN_MCCIncRef, ret, opnds);
    bb->InsertMeStmtBefore(&ret, incCall);
  } else {
    // remove argument from intrinsiccall MPL_CLEANUP_LOCALREFVARS (dread ref %Reg1_R5678, ...
    const MapleVector<MeExpr*> *opnds = &cleanup->GetOpnds();
    for (auto iter = opnds->begin(); iter != opnds->end(); ++iter) {
      if (*iter == retVar || (!isAnalyzed && static_cast<VarMeExpr*>(*iter)->GetOStIdx() == retVar->GetOStIdx())) {
        cleanup->EraseOpnds(iter);
        cleanup->PushBackOpnd(retVar);  // pin it to end of std::vector
        cleanup->SetIntrinsic(INTRN_MPL_CLEANUP_LOCALREFVARS_SKIP);
        MIRSymbol *sym = ssaTab.GetMIRSymbolFromID(retVar->GetOStIdx());
        if (sym->GetAttr(ATTR_localrefvar)) {
          func.GetMirFunc()->InsertMIRSymbol(sym);
        }
        break;
      }
    }
  }
}

void RCLowering::HandleReturnFormal(RetMeStmt &ret) {
  BB *bb = ret.GetBB();
  CHECK_FATAL(bb != nullptr, "bb null ptr check");
  auto *retVar = static_cast<VarMeExpr*>(ret.GetOpnd(0));
  CHECK_FATAL(retVar != nullptr, "retVal null ptr check");
  std::vector<MeExpr*> opnds = { retVar };
  IntrinsiccallMeStmt *incRefStmt = CreateRCIntrinsic(INTRN_MCCIncRef, ret, opnds, true);
  ret.SetOpnd(0, incRefStmt->GetMustDefList()->front().GetLHS());
  IntrinsiccallMeStmt *cleanup = FindCleanupIntrinsic(ret);
  if (cleanup == nullptr) {
    bb->InsertMeStmtBefore(&ret, incRefStmt);
  } else {
    bb->InsertMeStmtAfter(cleanup, incRefStmt);
  }
}

void RCLowering::HandleReturnIvar(RetMeStmt &ret) {
  // insert calls
  auto *retIvar = static_cast<IvarMeExpr*>(ret.GetOpnd(0));
  if (retIvar->IsVolatile()) {
    std::vector<MeExpr*> opnds = { &retIvar->GetBase()->GetAddrExprBase(), irMap.CreateAddrofMeExpr(*retIvar) };
    IntrinsiccallMeStmt *loadCall = CreateRCIntrinsic(PrepareVolatileCall(ret, INTRN_MCCLoadRefVol), ret, opnds, true);
    ret.GetBB()->InsertMeStmtBefore(&ret, loadCall);
    ret.SetOpnd(0, loadCall->GetMustDefList()->front().GetLHS());
  } else {
    std::vector<MeExpr*> opnds = { &retIvar->GetBase()->GetAddrExprBase(), irMap.CreateAddrofMeExpr(*retIvar) };
    MeStmt *loadCall = CreateRCIntrinsic(INTRN_MCCLoadRef, ret, opnds, true);
    ret.GetBB()->InsertMeStmtBefore(&ret, loadCall);
    ret.SetOpnd(0, loadCall->GetMustDefList()->front().GetLHS());

  }
}

void RCLowering::HandleReturnReg(RetMeStmt &ret) {
  // if the register with ref value is defined by callassigned or gcmalloc
  // return incref should have been delegated and not needed.
  auto *regRet = static_cast<RegMeExpr*>(ret.GetOpnd(0));
  if (regRet->GetDefBy() == kDefByMustDef) {
    return;
  }
  if (regRet->GetDefBy() == kDefByStmt && regRet->GetDefStmt()->GetOp() == OP_regassign) {
    MeExpr *rhs = regRet->GetDefStmt()->GetRHS();
    ASSERT_NOT_NULL(rhs);
    if (rhs->GetOp() == OP_gcmalloc || rhs->GetOp() == OP_gcmallocjarray) {
      return;
    }
  }
  std::vector<MeExpr*> opnds = { regRet };
  IntrinsiccallMeStmt *incCall = CreateRCIntrinsic(INTRN_MCCIncRef, ret, opnds, true);
  ret.SetOpnd(0, incCall->GetMustDefList()->front().GetLHS());
  ret.GetBB()->InsertMeStmtBefore(&ret, incCall);
}

void RCLowering::HandleReturnWithCleanup() {
  for (auto *stmt : rets) {
    auto *ret = static_cast<RetMeStmt*>(stmt);
    IntrinsiccallMeStmt *cleanup = FindCleanupIntrinsic(*ret);
    if (cleanup != nullptr && !tmpLocalRefVars.empty()) {  // new localrefvar introduced in this phase
      for (auto tmpVar : tmpLocalRefVars) {
        cleanup->PushBackOpnd(tmpVar);
      }
    }
    if (ret->GetOpnds().empty()) {
      continue;
    }
    MeExpr *retVal = ret->GetOpnd(0);
    // handle nullptr return first
    if (retVal->GetMeOp() == kMeOpConst && static_cast<ConstMeExpr*>(retVal)->IsZero()) {
      continue;
    }
    if (retVal->GetMeOp() == kMeOpVar) {
      HandleReturnVar(*ret);
    } else if (retVal->GetMeOp() == kMeOpIvar) {
      HandleReturnIvar(*ret);
    } else if (retVal->GetMeOp() == kMeOpReg) {
      HandleReturnReg(*ret);
    } else {
      // incref by default
      RegMeExpr *tmpReg = irMap.CreateRegMeExpr(PTY_ref);
      RegassignMeStmt *temp = irMap.CreateRegassignMeStmt(*tmpReg, *retVal, *stmt->GetBB());
      stmt->GetBB()->InsertMeStmtBefore(stmt, temp);
      std::vector<MeExpr*> opnds = { tmpReg };
      IntrinsiccallMeStmt *incCall = CreateRCIntrinsic(INTRN_MCCIncRef, *stmt, opnds);
      stmt->GetBB()->InsertMeStmtBefore(stmt, incCall);
      ret->SetOpnd(0, tmpReg);
    }
  }
}

void RCLowering::HandleReturnNeedBackup() {
  // any return value expression containing ivar has to be saved in a
  // temp with the temp being returned
  for (auto *stmt : rets) {
    auto *ret = static_cast<RetMeStmt*>(stmt);
    if (ret->NumMeStmtOpnds() == 0) {
      continue;
    }
    MeExpr *retVal = ret->GetOpnd(0);
    if (!retVal->HasIvar()) {
      continue;
    }
    RegMeExpr *curTmp = irMap.CreateRegMeExpr(retVal->GetPrimType());
    MeStmt *regAssign = irMap.CreateRegassignMeStmt(*curTmp, *retVal, *ret->GetBB());
    ret->GetBB()->InsertMeStmtBefore(ret, regAssign);
    ret->SetOpnd(0, curTmp);
  }
}

void RCLowering::HandleReturnStmt() {
  bool returnRef = (func.GetMirFunc()->GetReturnType()->GetPrimType() == PTY_ref);
  bool updateCleanup = !tmpLocalRefVars.empty();
  if (returnRef || updateCleanup) {
    HandleReturnWithCleanup();
  } else {
    HandleReturnNeedBackup();
  }
}

void RCLowering::HandleArguments() {
  // handle arguments, if the formal gets modified
  // insert incref at entry and decref before all returns
  MIRFunction *mirFunc = func.GetMirFunc();
  BB *firstBB = func.GetFirstBB();
  MeStmt *firstMeStmt = to_ptr(firstBB->GetMeStmts().begin());
  for (size_t i = (mirFunc->IsStatic() ? 0 : 1); i < mirFunc->GetFormalCount(); ++i) {
    MIRSymbol *sym = mirFunc->GetFormal(i);
    if (sym == nullptr || sym->IgnoreRC() || (!sym->IsGlobal() && assignedPtrSym.count(sym) == 0)) {
      continue;
    }
    VarMeExpr *argVar = CreateVarMeExprFromSym(*sym);
    CHECK_FATAL(argVar != nullptr, "null ptr check");
    IntrinsiccallMeStmt *incCall = nullptr;
    if (firstMeStmt != nullptr) {
      std::vector<MeExpr*> opnds = { argVar };
      incCall = CreateRCIntrinsic(INTRN_MCCIncRef, *firstMeStmt, opnds);
      firstBB->InsertMeStmtBefore(firstMeStmt, incCall);
    }
    TypeAttrs typeAttr = mirFunc->GetNthParamAttr(i);
    typeAttr.SetAttr(ATTR_localrefvar);
    mirFunc->SetNthParamAttr(i, typeAttr);
    sym->SetLocalRefVar();

    for (auto *stmt : rets) {
      std::vector<MeExpr*> opnds = { argVar };
      IntrinsiccallMeStmt *decRefCall = CreateRCIntrinsic(INTRN_MCCDecRef, *stmt, opnds);
      stmt->GetBB()->InsertMeStmtBefore(stmt, decRefCall);
    }
  }
}

void RCLowering::PostRCLower() {
  HandleArguments();
  // handle ret stmts
  HandleReturnStmt();
  // compact RC
  // to prevent valid_end from being called repeatedly, don't modify the definition of eIt
  for (auto bIt = func.valid_begin(), eIt = func.valid_end(); bIt != eIt; ++bIt) {
    CompactRC(**bIt);
  }
}

void RCLowering::Finish() {
  if (enabledDebug) {
    LogInfo::MapleLogger() << "\n============== After RC LOWERING =============" << '\n';
    func.Dump(false);
  }
}


static inline bool CheckIntrinsicID(MeStmt *stmt, MIRIntrinsicID id) {
  if (!CheckOp(stmt, OP_intrinsiccall)) {
    return false;
  }
  auto *intrinsicCall = static_cast<IntrinsiccallMeStmt*>(stmt);
  return intrinsicCall->GetIntrinsic() == id;
}

static inline MeStmt *GetPrevMeStmt(const MeStmt &stmt) {
  MeStmt *prevStmt = stmt.GetPrev();
  while (CheckOp(prevStmt, OP_comment)) {
    prevStmt = prevStmt->GetPrev();
  }
  return prevStmt;
}

// iterate over stmts and find back-to-back incref/decref
// and merge them into one intrinsic if needed
void RCLowering::CompactRC(BB &bb) {
  for (auto &stmt : bb.GetMeStmts()) {
    if (CheckIntrinsicID(&stmt, INTRN_MCCIncRef)) {
      MeStmt *prevStmt = GetPrevMeStmt(stmt);
      if (CheckIntrinsicID(prevStmt, INTRN_MCCDecRef)) {
        CompactIncAndDec(stmt, *prevStmt);
      }
    }
    if (CheckIntrinsicID(&stmt, INTRN_MCCDecRef)) {
      MeStmt *prevStmt = GetPrevMeStmt(stmt);
      if (CheckIntrinsicID(prevStmt, INTRN_MCCIncRef)) {
        CompactIncAndDec(*prevStmt, stmt);
      }
    }
  }
}

void RCLowering::CompactIncAndDec(const MeStmt &incStmt, const MeStmt &decStmt) {
  BB *bb = incStmt.GetBB();
  CHECK_FATAL(bb != nullptr, "bb nullptr check");
  MeExpr *incOpnd = incStmt.GetOpnd(0);
  MeExpr *decOpnd = decStmt.GetOpnd(0);
  if (incOpnd == decOpnd) {
    bb->RemoveMeStmt(&incStmt);
  } else {
    std::vector<MeExpr*> opnds = { incOpnd, decOpnd };
    IntrinsiccallMeStmt *incDecCall = CreateRCIntrinsic(INTRN_MCCIncDecRef, incStmt, opnds);
    bb->ReplaceMeStmt(&incStmt, incDecCall);
  }
  bb->RemoveMeStmt(&decStmt);
}

void RCLowering::CompactIncAndDecReset(const MeStmt &incStmt, const MeStmt &resetStmt) {
  BB *bb = incStmt.GetBB();
  CHECK_FATAL(bb != nullptr, "bb nullptr check");
  MeExpr *incOpnd = incStmt.GetOpnd(0);
  MeExpr *resetOpnd = resetStmt.GetOpnd(0);
  if (incOpnd == resetOpnd) {
    bb->RemoveMeStmt(&resetStmt);
  } else {
    std::vector<MeExpr*> opnds = { incOpnd, resetOpnd };
    IntrinsiccallMeStmt *incDecReset = CreateRCIntrinsic(INTRN_MCCIncDecRefReset, resetStmt, opnds);
    bb->ReplaceMeStmt(&resetStmt, incDecReset);
  }
  bb->RemoveMeStmt(&incStmt);
}

void RCLowering::ReplaceDecResetWithDec(MeStmt &prevStmt, const MeStmt &stmt) {
  auto *addrofMeExpr = static_cast<AddrofMeExpr*>(stmt.GetOpnd(0));
  ASSERT_NOT_NULL(addrofMeExpr);
  auto *dass = static_cast<DassignMeStmt*>(&prevStmt);
  if (dass->GetRHS()->GetMeOp() != kMeOpReg || dass->GetVarLHS()->GetOStIdx() != addrofMeExpr->GetOstIdx()) {
    return;
  }
  BB *bb = stmt.GetBB();
  CHECK_FATAL(bb != nullptr, "bb nullptr check");
  std::vector<MeExpr*> opnds = { dass->GetRHS() };
  IntrinsiccallMeStmt *decRefCall = CreateRCIntrinsic(INTRN_MCCDecRef, stmt, opnds);
  bb->ReplaceMeStmt(&stmt, decRefCall);
  bb->RemoveMeStmt(&prevStmt);
}

void RCLowering::CompactAdjacentDecReset(const MeStmt &prevStmt, const MeStmt &stmt) {
  MeExpr *prevOpnd = prevStmt.GetOpnd(0);
  MeExpr *curOpnd = stmt.GetOpnd(0);
  BB *bb = stmt.GetBB();
  if (prevOpnd != curOpnd) {
    std::vector<MeExpr*> opnds = { prevOpnd, curOpnd };
    IntrinsiccallMeStmt *decRefCall = CreateRCIntrinsic(INTRN_MCCDecRefResetPair, stmt, opnds);
    bb->ReplaceMeStmt(&stmt, decRefCall);
  }
  bb->RemoveMeStmt(&prevStmt);
}

OriginalSt *RCLowering::RetrieveOSt(const std::string &name, bool isLocalRefVar) const {
  MIRSymbol *backupSym = mirModule.GetMIRBuilder()->GetOrCreateLocalDecl(
      name, *GlobalTables::GetTypeTable().GetTypeFromTyIdx(TyIdx(PTY_ptr)));  // use PTY_ptr for temp
  backupSym->SetIsTmp(true);
  if (isLocalRefVar) {
    backupSym->SetLocalRefVar();
  }
  OriginalSt *ost = ssaTab.FindOrCreateSymbolOriginalSt(*backupSym, func.GetMirFunc()->GetPuidx(), 0);
  return ost;
}

// function for creating short-lived temp
VarMeExpr *RCLowering::CreateNewTmpVarMeExpr(bool isLocalRefVar) {
  std::string name = std::string("__RCTemp__").append(std::to_string(++tmpCount));
  OriginalSt *ost = RetrieveOSt(name, isLocalRefVar);
  if (ost->GetZeroVersionIndex() == 0) {
    ost->SetZeroVersionIndex(irMap.GetVerst2MeExprTableSize());
    irMap.PushBackVerst2MeExprTable(nullptr);
    ost->PushbackVersionIndex(ost->GetZeroVersionIndex());
  }
  VarMeExpr *varMeExpr = irMap.CreateNewVarMeExpr(*ost, PTY_ref, 0);
  if (isLocalRefVar) {
    tmpLocalRefVars.insert(varMeExpr);
  }
  return varMeExpr;
}

AnalysisResult *MeDoRCLowering::Run(MeFunction *func, MeFuncResultMgr *funcResMgr, ModuleResultMgr *moduleResMgr) {
  if (func->GetIRMap() == nullptr) {
    auto *hmap = static_cast<MeIRMap*>(funcResMgr->GetAnalysisResult(MeFuncPhase_IRMAP, func));
    CHECK_FATAL(hmap != nullptr, "hssamap has problem");
    func->SetIRMap(hmap);
  }
  CHECK_FATAL(func->GetMeSSATab() != nullptr, "ssatab has problem");
  RCLowering rcLowering(*func, DEBUGFUNC(func));

  rcLowering.Prepare();
  rcLowering.PreRCLower();
  rcLowering.RCLower();
  // handle all the extra RC work
  rcLowering.PostRCLower();
  rcLowering.Finish();
  return nullptr;
}
}  // namespace maple
