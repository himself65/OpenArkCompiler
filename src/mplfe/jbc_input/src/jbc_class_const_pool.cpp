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
#include "jbc_class_const_pool.h"
#include <sstream>

namespace maple {
namespace jbc {
JBCConstPool::JBCConstPool(MapleAllocator &alloc)
    : allocator(alloc),
      pool(allocator.Adapter()),
      majorVersion(0) {
  pool.push_back(nullptr);
}

uint16 JBCConstPool::InsertConst(JBCConst *objConst) {
  CHECK_NULL_FATAL(objConst);
  size_t ans = pool.size();
  pool.push_back(objConst);
  CHECK_FATAL(ans < 0xFFFF, "const pool is full");
  return static_cast<uint16>(ans);
}

void JBCConstPool::InsertConstDummyForWide() {
  size_t ans = pool.size();
  pool.push_back(nullptr);
  CHECK_FATAL(ans < 0xFFFF, "const pool is full");
}

const JBCConst *JBCConstPool::GetConstByIdx(uint16 idx, bool safe) const {
  if (idx == 0 || idx >= pool.size()) {
    std::stringstream ss;
    ss << "invalid const pool idx " << static_cast<uint32>(idx) << ".";
    CHECK_FATAL(!safe, "%s", ss.str().c_str());
    ERR(kLncErr, "%s", ss.str().c_str());
    return nullptr;
  }
  return pool[idx];
}

const JBCConst *JBCConstPool::GetConstByIdxWithTag(uint16 idx, JBCConstTag tag, bool safe) const {
  const JBCConst *obj = GetConstByIdx(idx, safe);
  if (obj != nullptr && obj->GetTag() != tag) {
    std::stringstream ss;
    ss << "invalid const pool idx " << static_cast<uint32>(idx) << " (tag=" <<
          JBCConstTagName::GetTagName(obj->GetTag()).c_str() << ", expect " <<
          JBCConstTagName::GetTagName(tag).c_str() << ").";
    CHECK_FATAL(!safe, "%s Exit.", ss.str().c_str());
    ERR(kLncErr, "%s", ss.str().c_str());
    return nullptr;
  }
  return obj;
}

const JBCConst *JBCConstPool::GetConstValueByIdx(uint16 idx, bool safe) const {
  const JBCConst *obj = GetConstByIdx(idx, safe);
  if (obj != nullptr && obj->IsValue() == false) {
    std::stringstream ss;
    ss << "invalid const pool idx " << static_cast<uint32>(idx) << " (not value const).";
    CHECK_FATAL(!safe, "%s Exit.", ss.str().c_str());
    ERR(kLncErr, "%s", ss.str().c_str());
    return nullptr;
  }
  return obj;
}

const JBCConst *JBCConstPool::GetConstValue4ByteByIdx(uint16 idx, bool safe) const {
  const JBCConst *obj = GetConstByIdx(idx, safe);
  if (obj != nullptr && obj->IsValue4Byte() == false) {
    std::stringstream ss;
    ss << "invalid const pool idx " << static_cast<uint32>(idx) << " (not 4-byte value const).";
    CHECK_FATAL(!safe, "%s Exit.", ss.str().c_str());
    ERR(kLncErr, "%s", ss.str().c_str());
    return nullptr;
  }
  return obj;
}

const JBCConst *JBCConstPool::GetConstValue8ByteByIdx(uint16 idx, bool safe) const {
  const JBCConst *obj = GetConstByIdx(idx, safe);
  if (obj != nullptr && obj->IsValue8Byte() == false) {
    std::stringstream ss;
    ss << "invalid const pool idx " << static_cast<uint32>(idx) << " (not 8-byte value const).";
    CHECK_FATAL(!safe, "%s Exit.", ss.str().c_str());
    ERR(kLncErr, "%s", ss.str().c_str());
    return nullptr;
  }
  return obj;
}

std::string JBCConstPool::GetNameByClassInfoIdx(uint16 idx, bool safe) const {
  const JBCConstClass *constClass = static_cast<const JBCConstClass*>(GetConstByIdxWithTag(idx, kConstClass, safe));
  if (constClass == nullptr) {
    return "";
  } else {
    return constClass->GetClassNameOrin();
  }
}

bool JBCConstPool::PreProcess(uint16 argMajorVersion) {
  majorVersion = argMajorVersion;
  for (JBCConst *c : pool) {
    if (c != nullptr && !c->PreProcess(*this)) {
      return false;
    }
  }
  return true;
}

bool JBCConstPool::PrepareFEStructElemInfo() {
  bool success = true;
  for (JBCConst *c : pool) {
    if (c == nullptr) {
      continue;
    }
    JBCConstTag tag = c->GetTag();
    if (tag == JBCConstTag::kConstFieldRef || tag == JBCConstTag::kConstMethodRef ||
        tag == JBCConstTag::kConstInterfaceMethodRef) {
      JBCConstRef *constRef = static_cast<JBCConstRef*>(c);
      success = success && constRef->PrepareFEStructElemInfo();
    }
  }
  return success;
}

JBCConstUTF8 *JBCConstPool::NewConstUTF8(uint16 &idx, const std::string &str) {
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp != nullptr, "mempool is nullptr");
  JBCConstUTF8 *constItem = mp->New<JBCConstUTF8>(allocator, kConstUTF8, str);
  idx = InsertConst(constItem);
  return constItem;
}

JBCConst4Byte *JBCConstPool::NewConst4Byte(uint16 &idx, int32 value) {
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp != nullptr, "mempool is nullptr");
  JBCConst4Byte *constItem = mp->New<JBCConst4Byte>(allocator, kConstInteger, value);
  idx = InsertConst(constItem);
  return constItem;
}

JBCConst4Byte *JBCConstPool::NewConst4Byte(uint16 &idx, float value) {
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp != nullptr, "mempool is nullptr");
  JBCConst4Byte *constItem = mp->New<JBCConst4Byte>(allocator, kConstFloat, value);
  idx = InsertConst(constItem);
  return constItem;
}

JBCConst8Byte *JBCConstPool::NewConst8Byte(uint16 &idx, int64 value) {
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp != nullptr, "mempool is nullptr");
  JBCConst8Byte *constItem = mp->New<JBCConst8Byte>(allocator, kConstLong, value);
  idx = InsertConst(constItem);
  return constItem;
}

JBCConst8Byte *JBCConstPool::NewConst8Byte(uint16 &idx, double value) {
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp != nullptr, "mempool is nullptr");
  JBCConst8Byte *constItem = mp->New<JBCConst8Byte>(allocator, kConstDouble, value);
  idx = InsertConst(constItem);
  return constItem;
}

JBCConstClass *JBCConstPool::NewConstClass(uint16 &idx, const std::string &className) {
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp != nullptr, "mempool is nullptr");
  std::string classNameInternal = JBCConst::FullNameToInternalName(className);
  JBCConstUTF8 *constUTF8 = mp->New<JBCConstUTF8>(allocator, kConstUTF8, classNameInternal);
  uint16 idxTmp = InsertConst(constUTF8);
  CHECK_FATAL(idxTmp != UINT16_MAX, "constpool insert failed");
  JBCConstClass *constClass = mp->New<JBCConstClass>(allocator, kConstClass, idxTmp);
  CHECK_FATAL(constClass->PreProcess(*this), "New ConstClass failed");
  idx = InsertConst(constClass);
  return constClass;
}

JBCConstString *JBCConstPool::NewConstString(uint16 &idx, const std::string &str) {
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp != nullptr, "mempool is nullptr");
  JBCConstUTF8 *constUTF8 = mp->New<JBCConstUTF8>(allocator, kConstUTF8, str);
  uint16 idxTmp = InsertConst(constUTF8);
  CHECK_FATAL(idxTmp != UINT16_MAX, "constpool insert failed");
  JBCConstString *constString = mp->New<JBCConstString>(allocator, kConstString, idxTmp);
  CHECK_FATAL(constString->PreProcess(*this), "New ConstClass failed");
  idx = InsertConst(constString);
  return constString;
}

JBCConstRef *JBCConstPool::NewConstRef(uint16 &idx, JBCConstTag tag, const std::string &className,
                                       const std::string &name, const std::string &desc) {
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp != nullptr, "mempool is nullptr");
  uint16 idxConstClass;
  uint16 idxConstNameAndType;
  JBCConstClass *constClass = NewConstClass(idxConstClass, className);
  CHECK_FATAL(idxConstClass != UINT16_MAX, "constpool insert failed");
  CHECK_NULL_FATAL(constClass);
  JBCConstNameAndType *constNameAndType = NewConstNameAndType(idxConstNameAndType, name, desc);
  CHECK_FATAL(idxConstNameAndType != UINT16_MAX, "constpool insert failed");
  CHECK_NULL_FATAL(constNameAndType);
  JBCConstRef *constRef = mp->New<JBCConstRef>(allocator, tag, idxConstClass, idxConstNameAndType);
  CHECK_FATAL(constRef->PreProcess(*this), "New ConstClass failed");
  idx = InsertConst(constRef);
  return constRef;
}

JBCConstNameAndType *JBCConstPool::NewConstNameAndType(uint16 &idx, const std::string &name, const std::string &desc) {
  MemPool *mp = allocator.GetMemPool();
  ASSERT(mp != nullptr, "mempool is nullptr");
  uint16 idxConstName;
  uint16 idxConstDesc;
  JBCConstUTF8 *constName = NewConstUTF8(idxConstName, name);
  CHECK_FATAL(idxConstName != UINT16_MAX, "constpool insert failed");
  CHECK_NULL_FATAL(constName);
  JBCConstUTF8 *constDesc = NewConstUTF8(idxConstDesc, desc);
  CHECK_FATAL(idxConstDesc != UINT16_MAX, "constpool insert failed");
  CHECK_NULL_FATAL(constDesc);
  JBCConstNameAndType *constNameAndType = mp->New<JBCConstNameAndType>(allocator, kConstNameAndType, idxConstName,
                                                                       idxConstDesc);
  CHECK_FATAL(constNameAndType->PreProcess(*this), "New ConstClass failed");
  idx = InsertConst(constNameAndType);
  return constNameAndType;
}
}  // namespace jbc
}  // namespace maple
