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
#ifndef MPLFE_INCLUDE_COMMON_FE_ALGORITHM_H
#define MPLFE_INCLUDE_COMMON_FE_ALGORITHM_H
#include <list>
#include <set>
#include <map>
#include "types_def.h"
#include "fe_configs.h"

namespace maple {
template <typename T>
class CorrelativeMerge {
 public:
  using PtrMergeFunc = bool (T::*)(const T&);
  using PtrMergableFunc = bool (T::*)() const;
  CorrelativeMerge(std::map<T*, std::set<T*>> &argCorrelationMap, PtrMergeFunc argPtrMergeFunc,
                   PtrMergableFunc argPtrMergableFunc)
      : correlationMap(argCorrelationMap),
        ptrMergeFunc(argPtrMergeFunc),
        ptrMergableFunc(argPtrMergableFunc),
        inLoop(false),
        error(false),
        success(false),
        visitCount(0) {}

  ~CorrelativeMerge() = default;
  void ProcessAll() {
    visitCount = 0;
    // loop check
    LoopCheckAll();
    // process
    CHECK_NULL_FATAL(ptrMergableFunc);
    for (const std::pair<T*, std::set<T*>> &item : correlationMap) {
      T *dst = item.first;
      CHECK_NULL_FATAL(dst);
      if ((dst->*ptrMergableFunc)()) {
        continue;
      }
      ProcessOne(*dst, true);
      if (!success) {
        error = true;
        break;
      }
    }
  }

  void LoopCheckAll() {
    for (const std::pair<T*, std::set<T*>> &item : correlationMap) {
      visitedSet.clear();
      T *dst = item.first;
      CHECK_NULL_FATAL(dst);
      inLoop = false;
      LoopCheck(*dst);
      loopStatus[dst] = inLoop;
    }
  }

  void LoopCheck(T &dst) {
    // loop check
    if (visitedSet.find(&dst) != visitedSet.end()) {
      inLoop = true;
      return;
    }
    // correlation check
    CHECK_FATAL(visitedSet.insert(&dst).second, "visitedSet insert failed");
    auto itCorr = correlationMap.find(&dst);
    if (itCorr != correlationMap.end()) {
      CHECK_NULL_FATAL(ptrMergableFunc);
      for (T *src : itCorr->second) {
        CHECK_NULL_FATAL(src);
        if (!(src->*ptrMergableFunc)()) {
          LoopCheck(*src);
          if (inLoop) {
            return;
          }
        }
      }
    }
  }

  void ProcessOne(T &dst, bool first) {
    if (first) {
      success = true;
      visitedSet.clear();
    }
    // loop check
    visitCount++;
    // correlation end check
    auto itCorr = correlationMap.find(&dst);
    if (itCorr == correlationMap.end()) {
      return;
    }
    CHECK_FATAL(visitedSet.insert(&dst).second, "visitedSet insert failed");
    // correlation check
    auto itLoopStatus = loopStatus.find(&dst);
    ASSERT(itLoopStatus != loopStatus.end(), "loop status not existed");
    bool updated = (first || !itLoopStatus->second);
    if (ptrMergableFunc == nullptr || ptrMergeFunc == nullptr) {
      CHECK_FATAL(false, "nullptr error.");
    }
    for (T *src : itCorr->second) {
      CHECK_NULL_FATAL(src);
      if (correlationMap.find(src) == correlationMap.end()) {
        success = success && (dst.*ptrMergeFunc)(*src);
      } else if ((src->*ptrMergableFunc)()) {
        success = success && (dst.*ptrMergeFunc)(*src);
      } else if (updatedSet.find(src) != updatedSet.end()) {
        success = success && (dst.*ptrMergeFunc)(*src);
      } else {
        if (visitedSet.find(src) == visitedSet.end()) {
          ProcessOne(*src, false);
          success = success && (dst.*ptrMergeFunc)(*src);
        }
      }
    }
    if (!updated) {
      return;
    }
    if (updatedSet.find(&dst) == updatedSet.end()) {
      CHECK_FATAL(updatedSet.insert(&dst).second, "updatedSet insert failed");
    }
  }

  uint32 GetVisitCount() const {
    return visitCount;
  }

  bool GetError() const {
    return error;
  }

 LLT_PRIVATE:
  std::map<T*, std::set<T*>> &correlationMap;
  PtrMergeFunc ptrMergeFunc;
  PtrMergableFunc ptrMergableFunc;
  std::set<T*> visitedSet;
  std::set<T*> updatedSet;
  std::map<T*, bool> loopStatus;
  bool inLoop : 1;
  bool error : 1;
  bool success : 1;
  uint32 visitCount;
};  // class CorrelativeMerge
}  // namespace maple
#endif
