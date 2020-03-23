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

bool DSE::ExprNonDeletable(const BaseNode &expr) const {
  if (kOpcodeInfo.HasSideEffect(expr.GetOpCode())) {
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
    case OP_gcmallocjarray:
    case OP_gcpermallocjarray:
      // may throw exception
      return true;
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
      CHECK_FATAL(rets != nullptr, "null ptr check  ");
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
        bb.RemoveBBFromSucc(succBB);
        succBB->RemoveBBFromPred(&bb);
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
      bb.RemoveStmtNode(&stmt);
      continue;
    }

    CheckRemoveCallAssignedReturn(stmt);
  }
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
    const MustDefNode *mustdef = vst.GetMustDef();
    ASSERT(mustdef->GetResult() == &vst, "MarkVst: wrong corresponding version st in mustdef");
    MarkStmtRequired(ToRef(mustdef->GetStmt()), ToRef(dfBB));
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
      if (StmtMustRequired(*itStmt, *bb)) {
        MarkStmtRequired(*itStmt, *bb);
        continue;
      }
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
