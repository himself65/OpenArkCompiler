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
 * -@TestCaseID: ThreadConstructors8
 *- @TestCaseName: Thread_ThreadConstructors8.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Constructors Thread(ThreadGroup group, Runnable target, String name), use super() in subclass of
  *                     Thread, both run() in target and subclass will be executed
 *- @Brief: see below
 * -#step1: 以god为参数，创建一个ThreadGroup类的实例对象threadGroup；
 * -#step2: 创建一个ThreadConstructors8_a类的实例对象threadConstructors8_a；
 * -#step3: 以threadGroup、threadConstructors8_a、good为参数，创建一个ThreadConstructors8类的实例对象
 *          threadConstructors8，并且ThreadConstructors8类的run()方法中含有super.run()；
 * -#step4: 令threadConstructors8.getThreadGroup().toString()的返回值为message；
 * -#step5: 调用threadConstructors8的start()方法启动该线程；
 * -#step6: 调用threadConstructors8的join()方法；
 * -#step7: 经判断得知ThreadConstructors8类和ThreadConstructors8_a类的int类型的静态变量i和t经调用各自内部的run()方法后均
 *          由0变为1，并且message的信息与字符串"java.lang.ThreadGroup[name=god,maxpri=10]"相同，另外
 *          threadConstructors8.getName()的返回值与字符串"good"相同；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadConstructors8.java
 *- @ExecuteClass: ThreadConstructors8
 *- @ExecuteArgs:
 */

class ThreadConstructors8_a implements Runnable {
    static int t = 0;

    public void run() {
        t++;
    }
}

public class ThreadConstructors8 extends Thread {
    static int i = 0;

    public ThreadConstructors8(ThreadGroup group, Runnable target, String name) {
        super(group, target, name);
    }

    public static void main(String[] args) {
        String message;
        ThreadGroup threadGroup = new ThreadGroup("god");
        ThreadConstructors8_a threadConstructors8_a = new ThreadConstructors8_a();
        ThreadConstructors8 threadConstructors8 = new ThreadConstructors8(threadGroup, threadConstructors8_a,
                "good");
        message = threadConstructors8.getThreadGroup().toString();
        threadConstructors8.start();
        try {
            threadConstructors8.join();
        } catch (InterruptedException e) {
            System.out.println("Join is interrupted");
        }
        if (i == 1 && ThreadConstructors8_a.t == 1) {
            if (message.equals("java.lang.ThreadGroup[name=god,maxpri=10]")) {
                if (threadConstructors8.getName().equals("good")) {
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