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
 *- @Title/Destination: when there are only daemon thread, JVM exit.
 *- @Brief: see below
 * -#step1: 创建两个ThreadRunning1类的实例对象threadRunning1、threadRunning3，创建一个ThreadRunning2类的实例对象
 *          threadRunning2，并且ThreadRunning1类、ThreadRunning2类都继承自Thread类；
 * -#step2: 分别调用threadRunning1、threadRunning3的setDaemon()方法，设置其属性为true；
 * -#step3: 分别调用threadRunning1、threadRunning2、threadRunning3的start()方法启动这三个线程；
 * -#step4: 让当前线程休眠1000ms；
 * -#step5: 将类变量threadRunning2.stopWork赋值为true；
 * -#step6: 经判断可知，程序最终只打印了ThreadRunning2；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadTest.java
 *- @ExecuteClass: ThreadTest
 *- @ExecuteArgs:
 */
public class ThreadTest {
    public static void main(String[] args) throws Exception {
        //Try to make a running thread daemon, test SetDaemonLiveThread

        ThreadRunning1 threadRunning1 = new ThreadRunning1();
        ThreadRunning2 threadRunning2 = new ThreadRunning2();
        ThreadRunning1 threadRunning3 = new ThreadRunning1();
        threadRunning1.setDaemon(true);
        threadRunning3.setDaemon(true);
        threadRunning1.start();
        threadRunning2.start();
        threadRunning3.start();
        try {
            Thread.sleep(1000);
        } catch (Exception e) {
        }
        threadRunning2.stopWork = true;
    }

    static class ThreadRunning1 extends Thread {
        public volatile int i = 0;
        volatile boolean stopWork = false;

        ThreadRunning1() {
            super();
        }

        public void run() {
            while (!stopWork) {
                i++;
            }
            System.out.println("ThreadRunning1");
        }
    }

    static class ThreadRunning2 extends Thread {
        public volatile int i = 0;
        volatile boolean stopWork = false;

        ThreadRunning2() {
            super();
        }

        public void run() {
            while (!stopWork) {
                i++;
                i--;
            }
            System.out.println("ThreadRunning2");
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ThreadRunning2\n