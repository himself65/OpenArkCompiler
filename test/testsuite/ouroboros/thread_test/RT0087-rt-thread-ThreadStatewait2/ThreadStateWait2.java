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
 * -@TestCaseID: ThreadStateWait2
 *- @TestCaseName: Thread_ThreadStateWait2.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Thread state when the target thread is in an infinite wait
 *- @Brief: see below
 * -#step1: 创建两个ThreadStateWait2类的实例对象threadStateWait21、threadStateWait22，并且ThreadStateWait2类继承自
 *          Thread类；
 * -#step2: 分别调用threadStateWait21、threadStateWait22的start()方法启动这两个线程；
 * -#step3: 让当前线程休眠100ms；
 * -#step4: 在ThreadStateWait2类内部的run()方法执行完之后（run()方法内会以object为对象锁，并在执行完对i的值加1后，会调用
 *          object的wait()方法），经判断得知，线程threadStateWait21、threadStateWait22的状态均是"WAITING"的，并且int类型
 *          的静态全局变量i的值由0变为2，t的值始终为0；
 * -#step5: 分别调用threadStateWait21、threadStateWait22的interrupt()方法；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadStateWait2.java
 *- @ExecuteClass: ThreadStateWait2
 *- @ExecuteArgs:
 */

public class ThreadStateWait2 extends Thread {
    static Object object = "aa";
    static int i = 0;
    static int t = 0;

    public static void main(String[] args) {
        ThreadStateWait2 threadStateWait21 = new ThreadStateWait2();
        ThreadStateWait2 threadStateWait22 = new ThreadStateWait2();
        threadStateWait21.start();
        threadStateWait22.start();
        try {
            sleep(100);
        } catch (InterruptedException e1) {
        }
        if (threadStateWait21.getState().toString().equals("WAITING")
                && threadStateWait22.getState().toString().equals("WAITING") && i == 2 && t == 0) {
            threadStateWait21.interrupt();
            threadStateWait22.interrupt();
            System.out.println(0);
        }
    }

    public void run() {
        synchronized (object) {
            try {
                i++;
                object.wait();
                t++;
            } catch (InterruptedException e) {
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n