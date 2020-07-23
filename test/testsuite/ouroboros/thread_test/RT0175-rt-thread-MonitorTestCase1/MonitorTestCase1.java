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
 * -@TestCaseID: MonitorTestCase1.java
 *- @TestCaseName: Thread_MonitorTestCase1.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Added according to stress cases SyscompTestCase/Maple_harness/tools/testapp_MemStressTest/src
 *                      /com/huawei/stress.
 *- @Brief: see below
 * -#step1: 创建一个static的类StopAbleThread继承Thread，类的构造方法使用了父函数的构造方法。
 * -#step2: 分配INIT_DEFAULT_THREAD_NUM / 2个对象来自于new StopAbleThread类，同时启动监控线程。
 * -#step3: 调用start()启动所有的对象线程。
 * -#step4: 调用join()等待线程运行结束。
 * -#step5: 调用Runtime.getRuntime().gc()回收资源。
 * -#step6: 重复步骤1~4。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: MonitorTestCase1.java
 *- @ExecuteClass: MonitorTestCase1
 *- @ExecuteArgs:
 */

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

public class MonitorTestCase1 {
    private static final int INIT_DEFAULT_THREAD_NUM = 100;
    private static int DEFAULT_THREAD_NUM_HALF = INIT_DEFAULT_THREAD_NUM / 2;
    private static final int THREAD_SLEEP_TIME_DEFAULT = 1;
    private static final String MODULE_NAME_MONITOR = "M";
    private static final int THREAD_REPEATS_INFINITE = -1;
    private static final int THREAD_REPEATS_DEFAULT = 1;
    private static boolean mRunning = true;
    private static boolean mRest = false;
    private static int mRestTime = 5000;
    static List<Thread> mAllThread = new ArrayList<>();

    public static void main(String[] args) {
        testCase1();
        Runtime.getRuntime().gc();
        testCase1();
        System.out.println(0);
    }

    public static void testCase1() {
        ArrayList<Thread> list = new ArrayList<>();
        Thread thread;
        Object lockObject = new Object();
        for (int i = 0; i < DEFAULT_THREAD_NUM_HALF; i++) {
            thread = new StopAbleThread(new MonitorTest(lockObject, THREAD_SLEEP_TIME_DEFAULT, THREAD_REPEATS_DEFAULT),
                    MODULE_NAME_MONITOR + "_testCase1_" + i);
            list.add(thread);
        }
        startAllThread(list);
        waitAllThreadFinish(list);
    }

    public static void waitAllThreadFinish(List<Thread> list) {
        for (Thread s : list) {
            try {
                s.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            } finally {
                removeDeadThread(s);
            }
        }
    }

    public static void removeDeadThread(Thread t) {
        if (t == null) {
            return;
        }
        if (!t.isAlive()) {
            synchronized (mAllThread) {
                mAllThread.remove(t);
            }
        }
    }

    public static void startAllThread(List<Thread> list) {
        for (Thread s : list) {
            s.start();
            trySleep(new Random().nextInt(2));
        }
    }

    public static void trySleep(long time) {
        try {
            Thread.sleep(time);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    static class MonitorTest implements Runnable {
        Object lockObject;
        int sleepTime;
        int repeats;

        public MonitorTest(Object lockObject, int sleepTime, int repeatTimes) {
            this.lockObject = lockObject;
            this.sleepTime = sleepTime;
            repeats = repeatTimes;
        }

        @Override
        public void run() {
            while (THREAD_REPEATS_INFINITE == repeats || repeats > 0) {
                if (repeats > 0) {
                    repeats--;
                }
                synchronized (lockObject) {
                    lockObject.toString();
                    tryRest();
                    trySleep(sleepTime);
                    if (!mRunning) {
                        break;
                    }
                }
            }
        }
    }

    private static boolean tryRest() {
        if (mRest) {
            trySleep(mRestTime);
            return true;
        }
        return false;
    }

    static class StopAbleThread extends Thread {
        public StopAbleThread(Runnable r, String name) {
            super(r, name);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n