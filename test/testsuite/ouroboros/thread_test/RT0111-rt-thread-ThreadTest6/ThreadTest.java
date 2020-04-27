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
 * -@TestCaseID: ThreadTest
 *- @TestCaseName: Thread_ThreadTest.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Thread.getId() return identifier for each thread, the id is unique and remain same when thread
 *                      state changes
 *- @Brief: see below
 * -#step1: Create two thread.
 * -#step2: Change the thread state.
 * -#step3: Check that threads id doesn't changed.
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadTest.java
 *- @ExecuteClass: ThreadTest
 *- @ExecuteArgs:
 */

public class ThreadTest {
    public static void main(String[] args) throws Exception {
        Thread thread1 = new Thread();
        Thread thread2 = new Thread();
        long tId1 = thread1.getId();
        long tId2 = thread2.getId();
        System.out.println(tId1 != tId2);
        thread1.join();
        thread2.join();
        long tId3 = thread1.getId();
        long tId4 = thread2.getId();
        System.out.println(tId1 == tId3);
        System.out.println(tId2 == tId4);
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan true\s*true\s*true