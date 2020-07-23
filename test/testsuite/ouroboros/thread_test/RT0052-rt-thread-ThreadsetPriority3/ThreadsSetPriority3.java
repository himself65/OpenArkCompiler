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
 * -@TestCaseID: ThreadsSetPriority3
 *- @TestCaseName: Thread_ThreadsSetPriority3.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: SetPriority() throws IllegalArgumentException If the priority is not in the range MIN_PRIORITY
 *                      to MAX_PRIORITY
 *- @Brief: see below
 * -#step1: Create four thread instance.
 * -#step2: Set priority of thread.
 * -#step3: Check that throw the IllegalArgumentException when priority out of range.
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadsSetPriority3.java
 *- @ExecuteClass: ThreadsSetPriority3
 *- @ExecuteArgs:
 */

public class ThreadsSetPriority3 extends Thread {
    public static void main(String[] args) {
        ThreadsSetPriority3 thread_obj1 = new ThreadsSetPriority3();
        ThreadsSetPriority3 thread_obj2 = new ThreadsSetPriority3();
        ThreadsSetPriority3 thread_obj3 = new ThreadsSetPriority3();
        ThreadsSetPriority3 thread_obj4 = new ThreadsSetPriority3();
        try {
            thread_obj1.setPriority(11);
            System.out.println(2);
        } catch (IllegalArgumentException e1) {
            try {
                thread_obj2.setPriority(0);
                System.out.println(2);
            } catch (IllegalArgumentException e2) {
                try {
                    thread_obj3.setPriority(MAX_PRIORITY + 1);
                    System.out.println(2);
                } catch (IllegalArgumentException e3) {
                    try {
                        thread_obj4.setPriority(MIN_PRIORITY - 1);
                        System.out.println(2);
                    } catch (IllegalArgumentException e4) {
                        System.out.println(0);
                    }
                }
            }
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n