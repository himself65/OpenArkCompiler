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
#ifndef MPLFE_INCLUDE_FE_UTILS_H
#define MPLFE_INCLUDE_FE_UTILS_H
#include <vector>
#include <string>
#include <list>
#include "mpl_logging.h"
#include "prim_types.h"

namespace maple {
class FEUtils {
 public:
  FEUtils() = default;
  ~FEUtils() = default;
  static std::vector<std::string> Split(const std::string &str, char delim);
  static uint8 GetWidth(PrimType primType);
  static bool IsInteger(PrimType primType);
  static bool IsSignedInteger(PrimType primType);
  static bool IsUnsignedInteger(PrimType primType);
  static PrimType MergePrimType(PrimType primType1, PrimType primType2);

 private:
  static bool LogicXOR(bool v1, bool v2) {
    return (v1 && !v2) || (!v1 && v2);
  }
};

class FELinkListNode {
 public:
  FELinkListNode();
  virtual ~FELinkListNode();
  void InsertBefore(FELinkListNode *ins);
  void InsertAfter(FELinkListNode *ins);
  static void InsertBefore(FELinkListNode *ins, FELinkListNode *pos);
  static void InsertAfter(FELinkListNode *ins, FELinkListNode *pos);
  FELinkListNode *GetPrev() const {
    return prev;
  }

  void SetPrev(FELinkListNode *node) {
    CHECK_NULL_FATAL(node);
    prev = node;
  }

  void SetPrevNull() {
    prev = nullptr;
  }

  FELinkListNode *GetNext() const {
    return next;
  }

  void SetNextNull() {
    next = nullptr;
  }

  void SetNext(FELinkListNode *node) {
    CHECK_NULL_FATAL(node);
    next = node;
  }

 protected:
  FELinkListNode *prev;
  FELinkListNode *next;
};
}  // namespace maple
#endif  // MPLFE_INCLUDE_FE_UTILS_H