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
 * -@TestCaseID: ThreadRunnable
 *- @TestCaseName: Thread_ThreadRunnable.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Validation creates a thread by implementing the Runnable interface.
 *- @Brief: see below
 * -#step1: 创建一个ThreadRunnable类的实例对象threadRunnable，并且ThreadRunnable类实现了Runnable接口；
 * -#step2: 以threadRunnable为参数，通过new Thread()方法创建一个新线程，并调用start()方法启动该线程；
 * -#step3: 重复step2两次；
 * -#step4: 经判断得知step2和step3以及ThreadRunnable类在执行其内部的run()方法后均未抛出异常；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadRunnable.java
 *- @ExecuteClass: ThreadRunnable
 *- @ExecuteArgs:
 */

public class ThreadRunnable implements Runnable {
    private int ticket = 10;

    public static void main(String[] args) {
        ThreadRunnable threadRunnable = new ThreadRunnable();
        try {
            new Thread(threadRunnable).start();
            new Thread(threadRunnable).start();
            new Thread(threadRunnable).start();
        } catch (Exception e) {
            System.out.println(2);
            return;
        }
        System.out.println(0);
    }

    public synchronized void run() {
        for (int i = 0; i < 20; i++) {
            if (this.ticket > 0) {
                this.ticket--;
            } else {
                break;
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n