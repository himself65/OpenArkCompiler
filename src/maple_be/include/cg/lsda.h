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
#ifndef MAPLEBE_INCLUDE_CG_LSDA_H
#define MAPLEBE_INCLUDE_CG_LSDA_H
#include "types_def.h"
#include "mir_nodes.h"
#include "cgbb.h"

namespace maplebe {
using namespace maple;

class LabelPair {
 public:
  LabelPair() = default;
  LabelPair(LabelNode *startOffsetLbl, LabelNode *endOffsetLbl) {
    startOffset = startOffsetLbl;
    endOffset = endOffsetLbl;
  }
  ~LabelPair() = default;

  const LabelNode *GetStartOffset() const {
    return startOffset;
  }

  void SetStartOffset(LabelNode *lableNode) {
    startOffset = lableNode;
  }

  const LabelNode *GetEndOffset() const {
    return endOffset;
  }

  void SetEndOffsetLabelIdx(LabelIdx index) {
    endOffset->SetLabelIdx(index);
  }

  void SetEndOffset(LabelNode *labelNode) {
    endOffset = labelNode;
  }

 private:
  LabelNode *startOffset;
  LabelNode *endOffset;
};

class LSDAHeader {
 public:
  const LabelNode *GetLSDALabel() const {
    return lsdaLabel;
  }

  void SetLSDALabel(LabelNode &labelNode) {
    lsdaLabel = &labelNode;
  }

  uint8 GetLPStartEncoding() const {
    return lpStartEncoding;
  }

  void SetLPStartEncoding(uint8 encoding) {
    lpStartEncoding = encoding;
  }

  uint8 GetTTypeEncoding() const {
    return tTypeEncoding;
  }

  void SetTTypeEncoding(uint8 encoding) {
    tTypeEncoding = encoding;
  }

  const LabelPair &GetTTypeOffset() const {
    return tTypeOffset;
  }

  void SetTTypeOffset(LabelNode *start, LabelNode *end) {
    tTypeOffset.SetStartOffset(start);
    tTypeOffset.SetEndOffset(end);
  }

  uint8 GetCallSiteEncoding() const {
    return callSiteEncoding;
  }

  void SetCallSiteEncoding(uint8 encoding) {
    callSiteEncoding = encoding;
  }

 private:
  LabelNode *lsdaLabel;
  uint8 lpStartEncoding;
  uint8 tTypeEncoding;
  LabelPair tTypeOffset;
  uint8 callSiteEncoding;
};

struct LSDACallSite {
  LabelPair csStart;
  LabelPair csLength;
  LabelPair csLandingPad;
  uint32 csAction;

 public:
  void Init(LabelPair &start, LabelPair &length, LabelPair &landingPad, uint32 action) {
    csStart = start;
    csLength = length;
    csLandingPad = landingPad;
    csAction = action;
  }
};

class LSDAAction {
 public:
  LSDAAction(uint8 idx, uint8 filter) : actionIndex(idx), actionFilter(filter) {}
  ~LSDAAction() = default;

  uint8 GetActionIndex() const {
    return actionIndex;
  }

  uint8 GetActionFilter() const {
    return actionFilter;
  }

 private:
  uint8 actionIndex;
  uint8 actionFilter;
};

class LSDACallSiteTable {
 public:
  explicit LSDACallSiteTable(MapleAllocator &alloc) : callSiteTable(alloc.Adapter()) {
    csTable.SetStartOffset(nullptr);
    csTable.SetEndOffset(nullptr);
  }
  ~LSDACallSiteTable() = default;

  const MapleVector<LSDACallSite*> &GetCallSiteTable() const {
    return callSiteTable;
  }

  void PushBack(LSDACallSite &lsdaCallSite) {
    callSiteTable.push_back(&lsdaCallSite);
  }

  const LabelPair &GetCSTable() const {
    return csTable;
  }

  void SetCSTable(LabelNode *start, LabelNode *end) {
    csTable.SetStartOffset(start);
    csTable.SetEndOffset(end);
  }

  void UpdateCallSite(const BB &oldBB, const BB &newBB) {
    for (auto *callSite : callSiteTable) {
      if (callSite->csStart.GetEndOffset() != nullptr) {
        if (callSite->csStart.GetEndOffset()->GetLabelIdx() == oldBB.GetLabIdx()) {
          callSite->csStart.SetEndOffsetLabelIdx(newBB.GetLabIdx());
        }
      }

      CHECK_NULL_FATAL(callSite->csLength.GetEndOffset());
      if (callSite->csLength.GetEndOffset()->GetLabelIdx() == oldBB.GetLabIdx()) {
        callSite->csLength.SetEndOffsetLabelIdx(newBB.GetLabIdx());
      }

      if (callSite->csLandingPad.GetEndOffset() != nullptr) {
        if (callSite->csLandingPad.GetEndOffset()->GetLabelIdx() == oldBB.GetLabIdx()) {
          callSite->csLandingPad.SetEndOffsetLabelIdx(newBB.GetLabIdx());
        }
      }
    }
  }

  /* return true if label is in callSiteTable */
  bool InCallSiteTable(LabelIdx label) const {
    for (auto *callSite : callSiteTable) {
      if (label == callSite->csStart.GetEndOffset()->GetLabelIdx() ||
          label == callSite->csStart.GetStartOffset()->GetLabelIdx()) {
        return true;
      }
      if (label == callSite->csLength.GetEndOffset()->GetLabelIdx() ||
          label == callSite->csLength.GetStartOffset()->GetLabelIdx()) {
        return true;
      }
      if (callSite->csLandingPad.GetStartOffset()) {
        if (label == callSite->csLandingPad.GetEndOffset()->GetLabelIdx() ||
            label == callSite->csLandingPad.GetStartOffset()->GetLabelIdx()) {
          return true;
        }
      }
    }
    return false;
  }

  bool IsTryBlock(const BB &bb) const {
    for (auto *callSite : callSiteTable) {
      if (callSite->csLength.GetStartOffset()->GetLabelIdx() == bb.GetLabIdx()) {
        return true;
      }
    }
    return false;
  }

  void SortCallSiteTable(std::function<bool(LSDACallSite *site1, LSDACallSite *site2)> const &func) {
    std::sort(callSiteTable.begin(), callSiteTable.end(), func);
  }

 private:
  MapleVector<LSDACallSite*> callSiteTable;
  LabelPair csTable;
};

class LSDAActionTable {
 public:
  explicit LSDAActionTable(MapleAllocator &alloc) : actionTable(alloc.Adapter()) {}
  virtual ~LSDAActionTable() = default;

  const MapleVector<LSDAAction*> &GetActionTable() const{
    return actionTable;
  }

  void PushBack(LSDAAction &lsdaAction) {
    actionTable.push_back(&lsdaAction);
  }

  size_t Size() const {
    return actionTable.size();
  }

 private:
  MapleVector<LSDAAction*> actionTable;
};
}  /* namespace maplebe */

#endif  /* MAPLEBE_INCLUDE_CG_LSDA_H */