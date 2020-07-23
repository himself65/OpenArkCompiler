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
 * -@TestCaseID: ThreadStateWait3
 *- @TestCaseName: Thread_ThreadStateWait3.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Thread state when the target thread is waiting for the millisecond-level time
 *- @Brief: see below
 * -#step1: 创建两个ThreadStateWait3类的实例对象threadStateWait31、threadStateWait32，并且ThreadStateWait3类继承自
 *          Thread类；
 * -#step2: 分别调用threadStateWait31、threadStateWait32的start()方法启动这两个线程；
 * -#step3: 让当前线程休眠100ms；
 * -#step4: 在ThreadStateWait3类内部的run()方法执行完之后（run()方法内会以object为对象锁，并在执行完对i的值加1后，会调用
 *          object的wait()方法,参数为1000、500），经判断得知，线程threadStateWait21、threadStateWait22的状态均是
 *          "TIMED_WAITING"的，并且int类型的静态全局变量i的值由0变为2，t的值始终为0；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadStateWait3.java
 *- @ExecuteClass: ThreadStateWait3
 *- @ExecuteArgs:
 */

public class ThreadStateWait3 extends Thread {
    static Object object = "aa";
    static int i = 0;
    static int t = 0;

    public static void main(String[] args) {
        ThreadStateWait3 threadStateWait31 = new ThreadStateWait3();
        ThreadStateWait3 threadStateWait32 = new ThreadStateWait3();
        threadStateWait31.start();
        threadStateWait32.start();
        try {
            sleep(100);
        } catch (InterruptedException e1) {
        }
        if (threadStateWait31.getState().toString().equals("TIMED_WAITING")
                && threadStateWait32.getState().toString().equals("TIMED_WAITING") && i == 2 && t == 0) {
            System.out.println(0);
        }
    }

    public void run() {
        synchronized (object) {
            try {
                i++;
                object.wait(1000, 500);
                t++;
            } catch (InterruptedException e) {
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n