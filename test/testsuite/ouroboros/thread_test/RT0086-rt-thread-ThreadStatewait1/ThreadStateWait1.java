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
 * -@TestCaseID: ThreadStateWait1
 *- @TestCaseName: Thread_ThreadStateWait1.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Thread state when the target thread is waiting for a second-level time
 *- @Brief: see below
 * -#step1: 创建两个ThreadStateWait1类的实例对象threadStateWait1、threadStateWait2，且ThreadStateWait1类继承自Thread类；
 * -#step2: 分别调用threadStateWait1、threadStateWait2的start()方法启动这两个线程；
 * -#step3: 让当前线程休眠100ms；
 * -#step4: 在ThreadStateWait1类内部的run()执行完之后，经判断得出，线程threadStateWait1、threadStateWait2的状态均是
 *          "TIMED_WAITING"，并且int类型的静态全局变量i的值由0变为2，t的值始终为0；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadStateWait1.java
 *- @ExecuteClass: ThreadStateWait1
 *- @ExecuteArgs:
 */

public class ThreadStateWait1 extends Thread {
    static Object object = "aa";
    static int i = 0;
    static int t = 0;

    public static void main(String[] args) {
        ThreadStateWait1 threadStateWait1 = new ThreadStateWait1();
        ThreadStateWait1 threadStateWait2 = new ThreadStateWait1();
        threadStateWait1.start();
        threadStateWait2.start();
        try {
            sleep(100);
        } catch (InterruptedException e1) {
        }
        if (threadStateWait1.getState().toString().equals("TIMED_WAITING")
                && threadStateWait2.getState().toString().equals("TIMED_WAITING") && i == 2 && t == 0) {
            System.out.println(0);
        }
    }

    public void run() {
        synchronized (object) {
            try {
                i++;
                object.wait(1000);
                t++;
            } catch (InterruptedException e) {
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n