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
 * -@TestCaseID: ThreadIsDaemon2
 *- @TestCaseName: Thread_ThreadIsDaemon2.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Setting child threads to daemon threads has no effect on the main thread.
 *- @Brief: see below
 * -#step1: 创建ThreadIsDaemon2类的实例对象threadIsDaemon2，并且ThreadIsDaemon2类继承自Thread类；
 * -#step2: 通过threadIsDaemon2的setDaemon()方法设置其属性为true；
 * -#step3: 调用threadIsDaemon2的start()方法启动该线程；
 * -#step4: 经判断得知调用currentThread()的isDaemon()方法其返回值为false；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadIsDaemon2.java
 *- @ExecuteClass: ThreadIsDaemon2
 *- @ExecuteArgs:
 */

public class ThreadIsDaemon2 extends Thread {
    public static void main(String[] args) {
        ThreadIsDaemon2 threadIsDaemon2 = new ThreadIsDaemon2();
        threadIsDaemon2.setDaemon(true);
        threadIsDaemon2.start();
        if (currentThread().isDaemon()) {
            System.out.println(2);
            return;
        }
        System.out.println(0);
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n