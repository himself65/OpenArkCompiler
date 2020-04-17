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
#include "becommon.h"
#include <cinttypes>
#include <list>
#include "aarch64_rt.h"
#include "cg_option.h"
#include "mir_builder.h"
#include "mpl_logging.h"

namespace maplebe {
using namespace maple;

BECommon::BECommon(MIRModule &mod)
    : mirModule(mod),
      typeSizeTable(GlobalTables::GetTypeTable().GetTypeTable().size(), 0, mirModule.GetMPAllocator().Adapter()),
      tableAlignTable(GlobalTables::GetTypeTable().GetTypeTable().size(), 0, mirModule.GetMPAllocator().Adapter()),
      structFieldCountTable(GlobalTables::GetTypeTable().GetTypeTable().size(),
                            0, mirModule.GetMPAllocator().Adapter()),
      jClassLayoutTable(mirModule.GetMPAllocator().Adapter()) {
    for (uint32 i = 1; i < GlobalTables::GetTypeTable().GetTypeTable().size(); ++i) {
      MIRType *ty = GlobalTables::GetTypeTable().GetTypeTable()[i];
      ComputeTypeSizesAligns(*ty);
      LowerTypeAttribute(*ty);
  }

  if (mirModule.IsJavaModule()) {
    for (uint32 i = 0; i < GlobalTables::GetGsymTable().GetSymbolTableSize(); ++i) {
      MIRSymbol *sym = GlobalTables::GetGsymTable().GetSymbol(i);
      if (sym == nullptr) {
        continue;
      }
      LowerJavaVolatileForSymbol(*sym);
    }
  }
}

/*
 * try to find an available padding slot, and allocate the given field in it.
 * return the offset of the allocated memory. 0 if not available
 * Note: this will update lists in paddingSlots
 * Note: padding slots is a list of un-occupied (small size) slots
 *       available to allocate new fields. so far, just for 1, 2, 4 bytes
 *       types (map to array index 0, 1, 2)
 */
static uint32 TryAllocInPaddingSlots(std::list<uint32> paddingSlots[],
                                     uint32 fieldSize,
                                     uint32 fieldAlign,
                                     size_t paddingSlotsLength) {
  CHECK_FATAL(paddingSlotsLength > 0, "expect paddingSlotsLength > 0");
  if (fieldSize > 4) {
    return 0;  /* padding slots are for size 1/2/4 bytes */
  }

  uint32 fieldOffset = 0;
  /* here is a greedy search */
  for (size_t freeSlot = (fieldSize >> 1); freeSlot < paddingSlotsLength; ++freeSlot) {
    if (!paddingSlots[freeSlot].empty()) {
      uint32 paddingOffset = paddingSlots[freeSlot].front();
      if (IsAlignedTo(paddingOffset, fieldAlign)) {
        /* reuse one padding slot */
        paddingSlots[freeSlot].pop_front();
        fieldOffset = paddingOffset;
        /* check whether there're still space left in this slot */
        uint32 leftSize = (1u << freeSlot) - fieldSize;
        if (leftSize != 0) {
          uint32 leftOffset = paddingOffset + fieldSize;
          if (leftSize & 0x1) {  /* check whether the last bit is 1 */
            paddingSlots[0].push_front(leftOffset);
            leftOffset += 1;
          }
          if (leftSize & 0x2) {  /* check whether the penultimate bit is 1 */
            paddingSlots[1].push_front(leftOffset);
          }
        }
        break;
      }
    }
  }
  return fieldOffset;
}

static void AddPaddingSlot(std::list<uint32> paddingSlots[], uint32 offset, uint32 size, size_t paddingSlotsLength) {
  CHECK_FATAL(paddingSlotsLength > 0, "expect paddingSlotsLength > 0");
  /*
   * decompose the padding into 1/2/4 bytes slots.
   * to satisfy alignment constraints.
   */
  for (size_t i = 0; i < paddingSlotsLength; ++i) {
    if (size & (1u << i)) {
      paddingSlots[i].push_front(offset);
      offset += (1u << i);
    }
  }
}

void BECommon::ComputeStructTypeSizesAligns(MIRType &ty, const TyIdx &tyIdx, uint8 align) {
  auto &structType = static_cast<MIRStructType&>(ty);
  const FieldVector &fields = structType.GetFields();
  uint64 allocedSize = 0;
  uint64 allocedSizeInBits = 0;
  SetStructFieldCount(structType.GetTypeIndex(), fields.size());
  for (size_t j = 0; j < fields.size(); ++j) {
    TyIdx fieldTyIdx = fields[j].second.first;
    MIRType *fieldType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(fieldTyIdx);
    uint32 fieldTypeSize = GetTypeSize(fieldTyIdx);
    if (fieldTypeSize == 0) {
      ComputeTypeSizesAligns(*fieldType);
      fieldTypeSize = GetTypeSize(fieldTyIdx);
    }
    uint8 fieldAlign = GetTypeAlign(fieldTyIdx);
    CHECK_FATAL(fieldAlign != 0, "expect fieldAlign not equal 0");
    if ((fieldType->GetKind() == kTypeStruct) || (fieldType->GetKind() == kTypeClass)) {
      AppendStructFieldCount(structType.GetTypeIndex(), GetStructFieldCount(fieldTyIdx));
    }
    if (structType.GetKind() != kTypeUnion) {
      if (fieldType->GetKind() == kTypeBitField) {
        uint32 fieldSize = static_cast<MIRBitFieldType*>(fieldType)->GetFieldSize();
        /* is this field is crossing the align boundary of its base type? */
        if ((allocedSizeInBits / (fieldAlign * 8u)) != ((allocedSizeInBits + fieldSize - 1u) / (fieldAlign * 8u))) {
          /* the field is crossing the align boundary of its base type; */
          /* align alloced_size_in_bits to fieldAlign */
          allocedSizeInBits = RoundUp(allocedSizeInBits, fieldAlign * kBitsPerByte);
        }
        /* allocate the bitfield */
        allocedSizeInBits += fieldSize;
        allocedSize = std::max(allocedSize, RoundUp(allocedSizeInBits, fieldAlign * kBitsPerByte) /
                                            kBitsPerByte);
      } else {
        /* pad alloced_size according to the field alignment */
        allocedSize = RoundUp(allocedSize, fieldAlign);
        allocedSize += fieldTypeSize;
        allocedSizeInBits = allocedSize * kBitsPerByte;
      }
    } else {  /* for unions, bitfields are treated as non-bitfields */
      allocedSize = std::max(allocedSize, static_cast<uint64>(fieldTypeSize));
    }
    SetTypeAlign(tyIdx, std::max(GetTypeAlign(tyIdx), fieldAlign));
  }
  SetTypeSize(tyIdx, RoundUp(allocedSize, align));
}

void BECommon::ComputeClassTypeSizesAligns(MIRType &ty, const TyIdx &tyIdx, uint8 align) {
  uint64 allocedSize = 0;
  const FieldVector &fields = static_cast<MIRStructType&>(ty).GetFields();

  auto &classType = static_cast<MIRClassType&>(ty);
  TyIdx prntTyIdx = classType.GetParentTyIdx();
  /* process parent class */
  if (prntTyIdx != 0u) {
    MIRClassType *parentType =
        static_cast<MIRClassType*>(GlobalTables::GetTypeTable().GetTypeFromTyIdx(prntTyIdx));
    uint32 prntSize = GetTypeSize(prntTyIdx);
    if (prntSize == 0) {
      ComputeTypeSizesAligns(*parentType);
      prntSize = GetTypeSize(prntTyIdx);
    }
    uint8 prntAlign = GetTypeAlign(prntTyIdx);
    AppendStructFieldCount(tyIdx, GetStructFieldCount(prntTyIdx) + 1);
    /* pad alloced_size according to the field alignment */
    allocedSize = RoundUp(allocedSize, prntAlign);

    JClassLayout *layout = mirModule.GetMemPool()->New<JClassLayout>(mirModule.GetMPAllocator().Adapter());
    /* add parent's record to the front */
    layout->push_back(JClassFieldInfo(false, false, false, allocedSize));
    /* copy parent's layout plan into my plan */
    if (HasJClassLayout(*parentType)) {  /* parent may have incomplete type definition. */
      const JClassLayout &parentLayout = GetJClassLayout(*parentType);
      layout->insert(layout->end(), parentLayout.begin(), parentLayout.end());
      allocedSize += prntSize;
      SetTypeAlign(tyIdx, std::max(GetTypeAlign(tyIdx), prntAlign));
    } else {
      LogInfo::MapleLogger() << "Warning:try to layout class with incomplete type:" << parentType->GetName() << "\n";
    }
    jClassLayoutTable[&classType] = layout;
  } else {
    /* This is the root class, say, The Object */
    jClassLayoutTable[&classType] = mirModule.GetMemPool()->New<JClassLayout>(mirModule.GetMPAllocator().Adapter());
  }

  /*
   * a list of un-occupied (small size) slots available for insertion
   * so far, just for 1, 2, 4 bytes types (map to array index 0, 1, 2)
   */
  std::list<uint32> paddingSlots[3];
  /* process fields */
  AppendStructFieldCount(tyIdx, fields.size());
  for (uint32 j = 0; j < fields.size(); ++j) {
    TyIdx fieldTyIdx = fields[j].second.first;
    MIRType *fieldType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(fieldTyIdx);
    FieldAttrs fieldAttr = fields[j].second.second;
    uint32 fieldSize = GetTypeSize(fieldTyIdx);
    if (fieldSize == 0) {
      ComputeTypeSizesAligns(*fieldType);
      fieldSize = GetTypeSize(fieldTyIdx);
    }
    uint8 fieldAlign = GetTypeAlign(fieldTyIdx);

    if ((fieldType->GetKind() == kTypePointer) && (fieldType->GetPrimType() == PTY_a64)) {
      /* handle class reference field */
      fieldSize = AArch64RTSupport::kRefFieldSize;
      fieldAlign = AArch64RTSupport::kRefFieldAlign;
    }

    /* try to alloc the field in one of previously created padding slots */
    uint32 currentFieldOffset = TryAllocInPaddingSlots(paddingSlots, fieldSize, fieldAlign,
                                                       sizeof(paddingSlots) / sizeof(paddingSlots[0]));
    /* cannot reuse one padding slot. layout to current end */
    if (currentFieldOffset == 0) {
      /* pad alloced_size according to the field alignment */
      currentFieldOffset = RoundUp(allocedSize, fieldAlign);
      if (currentFieldOffset != allocedSize) {
        /* rounded up, create one padding-slot */
        uint32 paddingSize = currentFieldOffset - allocedSize;
        AddPaddingSlot(paddingSlots, allocedSize, paddingSize,
                       sizeof(paddingSlots) / sizeof(paddingSlots[0]));
        allocedSize = currentFieldOffset;
      }
      /* need new memory for this field */
      allocedSize += fieldSize;
    }
    AddElementToJClassLayout(classType, JClassFieldInfo(fieldType->GetKind() == kTypePointer,
                                                        fieldAttr.GetAttr(FLDATTR_rcunowned),
                                                        fieldAttr.GetAttr(FLDATTR_rcweak),
                                                        currentFieldOffset));
    SetTypeAlign(tyIdx, std::max(GetTypeAlign(tyIdx), fieldAlign));
  }
  SetTypeSize(tyIdx, RoundUp(allocedSize, align));
}

void BECommon::ComputeArrayTypeSizesAligns(MIRType &ty, const TyIdx &tyIdx) {
  MIRArrayType &arrayType = static_cast<MIRArrayType&>(ty);
  MIRType *elemType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(arrayType.GetElemTyIdx());
  uint32 elemSize = GetTypeSize(elemType->GetTypeIndex());
  if (elemSize == 0) {
    ComputeTypeSizesAligns(*elemType);
    elemSize = GetTypeSize(elemType->GetTypeIndex());
  }
  CHECK_FATAL(elemSize != 0, "elemSize should not equal 0");
  CHECK_FATAL(elemType->GetTypeIndex() != 0u, "elemType's idx should not equal 0");
  elemSize = std::max(elemSize, static_cast<uint32>(GetTypeAlign(elemType->GetTypeIndex())));
  /* compute total number of elements from the multipel dimensions */
  uint64 numElems = 1;
  for (int d = 0; d < arrayType.GetDim(); ++d) {
    numElems *= arrayType.GetSizeArrayItem(d);
  }
  SetTypeSize(tyIdx, elemSize * numElems);
  SetTypeAlign(tyIdx, GetTypeAlign(elemType->GetTypeIndex()));
}

void BECommon::ComputeFArrayOrJArrayTypeSizesAligns(MIRType &ty, const TyIdx &tyIdx) {
  MIRFarrayType &arrayType = static_cast<MIRFarrayType&>(ty);
  MIRType *elemType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(arrayType.GetElemTyIdx());
  uint32 elemSize = GetTypeSize(elemType->GetTypeIndex());
  if (elemSize == 0) {
    ComputeTypeSizesAligns(*elemType);
    elemSize = GetTypeSize(elemType->GetTypeIndex());
  }
  CHECK_FATAL(elemSize != 0, "elemSize should not equal 0");
  CHECK_FATAL(GetTypeAlign(elemType->GetTypeIndex()) != 0u, "GetTypeAlign return 0 is not expected");
  elemSize = std::max(elemSize, static_cast<uint32>(GetTypeAlign(elemType->GetTypeIndex())));
  SetTypeSize(tyIdx, 0);
  SetTypeAlign(tyIdx, GetTypeAlign(elemType->GetTypeIndex()));
}

/* Note: also do java class layout */
void BECommon::ComputeTypeSizesAligns(MIRType &ty, uint8 align) {
  TyIdx tyIdx = ty.GetTypeIndex();
  if ((structFieldCountTable.size() > tyIdx) && (GetStructFieldCount(tyIdx) != 0)) {
    return;  /* processed before */
  }

  if ((ty.GetPrimType() == PTY_ptr) || (ty.GetPrimType() == PTY_ref)) {
    ty.SetPrimType(LOWERED_PTR_TYPE);
  }

  switch (ty.GetKind()) {
    case kTypeScalar:
#ifdef TARGARM32
      SetTypeSize(tyIdx, GetPrimTypeSize(ty.GetPrimType()));
      SetTypeAlign(tyIdx, GetPrimTypeSize(ty.GetPrimType()) > k4ByteSize ? k4ByteSize
                                                                         : GetPrimTypeSize(ty.GetPrimType()));
      break;
#endif
    case kTypePointer:
    case kTypeBitField:
    case kTypeFunction:
      SetTypeSize(tyIdx, GetPrimTypeSize(ty.GetPrimType()));
      SetTypeAlign(tyIdx, GetTypeSize(tyIdx));
      break;
    case kTypeArray: {
      ComputeArrayTypeSizesAligns(ty, tyIdx);
      break;
    }
    case kTypeFArray:
    case kTypeJArray: {
      ComputeFArrayOrJArrayTypeSizesAligns(ty, tyIdx);
      break;
    }
    case kTypeUnion:
    case kTypeStruct: {
      ComputeStructTypeSizesAligns(ty, tyIdx, align);
      break;
    }
    case kTypeInterface: {  /* interface shouldn't have instance fields */
      SetTypeAlign(tyIdx, 0);
      SetTypeSize(tyIdx, 0);
      SetStructFieldCount(tyIdx, 0);
      break;
    }
    case kTypeClass: {  /* cannot have union or bitfields */
      ComputeClassTypeSizesAligns(ty, tyIdx, align);
      break;
    }
    case kTypeByName:
    case kTypeVoid:
    default:
      SetTypeSize(tyIdx, 0);
      break;
  }
  /* there may be passed-in align attribute declared with the symbol */
  SetTypeAlign(tyIdx, std::max(GetTypeAlign(tyIdx), align));
}

void BECommon::LowerTypeAttribute(MIRType &ty) {
  if (mirModule.IsJavaModule()) {
    LowerJavaTypeAttribute(ty);
  }
}

void BECommon::LowerJavaTypeAttribute(MIRType &ty) {
  /* we process volatile only for now */
  switch (ty.GetKind()) {
    case kTypeClass:  /* cannot have union or bitfields */
      LowerJavaVolatileInClassType(static_cast<MIRClassType&>(ty));
      break;

    default:
      break;
  }
}

void BECommon::LowerJavaVolatileInClassType(MIRClassType &ty) {
  for (auto &field : ty.GetFields()) {
    if (field.second.second.GetAttr(FLDATTR_volatile)) {
      field.second.second.SetAttr(FLDATTR_memory_order_acquire);
      field.second.second.SetAttr(FLDATTR_memory_order_release);
    } else {
      MIRType *fieldType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(field.second.first);
      if (fieldType->GetKind() == kTypeClass) {
        LowerJavaVolatileInClassType(static_cast<MIRClassType&>(*fieldType));
      }
    }
  }
}

bool BECommon::IsRefField(MIRStructType &structType, FieldID fieldID) {
  if (structType.GetKind() == kTypeClass) {
    CHECK_FATAL(HasJClassLayout(static_cast<MIRClassType&>(structType)), "Cannot found java class layout information");
    const JClassLayout &layout = GetJClassLayout(static_cast<MIRClassType&>(structType));
    if (layout.empty()) {
      ERR(kLncErr, "layout is null in BECommon::IsRefField");
      return false;
    }
    return layout[fieldID - 1].IsRef();
  }
  return false;
}

void BECommon::LowerJavaVolatileForSymbol(MIRSymbol &sym) {
  /* type attr is associated with symbol */
  if (sym.GetAttr(ATTR_volatile)) {
    sym.SetAttr(ATTR_memory_order_acquire);
    sym.SetAttr(ATTR_memory_order_release);
  }
}

void BECommon::GenFieldOffsetMap(const std::string &className) {
  MIRType *type = GlobalTables::GetTypeTable().GetOrCreateClassType(className, mirModule);
  CHECK_FATAL(type != nullptr, "unknown class, type should not be nullptr");
  MIRClassType *classType = static_cast<MIRClassType*>(type);
  for (FieldID i = 1; i <= GetStructFieldCount(classType->GetTypeIndex()); ++i) {
    FieldID fieldID = i;
    FieldPair fp = classType->TraverseToFieldRef(fieldID);
    GStrIdx strIdx = fp.first;
    if (strIdx == 0u) {
      continue;
    }

    const std::string &fieldName = GlobalTables::GetStrTable().GetStringFromStrIdx(strIdx);

    TyIdx fieldTyIdx = fp.second.first;
    uint32 fieldSize = GetTypeSize(fieldTyIdx);
    MIRType *fieldType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(fieldTyIdx);

    if ((fieldType->GetKind() == kTypePointer) && (fieldType->GetPrimType() == PTY_a64)) {
      /* handle class reference field */
      fieldSize = AArch64RTSupport::kRefFieldSize;
    }

    std::pair<int32, int32> p = GetFieldOffset(*classType, i);
    CHECK_FATAL(p.second == 0, "expect p.second equals 0");
    LogInfo::MapleLogger() << "CLASS_FIELD_OFFSET_MAP(" << className.c_str() << "," << fieldName.c_str() << ","
                           << p.first << "," << fieldSize << ")\n";
  }
}

void BECommon::GenFieldOffsetMap(MIRClassType &classType, FILE &outFile) {
  const std::string &className = classType.GetName();

  /*
   * We only enumerate fields defined in the current class.  There are cases
   * where a parent classes may define private fields that have the same name as
   * a field in the current class.This table is generated for the convenience of
   * C programmers.  If the C programmer wants to access parent class fields,
   * the programmer should access them as `Parent.field`.
   */
  FieldID myEnd = structFieldCountTable.at(classType.GetTypeIndex());
  FieldID myBegin = myEnd - static_cast<FieldID>(classType.GetFieldsSize()) + 1;

  for (FieldID i = myBegin; i <= myEnd; ++i) {
    FieldID fieldID = i;
    FieldPair fp = classType.TraverseToFieldRef(fieldID);
    GStrIdx strIdx = fp.first;
    if (strIdx == 0u) {
      continue;
    }
    FieldAttrs attrs = fp.second.second;
    if (attrs.GetAttr(FLDATTR_static)) {
      continue;
    }

    const std::string &fieldName = GlobalTables::GetStrTable().GetStringFromStrIdx(strIdx);

    TyIdx fieldTyIdx = fp.second.first;
    uint32 fieldSize = GetTypeSize(fieldTyIdx);
    MIRType *fieldType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(fieldTyIdx);

    if ((fieldType->GetKind() == kTypePointer) && (fieldType->GetPrimType() == PTY_a64)) {
      /* handle class reference field */
      fieldSize = AArch64RTSupport::kRefFieldSize;
    }

    std::pair<int32, int32> p = GetFieldOffset(classType, i);
    CHECK_FATAL(p.second == 0, "expect p.second equals 0");
    fprintf(&outFile, "__MRT_CLASS_FIELD(%s, %s, %d, %u)\n", className.c_str(), fieldName.c_str(), p.first, fieldSize);
  }
}

void BECommon::GenObjSize(MIRClassType &classType, FILE &outFile) {
  const std::string &className = classType.GetName();
  uint64_t objSize = GetTypeSize(classType.GetTypeIndex());
  if (objSize == 0) {
    return;
  }

  TyIdx parentTypeIdx = classType.GetParentTyIdx();
  MIRType *parentType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(parentTypeIdx);
  const char *parentName = nullptr;
  if (parentType != nullptr) {
    MIRClassType *parentClass = static_cast<MIRClassType*>(parentType);
    parentName = parentClass->GetName().c_str();
  } else {
    parentName = "THIS_IS_ROOT";
  }
  fprintf(&outFile, "__MRT_CLASS(%s, %" PRIu64 ", %s)\n", className.c_str(), objSize, parentName);
}

/*
 * compute the offset of the field given by fieldID within the structure type
 * structy; it returns the answer in the pair (byteoffset, bitoffset) such that
 * if it is a bitfield, byteoffset gives the offset of the container for
 * extracting the bitfield and bitoffset is with respect to the container
 */
std::pair<int32, int32> BECommon::GetFieldOffset(MIRStructType &structType, FieldID fieldID) {
  CHECK_FATAL(fieldID <= GetStructFieldCount(structType.GetTypeIndex()), "GetFieldOFfset: fieldID too large");
  uint64 allocedSize = 0;
  uint64 allocedSizeInBits = 0;
  FieldID curFieldID = 1;
  if (fieldID == 0) {
    return std::pair<int32, int32>(0, 0);
  }

  if (structType.GetKind() == kTypeClass) {
    CHECK_FATAL(HasJClassLayout(static_cast<MIRClassType&>(structType)), "Cannot found java class layout information");
    const JClassLayout &layout = GetJClassLayout(static_cast<MIRClassType&>(structType));
    CHECK_FATAL(static_cast<uint32>(fieldID) - 1 < layout.size(), "subscript out of range");
    return std::pair<int32, int32>(static_cast<int32>(layout[fieldID - 1].GetOffset()), 0);
  }

  /* process the struct fields */
  FieldVector fields = structType.GetFields();
  for (uint32 j = 0; j < fields.size(); ++j) {
    TyIdx fieldTyIdx = fields[j].second.first;
    MIRType *fieldType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(fieldTyIdx);
    uint32 fieldTypeSize = GetTypeSize(fieldTyIdx);
    uint8 fieldAlign = GetTypeAlign(fieldTyIdx);
    CHECK_FATAL(fieldAlign != 0, "fieldAlign should not equal 0");
    if (structType.GetKind() != kTypeUnion) {
      if (fieldType->GetKind() == kTypeBitField) {
        uint32 fieldSize = static_cast<MIRBitFieldType*>(fieldType)->GetFieldSize();
        /* is this field is crossing the align boundary of its base type? */
        if ((allocedSizeInBits / (fieldAlign * 8u)) != ((allocedSizeInBits + fieldSize - 1u) / (fieldAlign * 8u))) {
          /*
           * the field is crossing the align boundary of its base type;
           * align alloced_size_in_bits to fieldAlign
           */
          allocedSizeInBits = RoundUp(allocedSizeInBits, fieldAlign * kBitsPerByte);
        }
        /* allocate the bitfield */
        if (curFieldID == fieldID) {
          return std::pair<int32, int32>((allocedSizeInBits / (fieldAlign * 8u)) * fieldAlign,
                                         allocedSizeInBits % (fieldAlign * 8u));
        } else if (fieldType->GetKind() == kTypeStruct) {
          if ((curFieldID + GetStructFieldCount(fieldTyIdx)) >= fieldID) {
            MIRStructType *subStructType = static_cast<MIRStructType*>(fieldType);
            std::pair<int32, int32> result = GetFieldOffset(*subStructType, fieldID - curFieldID);
            return std::pair<int32, int32>(result.first + allocedSize, result.second);
          }
          curFieldID += GetStructFieldCount(fieldTyIdx) + 1;
        } else {
          ++curFieldID;
        }
        allocedSizeInBits += fieldSize;
        allocedSize = std::max(allocedSize, RoundUp(allocedSizeInBits, fieldAlign * kBitsPerByte) / kBitsPerByte);
      } else {
        allocedSize = RoundUp(allocedSize, fieldAlign);

        if (curFieldID == fieldID) {
          return std::pair<int32, int32>(allocedSize, 0);
        } else if (fieldType->GetKind() == kTypeStruct) {
          if ((curFieldID + GetStructFieldCount(fieldTyIdx)) >= fieldID) {
            MIRStructType *subStructType = static_cast<MIRStructType*>(fieldType);
            std::pair<int32, int32> result = GetFieldOffset(*subStructType, fieldID - curFieldID);
            return std::pair<int32, int32>(result.first + allocedSize, result.second);
          }
          curFieldID += GetStructFieldCount(fieldTyIdx) + 1;
        } else {
          ++curFieldID;
        }

        allocedSize += fieldTypeSize;
        allocedSizeInBits = allocedSize * kBitsPerByte;
      }
    } else {  /* for unions, bitfields are treated as non-bitfields */
      if (curFieldID == fieldID) {
        return std::pair<int32, int32>(0, 0);
      } else if (fieldType->GetKind() == kTypeStruct) {
        /* union cannot be kTypeClass */
        if ((curFieldID + GetStructFieldCount(fieldTyIdx)) >= fieldID) {
          return GetFieldOffset(static_cast<MIRStructType&>(*fieldType), fieldID - curFieldID);
        }
        curFieldID += GetStructFieldCount(fieldTyIdx) + 1;
      } else {
        ++curFieldID;
      }
    }
  }
  CHECK_FATAL(false, "GetFieldOffset() fails to find field");

  return std::pair<int32, int32>(0, 0);
}

bool BECommon::TyIsInSizeAlignTable(const MIRType &ty) const {
  if (typeSizeTable.size() != tableAlignTable.size()) {
    return false;
  }
  return ty.GetTypeIndex() < typeSizeTable.size();
}

void BECommon::AddAndComputeSizeAlign(MIRType &ty) {
  CHECK_FATAL(ty.GetTypeIndex() == typeSizeTable.size(), "make sure the ty idx is exactly the table size");
  tableAlignTable.push_back(0);
  typeSizeTable.push_back(0);
  ComputeTypeSizesAligns(ty);
}

void BECommon::AddElementToJClassLayout(MIRClassType &klass, JClassFieldInfo info) {
  JClassLayout &layout = *(jClassLayoutTable.at(&klass));
  layout.push_back(info);
}

MIRType *BECommon::BeGetOrCreatePointerType(const MIRType &pointedType) {
  MIRType *newType = GlobalTables::GetTypeTable().GetOrCreatePointerType(pointedType, PTY_a64);
  if (TyIsInSizeAlignTable(*newType)) {
    return newType;
  }
  AddAndComputeSizeAlign(*newType);
  return newType;
}

MIRType *BECommon::BeGetOrCreateFunctionType(TyIdx tyIdx, const std::vector<TyIdx> &vecTy,
                                             const std::vector<TypeAttrs> &vecAt) {
  MIRType *newType = GlobalTables::GetTypeTable().GetOrCreateFunctionType(mirModule, tyIdx, vecTy, vecAt);
  if (TyIsInSizeAlignTable(*newType)) {
    return newType;
  }
  AddAndComputeSizeAlign(*newType);
  return newType;
}

BaseNode *BECommon::GetAddressOfNode(const BaseNode &node) {
  switch (node.GetOpCode()) {
    case OP_dread: {
      const DreadNode &dNode = static_cast<const DreadNode&>(node);
      const StIdx &index = dNode.GetStIdx();
      return mirModule.GetMIRBuilder()->CreateAddrof(*mirModule.CurFunction()->GetLocalOrGlobalSymbol(index));
    }
    case OP_iread: {
      const IreadNode &iNode = static_cast<const IreadNode&>(node);
      if (iNode.GetFieldID() == 0) {
        return iNode.Opnd(0);
      }

      uint32 index = static_cast<MIRPtrType*>(GlobalTables::GetTypeTable().GetTypeTable().at(
          iNode.GetTyIdx()))->GetPointedTyIdx();
      MIRType *pointedType = GlobalTables::GetTypeTable().GetTypeTable().at(index);
      std::pair<int32, int32> byteBitOffset =
          GetFieldOffset(static_cast<MIRStructType&>(*pointedType), iNode.GetFieldID());
#if TARGAARCH64
      ASSERT(GetAddressPrimType() == PTY_a64, "incorrect address type, expect a PTY_a64");
#endif
      return mirModule.GetMIRBuilder()->CreateExprBinary(
          OP_add, *GlobalTables::GetTypeTable().GetPrimType(GetAddressPrimType()),
          static_cast<BaseNode*>(iNode.Opnd(0)),
          mirModule.GetMIRBuilder()->CreateIntConst(byteBitOffset.first, PTY_u32));
    }
    default:
      return nullptr;
  }
}
}  /* namespace maplebe */
