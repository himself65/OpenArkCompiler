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
 * -@TestCaseID: ThreadcurrentThread
 *- @TestCaseName: Thread_ThreadCurrentThread.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: currentThread() Returns a reference to the currently executing thread object.
 *- @Brief: see below
 * -#step1: 创建两个ThreadCurrentThread类的实例对象并分别命名为threadCurrentThread1、threadCurrentThread2；
 * -#step2: 给threadCurrentThread1起一个线程名为good，并调用其start()方法启动该线程，给threadCurrentThread2起一个线程名
 *          为bad，并调用其start()方法启动该线程；
 * -#step3: 分别调用threadCurrentThread1、threadCurrentThread2的join()方法；
 * -#step4: 经判断得知currentThread().getName()的返回值与字符串"main"相同，并且ThreadCurrentThread类因继承自Thread类，其
 *          内部调用了两次run()方法后使得int类型的静态变量nameMatch由0变为2；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadCurrentThread.java
 *- @ExecuteClass: ThreadCurrentThread
 *- @ExecuteArgs:
 */

public class ThreadCurrentThread extends Thread {
    static int nameMatch = 0;
    Object a = new Object();

    public static void main(String[] args) {
        ThreadCurrentThread threadCurrentThread1 = new ThreadCurrentThread();
        ThreadCurrentThread threadCurrentThread2 = new ThreadCurrentThread();
        threadCurrentThread1.setName("good");
        threadCurrentThread2.setName("bad");
        threadCurrentThread1.start();
        threadCurrentThread2.start();
        try {
            threadCurrentThread1.join();
            threadCurrentThread2.join();
        } catch (Exception e) {
            System.out.println("Join is interrupted");
        }
        if (currentThread().getName().equals("good")) {
            System.out.println(2);
        }
        if (currentThread().getName().equals("bad")) {
            System.out.println(3);
        }
        if (currentThread().getName().equals("main")) {
            if (nameMatch == 2) {
                System.out.println(0);
                return;
            }
        }
        System.out.println(2);
    }

    public synchronized void run() {
        if (currentThread().getName() == getName()) {
            synchronized (a) {
                nameMatch++;
            }
        }
        try {
            wait(500);
        } catch (InterruptedException e1) {
            System.out.println("Wait is interrupted");
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n