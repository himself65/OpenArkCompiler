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
#ifndef MPLFE_INCLUDE_JBC_CLASS_ACCESS_H
#define MPLFE_INCLUDE_JBC_CLASS_ACCESS_H

namespace maple {
namespace jbc {
enum JBCClassAccessFlag {
  kAccClassPublic = 0x0001,
  kAccClassFinal = 0x0010,
  kAccClassSuper = 0x0020,
  kAccClassInterface = 0x0200,
  kAccClassAbstract = 0x0400,
  kAccClassSynthetic = 0x1000,
  kAccClassAnnotation = 0x2000,
  kAccClassEnum = 0x4000,
};

enum JBCClassFieldAccessFlag {
  kAccFieldPublic = 0x0001,
  kAccFieldPrivate = 0x0002,
  kAccFieldProtected = 0x0004,
  kAccFieldStatic = 0x0008,
  kAccFieldFinal = 0x0010,
  kAccFieldVolatile = 0x0040,
  kAccFieldTransient = 0x0080,
  kAccFieldSynthetic = 0x1000,
  kAccFieldEnum = 0x4000,
};

enum JBCClassMethodAccessFlag {
  kAccMethodPublic = 0x0001,
  kAccMethodPrivate = 0x0002,
  kAccMethodProtected = 0x0004,
  kAccMethodStatic = 0x0008,
  kAccMethodFinal = 0x0010,
  kAccMethodSynchronized = 0x0020,
  kAccMethodBridge = 0x0040,
  kAccMethodVarargs = 0x0080,
  kAccMethodNative = 0x0100,
  kAccMethodAbstract = 0x0400,
  kAccMethodStrict = 0x0800,
  kAccMethodSynthetic = 0x1000,
};

enum JBCAccessFlag {
  kAccPublic = 0x0001,
  kAccPrivate = 0x0002,
  kAccProtected = 0x0004,
  kAccStatic = 0x0008,
  kAccFinal = 0x0010,
  kAccSuperOrSynchronized = 0x0020,
  kAccBridgeOrVolatile = 0x0040,
  kAccVarargsOrTransient = 0x0080,
  kAccNative = 0x0100,
  kAccInterface = 0x0200,
  kAccAbstract = 0x0400,
  kAccStrict = 0x0800,
  kAccSynthetic = 0x1000,
  kAccAnnotation = 0x2000,
  kAccEnum = 0x4000,
};
}  // namespace jbc
}  // namespace maple
#endif  // MPLFE_INCLUDE_JBC_CLASS_ACCESS_H