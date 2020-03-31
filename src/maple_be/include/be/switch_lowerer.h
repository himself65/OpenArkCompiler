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
#ifndef MAPLEBE_INCLUDE_BE_SWITCH_LOWERER_H
#define MAPLEBE_INCLUDE_BE_SWITCH_LOWERER_H
#include "mir_nodes.h"
#include "mir_module.h"

namespace maplebe {
class BELowerer;

class SwitchLowerer {
 public:
  SwitchLowerer(maple::MIRModule &mod, maple::SwitchNode &stmt,
                maple::MapleAllocator &allocator)
      : mirModule(mod),
        stmt(&stmt),
        switchItems(allocator.Adapter()),
        ownAllocator(&allocator) {}

  ~SwitchLowerer() = default;

  maple::BlockNode *LowerSwitch();

 private:
  using Cluster = std::pair<maple::int32, maple::int32>;
  using SwitchItem = std::pair<maple::int32, maple::int32>;

  maple::MIRModule &mirModule;
  maple::SwitchNode *stmt;
  /*
   * the original switch table is sorted and then each dense (in terms of the
   * case tags) region is condensed into 1 switch item; in the switchItems
   * table, each item either corresponds to an original entry in the original
   * switch table (pair's second is 0), or to a dense region (pair's second
   * gives the upper limit of the dense range)
   */
  maple::MapleVector<SwitchItem> switchItems;  /* uint32 is index in switchTable */
  maple::MapleAllocator *ownAllocator;
  const maple::int32 kClusterSwitchCutoff = 6;
  const float kClusterSwitchDensity = 0.7;
  const maple::int32 kMaxRangeGotoTableSize = 127;
  bool jumpToDefaultBlockGenerated = false;

  void FindClusters(maple::MapleVector<Cluster> &clusters);
  void InitSwitchItems(maple::MapleVector<Cluster> &clusters);
  maple::RangeGotoNode *BuildRangeGotoNode(maple::int32 startIdx, maple::int32 endIdx);
  maple::CompareNode *BuildCmpNode(maple::Opcode opCode, maple::uint32 idx);
  maple::GotoNode *BuildGotoNode(maple::int32 idx);
  maple::CondGotoNode *BuildCondGotoNode(maple::int32 idx, maple::Opcode opCode, maple::BaseNode &cond);
  maple::BlockNode *BuildCodeForSwitchItems(maple::int32 start, maple::int32 end, bool lowBNdChecked,
                                            bool highBNdChecked);
};
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_BE_SWITCH_LOWERER_H */