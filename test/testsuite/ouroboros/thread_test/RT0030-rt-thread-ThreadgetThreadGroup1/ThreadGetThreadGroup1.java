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
 * -@TestCaseID: ThreadGetThreadGroup1
 *- @TestCaseName: Thread_ThreadGetThreadGroup1.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Get thread groups for threads
 *- @Brief: see below
 * -#step1: 以god为参数，创建一个ThreadGroup类的实例对象threadGroup；
 * -#step2: 通过threadGroup的setMaxPriority()方法设置其属性为7；
 * -#step3: 以threadGroup、banana为参数，创建一个ThreadGetThreadGroup1类的实例对象threadGetThreadGroup1，且
 *          ThreadGetThreadGroup1类继承自Thread类；
 * -#step4: 通过threadGetThreadGroup1的getThreadGroup()方法的返回值记为string；
 * -#step5: 经判断得知string与字符串"java.lang.ThreadGroup[name=god,maxpri=7]"相同；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadGetThreadGroup1.java
 *- @ExecuteClass: ThreadGetThreadGroup1
 *- @ExecuteArgs:
 */

public class ThreadGetThreadGroup1 extends Thread {
    public ThreadGetThreadGroup1(ThreadGroup group, String name) {
        super(group, name);
    }

    public static void main(String[] args) {
        String string;
        ThreadGroup threadGroup = new ThreadGroup("god");
        threadGroup.setMaxPriority(7);
        ThreadGetThreadGroup1 threadGetThreadGroup1 = new ThreadGetThreadGroup1(threadGroup, "banana");
        string = threadGetThreadGroup1.getThreadGroup().toString();
        if (string.equals("java.lang.ThreadGroup[name=god,maxpri=7]")) {
            System.out.println(0);
        } else {
            System.out.println(2);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0