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
#include "occur.h"
#include "ssa_pre.h"

// The methods associated with the data structures that represent occurrences
// and work candidates for SSAPRE
namespace {
constexpr uint32_t kOffsetMeExprID = 5;
constexpr uint32_t kOffsetVarMeExprOstIdx = 4;
constexpr uint32_t kOffsetRegMeExprRegIdx = 6;
constexpr uint32_t kOffsetIvarMeExprTyIdx = 3;
constexpr uint32_t kOffsetOpMeExprOpnd = 2;
constexpr uint32_t kOffsetNaryMeExprOpnd = 2;
constexpr uint32_t kOffsetMeStmtOpcode = 3;
constexpr uint32_t kOffsetUnaryMeStmtOpcode = 2;
constexpr uint32_t kOffsetIntrinsicCallMeStmtIntrinsic = 3;
constexpr uint32_t kOffsetNaryMeStmtOpnd = 2;
}

namespace maple {
void MeOccur::DumpOccur(IRMap &irMap) {
  MIRModule *mod = &irMap.GetSSATab().GetModule();
  mod->GetOut() << "MeOccur ";
  Dump(irMap);
  mod->GetOut() << "\n  class: " << classID << '\n';
  if (def != nullptr) {
    mod->GetOut() << "  def by class: " << def->GetClassID();
  } else {
    mod->GetOut() << "  no-def";
  }
  mod->GetOut() << '\n';
}

// return if this occur dominate occ
bool MeOccur::IsDominate(Dominance &dom, MeOccur &occ) {
  switch (occTy) {
    case kOccReal: {
      switch (occ.GetOccType()) {
        case kOccReal: {
          if (mirBB == occ.GetBB()) {
            auto *thisRealOcc = static_cast<MeRealOcc*>(this);
            auto *domOcc = static_cast<MeRealOcc*>(&occ);
            return thisRealOcc->GetSequence() <= domOcc->GetSequence();
          }
          return dom.Dominate(*mirBB, *occ.GetBB());
        }
        case kOccPhiocc: {
          if (mirBB == occ.GetBB()) {
            return false;
          }
          return dom.Dominate(*mirBB, *occ.GetBB());
        }
        case kOccPhiopnd:
        case kOccExit:
        case kOccMembar:
        case kOccUse:
          return dom.Dominate(*mirBB, *occ.GetBB());
        default:
          ASSERT(false, "should not be here");
          break;
      }
      break;
    }
    case kOccPhiocc:
    case kOccMembar:
    case kOccUse:
      return dom.Dominate(*mirBB, *occ.GetBB());
    default:
      ASSERT(false, "should not be here");
      return false;
  }

  return false;
}

MeExpr *MeOccur::GetSavedExpr() {
  switch (occTy) {
    case kOccReal:
      return static_cast<MeRealOcc*>(this)->GetSavedExpr();
    case kOccPhiocc: {
      auto *phiOcc = static_cast<MePhiOcc*>(this);
      MePhiNode *regPhi = phiOcc->GetRegPhi();
      if (regPhi != nullptr) {
        return regPhi->GetLHS();
      } else {
        return phiOcc->GetVarPhi()->GetLHS();
      }
    }
    case kOccInserted:
      return static_cast<MeInsertedOcc*>(this)->GetSavedExpr();
    default:
      CHECK_FATAL(false, "error or NYI");
  }
}

// return true if either:
// operand is nullptr (def is null), or
// hasRealUse is false and defined by a PHI not will be avail
bool MePhiOpndOcc::IsOkToInsert() const {
  if (GetDef() == nullptr) {
    return true;
  }
  if (!hasRealUse) {
    const MeOccur *defOcc = GetDef();
    if (defOcc->GetOccType() == kOccPhiocc && !static_cast<const MePhiOcc*>(defOcc)->IsWillBeAvail()) {
      return true;
    }
  }
  return false;
}

bool MePhiOcc::IsOpndDefByRealOrInserted() const {
  for (MePhiOpndOcc *phiOpnd : phiOpnds) {
    MeOccur *defOcc = phiOpnd->GetDef();
    if (defOcc->GetOccType() == kOccReal || defOcc->GetOccType() == kOccInserted) {
      return true;
    }
  }
  return false;
}

void MeOccur::Dump(const IRMap &irMap) const {
  MIRModule *mod = &irMap.GetSSATab().GetModule();
  if (occTy == kOccExit) {
    mod->GetOut() << "ExitOcc at bb" << GetBB()->GetBBId();
  } else if (occTy == kOccUse) {
    mod->GetOut() << "UseOcc at bb" << GetBB()->GetBBId();
  } else {
    CHECK_FATAL(false, "wrong occur type");
  }
}

void MeRealOcc::Dump(const IRMap &irMap) const {
  MIRModule *mod = &irMap.GetSSATab().GetModule();
  if (GetOccType() == kOccReal) {
    if (!isLHS) {
      mod->GetOut() << "RealOcc ";
    } else {
      mod->GetOut() << "RealOcc(LHS) ";
    }
    if (meExpr != nullptr) {
      meExpr->Dump(&irMap);
    } else {
      ASSERT_NOT_NULL(meStmt);
      meStmt->Dump(&irMap);
    }
    if (meStmt != nullptr && meStmt->GetBB()) {
      mod->GetOut() << " at bb" << meStmt->GetBB()->GetBBId() << " seq " << seq << " classID " << GetClassID();
    } else {
      mod->GetOut() << " classID " << GetClassID();
    }
  } else {
    mod->GetOut() << "MembarOcc ";
    mod->GetOut() << " at bb" << GetBB()->GetBBId() << " seq " << seq;
  }
}

void MePhiOcc::Dump(const IRMap &irMap) const {
  MIRModule *mod = &irMap.GetSSATab().GetModule();
  mod->GetOut() << "PhiOcc ";
  mod->GetOut() << "PHI(";
  size_t size = phiOpnds.size();
  for (size_t i = 0; i < size; i++) {
    phiOpnds[i]->Dump(irMap);
    if (i != size - 1) {
      mod->GetOut() << ",";
    }
  }
  mod->GetOut() << ")";
  mod->GetOut() << " at bb" << GetBB()->GetBBId() << " classID " << GetClassID();
}

void MePhiOpndOcc::Dump(const IRMap &irMap) const {
  MIRModule *mod = &irMap.GetSSATab().GetModule();
  mod->GetOut() << "PhiOpndOcc at bb" << GetBB()->GetBBId() << " classID " << GetClassID();
  if (hasRealUse) {
    mod->GetOut() << "(hasRealUse) ";
  }
}

void MeInsertedOcc::Dump(const IRMap &irMap) const {
  MIRModule *mod = &irMap.GetSSATab().GetModule();
  mod->GetOut() << "InsertedOcc at bb" << GetBB()->GetBBId() << " classID " << GetClassID();
}

// compute bucket index for the work candidate in workCandHashTable
uint32 PreWorkCandHashTable::ComputeWorkCandHashIndex(const MeExpr &meExpr) {
  uint32 hashIdx = 0;
  MeExprOp meOp = meExpr.GetMeOp();
  switch (meOp) {
    case kMeOpAddrof:
    case kMeOpAddroffunc:
    case kMeOpGcmalloc:
    case kMeOpConst:
    case kMeOpConststr:
    case kMeOpConststr16:
    case kMeOpSizeoftype:
    case kMeOpFieldsDist:
      hashIdx = (static_cast<uint32>(meExpr.GetExprID())) << kOffsetMeExprID;
      break;
    case kMeOpVar: {
      auto &varMeExpr = static_cast<const VarMeExpr&>(meExpr);
      hashIdx = static_cast<uint32_t>(varMeExpr.GetOStIdx()) << kOffsetVarMeExprOstIdx;
      break;
    }
    case kMeOpReg: {
      auto &regMeExpr = static_cast<const RegMeExpr&>(meExpr);
      hashIdx = (static_cast<uint32>(static_cast<PregIdx>(regMeExpr.GetRegIdx()))) << kOffsetRegMeExprRegIdx;
      break;
    }
    case kMeOpIvar: {
      auto &iVar = static_cast<const IvarMeExpr&>(meExpr);
      hashIdx = ComputeWorkCandHashIndex(*iVar.GetBase()) +
          (static_cast<uint32>(iVar.GetTyIdx()) << kOffsetIvarMeExprTyIdx) + iVar.GetFieldID();
      break;
    }
    case kMeOpOp: {
      hashIdx = static_cast<uint32>(meExpr.GetOp());
      for (size_t idx = 0; idx < kOperandNumTernary; ++idx) {
        MeExpr *opnd = meExpr.GetOpnd(idx);
        if (opnd == nullptr) {
          break;
        }
        hashIdx += ComputeWorkCandHashIndex(*opnd) << kOffsetOpMeExprOpnd;
      }
      break;
    }
    case kMeOpNary: {
      hashIdx = static_cast<uint32>(meExpr.GetOp());
      for (uint8 i = 0; i < meExpr.GetNumOpnds(); ++i) {
        hashIdx += ComputeWorkCandHashIndex(*meExpr.GetOpnd(i)) << kOffsetNaryMeExprOpnd;
      }
      break;
    }
    default:
      CHECK_FATAL(false, "MeOP NIY");
  }
  return hashIdx % workCandHashLength;
}

// insert occ as realOccs[pos] after shifting the vector elements further down
void PreWorkCand::InsertRealOccAt(MeRealOcc &occ, const MapleVector<MeRealOcc*>::iterator it, PUIdx pIdx) {
  ASSERT(pIdx != 0, "puIdx of realocc cannot be 0");
  if (pIdx != puIdx) {
    ASSERT(!hasLocalOpnd, "candidate with local opnd cannot have real occurrences in more than one PU");
    puIdx = 0;
  }
  CHECK(!realOccs.empty(), "realOccs has no element in PreWorkCand::InsertRealOccAt");
  realOccs.insert(it, &occ);
}

// insert occ in realOccs maintaining sorted order according to dt_preorder
void PreWorkCand::AddRealOccSorted(const Dominance &dom, MeRealOcc &occ, PUIdx pIdx) {
  ASSERT(!realOccs.empty(), "AddRealOccSorted: realOccs is empty");
  uint32 occDfn = dom.GetDtDfnItem(occ.GetBB()->GetBBId());
  // check the end of realOccs first because inserting at end is most frequent
  if (occDfn > dom.GetDtDfnItem(realOccs.back()->GetBB()->GetBBId())) {
    AddRealOccAsLast(occ, pIdx);
  } else if (occDfn == dom.GetDtDfnItem(realOccs.back()->GetBB()->GetBBId())) {
    if (occ.GetSequence() >= realOccs.back()->GetSequence()) {
      AddRealOccAsLast(occ, pIdx);
    } else {
      auto rIt = realOccs.rbegin();
      ++rIt;
      while (rIt != realOccs.rend()) {
        if (occDfn > dom.GetDtDfnItem((*rIt)->GetBB()->GetBBId())) {
          break;
        }
        if (occ.GetSequence() >= (*rIt)->GetSequence()) {
          break;
        }
        ++rIt;
      }
      InsertRealOccAt(occ, rIt.base(), pIdx);
    }
  } else {
    // search from beginning of realOccs
    auto it = realOccs.begin();
    bool found = false;
    do {
      if (occDfn > dom.GetDtDfnItem((*it)->GetBB()->GetBBId())) {
        ++it;
      } else if (occDfn == dom.GetDtDfnItem((*it)->GetBB()->GetBBId())) {
        if (occ.GetSequence() > (*it)->GetSequence()) {
          ++it;
        } else {
          found = true;
        }
      } else {
        found = true;
      }
    } while (!found && it != realOccs.end());
    if (!found) {
      AddRealOccAsLast(occ, pIdx);
    } else {
      InsertRealOccAt(occ, it, pIdx);
    }
  }
}

// compute bucket index for the work candidate in workCandHashTable
uint32 PreWorkCandHashTable::ComputeStmtWorkCandHashIndex(const MeStmt &stmt) {
  uint32 hIdx = (static_cast<uint32>(stmt.GetOp())) << kOffsetMeStmtOpcode;
  switch (stmt.GetOp()) {
    case OP_assertnonnull: {
      hIdx += ComputeWorkCandHashIndex(*stmt.GetOpnd(0)) << kOffsetUnaryMeStmtOpcode;
      break;
    }
    case OP_dassign: {
      CHECK_NULL_FATAL(stmt.GetVarLHS());
      CHECK_NULL_FATAL(stmt.GetRHS());
      VarMeExpr *varMeExpr = stmt.GetVarLHS();
      hIdx += static_cast<uint32_t>(varMeExpr->GetOStIdx()) << kOffsetVarMeExprOstIdx;
      hIdx += ComputeWorkCandHashIndex(*stmt.GetRHS()) << 1;
      break;
    }
    case OP_intrinsiccall:
    case OP_intrinsiccallwithtype: {
      auto &intrnStmt = static_cast<const IntrinsiccallMeStmt&>(stmt);
      hIdx += (static_cast<uint32>(intrnStmt.GetIntrinsic())) << kOffsetIntrinsicCallMeStmtIntrinsic;
      if (stmt.GetOp() == OP_intrinsiccallwithtype) {
        hIdx += static_cast<uint32>(intrnStmt.GetTyIdx()) << 1;
      }
      for (size_t i = 0; i < intrnStmt.NumMeStmtOpnds(); ++i) {
        hIdx += ComputeWorkCandHashIndex(*intrnStmt.GetOpnd(i)) << 1;
      }
      break;
    }
    case OP_callassigned: {
      auto &callAss = static_cast<const CallMeStmt&>(stmt);
      hIdx += callAss.GetPUIdx();
      for (size_t i = 0; i < callAss.NumMeStmtOpnds(); ++i) {
        hIdx += ComputeWorkCandHashIndex(*callAss.GetOpnd(i)) << kOffsetNaryMeStmtOpnd;
      }
      if (!callAss.GetMustDefList().empty()) {
        const MeExpr *lhs = callAss.GetMustDefList().front().GetLHS();
        auto *lhsVar = static_cast<const VarMeExpr*>(lhs);
        hIdx += static_cast<uint32>(lhsVar->GetOStIdx()) << 1;
      }
      break;
    }
    default:
      CHECK_FATAL(false, "ComputeStmtWorkCandHashIndex: NYI");
  }
  return hIdx % workCandHashLength;
}
}  // namespace maple
