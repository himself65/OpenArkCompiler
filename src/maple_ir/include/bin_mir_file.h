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
#ifndef MAPLE_IR_INCLUDE_BIN_MIR_FILE_H
#define MAPLE_IR_INCLUDE_BIN_MIR_FILE_H
#include <string>
#include "types_def.h"

namespace maple {
const std::string kBinMirFileID = "HWCMPL";  // for magic in file header
constexpr uint8 kVersionMajor = 0;           // experimental version
constexpr uint8 kVersionMinor = 1;
constexpr int kMagicSize = 7;

enum BinMirFileType {
  kMjsvmFileTypeCmplV1,
  kMjsvmFileTypeCmpl,  // kCmpl v2 is the release version of
  kMjsvmFileTypeUnknown
};

inline uint8 MakeVersionNum(uint8 major, uint8 minor) {
  uint8 mj = major & 0x0Fu;
  uint8 mn = minor & 0x0Fu;
  return (mj << 4) | mn;
}

// file header for binary format kMmpl, 8B in total
// Note the header is different with the specification
struct BinMIRFileHeader {
  char magic[kMagicSize]; // “HWCMPL”, or "HWLOS_"
  uint8 segNum;    // number of segments (e.g. one raw IR file is a segment unit)
  uint8 type;      // enum of type of VM file (e.g. MapleIR, TE)
  uint8 version;   // version of IR format (should be major.minor)
};
}  // namespace maple
#endif  // MAPLE_IR_INCLUDE_BIN_MIR_FILE_H
