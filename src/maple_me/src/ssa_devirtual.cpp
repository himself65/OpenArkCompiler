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
#include "ssa_devirtual.h"

// This phase performs devirtualization based on SSA. Ideally, we should have
// precise alias information, so that for each reference we know exactly the
// objects it refers to, then the exact method it calls. However, precise alias
// analysis costs a lot.
//
// For now, we only use a simple policy to help devirtualize, E.g.
// Base b is defined as new Derived(), and b.foo() denotes b invokes method foo().
// We can devirtual the b.foo() to be Derived::foo().
namespace maple {
bool SSADevirtual::debug = false;

static bool MaybeNull(const MeExpr &expr) {
  if (expr.GetMeOp() == kMeOpVar) {
    return static_cast<const VarMeExpr*>(&expr)->GetMaybeNull();
  }
  if (expr.GetMeOp() == kMeOpIvar) {
    return static_cast<const IvarMeExpr*>(&expr)->GetMaybeNull();
  }
  if (expr.GetOp() == OP_retype) {
    MeExpr *retypeRHS = (static_cast<const OpMeExpr*>(&expr))->GetOpnd(0);
    if (retypeRHS->GetMeOp() == kMeOpVar) {
      return static_cast<VarMeExpr*>(retypeRHS)->GetMaybeNull();
    }
  }
  return true;
}

static bool IsFinalMethod(const MIRFunction *mirFunc) {
  if (mirFunc == nullptr) {
    return false;
  }
  const auto *classType = static_cast<const MIRClassType*>(mirFunc->GetClassType());
  // Return true if the method or its class is declared as final
  return (classType != nullptr && (mirFunc->IsFinal() || classType->IsFinal()));
}

TyIdx SSADevirtual::GetInferredTyIdx(MeExpr &expr) const {
  if (expr.GetMeOp() == kMeOpVar) {
    auto *varMeExpr = static_cast<VarMeExpr*>(&expr);
    if (varMeExpr->GetInferredTyIdx() == 0u) {
      // If varMeExpr->inferredTyIdx has not been set, we can double check
      // if it is coming from a static final field
      const OriginalSt *ost = irMap->GetSSATab().GetOriginalStFromID(varMeExpr->GetOStIdx());
      const MIRSymbol *mirSym = ost->GetMIRSymbol();
      if (mirSym->IsStatic() && mirSym->IsFinal() && mirSym->GetInferredTyIdx() != kInitTyIdx &&
          mirSym->GetInferredTyIdx() != kNoneTyIdx) {
        varMeExpr->SetInferredTyIdx(mirSym->GetInferredTyIdx());
      }
      if (mirSym->GetType()->GetKind() == kTypePointer) {
        MIRType *pointedType = (static_cast<MIRPtrType*>(mirSym->GetType()))->GetPointedType();
        if (pointedType->GetKind() == kTypeClass) {
          if ((static_cast<MIRClassType*>(pointedType))->IsFinal()) {
            varMeExpr->SetInferredTyIdx(pointedType->GetTypeIndex());
          }
        }
      }
    }
    return varMeExpr->GetInferredTyIdx();
  }
  if (expr.GetMeOp() == kMeOpIvar) {
    return static_cast<IvarMeExpr*>(&expr)->GetInferredTyIdx();
  }
  if (expr.GetOp() == OP_retype) {
    MeExpr *retypeRHS = (static_cast<OpMeExpr*>(&expr))->GetOpnd(0);
    if (retypeRHS->GetMeOp() == kMeOpVar) {
      return static_cast<VarMeExpr*>(retypeRHS)->GetInferredTyIdx();
    }
  }
  return TyIdx(0);
}

void SSADevirtual::ReplaceCall(CallMeStmt &callStmt, const MIRFunction &targetFunc) {
  if (SSADevirtual::debug) {
    MIRFunction &mirFunc = callStmt.GetTargetFunction();
    LogInfo::MapleLogger() << "[SSA-DEVIRT] " << kOpcodeInfo.GetTableItemAt(callStmt.GetOp()).name << " " <<
        NameMangler::DecodeName(mirFunc.GetName());
  }
  if (callStmt.GetOp() == OP_virtualicall || callStmt.GetOp() == OP_virtualicallassigned ||
      callStmt.GetOp() == OP_interfaceicall || callStmt.GetOp() == OP_interfaceicallassigned) {
    // delete 1st argument
    callStmt.EraseOpnds(callStmt.GetOpnds().begin());
  }
  MeExpr *receiver = callStmt.GetOpnd(0);
  if (NeedNullCheck(*receiver)) {
    InsertNullCheck(callStmt, *receiver);
    ++nullCheckCount;
  }
  // Set the actuall callee puIdx
  callStmt.SetPUIdx(targetFunc.GetPuidx());
  if (callStmt.GetOp() == OP_virtualcall || callStmt.GetOp() == OP_virtualicall) {
    callStmt.SetOp(OP_call);
    ++optedVirtualCalls;
  } else if (callStmt.GetOp() == OP_virtualcallassigned || callStmt.GetOp() == OP_virtualicallassigned) {
    callStmt.SetOp(OP_callassigned);
    ++optedVirtualCalls;
  } else if (callStmt.GetOp() == OP_interfacecall || callStmt.GetOp() == OP_interfaceicall) {
    callStmt.SetOp(OP_call);
    ++optedInterfaceCalls;
  } else if (callStmt.GetOp() == OP_interfacecallassigned || callStmt.GetOp() == OP_interfaceicallassigned) {
    callStmt.SetOp(OP_callassigned);
    ++optedInterfaceCalls;
  }
  if (clone != nullptr && OP_callassigned == callStmt.GetOp()) {
    clone->UpdateReturnVoidIfPossible(&callStmt, targetFunc);
  }
  if (SSADevirtual::debug) {
    LogInfo::MapleLogger() << "\t -> \t" << kOpcodeInfo.GetTableItemAt(callStmt.GetOp()).name << " " <<
        NameMangler::DecodeName(targetFunc.GetName());
    if (NeedNullCheck(*receiver)) {
      LogInfo::MapleLogger() << " with null-check ";
    }
    LogInfo::MapleLogger() << "\t at " << mod->GetFileNameFromFileNum(callStmt.GetSrcPosition().FileNum()) << ":" <<
        callStmt.GetSrcPosition().LineNum() << '\n';
  }
}

bool SSADevirtual::DevirtualizeCall(CallMeStmt &callStmt) {
  switch (callStmt.GetOp()) {
    case OP_interfacecall:
    case OP_interfaceicall:
    case OP_interfacecallassigned:
    case OP_interfaceicallassigned:
      totalInterfaceCalls++;  // FALLTHROUGH
    [[clang::fallthrough]];
    case OP_virtualcall:
    case OP_virtualicall:
    case OP_virtualcallassigned:
    case OP_virtualicallassigned: {
      totalVirtualCalls++;  // actually the number of interfacecalls + virtualcalls
      const MapleVector<MeExpr*> &parms = callStmt.GetOpnds();
      if (parms.empty() || parms[0] == nullptr) {
        break;
      }
      MeExpr *thisParm = parms[0];
      if (callStmt.GetOp() == OP_interfaceicall || callStmt.GetOp() == OP_interfaceicallassigned ||
          callStmt.GetOp() == OP_virtualicall || callStmt.GetOp() == OP_virtualicallassigned) {
        thisParm = parms[1];
      }
      TyIdx receiverInferredTyIdx = GetInferredTyIdx(*thisParm);
      MIRFunction &mirFunc = callStmt.GetTargetFunction();
      if (thisParm->GetPrimType() == PTY_ref && receiverInferredTyIdx != 0u) {
        Klass *inferredKlass = kh->GetKlassFromTyIdx(receiverInferredTyIdx);
        if (inferredKlass == nullptr) {
          break;
        }
        GStrIdx funcName = mirFunc.GetBaseFuncNameWithTypeStrIdx();
        MIRFunction *inferredFunction = inferredKlass->GetClosestMethod(funcName);
        if (inferredFunction == nullptr) {
          if (SSADevirtual::debug) {
            LogInfo::MapleLogger() << "Can not get function for " << inferredKlass->GetKlassName() <<
                mirFunc.GetBaseFuncNameWithType() << '\n';
          }
          break;
        }
        if (thisParm->GetMeOp() != kMeOpVar && thisParm->GetMeOp() != kMeOpIvar) {
          break;
        }
        ReplaceCall(callStmt, *inferredFunction);
        return true;
      } else if (IsFinalMethod(&mirFunc)) {
        GStrIdx uniqFuncNameStrIdx = mirFunc.GetNameStrIdx();
        CHECK_FATAL(uniqFuncNameStrIdx != 0u, "check");
        MIRSymbol *uniqFuncSym = GlobalTables::GetGsymTable().GetSymbolFromStrIdx(uniqFuncNameStrIdx);
        ASSERT(uniqFuncSym != nullptr, "The real callee %s has not been seen in any imported .mplt file",
               mirFunc.GetName().c_str());
        MIRFunction *uniqFunc = uniqFuncSym->GetFunction();
        ASSERT(mirFunc.GetBaseFuncNameWithType() == uniqFunc->GetBaseFuncNameWithType(),
               "Invalid function replacement in devirtualization");
        ReplaceCall(callStmt, *uniqFunc);
        return true;
      } else {
        if (thisParm->GetMeOp() == kMeOpVar) {
          auto *varMeExpr = static_cast<VarMeExpr*>(thisParm);
          const MapleVector<TyIdx> inferredTypeCandidates = varMeExpr->GetInferredTypeCandidates();
          if (inferredTypeCandidates.size() > 0) {
            GStrIdx funcName = mirFunc.GetBaseFuncNameWithTypeStrIdx();
            MIRFunction *inferredFunction = nullptr;
            size_t i = 0;
            for (; i < inferredTypeCandidates.size(); ++i) {
              Klass *inferredKlass = kh->GetKlassFromTyIdx(inferredTypeCandidates.at(i));
              if (inferredKlass == nullptr) {
                break;
              }
              MIRFunction *tmpFunction = inferredKlass->GetClosestMethod(funcName);
              if (tmpFunction == nullptr) {
                break;
              }
              if (inferredFunction == nullptr) {
                inferredFunction = tmpFunction;
              } else if (inferredFunction != tmpFunction) {
                break;
              }
            }
            if (i == inferredTypeCandidates.size() && inferredFunction != nullptr) {
              if (SSADevirtual::debug) {
                ASSERT_NOT_NULL(GetMIRFunction());
                LogInfo::MapleLogger() << "Devirutalize based on set of inferred types: In " <<
                    GetMIRFunction()->GetName() << "; Devirtualize: " << mirFunc.GetName() << '\n';
              }
              ReplaceCall(callStmt, *inferredFunction);
              return true;
            }
          }
        }
      }
      break;
    }
    default:
      break;
  }
  return false;
}

bool SSADevirtual::NeedNullCheck(const MeExpr &receiver) const {
  return MaybeNull(receiver);
}

// Java requires to throw Null-Pointer-Execption if the receiver of
// the virtualcall is null. We insert an eval(iread recevier, 0)
// statment perform the null-check.
void SSADevirtual::InsertNullCheck(const CallMeStmt &callStmt, MeExpr &receiver) const {
  UnaryMeStmt *nullCheck = irMap->New<UnaryMeStmt>(OP_assertnonnull);
  nullCheck->SetBB(callStmt.GetBB());
  nullCheck->SetSrcPos(callStmt.GetSrcPosition());
  nullCheck->SetMeStmtOpndValue(&receiver);
  callStmt.GetBB()->InsertMeStmtBefore(&callStmt, nullCheck);
}

void SSADevirtual::PropVarInferredType(VarMeExpr &varMeExpr) const {
  if (varMeExpr.GetInferredTyIdx() != 0u) {
    return;
  }
  if (varMeExpr.GetDefBy() == kDefByStmt) {
    DassignMeStmt &defStmt = utils::ToRef(safe_cast<DassignMeStmt>(varMeExpr.GetDefStmt()));
    MeExpr *rhs = defStmt.GetRHS();
    if (rhs->GetOp() == OP_gcmalloc) {
      varMeExpr.SetInferredTyIdx(static_cast<GcmallocMeExpr*>(rhs)->GetTyIdx());
      varMeExpr.SetMaybeNull(false);
      if (SSADevirtual::debug) {
        MIRType *type = GlobalTables::GetTypeTable().GetTypeFromTyIdx(varMeExpr.GetInferredTyIdx());
        LogInfo::MapleLogger() << "[SSA-DEVIRT] [TYPE-INFERRING] mx" << varMeExpr.GetExprID() << " ";
        type->Dump(0, false);
        LogInfo::MapleLogger() << '\n';
      }
    } else {
      TyIdx tyIdx = GetInferredTyIdx(*rhs);
      varMeExpr.SetMaybeNull(MaybeNull(*rhs));
      if (tyIdx != 0u) {
        varMeExpr.SetInferredTyIdx(tyIdx);
        if (SSADevirtual::debug) {
          MIRType *type = GlobalTables::GetTypeTable().GetTypeFromTyIdx(varMeExpr.GetInferredTyIdx());
          LogInfo::MapleLogger() << "[SSA-DEVIRT] [TYPE-INFERRING] mx" << varMeExpr.GetExprID() << " ";
          type->Dump(0, false);
          LogInfo::MapleLogger() << '\n';
        }
      }
    }
    if (varMeExpr.GetInferredTyIdx() != 0u) {
      OriginalSt *ost = irMap->GetSSATab().GetOriginalStFromID(defStmt.GetVarLHS()->GetOStIdx());
      MIRSymbol *mirSym = ost->GetMIRSymbol();
      if (mirSym->IsStatic() && mirSym->IsFinal()) {
        // static final field can store and propagate inferred typeinfo
        if (mirSym->GetInferredTyIdx() == kInitTyIdx) {
          // mirSym->_inferred_tyIdx has not been set before
          mirSym->SetInferredTyIdx(varMeExpr.GetInferredTyIdx());
        } else if (mirSym->GetInferredTyIdx() != varMeExpr.GetInferredTyIdx()) {
          // If mirSym->_inferred_tyIdx has been set before, it means we have
          // seen a divergence on control flow. Set to NONE if not all
          // branches reach the same conclusion.
          mirSym->SetInferredTyIdx(kNoneTyIdx);
        }
      }
    }
  } else if (varMeExpr.GetDefBy() == kDefByPhi) {
    if (SSADevirtual::debug) {
      LogInfo::MapleLogger() << "[SSA-DEVIRT] [TYPE-INFERRING] " << "Def by phi " << '\n';
    }
  }
}

void SSADevirtual::PropIvarInferredType(IvarMeExpr &ivar) const {
  if (ivar.GetInferredTyIdx() != 0u) {
    return;
  }
  IassignMeStmt *defStmt = ivar.GetDefStmt();
  if (defStmt == nullptr) {
    return;
  }
  MeExpr *rhs = defStmt->GetRHS();
  CHECK_NULL_FATAL(rhs);
  if (rhs->GetOp() == OP_gcmalloc) {
    ivar.GetInferredTyIdx() = static_cast<GcmallocMeExpr*>(rhs)->GetTyIdx();
    ivar.SetMaybeNull(false);
    if (SSADevirtual::debug) {
      MIRType *type = GlobalTables::GetTypeTable().GetTypeFromTyIdx(ivar.GetInferredTyIdx());
      LogInfo::MapleLogger() << "[SSA-DEVIRT] [TYPE-INFERRING] mx" << ivar.GetExprID() << " ";
      type->Dump(0, false);
      LogInfo::MapleLogger() << '\n';
    }
  } else {
    TyIdx tyIdx = GetInferredTyIdx(*rhs);
    ivar.SetMaybeNull(MaybeNull(*rhs));
    if (tyIdx != 0u) {
      ivar.SetInferredTyidx(tyIdx);
      if (SSADevirtual::debug) {
        MIRType *type = GlobalTables::GetTypeTable().GetTypeFromTyIdx(ivar.GetInferredTyIdx());
        LogInfo::MapleLogger() << "[SSA-DEVIRT] [TYPE-INFERRING] mx" << ivar.GetExprID() << " ";
        type->Dump(0, false);
        LogInfo::MapleLogger() << '\n';
      }
    }
  }
}

void SSADevirtual::VisitVarPhiNode(MePhiNode &varPhi) const {
  MapleVector<ScalarMeExpr*> opnds = varPhi.GetOpnds();
  VarMeExpr *lhs = static_cast<VarMeExpr *>(varPhi.GetLHS());

  // RegPhiNode cases NYI
  if (lhs == nullptr)
    return; 
  
  const MapleVector<TyIdx> &inferredTypeCandidates = lhs->GetInferredTypeCandidates();
  for (size_t i = 0; i < opnds.size(); ++i) {
    VarMeExpr *opnd = static_cast<VarMeExpr *>(opnds[i]);
    PropVarInferredType(*opnd);
    if (opnd->GetInferredTyIdx() != 0u) {
      size_t j = 0;
      for (; j < inferredTypeCandidates.size(); j++) {
        if (inferredTypeCandidates.at(j) == opnd->GetInferredTyIdx()) {
          break;
        }
      }
      if (j == inferredTypeCandidates.size()) {
        lhs->AddInferredTypeCandidate(opnd->GetInferredTyIdx());
      }
    } else {
      lhs->ClearInferredTypeCandidates();
      break;
    }
  }
}

void SSADevirtual::VisitMeExpr(MeExpr *meExpr) const {
  if (meExpr == nullptr) {
    return;
  }
  MeExprOp meOp = meExpr->GetMeOp();
  switch (meOp) {
    case kMeOpVar: {
      auto *varExpr = static_cast<VarMeExpr*>(meExpr);
      PropVarInferredType(*varExpr);
      break;
    }
    case kMeOpReg:
      break;
    case kMeOpIvar: {
      auto *iVar = static_cast<IvarMeExpr*>(meExpr);
      PropIvarInferredType(*iVar);
      break;
    }
    case kMeOpOp: {
      auto *meOpExpr = static_cast<OpMeExpr*>(meExpr);
      for (uint32 i = 0; i < kOperandNumTernary; ++i) {
        VisitMeExpr(meOpExpr->GetOpnd(i));
      }
      break;
    }
    case kMeOpNary: {
      auto *naryMeExpr = static_cast<NaryMeExpr*>(meExpr);
      for (MeExpr *opnd : naryMeExpr->GetOpnds()) {
        VisitMeExpr(opnd);
      }
      break;
    }
    case kMeOpAddrof:
    case kMeOpAddroffunc:
    case kMeOpGcmalloc:
    case kMeOpConst:
    case kMeOpConststr:
    case kMeOpConststr16:
    case kMeOpSizeoftype:
    case kMeOpFieldsDist:
      break;
    default:
      CHECK_FATAL(false, "MeOP NIY");
      break;
  }
}

void SSADevirtual::ReturnTyIdxInferring(const RetMeStmt &retMeStmt) {
  const MapleVector<MeExpr*> &opnds = retMeStmt.GetOpnds();
  CHECK_FATAL(opnds.size() <= 1, "Assume at most one return value for now");
  for (size_t i = 0; i < opnds.size(); ++i) {
    MeExpr *opnd = opnds[i];
    TyIdx tyIdx = GetInferredTyIdx(*opnd);
    if (retTy == kNotSeen) {
      // seen the first return stmt
      retTy = kSeen;
      inferredRetTyIdx = tyIdx;
    } else if (retTy == kSeen) {
      // has seen an inferred type before, check if they agreed
      if (inferredRetTyIdx != tyIdx) {
        retTy = kFailed;
        inferredRetTyIdx = TyIdx(0);  // not agreed, cleared.
      }
    }
  }
}

void SSADevirtual::TraversalMeStmt(MeStmt &meStmt) {
  Opcode op = meStmt.GetOp();
  switch (op) {
    case OP_dassign: {
      auto *varMeStmt = static_cast<DassignMeStmt*>(&meStmt);
      VisitMeExpr(varMeStmt->GetRHS());
      break;
    }
    case OP_regassign: {
      auto *regMeStmt = static_cast<RegassignMeStmt*>(&meStmt);
      VisitMeExpr(regMeStmt->GetRHS());
      break;
    }
    case OP_maydassign: {
      auto *maydStmt = static_cast<MaydassignMeStmt*>(&meStmt);
      VisitMeExpr(maydStmt->GetRHS());
      break;
    }
    case OP_iassign: {
      auto *ivarStmt = static_cast<IassignMeStmt*>(&meStmt);
      VisitMeExpr(ivarStmt->GetRHS());
      break;
    }
    case OP_syncenter:
    case OP_syncexit: {
      auto *syncMeStmt = static_cast<SyncMeStmt*>(&meStmt);
      const MapleVector<MeExpr*> &opnds = syncMeStmt->GetOpnds();
      for (size_t i = 0; i < opnds.size(); ++i) {
        MeExpr *opnd = opnds[i];
        VisitMeExpr(opnd);
      }
      break;
    }
    case OP_throw: {
      auto *thrMeStmt = static_cast<ThrowMeStmt*>(&meStmt);
      VisitMeExpr(thrMeStmt->GetOpnd());
      break;
    }
    case OP_assertnonnull:
    case OP_eval:
    case OP_free: {
      auto *unaryStmt = static_cast<UnaryMeStmt*>(&meStmt);
      VisitMeExpr(unaryStmt->GetOpnd());
      break;
    }
    case OP_call:
    case OP_virtualcall:
    case OP_virtualicall:
    case OP_superclasscall:
    case OP_interfacecall:
    case OP_interfaceicall:
    case OP_customcall:
    case OP_polymorphiccall:
    case OP_callassigned:
    case OP_virtualcallassigned:
    case OP_virtualicallassigned:
    case OP_superclasscallassigned:
    case OP_interfacecallassigned:
    case OP_interfaceicallassigned:
    case OP_customcallassigned:
    case OP_polymorphiccallassigned: {
      auto *callMeStmt = static_cast<CallMeStmt*>(&meStmt);
      const MapleVector<MeExpr*> &opnds = callMeStmt->GetOpnds();
      for (size_t i = 0; i < opnds.size(); ++i) {
        MeExpr *opnd = opnds[i];
        VisitMeExpr(opnd);
      }
      (void)DevirtualizeCall(*callMeStmt);
      if (clone != nullptr && OP_callassigned == callMeStmt->GetOp()) {
        MIRFunction &targetFunc = callMeStmt->GetTargetFunction();
        clone->UpdateReturnVoidIfPossible(callMeStmt, targetFunc);
      }
      break;
    }
    case OP_icall:
    case OP_icallassigned: {
      auto *icallMeStmt = static_cast<IcallMeStmt*>(&meStmt);
      const MapleVector<MeExpr*> &opnds = icallMeStmt->GetOpnds();
      for (size_t i = 0; i < opnds.size(); ++i) {
        MeExpr *opnd = opnds[i];
        VisitMeExpr(opnd);
      }
      break;
    }
    case OP_intrinsiccallwithtype:
    case OP_intrinsiccall:
    case OP_xintrinsiccall:
    case OP_intrinsiccallwithtypeassigned:
    case OP_intrinsiccallassigned:
    case OP_xintrinsiccallassigned: {
      auto *intrinCallStmt = static_cast<IntrinsiccallMeStmt*>(&meStmt);
      const MapleVector<MeExpr*> &opnds = intrinCallStmt->GetOpnds();
      for (size_t i = 0; i < opnds.size(); ++i) {
        MeExpr *opnd = opnds[i];
        VisitMeExpr(opnd);
      }
      break;
    }
    case OP_brtrue:
    case OP_brfalse: {
      auto *condGotoStmt = static_cast<CondGotoMeStmt*>(&meStmt);
      VisitMeExpr(condGotoStmt->GetOpnd());
      break;
    }
    case OP_switch: {
      auto *switchStmt = static_cast<SwitchMeStmt*>(&meStmt);
      VisitMeExpr(switchStmt->GetOpnd());
      break;
    }
    case OP_return: {
      auto *retMeStmt = static_cast<RetMeStmt*>(&meStmt);
      const MapleVector<MeExpr*> &opnds = retMeStmt->GetOpnds();
      for (size_t i = 0; i < opnds.size(); ++i) {
        MeExpr *opnd = opnds[i];
        VisitMeExpr(opnd);
      }
      ReturnTyIdxInferring(*retMeStmt);
      break;
    }
    case OP_assertlt:
    case OP_assertge: {
      auto *assMeStmt = static_cast<AssertMeStmt*>(&meStmt);
      VisitMeExpr(assMeStmt->GetOpnd(0));
      VisitMeExpr(assMeStmt->GetOpnd(1));
      break;
    }
    case OP_jstry:
    case OP_jscatch:
    case OP_finally:
    case OP_endtry:
    case OP_cleanuptry:
    case OP_try:
    case OP_catch:
    case OP_goto:
    case OP_gosub:
    case OP_retsub:
    case OP_comment:
    case OP_membaracquire:
    case OP_membarrelease:
    case OP_membarstoreload:
    case OP_membarstorestore:
      break;
    default:
      CHECK_FATAL(false, "unexpected stmt in ssadevirt or NYI");
  }
  if (meStmt.GetOp() != OP_callassigned) {
    return;
  }
  MapleVector<MustDefMeNode> *mustDefList = meStmt.GetMustDefList();
  if (mustDefList->empty()) {
    return;
  }
  MeExpr *meLHS = mustDefList->front().GetLHS();
  if (meLHS->GetMeOp() != kMeOpVar) {
    return;
  }
  auto *lhsVar = static_cast<VarMeExpr*>(meLHS);
  auto *callMeStmt = static_cast<CallMeStmt*>(&meStmt);
  MIRFunction &called = callMeStmt->GetTargetFunction();
  if (called.GetInferredReturnTyIdx() != 0u) {
    lhsVar->SetInferredTyIdx(called.GetInferredReturnTyIdx());
    if (SSADevirtual::debug) {
      MIRType *type = GlobalTables::GetTypeTable().GetTypeFromTyIdx(lhsVar->GetInferredTyIdx());
      LogInfo::MapleLogger() << "[SSA-DEVIRT] [TYPE-INFERRING] mx" << lhsVar->GetExprID() << " ";
      type->Dump(0, false);
      LogInfo::MapleLogger() << '\n';
    }
  }
}

void SSADevirtual::TraversalBB(BB *bb) {
  if (bb == nullptr) {
    return;
  }
  if (bbVisited[bb->GetBBId()]) {
    return;
  }
  bbVisited[bb->GetBBId()] = true;
  // traversal var phi nodes
  MapleMap<OStIdx, MePhiNode*> &mePhiList = bb->GetMePhiList();
  for (auto it = mePhiList.begin(); it != mePhiList.end(); ++it) {
    MePhiNode *phiMeNode = it->second;
    VisitVarPhiNode(*phiMeNode);
  }
  // traversal reg phi nodes (NYI)
  // traversal on stmt
  for (auto &meStmt : bb->GetMeStmts()) {
    TraversalMeStmt(meStmt);
  }
}

void SSADevirtual::Perform(BB &entryBB) {
  // Pre-order traverse the cominance tree, so that each def is traversed
  // before its use
  std::queue<BB*> bbList;
  bbList.push(&entryBB);
  while (!bbList.empty()) {
    BB *bb = bbList.front();
    bbList.pop();
    TraversalBB(bb);
    const MapleSet<BBId> &domChildren = dom->GetDomChildren(bb->GetBBId());
    for (const BBId &bbId : domChildren) {
      bbList.push(GetBB(bbId));
    }
  }
  MIRFunction *mirFunc = GetMIRFunction();
  if (mirFunc == nullptr) {
    return;  // maybe wpo
  }
  if (retTy == kSeen) {
    mirFunc->SetInferredReturnTyIdx(this->inferredRetTyIdx);
  }
  // Simple rule: if method's declared returning type is a final class, then
  // the actual returning type is same with the declared returning type.
  MIRType *declReturnType = mirFunc->GetReturnType();
  if (declReturnType->GetPrimType() == PTY_ref && declReturnType->GetKind() == kTypePointer) {
    MIRType *pointedType = static_cast<MIRPtrType*>(declReturnType)->GetPointedType();
    MIRClassType *declReturnClass = safe_cast<MIRClassType>(pointedType);
    if (declReturnClass != nullptr && declReturnClass->IsFinal()) {
      mirFunc->SetInferredReturnTyIdx(declReturnClass->GetTypeIndex());
    }
  }
  if (SSADevirtual::debug) {
    LogInfo::MapleLogger() << "[SSA-DEVIRT]" << " {virtualcalls: total " << (totalVirtualCalls - totalInterfaceCalls) <<
        ", devirtualized " << optedVirtualCalls << "}" << " {interfacecalls: total " <<
        totalInterfaceCalls << ", devirtualized " << optedInterfaceCalls << "}" <<
        ", {null-checks: " << nullCheckCount << "}" << "\t" << mirFunc->GetName() << '\n';
    if (mirFunc != nullptr && mirFunc->GetInferredReturnTyIdx() != 0u) {
      MIRType *type = GlobalTables::GetTypeTable().GetTypeFromTyIdx(mirFunc->GetInferredReturnTyIdx());
      LogInfo::MapleLogger() << "[SSA-DEVIRT] [FUNC-RETTYPE] ";
      type->Dump(0, false);
      LogInfo::MapleLogger() << '\n';
    }
  }
}
}  // namespace maple
