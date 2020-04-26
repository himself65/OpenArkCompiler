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
 *- @Title/Destination: use volatile to control when a thread should stop running.
 *- @Brief: see below
 * -#step1: 创建一个ThreadRunning类的实例对象threadRunning，且ThreadRunning类继承自Thread类；
 * -#step2: 调用threadRunning的start()方法启动该线程；
 * -#step3: 调用threadRunning的isAlive()方法得其返回值为true，得知线程threadRunning是活跃着的；
 * -#step4: 将类变量threadRunning.stopWork赋值为true；
 * -#step5: 调用threadRunning的join()方法；
 * -#step6: 调用threadRunning的isAlive()方法得其返回值为false，得知线程threadRunning已经不活跃；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadTest.java
 *- @ExecuteClass: ThreadTest
 *- @ExecuteArgs:
 */
public class ThreadTest {
    public static void main(String[] args) throws Exception {
        //Try to make a running thread daemon, test SetDaemonLiveThread

        ThreadRunning threadRunning = new ThreadRunning();
        threadRunning.start();
        System.out.println(threadRunning.isAlive());
        threadRunning.stopWork = true;
        threadRunning.join();
        System.out.println(threadRunning.isAlive());
    }

    static class ThreadRunning extends Thread {
        public volatile int i = 0;
        volatile boolean stopWork = false;

        ThreadRunning() {
            super();
        }

        public void run() {
            while (!stopWork) {
                i++;
                i--;
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan true\s*false