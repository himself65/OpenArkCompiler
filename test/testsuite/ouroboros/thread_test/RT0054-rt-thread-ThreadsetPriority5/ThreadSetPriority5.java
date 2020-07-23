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
 * -@TestCaseID: ThreadSetPriority5
 *- @TestCaseName: Thread_ThreadSetPriority5.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: SetPriority(NULL) throws NullPointerException
 *- @Brief: see below
 * -#step1: Create thread instance.
 * -#step2: Test SetPriority(NULL) when its param is null.
 * -#step3: Check that NullPointerException was threw.
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadSetPriority5.java
 *- @ExecuteClass: ThreadSetPriority5
 *- @ExecuteArgs:
 */

public class ThreadSetPriority5 extends Thread {
    public static void main(String[] args) {
        Integer i = null;
        ThreadSetPriority5 thread_obj1 = new ThreadSetPriority5();
        try {
            thread_obj1.setPriority(i);
            System.out.println(2);
        } catch (IllegalArgumentException e) {
            System.out.println(2);
        } catch (NullPointerException e) {
            System.out.println(0);
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n