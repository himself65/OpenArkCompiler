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
 * -@TestCaseID: ThreadGetUncaughtExceptionHandler1
 *- @TestCaseName: Thread_ThreadGetUncaughtExceptionHandler1.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Call setUncaughtExceptionHandler to for a child thread, can use getUncaughtExceptionHandler to
 *                      get exception handler program in main thread
 *- @Brief: see below
 * -#step1: 创建一个ThreadGetUncaughtExceptionHandler1类的实例对象threadGetUncaughtExceptionHandler1，且
 *          ThreadGetUncaughtExceptionHandler1类继承自Thread类；
 * -#step2: 通过threadGetUncaughtExceptionHandler1的setUncaughtExceptionHandler()方法设置其相关属性；
 * -#step3: 通过threadGetUncaughtExceptionHandler1的start()方法启动该线程；
 * -#step4: 让当前线程休眠10ms；
 * -#step5: 将threadGetUncaughtExceptionHandler1的getUncaughtExceptionHandler()方法的返回值赋值给string；
 * -#step6: 经判断得知string包含字符串"ThreadGetUncaughtExceptionHandler1"、"$"和"@"；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadGetUncaughtExceptionHandler1.java
 *- @ExecuteClass: ThreadGetUncaughtExceptionHandler1
 *- @ExecuteArgs:
 */

public class ThreadGetUncaughtExceptionHandler1 extends Thread{
    static String string;

    public void run() {
        try {
            sleep(50);
        } catch (InterruptedException e2) {
            System.out.println("Sleep is interrupted");
        }
        System.out.println(3/0);
    }

    public static void main(String[] args) {
        ThreadGetUncaughtExceptionHandler1 threadGetUncaughtExceptionHandler1 = new
                ThreadGetUncaughtExceptionHandler1();
        threadGetUncaughtExceptionHandler1.setUncaughtExceptionHandler(new UncaughtExceptionHandler() {
            public void uncaughtException(Thread t, Throwable e) {
            }
        });
        threadGetUncaughtExceptionHandler1.start();
        try {
            sleep(10);
        } catch (InterruptedException e1) {
            System.out.println("Sleep is interrupted");
        }
        string = threadGetUncaughtExceptionHandler1.getUncaughtExceptionHandler().toString();
        if (string.indexOf("ThreadGetUncaughtExceptionHandler1") != -1) {
            if (string.toString().indexOf("$") != -1) {
                if (string.toString().indexOf("@") != -1) {
                    System.out.println(0);
                }
            }
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0