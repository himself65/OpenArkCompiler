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
#include "dse.h"
#include "ssa_mir_nodes.h"
#include "ver_symbol.h"
#include "ssa.h"
#include "opcode_info.h"
#include "mir_function.h"
#include "utils.h"
#include "mir_builder.h"

// This phase do dead store elimination. This optimization is done on SSA
// version basis.
// This optimization consider all stmt are not needed at first. The whole
// algorithm is as follow:
// 1. mark all stmt are not needed. init an empty worklist to put live node.
// 2. mark some special stmts which has side effect as needed, such as
//    return/eh/call and some assigned stmts who have volatile fileds and so on.
//    Put all operands and mayUse nodes of the needed stmt into worklist.
// 3. For the nodes in worklist mark the def stmt as needed just as step 2 and
//    pop the node from the worklist.
// 4. Repeat step 3 until the worklist is empty.
namespace maple {
using namespace utils;

bool DSE::ExprHasSideEffect(const BaseNode &expr) const {
  Opcode op = expr.GetOpCode();
  if (kOpcodeInfo.HasSideEffect(op)) {
    return true;
  }
  // may throw exception
  if (op == OP_gcmallocjarray || op == OP_gcpermallocjarray) {
    return true;
  }
  // create a instance of interface
  if (op == OP_gcmalloc || op == OP_gcpermalloc) {
    auto &gcMallocNode = static_cast<const GCMallocNode&>(expr);
    MIRType *type = GlobalTables::GetTypeTable().GetTypeFromTyIdx(gcMallocNode.GetTyIdx());
    return type->GetKind() == kTypeInterface;
  }
  return false;
}

bool DSE::ExprNonDeletable(const BaseNode &expr) const {
  if (ExprHasSideEffect(expr)) {
    return true;
  }
  switch (expr.GetOpCode()) {
    case OP_dread: {
      auto &dread = static_cast<const AddrofSSANode&>(expr);
      const MIRSymbol &sym = dread.GetMIRSymbol();
      return sym.IsVolatile() || sym.IsTypeVolatile(dread.GetFieldID());
    }
    case OP_iread: {
      auto &iread = static_cast<const IreadSSANode&>(expr);
      auto &ty = static_cast<const MIRPtrType&>(GetTypeFromTyIdx(iread.GetTyIdx()));
      return ty.IsPointedTypeVolatile(iread.GetFieldID()) || ExprNonDeletable(ToRef(iread.Opnd(0)));
    }
    case OP_regread: {
      auto &regreadNode = static_cast<const RegreadSSANode&>(expr);
      return (regreadNode.GetRegIdx() == -kSregThrownval);
    }
    case OP_intrinsicop: {
      auto &node = static_cast<const IntrinsicopNode&>(expr);
      const IntrinDesc &intrinDesc = node.GetIntrinDesc();
      return (!intrinDesc.HasNoSideEffect());
    }
    default: {
      break;
    }
  }
  for (size_t i = 0; i < expr.NumOpnds(); ++i) {
    if (ExprNonDeletable(ToRef(expr.Opnd(i)))) {
      return true;
    }
  }
  return false;
}

bool DSE::HasNonDeletableExpr(const StmtNode &stmt) const {
  Opcode op = stmt.GetOpCode();

  switch (op) {
    case OP_dassign: {
      auto &node = static_cast<const DassignNode&>(stmt);
      const MIRSymbol &sym = ssaTab.GetStmtMIRSymbol(stmt);
      return (sym.IsVolatile() || sym.IsTypeVolatile(node.GetFieldID()) ||
              ExprNonDeletable(ToRef(node.GetRHS())));
    }
    case OP_regassign: {
      return ExprNonDeletable(ToRef(stmt.Opnd(0)));
    }
    // possible to throw exception
    case OP_maydassign: {
      return true;
    }
    case OP_iassign: {
      auto &node = static_cast<const IassignNode&>(stmt);
      auto &ty = static_cast<const MIRPtrType&>(GetTypeFromTyIdx(node.GetTyIdx()));
      return (ty.IsPointedTypeVolatile(node.GetFieldID()) ||
              ExprNonDeletable(ToRef(node.Opnd(0))) ||
              ExprNonDeletable(ToRef(node.GetRHS())));
    }
    default:
      return false;
  }
}

bool DSE::StmtMustRequired(const StmtNode &stmt, const BB &bb) const {
  Opcode op = stmt.GetOpCode();
  // special opcode stmt cannot be eliminated
  if (IsStmtMustRequire(op)) {
    return true;
  }

  // control flow in an infinite loop cannot be eliminated
  if (ControlFlowInInfiniteLoop(bb, op)) {
    return true;
  }

  return HasNonDeletableExpr(stmt);
}

void DSE::DumpStmt(const StmtNode &stmt, const std::string &msg) const {
  if (enableDebug) {
    LogInfo::MapleLogger() << msg;
    stmt.Dump();
  }
}

void DSE::CheckRemoveCallAssignedReturn(StmtNode &stmt) {
  if (kOpcodeInfo.IsCallAssigned(stmt.GetOpCode())) {
    MapleVector<MustDefNode> &mustDefs = ssaTab.GetStmtMustDefNodes(stmt);
    for (auto &node : mustDefs) {
      if (IsSymbolLived(ToRef(node.GetResult()))) {
        continue;
      }
      DumpStmt(stmt, "**** DSE1 deleting return value assignment in: ");
      CallReturnVector *rets = stmt.GetCallReturnVector();
      CHECK_FATAL(rets != nullptr, "null ptr check");
      rets->clear();
      mustDefs.clear();
      break;
    }
  }
}

void DSE::OnRemoveBranchStmt(BB &bb, const StmtNode &stmt) {
  // switch is special, which can not be set to kBBFallthru
  if (IsBranch(stmt.GetOpCode()) && stmt.GetOpCode() != OP_switch) {
    // update BB pred/succ
    bb.SetKind(kBBFallthru);
    cfgUpdated = true;  // tag cfg is changed
    LabelIdx labelIdx = (stmt.GetOpCode() == OP_goto) ? static_cast<const GotoNode&>(stmt).GetOffset()
                                                      : static_cast<const CondGotoNode&>(stmt).GetOffset();
    for (size_t i = 0; i < bb.GetSucc().size(); ++i) {
      if (bb.GetSucc(i)->GetBBLabel() == labelIdx) {
        BB *succBB = bb.GetSucc(i);
        bb.RemoveSucc(*succBB);
        break;
      }
    }
  }
}

void DSE::RemoveNotRequiredStmtsInBB(BB &bb) {
  for (auto &stmt : bb.GetStmtNodes()) {
    if (!IsStmtRequired(stmt)) {
      DumpStmt(stmt, "**** DSE1 deleting: ");
      OnRemoveBranchStmt(bb, stmt);
      // A iread node contained in stmt or iass stmt
      if (stmt2NotNullExpr.find(&stmt) != stmt2NotNullExpr.end()) {
        for (BaseNode *node : stmt2NotNullExpr.at(&stmt)) {
          if (NeedNotNullCheck(*node, bb)) {
            MIRModule &mod = ssaTab.GetModule();
            UnaryStmtNode *nullCheck = mod.GetMIRBuilder()->CreateStmtUnary(OP_assertnonnull, node);
            bb.InsertStmtBefore(&stmt, nullCheck);
            nullCheck->SetIsLive(true);
            notNullExpr2Stmt[node].push_back(std::make_pair(nullCheck, &bb));
          }
        }
      }
      bb.RemoveStmtNode(&stmt);
      continue;
    }

    CheckRemoveCallAssignedReturn(stmt);
  }
}

// If a ivar's base not used as not null, should insert a not null stmt
// Only make sure throw NPE in same BB
// If must make sure throw at first stmt, much more not null stmt will be inserted
bool DSE::NeedNotNullCheck(BaseNode &node, const BB &bb) {
  for (auto item : notNullExpr2Stmt[&node]) {
    if (!IsStmtRequired(*(item.first))) {
      continue;
    }
    if (postDom.Dominate(*(item.second), bb)) {
      return false;
    }
  }
  return true;
}

void DSE::PropagateUseLive(const VersionSt &vst) {
  if (IsSymbolLived(vst)) {
    return;
  }
  SetSymbolLived(vst);
  const BB *dfBB = vst.GetDefBB();
  if (dfBB == nullptr) {
    return;
  }
  if (vst.GetDefType() == VersionSt::kAssign) {
    const StmtNode *assign = vst.GetAssignNode();
    MarkStmtRequired(ToRef(assign), ToRef(dfBB));
  } else if (vst.GetDefType() == VersionSt::kPhi) {
    const PhiNode *phi = vst.GetPhi();
    ASSERT(phi->GetResult() == &vst, "MarkVst: wrong corresponding version st in phi");
    MarkControlDependenceLive(ToRef(dfBB));
    for (size_t i = 0; i < phi->GetPhiOpnds().size(); ++i) {
      const VersionSt *verSt = phi->GetPhiOpnds()[i];
      AddToWorkList(verSt);
    }
  } else if (vst.GetDefType() == VersionSt::kMayDef) {
    const MayDefNode *mayDef = vst.GetMayDef();
    ASSERT(mayDef->GetResult() == &vst, "MarkVst: wrong corresponding version st in maydef");
    const VersionSt *verSt = mayDef->GetOpnd();
    MarkStmtRequired(ToRef(mayDef->GetStmt()), ToRef(dfBB));
    AddToWorkList(verSt);
  } else {
    const MustDefNode *mustDef = vst.GetMustDef();
    ASSERT(mustDef->GetResult() == &vst, "MarkVst: wrong corresponding version st in mustDef");
    MarkStmtRequired(ToRef(mustDef->GetStmt()), ToRef(dfBB));
  }
}

void DSE::MarkLastGotoInPredBBRequired(const BB &bb) {
  for (auto predIt = bb.GetPred().begin(); predIt != bb.GetPred().end(); ++predIt) {
    const BB *predBB = *predIt;
    CHECK_NULL_FATAL(predBB);
    if (predBB == &bb || predBB->IsEmpty()) {
      continue;
    }
    const StmtNode &lastStmt = predBB->GetLast();
    if (lastStmt.GetOpCode() == OP_goto) {
      MarkStmtRequired(lastStmt, ToRef(predBB));
    }
  }
}

void DSE::MarkLastBranchStmtInPDomBBRequired(const BB &bb) {
  ASSERT(bb.GetBBId() < postDom.GetPdomFrontierSize(), "index out of range in DSE::MarkBBLive ");
  for (BBId pdomBBID : postDom.GetPdomFrontierItem(bb.GetBBId())) {
    const BB *cdBB = bbVec[pdomBBID];
    CHECK_FATAL(cdBB != nullptr, "cd_bb is null in DSE::MarkLastBranchStmtInPDomBBRequired");
    if (cdBB == &bb || cdBB->IsEmpty()) {
      continue;
    }

    const StmtNode &lastStmt = cdBB->GetLast();
    Opcode op = lastStmt.GetOpCode();
    if (IsBranch(op)) {
      MarkStmtRequired(lastStmt, ToRef(cdBB));
    }
  }
}

void DSE::MarkLastBranchStmtInBBRequired(const BB &bb) {
  if (!bb.IsEmpty()) {
    // if bb's last stmt is a branch instruction, it is also needed
    const StmtNode &lastStmt = bb.GetLast();
    if (IsBranch(lastStmt.GetOpCode())) {
      MarkStmtRequired(lastStmt, bb);
    }
  }
}

void DSE::MarkControlDependenceLive(const BB &bb) {
  if (bbRequired[bb.GetBBId()]) {
    return;
  }
  bbRequired[bb.GetBBId()] = true;

  MarkLastBranchStmtInBBRequired(bb);
  MarkLastBranchStmtInPDomBBRequired(bb);
  MarkLastGotoInPredBBRequired(bb);
}

void DSE::MarkSingleUseLive(const BaseNode &mirNode) {
  Opcode op = mirNode.GetOpCode();
  switch (op) {
    case OP_dread: {
      auto &addrofSSANode = static_cast<const AddrofSSANode&>(mirNode);
      const VersionSt *verSt = addrofSSANode.GetSSAVar();
      AddToWorkList(verSt);
      break;
    }
    case OP_regread: {
      auto &regreadSSANode = static_cast<const RegreadSSANode&>(mirNode);
      const VersionSt *verSt = regreadSSANode.GetSSAVar();
      AddToWorkList(verSt);
      break;
    }
    case OP_iread: {
      auto &ireadSSANode = static_cast<const IreadSSANode&>(mirNode);
      const VersionSt *verSt = ireadSSANode.GetSSAVar();
      CHECK_FATAL(verSt != nullptr, "DSE::MarkSingleUseLive: iread has no mayUse opnd");
      AddToWorkList(verSt);
      if (!verSt->IsInitVersion()) {
        auto *mayDefList = SSAGenericGetMayDefsFromVersionSt(ToRef(verSt), ssaTab.GetStmtsSSAPart());
        if (mayDefList != nullptr) {
          for (auto it = mayDefList->begin(); it != mayDefList->end(); ++it) {
            AddToWorkList(it->second.GetResult());
          }
        }
      }
      MarkSingleUseLive(ToRef(mirNode.Opnd(0)));
      break;
    }
    default: {
      for (size_t i = 0; i < mirNode.NumOpnds(); ++i) {
        MarkSingleUseLive(ToRef(mirNode.Opnd(i)));
      }
      break;
    }
  }
}

void DSE::MarkStmtUseLive(const StmtNode &stmt) {
  for (size_t i = 0; i < stmt.NumOpnds(); ++i) {
    MarkSingleUseLive(ToRef(stmt.Opnd(i)));
  }

  if (kOpcodeInfo.HasSSAUse(stmt.GetOpCode())) {
    for (auto &pair : ssaTab.GetStmtMayUseNodes(stmt)) {
      const MayUseNode &mayUse = pair.second;
      AddToWorkList(mayUse.GetOpnd());
    }
  }
}

void DSE::MarkStmtRequired(const StmtNode &stmt, const BB &bb) {
  if (IsStmtRequired(stmt)) {
    return;
  }
  SetStmtRequired(stmt);

  // save only one line comment
  StmtNode *prev = stmt.GetPrev();
  if (prev != nullptr && prev->GetOpCode() == OP_comment) {
    SetStmtRequired(*prev);
  }

  MarkStmtUseLive(stmt);

  MarkControlDependenceLive(bb);
}

void DSE::MarkSpecialStmtRequired() {
  for (auto bIt = bbVec.rbegin(); bIt != bbVec.rend(); ++bIt) {
    auto *bb = *bIt;
    if (bb == nullptr) {
      continue;
    }
    for (auto itStmt = bb->GetStmtNodes().rbegin(); itStmt != bb->GetStmtNodes().rend(); ++itStmt) {
      StmtNode &stmt = *itStmt;
      if (StmtMustRequired(stmt, *bb)) {
        MarkStmtRequired(stmt, *bb);
      }
      CollectNotNullNode(stmt, *bb);
    }
  }
}

// Find all stmt contains ivar and save to stmt2NotNullExpr
// Find all not null expr used as ivar's base、OP_array's or OP_assertnonnull's opnd
// And save to notNullExpr2Stmt
void DSE::CollectNotNullNode(StmtNode &stmt, BB &bb) {
  uint8 opndNum = static_cast<uint8>(stmt.NumOpnds());
  uint8 nodeType = kNodeTypeNormal;
  for (uint8 i = 0; i < opndNum; ++i) {
    BaseNode *opnd = stmt.Opnd(i);
    if (i == 0 && instance_of<CallNode>(stmt)) {
      // A non-static call's first opnd is this, should be not null
      CallNode &call = static_cast<CallNode&>(stmt);
      if (!GlobalTables::GetFunctionTable().GetFunctionFromPuidx(call.GetPUIdx())->IsStatic()) {
        nodeType = kNodeTypeNotNull;
      }
    } else if (i == 0 && stmt.GetOpCode() == OP_iassign) {
      // A iass stmt, mark and save
      BaseNode &base = static_cast<IassignNode&>(stmt).GetAddrExprBase();
      stmt2NotNullExpr[&stmt].push_back(&base);
      MarkSingleUseLive(base);
      notNullExpr2Stmt[&base].push_back(std::make_pair(&stmt, &bb));
      nodeType = kNodeTypeIvar;
    } else {
      // A normal opnd not sure
      Opcode opndOp = opnd->GetOpCode();
      if (opndOp == OP_dread || opndOp == OP_regread) {
        continue;
      }
      nodeType = kNodeTypeNormal;
    }
    CollectNotNullNode(stmt, ToRef(opnd), bb, nodeType);
  }
}

void DSE::CollectNotNullNode(StmtNode &stmt, BaseNode &node, BB &bb, uint8 nodeType) {
  Opcode op = node.GetOpCode();
  switch (op) {
    case OP_dread:
    case OP_regread:
    case OP_constval: {
      // Ref expr used in ivar、array or assertnotnull
      PrimType type = node.GetPrimType();
      if (nodeType != kNodeTypeNormal && (type == PTY_ref || type == PTY_ptr)) {
        notNullExpr2Stmt[&node].push_back(std::make_pair(&stmt, &bb));
      }
      break;
    }
    case OP_iread: {
      BaseNode &base = static_cast<IreadNode&>(node).GetAddrExprBase();
      if (nodeType != kNodeTypeIvar) {
        stmt2NotNullExpr[&stmt].push_back(&base);
        MarkSingleUseLive(base);
      }
      notNullExpr2Stmt[&base].push_back(std::make_pair(&stmt, &bb));
      CollectNotNullNode(stmt, base, bb, kNodeTypeIvar);
      break;
    }
    default: {
      if (nodeType != kNodeTypeNormal) {
        // Ref expr used in ivar、array or assertnotnull
        PrimType type = node.GetPrimType();
        if (type == PTY_ref || type == PTY_ptr) {
          notNullExpr2Stmt[&node].push_back(std::make_pair(&stmt, &bb));
        }
      } else {
        // Ref expr used array or assertnotnull
        bool notNull = op == OP_array || op == OP_assertnonnull;
        nodeType = notNull ? kNodeTypeNotNull : kNodeTypeNormal;
      }
      for (size_t i = 0; i < node.GetNumOpnds(); ++i) {
        CollectNotNullNode(stmt, ToRef(node.Opnd(i)), bb, nodeType);
      }
      break;
    }
  }
}

void DSE::Init() {
  bbRequired[commonEntryBB.GetBBId()] = true;
  bbRequired[commonExitBB.GetBBId()] = true;
}

void DSE::DoDSE() {
  Init();
  MarkSpecialStmtRequired();
  PropagateLive();
  RemoveNotRequiredStmts();
}
}  // namespace maple
