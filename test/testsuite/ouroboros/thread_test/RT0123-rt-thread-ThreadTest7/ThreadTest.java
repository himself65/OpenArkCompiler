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
 *- @Title/Destination: the default priority of a thread is in range (MIN_PRIORITY,MAX_PRIORITY).
 *- @Brief: see below
 * -#step1: 创建一个Thread类的实例对象thread；
 * -#step2: 调用thread的getPriority()方法，获取线程的优先级并记为p；
 * -#step3: 确认该线程的优先级p介于最小优先级和最大优先级之间；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadTest.java
 *- @ExecuteClass: ThreadTest
 *- @ExecuteArgs:
 */
public class ThreadTest {
    //test for getPriority()
    public static void main(String[] args) throws Exception {
        Thread thread = new Thread();
        int p = thread.getPriority();
        System.out.println(Thread.MIN_PRIORITY + " <= " + p + " <= " + Thread.MAX_PRIORITY);
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 1 <= 5 <= 10\n