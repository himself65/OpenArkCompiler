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
 * -@TestCaseID: ThreadConstructors9
 *- @TestCaseName: Thread_ThreadConstructors9.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Constructors Thread(ThreadGroup group, Runnable target, String name, long stackSize),use super()
  *                     in subclass of Thread, both run() in target and subclass will be executed
 *- @Brief: see below
 * -#step1: 以god为参数，创建一个ThreadGroup类的实例对象threadGroup；
 * -#step2: 创建一个ThreadConstructors9_a类的实例对象threadConstructors9_a；
 * -#step3: 以threadGroup、threadConstructors9_a、good为参数，创建一个ThreadConstructors9类的实例对象
 *          threadConstructors9，并且ThreadConstructors9类的run()方法中含有super.run()；
 * -#step4: 令threadConstructors9.getThreadGroup().toString()的返回值为message；
 * -#step5: 调用threadConstructors9的start()方法启动该线程；
 * -#step6: 调用threadConstructors9的join()方法；
 * -#step7: 经判断得知ThreadConstructors9类和ThreadConstructors9_a类的int类型的静态变量i和t经调用各自内部的run()方法后均
 *          由0变为1，并且k = stackSize= 1073741824, message的信息与字符串"java.lang.ThreadGroup[name=god,maxpri=10]"相
 *          同，另外threadConstructors9.getName()的返回值与字符串"good"相同；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadConstructors9.java
 *- @ExecuteClass: ThreadConstructors9
 *- @ExecuteArgs:
 */

class ThreadConstructors9_a implements Runnable {
    static int t = 0;

    public void run() {
        t++;
    }
}

public class ThreadConstructors9 extends Thread {
    static int i = 0;
    static long k;

    public ThreadConstructors9(ThreadGroup group, Runnable target, String name, long stackSize) {
        super(group, target, name, stackSize);
        k = stackSize;
    }

    public static void main(String[] args) {
        String message;
        ThreadGroup threadGroup = new ThreadGroup("god");
        ThreadConstructors9_a threadConstructors9_a = new ThreadConstructors9_a();
        ThreadConstructors9 threadConstructors9 = new ThreadConstructors9(threadGroup, threadConstructors9_a,
                "good", 1 << 30);
        message = threadConstructors9.getThreadGroup().toString();
        threadConstructors9.start();
        try {
            threadConstructors9.join();
        } catch (InterruptedException e) {
            System.out.println("Join is interrupted");
        }
        if (i == 1 && k == 1073741824 && ThreadConstructors9_a.t == 1) {
            if (message.equals("java.lang.ThreadGroup[name=god,maxpri=10]")) {
                if (threadConstructors9.getName().equals("good")) {
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