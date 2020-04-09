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
#ifndef MPLFE_INCLUDE_COMMON_FE_MANAGER_H
#define MPLFE_INCLUDE_COMMON_FE_MANAGER_H
#include <memory>
#include "mir_module.h"
#include "mir_builder.h"
#include "fe_type_manager.h"
#include "fe_java_string_manager.h"

namespace maple {
class FEManager {
 public:
  static FEManager &GetManager() {
    ASSERT(manager, "manager is not initialize");
    return *manager;
  }

  static FETypeManager &GetTypeManager() {
    ASSERT(manager, "manager is not initialize");
    return manager->typeManager;
  }

  static FEJavaStringManager &GetJavaStringManager() {
    ASSERT(manager, "manager is not initialize");
    return manager->javaStringManager;
  }

  static MIRBuilder &GetMIRBuilder() {
    ASSERT(manager, "manager is not initialize");
    return manager->builder;
  }

  static void Init(MIRModule &moduleIn) {
    manager = new FEManager(moduleIn);
  }

  static void Release() {
    if (manager != nullptr) {
      manager->typeManager.ReleaseMemPool();
      delete manager;
      manager = nullptr;
    }
  }

 private:
  static FEManager *manager;
  MIRModule &module;
  FETypeManager typeManager;
  FEJavaStringManager javaStringManager;
  MIRBuilder builder;
  explicit FEManager(MIRModule &moduleIn)
      : module(moduleIn), typeManager(module), javaStringManager(moduleIn), builder(&module) {}
  ~FEManager() = default;
};
}  // namespace maple
#endif  // MPLFE_INCLUDE_COMMON_FE_MANAGER_H