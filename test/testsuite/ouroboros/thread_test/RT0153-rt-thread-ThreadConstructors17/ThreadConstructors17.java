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
 * -@TestCaseID: ThreadConstructors17
 *- @TestCaseName: Thread_ThreadConstructors17.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Negative input for Constructors Thread(ThreadGroup group, String name)
 *- @Brief: see below
 * -#step1: Create thread instance.
 * -#step2: Test Constructors Thread(ThreadGroup group, String name) with different params.
 * -#step3: Check that NullPointerException was threw when name param was null;
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadConstructors17.java
 *- @ExecuteClass: ThreadConstructors17
 *- @ExecuteArgs:
 */

public class ThreadConstructors17 extends Thread {
    static int i = 0;
    static int ecount = 0;

    public ThreadConstructors17(ThreadGroup group, String name) {
        super(group, name);
    }

    public static void main(String[] args) {
        try {
            ThreadConstructors17 test_illegal1 = new ThreadConstructors17(null, null);
        } catch (NullPointerException e) {
            //System.out.println("NullPointerException");
            ecount++;
        }

        ThreadConstructors17 test_illegal2 = new ThreadConstructors17(null, "");
        ThreadConstructors17 test_illegal3 = new ThreadConstructors17(null, new String());

        test_illegal2.start();
        try {
            test_illegal2.join();
        } catch (InterruptedException e) {
            System.out.println("InterruptedException");
        }

        test_illegal3.start();
        try {
            test_illegal3.join();
        } catch (InterruptedException e) {
            System.out.println("InterruptedException");
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