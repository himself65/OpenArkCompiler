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
 * -@TestCaseID: ThreadStateStart1
 *- @TestCaseName: Thread_ThreadStateStart1.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Start thread
 *- @Brief: see below
 * -#step1: 创建一个ThreadStateStart1类的实例对象threadStateStart1，并且ThreadStateStart1类继承自Thread类；
 * -#step2: 调用threadStateStart1的start()方法开启此线程；
 * -#step3: 令int类型的静态全局变量i的值加1；
 * -#step5: 在ThreadStateStart1类的内部的run()方法执行完之后，经判断得知，线程threadStateStart1的状态是"RUNNABLE"的，并
 *          且i的值由0变为1；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadStateStart1.java
 *- @ExecuteClass: ThreadStateStart1
 *- @ExecuteArgs:
 */

public class ThreadStateStart1 extends Thread {
    static int i = 0;

    public static void main(String[] args) {
        ThreadStateStart1 threadStateStart1 = new ThreadStateStart1();
        threadStateStart1.start();
        i++;
        if (threadStateStart1.getState().toString().equals("RUNNABLE") && i == 1) {
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
// ASSERT: scan-full 0\n