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
 * -@TestCaseID:rc/stress/Memory_stressTestCase29.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination:
 *- @Brief:functionTest
 *- @Expect:
 *- @Priority: High
 *- @Source: Memory_stressTestCase29.java
 *- @ExecuteClass: Memory_stressTestCase29
 *- @ExecuteArgs:
 */


import java.util.ArrayList;
import java.util.List;
import java.util.Random;


import sun.misc.Cleaner;

public class Memory_stressTestCase29 {
    private static final int INIT_DEFAULT_THREAD_NUM = 100;
    private static int DEFAULT_THREAD_NUM = INIT_DEFAULT_THREAD_NUM;
    static List<List<AllocUnit>> mAllThread = new ArrayList<List<AllocUnit>>();

    public static void main(String[] args) {
        testCase29();
        Runtime.getRuntime().gc();
        testCase29();
        Runtime.getRuntime().gc();
        testCase29();
        Runtime.getRuntime().gc();
        testCase29();
        Runtime.getRuntime().gc();
        testCase29();
        System.out.println("ExpectResult");
    }

    public static void testCase29() {
        ArrayList<Thread> list = new ArrayList<>();
        for (int i = 0; i < (DEFAULT_THREAD_NUM / 10); i++) {
            list.add(new StopAbleThread(() -> {
                ArrayList<Memory_stressTestCase29.AllocUnitwithVerifyCleaner> units = new ArrayList<>();
                Random r = new Random();
                for (int j = 0; j < 100; j++) {
                    units.add(new Memory_stressTestCase29.AllocUnitwithVerifyCleaner(r.nextInt(512)));
                }
                trySleep(r.nextInt(500));
                for (Memory_stressTestCase29.AllocUnitwithVerifyCleaner unit : units) {
                    unit.CanbeFree();
                }
                units.clear();
            }, "testCase29_alloc_" + (i + 1)));
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


    public static class StopAbleThread extends Thread {
        public static boolean mRuning = true;
        static List<List<AllocUnit>> mAllThread = new ArrayList<List<AllocUnit>>();

        public StopAbleThread(Runnable r, String name) {
            super(r, name);


        }
    }

    public static class AllocUnit {
        public byte unit[];

        public AllocUnit(int arrayLength) {
            unit = new byte[arrayLength];
        }
    }

    public static class AllocUnitwithVerifyCleaner extends AllocUnit {
        static final int FLAG_INUSE = 0x112233;
        static final int FLAG_CANBECLEAN = 0xddee;
        static final int FLAG_CLEARED = 0xaabb;
        public CleanerStatus status;

        public AllocUnitwithVerifyCleaner(int arrayLength) {
            super(arrayLength);
            status = new CleanerStatus();
            this.cleaner = Cleaner.create(this, new TestCleaner(status));
        }

        private final Cleaner cleaner;

        private static class CleanerStatus {
            public int status;

            CleanerStatus() {
                status = FLAG_INUSE;
            }
        }

        public void CanbeFree() {
            if (this.status.status != FLAG_INUSE) {
                throw new NullPointerException();
            }
            this.status.status = FLAG_CANBECLEAN;
        }

        private static class TestCleaner implements Runnable {
            public CleanerStatus status;

            public TestCleaner(CleanerStatus status) {
                this.status = status;
            }

            public void run() {
                if (this.status.status != FLAG_CANBECLEAN) {
                }
                this.status.status = FLAG_CLEARED;
            }
        }

    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult
