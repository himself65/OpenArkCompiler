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
 * -@TestCaseID: ThrowableExObjectwait.java
 * -@TestCaseName: Exception in Throwable: final void wait(*)
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:Test Throwable api wait extends from Object
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ThrowableExObjectwait.java
 * -@ExecuteClass: ThrowableExObjectwait
 * -@ExecuteArgs:
 */

import java.lang.Throwable;

public class ThrowableExObjectwait {
    static int res = 99;
    private Throwable cause = new Throwable("detailed message of cause");

    public static void main(String argv[]) {
        System.out.println(new ThrowableExObjectwait().run());
    }

    private class ThrowableExObjectwait11 implements Runnable {
        // final void wait()
        private int remainder;

        private ThrowableExObjectwait11(int remainder) {
            this.remainder = remainder;
        }

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (cause) {
                cause.notifyAll();
                try {
                    cause.wait();
                    ThrowableExObjectwait.res = ThrowableExObjectwait.res - 15;
                } catch (InterruptedException e1) {
                    ThrowableExObjectwait.res = ThrowableExObjectwait.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    ThrowableExObjectwait.res = ThrowableExObjectwait.res - 10;
                }
            }
        }
    }

    private class ThrowableExObjectwait21 implements Runnable {
        // final void wait(long millis)
        private int remainder;
        long millis = 10;

        private ThrowableExObjectwait21(int remainder) {
            this.remainder = remainder;
        }

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (cause) {
                cause.notify();
                try {
                    cause.wait(millis);
                    ThrowableExObjectwait.res = ThrowableExObjectwait.res - 15;
                } catch (InterruptedException e1) {
                    ThrowableExObjectwait.res = ThrowableExObjectwait.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    ThrowableExObjectwait.res = ThrowableExObjectwait.res - 10;
                } catch (IllegalArgumentException e3) {
                    ThrowableExObjectwait.res = ThrowableExObjectwait.res - 5;
                }
            }
        }
    }

    private class ThrowableExObjectwait31 implements Runnable {
        // final void wait(long millis, int nanos)

        private int remainder;
        long millis = 10;
        int nanos = 10;

        private ThrowableExObjectwait31(int remainder) {
            this.remainder = remainder;
        }

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (cause) {
                cause.notifyAll();
                try {
                    cause.wait(millis, nanos);
                    ThrowableExObjectwait.res = ThrowableExObjectwait.res - 15;
                } catch (InterruptedException e1) {
                    ThrowableExObjectwait.res = ThrowableExObjectwait.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    ThrowableExObjectwait.res = ThrowableExObjectwait.res - 10;
                } catch (IllegalArgumentException e3) {
                    ThrowableExObjectwait.res = ThrowableExObjectwait.res - 5;
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
        Thread t1 = new Thread(new ThrowableExObjectwait11(1));
        Thread t2 = new Thread(new ThrowableExObjectwait11(2));
        // final void wait(long millis)
        Thread t3 = new Thread(new ThrowableExObjectwait21(3));
        // final void wait(long millis, int nanos)
        Thread t5 = new Thread(new ThrowableExObjectwait31(5));
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
        t1.interrupt();

        waitFinish(t1);
        waitFinish(t3);
        waitFinish(t5);


        if (result == 2 && ThrowableExObjectwait.res == 54) {
            result = 0;
        }

        t2.interrupt();
        return result;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n