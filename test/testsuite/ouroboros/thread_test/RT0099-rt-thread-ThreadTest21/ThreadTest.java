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
 * -@TestCaseID: ThreadTest
 *- @TestCaseName: Thread_ThreadTest.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: test for interrupt(), interrupted(), isInterrupted()
 *- @Brief: see below
 * -#step1: 创建一个Thread类的实例对象thread，其内的run()方法实现是打印相关信息并调用interrupt()方法中断线程；
 * -#step2: 调用thread的start()方法启动该线程；
 * -#step3: 调用thread的join()方法；
 * -#step4: 确定程序可以正常终止；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadTest.java
 *- @ExecuteClass: ThreadTest
 *- @ExecuteArgs:
 */
public class ThreadTest {
    // test for interrupt(), interrupted(), isInterrupted()
    public static void main(String[] args) throws Exception {
        Thread thread = new Thread() {
            public void run() {
                System.out.println("helloworld executes interrupt()");
                interrupt();
                System.out.println("isInterrupted() returns -- " + isInterrupted());
                System.out.println("1st interrupted() should true -- " + Thread.interrupted());
                System.out.println("isInterrupted() returns -- " + isInterrupted());
                System.out.println("2nd interrupted() should false -- " + Thread.interrupted());
                System.out.println("isInterrupted() returns -- " + isInterrupted());
            }
        };
        thread.start();
        thread.join();
        System.out.println("PASS");
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full helloworld executes interrupt()\nisInterrupted() returns -- true\n1st interrupted() should true -- true\nisInterrupted() returns -- false\n2nd interrupted() should false -- false\nisInterrupted() returns -- false\nPASS\n