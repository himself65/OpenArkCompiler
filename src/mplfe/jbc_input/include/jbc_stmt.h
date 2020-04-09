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
#ifndef MPLFE_INCLUDE_JBC_STMT_H
#define MPLFE_INCLUDE_JBC_STMT_H
#include <memory>
#include <list>
#include "general_stmt.h"
#include "jbc_opcode.h"
#include "jbc_stack2fe_helper.h"

namespace maple {
enum JBCStmtKind : uint8 {
  kJBCStmtDefault = 0,
  kJBCStmtFuncBeing,
  kJBCStmtFuncEnd,
  kJBCStmtInst,
  kJBCStmtInstBranch,
  kJBCStmtPesudoComment,
  kJBCStmtPesudoLOC,
  kJBCStmtPesudoLabel,
  kJBCStmtPesudoTry,
  kJBCStmtPesudoEndTry,
  kJBCStmtPesudoCatch
};

class JBCStmtKindHelper {
 public:
  static std::string JBCStmtKindName(JBCStmtKind kind);

 private:
  JBCStmtKindHelper() = default;
  ~JBCStmtKindHelper() = default;
};  // class JBCStmtKindHelper

class JBCStmt : public GeneralStmt {
 public:
  explicit JBCStmt(JBCStmtKind argKind)
      : kind(argKind) {}

  JBCStmt(GeneralStmtKind argGenKind, JBCStmtKind argKind)
      : GeneralStmt(argGenKind),
        kind(argKind) {}

  virtual ~JBCStmt() = default;
  std::list<UniqueFEIRStmt> EmitToFEIR(JBCStack2FEHelper &stack2feHelper,
                                       const jbc::JBCConstPool &constPool,
                                       bool &success) const {
    return EmitToFEIRImpl(stack2feHelper, constPool, success);
  }

  JBCStmtKind GetKind() const {
    return kind;
  }

  void SetKind(JBCStmtKind argKind) {
    kind = argKind;
  }

 protected:
  virtual std::list<UniqueFEIRStmt> EmitToFEIRImpl(JBCStack2FEHelper &stack2feHelper,
                                                   const jbc::JBCConstPool &constPool,
                                                   bool &success) const = 0;

  JBCStmtKind kind;
};

class JBCStmtInst : public JBCStmt {
 public:
  explicit JBCStmtInst(const jbc::JBCOp &argOp);
  ~JBCStmtInst() = default;
  const jbc::JBCOp &GetOp() const {
    return op;
  }

 protected:
  bool IsStmtInstImpl() const override;
  void DumpImpl(const std::string &prefix) const override;
  std::string DumpDotStringImpl() const override;
  std::list<UniqueFEIRStmt> EmitToFEIRImpl(JBCStack2FEHelper &stack2feHelper,
                                           const jbc::JBCConstPool &constPool,
                                           bool &success) const override;

 private:
  const jbc::JBCOp &op;
  using FuncPtrEmitToFEIR = std::list<UniqueFEIRStmt> (JBCStmtInst::*)(JBCStack2FEHelper &stack2feHelper,
                                                                       const jbc::JBCConstPool &constPool,
                                                                       bool &success) const;
  static std::map<jbc::JBCOpcodeKind, FuncPtrEmitToFEIR> funcPtrMapForEmitToFEIR;
  static std::map<jbc::JBCOpcodeKind, FuncPtrEmitToFEIR> InitFuncPtrMapForEmitToFEIR();
  static std::map<jbc::JBCOpcode, Opcode> opcodeMapForMathBinop;
  static std::map<jbc::JBCOpcode, Opcode> InitOpcodeMapForMathBinop();
  static std::map<jbc::JBCOpcode, Opcode> opcodeMapForMathUnop;
  static std::map<jbc::JBCOpcode, Opcode> InitOpcodeMapForMathUnop();
  static std::map<jbc::JBCOpcode, Opcode> opcodeMapForMonitor;
  static std::map<jbc::JBCOpcode, Opcode> InitOpcodeMapForMonitor();
  std::list<UniqueFEIRStmt> EmitToFEIRForOpConst(JBCStack2FEHelper &stack2feHelper,
                                                 const jbc::JBCConstPool &constPool,
                                                 bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpConstCommon(JBCStack2FEHelper &stack2feHelper,
                                                       const jbc::JBCConstPool &constPool,
                                                       bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpAConstNull(JBCStack2FEHelper &stack2feHelper,
                                                      const jbc::JBCConstPool &constPool,
                                                      bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpIConst(JBCStack2FEHelper &stack2feHelper,
                                                  const jbc::JBCConstPool &constPool,
                                                  bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpLConst(JBCStack2FEHelper &stack2feHelper,
                                                  const jbc::JBCConstPool &constPool,
                                                  bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpFConst(JBCStack2FEHelper &stack2feHelper,
                                                  const jbc::JBCConstPool &constPool,
                                                  bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpDConst(JBCStack2FEHelper &stack2feHelper,
                                                  const jbc::JBCConstPool &constPool,
                                                  bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpBiPush(JBCStack2FEHelper &stack2feHelper,
                                                  const jbc::JBCConstPool &constPool,
                                                  bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpSiPush(JBCStack2FEHelper &stack2feHelper,
                                                  const jbc::JBCConstPool &constPool,
                                                  bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpLdc(JBCStack2FEHelper &stack2feHelper,
                                               const jbc::JBCConstPool &constPool,
                                               bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpLoad(JBCStack2FEHelper &stack2feHelper,
                                                const jbc::JBCConstPool &constPool,
                                                bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpStore(JBCStack2FEHelper &stack2feHelper,
                                                 const jbc::JBCConstPool &constPool,
                                                 bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpArrayLoad(JBCStack2FEHelper &stack2feHelper,
                                                     const jbc::JBCConstPool &constPool,
                                                     bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpArrayStore(JBCStack2FEHelper &stack2feHelper,
                                                      const jbc::JBCConstPool &constPool,
                                                      bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpPop(JBCStack2FEHelper &stack2feHelper,
                                               const jbc::JBCConstPool &constPool,
                                               bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpDup(JBCStack2FEHelper &stack2feHelper,
                                               const jbc::JBCConstPool &constPool,
                                               bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpSwap(JBCStack2FEHelper &stack2feHelper,
                                                const jbc::JBCConstPool &constPool,
                                                bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpMathBinop(JBCStack2FEHelper &stack2feHelper,
                                                     const jbc::JBCConstPool &constPool,
                                                     bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpMathUnop(JBCStack2FEHelper &stack2feHelper,
                                                    const jbc::JBCConstPool &constPool,
                                                    bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpMathInc(JBCStack2FEHelper &stack2feHelper,
                                                   const jbc::JBCConstPool &constPool,
                                                   bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpConvert(JBCStack2FEHelper &stack2feHelper,
                                                   const jbc::JBCConstPool &constPool,
                                                   bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpCompare(JBCStack2FEHelper &stack2feHelper,
                                                   const jbc::JBCConstPool &constPool,
                                                   bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpReturn(JBCStack2FEHelper &stack2feHelper,
                                                  const jbc::JBCConstPool &constPool,
                                                  bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpStaticFieldOpr(JBCStack2FEHelper &stack2feHelper,
                                                          const jbc::JBCConstPool &constPool,
                                                          bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpFieldOpr(JBCStack2FEHelper &stack2feHelper,
                                                    const jbc::JBCConstPool &constPool,
                                                    bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpInvoke(JBCStack2FEHelper &stack2feHelper,
                                                  const jbc::JBCConstPool &constPool,
                                                  bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpNew(JBCStack2FEHelper &stack2feHelper,
                                               const jbc::JBCConstPool &constPool,
                                               bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpMultiANewArray(JBCStack2FEHelper &stack2feHelper,
                                                          const jbc::JBCConstPool &constPool,
                                                          bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpThrow(JBCStack2FEHelper &stack2feHelper,
                                                 const jbc::JBCConstPool &constPool,
                                                 bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpTypeCheck(JBCStack2FEHelper &stack2feHelper,
                                                     const jbc::JBCConstPool &constPool,
                                                     bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpMonitor(JBCStack2FEHelper &stack2feHelper,
                                                   const jbc::JBCConstPool &constPool,
                                                   bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpArrayLength(JBCStack2FEHelper &stack2feHelper,
                                                       const jbc::JBCConstPool &constPool,
                                                       bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRCommon(JBCStack2FEHelper &stack2feHelper,
                                             const jbc::JBCConstPool &constPool,
                                             bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRCommon2(JBCStack2FEHelper &stack2feHelper,
                                              const jbc::JBCConstPool &constPool,
                                              bool &success) const;
  UniqueFEIRStmt GenerateStmtForConstI32(JBCStack2FEHelper &stack2feHelper, int32 val, bool &success) const;
  UniqueFEIRStmt GenerateStmtForConstI64(JBCStack2FEHelper &stack2feHelper, int64 val, bool &success) const;
  UniqueFEIRStmt GenerateStmtForConstF32(JBCStack2FEHelper &stack2feHelper, float val, bool &success) const;
  UniqueFEIRStmt GenerateStmtForConstF64(JBCStack2FEHelper &stack2feHelper, double val, bool &success) const;
};

class JBCStmtPesudoLabel;

class JBCStmtInstBranch : public JBCStmt {
 public:
  explicit JBCStmtInstBranch(const jbc::JBCOp &argOp);
  ~JBCStmtInstBranch() = default;
  std::list<UniqueFEIRStmt> EmitToFEIRWithLabel(JBCStack2FEHelper &stack2feHelper,
                                                const std::map<uint32, JBCStmtPesudoLabel*> &mapPCStmtLabel,
                                                bool &success) const;

  const jbc::JBCOp &GetOp() const {
    return op;
  }

 protected:
  bool IsStmtInstImpl() const override;
  void DumpImpl(const std::string &prefix) const override;
  std::string DumpDotStringImpl() const override;
  std::list<UniqueFEIRStmt> EmitToFEIRImpl(JBCStack2FEHelper &stack2feHelper,
                                           const jbc::JBCConstPool &constPool,
                                           bool &success) const override;

 private:
  // bitwise mode
  enum {
    kModeDefault = 0,  // for int32 using normal opnd
    kModeUseRef = 0x1,  // bit0: 1 for ref, 0 for int32
    kModeUseZeroAsSecondOpnd = 0x2  // bit1: 1 for using 0 for 2nd opnd, 0 for using normal opnd
  };

  const jbc::JBCOp &op;
  using FuncPtrEmitToFEIR =
      std::list<UniqueFEIRStmt> (JBCStmtInstBranch::*)(JBCStack2FEHelper &stack2feHelper,
                                                       const std::map<uint32, JBCStmtPesudoLabel*> &mapPCStmtLabel,
                                                       bool &success) const;
  static std::map<jbc::JBCOpcodeKind, FuncPtrEmitToFEIR> funcPtrMapForEmitToFEIR;
  static std::map<jbc::JBCOpcodeKind, FuncPtrEmitToFEIR> InitFuncPtrMapForEmitToFEIR();
  static std::map<jbc::JBCOpcode, std::tuple<Opcode, Opcode, uint8>> opcodeMapForCondGoto;
  static std::map<jbc::JBCOpcode, std::tuple<Opcode, Opcode, uint8>> InitOpcodeMapForCondGoto();
  std::list<UniqueFEIRStmt> EmitToFEIRForOpGoto(JBCStack2FEHelper &stack2feHelper,
                                                const std::map<uint32, JBCStmtPesudoLabel*> &mapPCStmtLabel,
                                                bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpBranch(JBCStack2FEHelper &stack2feHelper,
                                                  const std::map<uint32, JBCStmtPesudoLabel*> &mapPCStmtLabel,
                                                  bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRForOpSwitch(JBCStack2FEHelper &stack2feHelper,
                                                  const std::map<uint32, JBCStmtPesudoLabel*> &mapPCStmtLabel,
                                                  bool &success) const;
  std::list<UniqueFEIRStmt> EmitToFEIRCommon(JBCStack2FEHelper &stack2feHelper,
                                             const std::map<uint32, JBCStmtPesudoLabel*> &mapPCStmtLabel,
                                             bool &success) const;
};

class JBCStmtPesudoLabel : public JBCStmt {
 public:
  JBCStmtPesudoLabel()
      : JBCStmt(GeneralStmtKind::kStmtMultiIn, kJBCStmtPesudoLabel),
        labelIdx(0) {}

  ~JBCStmtPesudoLabel() = default;
  void SetLabelIdx(uint32 arg) {
    labelIdx = arg;
  }

  uint32 GetLabelIdx() const {
    return labelIdx;
  }

 protected:
  void DumpImpl(const std::string &prefix) const override;
  std::string DumpDotStringImpl() const override;
  std::list<UniqueFEIRStmt> EmitToFEIRImpl(JBCStack2FEHelper &stack2feHelper,
                                           const jbc::JBCConstPool &constPool,
                                           bool &success) const override;

  uint32 labelIdx;
};

class JBCStmtPesudoCatch : public JBCStmtPesudoLabel {
 public:
  JBCStmtPesudoCatch()
      : JBCStmtPesudoLabel() {
    kind = kJBCStmtPesudoCatch;
  }
  ~JBCStmtPesudoCatch() = default;
  void AddCatchTypeName(const GStrIdx &nameIdx) {
    if (catchTypeNames.find(nameIdx) == catchTypeNames.end()) {
      CHECK_FATAL(catchTypeNames.insert(nameIdx).second, "catchTypeNames insert failed");
    }
  }

 protected:
  void DumpImpl(const std::string &prefix) const override;
  std::string DumpDotStringImpl() const override;
  std::list<UniqueFEIRStmt> EmitToFEIRImpl(JBCStack2FEHelper &stack2feHelper,
                                           const jbc::JBCConstPool &constPool,
                                           bool &success) const override;

 private:
  std::set<GStrIdx> catchTypeNames;
};

class JBCStmtPesudoTry : public JBCStmt {
 public:
  JBCStmtPesudoTry()
      : JBCStmt(kJBCStmtPesudoTry) {}

  ~JBCStmtPesudoTry() = default;
  void AddCatchStmt(JBCStmtPesudoCatch *stmt) {
    ASSERT(stmt != nullptr, "stmt is nullptr");
    catchStmts.push_back(stmt);
  }

  size_t GetCatchCount() const {
    return catchStmts.size();
  }

  JBCStmtPesudoCatch *GetCatchStmt(uint32 idx) {
    ASSERT(idx < catchStmts.size(), "index out of range");
    return static_cast<JBCStmtPesudoCatch*>(catchStmts[idx]);
  }

 protected:
  void DumpImpl(const std::string &prefix) const override;
  std::string DumpDotStringImpl() const override;
  std::list<UniqueFEIRStmt> EmitToFEIRImpl(JBCStack2FEHelper &stack2feHelper,
                                           const jbc::JBCConstPool &constPool,
                                           bool &success) const override;

 private:
  std::vector<JBCStmtPesudoCatch*> catchStmts;
};

class JBCStmtPesudoEndTry : public JBCStmt {
 public:
  JBCStmtPesudoEndTry()
      : JBCStmt(kJBCStmtPesudoEndTry) {
    isAuxPost = true;
  }

  ~JBCStmtPesudoEndTry() = default;

 protected:
  void DumpImpl(const std::string &prefix) const override;
  std::string DumpDotStringImpl() const override;
  std::list<UniqueFEIRStmt> EmitToFEIRImpl(JBCStack2FEHelper &stack2feHelper,
                                           const jbc::JBCConstPool &constPool,
                                           bool &success) const override;
};

class JBCStmtPesudoComment : public JBCStmt {
 public:
  explicit JBCStmtPesudoComment(const std::string &argContent)
      : JBCStmt(kJBCStmtPesudoComment),
        content(argContent) {
    isAuxPre = true;
  }

  ~JBCStmtPesudoComment() = default;
  void SetContent(const std::string &argContent) {
    content = argContent;
  }

 protected:
  void DumpImpl(const std::string &prefix) const override;
  std::string DumpDotStringImpl() const override;
  std::list<UniqueFEIRStmt> EmitToFEIRImpl(JBCStack2FEHelper &stack2feHelper,
                                           const jbc::JBCConstPool &constPool,
                                           bool &success) const override;

 private:
  std::string content = "";
};  // class JBCStmtPesudoComment

class JBCStmtPesudoLOC : public JBCStmt {
 public:
  JBCStmtPesudoLOC()
      : JBCStmt(kJBCStmtPesudoLOC),
        srcFileIdx(0),
        lineNumber(0) {
    isAuxPre = true;
  }

  JBCStmtPesudoLOC(uint32 argSrcFileIdx, uint32 argLineNumber)
      : JBCStmt(kJBCStmtPesudoLOC),
        srcFileIdx(argSrcFileIdx),
        lineNumber(argLineNumber) {
    isAuxPre = true;
  }

  ~JBCStmtPesudoLOC() = default;
  void SetSrcFileIdx(uint32 idx) {
    srcFileIdx = idx;
  }

  uint32 GetSrcFileIdx() const {
    return srcFileIdx;
  }

  void SetLineNumber(uint32 line) {
    lineNumber = line;
  }

  uint32 GetLineNumber() const {
    return lineNumber;
  }

 protected:
  void DumpImpl(const std::string &prefix) const override;
  std::string DumpDotStringImpl() const override;
  std::list<UniqueFEIRStmt> EmitToFEIRImpl(JBCStack2FEHelper &stack2feHelper,
                                           const jbc::JBCConstPool &constPool,
                                           bool &success) const override;

 private:
  uint32 srcFileIdx;
  uint32 lineNumber;
};
}
#endif  // MPLFE_INCLUDE_JBC_STMT_H
