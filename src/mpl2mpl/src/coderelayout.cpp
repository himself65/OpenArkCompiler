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
#include "coderelayout.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include "profile.h"

// This phase layout the function according the profile.
// First parse the profile,find the corresponding dex file's
// function type info,the profile file give three function type
//  1.boot-only-hot  function which is only hot in phone boot phase
//  2.both-hot       function which is hot in phone run phase and phone boot phase
//  3.run-hot        function which is only hot in phone run phase
// Every functon have just one layout type,layout the function
// together according there layout type.Currently the layout is below
//
// [BootHot]
// [BothHot]
// [RunHot]
// [StartupOnly]
// [UsedOnce]
// [Executed] function excuted in some condition
// [Unused]
namespace maple {
CodeReLayout::CodeReLayout(MIRModule &mod, KlassHierarchy *kh, bool dump) : FuncOptimizeImpl(mod, kh, dump) {
  if (!Options::proFileData.empty()) {
    size_t pos = 0;
    if ((pos = Options::proFileData.find(':')) != std::string::npos) {
      Options::proFileFuncData = Options::proFileData.substr(0, pos);
      Options::proFileClassData = Options::proFileData.substr(pos + 1);
    }
    Options::proFileFuncData = Options::proFileData;
    LogInfo::MapleLogger() << "func profile " << Options::proFileFuncData << " class profile "
                           << Options::proFileClassData << "\n";
  }
  if (Options::profileStaticFields) {
    std::string staticFieldsFile = StaticFieldFilename(GetMIRModule().GetFileName());
    LogInfo::MapleLogger() << staticFieldsFile << "\n";
    std::ofstream staticFields;
    staticFields.open(staticFieldsFile, std::ofstream::trunc);
    if (!staticFields.is_open()) {
      ERR(kLncErr, " %s open failed!", staticFieldsFile.c_str());
    }
  }
}

CallNode *CodeReLayout::CreateRecordFieldStaticCall(BaseNode *node, const std::string &name) {
  MIRFunction *callee = builder->GetOrCreateFunction("MCC_RecordStaticField", static_cast<TyIdx>(PTY_void));
  BaseNode *nameAddr = builder->CreateExprAddrof(0, *GetorCreateStaticFieldSym(name));
  MapleVector<BaseNode*> args(builder->GetCurrentFuncCodeMpAllocator()->Adapter());
  args.push_back(node);
  args.push_back(nameAddr);
  return builder->CreateStmtCall(callee->GetPuidx(), args);
}

std::string CodeReLayout::StaticFieldFilename(const std::string &mplFile) const {
  size_t pos = mplFile.rfind(".mpl");
  size_t postfixSize = 4;
  CHECK_FATAL(pos != std::string::npos, "Not found .mpl postfix");
  CHECK_FATAL(pos == mplFile.length() - postfixSize, "Not compiling .mpl file?");
  std::string smryFileName = mplFile.substr(0, pos) + ".staticfields";
  return smryFileName;
}

void CodeReLayout::AddStaticFieldRecord() {
  StmtNode *stmt = currFunc->GetBody()->GetFirst();
  StmtNode *next = nullptr;
  while (stmt != nullptr) {
    next = stmt->GetNext();
    FindDreadRecur(stmt, stmt);
    if (stmt->GetOpCode() == OP_dassign) {
      MIRSymbol *mirSym = currFunc->GetLocalOrGlobalSymbol(static_cast<DassignNode*>(stmt)->GetStIdx());
      if (mirSym->IsStatic()) {
        BaseNode *node = builder->CreateExprAddrof((static_cast<DassignNode*>(stmt))->GetFieldID(),
                                                   (static_cast<DassignNode*>(stmt))->GetStIdx());
        CallNode *call = CreateRecordFieldStaticCall(node, mirSym->GetName());
        currFunc->GetBody()->InsertBefore(stmt, call);
      }
    }
    stmt = next;
  }
}

void CodeReLayout::FindDreadRecur(const StmtNode *stmt, BaseNode *node) {
  if (node == nullptr) {
    return;
  }
  BinaryOpnds *bOpnds = nullptr;
  switch (node->GetOpCode()) {
    case OP_dread:
    case OP_addrof: {
      return InsertProfileBeforeDread(stmt, node);
    }
    case OP_array:
    case OP_intrinsicop:
    case OP_intrinsicopwithtype:
    case OP_call:
    case OP_callassigned:
    case OP_icall:
    case OP_icallassigned:
    case OP_intrinsiccall:
    case OP_intrinsiccallwithtype:
    case OP_return:
    case OP_switch:
    case OP_dassign:
    case OP_iassign: {
      for (size_t i = 0; i < node->NumOpnds(); i++) {
        FindDreadRecur(stmt, node->Opnd(i));
      }
      break;
    }
    default: {
      if (node->IsUnaryNode()) {
        UnaryNode *uNode = static_cast<UnaryNode*>(node);
        FindDreadRecur(stmt, uNode->Opnd(0));
      } else if (node->IsBinaryNode()) {
        BinaryNode *bNode = static_cast<BinaryNode*>(node);
        bOpnds = static_cast<BinaryOpnds*>(bNode);
        FindDreadRecur(stmt, bOpnds->GetBOpnd(0));
        FindDreadRecur(stmt, bOpnds->GetBOpnd(1));
      }
      break;
    }
  }
}

void CodeReLayout::InsertProfileBeforeDread(const StmtNode *stmt, BaseNode *opnd) {
  if (opnd == nullptr || (opnd->GetOpCode() != OP_dread && opnd->GetOpCode() != OP_addrof)) {
    return;
  }
  DreadNode *dreadNode = static_cast<DreadNode*>(opnd);
  MIRSymbol *mirSym = currFunc->GetLocalOrGlobalSymbol(dreadNode->GetStIdx());
  if (mirSym->IsStatic()) {
    BaseNode *node = opnd;
    if (opnd->GetOpCode() == OP_dread) {
      node = builder->CreateExprAddrof(dreadNode->GetFieldID(), dreadNode->GetStIdx());
    }
    CallNode *call = CreateRecordFieldStaticCall(node, mirSym->GetName());
    currFunc->GetBody()->InsertBefore(stmt, call);
  }
}

void CodeReLayout::ProcessFunc(MIRFunction *func) {
  if (func->IsEmpty()) {
    return;
  }
  SetCurrentFunction(*func);
  if (Options::profileStaticFields) {
    AddStaticFieldRecord();
  }
  if (func->IsClinit()) {
    func->SetLayoutType(kLayoutUsedOnce);
  }
}

void CodeReLayout::Finish() {
  const auto &proFileData = GetMIRModule().GetProfile().GetFunctionProf();
  for (auto &item : proFileData) {
    std::string funcName = item.first;
    MIRFunction *sortFunction = builder->GetFunctionFromName(funcName);
    if (sortFunction != nullptr && sortFunction->GetBody()) {
      sortFunction->SetCallTimes((item.second).callTimes);
      sortFunction->SetLayoutType((item.second).type);
    }
  }
  for (auto &function : GetMIRModule().GetFunctionList()) {
    ++layoutCount[static_cast<size_t>(function->GetLayoutType())];
  }
  if (trace) {
    for (uint32 i = 0; i < static_cast<uint32>(LayoutType::kLayoutTypeCount); ++i) {
      LogInfo::MapleLogger() << "function in category\t" << GetLayoutTypeString(i)
                             << "\tcount=" << layoutCount[i] << "\n";
    }
  }
  std::stable_sort(GetMIRModule().GetFunctionList().begin(), GetMIRModule().GetFunctionList().end(),
                   [](const MIRFunction *a, const MIRFunction *b) {
                     ASSERT_NOT_NULL(a);
                     ASSERT_NOT_NULL(b);
                     return a->GetLayoutType() < b->GetLayoutType();
                   });
  uint32 last = 0;
  for (uint32 i = 0; i <= static_cast<uint32>(LayoutType::kLayoutRunHot); i++) {
    if (trace) {
      LogInfo::MapleLogger() << "last\t" << last << "\tcount\t" << layoutCount[i] << "\n";
    }
    std::stable_sort(GetMIRModule().GetFunctionList().begin() + last,
                     GetMIRModule().GetFunctionList().begin() + last + layoutCount[i],
                     [](const MIRFunction *a, const MIRFunction *b) {
                       ASSERT_NOT_NULL(a);
                       ASSERT_NOT_NULL(b);
                       return a->GetCallTimes() < b->GetCallTimes();
                     });
    last += layoutCount[i];
  }
  // Create layoutInfo
  GenLayoutSym();
}

MIRSymbol *CodeReLayout::GenStrSym(const std::string &str) {
  std::string newStr = str + '\0';
  MIRArrayType &strTabType = *GlobalTables::GetTypeTable().GetOrCreateArrayType(
      *GlobalTables::GetTypeTable().GetUInt8(), static_cast<uint32>(newStr.length()));
  std::string strTabName = namemangler::kStaticFieldNamePrefixStr + str;
  MIRSymbol *staticSym = builder->CreateGlobalDecl(strTabName, strTabType);
  MIRAggConst *strTabAggConst = GetMIRModule().GetMemPool()->New<MIRAggConst>(GetMIRModule(), strTabType);
  staticSym->SetStorageClass(kScFstatic);
  for (const char &c : newStr) {
    MIRConst *newConst = GlobalTables::GetIntConstTable().GetOrCreateIntConst(
        c, *GlobalTables::GetTypeTable().GetUInt8());
    strTabAggConst->PushBack(newConst);
  }
  staticSym->SetKonst(strTabAggConst);
  return staticSym;
}

MIRSymbol *CodeReLayout::GetorCreateStaticFieldSym(const std::string &fieldName) {
  auto it = str2SymMap.find(fieldName);
  if (it != str2SymMap.end()) {
    return it->second;
  } else {
    MIRSymbol *sym = GenStrSym(fieldName);
    (void)str2SymMap.insert(std::make_pair(fieldName, sym));
    return sym;
  }
}

void CodeReLayout::GenLayoutSym() {
  MIRArrayType &arrayType =
      *GlobalTables::GetTypeTable().GetOrCreateArrayType(*GlobalTables::GetTypeTable().GetVoidPtr(), 1);
  MIRAggConst *funcLayoutConst = GetMIRModule().GetMemPool()->New<MIRAggConst>(GetMIRModule(), arrayType);
  uint32 funcIdx = 0;
  MIRConst *fieldConst = nullptr;
  MIRFunction *method = nullptr;
  for (uint32 i = 0; i < static_cast<uint32>(LayoutType::kLayoutTypeCount); ++i) {
    if (funcIdx < GetMIRModule().GetFunctionList().size()) {
      method = GetMIRModule().GetFunction(funcIdx);
    } else {
      std::cerr << "no method for codelayout type " << GetLayoutTypeString(i) << "\n";
      return;
    }
    while (method->IsAbstract() || method->GetBody() == nullptr) {
      // find the function not Abstract
      if (trace) {
        LogInfo::MapleLogger() << "encounter valid method " << funcIdx << "\n";
      }
      funcIdx++;
      if (funcIdx < GetMIRModule().GetFunctionList().size()) {
        method = GetMIRModule().GetFunction(funcIdx);
      } else {
        std::cerr << "no method for codelayout" << GetLayoutTypeString(i) << "\n";
        return;
      }
    }
    if (trace) {
      LogInfo::MapleLogger() << "Start of category " << i << " in funcIdx " << funcIdx << " "
                             << method->GetName() << "\n";
    }
    AddroffuncNode *addroffuncExpr = builder->CreateExprAddroffunc(method->GetPuidx(), GetMIRModule().GetMemPool());
    fieldConst =
        GetMIRModule().GetMemPool()->New<MIRAddroffuncConst>(addroffuncExpr->GetPUIdx(),
                                                             *GlobalTables::GetTypeTable().GetVoidPtr());
    funcLayoutConst->PushBack(fieldConst);
    funcIdx += layoutCount[i];
  }
  std::string funcLayoutSymName = namemangler::kFunctionLayoutStr + GetMIRModule().GetFileNameAsPostfix();
  MIRSymbol *funcLayoutSym = builder->CreateGlobalDecl(funcLayoutSymName, arrayType);
  funcLayoutSym->SetKonst(funcLayoutConst);
  funcLayoutSym->SetStorageClass(kScFstatic);
}
}  // namespace maple
