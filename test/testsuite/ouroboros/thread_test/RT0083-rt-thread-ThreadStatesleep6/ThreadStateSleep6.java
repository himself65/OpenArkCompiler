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
 * -@TestCaseID: ThreadStateSleep6
 *- @TestCaseName: Thread_ThreadStateSleep6.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Sleep negative millisecond time will throw IllegalArgumentException.
 *- @Brief: see below
 * -#step1: 创建一个ThreadStateSleep6类的实例对象threadStateSleep6，并且ThreadStateSleep6类继承自Thread类；
 * -#step2: 调用threadStateSleep6的start()方法启动该线程；
 * -#step3: 尝试执行sleep()方法，参数为1000、-100；
 * -#step4: step3运行失败，会抛出IllegalArgumentException；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadStateSleep6.java
 *- @ExecuteClass: ThreadStateSleep6
 *- @ExecuteArgs:
 */

public class ThreadStateSleep6 extends Thread {
    public static void main(String[] args) {
        ThreadStateSleep6 threadStateSleep6 = new ThreadStateSleep6();
        threadStateSleep6.start();
        try {
            sleep(1000, -100);
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