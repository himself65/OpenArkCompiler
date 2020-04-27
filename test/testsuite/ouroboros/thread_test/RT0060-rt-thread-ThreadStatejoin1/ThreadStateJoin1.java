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
 * -@TestCaseID: ThreadStateJoin1
 *- @TestCaseName: Thread_ThreadStateJoin1.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Wait for target thread to end.
 *- @Brief: see below
 * -#step1: 定义继承Thread类的线程类ThreadStateJoin1。
 * -#step2: 通过new关键字得到ThreadStateInterrupt2类的线程对象。
 * -#step3：启动线程对象，调用join()方法确认该线程的状态为TERMINATED。
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadStateJoin1.java
 *- @ExecuteClass: ThreadStateJoin1
 *- @ExecuteArgs:
 */

public class ThreadStateJoin1 extends Thread {
    public static void main(String[] args) {
        ThreadStateJoin1 cls = new ThreadStateJoin1();
        cls.start();
        try {
            cls.join();
        } catch (InterruptedException e) {
            System.out.println("Join is interrupted");
        }
        if (cls.getState().toString().equals("TERMINATED")) {
            System.out.println(0);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0