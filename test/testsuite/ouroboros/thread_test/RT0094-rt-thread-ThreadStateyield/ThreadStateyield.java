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
 * -@TestCaseID: Ouroboros/rt_test/ThreadStateyield
 *- @TestCaseName: Thread_ThreadStateyield.java
 *- @RequirementName: Java Thread
 *- @Title/Destination:yield() give a hint to the scheduler that the current thread is willing to yield its current use of a processor. other threads will be executed before current thread.
 *- @Brief: see below
 * -#step1:分别以cls1、cls2为参数，创建ThreadStateYield类的两个实例对象threadStateYield1、threadStateYield2，并且ThreadStateYield类继承自Thread类；
 * -#step2:调用threadStateYield1的start()方法启动该线程；然后休眠10ms
 * -#step3:调用threadStateYield2的start()方法启动该线程；然后休眠1000ms；
 * -#step4:在ThreadStateYield类内部的run()方法执行完之后（run()方法内的逻辑是：如果当前线程名与"cls1"相同时，则执行yield()方法，并对int类型的静态全局变量i的值加1，并尝试让当前线程休眠10ms；如若不相同，则直接跳出循环），经判断得知，线程threadStateYield1的状态是"TIMED_WAITING"的，线程threadStateYield2的状态是"TERMINATED"的，并且i的值不为0；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadStateyield.java
 *- @ExecuteClass: ThreadStateyield
 *- @ExecuteArgs:
 */

import java.io.PrintStream;

public class ThreadStateyield extends Thread {
    static int i = 0;

    public ThreadStateyield(String name) {
        super(name);
    }

    public static void main(String[] args) {
        System.exit(run(args, System.out));
    }

    public static int run(String[] args, PrintStream out) {
        ThreadStateyield zqp1 = new ThreadStateyield("zqp1");
        ThreadStateyield zqp2 = new ThreadStateyield("zqp2");
        zqp1.start();
        try {
            sleep(10);
        } catch (InterruptedException e1) {
        }
        zqp2.start();
        try {
            sleep(1000);
        } catch (InterruptedException e1) {
        }
        System.out.println(zqp2.getState());
        System.out.println(zqp1.getState());
        if (zqp2.getState().toString().equals("TERMINATED") && zqp1.getState().toString().equals("TIMED_WAITING") && i != 0) {
            return 0;
        }
        return 2;
    }

    public void run() {
        for (; ; ) {
            if (currentThread().getName().equals("zqp1")) {
                yield();
                i++;
                try {
                    sleep(10);
                } catch (InterruptedException e) {
                }
            } else break;
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full TERMINATED\nTIMED_WAITING\n