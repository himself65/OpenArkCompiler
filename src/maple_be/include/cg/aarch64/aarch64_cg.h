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
#ifndef MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_CG_H
#define MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_CG_H

#include "cg.h"
#include "aarch64_cgfunc.h"

namespace maplebe {
constexpr int64 kShortBRDistance = (8 * 1024);
constexpr int64 kNegativeImmLowerLimit = -4096;
constexpr int32 kIntRegTypeNum = 5;

/* Supporting classes for GCTIB merging */
class GCTIBKey {
 public:
  GCTIBKey(MapleAllocator &allocator, uint32 rcHeader, std::vector<uint64> &patternWords)
      : header(rcHeader), bitMapWords(allocator.Adapter()) {
    bitMapWords.insert(bitMapWords.begin(), patternWords.begin(), patternWords.end());
  }

  ~GCTIBKey() = default;

  uint32 GetHeader() const {
    return header;
  }

  const MapleVector<uint64> &GetBitmapWords() const {
    return bitMapWords;
  }

 private:
  uint32 header;
  MapleVector<uint64> bitMapWords;
};

class Hasher {
 public:
  size_t operator()(const GCTIBKey *key) const {
    CHECK_NULL_FATAL(key);
    size_t hash = key->GetHeader();
    return hash;
  }
};

class EqualFn {
 public:
  bool operator()(const GCTIBKey *firstKey, const GCTIBKey *secondKey) const {
    CHECK_NULL_FATAL(firstKey);
    CHECK_NULL_FATAL(secondKey);
    const MapleVector<uint64> &firstWords = firstKey->GetBitmapWords();
    const MapleVector<uint64> &secondWords = secondKey->GetBitmapWords();

    if ((firstKey->GetHeader() != secondKey->GetHeader()) || (firstWords.size() != secondWords.size())) {
      return false;
    }

    for (size_t i = 0; i < firstWords.size(); ++i) {
      if (firstWords[i] != secondWords[i]) {
        return false;
      }
    }
    return true;
  }
};

class GCTIBPattern {
 public:
  GCTIBPattern(GCTIBKey &patternKey, MemPool &mp) : name(&mp) {
    key = &patternKey;
    id = GetId();
    name = GCTIB_PREFIX_STR + std::string("PTN_") + std::to_string(id);
  }

  ~GCTIBPattern() = default;

  int GetId() {
    static int id = 0;
    return id++;
  }

  std::string GetName() const {
    ASSERT(!name.empty(), "null name check!");
    return std::string(name.c_str());
  }

  void SetName(const std::string &ptnName) {
    name = ptnName;
  }

 private:
  int id;
  MapleString name;
  GCTIBKey *key;
};

class AArch64CG : public CG {
 public:
  AArch64CG(MIRModule &mod, const CGOptions &opts, const std::vector<std::string> &nameVec,
            const std::unordered_map<std::string, std::vector<std::string>> &patternMap)
      : CG(mod, opts),
        ehExclusiveNameVec(nameVec),
        cyclePatternMap(patternMap),
        keyPatternMap(allocator.Adapter()),
        symbolPatternMap(allocator.Adapter()) {}

  ~AArch64CG() override = default;

  CGFunc *CreateCGFunc(MIRModule &mod, MIRFunction &mirFunc, BECommon &bec, MemPool &memPool,
                       MapleAllocator &mallocator, uint32 funcId) override {
    return memPool.New<AArch64CGFunc>(mod, *this, mirFunc, bec, memPool, mallocator, funcId);
  }

  const std::unordered_map<std::string, std::vector<std::string>> &GetCyclePatternMap() const {
    return cyclePatternMap;
  }

  void GenerateObjectMaps(BECommon &beCommon) override;

  bool IsExclusiveFunc(MIRFunction&) override;

  void FindOrCreateRepresentiveSym(std::vector<uint64> &bitmapWords, uint32 rcHeader, const std::string &name);

  void CreateRefSymForGlobalPtn(GCTIBPattern &ptn);

  std::string FindGCTIBPatternName(const std::string &name) const override;

  static const AArch64MD kMd[kMopLast];
  enum : uint8 {
    kR8List,
    kR16List,
    kR32List,
    kR64List,
    kV64List
  };
  static std::array<std::array<const std::string, kAllRegNum>, kIntRegTypeNum> intRegNames;

 private:
  const std::vector<std::string> &ehExclusiveNameVec;
  const std::unordered_map<std::string, std::vector<std::string>> &cyclePatternMap;
  MapleUnorderedMap<GCTIBKey*, GCTIBPattern*, Hasher, EqualFn> keyPatternMap;
  MapleUnorderedMap<std::string, GCTIBPattern*> symbolPatternMap;
};
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_AARCH64_AARCH64_CG_H */