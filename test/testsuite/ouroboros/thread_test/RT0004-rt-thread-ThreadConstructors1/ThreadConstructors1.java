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
 * -@TestCaseID: ThreadConstructors1
 *- @TestCaseName: Thread_ThreadConstructors1.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Constructors Thread(): create a new Thread with no parameter
 *- @Brief: see below
 * -#step1: 创建一个Thread类的实例对象threadConstructors1；
 * -#step2: 调用threadConstructors1的toString()的返回值与字符串"Thread[Thread-0,5,main]"相同，表明创建一个没有参数的新
 *          线程成功；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadConstructors1.java
 *- @ExecuteClass: ThreadConstructors1
 *- @ExecuteArgs:
 */

public class ThreadConstructors1 extends Thread {
    public ThreadConstructors1() {
    }

    public static void main(String[] args) {
        ThreadConstructors1 threadConstructors1 = new ThreadConstructors1();
        if (threadConstructors1.toString().equals("Thread[Thread-0,5,main]")) {
            System.out.println(0);
            return;
        }
        System.out.println(2);
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n