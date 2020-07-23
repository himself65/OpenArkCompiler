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
 * -@TestCaseID: ThreadGetThreadGroup2
 *- @TestCaseName: Thread_ThreadGetThreadGroup2.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: getThreadGroup() returns null if this thread has died (been stopped).
 *- @Brief: see below
 * -#step1: 以god为参数，创建一个ThreadGroup类的实例对象threadGroup；
 * -#step2: 通过threadGroup的setMaxPriority()方法设置其属性为7；
 * -#step3: 以threadGroup、banana为参数，创建一个ThreadGetThreadGroup2类的实例对象threadGetThreadGroup2，且
 *          ThreadGetThreadGroup2类继承自Thread类；
 * -#step4: 调用threadGetThreadGroup2的start()方法启动该线程；
 * -#step5: 调用threadGetThreadGroup2的join()方法；
 * -#step6: 尝试将threadGetThreadGroup2的getThreadGroup()方法的返回值赋值给string；
 * -#step7: 经判断得知step6将会抛出NullPointerException；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadGetThreadGroup2.java
 *- @ExecuteClass: ThreadGetThreadGroup2
 *- @ExecuteArgs:
 */

public class ThreadGetThreadGroup2 extends Thread {
    public ThreadGetThreadGroup2(ThreadGroup group, String name) {
        super(group, name);
    }

    public static void main(String[] args) {
        String string;
        ThreadGroup threadGroup = new ThreadGroup("god");
        threadGroup.setMaxPriority(7);
        ThreadGetThreadGroup2 threadGetThreadGroup2 = new ThreadGetThreadGroup2(threadGroup, "banana");
        threadGetThreadGroup2.start();
        try {
            threadGetThreadGroup2.join();
        } catch (InterruptedException e1) {
            System.out.println("Join is interrupted");
        }
        try {
            string = threadGetThreadGroup2.getThreadGroup().toString();
            System.out.println(2);
        } catch (NullPointerException e2) {
            System.out.println(0);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n