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
 * -@TestCaseID: ThreadIsAlive3
 *- @TestCaseName: Thread_ThreadIsAlive3.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Test isAlive() return false when a thread died.
 *- @Brief: see below
 * -#step1: 创建一个ThreadIsAlive3类的实例对象threadIsAlive3，并且ThreadIsAlive3类继承自Thread类；
 * -#step2: 调用threadIsAlive3的start()方法启动该线程；
 * -#step3: 调用threadIsAlive3的join()方法；
 * -#step4: 经判断得知调用threadIsAlive3的isAlive()方法其返回值为false；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadIsAlive3.java
 *- @ExecuteClass: ThreadIsAlive3
 *- @ExecuteArgs:
 */

public class ThreadIsAlive3 extends Thread {
    public static void main(String[] args) {
        ThreadIsAlive3 threadIsAlive3 = new ThreadIsAlive3();
        threadIsAlive3.start();
        try {
            threadIsAlive3.join();
        } catch (InterruptedException e) {
            System.out.println("Join is interrupted");
        }
        if (threadIsAlive3.isAlive()) {
            System.out.println(2);
            return;
        }
        System.out.println(0);
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0