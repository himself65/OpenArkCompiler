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
 * -@TestCaseID: ThreadTest_24
 *- @TestCaseName: Thread_ThreadTest_24.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Test for interrupt() -- Interrupt the current thread.
 *- @Brief: see below
 * -#step1：通过new得到一个新的线程对象，定义run()方法，在run方法里调用isInterrupted。
 * -#step2: 启动线程，通过sleep()指定休眠时间。
 * -#step3: 确认run里调用interrupt()返回true, main函数里调用interrupt()返回false。
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadTest_24.java
 *- @ExecuteClass: ThreadTest_24
 *- @ExecuteArgs:
 */

class ThreadTest_24 {
    /* Test for interrupt() -- Interrupt the current thread
     * If this thread is blocked in an invocation
     * of the wait(), wait(long), or wait(long, int) methods of the Object class, or
     * of the join(), join(long), join(long, int), sleep(long), or sleep(long, int), methods of this class,
     * then its interrupt status will be cleared and it will receive an InterruptedException.
     */
    public static void main(String[] args) {
        Thread t = new Thread() {
            public void run() {
                interrupt();
                System.out.println("in the new thread, isInterrupted() returns -- " + isInterrupted());
            }
        };

        t.start();
        try {
            Thread.sleep(3000);
        } catch (InterruptedException e) {
            System.out.println("unexpected InterruptedException while sleeping");
        }
        System.out.println("isInterrupted() finally returns -- " + t.isInterrupted());
        System.out.println("PASS");
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan in\s*the\s*new\s*thread\,\s*isInterrupted\(\)\s*returns\s*\-\-\s*true\s*isInterrupted\(\)\s*finally\s*returns\s*\-\-\s*false\s*PASS