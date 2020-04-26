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
 * -@TestCaseID: ThreadHoldsLock3
 *- @TestCaseName: Thread_ThreadHoldsLock3.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: holdsLock(ob1) throws NullPointerException when object is null.
 *- @Brief: see below
 * -#step1: 创建两个ThreadHoldsLock3类的实例对象threadHoldsLock31、threadHoldsLock32；
 * -#step2: 调用threadHoldsLock31的start()方法启动该线程；
 * -#step3: 让该线程休眠200ms；
 * -#step4: 调用threadHoldsLock32的start()方法启动该线程；
 * -#step5: 以object1=null为参数，尝试执行holdsLock()方法；
 * -#step6: 确定step5中会抛出NullPointerException；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadHoldsLock3.java
 *- @ExecuteClass: ThreadHoldsLock3
 *- @ExecuteArgs:
 */

public class ThreadHoldsLock3 extends Thread {
    static Object object = "aa";
    static Object object1 = null;

    public static void main(String[] args) {
        ThreadHoldsLock3 threadHoldsLock31 = new ThreadHoldsLock3();
        ThreadHoldsLock3 threadHoldsLock32 = new ThreadHoldsLock3();
        threadHoldsLock31.start();
        try {
            sleep(200);
        } catch (InterruptedException e1) {
            System.out.println("Sleep is interrupted");
        }
        threadHoldsLock32.start();
        try {
            holdsLock(object1);
            System.out.println(2);
        } catch (NullPointerException e) {
            System.out.println(0);
        }
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