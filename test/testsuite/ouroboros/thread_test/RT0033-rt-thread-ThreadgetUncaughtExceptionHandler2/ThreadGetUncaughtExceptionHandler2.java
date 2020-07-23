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
 * -@TestCaseID: ThreadGetUncaughtExceptionHandler2
 *- @TestCaseName: Thread_ThreadGetUncaughtExceptionHandler2.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: When a thread does not have an uncaught exception explicitly, the thread's ThreadGroup is
 *                      returned when call getUncaughtExceptionHandler.
 *- @Brief: see below
 * -#step1: 创建一个ThreadGetUncaughtExceptionHandler2类的实例对象threadGetUncaughtExceptionHandler2，且
 *          ThreadGetUncaughtExceptionHandler2类继承自Thread类；
 * -#step2: 通过Thread类的setUncaughtExceptionHandler()方法设置其相关属性；
 * -#step3: 通过threadGetUncaughtExceptionHandler2的start()方法启动该线程；
 * -#step4: 将threadGetUncaughtExceptionHandler2的getUncaughtExceptionHandler()方法的返回值赋值给string；
 * -#step5: 经判断得知string中含有字符串"java.lang.ThreadGroup"；
 *- @Expect: expected.txt
 * -#step1xt
 *- @Priority: High
 *- @Source: ThreadGetUncaughtExceptionHandler2.java
 *- @ExecuteClass: ThreadGetUncaughtExceptionHandler2
 *- @ExecuteArgs:
 */

public class ThreadGetUncaughtExceptionHandler2 extends Thread{
    static String string;

    public void run() {
        System.out.println(3/0);
    }

    public static void main(String[] args) {
        ThreadGetUncaughtExceptionHandler2 threadGetUncaughtExceptionHandler2 = new
                ThreadGetUncaughtExceptionHandler2();
        Thread.setDefaultUncaughtExceptionHandler(new UncaughtExceptionHandler() {
            public void uncaughtException(Thread t, Throwable e) {
            }
        });
        threadGetUncaughtExceptionHandler2.start();
        string = threadGetUncaughtExceptionHandler2.getUncaughtExceptionHandler().toString();
        if (string.indexOf("java.lang.ThreadGroup") != -1) {
            System.out.println(0);
        } else {
            System.out.println(2);
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n