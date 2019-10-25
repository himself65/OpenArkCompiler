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
#include "me_ir.h"
#include "opcode_info.h"
#include "irmap.h"
#include "printing.h"
#include "me_ssa.h"
#include "mir_function.h"

namespace maple {
constexpr int32_t kDefaultPrintIndentNum = 5;

bool MeExpr::IsTheSameWorkcand(const MeExpr &expr) const {
  ASSERT((exprID != -1 || expr.GetExprID() != -1), "at least one of them should not be none initialized");
  if (exprID == expr.GetExprID()) {
    return true;
  }
  if (op != expr.GetOp()) {
    return false;
  }
  if (IsPrimitiveFloat(primType) != IsPrimitiveFloat(expr.GetPrimType())) {
    return false;
  }
  if (GetPrimTypeSize(primType) != GetPrimTypeSize(expr.GetPrimType())) {
    return false;
  }
  if (op == OP_cvt && primType != expr.GetPrimType()) {
    // exclude cvt for different return type
    return false;
  }
  if (op == OP_sext &&
      static_cast<const OpMeExpr*>(this)->GetBitsSize() != static_cast<const OpMeExpr &>(expr).GetBitsSize()) {
    return false;
  }
  if (op == OP_resolveinterfacefunc || op == OP_resolvevirtualfunc) {
    if (static_cast<const OpMeExpr*>(this)->GetFieldID() != static_cast<const OpMeExpr &>(expr).GetFieldID()) {
      return false;
    }
  }
  return IsUseSameSymbol(expr);
}

void MeExpr::UpdateDepth() {
  uint8 maxDepth = 0;
  for (int32 i = 0; i < GetNumOpnds(); i++) {
    MeExpr *opnd = GetOpnd(i);
    if (opnd == nullptr) {
      continue;
    }
    uint8 curDepth = opnd->GetDepth();
    if (curDepth > maxDepth) {
      maxDepth = curDepth;
    }
  }
  ASSERT(maxDepth < UINT8_MAX, "runtime check error");
  depth = static_cast<uint8>(maxDepth + 1);
}

// get the definition of this
// for example:
// v2 = x + b
// v1 = v2
// then v1->ResolveMeExprValue() returns x+b
MeExpr *MeExpr::ResolveMeExprValue() {
  MeExpr *cmpOpLocal = this;
  while (cmpOpLocal != nullptr && cmpOpLocal->GetMeOp() == kMeOpVar) {
    auto *varCmpOp = static_cast<VarMeExpr*>(cmpOpLocal);
    cmpOpLocal = nullptr;
    if (varCmpOp->GetDefBy() == kDefByStmt) {
      cmpOpLocal = static_cast<DassignMeStmt*>(varCmpOp->GetDefStmt())->GetRHS();
    } else if (varCmpOp->GetDefBy() == kDefByChi) {
      ChiMeNode &defchi = varCmpOp->GetDefChi();
      MeStmt *base = defchi.GetBase();
      if (base->GetOp() == OP_maydassign) {
        cmpOpLocal = static_cast<MaydassignMeStmt*>(base)->GetRHS();
      }
    }
  }
  return cmpOpLocal;
}

bool VarMeExpr::IsSameVariableValue(const VarMeExpr &expr) const {
  if (&expr == this) {
    return true;
  }

  if (GetMeOp() == kMeOpVar && GetDefBy() == kDefByStmt && GetDefStmt()->GetOp() == OP_dassign) {
    auto *stmt = static_cast<DassignMeStmt*>(GetDefStmt());
    if (stmt->GetRHS() == &expr) {
      return true;
    }
  }

  return MeExpr::IsSameVariableValue(expr);
}

bool RegMeExpr::IsSameVariableValue(const VarMeExpr &expr) const {
  if (static_cast<const MeExpr *>(&expr) == this) {
    return true;
  }

  if (GetMeOp() == kMeOpReg &&GetDefBy() == kDefByStmt && GetDefStmt()->GetOp() == OP_regassign) {
    auto *stmt = static_cast<RegassignMeStmt*>(GetDefStmt());
    if (stmt->GetRHS() == &expr) {
      return true;
    }
  }

  return MeExpr::IsSameVariableValue(expr);
}

// get the definition VarMeExpr of this
// for expample:
// v2 = v3
// v1 = v2
// this = v1
// this->ResolveVarMeValue() returns v3;
// if no resolved VarMeExpr, return this
VarMeExpr &VarMeExpr::ResolveVarMeValue() {
  VarMeExpr *cmpop0 = this;
  while (true) {
    if (cmpop0->defBy != kDefByStmt) {
      break;
    }

    auto *defStmt = static_cast<DassignMeStmt*>(cmpop0->def.defStmt);
    MeExpr *expr = defStmt->GetRHS();
    if (expr->GetMeOp() != kMeOpVar) {
      break;
    }

    cmpop0 = static_cast<VarMeExpr*>(expr);
  }
  return *cmpop0;
}

// *this can be any node, but *v must be VarMeExpr
bool MeExpr::IsSameVariableValue(const VarMeExpr &expr) const {
  if (&expr == this) {
    return true;
  }
  // look up v's value
  if (expr.GetDefBy() == kDefByStmt && expr.GetDefStmt()->GetOp() == OP_dassign) {
    auto *stmt = static_cast<DassignMeStmt*>(expr.GetDefStmt());
    if (stmt->GetRHS() == this) {
      return true;
    }
  }
  return false;
}

// return true if the expression could throw exception; needs to be in sync with
// BaseNode::MayThrowException()
bool MeExpr::CouldThrowException() const {
  if (kOpcodeInfo.MayThrowException(op)) {
    if (op != OP_array) {
      return true;
    }
    if (static_cast<const NaryMeExpr*>(this)->GetBoundCheck()) {
      return true;
    }
  } else if (op == OP_intrinsicop) {
    if (static_cast<const NaryMeExpr*>(this)->GetIntrinsic() == INTRN_JAVA_ARRAY_LENGTH) {
      return true;
    }
  }
  for (int32 i = 0; i < GetNumOpnds(); i++) {
    if (GetOpnd(i)->CouldThrowException()) {
      return true;
    }
  }
  return false;
}

// search through the SSA graph to find a version with GetDefBy() == DefBYy_stmt
// visited is for avoiding processing a node more than once
RegMeExpr *RegMeExpr::FindDefByStmt(std::set<RegMeExpr*> &visited) {
  if (visited.find(this) != visited.end()) {
    return nullptr;
  }
  visited.insert(this);
  if (GetDefBy() == kDefByStmt) {
    return this;
  }
  if (GetDefBy() == kDefByPhi) {
    MeRegPhiNode &phireg = GetDefPhi();
    for (RegMeExpr *phiOpnd : phireg.GetOpnds()) {
      RegMeExpr *res = phiOpnd->FindDefByStmt(visited);
      if (res != nullptr) {
        return res;
      }
    }
  }
  return nullptr;
}

MeExpr &MeExpr::GetAddrExprBase() {
  switch (meOp) {
    case kMeOpAddrof:
    case kMeOpVar:
      return *this;
    case kMeOpOp:
      if (op == OP_add || op == OP_sub) {
        auto *opMeExpr = static_cast<OpMeExpr*>(this);
        return opMeExpr->GetOpnd(0)->GetAddrExprBase();
      }
      return *this;
    case kMeOpNary:
      if (op == OP_array) {
        auto *naryExpr = static_cast<NaryMeExpr*>(this);
        ASSERT(naryExpr->GetOpnd(0) != nullptr, "");
        return naryExpr->GetOpnd(0)->GetAddrExprBase();
      }
      return *this;
    case kMeOpReg: {
      auto *baseVar = static_cast<RegMeExpr*>(this);
      std::set<RegMeExpr*> visited;
      baseVar = baseVar->FindDefByStmt(visited);
      if (baseVar != nullptr && baseVar->GetDefBy() == kDefByStmt) {
        MeStmt *baseDefStmt = baseVar->GetDefStmt();
        auto *regAssign = static_cast<RegassignMeStmt*>(baseDefStmt);
        MeExpr *rhs = regAssign->GetRHS();
        // Following we only consider array, add and sub
        // Prevent the following situation for reg %1
        // regAssign ref %1 (gcmallocjarray ref ...)
        if (rhs->op == OP_array || rhs->op == OP_add || rhs->op == OP_sub) {
          return rhs->GetAddrExprBase();
        }
      }
      return *this;
    }
    default:
      return *this;
  }
}

bool NaryMeExpr::IsUseSameSymbol(const MeExpr &expr) const {
  if (expr.GetMeOp() != kMeOpNary) {
    return false;
  }
  auto &naryMeExpr = static_cast<const NaryMeExpr&>(expr);
  if (expr.GetOp() != GetOp() || naryMeExpr.GetIntrinsic() != intrinsic || naryMeExpr.tyIdx != tyIdx) {
    return false;
  }
  if (opnds.size() != naryMeExpr.GetOpnds().size()) {
    return false;
  }
  for (size_t i = 0; i < opnds.size(); i++) {
    if (!opnds[i]->IsUseSameSymbol(*naryMeExpr.GetOpnd(i))) {
      return false;
    }
  }
  return true;
}

bool MeExpr::IsAllOpndsIdentical(const MeExpr &meExpr) const {
  for (uint8 i = 0; i < GetNumOpnds(); i++) {
    if (GetOpnd(i)->GetExprID() != meExpr.GetOpnd(i)->GetExprID()) {
      return false;
    }
  }
  return true;
}

bool OpMeExpr::IsIdentical(const OpMeExpr &meExpr) const {
  if (meExpr.GetOp() != GetOp()) {
    return false;
  }
  if (meExpr.GetPrimType() != GetPrimType() || meExpr.opndType != opndType || meExpr.bitsOffset != bitsOffset ||
      meExpr.bitsSize != bitsSize || meExpr.tyIdx != tyIdx || meExpr.fieldID != fieldID) {
    return false;
  }

  return IsAllOpndsIdentical(meExpr);
}

bool NaryMeExpr::IsIdentical(NaryMeExpr &meExpr) const {
  if (meExpr.GetOp() != GetOp() || meExpr.tyIdx != tyIdx || meExpr.GetIntrinsic() != intrinsic ||
      meExpr.boundCheck != boundCheck) {
    return false;
  }
  if (meExpr.GetNumOpnds() != GetNumOpnds()) {
    return false;
  }

  return IsAllOpndsIdentical(meExpr);
}

bool IvarMeExpr::IsUseSameSymbol(const MeExpr &expr) const {
  if (expr.GetExprID() == GetExprID()) {
    return true;
  }
  if (expr.GetMeOp() != kMeOpIvar) {
    return false;
  }
  auto &ivarMeExpr = static_cast<const IvarMeExpr&>(expr);
  if (base->IsUseSameSymbol(*ivarMeExpr.base) && fieldID == ivarMeExpr.fieldID) {
    return true;
  }
  return false;
}

bool IvarMeExpr::IsVolatile() {
  MIRPtrType *ty = static_cast<MIRPtrType*>(GlobalTables::GetTypeTable().GetTypeFromTyIdx(tyIdx));
  MIRType *pointedType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(ty->GetPointedTyIdx());
  if (fieldID == 0) {
    return pointedType->HasVolatileField();
  }
  return static_cast<MIRStructType*>(pointedType)->IsFieldVolatile(fieldID);
}

bool IvarMeExpr::IsFinal() {
  MIRPtrType *ty = static_cast<MIRPtrType*>(GlobalTables::GetTypeTable().GetTypeFromTyIdx(tyIdx));
  MIRType *pointedTy = GlobalTables::GetTypeTable().GetTypeFromTyIdx(ty->GetPointedTyIdx());
  if (fieldID == 0) {
    return false;
  }
  return static_cast<MIRStructType*>(pointedTy)->IsFieldFinal(fieldID);
}

/*
 * check paragma
 *   pragma 0 var %keySet <$Ljava_2Flang_2Fannotation_2FRCWeakRef_3B>
 */
bool IvarMeExpr::IsRCWeak() {
  MIRPtrType *ty = static_cast<MIRPtrType*>(GlobalTables::GetTypeTable().GetTypeFromTyIdx(tyIdx));
  MIRType *pointedType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(ty->GetPointedTyIdx());
  if (pointedType->GetKind() == kTypeClass) {
    MIRClassType *classType = static_cast<MIRClassType*>(pointedType);
    return classType->IsFieldRCWeak(fieldID);
  }
  return false;
}

BB *VarMeExpr::GetDefByBBMeStmt(Dominance &dominance, MeStmtPtr &defMeStmt) {
  switch (defBy) {
    case kDefByNo:
      return &dominance.GetCommonEntryBB();
    case kDefByStmt:
      defMeStmt = def.defStmt;
      return defMeStmt->GetBB();
    case kDefByMustDef:
      defMeStmt = def.defMustDef->GetBase();
      return defMeStmt->GetBB();
    case kDefByChi:
      defMeStmt = def.defChi->GetBase();
      return defMeStmt->GetBB();
    case kDefByPhi:
      return def.defPhi->GetDefBB();
    default:
      return nullptr;
  }
}

bool VarMeExpr::IsUseSameSymbol(const MeExpr &expr) const {
  if (expr.GetMeOp() != kMeOpVar) {
    return false;
  }
  auto &varMeExpr = static_cast<const VarMeExpr&>(expr);
  return ostIdx == varMeExpr.ostIdx;
}

bool VarMeExpr::IsPureLocal(SSATab &tab, const MIRFunction &irFunc) const {
  const MIRSymbol *st = tab.GetMIRSymbolFromID(ostIdx);
  return st->IsLocal() && !irFunc.IsAFormal(st);
}

bool VarMeExpr::IsZeroVersion(SSATab &ssatab) const {
  ASSERT(vstIdx != 0, "VarMeExpr::IsZeroVersion: cannot determine because vstIdx is 0");
  const OriginalSt *ost = ssatab.GetOriginalStFromID(ostIdx);
  return ost->GetZeroVersionIndex() == vstIdx;
}

bool RegMeExpr::IsUseSameSymbol(const MeExpr &expr) const {
  if (expr.GetMeOp() != kMeOpReg) {
    return false;
  }
  auto &regMeExpr = static_cast<const RegMeExpr&>(expr);
  return ostIdx == regMeExpr.ostIdx;
}

BB *RegMeExpr::DefByBB() {
  switch (defBy) {
    case kDefByNo:
      return nullptr;
    case kDefByStmt:
      ASSERT(def.defStmt, "VarMeExpr::DefByBB: defStmt cannot be nullptr");
      return def.defStmt->GetBB();
    case kDefByPhi:
      ASSERT(def.defPhi, "VarMeExpr::DefByBB: defPhi cannot be nullptr");
      return def.defPhi->GetDefBB();
    case kDefByMustDef:
      ASSERT(def.defMustDef, "VarMeExpr::DefByBB: defMustDef cannot be nullptr");
      ASSERT(def.defMustDef->GetBase(), "VarMeExpr::DefByBB: defMustDef->base cannot be nullptr");
      return def.defMustDef->GetBase()->GetBB();
    default:
      ASSERT(false, "reg define unknown");
      return nullptr;
  }
}

bool AddrofMeExpr::IsUseSameSymbol(const MeExpr &expr) const {
  if (expr.GetMeOp() != kMeOpAddrof) {
    return false;
  }
  const auto &varMeExpr = static_cast<const AddrofMeExpr&>(expr);
  return ostIdx == varMeExpr.ostIdx;
}

bool OpMeExpr::IsUseSameSymbol(const MeExpr &expr) const {
  if (expr.GetOp() != GetOp()) {
    return false;
  }
  if (expr.GetMeOp() != kMeOpOp) {
    return false;
  }
  auto &opMeExpr = static_cast<const OpMeExpr&>(expr);
  for (uint32 i = 0; i < kOperandNumTernary; i++) {
    if (opnds[i]) {
      if (!opMeExpr.opnds[i]) {
        return false;
      }
      if (!opnds[i]->IsUseSameSymbol(*opMeExpr.opnds[i])) {
        return false;
      }
    } else {
      if (opMeExpr.opnds[i]) {
        return false;
      }
    }
  }
  return true;
}

// first, make sure it's int const and return true if the int const great or eq 0
bool ConstMeExpr::GeZero() const {
  return (GetIntValue() >= 0);
}

bool ConstMeExpr::GtZero() const {
  if (constVal->GetKind() != kConstInt) {
    return false;
  }
  return (static_cast<MIRIntConst*>(constVal)->GetValue() > 0);
}

bool ConstMeExpr::IsZero() const {
  return (GetIntValue() == 0);
}

bool ConstMeExpr::IsOne() const {
  if (constVal->GetKind() != kConstInt) {
    return false;
  }
  return (static_cast<MIRIntConst*>(constVal)->GetValue() == 1);
}

int64 ConstMeExpr::GetIntValue() const {
  CHECK_FATAL(constVal->GetKind() == kConstInt, "expect int const");
  return static_cast<MIRIntConst*>(constVal)->GetValue();
}

void MeVarPhiNode::Dump(IRMap *irMap) const {
  LogInfo::MapleLogger() << "VAR:";
  irMap->GetSSATab().GetOriginalStFromID(lhs->GetOStIdx())->Dump();
  LogInfo::MapleLogger() << " mx" << lhs->GetExprID();
  LogInfo::MapleLogger() << " = MEPHI{";
  for (size_t i = 0; i < opnds.size(); i++) {
    LogInfo::MapleLogger() << "mx" << opnds[i]->GetExprID();
    if (i != opnds.size() - 1) {
      LogInfo::MapleLogger() << ",";
    }
  }
  LogInfo::MapleLogger() << "}";
  if (!isLive) {
    LogInfo::MapleLogger() << " dead";
  }
  LogInfo::MapleLogger() << '\n';
}

void MeRegPhiNode::Dump(IRMap *irMap) const {
  LogInfo::MapleLogger() << "REGVAR: " << lhs->GetRegIdx();
  LogInfo::MapleLogger() << "(%"
                         << irMap->GetMIRModule().CurFunction()
                                                 ->GetPregTab()
                                                 ->PregFromPregIdx(static_cast<PregIdx>(lhs->GetRegIdx()))
                                                 ->GetPregNo()
                         << ")";
  LogInfo::MapleLogger() << " mx" << lhs->GetExprID();
  LogInfo::MapleLogger() << " = MEPHI{";
  for (size_t i = 0; i < opnds.size(); i++) {
    LogInfo::MapleLogger() << "mx" << opnds[i]->GetExprID();
    if (i != opnds.size() - 1) {
      LogInfo::MapleLogger() << ",";
    }
  }
  LogInfo::MapleLogger() << "}" << '\n';
}

void VarMeExpr::Dump(IRMap *irMap, int32 indent) const {
  LogInfo::MapleLogger() << "VAR ";
  irMap->GetSSATab().GetOriginalStFromID(ostIdx)->Dump();
  LogInfo::MapleLogger() << " (field)" << fieldID;
  LogInfo::MapleLogger() << " mx" << GetExprID();
  if (IsZeroVersion(irMap->GetSSATab())) {
    LogInfo::MapleLogger() << "<Z>";
  }
}

void RegMeExpr::Dump(IRMap *irMap, int32 indent) const {
  LogInfo::MapleLogger() << "REGINDX:" << regIdx;
  LogInfo::MapleLogger()
      << " %"
      << irMap->GetMIRModule().CurFunction()->GetPregTab()->PregFromPregIdx(static_cast<PregIdx>(regIdx))->GetPregNo();
  LogInfo::MapleLogger() << " mx" << GetExprID();
}

void AddroffuncMeExpr::Dump(IRMap *irMap, int32 indent) const {
  LogInfo::MapleLogger() << "ADDROFFUNC:";
  LogInfo::MapleLogger() << GlobalTables::GetFunctionTable().GetFunctionFromPuidx(puIdx)->GetName();
  LogInfo::MapleLogger() << " mx" << GetExprID();
}

void GcmallocMeExpr::Dump(IRMap *irMap, int32 indent) const {
  LogInfo::MapleLogger() << kOpcodeInfo.GetTableItemAt(GetOp()).name << " " << GetPrimTypeName(GetPrimType());
  LogInfo::MapleLogger() << " ";
  GlobalTables::GetTypeTable().GetTypeFromTyIdx(tyIdx)->Dump(0);
  LogInfo::MapleLogger() << " mx" << GetExprID();
  LogInfo::MapleLogger() << " ";
}

void ConstMeExpr::Dump(IRMap *irMap, int32 indent) const {
  LogInfo::MapleLogger() << "CONST";
  LogInfo::MapleLogger() << " ";
  constVal->Dump();
  LogInfo::MapleLogger() << " mx" << GetExprID();
}

void ConststrMeExpr::Dump(IRMap *irMap, int32 indent) const {
  LogInfo::MapleLogger() << "CONSTSTR";
  LogInfo::MapleLogger() << " ";
  LogInfo::MapleLogger() << strIdx.GetIdx();
  LogInfo::MapleLogger() << " mx" << GetExprID();
}

void Conststr16MeExpr::Dump(IRMap *irMap, int32 indent) const {
  LogInfo::MapleLogger() << "CONSTSTR16";
  LogInfo::MapleLogger() << " ";
  LogInfo::MapleLogger() << strIdx.GetIdx();
  LogInfo::MapleLogger() << " mx" << GetExprID();
}

void SizeoftypeMeExpr::Dump(IRMap *irMap, int32 indent) const {
  LogInfo::MapleLogger() << kOpcodeInfo.GetTableItemAt(GetOp()).name << " " << GetPrimTypeName(GetPrimType());
  LogInfo::MapleLogger() << " TYIDX:" << tyIdx.GetIdx();
  MIRType *mirType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(tyIdx);
  mirType->Dump(0);
  LogInfo::MapleLogger() << " mx" << GetExprID();
}

void FieldsDistMeExpr::Dump(IRMap *irMap, int32 indent) const {
  LogInfo::MapleLogger() << kOpcodeInfo.GetTableItemAt(GetOp()).name << " " << GetPrimTypeName(GetPrimType());
  LogInfo::MapleLogger() << " TYIDX:" << tyIdx.GetIdx();
  MIRType *mirType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(tyIdx);
  mirType->Dump(0);
  LogInfo::MapleLogger() << " (field)" << fieldID1;
  LogInfo::MapleLogger() << " (field)" << fieldID2;
  LogInfo::MapleLogger() << " mx" << GetExprID();
}

void AddrofMeExpr::Dump(IRMap *irMap, int32 indent) const {
  LogInfo::MapleLogger() << "ADDROF:";
  irMap->GetSSATab().GetOriginalStFromID(ostIdx)->Dump();
  LogInfo::MapleLogger() << " (field)" << fieldID;
  LogInfo::MapleLogger() << " mx" << GetExprID();
}

void OpMeExpr::Dump(IRMap *irMap, int32 indent) const {
  LogInfo::MapleLogger() << "OP " << kOpcodeInfo.GetTableItemAt(GetOp()).name;
  LogInfo::MapleLogger() << " mx" << GetExprID();
  LogInfo::MapleLogger() << '\n';
  ASSERT(opnds[0] != nullptr, "OpMeExpr::Dump: cannot have 0 operand");
  PrintIndentation(indent + 1);
  LogInfo::MapleLogger() << "opnd[0] = ";
  opnds[0]->Dump(irMap, indent + 1);
  if (opnds[1]) {
    LogInfo::MapleLogger() << '\n';
  } else {
    return;
  }
  PrintIndentation(indent + 1);
  LogInfo::MapleLogger() << "opnd[1] = ";
  opnds[1]->Dump(irMap, indent + 1);
  if (opnds[2]) {
    LogInfo::MapleLogger() << '\n';
  } else {
    return;
  }
  PrintIndentation(indent + 1);
  LogInfo::MapleLogger() << "opnd[2] = ";
  opnds[2]->Dump(irMap, indent + 1);
}

void IvarMeExpr::Dump(IRMap *irMap, int32 indent) const {
  LogInfo::MapleLogger() << "IVAR mx" << GetExprID();
  LogInfo::MapleLogger() << " " << GetPrimTypeName(GetPrimType());
  LogInfo::MapleLogger() << " TYIDX:" << tyIdx.GetIdx();
  MIRType *mirType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(tyIdx);
  mirType->Dump(0);
  LogInfo::MapleLogger() << " (field)" << fieldID << '\n';
  PrintIndentation(indent + 1);
  LogInfo::MapleLogger() << "base = ";
  base->Dump(irMap, indent + 1);
  LogInfo::MapleLogger() << '\n';
  PrintIndentation(indent + 1);
  LogInfo::MapleLogger() << "- MU: {";
  if (mu != nullptr) {
    mu->Dump(irMap);
  }
  LogInfo::MapleLogger() << "}";
}

void NaryMeExpr::Dump(IRMap *irMap, int32 indent) const {
  ASSERT(static_cast<size_t>(GetNumOpnds()) == opnds.size(), "array size error");
  if (GetOp() == OP_array) {
    LogInfo::MapleLogger() << "ARRAY ";
  } else if (GetOp() == OP_intrinsicop) {
    LogInfo::MapleLogger() << GetIntrinsicName(intrinsic);
  } else {
    ASSERT(GetOp() == OP_intrinsicopwithtype, "NaryMeExpr has bad GetOp()code");
    LogInfo::MapleLogger() << "INTRINOPWTY[" << intrinsic << "]";
  }
  LogInfo::MapleLogger() << " mx" << GetExprID() << '\n';
  for (int32 i = 0; i < GetNumOpnds(); i++) {
    PrintIndentation(indent + 1);
    LogInfo::MapleLogger() << "opnd[" << i << "] = ";
    opnds[i]->Dump(irMap, indent + 1);
    if (i != GetNumOpnds() - 1) {
      LogInfo::MapleLogger() << '\n';
    }
  }
}

MeExpr *DassignMeStmt::GetLHSRef(SSATab &ssatab, bool excludelocalrefvar) {
  VarMeExpr *l = GetVarLHS();
  if (l->GetPrimType() != PTY_ref) {
    return nullptr;
  }
  const OriginalSt *ost = ssatab.GetOriginalStFromID(lhs->GetOStIdx());
  if (ost->IsIgnoreRC()) {
    return nullptr;
  }
  if (excludelocalrefvar && ost->IsLocal()) {
    return nullptr;
  }
  return l;
}

MeExpr *MaydassignMeStmt::GetLHSRef(SSATab &ssatab, bool excludelocalrefvar) {
  VarMeExpr *l = GetVarLHS();
  if (l->GetPrimType() != PTY_ref) {
    return nullptr;
  }
  const OriginalSt *ost = ssatab.GetOriginalStFromID(l->GetOStIdx());
  if (ost->IsIgnoreRC()) {
    return nullptr;
  }
  if (excludelocalrefvar && ost->IsLocal()) {
    return nullptr;
  }
  return l;
}

MeExpr *IassignMeStmt::GetLHSRef(SSATab &ssatab, bool excludelocalrefvar) {
  MIRType *baseType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(lhsVar->GetTyIdx());
  ASSERT(baseType != nullptr, "null ptr check");
  MIRType *ptype = static_cast<MIRPtrType*>(baseType)->GetPointedType();
  MIRStructType *structType = dynamic_cast<MIRStructType*>(ptype);
  if (structType == nullptr) {
    if (ptype->GetKind() == kTypePointer) {
      if (lhsVar->GetFieldID() == 0) {
        if (static_cast<MIRPtrType*>(ptype)->GetPrimType() != PTY_ref) {
          return nullptr;
        }
      } else {
        MIRType *pptype = static_cast<MIRPtrType*>(ptype)->GetPointedType();
        TyIdx ftyidx = static_cast<MIRStructType*>(pptype)->GetFieldTyIdx(lhsVar->GetFieldID());
        if (GlobalTables::GetTypeTable().GetTypeFromTyIdx(ftyidx)->GetPrimType() != PTY_ref) {
          return nullptr;
        }
      }
    } else if (ptype->GetKind() == kTypeJArray) {
      MIRType *pptype = static_cast<MIRPtrType*>(ptype)->GetPointedType();
      if (static_cast<MIRPtrType*>(pptype)->GetPrimType() != PTY_ref) {
        return nullptr;
      }
    } else {
      return nullptr;
    }
  } else {
    if (lhsVar->GetFieldID() == 0) {
      return nullptr;  // struct assign is not ref
    }
    if (structType->GetFieldType(lhsVar->GetFieldID())->GetPrimType() != PTY_ref) {
      return nullptr;
    }
  }
  return lhsVar;
}

VarMeExpr *AssignedPart::GetAssignedPartLHSRef(SSATab &ssatab, bool excludelocalrefvar) {
  if (mustDefList.empty()) {
    return nullptr;
  }
  MeExpr *assignedLHS = mustDefList.front().GetLHS();
  if (assignedLHS->GetMeOp() != kMeOpVar) {
    return nullptr;
  }
  auto *theLHS = static_cast<VarMeExpr*>(assignedLHS);
  if (theLHS->GetPrimType() != PTY_ref) {
    return nullptr;
  }
  const OriginalSt *ost = ssatab.GetOriginalStFromID(theLHS->GetOStIdx());
  if (ost->IsIgnoreRC()) {
    return nullptr;
  }
  if (excludelocalrefvar && ost->IsLocal()) {
    return nullptr;
  }
  return theLHS;
}

// default Dump
void MeStmt::Dump(IRMap *irMap) const {
  if (op == OP_comment) {
    return;
  }
  irMap->GetMIRModule().GetOut() << "||MEIR|| " << kOpcodeInfo.GetTableItemAt(op).name << '\n';
}

// get the real next mestmt that is not a comment
MeStmt *MeStmt::GetNextMeStmt() {
  MeStmt *nextMeStmt = next;
  while (nextMeStmt != nullptr && nextMeStmt->op == OP_comment) {
    nextMeStmt = nextMeStmt->next;
  }
  return nextMeStmt;
}

void DassignMeStmt::Dump(IRMap *irMap) const {
  LogInfo::MapleLogger() << "||MEIR|| " << kOpcodeInfo.GetTableItemAt(GetOp()).name << " ";
  lhs->Dump(irMap);
  LogInfo::MapleLogger() << '\n';
  PrintIndentation(kDefaultPrintIndentNum);
  LogInfo::MapleLogger() << "rhs = ";
  rhs->Dump(irMap, kDefaultPrintIndentNum);
  if (needIncref) {
    LogInfo::MapleLogger() << " [RC+]";
  }
  if (needDecref) {
    LogInfo::MapleLogger() << " [RC-]";
  }
  LogInfo::MapleLogger() << '\n';
  DumpChiList(irMap, chiList);
}

void RegassignMeStmt::Dump(IRMap *irMap) const {
  LogInfo::MapleLogger() << "||MEIR|| " << kOpcodeInfo.GetTableItemAt(GetOp()).name << " ";
  lhs->Dump(irMap);
  LogInfo::MapleLogger() << '\n';
  PrintIndentation(kDefaultPrintIndentNum);
  LogInfo::MapleLogger() << "rhs = ";
  rhs->Dump(irMap, kDefaultPrintIndentNum);
  if (needIncref) {
    LogInfo::MapleLogger() << " [RC+]";
  }
  LogInfo::MapleLogger() << '\n';
}

void MaydassignMeStmt::Dump(IRMap *irMap) const {
  LogInfo::MapleLogger() << "||MEIR|| " << kOpcodeInfo.GetTableItemAt(GetOp()).name << '\n';
  PrintIndentation(kDefaultPrintIndentNum);
  LogInfo::MapleLogger() << "rhs = ";
  rhs->Dump(irMap, kDefaultPrintIndentNum);
  if (needIncref) {
    LogInfo::MapleLogger() << " [RC+]";
  }
  if (needDecref) {
    LogInfo::MapleLogger() << " [RC-]";
  }
  LogInfo::MapleLogger() << '\n';
  DumpChiList(irMap, chiList);
}

void ChiMeNode::Dump(IRMap *irMap) const {
  auto *melhs = static_cast<VarMeExpr*>(lhs);
  auto *merhs = static_cast<VarMeExpr*>(rhs);
  CHECK_FATAL(melhs != nullptr, "Node doesn't have lhs?");
  CHECK_FATAL(merhs != nullptr, "Node doesn't have rhs?");
  if (!DumpOptions::GetSimpleDump()) {
    LogInfo::MapleLogger() << "VAR:";
    irMap->GetSSATab().GetOriginalStFromID(melhs->GetOStIdx())->Dump();
  }
  LogInfo::MapleLogger() << " mx" << melhs->GetExprID() << " = CHI{";
  LogInfo::MapleLogger() << "mx" << merhs->GetExprID() << "}";
}

void DumpMuList(IRMap *irMap, const MapleMap<OStIdx, VarMeExpr*> &muList, int32 indent) {
  if (muList.empty()) {
    return;
  }
  int count = 0;
  LogInfo::MapleLogger() << "---- MULIST: { ";
  for (auto it = muList.begin();;) {
    if (!DumpOptions::GetSimpleDump()) {
      (*it).second->Dump(irMap);
    } else {
      LogInfo::MapleLogger() << "mx" << (*it).second->GetExprID();
    }
    it++;
    if (it == muList.end()) {
      break;
    } else {
      LogInfo::MapleLogger() << ", ";
    }
    if (DumpOptions::GetDumpVsyNum() > 0 && ++count >= DumpOptions::GetDumpVsyNum()) {
      LogInfo::MapleLogger() << " ... ";
      break;
    }
  }
  LogInfo::MapleLogger() << " }\n";
}

void DumpChiList(IRMap *irMap, const MapleMap<OStIdx, ChiMeNode*> &chiList) {
  if (chiList.empty()) {
    return;
  }
  int count = 0;
  LogInfo::MapleLogger() << "---- CHILIST: { ";
  for (auto it = chiList.begin();;) {
    it->second->Dump(irMap);
    it++;
    if (it == chiList.end()) {
      break;
    } else {
      LogInfo::MapleLogger() << ", ";
    }
    if (DumpOptions::GetDumpVsyNum() > 0 && count++ >= DumpOptions::GetDumpVsyNum()) {
      LogInfo::MapleLogger() << " ... ";
      break;
    }
  }
  LogInfo::MapleLogger() << " }\n";
}

void IassignMeStmt::Dump(IRMap *irMap) const {
  LogInfo::MapleLogger() << "||MEIR|| " << kOpcodeInfo.GetTableItemAt(GetOp()).name << '\n';
  PrintIndentation(kDefaultPrintIndentNum);
  LogInfo::MapleLogger() << "lhs = ";
  lhsVar->Dump(irMap, kDefaultPrintIndentNum);
  LogInfo::MapleLogger() << '\n';
  PrintIndentation(kDefaultPrintIndentNum);
  LogInfo::MapleLogger() << "rhs = ";
  rhs->Dump(irMap, kDefaultPrintIndentNum);
  if (needIncref) {
    LogInfo::MapleLogger() << " [RC+]";
  }
  if (needDecref) {
    LogInfo::MapleLogger() << " [RC-]";
  }
  LogInfo::MapleLogger() << '\n';
  DumpChiList(irMap, chiList);
}

void NaryMeStmt::DumpOpnds(IRMap *irMap) const {
  for (size_t i = 0; i < opnds.size(); i++) {
    PrintIndentation(kDefaultPrintIndentNum);
    LogInfo::MapleLogger() << "opnd[" << i << "] = ";
    opnds[i]->Dump(irMap, kDefaultPrintIndentNum);
    LogInfo::MapleLogger() << '\n';
  }
}

void NaryMeStmt::Dump(IRMap *irMap) const {
  LogInfo::MapleLogger() << "||MEIR|| " << kOpcodeInfo.GetTableItemAt(GetOp()).name << " ";
  DumpOpnds(irMap);
}

void AssignedPart::DumpAssignedPart(IRMap *irMap) const {
  LogInfo::MapleLogger() << "    assignedpart: {";
  for (auto it = mustDefList.begin(); it != mustDefList.end(); it++) {
    const MeExpr *lhsvar = (*it).GetLHS();
    lhsvar->Dump(irMap);
  }
  if (needIncref) {
    LogInfo::MapleLogger() << " [RC+]";
  }
  if (needDecref) {
    LogInfo::MapleLogger() << " [RC-]";
  }
  LogInfo::MapleLogger() << "}\n";
}

void CallMeStmt::Dump(IRMap *irMap) const {
  LogInfo::MapleLogger() << "||MEIR|| " << kOpcodeInfo.GetTableItemAt(GetOp()).name << " ";
  if (tyIdx != 0) {
    LogInfo::MapleLogger() << " TYIDX:" << tyIdx.GetIdx();
    MIRType *mirType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(tyIdx);
    mirType->Dump(0);
  }
  // target function name
  MIRFunction *func = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(puIdx);
  LogInfo::MapleLogger() << NameMangler::DecodeName(func->GetName()) << '\n';
  DumpOpnds(irMap);
  DumpMuList(irMap, muList, 0);
  DumpChiList(irMap, chiList);
  DumpAssignedPart(irMap);
}

void IcallMeStmt::Dump(IRMap *irMap) const {
  LogInfo::MapleLogger() << "||MEIR|| " << kOpcodeInfo.GetTableItemAt(GetOp()).name << " ";
  LogInfo::MapleLogger() << " TYIDX:" << retTyIdx.GetIdx();
  DumpOpnds(irMap);
  DumpMuList(irMap, muList, 0);
  DumpChiList(irMap, chiList);
  DumpAssignedPart(irMap);
}

void IntrinsiccallMeStmt::Dump(IRMap *irMap) const {
  LogInfo::MapleLogger() << "||MEIR|| " << kOpcodeInfo.GetTableItemAt(GetOp()).name << " ";
  LogInfo::MapleLogger() << "TYIDX:" << tyIdx.GetIdx();
  MIRType *mirType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(tyIdx);
  if (mirType != nullptr) {
    mirType->Dump(0);
  }
  LogInfo::MapleLogger() << GetIntrinsicName(intrinsic) << '\n';
  DumpOpnds(irMap);
  DumpMuList(irMap, muList, 0);
  DumpChiList(irMap, chiList);
  DumpAssignedPart(irMap);
}

void RetMeStmt::Dump(IRMap *irMap) const {
  LogInfo::MapleLogger() << "||MEIR|| " << kOpcodeInfo.GetTableItemAt(GetOp()).name << '\n';
  DumpOpnds(irMap);
  DumpMuList(irMap, muList, 1);
}

void CondGotoMeStmt::Dump(IRMap *irMap) const {
  LogInfo::MapleLogger() << "||MEIR|| " << kOpcodeInfo.GetTableItemAt(GetOp()).name << '\n';
  PrintIndentation(kDefaultPrintIndentNum);
  LogInfo::MapleLogger() << "cond: ";
  GetOpnd()->Dump(irMap, kDefaultPrintIndentNum);
  LogInfo::MapleLogger() << '\n';
}

void UnaryMeStmt::Dump(IRMap *irMap) const {
  LogInfo::MapleLogger() << "||MEIR|| " << kOpcodeInfo.GetTableItemAt(GetOp()).name << '\n';
  PrintIndentation(kDefaultPrintIndentNum);
  LogInfo::MapleLogger() << " unaryopnd: ";
  opnd->Dump(irMap, kDefaultPrintIndentNum);
  LogInfo::MapleLogger() << '\n';
}

void SwitchMeStmt::Dump(IRMap *irMap) const {
  LogInfo::MapleLogger() << "||MEIR|| " << kOpcodeInfo.GetTableItemAt(GetOp()).name << '\n';
  PrintIndentation(kDefaultPrintIndentNum);
  LogInfo::MapleLogger() << "switchOpnd: ";
  GetOpnd()->Dump(irMap, kDefaultPrintIndentNum);
  LogInfo::MapleLogger() << '\n';
}

void GosubMeStmt::Dump(IRMap *irMap) const {
  LogInfo::MapleLogger() << "||MEIR|| " << kOpcodeInfo.GetTableItemAt(GetOp()).name << '\n';
  DumpMuList(irMap, *GetMuList(), 0);
  LogInfo::MapleLogger() << '\n';
}

void ThrowMeStmt::Dump(IRMap *irMap) const {
  LogInfo::MapleLogger() << "||MEIR|| " << kOpcodeInfo.GetTableItemAt(GetOp()).name << '\n';
  PrintIndentation(kDefaultPrintIndentNum);
  LogInfo::MapleLogger() << "throwopnd: ";
  opnd->Dump(irMap, kDefaultPrintIndentNum);
  LogInfo::MapleLogger() << '\n';
  DumpMuList(irMap, *GetMuList(), 0);
}

void SyncMeStmt::Dump(IRMap *irMap) const {
  LogInfo::MapleLogger() << "||MEIR|| " << kOpcodeInfo.GetTableItemAt(GetOp()).name << '\n';
  DumpOpnds(irMap);
  DumpMuList(irMap, muList, 0);
  DumpChiList(irMap, chiList);
}

bool MeStmt::IsTheSameWorkcand(MeStmt &mestmt) const {
  if (op != mestmt.op) {
    return false;
  }
  if (op == OP_dassign) {
    if (this->GetVarLHS()->GetOStIdx() != mestmt.GetVarLHS()->GetOStIdx()) {
      return false;
    }
  } else if (op == OP_intrinsiccallwithtype) {
    if (static_cast<const IntrinsiccallMeStmt*>(this)->GetTyIdx() !=
        static_cast<IntrinsiccallMeStmt &>(mestmt).GetTyIdx()) {
      return false;
    }
    if (static_cast<const IntrinsiccallMeStmt*>(this)->GetIntrinsic() !=
        static_cast<IntrinsiccallMeStmt &>(mestmt).GetIntrinsic()) {
      return false;
    }
  } else if (op == OP_callassigned) {
    auto *thisCass = static_cast<const CallMeStmt*>(this);
    auto &cass = static_cast<CallMeStmt &>(mestmt);
    if (thisCass->GetPUIdx() != cass.GetPUIdx()) {
      return false;
    }
    if (thisCass->MustDefListSize() != cass.MustDefListSize()) {
      return false;
    }
    if (thisCass->MustDefListSize() > 0) {
      auto *thisVarMeExpr = static_cast<const VarMeExpr*>(thisCass->GetAssignedLHS());
      auto *varMeExpr = static_cast<const VarMeExpr*>(cass.GetAssignedLHS());
      if (thisVarMeExpr->GetOStIdx() != varMeExpr->GetOStIdx()) {
        return false;
      }
    }
  }
  // check the operands
  for (size_t i = 0; i < NumMeStmtOpnds(); ++i) {
    ASSERT(GetOpnd(i) != nullptr, "null ptr check");
    if (!GetOpnd(i)->IsUseSameSymbol(*mestmt.GetOpnd(i))) {
      return false;
    }
  }
  return true;
}

void AssertMeStmt::Dump(IRMap *irMap) const {
  LogInfo::MapleLogger() << "||MEIR|| " << kOpcodeInfo.GetTableItemAt(GetOp()).name << '\n';
  PrintIndentation(kDefaultPrintIndentNum);
  LogInfo::MapleLogger() << "opnd[0] = ";
  opnds[0]->Dump(irMap, kDefaultPrintIndentNum);
  LogInfo::MapleLogger() << '\n';
  PrintIndentation(kDefaultPrintIndentNum);
  LogInfo::MapleLogger() << "opnd[1] = ";
  opnds[1]->Dump(irMap, kDefaultPrintIndentNum);
  LogInfo::MapleLogger() << '\n';
}

BB *VarMeExpr::DefByBB() {
  switch (defBy) {
    case kDefByNo:
      return nullptr;
    case kDefByStmt:
      ASSERT(def.defStmt, "VarMeExpr::DefByBB: defStmt cannot be nullptr");
      return def.defStmt->GetBB();
    case kDefByPhi:
      ASSERT(def.defPhi, "VarMeExpr::DefByBB: defPhi cannot be nullptr");
      return def.defPhi->GetDefBB();
    case kDefByChi: {
      ASSERT(def.defChi, "VarMeExpr::DefByBB: defChi cannot be nullptr");
      ASSERT(def.defChi->GetBase(), "VarMeExpr::DefByBB: defChi->base cannot be nullptr");
      return def.defChi->GetBase()->GetBB();
    }
    case kDefByMustDef: {
      ASSERT(def.defMustDef, "VarMeExpr::DefByBB: defMustDef cannot be nullptr");
      ASSERT(def.defMustDef->GetBase(), "VarMeExpr::DefByBB: defMustDef->base cannot be nullptr");
      return def.defMustDef->GetBase()->GetBB();
    }
    default:
      ASSERT(false, "var define unknown");
      return nullptr;
  }
}

bool VarMeExpr::IsVolatile(SSATab &ssatab) {
  const OriginalSt *ost = ssatab.GetOriginalStFromID(ostIdx);
  if (!ost->IsSymbolOst()) {
    return false;
  }
  const MIRSymbol *sym = ost->GetMIRSymbol();
  if (sym->IsVolatile()) {
    return true;
  }
  MIRType *ty = GlobalTables::GetTypeTable().GetTypeFromTyIdx(sym->GetTyIdx());
  if (fieldID == 0) {
    return (ty->HasVolatileField());
  }
  auto *structType = static_cast<MIRStructType*>(ty);
  return structType->IsFieldVolatile(fieldID);
}

MeExpr *MeExpr::FindSymAppearance(OStIdx oidx) {
  if (meOp == kMeOpVar) {
    if (static_cast<VarMeExpr*>(this)->GetOStIdx() == oidx) {
      return this;
    }
    return nullptr;
  }
  for (uint8 i = 0; i < GetNumOpnds(); i++) {
    MeExpr *retx = GetOpnd(i)->FindSymAppearance(oidx);
    if (retx != nullptr) {
      return retx;
    }
  }
  return nullptr;
}

bool MeExpr::SymAppears(OStIdx oidx) {
  return FindSymAppearance(oidx) != nullptr;
}

bool MeExpr::HasIvar() const {
  if (meOp == kMeOpIvar) {
    return true;
  }
  for (uint8 i = 0; i < GetNumOpnds(); i++) {
    ASSERT(GetOpnd(i) != nullptr, "null ptr check");
    if (GetOpnd(i)->HasIvar()) {
      return true;
    }
  }
  return false;
}


// check if MeExpr can be a pointer to something that requires incref for its
// assigned target
bool MeExpr::PointsToSomethingThatNeedsIncRef() {
  if (op == OP_retype) {
    return true;
  }
  if (meOp == kMeOpIvar) {
    return true;
  }
  if (meOp == kMeOpVar) {
    VarMeExpr *var = static_cast<VarMeExpr*>(this);
    if (var->GetDefBy() == kDefByMustDef) {
      MeStmt *baseStmt = var->GetDefMustDef().GetBase();
      if (baseStmt->GetOp() == OP_callassigned) {
        CallMeStmt *call = static_cast<CallMeStmt*>(baseStmt);
        MIRFunction *callFunc = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(call->GetPUIdx());
        if (callFunc->GetName() == "MCC_GetOrInsertLiteral") {
          return false;
        }
      }
    }
    return true;
  }
  if (meOp == kMeOpReg) {
    RegMeExpr *r = static_cast<RegMeExpr*>(this);
    return r->GetRegIdx() != -kSregThrownval;
  }
  return false;
}

MapleMap<OStIdx, ChiMeNode*> *GenericGetChiListFromVarMeExprInner(VarMeExpr &expr,
                                                                  std::unordered_set<VarMeExpr*> &visited) {
  if (expr.GetDefBy() == kDefByNo || visited.find(&expr) != visited.end()) {
    return nullptr;
  }
  visited.insert(&expr);
  if (expr.GetDefBy() == kDefByPhi) {
    MeVarPhiNode &phime = expr.GetDefPhi();
    MapleVector<VarMeExpr*> &phiopnds = phime.GetOpnds();
    for (auto it = phiopnds.begin(); it != phiopnds.end(); it++) {
      VarMeExpr *meExpr = *it;
      MapleMap<OStIdx, ChiMeNode*> *chiList = GenericGetChiListFromVarMeExprInner(*meExpr, visited);
      if (chiList != nullptr) {
        return chiList;
      }
    }
  } else if (expr.GetDefBy() == kDefByChi) {
    return expr.GetDefChi().GetBase()->GetChiList();
  } else {
    // not yet implemented
    return nullptr;
  }
  return nullptr;
}

MapleMap<OStIdx, ChiMeNode*> *GenericGetChiListFromVarMeExpr(VarMeExpr &expr) {
  std::unordered_set<VarMeExpr*> visited;
  return GenericGetChiListFromVarMeExprInner(expr, visited);
}

void CallMeStmt::SetCallReturn(MeExpr &curexpr) {
  MustDefMeNode &mustDefMeNode = GetMustDefList()->front();
  mustDefMeNode.UpdateLHS(curexpr);
}

bool DumpOptions::simpleDump = false;
int DumpOptions::dumpVsymNum = 0;
}  // namespace maple
