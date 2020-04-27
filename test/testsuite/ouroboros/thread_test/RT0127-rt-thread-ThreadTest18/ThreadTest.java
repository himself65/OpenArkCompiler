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
 *- @Title/Destination: set a Thread to Daemon and run it.
 *- @Brief: see below
 * -#step1: 创建一个ThreadRunning类的实例对象threadRunning，并且ThreadRunning类继承自Thread类；
 * -#step2: 调用threadRunning的setDaemon()方法，设置其属性为true；
 * -#step3: 调用threadRunning的start()方法启动该线程；
 * -#step4: 让当前线程休眠1000ms；
 * -#step5: 经判断得知，threadRunning.isDaemon()与threadRunning.isAlive()的返回值均为true；
 * -#step6: 将类变量threadRunning.stopWork赋值为true；
 * -#step7: 让当前线程休眠1000ms；
 * -#step8: 在ThreadRunning类内部的run()方法执行完成后，经判断得知，threadRunning.isDaemon()的返回值为true，
 *          threadRunning.isAlive()的返回值均为false；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadTest.java
 *- @ExecuteClass: ThreadTest
 *- @ExecuteArgs:
 */

public class ThreadTest {
    public static void main(String[] args) throws Exception {
        //Try to make a running thread daemon, test SetDaemonLiveThread
        ThreadRunning threadRunning = new ThreadRunning();
        try {
            threadRunning.setDaemon(true);
            threadRunning.start();
        } catch (Exception e) {
            System.out.println("fail");
        }
        try {
            Thread.sleep(1000);
        } catch (Exception e) {
        }
        System.out.println(threadRunning.isDaemon());
        System.out.println(threadRunning.isAlive());
        threadRunning.stopWork = true;
        try {
            Thread.sleep(1000);
        } catch (Exception e) {
        }
        System.out.println(threadRunning.isDaemon());
        System.out.println(threadRunning.isAlive());
    }

    static class ThreadRunning extends Thread {
        public volatile int i = 0;
        volatile boolean stopWork = false;

        ThreadRunning() {
            super();
        }

        public void run() {
            while (!stopWork) {
                i++;
                i--;
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan true\s*true\s*true\s*false