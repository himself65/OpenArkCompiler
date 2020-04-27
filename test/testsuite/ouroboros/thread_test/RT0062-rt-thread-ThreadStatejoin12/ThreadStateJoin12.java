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
 * -@TestCaseID: ThreadStateJoin12
 *- @TestCaseName: Thread_ThreadStateJoin12.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Verify the accuracy of the wait time after the join gets the object lock.
 *- @Brief: see below
 * -#step1: 定义继承Thread类的线程类ThreadStateJoin12。
 * -#step2: 定义run()方法，指定synchronized的对象，指定休眠时间。
 * -#step2: 通过new关键字得到ThreadStateJoin12类的线程对象。
 * -#step3：启动线程对象，获取启动时的当前时间。调用join()方法，获取线程结束时的当前时间。计算花费时间在590~1010毫秒之间。
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadStateJoin12.java
 *- @ExecuteClass: ThreadStateJoin12
 *- @ExecuteArgs:
 */

public class ThreadStateJoin12 extends Thread {
    public static void main(String[] args) {
        long startTime;
        long endTime;
        ThreadStateJoin12 cls = new ThreadStateJoin12();
        cls.start();
        startTime = System.currentTimeMillis();
        try {
            cls.join(600);
        } catch (InterruptedException e1) {
            System.out.println("Join is interrupted");
        }
        endTime = System.currentTimeMillis();
        if (endTime - startTime > 590 && endTime - startTime < 1010) {
            System.out.println(0);
        }
    }

    public void run() {
        synchronized (currentThread()) {
            for (int i = 1; i <= 5; i++) {
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
// ASSERT: scan 0