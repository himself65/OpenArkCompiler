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
 * -@TestCaseID: MonitorTestCase8.java
 *- @TestCaseName: Thread_MonitorTestCase8.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Added according to stress cases SyscompTestCase/Maple_harness/tools/testapp_MemStressTest/src
 *                      /com/huawei/stress.
 *- @Brief: see below
 * -#step1: 创建一个static的类StopAbleThread继承Thread，类的构造方法使用了父函数的构造方法。
 * -#step2: 分配DEFAULT_THREAD_NUM_HALF / 2个对象来自于new StopAbleThread类，同时在new的参数1里定义new CommonRun监控线程。
 * -#step3: 调用start()启动所有的对象线程。
 * -#step4: 调用join()等待线程运行结束。
 * -#step5: 调用Runtime.getRuntime().gc()回收资源。
 * -#step6: 重复步骤1~4。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: MonitorTestCase8.java
 *- @ExecuteClass: MonitorTestCase8
 *- @ExecuteArgs:
 */

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

public class MonitorTestCase8 {
    private static final int INIT_DEFAULT_THREAD_NUM = 10;
    private static int DEFAULT_THREAD_NUM_HALF = INIT_DEFAULT_THREAD_NUM / 2;
    private static final int THREAD_REPEATS_DEFAULT = 1;
    private static final int THREAD_REPEATS_INFINITE = -1;
    private static boolean mRunning = true;
    private static boolean mRest = false;
    private static int mRestTime = 500;
    static List<Thread> mAllThread = new ArrayList<>();

    public static void main(String[] args) {
        testCase8();
        Runtime.getRuntime().gc();
        testCase8();
        System.out.println("0");
    }

    public static void testCase8() {
        List<Thread> list = new ArrayList<>();
        List<String> stringList = new ArrayList<>();
        for (int i = 0; i < (DEFAULT_THREAD_NUM_HALF / 2); i++) {
            int index = i;
            stringList.add("");
            list.add(new StopAbleThread(new CommonRun(() -> {
                ArrayList<String> strings = new ArrayList<>(100);
                for (int j = 0; j < 100; j++) {
                    strings.add(StringMethod.getStringRandom());
                }
                for (String str : strings) {
                    stringList.set(index, stringList.get(index) + str);
                }
            }, 100, THREAD_REPEATS_DEFAULT), "testCase8_" + (i + 1)));
        }
        startAllThread(list);
        waitAllThreadFinish(list);
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

    static class StopAbleThread extends Thread {
        public StopAbleThread(Runnable r, String name) {
            super(r, name);
        }
    }

    static class CommonRun implements Runnable {
        int sleepTime;
        int repeats;
        Runnable cbFun;

        public CommonRun(Runnable cb, int sleepTime, int repeatTimes) {
            this.sleepTime = sleepTime;
            repeats = repeatTimes;
            cbFun = cb;
        }

        @Override
        public void run() {
            while (THREAD_REPEATS_INFINITE == repeats || repeats > 0) {
                if (repeats > 0) {
                    repeats--;
                }
                tryRest();
                if (cbFun != null) {
                    cbFun.run();
                }
                trySleep(sleepTime);
                if (!mRunning) {
                    break;
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

    private enum StringMethod {
        METHOD0("DirectString") {
            @Override
            public String apply() {
                return "DirectString";
            }
        },
        METHOD1("ReferenceString") {
            @Override
            public String apply() {
                return this.name;
            }
        },
        METHOD2("FinalString") {
            @Override
            public String apply() {
                return FINAL_STRING;
            }
        },
        METHOD3("ConcatString") {
            @Override
            public String apply() {
                return "Concat".concat("String");
            }
        },
        METHOD4("StringAddition") {
            @Override
            public String apply() {
                return "String" + "Addition" + 1;
            }
        },
        METHOD5("CopyValueOf") {
            @Override
            public String apply() {
                char[] tmp = {'C', 'o', 'p', 'y', 'V', 'a', 'l', 'u', 'e', 'O', 'f'};
                return String.copyValueOf(tmp);
            }
        },
        METHOD6("Intern") {
            @Override
            public String apply() {
                return mRandom.toString().intern();
            }
        },
        METHOD7("Integer") {
            @Override
            public String apply() {
                return Integer.toString(mRandom.nextInt());
            }
        },
        METHOD8("Float") {
            @Override
            public String apply() {
                return Float.toString(mRandom.nextFloat());
            }
        },
        METHOD9("Double") {
            @Override
            public String apply() {
                return Double.toString(mRandom.nextDouble());
            }
        },
        METHOD10("Boolean") {
            @Override
            public String apply() {
                return Boolean.toString(mRandom.nextBoolean());
            }
        },
        METHOD11("ReplaceChar") {
            @Override
            public String apply() {
                return mRandom.toString().replace('R', 'r');
            }
        },
        METHOD12("ReplaceString") {
            @Override
            public String apply() {
                return mRandom.toString().replace("Ran", "RED");
            }
        },
        METHOD13("Split") {
            @Override
            public String apply() {
                return mRandom.toString().split("@")[0];
            }
        },
        METHOD14("SubString") {
            @Override
            public String apply() {
                return mRandom.toString().substring(3, 5);
            }
        },
        METHOD15("ToLowerCase") {
            @Override
            public String apply() {
                return mRandom.toString().toLowerCase();
            }
        },
        METHOD16("ToUpperCase") {
            @Override
            public String apply() {
                return mRandom.toString().toUpperCase();
            }
        },
        METHOD17("StringBuffer") {
            @Override
            public String apply() {
                return new StringBuffer("String").append("Buffer").append(1).toString();
            }
        },
        METHOD18("StringBuilder") {
            @Override
            public String apply() {
                return new StringBuilder(20).append(mRandom).toString();
            }
        },
        METHOD19("subSequence") {
            @Override
            public String apply() {
                return mRandom.toString().subSequence(1, 3).toString();
            }
        };

        public final String name;
        private static final String FINAL_STRING = "FinalString";
        private static final Random mRandom = new Random();

        StringMethod(String name) {
            this.name = name;
        }

        public String apply() {
            return name;
        }

        public static String getString(int id) {
            if ((id < 0) || (id >= StringMethod.values().length)) {
                return "";
            } else {
                return StringMethod.values()[id].apply();
            }
        }

        public static String getStringRandom() {
            return getString(mRandom.nextInt(StringMethod.values().length));
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n