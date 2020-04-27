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
 * -@TestCaseID: ThreadConstructors3
 *- @TestCaseName: Thread_ThreadConstructors3.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Constructors Thread(Runnable target) don't use super() in subclass of Thread, only run() in
 *                      subclass will be executed.
 *- @Brief: see below
 * -#step1: 创建一个ThreadConstructors3_a类的实例对象threadConstructors3_a；
 * -#step2: 以threadConstructors3_a为参数，创建一个ThreadConstructors3类的实例对象threadConstructors3,并且
 *          ThreadConstructors3类的run()中不含有super.run()；
 * -#step3: 调用threadConstructors3的start()方法启动该线程；
 * -#step4: 调用threadConstructors3的join()方法；
 * -#step5: 经判断得知ThreadConstructors3类的int类型的静态变量i经调用其内部的run()方法后由0变为1，而
 *          ThreadConstructors3_a类的int类型的静态变量t始终为0；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadConstructors3.java
 *- @ExecuteClass: ThreadConstructors3
 *- @ExecuteArgs:
 */

class ThreadConstructors3_a implements Runnable {
    static int t = 0;

    public void run() {
        t++;
    }
}

public class ThreadConstructors3 extends Thread {
    static int i = 0;

    public ThreadConstructors3(Runnable target) {
        super(target);
    }

    public static void main(String[] args) {
        ThreadConstructors3_a threadConstructors3_a = new ThreadConstructors3_a();
        ThreadConstructors3 threadConstructors3 = new ThreadConstructors3(threadConstructors3_a);
        threadConstructors3.start();
        try {
            threadConstructors3.join();
        } catch (InterruptedException e) {
            System.out.println("Join is interrupted");
        }
        if (i == 1) {
            if (ThreadConstructors3_a.t == 0) {
                System.out.println(0);
                return;
            }
        }
        System.out.println(2);
    }

    public void run() {
        i++;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n