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
 *- @Title/Destination: Create a thread with Thread(ThreadGroup, Runnable, String, long), stack size is Long.MAX_VALUE.
 *- @Brief: see below
 * -#step1: 定义类Square实现Runnable接口，定义run()方法，通过sleep()指定线程休眠时间。
 * -#step2: 创建Square对象。
 * -#step3: 调用Thread(ThreadGroup, Runnable, String, long)创建Thread对象，启动线程对象。确认正常启动，run方法被调用，无异常
 *          抛出。
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadTest.java
 *- @ExecuteClass: ThreadTest
 *- @ExecuteArgs:
 */
public class ThreadTest {
    /**
     * Thread(ThreadGroup, Runnable, String, long) stack size is Long.MAX_VALUE.
     */
    public static void main(String[] args) throws Exception {
        // Does not work in maple, no exception occurs, run() is not executed.
        boolean expired = false;
        ThreadGroup tg = new ThreadGroup("newGroup");
        String name = "t1";
        Square s = new Square();
        System.out.println(Long.MAX_VALUE);
        try {
            Thread t = new Thread(tg, s, name, Long.MAX_VALUE);
            System.out.println("Creation_OK");
            t.start();
        } catch (OutOfMemoryError er) {
            System.out.println("OutOfMemoryError");
        }
        System.out.println("PASS");
    }

    static class Square implements Runnable {
        public void run() {
            System.out.println("Enter_OK");
            try {
                Thread.sleep(2000);
            } catch (InterruptedException e) {
                System.out.println("unexpected InterruptedException while sleeping");
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 9223372036854775807\s*Creation_OK\s*OutOfMemoryError\s*PASS