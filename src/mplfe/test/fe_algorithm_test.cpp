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
#include <set>
#include <memory>
#include <iostream>
#include "fe_options.h"
#include "fe_algorithm.h"
#include "mpl_logging.h"

namespace maple {
class TestNode {
 public:
  TestNode(int argID, std::set<int> argItems) : id(argID) {
    for (int item : argItems) {
      items.insert(item);
    }
    mergeEnd = items.size() > 0;
  }
  ~TestNode() = default;

  bool Merge(const TestNode &src) {
    for (int item : src.GetItems()) {
      items.insert(item);
    }
    return true;
  }

  bool MergeEnd() const {
    return mergeEnd;
  }

  bool CheckExist(int item) const {
    return items.find(item) != items.end();
  }

  bool CheckExist(std::set<int> argItems) const {
    for (int item : argItems) {
      if (items.find(item) == items.end()) {
        return false;
      }
    }
    return true;
  }

  const std::set<int> &GetItems() const {
    return items;
  }

  int GetID() const {
    return id;
  }

 private:
  int id;
  bool mergeEnd;
  std::set<int> items;
};  // class TestNode

class CorrelativeMergeTest {
 public:
  CorrelativeMergeTest() = default;
  ~CorrelativeMergeTest() = default;
};

// Test1
TEST(CorrelativeMergeTest, Test1) {
  std::map<TestNode*, std::set<TestNode*>> correlation;
  std::unique_ptr<TestNode> node0 = std::make_unique<TestNode>(0, std::set<int>({ 0 }));
  std::unique_ptr<TestNode> node1 = std::make_unique<TestNode>(1, std::set<int>());
  correlation[node1.get()].insert(node0.get());
  CorrelativeMerge<TestNode> mergeHelper(correlation, &TestNode::Merge, &TestNode::MergeEnd);
  mergeHelper.ProcessAll();
  EXPECT_EQ(node1->CheckExist(0), true);
}

// Test2
// Graph: doc/images/ut_cases/CorrelativeMerge/Test2.dot
// Image: doc/images/ut_cases/CorrelativeMerge/Test2.png
TEST(CorrelativeMergeTest, Test2) {
  std::map<TestNode*, std::set<TestNode*>> correlation;
  std::unique_ptr<TestNode> node0 = std::make_unique<TestNode>(0, std::set<int>({ 0 }));
  std::unique_ptr<TestNode> node1 = std::make_unique<TestNode>(1, std::set<int>());
  std::unique_ptr<TestNode> node2 = std::make_unique<TestNode>(2, std::set<int>());
  correlation[node1.get()].insert(node0.get());
  correlation[node1.get()].insert(node2.get());
  correlation[node2.get()].insert(node1.get());
  CorrelativeMerge<TestNode> mergeHelper(correlation, &TestNode::Merge, &TestNode::MergeEnd);
  mergeHelper.ProcessAll();
  EXPECT_EQ(node1->CheckExist(0), true);
  EXPECT_EQ(node2->CheckExist(0), true);
  INFO(kLncInfo, "Visit Count = %d", mergeHelper.GetVisitCount());
}

// Test3
// Graph: doc/images/ut_cases/CorrelativeMerge/Test3.dot
// Image: doc/images/ut_cases/CorrelativeMerge/Test3.png
TEST(CorrelativeMergeTest, Test3) {
  std::map<TestNode*, std::set<TestNode*>> correlation;
  std::unique_ptr<TestNode> node0 = std::make_unique<TestNode>(0, std::set<int>({ 0 }));
  std::unique_ptr<TestNode> node1 = std::make_unique<TestNode>(1, std::set<int>());
  std::unique_ptr<TestNode> node2 = std::make_unique<TestNode>(2, std::set<int>({ 2 }));
  correlation[node1.get()].insert(node0.get());
  correlation[node1.get()].insert(node2.get());
  correlation[node2.get()].insert(node1.get());
  CorrelativeMerge<TestNode> mergeHelper(correlation, &TestNode::Merge, &TestNode::MergeEnd);
  mergeHelper.ProcessAll();
  EXPECT_EQ(node1->CheckExist(std::set<int>({ 0, 2 })), true);
  INFO(kLncInfo, "Visit Count = %d", mergeHelper.GetVisitCount());
}

// Test4
// Graph: doc/images/ut_cases/CorrelativeMerge/Test4.dot
// Image: doc/images/ut_cases/CorrelativeMerge/Test4.png
TEST(CorrelativeMergeTest, Test4) {
  std::map<TestNode*, std::set<TestNode*>> correlation;
  std::unique_ptr<TestNode> node0 = std::make_unique<TestNode>(0, std::set<int>({ 0 }));
  std::unique_ptr<TestNode> node1 = std::make_unique<TestNode>(1, std::set<int>());
  std::unique_ptr<TestNode> node2 = std::make_unique<TestNode>(2, std::set<int>());
  std::unique_ptr<TestNode> node3 = std::make_unique<TestNode>(3, std::set<int>());
  std::unique_ptr<TestNode> node4 = std::make_unique<TestNode>(4, std::set<int>());
  std::unique_ptr<TestNode> node5 = std::make_unique<TestNode>(5, std::set<int>());
  correlation[node1.get()].insert(node0.get());
  correlation[node2.get()].insert(node1.get());
  correlation[node2.get()].insert(node5.get());
  correlation[node3.get()].insert(node2.get());
  correlation[node4.get()].insert(node3.get());
  correlation[node5.get()].insert(node4.get());
  CorrelativeMerge<TestNode> mergeHelper(correlation, &TestNode::Merge, &TestNode::MergeEnd);
  mergeHelper.ProcessAll();
  EXPECT_EQ(node1->CheckExist(0), true);
  EXPECT_EQ(node2->CheckExist(0), true);
  EXPECT_EQ(node3->CheckExist(0), true);
  EXPECT_EQ(node4->CheckExist(0), true);
  EXPECT_EQ(node5->CheckExist(0), true);
  INFO(kLncInfo, "Visit Count = %d", mergeHelper.GetVisitCount());
}

// Test5
// Graph: doc/images/ut_cases/CorrelativeMerge/Test5.dot
// Image: doc/images/ut_cases/CorrelativeMerge/Test5.png
TEST(CorrelativeMergeTest, Test5) {
  std::map<TestNode*, std::set<TestNode*>> correlation;
  std::unique_ptr<TestNode> node0 = std::make_unique<TestNode>(0, std::set<int>({ 0 }));
  std::unique_ptr<TestNode> node1 = std::make_unique<TestNode>(1, std::set<int>());
  std::unique_ptr<TestNode> node2 = std::make_unique<TestNode>(2, std::set<int>());
  std::unique_ptr<TestNode> node3 = std::make_unique<TestNode>(3, std::set<int>());
  std::unique_ptr<TestNode> node4 = std::make_unique<TestNode>(4, std::set<int>({ 4 }));
  std::unique_ptr<TestNode> node5 = std::make_unique<TestNode>(5, std::set<int>());
  correlation[node1.get()].insert(node0.get());
  correlation[node2.get()].insert(node1.get());
  correlation[node2.get()].insert(node5.get());
  correlation[node3.get()].insert(node2.get());
  correlation[node4.get()].insert(node3.get());
  correlation[node5.get()].insert(node4.get());
  CorrelativeMerge<TestNode> mergeHelper(correlation, &TestNode::Merge, &TestNode::MergeEnd);
  mergeHelper.ProcessAll();
  EXPECT_EQ(node1->CheckExist(0), true);
  EXPECT_EQ(node2->CheckExist(std::set<int>({ 0, 4 })), true);
  EXPECT_EQ(node3->CheckExist(std::set<int>({ 0, 4 })), true);
  EXPECT_EQ(node5->CheckExist(4), true);
  INFO(kLncInfo, "Visit Count = %d", mergeHelper.GetVisitCount());
}

// Test6
// Graph: doc/images/ut_cases/CorrelativeMerge/Test6.dot
// Image: doc/images/ut_cases/CorrelativeMerge/Test6.png
TEST(CorrelativeMergeTest, Test6) {
  std::map<TestNode*, std::set<TestNode*>> correlation;
  std::unique_ptr<TestNode> node0 = std::make_unique<TestNode>(0, std::set<int>({ 0 }));
  std::unique_ptr<TestNode> node1 = std::make_unique<TestNode>(1, std::set<int>());
  std::unique_ptr<TestNode> node2 = std::make_unique<TestNode>(2, std::set<int>({ 2 }));
  std::unique_ptr<TestNode> node3 = std::make_unique<TestNode>(3, std::set<int>());
  std::unique_ptr<TestNode> node4 = std::make_unique<TestNode>(4, std::set<int>());
  correlation[node1.get()].insert(node0.get());
  correlation[node2.get()].insert(node0.get());
  correlation[node2.get()].insert(node4.get());
  correlation[node3.get()].insert(node1.get());
  correlation[node3.get()].insert(node2.get());
  correlation[node4.get()].insert(node3.get());
  CorrelativeMerge<TestNode> mergeHelper(correlation, &TestNode::Merge, &TestNode::MergeEnd);
  mergeHelper.ProcessAll();
  EXPECT_EQ(node1->CheckExist(0), true);
  EXPECT_EQ(node3->CheckExist(std::set<int>({ 0, 2 })), true);
  EXPECT_EQ(node4->CheckExist(std::set<int>({ 0, 2 })), true);
  INFO(kLncInfo, "Visit Count = %d", mergeHelper.GetVisitCount());
}

// Test7
// Graph: doc/images/ut_cases/CorrelativeMerge/Test7.dot
// Image: doc/images/ut_cases/CorrelativeMerge/Test7.png
TEST(CorrelativeMergeTest, Test7) {
  std::map<TestNode*, std::set<TestNode*>> correlation;
  std::unique_ptr<TestNode> node0 = std::make_unique<TestNode>(0, std::set<int>({ 0 }));
  std::unique_ptr<TestNode> node1 = std::make_unique<TestNode>(1, std::set<int>());
  std::unique_ptr<TestNode> node2 = std::make_unique<TestNode>(2, std::set<int>());
  std::unique_ptr<TestNode> node3 = std::make_unique<TestNode>(3, std::set<int>());
  std::unique_ptr<TestNode> node4 = std::make_unique<TestNode>(4, std::set<int>());
  std::unique_ptr<TestNode> node5 = std::make_unique<TestNode>(5, std::set<int>());
  correlation[node1.get()].insert(node0.get());
  correlation[node1.get()].insert(node5.get());
  correlation[node2.get()].insert(node1.get());
  correlation[node3.get()].insert(node2.get());
  correlation[node4.get()].insert(node2.get());
  correlation[node5.get()].insert(node3.get());
  correlation[node5.get()].insert(node4.get());
  CorrelativeMerge<TestNode> mergeHelper(correlation, &TestNode::Merge, &TestNode::MergeEnd);
  mergeHelper.LoopCheckAll();
  mergeHelper.ProcessOne(*node3, true);
  EXPECT_EQ(node3->CheckExist(std::set<int>({ 0 })), true);
  INFO(kLncInfo, "Visit Count = %d", mergeHelper.GetVisitCount());
}
}  // namespace MAPLE