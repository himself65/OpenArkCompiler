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
 * -@TestCaseID: ThreadIsAlive1
 *- @TestCaseName: Thread_ThreadIsAlive1.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: isAlive() return false when a thread haven't been started yet
 *- @Brief: see below
 * -#step1: 创建一个ThreadIsAlive1类的实例对象threadIsAlive1，并且ThreadIsAlive1类继承自Thread类；
 * -#step2: 经判断得知调用threadIsAlive1的isAlive()方法其返回值为false；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadIsAlive1.java
 *- @ExecuteClass: ThreadIsAlive1
 *- @ExecuteArgs:
 */

public class ThreadIsAlive1 extends Thread {
    public static void main(String[] args) {
        ThreadIsAlive1 threadIsAlive1 = new ThreadIsAlive1();
        if (threadIsAlive1.isAlive()) {
            System.out.println(2);
            return;
        }
        System.out.println(0);
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n