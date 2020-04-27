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
 *- @Title/Destination: Test getThreadGroup().
 *- @Brief: see below
 * -#step1：通过new得到线程对象，通过getThreadGroup()得到线程对象所在的线程组对象。通过currentThread().getThreadGroup()
 *          得到当前正在执行的线程组对象。
 * -#step2：通过getName()得到线程组对象的引用并确认为main，与当前正常执行的线程组比较并打印，确认结果为true。
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadTest.java
 *- @ExecuteClass: ThreadTest
 *- @ExecuteArgs:
 */
public class ThreadTest {
    // Test for getThreadGroup()
    public static void main(String[] args) throws Exception {
        Thread t = new Thread();
        ThreadGroup threadGroup = t.getThreadGroup();
        ThreadGroup curThreadGroup = Thread.currentThread().getThreadGroup();
        System.out.println(threadGroup.getName());
        System.out.println(threadGroup.equals(curThreadGroup));
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan main\s*true