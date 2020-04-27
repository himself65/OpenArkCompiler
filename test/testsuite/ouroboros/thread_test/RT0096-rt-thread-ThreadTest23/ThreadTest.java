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
 *- @Title/Destination: test for interrupt() -- Interrupt a running thread
 *- @Brief: see below
 * -#step1: 定义一个ThreadRunning类，并且该类继承自Thread类，其内run()方法的逻辑是当boolean类型的变量stopWork等于false
 *          时，对int类型的变量i的值进行加1；
 * -#step2: 创建一个ThreadRunning类的实例对象threadRunning；
 * -#step3: 调用threadRunning的start()方法启动该线程；
 * -#step4: 让当前线程休眠2000ms；
 * -#step5: 调用threadRunning的interrupt()方法使线程中断；
 * -#step6: 让当前线程休眠1000ms；
 * -#step7: 将类变量threadRunning.stopWork赋值为true，经判断得知，在执行完ThreadRunning类的内部的run()方法后程序可以正常
 *          结束；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadTest.java
 *- @ExecuteClass: ThreadTest
 *- @ExecuteArgs:
 */

class ThreadTest {
    public static void main(String[] args) {
        ThreadRunning threadRunning = new ThreadRunning();

        threadRunning.start();
        try {
            Thread.sleep(2000);
        } catch (InterruptedException e) {
            System.out.println("unexpected InterruptedException while sleeping");
        }
        threadRunning.interrupt();
        try {
            Thread.sleep(1000);
        } catch (InterruptedException e) {
            System.out.println("unexpected InterruptedException while sleeping");
        }
        System.out.println("isInterrupted() finally returns -- " + threadRunning.isInterrupted());
        threadRunning.stopWork = true;
        System.out.println("PASS");
    }

    //test for interrupt() -- Interrupt a running thread
    /* If this thread is blocked in an invocation
     * of the wait(), wait(long), or wait(long, int) methods of the Object class, or
     * of the join(), join(long), join(long, int), sleep(long), or sleep(long, int), methods of this class,
     * then its interrupt status will be cleared and it will receive an InterruptedException.
     */

    static class ThreadRunning extends Thread {
        public volatile int i = 0;
        volatile boolean stopWork = false;

        ThreadRunning() {
            super();
        }

        public void run() {
            while (!stopWork) {
                i++;
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan isInterrupted\(\)\s*finally\s*returns\s*\-\-\s*true\s*PASS