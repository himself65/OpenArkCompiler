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
 *- @Title/Destination: interrupt a running thread.
 *- @Brief: see below
 * -#step1: 创建一个MyThread类的实例对象thread，并且MyThread类继承自Thread类；
 * -#step2: 调用thread的start()方法启动该线程；
 * -#step3: 让当前线程休眠1000ms；
 * -#step4: 调用thread的interrupt()方法中断线程；
 * -#step5: 在thread内部的run()执行完成后（内部逻辑是当isInterrupted()返回为false时，调用yield()方法，并分别打印
 *          isInterrupted()、interrupted()、isInterrupted()的值），isInterrupted()、interrupted()、isInterrupted()的值
 *          分别为true、true、false；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadTest.java
 *- @ExecuteClass: ThreadTest
 *- @ExecuteArgs:
 */

class MyThread extends Thread {
    public void run() {
        while (!isInterrupted()) {
            yield();
        }
        System.out.println(isInterrupted());
        System.out.println(interrupted());
        System.out.println(isInterrupted());
    }
}

public class ThreadTest {
    public static void main(String[] args) {
        Thread thread = new MyThread();
        thread.start();

        try {
            Thread.sleep(1000);
            thread.interrupt();
        } catch (Exception e) {
            System.out.println("catch");
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full true\ntrue\nfalse\n