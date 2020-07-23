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
 * -@TestCaseID: ThreadStateWait8
 *- @TestCaseName: Thread_ThreadStateWait8.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: InterruptedException is thrown if any thread interrupted the current thread before or while the
  *                     current thread was waiting for a notification.
 *- @Brief: see below
 * -#step1: 新建一个Thread类的数组tt，数组长度为10；
 * -#step2: 对于i < tt.length = 10，令tt[i] = new Thread()，其内重写的run()方法执行的是以object为对象锁，并尝试对object
 *          调用wait()方法，若出现异常，则int类型的静态全局变量k的值会加1；
 * -#step3: 对于i < tt.length = 10，执行线程tt[i]的start()方法启动该线程；
 * -#step4: 对于i < tt.length = 10，执行线程tt[i]的interrupt()方法；
 * -#step5: 对于i < tt.length = 10，尝试调用线程tt[i]的join()方法；
 * -#step6: step5成功执行，并且经判断得知，k的值最终等于10；

 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadStateWait8.java
 *- @ExecuteClass: ThreadStateWait8
 *- @ExecuteArgs:
 */

public class ThreadStateWait8 extends Thread {
    static int k = 0;

    public static void main(String[] args) {
        Object object = "aa";
        Thread[] tt = new Thread[10];
        for (int i = 0; i < tt.length; i++) {
            tt[i] = new Thread() {
                public void run() {
                    synchronized (object) {
                        try {
                            object.wait();
                        } catch (InterruptedException e1) {
                            k++;
                        }
                    }
                }
            };
        }
        for (int i = 0; i < tt.length; i++) {
            tt[i].start();
        }
        for (int i = 0; i < tt.length; i++) {
            tt[i].interrupt();
        }
        for (int i = 0; i < tt.length; i++) {
            try {
                tt[i].join();
            } catch (InterruptedException e2) {
                System.err.println(e2);
            }
        }
        if (k == 10) {
            System.out.println(0);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n