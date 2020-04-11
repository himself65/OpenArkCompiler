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
#ifndef MAPLEBE_INCLUDE_BE_LOWERER_H
#define MAPLEBE_INCLUDE_BE_LOWERER_H
/* C++ headers. */
#include <vector>
#include <unordered_map>
#include <utility>
#include <cstddef>
#include "intrinsics.h"  /* For IntrinDesc. This includes 'intrinsic_op.h' as well */
#include "becommon.h"
#include "bbt.h"
/* MapleIR headers. */
#include "mir_nodes.h"
#include "mir_module.h"
#include "mir_function.h"
#include "mir_lower.h"

namespace maplebe {
class CGLowerer {
  enum Option : uint64 {
    kUndefined = 0,
    kGenEh = 1ULL << 0,
    kVerboseAsm = 1ULL << 1,
  };

  using BuiltinFunctionID = uint32;
  using OptionFlag = uint64;

 public:
  CGLowerer(MIRModule &mod, BECommon &common, MIRFunction *func = nullptr)
      : mirModule(mod),
        beCommon(common) {
    SetOptions(kGenEh);
    mirBuilder = mod.GetMIRBuilder();
    SetCurrentFunc(func);
  }

  CGLowerer(MIRModule &mod, BECommon &common, bool genEh, bool verboseAsm)
      : mirModule(mod),
        beCommon(common) {
    OptionFlag option = 0;
    if (genEh) {
      option |= kGenEh;
    }
    if (verboseAsm) {
      option |= kVerboseAsm;
    }
    SetOptions(option);
    mirBuilder = mod.GetMIRBuilder();
    SetCurrentFunc(nullptr);
  }

  ~CGLowerer() = default;

  MIRFunction *RegisterFunctionVoidStarToVoid(BuiltinFunctionID id, const std::string &name,
                                              const std::string &paramName);

  void RegisterBuiltIns();

  void LowerFunc(MIRFunction &func);

  BaseNode *LowerIntrinsicop(const BaseNode&, IntrinsicopNode&, BlockNode&);

  BaseNode *LowerIntrinsicopwithtype(const BaseNode&, IntrinsicopNode&, BlockNode&);

  StmtNode *LowerIntrinsicMplClearStack(IntrinsiccallNode &intrinCall, BlockNode &newBlk);

  StmtNode *LowerIntrinsicRCCall(IntrinsiccallNode &intrinCall);

  void LowerArrayStore(IntrinsiccallNode &intrinCall, BlockNode &newBlk);

  StmtNode *LowerDefaultIntrinsicCall(IntrinsiccallNode &intrinCall, MIRSymbol &st, MIRFunction &fn);

  StmtNode *LowerIntrinsicMplCleanupLocalRefVarsSkip(IntrinsiccallNode &intrinCall);

  StmtNode *LowerIntrinsiccall(IntrinsiccallNode &intrinCall, BlockNode&);

  StmtNode *LowerSyncEnterSyncExit(StmtNode &stmt);

  MIRFunction *GetCurrentFunc() const {
    return mirModule.CurFunction();
  }

  BaseNode *LowerExpr(const BaseNode&, BaseNode&, BlockNode&);

  BaseNode *LowerDread(DreadNode &dread);

  BaseNode *LowerIread(IreadNode &iread) {
    /* use PTY_u8 for boolean type in dread/iread */
    if (iread.GetPrimType() == PTY_u1) {
      iread.SetPrimType(PTY_u8);
    }
    return (iread.GetFieldID() == 0 ? &iread : LowerIreadBitfield(iread));
  }

  BaseNode *LowerIreadBitfield(IreadNode &iread);

  void LowerDassign(DassignNode &dassign, BlockNode &block);

  void LowerResetStmt(StmtNode &stmt, BlockNode &block);

  void LowerIassign(IassignNode &iassign, BlockNode &block);

  void LowerRegassign(RegassignNode &regAssign, BlockNode &block);

  StmtNode *LowerIntrinsicopDassign(const DassignNode &dassign, IntrinsicopNode &intrinsic, BlockNode &block);

  void LowerGCMalloc(const BaseNode &node, const GCMallocNode &gcNode, BlockNode &blkNode, bool perm = false);

  std::string GetNewArrayFuncName(const uint32 elemSize, const bool perm) const;

  void LowerJarrayMalloc(const StmtNode &stmt, const JarrayMallocNode &node, BlockNode &block, bool perm = false);

  BaseNode *LowerAddrof(AddrofNode &addrof) {
    return &addrof;
  }

  BaseNode *LowerIaddrof(const IreadNode &iaddrof);
  BaseNode *LowerFarray(ArrayNode &array);
  BaseNode *LowerArrayDim(ArrayNode &array, int32 dim);
  BaseNode *LowerArrayForLazyBiding(BaseNode &baseNode, BaseNode &offsetNode, const BaseNode &parent);
  BaseNode *LowerArray(ArrayNode &array, const BaseNode &parent);

  DassignNode *SaveReturnValueInLocal(StIdx, uint16);
  void LowerCallStmt(StmtNode&, StmtNode*&, BlockNode&);
  BlockNode *LowerCallAssignedStmt(StmtNode &stmt);
  BaseNode *LowerRem(BaseNode &rem, BlockNode &block);

  void LowerStmt(StmtNode &stmt, BlockNode &block);

  MIRSymbol *CreateNewRetVar(const MIRType &ty, const std::string &prefix);

  void RegisterExternalLibraryFunctions();

  BlockNode *LowerBlock(BlockNode &block);

  void LowerTryCatchBlocks(BlockNode &body);

#if TARGARM32 || TARGAARCH64
  BlockNode *LowerReturnStruct(NaryStmtNode &retNode);
#endif
  virtual BlockNode *LowerReturn(NaryStmtNode &retNode);
  void LowerEntry(MIRFunction &func);

  StmtNode *LowerCall(CallNode &call, StmtNode *&stmt, BlockNode &block);

  void CleanupBranches(MIRFunction &func) const;

  void LowerTypePtr(BaseNode &expr) const;

  BaseNode *LowerDreadBitfield(DreadNode &dread);

  StmtNode *LowerDassignBitfield(DassignNode &dassign, BlockNode &block);
  StmtNode *LowerIassignBitfield(IassignNode &iassign, BlockNode &block);

  bool ShouldOptarray() const {
    ASSERT(mirModule.CurFunction() != nullptr, "nullptr check");
    return MIRLower::ShouldOptArrayMrt(*mirModule.CurFunction());
  }

  BaseNode *NodeConvert(PrimType mtype, BaseNode &expr);
  /* Lower pointer/reference types if found in pseudo registers. */
  void LowerPseudoRegs(MIRFunction &func);

  /* A pseudo register refers to a symbol when DreadNode is converted to RegreadNode. */
  StIdx GetSymbolReferredToByPseudoRegister(PregIdx regNO) const {
    (void)regNO;
    return StIdx();
  }

  void SetOptions(OptionFlag option) {
    options = option;
  }

  void SetCheckLoadStore(bool value) {
    checkLoadStore = value;
  }

  /* if it defines a built-in to use for the given intrinsic, return the name. otherwise, return nullptr */
  PUIdx GetBuiltinToUse(BuiltinFunctionID id) const;

  MIRModule &mirModule;
  BECommon &beCommon;
  BlockNode *currentBlock = nullptr;  /* current block for lowered statements to be inserted to */
  bool checkLoadStore = false;
  int64 seed = 0;
  static const std::string kIntrnRetValPrefix;

  static constexpr PUIdx kFuncNotFound = PUIdx(-1);
  static constexpr int kThreeDimArray = 3;
  static constexpr int kNodeThirdOpnd = 2;
  static constexpr int kMCCSyncEnterFast0 = 0;
  static constexpr int kMCCSyncEnterFast1 = 1;
  static constexpr int kMCCSyncEnterFast2 = 2;
  static constexpr int kMCCSyncEnterFast3 = 3;

 protected:
  /*
   * true if the lower level (e.g. mplcg) can handle the intrinsic directly.
   * For example, the INTRN_MPL_ATOMIC_EXCHANGE_PTR can be directly handled by mplcg,
   * and generate machine code sequences not containing any function calls.
   * Such intrinsics will bypass the lowering of "assigned",
   * and let mplcg handle the intrinsic results which are not return values.
   */
  bool IsIntrinsicCallHandledAtLowerLevel(MIRIntrinsicID intrinsic);

 private:

  void SetCurrentFunc(MIRFunction *func) {
    mirModule.SetCurFunction(func);
  }

  bool ShouldAddAdditionalComment() const {
    return (options & kVerboseAsm) != 0;
  }

  bool GenerateExceptionHandlingCode() const {
    return (options & kGenEh) != 0;
  }

  BaseNode *MergeToCvtType(PrimType dtyp, PrimType styp, BaseNode &src);
  BaseNode *LowerJavascriptIntrinsicop(IntrinsicopNode &intrinNode, const IntrinDesc &desc);
  StmtNode *CreateStmtCallWithReturnValue(const IntrinsicopNode &intrinNode, const MIRSymbol &ret, PUIdx bFunc,
                                          BaseNode *extraInfo = nullptr);
  StmtNode *CreateStmtCallWithReturnValue(const IntrinsicopNode &intrinNode, PregIdx retPregIdx, PUIdx bFunc,
                                          BaseNode *extraInfo = nullptr);
  BaseNode *LowerIntrinsicop(const BaseNode &parent, IntrinsicopNode &intrinNode);
  BaseNode *LowerIntrinJavaMerge(const BaseNode &parent, IntrinsicopNode &intrinNode);
  BaseNode *LowerIntrinJavaArrayLength(const BaseNode &parent, IntrinsicopNode &intrinNode);
  BaseNode *LowerIntrinsicopWithType(const BaseNode &parent, IntrinsicopNode &intrinNode);

  MIRType *GetArrayNodeType(BaseNode &baseNode);
  IreadNode &GetLenNode(BaseNode &opnd0);
  LabelIdx GetLabelIdx(MIRFunction &curFunc) const;
  void ProcessArrayExpr(BaseNode &expr, BlockNode &blkNode);
  void ProcessClassInfo(MIRType &classType, bool &classInfoFromRt, std::string &classInfo);
  StmtNode *GenCallNode(const StmtNode &stmt, PUIdx &funcCalled, CallNode& origCall);
  StmtNode *GenIntrinsiccallNode(const StmtNode &stmt, PUIdx &funcCalled, bool &handledAtLowerLevel,
                                    IntrinsiccallNode &origCall);
  StmtNode *GenIcallNode(PUIdx &funcCalled, IcallNode &origCall);
  BlockNode *GenBlockNode(StmtNode &newCall, const CallReturnVector &p2nRets, const Opcode &opcode,
                          const PUIdx &funcCalled, bool handledAtLowerLevel);
  BaseNode *GetClassInfoExprFromRuntime(const std::string &classInfo);
  BaseNode *GetClassInfoExpr(const std::string &classInfo);
  BaseNode *GetBaseNodeFromCurFunc(MIRFunction &curFunc, bool isJarray);

  OptionFlag options = 0;
  bool needBranchCleanup = false;
  bool hasTry = false;

  static std::vector<std::pair<BuiltinFunctionID, PUIdx>> builtinFuncIDs;
  MIRBuilder *mirBuilder = nullptr;
  uint32 labelIdx = 0;
  static std::unordered_map<IntrinDesc*, PUIdx> intrinFuncIDs;
};
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_BE_LOWERER_H */
