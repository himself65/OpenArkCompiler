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
 * -@TestCaseID: ThreadConstructors13
 *- @TestCaseName: Thread_ThreadConstructors13.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Negative input for Constructors Thread(String name)
 *- @Brief: see below
 * -#step1: Create thread instances.
 * -#step2: Test Constructors Thread(String name) with null and "" params.
 * -#step3: Check that Thread(String name) threw NullPointerException when param was null,and when param is "",it will
 *          be  normal.
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadConstructors13.java
 *- @ExecuteClass: ThreadConstructors13
 *- @ExecuteArgs:
 */

public class ThreadConstructors13 extends Thread {
    static int i = 0;
    private static int ecount = 0;

    public ThreadConstructors13(String name) {
        super(name);
    }

    public static void main(String[] args) {
        try {
            ThreadConstructors13 test_illegal1 = new ThreadConstructors13(null);
        } catch (NullPointerException e) {
            ecount++;
        }
        ThreadConstructors13 test_illegal2 = new ThreadConstructors13("");
        ThreadConstructors13 test_illegal3 = new ThreadConstructors13("");

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