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
 * -@TestCaseID: ThreadActiveCount
 *- @TestCaseName: Thread_ThreadActiveCount.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Thread.activeCount() returns an estimate of the number of active threads in the current thread's
  *                     thread group and its subgroups.
 *- @Brief: see below
 * -#step1: 以god为参数，创建一个ThreadGroup类的实例对象threadGroup；
 * -#step2: 以threadGroup、"banana" + i（i < 10）为参数，新建一个ThreadActiveCount线程，并调用start()方法启动该线程；
 * -#step3: 让当前线程休眠500ms；
 * -#step4: 因ThreadActiveCount类继承自Thread类，导致静态int型变量p的值经内部多次调用run()方法后变为10；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadActiveCount.java
 *- @ExecuteClass: ThreadActiveCount
 *- @ExecuteArgs:
 */

public class ThreadActiveCount extends Thread {
    static int num;
    static Object ob = "aa";

    public ThreadActiveCount(ThreadGroup group, String name) {
        super(group, name);
    }

    public static void main(String[] args) {
        ThreadGroup threadGroup = new ThreadGroup("god");
        for (int i = 0; i < 10; i++) {
            (new ThreadActiveCount(threadGroup, "banana" + i)).start();
        }
        try {
            sleep(500);
        } catch (InterruptedException e1) {
            e1.printStackTrace();
        }
        if (num == 10) {
            System.out.println(0);
        } else {
            System.out.println(2);
        }
    }

    public void run() {
        synchronized (ob) {
            num = activeCount();
            try {
                ob.wait(1000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n