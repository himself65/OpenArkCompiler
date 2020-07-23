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
 * -@TestCaseID: ThreadSetPriority1
 *- @TestCaseName: Thread_ThreadSetPriority1.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Repeatedly set thread priority
 *- @Brief: see below
 * -#step1: Create four thread instance.
 * -#step2: Set priority for four thread.
 * -#step3: Start the threads.
 * -#step4: Check that priority of instances was set correctly.
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadSetPriority1.java
 *- @ExecuteClass: ThreadSetPriority1
 *- @ExecuteArgs:
 */

public class ThreadSetPriority1 extends Thread {
    public static void main(String[] args) {
        ThreadSetPriority1 thread_obj1 = new ThreadSetPriority1();
        ThreadSetPriority1 thread_obj2 = new ThreadSetPriority1();
        ThreadSetPriority1 thread_obj3 = new ThreadSetPriority1();
        ThreadSetPriority1 thread_obj4 = new ThreadSetPriority1();
        thread_obj1.setPriority(6);
        thread_obj4.setPriority(MIN_PRIORITY);
        thread_obj2.setPriority(MAX_PRIORITY);
        thread_obj3.setPriority(3);
        thread_obj1.setPriority(2);
        thread_obj2.setPriority(MIN_PRIORITY);
        thread_obj3.setPriority(NORM_PRIORITY);
        thread_obj4.setPriority(7);
        thread_obj1.start();
        thread_obj2.start();
        thread_obj3.start();
        thread_obj4.start();
        if (thread_obj1.getPriority() != 2) {
            System.out.println(2);
        }
        if (thread_obj2.getPriority() != 1) {
            System.out.println(2);
        }
        if (thread_obj3.getPriority() != 5) {
            System.out.println(2);
        }
        if (thread_obj4.getPriority() != 7) {
            System.out.println(2);
        }
        System.out.println(0);
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n