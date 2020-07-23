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
 * -@TestCaseID: ThreadHoldsLock1
 *- @TestCaseName: Thread_ThreadHoldsLock1.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Thread.holdsLock(Object) returns true if and only if the current thread holds the monitor lock
 *                      on the specified object. before and after the wait, holdsLock return true.
 *- @Brief: see below
 * -#step1: 创建两个ThreadHoldsLock1类的实例对象threadHoldsLock1、threadHoldsLock2；
 * -#step2: 调用threadHoldsLock1的start()方法启动该线程；
 * -#step3: 让该线程休眠200ms；
 * -#step4: 调用threadHoldsLock2的start()方法启动该线程；
 * -#step5: 经判断得知，在经过ThreadHoldsLock1类内部的run()方法执行后int类型的静态变量都由0变为1；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadHoldsLock1.java
 *- @ExecuteClass: ThreadHoldsLock1
 *- @ExecuteArgs:
 */

public class ThreadHoldsLock1 extends Thread {
    static Object object = "aa";
    static int i = 0;
    static int t = 0;

    public static void main(String[] args) {
        ThreadHoldsLock1 threadHoldsLock1 = new ThreadHoldsLock1();
        ThreadHoldsLock1 threadHoldsLock2 = new ThreadHoldsLock1();
        threadHoldsLock1.start();
        try {
            sleep(200);
        } catch (InterruptedException e1) {
            System.out.println("Sleep is interrupted");
        }
        threadHoldsLock2.start();
        if (i == 1 && t == 1) {
            System.out.println(0);
        } else {
            System.out.println(2);
        }
    }

    public void run() {
        synchronized (object) {
            try {
                if (holdsLock(object)) {
                    t = 1;
                }
                object.wait(100);
            } catch (InterruptedException e1) {
                System.out.println("Wait is interrupted");
            }
            if (holdsLock(object)) {
                i = 1;
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n