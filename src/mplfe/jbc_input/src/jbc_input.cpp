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
#include "jbc_input.h"
#include "basic_io.h"
#include "simple_zip.h"
#include "fe_macros.h"
#include "fe_options.h"
#include "mplfe_env.h"

namespace maple {
namespace jbc {
namespace {
const std::string kClassFileSuffix = ".class";
const uint32 kClassFileSuffixLength = 6;
const std::string kJarMetaInf = "META-INF";
const uint32 kJarMetaInfLength = 8;
}
JBCInput::JBCInput(MIRModule &moduleIn)
    : module(moduleIn),
      mp(memPoolCtrler.NewMemPool("mempool for JBC Input Helper")),
      allocator(mp),
      klassList(allocator.Adapter()) {
  itKlass = klassList.end();
}

JBCInput::~JBCInput() {
  mp = nullptr;
}

void JBCInput::ReleaseMemPool() {
  memPoolCtrler.DeleteMemPool(mp);
  mp = nullptr;
}

bool JBCInput::ReadClassFile(const std::string &fileName) {
  (void)GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(fileName);
  BasicIOMapFile file(fileName);
  BasicIORead io(file, true);
  if (file.OpenAndMap() == false) {
    ERR(kLncErr, "Unable to open class file %s", fileName.c_str());
    return false;
  }
  JBCClass *klass = JBCClass::InClass(allocator, io);
  if (klass == nullptr) {
    ERR(kLncErr, "Unable to parse class file %s", fileName.c_str());
    file.Close();
    return false;
  }
  klass->SetFilePathName(fileName);
  RegisterSrcFileInfo(*klass);
  klassList.push_back(klass);
  file.Close();
  return true;
}

bool JBCInput::ReadClassFiles(const std::list<std::string> &fileNames) {
  bool success = true;
  for (const std::string &fileName : fileNames) {
    FE_INFO_LEVEL(FEOptions::kDumpLevelInfoDetail, "===== Process JBCInput::ReadClassFiles(%s) =====",
                  fileName.c_str());
    success = ReadClassFile(fileName) ? success : false;
  }
  return success;
}

bool JBCInput::ReadJarFile(const std::string &fileName) {
  (void)GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(fileName);
  BasicIOMapFile file(fileName);
  bool success = true;
  if (file.OpenAndMap() == false) {
    ERR(kLncErr, "Unable to open jar file %s", fileName.c_str());
    return false;
  }
  SimpleZip zipFile(file);
  if (zipFile.ParseFile() == false) {
    ERR(kLncErr, "Unable to unzip jar file %s", fileName.c_str());
    file.Close();
    return false;
  }
  for (const std::unique_ptr<ZipLocalFile> &zipLocalFile : zipFile.GetFiles()) {
    std::string zipLocalFileName = zipLocalFile->GetFileName();
    size_t len = zipLocalFileName.length();
    if (len > kClassFileSuffixLength &&
        zipLocalFileName.substr(len - kClassFileSuffixLength).compare(kClassFileSuffix) == 0) {
      if (zipLocalFileName.length() >= kJarMetaInfLength &&
          zipLocalFileName.substr(0, kJarMetaInfLength).compare(kJarMetaInf) == 0) {
        continue;
      }
      // class file
      BasicIOMapFile classFile(zipLocalFileName, zipLocalFile->GetUnCompData(), zipLocalFile->GetUnCompDataSize());
      BasicIORead ioClassFile(classFile, true);
      JBCClass *klass = JBCClass::InClass(allocator, ioClassFile);
      if (klass == nullptr) {
        ERR(kLncErr, "Unable to parse class file %s", zipLocalFileName.c_str());
        success = false;
      } else {
        klass->SetFilePathName(zipLocalFileName);
        RegisterSrcFileInfo(*klass);
        klassList.push_back(klass);
      }
    }
  }
  file.Close();
  if (!success) {
    return false;
  }
  return true;
}

bool JBCInput::ReadJarFiles(const std::list<std::string> &fileNames) {
  bool success = true;
  for (const std::string &fileName : fileNames) {
    FE_INFO_LEVEL(FEOptions::kDumpLevelInfoDetail, "===== Process JBCInput::ReadJarFiles(%s) =====", fileName.c_str());
    success = ReadJarFile(fileName) ? success : false;
  }
  return success;
}

const JBCClass *JBCInput::GetFirstClass() {
  if (klassList.size() == 0) {
    return nullptr;
  }
  itKlass = klassList.begin();
  return *itKlass;
}

const JBCClass *JBCInput::GetNextClass() {
  if (itKlass == klassList.end()) {
    return nullptr;
  }
  ++itKlass;
  if (itKlass == klassList.end()) {
    return nullptr;
  }
  return *itKlass;
}

void JBCInput::RegisterSrcFileInfo(JBCClass &klass) {
  GStrIdx fileNameIdx;
  if (FEOptions::GetInstance().IsJBCInfoUsePathName()) {
    fileNameIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(klass.GetFilePathName());
  } else {
    fileNameIdx = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName(klass.GetFileName());
  }
  uint32 srcFileIdx = MPLFEEnv::GetInstance().NewSrcFileIdx(fileNameIdx);
  module.PushbackFileInfo(MIRInfoPair(fileNameIdx, srcFileIdx));
  klass.SetSrcFileInfoIdx(srcFileIdx);
}
}  // namespace jbc
}  // namespace maple
