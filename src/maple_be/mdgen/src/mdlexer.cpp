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
#include <cerrno>
#include "mdlexer.h"

namespace MDGen {
void MDLexer::PrepareFile(const std::string &mdfileName) {
  mdFileInternal.open(mdfileName);
  if (!mdFileInternal.is_open()) {
    CHECK_FATAL(false, "Open target file failed");
  }
  mdFile = &mdFileInternal;
}

MDTokenKind MDLexer::ReturnError() const {
  maple::LogInfo::MapleLogger() << "Unexpect character at Line" << lineNumber << "\n";
  return kMDError;
}

int MDLexer::ReadOneLine() {
  if (mdFile == nullptr) {
    strLine = "";
    return -1;
  }
  curPos = 0;
  if (!std::getline(*mdFile, strLine)) { /* EOF */
    strLine = "";
    mdFile = nullptr;
    return -1;
  }
  RemoveInValidAtBack();
  return GetStrLineSize();
}

MDTokenKind MDLexer::NextToken() {
  curKind = LexToken();
  return curKind;
}

MDTokenKind MDLexer::LexToken() {
  char c = GetCurChar();
  while (c == ' ' || c == '\t') { /* skip space && tab */
    c = GetNextChar();
  }
  while (c == 0) {
    if (ReadOneLine() < 0) {
      return kMDEOF;
    }
    lineNumber++;
    c = GetCurChar();
    while (c == ' ' || c == '\t') {
      c = GetNextChar();
    }
  }
  curPos++;
  switch (c) {
    case '(':
      return kMDOpenParen;
    case ')':
      return kMDCloseParen;
    case '{':
      return kMDOpenBrace;
    case '}':
      return kMDCloseBrace;
    case '[':
      return kMDOpenSquare;
    case ']':
      return kMDCloseSquare;
    case '<':
      return kMDLess;
    case '>':
      return kMDGreater;
    case ';':
      return kMDSemi;
    case ',':
      return kMDComma;
    case ':':
      return kMDColon;
    case '=':
      return kMDEqual;
    case '&':
      return kMDLgAnd;
    case '|':
      return kMDLgOr;
    case '0': /* start handling number */
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '-':
      curPos--; /* support HEX AND INTERGER at present */
      return GetTokenConstVal();
    case '/': { /* handle comment; */
      char cn = GetCurChar();
      if (cn == '/') {
        SkipALineComment();
      } else if (cn == '*') {
        if (!SkipCComment()) {
          return kMDError;
        }
      } else {
        return ReturnError();
      }
      return LexToken();
    }
    default:
      if (isalpha(c) || c == '_') {
        return GetTokenIdentifier(); /* identifier need to be modify */
      }
      return ReturnError();
  }
}

MDTokenKind MDLexer::GetTokenConstVal() {
  bool negative = false;
  char curC = GetCurChar();
  if (curC == '-') {
    curC = GetNextChar();
    /* have Special Float const? */
    negative = true;
  }
  const uint32 hexPrefixLength = 2;
  if (strLine.compare(curPos, hexPrefixLength, "0x") == 0) {
    curPos += hexPrefixLength;
    return GetHexConst(curPos, negative);
  }
  uint32 digitStartPos = curPos;
  char digitStartC = GetCurChar();
  while (isdigit(curC)) {
    curC = GetNextChar();
  }
  if (!isdigit(digitStartC) && curC != '.') {
    return kMDInvalid;
  }
  if (curC != '.' && curC != 'e' && curC != 'E') {
    return  GetIntConst(digitStartPos, negative);
  }
  return GetFloatConst();
}

MDTokenKind MDLexer::GetHexConst(uint32 digitStartPos, bool isNegative) {
  if (digitStartPos >= strLine.length()) {
    return ReturnError();
  }
  char c = GetCurChar();
  if (!isxdigit(c)) {
    return kMDInvalid;
  }
  int loopDepth = 0;
  while (isxdigit(c)) {
    c = GetNextChar();
    ++loopDepth;
    if (loopDepth > maxNumLength) {
      return ReturnError();
    }
  }
  std::string hexStr = strLine.substr(digitStartPos, curPos - digitStartPos);
  const char *hexStrPtr = hexStr.c_str();
  errno = 0;
  constexpr int hexInDec = 16;
  intVal = static_cast<int32>(std::strtoll(hexStrPtr, nullptr, hexInDec));
  if (errno == EINVAL) {  /* Invalid hexadecimal number */
    return ReturnError();
  }
  if (errno == ERANGE) {
    errno = 0;
    intVal = static_cast<uint32>(std::strtoll(hexStrPtr, nullptr, hexInDec));
    if (errno == EINVAL) { /* Invalid hexadecimal number */
      return ReturnError();
    }
    if (errno == ERANGE) {  /* input number is out of range */
      return ReturnError();
    }
  }
  if (isNegative) {
    intVal = -intVal;
  }
  return kMDIntVal;
}

MDTokenKind MDLexer::GetIntConst(uint32 digitStartPos, bool isNegative) {
  char c = GetCharAt(digitStartPos);
  /* no ULL LL suffix at present */
  int loopDepth = 0;
  while (isdigit(c)) {
    c = GetNextChar();
    ++loopDepth;
    if (loopDepth > maxNumLength) {
      return ReturnError();
    }
  }
  curPos--;
  if (digitStartPos >= strLine.length() || digitStartPos > curPos) {
    return ReturnError();
  }
  std::string intStr = strLine.substr(digitStartPos, curPos - digitStartPos);
  const char *intStrPtr = intStr.c_str();
  errno = 0;
  constexpr int decInDec = 10;
  intVal = static_cast<int32>(std::strtoll(intStrPtr, nullptr, decInDec));
  if (errno == ERANGE) {
    return ReturnError();
  }
  if (isNegative) {
    intVal = -intVal;
  }
  return kMDIntVal;
}

MDTokenKind MDLexer::GetFloatConst() {
  floatVal = 0;
  return kMDInvalid;
}

MDTokenKind MDLexer::GetTokenIdentifier() {
  --curPos;
  uint32 startPos = curPos;
  char curC = GetCurChar();

  while (isalnum(curC) || curC == '_' || curC == '-' || curC < 0) {
    curC = GetNextChar();
  }
  if (startPos >= strLine.length()) {
    return ReturnError();
  }
  strToken = strLine.substr(startPos, curPos - startPos);
  auto it = keywords.find(strToken);
  if (it != keywords.end()) {
    return it->second;
  }
  return kMDIdentifier;
}

void MDLexer::SkipALineComment() {
  while (curPos < GetStrLineSize()) {
    curPos++;
  }
  /* if comment is required to be stored. it can be done here */
}

bool MDLexer::SkipCComment() {
  bool startAnewLine = false;
  char commentNext;
  while (true) {
    if (!startAnewLine) {
      commentNext = GetNextChar();
    } else {
      commentNext = GetCurChar();
      startAnewLine = false;
    }
    switch (commentNext) {
      case 0:
        if (ReadOneLine() < 0) {
          ASSERT(false, "Untermianted comment");
          return false;
        }
        ++lineNumber;
        startAnewLine = true;
        break;
      case '*':
        commentNext = GetNextChar();
        if (commentNext == '/') {
          ++curPos;
          return true;
        }
        break;
      default:
        break;
    }
  }
  return false;
}
}
