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
 *- @Title/Destination: Test for interrupt() -- Interrupt a joining thread.
 *- @Brief: see below
 * -#step1: 定义私有静态类ThreadJoining继承于Thread，创建ThreadJoining的构造方法。定义run()。
 * -#step2: 通过new得到ThreadJoining的对象，启动线程对象，通过sleep()指定线程休眠时间。
 * -#step3: 中断线程，等待3秒，确认抛出InterruptedException，调用isInterrupted()返回false。
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadTest.java
 *- @ExecuteClass: ThreadTest
 *- @ExecuteArgs:
 */

class ThreadTest {
    public static void main(String[] args) {
        ThreadJoining t = new ThreadJoining(10000, 0);
        t.start();
        try {
            Thread.sleep(2000);
        } catch (InterruptedException e) {
            System.out.println("main throws InterruptedException");
        }
        t.interrupt();
        try {
            Thread.sleep(3000);
        } catch (InterruptedException e) {
            System.out.println("main throws InterruptedException");
        }
        System.out.println("interrupt status has been cleared, if the output is false -- " + t.isInterrupted());
    }

    /* Test for interrupt() -- Interrupt a joining thread
     * If this thread is blocked in an invocation
     * of the wait(), wait(long), or wait(long, int) methods of the Object class, or
     * of the join(), join(long), join(long, int), sleep(long), or sleep(long, int), methods of this class,
     * then its interrupt status will be cleared and it will receive an InterruptedException.
     */
    private static class ThreadJoining extends Thread {
        private long millis;
        private int nanos;

        ThreadJoining(long millis, int nanos) {
            this.millis = millis;
            this.nanos = nanos;
        }

        public void run() {
            try {
                this.join(millis, nanos);
                System.out.println("Fail -- joining thread has not received the InterruptedException");
            } catch (InterruptedException e) {
                System.out.println("joining thread has received the InterruptedException");
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full joining thread has received the InterruptedException\ninterrupt status has been cleared, if the output is false -- false\n