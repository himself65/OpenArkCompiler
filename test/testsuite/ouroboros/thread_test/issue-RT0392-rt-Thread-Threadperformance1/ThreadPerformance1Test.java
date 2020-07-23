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
 * -@TestCaseID: ThreadPerformance1Test
 *- @TestCaseName: Thread_ThreadPerformance1Test.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Gets the average time to perform 20 group reentrant locks/unlock 100 times of monitor.
 *- @Brief: see below
 * -#step1: 创建个100长度的线程组，线程里有6个同步锁，第5和6个解锁后各等待10毫秒，调用notifyAll()进行解锁。
 * -#step2：打点开始时间startTime,并发启动线程组的线程进行解锁和关锁的操作。
 * -#step3：调用join()等待所有线程结束，打点结束时间endTime，endTime - startTime算出每次运行时间。
 * -#step4：重复步骤1~3 20次，计算每次的平均时间。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadPerformance1Test.java
 *- @ExecuteClass: ThreadPerformance1Test
 *- @ExecuteArgs:
 */

class LockCls {}

public class ThreadPerformance1Test extends  Thread{
    public static void main(String[] args) {
        LockCls lock = new LockCls();
        long startTime;
        long endTime;
        long[] ThreadPerformance1Test = new long[20];
        long sum = 0;
        long ave;
        Object obj = "aa";
        Thread[] tt = new Thread[100];
        for (int ii = 0; ii < ThreadPerformance1Test.length; ii++) {
            for (int i = 0; i < tt.length; i++) {
                tt[i] = new Thread(new Runnable() {
                    public void run() {
                        synchronized (lock) {
                            synchronized (lock) {
                                synchronized (obj) {
                                    synchronized (this) {
                                        synchronized (obj) {
                                            synchronized (this) {
                                                try {
                                                    this.wait(10);
                                                } catch (InterruptedException e1) {
                                                    System.err.println(e1);
                                                }
                                            }
                                            try {
                                                obj.wait(10);
                                            } catch (InterruptedException e2) {
                                                System.err.println(e2);
                                            }
                                        }
                                        this.notifyAll();
                                    }
                                    obj.notify();
                                }
                                try {
                                    lock.wait(10);
                                } catch (InterruptedException e3) {
                                    System.err.println(e3);
                                }
                            }
                            lock.notifyAll();
                        }
                    }
                });
            }
            startTime = System.currentTimeMillis();
            for (int i = 0; i < tt.length; i++) {
                tt[i].start();
            }
            for (int i = 0; i < tt.length; i++) {
                try {
                    tt[i].join();
                } catch (InterruptedException e) {
                    System.err.println(e);
                }
            }
            endTime = System.currentTimeMillis();
            //System.out.println(startTime);
            //System.out.println(endTime);
            //System.out.println(endTime - startTime);
            ThreadPerformance1Test[ii] = endTime - startTime;
        }
        for(int ii = 0; ii < ThreadPerformance1Test.length; ii++) {
            sum += ThreadPerformance1Test[ii];
        }
        ave = sum / ThreadPerformance1Test.length;
        //System.out.println("ave=" + ave + "ms");
        System.out.println(0);
        return;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n