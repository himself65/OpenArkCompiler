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
#include "me_delegate_rc.h"
#include "mir_builder.h"
#include "me_hdse.h"

// This phase finds local ref pointer variables that are delegated and thus can
// have their RC omitted.  This optimization is done on a per-SSA version basis.
// As a result, an overall criterion is that the SSA version must not have
// appeared as phi operand;
//
// There are two forms of delegation:
//
// Form A. The following conditions have to be satisfied for an SSA version to be
// delegated:
// (1) In the BB where it is defined, there is a last use of it as the assigned
//     value in an assignment statement, which is either iassign or
//     dassign-to-global, and the assignment statement is marked need_incref.
// (2) Between its definition and last use, there is no statement that may
//     potentially raise exception.
// For each such case, the optimization is as follows:
// (a) The SSA version is renamed to a new and unique preg with PTY_ptr type;
//     this has the effect of eliminating its store and loads and its
//     decref-before-return;
// (b) The need_incref flag in the respective assignment statement is cleared.
// The reference count correctness is maintained because the removed decref and
// incref are for the same offect.
//
// Form B. An SSA version is determined to not need incref at its def and not
// need decref when it becomes dead.  In this case, the SSA version is renamed
// to a new and unique preg with PTY_ptr type as in (a) above.
//
// There are different ways to establish that an SSA version does not any RC:
// B1: The SSA version is never dereferenced and never copied to another variable
//     or passed on as a parameter.
// B2: The RHS of its definition is iread of a final field with this as base or
//     dread of a static final field.
// B3: Within the SSA version's live range, there is no operation that can result
//     in decref of any object.
namespace {
// following intrinsics can throw exception
const std::set<maple::MIRIntrinsicID> canThrowIntrinsicsList {
    maple::INTRN_MPL_CLINIT_CHECK,
    maple::INTRN_MPL_BOUNDARY_CHECK,
    maple::INTRN_JAVA_CLINIT_CHECK,
    maple::INTRN_JAVA_CHECK_CAST,
    maple::INTRN_JAVA_THROW_ARITHMETIC,
};
}

namespace maple {
static bool CheckOp(const MeStmt &stmt, const Opcode op) {
  return stmt.GetOp() == op;
}
static bool IsIntrinsic(const MeStmt &stmt) {
  return CheckOp(stmt, OP_intrinsiccall) || CheckOp(stmt, OP_intrinsiccallassigned) ||
         CheckOp(stmt, OP_xintrinsiccall) || CheckOp(stmt, OP_xintrinsiccallassigned) ||
         CheckOp(stmt, OP_intrinsiccallwithtype) || CheckOp(stmt, OP_intrinsiccallwithtypeassigned);
}

static bool IsCleanUpStmt(const MeStmt &stmt) {
  return CheckOp(stmt, OP_intrinsiccall) &&
         static_cast<const IntrinsiccallMeStmt&>(stmt).GetIntrinsic() == INTRN_MPL_CLEANUP_LOCALREFVARS;
}

static bool IsVarDecRefStmt(const MeStmt &stmt) {
  return (CheckOp(stmt, OP_decref) && stmt.GetOpnd(0)->GetMeOp() == kMeOpVar) ||
         CheckOp(stmt, OP_decrefreset);
}

void DelegateRC::SetCantDelegate(const MapleMap<OStIdx, MeVarPhiNode*> &meVarPhiList) {
  for (auto it = meVarPhiList.begin(); it != meVarPhiList.end(); ++it) {
    const OriginalSt *ost = ssaTab.GetOriginalStFromID(it->first);
    if (!ost->IsSymbolOst() || ost->GetIndirectLev() != 0) {
      continue;
    }
    MeVarPhiNode *mePhi = it->second;
    if (!mePhi->GetIsLive()) {
      continue;
    }
    for (VarMeExpr *phiOpnd : mePhi->GetOpnds()) {
      verStCantDelegate[phiOpnd->GetVstIdx()] = true;
    }
  }
}

void DelegateRC::SaveDerefedOrCopiedVst(const MeExpr *expr) {
  CHECK_NULL_FATAL(expr);
  while (expr->GetOp() == OP_retype) {
    expr = expr->GetOpnd(0);
  }
  if (expr->GetMeOp() == kMeOpVar) {
    const auto *varExpr = static_cast<const VarMeExpr*>(expr);
    verStDerefedCopied[varExpr->GetVstIdx()] = true;
  }
}

bool DelegateRC::IsCopiedOrDerefedOp(Opcode op) const {
  return op == OP_dassign || op == OP_maydassign || op == OP_regassign || op == OP_syncenter ||
         op == OP_syncexit || op == OP_throw || op == OP_return || op == OP_iassign ||  // cause var copied
         kOpcodeInfo.IsCall(op);  // callstmt need considering parameter
}

void DelegateRC::CollectDerefedOrCopied(const MeStmt &stmt) {
  Opcode op = stmt.GetOp();
  if (!IsCopiedOrDerefedOp(op)) {
    return;
  }
  for (size_t i = 0; i < stmt.NumMeStmtOpnds(); ++i) {
    MeExpr *curOpnd = stmt.GetOpnd(i);
    SaveDerefedOrCopiedVst(curOpnd);
  }
}

void DelegateRC::CollectDerefedOrCopied(const MeExpr &expr) {
  Opcode op = expr.GetOp();
  if (op == OP_iaddrof || op == OP_add) {
    for (size_t i = 0; i < expr.GetNumOpnds(); ++i) {
      SaveDerefedOrCopiedVst(expr.GetOpnd(i));
    }
    return;
  }

  if (op == OP_cvt) {
    // in some cases, we have cvt from int to ref
    // dassign %Reg0_I 0 (cvt i32 ref (dread ref %Reg0_XXXX))
    // cvt ref i32 (dread i32 %Reg0_I)
    if (expr.GetPrimType() == PTY_ref && expr.GetOpnd(0)->GetMeOp() == kMeOpVar) {
      auto *baseVar = static_cast<VarMeExpr*>(expr.GetOpnd(0));
      verStDerefedCopied[baseVar->GetVstIdx()] = true;
      // collect the def of basevar
      if (baseVar->GetDefBy() == kDefByStmt) {
        MeStmt *defStmt = baseVar->GetDefStmt();
        if (defStmt->GetOp() == OP_dassign && defStmt->GetRHS() != nullptr && defStmt->GetRHS()->GetOp() == OP_cvt) {
          SaveDerefedOrCopiedVst(defStmt->GetRHS()->GetOpnd(0));
        }
      } else if (baseVar->GetDefBy() == kDefByPhi) {
        MeVarPhiNode &defPhi = baseVar->GetDefPhi();
        for (VarMeExpr *phiOpnd : defPhi.GetOpnds()) {
          if (phiOpnd->GetDefBy() == kDefByStmt) {
            MeStmt *defStmt = phiOpnd->GetDefStmt();
            if (defStmt->GetOp() == OP_dassign && defStmt->GetRHS() != nullptr &&
                defStmt->GetRHS()->GetOp() == OP_cvt) {
              SaveDerefedOrCopiedVst(defStmt->GetRHS()->GetOpnd(0));
            }
          }
        }  // next phi opnd
      }  // end of all phi
    }
    return;
  }

  if (op == OP_array ||
      (op == OP_intrinsicop && static_cast<const NaryMeExpr&>(expr).GetIntrinsic() == INTRN_JAVA_ARRAY_LENGTH) ||
      (op == OP_intrinsicopwithtype && static_cast<const NaryMeExpr&>(expr).GetIntrinsic() == INTRN_JAVA_INSTANCE_OF)) {
    CHECK_FATAL(expr.GetNumOpnds() != 0, "container check");
    SaveDerefedOrCopiedVst(expr.GetOpnd(0));
    return;
  }

  if (expr.GetMeOp() == kMeOpIvar) {
    SaveDerefedOrCopiedVst(expr.GetOpnd(0));
  }
}

void DelegateRC::CollectVstCantDecrefEarly(MeExpr &opnd0, MeExpr &opnd1) {
  if (opnd0.GetMeOp() != kMeOpVar || opnd1.GetMeOp() != kMeOpVar) {
    return;
  }
  auto &varOpnd0 = static_cast<VarMeExpr&>(opnd0);
  auto &varOpnd1 = static_cast<VarMeExpr&>(opnd1);
  if (varOpnd0.GetDefBy() == kDefByNo || varOpnd1.GetDefBy() == kDefByNo) {
    return;
  }
  BB *opnd0DefBB = varOpnd0.DefByBB();
  BB *opnd1DefBB = varOpnd1.DefByBB();

  if (opnd0DefBB == opnd1DefBB) {
    verStCantDecrefEarly[varOpnd0.GetVstIdx()] = true;
    verStCantDecrefEarly[varOpnd1.GetVstIdx()] = true;
  } else if (dominance.Dominate(*opnd0DefBB, *opnd1DefBB)) {
    verStCantDecrefEarly[varOpnd0.GetVstIdx()] = true;
  } else {
    verStCantDecrefEarly[varOpnd1.GetVstIdx()] = true;
  }
}

void DelegateRC::CollectUseCounts(const MeExpr &expr) {
  if (expr.GetMeOp() == kMeOpVar) {
    const auto &varMeExpr = static_cast<const VarMeExpr&>(expr);
    ASSERT(varMeExpr.GetVstIdx() < verStUseCounts.size(), "CollectUsesInfo: vstIdx out of bounds");
    verStUseCounts[varMeExpr.GetVstIdx()]++;
  }
}

void DelegateRC::CollectUsesInfo(const MeExpr &expr) {
  for (size_t i = 0; i < expr.GetNumOpnds(); ++i) {
    CollectUsesInfo(*expr.GetOpnd(i));
  }
  if ((expr.GetOp() == OP_eq || expr.GetOp() == OP_ne) && static_cast<const OpMeExpr&>(expr).GetOpndType() == PTY_ref) {
    CollectVstCantDecrefEarly(*expr.GetOpnd(0), *expr.GetOpnd(1));
  }
  CollectDerefedOrCopied(expr);
  CollectUseCounts(expr);
}

// traverse expression x; at each occurrence of rhsvar in x, decrement
// remaining_uses
void DelegateRC::FindAndDecrUseCount(const VarMeExpr &rhsVar, const MeExpr &expr, int32 &remainingUses) const {
  for (size_t i = 0; i < expr.GetNumOpnds(); ++i) {
    FindAndDecrUseCount(rhsVar, *expr.GetOpnd(i), remainingUses);
  }
  if (&expr == &rhsVar) {
    --remainingUses;
  }
}

bool DelegateRC::MayThrowException(const MeStmt &stmt) const {
  if (CheckOp(stmt, OP_maydassign) || CheckOp(stmt, OP_throw)) {
    return true;
  }

  if (CheckOp(stmt, OP_dassign)) {
    const auto &dass = static_cast<const DassignMeStmt&>(stmt);
    const MeExpr *rhs = dass.GetRHS();
    CHECK_NULL_FATAL(rhs);
    return dass.GetWasMayDassign() || rhs->GetOp() == OP_gcmalloc || rhs->GetOp() == OP_gcmallocjarray;
  }

  if (CheckOp(stmt, OP_regassign)) {
    ASSERT_NOT_NULL(stmt.GetRHS());
    return stmt.GetRHS()->GetOp() == OP_gcmalloc || stmt.GetRHS()->GetOp() == OP_gcmallocjarray;
  }

  if (IsIntrinsic(stmt)) {
    const auto &intrn = static_cast<const IntrinsiccallMeStmt&>(stmt);
    return canThrowIntrinsicsList.find(intrn.GetIntrinsic()) != canThrowIntrinsicsList.end();
  }

  if (kOpcodeInfo.IsCall(stmt.GetOp())) {
    const auto &callStmt = static_cast<const CallMeStmt&>(stmt);
    MIRFunction *callee = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(callStmt.GetPUIdx());
    return (!callee->GetFuncAttrs().GetAttr(FUNCATTR_nothrow_exception)) ||
           (!MeOption::ignoreIPA && !callee->IsNoThrowException());
  }
  return false;
}

// Traverse backwards from fromstmt to tostmt to see if the single use of rhsvar
// in fromstmt is the last use of rhsvar; tostmt is the statement that defines
// rhsvar, so it can be assumed that tostmt does not contain any use; this check
// make use of verStUseCounts in its determination. In addition, if it comes
// across any stmt that can raise exception, also return false.
bool DelegateRC::ContainAllTheUses(const VarMeExpr &rhsVar, const MeStmt &fromStmt, const MeStmt &toStmt) const {
  int32 remainingUses = static_cast<uint32>(verStUseCounts[rhsVar.GetVstIdx()]) - 1;
  for (MeStmt *cur = fromStmt.GetPrev(); cur != &toStmt; cur = cur->GetPrev()) {
    // do not count decref operands
    if (IsVarDecRefStmt(*cur)) {
      continue;
    }
    if (MayThrowException(*cur)) {
      return false;
    }
    if (IsCleanUpStmt(*cur)) {
      continue;
    }
    for (size_t i = 0; i < cur->NumMeStmtOpnds(); ++i) {
      FindAndDecrUseCount(rhsVar, *cur->GetOpnd(i), remainingUses);
    }
  }
  CHECK_FATAL(remainingUses >= 0, "ContainAllTheUses: inconsistent use count");
  return remainingUses == 0;
}

// return the RegMeExpr node to replace the original temp; nullptr if not successful
RegMeExpr *DelegateRC::RHSTempDelegated(MeExpr &rhs, const MeStmt &useStmt) {
  if (rhs.GetMeOp() != kMeOpVar) {
    return nullptr;
  }
  auto &rhsVar = static_cast<VarMeExpr&>(rhs);
  if (verStCantDelegate[rhsVar.GetVstIdx()]) {
    return nullptr;
  }
  if (refVar2RegMap.find(&rhsVar) != refVar2RegMap.end()) {
    return nullptr;  // already delegated by another assignment
  }
  const OriginalSt *ost = ssaTab.GetOriginalStFromID(rhsVar.GetOStIdx());
  if (ost->IsFormal() || ost->GetMIRSymbol()->IsGlobal()) {
    return nullptr;
  }
  if (rhsVar.GetDefBy() == kDefByMustDef) {
    MustDefMeNode &mustDef = rhsVar.GetDefMustDef();
    ASSERT(mustDef.GetLHS() == &rhsVar, "DelegateRCTemp: inconsistent mustdef");
    MeStmt *callStmt = mustDef.GetBase();
    if (callStmt->GetBB() != useStmt.GetBB()) {
      return nullptr;
    }
    if (!ContainAllTheUses(rhsVar, useStmt, *callStmt)) {
      return nullptr;
    }
    if (enabledDebug) {
      LogInfo::MapleLogger() << "delegaterc of form A for func " << func.GetName() << '\n';
      LogInfo::MapleLogger() << "\nreplace stmt :\n" << '\n';
      callStmt->Dump(func.GetIRMap());
    }
    // replace temp by a new preg
    rhsVar.SetDefBy(kDefByNo);
    RegMeExpr *curReg = nullptr;
    if (ost->GetMIRSymbol()->GetType()->GetPrimType() == PTY_ptr) {
      curReg = irMap.CreateRegMeExpr(PTY_ptr);
    } else {
      curReg = irMap.CreateRegRefMeExpr(rhsVar);
    }
    refVar2RegMap[&rhsVar] = curReg;  // record this replacement
    mustDef.UpdateLHS(*curReg);
    if (enabledDebug) {
      LogInfo::MapleLogger() << "with stmt :\n" << '\n';
      mustDef.GetBase()->Dump(func.GetIRMap());
    }
    return curReg;
  } else if (rhsVar.GetDefBy() == kDefByStmt) {
    MeStmt *defStmt = rhsVar.GetDefStmt();
    if (defStmt->GetBB() != useStmt.GetBB()) {
      return nullptr;
    }
    if (!ContainAllTheUses(rhsVar, useStmt, *defStmt)) {
      return nullptr;
    }
    MeExpr *rhsExpr = defStmt->GetRHS();
    bool defStmtNeedIncref = defStmt->NeedIncref();
    CHECK_FATAL(defStmt->GetOp() == OP_dassign || defStmt->GetOp() == OP_maydassign,
                "DelegateRCTemp: unexpected stmt op for kDefByStmt");
    ASSERT(defStmt->GetVarLHS() == &rhsVar, "DelegateRCTemp: inconsistent def by dassign");
    if (enabledDebug) {
      LogInfo::MapleLogger() << "delegaterc of form A for func " << func.GetName() << '\n';
      LogInfo::MapleLogger() << "\nreplace stmt :\n" << '\n';
      defStmt->Dump(func.GetIRMap());
    }
    // replace temp by a new preg
    rhsVar.SetDefBy(kDefByNo);
    RegMeExpr *curReg = irMap.CreateRegRefMeExpr(rhsVar);
    refVar2RegMap[&rhsVar] = curReg;  // record this replacement
    // create new regassign statement
    MeStmt *regass = irMap.CreateRegassignMeStmt(*curReg, *rhsExpr, *defStmt->GetBB());
    curReg->SetDefByStmt(*regass);
    if (defStmtNeedIncref) {
      regass->EnableNeedIncref();
    } else {
      regass->DisableNeedIncref();
    }
    defStmt->GetBB()->ReplaceMeStmt(defStmt, regass);
    regass->SetSrcPos(defStmt->GetSrcPosition());
    if (enabledDebug) {
      LogInfo::MapleLogger() << "with stmt :\n" << '\n';
      regass->Dump(func.GetIRMap());
    }
    return curReg;
  }
  return nullptr;
}

// process each assignment statement for Form A delegation
void DelegateRC::DelegateRCTemp(MeStmt &stmt) {
  switch (stmt.GetOp()) {
    case OP_iassign: {
      if (!stmt.NeedIncref()) {
        break;
      }
      IvarMeExpr *lhs = static_cast<IassignMeStmt&>(stmt).GetLHSVal();
      if (lhs->IsRCWeak() || lhs->IsVolatile()) {
        break;
      }
      if (lhs->GetBase()->GetOp() == OP_array) {
        // array may raise exception
        break;
      }
      MIRType *baseType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(lhs->GetTyIdx());
      ASSERT(baseType->GetKind() == kTypePointer, "DelegateRCTemp: unexpected type");
      MIRType *pointedType = static_cast<MIRPtrType*>(baseType)->GetPointedType();
      if (pointedType->GetKind() == kTypeClass &&
          static_cast<MIRClassType*>(pointedType)->IsFieldRCUnownedRef(lhs->GetFieldID())) {
        break;
      }
      MeExpr *rhs = stmt.GetRHS();
      CHECK_FATAL(rhs != nullptr, "null rhs check");
      RegMeExpr *curReg = RHSTempDelegated(*rhs, stmt);
      if (curReg != nullptr) {
        rhs = curReg;
        stmt.DisableNeedIncref();
      }
      break;
    }
    case OP_dassign: {
      if (!stmt.NeedIncref()) {
        break;
      }
      VarMeExpr *lhsVar = stmt.GetVarLHS();
      CHECK_FATAL(lhsVar != nullptr, "null lhs check");
      MeExpr *rhs = stmt.GetRHS();
      CHECK_FATAL(rhs != nullptr, "null rhs check");
      RegMeExpr *curReg = RHSTempDelegated(*rhs, stmt);
      if (curReg != nullptr) {
        rhs = curReg;
        stmt.DisableNeedIncref();
      }
      break;
    }
    case OP_return: {
      auto &retStmt = static_cast<RetMeStmt&>(stmt);
      if (!retStmt.NumMeStmtOpnds()) {
        break;
      }
      MeExpr *ret = retStmt.GetOpnd(0);
      if (ret->GetPrimType() != PTY_ref && ret->GetPrimType() != PTY_ptr) {
        break;
      }
      if (ret->GetMeOp() == kMeOpVar) {
        auto *val = static_cast<VarMeExpr*>(ret);
        if (val->GetDefBy() == kDefByMustDef) {
          MeStmt *defStmt = val->GetDefMustDef().GetBase();
          if (retStmt.GetBB() == defStmt->GetBB() && ContainAllTheUses(*val, stmt, *defStmt)) {
            RegMeExpr *curReg = RHSTempDelegated(*ret, stmt);
            if (curReg != nullptr) {
              retStmt.SetOpnd(0, curReg);
            }
          }
        } else if (val->GetDefBy() == kDefByStmt) {
          MeStmt *defStmt = val->GetDefStmt();
          MeExpr *rhs = defStmt->GetRHS();
          CHECK_FATAL(rhs != nullptr, "null rhs check");
          const OriginalSt *ost = nullptr;
          if (rhs->GetMeOp() == kMeOpVar) {
            auto *theVar = static_cast<VarMeExpr*>(rhs);
            ost = ssaTab.GetSymbolOriginalStFromID(theVar->GetOStIdx());
          }
          if (rhs->IsGcmalloc() || (rhs->GetMeOp() == kMeOpIvar && !static_cast<IvarMeExpr*>(rhs)->IsFinal()) ||
              (rhs->GetMeOp() == kMeOpVar && !ost->IsFinal() && ost->GetMIRSymbol()->IsGlobal()) ||
              (rhs->GetOp() == OP_regread && static_cast<RegMeExpr*>(rhs)->GetRegIdx() == -kSregThrownval)) {
            if (retStmt.GetBB() == defStmt->GetBB() && ContainAllTheUses(*val, stmt, *defStmt)) {
              RegMeExpr *curReg = RHSTempDelegated(*ret, stmt);
              if (curReg != nullptr) {
                retStmt.SetOpnd(0, curReg);
                // Convert following cases:
                //   dassign %Reg_xxx  (iread ref xxx)
                //   return (dread ref %Reg_xxx)
                // To:
                //   // iread will be converted to LoadRefField
                //   regassign %1 (iread ref xxx)  [RC+]
                //   return (regread ref %1)
                if (rhs->GetMeOp() == kMeOpIvar ||
                    (rhs->GetMeOp() == kMeOpVar && !ost->IsFinal() && ost->GetMIRSymbol()->IsGlobal())) {
                  curReg->GetDefStmt()->EnableNeedIncref();
                }
              }
            }
          }
        }
      }
      break;
    }
    default:
      break;
  }
}

bool DelegateRC::FinalRefNoRC(const MeExpr &expr) const {
  if (expr.GetMeOp() == kMeOpVar) {
    const auto &theVar = static_cast<const VarMeExpr&>(expr);
    const OriginalSt *ost = ssaTab.GetSymbolOriginalStFromID(theVar.GetOStIdx());
    return ost->IsFinal() && ost->GetMIRSymbol()->IsGlobal();
  } else if (expr.GetMeOp() == kMeOpIvar) {
    if (func.GetMirFunc()->IsConstructor() || func.GetMirFunc()->IsStatic() ||
        func.GetMirFunc()->GetFormalCount() == 0) {
      return false;
    }
    const auto &ivar = static_cast<const IvarMeExpr&>(expr);
    MIRType *ty = GlobalTables::GetTypeTable().GetTypeFromTyIdx(ivar.GetTyIdx());
    ASSERT(ty->GetKind() == kTypePointer, "FinalRefNoRC: pointer type expected");
    MIRType *pointedTy = static_cast<MIRPtrType*>(ty)->GetPointedType();
    if (pointedTy->GetKind() == kTypeClass) {
      auto *structType = static_cast<MIRStructType*>(pointedTy);
      FieldID fieldID = ivar.GetFieldID();
      if (structType->IsFieldFinal(fieldID) && !structType->IsFieldRCUnownedRef(fieldID)) {
        if (ivar.GetBase()->GetMeOp() != kMeOpVar) {
          return false;
        }
        const auto *varMeExpr = static_cast<const VarMeExpr*>(ivar.GetBase());
        const OriginalSt *ost = ssaTab.GetOriginalStFromID(varMeExpr->GetOStIdx());
        if (ost->IsSymbolOst()) {
          const MIRSymbol *mirst = ost->GetMIRSymbol();
          return mirst == func.GetMirFunc()->GetFormal(0);
        }
      }
    }
  }
  return false;
}

// return true if it is OK to omit reference counting for the LHS variable; if
// returning true, only_with_decref specifies whether a decref needs inserted
bool DelegateRC::CanOmitRC4LHSVar(const MeStmt &stmt, bool &onlyWithDecref) const {
  onlyWithDecref = false;
  switch (stmt.GetOp()) {
    case OP_dassign:
    case OP_maydassign: {
      const VarMeExpr *theLhs = stmt.GetVarLHS();
      MeExpr *theRhs = stmt.GetRHS();
      CHECK_FATAL(theLhs != nullptr, "null ptr check");
      CHECK_FATAL(theRhs != nullptr, "null ptr check");
      if (theLhs->GetPrimType() != PTY_ref || theLhs->GetNoDelegateRC()) {
        return false;
      }
      const OriginalSt *ost = ssaTab.GetOriginalStFromID(theLhs->GetOStIdx());
      if (!ost->IsLocal() || ost->IsFormal()) {
        return false;
      }
      if (ost->GetMIRSymbol()->IsInstrumented()) {
        return false;
      }
      if (verStCantDelegate[theLhs->GetVstIdx()]) {
        return false;
      }
      if (theRhs->GetMeOp() == kMeOpIvar) {
        auto *ivarRhs = static_cast<IvarMeExpr*>(theRhs);
        if (ivarRhs->IsVolatile() || ivarRhs->IsRCWeak()) {
          return false;
        }
      }
      // condition B2
      if (FinalRefNoRC(*theRhs)) {
        return true;
      }
      // condition B1
      if (!verStDerefedCopied[theLhs->GetVstIdx()]) {
        onlyWithDecref = theRhs->GetOp() == OP_gcmalloc || theRhs->GetOp() == OP_gcmallocjarray ||
            (theRhs->GetOp() == OP_regread && static_cast<RegMeExpr*>(theRhs)->GetRegIdx() == -kSregThrownval);
        if (onlyWithDecref && verStCantDecrefEarly[theLhs->GetVstIdx()]) {
          onlyWithDecref = false;
          return false;
        }
        return true;
      }
      break;
    }
    default:
      if (kOpcodeInfo.IsCallAssigned(stmt.GetOp())) {
        const MapleVector<MustDefMeNode> &mustdefList = stmt.GetMustDefList();
        if (mustdefList.empty()) {
          return false;
        }
        const MeExpr *lhs = mustdefList.front().GetLHS();
        if (lhs->GetMeOp() != kMeOpVar) {
          return false;
        }
        const auto *theLhs = static_cast<const VarMeExpr*>(lhs);
        if (theLhs->GetPrimType() != PTY_ref) {
          return false;
        }
        const OriginalSt *ost = ssaTab.GetOriginalStFromID(theLhs->GetOStIdx());
        if (!ost->IsLocal() || ost->IsFormal()) {
          return false;
        }
        if (verStCantDelegate[theLhs->GetVstIdx()]) {
          return false;
        }
        if (!verStDerefedCopied[theLhs->GetVstIdx()]) {
          // condition B1
          if (!verStCantDecrefEarly[theLhs->GetVstIdx()]) {
            onlyWithDecref = true;
            return true;
          }
        }
      }
      break;
  }
  return false;
}

void DelegateRC::DelegateHandleNoRCStmt(MeStmt &stmt, bool addDecref) {
  VarMeExpr *theLhs = nullptr;
  MeExpr *rhsExpr = stmt.GetRHS();
  if (CheckOp(stmt, OP_dassign) || CheckOp(stmt, OP_maydassign)) {
    theLhs = stmt.GetVarLHS();
  } else if (kOpcodeInfo.IsCallAssigned(stmt.GetOp()) && addDecref) {
    theLhs = static_cast<VarMeExpr*>(stmt.GetAssignedLHS());
  } else {
    return;
  }
  CHECK_FATAL(theLhs != nullptr, "null ptr check");
  if (theLhs->GetPrimType() != PTY_ref) {
    return;
  }

  BB &bb = *stmt.GetBB();
  // bool defstmt_need_incref;
  if (enabledDebug) {
    LogInfo::MapleLogger() << "delegaterc of form B for func " << func.GetName() << '\n';
    LogInfo::MapleLogger() << "\nreplace stmt :\n" << '\n';
    stmt.Dump(func.GetIRMap());
  }
  // replace temp by a new preg
  MeStmt *newStmt = &stmt;
  theLhs->SetDefBy(kDefByNo);
  RegMeExpr *curReg = irMap.CreateRegRefMeExpr(*theLhs);
  refVar2RegMap[theLhs] = curReg;  // record this replacement
  if (rhsExpr != nullptr) {
    // create new regassign statement
    MeStmt *regass = irMap.CreateRegassignMeStmt(*curReg, *rhsExpr, *stmt.GetBB());
    curReg->SetDefByStmt(*regass);
    bb.ReplaceMeStmt(newStmt, regass);
    newStmt = regass;  // for inserting defref after it below
  } else {
    // callassigned
    static_cast<CallMeStmt&>(stmt).SetCallReturn(*curReg);
  }
  if (enabledDebug) {
    LogInfo::MapleLogger() << "with stmt :\n" << '\n';
    newStmt->Dump(func.GetIRMap());
  }
  if (addDecref) {
    // We use RC intrinsic instead of direct function call,
    // so that mplcg can decide how to generate code for it.
    // for example: when GCONLY is enabled, decref will be omitted.
    std::vector<MeExpr*> opnds = { curReg };
    IntrinsiccallMeStmt *decRefIntrin = irMap.CreateIntrinsicCallMeStmt(INTRN_MCCDecRef, opnds);
    decRefIntrin->SetSrcPos(newStmt->GetSrcPosition());
    bb.InsertMeStmtAfter(newStmt, decRefIntrin);
  }
}

void DelegateRC::RenameDelegatedRefVarUses(MeStmt &meStmt, MeExpr &meExpr) {
  for (size_t i = 0; i < meExpr.GetNumOpnds(); ++i) {
    RenameDelegatedRefVarUses(meStmt, *meExpr.GetOpnd(i));
  }
  if (meExpr.GetMeOp() == kMeOpVar) {
    auto &varMeExpr = static_cast<VarMeExpr&>(meExpr);
    auto it = refVar2RegMap.find(&varMeExpr);
    if (it != refVar2RegMap.end()) {
      (void)irMap.ReplaceMeExprStmt(meStmt, varMeExpr, *it->second);
    }
  }
}

void DelegateRC::SetCantDelegateAndCountUses() {
  auto eIt = func.valid_end();
  for (auto bIt = func.valid_begin(); bIt != eIt; ++bIt) {
    auto &bb = **bIt;
    SetCantDelegate(bb.GetMevarPhiList());
    for (auto &stmt : bb.GetMeStmts()) {
      // do not count decref operands
      if (IsVarDecRefStmt(stmt)) {
        continue;
      }
      // do not count the cleanup intrinsic
      if (IsCleanUpStmt(stmt)) {
        continue;
      }
      for (size_t i = 0; i < stmt.NumMeStmtOpnds(); i++) {
        CHECK_FATAL(stmt.GetOpnd(i) != nullptr, "null mestmtopnd check");
        CollectUsesInfo(*stmt.GetOpnd(i));
      }
      CollectDerefedOrCopied(stmt);
    }
  }
}

void DelegateRC::DelegateStmtRC() {
  auto eIt = func.valid_end();
  for (auto bIt = func.valid_begin(); bIt != eIt; ++bIt) {
    auto &bb = **bIt;
    for (auto &stmt : bb.GetMeStmts()) {
      bool withDecref = false;
      if (CanOmitRC4LHSVar(stmt, withDecref)) {
        DelegateHandleNoRCStmt(stmt, withDecref);  // Form B
      } else {
        DelegateRCTemp(stmt);  // Form A
      }
    }
  }
}

std::set<OStIdx> DelegateRC::RenameAndGetLiveLocalRefVar() {
  std::set<OStIdx> liveLocalrefvars;
  auto eIt = func.valid_end();
  for (auto bIt = func.valid_begin(); bIt != eIt; ++bIt) {
    auto &bb = **bIt;
    for (auto &stmt : bb.GetMeStmts()) {
      if (IsVarDecRefStmt(stmt)) {
        continue;  // it is wrong to replace decref operand as it is intended for the original localrefvar
      }
      // no need process the cleanup intrinsic
      if (IsCleanUpStmt(stmt)) {
        continue;
      }
      for (size_t i = 0; i < stmt.NumMeStmtOpnds(); ++i) {
        CHECK_FATAL(stmt.GetOpnd(i) != nullptr, "null mestmtopnd check");
        RenameDelegatedRefVarUses(stmt, *stmt.GetOpnd(i));
      }
      // for live_localrefvars
      if (CheckOp(stmt, OP_dassign) || CheckOp(stmt, OP_maydassign)) {
        VarMeExpr *lhs = stmt.GetVarLHS();
        CHECK_FATAL(lhs != nullptr, "null ptr check");
        const OriginalSt *ost = ssaTab.GetOriginalStFromID(lhs->GetOStIdx());
        if (ost->IsLocal() && !ost->IsFormal() && !ost->IsIgnoreRC() && lhs->GetPrimType() == PTY_ref) {
          (void)liveLocalrefvars.insert(lhs->GetOStIdx());
        }
      } else if (kOpcodeInfo.IsCallAssigned(stmt.GetOp())) {
        MapleVector<MustDefMeNode> *mustdefList = stmt.GetMustDefList();
        CHECK_NULL_FATAL(mustdefList);
        if (mustdefList->empty()) {
          continue;
        }
        MeExpr *theLhs = mustdefList->front().GetLHS();
        if (theLhs->GetMeOp() == kMeOpVar && theLhs->GetPrimType() == PTY_ref) {
          auto *varLhs = static_cast<VarMeExpr*>(theLhs);
          const OriginalSt *ost = ssaTab.GetOriginalStFromID(varLhs->GetOStIdx());
          if (ost->IsLocal() && !ost->IsFormal() && !ost->IsIgnoreRC()) {
            (void)liveLocalrefvars.insert(varLhs->GetOStIdx());
          }
        }
      }
    }
  }
  return liveLocalrefvars;
}

void DelegateRC::CleanUpDeadLocalRefVar(const std::set<OStIdx> &liveLocalrefvars) {
  for (BB *bb : func.GetCommonExitBB()->GetPred()) {
    auto &meStmts = bb->GetMeStmts();
    if (meStmts.empty() || meStmts.back().GetOp() != OP_return) {
      continue;
    }
    MeStmt *stmt = meStmts.back().GetPrev();
    while (stmt != nullptr && stmt->GetOp() != OP_intrinsiccall) {
      stmt = stmt->GetPrev();
    }
    if (stmt == nullptr) {
      continue;
    }
    if (IsCleanUpStmt(*stmt)) {
      continue;
    }
    // delete the operands that are not live
    size_t nextPos = 0;
    IntrinsiccallMeStmt *intrin = static_cast<IntrinsiccallMeStmt*>(stmt);
    for (size_t i = 0; i < intrin->NumMeStmtOpnds(); ++i) {
      auto *varMeExpr = static_cast<VarMeExpr*>(intrin->GetOpnd(i));
      if (liveLocalrefvars.find(varMeExpr->GetOStIdx()) == liveLocalrefvars.end()) {
        continue;
      }
      if (nextPos != i) {
        intrin->SetOpnd(nextPos, varMeExpr);
      }
      ++nextPos;
    }
    intrin->EraseOpnds(intrin->GetOpnds().begin() + nextPos, intrin->GetOpnds().end());
  }
}

AnalysisResult *MeDoDelegateRC::Run(MeFunction *func, MeFuncResultMgr *m, ModuleResultMgr*) {
  static uint32 puCount = 0;
  auto *dom = static_cast<Dominance*>(m->GetAnalysisResult(MeFuncPhase_DOMINANCE, func));
  ASSERT(dom != nullptr, "dominance phase has problem");
  {
    // invoke hdse to update isLive only
    MeHDSE hdse(*func, *dom, *func->GetIRMap(), DEBUGFUNC(func));
    hdse.InvokeHDSEUpdateLive();
  }
  if (DEBUGFUNC(func)) {
    LogInfo::MapleLogger() << " Processing " << func->GetMirFunc()->GetName() << '\n';
  }
  DelegateRC delegaterc(*func, *dom, NewMemPool(), DEBUGFUNC(func));
  if (puCount > MeOption::delRcPULimit) {
    ++puCount;
    return nullptr;
  }
  if (puCount == MeOption::delRcPULimit) {
    LogInfo::MapleLogger() << func->GetMirFunc()->GetName()
                           << " is last PU optimized by delegaterc under -delrcpulimit option" << '\n';
  }
  // first pass
  delegaterc.SetCantDelegateAndCountUses();
  // main pass
  delegaterc.DelegateStmtRC();
  // final pass: rename the uses of the delegated ref pointer variable versions;
  // set live_localrefvars based on appearances on LHS
  // to detect dead localrefvars
  std::set<OStIdx> liveLocalRefVars = delegaterc.RenameAndGetLiveLocalRefVar();
  // postpass: go through the cleanup intrinsics to delete dead localrefvars
  delegaterc.CleanUpDeadLocalRefVar(liveLocalRefVars);
  if (DEBUGFUNC(func)) {
    LogInfo::MapleLogger() << "\n============== After DELEGATE RC =============" << '\n';
    func->GetIRMap()->Dump();
  }
  ++puCount;
  return nullptr;
}
}  // namespace maple
