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
 * -@TestCaseID: NullStackTrace
 *- @TestCaseName: Thread_NullStackTrace.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Thread.getStackTrace() on runnable threads, should not return null
 *- @Brief: see below
 * -#step1: Create a thread instance.
 * -#step2: Start the thread.
 * -#step3: Check that Thread.getStackTrace() doesn't return null.
 * -#step4: Execute step 1~3 repeatedly 1000 times.
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: NullStackTrace.java
 *- @ExecuteClass: NullStackTrace
 *- @ExecuteArgs:
 */

public class NullStackTrace {
    static final int TIMES = 1000;

    public static void main(String[] args) {
        for (int i = 0; i < TIMES; i++) {
            Thread t = new Thread();
            t.start();

            StackTraceElement[] ste = t.getStackTrace();
            if (ste == null)
                throw new RuntimeException("Failed: Thread.getStackTrace should not return null");
        }
        System.out.println("Passed");
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan Passed