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
 * -@TestCaseID: ThreadConstructors16
 *- @TestCaseName: Thread_ThreadConstructors16.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Negative input for Thread(ThreadGroup group, Runnable target, String name, long stackSize)
 *- @Brief: see below
 * -#step1: Create thread instance.
 * -#step2: Test Thread(ThreadGroup group, Runnable target, String name, long stackSize) with different params.
 * -#step3: Check that whatever stackSize is any value,there is no exception threw.
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadConstructors16.java
 *- @ExecuteClass: ThreadConstructors16
 *- @ExecuteArgs:
 */

public class ThreadConstructors16 extends Thread {
    static int i = 0;
    static int ecount = 0;

    public ThreadConstructors16(ThreadGroup group, Runnable target, String name, long stackSize) {
        super(group, target, name, stackSize);
    }

    public static void main(String[] args) {
        try {
            ThreadConstructors16 test_illegal1 = new ThreadConstructors16(null, null, null, 0);
        } catch (NullPointerException e) {
            //System.out.println("NullPointerException");
            ecount++;
        }

        try {
            ThreadConstructors16 test_illegal2 = new ThreadConstructors16(null, null, "", Long.MAX_VALUE);
        } catch (OutOfMemoryError ee) {
            System.out.println("OutOfMemoryError");
            ecount += 2;
        }

        ThreadConstructors16[] test_illegal = new ThreadConstructors16[3];

        test_illegal[0] = new ThreadConstructors16(null, null, "", 100); //Long.MIN_VALUE);
        test_illegal[1] = new ThreadConstructors16(null, null, "", new Integer("-1"));
        test_illegal[2] = new ThreadConstructors16(null, null, "", new Short("00356"));

        for (ThreadConstructors16 thread : test_illegal) {
            thread.start();
            try {
                thread.join();
            } catch (InterruptedException e) {
               // System.out.println("InterruptedException");
            }
        }

        if (i == 3) {
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