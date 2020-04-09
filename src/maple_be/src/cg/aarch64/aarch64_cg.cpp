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
#include "aarch64_cg.h"
#include "aarch64_rt.h"
#include "mir_builder.h"
#include "becommon.h"

namespace maplebe {
#include "aarch64_opnd.def"
const AArch64MD AArch64CG::kMd[kMopLast] = {
#include "aarch64_md.def"
};

std::array<std::array<const std::string, kAllRegNum>, kIntRegTypeNum> AArch64CG::intRegNames = {
  std::array<const std::string, kAllRegNum> {
    "err",   "err0",  "err1",  "err2",  "err3",  "err4",  "err5",  "err6",  "err7",  "err8",  "err9",  "err10",
    "err11", "err12", "err13", "err14", "err15", "err16", "err17", "err18", "err19", "err20", "err21", "err22",
    "err23", "err24", "err25", "err26", "err27", "err28", "err",   "err",   "errsp", "errzr",  /* x29 is fp */
    "b0",    "b1",    "b2",    "b3",    "b4",    "b5",    "b6",    "b7",    "b8",    "b9",    "b10",   "b11",
    "b12",   "b13",   "b14",   "b15",   "b16",   "b17",   "b18",   "b19",   "b20",   "b21",   "b22",   "b23",
    "b24",   "b25",   "b26",   "b27",   "b28",   "b29",   "b30",   "b31",   "errMaxRegNum",   "rflag" },
  std::array<const std::string, kAllRegNum> {
    "err",   "err0",  "err1",  "err2",  "err3",  "err4",  "err5",  "err6",  "err7",  "err8",  "err9",  "err10",
    "err11", "err12", "err13", "err14", "err15", "err16", "err17", "err18", "err19", "err20", "err21", "err22",
    "err23", "err24", "err25", "err26", "err27", "err28", "err29", "err30", "errsp", "errzr",   /* x29 is fp */
    "h0",    "h1",    "h2",    "h3",    "h4",    "h5",    "h6",    "h7",    "h8",    "h9",    "h10",   "h11",
    "h12",   "h13",   "h14",   "h15",   "h16",   "h17",   "h18",   "h19",   "h20",   "h21",   "h22",   "h23",
    "h24",   "h25",   "h26",   "h27",   "h28",   "h29",   "h30",   "h31",   "errMaxRegNum",   "rflag" },
  std::array<const std::string, kAllRegNum> {
    "err", "w0",  "w1",  "w2",  "w3",  "w4",  "w5",  "w6",  "w7",  "w8",  "w9",  "w10", "w11", "w12", "w13", "w14",
    "w15", "w16", "w17", "w18", "w19", "w20", "w21", "w22", "w23", "w24", "w25", "w26", "w27", "w28", "err", "err",
    "wsp", "wzr", /* x29 is fp */
    "s0",  "s1",  "s2",  "s3",  "s4",  "s5",  "s6",  "s7",  "s8",  "s9",  "s10", "s11", "s12", "s13", "s14", "s15",
    "s16", "s17", "s18", "s19", "s20", "s21", "s22", "s23", "s24", "s25", "s26", "s27", "s28", "s29", "s30", "s31",
    "errMaxRegNum", "rflag" },
  std::array<const std::string, kAllRegNum> {
    "err", "x0",  "x1",  "x2",  "x3",  "x4",  "x5",  "x6",  "x7",  "x8",  "x9",  "x10", "x11", "x12", "x13", "x14",
    "x15", "x16", "x17", "x18", "x19", "x20", "x21", "x22", "x23", "x24", "x25", "x26", "x27", "x28", "x29", "x30",
    "sp",  "xzr", /* x29 is fp */
    "d0",  "d1",  "d2",  "d3",  "d4",  "d5",  "d6",  "d7",  "d8",  "d9",  "d10", "d11", "d12", "d13", "d14", "d15",
    "d16", "d17", "d18", "d19", "d20", "d21", "d22", "d23", "d24", "d25", "d26", "d27", "d28", "d29", "d30", "d31",
    "errMaxRegNum", "rflag" },
  std::array<const std::string, kAllRegNum> {
    "err", "x0",  "x1",  "x2",  "x3",  "x4",  "x5",  "x6",  "x7",  "x8",  "x9",  "x10", "x11", "x12", "x13", "x14",
    "x15", "x16", "x17", "x18", "x19", "x20", "x21", "x22", "x23", "x24", "x25", "x26", "x27", "x28", "x29", "x30",
    "sp",  "xzr", /* x29 is fp */
    "v0",  "v1",  "v2",  "v3",  "v4",  "v5",  "v6",  "v7",  "v8",  "v9",  "v10", "v11", "v12", "v13", "v14", "v15",
    "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31",
    "errMaxRegNum", "rflag" }
};

bool AArch64CG::IsExclusiveFunc(MIRFunction &mirFunc) {
  const std::string &funcName = mirFunc.GetName();
  for (const auto &it : ehExclusiveNameVec) {
    if (it.compare(funcName) == 0) {
      return true;
    }
  }
  return false;
}
namespace wordsMap {
  /*
   * Generate object maps.
   *
   * 1. each class record its GCTIB in method meta (not read only meta)
   * 2. GCTIB include: header protoType; n bitmap word; bitmap word
   * 3. each reference word(4 or 8 bytes) is represented by 2 bits
   *    00: not ref
   *    01: normal ref
   *    10: weak ref
   *    11: unowned ref
   *
   * For example, if a scalar object has five ptr fields at offsets 24, 40(weak),
   * 64(unowned), the generated code will be like:
   *
   * MCC_GCTIB__xxx:
   * .long 0x40      // object has child reference
   * .long 1         // one word in the bitmap
   * .quad 0b110000100001000000
   * ...
   */
  const uint32 kRefWordsPerMapWord = 32;    /* contains bitmap for 32 ref words in 64 bits */
  const uint32 kLogRefWordsPerMapWord = 5;
#ifdef USE_32BIT_REF
  const uint32 kReferenceWordSize = 4;
  const uint32 kLog2ReferenceWordSize = 2;
#else
  const uint32 kReferenceWordSize = 8;
  const uint32 kLog2ReferenceWordSize = 3;
#endif
  const uint32 kInMapWordOffsetMask = ((kReferenceWordSize * kRefWordsPerMapWord) - 1);
  const uint32 kInMapWordIndexShift = (kLog2ReferenceWordSize - 1);
  const uint32 kMapWordIndexShift = (kLog2ReferenceWordSize + kLogRefWordsPerMapWord);

  const uint64 kRefBits = 1;
  const uint64 kWeakRefBits = 2;
  const uint64 kUnownedRefBits = 3;

  /*
   * Give a structrue type, calculate its bitmap_vector
   */
  static void GetGCTIBBitMapWords(const BECommon &beCommon, MIRStructType &stType, std::vector<uint64> &bitmapWords) {
    bitmapWords.clear();
    if (stType.GetKind() == kTypeClass) {
      uint64 curBitmap = 0;
      uint32 curBitmapIndex = 0;
      uint32 prevOffset = 0;
      for (const auto &fieldInfo : beCommon.GetJClassLayout(static_cast<MIRClassType&>(stType))) {
        if (fieldInfo.IsRef()) {
          uint32 curOffset = fieldInfo.GetOffset();
          /* skip meta field */
          if (curOffset == 0) {
            continue;
          }
          CHECK_FATAL((curOffset > prevOffset) || (prevOffset == 0), "not ascending offset");
          uint32 wordIndex = curOffset >> kMapWordIndexShift;
          if (wordIndex > curBitmapIndex) {
            bitmapWords.push_back(curBitmap);
            for (uint32 i = curBitmapIndex + 1; i < wordIndex; i++) {
              bitmapWords.push_back(0);
            }
            curBitmap = 0;
            curBitmapIndex = wordIndex;
          }
          uint32 bitOffset = (curOffset & kInMapWordOffsetMask) >> kInMapWordIndexShift;
          if (CGOptions::IsGCOnly()) {
            /* ignore unowned/weak when GCONLY is enabled. */
            curBitmap |= (kRefBits << bitOffset);
          } else if (fieldInfo.IsUnowned()) {
            curBitmap |= (kUnownedRefBits << bitOffset);
          } else if (fieldInfo.IsWeak()) {
            curBitmap |= (kWeakRefBits << bitOffset);
          } else {
            /* ref */
            curBitmap |= (kRefBits << bitOffset);
          }
          prevOffset = curOffset;
        }
      }
      if (curBitmap != 0) {
        bitmapWords.push_back(curBitmap);
      }
    } else if (stType.GetKind() != kTypeInterface) {
      /* interface doesn't have reference fields */
      CHECK_FATAL(false, "GetGCTIBBitMapWords unexpected type");
    }
  }
}
/*
 * Find if there exist same GCTIB (both rcheader and bitmap are same)
 * for different class. If ture reuse, if not emit and record new GCTIB.
 */
void AArch64CG::FindOrCreateRepresentiveSym(std::vector<uint64> &bitmapWords, uint32 rcHeader,
                                            const std::string &name) {
  GCTIBKey *key = memPool->New<GCTIBKey>(allocator, rcHeader, bitmapWords);
  const std::string &gcTIBName = GCTIB_PREFIX_STR + name;
  MapleUnorderedMap<GCTIBKey*, GCTIBPattern*, Hasher, EqualFn>::const_iterator iter = keyPatternMap.find(key);
  if (iter == keyPatternMap.end()) {
    /* Emit the GCTIB label for the class */
    GCTIBPattern *ptn = memPool->New<GCTIBPattern>(*key, *memPool);

    if (gcTIBName.compare("MCC_GCTIB__Ljava_2Flang_2FObject_3B") == 0) {
      ptn->SetName("MCC_GCTIB__Ljava_2Flang_2FObject_3B");
    }
    keyPatternMap.insert(std::make_pair(key, ptn));
    symbolPatternMap.insert(std::make_pair(gcTIBName, ptn));

    /* Emit GCTIB pattern */
    std::string ptnString = "\t.type " + ptn->GetName() + ", %object\n" + "\t.data\n" + "\t.align 3\n";

    MIRSymbol *gcTIBSymbol = GlobalTables::GetGsymTable().GetSymbolFromStrIdx(
        GlobalTables::GetStrTable().GetStrIdxFromName(NameMangler::GetInternalNameLiteral(gcTIBName)));
    if (gcTIBSymbol != nullptr && gcTIBSymbol->GetStorageClass() == kScFstatic) {
      ptnString += "\t.local ";
    } else {
      ptnString += "\t.global ";
    }

    Emitter *emitter = GetEmitter();
    emitter->Emit(ptnString);
    emitter->Emit(ptn->GetName());
    emitter->Emit("\n");

    /* Emit the GCTIB pattern label for the class */
    emitter->Emit(ptn->GetName());
    emitter->Emit(":\n");

    emitter->Emit("\t.long ");
    emitter->EmitHexUnsigned(rcHeader);
    emitter->Emit("\n");

    /* generate n_bitmap word */
    emitter->Emit("\t.long ");   /* AArch64-specific. Generate a 64-bit value. */
    emitter->EmitDecUnsigned(bitmapWords.size());
    emitter->Emit("\n");

    /* Emit each bitmap word */
    for (const auto &bitmapWord : bitmapWords) {
      if (!IsQuiet()) {
        LogInfo::MapleLogger() << "  bitmap_word: 0x"<< bitmapWord << " " << PRIx64 << "\n";
      }
      emitter->Emit("\t.quad ");  /* AArch64-specific. Generate a 64-bit value. */
      emitter->EmitHexUnsigned(bitmapWord);
      emitter->Emit("\n");
    }
    if (gcTIBSymbol != nullptr && gcTIBSymbol->GetStorageClass() != kScFstatic) {
      /* add local symbol REF_XXX to every global GCTIB symbol */
      CreateRefSymForGlobalPtn(*ptn);
      keyPatternMap[key] = ptn;
    }
  } else {
    symbolPatternMap.insert(make_pair(gcTIBName, iter->second));
  }
}

/*
 * Add local symbol REF_XXX to global GCTIB symbol,
 * and replace the global GCTIBPattern in keyPatternMap.
 */
void AArch64CG::CreateRefSymForGlobalPtn(GCTIBPattern &ptn) {
  const std::string &refPtnString = REF_PREFIX_STR + ptn.GetName();
  const std::string &ptnString = "\t.type " + refPtnString + ", %object\n" +
                                 "\t.data\n"  +
                                 "\t.align 3\n" +
                                 "\t.local " + refPtnString + "\n" +
                                 refPtnString + ":\n" +
                                 "\t.quad " + ptn.GetName() + "\n";
  Emitter *emitter = GetEmitter();
  emitter->Emit(ptnString);
  ptn.SetName(refPtnString);
}

std::string AArch64CG::FindGCTIBPatternName(const std::string &name) const {
  auto iter = symbolPatternMap.find(name);
  if (iter == symbolPatternMap.end()) {
    CHECK_FATAL(false, "No GCTIB pattern found for symbol: %s", name.c_str());
  }
  return iter->second->GetName();
}

void AArch64CG::GenerateObjectMaps(BECommon &beCommon) {
  if (!IsQuiet()) {
    LogInfo::MapleLogger() << "DEBUG: Generating object maps...\n";
  }

  for (auto &tyId : GetMIRModule()->GetClassList()) {
    if (!IsQuiet()) {
      LogInfo::MapleLogger() << "Class tyIdx: " << tyId << "\n";
    }
    TyIdx tyIdx(tyId);
    MIRType *ty = GlobalTables::GetTypeTable().GetTypeFromTyIdx(tyIdx);
    ASSERT(ty != nullptr, "ty nullptr check");
    /* Only emit GCTIB for classes owned by this module */
    ASSERT(ty->IsStructType(), "ty isn't MIRStructType* in AArch64CG::GenerateObjectMaps");
    MIRStructType *strTy = static_cast<MIRStructType*>(ty);
    if (!strTy->IsLocal()) {
      continue;
    }

    GStrIdx nameIdx = ty->GetNameStrIdx();

    const std::string &name = GlobalTables::GetStrTable().GetStringFromStrIdx(nameIdx);

    /* Emit for a class */
    if (!IsQuiet()) {
      LogInfo::MapleLogger() << "  name: " << name << "\n";
    }

    std::vector<uint64> bitmapWords;
    wordsMap::GetGCTIBBitMapWords(beCommon, *strTy, bitmapWords);
    /* fill specific header according to the size of bitmapWords */
    uint32 rcHeader = (!bitmapWords.empty()) ? 0x40 : 0;
    FindOrCreateRepresentiveSym(bitmapWords, rcHeader, name);
  }
}
}  /* namespace maplebe */
