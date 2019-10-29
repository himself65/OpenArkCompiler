/*
 * Copyright (c) [2019] Huawei Technologies Co.,Ltd.All rights reserved.
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
#ifndef MAPLE_ME_INCLUDE_UNION_FIND_H
#define MAPLE_ME_INCLUDE_UNION_FIND_H
#include "mempool.h"
#include "mempool_allocator.h"
#include "mpl_logging.h"
#include "types_def.h"

namespace maple {
// This uses the Weighted Quick Union with Path Compression algorithm.
// Build a flattened tree to represent each class, where all its class members
// are linked together via the tree. The tree is represented by child pointing
// to its parent only. The members of the same class are identified at any time
// by having the same root, and the id of its root member can be used as class
// id.
class UnionFind {
 public:
  explicit UnionFind(MemPool &memPool)
      : ufAlloc(&memPool), num(0), id(ufAlloc.Adapter()), sz(ufAlloc.Adapter()) {}

  UnionFind(MemPool &memPool, uint32 siz)
      : ufAlloc(&memPool), num(siz), id(siz, 0, ufAlloc.Adapter()), sz(siz, 0, ufAlloc.Adapter()) {
    Reinit();
  }

  ~UnionFind() {
    // for the root id's, the sum of their size should be population size
#if DEBUG
    unsigned int sum = 0;
    for (unsigned int i = 0; i < num; i++)
      if (id[i] == i) {
        // it is a root
        sum += sz[i];
      }
    ASSERT(sum == num, "Something wrong in UnionFind");
#endif
  }

  void Reinit() {
    for (unsigned int i = 0; i < num; i++) {
      id[i] = i;
      sz[i] = 1;
    }
  }

  unsigned int NewMember() {
    id.push_back(num);  // new member is its own root
    sz.push_back(1);
    return ++num;
  }

  unsigned int Root(unsigned int i) {
    while (id[i] != i) {
      id[i] = id[id[i]];  // this compresses the path
      i = id[i];
    }
    return i;
  }

  bool Find(unsigned int p, unsigned int q) {
    return Root(p) == Root(q);
  }

  void Union(unsigned int p, unsigned int q) {
    unsigned int i = Root(p);
    unsigned int j = Root(q);
    if (i == j) {
      return;
    }
    // construct a balanced tree
    if (sz[i] < sz[j]) {
      id[i] = j;
      sz[j] += sz[i];
    } else {
      id[j] = i;
      sz.at(i) += sz.at(j);
    }
  }

  unsigned int GetElementsNumber(int i) const {
    ASSERT(i < sz.size(), "index out of range");
    return sz[i];
  }

  bool SingleMemberClass(unsigned int p) {
    unsigned int i = Root(p);
    ASSERT(i < sz.size(), "index out of range");
    return sz[i] == 1;
  }

 private:
  MapleAllocator ufAlloc;
  unsigned int num;                     // the population size; can continue to increase
  MapleVector<unsigned int> id;  // array index is id of each population member;
  // value is id of the root member of its class;
  // the member is a root if its value is itself;
  // as its root changes, will keep updating so as to
  // maintain a flat tree
  MapleVector<unsigned int> sz;  // gives number of elements in the tree rooted there
};
}  // namespace maple
#endif  // MAPLE_ME_INCLUDE_UNION_FIND_H
