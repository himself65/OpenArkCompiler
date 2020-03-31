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
#include "jbc_function.h"
#include "unistd.h"
#include "fe_macros.h"
#include "fe_algorithm.h"
#include "feir_var_reg.h"
#include "jbc_attr.h"
#include "fe_timer.h"

namespace maple {
JBCFunction::JBCFunction(const JBCClassMethod2FEHelper &argMethodHelper, MIRFunction &mirFunc,
                         const std::unique_ptr<FEFunctionPhaseResult> &argPhaseResultTotal)
    : FEFunction(mirFunc, argPhaseResultTotal),
      methodHelper(argMethodHelper),
      method(methodHelper.GetMethod()),
      error(false),
      pesudoBBCatchPred(nullptr) {
}

JBCFunction::~JBCFunction() {
  pesudoBBCatchPred = nullptr;
}

void JBCFunction::InitImpl() {
  FEFunction::InitImpl();
  generalCFG = std::make_unique<JBCFunctionCFG>(method, *genStmtHead, *genStmtTail);
  generalCFG->Init();
  pesudoBBCatchPred = RegisterGeneralBB(std::make_unique<JBCBBPesudoCatchPred>());
}

void JBCFunction::PreProcessImpl() {
}

void JBCFunction::ProcessImpl() {
  FE_INFO_LEVEL(FEOptions::kDumpLevelInfoDetail, "JBCFunction::Process() for %s", method.GetFullName().c_str());
  bool success = true;
  success = success && GenerateGeneralStmt("create general stmt");
  success = success && BuildGeneralBB("build general bb");
  success = success && BuildGeneralCFG("build general cfg");
  success = success && CheckDeadBB("check dead bb");
  success = success && LabelGeneralStmts("label general stmt");
  success = success && LabelGeneralBBs("label general bb");
  success = success && LabelLabelIdx("label label idx");
  success = success && CheckJVMStack("check jvm stack");
  success = success && GenerateArgVarList("gen arg var list");
  success = success && ProcessFunctionArgs("process func args");
  success = success && EmitToFEIRStmt("emit to feir");
  success = success && ProcessFEIRFunction();
  if (!success) {
    error = true;
    ERR(kLncErr, "JBCFunction::Process() failed for %s", method.GetFullName().c_str());
  }
}

void JBCFunction::FinishImpl() {
  bool dumpFunc = true;
  dumpFunc = (FEOptions::GetInstance().IsDumpJBCErrorOnly() && error) ||
             FEOptions::GetInstance().IsDumpJBCAll() ||
             FEOptions::GetInstance().IsDumpJBCFuncName(method.GetFullName());
  if (dumpFunc) {
    (void)BuildGeneralStmtBBMap("gen stmt_bb_map");
  }
  if (FEOptions::GetInstance().IsDumpJBCStmt() && dumpFunc) {
    DumpGeneralStmts();
  }
  if (FEOptions::GetInstance().IsDumpJBCBB() && dumpFunc) {
    DumpGeneralBBs();
  }
  if (FEOptions::GetInstance().IsDumpGeneralCFGGraph() && dumpFunc) {
    DumpGeneralCFGGraph();
  }
  (void)UpdateFormal("finish/update formal");
  (void)EmitToMIR("finish/emit to mir");
  (void)ReleaseGenStmts("finish/release memory");
  bool recordTime = FEOptions::GetInstance().IsDumpPhaseTime() || FEOptions::GetInstance().IsDumpPhaseTimeDetail();
  if (phaseResultTotal != nullptr && recordTime) {
    phaseResultTotal->Combine(phaseResult);
  }
  if (FEOptions::GetInstance().IsDumpPhaseTimeDetail()) {
    INFO(kLncInfo, "[PhaseTime] function: %s", method.GetFullName().c_str());
    phaseResult.Dump();
  }
}

bool JBCFunction::PreProcessTypeNameIdx() {
  return false;
}

bool JBCFunction::GenerateGeneralStmt(const std::string &phaseName) {
  phaseResult.RegisterPhaseNameAndStart(phaseName);
  const jbc::JBCAttrCode *code = method.GetCode();
  if (phaseResult.IsSuccess() && code != nullptr) {
    BuildStmtFromInstruction(*code);
    BuildStmtForCatch(*code);
    BuildStmtForTry(*code);
    BuildStmtForLOC(*code);
    if (FEOptions::GetInstance().IsDumpInstComment()) {
      BuildStmtForInstComment(*code);
    }
    ArrangeStmts();
  }
  return phaseResult.Finish(true);
}

void JBCFunction::GenerateGeneralStmtFailCallBack() {
}

void JBCFunction::GenerateGeneralDebugInfo() {
}

bool JBCFunction::VerifyGeneral() {
  return false;
}

void JBCFunction::VerifyGeneralFailCallBack() {
}

bool JBCFunction::GenerateArgVarList(const std::string &phaseName) {
  phaseResult.RegisterPhaseNameAndStart(phaseName);
  InitStack2FEHelper();
  uint32 slotIdx = 0;
  for (FEIRType *type : methodHelper.GetArgTypes()) {
    PrimType pt = type->GetPrimType();
    std::unique_ptr<FEIRVar> var = std::make_unique<FEIRVarReg>(stack2feHelper.GetRegNumForSlot(slotIdx),
                                                                type->Clone());
    argVarList.push_back(std::move(var));
    if (type->IsScalar() && (pt == PTY_i64 || pt == PTY_f64)) {
      slotIdx += JBCStack2FEHelper::kRegNumOffWide;
    } else {
      slotIdx += JBCStack2FEHelper::kRegNumOff;
    }
  }
  return phaseResult.Finish();
}

bool JBCFunction::ProcessFunctionArgs(const std::string &phaseName) {
  phaseResult.RegisterPhaseNameAndStart(phaseName);
  for (const UniqueFEIRVar &argVar : argVarList) {
    ASSERT(argVar != nullptr, "nullptr check for argVar");
    const UniqueFEIRType &type = argVar->GetType();
    ASSERT(type != nullptr, "nullptr check for type");
    if (!type->IsScalar()) {
      continue;
    }
    PrimType pty = type->GetPrimType();
    bool useZExt = (pty == PTY_u1 || pty == PTY_u16);
    bool useSExt = (pty == PTY_i8 || pty == PTY_i16);
    if (useZExt || useSExt) {
      UniqueFEIRVar dstVar = argVar->Clone();
      dstVar->GetType()->SetPrimType(PTY_i32);
      UniqueFEIRExpr exprExt = useZExt ? FEIRBuilder::CreateExprZExt(argVar->Clone()) :
                                         FEIRBuilder::CreateExprSExt(argVar->Clone());
      UniqueFEIRStmt stmtExt = FEIRBuilder::CreateStmtDAssign(std::move(dstVar), std::move(exprExt));
      FEIRStmt *ptrFEIRStmt = RegisterFEIRStmt(std::move(stmtExt));
      feirStmtTail->InsertBefore(ptrFEIRStmt);
    }
  }
  return phaseResult.Finish();
}

bool JBCFunction::EmitToFEIRStmt(const std::string &phaseName) {
  phaseResult.RegisterPhaseNameAndStart(phaseName);
  const FELinkListNode *bbNode = genBBHead->GetNext();
  while (bbNode != nullptr && bbNode != genBBTail) {
    const JBCBB *bb = static_cast<const JBCBB*>(bbNode);
    EmitToFEIRStmt(*bb);
    bbNode = bbNode->GetNext();
  }
  return phaseResult.Finish();
}

void JBCFunction::EmitToFEIRStmt(const JBCBB &bb) {
  bool success = false;
  std::list<UniqueFEIRStmt> feirStmts = stack2feHelper.LoadSwapStack(bb.GetMinStackIn(), success);
  if (success == false) {
    ERR(kLncErr, "LoadSwapStack Error for %s", method.GetFullName().c_str());
    return;
  }
  AppendFEIRStmts(feirStmts);
  const FELinkListNode *stmtNode = bb.GetStmtHead();
  while (stmtNode != nullptr && success) {
    const JBCStmt *stmt = static_cast<const JBCStmt*>(stmtNode);
    if (stmt->GetKind() == JBCStmtKind::kJBCStmtInstBranch) {
      const JBCStmtInstBranch *stmtBranch = static_cast<const JBCStmtInstBranch*>(stmt);
      feirStmts = stmtBranch->EmitToFEIRWithLabel(stack2feHelper, mapPCLabelStmt, success);
    } else {
      feirStmts = stmt->EmitToFEIR(stack2feHelper, method.GetConstPool(), success);
    }
    AppendFEIRStmts(feirStmts);
    if (stmtNode == bb.GetStmtTail()) {
      break;
    }
    stmtNode = stmtNode->GetNext();
  }
}

std::string JBCFunction::GetGeneralFuncName() const {
  return method.GetFullName();
}

GeneralBB *JBCFunction::NewGeneralBB() {
  return new JBCBB(method.GetConstPool());
}

GeneralBB *JBCFunction::NewGeneralBB(uint8 argBBKind) {
  return new JBCBB(argBBKind, method.GetConstPool());
}

bool JBCFunction::BuildGeneralBB(const std::string &phaseName) {
  phaseResult.RegisterPhaseNameAndStart(phaseName);
  bool success = phaseResult.IsSuccess();
  genBBHead = generalCFG->GetDummyHead();
  genBBTail = generalCFG->GetDummyTail();
  if (success) {
    generalCFG->BuildBB();
  }
  return phaseResult.Finish(success);
}

bool JBCFunction::BuildGeneralCFG(const std::string &phaseName) {
  phaseResult.RegisterPhaseNameAndStart(phaseName);
  bool success = phaseResult.IsSuccess();
  success = success && generalCFG->BuildCFG();
  // process catch BB
  ASSERT(pesudoBBCatchPred != nullptr, "nullptr check");
  FELinkListNode *nodeBB = genBBHead->GetNext();
  while (nodeBB != nullptr && nodeBB != genBBTail) {
    GeneralBB *bb = static_cast<GeneralBB*>(nodeBB);
    const JBCStmt *stmtHead = static_cast<const JBCStmt*>(bb->GetStmtNoAuxHead());
    if (stmtHead != nullptr && stmtHead->GetKind() == JBCStmtKind::kJBCStmtPesudoCatch) {
      bb->AddPredBB(pesudoBBCatchPred);
      pesudoBBCatchPred->AddSuccBB(bb);
    }
    nodeBB = nodeBB->GetNext();
  }
  return phaseResult.Finish(success);
}

bool JBCFunction::CheckJVMStack(const std::string &phaseName) {
  phaseResult.RegisterPhaseNameAndStart(phaseName);
  bool success = phaseResult.IsSuccess();
  FELinkListNode *nodeBB = genBBHead->GetNext();
  if (nodeBB == genBBTail) {
    // empty function
    return phaseResult.Finish(success);
  }
  // initialize function head
  JBCBB *jbcBB = static_cast<JBCBB*>(nodeBB);
  success = success && jbcBB->InitForFuncHeader();
  // initialize catch
  for (GeneralBB *bbCatch : pesudoBBCatchPred->GetSuccBBs()) {
    JBCBB *jbcBBCatch = static_cast<JBCBB*>(bbCatch);
    success = success && jbcBBCatch->InitForCatch();
  }
  // process
  if (!success) {
    return phaseResult.Finish(false);
  }
  std::map<JBCBB*, std::set<JBCBB*>> correlation;
  while (nodeBB != genBBTail) {
    jbcBB = static_cast<JBCBB*>(nodeBB);
    for (GeneralBB *bb : jbcBB->GetPredBBs()) {
      if (bb != genBBHead && bb != pesudoBBCatchPred) {
        CHECK_FATAL(correlation[jbcBB].insert(static_cast<JBCBB*>(bb)).second, "correlation map insert failed");
      }
    }
    nodeBB = nodeBB->GetNext();
  }
  CorrelativeMerge<JBCBB> stackUpdater(correlation, &JBCBB::UpdateStackByPredBB, &JBCBB::UpdateStackByPredBBEnd);
  stackUpdater.ProcessAll();
  success = !stackUpdater.GetError();
  success = success && CheckJVMStackResult();
  return phaseResult.Finish(success);
}

bool JBCFunction::CheckJVMStackResult() {
  FELinkListNode *nodeBB = genBBHead->GetNext();
  while (nodeBB != nullptr && nodeBB != genBBTail) {
    JBCBB *jbcBB = static_cast<JBCBB*>(nodeBB);
    if (!jbcBB->CheckStack()) {
      return false;
    }
    nodeBB = nodeBB->GetNext();
  }
  return true;
}

void JBCFunction::BuildStmtFromInstruction(const jbc::JBCAttrCode &code) {
  GeneralStmt *stmt = nullptr;
  const MapleMap<uint32, jbc::JBCOp*> &instMap = code.GetInstMap();
  for (const std::pair<uint32, jbc::JBCOp*> &it : instMap) {
    uint32 pc = it.first;
    const jbc::JBCOp *op = it.second;
    ASSERT(op != nullptr, "null ptr check");
    switch (op->GetOpcodeKind()) {
      case jbc::kOpKindBranch: {
        const std::unique_ptr<GeneralStmt> &uniStmt =
            RegisterGeneralStmtUniqueReturn(std::make_unique<JBCStmtInstBranch>(*op));
        stmt = uniStmt.get();
        const jbc::JBCOpBranch *opBranch = static_cast<const jbc::JBCOpBranch*>(op);
        GeneralStmt *target = BuildAndUpdateLabel(opBranch->GetTarget(), uniStmt);
        static_cast<JBCStmtInstBranch*>(stmt)->AddSucc(target);
        break;
      }
      case jbc::kOpKindGoto: {
        const std::unique_ptr<GeneralStmt> &uniStmt =
            RegisterGeneralStmtUniqueReturn(std::make_unique<JBCStmtInstBranch>(*op));
        stmt = uniStmt.get();
        stmt->SetFallThru(false);
        const jbc::JBCOpGoto *opGoto = static_cast<const jbc::JBCOpGoto*>(op);
        GeneralStmt *target = BuildAndUpdateLabel(opGoto->GetTarget(), uniStmt);
        static_cast<JBCStmtInstBranch*>(stmt)->AddSucc(target);
        break;
      }
      case jbc::kOpKindSwitch:{
        const std::unique_ptr<GeneralStmt> &uniStmt =
            RegisterGeneralStmtUniqueReturn(std::make_unique<JBCStmtInstBranch>(*op));
        stmt = uniStmt.get();
        stmt->SetFallThru(false);
        const jbc::JBCOpSwitch *opSwitch = static_cast<const jbc::JBCOpSwitch*>(op);
        for (const std::pair<int32, uint32> &targetInfo : opSwitch->GetTargets()) {
          GeneralStmt *target = BuildAndUpdateLabel(targetInfo.second, uniStmt);
          static_cast<JBCStmtInstBranch*>(stmt)->AddSucc(target);
        }
        GeneralStmt *target = BuildAndUpdateLabel(opSwitch->GetDefaultTarget(), uniStmt);
        static_cast<JBCStmtInstBranch*>(stmt)->AddSucc(target);
        break;
      }
      default:
        stmt = RegisterGeneralStmt(std::make_unique<JBCStmtInst>(*op));
        break;
    }
    genStmtTail->InsertBefore(stmt);
    mapPCStmtInst[pc] = stmt;
  }
  mapPCStmtInst[code.GetCodeLength()] = genStmtTail;
}

void JBCFunction::BuildStmtForCatch(const jbc::JBCAttrCode &code) {
  const MapleVector<jbc::attr::ExceptionTableItem*> &exceptionInfo = code.GetExceptionInfos();
  for (const jbc::attr::ExceptionTableItem *item : exceptionInfo) {
    uint16 handlerPC = item->GetHandlerPC();
    JBCStmtPesudoCatch *stmtCatch = nullptr;
    auto it = mapPCCatchStmt.find(handlerPC);
    if (it == mapPCCatchStmt.end()) {
      stmtCatch = static_cast<JBCStmtPesudoCatch*>(RegisterGeneralStmt(std::make_unique<JBCStmtPesudoCatch>()));
      mapPCCatchStmt[handlerPC] = stmtCatch;
    } else {
      stmtCatch = static_cast<JBCStmtPesudoCatch*>(it->second);
    }
    const jbc::JBCConstClass *catchType = item->GetCatchType();
    if (catchType != nullptr) {
      stmtCatch->AddCatchTypeName(catchType->GetClassNameIdxMpl());
    } else {
      stmtCatch->AddCatchTypeName(GStrIdx(0));
    }
  }
}

void JBCFunction::BuildStmtForTry(const jbc::JBCAttrCode &code) {\
  std::map<std::pair<uint32, uint32>, std::vector<uint32>> rawInfo;
  std::map<uint32, uint32> outMapStartEnd;
  std::map<uint32, std::vector<uint32>> outMapStartCatch;
  const MapleVector<jbc::attr::ExceptionTableItem*> &exceptionInfo = code.GetExceptionInfos();
  for (const jbc::attr::ExceptionTableItem *item : exceptionInfo) {
    uint32 start = item->GetStartPC();
    uint32 end = item->GetEndPC();
    uint32 handlerPC = item->GetHandlerPC();
    rawInfo[std::make_pair(start, end)].push_back(handlerPC);
  }
  BuildTryInfo(rawInfo, outMapStartEnd, outMapStartCatch);
  for (const std::pair<uint32, uint32> &startEnd : outMapStartEnd) {
    // Try
    JBCStmtPesudoTry *stmtTry =
        static_cast<JBCStmtPesudoTry*>(RegisterGeneralStmt(std::make_unique<JBCStmtPesudoTry>()));
    auto it = outMapStartCatch.find(startEnd.first);
    CHECK_FATAL(it != outMapStartCatch.end(), "catch info not exist");
    for (uint32 handlerPC : it->second) {
      CHECK_FATAL(mapPCCatchStmt.find(handlerPC) != mapPCCatchStmt.end(), "catch stmt not exist");
      stmtTry->AddCatchStmt(mapPCCatchStmt[handlerPC]);
    }
    mapPCTryStmt[startEnd.first] = stmtTry;
    // EndTry
    JBCStmtPesudoEndTry *stmtEndTry =
        static_cast<JBCStmtPesudoEndTry*>(RegisterGeneralStmt(std::make_unique<JBCStmtPesudoEndTry>()));
    mapPCEndTryStmt[startEnd.second] = stmtEndTry;
  }
}

void JBCFunction::BuildTryInfo(const std::map<std::pair<uint32, uint32>, std::vector<uint32>> &rawInfo,
                               std::map<uint32, uint32> &outMapStartEnd,
                               std::map<uint32, std::vector<uint32>> &outMapStartCatch) {
  const uint8 flagStart = 0x1;
  const uint8 flagEnd = 0x2;
  const uint8 flagCatch = 0x4;
  std::map<uint32, uint8> keyPoints;
  std::map<uint32, std::set<uint32>> mapStartEnds;
  std::map<uint32, std::set<uint32>> mapEndStarts;
  for (auto info : rawInfo) {
    keyPoints[info.first.first] |= flagStart;
    keyPoints[info.first.second] |= flagEnd;
    for (uint32 handlerPC : info.second) {
      keyPoints[handlerPC] |= flagCatch;
    }
    CHECK_FATAL(mapStartEnds[info.first.first].insert(info.first.second).second, "mapStartEnds insert failed");
    CHECK_FATAL(mapEndStarts[info.first.second].insert(info.first.first).second, "mapEndStarts insert failed");
  }
  std::deque<std::pair<uint32, uint32>> blockQueue;
  const uint32 posInvalid = 0xFFFFFFFF;
  uint32 startPos = posInvalid;
  for (auto keyPoint : keyPoints) {
    uint32 curPos = keyPoint.first;
    uint8 flag = keyPoint.second;
    // set try block
    if (startPos != posInvalid) {
      outMapStartEnd[startPos] = curPos;
      BuildTryInfoCatch(rawInfo, blockQueue, startPos, outMapStartCatch);
      startPos = curPos;
    }
    // process end
    if ((flag & flagEnd) != 0) {
      blockQueue.push_back(std::make_pair(posInvalid, posInvalid));
      std::pair<uint32, uint32> top;
      do {
        top = blockQueue.front();
        blockQueue.pop_front();
        bool deleted = false;
        if (top.second == curPos) {
          ASSERT(mapEndStarts.find(curPos) != mapEndStarts.end(), "try end not found");
          if (mapEndStarts[curPos].find(top.first) != mapEndStarts[curPos].end()) {
            deleted = true;
          }
        }
        if (!deleted && top.first != posInvalid) {
          blockQueue.push_back(top);
        }
      } while (top.first != posInvalid);
      if (blockQueue.size() == 0) {
        startPos = posInvalid;
      }
    }
    // process start
    if ((flag & flagStart) != 0) {
      startPos = curPos;
      for (uint32 endPos : mapStartEnds[curPos]) {
        blockQueue.push_back(std::make_pair(curPos, endPos));
      }
    }
  }
}

void JBCFunction::BuildTryInfoCatch(const std::map<std::pair<uint32, uint32>, std::vector<uint32>> &rawInfo,
                                    const std::deque<std::pair<uint32, uint32>> &blockQueue,
                                    uint32 startPos,
                                    std::map<uint32, std::vector<uint32>> &outMapStartCatch) {
  std::set<uint32> catchSet;
  for (const std::pair<uint32, uint32> &block : blockQueue) {
    auto it = rawInfo.find(block);
    CHECK_FATAL(it != rawInfo.end(), "block not found");
    for (uint32 handlerPC : it->second) {
      if (catchSet.find(handlerPC) == catchSet.end()) {
        outMapStartCatch[startPos].push_back(handlerPC);
        CHECK_FATAL(catchSet.insert(handlerPC).second, "catchSet insert failed");
      }
    }
  }
}

void JBCFunction::BuildStmtForLOC(const jbc::JBCAttrCode &code) {
  const jbc::JBCAttr *attrRaw = code.GetAttr(jbc::JBCAttrKind::kAttrLineNumberTable);
  if (attrRaw == nullptr) {
    return;
  }
  const jbc::JBCAttrLineNumberTable *attrLineNumTab = static_cast<const jbc::JBCAttrLineNumberTable*>(attrRaw);
  uint32 srcFileIdx = method.GetClass().GetSrcFileInfoIdx();
  const MapleVector<jbc::attr::LineNumberTableItem*> &lineNums = attrLineNumTab->GetLineNums();
  for (const jbc::attr::LineNumberTableItem *item : lineNums) {
    JBCStmtPesudoLOC *stmtLOC =
        static_cast<JBCStmtPesudoLOC*>(RegisterGeneralStmt(std::make_unique<JBCStmtPesudoLOC>()));
    stmtLOC->SetSrcFileIdx(srcFileIdx);
    stmtLOC->SetLineNumber(item->GetLineNumber());
    mapPCStmtLOC[item->GetStartPC()] = stmtLOC;
  }
}

void JBCFunction::BuildStmtForInstComment(const jbc::JBCAttrCode &code) {
  const jbc::JBCAttr *attrRaw = code.GetAttr(jbc::JBCAttrKind::kAttrLineNumberTable);
  std::map<uint32, uint16> mapPCLineNum;
  if (attrRaw != nullptr) {
    const jbc::JBCAttrLineNumberTable *attrLineNumTab = static_cast<const jbc::JBCAttrLineNumberTable*>(attrRaw);
    const MapleVector<jbc::attr::LineNumberTableItem*> &lineNums = attrLineNumTab->GetLineNums();
    for (const jbc::attr::LineNumberTableItem *item : lineNums) {
      ASSERT_NOT_NULL(item);
      mapPCLineNum[item->GetStartPC()] = item->GetLineNumber();
    }
  }
  uint16 currLineNum = 0xFFFF;  // use 0xFFFF as invalid line number
  std::stringstream ss;
  const jbc::JBCConstPool &constPool = method.GetConstPool();
  const MapleMap<uint32, jbc::JBCOp*> &instMap = code.GetInstMap();
  for (const std::pair<uint32, jbc::JBCOp*> &it : instMap) {
    uint32 pc = it.first;
    jbc::JBCOp *op = it.second;
    if (mapPCLineNum.find(pc) != mapPCLineNum.end()) {
      currLineNum = mapPCLineNum[pc];
    }
    ASSERT(pc < 0x10000, "invalid pc");  // pc use 16bit in java bytecode (less than 0x10000)
    ss << "LINE " << srcFileName << " : ";
    if (currLineNum != 0xFFFF) {  // use 0xFFFF as invalid line number
      ss << std::dec << currLineNum;
    } else {
      ss << "unknown";
    }
    ss << ", PC : " << std::setfill(' ') << std::setw(5) << pc << "||" <<
          std::setfill('0') << std::setw(4) << std::hex << pc << " : " <<
          op->Dump(constPool);
    std::unique_ptr<JBCStmt> stmt = std::make_unique<JBCStmtPesudoComment>(ss.str());
    JBCStmtPesudoComment *ptrStmt = static_cast<JBCStmtPesudoComment*>(RegisterGeneralStmt(std::move(stmt)));
    mapPCCommentStmt[pc] = ptrStmt;
    ss.str("");
  }
}

GeneralStmt *JBCFunction::BuildAndUpdateLabel(uint32 dstPC, const std::unique_ptr<GeneralStmt> &srcStmt) {
  auto it = mapPCLabelStmt.find(dstPC);
  JBCStmtPesudoLabel *stmtLabel = nullptr;
  if (it == mapPCLabelStmt.end()) {
    stmtLabel = static_cast<JBCStmtPesudoLabel*>(RegisterGeneralStmt(std::make_unique<JBCStmtPesudoLabel>()));
    mapPCLabelStmt[dstPC] = stmtLabel;
  } else {
    stmtLabel = it->second;
  }
  ASSERT(stmtLabel != nullptr, "null ptr check");
  stmtLabel->AddPred(srcStmt.get());
  return stmtLabel;
}

void JBCFunction::ArrangeStmts() {
  /* Type of stmt: inst, label, try, endtry, catch, comment, loc
   *   endtry
   *   loc
   *   catch
   *   label
   *   try
   *   comment
   *   inst
   */
  for (const std::pair<uint32, GeneralStmt*> &pcInst : mapPCStmtInst) {
    uint32 pc = pcInst.first;
    GeneralStmt *stmtInst = pcInst.second;
    if (mapPCEndTryStmt.find(pc) != mapPCEndTryStmt.end()) {
      stmtInst->InsertBefore(mapPCEndTryStmt[pc]);
    }
    if (mapPCStmtLOC.find(pc) != mapPCStmtLOC.end()) {
      stmtInst->InsertBefore(mapPCStmtLOC[pc]);
    }
    if (mapPCCatchStmt.find(pc) != mapPCCatchStmt.end()) {
      stmtInst->InsertBefore(mapPCCatchStmt[pc]);
    }
    if (mapPCLabelStmt.find(pc) != mapPCLabelStmt.end()) {
      stmtInst->InsertBefore(mapPCLabelStmt[pc]);
    }
    if (mapPCTryStmt.find(pc) != mapPCTryStmt.end()) {
      stmtInst->InsertBefore(mapPCTryStmt[pc]);
    }
    if (mapPCCommentStmt.find(pc) != mapPCCommentStmt.end()) {
      stmtInst->InsertBefore(mapPCCommentStmt[pc]);
    }
  }
}

void JBCFunction::InitStack2FEHelper() {
  const jbc::JBCAttrCode *code = method.GetCode();
  // args
  uint32 nargs = 0;
  for (FEIRType *type : methodHelper.GetArgTypes()) {
    PrimType pt = type->GetPrimType();
    if (type->IsScalar() && (pt == PTY_i64 || pt == PTY_f64)) {
      nargs += JBCStack2FEHelper::kRegNumOffWide;
    } else {
      nargs += JBCStack2FEHelper::kRegNumOff;
    }
  }
  if (code == nullptr) {
    stack2feHelper.SetNArgs(nargs);
    stack2feHelper.SetNLocals(0);
    stack2feHelper.SetNStacks(0);
    stack2feHelper.SetNSwaps(0);
    return;
  }
  stack2feHelper.SetNArgs(nargs);
  CHECK_FATAL(nargs <= code->GetMaxLocals(), "invalid jbc method");
  stack2feHelper.SetNLocals(code->GetMaxLocals() - nargs);
  stack2feHelper.SetNStacks(code->GetMaxStack());
  stack2feHelper.SetNSwaps(CalculateMaxSwapSize());
  if (!FEOptions::GetInstance().IsDumpInstComment()) {
    return;
  }
  std::stringstream ss;
  ss << "StackToReg Info: nStacks=" << stack2feHelper.GetNStacks() <<
        ", nSwaps=" << stack2feHelper.GetNSwaps() <<
        ", maxLocals=" << code->GetMaxLocals() <<
        ", nArgs=" << stack2feHelper.GetNArgs();
  feirStmtTail->InsertBefore(RegisterGeneralStmt(FEIRBuilder::CreateStmtComment(ss.str())));
  feirStmtTail->InsertBefore(RegisterGeneralStmt(FEIRBuilder::CreateStmtComment("==== Reg Map ====")));
  uint32 regStart = 0;
  ss.str("");
  ss << "  " << regStart << " - " << (regStart + stack2feHelper.GetNStacks() - 1) << ": stacks";
  feirStmtTail->InsertBefore(RegisterGeneralStmt(FEIRBuilder::CreateStmtComment(ss.str())));
  regStart += stack2feHelper.GetNStacks();
  if (stack2feHelper.GetNSwaps() > 0) {
    ss.str("");
    ss << "  " << regStart << " - " << (regStart + stack2feHelper.GetNSwaps() - 1) << ": swaps";
    feirStmtTail->InsertBefore(RegisterGeneralStmt(FEIRBuilder::CreateStmtComment(ss.str())));
    regStart += stack2feHelper.GetNSwaps();
  }
  if (stack2feHelper.GetNLocals() > 0) {
    ss.str("");
    ss << "  " << regStart << " - " << (regStart + stack2feHelper.GetNLocals() - 1) << ": locals";
    feirStmtTail->InsertBefore(RegisterGeneralStmt(FEIRBuilder::CreateStmtComment(ss.str())));
    regStart += stack2feHelper.GetNLocals();
  }
  if (stack2feHelper.GetNArgs() > 0) {
    ss.str("");
    ss << "  " << regStart << " - " << (regStart + stack2feHelper.GetNArgs() - 1) << ": args";
    feirStmtTail->InsertBefore(RegisterGeneralStmt(FEIRBuilder::CreateStmtComment(ss.str())));
  }
  feirStmtTail->InsertBefore(RegisterGeneralStmt(FEIRBuilder::CreateStmtComment("=================")));
}

uint32 JBCFunction::CalculateMaxSwapSize() const {
  const FELinkListNode *nodeBB = genBBHead->GetNext();
  uint32 size = 0;
  while (nodeBB != nullptr && nodeBB != genBBTail) {
    const JBCBB *bb = static_cast<const JBCBB*>(nodeBB);
    uint32 tmp = bb->GetSwapSize();
    size = size > tmp ? size : tmp;
    nodeBB = nodeBB->GetNext();
  }
  return size;
}

bool JBCFunction::LabelLabelIdx(const std::string &phaseName) {
  phaseResult.RegisterPhaseNameAndStart(phaseName);
  uint32 idx = 0;
  FELinkListNode *nodeStmt = genStmtHead->GetNext();
  while (nodeStmt != nullptr && nodeStmt != genStmtTail) {
    JBCStmt *stmt = static_cast<JBCStmt*>(nodeStmt);
    if (stmt->GetKind() == JBCStmtKind::kJBCStmtPesudoLabel ||
        stmt->GetKind() == JBCStmtKind::kJBCStmtPesudoCatch) {
      JBCStmtPesudoLabel *stmtLabel = static_cast<JBCStmtPesudoLabel*>(stmt);
      stmtLabel->SetLabelIdx(idx);
      idx++;
    }
    nodeStmt = nodeStmt->GetNext();
  }
  return phaseResult.Finish();
}

bool JBCFunction::NeedConvertToInt32(const std::unique_ptr<FEIRVar> &var) {
  ASSERT(var != nullptr, "nullptr check for var");
  const std::unique_ptr<FEIRType> &type = var->GetType();
  if (!type->IsScalar()) {
    return false;
  }
  PrimType pty = type->GetPrimType();
  // Z: PTY_u1
  // B: PTY_i8
  // C: PTY_u16
  // S: PTY_i16
  if (pty == PTY_u1 || pty == PTY_u8 || pty == PTY_i8 || pty == PTY_u16 || pty == PTY_i16) {
    return true;
  } else {
    return false;
  }
}

void JBCFunction::AppendFEIRStmts(std::list<UniqueFEIRStmt> &stmts) {
  ASSERT_NOT_NULL(feirStmtTail);
  InsertFEIRStmtsBefore(*feirStmtTail, stmts);
}

void JBCFunction::InsertFEIRStmtsBefore(FEIRStmt &pos, std::list<UniqueFEIRStmt> &stmts) {
  while (stmts.size() > 0) {
    FEIRStmt *ptrFEIRStmt = RegisterFEIRStmt(std::move(stmts.front()));
    stmts.pop_front();
    pos.InsertBefore(ptrFEIRStmt);
  }
}
}  // namespace maple
