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
 * -@TestCaseID: ThreadSetPriority7
 *- @TestCaseName: Thread_ThreadSetPriority7.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: SetPriority() with value exceeds ThreadGroup's MaxPriority will not success,and does not throws
 *                      exception.
 *- @Brief: see below
 * -#step1: Create a new ThreadGroup instance.
 * -#step2: Assign the new thread to thread group.
 * -#step3: Set thread and thread group priority.
 * -#step4: Check that thread's priority will not greater than thread-group's.
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadSetPriority7.java
 *- @ExecuteClass: ThreadSetPriority7
 *- @ExecuteArgs:
 */

public class ThreadSetPriority7 extends Thread {
    public ThreadSetPriority7(ThreadGroup group, String name) {
        super(group, name);
    }

    public static void main(String[] args) {
        ThreadGroup threadGroup = new ThreadGroup("god");
        ThreadSetPriority7 threadSetPriority7 = new ThreadSetPriority7(threadGroup, "good");
        threadGroup.setMaxPriority(3);
        threadSetPriority7.setPriority(8);
        if (threadGroup.getMaxPriority() == 3 && threadSetPriority7.getPriority() == 3) {
            System.out.println(0);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n