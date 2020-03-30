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
#include "callgraph.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_set>
#include <algorithm>
#include "option.h"
#include "retype.h"
#include "string_utils.h"
//                   Call Graph Analysis
// This phase is a foundation phase of compilation. This phase build
// the call graph not only for this module also for the modules it
// depends on when this phase is running for IPA.
// The main procedure shows as following.
// A. Devirtual virtual call of private final static and none-static
//    variable. This step aims to reduce the callee set for each call
//    which can benefit IPA analysis.
// B. Build Call Graph.
//    i)  For IPA, it rebuild all the call graph of the modules this
//        module depends on. All necessary information is stored in mplt.
//    ii) Analysis each function in this module. For each call statement
//        create a CGNode, and collect potential callee functions to
//        generate Call Graph.
// C. Find All Root Node for the Call Graph.
// D. Construct SCC based on Tarjan Algorithm
// E. Set compilation order as the bottom-up order of callgraph. So callee
//    is always compiled before caller. This benefits those optimizations
//    need interprocedure information like escape analysis.
namespace maple {
const char *CallInfo::GetCallTypeName() const {
  switch (ctype) {
    case kCallTypeCall:
      return "c";
    case kCallTypeVirtualCall:
      return "v";
    case kCallTypeSuperCall:
      return "s";
    case kCallTypeInterfaceCall:
      return "i";
    case kCallTypeIcall:
      return "icall";
    case kCallTypeIntrinsicCall:
      return "intrinsiccall";
    case kCallTypeXinitrinsicCall:
      return "xintrinsiccall";
    case kCallTypeIntrinsicCallWithType:
      return "intrinsiccallwithtype";
    case kCallTypeFakeThreadStartRun:
      return "fakecallstartrun";
    case kCallTypeCustomCall:
      return "customcall";
    case kCallTypePolymorphicCall:
      return "polymorphiccall";
    default:
      CHECK_FATAL(false, "unsupport CALL type");
      return nullptr;
  }
}

const char *CallInfo::GetCalleeName() const {
  if ((ctype >= kCallTypeCall) && (ctype <= kCallTypeInterfaceCall)) {
    MIRFunction *mirf = mirFunc;
    return mirf->GetName().c_str();
  } else if (ctype == kCallTypeIcall) {
    return "IcallUnknown";
  } else if ((ctype >= kCallTypeIntrinsicCall) && (ctype <= kCallTypeIntrinsicCallWithType)) {
    return "IntrinsicCall";
  } else if (ctype == kCallTypeCustomCall) {
    return "CustomCall";
  } else if (ctype == kCallTypePolymorphicCall) {
    return "PolymorphicCall";
  }
  CHECK_FATAL(false, "should not be here");
  return nullptr;
}

void CGNode::DumpDetail() const {
  LogInfo::MapleLogger() << "---CGNode  @" << this << ": " << mirFunc->GetName() << "\t";
  if (HasOneCandidate() != nullptr) {
    LogInfo::MapleLogger() << "@One Candidate\n";
  } else {
    LogInfo::MapleLogger() << std::endl;
  }
  if (HasSetVCallCandidates()) {
    for (uint32 i = 0; i < vcallCands.size(); i++) {
      LogInfo::MapleLogger() << "   virtual call candidates: " << vcallCands[i]->GetName() << "\n";
    }
  }
  for (auto &callSite : callees) {
    for (auto &cgIt : *callSite.second) {
      CallInfo *ci = callSite.first;
      CGNode *node = cgIt;
      MIRFunction *mf = node->GetMIRFunction();
      if (mf != nullptr) {
        LogInfo::MapleLogger() << "\tcallee in module : " << mf->GetName() << "  ";
      } else {
        LogInfo::MapleLogger() << "\tcallee external: " << ci->GetCalleeName();
      }
    }
  }
  // dump caller
  for (auto const &callernode : callerSet) {
    CHECK_FATAL(callernode && callernode->mirFunc, "");
    LogInfo::MapleLogger() << "\tcaller : " << callernode->mirFunc->GetName() << std::endl;
  }
}

void CGNode::Dump(std::ofstream &fout) const {
  /* if dumpall == 1, dump whole call graph
   * else dump callgraph with function defined in same module */
  CHECK_FATAL(mirFunc != nullptr, "");
  if (callees.empty()) {
    fout << "\"" << mirFunc->GetName() << "\";\n";
    return;
  }
  for (auto &callSite : callees) {
    for (auto &cgIt : *callSite.second) {
      CallInfo *ci = callSite.first;
      CGNode *node = cgIt;
      if (node == nullptr) {
        continue;
      }
      MIRFunction *func = node->GetMIRFunction();
      fout << "\"" << mirFunc->GetName() << "\" -> ";
      if (func != nullptr) {
        if (node->GetSCCNode() != nullptr && node->GetSCCNode()->GetCGNodes().size() > 1) {
          fout << "\"" << func->GetName() << "\"[label=" << node->GetSCCNode()->id << " color=red];\n";
        } else {
          fout << "\"" << func->GetName() << "\"[label=" << 0 << " color=blue];\n";
        }
      } else {
        // unknown / external function with empty function body
        fout << "\"" << ci->GetCalleeName() << "\"[label=" << ci->GetCallTypeName() << " color=blue];\n";
      }
    }
  }
}

void CGNode::AddCallsite(CallInfo *ci, MapleSet<CGNode*, Comparator<CGNode>> *callee) {
  callees.insert(std::pair<CallInfo*, MapleSet<CGNode*, Comparator<CGNode>>*>(ci, callee));
}

void CGNode::AddCallsite(CallInfo *ci, CGNode *node) {
  CHECK_FATAL(ci->GetCallType() != kCallTypeInterfaceCall, "must be true");
  CHECK_FATAL(ci->GetCallType() != kCallTypeVirtualCall, "must be true");
  auto *cgVector = alloc->GetMemPool()->New<MapleSet<CGNode*, Comparator<CGNode>>>(alloc->Adapter());
  cgVector->insert(node);
  (void)callees.emplace(ci, cgVector);
  if (node) {
    node->AddNumRefs();
  }
}

void CGNode::RemoveCallsite(const CallInfo *ci, CGNode *node) {
  for (Callsite callSite : GetCallee()) {
    if (callSite.first == ci) {
      auto cgIt = callSite.second->find(node);
      if (cgIt != callSite.second->end()) {
        callSite.second->erase(cgIt);
        return;
      }
      CHECK_FATAL(false, "node isn't in ci");
    }
  }
}

bool CGNode::IsCalleeOf(CGNode *func) {
  return callerSet.find(func) != callerSet.end();
}

void CallGraph::DelNode(CGNode *node) {
  ASSERT_NOT_NULL(node);
  if (!node->GetMIRFunction()) {
    return;
  }
  for (auto &callSite : node->GetCallee()) {
    for (auto &cgIt : *callSite.second) {
      cgIt->DelCaller(node);
      if (!cgIt->HasCaller()) {
        DelNode(cgIt);
      }
    }
  }
  MIRFunction *func = node->GetMIRFunction();
  // Delete the method of class info
  if (func->GetClassTyIdx() != 0u) {
    MIRType *classType = GlobalTables::GetTypeTable().GetTypeTable().at(func->GetClassTyIdx());
    auto *mirStructType = static_cast<MIRStructType*>(classType);
    uint32 j = 0;
    for (; j < mirStructType->GetMethods().size(); ++j) {
      if (mirStructType->GetMethods()[j].first == func->GetStIdx()) {
        mirStructType->GetMethods().erase(mirStructType->GetMethods().begin() + j);
        break;
      }
    }
  }
  for (uint32 i = 0; i < GlobalTables::GetFunctionTable().GetFuncTable().size(); ++i) {
    if (GlobalTables::GetFunctionTable().GetFunctionFromPuidx(i) == func) {
      uint32 j = 0;
      for (; j < mirModule->GetFunctionList().size(); ++j) {
        if (mirModule->GetFunction(j) == GlobalTables::GetFunctionTable().GetFunctionFromPuidx(i)) {
          break;
        }
      }
      if (j < mirModule->GetFunctionList().size()) {
        mirModule->GetFunctionList().erase(mirModule->GetFunctionList().begin() + j);
      }
      GlobalTables::GetFunctionTable().GetFuncTable()[i] = nullptr;
      break;
    }
  }
  nodesMap.erase(func);
  // Update Klass info as it has been built
  if (klassh->GetKlassFromFunc(func)) {
    klassh->GetKlassFromFunc(func)->DelMethod(*func);
  }
}

CallGraph::CallGraph(MIRModule *m, MemPool *memPool, KlassHierarchy *kh, const char *fn)
    : AnalysisResult(memPool),
      mirModule(m),
      cgalloc(memPool),
      mirBuilder(cgalloc.GetMemPool()->New<MIRBuilder>(m)),
      entry_node(nullptr),
      rootNodes(cgalloc.Adapter()),
      fileName(fn),
      klassh(kh),
      nodesMap(cgalloc.Adapter()),
      sccTopologicalVec(cgalloc.Adapter()),
      numOfNodes(0),
      numOfSccs(0) {
  CHECK_FATAL(fn != nullptr, "");
  callExternal = cgalloc.GetMemPool()->New<CGNode>(static_cast<MIRFunction*>(nullptr), &cgalloc, numOfNodes++);
  debug_flag = false;
  debug_scc = false;
}

CallType CallGraph::GetCallType(Opcode op) const {
  CallType t = kCallTypeInvalid;
  switch (op) {
    case OP_call:
    case OP_callassigned:
      t = kCallTypeCall;
      break;
    case OP_virtualcall:
    case OP_virtualcallassigned:
      t = kCallTypeVirtualCall;
      break;
    case OP_superclasscall:
    case OP_superclasscallassigned:
      t = kCallTypeSuperCall;
      break;
    case OP_interfacecall:
    case OP_interfacecallassigned:
      t = kCallTypeInterfaceCall;
      break;
    case OP_icall:
    case OP_icallassigned:
      t = kCallTypeIcall;
      break;
    case OP_intrinsiccall:
    case OP_intrinsiccallassigned:
      t = kCallTypeIntrinsicCall;
      break;
    case OP_xintrinsiccall:
    case OP_xintrinsiccallassigned:
      t = kCallTypeXinitrinsicCall;
      break;
    case OP_intrinsiccallwithtype:
    case OP_intrinsiccallwithtypeassigned:
      t = kCallTypeIntrinsicCallWithType;
      break;
    case OP_customcall:
    case OP_customcallassigned:
      t = kCallTypeCustomCall;
      break;
    case OP_polymorphiccall:
    case OP_polymorphiccallassigned:
      t = kCallTypePolymorphicCall;
      break;
    default:
      break;
  }
  return t;
}

CGNode *CallGraph::GetCGNode(MIRFunction *func) const {
  if (nodesMap.find(func) != nodesMap.end()) {
    return nodesMap.at(func);
  }
  return nullptr;
}

CGNode *CallGraph::GetCGNode(PUIdx puIdx) const {
  return GetCGNode(GlobalTables::GetFunctionTable().GetFunctionFromPuidx(puIdx));
}

SCCNode *CallGraph::GetSCCNode(MIRFunction *func) const {
  CGNode *cgnode = GetCGNode(func);
  return cgnode ? cgnode->GetSCCNode() : nullptr;
}

bool CallGraph::IsRootNode(MIRFunction *func) const {
  if (GetCGNode(func)) {
    return (!GetCGNode(func)->HasCaller());
  } else {
    return false;
  }
}

CGNode *CallGraph::GetOrGenCGNode(PUIdx puIdx, bool isVcall, bool isIcall) {
  CGNode *node = GetCGNode(puIdx);
  if (node == nullptr) {
    MIRFunction *mirFunc = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(puIdx);
    node = cgalloc.GetMemPool()->New<CGNode>(mirFunc, &cgalloc, numOfNodes++);
    nodesMap.insert(std::make_pair(mirFunc, node));
  }
  if (isVcall && !node->IsVcallCandidatesValid()) {
    MIRFunction *mirFunc = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(puIdx);
    Klass *klass = nullptr;
    if (StringUtils::StartsWith(mirFunc->GetBaseClassName(), JARRAY_PREFIX_STR)) { // Array
      klass = klassh->GetKlassFromName(NameMangler::kJavaLangObjectStr);
    } else {
      klass = klassh->GetKlassFromStrIdx(mirFunc->GetBaseClassNameStrIdx());
    }
    if (klass == nullptr) { // Incomplete
      node->SetVcallCandidatesValid();
      return node;
    }
    // Traverse all subclasses
    std::vector<Klass*> klassVector;
    klassVector.push_back(klass);
    GStrIdx calleeFuncStrIdx = mirFunc->GetBaseFuncNameWithTypeStrIdx();
    for (Klass *currKlass : klassVector) {
      const MIRFunction *method = currKlass->GetMethod(calleeFuncStrIdx);
      if (method != nullptr) {
        node->AddVcallCandidate(GetOrGenCGNode(method->GetPuidx()));
      }
      // add subclass of currKlass into vector
      for (Klass *subKlass : currKlass->GetSubKlasses()) {
        klassVector.push_back(subKlass);
      }
    }
    if (!klass->GetMIRClassType()->IsAbstract()) {
      // If klass.foo does not exist, search superclass and find the nearest one
      // klass.foo does not exist
      auto &klassMethods = klass->GetMethods();
      if (std::find(klassMethods.begin(), klassMethods.end(), mirFunc) == klassMethods.end()) {
        Klass *superKlass = klass->GetSuperKlass();
        while (superKlass != nullptr) {
          const MIRFunction *method = superKlass->GetMethod(calleeFuncStrIdx);
          if (method != nullptr) {
            node->AddVcallCandidate(GetOrGenCGNode(method->GetPuidx()));
            break;
          }
          superKlass = superKlass->GetSuperKlass();
        }
      }
    }
    node->SetVcallCandidatesValid();
  }
  if (isIcall && !node->IsIcallCandidatesValid()) {
    Klass *CallerKlass = nullptr;
    if (StringUtils::StartsWith(CurFunction()->GetBaseClassName(), JARRAY_PREFIX_STR)) { // Array
      CallerKlass = klassh->GetKlassFromName(NameMangler::kJavaLangObjectStr);
    } else {
      CallerKlass = klassh->GetKlassFromStrIdx(CurFunction()->GetBaseClassNameStrIdx());
    }
    if (CallerKlass == nullptr) { // Incomplete
      CHECK_FATAL(false, "class is incomplete, impossible.");
      return node;
    }
    MIRFunction *mirFunc = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(puIdx);
    Klass *klass = nullptr;
    if (StringUtils::StartsWith(mirFunc->GetBaseClassName(), JARRAY_PREFIX_STR)) {  // Array
      klass = klassh->GetKlassFromName(NameMangler::kJavaLangObjectStr);
    } else {
      klass = klassh->GetKlassFromStrIdx(mirFunc->GetBaseClassNameStrIdx());
    }
    if (klass == nullptr) { // Incomplete
      node->SetIcallCandidatesValid();
      return node;
    }
    GStrIdx calleeFuncStrIdx = mirFunc->GetBaseFuncNameWithTypeStrIdx();
    // Traverse all classes which implement the interface
    for (Klass *implKlass : klass->GetImplKlasses()) {
      const MIRFunction *method = implKlass->GetMethod(calleeFuncStrIdx);
      if (method != nullptr) {
        node->AddIcallCandidate(GetOrGenCGNode(method->GetPuidx()));
      } else if (!implKlass->GetMIRClassType()->IsAbstract()) {
        // Search in its parent class
        Klass *superKlass = implKlass->GetSuperKlass();
        while (superKlass != nullptr) {
          const MIRFunction *methodT = superKlass->GetMethod(calleeFuncStrIdx);
          if (methodT != nullptr) {
            node->AddIcallCandidate(GetOrGenCGNode(methodT->GetPuidx()));
            break;
          }
          superKlass = superKlass->GetSuperKlass();
        }
      }
    }
    node->SetIcallCandidatesValid();
  }
  return node;
}

void CallGraph::HandleBody(MIRFunction *func, BlockNode *body, CGNode *node, uint32 loopDepth) {
  StmtNode *stmtNext = nullptr;
  for (StmtNode *stmt = body->GetFirst(); stmt != nullptr; stmt = stmtNext) {
    stmtNext = static_cast<StmtNode*>(stmt)->GetNext();
    Opcode op = stmt->GetOpCode();
    if (op == OP_comment) {
      continue;
    } else if (op == OP_doloop) {
      DoloopNode *n = static_cast<DoloopNode*>(stmt);
      HandleBody(func, n->GetDoBody(), node, loopDepth + 1);
    } else if (op == OP_dowhile || op == OP_while) {
      WhileStmtNode *n = static_cast<WhileStmtNode*>(stmt);
      HandleBody(func, n->GetBody(), node, loopDepth + 1);
    } else if (op == OP_if) {
      IfStmtNode *n = static_cast<IfStmtNode*>(stmt);
      HandleBody(func, n->GetThenPart(), node, loopDepth);
      if (n->GetElsePart()) {
        HandleBody(func, n->GetElsePart(), node, loopDepth);
      }
    } else {
      node->IncrStmtCount();
      CallType ct = GetCallType(op);
      switch (ct) {
        case kCallTypeVirtualCall: {
          PUIdx calleePUIdx = (static_cast<CallNode*>(stmt))->GetPUIdx();
          MIRFunction *calleefunc = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(calleePUIdx);
          CallInfo *callInfo = GenCallInfo(kCallTypeVirtualCall, calleefunc, stmt, loopDepth, stmt->GetStmtID());
          // Retype makes object type more inaccurate.
          StmtNode *stmtPrev = static_cast<StmtNode*>(stmt)->GetPrev();
          if (stmtPrev != nullptr && stmtPrev->GetOpCode() == OP_dassign) {
            DassignNode *dassignNode = static_cast<DassignNode*>(stmtPrev);
            if (dassignNode->GetRHS()->GetOpCode() == OP_retype) {
              CallNode *callNode = static_cast<CallNode*>(stmt);
              CHECK_FATAL(callNode->Opnd(0)->GetOpCode() == OP_dread, "Must be dread.");
              AddrofNode *dread = static_cast<AddrofNode*>(callNode->Opnd(0));
              if (dassignNode->GetStIdx() == dread->GetStIdx()) {
                RetypeNode *retypeNode = static_cast<RetypeNode *>(dassignNode->GetRHS());
                CHECK_FATAL(retypeNode->Opnd(0)->GetOpCode() == OP_dread, "Must be dread.");
                AddrofNode *dreadT = static_cast<AddrofNode*>(retypeNode->Opnd(0));
                MIRType *type = func->GetLocalOrGlobalSymbol(dreadT->GetStIdx())->GetType();
                CHECK_FATAL(type->IsMIRPtrType(), "Must be ptr type.");
                MIRPtrType *ptrType = static_cast<MIRPtrType*>(type);
                MIRType *targetType = ptrType->GetPointedType();
                MIRFunction *calleefuncT = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(calleePUIdx);
                GStrIdx calleeFuncStrIdx = calleefuncT->GetBaseFuncNameWithTypeStrIdx();
                Klass *klass = klassh->GetKlassFromTyIdx(targetType->GetTypeIndex());
                if (klass != nullptr) {
                  const MIRFunction *method = klass->GetMethod(calleeFuncStrIdx);
                  if (method != nullptr) {
                    calleePUIdx = method->GetPuidx();
                  } else {
                    std::string funcName = klass->GetKlassName();
                    funcName.append((NameMangler::kNameSplitterStr));
                    funcName.append(calleefuncT->GetBaseFuncNameWithType());
                    MIRFunction *methodT = mirBuilder->GetOrCreateFunction(funcName, (TyIdx) (PTY_void));
                    methodT->SetBaseClassNameStrIdx(klass->GetKlassNameStrIdx());
                    methodT->SetBaseFuncNameWithTypeStrIdx(calleeFuncStrIdx);
                    calleePUIdx = methodT->GetPuidx();
                  }
                }
              }
            }
          }
          // Add a call node whether or not the calleefunc has its body
          CGNode *calleeNode = GetOrGenCGNode(calleePUIdx, true);
          CHECK_FATAL(calleeNode != nullptr, "calleenode is null");
          CHECK_FATAL(calleeNode->IsVcallCandidatesValid(), "vcall candidate must be valid");
          node->AddCallsite(callInfo, &calleeNode->GetVcallCandidates());
          for (auto &cgIt : calleeNode->GetVcallCandidates()) {
            CGNode *calleeNodeT = cgIt;
            calleeNodeT->AddCaller(node);
          }
          break;
        }
        case kCallTypeInterfaceCall: {
          PUIdx calleePUIdx = (static_cast<CallNode*>(stmt))->GetPUIdx();
          MIRFunction *calleeFunc = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(calleePUIdx);
          CallInfo *callInfo = GenCallInfo(kCallTypeInterfaceCall, calleeFunc, stmt, loopDepth, stmt->GetStmtID());
          // Add a call node whether or not the calleeFunc has its body
          CGNode *calleeNode = GetOrGenCGNode(calleeFunc->GetPuidx(), false, true);
          CHECK_FATAL(calleeNode != nullptr, "calleenode is null");
          CHECK_FATAL(calleeNode->IsIcallCandidatesValid(), "icall candidate must be valid");
          node->AddCallsite(callInfo, &calleeNode->GetIcallCandidates());
          for (auto &cgIt : calleeNode->GetIcallCandidates()) {
            CGNode *calleeNodeT = cgIt;
            calleeNodeT->AddCaller(node);
          }
          break;
        }
        case kCallTypeCall: {
          PUIdx calleePUIdx = (static_cast<CallNode*>(stmt))->GetPUIdx();
          MIRFunction *calleeFunc = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(calleePUIdx);
          // Ignore clinit
          if (!calleeFunc->IsClinit()) {
            CallInfo *callInfo = GenCallInfo(kCallTypeCall, calleeFunc, stmt, loopDepth, stmt->GetStmtID());
            CGNode *calleeNode = GetOrGenCGNode(calleeFunc->GetPuidx());
            ASSERT(calleeNode != nullptr, "calleenode is null");
            calleeNode->AddCaller(node);
            node->AddCallsite(callInfo, calleeNode);
          }
          break;
        }
        case kCallTypeSuperCall: {
          PUIdx calleePUIdx = (static_cast<CallNode*>(stmt))->GetPUIdx();
          MIRFunction *calleeFunc = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(calleePUIdx);
          Klass *klass = klassh->GetKlassFromFunc(calleeFunc);
          if (klass == nullptr) {  // Fix CI
            continue;
          }
          ASSERT(klass != nullptr, "Klass not found");
          MapleVector<MIRFunction*> *cands = klass->GetCandidates(calleeFunc->GetBaseFuncNameWithTypeStrIdx());
          // continue to search its implinterfaces
          if (cands == nullptr) {
            for (Klass *implinterface : klass->GetImplInterfaces()) {
              cands = implinterface->GetCandidates(calleeFunc->GetBaseFuncNameWithTypeStrIdx());
              if (cands != nullptr && !cands->empty()) {
                break;
              }
            }
          }
          if (cands == nullptr || cands->empty()) {
            continue;  // Fix CI
          }
          MIRFunction *actualMirfunc = cands->at(0);
          CallInfo *callInfo = GenCallInfo(kCallTypeCall, actualMirfunc, stmt, loopDepth, stmt->GetStmtID());
          CGNode *calleeNode = GetOrGenCGNode(actualMirfunc->GetPuidx());
          ASSERT(calleeNode != nullptr, "calleenode is null");
          calleeNode->AddCaller(node);
          (static_cast<CallNode*>(stmt))->SetPUIdx(actualMirfunc->GetPuidx());
          node->AddCallsite(callInfo, calleeNode);
          break;
        }
        case kCallTypeIntrinsicCall:
        case kCallTypeIntrinsicCallWithType:
        case kCallTypeCustomCall:
        case kCallTypePolymorphicCall:
        case kCallTypeIcall:
        case kCallTypeXinitrinsicCall:
        case kCallTypeInvalid: {
          break;
        }
        default: {
          CHECK_FATAL(false, "TODO::unsupport call type");
        }
      }
    }
  }
}

void CallGraph::UpdateCallGraphNode(CGNode *node) {
  ASSERT_NOT_NULL(node);
  node->Reset();
  MIRFunction *func = node->GetMIRFunction();
  BlockNode *body = func->GetBody();
  HandleBody(func, body, node, 0);
}

void CallGraph::RecomputeSCC() {
  sccTopologicalVec.clear();
  numOfSccs = 0;
  BuildSCC();
}

void CallGraph::AddCallGraphNode(MIRFunction *func) {
  CGNode *node = GetOrGenCGNode(func->GetPuidx());
  CHECK_FATAL(node != nullptr, "node is null in CallGraph::GenCallGraph");
  BlockNode *body = func->GetBody();
  HandleBody(func, body, node, 0);
  /* set root if current function is static main */
  if (func->GetName() == mirModule->GetEntryFuncName()) {
    mirModule->SetEntryFunction(func);
    entry_node = node;
  }
}

static void ResetInferredType(std::vector<MIRSymbol*> &inferredSymbols) {
  for (unsigned int i = 0; i < inferredSymbols.size(); i++) {
    inferredSymbols[i]->SetInferredTyIdx(TyIdx());
  }
  inferredSymbols.clear();
}

static void ResetInferredType(std::vector<MIRSymbol*> &inferredSymbols, MIRSymbol *s) {
  if (s == nullptr) {
    return;
  }
  if (s->GetInferredTyIdx() == kInitTyIdx || s->GetInferredTyIdx() == kNoneTyIdx) {
    return;
  }
  unsigned int i = 0;
  for (; i < inferredSymbols.size(); i++) {
    if (inferredSymbols[i] == s) {
      s->SetInferredTyIdx(TyIdx());
      inferredSymbols.erase(inferredSymbols.begin() + i);
      break;
    }
  }
}

static void SetInferredType(std::vector<MIRSymbol*> &inferredSymbols, MIRSymbol *s, TyIdx idx) {
  s->SetInferredTyIdx(idx);
  unsigned int i = 0;
  for (; i < inferredSymbols.size(); i++) {
    if (inferredSymbols[i] == s) {
      break;
    }
  }
  if (i == inferredSymbols.size()) {
    inferredSymbols.push_back(s);
  }
}

void IPODevirtulize::SearchDefInClinit(const Klass *klass) {
  MIRClassType *classtype = static_cast<MIRClassType*>(klass->GetMIRStructType());
  std::vector<MIRSymbol*> staticFinalPrivateSymbols;
  for (uint32 i = 0; i < classtype->GetStaticFields().size(); i++) {
    FieldAttrs attribute = classtype->GetStaticFields()[i].second.second;
    if (attribute.GetAttr(FLDATTR_final)) {
      staticFinalPrivateSymbols.push_back(
        GlobalTables::GetGsymTable().GetSymbolFromStrIdx(classtype->GetStaticFields()[i].first));
    }
  }
  std::string typeName = klass->GetKlassName();
  typeName.append(NameMangler::kClinitSuffix);
  GStrIdx clinitFuncGstridx =
      GlobalTables::GetStrTable().GetStrIdxFromName(NameMangler::GetInternalNameLiteral(typeName));
  if (clinitFuncGstridx == 0u) {
    return;
  }
  MIRFunction *func = GlobalTables::GetGsymTable().GetSymbolFromStrIdx(clinitFuncGstridx)->GetFunction();
  if (!func->GetBody()) {
    return;
  }
  StmtNode *stmtNext = nullptr;
  std::vector<MIRSymbol*> gcmallocSymbols;
  for (StmtNode *stmt = func->GetBody()->GetFirst(); stmt != nullptr; stmt = stmtNext) {
    stmtNext = stmt->GetNext();
    Opcode op = stmt->GetOpCode();
    switch (op) {
      case OP_comment:
        break;
      case OP_dassign: {
        DassignNode *dassignNode = static_cast<DassignNode*>(stmt);
        MIRSymbol *leftSymbol = func->GetLocalOrGlobalSymbol(dassignNode->GetStIdx());
        unsigned i = 0;
        for (; i < staticFinalPrivateSymbols.size(); i++) {
          if (staticFinalPrivateSymbols[i] == leftSymbol) {
            break;
          }
        }
        if (i < staticFinalPrivateSymbols.size()) {
          if (dassignNode->GetRHS()->GetOpCode() == OP_dread) {
            DreadNode *dreadNode = static_cast<DreadNode*>(dassignNode->GetRHS());
            MIRSymbol *rightSymbol = func->GetLocalOrGlobalSymbol(dreadNode->GetStIdx());
            if (rightSymbol->GetInferredTyIdx() != kInitTyIdx && rightSymbol->GetInferredTyIdx() != kNoneTyIdx &&
                (staticFinalPrivateSymbols[i]->GetInferredTyIdx() == kInitTyIdx ||
                 (staticFinalPrivateSymbols[i]->GetInferredTyIdx() == rightSymbol->GetInferredTyIdx()))) {
              staticFinalPrivateSymbols[i]->SetInferredTyIdx(rightSymbol->GetInferredTyIdx());
            } else {
              staticFinalPrivateSymbols[i]->SetInferredTyIdx(kInitTyIdx);
              staticFinalPrivateSymbols.erase(staticFinalPrivateSymbols.begin() + i);
            }
          } else {
            staticFinalPrivateSymbols[i]->SetInferredTyIdx(kInitTyIdx);
            staticFinalPrivateSymbols.erase(staticFinalPrivateSymbols.begin() + i);
          }
        } else if (dassignNode->GetRHS()->GetOpCode() == OP_gcmalloc) {
          GCMallocNode *gcmallocNode = static_cast<GCMallocNode*>(dassignNode->GetRHS());
          TyIdx inferredTypeIdx = gcmallocNode->GetTyIdx();
          SetInferredType(gcmallocSymbols, leftSymbol, inferredTypeIdx);
        } else {
          ResetInferredType(gcmallocSymbols, leftSymbol);
        }
        break;
      }
      case OP_call:
      case OP_callassigned: {
        CallNode *cnode = static_cast<CallNode*>(stmt);
        MIRFunction *calleefunc = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(cnode->GetPUIdx());
        if (calleefunc->GetName().find(NameMangler::kClinitSubStr, 0) != std::string::npos) {
          // ignore all side effect of initizlizor
          continue;
        }
        for (unsigned int i = 0; i < cnode->GetReturnVec().size(); i++) {
          StIdx stidx = cnode->GetReturnVec()[i].first;
          MIRSymbol *tmpSymbol = func->GetLocalOrGlobalSymbol(stidx);
          ResetInferredType(gcmallocSymbols, tmpSymbol);
        }
        for (size_t i = 0; i < cnode->GetNopndSize(); i++) {
          BaseNode *node = cnode->GetNopndAt(i);
          if (node->GetOpCode() != OP_dread) {
            continue;
          }
          DreadNode *dreadNode = static_cast<DreadNode*>(node);
          MIRSymbol *tmpSymbol = func->GetLocalOrGlobalSymbol(dreadNode->GetStIdx());
          ResetInferredType(gcmallocSymbols, tmpSymbol);
        }
        break;
      }
      case OP_intrinsiccallwithtype: {
        IntrinsiccallNode *callnode = static_cast<IntrinsiccallNode*>(stmt);
        if (callnode->GetIntrinsic() != INTRN_JAVA_CLINIT_CHECK) {
          ResetInferredType(gcmallocSymbols);
        }
        break;
      }
      default:
        ResetInferredType(gcmallocSymbols);
        break;
    }
  }
}

void IPODevirtulize::SearchDefInMemberMethods(const Klass *klass) {
  SearchDefInClinit(klass);
  MIRClassType *classtype = static_cast<MIRClassType*>(klass->GetMIRStructType());
  std::vector<FieldID> finalPrivateFieldID;
  for (uint32 i = 0; i < classtype->GetFieldsSize(); i++) {
    FieldAttrs attribute = classtype->GetFields()[i].second.second;
    if (attribute.GetAttr(FLDATTR_final)) {
      FieldID id = mirBuilder->GetStructFieldIDFromFieldNameParentFirst(
        classtype, GlobalTables::GetStrTable().GetStringFromStrIdx(classtype->GetFields()[i].first));
      finalPrivateFieldID.push_back(id);
    }
  }
  std::vector<MIRFunction*> initMethods;
  std::string typeName = klass->GetKlassName();
  typeName.append(NameMangler::kCinitStr);
  for (MIRFunction *const &method : klass->GetMethods()) {
    if (!strncmp(method->GetName().c_str(), typeName.c_str(), typeName.length())) {
      initMethods.push_back(method);
    }
  }
  if (initMethods.empty()) {
    return;
  }
  ASSERT(!initMethods.empty(), "Must have initializor");
  StmtNode *stmtNext = nullptr;
  for (unsigned int i = 0; i < initMethods.size(); i++) {
    MIRFunction *func = initMethods[i];
    if (!func->GetBody()) {
      continue;
    }
    std::vector<MIRSymbol*> gcmallocSymbols;
    for (StmtNode *stmt = func->GetBody()->GetFirst(); stmt != nullptr; stmt = stmtNext) {
      stmtNext = stmt->GetNext();
      Opcode op = stmt->GetOpCode();
      switch (op) {
        case OP_comment:
          break;
        case OP_dassign: {
          DassignNode *dassignNode = static_cast<DassignNode*>(stmt);
          MIRSymbol *leftSymbol = func->GetLocalOrGlobalSymbol(dassignNode->GetStIdx());
          if (dassignNode->GetRHS()->GetOpCode() == OP_gcmalloc) {
            GCMallocNode *gcmallocNode = static_cast<GCMallocNode*>(dassignNode->GetRHS());
            SetInferredType(gcmallocSymbols, leftSymbol, gcmallocNode->GetTyIdx());
          } else if (dassignNode->GetRHS()->GetOpCode() == OP_retype) {
            RetypeNode *retystmt = static_cast<RetypeNode*>(dassignNode->GetRHS());
            BaseNode *fromnode = retystmt->Opnd(0);
            if (fromnode->GetOpCode() == OP_dread) {
              DreadNode *dreadNode = static_cast<DreadNode*>(fromnode);
              MIRSymbol *fromSymbol = func->GetLocalOrGlobalSymbol(dreadNode->GetStIdx());
              SetInferredType(gcmallocSymbols, leftSymbol, fromSymbol->GetInferredTyIdx());
            } else {
              ResetInferredType(gcmallocSymbols, leftSymbol);
            }
          } else {
            ResetInferredType(gcmallocSymbols, leftSymbol);
          }
          break;
        }
        case OP_call:
        case OP_callassigned: {
          CallNode *cnode = static_cast<CallNode*>(stmt);
          MIRFunction *calleefunc = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(cnode->GetPUIdx());
          if (calleefunc->GetName().find(NameMangler::kClinitSubStr, 0) != std::string::npos) {
            // ignore all side effect of initizlizor
            continue;
          }
          for (size_t j = 0; j < cnode->GetReturnVec().size(); j++) {
            StIdx stidx = cnode->GetReturnVec()[j].first;
            MIRSymbol *tmpSymbol = func->GetLocalOrGlobalSymbol(stidx);
            ResetInferredType(gcmallocSymbols, tmpSymbol);
          }
          for (size_t j = 0; j < cnode->GetNopndSize(); j++) {
            BaseNode *node = cnode->GetNopndAt(j);
            if (node->GetOpCode() != OP_dread) {
              continue;
            }
            DreadNode *dreadNode = static_cast<DreadNode*>(node);
            MIRSymbol *tmpSymbol = func->GetLocalOrGlobalSymbol(dreadNode->GetStIdx());
            ResetInferredType(gcmallocSymbols, tmpSymbol);
          }
          break;
        }
        case OP_intrinsiccallwithtype: {
          IntrinsiccallNode *callnode = static_cast<IntrinsiccallNode*>(stmt);
          if (callnode->GetIntrinsic() != INTRN_JAVA_CLINIT_CHECK) {
            ResetInferredType(gcmallocSymbols);
          }
          break;
        }
        case OP_iassign: {
          IassignNode *iassignNode = static_cast<IassignNode*>(stmt);
          MIRType *type = GlobalTables::GetTypeTable().GetTypeFromTyIdx(iassignNode->GetTyIdx());
          ASSERT(type->GetKind() == kTypePointer, "Must be pointer type");
          MIRPtrType *pointedType = static_cast<MIRPtrType*>(type);
          if (pointedType->GetPointedTyIdx() == classtype->GetTypeIndex()) {
            // set field of current class
            FieldID fieldID = iassignNode->GetFieldID();
            unsigned j = 0;
            for (; j < finalPrivateFieldID.size(); j++) {
              if (finalPrivateFieldID[j] == fieldID) {
                break;
              }
            }
            if (j < finalPrivateFieldID.size()) {
              if (iassignNode->GetRHS()->GetOpCode() == OP_dread) {
                DreadNode *dreadNode = static_cast<DreadNode*>(iassignNode->GetRHS());
                CHECK_FATAL(dreadNode != nullptr, "Impossible");
                MIRSymbol *rightSymbol = func->GetLocalOrGlobalSymbol(dreadNode->GetStIdx());
                if (rightSymbol->GetInferredTyIdx() != kInitTyIdx && rightSymbol->GetInferredTyIdx() != kNoneTyIdx &&
                    (classtype->GetElemInferredTyIdx(fieldID) == kInitTyIdx ||
                     (classtype->GetElemInferredTyIdx(fieldID) == rightSymbol->GetInferredTyIdx()))) {
                  classtype->SetElemInferredTyIdx(fieldID, rightSymbol->GetInferredTyIdx());
                } else {
                  classtype->SetElemInferredTyIdx(fieldID, kInitTyIdx);
                  finalPrivateFieldID.erase(finalPrivateFieldID.begin() + j);
                }
              } else {
                classtype->SetElemInferredTyIdx(fieldID, kInitTyIdx);
                finalPrivateFieldID.erase(finalPrivateFieldID.begin() + j);
              }
            }
          }
          break;
        }
        default:
          ResetInferredType(gcmallocSymbols);
          break;
      }
    }
  }
}

void DoDevirtual(const Klass *klass, const KlassHierarchy *klassh) {
  MIRClassType *classtype = static_cast<MIRClassType*>(klass->GetMIRStructType());
  for (auto &func : klass->GetMethods()) {
    if (func->GetBody() == nullptr) {
      continue;
    }
    StmtNode *stmtNext = nullptr;
    std::vector<MIRSymbol*> inferredSymbols;
    for (StmtNode *stmt = func->GetBody()->GetFirst(); stmt != nullptr; stmt = stmtNext) {
      stmtNext = stmt->GetNext();
      Opcode op = stmt->GetOpCode();
      switch (op) {
        case OP_comment:
        case OP_assertnonnull:
        case OP_brtrue:
        case OP_brfalse:
          break;
        case OP_dassign: {
          DassignNode *dassignNode = static_cast<DassignNode*>(stmt);
          MIRSymbol *leftSymbol = func->GetLocalOrGlobalSymbol(dassignNode->GetStIdx());
          if (dassignNode->GetRHS()->GetOpCode() == OP_dread) {
            DreadNode *dreadNode = static_cast<DreadNode*>(dassignNode->GetRHS());
            if (func->GetLocalOrGlobalSymbol(dreadNode->GetStIdx())->GetInferredTyIdx() != kInitTyIdx) {
              SetInferredType(inferredSymbols, leftSymbol,
                              func->GetLocalOrGlobalSymbol(dreadNode->GetStIdx())->GetInferredTyIdx());
            }
          } else if (dassignNode->GetRHS()->GetOpCode() == OP_iread) {
            IreadNode *ireadNode = static_cast<IreadNode*>(dassignNode->GetRHS());
            MIRType *type = GlobalTables::GetTypeTable().GetTypeFromTyIdx(ireadNode->GetTyIdx());
            ASSERT(type->GetKind() == kTypePointer, "Must be pointer type");
            MIRPtrType *pointedType = static_cast<MIRPtrType*>(type);
            if (pointedType->GetPointedTyIdx() == classtype->GetTypeIndex()) {
              FieldID fieldID = ireadNode->GetFieldID();
              FieldID tmpID = fieldID;
              TyIdx tmpTyIdx = classtype->GetElemInferredTyIdx(tmpID);
              if (tmpTyIdx != kInitTyIdx && tmpTyIdx != kNoneTyIdx) {
                SetInferredType(inferredSymbols, leftSymbol, classtype->GetElemInferredTyIdx(fieldID));
              }
            }
          } else {
            ResetInferredType(inferredSymbols, leftSymbol);
          }
          break;
        }
        case OP_interfacecall:
        case OP_interfacecallassigned:
        case OP_virtualcall:
        case OP_virtualcallassigned: {
          CallNode *calleeNode = static_cast<CallNode*>(stmt);
          MIRFunction *calleefunc = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(calleeNode->GetPUIdx());
          if (calleeNode->GetNopndAt(0)->GetOpCode() == OP_dread) {
            DreadNode *dreadNode = static_cast<DreadNode*>(calleeNode->GetNopndAt(0));
            MIRSymbol *rightSymbol = func->GetLocalOrGlobalSymbol(dreadNode->GetStIdx());
            if (rightSymbol->GetInferredTyIdx() != kInitTyIdx && rightSymbol->GetInferredTyIdx() != kNoneTyIdx) {
              // Devirtual
              Klass *currKlass = klassh->GetKlassFromTyIdx(rightSymbol->GetInferredTyIdx());
              if (op == OP_interfacecall || op == OP_interfacecallassigned || op == OP_virtualcall ||
                  op == OP_virtualcallassigned) {
                std::vector<Klass*> klassVector;
                klassVector.push_back(currKlass);
                bool hasDevirtualed = false;
                for (unsigned int index = 0; index < klassVector.size(); index++) {
                  Klass *tmpKlass = klassVector[index];
                  for (MIRFunction *const &method : tmpKlass->GetMethods()) {
                    if (calleefunc->GetBaseFuncNameWithTypeStrIdx() == method->GetBaseFuncNameWithTypeStrIdx()) {
                      calleeNode->SetPUIdx(method->GetPuidx());
                      if (op == OP_virtualcall || op == OP_interfacecall) {
                        calleeNode->SetOpCode(OP_call);
                      }
                      if (op == OP_virtualcallassigned || op == OP_interfacecallassigned) {
                        calleeNode->SetOpCode(OP_callassigned);
                      }
                      hasDevirtualed = true;
                      if (false) {
                        LogInfo::MapleLogger() << "Devirtualize In function:" + func->GetName() << '\n';
                        LogInfo::MapleLogger() << calleeNode->GetOpCode() << '\n';
                        LogInfo::MapleLogger() << "    From:" << calleefunc->GetName() << '\n';
                        LogInfo::MapleLogger() << "    To  :"
                            << GlobalTables::GetFunctionTable().GetFunctionFromPuidx(calleeNode->GetPUIdx())->GetName()
                            << '\n';
                      }
                      break;
                    }
                  }
                  if (hasDevirtualed) {
                    break;
                  }
                  // add subclass of currKlass into vecotr
                  for (Klass *superKlass : tmpKlass->GetSuperKlasses()) {
                    klassVector.push_back(superKlass);
                  }
                }
                if (hasDevirtualed) {
                  for (size_t i = 0; i < calleeNode->GetNopndSize(); i++) {
                    BaseNode *node = calleeNode->GetNopndAt(i);
                    if (node->GetOpCode() != OP_dread) {
                      continue;
                    }
                    dreadNode = static_cast<DreadNode*>(node);
                    MIRSymbol *tmpSymbol = func->GetLocalOrGlobalSymbol(dreadNode->GetStIdx());
                    ResetInferredType(inferredSymbols, tmpSymbol);
                  }
                  if (op == OP_interfacecallassigned || op == OP_virtualcallassigned) {
                    CallNode *cnode = static_cast<CallNode*>(stmt);
                    for (unsigned int i = 0; i < cnode->GetReturnVec().size(); i++) {
                      StIdx stidx = cnode->GetReturnVec()[i].first;
                      MIRSymbol *tmpSymbol = func->GetLocalOrGlobalSymbol(stidx);
                      ResetInferredType(inferredSymbols, tmpSymbol);
                    }
                  }
                  break;
                }
                // Search default function in interfaces
                Klass *tmpInterface = nullptr;
                MIRFunction *tmpMethod = nullptr;
                for (Klass *iklass : currKlass->GetImplInterfaces()) {
                  for (MIRFunction *const &method : iklass->GetMethods()) {
                    if (calleefunc->GetBaseFuncNameWithTypeStrIdx() == method->GetBaseFuncNameWithTypeStrIdx() &&
                        !method->GetFuncAttrs().GetAttr(FUNCATTR_abstract)) {
                      if (tmpInterface == nullptr || klassh->IsSuperKlassForInterface(tmpInterface, iklass)) {
                        tmpInterface = iklass;
                        tmpMethod = method;
                      }
                      break;
                    }
                  }
                }
                // Add this check for the thirdparty APP compile
                if (tmpMethod == nullptr) {
                  Klass *parentKlass = klassh->GetKlassFromName(calleefunc->GetBaseClassName());
                  CHECK_FATAL(parentKlass != nullptr, "null ptr check");
                  bool flag = false;
                  if (parentKlass->GetKlassName() == currKlass->GetKlassName()) {
                    flag = true;
                  } else {
                    for (Klass *const &superclass : currKlass->GetSuperKlasses()) {
                      if (parentKlass->GetKlassName() == superclass->GetKlassName()) {
                        flag = true;
                        break;
                      }
                    }
                    if (!flag && parentKlass->IsInterface()) {
                      for (Klass *const &implClass : currKlass->GetImplKlasses()) {
                        if (parentKlass->GetKlassName() == implClass->GetKlassName()) {
                          flag = true;
                          break;
                        }
                      }
                    }
                  }
                  if (!flag) {
                    LogInfo::MapleLogger() << "warning: func " << calleefunc->GetName() << " is not found in DeVirtual!"
                                           << std::endl;
                    LogInfo::MapleLogger() << "warning: " << calleefunc->GetBaseClassName() << " is not the parent of "
                                           << currKlass->GetKlassName() << std::endl;
                  }
                }
                if (tmpMethod == nullptr) {  // SearchWithoutRettype, search only in current class now.
                  MIRType *retType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(calleefunc->GetReturnTyIdx());
                  Klass *targetKlass = nullptr;
                  bool isCalleeScalar = false;
                  if (retType->GetKind() == kTypePointer && retType->GetPrimType() == PTY_ref) {
                    MIRType *ptrType = (static_cast<MIRPtrType*>(retType))->GetPointedType();
                    targetKlass = klassh->GetKlassFromTyIdx(ptrType->GetTypeIndex());
                  } else if (retType->GetKind() == kTypeScalar) {
                    isCalleeScalar = true;
                  } else {
                    targetKlass = klassh->GetKlassFromTyIdx(retType->GetTypeIndex());
                  }
                  if (targetKlass == nullptr && !isCalleeScalar) {
                    CHECK_FATAL(targetKlass != nullptr, "null ptr check");
                  }
                  Klass *curRetKlass = nullptr;
                  bool isCurrVtabScalar = false;
                  bool isFindMethod = false;
                  for (MIRFunction *const &method : currKlass->GetMethods()) {
                    if (calleefunc->GetBaseFuncSigStrIdx() == method->GetBaseFuncSigStrIdx()) {
                      Klass *tmpKlass = nullptr;
                      MIRType *tmpType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(method->GetReturnTyIdx());
                      if (tmpType->GetKind() == kTypePointer && tmpType->GetPrimType() == PTY_ref) {
                        MIRType *ptrType = (static_cast<MIRPtrType*>(tmpType))->GetPointedType();
                        tmpKlass = klassh->GetKlassFromTyIdx(ptrType->GetTypeIndex());
                      } else if (tmpType->GetKind() == kTypeScalar) {
                        isCurrVtabScalar = true;
                      } else {
                        tmpKlass = klassh->GetKlassFromTyIdx(tmpType->GetTypeIndex());
                      }
                      if (tmpKlass == nullptr && !isCurrVtabScalar) {
                        CHECK_FATAL(false, "null ptr check");
                      }
                      if (isCalleeScalar || isCurrVtabScalar) {
                        if (isFindMethod) {
                          LogInfo::MapleLogger() << "warning: this " << currKlass->GetKlassName()
                              << " has mult methods with the same function name but with different return type!"
                              << std::endl;
                          break;
                        }
                        tmpMethod = method;
                        isFindMethod = true;
                        continue;
                      }
                      if (targetKlass->IsClass() && klassh->IsSuperKlass(tmpKlass, targetKlass) &&
                          (curRetKlass == nullptr || klassh->IsSuperKlass(curRetKlass, tmpKlass))) {
                        curRetKlass = tmpKlass;
                        tmpMethod = method;
                      }
                      if (targetKlass->IsClass() && klassh->IsInterfaceImplemented(tmpKlass, targetKlass)) {
                        tmpMethod = method;
                        break;
                      }
                      if (!targetKlass->IsClass()) {
                        CHECK_FATAL(tmpKlass != nullptr, "Klass null ptr check");
                        if (tmpKlass->IsClass() && klassh->IsInterfaceImplemented(targetKlass, tmpKlass) &&
                            (curRetKlass == nullptr || klassh->IsSuperKlass(curRetKlass, tmpKlass))) {
                          curRetKlass = tmpKlass;
                          tmpMethod = method;
                        }
                        if (!tmpKlass->IsClass() && klassh->IsSuperKlassForInterface(tmpKlass, targetKlass) &&
                            (curRetKlass == nullptr || klassh->IsSuperKlass(curRetKlass, tmpKlass))) {
                          curRetKlass = tmpKlass;
                          tmpMethod = method;
                        }
                      }
                    }
                  }
                }
                if (tmpMethod == nullptr && (currKlass->IsClass() || currKlass->IsInterface())) {
                  LogInfo::MapleLogger() << "warning: func " << calleefunc->GetName()
                                         << " is not found in DeVirtual!" << std::endl;
                  stmt->SetOpCode(OP_callassigned);
                  break;
                } else if (tmpMethod == nullptr) {
                  LogInfo::MapleLogger() << "Error: func " << calleefunc->GetName() << " is not found!" << std::endl;
                  ASSERT(tmpMethod, "Must not be null");
                }
                calleeNode->SetPUIdx(tmpMethod->GetPuidx());
                if (op == OP_virtualcall || op == OP_interfacecall) {
                  calleeNode->SetOpCode(OP_call);
                }
                if (op == OP_virtualcallassigned || op == OP_interfacecallassigned) {
                  calleeNode->SetOpCode(OP_callassigned);
                }
                if (false) {
                  LogInfo::MapleLogger() << "Devirtualize In function:" + func->GetName() << '\n';
                  LogInfo::MapleLogger() << calleeNode->GetOpCode() << '\n';
                  LogInfo::MapleLogger() << "    From:" << calleefunc->GetName() << '\n';
                  LogInfo::MapleLogger() << "    To  :"
                      << GlobalTables::GetFunctionTable().GetFunctionFromPuidx(calleeNode->GetPUIdx())->GetName()
                      << '\n';
                }
                for (size_t i = 0; i < calleeNode->GetNopndSize(); i++) {
                  BaseNode *node = calleeNode->GetNopndAt(i);
                  if (node->GetOpCode() != OP_dread) {
                    continue;
                  }
                  dreadNode = static_cast<DreadNode*>(node);
                  MIRSymbol *tmpSymbol = func->GetLocalOrGlobalSymbol(dreadNode->GetStIdx());
                  ResetInferredType(inferredSymbols, tmpSymbol);
                }
                if (op == OP_interfacecallassigned || op == OP_virtualcallassigned) {
                  CallNode *cnode = static_cast<CallNode*>(stmt);
                  for (unsigned int i = 0; i < cnode->GetReturnVec().size(); i++) {
                    StIdx stidx = cnode->GetReturnVec()[i].first;
                    MIRSymbol *tmpSymbol = func->GetLocalOrGlobalSymbol(stidx);
                    ResetInferredType(inferredSymbols, tmpSymbol);
                  }
                }
                break;
              }
            }
          }
        }
        case OP_call:
        case OP_callassigned: {
          CallNode *cnode = static_cast<CallNode*>(stmt);
          for (size_t i = 0; i < cnode->GetReturnVec().size(); i++) {
            StIdx stidx = cnode->GetReturnVec()[i].first;
            MIRSymbol *tmpSymbol = func->GetLocalOrGlobalSymbol(stidx);
            ResetInferredType(inferredSymbols, tmpSymbol);
          }
          for (size_t i = 0; i < cnode->GetNopndSize(); i++) {
            BaseNode *node = cnode->GetNopndAt(i);
            if (node->GetOpCode() != OP_dread) {
              continue;
            }
            DreadNode *dreadNode = static_cast<DreadNode*>(node);
            MIRSymbol *tmpSymbol = func->GetLocalOrGlobalSymbol(dreadNode->GetStIdx());
            ResetInferredType(inferredSymbols, tmpSymbol);
          }
          break;
        }
        default:
          ResetInferredType(inferredSymbols);
          break;
      }
    }
  }
}

void IPODevirtulize::DevirtualFinal() {
  // Search all klass in order to find final variables
  MapleMap<GStrIdx, Klass*>::const_iterator it = klassh->GetKlasses().begin();
  for (; it != klassh->GetKlasses().end(); ++it) {
    Klass *klass = it->second;
    if (klass->IsClass()) {
      MIRClassType *classtype = static_cast<MIRClassType*>(klass->GetMIRStructType());
      // Initialize inferred type of member fileds as kInitTyidx
      for (unsigned int i = 0; i < classtype->GetFieldsSize(); i++) {  // Don't include parent's field
        classtype->SetElemInferredTyIdx(i, kInitTyIdx);
      }
      SearchDefInMemberMethods(klass);
      for (unsigned int i = 0; i < classtype->GetFieldInferredTyIdx().size(); i++) {
        if (classtype->GetElemInferredTyIdx(i) != kInitTyIdx && classtype->GetElemInferredTyIdx(i) != kNoneTyIdx) {
          if (debugFlag) {
            FieldID tmpID = i;
            FieldPair pair = classtype->TraverseToFieldRef(tmpID);
            LogInfo::MapleLogger() << "Inferred Final Private None-Static Variable:" + klass->GetKlassName() + ":" +
                        GlobalTables::GetStrTable().GetStringFromStrIdx(pair.first)
                 << '\n';
          }
        }
      }
      for (uint32 i = 0; i < classtype->GetStaticFields().size(); i++) {
        FieldAttrs attribute = classtype->GetStaticFields()[i].second.second;
        if (!GlobalTables::GetGsymTable().GetSymbolFromStrIdx(classtype->GetStaticFields()[i].first)) {
          continue;
        }
        if (GlobalTables::GetGsymTable().GetSymbolFromStrIdx(classtype->GetStaticFields()[i].first)->GetInferredTyIdx()
            != kInitTyIdx &&
            GlobalTables::GetGsymTable().GetSymbolFromStrIdx(classtype->GetStaticFields()[i].first)->GetInferredTyIdx()
            != kNoneTyIdx) {
          CHECK_FATAL(attribute.GetAttr(FLDATTR_final), "Must be final private");
          if (debugFlag) {
            LogInfo::MapleLogger() << "Final Private Static Variable:" +
                        GlobalTables::GetStrTable().GetStringFromStrIdx(classtype->GetStaticFields()[i].first)
                 << '\n';
          }
        }
      }
      DoDevirtual(klass, GetKlassh());
    }
  }
}

void CallGraph::GenCallGraph() {
  // Read existing call graph from mplt, std::map<PUIdx, std::vector<CallInfo*> >
  // caller_PUIdx and all call site info are needed. Rebuild all other info of CGNode using CHA
  for (auto const &it : mirModule->GetMethod2TargetMap()) {
    CGNode *node = GetOrGenCGNode(it.first);
    CHECK_FATAL(node != nullptr, "node is null");
    std::vector<CallInfo*> callees = it.second;
    for (auto itInner = callees.begin(); itInner != callees.end(); ++itInner) {
      CallInfo *info = *itInner;
      CGNode *calleeNode = GetOrGenCGNode(info->GetFunc()->GetPuidx(), info->GetCallType() == kCallTypeVirtualCall,
                                          info->GetCallType() == kCallTypeInterfaceCall);
      CHECK_FATAL(calleeNode != nullptr, "calleeNode is null");
      if (info->GetCallType() == kCallTypeVirtualCall) {
        node->AddCallsite(*itInner, &calleeNode->GetVcallCandidates());
      } else if (info->GetCallType() == kCallTypeInterfaceCall) {
        node->AddCallsite(*itInner, &calleeNode->GetIcallCandidates());
      } else if (info->GetCallType() == kCallTypeCall) {
        node->AddCallsite(*itInner, calleeNode);
      } else if (info->GetCallType() == kCallTypeSuperCall) {
        const MIRFunction *calleefunc = info->GetFunc();
        Klass *klass = klassh->GetKlassFromFunc(calleefunc);
        if (klass == nullptr) {  // Fix CI
          continue;
        }
        MapleVector<MIRFunction*> *cands = klass->GetCandidates(calleefunc->GetBaseFuncNameWithTypeStrIdx());
        // continue to search its implinterfaces
        if (cands == nullptr) {
          for (Klass *implinterface : klass->GetImplInterfaces()) {
            cands = implinterface->GetCandidates(calleefunc->GetBaseFuncNameWithTypeStrIdx());
            if (cands && !cands->empty()) {
              break;
            }
          }
        }
        if (cands == nullptr || cands->empty()) {
          continue;  // Fix CI
        }
        MIRFunction *actualMirfunc = cands->at(0);
        CGNode *tempNode = GetOrGenCGNode(actualMirfunc->GetPuidx());
        ASSERT(tempNode != nullptr, "calleenode is null in CallGraph::HandleBody");
        node->AddCallsite(info, tempNode);
      }
      for (auto &callSite : node->GetCallee()) {
        if (callSite.first == info) {
          for (auto &cgIt : *callSite.second) {
            CGNode *tempNode = cgIt;
            tempNode->AddCaller(node);
          }
          break;
        }
      }
    }
  }
  // Deal with function override, function in current module override functions from mplt.
  // Don't need anymore as we rebuild candidate base on the latest CHA.
  for (auto it = GlobalTables::GetFunctionTable().GetFuncTable().begin();
       it != GlobalTables::GetFunctionTable().GetFuncTable().end(); it++) {
    MIRFunction *mirFunc = *it;
    if (mirFunc == nullptr || mirFunc->GetBody() == nullptr) {
      continue;
    }
    mirModule->SetCurFunction(mirFunc);
    AddCallGraphNode(mirFunc);
  }
  // Add all root nodes
  FindRootNodes();
  BuildSCC();
}

void CallGraph::FindRootNodes() {
  if (!rootNodes.empty()) {
    CHECK_FATAL(false, "rootNodes has already been set");
  }
  for (auto const &it : nodesMap) {
    CGNode *node = it.second;
    if (!node->HasCaller()) {
      rootNodes.push_back(node);
    }
  }
}

void CallGraph::Dump() const {
  for (auto const &it : nodesMap) {
    CGNode *node = it.second;
    node->DumpDetail();
  }
}

void CallGraph::DumpToFile(bool dumpall) {
  if (Options::noDot) {
    return;
  }
  std::ofstream cgfile;
  char *outName = nullptr;
  MapleString outfile(fileName, GetMempool());
  if (dumpall) {
    outName = (outfile.append("-callgraph.dot")).c_str();
  } else {
    outName = (outfile.append("-callgraphlight.dot")).c_str();
  }
  cgfile.open(outName, std::ios::trunc);
  cgfile << "digraph graphname {\n";
  for (auto const &it : nodesMap) {
    CGNode *node = it.second;
    // dump user defined function
    if (dumpall) {
      node->Dump(cgfile);
    } else {
      if ((node->GetMIRFunction() != nullptr) && (!node->GetMIRFunction()->IsEmpty())) {
        node->Dump(cgfile);
      }
    }
  }
  cgfile << "}\n";
  cgfile.close();
}

void CallGraph::BuildCallGraph() {
  GenCallGraph();
  // Dump callgraph to dot file
  if (debug_flag) {
    DumpToFile(true);
  }
  SetCompilationFunclist();
}

// Sort CGNode within an SCC. Try best to arrange callee appears before
// its (direct) caller, so that caller can benefit from summary info.
// If we have iterative inter-procedure analysis, then would not bother
// do this.
static bool CGNodeCompare(CGNode *left, CGNode *right) {
  // special case: left calls right and right calls left, then compare by id
  if (left->IsCalleeOf(right) && right->IsCalleeOf(left)) {
    return left->GetID() < right->GetID();
  }
  // left is right's direct callee, then make left appears first
  if (left->IsCalleeOf(right)) {
    return true;
  } else if (right->IsCalleeOf(left)) {
    return false;
  }
  return left->GetID() < right->GetID();
}

// Set compilation order as the bottom-up order of callgraph. So callee
// is always compiled before caller. This benifits thoses optimizations
// need interprocedure information like escape analysis.
void CallGraph::SetCompilationFunclist() {
  mirModule->GetCompilationList().clear();
  mirModule->GetFunctionList().clear();
  const MapleVector<SCCNode*> &sccTopVec = GetSCCTopVec();
  for (int i = sccTopVec.size() - 1; i >= 0; i--) {
    SCCNode *sccNode = sccTopVec[i];
    std::sort(sccNode->cgNodes.begin(), sccNode->cgNodes.end(), CGNodeCompare);
    for (auto const kIt : sccNode->cgNodes) {
      CGNode *node = kIt;
      MIRFunction *func = node->GetMIRFunction();
      if ((func && func->GetBody() && !IsInIPA()) || (func && !func->IsNative())) {
        mirModule->GetCompilationList().push_back(func);
        mirModule->GetFunctionList().push_back(func);
      }
    }
  }
  if ((mirModule->GetCompilationList().size() != mirModule->GetFunctionList().size() &&
       mirModule->GetCompilationList().size() != mirModule->GetFunctionList().size() - mirModule->GetOptFuncsSize())) {
    CHECK_FATAL(false, "should be equal");
  }
}

bool SCCNode::HasRecursion() const {
  if (cgNodes.empty()) {
    return false;
  }
  if (cgNodes.size() > 1) {
    return true;
  }
  CGNode *node = cgNodes[0];
  for (auto &callSite : node->GetCallee()) {
    for (auto &cgIt : *callSite.second) {
      CGNode *calleeNode = cgIt;
      if (calleeNode == nullptr) {
        continue;
      }
      if (node == calleeNode) {
        return true;
      }
    }
  }
  return false;
}

bool SCCNode::HasSelfRecursion() const {
  if (cgNodes.size() != 1) {
    return false;
  }
  CGNode *node = cgNodes[0];
  for (auto &callSite : node->GetCallee()) {
    for (auto &cgIt : *callSite.second) {
      CGNode *calleeNode = cgIt;
      if (calleeNode == nullptr) {
        continue;
      }
      if (node == calleeNode) {
        return true;
      }
    }
  }
  return false;
}

void SCCNode::Dump() {
  printf("SCC %d contains\n", id);
  for (auto const kIt : cgNodes) {
    CGNode *node = kIt;
    if (node->GetMIRFunction()) {
      printf("  function(%d): %s\n", node->GetMIRFunction()->GetPuidx(), node->GetMIRFunction()->GetName().c_str());
    } else {
      printf("  function: external\n");
    }
  }
}

void SCCNode::DumpCycle() {
  CGNode *currNode = cgNodes[0];
  std::vector<CGNode*> searched;
  searched.push_back(currNode);
  std::vector<CGNode*> invalidNodes;
  while (true) {
    bool findNewCallee = false;
    for (auto &callSite : currNode->GetCallee()) {
      for (auto &cgIt : *callSite.second) {
        CGNode *calleeNode = cgIt;
        if (calleeNode->GetSCCNode() == this) {
          unsigned int j = 0;
          for (; j < invalidNodes.size(); j++) {
            if (invalidNodes[j] == calleeNode) {
              break;
            }
          }
          // Find a invalid node
          if (j < invalidNodes.size()) {
            continue;
          }
          for (j = 0; j < searched.size(); j++) {
            if (searched[j] == calleeNode) {
              break;
            }
          }
          if (j == searched.size()) {
            currNode = calleeNode;
            searched.push_back(currNode);
            findNewCallee = true;
            break;
          }
        }
      }
    }
    if (searched.size() == cgNodes.size()) {
      break;
    }
    if (!findNewCallee) {
      invalidNodes.push_back(searched[searched.size() - 1]);
      searched.pop_back();
      currNode = searched[searched.size() - 1];
    }
  }
  for (auto it = searched.begin(); it != searched.end(); it++) {
    LogInfo::MapleLogger() << (*it)->GetMIRFunction()->GetName() << '\n';
  }
}

void SCCNode::Verify() {
  if (cgNodes.size() <= 0) {
    CHECK_FATAL(false, "");
  }
  for (CGNode *const &node : cgNodes) {
    if (node->GetSCCNode() != this) {
      CHECK_FATAL(false, "");
    }
  }
}

void SCCNode::Setup() {
  for (CGNode *const &node : cgNodes) {
    for (auto &callSite : node->GetCallee()) {
      for (auto &cgIt : *callSite.second) {
        CGNode *calleeNode = cgIt;
        if (calleeNode == nullptr) {
          continue;
        }
        if (calleeNode->GetSCCNode() == this) {
          continue;
        }
        calleeScc.insert(calleeNode->GetSCCNode());
      }
    }
    for (auto itCaller = node->CallerBegin(); itCaller != node->CallerEnd(); itCaller++) {
      CGNode *callerNode = *itCaller;
      if (callerNode->GetSCCNode() == this) {
        continue;
      }
      callerScc.insert(callerNode->GetSCCNode());
    }
  }
}

void CallGraph::BuildSCCDFS(CGNode *caller, uint32 &visitIndex, std::vector<SCCNode*> &sccNodes,
                            std::vector<CGNode*> &cgNodes, std::vector<uint32> &visitedOrder,
                            std::vector<uint32> &lowestOrder, std::vector<bool> &inStack,
                            std::vector<uint32> &visitStack) {
  uint32 id = caller->GetID();
  cgNodes.at(id) = caller;
  visitedOrder.at(id) = visitIndex;
  lowestOrder.at(id) = visitIndex;
  visitIndex++;
  visitStack.push_back(id);
  inStack.at(id) = true;
  for (auto &callSite : caller->GetCallee()) {
    for (auto &cgIt : *callSite.second) {
      CGNode *calleeNode = cgIt;
      if (calleeNode == nullptr) {
        continue;
      }
      uint32 calleeId = calleeNode->GetID();
      if (!visitedOrder.at(calleeId)) {
        // callee has not been processed yet
        BuildSCCDFS(calleeNode, visitIndex, sccNodes, cgNodes, visitedOrder, lowestOrder, inStack, visitStack);
        if (lowestOrder.at(calleeId) < lowestOrder.at(id)) {
          lowestOrder.at(id) = lowestOrder.at(calleeId);
        }
      } else if (inStack.at(calleeId) && visitedOrder.at(calleeId) < lowestOrder.at(id)) {
        // back edge
        lowestOrder.at(id) = visitedOrder.at(calleeId);
      }
    }
  }
  if (visitedOrder.at(id) == lowestOrder.at(id)) {
    SCCNode *sccNode = cgalloc.GetMemPool()->New<SCCNode>(numOfSccs++, &cgalloc);
    uint32 stackTopId;
    do {
      stackTopId = visitStack.back();
      visitStack.pop_back();
      inStack.at(stackTopId) = false;
      CGNode *topNode = cgNodes.at(stackTopId);
      topNode->SetSCCNode(sccNode);
      sccNode->AddCGNode(topNode);
    } while (stackTopId != id);
    sccNodes.push_back(sccNode);
  }
}

void CallGraph::VerifySCC() {
  for (auto const &it : nodesMap) {
    CGNode *node = it.second;
    if (node->GetSCCNode() == nullptr) {
      CHECK_FATAL(false, "");
    }
  }
}

void CallGraph::BuildSCC() {
  // This is the mapping between cg_id to cg_node. We may consider putting this in the CallGraph if it will be used
  // frenqutenly in the future.
  std::vector<CGNode*> cgNodes(numOfNodes, nullptr);
  std::vector<uint32> visitedOrder(numOfNodes, 0);
  std::vector<uint32> lowestOrder(numOfNodes, 0);
  std::vector<bool> inStack(numOfNodes, false);
  std::vector<SCCNode*> sccNodes;
  uint32 visitIndex = 1;
  std::vector<uint32> visitStack;
  // Starting from roots is a good strategy for DSF
  for (CGNode *const &root : rootNodes) {
    BuildSCCDFS(root, visitIndex, sccNodes, cgNodes, visitedOrder, lowestOrder, inStack, visitStack);
  }
  // However, not all SCC can be reached from roots.
  // E.g. foo()->foo(), foo is not considered as a root.
  for (auto const &it : nodesMap) {
    CGNode *node = it.second;
    if (!node->GetSCCNode()) {
      BuildSCCDFS(node, visitIndex, sccNodes, cgNodes, visitedOrder, lowestOrder, inStack, visitStack);
    }
  }
  for (SCCNode *const &scc : sccNodes) {
    // LogInfo::MapleLogger() << "SCC:" << scc->cgNodes.size() << '\n';
    scc->Verify();
    scc->Setup();  // fix caller and callee info.
    if (debug_scc && scc->HasRecursion()) {
      scc->Dump();
    }
  }
  SCCTopologicalSort(sccNodes);
}

void CallGraph::SCCTopologicalSort(std::vector<SCCNode*> &sccNodes) {
  std::set<SCCNode*, Comparator<SCCNode>> inQueue;  // Local variable, no need to use MapleSet
  for (SCCNode *const &node : sccNodes) {
    if (!node->HasCaller()) {
      sccTopologicalVec.push_back(node);
      inQueue.insert(node);
    }
  }
  // Top-down iterates all nodes
  for (unsigned i = 0; i < sccTopologicalVec.size(); i++) {
    SCCNode *sccNode = sccTopologicalVec[i];
    for (SCCNode *callee : sccNode->calleeScc) {
      if (inQueue.find(callee) == inQueue.end()) {
        // callee has not been visited
        bool callerAllVisited = true;
        // Check whether all callers of the current callee have been visited
        for (SCCNode *caller : callee->callerScc) {
          if (inQueue.find(caller) == inQueue.end()) {
            callerAllVisited = false;
            break;
          }
        }
        if (callerAllVisited) {
          sccTopologicalVec.push_back(callee);
          inQueue.insert(callee);
        }
      }
    }
  }
}

void CGNode::AddCandsForCallNode(const KlassHierarchy *kh) {
  /* already set vcall candidates information */
  ASSERT_NOT_NULL(kh);
  if (HasSetVCallCandidates()) {
    return;
  }
  CHECK_FATAL(mirFunc != nullptr, "");
  Klass *klass = kh->GetKlassFromFunc(mirFunc);
  if (klass != nullptr) {
    MapleVector<MIRFunction*> *v = klass->GetCandidates(mirFunc->GetBaseFuncNameWithTypeStrIdx());
    if (v != nullptr) {
      vcallCands = *v;  // Vector copy
    }
  }
}

MIRFunction *CGNode::HasOneCandidate() const {
  int count = 0;
  MIRFunction *cand = nullptr;
  if (!mirFunc->IsEmpty()) {
    count++;
    cand = mirFunc;
  }
  /* scan candidates */
  for (uint32 i = 0; i < vcallCands.size(); i++) {
    if (vcallCands[i] == nullptr) {
      CHECK_FATAL(false, "");
    }
    if (!vcallCands[i]->IsEmpty()) {
      count++;
      if (cand == nullptr) {
        cand = vcallCands[i];
      }
    }
  }
  return count == 1 ? cand : nullptr;
}

AnalysisResult *DoCallGraph::Run(MIRModule *module, ModuleResultMgr *m) {
  MemPool *memPool = memPoolCtrler.NewMemPool("callgraph mempool");
  KlassHierarchy *cha = static_cast<KlassHierarchy*>(m->GetAnalysisResult(MoPhase_CHA, module));
  CHECK_FATAL(cha != nullptr, "CHA can't be null");
  CallGraph *cg = memPool->New<CallGraph>(module, memPool, cha, module->GetFileName().c_str());
  cg->debug_flag = TRACE_PHASE;
  cg->BuildCallGraph();
  m->AddResult(GetPhaseID(), *module, *cg);
  if (!module->IsInIPA()) {
    // do retype
    MemPool *localMp = memPoolCtrler.NewMemPool(PhaseName());
    maple::MIRBuilder dexMirbuilder(module);
    Retype retype(module, localMp, dexMirbuilder, cha);
    retype.DoRetype();
    memPoolCtrler.DeleteMemPool(localMp);
  }
  return cg;
}

AnalysisResult *DoIPODevirtulize::Run(MIRModule *module, ModuleResultMgr *m) {
  MemPool *memPool = memPoolCtrler.NewMemPool("ipodevirulize mempool");
  KlassHierarchy *cha = static_cast<KlassHierarchy*>(m->GetAnalysisResult(MoPhase_CHA, module));
  CHECK_FATAL(cha != nullptr, "");
  IPODevirtulize *dev = memPool->New<IPODevirtulize>(module, memPool, cha);
  // Devirtualize vcall of final variable
  dev->DevirtualFinal();
  memPoolCtrler.DeleteMemPool(memPool);
  return nullptr;
}

}  // namespace maple
