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
 * -@TestCaseID: ThreadStateJoin6
 *- @TestCaseName: Thread_ThreadStateJoin6.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Wait time unit seconds target thread does not end.
 *- @Brief: see below
 * -#step1: 创建一个ThreadStateJoin6类的实例对象threadStateJoin6，并且ThreadStateJoin6类继承自Thread类；
 * -#step2: 调用threadStateJoin6的start()方法启动该线程；
 * -#step3: 调用threadStateJoin6的join()方法，参数为500；
 * -#step4: run方法循环5次，每次int类型的全局静态变量t的值加1，然后wait，然后再加1；
 * -#step5: 经判断得知threadStateJoin6.getState().toString()的返回值与字符串"TIMED_WAITING"相同，并且ThreadStateJoin6类
 *          在其内部的run()方法执行完之后t的值变为2；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadStateJoin6.java
 *- @ExecuteClass: ThreadStateJoin6
 *- @ExecuteArgs:
 */

public class ThreadStateJoin6 extends Thread {
    static int t = 0;

    public static void main(String[] args) {
        ThreadStateJoin6 threadStateJoin6 = new ThreadStateJoin6();
        threadStateJoin6.start();
        try {
            threadStateJoin6.join(500);
            t++;
        } catch (InterruptedException e1) {
            System.out.println("Join is interrupted");
        }
        if (threadStateJoin6.getState().toString().equals("TIMED_WAITING") && t == 2) {
            System.out.println(0);
            return;
        }
        System.out.println(2);
    }

    public void run() {
        synchronized (currentThread()) {
            for (int i = 1; i <= 5; i++) {
                try {
                    t++;
                    wait(1000);
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