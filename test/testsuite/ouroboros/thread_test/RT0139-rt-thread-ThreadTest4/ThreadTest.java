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
 *- @Title/Destination: Test for static int enumerate(Thread[] tarray) .
 *- @Brief: see below
 * -#step1: 定义类ThreadRunning继承于Thread。定义构造方法，调用super()。定义run()。
 * -#step2: 通过new得到类ThreadRunning的多个线程对象并启动，调用enumerate(Thread[] tarray)复制线程列表。
 * -#step3: 遍历线程，暂停线程。返回线程总数为7。
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadTest.java
 *- @ExecuteClass: ThreadTest
 *- @ExecuteArgs:
 */
public class ThreadTest {
    // Enumerate() does not work in maple.
    public static void main(String[] args) throws Exception {
        ThreadRunning t1 = new ThreadRunning();
        t1.start();
        ThreadRunning t2 = new ThreadRunning();
        t2.start();
        ThreadRunning t11 = new ThreadRunning();
        t11.start();
        ThreadRunning t12 = new ThreadRunning();
        t12.start();
        ThreadRunning t121 = new ThreadRunning();
        t121.start();
        ThreadRunning t122 = new ThreadRunning();
        t122.start();
        // Estimate dimension as 6 created threads.
        // Plus 10 for some other threads.
        int estimateLength = 16;
        Thread[] list;
        int count;
        while (true) {
            list = new Thread[estimateLength];
            count = Thread.enumerate(list);
            if (count == estimateLength) {
                estimateLength *= 2;
            } else {
                //System.out.println(count);
                break;
            }
        }
        t1.stopWork = true;
        t2.stopWork = true;
        t11.stopWork = true;
        t12.stopWork = true;
        t121.stopWork = true;
        t122.stopWork = true;
        System.out.println("thread count : " + count);
    }

    // Test for enumerate()
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
// ASSERT: scan thread\s*count\s*\:\s*7