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
 * -@TestCaseID: ThreadGetDefaultUncaughtExceptionHandler1
 *- @TestCaseName: Thread_ThreadGetDefaultUncaughtExceptionHandler1.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Use setDefaultUncaughtExceptionHandler to set the default exception handling subroutine, call
 *                      getDefaultExceptionHandler() can get the handler.
 *- @Brief: see below
 * -#step1: 定义继承Thread类的线程类ThreadGetDefaultUncaughtExceptionHandler1，含run()方法。
 * -#step2: New一个ThreadGetDefaultUncaughtExceptionHandler1的对象，使用setDefaultUncaughtExceptionHandler()设置默认的
 *           异常处理程序。
 * -#step3：启动线程对象，可以获取处理程序，确认结果正确。
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadGetDefaultUncaughtExceptionHandler1.java
 *- @ExecuteClass: ThreadGetDefaultUncaughtExceptionHandler1
 *- @ExecuteArgs:
 */

public class ThreadGetDefaultUncaughtExceptionHandler1 extends Thread{
    static String p;
    public void run() {
        System.out.println(3/0);
    }
    public static void main(String[] args) {
        ThreadGetDefaultUncaughtExceptionHandler1 cls=new ThreadGetDefaultUncaughtExceptionHandler1();
        Thread.setDefaultUncaughtExceptionHandler(new UncaughtExceptionHandler() {
            public void uncaughtException(Thread t, Throwable e) {
            }
        });
        cls.start();
        try {
            cls.join();
        } catch (InterruptedException e1) {
        }
        p=Thread.getDefaultUncaughtExceptionHandler().toString();
        if (p.indexOf("ThreadGetDefaultUncaughtExceptionHandle") !=-1) {
            if (p.toString().indexOf("$") !=-1) {
                if (p.toString().indexOf("@") !=-1) {
                    System.out.println(0);
                }
            }
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n