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
 * -@TestCaseID: ThreadSetUncaughtExceptionHandlerTest
 *- @TestCaseName: Thread_ThreadSetUncaughtExceptionHandlerTest.java
 *- @RequirementName: Java Thread
 *- @Title/Destination:test negative input for setUncaughtExceptionHandler()
 *- @Brief: see below
 * -#step1: 创建Thread对象实例。
 * -#step2: 实例调用getUncaughtExceptionHandler()确认返回不为null。
 * -#step3：构造参数eh为实例的null，调用setUncaughtExceptionHandler(Thread.UncaughtExceptionHandler eh)。
 * -#step4: 启动线程，实例调用getUncaughtExceptionHandler()确认返回不为null。
 * -#step5：等待线程结束，实例调用getUncaughtExceptionHandler()确认返回不为null，run函数被成功调用。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadSetUncaughtExceptionHandlerTest.java
 *- @ExecuteClass: ThreadSetUncaughtExceptionHandlerTest
 *- @ExecuteArgs:
 */

public class ThreadSetUncaughtExceptionHandlerTest extends Thread {
    static int i = 0;

    public static void main(String[] args) {
        ThreadSetUncaughtExceptionHandlerTest setEH = new ThreadSetUncaughtExceptionHandlerTest();
        if (setEH.getUncaughtExceptionHandler() == null) {
            System.out.println(1);
            return;
        }
        setEH.setUncaughtExceptionHandler(null);
        setEH.start();
        if (setEH.getUncaughtExceptionHandler() == null) {
            System.out.println(2);
            return;
        }
        try {
            setEH.join();
        } catch (InterruptedException e1) {
            System.out.println("Join is interrupted");
        }

        if (i == 1 && setEH.getUncaughtExceptionHandler() != null) {
            System.out.println(0);
            return;
        }
        System.out.println(3);
    }

    public void run() {
        i++;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n