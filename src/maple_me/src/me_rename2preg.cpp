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
#include "me_rename2preg.h"
#include <unordered_map>
#include <utils.h>
#include "mir_builder.h"
#include "me_irmap.h"
#include "alias_class.h"

// This phase mainly renames the variables to pseudo register.
// Only non-ref-type variables (including parameters) with no alias are
// workd on here.  Remaining variables are left to LPRE phase.  This is
// because for ref-type variables, their stores have to be left intact.

namespace {
using namespace maple;

// Part1: Generalize GetAnalysisResult from MeFuncResultMgr
template <MePhaseID id> struct ExtractPhaseClass {};

template <> struct ExtractPhaseClass<MeFuncPhase_IRMAP> {
  using type = MeIRMap;
};

template <> struct ExtractPhaseClass<MeFuncPhase_ALIASCLASS> {
  using type = AliasClass;
};

template <MePhaseID id, typename RetT = std::add_pointer_t<typename ExtractPhaseClass<id>::type>>
inline RetT GetAnalysisResult(MeFunction &func, MeFuncResultMgr &funcRst) {
  return static_cast<RetT>(funcRst.GetAnalysisResult(id, &func));
}

// Part2: Phase implementation.
class OStCache final {
 public:
  explicit OStCache(SSATab &ssaTab) : ssaTab(ssaTab) {}
  ~OStCache() = default;

  const OriginalSt *Find(OStIdx ostIdx) const {
    auto it = cache.find(ostIdx);
    return it == cache.end() ? nullptr : it->second;
  }

  OriginalSt &CreatePregOst(const RegMeExpr &regExpr, const OriginalSt &ost) {
    OriginalStTable &ostTbl = ssaTab.GetOriginalStTable();
    OriginalSt *regOst = ostTbl.CreatePregOriginalSt(regExpr.GetRegIdx(), regExpr.GetPuIdx());
    utils::ToRef(regOst).SetIsFormal(ost.IsFormal());
    cache[ost.GetIndex()] = regOst;
    return *regOst;
  }

 private:
  SSATab &ssaTab;
  std::map<OStIdx, OriginalSt *> cache; // map var to reg in original symbol
};

class PregCache final {
 public:
  explicit PregCache(MeIRMap &irMap) : irMap(irMap) {}
  ~PregCache() = default;

  RegMeExpr &CloneRegExprIfNotExist(const VarMeExpr &varExpr,
                                    const std::function<RegMeExpr *(MeIRMap &)> &creator) {
    auto it = cache.find(varExpr.GetExprID());
    if (it != cache.end()) {
      return utils::ToRef(it->second);
    }

    RegMeExpr *regExpr = creator(irMap);
    cache.insert(std::make_pair(varExpr.GetExprID(), regExpr));
    return utils::ToRef(regExpr);
  }

  RegMeExpr &CreatePregExpr(const VarMeExpr &varExpr, TyIdx symbolIdx) {
    MIRType &ty = utils::ToRef(GlobalTables::GetTypeTable().GetTypeFromTyIdx(symbolIdx));
    RegMeExpr *regExpr = ty.GetPrimType() == PTY_ref ?
      irMap.CreateRegRefMeExpr(ty) : irMap.CreateRegMeExpr(varExpr.GetPrimType());
    cache.insert(std::make_pair(varExpr.GetExprID(), regExpr));
    return utils::ToRef(regExpr);
  }

 private:
  MeIRMap &irMap;
  std::unordered_map<int32, RegMeExpr *> cache; // maps the VarMeExpr's exprID to RegMeExpr
};

class CacheProxy final {
 public:
  CacheProxy() = default;
  ~CacheProxy() = default;

  void Init(SSATab &ssaTab, MeIRMap &irMap) {
    ost = std::make_unique<OStCache>(ssaTab);
    preg = std::make_unique<PregCache>(irMap);
  }

  OStCache &OSt() {
    return utils::ToRef(ost);
  }

  PregCache &Preg() {
    return utils::ToRef(preg);
  }

 private:
  std::unique_ptr<OStCache> ost;
  std::unique_ptr<PregCache> preg;
};

class FormalRenaming final {
 public:
  explicit FormalRenaming(MIRFunction &irFunc)
      : irFunc(irFunc),
        paramUsed(irFunc.GetFormalCount(), false),
        renamedReg(irFunc.GetFormalCount(), nullptr) {}

  ~FormalRenaming() = default;

  void MarkUsed(const OriginalSt &ost) {
    if (ost.IsFormal() && ost.IsSymbolOst()) {
      const MIRSymbol *sym = ost.GetMIRSymbol();
      uint32 idx = irFunc.GetFormalIndex(sym);
      paramUsed[idx] = true;
    }
  }

  void MarkRenamed(const OriginalSt &ost, const MIRSymbol &irSymbol, RegMeExpr &regExpr) {
    if (ost.IsFormal()) {
      uint32 idx = irFunc.GetFormalIndex(&irSymbol);
      CHECK_FATAL(paramUsed[idx], "param used not set correctly.");
      if (!renamedReg[idx]) {
        renamedReg[idx] = &regExpr;
      }
    }
  }

  void Rename(MIRBuilder &irBuilder) {
    for (size_t i = 0; i < irFunc.GetFormalCount(); ++i) {
      if (!paramUsed[i]) {
        // in this case, the paramter is not used by any statement, promote it
        MIRType &irTy = utils::ToRef(irFunc.GetNthParamType(i));
        MIRPregTable &irPregTbl = utils::ToRef(irFunc.GetPregTab());
        PregIdx16 regIdx = (irTy.GetPrimType() == PTY_ref) ?
                           irPregTbl.CreateRefPreg(irTy) : irPregTbl.CreatePreg(irTy.GetPrimType());
        irFunc.SetFormal(i, irBuilder.CreatePregFormalSymbol(irTy.GetTypeIndex(), regIdx, irFunc));
      } else {
        RegMeExpr *regExpr = renamedReg[i];
        if (regExpr != nullptr) {
          PregIdx16 regIdx = regExpr->GetRegIdx();
          MIRSymbol &irSym = utils::ToRef(irFunc.GetFormal(i));
          MIRSymbol *newIrSym = irBuilder.CreatePregFormalSymbol(irSym.GetTyIdx(), regIdx, irFunc);
          irFunc.SetFormal(i, newIrSym);
        }
      }
    }
  }

 private:
  MIRFunction &irFunc;

  std::vector<bool> paramUsed; // if parameter is not used, it's false, otherwise true
  // if the parameter got promoted, the nth of func->mirfunc->_formal is the nth of reg_formal_vec, otherwise nullptr;
  std::vector<RegMeExpr *> renamedReg;
};

class SSARename2Preg {
 public:
  SSARename2Preg(MIRFunction &irFunc, SSATab *ssaTab, bool enabledDebug)
      : ssaTab(ssaTab),
        enabledDebug(enabledDebug),
        formal(irFunc) {}

  virtual ~SSARename2Preg() = default;

  static const std::string &PhaseName() {
    static const std::string name = "rename2preg";
    return name;
  }

  void Run(MeFunction &func, MeFuncResultMgr *pFuncRst) {
    bool emptyFunc = func.empty();
    if (!emptyFunc) {
      MeFuncResultMgr &funcRst = utils::ToRef(pFuncRst);
      MeIRMap &irMap = utils::ToRef(GetAnalysisResult<MeFuncPhase_IRMAP>(func, funcRst));
      const AliasClass &aliasClass = utils::ToRef(GetAnalysisResult<MeFuncPhase_ALIASCLASS>(func, funcRst));

      cacheProxy.Init(utils::ToRef(ssaTab), irMap);
      for (auto it = func.valid_begin(), eIt = func.valid_end(); it != eIt; ++it) {
        BB &bb = utils::ToRef(*it);

        // rename the phi'ss
        if (enabledDebug) {
          LogInfo::MapleLogger() << " working on phi part of BB" << bb.GetBBId() << '\n';
        }
        for (auto &varPhi : bb.GetMevarPhiList()) {
          Rename2PregPhi(aliasClass, utils::ToRef(varPhi.second), irMap);
        }

        if (enabledDebug) {
          LogInfo::MapleLogger() << " working on stmt part of BB" << bb.GetBBId() << '\n';
        }
        for (MeStmt &stmt : bb.GetMeStmts()) {
          Rename2PregStmt(aliasClass, irMap, stmt);
        }
      }
    }

    MIRBuilder &irBuilder = utils::ToRef(func.GetMIRModule().GetMIRBuilder());
    formal.Rename(irBuilder);

    if (!emptyFunc && enabledDebug) {
      func.Dump(false);
    }
  }

 private:
  void Rename2PregStmt(const AliasClass &aliasClass, MeIRMap &irMap, MeStmt &stmt) {
    Opcode code = stmt.GetOp();
    if (IsDAssign(code)) {
      Rename2PregExpr(aliasClass, irMap, stmt, utils::ToRef(stmt.GetRHS()));
      Rename2PregLeafLHS(aliasClass, utils::ToRef(stmt.GetVarLHS()), stmt, irMap);
    } else if (IsCallAssigned(code)) {
      for (size_t i = 0; i < stmt.NumMeStmtOpnds(); ++i) {
        Rename2PregExpr(aliasClass, irMap, stmt, utils::ToRef(stmt.GetOpnd(i)));
      }
      Rename2PregCallReturn(aliasClass, utils::ToRef(stmt.GetMustDefList()));
    } else if (auto *iAssignStmt = safe_cast<IassignMeStmt>(stmt)) {
      Rename2PregExpr(aliasClass, irMap, stmt, utils::ToRef(iAssignStmt->GetRHS()));
      Rename2PregExpr(aliasClass, irMap, stmt, utils::ToRef(utils::ToRef(iAssignStmt->GetLHSVal()).GetBase()));
    } else {
      for (size_t i = 0; i < stmt.NumMeStmtOpnds(); ++i) {
        Rename2PregExpr(aliasClass, irMap, stmt, utils::ToRef(stmt.GetOpnd(i)));
      }
    }
  }

  void Rename2PregCallReturn(const AliasClass &aliasClass, MapleVector<MustDefMeNode> &mustDefNodes) {
    if (mustDefNodes.empty()) {
      return;
    }

    CHECK_FATAL(mustDefNodes.size() == 1, "NYI");
    MustDefMeNode &mustDefNode = mustDefNodes.front();
    auto *lhs = safe_cast<VarMeExpr>(mustDefNode.GetLHS());
    if (lhs == nullptr) {
      return;
    }

    OriginalSt &ost = utils::ToRef(utils::ToRef(ssaTab).GetOriginalStFromID(lhs->GetOStIdx()));
    RegMeExpr *regExpr = RenameVar(aliasClass, *lhs);
    if (regExpr != nullptr) {
      mustDefNode.UpdateLHS(*regExpr);
    } else {
      CHECK_FATAL(ost.IsRealSymbol(), "NYI");
    }
  }

  // only handle the leaf of load, because all other expressions has been done by previous SSAPre
  void Rename2PregExpr(const AliasClass &aliasClass, MeIRMap &irMap, MeStmt &stmt, MeExpr &expr) {
    if (auto *opExpr = safe_cast<OpMeExpr>(expr)) {
      for (size_t i = 0; i < kOperandNumTernary; ++i) {
        MeExpr *opnd = opExpr->GetOpnd(i);
        if (opnd != nullptr) {
          Rename2PregExpr(aliasClass, irMap, stmt, *opnd);
        }
      }
    } else if (auto *naryExpr = safe_cast<NaryMeExpr>(expr)) {
      MapleVector<MeExpr *> &opnds = naryExpr->GetOpnds();
      for (auto *opnd : opnds) {
        Rename2PregExpr(aliasClass, irMap, stmt, utils::ToRef(opnd));
      }
    } else if (auto *ivarExpr = safe_cast<IvarMeExpr>(expr)) {
      Rename2PregExpr(aliasClass, irMap, stmt, utils::ToRef(ivarExpr->GetBase()));
    } else if (auto *varExpr = safe_cast<VarMeExpr>(expr)) {
      Rename2PregLeafRHS(aliasClass, irMap, stmt, *varExpr);
    }
  }

  void Rename2PregLeafRHS(const AliasClass &aliasClass, MeIRMap &irMap, MeStmt &stmt, VarMeExpr &varExpr) {
    RegMeExpr *regExpr = RenameVar(aliasClass, varExpr);
    if (regExpr != nullptr) {
      (void)irMap.ReplaceMeExprStmt(stmt, varExpr, *regExpr);
    }
  }

  void Rename2PregLeafLHS(const AliasClass &aliasClass,
                          const VarMeExpr &varExpr,
                          MeStmt &stmt,
                          MeIRMap &irMap) {
    RegMeExpr *regExpr = RenameVar(aliasClass, varExpr);
    if (regExpr != nullptr) {
      MeExpr *oldRhs = nullptr;
      if (auto *dAStmt = safe_cast<DassignMeStmt>(stmt)) {
        oldRhs = dAStmt->GetRHS();
      } else if (auto *mayDAStmt = safe_cast<MaydassignMeStmt>(stmt)) {
        oldRhs = mayDAStmt->GetRHS();
      } else {
        CHECK_FATAL(false, "NYI");
      }

      auto &regAssignStmt = utils::ToRef(irMap.New<RegassignMeStmt>(regExpr, oldRhs));
      regAssignStmt.CopyBase(stmt);
      utils::ToRef(stmt.GetBB()).InsertMeStmtBefore(&stmt, &regAssignStmt);
      utils::ToRef(stmt.GetBB()).RemoveMeStmt(&stmt);
    }
  }

  void Rename2PregPhi(const AliasClass &aliasClass, MeVarPhiNode &varPhiNode, MeIRMap &irMap) {
    VarMeExpr &varExpr = utils::ToRef(varPhiNode.GetLHS());
    RegMeExpr *pRegExpr = RenameVar(aliasClass, varExpr);
    if (pRegExpr == nullptr) {
      return;
    }
    RegMeExpr &regExpr = *pRegExpr;
    MeRegPhiNode &regPhiNode = utils::ToRef(irMap.CreateMeRegPhi(regExpr));
    regPhiNode.SetDefBB(varPhiNode.GetDefBB());
    UpdateRegPhi(regExpr, varPhiNode.GetOpnds(), regPhiNode);
  }

  // update regphinode operands
  void UpdateRegPhi(const RegMeExpr &curRegExpr, const MapleVector<VarMeExpr*> &phiNodeOpnds,
                    MeRegPhiNode &regPhiNode) {
    PregCache &pregCache = cacheProxy.Preg();
    for (VarMeExpr *phiOpnd : phiNodeOpnds) {
      const VarMeExpr &varExpr = utils::ToRef(phiOpnd);
      RegMeExpr &regExpr = pregCache.CloneRegExprIfNotExist(varExpr, [&curRegExpr](MeIRMap &irMap) {
        return irMap.CreateRegMeExprVersion(curRegExpr);
      });
      regPhiNode.GetOpnds().push_back(&regExpr);
      regExpr.GetPhiUseSet().insert(&regPhiNode);
    }
  }

  RegMeExpr *RenameVar(const AliasClass &aliasClass, const VarMeExpr &varExpr) {
    const OriginalSt &ost = utils::ToRef(utils::ToRef(ssaTab).GetOriginalStFromID(varExpr.GetOStIdx()));
    formal.MarkUsed(ost);

    if (varExpr.GetFieldID() != 0) {
      return nullptr;
    }

    if (ost.GetIndirectLev() != 0) {
      return nullptr;
    }

    CHECK_FATAL(ost.IsRealSymbol(), "NYI");
    const MIRSymbol &irSymbol = utils::ToRef(ost.GetMIRSymbol());
    if (irSymbol.GetAttr(ATTR_localrefvar)) {
      return nullptr;
    }
    if (ost.IsFormal() && varExpr.GetPrimType() == PTY_ref) {
      return nullptr;
    }

    const OriginalSt *cacheOSt = cacheProxy.OSt().Find(ost.GetIndex());
    if (cacheOSt != nullptr) {
      // replaced previously
      return &cacheProxy.Preg().CloneRegExprIfNotExist(varExpr, [cacheOSt](MeIRMap &irMap) {
        return irMap.CreateRegMeExprVersion(*cacheOSt);
      });
    }

    if (!irSymbol.IsLocal()) {
      return nullptr;
    }
    const AliasElem *aliasElem = GetAliasElem(aliasClass, ost);
    if (aliasElem == nullptr || aliasElem->GetClassSet() != nullptr) {
      return nullptr;
    }

    RegMeExpr &newRegExpr = cacheProxy.Preg().CreatePregExpr(varExpr, irSymbol.GetTyIdx());
    OriginalSt &pregOst = cacheProxy.OSt().CreatePregOst(newRegExpr, ost);

    formal.MarkRenamed(ost, irSymbol, newRegExpr);

    if (enabledDebug) {
      ost.Dump();
      LogInfo::MapleLogger() << "(ost idx " << ost.GetIndex() << ") renamed to ";
      pregOst.Dump();
      LogInfo::MapleLogger() << '\n';
    }
    return &newRegExpr;
  }

  const AliasElem *GetAliasElem(const AliasClass &aliasClass, const OriginalSt &ost) const {
    if (ost.GetIndex() >= aliasClass.GetAliasElemCount()) {
      return nullptr;
    }
    return aliasClass.FindAliasElem(ost);
  }

 private:
  SSATab *ssaTab;
  bool enabledDebug;

  CacheProxy cacheProxy;
  FormalRenaming formal;
};

}

namespace maple {

AnalysisResult *MeDoSSARename2Preg::Run(MeFunction *func, MeFuncResultMgr *funcRst, ModuleResultMgr *) {
  MeFunction &rFunc = utils::ToRef(func);
  SSARename2Preg phase(utils::ToRef(rFunc.GetMirFunc()), rFunc.GetMeSSATab(), DEBUGFUNC(func));
  phase.Run(rFunc, funcRst);

  return nullptr;
}

std::string MeDoSSARename2Preg::PhaseName() const {
  return SSARename2Preg::PhaseName();
}

}  // namespace maple

