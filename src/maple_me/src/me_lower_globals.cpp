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
#include "me_lower_globals.h"

// the dassign/dread of global or static variables are lowered into
// iassign/iread to expose the addrof.  The inserted addrof is always of fieldID
// 0 to reduce the number of registers needed for storing addresses after the
// addrof's are allocated to pregs.  The fieldID's thus reside in the iassign's
// and iread's.
//
// By the same token, an addrof with non-zero field id is also lowered into
// iaddrof with same field id whose operand is addrof with 0 field id.
//
// call and callassigned are lowered into icall/icallassigned and addroffunc.
// The addroffunc is inserted as the extra first operand and the
// call/callassinged is renamed to icall/icallassigned.
namespace maple {
void MeLowerGlobals::LowerGlobalDreads(MeStmt &stmt, MeExpr &expr) {
  switch (expr.GetMeOp()) {
    case kMeOpOp: {
      auto &meOpExpr = static_cast<OpMeExpr&>(expr);
      for (size_t i = 0; i < kOperandNumTernary; ++i) {
        MeExpr *opnd = meOpExpr.GetOpnd(i);
        if (opnd != nullptr) {
          LowerGlobalDreads(stmt, *opnd);
        }
      }
      break;
    }
    case kMeOpNary: {
      auto &naryMeExpr = static_cast<NaryMeExpr&>(expr);
      MapleVector<MeExpr*> &opnds = naryMeExpr.GetOpnds();
      for (auto it = opnds.begin(); it != opnds.end(); ++it) {
        LowerGlobalDreads(stmt, **it);
      }
      break;
    }
    case kMeOpIvar: {
      auto &ivarMeExpr = static_cast<IvarMeExpr&>(expr);
      LowerGlobalDreads(stmt, *ivarMeExpr.GetBase());
      break;
    }
    case kMeOpVar: {
      auto &varExpr = static_cast<VarMeExpr&>(expr);
      OriginalSt *ost = ssaTable->GetSymbolOriginalStFromID(varExpr.GetOStIdx());
      if (ost->IsLocal()) {
        break;
      }
      // lower to ivar to expose addrof
      OriginalSt *baseOst = ost;
      if (ost->GetFieldID() != 0) {
        PUIdx puIdx = func.GetMirFunc()->GetPuidx();
        baseOst = ssaTable->FindOrCreateSymbolOriginalSt(*ost->GetMIRSymbol(), puIdx, 0);
      }
      auto *addrofExpr = static_cast<AddrofMeExpr*>(irMap->CreateAddrofMeExpr(varExpr));
      MIRPtrType ptrType(baseOst->GetTyIdx(), PTY_ptr);
      if (ost->IsVolatile()) {
        TypeAttrs attrs;
        attrs.SetAttr(ATTR_volatile);
        ptrType.SetTypeAttrs(attrs);
      }
      TyIdx addrTyIdx = GlobalTables::GetTypeTable().GetOrCreateMIRType(&ptrType);
      auto *ivarExpr = static_cast<IvarMeExpr*>(irMap->CreateIvarMeExpr(varExpr, addrTyIdx, *addrofExpr));
      (void)irMap->ReplaceMeExprStmt(stmt, varExpr, *ivarExpr);
      break;
    }
    case kMeOpAddrof: {
      auto &addrofExpr = static_cast<AddrofMeExpr&>(expr);
      if (addrofExpr.GetFieldID() == 0) {
        break;
      }
      OriginalSt *ost = ssaTable->GetSymbolOriginalStFromID(addrofExpr.GetOstIdx());
      if (ost->IsLocal()) {
        break;
      }
      // lower to iaddrof to expose addrof with 0 fieldID
      PUIdx puIdx = func.GetMirFunc()->GetPuidx();
      OriginalSt *baseOst = ssaTable->FindOrCreateSymbolOriginalSt(*ost->GetMIRSymbol(),
          func.GetMirFunc()->GetPuidx(), 0);
      MeExpr *newAddrofExpr = irMap->CreateAddrofMeExprFromSymbol(*ost->GetMIRSymbol(), puIdx);
      MIRPtrType ptrType(baseOst->GetTyIdx(), PTY_ptr);
      TyIdx addrTyIdx = GlobalTables::GetTypeTable().GetOrCreateMIRType(&ptrType);
      ASSERT_NOT_NULL(irMap);
      MeExpr *iaddrofExpr = irMap->CreateIaddrofMeExpr(addrofExpr, addrTyIdx, *newAddrofExpr);
      (void)irMap->ReplaceMeExprStmt(stmt, addrofExpr, *iaddrofExpr);
      break;
    }
    default:
      break;
  }
}

void MeLowerGlobals::Run() {
  auto eIt = func.valid_end();
  for (auto bIt = func.valid_begin(); bIt != eIt; ++bIt) {
    auto *bb = *bIt;
    for (auto &stmt : bb->GetMeStmts()) {
      for (size_t i = 0; i < stmt.NumMeStmtOpnds(); ++i) {
        LowerGlobalDreads(stmt, *stmt.GetOpnd(i));
      }
      if (stmt.GetOp() == OP_dassign) {
        auto &dass = static_cast<DassignMeStmt&>(stmt);
        OriginalSt *ost = ssaTable->GetSymbolOriginalStFromID(dass.GetVarLHS()->GetOStIdx());
        if (ost->IsLocal()) {
          continue;
        }
        // lower to iassign to expose addrof
        OriginalSt *baseOst = ost;
        if (ost->GetFieldID() != 0) {
          baseOst = ssaTable->FindOrCreateSymbolOriginalSt(*ost->GetMIRSymbol(),
              func.GetMirFunc()->GetPuidx(), 0);
        }
        MeExpr *addrof = irMap->CreateAddrofMeExpr(baseOst->GetIndex());
        MIRPtrType ptrType(baseOst->GetTyIdx(), PTY_ptr);
        if (ost->IsVolatile()) {
          TypeAttrs attrs;
          attrs.SetAttr(ATTR_volatile);
          ptrType.SetTypeAttrs(attrs);
        }
        TyIdx addrTyIdx = GlobalTables::GetTypeTable().GetOrCreateMIRType(&ptrType);
        MeExpr *lhs = dass.GetLHS();
        CHECK_NULL_FATAL(lhs);
        auto *lhsIvar = static_cast<IvarMeExpr*>(irMap->CreateIvarMeExpr(*lhs, addrTyIdx, *addrof));
        IassignMeStmt *iass = irMap->NewInPool<IassignMeStmt>(addrTyIdx, lhsIvar, dass.GetRHS(), &*dass.GetChiList());
        iass->SetBB(bb);
        iass->SetSrcPos(dass.GetSrcPosition());
        iass->SetIsLive(true);
        bb->ReplaceMeStmt(&stmt, iass);
      } else if (stmt.GetOp() == OP_call || stmt.GetOp() == OP_callassigned) {
        // don't do this if current function is in libcore-all
        if (!MeOption::optDirectCall) {
          continue;
        }
        if (GlobalTables::GetGsymTable().GetSymbolFromStrIdx(GlobalTables::GetStrTable().GetStrIdxFromName(
          namemangler::GetInternalNameLiteral(namemangler::kJavaLangObjectStr))) != nullptr) {
          continue;
        }
        auto &callStmt = static_cast<CallMeStmt&>(stmt);
        MIRFunction &callee = callStmt.GetTargetFunction();
        if (!callee.IsJava() && callee.GetBaseClassName().empty()) {
          continue;  // not a java callee
        }
        MeExpr *addroffuncExpr = irMap->CreateAddroffuncMeExpr(callee.GetPuidx());
        auto insertpos = callStmt.GetOpnds().begin();
        callStmt.InsertOpnds(insertpos, addroffuncExpr);
        IcallMeStmt *icallStmt = irMap->NewInPool<IcallMeStmt>(stmt.GetOp() == OP_call ? OP_icall : OP_icallassigned);
        icallStmt->SetIsLive(callStmt.GetIsLive());
        icallStmt->SetSrcPos(callStmt.GetSrcPosition());
        for (MeExpr *o : callStmt.GetOpnds()) {
          icallStmt->PushBackOpnd(o);
        }
        icallStmt->GetMuList()->insert(callStmt.GetMuList()->begin(), callStmt.GetMuList()->end());
        icallStmt->GetChiList()->insert(callStmt.GetChiList()->begin(), callStmt.GetChiList()->end());
        icallStmt->SetRetTyIdx(callee.GetReturnTyIdx());
        if (stmt.GetOp() != OP_call) {
          if (callStmt.NeedDecref()) {
            icallStmt->EnableNeedDecref();
          } else {
            icallStmt->DisableNeedDecref();
          }
          if (callStmt.NeedIncref()) {
            icallStmt->EnableNeedIncref();
          } else {
            icallStmt->DisableNeedIncref();
          }
          icallStmt->GetMustDefList()->assign(callStmt.GetMustDefList()->begin(), callStmt.GetMustDefList()->end());
        }
        bb->ReplaceMeStmt(&stmt, icallStmt);
      }
    }
  }
}
}  // namespace maple
