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
 * -@TestCaseID: ThreadSetDefaultUncaughtExceptionHandlerTest
 *- @TestCaseName: Thread_ThreadSetDefaultUncaughtExceptionHandlerTest.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Test negative input for setDefaultUncaughtExceptionHandler().
 *- @Brief: see below
 * -#step1: 创建Thread对象实例。
 * -#step2：构造参数eh为实例的getUncaughtExceptionHandler()，调用
 *          setDefaultUncaughtExceptionHandler(Thread.UncaughtExceptionHandler eh)。
 * -#step3：调用getDefaultUncaughtExceptionHandler()确认返回不为null。
 * -#step4：构造参数eh为实例的null，调用setDefaultUncaughtExceptionHandler(Thread.UncaughtExceptionHandler eh)。
 * -#step3：调用getDefaultUncaughtExceptionHandler()确认返回为null。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadSetDefaultUncaughtExceptionHandlerTest.java
 *- @ExecuteClass: ThreadSetDefaultUncaughtExceptionHandlerTest
 *- @ExecuteArgs:
 */

public class ThreadSetDefaultUncaughtExceptionHandlerTest extends Thread {
    public static void main(String[] args) {
        ThreadSetDefaultUncaughtExceptionHandlerTest setEH = new ThreadSetDefaultUncaughtExceptionHandlerTest();
        Thread.setDefaultUncaughtExceptionHandler(setEH.getUncaughtExceptionHandler());
        if (getDefaultUncaughtExceptionHandler() == null) {
            System.out.println(1);
            return;
        }

        Thread.setDefaultUncaughtExceptionHandler(null);
        if (getDefaultUncaughtExceptionHandler() == null) {
            System.out.println(0);
            return;
        }
        System.out.println(2);
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n