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
 * -@TestCaseID: ThreadIsAlive2
 *- @TestCaseName: Thread_ThreadIsAlive2.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: isAlive() return false when a thread is running
 *- @Brief: see below
 * -#step1: 创建一个ThreadIsAlive2类的实例对象threadIsAlive2，并且ThreadIsAlive2类继承自Thread类；
 * -#step2: 调用threadIsAlive2的start()方法启动该线程；
 * -#step3: 经判断得知调用threadIsAlive2的isAlive()方法其返回值为true；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadIsAlive2.java
 *- @ExecuteClass: ThreadIsAlive2
 *- @ExecuteArgs:
 */

public class ThreadIsAlive2 extends Thread {
    public static void main(String[] args) {
        ThreadIsAlive2 threadIsAlive2 = new ThreadIsAlive2();
        threadIsAlive2.start();
        if (!threadIsAlive2.isAlive()) {
            System.out.println(2);
            return;
        }
        System.out.println(0);
    }

    public void run() {
        try {
            sleep(50);
        } catch (InterruptedException e) {
            System.out.println("Sleep is interrupted");
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n