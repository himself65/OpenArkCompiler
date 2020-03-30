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
#include "analyzector.h"
#include "utils.h"

// AnalyzeCtor analyzes which fields are assigned inside of each constructor
namespace maple {
void AnalyzeCtor::ProcessFunc(MIRFunction *func) {
  if (!func->IsConstructor() || func->IsEmpty() || func->GetParamSize() == 0) {
    return;
  }
  SetCurrentFunction(*func);
  hasSideEffect = false;
  fieldSet.clear();
  if (func->GetBody() != nullptr) {
    ProcessBlock(*func->GetBody());
  }
  PUIdx puIdx = func->GetPuidx();
  const MapleMap<PUIdx, MapleSet<FieldID>*> &puIdxFieldMap = GetMIRModule().GetPuIdxFieldInitializedMap();
  CHECK_FATAL(puIdxFieldMap.find(puIdx) == puIdxFieldMap.end(),
              "%s has been processed before", func->GetName().c_str());
  // if the function has calls with sideeffect, conservatively
  // we assume all fields are modified in ctor
  if (hasSideEffect) {
    MapleSet<FieldID> *fieldSubSet =
        GetMIRModule().GetMemPool()->New<MapleSet<FieldID>>(std::less<FieldID>(),
                                                            GetMIRModule().GetMPAllocator().Adapter());
    fieldSubSet->insert(0);  // write to all
    GetMIRModule().SetPuIdxFieldSet(puIdx, fieldSubSet);
  } else if (!fieldSet.empty()) {
    MapleSet<FieldID> *fieldSubSet =
        GetMIRModule().GetMemPool()->New<MapleSet<FieldID>>(std::less<FieldID>(),
                                                            GetMIRModule().GetMPAllocator().Adapter());
    std::copy(fieldSet.begin(), fieldSet.end(), std::inserter(*fieldSubSet, fieldSubSet->begin()));
    GetMIRModule().SetPuIdxFieldSet(puIdx, fieldSubSet);
  } else {
    // no fields are assigned in constructor
    GetMIRModule().SetPuIdxFieldSet(puIdx, nullptr);
  }
}

// collect field ids which are assigned inside the stmt and mark sideeffect
// flag for non-ctor calls
void AnalyzeCtor::ProcessStmt(StmtNode &stmt) {
  switch (stmt.GetOpCode()) {
    case OP_iassign: {
      auto &iassign = static_cast<IassignNode&>(stmt);
      MIRType *baseType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(iassign.GetTyIdx());
      MIRType *pointedType = utils::ToRef(safe_cast<MIRPtrType>(baseType)).GetPointedType();
      auto structType = safe_cast<MIRStructType>(pointedType);
      if (structType != nullptr) {
        MIRType *fieldType = structType->GetFieldType(iassign.GetFieldID());
        if (fieldType->GetPrimType() != PTY_ref) {
          break;
        }
      }
      fieldSet.insert(iassign.GetFieldID());
      break;
    }
    case OP_callassigned:
    case OP_call:
    case OP_icall:
    case OP_intrinsiccall:
    case OP_xintrinsiccall:
    case OP_virtualcall:
    case OP_superclasscall:
    case OP_interfacecall: {
      hasSideEffect = true;
      break;
    }
    default:
      break;
  }
}

void AnalyzeCtor::Finish() {
  if (!trace) {
    return;
  }
  for (auto &pit : GetMIRModule().GetPuIdxFieldInitializedMap()) {
    GlobalTables::GetFunctionTable().GetFunctionFromPuidx(pit.first)->Dump(true);
    LogInfo::MapleLogger() << "field:";
    MapleSet<FieldID> *fieldIDSet = pit.second;
    if (fieldIDSet == nullptr) {
      LogInfo::MapleLogger() << "write nothing\n";
      continue;
    }
    for (FieldID fid : *fieldIDSet) {
      LogInfo::MapleLogger() << fid << " ";
    }
    LogInfo::MapleLogger() << '\n';
  }
}
}  // namespace maple
