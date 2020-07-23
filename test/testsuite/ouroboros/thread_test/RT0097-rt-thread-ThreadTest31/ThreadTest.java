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
 *- @Title/Destination: test for call of static method interrupted(), and method interrupt()
 *- @Brief: see below
 * -#step1: 创建一个ThreadTest类的实例对象threadTest，并且ThreadTest类继承自Thread类；
 * -#step2: 调用threadTest的start()方法启动该线程；
 * -#step3: 调用threadTest的join()方法；
 * -#step4: 在ThreadTest类的内部的run()方法执行完之后（run()方法内的执行逻辑是打印相关信息，并使当前线程中断），程序可以
 *          正常终止；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadTest.java
 *- @ExecuteClass: ThreadTest
 *- @ExecuteArgs:
 */

public class ThreadTest extends Thread {
    public static void main(String[] args) throws Exception {
        ThreadTest threadTest = new ThreadTest();
        threadTest.start();
        threadTest.join();
        System.out.println("PASS");
    }

    //test for call of static method interrupted(), and method interrupt()
    // in java a static method can be called by className.staticMethod and by objectName.staticMethod
    public void run() {
        System.out.println("isInterrupted() returns -- " + isInterrupted());
        interrupt();
        System.out.println("after executing interrupt()");
        System.out.println("isInterrupted() returns -- " + isInterrupted());
        System.out.println("1st should true -- " + interrupted());
        System.out.println("isInterrupted() returns -- " + isInterrupted());
        System.out.println("2nd should false -- " + interrupted());
        System.out.println("isInterrupted() returns -- " + isInterrupted());
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full isInterrupted() returns -- false\nafter executing interrupt()\nisInterrupted() returns -- true\n1st should true -- true\nisInterrupted() returns -- false\n2nd should false -- false\nisInterrupted() returns -- false\nPASS\n