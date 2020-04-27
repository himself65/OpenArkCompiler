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
 * -@TestCaseID: ThreadStateSleep3
 *- @TestCaseName: Thread_ThreadStateSleep3.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Sleep millisecond Time
 *- @Brief: see below
 * -#step1: 创建一个ThreadStateSleep3类的实例对象threadStateSleep3，并且ThreadStateSleep3类继承自Thread类；
 * -#step2: 调用threadStateSleep3的start()方法启动该线程；
 * -#step3: 执行sleep()方法让当前线程休眠，参数为100、100；
 * -#step4: 经判断得知线程threadStateSleep3的状态为"TERMINATED"的；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadStateSleep3.java
 *- @ExecuteClass: ThreadStateSleep3
 *- @ExecuteArgs:
 */

public class ThreadStateSleep3 extends Thread {
    public static void main(String[] args) {
        ThreadStateSleep3 threadStateSleep3 = new ThreadStateSleep3();
        threadStateSleep3.start();
        try {
            sleep(100, 100);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        if (threadStateSleep3.getState().toString().equals("TERMINATED")) {
            System.out.println(0);
            return;
        }
        System.out.println(2);
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n