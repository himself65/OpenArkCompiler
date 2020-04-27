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
 * -@TestCaseID: ThreadTest
 *- @TestCaseName: Thread_ThreadTest.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Verify the setName(null) method, and throw NullPointerException.
 *- @Brief: see below
 * -#step1: 通过new得到Thread的线程对象，通过setName()设置线程名称。输出线程设置的名称。
 * -#step2: 通过new得到Thread的线程对象，通过setName()设置一个空的名称。抛出NullPointerException。
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadTest.java
 *- @ExecuteClass: ThreadTest
 *- @ExecuteArgs:
 */
public class ThreadTest {
    // Test for setName()
    public static void main(String[] args) throws Exception {
        Thread thread = new Thread();
        String newName = "maple_thread";
        thread.setName(newName);
        System.out.println("set the new name -- " + newName + " ---- " + thread.getName());

        /**
         * Verify the setName(null) method, and throw NullPointerException
         */
        Thread t = new Thread();
        try {
            t.setName(null);
        } catch (NullPointerException e) {
            System.out.println("setName() should not accept null names");
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan set\s*the\s*new\s*name\s*\-\-\s*maple_thread\s*\-\-\-\-\s*maple_thread\s*setName\(\)\s*should\s*not\s*accept\s*null\s*names