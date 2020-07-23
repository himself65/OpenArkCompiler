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
 * -@TestCaseID: MemoryLeak
 *- @TestCaseName: Thread_MemoryLeak.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Test to see that memory leak no longer exists.
 *- @Brief: see below
 * -#step1: Create ThreadLocal instance.
 * -#step2: Set the value of the thread local variable for the current thread
 * -#step2: Check that memory leak no longer exists.
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: MemoryLeak.java
 *- @ExecuteClass: MemoryLeak
 *- @ExecuteArgs:
 */

public class MemoryLeak {
    public static void main(String[] args) {

        for (int i = 0; i < 1000; i++) {
            ThreadLocal t = new ThreadLocal();
            t.set(new Object());
            t.set(null);
        }
        System.out.println("Passed");
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full Passed\n