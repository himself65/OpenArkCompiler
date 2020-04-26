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
 * -@TestCaseID: ThreadIsDaemon1
 *- @TestCaseName: Thread_ThreadIsDaemon1.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: To set a child thread as a daemon thread
 *- @Brief: see below
 * -#step1: 创建ThreadIsDaemon1类的实例对象threadIsDaemon1，并且ThreadIsDaemon1类继承自Thread类；
 * -#step2: 通过threadIsDaemon1的setDaemon()方法设置其属性为true；
 * -#step3: 调用threadIsDaemon1的start()方法启动该线程；
 * -#step4: 经判断得知调用threadIsDaemon1的isDaemon()方法其返回值为true；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadIsDaemon1.java
 *- @ExecuteClass: ThreadIsDaemon1
 *- @ExecuteArgs:
 */

public class ThreadIsDaemon1 extends Thread {
    public static void main(String[] args) {
        ThreadIsDaemon1 threadIsDaemon1 = new ThreadIsDaemon1();
        threadIsDaemon1.setDaemon(true);
        threadIsDaemon1.start();
        if (threadIsDaemon1.isDaemon()) {
            System.out.println(0);
        } else {
            System.out.println(2);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0