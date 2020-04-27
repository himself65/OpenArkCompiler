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
 * -@TestCaseID: ThreadEnumerate
 *- @TestCaseName: Thread_ThreadEnumerate.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: enumerate(Thread[] tarray) Copies into the specified array every active thread in the current
 *                      thread's thread group and its subgroups.
 *- @Brief: see below
 * -#step1: 定义线程类继承Thread类，包含静态线程组变量，并构造含参的构造方法。
 * -#step2：定义线程安全的run()方法。
 * -#step3：调用ThreadGroup()并实例化对象，启动线程并将线程添加到指定线程组。
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadEnumerate.java
 *- @ExecuteClass: ThreadEnumerate
 *- @ExecuteArgs:
 */

public class ThreadEnumerate extends Thread {
    static Thread[] p;
    static int w, q;
    static Object ob = "aa";

    public ThreadEnumerate(ThreadGroup group, String name) {
        super(group, name);
    }

    public static void main(String[] args) {
        ThreadGroup cls1 = new ThreadGroup("god");
        for (int i = 0; i < 10; i++) {
            (new ThreadEnumerate(cls1, "banana" + i)).start();
        }
        try {
            sleep(500);
        } catch (InterruptedException e1) {
        }
        if (w == 10) {
            System.out.println(0);
        }
    }

    public void run() {
        synchronized (ob) {
            q = activeCount();
            p = new Thread[q + 1];
            w = enumerate(p);
            try {
                ob.wait(1000);
            } catch (InterruptedException e) {
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0