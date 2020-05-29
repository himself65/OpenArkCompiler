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
#ifndef MAPLEBE_INCLUDE_CG_STRLDR_H
#define MAPLEBE_INCLUDE_CG_STRLDR_H
#include "cg_phase.h"

namespace maplebe {
class StoreLoadOpt {
 public:
  StoreLoadOpt(CGFunc &func, MemPool &memPool) : cgFunc(func), memPool(memPool) {}
  virtual ~StoreLoadOpt() = default;
  virtual void Run() = 0;
  std::string PhaseName() const {
    return "storeloadopt";
  }

 protected:
  CGFunc &cgFunc;
  MemPool &memPool;
  /* if the number of bbs is more than 500 or the number of insns is more than 9000, don't optimize. */
  static constexpr uint32 kMaxBBNum = 500;
  static constexpr uint32 kMaxInsnNum = 9000;
};

CGFUNCPHASE_CANSKIP(CgDoStoreLoadOpt, "storeloadopt")
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_STRLDR_H */