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
 * -@TestCaseID: ThreadStateJoin8
 *- @TestCaseName: Thread_ThreadStateJoin8.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Wait time unit seconds target thread is still sleeping.
 *- @Brief: see below
 * -#step1: 创建一个ThreadStateJoin8类的实例对象threadStateJoin8，并且ThreadStateJoin8类继承自Thread类；
 * -#step2: 调用threadStateJoin8的start()方法启动该线程；
 * -#step3: 调用threadStateJoin8的join()方法，参数为1000；
 * -#step4: run方法循环5次，每次sleep后int类型的全局静态变量t的值加1；
 * -#step5: 经判断得知threadStateJoin8.getState().toString()的返回值与字符串"TIMED_WAITING"相同，并且ThreadStateJoin8类
 *          在其内部的run()方法执行完之后t的值变为3；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadStateJoin8.java
 *- @ExecuteClass: ThreadStateJoin8
 *- @ExecuteArgs:
 */

public class ThreadStateJoin8 extends Thread {
    static int t = 0;

    public static void main(String[] args) {
        ThreadStateJoin8 threadStateJoin8 = new ThreadStateJoin8();
        threadStateJoin8.start();
        try {
            threadStateJoin8.join(1000);
            t++;
        } catch (InterruptedException e1) {
            System.out.println("Join is interrupted");
        }
        if (threadStateJoin8.getState().toString().equals("TIMED_WAITING") && t == 3) {
            System.out.println(0);
            return;
        }
    }

    public void run() {
        for (int i = 1; i <= 5; i++) {
            try {
                sleep(400);
                t++;
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n