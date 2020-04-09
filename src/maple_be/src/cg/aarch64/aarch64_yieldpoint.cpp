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
#include "aarch64_yieldpoint.h"
#include "aarch64_cgfunc.h"

namespace maplebe {
using namespace maple;

void AArch64YieldPointInsertion::Run() {
  InsertYieldPoint();
}

void AArch64YieldPointInsertion::InsertYieldPoint() {
  AArch64CGFunc *aarchCGFunc = static_cast<AArch64CGFunc*>(cgFunc);
  std::string refQueueName = "Ljava_2Flang_2Fref_2FReference_3B_7C_3Cinit_3E_7C_"
                             "28Ljava_2Flang_2FObject_3BLjava_2Flang_2Fref_2FReferenceQueue_3B_29V";
  if (!CGOptions::IsGCOnly() && (aarchCGFunc->GetName() == refQueueName)) {
    /* skip insert yieldpoint in reference constructor, avoid rc verify issue */
    ASSERT(aarchCGFunc->GetYieldPointInsn() != nullptr, "the entry yield point has been inserted");
    aarchCGFunc->GetYieldPointInsn()->GetBB()->RemoveInsn(*aarchCGFunc->GetYieldPointInsn());
    return;
  }

  /*
   * do not insert yieldpoint in function that not saved X30 into stack,
   * because X30 will be changed after yieldpoint is taken.
   */
  if (!aarchCGFunc->GetHasProEpilogue()) {
    ASSERT (aarchCGFunc->GetYieldPointInsn() != nullptr, "the entry yield point has been inserted");
    aarchCGFunc->GetYieldPointInsn()->GetBB()->RemoveInsn(*aarchCGFunc->GetYieldPointInsn());
    return;
  }
  /* skip if no GetFirstbb(). */
  if (aarchCGFunc->GetFirstBB() == nullptr) {
    return;
  }
  /*
   * The yield point in the entry of the GetFunction() is inserted just after the initialization
   * of localrefvars in HandleRCCall.
   * for BBs after firstbb.
   */
  for (BB *bb = aarchCGFunc->GetFirstBB()->GetNext(); bb != nullptr; bb = bb->GetNext()) {
    if (Globals::GetInstance()->GetOptimLevel() > 0) {
      /* insert a yieldpoint before the last jump instruction of a goto BB. */
      if (bb->IsBackEdgeDest()) {
        aarchCGFunc->GetDummyBB()->ClearInsns();
        aarchCGFunc->GenerateYieldpoint(*aarchCGFunc->GetDummyBB());
        bb->InsertAtBeginning(*aarchCGFunc->GetDummyBB());
        continue;
      }
    } else {
      /* when -O0, there is no backedge info available. */
      if ((bb->GetKind() == BB::kBBGoto) && (bb->GetLastInsn() != nullptr) && bb->GetLastInsn()->IsBranch()) {
        aarchCGFunc->GetDummyBB()->ClearInsns();
        aarchCGFunc->GenerateYieldpoint(*aarchCGFunc->GetDummyBB());
        bb->InsertAtBeginning(*aarchCGFunc->GetDummyBB());
        continue;
      }
      /*
       * insert yieldpoint for empty loop (CondGoto backward),
       * aka. last instruction jump to the top of the BB.
       */
      if (bb->GetLastInsn() != nullptr && bb->GetLastInsn()->IsBranch()) {
        /* the jump instruction. */
        Insn *jump = bb->GetLastInsn();
        /* skip if no jump target. */
        if (jump->GetOpndNum() == 0) {
          continue;
        }
        /* get the jump target operand. */
        Operand *op = jump->GetOpnd(jump->GetOpndNum() - 1);
        /* last operand not found or not a label operand. */
        if (op == nullptr || !op->IsLabel() || !op->IsLabelOpnd()) {
          continue;
        }
        /* the label operand of the jump instruction. */
        LabelOperand *label = static_cast<LabelOperand*>(op);
        if (label->GetLabelIndex() != bb->GetLabIdx()) {
          continue;
        }
        /* insert yieldpoint before jump instruction. */
        aarchCGFunc->GetDummyBB()->ClearInsns();
        aarchCGFunc->GenerateYieldpoint(*aarchCGFunc->GetDummyBB());
        bb->InsertAtBeginning(*aarchCGFunc->GetDummyBB());
      }
    }
  }
}
} /* namespace maplebe */
