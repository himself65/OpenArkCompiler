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
#ifndef MPLFE_INCLUDE_JBC_CLASS_HEADER_H
#define MPLFE_INCLUDE_JBC_CLASS_HEADER_H
#include "types_def.h"
#include "basic_io.h"

namespace maple {
namespace jbc {
class JBCClassHeader {
 public:
  JBCClassHeader();
  ~JBCClassHeader() = default;
  bool ParseClassFile(BasicIORead &io) const;

 private:
  uint32 magic;
  uint16 minorVersion;
  uint16 majorVersion;
  uint16 constPoolCount;
  uint16 accessFlag;
  uint16 thisClass;
  uint16 superClass;
  uint16 interfacesCount;
  uint16 fieldsCount;
  uint16 methodsCount;
  uint16 attrsCount;
};
}  // namespace jbc
}  // namespace maple
#endif