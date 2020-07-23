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
 *- @Title/Destination: Test for interrupt() -- Interrupt a waiting thread
 *- @Brief: see below
 * -#step1: Define test class which inherited from thread class.
 * -#step2: Create a ThreadWaiting instance.
 * -#step3: Start the thread.
 * -#step4: Block the thread with wait().
 * -#step5: Check that interrupt status was cleared by calling interrupt().
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadTest.java
 *- @ExecuteClass: ThreadTest
 *- @ExecuteArgs:
 */

class ThreadTest {
    public static void main(String[] args) {
        ThreadWaiting t = new ThreadWaiting(5000, 0);
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
        System.out.println("interrupt status has been cleared, if the output is false -- " +
                t.isInterrupted());
    }

    private static class ThreadWaiting extends Thread {
        private long millis;
        private int nanos;

        ThreadWaiting(long millis, int nanos) {
            this.millis = millis;
            this.nanos = nanos;
        }

        public void run() {
            synchronized (this) {
                try {
                    this.wait(millis, nanos);
                    System.out.println("Fail -- waiting thread has not received the InterruptedException");
                } catch (InterruptedException e) {
                    System.out.println("waiting thread has received the InterruptedException");
                }
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full waiting thread has received the InterruptedException\ninterrupt status has been cleared, if the output is false -- false\n