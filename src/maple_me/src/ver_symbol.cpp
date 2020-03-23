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
#include "ver_symbol.h"
#include "bb.h"
#include "me_ssa.h"
#include "ssa_mir_nodes.h"

namespace maple {
VersionSt VersionStTable::dummyVST(0, 0, nullptr);
void VersionSt::DumpDefStmt(const MIRModule*) const {
  if (version <= 0) {
    return;
  }
  switch (defType) {
    case kAssign:
      defStmt.assign->Dump(0);
      return;
    case kPhi:
      defStmt.phi->Dump();
      return;
    case kMayDef:
      defStmt.mayDef->Dump();
      return;
    case kMustDef:
      defStmt.mustDef->Dump();
      return;
    default:
      ASSERT(false, "not yet implement");
  }
}

VersionSt *VersionStTable::CreateVersionSt(OriginalSt *ost, size_t version) {
  ASSERT(ost != nullptr, "nullptr check");
  ASSERT(ost->GetVersionsIndex().size() == version, "ssa version need to be created incrementally!");
  auto *vst = vstAlloc.GetMemPool()->New<VersionSt>(versionStVector.size(), version, ost);
  versionStVector.push_back(vst);
  ost->PushbackVersionIndex(vst->GetIndex());
  if (version == kInitVersion) {
    ost->SetZeroVersionIndex(vst->GetIndex());
  }
  vst->SetOrigSt(ost);
  return vst;
}

VersionSt *VersionStTable::FindOrCreateVersionSt(OriginalSt *ost, size_t version) {
  // this version already exists...
  ASSERT(ost != nullptr, "nullptr check");
  if (ost->GetVersionsIndex().size() > version) {
    size_t versionIndex = ost->GetVersionIndex(version);
    ASSERT(versionStVector.size() > versionIndex, "versionStVector out of range");
    return versionStVector.at(versionIndex);
  }
  return CreateVersionSt(ost, version);
}

void VersionStTable::Dump(const MIRModule *mod) const {
  ASSERT(mod != nullptr, "nullptr check");
  LogInfo::MapleLogger() << "=======version st table entries=======\n";
  for (size_t i = 1; i < versionStVector.size(); ++i) {
    const VersionSt *vst = versionStVector[i];
    vst->Dump();
    if (vst->GetVersion() > 0) {
      LogInfo::MapleLogger() << " defined BB" << vst->GetDefBB()->GetBBId() << ": ";
      vst->DumpDefStmt(mod);
    } else {
      LogInfo::MapleLogger() << '\n';
    }
  }
  mod->GetOut() << "=======end version st table===========\n";
}
}  // namespace maple
