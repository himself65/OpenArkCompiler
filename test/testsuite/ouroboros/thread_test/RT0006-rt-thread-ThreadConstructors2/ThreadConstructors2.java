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
 * -@TestCaseID: ThreadConstructors2
 *- @TestCaseName: Thread_ThreadConstructors2.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Constructors Thread(Runnable target), use super() in subclass of Thread, both run() in target
 *                      and subclass will be executed
 *- @Brief: see below
 * -#step1: 创建一个ThreadConstructors2_a类的实例对象threadConstructors2_a；
 * -#step2: 以threadConstructors2_a为参数，创建一个ThreadConstructors2类的实例对象threadConstructors2，并且
 *          ThreadConstructors2类的run()中含有super.run()；
 * -#step3: 调用threadConstructors2的start()方法启动该线程；
 * -#step4: 调用threadConstructors2的join()方法；
 * -#step5: 经判断得知ThreadConstructors2类和ThreadConstructors2_a类的int类型的静态变量i和t经调用各自类的内部的run()方法
 *          后均由0变为1；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadConstructors2.java
 *- @ExecuteClass: ThreadConstructors2
 *- @ExecuteArgs:
 */

class ThreadConstructors2_a implements Runnable {
    static int t = 0;

    public void run() {
        t++;
    }
}

public class ThreadConstructors2 extends Thread {
    static int i = 0;

    public ThreadConstructors2(Runnable target) {
        super(target);
    }

    public static void main(String[] args) {
        ThreadConstructors2_a threadConstructors2_a = new ThreadConstructors2_a();
        ThreadConstructors2 threadConstructors2 = new ThreadConstructors2(threadConstructors2_a);
        threadConstructors2.start();
        try {
            threadConstructors2.join();
        } catch (InterruptedException e) {
        }
        if (i == 1) {
            if (ThreadConstructors2_a.t == 1) {
                System.out.println(0);
                return;
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
// ASSERT: scan 0\n