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
 * -@TestCaseID: ThreadStateJoin15
 *- @TestCaseName: Thread_ThreadStateJoin15.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Validation waits for negative milliseconds  throws IllegalArgumentException.
 *- @Brief: see below
 * -#step1: 定义继承Thread类的线程类ThreadStateJoin15。
 * -#step2: 定义run()方法，循环三次，指定synchronized的对象，指定休眠时间。
 * -#step2: 通过new关键字得到ThreadStateJoin15类的线程对象。
 * -#step3：启动线程对象。调用join()方法，构造参数nanos为负值，millis为1000。确认会抛出IllegalArgumentException。
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadStateJoin15.java
 *- @ExecuteClass: ThreadStateJoin15
 *- @ExecuteArgs:
 */

public class ThreadStateJoin15 extends Thread {
    public static void main(String[] args) {
        ThreadStateJoin15 cls = new ThreadStateJoin15();
        cls.start();
        try {
            cls.join(1000, -10);
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