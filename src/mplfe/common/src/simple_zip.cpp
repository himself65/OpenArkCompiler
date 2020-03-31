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
#include "simple_zip.h"
#include <malloc.h>
#include <zlib.h>

namespace maple {
ZipLocalFileHeader::~ZipLocalFileHeader() {
  if (extraField != nullptr) {
    free(extraField);
    extraField = nullptr;
  }
}

std::unique_ptr<ZipLocalFileHeader> ZipLocalFileHeader::Parse(BasicIORead &io) {
  std::unique_ptr<ZipLocalFileHeader> header = std::make_unique<ZipLocalFileHeader>();
  header->signature = io.ReadUInt32();
  if (header->signature != kZipSigLocalFile) {
    return std::unique_ptr<ZipLocalFileHeader>(nullptr);
  }
  header->minVersion = io.ReadUInt16();
  header->gpFlag = io.ReadUInt16();
  header->compMethod = io.ReadUInt16();
  header->rawTime = io.ReadUInt16();
  header->rawDate = io.ReadUInt16();
  header->crc32 = io.ReadUInt32();
  header->compSize = io.ReadUInt32();
  header->unCompSize = io.ReadUInt32();
  header->lengthFileName = io.ReadUInt16();
  header->lengthExtraField = io.ReadUInt16();
  header->fileName = io.ReadString(header->lengthFileName);
  if (header->lengthExtraField > 0) {
    header->extraField = static_cast<uint8*>(malloc(header->lengthExtraField));
    CHECK_NULL_FATAL(header->extraField);
    io.ReadBufferUInt8(header->extraField, header->lengthExtraField);
  }
  return header;
}

std::unique_ptr<ZipDataDescriptor> ZipDataDescriptor::Parse(BasicIORead &io) {
  std::unique_ptr<ZipDataDescriptor> desc = std::make_unique<ZipDataDescriptor>();
  desc->signature = io.ReadUInt32();
  CHECK_FATAL(desc->signature == kZipSigDataDescriptor, "invalid zip file: wrong signature for data descriptor");
  desc->crc32 = io.ReadUInt32();
  desc->compSize = io.ReadUInt32();
  desc->unCompSize = io.ReadUInt32();
  return desc;
}

ZipLocalFile::~ZipLocalFile() {
  if (compData != nullptr) {
    free(compData);
    compData = nullptr;
  }
  if (unCompData != nullptr) {
    free(unCompData);
    unCompData = nullptr;
  }
}

std::unique_ptr<ZipLocalFile> ZipLocalFile::Parse(BasicIORead &io) {
  std::unique_ptr<ZipLocalFile> zf = std::make_unique<ZipLocalFile>();
  zf->header = ZipLocalFileHeader::Parse(io);
  if (zf->header == nullptr) {
    return std::unique_ptr<ZipLocalFile>(nullptr);
  }
  CHECK_FATAL((zf->header->GetGPFlag() & 0x1) == 0, "encrypted file is not supported");
  uint32 posDataStart = io.GetPos();
  uint32 posDataEnd = zf->GetDataEndPos(io);
  if (zf->isCompressed == false) {
    zf->ProcessUncompressedFile(io, posDataStart, posDataEnd);
  } else {
    zf->ProcessCompressedFile(io, posDataStart, posDataEnd);
  }
  return zf;
}

uint32 ZipLocalFile::GetDataEndPos(const BasicIORead &io) {
  const uint8 offsetSize = 4;
  uint32 posDataStart = io.GetPos();
  uint32 posDataEnd = posDataStart;
  const uint8 *buf = io.GetSafeBuffer(offsetSize);
  while (posDataEnd + offsetSize < io.GetFileLength()) {
    uint32 sig = BasicIOEndian::GetUInt32LittleEndian(buf);
    if (sig == kZipSigLocalFile) {
      break;
    }
    if (sig == kZipSigCentralDir) {
      break;
    }
    if (sig == kZipSigDataDescriptor) {
      isCompressed = true;
      break;
    }
    ++buf;
    ++posDataEnd;
  }
  CHECK_FATAL(posDataEnd + offsetSize < io.GetFileLength(), "invalid zip file: no data descriptor");
  return posDataEnd;
}

void ZipLocalFile::ProcessUncompressedFile(BasicIORead &io, uint32 start, uint32 end) {
  unCompDataSize = end - start;
  if (unCompDataSize > 0) {
    unCompData = static_cast<uint8*>(malloc(unCompDataSize));
    CHECK_NULL_FATAL(unCompData);
    io.ReadBufferUInt8(unCompData, unCompDataSize);
  }
}

void ZipLocalFile::ProcessCompressedFile(BasicIORead &io, uint32 start, uint32 end) {
  uint32 compDataLength = static_cast<uint32>(end - start);
  if (compDataLength == 0) {
    return;
  }
  compData = static_cast<uint8*>(malloc(compDataLength));
  CHECK_NULL_FATAL(compData);
  io.ReadBufferUInt8(compData, compDataLength);
  dataDesc = ZipDataDescriptor::Parse(io);
  CHECK_FATAL(compDataLength == dataDesc->GetCompSize(), "invalid zip file: wrong compsize");
  if (dataDesc->GetUnCompSize() > 0) {
    unCompData = static_cast<uint8*>(malloc(dataDesc->GetUnCompSize()));
    CHECK_NULL_FATAL(unCompData);
    z_stream zs;
    zs.zalloc = static_cast<alloc_func>(0);
    zs.zfree = static_cast<free_func>(0);
    int err = inflateInit2(&zs, -MAX_WBITS);
    CHECK_FATAL(err == 0, "inflateInit2 error");
    zs.next_in = compData;
    zs.avail_in = dataDesc->GetCompSize();
    zs.total_in = 0;
    zs.next_out = unCompData;
    zs.avail_out = dataDesc->GetUnCompSize();
    zs.total_out = 0;
    err = inflate(&zs, Z_NO_FLUSH);
    if (err == Z_STREAM_END) {
      err = inflateEnd(&zs);
    }
    unCompDataSize = dataDesc->GetUnCompSize();
    if (err != Z_OK) {
      free(unCompData);
      unCompData = nullptr;
      unCompDataSize = 0;
      CHECK_FATAL(false, "inflate failed");
    }
  }
}

SimpleZip::SimpleZip(BasicIOMapFile &file) : BasicIORead(file, false) {}

SimpleZip::~SimpleZip() {}

bool SimpleZip::ParseFile() {
  while (true) {
    std::unique_ptr<ZipLocalFile> zf = ZipLocalFile::Parse(*this);
    if (zf != nullptr) {
      files.push_back(std::move(zf));
    } else {
      break;
    }
  }
  return true;
}
}  // namespace maple
