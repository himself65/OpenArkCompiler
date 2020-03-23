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
#include "me_inequality_graph.h"

namespace maple {
  ESSAConstNode *InequalityGraph::GetOrCreateConstNode(int value) {
    if (HasNode(value)) {
      return static_cast<ESSAConstNode*>(constNodes[value].get());
    }
    std::unique_ptr<ESSAConstNode> newConstNode = std::make_unique<ESSAConstNode>(GetValidID(), value);
    CHECK_FATAL(newConstNode != nullptr, "new failed");
    ESSAConstNode *newConst = newConstNode.get();
    constNodes[value] = std::move(newConstNode);
    return newConst;
  }

  ESSAVarNode *InequalityGraph::GetOrCreateVarNode(MeExpr &meExpr) {
    if (HasNode(meExpr)) {
      return static_cast<ESSAVarNode*>(varNodes[meExpr.GetExprID()].get());
    }
    CHECK_FATAL(meExpr.GetMeOp() == kMeOpVar || meExpr.GetMeOp() == kMeOpIvar, "meExpr must be VarMeExpr");
    std::unique_ptr<ESSAVarNode> newVarNode = std::make_unique<ESSAVarNode>(GetValidID(), meExpr);
    CHECK_FATAL(newVarNode != nullptr, "new failed");
    ESSAVarNode *newVar = newVarNode.get();
    varNodes[meExpr.GetExprID()] = std::move(newVarNode);
    return newVar;
  }

  ESSAPhiNode *InequalityGraph::GetOrCreatePhiNode(MeVarPhiNode &phiNode) {
    MeExpr *expr = phiNode.GetLHS();
    CHECK_FATAL(expr != nullptr, "meExpr phiNode must has lhs");
    if (HasNode(*expr)) {
      return static_cast<ESSAPhiNode*>(varNodes[expr->GetExprID()].get());
    }
    CHECK_FATAL(expr->GetMeOp() == kMeOpVar, "meExpr must be VarMeExpr");
    std::unique_ptr<ESSAPhiNode> newPhiNode = std::make_unique<ESSAPhiNode>(GetValidID(), *expr);
    CHECK_FATAL(newPhiNode != nullptr, "new failed");
    newPhiNode->SetPhiOpnds(phiNode.GetOpnds());
    ESSAPhiNode *newPhi = newPhiNode.get();
    varNodes[expr->GetExprID()] = std::move(newPhiNode);
    for (VarMeExpr *phiRHS : phiNode.GetOpnds()) {
      ESSABaseNode *rhs = nullptr;
      if (phiRHS->GetDefBy() != kDefByPhi) {
        rhs = GetOrCreateVarNode(*phiRHS);
      } else {
        MeVarPhiNode *defPhi = &(phiRHS->GetDefPhi());
        rhs = GetOrCreatePhiNode(*defPhi);
      }
      AddPhiEdge(*rhs, *newPhi, EdgeType::kUpper);
      AddPhiEdge(*newPhi, *rhs, EdgeType::kLower);
    }
    CHECK_FATAL(newPhi->GetPhiOpnds().size() == newPhi->GetInPhiEdgeMap().size(), "must be");
    return newPhi;
  }

  ESSAArrayNode *InequalityGraph::GetOrCreateArrayNode(MeExpr &meExpr) {
    if (HasNode(meExpr)) {
      return static_cast<ESSAArrayNode*>(varNodes[meExpr.GetExprID()].get());
    }
    CHECK_FATAL(meExpr.GetMeOp() == kMeOpVar, "meExpr must be VarMeExpr");
    std::unique_ptr<ESSAArrayNode> newArrayNode = std::make_unique<ESSAArrayNode>(GetValidID(), meExpr);
    CHECK_FATAL(newArrayNode != nullptr, "new failed");
    ESSAArrayNode *newArray = newArrayNode.get();
    varNodes[meExpr.GetExprID()] = std::move(newArrayNode);
    return newArray;
  }

  InequalEdge *InequalityGraph::AddEdge(ESSABaseNode &from, ESSABaseNode &to, int value, EdgeType type) {
    InequalEdge tmpEdge = InequalEdge(value, type);
    InequalEdge *edge = HasEdge(from, to, tmpEdge);
    if (edge != nullptr) {
      return edge;
    }
    std::unique_ptr<InequalEdge> e = std::make_unique<InequalEdge>(value, type);
    InequalEdge *ePtr = e.get();
    CHECK_FATAL(e != nullptr, "new failed");
    from.InsertOutWithConstEdgeMap(to, *e);
    to.InsertInWithConstEdgeMap(from, *e);
    from.InsertEdges(std::move(e));
    return ePtr;
  }

  void InequalityGraph::AddPhiEdge(ESSABaseNode &from, ESSABaseNode &to, EdgeType type) {
    std::unique_ptr<InequalEdge> edge = std::make_unique<InequalEdge>(0, type);
    CHECK_FATAL(edge != nullptr, "new failed");
    if (type == EdgeType::kUpper) {
      from.InsertOutWithConstEdgeMap(to, *edge);
      CHECK_FATAL(to.GetKind() == kPhiNode, "must be");
      static_cast<ESSAPhiNode&>(to).InsertInPhiEdgeMap(from, *edge);
      from.InsertEdges(std::move(edge));
    } else {
      CHECK_FATAL(from.GetKind() == kPhiNode, "must be");
      static_cast<ESSAPhiNode&>(from).InsertOutPhiEdgeMap(to, *edge);
      to.InsertInWithConstEdgeMap(from, *edge);
      from.InsertEdges(std::move(edge));
    }
  }

  void InequalityGraph::AddEdge(ESSABaseNode &from, ESSABaseNode &to, MeExpr &value, bool positive, EdgeType type) {
    InequalEdge tmpEdge = InequalEdge(value, positive, type);
    if (HasEdge(from, to, tmpEdge)) {
      return;
    }
    std::unique_ptr<InequalEdge> edge = std::make_unique<InequalEdge>(value, positive, type);
    CHECK_FATAL(edge != nullptr, "new failed");
    from.InsertOutWithVarEdgeMap(to, *edge);
    to.InsertInWithVarEdgeMap(from, *edge);
    from.InsertEdges(std::move(edge));
  }

  bool InequalityGraph::HasNode(const MeExpr &meExpr) const {
    return varNodes.find(meExpr.GetExprID()) != varNodes.end();
  }

  bool InequalityGraph::HasNode(int32 value) const {
    return constNodes.find(value) != constNodes.end();
  }

  void InequalityGraph::ConnectTrivalEdge() {
    int32 prevValue = 0;
    ESSABaseNode* prevNode = nullptr;
    for (auto &pair : constNodes) {
      int32 value = pair.first;
      ESSABaseNode* node = pair.second.get();
      if (prevNode == nullptr) {
        prevValue = value;
        prevNode = node;
        continue;
      }
      CHECK_FATAL(value > prevValue, "must be");
      InequalEdge *pairEdge1 = AddEdge(*node, *prevNode, prevValue - value, EdgeType::kNone);
      InequalEdge *pairEdge2 = AddEdge(*prevNode, *node, value - prevValue, EdgeType::kNone);
      pairEdge1->SetPairEdge(*pairEdge2);
      pairEdge2->SetPairEdge(*pairEdge1);
      prevValue = value;
      prevNode = node;
    }
  }

  ESSABaseNode &InequalityGraph::GetNode(const MeExpr &meExpr) {
    CHECK_FATAL(HasNode(meExpr), "node is not created");
    return *varNodes[meExpr.GetExprID()];
  }

  ESSABaseNode &InequalityGraph::GetNode(int32 value) {
    CHECK_FATAL(HasNode(value), "node is not created");
    return *constNodes[value];
  }

  InequalEdge *InequalityGraph::HasEdge(ESSABaseNode &from, ESSABaseNode &to, InequalEdge &type) const {
    auto miter = from.GetOutWithConstEdgeMap().equal_range(&to);
    for (auto it = miter.first; it != miter.second; ++it) {
      if (it->second->IsSame(type)) {
        return it->second;
      }
    }
    miter = from.GetOutWithVarEdgeMap().equal_range(&to);
    for (auto it = miter.first; it != miter.second; ++it) {
      if (it->second->IsSame(type)) {
        return it->second;
      }
    }
    return nullptr;
  }

  std::string InequalityGraph::GetName(const MeExpr &meExpr, IRMap &irMap) const {
    std::string name;
    MeExprOp meOp = meExpr.GetMeOp();
    CHECK_FATAL(meOp == kMeOpVar, "must be VarMeExpr");
    const auto *varMeExpr = static_cast<const VarMeExpr*>(&meExpr);
    MIRSymbol *sym = irMap.GetSSATab().GetMIRSymbolFromID(varMeExpr->GetOStIdx());
    name = sym->GetName() + "\\nmx" + std::to_string(meExpr.GetExprID());
    return name;
  }

  std::string InequalityGraph::GetName(ESSABaseNode &node, IRMap &irMap) const {
    std::string name;
    if (node.GetKind() == kConstNode) {
      return std::to_string((*static_cast<ESSAConstNode*>(&node)).GetValue());
    }
    const MeExpr *meExpr = &(node.GetMeExpr());
    CHECK_FATAL(meExpr != nullptr, "meExpr is nullptr");
    const VarMeExpr *varMeExpr = nullptr;
    if (meExpr->GetMeOp() == kMeOpVar) {
      varMeExpr = static_cast<const VarMeExpr*>(meExpr);
    } else if (meExpr->GetMeOp() == kMeOpIvar) {
      const auto *ivarMeExpr = static_cast<const IvarMeExpr*>(meExpr);
      const MeExpr *base = ivarMeExpr->GetBase();
      if (base->GetMeOp() == kMeOpVar) {
        varMeExpr = static_cast<const VarMeExpr*>(base);
      } else {
        name = "ivar" + std::to_string(ivarMeExpr->GetExprID());
        return name;
      }
    }
    MIRSymbol *sym = irMap.GetSSATab().GetMIRSymbolFromID(varMeExpr->GetOStIdx());
    name = sym->GetName() + "\\nmx" + std::to_string(meExpr->GetExprID());
    if (node.GetKind() == kArrayNode) {
      name += ".length";
    } else if (node.GetKind() == kPhiNode) {
      name += " = phi(";
      for (auto iter = (*static_cast<ESSAPhiNode*>(&node)).GetPhiOpnds().begin();
           iter != (*static_cast<ESSAPhiNode*>(&node)).GetPhiOpnds().end(); ++iter) {
        name += "mx" + std::to_string((*iter)->GetExprID()) + ", ";
      }
      name += ")";
    }
    name += "\\nID: " + std::to_string(node.GetID());
    return name;
  }

  std::string InequalityGraph::GetColor(EdgeType type) const {
    switch (type) {
      case kUpper:
        return "color = red";
      case kUpperInvalid:
        return "color = red, style=dashed";
      case kLower:
        return "color = blue";
      case kLowerInvalid:
        return "color = blue, style=dashed";
      case kNone:
        return "color = black";
      case kNoneInValid:
        return "color = black, style=dashed";
    }
  }

  void InequalityGraph::DumpDotEdges(IRMap &irMap, const std::pair<ESSABaseNode*, InequalEdge*> &map,
                                     std::ostream &out, std::string &from) const {
    if (map.second->IsVarValue()) {
      std::string to = GetName(*(map.first), irMap);
      std::string positive = map.second->GetVarValue().IsPositive() ? "" : "-";
      std::string color = GetColor(map.second->GetEdgeType());
      out << "\"" << from << "\" -> \"" << to << "\" ";
      out << "[label=\"" << positive << GetName(map.second->GetVarValue().GetVarMeExpr(), irMap)
          << "\" " << color << "];\n";
    } else {
      std::string to = GetName(*(map.first), irMap);
      std::string color = GetColor(map.second->GetEdgeType());
      out << "\"" << from << "\" -> \"" << to << "\" ";
      out << "[label=\"" << map.second->GetConstValue() << "\" " << color << "];\n";
    }
  }

  void InequalityGraph::DumpDotNodes(IRMap &irMap, std::ostream &out, DumpType dumpType,
                                     const std::map<int32, std::unique_ptr<ESSABaseNode>> &nodes) const {
    for (auto iter = nodes.begin(); iter != nodes.end(); ++iter) {
      std::string from = GetName(*(iter->second), irMap);
      out << "\"" << from << "\";\n";
      for (auto iterConstEdges = iter->second.get()->GetOutWithConstEdgeMap().begin();
           iterConstEdges != iter->second.get()->GetOutWithConstEdgeMap().end(); ++iterConstEdges) {
        EdgeType edgeType = iterConstEdges->second->GetEdgeType();
        if (dumpType == kDumpNone) {
          DumpDotEdges(irMap, *iterConstEdges, out, from);
        } else if ((dumpType == kDumpUpperAndNone) &&
                   (edgeType == kUpper || edgeType == kNone || edgeType == kUpperInvalid || edgeType == kNoneInValid)) {
          DumpDotEdges(irMap, *iterConstEdges, out, from);
        } else if ((dumpType == kDumpLowerAndNone) &&
                   (edgeType == kLower || edgeType == kNone || edgeType == kLowerInvalid || edgeType == kNoneInValid)) {
          DumpDotEdges(irMap, *iterConstEdges, out, from);
        }
      }
      if (iter->second.get()->GetKind() == kPhiNode) {
        auto *phiNode = static_cast<ESSAPhiNode*>(iter->second.get());
        for (auto iterConstEdges = phiNode->GetOutPhiEdgeMap().begin();
             iterConstEdges != phiNode->GetOutPhiEdgeMap().end(); ++iterConstEdges) {
          EdgeType edgeType = iterConstEdges->second->GetEdgeType();
          if (dumpType == kDumpNone) {
            DumpDotEdges(irMap, *iterConstEdges, out, from);
          } else if ((dumpType == kDumpUpperAndNone) &&
                     (edgeType == kUpper || edgeType == kNone ||
                      edgeType == kUpperInvalid || edgeType == kNoneInValid)) {
            DumpDotEdges(irMap, *iterConstEdges, out, from);
          } else if ((dumpType == kDumpLowerAndNone) &&
                     (edgeType == kLower || edgeType == kNone ||
                      edgeType == kLowerInvalid || edgeType == kNoneInValid)) {
            DumpDotEdges(irMap, *iterConstEdges, out, from);
          }
        }
      }
      for (auto iterVarEdges = iter->second.get()->GetOutWithVarEdgeMap().begin();
           iterVarEdges != iter->second.get()->GetOutWithVarEdgeMap().end(); ++iterVarEdges) {
        EdgeType edgeType = iterVarEdges->second->GetEdgeType();
        if (dumpType == kDumpNone) {
          DumpDotEdges(irMap, *iterVarEdges, out, from);
        } else if ((dumpType == kDumpUpperAndNone) &&
                   (edgeType == kUpper || edgeType == kNone || edgeType == kUpperInvalid || edgeType == kNoneInValid)) {
          DumpDotEdges(irMap, *iterVarEdges, out, from);
        } else if ((dumpType == kDumpLowerAndNone) &&
                   (edgeType == kLower || edgeType == kNone || edgeType == kLowerInvalid || edgeType == kNoneInValid)) {
          DumpDotEdges(irMap, *iterVarEdges, out, from);
        }
      }
    }
  }

  void InequalityGraph::DumpDotFile(IRMap &irMap, DumpType dumpType) const {
    std::filebuf fileBuf;
    std::string dumpT = dumpType == kDumpUpperAndNone ? "Upper_" : "Lower_";
    std::string outFile =  dumpT + meFunction->GetName() + "-inequalityGraph.dot";
    fileBuf.open(outFile, std::ios::trunc | std::ios::out);
    std::ostream essaDotFile(&fileBuf);
    essaDotFile << "digraph InequalityGraph {\n";
    DumpDotNodes(irMap, essaDotFile, dumpType, constNodes);
    DumpDotNodes(irMap, essaDotFile, dumpType, varNodes);
    essaDotFile << "}\n";
    fileBuf.close();
  }

  bool ABCD::DemandProve(const MeExpr &arrayNode, const MeExpr &idx) {
    ESSABaseNode &aNode = inequalityGraph->GetNode(arrayNode);
    ESSABaseNode *idxNode = nullptr;
    if (idx.GetMeOp() == kMeOpVar) {
      idxNode = &(inequalityGraph->GetNode(idx));
    } else {
      CHECK_FATAL(idx.GetMeOp() == kMeOpConst, "must be");
      idxNode = &(inequalityGraph->GetNode(static_cast<const ConstMeExpr&>(idx).GetIntValue()));
    }
    ESSABaseNode &zNode = inequalityGraph->GetNode(0);
    bool upperResult = ABCD::DemandProve(aNode, *idxNode, kUpper);
    bool lowerResult = ABCD::DemandProve(zNode, *idxNode, kLower);
    return  upperResult && lowerResult;
  }

  bool ABCD::DemandProve(ESSABaseNode &firstNode, ESSABaseNode &secondNode, EdgeType edgeType) {
    std::unique_ptr<InequalEdge> e =
        std::make_unique<InequalEdge>(edgeType == kUpper ? kUpperBound : kLowerBound, edgeType);
    active.clear();
    ProveResult res = Prove(firstNode, secondNode, *e.get());
    return res == kTrue;
  }

  void ABCD::PrintTracing() {
    for (size_t i = 0; i < tracing.size(); i++) {
      std::cout << "Traversing Node : " << tracing[i]->GetExprID() << '\n';
    }
    std::cout << '\n';
  }

  ProveResult ABCD::Prove(ESSABaseNode &aNode, ESSABaseNode &bNode, InequalEdge &edge) {
    ++recursiveCount;
    if (recursiveCount > kDFSLimit) {
      return kFalse;
    }

    tracing.push_back(&bNode);
    if (&aNode == &bNode) {
      if (edge.GreaterEqual(0)) {
        tracing.pop_back();
        return kTrue;
      } else {
        tracing.pop_back();
        return kFalse;
      }
    }
    bool hasPreNode = false;
    auto constEdge = (edge.GetEdgeType() == kUpper) ? bNode.GetInWithConstEdgeMap() : bNode.GetOutWithConstEdgeMap();
    for (auto iter = constEdge.begin(); iter != constEdge.end(); ++iter) {
      if (iter->second->GetEdgeType() == edge.GetEdgeType() || iter->second->GetEdgeType() == EdgeType::kNone) {
        hasPreNode = true;
        break;
      }
    }
    if (bNode.GetKind() == kPhiNode) {
      hasPreNode = true;
    }
    if (!hasPreNode) {
      tracing.pop_back();
      return kFalse;
    }

    if (active.find(&bNode) != active.end()) {
      if (active.find(&bNode)->second->LessEqual(edge)) {
        tracing.pop_back();
        return kReduced;
      } else {
        tracing.pop_back();
        return kFalse;
      }
    }

    active[&bNode] = &edge;
    ProveResult res = bNode.GetKind() == kPhiNode ? UpdateCacheResult(aNode, bNode, edge, Min)
                                                  : UpdateCacheResult(aNode, bNode, edge, Max);
    active.erase(&bNode);
    tracing.pop_back();
    return res;
  }

  ProveResult ABCD::UpdateCacheResult(ESSABaseNode &aNode, ESSABaseNode &bNode, InequalEdge &edge, MeetFunction meet) {
    ProveResult res = meet == Min ? kReduced : kFalse;
    if (meet == Min) {
      CHECK_FATAL(bNode.GetKind() == kPhiNode, "must be");
      auto& bPhiNode = static_cast<ESSAPhiNode&>(bNode);
      auto constEdgeMap = (edge.GetEdgeType() == kUpper) ? bPhiNode.GetInPhiEdgeMap() : bPhiNode.GetOutPhiEdgeMap();
      for (auto iter = constEdgeMap.begin(); iter != constEdgeMap.end(); ++iter) {
        if (((res == kTrue) && (meet == Max)) || ((res == kFalse) && (meet == Min))) {
          break;
        }
        InequalEdge *in = iter->second;
        if (in->GetEdgeType() == edge.GetEdgeType() || in->GetEdgeType() == kNone) {
          InequalEdge nextEdge(edge, *in);
          if (in->GetPairEdge()) {
            in->GetPairEdge()->SetEdgeTypeInValid();
          }
          res = meet(res, Prove(aNode, *(iter->first), nextEdge));
          if (in->GetPairEdge()) {
            in->GetPairEdge()->SetEdgeTypeValid();
          }
        }
      }
    }
    auto constEdgeMap = (edge.GetEdgeType() == kUpper) ? bNode.GetInWithConstEdgeMap() : bNode.GetOutWithConstEdgeMap();
    for (auto iter = constEdgeMap.begin(); iter != constEdgeMap.end(); ++iter) {
      if (res == kTrue) {
        break;
      }
      InequalEdge *in = iter->second;
      if (in->GetEdgeType() == edge.GetEdgeType() || in->GetEdgeType() == kNone) {
        InequalEdge nextEdge(edge, *in);
        if (in->GetPairEdge()) {
          in->GetPairEdge()->SetEdgeTypeInValid();
        }
        res = Max(res, Prove(aNode, *(iter->first), nextEdge));
        if (in->GetPairEdge()) {
          in->GetPairEdge()->SetEdgeTypeValid();
        }
      }
    }

    return res;
  }
} // namespace maple
