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
#include "basic_io.h"
#include "base64.h"

namespace maple {
TEST(BasicIOMapFile, GenFileInMemory) {
  uint8 buf[] = { 0x00, 0x01, 0x02, 0x03 };
  std::unique_ptr<BasicIOMapFile> mapFile = BasicIOMapFile::GenFileInMemory("test", buf, sizeof(buf));
  EXPECT_NE(mapFile.get(), nullptr);
  EXPECT_EQ(mapFile->GetLength(), 4);
}

class BasicIOReadTest : public testing::Test {
 public:
  BasicIOReadTest() {
    testFile1 = BasicIOMapFile::GenFileInMemory("test", buf1, sizeof(buf1));
    testFile2 = BasicIOMapFile::GenFileInMemory("test", buf2, sizeof(buf2));
    testFile3 = BasicIOMapFile::GenFileInMemory("test", buf3, sizeof(buf3));
    testFile4 = BasicIOMapFile::GenFileInMemory("test", buf4, sizeof(buf4));
    testFile5 = BasicIOMapFile::GenFileInMemory("test", buf5, sizeof(buf5));
  }

  ~BasicIOReadTest() = default;

  BasicIOMapFile &GetTestFile1() {
    CHECK_NULL_FATAL(testFile1.get());
    return *testFile1;
  }

  BasicIOMapFile &GetTestFile2() {
    CHECK_NULL_FATAL(testFile2.get());
    return *testFile2;
  }

  BasicIOMapFile &GetTestFile3() {
    CHECK_NULL_FATAL(testFile3.get());
    return *testFile3;
  }

  BasicIOMapFile &GetTestFile4() {
    CHECK_NULL_FATAL(testFile4.get());
    return *testFile4;
  }

  BasicIOMapFile &GetTestFile5() {
    CHECK_NULL_FATAL(testFile5.get());
    return *testFile5;
  }

 private:
  std::unique_ptr<BasicIOMapFile> testFile1;
  std::unique_ptr<BasicIOMapFile> testFile2;
  std::unique_ptr<BasicIOMapFile> testFile3;
  std::unique_ptr<BasicIOMapFile> testFile4;
  std::unique_ptr<BasicIOMapFile> testFile5;
  uint8 buf1[16] = {0x01, 0xFF, 0x02, 0xFE, 0x03, 0xFD, 0x04, 0xFC, 0x05, 0xFB, 0x06, 0xFA, 0x07, 0xF8, 0x08, 0xF7};
  uint8 buf2[12] = {0x3F, 0x80, 0x00, 0x00, 0xBF, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  uint8 buf3[12] = {0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xBF};
  uint8 buf4[4] = {'A', 'B', 'C', 'D'};
  uint8 buf5[1] = {0x00};
};

TEST_F(BasicIOReadTest, ReadUInt8) {
  BasicIORead ioBigEndian(GetTestFile1(), true);
  BasicIORead ioLittleEndian(GetTestFile1(), false);
  bool success = false;
  EXPECT_EQ(ioBigEndian.ReadUInt8(), 0x01);
  EXPECT_EQ(ioLittleEndian.ReadUInt8(), 0x01);
  EXPECT_EQ(ioBigEndian.ReadUInt8(success), 0xFF);
  EXPECT_EQ(success, true);
  EXPECT_EQ(ioLittleEndian.ReadUInt8(success), 0xFF);
  EXPECT_EQ(success, true);
}

TEST_F(BasicIOReadTest, ReadInt8) {
  BasicIORead ioBigEndian(GetTestFile1(), true);
  BasicIORead ioLittleEndian(GetTestFile1(), false);
  bool success = false;
  EXPECT_EQ(ioBigEndian.ReadInt8(), static_cast<int8>(0x01));
  EXPECT_EQ(ioLittleEndian.ReadInt8(), static_cast<int8>(0x01));
  EXPECT_EQ(ioBigEndian.ReadInt8(success), static_cast<int8>(0xFF));
  EXPECT_EQ(success, true);
  EXPECT_EQ(ioLittleEndian.ReadInt8(success), static_cast<int8>(0xFF));
  EXPECT_EQ(success, true);
}

TEST_F(BasicIOReadTest, ReadUInt16) {
  BasicIORead ioBigEndian(GetTestFile1(), true);
  BasicIORead ioLittleEndian(GetTestFile1(), false);
  bool success = false;
  EXPECT_EQ(ioBigEndian.ReadUInt16(), 0x01FF);
  EXPECT_EQ(ioLittleEndian.ReadUInt16(), 0xFF01);
  EXPECT_EQ(ioBigEndian.ReadUInt16(success), 0x02FE);
  EXPECT_EQ(success, true);
  EXPECT_EQ(ioLittleEndian.ReadUInt16(success), 0xFE02);
  EXPECT_EQ(success, true);
}

TEST_F(BasicIOReadTest, ReadInt16) {
  BasicIORead ioBigEndian(GetTestFile1(), true);
  BasicIORead ioLittleEndian(GetTestFile1(), false);
  bool success = false;
  EXPECT_EQ(ioBigEndian.ReadInt16(), static_cast<int16>(0x01FF));
  EXPECT_EQ(ioLittleEndian.ReadInt16(), static_cast<int16>(0xFF01));
  EXPECT_EQ(ioBigEndian.ReadInt16(success), static_cast<int16>(0x02FE));
  EXPECT_EQ(success, true);
  EXPECT_EQ(ioLittleEndian.ReadInt16(success), static_cast<int16>(0xFE02));
  EXPECT_EQ(success, true);
}

TEST_F(BasicIOReadTest, ReadUInt32) {
  BasicIORead ioBigEndian(GetTestFile1(), true);
  BasicIORead ioLittleEndian(GetTestFile1(), false);
  bool success = false;
  EXPECT_EQ(ioBigEndian.ReadUInt32(), 0x01FF02FE);
  EXPECT_EQ(ioLittleEndian.ReadUInt32(), 0xFE02FF01);
  EXPECT_EQ(ioBigEndian.ReadUInt32(success), 0x03FD04FC);
  EXPECT_EQ(success, true);
  EXPECT_EQ(ioLittleEndian.ReadUInt32(success), 0xFC04FD03);
  EXPECT_EQ(success, true);
}

TEST_F(BasicIOReadTest, ReadInt32) {
  BasicIORead ioBigEndian(GetTestFile1(), true);
  BasicIORead ioLittleEndian(GetTestFile1(), false);
  bool success = false;
  EXPECT_EQ(ioBigEndian.ReadInt32(), static_cast<int32>(0x01FF02FE));
  EXPECT_EQ(ioLittleEndian.ReadInt32(), static_cast<int32>(0xFE02FF01));
  EXPECT_EQ(ioBigEndian.ReadInt32(success), static_cast<int32>(0x03FD04FC));
  EXPECT_EQ(success, true);
  EXPECT_EQ(ioLittleEndian.ReadInt32(success), static_cast<int32>(0xFC04FD03));
  EXPECT_EQ(success, true);
}

TEST_F(BasicIOReadTest, ReadUInt64) {
  BasicIORead ioBigEndian(GetTestFile1(), true);
  BasicIORead ioLittleEndian(GetTestFile1(), false);
  bool success = false;
  EXPECT_EQ(ioBigEndian.ReadUInt64(), 0x01FF02FE03FD04FC);
  EXPECT_EQ(ioLittleEndian.ReadUInt64(), 0xFC04FD03FE02FF01);
  EXPECT_EQ(ioBigEndian.ReadUInt64(success), 0x05FB06FA07F808F7);
  EXPECT_EQ(success, true);
  EXPECT_EQ(ioLittleEndian.ReadUInt64(success), 0xF708F807FA06FB05);
  EXPECT_EQ(success, true);
}

TEST_F(BasicIOReadTest, ReadInt64) {
  BasicIORead ioBigEndian(GetTestFile1(), true);
  BasicIORead ioLittleEndian(GetTestFile1(), false);
  bool success = false;
  EXPECT_EQ(ioBigEndian.ReadInt64(), static_cast<int64>(0x01FF02FE03FD04FC));
  EXPECT_EQ(ioLittleEndian.ReadInt64(), static_cast<int64>(0xFC04FD03FE02FF01));
  EXPECT_EQ(ioBigEndian.ReadInt64(success), static_cast<int64>(0x05FB06FA07F808F7));
  EXPECT_EQ(success, true);
  EXPECT_EQ(ioLittleEndian.ReadInt64(success), static_cast<int64>(0xF708F807FA06FB05));
  EXPECT_EQ(success, true);
}

TEST_F(BasicIOReadTest, ReadFloat) {
  BasicIORead ioBigEndian(GetTestFile2(), true);
  BasicIORead ioLittleEndian(GetTestFile3(), false);
  EXPECT_EQ(ioBigEndian.ReadFloat(), 1.0f);
  EXPECT_EQ(ioLittleEndian.ReadFloat(), 1.0f);
  EXPECT_EQ(ioBigEndian.ReadDouble(), -1.0);
  EXPECT_EQ(ioLittleEndian.ReadDouble(), -1.0);
}

TEST_F(BasicIOReadTest, ReadFloat2) {
  BasicIORead ioBigEndian(GetTestFile2(), true);
  BasicIORead ioLittleEndian(GetTestFile3(), false);
  bool success = false;
  EXPECT_EQ(ioBigEndian.ReadFloat(success), 1.0f);
  EXPECT_EQ(success, true);
  EXPECT_EQ(ioLittleEndian.ReadFloat(success), 1.0f);
  EXPECT_EQ(success, true);
  EXPECT_EQ(ioBigEndian.ReadDouble(success), -1.0);
  EXPECT_EQ(success, true);
  EXPECT_EQ(ioLittleEndian.ReadDouble(success), -1.0);
  EXPECT_EQ(success, true);
}

TEST_F(BasicIOReadTest, ReadBufferUInt8) {
  BasicIORead ioBigEndian(GetTestFile4(), true);
  BasicIORead ioLittleEndian(GetTestFile4(), false);
  bool success = false;
  uint8 buf[2];
  // big endian
  ioBigEndian.ReadBufferUInt8(buf, 2);
  EXPECT_EQ(buf[0], static_cast<uint8>('A'));
  EXPECT_EQ(buf[1], static_cast<uint8>('B'));
  ioBigEndian.ReadBufferUInt8(buf, 2, success);
  EXPECT_EQ(success, true);
  EXPECT_EQ(buf[0], static_cast<uint8>('C'));
  EXPECT_EQ(buf[1], static_cast<uint8>('D'));
  ioBigEndian.ReadBufferUInt8(buf, 2, success);
  EXPECT_EQ(success, false);
  // little endian
  ioLittleEndian.ReadBufferUInt8(buf, 2);
  EXPECT_EQ(buf[0], static_cast<uint8>('A'));
  EXPECT_EQ(buf[1], static_cast<uint8>('B'));
  ioLittleEndian.ReadBufferUInt8(buf, 2, success);
  EXPECT_EQ(success, true);
  EXPECT_EQ(buf[0], static_cast<uint8>('C'));
  EXPECT_EQ(buf[1], static_cast<uint8>('D'));
  ioLittleEndian.ReadBufferUInt8(buf, 2, success);
  EXPECT_EQ(success, false);
}

TEST_F(BasicIOReadTest, ReadBufferInt8) {
  BasicIORead ioBigEndian(GetTestFile4(), true);
  BasicIORead ioLittleEndian(GetTestFile4(), false);
  bool success = false;
  int8 buf[2];
  // big endian
  ioBigEndian.ReadBufferInt8(buf, 2);
  EXPECT_EQ(buf[0], static_cast<int8>('A'));
  EXPECT_EQ(buf[1], static_cast<int8>('B'));
  ioBigEndian.ReadBufferInt8(buf, 2, success);
  EXPECT_EQ(success, true);
  EXPECT_EQ(buf[0], static_cast<int8>('C'));
  EXPECT_EQ(buf[1], static_cast<int8>('D'));
  ioBigEndian.ReadBufferInt8(buf, 2, success);
  EXPECT_EQ(success, false);
  // little endian
  ioLittleEndian.ReadBufferInt8(buf, 2);
  EXPECT_EQ(buf[0], static_cast<int8>('A'));
  EXPECT_EQ(buf[1], static_cast<int8>('B'));
  ioLittleEndian.ReadBufferInt8(buf, 2, success);
  EXPECT_EQ(success, true);
  EXPECT_EQ(buf[0], static_cast<int8>('C'));
  EXPECT_EQ(buf[1], static_cast<int8>('D'));
  ioLittleEndian.ReadBufferInt8(buf, 2, success);
  EXPECT_EQ(success, false);
}

TEST_F(BasicIOReadTest, ReadBufferChar) {
  BasicIORead ioBigEndian(GetTestFile4(), true);
  BasicIORead ioLittleEndian(GetTestFile4(), false);
  bool success = false;
  char buf[2];
  // big endian
  ioBigEndian.ReadBufferChar(buf, 2);
  EXPECT_EQ(buf[0], 'A');
  EXPECT_EQ(buf[1], 'B');
  ioBigEndian.ReadBufferChar(buf, 2, success);
  EXPECT_EQ(success, true);
  EXPECT_EQ(buf[0], 'C');
  EXPECT_EQ(buf[1], 'D');
  ioBigEndian.ReadBufferChar(buf, 2, success);
  EXPECT_EQ(success, false);
  // little endian
  ioLittleEndian.ReadBufferChar(buf, 2);
  EXPECT_EQ(buf[0], 'A');
  EXPECT_EQ(buf[1], 'B');
  ioLittleEndian.ReadBufferChar(buf, 2, success);
  EXPECT_EQ(success, true);
  EXPECT_EQ(buf[0], 'C');
  EXPECT_EQ(buf[1], 'D');
  ioLittleEndian.ReadBufferChar(buf, 2, success);
  EXPECT_EQ(success, false);
}

TEST_F(BasicIOReadTest, ReadString) {
  BasicIORead ioBigEndian(GetTestFile4(), true);
  BasicIORead ioLittleEndian(GetTestFile4(), false);
  bool success = false;
  // big endian
  EXPECT_EQ(ioBigEndian.ReadString(2), "AB");
  EXPECT_EQ(ioBigEndian.ReadString(2, success), "CD");
  EXPECT_EQ(success, true);
  EXPECT_EQ(ioBigEndian.ReadString(2, success), "");
  EXPECT_EQ(success, false);
  // little endian
  EXPECT_EQ(ioLittleEndian.ReadString(2), "AB");
  EXPECT_EQ(ioLittleEndian.ReadString(2, success), "CD");
  EXPECT_EQ(success, true);
  EXPECT_EQ(ioLittleEndian.ReadString(2, success), "");
  EXPECT_EQ(success, false);
}

TEST_F(BasicIOReadTest, Read_BoundaryCheck) {
  BasicIORead ioBigEndian(GetTestFile5(), true);
  ioBigEndian.ReadUInt8();
  bool success = false;
  ioBigEndian.ReadUInt8(success);
  EXPECT_EQ(success, false);
  ioBigEndian.ReadUInt16(success);
  EXPECT_EQ(success, false);
  ioBigEndian.ReadUInt32(success);
  EXPECT_EQ(success, false);
  ioBigEndian.ReadUInt64(success);
  EXPECT_EQ(success, false);
  ioBigEndian.ReadFloat(success);
  EXPECT_EQ(success, false);
  ioBigEndian.ReadDouble(success);
  EXPECT_EQ(success, false);
}

}  // namespace maple