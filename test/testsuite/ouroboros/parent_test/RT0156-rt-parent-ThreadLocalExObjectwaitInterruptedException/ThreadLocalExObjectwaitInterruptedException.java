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
 * -@TestCaseID: ThreadLocalExObjectwaitInterruptedException.java
 * -@TestCaseName: Exception in ThreadLocal: final void wait(*)
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1:Test ThreadLocal api wait extends from Object
 * -#step2:Thread is interruptted when wait
 * -#step3:Throw InterruptedException
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ThreadLocalExObjectwaitInterruptedException.java
 * -@ExecuteClass: ThreadLocalExObjectwaitInterruptedException
 * -@ExecuteArgs:
 */

import java.lang.Thread;

public class ThreadLocalExObjectwaitInterruptedException {
    static int res = 99;
    static ThreadLocal<Object> tl1 = new ThreadLocal<>();

    public static void main(String argv[]) {
        System.out.println(new ThreadLocalExObjectwaitInterruptedException().run());
    }

    private class ThreadLocalExObjectwaitInterruptedException11 implements Runnable {
        // final void wait()
        private int remainder;

        private ThreadLocalExObjectwaitInterruptedException11(int remainder) {
            this.remainder = remainder;
        }

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (tl1) {
                tl1.notifyAll();
                try {
                    tl1.wait();
                    ThreadLocalExObjectwaitInterruptedException.res = ThreadLocalExObjectwaitInterruptedException.res - 15;
                } catch (InterruptedException e1) {
                    ThreadLocalExObjectwaitInterruptedException.res = ThreadLocalExObjectwaitInterruptedException.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    ThreadLocalExObjectwaitInterruptedException.res = ThreadLocalExObjectwaitInterruptedException.res - 10;
                }
            }
        }
    }

    private class ThreadLocalExObjectwaitInterruptedException21 implements Runnable {
        // final void wait(long millis)
        private int remainder;
        long millis = 10000;

        private ThreadLocalExObjectwaitInterruptedException21(int remainder) {
            this.remainder = remainder;
        }

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (tl1) {
                tl1.notifyAll();
                try {
                    tl1.wait(millis);
                    ThreadLocalExObjectwaitInterruptedException.res = ThreadLocalExObjectwaitInterruptedException.res - 15;
                } catch (InterruptedException e1) {
                    ThreadLocalExObjectwaitInterruptedException.res = ThreadLocalExObjectwaitInterruptedException.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    ThreadLocalExObjectwaitInterruptedException.res = ThreadLocalExObjectwaitInterruptedException.res - 10;
                } catch (IllegalArgumentException e3) {
                    ThreadLocalExObjectwaitInterruptedException.res = ThreadLocalExObjectwaitInterruptedException.res - 5;
                }
            }
        }
    }

    private class ThreadLocalExObjectwaitInterruptedException31 implements Runnable {
        // final void wait(long millis, int nanos)
        private int remainder;
        long millis = 10000;
        int nanos = 100;

        private ThreadLocalExObjectwaitInterruptedException31(int remainder) {
            this.remainder = remainder;
        }

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (tl1) {
                tl1.notifyAll();
                try {
                    tl1.wait(millis, nanos);
                    ThreadLocalExObjectwaitInterruptedException.res = ThreadLocalExObjectwaitInterruptedException.res - 15;
                } catch (InterruptedException e1) {
                    ThreadLocalExObjectwaitInterruptedException.res = ThreadLocalExObjectwaitInterruptedException.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    ThreadLocalExObjectwaitInterruptedException.res = ThreadLocalExObjectwaitInterruptedException.res - 10;
                } catch (IllegalArgumentException e3) {
                    ThreadLocalExObjectwaitInterruptedException.res = ThreadLocalExObjectwaitInterruptedException.res - 5;
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
     * main test fun
     *
     * @return status code
     */
    public int run() {
        int result = 2; /*STATUS_FAILED*/
        // check api normal
        // final void wait()
        Thread t1 = new Thread(new ThreadLocalExObjectwaitInterruptedException11(1));
        // final void wait(long millis)
        Thread t3 = new Thread(new ThreadLocalExObjectwaitInterruptedException21(3));
        // final void wait(long millis, int nanos)
        Thread t5 = new Thread(new ThreadLocalExObjectwaitInterruptedException31(5));
        t1.setUncaughtExceptionHandler(new Thread.UncaughtExceptionHandler() {
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
        sleep(100);
        t1.interrupt();
        sleep(100);
        t3.start();
        sleep(100);
        t3.interrupt();
        sleep(100);
        t5.start();
        sleep(100);
        t5.interrupt();
        sleep(100);

        if (result == 2 && ThreadLocalExObjectwaitInterruptedException.res == 96) {
            result = 0;
        }

        return result;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n