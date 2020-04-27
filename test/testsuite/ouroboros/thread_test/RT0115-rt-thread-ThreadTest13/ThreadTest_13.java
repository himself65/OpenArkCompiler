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
 * -@TestCaseID: ThreadTest_13
 *- @TestCaseName: Thread_ThreadTest_13.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: test run() of Thread and the runnable object.
 *- @Brief: see below
 * -#step1：定义私有静态类ThreadRun实现Runnable接口，定义ThreadRun()和run()。
 * -#step2: 通过new得到一个线程对象，通过getState()获取启动前状态。运行线程。通过getState()启动后状态。确认得到的状态正确。
 * -#step3：通过new得到一个runnable线程对象，通过getState()启动前状态。运行线程，通过getState()启动后状态。确认得到的状
 *          态正确。
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadTest_13.java
 *- @ExecuteClass: ThreadTest_13
 *- @ExecuteArgs:
 */

public class ThreadTest_13 {
    /**
     * Test for run(). Should do nothing.
     */
    public static void main(String[] args) throws Exception {
        Thread t = new Thread();  // Test run() for thread
        Thread.State tsBefore = t.getState();
        t.run();
        Thread.State tsAfter = t.getState();
        System.out.println("run() should do nothing --- " + tsBefore.equals(Thread.State.NEW));
        System.out.println("run() should do nothing --- " + tsBefore.equals(tsAfter));
        System.out.println("run() should do nothing --- " + tsBefore);

        Thread tt = new Thread(new ThreadRun("helloworld")); // Test run() for runnable object
        Thread.State ttsBefore = tt.getState();
        tt.run();
        Thread.State ttsAfter = tt.getState();
        System.out.println("run() should do nothing --- " + ttsBefore.equals(Thread.State.NEW));
        System.out.println("run() should do nothing --- " + ttsBefore.equals(ttsAfter));
    }

    private static class ThreadRun implements Runnable {
        private final String helloWorld;

        public ThreadRun(String str) {
            helloWorld = str;
        }

        public void run() {
            System.out.println(helloWorld);
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan run\(\)\s*should\s*do\s*nothing\s*\-\-\-\s*true\s*run\(\)\s*should\s*do\s*nothing\s*\-\-\-\s*true\s*run\(\)\s*should\s*do\s*nothing\s*\-\-\-\s*NEW\s*helloworld\s*run\(\)\s*should\s*do\s*nothing\s*\-\-\-\s*true\s*run\(\)\s*should\s*do\s*nothing\s*\-\-\-\s*true