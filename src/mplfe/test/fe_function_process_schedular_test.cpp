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
#include "mplfe_compiler_component.h"
#include <list>
#include <unistd.h>
#include <memory>

namespace maple {
class FEFunctionChild : public FEFunction {
 public:
  FEFunctionChild(std::list<uint32> &argResult, uint32 argIdx)
      : FEFunction(nullptr),
        result(argResult),
        idx(argIdx) {}
  virtual ~FEFunctionChild() = default;
  void Process() override {
    usleep(rand() % 1000);
  }
  void EmitToMIR() override {
    result.push_back(idx);
  }
  bool PreProcessTypeNameIdx() override {
    return true;
  }
  void GenerateGeneralStmt(bool &success) override {}
  void GenerateGeneralStmtFailCallBack() override {}
  void GenerateGeneralDebugInfo() override {}
  bool VerifyGeneral() override {
    return true;
  }
  void VerifyGeneralFailCallBack() override {}
  void EmitToFEIRStmt() override {}
  void GenerateArgVarList() override {}
  bool HasThis() override {
    return false;
  }

 private:
  std::list<uint32> &result;
  uint32 idx;
};  // class FEFunctionChild

TEST(FEFunctionProcessSchedular, RunSingle) {
  std::list<uint32> results;
  std::unique_ptr<FEFunction> function = std::make_unique<FEFunctionChild>(results, 0);
  std::unique_ptr<FEFunctionProcessTask> task = std::make_unique<FEFunctionProcessTask>(function.get());
  task->Run();
  task->Finish();
  EXPECT_EQ(results.size(), 1);
}

TEST(FEFunctionProcessSchedular, RunDirect) {
  std::list<std::unique_ptr<FEFunction>> functions;
  std::list<std::unique_ptr<FEFunctionProcessTask>> tasks;
  std::list<uint32> results;
  FEFunctionProcessSchedular schedular("test schedular");
  for (uint32 i = 0; i < 1000; i++) {
    functions.push_back(std::make_unique<FEFunctionChild>(results, i));
    tasks.push_back(std::make_unique<FEFunctionProcessTask>(functions.back().get()));
  }
  for (const std::unique_ptr<FEFunctionProcessTask> &task : tasks) {
    task->Run();
    task->Finish();
  }
  for (uint32 i = 0; i < 1000; i++) {
    EXPECT_EQ(results.front(), i);
    results.pop_front();
  }
}

TEST(FEFunctionProcessSchedular, RunSerial) {
  std::list<std::unique_ptr<FEFunction>> functions;
  std::list<std::unique_ptr<FEFunctionProcessTask>> tasks;
  std::list<uint32> results;
  FEFunctionProcessSchedular schedular("test schedular");
  for (uint32 i = 0; i < 1000; i++) {
    functions.push_back(std::make_unique<FEFunctionChild>(results, i));
    tasks.push_back(std::make_unique<FEFunctionProcessTask>(functions.back().get()));
  }
  for (const std::unique_ptr<FEFunctionProcessTask> &task : tasks) {
    schedular.AddTask(task.get());
  }
  schedular.RunTask(1, true);
  for (uint32 i = 0; i < 1000; i++) {
    EXPECT_EQ(results.front(), i);
    results.pop_front();
  }
}

TEST(FEFunctionProcessSchedular, RunParallel2) {
  std::list<std::unique_ptr<FEFunction>> functions;
  std::list<std::unique_ptr<FEFunctionProcessTask>> tasks;
  std::list<uint32> results;
  FEFunctionProcessSchedular schedular("test schedular");
  for (uint32 i = 0; i < 1000; i++) {
    functions.push_back(std::make_unique<FEFunctionChild>(results, i));
    tasks.push_back(std::make_unique<FEFunctionProcessTask>(functions.back().get()));
  }
  for (const std::unique_ptr<FEFunctionProcessTask> &task : tasks) {
    schedular.AddTask(task.get());
  }
  schedular.RunTask(2, true);
  for (uint32 i = 0; i < 1000; i++) {
    EXPECT_EQ(results.front(), i);
    results.pop_front();
  }
}

TEST(FEFunctionProcessSchedular, RunParallel4) {
  std::list<std::unique_ptr<FEFunction>> functions;
  std::list<std::unique_ptr<FEFunctionProcessTask>> tasks;
  std::list<uint32> results;
  FEFunctionProcessSchedular schedular("test schedular");
  for (uint32 i = 0; i < 1000; i++) {
    functions.push_back(std::make_unique<FEFunctionChild>(results, i));
    tasks.push_back(std::make_unique<FEFunctionProcessTask>(functions.back().get()));
  }
  for (const std::unique_ptr<FEFunctionProcessTask> &task : tasks) {
    schedular.AddTask(task.get());
  }
  schedular.RunTask(4, true);
  for (uint32 i = 0; i < 1000; i++) {
    EXPECT_EQ(results.front(), i);
    results.pop_front();
  }
}

TEST(FEFunctionProcessSchedular, RunParallel8) {
  std::list<std::unique_ptr<FEFunction>> functions;
  std::list<std::unique_ptr<FEFunctionProcessTask>> tasks;
  std::list<uint32> results;
  FEFunctionProcessSchedular schedular("test schedular");
  for (uint32 i = 0; i < 1000; i++) {
    functions.push_back(std::make_unique<FEFunctionChild>(results, i));
    tasks.push_back(std::make_unique<FEFunctionProcessTask>(functions.back().get()));
  }
  for (const std::unique_ptr<FEFunctionProcessTask> &task : tasks) {
    schedular.AddTask(task.get());
  }
  schedular.RunTask(8, true);
  for (uint32 i = 0; i < 1000; i++) {
    EXPECT_EQ(results.front(), i);
    results.pop_front();
  }
}

TEST(FEFunctionProcessSchedular, RunParallel16) {
  std::list<std::unique_ptr<FEFunction>> functions;
  std::list<std::unique_ptr<FEFunctionProcessTask>> tasks;
  std::list<uint32> results;
  FEFunctionProcessSchedular schedular("test schedular");
  for (uint32 i = 0; i < 1000; i++) {
    functions.push_back(std::make_unique<FEFunctionChild>(results, i));
    tasks.push_back(std::make_unique<FEFunctionProcessTask>(functions.back().get()));
  }
  for (const std::unique_ptr<FEFunctionProcessTask> &task : tasks) {
    schedular.AddTask(task.get());
  }
  schedular.RunTask(16, true);
  for (uint32 i = 0; i < 1000; i++) {
    EXPECT_EQ(results.front(), i);
    results.pop_front();
  }
}

TEST(FEFunctionProcessSchedular, RunParallel32) {
  std::list<std::unique_ptr<FEFunction>> functions;
  std::list<std::unique_ptr<FEFunctionProcessTask>> tasks;
  std::list<uint32> results;
  FEFunctionProcessSchedular schedular("test schedular");
  for (uint32 i = 0; i < 1000; i++) {
    functions.push_back(std::make_unique<FEFunctionChild>(results, i));
    tasks.push_back(std::make_unique<FEFunctionProcessTask>(functions.back().get()));
  }
  for (const std::unique_ptr<FEFunctionProcessTask> &task : tasks) {
    schedular.AddTask(task.get());
  }
  schedular.RunTask(32, true);
  for (uint32 i = 0; i < 1000; i++) {
    EXPECT_EQ(results.front(), i);
    results.pop_front();
  }
}

TEST(FEFunctionProcessSchedular, RunParallel48) {
  std::list<std::unique_ptr<FEFunction>> functions;
  std::list<std::unique_ptr<FEFunctionProcessTask>> tasks;
  std::list<uint32> results;
  FEFunctionProcessSchedular schedular("test schedular");
  for (uint32 i = 0; i < 1000; i++) {
    functions.push_back(std::make_unique<FEFunctionChild>(results, i));
    tasks.push_back(std::make_unique<FEFunctionProcessTask>(functions.back().get()));
  }
  for (const std::unique_ptr<FEFunctionProcessTask> &task : tasks) {
    schedular.AddTask(task.get());
  }
  schedular.RunTask(48, true);
  for (uint32 i = 0; i < 1000; i++) {
    EXPECT_EQ(results.front(), i);
    results.pop_front();
  }
}

TEST(FEFunctionProcessSchedular, RunParallel64) {
  std::list<std::unique_ptr<FEFunction>> functions;
  std::list<std::unique_ptr<FEFunctionProcessTask>> tasks;
  std::list<uint32> results;
  FEFunctionProcessSchedular schedular("test schedular");
  for (uint32 i = 0; i < 1000; i++) {
    functions.push_back(std::make_unique<FEFunctionChild>(results, i));
    tasks.push_back(std::make_unique<FEFunctionProcessTask>(functions.back().get()));
  }
  for (const std::unique_ptr<FEFunctionProcessTask> &task : tasks) {
    schedular.AddTask(task.get());
  }
  schedular.RunTask(64, true);
  for (uint32 i = 0; i < 1000; i++) {
    EXPECT_EQ(results.front(), i);
    results.pop_front();
  }
}

TEST(FEFunctionProcessSchedular, RunParallel128) {
  std::list<std::unique_ptr<FEFunction>> functions;
  std::list<std::unique_ptr<FEFunctionProcessTask>> tasks;
  std::list<uint32> results;
  FEFunctionProcessSchedular schedular("test schedular");
  schedular.Init();
  for (uint32 i = 0; i < 1000; i++) {
    functions.push_back(std::make_unique<FEFunctionChild>(results, i));
    tasks.push_back(std::make_unique<FEFunctionProcessTask>(functions.back().get()));
  }
  for (const std::unique_ptr<FEFunctionProcessTask> &task : tasks) {
    schedular.AddTask(task.get());
  }
  schedular.RunTask(128, true);
  for (uint32 i = 0; i < 1000; i++) {
    EXPECT_EQ(results.front(), i);
    results.pop_front();
  }
}
}  // namespace maple