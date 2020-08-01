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
#include "feir_test_base.h"
#include "mplfe_ut_environment.h"

namespace maple {
MemPool *FEIRTestBase::mp = nullptr;

FEIRTestBase::FEIRTestBase()
    : allocator(mp),
      mirBuilder(&MPLFEUTEnvironment::GetMIRModule()),
      func(&MPLFEUTEnvironment::GetMIRModule(), StIdx(0, 0)) {
  func.Init();
  mirBuilder.SetCurrentFunction(func);
}

void FEIRTestBase::SetUpTestCase() {
  mp = memPoolCtrler.NewMemPool("MemPool for FEIRTestBase");
}

void FEIRTestBase::TearDownTestCase() {
  memPoolCtrler.DeleteMemPool(mp);
  mp = nullptr;
}
}  // namespace maple