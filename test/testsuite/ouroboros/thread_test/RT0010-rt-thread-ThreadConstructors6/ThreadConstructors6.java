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
 * -@TestCaseID: ThreadConstructors6
 *- @TestCaseName: Thread_ThreadConstructors6.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Constructors Thread(ThreadGroup group, String name).
 *- @Brief: see below
 * -#step1: 以god为参数，创建一个ThreadGroup类的实例对象threadGroup；
 * -#step2: 以threadGroup、good为参数，创建一个ThreadConstructors6类的实例对象threadConstructors6；
 * -#step3: 经判断得知threadConstructors6.toString()的返回值与字符串"Thread[good,5,god]"相同；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadConstructors6.java
 *- @ExecuteClass: ThreadConstructors6
 *- @ExecuteArgs:
 */

public class ThreadConstructors6 extends Thread {
    public ThreadConstructors6(ThreadGroup group, String name) {
        super(group, name);
    }

    public static void main(String[] args) {
        ThreadGroup threadGroup = new ThreadGroup("god");
        ThreadConstructors6 threadConstructors6 = new ThreadConstructors6(threadGroup, "good");
        if (threadConstructors6.toString().equals("Thread[good,5,god]")) {
            System.out.println(0);
            return;
        }
        System.out.println(2);
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n