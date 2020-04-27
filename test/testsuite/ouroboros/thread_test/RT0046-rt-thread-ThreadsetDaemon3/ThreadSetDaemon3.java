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
 * -@TestCaseID: ThreadSetDaemon3
 *- @TestCaseName: Thread_ThreadSetDaemon3.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Verify that the main thread end daemon does not end directly when some other non-daemon thread
 *                      still running
 *- @Brief: see below
 * -#step1: 创建两个ThreadSetDaemon3类的实例对象threadSetDaemon31、threadSetDaemon32，并且ThreadSetDaemon3类继承自
 *          Thread类；
 * -#step2: 通过threadSetDaemon31的setDaemon()方法为其设置属性为true；
 * -#step3: 通过threadSetDaemon31的start()方法启动该线程；
 * -#step4: 让当前线程休眠50ms；
 * -#step5: 将int类型的全局静态变量t的值加1；
 * -#step6: 通过threadSetDaemon32的start()方法启动该线程；
 * -#step7: 让当前线程休眠50ms；
 * -#step8: 将int类型的全局静态变量t的值加1；
 * -#step9: 确定以上步骤以及ThreadSetDaemon3类在执行其内部的run()后均未抛出异常；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadSetDaemon3.java
 *- @ExecuteClass: ThreadSetDaemon3
 *- @ExecuteArgs:
 */

public class ThreadSetDaemon3 extends Thread {
    static int i = 0;
    static int t = 0;

    public static void main(String[] args) {
        ThreadSetDaemon3 threadSetDaemon31 = new ThreadSetDaemon3();
        ThreadSetDaemon3 threadSetDaemon32 = new ThreadSetDaemon3();
        threadSetDaemon31.setDaemon(true);
        threadSetDaemon31.start();
        try {
            sleep(50);
            t++;
        } catch (InterruptedException e3) {
            System.out.println("Sleep is interrupted");
        }
        threadSetDaemon32.start();
        try {
            sleep(50);
            t++;
        } catch (InterruptedException e2) {
            System.out.println("Sleep is interrupted");
        }
        System.out.println(0);
    }

    public void run() {
        try {
            sleep(300);
            i++;
        } catch (InterruptedException e1) {
            System.out.println("Sleep is interrupted");
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n