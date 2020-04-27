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
 * -@TestCaseID: ThreadGetDefaultUncaughtExceptionHandler2
 *- @TestCaseName: Thread_ThreadGetDefaultUncaughtExceptionHandler2.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Failed to get default exception handler after setting non-default exception capture subroutine
 *                      (setUncaughtExceptionHandler)
 *- @Brief: see below
 * -#step1: 定义继承Thread类的线程类ThreadGetDefaultUncaughtExceptionHandler1，含run()方法。
 * -#step2: New一个ThreadGetDefaultUncaughtExceptionHandler1的对象，使用setDefaultUncaughtExceptionHandler()设置默认的
 *           异常处理程序。
 * -#step3: 启动线程对象，不可以获取处理程序，NullPointerException抛出。
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadGetDefaultUncaughtExceptionHandler2.java
 *- @ExecuteClass: ThreadGetDefaultUncaughtExceptionHandler2
 *- @ExecuteArgs:
 */

public class ThreadGetDefaultUncaughtExceptionHandler2 extends Thread{
    public void run() {
        System.out.println(3/0);
    }
    public static void main(String[] args) {
        ThreadGetDefaultUncaughtExceptionHandler2 cls=new ThreadGetDefaultUncaughtExceptionHandler2();
        cls.setUncaughtExceptionHandler(new UncaughtExceptionHandler() {
            public void uncaughtException(Thread t, Throwable e) {
            }
        });
        cls.start();
        try {
            cls.join();
        } catch (InterruptedException e1) {
        }
        try {
            Thread.getDefaultUncaughtExceptionHandler().toString();
            System.out.println(2);
        } catch (NullPointerException e2) {
            System.out.println(0);
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0