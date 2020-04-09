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
#include "jbc_class_header.h"
namespace maple {
namespace jbc {
JBCClassHeader::JBCClassHeader()
    : magic(0),
      minorVersion(0),
      majorVersion(0),
      constPoolCount(0),
      accessFlag(0),
      thisClass(0),
      superClass(0),
      interfacesCount(0),
      fieldsCount(0),
      methodsCount(0),
      attrsCount(0) {}

bool JBCClassHeader::ParseClassFile(BasicIORead &io) const {
  return false;
}
}  // namespace jbc
}  // namespace maple