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
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "base64.h"
#include "jbc_class.h"

namespace maple {
namespace jbc {
class JBCClassTest : public testing::Test {
 public:
  JBCClassTest()
      : allocator(mp),
        fileJBC0001(nullptr),
        ioJBC0001(nullptr) {
    LoadFileJBC0001();
  }

  ~JBCClassTest() = default;

  void LoadFileJBC0001() {
    std::string base64Str =
        "yv66vgAAADQAKQoADQAiAxI0VngJAAwAIwRBgAAACQAMACQFAAAAEjRWeJAJ"
        "AAwAJQbAMAAAAAAAAAkADAAmBwAnBwAoAQACdjEBAAFJAQACdjIBAAFKAQAC"
        "djMBAAFGAQACdjQBAAFEAQAGPGluaXQ+AQADKClWAQAEQ29kZQEAD0xpbmVO"
        "dW1iZXJUYWJsZQEAEkxvY2FsVmFyaWFibGVUYWJsZQEABHRoaXMBAAZMVGVz"
        "dDsBAAVmdW5jMQEABWZ1bmMyAQAIPGNsaW5pdD4BAApTb3VyY2VGaWxlAQAJ"
        "VGVzdC5qYXZhDAAWABcMAA4ADwwAEgATDAAQABEMABQAFQEABFRlc3QBABBq"
        "YXZhL2xhbmcvT2JqZWN0ACEADAANAAAABAABAA4ADwAAAAkAEAARAAAAAQAS"
        "ABMAAAAJABQAFQAAAAQAAQAWABcAAQAYAAAAQwACAAEAAAARKrcAASoSArUA"
        "AyoSBLUABbEAAAACABkAAAAOAAMAAAABAAQAAgAKAAQAGgAAAAwAAQAAABEA"
        "GwAcAAAAAQAdABcAAQAYAAAAKwAAAAEAAAABsQAAAAIAGQAAAAYAAQAAAAgA"
        "GgAAAAwAAQAAAAEAGwAcAAAACQAeABcAAQAYAAAAGQAAAAAAAAABsQAAAAEA"
        "GQAAAAYAAQAAAAsACAAfABcAAQAYAAAAKQACAAAAAAANFAAGswAIFAAJswAL"
        "sQAAAAEAGQAAAAoAAgAAAAMABgAFAAEAIAAAAAIAIQ==";
    size_t length = 0;
    uint8 *ptr = Base64::Decode(base64Str, length);
    fileJBC0001 = BasicIOMapFile::GenFileInMemory("JBC0001", ptr, length);
    ioJBC0001 = mp->New<BasicIORead>(*fileJBC0001, true);
  }

  inline MapleAllocator &GetAllocator() {
    return allocator;
  }

 protected:
  static MemPool *mp;
  MapleAllocator allocator;
  std::unique_ptr<BasicIOMapFile> fileJBC0001;
  BasicIORead *ioJBC0001;

  static void SetUpTestCase() {
    mp = memPoolCtrler.NewMemPool("MemPool for JBCClassTest");
  }

  static void TearDownTestCase() {
    memPoolCtrler.DeleteMemPool(mp);
    mp = nullptr;
  }
};
MemPool *JBCClassTest::mp = nullptr;

TEST_F(JBCClassTest, ParseFileJBC0001) {
  JBCClass klass(allocator);
  bool success = klass.ParseFile(*ioJBC0001);
  EXPECT_EQ(success, true);
  // ConstPoolCount test
  uint16 constSize = klass.GetConstPoolCount();
  EXPECT_EQ(constSize, 41);
  // GetConstByIdx test
  const JBCConst *const0 = klass.GetConstPool().GetConstByIdx(0);
  EXPECT_EQ(const0, nullptr);
  const JBCConst *const9 = klass.GetConstPool().GetConstByIdx(9);
  EXPECT_NE(const9, nullptr);
  const JBCConst *const42 = klass.GetConstPool().GetConstByIdx(42);
  EXPECT_EQ(const42, nullptr);
  // GetConstByIdxWithTag test
  const JBCConst *const3 = klass.GetConstPool().GetConstByIdxWithTag(3, kConstFieldRef);
  EXPECT_NE(const3, nullptr);
  const JBCConst *const11 = klass.GetConstPool().GetConstByIdxWithTag(11, kConstUTF8);
  EXPECT_EQ(const11, nullptr);
  // GetConstValueByIdx test
  const JBCConst *const4 = klass.GetConstPool().GetConstValueByIdx(4);
  EXPECT_NE(const4, nullptr);
  const JBCConst *const6 = klass.GetConstPool().GetConstValueByIdx(6);
  EXPECT_NE(const6, nullptr);
  const JBCConst *const8 = klass.GetConstPool().GetConstValueByIdx(8);
  EXPECT_EQ(const8, nullptr);
  // GetConstValue4ByteByIdx test
  const4 = klass.GetConstPool().GetConstValue4ByteByIdx(4);
  EXPECT_NE(const4, nullptr);
  const6 = klass.GetConstPool().GetConstValue4ByteByIdx(6);
  EXPECT_EQ(const6, nullptr);
  // GetConstValue8ByteByIdx test
  const4 = klass.GetConstPool().GetConstValue8ByteByIdx(4);
  EXPECT_EQ(const4, nullptr);
  const6 = klass.GetConstPool().GetConstValue8ByteByIdx(6);
  EXPECT_NE(const6, nullptr);
  // Field Count
  EXPECT_EQ(klass.GetFieldCount(), 4);
  // Method Count
  EXPECT_EQ(klass.GetMethodCount(), 4);
  // Attr Count
  EXPECT_EQ(klass.GetAttrCount(), 1);
}
}  // namespace jbc
}  // namespace maple