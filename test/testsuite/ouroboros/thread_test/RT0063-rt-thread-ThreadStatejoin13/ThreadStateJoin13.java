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
 * -@TestCaseID: ThreadStateJoin13
 *- @TestCaseName: Thread_ThreadStateJoin13.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Validation join(0) equals infinite wait.
 *- @Brief: see below
 * -#step1: 定义继承Thread类的线程类ThreadStateJoin13。
 * -#step2: 定义run()方法，循环三次，指定synchronized的对象，指定休眠时间，t自增1。
 * -#step3: 通过new关键字得到ThreadStateJoin13类的线程对象。
 * -#step4：启动线程对象，调用join()方法，等待线程运行结束。获取线程的状态为TERMINATED。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadStateJoin13.java
 *- @ExecuteClass: ThreadStateJoin13
 *- @ExecuteArgs:
 */

public class ThreadStateJoin13 extends Thread {
    static int t = 0;

    public static void main(String[] args) {
        ThreadStateJoin13 cls = new ThreadStateJoin13();
        cls.start();
        try {
            cls.join(0);
            t++;
        } catch (InterruptedException e1) {
            System.out.println("Join is interrupted");
        }
        if (cls.getState().toString().equals("TERMINATED") && t == 4) {
            System.out.println(0);
        }
    }

    public void run() {
        synchronized (currentThread()) {
            for (int i = 1; i <= 3; i++) {
                try {
                    sleep(200);
                    t++;
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n