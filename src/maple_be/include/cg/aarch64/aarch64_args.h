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
#ifndef MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_ARGS_H
#define MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_ARGS_H

#include "args.h"
#include "aarch64_cgfunc.h"

namespace maplebe {
using namespace maple;

struct ArgInfo {
  AArch64reg reg;
  MIRType *mirTy;
  uint32 symSize;
  uint32 stkSize;
  RegType regType;
  MIRSymbol *sym;
  const AArch64SymbolAlloc *symLoc;
};

class AArch64MoveRegArgs : public MoveRegArgs {
 public:
  explicit AArch64MoveRegArgs(CGFunc &func) : MoveRegArgs(func) {}
  ~AArch64MoveRegArgs() override = default;
  void Run() override;

 private:
  RegOperand *baseReg = nullptr;
  const MemSegment *lastSegment = nullptr;
  void CollectRegisterArgs(std::map<uint32, AArch64reg> &argsList, std::vector<uint32> &indexList) const;
  ArgInfo GetArgInfo(std::map<uint32, AArch64reg> &argsList, uint32 argIndex) const;
  bool IsInSameSegment(const ArgInfo &firstArgInfo, const ArgInfo &secondArgInfo) const;
  void GenerateStpInsn(const ArgInfo &firstArgInfo, const ArgInfo &secondArgInfo);
  void GenerateStrInsn(ArgInfo &argInfo);
  void MoveRegisterArgs();
  void MoveVRegisterArgs();
  void MoveLocalRefVarToRefLocals(MIRSymbol &mirSym);
  void LoadStackArgsToVReg(MIRSymbol &mirSym);
  void MoveArgsToVReg(const PLocInfo &ploc, MIRSymbol &mirSym);
};
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_ARGS_H */