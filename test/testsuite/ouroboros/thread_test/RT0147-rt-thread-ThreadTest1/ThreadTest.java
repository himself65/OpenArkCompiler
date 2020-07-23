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
 *- @Title/Destination: Interrupt a sleeping thread
 *- @Brief: see below
 * -#step1: Define a test class inherited from thread.
 * -#step2: Create a thread instance.
 * -#step3: Sleep the current thread.
 * -#step4: Interrupt the sleeping thread.
 * -#step5: Check that the sleeping thread was Interrupted.
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadTest.java
 *- @ExecuteClass: ThreadTest
 *- @ExecuteArgs:
 */

class MyThread extends Thread {
    public void run() {
        try {
            while (!isInterrupted()) {
                Thread.sleep(10000);
            }
        } catch (Exception e) {
            System.out.println("isInterrupted");
        }
    }
}

public class ThreadTest {
    public static void main(String[] args) {
        Thread t1 = new MyThread();
        t1.start();

        try {
            Thread.sleep(1000);
            t1.interrupt();
        } catch (Exception e) {
            System.out.println("catch");
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full isInterrupted\n