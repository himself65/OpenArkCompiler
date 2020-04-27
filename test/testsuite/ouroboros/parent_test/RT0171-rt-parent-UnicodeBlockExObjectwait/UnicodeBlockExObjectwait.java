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
 * -@TestCaseID: UnicodeBlockExObjectwait.java
 * -@TestCaseName: Exception in Character/UnicodeBlock: final void wait(*)
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:Test UnicodeBlock api wait extends from Object
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: UnicodeBlockExObjectwait.java
 * -@ExecuteClass: UnicodeBlockExObjectwait
 * -@ExecuteArgs:
 */

import java.lang.Character;

public class UnicodeBlockExObjectwait {
    static int res = 99;
    private static Character.UnicodeBlock unb1 = null;

    public static void main(String argv[]) {
        for (int cp = 0; cp <= 10; ++cp) {
            unb1 = Character.UnicodeBlock.of(cp);
        }
        System.out.println(new UnicodeBlockExObjectwait().run());
    }

    private class UnicodeBlockExObjectwait11 implements Runnable {
        // final void wait()

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (unb1) {
                unb1.notifyAll();
                try {
                    unb1.wait();
                    UnicodeBlockExObjectwait.res = UnicodeBlockExObjectwait.res - 15;
                } catch (InterruptedException e1) {
                    UnicodeBlockExObjectwait.res = UnicodeBlockExObjectwait.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    UnicodeBlockExObjectwait.res = UnicodeBlockExObjectwait.res - 10;
                }
            }
        }
    }

    private class UnicodeBlockExObjectwait21 implements Runnable {
        // final void wait(long millis)
        long millis = 10;

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (unb1) {
                unb1.notify();
                try {
                    unb1.wait(millis);
                    UnicodeBlockExObjectwait.res = UnicodeBlockExObjectwait.res - 15;
                } catch (InterruptedException e1) {
                    UnicodeBlockExObjectwait.res = UnicodeBlockExObjectwait.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    UnicodeBlockExObjectwait.res = UnicodeBlockExObjectwait.res - 10;
                } catch (IllegalArgumentException e3) {
                    UnicodeBlockExObjectwait.res = UnicodeBlockExObjectwait.res - 5;
                }
            }
        }
    }

    private class UnicodeBlockExObjectwait31 implements Runnable {
        // final void wait(long millis, int nanos)
        long millis = 10;
        int nanos = 10;

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (unb1) {
                unb1.notifyAll();
                try {
                    unb1.wait(millis, nanos);
                    UnicodeBlockExObjectwait.res = UnicodeBlockExObjectwait.res - 15;
                } catch (InterruptedException e1) {
                    UnicodeBlockExObjectwait.res = UnicodeBlockExObjectwait.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    UnicodeBlockExObjectwait.res = UnicodeBlockExObjectwait.res - 10;
                } catch (IllegalArgumentException e3) {
                    UnicodeBlockExObjectwait.res = UnicodeBlockExObjectwait.res - 5;
                }
            }
        }
    }

    /**
     * sleep fun
     *
     * @param slpnum wait time
     */
    public void sleep(int slpnum) {
        try {
            Thread.sleep(slpnum);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    /**
     * join fun
     * @param thread wait join thread
     */
    private void waitFinish(Thread thread) {
        try {
            thread.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    /**
     * main test fun
     *
     * @return status code
     */
    public int run() {
        int result = 2; /*STATUS_FAILED*/
        // check api normal
        // final void wait()
        Thread t1 = new Thread(new UnicodeBlockExObjectwait11());
        Thread t2 = new Thread(new UnicodeBlockExObjectwait11());
        // final void wait(long millis)
        Thread t3 = new Thread(new UnicodeBlockExObjectwait21());
        // final void wait(long millis, int nanos)
        Thread t5 = new Thread(new UnicodeBlockExObjectwait31());
        t1.setUncaughtExceptionHandler(new Thread.UncaughtExceptionHandler() {
            @Override
            public void uncaughtException(Thread t, Throwable e) {
                System.out.println(t.getName() + " : " + e.getMessage());
            }
        });
        t2.setUncaughtExceptionHandler(new Thread.UncaughtExceptionHandler() {
            @Override
            public void uncaughtException(Thread t, Throwable e) {
                System.out.println(t.getName() + " : " + e.getMessage());
            }
        });
        t3.setUncaughtExceptionHandler(new Thread.UncaughtExceptionHandler() {
            @Override
            public void uncaughtException(Thread t, Throwable e) {
                System.out.println(t.getName() + " : " + e.getMessage());
            }
        });
        t5.setUncaughtExceptionHandler(new Thread.UncaughtExceptionHandler() {
            @Override
            public void uncaughtException(Thread t, Throwable e) {
                System.out.println(t.getName() + " : " + e.getMessage());
            }
        });
        t1.start();
        sleep(1000);
        t3.start();
        sleep(1000);
        t5.start();
        sleep(1000);
        t2.start();
        sleep(1000);
        t1.interrupt();

        waitFinish(t1);
        waitFinish(t3);
        waitFinish(t5);

        if (result == 2 && UnicodeBlockExObjectwait.res == 54) {
            result = 0;
        }

        t2.interrupt();
        return result;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n