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
#include "mir_parser.h"
#include "mir_function.h"
#include "opcode_info.h"

namespace maple {
std::map<TokenKind, MIRParser::FuncPtrParseExpr> MIRParser::funcPtrMapForParseExpr =
    MIRParser::InitFuncPtrMapForParseExpr();
std::map<TokenKind, MIRParser::FuncPtrParseStmt> MIRParser::funcPtrMapForParseStmt =
    MIRParser::InitFuncPtrMapForParseStmt();
std::map<TokenKind, MIRParser::FuncPtrParseStmtBlock> MIRParser::funcPtrMapForParseStmtBlock =
    MIRParser::InitFuncPtrMapForParseStmtBlock();

bool MIRParser::ParseStmtDassign(StmtNodePtr &stmt) {
  if (lexer.GetTokenKind() != TK_dassign) {
    Error("expect dassign but get ");
    return false;
  }
  // parse %i
  lexer.NextToken();
  StIdx stidx;
  if (!ParseDeclaredSt(stidx)) {
    return false;
  }
  if (stidx.FullIdx() == 0) {
    Error("expect a symbol parsing ParseStmtDassign");
    return false;
  }
  auto *assignStmt = mod.CurFuncCodeMemPool()->New<DassignNode>();
  assignStmt->SetStIdx(stidx);
  TokenKind nextToken = lexer.NextToken();
  // parse field id
  if (nextToken == TK_intconst) {  // may be a field id
    assignStmt->SetFieldID(lexer.GetTheIntVal());
    (void)lexer.NextToken();
  }
  // parse expression like (constval i32 0)
  BaseNode *expr = nullptr;
  if (!ParseExprOneOperand(expr)) {
    return false;
  }
  assignStmt->SetRHS(expr);
  stmt = assignStmt;
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseStmtRegassign(StmtNodePtr &stmt) {
  if (!IsPrimitiveType(lexer.NextToken())) {
    Error("expect type parsing binary operator but get ");
    return false;
  }
  auto *regAssign = mod.CurFuncCodeMemPool()->New<RegassignNode>();
  regAssign->SetPrimType(GetPrimitiveType(lexer.GetTokenKind()));
  lexer.NextToken();
  if (lexer.GetTokenKind() == TK_specialreg) {
    PregIdx tempPregIdx = regAssign->GetRegIdx();
    bool isSuccess = ParseSpecialReg(tempPregIdx);
    regAssign->SetRegIdx(tempPregIdx);
    if (!isSuccess) {
      return false;
    }
  } else if (lexer.GetTokenKind() == TK_preg) {
    PregIdx tempPregIdx = regAssign->GetRegIdx();
    bool isSuccess = ParsePseudoReg(regAssign->GetPrimType(), tempPregIdx);
    regAssign->SetRegIdx(tempPregIdx);
    if (!isSuccess) {
      return false;
    }
  } else {
    Error("expect special or pseudo register but get ");
    return false;
  }
  BaseNode *expr = nullptr;
  if (!ParseExprOneOperand(expr)) {
    return false;
  }
  regAssign->SetOpnd(expr);
  if (regAssign->GetRegIdx() > 0) {  // check type consistenency for the preg
    MIRPreg *preg = mod.CurFunction()->GetPregTab()->PregFromPregIdx(regAssign->GetRegIdx());
    if (preg->GetPrimType() == kPtyInvalid) {
      preg->SetPrimType(expr->GetPrimType());
    } else if (preg->GetPrimType() == PTY_dynany) {
      if (!IsPrimitiveDynType(expr->GetPrimType())) {
        Error("inconsistent preg primitive dynamic type at ");
        return false;
      }
    } else if (preg->GetPrimType() != expr->GetPrimType()) {
      if (!IsRefOrPtrAssign(preg->GetPrimType(), expr->GetPrimType()) &&
          !IsNoCvtNeeded(preg->GetPrimType(), expr->GetPrimType())) {
        Error("inconsistent preg primitive type or need a cvt ");
        return false;
      }
    }
  }
  stmt = regAssign;
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseStmtIassign(StmtNodePtr &stmt) {
  // iAssign <* [10] int> ()
  if (lexer.GetTokenKind() != TK_iassign) {
    Error("expect iassign but get ");
    return false;
  }
  // expect <> derived type
  lexer.NextToken();
  TyIdx tyIdx(0);
  if (!ParseDerivedType(tyIdx)) {
    Error("ParseStmtIassign failed when parsing derived type");
    return false;
  }
  auto *iAssign = mod.CurFuncCodeMemPool()->New<IassignNode>();
  iAssign->SetTyIdx(tyIdx);
  if (lexer.GetTokenKind() == TK_intconst) {
    iAssign->SetFieldID(lexer.theIntVal);
    lexer.NextToken();
  }
  BaseNode *addr = nullptr;
  BaseNode *rhs = nullptr;
  // parse 2 operands then, #1 is address, the other would be value
  if (!ParseExprTwoOperand(addr, rhs)) {
    return false;
  }
  iAssign->SetOpnd(addr, 0);
  iAssign->SetRHS(rhs);
  lexer.NextToken();
  stmt = iAssign;
  return true;
}

bool MIRParser::ParseStmtIassignoff(StmtNodePtr &stmt) {
  if (!IsPrimitiveType(lexer.NextToken())) {
    Error("expect type parsing binary operator but get ");
    return false;
  }
  // iassign <prim-type> <offset> ( <addr-expr>, <rhs-expr> )
  auto *iAssignOff = mod.CurFuncCodeMemPool()->New<IassignoffNode>();
  iAssignOff->SetPrimType(GetPrimitiveType(lexer.GetTokenKind()));
  if (lexer.NextToken() != TK_intconst) {
    Error("expect offset but get ");
    return false;
  }
  iAssignOff->SetOffset(lexer.GetTheIntVal());
  lexer.NextToken();
  BaseNode *addr = nullptr;
  BaseNode *rhs = nullptr;
  if (!ParseExprTwoOperand(addr, rhs)) {
    return false;
  }
  iAssignOff->SetBOpnd(addr, 0);
  iAssignOff->SetBOpnd(rhs, 1);
  lexer.NextToken();
  stmt = iAssignOff;
  return true;
}

bool MIRParser::ParseStmtIassignFPoff(StmtNodePtr &stmt) {
  if (!IsPrimitiveType(lexer.NextToken())) {
    Error("expect type parsing binary operator but get ");
    return false;
  }
  // iassignfpoff <prim-type> <offset> (<rhs-expr> )
  auto *iAssignOff = mod.CurFuncCodeMemPool()->New<IassignFPoffNode>();
  iAssignOff->SetPrimType(GetPrimitiveType(lexer.GetTokenKind()));
  if (lexer.NextToken() != TK_intconst) {
    Error("expect offset but get ");
    return false;
  }
  iAssignOff->SetOffset(lexer.GetTheIntVal());
  lexer.NextToken();
  BaseNode *expr = nullptr;
  if (!ParseExprOneOperand(expr)) {
    return false;
  }
  iAssignOff->SetOpnd(expr);
  lexer.NextToken();
  stmt = iAssignOff;
  return true;
}

bool MIRParser::ParseStmtDoloop(StmtNodePtr &stmt) {
  // syntax: doloop <do-var> (<start-expr>, <cont-expr>, <incr-amt>) {
  //              <body-stmts> }
  auto *doLoopNode = mod.CurFuncCodeMemPool()->New<DoloopNode>();
  stmt = doLoopNode;
  lexer.NextToken();
  if (lexer.GetTokenKind() == TK_preg) {
    PregIdx pregIdx = LookupOrCreatePregIdx(static_cast<uint32>(lexer.GetTheIntVal()), false, *mod.CurFunction());
    doLoopNode->SetIsPreg(true);
    doLoopNode->SetDoVarStFullIdx(pregIdx);
    // let other appearances handle the preg primitive type
  } else {
    StIdx stIdx;
    if (!ParseDeclaredSt(stIdx)) {
      return false;
    }
    if (stIdx.FullIdx() == 0) {
      Error("expect a symbol parsing ParseStmtDoloop");
      return false;
    }
    if (stIdx.IsGlobal()) {
      Error("expect local variable for doloop var but get ");
      return false;
    }
    doLoopNode->SetDoVarStIdx(stIdx);
  }
  // parse (
  if (lexer.NextToken() != TK_lparen) {
    Error("expect ( but get ");
    return false;
  }
  // parse start expression
  lexer.NextToken();
  BaseNode *start = nullptr;
  if (!ParseExpression(start)) {
    Error("ParseStmtDoloop when parsing start expression");
    return false;
  }
  if (doLoopNode->IsPreg()) {
    auto regIdx = static_cast<PregIdx>(doLoopNode->GetDoVarStIdx().FullIdx());
    MIRPreg *mpReg = mod.CurFunction()->GetPregTab()->PregFromPregIdx(regIdx);
    if (mpReg->GetPrimType() == kPtyInvalid) {
      CHECK_FATAL(start != nullptr, "null ptr check");
      mpReg->SetPrimType(start->GetPrimType());
    }
  }
  if (lexer.GetTokenKind() != TK_coma) {
    Error("expect , after start expression but get ");
    return false;
  }
  doLoopNode->SetStartExpr(start);
  // parse end expression
  lexer.NextToken();
  BaseNode *end = nullptr;
  if (!ParseExpression(end)) {  // here should be a compare expression
    Error("ParseStmtDoloop when parsing end expression");
    return false;
  }
  if (lexer.GetTokenKind() != TK_coma) {
    Error("expect , after condition expression but get ");
    return false;
  }
  doLoopNode->SetContExpr(end);
  // parse renew induction expression
  lexer.NextToken();
  BaseNode *induction = nullptr;
  if (!ParseExpression(induction)) {
    Error("ParseStmtDoloop when parsing induction");
    return false;
  }
  // parse )
  if (lexer.GetTokenKind() != TK_rparen) {
    Error("expect ) parsing doloop but get ");
    return false;
  }
  doLoopNode->SetIncrExpr(induction);
  // parse body of the loop
  lexer.NextToken();
  BlockNode *bodyStmt = nullptr;
  if (!ParseStmtBlock(bodyStmt)) {
    Error("ParseStmtDoloop when parsing body of the loop");
    return false;
  }
  doLoopNode->SetDoBody(bodyStmt);
  return true;
}

bool MIRParser::ParseStmtForeachelem(StmtNodePtr &stmt) {
  // syntax: foreachelem <elemvar> <arrayvar> {
  //              <body-stmts> }
  auto *forNode = mod.CurFuncCodeMemPool()->New<ForeachelemNode>();
  stmt = forNode;
  lexer.NextToken();  // skip foreachelem token
  StIdx stidx;
  if (!ParseDeclaredSt(stidx)) {
    return false;
  }
  if (stidx.FullIdx() == 0) {
    Error("error parsing element variable of foreachelem in ");
    return false;
  }
  if (stidx.IsGlobal()) {
    Error("illegal global scope for element variable for foreachelem in ");
    return false;
  }
  forNode->SetElemStIdx(stidx);
  lexer.NextToken();
  if (!ParseDeclaredSt(stidx)) {
    return false;
  }
  if (stidx.FullIdx() == 0) {
    Error("error parsing array/collection variable of foreachelem in ");
    return false;
  }
  forNode->SetArrayStIdx(stidx);
  lexer.NextToken();
  // parse body of the loop
  BlockNode *bodyStmt = nullptr;
  if (!ParseStmtBlock(bodyStmt)) {
    Error("error when parsing body of foreachelem loop in ");
    return false;
  }
  forNode->SetLoopBody(bodyStmt);
  return true;
}

bool MIRParser::ParseStmtIf(StmtNodePtr &stmt) {
  if (lexer.GetTokenKind() != TK_if) {
    Error("expect if but get ");
    return false;
  }
  auto *ifStmt = mod.CurFuncCodeMemPool()->New<IfStmtNode>();
  lexer.NextToken();
  BaseNode *expr = nullptr;
  if (!ParseExprOneOperand(expr)) {
    return false;
  }
  ifStmt->SetOpnd(expr);
  if (lexer.NextToken() != TK_lbrace) {
    Error("expect { begin if body but get ");
    return false;
  }
  BlockNode *thenBlock = nullptr;
  if (!ParseStmtBlock(thenBlock)) {
    Error("ParseStmtIf failed when parsing then block");
    return false;
  }
  ifStmt->SetThenPart(thenBlock);
  BlockNode *elseBlock = nullptr;
  if (lexer.GetTokenKind() == TK_else) {
    // has else part
    if (lexer.NextToken() != TK_lbrace) {
      Error("expect { begin if body but get ");
      return false;
    }
    if (!ParseStmtBlock(elseBlock)) {
      Error("ParseStmtIf failed when parsing else block");
      return false;
    }
    ifStmt->SetElsePart(elseBlock);
    if (elseBlock != nullptr) {
      ifStmt->SetNumOpnds(ifStmt->GetNumOpnds() + 1);
    }
  }
  stmt = ifStmt;
  return true;
}

bool MIRParser::ParseStmtWhile(StmtNodePtr &stmt) {
  if (lexer.GetTokenKind() != TK_while) {
    Error("expect while but get ");
    return false;
  }
  auto *whileStmt = mod.CurFuncCodeMemPool()->New<WhileStmtNode>(OP_while);
  lexer.NextToken();
  BaseNode *expr = nullptr;
  if (!ParseExprOneOperand(expr)) {
    return false;
  }
  whileStmt->SetOpnd(expr);
  if (lexer.NextToken() != TK_lbrace) {
    Error("expect { begin if body but get ");
    return false;
  }
  BlockNode *whileBody = nullptr;
  if (!ParseStmtBlock(whileBody)) {
    Error("ParseStmtWhile failed when parse while body");
    return false;
  }
  whileStmt->SetBody(whileBody);
  stmt = whileStmt;
  return true;
}

bool MIRParser::ParseStmtDowhile(StmtNodePtr &stmt) {
  if (lexer.GetTokenKind() != TK_dowhile) {
    Error("expect while but get ");
    return false;
  }
  auto *whileStmt = mod.CurFuncCodeMemPool()->New<WhileStmtNode>(OP_dowhile);
  if (lexer.NextToken() != TK_lbrace) {
    Error("expect { begin if body but get ");
    return false;
  }
  BlockNode *doWhileBody = nullptr;
  if (!ParseStmtBlock(doWhileBody)) {
    Error("ParseStmtDowhile failed when trying to parsing do while body");
    return false;
  }
  whileStmt->SetBody(doWhileBody);
  BaseNode *expr = nullptr;
  if (!ParseExprOneOperand(expr)) {
    return false;
  }
  whileStmt->SetOpnd(expr);
  lexer.NextToken();
  stmt = whileStmt;
  return true;
}

bool MIRParser::ParseStmtLabel(StmtNodePtr &stmt) {
  GStrIdx strIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(lexer.GetName());
  LabelIdx labIdx = mod.CurFunction()->GetLabelTab()->GetStIdxFromStrIdx(strIdx);
  if (labIdx == 0) {
    labIdx = mod.CurFunction()->GetLabelTab()->CreateLabel();
    mod.CurFunction()->GetLabelTab()->SetSymbolFromStIdx(labIdx, strIdx);
    mod.CurFunction()->GetLabelTab()->AddToStringLabelMap(labIdx);
  } else {
    if (definedLabels.size() > labIdx && definedLabels[labIdx]) {
      Error("label multiply declared ");
      return false;
    }
  }
  if (definedLabels.size() <= labIdx) {
    definedLabels.resize(labIdx + 1);
  }
  definedLabels[labIdx] = true;
  auto *labNode = mod.CurFuncCodeMemPool()->New<LabelNode>();
  labNode->SetLabelIdx(labIdx);
  stmt = labNode;
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseStmtGoto(StmtNodePtr &stmt) {
  if (lexer.GetTokenKind() != TK_goto) {
    Error("expect goto but get ");
    return false;
  }
  if (lexer.NextToken() != TK_label) {
    Error("expect label in goto but get ");
    return false;
  }
  GStrIdx strIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(lexer.GetName());
  LabelIdx labIdx = mod.CurFunction()->GetLabelTab()->GetStIdxFromStrIdx(strIdx);
  if (labIdx == 0) {
    labIdx = mod.CurFunction()->GetLabelTab()->CreateLabel();
    mod.CurFunction()->GetLabelTab()->SetSymbolFromStIdx(labIdx, strIdx);
    mod.CurFunction()->GetLabelTab()->AddToStringLabelMap(labIdx);
  }
  auto *gotoNode = mod.CurFuncCodeMemPool()->New<GotoNode>(OP_goto);
  gotoNode->SetOffset(labIdx);
  stmt = gotoNode;
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseStmtBr(StmtNodePtr &stmt) {
  TokenKind tk = lexer.GetTokenKind();
  if (tk != TK_brtrue && tk != TK_brfalse) {
    Error("expect brtrue/brfalse but get ");
    return false;
  }
  if (lexer.NextToken() != TK_label) {
    Error("expect label in goto but get ");
    return false;
  }
  GStrIdx strIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(lexer.GetName());
  LabelIdx labIdx = mod.CurFunction()->GetLabelTab()->GetStIdxFromStrIdx(strIdx);
  if (labIdx == 0) {
    labIdx = mod.CurFunction()->GetLabelTab()->CreateLabel();
    mod.CurFunction()->GetLabelTab()->SetSymbolFromStIdx(labIdx, strIdx);
    mod.CurFunction()->GetLabelTab()->AddToStringLabelMap(labIdx);
  }
  auto *condGoto = mod.CurFuncCodeMemPool()->New<CondGotoNode>(tk == TK_brtrue ? OP_brtrue : OP_brfalse);
  condGoto->SetOffset(labIdx);
  lexer.NextToken();
  // parse (<expr>)
  BaseNode *expr = nullptr;
  if (!ParseExprOneOperand(expr)) {
    return false;
  }
  condGoto->SetOpnd(expr);
  stmt = condGoto;
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseSwitchCase(int32 &constVal, LabelIdx &lblIdx) {
  // syntax <intconst0>: goto <label0>
  if (lexer.GetTokenKind() != TK_intconst) {
    Error("expect intconst in switch but get ");
    return false;
  }
  constVal = lexer.GetTheIntVal();
  if (lexer.NextToken() != TK_colon) {
    Error("expect : in switch but get ");
    return false;
  }
  if (lexer.NextToken() != TK_goto) {
    Error("expect goto in switch case but get ");
    return false;
  }
  if (lexer.NextToken() != TK_label) {
    Error("expect label in switch but get ");
    return false;
  }
  lblIdx = mod.CurFunction()->GetOrCreateLableIdxFromName(lexer.GetName());
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseStmtSwitch(StmtNodePtr &stmt) {
  auto *switchNode = mod.CurFuncCodeMemPool()->New<SwitchNode>(mod);
  stmt = switchNode;
  lexer.NextToken();
  BaseNode *expr = nullptr;
  if (!ParseExprOneOperand(expr)) {
    return false;
  }
  switchNode->SetSwitchOpnd(expr);
  if (!IsPrimitiveInteger(expr->GetPrimType())) {
    Error("expect expression return integer but get ");
    return false;
  }
  if (lexer.NextToken() == TK_label) {
    switchNode->SetDefaultLabel(mod.CurFunction()->GetOrCreateLableIdxFromName(lexer.GetName()));
  } else {
    Error("expect label in switch but get ");
    return false;
  }
  if (lexer.NextToken() != TK_lbrace) {
    Error("expect { in switch but get ");
    return false;
  }
  // <intconst0>: goto <label0>
  // <intconst1>: goto <label1>
  // ...
  // <intconstn>: goto <labeln>
  TokenKind tk = lexer.NextToken();
  std::set<int32> casesSet;
  while (tk != TK_rbrace) {
    int32 constVal = 0;
    LabelIdx lbl = 0;
    if (!ParseSwitchCase(constVal, lbl)) {
      Error("parse switch case failed ");
      return false;
    }
    if (casesSet.find(constVal) != casesSet.end()) {
      Error("duplicated switch case ");
      return false;
    }
    switchNode->InsertCasePair(CasePair(constVal, lbl));
    casesSet.insert(constVal);
    tk = lexer.GetTokenKind();
  }
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseStmtRangegoto(StmtNodePtr &stmt) {
  auto *rangeGotoNode = mod.CurFuncCodeMemPool()->New<RangeGotoNode>(mod);
  stmt = rangeGotoNode;
  lexer.NextToken();
  BaseNode *expr = nullptr;
  if (!ParseExprOneOperand(expr)) {
    return false;
  }
  if (!IsPrimitiveInteger(expr->GetPrimType())) {
    rangeGotoNode->SetOpnd(expr);
    Error("expect expression return integer but get ");
    return false;
  }
  if (lexer.NextToken() == TK_intconst) {
    rangeGotoNode->SetTagOffset(lexer.GetTheIntVal());
  } else {
    Error("expect tag offset in rangegoto but get ");
    return false;
  }
  if (lexer.NextToken() != TK_lbrace) {
    Error("expect { in switch but get ");
    return false;
  }
  // <intconst0>: goto <label0>
  // <intconst1>: goto <label1>
  // ...
  // <intconstn>: goto <labeln>
  TokenKind tk = lexer.NextToken();
  std::set<uint16> casesSet;
  int32 minIdx = UINT16_MAX;
  int32 maxIdx = 0;
  while (tk != TK_rbrace) {
    int32 constVal = 0;
    LabelIdx lbl = 0;
    if (!ParseSwitchCase(constVal, lbl)) {
      Error("parse switch case failed ");
      return false;
    }
    if (constVal > UINT16_MAX || constVal < 0) {
      Error("rangegoto case tag not within unsigned 16 bits range ");
      return false;
    }
    if (casesSet.find(constVal) != casesSet.end()) {
      Error("duplicated switch case ");
      return false;
    }
    if (constVal < minIdx) {
      minIdx = constVal;
    }
    if (constVal > maxIdx) {
      maxIdx = constVal;
    }
    rangeGotoNode->AddRangeGoto(static_cast<uint32>(constVal), static_cast<uint32>(lbl));
    casesSet.insert(constVal);
    tk = lexer.GetTokenKind();
  }
  ASSERT(rangeGotoNode->GetNumOpnds() == 1, "Rangegoto is a UnaryOpnd; numOpnds must be 1");
  // check there is no gap
  if (static_cast<size_t>(static_cast<uint32>(maxIdx - minIdx) + 1) != casesSet.size()) {
    Error("gap not allowed in rangegoto case tags ");
    return false;
  }
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseStmtMultiway(StmtNodePtr &stmt) {
  auto *multiwayNode = mod.CurFuncCodeMemPool()->New<MultiwayNode>(mod);
  stmt = multiwayNode;
  lexer.NextToken();
  BaseNode *expr = nullptr;
  if (!ParseExprOneOperand(expr)) {
    return false;
  }
  multiwayNode->SetMultiWayOpnd(expr);
  if (lexer.NextToken() == TK_label) {
    multiwayNode->SetDefaultlabel(mod.CurFunction()->GetOrCreateLableIdxFromName(lexer.GetName()));
  } else {
    Error("expect label in multiway but get ");
    return false;
  }
  if (lexer.NextToken() != TK_lbrace) {
    Error("expect { in switch but get ");
    return false;
  }
  // (<expr0>): goto <label0>
  // (<expr1>): goto <label1>
  // ...
  // (<exprn>): goto <labeln>
  TokenKind tk = lexer.NextToken();
  while (tk != TK_rbrace) {
    BaseNode *x = nullptr;
    if (!ParseExprOneOperand(x)) {
      return false;
    }
    if (lexer.NextToken() != TK_colon) {
      Error("expect : parsing multiway case tag specification but get ");
      return false;
    }
    if (lexer.NextToken() != TK_goto) {
      Error("expect goto in multiway case expression but get ");
      return false;
    }
    if (lexer.NextToken() != TK_label) {
      Error("expect goto label after multiway case expression but get ");
      return false;
    }
    LabelIdx lblIdx = mod.CurFunction()->GetOrCreateLableIdxFromName(lexer.GetName());
    lexer.NextToken();
    multiwayNode->AppendElemToMultiWayTable(MCasePair(static_cast<BaseNode*>(x), lblIdx));
    tk = lexer.GetTokenKind();
  }
  const MapleVector<MCasePair> &multiWayTable = multiwayNode->GetMultiWayTable();
  multiwayNode->SetNumOpnds(multiWayTable.size());
  lexer.NextToken();
  return true;
}

// used only when parsing mmpl
PUIdx MIRParser::EnterUndeclaredFunction(bool isMcount) {
  std::string funcName;
  if (isMcount) {
    funcName = "_mcount";
  } else {
    funcName = lexer.GetName();
  }
  GStrIdx strIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(funcName);
  MIRSymbol *funcSt = GlobalTables::GetGsymTable().CreateSymbol(kScopeGlobal);
  funcSt->SetNameStrIdx(strIdx);
  (void)GlobalTables::GetGsymTable().AddToStringSymbolMap(*funcSt);
  funcSt->SetStorageClass(kScText);
  funcSt->SetSKind(kStFunc);
  auto *fn = mod.GetMemPool()->New<MIRFunction>(&mod, funcSt->GetStIdx());
  fn->Init();
  fn->SetPuidx(GlobalTables::GetFunctionTable().GetFuncTable().size());
  GlobalTables::GetFunctionTable().GetFuncTable().push_back(fn);
  funcSt->SetFunction(fn);
  auto *funcType = mod.GetMemPool()->New<MIRFuncType>();
  fn->SetMIRFuncType(funcType);
  if (isMcount) {
    MIRType *retType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(TyIdx(PTY_void));
    fn->SetReturnTyIdx(retType->GetTypeIndex());
  }
  return fn->GetPuidx();
}

bool MIRParser::ParseStmtCallMcount(StmtNodePtr &stmt) {
  // syntax: call <PU-name> (<opnd0>, ..., <opndn>)
  Opcode o = OP_call;
  PUIdx pIdx = EnterUndeclaredFunction(true);
  auto *callStmt = mod.CurFuncCodeMemPool()->New<CallNode>(mod, o);
  callStmt->SetPUIdx(pIdx);
  MapleVector<BaseNode*> opndsvec(mod.CurFuncCodeMemPoolAllocator()->Adapter());
  callStmt->SetNOpnd(opndsvec);
  callStmt->SetNumOpnds(opndsvec.size());
  stmt = callStmt;
  return true;
}

bool MIRParser::ParseStmtCall(StmtNodePtr &stmt) {
  // syntax: call <PU-name> (<opnd0>, ..., <opndn>)
  TokenKind tk = lexer.GetTokenKind();
  Opcode o = GetOpFromToken(tk);
  ASSERT(kOpcodeInfo.IsCall(o), "ParseStmtCall: not a call opcode");
  bool hasAssigned = kOpcodeInfo.IsCallAssigned(o);
  bool hasInstant = false;
  bool withType = false;
  switch (tk) {
    case TK_polymorphiccall:
    case TK_polymorphiccallassigned:
      withType = true;
      break;
    case TK_callinstant:
    case TK_virtualcallinstant:
    case TK_superclasscallinstant:
    case TK_interfacecallinstant:
    case TK_callinstantassigned:
    case TK_virtualcallinstantassigned:
    case TK_superclasscallinstantassigned:
    case TK_interfacecallinstantassigned:
      hasInstant = true;
      break;
    default:
      break;
  }
  TyIdx polymophicTyidx(0);
  if (o == OP_polymorphiccallassigned || o == OP_polymorphiccall) {
    TokenKind nextTk = lexer.NextToken();
    if (nextTk == TK_langle) {
      nextTk = lexer.NextToken();
      if (nextTk == TK_func) {
        lexer.NextToken();
        if (!ParseFuncType(polymophicTyidx)) {
          Error("error parsing functype in ParseStmtCall for polymorphiccallassigned at ");
          return false;
        }
      } else {
        Error("expect func in functype but get ");
        return false;
      }
    } else {
      Error("expect < in functype but get ");
      return false;
    }
  }
  TokenKind funcTk = lexer.NextToken();
  if (funcTk != TK_fname) {
    Error("expect func name in call but get ");
    return false;
  }
  PUIdx pIdx;
  if (!ParseDeclaredFunc(pIdx)) {
    if (mod.GetFlavor() < kMmpl) {
      Error("expect .mmpl");
      return false;
    }
    pIdx = EnterUndeclaredFunction();
  }
  lexer.NextToken();
  CallNode *callStmt = nullptr;
  CallinstantNode *callInstantStmt = nullptr;
  if (withType) {
    callStmt = mod.CurFuncCodeMemPool()->New<CallNode>(mod, o);
    callStmt->SetTyIdx(polymophicTyidx);
  } else if (hasInstant) {
    TokenKind langleTk = lexer.GetTokenKind();
    if (langleTk != TK_langle) {
      Error("missing < in generic method instantiation at ");
      return false;
    }
    TokenKind lbraceTk = lexer.NextToken();
    if (lbraceTk != TK_lbrace) {
      Error("missing { in generic method instantiation at ");
      return false;
    }
    MIRInstantVectorType instVecTy;
    if (!ParseGenericInstantVector(instVecTy)) {
      Error("error parsing generic method instantiation at ");
      return false;
    }
    TokenKind rangleTk = lexer.GetTokenKind();
    if (rangleTk != TK_rangle) {
      Error("missing > in generic method instantiation at ");
      return false;
    }
    TyIdx tyIdx = GlobalTables::GetTypeTable().GetOrCreateMIRType(&instVecTy);
    callInstantStmt = mod.CurFuncCodeMemPool()->New<CallinstantNode>(mod, o, tyIdx);
    callStmt = callInstantStmt;
    lexer.NextToken();  // skip the >
  } else {
    callStmt = mod.CurFuncCodeMemPool()->New<CallNode>(mod, o);
  }
  callStmt->SetPUIdx(pIdx);
  MapleVector<BaseNode*> opndsVec(mod.CurFuncCodeMemPoolAllocator()->Adapter());
  if (!ParseExprNaryOperand(opndsVec)) {
    return false;
  }
  callStmt->SetNOpnd(opndsVec);
  callStmt->SetNumOpnds(opndsVec.size());
  if (hasAssigned) {
    CallReturnVector retsVec(mod.CurFuncCodeMemPoolAllocator()->Adapter());
    if (!ParseCallReturns(retsVec)) {
      return false;
    }
    if (!hasInstant) {
      ASSERT(callStmt != nullptr, "callstmt is null in MIRParser::ParseStmtCall");
      callStmt->SetReturnVec(retsVec);
    } else {
      ASSERT(callInstantStmt != nullptr, "callinstantstmt is null in MIRParser::ParseStmtCall");
      callInstantStmt->SetReturnVec(retsVec);
    }
  }
  lexer.NextToken();
  stmt = callStmt;
  return true;
}

bool MIRParser::ParseStmtIcall(StmtNodePtr &stmt, bool isAssigned) {
  // syntax: icall (<PU-ptr>, <opnd0>, ..., <opndn>)
  //         icallassigned <PU-ptr> (<opnd0>, ..., <opndn>) {
  //              dassign <var-name0> <field-id0>
  //              dassign <var-name1> <field-id1>
  //               . . .
  //              dassign <var-namen> <field-idn> }
  auto *iCallStmt = mod.CurFuncCodeMemPool()->New<IcallNode>(mod, !isAssigned ? OP_icall : OP_icallassigned);
  lexer.NextToken();
  MapleVector<BaseNode*> opndsVec(mod.CurFuncCodeMemPoolAllocator()->Adapter());
  if (!ParseExprNaryOperand(opndsVec)) {
    return false;
  }
  iCallStmt->SetNOpnd(opndsVec);
  iCallStmt->SetNumOpnds(opndsVec.size());
  if (isAssigned) {
    CallReturnVector retsVec(mod.CurFuncCodeMemPoolAllocator()->Adapter());
    if (!ParseCallReturns(retsVec)) {
      return false;
    }
    iCallStmt->SetReturnVec(retsVec);
  }
  lexer.NextToken();
  stmt = iCallStmt;
  return true;
}

bool MIRParser::ParseStmtIcall(StmtNodePtr &stmt) {
  return ParseStmtIcall(stmt, false);
}

bool MIRParser::ParseStmtIcallassigned(StmtNodePtr &stmt) {
  return ParseStmtIcall(stmt, true);
}

bool MIRParser::ParseStmtIntrinsiccall(StmtNodePtr &stmt, bool isAssigned) {
  Opcode o = !isAssigned ? (lexer.GetTokenKind() == TK_intrinsiccall ? OP_intrinsiccall : OP_xintrinsiccall)
                         : (lexer.GetTokenKind() == TK_intrinsiccallassigned ? OP_intrinsiccallassigned
                                                                             : OP_xintrinsiccallassigned);
  auto *intrnCallNode = mod.CurFuncCodeMemPool()->New<IntrinsiccallNode>(mod, o);
  lexer.NextToken();
  if (o == !isAssigned ? OP_intrinsiccall : OP_intrinsiccallassigned) {
    intrnCallNode->SetIntrinsic(GetIntrinsicID(lexer.GetTokenKind()));
  } else {
    intrnCallNode->SetIntrinsic(static_cast<MIRIntrinsicID>(lexer.GetTheIntVal()));
  }
  lexer.NextToken();
  MapleVector<BaseNode*> opndsVec(mod.CurFuncCodeMemPoolAllocator()->Adapter());
  if (!ParseExprNaryOperand(opndsVec)) {
    return false;
  }
  intrnCallNode->SetNOpnd(opndsVec);
  intrnCallNode->SetNumOpnds(opndsVec.size());
  if (isAssigned) {
    CallReturnVector retsVec(mod.CurFuncCodeMemPoolAllocator()->Adapter());
    if (!ParseCallReturns(retsVec)) {
      return false;
    }
    // store return type of IntrinsiccallNode
    if (retsVec.size() == 1 && retsVec[0].first.Idx() != 0) {
      MIRSymbol *retSymbol = curFunc->GetSymTab()->GetSymbolFromStIdx(retsVec[0].first.Idx());
      MIRType *retType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(retSymbol->GetTyIdx());
      CHECK_FATAL(retType != nullptr, "rettype is null in MIRParser::ParseStmtIntrinsiccallAssigned");
      intrnCallNode->SetPrimType(retType->GetPrimType());
    }
    intrnCallNode->SetReturnVec(retsVec);
  }
  stmt = intrnCallNode;
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseStmtIntrinsiccall(StmtNodePtr &stmt) {
  return ParseStmtIntrinsiccall(stmt, false);
}

bool MIRParser::ParseStmtIntrinsiccallassigned(StmtNodePtr &stmt) {
  return ParseStmtIntrinsiccall(stmt, true);
}

bool MIRParser::ParseStmtIntrinsiccallwithtype(StmtNodePtr &stmt, bool isAssigned) {
  Opcode o = (!isAssigned) ? OP_intrinsiccallwithtype : OP_intrinsiccallwithtypeassigned;
  IntrinsiccallNode *intrnCallNode = mod.CurFuncCodeMemPool()->New<IntrinsiccallNode>(mod, o);
  TokenKind tk = lexer.NextToken();
  TyIdx tyIdx(0);
  if (IsPrimitiveType(tk)) {
    if (!ParsePrimType(tyIdx)) {
      Error("expect primitive type in ParseStmtIntrinsiccallwithtype but get ");
      return false;
    }
  } else if (!ParseDerivedType(tyIdx)) {
    Error("error parsing type in ParseStmtIntrinsiccallwithtype at ");
    return false;
  }
  intrnCallNode->SetTyIdx(tyIdx);
  intrnCallNode->SetIntrinsic(GetIntrinsicID(lexer.GetTokenKind()));
  lexer.NextToken();
  MapleVector<BaseNode*> opndsVec(mod.CurFuncCodeMemPoolAllocator()->Adapter());
  if (!ParseExprNaryOperand(opndsVec)) {
    return false;
  }
  intrnCallNode->SetNOpnd(opndsVec);
  intrnCallNode->SetNumOpnds(opndsVec.size());
  if (isAssigned) {
    CallReturnVector retsVec(mod.CurFuncCodeMemPoolAllocator()->Adapter());
    if (!ParseCallReturns(retsVec)) {
      return false;
    }
    // store return type of IntrinsiccallNode
    if (retsVec.size() == 1 && retsVec[0].first.Idx() != 0) {
      MIRSymbol *retSymbol = curFunc->GetSymTab()->GetSymbolFromStIdx(retsVec[0].first.Idx());
      MIRType *retType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(retSymbol->GetTyIdx());
      CHECK_FATAL(retType != nullptr, "rettype is null in MIRParser::ParseStmtIntrinsiccallwithtypeAssigned");
      intrnCallNode->SetPrimType(retType->GetPrimType());
    }
    intrnCallNode->SetReturnVec(retsVec);
  }
  stmt = intrnCallNode;
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseStmtIntrinsiccallwithtype(StmtNodePtr &stmt) {
  return ParseStmtIntrinsiccallwithtype(stmt, false);
}

bool MIRParser::ParseStmtIntrinsiccallwithtypeassigned(StmtNodePtr &stmt) {
  return ParseStmtIntrinsiccallwithtype(stmt, true);
}

bool MIRParser::ParseCallReturns(CallReturnVector &retsvec) {
  //             {
  //              dassign <var-name0> <field-id0>
  //              dassign <var-name1> <field-id1>
  //               . . .
  //              dassign <var-namen> <field-idn> }
  //              OR
  //             {
  //               regassign <type> <reg1>
  //               regassign <type> <reg2>
  //               regassign <type> <reg3>
  //             }
  if (lexer.NextToken() != TK_lbrace) {
    Error("expect { parsing call return values. ");
    return false;
  }
  TokenKind tk = lexer.NextToken();
  while (tk != TK_rbrace) {
    if (lexer.GetTokenKind() != TK_dassign && lexer.GetTokenKind() != TK_regassign) {
      Error("expect dassign/regassign but get ");
      return false;
    }
    bool isst = (lexer.GetTokenKind() == TK_dassign);
    if (isst) {
      // parse %i
      lexer.NextToken();
      StIdx stidx;
      // How to use islocal??
      if (!ParseDeclaredSt(stidx)) {
        return false;
      }
      if (lexer.GetTokenKind() == TK_lname) {
        MIRSymbolTable *lSymTab = mod.CurFunction()->GetSymTab();
        MIRSymbol *lSym = lSymTab->GetSymbolFromStIdx(stidx.Idx(), 0);
        ASSERT(lSym != nullptr, "lsym MIRSymbol is null");
        if (lSym->GetName().find("L_STR") == 0) {
          MIRType *ty = GlobalTables::GetTypeTable().GetTypeFromTyIdx(lSym->GetTyIdx());
          auto *ptrTy = static_cast<MIRPtrType*>(ty->CopyMIRTypeNode());
          ASSERT(ptrTy != nullptr, "null ptr check");
          ptrTy->SetPrimType(PTY_ptr);
          TyIdx newTyidx = GlobalTables::GetTypeTable().GetOrCreateMIRType(ptrTy);
          delete ptrTy;
          lSym->SetTyIdx(newTyidx);
        }
      }
      if (stidx.FullIdx() == 0) {
        Error("expect a symbol parsing ParseCallAssignedStmts. ");
        return false;
      }
      uint16 fieldId = 0;
      TokenKind nextToken = lexer.NextToken();
      // parse field id
      if (nextToken == TK_intconst) {
        fieldId = lexer.GetTheIntVal();
      } else {
        Error("expect a fieldID parsing ParseCallAssignedStmts. ");
      }
      RegFieldPair regFieldPair;
      regFieldPair.SetFieldID(fieldId);
      retsvec.push_back(CallReturnPair(stidx, regFieldPair));
      tk = lexer.NextToken();
    } else {
      // parse type
      lexer.NextToken();
      TyIdx tyidx(0);
      // RegreadNode regreadexpr;
      bool ret = ParsePrimType(tyidx);
      if (ret != true) {
        Error("call ParsePrimType failed in ParseCallReturns");
        return false;
      }
      if (tyidx == 0) {
        Error("expect primitive type but get ");
        return false;
      }
      PrimType ptype = GlobalTables::GetTypeTable().GetPrimTypeFromTyIdx(tyidx);
      PregIdx pregIdx;
      if (lexer.GetTokenKind() == TK_specialreg) {
        if (!ParseSpecialReg(pregIdx)) {
          Error("expect specialreg parsing callassign CallReturnVector");
          return false;
        }
      } else if (lexer.GetTokenKind() == TK_preg) {
        if (!ParsePseudoReg(ptype, pregIdx)) {
          Error("expect pseudoreg parsing callassign CallReturnVector");
          return false;
        }
      } else {
        Error("expect special or pseudo register but get ");
        return false;
      }
      ASSERT(pregIdx > 0, "register number is zero");
      ASSERT(pregIdx <= 0xffff, "register number is over 16 bits");
      RegFieldPair regFieldPair;
      regFieldPair.SetPregIdx(pregIdx);
      retsvec.push_back(CallReturnPair(StIdx(), regFieldPair));
      tk = lexer.GetTokenKind();
    }
  }
  return true;
}

bool MIRParser::ParseStmtJsTry(StmtNodePtr &stmt) {
  auto *tryNode = mod.CurFuncCodeMemPool()->New<JsTryNode>();
  lexer.NextToken();
  // parse handler label
  if (lexer.GetTokenKind() == TK_intconst && lexer.GetTheIntVal() == 0) {
    tryNode->SetCatchOffset(0);
  } else {
    if (lexer.GetTokenKind() != TK_label) {
      Error("expect handler label in try but get ");
      return false;
    }
    GStrIdx stridx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(lexer.GetName());
    LabelIdx labidx = mod.CurFunction()->GetLabelTab()->GetStIdxFromStrIdx(stridx);
    if (labidx == 0) {
      labidx = mod.CurFunction()->GetLabelTab()->CreateLabel();
      mod.CurFunction()->GetLabelTab()->SetSymbolFromStIdx(labidx, stridx);
      mod.CurFunction()->GetLabelTab()->AddToStringLabelMap(labidx);
    }
    tryNode->SetCatchOffset(labidx);
  }
  lexer.NextToken();
  // parse finally label
  if (lexer.GetTokenKind() == TK_intconst && lexer.GetTheIntVal() == 0) {
    tryNode->SetFinallyOffset(0);
  } else {
    if (lexer.GetTokenKind() != TK_label) {
      Error("expect finally label in try but get ");
      return false;
    }
    GStrIdx stridx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(lexer.GetName());
    LabelIdx labidx = mod.CurFunction()->GetLabelTab()->GetStIdxFromStrIdx(stridx);
    if (labidx == 0) {
      labidx = mod.CurFunction()->GetLabelTab()->CreateLabel();
      mod.CurFunction()->GetLabelTab()->SetSymbolFromStIdx(labidx, stridx);
      mod.CurFunction()->GetLabelTab()->AddToStringLabelMap(labidx);
    }
    tryNode->SetFinallyOffset(labidx);
  }
  stmt = tryNode;
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseStmtTry(StmtNodePtr &stmt) {
  auto *tryNode = mod.CurFuncCodeMemPool()->New<TryNode>(mod);
  lexer.NextToken();
  ASSERT(lexer.GetTokenKind() == TK_lbrace, "expect left brace in try but get ");
  lexer.NextToken();
  // parse handler label
  while (lexer.GetTokenKind() != TK_rbrace) {
    if (lexer.GetTokenKind() != TK_label) {
      Error("expect handler label in try but get ");
      return false;
    }
    GStrIdx stridx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(lexer.GetName());
    LabelIdx labidx = mod.CurFunction()->GetLabelTab()->GetStIdxFromStrIdx(stridx);
    if (labidx == 0) {
      labidx = mod.CurFunction()->GetLabelTab()->CreateLabel();
      mod.CurFunction()->GetLabelTab()->SetSymbolFromStIdx(labidx, stridx);
      mod.CurFunction()->GetLabelTab()->AddToStringLabelMap(labidx);
    }
    tryNode->AddOffset(labidx);
    lexer.NextToken();
  }
  stmt = tryNode;
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseStmtCatch(StmtNodePtr &stmt) {
  auto *catchNode = mod.CurFuncCodeMemPool()->New<CatchNode>(mod);
  lexer.NextToken();
  ASSERT(lexer.GetTokenKind() == TK_lbrace, "expect left brace in catch but get ");
  lexer.NextToken();
  while (lexer.GetTokenKind() != TK_rbrace) {
    TyIdx tyidx(0);
    if (!ParseType(tyidx)) {
      Error("expect type parsing java catch statement");
      return false;
    }
    catchNode->PushBack(tyidx);
  }
  catchNode->SetNumOpnds(0);
  stmt = catchNode;
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseUnaryStmt(Opcode op, StmtNodePtr &stmt) {
  lexer.NextToken();
  auto *throwStmt = mod.CurFuncCodeMemPool()->New<UnaryStmtNode>(op);
  stmt = throwStmt;
  BaseNode *expr = nullptr;
  if (!ParseExprOneOperand(expr)) {
    return false;
  }
  throwStmt->SetOpnd(expr);
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseUnaryStmtThrow(StmtNodePtr &stmt) {
  return ParseUnaryStmt(OP_throw, stmt);
}

bool MIRParser::ParseUnaryStmtDecRef(StmtNodePtr &stmt) {
  return ParseUnaryStmt(OP_decref, stmt);
}

bool MIRParser::ParseUnaryStmtIncRef(StmtNodePtr &stmt) {
  return ParseUnaryStmt(OP_incref, stmt);
}

bool MIRParser::ParseUnaryStmtDecRefReset(StmtNodePtr &stmt) {
  return ParseUnaryStmt(OP_decrefreset, stmt);
}

bool MIRParser::ParseUnaryStmtEval(StmtNodePtr &stmt) {
  return ParseUnaryStmt(OP_eval, stmt);
}

bool MIRParser::ParseUnaryStmtFree(StmtNodePtr &stmt) {
  return ParseUnaryStmt(OP_free, stmt);
}

bool MIRParser::ParseUnaryStmtAssertNonNull(StmtNodePtr &stmt) {
  return ParseUnaryStmt(OP_assertnonnull, stmt);
}

bool MIRParser::ParseStmtMarker(StmtNodePtr &stmt) {
  Opcode op;
  switch (paramTokenKindForStmt) {
    case TK_jscatch:
      op = OP_jscatch;
      break;
    case TK_finally:
      op = OP_finally;
      break;
    case TK_cleanuptry:
      op = OP_cleanuptry;
      break;
    case TK_endtry:
      op = OP_endtry;
      break;
    case TK_retsub:
      op = OP_retsub;
      break;
    case TK_membaracquire:
      op = OP_membaracquire;
      break;
    case TK_membarrelease:
      op = OP_membarrelease;
      break;
    case TK_membarstoreload:
      op = OP_membarstoreload;
      break;
    case TK_membarstorestore:
      op = OP_membarstorestore;
      break;
    default:
      return false;
  }
  auto *stmtNode = mod.CurFuncCodeMemPool()->New<StmtNode>(op);
  stmt = stmtNode;
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseStmtGosub(StmtNodePtr &stmt) {
  if (lexer.NextToken() != TK_label) {
    Error("expect finally label in gosub but get ");
    return false;
  }
  GStrIdx stridx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(lexer.GetName());
  LabelIdx labidx = mod.CurFunction()->GetLabelTab()->GetStIdxFromStrIdx(stridx);
  if (labidx == 0) {
    labidx = mod.CurFunction()->GetLabelTab()->CreateLabel();
    mod.CurFunction()->GetLabelTab()->SetSymbolFromStIdx(labidx, stridx);
    mod.CurFunction()->GetLabelTab()->AddToStringLabelMap(labidx);
  }
  auto *goSubNode = mod.CurFuncCodeMemPool()->New<GotoNode>(OP_gosub);
  goSubNode->SetOffset(labidx);
  stmt = goSubNode;
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseBinaryStmt(StmtNodePtr &stmt, Opcode op) {
  auto *assStmt = mod.CurFuncCodeMemPool()->New<BinaryStmtNode>(op);
  lexer.NextToken();
  BaseNode *opnd0 = nullptr;
  BaseNode *opnd1 = nullptr;
  if (!ParseExprTwoOperand(opnd0, opnd1)) {
    return false;
  }
  assStmt->SetBOpnd(opnd0, 0);
  assStmt->SetBOpnd(opnd1, 1);
  stmt = assStmt;
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseBinaryStmtAssertGE(StmtNodePtr &stmt) {
  return ParseBinaryStmt(stmt, OP_assertge);
}

bool MIRParser::ParseBinaryStmtAssertLT(StmtNodePtr &stmt) {
  return ParseBinaryStmt(stmt, OP_assertlt);
}

bool MIRParser::ParseNaryStmt(StmtNodePtr &stmt, Opcode op) {
  auto *stmtReturn = mod.CurFuncCodeMemPool()->New<NaryStmtNode>(mod, op);
  if (lexer.NextToken() != TK_lparen) {
    Error("expect return with ( but get ");
    return false;
  }
  TokenKind exprTk = lexer.NextToken();
  if (exprTk == TK_rparen) {  // no operand
    stmt = stmtReturn;
    lexer.NextToken();
    return true;
  }
  BaseNode *expr = nullptr;
  if (!ParseExpression(expr)) {
    Error("ParseStmtReturn failed");
    return false;
  }
  stmtReturn->GetNopnd().push_back(expr);
  if (op == OP_syncenter) {
    if (lexer.GetTokenKind() == TK_coma) {
      lexer.NextToken();
      BaseNode *exprSync = nullptr;
      if (!ParseExpression(exprSync)) {
        Error("ParseStmtReturn failed");
        return false;
      }
      stmtReturn->GetNopnd().push_back(exprSync);
    } else {
      MIRType *intType = GlobalTables::GetTypeTable().GetTypeFromTyIdx((TyIdx)PTY_i32);
      // default 2 for __sync_enter_fast()
      MIRIntConst *intConst = mod.GetMemPool()->New<MIRIntConst>(2, *intType);
      ConstvalNode *exprConst = mod.GetMemPool()->New<ConstvalNode>();
      exprConst->SetPrimType(PTY_i32);
      exprConst->SetConstVal(intConst);
      stmtReturn->GetNopnd().push_back(exprConst);
      stmtReturn->SetNumOpnds(stmtReturn->GetNopndSize());
    }
  }
  if (lexer.GetTokenKind() != TK_rparen) {
    Error("expect ) parsing return but get ");
    return false;
  }
  stmtReturn->SetNumOpnds(stmtReturn->GetNopndSize());
  stmt = stmtReturn;
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseNaryStmtReturn(StmtNodePtr &stmt) {
  return ParseNaryStmt(stmt, OP_return);
}

bool MIRParser::ParseNaryStmtSyncEnter(StmtNodePtr &stmt) {
  return ParseNaryStmt(stmt, OP_syncenter);
}

bool MIRParser::ParseNaryStmtSyncExit(StmtNodePtr &stmt) {
  return ParseNaryStmt(stmt, OP_syncexit);
}

bool MIRParser::ParseLoc(StmtNodePtr &stmt) {
  if (lexer.NextToken() != TK_intconst) {
    Error("expect intconst in LOC but get ");
    return false;
  }
  lastFileNum = lexer.GetTheIntVal();
  if (lexer.NextToken() != TK_intconst) {
    Error("expect intconst in LOC but get ");
    return false;
  }
  lastLineNum = lexer.GetTheIntVal();
  if (firstLineNum == 0) {
    firstLineNum = lastLineNum;
  }
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseStatement(StmtNodePtr &stmt) {
  paramTokenKindForStmt = lexer.GetTokenKind();
  uint32 mplNum = lexer.GetLineNum();
  uint32 lnum = lastLineNum;
  uint32 fnum = lastFileNum;
  std::map<TokenKind, FuncPtrParseStmt>::iterator itFuncPtr = funcPtrMapForParseStmt.find(paramTokenKindForStmt);
  if (itFuncPtr != funcPtrMapForParseStmt.end()) {
    if (!(this->*(itFuncPtr->second))(stmt)) {
      return false;
    }
  } else {
    return false;
  }
  if (stmt && stmt->GetSrcPos().MplLineNum() == 0) {
    stmt->GetSrcPos().SetFileNum(fnum);
    stmt->GetSrcPos().SetLineNum(lnum);
    stmt->GetSrcPos().SetMplLineNum(mplNum);
  }
  return true;
}

/* parse the statements enclosed by { and }
 */
bool MIRParser::ParseStmtBlock(BlockNodePtr &blk) {
  if (lexer.GetTokenKind() != TK_lbrace) {
    Error("expect { for func body but get ");
    return false;
  }
  blk = mod.CurFuncCodeMemPool()->New<BlockNode>();
  MIRFunction *fn = mod.CurFunction();
  paramCurrFuncForParseStmtBlock = fn;
  lexer.NextToken();
  // Insert _mcount for PI.
  if (mod.GetWithProfileInfo()) {
    StmtNode *stmtt = nullptr;
    if (!ParseStmtCallMcount(stmtt)) {
      return false;
    }
    blk->AddStatement(stmtt);
  }
  while (true) {
    TokenKind stmtTk = lexer.GetTokenKind();
    // calculate the mpl file line number mplNum here to get accurate result
    uint32 mplNum = lexer.GetLineNum();
    if (IsStatement(stmtTk)) {
      ParseStmtBlockForSeenComment(blk, mplNum);
      StmtNode *stmt = nullptr;
      if (!ParseStatement(stmt)) {
        Error("ParseStmtBlock failed when parsing a statement");
        return false;
      }
      if (stmt != nullptr) {  // stmt is nullptr if it is a LOC
        SetSrcPos(stmt, mplNum);
        blk->AddStatement(stmt);
      }
    } else {
      std::map<TokenKind, FuncPtrParseStmtBlock>::iterator itFuncPtr = funcPtrMapForParseStmtBlock.find(stmtTk);
      if (itFuncPtr == funcPtrMapForParseStmtBlock.end()) {
        if (stmtTk == TK_rbrace) {
          ParseStmtBlockForSeenComment(blk, mplNum);
          lexer.NextToken();
          return true;
        } else {
          Error("expect } or var or statement for func body but get ");
          return false;
        }
      } else {
        if (!(this->*(itFuncPtr->second))()) {
          return false;
        }
      }
    }
  }
}

void MIRParser::ParseStmtBlockForSeenComment(BlockNodePtr blk, uint32 mplNum) {
  // collect accumulated comments into comment statement nodes
  if (!lexer.seenComments.empty()) {
    for (size_t i = 0; i < lexer.seenComments.size(); ++i) {
      auto *cmnt = mod.CurFuncCodeMemPool()->New<CommentNode>(mod);
      cmnt->SetComment(lexer.seenComments[i]);
      SetSrcPos(cmnt, mplNum);
      blk->AddStatement(cmnt);
    }
    lexer.seenComments.clear();
  }
}

bool MIRParser::ParseStmtBlockForVar(TokenKind stmtTK) {
  MIRFunction *fn = paramCurrFuncForParseStmtBlock;
  MIRSymbol *st = fn->GetSymTab()->CreateSymbol(kScopeLocal);
  st->SetStorageClass(kScAuto);
  st->SetSKind(kStVar);
  if (stmtTK == TK_tempvar) {
    st->SetIsTmp(true);
  }
  if (!ParseDeclareVar(*st)) {
    return false;
  }
  if (!fn->GetSymTab()->AddToStringSymbolMap(*st)) {
    Error("duplicate declare symbol parse function ");
    return false;
  }
  return true;
}

bool MIRParser::ParseStmtBlockForVar() {
  return ParseStmtBlockForVar(TK_var);
}

bool MIRParser::ParseStmtBlockForTempVar() {
  return ParseStmtBlockForVar(TK_tempvar);
}

bool MIRParser::ParseStmtBlockForReg() {
  lexer.NextToken();
  if (lexer.GetTokenKind() != TK_preg) {
    Error("expect %%preg after reg");
    return false;
  }
  PregIdx pregIdx;
  if (!ParseRefPseudoReg(pregIdx)) {
    return false;
  }
  MIRPreg *preg = mod.CurFunction()->GetPregTab()->PregFromPregIdx(pregIdx);
  TyIdx tyidx(0);
  if (!ParseType(tyidx)) {
    Error("ParseDeclareVar failed when parsing the type");
    return false;
  }
  ASSERT(tyidx > 0, "parse declare var failed ");
  MIRType *mirType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(tyidx);
  preg->SetMIRType(mirType);
  if (lexer.GetTokenKind() == TK_intconst) {
    int64 theIntVal = lexer.GetTheIntVal();
    if (theIntVal != 0 && theIntVal != 1) {
      Error("parseDeclareReg failed");
      return false;
    }
    preg->SetNeedRC(theIntVal == 0 ? false : true);
  } else {
    Error("parseDeclareReg failed");
    return false;
  }
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseStmtBlockForType() {
  paramParseLocalType = true;
  if (!ParseTypedef()) {
    return false;
  }
  return true;
}

bool MIRParser::ParseStmtBlockForFrameSize() {
  MIRFunction *fn = paramCurrFuncForParseStmtBlock;
  lexer.NextToken();
  if (lexer.GetTokenKind() != TK_intconst) {
    Error("expect integer after frameSize but get ");
    return false;
  }
  fn->SetFrameSize(lexer.GetTheIntVal());
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseStmtBlockForUpformalSize() {
  MIRFunction *fn = paramCurrFuncForParseStmtBlock;
  lexer.NextToken();
  if (lexer.GetTokenKind() != TK_intconst) {
    Error("expect integer after upFormalSize but get ");
    return false;
  }
  fn->SetUpFormalSize(lexer.GetTheIntVal());
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseStmtBlockForModuleID() {
  MIRFunction *fn = paramCurrFuncForParseStmtBlock;
  lexer.NextToken();
  if (lexer.GetTokenKind() != TK_intconst) {
    Error("expect integer after moduleid but get ");
    return false;
  }
  fn->SetModuleID(lexer.GetTheIntVal());
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseStmtBlockForFuncSize() {
  MIRFunction *fn = paramCurrFuncForParseStmtBlock;
  lexer.NextToken();
  if (lexer.GetTokenKind() != TK_intconst) {
    Error("expect integer after funcSize but get ");
    return false;
  }
  fn->SetFuncSize(lexer.GetTheIntVal());
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseStmtBlockForFuncID() {
  // funcid is for debugging purpose
  MIRFunction *fn = paramCurrFuncForParseStmtBlock;
  lexer.NextToken();
  if (lexer.GetTokenKind() != TK_intconst) {
    Error("expect integer after funcid but get ");
    return false;
  }
  fn->SetPuidxOrigin(lexer.GetTheIntVal());
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseStmtBlockForFormalWordsTypeTagged() {
  MIRFunction *fn = paramCurrFuncForParseStmtBlock;
  uint8 *addr = ParseWordsInfo(fn->GetUpFormalSize());
  if (addr == nullptr) {
    Error("parser error for formalwordstypetagged");
    return false;
  }
  fn->SetFormalWordsTypeTagged(addr);
  return true;
}

bool MIRParser::ParseStmtBlockForLocalWordsTypeTagged() {
  MIRFunction *fn = paramCurrFuncForParseStmtBlock;
  uint8 *addr = ParseWordsInfo(fn->GetFrameSize());
  if (addr == nullptr) {
    Error("parser error for localWordsTypeTagged");
    return false;
  }
  fn->SetLocalWordsTypeTagged(addr);
  return true;
}

bool MIRParser::ParseStmtBlockForFormalWordsRefCounted() {
  MIRFunction *fn = paramCurrFuncForParseStmtBlock;
  uint8 *addr = ParseWordsInfo(fn->GetUpFormalSize());
  if (addr == nullptr) {
    Error("parser error for formalwordsrefcounted");
    return false;
  }
  fn->SetFormalWordsRefCounted(addr);
  return true;
}

bool MIRParser::ParseStmtBlockForLocalWordsRefCounted() {
  MIRFunction *fn = paramCurrFuncForParseStmtBlock;
  uint8 *addr = ParseWordsInfo(fn->GetFrameSize());
  if (addr == nullptr) {
    Error("parser error for localwordsrefcounted");
    return false;
  }
  fn->SetLocalWordsRefCounted(addr);
  return true;
}

bool MIRParser::ParseStmtBlockForFuncInfo() {
  lexer.NextToken();
  if (!ParseFuncInfo()) {
    return false;
  }
  return true;
}

/* exprparser */
static Opcode GetUnaryOp(TokenKind tk) {
  switch (tk) {
#define UNARYOP(P) \
  case TK_##P:     \
    return OP_##P;
#include "unary_op.def"
#undef UNARYOP
    default:
      return OP_undef;
  }
}

static Opcode GetBinaryOp(TokenKind tk) {
  switch (tk) {
#define BINARYOP(P) \
  case TK_##P:      \
    return OP_##P;
#include "binary_op.def"
#undef BINARYOP
    default:
      return OP_undef;
  }
}

static Opcode GetConvertOp(TokenKind tk) {
  switch (tk) {
    case TK_ceil:
      return OP_ceil;
    case TK_cvt:
      return OP_cvt;
    case TK_floor:
      return OP_floor;
    case TK_round:
      return OP_round;
    case TK_trunc:
      return OP_trunc;
    default:
      return OP_undef;
  }
}

bool MIRParser::ParseExprOneOperand(BaseNodePtr &expr) {
  if (lexer.GetTokenKind() != TK_lparen) {
    Error("expect ( parsing operand parsing unary ");
    return false;
  }
  lexer.NextToken();
  if (!ParseExpression(expr)) {
    Error("expect expression as openrand of unary expression ");
    return false;
  }
  if (lexer.GetTokenKind() != TK_rparen) {
    Error("expect ) parsing operand parsing unary ");
    return false;
  }
  return true;
}

bool MIRParser::ParseExprTwoOperand(BaseNodePtr &opnd0, BaseNodePtr &opnd1) {
  if (lexer.GetTokenKind() != TK_lparen) {
    Error("expect ( parsing operand parsing unary ");
    return false;
  }
  lexer.NextToken();
  if (!ParseExpression(opnd0)) {
    return false;
  }
  if (lexer.GetTokenKind() != TK_coma) {
    Error("expect , between two operands but get ");
    return false;
  }
  lexer.NextToken();
  if (!ParseExpression(opnd1)) {
    return false;
  }
  if (lexer.GetTokenKind() != TK_rparen) {
    Error("expect ) parsing operand parsing unary ");
    return false;
  }
  return true;
}

bool MIRParser::ParseExprNaryOperand(MapleVector<BaseNode*> &opndVec) {
  if (lexer.GetTokenKind() != TK_lparen) {
    Error("expect ( parsing operand parsing nary operands ");
    return false;
  }
  TokenKind tk = lexer.NextToken();
  while (tk != TK_rparen) {
    BaseNode *opnd = nullptr;
    if (!ParseExpression(opnd)) {
      Error("expect expression parsing nary operands ");
      return false;
    }
    opndVec.push_back(opnd);
    tk = lexer.GetTokenKind();
    if (tk == TK_coma) {
      tk = lexer.NextToken();
    }
  }
  return true;
}

bool MIRParser::ParseDeclaredSt(StIdx &stidx) {
  TokenKind varTk = lexer.GetTokenKind();
  stidx.SetFullIdx(0);
  GStrIdx stridx = GlobalTables::GetStrTable().GetStrIdxFromName(lexer.GetName());
  if (stridx == 0) {
    Error("symbol not declared ");
    stidx.SetFullIdx(0);
    return false;
  }
  if (varTk == TK_gname) {
    stidx = GlobalTables::GetGsymTable().GetStIdxFromStrIdx(stridx);
    if (stidx.FullIdx() == 0) {
      Error("global symbol not declared ");
      return false;
    }
  } else if (varTk == TK_lname) {
    stidx = mod.CurFunction()->GetSymTab()->GetStIdxFromStrIdx(stridx);
    if (stidx.FullIdx() == 0) {
      Error("local symbol not declared ");
      return false;
    }
  } else {
    Error("expect global/local name but get ");
    return false;
  }
  return true;
}

bool MIRParser::ParseDeclaredFunc(PUIdx &puidx) {
  GStrIdx stridx = GlobalTables::GetStrTable().GetStrIdxFromName(lexer.GetName());
  if (stridx == 0) {
    Error("symbol not declared ");
    return false;
  }
  StIdx stidx = GlobalTables::GetGsymTable().GetStIdxFromStrIdx(stridx);
  if (stidx.FullIdx() == 0) {
    Error("function symbol not declared ");
    return false;
  }
  MIRSymbol *st = GlobalTables::GetGsymTable().GetSymbolFromStidx(stidx.Idx());
  if (st->GetSKind() != kStFunc) {
    Error("function name not declared as function");
    return false;
  }
  MIRFunction *func = st->GetFunction();
  puidx = func->GetPuidx();
  return true;
}

bool MIRParser::ParseExprDread(BaseNodePtr &expr) {
  if (lexer.GetTokenKind() != TK_dread) {
    Error("expect dread but get ");
    return false;
  }
  AddrofNode *dexpr = mod.CurFuncCodeMemPool()->New<AddrofNode>(OP_dread);
  expr = dexpr;
  lexer.NextToken();
  TyIdx tyidx(0);
  bool parseRet = ParsePrimType(tyidx);
  if (tyidx == 0 || !parseRet) {
    Error("expect primitive type but get ");
    return false;
  }
  expr->SetPrimType(GlobalTables::GetTypeTable().GetPrimTypeFromTyIdx(tyidx));
  StIdx stidx;
  if (!ParseDeclaredSt(stidx)) {
    return false;
  }
  if (stidx.FullIdx() == 0) {
    Error("expect a symbol ParseExprDread failed");
    return false;
  }
  dexpr->SetStIdx(stidx);
  TokenKind endtk = lexer.NextToken();
  if (endtk == TK_intconst) {
    dexpr->SetFieldID(lexer.GetTheIntVal());
    lexer.NextToken();
  } else if (!IsDelimitationTK(endtk)) {
    Error("expect , or ) delimitation token but get ");
    return false;
  } else {
    dexpr->SetFieldID(0);
  }
  if (!dexpr->CheckNode(mod)) {
    Error("dread is not legal");
    return false;
  }
  return true;
}

bool MIRParser::ParseExprRegread(BaseNodePtr &expr) {
  auto *regRead = mod.CurFuncCodeMemPool()->New<RegreadNode>();
  expr = regRead;
  lexer.NextToken();
  TyIdx tyidx(0);
  if (!ParsePrimType(tyidx)) {
    return false;
  }
  if (tyidx == 0) {
    Error("expect primitive type but get ");
    return false;
  }
  expr->SetPrimType(GlobalTables::GetTypeTable().GetPrimTypeFromTyIdx(tyidx));
  if (lexer.GetTokenKind() == TK_specialreg) {
    PregIdx tempPregIdx = regRead->GetRegIdx();
    bool isSuccess = ParseSpecialReg(tempPregIdx);
    regRead->SetRegIdx(tempPregIdx);
    return isSuccess;
  }
  if (lexer.GetTokenKind() == TK_preg) {
    if (expr->GetPrimType() == PTY_ptr || expr->GetPrimType() == PTY_ref) {
      PregIdx tempPregIdx = regRead->GetRegIdx();
      bool isSuccess = ParseRefPseudoReg(tempPregIdx);
      regRead->SetRegIdx(tempPregIdx);
      return isSuccess;
    } else {
      PregIdx tempPregIdx = regRead->GetRegIdx();
      bool isSuccess = ParsePseudoReg(expr->GetPrimType(), tempPregIdx);
      regRead->SetRegIdx(tempPregIdx);
      return isSuccess;
    }
  }
  Error("expect special or pseudo register but get ");
  return false;
}

bool MIRParser::ParseExprConstval(BaseNodePtr &expr) {
  auto *exprConst = mod.CurFuncCodeMemPool()->New<ConstvalNode>();
  TokenKind typeTk = lexer.NextToken();
  if (!IsPrimitiveType(typeTk)) {
    Error("expect type for GetConstVal but get ");
    return false;
  }
  exprConst->SetPrimType(GetPrimitiveType(typeTk));
  lexer.NextToken();
  MIRConst *constVal = nullptr;
  if (!ParseScalarValue(constVal, *GlobalTables::GetTypeTable().GetPrimType(exprConst->GetPrimType()))) {
    Error("expect scalar type but get ");
    return false;
  }
  exprConst->SetConstVal(constVal);
  expr = exprConst;
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseExprConststr(BaseNodePtr &expr) {
  auto *strConst = mod.CurFuncCodeMemPool()->New<ConststrNode>();
  TokenKind tk = lexer.NextToken();
  if (!IsPrimitiveType(tk)) {
    Error("expect primitive type for conststr but get ");
    return false;
  }
  strConst->SetPrimType(GetPrimitiveType(tk));
  if (!IsAddress(strConst->GetPrimType())) {
    Error("expect primitive type for conststr but get ");
    return false;
  }
  tk = lexer.NextToken();
  if (tk != TK_string) {
    Error("expect string literal for conststr but get ");
    return false;
  }
  strConst->SetStrIdx(GlobalTables::GetUStrTable().GetOrCreateStrIdxFromName(lexer.GetName()));
  expr = strConst;
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseExprConststr16(BaseNodePtr &expr) {
  auto *str16Const = mod.CurFuncCodeMemPool()->New<Conststr16Node>();
  TokenKind tk = lexer.NextToken();
  if (!IsPrimitiveType(tk)) {
    Error("expect primitive type for conststr16 but get ");
    return false;
  }
  str16Const->SetPrimType(GetPrimitiveType(tk));
  if (!IsAddress(str16Const->GetPrimType())) {
    Error("expect primitive type for conststr16 but get ");
    return false;
  }
  tk = lexer.NextToken();
  if (tk != TK_string) {
    Error("expect string literal for conststr16 but get ");
    return false;
  }
  // UTF-16 strings in mpl files are presented as UTF-8 strings
  // to keep the printable chars in ascii form
  // so we need to do a UTF8ToUTF16 conversion
  std::string str = lexer.GetName();
  std::u16string str16;
  NameMangler::UTF8ToUTF16(str16, str);
  str16Const->SetStrIdx(GlobalTables::GetU16StrTable().GetOrCreateStrIdxFromName(str16));
  expr = str16Const;
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseExprSizeoftype(BaseNodePtr &expr) {
  auto *exprSizeOfType = mod.CurFuncCodeMemPool()->New<SizeoftypeNode>();
  TokenKind typeTk = lexer.NextToken();
  if (!IsPrimitiveType(typeTk)) {
    Error("expect type for GetConstVal but get ");
    return false;
  }
  exprSizeOfType->SetPrimType(GetPrimitiveType(typeTk));
  lexer.NextToken();
  TyIdx tyidx(0);
  if (!ParseType(tyidx)) {
    Error("expect type parsing array but get ");
    return false;
  }
  exprSizeOfType->SetTyIdx(tyidx);
  expr = exprSizeOfType;
  return true;
}

bool MIRParser::ParseExprFieldsDist(BaseNodePtr &expr) {
  TokenKind typeTk = lexer.NextToken();
  if (!IsPrimitiveType(typeTk)) {
    Error("expect type for GetConstVal but get ");
    return false;
  }
  auto *node = mod.CurFuncCodeMemPool()->New<FieldsDistNode>();
  node->SetPrimType(GetPrimitiveType(typeTk));
  lexer.NextToken();
  TyIdx tyIdx(0);
  if (!ParseType(tyIdx)) {
    Error("expect type parsing array but get ");
    return false;
  }
  node->SetTyIdx(tyIdx);
  TokenKind tk = lexer.GetTokenKind();
  if (tk != TK_intconst) {
    Error("expect type int but get");
    return false;
  }
  node->SetFiledID1(lexer.GetTheIntVal());
  tk = lexer.NextToken();
  if (tk != TK_intconst) {
    Error("expect type int but get");
    return false;
  }
  node->SetFiledID2(lexer.GetTheIntVal());
  lexer.NextToken();
  expr = node;
  return true;
}

bool MIRParser::ParseExprBinary(BaseNodePtr &expr) {
  Opcode opcode = GetBinaryOp(lexer.GetTokenKind());
  if (opcode == OP_undef) {
    Error("expect add operator but get ");
    return false;
  }
  auto *addExpr = mod.CurFuncCodeMemPool()->New<BinaryNode>(opcode);
  if (!IsPrimitiveType(lexer.NextToken())) {
    Error("expect type parsing binary operator but get ");
    return false;
  }
  addExpr->SetPrimType(GetPrimitiveType(lexer.GetTokenKind()));
  lexer.NextToken();
  BaseNode *opnd0 = nullptr;
  BaseNode *opnd1 = nullptr;
  if (!ParseExprTwoOperand(opnd0, opnd1)) {
    return false;
  }
  addExpr->SetBOpnd(opnd0, 0);
  addExpr->SetBOpnd(opnd1, 1);
  expr = addExpr;
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseExprCompare(BaseNodePtr &expr) {
  Opcode opcode = GetBinaryOp(lexer.GetTokenKind());
  auto *addExpr = mod.CurFuncCodeMemPool()->New<CompareNode>(opcode);
  if (!IsPrimitiveType(lexer.NextToken())) {
    Error("expect type parsing compare operator but get ");
    return false;
  }
  addExpr->SetPrimType(GetPrimitiveType(lexer.GetTokenKind()));
  if (!IsPrimitiveType(lexer.NextToken())) {
    Error("expect operand type parsing compare operator but get ");
    return false;
  }
  addExpr->SetOpndType(GetPrimitiveType(lexer.GetTokenKind()));
  lexer.NextToken();
  BaseNode *opnd0 = nullptr;
  BaseNode *opnd1 = nullptr;
  if (!ParseExprTwoOperand(opnd0, opnd1)) {
    return false;
  }
  addExpr->SetBOpnd(opnd0, 0);
  addExpr->SetBOpnd(opnd1, 1);
  expr = addExpr;
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseExprDepositbits(BaseNodePtr &expr) {
  // syntax: depositbits <int-type> <boffset> <bSize> (<opnd0>, <opnd1>)
  if (lexer.GetTokenKind() != TK_depositbits) {
    Error("expect depositbits but get ");
    return false;
  }
  auto *dpsbNode = mod.CurFuncCodeMemPool()->New<DepositbitsNode>();
  expr = dpsbNode;
  PrimType ptyp = GetPrimitiveType(lexer.NextToken());
  if (!IsPrimitiveInteger(ptyp)) {
    Error("expect <int-type> but get ");
    return false;
  }
  dpsbNode->SetPrimType(ptyp);
  if (lexer.NextToken() != TK_intconst) {
    Error("expect boffset but get ");
    return false;
  }
  dpsbNode->SetBitsOffset(lexer.GetTheIntVal());
  if (lexer.NextToken() != TK_intconst) {
    Error("expect bSize but get ");
    return false;
  }
  dpsbNode->SetBitsSize(lexer.GetTheIntVal());
  lexer.NextToken();
  BaseNode *opnd0 = nullptr;
  BaseNode *opnd1 = nullptr;
  if (!ParseExprTwoOperand(opnd0, opnd1)) {
    Error("ParseExprDepositbits when parsing two operand");
    return false;
  }
  dpsbNode->SetBOpnd(opnd0, 0);
  dpsbNode->SetBOpnd(opnd1, 1);
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseExprIreadIaddrof(IreadNode &expr) {
  // syntax : iread/iaddrof <prim-type> <type> <field-id> (<addr-expr>)
  if (!IsPrimitiveType(lexer.NextToken())) {
    Error("expect primitive type but get ");
    return false;
  }
  TyIdx tyidx(0);
  if (!ParsePrimType(tyidx)) {
    return false;
  }
  expr.SetPrimType(GlobalTables::GetTypeTable().GetPrimTypeFromTyIdx(tyidx));
  tyidx = TyIdx(0);
  if (!ParseDerivedType(tyidx)) {
    Error("ParseExprIreadIaddrof failed when paring derived type");
    return false;
  }
  expr.SetTyIdx(tyidx);
  if (lexer.GetTokenKind() == TK_intconst) {
    expr.SetFieldID(lexer.theIntVal);
    lexer.NextToken();
  }
  BaseNode *opnd0 = nullptr;
  if (!ParseExprOneOperand(opnd0)) {
    return false;
  }
  expr.SetOpnd(opnd0);
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseExprIread(BaseNodePtr &expr) {
  // syntax : iread <prim-type> <type> <field-id> (<addr-expr>)
  auto *iExpr = mod.CurFuncCodeMemPool()->New<IreadNode>(OP_iread);
  if (!ParseExprIreadIaddrof(*iExpr)) {
    Error("ParseExprIread failed when trying to parse addof");
    return false;
  }
  expr = iExpr;
  return true;
}

bool MIRParser::ParseExprIaddrof(BaseNodePtr &expr) {
  // syntax : iaddrof <prim-type> <type> <field-id> (<addr-expr>)
  auto *iExpr = mod.CurFuncCodeMemPool()->New<IreadNode>(OP_iaddrof);
  if (!ParseExprIreadIaddrof(*iExpr)) {
    Error("ParseExprIaddrof failed when trying to parse addof");
    return false;
  }
  expr = iExpr;
  return true;
}

bool MIRParser::ParseExprIreadoff(BaseNodePtr &expr) {
  // syntax : iread <prim-type> <offset> (<addr-expr>)
  auto *iReadOff = mod.CurFuncCodeMemPool()->New<IreadoffNode>();
  expr = iReadOff;
  if (!IsPrimitiveType(lexer.NextToken())) {
    Error("expect primitive type but get ");
    return false;
  }
  TyIdx tyidx(0);
  if (!ParsePrimType(tyidx)) {
    return false;
  }
  iReadOff->SetPrimType(GlobalTables::GetTypeTable().GetPrimTypeFromTyIdx(tyidx));
  if (!IsPrimitiveScalar(iReadOff->GetPrimType())) {
    Error("only scalar types allowed for ireadoff");
    return false;
  }
  if (lexer.GetTokenKind() != TK_intconst) {
    Error("expect offset but get ");
    return false;
  }
  iReadOff->SetOffset(lexer.GetTheIntVal());
  lexer.NextToken();
  BaseNode *opnd = nullptr;
  if (!ParseExprOneOperand(opnd)) {
    Error("ParseExprIreadoff when paring one operand");
    return false;
  }
  iReadOff->SetOpnd(opnd);
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseExprIreadFPoff(BaseNodePtr &expr) {
  // syntax : iread <prim-type> <offset>
  auto *iReadOff = mod.CurFuncCodeMemPool()->New<IreadFPoffNode>();
  expr = iReadOff;
  if (!IsPrimitiveType(lexer.NextToken())) {
    Error("expect primitive type but get ");
    return false;
  }
  TyIdx tyidx(0);
  if (!ParsePrimType(tyidx)) {
    return false;
  }
  iReadOff->SetPrimType(GlobalTables::GetTypeTable().GetPrimTypeFromTyIdx(tyidx));
  if (!IsPrimitiveScalar(iReadOff->GetPrimType())) {
    Error("only scalar types allowed for ireadoff");
    return false;
  }
  if (lexer.GetTokenKind() != TK_intconst) {
    Error("expect offset but get ");
    return false;
  }
  iReadOff->SetOffset(lexer.GetTheIntVal());
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseExprAddrof(BaseNodePtr &expr) {
  // syntax: addrof <prim-type> <var-name> <field-id>
  auto *addrofNode = mod.CurFuncCodeMemPool()->New<AddrofNode>(OP_addrof);
  expr = addrofNode;
  if (lexer.GetTokenKind() != TK_addrof) {
    Error("expect addrof but get ");
    return false;
  }
  lexer.NextToken();
  TyIdx tyidx(0);
  if (!ParsePrimType(tyidx)) {
    Error("expect primitive type but get ");
    return false;
  }
  addrofNode->SetPrimType(GlobalTables::GetTypeTable().GetPrimTypeFromTyIdx(tyidx));
  StIdx stidx;
  if (!ParseDeclaredSt(stidx)) {
    return false;
  }
  if (stidx.FullIdx() == 0) {
    Error("expect symbol ParseExprAddroffunc");
    return false;
  }
  addrofNode->SetStIdx(stidx);
  TokenKind tk = lexer.NextToken();
  if (IsDelimitationTK(tk)) {
    addrofNode->SetFieldID(0);
  } else if (tk == TK_intconst) {
    addrofNode->SetFieldID(lexer.GetTheIntVal());
    lexer.NextToken();
  } else {
    addrofNode->SetFieldID(0);
  }
  return true;
}

bool MIRParser::ParseExprAddroffunc(BaseNodePtr &expr) {
  auto *addrOfFuncNode = mod.CurFuncCodeMemPool()->New<AddroffuncNode>();
  expr = addrOfFuncNode;
  TokenKind tk = lexer.NextToken();
  if (tk != TK_a32 && tk != TK_a64 && tk != TK_ptr) {
    Error("expect address primitive type but get ");
    return false;
  }
  TyIdx tyidx(0);
  if (!ParsePrimType(tyidx)) {
    Error("ParseExprAddroffunc failed when parsing primitive type");
    return false;
  }
  addrOfFuncNode->SetPrimType(GlobalTables::GetTypeTable().GetPrimTypeFromTyIdx(tyidx));
  if (lexer.GetTokenKind() != TK_fname) {
    Error("expect function name but get ");
    return false;
  }
  PUIdx pidx;
  if (!ParseDeclaredFunc(pidx)) {
    if (mod.GetFlavor() < kMmpl) {
      Error("expect .mmpl file");
      return false;
    }
    pidx = EnterUndeclaredFunction();
  }
  addrOfFuncNode->SetPUIdx(pidx);
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseExprAddroflabel(BaseNodePtr &expr) {
  // syntax: addroflabel <prim-type> <label>
  auto *addrOfLabelNode = mod.CurFuncCodeMemPool()->New<AddroflabelNode>();
  expr = addrOfLabelNode;
  TokenKind tk = lexer.NextToken();
  if (tk != TK_a32 && tk != TK_a64 && tk != TK_ptr) {
    Error("expect address primitive type but get ");
    return false;
  }
  TyIdx tyidx(0);
  if (!ParsePrimType(tyidx)) {
    Error("ParseExprAddroflabel failed");
    return false;
  }
  addrOfLabelNode->SetPrimType(GlobalTables::GetTypeTable().GetPrimTypeFromTyIdx(tyidx));
  if (lexer.GetTokenKind() != TK_label) {
    Error("expect label but get ");
    return false;
  }
  LabelIdx lblIdx = mod.CurFunction()->GetOrCreateLableIdxFromName(lexer.GetName());
  addrOfLabelNode->SetOffset(lblIdx);
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseExprUnary(BaseNodePtr &expr) {
  // syntax op <prim-type> <label>
  Opcode op = GetUnaryOp(lexer.GetTokenKind());
  if (op == OP_undef) {
    Error("expect unary op but get ");
    return false;
  }
  lexer.NextToken();
  TyIdx tyidx(0);
  if (!ParsePrimType(tyidx)) {
    Error("expect primitive parsing unary operator ");
    return false;
  }
  auto *unaryNode = mod.CurFuncCodeMemPool()->New<UnaryNode>(op);
  expr = unaryNode;
  unaryNode->SetPrimType(GlobalTables::GetTypeTable().GetPrimTypeFromTyIdx(tyidx));
  BaseNode *opnd = nullptr;
  if (!ParseExprOneOperand(opnd)) {
    Error("parsing unary wrong ");
    return false;
  }
  unaryNode->SetOpnd(opnd);
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseExprGCMalloc(BaseNodePtr &expr) {
  // syntax op <prim-type> <type>
  TokenKind tk = lexer.GetTokenKind();
  if (tk != TK_gcmalloc && tk != TK_gcpermalloc) {
    Error("expect gcmalloc or gcpermalloc but get ");
    return false;
  }
  Opcode op = (tk == TK_gcmalloc) ? OP_gcmalloc : OP_gcpermalloc;
  lexer.NextToken();
  TyIdx ptyidx(0);
  if (IsPrimitiveType(lexer.GetTokenKind())) {
    if (!ParsePrimType(ptyidx)) {
      return false;
    }
  }
  TyIdx tyidx(0);
  if (!ParseType(tyidx)) {
    Error("expect type parsing unary operator ");
    return false;
  }
  auto *mallocNode = mod.CurFuncCodeMemPool()->New<GCMallocNode>(op);
  expr = mallocNode;
  mallocNode->SetPrimType(GlobalTables::GetTypeTable().GetPrimTypeFromTyIdx(ptyidx));
  mallocNode->SetOrigPType(mallocNode->GetPrimType());
  mallocNode->SetTyIdx(tyidx);
  return true;
}

bool MIRParser::ParseExprJarray(BaseNodePtr &expr) {
  // syntax op <prim-type> <java-array-type> <label>
  Opcode op = GetUnaryOp(lexer.GetTokenKind());
  if (op != OP_gcmallocjarray && op != OP_gcpermallocjarray) {
    Error("expect gcmallocjarray or gcpermallocjarray but get ");
    return false;
  }
  TyIdx ptyidx(0);
  lexer.NextToken();
  if (IsPrimitiveType(lexer.GetTokenKind())) {
    if (!ParsePrimType(ptyidx)) {
      return false;
    }
  }
  TyIdx tyidx(0);
  if (!ParseType(tyidx)) {
    Error("expect primitive parsing unary operator ");
    return false;
  }
  auto *jarrayNode = mod.CurFuncCodeMemPool()->New<JarrayMallocNode>(op);
  expr = jarrayNode;
  jarrayNode->SetPrimType(GlobalTables::GetTypeTable().GetPrimTypeFromTyIdx(ptyidx));
  jarrayNode->SetTyIdx(tyidx);
  BaseNode *opnd = nullptr;
  if (!ParseExprOneOperand(opnd)) {
    Error("parsing unary wrong ");
    return false;
  }
  jarrayNode->SetOpnd(opnd);
  lexer.NextToken();
  return true;
}

// parse extractbits, sext, zext
bool MIRParser::ParseExprExtractbits(BaseNodePtr &expr) {
  // extractbits <int-type> <boffset> <bSize> (<opnd0>)
  Opcode op = GetUnaryOp(lexer.GetTokenKind());
  if (op == OP_undef) {
    Error("expect unary op but get ");
    return false;
  }
  auto *extrctNode = mod.CurFuncCodeMemPool()->New<ExtractbitsNode>(op);
  expr = extrctNode;
  lexer.NextToken();
  TyIdx tyidx(0);
  if (!ParsePrimType(tyidx)) {
    Error("expect int type after extractbits after");
    return false;
  }
  PrimType ptyp = GlobalTables::GetTypeTable().GetPrimTypeFromTyIdx(tyidx);
  if (!IsPrimitiveInteger(ptyp)) {
    Error("sematical error expect int type for extractbits");
    return false;
  }
  extrctNode->SetPrimType(ptyp);
  if (op == OP_extractbits) {
    if (lexer.GetTokenKind() != TK_intconst) {
      Error("expect boffset but get ");
      return false;
    }
    extrctNode->SetBitsOffset(lexer.GetTheIntVal());
    lexer.NextToken();
  }
  if (lexer.GetTokenKind() != TK_intconst) {
    Error("expect bSize but get ");
    return false;
  }
  extrctNode->SetBitsSize(lexer.GetTheIntVal());
  lexer.NextToken();
  BaseNode *opnd = nullptr;
  if (!ParseExprOneOperand(opnd)) {
    Error("ParseExprExtractbits failed");
    return false;
  }
  extrctNode->SetOpnd(opnd);
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseExprTyconvert(BaseNodePtr &expr) {
  Opcode op = GetConvertOp(lexer.GetTokenKind());
  if (op == OP_undef) {
    Error("expect covertion operator but get ");
    return false;
  }
  auto *cvtNode = mod.CurFuncCodeMemPool()->New<TypeCvtNode>(op);
  expr = static_cast<BaseNode*>(cvtNode);
  PrimType toType = GetPrimitiveType(lexer.NextToken());
  if (toType == kPtyInvalid) {
    Error("expect to-type parsing conversion");
    return false;
  }
  cvtNode->SetPrimType(toType);
  lexer.NextToken();
  PrimType fromType = GetPrimitiveType(lexer.GetTokenKind());
  if (fromType == kPtyInvalid) {
    Error("expect type parsing conversion ");
    return false;
  }
  cvtNode->SetFromType(fromType);
  lexer.NextToken();
  BaseNode *opnd = nullptr;
  if (!ParseExprOneOperand(opnd)) {
    return false;
  }
  cvtNode->SetOpnd(opnd);
  if (op == OP_retype) {
    cvtNode->SetFromType(opnd->GetPrimType());
  }
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseExprRetype(BaseNodePtr &expr) {
  auto *cvtNode = mod.CurFuncCodeMemPool()->New<RetypeNode>();
  expr = static_cast<BaseNode*>(cvtNode);
  PrimType toType = GetPrimitiveType(lexer.NextToken());
  if (toType == kPtyInvalid) {
    Error("expect to-type parsing conversion");
    return false;
  }
  cvtNode->SetPrimType(toType);
  lexer.NextToken();
  TyIdx tyidx(0);
  if (!ParseDerivedType(tyidx)) {
    Error("ParseExprRetype failed when parsing derived type ");
    return false;
  }
  cvtNode->SetTyIdx(tyidx);
  BaseNode *opnd = nullptr;
  if (!ParseExprOneOperand(opnd)) {
    return false;
  }
  cvtNode->SetOpnd(opnd);
  cvtNode->SetFromType(opnd->GetPrimType());
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseExprTernary(BaseNodePtr &expr) {
  if (lexer.GetTokenKind() != TK_select) {
    Error("expect select but get ");
    return false;
  }
  TernaryNode *ternaryNode = mod.CurFuncCodeMemPool()->New<TernaryNode>(OP_select);
  expr = ternaryNode;
  lexer.NextToken();
  TyIdx tyidx(0);
  if (!ParsePrimType(tyidx)) {
    Error("expect primtype type but get ");
    return false;
  }
  ternaryNode->SetPrimType(GlobalTables::GetTypeTable().GetPrimTypeFromTyIdx(tyidx));
  MapleVector<BaseNode*> opndVec(mod.CurFuncCodeMemPoolAllocator()->Adapter());
  if (!ParseExprNaryOperand(opndVec)) {
    Error("ParseExprTernary failed");
    return false;
  }
  if (opndVec.size() != 3) {  // expect number of operands to be 3
    Error("expect 3 operands for ternary operator ");
    return false;
  }
  ternaryNode->SetOpnd(opndVec[0], 0);
  ternaryNode->SetOpnd(opndVec[1], 1);
  ternaryNode->SetOpnd(opndVec[2], 2);
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseExprArray(BaseNodePtr &expr) {
  // syntax: array <addr-type> <array-type> (<opnd0>, <opnd1>, . . . , <opndn>)
  auto *arrayNode = mod.CurFuncCodeMemPool()->New<ArrayNode>(mod);
  expr = arrayNode;
  if (lexer.GetTokenKind() != TK_array) {
    Error("expect array but get ");
    return false;
  }
  lexer.NextToken();
  if (lexer.GetTokenKind() == TK_intconst) {
    if (lexer.GetTheIntVal() == 1) {
      arrayNode->SetBoundsCheck(true);
    } else if (lexer.GetTheIntVal() == 0) {
      arrayNode->SetBoundsCheck(false);
    } else {
      Error("expect bounds_check(0/1) but get ");
      return false;
    }
  }
  lexer.NextToken();
  TyIdx tyidx;
  if (!ParsePrimType(tyidx)) {
    Error("expect address type but get ");
    return false;
  }
  arrayNode->SetPrimType(GlobalTables::GetTypeTable().GetPrimTypeFromTyIdx(tyidx));
  tyidx = TyIdx(0);
  if (!ParseType(tyidx)) {
    Error("expect type parsing array but get ");
    return false;
  }
  arrayNode->SetTyIdx(tyidx);
  // number of operand can not be zero
  MapleVector<BaseNode*> opndVec(mod.CurFuncCodeMemPoolAllocator()->Adapter());
  if (!ParseExprNaryOperand(opndVec)) {
    Error("ParseExprArray failed");
    return false;
  }
  if (opndVec.empty()) {
    Error("sematic error operands number of array expression is 0 ");
    return false;
  }
  arrayNode->SetNOpnd(opndVec);
  arrayNode->SetNumOpnds(opndVec.size());
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseIntrinsicId(IntrinsicopNode &intrnOpNode) {
  MIRIntrinsicID intrinId = GetIntrinsicID(lexer.GetTokenKind());
  if (intrinId <= INTRN_UNDEFINED || intrinId >= INTRN_LAST) {
    Error("wrong intrinsic id ");
    return false;
  }
  intrnOpNode.SetIntrinsic(intrinId);
  return true;
}

bool MIRParser::ParseExprIntrinsicop(BaseNodePtr &expr) {
  // syntax: intrinsicop <prim-type> <intrinsic> (<opnd0>, ..., <opndn>)
  // syntax: intrinsicopwithtype <prim-type> <intrinsic> (<opnd0>, ..., <opndn>)
  Opcode opCode = OP_intrinsicopwithtype;
  TokenKind kind = lexer.GetTokenKind();
  if (kind == TK_intrinsicop) {
    opCode = OP_intrinsicop;
  } else if (kind != TK_intrinsicopwithtype) {
    Error("expect intrinsicop(withtype) but get ");
    return false;
  }
  lexer.NextToken();
  TyIdx pTyIdx(0);
  if (!ParsePrimType(pTyIdx)) {
    Error("ParseExprIntrinsicop(withtype) failed when parsing type");
    return false;
  }

  TyIdx tyIdx(0);
  if (opCode == OP_intrinsicopwithtype && !ParseDerivedType(tyIdx)) {
    Error("ParseExprIntrinsicop(withtype) failed when parsing derived type ");
    return false;
  }
  auto *intrnOpNode = mod.CurFuncCodeMemPool()->New<IntrinsicopNode>(
      mod, opCode, GlobalTables::GetTypeTable().GetPrimTypeFromTyIdx(pTyIdx), tyIdx);
  expr = intrnOpNode;
  if (!ParseIntrinsicId(*intrnOpNode)) {
    return false;
  }
  // number of operand can not be zero
  lexer.NextToken();
  MapleVector<BaseNode*> opndVec(mod.CurFuncCodeMemPoolAllocator()->Adapter());
  if (!ParseExprNaryOperand(opndVec)) {
    Error("ParseExprIntrinsicop(withtype) failed");
    return false;
  }
  intrnOpNode->SetNOpnd(opndVec);
  intrnOpNode->SetNumOpnds(opndVec.size());
  lexer.NextToken();
  return true;
}

bool MIRParser::ParseScalarValue(MIRConstPtr &stype, MIRType &type) {
  PrimType ptp = type.GetPrimType();
  if (IsPrimitiveInteger(ptp) || IsPrimitiveDynType(ptp) || ptp == PTY_gen) {
    if (lexer.GetTokenKind() != TK_intconst) {
      Error("constant value incompatible with integer type at ");
      return false;
    }
    stype = mod.GetMemPool()->New<MIRIntConst>(lexer.GetTheIntVal(), type);
  } else if (ptp == PTY_f32) {
    if (lexer.GetTokenKind() != TK_floatconst) {
      Error("constant value incompatible with single-precision float type at ");
      return false;
    }
    MIRFloatConst *fConst = GlobalTables::GetFpConstTable().GetOrCreateFloatConst(lexer.GetTheFloatVal());
    stype = fConst;
  } else if (ptp == PTY_f64) {
    if (lexer.GetTokenKind() != TK_doubleconst && lexer.GetTokenKind() != TK_intconst) {
      Error("constant value incompatible with double-precision float type at ");
      return false;
    }
    MIRDoubleConst *dconst = GlobalTables::GetFpConstTable().GetOrCreateDoubleConst(lexer.GetTheDoubleVal());
    stype = dconst;
  } else {
    return false;
  }
  return true;
}

bool MIRParser::ParseConstAddrLeafExpr(MIRConstPtr &cexpr, MIRType &type) {
  BaseNode *expr = nullptr;
  if (!ParseExpression(expr)) {
    return false;
  }
  CHECK_FATAL(expr != nullptr, "null ptr check");
  if (expr->GetOpCode() != OP_addrof && expr->GetOpCode() != OP_addroffunc && expr->GetOpCode() != OP_conststr &&
      expr->GetOpCode() != OP_conststr16) {
    Error("ParseConstAddrLeafExpr expects one of OP_addrof, OP_addroffunc, OP_conststr and OP_conststr16");
    return false;
  }
  if (expr->GetOpCode() == OP_addrof) {
    auto *anode = static_cast<AddrofNode*>(expr);
    auto *currFn = static_cast<MIRFunction*>(mod.CurFunction());
    MIRSymbol *var = currFn->GetLocalOrGlobalSymbol(anode->GetStIdx());
    var->SetNeedForwDecl();
    mod.SetSomeSymbolNeedForDecl(true);
    TyIdx ptyIdx = var->GetTyIdx();
    MIRPtrType ptrType(ptyIdx, (mod.IsJavaModule() ? PTY_ref : PTY_ptr));
    ptyIdx = GlobalTables::GetTypeTable().GetOrCreateMIRType(&ptrType);
    MIRType *exprTy = GlobalTables::GetTypeTable().GetTypeFromTyIdx(ptyIdx);
    cexpr = mod.CurFuncCodeMemPool()->New<MIRAddrofConst>(anode->GetStIdx(), anode->GetFieldID(), *exprTy);
  } else if (expr->GetOpCode() == OP_addroffunc) {
    auto *aof = static_cast<AddroffuncNode*>(expr);
    MIRFunction *f = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(aof->GetPUIdx());
    const MIRSymbol *fName = f->GetFuncSymbol();
    TyIdx ptyIdx = fName->GetTyIdx();
    MIRPtrType ptrType(ptyIdx);
    ptyIdx = GlobalTables::GetTypeTable().GetOrCreateMIRType(&ptrType);
    MIRType *exprTy = GlobalTables::GetTypeTable().GetTypeFromTyIdx(ptyIdx);
    cexpr = mod.CurFuncCodeMemPool()->New<MIRAddroffuncConst>(aof->GetPUIdx(), *exprTy);
  } else if (expr->GetOpCode() == OP_conststr) {
    auto *cs = static_cast<ConststrNode*>(expr);
    UStrIdx stridx = cs->GetStrIdx();
    TyIdx ptyIdx = GlobalTables::GetTypeTable().GetTypeFromTyIdx(TyIdx(PTY_u8))->GetTypeIndex();
    MIRPtrType ptrtype(ptyIdx);
    ptyIdx = GlobalTables::GetTypeTable().GetOrCreateMIRType(&ptrtype);
    MIRType *exprty = GlobalTables::GetTypeTable().GetTypeFromTyIdx(ptyIdx);
    cexpr = mod.CurFuncCodeMemPool()->New<MIRStrConst>(stridx, *exprty);
  } else {
    auto *cs = static_cast<Conststr16Node*>(expr);
    U16StrIdx stridx = cs->GetStrIdx();
    TyIdx ptyIdx = GlobalTables::GetTypeTable().GetTypeFromTyIdx(TyIdx(PTY_u16))->GetTypeIndex();
    MIRPtrType ptrType(ptyIdx);
    ptyIdx = GlobalTables::GetTypeTable().GetOrCreateMIRType(&ptrType);
    MIRType *exprTy = GlobalTables::GetTypeTable().GetTypeFromTyIdx(ptyIdx);
    cexpr = mod.CurFuncCodeMemPool()->New<MIRStr16Const>(stridx, *exprTy);
  }
  return true;
}

bool MIRParser::ParseExpression(BaseNodePtr &expr) {
  TokenKind tk = lexer.GetTokenKind();
  std::map<TokenKind, MIRParser::FuncPtrParseExpr>::iterator itFuncPtr = funcPtrMapForParseExpr.find(tk);
  if (itFuncPtr == funcPtrMapForParseExpr.end()) {
    Error("expect expression but get ");
    return false;
  } else {
    if (!(this->*(itFuncPtr->second))(expr)) {
      return false;
    }
  }
  return true;
}

std::map<TokenKind, MIRParser::FuncPtrParseExpr> MIRParser::InitFuncPtrMapForParseExpr() {
  std::map<TokenKind, MIRParser::FuncPtrParseExpr> funcPtrMap;
  funcPtrMap[TK_addrof] = &MIRParser::ParseExprAddrof;
  funcPtrMap[TK_addroffunc] = &MIRParser::ParseExprAddroffunc;
  funcPtrMap[TK_addroflabel] = &MIRParser::ParseExprAddroflabel;
  funcPtrMap[TK_abs] = &MIRParser::ParseExprUnary;
  funcPtrMap[TK_bnot] = &MIRParser::ParseExprUnary;
  funcPtrMap[TK_lnot] = &MIRParser::ParseExprUnary;
  funcPtrMap[TK_neg] = &MIRParser::ParseExprUnary;
  funcPtrMap[TK_recip] = &MIRParser::ParseExprUnary;
  funcPtrMap[TK_sqrt] = &MIRParser::ParseExprUnary;
  funcPtrMap[TK_alloca] = &MIRParser::ParseExprUnary;
  funcPtrMap[TK_malloc] = &MIRParser::ParseExprUnary;
  funcPtrMap[TK_gcmalloc] = &MIRParser::ParseExprGCMalloc;
  funcPtrMap[TK_gcpermalloc] = &MIRParser::ParseExprGCMalloc;
  funcPtrMap[TK_gcmallocjarray] = &MIRParser::ParseExprJarray;
  funcPtrMap[TK_gcpermallocjarray] = &MIRParser::ParseExprJarray;
  funcPtrMap[TK_sext] = &MIRParser::ParseExprExtractbits;
  funcPtrMap[TK_zext] = &MIRParser::ParseExprExtractbits;
  funcPtrMap[TK_extractbits] = &MIRParser::ParseExprExtractbits;
  funcPtrMap[TK_ceil] = &MIRParser::ParseExprTyconvert;
  funcPtrMap[TK_cvt] = &MIRParser::ParseExprTyconvert;
  funcPtrMap[TK_floor] = &MIRParser::ParseExprTyconvert;
  funcPtrMap[TK_round] = &MIRParser::ParseExprTyconvert;
  funcPtrMap[TK_trunc] = &MIRParser::ParseExprTyconvert;
  funcPtrMap[TK_retype] = &MIRParser::ParseExprRetype;
  funcPtrMap[TK_select] = &MIRParser::ParseExprTernary;
  funcPtrMap[TK_array] = &MIRParser::ParseExprArray;
  funcPtrMap[TK_intrinsicop] = &MIRParser::ParseExprIntrinsicop;
  funcPtrMap[TK_intrinsicopwithtype] = &MIRParser::ParseExprIntrinsicop;
  funcPtrMap[TK_constval] = &MIRParser::ParseExprConstval;
  funcPtrMap[TK_conststr] = &MIRParser::ParseExprConststr;
  funcPtrMap[TK_conststr16] = &MIRParser::ParseExprConststr16;
  funcPtrMap[TK_sizeoftype] = &MIRParser::ParseExprSizeoftype;
  funcPtrMap[TK_fieldsdist] = &MIRParser::ParseExprFieldsDist;
  funcPtrMap[TK_iaddrof] = &MIRParser::ParseExprIaddrof;
  funcPtrMap[TK_iread] = &MIRParser::ParseExprIread;
  funcPtrMap[TK_ireadoff] = &MIRParser::ParseExprIreadoff;
  funcPtrMap[TK_ireadfpoff] = &MIRParser::ParseExprIreadFPoff;
  funcPtrMap[TK_dread] = &MIRParser::ParseExprDread;
  funcPtrMap[TK_regread] = &MIRParser::ParseExprRegread;
  funcPtrMap[TK_add] = &MIRParser::ParseExprBinary;
  funcPtrMap[TK_ashr] = &MIRParser::ParseExprBinary;
  funcPtrMap[TK_band] = &MIRParser::ParseExprBinary;
  funcPtrMap[TK_bior] = &MIRParser::ParseExprBinary;
  funcPtrMap[TK_bxor] = &MIRParser::ParseExprBinary;
  funcPtrMap[TK_cand] = &MIRParser::ParseExprBinary;
  funcPtrMap[TK_cior] = &MIRParser::ParseExprBinary;
  funcPtrMap[TK_div] = &MIRParser::ParseExprBinary;
  funcPtrMap[TK_land] = &MIRParser::ParseExprBinary;
  funcPtrMap[TK_lior] = &MIRParser::ParseExprBinary;
  funcPtrMap[TK_lshr] = &MIRParser::ParseExprBinary;
  funcPtrMap[TK_max] = &MIRParser::ParseExprBinary;
  funcPtrMap[TK_min] = &MIRParser::ParseExprBinary;
  funcPtrMap[TK_mul] = &MIRParser::ParseExprBinary;
  funcPtrMap[TK_rem] = &MIRParser::ParseExprBinary;
  funcPtrMap[TK_shl] = &MIRParser::ParseExprBinary;
  funcPtrMap[TK_sub] = &MIRParser::ParseExprBinary;
  funcPtrMap[TK_CG_array_elem_add] = &MIRParser::ParseExprBinary;
  funcPtrMap[TK_cmp] = &MIRParser::ParseExprCompare;
  funcPtrMap[TK_cmpl] = &MIRParser::ParseExprCompare;
  funcPtrMap[TK_cmpg] = &MIRParser::ParseExprCompare;
  funcPtrMap[TK_eq] = &MIRParser::ParseExprCompare;
  funcPtrMap[TK_ge] = &MIRParser::ParseExprCompare;
  funcPtrMap[TK_gt] = &MIRParser::ParseExprCompare;
  funcPtrMap[TK_le] = &MIRParser::ParseExprCompare;
  funcPtrMap[TK_lt] = &MIRParser::ParseExprCompare;
  funcPtrMap[TK_ne] = &MIRParser::ParseExprCompare;
  funcPtrMap[TK_depositbits] = &MIRParser::ParseExprDepositbits;
  return funcPtrMap;
}

std::map<TokenKind, MIRParser::FuncPtrParseStmt> MIRParser::InitFuncPtrMapForParseStmt() {
  std::map<TokenKind, MIRParser::FuncPtrParseStmt> funcPtrMap;
  funcPtrMap[TK_dassign] = &MIRParser::ParseStmtDassign;
  funcPtrMap[TK_iassign] = &MIRParser::ParseStmtIassign;
  funcPtrMap[TK_iassignoff] = &MIRParser::ParseStmtIassignoff;
  funcPtrMap[TK_iassignfpoff] = &MIRParser::ParseStmtIassignFPoff;
  funcPtrMap[TK_regassign] = &MIRParser::ParseStmtRegassign;
  funcPtrMap[TK_doloop] = &MIRParser::ParseStmtDoloop;
  funcPtrMap[TK_foreachelem] = &MIRParser::ParseStmtForeachelem;
  funcPtrMap[TK_dowhile] = &MIRParser::ParseStmtDowhile;
  funcPtrMap[TK_if] = &MIRParser::ParseStmtIf;
  funcPtrMap[TK_while] = &MIRParser::ParseStmtWhile;
  funcPtrMap[TK_goto] = &MIRParser::ParseStmtGoto;
  funcPtrMap[TK_brfalse] = &MIRParser::ParseStmtBr;
  funcPtrMap[TK_brtrue] = &MIRParser::ParseStmtBr;
  funcPtrMap[TK_switch] = &MIRParser::ParseStmtSwitch;
  funcPtrMap[TK_rangegoto] = &MIRParser::ParseStmtRangegoto;
  funcPtrMap[TK_multiway] = &MIRParser::ParseStmtMultiway;
  funcPtrMap[TK_call] = &MIRParser::ParseStmtCall;
  funcPtrMap[TK_virtualcall] = &MIRParser::ParseStmtCall;
  funcPtrMap[TK_virtualicall] = &MIRParser::ParseStmtCall;
  funcPtrMap[TK_superclasscall] = &MIRParser::ParseStmtCall;
  funcPtrMap[TK_interfacecall] = &MIRParser::ParseStmtCall;
  funcPtrMap[TK_interfaceicall] = &MIRParser::ParseStmtCall;
  funcPtrMap[TK_customcall] = &MIRParser::ParseStmtCall;
  funcPtrMap[TK_polymorphiccall] = &MIRParser::ParseStmtCall;
  funcPtrMap[TK_callinstant] = &MIRParser::ParseStmtCall;
  funcPtrMap[TK_virtualcallinstant] = &MIRParser::ParseStmtCall;
  funcPtrMap[TK_superclasscallinstant] = &MIRParser::ParseStmtCall;
  funcPtrMap[TK_interfacecallinstant] = &MIRParser::ParseStmtCall;
  funcPtrMap[TK_callassigned] = &MIRParser::ParseStmtCall;
  funcPtrMap[TK_virtualcallassigned] = &MIRParser::ParseStmtCall;
  funcPtrMap[TK_virtualicallassigned] = &MIRParser::ParseStmtCall;
  funcPtrMap[TK_superclasscallassigned] = &MIRParser::ParseStmtCall;
  funcPtrMap[TK_interfacecallassigned] = &MIRParser::ParseStmtCall;
  funcPtrMap[TK_interfaceicallassigned] = &MIRParser::ParseStmtCall;
  funcPtrMap[TK_customcallassigned] = &MIRParser::ParseStmtCall;
  funcPtrMap[TK_polymorphiccallassigned] = &MIRParser::ParseStmtCall;
  funcPtrMap[TK_callinstantassigned] = &MIRParser::ParseStmtCall;
  funcPtrMap[TK_virtualcallinstantassigned] = &MIRParser::ParseStmtCall;
  funcPtrMap[TK_superclasscallinstantassigned] = &MIRParser::ParseStmtCall;
  funcPtrMap[TK_interfacecallinstantassigned] = &MIRParser::ParseStmtCall;
  funcPtrMap[TK_icall] = &MIRParser::ParseStmtIcall;
  funcPtrMap[TK_icallassigned] = &MIRParser::ParseStmtIcallassigned;
  funcPtrMap[TK_intrinsiccall] = &MIRParser::ParseStmtIntrinsiccall;
  funcPtrMap[TK_intrinsiccallassigned] = &MIRParser::ParseStmtIntrinsiccallassigned;
  funcPtrMap[TK_xintrinsiccall] = &MIRParser::ParseStmtIntrinsiccall;
  funcPtrMap[TK_xintrinsiccallassigned] = &MIRParser::ParseStmtIntrinsiccallassigned;
  funcPtrMap[TK_intrinsiccallwithtype] = &MIRParser::ParseStmtIntrinsiccallwithtype;
  funcPtrMap[TK_intrinsiccallwithtypeassigned] = &MIRParser::ParseStmtIntrinsiccallwithtypeassigned;
  funcPtrMap[TK_return] = &MIRParser::ParseNaryStmtReturn;
  funcPtrMap[TK_jstry] = &MIRParser::ParseStmtJsTry;
  funcPtrMap[TK_try] = &MIRParser::ParseStmtTry;
  funcPtrMap[TK_catch] = &MIRParser::ParseStmtCatch;
  funcPtrMap[TK_syncenter] = &MIRParser::ParseNaryStmtSyncEnter;
  funcPtrMap[TK_syncexit] = &MIRParser::ParseNaryStmtSyncExit;
  funcPtrMap[TK_throw] = &MIRParser::ParseUnaryStmtThrow;
  funcPtrMap[TK_decref] = &MIRParser::ParseUnaryStmtDecRef;
  funcPtrMap[TK_incref] = &MIRParser::ParseUnaryStmtIncRef;
  funcPtrMap[TK_decrefreset] = &MIRParser::ParseUnaryStmtDecRefReset;
  funcPtrMap[TK_jscatch] = &MIRParser::ParseStmtMarker;
  funcPtrMap[TK_finally] = &MIRParser::ParseStmtMarker;
  funcPtrMap[TK_cleanuptry] = &MIRParser::ParseStmtMarker;
  funcPtrMap[TK_endtry] = &MIRParser::ParseStmtMarker;
  funcPtrMap[TK_retsub] = &MIRParser::ParseStmtMarker;
  funcPtrMap[TK_membaracquire] = &MIRParser::ParseStmtMarker;
  funcPtrMap[TK_membarrelease] = &MIRParser::ParseStmtMarker;
  funcPtrMap[TK_membarstoreload] = &MIRParser::ParseStmtMarker;
  funcPtrMap[TK_membarstorestore] = &MIRParser::ParseStmtMarker;
  funcPtrMap[TK_gosub] = &MIRParser::ParseStmtGosub;
  funcPtrMap[TK_eval] = &MIRParser::ParseUnaryStmtEval;
  funcPtrMap[TK_free] = &MIRParser::ParseUnaryStmtFree;
  funcPtrMap[TK_assertnonnull] = &MIRParser::ParseUnaryStmtAssertNonNull;
  funcPtrMap[TK_assertge] = &MIRParser::ParseBinaryStmtAssertGE;
  funcPtrMap[TK_assertlt] = &MIRParser::ParseBinaryStmtAssertLT;
  funcPtrMap[TK_label] = &MIRParser::ParseStmtLabel;
  funcPtrMap[TK_LOC] = &MIRParser::ParseLoc;
  funcPtrMap[TK_ALIAS] = &MIRParser::ParseAlias;
  return funcPtrMap;
}

std::map<TokenKind, MIRParser::FuncPtrParseStmtBlock> MIRParser::InitFuncPtrMapForParseStmtBlock() {
  std::map<TokenKind, MIRParser::FuncPtrParseStmtBlock> funcPtrMap;
  funcPtrMap[TK_var] = &MIRParser::ParseStmtBlockForVar;
  funcPtrMap[TK_tempvar] = &MIRParser::ParseStmtBlockForTempVar;
  funcPtrMap[TK_reg] = &MIRParser::ParseStmtBlockForReg;
  funcPtrMap[TK_type] = &MIRParser::ParseStmtBlockForType;
  funcPtrMap[TK_framesize] = &MIRParser::ParseStmtBlockForFrameSize;
  funcPtrMap[TK_upformalsize] = &MIRParser::ParseStmtBlockForUpformalSize;
  funcPtrMap[TK_moduleid] = &MIRParser::ParseStmtBlockForModuleID;
  funcPtrMap[TK_funcsize] = &MIRParser::ParseStmtBlockForFuncSize;
  funcPtrMap[TK_funcid] = &MIRParser::ParseStmtBlockForFuncID;
  funcPtrMap[TK_formalwordstypetagged] = &MIRParser::ParseStmtBlockForFormalWordsTypeTagged;
  funcPtrMap[TK_localwordstypetagged] = &MIRParser::ParseStmtBlockForLocalWordsTypeTagged;
  funcPtrMap[TK_formalwordsrefcounted] = &MIRParser::ParseStmtBlockForFormalWordsRefCounted;
  funcPtrMap[TK_localwordsrefcounted] = &MIRParser::ParseStmtBlockForLocalWordsRefCounted;
  funcPtrMap[TK_funcinfo] = &MIRParser::ParseStmtBlockForFuncInfo;
  return funcPtrMap;
}

void MIRParser::SetSrcPos(StmtNodePtr stmt, uint32 mplNum) {
  stmt->GetSrcPos().SetFileNum(lastFileNum);
  stmt->GetSrcPos().SetLineNum(lastLineNum);
  stmt->GetSrcPos().SetMplLineNum(mplNum);
}
}  // namespace maple
