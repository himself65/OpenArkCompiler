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
 * -@TestCaseID: ThreadConstructors7
 *- @TestCaseName: Thread_ThreadConstructors7.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Constructors Thread(Runnable target, String name), use super() in subclass of Thread, both run()
  *                     in target and subclass will be executed.
 *- @Brief: see below
 * -#step1: 创建一个ThreadConstructors7_a类的实例对象threadConstructors7_a；
 * -#step2: 以threadConstructors7_a、good为参数，创建一个ThreadConstructors7类的实例对象threadConstructors7，并且
 *          ThreadConstructors7类的run()方法中含有super.run()；
 * -#step3: 调用threadConstructors7的start()方法启动该线程；
 * -#step4: 调用threadConstructors7的join()方法；
 * -#step5: 经判断得知ThreadConstructors7类和ThreadConstructors7_a类的int类型的静态变量i和t经调用各自内部的run()方法后均
 *          为1，并且threadConstructors7.getName()的返回值与字符串"good"相同；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadConstructors7.java
 *- @ExecuteClass: ThreadConstructors7
 *- @ExecuteArgs:
 */

class ThreadConstructors7_a implements Runnable {
    static int t = 0;

    public void run() {
        t++;
    }
}

public class ThreadConstructors7 extends Thread {
    static int i = 0;

    public ThreadConstructors7(Runnable target, String name) {
        super(target, name);
    }

    public static void main(String[] args) {
        ThreadConstructors7_a threadConstructors7_a = new ThreadConstructors7_a();
        ThreadConstructors7 threadConstructors7 = new ThreadConstructors7(threadConstructors7_a, "good");
        threadConstructors7.start();
        try {
            threadConstructors7.join();
        } catch (InterruptedException e) {
            System.out.println("Join is interrupted");
        }
        if (i == 1) {
            if (ThreadConstructors7_a.t == 1) {
                if (threadConstructors7.getName().equals("good")) {
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
// ASSERT: scan 0\n