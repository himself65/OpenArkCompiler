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

#include "aarch64_isa.h"

namespace maplebe {
/*
 * Get the ldp/stp corresponding to ldr/str
 * mop : a ldr or str machine operator
 */
MOperator GetMopPair(MOperator mop) {
  switch (mop) {
    case MOP_xldr:
      return MOP_xldp;
    case MOP_wldr:
      return MOP_wldp;
    case MOP_xstr:
      return MOP_xstp;
    case MOP_wstr:
      return MOP_wstp;
    case MOP_dldr:
      return MOP_dldp;
    case MOP_sldr:
      return MOP_sldp;
    case MOP_dstr:
      return MOP_dstp;
    case MOP_sstr:
      return MOP_sstp;
    default:
      ASSERT(false, "should not run here");
      return MOP_undef;
  }
}
}  /* namespace maplebe */