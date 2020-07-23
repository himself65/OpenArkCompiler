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
#ifndef MAPLEBE_INCLUDE_CG_ASM_EMIT_H
#define MAPLEBE_INCLUDE_CG_ASM_EMIT_H

#include "emit.h"

namespace maplebe {
class AsmFuncEmitInfo : public FuncEmitInfo {
 public:
  AsmFuncEmitInfo(CGFunc &func) : FuncEmitInfo(func) {}
  virtual ~AsmFuncEmitInfo() = default;
};

class AsmEmitter : public Emitter {
 protected:
  AsmEmitter(CG &cg, const std::string &asmFileName) : Emitter(cg, asmFileName) {}
  virtual ~AsmEmitter() = default;

  virtual void EmitRefToMethodDesc(FuncEmitInfo &funcEmitInfo, Emitter &emitter) = 0;
  virtual void EmitRefToMethodInfo(FuncEmitInfo &funcEmitInfo, Emitter &emitter) = 0;
  virtual void EmitMethodDesc(FuncEmitInfo &funcEmitInfo, Emitter &emitter) = 0;
  virtual void EmitFastLSDA(FuncEmitInfo &funcEmitInfo) = 0;
  virtual void EmitFullLSDA(FuncEmitInfo &funcEmitInfo) = 0;
  virtual void EmitBBHeaderLabel(FuncEmitInfo &funcEmitInfo, const std::string &name, LabelIdx labIdx) = 0;
  virtual void EmitJavaInsnAddr(FuncEmitInfo &funcEmitInfo) = 0;
  virtual void Run(FuncEmitInfo &funcEmitInfo) = 0;
};
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_ASM_EMIT_H */
