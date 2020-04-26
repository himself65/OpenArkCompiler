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
 * -@TestCaseID: ThreadStateJoin11
 *- @TestCaseName: Thread_ThreadStateJoin11.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Verify the accuracy of join time.
 *- @Brief: see below
 * -#step1: 定义继承Thread类的线程类ThreadStateJoin11。
 * -#step2: 定义run()方法，指定synchronized的对象，指定休眠时间。
 * -#step2: 通过new关键字得到ThreadStateJoin11类的线程对象。
 * -#step3：启动线程对象，获取启动时的当前时间。调用join()方法，获取线程结束时的当前时间。计算花费时间在970~1030毫秒之间。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadStateJoin11.java
 *- @ExecuteClass: ThreadStateJoin11
 *- @ExecuteArgs:
 */

public class ThreadStateJoin11 extends Thread {
    static Object ob = "aa";

    public static void main(String[] args) {
        long startTime;
        long endTime;
        ThreadStateJoin11 cls = new ThreadStateJoin11();
        cls.start();
        startTime = System.currentTimeMillis();
        try {
            cls.join(1000);
        } catch (InterruptedException ex) {
            System.out.println("Join is interrupted");
        }
        endTime = System.currentTimeMillis();
        if (endTime - startTime > 970 && endTime - startTime < 1030) {
            System.out.println(0);
            return;
        }
        System.out.println(2);
    }

    public void run() {
        synchronized (ob) {
            try {
                sleep(2000);
            } catch (InterruptedException e) {
                System.out.println("Sleep is interrupted");
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n