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
 * -@TestCaseID: ThreadGetStackTraceTest
 *- @TestCaseName: Thread_ThreadgetStackTrace.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Test getStackTrace().
 *- @Brief: see below
 * -#step1: 调用Thread.currentThread()获取Thread 实例。
 * -#step2：调用getStackTrace()获取对应的StackTraceElement数组。
 * -#step3：确认获取的StackTraceElement数组成员内容正确。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadgetStackTrace.java
 *- @ExecuteClass: ThreadgetStackTrace
 *- @ExecuteArgs:
 */


import java.io.PrintStream;

public class ThreadgetStackTrace extends Thread {

    public static void main(String[] args) {
        System.out.println(run(args, System.out));
    }

    public static int run(String[] args, PrintStream out) {
        StackTraceElement[] stackTraceElements = Thread.currentThread().getStackTrace();

        int l = stackTraceElements.length;
        if (stackTraceElements[l - 1].toString().contains("main") && stackTraceElements[l - 2].toString().contains("run") && stackTraceElements[l - 3].toString().contains("getStackTrace")) {
            return 0;
        }

        return 2;
    }
}

// DEPENDENCE: ThreadGetStackTraceTest.java_bak
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n