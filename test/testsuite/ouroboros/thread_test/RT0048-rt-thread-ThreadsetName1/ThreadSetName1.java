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
 * -@TestCaseID: ThreadSetName1
 *- @TestCaseName: Thread_ThreadSetName1.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Set Thread name
 *- @Brief: see below
 * -#step1: Create a thread instance.
 * -#step2: Start the thread.
 * -#step3: Set a name for thread.
 * -#step4: Check that name of thread is right.
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadSetName1.java
 *- @ExecuteClass: ThreadSetName1
 *- @ExecuteArgs:
 */

public class ThreadSetName1 extends Thread {
    public static void main(String[] args) {
        ThreadSetName1 thread_obj = new ThreadSetName1();
        thread_obj.start();
        thread_obj.setName("我abc123!?*{$");
        if (thread_obj.getName().equals("我abc123!?*{$")) {
            System.out.println(0);
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0