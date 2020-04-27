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
 * -@TestCaseID: ThreadConstructors15
 *- @TestCaseName: Thread_ThreadConstructors15.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Negative input for Constructors Thread(ThreadGroup group, Runnable target, String name)
 *- @Brief: see below
 * -#step1: Create a thread instance.
 * -#step2: Test Thread(ThreadGroup group, Runnable target, String name) with different params.
 * -#step3: Check that NullPointerException was threw when the params are null.
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadConstructors15.java
 *- @ExecuteClass: ThreadConstructors15
 *- @ExecuteArgs:
 */

public class ThreadConstructors15 extends Thread {
    static int i = 0;
    static int ecount = 0;

    public ThreadConstructors15(ThreadGroup group, Runnable target, String name) {
        super(group, target, name);
    }

    public static void main(String[] args) {
        try {
            ThreadConstructors15 test_illegal1 = new ThreadConstructors15(null, null, null);
        } catch (NullPointerException e) {
            //System.out.println("NullPointerException");
            ecount++;
        }
        ThreadConstructors15 test_illegal2 = new ThreadConstructors15(null, null, "");
        ThreadConstructors15 test_illegal3 = new ThreadConstructors15(null, null, "");

        test_illegal2.start();
        try {
            test_illegal2.join();
        } catch (InterruptedException e) {
            //System.out.println("NullPointerException");
        }

        test_illegal3.start();
        try {
            test_illegal3.join();
        } catch (InterruptedException e) {
            //System.out.println("NullPointerException");
        }

        if (i == 2) {
            if (ecount == 1) {
                System.out.println("0");
                return;
            }
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
// ASSERT: scan 0