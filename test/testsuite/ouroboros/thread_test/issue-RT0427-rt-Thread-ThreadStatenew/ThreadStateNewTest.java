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
 * -@TestCaseID: ThreadStateNewTest
 *- @TestCaseName: Thread_ThreadStateNewTest.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Test getState() return NEW when the thread have not start.
 *- @Brief: see below
 * -#step1: 用用例的类来new一个对象。
 * -#step2: 用对象调用getState()方法。
 * -#step3：确认返回NEW。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadStateNewTest.java
 *- @ExecuteClass: ThreadStateNewTest
 *- @ExecuteArgs:
 */

public class ThreadStateNewTest extends Thread {
    public static void main(String[] args) {
        ThreadStateNewTest cls = new ThreadStateNewTest();
        if (cls.getState().toString().equals("NEW")) {
            System.out.println(0);
            return;
        }
        System.out.println(2);
        return;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n