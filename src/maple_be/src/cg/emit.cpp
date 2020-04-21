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
#include "emit.h"
#include <unistd.h>
#ifdef _WIN32
#include <direct.h>
#endif
#include "reflection_analysis.h"
#include "muid_replacement.h"
#include "metadata_layout.h"
#include "string_utils.h"
using namespace NameMangler;

namespace {
using namespace maple;
constexpr uint32 kSizeOfHugesoRoutine = 3;

int32 GetPrimitiveTypeSize(const std::string &name) {
  if (name.length() != 1) {
    return -1;
  }
  char typeName = name[0];
  switch (typeName) {
    case 'Z':
      return static_cast<int32>(GetPrimTypeSize(PTY_u1));
    case 'B':
      return static_cast<int32>(GetPrimTypeSize(PTY_i8));
    case 'S':
      return static_cast<int32>(GetPrimTypeSize(PTY_i16));
    case 'C':
      return static_cast<int32>(GetPrimTypeSize(PTY_u16));
    case 'I':
      return static_cast<int32>(GetPrimTypeSize(PTY_i32));
    case 'J':
      return static_cast<int32>(GetPrimTypeSize(PTY_i64));
    case 'F':
      return static_cast<int32>(GetPrimTypeSize(PTY_f32));
    case 'D':
      return static_cast<int32>(GetPrimTypeSize(PTY_f64));
    case 'V':
      return static_cast<int32>(GetPrimTypeSize(PTY_void));
    default:
      return -1;
  }
}
}

namespace maplebe {
using namespace maple;
using namespace cfi;

void Emitter::EmitLabelRef(const std::string &name, LabelIdx labIdx) {
  outStream << ".Label." << name << "." << labIdx;
}

void Emitter::EmitStmtLabel(const std::string &name, LabelIdx labIdx) {
  EmitLabelRef(name, labIdx);
  outStream << ":\n";
}

void Emitter::EmitLabelPair(const std::string &name, const LabelPair &pairLabel) {
  ASSERT(pairLabel.GetEndOffset() || pairLabel.GetStartOffset(), "NYI");
  outStream << ".Label." << name << "." << pairLabel.GetEndOffset()->GetLabelIdx() << " - "
            << ".Label." << name << "." << pairLabel.GetStartOffset()->GetLabelIdx() << "\n";
}

AsmLabel Emitter::GetTypeAsmInfoName(PrimType primType) const {
  uint32 size = GetPrimTypeSize(primType);
  /* case x : x occupies bytes of pty */
  switch (size) {
    case k1ByteSize:
      return kAsmByte;
    case k2ByteSize:
#if TARGAARCH64
      return kAsmShort;
#else
      return kAsmValue;
#endif
    case k4ByteSize:
      return kAsmLong;
    case k8ByteSize:
      return kAsmQuad;
    default:
      ASSERT(false, "NYI");
      break;
  }
  return kAsmLong;
}

void Emitter::EmitFileInfo(const std::string &fileName) {
#if defined(_WIN32) || defined(DARWIN)
  char *curDirName = getcwd(nullptr, 0);
#else
  char *curDirName = get_current_dir_name();
#endif
  CHECK_FATAL(curDirName != nullptr, "null ptr check ");
  Emit(asmInfo->GetCmnt());
  std::string path(curDirName);
#ifdef _WIN32
  std::string cgFile(path.append("\\mplcg"));
#else
  std::string cgFile(path.append("/mplcg"));
#endif
  Emit(cgFile);
  Emit("\n");

  std::string compile("Compiling ");
  Emit(asmInfo->GetCmnt());
  Emit(compile);
  Emit("\n");

  std::string beOptions("Be options");
  Emit(asmInfo->GetCmnt());
  Emit(beOptions);
  Emit("\n");

  path = curDirName;
  path.append("/").append(fileName);
  /* strip path before out/ */
  std::string out = "/out/";
  size_t pos = path.find(out.c_str(), 0, out.length());
  if (pos != std::string::npos) {
    path.erase(0, pos + 1);
  }
  std::string irFile("\"");
  irFile.append(path).append("\"");
  Emit(asmInfo->GetFile());
  Emit(irFile);
  Emit("\n");

  /* .file #num src_file_name */
  if (cg->GetCGOptions().WithLoc()) {
    /* .file 1 mpl_file_name */
    if (cg->GetCGOptions().WithAsm()) {
      Emit("\t// ");
    }
    Emit(asmInfo->GetFile());
    Emit("1 ");
    Emit(irFile);
    Emit("\n");
    if (cg->GetCGOptions().WithSrc()) {
      /* insert a list of src files */
      for (auto it : cg->GetMIRModule()->GetSrcFileInfo()) {
        if (cg->GetCGOptions().WithAsm()) {
          Emit("\t// ");
        }
        Emit(asmInfo->GetFile());
        Emit(it.second).Emit(" \"");
        const std::string kStr = GlobalTables::GetStrTable().GetStringFromStrIdx(it.first);
        Emit(kStr);
        Emit("\"\n");
      }
    }
  }
  free(curDirName);
#if TARGARM32
  Emit("\t.syntax unified\n");
  /*
   * "The arm instruction set is a subset of
   *  the most commonly used 32-bit ARM instructions."
   * http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0210c/CACBCAAE.html
   */
  Emit("\t.arm\n");
  Emit("\t.fpu vfpv4\n");
  Emit("\t.arch armv7-a\n");
  Emit("\t.eabi_attribute 15, 1\n");
  Emit("\t.eabi_attribute 16, 1\n");
  Emit("\t.eabi_attribute 17, 2\n");
  Emit("\t.eabi_attribute 28, 1\n");
  Emit("\t.eabi_attribute 20, 1\n");
  Emit("\t.eabi_attribute 21, 1\n");
  Emit("\t.eabi_attribute 23, 3\n");
  Emit("\t.eabi_attribute 24, 1\n");
  Emit("\t.eabi_attribute 25, 1\n");
  Emit("\t.eabi_attribute 26, 2\n");
  Emit("\t.eabi_attribute 30, 6\n");
  Emit("\t.eabi_attribute 34, 1\n");
  Emit("\t.eabi_attribute 18, 4\n");
#endif /* TARGARM32 */
}

void Emitter::EmitAsmLabel(AsmLabel label) {
  switch (label) {
    case kAsmData: {
      Emit(asmInfo->GetData());
      Emit("\n");
      return;
    }
    case kAsmByte: {
      Emit(asmInfo->GetByte());
      return;
    }
    case kAsmShort: {
      Emit(asmInfo->GetShort());
      return;
    }
    case kAsmValue: {
      Emit(asmInfo->GetValue());
      return;
    }
    case kAsmLong: {
      Emit(asmInfo->GetLong());
      return;
    }
    case kAsmQuad: {
      Emit(asmInfo->GetQuad());
      return;
    }
    case kAsmZero:
      Emit(asmInfo->GetZero());
      return;
    default:
      ASSERT(false, "should not run here");
      return;
  }
}

void Emitter::EmitAsmLabel(const MIRSymbol &mirSymbol, AsmLabel label) {
  MIRType *mirType = mirSymbol.GetType();
  const std::string &symName = mirSymbol.GetName();

  if (Globals::GetInstance()->GetBECommon()->IsEmptyOfTypeAlignTable()) {
    ASSERT(false, "container empty check");
  }
#if TARGARM32 || TARGAARCH64
  std::string align = std::to_string(
      static_cast<int>(log2(Globals::GetInstance()->GetBECommon()->GetTypeAlign(mirType->GetTypeIndex()))));
#else
  std::string align = std::to_string(
      static_cast<int>(log2(Globals::GetInstance()->GetBECommon()->GetTypeAlign(mirType->GetTypeIndex()))));
#endif
  std::string size = std::to_string(
      Globals::GetInstance()->GetBECommon()->GetTypeSize(mirType->GetTypeIndex()));
  switch (label) {
    case kAsmGlbl: {
      Emit(asmInfo->GetGlobal());
      Emit(symName);
      Emit("\n");
      return;
    }
    case kAsmHidden: {
      Emit(asmInfo->GetHidden());
      Emit(symName);
      Emit("\n");
      return;
    }
    case kAsmLocal: {
      Emit(asmInfo->GetLocal());
      Emit(symName);
      Emit("\n");
      return;
    }
    case kAsmWeak: {
      Emit(asmInfo->GetWeak());
      Emit(symName);
      Emit("\n");
      return;
    }
    case kAsmComm: {
      Emit(asmInfo->GetComm());
      Emit(symName);
      Emit(", ");
      Emit(size);
      Emit(", ");
#if PECOFF
      emit(align);
#else /* ELF */
      /* output align, symbol name begin with "classInitProtectRegion" align is 4096 */
      if (symName.find("classInitProtectRegion") == 0) {
        Emit(4096);
      } else {
        Emit(std::to_string(Globals::GetInstance()->GetBECommon()->GetTypeAlign(mirType->GetTypeIndex())));
      }
#endif
      Emit("\n");
      return;
    }
    case kAsmAlign: {
      Emit(asmInfo->GetAlign());
      Emit(align);
      Emit("\n");
      return;
    }
    case kAsmSyname: {
      Emit(symName);
      Emit(":\n");
      return;
    }
    case kAsmSize: {
      Emit(asmInfo->GetSize());
      Emit(symName);
      Emit(", ");
      Emit(size);
      Emit("\n");
      return;
    }
    case kAsmType: {
      Emit(asmInfo->GetType());
      Emit(symName);
      Emit(",");
      Emit(asmInfo->GetAtobt());
      Emit("\n");
      return;
    }
    default:
      ASSERT(false, "should not run here");
      return;
  }
}

void Emitter::EmitNullConstant(uint32 size) {
  EmitAsmLabel(kAsmZero);
  Emit(std::to_string(size));
  Emit("\n");
}

void Emitter::EmitCombineBfldValue(StructEmitInfo &structEmitInfo) {
  uint8 charBitWidth = GetPrimTypeSize(PTY_i8) * kBitsPerByte;
  while (structEmitInfo.GetCombineBitFieldWidth() > charBitWidth) {
    /* fetch the lower 8 bits */
    uint64 tmp = structEmitInfo.GetCombineBitFieldValue() & 0x00000000000000ffUL;
    EmitAsmLabel(kAsmByte);
    Emit(std::to_string(tmp));
    Emit("\n");
    structEmitInfo.DecreaseCombineBitFieldWidth(charBitWidth);
    structEmitInfo.SetCombineBitFieldValue(structEmitInfo.GetCombineBitFieldValue() >> charBitWidth);
  }
  if (structEmitInfo.GetCombineBitFieldWidth() != 0) {
    EmitAsmLabel(kAsmByte);
    Emit(std::to_string(structEmitInfo.GetCombineBitFieldValue()));
    Emit("\n");
  }
  CHECK_FATAL(charBitWidth != 0, "divide by zero");
  if ((structEmitInfo.GetNextFieldOffset() % charBitWidth) != 0) {
    uint8 value = charBitWidth - (structEmitInfo.GetNextFieldOffset() % charBitWidth);
    structEmitInfo.IncreaseNextFieldOffset(value);
  }
  structEmitInfo.SetTotalSize(structEmitInfo.GetNextFieldOffset() / charBitWidth);
  structEmitInfo.SetCombineBitFieldValue(0);
  structEmitInfo.SetCombineBitFieldWidth(0);
}

void Emitter::EmitBitFieldConstant(StructEmitInfo &structEmitInfo, MIRConst &mirConst, const MIRType *nextType,
                                   uint64 fieldOffset) {
  MIRType &mirType = mirConst.GetType();
  if (fieldOffset > structEmitInfo.GetNextFieldOffset()) {
    uint8 curFieldOffset = structEmitInfo.GetNextFieldOffset() - structEmitInfo.GetCombineBitFieldWidth();
    structEmitInfo.SetCombineBitFieldWidth(fieldOffset - curFieldOffset);
    EmitCombineBfldValue(structEmitInfo);
    ASSERT(structEmitInfo.GetNextFieldOffset() <= fieldOffset,
           "structEmitInfo's nextFieldOffset should be <= fieldOffset");
    structEmitInfo.SetNextFieldOffset(fieldOffset);
  }
  uint32 fieldSize = static_cast<MIRBitFieldType&>(mirType).GetFieldSize();
  MIRIntConst &fieldValue = static_cast<MIRIntConst&>(mirConst);
  /* Truncate the size of FieldValue to the bit field size. */
  if (fieldSize < fieldValue.GetBitWidth()) {
    fieldValue.Trunc(fieldSize);
  }
  structEmitInfo.SetCombineBitFieldValue(
      (static_cast<uint64>(fieldValue.GetValue()) << structEmitInfo.GetCombineBitFieldWidth()) +
      structEmitInfo.GetCombineBitFieldValue());
  structEmitInfo.IncreaseCombineBitFieldWidth(fieldSize);
  structEmitInfo.IncreaseNextFieldOffset(fieldSize);
  if ((nextType == nullptr) || (kTypeBitField != nextType->GetKind())) {
    /* emit structEmitInfo->combineBitFieldValue */
    EmitCombineBfldValue(structEmitInfo);
  }
}

void Emitter::EmitStrConstant(const MIRStrConst &mirStrConst) {
  Emit("\t.string \"");
  /*
   * don't expand special character in a writeout to .s,
   * convert all \s to \\s in std::string for storing in .string
   */
  const char *str = GlobalTables::GetUStrTable().GetStringFromStrIdx(mirStrConst.GetValue()).c_str();
  constexpr int bufSize = 6;
  while (*str) {
    char buf[bufSize];
    if (isprint(*str)) {
      buf[0] = *str;
      buf[1] = 0;
      Emit(buf);
    } else if (*str == '\n') {
      Emit("\\n");
    } else if (*str == '\t') {
      Emit("\\t");
    } else {
      /*
       * all others, print as number
       * fetch  str's lower 8 bit data
       */
      int ret = snprintf_s(buf, sizeof(buf), bufSize - 1, "\\\\x%02x",
                           static_cast<uint32>(static_cast<unsigned char>(*str)) & 0xFF);
      if (ret < 0) {
        FATAL(kLncFatal, "snprintf_s failed");
      }
      buf[bufSize - 1] = '\0';
      Emit(buf);
    }
    str++;
  }
  Emit("\"");
}

void Emitter::EmitStr16Constant(const MIRStr16Const &mirStr16Const) {
  Emit("\t.byte ");
  /* note: for now, u16string is emitted 2 bytes without any \u indication */
  const std::u16string &str16 = GlobalTables::GetU16StrTable().GetStringFromStrIdx(mirStr16Const.GetValue());
  constexpr int bufSize = 9;
  char buf[bufSize];
  char16_t c = str16[0];
  /* fetch the type of char16_t c's top 8 bit data */
  int ret1 = snprintf_s(buf, sizeof(buf), bufSize - 1, "%d,%d", (c >> 8) & 0xFF, c & 0xFF);
  if (ret1 < 0) {
    FATAL(kLncFatal, "snprintf_s failed");
  }
  buf[bufSize - 1] = '\0';
  Emit(buf);
  for (uint32 i = 1; i < str16.length(); ++i) {
    c = str16[i];
    /* fetch the type of char16_t c's top 8 bit data */
    int ret2 = snprintf_s(buf, sizeof(buf), bufSize - 1, ",%d,%d", (c >> 8) & 0xFF, c & 0xFF);
    if (ret2 < 0) {
      FATAL(kLncFatal, "snprintf_s failed");
    }
    buf[bufSize - 1] = '\0';
    Emit(buf);
  }
  if ((str16.length() & 0x1) == 1) {
    Emit(",0,0");
  }
}

void Emitter::EmitScalarConstant(MIRConst &mirConst, bool newLine, bool flag32) {
  MIRType &mirType = mirConst.GetType();
  AsmLabel asmName = GetTypeAsmInfoName(mirType.GetPrimType());
  switch (mirConst.GetKind()) {
    case kConstInt: {
      MIRIntConst &intCt = static_cast<MIRIntConst&>(mirConst);
      uint32 sizeInBits = GetPrimTypeBitSize(mirType.GetPrimType());
      if (intCt.GetBitWidth() > sizeInBits) {
        intCt.Trunc(sizeInBits);
      }
      if (flag32) {
        EmitAsmLabel(AsmLabel::kAsmLong);
      } else {
        EmitAsmLabel(asmName);
      }
      Emit(intCt.GetValue());
      break;
    }
    case kConstFloatConst: {
      MIRFloatConst &floatCt = static_cast<MIRFloatConst&>(mirConst);
      EmitAsmLabel(asmName);
      Emit(std::to_string(floatCt.GetIntValue()));
      break;
    }
    case kConstDoubleConst: {
      MIRDoubleConst &doubleCt = static_cast<MIRDoubleConst&>(mirConst);
      EmitAsmLabel(asmName);
      Emit(std::to_string(doubleCt.GetIntValue()));
      break;
    }
    case kConstStrConst: {
      MIRStrConst &strCt = static_cast<MIRStrConst&>(mirConst);
      EmitStrConstant(strCt);
      break;
    }
    case kConstStr16Const: {
      MIRStr16Const &str16Ct = static_cast<MIRStr16Const&>(mirConst);
      EmitStr16Constant(str16Ct);
      break;
    }
    case kConstAddrof: {
      MIRAddrofConst &symAddr = static_cast<MIRAddrofConst&>(mirConst);
      MIRSymbol *symAddrSym = GlobalTables::GetGsymTable().GetSymbolFromStidx(symAddr.GetSymbolIndex().Idx());
      ASSERT(symAddrSym != nullptr, "null ptr check");
      EmitAddressString(symAddrSym->GetName());
      break;
    }
    default:
      ASSERT(false, "NYI");
      break;
  }
  if (newLine) {
    Emit("\n");
  }
}

void Emitter::EmitAddrofFuncConst(const MIRSymbol &mirSymbol, MIRConst &elemConst, size_t idx) {
  MIRAddroffuncConst &funcAddr = static_cast<MIRAddroffuncConst&>(elemConst);
  const std::string stName = mirSymbol.GetName();
  MIRFunction *func = GlobalTables::GetFunctionTable().GetFunctionFromPuidx(funcAddr.GetValue());
  const std::string &funcName = func->GetName();
  if ((idx == kFuncDefNameIndex) && mirSymbol.IsMuidFuncInfTab()) {
    Emit("\t.long\t.Label.name.");
    Emit(funcName + " - .");
    Emit("\n");
    return;
  }
  if ((idx == kFuncDefSizeIndex) && mirSymbol.IsMuidFuncInfTab()) {
    Emit("\t.long\t.Label.end.");
    Emit(funcName + " - ");
    Emit(funcName + "\n");
    return;
  }
  if ((idx == static_cast<uint32>(MethodProperty::kPaddrData)) && mirSymbol.IsReflectionMethodsInfo()) {
#ifdef USE_32BIT_REF
    Emit("\t.long\t");
#else

#if TARGAARCH64
    Emit("\t.quad\t");
#else
    Emit("\t.word\t");
#endif

#endif  /* USE_32BIT_REF */
    Emit(funcName + " - .\n");
    return;
  }
  if (((idx == static_cast<uint32>(MethodInfoCompact::kPaddrData)) && mirSymbol.IsReflectionMethodsInfoCompact()) ||
      ((idx == static_cast<uint32>(ClassRO::kClinitAddr)) && mirSymbol.IsReflectionClassInfoRO())) {
    Emit("\t.long\t");
    Emit(funcName + " - .\n");
    return;
  }

  if (mirSymbol.IsReflectionMethodAddrData()) {
#ifdef USE_32BIT_REF
    Emit("\t.long\t");
#else

#if TARGAARCH64
    Emit("\t.quad\t");
#else
    Emit("\t.word\t");
#endif

#endif  /* USE_32BIT_REF */
    Emit(funcName + " - .\n");
    return;
  }

  if (idx == kFuncDefAddrIndex && mirSymbol.IsMuidFuncDefTab()) {
#if defined(USE_32BIT_REF)
    Emit("\t.long\t");
#else

#if TARGAARCH64
    Emit("\t.quad\t");
#else
    Emit("\t.word\t");
#endif

#endif  /* USE_32BIT_REF */
    if (CGOptions::IsLazyBinding() && !cg->IsLibcore()) {
      /*
       * Check enum BindingState defined in Mpl_Binding.h,
       * 6 means kBindingStateMethodDef:6 offset away from base __BindingProtectRegion__.
       */
#if defined(USE_32BIT_REF)
      Emit("0x6\n"); /* Fix it in runtime, __BindingProtectRegion__ + kBindingStateMethodDef:6. */
#else
      Emit("__BindingProtectRegion__ + 6\n");
#endif  /* USE_32BIT_REF */
    } else {
#if defined(USE_32BIT_REF)
#if defined(MPL_LNK_ADDRESS_VIA_BASE)
      Emit(funcName + "\n");
#else  /* MPL_LNK_ADDRESS_VIA_BASE */
      Emit(funcName + "-.\n");
#endif /* MPL_LNK_ADDRESS_VIA_BASE */
#else  /* USE_32BIT_REF */
      Emit(funcName + "\n");
#endif /* USE_32BIT_REF */
    }
    return;
  }

  if (idx == kFuncDefAddrIndex && mirSymbol.IsMuidFuncDefOrigTab()) {
    if (CGOptions::IsLazyBinding() && !cg->IsLibcore()) {
#if defined(USE_32BIT_REF)
      Emit("\t.long\t");
#else

#if TARGAARCH64
      Emit("\t.quad\t");
#else
      Emit("\t.word\t");
#endif

#endif  /* USE_32BIT_REF */
#if defined(USE_32BIT_REF)
#if defined(MPL_LNK_ADDRESS_VIA_BASE)
      Emit(funcName + "\n");
#else /* MPL_LNK_ADDRESS_VIA_BASE */
      Emit(funcName + "-.\n");
#endif /* MPL_LNK_ADDRESS_VIA_BASE */
#else /* USE_32BIT_REF */
      Emit(funcName + "\n");
#endif /* USE_32BIT_REF */
    }
    return;
  }

#if TARGAARCH64
    Emit("\t.quad\t");
#else
    Emit("\t.word\t");
#endif
  Emit(funcName);
  if ((stName.find(VTAB_PREFIX_STR) == 0) || (stName.find(ITAB_PREFIX_STR) == 0) ||
      (stName.find(ITAB_CONFLICT_PREFIX_STR) == 0)) {
    Emit(" - .\n");
    return;
  }
  if (cg->GetCGOptions().GeneratePositionIndependentExecutable()) {
    Emit(" - ");
    Emit(stName);
  }
  Emit("\n");
}

void Emitter::EmitAddrofSymbolConst(const MIRSymbol &mirSymbol, MIRConst &elemConst, size_t idx) {
  MIRAddrofConst &symAddr = static_cast<MIRAddrofConst&>(elemConst);
  const std::string stName = mirSymbol.GetName();

  MIRSymbol *symAddrSym = GlobalTables::GetGsymTable().GetSymbolFromStidx(symAddr.GetSymbolIndex().Idx());
  const std::string &symAddrName = symAddrSym->GetName();

  if (((idx == static_cast<uint32>(FieldProperty::kPOffset)) && mirSymbol.IsReflectionFieldsInfo()) ||
      mirSymbol.IsReflectionFieldOffsetData()) {
#if USE_32BIT_REF
    Emit("\t.long\t");
#else

#if TARGAARCH64
    Emit("\t.quad\t");
#else
    Emit("\t.word\t");
#endif

#endif  /* USE_32BIT_REF */
    Emit(symAddrName + " - .\n");
    return;
  }

  if ((idx == static_cast<uint32>(FieldPropertyCompact::kPOffset)) && mirSymbol.IsReflectionFieldsInfoCompact()) {
    Emit("\t.long\t");
    Emit(symAddrName + " - .\n");
    return;
  }

  if (((idx == static_cast<uint32>(MethodProperty::kDeclarclass)) ||
      (idx == static_cast<uint32>(MethodProperty::kPaddrData))) && mirSymbol.IsReflectionMethodsInfo()) {
#if USE_32BIT_REF
    Emit("\t.long\t");
#else

#if TARGAARCH64
    Emit("\t.quad\t");
#else
    Emit("\t.word\t");
#endif

#endif  /* USE_32BIT_REF */
    if (idx == static_cast<uint32>(MethodProperty::kDeclarclass)) {
      Emit(symAddrName + " - .\n");
    } else {
      Emit(symAddrName + " - . + 2\n");
    }
    return;
  }

  if ((idx == static_cast<uint32>(MethodInfoCompact::kPaddrData)) && mirSymbol.IsReflectionMethodsInfoCompact()) {
    Emit("\t.long\t");
    Emit(symAddrName + " - . + 2\n");
    return;
  }

  if ((idx == static_cast<uint32>(FieldProperty::kDeclarclass)) && mirSymbol.IsReflectionFieldsInfo()) {
#if TARGAARCH64
    Emit("\t.quad\t");
#else
    Emit("\t.word\t");
#endif
    Emit(symAddrName + " - .\n");
    return;
  }

  if ((idx == kDataDefAddrIndex) && (mirSymbol.IsMuidDataUndefTab() || mirSymbol.IsMuidDataDefTab())) {
    if (symAddrSym->IsReflectionClassInfo()) {
      Emit(".LDW.ref." + symAddrName + ":\n");
    }
    Emit(kPtrPrefixStr + symAddrName + ":\n");
#if defined(USE_32BIT_REF)
    Emit("\t.long\t");
#else

#if TARGAARCH64
    Emit("\t.quad\t");
#else
    Emit("\t.word\t");
#endif

#endif  /* USE_32BIT_REF */
    if (mirSymbol.IsMuidDataUndefTab()) {
      if (CGOptions::IsLazyBinding() && !cg->IsLibcore()) {
        if (symAddrSym->IsReflectionClassInfo()) {
          /*
           * Check enum BindingState defined in Mpl_Binding.h,
           * 1 means kBindingStateCinfUndef:1 offset away from base __BindingProtectRegion__.
           */
#if defined(USE_32BIT_REF)
          Emit("0x1\n"); /* Fix it in runtime, __BindingProtectRegion__ + kBindingStateCinfUndef:1. */
#else
          Emit("__BindingProtectRegion__ + 1\n");
#endif  /* USE_32BIT_REF */
        } else {
          /*
           * Check enum BindingState defined in Mpl_Binding.h,
           * 3 means kBindingStateDataUndef:3 offset away from base __BindingProtectRegion__.
           */
#if defined(USE_32BIT_REF)
          Emit("0x3\n"); /* Fix it in runtime, __BindingProtectRegion__ + kBindingStateDataUndef:3. */
#else
          Emit("__BindingProtectRegion__ + 3\n");
#endif  /* USE_32BIT_REF */
        }
      } else {
        Emit("0\n");
      }
    } else {
      if (CGOptions::IsLazyBinding() && !cg->IsLibcore()) {
        if (symAddrSym->IsReflectionClassInfo()) {
          /*
           * Check enum BindingState defined in Mpl_Binding.h,
           * 2 means kBindingStateCinfDef:2 offset away from base __BindingProtectRegion__.
           */
#if defined(USE_32BIT_REF)
          Emit("0x2\n"); /* Fix it in runtime, __BindingProtectRegion__ + kBindingStateCinfDef:2. */
#else
          Emit("__BindingProtectRegion__ + 2\n");
#endif  /* USE_32BIT_REF */
        } else {
          /*
           * Check enum BindingState defined in Mpl_Binding.h,
           * 4 means kBindingStateDataDef:4 offset away from base __BindingProtectRegion__.
           */
#if defined(USE_32BIT_REF)
          Emit("0x4\n"); /* Fix it in runtime, __BindingProtectRegion__ + kBindingStateDataDef:4. */
#else
          Emit("__BindingProtectRegion__ + 4\n");
#endif  /* USE_32BIT_REF */
        }
      } else {
#if defined(USE_32BIT_REF)
#if defined(MPL_LNK_ADDRESS_VIA_BASE)
        Emit(symAddrName + "\n");
#else /* MPL_LNK_ADDRESS_VIA_BASE */
        Emit(symAddrName + "-.\n");
#endif /* MPL_LNK_ADDRESS_VIA_BASE */
#else /* USE_32BIT_REF */
        Emit(symAddrName + "\n");
#endif /* USE_32BIT_REF */
      }
    }
    return;
  }

  if (idx == kDataDefAddrIndex && mirSymbol.IsMuidDataDefOrigTab()) {
    if (CGOptions::IsLazyBinding() && !cg->IsLibcore()) {
#if defined(USE_32BIT_REF)
      Emit("\t.long\t");
#else

#if TARGAARCH64
      Emit("\t.quad\t");
#else
      Emit("\t.word\t");
#endif

#endif  /* USE_32BIT_REF */

#if defined(USE_32BIT_REF)
#if defined(MPL_LNK_ADDRESS_VIA_BASE)
      Emit(symAddrName + "\n");
#else /* MPL_LNK_ADDRESS_VIA_BASE */
      Emit(symAddrName + "-.\n");
#endif /* MPL_LNK_ADDRESS_VIA_BASE */
#else /* USE_32BIT_REF */
      Emit(symAddrName + "\n");
#endif /* USE_32BIT_REF */
    }
    return;
  }

  if (StringUtils::StartsWith(stName, kLocalClassInfoStr)) {
#if TARGAARCH64
    Emit("\t.quad\t");
#else
    Emit("\t.word\t");
#endif
    Emit(symAddrName);
    Emit(" - . + ").Emit(kDataRefIsOffset);
    Emit("\n");
    return;
  }
#ifdef USE_32BIT_REF
  if (mirSymbol.IsReflectionHashTabBucket() || (stName.find(ITAB_PREFIX_STR) == 0) ||
      (mirSymbol.IsReflectionClassInfo() && (idx == static_cast<uint32>(ClassProperty::kInfoRo)))) {
    Emit("\t.word\t");
  } else {
#if TARGAARCH64
    Emit("\t.quad\t");
#else
    Emit("\t.word\t");
#endif
  }
#else

#if TARGAARCH64
    Emit("\t.quad\t");
#else
    Emit("\t.word\t");
#endif

#endif  /* USE_32BIT_REF */

  if ((stName.find(ITAB_CONFLICT_PREFIX_STR) == 0) || (stName.find(ITAB_PREFIX_STR) == 0)) {
    Emit(symAddrName + " - .\n");
    return;
  }
  if (mirSymbol.IsMuidRangeTab()) {
    if (idx == kRangeBeginIndex) {
      Emit(symAddrSym->GetMuidTabName() + "_begin\n");
    } else {
      Emit(symAddrSym->GetMuidTabName() + "_end\n");
    }
    return;
  }

  if (symAddrName.find(GCTIB_PREFIX_STR) == 0) {
    Emit(cg->FindGCTIBPatternName(symAddrName));
  } else {
    Emit(symAddrName);
  }

  if ((((idx == static_cast<uint32>(ClassRO::kIfields)) || (idx == static_cast<uint32>(ClassRO::kMethods))) &&
       mirSymbol.IsReflectionClassInfoRO()) ||
       mirSymbol.IsReflectionHashTabBucket()) {
    Emit(" - .");
    if (symAddrSym->IsReflectionFieldsInfoCompact() ||
        symAddrSym->IsReflectionMethodsInfoCompact()) {
      /* Mark the least significant bit as 1 for compact fieldinfo */
      Emit(" + ").Emit(kDataRefIsCompact);
    }
  } else if (mirSymbol.IsReflectionClassInfo()) {
    if ((idx == static_cast<uint32>(ClassProperty::kItab)) ||
        (idx == static_cast<uint32>(ClassProperty::kVtab)) ||
        (idx == static_cast<uint32>(ClassProperty::kInfoRo))) {
      Emit(" - . + ").Emit(kDataRefIsOffset);
    } else if (idx == static_cast<uint32>(ClassProperty::kGctib)) {
      if (cg->FindGCTIBPatternName(symAddrName).find(REF_PREFIX_STR) == 0) {
        Emit(" - . + ").Emit(kGctibRefIsIndirect);
      } else {
        Emit(" - .");
      }
    }
  } else if (mirSymbol.IsReflectionClassInfoRO()) {
    if (idx == static_cast<uint32>(ClassRO::kSuperclass)) {
      Emit(" - . + ").Emit(kDataRefIsOffset);
    }
  }

  if (cg->GetCGOptions().GeneratePositionIndependentExecutable()) {
    Emit(" - ");
    Emit(stName);
  }
  Emit("\n");
}

MIRAddroffuncConst *Emitter::GetAddroffuncConst(const MIRSymbol &mirSymbol, MIRAggConst &aggConst) {
  MIRAddroffuncConst *innerFuncAddr = nullptr;
  size_t addrIndex = mirSymbol.IsReflectionMethodsInfo() ? static_cast<size_t>(MethodProperty::kPaddrData) :
                                                           static_cast<size_t>(MethodInfoCompact::kPaddrData);
  MIRConst *pAddrConst = aggConst.GetConstVecItem(addrIndex);
  if (pAddrConst->GetKind() == kConstAddrof) {
    /* point addr data. */
    MIRAddrofConst *pAddr = safe_cast<MIRAddrofConst>(pAddrConst);
    MIRSymbol *symAddrSym = GlobalTables::GetGsymTable().GetSymbolFromStidx(pAddr->GetSymbolIndex().Idx());
    MIRAggConst *methodAddrAggConst = safe_cast<MIRAggConst>(symAddrSym->GetKonst());
    MIRAggConst *addrAggConst = safe_cast<MIRAggConst>(methodAddrAggConst->GetConstVecItem(0));
    MIRConst *funcAddrConst = addrAggConst->GetConstVecItem(0);
    if (funcAddrConst->GetKind() == kConstAddrofFunc) {
      /* func sybmol. */
      innerFuncAddr = safe_cast<MIRAddroffuncConst>(funcAddrConst);
    } else if (funcAddrConst->GetKind() == kConstInt) {
      /* def table index, replaced by def table for lazybinding. */
      std::string funcDefTabName = NameMangler::kMuidFuncDefTabPrefixStr + cg->GetMIRModule()->GetFileNameAsPostfix();
      MIRSymbol *funDefTabSy = GlobalTables::GetGsymTable().GetSymbolFromStrIdx(
          GlobalTables::GetStrTable().GetStrIdxFromName(funcDefTabName));
      MIRAggConst &funDefTabAggConst = static_cast<MIRAggConst&>(*funDefTabSy->GetKonst());
      MIRIntConst *funcAddrIndexConst = safe_cast<MIRIntConst>(funcAddrConst);
      int64 indexDefTab = funcAddrIndexConst->GetValue();
      MIRAggConst *defTabAggConst = safe_cast<MIRAggConst>(funDefTabAggConst.GetConstVecItem(indexDefTab));
      MIRConst *funcConst = defTabAggConst->GetConstVecItem(0);
      if (funcConst->GetKind() == kConstAddrofFunc) {
        innerFuncAddr = safe_cast<MIRAddroffuncConst>(funcConst);
      }
    }
  } else if (pAddrConst->GetKind() == kConstAddrofFunc) {
    innerFuncAddr = safe_cast<MIRAddroffuncConst>(pAddrConst);
  }
  return innerFuncAddr;
}

int64 Emitter::GetFieldOffsetValue(const std::string &className, const MIRIntConst &intConst,
                                   const std::map<GStrIdx, MIRType*> &strIdx2Type) {
  int64 idx = intConst.GetValue();
  bool isDefTabIndex = static_cast<uint64>(idx) & 0x1;
  int64 fieldIdx = static_cast<uint64>(idx) >> 1;
  if (isDefTabIndex) {
    /* it's def table index. */
    return fieldIdx;
  } else {
    /* really offset. */
    uint8 charBitWidth = GetPrimTypeSize(PTY_i8) * kBitsPerByte;
    GStrIdx strIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(className);
    auto it = strIdx2Type.find(strIdx);
    CHECK_FATAL(it->second != nullptr, "valid iterator check");
    ASSERT(it != strIdx2Type.end(), "Can not find type");
    MIRType &ty = *it->second;
    MIRStructType &structType = static_cast<MIRStructType&>(ty);
    std::pair<int32, int32> fieldOffsetPair =
        Globals::GetInstance()->GetBECommon()->GetFieldOffset(structType, fieldIdx);
    int64 fieldOffset = fieldOffsetPair.first * static_cast<int64>(charBitWidth) + fieldOffsetPair.second;
    return fieldOffset;
  }
}

void Emitter::InitRangeIdx2PerfixStr() {
  rangeIdx2PrefixStr[RangeIdx::kVtab] = kMuidVtabPrefixStr;
  rangeIdx2PrefixStr[RangeIdx::kItab] = kMuidItabPrefixStr;
  rangeIdx2PrefixStr[RangeIdx::kVtabOffset] = kMuidVtabOffsetPrefixStr;
  rangeIdx2PrefixStr[RangeIdx::kFieldOffset] = kMuidFieldOffsetPrefixStr;
  rangeIdx2PrefixStr[RangeIdx::kValueOffset] = kMuidValueOffsetPrefixStr;
  rangeIdx2PrefixStr[RangeIdx::kLocalClassInfo] = kMuidLocalClassInfoStr;
  rangeIdx2PrefixStr[RangeIdx::kConststr] = kMuidConststrPrefixStr;
  rangeIdx2PrefixStr[RangeIdx::kSuperclass] = kMuidSuperclassPrefixStr;
  rangeIdx2PrefixStr[RangeIdx::kGlobalRootlist] = kMuidGlobalRootlistPrefixStr;
  rangeIdx2PrefixStr[RangeIdx::kClassmetaData] = kMuidClassMetadataPrefixStr;
  rangeIdx2PrefixStr[RangeIdx::kClassBucket] = kMuidClassMetadataBucketPrefixStr;
  rangeIdx2PrefixStr[RangeIdx::kJavatext] = kMuidJavatextPrefixStr;
  rangeIdx2PrefixStr[RangeIdx::kDataSection] = kMuidDataSectionStr;
  rangeIdx2PrefixStr[RangeIdx::kJavajni] = kRegJNITabPrefixStr;
  rangeIdx2PrefixStr[RangeIdx::kJavajniFunc] = kRegJNIFuncTabPrefixStr;
  rangeIdx2PrefixStr[RangeIdx::kDecoupleStaticKey] = kDecoupleStaticKeyStr;
  rangeIdx2PrefixStr[RangeIdx::kDecoupleStaticValue] = kDecoupleStaticValueStr;
  rangeIdx2PrefixStr[RangeIdx::kBssStart] = kBssSectionStr;
  rangeIdx2PrefixStr[RangeIdx::kLinkerSoHash] = kLinkerHashSoStr;
}

void Emitter::EmitIntConst(const MIRSymbol &mirSymbol, MIRAggConst &aggConst, uint32 itabConflictIndex,
                           const std::map<GStrIdx, MIRType*> &strIdx2Type, size_t idx) {
  MIRConst *elemConst = aggConst.GetConstVecItem(idx);
  const std::string stName = mirSymbol.GetName();

  MIRIntConst *intConst = safe_cast<MIRIntConst>(elemConst);
  ASSERT(intConst != nullptr, "Uexpected const type");

  /* ignore abstract function addr */
  if ((idx == static_cast<uint32>(MethodInfoCompact::kPaddrData)) && mirSymbol.IsReflectionMethodsInfoCompact()) {
    return;
  }

  if (((idx == static_cast<uint32>(MethodProperty::kVtabIndex)) && (mirSymbol.IsReflectionMethodsInfo())) ||
      ((idx == static_cast<uint32>(MethodInfoCompact::kVtabIndex)) && mirSymbol.IsReflectionMethodsInfoCompact())) {
    MIRAddroffuncConst *innerFuncAddr = GetAddroffuncConst(mirSymbol, aggConst);
    if (innerFuncAddr != nullptr) {
      Emit(".Label.name." + GlobalTables::GetFunctionTable().GetFunctionFromPuidx(
          innerFuncAddr->GetValue())->GetName());
      Emit(":\n");
    }
  }
  /* refer to DeCouple::GenOffsetTableType */
  constexpr int fieldTypeIdx = 2;
  constexpr int methodTypeIdx = 2;
  bool isClassInfo = (idx == static_cast<uint32>(ClassRO::kClassName) ||
                      idx == static_cast<uint32>(ClassRO::kAnnotation)) && mirSymbol.IsReflectionClassInfoRO();
  bool isMethodsInfo = (idx == static_cast<uint32>(MethodProperty::kMethodName) ||
                        idx == static_cast<uint32>(MethodProperty::kSigName) ||
                        idx == static_cast<uint32>(MethodProperty::kAnnotation)) && mirSymbol.IsReflectionMethodsInfo();
  bool isFieldsInfo = (idx == static_cast<uint32>(FieldProperty::kTypeName) ||
                       idx == static_cast<uint32>(FieldProperty::kName) ||
                       idx == static_cast<uint32>(FieldProperty::kAnnotation)) && mirSymbol.IsReflectionFieldsInfo();
  /* RegisterTable has been Int Array, visit element instead of field. */
  bool isInOffsetTab = (idx == 1 || idx == methodTypeIdx) &&
                       (StringUtils::StartsWith(stName, kVtabOffsetTabStr) ||
                        StringUtils::StartsWith(stName, kFieldOffsetTabStr));
  /* The 1 && 2 of Decouple static struct is the string name */
  bool isStaticStr = (idx == 1 || idx == 2) && aggConst.GetConstVec().size() == kSizeOfDecoupleStaticStruct &&
                     StringUtils::StartsWith(stName, kDecoupleStaticKeyStr);
  /* process conflict table index larger than itabConflictIndex * 2 + 2 element */
  bool isConflictPerfix = (idx >= (static_cast<uint64>(itabConflictIndex) * 2 + 2)) && (idx % 2 == 0) &&
                          StringUtils::StartsWith(stName, ITAB_CONFLICT_PREFIX_STR);
  if (isClassInfo || isMethodsInfo || isFieldsInfo || mirSymbol.IsRegJNITab() || isInOffsetTab ||
      isStaticStr || isConflictPerfix) {
    /* compare with all 1s */
    uint32 index = static_cast<uint32>((safe_cast<MIRIntConst>(elemConst))->GetValue()) & 0xFFFFFFFF;
    bool isHotReflectStr = (index & 0x00000003) != 0;     /* use the last two bits of index in this expression */
    std::string hotStr;
    if (isHotReflectStr) {
      uint32 tag = (index & 0x00000003) - kCStringShift;  /* use the last two bits of index in this expression */
      if (tag == kLayoutBootHot) {
        hotStr = kReflectionStartHotStrtabPrefixStr;
      } else if (tag == kLayoutBothHot) {
        hotStr = kReflectionBothHotStrTabPrefixStr;
      } else {
        hotStr = kReflectionRunHotStrtabPrefixStr;
      }
    }
    std::string reflectStrTabPrefix = isHotReflectStr ? hotStr : kReflectionStrtabPrefixStr;
    std::string strTabName = reflectStrTabPrefix + cg->GetMIRModule()->GetFileNameAsPostfix();
    /* left shift 2 bit to get low 30 bit data for MIRIntConst */
    elemConst = GlobalTables::GetIntConstTable().GetOrCreateIntConst(
        index >> 2, elemConst->GetType(), elemConst->GetFieldId());
    intConst = safe_cast<MIRIntConst>(elemConst);
    aggConst.SetConstVecItem(idx, *intConst);
#ifdef USE_32BIT_REF
    if (stName.find(ITAB_CONFLICT_PREFIX_STR) == 0) {
      EmitScalarConstant(*elemConst, false, true);
    } else {
      EmitScalarConstant(*elemConst, false);
    }
#else
    EmitScalarConstant(*elemConst, false);
#endif  /* USE_32BIT_REF */
    Emit("+" + strTabName);
    if (mirSymbol.IsRegJNITab() || mirSymbol.IsReflectionMethodsInfo() || mirSymbol.IsReflectionFieldsInfo()) {
      Emit("-.");
    }
    if (StringUtils::StartsWith(stName, kDecoupleStaticKeyStr)) {
      Emit("-.");
    }
    if (mirSymbol.IsReflectionClassInfoRO()) {
      if (idx == static_cast<uint32>(ClassRO::kAnnotation)) {
        Emit("-.");
      } else if (idx == static_cast<uint32>(ClassRO::kClassName)) {
        /* output in hex format to show it is a flag of bits. */
        std::stringstream ss;
        ss << std::hex << "0x" << MByteRef::PositiveOffsetBias;
        Emit(" - . + " + ss.str());
      }
    }
    if (StringUtils::StartsWith(stName, ITAB_PREFIX_STR)) {
      Emit("-.");
    }
    if (StringUtils::StartsWith(stName, ITAB_CONFLICT_PREFIX_STR)) {
      /* output in hex format to show it is a flag of bits. */
      std::stringstream ss;
      ss << std::hex << "0x" << MByteRef32::PositiveOffsetBias;
      Emit(" - . + " + ss.str());
    }
    if ((idx == 1 || idx == methodTypeIdx) && StringUtils::StartsWith(stName, kVtabOffsetTabStr)) {
      Emit("-.");
    }
    if ((idx == 1 || idx == fieldTypeIdx) && StringUtils::StartsWith(stName, kFieldOffsetTabStr)) {
      Emit("-.");
    }
    Emit("\n");
  } else if (idx == kFuncDefAddrIndex && mirSymbol.IsMuidFuncUndefTab()) {
#if defined(USE_32BIT_REF)
    Emit("\t.long\t");
#else
    Emit("\t.quad\t");
#endif  /* USE_32BIT_REF */
    if (CGOptions::IsLazyBinding() && !cg->IsLibcore()) {
      /*
       * Check enum BindingState defined in Mpl_Binding.h,
       * 5 means kBindingStateMethodUndef:5 offset away from base __BindingProtectRegion__.
       */
#if defined(USE_32BIT_REF)
      Emit("0x5\n");  /* Fix it in runtime, __BindingProtectRegion__ + kBindingStateMethodUndef:5. */
#else
      Emit("__BindingProtectRegion__ + 5\n");
#endif  /* USE_32BIT_REF */
    } else {
      Emit("0\n");
    }
  } else if (mirSymbol.IsRegJNIFuncTab()) {
    std::string strTabName = kRegJNITabPrefixStr + cg->GetMIRModule()->GetFileNameAsPostfix();
    EmitScalarConstant(*elemConst, false);
    Emit("+" + strTabName + "\n");
  } else if (mirSymbol.IsReflectionMethodAddrData()) {
    int64 defTabIndex = intConst->GetValue();
#ifdef USE_32BIT_REF
    Emit("\t.long\t");
#else
    Emit("\t.quad\t");
#endif  /* USE_32BIT_REF */
    Emit(std::to_string(defTabIndex) + "\n");
  } else if (mirSymbol.IsReflectionFieldOffsetData()) {
    /* Figure out instance field offset now. */
    size_t prefixStrLen = strlen(kFieldOffsetDataPrefixStr);
    size_t pos = stName.find("_FieldID_");
    std::string typeName = stName.substr(prefixStrLen, pos - prefixStrLen);
#ifdef USE_32BIT_REF
    std::string widthFlag = ".long";
#else
    std::string widthFlag = ".quad";
#endif  /* USE_32BIT_REF */
    int64 fieldOffset = GetFieldOffsetValue(typeName, *intConst, strIdx2Type);
    int64 fieldIdx = intConst->GetValue();
    bool isDefTabIndex = static_cast<uint64>(fieldIdx) & 0x1;
    if (isDefTabIndex) {
      /* it's def table index. */
      Emit("\t//  " + typeName + " static field, data def table index " + std::to_string(fieldOffset) + "\n");
    } else {
      /* really offset. */
      fieldIdx = static_cast<uint64>(fieldIdx) >> 1;
      Emit("\t//  " + typeName + "\t field" + std::to_string(fieldIdx) + "\n");
    }
    Emit("\t" + widthFlag + "\t" + std::to_string(fieldOffset) + "\n");
  } else if (((idx == static_cast<uint32>(FieldProperty::kPOffset)) && mirSymbol.IsReflectionFieldsInfo()) ||
             ((idx == static_cast<uint32>(FieldPropertyCompact::kPOffset)) &&
              mirSymbol.IsReflectionFieldsInfoCompact())) {
    std::string typeName;
    std::string widthFlag;
#ifdef USE_32BIT_REF
    const int width = 4;
#else
    const int width = 8;
#endif  /* USE_32BIT_REF */
    if (mirSymbol.IsReflectionFieldsInfo()) {
      typeName = stName.substr(strlen(kFieldsInfoPrefixStr));
#ifdef USE_32BIT_REF
      widthFlag = ".long";
#else
      widthFlag = ".quad";
#endif  /* USE_32BIT_REF */
    } else {
      size_t prefixStrLen = strlen(kFieldsInfoCompactPrefixStr);
      typeName = stName.substr(prefixStrLen);
      widthFlag = ".long";
    }
    int64 fieldIdx = intConst->GetValue();
    MIRSymbol *pOffsetData = GlobalTables::GetGsymTable().GetSymbolFromStrIdx(
        GlobalTables::GetStrTable().GetStrIdxFromName(kFieldOffsetDataPrefixStr + typeName));
    if (pOffsetData != nullptr) {
      fieldIdx *= width;
      std::string fieldOffset = kFieldOffsetDataPrefixStr + typeName;
      Emit("\t" + widthFlag + "\t" + std::to_string(fieldIdx) + " + " + fieldOffset + " - .\n");
    } else {
      /* pOffsetData null, means FieldMeta.offset is really offset */
      int64 fieldOffset = GetFieldOffsetValue(typeName, *intConst, strIdx2Type);
      Emit("\t//  " + typeName + "\t field" + std::to_string(fieldIdx) + "\n");
      Emit("\t" + widthFlag + "\t" + std::to_string(fieldOffset) + "\n");
    }
  } else if ((idx == static_cast<uint32>(ClassProperty::kObjsize)) && mirSymbol.IsReflectionClassInfo()) {
    std::string delimiter = "$$";
    std::string typeName =
        stName.substr(strlen(CLASSINFO_PREFIX_STR), stName.find(delimiter) - strlen(CLASSINFO_PREFIX_STR));
    uint32 objSize = 0;
    std::string comments;

    if (typeName.size() > 1 && typeName[0] == '$') {
      /* fill element size for array class; */
      std::string newTypeName = typeName.substr(1);
      /* another $(arraysplitter) */
      if (newTypeName.find("$") == std::string::npos) {
        CHECK_FATAL(false, "can not find $ in std::string");
      }
      typeName = newTypeName.substr(newTypeName.find("$") + 1);
      int32 pTypeSize;

      /* we only need to calculate primitive type in arrays. */
      if ((pTypeSize = GetPrimitiveTypeSize(typeName)) != -1) {
        objSize = static_cast<uint32>(pTypeSize);
      }
      comments = "// elemobjsize";
    } else {
      comments = "// objsize";
    }

    if (!objSize) {
      GStrIdx strIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(typeName);
      auto it = strIdx2Type.find(strIdx);
      ASSERT(it != strIdx2Type.end(), "Can not find type");
      MIRType *mirType = it->second;

      objSize = Globals::GetInstance()->GetBECommon()->GetTypeSize(mirType->GetTypeIndex());
    }
    /* objSize should not exceed 16 bits */
    ASSERT(objSize <= 0xffff, "Error:the objSize is too large");
    Emit("\t.short\t" + std::to_string(objSize) + comments + "\n");
  } else if (mirSymbol.IsMuidRangeTab()) {
    MIRIntConst *subIntCt = safe_cast<MIRIntConst>(elemConst);
    int flag = subIntCt->GetValue();
    InitRangeIdx2PerfixStr();
    if (rangeIdx2PrefixStr.find(flag) == rangeIdx2PrefixStr.end()) {
      EmitScalarConstant(*elemConst, false);
      Emit("\n");
      return;
    }
    std::string prefix = rangeIdx2PrefixStr[flag];
#if TARGAARCH64
    Emit("\t.quad\t");
#else
    Emit("\t.word\t");
#endif
    if (idx == kRangeBeginIndex) {
      Emit(prefix + "_begin\n");
    } else {
      Emit(prefix + "_end\n");
    }
  } else {
#ifdef USE_32BIT_REF
    if (StringUtils::StartsWith(stName, ITAB_CONFLICT_PREFIX_STR) || StringUtils::StartsWith(stName, ITAB_PREFIX_STR) ||
        StringUtils::StartsWith(stName, VTAB_PREFIX_STR)) {
      EmitScalarConstant(*elemConst, false, true);
    } else {
      EmitScalarConstant(*elemConst, false);
    }
#else
    EmitScalarConstant(*elemConst, false);
#endif  /* USE_32BIT_REF */
    Emit("\n");
  }
}

void Emitter::EmitConstantTable(const MIRSymbol &mirSymbol, MIRConst &mirConst,
                                const std::map<GStrIdx, MIRType*> &strIdx2Type) {
  const std::string stName = mirSymbol.GetName();
  MIRAggConst &aggConst = static_cast<MIRAggConst&>(mirConst);
  uint32 itabConflictIndex = 0;
  for (size_t i = 0; i < aggConst.GetConstVec().size(); ++i) {
    MIRConst *elemConst = aggConst.GetConstVecItem(i);
    if (i == 0 && StringUtils::StartsWith(stName, ITAB_CONFLICT_PREFIX_STR)) {
#ifdef USE_32BIT_REF
      itabConflictIndex = static_cast<uint64>((safe_cast<MIRIntConst>(elemConst))->GetValue()) & 0xffff;
#else
      itabConflictIndex = static_cast<uint64>((safe_cast<MIRIntConst>(elemConst))->GetValue()) & 0xffffffff;
#endif
    }
    if (IsPrimitiveScalar(elemConst->GetType().GetPrimType())) {
      if (elemConst->GetKind() == kConstAddrofFunc) {     /* addroffunc const */
        EmitAddrofFuncConst(mirSymbol, *elemConst, i);
      } else if (elemConst->GetKind() == kConstAddrof) {  /* addrof symbol const */
        EmitAddrofSymbolConst(mirSymbol, *elemConst, i);
      } else {                                            /* intconst */
        EmitIntConst(mirSymbol, aggConst, itabConflictIndex,  strIdx2Type, i);
      }
    } else if (elemConst->GetType().GetKind() == kTypeArray || elemConst->GetType().GetKind() == kTypeStruct) {
      if (StringUtils::StartsWith(mirSymbol.GetName(), NameMangler::kOffsetTabStr) && (i == 0 || i == 1)) {
        // EmitOffsetValueTable
#ifdef USE_32BIT_REF
        Emit("\t.long\t");
#else
        Emit("\t.quad\t");
#endif
        if (i == 0) {
          Emit(NameMangler::kVtabOffsetTabStr + cg->GetMIRModule()->GetFileNameAsPostfix() + " - .\n");
        } else {
          Emit(NameMangler::kFieldOffsetTabStr + cg->GetMIRModule()->GetFileNameAsPostfix() + " - .\n");
        }
      } else {
        EmitConstantTable(mirSymbol, *elemConst, strIdx2Type);
      }
    }
  }
}

void Emitter::EmitArrayConstant(MIRConst &mirConst) {
  MIRType &mirType = mirConst.GetType();
  MIRAggConst &arrayCt = static_cast<MIRAggConst&>(mirConst);
  MIRArrayType &arrayType = static_cast<MIRArrayType&>(mirType);
  size_t uNum = arrayCt.GetConstVec().size();
  int64 iNum = (arrayType.GetSizeArrayItem(0) > 0) ? (static_cast<int64>(arrayType.GetSizeArrayItem(0))) - uNum : 0;
  for (size_t i = 0; i < uNum; ++i) {
    MIRConst *elemConst = arrayCt.GetConstVecItem(i);
    if (IsPrimitiveScalar(elemConst->GetType().GetPrimType())) {
      EmitScalarConstant(*elemConst);
    } else if (elemConst->GetType().GetKind() == kTypeArray) {
      EmitArrayConstant(*elemConst);
    } else if (elemConst->GetType().GetKind() == kTypeStruct || elemConst->GetType().GetKind() == kTypeClass) {
      EmitStructConstant(*elemConst);
    } else {
      ASSERT(false, "should not run here");
    }
  }
  if (iNum > 0) {
    CHECK_FATAL(!Globals::GetInstance()->GetBECommon()->IsEmptyOfTypeSizeTable(), "container empty check");
    CHECK_FATAL(!arrayCt.GetConstVec().empty(), "container empty check");
    uint64 unInSizeInByte = static_cast<uint64>(iNum) * static_cast<uint64>(
        Globals::GetInstance()->GetBECommon()->GetTypeSize(arrayCt.GetConstVecItem(0)->GetType().GetTypeIndex()));
    if (unInSizeInByte != 0) {
      EmitNullConstant(unInSizeInByte);
    }
  }
  Emit("\n");
}

void Emitter::EmitStructConstant(MIRConst &mirConst) {
  StructEmitInfo *sEmitInfo = cg->GetMIRModule()->GetMemPool()->New<StructEmitInfo>();
  CHECK_FATAL(sEmitInfo != nullptr, "create a new struct emit info failed in Emitter::EmitStructConstant");
  MIRType &mirType = mirConst.GetType();
  MIRAggConst &structCt = static_cast<MIRAggConst&>(mirConst);
  MIRStructType &structType = static_cast<MIRStructType&>(mirType);
  ASSERT(structType.GetKind() != kTypeUnion, "NYI, not support now.");
  /* all elements of struct. */
  uint8 num = structType.GetFieldsSize();
  /* total size of emitted elements size. */
  uint32 size = Globals::GetInstance()->GetBECommon()->GetTypeSize(structType.GetTypeIndex());
  uint32 fieldIdx = 1;
  for (uint32 i = 0; i < num; ++i) {
    MIRConst *elemConst = structCt.GetAggConstElement(fieldIdx);
    MIRType &elemType = *structType.GetElemType(i);
    MIRType *nextElemType = nullptr;
    if (i != static_cast<uint32>(num - 1)) {
      nextElemType = structType.GetElemType(i + 1);
    }
    uint32 elemSize = Globals::GetInstance()->GetBECommon()->GetTypeSize(elemType.GetTypeIndex());
    uint8 charBitWidth = GetPrimTypeSize(PTY_i8) * kBitsPerByte;
    if (elemType.GetKind() == kTypeBitField) {
      if (elemConst == nullptr) {
        MIRIntConst *zeroFill = GlobalTables::GetIntConstTable().GetOrCreateIntConst(0, elemType);
        zeroFill->SetFieldID(fieldIdx);
        elemConst = zeroFill;
      }
      uint64 fieldOffset =
          static_cast<uint64>(static_cast<int64>(
              Globals::GetInstance()->GetBECommon()->GetFieldOffset(structType, fieldIdx).first)) *
          static_cast<uint64>(charBitWidth) +
          static_cast<uint64>(static_cast<int64>(
              Globals::GetInstance()->GetBECommon()->GetFieldOffset(structType, fieldIdx).second));
      EmitBitFieldConstant(*sEmitInfo, *elemConst, nextElemType, fieldOffset);
    } else {
      if (elemConst != nullptr) {
        if (IsPrimitiveScalar(elemType.GetPrimType())) {
          EmitScalarConstant(*elemConst);
        } else if (elemType.GetKind() == kTypeArray) {
          EmitArrayConstant(*elemConst);
        } else if ((elemType.GetKind() == kTypeStruct) || (elemType.GetKind() == kTypeClass)) {
          EmitStructConstant(*elemConst);
        } else {
          ASSERT(false, "should not run here");
        }
      } else {
        EmitNullConstant(elemSize);
      }
      sEmitInfo->IncreaseTotalSize(elemSize);
      sEmitInfo->SetNextFieldOffset(sEmitInfo->GetTotalSize() * charBitWidth);
    }

    if (nextElemType != nullptr && kTypeBitField != nextElemType->GetKind()) {
      ASSERT(i < static_cast<uint32>(num - 1), "NYI");
      uint32 nextAlign = Globals::GetInstance()->GetBECommon()->GetTypeAlign(nextElemType->GetTypeIndex());
      ASSERT(nextAlign != 0, "expect non-zero");
      /* append size, append 0 when align need. */
      uint64 totalSize = sEmitInfo->GetTotalSize();
      uint32 psize = (totalSize % nextAlign == 0) ? 0 : (nextAlign - (totalSize % nextAlign));
      if (psize != 0) {
        EmitNullConstant(psize);
        sEmitInfo->IncreaseTotalSize(psize);
        sEmitInfo->SetNextFieldOffset(sEmitInfo->GetTotalSize() * charBitWidth);
      }
      /* element is uninitialized, emit null constant. */
    }
    fieldIdx += Globals::GetInstance()->GetBECommon()->GetFieldIdxIncrement(elemType);
  }

  uint32 opSize = size - sEmitInfo->GetTotalSize();
  if (opSize != 0) {
    EmitNullConstant(opSize);
  }
}

/* BlockMarker is for Debugging/Profiling */
void Emitter::EmitBlockMarker(const std::string &markerName, const std::string &sectionName,
                              bool withAddr, const std::string &addrName) {
  return;
}

void Emitter::EmitLiteral(const MIRSymbol &literal, const std::map<GStrIdx, MIRType*> &strIdx2Type) {
  /*
   * .type _C_STR_xxxx, %object
   * .local _C_STR_xxxx
   * .data
   * .align 3
   * _C_STR_xxxx:
   * .quad __cinf_Ljava_2Flang_2FString_3B
   * ....
   * .size _C_STR_xxxx, 40
   */
  if (literal.GetStorageClass() == kScUnused) {
    return;
  }
  EmitAsmLabel(literal, kAsmType);
  /* literal should always be fstatic and readonly? */
  EmitAsmLabel(literal, kAsmLocal);  /* alwasy fstatic */
  EmitAsmLabel(kAsmData);
  EmitAsmLabel(literal, kAsmAlign);
  EmitAsmLabel(literal, kAsmSyname);
  /* literal is an array */
  MIRConst *mirConst = literal.GetKonst();
  CHECK_FATAL(mirConst != nullptr, "mirConst should not be nullptr in EmitLiteral");
  if (literal.HasAddrOfValues()) {
    EmitConstantTable(literal, *mirConst, strIdx2Type);
  } else {
    EmitArrayConstant(*mirConst);
  }
  EmitAsmLabel(literal, kAsmSize);
}

void Emitter::EmitFuncLayoutInfo(const MIRSymbol &layout) {
  /*
   * .type $marker_name$, %object
   * .global $marker_name$
   * .data
   * .align 3
   * $marker_name$:
   * .quad funcaddr
   * .size $marker_name$, 8
   */
  MIRConst *mirConst = layout.GetKonst();
  MIRAggConst *aggConst = safe_cast<MIRAggConst>(mirConst);
  ASSERT(aggConst != nullptr, "null ptr check");
  if (aggConst->GetConstVec().size() != static_cast<uint32>(LayoutType::kLayoutTypeCount)) {
    maple::logInfo.MapleLogger(kLlErr) << "something wrong happen in funclayoutsym\t"
         << "constVec size\t" << aggConst->GetConstVec().size() << "\n";
    return;
  }
  for (size_t i = 0; i < static_cast<size_t>(LayoutType::kLayoutTypeCount); ++i) {
    std::string markerName = "__MBlock_" + GetLayoutTypeString(i) + "_func_start";
    CHECK_FATAL(aggConst->GetConstVecItem(i)->GetKind() == kConstAddrofFunc, "expect kConstAddrofFunc type");
    MIRAddroffuncConst *funcAddr = safe_cast<MIRAddroffuncConst>(aggConst->GetConstVecItem(i));
    ASSERT(funcAddr != nullptr, "null ptr check");
    Emit(asmInfo->GetType());
    Emit(markerName + ", %object\n");
    Emit(asmInfo->GetGlobal());
    Emit(markerName + "\n");
    EmitAsmLabel(kAsmData);
    Emit(asmInfo->GetAlign());
    Emit("  3\n" + markerName + ":\n");
    Emit("\t.quad ");
    Emit(GlobalTables::GetFunctionTable().GetFunctionFromPuidx(funcAddr->GetValue())->GetName());
    Emit("\n");
    Emit(asmInfo->GetSize());
    Emit(markerName + ", 8\n");
  }
}

void Emitter::EmitStaticFields(const std::vector<MIRSymbol*> &fields) {
  for (auto *itSymbol : fields) {
    EmitAsmLabel(*itSymbol, kAsmType);
    /* literal should always be fstatic and readonly? */
    EmitAsmLabel(*itSymbol, kAsmLocal);  /* alwasy fstatic */
    EmitAsmLabel(kAsmData);
    EmitAsmLabel(*itSymbol, kAsmAlign);
    EmitAsmLabel(*itSymbol, kAsmSyname);
    /* literal is an array */
    MIRConst *mirConst = itSymbol->GetKonst();
    EmitArrayConstant(*mirConst);
  }
}

void Emitter::EmitLiterals(std::vector<std::pair<MIRSymbol*, bool>> &literals,
                           const std::map<GStrIdx, MIRType*> &strIdx2Type) {
  /* emit hot literal start symbol */
  EmitBlockMarker("__MBlock_literal_hot_begin", "", false);
  /* emit hot literal end symbol */
  EmitBlockMarker("__MBlock_literal_hot_end", "", false);

  /* emit cold literal start symbol */
  EmitBlockMarker("__MBlock_literal_cold_begin", "", false);
  /* emit other literals (not in the profile) next. */
  for (const auto &literalPair : literals) {
    if (!literalPair.second) {
      /* not emit yet */
      EmitLiteral(*(literalPair.first), strIdx2Type);
    }
  }
  /* emit cold literal end symbol */
  EmitBlockMarker("__MBlock_literal_cold_end", "", false);
}

void Emitter::GetHotAndColdMetaSymbolInfo(const std::vector<MIRSymbol*> &mirSymbolVec,
                                          std::vector<MIRSymbol*> &hotFieldInfoSymbolVec,
                                          std::vector<MIRSymbol*> &coldFieldInfoSymbolVec, const std::string &prefixStr,
                                          bool forceCold) {
  bool isHot = true;
  for (auto mirSymbol : mirSymbolVec) {
    CHECK_FATAL(prefixStr.length() < mirSymbol->GetName().length(), "string length check");
    std::string name = mirSymbol->GetName().substr(prefixStr.length());
    std::string klassJavaDescriptor;
    NameMangler::DecodeMapleNameToJavaDescriptor(name, klassJavaDescriptor);
    if (isHot && !forceCold) {
      hotFieldInfoSymbolVec.push_back(mirSymbol);
    } else {
      coldFieldInfoSymbolVec.push_back(mirSymbol);
    }
  }
}

void Emitter::EmitMetaDataSymbolWithMarkFlag(const std::vector<MIRSymbol*> &mirSymbolVec,
                                             const std::map<GStrIdx, MIRType*> &strIdx2Type,
                                             const std::string &prefixStr, const std::string &sectionName,
                                             bool isHotFlag) {
  if (mirSymbolVec.empty()) {
    return;
  }
  const std::string &markString = "__MBlock" + prefixStr;
  const std::string &hotOrCold = isHotFlag ? "hot" : "cold";
  EmitBlockMarker((markString + hotOrCold + "_begin"), sectionName, false);
  if (prefixStr == kFieldsInfoCompactPrefixStr || prefixStr == kMethodsInfoCompactPrefixStr ||
      prefixStr == kFieldOffsetDataPrefixStr || prefixStr == kMethodAddrDataPrefixStr) {
    for (auto s : mirSymbolVec) {
      EmitMethodFieldSequential(*s, strIdx2Type, sectionName);
    }
  } else {
    for (auto s : mirSymbolVec) {
      EmitClassInfoSequential(*s, strIdx2Type, sectionName);
    }
  }
  EmitBlockMarker((markString + hotOrCold + "_end"), sectionName, false);
}

void Emitter::MarkVtabOrItabEndFlag(const std::vector<MIRSymbol*> &mirSymbolVec) {
  for (auto mirSymbol : mirSymbolVec) {
    auto *aggConst = safe_cast<MIRAggConst>(mirSymbol->GetKonst());
    if ((aggConst == nullptr) || (aggConst->GetConstVec().empty())) {
      continue;
    }
    size_t size = aggConst->GetConstVec().size();
    MIRConst *elemConst = aggConst->GetConstVecItem(size - 1);
    ASSERT(elemConst != nullptr, "null ptr check");
    if (elemConst->GetKind() == kConstAddrofFunc) {
      maple::logInfo.MapleLogger(kLlErr) << "ERROR: the last vtab/itab content should not be funcAddr\n";
    } else {
      if (elemConst->GetKind() != kConstInt) {
        CHECK_FATAL(elemConst->GetKind() == kConstAddrof, "must be");
        continue;
      }
      MIRIntConst *tabConst = static_cast<MIRIntConst*>(elemConst);
#ifdef USE_32BIT_REF
      /* #define COLD VTAB ITAB END FLAG  0X4000000000000000 */
      tabConst = GlobalTables::GetIntConstTable().GetOrCreateIntConst(
          static_cast<uint32>(tabConst->GetValue()) | 0X40000000, tabConst->GetType(), tabConst->GetFieldId());
#else
      /* #define COLD VTAB ITAB END FLAG  0X4000000000000000 */
      tabConst = GlobalTables::GetIntConstTable().GetOrCreateIntConst(
          static_cast<int64>(static_cast<uint64>(tabConst->GetValue()) | 0X4000000000000000),
          tabConst->GetType(), tabConst->GetFieldId());
#endif
      aggConst->SetConstVecItem(size - 1, *tabConst);
    }
  }
}

void Emitter::EmitGlobalVar(const MIRSymbol &globalVar) {
  EmitAsmLabel(globalVar, kAsmType);
  EmitAsmLabel(globalVar, kAsmLocal);
  EmitAsmLabel(globalVar, kAsmComm);
}

void Emitter::EmitGlobalVars(std::vector<std::pair<MIRSymbol*, bool>> &globalVars) {
  /* load globalVars profile */
  if (globalVars.empty()) {
    return;
  }
  std::unordered_set<std::string> hotVars;
  std::ifstream inFile;
  if (!CGOptions::IsGlobalVarProFileEmpty()) {
    inFile.open(CGOptions::GetGlobalVarProFile());
    if (inFile.fail()) {
      maple::logInfo.MapleLogger(kLlErr) << "Cannot open globalVar profile file " << CGOptions::GetGlobalVarProFile()
                                         << "\n";
    }
  }
  if (CGOptions::IsGlobalVarProFileEmpty() || inFile.fail()) {
    for (const auto &globalVarPair : globalVars) {
      EmitGlobalVar(*(globalVarPair.first));
    }
    return;
  }
  std::string globalVarName;
  while (inFile >> globalVarName) {
    hotVars.insert(globalVarName);
  }
  inFile.close();
  bool hotBeginSet = false;
  bool coldBeginSet = false;
  for (auto &globalVarPair : globalVars) {
    if (hotVars.find(globalVarPair.first->GetName()) != hotVars.end()) {
      if (!hotBeginSet) {
        /* emit hot globalvar start symbol */
        EmitBlockMarker("__MBlock_globalVars_hot_begin", "", true, globalVarPair.first->GetName());
        hotBeginSet = true;
      }
      EmitGlobalVar(*(globalVarPair.first));
      globalVarPair.second = true;
    }
  }
  for (const auto &globalVarPair : globalVars) {
    if (!globalVarPair.second) {  /* not emit yet */
      if (!coldBeginSet) {
        /* emit hot globalvar start symbol */
        EmitBlockMarker("__MBlock_globalVars_cold_begin", "", true, globalVarPair.first->GetName());
        coldBeginSet = true;
      }
      EmitGlobalVar(*(globalVarPair.first));
    }
  }
  MIRSymbol *endSym = globalVars.back().first;
  MIRType *mirType = endSym->GetType();
  const std::string kStaticVarEndAdd =
      std::to_string(Globals::GetInstance()->GetBECommon()->GetTypeSize(mirType->GetTypeIndex())) + "+" +
                     endSym->GetName();
  EmitBlockMarker("__MBlock_globalVars_cold_end", "", true, kStaticVarEndAdd);
}

void Emitter::EmitGlobalVariable() {
  std::vector<MIRSymbol*> typeStVec;
  std::vector<MIRSymbol*> typeNameStVec;
  std::map<GStrIdx, MIRType*> strIdx2Type;

  /* Create name2type map which will be used by reflection. */
  for (MIRType *type : GlobalTables::GetTypeTable().GetTypeTable()) {
    if (type == nullptr || (type->GetKind() != kTypeClass && type->GetKind() != kTypeInterface)) {
      continue;
    }
    GStrIdx strIdx = type->GetNameStrIdx();
    strIdx2Type[strIdx] = type;
  }

  /* sort symbols; classinfo-->field-->method */
  size_t size = GlobalTables::GetGsymTable().GetSymbolTableSize();
  std::vector<MIRSymbol*> classInfoVec;
  std::vector<MIRSymbol*> vtabVec;
  std::vector<MIRSymbol*> staticFieldsVec;
  std::vector<std::pair<MIRSymbol*, bool>> globalVarVec;
  std::vector<MIRSymbol*> itabVec;
  std::vector<MIRSymbol*> itabConflictVec;
  std::vector<MIRSymbol*> vtabOffsetVec;
  std::vector<MIRSymbol*> fieldOffsetVec;
  std::vector<MIRSymbol*> valueOffsetVec;
  std::vector<MIRSymbol*> localClassInfoVec;
  std::vector<MIRSymbol*> constStrVec;
  std::vector<std::pair<MIRSymbol*, bool>> literalVec;
  std::vector<MIRSymbol*> muidVec = { nullptr };
  std::vector<MIRSymbol*> fieldOffsetDatas;
  std::vector<MIRSymbol*> methodAddrDatas;
  std::vector<MIRSymbol*> staticDecoupleKeyVec;
  std::vector<MIRSymbol*> staticDecoupleValueVec;

  for (size_t i = 0; i < size; ++i) {
    MIRSymbol *mirSymbol = GlobalTables::GetGsymTable().GetSymbolFromStidx(i);
    if (mirSymbol == nullptr || mirSymbol->IsDeleted() || mirSymbol->GetStorageClass() == kScUnused) {
      continue;
    }
    if (mirSymbol->GetName().find(VTAB_PREFIX_STR) == 0) {
      vtabVec.push_back(mirSymbol);
      continue;
    } else if (mirSymbol->GetName().find(ITAB_PREFIX_STR) == 0) {
      itabVec.push_back(mirSymbol);
      continue;
    } else if (mirSymbol->GetName().find(ITAB_CONFLICT_PREFIX_STR) == 0) {
      itabConflictVec.push_back(mirSymbol);
      continue;
    } else if (mirSymbol->GetName().find(kVtabOffsetTabStr) == 0) {
      vtabOffsetVec.push_back(mirSymbol);
      continue;
    } else if (mirSymbol->GetName().find(kFieldOffsetTabStr) == 0) {
      fieldOffsetVec.push_back(mirSymbol);
      continue;
    } else if (mirSymbol->GetName().find(kOffsetTabStr) == 0) {
      valueOffsetVec.push_back(mirSymbol);
      continue;
    } else if (mirSymbol->GetName().find(kLocalClassInfoStr) == 0) {
      localClassInfoVec.push_back(mirSymbol);
      continue;
    } else if (StringUtils::StartsWith(mirSymbol->GetName(), NameMangler::kDecoupleStaticKeyStr)) {
      staticDecoupleKeyVec.push_back(mirSymbol);
      continue;
    } else if (StringUtils::StartsWith(mirSymbol->GetName(), NameMangler::kDecoupleStaticValueStr)) {
      staticDecoupleValueVec.push_back(mirSymbol);
      continue;
    } else if (mirSymbol->IsLiteral()) {
      literalVec.push_back(std::make_pair(mirSymbol, false));
      continue;
    } else if (mirSymbol->IsConstString() || mirSymbol->IsLiteralPtr()) {
      MIRConst *mirConst = mirSymbol->GetKonst();
      if (mirConst != nullptr && mirConst->GetKind() == kConstAddrof) {
        constStrVec.push_back(mirSymbol);
        continue;
      }
    } else if (mirSymbol->IsReflectionClassInfoPtr()) {
      /* _PTR__cinf is emitted in dataDefTab and dataUndefTab */
      continue;
    } else if (mirSymbol->IsMuidTab()) {
      muidVec[0] = mirSymbol;
      EmitMuidTable(muidVec, strIdx2Type, mirSymbol->GetMuidTabName());
      continue;
    } else if (mirSymbol->IsCodeLayoutInfo()) {
      EmitFuncLayoutInfo(*mirSymbol);
      continue;
    } else if (mirSymbol->GetName().find(kStaticFieldNamePrefixStr) == 0) {
      staticFieldsVec.push_back(mirSymbol);
      continue;
    } else if (mirSymbol->GetName().find(kGcRootList) == 0) {
      EmitGlobalRootList(*mirSymbol);
      continue;
    } else if (mirSymbol->GetName().find(kFunctionProfileTabPrefixStr) == 0) {
      muidVec[0] = mirSymbol;
      EmitMuidTable(muidVec, strIdx2Type, kFunctionProfileTabPrefixStr);
      continue;
    } else if (mirSymbol->IsReflectionFieldOffsetData()) {
      fieldOffsetDatas.push_back(mirSymbol);
      continue;
    } else if (mirSymbol->IsReflectionMethodAddrData()) {
      methodAddrDatas.push_back(mirSymbol);
      continue;
    }

    if (mirSymbol->IsReflectionInfo()) {
      if (mirSymbol->IsReflectionClassInfo()) {
        classInfoVec.push_back(mirSymbol);
      }
      continue;
    }
    /* symbols we do not emit here. */
    if (mirSymbol->GetSKind() == kStFunc || mirSymbol->GetSKind() == kStJavaClass ||
        mirSymbol->GetSKind() == kStJavaInterface) {
      continue;
    }
    if (mirSymbol->GetStorageClass() == kScTypeInfo) {
      typeStVec.push_back(mirSymbol);
      continue;
    }
    if (mirSymbol->GetStorageClass() == kScTypeInfoName) {
      typeNameStVec.push_back(mirSymbol);
      continue;
    }
    if (mirSymbol->GetStorageClass() == kScTypeCxxAbi) {
      continue;
    }

    MIRType *mirType = mirSymbol->GetType();
    if (mirType == nullptr) {
      continue;
    }

    /*
     * emit uninitialized global/static variables.
     * these variables store in .comm section.
     */
    if ((mirSymbol->GetStorageClass() == kScGlobal || mirSymbol->GetStorageClass() == kScFstatic) &&
        !mirSymbol->IsConst()) {
      if (mirSymbol->IsGctibSym()) {
        /* GCTIB symbols are generated in GenerateObjectMaps */
        continue;
      }
      if (mirSymbol->GetStorageClass() == kScGlobal) {
        EmitAsmLabel(*mirSymbol, kAsmType);
        EmitAsmLabel(*mirSymbol, kAsmComm);
      } else {
        globalVarVec.push_back(std::make_pair(mirSymbol, false));
      }
      continue;
    }

    EmitAsmLabel(*mirSymbol, kAsmType);
    /* emit initialized global/static variables. */
    if (mirSymbol->GetStorageClass() == kScGlobal ||
        (mirSymbol->GetStorageClass() == kScFstatic && !mirSymbol->IsReadOnly())) {
      /* Emit section */
      if (mirSymbol->IsReflectionStrTab()) {
        std::string sectionName = ".reflection_strtab";
        if (mirSymbol->GetName().find(kReflectionStartHotStrtabPrefixStr) == 0) {
          sectionName = ".reflection_start_hot_strtab";
        } else if (mirSymbol->GetName().find(kReflectionBothHotStrTabPrefixStr) == 0) {
          sectionName = ".reflection_both_hot_strtab";
        } else if (mirSymbol->GetName().find(kReflectionRunHotStrtabPrefixStr) == 0) {
          sectionName = ".reflection_run_hot_strtab";
        }
        Emit("\t.section\t" + sectionName + ",\"a\",%progbits\n");
      } else if (mirSymbol->GetName().find(kDecoupleOption) == 0) {
        Emit("\t.section\t." + std::string(kDecoupleStr) + ",\"a\",%progbits\n");
      } else if (mirSymbol->IsRegJNITab()) {
        Emit("\t.section\t.reg_jni_tab,\"a\", %progbits\n");
      } else if (mirSymbol->GetName().find(kCompilerVersionNum) == 0) {
        Emit("\t.section\t." + std::string(kCompilerVersionNumStr) + ",\"a\", %progbits\n");
      } else if (mirSymbol->GetName().find(kSourceMuid) == 0) {
        Emit("\t.section\t." + std::string(kSourceMuidSectionStr) + ",\"a\", %progbits\n");
      } else if (mirSymbol->GetName().find(kCompilerMfileStatus) == 0) {
        Emit("\t.section\t." + std::string(kCompilerMfileStatus) + ",\"a\", %progbits\n");
      } else if (mirSymbol->IsRegJNIFuncTab()) {
        Emit("\t.section\t.reg_jni_func_tab,\"aw\", %progbits\n");
      } else if (mirSymbol->IsReflectionPrimitiveClassInfo()) {
        Emit("\t.section\t.primitive_classinfo,\"awG\", %progbits,__primitive_classinfo__,comdat\n");
      } else if (mirSymbol->IsReflectionHashTabBucket()) {
        std::string stName = mirSymbol->GetName();
        const std::string delimiter = "$$";
        if (stName.find(delimiter) == std::string::npos) {
          FATAL(kLncFatal, "Can not find delimiter in target ");
        }
        std::string secName = stName.substr(0, stName.find(delimiter));
        /* remove leading "__" in sec name. */
        secName.erase(0, 2);
        Emit("\t.section\t." + secName + ",\"a\",%progbits\n");
      } else {
        EmitAsmLabel(kAsmData);
      }
      /* Emit size and align by type */
      if (mirSymbol->GetStorageClass() == kScGlobal) {
        if (mirSymbol->GetAttr(ATTR_weak) || mirSymbol->IsReflectionPrimitiveClassInfo()) {
          EmitAsmLabel(*mirSymbol, kAsmWeak);
        } else {
          EmitAsmLabel(*mirSymbol, kAsmGlbl);
        }
        EmitAsmLabel(*mirSymbol, kAsmHidden);
      } else if (mirSymbol->GetStorageClass() == kScFstatic) {
        EmitAsmLabel(*mirSymbol, kAsmLocal);
      }
      if (mirSymbol->IsReflectionStrTab()) {
        Emit("\t.align 3\n");  /* reflection-string-tab also aligned to 8B boundaries. */
      } else {
        EmitAsmLabel(*mirSymbol, kAsmAlign);
      }
      EmitAsmLabel(*mirSymbol, kAsmSyname);
      MIRConst *mirConst = mirSymbol->GetKonst();
      if (IsPrimitiveScalar(mirType->GetPrimType())) {
        EmitScalarConstant(*mirConst);
      } else if (mirType->GetKind() == kTypeArray) {
        if (mirSymbol->HasAddrOfValues()) {
          EmitConstantTable(*mirSymbol, *mirConst, strIdx2Type);
        } else {
          EmitArrayConstant(*mirConst);
        }
      } else if (mirType->GetKind() == kTypeStruct || mirType->GetKind() == kTypeClass) {
        if (mirSymbol->HasAddrOfValues()) {
          EmitConstantTable(*mirSymbol, *mirConst, strIdx2Type);
        } else {
          EmitStructConstant(*mirConst);
        }
      } else {
        ASSERT(false, "NYI");
      }
      EmitAsmLabel(*mirSymbol, kAsmSize);
      /* emit constant float/double */
    } else if (mirSymbol->IsReadOnly()) {
      /* emit .section */
      Emit(asmInfo->GetSection());
      Emit(asmInfo->GetRodata());
      Emit("\n");
      EmitAsmLabel(*mirSymbol, kAsmAlign);
      EmitAsmLabel(*mirSymbol, kAsmSyname);
      MIRConst *mirConst = mirSymbol->GetKonst();
      EmitScalarConstant(*mirConst);
    }
  } /* end proccess all mirSymbols. */
  /* emit global var */
  EmitGlobalVars(globalVarVec);
  /* emit literal std::strings */
  EmitLiterals(literalVec, strIdx2Type);
  /* emit static field std::strings */
  EmitStaticFields(staticFieldsVec);

  EmitMuidTable(constStrVec, strIdx2Type, kMuidConststrPrefixStr);

  /* emit classinfo, field, method */
  std::vector<MIRSymbol*> superClassStVec;
  std::vector<MIRSymbol*> fieldInfoStVec;
  std::vector<MIRSymbol*> fieldInfoStCompactVec;
  std::vector<MIRSymbol*> methodInfoStVec;
  std::vector<MIRSymbol*> methodInfoStCompactVec;

  std::string sectionName = kMuidClassMetadataPrefixStr;
  Emit("\t.section ." + sectionName + ",\"aw\",%progbits\n");
  Emit(sectionName + "_begin:\n");

  for (size_t i = 0; i < classInfoVec.size(); ++i) {
    MIRSymbol *mirSymbol = classInfoVec[i];
    if (mirSymbol != nullptr && mirSymbol->GetKonst() != nullptr && mirSymbol->IsReflectionClassInfo()) {
      /* Emit classinfo */
      EmitClassInfoSequential(*mirSymbol, strIdx2Type, sectionName);
      std::string stName = mirSymbol->GetName();
      std::string className = stName.substr(strlen(CLASSINFO_PREFIX_STR));
      /* Get classinfo ro symbol */
      MIRSymbol *classInfoROSt = GlobalTables::GetGsymTable().GetSymbolFromStrIdx(
        GlobalTables::GetStrTable().GetStrIdxFromName(CLASSINFO_RO_PREFIX_STR + className));
      EmitClassInfoSequential(*classInfoROSt, strIdx2Type, sectionName);
      /* Get fields */
      MIRSymbol *fieldSt = GlobalTables::GetGsymTable().GetSymbolFromStrIdx(
        GlobalTables::GetStrTable().GetStrIdxFromName(kFieldsInfoPrefixStr + className));
      MIRSymbol *fieldStCompact = GlobalTables::GetGsymTable().GetSymbolFromStrIdx(
        GlobalTables::GetStrTable().GetStrIdxFromName(kFieldsInfoCompactPrefixStr + className));
      /* Get methods */
      MIRSymbol *methodSt = GlobalTables::GetGsymTable().GetSymbolFromStrIdx(
        GlobalTables::GetStrTable().GetStrIdxFromName(kMethodsInfoPrefixStr + className));
      MIRSymbol *methodStCompact = GlobalTables::GetGsymTable().GetSymbolFromStrIdx(
        GlobalTables::GetStrTable().GetStrIdxFromName(kMethodsInfoCompactPrefixStr + className));
      /* Get superclass */
      MIRSymbol *superClassSt = GlobalTables::GetGsymTable().GetSymbolFromStrIdx(
        GlobalTables::GetStrTable().GetStrIdxFromName(SUPERCLASSINFO_PREFIX_STR + className));

      if (fieldSt != nullptr) {
        fieldInfoStVec.push_back(fieldSt);
      }
      if (fieldStCompact != nullptr) {
        fieldInfoStCompactVec.push_back(fieldStCompact);
      }
      if (methodSt != nullptr) {
        methodInfoStVec.push_back(methodSt);
      }
      if (methodStCompact != nullptr) {
        methodInfoStCompactVec.push_back(methodStCompact);
      }
      if (superClassSt != nullptr) {
        superClassStVec.push_back(superClassSt);
      }
    }
  }
  Emit(sectionName + "_end:\n");

  std::vector<MIRSymbol*> hotVtabStVec;
  std::vector<MIRSymbol*> coldVtabStVec;
  std::vector<MIRSymbol*> hotItabStVec;
  std::vector<MIRSymbol*> coldItabStVec;
  std::vector<MIRSymbol*> hotItabCStVec;
  std::vector<MIRSymbol*> coldItabCStVec;
  std::vector<MIRSymbol*> hotMethodsInfoCStVec;
  std::vector<MIRSymbol*> coldMethodsInfoCStVec;
  std::vector<MIRSymbol*> hotFieldsInfoCStVec;
  std::vector<MIRSymbol*> coldFieldsInfoCStVec;
  std::vector<MIRSymbol*> hotSuperClassStVec;
  std::vector<MIRSymbol*> coldSuperClassStVec;
  GetHotAndColdMetaSymbolInfo(vtabVec, hotVtabStVec, coldVtabStVec, VTAB_PREFIX_STR,
                              ((CGOptions::IsLazyBinding() || CGOptions::IsHotFix()) && !cg->IsLibcore()));
  GetHotAndColdMetaSymbolInfo(itabVec, hotItabStVec, coldItabStVec, ITAB_PREFIX_STR,
                              ((CGOptions::IsLazyBinding() || CGOptions::IsHotFix()) && !cg->IsLibcore()));
  GetHotAndColdMetaSymbolInfo(itabConflictVec, hotItabCStVec, coldItabCStVec, ITAB_CONFLICT_PREFIX_STR,
                              ((CGOptions::IsLazyBinding() || CGOptions::IsHotFix()) && !cg->IsLibcore()));
  GetHotAndColdMetaSymbolInfo(superClassStVec, hotSuperClassStVec, coldSuperClassStVec, SUPERCLASSINFO_PREFIX_STR,
                              ((CGOptions::IsLazyBinding() || CGOptions::IsHotFix()) && !cg->IsLibcore()));
  GetHotAndColdMetaSymbolInfo(fieldInfoStVec, hotFieldsInfoCStVec, coldFieldsInfoCStVec, kFieldsInfoPrefixStr);
  GetHotAndColdMetaSymbolInfo(methodInfoStVec, hotMethodsInfoCStVec, coldMethodsInfoCStVec, kMethodsInfoPrefixStr);

  std::string sectionNameIsEmpty;
  std::string fieldSectionName("rometadata.field");
  std::string methodSectionName("rometadata.method");

  /* fieldinfo */
  EmitMetaDataSymbolWithMarkFlag(hotFieldsInfoCStVec, strIdx2Type, kFieldsInfoPrefixStr, fieldSectionName, true);
  EmitMetaDataSymbolWithMarkFlag(coldFieldsInfoCStVec, strIdx2Type, kFieldsInfoPrefixStr, fieldSectionName, false);
  EmitMetaDataSymbolWithMarkFlag(fieldInfoStCompactVec, strIdx2Type, kFieldsInfoCompactPrefixStr, fieldSectionName,
                                 false);
  /* methodinfo */
  EmitMetaDataSymbolWithMarkFlag(hotMethodsInfoCStVec, strIdx2Type, kMethodsInfoPrefixStr, methodSectionName, true);
  EmitMetaDataSymbolWithMarkFlag(coldMethodsInfoCStVec, strIdx2Type, kMethodsInfoPrefixStr, methodSectionName, false);
  EmitMetaDataSymbolWithMarkFlag(methodInfoStCompactVec, strIdx2Type, kMethodsInfoCompactPrefixStr, methodSectionName,
                                 false);

  /* itabConflict */
  MarkVtabOrItabEndFlag(coldItabCStVec);
  EmitMuidTable(hotItabCStVec, strIdx2Type, kMuidItabPrefixStr);
  EmitMetaDataSymbolWithMarkFlag(coldItabCStVec, strIdx2Type, ITAB_PREFIX_STR, sectionNameIsEmpty, false);

  /*
   * vtab
   * And itab to vtab section
   */
  for (auto sym : hotItabStVec) {
    hotVtabStVec.push_back(sym);
  }
  for (auto sym : coldItabStVec) {
    coldVtabStVec.push_back(sym);
  }
  MarkVtabOrItabEndFlag(coldVtabStVec);
  EmitMuidTable(hotVtabStVec, strIdx2Type, kMuidVtabPrefixStr);
  EmitMetaDataSymbolWithMarkFlag(coldVtabStVec, strIdx2Type, VTAB_PREFIX_STR, sectionNameIsEmpty, false);

  /* vtab_offset */
  EmitMuidTable(vtabOffsetVec, strIdx2Type, kMuidVtabOffsetPrefixStr);
  /* field_offset */
  EmitMuidTable(fieldOffsetVec, strIdx2Type, kMuidFieldOffsetPrefixStr);
  /* value_offset */
  EmitMuidTable(valueOffsetVec, strIdx2Type, kMuidValueOffsetPrefixStr);
  /* local clasinfo */
  EmitMuidTable(localClassInfoVec, strIdx2Type, kMuidLocalClassInfoStr);
  /* Emit decouple static */
  EmitMuidTable(staticDecoupleKeyVec, strIdx2Type, kDecoupleStaticKeyStr);
  EmitMuidTable(staticDecoupleValueVec, strIdx2Type, kDecoupleStaticValueStr);

  /* super class */
  EmitMuidTable(hotSuperClassStVec, strIdx2Type, kMuidSuperclassPrefixStr);
  EmitMetaDataSymbolWithMarkFlag(coldSuperClassStVec, strIdx2Type, SUPERCLASSINFO_PREFIX_STR, sectionNameIsEmpty,
                                 false);

  /* field offset rw */
  EmitMetaDataSymbolWithMarkFlag(fieldOffsetDatas, strIdx2Type, kFieldOffsetDataPrefixStr, sectionNameIsEmpty, false);
  /* method address rw */
  EmitMetaDataSymbolWithMarkFlag(methodAddrDatas, strIdx2Type, kMethodAddrDataPrefixStr, sectionNameIsEmpty, false);

#if !defined(TARGARM32)
  /* finally emit __gxx_personality_v0 DW.ref */
  EmitDWRef("__mpl_personality_v0");
#endif
}
void Emitter::EmitAddressString(const std::string &address) {
#if TARGAARCH64
  Emit("\t.quad\t" + address);
#else
  Emit("\t.word\t" + address);
#endif
}
void Emitter::EmitGlobalRootList(const MIRSymbol &gcrootsSt) {
  Emit("\t.section .maple.gcrootsmap").Emit(",\"aw\",%progbits\n");
  std::vector<std::string> nameVec;
  std::string name = gcrootsSt.GetName();
  nameVec.push_back(name);
  nameVec.push_back(name + "Size");
  bool gcrootsFlag = true;
  uint64 vecSize = 0;
  for (const auto &gcrootsName : nameVec) {
#if TARGAARCH64
    Emit("\t.type\t" + gcrootsName + ", @object\n" + "\t.p2align 3\n");
#else
    Emit("\t.type\t" + gcrootsName + ", %object\n" + "\t.p2align 3\n");
#endif
    Emit("\t.global\t" + gcrootsName + "\n");
    if (gcrootsFlag) {
      Emit(kMuidGlobalRootlistPrefixStr).Emit("_begin:\n");
    }
    Emit(gcrootsName + ":\n");
    if (gcrootsFlag) {
      MIRAggConst *aggConst = safe_cast<MIRAggConst>(gcrootsSt.GetKonst());
      if (aggConst == nullptr) {
        continue;
      }
      size_t i = 0;
      while (i < aggConst->GetConstVec().size()) {
        MIRConst *elemConst = aggConst->GetConstVecItem(i);
        if (elemConst->GetKind() == kConstAddrof) {
          MIRAddrofConst *symAddr = safe_cast<MIRAddrofConst>(elemConst);
          CHECK_FATAL(symAddr != nullptr, "nullptr of symAddr");
          MIRSymbol *symAddrSym = GlobalTables::GetGsymTable().GetSymbolFromStidx(symAddr->GetSymbolIndex().Idx());
          const std::string &symAddrName = symAddrSym->GetName();
          EmitAddressString(symAddrName + "\n");
        } else {
          EmitScalarConstant(*elemConst);
        }
        i++;
      }
      vecSize = i;
    } else {
      EmitAddressString(std::to_string(vecSize) + "\n");
    }
    Emit("\t.size\t" + gcrootsName + ",.-").Emit(gcrootsName + "\n");
    if (gcrootsFlag) {
      Emit(kMuidGlobalRootlistPrefixStr).Emit("_end:\n");
    }
    gcrootsFlag = false;
  }
}

void Emitter::EmitMuidTable(const std::vector<MIRSymbol*> &vec, const std::map<GStrIdx, MIRType*> &strIdx2Type,
                            const std::string &sectionName) {
  MIRSymbol *st = nullptr;
  if (!vec.empty()) {
    st = vec[0];
  }
  if (st != nullptr && st->IsMuidRoTab()) {
    Emit("\t.section  ." + sectionName + ",\"a\",%progbits\n");
  } else {
    Emit("\t.section  ." + sectionName + ",\"aw\",%progbits\n");
  }
  Emit(sectionName + "_begin:\n");
  bool isConstString = sectionName == kMuidConststrPrefixStr;
  for (size_t i = 0; i < vec.size(); i++) {
    MIRSymbol *st1 = vec[i];
    ASSERT(st1 != nullptr, "null ptr check");
    if (st1->GetStorageClass() == kScUnused) {
      continue;
    }
    EmitAsmLabel(*st1, kAsmType);
    if (st1->GetStorageClass() == kScFstatic) {
      EmitAsmLabel(*st1, kAsmLocal);
    } else {
      EmitAsmLabel(*st1, kAsmGlbl);
      EmitAsmLabel(*st1, kAsmHidden);
    }
    EmitAsmLabel(*st1, kAsmAlign);
    EmitAsmLabel(*st1, kAsmSyname);
    MIRConst *mirConst = st1->GetKonst();
    CHECK_FATAL(mirConst != nullptr, "mirConst should not be nullptr in EmitMuidTable");
    if (mirConst->GetKind() == kConstAddrof) {
      MIRAddrofConst *symAddr = safe_cast<MIRAddrofConst>(mirConst);
      CHECK_FATAL(symAddr != nullptr, "call static_cast failed in EmitMuidTable");
      MIRSymbol *symAddrSym = GlobalTables::GetGsymTable().GetSymbolFromStidx(symAddr->GetSymbolIndex().Idx());
      if (isConstString) {
        EmitAddressString(symAddrSym->GetName() + " - . + ");
        Emit(kDataRefIsOffset);
        Emit("\n");
      } else {
        EmitAddressString(symAddrSym->GetName() + "\n");
      }
    } else if (mirConst->GetKind() == kConstInt) {
      EmitScalarConstant(*mirConst, true);
    } else {
      EmitConstantTable(*st1, *mirConst, strIdx2Type);
    }
    EmitAsmLabel(*st1, kAsmSize);
  }
  Emit(sectionName + "_end:\n");
}

void Emitter::EmitClassInfoSequential(const MIRSymbol &mirSymbol, const std::map<GStrIdx, MIRType*> &strIdx2Type,
                                      const std::string &sectionName) {
  EmitAsmLabel(mirSymbol, kAsmType);
  if (!sectionName.empty()) {
    Emit("\t.section ." + sectionName);
    if (StringUtils::StartsWith(sectionName, "ro")) {
      Emit(",\"a\",%progbits\n");
    } else {
      Emit(",\"aw\",%progbits\n");
    }
  } else {
    EmitAsmLabel(kAsmData);
  }
  EmitAsmLabel(mirSymbol, kAsmGlbl);
  EmitAsmLabel(mirSymbol, kAsmHidden);
  EmitAsmLabel(mirSymbol, kAsmAlign);
  EmitAsmLabel(mirSymbol, kAsmSyname);
  MIRConst *mirConst = mirSymbol.GetKonst();
  CHECK_FATAL(mirConst != nullptr, "mirConst should not be nullptr in EmitClassInfoSequential");
  EmitConstantTable(mirSymbol, *mirConst, strIdx2Type);
  EmitAsmLabel(mirSymbol, kAsmSize);
}

void Emitter::EmitMethodDeclaringClass(const MIRSymbol &mirSymbol, const std::string &sectionName) {
  std::string symName = mirSymbol.GetName();
  std::string emitSyName = symName + "_DeclaringClass";
  std::string declaringClassName = symName.substr(strlen(kFieldsInfoCompactPrefixStr) + 1);
  Emit(asmInfo->GetType());
  Emit(emitSyName + ", %object\n");
  if (!sectionName.empty()) {
    Emit("\t.section  ." + sectionName + "\n");
  } else {
    EmitAsmLabel(kAsmData);
  }
  Emit(asmInfo->GetLocal());
  Emit(emitSyName + "\n");
  Emit(asmInfo->GetAlign());
  Emit("  2\n" + emitSyName + ":\n");
  Emit("\t.long\t");
  Emit(CLASSINFO_PREFIX_STR + declaringClassName + " - .\n");
  Emit(asmInfo->GetSize());
  Emit(emitSyName + ", 4\n");
}

void Emitter::EmitMethodFieldSequential(const MIRSymbol &mirSymbol,
                                        const std::map<GStrIdx, MIRType*> &strIdx2Type,
                                        const std::string &sectionName) {
  std::string symName = mirSymbol.GetName();
  if (symName.find(kMethodsInfoCompactPrefixStr) != std::string::npos) {
    EmitMethodDeclaringClass(mirSymbol, sectionName);
  }
  EmitAsmLabel(mirSymbol, kAsmType);
  if (!sectionName.empty()) {
    Emit("\t.section  ." + sectionName + "\n");
  } else {
    EmitAsmLabel(kAsmData);
  }
  EmitAsmLabel(mirSymbol, kAsmLocal);

  /* Emit(2) is 4 bit align */
  Emit(asmInfo->GetAlign()).Emit(2).Emit("\n");
  EmitAsmLabel(mirSymbol, kAsmSyname);
  MIRConst *ct = mirSymbol.GetKonst();
  EmitConstantTable(mirSymbol, *ct, strIdx2Type);
  std::string symbolName = mirSymbol.GetName();
  Emit("\t.size\t" + symbolName + ", .-");
  Emit(symbolName + "\n");
}

void Emitter::EmitDWRef(const std::string &name) {
  /*
   *   .hidden DW.ref._ZTI3xxx
   *   .weak DW.ref._ZTI3xxx
   *   .section  .data.DW.ref._ZTI3xxx,"awG",@progbits,DW.ref._ZTI3xxx,comdat
   *   .align  3
   *   .type DW.ref._ZTI3xxx, %object
   *   .size DW.ref._ZTI3xxx, 8
   * DW.ref._ZTI3xxx:
   *   .xword  _ZTI3xxx
   */
  Emit("\t.hidden DW.ref." + name + "\n");
  Emit("\t.weak DW.ref." + name + "\n");
  Emit("\t.section .data.DW.ref." + name + ",\"awG\",%progbits,DW.ref.");
  Emit(name + ",comdat\n");
  Emit("\t.align 3\n");
  Emit("\t.type DW.ref." + name + ", \%object\n");
  Emit("\t.size DW.ref." + name + ",8\n");
  Emit("DW.ref." + name + ":\n");
#if TARGAARCH64
  Emit("\t.xword " + name + "\n");
#else
  Emit("\t.word " + name + "\n");
#endif
}

void Emitter::EmitDecSigned(int64 num) {
  std::ios::fmtflags flag(outStream.flags());
  outStream << std::dec << num;
  outStream.flags(flag);
}

void Emitter::EmitDecUnsigned(uint64 num) {
  std::ios::fmtflags flag(outStream.flags());
  outStream << std::dec << num;
  outStream.flags(flag);
}

void Emitter::EmitHexUnsigned(uint64 num) {
  std::ios::fmtflags flag(outStream.flags());
  outStream << "0x" << std::hex << num;
  outStream.flags(flag);
}


#define XSTR(s) str(s)
#define str(s) #s

void Emitter::EmitDIHeader() {
  Emit(".L" XSTR(TEXT_BEGIN) ":\n");
}

void Emitter::EmitDIFooter() {
  Emit("\t.text\n");
  Emit(".L" XSTR(TEXT_END) ":\n");
}

void Emitter::EmitDIHeaderFileInfo() {
  Emit("// dummy header file 1\n");
  Emit("// dummy header file 2\n");
  Emit("// dummy header file 3\n");
}


void Emitter::EmitHugeSoRoutines(bool lastRoutine) {
  if (!lastRoutine && (javaInsnCount < (static_cast<uint64>(hugeSoSeqence) *
                                        static_cast<uint64>(kHugeSoInsnCountThreshold)))) {
    return;
  }
  for (auto &target : hugeSoTargets) {
    Emit("\t.section ." + std::string(NameMangler::kMuidJavatextPrefixStr) + ",\"ax\"\n");
    Emit("\t.align 2\n");
    std::string routineName = target + HugeSoPostFix();
    Emit("\t.type\t" + routineName + ", %function\n");
    Emit(routineName + ":\n");
    Emit("\tadrp\tx17, :got:" + target + "\n");
    Emit("\tldr\tx17, [x17, :got_lo12:" + target + "]\n");
    Emit("\tbr\tx17\n");
    javaInsnCount += kSizeOfHugesoRoutine;
  }
  hugeSoTargets.clear();
  ++hugeSoSeqence;
}

void ImmOperand::Dump() const {
  LogInfo::MapleLogger() << "imm:" << value;
}

void LabelOperand::Emit(Emitter &emitter, const OpndProp *opndProp) const {
  (void)opndProp;
  emitter.Emit(".Label.").Emit(parentFunc).Emit(".").Emit(labelIndex);
}

void LabelOperand::Dump() const {
  LogInfo::MapleLogger() << "label:" << labelIndex;
}
}  /* namespace maplebe */
