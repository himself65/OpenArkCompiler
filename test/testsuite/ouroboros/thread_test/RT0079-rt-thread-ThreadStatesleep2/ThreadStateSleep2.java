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
 * -@TestCaseID: ThreadStateSleep2
 *- @TestCaseName: Thread_ThreadStateSleep2.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Main thread sleep time is lower than target threads
 *- @Brief: see below
 * -#step1: 创建两个ThreadStateSleep2类的实例对象ThreadStateSleep21、ThreadStateSleep22，并且ThreadStateSleep2类继承自
 *          Thread类；
 * -#step2: 分别调用ThreadStateSleep21、ThreadStateSleep22的start()方法启动这两个线程；
 * -#step3: 让当前线程休眠100ms；
 * -#step4: 在经过ThreadStateSleep2类的内部的run()方法执行后，线程threadStateSleep21的状态如果是阻塞的，那么线程
 *          threadStateSleep22的状态就是"TIMED_WAITING"的；同理，如果线程threadStateSleep21的状态是"TIMED_WAITING"的，
 *          那么线程threadStateSleep22的状态就是阻塞的，并且只能是两种情况中的一种,而b的值始终为0；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadStateSleep2.java
 *- @ExecuteClass: ThreadStateSleep2
 *- @ExecuteArgs:
 */

public class ThreadStateSleep2 extends Thread {
    static Object object = "aa";
    static int t = 0;
    static int b = 0;

    public static void main(String[] args) {
        ThreadStateSleep2 threadStateSleep21 = new ThreadStateSleep2();
        ThreadStateSleep2 threadStateSleep22 = new ThreadStateSleep2();
        threadStateSleep21.start();
        threadStateSleep22.start();
        try {
            sleep(100);
        } catch (InterruptedException e1) {
        }
        if (threadStateSleep21.getState().toString().equals("BLOCKED")
                && threadStateSleep22.getState().toString().equals("TIMED_WAITING") && t == 1 && b == 0) {
            System.out.println(0);
        }
        if (threadStateSleep22.getState().toString().equals("BLOCKED")
                && threadStateSleep21.getState().toString().equals("TIMED_WAITING") && t == 1 && b == 0) {
            System.out.println(0);
        }
    }

    public void run() {
        for (int i = 0; i < 1; i++) {
            synchronized (object) {
                try {
                    t++;
                    sleep(1000);
                    b++;
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n