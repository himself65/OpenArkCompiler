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
 * -@TestCaseID: ThreadConstructors4
 *- @TestCaseName: Thread_ThreadConstructors4.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Constructors Thread(ThreadGroup group, Runnable target), use super() in subclass of Thread,
 *                      both run() in target and subclass will be executed
 *- @Brief: see below
 * -#step1: 以god为参数，创建一个ThreadGroup类的实例对象threadGroup；
 * -#step2: 创建一个ThreadConstructors4_a类的实例对象threadConstructors4_a；
 * -#step3: 以threadGroup、threadConstructors4_a为参数，创建一个ThreadConstructors4类的实例对象threadConstructors4，并且
 *          ThreadConstructors4类的run()中含有super.run()；
 * -#step4: 令threadConstructors4.getThreadGroup().toString()的返回值为message；
 * -#step5: 调用threadConstructors4的start()方法启动该线程；
 * -#step6: 调用threadConstructors4的join()方法；
 * -#step7: 经判断得知ThreadConstructors4类和ThreadConstructors4_a类的int类型的静态变量经调用各自类内部的run()方法后均由
 *          0变为1，且message的信息与字符串"java.lang.ThreadGroup[name=god,maxpri=10]"相同；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadConstructors4.java
 *- @ExecuteClass: ThreadConstructors4
 *- @ExecuteArgs:
 */

class ThreadConstructors4_a implements Runnable {
    static int t = 0;

    public void run() {
        t++;
    }
}

public class ThreadConstructors4 extends Thread {
    static int i = 0;

    public ThreadConstructors4(ThreadGroup group, Runnable target) {
        super(group, target);
    }

    public static void main(String[] args) {
        String message;
        ThreadGroup threadGroup = new ThreadGroup("god");
        ThreadConstructors4_a threadConstructors4_a = new ThreadConstructors4_a();
        ThreadConstructors4 threadConstructors4 = new ThreadConstructors4(threadGroup, threadConstructors4_a);
        message = threadConstructors4.getThreadGroup().toString();
        threadConstructors4.start();
        try {
            threadConstructors4.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        if (i == 1) {
            if (ThreadConstructors4_a.t == 1) {
                if (message.equals("java.lang.ThreadGroup[name=god,maxpri=10]")) {
                    System.out.println(0);
                    return;
                }
            }
        }
        System.out.println(2);
    }

    public void run() {
        i++;
        super.run();
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n