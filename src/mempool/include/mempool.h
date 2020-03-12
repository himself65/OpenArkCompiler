/*
 * Copyright (c) [2019-2020] Huawei Technologies Co.,Ltd.All rights reserved.
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
#ifndef MEMPOOL_INCLUDE_MEMPOOL_H
#define MEMPOOL_INCLUDE_MEMPOOL_H
#include <list>
#include <set>
#include <stack>
#include <map>
#include <string>
#include "mir_config.h"
#include "mpl_logging.h"


namespace maple {
#ifdef _WIN32
#define FALSE 0
#define TRUE 1
#else
constexpr bool FALSE = false;
constexpr bool TRUE = true;
#endif

// Class declaration
class MemPool;  // circular dependency exists, no other choice
// Memory Pool controller class
class MemPoolCtrler {
  friend MemPool;

 public:  // Methods
  static bool freeMemInTime;
  MemPoolCtrler() = default;

  ~MemPoolCtrler();

  MemPool *NewMemPool(const std::string&);
  void DeleteMemPool(MemPool *memPool);
  void FreeMem() {
    for (MemBlock *block : freeMemBlocks) {
      free(block);
    }
    for (auto it = largeFreeMemBlocks.begin(); it != largeFreeMemBlocks.end(); ++it) {
      for (auto itr = (*it).second.begin(); itr != (*it).second.end(); ++itr) {
        free(*itr);
      }
    }
    freeMemBlocks.clear();
    largeFreeMemBlocks.clear();
  }
  bool IsEmpty() const {
    return memPools.empty();
  }

 private:  // Methods
  struct MemBlock {
    unsigned int available;  // Available memory size
    unsigned int origSize;   // original size
    void *ptr;               // Current pointer to the first available position
  };

  class MemBlockCmp {
   public:
    bool operator()(const MemBlock *l, const MemBlock *r) const {
      if (l->available != r->available) {
        return l->available > r->available;
      }
      return (std::uintptr_t)(l->ptr) > (std::uintptr_t)(r->ptr);
    }
  };

  // Free small/large size memory block list
  std::list<MemBlock*> freeMemBlocks;
  std::map<unsigned int, std::set<MemBlock*, MemBlockCmp>> largeFreeMemBlocks;
  std::set<MemPool*> memPools;  // set of mempools managed by it
};

class MemPool {
  friend MemPoolCtrler;

 public:  // Methods
  MemPool(MemPoolCtrler &ctl, const std::string &name) : ctrler(&ctl), name(name) {
  }

  ~MemPool();
  void *Malloc(size_t size);
  void *Calloc(size_t size);
  void *Realloc(const void *ptr, size_t oldSize, size_t newSize);
  void ReleaseContainingMem();
  const std::string &GetName() const {
    return name;
  }

  template <class T>
  T *Clone(const T &t) {
    void *p = Malloc(sizeof(T));
    CHECK_FATAL(p != nullptr, "ERROR: New error");
    p = new (p) T(t);  // Call clone constructor
    return static_cast<T*>(p);
  }

  // New templates
  template <class T, typename... Arguments>
  T *New(Arguments &&... args) {
    void *p = Malloc(sizeof(T));
    CHECK_FATAL(p != nullptr, "ERROR: New error");
    p = new (p) T(std::forward<Arguments>(args)...);  // Call constructor
    return static_cast<T*>(p);
  }

  // New Array template
  template <class T>
  T *NewArray(unsigned int num) {
    void *p = Malloc(sizeof(T) * num);
    CHECK_FATAL(p != nullptr, "ERROR: NewArray error");
    p = new (p) T[num];
    return static_cast<T*>(p);
  }

#define BITS_ALIGN(size) (((size) + 7) & (0xFFFFFFF8))
#define MEM_BLOCK_FIRST_PTR(x) \
  static_cast<void*>((reinterpret_cast<char*>(x)) + BITS_ALIGN(sizeof(MemPoolCtrler::MemBlock)))

 private:                                         // constants
  static constexpr size_t kMinBlockSize = 0x800;  // Minimum BlockSize is 2K
  static constexpr size_t kMemBlockOverhead = (BITS_ALIGN(sizeof(MemPoolCtrler::MemBlock)));
  MemPoolCtrler::MemBlock *GetLargeMemBlock(size_t size);  // Raw allocate large memory block
  MemPoolCtrler::MemBlock *GetMemBlock();
  MemPoolCtrler *ctrler;  // Hookup controller object
  std::string name;       // Name of the memory pool
  // Save the memory block stack
  std::stack<MemPoolCtrler::MemBlock*> memBlockStack;
  std::stack<MemPoolCtrler::MemBlock*> largeMemBlockStack;
};

extern MemPoolCtrler memPoolCtrler;
}  // namespace maple
#endif  // MEMPOOL_INCLUDE_MEMPOOL_H
