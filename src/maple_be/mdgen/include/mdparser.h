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
#ifndef MAPLEBE_MDGEN_INCLUDE_MDPARSER_H
#define MAPLEBE_MDGEN_INCLUDE_MDPARSER_H

#include "mdlexer.h"
#include "mdrecord.h"
#include "mempool.h"

namespace MDGen{
class MDParser {
 public:
  MDParser(MDClassRange &newKeeper, maple::MemPool *memPool) : dataKeeper(newKeeper), mdMemPool(memPool) {
  }
  ~MDParser() = default;

  bool ParseFile(const std::string &inputFile);
  bool ParseObjectStart();
  bool ParseObject();
  bool IsObjectStart(MDTokenKind k) const;
  bool ParseDefType();
  bool ParseMDClass();
  bool ParseMDClassBody(MDClass &oneClass);
  bool ParseMDObject();
  bool ParseMDObjBody(MDObject &curObj);
  bool ParseIntElement(MDObject &curObj, bool isVec);
  bool ParseStrElement(MDObject &curObj, bool isVec);
  bool ParseDefTyElement(MDObject &curObj, bool isVec, std::set<int> &childSet);
  bool ParseDefObjElement(MDObject &curObj, bool isVec, MDClass &pClass);

  /* error process */
  bool EmitError(const std::string &errMsg);

 private:
  MDLexer lexer;
  MDClassRange &dataKeeper;
  maple::MemPool *mdMemPool;
};
} /* namespace MDGen */

#endif /* MAPLEBE_MDGEN_INCLUDE_MDPARSER_H */