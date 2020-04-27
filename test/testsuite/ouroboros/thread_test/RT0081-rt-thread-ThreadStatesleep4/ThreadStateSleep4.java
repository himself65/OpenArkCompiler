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
 * -@TestCaseID: ThreadStateSleep4
 *- @TestCaseName: Thread_ThreadStateSleep4.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Verify the accuracy of sleep time.
 *- @Brief: see below
 * -#step1: 创建一个ThreadStateSleep4类的实例对象threadStateSleep4，并且ThreadStateSleep4类继承自Thread类；
 * -#step2: 调用threadStateSleep4的start()方法启动该线程；
 * -#step3: 调用threadStateSleep4的join()方法；
 * -#step4: 在ThreadStateSleep4类的内部的run()方法执行后，经判断得知，在让当前线程休眠1000ms之后的当前系统时间end与休眠
 *          之前的当前系统时间start的差值大于970且小于1030；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadStateSleep4.java
 *- @ExecuteClass: ThreadStateSleep4
 *- @ExecuteArgs:
 */

public class ThreadStateSleep4 extends Thread {
    static Object object = "aa";
    static long start = 0;
    static long end = 0;

    public static void main(String[] args) {
        ThreadStateSleep4 threadStateSleep4 = new ThreadStateSleep4();
        threadStateSleep4.start();

        try {
            threadStateSleep4.join();
        } catch (InterruptedException ex) {
            System.out.println(ex);
        }

        if (end - start > 970 && end - start < 1030) {
            System.out.println(0);
        } else {
            System.out.println(2);
        }
    }

    public void run() {
        synchronized (object) {
            try {
                start = System.currentTimeMillis();
                sleep(1000);
                end = System.currentTimeMillis();
            } catch (InterruptedException e) {
                System.out.println(e);
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n