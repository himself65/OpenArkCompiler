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
 *- @Title/Destination: Try to make a running thread daemon, test SetDaemonLiveThread.
 *- @Brief: see below
 * -#step1：定义Thread的子类ThreadRunning，调用super()引用父类的成员，定义run()方法。
 * -#step2：通过new得到ThreadRunning的对象。启动对象线程。指定join()的时间。
 * -#step3：线程结束后输出执行结果。
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadTest.java
 *- @ExecuteClass: ThreadTest
 *- @ExecuteArgs:
 */
public class ThreadTest {
    public static void main(String[] args) throws Exception {
        // Try to make a running thread daemon, test SetDaemonLiveThread
        ThreadRunning t = new ThreadRunning();
        t.start();
        t.join(1000);
        System.out.println("pass");
        t.stopWork = true;
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
            System.out.println("exit");
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan pass\s*exit