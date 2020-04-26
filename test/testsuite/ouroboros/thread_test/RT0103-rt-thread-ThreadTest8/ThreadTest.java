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
 * -@TestCaseID: ThreadTest
 *- @TestCaseName: Thread_ThreadTest.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: check status of thread when thety are NEW, RUNNABLE, BLOCKED, WAITING, TIMED_WAITING, TERMINATED
 *- @Brief: see below
 * -#step1: 创建一个Object类的实例对象object；
 * -#step2: 分别以object为参数，创建ThreadRunning1类、ThreadRunning3类的实例对象threadRunning1、threadRunning3，创建一个
 *          ThreadRunning2类的实例对象threadRunning2，并且这三个类继承自Thread类；
 * -#step3: 打印线程threadRunning1的状态；
 * -#step4: 分别调用threadRunning1、threadRunning2的start()方法启动这两个线程；
 * -#step5: 打印线程threadRunning1的状态；
 * -#step6: 让当前线程休眠200ms；
 * -#step7: 调用threadRunning3的start()方法启动这个线程；
 * -#step8: 让当前线程休眠200ms；
 * -#step9: 打印线程threadRunning3的状态；
 * -#step10: 将ThreadRunning1类中的类变量threadRunning1.stopWork赋值为true；
 * -#step11: 让当前线程休眠100ms；
 * -#step12: 依次打印threadRunning3、threadRunning2、threadRunning1的线程状态；
 * -#step13: 将ThreadRunning3类中的类变量threadRunning3.stopWork赋值为true；
 * -#step14: 以object为对象锁，在同步代码块中对object调用notify()方法；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadTest.java
 *- @ExecuteClass: ThreadTest
 *- @ExecuteArgs:
 */

class ThreadRunning1 extends Thread {
    public volatile int i = 0;
    volatile boolean stopWork = false;
    Object mylock;

    ThreadRunning1(Object obj) {
        mylock = obj;
    }

    public void run() {
        synchronized (mylock) {
            while (!stopWork) {
                i++;
                i--;
            }
        }
    }
}

class ThreadRunning2 extends Thread {
    public volatile int i = 0;
    volatile boolean stopWork = false;

    public void run() {
        try {
            Thread.sleep(1000);
        } catch (Exception e) {
        }
    }
}

class ThreadRunning3 extends Thread {
    public volatile int i = 0;
    volatile boolean stopWork = false;
    Object mylock;

    ThreadRunning3(Object obj) {
        mylock = obj;
    }

    public void run() {
        synchronized (mylock) {
            try {
                mylock.wait();
                while (!stopWork) {
                    i++;
                    i--;
                }
            } catch (Exception e) {
            }
        }
    }
}

public class ThreadTest {
    public static void main(String[] args) throws Exception {
        Object object = new Object();
        ThreadRunning1 threadRunning1 = new ThreadRunning1(object);
        ThreadRunning2 threadRunning2 = new ThreadRunning2();
        ThreadRunning3 threadRunning3 = new ThreadRunning3(object);
        // t1 NEW; t2 NEW; t3 NEW
        System.out.println(threadRunning1.getState());
        threadRunning1.start();
        threadRunning2.start();
        // t1 RUNNABLE; t2 TIMED_WAITING; t3 NEW
        System.out.println(threadRunning1.getState());
        try {
            Thread.sleep(200);
        } catch (Exception e) {
        }
        threadRunning3.start();
        try {
            Thread.sleep(200);
        } catch (Exception e) {
        }
        // t1 RUNNABLE; t2 TIMED_WAITING; t3 BmymylockED
        System.out.println(threadRunning3.getState());
        threadRunning1.stopWork = true;
        try {
            Thread.sleep(100);
        } catch (Exception e) {
        }
        // t1 TERMINATED; t2 TIMED_WAITING; t3 WAITING
        System.out.println(threadRunning3.getState());
        System.out.println(threadRunning2.getState());
        System.out.println(threadRunning1.getState());
        threadRunning3.stopWork = true;
        synchronized (object) {
            object.notify();
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan NEW\s*RUNNABLE\s*BLOCKED\s*WAITING\s*TIMED_WAITING\s*TERMINATED