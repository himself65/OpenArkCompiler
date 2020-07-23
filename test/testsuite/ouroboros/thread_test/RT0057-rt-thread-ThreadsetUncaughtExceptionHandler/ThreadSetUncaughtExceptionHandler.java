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
 * -@TestCaseID: ThreadSetUncaughtExceptionHandler
 *- @TestCaseName: Thread_ThreadSetUncaughtExceptionHandler.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: SetUncaughtExceptionHandler set he exception handler for a child thread.
 *- @Brief: see below
 * -#step1: 定义继承Thread类的线程类ThreadSetDefaultUncaughtExceptionHandler1，含run()方法设置3/0的异常。
 * -#step2: 通过new得到两个ThreadGetDefaultUncaughtExceptionHandler1的对象，分别使用setDefaultUncaughtExceptionHandler()
 *          设置默认的异常处理程序。
 * -#step3: 启动线程对象，等待线程结束，确认异常处理程序被调用。
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadSetUncaughtExceptionHandler.java
 *- @ExecuteClass: ThreadSetUncaughtExceptionHandler
 *- @ExecuteArgs:
 */

public class ThreadSetUncaughtExceptionHandler extends Thread {
    static int i = 0;
    static String p;

    public void run() {
        System.out.println(3/0);
    }

    public static void main(String[] args) {
        ThreadSetUncaughtExceptionHandler cls = new ThreadSetUncaughtExceptionHandler();
        ThreadSetUncaughtExceptionHandler cls2 = new ThreadSetUncaughtExceptionHandler();
        cls.setUncaughtExceptionHandler(new UncaughtExceptionHandler() {
            public void uncaughtException(Thread t, Throwable e) {
                p = e.toString();
                if (p.indexOf("java.lang.ArithmeticException") != -1) {
                    i++;
                }
            }
        });
        cls2.setUncaughtExceptionHandler(new UncaughtExceptionHandler() {
            public void uncaughtException(Thread t, Throwable e) {
            }
        });
        cls.start();
        cls2.start();
        try {
            cls.join();
            cls2.join();
        } catch (InterruptedException e1) {
            System.out.println("Join is interrupted");
        }
        if (i == 1) {
            System.out.println(0);
        }
    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n