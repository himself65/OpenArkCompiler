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
 * -@TestCaseID: ThreadStateJoin14
 *- @TestCaseName: Thread_ThreadStateJoin14.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Verify waiting for negative seconds throws IllegalArgumentException.
 *- @Brief: see below
 * -#step1: 定义继承Thread类的线程类ThreadStateJoin14。
 * -#step2: 定义run()方法，循环三次，指定synchronized的对象，指定休眠时间。
 * -#step2: 通过new关键字得到ThreadStateJoin14类的线程对象。
 * -#step3：启动线程对象。调用join()方法，指定否定秒为负值。确认抛出IllegalArgumentException。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadStateJoin14.java
 *- @ExecuteClass: ThreadStateJoin14
 *- @ExecuteArgs:
 */

public class ThreadStateJoin14 extends Thread {
    public static void main(String[] args) {
        ThreadStateJoin14 cls = new ThreadStateJoin14();
        cls.start();
        try {
            cls.join(-10);
        } catch (InterruptedException e1) {
            System.out.println(2);
        } catch (IllegalArgumentException e2) {
            System.out.println(0);
        }
    }

    public void run() {
        synchronized (currentThread()) {
            for (int i = 1; i <= 3; i++) {
                try {
                    sleep(200);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n