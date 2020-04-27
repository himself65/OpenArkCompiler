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
 * -@TestCaseID: ThreadStateSleep1
 *- @TestCaseName: Thread_ThreadStateSleep1.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Primary thread sleep second-level time, target thread has ended.
 *- @Brief: see below
 * -#step1: 创建四个ThreadStateSleep1类的实例对象threadStateSleep1、threadStateSleep2、threadStateSleep3、
 *          threadStateSleep4，并且ThreadStateSleep1类继承自Thread类；
 * -#step2: 分别调用threadStateSleep1、threadStateSleep2的start()方法启动这两个线程；
 * -#step3: 让当前线程休眠1000ms；
 * -#step4: 分别调用threadStateSleep3、threadStateSleep4的start()方法启动这两个线程；
 * -#step5: 经判断得知，线程threadStateSleep1的状态是"TERMINATED"的；另外线程threadStateSleep4的状态
 *          要么是"RUNNABLE"的，，要么是"TERMINATED"的，并且只能是两种情况中的一种；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadStateSleep1.java
 *- @ExecuteClass: ThreadStateSleep1
 *- @ExecuteArgs:
 */

public class ThreadStateSleep1 extends Thread {
    public static void main(String[] args) {
        ThreadStateSleep1 threadStateSleep1 = new ThreadStateSleep1();
        ThreadStateSleep1 threadStateSleep2 = new ThreadStateSleep1();
        ThreadStateSleep1 threadStateSleep3 = new ThreadStateSleep1();
        ThreadStateSleep1 threadStateSleep4 = new ThreadStateSleep1();
        threadStateSleep1.start();
        threadStateSleep2.start();
        try {
            sleep(1000);
        } catch (Exception e) {
            e.printStackTrace();
        }
        threadStateSleep3.start();
        threadStateSleep4.start();
        if (threadStateSleep1.getState().toString().equals("TERMINATED")
                && threadStateSleep4.getState().toString().equals("RUNNABLE")) {
            System.out.println(0);
            return;
        }
        if (threadStateSleep1.getState().toString().equals("TERMINATED")
                && threadStateSleep4.getState().toString().equals("TERMINATED")) {
            System.out.println(0);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n