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
 * -@TestCaseID: ThreadStateSleep5
 *- @TestCaseName: Thread_ThreadStateSleep5.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Sleep negative second grade time will throw IllegalArgumentException.
 *- @Brief: see below
 * -#step1: 创建一个ThreadStateSleep5类的实例对象threadStateSleep5，并且ThreadStateSleep5类继承自Thread类；
 * -#step2: 调用threadStateSleep5的start()方法启动该线程；
 * -#step3: 尝试执行sleep()方法，参数为-10；
 * -#step4: step3运行失败，会抛出IllegalArgumentException；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadStateSleep5.java
 *- @ExecuteClass: ThreadStateSleep5
 *- @ExecuteArgs:
 */

public class ThreadStateSleep5 extends Thread {
    public static void main(String[] args) {
        ThreadStateSleep5 threadStateSleep5 = new ThreadStateSleep5();
        threadStateSleep5.start();
        try {
            sleep(-10);
        } catch (InterruptedException e) {
            System.out.println(2);
        } catch (IllegalArgumentException e1) {
            System.out.println(0);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n