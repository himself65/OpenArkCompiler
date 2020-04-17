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
#include "feir_stmt.h"
#include "opcode_info.h"
#include "literalstrname.h"
#include "mir_type.h"
#include "feir_builder.h"
#include "feir_var_reg.h"
#include "feir_var_name.h"
#include "fe_manager.h"
#include "mplfe_env.h"

namespace maple {
// ---------- FEIRStmt ----------
std::list<StmtNode*> FEIRStmt::GenMIRStmtsImpl(MIRBuilder &mirBuilder) const {
  return std::list<StmtNode*>();
}

// ---------- FEIRStmtCheckPoint ----------
void FEIRStmtCheckPoint::Reset() {
  predCPs.clear();
  localUD.clear();
  lastDef.clear();
  cacheUD.clear();
  defs.clear();
  uses.clear();
}

void FEIRStmtCheckPoint::RegisterDFGNode(UniqueFEIRVar &var) {
  CHECK_NULL_FATAL(var);
  if (var->IsDef()) {
    defs.push_back(&var);
    lastDef[FEIRDFGNode(var)] = &var;
  } else {
    uses.push_back(&var);
    auto it = lastDef.find(FEIRDFGNode(var));
    if (it != lastDef.end()) {
      CHECK_FATAL(localUD[&var].insert(it->second).second, "localUD insert failed");
    }
  }
}

void FEIRStmtCheckPoint::RegisterDFGNodes(const std::list<UniqueFEIRVar*> &vars) {
  for (UniqueFEIRVar *var : vars) {
    CHECK_NULL_FATAL(var);
    RegisterDFGNode(*var);
  }
}

void FEIRStmtCheckPoint::AddPredCheckPoint(const UniqueFEIRStmt &stmtCheckPoint) {
  if (stmtCheckPoint == nullptr || stmtCheckPoint->GetKind() != FEIRNodeKind::kStmtCheckPoint) {
    CHECK_FATAL(false, "invalid input");
  }
  FEIRStmtCheckPoint *cp = static_cast<FEIRStmtCheckPoint*>(stmtCheckPoint.get());
  if (predCPs.find(cp) == predCPs.end()) {
    CHECK_FATAL(predCPs.insert(cp).second, "pred checkpoints insert error");
  }
}

std::set<UniqueFEIRVar*> &FEIRStmtCheckPoint::CalcuDef(const UniqueFEIRVar &use) {
  CHECK_NULL_FATAL(use);
  auto itLocal = localUD.find(&use);
  // search localUD
  if (itLocal != localUD.end()) {
    return itLocal->second;
  }
  // search cacheUD
  auto itCache = cacheUD.find(FEIRDFGNode(use));
  if (itCache != cacheUD.end()) {
    return itCache->second;
  }
  // search by DFS
  std::set<const FEIRStmtCheckPoint*> visitSet;
  std::set<UniqueFEIRVar*> &target = cacheUD[FEIRDFGNode(use)];
  CalcuDefDFS(target, use, *this, visitSet);
  if (target.size() == 0) {
    WARN(kLncWarn, "use var %s without def", use->GetNameRaw().c_str());
  }
  return target;
}

void FEIRStmtCheckPoint::CalcuDefDFS(std::set<UniqueFEIRVar*> &result, const UniqueFEIRVar &use,
                                     const FEIRStmtCheckPoint &cp,
                                     std::set<const FEIRStmtCheckPoint*> &visitSet) const {
  CHECK_NULL_FATAL(use);
  if (visitSet.find(&cp) != visitSet.end()) {
    return;
  }
  CHECK_FATAL(visitSet.insert(&cp).second, "visitSet insert failed");
  auto itLast = cp.lastDef.find(FEIRDFGNode(use));
  if (itLast != cp.lastDef.end()) {
    CHECK_FATAL(result.insert(itLast->second).second, "result insert failed");
    return;
  }
  // optimization by cacheUD
  auto itCache = cp.cacheUD.find(FEIRDFGNode(use));
  if (itCache != cp.cacheUD.end()) {
    for (UniqueFEIRVar *def : itCache->second) {
      CHECK_FATAL(result.insert(def).second, "result insert failed");
    }
    if (itCache->second.size() > 0) {
      return;
    }
  }
  // optimization by cacheUD (end)
  for (const FEIRStmtCheckPoint *pred : cp.predCPs) {
    CHECK_NULL_FATAL(pred);
    CalcuDefDFS(result, use, *pred, visitSet);
  }
}

// ---------- FEStmtAssign ----------
FEIRStmtAssign::FEIRStmtAssign(FEIRNodeKind argKind, std::unique_ptr<FEIRVar> argVar)
    : FEIRStmt(argKind),
      hasException(false),
      var(std::move(argVar)) {
}

// ---------- FEStmtDAssign ----------
FEIRStmtDAssign::FEIRStmtDAssign(std::unique_ptr<FEIRVar> argVar, std::unique_ptr<FEIRExpr> argExpr, int32 argFieldID)
    : FEIRStmtAssign(FEIRNodeKind::kStmtDAssign, std::move(argVar)),
      fieldID(argFieldID) {
  SetExpr(std::move(argExpr));
}

std::list<StmtNode*> FEIRStmtDAssign::GenMIRStmtsImpl(MIRBuilder &mirBuilder) const {
  std::list<StmtNode*> ans;
  ASSERT(var != nullptr, "dst var is nullptr");
  ASSERT(expr != nullptr, "src expr is nullptr");
  MIRSymbol *dstSym = var->GenerateMIRSymbol(mirBuilder);
  BaseNode *srcNode = expr->GenMIRNode(mirBuilder);
  StmtNode *mirStmt = mirBuilder.CreateStmtDassign(*dstSym, fieldID, srcNode);
  ans.push_back(mirStmt);
  return ans;
}

// ---------- FEIRStmtJavaTypeCheck ----------
FEIRStmtJavaTypeCheck::FEIRStmtJavaTypeCheck(std::unique_ptr<FEIRVar> argVar, std::unique_ptr<FEIRExpr> argExpr,
                                             std::unique_ptr<FEIRType> argType,
                                             FEIRStmtJavaTypeCheck::CheckKind argCheckKind)
    : FEIRStmtAssign(FEIRNodeKind::kStmtJavaTypeCheck, std::move(argVar)),
      checkKind(argCheckKind),
      expr(std::move(argExpr)),
      type(std::move(argType)) {}

std::list<StmtNode*> FEIRStmtJavaTypeCheck::GenMIRStmtsImpl(MIRBuilder &mirBuilder) const {
  std::list<StmtNode*> ans;
  CHECK_FATAL(expr->GetKind() == FEIRNodeKind::kExprDRead, "only support expr dread");
  BaseNode *objNode = expr->GenMIRNode(mirBuilder);
  MIRSymbol *ret = var->GenerateLocalMIRSymbol(mirBuilder);
  MIRType *mirType = type->GenerateMIRType();
  MIRType *mirPtrType = GlobalTables::GetTypeTable().GetOrCreatePointerType(*mirType, PTY_ref);
  MapleVector<BaseNode*> arguments(mirBuilder.GetCurrentFuncCodeMpAllocator()->Adapter());
  if (checkKind == kCheckCast) {
    arguments.push_back(objNode);
    StmtNode *callStmt = mirBuilder.CreateStmtIntrinsicCallAssigned(INTRN_JAVA_CHECK_CAST, arguments, ret,
                                                                    mirPtrType->GetTypeIndex());
    ans.push_back(callStmt);
  } else {
    BaseNode *instanceOf = mirBuilder.CreateExprIntrinsicop(INTRN_JAVA_INSTANCE_OF, OP_intrinsicopwithtype, *mirPtrType,
                                                            arguments);
    instanceOf->SetPrimType(PTY_u1);
    DassignNode *stmt = mirBuilder.CreateStmtDassign(*ret, 0, instanceOf);
    ans.push_back(stmt);
  }
  return ans;
}

// ---------- FEIRStmtJavaConstClass ----------
FEIRStmtJavaConstClass::FEIRStmtJavaConstClass(std::unique_ptr<FEIRVar> argVar, std::unique_ptr<FEIRType> argType)
    : FEIRStmtAssign(FEIRNodeKind::kStmtJavaConstClass, std::move(argVar)),
      type(std::move(argType)) {}

std::list<StmtNode*> FEIRStmtJavaConstClass::GenMIRStmtsImpl(MIRBuilder &mirBuilder) const {
  std::list<StmtNode*> ans;
  MIRSymbol *varSym = var->GenerateLocalMIRSymbol(mirBuilder);
  MapleVector<BaseNode*> args(mirBuilder.GetCurrentFuncCodeMpAllocator()->Adapter());
  MIRType *ptrType = type->GenerateMIRTypeAuto(kSrcLangJava);
  BaseNode *expr =
      mirBuilder.CreateExprIntrinsicop(INTRN_JAVA_CONST_CLASS, OP_intrinsicopwithtype, *ptrType, args);
  StmtNode *stmt = mirBuilder.CreateStmtDassign(*varSym, 0, expr);
  ans.push_back(stmt);
  return ans;
}

// ---------- FEIRStmtJavaConstString ----------
FEIRStmtJavaConstString::FEIRStmtJavaConstString(std::unique_ptr<FEIRVar> argVar, const GStrIdx &argStrIdx)
    : FEIRStmtAssign(FEIRNodeKind::kStmtJavaConstString, std::move(argVar)),
      strIdx(argStrIdx) {}

std::list<StmtNode*> FEIRStmtJavaConstString::GenMIRStmtsImpl(MIRBuilder &mirBuilder) const {
  std::list<StmtNode*> ans;
  const std::string &str = GlobalTables::GetStrTable().GetStringFromStrIdx(strIdx);
  MIRSymbol *literalVal = FEManager::GetJavaStringManager().GetLiteralVar(str);
  if (literalVal == nullptr) {
    literalVal = FEManager::GetJavaStringManager().CreateLiteralVar(mirBuilder, str, false);
  }
  MIRSymbol *literalValPtr = FEManager::GetJavaStringManager().GetLiteralPtrVar(literalVal);
  if (literalValPtr == nullptr) {
    std::string localStrName = kLocalStringPrefix + std::to_string(static_cast<uint32>(strIdx));
    MIRType *typeString = FETypeManager::kFEIRTypeJavaString->GenerateMIRTypeAuto(kSrcLangJava);
    MIRSymbol *symbolLocal = mirBuilder.GetOrCreateLocalDecl(localStrName.c_str(), *typeString);
    MapleVector<BaseNode*> args(mirBuilder.GetCurrentFuncCodeMpAllocator()->Adapter());
    args.push_back(mirBuilder.CreateExprAddrof(0, *literalVal));
    StmtNode *stmtCreate = mirBuilder.CreateStmtCallAssigned(
        FEManager::GetTypeManager().GetPuIdxForMCCGetOrInsertLiteral(), args, symbolLocal, OP_callassigned);
    ans.push_back(stmtCreate);
    literalValPtr = symbolLocal;
  }
  MIRSymbol *varDst = var->GenerateLocalMIRSymbol(mirBuilder);
  AddrofNode *node = mirBuilder.CreateDread(*literalValPtr, PTY_ptr);
  StmtNode *stmt = mirBuilder.CreateStmtDassign(*varDst, 0, node);
  ans.push_back(stmt);
  return ans;
}

// ---------- FEIRStmtJavaMultiANewArray ----------
UniqueFEIRVar FEIRStmtJavaMultiANewArray::varSize = nullptr;
UniqueFEIRVar FEIRStmtJavaMultiANewArray::varClass = nullptr;
UniqueFEIRType FEIRStmtJavaMultiANewArray::typeAnnotation = nullptr;
FEStructMethodInfo *FEIRStmtJavaMultiANewArray::methodInfoNewInstance = nullptr;

FEIRStmtJavaMultiANewArray::FEIRStmtJavaMultiANewArray(std::unique_ptr<FEIRVar> argVar,
                                                       std::unique_ptr<FEIRType> argType)
    : FEIRStmtAssign(FEIRNodeKind::kStmtJavaMultiANewArray, std::move(argVar)),
      type(std::move(argType)) {}

void FEIRStmtJavaMultiANewArray::AddVarSize(std::unique_ptr<FEIRVar> argVarSize) {
  argVarSize->SetType(FETypeManager::kPrimFEIRTypeI32->Clone());
  UniqueFEIRExpr expr = FEIRBuilder::CreateExprDRead(std::move(argVarSize));
  exprSizes.push_back(std::move(expr));
}

void FEIRStmtJavaMultiANewArray::AddVarSizeRev(std::unique_ptr<FEIRVar> argVarSize) {
  argVarSize->SetType(FETypeManager::kPrimFEIRTypeI32->Clone());
  UniqueFEIRExpr expr = FEIRBuilder::CreateExprDRead(std::move(argVarSize));
  exprSizes.push_front(std::move(expr));
}

std::list<StmtNode*> FEIRStmtJavaMultiANewArray::GenMIRStmtsImpl(MIRBuilder &mirBuilder) const {
  std::list<StmtNode*> ans;
  // size array fill
  MapleVector<BaseNode*> argsSizeArrayFill(mirBuilder.GetCurrentFuncCodeMpAllocator()->Adapter());
  for (const UniqueFEIRExpr &expr : exprSizes) {
    BaseNode *node = expr->GenMIRNode(mirBuilder);
    argsSizeArrayFill.push_back(node);
  }
  MIRSymbol *symSize = GetVarSize()->GenerateLocalMIRSymbol(mirBuilder);
  StmtNode *stmtSizeArrayFill = mirBuilder.CreateStmtIntrinsicCallAssigned(INTRN_JAVA_ARRAY_FILL, argsSizeArrayFill,
                                                                           symSize, TyIdx(PTY_i32));
  ans.push_back(stmtSizeArrayFill);
  // class annotation
  FEIRStmtJavaConstClass feStmtConstClass(GetVarClass()->Clone(), GetTypeAnnotation()->Clone());
  std::list<StmtNode*> stmtsConstClass = feStmtConstClass.GenMIRStmts(mirBuilder);
  (void)ans.insert(ans.end(), stmtsConstClass.begin(), stmtsConstClass.end());
  // invoke newInstance
  UniqueFEIRVar varRetCall = var->Clone();
  varRetCall->SetType(FETypeManager::kFEIRTypeJavaObject->Clone());
  FEIRStmtCallAssign feStmtCall(GetMethodInfoNewInstance(), OP_call, varRetCall->Clone(), true);
  feStmtCall.AddExprArg(FEIRBuilder::CreateExprDRead(GetVarClass()->Clone()));
  feStmtCall.AddExprArg(FEIRBuilder::CreateExprDRead(GetVarSize()->Clone()));
  std::list<StmtNode*> stmtsCall = feStmtCall.GenMIRStmts(mirBuilder);
  (void)ans.insert(ans.end(), stmtsCall.begin(), stmtsCall.end());
  // check cast
  var->SetType(type->Clone());
  UniqueFEIRExpr expr = std::make_unique<FEIRExprDRead>(std::move(varRetCall));
  FEIRStmtJavaTypeCheck feStmtCheck(var->Clone(), std::move(expr), type->Clone(), FEIRStmtJavaTypeCheck::kCheckCast);
  std::list<StmtNode*> stmtsCheck = feStmtCheck.GenMIRStmts(mirBuilder);
  (void)ans.insert(ans.end(), stmtsCheck.begin(), stmtsCheck.end());
  return ans;
}

const UniqueFEIRVar &FEIRStmtJavaMultiANewArray::GetVarSize() {
  if (varSize != nullptr) {
    return varSize;
  }
  MPLFE_PARALLEL_FORBIDDEN();
  GStrIdx varNameIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName("tmpsize");
  UniqueFEIRType varSizeType = FETypeManager::kPrimFEIRTypeI32->Clone();
  (void)varSizeType->ArrayIncrDim();
  varSize = std::make_unique<FEIRVarName>(varNameIdx, std::move(varSizeType), true);
  return varSize;
}

const UniqueFEIRVar &FEIRStmtJavaMultiANewArray::GetVarClass() {
  if (varClass != nullptr) {
    return varClass;
  }
  MPLFE_PARALLEL_FORBIDDEN();
  GStrIdx varNameIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName("tmpclass");
  varClass = std::make_unique<FEIRVarName>(varNameIdx, FETypeManager::kFEIRTypeJavaClass->Clone(), true);
  return varClass;
}

const UniqueFEIRType &FEIRStmtJavaMultiANewArray::GetTypeAnnotation() {
  if (typeAnnotation != nullptr) {
    return typeAnnotation;
  }
  MPLFE_PARALLEL_FORBIDDEN();
  typeAnnotation = std::make_unique<FEIRTypeDefault>(PTY_ref);
  static_cast<FEIRTypeDefault*>(typeAnnotation.get())->LoadFromJavaTypeName("Ljava/lang/annotation/Annotation;", false);
  return typeAnnotation;
}

FEStructMethodInfo &FEIRStmtJavaMultiANewArray::GetMethodInfoNewInstance() {
  if (methodInfoNewInstance != nullptr) {
    return *methodInfoNewInstance;
  }
  std::string methodNameJava = "Ljava/lang/reflect/Array;|newInstance|(Ljava/lang/Class;[I)Ljava/lang/Object;";
  GStrIdx methodNameIdx =
      GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(NameMangler::EncodeName(methodNameJava));
  methodInfoNewInstance = static_cast<FEStructMethodInfo*>(
      FEManager::GetTypeManager().RegisterStructMethodInfo(methodNameIdx, kSrcLangJava, true));
  return *methodInfoNewInstance;
}

// ---------- FEIRStmtUseOnly ----------
FEIRStmtUseOnly::FEIRStmtUseOnly(FEIRNodeKind argKind, Opcode argOp, std::unique_ptr<FEIRExpr> argExpr)
    : FEIRStmt(argKind),
      op(argOp) {
  if (argExpr != nullptr) {
    expr = std::move(argExpr);
  }
}

FEIRStmtUseOnly::FEIRStmtUseOnly(Opcode argOp, std::unique_ptr<FEIRExpr> argExpr)
    : FEIRStmtUseOnly(FEIRNodeKind::kStmtUseOnly, argOp, std::move(argExpr)) {}

std::list<StmtNode*> FEIRStmtUseOnly::GenMIRStmtsImpl(MIRBuilder &mirBuilder) const {
  std::list<StmtNode*> ans;
  ASSERT_NOT_NULL(expr);
  BaseNode *srcNode = expr->GenMIRNode(mirBuilder);
  StmtNode *mirStmt = mirBuilder.CreateStmtNary(op, srcNode);
  ans.push_back(mirStmt);
  return ans;
}

// ---------- FEIRStmtReturn ----------
FEIRStmtReturn::FEIRStmtReturn(std::unique_ptr<FEIRExpr> argExpr)
    : FEIRStmtUseOnly(FEIRNodeKind::kStmtReturn, OP_return, std::move(argExpr)) {}

std::list<StmtNode*> FEIRStmtReturn::GenMIRStmtsImpl(MIRBuilder &mirBuilder) const {
  std::list<StmtNode*> ans;
  StmtNode *mirStmt = nullptr;
  if (expr == nullptr) {
    mirStmt = mirBuilder.CreateStmtReturn(nullptr);
  } else {
    BaseNode *srcNode = expr->GenMIRNode(mirBuilder);
    mirStmt = mirBuilder.CreateStmtReturn(srcNode);
  }
  ans.push_back(mirStmt);
  return ans;
}

// ---------- FEIRStmtGoto ----------
FEIRStmtGoto::FEIRStmtGoto(uint32 argLabelIdx)
    : FEIRStmt(FEIRNodeKind::kStmtGoto),
      labelIdx(argLabelIdx),
      stmtTarget(nullptr) {}

FEIRStmtGoto::~FEIRStmtGoto() {
  stmtTarget = nullptr;
}

std::list<StmtNode*> FEIRStmtGoto::GenMIRStmtsImpl(MIRBuilder &mirBuilder) const {
  std::list<StmtNode*> ans;
  CHECK_NULL_FATAL(stmtTarget);
  GotoNode *gotoNode = mirBuilder.CreateStmtGoto(OP_goto, stmtTarget->GetMIRLabelIdx());
  ans.push_back(gotoNode);
  return ans;
}

// ---------- FEIRStmtCondGoto ----------
FEIRStmtCondGoto::FEIRStmtCondGoto(Opcode argOp, uint32 argLabelIdx, UniqueFEIRExpr argExpr)
    : FEIRStmtGoto(argLabelIdx),
      op(argOp) {
  kind = FEIRNodeKind::kStmtCondGoto;
  SetExpr(std::move(argExpr));
}

std::list<StmtNode*> FEIRStmtCondGoto::GenMIRStmtsImpl(MIRBuilder &mirBuilder) const {
  std::list<StmtNode*> ans;
  BaseNode *condNode = expr->GenMIRNode(mirBuilder);
  CHECK_NULL_FATAL(stmtTarget);
  CondGotoNode *gotoNode = mirBuilder.CreateStmtCondGoto(condNode, op, stmtTarget->GetMIRLabelIdx());
  ans.push_back(gotoNode);
  return ans;
}

// ---------- FEIRStmtSwitch ----------
FEIRStmtSwitch::FEIRStmtSwitch(UniqueFEIRExpr argExpr)
    : FEIRStmt(FEIRNodeKind::kStmtSwitch),
      defaultLabelIdx(0),
      defaultTarget(nullptr) {
  SetExpr(std::move(argExpr));
}

FEIRStmtSwitch::~FEIRStmtSwitch() {
  defaultTarget = nullptr;
}

std::list<StmtNode*> FEIRStmtSwitch::GenMIRStmtsImpl(MIRBuilder &mirBuilder) const {
  std::list<StmtNode*> ans;
  CaseVector switchTable(mirBuilder.GetCurrentFuncCodeMpAllocator()->Adapter());
  for (const std::pair<int32, FEIRStmtPesudoLabel*> &targetPair : mapValueTargets) {
    CHECK_NULL_FATAL(targetPair.second);
    switchTable.push_back(std::make_pair(targetPair.first, targetPair.second->GetMIRLabelIdx()));
  }
  BaseNode *exprNode = expr->GenMIRNode(mirBuilder);
  CHECK_NULL_FATAL(defaultTarget);
  SwitchNode *switchNode = mirBuilder.CreateStmtSwitch(exprNode, defaultTarget->GetMIRLabelIdx(), switchTable);
  ans.push_back(switchNode);
  return ans;
}

// ---------- FEIRStmtArrayStore ----------
FEIRStmtArrayStore::FEIRStmtArrayStore(UniqueFEIRExpr argExprElem, UniqueFEIRExpr argExprArray,
                                       UniqueFEIRExpr argExprIndex, UniqueFEIRType argTypeArray)
    : FEIRStmt(FEIRNodeKind::kStmtArrayStore),
      exprElem(std::move(argExprElem)),
      exprArray(std::move(argExprArray)),
      exprIndex(std::move(argExprIndex)),
      typeArray(std::move(argTypeArray)) {}

std::list<StmtNode*> FEIRStmtArrayStore::GenMIRStmtsImpl(MIRBuilder &mirBuilder) const {
  CHECK_FATAL(exprArray->GetKind() == kExprDRead, "only support dread expr for exprArray");
  CHECK_FATAL(exprIndex->GetKind() == kExprDRead, "only support dread expr for exprIndex");
  BaseNode *addrBase = exprArray->GenMIRNode(mirBuilder);
  BaseNode *indexBn = exprIndex->GenMIRNode(mirBuilder);
  MIRType *ptrMIRArrayType = typeArray->GenerateMIRType(true);
  BaseNode *arrayExpr = mirBuilder.CreateExprArray(*ptrMIRArrayType, addrBase, indexBn);
  UniqueFEIRType typeElem = typeArray->Clone();
  (void)typeElem->ArrayDecrDim();
  MIRType *ptrMIRElemType = typeElem->GenerateMIRType(true);
  BaseNode *elemBn = exprElem->GenMIRNode(mirBuilder);
  IassignNode *stmt = mirBuilder.CreateStmtIassign(*ptrMIRElemType, 0, arrayExpr, elemBn);
  return std::list<StmtNode*>({ stmt });
}

// ---------- FEIRStmtFieldStore ----------
FEIRStmtFieldStore::FEIRStmtFieldStore(UniqueFEIRVar argVarObj, UniqueFEIRVar argVarField,
                                       FEStructFieldInfo &argFieldInfo, bool argIsStatic)
    : FEIRStmt(FEIRNodeKind::kStmtFieldStore),
      varObj(std::move(argVarObj)),
      varField(std::move(argVarField)),
      fieldInfo(argFieldInfo),
      isStatic(argIsStatic) {}

std::list<StmtNode*> FEIRStmtFieldStore::GenMIRStmtsImpl(MIRBuilder &mirBuilder) const {
  // prepare and find root
  fieldInfo.Prepare(mirBuilder, isStatic);
  if (isStatic) {
    return GenMIRStmtsImplForStatic(mirBuilder);
  } else {
    return GenMIRStmtsImplForNonStatic(mirBuilder);
  }
}

std::list<StmtNode*> FEIRStmtFieldStore::GenMIRStmtsImplForStatic(MIRBuilder &mirBuilder) const {
  CHECK_FATAL(fieldInfo.GetFieldNameIdx() != 0, "invalid name idx");
  UniqueFEIRVar varTarget = std::make_unique<FEIRVarName>(fieldInfo.GetFieldNameIdx(), fieldInfo.GetType()->Clone());
  varTarget->SetGlobal(true);
  UniqueFEIRExpr exprDRead = FEIRBuilder::CreateExprDRead(varField->Clone());
  UniqueFEIRStmt stmtDAssign = FEIRBuilder::CreateStmtDAssign(std::move(varTarget), std::move(exprDRead));
  return stmtDAssign->GenMIRStmts(mirBuilder);
}

std::list<StmtNode*> FEIRStmtFieldStore::GenMIRStmtsImplForNonStatic(MIRBuilder &mirBuilder) const {
  std::list<StmtNode*> ans;
  FieldID fieldID = fieldInfo.GetFieldID();
  CHECK_FATAL(fieldID != 0, "invalid field ID");
  MIRStructType *structType = FEManager::GetTypeManager().GetStructTypeFromName(fieldInfo.GetStructName());
  CHECK_NULL_FATAL(structType);
  MIRType *ptrStructType = GlobalTables::GetTypeTable().GetOrCreatePointerType(*structType, PTY_ref);
  UniqueFEIRExpr exprDReadObj = FEIRBuilder::CreateExprDRead(varObj->Clone());
  UniqueFEIRExpr exprDReadField = FEIRBuilder::CreateExprDRead(varField->Clone());
  BaseNode *nodeObj = exprDReadObj->GenMIRNode(mirBuilder);
  BaseNode *nodeField = exprDReadField->GenMIRNode(mirBuilder);
  StmtNode *stmt = mirBuilder.CreateStmtIassign(*ptrStructType, fieldID, nodeObj, nodeField);
  ans.push_back(stmt);
  return ans;
}

// ---------- FEIRStmtFieldLoad ----------
FEIRStmtFieldLoad::FEIRStmtFieldLoad(UniqueFEIRVar argVarObj, UniqueFEIRVar argVarField,
                                     FEStructFieldInfo &argFieldInfo, bool argIsStatic)
    : FEIRStmtAssign(FEIRNodeKind::kStmtFieldLoad, std::move(argVarField)),
      varObj(std::move(argVarObj)),
      fieldInfo(argFieldInfo),
      isStatic(argIsStatic) {}

std::list<StmtNode*> FEIRStmtFieldLoad::GenMIRStmtsImpl(MIRBuilder &mirBuilder) const {
  // prepare and find root
  fieldInfo.Prepare(mirBuilder, isStatic);
  if (isStatic) {
    return GenMIRStmtsImplForStatic(mirBuilder);
  } else {
    return GenMIRStmtsImplForNonStatic(mirBuilder);
  }
}

std::list<StmtNode*> FEIRStmtFieldLoad::GenMIRStmtsImplForStatic(MIRBuilder &mirBuilder) const {
  UniqueFEIRVar varTarget = std::make_unique<FEIRVarName>(fieldInfo.GetFieldNameIdx(), fieldInfo.GetType()->Clone());
  varTarget->SetGlobal(true);
  UniqueFEIRExpr exprDRead = FEIRBuilder::CreateExprDRead(std::move(varTarget));
  UniqueFEIRStmt stmtDAssign = FEIRBuilder::CreateStmtDAssign(var->Clone(), std::move(exprDRead));
  return stmtDAssign->GenMIRStmts(mirBuilder);
}

std::list<StmtNode*> FEIRStmtFieldLoad::GenMIRStmtsImplForNonStatic(MIRBuilder &mirBuilder) const {
  std::list<StmtNode*> ans;
  FieldID fieldID = fieldInfo.GetFieldID();
  CHECK_FATAL(fieldID != 0, "invalid field ID");
  MIRStructType *structType = FEManager::GetTypeManager().GetStructTypeFromName(fieldInfo.GetStructName());
  CHECK_NULL_FATAL(structType);
  MIRType *ptrStructType = GlobalTables::GetTypeTable().GetOrCreatePointerType(*structType, PTY_ref);
  MIRType *fieldType = fieldInfo.GetType()->GenerateMIRTypeAuto(fieldInfo.GetSrcLang());
  UniqueFEIRExpr exprDReadObj = FEIRBuilder::CreateExprDRead(varObj->Clone());
  BaseNode *nodeObj = exprDReadObj->GenMIRNode(mirBuilder);
  BaseNode *nodeVal = mirBuilder.CreateExprIread(*fieldType, *ptrStructType, fieldID, nodeObj);
  MIRSymbol *valRet = var->GenerateLocalMIRSymbol(mirBuilder);
  StmtNode *stmt = mirBuilder.CreateStmtDassign(*valRet, 0, nodeVal);
  ans.push_back(stmt);
  return ans;
}

// ---------- FEIRStmtCallAssign ----------
std::map<Opcode, Opcode> FEIRStmtCallAssign::mapOpAssignToOp = FEIRStmtCallAssign::InitMapOpAssignToOp();
std::map<Opcode, Opcode> FEIRStmtCallAssign::mapOpToOpAssign = FEIRStmtCallAssign::InitMapOpToOpAssign();

FEIRStmtCallAssign::FEIRStmtCallAssign(FEStructMethodInfo &argMethodInfo, Opcode argMIROp, UniqueFEIRVar argVarRet,
                                       bool argIsStatic)
    : FEIRStmtAssign(FEIRNodeKind::kStmtCallAssign, std::move(argVarRet)),
      methodInfo(argMethodInfo),
      mirOp(argMIROp),
      isStatic(argIsStatic) {}

std::map<Opcode, Opcode> FEIRStmtCallAssign::InitMapOpAssignToOp() {
  std::map<Opcode, Opcode> ans;
  ans[OP_callassigned] = OP_call;
  ans[OP_virtualcallassigned] = OP_virtualcall;
  ans[OP_superclasscallassigned] = OP_superclasscall;
  ans[OP_interfacecallassigned] = OP_interfacecall;
  return ans;
}

std::map<Opcode, Opcode> FEIRStmtCallAssign::InitMapOpToOpAssign() {
  std::map<Opcode, Opcode> ans;
  ans[OP_call] = OP_callassigned;
  ans[OP_virtualcall] = OP_virtualcallassigned;
  ans[OP_superclasscall] = OP_superclasscallassigned;
  ans[OP_interfacecall] = OP_interfacecallassigned;
  return ans;
}

std::list<StmtNode*> FEIRStmtCallAssign::GenMIRStmtsImpl(MIRBuilder &mirBuilder) const {
  std::list<StmtNode*> ans;
  StmtNode *stmtCall = nullptr;
  // prepare and find root
  methodInfo.Prepare(mirBuilder, isStatic);
  if (methodInfo.IsJavaPolymorphicCall() || methodInfo.IsJavaDynamicCall()) {
    return GenMIRStmtsUseZeroReturn(mirBuilder);
  }
  Opcode op = AdjustMIROp();
  MapleVector<BaseNode*> args(mirBuilder.GetCurrentFuncCodeMpAllocator()->Adapter());
  for (const UniqueFEIRExpr &exprArg : exprArgs) {
    BaseNode *node = exprArg->GenMIRNode(mirBuilder);
    args.push_back(node);
  }
  PUIdx puIdx = methodInfo.GetPuIdx();
  if (methodInfo.IsReturnVoid()) {
    stmtCall = mirBuilder.CreateStmtCall(puIdx, args, op);
  } else {
    MIRSymbol *retVarSym = var->GenerateLocalMIRSymbol(mirBuilder);
    stmtCall = mirBuilder.CreateStmtCallAssigned(puIdx, args, retVarSym, op);
  }
  ans.push_back(stmtCall);
  return ans;
}

std::list<StmtNode*> FEIRStmtCallAssign::GenMIRStmtsUseZeroReturn(MIRBuilder &mirBuilder) const {
  std::list<StmtNode*> ans;
  if (methodInfo.IsReturnVoid()) {
    return ans;
  }
  const UniqueFEIRType &retType = methodInfo.GetReturnType();
  MIRType *mirRetType = retType->GenerateMIRTypeAuto(kSrcLangJava);
  MIRSymbol *mirRetSym = var->GenerateLocalMIRSymbol(mirBuilder);
  BaseNode *nodeZero;
  if (mirRetType->IsScalarType()) {
    switch (mirRetType->GetPrimType()) {
      case PTY_u1:
      case PTY_i8:
      case PTY_i16:
      case PTY_u16:
      case PTY_i32:
        nodeZero = mirBuilder.CreateIntConst(0, PTY_i32);
        break;
      case PTY_i64:
        nodeZero = mirBuilder.CreateIntConst(0, PTY_i64);
        break;
      case PTY_f32:
        nodeZero = mirBuilder.CreateFloatConst(0.0f);
        break;
      case PTY_f64:
        nodeZero = mirBuilder.CreateDoubleConst(0.0);
        break;
      default:
        nodeZero = mirBuilder.CreateIntConst(0, PTY_i32);
        break;
    }
  } else {
    nodeZero = mirBuilder.CreateIntConst(0, PTY_ref);
  }
  StmtNode *stmt = mirBuilder.CreateStmtDassign(mirRetSym->GetStIdx(), 0, nodeZero);
  ans.push_back(stmt);
  return ans;
}

Opcode FEIRStmtCallAssign::AdjustMIROp() const {
  if (methodInfo.IsReturnVoid()) {
    auto it = mapOpAssignToOp.find(mirOp);
    if (it != mapOpAssignToOp.end()) {
      return it->second;
    }
  } else {
    auto it = mapOpToOpAssign.find(mirOp);
    if (it != mapOpToOpAssign.end()) {
      return it->second;
    }
  }
  return mirOp;
}

// ---------- FEIRExpr ----------
FEIRExpr::FEIRExpr(FEIRNodeKind argKind)
    : kind(argKind),
      isNestable(true),
      isAddrof(false),
      hasException(false) {
  type = std::make_unique<FEIRTypeDefault>();
}

FEIRExpr::FEIRExpr(FEIRNodeKind argKind, std::unique_ptr<FEIRType> argType)
    : kind(argKind),
      isNestable(true),
      isAddrof(false),
      hasException(false) {
  SetType(std::move(argType));
}

std::vector<FEIRVar*> FEIRExpr::GetVarUsesImpl() const {
  return std::vector<FEIRVar*>();
}

bool FEIRExpr::IsNestableImpl() const {
  return isNestable;
}

bool FEIRExpr::IsAddrofImpl() const {
  return isAddrof;
}

bool FEIRExpr::HasExceptionImpl() const {
  return hasException;
}

// ---------- FEIRExprConst ----------
FEIRExprConst::FEIRExprConst()
    : FEIRExpr(FEIRNodeKind::kExprConst) {
  ASSERT(type != nullptr, "type is nullptr");
  type->SetPrimType(PTY_i32);
  value.raw = 0;
}

FEIRExprConst::FEIRExprConst(int64 val, PrimType argType)
    : FEIRExpr(FEIRNodeKind::kExprConst) {
  ASSERT(type != nullptr, "type is nullptr");
  type->SetPrimType(argType);
  value.valueI64 = val;
}

FEIRExprConst::FEIRExprConst(uint64 val, PrimType argType)
    : FEIRExpr(FEIRNodeKind::kExprConst) {
  ASSERT(type != nullptr, "type is nullptr");
  type->SetPrimType(argType);
  value.valueU64 = val;
}

FEIRExprConst::FEIRExprConst(float val)
    : FEIRExpr(FEIRNodeKind::kExprConst) {
  ASSERT(type != nullptr, "type is nullptr");
  type->SetPrimType(PTY_f32);
  value.valueF32 = val;
}

FEIRExprConst::FEIRExprConst(double val)
    : FEIRExpr(FEIRNodeKind::kExprConst) {
  ASSERT(type != nullptr, "type is nullptr");
  type->SetPrimType(PTY_f64);
  value.valueF64 = val;
}

std::unique_ptr<FEIRExpr> FEIRExprConst::CloneImpl() const {
  std::unique_ptr<FEIRExpr> expr = std::make_unique<FEIRExprConst>();
  FEIRExprConst *exprConst = static_cast<FEIRExprConst*>(expr.get());
  exprConst->value.raw = value.raw;
  ASSERT(type != nullptr, "type is nullptr");
  exprConst->type->SetPrimType(type->GetPrimType());
  return expr;
}

BaseNode *FEIRExprConst::GenMIRNodeImpl(MIRBuilder &mirBuilder) const {
  PrimType primType = GetPrimType();
  switch (primType) {
    case PTY_u1:
    case PTY_u8:
    case PTY_u16:
    case PTY_u32:
    case PTY_u64:
    case PTY_i8:
    case PTY_i16:
    case PTY_i32:
    case PTY_i64:
    case PTY_ref:
    case PTY_ptr:
      return mirBuilder.CreateIntConst(value.valueI64, primType);
    case PTY_f32:
      return mirBuilder.CreateFloatConst(value.valueF32);
    case PTY_f64:
      return mirBuilder.CreateDoubleConst(value.valueF64);
    default:
      ERR(kLncErr, "unsupported const kind");
      return nullptr;
  }
}

// ---------- FEIRExprDRead ----------
FEIRExprDRead::FEIRExprDRead(std::unique_ptr<FEIRVar> argVarSrc)
    : FEIRExpr(FEIRNodeKind::kExprDRead) {
  SetVarSrc(std::move(argVarSrc));
}

FEIRExprDRead::FEIRExprDRead(std::unique_ptr<FEIRType> argType, std::unique_ptr<FEIRVar> argVarSrc)
    : FEIRExpr(FEIRNodeKind::kExprDRead, std::move(argType)) {
  SetVarSrc(std::move(argVarSrc));
}

std::unique_ptr<FEIRExpr> FEIRExprDRead::CloneImpl() const {
  std::unique_ptr<FEIRExpr> expr = std::make_unique<FEIRExprDRead>(type->Clone(), varSrc->Clone());
  return expr;
}

BaseNode *FEIRExprDRead::GenMIRNodeImpl(MIRBuilder &mirBuilder) const {
  MIRType *type = varSrc->GetType()->GenerateMIRTypeAuto();
  MIRSymbol *symbol = varSrc->GenerateMIRSymbol(mirBuilder);
  ASSERT(type != nullptr, "type is nullptr");
  AddrofNode *node = mirBuilder.CreateExprDread(*type, *symbol);
  return node;
}

void FEIRExprDRead::SetVarSrc(std::unique_ptr<FEIRVar> argVarSrc) {
  CHECK_FATAL(argVarSrc != nullptr, "input is nullptr");
  varSrc = std::move(argVarSrc);
  SetType(varSrc->GetType()->Clone());
}

std::vector<FEIRVar*> FEIRExprDRead::GetVarUsesImpl() const {
  return std::vector<FEIRVar*>({ varSrc.get() });
}

// ---------- FEIRExprUnary ----------
std::map<Opcode, bool> FEIRExprUnary::mapOpNestable = FEIRExprUnary::InitMapOpNestableForExprUnary();

FEIRExprUnary::FEIRExprUnary(Opcode argOp, std::unique_ptr<FEIRExpr> argOpnd)
    : FEIRExpr(kExprUnary),
      op(argOp) {
  SetOpnd(std::move(argOpnd));
  SetExprTypeByOp();
}

FEIRExprUnary::FEIRExprUnary(std::unique_ptr<FEIRType> argType, Opcode argOp, std::unique_ptr<FEIRExpr> argOpnd)
    : FEIRExpr(kExprUnary, std::move(argType)),
      op(argOp) {
  SetOpnd(std::move(argOpnd));
  SetExprTypeByOp();
}

std::map<Opcode, bool> FEIRExprUnary::InitMapOpNestableForExprUnary() {
  std::map<Opcode, bool> ans;
  ans[OP_abs] = true;
  ans[OP_bnot] = true;
  ans[OP_lnot] = true;
  ans[OP_neg] = true;
  ans[OP_recip] = true;
  ans[OP_sqrt] = true;
  ans[OP_gcmallocjarray] = false;
  return ans;
}

std::unique_ptr<FEIRExpr> FEIRExprUnary::CloneImpl() const {
  std::unique_ptr<FEIRExpr> expr = std::make_unique<FEIRExprUnary>(type->Clone(), op, opnd->Clone());
  return expr;
}

BaseNode *FEIRExprUnary::GenMIRNodeImpl(MIRBuilder &mirBuilder) const {
  MIRType *mirType =
      GlobalTables::GetTypeTable().GetTypeFromTyIdx(TyIdx(static_cast<uint32>(GetTypeRef().GetPrimType())));
  ASSERT(mirType != nullptr, "mir type is nullptr");
  BaseNode *nodeOpnd = opnd->GenMIRNode(mirBuilder);
  BaseNode *expr = mirBuilder.CreateExprUnary(op, *mirType, nodeOpnd);
  return expr;
}

std::vector<FEIRVar*> FEIRExprUnary::GetVarUsesImpl() const {
  return opnd->GetVarUses();
}

void FEIRExprUnary::SetOpnd(std::unique_ptr<FEIRExpr> argOpnd) {
  CHECK_FATAL(argOpnd != nullptr, "opnd is nullptr");
  opnd = std::move(argOpnd);
}

void FEIRExprUnary::SetExprTypeByOp() {
  switch (op) {
    case OP_neg:
      type->SetPrimType(opnd->GetPrimType());
      break;
    default:
      break;
  }
}

// ---------- FEIRExprTypeCvt ----------
std::map<Opcode, bool> FEIRExprTypeCvt::mapOpNestable = FEIRExprTypeCvt::InitMapOpNestableForTypeCvt();
std::map<Opcode, FEIRExprTypeCvt::FuncPtrGenMIRNode> FEIRExprTypeCvt::funcPtrMapForParseExpr =
    FEIRExprTypeCvt::InitFuncPtrMapForParseExpr();

FEIRExprTypeCvt::FEIRExprTypeCvt(Opcode argOp, std::unique_ptr<FEIRExpr> argOpnd)
    : FEIRExprUnary(argOp, std::move(argOpnd)) {}

FEIRExprTypeCvt::FEIRExprTypeCvt(std::unique_ptr<FEIRType> exprType, Opcode argOp, std::unique_ptr<FEIRExpr> argOpnd)
    : FEIRExprUnary(std::move(exprType), argOp, std::move(argOpnd)) {}

std::unique_ptr<FEIRExpr> FEIRExprTypeCvt::CloneImpl() const {
  std::unique_ptr<FEIRExpr> expr = std::make_unique<FEIRExprTypeCvt>(type->Clone(), op, opnd->Clone());
  return expr;
}

BaseNode *FEIRExprTypeCvt::GenMIRNodeImpl(MIRBuilder &mirBuilder) const {
  auto ptrFunc = funcPtrMapForParseExpr.find(op);
  ASSERT(ptrFunc != funcPtrMapForParseExpr.end(), "unsupported op: %s", kOpcodeInfo.GetName(op));
  return (this->*(ptrFunc->second))(mirBuilder);
}

BaseNode *FEIRExprTypeCvt::GenMIRNodeMode1(MIRBuilder &mirBuilder) const {
  // MIR: op <to-type> <from-type> (<opnd0>)
  MIRType *mirTypeDst =
      GlobalTables::GetTypeTable().GetTypeFromTyIdx(TyIdx(static_cast<uint32>(GetTypeRef().GetPrimType())));
  MIRType *mirTypeSrc = GlobalTables::GetTypeTable().GetTypeFromTyIdx(TyIdx(static_cast<uint32>(opnd->GetPrimType())));
  BaseNode *nodeOpnd = opnd->GenMIRNode(mirBuilder);
  BaseNode *expr = mirBuilder.CreateExprTypeCvt(op, *mirTypeDst, *mirTypeSrc, nodeOpnd);
  return expr;
}

BaseNode *FEIRExprTypeCvt::GenMIRNodeMode2(MIRBuilder &mirBuilder) const {
  // MIR: op <prim-type> <float-type> (<opnd0>)
  PrimType primTypeSrc = opnd->GetPrimType();
  CHECK_FATAL(IsPrimitiveFloat(primTypeSrc), "from type must be float type");
  return GenMIRNodeMode1(mirBuilder);
}

BaseNode *FEIRExprTypeCvt::GenMIRNodeMode3(MIRBuilder &mirBuilder) const {
  // MIR: retype <prim-type> <type> (<opnd0>)
  MIRType *mirTypeDst = GetTypeRef().GenerateMIRType();
  MIRType *mirTypeSrc = opnd->GetTypeRef().GenerateMIRType();
  BaseNode *nodeOpnd = opnd->GenMIRNode(mirBuilder);
  BaseNode *expr = mirBuilder.CreateExprRetype(*mirTypeDst, *mirTypeSrc, nodeOpnd);
  return expr;
}

std::map<Opcode, bool> FEIRExprTypeCvt::InitMapOpNestableForTypeCvt() {
  std::map<Opcode, bool> ans;
  ans[OP_ceil] = true;
  ans[OP_cvt] = true;
  ans[OP_floor] = true;
  ans[OP_retype] = true;
  ans[OP_round] = true;
  ans[OP_trunc] = true;
  return ans;
}

std::map<Opcode, FEIRExprTypeCvt::FuncPtrGenMIRNode> FEIRExprTypeCvt::InitFuncPtrMapForParseExpr() {
  std::map<Opcode, FuncPtrGenMIRNode> ans;
  ans[OP_ceil] = &FEIRExprTypeCvt::GenMIRNodeMode2;
  ans[OP_cvt] = &FEIRExprTypeCvt::GenMIRNodeMode1;
  ans[OP_floor] = &FEIRExprTypeCvt::GenMIRNodeMode2;
  ans[OP_retype] = &FEIRExprTypeCvt::GenMIRNodeMode3;
  ans[OP_round] = &FEIRExprTypeCvt::GenMIRNodeMode2;
  ans[OP_trunc] = &FEIRExprTypeCvt::GenMIRNodeMode2;
  return ans;
}

// ---------- FEIRExprExtractBits ----------
std::map<Opcode, bool> FEIRExprExtractBits::mapOpNestable = FEIRExprExtractBits::InitMapOpNestableForExtractBits();
std::map<Opcode, FEIRExprExtractBits::FuncPtrGenMIRNode> FEIRExprExtractBits::funcPtrMapForParseExpr =
    FEIRExprExtractBits::InitFuncPtrMapForParseExpr();

FEIRExprExtractBits::FEIRExprExtractBits(Opcode argOp, PrimType argPrimType, uint8 argBitOffset, uint8 argBitSize,
                                         std::unique_ptr<FEIRExpr> argOpnd)
    : FEIRExprUnary(argOp, std::move(argOpnd)),
      bitOffset(argBitOffset),
      bitSize(argBitSize) {
  CHECK_FATAL(IsPrimitiveInteger(argPrimType), "only integer type is supported");
  type->SetPrimType(argPrimType);
}

FEIRExprExtractBits::FEIRExprExtractBits(Opcode argOp, PrimType argPrimType, std::unique_ptr<FEIRExpr> argOpnd)
    : FEIRExprExtractBits(argOp, argPrimType, 0, 0, std::move(argOpnd)) {}

std::unique_ptr<FEIRExpr> FEIRExprExtractBits::CloneImpl() const {
  std::unique_ptr<FEIRExpr> expr = std::make_unique<FEIRExprExtractBits>(op, type->GetPrimType(), bitOffset, bitSize,
                                                                         opnd->Clone());
  return expr;
}

BaseNode *FEIRExprExtractBits::GenMIRNodeImpl(MIRBuilder &mirBuilder) const {
  auto ptrFunc = funcPtrMapForParseExpr.find(op);
  ASSERT(ptrFunc != funcPtrMapForParseExpr.end(), "unsupported op: %s", kOpcodeInfo.GetName(op));
  return (this->*(ptrFunc->second))(mirBuilder);
}

std::map<Opcode, bool> FEIRExprExtractBits::InitMapOpNestableForExtractBits() {
  std::map<Opcode, bool> ans;
  ans[OP_extractbits] = true;
  ans[OP_sext] = true;
  ans[OP_zext] = true;
  return ans;
}

std::map<Opcode, FEIRExprExtractBits::FuncPtrGenMIRNode> FEIRExprExtractBits::InitFuncPtrMapForParseExpr() {
  std::map<Opcode, FuncPtrGenMIRNode> ans;
  ans[OP_extractbits] = &FEIRExprExtractBits::GenMIRNodeForExtrabits;
  ans[OP_sext] = &FEIRExprExtractBits::GenMIRNodeForExt;
  ans[OP_zext] = &FEIRExprExtractBits::GenMIRNodeForExt;
  return ans;
}

BaseNode *FEIRExprExtractBits::GenMIRNodeForExtrabits(MIRBuilder &mirBuilder) const {
  ASSERT(opnd != nullptr, "nullptr check");
  PrimType primTypeDst = GetTypeRef().GetPrimType();
  PrimType primTypeSrc = opnd->GetPrimType();
  CHECK_FATAL(FEUtils::IsInteger(primTypeDst), "dst type of extrabits must integer");
  CHECK_FATAL(FEUtils::IsInteger(primTypeSrc), "src type of extrabits must integer");
  uint8 widthDst = FEUtils::GetWidth(primTypeDst);
  uint8 widthSrc = FEUtils::GetWidth(primTypeSrc);
  CHECK_FATAL(widthDst >= bitSize, "dst width is not enough");
  CHECK_FATAL(widthSrc >= bitOffset + bitSize, "src width is not enough");
  MIRType *mirTypeDst = GlobalTables::GetTypeTable().GetTypeFromTyIdx(TyIdx(static_cast<uint32>(primTypeDst)));
  BaseNode *nodeOpnd = opnd->GenMIRNode(mirBuilder);
  BaseNode *expr = mirBuilder.CreateExprExtractbits(op, *mirTypeDst, bitOffset, bitSize, nodeOpnd);
  return expr;
}

BaseNode *FEIRExprExtractBits::GenMIRNodeForExt(MIRBuilder &mirBuilder) const {
  ASSERT(opnd != nullptr, "nullptr check");
  PrimType primTypeDst = GetTypeRef().GetPrimType();
  PrimType primTypeSrc = opnd->GetPrimType();
  CHECK_FATAL(FEUtils::IsInteger(primTypeSrc), "src type of sext/zext must integer");
  CHECK_FATAL(FEUtils::IsInteger(primTypeDst), "dst type of sext/zext must integer");
  uint8 widthDst = FEUtils::GetWidth(primTypeDst);
  uint8 widthSrc = (bitSize == 0) ? FEUtils::GetWidth(primTypeSrc) : bitSize;
  CHECK_FATAL(widthDst >= widthSrc, "width of dst must be not smaller than width of src");
  MIRType *mirTypeDst = GlobalTables::GetTypeTable().GetTypeFromTyIdx(TyIdx(static_cast<uint32>(primTypeDst)));
  BaseNode *nodeOpnd = opnd->GenMIRNode(mirBuilder);
  BaseNode *expr = mirBuilder.CreateExprExtractbits(op, *mirTypeDst, 0, widthSrc, nodeOpnd);
  return expr;
}

// ---------- FEIRExprBinary ----------
std::map<Opcode, FEIRExprBinary::FuncPtrGenMIRNode> FEIRExprBinary::funcPtrMapForGenMIRNode =
    FEIRExprBinary::InitFuncPtrMapForGenMIRNode();

FEIRExprBinary::FEIRExprBinary(Opcode argOp, std::unique_ptr<FEIRExpr> argOpnd0, std::unique_ptr<FEIRExpr> argOpnd1)
    : FEIRExpr(FEIRNodeKind::kExprBinary),
      op(argOp) {
  SetOpnd0(std::move(argOpnd0));
  SetOpnd1(std::move(argOpnd1));
  SetExprTypeByOp();
}

FEIRExprBinary::FEIRExprBinary(std::unique_ptr<FEIRType> exprType, Opcode argOp, std::unique_ptr<FEIRExpr> argOpnd0,
                               std::unique_ptr<FEIRExpr> argOpnd1)
    : FEIRExpr(FEIRNodeKind::kExprBinary, std::move(exprType)),
      op(argOp) {
  SetOpnd0(std::move(argOpnd0));
  SetOpnd1(std::move(argOpnd1));
  SetExprTypeByOp();
}

std::unique_ptr<FEIRExpr> FEIRExprBinary::CloneImpl() const {
  std::unique_ptr<FEIRExpr> expr = std::make_unique<FEIRExprBinary>(type->Clone(), op, opnd0->Clone(), opnd1->Clone());
  return expr;
}

BaseNode *FEIRExprBinary::GenMIRNodeImpl(MIRBuilder &mirBuilder) const {
  auto ptrFunc = funcPtrMapForGenMIRNode.find(op);
  ASSERT(ptrFunc != funcPtrMapForGenMIRNode.end(), "unsupported op: %s", kOpcodeInfo.GetName(op));
  return (this->*(ptrFunc->second))(mirBuilder);
}

std::vector<FEIRVar*> FEIRExprBinary::GetVarUsesImpl() const {
  std::vector<FEIRVar*> ans;
  for (FEIRVar *var : opnd0->GetVarUses()) {
    ans.push_back(var);
  }
  for (FEIRVar *var : opnd1->GetVarUses()) {
    ans.push_back(var);
  }
  return ans;
}

bool FEIRExprBinary::IsNestableImpl() const {
  return true;
}

bool FEIRExprBinary::IsAddrofImpl() const {
  return false;
}

void FEIRExprBinary::SetOpnd0(std::unique_ptr<FEIRExpr> argOpnd) {
  CHECK_FATAL(argOpnd != nullptr, "input is nullptr");
  opnd0 = std::move(argOpnd);
}

void FEIRExprBinary::SetOpnd1(std::unique_ptr<FEIRExpr> argOpnd) {
  CHECK_FATAL(argOpnd != nullptr, "input is nullptr");
  opnd1 = std::move(argOpnd);
}

std::map<Opcode, FEIRExprBinary::FuncPtrGenMIRNode> FEIRExprBinary::InitFuncPtrMapForGenMIRNode() {
  std::map<Opcode, FEIRExprBinary::FuncPtrGenMIRNode> ans;
  ans[OP_add] = &FEIRExprBinary::GenMIRNodeNormal;
  ans[OP_ashr] = &FEIRExprBinary::GenMIRNodeNormal;
  ans[OP_band] = &FEIRExprBinary::GenMIRNodeNormal;
  ans[OP_bior] = &FEIRExprBinary::GenMIRNodeNormal;
  ans[OP_bxor] = &FEIRExprBinary::GenMIRNodeNormal;
  ans[OP_cand] = &FEIRExprBinary::GenMIRNodeNormal;
  ans[OP_cior] = &FEIRExprBinary::GenMIRNodeNormal;
  ans[OP_cmp] = &FEIRExprBinary::GenMIRNodeCompare;
  ans[OP_cmpg] = &FEIRExprBinary::GenMIRNodeCompare;
  ans[OP_cmpl] = &FEIRExprBinary::GenMIRNodeCompare;
  ans[OP_div] = &FEIRExprBinary::GenMIRNodeNormal;
  ans[OP_eq] = &FEIRExprBinary::GenMIRNodeCompareU1;
  ans[OP_ge] = &FEIRExprBinary::GenMIRNodeCompareU1;
  ans[OP_gt] = &FEIRExprBinary::GenMIRNodeCompareU1;
  ans[OP_land] = &FEIRExprBinary::GenMIRNodeNormal;
  ans[OP_lior] = &FEIRExprBinary::GenMIRNodeNormal;
  ans[OP_le] = &FEIRExprBinary::GenMIRNodeCompareU1;
  ans[OP_lshr] = &FEIRExprBinary::GenMIRNodeNormal;
  ans[OP_lt] = &FEIRExprBinary::GenMIRNodeCompareU1;
  ans[OP_max] = &FEIRExprBinary::GenMIRNodeNormal;
  ans[OP_min] = &FEIRExprBinary::GenMIRNodeNormal;
  ans[OP_mul] = &FEIRExprBinary::GenMIRNodeNormal;
  ans[OP_ne] = &FEIRExprBinary::GenMIRNodeCompareU1;
  ans[OP_rem] = &FEIRExprBinary::GenMIRNodeNormal;
  ans[OP_shl] = &FEIRExprBinary::GenMIRNodeNormal;
  ans[OP_sub] = &FEIRExprBinary::GenMIRNodeNormal;
  return ans;
}

BaseNode *FEIRExprBinary::GenMIRNodeNormal(MIRBuilder &mirBuilder) const {
  MIRType *mirTypeDst = GlobalTables::GetTypeTable().GetTypeFromTyIdx(TyIdx(static_cast<uint32>(type->GetPrimType())));
  BaseNode *nodeOpnd0 = opnd0->GenMIRNode(mirBuilder);
  BaseNode *nodeOpnd1 = opnd1->GenMIRNode(mirBuilder);
  BaseNode *expr = mirBuilder.CreateExprBinary(op, *mirTypeDst, nodeOpnd0, nodeOpnd1);
  return expr;
}

BaseNode *FEIRExprBinary::GenMIRNodeCompare(MIRBuilder &mirBuilder) const {
  PrimType primTypeOpnd0 = opnd0->GetPrimType();
  PrimType primTypeOpnd1 = opnd1->GetPrimType();
  CHECK_FATAL(primTypeOpnd0 == primTypeOpnd1, "primtype of opnds must be the same");
  MIRType *mirTypeDst = GlobalTables::GetTypeTable().GetTypeFromTyIdx(TyIdx(static_cast<uint32>(type->GetPrimType())));
  MIRType *mirTypeSrc = GlobalTables::GetTypeTable().GetTypeFromTyIdx(TyIdx(primTypeOpnd0));
  BaseNode *nodeOpnd0 = opnd0->GenMIRNode(mirBuilder);
  BaseNode *nodeOpnd1 = opnd1->GenMIRNode(mirBuilder);
  BaseNode *expr = mirBuilder.CreateExprCompare(op, *mirTypeDst, *mirTypeSrc, nodeOpnd0, nodeOpnd1);
  return expr;
}

BaseNode *FEIRExprBinary::GenMIRNodeCompareU1(MIRBuilder &mirBuilder) const {
  PrimType primTypeOpnd0 = opnd0->GetPrimType();
  PrimType primTypeOpnd1 = opnd1->GetPrimType();
  CHECK_FATAL(primTypeOpnd0 == primTypeOpnd1, "primtype of opnds must be the same");
  MIRType *mirTypeSrc = GlobalTables::GetTypeTable().GetTypeFromTyIdx(TyIdx(static_cast<uint32>(primTypeOpnd0)));
  BaseNode *nodeOpnd0 = opnd0->GenMIRNode(mirBuilder);
  BaseNode *nodeOpnd1 = opnd1->GenMIRNode(mirBuilder);
  MIRType *mirTypeU1 = GlobalTables::GetTypeTable().GetUInt1();
  BaseNode *expr = mirBuilder.CreateExprCompare(op, *mirTypeU1, *mirTypeSrc, nodeOpnd0, nodeOpnd1);
  return expr;
}

void FEIRExprBinary::SetExprTypeByOp() {
  switch (op) {
    // Normal
    case OP_add:
    case OP_div:
    case OP_max:
    case OP_min:
    case OP_mul:
    case OP_rem:
    case OP_sub:
      SetExprTypeByOpNormal();
      break;
    // Shift
    case OP_ashr:
    case OP_lshr:
    case OP_shl:
      SetExprTypeByOpShift();
      break;
    // Logic
    case OP_band:
    case OP_bior:
    case OP_bxor:
    case OP_cand:
    case OP_cior:
    case OP_land:
    case OP_lior:
      SetExprTypeByOpLogic();
      break;
    // Compare
    case OP_cmp:
    case OP_cmpg:
    case OP_eq:
    case OP_ge:
    case OP_gt:
    case OP_le:
    case OP_lt:
    case OP_ne:
      SetExprTypeByOpCompare();
      break;
    default:
      break;
  }
}

void FEIRExprBinary::SetExprTypeByOpNormal() {
  PrimType primTypeOpnd0 = opnd0->GetPrimType();
  PrimType primTypeOpnd1 = opnd1->GetPrimType();
  CHECK_FATAL(primTypeOpnd0 == primTypeOpnd1, "primtype of opnds must be the same");
  type->SetPrimType(primTypeOpnd0);
}

void FEIRExprBinary::SetExprTypeByOpShift() {
  PrimType primTypeOpnd0 = opnd0->GetPrimType();
  PrimType primTypeOpnd1 = opnd1->GetPrimType();
  CHECK_FATAL(IsPrimitiveInteger(primTypeOpnd0), "logic's opnd0 must be integer");
  CHECK_FATAL(IsPrimitiveInteger(primTypeOpnd1), "logic's opnd1 must be integer");
  type->SetPrimType(primTypeOpnd0);
}

void FEIRExprBinary::SetExprTypeByOpLogic() {
  PrimType primTypeOpnd0 = opnd0->GetPrimType();
  PrimType primTypeOpnd1 = opnd1->GetPrimType();
  CHECK_FATAL(primTypeOpnd0 == primTypeOpnd1, "primtype of opnds must be the same");
  CHECK_FATAL(IsPrimitiveInteger(primTypeOpnd0), "logic's opnds must be integer");
  type->SetPrimType(primTypeOpnd0);
}

void FEIRExprBinary::SetExprTypeByOpCompare() {
  PrimType primTypeOpnd0 = opnd0->GetPrimType();
  PrimType primTypeOpnd1 = opnd1->GetPrimType();
  CHECK_FATAL(primTypeOpnd0 == primTypeOpnd1, "primtype of opnds must be the same");
  type->SetPrimType(PTY_i32);
}

// ---------- FEIRExprTernary ----------
FEIRExprTernary::FEIRExprTernary(Opcode argOp, std::unique_ptr<FEIRExpr> argOpnd0, std::unique_ptr<FEIRExpr> argOpnd1,
                                 std::unique_ptr<FEIRExpr> argOpnd2)
    : FEIRExpr(FEIRNodeKind::kExprTernary),
      op(argOp) {
  SetOpnd(std::move(argOpnd0), 0);
  SetOpnd(std::move(argOpnd1), 1);
  SetOpnd(std::move(argOpnd2), 2);
  SetExprTypeByOp();
}

std::unique_ptr<FEIRExpr> FEIRExprTernary::CloneImpl() const {
  std::unique_ptr<FEIRExpr> expr = std::make_unique<FEIRExprTernary>(op, opnd0->Clone(), opnd1->Clone(),
                                                                     opnd2->Clone());
  return expr;
}

BaseNode *FEIRExprTernary::GenMIRNodeImpl(MIRBuilder &mirBuilder) const {
  MIRType *mirTypeDst = GlobalTables::GetTypeTable().GetTypeFromTyIdx(TyIdx(static_cast<uint32>(type->GetPrimType())));
  BaseNode *nodeOpnd0 = opnd0->GenMIRNode(mirBuilder);
  BaseNode *nodeOpnd1 = opnd1->GenMIRNode(mirBuilder);
  BaseNode *nodeOpnd2 = opnd2->GenMIRNode(mirBuilder);
  BaseNode *expr = mirBuilder.CreateExprTernary(op, *mirTypeDst, nodeOpnd0, nodeOpnd1, nodeOpnd2);
  return expr;
}

std::vector<FEIRVar*> FEIRExprTernary::GetVarUsesImpl() const {
  std::vector<FEIRVar*> ans;
  for (FEIRVar *var : opnd0->GetVarUses()) {
    ans.push_back(var);
  }
  for (FEIRVar *var : opnd1->GetVarUses()) {
    ans.push_back(var);
  }
  for (FEIRVar *var : opnd2->GetVarUses()) {
    ans.push_back(var);
  }
  return ans;
}

bool FEIRExprTernary::IsNestableImpl() const {
  return true;
}

bool FEIRExprTernary::IsAddrofImpl() const {
  return false;
}

void FEIRExprTernary::SetOpnd(std::unique_ptr<FEIRExpr> argOpnd, uint32 idx) {
  CHECK_FATAL(argOpnd != nullptr, "input is nullptr");
  switch (idx) {
    case 0:
      opnd0 = std::move(argOpnd);
      break;
    case 1:
      opnd1 = std::move(argOpnd);
      break;
    case 2:
      opnd2 = std::move(argOpnd);
      break;
    default:
      CHECK_FATAL(false, "index out of range");
  }
}

void FEIRExprTernary::SetExprTypeByOp() {
  PrimType primTypeOpnd1 = opnd1->GetPrimType();
  PrimType primTypeOpnd2 = opnd2->GetPrimType();
  CHECK_FATAL(primTypeOpnd1 == primTypeOpnd2, "primtype of opnds must be the same");
  type->SetPrimType(primTypeOpnd1);
}

// ---------- FEIRExprNary ----------
FEIRExprNary::FEIRExprNary(Opcode argOp)
    : FEIRExpr(FEIRNodeKind::kExprNary),
      op(argOp) {}

std::vector<FEIRVar*> FEIRExprNary::GetVarUsesImpl() const {
  std::vector<FEIRVar*> ans;
  for (const std::unique_ptr<FEIRExpr> &opnd : opnds) {
    for (FEIRVar *var : opnd->GetVarUses()) {
      ans.push_back(var);
    }
  }
  return ans;
}

void FEIRExprNary::AddOpnd(std::unique_ptr<FEIRExpr> argOpnd) {
  CHECK_FATAL(argOpnd != nullptr, "input opnd is nullptr");
  opnds.push_back(std::move(argOpnd));
}

void FEIRExprNary::AddOpnds(const std::vector<std::unique_ptr<FEIRExpr>> &argOpnds) {
  for (const std::unique_ptr<FEIRExpr> &opnd : argOpnds) {
    ASSERT_NOT_NULL(opnd);
    AddOpnd(opnd->Clone());
  }
}

void FEIRExprNary::ResetOpnd() {
  opnds.clear();
}

// ---------- FEIRExprIntrinsicop ----------
FEIRExprIntrinsicop::FEIRExprIntrinsicop(std::unique_ptr<FEIRType> exprType, MIRIntrinsicID argIntrinsicID)
    : FEIRExprNary(OP_intrinsicop),
      intrinsicID(argIntrinsicID) {
  kind = FEIRNodeKind::kExprIntrinsicop;
  SetType(std::move(exprType));
}

FEIRExprIntrinsicop::FEIRExprIntrinsicop(std::unique_ptr<FEIRType> exprType, MIRIntrinsicID argIntrinsicID,
                                         std::unique_ptr<FEIRType> argParamType)
    : FEIRExprNary(OP_intrinsicopwithtype),
      intrinsicID(argIntrinsicID) {
  kind = FEIRNodeKind::kExprIntrinsicop;
  SetType(std::move(exprType));
  paramType = std::move(argParamType);
}

FEIRExprIntrinsicop::FEIRExprIntrinsicop(std::unique_ptr<FEIRType> exprType, MIRIntrinsicID argIntrinsicID,
                                         const std::vector<std::unique_ptr<FEIRExpr>> &argOpnds)
    : FEIRExprIntrinsicop(std::move(exprType), argIntrinsicID) {
  AddOpnds(argOpnds);
}

FEIRExprIntrinsicop::FEIRExprIntrinsicop(std::unique_ptr<FEIRType> exprType, MIRIntrinsicID argIntrinsicID,
                                         std::unique_ptr<FEIRType> argParamType,
                                         const std::vector<std::unique_ptr<FEIRExpr>> &argOpnds)
    : FEIRExprIntrinsicop(std::move(exprType), argIntrinsicID, std::move(argParamType)) {
  AddOpnds(argOpnds);
}

std::unique_ptr<FEIRExpr> FEIRExprIntrinsicop::CloneImpl() const {
  if (op == OP_intrinsicop) {
    return std::make_unique<FEIRExprIntrinsicop>(type->Clone(), intrinsicID, opnds);
  } else {
    CHECK_FATAL(paramType != nullptr, "error: param type is not set");
    return std::make_unique<FEIRExprIntrinsicop>(type->Clone(), intrinsicID, paramType->Clone(), opnds);
  }
}

BaseNode *FEIRExprIntrinsicop::GenMIRNodeImpl(MIRBuilder &mirBuilder) const {
  return nullptr;
}

bool FEIRExprIntrinsicop::IsNestableImpl() const {
  return false;
}

bool FEIRExprIntrinsicop::IsAddrofImpl() const {
  return false;
}

// ---------- FEIRExprJavaNewInstance ----------
FEIRExprJavaNewInstance::FEIRExprJavaNewInstance(UniqueFEIRType argType)
    : FEIRExpr(FEIRNodeKind::kExprJavaNewInstance) {
  SetType(std::move(argType));
}

std::unique_ptr<FEIRExpr> FEIRExprJavaNewInstance::CloneImpl() const {
  std::unique_ptr<FEIRExpr> expr = std::make_unique<FEIRExprJavaNewInstance>(type->Clone());
  CHECK_NULL_FATAL(expr);
  return expr;
}

BaseNode *FEIRExprJavaNewInstance::GenMIRNodeImpl(MIRBuilder &mirBuilder) const {
  MIRType *mirType = type->GenerateMIRType(kSrcLangJava, false);
  MIRType *ptrType = GlobalTables::GetTypeTable().GetOrCreatePointerType(*mirType, PTY_ref);
  BaseNode *expr = mirBuilder.CreateExprGCMalloc(OP_gcmalloc, *ptrType, *mirType);
  CHECK_NULL_FATAL(expr);
  return expr;
}

// ---------- FEIRExprJavaNewArray ----------
FEIRExprJavaNewArray::FEIRExprJavaNewArray(UniqueFEIRType argArrayType, UniqueFEIRExpr argExprSize)
    : FEIRExpr(FEIRNodeKind::kExprJavaNewArray) {
  SetArrayType(std::move(argArrayType));
  SetExprSize(std::move(argExprSize));
}

std::unique_ptr<FEIRExpr> FEIRExprJavaNewArray::CloneImpl() const {
  std::unique_ptr<FEIRExpr> expr = std::make_unique<FEIRExprJavaNewArray>(arrayType->Clone(), exprSize->Clone());
  CHECK_NULL_FATAL(expr);
  return expr;
}

BaseNode *FEIRExprJavaNewArray::GenMIRNodeImpl(MIRBuilder &mirBuilder) const {
  MIRType *mirType = arrayType->GenerateMIRType(kSrcLangJava, false);
  MIRType *ptrType = GlobalTables::GetTypeTable().GetOrCreatePointerType(*mirType, PTY_ref);
  BaseNode *sizeNode = exprSize->GenMIRNode(mirBuilder);
  BaseNode *expr = mirBuilder.CreateExprJarrayMalloc(OP_gcmallocjarray, *ptrType, *mirType, sizeNode);
  CHECK_NULL_FATAL(expr);
  return expr;
}

// ---------- FEIRExprJavaArrayLength ----------
FEIRExprJavaArrayLength::FEIRExprJavaArrayLength(UniqueFEIRExpr argExprArray)
    : FEIRExpr(FEIRNodeKind::kExprJavaArrayLength) {
  SetExprArray(std::move(argExprArray));
}

std::unique_ptr<FEIRExpr> FEIRExprJavaArrayLength::CloneImpl() const {
  UniqueFEIRExpr expr = std::make_unique<FEIRExprJavaArrayLength>(exprArray->Clone());
  CHECK_NULL_FATAL(expr);
  return expr;
}

BaseNode *FEIRExprJavaArrayLength::GenMIRNodeImpl(MIRBuilder &mirBuilder) const {
  BaseNode *arrayNode = exprArray->GenMIRNode(mirBuilder);
  MapleVector<BaseNode*> args(mirBuilder.GetCurrentFuncCodeMpAllocator()->Adapter());
  args.push_back(arrayNode);
  MIRType *retType = GlobalTables::GetTypeTable().GetInt32();
  return mirBuilder.CreateExprIntrinsicop(INTRN_JAVA_ARRAY_LENGTH, OP_intrinsicop, *retType, args);
}

// ---------- FEIRExprArrayLoad ----------
FEIRExprArrayLoad::FEIRExprArrayLoad(UniqueFEIRExpr argExprArray, UniqueFEIRExpr argExprIndex,
                                     UniqueFEIRType argTypeArray)
    : FEIRExpr(FEIRNodeKind::kExprArrayLoad),
      exprArray(std::move(argExprArray)),
      exprIndex(std::move(argExprIndex)),
      typeArray(std::move(argTypeArray)) {}

std::unique_ptr<FEIRExpr> FEIRExprArrayLoad::CloneImpl() const {
  std::unique_ptr<FEIRExpr> expr = std::make_unique<FEIRExprArrayLoad>(exprArray->Clone(), exprIndex->Clone(),
                                                                       typeArray->Clone());
  return expr;
}

BaseNode *FEIRExprArrayLoad::GenMIRNodeImpl(MIRBuilder &mirBuilder) const {
  CHECK_FATAL(exprArray->GetKind() == kExprDRead, "only support dread expr for exprArray");
  CHECK_FATAL(exprIndex->GetKind() == kExprDRead, "only support dread expr for exprIndex");
  BaseNode *addrBase = exprArray->GenMIRNode(mirBuilder);
  BaseNode *indexBn = exprIndex->GenMIRNode(mirBuilder);
  MIRType *ptrMIRArrayType = typeArray->GenerateMIRType(true);
  BaseNode *arrayExpr = mirBuilder.CreateExprArray(*ptrMIRArrayType, addrBase, indexBn);
  UniqueFEIRType typeElem = typeArray->Clone();
  (void)typeElem->ArrayDecrDim();
  MIRType *ptrMIRElemType = typeElem->GenerateMIRType(true);
  MIRType *mirElemType = typeElem->GenerateMIRType(false);
  BaseNode *elemBn = mirBuilder.CreateExprIread(*mirElemType, *ptrMIRElemType, 0, arrayExpr);
  return elemBn;
}

// ---------- FEIRStmtPesudoLabel ----------
FEIRStmtPesudoLabel::FEIRStmtPesudoLabel(uint32 argLabelIdx)
    : FEIRStmt(kStmtPesudoLabel),
      labelIdx(argLabelIdx),
      mirLabelIdx(0) {}

std::list<StmtNode*> FEIRStmtPesudoLabel::GenMIRStmtsImpl(MIRBuilder &mirBuilder) const {
  std::list<StmtNode*> ans;
  StmtNode *stmtLabel = mirBuilder.CreateStmtLabel(mirLabelIdx);
  ans.push_back(stmtLabel);
  return ans;
}

void FEIRStmtPesudoLabel::GenerateLabelIdx(MIRBuilder &mirBuilder) {
  std::stringstream ss;
  ss << "label" << MPLFEEnv::GetInstance().GetGlobalLabelIdx();
  MPLFEEnv::GetInstance().IncrGlobalLabelIdx();
  mirLabelIdx = mirBuilder.GetOrCreateMIRLabel(ss.str());
}

// ---------- FEIRStmtPesudoLOC ----------
FEIRStmtPesudoLOC::FEIRStmtPesudoLOC(uint32 argSrcFileIdx, uint32 argLineNumber)
    : FEIRStmt(kStmtPesudoLOC),
      srcFileIdx(argSrcFileIdx),
      lineNumber(argLineNumber) {}

std::list<StmtNode*> FEIRStmtPesudoLOC::GenMIRStmtsImpl(MIRBuilder &mirBuilder) const {
  return std::list<StmtNode*>();
}

// ---------- FEIRStmtPesudoJavaTry ----------
FEIRStmtPesudoJavaTry::FEIRStmtPesudoJavaTry()
    : FEIRStmt(kStmtPesudoJavaTry) {}

std::list<StmtNode*> FEIRStmtPesudoJavaTry::GenMIRStmtsImpl(MIRBuilder &mirBuilder) const {
  std::list<StmtNode*> ans;
  MapleVector<LabelIdx> vec(mirBuilder.GetCurrentFuncCodeMpAllocator()->Adapter());
  for (FEIRStmtPesudoLabel *stmtLabel : catchTargets) {
    vec.push_back(stmtLabel->GetMIRLabelIdx());
  }
  StmtNode *stmtTry = mirBuilder.CreateStmtTry(vec);
  ans.push_back(stmtTry);
  return ans;
}

// ---------- FEIRStmtPesudoEndTry ----------
FEIRStmtPesudoEndTry::FEIRStmtPesudoEndTry()
    : FEIRStmt(kStmtPesudoEndTry) {}

std::list<StmtNode*> FEIRStmtPesudoEndTry::GenMIRStmtsImpl(MIRBuilder &mirBuilder) const {
  std::list<StmtNode*> ans;
  MemPool *mp = mirBuilder.GetCurrentFuncCodeMp();
  ASSERT(mp != nullptr, "mempool is nullptr");
  StmtNode *stmt = mp->New<StmtNode>(OP_endtry);
  ans.push_back(stmt);
  return ans;
}

// ---------- FEIRStmtPesudoCatch ----------
FEIRStmtPesudoCatch::FEIRStmtPesudoCatch(uint32 argLabelIdx)
    : FEIRStmtPesudoLabel(argLabelIdx) {}

std::list<StmtNode*> FEIRStmtPesudoCatch::GenMIRStmtsImpl(MIRBuilder &mirBuilder) const {
  std::list<StmtNode*> ans;
  StmtNode *stmtLabel = mirBuilder.CreateStmtLabel(mirLabelIdx);
  ans.push_back(stmtLabel);
  MapleVector<TyIdx> vec(mirBuilder.GetCurrentFuncCodeMpAllocator()->Adapter());
  for (const UniqueFEIRType &type : catchTypes) {
    MIRType *mirType = type->GenerateMIRType(kSrcLangJava, true);
    vec.push_back(mirType->GetTypeIndex());
  }
  StmtNode *stmtCatch = mirBuilder.CreateStmtCatch(vec);
  ans.push_back(stmtCatch);
  return ans;
}

void FEIRStmtPesudoCatch::AddCatchTypeNameIdx(GStrIdx typeNameIdx) {
  UniqueFEIRType type = std::make_unique<FEIRTypeDefault>(PTY_ref, typeNameIdx);
  catchTypes.push_back(std::move(type));
}

// ---------- FEIRStmtPesudoComment ----------
FEIRStmtPesudoComment::FEIRStmtPesudoComment(FEIRNodeKind argKind)
    : FEIRStmt(argKind) {}

FEIRStmtPesudoComment::FEIRStmtPesudoComment(const std::string &argContent)
    : FEIRStmt(kStmtPesudoComment),
      content(argContent) {}

std::list<StmtNode*> FEIRStmtPesudoComment::GenMIRStmtsImpl(MIRBuilder &mirBuilder) const {
  std::list<StmtNode*> ans;
  StmtNode *stmt = mirBuilder.CreateStmtComment(content);
  ans.push_back(stmt);
  return ans;
}

// ---------- FEIRStmtPesudoCommentForInst ----------
FEIRStmtPesudoCommentForInst::FEIRStmtPesudoCommentForInst()
    : FEIRStmtPesudoComment(kStmtPesudoCommentForInst) {}

std::list<StmtNode*> FEIRStmtPesudoCommentForInst::GenMIRStmtsImpl(MIRBuilder &mirBuilder) const {
  std::list<StmtNode*> ans;
  return ans;
}
}  // namespace maple
