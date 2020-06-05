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
#ifndef MAPLE_IPA_INCLUDE_CALLGRAPH_H
#define MAPLE_IPA_INCLUDE_CALLGRAPH_H
#include "module_phase.h"
#include "mir_nodes.h"
#include "class_hierarchy.h"
#include "mir_builder.h"
namespace maple {
class SCCNode;
enum CallType {
  kCallTypeInvalid,
  kCallTypeCall,
  kCallTypeVirtualCall,
  kCallTypeSuperCall,
  kCallTypeInterfaceCall,
  kCallTypeIcall,
  kCallTypeIntrinsicCall,
  kCallTypeXinitrinsicCall,
  kCallTypeIntrinsicCallWithType,
  kCallTypeCustomCall,
  kCallTypePolymorphicCall,
  kCallTypeFakeThreadStartRun
};

struct NodeComparator {
  bool operator()(const MIRFunction *lhs, const MIRFunction *rhs) const {
    return lhs->GetPuidx() < rhs->GetPuidx();
  }
};

template<typename T>
struct Comparator {
  bool operator()(const T *lhs, const T *rhs) const {
    return lhs->GetID() < rhs->GetID();
  }
};

// Information description of each callsite
class CallInfo {
 public:
  CallInfo(CallType type, MIRFunction *call, StmtNode *s, uint32 ld, uint32 stmtId, bool local = false)
      : areAllArgsLocal(local), ctype(type), mirFunc(call), callStmt(s), loopDepth(ld), id(stmtId) {}

  virtual ~CallInfo() {}

  uint32 GetID() const {
    return id;
  }

  const char *GetCalleeName() const;
  CallType GetCallType() const {
    return ctype;
  }

  uint32 GetLoopDepth() const {
    return loopDepth;
  }

  const char *GetCallTypeName() const;
  StmtNode *GetCallStmt() const {
    return callStmt;
  }

  const MIRFunction *GetFunc() const {
    return mirFunc;
  }

  bool AreAllArgsLocal() const {
    return areAllArgsLocal;
  }

  void SetAllArgsLocal() {
    areAllArgsLocal = true;
  }

 private:
  bool areAllArgsLocal;
  CallType ctype;       // Call type
  MIRFunction *mirFunc; // Used to get signature
  StmtNode *callStmt;   // Call statement
  uint32 loopDepth;
  uint32 id;
};

// Node in callgraph
class CGNode {
 public:
  void AddNumRefs() {
    ++numReferences;
  }

  void DecreaseNumRefs() {
    --numReferences;
  }

  CGNode(MIRFunction *func, MapleAllocator *allocater, uint32 index)
      : alloc(allocater),
        id(index),
        sccNode(nullptr),
        mirFunc(func),
        callees(alloc->Adapter()),
        vcallCandidates(alloc->Adapter()),
        isVcallCandidatesValid(false),
        icallCandidates(alloc->Adapter()),
        isIcallCandidatesValid(false),
        numReferences(0),
        callerSet(alloc->Adapter()),
        stmtCount(0),
        nodeCount(0),
        mustNotBeInlined(false),
        vcallCands(alloc->Adapter()) {}

  ~CGNode() {}

  void Dump(std::ofstream &fout) const;
  void DumpDetail() const;

  MIRFunction *GetMIRFunction() const {
    return mirFunc;
  }

  void AddCallsite(CallInfo&, CGNode*);
  void AddCallsite(CallInfo*, MapleSet<CGNode*, Comparator<CGNode>>*);
  void RemoveCallsite(const CallInfo*, CGNode*);

  uint32 GetID() const {
    return id;
  }

  SCCNode *GetSCCNode() {
    return sccNode;
  }

  void SetSCCNode(SCCNode *node) {
    sccNode = node;
  }

  int32 GetPuIdx() const {
    return (mirFunc != nullptr) ? mirFunc->GetPuidx() : -1;
  }

  const std::string &GetMIRFuncName() const {
    return (mirFunc != nullptr) ? mirFunc->GetName() : GlobalTables::GetStrTable().GetStringFromStrIdx(GStrIdx(0));
  }

  void AddCandsForCallNode(const KlassHierarchy &kh);
  void AddVCallCandidate(MIRFunction *func) {
    vcallCands.push_back(func);
  }

  bool HasSetVCallCandidates() const {
    return !vcallCands.empty();
  }

  MIRFunction *HasOneCandidate() const;
  MapleVector<MIRFunction*> &GetVCallCandidates() {
    return vcallCands;
  }

  /* add caller to CGNode */
  void AddCaller(CGNode *caller) {
    callerSet.insert(caller);
  }

  void DelCaller(CGNode *caller) {
    callerSet.erase(caller);
  }

  bool HasCaller() const {
    return (!callerSet.empty());
  }

  uint32 NumberOfUses() const {
    return callerSet.size();
  }

  bool IsCalleeOf(CGNode *func) const;
  void IncrStmtCount() {
    ++stmtCount;
  }

  void IncrNodeCountBy(uint32 x) {
    nodeCount += x;
  }

  uint32 GetStmtCount() const {
    return stmtCount;
  }

  uint32 GetNodeCount() const {
    return nodeCount;
  }

  void Reset() {
    stmtCount = 0;
    nodeCount = 0;
    numReferences = 0;
    callees.clear();
    vcallCands.clear();
  }

  uint32 NumberOfCallSites() const {
    return callees.size();
  }

  const MapleMap<CallInfo*, MapleSet<CGNode*, Comparator<CGNode>>*, Comparator<CallInfo>> &GetCallee() const {
    return callees;
  }

  MapleMap<CallInfo*, MapleSet<CGNode*, Comparator<CGNode>>*, Comparator<CallInfo>> &GetCallee() {
    return callees;
  }

  const SCCNode *GetSCCNode() const {
    return sccNode;
  }

  MapleSet<CGNode*, Comparator<CGNode>>::iterator CallerBegin() const {
    return callerSet.begin();
  }

  MapleSet<CGNode*, Comparator<CGNode>>::iterator CallerEnd() const {
    return callerSet.end();
  }

  bool IsMustNotBeInlined() const {
    return mustNotBeInlined;
  }

  void SetMustNotBeInlined() {
    mustNotBeInlined = true;
  }

  bool IsVcallCandidatesValid() const {
    return isVcallCandidatesValid;
  }

  void SetVcallCandidatesValid() {
    isVcallCandidatesValid = true;
  }

  void AddVcallCandidate(CGNode *item) {
    vcallCandidates.insert(item);
  }

  MapleSet<CGNode*, Comparator<CGNode>> &GetVcallCandidates() {
    return vcallCandidates;
  }

  bool IsIcallCandidatesValid() const {
    return isIcallCandidatesValid;
  }

  void SetIcallCandidatesValid() {
    isIcallCandidatesValid = true;
  }

  void AddIcallCandidate(CGNode *item) {
    icallCandidates.insert(item);
  }

  MapleSet<CGNode*, Comparator<CGNode>> &GetIcallCandidates() {
    return icallCandidates;
  }

 private:
  // mirFunc is generated from callStmt's puIdx from mpl instruction
  // mirFunc will be nullptr if CGNode represents a external/intrinsic call
  MapleAllocator *alloc;
  uint32 id;
  SCCNode *sccNode;  // the id of the scc where this cgnode belongs to
  MIRFunction *mirFunc;
  // Each callsite corresponds to one element
  MapleMap<CallInfo*, MapleSet<CGNode*, Comparator<CGNode>>*, Comparator<CallInfo>> callees;
  MapleSet<CGNode*, Comparator<CGNode>> vcallCandidates;  // vcall candidates of mirFunc
  bool isVcallCandidatesValid;
  MapleSet<CGNode*, Comparator<CGNode>> icallCandidates;  // icall candidates of mirFunc
  bool isIcallCandidatesValid;
  uint32 numReferences;          // The number of the node in this or other CGNode's callees
  // function candidate for virtual call
  // now the candidates would be same function name from base class to subclass
  // with type inference, the candidates would be reduced
  MapleSet<CGNode*, Comparator<CGNode>> callerSet;
  uint32 stmtCount;  // count number of statements in the function, reuse this as callsite id
  uint32 nodeCount;  // count number of MIR nodes in the function/
  // this flag is used to mark the function which will read the current method invocation stack or something else,
  // so it cannot be inlined and all the parent nodes which contain this node should not be inlined, either.
  bool mustNotBeInlined;
  MapleVector<MIRFunction*> vcallCands;
};

using Callsite = std::pair<CallInfo*, MapleSet<CGNode*, Comparator<CGNode>>*>;
using CalleeIt = MapleMap<CallInfo*, MapleSet<CGNode*, Comparator<CGNode>>*, Comparator<CallInfo>>::iterator;

class SCCNode {
 public:
  uint32 id;
  MapleVector<CGNode*> cgNodes;
  MapleSet<SCCNode*, Comparator<SCCNode>> callerScc;
  MapleSet<SCCNode*, Comparator<SCCNode>> calleeScc;
  SCCNode(uint32 index, MapleAllocator *alloc)
      : id(index),
        cgNodes(alloc->Adapter()),
        callerScc(alloc->Adapter()),
        calleeScc(alloc->Adapter()) {}

  virtual ~SCCNode() {}

  void AddCGNode(CGNode *cgn) {
    cgNodes.push_back(cgn);
  }

  void Dump() const;
  void DumpCycle() const;
  void Verify() const;
  void Setup();
  const MapleVector<CGNode*> &GetCGNodes() const {
    return cgNodes;
  }

  const MapleSet<SCCNode*, Comparator<SCCNode>> &GetCalles() const {
    return calleeScc;
  }

  bool HasRecursion() const;
  bool HasSelfRecursion() const;
  bool HasCaller() const {
    return (!callerScc.empty());
  }

  uint32 GetID() const {
    return id;
  }
};

class CallGraph : public AnalysisResult {
 public:
  CallGraph(MIRModule &m, MemPool &memPool, KlassHierarchy &kh, const std::string &fn);
  ~CallGraph() {}

  CGNode *CallExternal() const {
    return callExternal;
  }

  void BuildCallGraph();
  CGNode *GetEntryNode() const {
    return entryNode;
  }

  const MapleVector<CGNode*> &GetRootNodes() const {
    return rootNodes;
  }

  const KlassHierarchy *GetKlassh() const {
    return klassh;
  }

  const MapleVector<SCCNode*> &GetSCCTopVec() const {
    return sccTopologicalVec;
  }

  const MapleMap<MIRFunction*, CGNode*, NodeComparator> &GetNodesMap() const {
    return nodesMap;
  }

  void HandleBody(MIRFunction&, BlockNode&, CGNode&, uint32);
  void AddCallGraphNode(MIRFunction&);
  void DumpToFile(bool dumpAll = true);
  void Dump() const;
  CGNode *GetCGNode(MIRFunction *func) const;
  CGNode *GetCGNode(PUIdx puIdx) const;
  SCCNode *GetSCCNode(MIRFunction *func) const;
  bool IsRootNode(MIRFunction *func) const;
  void UpdateCallGraphNode(CGNode &node);
  void RecomputeSCC();
  MIRFunction *CurFunction() const {
    return mirModule->CurFunction();
  }

  bool IsInIPA() const {
    return mirModule->IsInIPA();
  }

  /* iterator */
  using iterator = MapleMap<MIRFunction*, CGNode*>::iterator;
  iterator Begin() {
    return nodesMap.begin();
  }

  iterator End() {
    return nodesMap.end();
  }

  MIRFunction *GetMIRFunction(iterator it) const {
    return (*it).first;
  }

  CGNode *GetCGNode(iterator it) const {
    return (*it).second;
  }

  void DelNode(CGNode &node);
  bool debug_flag;
  bool debug_scc;
  void BuildSCC();
  void VerifySCC() const;
  void BuildSCCDFS(CGNode &caller, unsigned int &visitIndex, std::vector<SCCNode*> &sccNodes,
                   std::vector<CGNode*> &cgNodes, std::vector<uint32> &visitedOrder,
                   std::vector<uint32> &lowestOrder, std::vector<bool> &inStack,
                   std::vector<uint32> &visitStack);

 private:
  MIRModule *mirModule;
  MapleAllocator cgAlloc;
  MIRBuilder *mirBuilder;
  CGNode *entryNode;  // For main function, nullptr if there is multiple entries
  MapleVector<CGNode*> rootNodes;
  std::string fileName; /* used for output dot file */
  KlassHierarchy *klassh;
  MapleMap<MIRFunction*, CGNode*, NodeComparator> nodesMap;
  MapleVector<SCCNode*> sccTopologicalVec;
  CGNode *callExternal; /* Auxiliary node used in icall/intrinsic call */
  uint32 numOfNodes;
  uint32 numOfSccs;
  std::unordered_set<uint64> callsiteHash;
  void GenCallGraph();
  CGNode *GetOrGenCGNode(PUIdx puIdx, bool isVcall = false, bool isIcall = false);
  CallType GetCallType(Opcode op) const;
  CallInfo *GenCallInfo(CallType type, MIRFunction *call, StmtNode *s, uint32 loopDepth, uint32 callsiteID) {
    return cgAlloc.GetMemPool()->New<CallInfo>(type, call, s, loopDepth, callsiteID);
  }

  void FindRootNodes();
  void SCCTopologicalSort(const std::vector<SCCNode*> &sccNodes);
  void SetCompilationFunclist() const;
  void IncrNodesCount(CGNode *cgNode, BaseNode *bn);
};

class DoCallGraph : public ModulePhase {
 public:
  explicit DoCallGraph(ModulePhaseID id) : ModulePhase(id) {}

  AnalysisResult *Run(MIRModule *module, ModuleResultMgr *m) override;
  std::string PhaseName() const override {
    return "callgraph";
  }

  virtual ~DoCallGraph(){};
};

class IPODevirtulize {
 public:
  IPODevirtulize(MIRModule *m, MemPool *memPool, KlassHierarchy *kh)
      : cgalloc(memPool), mirBuilder(cgalloc.GetMemPool()->New<MIRBuilder>(m)), klassh(kh), debugFlag(false) {}

  virtual ~IPODevirtulize() = default;
  void DevirtualFinal();
  KlassHierarchy *GetKlassh() const {
    return klassh;
  }

 private:
  MapleAllocator cgalloc;
  MIRBuilder *mirBuilder;
  KlassHierarchy *klassh;
  bool debugFlag;
  void SearchDefInMemberMethods(const Klass &klass);
  void SearchDefInClinit(const Klass &klass);
};

class DoIPODevirtulize : public ModulePhase {
 public:
  explicit DoIPODevirtulize(ModulePhaseID id) : ModulePhase(id) {}

  AnalysisResult *Run(MIRModule *module, ModuleResultMgr *m) override;
  std::string PhaseName() const override {
    return "ipodevirtulize";
  }

  virtual ~DoIPODevirtulize(){};
};
}  // namespace maple
#endif  // MAPLE_IPA_INCLUDE_CALLGRAPH_H
