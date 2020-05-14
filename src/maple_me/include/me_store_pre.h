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
#ifndef MAPLE_ME_INCLUDE_MESTOREPRE_H
#define MAPLE_ME_INCLUDE_MESTOREPRE_H
#include "me_ssu_pre.h"
#include "me_alias_class.h"

namespace maple {
class MeStorePre : public MeSSUPre {
 public:
  MeStorePre(MeFunction &f, Dominance &dom, AliasClass &ac, MemPool &memPool, bool enabledDebug)
      : MeSSUPre(f, dom, memPool, kStorePre, enabledDebug), aliasClass(&ac), curTemp(nullptr),
        bbCurTempMap(spreAllocator.Adapter()) {}

  virtual ~MeStorePre() = default;

 private:
  inline bool IsJavaLang() const {
    return mirModule->IsJavaModule();
  }
  // step 6 methods
  void CheckCreateCurTemp();
  RegMeExpr *EnsureRHSInCurTemp(BB &bb);
  void CodeMotion();
  // step 0 methods
  void CreateRealOcc(const OStIdx &ostIdx, MeStmt &meStmt);
  void CreateUseOcc(const OStIdx &ostIdx, BB &bb) const;
  void CreateSpreUseOccsThruAliasing(const OriginalSt &muOst, BB &bb) const;
  void FindAndCreateSpreUseOccs(const MeExpr &meExpr, BB &bb) const;
  void CreateSpreUseOccsForAll(BB &bb) const;
  void BuildWorkListBB(BB *bb);
  void PerCandInit() {
    curTemp = nullptr;
    bbCurTempMap.clear();
  }

  AliasClass *aliasClass;
  // step 6 code motion
  RegMeExpr *curTemp;                               // the preg for the RHS of inserted stores
  MapleUnorderedMap<BB*, RegMeExpr*> bbCurTempMap;  // map bb to curTemp version
};

class MeDoStorePre : public MeFuncPhase {
 public:
  explicit MeDoStorePre(MePhaseID id) : MeFuncPhase(id) {}

  virtual ~MeDoStorePre() = default;
  AnalysisResult *Run(MeFunction *ir, MeFuncResultMgr *m, ModuleResultMgr *mrm) override;
  std::string PhaseName() const override {
    return "storepre";
  }
};
}  // namespace maple
#endif  // MAPLE_ME_INCLUDE_MESTOREPRE_H
