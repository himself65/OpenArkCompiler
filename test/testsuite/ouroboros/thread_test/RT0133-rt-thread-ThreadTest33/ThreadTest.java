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
 *- @Title/Destination: test yield() in thread. finally all thread finished running.
 *- @Brief: see below
 * -#step1: 创建两个ThreadYielding类的实例对象threadYielding1、threadYielding2，并且ThreadYielding类继承自Thread类；
 * -#step2: 分别调用threadYielding1、threadYielding2的start()方法启动这两个线程；
 * -#step3: 分别调用threadYielding1、threadYielding2的join()方法；
 * -#step4: 将类变量ThreadYielding.dim的值赋值给threadNum；
 * -#step5: 经判断得知，threadYielding1.count、threadYielding2.count的值与（threadNum / 2）的值相等；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadTest.java
 *- @ExecuteClass: ThreadTest
 *- @ExecuteArgs:
 */
public class ThreadTest {
    public static void main(String[] args) throws Exception {
        /**
         * Test for void yield()
         */
        ThreadYielding threadYielding1 = new ThreadYielding();
        ThreadYielding threadYielding2 = new ThreadYielding();
        threadYielding1.start();
        threadYielding2.start();
        try {
            threadYielding1.join();
            threadYielding2.join();
        } catch (InterruptedException e) {
            System.out.println("INTERRUPTED_MESSAGE");
        }
        int threadNum = ThreadYielding.dim;

        // We suppose that threads t1 and t2 alternate with each other.
        // The might be a case when some another thread (not t2) runs
        // while t1 is yelding. In this case the 'list' might start with 1s
        // and end with 2s and look like threads does not alternate.
        // We cannot treat this as failure nevertheless.
        // We just make sure that both threads have finished successfully.
        System.out.println("threads have finished successfully --- " +
                (threadYielding1.count == threadNum / 2) + " --- " + (threadYielding2.count == threadNum / 2));
        System.out.println("PASS");
    }

    private static class ThreadYielding extends Thread {
        public static final int dim = 200;
        public int count = 0;

        public synchronized void incCount() {
            count++;
        }

        public void run() {
            for (int i = 0; i < dim / 2; i++) {
                incCount();
                Thread.yield();
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full threads have finished successfully --- true --- true\nPASS\n