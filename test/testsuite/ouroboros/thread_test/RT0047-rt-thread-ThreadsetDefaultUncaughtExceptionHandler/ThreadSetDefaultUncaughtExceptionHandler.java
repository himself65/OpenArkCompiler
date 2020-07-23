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
 * -@TestCaseID: ThreadSetDefaultUncaughtExceptionHandler
 *- @TestCaseName: Thread_ThreadSetDefaultUncaughtExceptionHandler.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: SetDefaultUncaughtExceptionHandler set the exception handler for Thread class, all thread will
 *                     have the same exception handler
 *- @Brief: see below
 * -#step1: Override method run() include a uncaught error.
 * -#step2: Create two ThreadSetDefaultUncaughtExceptionHandler instance.
 * -#step3: Start the threads.
 * -#step4: Check that two times java.lang.ArithmeticException were handled by
 *          Thread.setDefaultUncaughtExceptionHandler,rather than catch.
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadSetDefaultUncaughtExceptionHandler.java
 *- @ExecuteClass: ThreadSetDefaultUncaughtExceptionHandler
 *- @ExecuteArgs:
 */

public class ThreadSetDefaultUncaughtExceptionHandler extends Thread{
    static int i=0;
    static String p;
    public void run() {
        System.out.println(3/0);
    }
    public static void main(String[] args) {

        ThreadSetDefaultUncaughtExceptionHandler thread_obj1 = new ThreadSetDefaultUncaughtExceptionHandler();
        ThreadSetDefaultUncaughtExceptionHandler thread_obj2 = new ThreadSetDefaultUncaughtExceptionHandler();
        Thread.setDefaultUncaughtExceptionHandler(new UncaughtExceptionHandler() {
            public void uncaughtException(Thread t, Throwable e) {
                p = e.toString();
                if (p.indexOf("java.lang.ArithmeticException") != -1) {
                    i++;
                }
            }
        });
        thread_obj1.start();
        thread_obj2.start();
        try {
        thread_obj1.join();
        thread_obj2.join();
        } catch (InterruptedException e1) {
        }
        if (i == 2) {
            System.out.println(0);
            return;
        }
        System.out.println(2);
    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n