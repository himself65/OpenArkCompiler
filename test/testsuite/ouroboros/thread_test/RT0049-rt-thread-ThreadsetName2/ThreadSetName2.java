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
 * -@TestCaseID: ThreadSetName2
 *- @TestCaseName: Thread_ThreadSetName2.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Thread.setName(NULL) throws NullPointerException
 *- @Brief: see below
 * -#step1: Create a thread instance.
 * -#step2: Start the thread.
 * -#step3: Set a null name for thread.
 * -#step4: Check that throw NullPointerException when setName(String str)'s param is null.
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadSetName2.java
 *- @ExecuteClass: ThreadSetName2
 *- @ExecuteArgs:
 */

public class ThreadSetName2 extends Thread {
    public static void main(String[] args) {
        String string = null;
        ThreadSetName2 threa_obj = new ThreadSetName2();
        try {
            threa_obj.setName(string);
            System.out.println(2);
        } catch (NullPointerException e) {
            System.out.println(0);
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n