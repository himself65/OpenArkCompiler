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
 * -@TestCaseID: ThreadSetDaemon1
 *- @TestCaseName: Thread_ThreadSetDaemon1.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Verify that the main thread end daemon ends directly when the daemon exists.
 *- @Brief: see below
 * -#step1: 创建ThreadSetDaemon1类的实例对象threadSetDaemon1，并且ThreadSetDaemon1类继承自Thread类；
 * -#step2: 通过threadSetDaemon1的setDaemon()方法设置其属性为true；
 * -#step3: 调用threadSetDaemon1的start()方法启动该线程；
 * -#step4: 让当前线程休眠100ms；
 * -#step5: 令int类型的全局静态变量t的值加1；
 * -#step6: 确定step4、step5以及ThreadSetDaemon1类在执行其内部的run()后均未抛出异常；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadSetDaemon1.java
 *- @ExecuteClass: ThreadSetDaemon1
 *- @ExecuteArgs:
 */

public class ThreadSetDaemon1 extends Thread {
    static int i = 0;
    static int t = 0;

    public static void main(String[] args) {
        ThreadSetDaemon1 threadSetDaemon1 = new ThreadSetDaemon1();
        threadSetDaemon1.setDaemon(true);
        threadSetDaemon1.start();
        try {
            sleep(100);
            t++;
        } catch (InterruptedException e2) {
            System.out.println("Sleep is interrupted");
        }
        System.out.println(0);
    }

    public void run() {
        try {
            sleep(1000);
            i++;
            System.out.println(2);
        } catch (InterruptedException e1) {
            System.out.println("Sleep is interrupted");
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n