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
 * -@TestCaseID: ThreadSetDaemon2
 *- @TestCaseName: Thread_ThreadSetDaemon2.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: If this thread is alive, thread.setDaemon() throws IllegalThreadStateException.
 *- @Brief: see below
 * -#step1: 创建一个ThreadSetDaemon2类的实例对象threadSetDaemon2，并且ThreadSetDaemon2类继承自Thread类；
 * -#step2: 调用threadSetDaemon2的start()方法启动该线程；
 * -#step3: 尝试通过threadSetDaemon2的setDaemon()方法为其设置属性为true；
 * -#step4: 确定step3中会抛出IllegalThreadStateException；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadSetDaemon2.java
 *- @ExecuteClass: ThreadSetDaemon2
 *- @ExecuteArgs:
 */

public class ThreadSetDaemon2 extends Thread {
    public static void main(String[] args) {
        ThreadSetDaemon2 threadSetDaemon2 = new ThreadSetDaemon2();
        threadSetDaemon2.start();
        try {
            threadSetDaemon2.setDaemon(true);
            System.out.println(2);
        } catch (IllegalThreadStateException e2) {
            System.out.println(0);
        }
    }

    public void run() {
        for (int i = 0; i < 5; i++) {
            try {
                sleep(200);
            } catch (InterruptedException e1) {
                System.out.println("Sleep is interrupted");
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n