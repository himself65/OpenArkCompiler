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
 * -@TestCaseID: ThreadSetPriority2
 *- @TestCaseName: Thread_ThreadSetPriority2.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Child thread inherits the priority of the main thread
 *- @Brief: see below
 * -#step1: Define a test class that inherited from thread class and override run().
 * -#step2: Set current thread's priority.
 * -#step3: Create two thread instance and start them.
 * -#step4: Check that all priority of child thread was same.
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadSetPriority2.java
 *- @ExecuteClass: ThreadSetPriority2
 *- @ExecuteArgs:
 */

class ThreadSetPriority_a extends Thread {
    public synchronized void run() {
        for (int i = 0; i < 10; i++) {
        }
    }
}

public class ThreadSetPriority2 extends ThreadSetPriority_a {
    public static void main(String[] args) {
        currentThread().setPriority(2);
        ThreadSetPriority2 thread_obj1 = new ThreadSetPriority2();
        ThreadSetPriority2 thread_obj2 = new ThreadSetPriority2();
        thread_obj1.start();
        thread_obj2.start();
        if (thread_obj1.getPriority() != 2) {
            System.out.println(2);
            return ;
        }
        if (thread_obj2.getPriority() != 2) {
            System.out.println(2);
            return ;
        }
        System.out.println(0);
        return;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n