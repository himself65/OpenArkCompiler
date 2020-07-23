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
#ifndef MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_EMITTER_H
#define MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_EMITTER_H

#include "asm_emit.h"

namespace maplebe {
using namespace maple;

class AArch64AsmEmitter : public AsmEmitter {
 public:
  AArch64AsmEmitter(CG &cg, const std::string &asmFileName) : AsmEmitter(cg, asmFileName) {}
  ~AArch64AsmEmitter() = default;

  void EmitRefToMethodDesc(FuncEmitInfo &funcEmitInfo, Emitter &emitter) override;
  void EmitRefToMethodInfo(FuncEmitInfo &funcEmitInfo, Emitter &emitter) override;
  void EmitMethodDesc(FuncEmitInfo &funcEmitInfo, Emitter &emitter) override;
  void EmitFastLSDA(FuncEmitInfo &funcEmitInfo) override;
  void EmitFullLSDA(FuncEmitInfo &funcEmitInfo) override;
  void EmitBBHeaderLabel(FuncEmitInfo &funcEmitInfo, const std::string &name, LabelIdx labIdx) override;
  void EmitJavaInsnAddr(FuncEmitInfo &funcEmitInfo) override;
  void Run(FuncEmitInfo &funcEmitInfo) override;
};
}  /* namespace maplebe */

#endif /* MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_EMITTER_H */
