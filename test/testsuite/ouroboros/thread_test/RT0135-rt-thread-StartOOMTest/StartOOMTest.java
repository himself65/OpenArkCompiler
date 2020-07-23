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
 * -@TestCaseID: StartOOMTest
 *- @TestCaseName: Thread_StartOOMTest.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: OutOfMemoryError is thrown when there are too many thread created.
 *- @Brief: see below
 * -#step1: 定义类SleepRunnable实现Runnable接口，定义run()方法，通过sleep()指定休眠时间。
 * -#step2: 通过new得到类SleepRunnable的对象、线程组对象、线程列表对象以及一个为null的failedThread。
 * -#step3: 在for()循环中，new得到100个设置了参数的Thread类对象，当跑到第100个是，throw OutOfMemoryError。
 * -#step4: Thread类对象循环调用interrupt()。启动failedThread线程对象，调用interrupt()。Thread类对象循环调用join()，
 *          failedThread对象赋值为最后一个新增的Thread类对象。
 * -#step5: 对前100个线程调用interrupt()进行中断，当活跃线程数大于50时，调用Thread.yield()。
 * -#step6: 等待所有线程运行结束，确认激活的线程组对象数量为0。
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: StartOOMTest.java
 *- @ExecuteClass: StartOOMTest
 *- @ExecuteArgs:
 */

import java.util.ArrayList;
import java.util.List;

public class StartOOMTest {
    public static void main(String[] args) throws Throwable {
        Runnable r = new SleepRunnable();
        ThreadGroup tg = new ThreadGroup("buggy");
        List<Thread> threads = new ArrayList<Thread>();
        Thread failedThread = null;
        int i = 0;
        for (i = 0; i < 101; i++) {
            Thread t = new Thread(tg, r);
            try {
                if (i == 100) {
                    throw new OutOfMemoryError("throw OutOfMemoryError");
                }
                t.start();
                threads.add(t);
            } catch (Throwable x) {
                failedThread = t;
                System.out.println(x);
                System.out.println(i);
                break;
            }
        }

        int j = 0;
        for (Thread t : threads) {
            t.interrupt();
        }

        while (tg.activeCount() > i / 2) {
            Thread.yield();
        }
        failedThread.start();
        failedThread.interrupt();

        for (Thread t : threads) {
            t.join();
        }
        failedThread.join();

        try {
            Thread.sleep(1000);
        } catch (Throwable ignore) {
            System.out.println("Sleep is interrupted");
        }

        int activeCount = tg.activeCount();
        System.out.println("activeCount = " + activeCount);

        if (activeCount > 0) {
            throw new RuntimeException("Failed: there  should be no active Threads in the group");
        }
    }

    static class SleepRunnable implements Runnable {
        public void run() {
            try {
                Thread.sleep(60 * 1000);
            } catch (Throwable t) {
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full java.lang.OutOfMemoryError: throw OutOfMemoryError\n100\nactiveCount = 0\n