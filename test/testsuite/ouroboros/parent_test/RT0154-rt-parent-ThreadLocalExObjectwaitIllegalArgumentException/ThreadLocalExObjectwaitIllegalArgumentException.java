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
 * -@TestCaseID: ThreadLocalExObjectwaitIllegalArgumentException.java
 * -@TestCaseName: Exception in ThreadLocal: final void wait(*)
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1:Test ThreadLocal api wait extends from Object
 * -#step2:Give the value of timeout is negative or the value of nanos is not in the range 0-999999
 * -#step3:Throw IllegalArgumentException
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ThreadLocalExObjectwaitIllegalArgumentException.java
 * -@ExecuteClass: ThreadLocalExObjectwaitIllegalArgumentException
 * -@ExecuteArgs:
 */

import java.lang.Thread;

public class ThreadLocalExObjectwaitIllegalArgumentException {
    static int res = 99;
    ThreadLocal<Object> tl1 = new ThreadLocal<Object>();

    public static void main(String argv[]) {
        System.out.println(new ThreadLocalExObjectwaitIllegalArgumentException().run());
    }

    private class ThreadLocalExObjectwaitIllegalArgumentException21 implements Runnable {
        // IllegalArgumentException - if the value of timeout is negative.
        // final void wait(long millis)
        private int remainder;
        long millis = -1;

        private ThreadLocalExObjectwaitIllegalArgumentException21(int remainder) {
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
                    ThreadLocalExObjectwaitIllegalArgumentException.res = ThreadLocalExObjectwaitIllegalArgumentException.res - 15;
                } catch (InterruptedException e1) {
                    ThreadLocalExObjectwaitIllegalArgumentException.res = ThreadLocalExObjectwaitIllegalArgumentException.res - 20;
                } catch (IllegalMonitorStateException e2) {
                    ThreadLocalExObjectwaitIllegalArgumentException.res = ThreadLocalExObjectwaitIllegalArgumentException.res - 10;
                } catch (IllegalArgumentException e3) {
                    ThreadLocalExObjectwaitIllegalArgumentException.res = ThreadLocalExObjectwaitIllegalArgumentException.res - 5;
                }
            }
        }
    }

    private class ThreadLocalExObjectwaitIllegalArgumentException31 implements Runnable {
        // IllegalArgumentException - if the value of timeout is negative or the value of nanos is not in the range 0-999999.
        //
        // final void wait(long millis, int nanos)

        private int remainder;
        long millis = -2;
        int nanos = 10;

        private ThreadLocalExObjectwaitIllegalArgumentException31(int remainder) {
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
                    ThreadLocalExObjectwaitIllegalArgumentException.res = ThreadLocalExObjectwaitIllegalArgumentException.res - 15;
                } catch (InterruptedException e1) {
                    ThreadLocalExObjectwaitIllegalArgumentException.res = ThreadLocalExObjectwaitIllegalArgumentException.res - 20;
                } catch (IllegalMonitorStateException e2) {
                    ThreadLocalExObjectwaitIllegalArgumentException.res = ThreadLocalExObjectwaitIllegalArgumentException.res - 10;
                } catch (IllegalArgumentException e3) {
                    ThreadLocalExObjectwaitIllegalArgumentException.res = ThreadLocalExObjectwaitIllegalArgumentException.res - 5;
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
        // final void wait(long millis)
        Thread t3 = new Thread(new ThreadLocalExObjectwaitIllegalArgumentException21(3));
        // final void wait(long millis, int nanos)
        Thread t5 = new Thread(new ThreadLocalExObjectwaitIllegalArgumentException31(5));
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
        t3.start();
        t5.start();
        sleep(1000);
        if (result == 2 && ThreadLocalExObjectwaitIllegalArgumentException.res == 89) {
            result = 0;
        }

        return result;
    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n