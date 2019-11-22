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
#ifndef MAPLE_IR_INCLUDE_MIR_SYMBOL_BUILDER_H
#define MAPLE_IR_INCLUDE_MIR_SYMBOL_BUILDER_H
#include <string>
#include <utility>
#include <vector>
#include <map>
#include "opcodes.h"
#include "prim_types.h"
#include "mir_type.h"
#include "mir_const.h"
#include "mir_symbol.h"
#include "mir_nodes.h"
#include "mir_module.h"
#include "mir_preg.h"
#include "mir_function.h"
#include "printing.h"
#include "intrinsic_op.h"
#include "opcode_info.h"
#include "global_tables.h"

namespace maple {
class MIRSymbolBuilder {
 public:
  static MIRSymbolBuilder &Instance() {
    static MIRSymbolBuilder builder;
    return builder;
  }

  MIRSymbol *GetLocalDecl(MIRSymbolTable &symbolTable, GStrIdx strIdx) const;
  MIRSymbol *CreateLocalDecl(MIRSymbolTable &symbolTable, GStrIdx strIdx, const MIRType &type) const;
  MIRSymbol *GetGlobalDecl(GStrIdx strIdx) const;
  MIRSymbol *CreateGlobalDecl(GStrIdx strIdx, const MIRType &type, MIRStorageClass sc) const;
  MIRSymbol *GetSymbol(TyIdx tyIdx, GStrIdx strIdx, MIRSymKind mClass, MIRStorageClass sClass, uint8 scpID,
                       bool sameType = false) const;
  MIRSymbol *CreateSymbol(TyIdx tyIdx, GStrIdx strIdx, MIRSymKind mClass, MIRStorageClass sClass,
                          MIRFunction *func, uint8 scpID) const;
  MIRSymbol *CreatePregFormalSymbol(TyIdx tyIdx, PregIdx pRegIdx, MIRFunction &func) const;

 private:
  MIRSymbolBuilder() = default;
  ~MIRSymbolBuilder() = default;
  MIRSymbolBuilder(const MIRSymbolBuilder&) = delete;
  MIRSymbolBuilder(const MIRSymbolBuilder&&) = delete;
  MIRSymbolBuilder &operator=(const MIRSymbolBuilder&) = delete;
  MIRSymbolBuilder &operator=(const MIRSymbolBuilder&&) = delete;
};
} // maple

#endif // MAPLEIR_INCLUDE_MIRSYMBOLBUILDER_H
