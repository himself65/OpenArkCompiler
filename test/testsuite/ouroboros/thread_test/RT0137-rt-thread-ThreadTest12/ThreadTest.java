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
 *- @Title/Destination: Test for setDaemon() and isDaemon(); child thread inherits from parent thread if it is daemon
 *                      or not.
 *- @Brief: see below
 * -#step1: 定义私有静态类ThreadRunningAnotherThread，继承于类Thread。定义构造方法，调用super()。定义run(),定义了Thread
 *          对象，currentThread对象，isDaemon对象。
 * -#step2: 通过new得到ThreadRunningAnotherThread的对象，设置对象的守护进程状态为true。启动线程对象，暂停线程。等待线程结束，
 *          调用childIsDaemon(),确认返回true。
 * -#step3: 通过new得到ThreadRunningAnotherThread的对象，设置对象的守护进程状态为false。启动线程对象，暂停线程。等待线程结束，
 *          调用childIsDaemon(),确认返回false。
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadTest.java
 *- @ExecuteClass: ThreadTest
 *- @ExecuteArgs:
 */

public class ThreadTest {
    public static void main(String[] args) throws Exception {
        /**
         * Verify that a thread created by a daemon thread is daemon
         */
        ThreadRunningAnotherThread t = new ThreadRunningAnotherThread();
        t.setDaemon(true);
        t.start();
        t.stop = true;
        try {
            t.join();
        } catch (InterruptedException e) {
            System.out.println("INTERRUPTED_MESSAGE --");
        }
        System.out.println("the child thread of a daemon thread is daemon --- " +
                t.childIsDaemon);
        /**
         * Verify that a thread created by a non-daemon thread is not daemon
         */
        ThreadRunningAnotherThread tt = new ThreadRunningAnotherThread();
        tt.setDaemon(false);
        tt.start();
        tt.stop = true;
        try {
            tt.join();
        } catch (InterruptedException e) {
            System.out.println("INTERRUPTED_MESSAGE --");
        }
        System.out.println("the child thread of a non-daemon thread is non-daemon --- " +
                tt.childIsDaemon);
    }

    // Test for setDaemon() and isDaemon()
    private static class ThreadRunningAnotherThread extends Thread {
        int field = 0;
        volatile boolean stop = false;
        boolean childIsDaemon = false;
        Thread curThread = null;

        public ThreadRunningAnotherThread() {
            super();
        }

        public void run() {
            Thread child = new Thread();
            curThread = Thread.currentThread();
            childIsDaemon = child.isDaemon();
            while (!stop) {
                field++;
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan the\s*child\s*thread\s*of\s*a\s*daemon\s*thread\s*is\s*daemon\s*\-\-\-\s*true\s*the\s*child\s*thread\s*of\s*a\s*non\-daemon\s*thread\s*is\s*non\-daemon\s*\-\-\-\s*false