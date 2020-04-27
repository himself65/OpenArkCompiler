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
 * -@TestCaseID: ThreadSetPriority6
 *- @TestCaseName: Thread_ThreadSetPriority6.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: SetMaxPriority() does not change the priority of threads in the Thread Group
 *- @Brief: see below
 * -#step1: Create a new ThreadGroup instance.
 * -#step2: Assign the new thread to thread group.
 * -#step3: Set priority to thread group.
 * -#step4: Check that the priority of threads in the Thread Group was not change by calling SetMaxPriority().
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadSetPriority6.java
 *- @ExecuteClass: ThreadSetPriority6
 *- @ExecuteArgs:
 */

public class ThreadSetPriority6 extends Thread {
    public ThreadSetPriority6(ThreadGroup group, String name) {
        super(group, name);
    }

    public static void main(String[] args) {
        ThreadGroup cls1 = new ThreadGroup("god");
        ThreadSetPriority6 cls2 = new ThreadSetPriority6(cls1, "good");
        cls1.setMaxPriority(3);
        if (cls1.getMaxPriority() == 3 && cls2.getPriority() == 5) {
            System.out.println(0);
            return;
        }
        System.out.println(2);
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0