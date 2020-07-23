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
 * -@TestCaseID: ThreadStateInterrupt1
 *- @TestCaseName: Thread_ThreadStateInterrupt1.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Interrupt a thread blocked in an invocation of the wait(), wait(long), or wait(long, int)
 *                      methods of the Object class, then its interrupt status will be cleared and it will receive an
 *                      InterruptedException.
 *- @Brief: see below
 * -#step1: 定义继承Thread类的线程类ThreadStateInterrupt1。
 * -#step2: 定义线程安全的run()方法，中断调用wait()的线程时会捕获通过InterruptedException异常。
 * -#step3: 通过new关键字得到Thread类的线程对象。
 * -#step4：启动线程对象，等待50毫秒后中断线程，抛出InterruptedException，线程后续操作未被执行。
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadStateInterrupt1.java
 *- @ExecuteClass: ThreadStateInterrupt1
 *- @ExecuteArgs:
 */

public class ThreadStateInterrupt1 extends Thread {
    static int i = 0;
    static boolean j;

    public static void main(String[] args) {
        ThreadStateInterrupt1 cls = new ThreadStateInterrupt1();
        cls.start();
        try {
            sleep(50);
        } catch (InterruptedException e2) {
            System.err.println(e2);
        }
        cls.interrupt();
        try {
            sleep(50);
        } catch (InterruptedException e3) {
            System.err.println(e3);
        }
        if (j && i == 0) {
            System.out.println(0);
        }
    }

    public synchronized void run() {
        try {
            while (!Thread.currentThread().isInterrupted()) {
                wait();
                i++;
            }
        } catch (InterruptedException e1) {
            Thread.currentThread().interrupt();
            j = Thread.currentThread().isInterrupted();
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n