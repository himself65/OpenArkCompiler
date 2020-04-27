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
 * -@TestCaseID: ThreadHoldsLock2
 *- @TestCaseName: Thread_ThreadHoldsLock2.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: only one of the threads holdsLock on the Object if they synchronized on the same object.
 *- @Brief: see below
 * -#step1: 创建两个ThreadHoldsLock2类的实例对象threadHoldsLock21、threadHoldsLock22；
 * -#step2: 调用threadHoldsLock21的start()方法启动该线程；
 * -#step3: 让该线程休眠200ms；
 * -#step4: 调用threadHoldsLock22的start()方法启动该线程；
 * -#step5: 经判断得知，在经过ThreadHoldsLock2类内部的run()方法执行后holdsLock(object)的返回值为false；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadHoldsLock2.java
 *- @ExecuteClass: ThreadHoldsLock2
 *- @ExecuteArgs:
 */

public class ThreadHoldsLock2 extends Thread {
    static Object object = "aa";

    public static void main(String[] args) {
        ThreadHoldsLock2 threadHoldsLock21 = new ThreadHoldsLock2();
        ThreadHoldsLock2 threadHoldsLock22 = new ThreadHoldsLock2();
        threadHoldsLock21.start();
        try {
            sleep(200);
        } catch (InterruptedException e1) {
            System.out.println("Sleep is interrupted");
        }
        threadHoldsLock22.start();
        if ((holdsLock(object))) {
            System.out.println(2);
            return;
        }
        System.out.println(0);
    }

    public void run() {
        synchronized (object) {
            try {
                object.wait(100);
            } catch (InterruptedException e1) {
                System.out.println("Wait is interrupted");
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0