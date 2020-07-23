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
 * -@TestCaseID: ThreadConstructors14
 *- @TestCaseName: Thread_ThreadConstructors14.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Negative input for Constructors Thread(ThreadGroup group, Runnable target)
 *- @Brief: see below
 * -#step1: Create a thread instance.
 * -#step2: Start the thread.
 * -#step3: Test Thread(ThreadGroup group, Runnable target) with null params.
 * -#step4: Check that no wrong or exception when Thread(ThreadGroup group, Runnable target)'s params are null.
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadConstructors14.java
 *- @ExecuteClass: ThreadConstructors14
 *- @ExecuteArgs:
 */

public class ThreadConstructors14 extends Thread {
    static int i = 0;

    public ThreadConstructors14(ThreadGroup group, Runnable target) {
        super(group, target);
    }

    public static void main(String[] args) {

        ThreadConstructors14 test_illegal1 = new ThreadConstructors14(null, null);

        test_illegal1.start();
        try {
            test_illegal1.join();
        } catch (InterruptedException e) {
            System.out.println("InterruptedException");
        }

        if (i == 1) {
            System.out.println("0");
            return;
        }
        System.out.println("2");
        return;
    }

    public void run() {
        i++;
        super.run();
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n