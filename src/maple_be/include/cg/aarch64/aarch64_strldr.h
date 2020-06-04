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
#ifndef MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_STRLDR_H
#define MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_STRLDR_H

#include "strldr.h"
#include "aarch64_reaching.h"

namespace maplebe {
using namespace maple;

class AArch64StoreLoadOpt : public StoreLoadOpt {
 public:
  AArch64StoreLoadOpt(CGFunc &func, MemPool &memPool)
      : StoreLoadOpt(func, memPool), localAlloc(&memPool), str2MovMap(localAlloc.Adapter()) {}
  ~AArch64StoreLoadOpt() override = default;
  void Run() final;
  void DoStoreLoadOpt();
  void DoLoadZeroToMoveTransfer(const Insn&, short, const InsnSet&) const;
  void DoLoadToMoveTransfer(Insn&, short, short, const InsnSet&);
  bool CheckStoreOpCode(MOperator opCode) const;
 private:
  void ProcessStrPair(Insn &insn);
  void ProcessStr(Insn &insn);
  void GenerateMoveLiveInsn(RegOperand &resRegOpnd, RegOperand &srcRegOpnd,
                            Insn &ldrInsn, Insn &strInsn, short memSeq);
  void GenerateMoveDeadInsn(RegOperand &resRegOpnd, RegOperand &srcRegOpnd,
                            Insn &ldrInsn, Insn &strInsn, short memSeq);
  MapleAllocator localAlloc;
  /* the max number of mov insn to optimize. */
  static constexpr uint8 kMaxMovNum = 2;
  MapleMap<Insn*, Insn*[kMaxMovNum]> str2MovMap;
};
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_STRLDR_H */