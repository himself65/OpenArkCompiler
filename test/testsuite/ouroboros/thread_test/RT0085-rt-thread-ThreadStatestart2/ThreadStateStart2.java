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
 * -@TestCaseID: ThreadStateStart2
 *- @TestCaseName: Thread_ThreadStateStart2.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: start a thread already started throws IllegalThreadStateException
 *- @Brief: see below
 * -#step1: 创建一个ThreadStateStart2类的实例对象threadStateStart2，并且ThreadStateStart2类继承自Thread类；
 * -#step2: 调用threadStateStart2的start()方法开启此线程；
 * -#step3: 重复调用threadStateStart2的start()方法；
 * -#step4: step3运行失败，会抛出IllegalThreadStateException；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadStateStart2.java
 *- @ExecuteClass: ThreadStateStart2
 *- @ExecuteArgs:
 */

public class ThreadStateStart2 extends Thread {
    public static void main(String[] args) {
        ThreadStateStart2 threadStateStart2 = new ThreadStateStart2();
        threadStateStart2.start();
        try {
            threadStateStart2.start();
        } catch (IllegalThreadStateException e1) {
            System.out.println(0);
        }
    }

    public void run() {
        try {
            sleep(50);
        } catch (InterruptedException e) {
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0