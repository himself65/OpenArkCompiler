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
 * -@TestCaseID: ThreadStateInterrupt2
 *- @TestCaseName: Thread_ThreadStateInterrupt2.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Threads that are not interrupted keep waiting till time up.
 *- @Brief: see below
 * -#step1: 定义继承Thread类的线程类ThreadStateInterrupt2。
 * -#step2: 定义线程安全的run()方法，线程未中断直到等待时间结束。
 * -#step3: 通过new关键字得到ThreadStateInterrupt2类的线程对象。
 * -#step4：启动线程对象，调用sleep()方法，线程未中断直到等待时间结束。
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadStateInterrupt2.java
 *- @ExecuteClass: ThreadStateInterrupt2
 *- @ExecuteArgs:
 */

public class ThreadStateInterrupt2 extends Thread {
    static int i = 0;
    static boolean j = false;

    public static void main(String[] args) {
        ThreadStateInterrupt2 cls = new ThreadStateInterrupt2();
        cls.start();
        try {
            sleep(50);
        } catch (InterruptedException e2) {
            System.err.println(e2);
        }
        if (!j && !cls.isInterrupted() && i == 0) {
            System.out.println(0);
        }
    }

    public synchronized void run() {
        try {
            while (!Thread.currentThread().isInterrupted()) {
                wait(2000);
                break;
            }
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
            j = Thread.currentThread().isInterrupted();
            i++;
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n