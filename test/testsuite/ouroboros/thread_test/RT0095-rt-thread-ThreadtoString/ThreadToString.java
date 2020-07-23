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
 * -@TestCaseID: ThreadToString
 *- @TestCaseName: Thread_ThreadToString.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: toString() Returns a string representation of this thread, including the thread's name,
 *                      priority, and thread group.
 *- @Brief: see below
 * -#step1: 以god为参数，创建一个ThreadGroup类的实例对象threadGroup；
 * -#step2: 以threadGroup、good为参数，创建一个ThreadToString类的实例对象threadToString，并且ThreadToString类继承自
 *          Thread类；
 * -#step3: 通过threadToString的setPriority()方法设置其优先级为8；
 * -#step4: 令threadToString.toString()的返回值为string；
 * -#step5: 经判断得知string的信息与字符串"Thread[good,8,god]"相同；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadToString.java
 *- @ExecuteClass: ThreadToString
 *- @ExecuteArgs:
 */

public class ThreadToString extends Thread {
    public ThreadToString(ThreadGroup group, String name) {
        super(group, name);
    }

    public static void main(String[] args) {
        String string;
        ThreadGroup threadGroup = new ThreadGroup("god");
        ThreadToString threadToString = new ThreadToString(threadGroup, "good");
        threadToString.setPriority(8);
        string = threadToString.toString();
        if (string.equals("Thread[good,8,god]")) {
            System.out.println(0);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n