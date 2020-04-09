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
#include "me_profile_gen.h"
#include <iostream>
#include "me_cfg.h"
#include "me_option.h"
#include "me_function.h"
#include "me_irmap.h"
#include "mir_builder.h"

/*
 * This phase do CFG edge profile using a minimum spanning tree based
 * on the following paper:
 * [1] Donald E. Knuth, Francis R. Stevenson. Optimal measurement of points
 * FOR program frequency counts. BIT Numerical Mathematics 1973, Volume 13,
 * Issue 3, pp 313-322
 * The count of edge on spanning tree can be derived from
 * those edges not on the spanning tree. Knuth proves this method instruments
 * the minimum number of edges.
 */

namespace maple {
uint64 MeProfGen::counterIdx = 0;
uint64 MeProfGen::totalBB = 0;
uint64 MeProfGen::instrumentBB = 0;
bool MeProfGen::firstRun = true;
MIRSymbol *MeProfGen::bbCounterTabSym = nullptr;

void MeProfGen::Init() {
  if (!firstRun) {
    return;
  }
  MIRArrayType &muidIdxArrayType =
      *GlobalTables::GetTypeTable().GetOrCreateArrayType(*GlobalTables::GetTypeTable().GetUInt32(), 0);
  std::string bbProfileName = NameMangler::kBBProfileTabPrefixStr + func->GetMIRModule().GetFileNameAsPostfix();
  bbCounterTabSym = func->GetMIRModule().GetMIRBuilder()->CreateGlobalDecl(bbProfileName.c_str(), muidIdxArrayType);
  MIRAggConst *bbProfileTab = func->GetMIRModule().GetMemPool()->New<MIRAggConst>(
      func->GetMIRModule(), *GlobalTables::GetTypeTable().GetUInt32());
  bbCounterTabSym->SetKonst(bbProfileTab);
  bbCounterTabSym->SetStorageClass(kScFstatic);
  firstRun = false;
}

void MeProfGen::InstrumentBB(BB &bb) {
  /* append intrinsic call at the begin of bb */
  CHECK_FATAL(counterIdx != (UINT32_MAX / GetPrimTypeSize(PTY_u32)), "profile counter overflow");
  MeExpr *arg0 = hMap->CreateIntConstMeExpr(counterIdx, PTY_u32);
  std::vector<MeExpr*> opnds = { arg0 };
  IntrinsiccallMeStmt *counterIncCall = hMap->CreateIntrinsicCallMeStmt(INTRN_MPL_PROF_COUNTER_INC, opnds);
  bb.AddMeStmtFirst(counterIncCall);
  bb.SetAttributes(kBBAttrIsInstrument);
  counterIdx++;
  if (dump) {
    LogInfo::MapleLogger() << "instrument on BB" << bb.GetBBId() << "\n";
  }
}

void MeProfGen::SaveProfile() {
  if (!Options::testCase) {
    return;
  }
  if (func->GetName().find("main") != std::string::npos) {
    std::vector<MeExpr*> opnds;
    IntrinsiccallMeStmt *saveProfCall = hMap->CreateIntrinsicCallMeStmt(INTRN_MCCSaveProf, opnds);
    for (BB *exitBB : func->GetCommonExitBB()->GetPred()) {
      exitBB->AddMeStmtFirst(saveProfCall);
    }
  }
}

void MeProfGen::InstrumentFunc() {
  FindInstrumentEdges();
  std::vector<BB*> instrumentBBs;
  GetInstrumentBBs(instrumentBBs);
  // record the current counter start, used in the function profile description
  uint32 counterStart = counterIdx;
  MIRAggConst *bbProfileTab = safe_cast<MIRAggConst>(bbCounterTabSym->GetKonst());
  for (auto *bb : instrumentBBs) {
    MIRIntConst *indexConst =
        func->GetMIRModule().GetMemPool()->New<MIRIntConst>(0, *GlobalTables::GetTypeTable().GetUInt32());
    bbProfileTab->PushBack(indexConst);
    InstrumentBB(*bb);
  }

  uint32 counterEnd = counterIdx - 1;
  uint64 hash = ComputeFuncHash();
  func->AddProfileDesc(hash, counterStart, counterEnd);
  instrumentBB += instrumentBBs.size();
  totalBB += GetAllBBs();
  SaveProfile();
  if (dump) {
    LogInfo::MapleLogger() << "******************after profile gen  dump function******************\n";
    func->Dump(true);
  }
  if (Options::testCase || dump) {
    LogInfo::MapleLogger() << func->GetName() << " profile description info: "
                           << "func hash " << std::hex << hash << std::dec << " counter range ["
                           << counterStart << "," << counterEnd << "]\n";
  }
}

AnalysisResult *MeDoProfGen::Run(MeFunction *func, MeFuncResultMgr *m, ModuleResultMgr*) {
  MemPool *tempMp = NewMemPool();
  auto *hMap = static_cast<MeIRMap*>(m->GetAnalysisResult(MeFuncPhase_IRMAP, func));
  CHECK_FATAL(hMap != nullptr, "hssamap is nullptr");
  MeProfGen profGen(*func, *tempMp, *hMap, DEBUGFUNC(func));
  profGen.InstrumentFunc();

  if (DEBUGFUNC(func)) {
    LogInfo::MapleLogger() << "dump edge info in profile gen phase " << func->GetMirFunc()->GetName() << std::endl;
    func->GetTheCfg()->DumpToFile("afterProfileGen", false);
    profGen.DumpEdgeInfo();
  }

  return nullptr;
}
}  // namespace maple
